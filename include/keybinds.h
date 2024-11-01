#ifndef KEYBINDS_H_
#define KEYBINDS_H_

#include "toggle.h"
#include "main.h"
#include <X11/keysym.h>
#include <X11/XF86keysym.h> 

/* key definitions */
/* common keys */
#define ALT         XCBMod1Mask
#define NUMLOCK     XCBMod2Mask
#define SUPER       XCBMod4Mask         /* "Command Key" "Windows Key" */
#define CTRL        XCBControlMask
#define SHIFT       XCBShiftMask
#define CAPSLOCK    XCBLockMask
#define TAB         XK_Tab
#define LMB         XCBButton1
#define MMB         XCBButton2
#define RMB         XCBButton3
#define ScrollUp    XCBButton4
#define ScrollDown  XCBButton5

/* taken from i3 */
#define ScrollLeft  6
#define ScrollRight 7


#define ENTER       XK_Return
#define RETURN      XK_Return
/* F1 - F12 */
#define XF1         XK_F1
#define XF2         XK_F2
#define XF3         XK_F3
#define XF4         XK_F4
#define XF5         XK_F5
#define XF6         XK_F6
#define XF7         XK_F7
#define XF8         XK_F8
#define XF9         XK_F9
#define XF10        XK_F10
#define XF11        XK_F11
#define XF12        XK_F12


/* XF86 */
#define XAudioMute                  XF86XK_AudioMute
#define XAudioLowerVolume           XF86XK_AudioLowerVolume
#define XAudioRaiseVolume           XF86XK_AudioRaiseVolume
#define XMonLowerBrightness         XF86XK_MonBrightnessDown
#define XMonRaiseBrightness         XF86XK_MonBrightnessUp
#define XAudioPlay                  XF86XK_AudioPlay
#define XAudioPause                 XF86XK_AudioPause
#define XAudioNext                  XF86XK_AudioNext
#define XAudioPrev                  XF86XK_AudioPrev

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }


/* assuming systemd */
#define SUSPEND             SHCMD("systemctl suspend")
#define POWEROFF            SHCMD("shutdown now")

