/**
 * @file xcb_trl.h
 * Code definitions.
 */

/* compiling
 * SUPPORTED: X86, X64
 *
 * xcb is dumb and sometimes doesnt find the required stuff so you just guess or search it up
 * but this shhould cover most if not all of xcb's libraries, atleast the ones used here
 * `pkg-config --cflags --libs xcb xcb-util xcb-aux xcb-xinerama xcb-event xcb-keysyms`
 */


/* Why use this?
 * you shouldnt, this was mostly for self-documentation and for macros.
 * Why make this?
 * xcb snake_case is a dumb change (in my opinion), and should have stayed camelCase or PascalCase.
 * but it likely wasnt to "dissasociate" from Xlib which is fine, however I personally dont like that form of casing.
 * also some changes encourage a certain type of usage which is fine, however when "some things" are poorly documented.
 * It becomes quite hard to understand those design choices.
 * However for the most part the important things are documented.
 * Secondly the design choices really make xcb more like a Async Xlib which is strange.
 * Is this bloat?
 * Yes, yes it is.
 * Why not seperate files?
 * 1. Faster compiling.
 * 2. ctrl+f or '/' for search, no need for grep.
 * 3. too much work.
 * 4. I am Lazy.
 *
 * Main reason to not use xcb
 * 1.) Way harder to use to its asyncrounous nature
 * Really, Xlib's locking of Server is overblown out of proportion when talking about xcb being better.
 * The main problem though with Xlib is that it could be async (which would make xcb less useful) and most of the functions are.
 * HOWEVER some "vital" or imporant ones arent which is the main problem with Xlib.
 * Secondly Xlib uses quite a bit more resources than xcb which is also why xcb use often times "better" for the most part though Xlib is fine,
 * What really should happen is we build a toolkit around xcb and NOT use Xlib in the first place. AND not do what Xlib did and be syncrounous.
 * Which is what we did... 
 * However its a shame that Xlib is seen as "deprecated" by people how dont use the X11 windoing system. Really it be more accurate to be seen as a bandage fix
 * XCB if documented would have already take over Xlib and xlib would without question already been banished to the shadow realm.
 * However XCB still has a way to go without its documentation, though it has improved greatly over the years.
 */


/*
 * Basic XCB Usage.
 *
 *
 * NOTE: This trl is NOT thread safe, instead one must manually lock the display for thread safety.
 * NOTE: These functions only LOCK the display NOT the current thread.
 * XCBLockDisplay();    // Analagous to pthread_mutex_lock
 * XCBUnlockDisplay();  // Analagous to pthread_mutex_unlock
 *
 *
 * xcb has a async way of handling stuff so when you call a function to do something async, unless you poll for reply nothing will happen.
 * This is because the item is buffered to itself meaning that unless you call XCBSync() or XCBFlush() you will never get your event back.
 * This is bad as it makes some things harder to use but its mostly good cause you can make a buffered requests and then just send that 1 big buffer.
 * This saves on time asking for ping backs for every single requests.
 * However do note that non-event type requests such as quering a windows attributes, cookies are sent immediatly to the XServer.
 * The Caveat with these is that you have to wait for the reply back. via the _reply() version of the functions.
 * Secondly with these if you never call for a _reply() of that cookie then you WILL leak memory. as you need to free 90% of the _reply() versions.
 *
 *
 *
 * <<< Event handling >>>
 *
 * XCB uses a rather unique approach to event handling which allows very little memory to be used rather an Xlib's simpler approach.
 *
 * This event handling basically is a just a simple structure with padding and id's;
 * XCBGenericEvent *ev;
 *
 * ev->pad;
 * ev->pad0;
 * ev->sequence;
 * ev->full_sequence;
 * ev->response_type;
 *
 * ev->pad              is where all the extra information is stored and allows for xcb to have little memory as this is where we store and cast other structures.
 * ev->pad0             is aligment padding
 * ev->sequence         is like the cookie id's so its mostly useless
 * ev->full_sequence    see above
 * ev->response_type    This is probably the most imporant part, and tells you what event you want.
 *
 * However you may notice that simply using ev->response_type yields access to very few events this is another part of xcb that allows for better memory control.
 * However because of that it is quite the inconvenient to use, there is a mask however provided by XCB that fixes this, a bit long for no reason too.
 *
 *
 * This is an example on how to use events 
 * <<< Events >>>
 *
 * 
 * #define DEBUG(fmt, ...) fprintf(stderr, "[%s:%d] by %s: " fmt "\n", __FILE__,__LINE__,__func__,__VA_ARGS__)
 * int screen;
 * XCBDisplay *dpy;
 *
 * // Open the display
 * dpy = XCBOpenDisplay(NULL, &screen);
 * // XCBOpenDisplay() returns NULL on failure.
 * if(!dpy)
 * {    printf("%s\n", "Failed to open display.");
 *      exit(EXIT_FAILURE);
 * }
 *
 *
 * 
 *
 * int running = 1;
 * uint8_t cleanev;
 * XCBGenericEvent *ev;
 *                  // XCBNextEvent returns 1, IF an I/O Errors occured, this more often than not this means the XServer died. Or we ran out of memory..
 *                  // Though linux will almost always prevent an out of memory error it could still happen.
 * while(running && !XCBNextEvent(dpy, &ev))
 * {
 *      // if ev->response_type is ever 0 then an error occured
 *      if(ev->response_type == 0)
 *      {
 *          printf("%s\n", "Woops we encountered an error exiting...");
 *          exit(EXIT_FAILURE);
 *      }
 *      // Back in the day you would do ev->response_type & ~0x80 however XCB changed this to a macro which is more typesafe
 *      // But what this does is allow use to use the first few bits as numbers
 *      // Let's just say it lets us use the response_type as enum.
 *      cleanev = XCB_EVENT_RESPONSE_TYPE(ev);
 *      // XCB Does NOT automatically free replies which is actually quite the pain to deal with but allows for more control over an application.
 *      // However you can discard certain replies, (mainly ones that return a cookie) using XCBDiscardReply(), which allows use to not need to free the reply,
 *      // but we dont get it back either, See the section "Self Spoofing"
 *      free(ev);
 *      // Technically you dont need to 'clean' the response_type for this function its just an example.
 *      printf("%s\n", XCBGetEventName(cleanev));
 *  }
 *
 * Cookies;
 *
 * Cookies are simply structures with ids.
 * These cookies are mostly useless and just ids for xcb to use.
 *
 * However you can do 2 unique things with them.
 *
 * Self Spoofing;
 *
 * Events in xcb are quite different compared to Xlib and are much simpler in theory.
 * Due to this we can self Spoof calls to our current event handler.
 * This is sometimes useful when you dont want to receive an event back (for whatever reason).
 * Or when you want to receive events back from yourself which can be useful most of the time.
 *
 * # This would send an event to the XServer that we want to map this window as a maprequest (assuming we sent it).
 * # and have to be handle by us.
 *
 * XCBMapWindow(dpy, cool_window);
 *
 * # <<< Event Generated >>> #
 *
 * However we can "spoof" no reply and generate no event for ourselves.
 *
 * XCBCookie cookie;
 * cookie = XCBMapWindow(dpy, cool_window);
 * XCBDiscardReply(dpy, cookie);
 *
 * # <<< No Event Generated >>> #
 *
 * This may not "always" work due to how xcb works so, we must to call XCBDiscardReply() after our generated cookie.
 * 
 * XCBCookie cookie;
 * cookie = XCBMapWindow(dpy, cool_window);
 * // Sync the Display
 * XCBSync(dpy) # or XCBFlush(dpy);
 *
 * # This does nothing because we flushed the buffer to the display using XCBSync()
 * XCBDiscardReply(dpy, cookie);
 *
 *
 * # Replies
 *
 * Simply put a reply is a 2 step container that holds what the information entails (info) (the first part).
 * And holds the actual data in the second part.
 *
 * Now both are technically the same thing, ie you only need to the free the "reply" (first part).
 *
 * Lets first start with Usual no data replies, these are replies which are just small enough to fit into 1 structure.
 *
 * // Example 1
 *
 * // Here we have a empty unfilled reply.
 * // Lets get a reply back.
 *
 * XCBGetWindowAttributes *wa = NULL;
 *
 * // Open display.
 * int screen = 0;
 * XCBDisplay *display = XCBOpenDisplay(NULL, &screen);
 * if(!display)
 * {    // display refused to open. May there is no XServer Active?
 *      exit(EXIT_FAILURE);
 * }
 *
 * // This is just a example target you can use any window ID you want.
 * const XCBWindow target = XCBRootWindow(display, screen);
 *
 *
 * XCBCookie cookie = XCBGetWindowAttributesCookie(display, target);
 *
 * // wait for the reply (this blocks the current thread and waits for a server reply back)s
 *
 * wa = XCBGetWindowAttributesReply(display, cookie);
 *
 * // Check if there was no reply, (Did we pass in the wrong data? Is the window already destroyed?)
 * if(!wa)
 * {    
 *      // In this case we simply remove our XServer connection and exit, 
 *      // but often times you assume the window doesnt have attributes, or a bug in your code.
 *      XCBCloseDisplay(display);
 *      exit(EXIT_FAILURE);
 * }
 * // else we got a window attribute back.
 *
 * // as you can see the Window attribute has all the data by itself, which is good as no other data was also allocated after.
 * wa->_class;
 * // Now this part is useless, but is "pad" in other words every reply has these, and is what we use as spare bits for extra data.
 * wa->pad0[0];
 * wa->pad0[1];
 *
 * // free resulting data.
 * free(wa);
 * 
 * // Example 2.
 *
 * // Here we will get the WM_NAME of a window if set.
 *
 * #define NO_BYTE_OFFSET               (0L)
 * #define REQUEST_MAX_NEEDED_ITEMS     (UINT32_MAX)
 * 
 * // See Xlib documentation for what these fields indicate.
 * XCBCookie  wmcookie = XCBGetWindowPropertyCookie(display, target, XCB_ATOM_WM_NAME, NO_BYTE_OFFSET, REQUEST_MAX_NEEDED_ITEMS, False, XCB_ATOM_STRING);
 * 
 * // Here we wait again for the reply to come back from the XServer.
 * XCBWindowProperty *prop = XCBGetWindowPropertyReply(display, wmcokie);
 * 
 * // First we check if the prop exists, in this case probably no because the target is the root window, 
 * // but if we used a actual window then we would get something back.
 *
 * if(!prop)
 * {    // no prop.
 *      XCBCloseDisplay(display);
 *      exit(EXIT_FAILURE);
 * }
 * // But if we did get a prop back then the window probably has the WM_NAME atleast configured, not necessarily set though.
 * int32_t offset = 0;
 * // This gets how big it is in bytes. Relative to the specified format which should be 8 (as this is a string).
 * XCBGetPropertyValueSize(prop, &offset);
 * char *str = XCBGetPropertyValue(prop);
 *
 * // Now technically you could just do nothing here and use this as it is, because this is now YOUR memory.
 * // But that would be somewhat memory inefficient.
 *
 * // Check if has value, or if the offset says 0, because that indicates how long our memory is for the value.
 * if(!str || !offset)
 * {    
 *      // In this case the target window did not have a WM_NAME net.
 *      XCBCloseDisplay(display);
 *      exit(EXIT_FAILURE);
 * }
 * // Print out the string, and hope its null-terminating.
 * printf("%s\n", str);
 *
 * // Now the actually handle the "string".
 * size_t sizeofstring = sizeof(char) * offset;     // strlen() of string.
 * size_t sizeofnullbyte = sizeof(char);            // nullbyte -> '\0'
 * size_t fullsize = sizeofstring + sizeofnullbyte;
 * char *newstring = malloc(fullsize);
 * if(!newstring)
 * {    // We failed to malloc memory, this is not covered by this example.
 *      XCBCloseDisplay(display);
 *      exit(EXIT_FAILURE);
 * }
 * memcpy(newstring, str, offset);
 * // Yes technically you can just do memset with 0, but no this is more clear.
 * memcpy(newstring + offset, "\0", sizeofnullbyte);
 *
 * // Now we have a fully save string that doesnt inclued extra padding.
 *
 * // And we free the prop, this includes the info in XCBGetWindowAttributes *prop,
 * // And the data we got, XCBGetPropertyValue(prop), or "str" in this case.
 *
 * // Here is how it would look like. (Not To Scale).
 * // As you can see the whole thing really is just 1 big 'o block of memory, 
 * // which is why we only need to free(prop), because of being the start of the chunk of memory.
 * // Now dont be silly and free the data, because that is quite hard to debug, I would know (it isnt).
 * // What it does do is leave a dangling pointer, which is equally bad.
 *
 * XCBWindowProperty *prop;            | void *data = XCBGetPropertyValue(prop);
 * 01010100 00110010 00000000 01000110   00011100 11001111 00110010 01010100 
 *
 * free(prop);
 *
 * // And were done with (most) props.
 * XCBCloseDisplay(display);
 * return EXIT_SUCCESS;
 *
 * # Reply64
 * # This really isnt too important as unless your doing some very specific things.
 * XCBDiscardReply64(dpy, cookie);
 *
 * Reply backs;
 *
 * The second thing we can do is ask for a reply back from the XServer.
 * This can be done manually using _XReply(), _XRead(), _XWrite() however xcb abstracts that away.
 * Because of that we can use xcb_cool_function_ and then _reply() so xcb_cool_function_reply(XCBDisplay *display, XCBCookie cookie);
 * Some xcb functions dont have a _reply() version and those are simply handled in the event queue;
 * 
 * Errors;
 *
 * Depending on the type of error a few things can occur.
 *
 * <<< XCBConnection Error >>> ;
 * This type of error can happen out of the blue if you arent careful.
 * To check if this error occured simply acces the display->has_error type;
 * If you want a more clear approach you can simply call XCBHasConnectionError();
 *
 * <<< Reply Errors >>> ;
 * By default Errors are handled in the event queue.
 * However reply backs, (ie any function that calls for a reply from a cookie).
 * Instead uses the XCBErrorHandler(), this can be set by the client using XCBSetErrorHandler().
 * By Default however we call die() on extension errors and so nothing on know errors that can be ignored.
 *
 *
 *
 *
 *
 * # Decorations ;
 *
 * <<< Server Sided decorations >>> ;
 *
 * Some window manager may imply "decorations" and/or titlebar(s) to your windows.
 * You can "ask" most "modern" window managers to disable them using motif hints.
 *
 * <<< Client side decorations >>> ;
 * By default these are always disabled, however one can enable them as a window manager using the enviroment variable GTK_CSD.
 * Simply by setting it to any value it should enable them for gtk windows if they support it.
 * see: https://wiki.archlinux.org/title/GTK#Client-side_decorations
 *
 * TODO: Maybe show docu for Qt5?
 *
 *
 *
 *
 *
 *
 */

