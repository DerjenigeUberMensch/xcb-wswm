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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_array.h"

GArray *
GArrayCreate(
    uint32_t item_size,
    uint32_t base_allocate
    )
{
    GArray *ret = malloc(sizeof(GArray));

    if(ret)
    {
        uint8_t status = GArrayCreateFilled(ret, item_size, base_allocate);
        if(status == EXIT_FAILURE)
        {
            GArrayWipe(ret);
            free(ret);
            ret = NULL;
        }
    }
    return ret;
}

int
GArrayCreateFilled(
    GArray *array_return,
    uint32_t item_size,
    uint32_t base_allocate
    )
{
    if(!array_return || !item_size)
    {   return EXIT_FAILURE;
    }
    array_return->item_size = item_size;
    array_return->data = NULL;
    array_return->data_len = 0;
    array_return->data_len_real = 0;
    GArrayResize(array_return, base_allocate);
    return EXIT_SUCCESS;
}

void
GArrayWipe(
    GArray *array
    )
{
    if(!array)
    {   return;
    }
    free(array->data);
}

int
GArrayResize(
    GArray *array,
    uint32_t item_len
    )
{
    if(!array)
    {   return EXIT_FAILURE;
    }

    if(array->data_len == item_len)
    {   return EXIT_SUCCESS;
    }

    const float MIN_GROWTH = 1.25f;
    
    if(item_len == 0)
    {   
        free(array->data);
        array->data = NULL;
    }
    else if(!array->data)
    {   
        uint64_t size = item_len * MIN_GROWTH;
        array->data = malloc(array->item_size * size);
        if(!array->data)
        {   return EXIT_FAILURE;
        }
        array->data_len_real = size;
    }
    else
    {
        const uint8_t toosmall = array->data_len_real < item_len;
        uint64_t size;
        if(toosmall)
        {   size = item_len + (item_len / (array->data_len_real + !array->data_len_real));
        }
        else
        {   size = array->data_len_real - (array->data_len_real - item_len);
        }
        
        void *rec = realloc(array->data, array->item_size * size);
        if(!rec )
        {   return EXIT_FAILURE;
        }
        array->data = rec;
    }

    array->data_len = item_len;

    return EXIT_SUCCESS;
}
int
GArrayPushBack(
    GArray *array,
    void *item_cpy
    )
{
    if(!array || !item_cpy)
    {   return EXIT_FAILURE;
    }
    uint8_t status = GArrayResize(array, array->data_len + 1);
    if(status == EXIT_SUCCESS)
    {   
        uint8_t *data = array->data;
        uint8_t *dest = data + (array->data_len - 1) * array->item_size;
        uint8_t *src = item_cpy;
        uint32_t size = array->item_size;

        memmove(dest, src, size);
    }
    return status;
}

int
GArrayPopBack(
    GArray *array
    )
{
    if(!array)
    {   return EXIT_FAILURE;
    }
    if(array->data)
    {   
        /* make sure no underflow */
        if(array->data_len)
        {   GArrayResize(array, array->data_len - 1);
        }
    }
    return EXIT_SUCCESS;
}

int
GArrayReplace(
    GArray *array,
    void *item_cpy,
    uint32_t index
    )
{
    if(!array)
    {   return EXIT_FAILURE;
    }
    if(index > array->data_len)
    {   return EXIT_FAILURE;
    }

    uint32_t size = array->item_size;;
    uint8_t *data = array->data;
    uint8_t *dest = data + index * size;
    uint8_t *src = item_cpy;

    if(item_cpy)
    {   memmove(dest, src, size);
    }
    else
    {   memset(dest, 0, size);
    }
    return EXIT_SUCCESS;
}

int
GArrayInsert(
    GArray *array,
    void *item_cpy,
    uint32_t index
    )
{
    if(!array)
    {   return EXIT_FAILURE;
    }
    if(index >= array->data_len)
    {   return EXIT_FAILURE;
    }

    uint8_t status = GArrayResize(array, array->data_len + 1);

    if(status == EXIT_SUCCESS)
    {   
        uint32_t size = array->item_size;

        uint8_t *data = array->data;
        uint8_t *dest = data + (index + 1) * size;
        uint8_t *src = data + index * size;

        uint32_t move_size = (array->data_len - index - 1) * size;

        memmove(dest, src, move_size);
        if(item_cpy)
        {   memmove(src, item_cpy, size);
        }
        else
        {   memset(src, 0, size);
        }
    }

    return status;
}

int
GArrayDelete(
    GArray *array,
    uint32_t index
    )
{
    if(!array)
    {   return EXIT_FAILURE;
    }
    if(index >= array->data_len)
    {   return EXIT_FAILURE;
    }

    const uint32_t size = array->item_size;
    const uint32_t BYTES_MOVE = (array->data_len - index - 1) * size;

    uint8_t *data = array->data;
    uint8_t *src = data + (size * (index + 1));
    uint8_t *dest = data + (size * index);

    /* Check if last so no invalid memove */
    if(index < array->data_len - 1)
    {   memmove(dest, src, BYTES_MOVE);
    }

    GArrayResize(array, array->data_len - 1);
    return EXIT_SUCCESS;
}


void *
GArrayAt(
        GArray *array,
        uint32_t index
        )
{
    if(array->data_len < index)
    {   return NULL;
    }
    return (uint8_t *)array->data + (index * array->item_size);
}

int
GArrayAtSafe(
        GArray *array,
        uint32_t index,
        void *fill_return
        )
{
    void *data = GArrayAt(array, index);
    int ret = EXIT_FAILURE;
    if(fill_return)
    {
        if(data)
        {
            memmove(fill_return, data, array->item_size);
            ret = EXIT_SUCCESS;
        }
    }
    return ret;
}


uint32_t
GArrayEnd(
        GArray *array
        )
{
    if(array)
    {   return array->data_len;
    }
    return 0;
}

uint32_t 
GArrayStart(
        GArray *array
        )
{   return (const unsigned int) 0;
}

