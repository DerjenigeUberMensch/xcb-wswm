#ifndef TOGGLE_H_
#define TOGGLE_H_

#include "main.h"


/* Mostly a testing function */
void UserStats(const Arg *arg);
/* Makes a window sticky */
void StickWindow(const Arg *arg);
/* Mostly a testing function */
void UserStatsCallStack(const Arg *arg);
/* Switch to a monitor based on the argument int arg i */
void FocusMonitor(const Arg *arg);
/* Kills the current window */
void KillWindow(const Arg *arg);
/* Attempts to kill the current window directly instead of just sending a signal and waiting for the window to respond */
void TerminateWindow(const Arg *arg);
/* keybind to move the current window where the mouse cursor is */
void DragWindow(const Arg *arg);
/* Hard restart, execvp() */
void Restart(const Arg *arg);
/* Soft restart, self hosted, ie: do {} while(restart) */
void RestartQ(const Arg *arg);
/* quits wm */
void Quit(const Arg *arg);
/* resizes the current window based on mouse position */
void ResizeWindow(const Arg *arg);
/* resizes the current window based on mouse position no restrictions */
void ResizeWindowAlt(const Arg *arg);
/* sets the window layout based on a enum in main.h -> Grid, Floating, Monocle, Tiled */
void SetWindowLayout(const Arg *arg);
/* Spawns a window based on arguments provided */
void SpawnWindow(const Arg *arg);
/* Maximizes the currently selected window */
void MaximizeWindow(const Arg *arg);
/* Maximizes a window vertically */ 
void MaximizeWindowVertical(const Arg *arg);
/* Maximizes a window horizontally */
void MaximizeWindowHorizontal(const Arg *arg);
/* Toggles if we show the Status bar or not */
void ToggleStatusBar(const Arg *arg);
/* Toggles fullscreen mode for all windows in current tag */
void ToggleFullscreen(const Arg *arg);
/* Toggles desktop based on arg->ui, index starts at 0 */
void ToggleDesktop(const Arg *arg);
#endif
