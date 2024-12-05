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
#ifndef __THREAD__PROMISE__H__
#define __THREAD__PROMISE__H__

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

typedef struct TPromise TPromise;

struct
TPromise
{
    void *data;
    int resolved;
    /* 4bytes pad */

    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

/* (A)Waits for a promise to resolve.
 *
 * NOTE: NULL may be returned if ResolveTPromise() was specified as NULL data.
 * NOTE: Awaiting a promise multiple times will not yield different results.
 * NOTE: Attempting to await a destroyed promise is undefined.
 *
 * RETURN: Data from Promise.
 */
void *
AwaitTPromise(
    TPromise *promise
    );


/* Destroys the given promise, usage of promise after being destroyed is undefined.
 * 
 * NOTE: if a promise is being resolved or awaited, behaviour is undefined.
 * NOTE: Calls 'free' on promise. -> free(promise);
 *
 */
void
DestroyTPromise(
    TPromise *promise
    );

/* Destroys the given promise, usage of promise after being destroyed is undefined.
 * 
 * NOTE: if a promise is being resolved or awaited, behaviour is undefined.
 * NOTE: Does not call 'free' on promise. 
 *
 */
void
DestroyTPromiseFilled(
        TPromise *promise
        );

/* Returns wheter or not promise has resolved its data.
 *
 * RETURN: true on is Done.
 * RETURN: false otherwise.
 */
int
IsTPromiseDone(
    TPromise *promise
    );

/* Allocates and intializes a promise for use.
 *
 * NOTE: This is the only Failure point for TPromises. 
 *
 * RETURN: TPromise * on Success.
 * RETURN: NULL on Failure.
 */
TPromise *
NewTPromise(
    void
    );

/* Intializes a promise for use.
 *
 * NOTE: This is the only Failure point for TPromises.
 * NOTE: field 'promise_return' SHOULD be allocated memory, at the minimum static stack memory.
 *       This is to avoid deadlocks in threads.
 *
 * RETURN: EXIT_SUCCESS on Success.
 * RETURN: EXIT_FAILURE on Failure.
 */
int
NewTPromiseFilled(
    TPromise *promise_return
    );

/* Resolves a TPromise by updating its data.
 *
 * NOTE: field 'data' MAY be NULL, in such cases NULL will be set as data.
 * NOTE: A promise may only be resolved once, further attempts to resolve a promise,
 *       Will simply 'wakeup' the other process/thread awaiting for the response.
 * NOTE: Attempting to resolve a destroyed promise is undefined.
 */
void
ResolveTPromise(
    TPromise *promise, 
    void *data
    );


#ifdef __cplusplus
}
#endif

#endif
