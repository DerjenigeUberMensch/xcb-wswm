
#include "client.h"
#include "monitor.h"
#include "desktop.h"
#include "bar.h"
#include "main.h"

extern WM _wm;
extern UserSettings _cfg;
extern XCBAtom netatom[];


void 
arrangeq(Desktop *desk)
{
    reorder(desk);
    restack(desk);
}

void __HOT__
arrange(Desktop *desk)
{
    /* bar stuff */
    updatebargeom(desk->mon);
    updatebarpos(desk->mon);

    reorder(desk);
    arrangedesktop(desk);
    restack(desk);
}

void
arrangedesktop(Desktop *desk)
{   
    if(layouts[desk->layout].arrange)
    {   layouts[desk->layout].arrange(desk);
    }
}


/* Macro helper */
#define __attach_helper(STRUCT, HEAD, NEXT, PREV, LAST)     do                                      \
                                                            {                                       \
                                                                STRUCT->NEXT = STRUCT->HEAD;        \
                                                                STRUCT->HEAD = STRUCT;              \
                                                                if(STRUCT->NEXT)                    \
                                                                {   STRUCT->NEXT->PREV = STRUCT;    \
                                                                }                                   \
                                                                else                                \
                                                                {   STRUCT->LAST = STRUCT;          \
                                                                }                                   \
                                                                /* prevent circular linked list */  \
                                                                STRUCT->PREV = NULL;                \
                                                            } while(0)

#define __attach_after(START, AFTER, NEXT, PREV, HEAD, LAST)        do                                  \
                                                                    {                                   \
                                                                        AFTER->NEXT = START->NEXT;      \
                                                                        AFTER->PREV = START;            \
                                                                        if(!START->NEXT)                \
                                                                        {   LAST = START;               \
                                                                        }                               \
                                                                        else                            \
                                                                        {   START->NEXT->PREV = AFTER;  \
                                                                        }                               \
                                                                        START->NEXT = AFTER;            \
                                                                    } while(0)
#define __attach_before(START, BEFORE, NEXT, PREV, HEAD, LAST, ATTACH)  do                                  \
                                                                        {                                   \
                                                                            if(!START->PREV)                \
                                                                            {   ATTACH(BEFORE);             \
                                                                            }                               \
                                                                            else                            \
                                                                            {                               \
                                                                                BEFORE->PREV = START->PREV; \
                                                                                BEFORE->NEXT = START;       \
                                                                                START->PREV->NEXT = BEFORE; \
                                                                                START->PREV = BEFORE;       \
                                                                            }                               \
                                                                        } while(0)

#define __detach_helper(NAME, TYPE, STRUCT, HEAD, NEXT, PREV, LAST)   do                                                        \
                                                                {                                                               \
                                                                    TYPE **tc;                                                  \
                                                                    for(tc = &HEAD; *tc && *tc != STRUCT; tc = &(*tc)->NEXT);   \
                                                                    *tc = STRUCT->NEXT;                                         \
                                                                    if(!(*tc))                                                  \
                                                                    {   LAST = STRUCT->PREV;                                    \
                                                                    }                                                           \
                                                                    else if(STRUCT->NEXT)                                       \
                                                                    {   STRUCT->NEXT->PREV = STRUCT->PREV;                      \
                                                                    }                                                           \
                                                                    else if(STRUCT->PREV)                                       \
                                                                    {                                                           \
                                                                        LAST = STRUCT->PREV;                                    \
                                                                        STRUCT->PREV->NEXT = NULL;                              \
                                                                    }                                                           \
                                                                    STRUCT->NEXT = NULL;                                        \
                                                                    STRUCT->PREV = NULL;                                        \
                                                                } while(0)
void
attach(Client *c)
{
    if(ISOVERRIDEREDIRECT(c))
    {   return;
    }
    __attach_helper(c, desktop->clients, next, prev, desktop->clast);
}

void
attachstack(Client *c)
{
    if(ISOVERRIDEREDIRECT(c))
    {   return;
    }
    __attach_helper(c, desktop->stack, snext, sprev, desktop->slast);
}

