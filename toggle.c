#include <unistd.h>
#include <signal.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <string.h>

#include <pthread.h>

#include "util.h"
#include "dwm.h"
#include "xcb_trl.h"
#include "config.h"
#include "toggle.h"

/*
 * For people wanting to make new functions:
 * XCB buffers requests to the display (for some of them not all (which is dumb btw.)) so you just do a bunch of stuff then when your done just do XCBFlush();
 */

/* TODO: Make these functions seperate threads */
extern void (*handler[]) (XCBGenericEvent *);

extern WM _wm;
extern CFG _cfg;

void
UserStats(const Arg *arg)
{
    char *str = XCBDebugGetCallStack();
    int i = 0;
    while(str[i])
    {
        fprintf(stderr, "%c", str[i]);
        if(str[i] == '\0')
        {   break;
        }
        ++i;
    }
    free(str);
}

void
FocusMonitor(const Arg *arg)
{
    Monitor *m;
    if(!_wm.mons)
    {   
        DEBUG0("There are no monitors, this should not be possible.");
        return;
    }
    if(!_wm.mons->next)
    {   DEBUG0("There is no other monitor to focus.");
    }

    if(!_wm.selmon)
    {   DEBUG0("No monitor selected in Context, this should not be possible");
    }

    if((m = dirtomon(arg->i)) == _wm.selmon)
    {   return;
    }

    if(_wm.selmon->desksel->sel)
    {   unfocus(_wm.selmon->desksel->sel, 0);
    }
    _wm.selmon = m;
    focus(NULL);
}

void
ChangeMasterWindow(const Arg *arg)
{
}

void
KillWindow(const Arg *arg)
{
    if(_wm.selmon->desksel->sel)
    {
        killclient(_wm.selmon->desksel->sel->win, Graceful);
    }
}

void
TerminateWindow(const Arg *arg)
{
    if(_wm.selmon->desksel->sel)
    {
        killclient(_wm.selmon->desksel->sel->win, Destroy);
    }
}

void
DragWindow(
    XCBDisplay *display, 
    XCBWindow win,
    const XCBKeyCode key_or_button)
{
    i16 ox, oy;     /*    old   */
    i16 x, y;       /*  current */
    i16 nx, ny;     /*    new   */

    XCBCookie gbpcookie = 
        XCBGrabPointerCookie(display, win, False, MOUSEMASK, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, XCB_NONE, XCB_NONE, XCB_CURRENT_TIME);
    XCBCookie qpcookie = XCBQueryPointerCookie(display, win);


    XCBGrabPointer *grb = XCBGrabPointerReply(display, gbpcookie);
    XCBQueryPointer *qp = XCBQueryPointerReply(display, qpcookie);


    if(!grb || grb->status != XCB_GRAB_STATUS_SUCCESS)
    {   
        if(grb)
        {   free(grb);
        }
        return;
    }
    if(!qp)
    {
        free(grb);
        return;
    }

    x = qp->root_x;
    y = qp->root_y;
    ox = qp->win_x;
    oy = qp->win_y;

    free(grb);
    free(qp);

    XCBMotionNotifyEvent *ev;
    u8 cleanev;
    u8 detail = 0;
    do
    {
        ev = (XCBMotionNotifyEvent *)XCBPollForEvent(display);
        cleanev = XCB_EVENT_RESPONSE_TYPE(ev);

        if(ev->event == win && cleanev == XCB_MOTION_NOTIFY)
        {
            nx = ox + (ev->root_x - x);
            ny = oy + (ev->root_y - y);
            XCBMoveWindow(display, win, nx, ny);
        }
        if(cleanev == XCB_BUTTON_RELEASE)
        {   
            detail = ev->detail;
        }
        free(ev);
    } while(cleanev != 0 && cleanev != XCB_BUTTON_RELEASE && detail != key_or_button);
}

void
Restart(const Arg *arg)
{
    restart();
    quit();
}

void
Restartf(const Arg *arg)
{
    restart();
}

void
Quit(const Arg *arg)
{
    quit();
}

