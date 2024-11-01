#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <unistd.h>
#include <signal.h>
#include <locale.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_atom.h>
#include <xcb/xproto.h>
#include <xcb/xkb.h>
#include <xcb/xinerama.h>

#include <X11/cursorfont.h>     /* cursors */

/* keycodes */
#include <X11/keysym.h>

#include "util.h"
#include "argcv.h"
#include "main.h"
#include "hashing.h"
#include "getprop.h"
#include "session.h"
#include "keybinds.h"
/* for HELP/DebugGING see under main() or the bottom */

extern void (*handler[XCBLASTEvent]) (XCBGenericEvent *);


WM _wm;
UserSettings _cfg;

XCBAtom netatom[NetLast];
XCBAtom wmatom[WMLast];
XCBAtom gtkatom[GTKLAST];
XCBAtom motifatom;
XCBAtom xembedatom[XEMBEDLAST];
XCBCursor cursors[CurLast];

static uint32_t 
__intersect_area(
        /* rect 1 */
        int32_t x1, 
        int32_t y1, 
        int32_t x2, 
        int32_t y2,
        /* rect 2 */
        int32_t x3,
        int32_t y3,
        int32_t x4,
        int32_t y4
        )
{
    const int32_t xoverlap = MAX(0, MIN(x2, x4) - MAX(x1, x3));
    const int32_t yoverlap = MAX(0, MIN(y2, y4) - MAX(y1, y3));
    const uint32_t area = xoverlap * yoverlap;
    return area;
}

enum BarSides GETBARSIDE(Monitor *m, Client *bar, uint8_t get_prev)
                                { 
                                    const float LEEWAY = .15f;
                                    const float LEEWAY_SIDE = .35f;

                                    /* top parametors */
                                    const i32 TOP_MIN_X = m->mx;
                                    const i32 TOP_MIN_Y = m->my;

                                    const i32 TOP_MAX_X = m->mx + m->mw;
                                    const i32 TOP_MAX_Y = m->my + (m->mh * LEEWAY);

                                    /* bottom parametors */
                                    const i32 BOTTOM_MIN_X = m->mx;
                                    const i32 BOTTOM_MIN_Y = m->my + m->mh - (m->mh * LEEWAY);

                                    const i32 BOTTOM_MAX_X = m->mx + m->mw;
                                    const i32 BOTTOM_MAX_Y = m->my + m->mh;

                                    /* sidebar left parametors */
                                    const i32 LEFT_MIN_X = m->mx;
                                    const i32 LEFT_MIN_Y = TOP_MAX_Y;

                                    const i32 LEFT_MAX_X = m->mx + (m->mw * LEEWAY_SIDE);
                                    const i32 LEFT_MAX_Y = BOTTOM_MIN_Y;

                                    /* sidebar right parametors */
                                    const i32 RIGHT_MIN_X = m->mx + m->mw - (m->mw * LEEWAY_SIDE);
                                    const i32 RIGHT_MIN_Y = TOP_MAX_Y;

                                    const i32 RIGHT_MAX_X = m->mx + m->mw;
                                    const i32 RIGHT_MAX_Y = BOTTOM_MIN_Y;

                                    enum BarSides side;
                                    i32 bx1;
                                    i32 by1;
                                    i32 bx2;
                                    i32 by2;

                                    if(get_prev)
                                    {
                                        bx1 = bar->oldx + (bar->oldw / 2);
                                        by1 = bar->oldy + (bar->oldh / 2);
                                        bx2 = bx1 + bar->oldw;
                                        by2 = by1 + bar->oldh;
                                    }
                                    else
                                    {
                                        bx1 = bar->x + (bar->w / 2);
                                        by1 = bar->y + (bar->h / 2);
                                        bx2 = bx1 + bar->w;
                                        by2 = by1 + bar->h;
                                    }

                                    uint32_t toparea = __intersect_area(
                                            TOP_MIN_X,
                                            TOP_MIN_Y,
                                            TOP_MAX_X,
                                            TOP_MAX_Y,
                                            bx1,
                                            by1,
                                            bx2,
                                            by2
                                            );
                                    uint32_t bottomarea = __intersect_area(
                                            BOTTOM_MIN_X,
                                            BOTTOM_MIN_Y,
                                            BOTTOM_MAX_X,
                                            BOTTOM_MAX_Y,
                                            bx1,
                                            by1,
                                            bx2,
                                            by2
                                            );
                                    uint32_t leftarea = __intersect_area(
                                            LEFT_MIN_X,
                                            LEFT_MIN_Y,
                                            LEFT_MAX_X,
                                            LEFT_MAX_Y,
                                            bx1,
                                            by1,
                                            bx2,
                                            by2
                                            );
                                    uint32_t rightarea = __intersect_area(
                                            RIGHT_MIN_X,
                                            RIGHT_MIN_Y,
                                            RIGHT_MAX_X,
                                            RIGHT_MAX_Y,
                                            bx1,
                                            by1,
                                            bx2,
                                            by2
                                            );

                                    uint32_t biggest = toparea;

                                    if(biggest < bottomarea)
                                    {   biggest = bottomarea;
                                    }

                                    if(biggest < leftarea)
                                    {   biggest = leftarea;
                                    }

                                    if(biggest < rightarea)
                                    {   biggest = rightarea;
                                    }

                                    /* prob should handle the rare change that the area would be the same as another,
                                     * But at that point we should just rework it to use buttonpress last pressed location.
                                     */
                                    if(biggest == toparea)
                                    {   side = BarSideTop;
                                    }
                                    else if(biggest == bottomarea)
                                    {   side = BarSideBottom;
                                    }
                                    else if(biggest == leftarea)
                                    {   side = BarSideLeft;
                                    }
                                    else if(biggest == rightarea)
                                    {   side = BarSideRight;
                                    }
                                    else    /* this is just for compiler ommiting warning */
                                    {   side = BarSideTop;
                                    }

                                    return side;
                                }

uint8_t
checknewbar(Monitor *m, Client *c, uint8_t has_strut_or_strutp)
{
    /* barwin checks */
    u8 checkbar = !m->bar;
    if(checkbar && COULDBEBAR(c, has_strut_or_strutp))
    {   
        setupbar(m, c);
        return 0;
    }
    return 1;
}

