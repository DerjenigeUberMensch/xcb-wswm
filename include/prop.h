#ifndef _PROP_HANDLING_H_
#define _PROP_HANDLING_H_


#include "XCB-TRL/xcb_trl.h"
#include "util.h"

enum PropertyType
{
    PropNone,
    PropTransient,
    PropWindowState,
    PropWindowType,
    PropSizeHints,
    PropWMHints,
    PropWMClass,
    PropWMProtocol,
    PropStrut,
    PropStrutp,

    PropNetWMName,
    PropWMName,
    PropPid,
    PropIcon,
    PropMotifHints,
    PropManage,
    PropUnmanage,

    /* Net State Setters */
    PropSetWtype,
    PropUnsetWtype,
    PropSetWState,
    PropUnsetWState,

    /* special */
    PropExitThread,
    PropLAST,
};

typedef union PropArg PropArg;
typedef struct GetPropCookie GetPropCookie;

union PropArg
{
    void *v;
    void **vv;

    /* char */
    int8_t c[8];
    uint8_t uc[8];

    /* short */
    int16_t s[4];
    uint16_t us[4];

    /* int */
    int32_t i[2];
    uint32_t ui[2];

    /* long */
    int64_t l[1];
    uint64_t ul[1];
};

struct  
GetPropCookie
{
    XCBWindow win;
    enum PropertyType type;
    PropArg arg;
    XCBCookie cookie;
    uint8_t pad0[4];
};



XCBCookie NonNull
PropGetInvalidCookie(
        XCBDisplay *display, 
        XCBWindow win
        );

XCBCookie NonNull
PropGetTransientCookie(
        XCBDisplay *display, 
        XCBWindow window
        );

XCBCookie NonNull
PropGetWindowStateCookie(
        XCBDisplay *display, 
        XCBWindow window
        );

XCBCookie NonNull
PropGetWindowTypeCookie(
        XCBDisplay *display, 
        XCBWindow window
        );

XCBCookie NonNull
PropGetSizeHintsCookie(
        XCBDisplay *display, 
        XCBWindow window
        );

XCBCookie NonNull
PropGetWMHintsCookie(
        XCBDisplay *display, 
        XCBWindow window
        );

XCBCookie NonNull
PropGetWMClassCookie(
        XCBDisplay *display, 
        XCBWindow window
        );

XCBCookie NonNull
PropGetWMProtocolCookie(
        XCBDisplay *display, 
        XCBWindow window
        );

XCBCookie NonNull
PropGetStrutCookie(
        XCBDisplay *display, 
        XCBWindow window
        );

XCBCookie NonNull
PropGetStrutpCookie(
        XCBDisplay *display, 
        XCBWindow window
        );

XCBCookie NonNull
PropGetNetWMNameCookie(
        XCBDisplay *display, 
        XCBWindow window
        );

XCBCookie NonNull
PropGetWMNameCookie(
        XCBDisplay *display, 
        XCBWindow window
        );

XCBCookie NonNull
PropGetPidCookie(
        XCBDisplay *display, 
        XCBWindow window
        );

XCBCookie NonNull
PropGetIconCookie(
        XCBDisplay *display, 
        XCBWindow window
        );

XCBCookie NonNull
PropGetMotifHintsCookie(
        XCBDisplay *display, 
        XCBWindow window
        );

void NonNullAll
PropUpdateInvalid(
        XCBDisplay *display, 
        GetPropCookie *cookie
        );

void NonNullAll
PropUpdateTrans(
        XCBDisplay *display, 
        GetPropCookie *cookie
        );

void NonNullAll
PropUpdateWindowState(
        XCBDisplay *display, 
        GetPropCookie *cookie
        );

void NonNullAll
PropUpdateWindowType(
        XCBDisplay *display, 
        GetPropCookie *cookie
        );

void NonNullAll
PropUpdateSizeHints(
        XCBDisplay *display, 
        GetPropCookie *cookie
        );

void NonNullAll
PropUpdateWMHints(
        XCBDisplay *display, 
        GetPropCookie *cookie
        );

void NonNullAll
PropUpdateWMClass(
        XCBDisplay *display, 
        GetPropCookie *cookie
        );

void NonNullAll
PropUpdateWMProtocol(
        XCBDisplay *display, 
        GetPropCookie *cookie
        );

void NonNullAll
PropUpdateStrut(
        XCBDisplay *display, 
        GetPropCookie *cookie
        );

void NonNullAll
PropUpdateStrutP(
        XCBDisplay *display, 
        GetPropCookie *cookie
        );

void NonNullAll
PropUpdateNetWMName(
        XCBDisplay *display, 
        GetPropCookie *cookie
        );

void NonNullAll
PropUpdateWMName(
        XCBDisplay *display, 
        GetPropCookie *cookie
        );

void NonNullAll
PropUpdatePid(
        XCBDisplay *display, 
        GetPropCookie *cookie
        );

void NonNullAll
PropUpdateIcon(
        XCBDisplay *display, 
        GetPropCookie *cookie
        );

void NonNullAll
PropUpdateMotifHints(
       XCBDisplay *display, 
       GetPropCookie *cookie
       );

void NonNullAll
PropUpdateManage(
        XCBDisplay *display, 
        GetPropCookie *cookie
        );

void NonNullAll
PropUpdateUnmanage(
        XCBDisplay *display, 
        GetPropCookie *cookie
        );

void NonNullAll
PropUpdateSetWType(
        XCBDisplay *display, 
        GetPropCookie *cookie
        );

void NonNullAll
PropUpdateUnsetWType(
        XCBDisplay *display, 
        GetPropCookie *cookie
        );

void NonNullAll
PropUpdateSetWState(
        XCBDisplay *display, 
        GetPropCookie *cookie
        );

void NonNullAll
PropUpdateUnsetWState(
        XCBDisplay *display, 
        GetPropCookie *cookie
        );


void NonNullAll
PropUpdateProperty(
        XCBDisplay *display,
        GetPropCookie *cookie
        );

#endif
