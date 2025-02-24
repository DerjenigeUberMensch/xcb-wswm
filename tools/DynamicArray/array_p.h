#ifndef __ARRAY__P__H__
#define __ARRAY__P__H__


#include "dynamic_array.h"

typedef struct __GArray__ GArrayP;


GArrayP *
GArrayCreateP(
    uint32_t base_allocate
    );

int
GArrayCreateFilledP(
    GArrayP *array_return,
    uint32_t base_allocate
    );

void
GArrayWipeP(
    GArrayP *array
    );

int
GArrayResizeP(
    GArrayP *array,
    uint32_t item_len
    );

int
GArrayPushBackP(
    GArrayP *array,
    void *item_cpy
    );

int
GArrayPopBackP(
    GArrayP *array
    );

int
GArrayReplaceP(
    GArrayP *array,
    void *item_cpy,
    uint32_t index
    );

int
GArrayInsertP(
    GArrayP *array,
    void *item_cpy,
    uint32_t index
    );

int
GArrayDeleteP(
    GArrayP *array,
    uint32_t index
    );


void *
GArrayAtP(
    GArrayP *array,
    uint32_t index
    );

int
GArrayAtSafeP(
    GArrayP *array,
    uint32_t index,
    void *fill_return
    );

uint32_t
GArrayEndP(
    GArrayP *array
    );

uint32_t 
GArrayStartP(
    GArrayP *array
    );


#endif