void
checkotherwm(void)
{
    XCBGenericEvent *ev = NULL;
    i32 response;
    /* display could sometimes could have events if too slow 
     * (Issue when using nested enviroments like Xephyr.)
     */
    XCBSyncf(_wm.dpy);
    XCBSelectInput(_wm.dpy, XCBRootWindow(_wm.dpy, _wm.screen), XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT);
    XCBSync(_wm.dpy);  /* XCBFlush has different behaviour suprisingly, its undesired though */
    /* XCBPollForEvent calls the XServer itself for the event, So if we get a reply then a type of Window manager must be running */
    if((ev = XCBPollForEvent(_wm.dpy)))
    {   
        response = ev->response_type;
        free(ev);
        XCBCloseDisplay(_wm.dpy);
        if(response == 0) 
        {   DIECAT("%s", "FATAL: ANOTHER WINDOW MANAGER IS RUNNING.");
        }
        /* UNREACHABLE */
        DIECAT("%s", "FATAL: UNKNOWN REPONSE_TYPE");
    }
    /* assuming this isnt a bug we received nothing because we pinged the server for a response and got nothing */
    /* The other edge case is if the display just doesnt work, however this is covered at startup() if(!_wm.dpy) { DIE(msg); } */
}

void
cleanup(void)
{
    /* save setting data. */
    USSave(&_cfg);
    SessionSave();
    savesession();
    PropDestroy(_wm.handler);
    if(!_wm.dpy)
    {
        /* sometimes due to our own lack of competence we can call quit twice and segfault here */
        if(_wm.selmon)
        {
            Debug0("Some data has not been freed exiting due to possible segfault.");
        }
        return;
    }
    /* cleanup cfg */
    USWipe(&_cfg);
    XCBCookie cookie = XCBDestroyWindow(_wm.dpy, _wm.wmcheckwin);
    cleanupcursors();
    XCBDiscardReply(_wm.dpy, cookie);
    XCBSetInputFocus(_wm.dpy, _wm.root, XCB_INPUT_FOCUS_POINTER_ROOT, XCB_CURRENT_TIME);
    XCBDeleteProperty(_wm.dpy, _wm.root, netatom[NetActiveWindow]);
    _wm.wmcheckwin = 0;
    if(_wm.syms)
    {   
        XCBKeySymbolsFree(_wm.syms);
        _wm.syms = NULL;
    }
    cleanupmons();
    XCBFlush(_wm.dpy);
    /* Free hashmap */
    cleanupclienthash();
    if(_wm.dpy)
    {
        XCBCloseDisplay(_wm.dpy);
        if(_wm.use_threads)
        {   pthread_mutex_destroy(&_wm.mutex);
        }
        _wm.dpy = NULL;
    }
}

void __HOT__
eventhandler(XCBGenericEvent *ev)
{
    /* TODO: Remove references to other clients, and move _wm.use_threads to run() instead. */
    if(_wm.use_threads)
    {   pthread_mutex_lock(&_wm.mutex);
    }
    /* int for speed */
    const int cleanev = XCB_EVENT_RESPONSE_TYPE(ev);
    /* Debug("%s", XCBGetEventName(cleanev)); */
    if(LENGTH(handler) > cleanev)
    {   handler[cleanev](ev);
    }

    /* TODO: Remove references to other clients, and move _wm.use_threads to run() instead. */
    if(_wm.use_threads)
    {   pthread_mutex_unlock(&_wm.mutex);
    }
}

void
exithandler(void)
{   
    Debug("%s", "Process Terminated Successfully.");
}

i8
getrootptr(i16 *x, i16 *y)
{
    u8 samescr;

    XCBCookie cookie = XCBQueryPointerCookie(_wm.dpy, _wm.root);
    XCBQueryPointer *reply = XCBQueryPointerReply(_wm.dpy, cookie);

    if(!reply)
    {   return 0;
    }

    *x = reply->root_x;
    *y = reply->root_y;
    samescr = reply->same_screen;
    free(reply);
    return samescr;
}

void
quit(void)
{
    _wm.running = 0;
    wakeupconnection(_wm.dpy, _wm.screen);
    Debug0("Exiting...");
}

static u8
_restore_parser(FILE *file, char *buffer, u16 bufflen)
{
    char *newline = NULL;
    const u8 success = 0;
    const u8 bufftoosmall = 1;
    const u8 error = 2;
    const u8 eof = 3;
    if(fgets(buffer, bufflen, file))
    {
        newline = strchr(buffer, '\n');
        if(!newline)
        {
            if(!feof(file))
            {   
                Debug0("Buffer too small for current line.");
                /* BUFF TOO SMALL */
                return bufftoosmall;
            }
        } /* remove new line char */
        else
        {   *newline = '\0';
        }
        /* Success. */
        return success;
    }
    if(ferror(file))
    {   /* ERROR */
        return error;
    }
    /* EOF */
    return eof;
}

void
restoresession(void)
{
    Monitor *m = NULL;
    Desktop *desk = NULL;

    const char *filename = SESSION_FILE;
    const int MAX_LENGTH_1 = 1024;
    const int offset = sizeof(SESSION_FILE) - 1;
    char buff[MAX_LENGTH_1];
    memset(buff, 0, MAX_LENGTH_1 * sizeof(char));
    strcpy(buff, filename);
    snprintf(buff + offset, MAX_LENGTH_1 - offset, "-%d", getpid());
    FILE *fr = fopen(buff, "r");
    const int MAX_LENGTH = 1024;
    char str[MAX_LENGTH];
    int output = 0;
    u8 ismon = 0;
    u8 isdesk = 0;
    u8 isclient = 0;
    u8 isclients = 0;

    if(!fr)
    {   return;
    }

    enum Causes
    {
        NoError = 0,
        BuffTooSmall = 1,
        err = 2,
        EndOfFile = 3
    };
    while(output != EndOfFile)
    {
        memset(str, 0, MAX_LENGTH);
        output = _restore_parser(fr, str, MAX_LENGTH);
        switch(output)
        {
            case NoError: break;
            case BuffTooSmall:
            case err:
            case EndOfFile: 
            default: continue;
        }

        if(isclient)
        {   
            if(!desk)
            {   continue;
            }
            restoreclientsession(desk, str, MAX_LENGTH);
            isclient = 0;
            isclients = 0;
        }
        if(isclients)
        {
            if(!desk)
            {   continue;
            }
            isclients = restorestacksession(desk, str, MAX_LENGTH);
        }
        if(isdesk)
        {   
            if(!m)
            {   continue;
            }
            desk = restoredesktopsession(m, str, MAX_LENGTH);
            isdesk = 0;
        }

        if(ismon)
        {   
            m = restoremonsession(str, MAX_LENGTH);
            ismon = 0;
        }

        ismon += !strcmp(str, "Monitor.");
        isdesk += !strcmp(str, "Desktop.");
        isclient += !strcmp(str, "Client.");
        isclients += !strcmp(str, "Clients.");
        
    }
    fclose(fr);
    arrangemons();
    /* map all the windows again */
    Client *c;
    for(m = _wm.mons; m; m = nextmonitor(m))
    {
        for(desk = m->desktops; desk; desk = nextdesktop(desk))
        {
            for(c = desk->clients; c; c = nextclient(c))
            {   
                showhide(c);
                XCBMapWindow(_wm.dpy, c->win);
            }
        }
    }
    focus(NULL);
    /* No need to flush run() syncs for us */
    /* XCBFlush(_wm.dpy) */
}

