

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


#include "main.h"

#include "parser.h"
#include "session.h"
#include "config.h"
#include "file_util.h"

extern WM _wm;




enum
{
    /* Yes this would be read as Monitor Monitor X, but for consitency this is kept as is */
    MonMX,
    MonMY,
    MonMW,
    MonMH,

    MonWX,
    MonWY,
    MonWW,
    MonWH,

    MonDesktopSelected,
    MonDesktopCount,

    MonLAST,
};

enum
{
    DesktopNumber,

    DesktopLayout,
    DesktopOldLayout,

    DesktopLAST,
};

enum
{
    ClientOldX,
    ClientOldY,
    ClientOldW,
    ClientOldH,

    ClientX,
    ClientY,
    ClientW,
    ClientH,

    ClientBorderWidth,
    ClientOldBorderWidth,
    ClientBorderColour,

    ClientWindow,

    ClientNext,
    ClientPrev,

    ClientStackNext,
    ClientStackPrev,

    ClientRestackNext,
    ClientRestackPrev,

    ClientFocusNext,
    ClientFocusPrev,

    ClientFlags,
    ClientEWMHFlags,
    ClientHasFocus,

    ClientLAST,
};

#define VOX_ADD_MEMBER_SESSION(NAME, FIELD_NAME, STRUCT_TYPE, TYPE, DEFAULT_SETTING) \
        VOX_ADD_MEMBER(NAME, TYPE, offsetof(STRUCT_TYPE, FIELD_NAME), FIELD_SIZEOF(STRUCT_TYPE, FIELD_NAME), DEFAULT_SETTING)

#define VOX_ADD_MEMBER_SESSION_MON(NAME, FIELD_NAME, TYPE, DEFAULT_SETTING) \
        VOX_ADD_MEMBER_SESSION(NAME, FIELD_NAME, Monitor, TYPE, DEFAULT_SETTING)

#define VOX_ADD_MEMBER_SESSION_DESKTOP(NAME, FIELD_NAME, TYPE, DEFAULT_SETTING) \
        VOX_ADD_MEMBER_SESSION(NAME, FIELD_NAME, Desktop, TYPE, DEFAULT_SETTING)

#define VOX_ADD_MEMBER_SESSION_CLIENT(NAME, FIELD_NAME, TYPE, DEFAULT_SETTING) \
        VOX_ADD_MEMBER_SESSION(NAME, FIELD_NAME, Client, TYPE, DEFAULT_SETTING)

const SCSetting MonSave[MonLAST] = 
{
    VOX_ADD_MEMBER_SESSION_MON(MonMX, mx, SCTypeSHORT, 0)
    VOX_ADD_MEMBER_SESSION_MON(MonMY, my, SCTypeSHORT, 0)
    VOX_ADD_MEMBER_SESSION_MON(MonMW, mw, SCTypeUSHORT, 1)
    VOX_ADD_MEMBER_SESSION_MON(MonMH, mh, SCTypeUSHORT, 1)

    VOX_ADD_MEMBER_SESSION_MON(MonWX, wx, SCTypeSHORT, 0)
    VOX_ADD_MEMBER_SESSION_MON(MonWY, wy, SCTypeSHORT, 0)
    VOX_ADD_MEMBER_SESSION_MON(MonWW, ww, SCTypeUSHORT, 1)
    VOX_ADD_MEMBER_SESSION_MON(MonWH, wh, SCTypeUSHORT, 1)

    VOX_ADD_MEMBER_SESSION_MON(MonDesktopCount, deskcount, SCTypeUSHORT, 1)


    /* These values are overriden, but maintain their type. 
     * EX:
     * VOX_ADD_MEMBER_SESSION_XXX(EnumThing, xx, SCTypeMySize, MyValue)
     *                                       ^^
     *                              This value is overwritten.
     *                              Everything else is still read and interpreted the same though.
     * (Everything below is counted, if none below assume none require overriding)
     */
    VOX_ADD_MEMBER_SESSION_MON(MonDesktopSelected, mx, SCTypeUSHORT, 2)
};



