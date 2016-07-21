#include "stdafx.h"
#include "msi_misc.h"
#include "../common/ca_misc.h"
#include "../common/ca_data_parser.h"

//////////////////////////////////////////////////////////////////////////
class ServiceRegisterParams
{
public:
	ServiceRegisterParams(MSIHANDLE hInstall, const cstring& sData);
	bool IsValid() const				{ return m_isValid; }
	const cstring& Name() const			{ return m_name; }
	const cstring& Path() const			{ return m_path; }
	const cstring& DisplayName() const  { return m_displayName; }
	const cstring& Description() const  { return m_description; }
	DWORD ServiceType() const			{ return m_serviceType; }
	DWORD StartType() const				{ return m_startType; }
	DWORD ErrorControl() const			{ return m_errorControl; }
	bool StartAfterInstall() const		{ return m_startAfterInstall; }
private:
	static bool GetStartAfterInstall(const cstring& sValue, bool& result);
	static bool GetServiceType(const cstring& sValue, DWORD& result);
	static bool GetStartType(const cstring& sValue, DWORD& result);
	static bool GetErrorControl(const cstring& sValue, DWORD& result);
private:
	bool	m_isValid;
	cstring m_name;
	cstring m_path;
	cstring m_displayName;
	cstring m_description;
	DWORD	m_serviceType;
	DWORD	m_startType;
	DWORD	m_errorControl;
	bool	m_startAfterInstall;
};

//////////////////////////////////////////////////////////////////////////
ServiceRegisterParams::ServiceRegisterParams(MSIHANDLE hInstall, const cstring& sData)
	: m_isValid(false)
	, m_serviceType(0)
	, m_startType(0)
	, m_startAfterInstall(false)
	, m_errorControl(SERVICE_ERROR_NORMAL)
{
	using namespace MsiSupport::Utility;

	CADataParser<char> parser;
	if (parser.parse(sData))
	{
		m_name = parser.value("Name");
		m_path = parser.value("Path");
		m_displayName = parser.value("DisplayName");
		if (m_displayName.empty())
			m_displayName = m_name;
		m_description = parser.value("Description");
		m_isValid = 
			(
				!m_name.empty() && 
				!m_path.empty() &&
				GetStartAfterInstall(
						parser.value("StartOnInstall"), 
						m_startAfterInstall
						) &&
				GetServiceType(
						parser.value("ServiceType"),
						m_serviceType
						) &&
				GetStartType(
						parser.value("StartType"),
						m_startType
						) &&
				GetErrorControl(
						parser.value("ErrorControl"),
						m_errorControl
						)
			);
	}
	else
	{
		//MessageBox(NULL, NULL, szResult, MB_OK);
		installLog(hInstall, "ServiceRegisterParams parsing cadata %s failed", sData.c_str());
	}

}

