/**
 * @file xcb_trl.c
 * Code implementations.
 */


/* most of these implementations have been tanken from the tutorial, other websites, and or just straight reordering
 * some however have been taken from the source itself
 */



/*
 * NOTE: while xcb claims to be big speed it doesnt hesitate to call malloc.
 */



/*
 * TODO: xcb for some reason doesnt have a 32bit version of alot of the xlib to xcb ports for some reason... so we have to do it ourselves
 */

#include "xcb_trl.h"

#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_event.h>
#include <xcb/xcbext.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xcb_cursor.h>
#include <xcb/xinerama.h>
#include <xcb/xcb_xrm.h>
#include <xcb/xcb_errors.h>


/* error codes */
#include <X11/Xproto.h>

#include <stdio.h>
#include <string.h>
#include <limits.h> /* UINT_MAX */
#include <stdarg.h>

/** Number of elements in this structure */
#define XCB_ICCCM_NUM_WM_HINTS_ELEMENTS 9




typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;


#ifdef XCB_TRL_ENABLE_DEBUG
#define DBG             1
#define DGB             1
#define GDB             1
#define BDG             1
#define GBD             1
#define _fn             __func__
#endif
#define _XCB_MANUAL_DEBUG(fmt, ...)             (fprintf(stderr, "_XCB_DEBUG_ [%s:%d] by %s(): " fmt "\n", __FILE__,__LINE__,__func__,__VA_ARGS__))
#define _XCB_MANUAL_DEBUG0(X)                   (fprintf(stderr, "_XCB_DEBUG_ [%s:%d] by %s(): " X "\n", __FILE__, __LINE__, __func__))

/* HELPER FUNCTION */
static XCBScreen *
screen_of_display(
        XCBDisplay *display, 
        int screen
        )
{
    xcb_screen_iterator_t iter;

    iter = xcb_setup_roots_iterator(xcb_get_setup(display));
    for(; iter.rem; --screen, xcb_screen_next(&iter))
    {
        if(screen == 0)
        {   return iter.data;
        }
    }
    return NULL;
}

static void
color_parser_foreground(
        XCBColor *foreground, 
        uint16_t *red, 
        uint16_t *green, 
        uint16_t *blue
        )
{
    if(foreground)
    {
        if(foreground->flags & XCBDoRed)
        {   *red = foreground->red;
        }
        if(foreground->flags & XCBDoGreen)
        {   *green = foreground->green;
        }
        if(foreground->flags & XCBDoBlue)
        {   *blue = foreground->blue;
        }
    }

}
static void
color_parser_background(
        XCBColor *background, 
        uint16_t *red, 
        uint16_t *green, 
        uint16_t *blue
        )
{
    if(background)
    {
        if(background->flags & XCBDoRed)
        {   *red = background->red;
        }
        if(background->flags & XCBDoGreen)
        {   *green = background->green;
        }
        if(background->flags & XCBDoBlue)
        {   *blue = background->blue;
        }
    }
}


#ifdef DBG
#define _xcb_push_func(cookie) \
    _xcb_caller item;           \
    item.id = cookie.sequence;  \
    item.name = (char *)_fn;    \
                                \
    if(_xcb_full_func())        \
    {                           \
        /* we dont care it will just wrap back but this might be useful in testing */   \
        _XCB_MANUAL_DEBUG0("DEBUG Ran Out of Space wrapping data...");                  \
    }                                                                                   \
    front *= front != -1;       \
    rear = (rear + 1) % MAX_DEBUG_LIMIT;    \
    _xcb_funcs[rear] = item;
#else
#define _xcb_push_func(cookie) ((void)cookie)
#endif
#ifdef DBG


typedef struct _xcb_caller _xcb_caller;

struct _xcb_caller 
{
    unsigned int id;
    /* TODO: make like a static char *table and just use a uint16_t and save some 6 bytes */
    char *name;
};

#define MAX_DEBUG_LIMIT     ((32768 << 6))/* the number means nothing its just some random big enough number for most use cases */
                
_xcb_caller _xcb_funcs[MAX_DEBUG_LIMIT];
long long rear = -1;
long long front = -1;


/*
static void
_xcb_jmpck(XCBDisplay *d, XCBGenericError *err)
{
    fprintf(stderr, "_XCB_DEBUG_\n");
    fprintf(stderr, "%s %s\n", XCBGetErrorCodeText(err->error_code), XCBGetErrorMajorCodeText(err->major_code));
    fprintf(stderr, 
            "error_code:    [%d]\n"
            "major_code:    [%d]\n"
            "minor_code:    [%d]\n"
            "sequence:      [%d]\n"
            "response_type: [%d]\n"
            "resource_id:   [%d]\n"
            "full_sequence: [%d]\n"
              ,
           err->error_code, err->major_code, err->minor_code, 
           err->sequence, err->response_type, err->resource_id, 
           err->full_sequence);

}
*/


static int _xcb_full_func(void);
static int _xcb_empty_func(void);

/*
static void _xcb_pop_func(XCBCookie cookie);

static void
_xcb_pop_func(XCBCookie cookie)
{
    if(_xcb_empty_func())
    {   
        _XCB_MANUAL_DEBUG0("Nothing to pop.");
        return;
    }

    _xcb_funcs[front].name = NULL;
    _xcb_funcs[front].id = 0;
    if(front == rear)
    {
        front = -1;
        rear = -1;   
    }
    else
    {   front = (front + 1) % MAX_DEBUG_LIMIT;
    }
}

*/

static int
_xcb_full_func(void)
{
    return front == rear + 1 || (front == 0 && rear == MAX_DEBUG_LIMIT - 1);
}

static int
_xcb_empty_func(void)
{
    return front == -1;
}



/*
static void
_xcb_show_call_stack(void)
{
    if(_xcb_empty_func())
    {   _XCB_MANUAL_DEBUG0("No stack to show");
    }
    else
    {
        for(long long i = front; i != rear; i = (i + 1) % MAX_DEBUG_LIMIT)
        {   _XCB_MANUAL_DEBUG("%s", _xcb_funcs[i].name);
        }
    }
}

*/

void  
XCBBreakPoint(void) 
{   volatile int e; e = 2; if(e == 2) { return; }
}
#endif

char *
XCBDebugGetCallStack(void)
{
    char *stack = NULL;
#ifdef DBG
    long long size = 0;
    long long charsize = 0;
    /* yeah idk */
    for(long long i = front; i != rear; i = (i + 1) % MAX_DEBUG_LIMIT, ++size)
    {   charsize += strlen(_xcb_funcs[i].name);
    }
    /* +size cause we need spaces, and +1 cause we need \0 */
    stack = calloc(1, charsize * sizeof(char) + size * sizeof(char) + 1);
    if(stack)
    {   
        if(_xcb_empty_func())
        {   
            free(stack);
            return NULL;
        }
        for(long long i = front; i != rear; i = (i + 1) % MAX_DEBUG_LIMIT)
        {   strcat(stack, _xcb_funcs[i].name);
            strcat(stack, "\n");
        }
        strcat(stack, "\0");
    }
#endif
    return stack;
}

char *
XCBDebugGetLastCall(void)
{
    const char *lastcall = NULL;
#ifdef DBG
    if(!_xcb_empty_func())
    {
        if(_xcb_funcs[rear].name)
        {   lastcall = _xcb_funcs[rear].name;
        }
    }
#endif
    return (char *)lastcall;
}

char *
XCBDebugGetFirstCall(void)
{
    char *firstcall = NULL;
#if DBG
    if(!_xcb_empty_func())
    {
        if(_xcb_funcs[front].name)
        {   firstcall = _xcb_funcs[front].name;
        }
    }
#endif
    return firstcall;
}

/* TODO */
char *
XCBDebugGetAdjacentCallers(
        XCBCookie cookie
        )
{
    /* prob could use a hashmap or something */
#ifdef DBG
    unsigned int id = cookie.sequence;
    for(long long i = front; i != rear; i = (i + 1) % MAX_DEBUG_LIMIT)
    {
        if(_xcb_funcs[i].id == id)
        {   
        }
    }
#endif
    return NULL;
}

char *
XCBDebugGetNameFromId(
        XCBCookie id
        )
{
#ifdef XCB_TRL_ENABLE_DEBUG
    for(long long i = front; i != rear; i = (i + 1) % MAX_DEBUG_LIMIT)
    {   
        if(_xcb_funcs[i].id == id.sequence)
        {   return _xcb_funcs[i].name;
        }
    }
#endif
    return NULL;
}

static void
_xcb_handler(
        XCBDisplay *display, 
        XCBGenericError *err
        )
{
#ifdef XCB_TRL_ENABLE_DEBUG
    for(long long i = front; i != rear; i = (i + 1) % MAX_DEBUG_LIMIT)
    {   
        if(_xcb_funcs[i].id == err->error_code)
        {   _XCB_MANUAL_DEBUG("%s", _xcb_funcs[i].name);
        }
    }
#endif
}
                                                        /* this saves a conditional check which isnt "expensive".
                                                         * But is basically free no overhead aside from the function call which your gonna do anyway.
                                                         */
static void (*_handler)(XCBDisplay *, XCBGenericError *) = _xcb_handler;




static void
_xcb_err_handler(
        XCBDisplay *display, 
        XCBGenericError *err
        )
{
    _handler(display, err);
    /* this just for convinience also less lines of code I guess? */
    free(err);
#ifdef DBG
    XCBBreakPoint();
#endif
}

static void
__XCBThrowError(
        XCBDisplay *display,
        XCBCookie cookie, 
        uint8_t error, 
        uint8_t major_code, 
        uint16_t minor_code
        )
{
    XCBGenericError _err;
    memset(&_err, 0, sizeof(XCBGenericError));
    _err.error_code = error;
    _err.major_code = major_code;
    _err.minor_code = minor_code;
    _err.response_type = XCB_NONE;
    _err.full_sequence = cookie.sequence;
    _err.sequence = cookie.sequence;
    _xcb_handler(display, &_err);
}

static const uint32_t
__XValidFormat(uint8_t x) 
{
    switch(x)
    {
        case 32:
        case 16:
        case 8:
            return 1;
    }
    return 0;
}
/* Automaticalyly frees data on error */
static const uint32_t
__XValidFormatThrow(
        XCBDisplay *display, 
        XCBCookie cookie,
        XCBWindowProperty *prop
        )
{
    const uint32_t ret = __XValidFormat(prop->format);
    if(ret)
    {   return 1;
    }
    __XCBThrowError(display, cookie, XCBBadImplementation, X_GetProperty, XCB_NONE);
    free(prop);
    return 0;
}

static const uint32_t
__XValidSize(
        uint32_t value_len
        ) 
{
    /* X11, GetProp.c
     * Protect against both integer overflow and just plain oversized
     * memory allocation - no server should ever return this many props.
     */
    return value_len <= (INT32_MAX >> 4);
}

static const uint32_t
__XValidSizeThrow(
        XCBDisplay *display, 
        XCBCookie cookie,
        XCBWindowProperty *prop
        )
{
    const uint32_t ret = __XValidSize(prop->value_len);
    if(ret)
    {   return 1;
    }
    __XCBThrowError(display, cookie, XCBBadImplementation, X_GetProperty, XCB_NONE);
    return 0;
}

