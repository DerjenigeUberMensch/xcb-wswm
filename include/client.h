#ifndef _WM_CLIENT_H
#define _WM_CLIENT_H

#include "../tools/XCB-TRL/xcb_trl.h"
#include "decorations.h"
#include "safebool.h"
#include "util.h"

#include <stdint.h>
/* EWMH window types */
enum EnumAsBitFlags
EWMHFlags
{
    /* Window Type */
    WTypeFlagDesktop =          1u << 0,
    WTypeFlagDock =             1u << 1,
    WTypeFlagToolbar =          1u << 2,
    WTypeFlagMenu =             1u << 3,
    WTypeFlagUtility =          1u << 4,
    WTypeFlagSplash =           1u << 5,
    WTypeFlagDialog =           1u << 6,
    WTypeFlagDropdownMenu =     1u << 7,
    WTypeFlagPopupMenu =        1u << 8,
    WTypeFlagTooltip =          1u << 9,
    WTypeFlagNotification =     1u << 10,
    WTypeFlagCombo =            1u << 11,
    WTypeFlagDnd =              1u << 12,
    WTypeFlagNormal =           1u << 13,

    /* Window State */
    WStateFlagModal =           1u << 14,
    WStateFlagSticky =          1u << 15,
    WStateFlagMaximizedVert =   1u << 16,
    WStateFlagMaximizedHorz =   1u << 17,
    WStateFlagShaded =          1u << 18,
    WStateFlagSkipTaskbar =     1u << 19,
    WStateFlagSkipPager =       1u << 20,
    WStateFlagHidden =          1u << 21,
    WStateFlagFullscreen =      1u << 22,
    WStateFlagAbove =           1u << 23,
    WStateFlagBelow =           1u << 24,
    WStateFlagDemandAttention = 1u << 25,
    WStateFlagFocused =         1u << 26,

    /* Extra Bits */
    WStateFlagWMTakeFocus =     1u << 27,
    WStateFlagWMSaveYourself =  1u << 28,
    WStateFlagWMDeleteWindow =  1u << 29,
    WStateFlagNeverFocus =      1u << 30,
/* Iso prevents it cause why not */
/*    WStateFlagMapIconic =       1u << 31,     */
};
/* Window map states, Widthdrawn, Iconic, Normal. */
#define WStateFlagMapIconic     (1u << 31)

enum WMMapState
{
    WMMapStateMapped,
    WMMapStateUnmapped,
};
/* Client struct flags */
enum EnumAsBitFlags
ClientFlags
{
    ClientFlagFloating = 1u << 0,
    ClientFlagWasFloating = 1 << 1,
    ClientFlagShowDecor = 1u << 2,
    ClientFlagKeepFocus = 1u << 4,
    ClientFlagDisableBorder = 1u << 5,
    ClientFlagOverrideRedirect = 1u << 6,
    ClientFlagMapped = 1u << 7,   /* desktop optimizations */
};

/* kill client type */
enum KillType 
{ 
    Graceful, 
    Safedestroy, 
    Destroy, 
};

/* Manage cookies */
/* Manage cookies */
enum ManageClientProperties
{
    ManageClientAttributes,
    ManageClientGeometry,
    ManageClientTransient,
    ManageClientWType,
    ManageClientWState,
    ManageClientSizeHint,
    ManageClientWMHints,
    ManageClientClass,
    ManageClientWMProtocol,
    ManageClientStrutP,
    ManageClientStrut,
    ManageClientNetWMName,
    ManageClientWMName,
    ManageClientPid,
    ManageClientIcon,
    ManageClientMotif,
    

    ManageClientLAST
};

enum
FloatType
{
    DefinitelyFloating,
    ProbablyFloating,
    CouldBeFloating,
    ProbablyNotFloating,
    DefinitelyNotFloating,

    FLOATINGLAST,
};

typedef struct Client Client;
/* extern structs */
struct Desktop;
struct Monitor;

struct Client
{
    int16_t x;          /* X coordinate             */
    int16_t y;          /* Y coordinate             */

    uint16_t w;         /* Width                    */
    uint16_t h;         /* height                   */