void
attachrestack(Client *c)
{
    if(ISOVERRIDEREDIRECT(c))
    {   return;
    }
    __attach_helper(c, desktop->rstack, rnext, rprev, desktop->rlast);
}

void
attachfocus(Client *c)
{
    if(ISOVERRIDEREDIRECT(c))
    {   return;
    }
    __attach_helper(c, desktop->focus, fnext, fprev, desktop->flast);
}

void
attachfocusafter(Client *start, Client *after)
{
    if(ISOVERRIDEREDIRECT(after))
    {   return;
    }
    Desktop *desk = start->desktop;
    detachfocus(after);
    __attach_after(start, after, fnext, fprev, desk->focus, desk->slast);
}

void
attachfocusbefore(Client *start, Client *after)
{
    if(ISOVERRIDEREDIRECT(after))
    {   return;
    }
    Desktop *desk = start->desktop;
    detachfocus(after);
    __attach_before(start, after, fnext, fprev, desk->focus, desk->slast, attachfocus);
    /* block 'unused' variable warnings */
    (void)desk;
}

void
detach(Client *c)
{
    __detach_helper(client, Client, c, c->desktop->clients, next, prev, c->desktop->clast);
}

void
detachcompletely(Client *c)
{
    Desktop *desk = c->desktop;
    if(desk)
    {
        Monitor *m = desk->mon;
        if(m->bar == c)
        {   
            m->bar = NULL;
            Debug0("Detaching bar? Potential memory leak");
        }
        if(desk->sel == c)
        {   desk->sel = NULL;
        }
    }
    detach(c);
    detachstack(c);
    detachfocus(c);
    detachrestack(c);
}

void
detachstack(Client *c)
{
    __detach_helper(stack, Client, c, c->desktop->stack, snext, sprev, c->desktop->slast);
}

void
detachrestack(Client *c)
{
    __detach_helper(rstack, Client, c, c->desktop->rstack, rnext, rprev, c->desktop->rlast);
}

void
detachfocus(Client *c)
{
    __detach_helper(focus, Client, c, c->desktop->focus, fnext, fprev, c->desktop->flast);

    Desktop *desk = c->desktop;
    /* "detach" */
    c->fprev = NULL;
    c->fnext = NULL;
    /* this just updates desktop->sel */
    if (c == desk->sel)
    {
        for (c = startfocus(desk); c && !ISVISIBLE(c); c = nextfocus(c));
        if(c)
        {   c->desktop->sel = c;
        }
    }
}

void
cleanupdesktop(Desktop *desk)
{
    Client *c = NULL;
    Client *next = NULL;
    c = startclient(desk);
    while(c)
    {
        next = nextclient(c);
        cleanupclient(c);
        c = next;
    }
    free(desk);
    desk = NULL;
}

Desktop *
createdesktop(void)
{
    Desktop *desk = calloc(1, sizeof(Desktop));
    if(!desk)
    {
        Debug("%s", "WARN: FAILED TO CREATE DESKTOP");
        return NULL;
    }
    desk->layout = 0;
    desk->olayout= 0;
    desk->clients= NULL;
    desk->clast = NULL;
    desk->stack = NULL;
    desk->slast = NULL;
    desk->mon = NULL;
    return desk;
}

void
floating(Desktop *desk)
{
    /* for now just check in restack for it */
    monocle(desk);
}

