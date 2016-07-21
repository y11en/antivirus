// msi_scm.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "msi_scm.h"
#include <sstream>
#include <algorithm>
#include <tchar.h>
#pragma warning(disable : 4786)
#include "../common/ca_misc.h"
#include "../common/ca_data_parser.h"
#include "../common/ca_logging.h"

//////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
class OSDetect
{
public:
	OSDetect()
	{
		ZeroMemory(&m_info, sizeof(OSVERSIONINFO));
		m_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		m_isValid = (GetVersionEx(&m_info) ? true : false);
	}
	bool IsValid() const
	{
		return m_isValid;
	}
	bool IsPlatform9x() const  // Windows 95, Windows 98, or Windows Me
	{
		return (m_info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS);
	}
	bool IsPlatformNT() const  // Windows NT, Windows 2000, Windows XP, or Windows .NET Server 2003 family
	{
		return (m_info.dwPlatformId == VER_PLATFORM_WIN32_NT);
	}
	bool IsWin2000OrLater() const  // Windows 2000, Windows XP or Windows .NET Server 2003 family
	{
		return ((m_info.dwMajorVersion > 5) || (m_info.dwMajorVersion == 5 && m_info.dwMinorVersion >= 0));
	}
	bool IsWinXPOrLater() const  // Windows XP or Windows .NET Server 2003 family
	{
		return ((m_info.dwMajorVersion > 5) || (m_info.dwMajorVersion == 5 && m_info.dwMinorVersion > 0));
	}
private:
	bool m_isValid;
	OSVERSIONINFO m_info;
};

//////////////////////////////////////////////////////////////////////////
class ServiceRegisterParams
{
public:
	ServiceRegisterParams(const tstring& sData);
	bool IsValid() const					{ return m_isValid; }
	const tstring& Name() const				{ return m_name; }
	const tstring& Path() const				{ return m_path; }
	bool DoNotQuotePath() const				{ return m_doNotQuotePath; }
	const tstring& DisplayName() const		{ return m_displayName; }
	const tstring& Description() const		{ return m_description; }
	const tstring& Dependencies() const		{ return m_dependencies; }
	DWORD ServiceType() const				{ return m_serviceType; }
	DWORD StartType() const					{ return m_startType; }
	DWORD ErrorControl() const				{ return m_errorControl; }
	DWORD FailureCountResetPeriod() const	{ return m_failureCountResetPeriod; }
	const SC_ACTION& FailureAction1() const	{ return m_failureAction1; }
	const SC_ACTION& FailureAction2() const	{ return m_failureAction2; }
	const SC_ACTION& FailureAction3() const	{ return m_failureAction3; }
	bool StartAfterInstall() const			{ return m_startAfterInstall; }
private:
	static bool GetStartAfterInstall(const tstring& sValue, bool& result);
	static bool GetServiceType(const tstring& sValue, DWORD& result);
	static bool GetStartType(const tstring& sValue, DWORD& result);
	static bool GetErrorControl(const tstring& sValue, DWORD& result);
	static bool GetFailureAction(const tstring& sValue, SC_ACTION& result);
	static bool GetDependencies(const tstring& sValue, tstring& result);
	static bool GetDoNotQuotePath(const tstring& sValue, bool& result);
private:
	bool	m_isValid;
	tstring m_name;
	tstring m_path;
	bool	m_doNotQuotePath;
	tstring m_displayName;
	tstring m_description;
	tstring m_dependencies;
	DWORD	m_serviceType;
	DWORD	m_startType;
	DWORD	m_errorControl;
	DWORD	m_failureCountResetPeriod;
	SC_ACTION m_failureAction1;
	SC_ACTION m_failureAction2;
	SC_ACTION m_failureAction3;
	bool	m_startAfterInstall;
};