    int16_t oldx;       /* Previous X coordinate    */
    int16_t oldy;       /* Previous Y coordinate    */

    uint16_t oldw;      /* Previous Width           */
    uint16_t oldh;      /* Previous Height          */


    uint16_t bw;        /* Border Width             */
    uint16_t oldbw;     /* Old Border Width         */

    uint32_t bcol;      /* Border Colour            */


    float mina;         /* Minimum Aspect           */
    float maxa;         /* Maximum Aspect           */


    uint16_t basew;     /* Base Width               */
    uint16_t baseh;     /* Base Height              */

    uint16_t incw;      /* Increment Width          */
    uint16_t inch;      /* Increment Height         */


    uint16_t maxw;      /* Max Width.               */
    uint16_t maxh;      /* Max Height.              */

    uint16_t minw;      /* Minimum Width            */
    uint16_t minh;      /* Minimum Height           */


    XCBWindow win;      /* Client Window            */
    pid_t pid;          /* Client Pid               */


    Client *next;       /* The next client in list  */
    Client *prev;       /* The previous client      */
    Client *sprev;      /* The prev stack order clnt*/
    Client *snext;      /* The next client in stack */
    Client *rnext;      /* Restack Next             */
    Client *rprev;      /* Restack Prev             */
    Client *fnext;      /* The next focused client  */
    Client *fprev;      /* The previous focused clnt*/
    struct Desktop *desktop;   /* Client Associated Desktop*/
    Decoration *decor;  /* Decoration AKA title bar.*/

    char *netwmname;    /* Client Name              */
    char *wmname;       /* Client Name backup       */
    char *classname;    /* Class Name               */
    char *instancename; /* Instance Name            */
    uint32_t *icon;     /* Array of icon values     */

    uint16_t rstacknum; /* Used in calculating pos  */
    uint16_t flags;     /* Misc States              */

    uint32_t ewmhflags; /* EWMH types/states        */
    enum XCBBitGravity gravity; /* Client gravity   */
    XCBColormap colormap;   /* Clients colormap     */
};

/* Applies the gravity shifts specified by the gravity onto the x and y coordinates.
*/
void NonNullAll applygravity(const enum XCBBitGravity gravity, int32_t *x, int32_t *y, const uint32_t width, const uint32_t height, const uint32_t border_width);
/* Applies size hints to the specified values.
* interact:             1/true/True         Does not restrict bounds to window area.
*                       0/false/False       Restricts bounds to window area.
* RETURN: 1 if the specified x/y/w/h does not match after sizehints applied. (aka need to resize.)
* RETURN: 0 if the specified x/y/w/h does match after the sizehints applied. (No need to resize.)
*/
uint8_t NonNullAll applysizehints(Client *c, int32_t *x, int32_t *y, int32_t *width, int32_t *height, uint8_t interact);
/* Frees Client and allocated client properties. 
*/
void NonNull cleanupclient(Client *c);
/* Initializes the Clients colormap */
void NonNullArg(1) clientinitcolormap(Client *c, XCBGetWindowAttributes *wa);
/* Initializes the Client decoration */
void NonNull clientinitdecor(Client *c);
/* Initializes the Clients floating status, based on clients flags/hints. _NET_WM_STATE/_NET_WM_WINDOW_TYPE */
void NonNull clientinitfloat(Client *c);
/* Initializes the Client geometry from the specified XCBWindowGeometry struct. 
 */
void NonNullArg(1) clientinitgeom(Client *c, XCBWindowGeometry *geometry);
/* Initializes the Client window type from the specified XCBWindowProperty. */
void NonNullArg(1) clientinitwtype(Client *c, XCBWindowProperty *windowtypereply);
/* Initializes the Client window state from the specified XCBWindowProperty. */
void NonNullArg(1) clientinitwstate(Client *c, XCBWindowProperty *windowstatereply);
/* Sets the correct client desktop if trans found, default to _wm.selmon->desksel if not.*/
void NonNull clientinittrans(Client *c, XCBWindow trans);
/* Updates the XServers knowledge of the clients coordinates.
 * NOTE: This is a sendevent to the c->win data type.
 * NOTE: XCBFlush(); must be called to push the XCB internal buffer to send this request.
 */