#ifndef XCB_PTL_TYPEDEF_H_
#define XCB_PTL_TYPEDEF_H_

#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_event.h>
#include <xcb/xcbext.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xcb_cursor.h>
#include <xcb/xinerama.h>
#include <xcb/xcb_xrm.h>


#include "icccm.h"
#include "xcb_trl_types.h"
#include "xcb_keysym.h"




/* #define XCB_TRL_ENABLE_DEBUG */              /* This allows more specific information about certain errors, things
                                                 * Regarding the information of XCB_TRL.
                                                 * This should NOT be enabled when releasing a non debug build
                                                 * as it uses a considerable amount of memory and binary size.
                                                 * Instead it is recommended to disable this to allow for optimizations.
                                                 * It is also further recommended not not use a error handler as this will print out the info already.
                                                 * Secondly this may cause unintended behaviour due to the use of the _checked version of the functions.
                                                 *
                                                 * One MASSIVE caveat:
                                                 * if you dont use this api you cant really use the debug ability.
                                                 * so if you use xcb_map_window(); you wont ever see it in the api as you should you XCBMapWindow();
                                                 * thats it.
                                                 */



#ifdef XCB_TRL_ENABLE_DEBUG
/* if you want to pause execution right as we hit something and just check whatever is next you are free to do so */
void XCBBreakPoint(void);
#endif





/*  
 * Opens the display returning a XCBDisplay * to the connection
 *
 *
 *
 * display_name:                Pass in as ":X" where X is the number of the display to connect to.              
 * *screen_number_return:       This returns the display screen number.
 *
 * RETURN: NULL on Failure.
 * RETURN: XCBDisplay * on Success.
 */
XCBDisplay *
XCBOpenDisplay(
        const char *display_name, 
        int *screen_number_return);
/* 
 * Opens the display and returns a XCBDisplay* on Success.
 * Unchecked, caller must check for connection error using XCBCheckDisplayError(display);
 *
 *
 * display_name:                Pass in as ":X" where X is the number of the display to connect to.              
 * *screen_number_return:       This returns the display screen number.
 *
 * NOTE: This function will NEVER return NULL.
 * NOTE: To check error use XCBCheckDisplayError() to get error number.
 * 
 * RETURN: XCB Display * ;
 */
XCBDisplay *
XCBOpenConnection(
        const char *display_name,
        int *screen_number_return);
/* 
 * Closes the connection Specified and frees the data associated with connection.
 * No side effects if display is NULL.
 */
void 
XCBCloseDisplay(
        XCBDisplay *display);
/* 
 * display: Specifies the connection to the X server.
 *
 * RETURN: a connection number for the specified display, On a POSIX-conformant system, this is the file descriptor of the connection.
 */
int 
XCBDisplayNumber(
        XCBDisplay *display);
/* 
 * display: Specifies the connection to the X server.
 *
 * RETURN: a connection number for the specified display, On a POSIX-conformant system, this is the file descriptor of the connection.
 */
int 
XCBConnectionNumber(
        XCBDisplay *display);
/*
 * RETURN: a pointer to the indicated screen.
 */
XCBScreen *
XCBScreenOfDisplay(
        XCBDisplay *display, 
        int screen);
/*
 * RETURN: a pointer to the default screen.
 */
XCBScreen *
XCBDefaultScreenOfDisplay(
        XCBDisplay *display, 
        int screen);
/*
 * display: Specifies the connection to the X server.
 *
 * RETURN: the number of available screens.
 */
int 
XCBScreenCount(
        XCBDisplay *display);
/*
 * RETURN: * a pointer to a string that provides some identification of the owner of the X server implementation. 
 * If the data returned by the server is in the Latin Portable Character Encoding, then the string is in the Host Portable Character Encoding. 
 * Otherwise, the contents of the string are implementation dependent.
 *
 * MUST BE FREED BY CALLING free()
 */
char *
XCBServerVendor(
        XCBDisplay *display);
/*  display Specifies the connection to the X server.
 *
 *  RETURN: the major version number (11) of the X protocol associated with the connected display. 
 */
int 
XCBProtocolVersion(
        XCBDisplay *display);
/* display 	Specifies the connection to the X server.
 *
 * RETURN: the minor protocol revision number of the X server.
 */
int 
XCBProtocolRevision(
        XCBDisplay *display);
/* Returns a number related to a vendor's release of the X server.
 *
 * RETURN: int
 */
int 
XCBVendorRelease(
        XCBDisplay *display);
/* Returns the size of a bitmap's scanline unit in bits. 
 * The scanline is calculated in multiples of this value.
 *
 * RETURN: int
 */
int 
XCBBitmapUnit(
        XCBDisplay *display);

/*
 * Within each bitmap unit, the left-most bit in the bitmap as displayed on the screen is either the least-significant or most-significant bit in the unit. 
 * This macro or function can return LSBFirst or MSBFirst. 
 *
 * RETURN: int
 */
int 
XCBBitmapBitOrder(
        XCBDisplay *display);

/*
 * Each scanline must be padded to a multiple of bits returned by this macro or function.
 *
 * RETURN: int
 */
int 
XCBBitmapPad(
        XCBDisplay *display);

/*
 * Both specify the required byte order for images for each scanline unit in XY format (bitmap) or for each pixel value in Z format. 
 * The macro or function can return either LSBFirst or MSBFirst
 * 
 * RETURN: int
 */
int 
XCBImageByteOrder(
        XCBDisplay *display);

/*
 */
int 
XCBRootOfScreen(
        XCBDisplay *display);

/* Gets the screen setup struct AKA screen stuff 
 *
 * RETURN: const XCBSetup *
 */
const XCBSetup *
XCBGetSetup(
        XCBDisplay *display);
/* Returns a pointer to the screen structure for a display.
 * 
 * The structure contains values for: 
 * White/Black Pixels.
 * Current Input Masks.
 * Screen width/height in pixels or millimeters.
 * min/max installed maps.
 * The Root Visual.
 * Backing Store.
 * Save Unders.
 * Root Depth.
 * Allowed Depths Len.
 *
 * RETURN: XCBScreen *
*/
XCBScreen *
XCBGetScreen(
        XCBDisplay *display);

/* Returns the current Root Window for the specified display and screen.
 *
 * RETURN: Root window of current display.
 */
XCBWindow 
XCBRootWindow(
        XCBDisplay *display, 
        int screen);
/* Returns the default root window of the spcified display and screen. 
 *
 * RETURN: The default root window of the current display.
 */
XCBWindow 
XCBDefaultRootWindow(
        XCBDisplay *display, 
        int screen);
/* Returns the width of the specified screen in pixels.
 * RETURN: Display Width.
 */
uint16_t 
XCBDisplayWidth(
        XCBDisplay *display, 
        int screen);
/* Returns the height of the specified screen in pixels.
 * RETURN: Display Height.
 */
uint16_t 
XCBDisplayHeight(
        XCBDisplay *display, 
        int screen);
/* Returns the current display depth.
 *
 * RETURN: uint8_t;
*/
uint8_t 
XCBDisplayDepth(
        XCBDisplay *display, 
        int screen);
/* Returns the default display depth.
 * 
 * RETURN uint8_t;
 */
uint8_t 
XCBDefaultDepth(
        XCBDisplay *display, 
        int screen);
/* See Xlib's documentation of XSelectInput()
 *
 * NOTE: This function IS buffered and must be Flushed before receiving any thing back. (IE your events you want to listen to.)
 *       XCBFlush(); 
 *       XCBSync(); Has different behaviour, prefer XCBFlush()
 *
 * RETURN: Cookie to request.
*/
XCBCookie
XCBSelectInput(
        XCBDisplay *display, 
        XCBWindow window, 
        uint32_t mask);
/*  
 *  revert_to:          XCB_INPUT_FOCUS_NONE                The focus reverts to `XCB_NONE`, so no window will have the input focus.
 *                      XCB_INPUT_FOCUS_POINTER_ROOT        The focus reverts to `XCB_POINTER_ROOT` respectively. When the focus reverts.
 *                                                          FocusIn and FocusOut events are generated, but the last-focus-change time is.
 *                                                          not changed.
 *                      XCB_INPUT_FOCUS_PARENT              The focus reverts to the parent (or closest viewable ancestor) and the new.
 *                                                          revert_to value is `XCB_INPUT_FOCUS_NONE`.
 *                      XCB_INPUT_FOCUS_FOLLOW_KEYBOARD     NOT YET DOCUMENTED. Only relevant for the xinput extension.
 *
 *  
 *
 *  time:               XCBTimestamp                        The time you want, ONLY use this if you know what your doing.
 *                      XCB_CURRENT_TIME                    A u32 value to the specified XCB_TIME
 *                      XCB_TIME_CURRENT_TIME               The current XCB time.
 *
 *
 *  NOTE:   If the specified time is earlier than the current last-focus-change time, the request is ignored .
 *          (to avoid race conditions when running X over the network).
 *  NOTE:   A FocusIn and FocusOut event is generated when focus is changed.
 * 
 *  RETURN: Cookie to request.
 */
XCBCookie
XCBSetInputFocus(
        XCBDisplay *display,
        XCBWindow window,
        uint8_t revert_to,
        XCBTimestamp time 
        );
/*
 * Depending on the valuemask, XCBChangeWindowAttributes() uses the window attributes in the XCBWindowAttributes structure to change the specified window attributes. 
 * Changing the background does not cause the window contents to be changed. 
 * To repaint the window and its background, use XCBClearWindow().
 *  
 * Setting the border or changing the background such that the border tile origin changes causes the border to be repainted. 
 * Changing the background of a root window to None or ParentRelative restores the default background pixmap. 
 * Changing the border of a root window to CopyFromParent restores the default border pixmap. 
 * Changing the win-gravity does not affect the current position of the window. 
 * Changing the backing-store of an obscured window to WhenMapped or Always, or changing the backing-planes, backing-pixel, or save-under of a mapped window may have no immediate effect. 
 * Changing the colormap of a window (that is, defining a new map, not changing the contents of the existing map) generates a ColormapNotify event. 
 * Changing the colormap of a visible window may have no immediate effect on the screen because the map may not be installed (see XInstallColormap()). 
 * Changing the cursor of a root window to None restores the default cursor. 
 * Whenever possible, you are encouraged to share colormaps.
 * Multiple clients can select input on the same window. Their event masks are maintained separately. 
 * When an event is generated, it is reported to all interested clients. 
 * However, only one client at a time can select for SubstructureRedirectMask ResizeRedirectMask and ButtonPressMask If a client attempts to select any of these event masks and some other client has already selected one, a BadAccess error results. 
 * There is only one do-not-propagate-mask for a window, not one per client.
 *
 * XChangeWindowAttributes() can generate BadAccess, BadColor, BadCursor, BadMatch, BadPixmap, BadValue, and BadWindow errors.
 */
XCBCookie
XCBChangeWindowAttributes(
        XCBDisplay *display, 
        XCBWindow window, 
        uint32_t mask, 
        XCBWindowAttributes *window_attributes
        );

XCBCookie
XCBInstallColormap(
        XCBDisplay *display,
        XCBColormap colormap
        );

XCBCookie
XCBUninstallColormap(
        XCBDisplay *display,
        XCBColormap colormap
        );


/* Returns the "Black Pixel" value for the specified screen for applications that implement a monochrome version.
 *
 * NOTE: "Black Pixel" may not always be the color "Black" but rather whatever colour the specified screen classifies as "Black".
 *
 * RETURN: uint32_t
 */
uint32_t 
XCBBlackPixel(
        XCBDisplay *display, 
        int screen
        );

/* Returns the "White Pixel" value for the specified screen for applications that implement a monochrome version.
 *
 * NOTE: "White Pixel" may not always be the color "White" but rather whatever colour the specified screen classifies as "White".
 *
 * RETURN: uint32_t
 */
uint32_t 
XCBWhitePixel(
        XCBDisplay *display, 
        int screen
        );