Client *
restoreclientsession(Desktop *desk, char *buff, u16 len)
{
    const u8 SCANF_CHECK_SUM = 13;
    u8 check = 0;

    i32 x, y;
    i32 ox, oy;
    u32 w, h;
    u32 ow, oh;
    XCBWindow WindowId;
    XCBWindow WindowIdFocus;
    XCBWindow WindowIdStack;
    u32 _Flags;
    u32 __EWMHFlag;
    u32 BorderWidth;
    u32 BorderColor;

    x = y = ox = oy = w = h = ow = oh = WindowId = WindowIdFocus = WindowIdStack = BorderWidth = BorderColor = 0;
    _Flags = __EWMHFlag = 0;

    check = sscanf(buff, 
                    "(x: %d, y: %d) (w: %u h: %u)" " "
                    "(ox: %d, oy: %d) (ow: %u oh: %u)" " "
                    "WindowId: %u" " "
                    "BorderWidth: %u" " "
                    "BorderColor: %u" " "
                    "Flags: %u" " "
                    "EWMHFlags: %u" " "
                    ,
                    &x, &y, &w, &h,
                    &ox, &oy, &ow, &oh,
                    &WindowId,
                    &BorderWidth,
                    &BorderColor,
                    &_Flags,
                    &__EWMHFlag
                    );

    Client *cclient = NULL;
    if(check == SCANF_CHECK_SUM)
    {
        cclient = wintoclient(WindowId);
        if(cclient)
        {
            setborderwidth(cclient, BorderWidth);
            setbordercolor32(cclient, BorderColor);
            updateborder(cclient);
            /* some clients break on their output if we resize them (like st),
             * So we dont want to resize its old size
             */
            applysizehints(cclient, &ox, &oy, (i32 *)&ow, (i32 *)&oh, 0);
            applysizehints(cclient, &x, &y, (i32 *)&w, (i32 *)&h, 0);
            cclient->oldx = ox;
            cclient->oldy = oy;
            cclient->oldw = ow;
            cclient->oldh = oh;
            cclient->x = x;
            cclient->y = y;
            cclient->w = w;
            cclient->h = h;
            cclient->flags = _Flags;
            cclient->ewmhflags = __EWMHFlag;
        }
    }
    else
    {   Debug0("FAILED CHECKSUM");
    }

    if(cclient)
    {   Debug("Restored Client: [%u]", cclient->win);
    }
    else if(check != SCANF_CHECK_SUM)
    {   Debug("Failed to parse Client str: \"%s\"", buff);
    }
    else
    {   Debug("Client Not Found: [%u]", WindowId);
    }
    return cclient;
}

Desktop *
restoredesktopsession(Monitor *m, char *buff, u16 len)
{
    const u8 SCANF_CHECK_SUM = 4;
    u8 check = 0;

    unsigned int DesktopLayout;
    unsigned int DesktopOLayout;
    unsigned int DesktopNum;
    XCBWindow DesktopSel;
    DesktopLayout = DesktopOLayout = DesktopNum = DesktopSel = 0;

    check = sscanf(buff, 
                    "DesktopLayout: %u" " "
                    "DesktopOLayout: %u" " "
                    "DesktopNum: %u" " "
                    "DesktopSel: %u" " "
                    ,
                    &DesktopLayout,
                    &DesktopOLayout,
                    &DesktopNum,
                    &DesktopSel
                    );

    if(check == SCANF_CHECK_SUM)
    {
        if(DesktopNum > m->deskcount)
        {   setdesktopcount(m, DesktopNum + 1);
        }
        u16 i;
        Desktop *desk = m->desktops;
        for(i = 0; i < DesktopNum; ++i)
        {   desk = nextdesktop(desk);
        }
        if(desk)
        {
            Client *sel = wintoclient(DesktopSel);
            if(sel && sel->desktop != desk)
            {   setclientdesktop(sel, desk);
            }
            focus(sel);
            setdesktoplayout(desk, DesktopOLayout);
            setdesktoplayout(desk, DesktopLayout);
        }
        Debug("Restored desktop: [%d]", desk ? desk->num : -1);
        return desk;
    }
    else
    {   Debug("Failed to parse Desktop str: \"%s\"", buff);
    }
    return NULL;
}

Monitor *
restoremonsession(char *buff, u16 len)
{
    const u8 SCANF_CHECK_SUM = 7;
    u8 check = 0;

    int x;
    int y;
    unsigned int h;
    unsigned int w;
    XCBWindow BarId;
    unsigned int DeskCount;
    unsigned int DeskSelNum;

    x = y = h = w = BarId = DeskCount = DeskSelNum = 0;

    check = sscanf(buff,
                    "(x: %d, y: %d) (w: %u h: %u)" " "
                    "BarId: %u" " "
                    "DeskCount: %u" " "
                    "DeskSelNum: %u" " "
                    ,
                    &x, &y, &w, &h,
                    &BarId,
                    &DeskCount,
                    &DeskSelNum
                    );
    if(check == SCANF_CHECK_SUM)
    {
        Monitor *pullm = NULL;
        Monitor *target = NULL;
        const u8 errorleeway = 5;
        Monitor *possible[errorleeway];
        int i;
        for(i = 0; i < errorleeway; ++i) { possible[i] = NULL; }
        u8 possibleInterator = 0;

        for(pullm = _wm.mons; pullm; pullm = nextmonitor(pullm))
        {
            if(pullm->mw == w && pullm->mh == h)
            {
                if(pullm->mx == x && pullm->my == y)
                {   target = pullm;
                    break;
                }
                else if(possibleInterator < errorleeway)
                {   possible[possibleInterator++] = pullm;
                }
            }
        }
        for(pullm = _wm.mons; pullm; pullm = nextmonitor(pullm))
        {
            if(BETWEEN(w, pullm->mw + errorleeway, pullm->mh - errorleeway) && BETWEEN(h, pullm->mh + errorleeway, pullm->mh - errorleeway))
            {
                if(possibleInterator < errorleeway)
                {   possible[possibleInterator++] = pullm;
                }
            }
        }
        pullm = target;
        if(!pullm) 
        {
            for(i = 0; i < errorleeway; ++i)
            {
                if(possible[i])
                {
                    if((pullm = recttomon(possible[i]->mx, possible[i]->my, possible[i]->mw, possible[i]->mh)))
                    {   break;
                    }
                }
            }
            if(!pullm)
            {   pullm = possible[0];
            }
        }
        if(pullm)
        {
            Client *b = wintoclient(BarId);
            if(b)
            {   
                if(pullm->bar && pullm->bar != b)
                {   
                    XCBWindow win = pullm->bar->win;
                    unmanage(pullm->bar, 0);
                    PropListen(_wm.handler, _wm.dpy, win, PropManage);
                }
                setupbar(pullm, b);
            }
            /* TODO */
            setdesktopcount(pullm, DeskCount);
            if(DeskSelNum != pullm->desksel->num)
            {
                Desktop *desk;
                for(desk = pullm->desktops; desk && desk->num != DeskSelNum; desk = nextdesktop(desk));
                if(desk)
                {   setdesktopsel(pullm, desk);
                }
            }
            Debug("Restored Monitor: [%p]", (void *)pullm);
        }
        return pullm;
    }
    else
    {   Debug("Failed to parse Monitor str: \"%s\"", buff);
    }
    return NULL;
}

