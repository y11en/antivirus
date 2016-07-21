// hips_base_serializer.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <stdlib.h>

#define PR_IMPEX_DEF
#include <prague.h>
#include <pr_serializable.h>
#include <pr_vector.h>
#include "CPrague.h"
#include <plugin/p_propertiesmap.h>
//#include <structs/s_hips.h>

#include "hips_base_serializer.h"



//hROOT g_root = NULL;

hPLUGIN g_ParamsPpl = 0;

_CPrague prague;

CHipsSettings * g_pSettings = 0;

//cSerRegistrar * g_serializable_registrar_head = NULL;

bool SimpleStreamSerialize(wchar_t * l_pBaseFileName);
bool SerializeToXML(wchar_t * l_pBaseFileName);
bool RegStreamSerialize(wchar_t * l_pBaseFileName);

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
HIPS_BASE_SERIALIZER_DLL_EXPORTS
bool InitializeConst(
             int * l_HIPS_FLAG_ALLOW,
             int * l_HIPS_FLAG_ASK,
             int * l_HIPS_FLAG_DENY,
             int * l_HIPS_FLAG_INHERIT,
             int * l_HIPS_POS_WRITE,
             int * l_HIPS_POS_PERMIS,
             int * l_HIPS_POS_READ,
             int * l_HIPS_POS_ENUM,
             int * l_HIPS_POS_DELETE,
             int * l_HIPS_LOG_OFF,
             int * l_HIPS_LOG_ON
					 )
{
	*l_HIPS_FLAG_ALLOW = HIPS_FLAG_ALLOW;
	*l_HIPS_FLAG_ASK = HIPS_FLAG_ASK;
	*l_HIPS_FLAG_DENY = HIPS_FLAG_DENY;
	*l_HIPS_FLAG_INHERIT = HIPS_FLAG_INHERIT;
	*l_HIPS_POS_WRITE = HIPS_POS_WRITE;
	*l_HIPS_POS_PERMIS = HIPS_POS_PERMIS;
	*l_HIPS_POS_READ = HIPS_POS_READ;
	*l_HIPS_POS_ENUM = HIPS_POS_ENUM;
	*l_HIPS_POS_DELETE = HIPS_POS_DELETE;
	*l_HIPS_LOG_OFF = HIPS_LOG_OFF;
	*l_HIPS_LOG_ON = HIPS_LOG_ON;
	return true;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
HIPS_BASE_SERIALIZER_DLL_EXPORTS
bool Initialize()
{
	//MessageBox(0, L"Initialize start", 0, 0);
	if (!prague.LoadPrague()  || !g_root)
	{
		fprintf(stderr, "Error: cannot initialize Prague\n");
		MessageBox(0, L"Error: cannot initialize Prague", 0, 0);
		return false;
	}
	if (PR_FAIL(g_root->LoadModule(&g_ParamsPpl,"params.ppl",(tDWORD)strlen("params.ppl"),cCP_ANSI)))
	{
		MessageBox(0, L"Error: cannot load params.ppl", 0, 0);
		false;
	}
	if (g_pSettings == 0)
		g_pSettings = new CHipsSettings();

	return true;
}

HIPS_BASE_SERIALIZER_DLL_EXPORTS
bool UnInitialize()
{
	if (g_pSettings)
		delete [] g_pSettings;
	g_pSettings = 0;
	if (g_ParamsPpl)
		g_ParamsPpl->Unload();
	g_ParamsPpl = 0;
	prague.UnloadPrague();
	return true;
}
//bool TryAddToParent(CHipsResourceGroupsItem * l_Parent, CHipsResourceGroupsItem * pItem, DWORD l_ParentId)
//{
//	if (l_Parent->m_ResGroupID == l_ParentId)
//	{
//		l_Parent->m_vChildResGroupList.push_back(*pItem);
//		return true;
//	}
//	else
//	{
//		for (DWORD i = 0; i < l_Parent->m_vChildResGroupList.count(); i++)
//		{
//			if (TryAddToParent(&l_Parent->m_vChildResGroupList[i], pItem, l_ParentId))
//				return true;
//		}
//	}
//	return false;
//}
bool TryAddToParent(cAppGroup * l_Parent, cAppGroup * pItem, DWORD l_ParentId)
{
	if (l_Parent->m_nAppGrpID == l_ParentId)
	{
		l_Parent->m_aChilds.push_back(*pItem);
		return true;
	}
	else
	{
		for (DWORD i = 0; i < l_Parent->m_aChilds.count(); i++)
		{
			if (TryAddToParent(&l_Parent->m_aChilds[i], pItem, l_ParentId))
				return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
HIPS_BASE_SERIALIZER_DLL_EXPORTS
bool AddResourceGroup(DWORD l_GrID, DWORD l_ParentGrID, wchar_t * l_ResGrName, DWORD l_ExFlags)
{
	if (g_root == 0)
	{
		return false;
	}
	if (l_GrID == 0)
		return true;

	char  chTemp[MAX_PATH];
	//WideCharToMultiByte(CP_UTF8, 0, l_ResGrName, -1, chTemp, MAX_PATH, 0, 0);

	cResource UnicRes;
	UnicRes.m_nID = l_GrID;//HIPS_MAKE_ID_EX(l_GrID, 1);
	UnicRes.m_nParentID = l_ParentGrID;//HIPS_MAKE_ID_EX(l_ParentGrID, 1);
	UnicRes.m_sName = l_ResGrName;//chTemp;
	UnicRes.m_nFlags |= l_ExFlags;
	g_pSettings->m_Resource.AddResource(UnicRes);

	//bool WasAdded = false;
	//if ((l_GrID == l_ParentGrID) && (l_ParentGrID == 2))
	//{
	//	//	this is root
	//	g_pSettings->m_ResGroups.m_ResGroupID = l_GrID;
	//	g_pSettings->m_ResGroups.m_ResGroupName = chTemp;
	//	return true;
	//}
	//else
	//{
	//	CHipsResourceGroupsItem GroupItem;
	//	GroupItem.m_ResGroupID = l_GrID;
	//	GroupItem.m_ResGroupName = chTemp;//l_ResGrName;
	//	WasAdded = TryAddToParent(&g_pSettings->m_ResGroups, &GroupItem, l_ParentGrID);
	//}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
HIPS_BASE_SERIALIZER_DLL_EXPORTS
bool AddAppGroup(DWORD l_GrID, DWORD l_ParentGrID, wchar_t * l_AppGrName, DWORD l_ServStrType, wchar_t * l_ServStr)
{
	if (g_root == 0)
	{
		return false;
	}
	if (l_GrID == 1)
		return true;

	char  chTemp[MAX_PATH];
	WideCharToMultiByte(CP_UTF8, 0, l_AppGrName, -1, chTemp, MAX_PATH, 0, 0);
	char  chTempServStr[MAX_PATH];
	WideCharToMultiByte(CP_UTF8, 0, l_ServStr, -1, chTempServStr, MAX_PATH, 0, 0);
	

	bool WasAdded = false;
	if ((l_GrID == l_ParentGrID) &&	(l_ParentGrID == 2))
	{
		//	this is root
		g_pSettings->m_AppGroups.m_nAppGrpID = l_GrID;
		g_pSettings->m_AppGroups.m_sGroupName = chTemp;
		return true;
	}
	else
	{
		cAppGroup  GroupItem;
		GroupItem.m_nAppGrpID = l_GrID;
		GroupItem.m_sGroupName = chTemp;//l_ResGrName;
		GroupItem.m_nServStrType = l_ServStrType;
		GroupItem.m_sServStr = chTempServStr;
		WasAdded = TryAddToParent(&g_pSettings->m_AppGroups, &GroupItem, l_ParentGrID);
	}
	return WasAdded;
}
////////////////////////////////////////////////////////////////////////////////
// string format: 127.0.0.1, 127.0.0.1 - 127.0.0.3, 1.2.3.4:255.255.255.0, %dhcp%, %dns%, %wins%
////////////////////////////////////////////////////////////////////////////////
//void AddHost(wchar_t *&sDnsName, cIP& IpLo, cIP& IpHi, cIP& Mask, cFwAddresses &aAddrs)
//{
//    if(IpLo.IsValid())
//	{
//		cFwAddress &Addr = aAddrs.push_back();
//		if(sDnsName)
//		{
//			Addr.m_Address = sDnsName;
//			Addr.m_AddressIP.push_back(IpLo);
//		}
//		else
//		{
//			//Addr.m_IPLo = Addr.m_IPHi = IpLo;
//			//if(IpHi.IsValid())
//			//	Addr.m_IPHi = IpHi;
//			//if(Mask.IsValid())
//			//	Addr.m_IPMask = Mask;
//		}
//	}
//	IpLo = IpHi = Mask = cIP();
//	sDnsName = NULL;
//}
//
//////////////////////////////////////////////////////////////////////////////////
////
//////////////////////////////////////////////////////////////////////////////////
static bool IsIPAddressChar(wchar_t ch)
{
	return (isdigit(ch) || ch == ':' || ch == '.' ||
		ch >= 'A' && ch <= 'F' ||
		ch >= 'a' && ch <= 'f');
}
//////////////////////////////////////////////////////////////////////////////////
////
//////////////////////////////////////////////////////////////////////////////////
void ParseHostListStr(wchar_t *sAddrs, cFwAddresses &aAddrs)
{
	if ((sAddrs == 0) || (sAddrs[0]==0))
		return;
	aAddrs.clear();

	wchar_t *sDnsName = NULL;
	cIP IpLo;
	cIP IpHi;
	cIP Mask;
	cIP * pAddrToScan = &IpLo;

	cFwAddress tempAddr;
	cStringObj tempStr;

	for(; *sAddrs; sAddrs++)
	{
		if(*sAddrs == ' ')
			continue;
		if(IsIPAddressChar(*sAddrs))
		{
			const wchar_t* pAddr = sAddrs;
			for(; IsIPAddressChar(*sAddrs); sAddrs++);
			tempStr.assign(cStringObj(pAddr), 0, sAddrs-pAddr);
   //         if(pAddrToScan == &Mask && IpLo.IsV6())
			//{
   //             //pAddrToScan->SetV6Mask( atoi(tString(pAddr, sAddrs-pAddr).c_str()), IpLo.m_V6.m_Zone );
			//}
   //         else
			//{
   //             //*pAddrToScan = Str2Ip( tString(pAddr, sAddrs-pAddr).c_str() );
			//}
			sAddrs--;
			continue;
		}		
		if(*sAddrs == ',')
		{
			tempAddr.m_IP.FromStr(tempStr.c_str(cCP_ANSI));
			aAddrs.push_back(tempAddr);
			//AddHost(sDnsName, IpLo, IpHi, Mask, aAddrs);
			//pAddrToScan = &IpLo;
			continue;
		}
		//if(*sAddrs == '-')
		//{
		//	pAddrToScan = &IpHi;
		//	continue;
		//}		
		//if(*sAddrs == '/')
		//{
		//	pAddrToScan = &Mask;
		//	continue;
		//}
		//if(*sAddrs == '%')
		//{
		//	wchar_t *tmp = sAddrs;
		//	for(; *tmp && *tmp != ' ' && *tmp != ','; tmp++);
		//	bool bEOL = !*tmp;
		//	*tmp = 0;

		//	cFwAddress addr;
		//	addr.m_Address = sAddrs;
		//	if( addr.RecognizeAddress() != _fwaUnknown )
		//		aAddrs.push_back(addr);

		//	sAddrs = bEOL ? tmp - 1 : tmp;
		//	continue;
		//}
		//if(*sAddrs == ')')
		//	continue;
		sDnsName = sAddrs;
		//for(; *sAddrs && *sAddrs != '('; sAddrs++);
		*sAddrs = 0;
	}
	tempAddr.m_IP.FromStr(tempStr.c_str(cCP_ANSI));
	aAddrs.push_back(tempAddr);
	//AddHost(sDnsName, IpLo, IpHi, Mask, aAddrs);
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
HIPS_BASE_SERIALIZER_DLL_EXPORTS
bool AddSimpleResource(bool l_IsEnabled, DWORD l_ExFlags, DWORD l_ResID, DWORD l_ResGrID, DWORD l_ResType, wchar_t * l_ResDescr, wchar_t * l_Param1, wchar_t * l_Param2)
{
	if (g_root == 0)
	{
		return false;
	}
	if (l_ResID == 0)
		return true;

	cResource * pParentRes = g_pSettings->m_Resource.Find(l_ResGrID);
	if (pParentRes == 0)
	{
		return false;
	}
	
	//DWORD newFlags = HIPS_SET_RES_FLAG(1, l_ResType);

	//if ((pParentRes->m_nFlags != 0) &&
	//	(pParentRes->m_nFlags != newFlags))
	//{
	//	return false;
	//}
	//pParentRes->m_nFlags = newFlags;


	cResource Res;
	Res.m_nID = l_ResID;
	Res.m_nParentID = l_ResGrID;//HIPS_MAKE_ID_EX(l_ResGrID, 1);
	Res.m_sName = l_ResDescr;
	Res.m_nFlags |= HIPS_SET_RES_FLAG(l_IsEnabled, l_ResType);
	Res.m_nFlags |= l_ExFlags;


	//CHipsResourceParam Param;
	switch (l_ResType/*ResItem.m_ResType*/)
	{
		case cResource::ehrtFile: 
			{
				cFileInfo pFile;
				pFile.m_Path = l_Param1;
				
				Res.m_pData.assign(pFile);
				g_pSettings->m_Resource.AddResource(Res);
				break;
			}
		case cResource::ehrtReg: 
			{
				cRegKeyInfo pReg;
				pReg.m_KeyPath = l_Param1;
				pReg.m_KeyValue = l_Param2;

				Res.m_pData.assign(pReg);
				g_pSettings->m_Resource.AddResource(Res);
				break;
			}
		case cResource::ehrtDevice:
			{
				cDeviceInfo pDev;
				pDev.m_sClassGUID = l_Param1;
				pDev.m_sTypeName = l_Param2;

				//Res.m_nFlags |= cResource::fResourceView | cResource::fFilesRegistyrView;

				Res.m_pData.assign(pDev);
				g_pSettings->m_Resource.AddResource(Res);
				break;
			}
		case cResource::ehrtPrivileges:
			{
				cPrivilegeInfo pPriv;
				if (l_Param2)
					pPriv.m_nType = _wtoi(l_Param2);
				else
					pPriv.m_nType = cPrivilegeInfo::ehptUnknown;

				Res.m_pData.assign(pPriv);
				g_pSettings->m_Resource.AddResource(Res);
				break;
			}
		case cResource::ehrtIPRange:
			{
				cIpRange pRange;
				cFwAddresses lAddr;
				cFwAddresses rAddr;

				ParseHostListStr(l_Param1, lAddr);
				ParseHostListStr(l_Param2, rAddr);

				pRange.m_aLocalAddresses = lAddr;
				pRange.m_aRemoteAddresses = rAddr;
				Res.m_pData.assign(pRange);
				g_pSettings->m_Resource.AddResource(Res);
				break;
			}
		case cResource::ehrtSeverity:
			{
				cSeverity pSev;
				if (l_Param2)
					pSev.m_nType = _wtoi(l_Param2);
				else
					pSev.m_nType = cSeverity::ehsUnknown;

				Res.m_pData.assign(pSev);
				g_pSettings->m_Resource.AddResource(Res);
				break;
			}
		case cResource::ehrtExclusion:
			{
				cExclusion pExcl;
				if (l_Param2)
					pExcl.m_nType = _wtoi(l_Param2);
				else
					pExcl.m_nType = cExclusion::ehexOAS;

				Res.m_pData.assign(pExcl);
				g_pSettings->m_Resource.AddResource(Res);
				break;
			}
		case cResource::ehrtNetZone:
			{
				cHipsNetZone pZone;
				if (l_Param2)
					pZone.m_nID = _wtoi(l_Param2);
				else
					pZone.m_nID = 0;

				Res.m_pData.assign(pZone);
				g_pSettings->m_Resource.AddResource(Res);
				break;
			}

		default:
			return false;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
static bool ParsePortListStr(wchar_t *sPorts, cFwPorts &aPorts)
{
	aPorts.clear();

	tWORD nPortLo = 0;
	tWORD nPortHi = 0;
	tWORD * pPortToScan = &nPortLo;
	
	for(; *sPorts; sPorts++)
	{
		if(isdigit(*sPorts))
		{
            int port = _wtoi(sPorts);
            if (port > 0 && port <= 65535)
            {
			    *pPortToScan = (tWORD)port;
			    for(; isdigit(*sPorts); sPorts++); sPorts--;
			    continue;
            }
            else
                return false;
		}
		else if(*sPorts == L',')
		{
			AddPort(nPortLo, nPortHi, aPorts);
			pPortToScan = &nPortLo;
			continue;
		}
		else if(*sPorts == L'-')
		{
			pPortToScan = &nPortHi;
			continue;
		}
        else if(*sPorts == L' ' || *sPorts == L'\t')
        {
            continue;
        }
        else
            return false;
	}
	AddPort(nPortLo, nPortHi, aPorts);
    return true;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
HIPS_BASE_SERIALIZER_DLL_EXPORTS
bool AddWebService(bool l_IsEnabled, DWORD l_WebServID, DWORD l_GrID, wchar_t * l_Desc,
				   bool l_UseProtocol, DWORD l_Prot,
				   bool l_UseType, DWORD l_IType,
				   bool l_UseCode, DWORD l_ICode,
				   DWORD l_Dir, wchar_t * l_RemPorts, wchar_t * l_LocPorts)
{
	cResource Res;
	Res.m_nID = l_WebServID;
	Res.m_nParentID = l_GrID;//HIPS_MAKE_ID_EX(l_GrID, 1);
	Res.m_sName = l_Desc;
	Res.m_nFlags |= HIPS_SET_RES_FLAG(l_IsEnabled, cResource::ehrtWebService);
	cWebService WebServ;

	WebServ.m_eDirection = l_Dir;
	WebServ.m_nFlags |= (
		(l_UseProtocol ? (cWebService::fProtocol & 0xff) : 0) |
		(l_UseType ? (cWebService::fIcmpType & 0xff) : 0) |
		(l_UseCode ? (cWebService::fIcmpCode & 0xff) : 0));

	WebServ.m_nIcmpType = l_IType;
	WebServ.m_nIcmpCode = l_ICode;
	WebServ.m_nProto = l_Prot;


	//cFwPort Port;

	cFwPorts lPorts;
	cFwPorts rPorts;

	ParsePortListStr(l_RemPorts, rPorts);
	ParsePortListStr(l_LocPorts, lPorts);

	
	WebServ.m_aRemotePorts = rPorts;
	WebServ.m_aLocalPorts = lPorts;


	Res.m_pData.assign(WebServ);
	g_pSettings->m_Resource.AddResource(Res);
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
HIPS_BASE_SERIALIZER_DLL_EXPORTS
bool AddApp(DWORD l_AppID, DWORD l_AppGrID, wchar_t * l_AppName, wchar_t * l_AppHash, DWORD l_AppFlags)
{
	if (l_AppID == 1)
		return true;
	CHipsAppItem AppItem;
	AppItem.m_AppFlags.Zero(); //l_AppFlags;
	AppItem.m_AppId = l_AppID;
	AppItem.m_AppGrId = l_AppGrID;
	AppItem.m_AppName = l_AppName;
	
	BYTE Hash[HIPS_HASH_SIZE];
	memset(&Hash[0], 0, sizeof(Hash));
	DWORD i = 0;
	DWORD TempReadByte = 0;
	if (l_AppHash != 0 && wcslen(l_AppHash) == 32)
	{
		wchar_t * ptr = l_AppHash;
		while (swscanf(ptr, L"%02X", &TempReadByte) && i < HIPS_HASH_SIZE)
		{
			Hash[i] = TempReadByte;
			if (++i == HIPS_HASH_SIZE)
				break;
			if (wcslen(ptr) == 0)
				break;
			ptr += 2;
		}
		memcpy(AppItem.m_AppHash, Hash, sizeof(Hash));
	}
	g_pSettings->m_vAppList.push_back(AppItem);
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
HIPS_BASE_SERIALIZER_DLL_EXPORTS
bool AddRule(DWORD l_RuleID, DWORD l_RuleState, DWORD l_RuleType, DWORD l_AppId, DWORD l_AppGrId, DWORD l_AppFlag, DWORD l_ResId, DWORD l_Res2Id, DWORD l_AccessFlag)
{
	if (g_root == 0)
	{
			return false;
	}
	CHipsRuleItem RuleItem;
	RuleItem.m_RuleId = l_RuleID;
	RuleItem.m_RuleState = l_RuleState;
	RuleItem.m_RuleTypeId = l_RuleType;
	RuleItem.m_AccessFlag = l_AccessFlag;

	//	if gr == 1 it`s meen that gr not specified
	RuleItem.m_AppIdEx = HIPS_MAKE_ID_EX(((l_AppGrId > 1) ? l_AppGrId : l_AppId), (l_AppGrId > 1));
	if (l_AppGrId == 1 && l_AppId == 1)
	{
		//	this is global rule (not app sensitive)
		RuleItem.m_AppIdEx = 0;
	}

	RuleItem.m_ResIdEx = l_ResId;//(l_ResGrId > 1) ? l_ResGrId : l_ResId;//HIPS_MAKE_ID_EX(((l_ResGrId > 1) ? l_ResGrId : l_ResId), (l_ResGrId > 1));
	RuleItem.m_ResIdEx2 = l_Res2Id;

	if (l_RuleType == 4)	//???
	{
		CHipsPacketRule PacketRuleItem;
		*((CHipsRuleItem*)&PacketRuleItem) = RuleItem;
		PacketRuleItem.m_AppIdEx = 0;
		PacketRuleItem.m_Severity = l_AppFlag;
		g_pSettings->m_PacketRules.push_back(PacketRuleItem);
	}
	else
	{
		g_pSettings->m_vRuleList.push_back(RuleItem);
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
bool SimpleStreamSerialize(wchar_t * l_pBaseFileName)
{
	tChunckBuff buff; tDWORD size;
	tERROR err;
	err = g_root->StreamSerialize(g_pSettings, /*SERID_UNKNOWN*/CHipsSettings::eIID, (tBYTE*)buff.ptr(), buff.count(), &size);

	if( err == errBUFFER_TOO_SMALL )
		err = g_root->StreamSerialize(g_pSettings, /*SERID_UNKNOWN*/CHipsSettings::eIID, (tBYTE*)buff.get(size, false), size, &size);

	if( PR_FAIL(err) )
		return false;
	FILE * pFile = _wfopen(l_pBaseFileName, L"wb");
	if (pFile) 
	{
		fwrite(buff.ptr(), 1, buff.size(), pFile);
		fclose(pFile);
		return true;
	}
	return false;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
bool RegStreamSerialize(wchar_t * l_pBaseFileName)
{
	hREGISTRY aReg = 0;

	aReg = 0;  

	tERROR anError;
	anError = g_root->sysCreateObject ( reinterpret_cast<hOBJECT*> ( &aReg ), IID_REGISTRY, PID_PROPERTIESMAP, SUBTYPE_ANY );

	if ( PR_SUCC ( anError ) )
		anError = aReg->set_pgSAVE_RESULTS_ON_CLOSE ( cTRUE );
	else
	{
		wchar_t tt[100];
		_swprintf(tt, L"%x", anError);
		MessageBox(0, L"sysCreateObject PID_PROPERTIESMAP FAIL ", tt, 0);
	}

	if ( PR_SUCC ( anError ) ) {
		tCHAR aFileName [MAX_PATH];
		WideCharToMultiByte(CP_UTF8, 0, l_pBaseFileName, -1, aFileName, MAX_PATH, 0, 0);
		anError = aReg->propSetStr ( 0 , pgOBJECT_NAME, aFileName, 0, cCP_ANSI );
	}

	if ( PR_SUCC ( anError ) )
		anError = aReg->sysCreateObjectDone ();
	else
	{
		wchar_t tt[100];
		_swprintf(tt, L"%x", anError);
		MessageBox(0, L"sysCreateObjectDone FAIL ", tt, 0);
	}

	anError = aReg->Clean();
	anError = g_root->RegSerialize(g_pSettings, CHipsSettings::eIID, aReg, 0);
	if( anError == errBUFFER_TOO_SMALL )
		anError = g_root->RegSerialize(g_pSettings, CHipsSettings::eIID, aReg, 0);

	if ( PR_FAIL ( anError ) )
	{
		wchar_t tt[100];
		_swprintf(tt, L"%x", anError);
		MessageBox(0, L"RegSerialize FAIL ", tt, 0);
	}

	aReg->sysCloseObject();

	return anError == 0;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
/*
bool SerializeToXML(wchar_t * l_pBaseFileName)
{
	tERROR anError = errOK; 

	hREGISTRY aReg = 0;

	aReg = 0;  

	anError = g_root->sysCreateObject ( reinterpret_cast<hOBJECT*> ( &aReg ), IID_REGISTRY, PID_PROPERTIESMAP, SUBTYPE_ANY );

	if ( PR_SUCC ( anError ) )
		anError = aReg->set_pgSAVE_RESULTS_ON_CLOSE ( cTRUE );

	if ( PR_SUCC ( anError ) ) {
		tCHAR aFileName [] = "propertiesmap_test.xml";
		anError = aReg->propSetStr ( 0 , pgOBJECT_NAME, aFileName, 0, cCP_ANSI );
	}

	if ( PR_SUCC ( anError ) )
		anError = aReg->sysCreateObjectDone ();

	tRegKey hRoot, hResourceList, hResource, hParamsList, hParam;
	char chResourceId[10];
	char chParamId[10];
	DWORD dwValue = 0;
	CHipsResourceItem tempResource;
	CHipsResourceParam tempParam;
	
	anError = aReg->OpenKey(&hRoot, 0, "HipsSettingsRoot", cTRUE);
	if (PR_SUCC(anError))
	{
		anError = aReg->OpenKey(&hResourceList, hRoot, "Hips_Resources", cTRUE);
		if (PR_SUCC(anError))
		{
			for (DWORD i = 0; i < g_pSettings->m_vResList.count(); i++)
			{
				tempResource = g_pSettings->m_vResList[i];
				sprintf(chResourceId, "%d", tempResource.m_ResID);

				anError = aReg->OpenKey(&hResource, hResourceList, chResourceId, cTRUE);
				if (PR_SUCC(anError))
				{
					anError = aReg->SetValue ( hResource, "ResID", tid_DWORD, &tempResource.m_ResID, sizeof(tempResource.m_ResID), cTRUE );
				    anError = aReg->SetValue ( hResource, "ResType", tid_DWORD, &tempResource.m_ResType, sizeof(tempResource.m_ResType), cTRUE );
				    anError = aReg->SetValue ( hResource, "ResDesc", tid_STRING, tempResource.m_ResDescr.c_str(cCP_ANSI), tempResource.m_ResDescr.size(), cTRUE );

					//	saving param list
					anError = aReg->OpenKey(&hParamsList, hResource, "ParamList", cTRUE);
					if (PR_SUCC(anError))
					{
						for (DWORD j = 0; j < tempResource.m_vResParamList.count(); j++)
						{
							//	save one param
							tempParam = tempResource.m_vResParamList[j];
							sprintf(chParamId, "%d", j);

							anError = aReg->OpenKey(&hParam, hParamsList, chParamId, cTRUE);
							if (PR_SUCC(anError))
							{
								anError = aReg->SetValue ( hParam, "ParamType", tid_DWORD, &tempParam.m_ResParamType, sizeof(tempParam.m_ResParamType), cTRUE );
								switch (tempParam.m_ResParamType)
								{
									case ehrptNum:
										{
											anError = aReg->SetValue ( hParam, "ParamValueNum", tid_DWORD, &tempParam.m_ResParamValueNum, sizeof(tempParam.m_ResParamValueNum), cTRUE );
											break;
										}
									case ehrptStr:
										{
										    anError = aReg->SetValue ( hParam, "ParamValueStr", tid_STRING, tempParam.m_ResParamValueStr.c_str(cCP_ANSI), tempParam.m_ResParamValueStr.size(), cTRUE );
											break;
										}
									default:
										{
											break;
										}
								}
							}
						}//for
					}
				}
			}



			
		}
	}


	//if ( PR_SUCC ( anError ) ) {
	//tCHAR aFileName [] = "propertiesmap_test2.xml";
	//anError = aReg->propSetStr ( 0 , pgOBJECT_NAME, aFileName, 0, cCP_ANSI );
	//}

	aReg->sysCloseObject();
	aReg = 0;  


	return true;
}
*/
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
HIPS_BASE_SERIALIZER_DLL_EXPORTS
bool CompileBase(wchar_t * l_pBaseFileName)
{
	//return SerializeToXML(l_pBaseFileName);

	return RegStreamSerialize(l_pBaseFileName);
//	return SimpleStreamSerialize(l_pBaseFileName);
}


////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
HIPS_BASE_SERIALIZER_DLL_EXPORTS
DWORD SetBlockPos(DWORD am, bool is_log, bool is_inh, DWORD pos, DWORD val)
{
	return HIPS_SET_BLOCK_POS(am, is_log, is_inh, pos, val);
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
HIPS_BASE_SERIALIZER_DLL_EXPORTS
DWORD GetInh(DWORD pos, DWORD val)
{
	return HIPS_GET_INH(val, pos);
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
HIPS_BASE_SERIALIZER_DLL_EXPORTS
DWORD GetLog(DWORD pos, DWORD val)
{
	return HIPS_GET_LOG(val, pos);
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
HIPS_BASE_SERIALIZER_DLL_EXPORTS
DWORD GetAM(DWORD pos, DWORD val)
{
	return HIPS_GET_AM(val, pos);
}
