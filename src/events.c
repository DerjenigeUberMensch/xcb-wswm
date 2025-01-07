#include <string.h>

#include "events.h"
#include "util.h"
#include "keybinds.h"
#include "main.h"
#include "monitor.h"
#include "client.h"
#include "desktop.h"
#include "getprop.h"
#include "settings.h"


extern WM _wm;
extern UserSettings _cfg;
extern XCBAtom netatom[NetLast];
extern XCBAtom wmatom[WMLast];
extern XCBAtom motifatom;

extern void xerror(XCBDisplay *display, XCBGenericError *error);

void (*handler[XCBLASTEvent]) (XCBGenericEvent *) = 
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

    (void)rootx;
    (void)rooty;
    (void)eventx;
    (void)eventy;
    (void)eventroot;
    (void)eventwin;
    (void)eventchild;
    (void)samescreen;
    (void)tim;

    const i32 cleanstate = CLEANMASK(state);
    /* ONLY use lowercase cause we dont know how to handle anything else */
    const XCBKeysym sym = XCBKeySymbolsGetKeySym(_wm.syms, keydetail, 0);
    /* Only use upercase cause we dont know how to handle anything else
     * sym = XCBKeySymbolsGetKeySym(_wm.syms,  keydetail, 0);
     */
    /* This Could work MAYBE allowing for upercase and lowercase Keybinds However that would complicate things due to our ability to mask Shift
     * sym = XCBKeySymbolsGetKeySym(_wm.syms, keydetail, cleanstate); 
     */
    Debug("%d", sym);
    int i;
    u8 sync = 0;
    for(i = 0; i < LENGTH(keys); ++i)
    {
        if(keys[i].type == XCB_KEY_PRESS)
        {
            if (sym == keys[i].keysym
                    && CLEANMASK(keys[i].mod) == cleanstate
                    && keys[i].func) 
            {   
                keys[i].func(&(keys[i].arg));
                sync = 1;
                break;
            }
        }
    }
    if(sync)
    {   XCBFlush(_wm.dpy);
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

    (void)rootx;
    (void)rooty;
    (void)eventx;
    (void)eventy;
    (void)eventroot;
    (void)eventwin;
    (void)eventchild;
    (void)samescreen;
    (void)tim;


    const i32 cleanstate = CLEANMASK(state);
    /* ONLY use lowercase cause we dont know how to handle anything else */
    const XCBKeysym sym = XCBKeySymbolsGetKeySym(_wm.syms, keydetail, 0);
    /* Only use upercase cause we dont know how to handle anything else
     * sym = XCBKeySymbolsGetKeySym(_wm.syms,  keydetail, 0);
     */
    /* This Could work MAYBE allowing for upercase and lowercase Keybinds However that would complicate things due to our ability to mask Shift
     * sym = XCBKeySymbolsGetKeySym(_wm.syms, keydetail, cleanstate); 
     */
    int i;
    u8 sync = 0;
    for(i = 0; i < LENGTH(keys); ++i)
    {
        if(keys[i].type == XCB_KEY_RELEASE)
        {
            if (sym == keys[i].keysym
                    && CLEANMASK(keys[i].mod) == cleanstate
                    && keys[i].func) 
            {   
                keys[i].func(&(keys[i].arg));
                sync = 1;
                break;
            }
        }
    }
    if(sync)
    {   XCBFlush(_wm.dpy);
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

    (void)rootx;
    (void)rooty;
    (void)eventx;
    (void)eventy;
    (void)eventroot;
    (void)eventchild;
    (void)samescreen;
    (void)tim;

    const i32 cleanstate = CLEANMASK(state);

    u8 sync = 0;
    Monitor *m = NULL;
    /* focus monitor if necessary */
    if ((m = wintomon(eventroot)))
    {   setmonsel(m);
    }

    Client *c;
    if((c = wintoclient(eventwin)))
    {
        focus(c);
        arrangeq(c->desktop);
        XCBAllowEvents(_wm.dpy, XCB_ALLOW_REPLAY_POINTER, XCB_CURRENT_TIME);
        sync = 1;
    }
    else
    {   
        /* set focus to root, but still maintain the selected client.
         * This makes it "seem" like we unfocused the window while still maintaing correct stack order.
         */
        if(_wm.selmon->desksel->sel)
        {   unfocus(_wm.selmon->desksel->sel, 1);
        }
        /* if no selected window, this should just set input focus to root, failsafe for above basically */
        XCBSetInputFocus(_wm.dpy, eventwin, XCB_INPUT_FOCUS_POINTER_ROOT, XCB_CURRENT_TIME);
        XCBChangeProperty(_wm.dpy, _wm.root, netatom[NetActiveWindow], XCB_ATOM_WINDOW, 32, XCB_PROP_MODE_REPLACE, (unsigned char *)&(eventwin), 1);
        /* shouldnt need to sync, but too lazy to test */
        sync = 1;
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
            sync = 1;
            Debug("%d", buttons[i].button);
            break;
        }
    }
    if(sync)
    {   XCBFlush(_wm.dpy);
    }
    Debug("ButtonPress: (x: %d, y: %d) [%u]", rootx, rooty, eventwin);
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


    (void)rootx;
    (void)rooty;
    (void)eventx;
    (void)eventy;
    (void)eventroot;
    (void)eventwin;
    (void)eventchild;
    (void)samescreen;
    (void)tim;

    const i32 cleanstate = CLEANMASK(state);
    u8 sync = 0;

    i16 i;
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
            sync = 1;
            Debug("%d", buttons[i].button);
            break;
        }
    }
    
    if(sync)
    {   XCBFlush(_wm.dpy);
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


    (void)eventx;
    (void)eventy;
    (void)eventroot;
    (void)state;
    (void)eventchild;
    (void)samescreen;
    (void)keydetail;
    (void)tim;


    /* due to the mouse being able to move a ton we want to limit the cycles burnt for non root events */
    if(eventwin != _wm.root)
    {   return;
    }


    u8 sync = 0;
    static Monitor *mon = NULL;
    Monitor *m;

    //Debug("(x: %d, y: %d)", rootx, rooty);
    //Debug("(w: %d, h: %d)", XCBDisplayWidth(_wm.dpy, _wm.screen), XCBDisplayHeight(_wm.dpy, _wm.screen));
    if((m = recttomon(rootx, rooty, 1, 1)) != mon && mon)
    {
        setmonsel(m);
        sync = 1;
    }
    mon = m;

    if(sync)
    {   XCBFlush(_wm.dpy);
    }
}