//////////////////////////////////////////////////////////////////////////
ServiceRegisterParams::ServiceRegisterParams(const tstring& sData)
	: m_isValid(false)
	, m_doNotQuotePath(false)
	, m_serviceType(0)
	, m_startType(0)
	, m_startAfterInstall(false)
	, m_errorControl(SERVICE_ERROR_NORMAL)
	, m_failureCountResetPeriod(INFINITE)
{
	m_failureAction1.Type = 
		m_failureAction2.Type = 
		m_failureAction3.Type = SC_ACTION_NONE;
	m_failureAction1.Delay = 
		m_failureAction2.Delay = 
		m_failureAction3.Delay = 0L;

	using namespace MsiSupport::Utility;

	CADataParser<TCHAR> parser;
	if (parser.parse(sData))
	{
		m_name = parser.value(TEXT("Name"));
		m_path = parser.value(TEXT("Path"));
		m_displayName = parser.value(TEXT("DisplayName"));
		if (m_displayName.empty())
			m_displayName = m_name;
		m_description = parser.value(TEXT("Description"));
		{
			tstring tmp(parser.value(TEXT("FailureCountResetPeriod")));
			if (!tmp.empty())
				m_failureCountResetPeriod = _ttol(tmp.c_str());
		}
		m_isValid = 
			(
				!m_name.empty() && 
				!m_path.empty() &&
				GetDoNotQuotePath(
						parser.value(TEXT("DoNotQuotePath")), 
						m_doNotQuotePath
						) &&
				GetStartAfterInstall(
						parser.value(TEXT("StartOnInstall")), 
						m_startAfterInstall
						) &&
				GetServiceType(
						parser.value(TEXT("ServiceType")),
						m_serviceType
						) &&
				GetStartType(
						parser.value(TEXT("StartType")),
						m_startType
						) &&
				GetErrorControl(
						parser.value(TEXT("ErrorControl")),
						m_errorControl
						) &&
				GetFailureAction(
						parser.value(TEXT("FailureAction1")),
						m_failureAction1
						) &&
				GetFailureAction(
						parser.value(TEXT("FailureAction2")),
						m_failureAction2
						) &&
				GetFailureAction(
						parser.value(TEXT("FailureAction3")),
						m_failureAction3
						) &&
				GetDependencies(
						parser.value(TEXT("Dependencies")),
						m_dependencies
						)
			);
	}
}

