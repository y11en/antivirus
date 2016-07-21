#ifndef _KL_PE_LOADER_H_
#define _KL_PE_LOADER_H_

#include "kldef.h"
#include "klstatus.h"
#include "kl_file.h"
#include "kl_list.h"
#include "g_registry.h"

#ifndef ISWIN9X
#include "kl_hook.h"
#include "klload.h"
#endif

// инициализация загрузчика
#define LoaderInitialize Init
KLSTATUS
Init( );

// деинициализвция загрузчика ( непонятно зачем ? )
#define LoaderDeinitialize Deinit
VOID
Deinit();

// загрузить модуль в память и вызвать его EntryPoint
#define KlLoadModule Load
KLSTATUS 
Load    ( char*  ModName, wchar_t* ModPath, bool Boot );

// выгрузить модуль из памяти.
#define KlUnloadModule Unload
KLSTATUS
Unload  ( char* ModName );

#define LoadBoot0 MyLoad
KLSTATUS
MyLoad();


#ifndef ISWIN9X
#define FindDriver Find
PDRIVER_OBJECT  Find(PDEVICE_OBJECT   DevObj);
#else
#endif


#define GetModuleName GetName
static
wchar_t* 
GetName(wchar_t* ModPath)
{
    wchar_t* ModName = ModPath;    

    if ( ModPath )
    {   
        ModName += wcslen( ModPath );
        while ( ( ModName != ModPath ) && ( *ModName != L'\\' ) )
            ModName--;

        if ( *ModName == L'\\')
            ModName++;
    }

    return ModName;
}

enum BOOT_STATE
{
    BOOT0 = 0,
    BOOT1,
    BOOT2,
    BOOT3,
    BOOT4
};

static BOOT_STATE   BootState;

#endif