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

#ifndef __XCBKEYS_H
#define __XCBKEYS_H

#include <xcb/xcb.h>

/**
 * @brief Returns a pointer to an array of all keysyms in a keyboard mapping
 *
 * @param R The mapping of a keyboard (xcb_get_keyboard_mapping_reply_t).
 *
 * @return xcb_keysym_t*.
 */
xcb_keysym_t *xcb_get_keysym_map(xcb_get_keyboard_mapping_reply_t *R);

/**
 * @brief Calculates the index number of the first keysym in a keysym array that matches with K.
 * @brief 2nd/3rd/4th/... keysyms follow directly after the first until keysyms_per_keycode is reached
 *
 * @param K The keycode to find the keysym of (xcb_keycode_t).
 * @param M The minimum keycode value (xcb_keycode_t).
 * @param P The number of keysyms per keycode (uint8_t).
 *
 * @return unsigned int
*/
uint32_t xcb_keycode_to_keysym_index(xcb_keycode_t K, xcb_keycode_t M, xcb_keycode_t P);

/**
 * @brief Calculates a keycode based on the index of a keysym in a keysym array.
 *
 * @param I The index of the keysym to find the keycode of (unsigned int).
 * @param P The number of keysyms per keycode (uint8_t).
 * @param M The minimum keycode value (xcb_keycode_t).
 *
 * @return xcb_keycode_t
 */
xcb_keycode_t xcb_keysym_index_to_keycode(uint32_t I, uint8_t M, xcb_keycode_t P);

/**
 * @brief Returns the first keysym found that corresponds to the specified keycode.
 *
 * @param setup The setup information about the x server. See xcb_get_setup().
 * @param mapping The mapping of the keyboard. See xcb_get_keyboard_mapping().
 * @param keycode The keycode to find the keysym of.
 *
 * @return =0 keycode is invalid or it has no corresponding keysym.
 * @return !=0 The keysym has been returned successfully.
 */
xcb_keysym_t xcb_keycode_to_keysym(const xcb_setup_t *setup, xcb_get_keyboard_mapping_reply_t *mapping, xcb_keycode_t keycode);
/**
 * @brief Returns an array of keysyms which correspond to the specified keycode.
 * @brief Any array values outside of mapping->keysyms_per_keycode should be considered invalid.
 *
 * @param setup The setup information about the x server. See xcb_get_setup().
 * @param mapping The mapping of the keyboard. See xcb_get_keyboard_mapping().
 * @param index The keycode to find the keysyms of.
 *
 * @return =0 The keycode specified is invalid or it has no corresponding keysyms.
 * @return !=0 The array of keysyms were successfully returned.
 */
xcb_keysym_t *xcb_keycode_to_keysyms(const xcb_setup_t *setup, xcb_get_keyboard_mapping_reply_t *mapping, xcb_keycode_t keycode);
/**
 * @brief Returns the keycode that corresponds to the specified keysym.
 *
 * @param setup The setup information about the x server. See xcb_get_setup().
 * @param mapping The mapping of the keyboard. See xcb_get_keyboard_mapping().
 * @param keysym The keysym to find the keycode of.
 *
 * @return =0 keysym has no corresponding keysym
 * @return !=0 The keycode has been returned successfully.
 */
xcb_keycode_t xcb_keysym_to_keycode(const xcb_setup_t *setup, xcb_get_keyboard_mapping_reply_t *mapping, xcb_keysym_t keysym);

#endif /*__XCBKEYS_H */


