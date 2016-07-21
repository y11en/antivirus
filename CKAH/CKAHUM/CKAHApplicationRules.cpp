#include "stdafx.h"
#include <CKAH/ckahrules.h>
#include "CKAHApplicationRules.h"
#include "../ckahrules/ckahrulesint.h"
#include "PathFuncs.h"
#include "Utils.h"

bool FillApplicationRuleFromFLTLIST (const FLTLIST &flt_list, CApplicationRule &ApplicationRule)
{
	FLTLIST::const_iterator i;

	PFILTER_PARAM pFilterParam = NULL;

	for (i = flt_list.begin (); i != flt_list.end (); ++i)
	{
		pFilterParam = i->FindParam (PACKET_FILTER_WIZARD_DATA);

		if  (pFilterParam)
			break;			
	}

	if (pFilterParam)
	{
		PoliType (CMemStorage (pFilterParam->m_ParamValue, pFilterParam->m_ParamSize)) << ApplicationRule;

		return true;
	}
		
	return false;
}

static bool ExportApplicationRuleItem (const CApplicationRuleElementItem &item, IN OUT FLTLIST &flt_list, USHORT Proto, bool bIsInbound,
                                       const std::vector<BYTE> &AppNameBuffer, bool bIsCmdLine, LPCWSTR CmdLine, DWORD dwFilterFlags)
{
	FLTLIST fltlist_remote_ip, fltlist_remote_ports, fltlist_local_ports;

	CGuardFilter Initiator;

	DWORD dwMajor;

	if (Proto == CKAHFW::PROTO_TCP)
	{
		DWORD direction = bIsInbound ? FW_CONNECT_INCOMING : FW_CONNECT_OUTGOING;
		Initiator.AddParam (FW_ID_INITIATOR, FLT_PARAM_EUA, 4, &direction);
		dwMajor = FW_EVENT_CONNECT;
	}
	else if (Proto == CKAHFW::PROTO_UDP)
	{
		BYTE Proto = (BYTE)CKAHFW::PROTO_UDP;
		Initiator.AddParam (FW_ID_PROTOCOL, FLT_PARAM_EUA, 1, &Proto);
		dwMajor = bIsInbound ? FW_EVENT_UDP_RECEIVE : FW_EVENT_UDP_SEND;

        if (item->m_StreamDirection != CKAHFW::sdBoth)
        {
		    DWORD stream = FW_STREAM_UNDEFINED;  
            switch (item->m_StreamDirection)
            {
            case CKAHFW::sdIncoming: stream = FW_STREAM_IN; break;
            case CKAHFW::sdOutgoing: stream = FW_STREAM_OUT; break;
            }
		    Initiator.AddParam (FW_ID_STREAM_DIRECTION, FLT_PARAM_EUA, 4, &stream);
        }
	}
    else
    {
        assert(0);
        return false;
    }

	if (item->m_LocalPorts->m_Ports.size ())
		ExportRulePortList (item->m_LocalPorts->m_Ports, fltlist_local_ports, false);
		
	if (item->m_RemotePorts->m_Ports.size ())
		ExportRulePortList (item->m_RemotePorts->m_Ports, fltlist_remote_ports, true);

	if (item->m_RemoteAddresses->m_Addresses.size ())
		ExportRuleIPList (item->m_RemoteAddresses->m_Addresses, fltlist_remote_ip, true);

	//Initiator.AddParam (FW_ID_FULL_APP_PATH, FLT_PARAM_EUA, ( wcslen ( (wchar_t*)&AppNameBuffer[0] ) + 1 ) << 1, (LPVOID)&AppNameBuffer[0]);
	Initiator.AddParam (FW_ID_FULL_APP_PATH, FLT_PARAM_MASKSENS_LISTW, AppNameBuffer.size(), (LPVOID)&AppNameBuffer[0]);

    if (bIsCmdLine)
	    //Initiator.AddParam (FW_ID_COMMAND_LINE, FLT_PARAM_WILDCARD, (CmdLine.size() + 1)*sizeof(WCHAR), (LPVOID)(CmdLine.c_str()));
	    Initiator.AddParam (FW_ID_COMMAND_LINE, FLT_PARAM_EUA, ( wcslen (CmdLine) + 1 )*sizeof(WCHAR), (LPVOID)CmdLine);

	FLTLIST flt_ret = fltlist_remote_ip * fltlist_remote_ports * fltlist_local_ports * Initiator;

    dwFilterFlags |= FLT_A_DISPATCH_MIRROR;

	flt_ret.SetOptions (FLTTYPE_KLIN, dwMajor, 0, dwFilterFlags);
	
	flt_list += flt_ret;

	return true;
}

