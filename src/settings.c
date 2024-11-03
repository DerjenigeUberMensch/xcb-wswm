#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#include "settings.h"
#include "util.h"
#include "safebool.h"
#include "config.h"
#include "file_util.h"


struct 
USSettingData
{
    const char *const names[UserSettingsLAST];
    const u8 name_len[UserSettingsLAST];
    const u8 field_size[UserSettingsLAST];
    const u16 field_offset[UserSettingsLAST];
    const enum SCType type[UserSettingsLAST];
    const Generic default_data[UserSettingsLAST];
};

#ifdef ADD_MEMBER
#define ADD_MEMBER1456 ADD_MEMBER
#undef ADD_MEMBER
#endif


/* non-extension compliant 'switch' statment. */
#define ADD_MEMBER_SCTypeNoType(DEFAULT_SETTING)     { .data64 = { DEFAULT_SETTING } }
#define ADD_MEMBER_SCTypeBOOL(DEFAULT_SETTING)       { .data8  = { DEFAULT_SETTING } }
#define ADD_MEMBER_SCTypeUCHAR(DEFAULT_SETTING)      { .data8  = { DEFAULT_SETTING } }
#define ADD_MEMBER_SCTypeCHAR(DEFAULT_SETTING)       { .data8i = { DEFAULT_SETTING } }
#define ADD_MEMBER_SCTypeUSHORT(DEFAULT_SETTING)     { .data16 = { DEFAULT_SETTING } }
#define ADD_MEMBER_SCTypeSHORT(DEFAULT_SETTING)      { .data16i= { DEFAULT_SETTING } }
#define ADD_MEMBER_SCTypeUINT(DEFAULT_SETTING)       { .data32 = { DEFAULT_SETTING } }
#define ADD_MEMBER_SCTypeINT(DEFAULT_SETTING)        { .data32i= { DEFAULT_SETTING } }
#define ADD_MEMBER_SCTypeFLOAT(DEFAULT_SETTING)      { .dataf  = { DEFAULT_SETTING } }
#define ADD_MEMBER_SCTypeDOUBLE(DEFAULT_SETTING)     { .datad  = { DEFAULT_SETTING } }
#define ADD_MEMBER_SCTypeLONG(DEFAULT_SETTING)       { .data64i= { DEFAULT_SETTING } }
#define ADD_MEMBER_SCTypeULONG(DEFAULT_SETTING)      { .data64 = { DEFAULT_SETTING } }
#define ADD_MEMBER_SCTypeSTRING(DEFAULT_SETTING)     { .v      = { DEFAULT_SETTING } }

#define ADD_MEMBER_TYPED43(TYPE, DEFAULT_SETTING) \
        ADD_MEMBER_##TYPE(DEFAULT_SETTING)

