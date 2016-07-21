#include "stdafx.h"
#include <CKAH/ckahum.h>
#include "CKAHMain.h"
#include "PluginUtils.h"
#include "PacketClient.h"
#include "ApplicationClient.h"
#include "../ckahrules/ckahrulesint.h"
#include "CKAHPacketRules.h"
#include "CKAHApplicationRules.h"
#include "md5hash.h"
#include "Utils.h"

#pragma comment( lib, "fssync.lib" )

CKAHUM::OpResult _SetWorkingMode (CKAHFW::WorkingMode mode,
								  bool bForceSet = false,
								  bool bPacketClientAdd = true, bool bPacketClientRemove = true,
								  bool bApplicationClientAdd = true, bool bApplicationClientRemove = true);

static CPacketClient g_PacketClient;
static CApplicationClient g_ApplicationClient;
static volatile CKAHFW::WorkingMode g_WorkingMode = (CKAHFW::WorkingMode)-1;

static bool StartPacketFilterReceiver (CKAHFW::PACKETRULENOTIFYCALLBACK fnPacketNotifyCallback, LPVOID lpCallbackParam)
{
	if (g_PacketClient.Enable (true))
	{
		g_PacketClient.SetPacketNotifyCallback (fnPacketNotifyCallback, lpCallbackParam);
		return true;
	}
	return false;
}

static bool StartApplicationFilterReceiver (CKAHFW::APPLICATIONRULENOTIFYCALLBACK fnApplicationNotifyCallback,
											LPVOID lpCallbackParam,
											CKAHFW::CHECKSUMNOTIFYCALLBACK ChecksumCallback,
											LPVOID lpChecksumCallbackContext)
{
	if (g_ApplicationClient.Enable (true))
	{
		g_ApplicationClient.SetApplicationNotifyCallback (fnApplicationNotifyCallback, lpCallbackParam);
		g_ApplicationClient.SetChecksumNotifyCallback (ChecksumCallback, lpChecksumCallbackContext);
		return true;
	}
	return false;
}

CKAHUM::OpResult CKAHFW::Start (PACKETRULENOTIFYCALLBACK PacketRuleNotifyCallback,
								LPVOID lpPacketCallbackContex,
								APPLICATIONRULENOTIFYCALLBACK ApplicationRuleNotifyCallback,
								LPVOID lpApplicationCallbackContext,
								CHECKSUMNOTIFYCALLBACK ChecksumCallback,
								LPVOID lpChecksumCallbackContext)
{
	log.WriteFormat (_T("[CKAHFW::Start] Starting firewall..."), PEL_INFO);

	log.WriteFormat (_T("[CKAHFW::Start] Starting packet receiver"), PEL_INFO);
	
	if (!StartPacketFilterReceiver (PacketRuleNotifyCallback, lpPacketCallbackContex))
	{
		g_PacketClient.Enable (false);
		log.WriteFormat (_T("[CKAHFW::Start] Failed to start packet receiver"), PEL_ERROR);
		return CKAHUM::srOpFailed;
	}

	log.WriteFormat (_T("[CKAHFW::Start] Packet receiver started, starting application receiver..."), PEL_INFO);
		
	if (!StartApplicationFilterReceiver (ApplicationRuleNotifyCallback, lpApplicationCallbackContext,
										ChecksumCallback, lpChecksumCallbackContext))
	{
		g_PacketClient.Enable (false);
		g_ApplicationClient.Enable (false);
		log.WriteFormat (_T("[CKAHFW::Start] Failed to start application receiver"), PEL_ERROR);
		return CKAHUM::srOpFailed;
	}

	log.WriteFormat (_T("[CKAHFW::Start] Application receiver started"), PEL_INFO);

	CKAHUM::OpResult result = CKAHUM_InternalStart (CCKAHPlugin::ptFirewall, g_lpStorage);

	if (result != CKAHUM::srOK && result != CKAHUM::srAlreadyStarted)
	{
		g_PacketClient.Enable (false);
		g_ApplicationClient.Enable (false);
		log.WriteFormat (_T("[CKAHFW::Start] Failed to start firewall"), PEL_ERROR);
		return result;
	}

	log.WriteFormat (_T("[CKAHFW::Start] Firewall successfully started"), PEL_INFO);
	
	FixPid();

	return result;	
}

