// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  03 July 2006,  11:46 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Alexey Antropov
// File Name   -- startupenum2.cpp
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

// AVP Prague stamp begin( Interface version,  )
#define StartUpEnum2_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "startupenum2.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_root.h>

#include <AV/plugin/p_startupenum2.h>

#include <HOOK/FSDrvLib.h>

#include <windows.h>
// AVP Prague stamp end

#include "..\..\Windows\Hook\HOOK\avpgcom.h"
#include "..\..\Windows\Hook\HOOK\hookspec.h"

bool		g_bIsWin9x = false;

extern void				inithash(void);

void*	pfMemAlloc(hOBJECT hObj,void** ppMem,unsigned int Size);
void	pfMemFree(hOBJECT hObj,void* Obj);
void	GetFlag(tCHAR* chFlag,tDWORD* dwFlag,tDWORD* dwExtFlag);

//extern tBOOL ReadKeyValueStr(HKEY BaseKeyEnter, wchar_t KeyPath[], wchar_t NameValue[],cPrStrW& sValue, bool NeedStr = true);

extern "C" extern DWORD g_TotalInput;
extern "C" extern DWORD g_DupInput;
extern "C" extern DWORD g_TotalParse;
extern "C" extern DWORD g_DupParse;
extern "C" extern DWORD g_MissPars;
extern "C" extern DWORD g_DupMiss;
extern "C" extern DWORD g_TotalOutput;
extern "C" extern DWORD g_DupOutput;


