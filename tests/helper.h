#include <X11/Xatom.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


static Display *dpy;
static int screen;
static int sw;
static int sh;
static Window win;
static Window root;

static void
__test__start_basic(void)
{
    dpy = XOpenDisplay(NULL);
    if(!dpy)
    {   
        printf("Could not load display\n");
        exit(1);
    }
    screen = DefaultScreen(dpy);
    sw = DisplayWidth(dpy, screen);
    sh = DisplayHeight(dpy, screen);
    

    root = DefaultRootWindow(dpy);
    srand(time(NULL));
}

static Window
__Create_Window(unsigned int color, int bw, int bwcolor)
{
    XSetWindowAttributes wa =
    {
        .bit_gravity = StaticGravity,
        .backing_store = WhenMapped,
        .border_pixel = bwcolor,
        .background_pixel = color
    };
    unsigned long int mask = CWBitGravity|CWBackingStore|CWBorderPixel|CWBorderWidth|CWBackPixel;
    win = XCreateWindow(dpy, root, 0, 0, sw, sh, bw, DefaultDepth(dpy, screen), InputOutput, 0, mask, &wa);
    XMapWindow(dpy, win);
    return win;
}