#define ADD_MEMBER(NAME, STRUCT_NAME, TYPE, DEFAULT_SETTING) \
        .names[NAME] = #NAME, \
        .name_len[NAME] = sizeof(#NAME), \
        .field_size[NAME] = FIELD_SIZEOF(UserSettings, STRUCT_NAME), \
        .field_offset[NAME] = offsetof(UserSettings, STRUCT_NAME), \
        .type[NAME] = TYPE, \
        .default_data[NAME] = ADD_MEMBER_TYPED43(TYPE, DEFAULT_SETTING),

static const struct USSettingData
__USER__SETTINGS__DATA__ = 
{
    ADD_MEMBER(MFact, mfact, SCTypeFLOAT, 0.55f)
    ADD_MEMBER(GapRatio, gapratio, SCTypeFLOAT, 0.95f)
    ADD_MEMBER(MCount, mcount, SCTypeUSHORT, 1)
    ADD_MEMBER(Snap, snap, SCTypeUSHORT, 10)
    ADD_MEMBER(RefreshRate, refreshrate, SCTypeUSHORT, 60)
    ADD_MEMBER(MaxCC, maxcc, SCTypeUSHORT, 256)

    /* BOOL Types */
    ADD_MEMBER(HoverFocus, hoverfocus, SCTypeBOOL, false)
    ADD_MEMBER(UseDecorations, usedecorations, SCTypeBOOL, false)
    ADD_MEMBER(UseClientSideDecorations, useclientdecorations, SCTypeBOOL, true)
    ADD_MEMBER(PreferClientSideDecorations, preferclientdecorations, SCTypeBOOL, true)


    /* bar data */
    ADD_MEMBER(BarLX, lx, SCTypeFLOAT, 0.0f)    /*   lx    */
    ADD_MEMBER(BarLY, ly, SCTypeFLOAT, 0.0f)    /*   ly    */
    ADD_MEMBER(BarLW, lw, SCTypeFLOAT, 0.15f)   /*   lw    */
    ADD_MEMBER(BarLH, lh, SCTypeFLOAT, 1.0f)    /*   lh    */

    ADD_MEMBER(BarRX, rx, SCTypeFLOAT, 0.85f)   /* rx - rw */
    ADD_MEMBER(BarRY, ry, SCTypeFLOAT, 0.0f)    /*   ry    */
    ADD_MEMBER(BarRW, rw, SCTypeFLOAT, 0.15f)   /*   rw    */
    ADD_MEMBER(BarRH, rh, SCTypeFLOAT, 1.0f)    /*   rh    */

    ADD_MEMBER(BarTX, tx, SCTypeFLOAT, 0.0f)    /*   tx    */
    ADD_MEMBER(BarTY, ty, SCTypeFLOAT, 0.0f)    /*   ty    */
    ADD_MEMBER(BarTW, tw, SCTypeFLOAT, 1.0f)    /*   tw    */
    ADD_MEMBER(BarTH, th, SCTypeFLOAT, 0.15f)   /*   th    */

    ADD_MEMBER(BarBX, bx, SCTypeFLOAT, 0.0f)    /*   bx    */
    ADD_MEMBER(BarBY, by, SCTypeFLOAT, 0.85f)   /* bx - bh */
    ADD_MEMBER(BarBW, bw, SCTypeFLOAT, 1.0f)    /*   bw    */
    ADD_MEMBER(BarBH, bh, SCTypeFLOAT, 0.15f)   /*   bh    */
};


#undef ADD_MEMBER

#ifdef ADD_MEMBER1456
#define ADD_MEMBER ADD_MEMBER1456
#undef ADD_MEMBER1456
#endif

void
USSetupCFGVars(
        UserSettings *us
        )
{
    if(!us || !us->cfg)
    {   return;
    }

    SCParser *cfg = us->cfg;

    i32 i;
    u8 err = 0;
    const int READONLY = 1;
    const struct USSettingData *const usdata = &__USER__SETTINGS__DATA__;
    /* global settings */
    for(i = 0; i < UserSettingsLAST; ++i)
    {   
        err = SCParserNewVar(cfg, usdata->names[i], usdata->name_len[i], READONLY, usdata->field_size[i], usdata->type[i]);
        if(err)
        {   Debug("Failed to create: \"%s\"", usdata->names[i]);
        }
    }
}

void
USSetupCFGDefaults(
        UserSettings *us
        )
{
    if(!us || !us->cfg)
    {   return;
    }

    UserSettings *s = us;
    void *data;
    i32 i;
    const struct USSettingData *const usdata = &__USER__SETTINGS__DATA__;
    for(i = 0; i < UserSettingsLAST; ++i)
    {
        data = ((uint8_t *)s) + usdata->field_offset[i];
        memcpy(data, &usdata->default_data[i], usdata->field_size[i]);
    }
}

void
USInit(
        UserSettings *settings_init
        )
{
    if(!settings_init)
    {   return;
    }
    memset(settings_init, 0, sizeof(UserSettings));
    settings_init->cfg = SCParserCreate(UserSettingsLAST);
    if(settings_init->cfg)
    {
        USSetupCFGVars(settings_init);
        USSetupCFGDefaults(settings_init);
        USLoad(settings_init);
    }
}

void
USLoad(
        UserSettings *settings
        )
{
    if(!settings || !settings->cfg)
    {   return;
    }

    char __CONFIG__PATH__[FFSysGetConfigPathLengthMAX];
    SCParser *cfg = settings->cfg;
    SCItem *item;
    u8 status;

    status = WMConfigGetSettingsPath(__CONFIG__PATH__, FFSysGetConfigPathLengthMAX, NULL);
    if(status)
    {   
        Debug0("Failed to get system config path, loading defaults");
        return;
    }

    status = SCParserReadFile(cfg, __CONFIG__PATH__);
    if(status)
    {   
        Debug0("Failed to load previous data, loading defaults...");
        return;
    }
    i32 i;
    void *data;
    const struct USSettingData *const usdata = &__USER__SETTINGS__DATA__;
    for(i = 0; i < UserSettingsLAST; ++i)
    {
        data = ((uint8_t *)settings) + usdata->field_offset[i];
        item = SCParserSearch(cfg, usdata->names[i]);
        if(!item)
        {   item = SCParserSearchSlow(cfg, usdata->names[i]);
        }
        if(item)
        {   
            status = SCParserLoad(item, data, usdata->field_size[i], usdata->type[i]);
            if(status)
            {   Debug("Failed to LOAD, \"%s\"", usdata->names[i]);
            }
        }
        else
        {   Debug("Failed to FIND, \"%s\"", usdata->names[i]);
        }
    }
}

void
USSave(
        UserSettings *settings
        )
{
    if(!settings || !settings->cfg)
    {   return;
    }
    SCParser *cfg = settings->cfg;
    UserSettings *s = settings;
    i32 i;
    const struct USSettingData *const usdata = &__USER__SETTINGS__DATA__;

    for(i = 0; i < UserSettingsLAST; ++i)
    {   SCParserSaveVar(cfg, usdata->names[i], ((uint8_t *)s) + usdata->field_offset[i]);
    }

    char __CONFIG__PATH__[FFSysGetConfigPathLengthMAX];
    u8 status = WMConfigGetSettingsPath(__CONFIG__PATH__, FFSysGetConfigPathLengthMAX, NULL);

    if(status)
    {   
        Debug0("Failed to get system config path, cannot save settings.");
        return;
    }

    if(!FFFileExists(__CONFIG__PATH__))
    {
        status = FFCreateFile(__CONFIG__PATH__);
        if(status == EXIT_FAILURE)
        {   Debug0("Failed to create file, unable to write base config.");
        }
        else
        {
            Debug0("No file found, writing base config...");
            SCParserWrite(cfg, __CONFIG__PATH__);
        }
    }
    else if(FFIsFileEmpty(__CONFIG__PATH__))
    {
        Debug0("Empty file found, writing base config...");
        SCParserWrite(cfg, __CONFIG__PATH__);
    }
}

void
USWipe(
        UserSettings *settings
        )
{
    if(!settings || !settings->cfg)
    {   return;
    }
    SCParser *cfg = settings->cfg;
    SCParserDestroy(cfg);
    memset(settings, 0, sizeof(UserSettings));
}
