#ifndef DWM_H
#define DWM_H

#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <stdio.h>

#include "settings.h"
#include "client.h"
#include "desktop.h"
#include "monitor.h"
#include "prop.h"
#include "getprop.h"
#include "x.h"
#include "safebool.h"
#include "../tools/util.h"


#ifndef VERSION
#define VERSION     "NOT_SET.NOT_SET.NOT_SET"
#endif

#ifndef MARK
#define MARK    "Generic X Server Event Handler (XEH)"
#endif

#define __CLIENTS__ __CLIENTS__

/* CONSTANTS */
#define CLEANMASK(mask)         (mask & ~(_wm.numlockmask|XCB_MOD_MASK_LOCK) & \
                                (XCB_MOD_MASK_SHIFT|XCB_MOD_MASK_CONTROL| \
                                 XCB_MOD_MASK_1|XCB_MOD_MASK_2|XCB_MOD_MASK_3|XCB_MOD_MASK_4|XCB_MOD_MASK_5))
#define INTERSECT(x,y,w,h,m)    (MAX(0, MIN((x)+(w),(m)->wx+(m)->ww) - MAX((x),(m)->wx)) \
                                * MAX(0, MIN((y)+(h),(m)->wy+(m)->wh) - MAX((y),(m)->wy)))
#define LENGTH(X)               (sizeof X / sizeof X[0])
#define SESSION_FILE            "/tmp/dwm-session"
#define SESSION_COLD_FILE       "/var/tmp/dwm-cold-session"
#define CONFIG_FILE             "/var/tmp/dwm-config"   /* todo make dir .config/dwm/config or someting like that */
#define BORKED                  "NOT_SET"

enum { BUTTONMASK = (XCB_EVENT_MASK_BUTTON_PRESS|XCB_EVENT_MASK_BUTTON_RELEASE) };
enum { MOUSEMASK = (BUTTONMASK | XCB_EVENT_MASK_POINTER_MOTION) };



/* Modern X implementations allof some ~2000 clients
 * But this would still only be roughly 10 xcb_change_property, \
 * which is still nothing compared to previous implementaitons,
 * Yes xcb_change_property is slow, very slow.
 */
enum 
X11DefaultsWM 
{ 
    X11_DEFAULT_MAX_WINDOW_LIMIT = 256,
};

enum 
SchemeType 
{ 
    SchemeNorm, 
    SchemeSel 
};

/* clicks */
enum 
ClkType 
{ 
    ClkTagBar, 
    ClkLtSymbol, 
    ClkStatusText, 
    ClkWinTitle,
    ClkClientWin, 
    ClkRootWin, 
    ClkLast 
};

enum
RestartType
{
    NoRestart,
    SoftRestart,
    HardRestart
};

typedef union  Arg Arg;
typedef struct Key Key;
typedef struct Button Button;
typedef struct WM WM;
typedef struct MotifWmHints MotifWmHints;

union Arg
{
    int32_t i;              /* i  -> int            */
    uint32_t ui;            /* ui -> unsigned int   */
    int64_t l;              /* l  -> long int       */
    uint64_t ul;            /* ul -> unsigned long  */
    float f;                /* f  -> float          */
    double d;               /* d  -> double         */
    void *v;                /* v  -> void pointer   */
};

struct Key
{
    uint16_t type;              /* KeyPress/KeyRelease  */
    uint16_t mod;               /* Modifier             */
    XCBKeysym keysym;           /* Key symbol           */
    void (*func)(const Arg *);  /* Function             */
    Arg arg;                    /* Argument             */
};

struct Button
{
    uint8_t type;                   /* ButtonPress/ButtonRelease    */
    uint8_t button;                 /* Button                       */
    uint16_t mask;                  /* Modifier                     */
    void (*func)(const Arg *arg);   /* Function                     */
    Arg arg;                        /* Argument                     */
};

struct WM
{
    int running;                    /* Running flag         */
    int numlockmask;                /* numlockmask          */
    int screen;                     /* Screen id            */

    uint16_t sw;                    /* Screen Height u16    */
    uint16_t sh;                    /* Screen Width  u16    */

    XCBWindow root;                 /* The root window      */
    XCBWindow wmcheckwin;           /* window manager check */

    XCBDisplay *dpy;                /* The current display  */
    Monitor *selmon;                /* Selected Monitor     */
    Monitor *mons;                  /* Monitors             */
    XCBKeySymbols *syms;            /* keysym alloc         */
    char *wmname;                   /* WM_NAME              */
    PropHandler *handler;           /* Prop Handler         */