void
grid(Desktop *desk)
{
    const float bgwr = _cfg.GapRatio;

    i32 n, cols, rows, cn, rn, i, cx, cy, cw, ch;
    i32 nx, ny;
    i32 nw, nh;
	Client *c;
    Monitor *m = desk->mon;

	for(n = 0, c = nexttiled(startstack(desk)); c; c = nexttiled(nextstack(c)), n++);
	if(n == 0)
    {   return;
    }

	/* grid dimensions */
	for(cols = 0; cols <= n/2; cols++)
    {   
        if(cols*cols >= n)
        {    break;
        }
    }
    /* set layout against the general calculation: not 1:2:2, but 2:3 */
	if(n == 5) 
    {   cols = 2;
    }
	rows = n / cols;

	/* window geometries */
    cw = m->ww / (cols + !cols);
    /* current column number */
	cn = 0;
    /* current row number */
	rn = 0; 
	for(i = 0, c = nexttiled(startstack(desk)); c; i++, c = nexttiled(nextstack(c))) 
    {
		if((i / rows) + 1 > cols - (n % cols))
        {   rows = n/cols + 1;
        }
        ch = m->wh / (rows + !rows);
		cx = m->wx + cn * cw;
		cy = m->wy + rn * ch;

        nx = cx;
        ny = cy;
        nw = cw - (WIDTH(c) - c->w);
        nh = ch - (HEIGHT(c) - c->h);

        nx += (nw - (nw * bgwr)) / 2;
        ny += (nh - (nh * bgwr)) / 2;
        nw *= bgwr;
        nh *= bgwr;

		resize(c, nx, ny, nw, nh, 0);
        ++rn;
		if(rn >= rows) 
        {
			rn = 0;
            ++cn;
		}
	}
}

void
monocle(Desktop *desk)
{
    Client *c;
    Monitor *m = desk->mon;
    i32 nw, nh;
    const i32 nx = m->wx;
    const i32 ny = m->wy;

    for(c = nexttiled(startstack(desk)); c; c = nexttiled(nextstack(c)))
    {
        nw = m->ww - (c->bw * 2);
        nh = m->wh - (c->bw * 2);
        resize(c, nx, ny, nw, nh, 0);
    }
}

Desktop *
nextdesktop(Desktop *desk)
{
    return desk ? desk->next : desk;
}

Desktop *
prevdesktop(Desktop *desk)
{
    return desk ? desk->prev : desk;
}

void
restack(Desktop *desk)
{
    XCBWindowChanges wc;

    wc.stack_mode = XCB_STACK_MODE_BELOW;

    if(desk->mon->bar && !ISHIDDEN(desk->mon->bar))
    {   wc.sibling = desk->mon->bar->win;
    }
    else
    {   /* TODO: Maybe use wc.sibling = _wm.root? That causes error to be generated though. */
        wc.sibling = _wm.wmcheckwin;
    }

    Client *c = NULL;
    Client *slist = NULL;
    u8 config = 0;
    u8 instack = 0;
    i32 i = 0;
    XCBWindow winstack[X11_DEFAULT_MAX_WINDOW_LIMIT];
    
    c = startstack(desk);

    for(; c; c = nextstack(c))
    {
        instack = nextrstack(c) || prevrstack(c);
        /* Client holds both lists so we just check if the next's are the same if not configure it */
        config = nextrstack(c) != nextstack(c) || !instack;
        if(config)
        {   
            XCBConfigureWindow(_wm.dpy, c->win, XCB_CONFIG_WINDOW_SIBLING|XCB_CONFIG_WINDOW_STACK_MODE, &wc);
            Debug("Configured window: %s", c->netwmname);
        }
        wc.sibling = c->win;
        c->rprev = c->sprev;
        c->rnext = c->snext;

    }
    desk->rstack = desk->stack;;
    desk->rlast = desk->slast;

    slist = laststack(desk);
    /* reset client list */
    if(slist)
    {   XCBChangeProperty(_wm.dpy, _wm.root, netatom[NetClientListStacking], XCB_ATOM_WINDOW, 32, XCB_PROP_MODE_REPLACE, (unsigned char *)&slist->win, 1);
    }
    else
    {   XCBDeleteProperty(_wm.dpy, _wm.root, netatom[NetClientListStacking]);
    }

    for(slist = prevstack(slist); slist; slist = prevstack(slist))
    {
        winstack[i++] = slist->win;
        if(i == X11_DEFAULT_MAX_WINDOW_LIMIT)
        {   
            XCBChangeProperty(_wm.dpy, _wm.root, netatom[NetClientListStacking], XCB_ATOM_WINDOW, 32, XCB_PROP_MODE_APPEND, (unsigned char *)winstack, i);
            i = 0;
        }
    }
    if(i)
    {    XCBChangeProperty(_wm.dpy, _wm.root, netatom[NetClientListStacking], XCB_ATOM_WINDOW, 32, XCB_PROP_MODE_APPEND, (unsigned char *)winstack, i);
    }
}

