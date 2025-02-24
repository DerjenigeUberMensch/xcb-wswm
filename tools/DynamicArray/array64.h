#ifndef __ARRAY__64__H__
#define __ARRAY__64__H__

#include "dynamic_array.h"

typedef struct __GArray__ GArray64;


GArray64 *
GArrayCreate64(
    uint32_t base_allocate
    );

int
GArrayCreateFilled64(
    GArray64 *array_return,
    uint32_t base_allocate
    );

void
GArrayWipe64(
    GArray64 *array
    );

int
GArrayResize64(
    GArray64 *array,
    uint32_t item_len
    );

int
GArrayPushBack64(
    GArray64 *array,
    void *item_cpy
    );

int
GArrayPopBack64(
    GArray64 *array
    );

int
GArrayReplace64(
    GArray64 *array,
    void *item_cpy,
    uint32_t index
    );

int
GArrayInsert64(
    GArray64 *array,
    void *item_cpy,
    uint32_t index
    );

int
GArrayDelete64(
    GArray64 *array,
    uint32_t index
    );


void *
GArrayAt64(
        GArray64 *array,
        uint32_t index
        );

int
GArrayAtSafe64(
        GArray64 *array,
        uint32_t index,
        void *fill_return
        );

uint32_t
GArrayEnd64(
        GArray64 *array
        );

uint32_t 
GArrayStart64(
        GArray64 *array
        );


#endif