CKAHUM::OpResult CKAHFW::FixPid()
{
	ULONG	retsize;
	DWORD	dwPid = ::GetCurrentProcessId();
	CKAHUM::OpResult res = SendPluginMessage(
								CCKAHPlugin::ptFirewall, 
								PLUG_IOCTL, 
								FIREWALL_PLUGIN_NAME, 
								MSG_FIX_PID, &dwPid, sizeof( DWORD ), NULL, 0, &retsize );
	return res;
								
}

CKAHUM::OpResult CKAHFW::Stop ()
{
	log.WriteFormat (_T("[CKAHFW::Stop] Stopping firewall..."), PEL_INFO);

	log.WriteFormat (_T("[CKAHFW::Stop] Pausing..."), PEL_INFO);

	Pause ();

	log.WriteFormat (_T("[CKAHFW::Stop] Stopping receivers..."), PEL_INFO);

	g_ApplicationClient.SignalStop ();
	g_PacketClient.SignalStop ();

	g_ApplicationClient.Enable (false);
	g_PacketClient.Enable (false);

	log.WriteFormat (_T("[CKAHFW::Stop] Stopping plugins..."), PEL_INFO);

	CKAHUM::OpResult result = CKAHUM_InternalStop (CCKAHPlugin::ptFirewall);

	log.WriteFormat (_T("[CKAHFW::Stop] Firewall stopped"), PEL_INFO);

	return result;
}

CKAHUM::OpResult CKAHFW::Pause ()
{
	log.WriteFormat (_T("[CKAHFW::Pause] Pausing firewall..."), PEL_INFO);
	CKAHUM::OpResult result = CKAHUM_InternalPause (CCKAHPlugin::ptFirewall);
	log.WriteFormat (_T("[CKAHFW::Pause] Pausing done"), PEL_INFO);
	return result;
}

CKAHUM::OpResult CKAHFW::Resume ()
{
	log.WriteFormat (_T("[CKAHFW::Resume] Resuming firewall..."), PEL_INFO);
	CKAHUM::OpResult result = CKAHUM_InternalResume (CCKAHPlugin::ptFirewall);
	log.WriteFormat (_T("[CKAHFW::Resume] Resuming firewall done"), PEL_INFO);
	return result;
}