HANDLE
OLD_GetDriverHandle	(
	)
{
	HANDLE hDevice = INVALID_HANDLE_VALUE;
	DWORD dwErr;

	hDevice = CreateFileW( L"\\\\.\\KLIF", 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if (INVALID_HANDLE_VALUE == hDevice)
	{
		dwErr = GetLastError();
		if (!dwErr || dwErr == ERROR_ACCESS_DENIED)
		{
			for (int idx = 0; (idx < 3) && (hDevice == INVALID_HANDLE_VALUE); idx++)
			{
				hDevice = CreateFileW( L"\\\\.\\KLIF", 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
				if (INVALID_HANDLE_VALUE == hDevice)
					Sleep( 100 );
			}
		}
	}
	
	return hDevice;
}

HANDLE
OLD_ClientRegisterImp (
	APP_REGISTRATION* pAppReg,
	HANDLE* phWhistleup,
	HANDLE* phWFChanged,
	char* pWhistleupName,
	char* pChangedName,
	PCLIENT_INFO pClientInfo
	)
{
	HANDLE hDevice = INVALID_HANDLE_VALUE;
	pAppReg->m_InterfaceVersion = HOOK_INTERFACE_NUM;
	pAppReg->m_SyncAddr = GetCurrentProcessId();
	
	hDevice = OLD_GetDriverHandle();

	*phWhistleup = NULL;
	*phWFChanged = NULL;

	if(!(*phWhistleup = CreateEventA(NULL, FALSE, FALSE, pWhistleupName)) || 
		(!(*phWFChanged = CreateEventA(NULL, FALSE, FALSE, pChangedName)))) 
	{
		if (*phWhistleup != NULL)
			CloseHandle( *phWhistleup );
		if (*phWFChanged != NULL)
			CloseHandle( *phWFChanged );
		*phWhistleup = NULL;
		*phWFChanged = NULL;

		return INVALID_HANDLE_VALUE;
	}

	lstrcpyA( pAppReg->m_WhistleUp, pWhistleupName );
	lstrcpyA( pAppReg->m_WFChanged, pChangedName );

	if (INVALID_HANDLE_VALUE != hDevice)
	{
		DWORD dwRet;

		ADDITIONALSTATE AddState = {0};
		DeviceIoControl(hDevice, IOCTLHOOK_GetDriverInfo, NULL, 0, &AddState, sizeof(AddState), &dwRet, NULL);

		if (AddState.DrvFlags & _DRV_FLAG_MINIFILTER && !g_bIsWin9x)
		{
			pAppReg->m_WhistleUp[0] = 0;
			pAppReg->m_WhistleUp[5] = 0;
			*((ULONG*)(pAppReg->m_WhistleUp + 1)) = (ULONG) *phWhistleup;

			pAppReg->m_WFChanged[0] = 0;
			pAppReg->m_WFChanged[5] = 0;
			*((ULONG*)(pAppReg->m_WFChanged + 1)) = (ULONG) *phWFChanged;
		}

		BOOL bRet = DeviceIoControl (
			hDevice,
			IOCTLHOOK_RegistrApp,
			pAppReg,
			sizeof(APP_REGISTRATION),
			pClientInfo,
			sizeof(CLIENT_INFO),
			&dwRet,
			NULL
			);

		if(!bRet)
		{
			CloseHandle( hDevice );
			hDevice = INVALID_HANDLE_VALUE;
		}
	}

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		CloseHandle( *phWhistleup );
		CloseHandle( *phWFChanged );
		*phWhistleup = NULL;
		*phWFChanged = NULL;
	}
	
	return hDevice;
}

HANDLE
OLD_ClientRegister (
	PAPP_REGISTRATION pAppReg,
	PHANDLE phWhistleup,
	PHANDLE phWFChanged)
{
	HANDLE hDevice = INVALID_HANDLE_VALUE;
	pAppReg->m_CurProcId = GetCurrentProcessId();
	pAppReg->m_AppID = _DRIVER_SPECIFIC_ID;
	pAppReg->m_CacheSize = 0;
	pAppReg->m_Priority = AVPG_INFOPRIORITY;
	pAppReg->m_ClientFlags = _CLIENT_FLAG_WITHOUTWATCHDOG;
	pAppReg->m_BlueScreenTimeout = DEADLOCKWDOG_TIMEOUT;

	LARGE_INTEGER qPerfomance;
	QueryPerformanceCounter( (LARGE_INTEGER*) &qPerfomance );

	char WhistleupName[MAX_PATH];
	char WFChangedName[MAX_PATH];

	wsprintfA( WhistleupName, "IPU%08Xd%08X%08X", qPerfomance.HighPart, qPerfomance.LowPart, GetCurrentThreadId() );
	wsprintfA( WFChangedName, "IPC%08Xd%08X%08X", qPerfomance.HighPart, qPerfomance.LowPart, GetCurrentThreadId() );
	
	CLIENT_INFO ClientInfo;

	hDevice = OLD_ClientRegisterImp( pAppReg, phWhistleup, phWFChanged, WhistleupName, WFChangedName, &ClientInfo );
	pAppReg->m_AppID = ClientInfo.m_AppID;

	return hDevice;
}

void
OLD_ClientUnregister (
	HANDLE hDevice
	)
{
	HDSTATE Activity;

	Activity.Activity = _AS_Unload;
	Activity.AppID = InstallerProtection;

	DWORD dwRet;

	DeviceIoControl (
		hDevice,
		IOCTLHOOK_Activity,
		&Activity,
		sizeof(Activity),
		&Activity,
		sizeof(Activity),
		&dwRet,
		NULL
		);
}

BOOL
OLD_DrvRequest (
	HANDLE hDevice,
	PEXTERNAL_DRV_REQUEST pInRequest,
	PVOID pOutRequest,
	ULONG* OutRequestSize
	)
{
	if ((pInRequest == NULL) || (OutRequestSize == NULL))
		return FALSE;

	ULONG Outtmp = 0;
	if (!pOutRequest)
	{
		if ((FLTTYPE_SYSTEM == pInRequest->m_DrvID) && 
			(
			(_AVPG_IOCTL_GET_PREFETCHINFO == pInRequest->m_IOCTL)
			|| (_AVPG_IOCTL_GET_SECTIONLIST == pInRequest->m_IOCTL)
			|| (_AVPG_IOCTL_GET_DRVLIST == pInRequest->m_IOCTL)
			))
		{
			if (!OutRequestSize)
			{
				SetLastError(ERROR_INVALID_PARAMETER);
				return FALSE;
			}
			
			pOutRequest = &Outtmp;
			*OutRequestSize = sizeof(Outtmp);
		}
	}

	if (DeviceIoControl(hDevice, IOCTLHOOK_External_Drv_Request, pInRequest, sizeof(EXTERNAL_DRV_REQUEST) + pInRequest->m_BufferSize, 
		pOutRequest, *OutRequestSize, OutRequestSize, NULL))
	{
		if (pOutRequest == &Outtmp)
		{
			*OutRequestSize = Outtmp;
			return FALSE;
		}

		return TRUE;
	}

	if (pOutRequest == &Outtmp)
		*OutRequestSize = Outtmp;
	
	return FALSE;
}


// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "StartUpEnum2". Static(shared) property table variables
const tCODEPAGE PlugInCP = cCP_UNICODE; // must be READ_ONLY at runtime
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR StartUpEnum2::ObjectInit()
{
	OSVERSIONINFOA	osvi;
	tERROR error;
	PR_TRACE_FUNC_FRAME( "StartUpEnum2::ObjectInit method" );

	char tbuffer [9];
	PR_TRACE((this, prtNOTIFY, "startupenum2\tstart time, %s", _strtime(tbuffer)));

	hSubCureLib=NULL;
//	pfSpecialRepairMain=NULL;

//INT3;
	InitializeCriticalSection(&m_sCleanupCriticalSection);
	InitializeCriticalSection(&m_sCommonCriticalSection);

	memset(&osvi, 0, sizeof(OSVERSIONINFOA));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
	if(!GetVersionExA(&osvi)) return errBAD_VERSION;
	if (osvi.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS)
		g_bIsWin9x=1; // Windows 9x-like
	else
		g_bIsWin9x=0;												// Windows NT-like

	if (!InitRegAPI(&m_hRegLib))
		return errMODULE_CANNOT_BE_INITIALIZED;
	if (!InitFileAPI(&m_hFileLib))
		return errMODULE_CANNOT_BE_INITIALIZED;
	InitSecurityAPI(&m_hSecurityLib);

	APP_REGISTRATION AppReg;

	m_hDevice = OLD_ClientRegister( &AppReg, &m_hWhistleup, &m_hWFChanged );

	if (!m_hDevice)
	{
		PR_TRACE((this, prtERROR, "startupenum2\tNo handle driver!"));
	}
	else
	{
		PR_TRACE((this, prtNOTIFY, "startupenum2\thandle driver %x", m_hDevice));
		m_AppID = AppReg.m_AppID;
	}

//INT3;
	m_hDecodeIO=NULL;
	m_hBaseIni=NULL;

	cStrObj strStartupIni("%Bases%\\startup.ini");
//INT3;
	if( errOK_DECIDED == sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString (strStartupIni), 0, 0) )
		error = strStartupIni.copy(this, pgOBJECT_FULL_NAME);

	error = g_root->RegisterCustomPropId(&m_propId_PROGRESS, npPROP_PROGRESS_COUNT_PROCENT, pTYPE_DWORD);
	if (PR_FAIL(error))
	{
		PR_TRACE((this, prtERROR, "startupenum2\tCannot register property, %terr", error));
		return error;
	}
	

	// System/System32
	if (PR_SUCC(error))
	{
		tDWORD len;
		if (!m_sWindowsDirectory.reserve_len(MAX_PATH))
			return ERROR_NOT_ENOUGH_MEMORY;
		len = pfGetWindowsDirectory(m_sWindowsDirectory,MAX_PATH);
		if (len > MAX_PATH-1)
		{
			if (!m_sWindowsDirectory.reserve_len(len))
				return ERROR_NOT_ENOUGH_MEMORY;
			len = pfGetWindowsDirectory(m_sWindowsDirectory,MAX_PATH);
		}
		_wcslwr((wchar_t*)m_sWindowsDirectory);
		
		if (!m_sSystemDrive.reserve_len(4))
			return ERROR_NOT_ENOUGH_MEMORY;
		wcsncpy(m_sSystemDrive, m_sWindowsDirectory, 3);
		m_sSystemDrive[3] = 0;

		if (!g_bIsWin9x)
		{
			m_sSystem32Directory = L"\\\\?\\";
			m_sSystem32Directory.append(m_sWindowsDirectory);
			m_sWindowsDirectory = m_sSystem32Directory;
		}
		else
		{
			m_sSystem32Directory = m_sWindowsDirectory;
		}
		m_sSystem32Directory.append_path(L"system32");
		m_sSystem32DriversDirectory = m_sSystem32Directory;
		m_sSystem32DriversDirectory.append_path(L"drivers");
		m_sSystemDirectory = m_sWindowsDirectory;
		m_sSystemDirectory.append_path(L"system");
	}
	wchar_t* sPath = _wgetenv(L"PATH");
	if (sPath)
		m_nPathEnvLen = wcslen(sPath);
	_Hash_Init_Library(pfMemAlloc,pfMemFree);
	m_pHashKnownFiles=_Hash_InitNew(*this,8);

	LoadProfiles();

	m_bMainAdvancedDisinfection = cFALSE;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR StartUpEnum2::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "StartUpEnum2::ObjectInitDone method" );

	return error;
}
// AVP Prague stamp end



tERROR StartUpEnum2::FreeHostsLinks()
{
	tDWORD i;
	for (i=0;i<m_HostsStrings.nCount;i++)
	{
		if (m_HostsStrings.ppLinks[i])
			heapFree(m_HostsStrings.ppLinks[i]);
	}
	heapFree(m_HostsStrings.ppLinks);
	m_HostsStrings.ppLinks = NULL;
	m_HostsStrings.nAllocated = 0;
	m_HostsStrings.nCount = 0;
	return errOK;
}

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR StartUpEnum2::ObjectPreClose()
{
	tDWORD i=0;
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "StartUpEnum2::ObjectPreClose method" );

	// Place your code here