XCBDisplay *
XCBOpenDisplay(
        const char *displayName, 
        int *defaultScreenReturn
        )
{
    XCBDisplay *display = xcb_connect(displayName, defaultScreenReturn);
    /* We need to check return type cause xcb_connect never returns NULL (for some reason).
     * So we just check error, which requires display.
     */


    XCBCookie ck = { .sequence = 0 };
    _xcb_push_func(ck);


    if(!display || xcb_connection_has_error(display))
    {   

        /* This frees the connection data that was allocated. */
        xcb_disconnect(display);
        return NULL;
    }
    return display;
}
XCBDisplay *
XCBOpenConnection(
        const char *displayName, 
        int *defaultScreenReturn
        )
{

    XCBCookie ck = { .sequence = 0 };
    _xcb_push_func(ck);

    return xcb_connect(displayName, defaultScreenReturn);
}
void 
XCBCloseDisplay(
        XCBDisplay *display
        )
{
    /* Closes connection and frees resulting data. */

    XCBCookie ck = { .sequence = 9999999 };
    _xcb_push_func(ck);

    xcb_disconnect(display);
}

int 
XCBDisplayNumber(
        XCBDisplay *display
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    /* gets the file descriptor AKA the connection number */
    return xcb_get_file_descriptor(display);
}

int
XCBConnectionNumber(
        XCBDisplay *display
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    /* gets the file descriptor AKA the connection number */
    return xcb_get_file_descriptor(display);
}

XCBScreen *
XCBScreenOfDisplay(
        XCBDisplay *display, 
        int screen
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return xcb_aux_get_screen(display, screen);
}

XCBScreen *
XCBDefaultScreenOfDisplay(
        XCBDisplay *display, 
        int screen
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return screen_of_display(display, screen);
}


int 
XCBScreenCount(
        XCBDisplay *display
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return xcb_setup_roots_iterator (xcb_get_setup (display)).rem;
}

char *
XCBServerVendor(
        XCBDisplay *display
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    char *vendor = NULL;
    int length = 0;

    length = xcb_setup_vendor_length (xcb_get_setup (display));
    vendor = (char *)malloc (length + 1);
    if (vendor)
    {
        memcpy (vendor, xcb_setup_vendor (xcb_get_setup (display)), length);
        vendor[length] = '\0';
    }
    return vendor;
}


/* All of these use xcb_get_setup
 * The reason we can is because the access is essential O(1)
 * This is because xcb_get_setup() simply calls a error check and returns a adress
 * display->setup basically
 */

int 
XCBProtocolVersion(
        XCBDisplay *display
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return xcb_get_setup(display)->protocol_major_version;
}

int 
XCBProtocolRevision(
        XCBDisplay *display
        )
{   

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return xcb_get_setup(display)->protocol_minor_version;
}

int
XCBVendorRelease(
        XCBDisplay *display
        )
{   

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return xcb_get_setup (display)->release_number;
}

int
XCBBitmapUnit(
        XCBDisplay *display
        )
{   

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return xcb_get_setup(display)->bitmap_format_scanline_unit;
}

int
XCBBitmapBitOrder(
        XCBDisplay *display
        )
{   

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return xcb_get_setup(display)->bitmap_format_bit_order;
}

int
XCBBitmapPad(
        XCBDisplay *display
        )
{   

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return xcb_get_setup(display)->bitmap_format_scanline_pad;
}

int
XCBImageByteOrder(
        XCBDisplay *display
        )
{  

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return xcb_get_setup(display)->image_byte_order;
}

const XCBSetup *
XCBGetSetup(
        XCBDisplay *display
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return xcb_get_setup(display);
}

XCBScreen *
XCBGetScreen(
        XCBDisplay *display
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    /* could alos use xcb_aux_get_screen()*/   
    return xcb_setup_roots_iterator(xcb_get_setup(display)).data;
}

XCBWindow
XCBRootWindow(
        XCBDisplay *display, 
        int screen
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    /* Gets the screen structure */
    return xcb_aux_get_screen(display, screen)->root;
}

XCBWindow
XCBDefaultRootWindow(
        XCBDisplay *display, 
        int screen
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    XCBScreen *scr = screen_of_display(display, screen);
    if(scr) 
    {   return scr->root;
    }
    return 0; /* AKA NULL; AKA we didnt find a screen */
}

u16 
XCBDisplayWidth(
        XCBDisplay *display, 
        int screen
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    /* Gets the screen structure */
    return xcb_aux_get_screen(display, screen)->width_in_pixels;
}
u16
XCBDisplayHeight(
        XCBDisplay *display, 
        int screen
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    /* Gets the screen structure */
    return xcb_aux_get_screen(display, screen)->height_in_pixels;
}
u8
XCBDefaultDepth(
        XCBDisplay *display, 
        int screen
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    /* Gets the screen structure */
    return xcb_aux_get_screen(display, screen)->root_depth;
}

