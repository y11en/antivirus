#include "loggermod.h"
#include "g_precomp.h"
#pragma hdrstop

CKl_LoggerModule::CKl_LoggerModule()
{
    CKl_KlickPlugin*    NewPlugin	= NULL;
	PluginModuleVer				= MOD_VER;

    ADD_PLUGIN( CKl_PacketLogger,   LOGGER_PLUGIN_NAME   );

	Register( LOGGER_PLUGIN_NAME,			KLICK_BASE_DRV );
    Pause();
}

CKl_LoggerModule::~CKl_LoggerModule()
{
    // все почистится в базовом классе ....
}