/* Docs */
/*
# About

What is xcbkeys:
xcbkeys is a small library written on top of xcb that attempts to solve the problem of not having a standard and simple way
in xcb to convert keycodes to keysyms and vice versa. The functions and macros are designed to run optimally with
the least amount of code possible.

xcbkeys license:
xcbkeys is under the MIT license, more information can be found in xcbkeys.h.

How to use xcbkeys:
xcbkeys can be easily stored in your installation of xcb or in your project directory and #included into your source code.
All functions are compatible with X11/keysym.h.

Contact Information:
For any questions or feedback my email is archerpergande@gmail.com.


# Documentation

xcb_get_keysym_map:
This macro is functionally equivalent to the function xcb_get_keyboard_mapping_keysyms(). Because the function is small and does one 
operation with no error checking, it makes more sense for it to become a macro. It returns a pointer to an array of all keysyms on the 
system in an order corresponding with keycodes, so the first keysym in the array equals the first keycode (or minimum keycode). 
Be careful, as each keycode can have multiple keysyms and skews the ordering of the keysyms array.

xcb_keycode_to_keysym_index:
This macro calculates the index of a specific keysym in the keysym array gathered from xcb_get_keysym_map using an equation. The macro 
takes three parameters K, M, and P. K represents a keycode that the caller wants to find the corresponding keysym of, M represents the 
minimum keycode value, and P represents the number of keysyms per keycode. The first part of the equation (K - M) subtracts the keycode 
value the user wants to find the keysym of from the minimum keycode value. Because the keysym array will always start at index 0, and the 
first keycode can vary on where it starts but usually is not 0 there can be conflicts. For example lets say the starting keycode is 8, and 
a user wants to find its corresponding keysym. Because the keysyms array aligns with the keycodes, the user should be able to plug 8 into 
the keysyms array index. However, because the array starts at 0 and not 8, the user would get the 8th keysym and not the first. If the user 
instead subtracted the minimum keycode value (which is the same as the starting value) the user would get the equation 8-8=0, and using 0 
as the index would get me the 1st keysym. The second part of the equation * P multiplies the final value of K - M by the number of keysyms 
per keycode. Because there can be multiple keysyms per each keycode, the ratio of keycodes to keysym changes from 1:1 to 1:many. Multiplying 
accounts for that ratio change. A downside to this equation is that it can not find the index of any keysyms other than the first, meaning 
that if a user wanted to locate the secondary keysym that corresponds with a keycode, the user would have to add one to the resulting value 
of xcb_keycode_to_keysym_index.

xcb_keysym_index_to_keycode:
This maco is the opposite of xcb_keycode_to_keysym_index. It takes 3 parameters I, P, and M. I represents the index of a keysym that was 
calculated using xcb_keycode_to_keysym_index, P represents the number of keysysm per keycode, and M represents the minimum keycode value. More 
info on the math can be found in "math of xcb_keysym_index_to_keycode".

math of xcb_keysym_index_to_keycode:
starting equation (aka xcb_keycode_to_keysym_index) | I = (K - M) * P
divide by P on both sides | I/P = K - M
add M to both sides | (I/P)+M = K

xcb_keycode_to_keysym:
This function converts a keycode into its corresponding keysym by combining both xcb_get_keysym_map and xcb_keycode_to_keysym_index into one. 
It first gets the array of all keysyms from xcb_get_keysym_map and then calculates the index of the keysym with xcb_keycode_to_keysym_index 
using the callers specified keycode value. The function also checks if the caller's keycode is within the range of minimum keycode and maximum 
keycode because calculating an index with a value outside that range would result in an index that would go outside of the keysyms array and 
cause undefined behavior. A downside to this function is that it will only return the first keysym.

xcb_keycode_to_keysyms:
This function is almost equivalent to xcb_keycode_to_keysym, but instead of returning a keycode value, it uses pointer arithmetic and adds the 
value of the keysym index to the keysyms mapping, and returns the new pointer. This is useful because it allows callers to get every 
corresponding keysym to a keycode rather than just the first. A downside to this function is that because the returned value is just a pointer 
and if the caller calls free() on the xcb_get_keyboard_mapping_reply_t structure, it will cause the return value of this function to be invalid. 
Another cause for concern is that because a pointer is returned, it essentially cuts the keysyms array into two where index 0 is now the keysyms 
that corresponds to the keycode the user specified, so once a user goes outside the range of keysyms_per_keycode that is a completely new keysym 
that corresponds to a completely different keycode.

xcb_keysym_to_keycode:
This function does the complete opposite of xcb_keycode_to_keysym it takes in a keysym value (any not just the first) and returns the corresponding 
keycode value. Because there is no mathematical easy way to accomplish this the function has to iterate through every keycode and test if its 
keysyms are equal to the callee specified one. This process can be resource intensive, so to help speed the process, part of the equation in 
xcb_keycode_to_keysym_index is split so all constant values in the equation are calculated before iterating through every keycode. More info on the 
math can be found in "math of xcb_keysym_to_keycode". There can also be multiple keysyms per each keycode to check, so a secondary loop iterates 
through every second, third, and fourth keysym until all are scanned. The function will always check every first keysym before moving to the second 
and will always check every second keysym before moving to the third.

math of xcb_keysym_to_keycode:
starting equation (aka xcb_keycode_to_keysym_index) | I = (K - M) * P
distribute P | I = K*P - M*P
M (minimum keycode) and P (keysyms per keycode) are both constant and therefore can be calculated before iterating through every K (keycode).
*/
