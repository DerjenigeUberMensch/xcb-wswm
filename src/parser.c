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
#include <stdint.h>
#include <string.h>
#include <ctype.h>  /* isdigit() */

#include "parser.h"


#include "tools/DynamicArray/dynamic_array.h"
#include "khash.h"

/* hashing */
KHASH_MAP_INIT_STR(__STR__TABLE__, uint32_t)

struct 
_SP_PARSER_STRUT
{
    GArray items;
    uint32_t items_len;
    khash_t(__STR__TABLE__) *strtable;
};

struct
_SP_PARSER_ITEM
{
    char *name;
    char *typename;
    void *data;
    uint32_t name_len;
    uint32_t type_len;
    uint16_t size;
    uint8_t allocated;
    uint8_t type;
};

enum
{
    SC_PARSER_FORMAT_BUFF_FILL_SIZE = 4,
};


void
__REMOVE__CHAR__FROM__STRING__(char *str, const char remove)
{
    if(!str || !remove)
    {   return;
    }
    char *d = str;
    do
    {
        while(*d == remove)
        {   ++d;
        }
    } while((*str++ = *d++));
}


/*
 * RETURN: EXIT_SUCCESS on Success.
 * RETURN: EXIT_FAILURE on Failure.
 */
int
__REMOVE__EXTRAS__STRING(
    char *buff,
    uint32_t buff_length,
    uint32_t *len_return
    )
{
    if(!buff || !buff_length)
    {   return EXIT_FAILURE;
    }
    enum
    {
        WHITESPACE = ' ',
        COMMENT = '#',
    };

    uint32_t len = 0;

    __REMOVE__CHAR__FROM__STRING__(buff, WHITESPACE);

    /* set length if reached comment */
    while(*buff++)
    {
        switch(*buff)
        {
            case COMMENT:
                goto RETURN;
        }
        ++len;
    }

RETURN:
    if(len_return)
    {   *len_return = len;
    }
    return EXIT_SUCCESS;
}

static int
__SC__PARSER__NAME(
        char *str,
        uint32_t *len_return
        )
{
    if(!str)
    {   return EXIT_FAILURE;
    }
    const char *delimeter = "=";
    if(len_return)
    {   *len_return = strcspn(str, delimeter);
    }
    return EXIT_SUCCESS;
}

static int /* FILE READ,   Buffer FILL data, Buffer Length */
__FILE_GET_NEW_LINE(FILE *fr, char *buff, unsigned int bufflength)
{
    char *nl;
    if(fgets(buff, bufflength, fr))
    {
        nl = strchr(buff, '\n');
        if(!nl)
        {
            if(!feof(fr))
            {   return ParseOverflow;
            }
        } /* remove new line char */
        else
        {   *nl = '\0';
        }
        return ParseSuccess;
    }
    else
    {
        if(ferror(fr))
        {   return ParseError;
        }
        return ParseEOF;
    }
}

static int 
__SC_GET_FORMAT_FROM_TYPE_FILL(const enum SCType t, char fill_buff[SC_PARSER_FORMAT_BUFF_FILL_SIZE])
{
    int i = 0;

    fill_buff[i++] = '%';
    switch(t)
    {
        default:
        case SCTypeNoType:  fill_buff[i++] = 'd';
                            return EXIT_FAILURE;
        case SCTypeBOOL:    fill_buff[i++] = 's';   break;
        case SCTypeCHAR:    fill_buff[i++] = 'c';   break;
        case SCTypeUCHAR:   fill_buff[i++] = 'd';   break;
        case SCTypeSHORT:   fill_buff[i++] = 'd';   break;
        case SCTypeUSHORT:  fill_buff[i++] = 'd';   break;
        case SCTypeINT:     fill_buff[i++] = 'd';   break;
        case SCTypeUINT:    fill_buff[i++] = 'u';   break;
        case SCTypeFLOAT:   fill_buff[i++] = 'f';   break;
        case SCTypeDOUBLE:  fill_buff[i++] = 'l'; 
                            fill_buff[i++] = 'f';   break;
        case SCTypeLONG:    fill_buff[i++] = 'l'; 
                            fill_buff[i++] = 'd';   break;
        case SCTypeULONG:   fill_buff[i++] = 'l'; 
                            fill_buff[i++] = 'u';   break;
        case SCTypeSTRING:  fill_buff[i++] = 's';   break;
    }
    return EXIT_SUCCESS;
}
static int
__SC_GET_FORMAT_FROM_SIZE_FILL(const size_t size, char fill_buff[SC_PARSER_FORMAT_BUFF_FILL_SIZE])
{
    switch(size)
    {
        default:
        case sizeof(uint8_t):    return __SC_GET_FORMAT_FROM_TYPE_FILL(SCTypeCHAR, fill_buff);
        case sizeof(uint16_t):   return __SC_GET_FORMAT_FROM_TYPE_FILL(SCTypeINT, fill_buff);
        case sizeof(uint32_t):   return __SC_GET_FORMAT_FROM_TYPE_FILL(SCTypeFLOAT, fill_buff);
        case sizeof(uint64_t):   return __SC_GET_FORMAT_FROM_TYPE_FILL(SCTypeLONG, fill_buff);
    }
}

