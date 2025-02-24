/*
 * MIT License.
 *
 * Copyright (c) 2023 Archer Pergande
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */




#include "xcbkeys.h"



xcb_keysym_t *xcb_get_keysym_map(xcb_get_keyboard_mapping_reply_t *R)                           { return (xcb_keysym_t *) (R + 1);              }
uint32_t xcb_keycode_to_keysym_index(xcb_keycode_t K, xcb_keycode_t M, xcb_keycode_t P)         { return (uint32_t) (K - M) * P;                }
xcb_keycode_t xcb_keysym_index_to_keycode(uint32_t I, uint8_t M, xcb_keycode_t P)               { return (const xcb_keycode_t) (I / P) + M;     }

xcb_keysym_t 
xcb_keycode_to_keysym(const xcb_setup_t *setup, xcb_get_keyboard_mapping_reply_t *mapping, xcb_keycode_t keycode) 
{
    xcb_keysym_t *syms = xcb_keycode_to_keysyms(setup, mapping, keycode);

    if(syms)
    {   return *syms;
    }

    return 0;
}

xcb_keysym_t *
xcb_keycode_to_keysyms(const xcb_setup_t *setup, xcb_get_keyboard_mapping_reply_t *mapping, xcb_keycode_t keycode)
{
    if (keycode < setup->min_keycode || keycode > setup->max_keycode)
    {   return (xcb_keysym_t *)0;
    }

    xcb_keysym_t *syms = xcb_get_keysym_map(mapping);
    uint32_t index = xcb_keycode_to_keysym_index(keycode, setup->min_keycode, mapping->keysyms_per_keycode);

    return syms + index;
}

xcb_keycode_t 
xcb_keysym_to_keycode(const xcb_setup_t *setup, xcb_get_keyboard_mapping_reply_t *mapping, xcb_keysym_t keysym) 
{
    const xcb_keysym_t *keysym_map = xcb_get_keysym_map(mapping);
    const uint32_t precalc = setup->min_keycode * mapping->keysyms_per_keycode; /* pre-calculating part of xcb_keycode_to_keysym_index() equation to make the for loop faster */
    uint8_t shift;
    uint8_t keycode;
    for(shift = 0; shift < mapping->keysyms_per_keycode; ++shift)
    {
        for(keycode = setup->min_keycode; keycode < setup->max_keycode; ++keycode)
        {
            if(keysym_map[keycode * mapping->keysyms_per_keycode - precalc + shift] == keysym)
            {   return keycode;
            }
        }
    }
    return 0;
}
