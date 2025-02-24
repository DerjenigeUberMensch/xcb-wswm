
#include "util.h"



bool 
memempty(void *mem, size_t size)
{
    size_t i;

    for(i = 0; i < size; ++i)
    {
        char *num = (char *)mem + i;

        if(*num != 0)
        {   return false;
        }
    }

    return true;
}

bool 
memnonempty(void *mem, size_t size)
{   return !memempty(mem, size);
}

bool 
memfilled(void *mem, size_t size)
{
    size_t i;

    for(i = 0; i < size; ++i)
    {   
        char *num = (char *)mem + i;
        if(*num != ~0)
        {   return false;
        }
    }
    return true;
}




