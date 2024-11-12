#ifndef USER_SETTINGS
#define USER_SETTINGS

#include <stdint.h>

#include "parser.h"
#include "util.h"



/* non-extension compliant 'switch' statment. */
#define VOX_ADD_MEMBER_SCTypeNoType(DEFAULT_SETTING)     { .data64 = { DEFAULT_SETTING } }
#define VOX_ADD_MEMBER_SCTypeBOOL(DEFAULT_SETTING)       { .data8  = { DEFAULT_SETTING } }
#define VOX_ADD_MEMBER_SCTypeUCHAR(DEFAULT_SETTING)      { .data8  = { DEFAULT_SETTING } }
#define VOX_ADD_MEMBER_SCTypeCHAR(DEFAULT_SETTING)       { .data8i = { DEFAULT_SETTING } }
#define VOX_ADD_MEMBER_SCTypeUSHORT(DEFAULT_SETTING)     { .data16 = { DEFAULT_SETTING } }
#define VOX_ADD_MEMBER_SCTypeSHORT(DEFAULT_SETTING)      { .data16i= { DEFAULT_SETTING } }
#define VOX_ADD_MEMBER_SCTypeUINT(DEFAULT_SETTING)       { .data32 = { DEFAULT_SETTING } }
#define VOX_ADD_MEMBER_SCTypeINT(DEFAULT_SETTING)        { .data32i= { DEFAULT_SETTING } }
#define VOX_ADD_MEMBER_SCTypeFLOAT(DEFAULT_SETTING)      { .dataf  = { DEFAULT_SETTING } }
#define VOX_ADD_MEMBER_SCTypeDOUBLE(DEFAULT_SETTING)     { .datad  = { DEFAULT_SETTING } }
#define VOX_ADD_MEMBER_SCTypeLONG(DEFAULT_SETTING)       { .data64i= { DEFAULT_SETTING } }
#define VOX_ADD_MEMBER_SCTypeULONG(DEFAULT_SETTING)      { .data64 = { DEFAULT_SETTING } }
#define VOX_ADD_MEMBER_SCTypeSTRING(DEFAULT_SETTING)     { .v      = { DEFAULT_SETTING } }

#define VOX_ADD_MEMBER_TYPED(TYPE, DEFAULT_SETTING) \
        VOX_ADD_MEMBER_##TYPE(DEFAULT_SETTING)

#define VOX_ADD_MEMBER(NAME, TYPE, OFFSET, SIZE, DEFAULT_SETTING)       \
        [NAME] =                                                        \
        {                                                               \
            .name = #NAME,                                              \
            .name_len = sizeof(#NAME),                                  \
            .type = TYPE,                                               \
            .offset = OFFSET,                                           \
            .size = SIZE,                                               \
            .default_data = VOX_ADD_MEMBER_TYPED(TYPE, DEFAULT_SETTING) \
        },



/* User Settings Flags */

typedef struct SCSetting SCSetting;
typedef struct UserSettings UserSettings;

struct 
SCSetting
{
    const Generic default_data;
    const char *const name;
    const enum SCType type;
    const uint8_t name_len;
    const uint8_t size;
    const uint16_t offset;
};


struct 
UserSettings
{
    SCParser *cfg;          /* Cfg holder                                                       */

    float MFact;            /* factor of master area size [0.05..0.95]                          */
    float GapRatio;         /* invisible border pixel of windows (CFG_BORDER_PX not affected)   */
    uint16_t MCount;        /* number of clients in master area                                 */
    uint16_t Snap;          /* snap window to border in pixels; 0 to disable (NOT RECOMMENDED)  */
    uint16_t RefreshRate;   /* max refresh rate when resizing, moving windows;  0 to disable    */

    uint16_t MaxCC;         /* max number of clients (XOrg Default is 256)                      */

    /* Not bool or bitfield for portability */
    uint8_t HoverFocus;
    uint8_t UseDecorations;
    uint8_t UseClientSideDecorations;
    uint8_t PreferClientSideDecorations;            /* This option is to disable server side decorations and prefer client side if applicable */


    /* Bar Setting Data */
    /* Holds Ratios of size(s) relative to the monitor 
     * 0.0f -> 1.0f
     */
    float BarLX;    /* Ratio of Monitor x offset    */
    float BarLY;    /* Ratio of Monitor y offset    */
    float BarLW;    /* Ratio of Monitor w size      */
    float BarLH;    /* Ratio of Monitor h size      */

    /* Holds Ratios of size(s) relative to the monitor 
     * 0.0f -> 1.0f
     */
    float BarRX;    /* Ratio of Monitor x offset    */
    float BarRY;    /* Ratio of Monitor y offset    */
    float BarRW;    /* Ratio of Monitor w size      */
    float BarRH;    /* Ratio of Monitor h size      */

    /* Holds Ratios of size(s) relative to the monitor 
     * 0.0f -> 1.0f
     */
    float BarTX;    /* Ratio of Monitor x offset    */
    float BarTY;    /* Ratio of Monitor y offset    */
    float BarTW;    /* Ratio of Monitor w size      */
    float BarTH;    /* Ratio of Monitor h size      */

    /* Holds Ratios of size(s) relative to the monitor 
     * 0.0f -> 1.0f
     */
    float BarBX;    /* Ratio of Monitor x offset    */
    float BarBY;    /* Ratio of Monitor y offset    */
    float BarBW;    /* Ratio of Monitor w size      */
    float BarBH;    /* Ratio of Monitor h size      */
};

enum
{
    MFact,
    GapRatio,
    MCount,
    Snap,
    RefreshRate,
    MaxCC,


    HoverFocus,
    UseDecorations,
    UseClientSideDecorations,
    PreferClientSideDecorations,



    BarLX,
    BarLY,
    BarLW,
    BarLH,

    BarRX,
    BarRY,
    BarRW,
    BarRH,

    BarTX,
    BarTY,
    BarTW,
    BarTH,

    BarBX,
    BarBY,
    BarBW,
    BarBH,

    UserSettingsLAST,
};



/* Initialize Settings */
void NonNull
USInit(
        UserSettings *settings_init
        );
/*
 * RETURN: EXIT_SUCCESS on Success
 * RETURN: EXIT_FAILURE on Failure
 */
int 
USInitFile(
        void
        );
/* Save current settings */
void NonNull
USSave(
        UserSettings *settings
        );
/* Load save data into settings */
void NonNull
USLoad(
        UserSettings *settings
        );
/* Free settings data */
void NonNull
USWipe(
        UserSettings *settings
        );


/* Setter getters */

uint32_t HASHOVERFOCUS(UserSettings *settings);
uint32_t HASSERVERDECOR(UserSettings *settings);
uint32_t HASCLIENTDECOR(UserSettings *settings);
uint32_t PREFERCLIENTDECOR(UserSettings *settings);

void USSetHoverFocus(UserSettings *settings, uint8_t state);
void USSetUseServerDecor(UserSettings *settings, uint8_t state);
void USSetUseClientDecor(UserSettings *settings, uint8_t state);
void USSetPreferClientDecor(UserSettings *settings, uint8_t state);

#endif