XCBCookie
XCBSelectInput(
        XCBDisplay *display, 
        XCBWindow window, 
        u32 mask
        )
{
    XCBCookie ret = xcb_change_window_attributes(display, window, XCB_CW_EVENT_MASK, &mask);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBSetInputFocus(
        XCBDisplay *display, 
        XCBWindow window, 
        u8 revert_to, 
        XCBTimestamp tim
        )
{
    XCBCookie ret = xcb_set_input_focus(display, revert_to, window, tim);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBChangeWindowAttributes(
        XCBDisplay *display, 
        XCBWindow window, 
        u32 mask, 
        XCBWindowAttributes *window_attributes
        )
{
    XCBCookie ret = xcb_change_window_attributes_aux(display, window, mask, window_attributes);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBInstallColormap(
        XCBDisplay *display,
        XCBColormap colormap
        )
{
    XCBCookie ret = xcb_install_colormap(display, colormap);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBUninstallColormap(
        XCBDisplay *display,
        XCBColormap colormap
        )
{
    XCBCookie ret = xcb_uninstall_colormap(display, colormap);

    _xcb_push_func(ret);

    return ret;
}

u32
XCBBlackPixel(
        XCBDisplay *display, 
        int screen
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    const XCBScreen *scr = screen_of_display(display, screen);
    if(scr)
    {   return scr->black_pixel;
    }
    return 0; /* AKA NULL; AKA we didnt find a screen */
}

u32
XCBWhitePixel(
        XCBDisplay *display, 
        int screen
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    const XCBScreen *scr = screen_of_display(display, screen);
    if(scr)
    {   return scr->white_pixel;
    }
    return 0; /* AKA NULL; AKA we didnt find a screen */
}

void
XCBSync(
       XCBDisplay *display
       )
{ 

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    /* "https://community.kde.org/Xcb"
     * The xcb equivalent of XSync() is xcb_aux_sync(), which is in xcb-utils.
     * The reason you won't find a sync function in libxcb is that there is no sync request in the X protocol. 
     * Calling XSync() or xcb_aux_sync() is equivalent to calling XGetInputFocus() and throwing away the reply.
     */
    xcb_aux_sync(display);
}

void
XCBSyncf(
        XCBDisplay *display
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    /* analagous to XSync(display, True) */
    XCBSync(display);
    XCBGenericEvent *ev = NULL;
    while(((ev = XCBPollForQueuedEvent(display))))
    {   
        if(!ev->response_type)
        {   _xcb_err_handler(display, (xcb_generic_error_t *)ev);
        }
        else
        {   free(ev);
        }
    }
}


XCBCookie
XCBReparentWindow(
        XCBDisplay *display, 
        XCBWindow source, 
        XCBWindow parent, 
        i32 x, 
        i32 y
        )
{
    XCBCookie ret = xcb_reparent_window(display, source, parent, x, y);

    _xcb_push_func(ret);

    return ret;
}


XCBCookie
XCBMoveWindow(
        XCBDisplay *display, 
        XCBWindow window, 
        i32 x, 
        i32 y
        )
{
    const i32 values[4] = { x, y };
    const u16 mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y;
    XCBCookie ret = xcb_configure_window(display, window, mask, values);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBMoveResizeWindow(
        XCBDisplay *display, 
        XCBWindow window, 
        i32 x, 
        i32 y, 
        u32 width, 
        u32 height
        )
{
    /* fucks up sometimes so this will do */
    const i32 value1[2] = { x, y };
    const u16 mask1 = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y;
    const u32 value2[2] = { width, height };
    const u16 mask2 = XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
    xcb_configure_window(display, window, mask1, value1);
    XCBCookie ret = xcb_configure_window(display, window, mask2, value2);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBResizeWindow(
        XCBDisplay *display, 
        XCBWindow window, 
        u32 width, 
        u32 height
        )
{
    const u32 values[2] = { width, height };
    const u32 mask = XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
    XCBCookie ret = xcb_configure_window(display, window, mask, values);

    _xcb_push_func(ret);


    return ret;
}

XCBCookie
XCBRaiseWindow(
        XCBDisplay *display, 
        XCBWindow window
        )
{
    const u32 values = XCB_STACK_MODE_ABOVE;
    const u32 mask = XCB_CONFIG_WINDOW_STACK_MODE;
    XCBCookie ret = xcb_configure_window(display, window, mask, &values);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBMapRaised(
        XCBDisplay *display, 
        XCBWindow window
        )
{
    xcb_map_window(display, window);
    XCBCookie ret = XCBRaiseWindow(display, window);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBLowerWindow(
        XCBDisplay *display, 
        XCBWindow window
        )
{
    const u32 values = XCB_STACK_MODE_BELOW;
    const u32 mask = XCB_CONFIG_WINDOW_STACK_MODE;
    XCBCookie ret = xcb_configure_window(display, window, mask, &values);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBRaiseWindowIf(
        XCBDisplay *display, 
        XCBWindow window
        )
{
    const u32 values = XCB_STACK_MODE_TOP_IF;
    const u32 mask = XCB_CONFIG_WINDOW_STACK_MODE;
    XCBCookie ret = xcb_configure_window(display, window, mask, &values);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBLowerWindowIf(
        XCBDisplay *display, 
        XCBWindow window
        )
{
    const u32 values = XCB_STACK_MODE_BOTTOM_IF;
    const u32 mask = XCB_CONFIG_WINDOW_STACK_MODE;
    XCBCookie ret = xcb_configure_window(display, window, mask, &values);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBRaiseLowerWindow(
        XCBDisplay *display, 
        XCBWindow window
        )
{
    const u32 values = XCB_STACK_MODE_OPPOSITE;
    const u32 mask = XCB_CONFIG_WINDOW_STACK_MODE;
    XCBCookie ret = xcb_configure_window(display, window, mask, &values);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBSetWindowBorder(
        XCBDisplay *display, 
        XCBWindow window, 
        uint32_t border_pixel
        )
{
    const u32 values = border_pixel;
    const u32 mask = XCB_CW_BORDER_PIXEL;
    XCBCookie ret = xcb_change_window_attributes(display, window, mask, &values);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBSetWindowBorderWidth(
        XCBDisplay *display, 
        XCBWindow window, 
        u16 border_width
        )
{
    /* format must be i32 or u32 so we cant just pass the adress of u16 border_width */
    const u32 values = border_width;
    const u32 mask = XCB_CONFIG_WINDOW_BORDER_WIDTH;
    XCBCookie ret = xcb_configure_window(display, window, mask, &values);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBSetSibling(
        XCBDisplay *display, 
        XCBWindow window, 
        XCBWindow sibling
        )
{
    const u32 values = sibling;
    const u32 mask = XCB_CONFIG_WINDOW_SIBLING;
    XCBCookie ret = xcb_configure_window(display, window, mask, &values);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBAddToSaveSet(
        XCBDisplay *display,
        XCBWindow win
        )
{
    XCBCookie ret = xcb_change_save_set(display, XCB_SET_MODE_INSERT, win);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBChangeSaveSet(
        XCBDisplay *display,
        XCBWindow win,
        uint8_t change_mode
        )
{
    XCBCookie ret = xcb_change_save_set(display, change_mode, win);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBRemoveFromSaveSet(
        XCBDisplay *display,
        XCBWindow win
        )
{
    XCBCookie ret = xcb_change_save_set(display, XCB_SET_MODE_DELETE, win);

    _xcb_push_func(ret);

    return ret;
}


XCBCookie
XCBGetWindowAttributesCookie(
        XCBDisplay *display, 
        XCBWindow window
        )
{
    const xcb_get_window_attributes_cookie_t cookie = xcb_get_window_attributes(display, window);
    XCBCookie ret = { .sequence = cookie.sequence };

    _xcb_push_func(ret);

    return ret;
}

XCBGetWindowAttributes *
XCBGetWindowAttributesReply(
        XCBDisplay *display, 
        XCBCookie cookie
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    XCBGenericError *err = NULL;
    XCBGetWindowAttributes *reply = NULL;
    const xcb_get_window_attributes_cookie_t cookie1 = { .sequence = cookie.sequence };
    reply = xcb_get_window_attributes_reply(display, cookie1, &err);
    if(err)
    {
        _xcb_err_handler(display, err);
        if(reply)
        {   free(reply);
        }
        return NULL;
    }
    return reply;
}

XCBCookie
XCBGetWindowGeometryCookie(
        XCBDisplay *display,
        XCBWindow window
        )
{
    const xcb_get_geometry_cookie_t cookie = xcb_get_geometry(display, window);
    XCBCookie ret = { .sequence = cookie.sequence };

    _xcb_push_func(ret);

    return ret;
}

XCBGeometry *
XCBGetWindowGeometryReply(
        XCBDisplay *display, 
        XCBCookie cookie
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    XCBGenericError *err = NULL;
    XCBGeometry *reply = NULL;
    const xcb_get_geometry_cookie_t cookie1 = { .sequence = cookie.sequence };
    reply = xcb_get_geometry_reply(display, cookie1, &err);
    if(err)
    {   
        _xcb_err_handler(display, err);
        if(reply)
        {   free(reply);
        }
        return NULL;
    }
    return reply;
}


XCBCookie
XCBGetGeometryCookie(
        XCBDisplay *display,
        XCBWindow window)
{
    XCBCookie ret = XCBGetWindowGeometryCookie(display, window);

    _xcb_push_func(ret);

    return ret;
}

XCBGeometry *
XCBGetGeometryReply(
        XCBDisplay *display,
        XCBCookie cookie
        )
{
    XCBGeometry *reply = XCBGetWindowGeometryReply(display, cookie);

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return reply;
}


XCBCookie
XCBInternAtomCookie(XCBDisplay *display, const char *name, uint8_t only_if_exists)
{
    const xcb_intern_atom_cookie_t cookie = xcb_intern_atom(display, only_if_exists, strlen(name), name);
    XCBCookie ret = { .sequence = cookie.sequence };


    _xcb_push_func(ret);


    return ret;
}

XCBAtom
XCBInternAtomReply(XCBDisplay *display, XCBCookie cookie)
{
    XCBGenericError *err = NULL;
    const xcb_intern_atom_cookie_t cookie1 = { .sequence = cookie.sequence };
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(display, cookie1, &err);

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);


#ifdef DBG
    if(reply && reply->length > 1)
    {
        _XCB_MANUAL_DEBUG0("There are several possible atoms for the provided cookie, this might be important.");
        XCBBreakPoint();
    }
#endif


    if(err)
    {
        _xcb_err_handler(display, err);
        if(reply)
        {   free(reply);
        }
        return 0;
    }
    const xcb_atom_t atom = reply->atom;
    free(reply);
    return atom;
}

XCBCookie
XCBGetTransientForHintCookie(
        XCBDisplay *display, 
        XCBWindow win)
{
    xcb_get_property_cookie_t cookie = xcb_get_property(display, 0, win, XCB_ATOM_WM_TRANSIENT_FOR, XCB_ATOM_WINDOW, 0, 1);
    XCBCookie ret = { .sequence = cookie.sequence };

    _xcb_push_func(ret);

    return ret;
}

u8
XCBGetTransientForHintReply(
        XCBDisplay *display,
        XCBCookie cookie,
        XCBWindow *win
        )
{
    XCBWindowProperty *reply = XCBGetPropertyReply(display, cookie);
    u8 status = 1;


    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);


    if(!reply || reply->type != XCB_ATOM_WINDOW || reply->format != 32 || !reply->length)
    {   status = 0;
    }
    else
    {
        if(win)
        {   *win = *((xcb_window_t *) xcb_get_property_value(reply));
        }
        else
        {   
            status = 0;
            
            #ifdef DBG
                _XCB_MANUAL_DEBUG0("No adress specified in the 'win' field.");
                XCBBreakPoint();
            #endif
        }
    }

    free(reply);
    return status;
}

XCBCookie
XCBGetPropertyCookie(
        XCBDisplay *display,
        XCBWindow window,
        XCBAtom property,
        uint32_t long_offset,
        uint32_t long_length,
        uint8_t _delete,
        XCBAtom req_type
        )
{
    const xcb_get_property_cookie_t cookie = xcb_get_property(display, _delete, window, property, req_type, long_offset, long_length);
    XCBCookie ret = {.sequence = cookie.sequence };

    _xcb_push_func(ret);

    return ret;
}

XCBCookie 
XCBGetWindowPropertyCookie(
        XCBDisplay *display,
        XCBWindow window,
        XCBAtom property,
        uint32_t long_offset,
        uint32_t long_length,
        uint8_t _delete,
        XCBAtom req_type
        )
{
    const xcb_get_property_cookie_t cookie = xcb_get_property(display, _delete, window, property, req_type, long_offset, long_length);
    XCBCookie ret = {.sequence = cookie.sequence };

    _xcb_push_func(ret);

    return ret;
}

XCBWindowProperty *
XCBGetWindowPropertyReply(
        XCBDisplay *display,
        XCBCookie cookie
        )
{
    XCBWindowProperty *reply = XCBGetPropertyReply(display, cookie);

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return reply;
}

void *
XCBGetWindowPropertyValue(
        XCBWindowProperty *reply
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return xcb_get_property_value(reply);
}

u8
XCBGetWindowPropertyValueSize(
        XCBWindowProperty *reply,
        uint32_t *_ret
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    enum 
    {
        __XCB__FORMAT__8 = 8,
        __XCB__FORMAT__16 = 16,
        __XCB__FORMAT__32 = 32,
    };
    switch(reply->format)
    {
        case __XCB__FORMAT__8:
        case __XCB__FORMAT__16:
        case __XCB__FORMAT__32:
            *_ret = xcb_get_property_value_length(reply);
            return 0;
        default:
            *_ret = reply->value_len;
            return 1;
            break;
    }
}

u8
XCBGetWindowPropertyValueLength
(
        XCBWindowProperty *reply, 
        size_t size,
        uint32_t *_ret
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    const size_t safesize = size + !size;
    enum 
    {
        __XCB__FORMAT__8 = 8,
        __XCB__FORMAT__16 = 16,
        __XCB__FORMAT__32 = 32,
    };
    /* XCB malforms some requests for some reason :/ so go back to failsafe of reply->value_len. 
     * Which?: Some icons are malformed as to say size "0" when they do have a actual size.
     */
    switch(reply->format)
    {
        case __XCB__FORMAT__8:
        case __XCB__FORMAT__16:
        case __XCB__FORMAT__32:
            *_ret = xcb_get_property_value_length(reply) / safesize;
            return 0;
        default:
            *_ret = reply->value_len / safesize;
            return 1;
            break;
    }
}

XCBWindowProperty *
XCBGetPropertyReply(
        XCBDisplay *display,
        XCBCookie cookie
        )
{
    XCBGenericError *err = NULL;
    const xcb_get_property_cookie_t cookie1 = { .sequence = cookie.sequence };
    xcb_get_property_reply_t *reply = xcb_get_property_reply(display, cookie1, &err);

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);


    if(err)
    {
        _xcb_err_handler(display, err);
        if(reply)
        {   free(reply);
        }
        return NULL;
    }
    if(reply && reply->type != XCB_NONE)
    {   
        if(!__XValidFormatThrow(display, cookie, reply) || !__XValidSizeThrow(display, cookie, reply))
        {   
            free(reply);
            reply = NULL;
        }
    }
    return reply;
}

void *
XCBGetPropertyValue(
        XCBWindowProperty *reply
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return xcb_get_property_value(reply);
}

uint8_t 
XCBGetPropertyValueSize(
        XCBWindowProperty *reply,
        u32 *_ret
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return XCBGetWindowPropertyValueSize(reply, _ret);
}

uint8_t 
XCBGetPropertyValueLength(
        XCBWindowProperty *reply,
        size_t size,
        uint32_t *_ret
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return XCBGetWindowPropertyValueLength(reply, size, _ret);
}


XCBPixmap
XCBCreatePixmap(
        XCBDisplay *display, 
        XCBWindow root, 
        u16 width, 
        u16 height, 
        u8 depth
        )
{
    const XCBPixmap id = xcb_generate_id(display);
    const XCBCookie ret = xcb_create_pixmap(display, depth, id, root, width, height);

    _xcb_push_func(ret);


    (void)ret;

    return id;
}

XCBCookie
XCBCopyArea(
    XCBDisplay *display, 
    XCBDrawable source, 
    XCBDrawable destination, 
    XCBGC gc, 
    int16_t SourceStartCopyX, 
    int16_t SourceStartCopyY,
    uint16_t CopyWidth,
    uint16_t CopyHeight,
    int16_t DestinationStartPasteX,
    int16_t DestinationStartPasteY
    )
{
    XCBCookie ret = xcb_copy_area(
        display, 
        source, 
        destination, 
        gc, 
        SourceStartCopyX, 
        SourceStartCopyY,
        DestinationStartPasteX,
        DestinationStartPasteY,
        CopyWidth,
        CopyHeight
        );

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBFreePixmap(
        XCBDisplay *display, 
        XCBPixmap pixmap
        )
{
    XCBCookie ret = xcb_free_pixmap(display, pixmap);

    _xcb_push_func(ret);

    return ret;
}
/* Cursors */

XCBCursor
XCBCreateFontCursor(
        XCBDisplay *display, 
        XCBFont shape
        )
{
    /* fg = foreground; bg = background */

    /* black */
    const u16 fgred = 0;
    const u16 fggreen = 0;
    const u16 fgblue = 0;

    /* white */
    const u16 bgred = UINT16_MAX;
    const u16 bggreen = UINT16_MAX;
    const u16 bgblue = UINT16_MAX;

    const xcb_font_t font = xcb_generate_id(display);
    const u8 strlenofcursor = 6;    /* X only reads data for those 6 chars so no need for +1 for the \0 character */

    xcb_open_font(display, font, strlenofcursor, "cursor");

    const xcb_cursor_t id = xcb_generate_id(display);

    XCBCookie ret = xcb_create_glyph_cursor(display, id, font, font, shape, shape + 1,
                                            fgred, fggreen, fgblue,
                                            bgred, bggreen, bgblue
                                            );

    _xcb_push_func(ret);


    (void)ret;

    return id;
}

XCBCursor
XCBCreateGlyphCursor(
        XCBDisplay *display,
        XCBFont shape,
        XCBColor *foreground,
        XCBColor *background
        )
{
    u16 fgred = 0;
    u16 fggreen = 0;
    u16 fgblue = 0;

    u16 bgred = 0;
    u16 bggreen = 0;
    u16 bgblue = 0;

    color_parser_foreground(foreground, &fgred, &fggreen, &fgblue);
    color_parser_background(background, &bgred, &bggreen, &bgblue);

    const xcb_font_t font = xcb_generate_id(display);
    const u8 strlenofcursor = 6;    /* X only reads data for those 6 chars so no need for +1 for the \0 character */

    xcb_open_font(display, font, strlenofcursor, "cursor");

    const xcb_cursor_t id = xcb_generate_id(display);

    XCBCookie ret = xcb_create_glyph_cursor(display, id, font, font, shape, shape + 1,
                                            fgred, fggreen, fgblue,
                                            bgred, bggreen, bgblue
                                            );

    _xcb_push_func(ret);


    (void)ret;

    return id;
}

XCBCookie
XCBFreeCursor(
        XCBDisplay *display, 
        XCBCursor cursor
        )
{
    XCBCookie ret = xcb_free_cursor(display, cursor);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBDefineCursor(
        XCBDisplay *display, 
        XCBWindow window, 
        XCBCursor id
        )
{   
    const u32 mask = XCB_CW_CURSOR;
    XCBCookie ret = xcb_change_window_attributes(display, window, mask, &id);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie 
XCBRecolorCursor(
        XCBDisplay *display, 
        XCBCursor cursor, 
        XCBColor *foreground, 
        XCBColor *background
        )
{
    u16 fgred = 0;
    u16 fggreen = 0;
    u16 fgblue = 0;

    u16 bgred = 0;
    u16 bggreen = 0;
    u16 bgblue = 0;

    color_parser_foreground(foreground, &fgred, &fggreen, &fgblue);
    color_parser_background(background, &bgred, &bggreen, &bgblue);

    XCBCookie ret = xcb_recolor_cursor(display, cursor, fgred, fggreen, fgblue, bgred, bggreen, bgblue);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBOpenFont(
        XCBDisplay *display, 
        XCBFont id, 
        const char *name
        )
{
    const u16 len = strlen(name);
    XCBCookie ret = xcb_open_font(display, id, len, name);

    _xcb_push_func(ret);


    return ret;
}

XCBCookie
XCBCloseFont(
        XCBDisplay *display, 
        XCBFont id
        )
{
    XCBCookie ret = xcb_close_font(display, id);

    _xcb_push_func(ret);

    return ret;
}


/* text property textproperty */

XCBCookie
XCBGetTextPropertyCookie(
        XCBDisplay *display, 
        XCBWindow window, 
        XCBAtom property
        )
{
    const xcb_get_property_cookie_t cookie = xcb_get_property(display, 0, window, property, XCB_GET_PROPERTY_TYPE_ANY, 0, UINT_MAX);
    XCBCookie ret = {.sequence = cookie.sequence };


    _xcb_push_func(ret);


    return ret;
}

int 
XCBGetTextPropertyReply(
        XCBDisplay *display, 
        XCBCookie cookie, 
        XCBTextProperty *reply_return
        )
{
    u8 status = 1;

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    XCBWindowProperty *reply = XCBGetPropertyReply(display, cookie);
    
    if(!reply)
    {   status = 0;
    }
    else
    {
        reply_return->_reply = reply;
        reply_return->encoding = reply_return->_reply->type;
        reply_return->format = reply_return->_reply->format;
        reply_return->name = xcb_get_property_value(reply_return->_reply);
        /* char so doesnt matter */
        XCBGetWindowPropertyValueLength(reply_return->_reply, sizeof(char), &reply_return->name_len);
    }

    free(reply);
    return status;
}
int
XCBFreeTextProperty(
        XCBTextProperty *prop
        )
{   

    if(!prop)
    {
    #ifdef DBG
        _XCB_MANUAL_DEBUG0("No prop in context.");
        XCBBreakPoint();
    #endif
    }


    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    free(prop->_reply);
    return 1;
}

int 
XCBFlush(
        XCBDisplay *display
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    /* This locks the XServer thread (pthread mutix lock).
     * and writes the buffer to the server.
     * RETURN 1 on Success.
     * RETURN 0 on Failure.
     */
    return xcb_flush(display);
}

u32
XCBGetMaximumRequestLength(
        XCBDisplay *display
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    /* <Straight from the documentation.>
     *
     * In the absence of the BIG-REQUESTS extension, returns the
     * maximum request length field from the connection setup data, which
     * may be as much as 65535. If the server supports BIG-REQUESTS, then
     * the maximum request length field from the reply to the
     * BigRequestsEnable request will be returned instead.
     *
     * Note that this length is measured in four-byte units, making the
     * theoretical maximum lengths roughly 256kB without BIG-REQUESTS and
     * 16GB with.
     */
    return xcb_get_maximum_request_length(display);
}

int
XCBCheckDisplayError(
        XCBDisplay *display
        )
{   

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return xcb_connection_has_error(display);
}
int 
XCBHasConnectionError(
        XCBDisplay *display
        )
{   

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return xcb_connection_has_error(display);
}
int 
XCBCheckConnectionError(
        XCBDisplay *display
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return xcb_connection_has_error(display);
}
int 
XCBHasDisplayError(
        XCBDisplay *display
        )
{   

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return xcb_connection_has_error(display);
}

int
XCBSetErrorHandler(
        void (*error_handler)(
            XCBDisplay *, 
            XCBGenericError *
            )
        )
{   
    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    if(error_handler)
    {   _handler = error_handler;
    }
    else
    {   _handler = _xcb_handler;
    }
    return 1;
}

void
XCBSetIOErrorHandler(
        XCBDisplay *display, 
        void *IOHandler
        )
{   

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);


    _XCB_MANUAL_DEBUG0("Not Implemented.");

    /* not implemented */
    (void)display;
    (void)IOHandler;
    return;
}

char *
XCBGetEventName(
        uint8_t response_type
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    /* cant use const cause compiler complains boo hoo */
    XCBGenericEvent ev = { .response_type = response_type };
    return XCBGetEventNameFromEvent(&ev);
}

char *
XCBGetEventNameFromEvent(
        XCBGenericEvent *event
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    const char *const evs[36] = 
    {
        [XCB_NONE] = NULL,
        [XCB_KEY_PRESS] = "XCB_KEY_PRESS", 
        [XCB_KEY_RELEASE] = "XCB_KEY_RELEASE", 
        [XCB_BUTTON_PRESS] = "XCB_BUTTON_PRESS", 
        [XCB_BUTTON_RELEASE] = "XCB_BUTTON_RELEASE", 
        [XCB_MOTION_NOTIFY] = "XCB_MOTION_NOTIFY", 
        [XCB_ENTER_NOTIFY] = "XCB_ENTER_NOTIFY", 
        [XCB_LEAVE_NOTIFY] = "XCB_LEAVE_NOTIFY", 
        [XCB_FOCUS_IN] = "XCB_FOCUS_IN", 
        [XCB_FOCUS_OUT] = "XCB_FOCUS_OUT", 
        [XCB_KEYMAP_NOTIFY] = "XCB_KEYMAP_NOTIFY", 
        [XCB_EXPOSE] = "XCB_EXPOSE", 
        [XCB_GRAPHICS_EXPOSURE] = "XCB_GRAPHICS_EXPOSURE", 
        [XCB_NO_EXPOSURE] = "XCB_NO_EXPOSURE", 
        [XCB_VISIBILITY_NOTIFY] = "XCB_VISIBILITY_NOTIFY", 
        [XCB_CREATE_NOTIFY] = "XCB_CREATE_NOTIFY", 
        [XCB_DESTROY_NOTIFY] = "XCB_DESTROY_NOTIFY", 
        [XCB_UNMAP_NOTIFY] = "XCB_UNMAP_NOTIFY", 
        [XCB_MAP_NOTIFY] = "XCB_MAP_NOTIFY", 
        [XCB_MAP_REQUEST] = "XCB_MAP_REQUEST", 
        [XCB_REPARENT_NOTIFY] = "XCB_REPARENT_NOTIFY", 
        [XCB_CONFIGURE_NOTIFY] = "XCB_CONFIGURE_NOTIFY", 
        [XCB_CONFIGURE_REQUEST] = "XCB_CONFIGURE_REQUEST", 
        [XCB_GRAVITY_NOTIFY] = "XCB_GRAVITY_NOTIFY", 
        [XCB_RESIZE_REQUEST] = "XCB_RESIZE_REQUEST", 
        [XCB_CIRCULATE_NOTIFY] = "XCB_CIRCULATE_NOTIFY", 
        [XCB_CIRCULATE_REQUEST] = "XCB_CIRCULATE_REQUEST", 
        [XCB_PROPERTY_NOTIFY] = "XCB_PROPERTY_NOTIFY", 
        [XCB_SELECTION_CLEAR] = "XCB_SELECTION_CLEAR", 
        [XCB_SELECTION_REQUEST] = "XCB_SELECTION_REQUEST", 
        [XCB_SELECTION_NOTIFY] = "XCB_SELECTION_NOTIFY", 
        [XCB_COLORMAP_NOTIFY] = "XCB_COLORMAP_NOTIFY", 
        [XCB_CLIENT_MESSAGE] = "XCB_CLIENT_MESSAGE", 
        [XCB_MAPPING_NOTIFY] = "XCB_MAPPING_NOTIFY", 
        [XCB_GE_GENERIC] = "XCB_GE_GENERIC", 
    };
    if(!event)
    {   return NULL;
    }
    u8 cleanev = XCB_EVENT_RESPONSE_TYPE(event);
    /* bounds check */    /* & over && for better inlining */
    cleanev *= (cleanev > 0) & (cleanev < 35);

    return (char *const )evs[cleanev];
}

char *
XCBGetErrorCodeText(
        uint8_t error_code)
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    static const char *const errs[18] =
    {
        [0] = NULL,
        [XCBBadRequest] = "BadRequest",
        [XCBBadValue] = "BadValue",
        [XCBBadWindow] = "BadWindow",
        [XCBBadPixmap] = "BadPixmap",
        [XCBBadAtom] = "BadAtom",
        [XCBBadCursor] = "BadCursor",
        [XCBBadFont] = "BadFont",
        [XCBBadMatch] = "BadMatch",
        [XCBBadDrawable] = "BadDrawable",
        [XCBBadAccess] = "BadAccess",
        [XCBBadAlloc] = "BadAlloc",
        [XCBBadColor] = "BadColor",
        [XCBBadGC] = "BadGC",
        [XCBBadIDChoice] = "BadIDChoice",
        [XCBBadName] = "BadName",
        [XCBBadLength] = "BadLength",
        [XCBBadImplementation] = "BadImplementation",
    };
    /* bounds check */          /* & over && for better inlining */
    error_code *= (error_code > 0) & (error_code < 18);
    return (char *const)errs[error_code];
}

char *
XCBGetErrorMajorCodeText(
        uint8_t major_code)
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    static const char *const errs[128] = 
    {
        [0] = NULL,
        [X_CreateWindow] = "CreateWindow",
        [X_ChangeWindowAttributes] = "ChangeWindowAttributes",
        [X_GetWindowAttributes] = "GetWindowAttributes",
        [X_DestroyWindow] = "DestroyWindow",
        [X_DestroySubwindows] = "DestroySubwindows",
        [X_ChangeSaveSet] = "ChangeSaveSet",
        [X_ReparentWindow] = "ReparentWindow",
        [X_MapWindow] = "MapWindow",
        [X_MapSubwindows] = "MapSubwindows",
        [X_UnmapWindow] = "UnmapWindow",
        [X_UnmapSubwindows] = "UnmapSubwindows",
        [X_ConfigureWindow] = "ConfigureWindow",
        [X_CirculateWindow] = "CirculateWindow",
        [X_GetGeometry] = "GetGeometry",
        [X_QueryTree] = "QueryTree",
        [X_InternAtom] = "InternAtom",
        [X_GetAtomName] = "GetAtomName",
        [X_ChangeProperty] = "ChangeProperty",
        [X_DeleteProperty] = "DeleteProperty",
        [X_GetProperty] = "GetProperty",
        [X_ListProperties] = "ListProperties",
        [X_SetSelectionOwner] = "SetSelectionOwner",
        [X_GetSelectionOwner] = "GetSelectionOwner",
        [X_ConvertSelection] = "ConvertSelection",
        [X_SendEvent] = "SendEvent",
        [X_GrabPointer] = "GrabPointer",
        [X_UngrabPointer] = "UngrabPointer",
        [X_GrabButton] = "GrabButton",
        [X_UngrabButton] = "UngrabButton",
        [X_ChangeActivePointerGrab] = "ChangeActivePointerGrab",
        [X_GrabKeyboard] = "GrabKeyboard",
        [X_UngrabKeyboard] = "UngrabKeyboard",
        [X_GrabKey] = "GrabKey",
        [X_UngrabKey] = "UngrabKey",
        [X_AllowEvents] = "AllowEvents",
        [X_GrabServer] = "GrabServer",
        [X_UngrabServer] = "UngrabServer",
        [X_QueryPointer] = "QueryPointer",
        [X_GetMotionEvents] = "GetMotionEvents",
        [X_TranslateCoords] = "TranslateCoords",
        [X_WarpPointer] = "WarpPointer",
        [X_SetInputFocus] = "SetInputFocus",
        [X_GetInputFocus] = "GetInputFocus",
        [X_QueryKeymap] = "QueryKeymap",
        [X_OpenFont] = "OpenFont",
        [X_CloseFont] = "CloseFont",
        [X_QueryFont] = "QueryFont",
        [X_QueryTextExtents] = "QueryTextExtents",
        [X_ListFonts] = "ListFonts",
        [X_ListFontsWithInfo] = "ListFontsWithInfo",
        [X_SetFontPath] = "SetFontPath",
        [X_GetFontPath] = "GetFontPath",
        [X_CreatePixmap] = "CreatePixmap",
        [X_FreePixmap] = "FreePixmap",
        [X_CreateGC] = "CreateGC",
        [X_ChangeGC] = "ChangeGC",
        [X_CopyGC] = "CopyGC",
        [X_SetDashes] = "SetDashes",
        [X_SetClipRectangles] = "SetClipRectangles",
        [X_FreeGC] = "FreeGC",
        [X_ClearArea] = "ClearArea",
        [X_CopyArea] = "CopyArea",
        [X_CopyPlane] = "CopyPlane",
        [X_PolyPoint] = "PolyPoint",
        [X_PolyLine] = "PolyLine",
        [X_PolySegment] = "PolySegment",
        [X_PolyRectangle] = "PolyRectangle",
        [X_PolyArc] = "PolyArc",
        [X_FillPoly] = "FillPoly",
        [X_PolyFillRectangle] = "PolyFillRectangle",
        [X_PolyFillArc] = "PolyFillArc",
        [X_PutImage] = "PutImage",
        [X_GetImage] = "GetImage",
        [X_PolyText8] = "PolyText8",
        [X_PolyText16] = "PolyText16",
        [X_ImageText8] = "ImageText8",
        [X_ImageText16] = "ImageText16",
        [X_CreateColormap] = "CreateColormap",
        [X_FreeColormap] = "FreeColormap",
        [X_CopyColormapAndFree] = "CopyColormapAndFree",
        [X_InstallColormap] = "InstallColormap",
        [X_UninstallColormap] = "UninstallColormap",
        [X_ListInstalledColormaps] = "ListInstalledColormaps",
        [X_AllocColor] = "AllocColor",
        [X_AllocNamedColor] = "AllocNamedColor",
        [X_AllocColorCells] = "AllocColorCells",
        [X_AllocColorPlanes] = "AllocColorPlanes",
        [X_FreeColors] = "FreeColors",
        [X_StoreColors] = "StoreColors",
        [X_StoreNamedColor] = "StoreNamedColor",
        [X_QueryColors] = "QueryColors",
        [X_LookupColor] = "LookupColor",
        [X_CreateCursor] = "CreateCursor",
        [X_CreateGlyphCursor] = "CreateGlyphCursor",
        [X_FreeCursor] = "FreeCursor",
        [X_RecolorCursor] = "RecolorCursor",
        [X_QueryBestSize] = "QueryBestSize",
        [X_QueryExtension] = "QueryExtension",
        [X_ListExtensions] = "ListExtensions",
        [X_ChangeKeyboardMapping] = "ChangeKeyboardMapping",
        [X_GetKeyboardMapping] = "GetKeyboardMapping",
        [X_ChangeKeyboardControl] = "ChangeKeyboardControl",
        [X_GetKeyboardControl] = "GetKeyboardControl",
        [X_Bell] = "Bell",
        [X_ChangePointerControl] = "ChangePointerControl",
        [X_GetPointerControl] = "GetPointerControl",
        [X_SetScreenSaver] = "SetScreenSaver",
        [X_GetScreenSaver] = "GetScreenSaver",
        [X_ChangeHosts] = "ChangeHosts",
        [X_ListHosts] = "ListHosts",
        [X_SetAccessControl] = "SetAccessControl",
        [X_SetCloseDownMode] = "SetCloseDownMode",
        [X_KillClient] = "KillClient",
        [X_RotateProperties] = "RotateProperties",
        [X_ForceScreenSaver] = "ForceScreenSaver",
        [X_SetPointerMapping] = "SetPointerMapping",
        [X_GetPointerMapping] = "GetPointerMapping",
        [X_SetModifierMapping] = "SetModifierMapping",
        [X_GetModifierMapping] = "GetModifierMapping",
        [X_NoOperation] = "NoOperation"
    };
    /* bounds check */          /* & over && for better inlining */
    major_code *= (major_code > 0) & (major_code < 128);
    return (char *const )errs[major_code];
}

char *
XCBGetErrorMinorCodeText(
        uint16_t minor_code
        )
{   

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);


    _XCB_MANUAL_DEBUG0("Not implemented.");

    return NULL;
}

char *
XCBGetErrorDisplayText(
        uint8_t display_error
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    const char *const errs[7] = 
    {
        [0] = NULL,
        [XCB_CONN_ERROR] = "DisplayError",
        [XCB_CONN_CLOSED_EXT_NOTSUPPORTED] = "ExtensionNotSupported",
        [XCB_CONN_CLOSED_MEM_INSUFFICIENT] = "OutOfMemory",
        [XCB_CONN_CLOSED_REQ_LEN_EXCEED] = "TooManyRequests",
        [XCB_CONN_CLOSED_PARSE_ERR] = "InvalidDisplay",
        [XCB_CONN_CLOSED_INVALID_SCREEN] = "InvalidScreen"
    };
    /* bounds check */              /* & over && for better inlining */
    display_error *= (display_error > 0) & (display_error < 7);
    return (char *const)errs[display_error];
}

char *
XCBGetFullErrorText(
        uint8_t error_code
)
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    static const char *errs[18] =
    {
        [0] = NULL,
        [XCBBadRequest] = "BadRequest (invalid request code or no such operation)",
        [XCBBadValue] = "BadValue (integer parameter out of range for operation)",
        [XCBBadWindow] = "BadWindow (invalid Window parameter)",
        [XCBBadPixmap] = "BadPixmap (invalid Pixmap parameter)",
        [XCBBadAtom] = "BadAtom (invalid Atom parameter)",
        [XCBBadCursor] = "BadCursor (invalid Cursor parameter)",
        [XCBBadFont] = "BadFont (invalid Font parameter)" ,
        [XCBBadMatch] = "BadMatch (invalid parameter attributes)",
        [XCBBadDrawable] = "BadDrawable (invalid Pixmap or Window parameter)",
        [XCBBadAccess] = "BadAccess (attempt to access private resource denied)",
        [XCBBadAlloc] = "BadAlloc (insufficient resources for operation)",
        [XCBBadColor] = "BadColor (invalid Colormap parameter)",
        [XCBBadGC] = "BadGC (invalid GC parameter)",
        [XCBBadIDChoice] = "BadIDChoice (invalid resource ID chosen for this connection)",
        [XCBBadName] = "BadName (named color or font does not exist)",
        [XCBBadLength] = "BadLength (poly request too large or internal Xlib length error)",
        [XCBBadImplementation] = "BadImplementation (server does not implement operation)",
    };
    /* bounds check */          /* & over && for better inlining */
    error_code *= (error_code > 0) & (error_code < 18);
    return (char *const )errs[error_code];   
}

/* events */



XCBCookie
XCBAllowEvents(
        XCBDisplay *display, 
        u8 mode, 
        XCBTimestamp tim
        )
{
    XCBCookie ret = xcb_allow_events(display, mode, tim);

    _xcb_push_func(ret);


    return ret;
}

XCBCookie
XCBSendEvent(
        XCBDisplay *display,
        XCBWindow window,
        uint8_t propagate,
        uint32_t event_mask,
        const char *event
        )
{
    XCBCookie ret = xcb_send_event(display, propagate, window, event_mask, event);

    _xcb_push_func(ret);

    return ret;
}

void
XCBSendError(
        XCBDisplay *display,
        XCBGenericError *err
        )
{
    _xcb_handler(display, err);
}

void
XCBSendErrorP(
        XCBDisplay *display,
        XCBGenericError *err
        )
{
    _xcb_err_handler(display, err);
}

int 
XCBNextEvent(
        XCBDisplay *display, 
        XCBGenericEvent **event_return
        ) 
{
    /* waits till next event happens before returning */
    return !((*event_return = xcb_wait_for_event(display)));
}

XCBGenericEvent *
XCBWaitForEvent(
        XCBDisplay *display
        )
{
    /* waits till next event happens before returning */
    return xcb_wait_for_event(display);
}

XCBGenericEvent *
XCBPollForEvent(
        XCBDisplay *display
        )
{
    /* TODO */
    /* If I/O error do something */
   return  xcb_poll_for_event(display);
}

XCBGenericEvent *
XCBPollForQueuedEvent(
        XCBDisplay *display
        )
{
    return xcb_poll_for_queued_event(display);
}

void *
XCBCheckReply(
        XCBDisplay *display, 
        XCBCookie request)
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    XCBGenericError *err = NULL;
    void *reply = NULL;
    xcb_poll_for_reply(display, request.sequence, &reply, &err);

    if(err)
    {    
        _xcb_err_handler(display, err);
        if(reply)
        {   free(reply);
        }
        return NULL;
    }
    return reply;
}

void *
XCBCheckReply64(
        XCBDisplay *display, 
        XCBCookie64 request)
{   

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    XCBGenericError *err = NULL;
    void *reply = NULL;
    xcb_poll_for_reply64(display, request.sequence, &reply, &err);

    if(err)
    {    
        _xcb_err_handler(display, err);
        if(reply)
        {   free(reply);
        }
        return NULL;
    }
    return reply;
}


void *
XCBWaitForReply(
        XCBDisplay *display,
        XCBCookie cookie
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    XCBGenericError *err = NULL;
    void *reply = xcb_wait_for_reply(display, cookie.sequence, &err);
    if(err)
    {   
        _xcb_err_handler(display, err);
        if(reply)
        {   free(reply);
        }
        return NULL;
    }
    return reply;
}

void *
XCBWaitForReply64(
        XCBDisplay *display,
        XCBCookie64 cookie
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    XCBGenericError *err = NULL;
    void *reply = xcb_wait_for_reply64(display, cookie.sequence, &err);
    if(err)
    {   
        _xcb_err_handler(display, err);
        if(reply)
        {   free(reply);
        }
        return NULL;
    }
    return reply;
}

XCBCookie
XCBGrabKeyboardCookie(
        XCBDisplay *display,
        XCBWindow grab_window,
        uint8_t owner_events,
        uint8_t pointer_mode,
        uint8_t keyboard_mode,
        XCBTimestamp tim
        )
{
    const xcb_grab_keyboard_cookie_t cookie = xcb_grab_keyboard(display, owner_events, grab_window, tim, pointer_mode, keyboard_mode);
    const XCBCookie ret = { .sequence = cookie.sequence };

    _xcb_push_func(ret);

    return ret;
}

XCBGrabKeyboard *
XCBGrabKeyboardReply(
        XCBDisplay *display,
        XCBCookie cookie
        )
{
    XCBGenericError *err = NULL;
    const xcb_grab_keyboard_cookie_t cookie1 = { .sequence = cookie.sequence };
    XCBGrabKeyboard *reply = xcb_grab_keyboard_reply(display, cookie1, &err);

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    if(err)
    {
        _xcb_err_handler(display, err);
        free(reply);
        return NULL;
    }
    return reply;
}

XCBCookie
XCBUngrabKeyboard(
        XCBDisplay *display,
        XCBTimestamp tim
        )
{
    XCBCookie ret = xcb_ungrab_keyboard(display, tim);

    _xcb_push_func(ret);


    return ret;
}

XCBCookie
XCBGrabKey(
        XCBDisplay *display, 
        XCBKeyCode keycode, 
        u16 modifiers, 
        XCBWindow grab_window, 
        u8 owner_events, 
        u8 pointer_mode, 
        u8 keyboard_mode
        )
{
    XCBCookie ret = xcb_grab_key(display, owner_events, grab_window, modifiers, keycode, pointer_mode, keyboard_mode);

    _xcb_push_func(ret);


    return ret;
}

XCBCookie 
XCBUngrabKey(
        XCBDisplay *display, 
        XCBKeyCode key, 
        u16 modifiers, 
        XCBWindow grab_window
        )
{
    XCBCookie ret = xcb_ungrab_key(display, key, grab_window, modifiers);

    _xcb_push_func(ret);


    return ret;
}

XCBCookie
XCBUngrabButton(
        XCBDisplay *display, 
        uint8_t button, 
        uint16_t modifier, 
        XCBWindow window
        )
{
    XCBCookie ret = xcb_ungrab_button(display, button, window, modifier);

    _xcb_push_func(ret);


    return ret;
}

XCBCookie
XCBGrabButton(
        XCBDisplay *display, 
        u8 button, 
        u16 modifiers, 
        XCBWindow grab_window, 
        u8 owner_events, 
        u16 event_mask, 
        u8 pointer_mode,
        u8 keyboard_mode,
        XCBWindow confine_to,
        XCBCursor cursor)
{
    XCBCookie ret = xcb_grab_button(display, owner_events, grab_window, event_mask, pointer_mode, keyboard_mode, confine_to, cursor, button, modifiers);

    _xcb_push_func(ret);


    return ret;
}

XCBCookie
XCBGrabPointerCookie(
        XCBDisplay *display,
        XCBWindow grab_window,
        uint8_t owner_events,
        uint16_t event_mask,
        uint8_t pointer_mode,
        uint8_t keyboard_mode,
        XCBWindow confine_to,
        XCBCursor cursor,
        XCBTimestamp tim
        )
{
    xcb_grab_pointer_cookie_t cookie = xcb_grab_pointer(
            display, owner_events, grab_window, event_mask, pointer_mode, keyboard_mode, confine_to, cursor, tim
            );
    XCBCookie ret = { .sequence = cookie.sequence };

    _xcb_push_func(ret);

    return ret;

}

XCBGrabPointer *
XCBGrabPointerReply(
        XCBDisplay *display,
        XCBCookie cookie
        )
{
    const xcb_grab_pointer_cookie_t cookie1 = { .sequence = cookie.sequence };
    XCBGenericError *err = NULL;
    xcb_grab_pointer_reply_t *reply = xcb_grab_pointer_reply(display, cookie1, &err);

    XCBCookie ret = { .sequence = cookie.sequence + 1};
    _xcb_push_func(ret);

    if(err)
    {   
        _xcb_err_handler(display, err);
        if(reply)
        {   free(reply);
        }
        return NULL;
    }
    return reply;

}

XCBCookie
XCBUngrabPointer(
        XCBDisplay *display, 
        XCBTimestamp tim
        )
{
    const XCBCookie ret = xcb_ungrab_pointer(display, tim);

    _xcb_push_func(ret);

    return ret;
}

int
XCBDisplayKeyCodes(
        XCBDisplay *display, 
        int *min_keycode_return, 
        int *max_keycode_return
        )
{
    const XCBSetup *setup = xcb_get_setup(display);
    *min_keycode_return = setup->min_keycode;
    *max_keycode_return = setup->max_keycode;

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    /* xcb source: "length, invalid value" so if this is ever 0 we "failed" */
    return !!setup->length;
}

int 
XCBDisplayKeycodes(
        XCBDisplay *display, 
        int *min_keycode_return, 
        int *max_keycode_return
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    const XCBSetup *setup = xcb_get_setup(display);
    *min_keycode_return = setup->min_keycode;
    *max_keycode_return = setup->max_keycode;
    /* xcb source: "length, invalid value" so if this is ever 0 we "failed" */
    return !!setup->length;
}

XCBKeyCode *
XCBGetKeyCodes(
        XCBDisplay *display, 
        XCBKeysym keysym
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    xcb_key_symbols_t *keysyms;
	xcb_keycode_t *keycode;

	if (!(keysyms = xcb_key_symbols_alloc(display)))
    {   return NULL;
    }

	keycode = xcb_key_symbols_get_keycode(keysyms, keysym);
	xcb_key_symbols_free(keysyms);

	return keycode;
}

XCBKeycode *
XCBGetKeycodes(
        XCBDisplay *display, 
        XCBKeysym keysym
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return XCBGetKeyCodes(display, keysym);
}

XCBKeyCode *
XCBKeySymbolsGetKeyCode(
        XCBKeySymbols *symbols, 
        XCBKeysym keysym)
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return xcb_key_symbols_get_keycode(symbols, keysym);
}

XCBKeycode *
XCBKeySymbolsGetKeycode(
        XCBKeySymbols *symbols,
        XCBKeysym keysym
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return xcb_key_symbols_get_keycode(symbols, keysym);
}

XCBKeysym
XCBKeySymbolsGetKeySym(
        XCBKeySymbols *symbols,
        XCBKeyCode keycode,
        uint8_t column
        )
{   

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return xcb_key_symbols_get_keysym(symbols, keycode, column);
}

XCBKeySymbols *
XCBKeySymbolsAlloc(
        XCBDisplay *display
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    return xcb_key_symbols_alloc(display);
}

void
XCBKeySymbolsFree(
        XCBKeySymbols *keysyms
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    xcb_key_symbols_free(keysyms);
}

uint8_t
XCBRefreshKeyboardMapping(
        XCBKeySymbols *syms, 
        XCBMappingNotifyEvent *event)
{
    return xcb_refresh_keyboard_mapping(syms, event);
}

XCBCookie
XCBGetKeyboardMappingCookie(
        XCBDisplay *display, 
        XCBKeyCode first_keycode, 
        u8 count
        )
{
    const xcb_get_keyboard_mapping_cookie_t cookie = xcb_get_keyboard_mapping(display, first_keycode, count);
    XCBCookie ret = { .sequence = cookie.sequence };

    _xcb_push_func(ret);

    return ret;
}

XCBKeyboardMapping *
XCBGetKeyboardMappingReply(
        XCBDisplay *display, 
        XCBCookie cookie
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);


    XCBGenericError *err = NULL;
    const xcb_get_keyboard_mapping_cookie_t cookie1 = { .sequence = cookie.sequence };
    XCBKeyboardMapping *reply = xcb_get_keyboard_mapping_reply(display, cookie1, &err);
    if(err)
    {   
        _xcb_err_handler(display, err);
        if(reply)
        {   free(reply);
        }
        return NULL;
    }
    return reply;
}

XCBCookie
XCBQueryTreeCookie(
        XCBDisplay *display,
        XCBWindow window
        )
{
    const xcb_query_tree_cookie_t cookie = xcb_query_tree(display, window);
    XCBCookie ret = { .sequence = cookie.sequence };

    _xcb_push_func(ret);

    return ret;
}

XCBQueryTree *
XCBQueryTreeReply(
        XCBDisplay *display, 
        XCBCookie cookie
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    const xcb_query_tree_cookie_t cookie1 = { .sequence = cookie.sequence };
    XCBGenericError *err = NULL;
    xcb_query_tree_reply_t *reply = xcb_query_tree_reply(display, cookie1, &err);
    if(err)
    {   
        _xcb_err_handler(display, err);
        if(reply)
        {   free(reply);
        }
        return NULL;
    }
    return reply;
}

XCBWindow *
XCBQueryTreeChildren(
        const XCBQueryTree *tree
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    /* Why does this exist? its simply a offset of (tree + 1) */
    return xcb_query_tree_children(tree);
}


XCBCookie
XCBQueryPointerCookie(
        XCBDisplay *display, 
        XCBWindow window
        )
{
    const xcb_query_pointer_cookie_t cookie = xcb_query_pointer(display, window);
    XCBCookie ret = { .sequence = cookie.sequence };

    _xcb_push_func(ret);

    return ret;
}

XCBQueryPointer *
XCBQueryPointerReply(
        XCBDisplay *display, 
        XCBCookie cookie
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    XCBGenericError *err = NULL;
    const xcb_query_pointer_cookie_t cookie1 = { .sequence = cookie.sequence };
    XCBQueryPointer *reply = xcb_query_pointer_reply(display, cookie1, &err);
    if(err)
    {   
        _xcb_err_handler(display, err);
        if(reply)
        {   free(reply);
        }
        return NULL;
    }
    return reply;
}


XCBCookie
XCBMapWindow(
        XCBDisplay *display, 
        XCBWindow window
        )
{
    XCBCookie ret = xcb_map_window(display, window);

    _xcb_push_func(ret);

    return ret;
}


XCBCookie
XCBUnmapWindow(
        XCBDisplay *display,
        XCBWindow window
        )
{
    XCBCookie ret = xcb_unmap_window(display, window);

    _xcb_push_func(ret);

    return ret;
}


XCBCookie
XCBDestroyWindow(
        XCBDisplay *display,
        XCBWindow window
        )
{
    XCBCookie ret = xcb_destroy_window(display, window);

    _xcb_push_func(ret);

    return ret;
}

XCBWindow 
XCBCreateWindow(
        XCBDisplay *display, 
        XCBWindow parent, 
        int32_t x, 
        int32_t y, 
        uint32_t width, 
        uint32_t height, 
        uint32_t border_width, 
        uint8_t depth, 
        uint16_t class, 
        XCBVisual visual, 
        u32 valuemask, 
        const XCBCreateWindowValueList *value_list
        )
{
    const XCBWindow id = xcb_generate_id(display);

    /* not actually used but just for standards */
    XCBCookie ret = xcb_create_window_aux(display, depth, id, parent, x, y, width, height, border_width, 
    class, visual, valuemask, value_list);
    

    _xcb_push_func(ret);


    (void)ret;
    return id;
}

XCBWindow
XCBCreateSimpleWindow(
        XCBDisplay *display,
        XCBWindow parent,
        int32_t x,
        int32_t y,
        uint32_t width,
        uint32_t height,
        uint32_t border_width,
        uint32_t border_color,
        uint32_t background_color
        )
{
    const XCBWindow id = xcb_generate_id(display);
    const u8 depth = XCB_COPY_FROM_PARENT;
    const XCBVisual visual = XCBGetScreen(display)->root_visual;
    const u8  class = XCB_WINDOW_CLASS_INPUT_OUTPUT;
    const u32 mask = XCB_CW_BACK_PIXEL | XCB_CW_BORDER_PIXEL;
    const u32 color[2] = { background_color, border_color };

    /* not actually used but just for standards */
    XCBCookie ret = xcb_create_window(display, depth, id, parent, x, y, width, height, border_width, class, visual, mask, &color);

    _xcb_push_func(ret);

    (void)ret;
    return id;
}


XCBGC
XCBCreateGC(
        XCBDisplay *display, 
        XCBDrawable drawable, 
        u32 valuemask, 
        XCBCreateGCValueList *valuelist
        )
{
    const XCBGC id = xcb_generate_id(display);

    /* not actually used but just for standards */
    XCBCookie ret = xcb_create_gc_aux(display, id, drawable, valuemask, valuelist);

    _xcb_push_func(ret);


    (void)ret;
    return id;
}

XCBCookie
XCBFreeGC(
        XCBDisplay *display,
        XCBGC gc
        )
{
    XCBCookie ret = xcb_free_gc(display, gc);


    _xcb_push_func(ret);

    return ret;
}
int
XCBSetLineAttributes(
        XCBDisplay *display, 
        XCBGC gc, 
        u32 linewidth, 
        u32 linestyle, 
        u32 capstyle, 
        u32 joinstyle
        )
{
    u32 gcvalist[4];

    u8 i = 0; /* This is the index; u8 just to save some bytes */
    u32 mask = 0;
    mask |= XCB_GC_LINE_WIDTH;
    gcvalist[i++] = linewidth;

    mask |= XCB_GC_LINE_STYLE;
    gcvalist[i++] = linestyle;

    mask |= XCB_GC_CAP_STYLE;
    gcvalist[i++] = capstyle;

    mask |= XCB_GC_JOIN_STYLE; 
    gcvalist[i++] = joinstyle;


    XCBCookie ret = xcb_change_gc(display, gc, mask, gcvalist);

    _xcb_push_func(ret);


    /* This returns a cookie but changing the gc isnt doesnt really require a reply as you are directly manupulating your own gc
     */
    /* X11 src/SetLStyle.c always returns 1 (for some reason) */
    return !!ret.sequence;
}

XCBCookie
XCBChangeProperty(
        XCBDisplay *display, 
        XCBWindow window, 
        XCBAtom property, 
        XCBAtom type, 
        u8 format, 
        u8 mode, 
        const void *data, 
        u32 nelements
        )
{
    XCBCookie ret = xcb_change_property(display, mode, window, property, type, format, nelements, data);

    _xcb_push_func(ret);


    return ret;
}

XCBCookie
XCBDeleteProperty(
        XCBDisplay *display, 
        XCBWindow window, 
        XCBAtom property
        )
{
    XCBCookie ret = xcb_delete_property(display, window, property);

    _xcb_push_func(ret);


    return ret;
}

XCBCookie
XCBCirculateSubwindows(
        XCBDisplay *display,
        XCBWindow window,
        uint8_t direction
        )
{
    XCBCookie ret = xcb_circulate_window(display, direction, window);

    _xcb_push_func(ret);


    return ret;
}


XCBCookie
XCBCirculateSubwindowsUp(
        XCBDisplay *display,
        XCBWindow window
        )
{
    XCBCookie ret = XCBCirculateSubwindows(display, window, XCB_CIRCULATE_RAISE_LOWEST);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBCirculateSubwindowsDown(
        XCBDisplay *display,
        XCBWindow window
        )
{
    XCBCookie ret = XCBCirculateSubwindows(display, window, XCB_CIRCULATE_LOWER_HIGHEST);

    _xcb_push_func(ret);

    return ret;
}


XCBCookie
XCBConfigureWindow(
        XCBDisplay *display, 
        XCBWindow window,
        u16 value_mask,
        XCBWindowChanges *changes)
{
    XCBCookie ret = xcb_configure_window_aux(display, window, value_mask, changes);

    _xcb_push_func(ret);


    return ret;
}

XCBCookie
XCBStoreName(
        XCBDisplay *display,
        XCBWindow window,
        const char *window_name
        )
{
    const long int MAX_LEN = UINT32_MAX;
    const uint32_t len = strnlen(window_name, MAX_LEN);
    XCBCookie ret = xcb_change_property(display, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, len, window_name);


    _xcb_push_func(ret);

    return ret;
}

int
XCBSetClassHintFast(
        XCBDisplay *display,
        XCBWindow window,
        uint16_t instance_name_length,
        uint16_t class_name_length,
        XCBClassHint *class_hint
        )
{
    const uint8_t NULL_BYTE_COUNT = 2;

    char mem[USHRT_MAX];
    char *src;
    char *dest;

    uint16_t MAX_LEN = USHRT_MAX - NULL_BYTE_COUNT;
    size_t size;

    XCBCookie ret;

    if(instance_name_length > MAX_LEN)
    {   instance_name_length = MAX_LEN;
    }

    if(class_name_length > MAX_LEN)
    {   class_name_length = MAX_LEN;
    }

    if(class_hint->instance_name && instance_name_length)
    {
        /* shrink next posible cpy size */
        MAX_LEN -= instance_name_length;

        src = class_hint->instance_name;
        dest = mem;
        size = instance_name_length;
        memcpy(mem, src, size);
    }
    mem[instance_name_length] = '\0';

    size = 0;
    if(class_hint->instance_name && class_name_length)
    {
        src = class_hint->class_name;
        dest = mem + instance_name_length + 1;
        /* get smallest */
        size = class_name_length > MAX_LEN ? MAX_LEN : class_name_length;
        memcpy(dest, src, size);
    }

    mem[instance_name_length + size + sizeof('\0')] = '\0';

    ret = xcb_icccm_set_wm_class(display, window, instance_name_length + class_name_length + NULL_BYTE_COUNT, mem);


    _xcb_push_func(ret);
    (void)ret;

    return 0;
}

int
XCBSetClassHint(
        XCBDisplay *display, 
        XCBWindow window, 
        XCBClassHint *class_hint
        )
{
    enum { NULL_BYTE_COUNT = 2 };
    uint32_t length_left = USHRT_MAX - NULL_BYTE_COUNT;
    uint16_t ilen = 0;
    uint16_t clen = 0;

    XCBCookie ret = { .sequence = 0 };

    if(class_hint->instance_name)
    {   
        ilen = strnlen(class_hint->instance_name, length_left);
        length_left -= ilen;
    }

    if(class_hint->class_name)
    {   clen = strnlen(class_hint->class_name, length_left);
    }


    _xcb_push_func(ret);
    (void)ret;

    return 
        XCBSetClassHintFast(
            display, 
            window,
            ilen,
            clen,
            class_hint
            );
}


XCBCookie
XCBChangeGC(
        XCBDisplay *display, 
        XCBGC gc, 
        u32 valuemask, 
        XCBChangeGCValueList *valuelist
        )
{
    XCBCookie ret = xcb_change_gc_aux(display, gc, valuemask, valuelist);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBSetForeground(
        XCBDisplay *display, 
        XCBGC gc, 
        uint32_t colour
        )
{
    XCBChangeGCValueList va = { .foreground = colour };
    const u32 mask = XCB_GC_FOREGROUND;
    XCBCookie ret = XCBChangeGC(display, gc, mask, &va);


    _xcb_push_func(ret);

    return ret;
}


int
XCBDiscardReply(
        XCBDisplay *display, 
        XCBCookie cookie
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    xcb_discard_reply(display, cookie.sequence);
    return cookie.sequence;
}

int
XCBThrowAwayReply(
        XCBDisplay *display, 
        XCBCookie cookie
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    xcb_discard_reply(display, cookie.sequence);
    return cookie.sequence;
}

/* dumb stuff */

void 
XCBPrefetchMaximumRequestLength(
        XCBDisplay *display
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    /**
     * @brief Prefetch the maximum request length without blocking.
     * @param c The connection to the X server.
     *
     * Without blocking, does as much work as possible toward computing
     * the maximum request length accepted by the X server.
     *
     * Invoking this function may cause a call to xcb_big_requests_enable,
     * but will not block waiting for the reply.
     * xcb_get_maximum_request_length will return the prefetched data
     * after possibly blocking while the reply is retrieved.
     *
     * Note that in order for this function to be fully non-blocking, the
     * application must previously have called
     * xcb_prefetch_extension_data(c, &xcb_big_requests_id) and the reply
     * must have already arrived.
     */
    xcb_prefetch_maximum_request_length(display);
}

XCBCookie
XCBGetAtomNameCookie(
        XCBDisplay *display,
        XCBAtom atom
        )
{
    const xcb_get_atom_name_cookie_t cookie = xcb_get_atom_name(display, atom);
    const XCBCookie ret = { .sequence = cookie.sequence };


    _xcb_push_func(ret);

    return ret;
}

XCBAtomName *
XCBGetAtomNameReply(
        XCBDisplay *display,
        XCBCookie cookie
        )
{
    XCBGenericError *err = NULL;
    const xcb_get_atom_name_cookie_t cookie1 = { .sequence = cookie.sequence };
    xcb_get_atom_name_reply_t *reply = xcb_get_atom_name_reply(display, cookie1, &err);

    if(err)
    {
        _xcb_err_handler(display, err);
        if(reply)
        {   free(reply);
        }
        return NULL;
    }
    return reply;
}


XCBCookie64
XCBWiden(
        XCBDisplay *display, 
        XCBCookie cookie
        )
{
    /* this might break in the future so we dont want to typedef XCBDisplay */
    struct spoofIn
    {
        pthread_cond_t event_cond;
        int reading;
        char queue[4096];
        int queue_len;

        uint64_t request_expected;
        uint64_t request_read;
        uint64_t request_completed;
        uint64_t total_read;
        struct reply_list *current_reply;
        struct reply_list **current_reply_tail;

        void *replies;
        struct event_list *events;
        struct event_list **events_tail;
        struct reader_list *readers;
        struct special_list *special_waiters;

        struct pending_reply *pending_replies;
        struct pending_reply **pending_replies_tail;
    #if HAVE_SENDMSG
        _xcb_fd in_fd;
    #endif
    
        struct xcb_special_event *special_events;
    };
    enum lazy_reply_tag
    {
        LAZY_NONE = 0,
        LAZY_COOKIE,
        LAZY_FORCED
    };
    struct spoofOut {
        pthread_cond_t cond;
        int writing;

        pthread_cond_t socket_cond;
        void (*return_socket)(void *closure);
        void *socket_closure;
        int socket_moving;

        char queue[16384];
        int queue_len;

        uint64_t request;
        uint64_t request_written;
        uint64_t request_expected_written;
        uint64_t total_written;

        pthread_mutex_t reqlenlock;
        enum lazy_reply_tag maximum_request_length_tag;
        union {
            unsigned int cookie;
            uint32_t value;
        } maximum_request_length;
        #if HAVE_SENDMSG
        _xcb_fd out_fd;
        #endif
    };
    struct spoofDisplay
    {
        int has_error;
        xcb_setup_t *setup;
        int fd;
        pthread_mutex_t iolock;
        struct spoofIn in;
        struct spoofOut out;
        uint64_t infinity[1024];
    };
    struct spoofDisplay *disp = (struct spoofDisplay *)display;
    u64 widen = (disp->out.request & UINT64_C(0xffffffff00000000)) | cookie.sequence;
    if(widen > disp->out.request)
    {   widen -= UINT64_C(1) << 32;
    }
    return (XCBCookie64){ .sequence = widen };
}

XCBCookie64 
XCBWidenCookie(
        XCBDisplay *display, 
        XCBCookie cookie
        )
{
    return XCBWiden(display, cookie);
}

XCBCookie
XCBKillClient(
        XCBDisplay *display, 
        XCBWindow win
        )
{
    XCBCookie ret = xcb_kill_client(display, win);

    _xcb_push_func(ret);

    return ret;
}

/* ICCCM */

XCBCookie
XCBGetWMProtocolsCookie(
        XCBDisplay *display, 
        XCBWindow window, 
        XCBAtom WM_PROTOCOLS_ATOM_ID)
{
    const xcb_get_property_cookie_t cookie = xcb_icccm_get_wm_protocols(display, window, WM_PROTOCOLS_ATOM_ID);
    XCBCookie ret = { .sequence = cookie.sequence };

    _xcb_push_func(ret);

    return ret;
}

int
XCBGetWMProtocolsReply(
        XCBDisplay *display, 
        XCBCookie cookie,
        XCBWMProtocols *protocol_return
        )
{  

    XCBCookie ck = { .sequence = 0 };
    _xcb_push_func(ck);

    XCBGenericError *err = NULL;
    const xcb_get_property_cookie_t cookie1 = { .sequence = cookie.sequence };
    const int ret = xcb_icccm_get_wm_protocols_reply(display, cookie1, protocol_return, &err);
    if(err)
    {   
        _xcb_err_handler(display, err);
        return 0;
    }
    return ret;
}

void
XCBWipeGetWMProtocols(
        XCBWMProtocols *protocols
        )
{

    if(!protocols)
    {   
#ifdef DBG
        _XCB_MANUAL_DEBUG0("No WMprotocols to wipe? this shouldnt be possible.");
        XCBBreakPoint();
#endif
    }


    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    xcb_icccm_get_wm_protocols_reply_wipe(protocols);
    protocols->_reply = NULL;
}

XCBCookie
XCBGetWMHintsCookie(
        XCBDisplay *display,
        XCBWindow win
        )
{ 
    xcb_get_property_cookie_t cookie = xcb_icccm_get_wm_hints(display, win);
    XCBCookie ret = { .sequence = cookie.sequence };

    _xcb_push_func(ret);

    return ret;
}

XCBWMHints *
XCBGetWMHintsReply(
        XCBDisplay *display,
        XCBCookie cookie
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    XCBGenericError *err = NULL;

    XCBWindowProperty *reply = XCBGetPropertyReply(display, cookie);

    if(reply)
    {
        const uint32_t VALID_FORMAT = 32;
        const uint32_t DATA_FIELD_SIZE = 4;

        XCBWMHints *data = xcb_get_property_value(reply);
        int length = xcb_get_property_value_length(reply);
        unsigned int num_elem = abs(length) / DATA_FIELD_SIZE;

        u8 bad_format = reply->format != VALID_FORMAT;
        u8 bad_atom = reply->type != XCB_ATOM_WM_HINTS;
        u8 no_data = !data;

        if(bad_format || bad_atom || no_data || num_elem < XCB_ICCCM_NUM_WM_HINTS_ELEMENTS - 1)
        {   
            __XCBThrowError(display, cookie, XCBBadImplementation, X_GetProperty, XCB_NONE);
            goto USER_ERROR;
        }

        if(length > sizeof(xcb_size_hints_t))
        {   length = sizeof(xcb_size_hints_t);
        }
        memmove(reply, data, length);

        if(num_elem == XCB_ICCCM_NUM_WM_SIZE_HINTS_ELEMENTS - 1)
        {   
            /* dont ask me what this does but its a suppose to be here */
            ((xcb_icccm_wm_hints_t *)reply)->window_group = 0;
        }
    }

    /* error handling */
    if(err)
    {   
        _xcb_err_handler(display, err);
USER_ERROR:
        if(reply)
        {   free(reply);
        }
        reply = NULL;
    }

    return (XCBWMHints *)reply;
}

XCBCookie
XCBSetWMHintsCookie(
        XCBDisplay *display,
        XCBWindow window,
        XCBWMHints *wmhints
        )
{
    XCBCookie ret = xcb_icccm_set_wm_hints(display, window, wmhints);

    _xcb_push_func(ret);

    return ret;
}

XCBCookie
XCBGetWMNameCookie(
        XCBDisplay *display, 
        XCBWindow win
        )
{
    const xcb_get_property_cookie_t cookie = xcb_icccm_get_wm_name(display, win);
    XCBCookie ret = { .sequence = cookie.sequence };

    _xcb_push_func(ret);

    return ret;
}

uint8_t
XCBGetWMNameReply(
        XCBDisplay *display, 
        XCBCookie cookie, 
        XCBTextProperty *prop_return
        )
{
    XCBGenericError *err = NULL;
    const xcb_get_property_cookie_t cookie1 = { cookie.sequence };
    u8 status = xcb_icccm_get_wm_name_reply(display, cookie1, prop_return, &err);

    if(err)
    {   
        _xcb_err_handler(display, err);
        status = 0;
    }
    return status;
}

XCBCookie
XCBSetWMNormalHints(
        XCBDisplay *display,
        XCBWindow window,
        XCBSizeHints *hints
        )
{
    XCBCookie ret = xcb_icccm_set_wm_normal_hints(display, window, hints);


    _xcb_push_func(ret);


    return ret;
}

XCBCookie
XCBGetWMNormalHintsCookie(
        XCBDisplay *display,
        XCBWindow win
        )
{
    const xcb_get_property_cookie_t cookie = xcb_icccm_get_wm_normal_hints(display, win);
    XCBCookie ret = { .sequence = cookie.sequence };

    _xcb_push_func(ret);

    return ret;
}

uint8_t
XCBGetWMNormalHintsReply(
        XCBDisplay *display,
        XCBCookie cookie,
        XCBSizeHints *hints_return
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    XCBGenericError *err = NULL;
    xcb_get_property_cookie_t cookie1 = { .sequence = cookie.sequence };
    u8 status = xcb_icccm_get_wm_normal_hints_reply(display, cookie1, hints_return, &err);

    if(err)
    {   _xcb_err_handler(display, err);
        status = 0;
    }
    return status;
}

XCBSizeHints *
XCBGetWMNormalHintsReplyNoFill(
        XCBDisplay *display,
        XCBCookie cookie
        )
{
    XCBGenericError *err = NULL;
    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    XCBWindowProperty *reply = XCBGetPropertyReply(display, cookie);

    if(reply)
    {
        const uint32_t VALID_FORMAT = 32;

        void *data = xcb_get_property_value(reply);
        u8 bad_format = reply->format != VALID_FORMAT;
        u8 bad_atom = reply->type != XCB_ATOM_WM_SIZE_HINTS;
        u8 no_data = !data;

        if(bad_format || bad_atom || no_data)
        {   
            __XCBThrowError(display, cookie, XCBBadImplementation, X_GetProperty, XCB_NONE);
            goto USER_ERROR;
        }

        int length;

        /* format is never 0 as we check for bad_format, so no DIV by 0 errs */
        length = xcb_get_property_value_length(reply) / (reply->format / 8);

        if(length > XCB_ICCCM_NUM_WM_SIZE_HINTS_ELEMENTS)
        {   length = XCB_ICCCM_NUM_WM_SIZE_HINTS_ELEMENTS;
        }

        /* sizeof(uint32_t) is not could theoretically not be '4' */
        const uint8_t DATA_FIELD_SIZE = 4;
        const unsigned int bytes_copy = abs(length) * DATA_FIELD_SIZE;

        memmove(reply, data, bytes_copy);

        uint32_t VALID_FLAGS = (XCB_ICCCM_SIZE_HINT_US_POSITION|XCB_ICCCM_SIZE_HINT_US_SIZE|
                                        XCB_ICCCM_SIZE_HINT_P_POSITION|XCB_ICCCM_SIZE_HINT_P_SIZE|
                                        XCB_ICCCM_SIZE_HINT_P_MIN_SIZE|XCB_ICCCM_SIZE_HINT_P_MAX_SIZE|
                                        XCB_ICCCM_SIZE_HINT_P_RESIZE_INC|XCB_ICCCM_SIZE_HINT_P_ASPECT
                                        );

        xcb_size_hints_t *hint = (xcb_size_hints_t *)reply;
        /* NumPropSizeElements =- 18 (ICCCM ver. 1) */
        if(length == XCB_ICCCM_NUM_WM_SIZE_HINTS_ELEMENTS)
        {   VALID_FLAGS |= (XCB_ICCCM_SIZE_HINT_BASE_SIZE | XCB_ICCCM_SIZE_HINT_P_WIN_GRAVITY);
        }
        else
        {
            hint->base_width = 0;
            hint->base_height = 0;
            hint->win_gravity = 0;
        }
        hint->flags &= VALID_FLAGS;
    }

    if(err)
    {   
        _xcb_err_handler(display, err);
USER_ERROR:
        if(reply)
        {   free(reply);
        }
        reply = NULL;
    }
    return (XCBSizeHints *)reply;
}

XCBCookie
XCBGetWMClassCookie(
        XCBDisplay *display, 
        XCBWindow win
        )
{
    const xcb_get_property_cookie_t cookie = xcb_icccm_get_wm_class(display, win);
    XCBCookie ret = { .sequence = cookie.sequence };


    _xcb_push_func(ret);


    return ret;
}

u8
XCBGetWMClassReply(
        XCBDisplay *display,
        XCBCookie cookie,
        XCBWMClass *class_return
        )
{

    XCBCookie ret = { .sequence = 0 };
    _xcb_push_func(ret);

    XCBGenericError *err = NULL;
    xcb_get_property_cookie_t cookie1 = { .sequence = cookie.sequence };
    memset(class_return, 0, sizeof(XCBWMClass));
    u8 status = xcb_icccm_get_wm_class_reply(display, cookie1, class_return, &err);
    
    if(err)
    {
        _xcb_err_handler(display, err);
        XCBWipeGetWMClass(class_return);
        class_return->_reply = NULL;
        status = 0;
    }

    return status;
}

void
XCBWipeGetWMClass(
        XCBWMClass *class
        )
{
#ifdef DBG
    if(!class)
    {   _XCB_MANUAL_DEBUG0("No class to wipe? this shouldnt be possible.");
        XCBBreakPoint();
    }
#endif
    xcb_icccm_get_wm_class_reply_wipe(class);
    class->_reply = NULL;
}
