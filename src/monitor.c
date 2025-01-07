#include <X11/cursorfont.h>
/* keycodes */
#include <X11/keysym.h>

#include "monitor.h"
#include "bar.h"
#include "main.h"

#include <string.h>

extern WM _wm;
extern XCBCursor cursors[];
extern XCBAtom netatom[];


void
arrangemon(Monitor *m)
{
    Desktop *desk;
    for(desk = m->desktops; desk; desk = nextdesktop(desk))
    {   arrange(desk);
    }
}

void
arrangemons(void)
{
    Monitor *m;
    for(m = _wm.mons; m; m = nextmonitor(m))
    {   arrangemon(m);
    }
}

/* NOT RECOMMENDED AS REQUIRES MANUAL SETTING OF DESKTOP->NUM */
void
attachdesktop(Monitor *m, Desktop *desktop)
{
    desktop->mon = m;
    desktop->num = 0;
    /* can use attach helper here */
    desktop->next = m->desktops;
    m->desktops = desktop;
    if(desktop->next)
    {   desktop->next->prev = desktop;
    }
    else
    {   m->desklast = desktop;
    }
    desktop->prev = NULL;
}

void
attachdesktoplast(Monitor *m, Desktop *desk)
{
    if(!m->desktops)
    {   
        attachdesktop(m, desk);
        return;
    }

    desk->mon = m;
    desk->next = NULL;
    desk->prev = m->desklast;
    m->desklast->next = desk;
    m->desklast = desk;
    desk->num = m->deskcount++;
}

void
detachdesktop(Monitor *m, Desktop *desktop)
{
    Desktop **td;
    for(td = &m->desktops; *td && *td != desktop; td = &(*td)->next);
    *td = desktop->next;
    if(!(*td))
    {
        m->desklast = desktop->prev;
    }
    else if(desktop->next)
    {
        desktop->next->prev = desktop->prev;
    }
    else if(desktop->prev)
    {
        m->desklast = desktop->prev;
        desktop->prev->next = NULL;
    }
    --m->deskcount;

    desktop->next = NULL;
    desktop->prev = NULL;
    desktop->mon = NULL;
}

void
cleanupcursors(void)
{
    int i;
    for(i = 0; i < CurLast; ++i) 
    {   XCBFreeCursor(_wm.dpy, cursors[i]); 
    }
}

void
cleanupmon(Monitor *m)
{
    Desktop *desk = NULL;
    Desktop *desknext = NULL;
    desk = m->desktops;
    while(desk)
    {
        desknext = desk->next;
        cleanupdesktop(desk);
        desk = desknext;
    }
    if(m->bar)
    {   cleanupclient(m->bar);
    }
    free(m);
    m = NULL;
}

void
cleanupmons(void)
{
    Monitor *m = NULL;
    Monitor *mnext = NULL;
    m = _wm.mons;

    while(m)
    {   
        mnext = m->next;
        cleanupmon(m);
        m = mnext;
    }
}

Monitor *
createmon(void)
{
    Monitor *m = calloc(1, sizeof(Monitor ));
    if(!m)
    {   /* while calling "DIE" may seem extreme frankly we cannot afford a monitor to fail alloc. */
        DIE("%s", "(OutOfMemory) Could not alloc enough memory for a Monitor");
        return NULL;
    }
    m->mx = m->my = 0;
    m->mw = m->mh = 0;
    m->wx = m->wy = 0;
    m->ww = m->wh = 0;
    m->next = NULL;
    /* FIXME: for some reason this cant be 0 because then we would get 2 "0" desktop nums */
    m->deskcount = 1;
    setdesktopcount(m, 10);
    m->desksel = m->desktops;
    m->bar = NULL;
    return m;
}

Monitor *
dirtomon(u8 dir)
{
    Monitor *m = NULL;

    if(dir > 0)
    {   if(!(m = _wm.selmon->next)) m = _wm.mons;
    }
    else if (_wm.selmon == _wm.mons)
    {
        for(m = _wm.mons; m->next; m = nextmonitor(m));
    }
    else
    {   for(m = _wm.mons; m->next != _wm.selmon; m = nextmonitor(m));
    }
    return m;
}