static const unsigned int
__SC_GET_SIZE_FROM_TYPE(const enum SCType t)
{
    switch(t)
    {
        default:            return 0;
        case SCTypeSTRING:  return 0;
        case SCTypeNoType:  return 0;
        case SCTypeBOOL:    return sizeof(uint8_t);
        case SCTypeCHAR:    return sizeof(int8_t);
        case SCTypeUCHAR:   return sizeof(uint8_t);
        case SCTypeSHORT:   return sizeof(int16_t);
        case SCTypeUSHORT:  return sizeof(uint16_t);
        case SCTypeINT:     return sizeof(int32_t);
        case SCTypeUINT:    return sizeof(uint32_t);
        case SCTypeFLOAT:   return sizeof(float);
        case SCTypeDOUBLE:  return sizeof(double);
        case SCTypeLONG:    return sizeof(int64_t);
        case SCTypeULONG:   return sizeof(uint64_t);
    }
}

static uint32_t
__SC__PARSER__SEARCH__INDEX__(
        SCParser *parser,
        const char *const NAME,
        unsigned int *_NOT_FOUND
        )
{
    uint32_t ret = 0;
    if(!parser || !NAME)
    {   
        *_NOT_FOUND = 1;
        return 0;
    }
    khint_t k = kh_get(__STR__TABLE__, parser->strtable, NAME);
    khint_t end = kh_end(parser->strtable);
    if(k != end)
    {   return kh_val(parser->strtable, k);
    }
    else
    {
        const uint32_t LAST = parser->items_len;
        uint32_t i;
        SCItem *item;
        for(i = 0; i < LAST; ++i)
        {
            item = GArrayAt(&parser->items, i);
            if(item && !strcmp(item->name, NAME))
            {   return i;
            }
        }
    }
    *_NOT_FOUND = 1;
    return ret;
}

SCItem *
SCParserSearch(
        SCParser *parser,
        const char *const NAME
        )
{
    void *ret = NULL;
    if(!parser || !NAME)
    {   return ret;
    }
    khint_t k = kh_get(__STR__TABLE__, parser->strtable, NAME);
    khint_t end = kh_end(parser->strtable);
    if(k != end)
    {
        uint32_t index = kh_val(parser->strtable, k);
        ret = GArrayAt(&parser->items, index);
    }
    return ret;
}

SCItem *
SCParserSearchSlow(
        SCParser *parser,
        const char *const NAME
        )
{
    if(!parser || !NAME)
    {   return NULL;
    }
    const uint32_t LAST = parser->items_len;
    uint32_t i;
    SCItem *item;
    for(i = 0; i < LAST; ++i)
    {
        item = GArrayAt(&parser->items, i);
        if(item && !strcmp(item->name, NAME))
        {   return item;
        }
    }
    return NULL;
}

