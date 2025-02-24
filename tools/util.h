/* See LICENSE file for copyright and license details. */
#ifndef __UTIL_H__42
#define __UTIL_H__42

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "safebool.h"

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef int8_t byte;
typedef uint8_t ubyte;

typedef float f32;
typedef double f64;
typedef long double f128;

typedef union Generic Generic;
typedef union ARGB ARGB;

union 
Generic
{
    void *datav[1];
    void **datavl[1];

    uint8_t data8[8];
    uint16_t data16[4];
    uint32_t data32[2];
    uint64_t data64[1];

    int8_t data8i[8];
    int16_t data16i[4];
    int32_t data32i[2];
    int64_t data64i[1];


    float dataf[2];
    double datad[1];
};

/* ORDER.
 * BLUE + (GREEN << 8) + (RED << 16) + (ALPHA << 24)
 */
union ARGB
{
#if __BYTE_ORDER == __ORDER_LITTLE_ENDIAN__
    uint8_t a;  /* Alpha value */
    uint8_t r;  /* Red Value   */
    uint8_t g;  /* Green Value */
    uint8_t b;  /* Blue Value  */
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    uint8_t b;  /* Blue Value  */
    uint8_t g;  /* Green Value */
    uint8_t r;  /* Red Value   */
    uint8_t a;  /* Alpha value */
#else
    /* 
     * NO SUPPORTED ENDIAN TYPE.
     * If you are using PDP_ENDIAN you might have to manually shift the values yourself.
     */
    #error "No supported endian type. If you are using PDP_ENDIAN you might have to manually shift the values yourself."
#endif
    uint32_t argb;  /* ARGB 32bit value */
};

#ifndef M_STRINGIFY
#define M_STRINGIFY(x)  #x
#endif

#ifndef M_CONCAT
#define M_CONCAT(a, b)  a##b
#endif

#ifndef SIZEOF_VAR
#define SIZEOF_VAR(VAR)     ((char*)(&VAR + 1) - (char*)(&VAR))
#endif

#ifndef SIZEOF_TYPE
#define SIZEOF_TYPE(TYPE)   ((char*)((TYPE*)0 + 1) - (char*)((TYPE*)0))
#endif

#ifndef FIELD_SIZEOF
#define FIELD_SIZEOF(type, member) (sizeof(((type *)0)->member))
#endif


#ifndef MAX
#define MAX(A, B)               ((A) > (B) ? (A) : (B))
#endif

#ifndef MIN
#define MIN(A, B)               ((A) < (B) ? (A) : (B))
#endif

#ifndef BETWEEN
#define BETWEEN(X, A, B)        ((A) <= (X) && (X) <= (B))
#endif

#ifndef MOD
#define MOD(N,M)                ((N)%(M) < 0 ? (N)%(M) + (M) : (N)%(M))
#endif

#ifndef TRUNC
#define TRUNC(X,A,B)            (MAX((A), MIN((X), (B))))
#endif

#ifndef FREE
#define FREE(F)                 do              \
                                {               \
                                    free(F);    \
                                    F = NULL;   \
                                } while(0)
#endif

#ifndef CLEARFLAG
#define CLEARFLAG(FLAGS, FLAG)            (((FLAGS) &= (~FLAG)))
#endif

#ifndef SETFLAG
#define SETFLAG(FLAGS, FLAG, STATE)       do                                        \
                                          {                                         \
                                              ((FLAGS) &= (~FLAG));                 \
                                              ((FLAGS) |= ((FLAG * !!(STATE))));    \
                                          } while(0)
#endif

#ifndef FLAGSET
#define FLAGSET(FLAGS, FLAG)            (((FLAGS & FLAG)))
#endif

#ifndef DIE
#define DIE(fmt, ...)           do { fprintf(stderr, "[%s:%d] by %s(): " fmt "\n", __FILE__,__LINE__,__func__,__VA_ARGS__); exit(EXIT_FAILURE); } while (0)
#endif

#ifndef DIECAT
#define DIECAT(fmt, ...)        do { fprintf(stderr, "[%s:%d] by %s()\n"                \
                                    "________________________________\n"                \
                                    "|                    /)        |\n"                \
                                    "|           /\\___/\\ ((         |\n"              \
                                    "|           \\`@_@'/  ))        "  "   " fmt "\n"  \
                                    "|           {_:Y:.}_//         |\n"                \
                                    "|-----------{_}^-'{_}----------|\n"                \
                                    "\n", __FILE__,__LINE__,__func__,__VA_ARGS__); exit(EXIT_FAILURE); } while (0)
#endif


#ifdef DEBUG