int 
restorestacksession(Desktop *desk, char *buff, uint16_t len)
{
    const u8 SSCANF_CHECK_SUM = 3;
    u8 status = strcmp(buff, "ClientsEnd.");
    if(status)
    {
        XCBWindow client = 0;
        XCBWindow focus = 0;
        XCBWindow stack = 0;
        status = sscanf(buff, 
                            "Client: %u" " "
                            "Focus: %u" " "
                            "Stack: %u" " "
                            ,
                            &client,
                            &focus,
                            &stack
                        );

        if(status == SSCANF_CHECK_SUM)
        {   
            Client *c;
            if((c = wintoclient(client)))
            {   
                setclientdesktop(c, desk);
                Debug("Moving [%u] (Client) to right desktop...", c->win);
            }
            else
            {   Debug0("Could not find client in stack...");
            }
            if((c = wintoclient(focus)))
            {   
                detachfocus(c);
                attachfocus(c);
            }
            if((c = wintoclient(stack)))
            {   
                detachstack(c);
                attachstack(c);
            }
        }
        else
        {   /* TODO: Technically we dont need isclientsend, but having that prevents a "fail" due to strcmp("Client.", buff); happening after/ */
            Debug0("Failed to pass move checksum for client.");
        }
    }
    /* end stream */
    return status;
}

void
restart(void)
{
    _wm.restart = SoftRestart;
    Debug("Flag set %s", M_STRINGIFY(SoftRestart));
}

void
restarthard(void)
{
    _wm.restart = HardRestart;
    Debug("Flag set %s", M_STRINGIFY(HardRestart));
}

void 
run(void)
{
    XCBGenericEvent *ev = NULL;
    XCBSync(_wm.dpy);
    while(_wm.running && !XCBNextEvent(_wm.dpy, &ev))
    {
        eventhandler(ev); 
        free(ev);
        ev = NULL;
    }
    _wm.has_error = XCBCheckDisplayError(_wm.dpy);
}

void
savesession(void)
{
    /* assuming the server is still alive process next ~MAX_EVENT_PROCESS events to see if we should make any final changes before saving */
    if(!_wm.has_error)
    {
        /* This should be enough that any broken clients are ignored for event processing */
        const u16 MAX_EVENT_PROCESS = 1000;
        u8 i = 0;
        XCBGenericEvent *ev = NULL;
        while((ev = XCBPollForEvent(_wm.dpy)) && ++i < MAX_EVENT_PROCESS)
        {
            eventhandler(ev);
            free(ev);
            ev = NULL;
        }
    }
    if(!_wm.restart)
    {   return;
    }
    /* save client data. */
    const char *filename = SESSION_FILE;
    const int MAX_LENGTH = 1024;
    const int offset = sizeof(SESSION_FILE) - 1;
    char buff[MAX_LENGTH];
    memset(buff, 0, MAX_LENGTH * sizeof(char));
    strcpy(buff, filename);
    snprintf(buff + offset, MAX_LENGTH - offset, "-%d", getpid());
    Monitor *m;
    FILE *fw = fopen(buff, "w");
    if(!fw)
    {   Debug0("Failed to alloc FILE(OutOfMemory)");
        return;
    }

    for(m = _wm.mons; m; m = nextmonitor(m))
    {   savemonsession(fw, m);
    }
    fclose(fw);
}

void
saveclientsession(FILE *fw, Client *c)
{
    const char *IDENTIFIER = "Client.";

    fprintf(fw,
            "%s" 
            "\n"
            "(x: %d, y: %d) (w: %u h: %u)" " "
            "(ox: %d, oy: %d) (ow: %u oh: %u)" " "
            "WindowId: %u" " "
            "BorderWidth: %u" " "
            "BorderColor: %u" " "
            "Flags: %u" " "
            "EWMHFlags: %u" " "
            "\n"
            ,
            IDENTIFIER,
            c->x, c->y, c->w, c->h,
            c->oldx, c->oldy, c->oldw, c->oldh,
            c->win,
            c->bw,
            c->bcol,
            c->flags,
            c->ewmhflags
            );
}

void
savedesktopsession(FILE *fw, Desktop *desk)
{
    const char *const IDENTIFIER = "Desktop.";
    const char *const IDENTIFIERCLIENTS = "Clients.";
    const char *const IDENTIFIERCLIENTSEND = "ClientsEnd.";

    fprintf(fw,
            "%s"
            "\n"
            "DesktopLayout: %u" " "
            "DesktopOLayout: %u" " "
            "DesktopNum: %u" " "
            "DesktopSel: %u" " "
            "\n"
            ,
            IDENTIFIER,
            desk->layout,
            desk->olayout,
            desk->num,
            desk->sel ? desk->sel->win : 0
            );
    /* make sure correct order */
    reorder(desk);
    Client *c = desk->clast;
    Client *focus = desk->flast;
    Client *stack = desk->slast;
    fprintf(fw, "%s\n", IDENTIFIERCLIENTS);
    while(c)
    {
        fprintf(fw, 
                "Client: %u" " "
                "Focus: %u" " "
                "Stack: %u" " "
                "\n"
                ,
                c->win,
                focus ? focus->win : 0,
                stack ? stack->win : 0
                );
        c = prevclient(c);
        focus = prevfocus(focus);
        stack = prevstack(stack);
    }
    fprintf(fw, "%s\n", IDENTIFIERCLIENTSEND);
    for(c = desk->clast; c; c = prevclient(c))
    {   saveclientsession(fw, c); 
    }
}

void
savemonsession(FILE *fw, Monitor *m) 
{
    const char *IDENTIFIER = "Monitor.";
    Desktop *desk;
    fprintf(fw,
            "%s"
            "\n"
            "(x: %d, y: %d) (w: %u h: %u)" " "
            "BarId: %u" " "
            "DeskCount: %u" " "
            "DeskSelNum: %u" " "
            "\n"
            ,
            IDENTIFIER,
            m->mx, m->my, m->mw, m->mh,
            m->bar ? m->bar->win : 0,
            m->deskcount,
            m->desksel->num
            );
    for(desk = m->desktops; desk; desk = nextdesktop(desk))
    {   savedesktopsession(fw, desk);
    }
}

