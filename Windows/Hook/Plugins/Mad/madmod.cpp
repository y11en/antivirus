#include "madmod.h"
#include "g_precomp.h"
#pragma hdrstop

CKl_MadModule::CKl_MadModule()
{
    CKl_KlickPlugin*    NewPlugin	= NULL;
	PluginModuleVer				= MOD_VER;

    ADD_PLUGIN( CKl_Mad, "Make Attack Plugin");

	Register( "Make Attack Plugin",			KLICK_BASE_DRV );

}

CKl_MadModule::~CKl_MadModule()
{
    // все почистится в базовом классе ....
}