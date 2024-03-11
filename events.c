#include "xcb_trl.h"
#include "events.h"
#include "util.h"
#include "config.h"
#include "dwm.h"

extern WM *_wm;
extern void xerror(XCBDisplay *display, XCBGenericError *error);

void (*handler[LASTEvent]) (XCBGenericEvent *) = 
{
    /* Keyboard */
    [XCB_KEY_PRESS] = keypress,
    [XCB_KEY_RELEASE] = keyrelease,
    /* Pointer */
    [XCB_BUTTON_PRESS] = buttonpress,
    [XCB_BUTTON_RELEASE] = buttonrelease,
    [XCB_MOTION_NOTIFY] = motionnotify,
    /* Win Crossing */
    [XCB_ENTER_NOTIFY] = enternotify,
    [XCB_LEAVE_NOTIFY] = leavenotify,
    /* Input focus */
    [XCB_FOCUS_IN] = focusin,
    [XCB_FOCUS_OUT] = focusout,
    /* keymap state notification */
    [XCB_KEYMAP_NOTIFY] = keymapnotify,
    /* Exposure */
    [XCB_EXPOSE] = expose,
    [XCB_GRAPHICS_EXPOSURE] = graphicsexpose,
    [XCB_NO_EXPOSURE] = noexpose,
    /* structure control */
    [XCB_CONFIGURE_REQUEST] = configurerequest,
    [XCB_CIRCULATE_REQUEST] = circulaterequest,
    [XCB_MAP_REQUEST] = maprequest,
    [XCB_RESIZE_REQUEST] = resizerequest,

    /* window state notify */
    [XCB_CIRCULATE_NOTIFY] = circulatenotify,
    [XCB_CONFIGURE_NOTIFY] = configurenotify,
    [XCB_CREATE_NOTIFY] = createnotify,
    [XCB_DESTROY_NOTIFY] = destroynotify,
    [XCB_GRAVITY_NOTIFY] = gravitynotify,
    [XCB_MAP_NOTIFY] = mapnotify,
    [XCB_MAPPING_NOTIFY] = mappingnotify,
    [XCB_UNMAP_NOTIFY] = unmapnotify,
    [XCB_VISIBILITY_NOTIFY] = visibilitynotify,
    [XCB_REPARENT_NOTIFY] = reparentnotify,
    /* colour map state notify */
    [XCB_COLORMAP_NOTIFY] = colormapnotify,
    /* client communication */
    [XCB_CLIENT_MESSAGE] = clientmessage,
    [XCB_PROPERTY_NOTIFY] = propertynotify,
    [XCB_SELECTION_CLEAR] = selectionclear,
    [XCB_SELECTION_NOTIFY] = selectionnotify,
    [XCB_SELECTION_REQUEST] = selectionrequest,
    [XCB_GE_GENERIC] = genericevent,
    [XCB_NONE] = errorhandler,
};

void
keypress(XCBGenericEvent *event)
{
    XCBKeyPressEvent *ev = (XCBKeyPressEvent *)event;
    const i16 rootx             = ev->root_x;
    const i16 rooty             = ev->root_y;
    const i16 eventx            = ev->event_x;
    const i16 eventy            = ev->event_y;
    const u16 state             = ev->state;
    const XCBWindow eventroot   = ev->root;
    const XCBWindow eventwin    = ev->event;
    const XCBWindow eventchild  = ev->child;
    const u8 samescreen         = ev->same_screen;
    const XCBKeyCode keydetail  = ev->detail;
    const XCBTimestamp tim      = ev->time;


    const i32 cleanstate = CLEANMASK(state);

    /* ONLY use lowercase cause we dont know how to handle anything else */
    const XCBKeysym sym = XCBKeySymbolsGetKeySym(_wm->syms, keydetail, 0);
    /* Only use upercase cause we dont know how to handle anything else
     * sym = XCBKeySymbolsGetKeySym(_wm->syms,  keydetail, 0);
     */
    /* This Could work MAYBE allowing for upercase and lowercase Keybinds However that would complicate things due to our ability to mask Shift
     * sym = XCBKeySymbolsGetKeySym(_wm->syms, keydetail, cleanstate); 
     */
    int i;
    for(i = 0; i < LENGTH(keys); ++i)
    {
        if(keys[i].type == XCB_KEY_PRESS)
        {
            if (sym == keys[i].keysym
                    && CLEANMASK(keys[i].mod) == cleanstate
                    && keys[i].func) 
            {   keys[i].func(&(keys[i].arg));
                return;
            }
        }
    }
}