void NonNull configure(Client *c);
/* Allocates a client and client properties with all data set to 0 or the adress of any newly allocated data.
 * RETURN: Client * on Success.
 * RETURN: NULL on Failure.
*/
Client *createclient(void);
/* Sets focus to the specified client.
 * NOTE: if NULL provided first visible client in stack is choosen as focus specifier.
 */
void FuncNullable focus(Client *c);
/* updates focus order to specified client, DOES NOT however, set X11 focus, only updates internal list.
 * Which in turn desktop->sel IS NOT updated, and focus() MUST be called after.
 * NOTE: This should NOT be used aside from manual visual optimizations.
 * NOTE: if NULL provided first visible client in stack is choosen as focus specifier.
 */
Client *FuncNullable focusrealize(Client *c);
/* Grabs a win buttons. 
 * Basically this just allows us to receive button press/release events from windows.
 */
void NonNull grabbuttons(Client *c, uint8_t focused);
/* Grabs a windows keys.
 * Basically this just allows us to receive/intercept key press/release events.
 */
void grabkeys(void);
/* Kills the specified window.
 * type:            Graceful            Sends a message to the window to kill itself.
 *                  Safedestroy         Sends a message to the window to kill itself, on failure, forcefully kill the window.
 *                  Destroy             Destroys a window without sending any message for the window to response (Nuclear option.)
 */
void NonNull killclient(Client *c, enum KillType type);
/* Returns the last client in desktop.
 * RETURN: Client * on Success.
 * RETURN: NULL on Failure. (no clients)
 */
Client *NonNull lastclient(struct Desktop *desk);
/* Returns the last client in desktop.
 * RETURN: Client * on Success.
 * RETURN: NULL on Failure. (no clients)
 */
Client *NonNull lastfocus(struct Desktop *desk);
/* Returns the last client in desktop.
 * RETURN: Client * on Success.
 * RETURN: NULL on Failure. (no clients)
 */
Client *NonNull laststack(struct Desktop *desk);
/* Returns the last client in desktop.
 * RETURN: Client * on Success.
 * RETURN: NULL on Failure. (no clients)
 */
Client *NonNull lastrstack(struct Desktop *desk);
/* cleanups client replies. */
void NonNull managecleanup(void *replies[ManageClientLAST]);
/* requests for clients cookies */
void NonNullAll managerequest(XCBWindow win, XCBCookie requests[ManageClientLAST]);
/* requests for reply backs for clients clients cookies */
void NonNullAll managereplies(XCBCookie requests[ManageClientLAST], void *replies[ManageClientLAST]);
/* Part of main event loop "run()"
 * Manages AKA adds the window to our current or windows specified desktop.
 * Applies size checks, bounds, layout, etc...
 * RETURN: Client * on Success.
 * RETURN: NULL on Failure.
 */
Client *NonNullAll manage(XCBWindow window, void *replies[ManageClientLAST]);
/* Maximizes a client if unmaxed, Sets flag.
 */
void NonNull maximize(Client *c);
/* Maximizes horizontally a client if unmaxed horz, Sets flag.
 */
void NonNull maximizehorz(Client *c);
/* Maximizes vertically a client if unmaxed vert, Sets flag.
 */
void NonNull maximizevert(Client *c);
/* Returns the next client avaible.
 * RETURN: Client * on Success.
 * RETURN: NULL on Failure.
 */
Client *FuncNullable nextclient(Client *c);
/* Returns the next client in stack avaible.
 * RETURN: Client* on Success.
 * RETURN: NULL on Failure.
 */
Client *FuncNullable nextstack(Client *c);
/* Returns the next client in restack avaible.
 * RETURN: Client * on Success.
 * RETURN: NULL on Failure. 
 */
Client *FuncNullable nextrstack(Client *c);
/* Returns the next client in focus order avaible.
 * RETURN: Client* on Success.
 * RETURN: NULL on Failure.
 */