#ifndef Debug
#define Debug(fmt, ...) (fprintf(stderr, "[%s:%d] by %s(): " fmt "\n", __FILE__,__LINE__,__func__,__VA_ARGS__))
#endif

#ifndef Debug0
#define Debug0(X) (fprintf(stderr, "[%s:%d] by %s(): " X "\n", __FILE__, __LINE__, __func__))
#endif

#ifndef ASSERT
#include <assert.h>
#define ASSERT(X) (assert(X))
#endif

#else

#ifndef Debug
#define Debug(fmt, ...) ((void)0)
#endif

#ifndef Debug0
#define Debug0(X)       ((void)0)
#endif

#ifndef ASSERT
#define ASSERT(X)       ((void)0)
#endif


#endif

/* gcc */
#if defined(__GNUC__) || defined(__clang__)

#ifndef ASM
#define ASM(X)                          (__asm__(X))
#endif

#ifndef asm
#define asm __asm__
#endif

#ifndef __HOT__
#define __HOT__                         __attribute__((hot))
#endif

#ifndef __COLD__
#define __COLD__                        __attribute__((cold))
#endif

#ifndef __RETURN__NEVER__NULL__
#define __RETURN__NEVER__NULL__ __attribute__((returns_nonnull));
#endif

#ifndef __DEPRECATED__
#define __DEPRECATED__ __attribute__((deprecated))
#endif

/* Indicates that a function never returns (includes void).
 * What does this mean?;
 * A.) This function always results in a infinite loop;
 * b.) This function calls a variation of exit();
 */
#ifndef __FUNC__EXIT__POINT__
#define __FUNC__EXIT__POINT__ __attribute__((noreturn));
#endif

#ifndef __FUNC__ALLOC__POINT__
#define __FUNC__ALLOC__POINT__ __attribute__((malloc));
#endif

#ifndef __FUNC__ARG__ALLOC__SIZE__
#define __FUNC__ARG__ALLOC__SIZE__  __attribute__((alloc_size(1)));
#endif

#ifndef __FUNC__NO__OTHER__CALL__
#define __FUNC__NO__OTHER__CALL__ __attribute__((leaf));
#endif

#ifndef __FUNC__ARG__NON__NULL__
#define __FUNC__ARG__NON__NULL__ __attribute__((nonnull(1)));
#endif 

#ifndef __FUNC__VAR__USED__
#define __FUNC__VAR__USED__ __attribute__((used));
#endif

#ifndef NOINLINE
#define NOINLINE                        __attribute__ ((noinline))
#endif

#ifndef _PRAGMA_ONCE_
#define _PRAGMA_ONCE_                   #pragma once
#endif

#ifndef __cplusplus
#define fallthrough __attribute__((fallthrough));
#endif

#ifndef ASSUME
#define ASSUME(cond) do { if (!(cond)) __builtin_unreachable(); } while (0)
#endif

#ifndef likely
#define likely(X)    __builtin_expect(!!(X), 1)
#endif

#ifndef unlikely
#define unlikely(X) __builtin_expect(!!(X), 0)
#endif

#ifndef NonNull
#define NonNull __attribute__((nonnull(1)))
#endif

#ifndef NonNullArg
#define NonNullArg(PTRS_NON_NULL_COUNT) __attribute__((nonnull(PTRS_NON_NULL_COUNT)))
#endif

#ifndef NonNullAll
#define NonNullAll __attribute__((nonnull))
#endif

#ifndef FuncNullable
#define FuncNullable
#endif

#ifndef FuncNullableArg
#define FuncNullableArg(Arg)
#endif

#ifndef FuncNullableAll
#define FuncNullableAll
#endif

#else
#ifndef ASM
#define ASM(X)                          ((void)0)
#endif

#ifndef asm
#define asm 
#endif

#ifndef __HOT__
#define __HOT__
#endif

#ifndef __COLD__
#define __COLD__
#endif

#ifndef __RETURN__NEVER__NULL__
#define __RETURN__NEVER__NULL__ 
#endif

#ifndef __DEPRECATED__
#define __DEPRECATED__ 
#endif

#ifndef __FUNC__EXIT__POINT__
#define __FUNC__EXIT__POINT__ 
#endif

#ifndef __FUNC__ALLOC__POINT__
#define __FUNC__ALLOC__POINT__ 
#endif

#ifndef __FUNC__ARG__ALLOC__SIZE__
#define __FUNC__ARG__ALLOC__SIZE__  
#endif

#ifndef __FUNC__NO__OTHER__CALL__
#define __FUNC__NO__OTHER__CALL__ 
#endif

#ifndef __FUNC__ARG__NON__NULL__
#define __FUNC__ARG__NON__NULL__ 
#endif 

