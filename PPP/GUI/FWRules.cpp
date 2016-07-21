// FWRules.cpp: implementation of the CAHFWAppRule2 class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "FWRules.h"
#include <plugin/p_win32_nfio.h>


/*CFwPresets g_FwPresets;

#define PROFILE_SID_ANTIHACKER_LOC      "Anti_Hacker_Localize"
#define PRESET_FILE                     "%Bases%\\FwPresets.ini"
#define PRESET_FILE_ADMPLG              "%ProductRoot%\\FwPresets.ini"

#define STR_OEL                         "\r\n"
#define STR_COMMA                       ", "
#define STR_SCLOPELEFT                  "("
#define STR_SCLOPERIGHT                 ")"
#define STR_IPSUBNETDELIMETER           "/"
#define STR_IPRANGEDELIMETER            "-"
#define STR_PORTRANGEDELIMETER          "-"
#define STR_TIMERANGEDELIMETER          "-"
#define STR_DOT                         "."

inline tString fwrGetString(LPCSTR strID)
{
	return g_pRoot->GetString(TOR(tString,()), PROFILE_LOCALIZE, PROFILE_SID_ANTIHACKER_LOC, (strID));
}


//-----------------------------------------------------------------------------

enum eTags
{
	eExecutable,
	eCmdLine,
	eUseCmdLine,
	eRuleName,
	eEnable,
	eAllow,
	eLog,
	eWarning,
	eProtocol,
	eDirection,
	eRemotePort,
	eRemoteHost,
	eRemoteHost2,
	eLocalPort,
	eLocalHost,
	eLocalHost2,
	eTimeRange,
	eIcmpCode,
	eUnknown
};

static char *g_Keys[]=
{
	"App",
	"CommandLine",
	"UseCommandLine",
	"Name",
	"Enable",
	"Allow",
	"Log",
	"Warning",
	"Protocol",
	"Direction",
	"RemotePort",
	"RemoteAddress",
	"RemoteAddress2",
	"LocalPort",
	"LocalAddress",
	"LocalAddress2",
	"TimeRange",
	"IcmpCode"
};

static char *g_Directions[]=
{
	"",
	"Inbound",
	"Outbound",
	"InboundOutbound",
	"InboundStream",
	"OutboundStream"
};

//-----------------------------------------------------------------------------

inline static void skipspace(char *&str)
{
	while(*str == ' ') str++;
}

inline static void nextfield(char *&str, char delimeter = ',')
{
	if( str = strchr(str, delimeter) )
		skipspace(++str);
}

inline static bool isdigit(char symbol)
{
	return symbol >= '0' && symbol <='9';
}

static char *getline(char *&str)
{
	if(!str)
		return str;
	
	char *tmp = str;
	char *eol = strchr(str, 0x0d);
	if(eol)
	{
		*eol++ = 0;
		if(*eol == 0x0a)
			*eol++ = 0;
	}
	str = eol;
	
	skipspace(tmp);
	return tmp;
}

//-----------------------------------------------------------------------------

static cIP Str2Ip(const char *sIp)
{
    cIP ip;
	ip.FromStr(sIp);
	return ip;
}*/

//-----------------------------------------------------------------------------

static void AddPort(tWORD &nPortLo, tWORD &nPortHi, cFwPorts &aPorts)
{
	if(nPortLo)
	{
		cFwPort &Port = aPorts.push_back();
		Port.m_PortLo = Port.m_PortHi = nPortLo;
		if(nPortHi)
		{
			if(nPortHi > nPortLo)
				Port.m_PortHi = nPortHi;
			else
				Port.m_PortLo = nPortHi;
		}
	}
	nPortLo = nPortHi = 0;
}

// string format: 3, 7-9, 25