void
keyrelease(XCBGenericEvent *event)
{
    XCBKeyReleaseEvent *ev = (XCBKeyReleaseEvent *)event;
    const i16 rootx             = ev->root_x;
    const i16 rooty             = ev->root_y;
    const i16 eventx            = ev->event_x;
    const i16 eventy            = ev->event_y;
    const u16 state             = ev->state;
    const XCBWindow eventroot   = ev->root;
    const XCBWindow eventwin    = ev->event;
    const XCBWindow eventchild  = ev->child;
    const u8 samescreen         = ev->same_screen;
    const XCBKeyCode keydetail  = ev->detail;
    const XCBTimestamp tim      = ev->time;

    const i32 cleanstate = CLEANMASK(state);
    /* ONLY use lowercase cause we dont know how to handle anything else */
    const XCBKeysym sym = XCBKeySymbolsGetKeySym(_wm->syms, keydetail, 0);
    /* Only use upercase cause we dont know how to handle anything else
     * sym = XCBKeySymbolsGetKeySym(_wm->syms,  keydetail, 0);
     */
    /* This Could work MAYBE allowing for upercase and lowercase Keybinds However that would complicate things due to our ability to mask Shift
     * sym = XCBKeySymbolsGetKeySym(_wm->syms, keydetail, cleanstate); 
     */
    int i;
    for(i = 0; i < LENGTH(keys); ++i)
    {
        if(keys[i].type == XCB_KEY_RELEASE)
        {
            if (sym == keys[i].keysym
                    && CLEANMASK(keys[i].mod) == cleanstate
                    && keys[i].func) 
            {   keys[i].func(&(keys[i].arg));
                return;
            }
        }
    }
}

void
buttonpress(XCBGenericEvent *event)
{
    XCBButtonPressEvent *ev = (XCBButtonPressEvent *)event;
    const i16 rootx             = ev->root_x;
    const i16 rooty             = ev->root_y;
    const i16 eventx            = ev->event_x;
    const i16 eventy            = ev->event_y;
    const u16 state             = ev->state;
    const XCBWindow eventroot   = ev->root;
    const XCBWindow eventwin    = ev->event;
    const XCBWindow eventchild  = ev->child;
    const u8 samescreen         = ev->same_screen;
    const XCBKeyCode keydetail  = ev->detail;
    const XCBTimestamp tim      = ev->time;

    const i32 cleanstate = CLEANMASK(state);

    Monitor *m;
    /* focus monitor if necessary */
    if ((m = wintomon(eventwin)))
    {
        if(m != _wm->selmon)
        {
            unfocus(_wm->selmon->sel, 1);
            _wm->selmon = m;
            focus(NULL);
        }
        /* no need to handle any other clients */
        return;
    }

    Client *c;
    if((c = wintoclient(eventwin)))
    {   
        if(m && m->sel != c)   
        {   
            if(c->mon != m)
            {   c->mon = m;
            }
            detachclient(c);
            attachclient(c);
            focus(c);
            if(ISFLOATING(c) || ISALWAYSONTOP(c))
            {   XCBRaiseWindow(_wm->dpy, c->win);
            }
            XCBAllowEvents(_wm->dpy, XCB_ALLOW_REPLAY_POINTER, XCB_TIME_CURRENT_TIME);
        }
    }
    int i;
    for(i = 0; i < LENGTH(buttons); ++i)
    {   
        if(buttons[i].type == XCB_BUTTON_PRESS
            && buttons[i].func
            && buttons[i].button == keydetail
            && CLEANMASK(buttons[i].mask) == cleanstate)
        {
            Arg arg;
            arg.v = ev;
            buttons[i].func(&arg);
            break;
        }
    }
    XCBSync(_wm->dpy);
}

