#ifndef _WM_MONITOR_H
#define _WM_MONITOR_H

#include <stdint.h>
#include "desktop.h"
#include "util.h"

/* cursor */
enum CurType 
{ 
    CurNormal, 
    CurResizeTopL, 
    CurResizeTopR, 
    CurMove, 
    CurLast 
}; 

enum ClientListModes
{
    ClientListAdd, ClientListRemove, ClientListReload,
};

typedef struct Monitor Monitor;

struct Monitor
{
    int16_t mx;                 /* Monitor X (Screen Area)                  */
    int16_t my;                 /* Monitor Y (Screen Area)                  */
    uint16_t mw;                /* Monitor Width (Screen Area)              */
    uint16_t mh;                /* Monitor Height (Screen Area)             */
    int16_t wx;                 /* Monitor X (Window Area)                  */
    int16_t wy;                 /* Monitor Y (Window Area)                  */
    uint16_t ww;                /* Monitor Width (Window Area)              */
    uint16_t wh;                /* Monitor Height (Window Area)             */

    Desktop *desktops;          /* First Desktop in linked list             */
    Desktop *desklast;          /* Last Desktop                             */
    Desktop *desksel;           /* Selected Desktop                         */
    Monitor *next;              /* Next Monitor                             */
    Client *bar;                /* The Associated Task-Bar (can be NULL)    */

    uint16_t deskcount;         /* Desktop Counter                          */
    uint8_t pad0[6];
};



/* Arranges and restacks all the windows for every deskop in the specified monitor.
*/
void NonNull arrangemon(Monitor *m);
/* Arrange and restacks every window on all monitors.
*/
void arrangemons(void);
/* Adds desktop to specified monitor linked list.
*/
void NonNullAll attachdesktop(Monitor *m, Desktop *desk);
/* Removes desktop fromt specified monitor linked list.
*/
void NonNullAll detachdesktop(Monitor *m, Desktop *desk);
/* Frees allocated cursors.
 */
void cleanupcursors(void);
/* Frees Monitor and allocated Monitor properties.
*/
void NonNull cleanupmon(Monitor *m);
/* Frees all monitors and allocated Monitor properties.
*/
void cleanupmons(void);
/* Allocates a Monitor and Monitor properties with all data set to 0 or to the adress of any newly allocated data.
 * RETURN: Monitor * on Success.
 * RETURN: exit(1) on Failure.
 */
Monitor *createmon(void);
/* Finds the next monitor based on "dir" AKA Direction. 
 * RETURN: Monitor * on Success.
 * RETURN: NULL on Failure.
 */
Monitor *dirtomon(uint8_t dir);
/* Returns the next Monitor avaible.
 * RETURN: Monitor* on Success.
 * RETURN: NULL on Failure.
 */
Monitor *FuncNullable nextmonitor(Monitor *monitor);
/* Sets the desktop count rolling back any clients to previous desktops. */
void NonNull setdesktopcount(Monitor *m, uint16_t desktops);
/* Sets the currently selected desktop */
void NonNullAll setdesktopsel(Monitor *mon, Desktop *desksel);
/* Sets the selected monitor and updates mon data */
void setmonsel(Monitor *m);
/* Sets up the cursors used for the WM. */
void setupcursors(void);
/* Updates 
 * type:            0       Adds the client win .
 *                  1       Removes the specified win.
 *                  2       Reloads the entire list.
 * _NET_WM_CLIENT_LIST */
void updateclientlist(XCBWindow win, uint8_t type);
/* Updates Geometry for external monitors based on if they have different geometry */
int  updategeom(void);
/* checks and updates mask if numlock is active */
void updatenumlockmask(void);
/* Returns the Monitor if found from the specified window 
 * RETURN: Monitor* on Success.
 * RETURN: NULL on Failure.
 */
Monitor *wintomon(XCBWindow win);










#endif