static bool ParsePortListStr(char *sPorts, cFwPorts &aPorts)
{
	aPorts.clear();

	tWORD nPortLo = 0;
	tWORD nPortHi = 0;
	tWORD * pPortToScan = &nPortLo;
	
	for(; *sPorts; sPorts++)
	{
		if(isdigit(*sPorts))
		{
            int port = atoi(sPorts);
            if (port > 0 && port <= 65535)
            {
			    *pPortToScan = (tWORD)port;
			    for(; isdigit(*sPorts); sPorts++); sPorts--;
			    continue;
            }
            else
                return false;
		}
		else if(*sPorts == ',')
		{
			AddPort(nPortLo, nPortHi, aPorts);
			pPortToScan = &nPortLo;
			continue;
		}
		else if(*sPorts == '-')
		{
			pPortToScan = &nPortHi;
			continue;
		}
        else if(*sPorts == ' ' || *sPorts == '\t')
        {
            continue;
        }
        else
            return false;
	}
	AddPort(nPortLo, nPortHi, aPorts);
    return true;
}
/*
// string format: 127.0.0.1, 127.0.0.1 - 127.0.0.3, 1.2.3.4:255.255.255.0, %dhcp%, %dns%, %wins%

static void AddHost(char *&sDnsName, cIP& IpLo, cIP& IpHi, cIP& Mask, cVector<cFWAddress> &aAddrs)
{
    if(IpLo.IsValid())
	{
		cFWAddress &Addr = aAddrs.push_back();
		if(sDnsName)
		{
			Addr.m_Address = sDnsName;
			Addr.m_AddressIP.push_back(IpLo);
		}
		else
		{
			Addr.m_IPLo = Addr.m_IPHi = IpLo;
			if(IpHi.IsValid())
				Addr.m_IPHi = IpHi;
			if(Mask.IsValid())
				Addr.m_IPMask = Mask;
		}
	}
	IpLo = IpHi = Mask = cIP();
	sDnsName = NULL;
}

static bool IsIPAddressChar(char ch)
{
	return (isdigit(ch) || ch == ':' || ch == '.' ||
		ch >= 'A' && ch <= 'F' ||
		ch >= 'a' && ch <= 'f');
}

static void ParseHostListStr(char *sAddrs, cVector<cFWAddress> &aAddrs)
{
	aAddrs.clear();

	char *sDnsName = NULL;
	cIP IpLo;
	cIP IpHi;
	cIP Mask;
	cIP * pAddrToScan = &IpLo;

	for(; *sAddrs; sAddrs++)
	{
		if(*sAddrs == ' ')
			continue;
		if(IsIPAddressChar(*sAddrs))
		{
			const char* pAddr = sAddrs;
			for(; IsIPAddressChar(*sAddrs); sAddrs++);
            if(pAddrToScan == &Mask && IpLo.IsV6())
                pAddrToScan->SetV6Mask( atoi(tString(pAddr, sAddrs-pAddr).c_str()), IpLo.m_V6.m_Zone );
            else
                *pAddrToScan = Str2Ip( tString(pAddr, sAddrs-pAddr).c_str() );
			sAddrs--;
			continue;
		}		
		if(*sAddrs == ',')
		{
			AddHost(sDnsName, IpLo, IpHi, Mask, aAddrs);
			pAddrToScan = &IpLo;
			continue;
		}
		if(*sAddrs == '-')
		{
			pAddrToScan = &IpHi;
			continue;
		}		
		if(*sAddrs == '/')
		{
			pAddrToScan = &Mask;
			continue;
		}
		if(*sAddrs == '%')
		{
			char *tmp = sAddrs;
			for(; *tmp && *tmp != ' ' && *tmp != ','; tmp++);
			bool bEOL = !*tmp;
			*tmp = 0;

			cFWAddress addr;
			addr.m_Address = sAddrs;
			if( addr.RecognizeAddress() != _fwaUnknown )
				aAddrs.push_back(addr);

			sAddrs = bEOL ? tmp - 1 : tmp;
			continue;
		}
		if(*sAddrs == ')')
			continue;
		sDnsName = sAddrs;
		for(; *sAddrs && *sAddrs != '('; sAddrs++);
		*sAddrs = 0;
	}
	AddHost(sDnsName, IpLo, IpHi, Mask, aAddrs);
}

static void ParseTimeStr(char *sTime, tDWORD &nTimeFrom, tDWORD &nTimeTill)
{
	nTimeFrom = nTimeTill = 0;

	if( sTime && isdigit(*sTime) )
		nTimeFrom = atoi(sTime);
	if( sTime )
		nextfield(sTime, '-');
	if( sTime && isdigit(*sTime) )
		nTimeTill = atoi(sTime);
}



//-----------------------------------------------------------------------------

static bool ParsePresetLine(cFwAppRulePresets &aPresets, cFwAppRulePreset *&pCurPreset, cFwBaseRule *&pCurRule, char *sLine)
{
	if( *sLine == '[' )
	{
		pCurPreset = NULL;
		char *tmp = strchr(sLine, ']');
		if(tmp)
		{
			*tmp = 0;
			
			pCurPreset = &aPresets.push_back();
			pCurPreset->m_sName = sLine + 1;
			return true;
		}
	}
	if( !pCurPreset )
		return false;

	char *sData = sLine;
	eTags eCurTag;
	for(eCurTag = eExecutable; eCurTag != eUnknown; eCurTag = (eTags)((unsigned)eCurTag + 1))
	{
		int nLen = strlen(g_Keys[eCurTag]);
		if( !strncmp(sData, g_Keys[eCurTag], nLen) && sData[nLen] == '=' )
		{
			sData += nLen + 1;
			skipspace(sData);
			break;
		}
	}
	
	switch(eCurTag)
	{
	case eUnknown:
		return false;

	case eExecutable:
		while(sData)
		{
			char *tmp = strchr(sData, ',');
			if(tmp)
			{
				*tmp++ = 0;
				skipspace(tmp);
			}
			pCurPreset->m_aApps.push_back(sData);
			sData = tmp;
		}
		return true;

	case eCmdLine:
		pCurPreset->m_sCmdLine = sData;
		return true;
	
	case eUseCmdLine:
		pCurPreset->m_bUseCmdLine = *sData == '1';
		return true;

	case eIcmpCode:
		pCurPreset->m_nIcmpCode = atoi(sData);
		return true;

	case eRuleName:
		pCurRule = &pCurPreset->m_aRules.push_back();
		pCurRule->m_sName = sData;
		return true;
	}

	if( !pCurRule )
		return false;

	switch(eCurTag)
	{
	case eEnable:
		pCurRule->m_bEnabled = *sData == '1';
		break;

	case eAllow:
		pCurRule->m_bBlocking = *sData != '1';
		break;

	case eLog:
		pCurRule->m_bLog = *sData == '1';
		break;

	case eWarning:
		pCurRule->m_bWarning = *sData == '1';
		break;

	case eProtocol:
		if( !strcmp(sData, "TCP") )
			pCurRule->m_nProto = _fwPROTO_TCP;
		else
		if( !strcmp(sData, "UDP") )
			pCurRule->m_nProto = _fwPROTO_UDP;
		else
		if( pCurPreset->IsPacketPreset() )
			pCurRule->m_nProto = atoi(sData);
		break;
	
	case eDirection:
		{
			pCurRule->m_eDirection = _fwInboundOutbound;
			for(int i = 1; i < countof(g_Directions); i++)
				if(!strcmp(sData, g_Directions[i]))
					pCurRule->m_eDirection = (_eFwDirection)i;
		}
		break;

	case eRemotePort:
		ParsePortListStr(sData, pCurRule->m_aRemotePorts);
		break;
	
	case eLocalPort:
		ParsePortListStr(sData, pCurRule->m_aLocalPorts);
		break;
	
	case eRemoteHost:
	case eRemoteHost2:
		ParseHostListStr(sData, pCurRule->m_aRemoteAddresses);
		break;

	case eLocalHost:
	case eLocalHost2:
		if( pCurPreset->IsPacketPreset() )
			ParseHostListStr(sData, pCurRule->m_aLocalAddresses);
		break;
		
	case eTimeRange:
		ParseTimeStr(sData, pCurRule->m_nTimeFrom, pCurRule->m_nTimeTill);
	}

	return true;
}

static bool ParsePresets(cFwAppRulePresets &aPresets, char *sPresets)
{
	cFwBaseRule *pRule = NULL;
	cFwAppRulePreset *pPreset = NULL;
	
	for(char *sLine; sLine = getline(sPresets); )
	{
		switch(*sLine)
		{
		case 0:
		case ';': break;
		
		default:
			ParsePresetLine(aPresets, pPreset, pRule, sLine);
		}
	}
	return true;
}

//-----------------------------------------------------------------------------

inline static LPCSTR GetFileName(LPCSTR sFilePath)
{
	LPCSTR sFile = max(strrchr(sFilePath, '\\'), strrchr(sFilePath, '/'));
	return sFile ? sFile + 1 : sFilePath;
}

inline static cStringObj GetFileName(cStringObj &sFilePath)
{
	tDWORD nPos = sFilePath.find_last_of((tSTRING)"\\/");
	return nPos != cSTRING_EMPTY_LENGTH ? sFilePath.substr(nPos + 1) : sFilePath;
}

static tString ProtoAlias(tBYTE nProto)
{
	if( nProto == _fwPROTO_TCP ) return "TCP";
	if( nProto == _fwPROTO_UDP ) return "UDP";
	
	char strProto[40];
	sprintf(strProto, "%d", nProto);
	return strProto;
}

//-----------------------------------------------------------------------------

static void ExportRuleProp(tString &strBuff, eTags eTag, LPCSTR strVal = NULL)
{
	strBuff += g_Keys[eTag];
	strBuff += '=';
	strBuff += strVal;
	strBuff += STR_OEL;
}

static void ExportRuleHdr(tString &strBuff, LPCSTR sFilePath, LPCSTR sCmdLine, bool bUseCmdLine)
{
	LPCSTR sFile = GetFileName(sFilePath);

	strBuff += '[';
	strBuff += sFile;
	strBuff += ']';
	strBuff += STR_OEL;

	ExportRuleProp(strBuff, eExecutable, sFilePath);
	
	ExportRuleProp(strBuff, eCmdLine, sCmdLine);
	
	ExportRuleProp(strBuff, eUseCmdLine, bUseCmdLine ? "1" : "0");
}

static void ExportRuleBase(tString &strBuff, cFwBaseRule *pRule)
{
	ExportRuleProp(strBuff, eRuleName, pRule->m_sName.c_str(cCP_ANSI));

	ExportRuleProp(strBuff, eEnable, pRule->m_bEnabled ? "1" : "0");

	ExportRuleProp(strBuff, eAllow, pRule->m_bBlocking ? "0" : "1");

	ExportRuleProp(strBuff, eLog, pRule->m_bLog ? "1" : "0");

	ExportRuleProp(strBuff, eWarning, pRule->m_bWarning ? "1" : "0");

	ExportRuleProp(strBuff, eProtocol, ProtoAlias(pRule->m_nProto).c_str());

	ExportRuleProp(strBuff, eDirection, g_Directions[pRule->m_eDirection]);

	if( pRule->m_aRemotePorts.size() )
		ExportRuleProp(strBuff, eRemotePort, CFwRuleUtils::GetPortListStr(pRule->m_aRemotePorts).c_str());

	if( pRule->m_aLocalPorts.size() )
		ExportRuleProp(strBuff, eLocalPort, CFwRuleUtils::GetPortListStr(pRule->m_aLocalPorts).c_str());

	if( pRule->m_aRemoteAddresses.size() )
		ExportRuleProp(strBuff, eRemoteHost2, CFwRuleUtils::GetAddrListStr(pRule->m_aRemoteAddresses).c_str());

	if( pRule->m_aLocalAddresses.size() )
		ExportRuleProp(strBuff, eLocalHost2, CFwRuleUtils::GetAddrListStr(pRule->m_aLocalAddresses).c_str());

	if( pRule->m_nTimeFrom || pRule->m_nTimeTill )
	{
		char strTimeRange[40];
		sprintf(strTimeRange, "%d" STR_TIMERANGEDELIMETER "%d", pRule->m_nTimeFrom, pRule->m_nTimeTill);
		ExportRuleProp(strBuff, eTimeRange, strTimeRange);
	}
	
	if( pRule->m_nProto == _fwPROTO_ICMP && pRule->isBasedOn(cFwPacketRule::eIID) )
	{
		cFwPacketRule *pPacketRule = (cFwPacketRule *)pRule;
		if( pPacketRule->m_nIcmpCode )
		{
			char strIcmpCode[40];
			sprintf(strIcmpCode, "%d", pPacketRule->m_nIcmpCode);
			ExportRuleProp(strBuff, eIcmpCode, strIcmpCode);
		}
	}

	strBuff += STR_OEL;
}

//-----------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// CFwPresets

bool CFwPresets::Get()
{
	if(m_bInited)
		return true;
	
	m_bInited = CFwPresets::LoadPresets(PRESET_FILE, *this);
	if( !m_bInited )
		m_bInited = CFwPresets::LoadPresets(PRESET_FILE_ADMPLG, *this);

	return m_bInited;
}

void CFwPresets::Clear()
{
	clear();
	m_bInited = false;
}

bool CFwPresets::LoadPresets(LPCSTR strFile, cFwAppRulePresets &aPresets)
{
	cStringObj sFile = strFile;
	g_hGui->ExpandEnvironmentString(sFile);

	cAutoObj<cIO> hIo;
	if(PR_FAIL(g_root->sysCreateObject((hOBJECT *)&hIo, IID_IO, PID_WIN32_NFIO)))
		return false;

	if(PR_FAIL(hIo->propSetStr(0, pgOBJECT_FULL_NAME, sFile.c_str(cCP_ANSI))))
		return false;

	if(PR_FAIL(hIo->propSetDWord(pgOBJECT_OPEN_MODE, fOMODE_OPEN_IF_EXIST)))
		return false;

	if(PR_FAIL(hIo->propSetDWord(pgOBJECT_ACCESS_MODE, fACCESS_READ)))
		return false;
	
	if(PR_FAIL(hIo->sysCreateObjectDone()))
		return false;

	tQWORD nFileSize;
	if(PR_FAIL(hIo->GetSize(&nFileSize, IO_SIZE_TYPE_EXPLICIT)))
		return false;

	tDWORD size=(tDWORD)nFileSize;
	char *buffer;
	bool bSuccess = false;
	if(PR_SUCC(hIo->heapAlloc((void **)&buffer, size + 1)))
	{
		if(PR_SUCC(hIo->SeekRead(&size, 0, buffer, size)))
		{
			buffer[size] = 0;
			if( bSuccess = ParsePresets(aPresets, buffer) )
			{
				for(tDWORD i = 0; i < aPresets.size(); i++)
					aPresets[i].InitProps();
			}
		}
		hIo->heapFree(buffer);
	}
	return bSuccess;
}

void CFwPresets::ExportRule(tString &strBuff, cSerializable *pRule)
{
	if( pRule->isBasedOn(cFwPacketRule::eIID) )
	{
		strBuff += "[" FWPRESETNAME_PACKETRULE "]" STR_OEL;
		ExportRuleBase(strBuff, (cFwBaseRule *)pRule);
	}
	else if( pRule->isBasedOn(cFwAppRule::eIID) )
	{
		cFwAppRule Rule = *(cFwAppRule *)pRule;
		if( Rule.m_aRules.size() )
		{
			ExportRuleHdr(strBuff, Rule.m_sAppName.c_str(cCP_ANSI), Rule.m_sCmdLine.c_str(cCP_ANSI), !!Rule.m_bUseCmdLine);
			
			for(size_t i = 0; i < Rule.m_aRules.size(); i++)
				ExportRuleBase(strBuff, &Rule.m_aRules[i]);
		}
	}
//	else if( pRule->isBasedOn(cFwAppRuleEx::eIID) )
//	{
//		cFwAppRule &Rule = *(cFwAppRule *)pRule;
//
//		ExportRuleHdr(strBuff, Rule.m_sAppName.c_str(cCP_ANSI), Rule.m_sCmdLine.c_str(cCP_ANSI), !!Rule.m_bUseCmdLine);
//		ExportRuleBase(strBuff, (cFwBaseRule *)pRule);
//	}
}
static tString &staticString()  { static tString str; return str; }
static cStrObj &staticStringW() { static cStrObj str; return str; }

LPCSTR CFwPresets::Localize(CRootItem *pRoot, LPCSTR strPresetId, LPCSTR strPrefix)
{
	tString str = strPrefix; str += strPresetId;
	return pRoot->GetString(staticString(), PROFILE_LOCALIZE, PROFILE_SID_ANTIHACKER_LOC, str.c_str(), strPresetId);
}

LPCWSTR CFwPresets::LocalizeW(CRootItem *pRoot, LPCSTR strPresetId, LPCSTR strPrefix)
{
	tString str = strPrefix; str += strPresetId;
	return pRoot->LocalizeStr(staticStringW(), pRoot->GetString(TOR_tString, PROFILE_LOCALIZE, PROFILE_SID_ANTIHACKER_LOC, str.c_str(), strPresetId));
}

void CFwPresets::GetPresetsByApp(cStringObj &sAppName, cVector<cFwAppRulePreset *> &Presets)
{
	const cStringObj &strFile = GetFileName(sAppName);
	for(size_t i = 0; i < size(); i++)
	{
		cFwAppRulePreset &Preset = at(i);
		for(size_t j = 0; j < Preset.m_aApps.size(); j++)
		{
			if(Preset.m_aApps[j].compare(strFile, fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ)
			{
				Presets.push_back(&Preset);
			}
		}
	}
}

void CFwPresets::GetPresetsByActivity(cAHFWAppEventReport *pAppEvent, cVector<cFwAppRulePreset *> &Presets)
{
	const cStringObj &strFile = GetFileName(pAppEvent->m_AppName);
	for(size_t i = 0; i < size(); i++)
	{
		cFwAppRulePreset &Preset = at(i);
		for(size_t j = 0; j < Preset.m_aApps.size(); j++)
		{
			if(Preset.m_aApps[j].compare(strFile, fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ)
			{
				if(CheckPresetActivity(&Preset, pAppEvent))
				{
					Presets.push_back(&Preset);
				}
			}
		}
	}
}

void CFwPresets::GetGeneralPresets(cVector<cFwAppRulePreset *> &Presets, bool bAll)
{
	for(size_t i = bAll ? 0 : 2; i < size(); i++)
		if( at(i).m_aApps.empty() && !at(i).IsPacketPreset() )
			Presets.push_back(&at(i));
}

bool CFwPresets::CheckPresetActivity(cFwAppRulePreset *pPreset, cAHFWAppEventReport *pAppEvent)
{
	for(size_t i = 0; i < pPreset->m_aRules.size(); i++)
	{
		cFwBaseRule &Rule = pPreset->m_aRules[i];
		
		if(!Rule.m_bEnabled)
			continue;
		
		if(Rule.m_nProto != pAppEvent->m_Proto)
			continue;

		switch(Rule.m_eDirection)
		{
		case _fwInbound:
		case _fwInboundStream:
			if(pAppEvent->m_Direction != _fwInbound &&  pAppEvent->m_Direction != _fwInboundStream)
				continue;
			break;
		
		case _fwOutbound:
		case _fwOutboundStream:
			if(pAppEvent->m_Direction != _fwOutbound &&  pAppEvent->m_Direction != _fwOutboundStream)
				continue;
			break;
		
		case _fwInboundOutbound:
			break;
		}

		if(!CheckPorts(Rule.m_aRemotePorts, pAppEvent->m_RemotePort))
			continue;

		if(!CheckPorts(Rule.m_aLocalPorts, pAppEvent->m_LocalPort))
			continue;

		if(!cFWAddress::IsMatch(pAppEvent->m_LocalAddress, Rule.m_aLocalAddresses))
			continue;

		if(!cFWAddress::IsMatch(pAppEvent->m_RemoteAddress, Rule.m_aRemoteAddresses))
			continue;

		return true;
	}
	return false;
}

bool CFwPresets::CheckPorts(cVector<cFWPort> &aPorts, tWORD nPort)
{
	for(size_t i = 0; i < aPorts.size(); i++)
	{
		cFWPort &Port = aPorts[i];
		if(Port.m_PortLo == Port.m_PortHi)
		{
			if(nPort == Port.m_PortLo)
				return true;
		}
		else
		{
			if(nPort >= Port.m_PortLo && nPort <= Port.m_PortHi)
				return true;
		}
	}
	return !aPorts.size();
}

//-----------------------------------------------------------------------------

cFwAppRule *CFwRuleUtils::FindRule(cFwAppRules &aRules, cStringObj &sAppName, tBYTE *pHash, cStringObj &sCmdLine, tBOOL bUseCmdLine, tBOOL bAddIfNotExist)
{
	cFwAppRule *pRule = cAHFWSettings::FindAppRule(aRules, sAppName, pHash, sCmdLine, bUseCmdLine);
	if( !pRule && bAddIfNotExist )
	{
		pRule = &aRules.push_back();
		pRule->m_sAppName = sAppName;
		if( pHash )
			pRule->CopyHash(pHash);
		pRule->m_sCmdLine = sCmdLine;
		pRule->m_bUseCmdLine = bUseCmdLine;
	}
	return pRule;
}

void CFwRuleUtils::ApplyPreset(cFwAppRule &AppRule, cFwAppRulePreset *pPreset, tBOOL bBlocking)
{
	if( !pPreset )
		return;

	for(tDWORD i = 0; i < pPreset->m_aRules.size(); i++)
	{
		cFwBaseRule &Rule = pPreset->m_aRules[i];
		tBOOL bBlockingSave = Rule.m_bBlocking;

		if( bBlocking )
			Rule.m_bBlocking = cTRUE;

		AppRule.AppendRule(Rule);

		Rule.m_bBlocking = bBlockingSave;
	}
}

void CFwRuleUtils::MakeKnownIpRule(cFwAppRule &AppRule, const cIP& RemoteAddress, tBOOL bBlocking)
{
	cFWAddress Addr;
	Addr.m_IPHi = Addr.m_IPLo = RemoteAddress;

	cFwBaseRule Rule;
	Rule.m_aRemoteAddresses.push_back(Addr);
	Rule.m_bBlocking = bBlocking;

	Rule.m_sName = fwrGetString(bBlocking ? "ahfwBlockHostTCP" : "ahfwAllowHostTCP").c_str();
	Rule.m_nProto = _fwPROTO_TCP;
	AppRule.AppendRule(Rule);

	Rule.m_sName = fwrGetString(bBlocking ? "ahfwBlockHostUDP" : "ahfwAllowHostUDP").c_str();
	Rule.m_nProto = _fwPROTO_UDP;
	AppRule.AppendRule(Rule);

	AppRule.InitProps();
}

void CFwRuleUtils::App2AppEx(cFwAppRules &aRules, cFwAppRulesEx &aRulesEx)
{
	aRulesEx.clear();
	for(tDWORD i = 0; i < aRules.size(); i++)
	{
		cFwAppRule &Rule = aRules[i];
		for(tDWORD j = 0; j < Rule.m_aRules.size(); j++)
		{
			cFwAppRuleEx &RuleEx = aRulesEx.push_back();

			*(cFwBaseRule *)&RuleEx = Rule.m_aRules[j];
			RuleEx.m_sAppName    = Rule.m_sAppName;
			RuleEx.m_sCmdLine    = Rule.m_sCmdLine;
			RuleEx.m_bUseCmdLine = Rule.m_bUseCmdLine;
			RuleEx.CopyHash(Rule.m_nHash);
		}
	}
	aRules.clear();
}

void CFwRuleUtils::AppEx2App(cFwAppRulesEx &aRulesEx, cFwAppRules &aRules)
{
	aRules.clear();
	for(tDWORD i = 0; i < aRulesEx.size(); i++)
	{
		cFwAppRuleEx &RuleEx = aRulesEx[i];

		cFwAppRule &Rule = *FindRule(aRules, RuleEx.m_sAppName, RuleEx.m_nHash, RuleEx.m_sCmdLine, RuleEx.m_bUseCmdLine, cTRUE);
		if( !Rule.m_aRules.size() )
			Rule.m_bEnabled = cFALSE;
		
		Rule.AppendRule(*(cFwBaseRule*)&RuleEx);
		Rule.m_bEnabled |= RuleEx.m_bEnabled;
	}
	aRulesEx.clear();
}

int CFwRuleUtils::CompareAddrs(cFWAddress &Addr1, cFWAddress &Addr2)
{
	switch(Addr1.GetType() << 4 | Addr2.GetType())
	{
	case (_fwatSymbolic << 4) | _fwatSymbolic: return Addr1.m_Address.compare(Addr2.m_Address);
	case (_fwatSymbolic << 4) | _fwatSubnet:
	case (_fwatSymbolic << 4) | _fwatSingleIp:
	case (_fwatSymbolic << 4) | _fwatIpRange:  return 1;
	
	case (_fwatSubnet << 4)   | _fwatSymbolic: return -1;
	case (_fwatSubnet << 4)   | _fwatSubnet:
	case (_fwatSubnet << 4)   | _fwatSingleIp:
	case (_fwatSubnet << 4)   | _fwatIpRange:  return CFwRuleUtils::CompareIP(Addr1.m_IPLo, Addr2.m_IPLo);
	
	case (_fwatSingleIp << 4) | _fwatSymbolic: return -1;
	case (_fwatSingleIp << 4) | _fwatSubnet:
	case (_fwatSingleIp << 4) | _fwatSingleIp:
	case (_fwatSingleIp << 4) | _fwatIpRange:  return CFwRuleUtils::CompareIP(Addr1.m_IPLo, Addr2.m_IPLo);
	
	case (_fwatIpRange  << 4) | _fwatSymbolic: return -1;
	case (_fwatIpRange  << 4) | _fwatSubnet:
	case (_fwatIpRange  << 4) | _fwatSingleIp:
	case (_fwatIpRange  << 4) | _fwatIpRange:  return CFwRuleUtils::CompareIP(Addr1.m_IPLo, Addr2.m_IPLo);
	};
	return 0;
}

int CFwRuleUtils::CompareIP(const cIP& IP1, const cIP& IP2)
{
	if(IP1 > IP2) return 1;
	if(IP1 < IP2) return -1;
	return 0;
}

int CFwRuleUtils::ComparePorts(cFWPort &Port1, cFWPort &Port2)
{
	if(Port1.m_PortLo > Port2.m_PortLo) return 1;
	if(Port1.m_PortLo < Port2.m_PortLo) return -1;
	return 0;
}

bool CFwRuleUtils::IsIp(LPCTSTR strIp, tBYTE Version)
{
	switch (Version)
	{
	case 4:  { tIPv4 ip; return tIPv4FromStr(ip, strIp); }
	case 6:  { cIPv6 ip; return ip.FromStr(strIp); }
	default: { cIP   ip; return ip.FromStr(strIp); }
	}
}*/