void
_ResizeWindow(
    XCBDisplay *display, 
    XCBWindow win,
    const XCBKeyCode key_or_button
    ) /* resizemouse */
{
    i16 ox, oy;     /*    old   */
    i16 x, y;       /*  current */
    i16 nx, ny;     /*    new   */

    u16 ow, oh;     /*    old   */
    u16 nw, nh;     /*    new   */

    i8 horiz;       /* bounds checks    */
    i8 vert;        /* bounds checks    */

    const XCBCursor cur = XCB_NONE;
    XCBCookie qpcookie = XCBQueryPointerCookie(display, win);
    XCBCookie gpcookie = XCBGrabPointerCookie(display, win, False, MOUSEMASK, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, XCB_NONE, cur, XCB_CURRENT_TIME);
    XCBCookie gmcookie = XCBGetGeometryCookie(display, win);

    XCBQueryPointer *qp = XCBQueryPointerReply(display, qpcookie);
    XCBGrabPointer *gb = XCBGrabPointerReply(display, gpcookie);
    XCBGeometry *gm = XCBGetGeometryReply(display, gmcookie);

    if(!qp)
    {   DEBUG0("Could not query pointer.");
        return;
    }
    if(!gb || gb->status != XCB_GRAB_STATUS_SUCCESS)
    {   free(qp);
        DEBUG0("Could not grab pointer.");
        return;
    }
    if(!gm)
    {   
        free(qp); 
        free(gb);
        DEBUG0("No Geometry avaible.");
        return;
    }
    XCBSelectInput(_wm.dpy, win, 
            XCB_EVENT_MASK_POINTER_MOTION|XCB_EVENT_MASK_BUTTON_PRESS|XCB_EVENT_MASK_BUTTON_RELEASE|
            XCB_EVENT_MASK_KEY_PRESS|XCB_EVENT_MASK_KEY_RELEASE
            );

    horiz = qp->win_x < (gm->width / 2) ? -1 : 1;
    vert = qp->win_y < (gm->height / 2) ? -1 : 1;

    x = qp->win_x;
    y = qp->win_y; 

    ox = gm->x;
    oy = gm->y;

    ow = gm->width;
    oh = gm->height;


    XCBMotionNotifyEvent *ev;
    u8 cleanev = 0;
    XCBKeyCode detail = 0;
    XCBFlush(_wm.dpy);
    do
    {
        ev = (XCBMotionNotifyEvent *)XCBPollForEvent(display);
        if(!ev)
        {   continue;
        }
        cleanev = XCB_EVENT_RESPONSE_TYPE(ev);
        if(ev->event == win && cleanev == XCB_MOTION_NOTIFY)
        {
            nw = ow + (horiz    * (ev->root_x - x));
            nh = oh + (vert     * (ev->root_y - y));
            nx = ox + (!~horiz) * (ow - nw);
            ny = oy + (!~vert)  * (oh - nh);
            XCBMoveResizeWindow(display, win, nx, ny, nw, nh);
            DEBUG("(w: %d, h: %d)", nw, nh);
        }
        else
        {   eventhandler((XCBGenericEvent *)ev);
        }
        detail = ev->detail;
        free(ev);
        ev = NULL;
    } while((cleanev != XCB_BUTTON_RELEASE || cleanev != XCB_KEY_PRESS) || detail != key_or_button);
}

void
ResizeWindow(const Arg *arg)
{
    if(_wm.selmon->desksel->sel)
    {
        _ResizeWindow(_wm.dpy, _wm.selmon->desksel->sel->win, arg->i);
        DEBUG0("Done.");
    }
}

void
SetBorderWidth(const Arg *arg)
{
    if(_cfg.bw + arg->i < 0)
    {   return;
    }
    _cfg.bw += arg->i;
    Client *c;
    for(c = _wm.selmon->desksel->clients; c; c = nextclient(c))
    {   
        XCBSetWindowBorderWidth(_wm.dpy, c->win, _cfg.bw);
        setborderwidth(c, _cfg.bw);
    }
    arrangedesktop(_wm.selmon->desksel);
    XCBFlush(_wm.dpy);
    DEBUG("Border width: [%d]", _cfg.bw);
}

void
SetWindowLayout(const Arg *arg)
{
    const Monitor *m = _wm.selmon;

    if(!m) return;
    setdesktoplayout(m->desksel, arg->i);
    arrange(m->desksel);
}

void
SetMonitorFact(const Arg *arg)
{
}

void
SpawnWindow(const Arg *arg)
{
    /* TODO: trying to spawn lemonbar fails for some reason? */
    struct sigaction sa;
    if (fork() == 0)
    {
        if (_wm.dpy)
            close(XCBConnectionNumber(_wm.dpy));
        setsid();
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_handler = SIG_DFL;
        sigaction(SIGCHLD, &sa, NULL);
        execvp(((char **)arg->v)[0], (char **)arg->v);
        /* UNREACHABLE */
        DEBUG("%s", "execvp Failed");
    }
}

void
MaximizeWindow(const Arg *arg)
{
}

void
MaximizeWindowVertical(const Arg *arg) 
{
}

void
MaximizeWindowHorizontal(const Arg *arg) 
{
}

void
AltTab(const Arg *arg)
{
}

void
ToggleStatusBar(const Arg *arg)
{
    const Monitor *m = _wm.selmon;
    if(!m || !m->bar)
    {   return;
    }
    setshowbar(m->bar, !SHOWBAR(m->bar));
    updatebarpos(_wm.selmon);
    XCBMoveResizeWindow(_wm.dpy, _wm.selmon->bar->win, _wm.selmon->wx, _wm.selmon->bar->y, _wm.selmon->ww, _wm.selmon->bar->h);
    arrange(_wm.selmon->desksel);
    XCBFlush(_wm.dpy);
}

void
ToggleFullscreen(const Arg *arg)
{
}


void 
View(const Arg *arg)
{
}

