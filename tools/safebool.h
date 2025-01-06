#ifndef __SAFE__BOOL__H__
#define __SAFE__BOOL__H__
/* prevent redefinitions of header file */
#ifndef _STDBOOL_H
    #define _STDBOOL_H
#endif
/* undef defs */
#ifdef bool
#undef bool
#endif
#ifdef true
#undef true
#endif
#ifdef True
#undef True
#endif
#ifdef false
#undef false
#endif
#ifdef False
#undef False
#endif
#ifndef Nill
#define Nill ((void *)0)
#endif
#ifndef NULL
#define NULL ((void *)0)
#endif
#ifndef null
#define null ((void *)0)
#endif
#if !defined(__cplusplus)
    #if defined(__STDC_VERSION__) && __STDC_VERSION__ > 201710L
        /* true and false are keywords */
        #define True true
        #define False false
    #else
        typedef enum { False, True } boolc;
        #if !defined(__STDC_VERSION__)
            /* 
             * if not defined then must be c90 or c89 
             */
            typedef enum { false, true } bool;
        #else
            typedef enum { false, true } booll;
            typedef _Bool bool;
        #endif
    #endif
#else
    #define True true
    #define False false
    typedef bool _Bool;
#endif
#ifndef __bool_true_false_are_defined 
#define __bool_true_false_are_defined (1)
#endif
/* __H__ */
#endif