tString CFwRuleUtils::Ip2Str(const cIP& Ip, tBOOL bMask)
{
	tString str;

	if( bMask && Ip.m_Version == 6 )
	{
		tCHAR buf[4];
		_snprintf_s(buf, sizeof(buf), _TRUNCATE, "%u", Ip.m_V6.GetMaskPrefixLength());
		str = &buf[0];
	}
	else
	{
		tSIZE_T size = Ip.ToStr(NULL, 0);
		PR_ASSERT( size );
		if( size )
		{
			std::vector<tCHAR> buf(size);
			size = Ip.ToStr(&buf[0], buf.size());
			PR_ASSERT( size );
			str = &buf[0];
		}
	}

	return str;
}

/*cIP CFwRuleUtils::Str2Ip(LPCTSTR strIp, tBYTE Version)
{
    cIP Ip;
	switch (Version)
	{
	case 4: Ip.FromStrV4(strIp); break;
	case 6: Ip.FromStrV6(strIp); break;
	default: Ip.FromStr(strIp); break;
	}
	return Ip;
}

tString CFwRuleUtils::Address2Str(const cFWAddress &Addr)
{
	tString desc;
	if(!Addr.m_Address.empty())                              // symbolic address
	{
		desc += Addr.m_Address.c_str(cCP_TCHAR);
		if(!Addr.m_AddressIP.empty())
		{
			desc += STR_SCLOPELEFT;
			for(size_t i = 0; i < Addr.m_AddressIP.size(); i++)
			{
				desc += i ? STR_COMMA : "";
				desc += Ip2Str(Addr.m_AddressIP[i]);
			}
			desc += STR_SCLOPERIGHT;
		}
	}
	else
	if(Addr.m_IPMask.IsValid())                              // subnet
	{
		desc += Ip2Str(Addr.m_IPLo) + STR_IPSUBNETDELIMETER + Ip2Str(Addr.m_IPMask, cTRUE);
	}
	else
	if(Addr.m_IPLo != Addr.m_IPHi)                           // range
	{
		desc += Ip2Str(Addr.m_IPLo) + STR_IPRANGEDELIMETER + Ip2Str(Addr.m_IPHi);
	}
	else
	if(Addr.m_IPLo == Addr.m_IPHi)                           // ip
	{
		desc += Ip2Str(Addr.m_IPLo);
	}
	return desc;
}

tString CFwRuleUtils::Port2Str(const cFWPort &Port)
{
	tString desc;
	TCHAR sPortNum[20];
	if(Port.m_PortLo == Port.m_PortHi)                       // port
	{
		desc += _itoa(Port.m_PortLo, sPortNum, 10);
	}
	else                                                     // range
	{
		desc += _itoa(Port.m_PortLo, sPortNum, 10);
		desc += STR_PORTRANGEDELIMETER;
		desc += _itoa(Port.m_PortHi, sPortNum, 10);
	}
	return desc;
}

tString CFwRuleUtils::GetAddrListStr(const cVector<cFWAddress> &Addrs)
{
	tString desc;
	for(size_t i = 0; i < Addrs.size(); i++)
	{
		desc += i ? STR_COMMA : "";
		desc += CFwRuleUtils::Address2Str(Addrs[i]);
	}
	return desc;
}

tString CFwRuleUtils::GetPortListStr(const cVector<cFWPort> &Ports)
{
	tString desc;
	for(size_t i = 0; i < Ports.size(); i++)
	{
		desc += i ? STR_COMMA : "";
		desc += CFwRuleUtils::Port2Str(Ports[i]);
	}
	return desc;
}*/

