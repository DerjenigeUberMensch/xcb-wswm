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


#define VOX_ADD_MEMBER_SETTING(NAME, TYPE, DEFAULT_SETTING) \
        VOX_ADD_MEMBER(NAME, TYPE, offsetof(UserSettings, NAME), FIELD_SIZEOF(UserSettings, NAME) ,DEFAULT_SETTING)

static const SCSetting
__USER__SETTINGS__DATA__[] = 
{
    VOX_ADD_MEMBER_SETTING(MFact, SCTypeFLOAT, 0.55f)
    VOX_ADD_MEMBER_SETTING(GapRatio, SCTypeFLOAT, 0.95f)
    VOX_ADD_MEMBER_SETTING(MCount, SCTypeUSHORT, 1)
    VOX_ADD_MEMBER_SETTING(Snap, SCTypeUSHORT, 10)
    VOX_ADD_MEMBER_SETTING(RefreshRate, SCTypeUSHORT, 60)
    VOX_ADD_MEMBER_SETTING(MaxCC, SCTypeUSHORT, 256)

    /* BOOL Types */
    VOX_ADD_MEMBER_SETTING(HoverFocus, SCTypeBOOL, false)
    VOX_ADD_MEMBER_SETTING(UseDecorations, SCTypeBOOL, false)
    VOX_ADD_MEMBER_SETTING(UseClientSideDecorations, SCTypeBOOL, true)
    VOX_ADD_MEMBER_SETTING(PreferClientSideDecorations, SCTypeBOOL, true)


    /* bar data */
    VOX_ADD_MEMBER_SETTING(BarLX, SCTypeFLOAT, 0.0f)    /*   lx    */
    VOX_ADD_MEMBER_SETTING(BarLY, SCTypeFLOAT, 0.0f)    /*   ly    */
    VOX_ADD_MEMBER_SETTING(BarLW, SCTypeFLOAT, 0.15f)   /*   lw    */
    VOX_ADD_MEMBER_SETTING(BarLH, SCTypeFLOAT, 1.0f)    /*   lh    */

    VOX_ADD_MEMBER_SETTING(BarRX, SCTypeFLOAT, 0.85f)   /* rx - rw */
    VOX_ADD_MEMBER_SETTING(BarRY, SCTypeFLOAT, 0.0f)    /*   ry    */
    VOX_ADD_MEMBER_SETTING(BarRW, SCTypeFLOAT, 0.15f)   /*   rw    */
    VOX_ADD_MEMBER_SETTING(BarRH, SCTypeFLOAT, 1.0f)    /*   rh    */

    VOX_ADD_MEMBER_SETTING(BarTX, SCTypeFLOAT, 0.0f)    /*   tx    */
    VOX_ADD_MEMBER_SETTING(BarTY, SCTypeFLOAT, 0.0f)    /*   ty    */
    VOX_ADD_MEMBER_SETTING(BarTW, SCTypeFLOAT, 1.0f)    /*   tw    */
    VOX_ADD_MEMBER_SETTING(BarTH, SCTypeFLOAT, 0.15f)   /*   th    */

    VOX_ADD_MEMBER_SETTING(BarBX, SCTypeFLOAT, 0.0f)    /*   bx    */
    VOX_ADD_MEMBER_SETTING(BarBY, SCTypeFLOAT, 0.85f)   /* bx - bh */
    VOX_ADD_MEMBER_SETTING(BarBW, SCTypeFLOAT, 1.0f)    /*   bw    */
    VOX_ADD_MEMBER_SETTING(BarBH, SCTypeFLOAT, 0.15f)   /*   bh    */
};


void
USSetupCFGVars(
        UserSettings *us
        )
{
    if(!us || !us->cfg)
    {   return;
    }

    SCParser *cfg = us->cfg;
    const SCSetting *usdata = __USER__SETTINGS__DATA__;

    i32 i;
    u8 err = 0;
    const int READONLY = 1;
    /* global settings */
    for(i = 0; i < UserSettingsLAST; ++i)
    {   
        err = SCParserNewVar(cfg, usdata->name, usdata->name_len, READONLY, usdata->size, usdata->type);
        if(err)
        {   Debug("Failed to create: \"%s\"", usdata->name);
        }
        ++usdata;
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
    const SCSetting *usdata = __USER__SETTINGS__DATA__;
    for(i = 0; i < UserSettingsLAST; ++i)
    {
        data = ((uint8_t *)s) + usdata->offset;
        memcpy(data, &usdata->default_data, usdata->size);
        ++usdata;
    }
}

void
USInit(
        UserSettings *settings_init
        )
{
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
    if(!settings->cfg)
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

    const SCSetting *usdata = __USER__SETTINGS__DATA__;
    for(i = 0; i < UserSettingsLAST; ++i)
    {
        data = ((uint8_t *)settings) + usdata->offset;
        item = SCParserSearch(cfg, usdata->name);
        if(!item)
        {   item = SCParserSearchSlow(cfg, usdata->name);
        }
        if(item)
        {   
            status = SCParserLoad(item, data, usdata->size, usdata->type);
            if(status)
            {   Debug("Failed to LOAD, \"%s\"", usdata->name);
            }
        }
        else
        {   Debug("Failed to FIND, \"%s\"", usdata->name);
        }
        ++usdata;
    }
}

void
USSave(
        UserSettings *settings
        )
{
    if(!settings->cfg)
    {   return;
    }
    SCParser *cfg = settings->cfg;
    UserSettings *s = settings;
    i32 i;
    const SCSetting *usdata = __USER__SETTINGS__DATA__;

    for(i = 0; i < UserSettingsLAST; ++i)
    {   
        SCParserSaveVar(cfg, usdata->name, ((uint8_t *)s) + usdata->offset);
        ++usdata;
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
    if(!settings->cfg)
    {   return;
    }
    SCParser *cfg = settings->cfg;
    SCParserDestroy(cfg);
    free(cfg);
    memset(settings, 0, sizeof(UserSettings));
}