void
enternotify(XCBGenericEvent *event)
{
    if(!_cfg.HoverFocus)
    {   return;
    }
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



    (void)detail;
    (void)tim;
    (void)eventroot;
    (void)eventwin;
    (void)eventchild;
    (void)rootx;
    (void)rooty;
    (void)eventx;
    (void)eventy;
    (void)state;
    (void)mode;
    (void)samescreenfocus;

    Client *c;
    Monitor *m;
    u8 sync = 0;

    if((mode != XCB_NOTIFY_MODE_NORMAL || detail == XCB_NOTIFY_DETAIL_INFERIOR) && eventwin != _wm.root)
    {   return;
    }

    c = wintoclient(eventwin);
    m = c ? c->desktop->mon : wintomon(eventwin);

    setmonsel(m);

    if(c && c != _wm.selmon->desksel->sel)
    {   focus(c);
    }

    sync = 1;

    if(sync)
    {   XCBFlush(_wm.dpy);
    }
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

    (void)detail;
    (void)tim;
    (void)eventroot;
    (void)eventwin;
    (void)eventchild;
    (void)rootx;
    (void)rooty;
    (void)eventx;
    (void)eventy;
    (void)state;
    (void)mode;
    (void)samescreenfocus;
    
}

/* there are some broken focus acquiring clients needing extra handling */
void
focusin(XCBGenericEvent *event)
{
    XCBFocusInEvent *ev = (XCBFocusInEvent *)event;
    const u8 detail = ev->detail;
    const XCBWindow eventwin = ev->event;
    const u8 mode = ev->mode;

    (void)detail;
    (void)mode;

    u8 sync = 0;

    Client *sel = _wm.selmon->desksel->sel;
    if(sel && eventwin != sel->win)
    {   
        setfocus(sel);
        sync = 1;
    }
    if(sync)
    {   XCBFlush(_wm.dpy);
    }
}

void
focusout(XCBGenericEvent *event)
{
    XCBFocusOutEvent *ev = (XCBFocusOutEvent *)event;
    const u8 detail = ev->detail;
    const XCBWindow eventwin = ev->event;
    const u8 mode = ev->mode;

    (void)detail;
    (void)eventwin;
    (void)mode;
}

void
keymapnotify(XCBGenericEvent *event)
{
    XCBKeymapNotifyEvent *ev = (XCBKeymapNotifyEvent *)event;
    u8 *eventkeys   = ev->keys;        /* DONOT FREE */

    (void)eventkeys;
}

void
expose(XCBGenericEvent *event)
{
    XCBExposeEvent *ev = (XCBExposeEvent *)event;
    const XCBWindow win     = ev->window;
    const i16 x             = ev->x;
    const i16 y             = ev->y;
    const u16 w             = ev->width;
    const u16 h             = ev->height;
    const u16 count         = ev->count;


    (void)win;
    (void)x;
    (void)y;
    (void)w;
    (void)h;
    (void)count;

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


    (void)x;
    (void)y;
    (void)w;
    (void)h;
    (void)count;
    (void)drawable;
    (void)majoropcode;
    (void)minoropcode;
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


    (void)x;
    (void)y;
    (void)w;
    (void)h;
    (void)count;
    (void)win;
}

