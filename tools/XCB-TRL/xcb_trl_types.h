#ifndef __XCB__TRL__TYPES__H__
#define __XCB__TRL__TYPES__H__



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

/*  FILE xcb_conn.c ;
 *  TROLL Opaque structure;
 */
typedef xcb_connection_t XCBConnection;
/*  FILE: src/xcb_conn.c ;
 *  TROLL Opaque structure;
 */
typedef xcb_connection_t XCBDisplay;
/* FILE: xcb/xcb.h ;
 * PUBLIC: ;
 * u8 status -> ;
 * u16 protocol_major_version -> ;
 * u16 protocol_minor_version -> ;
 * u16 length -> ;
 * u32 release_number -> ;
 * u32 resource_id_base -> ;
 * u32 resource_id_mask -> ;
 * u32 motion_buffer_size -> ;
 * u16 vendor_len -> ;
 * u16 maximum_request_length -> ;
 * u8  roots_len -> ;
 * u8  pixmap_formats_len -> ;
 * u8  image_byte_order -> ;
 * u8  bitmap_format_bit_order -> ;
 * u8  bitmap_format_scanline_unit -> ;
 * u8  bitmap_format_scanline_pad -> ;
 * XCBKeyCode min_keycode -> ;
 * XCBKeyCode max_keycode -> ;
 * PRIVATE: ;
 * u8 pad0 -> This should be considered READONLY ;
 * u8 pad1[4] -> This should be considered READONLY ;
 */
typedef xcb_setup_t XCBSetup;
typedef xcb_screen_iterator_t XCBScreenIterator;
typedef xcb_screen_t XCBScreen;
typedef xcb_window_t XCBWindow;
typedef xcb_cursor_t XCBCursor;
typedef xcb_icccm_get_text_property_reply_t XCBTextProperty;
typedef xcb_grab_keyboard_reply_t XCBGrabKeyboard;
typedef xcb_grab_pointer_reply_t XCBGrabPointer;
typedef xcb_void_cookie_t XCBCookie;
typedef xcb_get_keyboard_mapping_reply_t XCBKeyboardMapping;
typedef xcb_get_modifier_mapping_reply_t XCBKeyboardModifier;
typedef xcb_colormap_t XCBColormap;
typedef xcb_atom_t XCBAtom;
typedef xcb_time_t XCBTime;
typedef xcb_timestamp_t XCBTimestamp;
typedef xcb_client_message_data_t  XCBClientMessageData;
typedef xcb_get_property_reply_t XCBWindowProperty;
typedef xcb_create_window_value_list_t XCBCreateWindowValueList;



/* Ghosts */
typedef xcb_create_gc_value_list_t XCBCreateGCValueList;
typedef xcb_change_gc_value_list_t XCBChangeGCValueList;

/* Analagous to Xlib's XA_(type)
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
*/
typedef xcb_atom_enum_t XCBAtomType;
typedef xcb_keysym_t XCBKeysym;
typedef xcb_key_symbols_t XCBKeySymbols;
typedef xcb_keycode_t XCBKeycode;
typedef xcb_keycode_t XCBKeyCode;

typedef xcb_visualid_t XCBVisual;


typedef struct XCBClassHint XCBClassHint;
typedef struct XCBColor XCBColor;

struct XCBClassHint
{
    char *instance_name;
    char *class_name;
};

struct XCBColor
{
    uint64_t pixel;

    uint16_t red;
    uint16_t green;

    uint16_t blue;
    uint8_t flags;      /* XCB_DO_RED.
                         * XCB_DO_GREEN.
                         * XCB_DO_BLUE.
                         */
    uint8_t pad;
};

typedef xcb_configure_window_value_list_t XCBWindowChanges;
typedef xcb_query_extension_reply_t XCBQueryExtension;
typedef xcb_query_tree_reply_t XCBQueryTree;
typedef xcb_get_window_attributes_reply_t XCBGetWindowAttributes;
typedef xcb_change_window_attributes_value_list_t XCBWindowAttributes;
typedef xcb_get_atom_name_reply_t XCBAtomName;
typedef xcb_get_geometry_reply_t XCBGeometry;
typedef xcb_get_geometry_reply_t XCBWindowGeometry;
typedef xcb_pixmap_t XCBPixmap;
typedef xcb_gcontext_t XCBGC;
typedef xcb_drawable_t XCBDrawable;
typedef xcb_font_t XCBFont;
typedef xcb_generic_event_t XCBGenericEvent;
typedef xcb_generic_event_t XCBEvent;

typedef xcb_generic_reply_t XCBGenericReply;
typedef xcb_generic_reply_t XCBReply;

typedef xcb_generic_error_t XCBGenericError;
typedef xcb_generic_error_t XCBError;

typedef xcb_query_pointer_reply_t XCBQueryPointer;

/* Xinerama */

typedef xcb_xinerama_screen_info_t XCBXineramaScreenInfo;
typedef xcb_xinerama_is_active_reply_t XCBXineramaIsActive;

typedef xcb_xinerama_query_screens_reply_t XCBXineramaQueryScreens;



/* events */
typedef xcb_key_press_event_t XCBKeyPressEvent;
typedef xcb_key_release_event_t XCBKeyReleaseEvent;
typedef xcb_button_press_event_t XCBButtonPressEvent;
typedef xcb_button_release_event_t XCBButtonReleaseEvent;
typedef xcb_motion_notify_event_t XCBMotionNotifyEvent;
typedef xcb_enter_notify_event_t XCBEnterNotifyEvent;

typedef xcb_leave_notify_event_t XCBLeaveNotifyEvent;
typedef xcb_focus_in_event_t XCBFocusInEvent;
typedef xcb_focus_out_event_t XCBFocusOutEvent;
typedef xcb_keymap_notify_event_t XCBKeymapNotifyEvent;
typedef xcb_expose_event_t XCBExposeEvent;