/* Syncs the current client to the XServer.
 *
 * This is generally a much faster approach than explicitly calling XCBFlush(); due to the lack of need to write to buffer(I/O)
 * However does not substitute in any way XCBFlush(); Use it instead when requiring a output;
 * 
 * Remember Syncing is cheap, Flushing is expensive.
 *
 * NOTE: Having too many request pending eg. not Syncing causes slow requests to the server, this is due to the buffer being full.
 *       Meaning you must sync the buffer with the XServer to send more, (eg. requests_pending > 1000).
 * NOTE: Flushing is mostly fine at requests_pending < 100, however syncing is still prefered when available.
 *       There would be a noticeable delay though when flushing at requests_pending > 200-1000.
 * NOTE: You can tests this on your system by sending alot of requests to the XServer and calling XFlush after x amount of requests.
 * NOTE: It is recommended to call XSync() after every ~1000 or so requests, if quickly sending alot of events.
 *       However if not sending alot of events in x < 1 (seconds) then syncing may not be nessesary.
 */
void 
XCBSync(
        XCBDisplay *display
        );
/* Syncs the current client to the XServer, and discard every event in queue.
 * For each protocol error received by XCB, XCBSyncf() calls the client application's error handling routine
 *
 * NOTE: This function should be used rarely if ever as it ignores any errors possibly generated during operation.
 */
void
XCBSyncf(
        XCBDisplay *display
        );

/* Reparents the window with win being the source Window and parent being the new Parent window 
 *
 * RETURN: Cookie to request.
 */
XCBCookie
XCBReparentWindow(
        XCBDisplay *display,
        XCBWindow win,
        XCBWindow parent,
        int32_t x,
        int32_t y
        );

/* Moves the specified window using the provided x and y coordinates.
 * 
 * RETURN: Cookie to request.
 */
XCBCookie
XCBMoveWindow(
        XCBDisplay *display, 
        XCBWindow window, 
        int32_t x, 
        int32_t y);

/* Moves the specified window using the provided x and y coordinates, and resizes the specified window using the provided width and height.
 *
 * NOTE: This function returns the cookie to the ResizeRequest and not MoveRequest.
 *
 * RETURN: Cookie to request.
 */
XCBCookie
XCBMoveResizeWindow(
        XCBDisplay *display, 
        XCBWindow window, 
        int32_t x, 
        int32_t y, 
        uint32_t width, 
        uint32_t height);

/* Resizes the specified window using the provided width and height dimentions.
 *
 * RETURN: Cookie to request.
 */
XCBCookie
XCBResizeWindow(
        XCBDisplay *display, 
        XCBWindow window, 
        uint32_t width, 
        uint32_t height);

/* Raises the specified window "above" all other windows.
 *
 * RETURN: Cookie to request.
 */
XCBCookie
XCBRaiseWindow(
        XCBDisplay *display, 
        XCBWindow window);

/* Maps the specified window, and raises it "above" all other windows.
 *
 * RETURN: Cookie to request.
 */
XCBCookie
XCBMapRaised(
        XCBDisplay *display, 
        XCBWindow window);

/* Lowers the specified window "below" all other windows.
 *
 * RETURN: Cookie to request.
 */
XCBCookie
XCBLowerWindow(
        XCBDisplay *display, 
        XCBWindow window);

/*
 *
 * RETURN: Cookie to request.
 */
XCBCookie
XCBRaiseWindowIf(
        XCBDisplay *display, 
        XCBWindow window);

/*
 *
 * RETURN: Cookie to request.
 */
XCBCookie
XCBLowerWindowIf(
        XCBDisplay *display, 
        XCBWindow window);
/* The XCBSetWindowBorder() function sets the border of the window to the pixel value you specify. 
 * If you attempt to perform this on an InputOnly window, a XCBBadMatch error results. 
 *
 * Depending if a window has a 32bit 24bit 16bit 8bit 4bit 2bit or 1bit (1 color border).
 * You may set the alpha values if the window is 32bit (ARGB).
 * If not you may set the default RGB values.
 *
 * BLUE     (0, 255) (unsigned char)
 * RED      (0, 255) (unsigned char)
 * GREEN    (0, 255) (unsigned char)
 * ALPHA    (0, 255) (unsigned char)
 *
 * FORMAT: BLUE + (GREEN << 8) + (RED << 16) + (ALPHA << 24).
 *
 * NOTE: You may use XCBARGB union for this and simply pass XCBARGB x.argb to the border_pixel, though this is not standard.
 *
 * RETURN: Cookie to request.
 */
XCBCookie
XCBSetWindowBorder(
        XCBDisplay *display,
        XCBWindow window,
        uint32_t border_pixel
        );
/* Sets the specified window's "Border Width" with the provided border_width. 
 *
 * RETURN: Cookie to request.
 */
XCBCookie
XCBSetWindowBorderWidth(
        XCBDisplay *display, 
        XCBWindow window, 
        uint16_t border_width
        );

/* Sets the specified window to be sibling of the specified other window.
 *
 * RETURN: Cookie to request.
 */
XCBCookie
XCBSetSibling(
        XCBDisplay *display, 
        XCBWindow window, 
        XCBWindow sibling
        );

XCBCookie
XCBAddToSaveSet(
        XCBDisplay *display,
        XCBWindow win
        );
/*
 * change_mode:         XCB_SET_MODE_INSERT     Same as XCBAddToSaveSet().
 *                      XCB_SET_MODE_DELETE     Same as XCBRemoveFromSaveSet().
 */
XCBCookie
XCBChangeSaveSet(
        XCBDisplay *display,
        XCBWindow win,
        uint8_t change_mode
        );

XCBCookie
XCBRemoveFromSaveSet(
        XCBDisplay *display,
        XCBWindow win
        );

/* Attempts to grab the XCBAtom ID number for the specified "name" value.
 *
 * name:                        Specifies the name associated with the atom you want returned.
 * only_if_exists:              Specifies a Boolean value that indicates whether the atom must be created.
 *
 * RETURN: Cookie to request.
 */
XCBCookie
XCBInternAtomCookie(
        XCBDisplay *display, 
        const char *name, 
        uint8_t only_if_exists
        );

/* Returns the XCBAtom from the provided cookie.
 *
 * RETURN: XCBAtom on Success.
 * RETURN: 0 on Failure.
 */
XCBAtom 
XCBInternAtomReply(
        XCBDisplay *display, 
        XCBCookie cookie
        );


/* Attemps to grab the "TransientFor Hint" from the specified window. 
 *
 * RETURN: Cookie to request.
 */
XCBCookie
XCBGetTransientForHintCookie(
        XCBDisplay *display, 
        XCBWindow win
        );

/* Returns the transient window in "trans_return" on Sucesss, else the window queried doesnt have a transient.
 *
 * NOTE: trans_return is filled data and should NOT be freed.
 *
 * RETURN: 1 on Success.
 * RETURN: 0 on Failure.
 */
uint8_t
XCBGetTransientForHintReply(
        XCBDisplay *display,
        XCBCookie cookie,
        XCBWindow *trans_return
        );


/*
 * property:        WM_(...)/_NET_(...)          This is the property you want to get refer to https://specifications.freedesktop.org/wm-spec/latest/
 * long_offset:     X                       Offset of where you want to grab the data from.
 *
 * long_length:     X                       How much data you want to get from the reply in 4 byte increments.
 *
 * _delete          0/false/False           Dont delete the property when you reply back.
 *                  1/true/True             Delete the property when you reply back.
 * 
 * req_type:        XCB_ATOM_(...)          The atom property to want to be formated refer to the wm-spec, "CARDINAL[][2+n]/32" part of it.
 *                                          this case it would be XCB_ATOM_CARDINAL.                        ^^^^^^^^
 *
 * RETURN: Cookie to requets.
 */
XCBCookie
XCBGetPropertyCookie(
        XCBDisplay *display,
        XCBWindow w,
        XCBAtom property,
        uint32_t long_offset,
        uint32_t long_length,
        uint8_t _delete,
        XCBAtom req_type
        );
/*
 *
 * NOTE: reply must be freed by caller.
 *
 * RETURN: XCBWindowProperty * on Success.
 * RETURN: NULL on Failure.
 *
 */
XCBWindowProperty *
XCBGetPropertyReply(
        XCBDisplay *display,
        XCBCookie cookie
        );

/*
 *
 * RETURN: void *
 */
void *
XCBGetPropertyValue(
        XCBWindowProperty *reply);


/*
 * NOTE: Returning 1 only means the request had no specified format size, and is exactly: sizeof(ThePropertyValue) / _size.
 *
 * PROPSIZE = sizeof(ThePropertyValue);
 * FORMAT;
 * FORMAT8 = sizeof(int8_t);    # bytes: 1;
 * FORMAT16 = sizeof(int16_t);  # bytes: 2;
 * FORMAT32 = sizeof(int32_t);  # bytes: 4;
 *
 * FORMULA: (PROPSIZE / FORMAT) / _size;
 *
 * NOTE: Format (if exists) is used from the reply, reply->format;
 *
 * RETURN: 0 on Success.
 * RETURN: 1 on No Format.
 */
uint8_t 
XCBGetPropertyValueLength(
        XCBWindowProperty *reply, 
        size_t _size,
        uint32_t *ret
        );

/*
 * NOTE: Returning 1 only means the request had no specified format size, and is exactly: sizeof(ThePropertyValue);
 *
 * PROPSIZE = sizeof(ThePropertyValue);
 * FORMAT;
 * FORMAT8 = sizeof(int8_t);    # bytes: 1;
 * FORMAT16 = sizeof(int16_t);  # bytes: 2;
 * FORMAT32 = sizeof(int32_t);  # bytes: 4;
 *
 * FORMULA: (PROPSIZE / FORMAT);
 *
 * NOTE: Format (if exists) is used from the reply, reply->format;
 *
 * RETURN: 0 on Success.
 * RETURN: 1 on No Format.
 */
uint8_t
XCBGetPropertyValueSize(
        XCBWindowProperty *reply,
        uint32_t *ret
        );

/*
 */
XCBCookie
XCBGetWindowPropertyCookie(
        XCBDisplay *display,
        XCBWindow w,
        XCBAtom property,
        uint32_t long_offset,
        uint32_t long_length,
        uint8_t _delete,
        XCBAtom req_type
        );
/*
 *
 * NOTE: reply must be freed by caller.
 *
 * RETURN: XCBWindowProperty * on Success.
 * RETURN: NULL on Failure.
 *
 */
XCBWindowProperty *
XCBGetWindowPropertyReply(
        XCBDisplay *display,
        XCBCookie cookie
        );

/*
 */
void *
XCBGetWindowPropertyValue(
        XCBWindowProperty *reply);

/*
 * NOTE: Returning 1 only means the request had no specified format size, and is exactly: sizeof(ThePropertyValue);
 *
 *
 * PROPSIZE = sizeof(ThePropertyValue);
 * FORMAT;
 * FORMAT8 = sizeof(int8_t);    # bytes: 1;
 * FORMAT16 = sizeof(int16_t);  # bytes: 2;
 * FORMAT32 = sizeof(int32_t);  # bytes: 4;
 *
 * FORMULA: PROPSIZE / FORMAT;
 *
 * NOTE: Format (if exists) is used from the reply, reply->format;
 *
 * RETURN: 0 on Success.
 * RETURN: 1 on No Format.
 */
uint8_t
XCBGetWindowPropertyValueSize(
        XCBWindowProperty *reply,
        uint32_t *size_return
        );

/*
 * NOTE: Returning 1 only means the request had no specified format size, and is exactly: sizeof(ThePropertyValue) / _size.
 *
 * PROPSIZE = sizeof(ThePropertyValue);
 * FORMAT;
 * FORMAT8 = sizeof(int8_t);    # bytes: 1;
 * FORMAT16 = sizeof(int16_t);  # bytes: 2;
 * FORMAT32 = sizeof(int32_t);  # bytes: 4;
 *
 * FORMULA: (PROPSIZE / FORMAT) / _size;
 *
 * NOTE: Format (if exists) is used from the reply, reply->format;
 *
 * RETURN: 0 on Success.
 * RETURN: 1 on No Format.
 */
uint8_t
XCBGetWindowPropertyValueLength(
        XCBWindowProperty *reply, 
        size_t _size,
        uint32_t *size_return
        );

/*
 */
XCBCookie
XCBGetWindowAttributesCookie(
        XCBDisplay *display, 
        XCBWindow window);

/*
 *
 * NOTE: reply must be freed by caller.
 *
 * RETURN: NULL on Failure.
 *
 */
XCBGetWindowAttributes *
XCBGetWindowAttributesReply(
        XCBDisplay *display, 
        XCBCookie cookie);

/*
 */
XCBCookie
XCBGetWindowGeometryCookie(
        XCBDisplay *display, 
        XCBWindow window);

/*
 *
 * NOTE: reply must be freed by caller.
 *
 * RETURN: NULL on Failure.
 *
 */
XCBWindowGeometry *
XCBGetWindowGeometryReply(
        XCBDisplay *display, 
        XCBCookie cookie);

/*
 */
XCBCookie
XCBGetGeometryCookie(
        XCBDisplay *display,
        XCBWindow window);
/*
 *
 * NOTE: reply must be freed by caller.
 *
 * RETURN: NULL on Failure.
 *
 */
XCBGeometry *
XCBGetGeometryReply(
        XCBDisplay *display,
        XCBCookie cookie
        );

/*
 */
XCBPixmap 
XCBCreatePixmap(
        XCBDisplay *display, 
        XCBWindow root, 
        uint16_t width, 
        uint16_t height, 
        uint8_t depth);
/*
 */
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
    );

/* 
 * RETURN: Cookie to request.
 */
XCBCookie
XCBFreePixmap(
        XCBDisplay *display, 
        XCBPixmap pixmap
        );
/* Creates a default white/black cursor "font" cursor resembling Xlib's cursor.
 * Coloured Cursors are possible and must be set using XCBCreateGlyphCursor() instead.
 *
 * RETURN: XCBCursor
 */