void
circulaterequest(XCBGenericEvent *event)
{
    XCBCirculateRequestEvent *ev = (XCBCirculateRequestEvent *)event;
    const XCBWindow win         = ev->window;
    const XCBWindow eventwin    = ev->event;
    const u8 place              = ev->place;

    (void)eventwin;

    /* TODO update stack */
    XCBCirculateSubwindows(_wm.dpy, win, !!place);
    switch(place)
    {   
        case XCB_CIRCULATE_RAISE_LOWEST:
            Debug("Circulate Up: [%u] ", win);
            break;
        case XCB_CIRCULATE_LOWER_HIGHEST:
            Debug("Circulate Down: [%u] ", win);
            break;
        default:
            Debug("Circulate Unknown: [%u] ", win);
            break;
    }
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

    (void)parent;

    Client *c;
    u8 sync = 0;
    u8 restack = 0;
    u8 geom = 0;
    if((c = wintoclient(win)))
    {
        const Monitor *m = c->desktop->mon;
        i32 rx = c->x;
        i32 ry = c->y;
        i32 rw = c->w;
        i32 rh = c->h;
        if(mask & XCB_CONFIG_WINDOW_BORDER_WIDTH)
        {                           /* Border width should NEVER be bigger than the screen */
            setborderwidth(c, bw);
            updateborderwidth(c);
        }
        if(mask & XCB_CONFIG_WINDOW_X)
        {   rx = m->mx + x;
        }
        if(mask & XCB_CONFIG_WINDOW_Y)
        {   ry = m->my + y;
        }
        if(mask & XCB_CONFIG_WINDOW_WIDTH)
        {   rw = w;
        }
        if(mask & XCB_CONFIG_WINDOW_HEIGHT)
        {   rh = h;
        }
        if(mask & XCB_CONFIG_WINDOW_STACK_MODE)
        {
            /* UNUSED, we do our own restacking */
            /* 
            XCBWindowChanges wc;
            const uint32_t modemask = XCB_CONFIG_WINDOW_STACK_MODE | (mask & XCB_CONFIG_WINDOW_SIBLING);
            wc.stack_mode = ev->stack_mode;
            wc.sibling = ev->sibling;
            XCBConfigureWindow(_wm.dpy, c->win, modemask, &wc);
            */
            if(mask & XCB_CONFIG_WINDOW_SIBLING)
            {
                Client *c1 = wintoclient(ev->sibling);
                switch(stack)
                {
                    case XCB_STACK_MODE_ABOVE:
                        if(c1)
                        {   attachfocusbefore(c, c1);
                        }
                        Debug("Raised Client: [%u] above [%u]", c->win, ev->sibling);
                        break;
                    case XCB_STACK_MODE_BELOW:
                        if(c1)
                        {   attachfocusafter(c, c1);
                        }
                        Debug("Lowered Client: [%u] below [%u]", c->win, ev->sibling);
                        break;
                    case XCB_STACK_MODE_TOP_IF:
                        if(c1)
                        {
                            if(stackpriority(c, c1))
                            {   
                                detachfocus(c);
                                attachfocus(c);
                            }
                        }
                        Debug("Raised Client: [%u] above stack from [%u]", c->win, ev->sibling);
                        break;
                    case XCB_STACK_MODE_BOTTOM_IF:  
                        if(c1)
                        {   
                            if(stackpriority(c, c1))
                            {   
                                if(c->desktop->slast != c)
                                {   attachfocusafter(c->desktop->slast, c);
                                }
                            }
                        }
                        Debug("Lowerd Client: [%u] below stack from [%u]", c->win, ev->sibling);
                        break;
                    case XCB_STACK_MODE_OPPOSITE:   
                        if(c1)
                        {   
                            if(stackpriority(c, c1))
                            {   
                                if(c->desktop->slast != c)
                                {   attachfocusafter(c->desktop->slast, c);
                                }
                            }
                            else
                            {   
                                detachfocus(c);
                                attachfocus(c);
                            }
                        }
                        Debug("Flipped Client: [%u] flipped one to bottom [%u]", c->win, ev->sibling);
                        break;
                }
            }
            else
            {
                switch(stack)
                {
                    case XCB_STACK_MODE_ABOVE:      
                        detachfocus(c);
                        attachfocus(c);
                        Debug("Raised Client: [%u] above stack", c->win); 
                        break;
                    case XCB_STACK_MODE_BELOW:      
                        if(c->desktop->slast != c)
                        {   attachfocusafter(c->desktop->slast, c);
                        }
                        Debug("Lowered Client: [%u] below stack", c->win); 
                        break;
                    case XCB_STACK_MODE_TOP_IF:
                        detachfocus(c);
                        attachfocus(c);
                        Debug("Raised Client: [%u] above stack if occluded", c->win);
                        break;
                    case XCB_STACK_MODE_BOTTOM_IF:
                        if(c->desktop->slast != c)
                        {   attachfocusafter(c->desktop->slast, c);
                        }
                        Debug("Lowerd Client: [%u] below stack if occluded", c->win);
                        break;
                    case XCB_STACK_MODE_OPPOSITE:
                        if(c->desktop->sel == c)
                        {
                            if(c->desktop->slast != c)
                            {   attachfocusafter(c->desktop->slast, c);
                            }
                        }
                        else
                        {   
                            detachfocus(c);
                            attachfocus(c);
                        }
                        Debug("Flipped Client: [%u] XORed stack if occluded (Above if so, Below if not)", c->win);
                        break;
                }
            }
            restack = 1;
        }
        geom = mask & (XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y|XCB_CONFIG_WINDOW_WIDTH|XCB_CONFIG_WINDOW_HEIGHT);
        if(geom)
        {
            applygravity(c->gravity, &rx, &ry, rw, rh, c->bw);
            resizeclient(c, rx, ry, rw, rh);
            if(!SHOULDBEFLOATING(c))
            {
                if(ISFLOATING(c))
                {   
                    setfloating(c, 0);
                    restack = 1;
                }
            }
            else
            {
                /* these checks are so we maintain wasfloating correctly without messing everything up */
                if(!ISFLOATING(c) && !DOCKED(c))
                {   
                    setfloating(c, 1);
                    restack = 1;
                }
            }
        }

        if(restack)
        {
            if(c)
            {   
                /* this fixes windows not maintaing focus order correctly for some applications that use configure requests.
                 * Basically while we did reattach focus we never set focus, so the stack order is correct but the focus order isnt.
                 * AKA altttab.
                 */
                focus(NULL);
                /* rearrange the stuff duh */
                arrange(c->desktop);
            }
        }
        sync = 1;
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
        XCBConfigureWindow(_wm.dpy, win, mask, &wc);
        sync = 1;
    }
    if(sync)
    {   XCBFlush(_wm.dpy);
    }
}