static bool ExportApplicationRuleTCPItem (const CApplicationRuleElementItem &item, IN OUT FLTLIST &flt_list, 
									const std::vector<BYTE> &AppNameBuffer, bool bIsCmdLine, LPCWSTR CmdLine, DWORD dwFilterFlags)
{
    switch (item->m_StreamDirection)
    {
    case CKAHFW::sdBoth:
        return ExportApplicationRuleItem(item, flt_list, CKAHFW::PROTO_TCP, true,  AppNameBuffer, bIsCmdLine, CmdLine, dwFilterFlags) &&
               ExportApplicationRuleItem(item, flt_list, CKAHFW::PROTO_TCP, false, AppNameBuffer, bIsCmdLine, CmdLine, dwFilterFlags);
    case CKAHFW::sdIncoming:
        return ExportApplicationRuleItem(item, flt_list, CKAHFW::PROTO_TCP, true,  AppNameBuffer, bIsCmdLine, CmdLine, dwFilterFlags);
    case CKAHFW::sdOutgoing:
        return ExportApplicationRuleItem(item, flt_list, CKAHFW::PROTO_TCP, false, AppNameBuffer, bIsCmdLine, CmdLine, dwFilterFlags);
    default:
        assert(0);
        return false;
    }
}

static bool ExportApplicationRuleUDPItem (const CApplicationRuleElementItem &item, IN OUT FLTLIST &flt_list, 
									const std::vector<BYTE> &AppNameBuffer, bool bIsCmdLine, LPCWSTR CmdLine, DWORD dwFilterFlags)
{
    switch (item->m_PacketDirection)
    {
    case CKAHFW::pdBoth:
        return ExportApplicationRuleItem(item, flt_list, CKAHFW::PROTO_UDP, true,  AppNameBuffer, bIsCmdLine, CmdLine, dwFilterFlags) &&
               ExportApplicationRuleItem(item, flt_list, CKAHFW::PROTO_UDP, false, AppNameBuffer, bIsCmdLine, CmdLine, dwFilterFlags);
    case CKAHFW::pdIncoming:
        return ExportApplicationRuleItem(item, flt_list, CKAHFW::PROTO_UDP, true,  AppNameBuffer, bIsCmdLine, CmdLine, dwFilterFlags);
    case CKAHFW::pdOutgoing:
        return ExportApplicationRuleItem(item, flt_list, CKAHFW::PROTO_UDP, false, AppNameBuffer, bIsCmdLine, CmdLine, dwFilterFlags);
    default:
        assert(0);
        return false;
    }
}

bool GetPathParamBuffer(LPCWSTR szwAppPath, std::vector<BYTE> &buffer)
{
	const DWORD dwDefaultParamSize = MAX_PATH * 3;

	DWORD dwParamSize = dwDefaultParamSize;
	
	BYTE *PathBuff = new BYTE[dwParamSize];
	if (PathBuff == NULL)
		return false;

	if (!GetPathParam (szwAppPath, dwParamSize, PathBuff))
	{
		delete []PathBuff;

		if(dwParamSize <= dwDefaultParamSize)
			return false; // не из-за размера
		// не хватило
		PathBuff = new BYTE[dwParamSize];
		if (PathBuff == NULL)
			return false;

		if (!GetPathParam (szwAppPath, dwParamSize, PathBuff))
		{
			delete []PathBuff;
			return false;
		}
	}

	buffer.assign(&PathBuff[0], &PathBuff[dwParamSize]);

	delete []PathBuff;

	return true;
}