Client *FuncNullable nextfocus(Client *c);
/* Returns next tiled client when tiling.
 * RETURN: Client * on Success.
 * RETURN: NULL on Failure.
 */
Client *FuncNullable nexttiled(Client *c);
/* Returns the next visible client avaible.
 * RETURN: Client* on Success.
 * RETURN: NULL on Failure.
 */
Client *FuncNullable nextvisible(Client *c);
/* Returns the prev client avaible.
 * RETURN: Client* on Success.
 * RETURN: NULL on Failure.
 */
Client *FuncNullable prevclient(Client *c);
/* Returns the prev focus client avaible.
 * RETURN: Client* on Success.
 * RETURN: NULL on Failure.
 */
Client *FuncNullable prevfocus(Client *c);
/* Returns the prev stack client avaible.
 * RETURN: Client* on Success.
 * RETURN: NULL on Failure.
 */
Client *FuncNullable prevstack(Client *c);
/* Returns the previous restack stack client avaible.
 * RETURN: Client * on Success.
 * RETURN: NULL on Failure.
 */
Client *FuncNullable prevrstack(Client *c);
/* Returns the prev visible client avaible.
 * RETURN: Client* on Success.
 * RETURN: NULL on Failure.
 */
Client *FuncNullable prevvisible(Client *c);
/* resize a client only if specified x/y/w/h is different 
 * interact
 * {1, 0}
 * 1 -> dont confide resize to monitor dimentions 
 * 0 -> confide resize within monitor dimentions
 */
void NonNull resize(Client *c, int32_t x, int32_t y, int32_t width, int32_t height, uint8_t interact);
/* resize a client given parametors without sizehints */
void NonNull resizeclient(Client *c, int16_t x, int16_t y, uint16_t width, uint16_t height);
/* moves a client only if specified x/y is different 
 * interact
 * {1, 0}
 * 1 -> dont confide resize to monitor dimentions 
 * 0 -> confide resize within monitor dimentions
 */
void NonNull resizemove(Client *c, int16_t x, int16_t y, uint8_t interact);
/* Sends a Protocol Event to specified client */
void NonNull sendprotocolevent(Client *c, XCBAtom proto);
/* Sets the flag "alwaysontop" to the provided Client */
void NonNull setalwaysontop(Client *c, uint8_t isalwaysontop);
/* Sets the flag "alwaysonbottom" to the provided Client */
void NonNull setalwaysonbottom(Client *c, uint8_t state);
/* Sets the Clients border opacity, "alpha" 0-255 */
void NonNull setborderalpha(Client *c, uint8_t alpha);
/* Sets the border color using red green and blue values */
void NonNull setbordercolor(Client *c, uint8_t red, uint8_t green, uint8_t blue);
/* Sets the border color only using the 32bit value */
void NonNull setbordercolor32(Client *c, uint32_t col);
/* Sets the border width to the provided Client */
void NonNull setborderwidth(Client *c, uint16_t border_width);
/* Sets the clients desktop to the specified desktop, 
 * and cleanups any data that may have been left from the previous desktop. 
 */