//INT3;
	PR_TRACE((this,prtNOTIFY,"startupenum2\t<ObjectPreClose> object close procedure started."));
	UnLoadProfiles();
	FreeHostsLinks();
	_Hash_Done(*this,m_pHashKnownFiles);
	
	if (INVALID_HANDLE_VALUE == m_hDevice)
	{
		OLD_ClientUnregister( m_hDevice );
		CloseHandle( m_hWhistleup );
		CloseHandle( m_hWFChanged );
		CloseHandle( m_hDevice );
		m_hDevice = INVALID_HANDLE_VALUE;
	}

	if (m_hShellLib)
	{
		pfCoUninitialize();
		FreeLibrary(m_hShellLib);
		m_hShellLib=NULL;
	}
	if (m_hMsiLib)
		FreeLibrary(m_hMsiLib);
	m_hMsiLib=NULL;
	if (m_hSecurityLib)
		FreeLibrary(m_hSecurityLib);
	m_hSecurityLib=NULL;
	if (m_hFileLib)
		FreeLibrary(m_hFileLib);
	m_hFileLib=NULL;
	if (m_hRegLib)
		FreeLibrary(m_hRegLib);
	m_hRegLib=NULL;

	char tbuffer [9];
	PR_TRACE((this, prtNOTIFY, "startupenum2\tstop time, %s", _strtime(tbuffer)));

	DeleteCriticalSection(&m_sCommonCriticalSection);
	DeleteCriticalSection(&m_sCleanupCriticalSection);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, IOPropGet )
