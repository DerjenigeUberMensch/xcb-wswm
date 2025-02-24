#include <stdlib.h>
#include <stdio.h>
#include <xcb/xcb.h>
#define XK_MISCELLANY
#define XK_XKB_KEYS
#define XK_LATIN1
#define XK_LATIN2
#define XK_LATIN3
#define XK_LATIN4
#define XK_CYRILLIC
#define XK_GREEK
#define XK_ARMENIAN
#include <X11/keysymdef.h>

#include "xcb_keysym.h"
#include "xcb_trl.h"
#include "khash.h"
#include "xcbkeys.h"


/* Private declaration */
enum tag_t 
{
    TAG_COOKIE,
    TAG_VALUE
};

typedef struct _XCBKeySymbols _XCBKeySymbols;

struct _XCBKeySymbols
{
    xcb_connection_t *c;
    enum tag_t     tag;
    union 
    {
        xcb_get_keyboard_mapping_cookie_t cookie;
        xcb_get_keyboard_mapping_reply_t *reply;
    } u;
};



KHASH_MAP_INIT_INT(__KEYSYM__TABLE__, XCBKeysym)
KHASH_MAP_INIT_INT(__KEYCODE__TABLE__, XCBKeyCode *)

struct
_XCBKeySymbolsFastTable 
{
    XCBKeySymbols *symbols;
    khash_t(__KEYSYM__TABLE__) *keysym_lookup;
    khash_t(__KEYCODE__TABLE__) *keycode_lookup;
};

XCBKeySymbolsFastTable *
XCBKeySymbolsFastTableCreate(  
        XCBDisplay *display,
        XCBKeySymbols *symbols
        )
{
    if(xcb_connection_has_error(display) || xcb_connection_has_error(symbols->c))
    {   return NULL;
    }

    XCBKeySymbolsFastTable *table = malloc(sizeof(*table));

    if(!table)
    {   return NULL;
    }

    table->symbols = symbols;
    table->keysym_lookup = kh_init(__KEYSYM__TABLE__);
    table->keycode_lookup = kh_init(__KEYCODE__TABLE__);

    if(!table->keysym_lookup)
    {   
        free(table);
        return NULL;
    }

    if(!table->keycode_lookup)
    {
        free(table->keysym_lookup);
        free(table);
        return NULL;
    }


    xcb_keysym_t *syms = xcb_get_keyboard_mapping_keysyms(table->symbols->u.reply);
    const xcb_setup_t *setup = xcb_get_setup(table->symbols->c);

    uint8_t per = table->symbols->u.reply->keysyms_per_keycode;
    uint32_t length = xcb_get_keyboard_mapping_keysyms_length(table->symbols->u.reply);
    xcb_keycode_t min_keycode = setup->min_keycode;
    xcb_keycode_t max_keycode = setup->max_keycode;

    int64_t i;

    int64_t counter = 0;

    for(i = 0; i < length; ++i)
    {
    }

    //fprintf(stderr, "Empty: %lu\n", counter);


    return table;
}

XCBKeysym
XCBKeySymbolsFastTableLookupKeySym(
        XCBKeySymbolsFastTable *table,
        XCBKeyCode code
        )
{
    khint_t k = kh_get(__KEYSYM__TABLE__, table->keysym_lookup, code);

    if(k != kh_end(table->keysym_lookup))
    {   return kh_val(table->keysym_lookup, k);
    }

    return 0;
}

XCBKeyCode *
XCBKeySymbolsFastTableLookupKeyCodes(
        XCBKeySymbolsFastTable *table,
        XCBKeysym keysym
        )
{
    khint_t k = kh_get(__KEYCODE__TABLE__, table->keycode_lookup, keysym);

    if(k != kh_end(table->keycode_lookup))
    {   return kh_val(table->keycode_lookup, k);
    }

    return NULL;
}

void
XCBKeySymbolsFastTableDestroy(
        XCBKeySymbolsFastTable *table
        )
{
    if(!table)
    {   return;
    }
    kh_destroy(__KEYSYM__TABLE__, table->keysym_lookup);
    kh_destroy(__KEYCODE__TABLE__, table->keycode_lookup);
    free(table);
}



XCBKeyCode *
XCBGetKeyCodes(
        XCBDisplay *display, 
        XCBKeysym keysym
        )
{
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
    return XCBGetKeyCodes(display, keysym);
}


XCBKeyCode *
XCBKeySymbolsGetKeyCode(
        XCBKeySymbols *symbols, 
        XCBKeysym keysym
        )
{
    return xcb_key_symbols_get_keycode(symbols, keysym);
}


XCBKeycode *
XCBKeySymbolsGetKeycode(
        XCBKeySymbols *symbols,
        XCBKeysym keysym
        )
{
    return xcb_key_symbols_get_keycode(symbols, keysym);
}

XCBKeysym
XCBKeySymbolsGetKeySym(
        XCBKeySymbols *symbols,
        XCBKeyCode keycode,
        enum XCBKeysymColumn column
        )
{   
    return xcb_key_symbols_get_keysym(symbols, keycode, column);
}

XCBKeySymbols *
XCBKeySymbolsAlloc(
        XCBDisplay *display
        )
{
    return xcb_key_symbols_alloc(display);
}

void
XCBKeySymbolsFree(
        XCBKeySymbols *keysyms
        )
{
    xcb_key_symbols_free(keysyms);
}