    pthread_mutex_t mutex;          /* Mutex for main thread */
    uint8_t restart;                /* Restart flag         */
    uint8_t has_error;              /* Error flag           */
    uint8_t use_threads;            /* Use thread Flag      */
    uint8_t pad0[5];
};

struct MotifWmHints
{
    /* These correspond to XmRInt resources. (VendorSE.c) */
    uint32_t flags;
    uint32_t functions;
    uint32_t decorations;
    uint32_t input_mode;
    uint32_t status;
};

/* Checks given the provided information if a window is eligible to be a new bar.
 * if it is then it becomes the new bar.
 * RETURN: 0 on Success.
 * RETURN: 1 on no new bar (Failure).
*/
uint8_t NonNullAll checknewbar(Monitor *m, Client *c, const uint8_t has_strut_or_strut_partial);
/* Inital startup check if there is another window manager running.
*/
void checkotherwm(void);
/* Cleanups and frees any data previously allocated.
*/
void cleanup(void);
/* Jumps to the specified function handler for the provided event.
*/
void NonNull eventhandler(XCBGenericEvent *ev);
/* handles atexit.
*/
void exithandler(void);
/* Gets the root ptr location if possible, */
int8_t NonNullAll getrootptr(int16_t *x, int16_t *y);
/* Sends a event to the main event loop to stop running.
 */
void quit(void);
/* Attempts to restore session from SESSION_FILE for all monitors */
void restoresession(void);
/* Attempts to restore session from buff for a client */
Client *restoreclientsession(Desktop *desk, char *buff, uint16_t len);
/* Attempts to restore session from buff for a desktop */
Desktop *restoredesktopsession(Monitor *m, char *buff, uint16_t len);
/* Attempts to restore session from buff for given monitor */
Monitor *restoremonsession(char *buff, uint16_t len);
/* Attemps to restore sesion stack/focus/client variables 
 * RETURN: 1 on Success.
 * RETURN: 0 on Failure.
 */
int restorestacksession(Desktop *desk, char *buff, uint16_t len);
/* Searches through every monitor for a possible big enough size to fit rectangle parametors specified */
Monitor *recttomon(int16_t x, int16_t y, uint16_t width, uint16_t height);
/* Flags RESTART, and restarts using self hosting.
 */
void restart(void);
/* Flags RESTART and sets running to 0;
 * results in execvp(self) and "restarts"
 */
void restarthard(void);
/* Main event loop */
void run(void);
/* Attemps to save session in for every monitor */
void savesession(void);
/* Attemps to save session for specified client. 
 */
void saveclientsession(FILE *fw, Client *c);
/* Attemps to save session for specified desktop */
void savedesktopsession(FILE *fw, Desktop *desktop);
/* Attemps to save session from file for specified Monitor 
 */
void savemonsession(FILE *fw, Monitor *m);
/* Scans for new clients on startup */
void scan(void);
/* Sets up Variables, Checks, WM specific data, etc.. */
void setup(void);
/* Sets up Atoms ID's from the XServer */
void setupatoms(void);
/* Sets up special data. */
void NonNullAll setupbar(Monitor *m, Client *bar);
/* Loads CFG data into Settings struct. */
void setupcfg(void);
/* Sets up System related data */
void setupsys(void);
/* Loads WM with valid memory data */
void setupwm(void);
/* waits for childs (zombies) to die */
void sigchld(int signo);
/* Handles Signals and how we use them */
void sighandler(void);
/* Calls restart */
void sighup(int signo);
/* Calls quit */
void sigterm(int signo);
/* Error checks, and checks for certain conditions to be meet for different behaviour 
 * NOTE: This is only checked when the program is about to exit.
 */
void specialconds(int argc, char *argcv[]);
/* Vital checks and data setup before any other action is performed. */
void startup(void);
/* Starts the wm basic process */
void startupwm(void);
/* updates the Status Bar Position from given monitor */
void NonNull updatebarpos(Monitor *m);
/* updates the bar geometry from the given monitor */
void NonNull updatebargeom(Monitor *m);
/* Wakups the current X connection by sending a event to it */
void NonNull wakeupconnection(XCBDisplay *display, int screen);
/* Error handler */
void NonNullArg(1) xerror(XCBDisplay *display, XCBGenericError *error);

#endif 