typedef xcb_graphics_exposure_event_t XCBGraphicsExposeEvent;
typedef xcb_graphics_exposure_event_t XCBGraphicsExposureEvent;
typedef xcb_no_exposure_event_t XCBNoExposeEvent;
typedef xcb_no_exposure_event_t XCBNoExposureEvent;
typedef xcb_circulate_notify_event_t XCBCirculateNotifyEvent;
typedef xcb_circulate_request_event_t XCBCirculateRequestEvent;
typedef xcb_map_request_event_t XCBMapRequestEvent;
typedef xcb_configure_request_event_t XCBConfigureRequestEvent;
typedef xcb_configure_notify_event_t XCBConfigureNotifyEvent;
typedef xcb_resize_request_event_t XCBResizeRequestEvent;
typedef xcb_create_notify_event_t XCBCreateNotifyEvent;
typedef xcb_destroy_notify_event_t XCBDestroyNotifyEvent;
typedef xcb_gravity_notify_event_t XCBGravityNotifyEvent;
typedef xcb_map_notify_event_t XCBMapNotifyEvent;
typedef xcb_mapping_notify_event_t XCBMappingNotifyEvent;
typedef xcb_unmap_notify_event_t XCBUnmapNotifyEvent;
typedef xcb_visibility_notify_event_t XCBVisibilityNotifyEvent;
typedef xcb_reparent_notify_event_t XCBReparentNotifyEvent;
typedef xcb_colormap_notify_event_t XCBColormapNotifyEvent;
typedef xcb_colormap_notify_event_t XCBColorMapNotifyEvent;
typedef xcb_client_message_event_t XCBClientMessageEvent;
typedef xcb_property_notify_event_t XCBPropertyNotifyEvent;
typedef xcb_selection_clear_event_t XCBSelectionClearEvent;
typedef xcb_selection_notify_event_t XCBSelectionNotifyEvent;
typedef xcb_selection_request_event_t XCBSelectionRequestEvent;
/* This is NOT short for XCBGenericEvent rather is used for Ge Events */
typedef xcb_ge_generic_event_t XCBGeEvent;


typedef struct XCBCookie64 XCBCookie64;
typedef union XCBARGB XCBARGB;

/* structs */
struct XCBCookie64
{   uint64_t sequence;
};

/* ORDER.
 * BLUE + (GREEN << 8) + (RED << 16) + (ALPHA << 24)
 */
union XCBARGB
{
    struct
    {
#if __BYTE_ORDER == __ORDER_LITTLE_ENDIAN__
    uint8_t b;  /* Blue Value  */
    uint8_t g;  /* Green Value */
    uint8_t r;  /* Red Value   */
    uint8_t a;  /* Alpha value */
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    uint8_t a;  /* Alpha value */
    uint8_t r;  /* Red Value   */
    uint8_t g;  /* Green Value */
    uint8_t b;  /* Blue Value  */
#else
    uint8_t pad[4];
    /* 
     * NO SUPPORTED ENDIAN TYPE.
     * If you are using PDP_ENDIAN you might have to manually shift the values yourself.
     */
    #error "No supported endian type. If you are using PDP_ENDIAN you might have to manually shift the values yourself."
#endif
    } c;
    uint32_t argb;  /* ARGB 32bit value */
};

/* macros */
enum 
XCBWMWindowState
{
    XCB_WINDOW_NORMAL_STATE = XCB_ICCCM_WM_STATE_NORMAL,
    XCB_WINDOW_ICONIC_STATE = XCB_ICCCM_WM_STATE_ICONIC,
    XCB_WINDOW_WITHDRAWN_STATE = XCB_ICCCM_WM_STATE_WITHDRAWN,
    XCB_WINDOW_WM_HINT_STATE = XCB_ICCCM_WM_HINT_STATE,
};


/* mostly cause of some design choices there are here.
 * But To be honest the regular XCB_ type's are prefered, though some are wierd.
 *
 * "Technicallities"
 * Technically XCB just copies that X.h had onto its base, but technically it could change the numbers at any time which is what this is mostly for.
 * Except for GX for some reason?
 */

 #define XCBNone               XCB_NONE

 /***************************************************************** 
 * EVENT DEFINITIONS 
 *****************************************************************/

/* Input Event Masks. Used as event-mask window attribute and as arguments
   to Grab requests.  Not to be confused with event names.  */
enum
{    
    XCBNoEventMask               = XCB_EVENT_MASK_NO_EVENT,
    XCBKeyPressMask              = XCB_EVENT_MASK_KEY_PRESS,
    XCBKeyReleaseMask            = XCB_EVENT_MASK_KEY_RELEASE,
    XCBButtonPressMask           = XCB_EVENT_MASK_BUTTON_PRESS,
    XCBButtonReleaseMask         = XCB_EVENT_MASK_BUTTON_RELEASE,
    XCBEnterWindowMask           = XCB_EVENT_MASK_ENTER_WINDOW,
    XCBLeaveWindowMask           = XCB_EVENT_MASK_LEAVE_WINDOW,
    XCBPointerMotionMask         = XCB_EVENT_MASK_POINTER_MOTION,
    XCBPointerMotionHintMask     = XCB_EVENT_MASK_POINTER_MOTION_HINT,
    XCBButton1MotionMask         = XCB_EVENT_MASK_BUTTON_1_MOTION,
    XCBButton2MotionMask         = XCB_EVENT_MASK_BUTTON_2_MOTION,
    XCBButton3MotionMask         = XCB_EVENT_MASK_BUTTON_3_MOTION,
    XCBButton4MotionMask         = XCB_EVENT_MASK_BUTTON_4_MOTION,
    XCBButton5MotionMask         = XCB_EVENT_MASK_BUTTON_5_MOTION,
    XCBButtonMotionMask          = XCB_EVENT_MASK_BUTTON_MOTION,
    XCBKeymapStateMask           = XCB_EVENT_MASK_KEYMAP_STATE,
    XCBExposureMask              = XCB_EVENT_MASK_EXPOSURE,
    XCBVisibilityChangeMask      = XCB_EVENT_MASK_VISIBILITY_CHANGE,
    XCBStructureNotifyMask       = XCB_EVENT_MASK_STRUCTURE_NOTIFY,
    XCBResizeRedirectMask        = XCB_EVENT_MASK_RESIZE_REDIRECT,
    XCBSubstructureNotifyMask    = XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY,
    XCBSubstructureRedirectMask  = XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
    XCBFocusChangeMask           = XCB_EVENT_MASK_FOCUS_CHANGE,
    XCBPropertyChangeMask        = XCB_EVENT_MASK_PROPERTY_CHANGE,
    XCBColormapChangeMask        = XCB_EVENT_MASK_COLOR_MAP_CHANGE,
    XCBOwnerGrabButtonMask       = XCB_EVENT_MASK_OWNER_GRAB_BUTTON,
};