const SCSetting DesktopSave[DesktopLAST] = 
{
    VOX_ADD_MEMBER_SESSION_DESKTOP(DesktopNumber, num, SCTypeSHORT, 0)
    VOX_ADD_MEMBER_SESSION_DESKTOP(DesktopLayout, layout, SCTypeUCHAR, Tiled)
    VOX_ADD_MEMBER_SESSION_DESKTOP(DesktopOldLayout, olayout, SCTypeUCHAR, Monocle)

    /* These values are overriden, but maintain their type. 
     * EX:
     * VOX_ADD_MEMBER_SESSION_XXX(EnumThing, xx, SCTypeMySize, MyValue)
     *                                       ^^
     *                              This value is overwritten.
     *                              Everything else is still read and interpreted the same though.
     * (Everything below is counted, if none below assume none require overriding)
     */
};

const SCSetting ClientSave[ClientLAST] = 
{
    VOX_ADD_MEMBER_SESSION_CLIENT(ClientOldX, oldx, SCTypeSHORT, 0)
    VOX_ADD_MEMBER_SESSION_CLIENT(ClientOldY, oldy, SCTypeSHORT, 0)
    VOX_ADD_MEMBER_SESSION_CLIENT(ClientOldW, oldw, SCTypeUSHORT, 1)
    VOX_ADD_MEMBER_SESSION_CLIENT(ClientOldH, oldh, SCTypeUSHORT, 1)

    VOX_ADD_MEMBER_SESSION_CLIENT(ClientX, x, SCTypeSHORT, 0)
    VOX_ADD_MEMBER_SESSION_CLIENT(ClientY, y, SCTypeSHORT, 0)
    VOX_ADD_MEMBER_SESSION_CLIENT(ClientW, w, SCTypeUSHORT, 1)
    VOX_ADD_MEMBER_SESSION_CLIENT(ClientH, h, SCTypeUSHORT, 1)

    
    VOX_ADD_MEMBER_SESSION_CLIENT(ClientBorderWidth, bw, SCTypeUSHORT, 0)
    VOX_ADD_MEMBER_SESSION_CLIENT(ClientOldBorderWidth, oldbw, SCTypeUSHORT, 0)
    VOX_ADD_MEMBER_SESSION_CLIENT(ClientBorderColour, bcol, SCTypeUINT, 0)

    VOX_ADD_MEMBER_SESSION_CLIENT(ClientWindow, win, SCTypeUINT, 0)


    VOX_ADD_MEMBER_SESSION_CLIENT(ClientFlags, flags, SCTypeUSHORT, 0)
    VOX_ADD_MEMBER_SESSION_CLIENT(ClientEWMHFlags, ewmhflags, SCTypeUINT, 0)





    /* These values are overriden, but maintain their type. 
     * EX:
     * VOX_ADD_MEMBER_SESSION_XXX(EnumThing, xx, SCTypeMySize, MyValue)
     *                                       ^^
     *                              This value is overwritten.
     *                              Everything else is still read and interpreted the same though.
     * (Everything below is counted, if none below assume none require overriding)
     */
    VOX_ADD_MEMBER_SESSION_CLIENT(ClientNext, win, SCTypeUINT, 0)
    VOX_ADD_MEMBER_SESSION_CLIENT(ClientPrev, win, SCTypeUINT, 0)

    VOX_ADD_MEMBER_SESSION_CLIENT(ClientStackNext, win, SCTypeUINT, 0)
    VOX_ADD_MEMBER_SESSION_CLIENT(ClientStackPrev, win, SCTypeUINT, 0)

    VOX_ADD_MEMBER_SESSION_CLIENT(ClientRestackNext, win, SCTypeUINT, 0)
    VOX_ADD_MEMBER_SESSION_CLIENT(ClientRestackPrev, win, SCTypeUINT, 0)

    VOX_ADD_MEMBER_SESSION_CLIENT(ClientFocusNext, win, SCTypeUINT, 0)
    VOX_ADD_MEMBER_SESSION_CLIENT(ClientFocusPrev, win, SCTypeUINT, 0)

    VOX_ADD_MEMBER_SESSION_CLIENT(ClientHasFocus, win, SCTypeBOOL, 0)
};