bool CFwRuleUtils::ParsePortListStr(LPCTSTR sPorts, cFwPorts &aPorts)
{
	return ::ParsePortListStr((char *)sPorts, aPorts);
}

/*bool CFwRuleUtils::CheckRule(cAHFWAppEventReport &AppEvent, cFwAppRule &AppRule)
{
	for(tDWORD i = 0; i < AppRule.m_aRules.size(); i++)
		if( CheckRule(AppEvent, AppRule.m_aRules[i]) )
			return true;
	
	return false;
}

bool CFwRuleUtils::CheckRule(cAHFWAppEventReport &AppEvent, cFwBaseRule &Rule)
{
	if( !Rule.m_bEnabled )
		return false;

	if( Rule.m_nProto != AppEvent.m_Proto )
		return false;

	switch(Rule.m_eDirection)
	{
	case _fwInbound:
	case _fwInboundStream:
		if( AppEvent.m_Direction != _fwInbound && AppEvent.m_Direction != _fwInboundStream )
			return false;
		break;
	
	case _fwOutbound:
	case _fwOutboundStream:
		if( AppEvent.m_Direction != _fwOutbound && AppEvent.m_Direction != _fwOutboundStream )
			return false;
		break;
	}
	
	if( !cFWAddress::IsMatch(AppEvent.m_RemoteAddress, Rule.m_aRemoteAddresses) )
		return false;
	
	if( !cFWPort::IsMatch(AppEvent.m_RemotePort, Rule.m_aRemotePorts) )
		return false;
	
	if( !cFWPort::IsMatch(AppEvent.m_LocalPort, Rule.m_aLocalPorts) )
		return false;

	return true;
}*/