/* scan for clients initally */
void
scan(void)
{
    u16 i, num;
    XCBWindow *wins = NULL;
    const XCBCookie cookie = XCBQueryTreeCookie(_wm.dpy, _wm.root);
    XCBQueryTree *tree = NULL;

    if((tree = XCBQueryTreeReply(_wm.dpy, cookie)))
    {
        num = tree->children_len;
        wins = XCBQueryTreeChildren(tree);
        if(wins)
        {
            const size_t countsz = sizeof(XCBCookie ) * num;
            const size_t managecookiesz = sizeof(XCBCookie) * ManageClientLAST;
            const size_t managerepliesz = sizeof(void *) * ManageClientLAST;
            XCBCookie *wa = malloc(countsz);
            XCBCookie *wastates = malloc(countsz);
            XCBCookie *tfh = malloc(countsz);
            XCBCookie *managecookies = malloc(managecookiesz * num);
            void **managereplys = malloc(managerepliesz * num);
            XCBGetWindowAttributes **replies = malloc(sizeof(XCBGetWindowAttributes *) * num);
            XCBGetWindowAttributes **replystates = malloc(sizeof(XCBGetWindowAttributes *) * num);
            XCBWindow *trans = malloc(sizeof(XCBWindow) * num);
            uint64_t index;


            if(!wa || !wastates || !tfh || !managecookies || !managereplys || !replies || !replystates || !trans)
            {   
                free(wa);
                free(wastates);
                free(tfh);
                free(managecookies);
                free(managereplys);
                free(replies);
                free(replystates);
                free(trans);
                free(tree);
                return;
            }

            for(i = 0; i < num; ++i)
            {   
                wa[i] = XCBGetWindowAttributesCookie(_wm.dpy, wins[i]);
                /* this specifically queries for the state which wa[i] might fail to provide */
                wastates[i] = XCBGetWindowPropertyCookie(_wm.dpy, wins[i], wmatom[WMState], 0L, 2L, False, wmatom[WMState]);
                tfh[i] = XCBGetTransientForHintCookie(_wm.dpy, wins[i]);
                index = i * ManageClientLAST;
                managerequest(wins[i], managecookies + index);
            }
            
            uint8_t hastrans = 0;
            /* get them replies back */
            for(i = 0; i < num; ++i)
            {
                replies[i] = XCBGetWindowAttributesReply(_wm.dpy, wa[i]);
                replystates[i] = XCBGetWindowAttributesReply(_wm.dpy, wastates[i]);
                hastrans = XCBGetTransientForHintReply(_wm.dpy, tfh[i], &trans[i]);
                index = ManageClientLAST * i;
                managereplies(managecookies + index, managereplys + index);

                if(!hastrans)
                {   trans[i] = 0;
                }
            }

            /* Manage the Windows */
            for(i = 0; i < num; ++i)
            {
                /* override_redirect only needed to be handled for old windows */
                /* X auto redirects when running wm so no need to do anything else */
                if(replies[i]->override_redirect || trans[i]) 
                {   continue;
                }
                index = ManageClientLAST * i;
                if(replies[i] && replies[i]->map_state == XCB_MAP_STATE_VIEWABLE)
                {   manage(wins[i], managereplys + index);
                }
                else if(replystates[i] && replystates[i]->map_state == XCB_WINDOW_ICONIC_STATE)
                {   manage(wins[i], managereplys + index);
                }
            }

            /* now the transients */
            for(i = 0; i < num; ++i)
            {   
                if(trans[i])
                {
                    if(replies[i]->map_state == XCB_MAP_STATE_VIEWABLE && replystates[i] && replystates[i]->map_state == XCB_WINDOW_ICONIC_STATE)
                    {
                        index = ManageClientLAST * i;
                        /* technically we shouldnt have to do this but just in case */
                        if(!wintoclient(wins[i]))
                        {   manage(wins[i], managereplys + index);
                        }
                    }
                }
            }
            /* cleanup */
            for(i = 0; i < num; ++i)
            {
                index = ManageClientLAST * i;
                managecleanup(managereplys + index);
                free(replies[i]);
                free(replystates[i]);
            }
            free(wa);
            free(wastates);
            free(tfh);
            free(managecookies);
            free(managereplys);
            free(replies);
            free(replystates);
            free(trans);
        }
        free(tree);
    }
    else
    {   Debug0("Failed to scan for clients.");
    }
    /* restore session covers this after */
}

void
sendmon(Client *c, Monitor *m)
{
    if(!c->desktop)
    {   c->desktop = m->desksel;
    }
    if(c->desktop->mon == m)
    {   Debug0("Cant send client to itself.");
        return;
    }
    unfocus(c, 1);
    setclientdesktop(c, m->desksel);
    focus(NULL);
    /* arrangeall() */
}

void
setup(void)
{
    /* clean up any zombies immediately */
    sighandler();

    setupatoms();
    setupcursors();
    setupcfg();
    setupwm();
    /* Setup prop handler */
    PropInit(_wm.handler);
    /* finds any monitor's */
    updategeom();
    updatedesktopnum();
    updatedesktop();
    updatedesktopnames();
    updateviewport();

    XCBWindowAttributes wa;
    /* xcb_event_mask_t */
    /* ~0 causes event errors because some event masks override others, for some reason... */
    wa.cursor = cursors[CurNormal];
    wa.event_mask =  XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT
                    |XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY
                    |XCB_EVENT_MASK_BUTTON_PRESS
                    |XCB_EVENT_MASK_BUTTON_RELEASE
                    |XCB_EVENT_MASK_POINTER_MOTION
                    |XCB_EVENT_MASK_ENTER_WINDOW
                    |XCB_EVENT_MASK_LEAVE_WINDOW
                    |XCB_EVENT_MASK_STRUCTURE_NOTIFY
                    |XCB_EVENT_MASK_PROPERTY_CHANGE
                    ;   /* the ; is here just so its out of the way */
    XCBChangeWindowAttributes(_wm.dpy, _wm.root, XCB_CW_EVENT_MASK|XCB_CW_CURSOR, &wa);
    XCBSelectInput(_wm.dpy, _wm.root, wa.event_mask);
    /* init numlock */
    updatenumlockmask();
    grabkeys();
    /* init hash */
    setupclienthash();
    focus(NULL);
}