/* Event names.  Used in "type" field in XEvent structures.  Not to be
confused with event masks above.  They start from 2 because 0 and 1
are reserved in the protocol for errors and replies. */
enum
{ 
    XCBKeyPress           = XCB_KEY_PRESS,
    XCBKeyRelease         = XCB_KEY_RELEASE,
    XCBButtonPress        = XCB_BUTTON_PRESS,
    XCBButtonRelease      = XCB_BUTTON_RELEASE,
    XCBMotionNotify       = XCB_MOTION_NOTIFY,
    XCBEnterNotify        = XCB_ENTER_NOTIFY,
    XCBLeaveNotify        = XCB_LEAVE_NOTIFY,
    XCBFocusIn            = XCB_FOCUS_IN,
    XCBFocusOut           = XCB_FOCUS_OUT,
    XCBKeymapNotify       = XCB_KEYMAP_NOTIFY,
    XCBExpose             = XCB_EXPOSE,
    XCBGraphicsExpose     = XCB_GRAPHICS_EXPOSURE,
    XCBNoExpose           = XCB_NO_EXPOSURE,
    XCBVisibilityNotify   = XCB_VISIBILITY_NOTIFY,
    XCBCreateNotify       = XCB_CREATE_NOTIFY,
    XCBDestroyNotify      = XCB_DESTROY_NOTIFY,
    XCBUnmapNotify        = XCB_UNMAP_NOTIFY,
    XCBMapNotify          = XCB_MAP_NOTIFY,
    XCBMapRequest         = XCB_MAP_REQUEST,
    XCBReparentNotify     = XCB_REPARENT_NOTIFY,
    XCBConfigureNotify    = XCB_CONFIGURE_NOTIFY,
    XCBConfigureRequest   = XCB_CONFIGURE_REQUEST,
    XCBGravityNotify      = XCB_GRAVITY_NOTIFY,
    XCBResizeRequest      = XCB_RESIZE_REQUEST,
    XCBCirculateNotify    = XCB_CIRCULATE_NOTIFY,
    XCBCirculateRequest   = XCB_CIRCULATE_REQUEST,
    XCBPropertyNotify     = XCB_PROPERTY_NOTIFY,
    XCBSelectionClear     = XCB_SELECTION_CLEAR,
    XCBSelectionRequest   = XCB_SELECTION_REQUEST,
    XCBSelectionNotify    = XCB_SELECTION_NOTIFY,
    XCBColormapNotify     = XCB_COLORMAP_NOTIFY,
    XCBClientMessage      = XCB_CLIENT_MESSAGE,
    XCBMappingNotify      = XCB_MAPPING_NOTIFY,
    XCBGeGeneric          = XCB_GE_GENERIC,
    XCBLASTEvent          = ((XCB_GE_GENERIC > 36 ? XCB_GE_GENERIC + 1 : 36)) /* must be bigger than any event number */
};
/* Key masks. Used as modifiers to GrabButton and GrabKey, results of QueryPointer,
   state in various key-, mouse-, and button-related events. */
enum
{
    XCBModMaskAny     = XCB_MOD_MASK_ANY,
    XCBShiftMask      = XCB_MOD_MASK_SHIFT,
    XCBLockMask       = XCB_MOD_MASK_LOCK,
    XCBControlMask    = XCB_MOD_MASK_CONTROL,
    XCBMod1Mask       = XCB_MOD_MASK_1,
    XCBMod2Mask       = XCB_MOD_MASK_2,
    XCBMod3Mask       = XCB_MOD_MASK_3,
    XCBMod4Mask       = XCB_MOD_MASK_4,
    XCBMod5Mask       = XCB_MOD_MASK_5,
};

/* modifier names.  Used to build a SetModifierMapping request or
   to read a GetModifierMapping request.  These correspond to the
   masks defined above. */
enum
{    
    XCBShiftMapIndex    = XCB_MAP_INDEX_SHIFT,
    XCBLockMapIndex     = XCB_MAP_INDEX_LOCK,
    XCBControlMapIndex  = XCB_MAP_INDEX_CONTROL,
    XCBMod1MapIndex     = XCB_MAP_INDEX_1,
    XCBMod2MapIndex     = XCB_MAP_INDEX_2,
    XCBMod3MapIndex     = XCB_MAP_INDEX_3,
    XCBMod4MapIndex     = XCB_MAP_INDEX_4,
    XCBMod5MapIndex     = XCB_MAP_INDEX_5,
};
/* button masks.  Used in same manner as Key masks above. Not to be confused
   with button names below. */
enum 
{
    XCBButton1Mask = XCB_BUTTON_MASK_1,
    XCBButton2Mask = XCB_BUTTON_MASK_2,
    XCBButton3Mask = XCB_BUTTON_MASK_3,
    XCBButton4Mask = XCB_BUTTON_MASK_4,
    XCBButton5Mask = XCB_BUTTON_MASK_5,
    /* extras LIMIT 32768 */
    XCBButton6Mask = XCBButton5Mask << 1,
};

/* extra's if you need more buttons just define them as the number ya want */
#define XCB_BUTTON_INDEX_6                  (6)
/*
#define XCB_BUTTON_INDEX_7                  (7)
#define XCB_BUTTON_INDEX_8                  (8)
#define XCB_BUTTON_INDEX_9                  (9)
#define XCB_BUTTON_INDEX_10                 (10)
#define XCB_BUTTON_INDEX_11                 (11)
#define XCB_BUTTON_INDEX_12                 (12)
#define XCB_BUTTON_INDEX_13                 (13)
#define XCB_BUTTON_INDEX_14                 (14)
#define XCB_BUTTON_INDEX_15                 (15)
#define XCB_BUTTON_INDEX_16                 (16)
#define XCB_BUTTON_INDEX_17                 (17)
#define XCB_BUTTON_INDEX_18                 (18)
#define XCB_BUTTON_INDEX_19                 (19)
#define XCB_BUTTON_INDEX_20                 (20)
*/

/* button names. Used as arguments to GrabButton and as detail in ButtonPress
 * and ButtonRelease events.  Not to be confused with button masks above.
 * Note that 0 is already defined above as "AnyButton".  */