/*
//-----------------------------------------------------------------------------

void CheckRuleContainment(cAHFWBaseRuleEx *pRule, CRuleContainerList &RuleList)
{
	// проверим на содержание правила в списке
	CRuleContainerList::const_iterator j;

	pRule->m_aConflictedRules.clear();

	// создаем контейнер для правила
	CRuleContainer Container( pRule );

	// сохраним оригинальный контейнер для добавления в список
	CRuleContainer SavedContainer ( Container );

	// вычтем из контейнера все пересекающиеся куски
	for (j = RuleList.begin(); j != RuleList.end(); ++j)
	{
		const CRuleContainer &ListContainer = *j;

		if ( Container.Subtract ( ListContainer ) )
		{
			// если есть пересечение, добавим в список накрывающих правил
			pRule->m_aConflictedRules.push_back ( ListContainer.GetRule() );
		}
	}

	// осталось ли что-то ненакрытым?
	bool bIsContainerEmpty = Container.IsEmpty();

	if ( !bIsContainerEmpty )
	{
		// очистим список накрывающих
		pRule->m_aConflictedRules.clear();
	}

	// включим в список то, что осталось
	if ( !bIsContainerEmpty && pRule->m_bEnabled )
	{
		RuleList.push_back( SavedContainer );
	}
}

void CheckRuleContainment(cAHFWBaseRuleEx *pRule, cVector<CAHFWPortRule> *pPortRules, cVector<CAHFWAppRule> *pAppRules)
{
	if(!pRule)
		return;
	
	CRuleContainerList RuleList;
	
	pRule->m_aConflictedRules.clear();
	if(pPortRules)
	{
		for(int i = 0; i < pPortRules->size(); i++)
		{
			CheckRuleContainment(&pPortRules->at(i), RuleList);
			if(pRule == &pPortRules->at(i))
				break;
		}
	}
	if(pAppRules)
	{
		for(int i = 0; i < pAppRules->size(); i++)
		{
			CheckRuleContainment(&pAppRules->at(i), RuleList);
			if(pRule == &pPortRules->at(i))
				break;
		}
	}
}

//-----------------------------------------------------------------------------

CRuleContainer::CRuleContainer(cSerializable *pRule) :
	m_pRule(pRule)
{
	if(pRule->isBasedOn(cAHFWPortRuleEx::eIID)) CreateFromPacketRule((cAHFWPortRuleEx *)pRule);
	if(pRule->isBasedOn(cAHFWAppRuleEx::eIID))  CreateFromAppRule((cAHFWAppRuleEx *)pRule);

#ifdef _TRACECONT
	for (CRuleContainerItemList::const_iterator i = m_Items.begin(); i != m_Items.end(); ++i )
		(*i).Dump();
#endif

}

#define	ntohl(netlong)                      (netlong)

void CRuleContainer::CreateFromPacketRule(cAHFWPortRuleEx *pRule)
{
	if(!pRule)
		return;

	CRuleContainerItem Item;

	// направление
	switch ( pRule->m_eDirection )
	{
	case _fwprdIncoming:
		Item.m_Vector[rciDirection][1] = 0;
		break;
	case _fwprdOutgoing:
		Item.m_Vector[rciDirection][0] = 1;
		break;
	}

	// протокол
	BYTE Proto = pRule->m_nProtocol == _fwPROTO_ALL ? pRule->m_nMinorProtocol : pRule->m_nProtocol;
	if ( Proto )
		Item.m_Vector[rciProto][0] = Item.m_Vector[rciProto][1] = Proto;

	// ICMP code
	if ( Proto == _fwPROTO_ICMP )
	{
		USHORT IcmpCode = pRule->m_nICMPCode;
		if ( IcmpCode != 0xFFFF )
			Item.m_Vector[rciIcmpCode][0] = Item.m_Vector[rciIcmpCode][1] = IcmpCode;
	}

	CreateFromRuleElement(Item, &pRule->m_aRemoteAddresses, &pRule->m_aLocalAddresses, &pRule->m_aRemotePorts, &pRule->m_aLocalPorts);
}

void CRuleContainer::CreateFromAppRule(cAHFWAppRuleEx *pRule)
{
	if(!pRule)
		return;

	m_strAppName = pRule->m_sAppName.c_str(cCP_TCHAR);
	if ( m_strAppName.empty() )
		m_strAppName = "*";

	CRuleContainerItem Item;

	switch(pRule->m_eRuleType)
	{
	case ahfwCategoryRule:
`		if(cAHFWAppCategory *pCat = g_AppRuleDef.GetAppCategory(pRule->m_nCategory))
		{
			Item.m_Vector[rciProto][0] = Item.m_Vector[rciProto][1] = ahfwTCP;
			Item.m_Vector[rciDirection][0] = Item.m_Vector[rciDirection][1] = 1;
			CreateFromRuleElement(Item, &pCat->m_TCPOutgoing.m_RemoteAddresses, NULL, &pCat->m_TCPOutgoing.m_RemotePorts, &pCat->m_TCPOutgoing.m_LocalPorts);
			
			Item.m_Vector[rciProto][0] = Item.m_Vector[rciProto][1] = ahfwTCP;
			Item.m_Vector[rciDirection][0] = Item.m_Vector[rciDirection][1] = 0;
			CreateFromRuleElement(Item, &pCat->m_TCPIncoming.m_RemoteAddresses, NULL, &pCat->m_TCPIncoming.m_RemotePorts, &pCat->m_TCPIncoming.m_LocalPorts);

			Item.m_Vector[rciProto][0] = Item.m_Vector[rciProto][1] = ahfwUDP;
			Item.m_Vector[rciDirection][0] = Item.m_Vector[rciDirection][1] = 1;
			CreateFromRuleElement(Item, &pCat->m_UDPOutgoing.m_RemoteAddresses, NULL, &pCat->m_UDPOutgoing.m_RemotePorts, &pCat->m_UDPOutgoing.m_LocalPorts);

			Item.m_Vector[rciProto][0] = Item.m_Vector[rciProto][1] = ahfwUDP;
			Item.m_Vector[rciDirection][0] = Item.m_Vector[rciDirection][1] = 0;
			CreateFromRuleElement(Item, &pCat->m_UDPIncoming.m_RemoteAddresses, NULL, &pCat->m_UDPIncoming.m_RemotePorts, &pCat->m_UDPIncoming.m_LocalPorts);
		}
		break;
	
	case ahfwBlockAllRule:
		{
			// All TCP
			Item.m_Vector[rciProto][0] = Item.m_Vector[rciProto][1] = ahfwTCP;
			m_Items.push_back ( Item );

			// All UDP
			Item.m_Vector[rciProto][0] = Item.m_Vector[rciProto][1] = ahfwUDP;
			m_Items.push_back ( Item );
		}
		break;
	
	case ahfwCustomRule:
		if(cAHFWProtocol * pProt = g_AppRuleDef.GetAppProtocol(pRule->m_nProtocol))
		{
			if(pRule->m_eDirection == _fwprdIncoming)
				Item.m_Vector[rciDirection][1] = 0;
			if(pRule->m_eDirection == _fwprdOutgoing)
				Item.m_Vector[rciDirection][0] = 1;
			
			Item.m_Vector[rciProto][0] = Item.m_Vector[rciProto][1] = pRule->m_eBaseProtocol;

			CreateFromRuleElement(
				Item,
				&pRule->m_aRemoteAddresses,
				&pRule->m_aLocalAddresses,
				&(pProt->m_aPorts.empty() ? pRule->m_aRemotePorts : pProt->m_aPorts),
				&pRule->m_aLocalPorts
				);
		}
		break;
	}
}

void CRuleContainer::CreateFromRuleElement(CRuleContainerItem &Item, cVector<cFWAddress> *pRemAddrs, cVector<cFWAddress> *pLocAddrs, cVector<cFWPort> *pRemPorts, cVector<cFWPort> *pLocPorts)
{
	cVector<cFWAddress> &RemoteAddr = pRemAddrs ? *pRemAddrs : cVector<cFWAddress>();
	cVector<cFWAddress> &LocalAddr  = pLocAddrs ? *pLocAddrs : cVector<cFWAddress>();
	cVector<cFWPort>    &RemotePort = pRemPorts ? *pRemPorts : cVector<cFWPort>();
	cVector<cFWPort>    &LocalPort  = pLocPorts ? *pLocPorts : cVector<cFWPort>();
	
	int rai = 0, lai = 0, rpi = 0, lpi = 0;

	bool bNeedIterateRemoteHostNameList = false;
	cVector<tDWORD> rhnl;
	int rhnli = 0;

	bool bNeedIterateLocalHostNameList = false;
	cVector<tDWORD> lhnl;
	int lhnli = 0;

	while ( 1 )
	{
		// считываем
		if ( rai != RemoteAddr.size() )
		{
			const cFWAddress &Addr = RemoteAddr[rai];
			if(!Addr.m_Address.empty())                              // symbolic address
			{
				if ( bNeedIterateRemoteHostNameList == false )
				{
					rhnl = Addr.m_AddressIP;
					rhnli = 0;
				}
				
				if ( rhnli != rhnl.size() )
					Item.m_Vector[rciRemoteAddr][0] = Item.m_Vector[rciRemoteAddr][1] = ntohl(rhnl[rhnli]);

				bNeedIterateRemoteHostNameList = true;
			}
			else
			if(Addr.m_IPMask)                                        // subnet
			{
//				((CCAddrSubnet *)pAddr)->GetRange(Item.m_Vector[rciRemoteAddr][0], Item.m_Vector[rciRemoteAddr][1]);
			}
			else
			if(Addr.m_IPLo != Addr.m_IPHi)                           // range
			{
				Item.m_Vector[rciRemoteAddr][0] = ntohl(Addr.m_IPLo);
				Item.m_Vector[rciRemoteAddr][1] = ntohl(Addr.m_IPHi);
			}
			else
			if(Addr.m_IPLo == Addr.m_IPHi)                           // ip
			{
				Item.m_Vector[rciRemoteAddr][0] = Item.m_Vector[rciRemoteAddr][1] = ntohl(Addr.m_IPLo);
			}
		}

		if ( lai != LocalAddr.size() )
		{
			const cFWAddress &Addr = LocalAddr[rai];
			if(!Addr.m_Address.empty())                              // symbolic address
			{
				if ( bNeedIterateLocalHostNameList == false )
				{
					lhnl = Addr.m_AddressIP;
					lhnli = 0;
				}
				
				if ( lhnli != lhnl.size() )
					Item.m_Vector[rciRemoteAddr][0] = Item.m_Vector[rciRemoteAddr][1] = ntohl(lhnl[lhnli]);

				bNeedIterateLocalHostNameList = true;
			}
			else
			if(Addr.m_IPMask)                                        // subnet
			{
//				((CCAddrSubnet *)pAddr)->GetRange(Item.m_Vector[rciRemoteAddr][0], Item.m_Vector[rciRemoteAddr][1]);
			}
			else
			if(Addr.m_IPLo != Addr.m_IPHi)                           // range
			{
				Item.m_Vector[rciLocalAddr][0] = ntohl(Addr.m_IPLo);
				Item.m_Vector[rciLocalAddr][1] = ntohl(Addr.m_IPHi);
			}
			else
			if(Addr.m_IPLo == Addr.m_IPHi)                           // ip
			{
				Item.m_Vector[rciLocalAddr][0] = Item.m_Vector[rciLocalAddr][1] = ntohl(Addr.m_IPLo);
			}
		}

		if ( rpi != RemotePort.size() )
		{
			cFWPort * pPort = &RemotePort[rpi];
			Item.m_Vector[rciRemotePort][0] = pPort->m_PortLo;
			Item.m_Vector[rciRemotePort][1] = pPort->m_PortHi;
		}

		if ( lpi != LocalPort.size() )
		{
			cFWPort * pPort = &LocalPort[lpi];;
			Item.m_Vector[rciLocalPort][0] = pPort->m_PortLo;
			Item.m_Vector[rciLocalPort][1] = pPort->m_PortHi;
		}

		// добавляем
		m_Items.push_back( Item );

		// увеличиваем счетчики
		if ( !LocalPort.empty() )
			++lpi;
		if ( lpi == LocalPort.size() )
		{
			if ( !RemotePort.empty() )
				++rpi;
			if ( rpi == RemotePort.size() )
			{
				if ( bNeedIterateLocalHostNameList && !lhnl.empty() )
					++lhnli;
				if ( !bNeedIterateLocalHostNameList || lhnli == lhnl.size() )
				{
					if ( bNeedIterateRemoteHostNameList && !rhnl.empty() )
						++rhnli;
					if ( !bNeedIterateLocalHostNameList || rhnli == rhnl.size() )
					{
						if ( !RemoteAddr.empty() )
							++rai;
						if ( rai == RemoteAddr.size() )
						{
							if ( !LocalAddr.empty() )
								++lai;
							if ( lai == LocalAddr.size() )
							{
								break;
							}
							rai = 0;
						}
						rhnli = 0;
					}
					lhnli = 0;
				}
				rpi = 0;
			}
			lpi = 0;
		}
	}
}*/