int
SCParserLoad(
        SCItem *item,
        void *_return,
        const size_t bytescopy,
        const enum SCType _optional_type
        )
{
    const int FAILURE = 1;
    const int SUCCESS = 0;
    if(!item || !item->typename || !_return)
    {   return FAILURE;
    }

    const char *const TRUE_STRING = "true";
    const char *const FALSE_STRING = "false";
    const int SSCANF_CHECKSUM = 1;
    const int DATA_SIZE = 32;
    uint8_t check;
    uint8_t data[DATA_SIZE];
    char format[SC_PARSER_FORMAT_BUFF_FILL_SIZE];
    char *str;

    /* clear memory */
    memset(data, 0, sizeof(uint8_t) * DATA_SIZE);
    memset(format, 0, sizeof(char) * SC_PARSER_FORMAT_BUFF_FILL_SIZE);
    /* type handler */
    if(_optional_type == SCTypeNoType)
    {   goto NOTYPE;
    }
    /* else we got a type */
    if(_optional_type == SCTypeSTRING)
    {   goto STRINGTYPE;
    }
    if(_optional_type == SCTypeBOOL)
    {   goto BOOLTYPE;
    }
    check = __SC_GET_FORMAT_FROM_TYPE_FILL(_optional_type, format);
    if(check == EXIT_FAILURE)
    {   goto NOTYPE;
    }
    goto SINGLETYPE;
BOOLTYPE:
    if(!strcmp(item->typename, TRUE_STRING))
    {   
        *(char *)_return = 1;
        return SUCCESS;
    }
    if(!strcmp(item->typename, FALSE_STRING))
    {   
        memset(_return, 0, sizeof(uint8_t));
        return SUCCESS;
    }
    /* FALLTHROUGH */
SINGLETYPE:
    check = sscanf(item->typename, format, &data);
    if(check == SSCANF_CHECKSUM)
    {
        size_t copysize = bytescopy;
        if(bytescopy > __SC_GET_SIZE_FROM_TYPE(item->type))
        {   copysize = __SC_GET_SIZE_FROM_TYPE(item->type);
        }
        memcpy(_return, data, copysize);
        return SUCCESS;
    }
    return FAILURE;
STRINGTYPE:
    str = malloc(item->type_len);
    if(str)
    {
        memcpy(str, item->typename, item->type_len);
        memcpy(_return, &str, sizeof(char *));
        return SUCCESS;
    }
    return FAILURE;
NOTYPE:
    check = __SC_GET_FORMAT_FROM_SIZE_FILL(item->size, (char *)format);
    if(check == EXIT_SUCCESS)
    {   check = sscanf(item->typename, format, data);
    }
    else
    {   check = !SSCANF_CHECKSUM;
    }
    if(check == SSCANF_CHECKSUM)
    {
        memcpy(_return, data, bytescopy);
        return SUCCESS;
    }
    return FAILURE;
}


int
SCParserWriteVarData(
            FILE *fw,
            char *data,
            enum SCType type,
            size_t size
            )
{
    if(!fw || !data)
    {   return EXIT_FAILURE;
    }
    char format[SC_PARSER_FORMAT_BUFF_FILL_SIZE];

    if(type)
    {
        __SC_GET_FORMAT_FROM_TYPE_FILL(type, format);
        switch(type)
        {
            case SCTypeBOOL:
                fprintf(fw, format, *(uint8_t *)data ? "true" : "false");
                break;
            case SCTypeCHAR:
                fprintf(fw, format, *(int8_t *)data);
                break;
            case SCTypeUCHAR:
                fprintf(fw, format, *(uint8_t *)data);
                break;
            case SCTypeSHORT:
                fprintf(fw, format, *(int16_t *)data);
                break;
            case SCTypeUSHORT:
                fprintf(fw, format, *(uint16_t *)data);
                break;
            case SCTypeINT:
                fprintf(fw, format, *(int32_t *)data);
                break;
            case SCTypeUINT:
                fprintf(fw, format, *(uint32_t *)data);
                break;
            case SCTypeFLOAT:
                fprintf(fw, format, *(float *)data);
                break;
            case SCTypeDOUBLE:
                fprintf(fw, format, *(double *)data);
                break;
            case SCTypeLONG:
                fprintf(fw, format, *(int64_t *)data);
                break;
            case SCTypeULONG:
                fprintf(fw, format, *(uint64_t *)data);
                break;
            case SCTypeSTRING:
                fprintf(fw, format, (char *)data);
                break;
            case SCTypeNoType:
                break;
        }
    }
    else
    {
        __SC_GET_FORMAT_FROM_SIZE_FILL(size, format);
        switch(size)
        {
            case sizeof(int8_t):
                fprintf(fw, format, *(char *)data);
                break;
            case sizeof(int16_t):
                fprintf(fw, format, *(int16_t *)data);
                break;
            case sizeof(int32_t):
                fprintf(fw, format, *(float *)data);
                break;
            case sizeof(int64_t):
                fprintf(fw, format, *(int64_t *)data);
                break;
        }
    }
    return EXIT_SUCCESS;
}

int
SCParserWriteVar(
        FILE *fw,
        SCItem *item
        )
{
    if(!item->data || !item->name)
    {   return EXIT_FAILURE;
    }
    const char *const name = item->name;
    const enum SCType type = item->type;
    const size_t size = item->size;

    fprintf(fw, "%s = ", name);
    SCParserWriteVarData(fw, item->data, type, size);
    fprintf(fw, "\n");
    return EXIT_SUCCESS;
}