void
buttonrelease(XCBGenericEvent *event)
{
    XCBButtonReleaseEvent *ev = (XCBButtonReleaseEvent *)event;
    const i16 rootx             = ev->root_x;
    const i16 rooty             = ev->root_y;
    const i16 eventx            = ev->event_x;
    const i16 eventy            = ev->event_y;
    const u16 state             = ev->state;
    const XCBWindow eventroot   = ev->root;
    const XCBWindow eventwin    = ev->event;
    const XCBWindow eventchild  = ev->child;
    const u8 samescreen         = ev->same_screen;
    const XCBKeyCode keydetail  = ev->detail;
    const XCBTimestamp tim      = ev->time;

    const i32 cleanstate = CLEANMASK(state);

    int i;
    for(i = 0; i < LENGTH(buttons); ++i)
    {   
        if(buttons[i].type == XCB_BUTTON_RELEASE
            && buttons[i].func
            && buttons[i].button == keydetail
            && CLEANMASK(buttons[i].mask) == cleanstate)
        {
            Arg arg;
            arg.v = ev;
            buttons[i].func(&arg);
            break;
        }
    }
}

void
motionnotify(XCBGenericEvent *event)
{
    XCBMotionNotifyEvent *ev = (XCBMotionNotifyEvent *)event;
    const i16 rootx             = ev->root_x;
    const i16 rooty             = ev->root_y;
    const i16 eventx            = ev->event_x;
    const i16 eventy            = ev->event_y;
    const u16 state             = ev->state;
    const XCBWindow eventroot   = ev->root;
    const XCBWindow eventwin    = ev->event;
    const XCBWindow eventchild  = ev->child;
    const u8 samescreen         = ev->same_screen;
    const XCBKeyCode keydetail  = ev->detail;
    const XCBTimestamp tim      = ev->time;


    

    static Monitor *mon = NULL;
    Monitor *m;
    const XCBWindow root = _wm->root;

    if(eventwin != root)
    {   return;
    }


    if((m = recttomon(rootx, rooty, 1, 1)) != mon && mon)
    {
        Client *c = _wm->selmon->sel;
        if(c)
        {   unfocus(c, 1);
        }
        _wm->selmon = m;
        focus(NULL);
    }
    mon = m;
}

void
enternotify(XCBGenericEvent *event)
{
    XCBEnterNotifyEvent *ev = (XCBEnterNotifyEvent *)event;
    const uint8_t detail    = ev->detail;
    const XCBTimestamp tim  = ev->time;
    const XCBWindow eventroot = ev->root;
    const XCBWindow eventwin = ev->event;
    const XCBWindow eventchild = ev->child;
    const int16_t rootx = ev->root_x;
    const int16_t rooty = ev->root_y;
    const int16_t eventx = ev->event_x;
    const int16_t eventy = ev->event_y;
    const uint16_t state = ev->state;
    const uint8_t mode   = ev->mode;
    const uint8_t samescreenfocus = ev->same_screen_focus;



    /* hover focus */



    Client *c;
    Monitor *m;

    if((mode != XCB_NOTIFY_MODE_NORMAL || detail == XCB_NOTIFY_DETAIL_INFERIOR) || eventwin != _wm->root)
    {   return;
    }

    c = wintoclient(eventwin);
    m = c ? c->mon : wintomon(eventwin);

    if(m != _wm->selmon)
    {
        unfocus(_wm->selmon->sel, 1);
        _wm->selmon = m;
    }
    else if(!c || c == _wm->selmon->sel)
    {   return;
    }
    focus(c);
}

void
leavenotify(XCBGenericEvent *event)
{
    XCBLeaveNotifyEvent *ev = (XCBLeaveNotifyEvent *)event;
    const uint8_t detail    = ev->detail;
    const XCBTimestamp tim  = ev->time;
    const XCBWindow eventroot = ev->root;
    const XCBWindow eventwin = ev->event;
    const XCBWindow eventchild = ev->child;
    const int16_t rootx = ev->root_x;
    const int16_t rooty = ev->root_y;
    const int16_t eventx = ev->event_x;
    const int16_t eventy = ev->event_y;
    const uint16_t state = ev->state;
    const uint8_t mode   = ev->mode;
    const uint8_t samescreenfocus = ev->same_screen_focus;
    
}