// Interface "StartUpEnum2". Method "Get" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
tERROR StartUpEnum2::IOPropGet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* multyproperty method IOPropGet" );

	switch ( prop ) {

		case pgOBJECT_NAME:
		case pgOBJECT_PATH:
		case pgOBJECT_FULL_NAME:
			if (m_hDecodeIO) error = CALL_SYS_PropertyGetStr(m_hDecodeIO,out_size, prop, buffer, size, cCP_UNICODE);
			else error = errOBJECT_NOT_INITIALIZED;
			break;

		case plADVANCED_DISINFECTION:
			if (size>=4)
			{
				memcpy(buffer,&m_bMainAdvancedDisinfection,4);
				*out_size=4;
			}
			break;

		default:
			error = errPARAMETER_INVALID;
			break;
	}

	PR_TRACE_A2( this, "Leave *GET* multyproperty method IOPropGet, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, IOPropSet )
// Interface "StartUpEnum2". Method "Set" for property(s):
//  -- OBJECT_NAME
//  -- OBJECT_PATH
//  -- OBJECT_FULL_NAME
tERROR StartUpEnum2::IOPropSet( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* multyproperty method IOPropSet" );

	switch ( prop ) {

		case pgOBJECT_NAME:
		case pgOBJECT_PATH:
		case pgOBJECT_FULL_NAME:
			if (PR_FAIL(error=CALL_SYS_ObjHeapRealloc(this,(tPTR*)&m_sOBJECT_FULL_NAME,(tPTR)m_sOBJECT_FULL_NAME,size+1))) return error;
			memcpy(m_sOBJECT_FULL_NAME,buffer,size);
			m_sizeOFN=size;
			break;

		case plADVANCED_DISINFECTION:
			memcpy(&m_bMainAdvancedDisinfection,buffer,4);
			break;

		default:
			error = errPARAMETER_INVALID;
			break;
	}

	PR_TRACE_A2( this, "Leave *SET* multyproperty method IOPropSet, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, StartEnum )
// Parameters are:
//! tERROR StartUpEnum2::StartEnum( hOBJECT p_pRetObject )
tERROR StartUpEnum2::StartEnum( hOBJECT p_pRetObject, tBOOL p_bAdvancedDisinfection )
{
	tERROR error;
	PR_TRACE_FUNC_FRAME( "StartUpEnum2::StartEnum method" );

	
	EnumContext	pContextScan(this, p_pRetObject);
	pContextScan.m_bAdvancedDisinfection = !!p_bAdvancedDisinfection;
	error=pContextScan.Scan(NULL);

	PR_TRACE((this,prtALWAYS_REPORTED_MSG,"startupenum2\tAfter Scan, Input:%d,Dup:%d  Parse:%d,Dup:%d,Miss:%d Output:%d,Dup:%d",g_TotalInput, g_DupInput, g_TotalParse, g_DupParse, g_MissPars, g_TotalOutput, g_DupOutput));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, CleanUp )
// Parameters are:
//! tERROR StartUpEnum2::CleanUp( hSTRING p_DelFName, hOBJECT p_pRetObject )
tERROR StartUpEnum2::CleanUp( hSTRING p_DelFName, hOBJECT p_pRetObject, tSTRING p_DetectedVirusName )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "StartUpEnum2::CleanUp method" );

	PR_TRACE((this,prtNOTIFY,"startupenum2\t<CleanUp> started"));

	cAutoNativeCS _lock(&m_sCleanupCriticalSection);

//INT3;	
	if (p_DetectedVirusName)
	{
		if (PR_FAIL(error=LoadSubCureLibrary()))
			PR_TRACE((this, prtERROR, "startupenum2\t<Init> Cannot load KLSubCure.dll, %terr", error));
	}
	else
		hSubCureLib=NULL;
	
	EnumContext	pContextScan(this, p_pRetObject);
	pContextScan.m_bAdvancedDisinfection = !!m_bMainAdvancedDisinfection;
	if (PR_FAIL(error = pContextScan.SetInfectedObjectName(p_DelFName)))
	{
		if (hSubCureLib)
			error=pfSpecialRepair(p_DelFName,p_pRetObject,p_DetectedVirusName);
		DeInitLibrary(/*hSubCureFreeBuf,*/hSubCureLib/*,pfSpecialRepairMain*/);
		return error;
	}

	error = pContextScan.Scan(p_DelFName);

//INT3;
	if (hSubCureLib)
		error=pfSpecialRepair(p_DelFName,p_pRetObject,p_DetectedVirusName);
	DeInitLibrary(/*hSubCureFreeBuf,*/hSubCureLib/*,pfSpecialRepairMain*/);

	PR_TRACE((this,prtNOTIFY,"startupenum2\t<CleanUp> delete LNK and PIF links to infected files"));
	if (pContextScan.pFileToDel)
	{
		tERROR	fRetS;
		DWORD i;
		DWORD Count=pContextScan.pFileToDel->Count;
		tDWORD dwSize=sizeof(ACTION_MESSAGE_DATA);
		for (i=0;i<Count;i++)
		{
			PR_TRACE((this,prtNOTIFY,"startupenum2\t<CleanUp> delete LNK and PIF links to infected files <%S>",(wchar_t*)pContextScan.pFileToDel->cProfileName[i]));
			pContextScan.m_SendData.m_sFilePath=(wchar_t*)pContextScan.pFileToDel->cProfileName[i];
			pContextScan.m_SendData.m_ObjType=OBJECT_LINK_FILE_TYPE;
			pContextScan.m_SendData.m_sRoot=(wchar_t*)pContextScan.pFileToDel->cProfileName[i];
			pContextScan.m_SendData.m_sSection=NULL;
			pContextScan.m_SendData.m_sValue=NULL;
			pContextScan.m_SendData.m_dAction=FileToDel;
			fRetS=pContextScan.m_pSendToObj->sysSendMsg(pmc_STARTUPENUM2,mc_FILE_FOUNDED,NULL,&pContextScan.m_SendData,&dwSize);
			if ((fRetS==ERROR_CANCELLED)||(fRetS==errOPERATION_CANCELED))
			{
				error=fRetS;
				break;
			}
		}
	}

	PR_TRACE((this,prtALWAYS_REPORTED_MSG,"startupenum2\tAfter CleanUp, Input:%d,Dup:%d  Parse:%d,Dup:%d,Miss:%d,Dup:%d Output:%d,Dup:%d",g_TotalInput, g_DupInput, g_TotalParse, g_DupParse, g_MissPars, g_DupMiss, g_TotalOutput, g_DupOutput));
	PR_TRACE((this,prtNOTIFY,"startupenum2\t<CleanUp> finished"));
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "StartUpEnum2". Register function
tERROR StartUpEnum2::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(StartUpEnum2_PropTable)
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_NAME, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_PATH, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_FULL_NAME, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
	mSHARED_PROPERTY_PTR( pgPLUGIN_CODEPAGE, PlugInCP, sizeof(PlugInCP) )
	mpLOCAL_PROPERTY_FN( plADVANCED_DISINFECTION, FN_CUST(IOPropGet), FN_CUST(IOPropSet) )
mpPROPERTY_TABLE_END(StartUpEnum2_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(StartUpEnum2)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
mINTERNAL_TABLE_END(StartUpEnum2)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(StartUpEnum2)
	mEXTERNAL_METHOD(StartUpEnum2, StartEnum)
	mEXTERNAL_METHOD(StartUpEnum2, CleanUp)
mEXTERNAL_TABLE_END(StartUpEnum2)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "StartUpEnum2::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_STARTUPENUM2,                       // interface identifier
		PID_STARTUPENUM2,                       // plugin identifier
		0x00000000,                             // subtype identifier
		StartUpEnum2_VERSION,                   // interface version
		VID_ALEX,                               // interface developer
		&i_StartUpEnum2_vtbl,                   // internal(kernel called) function table
		(sizeof(i_StartUpEnum2_vtbl)/sizeof(tPTR)),// internal function table size
		&e_StartUpEnum2_vtbl,                   // external function table
		(sizeof(e_StartUpEnum2_vtbl)/sizeof(tPTR)),// external function table size
		StartUpEnum2_PropTable,                 // property table
		mPROPERTY_TABLE_SIZE(StartUpEnum2_PropTable),// property table size
		sizeof(StartUpEnum2)-sizeof(cObjImpl),  // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"StartUpEnum2(IID_STARTUPENUM2) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call StartUpEnum2_Register( hROOT root ) { return StartUpEnum2::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgINTERFACE_VERSION
// You have to implement propetry: pgINTERFACE_COMMENT
// AVP Prague stamp end




void* pfMemAlloc(hOBJECT hObj,void** ppMem,unsigned int Size)
{
	CALL_SYS_ObjHeapRealloc(hObj,ppMem,*ppMem,Size);
	return *ppMem;
}

void pfMemFree(hOBJECT hObj,void* Obj)
{
	if (!Obj) return;
	CALL_SYS_ObjHeapFree(hObj,Obj);
	return;
}

tERROR StartUpEnum2::ReInitObjects()
{
	tERROR error;
	cAutoNativeCS _lock(&m_sCommonCriticalSection);
	LONG nScanActiv = InterlockedCompareExchange(&m_nScanActiv, 0, 0);
	PR_TRACE((this, prtIMPORTANT, "startupenum2\tReInitObjects nScanActiv=%d, m_hDecodeIO=%x, m_hBaseIni=%x", nScanActiv, m_hDecodeIO, m_hBaseIni));
	if (nScanActiv > 1 && m_hBaseIni)
	{
		PR_TRACE((this, prtIMPORTANT, "startupenum2\tReInitObjects skip re-init", nScanActiv));
		return error=errOK;
	}
	if (m_hDecodeIO) m_hDecodeIO->sysCloseObject();
	m_hDecodeIO=NULL;
	if (m_hBaseIni) m_hBaseIni->sysCloseObject();
	m_hBaseIni=NULL;
	error = sysCreateObject((hOBJECT*)&m_hDecodeIO, IID_DECODEIO, PID_STARTUPENUM2);
	if (PR_FAIL(error))
	{
		m_hDecodeIO=NULL;
		PR_TRACE((this, prtERROR, "startupenum2\tCannot create Decode IO object, %terr", error));
		return error;
	}
	error = m_hDecodeIO->propSetStr(NULL,pgOBJECT_FULL_NAME,m_sOBJECT_FULL_NAME,m_sizeOFN,cCP_UNICODE);
	if (PR_FAIL(error))
	{
		m_hDecodeIO->sysCloseObject();
		m_hDecodeIO=NULL;
		PR_TRACE((this, prtERROR, "startupenum2\tCannot set property for Decode IO object, %terr", error));
		return error;
	}
	error = m_hDecodeIO->sysCreateObjectDone();
	if (PR_FAIL(error))
	{
		m_hDecodeIO->sysCloseObject();
		m_hDecodeIO=NULL;
		PR_TRACE((this, prtERROR, "startupenum2\tCannot done Decode IO object, %terr", error));
		return error;
	}
	error = sysCreateObject((hOBJECT*)&m_hBaseIni, IID_INIFILE, PID_INIFILE);
	if (PR_FAIL(error))
	{
		m_hBaseIni=NULL;
		PR_TRACE((this, prtERROR, "startupenum2\tCannot create Base INI object, %terr", error));
		return error;
	}
	error = m_hBaseIni->set_pgm_hIO((cObject*)m_hDecodeIO);
	if (PR_FAIL(error))
	{
		m_hBaseIni->sysCloseObject();
		m_hBaseIni=NULL;
		PR_TRACE((this, prtERROR, "startupenum2\tCannot set property for Base INI object, %terr", error));
		return error;
	}
	error = m_hBaseIni->sysCreateObjectDone();
	if (PR_FAIL(error))
	{
		m_hBaseIni->sysCloseObject();
		m_hBaseIni=NULL;
		PR_TRACE((this, prtERROR, "startupenum2\tCannot done Base INI object, %terr", error));
		return error;
	}

	// reload hosts
	{
		tDWORD	dSectionIndex=0;
		FreeHostsLinks();
		error=m_hBaseIni->FindSection("HOSTS_ANALYSE_TYPE",&dSectionIndex);
		if (PR_SUCC(error))
		{
			tDWORD			i=0;
			tCHAR*			sValName;
			tCHAR*			sData;
			cPrStrA         sDataCopy;
			while (PR_SUCC(m_hBaseIni->EnumValues(dSectionIndex,i,&sValName,&sData)))
			{
				const tCHAR*	pParam;
				sDataCopy = sData;
				if (ParseIniString(sDataCopy,&pParam,1))
				{
					if (m_HostsStrings.nAllocated <= m_HostsStrings.nCount)
					{
						m_HostsStrings.nAllocated = m_HostsStrings.nCount + 10;
						if (PR_FAIL(error = CALL_SYS_ObjHeapRealloc(this,(tPTR*)&m_HostsStrings.ppLinks,(tPTR)m_HostsStrings.ppLinks,sizeof(tCHAR*)*(m_HostsStrings.nAllocated))))
						{
							m_HostsStrings.nCount = 0;
							m_HostsStrings.nAllocated = 0;
							m_HostsStrings.ppLinks = 0;
							return error;
						}
					}
					if (PR_FAIL(error = CALL_SYS_ObjHeapAlloc(this,(tPTR*)&m_HostsStrings.ppLinks[m_HostsStrings.nCount],strlen(pParam)+1)))
						return error;
					strcpy(m_HostsStrings.ppLinks[m_HostsStrings.nCount],pParam);
					m_HostsStrings.nCount++;
				}
				i++;
			}
			if (error==errEND_OF_THE_LIST) 
				error=errOK;
		}
	}
	return error;
}

void StartUpEnum2::LoadProfiles()
{
	PTOKEN_PRIVILEGES	pOldToken=NULL;
	cPrStrW sProfileFolder;
	cPrStrW sProfileName;
	HKEY	hKey=NULL;
	DWORD	d,i=0,j=0,retCodeEnum,nEnumErrors=0;
	wchar_t RegNameNT[]=L"software\\microsoft\\windows nt\\currentversion\\profilelist";
	wchar_t	RegName9x[]=L"software\\microsoft\\windows\\currentversion\\profilelist";
	wchar_t FileNameNT[]=L"ntuser.dat";
	wchar_t FileName9x[]=L"user.dat";
	wchar_t ProfileImageValue[]=L"ProfileImagePath";

	PR_TRACE((this,prtNOTIFY,"startupenum2\t<LoadProfiles> Enum and load user profiles"));
//INT3;
	if (Privilege(TRUE,&pOldToken))
	{
		EnumContext	pContextResolveFileName(this, *this);
		cRegEnumCtx* pRegEnumCtx=pContextResolveFileName.m_cRegEnumCtx;
		if (pRegEnumCtx->pfRegOpenKey(HKEY_LOCAL_MACHINE,(g_bIsWin9x==0)?RegNameNT:RegName9x,&hKey)==ERROR_SUCCESS)
		{
			m_bRegUsersLoaded=cTRUE;
			DWORD dProfilesNumber;
			if (pRegEnumCtx->pfRegQueryInfoKey(hKey,&dProfilesNumber,NULL,NULL,NULL,NULL)==ERROR_SUCCESS)
			{
				if (dProfilesNumber)
					heapAlloc((tPTR*)&pUnloadProfilesList,sizeof(DWORD)+dProfilesNumber*sizeof(DWORD));
			}
			if (!pUnloadProfilesList)
			{
				Privilege(FALSE,&pOldToken);
				if (pOldToken)
					free(pOldToken);
//				if (Privilege(FALSE,&pOldToken))
				return;
			}
			for (d=0;;d++)
			{
				sProfileName="";
				retCodeEnum=pRegEnumCtx->pfRegEnumKey(hKey,d,sProfileName);
				if (retCodeEnum==ERROR_NO_MORE_ITEMS) 
					break;
				if (retCodeEnum!=ERROR_SUCCESS)
				{
					if (nEnumErrors>5) 
						break;
					nEnumErrors++;
					continue;
				}
				if (pRegEnumCtx->ReadKeyValueStr(hKey,sProfileName,ProfileImageValue,sProfileFolder)) 
				{
					LONG error = ERROR_SUCCESS;
					HKEY hSubKey=NULL;
					bool bExist=false;
					int wchProfileFolderSize=sProfileFolder.getlen();
					sProfileFolder.append_path((g_bIsWin9x==0)?FileNameNT:FileName9x);
					PR_TRACE((this,prtNOTIFY,"startupenum2\t<LoadProfiles> Check profile for <%S> user from <%S> file.",(wchar_t*)sProfileName,(wchar_t*)sProfileFolder));
					if (pRegEnumCtx->pfRegOpenKey(HKEY_USERS,sProfileName,&hSubKey)==ERROR_SUCCESS)
					{
						DWORD dSubKeys;
						if (pRegEnumCtx->pfRegQueryInfoKey(hSubKey,&dSubKeys,NULL,NULL,NULL,NULL)==ERROR_SUCCESS)
						{
							if (dSubKeys)
							{
								PR_TRACE((this,prtNOTIFY,"startupenum2\t<LoadProfiles> Profile for <%S> user exist.",(wchar_t*)sProfileName));
								bExist=true;
							}
						}
						pRegEnumCtx->pfRegCloseKey(hSubKey);
					}
					if (!bExist)
					{
						cPrStrW     sFullFileName;
						if (PR_FAIL(pContextResolveFileName.ParsEnviroment((wchar_t*)sProfileFolder,sFullFileName)))
							sFullFileName=sProfileFolder;
						LONG error=pRegEnumCtx->pfRegLoadKey(sProfileName,sFullFileName);
						if (error!=ERROR_SUCCESS)
						{
							PR_TRACE((this,prtNOTIFY,"startupenum2\t<LoadProfiles> ERROR loading profile for <%S> user from <%S> file - %d.",(wchar_t*)sProfileName,(wchar_t*)sFullFileName,error));
							if (!pNotLoadedProfilesList)
								heapAlloc((tPTR*)&pNotLoadedProfilesList,sizeof(DWORD)+dProfilesNumber*sizeof(DWORD));
							if (pNotLoadedProfilesList)
							{
								sProfileFolder[wchProfileFolderSize]=0;
								heapAlloc((tPTR*)&(pNotLoadedProfilesList->cProfileName[j]),(wchProfileFolderSize+1)*sizeof(wchar_t));
								if (pNotLoadedProfilesList->cProfileName[j])
								{
									wcscpy((wchar_t*)pNotLoadedProfilesList->cProfileName[j],(wchar_t*)sProfileFolder);
									pNotLoadedProfilesList->Count++;
									j++;
								}
							}
						}
						else
						{
							PR_TRACE((this,prtNOTIFY,"startupenum2\t<LoadProfiles> <%S> profile from <%S> file is loaded.",(wchar_t*)sProfileName,(wchar_t*)sFullFileName));
							heapAlloc((tPTR*)&(pUnloadProfilesList->cProfileName[i]),(sProfileName.getlen()+1)*sizeof(wchar_t));
							if (pUnloadProfilesList->cProfileName[i])
							{
								wcscpy((wchar_t*)pUnloadProfilesList->cProfileName[i],(wchar_t*)sProfileName);
								pUnloadProfilesList->Count++;
								i++;
							}
						}
					}
					if (error != ERROR_SUCCESS)
						m_bRegUsersLoaded = cFALSE;
				}
				else
					m_bRegUsersLoaded = cFALSE;
			}
			pRegEnumCtx->pfRegCloseKey(hKey);
		}
	}
	Privilege(FALSE,&pOldToken);
	if (pOldToken)
		free(pOldToken);
	return;
}

void StartUpEnum2::UnLoadProfiles()
{
	PTOKEN_PRIVILEGES	pOldToken=NULL;
	DWORD i;
	DWORD Count;

//INT3;
	if (pUnloadProfilesList)
	{
		cRegEnumCtx pRegEnumCtx(this,this->m_AppID);
		Count=pUnloadProfilesList->Count;
		Privilege(TRUE,&pOldToken);
		for (i=0;i<Count;i++)
		{
			pRegEnumCtx.pfRegUnLoadKey((wchar_t*)pUnloadProfilesList->cProfileName[i]);
			heapFree((void*)pUnloadProfilesList->cProfileName[i]);
			pUnloadProfilesList->cProfileName[i]=NULL;
		}
		Privilege(FALSE,&pOldToken);
		if (pOldToken)
			free(pOldToken);
		heapFree(pUnloadProfilesList);
		pUnloadProfilesList=NULL;
	}
	if (pNotLoadedProfilesList)
	{
		Count=pNotLoadedProfilesList->Count;
		for (i=0;i<Count;i++)
		{
			heapFree((void*)pNotLoadedProfilesList->cProfileName[i]);
			pNotLoadedProfilesList->cProfileName[i]=NULL;
		}
		heapFree(pNotLoadedProfilesList);
		pNotLoadedProfilesList=NULL;
	}
	return;
}

BOOL StartUpEnum2::Privilege(BOOL bEnable,PTOKEN_PRIVILEGES* pOldToken)
{
	HANDLE				hToken;
	TOKEN_PRIVILEGES	tp;
	PTOKEN_PRIVILEGES	pOldTp=NULL;
	DWORD				dOldTpSize=0;
	BOOL				Ret=FALSE;

	if (g_bIsWin9x)
		return TRUE;
	if (!pOldToken)
		return Ret;
	if (!pfOpenProcessToken(TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken))
		return Ret;
	cAutoCloseHandle _close_handle(&hToken);
	{
		cAutoToken _restore_impersonation(hToken);
		// get the luid
		if (bEnable)
		{
			if (!pfLookupPrivilegeValue(L"SeRestorePrivilege",&tp.Privileges[0].Luid))
				return Ret;
			tp.PrivilegeCount = 1;
			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			// enable or disable the privilege
			pfAdjustTokenPrivileges(hToken,&tp,0,pOldTp,&dOldTpSize);
			dOldTpSize+=20;
			pOldTp=(PTOKEN_PRIVILEGES)malloc(dOldTpSize);
			if (pOldTp)
			{
				if (pfAdjustTokenPrivileges(hToken,&tp,dOldTpSize,pOldTp,&dOldTpSize)==ERROR_SUCCESS)
				{
					*pOldToken=pOldTp;
					Ret=TRUE;
				}
			}
		}
		else
		{
			if (*pOldToken)
			{
				if (pfAdjustTokenPrivileges(hToken,*pOldToken,0,NULL,NULL)==ERROR_SUCCESS)
					Ret=TRUE;
			}
		}
	}
	return Ret;
}

//#include <klsys\filemapping\filemapping.h>
//#include "prformat.h"
//
//using KLSysNS::FileMapping;
//
//tERROR UnMapLibrary(hPLUGININSTANCE hInst)
//{
//	VirtualFree ( (tPTR)hInst, 0, MEM_RELEASE );
//	return errOK; 
//}
//

#define SET_TRACE_OBJECT				201

void _stdcall SubCureTrace(tag_TRACE_LEVEL dTraceLevel,char* chMask)
{
	//INT3;
	PR_TRACE((g_root,dTraceLevel,chMask));
	return;
}

tERROR StartUpEnum2::LoadSubCureLibrary() //KLSubCure.dll
{
	DWORD	dAttr = -1;
	tERROR	error=errOK;
	cStrObj strSubCureLibName("%Bases%\\KLSubCure.dll");
//INT3;
	if( errOK_DECIDED == sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString (strSubCureLibName), 0, 0) )
		dAttr=pfGetFileAttributes((tWCHAR*)strSubCureLibName.data());
	if ((dAttr==-1) || (dAttr&FILE_ATTRIBUTE_DIRECTORY))
	{
		strSubCureLibName="%ProductRoot%\\KLSubCure.dll";
		if( errOK_DECIDED == sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString (strSubCureLibName), 0, 0) )
			dAttr=pfGetFileAttributes((tWCHAR*)strSubCureLibName.data());
	}
	if ((dAttr==-1) || (dAttr&FILE_ATTRIBUTE_DIRECTORY))
	{
		tWCHAR wCurModulePath[MAX_PATH*2];
		pfGetModuleFileName(NULL,wCurModulePath,MAX_PATH*2);
		tWCHAR* ch=wcsrchr(wCurModulePath,'\\')+1;
		*ch=0;
		wcscat(wCurModulePath,L"KLSubCure.dll");
		strSubCureLibName=wCurModulePath;
		dAttr=pfGetFileAttributes((tWCHAR*)strSubCureLibName.data());
	}
	if ((dAttr==-1) || (dAttr&FILE_ATTRIBUTE_DIRECTORY))
		return errOBJECT_NOT_FOUND;

//#ifdef _DEBUG //LoadLibrary
//	char	LibName[MAX_PATH*2];
//	hSubCureFreeBuf=NULL;
//	FastUni2Ansi((tWCHAR*)strSubCureLibName.data(),(char*)LibName,-1);
//INT3;
	hSubCureLib=pfLoadLibrary((tWCHAR*)strSubCureLibName.data());
//	hSubCureLib=LoadLibrary(LibName);
	if (hSubCureLib)
	{
		pfSpecialRepair=(pfSpecialRepairPtr)GetProcAddress((HMODULE)hSubCureLib,"SpecialRepair");
		if (!pfSpecialRepair)
		{
			PR_TRACE((this,prtERROR,"startupenum2\t<LoadSubCureLibrary> could not get \"SpecialRepair\" function address, error %d .",GetLastError()));
			if (hSubCureLib) 
				FreeLibrary((HMODULE)hSubCureLib);
			hSubCureLib=NULL;
			return errOBJECT_NOT_FOUND;
		}
		PIMAGE_NT_HEADERS lpHeader=(PIMAGE_NT_HEADERS)((LONG)hSubCureLib+((IMAGE_DOS_HEADER*)hSubCureLib)->e_lfanew);
		tPluginInit pfSpecialRepairMain=(tPluginInit)((DWORD)hSubCureLib+lpHeader->OptionalHeader.AddressOfEntryPoint);
		pfSpecialRepairMain(hSubCureLib,SET_TRACE_OBJECT,(long*)SubCureTrace);
		error=errOK;
	}
	else
	{
		PR_TRACE((this,prtERROR,"startupenum2\t<LoadSubCureLibrary> <%S> library could not be loaded, error %d .",(tWCHAR*)strSubCureLibName.data(),GetLastError()));
		return errOBJECT_NOT_FOUND;
	}
//#else //LoadLibrary
//	error=MapLibrary((tWCHAR*)strSubCureLibName.data(), &hSubCureFreeBuf, (void**)&hSubCureLib, &pfSpecialRepairMain);
//#endif //LoadLibrary
//INT3;
//	if (PR_SUCC(error) && pfSpecialRepairMain)
//	{
//		GET_FUNCTIONS sFunction={"SpecialRepair",(void**)&pfSpecialRepair};
//		if (PR_FAIL(InitLibrary(hSubCureLib,pfSpecialRepairMain,&sFunction)))
//		{
//#ifdef _DEBUG //FreeLibrary
//			FreeLibrary((HMODULE)hSubCureLib);
//			hSubCureLib=NULL;
//#else //FreeLibrary
//			UnMapLibrary(hSubCureLib);
//			hSubCureLib=NULL;
//			pfSpecialRepairMain=NULL;
//#endif //FreeLibrary
//		}
//	}
	return error;
}

//tERROR LoadSections(tBYTE* aPlugin, tUINT aSize, tBYTE* lpBase)
//{
//	PIMAGE_NT_HEADERS lpHeader;
//	IMAGE_SECTION_HEADER* SecTable;
//	tDWORD i;
//
//	lpHeader = (PIMAGE_NT_HEADERS)((LONG)aPlugin+((IMAGE_DOS_HEADER*)aPlugin)->e_lfanew);
//	for ( i=0, SecTable=IMAGE_FIRST_SECTION(lpHeader); i < (tDWORD)lpHeader->FileHeader.NumberOfSections; i++, SecTable++ ) 
//	{
//		if ((SecTable->PointerToRawData!=0) && (SecTable->PointerToRawData<lpHeader->OptionalHeader.SizeOfImage))
//		{
//			if ( ( SecTable->VirtualAddress+SecTable->SizeOfRawData) < aSize ) 
//				memcpy(lpBase+SecTable->PointerToRawData,aPlugin+SecTable->VirtualAddress,SecTable->SizeOfRawData);
//			else
//				return errOBJECT_READ;
//		}
//	}
//	return errOK;
//}
//
//#define GET_PROG_ADDRESS				200
//#define SET_TRACE_OBJECT				201
//
//tERROR LoadImports(tBYTE* pModule, tBYTE* pH, tDWORD* hLibArr, tDWORD* LibCount)
//{
//	PIMAGE_NT_HEADERS lpHeader=(PIMAGE_NT_HEADERS)((LONG)pModule+((IMAGE_DOS_HEADER*)pModule)->e_lfanew);
//	IMAGE_IMPORT_DESCRIPTOR* pReloc;
//	tDWORD Count=0;
//	
//	pReloc = (IMAGE_IMPORT_DESCRIPTOR*)(pModule + lpHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
//	DWORD pEnd = (DWORD)pReloc + lpHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
//	
//	while ((DWORD)pReloc < pEnd)
//	{
//		if (!hLibArr)
//		{
//			if (LibCount) 
//				(*LibCount)++;
//			pReloc++;
//			continue;
//		}
//		if (*LibCount<=Count || !hLibArr)
//			return errUNEXPECTED;
//		char* test=(char*)pH+pReloc->Name;
//		if (!pReloc->Name)
//		{
//			pReloc++;
//			continue;
//		}
//		HMODULE hLib=LoadLibrary((char*)(pH+pReloc->Name));
//		if (hLib==NULL)
//			return errUNEXPECTED;
//		*(hLibArr+sizeof(tDWORD)*Count)=(tDWORD)hLib;
//		PIMAGE_THUNK_DATA thunk    = (PIMAGE_THUNK_DATA)pReloc->Characteristics;
//		PIMAGE_THUNK_DATA thunkIAT = (PIMAGE_THUNK_DATA)pReloc->FirstThunk;
//		
//		if ( thunk == 0 )   // No Characteristics field?
//		{
//			// Yes! Gotta have a non-zero FirstThunk field then.
//			thunk = thunkIAT;
//			if ( thunk == 0 )   // No FirstThunk field?  Ooops!!!
//				return errUNEXPECTED;
//		}
//		
//		thunk    = (PIMAGE_THUNK_DATA)( (PBYTE)thunk    + (DWORD)pH);
//		thunkIAT = (PIMAGE_THUNK_DATA)( (PBYTE)thunkIAT + (DWORD)pH);
//		while ( 1 ) // Loop forever (or until we break out)
//		{
//			char* szFuncName;
//			if ( thunk == NULL || thunk->u1.AddressOfData == 0 )
//				break;
//			
//			if ( thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG )
//				// Function has no name - only ordinal
//				szFuncName = (char*)(IMAGE_ORDINAL(thunk->u1.Ordinal));
//			else
//				// nik pOrdinalName =  thunk->u1.AddressOfData;
//				szFuncName=(char*)((PIMAGE_IMPORT_BY_NAME)((PBYTE)(thunk->u1.AddressOfData) + (DWORD)pModule))->Name;
//			if (szFuncName == NULL)
//				continue;
//			
//			thunkIAT->u1.Function=(DWORD)GetProcAddress(hLib,szFuncName);
//			
//			thunk++;            // Advance to next thunk
//			thunkIAT++;         // advance to next thunk
//		}
//		Count++;
//		pReloc++;
//	}
//	
//	return errOK;
//}
//
//tERROR FreeImports(tBYTE* pH)
//{
//	tDWORD i;
//	for(i=1;i>(tDWORD)*pH;i++)
//		FreeLibrary((HMODULE)*((tDWORD*)pH+i));
//	return errOK;
//}
//
//tERROR RelocateIt(tBYTE* pModule, tBYTE* pH)
//{
//	PIMAGE_NT_HEADERS lpHeader=(PIMAGE_NT_HEADERS)((LONG)pModule+((IMAGE_DOS_HEADER*)pModule)->e_lfanew);
//	IMAGE_BASE_RELOCATION* pReloc;
//	short *pEntry;
//	long lIndex, lMax, lDelta;
//	tBYTE *pPage;
//	
//	pReloc = (IMAGE_BASE_RELOCATION*)(pModule + lpHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
//	if ((tBYTE*)pReloc == pH)
//		return errOK;
//	
//	lDelta = (long)pH - lpHeader->OptionalHeader.ImageBase;
//	
//	while ((pReloc->VirtualAddress != 0) && (pReloc->SizeOfBlock != 0))
//	{
//		lMax = pReloc->SizeOfBlock;
//		lIndex = sizeof(IMAGE_BASE_RELOCATION);
//		pEntry = (short*)((long)pReloc + lIndex);
//		pPage = pH + pReloc->VirtualAddress;
//		
//		for (; lIndex < lMax; (pEntry ++), (lIndex += sizeof(short)))
//		{
//			switch(*pEntry >> 12)
//			{
//			case 0:
//				continue;
//			case 3:
//				*(long*)(pPage + (*pEntry & 0xFFF)) += lDelta;
//				break;
//			default:
//				return errUNEXPECTED;
//			}
//		}
//		pReloc = (IMAGE_BASE_RELOCATION*)((DWORD)pReloc + (DWORD)lMax);
//	}
//	
//	return errOK;
//}
//
//tERROR StartUpEnum2::MapLibrary( const tWCHAR* szPluginName, tBYTE** pImage, hPLUGININSTANCE* pInst, tPluginInit* pEntryPoint ) 
//{
//	DWORD  old_error_mode;
//	tERROR err;
//	tDWORD aSize = 0;
//#if defined (_DEBUG)
//	tDWORD nNameLen = 0; 
//	tDWORD nNameLenAligned = 0; 
//#endif
//	tBYTE* hInst = 0;
//
//	PR_TRACE(( this, prtSPAM, "startupenum2\t<MapLibrary> Loading cure library <%S>", szPluginName ));
//	
//	if ( !pInst || !pEntryPoint )
//		return errPARAMETER_INVALID;
//	
//	*pInst = 0;
//	*pEntryPoint = 0;
//	
//	old_error_mode = SetErrorMode( SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX );
//
//	try 
//	{
//		FileMapping aFileMapping;
//		PIMAGE_NT_HEADERS PrHeader = 0;
//		tBYTE* aPlugin = 0;
//		tDWORD ImportCount=0;
//		FileMapping::kl_size_t aPluginSize = 0;
//
//		int not_created;
//		not_created = aFileMapping.create( (tWCHAR*)szPluginName, KL_FILEMAP_READ );
//	
//		if ( not_created ) 
//		{
//			PR_TRACE( (this,prtERROR,"startupenum2\t<MapLibrary> Cannot create mapping of library <%S>", szPluginName) );
//			return errOBJECT_NOT_FOUND;
//		}
//
//		aPlugin = (tBYTE*)aFileMapping.map ( 0, 0, &aPluginSize );
//		if ( !aPlugin || ( aPlugin == (tPTR)-1 ) ) 
//		{
//			PR_TRACE( (this,prtERROR,"startupenum2\t<MapLibrary> Cannot map library <%S>", szPluginName) );
//			return errUNEXPECTED;
//		}
//
//		PrHeader = (PIMAGE_NT_HEADERS)((LONG)aPlugin+((IMAGE_DOS_HEADER*)aPlugin)->e_lfanew);
//		aSize = PrHeader->OptionalHeader.SizeOfImage;
//
//		LoadImports(aPlugin,hInst,NULL,&ImportCount);
//		aSize += sizeof(tDWORD)*(ImportCount+1);
//
//#if defined (_DEBUG)
//		nNameLen = sizeof(tWCHAR) * (1 + wcslen((tWCHAR*)szPluginName));
//		nNameLenAligned =  (nNameLen & 0x0F ? ( nNameLen + 0x10 ) & 0xFFFFFFF0 : nNameLen);
//		aSize += nNameLenAligned;
//#endif
//
//		*pImage = (tBYTE*)VirtualAlloc ( 0, aSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
//		**((tDWORD**)pImage) = ImportCount;
//		hInst = *pImage + sizeof(tDWORD)*ImportCount;
//#if defined (_DEBUG)
//#endif
//		if( *pImage == NULL ) 
//		{
//			aFileMapping.unmap (  aPlugin, aPluginSize );
//			return errNOT_ENOUGH_MEMORY;
//		}
//
//#if defined (_DEBUG)
//		memcpy( hInst, szPluginName, nNameLen );
//		hInst += nNameLenAligned;
//#endif
//
//		if ( aPluginSize < sizeof(PrFileHeader) ) 
//		{
//			aFileMapping.unmap (  aPlugin, aPluginSize );
//			VirtualFree ( (tPTR)hInst, 0, MEM_RELEASE );
//			return errOBJECT_READ;
//		}
//		memcpy( hInst, PrHeader, sizeof(PrFileHeader) );
//		err = LoadSections( aPlugin, aPluginSize, hInst );
//		if( PR_SUCC(err) )
//			err = RelocateIt( aPlugin, hInst );
//		if (PR_SUCC(err))
//			err = LoadImports(aPlugin,hInst,(tDWORD*)(*pImage+sizeof(tDWORD)),&ImportCount);
//		if( PR_SUCC(err) ) 
//		{
//			*pInst = (hPLUGININSTANCE)hInst;
//			*pEntryPoint = (tPluginInit)(hInst+PrHeader->OptionalHeader.AddressOfEntryPoint);
//		}
//		else
//			VirtualFree ( (tPTR)hInst, 0, MEM_RELEASE );
//		aFileMapping.unmap ( aPlugin, aPluginSize );
//	}
//	catch (...)
//	{
//		err = errUNEXPECTED;
//		PR_TRACE(( this, prtERROR, "startupenum2\t<MapLibrary> exception caught on <%S>", szPluginName));
//	}
//
//	SetErrorMode( old_error_mode );
//	return err;
//}

//tERROR StartUpEnum2::InitLibrary(hPLUGININSTANCE hInst,tPluginInit pSpecialRepairMain,pGET_FUNCTIONS  sFunction)
//{
//	tLONG reserved = 0;
//	tBOOL LibInited=cFALSE;
//#ifndef _DEBUG //InitLibrary
//	if (pSpecialRepairMain(hInst,DLL_PROCESS_ATTACH,&reserved))
//	{
//		if (pSpecialRepairMain(hInst,DLL_THREAD_ATTACH,&reserved)) 
//		{
//#endif //InitLibrary
//			if (pSpecialRepairMain(hInst,GET_PROG_ADDRESS,(long*)sFunction))
//			{
//				pSpecialRepairMain(hInst,SET_TRACE_OBJECT,(long*)SubCureTrace);
//				return errOK;
//			}
//			else
//				PR_TRACE(( this, prtERROR, "startupenum2\t<InitLibrary> Error getting functions of SubCure library"));
//#ifndef _DEBUG //InitLibrary
//			pSpecialRepairMain(hInst,DLL_THREAD_DETACH,&reserved);
//		}
//		else
//			PR_TRACE(( this, prtERROR, "startupenum2\t<InitLibrary> Error during SubCure library DLL_THREAD_ATTACH"));
//		pSpecialRepairMain(hInst,DLL_PROCESS_DETACH,&reserved);
//	}
//	else
//		PR_TRACE(( this, prtERROR, "startupenum2\t<InitLibrary> Error during SubCure library DLL_PROCESS_ATTACH"));
//#endif //InitLibrary
//	return errMODULE_CANNOT_BE_INITIALIZED;
//}

tERROR StartUpEnum2::DeInitLibrary(/*tBYTE* hBuf,*/hPLUGININSTANCE hInst/*,tPluginInit pSpecialRepairMain*/)
{
//#ifdef _DEBUG //FreeLibrary
	FreeLibrary((HMODULE)hInst);
	hSubCureLib=NULL;
//#else //FreeLibrary
//	if (hInst)
//	{
//		if (pSpecialRepairMain)
//		{
//			tLONG reserved = 0;
//			pSpecialRepairMain(hInst,DLL_THREAD_DETACH,&reserved);
//			pSpecialRepairMain(hInst,DLL_PROCESS_DETACH,&reserved);
//		}
//		FreeImports(hBuf);
//		UnMapLibrary(hBuf);
//	}
//#endif //FreeLibrary
	return errOK;
}