void
setupatoms(void)
{
    XCBCookie motifcookie;
    XCBCookie wmcookie[WMLast];
    XCBCookie netcookie[NetLast];
    XCBCookie gtkcookie[GTKLAST];
    XCBCookie xembedcookie[XEMBEDLAST];

    motifcookie = XCBInternAtomCookie(_wm.dpy, "_MOTIF_WM_HINTS", False);
    XCBInitWMAtomsCookie(_wm.dpy, (XCBCookie *)wmcookie);
    XCBInitNetWMAtomsCookie(_wm.dpy, (XCBCookie *)netcookie);
    XCBInitGTKAtomsCookie(_wm.dpy, (XCBCookie *)gtkcookie);
    XCBInitXembedAtomsCookie(_wm.dpy, (XCBCookie *)xembedcookie);

    /* replies */
    motifatom = XCBInternAtomReply(_wm.dpy, motifcookie);
    XCBInitWMAtomsReply(_wm.dpy, wmcookie, wmatom);
    XCBInitNetWMAtomsReply(_wm.dpy, netcookie, netatom);
    XCBInitGTKAtomsReply(_wm.dpy, gtkcookie, gtkatom);
    XCBInitXembedAtomsReply(_wm.dpy, xembedcookie, xembedatom);
}

void
setupbar(Monitor *m, Client *bar)
{
    detachcompletely(bar);
    configure(bar);
    m->bar = bar;
    setoverrideredirect(bar, 1);
    setborderwidth(bar, 0);
    updateborder(bar);
    setdisableborder(bar, 1);
    setfullscreen(bar, 0);
    sethidden(bar, 0);
    setsticky(bar, 1);
    updatebargeom(m);
    updatebarpos(m);
    Debug("Found a bar: [%d]", bar->win);
}

void
setupcfg(void)
{   
    USInit(&_cfg);
}
void
setupsys(void)
{
#ifdef __OpenBSD__
    if (pledge("stdio rpath proc exec", NULL) == -1)
    {   DIECAT("pledge");
    }
#endif /* __OpenBSD__ */
    if(!setlocale(LC_CTYPE, ""))
    {   fputs("WARN: NO_LOCALE_SUPPORT\n", stderr);
    }
}

void
setupwm(void)
{
    setenv("GTK_CSD", "amogus", 1);
    /* startup wm */
    _wm.running = 1;
    _wm.syms = XCBKeySymbolsAlloc(_wm.dpy);
    _wm.sw = XCBDisplayWidth(_wm.dpy, _wm.screen);
    _wm.sh = XCBDisplayHeight(_wm.dpy, _wm.screen);
    _wm.root = XCBRootWindow(_wm.dpy, _wm.screen);
    /* Most java apps require this see:
     * https://wiki.archlinux.org/title/Java#Impersonate_another_window_manager
     * https://wiki.archlinux.org/title/Java#Gray_window,_applications_not_resizing_with_WM,_menus_immediately_closing
     * for more information.
     * "Hard coded" window managers to ignore "Write Once, Debug Everywhere"
     * This fixes java apps just having a blank white screen on some screen instances.
     * One example is Ghidra, made by the CIA.
     */
    _wm.wmname = "LG3D";

    if(!_wm.syms)
    {   
        cleanup();
        DIECAT("%s", "Could not establish connection with keyboard (OutOfMemory)");
    }
    /* supporting window for NetWMCheck */
    _wm.wmcheckwin = XCBCreateSimpleWindow(_wm.dpy, _wm.root, 0, 0, 1, 1, 0, 0, 0);
    XCBSelectInput(_wm.dpy, _wm.wmcheckwin, XCB_NONE);
    XCBChangeProperty(_wm.dpy, _wm.wmcheckwin, netatom[NetSupportingWMCheck], XCB_ATOM_WINDOW, 32, XCB_PROP_MODE_REPLACE, (unsigned char *)&_wm.wmcheckwin, 1);
    XCBChangeProperty(_wm.dpy, _wm.wmcheckwin, netatom[NetWMName], netatom[NetUtf8String], 8, XCB_PROP_MODE_REPLACE, _wm.wmname, strlen(_wm.wmname) + 1);
    XCBChangeProperty(_wm.dpy, _wm.root, netatom[NetSupportingWMCheck], XCB_ATOM_WINDOW, 32, XCB_PROP_MODE_REPLACE, (unsigned char *)&_wm.wmcheckwin, 1);
    /* EWMH support per view */
    XCBChangeProperty(_wm.dpy, _wm.root, netatom[NetSupported], XCB_ATOM_ATOM, 32, XCB_PROP_MODE_REPLACE, (unsigned char *)&netatom, NetLast);
    XCBChangeProperty(_wm.dpy, _wm.root, netatom[NetSupported], XCB_ATOM_ATOM, 32, XCB_PROP_MODE_APPEND, (unsigned char *)&wmatom, WMLast);
    XCBChangeProperty(_wm.dpy, _wm.root, netatom[NetSupported], XCB_ATOM_ATOM, 32, XCB_PROP_MODE_APPEND, (unsigned char *)&gtkatom, GTKLAST);
    XCBChangeProperty(_wm.dpy, _wm.root, netatom[NetSupported], XCB_ATOM_ATOM, 32, XCB_PROP_MODE_APPEND, (unsigned char *)&motifatom, 1);
    XCBDeleteProperty(_wm.dpy, _wm.root, netatom[NetClientList]);
    XCBDeleteProperty(_wm.dpy, _wm.root, netatom[NetClientListStacking]);
}