/* there are some broken focus acquiring clients needing extra handling */
void
focusin(XCBGenericEvent *event)
{
    XCBFocusInEvent *ev = (XCBFocusInEvent *)event;
    const u8 detail = ev->detail;
    const XCBWindow eventwin = ev->event;
    const u8 mode = ev->mode;

    if(_wm->selmon->sel && eventwin != _wm->selmon->sel->win)
    {   setfocus(_wm->selmon->sel);
    }
    XCBSync(_wm->dpy);
}

void
focusout(XCBGenericEvent *event)
{
    XCBFocusOutEvent *ev = (XCBFocusOutEvent *)event;
    const u8 detail = ev->detail;
    const XCBWindow eventwin = ev->event;
    const u8 mode = ev->mode;
}

void
keymapnotify(XCBGenericEvent *event)
{
    XCBKeymapNotifyEvent *ev = (XCBKeymapNotifyEvent *)event;
    u8 *eventkeys   = ev->keys;        /* DONOT FREE */
}

void
expose(XCBGenericEvent *event)
{
    XCBExposeEvent *ev = (XCBExposeEvent *)event;
}

void
graphicsexpose(XCBGenericEvent *event)
{
    XCBGraphicsExposeEvent *ev = (XCBGraphicsExposeEvent *)event;
    const i16 x                 = ev->x;
    const i16 y                 = ev->y;
    const u16 w                 = ev->width;
    const u16 h                 = ev->height;
    const u16 count             = ev->count;
    const XCBDrawable drawable  = ev->drawable;
    const u8 majoropcode        = ev->major_opcode;
    const u16 minoropcode       = ev->minor_opcode;
}

void
noexpose(XCBGenericEvent *event)
{
    XCBExposeEvent *ev = (XCBExposeEvent *)event;
    const i16 x                 = ev->x;
    const i16 y                 = ev->y;
    const u16 w                 = ev->width;
    const u16 h                 = ev->height;
    const u16 count             = ev->count;
    const XCBWindow win         = ev->window;
}

void
circulaterequest(XCBGenericEvent *event)
{
    XCBCirculateRequestEvent *ev = (XCBCirculateRequestEvent *)event;
    const XCBWindow win         = ev->window;
    const XCBWindow eventwin    = ev->event;
    const u8 place              = ev->place;
}

