#ifndef __XCB__TRL__KEYSYM__H__
#define __XCB__TRL__KEYSYM__H__


#include "xcb_trl_types.h"


typedef struct _XCBKeySymbolsFastTable XCBKeySymbolsFastTable;

enum
XCBKeysymColumn
{
    XCBKeysymColUnmodified = 0,
    XCBKeysymColShiftModifier = 1,
    XCBKeysymColCapsLockModifier = 2,
    XCBKeysymColControlModifier = 3,
    XCBKeysymColLeftAltModifier = 4,
    XCBKeysymColLeftAltGrModifier = 5,
};


XCBKeySymbolsFastTable *
XCBKeySymbolsFastTableCreate(  
        XCBDisplay *display,
        XCBKeySymbols *symbols
        );





/* This gets the keycodes of the specfied display.
 *
 * NOTE: This operation is relativly expensive so it is recommended to not use this function;
 *       Instead it is recommended to use XCBKeySymbolsAlloc() and search them yourself.
 *
 * NOTE: RETURN MUST BE RELEASED BY CALLER USING free().
 */
XCBKeycode *
XCBGetKeycodes(
        XCBDisplay *display, 
        XCBKeysym keysym
        );

/* This gets the keycodes of the specfied display.
 *
 * NOTE: This operation is relativly expensive so it is recommended to not use this function.
 *       Instead it is recommended to use XCBKeySymbolsAlloc() and search them using XCBKeySymbolsGetKeyCode.
 *
 * NOTE: RETURN MUST BE RELEASED BY CALLER USING free().
 */
XCBKeyCode *
XCBGetKeyCodes(
        XCBDisplay *display, 
        XCBKeysym keysym
        );

/*
 */
XCBKeyCode *
XCBKeySymbolsGetKeyCode(
        XCBKeySymbols *symbols, 
        XCBKeysym keysym
        );

/*
 */
XCBKeycode *
XCBKeySymbolsGetKeycode(
        XCBKeySymbols *symbols,
        XCBKeysym keysym
        );

/*
 * column:          enum XCBKeysymColumn        
 *
 */
XCBKeysym
XCBKeySymbolsGetKeySym(
        XCBKeySymbols *symbols,
        XCBKeyCode keycode,
        enum XCBKeysymColumn column
        );

/*
 */
XCBKeySymbols *
XCBKeySymbolsAlloc(
        XCBDisplay *display
        );

/*
 */
void
XCBKeySymbolsFree(
        XCBKeySymbols *keysyms
        );




































#endif
