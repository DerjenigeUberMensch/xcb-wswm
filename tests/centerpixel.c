#include <X11/Xlib.h>
#include <X11/X.h>
#include "helper.h"

int
main(void)
{
    __test__start_basic();


    XEvent ev;
    GC gc = XCreateGC(dpy, win, 0, NULL);
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    XSelectInput(dpy, win, StructureNotifyMask|SubstructureNotifyMask|ExposureMask);
    XSetForeground(dpy, gc, 0);
    XSync(dpy, screen);
    while(!XNextEvent(dpy, &ev))
    {
        unsigned int sqw;
        unsigned int sqh;
        int sqx;
        int sqy;
        switch(ev.type)
        {
            case ConfigureNotify:
                x = ev.xconfigure.x;
                y = ev.xconfigure.y;
                w = ev.xconfigure.width;
                h = ev.xconfigure.height;
            case Expose:
                XFillRectangle(dpy, win, gc, 0, 0, w, h);
                XSetForeground(dpy, gc, ~0);
                sqw = sw / 10;
                sqh = sw / 10;
                //unsigned int sqx = w / 2 - sqw / 2;
                //unsigned int sqy = h / 2 - sqh / 2;
                //sqx = (((sw / 2) - sqw) - x) + sqw / 2;
                //sqy = (((sh / 2) - sqh) - y) + sqh / 2;
                sqx = ((sw / 2)) - sqw / 2 - x;
                sqy = ((sh / 2)) - sqh / 2 - y;
                XFillArc(dpy, win, gc, sqx, sqy, sqw, sqh, 1 * 64, 360 * 64);
                //XFillRectangle(dpy, win, gc, sqx, sqy, sqw, sqh);
                XSetForeground(dpy, gc, 0);
                break;
        }
    }



    return 0;
}