void
configurerequest(XCBGenericEvent *event)
{   
    XCBConfigureRequestEvent *ev = (XCBConfigureRequestEvent *)event;
    const i16 x     = ev->x;
    const i16 y     = ev->y;
    const u16 w     = ev->width;
    const u16 h     = ev->height;
    const u16 bw    = ev->border_width;
    const u16 mask  = ev->value_mask;
    const u8  stack = ev->stack_mode;
    const XCBWindow win     = ev->window;
    const XCBWindow parent  = ev->parent;
    const XCBWindow sibling = ev->sibling;

    Client *c;
    Monitor *m;
    if((c = wintoclient(win)))
    {
        m = c->mon;
        if(mask & XCB_CONFIG_WINDOW_BORDER_WIDTH)
        {   setborderwidth(c, bw);
        }
        if(mask & XCB_CONFIG_WINDOW_X)
        {
            c->oldx = c->x;
            c->x = m->mx + ev->x;
        }
        if(mask & XCB_CONFIG_WINDOW_Y)
        {
            c->oldy = c->y;
            c->y = m->my + ev->y;
        }
        if(mask & XCB_CONFIG_WINDOW_WIDTH)
        {
            c->oldw = c->w;
            c->w = ev->width;
        }
        if(mask & XCB_CONFIG_WINDOW_HEIGHT)
        {
            c->oldh = c->h;
            c->h = ev->height;
        }
        if(mask & XCB_CONFIG_WINDOW_SIBLING)
        {
            ASSUME(0);
            if(sibling != None)
            {   /* Ignore these requests we handle stack order */
            }
        }
        if(mask & XCB_CONFIG_WINDOW_STACK_MODE)
        {
            /* Ignore these requests we handle stack order */
            ASSUME(0);
            switch(stack)
            {
                case XCB_STACK_MODE_ABOVE: /* XRaiseAboveSibling(ev->above) */ break;
                case XCB_STACK_MODE_BELOW: /* XLowerBelowSibling(ev->above) */ break;
                case XCB_STACK_MODE_TOP_IF: /* XRaiseWindow(dpy, ev->window) */ break;
                case XCB_STACK_MODE_BOTTOM_IF:/* XLowerToBottomWindow(dpy, e->window)*/ break;
                case XCB_STACK_MODE_OPPOSITE: /* XFlipStackOrder(ev->above, ev->window)*/ break;
            }
        }
        if((c->x + c->w) > m->mx + m->mw && ISFLOATING(c))
        {   
            c->oldx = c->x;
            c->x = m->mx + ((m->mw >> 1) - (WIDTH(c) >> 1)); /* center in x direction */
        }
        if((c->y + c->h) > m->my + m->mh && ISFLOATING(c))
        {   
            c->oldy = c->y;
            c->y = m->my + ((m->mh >> 1) - (HEIGHT(c) >> 1)); /* center in y direction */
        }
        if(mask & (XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y) 
        && !(mask & (XCB_CONFIG_WINDOW_WIDTH|XCB_CONFIG_WINDOW_HEIGHT)))
        {   configure(c);
        }
        if(ISVISIBLE(c))
        {   XCBMoveResizeWindow(_wm->dpy, c->win, c->x, c->y, c->w, c->h);
        }
    }
    else
    {
        XCBWindowChanges wc;
        wc.x = x;
        wc.y = y;
        wc.width = w;
        wc.height = h;
        wc.border_width = bw;
        wc.sibling = sibling;
        wc.stack_mode = stack;
        XCBConfigureWindow(_wm->dpy, win, mask, &wc);
    }
}

void
maprequest(XCBGenericEvent *event)
{
    XCBMapRequestEvent *ev  = (XCBMapRequestEvent *)event;
    const XCBWindow parent  = ev->parent;
    const XCBWindow win     = ev->window;

    if(!wintoclient(win))
    {   manage(ev->window);
    }
    XCBSync(_wm->dpy);
}
void
resizerequest(XCBGenericEvent *event)
{
    XCBResizeRequestEvent *ev = (XCBResizeRequestEvent *)event;
    const XCBWindow win = ev->window;
    const u16 w         = ev->width;
    const u16 h         = ev->height;

    /* popup windows sometimes need this */
    Client *c;
    
    if((c = wintoclient(win)))
    {   resize(c, c->x, c->y, w, h, 1);
    }
    else
    {   XCBResizeWindow(_wm->dpy, win, w, h);
    }
}

void
circulatenotify(XCBGenericEvent *event)
{
    XCBCirculateNotifyEvent *ev = (XCBCirculateNotifyEvent *)event;
}

/* These events are mostly just Info events of stuff that has happened already
 * so this tells that happened (x/y/w/h) and only tells that (AKA sends this event)
 * if we sucesfully did that action So we only really need to check root events here
 * cause this only occurs on sucesfull actions
 */
void
configurenotify(XCBGenericEvent *event)
{
    XCBConfigureNotifyEvent *ev = (XCBConfigureNotifyEvent *)event;
    const XCBWindow eventwin = ev->event;
    const XCBWindow win = ev->window;
    const XCBWindow abovesibling = ev->above_sibling;
    const i16 x = ev->x;
    const i16 y = ev->y;
    const u16 w = ev->width;
    const u16 h = ev->height;
    const u16 borderwidth = ev->border_width;
    const u8 overrideredirect = ev->override_redirect;

    if(win == _wm->root)
    {
        u8 dirty;
        dirty = (_wm->sw != w || _wm->sh != h);
        _wm->sw = w;
        _wm->sh = h;

        if(updategeom() || dirty)
        {
            Monitor *m;
            Desktop *desk;
            Client *c;
            /* update the bar */

            m = _wm->mons;
            while((m = nextmonitor(m)))
            {
                desk = m->desktops;
                while((desk = nextdesktop(desk)))
                {
                    c = desk->clients;
                    while((c = nextclient(c)))
                    {
                        if(ISFULLSCREEN(c))
                        {   resizeclient(c, m->mx, m->my, m->mw, m->mh);
                        }
                        XCBMoveResizeWindow(_wm->dpy, m->barwin, m->wx, m->by, m->ww, m->bh);
                    }
                }
            }
            focus(NULL);
            /* arrangeall */
        }
    }

    if(overrideredirect)
    {
        Client *c;
        if((c = wintoclient(win)))
        {   unmanage(c);
        }
    }
}