enum 
{
    /* This is a button number, NOT a button mask. */
    XCBButtonAny = XCB_BUTTON_INDEX_ANY,
    XCBButton1 = XCB_BUTTON_INDEX_1,
    XCBButton2 = XCB_BUTTON_INDEX_2,
    XCBButton3 = XCB_BUTTON_INDEX_3,
    XCBButton4 = XCB_BUTTON_INDEX_4,
    XCBButton5 = XCB_BUTTON_INDEX_5,
    XCBButton6 = XCB_BUTTON_INDEX_6,
};

/* Notify modes */
enum 
{
    XCBNotifyNormal = XCB_NOTIFY_MODE_NORMAL,
    XCBNotifyGrab = XCB_NOTIFY_MODE_GRAB,
    XCBNotifyUngrab = XCB_NOTIFY_MODE_UNGRAB,
    XCBNotifyWhileGrabbed = XCB_NOTIFY_MODE_WHILE_GRABBED,
    /* for MotionNotify events */
    XCBNotifyHint = XCB_MOTION_HINT,
};


/* Notify detail */

enum 
{
    XCBNotifyAncestor = XCB_NOTIFY_DETAIL_ANCESTOR,
    XCBNotifyVirtual = XCB_NOTIFY_DETAIL_VIRTUAL,
    XCBNotifyInferior = XCB_NOTIFY_DETAIL_INFERIOR,
    XCBNotifyNonlinear = XCB_NOTIFY_DETAIL_NONLINEAR,
    XCBNotifyNonlinearVirtual = XCB_NOTIFY_DETAIL_NONLINEAR_VIRTUAL,
    XCBNotifyPointer = XCB_NOTIFY_DETAIL_POINTER,
    XCBNotifyPointerRoot = XCB_NOTIFY_DETAIL_POINTER_ROOT,
    XCBNotifyDetailNone = XCB_NOTIFY_DETAIL_NONE,
};
/* Visibility notify */

enum XCBVisibility
{
    XCBVisibilityUnobscured = XCB_VISIBILITY_UNOBSCURED,
    XCBVisibilityPartiallyObscured = XCB_VISIBILITY_PARTIALLY_OBSCURED,
    XCBVisibilityFullyObscured = XCB_VISIBILITY_FULLY_OBSCURED,
};
/* Circulation request */

enum XCBPlacement 
{
    XCBPlaceOnTop = XCB_PLACE_ON_TOP,
    XCBPlaceOnBottom = XCB_PLACE_ON_BOTTOM,
};

/* protocol families */
enum XCBFamily 
{
    /* IPv4 */
    XCBFamilyInternet = XCB_FAMILY_INTERNET,
    XCBFamilyDECnet = XCB_FAMILY_DECNET,
    XCBFamilyChaos = XCB_FAMILY_CHAOS,
    /* authentication families not tied to a specific protocol */
    XCBFamilyServerInterpreted = XCB_FAMILY_SERVER_INTERPRETED,
    /* IPv6 */
    XCBFamilyInternet6 = XCB_FAMILY_INTERNET_6,
};
/* Property notification */
enum XCBPropertyNotification 
{
    XCBPropertyNewValue = XCB_PROPERTY_NEW_VALUE,
    XCBPropertyDelete = XCB_PROPERTY_DELETE,
};
/* Color Map notification */
enum XCBColorMapNotification 
{
    XCBColormapUninstalled = XCB_COLORMAP_STATE_UNINSTALLED,
    XCBColormapInstalled = XCB_COLORMAP_STATE_INSTALLED,
};
/* GrabPointer, GrabButton, GrabKeyboard, GrabKey Modes */
enum XCBGrabMode 
{
    XCBGrabModeSync = XCB_GRAB_MODE_SYNC,
    XCBGrabModeAsync = XCB_GRAB_MODE_ASYNC,
};

/* GrabPointer, GrabKeyboard reply status */
enum XCBGrabStatus
{
    XCBGrabSuccess = XCB_GRAB_STATUS_SUCCESS,
    XCBAlreadyGrabbed = XCB_GRAB_STATUS_ALREADY_GRABBED,
    XCBGrabInvalidTime = XCB_GRAB_STATUS_INVALID_TIME,
    XCBGrabNotViewable = XCB_GRAB_STATUS_NOT_VIEWABLE,
    XCBGrabFrozen = XCB_GRAB_STATUS_FROZEN,
};
/* AllowEvents modes */
enum XCBAllowEventsMode
{
    XCBAsyncPointer = XCB_ALLOW_ASYNC_POINTER,
    XCBSyncPointer = XCB_ALLOW_SYNC_POINTER,
    XCBReplayPointer = XCB_ALLOW_REPLAY_POINTER,
    XCBAsyncKeyboard = XCB_ALLOW_ASYNC_KEYBOARD,
    XCBSyncKeyboard = XCB_ALLOW_SYNC_KEYBOARD,
    XCBReplayKeyboard = XCB_ALLOW_REPLAY_KEYBOARD,
    XCBAsyncBoth = XCB_ALLOW_ASYNC_BOTH,
    XCBSyncBoth = XCB_ALLOW_SYNC_BOTH,
};
/* Used in SetInputFocus, GetInputFocus */
enum XCBInputFocusMode
{
    XCBRevertToNone = XCB_INPUT_FOCUS_NONE,
    XCBRevertToPointerRoot = XCB_INPUT_FOCUS_POINTER_ROOT,
    XCBRevertToParent = XCB_INPUT_FOCUS_PARENT,
       /* NOT YET DOCUMENTED. Only relevant for the xinput extension. */
    XCBRevertToFollowKeyboard = XCB_INPUT_FOCUS_FOLLOW_KEYBOARD,
};

/*****************************************************************
 * ERROR CODES (Mostly to interface with X but not with XCB)
 *****************************************************************/

