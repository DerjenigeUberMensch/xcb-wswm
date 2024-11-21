#ifndef __SESSION__H__
#define __SESSION__H__


#include <stddef.h>

#include "XCB-TRL/xcb_trl.h"
#include "settings.h"


typedef struct SessionSettings SessionSettings;
typedef struct SessionMonitorSave SessionMonitorSave;
typedef struct SessionDesktopSave SessionDesktopSave;
typedef struct SessionClientSave SessionClientSave;

struct
SessionSettings
{
    SCParser *cfg;
};

struct
SessionMonitorSave
{
    int16_t mx;
    int16_t my;
    int16_t mw;
    int16_t mh;
    int16_t wx;
    int16_t wy;
    int16_t ww;
    int16_t wh;
    uint16_t deskselnum;
    uint16_t deskcount;
};

struct
SessionDesktopSave
{
    int16_t desknum;
    uint8_t layout;
    uint8_t oldlayout;
};

struct
SessionClientSave
{
    int16_t oldx;
    int16_t oldy;

    int16_t oldw;
    int16_t oldh;

    uint16_t bw;
    uint16_t oldbw;

    XCBWindow win;

    XCBWindow next;
    XCBWindow prev;

    XCBWindow snext;
    XCBWindow sprev;

    XCBWindow rnext;
    XCBWindow rprev;

    XCBWindow fnext;
    XCBWindow fprev;
};

void SessionSave(
        void
        );



#endif