XCBCursor 
XCBCreateFontCursor(
        XCBDisplay *display, 
        XCBFont shape
        );

/*
 */
XCBCursor
XCBCreateGlyphCursor(
        XCBDisplay *display,
        XCBFont shape,
        XCBColor *foreground,
        XCBColor *background
        );

/*
 */
XCBCookie
XCBDefineCursor(
        XCBDisplay *display, 
        XCBWindow window, XCBCursor id);
/*
 * NOTE: foreground/background flags must be set, "pixel" property is always ignored.
 */
XCBCookie 
XCBRecolorCursor(
        XCBDisplay *display, 
        XCBCursor cursor, 
        XCBColor *foreground, 
        XCBColor *background
        );

/*
 */
XCBCookie
XCBFreeCursor(
        XCBDisplay *display, 
        XCBCursor cursor);

/*
 */
XCBCookie
XCBOpenFont(
        XCBDisplay *display, 
        XCBFont id, const char *name);

/*
 */
XCBCookie
XCBCloseFont(
        XCBDisplay *display,
        XCBFont id
        );

/*
 */
XCBCookie
XCBQueryTreeCookie(
        XCBDisplay *display,
        XCBWindow window
        );

/*
 * 
 * NOTE: reply must be freed by caller.
 *
 * RETURN: XCBQueryTree * on Success.
 * RETURN: NULL on Failure.
 */
XCBQueryTree *
XCBQueryTreeReply(
        XCBDisplay *display,
        XCBCookie cookie
        );

/* "Query" the trees children, really this is just a offset jmp.
 * (tree + 1);
 *
 *
 * NOTE: This call IS async.
 *
 * RETURN: XCBWindow * On Success.
 * RETURN: NULL on Failure.
 */
XCBWindow *
XCBQueryTreeChildren(
        const XCBQueryTree *tree);

/*
 */
XCBCookie
XCBQueryPointerCookie(
        XCBDisplay *display, 
        XCBWindow window);

/*
 *
 * NOTE: reply must be freed by caller.
 *
 * RETURN: 0 on Failure.
 *
 */
XCBQueryPointer *
XCBQueryPointerReply(
        XCBDisplay *display, 
        XCBCookie cookie);
/* text props */
XCBCookie
XCBGetTextPropertyCookie(
        XCBDisplay *display, 
        XCBWindow window, 
        XCBAtom property);
/* 
 * resulting data ON Success MUST be freed using XCBFreeTextProperty()
 * RETURN: 
 * 1 -> Success.
 * 0 -> Failure.
 */
int 
XCBGetTextPropertyReply(
        XCBDisplay *display, 
        XCBCookie cookie, 
        XCBTextProperty *reply_return);

/*
 * RETURN: 1 on Success.
 */
int
XCBFreeTextProperty(
        XCBTextProperty *prop);

/* 
 * Flushes buffered output to XServer.
 * Blocks Until buffer is fully flushed.
 *
 * NOTE: >> This is how you send requests to the server after doing buffering requests          <<.
 *       >> AKA you wont get responses back from non XCBGetCookie/XCBGetReply type functions    <<.
 *       >> Remember to Flush! Or Sync (slower)                                                 <<.
 *       
 * RETURN: 1 on Success.
 * RETURN: 0 on Failure.
 */ 
int 
XCBFlush(
        XCBDisplay *display);

/*
 * Gets the maximum data that a XCBDisplay* can hold in bytes / 4.
 * Ex: 4 bytes size would return 1
 * 
 * RANGE:
 *                    (256kiB)
 * NO_EXTENSION -> (0, 65535)
 * 
 * //COMMENT, Im not sure on this one docu is vague 
 *                    (16GB)
 * BIG_REQUESTS -> (0, 2097152)
 * 
 * RETURN: Max Request length in (bytes / 4)
 */
uint32_t
XCBGetMaximumRequestLength(
        XCBDisplay *display);



/* Check if the display flag has set a connection error display->has_error;
 * 
 * RETURN: XCB_CONN_ERROR,                      because of socket errors, pipe errors or other stream errors.
 * RETURN: XCB_CONN_CLOSED_EXT_NOTSUPPORTED,    when extension not supported.
 * RETURN: XCB_CONN_CLOSED_MEM_INSUFFICIENT,    when memory not available.
 * RETURN: XCB_CONN_CLOSED_REQ_LEN_EXCEED,      exceeding request length that server accepts.
 * RETURN: XCB_CONN_CLOSED_PARSE_ERR,           error during parsing display string.
 * RETURN: XCB_CONN_CLOSED_INVALID_SCREEN,      because the server does not have a screen matching the display.
 */
int 
XCBCheckDisplayError(
        XCBDisplay *display);
/* Check if the display flag has set a connection error display->has_error;
 * 
 * RETURN: XCB_CONN_ERROR,                      because of socket errors, pipe errors or other stream errors.
 * RETURN: XCB_CONN_CLOSED_EXT_NOTSUPPORTED,    when extension not supported.
 * RETURN: XCB_CONN_CLOSED_MEM_INSUFFICIENT,    when memory not available.
 * RETURN: XCB_CONN_CLOSED_REQ_LEN_EXCEED,      exceeding request length that server accepts.
 * RETURN: XCB_CONN_CLOSED_PARSE_ERR,           error during parsing display string.
 * RETURN: XCB_CONN_CLOSED_INVALID_SCREEN,      because the server does not have a screen matching the display.
 */
int
XCBCheckConnectionError(
        XCBDisplay *display);
/* Check if the display flag has set a connection error display->has_error;
 * 
 * RETURN: XCB_CONN_ERROR,                      because of socket errors, pipe errors or other stream errors.
 * RETURN: XCB_CONN_CLOSED_EXT_NOTSUPPORTED,    when extension not supported.
 * RETURN: XCB_CONN_CLOSED_MEM_INSUFFICIENT,    when memory not available.
 * RETURN: XCB_CONN_CLOSED_REQ_LEN_EXCEED,      exceeding request length that server accepts.
 * RETURN: XCB_CONN_CLOSED_PARSE_ERR,           error during parsing display string.
 * RETURN: XCB_CONN_CLOSED_INVALID_SCREEN,      because the server does not have a screen matching the display.
 */
int 
XCBHasConnectionError(
        XCBDisplay *display);
/* Check if the display flag has set a connection error display->has_error;
 * 
 * RETURN: XCB_CONN_ERROR,                      because of socket errors, pipe errors or other stream errors.
 * RETURN: XCB_CONN_CLOSED_EXT_NOTSUPPORTED,    when extension not supported.
 * RETURN: XCB_CONN_CLOSED_MEM_INSUFFICIENT,    when memory not available.
 * RETURN: XCB_CONN_CLOSED_REQ_LEN_EXCEED,      exceeding request length that server accepts.
 * RETURN: XCB_CONN_CLOSED_PARSE_ERR,           error during parsing display string.
 * RETURN: XCB_CONN_CLOSED_INVALID_SCREEN,      because the server does not have a screen matching the display.
 */
int 
XCBHasDisplayError(
        XCBDisplay *display);

/* 
 * Incase of an unset error handler (default) XCB does nothing.
 * One should note that this function simply sets the function to be called when an error occurs in general X11 fashion.
 * Though this api may provide more specific calls when debug mode is used.
 *
 * NOTE: Handler provided should NOT free() the XCBGenericError * provided.
 * NOTE: Handler is global and only 1 handler can be set when interacting with this API.
 *
 * RETURN: 1 Error handler set.
 * RETURN: 0 Error handler unset.
 */
int 
XCBSetErrorHandler(
        void (*error_handler)(XCBDisplay *, XCBGenericError *)
        );


/* NOT IMPLEMENTED FOR THE FOLLOWING REASONS:
 * 1.) Dont know how to implement without some overhead.
 * 2.) Mostly useless, cause at this point your server is dead.
 * 3.) See 1.
 */
void 
XCBSetIOErrorHandler(
        XCBDisplay *display, 
        void *IOHandler);



/* This gets the XCB event name from a list of possible events using the specified reponse_type.
 *
 * RETURN: Name of event.
 */
char *
XCBGetEventName(
        uint8_t response_type
        );

/* This gets the XCB event name from a list of possible events using the specified event.
 *
 * RETURN: Name of event.
 */
char *
XCBGetEventNameFromEvent(
        XCBGenericEvent *event
        );
/* Returns Bad(The error) using a number provided.
 * The number is from the generic structure XCBGenericError.
 * XCBGenericError *err;
 * err->error_code;
 *
 * RETURN: Error text on Success.
 * RETURN: NULL on Failure.
 */
char *
XCBGetErrorCodeText(
        uint8_t error_code);

/* Returns (The error) using a number provided.
 * The number is from the generic structure XCBGenericError.
 * XCBGenericError *err;
 * err->major_code;
 *
 * NOTE: Usage may result in bigger binary sizes.
 *
 * RETURN: Error text on Success.
 * RETURN: NULL on Failure.
 */
char *
XCBGetErrorMajorCodeText(
        uint8_t major_code);

/*  
 * NOTE: This function is currently not supported.
 *
 * Returns (The error) using a number provided by.
 * The number is from the generic structure XCBGenericError.
 * XCBGenericError *err;
 * err->minor_code;
 *
 * RETURN: Error text on Success.
 * RETURN: NULL on Failure.
 */
char *
XCBGetErrorMinorCodeText(
        uint16_t minor_code
        );

/* Returns (The full error) using a number provided.
 * The different vs just XCBGetErrorText is you get the server dying mini description.
 * EX: BadMatch (invalid parameter attributes).
 *
 * The number is from the generic structure XCBGenericError.
 * XCBGenericError *err;
 * err->major_code;
 *
 * NOTE: Usage may result in bigger binary sizes.
 *
 * RETURN: Error text on Success.
 * RETURN: NULL on Failure.
*/
char *
XCBGetFullErrorText(
        uint8_t error_code
);

/*
 * Returns one of the following using the number provided.
 *
 * "DisplayError"                                       Because of socket errors, pipe errors or other stream errors.
 * "ExtensionNotSupported"                              When extension not supported.
 * "OutOfMemory"                                        When memory not available.
 * "TooManyRequests"                                    Exceeding request length that server accepts.
 * "InvalidDisplay"                                     Error during parsing display string.
 * "InvalidScreen"                                      Because the server does not have a screen matching the display.
 *
 * RETURN: Error text on Success.
 * RETURN: NULL on Failure.
 */
char *
XCBGetErrorDisplayText(
        uint8_t display_error
        );






/* events */

/*
 * mode:            XCB_ALLOW_ASYNC_POINTER             
 *                                                      For AsyncPointer, if the pointer is frozen by the client, pointer event.
 *                                                      processing continues normally. If the pointer is frozen twice by the client on.
 *                                                      behalf of two separate grabs, AsyncPointer thaws for both. AsyncPointer has no.
 *                                                      effect if the pointer is not frozen by the client, but the pointer need not be.
 *                                                      grabbed by the client.
 *
 *                  XCB_ALLOW_SYNC_POINTER
 *                                                      For SyncPointer, if the pointer is frozen and actively grabbed by the client.
 *                                                      pointer event processing continues normally until the next ButtonPress or.
 *                                                      ButtonRelease event is reported to the client, at which time the pointer again.
 *                                                      appears to freeze, however, if the reported event causes the pointer grab to be.
 *                                                      released, then the pointer does not freeze. SyncPointer has no effect if the.
 *                                                      pointer is not frozen by the client or if the pointer is not grabbed by the client.
 *
 *                  XCB_ALLOW_REPLAY_POINTER            
 *                                                      For ReplayPointer, if the pointer is actively grabbed by the client and is.
 *                                                      frozen as the result of an event having been sent to the client (either from
 *                                                      the activation of a GrabButton or from a previous AllowEvents with mode
 *                                                      SyncPointer but not from a GrabPointer), then the pointer grab is released and
 *                                                      that event is completely reprocessed, this time ignoring any passive grabs at
 *                                                      or above (towards the root) the grab-window of the grab just released. The
 *                                                      request has no effect if the pointer is not grabbed by the client or if the
 *                                                      pointer is not frozen as the result of an event.
 *
 *                  XCB_ALLOW_ASYNC_KEYBOARD
 *                                                      For AsyncKeyboard, if the keyboard is frozen by the client, keyboard event.
 *                                                      processing continues normally. If the keyboard is frozen twice by the client on.
 *                                                      behalf of two separate grabs, AsyncKeyboard thaws for both. AsyncKeyboard has.
 *                                                      no effect if the keyboard is not frozen by the client, but the keyboard need.
 *                                                      not be grabbed by the client.
 *
 *                  XCB_ALLOW_SYNC_KEYBOARD
 *                                                      For SyncKeyboard, if the keyboard is frozen and actively grabbed by the client.
 *                                                      keyboard event processing continues normally until the next KeyPress or.
 *                                                      KeyRelease event is reported to the client, at which time the keyboard again.
 *                                                      appears to freeze. However, if the reported event causes the keyboard grab to.
 *                                                      be released, then the keyboard does not freeze. SyncKeyboard has no effect if.
 *                                                      the keyboard is not frozen by the client or if the keyboard is not grabbed by client.
 *                  XCB_ALLOW_REPLAY_KEYBOARD
 *                                                      For ReplayKeyboard, if the keyboard is actively grabbed by the client and is.
 *                                                      frozen as the result of an event having been sent to the client (either from.
 *                                                      the activation of a GrabKey or from a previous AllowEvents with mode.
 *                                                      SyncKeyboard but not from a GrabKeyboard), then the keyboard grab is released.
 *                                                      and that event is completely reprocessed, this time ignoring any passive grabs.
 *                                                      at or above (towards the root) the grab-window of the grab just released. The.
 *                                                      request has no effect if the keyboard is not grabbed by the client or if the.
 *                                                      keyboard is not frozen as the result of an event.
 *                  XCB_ALLOW_ASYNC_BOTH
 *                                                      For AsyncBoth, if the pointer and the keyboard are frozen by the client, event.
 *                                                      processing for both devices continues normally. If a device is frozen twice by.
 *                                                      the client on behalf of two separate grabs, AsyncBoth thaws for both. AsyncBoth
 *                                                      has no effect unless both pointer and keyboard are frozen by the client.
 *                  XCB_ALLOW_SYNC_BOTH
 *                                                      For SyncBoth, if both pointer and keyboard are frozen by the client, event.
 *                                                      processing (for both devices) continues normally until the next ButtonPress,
 *                                                      ButtonRelease, KeyPress, or KeyRelease event is reported to the client for a.
 *                                                      grabbed device (button event for the pointer, key event for the keyboard), at.
 *                                                      which time the devices again appear to freeze. However, if the reported event
 *                                                      causes the grab to be released, then the devices do not freeze (but if the.
 *                                                      other device is still grabbed, then a subsequent event for it will still cause.
 *                                                      both devices to freeze). SyncBoth has no effect unless both pointer and.
 *                                                      keyboard are frozen by the client. If the pointer or keyboard is frozen twice.
 *                                                      by the client on behalf of two separate grabs, SyncBoth thaws for both (but a.
 *                                                      subsequent freeze for SyncBoth will only freeze each device once).
 */