enum
{
    XCBSuccess,		    /* everything's okay                        */
    XCBBadRequest,	    /* bad request code                         */
    XCBBadValue,	    /* int parameter out of range               */
    XCBBadWindow,	    /* parameter not a Window                   */
    XCBBadPixmap,	    /* parameter not a Pixmap                   */
    XCBBadAtom,		    /* parameter not an Atom                    */
    XCBBadCursor,	    /* parameter not a Cursor                   */
    XCBBadFont,		    /* parameter not a Font                     */
    XCBBadMatch,	    /* parameter mismatch                       */
    XCBBadDrawable,	    /* parameter not a Pixmap or Window         */
    XCBBadAccess,	    /* depending on context:
                         *   - key/button already grabbed
                         *   - attempt to free an illegal
                         *        cmap entry
                         *   - attempt to store into a read-only
                         *        color map entry.
                         *   - attempt to modify the access control
                         *        list from other than the local host.
                         */
    XCBBadAlloc,	    /* insufficient resources                   */
    XCBBadColor,	    /* no such colormap                         */
    XCBBadGC,		    /* parameter not a GC                       */
    XCBBadIDChoice,	    /* choice not in range or already used      */
    XCBBadName,		    /* font or color name doesn't exist         */
    XCBBadLength,	    /* Request length incorrect                 */
    XCBBadImplementation,/* server is defective                     */




    XCBFirstExtensionError = 128,
    XCBLastExtensionError = 255,
};

enum
{
    XCB_Error,
    XCB_Reply,
    XCB_CreateWindow = XCB_Reply,
    XCB_ChangeWindowAttributes,
    XCB_GetWindowAttributes,
    XCB_DestroyWindow,
    XCB_DestroySubwindows,
    XCB_ChangeSaveSet,
    XCB_ReparentWindow,
    XCB_MapWindow,
    XCB_MapSubwindows,
    XCB_UnmapWindow,
    XCB_UnmapSubwindows,
    XCB_ConfigureWindow,
    XCB_CirculateWindow,
    XCB_GetGeometry,
    XCB_QueryTree,
    XCB_InternAtom,
    XCB_GetAtomName,
    XCB_ChangeProperty,
    XCB_DeleteProperty,
    XCB_GetProperty,
    XCB_ListProperties,
    XCB_SetSelectionOwner,
    XCB_GetSelectionOwner,
    XCB_ConvertSelection,
    XCB_SendEvent,
    XCB_GrabPointer,
    XCB_UngrabPointer,
    XCB_GrabButton,
    XCB_UngrabButton,
    XCB_ChangeActivePointerGrab,
    XCB_GrabKeyboard,
    XCB_UngrabKeyboard,
    XCB_GrabKey,
    XCB_UngrabKey,
    XCB_AllowEvents,
    XCB_GrabServer,
    XCB_UngrabServer,
    XCB_QueryPointer,
    XCB_GetMotionEvents,
    XCB_TranslateCoords,
    XCB_WarpPointer,
    XCB_SetInputFocus,
    XCB_GetInputFocus,
    XCB_QueryKeymap,
    XCB_OpenFont,
    XCB_CloseFont,
    XCB_QueryFont,
    XCB_QueryTextExtents,
    XCB_ListFonts,
    XCB_ListFontsWithInfo,
    XCB_SetFontPath,
    XCB_GetFontPath,
    XCB_CreatePixmap,
    XCB_FreePixmap,
    XCB_CreateGC,
    XCB_ChangeGC,
    XCB_CopyGC,
    XCB_SetDashes,
    XCB_SetClipRectangles,
    XCB_FreeGC,
    XCB_ClearArea,
    XCB_CopyArea,
    XCB_CopyPlane,
    XCB_PolyPoint,
    XCB_PolyLine,
    XCB_PolySegment,
    XCB_PolyRectangle,
    XCB_PolyArc,
    XCB_FillPoly,
    XCB_PolyFillRectangle,
    XCB_PolyFillArc,
    XCB_PutImage,
    XCB_GetImage,
    XCB_PolyText8,
    XCB_PolyText16,
    XCB_ImageText8,
    XCB_ImageText16,
    XCB_CreateColormap,
    XCB_FreeColormap,
    XCB_CopyColormapAndFree,
    XCB_InstallColormap,
    XCB_UninstallColormap,
    XCB_ListInstalledColormaps,
    XCB_AllocColor,
    XCB_AllocNamedColor,
    XCB_AllocColorCells,
    XCB_AllocColorPlanes,
    XCB_FreeColors,
    XCB_StoreColors,
    XCB_StoreNamedColor,
    XCB_QueryColors,
    XCB_LookupColor,
    XCB_CreateCursor,
    XCB_CreateGlyphCursor,
    XCB_FreeCursor,
    XCB_RecolorCursor,
    XCB_QueryBestSize,
    XCB_QueryExtension,
    XCB_ListExtensions,
    XCB_ChangeKeyboardMapping,
    XCB_GetKeyboardMapping,
    XCB_ChangeKeyboardControl,
    XCB_GetKeyboardControl,
    XCB_Bell,
    XCB_ChangePointerControl,
    XCB_GetPointerControl,
    XCB_SetScreenSaver,
    XCB_GetScreenSaver,
    XCB_ChangeHosts,
    XCB_ListHosts,
    XCB_SetAccessControl,
    XCB_SetCloseDownMode,
    XCB_KillClient,
    XCB_RotateProperties,
    XCB_ForceScreenSaver,
    XCB_SetPointerMapping,
    XCB_GetPointerMapping,
    XCB_SetModifierMapping,
    XCB_GetModifierMapping,




    XCB_NoOperation = 127
};


/*****************************************************************
 * WINDOW DEFINITIONS
 *****************************************************************/

/* Window classes used by CreateWindow */
/* Note that CopyFromParent is already defined as 0 above (good to have though.)*/
/* Window classes used by CreateWindow */
enum XCBWindowClass 
{
    XCBInputCopyFromParent = XCB_WINDOW_CLASS_COPY_FROM_PARENT,
    XCBInputOutput = XCB_WINDOW_CLASS_INPUT_OUTPUT,
    XCBInputOnly = XCB_WINDOW_CLASS_INPUT_ONLY,
};
/* Window attributes for CreateWindow and ChangeWindowAttributes */
enum XCBCreateWindowAttributesCW
{
    XCBCWBackPixmap         = XCB_CW_BACK_PIXMAP,
    XCBCWBackPixel          = XCB_CW_BACK_PIXEL,
    XCBCWBorderPixmap       = XCB_CW_BORDER_PIXMAP,
    XCBCWBorderPixel        = XCB_CW_BORDER_PIXEL,
    XCBCWBitGravity         = XCB_CW_BIT_GRAVITY,
    XCBCWWinGravity         = XCB_CW_WIN_GRAVITY,
    XCBCWBackingStore       = XCB_CW_BACKING_STORE,
    XCBCWBackingPlanes      = XCB_CW_BACKING_PLANES,
    XCBCWBackingPixel       = XCB_CW_BACKING_PIXEL,
    XCBCWOverrideRedirect   = XCB_CW_OVERRIDE_REDIRECT,
    XCBCWSaveUnder          = XCB_CW_SAVE_UNDER,
    XCBCWEventMask          = XCB_CW_EVENT_MASK,
    XCBCWDontPropagate      = XCB_CW_DONT_PROPAGATE,
    XCBCWColormap           = XCB_CW_COLORMAP,
    XCBCWCursor             = XCB_CW_CURSOR
};