void NonNull setclientdesktop(Client *c, struct Desktop *desktop);
/* Sets the clients wmatom[WMState] property. */
void NonNull setclientstate(Client *c, uint8_t state);
/* Sets the decor visibility. */
void NonNull setdecorvisible(Client *c, uint8_t state);
/* Sets the flag to disable border >>CHANGES<< for a client. */
void NonNull setdisableborder(Client *c, uint8_t state);
/* Sets the clients pid. */
void NonNull setclientpid(Client *c, pid_t pid);
/* Sets the flag mapstate, used in desktop optimizations */
void NonNull setmapstate(Client *c, enum WMMapState state);
/* Sets the Clients IS Desktop Flag. */
void NonNull setwtypedesktop(Client *c, uint8_t state);
/* Sets the Clients IS Dialog Flag. */
void NonNull setwtypedialog(Client *c, uint8_t state);
/* Sets the Clients IS Dock Flag. */
void NonNull setwtypedock(Client *c, uint8_t state);
/* Sets the Clients IS ToolBar Flag. */
void NonNull setwtypetoolbar(Client *c, uint8_t state);
/* Sets the Clients IS Menu Flag. */
void NonNull setwtypemenu(Client *c, uint8_t state);
/* Sets the Clients Never Focus Flag. */
void NonNull setwtypeneverfocus(Client *c, uint8_t state);
/* SeNonNull ts the Clients IS Utility Flag. */
void NonNull setwtypeutility(Client *c, uint8_t state);
/* Sets the Clients IS Splash Flag. */
void NonNull setwtypesplash(Client *c, uint8_t state);
/* Sets the Clients IS Dropdown Menu Flag. */
void NonNull setwtypedropdownmenu(Client *c, uint8_t state);
/* Sets the Clients IS Popup Menu Flag. */
void NonNull setwtypepopupmenu(Client *c, uint8_t state);
/* Sets the Clients IS Tool Tip Flag. */
void NonNull setwtypetooltip(Client *c, uint8_t state);
/* Sets the Clients IS Notification Flag. */
void NonNull setwtypenotification(Client *c, uint8_t state);
/* Sets the Clients IS Combo Flag. */
void NonNull setwtypecombo(Client *c, uint8_t state);
/* Sets the Clients IS DND Flag. */
void NonNull setwtypednd(Client *c, uint8_t state);
/* Sets the Clients IS Normal Flag. */
void NonNull setwtypenormal(Client *c, uint8_t state);
/* Sets the Clients IS Map Iconic Flag. */
void NonNull setwtypemapiconic(Client *c, uint8_t state);
/* Sets the Clients IS Map Normal Flag. */
void NonNull setwtypemapnormal(Client *c, uint8_t state);
/* Sets the Clients IS Take Focus Flag. */
void NonNull setwmtakefocus(Client *c, uint8_t state);
/* Sets the Clients IS Save Yourself Flag. */
void NonNull setwmsaveyourself(Client *c, uint8_t state);
/* Sets the Clients IS Delete Window Flag. */
void NonNull setwmdeletewindow(Client *c, uint8_t state);
/* Sets the Clients IS Skip Pager Flag. */
void NonNull setskippager(Client *c, uint8_t state);
/* Sets the Clients IS Skip Taskbar Flag. */
void NonNull setskiptaskbar(Client *c, uint8_t state);
/* Sets the Clients Show Decor Flag. */
void NonNull setshowdecor(Client *c, uint8_t state);
/* Makes a client fullscreen and take up the entire monitor. (also sets the isfullscreen flag)*/
void NonNull setfullscreen(Client *c, uint8_t isfullscreen);
/* Sets the Clients IS Floating Flag. */
void NonNull setfloating(Client *c, uint8_t state);
/* Sets the current Window Focus. */
void NonNull setfocus(Client *c);
/* Sets the Windows Map State (Iconic/Normal), and IS hidden Flag. */
void NonNull sethidden(Client *c, uint8_t state);
/* Sets the keep focus state flag 
 * NOTE: Client must already be in focus to work.
 */
void NonNull setkeepfocus(Client *c, uint8_t state);
/* Sets the "Maximized Vert" Flag */
void NonNull setmaximizedvert(Client *c, uint8_t state);
/* Sets the "Maximized Horz" Flag */
void NonNull setmaximizedhorz(Client *c, uint8_t state);
/* Sets the Clients IS Shaded Flag. */
void NonNull setshaded(Client *c, uint8_t state);
/* Sets the Clients IS Modal Flag. */
void NonNull setmodal(Client *c, uint8_t state);
/* Sets override redirect flag, which disallows attaching to any linked list for a desktop 
 * But still allows a client to be found using wintoclient()
 */
void NonNull setoverrideredirect(Client *c, uint8_t state);
/* Replaces the Clients state with the sticky state, and sets IS sticky Flag. */
void NonNull setsticky(Client *c, uint8_t state);
/* Updates a Clients state to Urgent, and sets the Urgent Flag. (Updates window border to urgen color.) */
void NonNull seturgent(Client *c, uint8_t isurgent);
/* Shows client if ISVISIBLE(c);
 * else hides client.
 */