XCBCookie
XCBAllowEvents(
        XCBDisplay *display, 
        uint8_t mode, 
        XCBTimestamp timestamp);

/*
 */
XCBCookie
XCBSendEvent(
        XCBDisplay *display,
        XCBWindow window,
        uint8_t propagate,
        uint32_t event_mask,
        const char *event
        );

/* Copies err to xcb err handler set when using this API.
 */
void
XCBSendError(
        XCBDisplay *display,
        XCBGenericError *err
        );
/* This is used by external libraries. 
 * NOTE: Field '*err' must be a pointer to a memory allocated block of memory that is inacessible after callig this function.
 */
void
XCBSendErrorP(
        XCBDisplay *display,
        XCBGenericError *err
        );

/* 
 * Gets and returns the next Event from the XServer.
 * This returns a structure called xcb_generic_event_t.
 * This Function Blocks until a request is received.
 *
 * NOTE: It is not recommended to use this function as some clients may not be poll() or select() compatible.
 *       Instead it is recommended to either a have a custom XCBPollForEvent() queue or to wakeup the thread,
 *       by sending any event to the caller (root, the window...) and flushing the connection.
 *
 * event_return: XCBGenericEvent * on Success.
 * event_return: XCBGenericError * on Error.
 * event_return: NULL on I/O Error.
 *
 * RETURN: 1 On Failure.
 * RETURN: 0 On Success.
 */
int
XCBNextEvent(
        XCBDisplay *display, 
        XCBGenericEvent **event_return);
/* 
 * Gets and returns the next Event from the XServer.
 * This returns a structure called xcb_generic_event_t.
 * This Function Blocks until a request is received.
 * 
 * NOTE: It is not recommended to use this function as some clients may not be poll() or select() compatible.
 *       Instead it is recommended to either a have a custom XCBPollForEvent() queue or to wakeup the thread,
 *       by sending any event to the caller (root, the window...) and flushing the connection.
 *
 * RETURN: XCBGenericEvent * on Success.
 * RETURN: XCBGenericError * on Error.
 * RETURN: NULL on I/O Error.
 */
XCBGenericEvent *
XCBWaitForEvent(
        XCBDisplay *display);

/**
 * @brief Returns the next event or error from the server.
 * @param c The connection to the X server.
 * @return The next event from the server.
 *
 * Returns the next event or error from the server, if one is
 * available, or returns @c NULL otherwise. If no event is available, that
 * might be because an I/O error like connection close occurred while
 * attempting to read the next event, in which case the connection is
 * shut down when this function returns.
 *
 * NOTE: XCBGenericEvent event_type
 */

XCBGenericEvent *
XCBPollForEvent(
        XCBDisplay *display);
/** <Straight from the documentation.>
 * @brief Returns the next event without reading from the connection.
 * @param c The connection to the X server.
 * @return The next already queued event from the server.
 *
 * This is a version of xcb_poll_for_event that only examines the
 * event queue for new events. The function doesn't try to read new
 * events from the connection if no queued events are found.
 *
 * This function is useful for callers that know in advance that all
 * interesting events have already been read from the connection. For
 * example, callers might use xcb_wait_for_reply and be interested
 * only of events that preceded a specific reply.
 */
XCBGenericEvent *
XCBPollForQueuedEvent(
        XCBDisplay *display);
/* Check if a specified cookie request has a reply available from the XServer.
 * 
 * RETURN: 1 On Success.
 * RETURN: 0 On Not Avaible.
 */
int 
XCBPollForReply(
        XCBDisplay *display, 
        XCBCookie request, 
        void **reply_return);
/* Check if a specified cookie request has a reply available from the XServer.
 * This is different from XCBPollForReply() as it assumes the request has already be widened.
 *
 * RETURN: 1 On Success.
 * RETURN: 0 On Not Avaible/Failure.
 */
int 
XCBPollForReply64(
        XCBDisplay *display, 
        XCBCookie request, 
        void **reply);
/* Check if a specified cookie request has a reply available from the XServer.
 * 
 * RETURN: void * On Success.
 * RETURN: NULL On Not Avaible/Failure.
 */
void *
XCBCheckReply(
        XCBDisplay *display, 
        XCBCookie request);
/* Check if a specified cookie request has a reply available from the XServer.
 * This is different from XCBCheckReply() as it assumes the request has already be widened.
 *
 * NOTE: Casting a XCBCookie as a XCBCookie64 is not a safe operation.
 *
 * RETURN: void * On Success.
 * RETURN: NULL On not Avaible/Failure.
 */
void *
XCBCheckReply64(
        XCBDisplay *display, 
        XCBCookie64 request);

/* Waits for the specifed cookie value to be returned as a void *
 * 
 * RETURN: void * On Success.
 * RETURN: NULL On IOError/Failure.
 */
void *
XCBWaitForReply(
        XCBDisplay *display,
        XCBCookie cookie
        );

/* Waits for the specifed cookie value to be returned as a void *
 * This is different from XCBWaitForReply() as it assumes the request has already be widened.
 *
 * NOTE: Casting a XCBCookie as a XCBCookie64 is not a safe operation.
 *
 * RETURN: void * On Success.
 * RETURN: NULL On IOError/Failure.
 */
void *
XCBWaitForReply64(
        XCBDisplay *display,
        XCBCookie64 cookie
        );



/* grabbing/grab */



/*
 *
 * grab_window:     XCBWindow                   Specifies the window on which the pointer should be grabbed.
 * owner_events:    1/true/True                 The grab_window will still get the pointer events
 *                  0/false/False               If 0, events are not reported to the grab_window.
 *
 * pointer_mode:    XCB_GRAB_MODE_SYNC          The state of the keyboard appears to freeze: No further keyboard events are.
 *                                              generated by the server until the grabbing client issues a releasing.
 *                                              `AllowEvents` request or until the keyboard grab is released.
 *                  XCB_GRAB_MODE_ASYNC         Keyboard event processing continues normally.
 *
 * keyboard_mode:   XCB_GRAB_MODE_SYNC          The state of the keyboard appears to freeze: No further keyboard events are.
 *                                              generated by the server until the grabbing client issues a releasing.
 *                                              `AllowEvents` request or until the keyboard grab is released.
 *                  XCB_GRAB_MODE_ASYNC         Keyboard event processing continues normally.
 *
 * tim:             XCBTimestamp                Timestamp to avoid race conditions when running X over the network.
 *                  XCB_CURRENT_TIME            Use the current time if avaible.
 *
 * RETURN: Cookie to request.
 */

XCBCookie
XCBGrabKeyboardCookie(
        XCBDisplay *display,
        XCBWindow grab_window,
        uint8_t owner_events,
        uint8_t pointer_mode,
        uint8_t keyboard_mode,
        XCBTimestamp tim
        );
/*
 * Returns a XCBGrabKeyboard * structure.
 *
 * NOTE: RETURN MUST BE RELEASED BY CALLER USING free().
 *
 * RETURN: XCBGrabKeyboard * on Success.
 * RETURN: NULL on Failure.
 */
XCBGrabKeyboard *
XCBGrabKeyboardReply(
        XCBDisplay *display,
        XCBCookie cookie
        );

/* 
 * Sends a Event to the XServer to ungrab the keyboard.
 * 
 *
 * RETURN: Cookie to request.
 */
XCBCookie
XCBUngrabKeyboard(
        XCBDisplay *display,
        XCBTimestamp tim
        );
/*
 * key:             XCB_GRAB_ANY                Grab all possible key codes.
 *                  XCBKeyCode                  The keycode of the specified key combination.
 *
 * modifiers:       XCB_MOD_MASK_ANY            Release all key combinations regardless of modifier.
 *                  XCB_MOD_MASK_SHIFT          The Shift Key. 
 *                  XCB_MOD_MASK_LOCK           The Capslock Key.
 *                  XCB_MOD_MASK_CONTROL        The Ctrl Key.
 *                  XCB_MOD_MASK_1              The Alt Key.
 *                  XCB_MOD_MASK_2              The Numlock Key.
 *                  XCB_MOD_MASK_3              ISO_LEVEL5_SHIFT              
 *                  XCB_MOD_MASK_4              The Super/Windows Key.
 *                  XCB_MOD_MASK_5              ISO_LEVEL3_SHIFT 
 *
 * grab_window:     XCBWindow                   The window on which the grabbed key combination will be grabbed.
 *
 * owner_events:    0/false/False               The Key Events are not reported to the grab_window.
 *                  1/true/True                 The Key Events are reported to the grab_window.
 * 
 * pointer_mode:    XCB_GRAB_MODE_SYNC          The state of the keyboard appears to freeze: No further keyboard events are.
 *                                              generated by the server until the grabbing client issues a releasing.
 *                                              `AllowEvents` request or until the keyboard grab is released.
 *                  XCB_GRAB_MODE_ASYNC         Keyboard event processing continues normally.
 *
 * keyboard_mode:   XCB_GRAB_MODE_SYNC          The state of the keyboard appears to freeze: No further keyboard events are.
 *                                              generated by the server until the grabbing client issues a releasing.
 *                                              `AllowEvents` request or until the keyboard grab is released.
 *                  XCB_GRAB_MODE_ASYNC         Keyboard event processing continues normally.
 *
 * RETURN: Cookie to request.
 */
XCBCookie
XCBGrabKey(
        XCBDisplay *display, 
        XCBKeyCode key, 
        uint16_t modifiers, 
        XCBWindow grab_window, 
        uint8_t owner_events, 
        uint8_t pointer_mode, 
        uint8_t keyboard_mode);
/*  
 * keycode:         XCB_GRAB_ANY                Release all possible key codes.
 *                  XCBKeyCode                  The keycode of the specified key combination.
 * modifiers:       XCB_MOD_MASK_ANY            Release all key combinations regardless of modifier.
 *                  XCB_MOD_MASK_SHIFT          The Shift Key. 
 *                  XCB_MOD_MASK_LOCK           The Capslock Key.
 *                  XCB_MOD_MASK_CONTROL        The Ctrl Key.
 *                  XCB_MOD_MASK_1              The Alt Key.
 *                  XCB_MOD_MASK_2              The Numlock Key.
 *                  XCB_MOD_MASK_3              ISO_LEVEL5_SHIFT              
 *                  XCB_MOD_MASK_4              The Super/Windows Key.
 *                  XCB_MOD_MASK_5              ISO_LEVEL3_SHIFT 
 *
 * grab_window:     XCBWindow                   The window on which the grabbed key combination will be released.
 *
 * RETURN: Cookie to request.
 */
XCBCookie
XCBUngrabKey(
        XCBDisplay *display,
        XCBKeyCode key,
        uint16_t modifiers,
        XCBWindow grab_window
        );
/* button:      XCB_BUTTON_INDEX_ANY        Any of the following (or none).
 *              XCB_BUTTON_INDEX_1          The left mouse button.
 *              XCB_BUTTON_INDEX_2          The right mouse button.
 *              XCB_BUTTON_INDEX_3          The middle mouse button.
 *              XCB_BUTTON_INDEX_4          The Scroll Wheel. (direction TODO).
 *              XCB_BUTTON_INDEX_5          The Scroll Wheel. (direction TODO).
 *
 * modifier:    XCB_BUTTON_MASK_1           Not enough information
 *              XCB_BUTTON_MASK_2           Not enough information
 *              XCB_BUTTON_MASK_3           Not enough information
 *              XCB_BUTTON_MASK_4           Not enough information.
 *              XCB_BUTTON_MASK_5           Not enough information.
 *              XCB_BUTTON_MASK_ANY         This is guaranteed to be a mask.
 *  
 *  NOTE: ''uint8_t button' IS NOT a mask.
 *  NOTE: 'modifier' is undocumented.
 *
 *  RETURN: Cookie to request.
 */
XCBCookie
XCBUngrabButton(
        XCBDisplay *display, 
        uint8_t button, 
        uint16_t modifier, 
        XCBWindow window
        );
