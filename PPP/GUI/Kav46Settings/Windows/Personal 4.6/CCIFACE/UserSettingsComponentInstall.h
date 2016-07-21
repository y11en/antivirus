#ifndef __USERSETTINGS_COMPONENT_INSTALL_H__
#define __USERSETTINGS_COMPONENT_INSTALL_H__

#include "DEFS.H"
#include "BaseMarshal.h"
#include "../release_type.h"

struct CInstallFailedNotification : public CCBaseMarshal<PID_CCCMD_INSTALL_FAILED_NOTIFICATION>
{
	enum EInstallStatus{
			eFailed, 
			eOK, 
			eNeedRestartAppQuestion, 
			eNeedRebootOSQuestion, 
			eForceRestartApp, 
			eForceRebootOS, 
			eLast};

	CInstallFailedNotification() : m_eComponent(CInterfaceCCServiceIPCServer::eLast), m_eStatus(eLast), m_dwLastError(0){}
	CInstallFailedNotification(CInterfaceCCServiceIPCServer::EComponents eComponent, EInstallStatus eStatus, DWORD dwErr = 0)
		: m_eComponent(eComponent),
		m_eStatus(eStatus),
		m_dwLastError(dwErr){}
	
	PoliType &MarshalImpl(PoliType &pt)
	{
		return BaseMarshal::MarshalImpl(pt) << SIMPLE_DATA_SER(m_eComponent) << SIMPLE_DATA_SER(m_eStatus) << m_dwLastError;
	}
	
	CInterfaceCCServiceIPCServer::EComponents m_eComponent;
	EInstallStatus m_eStatus;
	DWORD m_dwLastError;
};

#endif //__USERSETTINGS_COMPONENT_INSTALL_H__