void NonNull showhide(Client *c);
/* Returns the first client in desktop.
 * RETURN: Client * on Success.
 * RETURN: NULL on Failure. (no clients)
 */
Client *NonNull startclient(struct Desktop *desk);
/* Returns the first client in desktop.
 * RETURN: Client * on Success.
 * RETURN: NULL on Failure. (no clients)
 */
Client *NonNull startfocus(struct Desktop *desk);
/* Returns the first client in desktop stack.
 * RETURN: Client * on Success.
 * RETURN: NULL on Failure. (no clients)
 */
Client *NonNull startstack(struct Desktop *desk);
/* Returns the first client in desktop rstack.
 * RETURN: Client * on Success.
 * RETURN: NULL on Failure. (no clients)
 */
Client *NonNull startrstack(struct Desktop *desk);
/* Unfocuses specified client and sets to focus to root if setfocus is true */
void NonNull unfocus(Client *c, uint8_t setfocus);
/* updates a clients classname from XCBWMClass *_class 
 * No side effects on non filled _class dataw;
 */
void NonNullArg(1) updateclass(Client *c, XCBWMClass *_class);
/* Updates the Clients colormap */
void NonNull updatecolormap(Client *c, XCBColormap colormap);
void NonNull updatedecor(Client *c);
/* Updates the Client icon if we find one */
void NonNullAll updateicon(Client *c, XCBWindowProperty *iconprop);
/* Updates the Clients border properties to the XServer */
void NonNull updateborder(Client *c);
/* Updates the Clients border colour to the XServer */
void NonNull updatebordercol(Client *c);
/* Updates the Clients border width to the XServer */
void NonNull updateborderwidth(Client *c);
/* updates motif hints if they are set */
void NonNullArg(1) updatemotifhints(Client *c, XCBWindowProperty *motifprop);
/* Updates a Clients sizehints property using the provided hints pointer "size".
 * Doesnt require any data from client, AKA modular. still requires "size" though.
 */
void NonNullArg(1) updatesizehints(Client *c, XCBSizeHints *size);
/* Updates Client tile if we find one;
 * if none found default to main.h BROKEN
 */
void NonNullArg(1) updatetitle(Client *c, char *netwmname, char *wmname);
/* Updates Our own window protocol status (dont have to query every time) */
void NonNullArg(1) updatewindowprotocol(Client *c, XCBWMProtocols *protocols);
/* Updates Our own state based on Client state specified */
void NonNullArg(1) updatewindowstate(Client *c, XCBAtom state, uint8_t add_remove_toggle);
/* UpNonNull dates Our own states based on Client state specified */
void NonNullArg(1) updatewindowstates(Client *c, XCBAtom state[], uint32_t atomslength);
/* Updates Our own state based on windowtype in Client */
void NonNullArg(1) updatewindowtype(Client *c, XCBAtom wtype, uint8_t add_remove_toggle);
/* Updates Our own states based on windowtype in Client */
void NonNullArg(1) updatewindowtypes(Client *c, XCBAtom wtype[], uint32_t atomslength);
/* Updates WM_HINTS for specified Client */
void NonNullArg(1) updatewmhints(Client *c, XCBWMHints *hints);
/* Returns the client if found from the specified window 
 * RETURN: Client * on Success.
 * RETURN: NULL on Failure.
 */
Client *wintoclient(XCBWindow win);
/* Unmanages Client AKA we dont tell it what todo Nor does it use our resources;
 * And perform checks based on specified "destroyed";
 * 1 -> widthdraw window;
 * 0 -> skip checks (window already destroyed)
 */
void NonNull unmanage(Client *c, uint8_t destroyed);
/* unmaximizes a client if maxed, Sets flag. */
void NonNull unmaximize(Client *c);
/* unmaximizes a client horizontally if maxed horz, Sets flag. */
void NonNull unmaximizehorz(Client *c);
/* unmaximizes a client vertically if maxed vert, Sets flag. */
void NonNull unmaximizevert(Client *c);

/* MACROS */

