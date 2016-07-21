// msi_scm.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "msi_scm.h"
//#include "sa.h"
#include <aclapi.h>
#pragma warning(disable : 4786)
#include "../common/ca_misc.h"
#include "../common/ca_data_parser.h"

//////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
class ServiceRegisterParams
{
public:
	ServiceRegisterParams(const tstring& sData);
	bool IsValid() const				{ return m_isValid; }
	const tstring& Name() const			{ return m_name; }
	const tstring& Path() const			{ return m_path; }
	const tstring& DisplayName() const  { return m_displayName; }
	const tstring& Description() const  { return m_description; }
	DWORD ServiceType() const			{ return m_serviceType; }
	DWORD StartType() const				{ return m_startType; }
	DWORD ErrorControl() const			{ return m_errorControl; }
	bool StartAfterInstall() const		{ return m_startAfterInstall; }
private:
	static bool GetStartAfterInstall(const tstring& sValue, bool& result);
	static bool GetServiceType(const tstring& sValue, DWORD& result);
	static bool GetStartType(const tstring& sValue, DWORD& result);
	static bool GetErrorControl(const tstring& sValue, DWORD& result);
private:
	bool	m_isValid;
	tstring m_name;
	tstring m_path;
	tstring m_displayName;
	tstring m_description;
	DWORD	m_serviceType;
	DWORD	m_startType;
	DWORD	m_errorControl;
	bool	m_startAfterInstall;
};

//////////////////////////////////////////////////////////////////////////
ServiceRegisterParams::ServiceRegisterParams(const tstring& sData)
	: m_isValid(false)
	, m_serviceType(0)
	, m_startType(0)
	, m_startAfterInstall(false)
	, m_errorControl(SERVICE_ERROR_NORMAL)
{
	using namespace MsiSupport::Utility;

	CADataParser<TCHAR> parser;
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
DWORD RegisterDriverImpl(const ServiceRegisterParams& service_params)
{
	DWORD dwRet = ERROR_INSTALL_FAILURE;
	DWORD dwErr = -1;
	
	//MessageBox(NULL, "CreateService", "CreateService", MB_OK);
	SC_HANDLE schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!schSCManager)
	{
		dwErr = GetLastError();
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
				dwErr = ::GetLastError();
			}	

		}

		if (schService)
		{
			dwRet = ERROR_SUCCESS;

			/*if (StartService(schService, 0, NULL) == 0)	
			{
				dwErr = ::GetLastError();				
			}*/

			::CloseServiceHandle(schService);
		}
		else
		{
			dwErr = ::GetLastError();
		}
		
		::CloseServiceHandle(schSCManager);
		
	}

	return dwRet;
}

//////////////////////////////////////////////////////////////////////////
MSI_SCM_API UINT __stdcall RegisterDriver(MSIHANDLE hInstall)
{
	using namespace MsiSupport::Utility;
	tstring sData;

	//MessageBox(NULL, "RegisterDriver", "RegisterDriver", MB_OK);

	if (CAGetData(hInstall, sData))
	{
		ServiceRegisterParams service_params(sData);
		if (service_params.IsValid())
		{
			if (RegisterDriverImpl(service_params) == ERROR_SUCCESS)
			{
				return ERROR_SUCCESS;
			}
		}
	}
	return ERROR_INSTALL_FAILURE;
}

//////////////////////////////////////////////////////////////////////////
UINT UnregisterDriverImpl(const tstring& sName)
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
MSI_SCM_API UINT __stdcall UnregisterDriver(MSIHANDLE hInstall)
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
				res = UnregisterDriverImpl(sName);
			}
		}
		return res;
	}
	else
	{
		return ERROR_INSTALL_FAILURE;
	}
}


//////////////////////////////////////////////////////////////////////////
UINT StartDriverImpl(const tstring& sName)
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
			if (::StartService(schService, 0, NULL))
			{
				dwRet = ERROR_SUCCESS;
			}
			else
			{
				dwErr = GetLastError();
			}
			::CloseServiceHandle(schService);
		}
		::CloseServiceHandle(schSCManager);
	}

	return dwRet;
}

//////////////////////////////////////////////////////////////////////////
MSI_SCM_API UINT __stdcall StartDriver(MSIHANDLE hInstall)
{
	using namespace MsiSupport::Utility;
	tstring sData;
	
	//MessageBox(NULL, "StartDriver", "StartDriver", MB_OK);

	if (CAGetData(hInstall, sData))
	{
		CADataParser<TCHAR> parser;
		UINT res = ERROR_INSTALL_FAILURE;
		if (parser.parse(sData))
		{
			const tstring& sName = parser.value(TEXT("Name"));
			if (!sName.empty())
			{
				res = StartDriverImpl(sName);
			}
		}
		return res;
	}
	else
	{
		return ERROR_INSTALL_FAILURE;
	}
}

#define DisplayError(error, name) {errResult = error; _DisplayError(errResult, name);}
#define _trace OutputDebugString
#define LOCAL_USERS_RIGHTS	(READ_CONTROL | SERVICE_ENUMERATE_DEPENDENTS | SERVICE_INTERROGATE | SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS | SERVICE_USER_DEFINED_CONTROL)

void _DisplayError(DWORD dwError, LPTSTR pszAPI)
{
	LPVOID lpvMessageBuffer;
	
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpvMessageBuffer, 0, NULL);
	
	// Display the string.
	_trace(pszAPI);
	_trace(": ");
	_trace((LPTSTR)lpvMessageBuffer);
	_trace("\n");
	
	// Free the buffer allocated by the system.
	LocalFree(lpvMessageBuffer);
}


#define SA_AUTHORITY_EVERYONE	SECURITY_WORLD_RID