void
reorder(Desktop *desk)
{
    updatestackpriorityfocus(desk);
    MERGE_SORT_LINKED_LIST(Client, stackpriority, desk->stack, desk->slast, snext, sprev, 1, 0);
}

/* how to return.
 * reference point is c1.
 *
 * sort order is 1,2,3,4,5,6
 */
static inline bool
__stack_priority_helper_above(const u32 x1, const u32 x2)
{
    return !(x1 & x2);
}
/* how to return.
 * reference point is c1.
 *
 * sort order is 6,5,4,3,2,1
 */
static inline bool
__stack_priority_helper_below(const u32 x1, const u32 x2)
{
    return x1 & x2;
}

void
setdesktoplayout(Desktop *desk, uint8_t layout)
{
    desk->olayout = desk->layout;
    desk->layout = layout;
}

int
stackpriority(Client *c1, Client *c2)
{
    const u32 ewmhflag1 = c1->ewmhflags;
    const u32 flags1 = c1->flags;
    const u32 ewmhflag2 = c2->ewmhflags;
    const u32 flags2 = c2->flags;

    const u32 ewmhflags = ewmhflag1 ^ ewmhflag2;
    const u32 flags = flags1 ^ flags2;

    /* PRIORITY WHEN CHECKED: (IN ORDER)
     * HIGHEST [0]
     * MEDIUM  [1]
     * LOWEST  [2]
     */
    static const u32 BELOW_PRIORTY[] = 
    {
        WStateFlagBelow,
        WStateFlagHidden,
    };
    /* PRIORITY WHEN CHECKED: (IN ORDER)
     * HIGHEST [0]
     * MEDIUM  [1]
     * LOWEST  [2]
     */
    static const u32 ABOVE_PRIORITY[] = 
    {
        /* Due to the lack of virtual desktop handling this is no used by default. */
        /* WTypeFlagDesktop, */

        WTypeFlagDock,
        WTypeFlagSplash,
        WTypeFlagNotification,
        /* WStateFlagModal, */
        WStateFlagAbove,
        WTypeFlagDialog,
    };

    int i;
    for(i = 0; i < LENGTH(BELOW_PRIORTY); ++i)
    {
        if(ewmhflags & BELOW_PRIORTY[i])
        {   return __stack_priority_helper_below(ewmhflag1, BELOW_PRIORTY[i]);
        }
    }
    for(i = 0; i < LENGTH(ABOVE_PRIORITY); ++i)
    {
        if(ewmhflags & ABOVE_PRIORITY[i])
        {   return __stack_priority_helper_above(ewmhflag1, ABOVE_PRIORITY[i]);
        }
    }
    
    /* These are Special flags, they require extra handling :( */
    if(flags & ClientFlagFloating)
    {   
        /* possible cache miss if reverse order due to desktop pointer indirection */
        if(c1->desktop->layout != Floating && c2->desktop->layout != Floating)
        {   return __stack_priority_helper_above(flags1, ClientFlagFloating);
        }
    }


    /* return correct focus order */
    return c1->rstacknum > c2->rstacknum;
}

void
tile(Desktop *desk)
{
    const u16 nmaster = _cfg.MCount;
    const float mfact = _cfg.MFact;
    const float bgwr = _cfg.GapRatio;

    i32 h, mw, my, ty;
    i32 n, i;
    i32 nx, ny;
    i32 nw, nh;

    Client *c = NULL;
    Monitor *m = desk->mon;

    n = 0;
    for(c = nexttiled(startstack(desk)); c; c = nexttiled(nextstack(c)), ++n);

    if(!n) 
    {   return;
    }
    
    if(n > nmaster)
    {   mw = nmaster ? m->ww * mfact: 0;
    }
    else
    {   mw = m->ww;
    }

    i = my = ty = 0;
    for (c = nexttiled(startstack(desk)); c; c = nexttiled(nextstack(c)))
    {
        if (i < nmaster)
        {
            h = (m->wh - my) / (MIN(n, nmaster) - i);
            nx = m->wx;
            ny = m->wy + my;
            nw = mw - c->bw * 2;
            nh = h - c->bw * 2;

            nx += (nw - (nw * bgwr)) / 2;
            ny += (nh - (nh * bgwr)) / 2;
            nw *= bgwr;
            nh *= bgwr;

            resize(c, nx, ny, nw, nh, 0);
            if (my + HEIGHT(c) < m->wh) 
            {   my += HEIGHT(c);
            }
        }
        else
        {
            h = (m->wh - ty) / (n - i);
            nx = m->wx + mw;
            ny = m->wy + ty;
            nw = m->ww - mw - c->bw * 2;
            nh = h - c->bw * 2;

            nx += (nw - (nw * bgwr)) / 2;
            ny += (nh - (nh * bgwr)) / 2;
            nw *= bgwr;
            nh *= bgwr;

            resize(c, nx, ny, nw, nh, 0);
            if (ty + HEIGHT(c) < m->wh) 
            {   ty += HEIGHT(c);
            }
        }
        ++i;
    }
}

