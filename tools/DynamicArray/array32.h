#ifndef __ARRAY__32__H__
#define __ARRAY__32__H__

#include "dynamic_array.h"

typedef struct __GArray__ GArray32;


GArray32 *
GArrayCreate32(
    uint32_t base_allocate
    );

int
GArrayCreateFilled32(
    GArray32 *array_return,
    uint32_t base_allocate
    );

void
GArrayWipe32(
    GArray32 *array
    );

int
GArrayResize32(
    GArray32 *array,
    uint32_t item_len
    );

int
GArrayPushBack32(
    GArray32 *array,
    void *item_cpy
    );
int
GArrayPopBack32(
    GArray32 *array
    );

int
GArrayReplace32(
    GArray32 *array,
    void *item_cpy,
    uint32_t index
    );

int
GArrayInsert32(
    GArray32 *array,
    void *item_cpy,
    uint32_t index
    );

int
GArrayDelete32(
    GArray32 *array,
    uint32_t index
    );


void *
GArrayAt32(
        GArray32 *array,
        uint32_t index
        );

int
GArrayAtSafe32(
        GArray32 *array,
        uint32_t index,
        void *fill_return
        );

uint32_t
GArrayEnd32(
        GArray32 *array
        );

uint32_t 
GArrayStart32(
        GArray32 *array
        );


#endif