//////////////////////////////////////////////////////////////////////////
bool ServiceRegisterParams::GetStartAfterInstall(const tstring& sValue, bool& result)
{
	if (sValue.empty() || sValue == TEXT("no") || sValue == TEXT("0"))
		result = false;
	else if (sValue == TEXT("yes") || sValue == TEXT("1"))
		result = true;
	else
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ServiceRegisterParams::GetDoNotQuotePath(const tstring& sValue, bool& result)
{
	if (sValue.empty() || sValue == TEXT("no") || sValue == TEXT("0"))
		result = false;
	else if (sValue == TEXT("yes") || sValue == TEXT("1"))
		result = true;
	else
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ServiceRegisterParams::GetServiceType(const tstring& sValue, DWORD& result)
{
	if (sValue == TEXT("kernelDriver"))
		result = SERVICE_KERNEL_DRIVER;
	else if (sValue == TEXT("systemDriver"))
		result = SERVICE_FILE_SYSTEM_DRIVER;
	else if (sValue == TEXT("ownProcess"))
		result = SERVICE_WIN32_OWN_PROCESS;
	else if (sValue == TEXT("shareProcess"))
		result = SERVICE_WIN32_SHARE_PROCESS;
	else
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ServiceRegisterParams::GetStartType(const tstring& sValue, DWORD& result)
{
	if (sValue == TEXT("auto"))
		result = SERVICE_AUTO_START;
	else if (sValue == TEXT("system"))
		result = SERVICE_SYSTEM_START;
	else if (sValue == TEXT("boot"))
		result = SERVICE_BOOT_START;
	else if (sValue == TEXT("demand"))
		result = SERVICE_DEMAND_START;
	else if (sValue == TEXT("disabled"))
		result = SERVICE_DISABLED;
	else
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ServiceRegisterParams::GetErrorControl(const tstring& sValue, DWORD& result)
{
	if (sValue.empty() || sValue == TEXT("normal"))
		result = SERVICE_ERROR_NORMAL;
	else if (sValue == TEXT("ignore"))
		result = SERVICE_ERROR_IGNORE;
	else if (sValue == TEXT("critical"))
		result = SERVICE_ERROR_CRITICAL;
	else if (sValue == TEXT("severe"))
		result = SERVICE_ERROR_SEVERE;
	else
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ServiceRegisterParams::GetFailureAction(const tstring& sValue, SC_ACTION& result)
{
	tstring sAction;
	DWORD delay = 0L;

	tstring::const_iterator itComma = std::find(sValue.begin(), sValue.end(), TEXT(','));
	if (itComma == sValue.end())
	{
		// no delay specified
		sAction = sValue;
	}
	else
	{
		sAction.assign(sValue.begin(), itComma);
		tstring tmp(++itComma, sValue.end());
		if (!tmp.empty())
			delay = _ttol(tmp.c_str());
	}

	SC_ACTION_TYPE action_type = SC_ACTION_NONE;

	if (sAction.empty() || sAction == TEXT("none"))
		action_type = SC_ACTION_NONE;
	else if (sAction == TEXT("restart"))
		action_type = SC_ACTION_RESTART;
	else
		return false;

	result.Type = action_type;
	result.Delay = delay;

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ServiceRegisterParams::GetDependencies(const tstring& sValue, tstring& result)
{
	tstring tmp;
	tstring::const_iterator itCurrent = sValue.begin();

	while (itCurrent != sValue.end())
	{
		tstring::const_iterator itComma = std::find(itCurrent, sValue.end(), TEXT(','));
		tmp.append(itCurrent, itComma).append(1, TEXT('\0'));
		if (itComma != sValue.end())
			++itComma;
		itCurrent = itComma;
	}

	if (!tmp.empty())
		tmp += TEXT('\0');

	result.swap(tmp);

	return true;
}

//////////////////////////////////////////////////////////////////////////
DWORD RegisterServiceImpl(
	const ServiceRegisterParams& service_params,
	const MsiSupport::Utility::CALogWriter& log
	)
{
	DWORD dwRet = ERROR_INSTALL_FAILURE;
	DWORD dwErr = -1;

	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schSCManager)
	{
		dwErr = GetLastError();
		log.Write(TEXT("CARegisterService: can't open SCM."));
	}
	else
	{
		log.Write(TEXT("CARegisterService: SCM opened successfully."));
		tstring sPath;
		if (service_params.DoNotQuotePath())
			sPath.assign(service_params.Path());
		else
			sPath.append(1, TEXT('"')).append(service_params.Path()).append(1, TEXT('"'));
		SC_HANDLE schService = ::CreateService(
										schSCManager, 
										service_params.Name().c_str(), 
										service_params.DisplayName().c_str(), 
										SERVICE_ALL_ACCESS, 
										service_params.ServiceType(), 
										service_params.StartType(), 
										service_params.ErrorControl(), 
										sPath.c_str(), 
										NULL, 
										NULL, 
										(service_params.Dependencies().empty() 
											? NULL 
											: service_params.Dependencies().c_str()), 
										NULL, 
										NULL
										);
		if (schService)
		{
			// write advanced parameters for service (only for Windows 2000 and later)
			if (OSDetect().IsWin2000OrLater())
			{
				// write description
				if (!service_params.Description().empty())
				{
					SERVICE_DESCRIPTION sd;
					sd.lpDescription = const_cast<TCHAR*>(service_params.Description().c_str());
					if (!::ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &sd))
						log.Write(TEXT("CARegisterService: can't assign service description."));
				}
				// write failure actions
				if (service_params.FailureAction1().Type != SC_ACTION_NONE ||
					service_params.FailureAction2().Type != SC_ACTION_NONE ||
					service_params.FailureAction3().Type != SC_ACTION_NONE)
				{
					SC_ACTION actions[3];
					actions[0] = service_params.FailureAction1();
					actions[1] = service_params.FailureAction2();
					actions[2] = service_params.FailureAction3();
					SERVICE_FAILURE_ACTIONS fa;
					fa.dwResetPeriod = service_params.FailureCountResetPeriod();
					fa.lpCommand = NULL;
					fa.lpRebootMsg = NULL;
					fa.cActions = sizeof(actions) / sizeof(actions[0]);
					fa.lpsaActions = actions;
					if (!::ChangeServiceConfig2(schService, SERVICE_CONFIG_FAILURE_ACTIONS, &fa))
						log.Write(TEXT("CARegisterService: can't assign service failure actions."));
				}
			}
			log.Write(TEXT("CARegisterService: service created successfully."));
		}
		else
		{
			dwErr = ::GetLastError();
			if (dwErr == ERROR_SERVICE_EXISTS)
			{
				log.Write(TEXT("CARegisterService: service already exists."));
				schService = ::OpenService(
									schSCManager, 
									service_params.Name().c_str(), 
									SERVICE_ALL_ACCESS
									);
				if (!schService)
				{
					log.Write(TEXT("CARegisterService: can't open service."));
					dwErr = ::GetLastError();
				}
			}
			else
			{
				std::basic_stringstream<TCHAR> ssMessage;
				ssMessage << TEXT("CARegisterService: can't create service (error=0x")
						  << std::hex << dwErr 
						  << TEXT(").");
				log.Write(ssMessage.str());
			}
		}
		if (schService)
		{
			dwRet = ERROR_SUCCESS;
			// start service if needed
			if (service_params.StartAfterInstall())
			{
				if (::StartService(schService, NULL, NULL))
				{
					log.Write(TEXT("CARegisterService: service started successfully."));
				}
				else
				{
					dwRet = ::GetLastError();
					if (dwRet == ERROR_SERVICE_ALREADY_RUNNING)
					{
						log.Write(TEXT("CARegisterService: service already running."));
						dwRet = ERROR_SUCCESS;
					}
					else
					{
						std::basic_stringstream<TCHAR> ssMessage;
						ssMessage << TEXT("CARegisterService: can't start service (error=0x")
								  << std::hex << dwErr 
								  << TEXT(").");
						log.Write(ssMessage.str());
					}
				}
			}
			::CloseServiceHandle(schService);
		}
		::CloseServiceHandle(schSCManager);
	}

	return dwRet;
}

//////////////////////////////////////////////////////////////////////////
MSI_SCM_API UINT __stdcall CARegisterService(MSIHANDLE hInstall)
{
	using namespace MsiSupport::Utility;
	tstring sData;
	if (CAGetData(hInstall, sData))
	{
		CALogWriter log(hInstall);
		ServiceRegisterParams service_params(sData);
		if (service_params.IsValid())
		{
			if (RegisterServiceImpl(service_params, log) == ERROR_SUCCESS)
			{
				return ERROR_SUCCESS;
			}
		}
		else
		{
			log.Write(TEXT("CARegisterService: operation arguments are invalid."));
		}
	}
	return ERROR_INSTALL_FAILURE;
}

//////////////////////////////////////////////////////////////////////////
UINT UnregisterServiceImpl(const tstring& sName)
{
	DWORD dwRet = ERROR_INSTALL_FAILURE;
	DWORD dwErr = -1;

	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schSCManager)
	{
		dwErr = GetLastError();
	}
	else
	{
		SC_HANDLE schService = ::OpenService(schSCManager, sName.c_str(), SERVICE_ALL_ACCESS);
		if (schService)
		{
			if (::DeleteService(schService))
			{
				dwRet = ERROR_SUCCESS;
			}
			else
			{
				dwErr = GetLastError();
				if (ERROR_SERVICE_MARKED_FOR_DELETE == dwErr)
					dwRet = ERROR_SUCCESS;
			}
			::CloseServiceHandle(schService);
		}
		::CloseServiceHandle(schSCManager);
	}

	return dwRet;
}

//////////////////////////////////////////////////////////////////////////
MSI_SCM_API UINT __stdcall CAUnregisterService(MSIHANDLE hInstall)
{
	using namespace MsiSupport::Utility;
	tstring sData;
	UINT res = ERROR_INSTALL_FAILURE;
	if (CAGetData(hInstall, sData))
	{
		CADataParser<TCHAR> parser;
		if (parser.parse(sData))
		{
			const tstring& sName = parser.value(TEXT("Name"));
			if (!sName.empty())
			{
				res = UnregisterServiceImpl(sName);
			}
		}
	}
	return res;
}

//////////////////////////////////////////////////////////////////////////
UINT StartServiceImpl(
	const tstring& sName,
	const MsiSupport::Utility::CALogWriter& log
	)
{
	log.Write(TEXT("StartService: begin processing..."));

	DWORD dwRet = ERROR_INSTALL_FAILURE;
	DWORD dwErr = -1;
	TCHAR ul_conv_buf[33];

	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schSCManager)
	{
		dwRet = GetLastError();
		log.Write(tstring(TEXT("StartService: can't open SCM: 0x")) + ultoa(dwRet, ul_conv_buf, 16));
	}
	else
	{
		log.Write(TEXT("StartService: SCM opened successfully."));
		SC_HANDLE schService = ::OpenService(schSCManager, sName.c_str(), SERVICE_ALL_ACCESS);
		if (schService)
		{
			if (::StartService(schService, NULL, NULL))
			{
				dwRet = ERROR_SUCCESS;
				log.Write(TEXT("StartService: service started successfully."));
			}
			else
			{
				dwRet = GetLastError();
				if (dwRet == ERROR_SERVICE_ALREADY_RUNNING)
				{
					log.Write(TEXT("StartService: service already running."));
					dwRet = ERROR_SUCCESS;
				}
				else
				{
					log.Write(tstring(TEXT("StartService: can't start service: 0x")) + ultoa(dwRet, ul_conv_buf, 16));
				}
			}
			::CloseServiceHandle(schService);
		}
		else
		{
			dwRet = GetLastError();
			log.Write(tstring(TEXT("StartService: can't open service: 0x")) + ultoa(dwRet, ul_conv_buf, 16));
		}
		::CloseServiceHandle(schSCManager);
	}

	log.Write(TEXT("StartService: processing finished."));

	return dwRet;
}

//////////////////////////////////////////////////////////////////////////
MSI_SCM_API UINT __stdcall CAStartService(MSIHANDLE hInstall)
{
	using namespace MsiSupport::Utility;
	CALogWriter log(hInstall);
	tstring sData;
	if (CAGetData(hInstall, sData))
	{
		log.Write(TEXT("StartService: custom action data loaded: ") + sData);
		CADataParser<TCHAR> parser;
		UINT res = ERROR_INSTALL_FAILURE;
		if (parser.parse(sData))
		{
			const tstring& sName = parser.value(TEXT("Name"));
			log.Write(TEXT("StartService: custom action data parsed: Name=") + sName);
			if (!sName.empty())
			{
				res = StartServiceImpl(sName, log);
			}
		}
		return res;
	}
	else
	{
		log.Write(TEXT("StartService: can't get custom action data."));
		return ERROR_INSTALL_FAILURE;
	}
}

//////////////////////////////////////////////////////////////////////////
UINT StopServiceImpl(
	const tstring& sName,
	const MsiSupport::Utility::CALogWriter& log
	)
{
	DWORD dwRet = ERROR_INSTALL_FAILURE;
	DWORD dwErr = -1;

	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schSCManager)
	{
		dwRet = GetLastError();
	}
	else
	{
		SC_HANDLE schService = ::OpenService(schSCManager, sName.c_str(), SERVICE_ALL_ACCESS);
		if (schService)
		{
			SERVICE_STATUS status;
			if (::ControlService(schService, SERVICE_CONTROL_STOP, &status))
			{
				dwRet = ERROR_SUCCESS;
			}
			else
			{
				dwRet = GetLastError();
			}
			::CloseServiceHandle(schService);
		}
		::CloseServiceHandle(schSCManager);
	}

	return dwRet;
}

//////////////////////////////////////////////////////////////////////////
MSI_SCM_API UINT __stdcall CAStopService(MSIHANDLE hInstall)
{
	using namespace MsiSupport::Utility;
	tstring sData;
	if (CAGetData(hInstall, sData))
	{
		CADataParser<TCHAR> parser;
		UINT res = ERROR_INSTALL_FAILURE;
		if (parser.parse(sData))
		{
			const tstring& sName = parser.value(TEXT("Name"));
			if (!sName.empty())
			{
				CALogWriter log(hInstall);
				res = StopServiceImpl(sName, log);
			}
		}
		return res;
	}
	else
	{
		return ERROR_INSTALL_FAILURE;
	}
}