uint32_t NonNull ISALWAYSONTOP(Client *c);
uint32_t NonNull ISALWAYSONBOTTOM(Client *c);
uint32_t NonNull WASFLOATING(Client *c);
uint32_t NonNull ISFLOATING(Client *c);
uint32_t NonNull ISOVERRIDEREDIRECT(Client *c);
uint32_t NonNull KEEPFOCUS(Client *c);
uint32_t NonNull DISABLEBORDER(Client *c);
uint32_t NonNull DOCKEDVERT(Client *c);
uint32_t NonNull DOCKEDHORZ(Client *c);
uint32_t NonNull DOCKED(Client *c);
enum FloatType NonNull COULDBEFLOATINGGEOM(Client *c);
enum FloatType NonNull COULDBEFLOATINGHINTS(Client *c);
bool NonNull SHOULDBEFLOATING(Client *c);
uint32_t NonNull SHOULDMAXIMIZE(Client *c);
uint32_t NonNull DOCKEDINITIAL(Client *c);
uint32_t NonNull WASDOCKEDVERT(Client *c);
uint32_t NonNull WASDOCKEDHORZ(Client *c);
uint32_t NonNull WASDOCKED(Client *c);
uint32_t SHOULDMANAGE(const XCBWindow window);
Client *NonNull REMOVECLIENTREFERENCES(Client *c);
uint32_t NonNull ISFIXED(Client *c);
uint32_t NonNull ISURGENT(Client *c);
/* flag */
uint32_t NonNull NEVERFOCUS(Client *c);
/* client state */
uint32_t NonNull NEVERHOLDFOCUS(Client *c);
uint32_t NonNull ISVISIBLE(Client *c);
/* TODO: XServer race conditions makes this unsuitable for usage */
 __DEPRECATED__ uint32_t NonNull ISMAPPED(Client *c); 
uint32_t NonNull SHOWDECOR(Client *c);
uint32_t NonNull ISSELECTED(Client *c);

/* EWMH Window types */
uint32_t NonNull ISDESKTOP(Client *c);
uint32_t NonNull ISDOCK(Client *c);
uint32_t NonNull ISTOOLBAR(Client *c);
uint32_t NonNull ISMENU(Client *c);
uint32_t NonNull ISUTILITY(Client *c);
uint32_t NonNull ISSPLASH(Client *c);
uint32_t NonNull ISDIALOG(Client *c);
uint32_t NonNull ISDROPDOWNMENU(Client *c);
uint32_t NonNull ISPOPUPMENU(Client *c);
uint32_t NonNull ISTOOLTIP(Client *c);
uint32_t NonNull ISNOTIFICATION(Client *c);
uint32_t NonNull ISCOMBO(Client *c);
uint32_t NonNull ISDND(Client *c);
uint32_t NonNull ISNORMAL(Client *c);
uint32_t NonNull ISMAPICONIC(Client *c);
uint32_t NonNull ISMAPNORMAL(Client *c);

/* EWMH Window states */
uint32_t NonNull ISMODAL(Client *c);
uint32_t NonNull ISSTICKY(Client *c);
uint32_t NonNull ISMAXIMIZEDVERT(Client *c);
uint32_t NonNull ISMAXIMIZEDHORZ(Client *c);
uint32_t NonNull ISSHADED(Client *c);
uint32_t NonNull SKIPTASKBAR(Client *c);
uint32_t NonNull SKIPPAGER(Client *c);
uint32_t NonNull ISHIDDEN(Client *c);
uint32_t NonNull ISFULLSCREEN(Client *c);
uint32_t NonNull ISABOVE(Client *c);
uint32_t NonNull ISBELOW(Client *c);
uint32_t NonNull DEMANDSATTENTION(Client *c);
uint32_t NonNull ISFOCUSED(Client *c);

/* WM Protocol */
uint32_t NonNull HASWMTAKEFOCUS(Client *c);
uint32_t NonNull HASWMSAVEYOURSELF(Client *c);
uint32_t NonNull HASWMDELETEWINDOW(Client *c);

uint16_t NonNull OLDWIDTH(Client *c);
uint16_t NonNull OLDHEIGHT(Client *c);
uint16_t NonNull WIDTH(Client *c);
uint16_t NonNull HEIGHT(Client *c);



#endif