Monitor *
recttomon(i16 x, i16 y, u16 w, u16 h)
{
	Monitor *m, *r = _wm.selmon;
	i32 a, area = 0;

	for (m = _wm.mons; m; m = m->next)
		if ((a = INTERSECT(x, y, w, h, m)) > area) {
			area = a;
			r = m;
		}
	return r;
}

Monitor *
nextmonitor(Monitor *m)
{
    return m ? m->next : m;
}

void 
setdesktopcount(Monitor *m, uint16_t desktops)
{
    const u8 MIN_DESKTOPS = 1;
    if(desktops <= MIN_DESKTOPS)
    {   Debug0("Cannot make desktop count less than possible.");
        return;
    }
    if(m->deskcount == desktops)
    {
        Debug("Desktops are already at specified capacity: [%u]", desktops);
        return;
    }
    u16 i;
    Desktop *desk = m->desklast;
    if(m->deskcount > desktops)
    {
        Client *c;
        Desktop *tmp = NULL;
        for(i = desktops; i > m->deskcount; --i)
        {
            if(desk && prevdesktop(desk))
            {   
                for(c = startclient(desk); c; c = nextclient(c))
                {   
                    setclientdesktop(c, prevdesktop(desk));
                } 
                tmp = prevdesktop(desk);
                detachdesktop(m, desk);
                cleanupdesktop(desk);
            }
            desk = tmp;
        }
    }
    else
    {
        u8 failurecount = 0;
        for(i = 0; i < desktops && failurecount < 10; ++i)
        {
            desk = createdesktop();
            if(desk)
            {   attachdesktoplast(m, desk);
            }
            else
            {   
                i--;
                ++failurecount;
            }
        }
        if(failurecount)
        {   Debug("Failed [%d]", failurecount);
        }
    }
}

void 
setdesktopsel(Monitor *mon, Desktop *desksel)
{
    if(desksel->mon != mon)
    {   /* TODO maybe add functionality to detach desktop or something? */
        Debug0("Cant set desktop of different monitor, FIXME");
        return;
    }
    if(mon->desksel != desksel)
    {
        mon->desksel = desksel;
        Desktop *desk;
        Client *c;
        for(c = startstack(desksel); c; c = nextstack(c))
        {   showhide(c);
        }
        for(desk = mon->desktops; desk; desk = nextdesktop(desk))
        {
            for(c = laststack(desk); c; c = prevstack(c))
            {   
                if(ISSTICKY(c))
                {   setclientdesktop(c, desksel);
                }
                if(desk != desksel)
                {   showhide(c);
                }
            }
        }
        updatedesktop();
    }
    else
    {   Debug0("Same desktop, no change.");
    }
}

void 
setmonsel(Monitor *m)
{
    if(_wm.selmon == m)
    {   return;
    }

    if(m->desksel->sel)
    {   unfocus(m->desksel->sel, 1);
    }

    _wm.selmon = m;

    focus(NULL);

    updatedesktopnum();
    updatedesktopnames();
    updatedesktopviewport();
    updatedesktopworkarea();
    updatedesktop();

    Debug0("Changed Monitor.");
}

void
setupcursors(void)
{
    cursors[CurNormal] = XCBCreateFontCursor(_wm.dpy, XC_left_ptr);
    cursors[CurResizeTopL] = XCBCreateFontCursor(_wm.dpy, XC_top_left_corner);
    cursors[CurResizeTopR] = XCBCreateFontCursor(_wm.dpy, XC_top_right_corner);
    cursors[CurMove] = XCBCreateFontCursor(_wm.dpy, XC_fleur);
}

