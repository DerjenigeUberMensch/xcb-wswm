#ifndef __ARRAY__16__H__
#define __ARRAY__16__H__


#include "dynamic_array.h"

typedef struct __GArray__ GArray16;


GArray16 *
GArrayCreate16(
    uint32_t base_allocate
    );

int
GArrayCreateFilled16(
    GArray16 *array_return,
    uint32_t base_allocate
    );

void
GArrayWipe16(
    GArray16 *array
    );

int
GArrayResize16(
    GArray16 *array,
    uint32_t item_len
    );

int
GArrayPushBack16(
    GArray16 *array,
    void *item_cpy
    );

int
GArrayPopBack16(
    GArray16 *array
    );

int
GArrayReplace16(
    GArray16 *array,
    void *item_cpy,
    uint32_t index
    );

int
GArrayInsert16(
    GArray16 *array,
    void *item_cpy,
    uint32_t index
    );

int
GArrayDelete16(
    GArray16 *array,
    uint32_t index
    );


void *
GArrayAt16(
        GArray16 *array,
        uint32_t index
        );

int
GArrayAtSafe16(
        GArray16 *array,
        uint32_t index,
        void *fill_return
        );

uint32_t
GArrayEnd16(
        GArray16 *array
        );

uint32_t 
GArrayStart16(
        GArray16 *array
        );


#endif
