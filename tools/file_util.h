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
#ifndef __FILE__UTIL__H
#define __FILE__UTIL__H


#ifndef __linux__
#error "OS not supported functions may have undefined behaviour. "
#endif



enum
{   
    FFSysGetConfigPathLengthMAX = 
    /* MAX_FILENAME */
        255 *
#ifdef __linux__
        /* Assuming /home/user/.config/mydir/dirname/filename
         * x * 2 to allow for upto x2 layers of config directories for wacky stuff
         */
        6
#else
        10
#endif
};

/* 
 *
 * NOTE: Recomended buff length atleast 2550 bytes
 *
 * RETURN: EXIT_SUCCESS on Success.
 * RETURN: EXIT_FAILURE on Failure.
 */
int
FFGetSysConfigPath(
        char *buff,
        unsigned int buff_len,
        unsigned int *len_return
        );

/*
 * RETURN: EXIT_SUCCESS on Success.
 * RETURN: EXIT_FAILURE on Failure.
 */
int
FFDirExists(
        char *const DIR_NAME
        );


/* Creates a directory based on path.
 *
 * Ex: 
 *      FFCreateDir("/home/user/.hidden_dir/other_dir/last_dir");
 * 
 * RETURN: EXIT_SUCCESS on Success.
 * RETURN: EXIT_FAILURE on Failure.
 */
int
FFCreateDir(
        char *const DIR_NAME
        );

/* Creates a file and specified subdirectories if necessary.
 *
 * NOTE: ~ and ~/ are not supported /home/<user>/ must be used for FILE_NAME assuming home directory is meant.
 * 
 * EX:
 *      FFCreateFile("/home/john/.config/johns_config_directory/johns_config_file.cfg");
 *
 *
 * RETURN: EXIT_SUCCESS on Success.
 * RETURN: EXIT_FAILURE on Failure.
 */
int
FFCreatePath(
        char *const FULL_PATH
        );

/*
 * RETURN: 1 on Path Exists.
 * RETURN: 0 on Path Doesnt Exists.
 */
int
FFPathExists(
        char *const FULL_PATH
        );

/*
 * RETURN: 1 on File Exists.
 * RETURN: 0 on File Doesnt Exists.
 */
int
FFFileExists(
        char *const FILE_NAME
        );

/* Creates a file and specified subdirectories if necessary.
 *
 * NOTE: ~ and ~/ are not supported /home/<user>/ must be used for FILE_NAME assuming home directory is meant.
 * 
 * EX:
 *      FFCreateFile("/home/john/.config/johns_config_directory/johns_config_file.cfg");
 *
 * RETURN: EXIT_SUCCESS on Success.
 * RETURN: EXIT_FAILURE on Failure.
 */
int
FFCreateFile(
        char *const FILE_NAME
        );

/* Checks if a file is empty.
 *
 * NOTE: No safeguards if FILE_NAME is NULL.
 *
 * RETURN: 1 on File is Empty.
 * RETURN: 0 on File is not Empty.
 */
int
FFIsFileEmpty(
        char *const FILE_NAME
        );


#endif