#ifdef XINERAMA
static int
isuniquegeom(XCBXineramaScreenInfo *unique, size_t n, XCBXineramaScreenInfo *info)
{
    while(n--)
    {   if(unique[n].x_org == info->x_org && unique[n].y_org == info->y_org && unique[n].width == info->width && unique[n].height == info->height)
        {   return 0;
        }
    }
    return 1;
}
#endif
int
updategeom(void)
{
	int dirty = 0;

#ifdef XINERAMA
    int xienabled = 0;
    int xiactive = 0;
    XCBQueryExtension *extrep = NULL;
    XCBXineramaIsActive *xia = NULL;

    /* check if we even have the extension enabled */
    extrep = (XCBQueryExtension *)xcb_get_extension_data(_wm.dpy, &xcb_xinerama_id);
    xienabled = (extrep && !extrep->present);

    if(xienabled)
    {
        xcb_xinerama_is_active_cookie_t xcookie = xcb_xinerama_is_active(_wm.dpy);
        /* let event handler handle a Xinerama error */
        xia = xcb_xinerama_is_active_reply(_wm.dpy, xcookie, NULL);
        xiactive = xia && xia->state;
    }
    /* assume no error and proceed */
    if(xiactive)
    {
        int i, j, n, nn;
        Client *c = NULL;
        Monitor *m = NULL;
        XCBGenericError *err = NULL;
        XCBXineramaQueryScreens *xsq = NULL;
        XCBXineramaScreenInfo *info = NULL, *unique = NULL;

        xsq = xcb_xinerama_query_screens_reply(_wm.dpy, xcb_xinerama_query_screens_unchecked(_wm.dpy), &err);
        if(!xsq || err)
        {
            /* were fucked */
            DIECAT("%s", "Xinerama is broken, contact a developer to fix this issue");
        }


        info = xcb_xinerama_query_screens_screen_info(xsq);
        nn = xcb_xinerama_query_screens_screen_info_length(xsq);


        for(n = 0, m = _wm.mons; m; m = m->next, ++n);
		/* only consider unique geometries as separate screens */
        unique = calloc(nn, sizeof(xcb_xinerama_query_screens_reply_t));
        if(!unique)
        {   return dirty;
        }
        for(i = 0, j = 0; i < nn; ++i)
        {   if(isuniquegeom(unique, j, &info[i]))
            {   memcpy(&unique[j++], &info[i], sizeof(xcb_xinerama_screen_info_t));
            }
        }

        free(xsq);
        nn = j;

		/* new monitors if nn > n */
		for (i = n; i < nn; ++i)
        {
			for (m = _wm.mons; m && m->next; m = m->next);
			if (m)
            {   m->next = createmon();
            }
			else
            {   _wm.mons = createmon();
            }
		}
		for (i = 0, m = _wm.mons; i < nn && m; m = m->next, ++i)
			if (i >= n
			|| unique[i].x_org != m->mx || unique[i].y_org != m->my
			|| unique[i].width != m->mw || unique[i].height != m->mh)
			{
				dirty = 1;
				m->mx = m->wx = unique[i].x_org;
				m->my = m->wy = unique[i].y_org;
				m->mw = m->ww = unique[i].width;
				m->mh = m->wh = unique[i].height;
                /* we should update the bar position if we have one */
                updatebarpos(m);
			}
		/* removed monitors if n > nn */
		for (i = nn; i < n; ++i)
        {
			for (m = _wm.mons; m && m->next; m = m->next);
			while ((c = m->desktops->clients)) 
            {
				dirty = 1;
				m->desktops->clients = c->next;
				detachstack(c);
                detachfocus(c);
                c->desktop = _wm.mons->desktops;
                /* TODO desktops might break. */
				attach(c);
				attachstack(c);
                attachfocus(c);
			}
			if (m == _wm.selmon)
            {   _wm.selmon = _wm.mons;
            }
			cleanupmon(m);
		}
		free(unique);
	} else
#endif /* XINERAMA */
	{  /* default monitor setup */
		if (!_wm.mons)
        {   _wm.mons = createmon();
        }
		if (_wm.mons->mw != _wm.sw || _wm.mons->mh != _wm.sh) 
        {
			dirty = 1;
			_wm.mons->mw = _wm.mons->ww = _wm.sw;
			_wm.mons->mh = _wm.mons->wh = _wm.sh;
            /* we should update the bar position if we have one */
            updatebarpos(_wm.mons);
		}
	}
	if (dirty) 
    {
		_wm.selmon = _wm.mons;
		_wm.selmon = wintomon(_wm.root);
	}
	return dirty;
}