int
SCParserWrite(
        SCParser *parser,
        const char *const FILE_NAME
        )
{
    const int SUCCESS = 0;
    const int FAILURE = 1;
    if(!parser)
    {   return FAILURE;
    }

    FILE *fw = fopen(FILE_NAME, "w");

    if(!fw)
    {   return FAILURE;
    }

    const uint32_t END = parser->items_len;
    SCItem *item;
    uint32_t i;

    for(i = 0; i < END; ++i)
    {
        item = GArrayAt(&parser->items, i);
        if(item)
        {   SCParserWriteVar(fw, item);
        }
    }
    fclose(fw);
    return SUCCESS;
}

SCParser *
SCParserCreate(
        const uint32_t BASE_VAR_COUNT
        )
{
    SCParser *p = malloc(sizeof(SCParser));
    if(p)
    {
        int status;
        p->strtable = kh_init(__STR__TABLE__);
        p->items_len = 0;

        if(!p->strtable)
        {
            free(p);
            return NULL;
        }
        status = GArrayCreateFilled(&p->items, sizeof(SCItem), BASE_VAR_COUNT);
        if(status == EXIT_FAILURE)
        {
            kh_destroy(__STR__TABLE__, p->strtable);
            GArrayWipe(&p->items);
            free(p);
            return NULL;
        }
    }
    return p;
}

void
SCParserDestroy(
        SCParser *parser
        )
{
    if(!parser)
    {   return;
    }
    const uint32_t END = parser->items_len;
    uint32_t i;
    SCItem *item;
    for(i = 0; i < END; ++i)
    {
        item = GArrayAt(&parser->items, i);
        if(item->allocated)
        {   free(item->name);
        }
        if(item->typename)
        {   free(item->typename);
        }
        free(item->data);
    }
    kh_destroy(__STR__TABLE__, parser->strtable);
    GArrayWipe(&parser->items);
    free(parser);
}

SCItem *
SCParserReadLine(
    SCParser *parser,
    char *buff,
    uint32_t buff_length,
    char **data_fill
    )
{
    uint32_t namelen;
    uint8_t namestatus;
    uint32_t typenamelen;
    __REMOVE__EXTRAS__STRING(buff, buff_length - 1, &buff_length);
    namestatus = __SC__PARSER__NAME(buff, &namelen);
    if(namestatus != EXIT_SUCCESS)
    {   return NULL;
    }
    
    const char saved_char = buff[namelen];

    buff[namelen] = '\0';

    SCItem *item;
    /* search for item. */
    item = SCParserSearch(parser, buff);
    if(!item)
    {   item = SCParserSearchSlow(parser, buff);
    }
    buff[namelen] = saved_char;

    if(item && data_fill)
    {
        namestatus = __SC__PARSER__NAME(buff + namelen + 1, &typenamelen);
        if(namestatus == EXIT_SUCCESS)
        {        
            char *data = malloc(typenamelen + 1);
            if(data)
            {   
                /* copy buff to data */
                memcpy(data, buff + namelen + 1, typenamelen);
                /* set string null byte */
                data[typenamelen] = '\0';
                /* fill data_fill with data pointer */
                memcpy(data_fill, &data, sizeof(void *));
            }
        }
    }
    return item;
}

int
SCParserReadFile(
        SCParser *parser,
        const char *const FILE_NAME
        )
{
    const int FAILURE = 1;
    const int SUCCESS = 0;

    if(!parser)
    {   return FAILURE;
    }


    FILE *fr = fopen(FILE_NAME, "r");

    if(!fr)
    {   return FAILURE;
    }

    const int BUFF_LIMIT = 1024;
    int running = 1;
    char buff[BUFF_LIMIT];
    char *typename = NULL;

    uint32_t bufflenreal = 0;
    uint32_t typenamelen = 0;
    SCItem *item;
    /* Make sure null byte is set */
    memset(buff, 0, BUFF_LIMIT);
    while(running)
    {
        switch(__FILE_GET_NEW_LINE(fr, buff, BUFF_LIMIT - 1))
        {
            case ParseOverflow:
            case ParseSuccess:
                break;
            case ParseEOF:
                running = 0;
                /* FALLTHROUGH */
            case ParseError:
            default:
                continue;
        }
        item = SCParserReadLine(parser, buff, BUFF_LIMIT, &typename);
        if(item)
        {
            if(item->typename)
            {   free(item->typename);
            }
            item->typename = typename;
            item->type_len = typenamelen;
        }
        else
        {   free(typename);
        }

        typename = NULL;
        memset(buff, 0, bufflenreal);
    }

    fclose(fr);
    return SUCCESS;
}