void
updatedesktop(void)
{
    i32 base = _wm.selmon->desktops->num;
    i32 offset = _wm.selmon->desksel->num;

    i32 num = offset - base;

    if(unlikely(num < 0))
    {   
        num = 0;
        Debug0("Desktop num is negative, FIXME");
    }

    XCBChangeProperty(_wm.dpy, _wm.root, netatom[NetCurrentDesktop], XCB_ATOM_CARDINAL, 32, XCB_PROP_MODE_REPLACE, (unsigned char *)&num, 1);
}

void
updatedesktopnames(void)
{
    /* TODO */
}

void
updatedesktopnum(void)
{
    i32 data = _wm.selmon->deskcount;
    XCBChangeProperty(_wm.dpy, _wm.root, netatom[NetNumberOfDesktops], XCB_ATOM_CARDINAL, 32, XCB_PROP_MODE_REPLACE, (unsigned char *)&data, 1);
}

void
updatedesktopviewport(void)
{
    enum { x, y, data_spec = 2 };
    enum { NO_SUPPORT_LARGE_DESKTOPS = 0 };

    i32 data[data_spec];

    /* As of 3.2.0 vox-wm does not support large desktops */

    data[x] = NO_SUPPORT_LARGE_DESKTOPS;
    data[y] = NO_SUPPORT_LARGE_DESKTOPS;
    

    XCBChangeProperty(_wm.dpy, _wm.root, netatom[NetDesktopViewport], XCB_ATOM_CARDINAL, 32, XCB_PROP_MODE_REPLACE, (unsigned char *)data, data_spec);
}

void
updatedesktopworkarea(void)
{
    enum { x, y, w, h, data_spec = 4 };


    i32 data[data_spec];

    /* The Window Manager SHOULD calculate this space by taking the current page minus space occupied by dock and panel windows, 
     * as indicated by the _NET_WM_STRUT or _NET_WM_STRUT_PARTIAL properties set on client windows.
     *
     * Should? So basically no. 
     * Reason: Apps should read this hint when designing their application *should*, doesnt mean they do though.
     *         Furthermore this simplifies things and apps trying to mess with our system a bit.
     */
    data[x] = _wm.selmon->mx;
    data[y] = _wm.selmon->my;
    data[w] = _wm.selmon->mw;
    data[h] = _wm.selmon->mh;

    u32 i;
    u16 desktops = _wm.selmon->deskcount;
    bool first = true;

    enum XCBPropertyMode mode;

    for(i = 0; i < desktops; ++i)
    {
        if(first)
        { 
            mode = XCBPropModeReplace;
            first = false;    
        }
        else
        {   mode = XCBPropModeAppend;
        }
        XCBChangeProperty(_wm.dpy, _wm.root, netatom[NetWorkarea], XCB_ATOM_CARDINAL, 32, mode, (unsigned char *)data, data_spec);
    }
}

void
updatestackpriorityfocus(Desktop *desk)
{
    Client *c;
    int i = 0;
    for(c = startfocus(desk); c; c = nextfocus(c))
    {
        c->rstacknum = ++i;
        if(ISFLOATING(c) && DOCKED(c))
        {   setfloating(c, 0);
        }
    }
}