#ifndef __FUNC__VAR__USED__
#define __FUNC__VAR__USED__ 
#endif

#ifndef NOINLINE
#define NOINLINE
#endif

#ifndef _PRAGMA_ONCE_
#define _PRAGMA_ONCE_
#endif

#ifndef __cplusplus
#define fallthrough 
#endif

#ifndef ASSUME
#define ASSUME(X)   ((void)0)
#endif

#ifndef likely
#define likely(X)    (!!X)
#endif

#ifndef unlikely
#define unlikely(X)  (!!X)
#endif

#ifndef NonNull
#define NonNull
#endif

#ifndef NonNullArg
#define NonNullArg(PTRS_NON_NULL_COUNT)
#endif
 
#ifndef NonNullAll
#define NonNullAll
#endif

#ifndef FuncNullable
#define FuncNullable
#endif

#ifndef FuncNullableArg
#define FuncNullableArg(Arg)
#endif

#ifndef FuncNullableAll
#define FuncNullableAll
#endif


#endif

#if defined(__clang__)

#ifndef EnumAsBitFlags
#define EnumAsBitFlags __attribute__((flag_enum))
#endif

#else

#ifndef EnumAsBitFlags
#define EnumAsBitFlags
#endif

#endif


/* Original implementation: Simon Tatham 
 * https://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html 
 */
/*
 * Copyright (c) 2001 Simon Tatham.
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL SIMON TATHAM BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#define MERGE_SORT_LINKED_LIST(TYPE, CMP_FUNC, HEAD, TAIL, NEXT, PREV, IS_DOUBLE, IS_CIRCULAR)\
                        do                                                              \
                        {                                                               \
                            TYPE *__p, *__q, *__e, *__tail, *__oldhead;                 \
                            TYPE *__list;                                               \
                            uint32_t __insize, __nmerges, __psize, __qsize, __i;        \
                            __insize = 1;                                               \
                            __list = HEAD;                                              \
                            while(1)                                                    \
                            {                                                           \
                                __p = __list;                                           \
                                if(IS_CIRCULAR)                                         \
                                {   __oldhead = __list;                                 \
                                }                                                       \
                                __list = NULL;                                          \
                                __tail = NULL;                                          \
                                /* count number of merges we do in this pass */         \
                                __nmerges = 0;                                          \
                                while(__p)                                              \
                                {   /* there exists a merge to be done */               \
                                    ++__nmerges;                                        \
                                    /* step `insize' places along from p */             \
                                    __q = __p;                                          \
                                    __psize = 0;                                        \
                                    for(__i = 0; __i < __insize; ++__i)                 \
                                    {                                                   \
                                        ++__psize;                                      \
                                        if(IS_CIRCULAR)                                 \
                                        {   __q = (__q->NEXT == __oldhead ? NULL : __q->NEXT);  \
                                        }                                               \
                                        else                                            \
                                        {   __q = __q->NEXT;                            \
                                        }                                               \
                                        if(!__q)                                        \
                                        {   break;                                      \
                                        }                                               \
                                    }                                                   \
                                    /* if q hasn't fallen off end,                      \
                                     * we have two lists to merge                       \
                                     */                                                 \
                                    __qsize = __insize;                                 \
                                                                                        \
                                    /* now we have two lists; merge them */             \
                                    while(__psize > 0 || (__qsize > 0 && __q))          \
                                    {                                                   \
                                        /* decide where the next element                \
                                         * of merge comes from p or q                   \
                                         */                                             \
                                        if(__psize == 0)                                \
                                        {                                               \
                                            /* p is empty; e must come from q. */       \
                                            __e = __q; __q = __q->NEXT; --__qsize;      \
                                            if(IS_CIRCULAR && __q == __oldhead)         \
                                            {   __q = NULL;                             \
                                            }                                           \
                                        }                                               \
                                        else if(__qsize == 0 || !__q)                   \
                                        {                                               \
                                            /* q is empty; e must come from p. */       \
                                            __e = __p; __p = __p->NEXT; --__psize;      \
                                            if(IS_CIRCULAR && __p == __oldhead)         \
                                            {   __p = NULL;                             \
                                            }                                           \
                                        }                                               \
                                        else if(CMP_FUNC(__p, __q) <= 0)                \
                                        {                                               \
                                            /* First element of p is lower (or same);   \
                                             * e must come from p.                      \
                                             */                                         \
                                            __e = __p; __p = __p->NEXT; --__psize;      \
                                            if(IS_CIRCULAR && __p == __oldhead)         \
                                            {   __p = NULL;                             \
                                            }                                           \
                                        }                                               \
                                        else                                            \
                                        {                                               \
                                            /* First element of q is lower;             \
                                             * e must come from q.                      \
                                             */                                         \
                                            __e = __q; __q = __q->NEXT; --__qsize;      \
                                            if(IS_CIRCULAR && __q == __oldhead)         \
                                            {   __q = NULL;                             \
                                            }                                           \
                                        }                                               \
                                        /* add the next element to the merged list */   \
                                        if(__tail)                                      \
                                        {   __tail->NEXT = __e;                         \
                                        }                                               \
                                        else                                            \
                                        {   __list = __e;                               \
                                        }                                               \
                                        if(IS_DOUBLE)                                   \
                                        {                                               \
                                            /* Maintain reverse pointers                \
                                             * in a doubly linked list.                 \
                                             */                                         \
                                            __e->PREV = __tail;                         \
                                        }                                               \
                                        __tail = __e;                                   \
                                    }                                                   \
                                    /* now p has stepped `insize' places along,         \
                                     * and q has too                                    \
                                     */                                                 \
                                    __p = __q;                                          \
                                }                                                       \
                                if(IS_CIRCULAR)                                         \
                                {                                                       \
                                    if(__tail)                                          \
                                    {   __tail->NEXT = __list;                          \
                                    }                                                   \
                                    if(IS_DOUBLE)                                       \
                                    {   __list->PREV = __tail;                          \
                                    }                                                   \
                                }                                                       \
                                else                                                    \
                                {                                                       \
                                    if(__tail)                                          \
                                    {   __tail->NEXT = NULL;                            \
                                    }                                                   \
                                }                                                       \
                                /* If we have done only one merge, we're finished.*/    \
                                if(__nmerges <= 1)                                      \
                                {                                                       \
                                    HEAD = __list;                                      \
                                    TAIL = __tail;                                      \
                                    break;                                              \
                                }                                                       \
                                /* Otherwise repeat, merging lists twice the size */    \
                                __insize *= 2;                                          \
                            }                                                           \
                        } while(0)