/*  
 * button:          XCB_BUTTON_INDEX_ANY        Any of the following (or none).
 *                  XCB_BUTTON_INDEX_1          The left mouse button.
 *                  XCB_BUTTON_INDEX_2          The right mouse button.
 *                  XCB_BUTTON_INDEX_3          The middle mouse button.
 *                  XCB_BUTTON_INDEX_4          The Scroll Wheel. (direction TODO).
 *                  XCB_BUTTON_INDEX_5          The Scroll Wheel. (direction TODO).
 *
 * modifiers:       XCB_MOD_MASK_SHIFT          The Shift Key. 
 *                  XCB_MOD_MASK_LOCK           The Capslock  Key.
 *                  XCB_MOD_MASK_CONTROL        The Ctrl Key.
 *                  XCB_MOD_MASK_1              The Alt Key.
 *                  XCB_MOD_MASK_2              The Numlock Key.
 *                  XCB_MOD_MASK_3              ISO_LEVEL5_SHIFT              
 *                  XCB_MOD_MASK_4              The Super/Windows Key.
 *                  XCB_MOD_MASK_5              ISO_LEVEL3_SHIFT 
 *                  XCB_MOD_MASK_ANY
 *
 * grab_window:     XCBWindow                   Specifies the window on which the pointer should be grabbed.
 *
 * owner_events:    1/true/True                 Report grabbed events to grab_window.
 *                  0/false/False               Dont report grabbed events to grab_window.
 *
 * event_mask:      XCB_EVENT_MASK_(...)        Specifies which pointer events are reported to the client.
 *
 * pointer_mode:    XCB_GRAB_MODE_SYNC          The state of the keyboard appears to freeze: No further keyboard events are.
 *                                              generated by the server until the grabbing client issues a releasing.
 *                                              `AllowEvents` request or until the keyboard grab is released.
 *                  XCB_GRAB_MODE_ASYNC         Keyboard event processing continues normally.
 *
 * keyboard_mode:   XCB_GRAB_MODE_SYNC          The state of the keyboard appears to freeze: No further keyboard events are.
 *                                              generated by the server until the grabbing client issues a releasing.
 *                                              `AllowEvents` request or until the keyboard grab is released.
 *                  XCB_GRAB_MODE_ASYNC         Keyboard event processing continues normally.
 *
 * confine_to:      XCB_WINDOW_NONE/XCB_NONE    Dont confine to window.
 *                  XCBWindow                   Specifies the window to confine the pointer in (the user will not be able to.
 *                                              move the pointer out of that window).
 * cursor:          XCB_CURSOR_NONE/XCB_NONE    Dont change the cursor/leave it as it was.
 *                  XCBCursor                   Specifies the cursor that should be displayed.
 * RETURN: Cookie to request.
 */
XCBCookie
XCBGrabButton(
        XCBDisplay *display, 
        uint8_t button, 
        uint16_t modifiers, 
        XCBWindow grab_window, 
        uint8_t owner_events, 
        uint16_t event_mask,
        uint8_t pointer_mode,
        uint8_t keyboard_mode,
        XCBWindow confine_to,
        XCBCursor cursor 
        );
/*
 * grab_window:     XCBWindow                   Specifies the window on which the pointer should be grabbed.
 * owner_events:    1/true/True                 The grab_window will still get the pointer events
 *                  0/false/False               If 0, events are not reported to the grab_window.
 *
 * event_mask:      XCB_EVENT_MASK_(...)        Specifies which pointer events are reported to the client.
 *
 * pointer_mode:    XCB_GRAB_MODE_SYNC          The state of the keyboard appears to freeze: No further keyboard events are.
 *                                              generated by the server until the grabbing client issues a releasing.
 *                                              `AllowEvents` request or until the keyboard grab is released.
 *                  XCB_GRAB_MODE_ASYNC         Keyboard event processing continues normally.
 *
 * keyboard_mode:   XCB_GRAB_MODE_SYNC          The state of the keyboard appears to freeze: No further keyboard events are.
 *                                              generated by the server until the grabbing client issues a releasing.
 *                                              `AllowEvents` request or until the keyboard grab is released.
 *                  XCB_GRAB_MODE_ASYNC         Keyboard event processing continues normally.
 *
 * confine_to:      XCB_WINDOW_NONE/XCB_NONE    Dont confine to window.
 *                  XCBWindow                   Specifies the window to confine the pointer in (the user will not be able to.
 *                                              move the pointer out of that window).
 *
 * cursor:          XCB_NONE                    Dont change the cursor.
 *                  XCBCursor                   Specifies the cursor that should be displayed.
 *
 * tim:             XCBTimestamp                Timestamp to avoid race conditions when running X over the network.
 *                  XCB_CURRENT_TIME            Use the current time if avaible.
 *
 *
 *
 * RETURN: Cookie to requets.
 */
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
        );

/*
 *
 * NOTE: RETURN MUST BE RELEASED BY CALLER USING free().
 *
 * RETURN: XCBGrabPointer * on Success.
 * RETURN: NULL on Failure.
 */
XCBGrabPointer *
XCBGrabPointerReply(
        XCBDisplay *display,
        XCBCookie cookie
        );

/* Ungrabs the pointer from the specfied display
 *
 * RETURN: Cookie to request.
 */
XCBCookie
XCBUngrabPointer(
        XCBDisplay *display,
        XCBTimestamp tim
        );

/* Returns min-keycodes and max-keycodes supported by the specified display.
 * The minimum number of KeyCodes returned is never less than 8, and the maximum number of KeyCodes returned is never greater than 255. 
 * Not all KeyCodes in this range are required to have corresponding keys.
 *
 * RETURN: 1 always;
 */
int 
XCBDisplayKeyCodes(
        XCBDisplay *display, 
        int *min_keycode_return, 
        int *max_keycode_return);
/* Returns min-keycodes and max-keycodes supported by the specified display.
 * The minimum number of KeyCodes returned is never less than 8, and the maximum number of KeyCodes returned is never greater than 255. 
 * Not all KeyCodes in this range are required to have corresponding keys.
 *
 * RETURN: 1 always;
 */
int 
XCBDisplayKeycodes(
        XCBDisplay *display, 
        int *min_keycode_return, 
        int *max_keycode_return);


/* <<< Straight From Xlib >>>
 * The XCBRefreshKeyboardMapping() function refreshes the stored modifier and keymap information. 
 * You usually call this function when a XCB_MAPPING_NOTIFY event with a request member of 
 *
 * XCB_MAPPIN_KEYBOARD or XCB_MAPPING_MODIFIER occurs. 
 * The result is to update XCB's knowledge of the keyboard. 
 *
 *
 * RETURN: 1 on Success.
 * RETURN: 0 on Failure.
 */
uint8_t
XCBRefreshKeyboardMapping(
        XCBKeySymbols *syms, 
        XCBMappingNotifyEvent *event);

/*
 */
XCBCookie
XCBGetKeyboardMappingCookie(
        XCBDisplay *display, 
        XCBKeyCode first_keycode, 
        uint8_t count);

/*
 *
 * NOTE: reply must be freed by caller.
 *
 * RETURN: 0 on Failure.
 *
 */
XCBKeyboardMapping *
XCBGetKeyboardMappingReply(
        XCBDisplay *display, 
        XCBCookie cookie);

/* Send a event to the XServer to map the window specified;
 *
 * RETURN: Cookie to request.
 */
XCBCookie
XCBMapWindow(
        XCBDisplay *display, 
        XCBWindow window);

/* Send a event to the XServer to unmap the window specified;
 *
 * RETURN: Cookie to request.
 */
XCBCookie
XCBUnmapWindow(
        XCBDisplay *display,
        XCBWindow window
        );

/*
 */
XCBCookie
XCBDestroyWindow(
        XCBDisplay *display,
        XCBWindow window
        );

/* 
 *  depth:              uint8_t                                 Depth you want your window to be.
 *                      XCB_COPY_FROM_PARENT                    Copy depth from parent.
 *
 *  _class:             XCB_WINDOW_CLASS_COPY_FROM_PARENT 
 *                      XCB_WINDOW_CLASS_INPUT_OUTPUT 
 *                      XCB_WINDOW_CLASS_INPUT_ONLY
 *
 *  visual:             XCBVisual                               Visual you want your window to be.
 *                      XCB_COPY_FROM_PARENT                    Copy visual from parent.
 *  value_mask:         (enum) xcb_cw_t                         Value mask for value_list.
 *                      XCB_CW_BACK_PIXMAP                      (mask) Window Background Pixmap.
 *                      XCB_CW_BACK_PIXEL                       (mask) Window Background Pixel.
 *                      XCB_CW_BORDER_PIXMAP                    (mask) Window Border Pixmap.
 *                      XCB_CW_BORDER_PIXEL                     (mask) Window Border Pixel.
 *                      XCB_CW_BIT_GRAVITY                      (mask) Window (Expose) Gravity.
 *                      XCB_CW_WIN_GRAVITY                      (mask) Window Gravity.
 *                      XCB_CW_BACKING_STORE                    (mask) Window Backing Store.
 *                      XCB_CW_BACKING_PLANES                   (mask) Window Backing Planes.
 *                      XCB_CW_BACKING_PIXEL                    (mask) Window Backing Pixel.
 *                      XCB_CW_OVERRIDE_REDIRECT                (mask) Window Override Redirect.
 *                      XCB_CW_SAVE_UNDER                       (mask) Window Save Under.
 *                      XCB_CW_EVENT_MASK                       (mask) Window Event Mask.
 *                      XCB_CW_DONT_PROPAGATE                   (mask) Window Dont Propogate.
 *                      XCB_CW_COLORMAP                         (mask) Window Colormap.
 *                      XCB_CW_CURSOR                           (mask) Window Cursor.
 */
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
        uint16_t _class, 
        XCBVisual visual, 
        uint32_t valuemask, 
        const XCBCreateWindowValueList *value_list
        );
/*
 *
 * border:          uint32_t        The Border Colour.
 *                  XCBARGB.argb
 *
 * background:      uint32_t        The background Colour.
 *                  XCBARGB.argb
 *
 * BLUE     (0, 255) (unsigned char)
 * RED      (0, 255) (unsigned char)
 * GREEN    (0, 255) (unsigned char)
 * ALPHA    (0, 255) (unsigned char)
 *
 * FORMAT: BLUE + (GREEN << 8) + (RED << 16) + (ALPHA << 24).
 */
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
        );
/* 
 * RETURN: GC id (identification number) 
 */
XCBGC 
XCBCreateGC(
        XCBDisplay *display, 
        XCBDrawable drawable, 
        uint32_t valuemask, 
        XCBCreateGCValueList *valuelist
        );

/*
 * Frees GC.
 */
XCBCookie
XCBFreeGC(
        XCBDisplay *display,
        XCBGC gc
        );
/* 
 * linewidth: measured in pixels and can be greater than or equal to one, a wide line, or the special value zero, a thin line.
 * linestyle: XCB_LINE_STYLE_SOLID          The full path of the line is drawn.
 *
 *            XCB_LINE_STYLE_DOUBLE_DASH    The full path of the line is drawn, 
 *                                          but the even dashes are filled differently than the odd dashes (see fill-style), 
 *                                          with Butt cap-style used where even and odd dashes meet.
 *
 *            XCB_LINE_STYLE_ON_OFF_DASH    Only the even dashes are drawn, 
 *                                          and cap-style applies to all internal ends of the individual dashes (except NotLast is treated as Butt).
 *
 * capstyle: XCB_CAP_STYLE_NOT_LAST         The result is equivalent to Butt, except that for a line-width of zero the final endpoint is not drawn.
 *           XCB_CAP_STYLE_BUTT             The result is square at the endpoint (perpendicular to the slope of the line) with no projection beyond.
 *           XCB_CAP_STYLE_ROUND            The result is a circular arc with its diameter equal to the line-width, centered
 *                                          on the endpoint; it is equivalent to Butt for line-width zero.
 *           XCB_CAP_STYLE_PROJECTING       The result is square at the end, but the path continues beyond the endpoint for
 *                                          a distance equal to half the line-width; it is equivalent to Butt for line-width zero.
 *
 * joinstyle: XCB_JOIN_STYLE_MITER          The outer edges of the two lines extend to meet at an angle. However, if the
 *                                          angle is less than 11 degrees, a Bevel join-style is used instead.
 *            XCB_JOIN_STYLE_ROUND          The result is a circular arc with a diameter equal to the line-width, centered
 *                                          on the joinpoint.
 *            XCB_JOIN_STYLE_BEVEL          The result is Butt endpoint styles, and then the triangular notch is filled.
 *
 *
 * RETURN: 1, always cause why not 
 */
int 
XCBSetLineAttributes(
        XCBDisplay *display, 
        XCBGC gc, 
        uint32_t linewidth, 
        uint32_t linestyle, 
        uint32_t capstyle, 
        uint32_t joinstyle);

/*
 * property:                                    The atom property AKA XInternAtom(dpy, "my_cool_atom", False);
 *
 * type:        XCBAtom                         Not often the case but specific atom require this, see above for type.
 *              XCBAtomType                     The XCB_ATOM_(sometype) of the property. (Xlib XA_(sometype))
 *
 * format:                                      Specifies whether the data should be viewed as a list of 8-bit, 16-bit or.
 *                                              32-bit quantities. Possible values are 8, 16 and 32. This information allows.
 *                                              the X server to correctly perform byte-swap operations as necessary.
 *
 * mode:        XCB_PROP_MODE_REPLACE           Discard the previous property value and store the new data.
 *
 *              XCB_PROP_MODE_PREPEND           Insert the new data before the beginning of existing data. The `format` must.
 *                                              match existing property value. If the property is undefined, it is treated as.
 *                                              defined with the correct type and format with zero-length data.
 *
 *              XCB_PROP_MODE_APPEND            Insert the new data after the beginning of existing data. The `format` must.
 *                                              match existing property value. If the property is undefined, it is treated as.
 *                                              defined with the correct type and format with zero-length data.
 * data:                                        The property data.
 * 
 * nelements:                                   Specifies the number of elements.
 * RETURN: Cookie to request.
*/
XCBCookie
XCBChangeProperty(
        XCBDisplay *display,
        XCBWindow window, 
        XCBAtom property, 
        XCBAtom type, 
        uint8_t format, 
        uint8_t mode, 
        const void *data, 
        uint32_t nelements); /* data_len */
