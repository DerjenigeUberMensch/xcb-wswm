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

    pthread_mutex_destroy(&promise->mutex);
    pthread_cond_destroy(&promise->cond);

    free(promise);
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