int
SCParserNewVar(
        SCParser *parser,
        const char *const VAR_NAME,
        const uint32_t VAR_NAME_FULL_LENGTH,
        const uint8_t READONLY_SECTION,
        const size_t size,
        const enum SCType _optional_type
        )
{
    const int FAILURE = 1;
    const int SUCCESS = 0;
    char format[SC_PARSER_FORMAT_BUFF_FILL_SIZE];
    int format_status = EXIT_FAILURE;

    if(!parser || !VAR_NAME)
    {   return FAILURE;
    }
    format_status = __SC_GET_FORMAT_FROM_TYPE_FILL(_optional_type, format);
    if(!size && format_status == EXIT_FAILURE)
    {   return FAILURE;
    }

    SCItem item;

    if(_optional_type == SCTypeSTRING)
    {
        item.size = sizeof(char *);
        item.type = SCTypeSTRING;
    }
    else if(format_status == EXIT_SUCCESS)
    {
        item.size = __SC_GET_SIZE_FROM_TYPE(_optional_type);
        item.type = _optional_type;
    }
    else if(size)
    {
        item.size = size;
        item.type = SCTypeNoType;
    }
    else
    {   return FAILURE;
    }

    if(READONLY_SECTION)
    {
        item.name = (char *)VAR_NAME;
        item.allocated = 0;
    }
    else
    {
        item.name = malloc(VAR_NAME_FULL_LENGTH * sizeof(char));
        item.allocated = 1;
        if(!item.name)
        {   return FAILURE;
        }
        memcpy(item.name, VAR_NAME, VAR_NAME_FULL_LENGTH);
    }
    item.name_len = VAR_NAME_FULL_LENGTH;
    item.data = malloc(item.size);
    item.typename = NULL;
    item.type_len = 0;

    int status;
    if(GArrayEnd(&parser->items) <= parser->items_len)
    {   status = GArrayPushBack(&parser->items, &item);
    }
    else
    {   status = GArrayReplace(&parser->items, &item, parser->items_len);
    }
    if(status == EXIT_FAILURE)
    {
        free(item.data);
        if(item.allocated)
        {   free(item.name);
        }
        return EXIT_FAILURE;
    }

    /* add to table */
    int err = 3;
    khint_t k = kh_put(__STR__TABLE__, parser->strtable, item.name, &err);
    enum
    {
        __KHASH_BAD_OPERATION = -1,
        __KHASH_ALREADY_PRESENT = 0,
        __KHASH_FIRST_HASH = 1,
        __KHASH_PREVIOUSLY_DELETED = 2,
    };
    /* set value to client */
    switch(err)
    {
        case __KHASH_BAD_OPERATION:
            /* FALLTHROUGH */
        case __KHASH_ALREADY_PRESENT:
            break;

        case __KHASH_FIRST_HASH:
            /* FALLTHROUGH */
        case __KHASH_PREVIOUSLY_DELETED:
            /* FALLTHROUGH */
        default:
            /* bounds check */
            if(kh_end(parser->strtable) > k)
            {   kh_value(parser->strtable, k) = parser->items_len;
            }
            break;
    }
    ++parser->items_len;
    return SUCCESS;
}

int
SCParserDelVar(
        SCParser *parser,
        const char *const VAR_NAME
        )
{
    unsigned int NOT_FOUND = 0;
    uint32_t index =  __SC__PARSER__SEARCH__INDEX__(parser, VAR_NAME, &NOT_FOUND);
    if(!NOT_FOUND)
    {
        SCItem *item = GArrayAt(&parser->items, index);
        if(item)
        {
            if(item->allocated)
            {   free(item->name);
            }
            free(item->typename);
            --parser->items_len;
            return GArrayDelete(&parser->items, index);
        }
    }
    return EXIT_FAILURE;
}

int
SCParserSaveVar(
        SCParser *parser,
        const char *const VAR_NAME,
        void *data
        )
{
    const int FAILURE = 1;
    const int SUCCESS = 0;
    if(!parser || !VAR_NAME)
    {   return FAILURE;
    }


    SCItem *item = SCParserSearch(parser, VAR_NAME);
    if(!item)
    {   item = SCParserSearchSlow(parser, VAR_NAME);
    }
    if(!item)
    {   return FAILURE;
    }

    if(!item->data)
    {   item->data = malloc(item->size);
    }
    if(!item->data)
    {   return FAILURE;
    }
    memcpy(item->data, data, item->size);
    return SUCCESS;
}