/*
 * RETURN: Cookie to request.
*/
XCBCookie
XCBDeleteProperty(
        XCBDisplay *display, 
        XCBWindow window, 
        XCBAtom property);

/* No Documentation Provided.
 * Prefer XCBRaiseWindow/XCBLowerWindow/XCBConfigureWindow.
 *
 * direction:           0/XCB_CIRCULATE_RAISE_LOWEST          Lowest mapped child (if any) will be raised to the top of the stack.
 *                      1/XCB_CIRCULATE_LOWER_HIGHEST         Highest mapped child (if any) will be lowered to the bottom of the stack.
 * RETURN: Cookie to request.
 */
XCBCookie
XCBCirculateSubwindows(
        XCBDisplay *display,
        XCBWindow window,
        uint8_t direction
        );

/* No Documentation Provided.
 * Prefer XCBRaiseWindow/XCBLowerWindow/XCBConfigureWindow.
 *
 * RETURN: Cookie to request.
 */
XCBCookie
XCBCirculateSubwindowsUp(
        XCBDisplay *display,
        XCBWindow window
        );

/* No Documentation Provided.
 * Prefer XCBRaiseWindow/XCBLowerWindow/XCBConfigureWindow.
 *
 * RETURN: Cookie to request.
 */
XCBCookie
XCBCirculateSubwindowsDown(
        XCBDisplay *display,
        XCBWindow window
        );


/* Mask;
 * value_mask:          XCB_CONFIG_WINDOW_X;            Sets the flag to configure the X axis.
 *                      XCB_CONFIG_WINDOW_Y;            Sets the flag to configure the Y axis.
 *                      XCB_CONFIG_WINDOW_WIDTH;        Sets the flag to configure the Width.
 *                      XCB_CONFIG_WINDOW_HEIGHT;       Sets the flag to configure the Height.
 *                      XCB_CONFIG_WINDOW_BORDER_WIDTH; Sets the flag to configure the Border Width.
 *                      XCB_CONFIG_WINDOW_SIBLING;      Sets the flag to configure the Window Sibling.
 *                      XCB_CONFIG_WINDOW_STACK_MODE;   Sets the flag to configure the Stack Mode.
 * 
 * changes:             XCBWindowChanges *              The structure to the changes.
 * RETURN: Cookie to request.
*/
XCBCookie
XCBConfigureWindow(
        XCBDisplay *display, 
        XCBWindow window,
        uint16_t value_mask,
        XCBWindowChanges *changes);

XCBCookie
XCBStoreName(
        XCBDisplay *display,
        XCBWindow window,
        const char *window_name
        );

/* 
 *
 * NOTE: instance_name_length must be strlen(mystring);
 * NOTE: class_name_length must be strlen(mystring);
 *
 * RETURN: 0 on Success.
 * RETURN: 1 on Failure.
 */
int
XCBSetClassHintFast(
        XCBDisplay *display,
        XCBWindow window,
        uint16_t instance_name_length,
        uint16_t class_name_length,
        XCBClassHint *hint
        );

/* 
 *
 * NOTE: This operation makes use of "strlen"
 *
 * RETURN: 0 on Success.
 * RETURN: 1 on Failure.
 */
int
XCBSetClassHint(
        XCBDisplay *display, 
        XCBWindow window, 
        XCBClassHint *hint
        );

/* Valuemasks
XCB_GC_FUNCTION
XCB_GC_PLANE_MASK
XCB_GC_FOREGROUND
XCB_GC_BACKGROUND
XCB_GC_LINE_WIDTH
XCB_GC_LINE_STYLE
XCB_GC_CAP_STYLE
XCB_GC_JOIN_STYLE
XCB_GC_FILL_STYLE
XCB_GC_FILL_RULE
XCB_GC_TILE
XCB_GC_STIPPLE
XCB_GC_TILE_STIPPLE_ORIGIN_X
XCB_GC_TILE_STIPPLE_ORIGIN_Y
XCB_GC_FONT
XCB_GC_SUBWINDOW_MODE
XCB_GC_GRAPHICS_EXPOSURES
XCB_GC_CLIP_ORIGIN_X
XCB_GC_CLIP_ORIGIN_Y
XCB_GC_CLIP_MASK
XCB_GC_DASH_OFFSET
XCB_GC_DASH_LIST
XCB_GC_ARC_MODE
*/
XCBCookie
XCBChangeGC(
        XCBDisplay *display, 
        XCBGC gc, 
        uint32_t valuemask, 
        XCBChangeGCValueList *va
        );

XCBCookie
XCBSetForeground(
        XCBDisplay *display, 
        XCBGC gc, 
        uint32_t colour
        );

/*
 */
int
XCBDiscardReply(
        XCBDisplay *display, 
        XCBCookie cookie);

/* dumb stuff */


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
void 
XCBPrefetchMaximumRequestLength(
        XCBDisplay *display
        );

/*
 * RETURN: Cookie to request.
 */
XCBCookie
XCBGetAtomNameCookie(
        XCBDisplay *display,
        XCBAtom atom
        );

/* Note strings returned from xcb_get_atom_name_name() are NOT null terminating, thus to safely use one must manually copy the string memcpy,
 * and add a nullbyte '\0' at the end.
 *
 *
 * RETURN: XCBAtomName * on Success.
 * RETURN: NULL on Failure.
 */
XCBAtomName *
XCBGetAtomNameReply(
        XCBDisplay *display,
        XCBCookie cookie
        );

/* This functions widens a cookies requets to match the 64 version of functions.
 *
 * NOTE: This function is NOT supported and may break at any time.
 * NOTE: This function consumes alot of memory ~.03GiB.
 * NOTE: DONOT USE THIS.
 * NOTE: Seriously dont.
 *
 * RETURN: XCBCookie64
 */
XCBCookie64
XCBWiden(
        XCBDisplay *display, 
        XCBCookie cookie
        );
/* This functions widens a cookies requets to match the 64 version of functions.
 *
 * NOTE: This function is NOT supported and may break at any time.
 * NOTE: This function consumes alot of memory ~.03GiB.
 * NOTE: DONOT USE THIS.
 * NOTE: Seriously dont.
 *
 * RETURN: XCBCookie64
 */
XCBCookie64 
XCBWidenCookie(
        XCBDisplay *display, 
        XCBCookie cookie
        );


XCBCookie
XCBKillClient(
        XCBDisplay *display, 
        XCBWindow win
        );



/*
 *  ICCCM
 */

typedef xcb_icccm_get_wm_protocols_reply_t XCBWMProtocols;
typedef xcb_icccm_wm_hints_t XCBWMHints;
typedef xcb_icccm_get_wm_class_reply_t XCBWMClass;
typedef xcb_size_hints_t XCBSizeHints;



enum
XCBWMHintsFlags
{
    XCB_WM_HINT_INPUT = XCB_ICCCM_WM_HINT_INPUT ,
    XCB_WM_HINT_STATE = XCB_ICCCM_WM_HINT_STATE ,
    XCB_WM_HINT_ICON_PIXMAP = XCB_ICCCM_WM_HINT_ICON_PIXMAP ,
    XCB_WM_HINT_ICON_WINDOW = XCB_ICCCM_WM_HINT_ICON_WINDOW ,
    XCB_WM_HINT_ICON_POSITION = XCB_ICCCM_WM_HINT_ICON_POSITION ,
    XCB_WM_HINT_ICON_MASK = XCB_ICCCM_WM_HINT_ICON_MASK ,
    XCB_WM_HINT_WINDOW_GROUP = XCB_ICCCM_WM_HINT_WINDOW_GROUP ,
    XCB_WM_HINT_URGENCY = XCB_ICCCM_WM_HINT_X_URGENCY,
    XCB_WM_HINT_ALL_HINTS = (XCB_ICCCM_WM_HINT_INPUT|
                            XCB_ICCCM_WM_HINT_STATE|
                            XCB_ICCCM_WM_HINT_ICON_PIXMAP|
                            XCB_ICCCM_WM_HINT_ICON_WINDOW|
                            XCB_ICCCM_WM_HINT_ICON_POSITION|
                            XCB_ICCCM_WM_HINT_ICON_MASK
                            |XCB_ICCCM_WM_HINT_WINDOW_GROUP
                            )
};

enum
XCBSizeHintsFlags
{
    XCB_SIZE_HINT_US_POSITION = XCB_ICCCM_SIZE_HINT_US_POSITION,
    XCB_SIZE_HINT_US_SIZE = XCB_ICCCM_SIZE_HINT_US_SIZE,
    XCB_SIZE_HINT_P_POSITION = XCB_ICCCM_SIZE_HINT_P_POSITION,
    XCB_SIZE_HINT_P_SIZE = XCB_ICCCM_SIZE_HINT_P_SIZE,
    XCB_SIZE_HINT_P_MIN_SIZE = XCB_ICCCM_SIZE_HINT_P_MIN_SIZE,
    XCB_SIZE_HINT_P_MAX_SIZE = XCB_ICCCM_SIZE_HINT_P_MAX_SIZE,
    XCB_SIZE_HINT_P_RESIZE_INC = XCB_ICCCM_SIZE_HINT_P_RESIZE_INC,
    XCB_SIZE_HINT_P_ASPECT = XCB_ICCCM_SIZE_HINT_P_ASPECT,
    XCB_SIZE_HINT_P_BASE_SIZE = XCB_ICCCM_SIZE_HINT_BASE_SIZE,
    XCB_SIZE_HINT_P_WIN_GRAVITY = XCB_ICCCM_SIZE_HINT_P_WIN_GRAVITY,
};

/* Queries the XServer for the wm_protocols avaible to the specified window.
 *
 * WM_PROTOCOLS_ATOM_ID:                The atom in which you may or may not have stored for the "WM_PROTOCOLS" atom.
 *                                      Ex: XCBGetWMProtocolsCookie(display, window, XInternAtom(display, "WM_PROTOCOLS", False));
 */
XCBCookie
XCBGetWMProtocolsCookie(
        XCBDisplay *display, 
        XCBWindow window, 
        XCBAtom WM_PROTOCOLS_ATOM_ID);
/*
 * Grabs the reply and returns a structure to the XCBGetWMProtocol containing the array length (atoms_len) and the array (*atoms).
 *
 * NOTE: CALLER MUST CALL XCBWipeGetWMProtocols() when done using data.
 *
 * RETURN: 1 On Success;
 * RETURN: 0 On Failure;
 */
int
XCBGetWMProtocolsReply(
        XCBDisplay *display, 
        XCBCookie cookie,
        XCBWMProtocols *protocol_return
        );

/* This frees resulting data from the protocols->_reply section of the provided structure.
 * 
 * NOTE: No protection against illegal memory frees, using free();
 *
 */
void
XCBWipeGetWMProtocols(
        XCBWMProtocols *protocols);

/*
 *
 * RETURN: Cookie to requets.
 */
XCBCookie
XCBGetWMHintsCookie(
        XCBDisplay *display,
        XCBWindow win
        );

/* 
 * NOTE: RETURN MUST BE RELEASED BY CALLER USING free().
 *
 * RETURN: XCBWMHints * on Success.
 * RETURN: NULL on Failure.
 */
XCBWMHints *
XCBGetWMHintsReply(
        XCBDisplay *display,
        XCBCookie cookie
        );
/*
 * RETURN: Cookie to requets.
 */
XCBCookie
XCBSetWMHintsCookie(
        XCBDisplay *display,
        XCBWindow window,
        XCBWMHints *wmhints
        );

/*
 */
XCBCookie
XCBGetWMNameCookie(
        XCBDisplay *display, 
        XCBWindow win);

/*
 * RETURN: 1 on Success.
 * RETURN: 0 on Failure.
 */
uint8_t
XCBGetWMNameReply(
        XCBDisplay *display, 
        XCBCookie cookie, 
        XCBTextProperty *prop_return
        );

/*
 */
XCBCookie
XCBSetWMNormalHints(
        XCBDisplay *display,
        XCBWindow window,
        XCBSizeHints *hints
        );
/*
 */
XCBCookie
XCBGetWMNormalHintsCookie(
        XCBDisplay *display,
        XCBWindow win
        );

/*
 */
uint8_t
XCBGetWMNormalHintsReply(
        XCBDisplay *display,
        XCBCookie cookie,
        XCBSizeHints *hints_return
        );
/*
 * RETURN: Cookie to Request.
 */
XCBCookie
XCBGetWMClassCookie(
        XCBDisplay *display, 
        XCBWindow win
        );
/*
 *
 * NOTE: Return must be wiped on Success, when done using XCBWipeGetWMClass();
 *
 * RETURN: 1 on Success.
 * RETURN: 0 on Failure.
 */
uint8_t
XCBGetWMClassReply(
        XCBDisplay *display,
        XCBCookie cookie,
        XCBWMClass *class_return
        );

/* This frees resulting data from the _class->_reply section of the provided structure.
 * 
 * NOTE: No protection against illegal memory frees, using free();
 *
 */