/* ConfigureWindow structure */
enum XCBConfigureWindowAttributesCW 
{
    XCBCWX               = XCB_CONFIG_WINDOW_X,
    XCBCWY               = XCB_CONFIG_WINDOW_Y,
    XCBCWWidth           = XCB_CONFIG_WINDOW_WIDTH,
    XCBCWHeight          = XCB_CONFIG_WINDOW_HEIGHT,
    XCBCWBorderWidth     = XCB_CONFIG_WINDOW_BORDER_WIDTH,
    XCBCWSibling         = XCB_CONFIG_WINDOW_SIBLING,
    XCBCWStackMode       = XCB_CONFIG_WINDOW_STACK_MODE
};
/* Bit Gravity */
enum XCBBitGravity 
{
    XCBForgetGravity     = XCB_GRAVITY_BIT_FORGET,
    /* Window gravity + bit gravity above (no)*/
    XCBUnmapGravity      = XCB_GRAVITY_WIN_UNMAP,
    XCBNorthWestGravity  = XCB_GRAVITY_NORTH_WEST,
    XCBNorthGravity      = XCB_GRAVITY_NORTH,
    XCBNorthEastGravity  = XCB_GRAVITY_NORTH_EAST,
    XCBWestGravity       = XCB_GRAVITY_WEST,
    XCBCenterGravity     = XCB_GRAVITY_CENTER,
    XCBEastGravity       = XCB_GRAVITY_EAST,
    XCBSouthWestGravity  = XCB_GRAVITY_SOUTH_WEST,
    XCBSouthGravity      = XCB_GRAVITY_SOUTH,
    XCBSouthEastGravity  = XCB_GRAVITY_SOUTH_EAST,
    XCBStaticGravity     = XCB_GRAVITY_STATIC,
};

/* Used in CreateWindow for backing-store hint */
enum XCBBackingStoreHint 
{
    XCBNotUseful            = XCB_BACKING_STORE_NOT_USEFUL,
    XCBWhenMapped           = XCB_BACKING_STORE_WHEN_MAPPED,
    XCBAlways               = XCB_BACKING_STORE_ALWAYS
};

/* Used in GetWindowAttributes reply */
enum XCBMapState 
{
    XCBIsUnmapped           = XCB_MAP_STATE_UNMAPPED,
    XCBIsUnviewable         = XCB_MAP_STATE_UNVIEWABLE,
    XCBIsViewable           = XCB_MAP_STATE_VIEWABLE
};
/* Used in ChangeSaveSet */
enum XCBSetMode 
{
    XCBSetModeInsert        = XCB_SET_MODE_INSERT,
    XCBSetModeDelete        = XCB_SET_MODE_DELETE
};
/* Used in ChangeCloseDownMode */
enum XCBCloseDownMode 
{
    XCBDestroyAll           = XCB_CLOSE_DOWN_DESTROY_ALL,
    XCBRetainPermanent      = XCB_CLOSE_DOWN_RETAIN_PERMANENT,
    XCBRetainTemporary      = XCB_CLOSE_DOWN_RETAIN_TEMPORARY
};
/* Window stacking method (in configureWindow) */
enum XCBStackingMethod 
{
    XCBAbove                = XCB_STACK_MODE_ABOVE,
    XCBBelow                = XCB_STACK_MODE_BELOW,
    XCBTopIf                = XCB_STACK_MODE_TOP_IF,
    XCBBottomIf             = XCB_STACK_MODE_BOTTOM_IF,
    XCBOpposite             = XCB_STACK_MODE_OPPOSITE
};
/* Circulation direction */
enum XCBCirculationDirection 
{
    XCBRaiseLowest          = XCB_CIRCULATE_RAISE_LOWEST,
    XCBLowerHighest         = XCB_CIRCULATE_LOWER_HIGHEST
};
/* Property modes */
enum XCBPropertyMode 
{
    XCBPropModeReplace      = XCB_PROP_MODE_REPLACE,
    XCBPropModePrepend      = XCB_PROP_MODE_PREPEND,
    XCBPropModeAppend       = XCB_PROP_MODE_APPEND
};
/*****************************************************************
 * GRAPHICS DEFINITIONS
 *****************************************************************/

/* graphics functions, as in GC.alu */
enum XCBGraphicsOperation 
{
    XCBGXclear              = XCB_GX_CLEAR,             /* 0 */
    XCBGXand                = XCB_GX_AND,               /* src AND dst */
    XCBGXandReverse         = XCB_GX_AND_REVERSE,       /* src AND NOT dst */
    XCBGXcopy               = XCB_GX_COPY,              /* src */
    XCBGXandInverted        = XCB_GX_AND_INVERTED,      /* NOT src AND dst */
    XCBGXnoop               = XCB_GX_NOOP,              /* dst */
    XCBGXxor                = XCB_GX_XOR,               /* src XOR dst */
    XCBGXor                 = XCB_GX_OR,                /* src OR dst */
    XCBGXnor                = XCB_GX_NOR,               /* NOT src AND NOT dst */
    XCBGXequiv              = XCB_GX_EQUIV,             /* NOT src XOR dst */
    XCBGXinvert             = XCB_GX_INVERT,            /* NOT dst */
    XCBGXorReverse          = XCB_GX_OR_REVERSE,        /* src OR NOT dst */
    XCBGXcopyInverted       = XCB_GX_COPY_INVERTED,     /* NOT src */
    XCBGXorInverted         = XCB_GX_OR_INVERTED,       /* NOT src OR dst */
    XCBGXnand               = XCB_GX_NAND,              /* NOT src OR NOT dst */
    XCBGXset			    = XCB_GX_SET,               /* 1 */
};

