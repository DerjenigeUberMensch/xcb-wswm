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
#include <stdio.h>
#include <stdlib.h>

#include "tpromise.h"


void *
AwaitTPromise(
    TPromise *promise
    )
{
    pthread_mutex_lock(&promise->mutex);
    while(!promise->resolved)
    {   pthread_cond_wait(&promise->cond, &promise->mutex);
    }
    pthread_mutex_unlock(&promise->mutex);
    return promise->data;
}

void
DestroyTPromise(
    TPromise *promise
    )
{
    if(!promise)
    {   return;
    }

    DestroyTPromiseFilled(promise);


    free(promise);
}

void
DestroyTPromiseFilled(
        TPromise *promise
        )
{
    if(!promise)
    {   return;
    }

    pthread_mutex_destroy(&promise->mutex);
    pthread_cond_destroy(&promise->cond);
}

int
IsTPromiseDone(
    TPromise *promise
    )
{
    int promise_done = 0;

    if(!promise)
    {   return promise_done;
    }

    pthread_mutex_lock(&promise->mutex);
    if(promise->resolved)
    {   promise_done = 1;
    }
    pthread_mutex_unlock(&promise->mutex);
    
    return promise_done;
}

TPromise *
NewTPromise(
    void
    )
{
    TPromise *promise = malloc(sizeof(*promise));
    if(promise)
    {
        int status = NewTPromiseFilled(promise);
        if(status != EXIT_SUCCESS)
        {   
            free(promise);
            promise = NULL;
        }
    }
    return promise;
}

int
NewTPromiseFilled(
    TPromise *promise_return
    )
{
    if(!promise_return)
    {   return EXIT_FAILURE;
    }
    const TPromise _cpy = 
    {
        .data = NULL,
        .resolved = 0,
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .cond = PTHREAD_COND_INITIALIZER,
    };
    *promise_return = _cpy;
    return EXIT_SUCCESS;
}

void
ResolveTPromise(
    TPromise *promise, 
    void *data
    )
{
    pthread_mutex_lock(&promise->mutex);
    if(!promise->resolved)
    {   
        promise->data = data;
        promise->resolved = 1;
    }
    pthread_cond_signal(&promise->cond);
    pthread_mutex_unlock(&promise->mutex);
}