/*void CRuleContainer::CreateFromRuleElement(CRuleContainerItem &Item, cVector<cFWAddress> *pRemAddrs, cVector<cFWAddress> *pLocAddrs, cVector<cFWPort> *pRemPorts, cVector<cFWPort> *pLocPorts)
{
	bool bRemAddrsEmpty = pRemAddrs && !pRemAddrs->empty();

	int ira = 0;
	do
	{
		if(pRemAddrs)
		{
			cFWAddress &Addr = pRemAddrs->at(ira);
			if(!Addr.m_Address.empty())
				

		}
		
		int ila = 0;
		do
		{
			if(pLocPorts)
			{
			}
			
			for(;;)

			int irp = 0;
			do
			{
				if(pRemPorts)
				{
					cFWPort &Port = pRemPorts->at(ilp);
					Item.m_Vector[rciRemotePort][0] = Port.m_PortLo;
					Item.m_Vector[rciRemotePort][1] = Port.m_PortHi;
				}
				
				int ilp = 0;
				do
				{
					if(pLocPorts)
					{
						cFWPort &Port = pLocPorts->at(ilp);
						Item.m_Vector[rciLocalPort][0] = Port.m_PortLo;
						Item.m_Vector[rciLocalPort][1] = Port.m_PortHi;
					}

					if(pRemAddrsIp)
					{
					}
					else
					if(pLocAddrsIp)
					{
					}
					else
						m_Items.push_back(Item);
				}
				while(pLocPorts && (ilp < pLocPorts->size()));
			}
			while(pRemPorts && (irp < pRemPorts->size()));
		}
		while(pLocAddrs && (ila < pLocAddrs->size()));
	}
	while(pRemAddrs && (ira < pRemAddrs->size()));
}




#pragma warning ( disable : 4035 )

inline bool IntersectSegments(IN const DWORD segm1[2], IN const DWORD segm2[2], OUT DWORD segm[2])
{
//	segm[0] = max ( segm1[0], segm2[0] );
//	segm[1] = min ( segm1[1], segm2[1] );
//	return segm[0] <= segm[1];

	_asm
	{
		mov		eax, [segm1]
		mov		ecx, [segm2]
		mov		edi, [segm]

		mov		edx, [eax]			; edx = segm1[0]
		cmp		edx, [ecx]
		ja		lbl1				; segm1[0] > segm2[0]
		mov		edx, [ecx]
lbl1:
		mov		[edi], edx			; edx = max ( segm1[0], segm2[0] )

		add		eax, 4
		add		ecx, 4
		add		edi, 4
		mov		edx, [eax]			; edx = segm2[1]
		cmp		edx, [ecx]
		jb		lbl2				; segm1[1] < segm2[1]
		mov		edx, [ecx]
lbl2:
		mov		[edi], edx

		cmp		edx, [edi - 4]
		sbb		eax, eax
		inc		eax
	}
}

bool CRuleContainer::Subtract(const CRuleContainer &Container)
{
	if(!m_strAppName.empty() && !Container.m_strAppName.empty() &&
		stricmp(m_strAppName.c_str(), Container.m_strAppName.c_str()) != 0)
	{
		return false;
	}

	bool bRetVal = false;
	int n;
	
	DWORD Intersection[rciDimCount][2];

	CRuleContainerItemList::iterator k, k1, k2, ki;
	CRuleContainerItemList::const_iterator j;

	CRuleContainerItem NewItem;

	// будем модифицировать список in-place (для скорости)
	// k1 и k2 - указатели перед началом и за концом обрабатываемого фрагмента
	// в начале каждой итерации внешнего цикла этот фрагмент содержит
	// один элемент
	for ( k1 = k2 = m_Items.begin(), ++k2; k1 != m_Items.end(); k1 = k2, ++k2 )
	{
		// k1 уменьшаем здесь после проверки условия цикла
		--k1;

		// будем вычитать все элементы другого списка из фрагмента
		for ( j = Container.m_Items.begin(); j != Container.m_Items.end(); ++j )
		{
			const DWORD (*Vector2)[2] = (*j).m_Vector;
			
			for ( k = k1, ++k; k != k2; ++k )
			{
				// вычислим разность ((*k) - (*j))

				CRuleContainerItem &Item1 = *k;
				DWORD (*Vector1)[2] = Item1.m_Vector;

				// найдем пересечение прямоугольников
				for ( n = 0; n < rciDimCount; ++n )
				{
					if ( !IntersectSegments( Vector1[n], Vector2[n], Intersection[n] ) )
					{
						// если нет пересечения хотя бы по одной координате, значит множества не пересекаются
						break;
					}
				}

				if ( n == rciDimCount )
				{
					// прямоугольники пересекаются

					bRetVal = true;

					// отрезаем 2 прямоугольника по каждому измерению (если они не пусты)
					for ( n = 0; n < rciDimCount; ++n )
					{
						DWORD *Vn = Vector1[n], *In = Intersection[n];

						if ( Vn[0] < In[0] )
						{
							NewItem = Item1;
							NewItem.m_Vector[n][1] = In[0] - 1;
							
							m_Items.insert ( k, NewItem );

							Vn[0] = In[0];
						}

						if ( In[1] < Vn[1] )
						{
							NewItem = Item1;
							NewItem.m_Vector[n][0] = In[1] + 1;
							
							m_Items.insert( k, NewItem );

							Vn[1] = In[1];
						}
					}

					// удаляем элемент, который распался на куски
					k = m_Items.erase ( k );
					--k;

				}
			}
		}
	}

	return bRetVal;
}

bool CRuleContainer::Intersect(const CRuleContainer &Container)
{
	CRuleContainerItemList::const_iterator i, j;

	CRuleContainerItem NewItem;
	DWORD (*Intersection)[2] = NewItem.m_Vector;

	CRuleContainerItemList NewItemList;

	int n;

	for ( i = m_Items.begin(); i != m_Items.end(); ++i )
	{
		const DWORD (*Vector1)[2] = (*i).m_Vector;

		for ( j = Container.m_Items.begin(); j != Container.m_Items.end(); ++j )
		{
			const DWORD (*Vector2)[2] = (*j).m_Vector;

			// найдем пересечение прямоугольников
			for ( n = 0; n < rciDimCount; ++n )
			{
				if ( !IntersectSegments( Vector1[n], Vector2[n], Intersection[n] ) )
				{
					break;
				}
			}

			if ( n == rciDimCount )
			{
				NewItemList.push_back( NewItem );
			}
		}
	}

	m_Items = NewItemList;

	return !m_Items.empty();
}
*/
