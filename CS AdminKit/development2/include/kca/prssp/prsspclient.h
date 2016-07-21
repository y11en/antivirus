/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	prssp/prsspclient.h
 * \author	Mikhail Karmazine
 * \date	16.04.2003 12:56:52
 * \brief	
 * 
 */


#ifndef __KLPRSSPCLIENT_H__
#define __KLPRSSPCLIENT_H__

#include <kca/prss/settingsstorage.h>

namespace KLPRSSP
{
    class SettingsStorageWithApply : public KLPRSS::SettingsStorage
    {
	public:
        virtual void ApplyChanges() = 0;
    };
}

KLCSC_DECL void KLPRSSP_CreateSettingsStorageWithApply(
    const std::wstring&	        wstrSSId,
	const std::wstring&	        wstrLocalComponentName,
	const std::wstring&	        wstrConnName,
    KLPRSSP::SettingsStorageWithApply**   ppSettingsStorageWithApply );

KLCSC_DECL void KLPRSSP_CreateSettingsStorageWithApply(
    const std::wstring&	        wstrSSId,
	const std::wstring&         wstrAddress,
    KLPRSSP::SettingsStorageWithApply**   ppSettingsStorageWithApply,
	const unsigned short*	    pPorts = NULL );

KLCSC_DECL void KLPRSSP_CreateDummySettingsStorageWithApply(
    KLPRSS::SettingsStorage * pSettingsStorage,
    KLPRSSP::SettingsStorageWithApply**   ppSettingsStorageWithApply );

#endif //__KLPRSSPCLIENT_H__
