#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "main.h"
#include "safebool.h"
#include "argcv.h"

extern WM _wm;

struct
ArgCVCommand
{
    void (*const func_run)(void);                   /* if NULL, command is not displayed */
    const char *const str_name_and_description;
    uint16_t name_len;
    uint16_t description_len;
};

#define ARG_CV_NEW_COMMAND(NAME, DESCRIPTION, FUNCTION)             \
            {                                                       \
                .func_run = FUNCTION,                               \
                .str_name_and_description = NAME "\0" DESCRIPTION,  \
                .name_len = sizeof(NAME) - 1,                       \
                .description_len = sizeof(DESCRIPTION) - 1,         \
            },


const char *const usage_format = "Usage: " MARK " [OPTION] ...";

const struct ArgCVCommand
__single__commands__[] = 
{
    ARG_CV_NEW_COMMAND("h", "Help Information", ArgcvDisplayHelp)
    ARG_CV_NEW_COMMAND("v", "Version Information", ArgcvDisplayCompilerInfo)
};

const struct ArgCVCommand
__double__commands__[] = 
{
    ARG_CV_NEW_COMMAND("NULL", "NULL.", NULL)
};


bool
ArgcvIsSingleCommand(
        char *str,
        unsigned long len
        )
{
    const unsigned char MIN_LENGTH = 2;     /* -X */
    const unsigned char SINGLE_OPTION_INDEX_DASH = 0;
    const unsigned char SINGLE_OPTION_INDEX_LETTER = 1;


    const bool LONG_ENOUGH = len >= MIN_LENGTH;
    const bool VALID_DASH = str[SINGLE_OPTION_INDEX_DASH] == '-';
    const bool VALID_CHAR = isalpha(str[SINGLE_OPTION_INDEX_LETTER]);

    return LONG_ENOUGH && VALID_DASH && VALID_CHAR;
}

bool
ArgcvIsDoubleCommand(
        char *str,
        unsigned long len
        )
{
    const unsigned char MIN_LENGTH = 3;
    const unsigned char DOUBLE_OPTION_INDEX_DASH_FIRST = 0;
    const unsigned char DOUBLE_OPTION_INDEX_DASH_SECOND = 1;
    const unsigned char DOUBLE_OPTION_INDEX_STRING_START = 2;

    const bool LONG_ENOUGH = len >= MIN_LENGTH;
    const bool VALID_DASHES = str[DOUBLE_OPTION_INDEX_DASH_FIRST] == '-' && str[DOUBLE_OPTION_INDEX_DASH_SECOND] == '-';
    const bool VALID_CHAR = isalpha(str[DOUBLE_OPTION_INDEX_STRING_START]);

    return LONG_ENOUGH && VALID_DASHES && VALID_CHAR;
}

void
ArgcvDisplayHelp(
        void
        )
{
    puts(usage_format);

    i32 i;
                                /* These random spaces are used in standard 'help' formatting IDK dont know why */
    const char *const format_single = "  -%s           %s.\n";
    const char *const format_double = "  --%s          %s.\n";
    for(i = 0; i < LENGTH(__single__commands__); ++i)
    {
        const struct ArgCVCommand *const command = __single__commands__ + i;

        if(command->func_run)
        {   printf(format_single, command->str_name_and_description, command->str_name_and_description + command->name_len + 1);
        }
    }

    for(i = 0; i < LENGTH(__double__commands__); ++i)
    {
        const struct ArgCVCommand *const command = __double__commands__ + i;
        if(command->func_run)
        {   printf(format_double, command->str_name_and_description, command->str_name_and_description + command->name_len + 1);
        }
    }

    exit(EXIT_SUCCESS);
}