void
XCBWipeGetWMClass(
        XCBWMClass *_class
        );














/* Returns a null terminating string to the call stack seperate by a \0 byte to the next called function.
 * With the last 2 bytes being \0 bytes.
 *
 * NOTE: XCB_TRL_ENABLE_DEBUG must be defined for this function to return any meaningfull data.
 * NOTE: char * MUST be freed. when done.
 *
 * RETURN: char * On Success.
 * RETURN: NULL On Failure.
 */
char *
XCBDebugGetCallStack(
        void
        );

/* Returns a null terminating string to the last called function.
 *
 * NOTE: XCB_TRL_ENABLE_DEBUG must be defined for this function to return any meaningfull data.
 * NOTE: char * should NOT be freed.
 *
 * RETURN: char * On Success.
 * RETURN: NULL On Failure.
 */
char *
XCBDebugGetLastCall(
        void
        );

/* Returns a null terminating string to the previous last called function.
 *
 * NOTE: XCB_TRL_ENABLE_DEBUG must be defined for this function to return any meaningfull data.
 * NOTE: char * should NOT be freed.
 *
 * RETURN: char * On Success.
 * RETURN: NULL On Failure.
 */
char *
XCBDebugGetPreviousCall(
        void
        );
/* Returns a null terminating string to the last called function.
 *
 * NOTE: XCB_TRL_ENABLE_DEBUG must be defined for this function to return any meaningfull data.
 * NOTE: char * should NOT be freed.
 *
 * RETURN: char * On Success.
 * RETURN: NULL On Failure.
 */
char *
XCBDebugGetFirstCall(
        void
        );

char *
XCBDebugGetNameFromId(
        XCBCookie id
        );












/* 1356 xproto.h */
/* MASKS */
/*
 * modifier 
 * -----------------
 * XCB_MOD_MASK_SHIFT = 1,
 * XCB_MOD_MASK_LOCK = 2,
 * XCB_MOD_MASK_CONTROL = 4,
 * XCB_MOD_MASK_1 = 8,
 * XCB_MOD_MASK_2 = 16,
 * XCB_MOD_MASK_3 = 32,
 * XCB_MOD_MASK_4 = 64,
 * XCB_MOD_MASK_5 = 128,
 * XCB_MOD_MASK_ANY = 32768
 *
 * Key
 * ---------------------
 * XCB_KEY_BUT_MASK_SHIFT = 1,
 * XCB_KEY_BUT_MASK_LOCK = 2,
 * XCB_KEY_BUT_MASK_CONTROL = 4,
 * XCB_KEY_BUT_MASK_MOD_1 = 8,
 * XCB_KEY_BUT_MASK_MOD_2 = 16,
 * XCB_KEY_BUT_MASK_MOD_3 = 32,
 * XCB_KEY_BUT_MASK_MOD_4 = 64,
 * XCB_KEY_BUT_MASK_MOD_5 = 128,
 * XCB_KEY_BUT_MASK_BUTTON_1 = 256,
 * XCB_KEY_BUT_MASK_BUTTON_2 = 512,
 * XCB_KEY_BUT_MASK_BUTTON_3 = 1024,
 * XCB_KEY_BUT_MASK_BUTTON_4 = 2048,
 * XCB_KEY_BUT_MASK_BUTTON_5 = 4096
 *
 * Button
 * ----------------------
 *
 * XCB_BUTTON_MASK_1 = 256,
 * XCB_BUTTON_MASK_2 = 512,
 * XCB_BUTTON_MASK_3 = 1024,
 * XCB_BUTTON_MASK_4 = 2048,
 * XCB_BUTTON_MASK_5 = 4096,
 * XCB_BUTTON_MASK_ANY = 32768
 *
 * Visual Class 
 * ---------------------
 * XCB_VISUAL_CLASS_STATIC_GRAY = 0,
 * XCB_VISUAL_CLASS_GRAY_SCALE = 1,
 * XCB_VISUAL_CLASS_STATIC_COLOR = 2,
 * XCB_VISUAL_CLASS_PSEUDO_COLOR = 3,
 * XCB_VISUAL_CLASS_TRUE_COLOR = 4,
 * XCB_VISUAL_CLASS_DIRECT_COLOR = 5
 *
 * Event 
 * --------------------
 * XCB_EVENT_MASK_NO_EVENT = 0,
 * XCB_EVENT_MASK_KEY_PRESS = 1,
 * XCB_EVENT_MASK_KEY_RELEASE = 2,
 * XCB_EVENT_MASK_BUTTON_PRESS = 4,
 * XCB_EVENT_MASK_BUTTON_RELEASE = 8,
 * XCB_EVENT_MASK_ENTER_WINDOW = 16,
 * XCB_EVENT_MASK_LEAVE_WINDOW = 32,
 * XCB_EVENT_MASK_POINTER_MOTION = 64,
 * XCB_EVENT_MASK_POINTER_MOTION_HINT = 128,
 * XCB_EVENT_MASK_BUTTON_1_MOTION = 256,
 * XCB_EVENT_MASK_BUTTON_2_MOTION = 512,
 * XCB_EVENT_MASK_BUTTON_3_MOTION = 1024,
 * XCB_EVENT_MASK_BUTTON_4_MOTION = 2048,
 * XCB_EVENT_MASK_BUTTON_5_MOTION = 4096,
 * XCB_EVENT_MASK_BUTTON_MOTION = 8192,
 * XCB_EVENT_MASK_KEYMAP_STATE = 16384,
 * XCB_EVENT_MASK_EXPOSURE = 32768,
 * XCB_EVENT_MASK_VISIBILITY_CHANGE = 65536,
 * XCB_EVENT_MASK_STRUCTURE_NOTIFY = 131072,
 * XCB_EVENT_MASK_RESIZE_REDIRECT = 262144,
 * XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY = 524288,
 * XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT = 1048576,
 * XCB_EVENT_MASK_FOCUS_CHANGE = 2097152,
 * XCB_EVENT_MASK_PROPERTY_CHANGE = 4194304,
 * XCB_EVENT_MASK_COLOR_MAP_CHANGE = 8388608,
 * XCB_EVENT_MASK_OWNER_GRAB_BUTTON = 16777216
 *
 * Motion
 * -------------------------------
 * XCB_MOTION_NORMAL = 0,
 * XCB_MOTION_HINT = 1
 *
 * Notify Detail
 * ------------------------------
 * XCB_NOTIFY_DETAIL_ANCESTOR = 0,
 * XCB_NOTIFY_DETAIL_VIRTUAL = 1,
 * XCB_NOTIFY_DETAIL_INFERIOR = 2,
 * XCB_NOTIFY_DETAIL_NONLINEAR = 3,
 * XCB_NOTIFY_DETAIL_NONLINEAR_VIRTUAL = 4,
 * XCB_NOTIFY_DETAIL_POINTER = 5,
 * XCB_NOTIFY_DETAIL_POINTER_ROOT = 6,
 * XCB_NOTIFY_DETAIL_NONE = 7
 *
 * Notify Mode
 * -----------------------------
 * XCB_NOTIFY_MODE_NORMAL = 0,
 * XCB_NOTIFY_MODE_GRAB = 1,
 * XCB_NOTIFY_MODE_UNGRAB = 2,
 * XCB_NOTIFY_MODE_WHILE_GRABBED = 3
 *
 * Visiblity 
 * ----------------------------
 * XCB_VISIBILITY_UNOBSCURED = 0,
 * XCB_VISIBILITY_PARTIALLY_OBSCURED = 1,
 * XCB_VISIBILITY_FULLY_OBSCURED = 2
 *
 * Place
 * ----------------------------
 * XCB_PLACE_ON_TOP = 0,
 * XCB_PLACE_ON_BOTTOM = 1
 *
 * Property
 * ----------------------------
 * XCB_PROPERTY_NEW_VALUE = 0,
 * XCB_PROPERTY_DELETE = 1
 *
 * Time
 * ----------------------------
 * XCB_TIME_CURRENT_TIME = 0
 *
 * Atom (Comparable to Xlib's XA_ properties, which is short of X_ATOM which is short for XSERVER_ATOM)
 * ----------------------------
 * XCB_ATOM_NONE = 0,
 * XCB_ATOM_ANY = 0,
 * XCB_ATOM_PRIMARY = 1,
 * XCB_ATOM_SECONDARY = 2,
 * XCB_ATOM_ARC = 3,
 * XCB_ATOM_ATOM = 4,
 * XCB_ATOM_BITMAP = 5,
 * XCB_ATOM_CARDINAL = 6,
 * XCB_ATOM_COLORMAP = 7,
 * XCB_ATOM_CURSOR = 8,
 * XCB_ATOM_CUT_BUFFER0 = 9,
 * XCB_ATOM_CUT_BUFFER1 = 10,
 * XCB_ATOM_CUT_BUFFER2 = 11,
 * XCB_ATOM_CUT_BUFFER3 = 12,
 * XCB_ATOM_CUT_BUFFER4 = 13,
 * XCB_ATOM_CUT_BUFFER5 = 14,
 * XCB_ATOM_CUT_BUFFER6 = 15,
 * XCB_ATOM_CUT_BUFFER7 = 16,
 * XCB_ATOM_DRAWABLE = 17,
 * XCB_ATOM_FONT = 18,
 * XCB_ATOM_INTEGER = 19,
 * XCB_ATOM_PIXMAP = 20,
 * XCB_ATOM_POINT = 21,
 * XCB_ATOM_RECTANGLE = 22,
 * XCB_ATOM_RESOURCE_MANAGER = 23,
 * XCB_ATOM_RGB_COLOR_MAP = 24,
 * XCB_ATOM_RGB_BEST_MAP = 25,
 * XCB_ATOM_RGB_BLUE_MAP = 26,
 * XCB_ATOM_RGB_DEFAULT_MAP = 27,
 * XCB_ATOM_RGB_GRAY_MAP = 28,
 * XCB_ATOM_RGB_GREEN_MAP = 29,
 * XCB_ATOM_RGB_RED_MAP = 30,
 * XCB_ATOM_STRING = 31,
 * XCB_ATOM_VISUALID = 32,
 * XCB_ATOM_WINDOW = 33,
 * XCB_ATOM_WM_COMMAND = 34,
 * XCB_ATOM_WM_HINTS = 35,
 * XCB_ATOM_WM_CLIENT_MACHINE = 36,
 * XCB_ATOM_WM_ICON_NAME = 37,
 * XCB_ATOM_WM_ICON_SIZE = 38,
 * XCB_ATOM_WM_NAME = 39,
 * XCB_ATOM_WM_NORMAL_HINTS = 40,
 * XCB_ATOM_WM_SIZE_HINTS = 41,
 * XCB_ATOM_WM_ZOOM_HINTS = 42,
 * XCB_ATOM_MIN_SPACE = 43,
 * XCB_ATOM_NORM_SPACE = 44,
 * XCB_ATOM_MAX_SPACE = 45,
 * XCB_ATOM_END_SPACE = 46,
 * XCB_ATOM_SUPERSCRIPT_X = 47,
 * XCB_ATOM_SUPERSCRIPT_Y = 48,
 * XCB_ATOM_SUBSCRIPT_X = 49,
 * XCB_ATOM_SUBSCRIPT_Y = 50,
 * XCB_ATOM_UNDERLINE_POSITION = 51,
 * XCB_ATOM_UNDERLINE_THICKNESS = 52,
 * XCB_ATOM_STRIKEOUT_ASCENT = 53,
 * XCB_ATOM_STRIKEOUT_DESCENT = 54,
 * XCB_ATOM_ITALIC_ANGLE = 55,
 * XCB_ATOM_X_HEIGHT = 56,
 * XCB_ATOM_QUAD_WIDTH = 57,
 * XCB_ATOM_WEIGHT = 58,
 * XCB_ATOM_POINT_SIZE = 59,
 * XCB_ATOM_RESOLUTION = 60,
 * XCB_ATOM_COPYRIGHT = 61,
 * XCB_ATOM_NOTICE = 62,
 * XCB_ATOM_FONT_NAME = 63,
 * XCB_ATOM_FAMILY_NAME = 64,
 * XCB_ATOM_FULL_NAME = 65,
 * XCB_ATOM_CAP_HEIGHT = 66,
 *
 * Colormap State
 * ------------------------------
 * XCB_COLORMAP_STATE_UNINSTALLED = 0,
 * XCB_COLORMAP_STATE_INSTALLED = 1
 * 
 * Colormap
 * ------------------------------
 * XCB_COLORMAP_NONE = 0
 *
 * Mapping
 * ------------------------------
 * XCB_MAPPING_MODIFIER = 0,
 * XCB_MAPPING_KEYBOARD = 1,
 * XCB_MAPPING_POINTER = 2
 *
 * Window Class
 * ------------------------------
 * XCB_WINDOW_CLASS_COPY_FROM_PARENT = 0,
 * XCB_WINDOW_CLASS_INPUT_OUTPUT = 1,
 * XCB_WINDOW_CLASS_INPUT_ONLY = 2
 *
 *
 *
 * Backing store
 * ------------------------------
 * XCB_BACKING_STORE_NOT_USEFUL = 0,
 * XCB_BACKING_STORE_WHEN_MAPPED = 1,
 * XCB_BACKING_STORE_ALWAYS = 2
 *
 * Image order
 * --------------------------
 * XCB_IMAGE_ORDER_LSB_FIRST = 0,
 * XCB_IMAGE_ORDER_MSB_FIRST = 1
 *
 * Window
 * -----------------------
 * XCB_WINDOW_NONE = 0,
 *
*/
#endif
