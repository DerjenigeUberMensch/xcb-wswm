#include "helper.h"

#include <stdint.h>


int
main(void)
{
    __test__start_basic();
    Atom wtype = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", False);
    Atom wstate = XInternAtom(dpy, "_NET_WM_STATE", False);

    Atom above = XInternAtom(dpy, "_NET_WM_STATE_ABOVE", False);
    Atom modal = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_MODAL", False);
    Atom dialog= XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DIALOG", False);
    Atom dock  = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DOCK", False);
    Atom below = XInternAtom(dpy, "_NET_WM_STATE_BELOW", False);


    (void)modal;
    (void)dialog;

    __test__start_basic();


    const int win_count = 5;
    Window wins[win_count];


    unsigned int red = 255;
    unsigned int blue = 255 << 8;
    unsigned int green = 255 << 16;


    unsigned int color = 0;
    int i;
    for(i = 0; i < win_count; ++i)
    {
        switch(i % 3)
        {
            case 0:
                color ^= red * rand();
            case 1:
                color ^= blue * rand();
            case 2:
                color ^= green * rand();
            default:
                color ^= rand();
                while(color == UINT32_MAX || color == 0)
                {   color ^= 255 * rand();
                }
                break;
        }
        Atom atom;
        Atom state;
        wins[i] = __Create_Window(color, 0, 0);
        if((color & red) % 255 > 150)
        {   
            atom = above;
            state = wstate;
        }
        else if((color & blue) % 255 > 100)
        {
            atom = dock;
            state = wtype;
        }
        else if((color & green) % 255 > 50)
        {   
            atom = below;
            state = wstate;
        }
        else
        {
            atom = above;
            state = wstate;
        }
        XChangeProperty(dpy, wins[i], state, XA_ATOM, 32, PropModeReplace, (const unsigned char *)&atom, 1);
    }

    XEvent ev;
    while(!XNextEvent(dpy, &ev));

    return 0;
}