void
sigchld(int signo) /* signal */
{
    (void)signo;

    struct sigaction sa;
    /* donot transform children into zombies when they terminate */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDSTOP | SA_NOCLDWAIT | SA_RESTART;
    sa.sa_handler = SIG_IGN;
    sigaction(SIGCHLD, &sa, NULL);

    /* wait for childs (zombie proccess) to die */
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void
sighandler(void)
{
    /* sig info: https://faculty.cs.niu.edu/~hutchins/csci480/signals.htm 
     * */
    if(signal(SIGCHLD, &sigchld) == SIG_ERR)
    {   DIECAT("%s", "FATAL: CANNOT_INSTALL_SIGCHLD_HANDLER");
    }
    /* wait for zombies to die */
    sigchld(0);
    if(signal(SIGTERM, &sigterm) == SIG_ERR) 
    {   
        DIECAT("%s", "FATAL: CANNOT_INSTALL_SIGTERM_HANDLER");
        signal(SIGTERM, SIG_DFL); /* default signal */
    }

    if(signal(SIGHUP, &sighup) == SIG_ERR) 
    {   
        Debug("%s", "WARNING: CANNOT_INSTALL_SIGHUP_HANDLER");
        signal(SIGHUP, SIG_DFL); /* default signal */
    }
    if(signal(SIGINT, &sigterm) == SIG_ERR)
    {   
        Debug("%s", "WARNING: CANNOT_INSTALL_SIGINT_HANDLER");
        signal(SIGINT, SIG_DFL);
    }
}

void
sighup(int signo) /* signal */
{
    restarthard();
}

void
sigterm(int signo)
{
    quit();
}

void
specialconds(int argc, char *argv[])
{
    /* local support */
    char *err = strerror_l(errno, uselocale((locale_t)0));
    if(err)
    {   Debug("%s", strerror_l(errno, uselocale((locale_t)0)));
    }

    err = NULL;
    switch(_wm.has_error)
    {
        case XCB_CONN_ERROR:
            err =   "Could Hold connection to the XServer for whatever reason BadConnection Error.";
            break;
        case XCB_CONN_CLOSED_EXT_NOTSUPPORTED:
            err =   "The XServer could not find an extention ExtensionNotSupported Error.\n"
                    "This is more or less a developer error, but if you messed up your build this could happen."
                    ;
            break;
        case XCB_CONN_CLOSED_MEM_INSUFFICIENT:
            err =   "The XServer died due to an OutOfMemory Error.\n"
                    "This can be for several reasons but the the main few are as follows:\n"
                    "1.) Alloc Failure, due to system calls failing xcb could die, but probably didnt.\n"
                    "2.) No Memory, basically you ran out of memory for the system.\n"
                    "3.) ulimit issues, basically your system set memory limits programs."
                    ;
            break;
        case XCB_CONN_CLOSED_REQ_LEN_EXCEED:
            err =   "The XServer died due to an TooBigRequests Error.\n"
                    "Basically we either.\n"
                    "A.) Dont use the BigRequests extension and we ran into a \"Big Request.\" \n"
                    "Or\n"
                    "B.) We did use the BigRequests extension and some rogue app sent a massive requests bigger than ~16GiB of data.\n"
                    "This mostly occurs with rogue app's for B but more likely for A due to regular requests being small ~256kB."
                    ;
            break;
        case XCB_CONN_CLOSED_PARSE_ERR:
            err =   "The XServer died to due an BadParse Error.\n"
                    "While the XServer probably didnt and shouldnt die on a BadParse error it really just depends.\n"
                    "In essence however the XServer simply couldnt parse som form of requests that was sent."
                    ;
            break;
        case XCB_CONN_CLOSED_INVALID_SCREEN:
            err =   "Could not connect to specified screen.\n"
                    "You should check if your DISPLAY variable is correctly set, or if you incorrectly passed a screen as a display connection.\n"
                    "You may have incorrectly set your DISPLAY variable using setenv(\"DISPLAY\", (char *)) the correct format is as follows:\n"
                    "char *display = \":0\"; /* you pass in the thing ':' and the the number so display 5 would be \":5\" */\n"
                    "setenv(\"DISPLAY\", display);\n"
                    "The same applies when connection to the XServer using so XOpenDisplay(\":2\"); would open the second display.\n"
                    "For more information see: https://www.x.org/releases/X11R7.7/doc/man/man3/XOpenDisplay.3.xhtml"
                    ;
            break;
        case 0:
            /* no error occured */
            break;
        default:
            err = "The XServer died with an unexpected error.";
            break;
    }

    if(err)
    {   Debug("%s\nError code: %d", err, _wm.has_error);
    }


    /* this is the end of the exithandler so we dont really care if we segfault here if at all.
     * But this covers some cases where system skips to here. (AKA manual interrupt)
     * Really this is mostly just to prevent XKeyboard saying we didnt free stuff. (its annoying)
     * Though it sometimes doesnt work, it works 90% of the time which is good enough.
     */
    if(_wm.dpy)
    {   cleanup();
    }

    /* Check if restart hard */
    switch(_wm.restart)
    {
        case NoRestart:
        case SoftRestart:
            break;
        case HardRestart:
            if(argc)
            {   execvp(argv[0], argv);
            }
            /* execvp failed, likely the binary no longer exists */
            Debug("Failed to restart using execvp, defaulting to %s", M_STRINGIFY(SoftRestart));
            break;
    }
}

void
startup(void)
{
    setupsys();
    startupwm();
    checkotherwm();
    atexit(exithandler);
#ifndef Debug
    XCBSetErrorHandler(xerror);
#endif
}

void
startupwm(void)
{
    /* Make clean */
    memset(&_wm, 0, sizeof(WM));
    /* init threading */
    pthread_mutexattr_t attr;
    /* TODO: Just make toggle functions use a seperate thread isntead of high jacking the main thread #DontBeStupid */
    _wm.use_threads = 0;
    if(!pthread_mutexattr_init(&attr))
    {
        if(!pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE))
        {   _wm.use_threads = !pthread_mutex_init(&_wm.mutex, &attr);
        }
        pthread_mutexattr_destroy(&attr);
    }
    char *display = NULL;
    _wm.dpy = XCBOpenDisplay(display, &_wm.screen);
    display = display ? display : getenv("DISPLAY");
    Debug("DISPLAY -> %s", display);
    if(!_wm.dpy)
    {   
        if(_wm.use_threads)
        {   pthread_mutex_destroy(&_wm.mutex);
        }
        DIECAT("FATAL: Cannot Connect to X Server. [%s]", display);
    }
    _wm.handler = PropCreateStatic();
    /* This allows for execvp and exec to only spawn process on the specified display rather than the default varaibles */
    if(display)
    {   setenv("DISPLAY", display, 1);
    }
}

void
updatebarpos(Monitor *m)
{
    /* reset space */
    m->ww = m->mw;
    m->wh = m->mh;
    m->wx = m->mx;
    m->wy = m->my;
    Client *bar = m->bar;
    if(!bar)
    {   return;
    }
    enum BarSides side = GETBARSIDE(m, bar, 0);
    if(ISFIXED(bar))
    {
        if(bar->w > bar->h)
        {   
            /* is it top bar ? */
            if(bar->y + bar->h / 2 <= m->my + m->mh / 2)
            {   side = BarSideTop;
            }
            /* its bottom bar */
            else
            {   side = BarSideBottom;
            }
        }
        else if(bar->w < bar->h)
        {
            /* is it left bar? */
            if(bar->x + bar->w / 2 <= m->mx + m->mw / 2)
            {   side = BarSideLeft;
            }
            /* its right bar */
            else
            {   side = BarSideRight;
            }
        }
        else
        {   Debug0("Detected bar is a square suprisingly.");
        }
    }

    i32 x;
    i32 y;
    i32 w;
    i32 h;

    /* default is top left side */
    x = m->mx;
    y = m->my;
    w = bar->w;
    h = bar->h;

    if(!ISHIDDEN(bar))
    {
        switch(side)
        {
            case BarSideLeft:
                m->wx += bar->w;
                m->ww -= bar->w;
                Debug0("Bar Placed Left.");
                break;
            case BarSideRight:
                x += m->mw - bar->w;
                m->ww -= bar->w;
                Debug0("Bar Placed Right.");
                break;
            case BarSideTop:
                m->wy += bar->h;
                m->wh -= bar->h;
                Debug0("Bar Placed Top.");
                break;
            case BarSideBottom:
                y += m->mh - bar->h;
                m->wh -= bar->h;
                Debug0("Bar Placed Bottom.");
                break;
            default:
                break;
        }
    }
    else
    {   
        switch(side)
        {
            case BarSideLeft:
                x = -bar->w;
                break;
            case BarSideRight:
                x = m->mw;
                break;
            case BarSideTop:
                y = -bar->h;
                break;
            case BarSideBottom:
                y = m->mh;
                break;
            default:
                /* just warp offscreen */
                x = m->mw;
                y = m->mh;
                break;
        }
    }
    resize(bar, x, y, w, h, 1);
}