//////////////////////////////////////////////////////////////////////////
bool ServiceRegisterParams::GetStartAfterInstall(const cstring& sValue, bool& result)
{
	if (sValue.empty() || sValue == "no" || sValue == "0")
		result = false;
	else if (sValue == "yes" || sValue == "1")
		result = true;
	else
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ServiceRegisterParams::GetServiceType(const cstring& sValue, DWORD& result)
{
	if (sValue == "kernelDriver")
		result = SERVICE_KERNEL_DRIVER;
	else if (sValue == "systemDriver")
		result = SERVICE_FILE_SYSTEM_DRIVER;
	else if (sValue == "ownProcess")
		result = SERVICE_WIN32_OWN_PROCESS;
	else if (sValue == "shareProcess")
		result = SERVICE_WIN32_SHARE_PROCESS;
	else
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ServiceRegisterParams::GetStartType(const cstring& sValue, DWORD& result)
{
	if (sValue == "auto")
		result = SERVICE_AUTO_START;
	else if (sValue == "system")
		result = SERVICE_SYSTEM_START;
	else if (sValue == "boot")
		result = SERVICE_BOOT_START;
	else if (sValue == "demand")
		result = SERVICE_DEMAND_START;
	else if (sValue == "disabled")
		result = SERVICE_DISABLED;
	else
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool ServiceRegisterParams::GetErrorControl(const cstring& sValue, DWORD& result)
{
	if (sValue.empty() || sValue == "normal")
		result = SERVICE_ERROR_NORMAL;
	else if (sValue == "ignore")
		result = SERVICE_ERROR_IGNORE;
	else if (sValue == "critical")
		result = SERVICE_ERROR_CRITICAL;
	else if (sValue == "severe")
		result = SERVICE_ERROR_SEVERE;
	else
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
DWORD RegisterDriverImpl(MSIHANDLE hInstall, const ServiceRegisterParams& service_params)
{
	DWORD dwRet = ERROR_SUCCESS;
	
	//MessageBox(NULL, "RegisterDriverImpl", "RegisterDriverImpl", MB_OK);
	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schSCManager)
	{
        dwRet = GetLastError();
        installLog(hInstall, "RegisterDriver: OpenSCManager failed: 0x%08X", dwRet);
	}
	else
	{
		SC_HANDLE schService = ::OpenService(
								schSCManager, 
								service_params.Name().c_str(), 
								SERVICE_ALL_ACCESS
								);
		if (!schService)
		{			
			//MessageBox(NULL, "CreateService", "CreateService", MB_OK);
	 		schService = ::CreateService(
											schSCManager, 
											service_params.Name().c_str(), 
											service_params.DisplayName().c_str(), 
											SERVICE_ALL_ACCESS, 
											service_params.ServiceType(), 
											service_params.StartType(), 
											service_params.ErrorControl(), 
											service_params.Path().c_str(), 
											NULL, 
											NULL, 
											NULL, 
											NULL, 
											NULL
											);

            if (!schService)
            {
                dwRet = GetLastError();
                installLog(hInstall, "RegisterDriver: CreateService(%s,%s,%d,%d,%d,%s) failed: 0x%08X", 
                    service_params.Name().c_str(), 
                    service_params.DisplayName().c_str(),
                    service_params.ServiceType(),
                    service_params.StartType(),
                    service_params.ErrorControl(),
                    service_params.Path().c_str(),
                    dwRet);
            }
		}
		else
		{
			//MessageBox(NULL, "CreateService", "ChangeServiceConfig", MB_OK);
			if (ChangeServiceConfig(
							schService, 
							service_params.ServiceType(),
							service_params.StartType(),
							service_params.ErrorControl(),
							service_params.Path().c_str(),
							NULL,
							NULL,
							NULL,
							NULL,
							NULL,
							service_params.DisplayName().c_str()
							) == 0)
			{
                dwRet = GetLastError();
                installLog(hInstall, "RegisterDriver: ChangeServiceConfig(%s,%d,%d,%d,%s,%s) failed: 0x%08X", 
                    service_params.Name().c_str(), 
                    service_params.ServiceType(),
                    service_params.StartType(),
                    service_params.ErrorControl(),
                    service_params.Path().c_str(),
                    service_params.DisplayName().c_str(),
                    dwRet);
			}	
        }

	    if (schService)
	    {
            // description

            //MessageBox(NULL, "::LoadLibrary", "RegisterDriverImpl", MB_OK);
            HMODULE hLib = ::LoadLibrary("advapi32.dll");
            if (hLib)
            {
                typedef BOOL (__stdcall *tChangeServiceConfig2)(SC_HANDLE,DWORD,LPVOID);

                //MessageBox(NULL, "::GetProcAddress", "RegisterDriverImpl", MB_OK);
                tChangeServiceConfig2 pChangeServiceConfig2 = (tChangeServiceConfig2)::GetProcAddress(hLib, "ChangeServiceConfig2A");

                if (pChangeServiceConfig2)
                {
                    SERVICE_DESCRIPTION sd = {(char*)service_params.Description().c_str()};

                    //MessageBox(NULL, "::ChangeServiceConfig2", "RegisterDriverImpl", MB_OK);
                    if (!pChangeServiceConfig2(schService,
                                          SERVICE_CONFIG_DESCRIPTION,
                                          &sd))
                    {
                        dwRet = GetLastError();
                        installLog(hInstall, "RegisterDriver: ChangeServiceConfig2(%s,%s) failed: 0x%08X", 
                            service_params.Name().c_str(),
                            (char*)service_params.Description().c_str(),
                            dwRet);
                    }
                }
                //MessageBox(NULL, "::FreeLibrary", "RegisterDriverImpl", MB_OK);
                FreeLibrary(hLib);
            }

			::CloseServiceHandle(schService);
		}
				
		::CloseServiceHandle(schSCManager);
		
	}

	return dwRet;
}

//////////////////////////////////////////////////////////////////////////
MSI_MISC_API UINT __stdcall RegisterDriver(MSIHANDLE hInstall)
{
    DWORD dwErr = ERROR_SUCCESS;
	using namespace MsiSupport::Utility;
	cstring sData;

	//MessageBox(NULL, "RegisterDriver", "RegisterDriver", MB_OK);

	if (CAGetData(hInstall, sData))
	{
		ServiceRegisterParams service_params(hInstall, sData);
		
		if (service_params.IsValid())
		{
            installLog(hInstall, "RegisterDriver: %s,%s,%d,%d,%d,%s", 
                    service_params.Name().c_str(), 
                    service_params.DisplayName().c_str(),
                    service_params.ServiceType(),
                    service_params.StartType(),
                    service_params.ErrorControl(),
                    service_params.Path().c_str());

            dwErr = RegisterDriverImpl(hInstall, service_params);
			if (dwErr == ERROR_SUCCESS)
			{
				return ERROR_SUCCESS;
			}
		}
        else
            installLog(hInstall, "RegisterDriver: service parameters are invalid (%s)", sData.c_str());

        char *msg = GetLastErrorStr(dwErr);
		ReportDeferredCAError(hInstall, "RegisterDriver", 100, msg);
        FreeLastErrorStr(msg);
	}
    else
        installLog(hInstall,"RegisterDriver: getting cadata failed");

	return ERROR_INSTALL_FAILURE;
}

//////////////////////////////////////////////////////////////////////////
DWORD UnregisterDriverImpl(MSIHANDLE hInstall, const cstring& sName)
{
	DWORD dwRet = ERROR_SUCCESS;

	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schSCManager)
	{
        dwRet = GetLastError();
        installLog(hInstall, "UnregisterDriver: OpenSCManager failed: 0x%08X", dwRet);
	}
	else
	{
		SC_HANDLE schService = ::OpenService(schSCManager, sName.c_str(), SERVICE_ALL_ACCESS);
		if (schService)
		{
			if (! ::DeleteService(schService))
			{
                DWORD dwRet = GetLastError ();

				if (ERROR_SERVICE_MARKED_FOR_DELETE == dwRet)
					dwRet = ERROR_SUCCESS;
                else
                    installLog(hInstall, "UnregisterDriver: DeleteService(%s) failed: 0x%08X", sName.c_str(), dwRet);
			}
			::CloseServiceHandle(schService);
		}
		::CloseServiceHandle(schSCManager);
	}

	return dwRet;
}

//////////////////////////////////////////////////////////////////////////
MSI_MISC_API UINT __stdcall UnregisterDriver(MSIHANDLE hInstall)
{
	using namespace MsiSupport::Utility;
	cstring sData;
	if (CAGetData(hInstall, sData))
	{
		CADataParser<char> parser;
		UINT res = ERROR_INSTALL_FAILURE;
		if (parser.parse(sData))
		{
			const cstring& sName = parser.value("Name");
			if (!sName.empty())
			{
                installLog(hInstall, "UnregisterDriver: %s", sName.c_str()); 

				res = UnregisterDriverImpl(hInstall, sName);
			}
		    return res;
		}
        else
        {
            installLog(hInstall,"RegisterDriver: parsing cadata failed: %s", sData.c_str());
		    return ERROR_INSTALL_FAILURE;
        }
	}
	else
	{
        installLog(hInstall,"RegisterDriver: getting cadata failed");
		return ERROR_INSTALL_FAILURE;
	}
}


//////////////////////////////////////////////////////////////////////////
DWORD StartDriverImpl(MSIHANDLE hInstall, const cstring& sName)
{
	DWORD dwRet = ERROR_SUCCESS;

	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schSCManager)
    {
        dwRet = GetLastError();
        installLog(hInstall, "StartDriver: OpenSCManager failed: 0x%08X", dwRet);
    }
	else
	{
		SC_HANDLE schService = ::OpenService(schSCManager, sName.c_str(), SERVICE_ALL_ACCESS);
		if (schService)
		{
			if (! ::StartService(schService, 0, NULL))
            {
                dwRet = GetLastError();
                installLog(hInstall, "StartDriver: StartService(%s) failed: 0x%08X", sName.c_str(), dwRet);
            }

			::CloseServiceHandle(schService);
		}
        else
        {
            dwRet = GetLastError();
            installLog(hInstall, "StartDriver: OpenService(%s) failed: 0x%08X", sName.c_str(), dwRet);
        }

		::CloseServiceHandle(schSCManager);
	}

	return dwRet;
}

//////////////////////////////////////////////////////////////////////////
MSI_MISC_API UINT __stdcall StartDriver(MSIHANDLE hInstall)
{
	using namespace MsiSupport::Utility;
	cstring sData;
	
	//MessageBox(NULL, "StartDriver", "StartDriver", MB_OK);

	if (CAGetData(hInstall, sData))
	{
		CADataParser<char> parser;
		UINT res = ERROR_INSTALL_FAILURE;
		if (parser.parse(sData))
		{
			const cstring& sName = parser.value("Name");
			if (!sName.empty())
			{
				res = StartDriverImpl(hInstall, sName);
			}
		}
		return res;
	}
	else
	{
		return ERROR_INSTALL_FAILURE;
	}
}