CKAHUM::OpResult CKAHFW::GetPacketRules (OUT HPACKETRULES *phRules)
{
	log.WriteFormat (_T("[CKAHFW::GETPR] Getting packet rules..."), PEL_INFO);
	if (!phRules)
	{
		log.WriteFormat (_T("[CKAHFW::GETPR] Invalid argument"), PEL_ERROR);
		return CKAHUM::srInvalidArg;
	}
	
	if (!g_PacketClient.IsCreated ())
	{
		log.WriteFormat (_T("[CKAHFW::GETPR] No packet receiver - not started"), PEL_ERROR);
		return CKAHUM::srNotStarted;
	}

	log.WriteFormat (_T("[CKAHFW::GETPR] Getting filters for packet rules..."), PEL_INFO);

	FLTLIST flt_list, named_filters;

	if (!g_PacketClient.GetAllUserFilters (flt_list))
	{
		log.WriteFormat (_T("[CKAHFW::GETPR] Failed to get filters for packet rules"), PEL_ERROR);
		return CKAHUM::srOpFailed;
	}

	if ((*phRules = PacketRules_Create ()) == NULL)
	{
		log.WriteFormat (_T("[CKAHFW::GETPR] Failed to create packet rules object"), PEL_ERROR);
		return CKAHUM::srOpFailed;
	}

	CPacketRules &PacketRules = **(PacketRulesItem *)*phRules;

	PFILTER_PARAM pParam = NULL;
	
	FLTLIST::iterator i = flt_list.First ();

	while (i != flt_list.end ())
	{
		pParam = i->FindParam (PF_FILTER_USER_ID);
		
		if (pParam)
		{
			ReGetFilterListByID (*(UUID *)pParam->m_ParamValue, named_filters, flt_list);

			if (!named_filters.empty ())
			{
				PacketRuleItem item (new CPacketRule);
				
				if (FillPacketRuleFromFLTLIST (named_filters, *item))
					PacketRules.m_Rules.push_back (item);
			}
		}
		else
		{
			break;
		}
		
		i = flt_list.First ();
	}

	log.WriteFormat (_T("[CKAHFW::GETPR] Getting packet rules done"), PEL_INFO);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::SetPacketRules (IN HPACKETRULES hRules)
{
	log.WriteFormat (_T("[CKAHFW::SETPR] Setting packet rules..."), PEL_INFO);

	if (!hRules)
	{
		log.WriteFormat (_T("[CKAHFW::SETPR] Invalid argument"), PEL_ERROR);
		return CKAHUM::srInvalidArg;
	}

	if (!g_PacketClient.IsCreated ())
	{
		log.WriteFormat (_T("[CKAHFW::SETPR] No packet receiver - not started"), PEL_ERROR);
		return CKAHUM::srNotStarted;
	}

//	log.WriteFormat (_T("[CKAHFW::SETPR] ======================================="), PEL_INFO);
//	log.WriteFormat (_T("[CKAHFW::SETPR] Following packet rules passed:"), PEL_INFO);
//	PacketRules_Dump (hRules);
//	log.WriteFormat (_T("[CKAHFW::SETPR] ======================================="), PEL_INFO);

	log.WriteFormat (_T("[CKAHFW::SETPR] Creating filters for packet rules..."), PEL_INFO);

	CPacketRules &PacketRules = **(PacketRulesItem *)hRules;

	PacketRuleList::iterator i;

	FLTLIST flt_list;

	for (i = PacketRules.m_Rules.begin (); i != PacketRules.m_Rules.end (); ++i)
	{
		FLTLIST flt_rule_list;

		if (CreateFLTLISTFromPacketRule (**i, flt_rule_list))
			flt_list += flt_rule_list;
	}

	log.WriteFormat (_T("[CKAHFW::SETPR] Removing filters for packet rules..."), PEL_INFO);
	g_PacketClient.RemoveAllUserFilters (true);
	log.WriteFormat (_T("[CKAHFW::SETPR] Adding filters for packet rules..."), PEL_INFO);
	g_PacketClient.AddFilterList (flt_list);

	log.WriteFormat (_T("[CKAHFW::SETPR] Setting current working mode..."), PEL_INFO);
	CKAHFW::WorkingMode mode;
	if(GetWorkingMode(&mode) == CKAHUM::srOK)
		_SetWorkingMode(mode, true, true, false, false, false);

	log.WriteFormat (_T("[CKAHFW::SETPR] Setting packet rules done"), PEL_INFO);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::GetApplicationRules (OUT HAPPRULES *phRules)
{
	log.WriteFormat (_T("[CKAHFW::GETAR] Getting application rules..."), PEL_INFO);
	if (!phRules)
	{
		log.WriteFormat (_T("[CKAHFW::GETAR] Invalid argument"), PEL_ERROR);
		return CKAHUM::srInvalidArg;
	}

	if (!g_ApplicationClient.IsCreated ())
	{
		log.WriteFormat (_T("[CKAHFW::GETAR] No application receiver - not started"), PEL_ERROR);
		return CKAHUM::srNotStarted;
	}

	log.WriteFormat (_T("[CKAHFW::GETAR] Getting filters for application rules..."), PEL_INFO);

	FLTLIST flt_list, named_filters;

	if (!g_ApplicationClient.GetAllUserFilters (flt_list))
		return CKAHUM::srOpFailed;

	if ((*phRules = ApplicationRules_Create ()) == NULL)
		return CKAHUM::srOpFailed;

	CApplicationRules &ApplicationRules = **(ApplicationRulesItem *)*phRules;

	PFILTER_PARAM pParam = NULL;
	
	FLTLIST::iterator i = flt_list.First ();

	while (i != flt_list.end ())
	{
		pParam = i->FindParam (PF_FILTER_USER_ID);
		
		if (pParam)
		{
			ReGetFilterListByID (*(UUID *)pParam->m_ParamValue, named_filters, flt_list);

			if (!named_filters.empty ())
			{
				ApplicationRuleItem item (new CApplicationRule);
				
				if (FillApplicationRuleFromFLTLIST (named_filters, *item))
					ApplicationRules.m_Rules.push_back (item);
			}
		}
		else
		{
			break;
		}
		
		i = flt_list.First ();
	}

	log.WriteFormat (_T("[CKAHFW::GETAR] Getting application rules done"), PEL_INFO);

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::SetApplicationRules (IN HAPPRULES hRules)
{
	log.WriteFormat (_T("[CKAHFW::SETAR] Setting application rules..."), PEL_INFO);

	if (!hRules)
	{
		log.WriteFormat (_T("[CKAHFW::SETAR] Invalid argument"), PEL_ERROR);
		return CKAHUM::srInvalidArg;
	}

	if (!g_ApplicationClient.IsCreated ())
	{
		log.WriteFormat (_T("[CKAHFW::SETAR] No application receiver - not started"), PEL_ERROR);
		return CKAHUM::srNotStarted;
	}

//	log.WriteFormat (_T("[CKAHFW::SETAR] ======================================="), PEL_INFO);
//	log.WriteFormat (_T("[CKAHFW::SETAR] Following application rules passed:"), PEL_INFO);
//	ApplicationRules_Dump (hRules);
//	log.WriteFormat (_T("[CKAHFW::SETAR] ======================================="), PEL_INFO);

	log.WriteFormat (_T("[CKAHFW::SETAR] Creating filters for application rules..."), PEL_INFO);

	CApplicationRules &ApplicationRules = **(ApplicationRulesItem *)hRules;

	ApplicationRuleList::iterator i;

	FLTLIST flt_list;

    ULONG GroupID = 0;

	for (i = ApplicationRules.m_Rules.begin (); i != ApplicationRules.m_Rules.end (); ++i)
	{
		FLTLIST flt_rule_list;

		if (CreateFLTLISTFromApplicationRule (**i, ++GroupID, flt_rule_list))
			flt_list += flt_rule_list;
	}

	log.WriteFormat (_T("[CKAHFW::SETAR] Removing filters for application rules..."), PEL_INFO);
	g_ApplicationClient.RemoveAllUserFilters (true);
	log.WriteFormat (_T("[CKAHFW::SETAR] Adding filters for application rules..."), PEL_INFO);
	g_ApplicationClient.AddFilterList (flt_list);
	
	log.WriteFormat (_T("[CKAHFW::SETAR] Setting current working mode..."), PEL_INFO);
	CKAHFW::WorkingMode mode;
	if(GetWorkingMode(&mode) == CKAHUM::srOK)
		_SetWorkingMode(mode, true, false, false, true, false);

	log.WriteFormat (_T("[CKAHFW::SETAR] Setting application rules done"), PEL_INFO);

	return CKAHUM::srOK;
}

static LPCTSTR GetWorkingModeString (CKAHFW::WorkingMode mode)
{
	switch (mode)
	{
	case CKAHFW::wmBlockAll:
		return _T("wmBlockAll");
	case CKAHFW::wmRejectNotFiltered:
		return _T("wmRejectNotFiltered");
	case CKAHFW::wmAskUser:
		return _T("wmAskUser");
	case CKAHFW::wmAllowNotFiltered:
		return _T("wmAllowNotFiltered");
	case CKAHFW::wmAllowAll:
		return _T("wmAllowAll");
	}
	return _T("<unknown>");
}

CKAHUM::OpResult CKAHFW::SetWorkingMode (WorkingMode mode)
{
	return _SetWorkingMode (mode);
}

CKAHUM::OpResult _SetWorkingMode (CKAHFW::WorkingMode mode,
								  bool bForceSet,
								  bool bPacketClientAdd, bool bPacketClientRemove,
								  bool bApplicationClientAdd, bool bApplicationClientRemove)
{
	log.WriteFormat (_T("[CKAHFW::SETWM] Setting '%s' mode..."), PEL_INFO, GetWorkingModeString (mode));

	if (!g_ApplicationClient.IsCreated () || !g_PacketClient.IsCreated ())
	{
		log.WriteFormat (_T("[CKAHFW::SETWM] Firewall not started"), PEL_ERROR);
		return CKAHUM::srNotStarted;
	}

	CKAHFW::WorkingMode old_mode;
	
	if (CKAHFW::GetWorkingMode (&old_mode) != CKAHUM::srOK)
	{
		log.WriteFormat (_T("[CKAHFW::SETWM] Failed to get current working mode"), PEL_ERROR);
		return CKAHUM::srOpFailed;
	}
	
	if (mode == old_mode && !bForceSet)
	{
		log.WriteFormat (_T("[CKAHFW::SETWM] Mode not changed, nothing to do"), PEL_INFO);
		return CKAHUM::srOK;
	}
	
	if (old_mode == CKAHFW::wmBlockAll || old_mode == CKAHFW::wmAllowAll)
	{
		if (bApplicationClientRemove)
			g_ApplicationClient.RemoveBlockFilters ();
		if (bPacketClientRemove)
			g_PacketClient.RemoveBlockFilters ();
	}
	else
	{
		if (bApplicationClientRemove)
			g_ApplicationClient.RemoveDefaultAnswerFilters ();
	}

	switch (mode)
	{
	case CKAHFW::wmBlockAll:
		if (bApplicationClientAdd)
			g_ApplicationClient.AddBlockFilters (true);
		if (bPacketClientAdd)
			g_PacketClient.AddBlockFilters (true);
		break;
	case CKAHFW::wmRejectNotFiltered:
		if (bApplicationClientAdd)
			g_ApplicationClient.AddBlockDefaultFilters (true);
		break;
	case CKAHFW::wmAskUser:
		if (bApplicationClientAdd)
			g_ApplicationClient.AddAskUserDefaultFilters();
		break;
	case CKAHFW::wmAllowNotFiltered:
/*
		if (bApplicationClientAdd)
			g_ApplicationClient.AddBlockDefaultFilters (false);
*/
		break;
	case CKAHFW::wmAllowAll:
		if (bApplicationClientAdd)
			g_ApplicationClient.AddBlockFilters (false);
		if (bPacketClientAdd)
			g_PacketClient.AddBlockFilters (false);
		break;
	default:
		return CKAHUM::srInvalidArg;
	}

	InterlockedExchange((volatile long *)&g_WorkingMode, mode);

	log.WriteFormat (_T("[CKAHFW::SETWM] Mode '%s' is set"), PEL_INFO, GetWorkingModeString (mode));

	return CKAHUM::srOK;
}

CKAHUM::OpResult CKAHFW::SetFilteringMode( FilteringMode mode)
{
    DWORD CheckTdiAtNdis;

    if ( mode == fmMaxCompatibility )
        CheckTdiAtNdis = 1;
    else if ( mode == fmMaxSpeed )
        CheckTdiAtNdis = 0;
    else
        return CKAHUM::srOpFailed;

    CKAHUM::OpResult Result = SendPluginMessage(
        CCKAHPlugin::ptFirewall, 
        PLUG_IOCTL, 
        NDIS_FIREWALL_PLUGIN_NAME, 
        MSG_CHECK_TDI_AT_NDIS_LEVEL, 
        &CheckTdiAtNdis, 
        sizeof ( CheckTdiAtNdis ), 
        NULL, 
        0);

    return Result;
}

CKAHUM::OpResult CKAHFW::GetFilteringMode( FilteringMode mode)
{
    return CKAHUM::srOpFailed;
}

static void PrintMode(CKAHFW::WorkingMode mode)
{
	log.WriteFormat (_T("[CKAHFW::GETWM] Mode is '%s'"), PEL_INFO, GetWorkingModeString (mode));
}

CKAHUM::OpResult CKAHFW::GetWorkingMode (WorkingMode *mode)
{
	log.WriteFormat (_T("[CKAHFW::GETWM] Getting mode..."), PEL_INFO);

	if (!mode)
	{
		log.WriteFormat (_T("[CKAHFW::GETWM] Invalid argument"), PEL_ERROR);
		return CKAHUM::srInvalidArg;
	}

	if (!g_ApplicationClient.IsCreated () || !g_PacketClient.IsCreated ())
	{
		log.WriteFormat (_T("[CKAHFW::GETWM] Firewall not started"), PEL_ERROR);
		return CKAHUM::srNotStarted;
	}

	if(g_WorkingMode != -1)
		return *mode = g_WorkingMode, PrintMode(*mode), CKAHUM::srOK; // return cached value if exists

	switch (g_ApplicationClient.GetBlockAllowAllFiltersState ())
	{
	case CApplicationClient::fBlock:
		return *mode = wmBlockAll, InterlockedExchange((volatile long *)&g_WorkingMode, *mode), PrintMode(*mode), CKAHUM::srOK;
	case CApplicationClient::fAllow:
		return *mode = wmAllowAll, InterlockedExchange((volatile long *)&g_WorkingMode, *mode), PrintMode(*mode), CKAHUM::srOK;
	}

	switch (g_ApplicationClient.GetDefaultAnswerFiltersState ())
	{
	case CApplicationClient::fBlock:
		return *mode = wmRejectNotFiltered, InterlockedExchange((volatile long *)&g_WorkingMode, *mode), PrintMode(*mode), CKAHUM::srOK;
	case CApplicationClient::fAllow:
		return *mode = wmAskUser, InterlockedExchange((volatile long *)&g_WorkingMode, *mode), PrintMode(*mode), CKAHUM::srOK;
	case CApplicationClient::fNotFound:
		return *mode = wmAllowNotFiltered, InterlockedExchange((volatile long *)&g_WorkingMode, *mode), PrintMode(*mode), CKAHUM::srOK;
	}

	PrintMode(*mode);

	return CKAHUM::srOpFailed;
}

class CWinHandleGuard
{
    HANDLE m_handle;

    void close()
    {
        if (m_handle != INVALID_HANDLE_VALUE)
        {
            ::CloseHandle(m_handle);
            m_handle = INVALID_HANDLE_VALUE;
        }
    }

public:

    CWinHandleGuard() : m_handle (INVALID_HANDLE_VALUE) {}

    CWinHandleGuard(HANDLE handle) : m_handle(handle) {}

    ~CWinHandleGuard() { close(); }

    operator HANDLE () const { return m_handle; }
    HANDLE get() const { return m_handle; }

    bool isvalid() const { return m_handle != INVALID_HANDLE_VALUE; }

    CWinHandleGuard& operator = (HANDLE handle)
    {
        close();
        m_handle = handle;
        return *this;
    }
};

CWinHandleGuard g_hgKlif;

HANDLE IDriver_GetDriverHandleQuick();

void* __cdecl
ImpMemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag
	)
{	
	void* ptr = HeapAlloc( GetProcessHeap(), 0, size );
	return ptr;
};

void __cdecl
ImpMemFree (
	PVOID pOpaquePtr,
	void** pptr
	)
{
	if (*pptr)
	{
		HeapFree( GetProcessHeap(), 0 , *pptr );
		*pptr = NULL;
	}
};

CKAHUM::OpResult CKAHFW::GetApplicationChecksumVersion (OUT PULONG pVersion)
{
	*pVersion = 0;

	PVOID pDriverContext = NULL;
	HRESULT hResult = DRV_Register (
		&pDriverContext,
		AVPG_Driver_Specific,
		AVPG_INFOPRIORITY,
		_CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_WITHOUTWATCHDOG,
		0,
		DEADLOCKWDOG_TIMEOUT,
		ImpMemAlloc,
		ImpMemFree,
		0
		);

	if (SUCCEEDED(hResult) )
	{
		hResult = DRV_GetHashVersion( pDriverContext, pVersion );
		if (SUCCEEDED( hResult ))
			log.WriteFormat (_T("[GetApplicationChecksumVersion] hash version: 0x%08X"), PEL_ERROR,  *pVersion );
		else
			log.WriteFormat (_T("[GetApplicationChecksumVersion] Unable to get hashfunc version from klif: 0x%08X"), PEL_ERROR,  hResult );

		DRV_UnRegister( &pDriverContext );
	}
	else
		log.WriteFormat (_T("[GetApplicationChecksumVersion] connect to driver failed 0x%x"), PEL_ERROR, hResult );

	return CKAHUM::srOK;
}


CKAHUM::OpResult CKAHFW::GetApplicationChecksum (IN LPCWSTR szwApplicationPath, OUT UCHAR Checksum[FW_HASH_SIZE])
{	
	PVOID pDriverContext = NULL;
	HRESULT hResult = DRV_Register (
		&pDriverContext,
		AVPG_Driver_Specific,
		AVPG_INFOPRIORITY,
		_CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_WITHOUTWATCHDOG,
		0,
		DEADLOCKWDOG_TIMEOUT,
		ImpMemAlloc,
		ImpMemFree,
		0
		);

	if (!SUCCEEDED(hResult) )
	{
		log.WriteFormat (_T("[GetApplicationChecksum] connect to driver failed 0x%x"), PEL_ERROR, hResult );
		return CKAHUM::srOpFailed;
	}

	bool bHashCalculated = false;
	HANDLE hFile = INVALID_HANDLE_VALUE;

	CWow64FsRedirectorDisabler Wow64FsRedirectorDisabler;
	hFile = CreateFileW(szwApplicationPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if ( INVALID_HANDLE_VALUE == hFile )
	{
		log.WriteFormat (_T("Unable to open File '%S'"), PEL_ERROR, szwApplicationPath );
		return CKAHUM::srOpFailed;
	}
	else
	{
		log.WriteFormat (_T("Opened file to get hash. %ls"), PEL_ERROR,  szwApplicationPath );

		FIDBOX2_REQUEST_DATA FidBox2;
		hResult = DRV_GetFileFB2( pDriverContext, hFile, &FidBox2 );

		/*log.WriteFormat (_T("[DRIVEREXCHANGE] Get file fb2 result 0x%x (len %d fmtid: 0x%x, volid: 0x%x...0x%x, fid: 0x%x...0x%x, cnt: 0x%x, ft: 0x%x-0x%x)"), 
			PEL_ERROR, hResult,
			sizeof(FidBox2),
			FidBox2.m_DataFormatId,
			FidBox2.m_VolumeID[0], FidBox2.m_VolumeID[sizeof(FidBox2.m_VolumeID) - 1],
			FidBox2.m_FileID[0], FidBox2.m_FileID[sizeof(FidBox2.m_FileID) - 1],
			FidBox2.m_Counter,
			FidBox2.m_ModificationTime.HighPart, FidBox2.m_ModificationTime.LowPart
			);*/
		
		if (SUCCEEDED( hResult ))
		{
			bHashCalculated = TRUE;
			
			md5_state_t state;
			md5_init( &state );
			md5_append( &state, (UCHAR*)&FidBox2, sizeof(FidBox2) );
			md5_finish( &state, (UCHAR*)Checksum );
		}
	}

	DRV_UnRegister( &pDriverContext );

	if (!bHashCalculated)
	{
		md5_state_t state;

		md5_init(&state);

		md5_byte_t pBuffer[32 * 1024];

		DWORD dwRead = 0;

		do
		{
			ReadFile(hFile, pBuffer, sizeof (pBuffer), &dwRead, NULL);
			md5_append(&state, pBuffer, dwRead);
		} while(dwRead == sizeof(pBuffer));

		md5_finish(&state, Checksum);

		bHashCalculated = true;
	}

	CloseHandle(hFile);

	if (bHashCalculated)
		return CKAHUM::srOK;

	return CKAHUM::srOpFailed;
}