void
updateclientlist(XCBWindow win, uint8_t type)
{
    switch(type)
    {
        case ClientListAdd:
            /* This allows for restart() to keep windows mapped on exit, basically it reduces flicker greatly. */
            XCBAddToSaveSet(_wm.dpy, win);
            break;
        case ClientListRemove:
            /* Destroyed windows are auto removed from saveset */
            if(wintoclient(win))
            {   XCBRemoveFromSaveSet(_wm.dpy, win);
            }
        case ClientListReload:
            break;
    }

    Monitor *m;
    Desktop *desk;
    Client *c;
    
    XCBWindow winlist[X11_DEFAULT_MAX_WINDOW_LIMIT];
    i32 i = 0;
    u32 first = 1;

    XCBDeleteProperty(_wm.dpy, _wm.root, netatom[NetClientList]);
    for(m = _wm.mons; m; m = nextmonitor(m))
    {
        for(desk = m->desktops; desk; desk = nextdesktop(desk))
        {
            for(c = startclient(desk); c; c = nextclient(c))
            {   
                if(first)
                {
                    XCBChangeProperty(_wm.dpy, _wm.root, netatom[NetClientList], 
                            XCB_ATOM_WINDOW, 32, XCB_PROP_MODE_REPLACE, (unsigned char *)&c->win, 1);
                    first = 0;
                }
                else
                {
                    winlist[i++] = c->win;
                    if(i == X11_DEFAULT_MAX_WINDOW_LIMIT)
                    {   
                        XCBChangeProperty(_wm.dpy, _wm.root, netatom[NetClientList], 
                                XCB_ATOM_WINDOW, 32, XCB_PROP_MODE_APPEND, (unsigned char *)winlist, i);
                        i = 0;
                    }
                }
            }
        }
    }
    if(i)
    {    
        XCBChangeProperty(_wm.dpy, _wm.root, netatom[NetClientList], 
                XCB_ATOM_WINDOW, 32, XCB_PROP_MODE_APPEND, (unsigned char *)winlist, i);
    }
}
/* this function is really slow, slower than malloc use only in startup or rare mapping changes */
void
updatenumlockmask(void)
{
    XCBKeyboardModifier *reply;
    XCBGenericError *err;

    reply = xcb_get_modifier_mapping_reply(_wm.dpy, xcb_get_modifier_mapping(_wm.dpy), &err);
    if(err)
    {   free(reply);
        free(err);
        return;
    }

	xcb_keycode_t *codes = xcb_get_modifier_mapping_keycodes(reply);
	xcb_keycode_t target, *temp = NULL;
	unsigned int i, j;

    if(!(temp = xcb_key_symbols_get_keycode(_wm.syms, XK_Num_Lock)))
    {   free(reply);
        return;
    }

	target = *temp;
	free(temp);

	for(i = 0; i < 8; i++)
    {
		for(j = 0; j < reply->keycodes_per_modifier; j++)
        {   
            if(codes[i * reply->keycodes_per_modifier + j] == target)
            {   _wm.numlockmask = (1 << i);
            }
        }
    }
    free(reply);
}

Monitor *
wintomon(XCBWindow win)
{
    i16 x, y;
    Client *c;
    Monitor *m;
    if(win == _wm.root && getrootptr(&x, &y)) 
    {   return recttomon(x, y, 1, 1);
    }
    for (m = _wm.mons; m; m = m->next)
    {   
        if (m->bar && win == m->bar->win) 
        {   return m;
        }
    }
    if ((c = wintoclient(win))) 
    {   return c->desktop->mon;
    }
    return _wm.selmon;
}