void
maprequest(XCBGenericEvent *event)
{
    XCBMapRequestEvent *ev  = (XCBMapRequestEvent *)event;
    const XCBWindow parent  = ev->parent;
    const XCBWindow win     = ev->window;

    (void)parent;

    u8 sync = 0;

    PropListen(_wm.handler, _wm.dpy, win, PropManage);

    if(sync)
    {   XCBFlush(_wm.dpy);
    }
}
/* popup windows sometimes need this */
void
resizerequest(XCBGenericEvent *event)
{
    XCBResizeRequestEvent *ev = (XCBResizeRequestEvent *)event;
    const XCBWindow win = ev->window;
    const u16 w         = ev->width;
    const u16 h         = ev->height;

    
    u8 sync = 0;

    Client *c = wintoclient(win);
    if(c)
    {   
        resize(c, c->x, c->y, w, h, 0);
        if(!ISFLOATING(c) && !DOCKED(c))
        {   setfloating(c, 1);
        }
        sync = 1;
    }
    else
    {   
        XCBResizeWindow(_wm.dpy, win, w, h);
        sync = 1;   /* we dont technically need to sync here but its just to catch up on somethings if we fucked up */
    }

    if(sync)
    {   XCBFlush(_wm.dpy);
    }
}

void
circulatenotify(XCBGenericEvent *event)
{
    XCBCirculateNotifyEvent *ev = (XCBCirculateNotifyEvent *)event;
    const u8 place                    = ev->place;
    const XCBWindow win               = ev->window;
    const XCBWindow eventwin          = ev->event;

    (void)place;
    (void)win;
    (void)eventwin;
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

    (void)eventwin;
    (void)abovesibling;
    (void)x;
    (void)y;
    (void)borderwidth;

    u8 sync = 0;
    if(win == _wm.root)
    {
        u8 dirty;
        dirty = (_wm.sw != w || _wm.sh != h);
        
        _wm.sw = w;
        _wm.sh = h;


        Debug("(w: %d, h: %d)", w, h);
        if(updategeom() || dirty)
        {
            Monitor *m;
            /* update the bar */
            for(m = _wm.mons; m; m = nextmonitor(m))
            {
                if(m->bar && m->bar->win)
                {   XCBMoveResizeWindow(_wm.dpy, m->bar->win, m->wx, m->bar->y, m->ww, m->bar->h);
                }
            }
            arrangemons();
            sync = 1;
        }
    }
    else if(overrideredirect)
    {   /* Who Cares? */
    }
    if(sync)
    {   XCBFlush(_wm.dpy);
    }
}

void
createnotify(XCBGenericEvent *event)
{
    XCBCreateNotifyEvent *ev = (XCBCreateNotifyEvent *)event;
    const u8 overrideredirect   = ev->override_redirect;
    const XCBWindow win         = ev->window;
    const XCBWindow parentwin   = ev->parent;
    const i16 x                 = ev->x;
    const i16 y                 = ev->y;
    const u16 w                 = ev->width;
    const u16 h                 = ev->height;
    const u16 bw                = ev->border_width;


    (void)overrideredirect;
    (void)win;
    (void)parentwin;
    (void)x;
    (void)y;
    (void)w;
    (void)h;
    (void)bw;
}

void
destroynotify(XCBGenericEvent *event)
{
    XCBDestroyNotifyEvent *ev = (XCBDestroyNotifyEvent *)event;
    const XCBWindow win         = ev->window;
    const XCBWindow eventwin    = ev->event;        /* The Event win is the window that sent the message */

    (void)eventwin;

    u8 sync = 0;

    Client *c = wintoclient(win);

    if(c)
    {   
        u32 sticky = ISSTICKY(c);
        Desktop *desk = c->desktop;
        unmanage(c, 1);
        /* if desktop was selected re arrange (no need to waste resources if not visbile) */
        if(desk->mon->desksel == desk || sticky)
        {
            focus(NULL);
            arrange(desk);
        }
        /* unused due to race conditions */
        /* PropListen(_wm.handler, _wm.dpy, win, PropUnmanage); */
        sync = 1;
    }

    if(sync)
    {   XCBFlush(_wm.dpy);
    }
}