/* NOTE SLOWER THAN MERGE SORT, BY ALOT */

/* ONLY NON-CIRCULAR, DOUBLE LINKED */
#define INSERTION_SORT_LINKED_LIST(TYPE, CMP_FUNC, HEAD, TAIL, NEXT, PREV) \
    if(!HEAD)                   \
    {   TAIL = HEAD;            \
    }                           \
    TYPE *__s__ = NULL;         \
    TYPE *__c__ = HEAD;         \
    TYPE *__n__ = NULL;         \
    TYPE *__cs__ = NULL;        \
    while(__c__)                \
    {                           \
        __n__ = __c__->NEXT;    \
        if(!__s__ || CMP_FUNC(__s__, __c__))    \
        {                                       \
            __c__->NEXT = __s__;                \
            if(__s__)                           \
            {   __s__->PREV = __c__;            \
            }                                   \
            __s__ = __c__;                      \
            __s__->PREV = ((void *)0);          \
        }                                       \
        else                                    \
        {                                       \
            __cs__ = __s__;                     \
            while(__cs__->NEXT && CMP_FUNC(__c__, __cs__)) \
            {   __cs__ = __cs__->NEXT;          \
            }                                   \
            __c__->NEXT = __cs__->NEXT;         \
            if(__cs__->NEXT)                    \
            {   __cs__->NEXT->PREV = __c__;     \
            }                                   \
            __cs__->NEXT = __c__;               \
            __c__->PREV = __cs__;               \
        }                                       \
        if(!__n__)                              \
        {   TAIL = __c__;                       \
        }                                       \
        __c__ = __n__;                          \
    }                                           \
    HEAD = __s__;



/* Functions */

/*
 * Check if a block of memory is empty (Zero'd out).
 *
 * 11111111 <- Not Empty.
 * 11110011 <- Not Empty.
 * 00000000 <- Empty.
 *
 * RETURN: true if empty.
 * RETURN: false otherwise.
 */
bool memempty(void *mem, size_t size);
/*
 * Check if a block of memory is not empty (Has atleast 1 bit filled of data).
 *
 * 00000000 <- Empty.
 * 11111111 <- Nonempty.
 * 11110011 <- Nonempty.
 *
 * RETURN: true if not empty.
 * RETURN: false otherwise.
 */
bool memnonempty(void *mem, size_t size);
/*
 * Check if a block of memory is filled (one'd out).
 *
 * 10111111 <- Not filled.
 * 00000000 <- Not filled.
 * 11111111 <- Filled.
 *
 * RETURN: true if filled.
 * RETURN: false otherwise.
 */
bool memfilled(void *mem, size_t size);




#endif
