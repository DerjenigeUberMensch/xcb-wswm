/* MIT License
 *
 * Copyright (c) 2024 Joseph
 * All rights reserved.
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

#ifndef __DY__NAMIC__ARRAY__H__
#define __DY__NAMIC__ARRAY__H__

#include <stdint.h>

#ifdef __G_ARRAY_H__
#error "Using Glib for dynamic array, delete this line to acknowledge this"
#else
#define __G_ARRAY_H__
#endif

/* G stands for generic by the way.
 */
typedef struct GArray GArray;
typedef struct __GArray__ __GArray__;
/* UNUSED cause why would you use this? 
 * Encapsulation is useless at this low level. 
 * Even had said that I still use encapsulation, and half the time I only do it for sustainability of the project. 
 * Otherwise why would I?
 */
typedef uint64_t garray_i;

struct
GArray
{
    void *data;
    uint32_t item_size;
    uint32_t data_len;
    uint32_t data_len_real;
    uint8_t pad0[4];
};

struct 
__GArray__
{
    void *data;
    uint32_t data_len;
    uint32_t data_len_real;
};

/*
 * RETURN: GArray * on Success
 * RETURN: NULL on Failure.
 */
GArray *
GArrayCreate(
    uint32_t item_size,
    uint32_t base_allocate
    );

/*
 * RETURN: EXIT_SUCCESS on Sucesss.
 * RETURN: EXIT_FAILURE on Failure.
 */
int
GArrayCreateFilled(
    GArray *array_return,
    uint32_t item_size,
    uint32_t base_allocate
    );

/*
 * RETURN: EXIT_SUCCESS on Sucesss.
 * RETURN: EXIT_FAILURE on Failure.
 */
void
GArrayWipe(
    GArray *array
    );

/*
 * RETURN: EXIT_SUCCESS on Sucesss.
 * RETURN: EXIT_FAILURE on Failure.
 */
int
GArrayResize(
    GArray *array,
    uint32_t item_len
    );

/*
 * RETURN: EXIT_SUCCESS on Sucesss.
 * RETURN: EXIT_FAILURE on Failure.
 */
int
GArrayPushBack(
    GArray *array,
    void *item_cpy
    );

/*
 * RETURN: EXIT_SUCCESS on Sucesss.
 * RETURN: EXIT_FAILURE on Failure.
 */
int
GArrayPopBack(
    GArray *array
    );

/*
 * RETURN: EXIT_SUCCESS on Sucesss.
 * RETURN: EXIT_FAILURE on Failure.
 */
int
GArrayReplace(
    GArray *array,
    void *item_cpy,
    uint32_t index
    );

/*
 * RETURN: EXIT_SUCCESS on Sucesss.
 * RETURN: EXIT_FAILURE on Failure.
 */
int
GArrayInsert(
    GArray *array,
    void *item_cpy,
    uint32_t index
    );

/*
 * RETURN: EXIT_SUCCESS on Sucesss.
 * RETURN: EXIT_FAILURE on Failure.
 */
int
GArrayDelete(
    GArray *array,
    uint32_t index
    );

void *
GArrayAt(
        GArray *array,
        uint32_t index
        );
int
GArrayAtSafe(
        GArray *array,
        uint32_t index,
        void *fill_return
        );

uint32_t
GArrayEnd(
        GArray *array
        );

uint32_t 
GArrayStart(
        GArray *array
        );

#endif