void
gravitynotify(XCBGenericEvent *event)
{
    XCBGravityNotifyEvent *ev = (XCBGravityNotifyEvent *)event;
    const XCBWindow eventwin          = ev->event;
    const XCBWindow win               = ev->window;
    const i16 x                       = ev->x;
    const i16 y                       = ev->y;

    (void)eventwin;
    (void)win;
    (void)x;
    (void)y;
    /* TODO? Maybe add Update Sizehints Not sure how this works and exactly. */
}

void
mapnotify(XCBGenericEvent *event)
{
    XCBMapNotifyEvent *ev = (XCBMapNotifyEvent *)event;
    const XCBWindow win             = ev->window;
    const XCBWindow eventwin        = ev->event;
    const uint8_t override_redirect = ev->override_redirect;

    (void)win;
    (void)eventwin;
    (void)override_redirect;
}

void
mappingnotify(XCBGenericEvent *event)
{
    XCBMappingNotifyEvent *ev = (XCBMappingNotifyEvent *)event;
    const XCBKeyCode first_keycode = ev->first_keycode;
    const uint8_t count            = ev->count;
    const uint8_t request          = ev->request;

    (void)count;
    (void)first_keycode;

    XCBRefreshKeyboardMapping(_wm.syms, ev);
    /* update the mask */
    updatenumlockmask();
    if(request == XCB_MAPPING_KEYBOARD)
    {   grabkeys();
    }
    else if(request == XCB_MAPPING_POINTER)
    {
    }
    XCBFlush(_wm.dpy);
}

void
unmapnotify(XCBGenericEvent *event)
{
    XCBUnmapNotifyEvent *ev = (XCBUnmapNotifyEvent *)event;
    const XCBWindow eventwin    = ev->event;
    const XCBWindow win         = ev->window;
    const uint8_t isconfigure   = ev->from_configure;

    (void)eventwin;
    (void)isconfigure;

    if(isconfigure)
    {   
        Debug0("Window unmapped, but will be remaped, AKA: FROM_CONFIGURE");
        return;
    }

    u8 sync = 0;

    Client *c = wintoclient(win);

    if(c)
    {   
        u32 sticky = ISSTICKY(c);
        Desktop *desk = c->desktop;
        unmanage(c, 1);
        /* if desktop was selected re arrange (no need to waste resources if not visbile) */
        if(desk->mon->desksel == desk || sticky)
        {
            focus(NULL);
            arrange(desk);
        }
        /* unused due to race conditions */
        /* PropListen(_wm.handler, _wm.dpy, win, PropUnmanage); */
        sync = 1;
    }

    if(sync)
    {   XCBFlush(_wm.dpy);
    }
}

void
visibilitynotify(XCBGenericEvent *event)
{
    XCBVisibilityNotifyEvent *ev = (XCBVisibilityNotifyEvent *)event;
    const XCBWindow win         = ev->window;
    const u8 state              = ev->state;

    (void)win;

    switch(state)
    {
        case XCB_VISIBILITY_UNOBSCURED:
            Debug("Viewable [%u]", win);
            break;
        case XCB_VISIBILITY_FULLY_OBSCURED:
            Debug("Obscured [%u]", win);
            break;
        case XCB_VISIBILITY_PARTIALLY_OBSCURED:
            Debug("Visible. [%u]", win);
            break;
        default:
            Debug0("Invalid visiblity.");
    }
}

void
reparentnotify(XCBGenericEvent *event)
{
    XCBReparentNotifyEvent *ev = (XCBReparentNotifyEvent *)event;
    const XCBWindow parent      = ev->parent;
    const XCBWindow win         = ev->window;
    const XCBWindow eventwin    = ev->event;
    const i16 x                 = ev->x;
    const i16 y                 = ev->y;
    const u8 override_redirect  = ev->override_redirect;

    (void)parent;
    (void)win;
    (void)eventwin;
    (void)x;
    (void)y;
    (void)override_redirect;

    Debug0("Reparent notify shenanigans occuring");
}

void
colormapnotify(XCBGenericEvent *event)
{
    XCBColormapNotifyEvent *ev = (XCBColormapNotifyEvent *)event;
    const XCBWindow win         = ev->window;
    const u8 state              = ev->state;
    const XCBColormap colormap  = ev->colormap;
    const u8 new                = ev->_new;

    (void)win;
    (void)state;
    (void)colormap;
    (void)new;
}