/* Multimedia Keys (laptops and function keyboards) */
static const char *up_vol[]   = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "+1%",   NULL };
static const char *down_vol[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-1%",   NULL };
static const char *mute_vol[] = { "pactl", "set-sink-mute",   "@DEFAULT_SINK@", "toggle", NULL };
static const char *pause_vol[]= { "playerctl", "play-pause"};
static const char *next_vol[] = { "playerctl", "next"};
static const char *prev_vol[] = { "playerctl", "previous"};
static const char *brighter[] = { "brightnessctl", "set", "1%+", NULL };
static const char *dimmer[]   = { "brightnessctl", "set", "1%-", NULL };

#define CFG_DMENU_TOP_BAR           0       /* 1 show dmenu bar on top; 0 for bottom bar                        */
#define CFG_DMENU_FAST_INPUT        0       /* 1 prioritize input over bar render; 0 to disable                 */
#define CFG_DMENU_CASE_SENSITIVE    1       /* 1 dmenu searches case senstivly; 0 to disable                    */
#define CFG_DMENU_COL_NORM_BACKGROUND       "#000000" /* dmenu background colour for NON selected items */
#define CFG_DMENU_COL_NORM_FOREGROUND       "#ffffff" /* dmenu text colour for NON selected items       */
#define CFG_DMENU_COL_SEL_BACKGROUND        "#000000" /* dmenu background colour for SELECTED items     */ 
#define CFG_DMENU_COL_SEL_FOREGROUND        "#ffffff" /* dmenu text colour for SELECTED items           */
/* commands */
static char dmenumon[2] = "0"; 
static const char dmenufont[]   =   {"monospace:size=12"};
static const char *dmenucmd[] = 
{ 
    "dmenu_run", "-m", dmenumon, "-fn", dmenufont, 
    "-nb", CFG_DMENU_COL_NORM_BACKGROUND, "-nf", CFG_DMENU_COL_NORM_FOREGROUND, 
    "-sb", CFG_DMENU_COL_SEL_BACKGROUND, "-sf", CFG_DMENU_COL_SEL_FOREGROUND, 
    CFG_DMENU_TOP_BAR ? NULL : "-b", CFG_DMENU_FAST_INPUT ? "-f" : NULL ,CFG_DMENU_CASE_SENSITIVE ? "-i" : NULL, NULL
}; /* flags -b == bottom bar; -f == getkeyboard input first then handle request; */

static const char *termcmd[]        = { "st", NULL};
static const char *filemanager[2]    = {"thunar", NULL };
static const Key keys[] =
{
/*  Action                 modifier                 key                     function            argument */
    /* debugging */
    { XCBKeyPress,         SUPER,                   XK_n,                   UserStats,          {0} },
    { XCBKeyPress,         SUPER|ALT,               XK_n,                   UserStatsCallStack, {0} },

    /* actual stuff */
    { XCBKeyPress,         SUPER,                   XK_d,                   SpawnWindow,        {.v = dmenucmd } },
    { XCBKeyPress,         SUPER,                   RETURN,                 SpawnWindow,        {.v = termcmd } },
    { XCBKeyPress,         SUPER,                   XK_e,                   SpawnWindow,        {.v = filemanager } },
    { XCBKeyPress,         SUPER,                   XK_b,                   ToggleStatusBar,    {0} },
    { XCBKeyPress,         SUPER|SHIFT,             XK_q,                   KillWindow,         {0} },
    { XCBKeyPress,         CTRL|ALT,                XK_q,	                TerminateWindow,    {0} },
    { XCBKeyPress,         SUPER,                   XK_w,                   MaximizeWindow,     {0} },
    { XCBKeyPress,         SUPER|SHIFT,             XK_p,                   Quit,               {0} },
    { XCBKeyPress,         SUPER|CTRL,              XK_p,                   RestartQ,           {0} },  /* UNSAFE sscanf() */
    { XCBKeyPress,         CTRL|ALT,                XK_p,                   Restart,            {0} },  /* UNSAFE sscanf() */
    { XCBKeyPress,         SUPER|CTRL,              XK_z,                   StickWindow,        {0} },
    { XCBKeyPress,         SUPER,                   XK_z,                   SetWindowLayout,    {Tiled} },
    { XCBKeyPress,         SUPER,                   XK_x,                   SetWindowLayout,    {Floating} },
    { XCBKeyPress,         SUPER,                   XK_c,                   SetWindowLayout,    {Monocle} },
    { XCBKeyPress,         SUPER,                   XK_g,                   SetWindowLayout,    {Grid} },
    { XCBKeyPress,         0,                       XF11,                   ToggleFullscreen,   {0} },

    /* desktop switching */
    { XCBKeyPress,          SUPER,                  XK_1,                   ToggleDesktop,      {.ui = 0} },
    { XCBKeyPress,          SUPER,                  XK_2,                   ToggleDesktop,      {.ui = 1} },
    { XCBKeyPress,          SUPER,                  XK_3,                   ToggleDesktop,      {.ui = 2} },
    { XCBKeyPress,          SUPER,                  XK_4,                   ToggleDesktop,      {.ui = 3} },
    { XCBKeyPress,          SUPER,                  XK_5,                   ToggleDesktop,      {.ui = 4} },
    { XCBKeyPress,          SUPER,                  XK_6,                   ToggleDesktop,      {.ui = 5} },
    { XCBKeyPress,          SUPER,                  XK_7,                   ToggleDesktop,      {.ui = 6} },
    { XCBKeyPress,          SUPER,                  XK_8,                   ToggleDesktop,      {.ui = 7} },
    { XCBKeyPress,          SUPER,                  XK_9,                   ToggleDesktop,      {.ui = 8} },
    /* due to keyboard layouts starting at 1 we assume this is the "tenth" */
    { XCBKeyPress,          SUPER,                  XK_0,                   ToggleDesktop,      {.ui = 9} },

    /* multimedia keys */
    { XCBKeyPress,         0,                       XAudioMute,             SpawnWindow,        {.v = mute_vol } },
    { XCBKeyPress,         0,                       XAudioLowerVolume,      SpawnWindow,        {.v = down_vol } },
    { XCBKeyPress,         0,                       XAudioRaiseVolume,      SpawnWindow,        {.v = up_vol } },
    { XCBKeyPress,         0,                       XMonLowerBrightness,    SpawnWindow,        {.v = dimmer } },
    { XCBKeyPress,         0,                       XMonRaiseBrightness,    SpawnWindow,        {.v = brighter } },
    { XCBKeyPress,         0,                       XAudioPlay,             SpawnWindow,        {.v = pause_vol } },
    { XCBKeyPress,         0,                       XAudioPause,            SpawnWindow,        {.v = pause_vol } },
    { XCBKeyPress,         0,                       XAudioNext,             SpawnWindow,        {.v = next_vol } },
    { XCBKeyPress,         0,                       XAudioPrev,             SpawnWindow,        {.v = prev_vol } },

};

static const Button buttons[] = 
{
    /* Type                 Button      Mask        function        arg */
    { XCBButtonPress,       RMB,        SUPER,      ResizeWindow,   {0} },
    { XCBButtonRelease,     RMB,        SUPER,      0,              {0} },

    /* debug */
    { XCBButtonPress,       RMB,        SUPER|ALT,  ResizeWindowAlt,   {0} },
    { XCBButtonRelease,     RMB,        SUPER|ALT,  0,              {0} },

    { XCBButtonPress,       LMB,        SUPER,      DragWindow,     {0} },
    { XCBButtonRelease,     LMB,        SUPER,      0,              {0} },
};


#endif