void
createnotify(XCBGenericEvent *event)
{
    XCBCreateNotifyEvent *ev = (XCBCreateNotifyEvent *)event;
}

void
destroynotify(XCBGenericEvent *event)
{
    XCBDestroyNotifyEvent *ev = (XCBDestroyNotifyEvent *)event;
}

void
gravitynotify(XCBGenericEvent *event)
{
    XCBGravityNotifyEvent *ev = (XCBGravityNotifyEvent *)event;
}

void
mapnotify(XCBGenericEvent *event)
{
    XCBMapNotifyEvent *ev = (XCBMapNotifyEvent *)event;
    const XCBWindow win             = ev->window;
    const XCBWindow eventwin        = ev->event;
    const uint8_t override_redirect = ev->override_redirect;
}

void
mappingnotify(XCBGenericEvent *event)
{
    XCBMappingNotifyEvent *ev = (XCBMappingNotifyEvent *)event;
    const XCBKeyCode first_keycode = ev->first_keycode;
    const uint8_t count            = ev->count;
    const uint8_t request          = ev->request;

    XCBRefreshKeyboardMapping(_wm->syms, ev);
    if(ev->request == XCB_MAPPING_KEYBOARD)
    {   grabkeys();
    }
}

void
unmapnotify(XCBGenericEvent *event)
{
    XCBUnMapNotifyEvent *ev = (XCBUnMapNotifyEvent *)event;
    const XCBWindow eventwin    = ev->event;
    const XCBWindow win         = ev->window;
    const uint8_t isconfigure   = ev->from_configure;

    Client *c;
    if((c = wintoclient(win)) || (c = wintoclient(eventwin)))
    {   unmanage(c);
    }
    XCBSync(_wm->dpy);
}

void
visibilitynotify(XCBGenericEvent *event)
{
    XCBVisibilityNotifyEvent *ev = (XCBVisibilityNotifyEvent *)event;
}

void
reparentnotify(XCBGenericEvent *event)
{
    XCBReparentNotifyEvent *ev = (XCBReparentNotifyEvent *)event;
}

void
colormapnotify(XCBGenericEvent *event)
{
    XCBColormapNotifyEvent *ev = (XCBColormapNotifyEvent *)event;
}

void
clientmessage(XCBGenericEvent *event)
{
    XCBClientMessageEvent *ev = (XCBClientMessageEvent *)event;
    const XCBWindow win             = ev->window;
    const XCBAtom atom              = ev->type;
    const u8 format                 = ev->format;
    const XCBClientMessageData data = ev->data;     /* union "same" as xlib data8 -> b[20] data16 -> s[10] data32 = l[5] */
}

void
propertynotify(XCBGenericEvent *event)
{
    XCBPropertyNotifyEvent *ev = (XCBPropertyNotifyEvent *)event;
}

void
selectionclear(XCBGenericEvent *event)
{
    XCBSelectionClearEvent *ev = (XCBSelectionClearEvent *)event;
}

void
selectionnotify(XCBGenericEvent *event)
{
    XCBSelectionNotifyEvent *ev = (XCBSelectionNotifyEvent *)event;
}

void
selectionrequest(XCBGenericEvent *event)
{
    XCBSelectionRequestEvent *ev = (XCBSelectionRequestEvent *)event;
}


void
genericevent(XCBGenericEvent *event)
{   (void)event;
}

void
errorhandler(XCBGenericEvent *event)
{   xerror(_wm->dpy, (XCBGenericError *)event);
}