/* TODO */
void
clientmessage(XCBGenericEvent *event)
{
    XCBClientMessageEvent *ev = (XCBClientMessageEvent *)event;
    const XCBWindow win             = ev->window;
    const XCBAtom atom              = ev->type;
    const u8 format                 = ev->format;
    const XCBClientMessageData data = ev->data;     /* union "same" as xlib data8 -> b[20] data16 -> s[10] data32 = l[5] */


    /* move resize */
    enum
    {
        _NET_WM_MOVERESIZE_SIZE_TOPLEFT,
        _NET_WM_MOVERESIZE_SIZE_TOP,
        _NET_WM_MOVERESIZE_SIZE_TOPRIGHT,
        _NET_WM_MOVERESIZE_SIZE_RIGHT,
        _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT,
        _NET_WM_MOVERESIZE_SIZE_BOTTOM,
        _NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT,
        _NET_WM_MOVERESIZE_SIZE_LEFT,
        _NET_WM_MOVERESIZE_MOVE,            /* movement only */
        _NET_WM_MOVERESIZE_SIZE_KEYBOARD,   /* size via keyboard */
        _NET_WM_MOVERESIZE_MOVE_KEYBOARD,   /* move via keyboard */
        _NET_WM_MOVERESIZE_CANCEL,          /* cancel operation */
    };

    /* These cover most of the important message's */
    /*
     * _NET_NUMBER_OF_DESKTOPS
     * _NET_DESKTOP_GEOMETRY
     * _NET_DESKTOP_VIEWPORT
     * _NET_CURRENT_DESKTOP
     * _NET_ACTIVE_WINDO
     * _NET_SHOWING_DESKTOP
     * _NET_CLOSE_WINDOW
     * _NET_MOVERESIZE_WINDOW
     * _NET_WM_MOVERESIZE
     * _NET_RESTACK_WINDOW
     * _NET_REQUEST_FRAME_EXTENTS
     * _NET_WM_DESKTOP
     * 
     *
     * _NET_WM_STATE
     * _NET_WM_WINDOW_TYPE
     * 
     *
     * _NET_WM_FULLSCREEN_MONITORS
     * 
     * subtypes:
     *
     * WM_PROTOCOLS:
     *      data.l[0]       _NET_WM_PING
     *      data.l[0]       _NET_WM_SYNC_REQUEST
     */

    if(format != 32)
    {   return;
    }

    u8 sync = 0;
    Client *c = wintoclient(win);
    if(c)
    {
        /* long data is often used and anything else is just padding */
        const i32 l0 = data.data32[0];
        const i32 l1 = data.data32[1];
        const i32 l2 = data.data32[2];
        const i32 l3 = data.data32[3];
        const i32 l4 = data.data32[4];
        
        /* ICCCM */
        if(atom == wmatom[WMChangeState])
        {
            /* Ok, barely any sensible app does this that isnt something like wine, so yeah basic handling */
            const enum XCBWMWindowState state = l0;
            const u32 neverfocus = NEVERFOCUS(c);
            const u32 inputflags = neverfocus ? XCB_WM_HINT_INPUT : 0;
            u32 wasvisible = ISVISIBLE(c);
            XCBWMHints wmh = 
            {
                .flags = 0|XCB_WM_HINT_STATE|inputflags,
                .input = !neverfocus,
                .initial_state = state
            };
            updatewmhints(c, &wmh);
            /* arrange if needed, else hide if needed */
            if((wasvisible && !ISVISIBLE(c)) || (!wasvisible && ISVISIBLE(c)))
            {   
                showhide(c);
                arrange(c->desktop);
            }
        }

        /* NET_WM */
        if(atom == netatom[NetWMState])
        {
            const u8 action = l0;   /* remove: 0 
                                     * add: 1 
                                     * toggle: 2 
                                     */
            const XCBAtom prop1 = l1;
            const XCBAtom prop2 = l2;
            updatewindowstate(c, prop1, action);
            updatewindowstate(c, prop2, action);
            sync = 1;
        }
        else if(atom == netatom[NetActiveWindow])
        {
            if(c->desktop->sel != c)
            {   
                focus(c);
                arrange(c->desktop);
                sync = 1;
            }
        }
        else if(atom == netatom[NetCloseWindow])
        {   
            killclient(c, Graceful);
            sync = 1;
        }
        else if(atom == netatom[NetMoveResize])
        {
            const int netwmstate = l2;
            /* some apps decided that they wanna be funny and fuck things up so this check prevents that */
            const i32 button = CLEANBUTTONMASK(l3);
            XCBButtonPressEvent bev;
            bev.state = SUPER;
            bev.root = _wm.root;
            bev.time = XCB_CURRENT_TIME;
            bev.child = 0;
            bev.event = win;
            bev.detail = button;
            bev.root_x = l0;
            bev.root_y = l1;
            bev.event_x = 0;
            bev.event_y = 0;
            bev.sequence = 0;
            bev.same_screen = 1;
            Arg arg;
            arg.v = &bev;
            /* TODO */
            switch(netwmstate)
            {
                case _NET_WM_MOVERESIZE_SIZE_TOPLEFT:
                case _NET_WM_MOVERESIZE_SIZE_TOP:
                case _NET_WM_MOVERESIZE_SIZE_TOPRIGHT:
                case _NET_WM_MOVERESIZE_SIZE_RIGHT:
                case _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT:
                case _NET_WM_MOVERESIZE_SIZE_BOTTOM:
                case _NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT:
                case _NET_WM_MOVERESIZE_SIZE_LEFT:
                    ResizeWindow(&arg);
                    break;
                case _NET_WM_MOVERESIZE_MOVE:
                    DragWindow(&arg);
                    break;
                case _NET_WM_MOVERESIZE_SIZE_KEYBOARD: 
                    break;
                case _NET_WM_MOVERESIZE_MOVE_KEYBOARD: 
                    break;
                /* Not sure where the race condition occurs?? */
                case _NET_WM_MOVERESIZE_CANCEL: 
                    break;
            }
            sync = 1;
        }
        else if(atom == netatom[NetMoveResizeWindow])
        {
            Debug0("NetMoveResizeWindow 'ed");
            /* specified as first 7 bits: 
             * https://specifications.freedesktop.org/wm-spec/latest/ar01s04.html
             */
            const u8 GRAVITY_BITS = UINT8_MAX - 1;
            const u32 FLAG_BITS = ~(GRAVITY_BITS);
            const i32 x = l1;
            const i32 y = l2;
            const i32 w = l3;
            const i32 h = l4;
            const u16 __NET_WM_MOVE_WINDOW_X = 1 << 8;
            const u16 __NET_WM_MOVE_WINDOW_Y = 1 << 9;
            const u16 __NET_WM_MOVE_WINDOW_WIDTH = 1 << 10;
            const u16 __NET_WM_MOVE_WINDOW_HEIGHT = 1 << 11;
            const u16 __NET_WM_MOVE_WINDOW_APPLICATION_FLAG = 1 << 12;
            const u16 __NET_WM_MOVE_WINDOW_PAGER_FLAG = 1 << 13;

            (void)__NET_WM_MOVE_WINDOW_APPLICATION_FLAG;
            (void)__NET_WM_MOVE_WINDOW_PAGER_FLAG;

            const enum XCBBitGravity gravity = l0 & GRAVITY_BITS;

            const u32 flags = l0 & FLAG_BITS;

            u32 tmpgravity = c->gravity;
            if(c->gravity != gravity)
            {   c->gravity = gravity;
            }

            u32 mask = 0;
            if(flags & __NET_WM_MOVE_WINDOW_X)
            {   mask |= XCB_CONFIG_WINDOW_X;
            }
            if(flags & __NET_WM_MOVE_WINDOW_Y)
            {   mask |= XCB_CONFIG_WINDOW_Y;
            }
            if(flags & __NET_WM_MOVE_WINDOW_WIDTH)
            {   mask |= XCB_CONFIG_WINDOW_WIDTH;
            }
            if(flags & __NET_WM_MOVE_WINDOW_HEIGHT)
            {   mask |= XCB_CONFIG_WINDOW_HEIGHT;
            }

            XCBGenericEvent _ev;
            XCBConfigureRequestEvent *gev = (XCBConfigureRequestEvent *)&_ev;
            gev->x = x;
            gev->y = y;
            gev->width = w;
            gev->height = h;
            gev->value_mask = mask;
            /* Should automatically flush. */ 
            configurerequest(&_ev);
            /* revert back to old gravity. */
            c->gravity = tmpgravity;
        }
        else if(atom == netatom[NetRestackWindow])
        {   
            /* "and therefore the Window Manager should always obey it. "
             * Doesnt seem very safe.
             */
            XCBWindow sibling = l1;
            u8 detail = l2;
            /* todo figure out what this does */
            (void)detail;
            XCBGenericEvent gev;
            memset(&gev, 0, sizeof(XCBGenericEvent));
            XCBConfigureRequestEvent *config = (XCBConfigureRequestEvent *)&gev;
            config->window = sibling;
            config->parent = c->win;
            config->value_mask = XCB_CONFIG_WINDOW_SIBLING | XCB_CONFIG_WINDOW_STACK_MODE;
            configurerequest((XCBGenericEvent *)&gev);
            sync = 1;
        }
        else if(atom == netatom[NetRequestFrameExtents])
        {   
            /* TODO Implement getframeextents() macro.
             * See comment on top setshowdecor().
             */
            const u32 _data[4] = { 0, 0, 0, 0 };
            XCBChangeProperty(_wm.dpy, c->win, netatom[NetRequestFrameExtents], XCB_ATOM_CARDINAL, 32, XCB_PROP_MODE_REPLACE, (unsigned char *)_data, 4);
        }
        else if (atom == netatom[NetNumberOfDesktops])
        {   /* ignore */
        }
        else if (atom == netatom[NetDesktopGeometry])
        {   /* ignore */
        }
        else if (atom == netatom[NetDesktopViewport])
        {   /* ignore */
        }
        else if (atom == netatom[NetCurrentDesktop])
        {   
            u32 target = l0;
            Monitor *m = c->desktop->mon;
            if(m)
            {
                Desktop *desk;
                u32 i = 0;
                for(desk = m->desktops; desk && i != target; desk = nextdesktop(desk), ++i);
                if(desk)
                {   
                    /* prevent focus/stack breaking if its already there */
                    if(c->desktop != desk)
                    {   
                        setclientdesktop(c, desk);
                        sync = 1;
                    }
                }
                else
                {   Debug0("Desktop was not in range defaulting to no desktop change.");
                }
            }
        }
        else if (atom == netatom[NetShowingDesktop])
        {   /* TODO
             * Prob just reserve a desktop and just warp to it/warp back. (of course with no clients.)
             */
        }
        else if (atom == netatom[NetWMDesktop])
        {
            /* refer: https://specifications.freedesktop.org/wm-spec/latest/ _NET_WM_DESKTOP */
            if(checksticky(l0))
            {   
                setsticky(c, 1);
                sync = 1;
            }
            else
            {
                u32 target = l0;
                Monitor *m = c->desktop->mon;
                if(m)
                {
                    Desktop *desk;
                    u32 i = 0;
                    for(desk = m->desktops; desk && i != target; desk = nextdesktop(desk), ++i);
                    if(desk)
                    {   
                        /* prevent focus/stack breaking if its already there */
                        if(c->desktop != desk)
                        {   
                            setclientdesktop(c, desk);
                            sync = 1;
                        }
                    }
                    else
                    {   Debug0("Desktop was not in range defaulting to no desktop change.");
                    }
                }
            }
        }
        else if (atom == netatom[WMProtocols])
        {   
            XCBAtom _atom = l0;
            XCBTimestamp _time = l1;
            (void)_time;
            XCBWMProtocols proto;
            proto.atoms = &_atom;
            proto.atoms_len = 1;
            updatewindowprotocol(c, &proto);
            sync = 1;
        }
        else if (atom == netatom[NetWMFullscreenMonitors])
        {   /* TODO */
        }
    }
    if(sync)
    {   XCBFlush(_wm.dpy);
    }
}