void
updatebargeom(Monitor *m)
{
    Client *bar = m->bar;
    if(!bar || ISHIDDEN(bar))
    {   return;
    }
    /* if the bar is fixed then the geom is impossible to update, also we dont want to update our current bar status cause of that also */
    if(ISFIXED(bar))
    {   return;
    }

    f32 bxr;
    f32 byr;
    f32 bwr;
    f32 bhr;
    enum BarSides side = GETBARSIDE(m, bar, 0);
    enum BarSides prev = GETBARSIDE(m, bar, 1);

    if(prev != side)
    {
        i32 x;
        i32 y;
        i32 w;
        i32 h;
        switch(side)
        {   
            case BarSideLeft:
                bxr = _cfg.lx;
                byr = _cfg.ly;
                bwr = _cfg.lw;
                bhr = _cfg.lh;
                break;
            case BarSideRight:
                bxr = _cfg.rx;
                byr = _cfg.ry;
                bwr = _cfg.rw;
                bhr = _cfg.rh;
                break;
            case BarSideTop:
                bxr = _cfg.tx;
                byr = _cfg.ty;
                bwr = _cfg.tw;
                bhr = _cfg.th;
                break;
            case BarSideBottom:
                bxr = _cfg.bx;
                byr = _cfg.by;
                bwr = _cfg.bw;
                bhr = _cfg.bh;
                break;
        }
        x = m->mx + (m->mw * bxr);
        y = m->my + (m->mh * byr);
        w = m->mw * bwr;
        h = m->mh * bhr;
        resize(bar, x, y, w, h, 1);
    }
    else
    {
        switch(side)
        {
            case BarSideLeft:
                _cfg.lx = bar->x;
                _cfg.ly = bar->y;
                _cfg.lw = bar->w;
                _cfg.lh = bar->h;
                break;
            case BarSideRight:
                _cfg.rx = bar->x;
                _cfg.ry = bar->y;
                _cfg.rw = bar->w;
                _cfg.rh = bar->h;
                break;
            case BarSideTop:
                _cfg.tx = bar->x;
                _cfg.ty = bar->y;
                _cfg.tw = bar->w;
                _cfg.th = bar->h;
                break;
            case BarSideBottom:
                _cfg.bx = bar->x;
                _cfg.by = bar->y;
                _cfg.bw = bar->w;
                _cfg.bh = bar->h;
                break;
        }
    }
}

void
wakeupconnection(XCBDisplay *display, int screen)
{
    if(!display)
    {   Debug0("No connection avaible");
        return;
    }
    XCBGenericEvent ev;
    XCBClientMessageEvent *cev = (XCBClientMessageEvent *)&ev;
    memset(&ev, 0, sizeof(XCBGenericEvent));
    cev->type = wmatom[WMProtocols];
    cev->response_type = XCB_CLIENT_MESSAGE;
    cev->window = _wm.root;
    cev->format = 32;
    cev->data.data32[0] = wmatom[WMDeleteWindow];
    cev->data.data32[1] = XCB_CURRENT_TIME;
                                        /* XCB_EVENT_MASK_NO_EVENT legit does nothing lol */
    XCBSendEvent(display, XCBRootWindow(display, screen), False, XCB_EVENT_MASK_STRUCTURE_NOTIFY, (const char *)&ev);
    /* make sure display gets the event (duh) */
    XCBFlush(_wm.dpy);
}

void
xerror(XCBDisplay *display, XCBGenericError *err)
{
    if(err)
    {   
        Debug("%s %s\n", XCBGetErrorMajorCodeText(err->major_code), XCBGetFullErrorText(err->error_code));
        Debug("error_code: [%d], major_code: [%d], minor_code: [%d]\n"
              "sequence: [%d], response_type: [%d], resource_id: [%d]\n"
              "full_sequence: [%d]\n"
              ,
           err->error_code, err->major_code, err->minor_code, 
           err->sequence, err->response_type, err->resource_id, 
           err->full_sequence);
        XCBCookie id;
        id.sequence = err->sequence;
        Debug("%s()", XCBDebugGetNameFromId(id));
        (void)id;
    }
}

int
main(int argc, char **argv)
{
    do
    {
        ArgcvHandler(argc, argv);
        startup();
        setup();
        scan();
        restoresession();
        run();
        cleanup();
        specialconds(argc, argv);
    } while(_wm.restart);
    return EXIT_SUCCESS;
}

/* See LICENSE file for copyright and license details.
 * 4 Tab spaces; No tab characters use spaces for tabs
 * Basic overview of dwm => https://ratfactor.com/dwm
 * For more information about xlib (X11)       visit https://x.org/releases/current/doc/libX11/libX11/libX11.html
 * For a quick peak at commonly used functions visit https://tronche.com/gui/x/xlib/
 * Cursors : https://tronche.com/gui/x/xlib/appendix/b/
 * XCursor:  https://man.archlinux.org/man/Xcursor.3
 * EWMH:     https://specifications.freedesktop.org/wm-spec/wm-spec-latest.html
 * XEvent:   https://tronche.com/gui/x/xlib/events/structures.html
 * Document: https://www.x.org/releases/X11R7.5/doc/x11proto/proto.pdf
 */


/* DebugGING
 * Stuff you need gdb xephyr
 * sudo pacman -S gdb xorg-server-xephyr
 *
 *
 * first make sure its compiled in Debug using config.mk
 *
 * run this command: Xephyr :1 -ac -resizeable -screen 680x480 &
 * set the display to the one you did for Xephyr in this case we did 1 so
 * run this command: export DISPLAY=:1
 * now you are mostly done
 * run this command: gdb dwm
 * you get menu
 * run this command: lay spl
 * you get layout and stuff
 * 
 * now basic gdb stuff
 * break or b somefunction # this sets a break point for whatever function AKA stop the code from running till we say so
 * next or n # this moves to the next line of logic code (logic code is current code line)
 * step or s # this moves to the next line of code (code being actual code so functions no longer exist instead we just go there)
 * ctrl-l # this resets the window thing which can break sometimes (not sure why it hasnt been fixed but ok)
 * skip somefunction # this tries to skip a function if it can but ussualy is worthless (AKA I dont know how to use it)(skip being not show to you but it does run in the code)
 *
 * after your done
 * run this command: exit
 * 
 */
