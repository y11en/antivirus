
#include <stdlib.h>
#include <stdio.h>

#include <prague.h>

#include <structs/s_ahfw.h>

#include <plugin/p_win32_nfio.h>

//#include <../basegui/basegui.h>

#define PRESET_FILE                     "%Bases%/FwPresets.ini"
#define STR_OEL                         "\r\n"

typedef const char * LPCSTR;

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
	eLocalPort,
	eLocalHost,
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
	"LocalPort",
	"LocalAddress",
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

inline static bool is_digit(char symbol)
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

static tDWORD Str2Ip(char *sIp)
{
	tDWORD nIp[4] = {0,};
	sscanf(sIp, "%u.%u.%u.%u", &nIp[3], &nIp[2], &nIp[1], &nIp[0]);
	tDWORD Ip;
	tBYTE *pIp = (tBYTE *)&Ip;
	pIp[0] = nIp[0];
	pIp[1] = nIp[1];
	pIp[2] = nIp[2];
	pIp[3] = nIp[3];

	return Ip;
}

//-----------------------------------------------------------------------------

static void AddPort(tWORD &nPortLo, tWORD &nPortHi, cVector<cFWPort> &aPorts)
{
	if(nPortLo)
	{
		cFWPort &Port = aPorts.push_back();
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

static void ParsePortListStr(char *sPorts, cVector<cFWPort> &aPorts)
{
	aPorts.clear();

	tWORD nPortLo = 0;
	tWORD nPortHi = 0;
	tWORD * pPortToScan = &nPortLo;
	
	for(; *sPorts; sPorts++)
	{
		if(is_digit(*sPorts))
		{
			*pPortToScan = (tWORD)atoi(sPorts);
			for(; is_digit(*sPorts); sPorts++); sPorts--;
			continue;
		}
		if(*sPorts == ',')
		{
			AddPort(nPortLo, nPortHi, aPorts);
			pPortToScan = &nPortLo;
			continue;
		}
		if(*sPorts == '-')
		{
			pPortToScan = &nPortHi;
			continue;
		}
	}
	AddPort(nPortLo, nPortHi, aPorts);
}

// string format: 127.0.0.1, 1.2.3.4:255.255.255.0

static void AddHost(tDWORD &nIpLo, tDWORD &nIpHi, tDWORD &nMask, cVector<cFWAddress> &aAddrs)
{
	if(nIpLo)
	{
		cFWAddress &Addr = aAddrs.push_back();
		Addr.m_IPLo = Addr.m_IPHi = nIpLo;
		if(nIpHi)
			Addr.m_IPHi = nIpHi;
		if(nMask)
			Addr.m_IPMask = nMask;
	}
	nIpLo = nIpHi = nMask = 0;
}

static void ParseHostListStr(char *sAddrs, cVector<cFWAddress> &aAddrs)
{
	aAddrs.clear();

	tDWORD nIpLo = 0;
	tDWORD nIpHi = 0;
	tDWORD nMask = 0;
	tDWORD * pAddrToScan = &nIpLo;

	for(; *sAddrs; sAddrs++)
	{
		if(is_digit(*sAddrs))
		{
			*pAddrToScan = Str2Ip(sAddrs);
			for(; is_digit(*sAddrs) || *sAddrs == '.'; sAddrs++); sAddrs--;
			continue;
		}		
		if(*sAddrs == ',')
		{
			AddHost(nIpLo, nIpHi, nMask, aAddrs);
			pAddrToScan = &nIpLo;
			continue;
		}
		if(*sAddrs == '-')
		{
			pAddrToScan = &nIpHi;
			continue;
		}		
		if(*sAddrs == ':' || *sAddrs == '/')
		{
			pAddrToScan = &nMask;
			continue;
		}
	}
	AddHost(nIpLo, nIpHi, nMask, aAddrs);
}

static void ParseTimeStr(char *sTime, tDWORD &nTimeFrom, tDWORD &nTimeTill)
{
	nTimeFrom = nTimeTill = 0;

	if( sTime && is_digit(*sTime) )
		nTimeFrom = atoi(sTime);
	if( sTime )
		nextfield(sTime, '-');
	if( sTime && is_digit(*sTime) )
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
	eTags eCurTag = eExecutable; 
	for(;eCurTag != eUnknown; eCurTag = (eTags)((unsigned)eCurTag + 1))
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
		ParseHostListStr(sData, pCurRule->m_aRemoteAddresses);
		break;

	case eLocalHost:
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

//////////////////////////////////////////////////////////////////////////
// LoadPresets

bool LoadPresets(LPCSTR strFile, cFwAppRulePresets &aPresets)
{
	cStringObj sFile = strFile ? strFile : PRESET_FILE;
	g_root->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString (sFile), 0, 0);

	cAutoObj<cIO> hIo;
	if(PR_FAIL(g_root->sysCreateObject((hOBJECT *)&hIo, IID_IO, PID_WIN32_NFIO)))
		return false;

	if(PR_FAIL(hIo->propSetStr(0, pgOBJECT_FULL_NAME, sFile.c_str(cCP_ANSI))))
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