void
propertynotify(XCBGenericEvent *event)
{
    XCBPropertyNotifyEvent *ev = (XCBPropertyNotifyEvent *)event;
    const XCBAtom atom          = ev->atom;
    const XCBWindow win         = ev->window;
    const XCBTimestamp timestamp= ev->time;
    const u16 state             = ev->state;

    (void)timestamp;

    if(state == XCB_PROPERTY_DELETE)
    {   return;
    }

    enum PropertyType type = PropNone;
    switch(atom)
    {
        case XCB_ATOM_WM_TRANSIENT_FOR:
            type = PropTransient;
            break;
        case XCB_ATOM_WM_HINTS:
            type = PropWMHints;
            break;
        case XCB_ATOM_WM_NORMAL_HINTS:
            type = PropSizeHints;
            break;
        case XCB_ATOM_WM_NAME:
            type = PropWMName;
            break;
        case XCB_ATOM_WM_ICON_NAME:
            /* ignore */
            return;
        case XCB_ATOM_WM_CLASS:
            type = PropWMClass;
            break;
        case XCB_ATOM_WM_CLIENT_MACHINE:
            /* ignore */
            break;
        default:
            /* other atoms */
            if(atom == motifatom)
            {   type = PropMotifHints;
            }
            else if(atom == netatom[NetWMName])
            {   type = PropNetWMName;
            }
            else if(atom == netatom[NetWMWindowType])
            {   type = PropWindowType;
            }
            else if(atom == netatom[NetWMState])
            {   type = PropWindowState;
            }
            else if(atom == wmatom[WMProtocols])
            {   type = PropWMProtocol;
            }
            else if(atom == netatom[NetWMStrut])
            {   type = PropStrut;
            }
            else if(atom == netatom[NetWMStrutPartial])
            {   type = PropStrutp;
            }
            else if(atom == netatom[NetWMPid])
            {   type = PropPid;
            }
            else if(atom == netatom[NetWMIcon])
            {   type = PropIcon;
            }
            else
            {
                /* not found return */
                return;
            }
    }
    /* encase we fuck up later down the line */
    if(type == PropNone || type >= PropLAST)
    {   
        Debug0("Prop is invalid FIXME");
        return;
    }
    PropListen(_wm.handler, _wm.dpy, win, type);
}

