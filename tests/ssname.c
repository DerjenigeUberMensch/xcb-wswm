
#include <X11/Xatom.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <string.h>
#include <unistd.h>


void
func(void)
{
    /* TODO, without this XServer sends way too many requests way too fast causing the window manager to hang till the event queue is clear.
     * A malicious app could hang the window manager.
     * How long to clear ~2-5 seconds per ~.5 seconds of execution, may vary on different systems.
     */
}

/* Stack Smash,
 * In this case overruning the event queue.
 * We attribute a somewhat deadly bug which if fast enough can cause the thread to overrun its own buffer, Why? This is somewhat of a async + XServer issue.
 * As of the creation of this script Stack Smash of this kind is no longer a issue but use to be.
 * Now what should happen is we just cause the cpu to spin up and take 100% cpu.
 * What shouldnt happen is the window manager crashes, (adress sanatizer) due to buffer overrun.
 */
int 
main(void)
{
    Display *dpy = XOpenDisplay(NULL);
    int screen = DefaultScreen(dpy);

    (void)screen;

    Window root = DefaultRootWindow(dpy);

    Window win = XCreateSimpleWindow(dpy, root, 0, 0, 1, 1, 0, 0, 0);
    XMapWindow(dpy, win);
    char *str = "x123049123049120-39120-3912";
    char *str2 = "1423120-9120-39120-391-3912";
    int i = 0;
    while(1)
    {
        if(i % 2)
        {   XChangeProperty(dpy, win, XA_WM_NAME, XA_WM_NAME, 32, PropModeAppend, (const unsigned char *)str, strlen(str) + 1);
        }
        else
        {   XChangeProperty(dpy, win, XA_WM_NAME, XA_WM_NAME, 32, PropModeReplace, (const unsigned char *)str2, strlen(str2) + 1);
        }
        ++i;
        func();
    }
    return 0;
}
