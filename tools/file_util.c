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
#include <string.h>

#include <sys/stat.h>
#include <errno.h>

#include "file_util.h"


int
FFGetSysConfigPath(
        char *buff,
        unsigned int buff_len,
        unsigned int *len_return
        )
{
    if(!buff || !buff_len)
    {   return EXIT_FAILURE;
    }
    char *home = NULL;
#ifdef __linux__
    home = getenv("XDG_CONFIG_HOME");
    if(!home)
    {   home = getenv("HOME");
    }
    if(home)
    {   
        const int SNPRINTF_FAILURE = -1;
        const char *const LINUX_CONFIG = "/.config/";
        int len = snprintf(buff, buff_len, "%s%s", home, LINUX_CONFIG);
        if(len == SNPRINTF_FAILURE)
        {   return EXIT_FAILURE;
        }
        if(len_return)
        {   *len_return = len;
        }
        return EXIT_SUCCESS;
    }
#endif
    return EXIT_FAILURE;
}

int
FFDirExists(
        char *const DIR_NAME
        )
{
    if(!DIR_NAME)
    {   return EXIT_FAILURE;
    }
    const int NOT_FOUND = -1;
    int statstatus = 0; 
    struct stat st = {0};

    statstatus = stat(DIR_NAME, &st);

    const int FOUND = statstatus != NOT_FOUND;
    const int IS_DIRECTORY = S_ISDIR(st.st_mode);

    return FOUND && IS_DIRECTORY;
}

int 
FFCreateDir(
        char *const DIR_NAME
        )
{
    if(!DIR_NAME)
    {   return EXIT_FAILURE;
    }
    unsigned long long int i = 0;
    unsigned long long int base = 0;

    enum { DIR_CHAR = '/' };

    char replaced_char;
    while(DIR_NAME[i])
    {   
        replaced_char = DIR_NAME[i];
        if(DIR_NAME[i] == DIR_CHAR)
        {   DIR_NAME[i] = '\0';
        }
        if(*DIR_NAME && *DIR_NAME != ' ' && !FFDirExists(DIR_NAME + base))   
        {
            int mkdirstatus = mkdir(DIR_NAME, 0777);
            if(mkdirstatus && errno != EEXIST)
            {   
                /* make sure data is not mutated */
                DIR_NAME[i] = replaced_char;
                return EXIT_FAILURE;
            }
        }
        DIR_NAME[i] = replaced_char;
        ++i;
        if(!DIR_NAME[i])
        {   break;
        }
        while(DIR_NAME[i] && DIR_NAME[i++] != DIR_CHAR);
        --i;
    }

    return EXIT_SUCCESS;
}

int
FFCreatePath(
        char *const FULL_PATH
        )
{
    if(!FULL_PATH)
    {   return EXIT_FAILURE;
    }
    unsigned long long int len = strlen(FULL_PATH);
    unsigned long long int file_index = 0;
    int ret = 0;

    enum { DIR_CHAR = '/' };

    while(len)
    {
        if(FULL_PATH[len] == DIR_CHAR)
        {   
            file_index = len;
            break;
        }
        --len;
    }

    char old_char;

    if(len)
    {   
        old_char = FULL_PATH[file_index];
        FULL_PATH[file_index] = '\0';
    }

    FFCreateDir(FULL_PATH);

    if(len)
    {   FULL_PATH[file_index] = old_char;
    }

    if(!FFFileExists(FULL_PATH))
    {
        FILE *f = fopen(FULL_PATH, "ab+");
        if(!f)
        {   ret = EXIT_FAILURE;
        }
        fclose(f);
    }

    return ret;
}

int
FFPathExists(
        char *const FULL_PATH
        )
{
    if(!FULL_PATH)
    {   return EXIT_FAILURE;
    }
    const int NOT_FOUND = -1;
    int statstatus = 0; 
    struct stat st = {0};

    statstatus = stat(FULL_PATH, &st);

    const int FOUND = statstatus != NOT_FOUND;

    return FOUND;
}

int
FFFileExists(
        char *const FILE_NAME
        )
{   
    if(!FILE_NAME)
    {   return EXIT_FAILURE;
    }
    const int NOT_FOUND = -1;
    int statstatus = 0; 
    struct stat st = {0};

    statstatus = stat(FILE_NAME, &st);

    const int FOUND = statstatus != NOT_FOUND;
    const int IS_FILE = S_ISREG(st.st_mode);

    return FOUND && IS_FILE;
}

int
FFCreateFile(
        char *const FILE_NAME
        )
{   return FFCreatePath(FILE_NAME);
}

int
FFIsFileEmpty(
        char *const FILE_NAME
        )
{
    int ret = 0;

    if(FFFileExists(FILE_NAME))
    {
        FILE *fr = fopen(FILE_NAME, "r");
        if(fr)
        {
            int c = fgetc(fr);
            if(c == EOF)
            {   ret = 1;
            }
        }
        fclose(fr);
    }

    return ret;
}
