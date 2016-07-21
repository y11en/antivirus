#ifndef _INTERNALNHPROVIDEDCONFIGURATORINTERFACES_H_
#define _INTERNALNHPROVIDEDCONFIGURATORINTERFACES_H_

#include <srvp/hst/hosts.h>
#include <srvp/pkg/packages.h>

namespace KLPlugin
{

// IAdditionalInfo::GetNHInfo returned params value names
const wchar_t c_spConnectedServerAddress[] = L"ConnectedServerAddress";		// STRING_T
const wchar_t c_spConnectedServerPort[] = L"ConnectedServerPort";			// INT_T
const wchar_t c_spConnectedServerSSLPort[] = L"ConnectedServerSSLPort";		// INT_T
const wchar_t c_spGlobalTask[] = L"GlobalTask";								// BOOL_T
const wchar_t c_spComputerType[] = L"ComputerType";							// INT_T (see KLWNF::ComputerType)
const wchar_t c_spComputerAddress[] = L"ComputerAddress";					// STRING_T

#define KAV_NH_INTERFACE_ADDITIONAL_INFO			L"NH_ADDITIONAL_INFO"
#define KAV_NH_INTERFACE_ISERVERSETTINGS			L"NH_ISERVERSETTINGS"

interface IAdditionalInfo
{
	virtual void GetNHInfo(KLPAR::Params** pparInfo) PURE;
};

};

#endif // _INTERNALNHPROVIDEDCONFIGURATORINTERFACES_H_