/* LineStyle */
enum XCBLineStyle 
{
    XCBLineSolid        = XCB_LINE_STYLE_SOLID,
    XCBLineOnOffDash    = XCB_LINE_STYLE_ON_OFF_DASH,
    XCBLineDoubleDash   = XCB_LINE_STYLE_DOUBLE_DASH
};
/* capStyle */
enum XCBCapStyle 
{
    XCBCapNotLast       = XCB_CAP_STYLE_NOT_LAST,
    XCBCapButt          = XCB_CAP_STYLE_BUTT,
    XCBCapRound         = XCB_CAP_STYLE_ROUND,
    XCBCapProjecting    = XCB_CAP_STYLE_PROJECTING
};
/* joinStyle */
enum XCBJoinStyle 
{
    XCBJoinMiter        = XCB_JOIN_STYLE_MITER,
    XCBJoinRound        = XCB_JOIN_STYLE_ROUND,
    XCBJoinBevel        = XCB_JOIN_STYLE_BEVEL
};
/* fillStyle */
enum XCBFillStyle 
{
    XCBFillSolid             = XCB_FILL_STYLE_SOLID,
    XCBFillTiled             = XCB_FILL_STYLE_TILED,
    XCBFillStippled          = XCB_FILL_STYLE_STIPPLED,
    XCBFillOpaqueStippled    = XCB_FILL_STYLE_OPAQUE_STIPPLED
};
/* fillRule */
enum XCBFillRule 
{
    XCBEvenOddRule    = XCB_FILL_RULE_EVEN_ODD,
    XCbWindingRule    = XCB_FILL_RULE_WINDING
};
/* subwindow mode */
enum XCBSubwindowMode 
{
    XCBClipByChildren     = XCB_SUBWINDOW_MODE_CLIP_BY_CHILDREN,
    XCBIncludeInferiors   = XCB_SUBWINDOW_MODE_INCLUDE_INFERIORS
};
/* SetClipRectangles ordering */
enum XCBClipOrdering 
{
    XCBUnsorted     = XCB_CLIP_ORDERING_UNSORTED,
    XCBYSorted      = XCB_CLIP_ORDERING_Y_SORTED,
    XCBYXSorted     = XCB_CLIP_ORDERING_YX_SORTED,
    XCBYXBanded     = XCB_CLIP_ORDERING_YX_BANDED
};
/* CoordinateMode for drawing routines */
enum XCBCoordMode 
{
    XCBCoordModeOrigin      = XCB_COORD_MODE_ORIGIN,    /* relative to the origin */
    XCBCoordModePrevious    = XCB_COORD_MODE_PREVIOUS   /* relative to previous point */
};

/* Polygon shapes */
enum XCBPolyShape 
{
    XCBComplex      = XCB_POLY_SHAPE_COMPLEX,   /* paths may intersect */
    XCBNonconvex    = XCB_POLY_SHAPE_NONCONVEX, /* no paths intersect, but not convex */
    XCBConvex       = XCB_POLY_SHAPE_CONVEX,    /* wholly convex */
};

/* Arc modes for PolyFillArc */
enum XCBArcMode 
{
    XCBArcChord     = XCB_ARC_MODE_CHORD,       /* join endpoints of arc */
    XCBArcPieSlice  = XCB_ARC_MODE_PIE_SLICE    /* join endpoints to center of arc */
};

/* GC components: masks used in CreateGC, CopyGC, ChangeGC, OR'ed into
   GC.stateChanges */

enum XCBGCComponent 
{
    XCBGCFunction           = XCB_GC_FUNCTION,
    XCBGCPlaneMask          = XCB_GC_PLANE_MASK,
    XCBGCForeground         = XCB_GC_FOREGROUND,
    XCBGCBackground         = XCB_GC_BACKGROUND,
    XCBGCLineWidth          = XCB_GC_LINE_WIDTH,
    XCBGCLineStyle          = XCB_GC_LINE_STYLE,
    XCBGCCapStyle           = XCB_GC_CAP_STYLE,
    XCBGCJoinStyle          = XCB_GC_JOIN_STYLE,
    XCBGCFillStyle          = XCB_GC_FILL_STYLE,
    XCBGCFillRule           = XCB_GC_FILL_RULE,
    XCBGCTile               = XCB_GC_TILE,
    XCBGCStipple            = XCB_GC_STIPPLE,
    XCBGCTileStipXOrigin    = XCB_GC_TILE_STIPPLE_ORIGIN_X,
    XCBGCTileStipYOrigin    = XCB_GC_TILE_STIPPLE_ORIGIN_Y,
    XCBGCFont               = XCB_GC_FONT,
    XCBGCSubwindowMode      = XCB_GC_SUBWINDOW_MODE,
    XCBGCGraphicsExposures  = XCB_GC_GRAPHICS_EXPOSURES,
    XCBGCClipXOrigin        = XCB_GC_CLIP_ORIGIN_X,
    XCBGCClipYOrigin        = XCB_GC_CLIP_ORIGIN_Y,
    XCBGCClipMask           = XCB_GC_CLIP_MASK,
    XCBGCDashOffset         = XCB_GC_DASH_OFFSET,
    XCBGCDashList           = XCB_GC_DASH_LIST,
    XCBGCArcMode            = XCB_GC_ARC_MODE,
                            /* Refers quite literrally to the last bit so   1000 0000 0000 0000
                             *                                              ^
                             * like that last bit, for now it does nothing
                             */
    XCBGCLastBit            = XCB_GC_ARC_MODE << 1L,
};
/*****************************************************************
 * FONTS
 *****************************************************************/

/* used in QueryFont -- draw direction */

enum XCBFontDraw 
{
    XCBFontLeftToRight = XCB_FONT_DRAW_LEFT_TO_RIGHT,
    XCBFontRightToLeft = XCB_FONT_DRAW_RIGHT_TO_LEFT,
};

/*****************************************************************
 *  IMAGING
 *****************************************************************/

