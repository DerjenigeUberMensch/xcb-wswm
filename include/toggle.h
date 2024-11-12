#ifndef TOGGLE_H_
#define TOGGLE_H_

#include "main.h"


/* Mostly a testing function */
void NonNull UserStats(const Arg *arg);
/* Makes a window sticky */
void NonNull StickWindow(const Arg *arg);
/* Mostly a testing function */
void NonNull UserStatsCallStack(const Arg *arg);
/* Switch to a monitor based on the argument int arg i */
void NonNull FocusMonitor(const Arg *arg);
/* Kills the current window */
void NonNull KillWindow(const Arg *arg);
/* Attempts to kill the current window directly instead of just sending a signal and waiting for the window to respond */
void NonNull TerminateWindow(const Arg *arg);
/* keybind to move the current window where the mouse cursor is */
void NonNull DragWindow(const Arg *arg);
/* Hard restart, execvp() */
void NonNull Restart(const Arg *arg);
/* Soft restart, self hosted, ie: do {} while(restart) */
void NonNull RestartQ(const Arg *arg);
/* quits wm */
void NonNull Quit(const Arg *arg);
/* resizes the current window based on mouse position */
void NonNull ResizeWindow(const Arg *arg);
/* resizes the current window based on mouse position no restrictions */
void NonNull ResizeWindowAlt(const Arg *arg);
/* sets the window layout based on a enum in main.h -> Grid, Floating, Monocle, Tiled */
void NonNull SetWindowLayout(const Arg *arg);
/* Spawns a window based on arguments provided */
void NonNull SpawnWindow(const Arg *arg);
/* Maximizes the currently selected window */
void NonNull MaximizeWindow(const Arg *arg);
/* Maximizes a window vertically */ 
void NonNull MaximizeWindowVertical(const Arg *arg);
/* Maximizes a window horizontally */
void NonNull MaximizeWindowHorizontal(const Arg *arg);
/* Toggles if we show the Status bar or not */
void NonNull ToggleStatusBar(const Arg *arg);
/* Toggles fullscreen mode for all windows in current tag */
void NonNull ToggleFullscreen(const Arg *arg);
/* Toggles desktop based on arg->ui, index starts at 0 */
void NonNull ToggleDesktop(const Arg *arg);

#endif