void SessionSaveMonitors(SCParser *parse);
void SessionSaveMonitor(SCParser *parser, Monitor *m);
void SessionSaveBar(SCParser *parser, Client *c);
void SessionSaveDesktops(SCParser *parser, Monitor *m);
void SessionSaveDesktop(SCParser *parser, Desktop *desktop);
void SessionSaveClients(SCParser *parser, Desktop *desktop);
void SessionSaveClient(SCParser *parser, Client *c);


void
SessionSave(
        void
        )
{
    char buff[FFSysGetConfigPathLengthMAX];
    int status;

    status = WMConfigGetSessionPath(buff, FFSysGetConfigPathLengthMAX, NULL);

    if(status == EXIT_FAILURE)
    {   
        Debug0("Failed to save data FIXME");
        return;
    }

    status = FFCreateFile(buff);

    if(status != EXIT_SUCCESS)
    {   
        /* This should not occur, but may due to out of memory, low storage, etc... */
        Debug0("Failed to create session save file.");
        return;
    }
}


void
SessionSaveMonitors(
    SCParser *parser
    )
{
    Monitor *m;
    for(m = _wm.mons; m; m = nextmonitor(m))
    {   SessionSaveMonitor(parser, m);
    }
}

void
SessionSaveMonitor(
    SCParser *parser,
    Monitor *m
    )
{
    i16 mx = m->mx;
    i16 my = m->my;
    u16 mw = m->mw;
    u16 mh = m->mh;
    i16 wx = m->wx;
    i16 wy = m->wy;
    u16 ww = m->ww;
    u16 wh = m->wh;

    u16 desksel = m->desksel->num;
    u16 deskcount = m->deskcount;


    SessionSaveBar(parser, m->bar);
    SessionSaveDesktops(parser, m);
}

void
SessionSaveBar(
    SCParser *parser,
    Client *bar
    )
{
    if(!bar)
    {   return;
    }
}

void
SessionSaveDesktops(
    SCParser *parser,
    Monitor *m
    )
{
    Desktop *desktop;
    for(desktop = m->desktops; desktop; desktop = nextdesktop(desktop))
    {   SessionSaveDesktop(parser, desktop);
    }
}

void
SessionSaveDesktop(
    SCParser *parser,
    Desktop *desktop
    )
{
    i16 desknum = desktop->num;
    u8 layout = desktop->layout;
    u8 old_layout = desktop->olayout;
    SessionSaveClients(parser, desktop);
}

void
SessionSaveClients(
    SCParser *parser,
    Desktop *desktop
    )
{
    Client *c;
    for(c = startclient(desktop); c; c = nextclient(c))
    {   SessionSaveClient(parser, c);
    }
}

void
SessionSaveClient(
    SCParser *parser,
    Client *c
    )
{

    i16 ox = c->oldx;
    i16 oy = c->oldy;
    i16 ow = c->oldw;
    i16 oh = c->oldh;

    i16 x = c->x;
    i16 y = c->y;
    i16 w = c->w;
    i16 h = c->h;

    u16 bw = c->bw;
    u16 oldwbw = c->oldbw;
    u32 bcol = c->bcol;

    XCBWindow win = c->win;

    XCBWindow next = nextclient(c) ? nextclient(c)->win : 0;
    XCBWindow prev = prevclient(c) ? prevclient(c)->win : 0;
    
    XCBWindow snext = nextstack(c) ? nextstack(c)->win : 0;
    XCBWindow sprev = prevstack(c) ? prevstack(c)->win : 0;

    XCBWindow rnext = nextrstack(c) ? nextrstack(c)->win : 0;
    XCBWindow rprev = nextrstack(c) ? nextrstack(c)->win : 0;

    XCBWindow fnext = nextfocus(c) ? nextfocus(c)->win : 0;
    XCBWindow fprev = prevfocus(c) ? prevfocus(c)->win : 0;

    u16 flags = c->flags;
    u32 ewmhflags = c->ewmhflags;
    u8 sel = c->desktop->sel == c;
}






