void
selectionclear(XCBGenericEvent *event)
{
    XCBSelectionClearEvent *ev = (XCBSelectionClearEvent *)event;
    const XCBWindow owner       = ev->owner;
    const XCBAtom selection     = ev->selection;
    const XCBTimestamp tim      = ev->time;

    (void)owner;
    (void)selection;
    (void)tim;
}

void
selectionnotify(XCBGenericEvent *event)
{
    XCBSelectionNotifyEvent *ev     = (XCBSelectionNotifyEvent *)event;
    const XCBWindow requestor       = ev->requestor;
    const XCBAtom property          = ev->property;
    const XCBAtom target            = ev->target;
    const XCBAtom selection         = ev->selection;
    const XCBTimestamp tim          = ev->time;

    (void)requestor;
    (void)property;
    (void)target;
    (void)selection;
    (void)tim;
}

void
selectionrequest(XCBGenericEvent *event)
{
    XCBSelectionRequestEvent *ev = (XCBSelectionRequestEvent *)event;
    const XCBWindow owner       = ev->owner;
    const XCBWindow requestor   = ev->requestor;
    const XCBAtom property      = ev->property;
    const XCBAtom target        = ev->target;
    const XCBAtom selection     = ev->selection;
    const XCBTimestamp tim      = ev->time;

    (void)owner;
    (void)requestor;
    (void)property;
    (void)target;
    (void)selection;
    (void)tim;
}



void
genericevent(XCBGenericEvent *event)
{
}

void
errorhandler(XCBGenericEvent *event)
{   xerror(_wm.dpy, (XCBGenericError *)event);
}