void
ArgcvDisplayCompilerInfo(
        void
        )
{
    char *compiler;
    short majorversion = -1;
    short minorversion = -1;
    short patchversion = -1;
    compiler = "UNKNOWN";
#if defined(__GNUC__)
    compiler = "GCC";
    majorversion = __GNUC__;
    minorversion = __GNUC_MINOR__;
    patchversion = __GNUC_PATCHLEVEL__;
#elif defined(__clang__)
    compiler = "clang";
    majorversion = __clang_major__;
    minorversion = __clang_minor__;
    patchversion = __clang_patchlevel__;
#elif defined(_MSC_VER)
    compiler = "MSVC";
    majorversion = _MSC_VER;
    minorversion = 0;
    patchversion = 0;
#elif defined(__INTEL__COMPILER)
    compiler = "INTEL";
    majorversion = __INTEL_COMPILER;
    minorversion = 0;
    patchversion = #ifdef __INTEL_COMPILER_UPDATE __INTEL_COMPILER_UPDATE #else 0 #endif;
#endif

    const int MMM = 3;
    const int SPACE = 1;
    const int DD = 2;

    /* format is MMM <space> DD <space> YYYY
     * which looks like this:        Jan  1 20XX
     * Or like this if double digit: Jan 10 20XX
     * Furthermore we must use [] instead of char * cause its some addr idk, segfault otherwise
     */
    char date [] = __DATE__ "";

    /* This fixes the Jan  1 20XX
     * where theres just a hanging space there 
     * Why are we doing all this work, well basically so printf is the only point of failure here, cause why not.
     * TODO FIXME.
     */
    if(date[MMM + SPACE + DD - 2] == ' ')
    {   date[MMM + SPACE + DD - 2] = '0';
    }

    printf( "Compiler Information.\n"
            "  Compiled:        %s %s\n"
            "  Compiler:        [%s v%d.%d.%d]\n" 
            "  STDC:            [%d] [%lu]\n"
            "  BYTE_ORDER:      [%d]\n"
            "  POINTER_SIZE:    [%d]\n"
            "Version Information.\n"
            "  VERSION:         [%s]\n"
            "  MARK:            [%s]\n"
            ,
            /* TODO __DATE__ has an extra space for some reason? */ 
            date, __TIME__,
            compiler, majorversion, minorversion, patchversion,
            __STDC_HOSTED__, __STDC_VERSION__,
            __BYTE_ORDER__,
            __SIZEOF_POINTER__,
            VERSION,
            MARK
          );

    exit(EXIT_SUCCESS);
}

void
ArgcvDisplayBadArgs(
        char *arg_str
        )
{
    printf("UNKNOWN COMMAND: \'%s\'\n", arg_str);
}

void
ArgcvSingleCommandHandler(
        char *str
        )
{
    const unsigned char SINGLE_OPTION_INDEX_LETTER = 1;

    int i;
    for(i = 0; i < LENGTH(__single__commands__); ++i)
    {
        const struct ArgCVCommand *const command = __single__commands__ + i;
        if(!strcmp(command->str_name_and_description, str + SINGLE_OPTION_INDEX_LETTER))
        {   
            if(command->func_run)
            {   command->func_run();
            }
            return;
        }
    }

    /* command not found, display error */
    ArgcvDisplayBadArgs(str);
}

void
ArgcvDoubleCommandHandler(
        char *str
        )
{
    const unsigned char DOUBLE_OPTION_INDEX_STR = 2;

    int i;
    for(i = 0; i < LENGTH(__double__commands__); ++i)
    {
        const struct ArgCVCommand *const command = __double__commands__ + i;
        if(!strcmp(command->str_name_and_description, str + DOUBLE_OPTION_INDEX_STR))
        {   
            if(command->func_run)
            {   command->func_run();
            }
            return;
        }
    }

    /* command not found, display error */
    ArgcvDisplayBadArgs(str);
}


void
ArgcvHandler(
        int argc, 
        char **argv
        )
{
    unsigned int i;
    unsigned long len;

    for(i = 0; i < argc; ++i)
    {
        if(!argv[i])
        {   continue;
        }
        len = strlen(argv[i]);
        if(ArgcvIsSingleCommand(argv[i], len))
        {   ArgcvSingleCommandHandler(argv[i]);
        }
        else if(ArgcvIsDoubleCommand(argv[i], len))
        {   ArgcvDoubleCommandHandler(argv[i]);
        }
        else
        {   
            const char exec1 = '.';
            const char exec2 = '/';
            const char execcount = 3; /* not 2 because we need \0 */ /* +1 for the possible 1 letter name and +1 again for \0   */
            if(argv[0] != NULL && strnlen(argv[0], execcount + 2) >= execcount && argv[0][0] == exec1 && argv[0][1] == exec2)
            {   
                /* We can call die because it is very likely this was run manually */
                if(i > 0)
                {
                    ArgcvDisplayHelp();
                    exit(EXIT_SUCCESS);
                }
            }
            else
            {   /* We dont die because likely this command was run using some form of exec */
                ArgcvDisplayBadArgs(argv[i]);
            }
        }
    }
}