bool CreateFLTLISTFromApplicationRule (IN const CApplicationRule &ApplicationRule, IN ULONG GroupID, OUT FLTLIST &flt_list)
{
    FLTLIST fltlist_elements, fltlist_times;

	DWORD flag = (ApplicationRule.m_bIsBlocking) ? (FLT_A_DEFAULT | FLT_A_DENY) : FLT_A_DEFAULT;
	
	if (ApplicationRule.m_bLog)
		flag |= FLT_A_INFO | FLT_A_USERLOG;

	if (ApplicationRule.m_bNotify)
		flag |= FLT_A_INFO | FLT_A_ALERT1;	

	bool bExportResult = true;

	std::vector<BYTE> AppNameBuffer;
	GetPathParamBuffer(ApplicationRule.m_ApplicationName.c_str(), AppNameBuffer);
	if(AppNameBuffer.empty())
	{
		assert(!"Failed to get application path buffer");
		return false;
	}

    bool IsCmdLine = ApplicationRule.m_bIsCommandLine;
    std::vector<wchar_t> CmdLine;
    CmdLine.resize(ApplicationRule.m_CommandLine.size() + 1);
    wcscpy(&CmdLine[0], ApplicationRule.m_CommandLine.c_str());
    //CharUpperW(CmdLine.begin());
	_wcsupr(&CmdLine[0]);

    for (CApplicationRuleElementList::const_iterator i = ApplicationRule.m_Elements.begin(); i != ApplicationRule.m_Elements.end(); ++i)
    {
        if ((*i)->m_bIsActive) 
        {
	        switch ((*i)->m_Proto)
	        {
            case CKAHFW::PROTO_ALL:
                bExportResult &= ExportApplicationRuleTCPItem ((*i), fltlist_elements, AppNameBuffer, IsCmdLine, &CmdLine[0], flag) &&
                                 ExportApplicationRuleUDPItem ((*i), fltlist_elements, AppNameBuffer, IsCmdLine, &CmdLine[0], flag);
                break;
	        case CKAHFW::PROTO_TCP:
                bExportResult &= ExportApplicationRuleTCPItem ((*i), fltlist_elements, AppNameBuffer, IsCmdLine, &CmdLine[0], flag);
                break;
            case CKAHFW::PROTO_UDP:
                bExportResult &= ExportApplicationRuleUDPItem ((*i), fltlist_elements, AppNameBuffer, IsCmdLine, &CmdLine[0], flag);
                break;
            default:
                assert(0);
                bExportResult = false;
            }
        }
    }

    if (ApplicationRule.m_Times->m_Times.size ())
        ExportRuleTimeList (ApplicationRule.m_Times->m_Times, fltlist_times);

	FLTLIST flt_ret = fltlist_elements * fltlist_times;

	if (bExportResult && !flt_ret.empty ())
	{
		FLTLIST::iterator i = flt_ret.begin ();

		CMemStorage mems;
		PoliType (mems, false) << (CApplicationRule)ApplicationRule;

		i->AddParam (PACKET_FILTER_WIZARD_DATA, FLT_PARAM_NOP,
							mems.size_ (), mems.get_lin_space_ (), _FILTER_PARAM_FLAG_NONE);
		
		UUID uuid;
		UuidCreate (&uuid);

		for (i = flt_ret.begin (); i != flt_ret.end (); ++i)
		{
			i->AddParam (PF_FILTER_USER_ID, FLT_PARAM_NOP, sizeof (UUID), (void *)&uuid, _FILTER_PARAM_FLAG_MAP_TO_EVENT/*| _FILTER_PARAM_FLAG_CACHABLE*/);

			i->AddParam (PACKET_FILTER_NAME, FLT_PARAM_NOP, (ApplicationRule.m_Name.size () + 1) * sizeof (wchar_t), (void *)ApplicationRule.m_Name.c_str (), _FILTER_PARAM_FLAG_MAP_TO_EVENT );

			if (ApplicationRule.m_UserData.size () > 0)
				i->AddParam (PF_FILTER_USER_DATA, FLT_PARAM_NOP, ApplicationRule.m_UserData.size (), (LPVOID)&ApplicationRule.m_UserData[0], _FILTER_PARAM_FLAG_MAP_TO_EVENT);

//			if (bMD5Ok)
//			{
//				if (ApplicationRule.m_bIsBlocking)
//					i->AddParam (FW_ID_FILE_CHECKSUM, FLT_PARAM_NOP, sizeof (MD5Hash), MD5Hash, _FILTER_PARAM_FLAG_NONE );
//				else
//					i->AddParam (FW_ID_FILE_CHECKSUM, FLT_PARAM_EUA, sizeof (MD5Hash), MD5Hash, _FILTER_PARAM_FLAG_MUSTEXIST );
//			}

			ApplicationRule.m_bIsEnabled ? i->Enable () : i->Disable ();

            i->SetGroupID (GroupID);
		}

		if(ApplicationRule.m_bCheckHash &&
           ApplicationRule.m_AppHash.size() == FW_HASH_SIZE)
		{
			CGuardFilter WatchDog;
			
			WatchDog.AddParam(PF_FILTER_USER_ID, FLT_PARAM_NOP, sizeof(UUID), (void *)&uuid, _FILTER_PARAM_FLAG_MAP_TO_EVENT);
			//WatchDog.AddParam(FW_ID_FULL_APP_PATH, FLT_PARAM_EUA, ( wcslen ( (wchar_t*)&AppNameBuffer[0] ) + 1 ) << 1, (LPVOID)&AppNameBuffer[0]);
            WatchDog.AddParam(FW_ID_FULL_APP_PATH, FLT_PARAM_MASKSENS_LISTW, AppNameBuffer.size(), (LPVOID)&AppNameBuffer[0]);
			// через этот параметр будем получать новый хэш
			WatchDog.AddParam(FW_ID_FILE_CHECKSUM, FLT_PARAM_EUA, ApplicationRule.m_AppHash.size(), (LPVOID)&ApplicationRule.m_AppHash[0], _FILTER_PARAM_FLAG_MUSTEXIST | _FILTER_PARAM_FLAG_INVERS_OP);
			// через этот параметр будем получать старый хэш
			WatchDog.AddParam(ID_WATCHDOG_PARAM, FLT_PARAM_NOP, ApplicationRule.m_AppHash.size(), (LPVOID)&ApplicationRule.m_AppHash[0], _FILTER_PARAM_FLAG_MAP_TO_EVENT);
			
			WatchDog.m_HookID = FLTTYPE_KLIN;
			
			WatchDog.m_Flags = FLT_A_POPUP | FLT_A_DENY | FLT_A_DISPATCH_MIRROR;
			
			WatchDog.m_FunctionMj = FW_EVENT_CONNECT;
			
			if(!ApplicationRule.m_bIsBlocking)
			{
				flt_ret.push_front(WatchDog);

				WatchDog.m_FunctionMj = FW_EVENT_UDP_SEND;
				flt_ret.push_front(WatchDog);

				WatchDog.m_FunctionMj = FW_EVENT_UDP_RECEIVE;
				flt_ret.push_front(WatchDog);
			}
			else
			{
				flt_ret.push_back(WatchDog);
				
				WatchDog.m_FunctionMj = FW_EVENT_UDP_SEND;
				flt_ret.push_back(WatchDog);
				
				WatchDog.m_FunctionMj = FW_EVENT_UDP_RECEIVE;
				flt_ret.push_back(WatchDog);
			}				
		}
	}

    flt_list += flt_ret;

	return true;
}