DWORD __stdcall SetServiceDACLImpl(LPCTSTR szServiceName, LPCTSTR szUserGroupName, DWORD dwAccessRights)
{
	BOOL                 bDaclPresent   = FALSE;
	BOOL                 bDaclDefaulted = FALSE;
	DWORD                dwError        = 0;
	DWORD                dwSize         = 0;
	EXPLICIT_ACCESS      ea;
	PACL                 pacl           = NULL;
	PACL                 pNewAcl        = NULL;
	PSECURITY_DESCRIPTOR psd            = NULL;
	SC_HANDLE            schManager     = NULL;
	SC_HANDLE            schService     = NULL;
	SECURITY_DESCRIPTOR  sd;
	DWORD                errResult = ERROR_SUCCESS;

//	if (!szUserGroupName)
//		szUserGroupName = TEXT("EVERYONE");
	if (!dwAccessRights)
		dwAccessRights = LOCAL_USERS_RIGHTS | SERVICE_START;
	
	_trace("SetServiceDACL\n");
	_trace("Service: "); _trace(szServiceName);_trace("\n");
	_trace("Group: "); _trace(szUserGroupName ? szUserGroupName : "<default>");_trace("\n");

	// Obtain a handle to the Service Controller.
	schManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (NULL == schManager)
	{
		DisplayError(GetLastError(), TEXT("OpenSCManager"));
	}
	else
	{
		// Obtain a handle to the service.
		schService = OpenService(schManager, szServiceName, READ_CONTROL | WRITE_DAC);
		if (NULL == schService)
			DisplayError(GetLastError(), TEXT("OpenService"))
		else
		{
			// Get the current security descriptor.
			if (!QueryServiceObjectSecurity(schService, DACL_SECURITY_INFORMATION, (void*)1, 0, &dwSize))
			{
				DWORD error = GetLastError();
				if (ERROR_INSUFFICIENT_BUFFER != error)
					DisplayError(error, TEXT("QueryServiceObjectSecurity"))
				else
				{
					psd = (PSECURITY_DESCRIPTOR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize);
					if (psd == NULL)
						DisplayError(0, TEXT("HeapAlloc")) // note HeapAlloc does not support GetLastError()
					else
					{
						if (!QueryServiceObjectSecurity(schService, DACL_SECURITY_INFORMATION, psd, dwSize, &dwSize))
						{
							HeapFree(GetProcessHeap(), 0, psd);
							psd = NULL;
							DisplayError(GetLastError(), TEXT("QueryServiceObjectSecurity"))
						}
					}
				}
			}
			
			if (psd)
			{
				// Get the DACL.
				if (!GetSecurityDescriptorDacl(psd, &bDaclPresent, &pacl, &bDaclDefaulted))
					DisplayError(GetLastError(), TEXT("GetSecurityDescriptorDacl"))
				else
				{
					// Build the ACE.
					SID_IDENTIFIER_AUTHORITY authWorld = SECURITY_WORLD_SID_AUTHORITY;
					SID* psid;
					if(!AllocateAndInitializeSid(&authWorld, 1, SA_AUTHORITY_EVERYONE, 0, 0, 0, 0, 0, 0, 0, (void**)&psid))
						DisplayError(GetLastError(), TEXT("AllocateAndInitializeSid"))
					else
					{
						if (szUserGroupName)
						{
							BuildExplicitAccessWithName(&ea, (LPTSTR)szUserGroupName, dwAccessRights, SET_ACCESS, NO_INHERITANCE);
						}
						else
						{
							ea.grfAccessPermissions = dwAccessRights;
							ea.grfAccessMode = SET_ACCESS;
							ea.grfInheritance = NO_INHERITANCE;
							BuildTrusteeWithSid(&ea.Trustee, psid);
						}

						dwError = SetEntriesInAcl(1, &ea, pacl, &pNewAcl);
						if (dwError != ERROR_SUCCESS)
							DisplayError(dwError, TEXT("SetEntriesInAcl"))
						else
						{
							// Initialize a NEW Security Descriptor.
							if (!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
								DisplayError(GetLastError(), TEXT("InitializeSecurityDescriptor"))
							else
							{
								// Set the new DACL in the Security Descriptor.
								if (!SetSecurityDescriptorDacl(&sd, TRUE, pNewAcl, FALSE))
									DisplayError(GetLastError(), TEXT("SetSecurityDescriptorDacl"))
								else
								{
									// Set the new DACL for the service object.
									if (!SetServiceObjectSecurity(schService, DACL_SECURITY_INFORMATION, &sd))
										DisplayError(GetLastError(), TEXT("SetServiceObjectSecurity"))
									else
										_trace("SetServiceDACL succeeded\n");
								}
							}
							LocalFree((HLOCAL)pNewAcl);
						}
						FreeSid(psid);
					}
				}
				HeapFree(GetProcessHeap(), 0, (LPVOID)psd);
			}
			CloseServiceHandle(schService);
		}
		CloseServiceHandle(schManager);
	}
	return errResult;
}


	//////////////////////////////////////////////////////////////////////////
MSI_SCM_API UINT __stdcall SetServiceDACL(MSIHANDLE hInstall)
{
	using namespace MsiSupport::Utility;
	tstring sData;
	
	//MessageBox(NULL, "SetServiceDACL", "SetServiceDACL", MB_OK);

	if (CAGetData(hInstall, sData))
	{
		CADataParser<TCHAR> parser;
		UINT res = ERROR_INSTALL_FAILURE;
		if (parser.parse(sData))
		{
			const tstring& sName = parser.value(TEXT("Name"));
			if (!sName.empty())
			{
				res = SetServiceDACLImpl(sName.c_str(), NULL, NULL);
			}
		}
		return res;
	}
	else
	{
		return ERROR_INSTALL_FAILURE;
	}
}