/* ImageFormat -- PutImage, GetImage */
enum XCBImageFormat 
{
    XCBXYBitmap             = XCB_IMAGE_FORMAT_XY_BITMAP,       /* depth 1, XYFormat */
    XCBXYPixmap             = XCB_IMAGE_FORMAT_XY_PIXMAP,       /* depth == drawable depth */
    XCBZPixmap              = XCB_IMAGE_FORMAT_Z_PIXMAP,        /* depth == drawable depth */
};

/*****************************************************************
 *  COLOR MAP STUFF
 *****************************************************************/

/* For CreateColormap */
enum XCBColormapAlloc 
{
    XCBAllocNone = XCB_COLORMAP_ALLOC_NONE,     /* create map with no entries */
    XCBAllocAll = XCB_COLORMAP_ALLOC_ALL,       /* allocate entire map writeable */
};

/* Flags used in StoreNamedColor, StoreColors */

enum XCBColorFlags 
{
    XCBDoRed = XCB_COLOR_FLAG_RED,
    XCBDoGreen = XCB_COLOR_FLAG_GREEN,
    XCBDoBlue = XCB_COLOR_FLAG_BLUE,
};

/*****************************************************************
 * CURSOR STUFF
 *****************************************************************/

/* QueryBestSize Class */
enum XCBCursorQueryShape 
{
    XCBCursorShape   = XCB_QUERY_SHAPE_OF_LARGEST_CURSOR,   /* Largest size that can be displayed */
    XCBTileShape     = XCB_QUERY_SHAPE_OF_FASTEST_TILE,     /* Size tiled fastest */
    XCBStippleShape  = XCB_QUERY_SHAPE_OF_FASTEST_STIPPLE   /* Size stippled fastest */
};
/*****************************************************************
 * KEYBOARD/POINTER STUFF
 *****************************************************************/

enum XCBAutoRepeatMode 
{
    XCBAutoRepeatModeOff     = XCB_AUTO_REPEAT_MODE_OFF,     /* Auto repeat off */
    XCBAutoRepeatModeOn      = XCB_AUTO_REPEAT_MODE_ON,      /* Auto repeat on */
    XCBAutoRepeatModeDefault = XCB_AUTO_REPEAT_MODE_DEFAULT  /* Default auto repeat mode */
};

enum XCBLedMode 
{
    XCBLedModeOff = XCB_LED_MODE_OFF,   /* Led mode off */
    XCBLedModeOn  = XCB_LED_MODE_ON     /* Led mode on */
};
/* masks for ChangeKeyboardControl */
enum XCBKeyboardControl 
{
    XCBKBKeyClickPercent = XCB_KB_KEY_CLICK_PERCENT,
    XCBKBBellPercent = XCB_KB_BELL_PERCENT,
    XCBKBBellPitch = XCB_KB_BELL_PITCH,
    XCBKBBellDuration = XCB_KB_BELL_DURATION,
    XCBKBLed = XCB_KB_LED,
    XCBKBLedMode = XCB_KB_LED_MODE,
    XCBKBKey = XCB_KB_KEY,
    XCBKBAutoRepeatMode = XCB_KB_AUTO_REPEAT_MODE
};

enum XCBMappingStatus 
{
    XCBMappingSuccess = XCB_MAPPING_STATUS_SUCCESS,
    XCBMappingBusy = XCB_MAPPING_STATUS_BUSY,
    XCBMappingFailed = XCB_MAPPING_STATUS_FAILURE
};

enum XCBMapping 
{
    XCBMappingModifier = XCB_MAPPING_MODIFIER,
    XCBMappingKeyboard = XCB_MAPPING_KEYBOARD,
    XCBMappingPointer = XCB_MAPPING_POINTER
};

/*****************************************************************
 * SCREEN SAVER STUFF
 *****************************************************************/

enum XCBBlanking 
{
    XCBDontPreferBlanking = XCB_BLANKING_NOT_PREFERRED,
    XCBPreferBlanking = XCB_BLANKING_PREFERRED,
    XCBDefaultBlanking = XCB_BLANKING_DEFAULT
};

enum
{
    /* Not sure how to directly translate this but this will probably do FIXME */
    XCBDisableScreenInterval = 0,	
};

enum XCBExposures
{
    XCBDontAllowExposures = XCB_EXPOSURES_NOT_ALLOWED,
    XCBAllowExposures = XCB_EXPOSURES_ALLOWED,
    XCBDefaultExposures = XCB_EXPOSURES_DEFAULT
};
/* for ForceScreenSaver */

enum XCBScreenSaverMode
{
    XCBScreenSaverReset = XCB_SCREEN_SAVER_RESET,
    /* Not sure how to directly translate this but this will probably do FIXME */
    XCBDisableScreenSaver = ((!XCB_SCREEN_SAVER_ACTIVE)),
    XCBScreenSaverActive = XCB_SCREEN_SAVER_ACTIVE
};

/*****************************************************************
 * HOSTS AND CONNECTIONS
 *****************************************************************/

/* for ChangeHosts */

enum XCBChangeHosts 
{
    XCBHostInsert = XCB_HOST_MODE_INSERT,
    XCBHostDelete = XCB_HOST_MODE_DELETE
};
/* for ChangeAccessControl */
enum XCBChangeAccessControl 
{
    XCBEnableAccess = XCB_ACCESS_CONTROL_ENABLE,
    XCBDisableAccess = XCB_ACCESS_CONTROL_DISABLE
};
/* Display classes  used in opening the connection
 * Note that the statically allocated ones are even numbered and the
 * dynamically changeable ones are odd numbered 
 */
enum XCBDisplayClasses 
{
    XCBStaticGray = XCB_VISUAL_CLASS_STATIC_GRAY,
    XCBGrayScale = XCB_VISUAL_CLASS_GRAY_SCALE,
    XCBStaticColor = XCB_VISUAL_CLASS_STATIC_COLOR,
    XCBPseudoColor = XCB_VISUAL_CLASS_PSEUDO_COLOR,
    XCBTrueColor = XCB_VISUAL_CLASS_TRUE_COLOR,
    XCBDirectColor = XCB_VISUAL_CLASS_DIRECT_COLOR
};

/* Byte order  used in imageByteOrder and bitmapBitOrder */
enum XCBByteOrder 
{
    XCBLSBFirst = XCB_IMAGE_ORDER_LSB_FIRST,
    XCBMSBFirst = XCB_IMAGE_ORDER_MSB_FIRST
};



#endif
