// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2007.
// -------------------------------------------
// Project     -- Hips
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Martynenko
// File Name   -- hipsmanager.cpp
// Date		   -- 2007.07.20 (creation)
// -------------------------------------------
#include "hipsmanager.h"

LONG gMemCounter = 0;
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void* __cdecl pfMemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag )
{
	void* ptr = HeapAlloc( GetProcessHeap(), 0, size );
	if (ptr)
		InterlockedIncrement( & gMemCounter );

	//PR_TRACE(( g_root, prtERROR, TR "allocated 0x%x, size %d", ptr, size ));
	
	return ptr;
};
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void __cdecl pfMemFree (
	PVOID pOpaquePtr,
	void** pptr)
{
	if (!*pptr)
		return;

	//PR_TRACE(( g_root, prtERROR, TR "free 0x%x", *pptr ));

	HeapFree( GetProcessHeap(), 0, *pptr );
	*pptr = NULL;

	InterlockedDecrement( & gMemCounter );
};

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
CHipsManager::CHipsManager(void)
{
	m_IsNeedToStopAll = false;
	m_ThPoolInfo = 0;
	m_ThPoolProcNotify = 0;
	m_ThPoolLog = 0;
	m_ThPoolOtherEv = 0;
	m_pClientContext = NULL;
	m_pTask = 0;
	m_hVerify = NULL;
	return;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
CHipsManager::~CHipsManager(void)
{
	FreeThreadPools();
	VerifyTrustDone(m_hVerify);
	return;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsManager::Initialize(void * l_pTask)
{
	if (l_pTask == 0)
	{
		PR_TRACE((g_root, prtERROR, TR "CHipsManager::Initialize l_pTask == 0, return FALSE"));
		return FALSE;
	}

	m_pTask = l_pTask;

	if (!m_HipsRuleManager.Init(m_pTask, this))
	{
		PR_TRACE((g_root, prtERROR, TR "CHipsManager::Initialize m_HipsRuleManager.Init FAIL, return FALSE"));
		return FALSE;
	}

	m_hVerify = VerifyTrustInit(FALSE);

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
inline wchar_t * CHipsManager::GetStrRes(DWORD Value)
{
	switch (Value)
	{
		case 0:
			return L"ALLOW";
		case 1:
			return L"ASK";
		case 2:
			return L"DENY";
	}
	return L"ERROR";
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//inline cHipsReportInfo::enEvent CHipsManager::GetEnumRes(DWORD Value)
//{
//	switch (Value)
//	{
//		case 0:
//			return cHipsReportInfo::ALLOW_ACTION;
//		case 1:
//			return cHipsReportInfo::UNKNOWN;
//		case 2:
//			return cHipsReportInfo::DENY_ACTION;
//	}
//	return cHipsReportInfo::UNKNOWN;
//}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
inline enObjectStatus CHipsManager::GetObjStatus(DWORD Value)
{
	switch (Value)
	{
		case efVerdict_Default://  Verdict_Default:
			return OBJSTATUS_UNKNOWN;;
		case efVerdict_Allow://  Verdict_Pass:
			return OBJSTATUS_ALLOWED;
		case efVerdict_Deny://  Verdict_Discard:
			return OBJSTATUS_DENIED;
		default:
			PR_TRACE((g_root, prtERROR,
				TR "HIPS:: CHipsManager::GetObjStatus anknown Value = %d", Value));
	}
	return OBJSTATUS_UNKNOWN;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
inline wchar_t * CHipsManager::GetStrDesiredOperation(DWORD Value, wchar_t * l_OutErrorMsg)
{
	if (l_OutErrorMsg)
	{
		l_OutErrorMsg[0] = 0;
	}
	if (Value & FILE_OP_WRITE)
	{
		return L"WRITE";
	}
	else if (Value & FILE_OP_READ)
	{
		return L"READ";
	}
	else if (Value & FILE_OP_ENUM)
	{
		return L"ENUM";
	}
	else if (Value & FILE_OP_DELETE)
	{
		return L"DELETE";
	}
	if (l_OutErrorMsg)
	{
		wsprintfW(l_OutErrorMsg, L"Unknown operation (%d)", Value);
	}
	return L"Unknown operation";
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
enEventAction CHipsManager::GetAction(DWORD Value)
{
	if (Value & FILE_OP_WRITE)
	{
		return evtWrite;
	}
	else if (Value & FILE_OP_READ)
	{
		return evtRead;
	}
	else if (Value & FILE_OP_ENUM)
	{
		return evtOpen;
	}
	else if (Value & FILE_OP_DELETE)
	{
		return evtDelete;
	}
	return evtOpen;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
enEventAction CHipsManager::GetPermissionAction(cResource * l_pRes)
{
	if (!l_pRes || !l_pRes->m_pData)
		return evtUnk;

	cPrivilegeInfo * pPriv = *(cPrivilegeInfo**)&l_pRes->m_pData;
	switch (pPriv->m_nType)
	{
		case cPrivilegeInfo::ehptProcStart:
		{
			return evtProcessStart;
		}
		case cPrivilegeInfo::ehptProcStop:
		{
			return evtProcessStop;
		}
		case cPrivilegeInfo::ehptSetHook:
		{
			return evtSetHook;
		}
		case cPrivilegeInfo::ehptCodeInject/*ehptCreateRemThread*/:
		{
			return evtCodeInject/*evtStartRemThread*/;
		}
		case cPrivilegeInfo::ehptWindowsShutDown:
		{
			return evtWindowsShutDown;
		}
		case cPrivilegeInfo::ehptKeyLogger/*ehptKeyLoggerAsyncKS*/:
		{
			return evtKeyLogger/*evtKeyLoggerAsyncKS*/;
		}
		//case cPrivilegeInfo::ehptSetThreadContext:
		//{
		//	return evtSetThreadCtx;
		//}
		case cPrivilegeInfo::ehptSetHardLink:
		{
			return evtSetHardLink;
		}
	}
	return evtUnk;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
bool CHipsManager::GetResInfo(DWORD RuleID, enObjType * l_pOutObjType, wchar_t ** l_ppOutResGrDesc, wchar_t ** l_ppOutResDesc)
{
	DWORD OutResId = 0;
	DWORD type = m_HipsRuleManager.GetResTypeByRuleID(RuleID, &OutResId);
	PR_TRACE((g_root, prtNOT_IMPORTANT,
		TR "HIPS:: CHipsManager::GetResType l_RuleID = %d, type = %d, OutResId = %d",
		RuleID, type, OutResId));
	switch (type)
	{
		case cResource::ehrtFile:
			if (l_pOutObjType) *l_pOutObjType = eFile;
			break;
		case cResource::ehrtReg:
			if (l_pOutObjType) *l_pOutObjType = eRegKey;
			break;
		default:
			if (l_pOutObjType) *l_pOutObjType = eFile;
			break;
	}

	cResource * pRes = m_HipsRuleManager.m_Settings.m_Resource.Find(OutResId);
	if (pRes)
	{
		DWORD size = wcslen((wchar_t*)pRes->m_sName.data()) + 1;
		*l_ppOutResDesc = new wchar_t[size];
		wcsncpy(*l_ppOutResDesc, (wchar_t*)pRes->m_sName.data(), size);
		cResource * pParentRes = m_HipsRuleManager.m_Settings.m_Resource.Find(pRes->m_nParentID);
		if (pParentRes)
		{
			size = wcslen((wchar_t*)pParentRes->m_sName.data()) + 1;
			*l_ppOutResGrDesc = new wchar_t[size];
			wcsncpy(*l_ppOutResGrDesc, (wchar_t*)pParentRes->m_sName.data(), size);
		}
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
enRDetectType CHipsManager::GetDetectTypeByResType(enObjType ResType)
{
	switch (ResType)
	{
		case eFile:
			return eFileAccess;
		case eRegKey:
			return eRegistryAccess;
		case eProcess:
			return eSecurityAccess;
		//case ePort: case eConnection: case ePacket:
		//	return eNetworkAccess;
		//return eHardwareAccess;
		//return eSecurityAccess;
		default:
			return eFileAccess;
	}
}
////////////////////////////////////////////////////////////////////////////////
//	write to product log HIPS event
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsManager::WriteToLog( CallBackHipsStruct * l_pStr, cResource * l_pResource)	
{
	if (l_pStr == 0 || l_pResource == 0)
	{
		PR_TRACE((g_root, prtERROR, "CHipsManager::WriteToLog:: FAIL (l_pStr = %x, l_pResource = %x)",
			l_pStr, l_pResource));
		return FALSE;
	}
	PR_TRACE((g_root, prtIMPORTANT,
		TR "HIPS:: CHipsManager::WriteToLog result = %d",
		l_pStr->m_Result
		));

	cHipsAskObjectAction action;
	action.m_strTaskType = AVP_TASK_HIPS;
	action.m_nNonCuredReason = NCREASON_REPONLY;
	action.m_nDetectDanger = DETDANGER_LOW;
	action.m_nDescription = 0;//0x800000C5;
	action.m_tmTimeStamp = cDateTime();
	action.m_nDetectStatus = DSTATUS_SURE;

	action.m_nObjectType = (enObjectType)GetObjType(l_pResource);// ObjType;
	action.m_nDetectType = (enDetectType)GetDetectTypeByResType((enObjType)action.m_nObjectType);

	GetObjName(l_pStr->m_ObjectName1, (enObjType)action.m_nObjectType, l_pStr, action.m_strObjectName);

	//	result (allow, denied)
	action.m_nObjectStatus = GetObjStatus(l_pStr->m_Result/*  l_Result*/);//OBJSTATUS_ALLOWED;
	//	read, write, delete, ...
	action.m_Action = GetAction(l_pStr->m_Action/*  l_Action*/);

	action.m_nPID = l_pStr->m_SrcPID;// l_PID;

	tERROR err;
	if (m_pTask)
		err = ((CHipsTask*)m_pTask)->sysSendMsg(pmc_EVENTS_CRITICAL, 0, 0, &action, SER_SENDMSG_PSIZE);

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//	write to product log add app events
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsManager::WriteToLogAddApp(DWORD l_SrcPID, wchar_t * l_NewGrName, wchar_t * l_OldGrName)
{
	PR_TRACE((g_root, prtIMPORTANT,
		TR "HIPS:: CHipsManager::WriteToLogAddApp l_SrcPID = %d, l_NewGrName = %S, l_OldGrName = %S",
		l_SrcPID, l_NewGrName, l_OldGrName
		));
	cHipsAskObjectAction action;
	action.m_strTaskType = AVP_TASK_HIPS;
	action.m_nNonCuredReason = NCREASON_REPONLY;
	action.m_nDetectDanger = DETDANGER_LOW;
	action.m_nDescription = 0;//0x800000C5;
	action.m_tmTimeStamp = cDateTime();
	action.m_nDetectStatus = DSTATUS_SURE;

	//action.m_nObjectType = (enObjectType)GetObjType(l_pResource);// ObjType;
	//action.m_nDetectType = (enDetectType)GetDetectTypeByResType((enObjType)action.m_nObjectType);

	//GetObjName(l_pStr->m_ObjectName1, (enObjType)action.m_nObjectType, l_pStr, action.m_strObjectName);
	action.m_strObjectName = l_NewGrName;

	//	result (allow, denied)
	//action.m_nObjectStatus = GetObjStatus(l_pStr->m_Result/*  l_Result*/);//OBJSTATUS_ALLOWED;
	//	read, write, delete, ...
	action.m_Action = evtAddAppToGr;
	action.m_nPID = l_SrcPID;// l_PID;

	tERROR err;
	if (m_pTask)
		err = ((CHipsTask*)m_pTask)->sysSendMsg(pmc_EVENTS_IMPORTANT, 0, 0, &action, SER_SENDMSG_PSIZE);//pmc_EVENTS_CRITICAL
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//	write to product log device events
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsManager::WriteToLog(wchar_t * l_pGUID, wchar_t * l_pType, DWORD l_AM)
{
	PR_TRACE((g_root, prtIMPORTANT,
		TR "HIPS:: CHipsManager::WriteToLog l_pGUID = %S, l_pType=%S, l_AM=%x",
		l_pGUID,
		l_pType, 
		l_AM));
	if (HIPS_LOG_OFF == HIPS_GET_LOG(l_AM, HIPS_POS_PERMIS))
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "HIPS:: CHipsManager::WriteToLog no need to log bu AM"));
		return TRUE;
	}

	cHipsAskObjectAction action;
	action.m_strTaskType = AVP_TASK_HIPS;
	action.m_nNonCuredReason = NCREASON_REPONLY;
	action.m_nDetectDanger = DETDANGER_LOW;
	action.m_nDescription = 0;//0x800000C5;
//	action.nNotification = eNtfNotImportant;
	//action.m_nObjectStatus = OBJSTATUS_CORRUPTED;
	action.m_tmTimeStamp = cDateTime();
	action.m_nDetectStatus = DSTATUS_SURE;

	//	reg, file, ...
	//action.m_nObjectType = (enObjectType)GetResType(l_RuleID);
	
	//enObjType ObjType;
	//GetResInfo(l_RuleID, &ObjType, 0, 0);
	action.m_nObjectType = (enObjectType)eDevice;

	cDeviceInfo Device;
	Device.m_sClassGUID = l_pGUID;
	Device.m_sTypeName = l_pType;
	cResource * pRes = m_HipsRuleManager.m_Settings.m_Resource.FindResByContent(&Device);


	if (pRes)
		action.m_strObjectName = pRes->m_sName;// : L"Unknown";// ObjectName1;//l_pProcName
	//	result (allow, denied)
	action.m_nObjectStatus = (HIPS_GET_AM(l_AM, HIPS_POS_PERMIS) == HIPS_FLAG_ALLOW) ? OBJSTATUS_ALLOWED : OBJSTATUS_DENIED;
	//	read, write, delete, ...
	action.m_Action = evtCreate;
	//	file access, reg access, ///
	action.m_nDetectType = (enDetectType)eHardwareAccess;

	action.m_nPID = 0/*l_PID*/;



	tERROR err;
	if (m_pTask)
		err = ((CHipsTask*)m_pTask)->sysSendMsg(pmc_EVENTS_CRITICAL, 0, 0, &action, SER_SENDMSG_PSIZE);

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//	write to product log permission events
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsManager::WriteToLogPerm(DWORD l_SrcPID, CHipsAppItem * l_pDestApp, wchar_t * l_CmdLine, cResource * l_pResource, DWORD l_AM, DWORD l_Result)
{
	PR_TRACE((g_root, prtIMPORTANT,
		TR "HIPS:: CHipsManager::WriteToLogPerm l_SrcPID = %d, l_pDestApp = %x(%S), l_pResource=%x, l_AM = %x, l_Result=%d",
		l_SrcPID,
		l_pDestApp,
		(l_pDestApp) ? l_pDestApp->m_AppName : ((l_CmdLine) ? l_CmdLine : L""),
		l_pResource, 
		l_AM,
		l_Result));

	if (HIPS_LOG_OFF == HIPS_GET_LOG(l_AM, HIPS_POS_PERMIS))
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "HIPS:: CHipsManager::WriteToLogPerm no need to log bu AM"));
		return TRUE;
	}

	cHipsAskObjectAction action;
	action.m_strTaskType = AVP_TASK_HIPS;
	action.m_nNonCuredReason = NCREASON_REPONLY;
	action.m_nDetectDanger = DETDANGER_LOW;
	action.m_nDescription = 0;//0x800000C5;
	action.m_tmTimeStamp = cDateTime();
	action.m_nDetectStatus = DSTATUS_SURE;

	action.m_nObjectType = (enObjectType)eProcess;

	if (l_pDestApp)
		action.m_strObjectName = l_pDestApp->m_AppName;
	else if (l_CmdLine)
		action.m_strObjectName = l_CmdLine;// : L"Unknown");//(l_pResource) ? l_pResource->m_sName : L"Unknown";
	//	result (allow, denied)
	action.m_nObjectStatus = GetObjStatus(l_Result);
	//	read, write, delete, ...
	action.m_Action = GetPermissionAction(l_pResource);
	//	file access, reg access, ///
	action.m_nDetectType = (enDetectType)eSecurityAccess;

	action.m_nPID = l_SrcPID;

	tERROR err;
	if (m_pTask)
		err = ((CHipsTask*)m_pTask)->sysSendMsg(pmc_EVENTS_CRITICAL, 0, 0, &action, SER_SENDMSG_PSIZE);

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
enObjType CHipsManager::GetObjType(cResource * l_pResource)
{
	if (l_pResource == 0 ||
		l_pResource->m_pData == 0)
		return eFile;
	switch (l_pResource->m_pData->getIID())
	{
		case cFileInfo::eIID:
			return eFile;
		case cRegKeyInfo::eIID:
			return eRegKey;
		case cDeviceInfo::eIID:
			return eDevice;
		case cPrivilegeInfo::eIID:
			return eProcess; //???
	};
	return eFile;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsManager::GetObjName(cResource * l_pResource, cStringObj & l_OutStr)
{
	if (l_pResource == 0 ||
		l_pResource->m_pData == 0)
	{
		PR_TRACE((g_root, prtERROR, TR "HIPS:: CHipsManager::GetObjName l_pResource = %x",
			l_pResource));
		//l_OutStr = "Unknown";
		return FALSE;
	}
	cFileInfo File;
	cRegKeyInfo Reg;
	switch (l_pResource->m_pData->getIID())
	{
		case cFileInfo::eIID:
			File.assign(*(l_pResource->m_pData.ptr_ref()), true);
			l_OutStr = File.m_Path;
			return TRUE;
		case cRegKeyInfo::eIID:
			Reg.assign(*(l_pResource->m_pData.ptr_ref()), true);
			l_OutStr = Reg.m_KeyPath;
			l_OutStr += "\\";
			l_OutStr += Reg.m_KeyValue;
			return TRUE;
		//case cDeviceInfo::eIID:
		//	return FALSE;
		//case cPrivilegeInfo::eIID:
		//	return FALSE; //???
	};
	PR_TRACE((g_root, prtERROR, TR "HIPS:: CHipsManager::GetObjName unknown eIID = %x",
		l_pResource->m_pData->getIID()));
	//l_OutStr = "Unknown";
	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsManager::GetObjName(wchar_t * l_pObjectName, enObjType l_ObjType, CallBackHipsStruct * l_CtxStruct, cStringObj & l_OutStr)
{
	switch (l_ObjType)
	{
		case eFile:
		{
			if (l_CtxStruct == 0)
				return FALSE;
			PWCHAR pwchFileName = 0;
			ULONG FileNameLen = 0;
			if (((CHipsTask*)m_pTask)->MakeFileName( l_CtxStruct->m_pContext,
							  l_CtxStruct->m_pMessage,
							  l_CtxStruct->m_MessageSize,
							  l_CtxStruct->m_ContextFlags,
							  &pwchFileName, &FileNameLen, false ))
			{
				l_OutStr = pwchFileName;
			}
			else
			{
				l_OutStr = l_pObjectName;
			}
			if (pwchFileName)
				pfMemFree( NULL, (void**) &pwchFileName );
			break;
		}
		case eRegKey:
		{
			if (l_CtxStruct == 0)
				return FALSE;
			wchar_t * pUserRegPath = 0;
			if (((CHipsTask*)m_pTask)->KernelRegPath2User(&pUserRegPath, l_pObjectName) && pUserRegPath)
			{
				l_OutStr = pUserRegPath;
				delete [] pUserRegPath;
				pUserRegPath = 0;
			}
			else
			{
				l_OutStr = l_pObjectName;
			}
			break;
		}
		default:
		{
			//l_OutStr = "Unknown";
			break;
		}
	};
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//	ask user for file\registry operations
////////////////////////////////////////////////////////////////////////////////
DWORD CHipsManager::GetUserAskAction(CHipsAppItem * l_pSrcApp, CallBackHipsStruct * l_pStr, cResource * l_pResource)
{
	DWORD ResVerdict = efVerdict_Allow;
	if (l_pStr == 0 || l_pResource == 0)
	{
		PR_TRACE((g_root, prtERROR, TR "HIPS:: CHipsManager::GetUserAskAction INVALID_ARG l_pResource = %x, l_pStr = %x",
			l_pResource, l_pStr));
		return ResVerdict;
	}

	cHipsAskObjectAction Action;
	Action.m_nDefaultAction = ACTION_DENY;
	//Action.m_nExcludeAction(ACTION_UNKNOWN),
	Action.m_nActionsAll = 	ACTION_ALLOW | ACTION_DENY;// | ACTION_TERMINATE;
	Action.m_nActionsMask = Action.m_nActionsAll;
	Action.m_nResultAction = Action.m_nDefaultAction;
	Action.m_nApplyToAll = (enApplyToAll)(APPLYTOALL_FALSE | APPLYTOALL_SESSION | APPLYTOALL_USE_SESSION);

	//GetResInfo(l_RuleID, &ObjType, &OutResGrDesc, &OutResDesc);
	Action.m_nObjectType = (enObjectType)GetObjType(l_pResource);// ObjType;
	Action.m_nDetectType = (enDetectType)GetDetectTypeByResType((enObjType)Action.m_nObjectType);

	//Action.m_strObjectName = l_pObjectName;
	GetObjName(l_pStr->m_ObjectName1, (enObjType)Action.m_nObjectType, l_pStr, Action.m_strObjectName);

	Action.m_nDetectDanger = DETDANGER_LOW;
	
	Action.m_nPID = l_pStr->m_SrcPID;

	//	read, write, delete, ...
	Action.m_Action = GetAction(l_pStr->m_Action);//l_Action

	cResource * pParentRes = m_HipsRuleManager.m_Settings.m_Resource.Find(l_pResource->m_nParentID);//FindResByID(0, l_pResource->m_nParentID);

	if (pParentRes)
		Action.m_strResGrName = pParentRes->m_sName;// : "Unknown";
	Action.m_strUserDescription = l_pResource->m_sName;

	tERROR err = errNOT_OK;
	if (m_pTask)
	{
		err = ((CHipsTask*)m_pTask)->sysSendMsg(pmcASK_ACTION, cHipsAskObjectAction::ASKACTION, 0, &Action, SER_SENDMSG_PSIZE);
	}
	bool IsWrongAnswer = false;
	DWORD flag = HIPS_FLAG_ALLOW;

	if ((errOK_NO_DECIDERS != err) && (PR_SUCC(err)))
	{	
		switch (Action.m_nResultAction)
		{
			case ACTION_ALLOW:
				ResVerdict = efVerdict_Allow;
				break;
			case ACTION_DENY:
				ResVerdict = efVerdict_Deny;
				flag = HIPS_FLAG_DENY;
				break;
			//case ACTION_TERMINATE:
			//	ResVerdict = efVerdict_Deny | efVerdict_TerminateProcess;
			//	break;
			default:
				PR_TRACE((g_root, prtERROR, TR "CHipsManager::GetUserAskAction: !!! unknown m_nResultAction = %d", Action.m_nResultAction));
				IsWrongAnswer = true;
				break;
		}//	switch (Action.m_nResultAction)

		if (!IsWrongAnswer && l_pResource && (Action.m_nApplyToAll & APPLYTOALL_TRUE))
		{
			if (Action.m_nApplyToAll & APPLYTOALL_SESSION)
			{
				PR_TRACE((g_root, prtIMPORTANT, TR "CHipsManager::GetUserAskAction add to cash(pid = %d, res = %x, value = %d)",
					l_pStr->m_SrcPID, l_pResource->m_nID, ResVerdict));
				m_LocalCash.AddAnswer(l_pStr->m_SrcPID, l_pResource->m_nID, ResVerdict);
			}
			else
			{
				if (l_pSrcApp)
				{
					PR_TRACE((g_root, prtIMPORTANT, TR "CHipsManager::GetUserAskAction add rule(pid = %d, res = %x, value = %d)",
						l_pStr->m_SrcPID, l_pResource->m_nID, ResVerdict));
					m_LocalCash.AddAnswer(l_pStr->m_SrcPID, l_pResource->m_nID, ResVerdict);
					CHipsRuleItem Rule;
					Rule.m_RuleId = m_HipsRuleManager.m_Settings.GetUniqueRuleID();
					Rule.m_RuleState = CHipsRuleItem::fEnabled;
					Rule.m_RuleTypeId = CHipsRuleItem::ehrltLocDeny;
					Rule.m_AppIdEx = l_pSrcApp->m_AppId;
					Rule.m_ResIdEx = l_pResource->m_nID;
					Rule.m_ResIdEx2 = 0;
					Rule.m_AccessFlag = HIPS_SET_BLOCK_POS(flag,	HIPS_LOG_ON, 0,	HIPS_POS_WRITE,		Rule.m_AccessFlag);
					Rule.m_AccessFlag = HIPS_SET_BLOCK_POS(flag,	HIPS_LOG_ON, 0,	HIPS_POS_READ,		Rule.m_AccessFlag);
					Rule.m_AccessFlag = HIPS_SET_BLOCK_POS(flag,	HIPS_LOG_ON, 0,	HIPS_POS_ENUM,		Rule.m_AccessFlag);
					Rule.m_AccessFlag = HIPS_SET_BLOCK_POS(flag,	HIPS_LOG_ON, 0,	HIPS_POS_DELETE,	Rule.m_AccessFlag);

					//Rule.m_AccessFlag = HIPS_SET_BLOCK(
					//	((ResVerdict & efVerdict_Deny) ? HIPS_FLAG_DENY : HIPS_FLAG_ALLOW),
					//	HIPS_LOG_ON, 0);
					m_HipsRuleManager.m_Settings.CreateRule(&Rule);
					((CHipsTask*)m_pTask)->sysSendMsg(pmc_TASK_SETTINGS_CHANGED, 0, NULL, NULL, NULL);
				}
			}
		}
	}	//errOK_NO_DECIDERS || ask fail
	else
	{
		PR_TRACE((g_root, prtERROR, TR "HIPS:: CHipsManager::GetUserAskAction sysSendMsg(pmcASK_ACTION) err = 0x%x, return 0x%x", err, ResVerdict));
	}
	return ResVerdict;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsManager::GetFileDigitalSignature(wchar_t * l_ProcName, wchar_t ** l_pOutStr)
{
	PR_TRACE((g_root, prtIMPORTANT,
		TR "HIPS:: CHipsManager::GetFileDigitalSignature l_ProcName = %S",
		l_ProcName));

	if (l_ProcName == 0 || l_pOutStr == 0)
		return FALSE;
	if (!InitWinTrustApi())
	{
		PR_TRACE((g_root, prtERROR, TR "HIPS:: CHipsManager::GetFileDigitalSignature InitWinTrustApi FAIL"));
		return FALSE;
	}
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(l_ProcName))
	{
		PR_TRACE((g_root, prtERROR, TR "HIPS:: CHipsManager::GetFileDigitalSignature GetFileAttributesW return INVALID_FILE_ATTRIBUTES"));
		return FALSE;
	}
	LPWSTR pName = NULL;
	HANDLE hWVTStateData;

	if (WTHlpVerifyEmbeddedSignature(0, l_ProcName, &hWVTStateData)) {
		DWORD dwStrType = CERT_X500_NAME_STR;
		if (!WTHlpGetCertName(hWVTStateData, CERT_NAME_ATTR_TYPE,
								0,  szOID_ORGANIZATION_NAME, &pName)) {
			WTHlpFreeStateData(hWVTStateData);
			PR_TRACE((g_root, prtERROR, TR "HIPS:: CHipsManager::GetFileDigitalSignature WTHlpGetCertName FAIL"));
			return FALSE;
		}
		WTHlpFreeStateData(hWVTStateData);
		*l_pOutStr = new wchar_t[wcslen(pName)+1];
		wcscpy(*l_pOutStr, pName);
		WTHlpFreeCertName(pName);
		return TRUE;
	}
	PR_TRACE((g_root, prtERROR, TR "HIPS:: CHipsManager::GetFileDigitalSignature WTHlpVerifyEmbeddedSignature FAIL"));
	
	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
EnumSecRating CHipsManager::GetSecRating(wchar_t * l_ProcName)
{
//enum EnumSecRating {
//	eSRTrusted			= 3,
//	eSRSecurity			= 4,
//	eSRRestrictedLow	= 5,
//	eSRRestrictedHi		= 6,
//	eSRUntrusted		= 7
//};
	//	check packed and other abnormal structure
	PR_TRACE((g_root, prtIMPORTANT, TR "HIPS:: CHipsManager::GetSecRating l_ProcName = %S", l_ProcName));
	
	if (l_ProcName == 0)
	{
		PR_TRACE((g_root, prtERROR, TR "HIPS:: CHipsManager::GetSecRating l_ProcName == 0, return eSRRestrictedHi"));
		return eSRRestrictedHi;
	}

	if (m_pTask == 0)
	{
		PR_TRACE((g_root, prtERROR, TR "HIPS:: CHipsManager::GetSecRating m_pTask == 0, return eSRRestrictedHi"));
		return eSRRestrictedHi;
	}


	//	temporarily (while kav not signed)
	if (0 == _wcsicmp(l_ProcName, L"c:\\Program Files\\Kaspersky Lab\\Kaspersky Internet Security 8.0\\avp.exe"))
	{
		return eSRSecurity;
	}
	if (0 == _wcsicmp(l_ProcName, L"c:\\Program Files\\Kaspersky Lab\\Kaspersky Internet Security 8.0\\avp.com"))
	{
		return eSRSecurity;
	}
	if (0 == _wcsicmp(l_ProcName, L"c:\\Program Files\\Kaspersky Lab\\Kaspersky Anti-Virus 8.0\\avp.exe"))
	{
		return eSRSecurity;
	}
	if (0 == _wcsicmp(l_ProcName, L"c:\\Program Files\\Kaspersky Lab\\Kaspersky Anti-Virus 8.0\\avp.com"))
	{
		return eSRSecurity;
	}

	//	check security rating
	cIOObj hIo((cObject*)m_pTask, cAutoString(l_ProcName), fACCESS_READ);
	tERROR error = hIo.last_error();
	tQWORD nFileSize;
	if(PR_SUCC(error) && PR_SUCC(hIo->GetSize(&nFileSize, IO_SIZE_TYPE_EXPLICIT)))
	{
		if (nFileSize < 30 * 1024)
		{
			PR_TRACE((g_root, prtIMPORTANT, TR "HIPS:: CHipsManager::GetSecRating nFileSize(%d) < 30 * 1024, return eSRRestrictedHi", nFileSize));
			return eSRRestrictedHi;
		}
	}
	PR_TRACE((g_root, prtIMPORTANT, TR "HIPS:: CHipsManager::GetSecRating return eSRRestrictedLow"));
	return eSRRestrictedLow;
}
////////////////////////////////////////////////////////////////////////////////
//	if it need update only m_Settings (AppList)
//	return IsNeedToReloadRule
////////////////////////////////////////////////////////////////////////////////
bool CHipsManager::CheckRuleForApp(wchar_t * l_ProcName, BYTE * l_pHash, DWORD l_HashSize, CHipsAppItem ** l_ppApp)
{
	return false;
	//PR_TRACE((g_root, prtIMPORTANT,
	//	TR "HIPS:: CHipsManager::CheckRuleForApp l_ProcName = %S",
	//	l_ProcName));
	//bool IsNeedToReloadRule = false;

	//if (l_ProcName == 0)
	//	return false;

	////	try to find app in base
	//CHipsAppItem * App;
	//if (PR_SUCC(m_HipsRuleManager.FindAppByHash(l_pHash, l_HashSize, &App)))
	//{
	//	PR_TRACE((g_root, prtIMPORTANT,
	//		TR "HIPS:: CHipsManager::CheckRuleForApp FindAppByHash(%S) return OK (AppId=%d)",
	//		l_ProcName,
	//		App->m_AppId));
	//	return false;
	//}

	////	application was no start before

	//bool IsSigned = false;
	//bool AppWasAdded = false;
	//wchar_t * Signature = 0;
	//wchar_t * SignatureMS = 0;
	//tERROR err = 0;
	////	check for digital signature (cat-root)
	//wchar_t * CatFileName = 0;
	//if (VTIsMicrosoftSigned(m_hVerify, l_ProcName, &CatFileName))
	//{
	//	IsSigned = true;
	//	PR_TRACE((g_root, prtIMPORTANT,
	//		TR "HIPS:: CHipsManager::CheckRuleForApp VTIsMicrosoftSigned(%S) return OK",
	//		l_ProcName, Signature));
	//	SignatureMS = L"Microsoft Corporation";
	//}
	////	check for digital signature (embedded)
	//else if (GetFileDigitalSignature(l_ProcName, &Signature) && Signature)
	//{
	//	IsSigned = true;
	//	PR_TRACE((g_root, prtIMPORTANT,
	//		TR "HIPS:: CHipsManager::CheckRuleForApp GetFileDigitalSignature(%S) return '%S'",
	//		l_ProcName, Signature));
	//}
	//else
	//{
	//	PR_TRACE((g_root, prtIMPORTANT,
	//		TR "HIPS:: CHipsManager::CheckRuleForApp file(%S) not signed",
	//		l_ProcName));
	//}

	//if (IsSigned)
	//{
	//	//	app was signed
	//	cAppGroup * AppGr;
	//	cAppGroup * ParentAppGr;

	//	//	try to find existing (known) app group in current settings
	//	if (PR_SUCC(m_HipsRuleManager.FindAppGrByServInfo(
	//					(SignatureMS) ? SignatureMS : Signature,
	//					1, HIPS_BASE_TYPE_CURRENT, &AppGr, &ParentAppGr)))
	//	{
	//		//	such group exist
	//		//	need to add this app to this group (auto inherit all rules)
	//		PR_TRACE((g_root, prtIMPORTANT,
	//			TR "HIPS:: CHipsManager::CheckRuleForApp FindAppGrByServInfo OK",
	//			l_ProcName));
	//		err = m_HipsRuleManager.AddApp(l_ProcName, l_pHash, l_HashSize, *AppGr, 0);
	//		if (PR_SUCC(err))
	//		{
	//			AppWasAdded = true;
	//		}
	//		else
	//		{
	//			PR_TRACE((g_root, prtERROR,
	//				TR "HIPS:: CHipsManager::CheckRuleForApp m_HipsRuleManager.AddApp(%S) return %x",
	//				l_ProcName,
	//				err));
	//		}
	//	} 
	//	//	try to find existing (known) app group in standard settings
	//	else if (PR_SUCC(m_HipsRuleManager.FindAppGrByServInfo(
	//					(SignatureMS) ? SignatureMS : Signature,
	//					1, HIPS_BASE_TYPE_STANDARD, &AppGr, &ParentAppGr)))
	//	{
	//		//	such group exist
	//		//	need to add this app to this group (auto inherit all rules)
	//		PR_TRACE((g_root, prtIMPORTANT,
	//			TR "HIPS:: CHipsManager::CheckRuleForApp FindAppGrByServInfo OK",
	//			l_ProcName));
	//		err = m_HipsRuleManager.AddApp(l_ProcName, l_pHash, l_HashSize, *AppGr, (ParentAppGr) ? ParentAppGr->m_nAppGrpID : 0);
	//		if (PR_SUCC(err))
	//		{
	//			AppWasAdded = true;
	//		}
	//		else
	//		{
	//			PR_TRACE((g_root, prtERROR,
	//				TR "HIPS:: CHipsManager::CheckRuleForApp m_HipsRuleManager.AddApp(%S, gr=%x) return %x",
	//				l_ProcName,
	//				ParentAppGr->m_nAppGrpID,
	//				err));
	//		}
	//	}

	//}
	//if (!AppWasAdded)
	//{
	//	//	app signed by unknow signature or not signed
	//	EnumSecRating SerRtg = GetSecRating(l_ProcName);
	//	PR_TRACE((g_root, prtIMPORTANT,
	//		TR "HIPS:: CHipsManager::CheckRuleForApp GetSecRating return %d", SerRtg));
	//	cAppGroup * pOutGr = 0;
	//	cAppGroup * pParentAppGr = 0;
	//	if (PR_SUCC(m_HipsRuleManager.m_Settings.GetAppGrById(
	//					m_HipsRuleManager.m_Settings.m_AppGroups,
	//					SerRtg,
	//					&pOutGr,
	//					&pParentAppGr)) ||
	//		PR_SUCC(m_HipsRuleManager.m_Settings.GetAppGrById(
	//								m_HipsRuleManager.m_pStandardSettings->m_AppGroups,
	//								SerRtg,
	//								&pOutGr,
	//								&pParentAppGr))
	//		)
	//	{
	//		//	such group exist
	//		//	need to add this app to this group (auto inherit all rules)
	//		PR_TRACE((g_root, prtIMPORTANT,
	//			TR "HIPS:: CHipsManager::CheckRuleForApp add app(%S) to gr==%d",
	//			l_ProcName, SerRtg));
	//		err = m_HipsRuleManager.AddApp(
	//					l_ProcName, l_pHash, l_HashSize, *pOutGr,
	//					(pParentAppGr && (SerRtg==eSRSecurity)) ? pParentAppGr->m_nAppGrpID : 0);
	//		if (PR_SUCC(err))
	//		{
	//			AppWasAdded = true;
	//		}
	//		else
	//		{
	//			PR_TRACE((g_root, prtERROR,
	//				TR "HIPS:: CHipsManager::CheckRuleForApp m_HipsRuleManager.AddApp(%S) return %x",
	//				l_ProcName,
	//				err));
	//		}
	//	}
	//}
	//if (AppWasAdded)
	//{
	//	IsNeedToReloadRule = true;
	//	if (l_ppApp && 
	//		PR_SUCC(m_HipsRuleManager.FindAppByHash(l_pHash, l_HashSize, &App)))
	//	{
	//		*l_ppApp = App;
	//	}
	//}
	//else
	//{
	//	PR_TRACE((g_root, prtERROR, TR "HIPS:: CHipsManager::CheckRuleForApp App was not added !?!?!!?" ));
	//}

	//if (Signature)
	//	delete [] Signature;

	//PR_TRACE((g_root, prtIMPORTANT,	TR "HIPS:: CHipsManager::CheckRuleForApp return(IsNeedToReloadRule) %d", IsNeedToReloadRule));
	//return IsNeedToReloadRule;
}
////////////////////////////////////////////////////////////////////////////////
//	return IsNeedToReloadRule
////////////////////////////////////////////////////////////////////////////////
bool CHipsManager::CheckRuleForAppNew(cProcMonNotifyEx * l_pProcMonNotifyEx, bool l_IsNeedToReloadToDrv)
{
	//DebugBreak();
	if (!l_pProcMonNotifyEx)
		return false;

	PR_TRACE((g_root, prtIMPORTANT, TR "CHipsManager::CheckRuleForAppNew PID = %d, name = %S",
		l_pProcMonNotifyEx->m_ProcessId,
		l_pProcMonNotifyEx->m_sImagePath.data()));

	BYTE ProcHash[HIPS_HASH_SIZE];
	bool IsNeedToReloadRule = false;
	cStringObj NewGrName = "";
	cStringObj ParentGrName;

	//	get app hash
	HRESULT hResult = MKL_QueryProcessHash(
			m_pClientContext,
			l_pProcMonNotifyEx->m_ProcessId,
			&ProcHash[0],
			HIPS_HASH_SIZE);
	if (!SUCCEEDED(hResult))
		return false;
	//	try to find app in base
	CHipsAppItem * App;
	if (PR_SUCC(m_HipsRuleManager.FindAppByHash(ProcHash, HIPS_HASH_SIZE, &App)))
	{
		PR_TRACE((g_root, prtIMPORTANT,
			TR "HIPS:: CHipsManager::CheckRuleForAppNew FindAppByHash(%S) return OK (AppId=0x%x)",
			l_pProcMonNotifyEx->m_sImagePath.data(),
			App->m_AppId));
		return false;
	}

	////	application was no start before
	PR_TRACE((g_root, prtIMPORTANT, TR "m_Signature.m_nTrustFlags = %x, KLSRL.errstat = %d",
		l_pProcMonNotifyEx->m_SecRating.m_Signature.m_nTrustFlags,
		l_pProcMonNotifyEx->m_SecRating.m_KlsrlInfo.m_errStatus
		));

	if ((0 == _wcsicmp(l_pProcMonNotifyEx->m_sImagePath.data(), L"c:\\Program Files\\Kaspersky Lab\\Kaspersky Internet Security 8.0\\avp.exe")) ||
		(0 == _wcsicmp(l_pProcMonNotifyEx->m_sImagePath.data(), L"c:\\Program Files\\Kaspersky Lab\\Kaspersky Internet Security 8.0\\avp.com")) ||
		(0 == _wcsicmp(l_pProcMonNotifyEx->m_sImagePath.data(), L"c:\\Program Files\\Kaspersky Lab\\Kaspersky Anti-Virus 8.0\\avp.exe")) ||
		(0 == _wcsicmp(l_pProcMonNotifyEx->m_sImagePath.data(), L"c:\\Program Files\\Kaspersky Lab\\Kaspersky Anti-Virus 8.0\\avp.com"))
		)
	{
		NewGrName = "Kaspersky";
		ParentGrName = "KLAppTrusted";
	}
	else if ((PR_SUCC(l_pProcMonNotifyEx->m_SecRating.m_ScanError)) &&
		(l_pProcMonNotifyEx->m_SecRating.m_Signature.m_nTrustFlags & (TF_SIGNED | TF_TRUSTED)))
	{
		//	app signed and trusted
		PR_TRACE((g_root, prtIMPORTANT, TR "signed and trusted (common name = %S, org = %S)",
			l_pProcMonNotifyEx->m_SecRating.m_Signature.m_sSignerCommonName.data(),
			l_pProcMonNotifyEx->m_SecRating.m_Signature.m_sSignerOrganization.data() ));

		NewGrName = l_pProcMonNotifyEx->m_SecRating.m_Signature.m_sSignerOrganization;
		ParentGrName = "KLAppTrusted";
	}
	else if (l_pProcMonNotifyEx->m_SecRating.m_KlsrlInfo.m_errStatus == errOK)
	{
		//	app is in KLSRL
		PR_TRACE((g_root, prtIMPORTANT, TR "KLSRL: vendor = %S, product = %S",
			l_pProcMonNotifyEx->m_SecRating.m_KlsrlInfo.m_sVendor.data(),
			l_pProcMonNotifyEx->m_SecRating.m_KlsrlInfo.m_sProduct.data()
			));

		NewGrName = l_pProcMonNotifyEx->m_SecRating.m_KlsrlInfo.m_sVendor;
		ParentGrName = "KLAppTrusted";
	}
	else
	{
		//	app unknown, check emul rating
		//DebugBreak();
		PR_TRACE((g_root, prtIMPORTANT, TR "m_EmulRating = %d",
			l_pProcMonNotifyEx->m_SecRating.m_EmulRating));
		ParentGrName = "KLAppRestrictedLow";
		//if (l_pProcMonNotifyEx->m_SecRating.m_EmulRating != -1)
		//{
		//}
		//else
		//{
		//	ParentGrName = "KLAppRestrictedLow";//"KLAppRestrictedHi";
		//}
	}

	PR_TRACE((g_root, prtIMPORTANT, TR "result: NewGrName = %S, ParentGrName = %S",
		NewGrName.data(), ParentGrName.data()));

	cAppGroup * pGroup = 0;
	cAppGroup * pParentGr = m_HipsRuleManager.m_Settings.m_AppGroups.FindByName(ParentGrName);
	if (!pParentGr)
	{
		PR_TRACE((g_root, prtERROR, TR "ParentGrName(%S) was not found", ParentGrName.data()));
		return errNOT_OK;
	}

	if (NewGrName == "")
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "NewGrName empty, put to parent"));
		pGroup = pParentGr;
		NewGrName = ParentGrName;
	}
	else
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "NewGrName not empty, try to found(%S)", NewGrName.data()));
		if (!(pGroup = m_HipsRuleManager.m_Settings.m_AppGroups.FindByName(NewGrName)))
		{
			//	gr not exist
			//	create
			PR_TRACE((g_root, prtIMPORTANT, TR "NewGrName was not founded, try to create"));
			pGroup = pParentGr->Add(NewGrName, m_HipsRuleManager.m_Settings.GetUniqueAppGrID());
		}
	}
	if (!pGroup)
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "pGroup == 0 !!!, FAIL"));
		return errNOT_OK;
	}

	IsNeedToReloadRule = true;
	//	add app
	CHipsAppItem * pApp = m_HipsRuleManager.AddApp((wchar_t*)l_pProcMonNotifyEx->m_sImagePath.data(), ProcHash, HIPS_HASH_SIZE, *pGroup, 0);

	if (pApp)
	{
		//	write to log
		WriteToLogAddApp(l_pProcMonNotifyEx->m_ProcessId, (wchar_t*)NewGrName.data(), 0);
	}

	if (pApp && l_IsNeedToReloadToDrv && (NewGrName != "Kaspersky"))
	{
		//	app added and need to reload to drv
		PR_TRACE((g_root, prtIMPORTANT, TR "IsNeedToReloadRule true"));

		//	need to update internal lists
		HipsRulesList tempHipsRulesList;
		PR_TRACE((g_root, prtIMPORTANT, TR "before AddRulesForAppToList"));
		m_HipsRuleManager.AddRulesForAppToList(pApp, &tempHipsRulesList);
		PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadProcNotifyCallback:: after AddRulesForAppToList (size = %d)", tempHipsRulesList.size()));
		//	need to upload rule to drive for this app
		PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadProcNotifyCallback:: before ReloadRulesInDrv"));
		((CHipsTask*)m_pTask)->ReloadRulesInDrv(&tempHipsRulesList);
		PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadProcNotifyCallback:: after ReloadRulesInDrv"));
		HipsRulesListIt It = tempHipsRulesList.begin();
		while(It != tempHipsRulesList.end())
		{
			if (*It)
			{
				m_HipsRuleManager.m_HipsRulesList.push_back(*It);
			}
			It++;
		};
		tempHipsRulesList.clear();
	}

	//	//pRating->m_KlsrlInfo.m_vTrustedFlags 
	//	//pRating->m_KlsrlInfo.m_sVendor 
	//	//pRating->m_KlsrlInfo.m_sProduct 
	//	//pRating->m_KlsrlInfo.m_HashMD5 
	//	//pRating->m_ModuleInfo.m_sImagePath 

	//}
	return IsNeedToReloadRule;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
bool CHipsManager::CreateThreadPool (
	cThreadPool** ppThPool,
	tDWORD MaxLen,
	tThreadPriority Priority,
	tDWORD MinThreads,
	tDWORD MaxThreads
	)
{
	if (m_pTask == 0)
	{
		PR_TRACE(( g_root, prtERROR, TR "CHipsManager::CreateThreadPool:: m_pTask == 0"));
		return false;
	}
	tERROR error = g_root->sysCreateObject( (hOBJECT*)ppThPool, IID_THREADPOOL, PID_THPOOLIMP );
	
	if (PR_FAIL( error ))
	{
		*ppThPool = 0;
		return false;
	}

	(*ppThPool)->set_pgTP_TASK_QUEUE_MAX_LEN( MaxLen );

	(*ppThPool)->set_pgTP_DEFAULT_THREAD_PRIORITY( Priority );
	(*ppThPool)->set_pgTP_MIN_THREADS( MinThreads );
	(*ppThPool)->set_pgTP_MAX_THREADS( MaxThreads );

	(*ppThPool)->set_pgTP_QUICK_EXECUTE( cFALSE );
	(*ppThPool)->set_pgTP_QUICK_DONE( cFALSE );

	(*ppThPool)->set_pgTP_THREAD_CALLBACK( OnThreadInitCallback );
	(*ppThPool)->set_pgTP_THREAD_CALLBACK_CONTEXT( m_pTask );

	error = (*ppThPool)->sysCreateObjectDone();
	
	if (PR_FAIL(error))
	{
		(*ppThPool)->sysCloseObject();
		*ppThPool = 0;

		return false;
	}

	return true;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsManager::InitThreadPools()
{
	//	create thread pool for ask events
	if (!CreateThreadPool( &m_ThPoolInfo, 0, TP_THREADPRIORITY_TIME_CRITICAL, 1, 1))
	{
		if (m_ThPoolInfo)
		{
			m_ThPoolInfo->sysCloseObject();
			m_ThPoolInfo = 0;
		}
		PR_TRACE((g_root, prtERROR, TR "CHipsManager::InitThreadPools:: create m_ThPoolInfo failed" ));
		return FALSE;
	}

	//	create thread pool for proc notify events
	if (!CreateThreadPool( &m_ThPoolProcNotify, 0, TP_THREADPRIORITY_TIME_CRITICAL, 1, 1))
	{
		if (m_ThPoolProcNotify)
		{
			m_ThPoolProcNotify->sysCloseObject();
			m_ThPoolProcNotify = 0;
		}
		PR_TRACE((g_root, prtERROR, TR "CHipsManager::InitThreadPools:: create m_ThPoolProcNotify failed" ));
		return FALSE;
	}

	//	create thread pool for proc notify events
	if (!CreateThreadPool( &m_ThPoolLog, 0, TP_THREADPRIORITY_TIME_CRITICAL, 1, 1))
	{
		if (m_ThPoolLog)
		{
			m_ThPoolLog->sysCloseObject();
			m_ThPoolLog = 0;
		}
		PR_TRACE((g_root, prtERROR, TR "CHipsManager::InitThreadPools:: create m_ThPoolLog failed" ));
		return FALSE;
	}
	//	create thread pool for proc notify events
	if (!CreateThreadPool( &m_ThPoolOtherEv, 0, TP_THREADPRIORITY_TIME_CRITICAL, 1, 3))
	{
		if (m_ThPoolOtherEv)
		{
			m_ThPoolOtherEv->sysCloseObject();
			m_ThPoolOtherEv = 0;
		}
		PR_TRACE((g_root, prtERROR, TR "CHipsManager::InitThreadPools:: create m_ThPoolOtherEv failed" ));
		return FALSE;
	}

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void CHipsManager::FreeThreadPools()
{
	if (m_ThPoolInfo)
	{
		m_ThPoolInfo->sysCloseObject();
		m_ThPoolInfo = NULL;
	}

	if (m_ThPoolProcNotify)
	{
		m_ThPoolProcNotify->sysCloseObject();
		m_ThPoolProcNotify = NULL;
	}

	if (m_ThPoolLog)
	{
		m_ThPoolLog->sysCloseObject();
		m_ThPoolLog = NULL;
	}

	if (m_ThPoolOtherEv)
	{
		m_ThPoolOtherEv->sysCloseObject();
		m_ThPoolOtherEv = NULL;
	}

	return;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
DWORD CHipsManager::GetPermissionFlag(CHipsAppItem * l_pSrcApp, CHipsAppItem * l_pDestApp, DWORD l_Permission, cResource ** l_ppOutRes)
{
	PR_TRACE((g_root, prtIMPORTANT, TR "CHipsManager::GetPermissionFlag start"));

	if (l_pSrcApp == 0/* || l_pDestApp == 0*/)
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "CHipsManager::GetPermissionFlag wrong params"));
		return HIPS_FLAG_ALLOW;
	}
	//	check global rules
	DWORD TempState = m_HipsRuleManager.m_Settings.m_PermissionsState;
	if (TempState == CHipsSettings::ehsssAllow)
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "CHipsManager::GetPermissionFlag ALLOW by m_PermissionsState"));
		return HIPS_FLAG_ALLOW;
	}
	else if (TempState == CHipsSettings::ehsssDeny)
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "CHipsManager::GetPermissionFlag DENY by m_PermissionsState"));
		return HIPS_FLAG_DENY;
	}



	CHipsRuleItem * pRealRule = 0;
	CHipsRuleItem VirtRule;

	cPrivilegeInfo Privilege;
	Privilege.m_nType = l_Permission;
	cResource * pRes = m_HipsRuleManager.m_Settings.m_Resource.FindResByContent(&Privilege);
	if (pRes == 0)
	{
		PR_TRACE((g_root, prtERROR, TR "CHipsManager::GetPermissionFlag FindResByContent(%d) fail", l_Permission));
		return HIPS_FLAG_ALLOW;
	}

	if (l_ppOutRes)
	{
		*l_ppOutRes = pRes;
	}

	PR_TRACE((g_root, prtIMPORTANT, TR "CHipsManager::GetPermissionFlag before GetRule (l_pSrcApp->m_AppId = %x, pRes->m_nID = %x)",
		l_pSrcApp->m_AppId,
		pRes->m_nID));
	if (PR_SUCC(m_HipsRuleManager.m_Settings.GetRule(
							l_pSrcApp->m_AppId , false,
							pRes->m_nID, false,
							&pRealRule, VirtRule)))
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "CHipsManager::GetPermissionFlag GetRule OK (pRealRule = %x)", pRealRule));
		if (pRealRule == 0)
			pRealRule = &VirtRule;
	}
	else 
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "CHipsManager::GetPermissionFlag GetRule FAIL"));
	}

	if (pRealRule)
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "CHipsManager::GetPermissionFlag m_RuleId = %x, m_ResIdEx = %x, m_AccessFlag = %x",
			pRealRule->m_RuleId, pRealRule->m_ResIdEx, pRealRule->m_AccessFlag));
	}

	PR_TRACE((g_root, prtIMPORTANT, TR "CHipsManager::GetPermissionFlag retrun ok"));
	return (pRealRule) ? pRealRule->m_AccessFlag : HIPS_FLAG_ALLOW;
}
////////////////////////////////////////////////////////////////////////////////
//	ask user for permissions
////////////////////////////////////////////////////////////////////////////////
DWORD CHipsManager::AskPermission(DWORD l_SrcPID, CHipsAppItem * l_pSrcApp, CHipsAppItem * l_pDestApp, cResource * l_pRes, wchar_t * l_CmdLine)
{
	if (/*l_pSrcApp == 0 || */l_pRes == 0)
	{
		PR_TRACE((g_root, prtERROR, TR "AskPermission ERROR (l_pSrcApp=%x, l_pRes=%x), return efVerdict_Allow",
			l_pSrcApp, l_pRes));
		return efVerdict_Allow;
	}
	cHipsAskObjectAction Action;
	Action.m_nDefaultAction = ACTION_DENY;
	Action.m_nActionsAll = 	ACTION_ALLOW | ACTION_DENY | ACTION_TERMINATE;
	Action.m_nActionsMask = Action.m_nActionsAll;
	Action.m_nResultAction = Action.m_nDefaultAction;
	Action.m_nApplyToAll = (enApplyToAll)(APPLYTOALL_FALSE | APPLYTOALL_SESSION | APPLYTOALL_USE_SESSION);

	Action.m_nObjectType = (enObjectType)GetObjType(l_pRes);
	Action.m_nDetectType = (enDetectType)GetDetectTypeByResType((enObjType)Action.m_nObjectType);


	if (l_pDestApp)
		Action.m_strObjectName = l_pDestApp->m_AppName;
	else
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "l_pDestApp == 0"));
		if (l_CmdLine)
			Action.m_strObjectName = l_CmdLine;// : L"UNKNOWN";
	}

	Action.m_nDetectDanger = DETDANGER_LOW;
	Action.m_nPID = l_SrcPID;

	//	read, write, delete, ...
	Action.m_Action = GetPermissionAction(l_pRes);
	cResource * pParentRes = m_HipsRuleManager.m_Settings.m_Resource.Find(l_pRes->m_nParentID);
	if (pParentRes)
		Action.m_strResGrName = pParentRes->m_sName;// : "Unknown";
	Action.m_strUserDescription = l_pRes->m_sName;

	BOOL bRes = TRUE;
	tERROR err;
	if (m_pTask)
	{
		err = ((CHipsTask*)m_pTask)->sysSendMsg(pmcASK_ACTION, cHipsAskObjectAction::ASKACTION, 0, &Action, SER_SENDMSG_PSIZE);
	}
	DWORD ResVerdict = efVerdict_Allow;
	bool IsWrongAnswer = false;

	if ((errOK_NO_DECIDERS != err) && (PR_SUCC(err)))
	{	
		switch (Action.m_nResultAction)
		{
			case ACTION_ALLOW:
				ResVerdict = efVerdict_Allow;
				break;
			case ACTION_DENY:
				ResVerdict = efVerdict_Deny;
				break;
			case ACTION_TERMINATE:
				ResVerdict = efVerdict_Deny | efVerdict_TerminateProcess;
				break;
			default:
				PR_TRACE((g_root, prtERROR, TR "CHipsManager::AskPermission: !!! unknown m_nResultAction = %d", Action.m_nResultAction));
				IsWrongAnswer = true;
				break;
		}//	switch (Action.m_nResultAction)

		if (!IsWrongAnswer && l_pRes && (Action.m_nApplyToAll & APPLYTOALL_TRUE))
		{
			if (Action.m_nApplyToAll & APPLYTOALL_SESSION)
			{
				PR_TRACE((g_root, prtIMPORTANT, TR "CHipsManager::AskPermission add to cash(pid = %d, res = %x, value = %d)",
					l_SrcPID, l_pRes->m_nID, ResVerdict));
				m_LocalCash.AddAnswer(l_SrcPID, l_pRes->m_nID, ResVerdict);
			}
			else
			{
				if (l_pSrcApp)
				{
					PR_TRACE((g_root, prtIMPORTANT, TR "CHipsManager::AskPermission add rule(pid = %d, res = %x, value = %d)",
						l_SrcPID, l_pRes->m_nID, ResVerdict));
					m_LocalCash.AddAnswer(l_SrcPID, l_pRes->m_nID, ResVerdict);
					CHipsRuleItem Rule;
					Rule.m_RuleId = m_HipsRuleManager.m_Settings.GetUniqueRuleID();
					Rule.m_RuleState = CHipsRuleItem::fEnabled;
					Rule.m_RuleTypeId = CHipsRuleItem::ehrltLocDeny;
					Rule.m_AppIdEx = l_pSrcApp->m_AppId;
					Rule.m_ResIdEx = l_pRes->m_nID;
					Rule.m_ResIdEx2 = 0;
					Rule.m_AccessFlag = HIPS_SET_BLOCK(
						((ResVerdict & efVerdict_Deny) ? HIPS_FLAG_DENY : HIPS_FLAG_ALLOW),
						HIPS_LOG_ON, 0);
					m_HipsRuleManager.m_Settings.CreateRule(&Rule);
					((CHipsTask*)m_pTask)->sysSendMsg(pmc_TASK_SETTINGS_CHANGED, 0, NULL, NULL, NULL);
					//m_HipsRuleManager.m_Settings.SetRule(Rule);
				}
			}
		}
	}	//errOK_NO_DECIDERS || ask fail
	else
	{
		PR_TRACE((g_root, prtERROR, TR "HIPS:: CHipsManager::GetUserAskAction sysSendMsg(pmcASK_ACTION) err = 0x%x, return 0x%x", err, ResVerdict));
	}
	return ResVerdict;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
DWORD CHipsManager::GetPermissionResult(DWORD l_SrcPID, CHipsAppItem * l_pSrcApp, CHipsAppItem * l_pDestApp, wchar_t * l_CmdLine, DWORD l_Permission)
{
	PR_TRACE(( g_root, prtIMPORTANT, TR "CHipsManager::GetPermissionResult start (src=%x, dest=%x, perm=%d)", l_pSrcApp, l_pDestApp, l_Permission));
	cResource * pRes = 0;
	DWORD ResVerdict = efVerdict_Allow;
	bool WrongAM = false;
	DWORD AccessMask = GetPermissionFlag(l_pSrcApp, l_pDestApp, l_Permission, &pRes);
	PR_TRACE(( g_root, prtIMPORTANT, TR "GetPermissionResult return AccessMask=%x", AccessMask));
	switch (HIPS_GET_AM(AccessMask, HIPS_POS_PERMIS))
	{
		case HIPS_FLAG_DENY: //	need deny
		{
			PR_TRACE(( g_root, prtIMPORTANT, TR "set deny"));
			ResVerdict = efVerdict_Deny;
			break;
		}
		case HIPS_FLAG_ALLOW: //	need deny
		{
			PR_TRACE(( g_root, prtIMPORTANT, TR "set allow"));
			ResVerdict = efVerdict_Allow;
			break;
		}
		case HIPS_FLAG_ASK:
		{
			PR_TRACE(( g_root, prtIMPORTANT, TR "ask user"));
			ResVerdict = AskPermission(l_SrcPID, l_pSrcApp, l_pDestApp, pRes, l_CmdLine);
			break;
		}
		default:
		{
			WrongAM = true;
			break;
		}
	};

	WriteToLogPerm(l_SrcPID, l_pDestApp, l_CmdLine, pRes, AccessMask, ResVerdict);
	
	if (WrongAM)
	{
		PR_TRACE(( g_root, prtERROR, TR "CHipsManager::GetPermissionResult WRONG AM=%x", (HIPS_GET_AM(AccessMask, HIPS_POS_PERMIS))));
	}
	return ResVerdict; 
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
CHipsAppItem * CHipsManager::GetAppByPid(PVOID l_pContext, DWORD l_PID)
{
	if (l_pContext == 0)
	{
		PR_TRACE(( g_root, prtERROR, TR "CHipsManager::GetAppByPid Wrong param !!!"));
		return 0;
	}
	BYTE ProcHash [HIPS_HASH_SIZE];
	CHipsAppItem * pApp = 0;
	HRESULT hResult = MKL_QueryProcessHash(
							l_pContext,
							l_PID, &ProcHash[0], HIPS_HASH_SIZE);
	if (SUCCEEDED(hResult))
	{
		if (PR_FAIL(m_HipsRuleManager.FindAppByHash(&ProcHash[0], HIPS_HASH_SIZE, &pApp)))
		{
			PR_TRACE((g_root, prtIMPORTANT, TR "CHipsManager::GetAppByPid(%d) FAIL !!!", l_PID));
			return 0;
		}
	}
	else
	{
		PR_TRACE(( g_root, prtERROR, TR "MKL_QueryProcessHash(pid = %d) return (hResult = %x)", l_PID, hResult));
	}
	return pApp;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
tERROR pr_call OnThreadInitCallback (
	tThreadCallType CallType,
	tPTR pCommonThreadContext,
	tPTR* ppThreadContext
	)
{
#ifdef HIPS_DEBUG_WITHOUT_DRV
	return errOK;
#endif

	tERROR err_ret = errOK;

	CHipsTask * pTask = (CHipsTask*)pCommonThreadContext;
	CHipsManager * pHipsManager = pTask->GetHipsManager();/*(CHipsManager*)lpParameter*/;

	switch (CallType)
	{
	case TP_CBTYPE_THREAD_INIT_CONTEXT:
		PR_TRACE(( g_root, prtERROR, TR "OnThreadInitCallback (pHipsManager = %x), pHipsManager->m_pClientContext = %x", pHipsManager, (pHipsManager) ? pHipsManager->m_pClientContext : 0));
		if (pHipsManager && pHipsManager->m_pClientContext)
		{
			HRESULT hResult = MKL_AddInvisibleThread( pHipsManager->m_pClientContext );
			if (!SUCCEEDED( hResult ))
			{
				PR_TRACE(( g_root, prtERROR, TR "new thread - result 0x%x. pHipsManager 0x%x, client context 0x%x", 
					hResult, pHipsManager, pHipsManager->m_pClientContext ));
			}
		}
		break;
	
	case TP_CBTYPE_THREAD_DONE_CONTEXT:
		//PR_TRACE(( g_root, prtERROR, TR "exit thread" ));
		break;
	
	case TP_CBTYPE_THREAD_YIELD:
		break;
	}

	//PR_TRACE(( g_root, prtERROR, TR "OnThreadInitCallback 7"));
	return err_ret;
}
//////////////////////////////////////////////////////////////////////////
//	callback threadpool function (ask events)
//////////////////////////////////////////////////////////////////////////
tERROR pr_call ThreadInfoCallback (
	tThreadCallType CallType,
	tPTR pThreadContext,
	tPTR* ppTaskContext,
	tPTR pTaskData,
	tDWORD TaskDataLen
	)
{
#ifdef HIPS_DEBUG_WITHOUT_DRV
	return errOK;
#endif
//	PR_TRACE(( g_root, prtERROR, TR "ThreadInfoCallback:: start (CallType = %d, pTaskData = %x)and return", CallType, pTaskData));

	if (CallType == TP_CBTYPE_TASK_PROCESS)
	{
		//Sleep(100);
		PR_TRACE(( g_root, prtIMPORTANT, TR "HIPS:: ThreadInfoCallback:: TP_CBTYPE_TASK_PROCESS"));
		if (pTaskData == 0)
		{
			PR_TRACE(( g_root, prtERROR, TR "HIPS:: ThreadInfoCallback:: pTaskData == 0"));
			return errNOT_OK;
		}
		MKLIF_REPLY_EVENT ResVerdict;
		memset( &ResVerdict, 0, sizeof(ResVerdict) );
		ResVerdict.m_VerdictFlags = efVerdict_Allow;
		ResVerdict.m_ExpTime = 2;
		
		CallBackHipsStruct * pStr = *(CallBackHipsStruct**)pTaskData;

		if (pStr == 0)
		{
			PR_TRACE(( g_root, prtERROR, TR "HIPS:: ThreadInfoCallback:: CallBackAskStruct* == 0"));
			return errNOT_OK;
		}

		PR_TRACE(( g_root, prtIMPORTANT, TR "HIPS::ThreadInfoCallback:: Str->m_pContext = %x, Str->m_pMessage = %x, Str->m_pHipsManager = %x, pStr->m_Action = %d, pStr->m_Pid = %d, pStr->m_RuleID = %d, pStr->m_pObjectName = %S",
			pStr->m_pContext,
			pStr->m_pMessage,
			pStr->m_pHipsManager,
			pStr->m_Action,
			pStr->m_SrcPID,
			pStr->m_RuleID,
			pStr->m_ObjectName1));

		cResource * pResource = 0;
		DWORD ResID = pStr->m_pHipsManager->m_HipsRuleManager.GetResIDByDrvRuleID(pStr->m_RuleID);
		if (ResID != 0)
		{
			pResource = pStr->m_pHipsManager->m_HipsRuleManager.m_Settings.m_Resource.Find(ResID);//FindResByID(0, ResID);
			PR_TRACE(( g_root, prtERROR, TR "pResource = %x", pResource));
		}

		if (pResource && 
			pStr->m_pHipsManager->m_LocalCash.IsInAnswerCash(pStr->m_SrcPID, pResource->m_nID, &ResVerdict.m_VerdictFlags))
		{
			//	there is cashed answer, no need to ask again
			PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadInfoCallback:: GetUserAskAction cashed answer finded (pid = %d, res = %x, value = %d)",
				pStr->m_SrcPID, pResource->m_nID, ResVerdict.m_VerdictFlags));
		}
		else
		{
			//	no cashed answer
			CHipsAppItem * pSrcApp = pStr->m_pHipsManager->GetAppByPid(pStr->m_pContext, pStr->m_SrcPID);
			ResVerdict.m_VerdictFlags = pStr->m_pHipsManager->GetUserAskAction(pSrcApp, pStr, pResource);
			PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadInfoCallback:: GetUserAskAction return m_VerdictFlags = %d)", ResVerdict.m_VerdictFlags));
		}


		PR_TRACE(( g_root, prtNOT_IMPORTANT, TR "HIPS::ThreadInfoCallback:: before Set result verdict "));
		HRESULT hResult = MKL_SetVerdict( pStr->m_pContext , pStr->m_pMessage, &ResVerdict );
		if (!SUCCEEDED(hResult))
		{
			PR_TRACE((g_root, prtERROR, TR "HIPS::MKL_SetVerdict return error = %x!\n", hResult));
		}
		PR_TRACE(( g_root, prtNOT_IMPORTANT, TR "HIPS::ThreadInfoCallback:: after Set result verdict"));
		pfMemFree(NULL, &pStr->m_pMessage);
		if (pStr)
		{
			PR_TRACE(( g_root, prtIMPORTANT, TR "HIPS::ThreadInfoCallback:: before delete pStr(%x)", pStr));
			delete pStr;
		}
	}

	return errOK;
}
//////////////////////////////////////////////////////////////////////////
//	callback threadpool function (ProcNotify events)
//////////////////////////////////////////////////////////////////////////
tERROR pr_call ThreadProcNotifyCallback (
	tThreadCallType CallType,
	tPTR pThreadContext,
	tPTR* ppTaskContext,
	tPTR pTaskData,
	tDWORD TaskDataLen
	)
{
#ifdef HIPS_DEBUG_WITHOUT_DRV
	return errOK;
#endif
	if (CallType == TP_CBTYPE_TASK_PROCESS)
	{
		PR_TRACE(( g_root, prtIMPORTANT, TR "HIPS:: ThreadProcNotifyCallback:: TP_CBTYPE_TASK_PROCESS"));
		if (pTaskData == 0)
		{
			PR_TRACE(( g_root, prtERROR, TR "HIPS:: ThreadProcNotifyCallback:: pTaskData == 0"));
			return errNOT_OK;
		}
		MKLIF_REPLY_EVENT ResVerdict;
		memset( &ResVerdict, 0, sizeof(ResVerdict) );
		ResVerdict.m_VerdictFlags = efVerdict_Allow;
		ResVerdict.m_ExpTime = 2;
		
		CallBackHipsStruct * pStr = *(CallBackHipsStruct**)pTaskData;

		if (pStr == 0)
		{
			PR_TRACE(( g_root, prtERROR, TR "HIPS:: ThreadProcNotifyCallback:: CallBackProcNotifyStruct* == 0"));
			return errNOT_OK;
		}

		CHipsAppItem * pApp = 0;	//	pointer to App in m_HipsSettings

		PR_TRACE(( g_root, prtIMPORTANT, TR "HIPS::ThreadProcNotifyCallback:: SrcPID = %d, Context = %x, pMessage = %x, pHipsManager = %x, pProcName= %S",
			pStr->m_SrcPID,
			pStr->m_pContext,
			pStr->m_pMessage,
			pStr->m_pHipsManager,
			pStr->m_SrcProcName));

		////	add to proc cash
		//pStr->m_pHipsManager->m_LocalCash.AddNewProcess(pStr->m_SrcPID);


		if (pStr->m_pHipsManager->CheckRuleForApp(pStr->m_SrcProcName, pStr->m_ProcHash, HIPS_HASH_SIZE, &pApp))
		{
			//	need to update GUI
			PR_TRACE((g_root, prtERROR, TR "HIPS::ThreadProcNotifyCallback:: IsNeedToReloadRule == true, sysSendMsg(pmc_TASK_SETTINGS_CHANGED)"));
			pStr->m_pHipsTask->sysSendMsg(pmc_TASK_SETTINGS_CHANGED, 0, NULL, NULL, NULL);
			//	need to update internal lists
			HipsRulesList tempHipsRulesList;
			PR_TRACE((g_root, prtERROR, TR "HIPS::ThreadProcNotifyCallback:: before AddRulesForAppToList"));
			pStr->m_pHipsManager->m_HipsRuleManager.AddRulesForAppToList(pApp, &tempHipsRulesList);
			PR_TRACE((g_root, prtERROR, TR "HIPS::ThreadProcNotifyCallback:: after AddRulesForAppToList (size = %d)", tempHipsRulesList.size()));
			//	need to upload rule to drive for this app
			PR_TRACE((g_root, prtERROR, TR "HIPS::ThreadProcNotifyCallback:: before ReloadRulesInDrv"));
			pStr->m_pHipsTask->ReloadRulesInDrv(&tempHipsRulesList);
			PR_TRACE((g_root, prtERROR, TR "HIPS::ThreadProcNotifyCallback:: after ReloadRulesInDrv"));
			HipsRulesListIt It = tempHipsRulesList.begin();
			while(It != tempHipsRulesList.end())
			{
				if (*It)
				{
					pStr->m_pHipsManager->m_HipsRuleManager.m_HipsRulesList.push_back(*It);
				}
				It++;
			};
			tempHipsRulesList.clear();
		}

		PR_TRACE(( g_root, prtIMPORTANT, TR "HIPS::ThreadProcNotifyCallback:: before Set result verdict "));
		HRESULT hResult = MKL_SetVerdict( pStr->m_pContext , pStr->m_pMessage, &ResVerdict );
		if (!SUCCEEDED(hResult))
		{
			PR_TRACE((g_root, prtERROR, TR "HIPS::ThreadProcNotifyCallback:: MKL_SetVerdict return error = %x!\n", hResult));
		}
		PR_TRACE(( g_root, prtIMPORTANT, TR "HIPS::ThreadProcNotifyCallback:: after Set result verdict"));
		if (pStr->m_pMessage)
		{
			pfMemFree(NULL, &pStr->m_pMessage);
		}
		if (pStr)
		{
			PR_TRACE(( g_root, prtIMPORTANT, TR "HIPS::ThreadProcNotifyCallback:: before delete pStr(%x)", pStr));
			delete pStr;
		}
	}

	return errOK;
}
//////////////////////////////////////////////////////////////////////////
//	callback threadpool function (log events)
//////////////////////////////////////////////////////////////////////////
tERROR pr_call ThreadLogCallback (
	tThreadCallType CallType,
	tPTR pThreadContext,
	tPTR* ppTaskContext,
	tPTR pTaskData,
	tDWORD TaskDataLen
	)
{
#ifdef HIPS_DEBUG_WITHOUT_DRV
	return errOK;
#endif
	if (CallType == TP_CBTYPE_TASK_PROCESS)
	{
		//PR_TRACE(( g_root, prtIMPORTANT, TR "HIPS:: ThreadLogCallback:: TP_CBTYPE_TASK_PROCESS"));
		if (pTaskData == 0)
		{
			PR_TRACE(( g_root, prtERROR, TR "HIPS:: ThreadLogCallback:: pTaskData == 0"));
			return errNOT_OK;
		}

		MKLIF_REPLY_EVENT ResVerdict;
		memset( &ResVerdict, 0, sizeof(ResVerdict) );
		ResVerdict.m_VerdictFlags = efVerdict_Allow;
		ResVerdict.m_ExpTime = 2;

		CallBackHipsStruct * pStr = *(CallBackHipsStruct**)pTaskData;

		if (pStr == 0)
		{
			PR_TRACE(( g_root, prtERROR, TR "HIPS:: ThreadLogCallback:: CallBackLogStruct* == 0"));
			return errNOT_OK;
		}

		//PR_TRACE(( g_root, prtIMPORTANT, TR "HIPS::ThreadLogCallback:: Str->m_pContext = %x, Str->m_pMessage = %x, Str->m_pHipsManager = %x, pStr->m_ProcName= %S",
		//	pStr->m_pContext,
		//	pStr->m_pMessage,
		//	pStr->m_pHipsManager/*,
		//	pStr->m_SrcProcName*/));

		//	set verdict
		//PR_TRACE(( g_root, prtNOT_IMPORTANT, TR "HIPS::ThreadLogCallback:: before Set result verdict "));
		HRESULT hResult = MKL_SetVerdict( pStr->m_pContext , pStr->m_pMessage, &ResVerdict );
		if (!SUCCEEDED(hResult))
		{
			PR_TRACE((g_root, prtERROR, TR "HIPS::ThreadLogCallback:: MKL_SetVerdict return error = %x!\n", hResult));
		}
		//PR_TRACE(( g_root, prtNOT_IMPORTANT, TR "HIPS::ThreadLogCallback:: after Set result verdict"));


		cResource * pResource = 0;
		DWORD ResID = pStr->m_pHipsManager->m_HipsRuleManager.GetResIDByDrvRuleID(pStr->m_RuleID);
		if (ResID != 0)
		{
			pResource = pStr->m_pHipsManager->m_HipsRuleManager.m_Settings.m_Resource.Find(ResID);//FindResByID(0, ResID);
			PR_TRACE(( g_root, prtIMPORTANT, TR "pResource = %x", pResource));
		}
		else
		{
			PR_TRACE(( g_root, prtERROR, TR "resource in drv rule 0x%x was not found", pStr->m_RuleID));
		}

		pStr->m_pHipsManager->WriteToLog(pStr, pResource);

					//pStr->m_RuleID,
					//pStr->m_SrcProcName,
					//pStr->m_SrcPID,
					//pStr->m_Action,// (pDesiredOperation) ? (*(DWORD*)pDesiredOperation->ParamValue) : 0,
					//pStr->m_Result,// (pParamResult) ? (*(DWORD*)pParamResult->ParamValue) : 0,
					//pStr->m_ObjectName1,
					//pStr->m_ObjectName2
					//);

		if (pStr->m_pMessage)
		{
			pfMemFree(NULL, &pStr->m_pMessage);
		}
		if (pStr)
		{
			//PR_TRACE(( g_root, prtIMPORTANT, TR "HIPS::ThreadLogCallback:: before delete pStr(%x)", pStr));
			delete pStr;
		}
	}
	return errOK;
}
//////////////////////////////////////////////////////////////////////////
//	callback threadpool function (other events)
//////////////////////////////////////////////////////////////////////////
tERROR pr_call ThreadOtherEvCallback (
	tThreadCallType CallType,
	tPTR pThreadContext,
	tPTR* ppTaskContext,
	tPTR pTaskData,
	tDWORD TaskDataLen
	)
{
#ifdef HIPS_DEBUG_WITHOUT_DRV
	return errOK;
#endif
	if (CallType == TP_CBTYPE_TASK_PROCESS)
	{
		//PR_TRACE(( g_root, prtIMPORTANT, TR "HIPS:: ThreadOtherEvCallback:: TP_CBTYPE_TASK_PROCESS"));
		if (pTaskData == 0)
		{
			PR_TRACE(( g_root, prtERROR, TR "HIPS:: ThreadOtherEvCallback:: pTaskData == 0"));
			return errNOT_OK;
		}

		MKLIF_REPLY_EVENT ResVerdict;
		memset( &ResVerdict, 0, sizeof(ResVerdict) );
		ResVerdict.m_VerdictFlags = efVerdict_Allow;
		ResVerdict.m_ExpTime = 2;

		CallBackHipsStruct * pStr = *(CallBackHipsStruct**)pTaskData;

		if (pStr == 0)
		{
			PR_TRACE(( g_root, prtERROR, TR "HIPS:: ThreadOtherEvCallback:: CallBackOtherEvStruct* == 0"));
			return errNOT_OK;
		}

		//PR_TRACE(( g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: Str->m_pContext = %x, Str->m_pMessage = %x, Str->m_pHipsManager = %x, pStr->m_FunctionMj = %x, pStr->m_FunctionMi = %x",
		//	pStr->m_pContext,
		//	pStr->m_pMessage,
		//	pStr->m_pHipsManager,
		//	pStr->m_FunctionMj,
		//	pStr->m_FunctionMi));

		//	check in cash
		bool IsCashable = true;
		cPrivilegeInfo Privelege;
		cResource * pResource;
		switch (pStr->m_FunctionMj)
		{
			case MJ_SYSTEM_WRITE_PROCESS_MEMORY:
			{
				Privelege.m_nType = cPrivilegeInfo::ehptProcStart; break;
			}
			case MJ_SYSTEM_USER_SET_WINDOWS_HOOK_EX:
			{
				Privelege.m_nType = cPrivilegeInfo::ehptSetHook; break;
			}
			case MJ_SYSTEM_CREATE_REMOTE_THREAD:
			{
				Privelege.m_nType = cPrivilegeInfo::ehptCodeInject/*ehptCreateRemThread*/; break;
			}
			case MJ_SYSTEM_SHUTDOWN:
			{
				Privelege.m_nType = cPrivilegeInfo::ehptWindowsShutDown; break;
			}
			case MJ_SYSTEM_KEYLOGGER_DETECTED:
			{
				Privelege.m_nType = cPrivilegeInfo::ehptKeyLogger/*ehptKeyLoggerAsyncKS*/; break;
			}
			case MJ_SYSTEM_OPEN_PROCESS:
			{
				Privelege.m_nType = cPrivilegeInfo::ehptProcStop; break;
			}
			case MJ_SYSTEM_SET_THREAD_CONTEXT:
			{
				Privelege.m_nType = cPrivilegeInfo::ehptCodeInject/*ehptSetThreadContext*/; break;
			}
			case IRP_MJ_SET_INFORMATION:
			{
				Privelege.m_nType = cPrivilegeInfo::ehptSetHardLink; break;
			}
			
			default:
			{
				IsCashable = false;
				break;
			}
		};

		if ((IsCashable) &&
			(pResource = pStr->m_pHipsManager->m_HipsRuleManager.m_Settings.m_Resource.FindResByContent(&Privelege)) && 
			(pStr->m_pHipsManager->m_LocalCash.IsInAnswerCash(pStr->m_SrcPID, pResource->m_nID, &ResVerdict.m_VerdictFlags))
			)
		{
			//	there is cashed answer, no need to ask again
			PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::ThreadOtherEvCallback:: cashed answer finded (pid = %d, res = %x, value = %d)",
				pStr->m_SrcPID, pResource->m_nID, ResVerdict.m_VerdictFlags));
		}
		else
		{
			//	no in cash or something wrong

			switch (pStr->m_FunctionMj)
			{
				case MJ_SYSTEM_KLFLTDEV:
				{
					//	plug devise
					PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::ThreadOtherEvCallback:: MJ_SYSTEM_KLFLTDEV"));
					PSINGLE_PARAM pSPGuid = MKL_GetEventParam( pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_URL_W, FALSE );
					PSINGLE_PARAM pSPDeviceType = MKL_GetEventParam( pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_VOLUME_NAME_W, FALSE );
					PSINGLE_PARAM pSPAccessMask = MKL_GetEventParam( pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_ACCESSATTR, FALSE );

					wchar_t * pGUID = 0;
					wchar_t * pType = 0;
					DWORD AM = 0;
					if (pSPGuid && pSPDeviceType && pSPAccessMask)
					{
						pGUID = (pSPGuid) ? ((wchar_t*)(pSPGuid->ParamValue)) : 0;
						pType = (pSPDeviceType) ? (wchar_t*)pSPDeviceType->ParamValue : 0;
						AM = (pSPAccessMask) ? *(DWORD*)pSPAccessMask->ParamValue : 0;
						PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: pGUID = %S, pType = %S, AM = %x",
								pGUID, pType, AM));
						pStr->m_pHipsManager->WriteToLog(pGUID, pType, AM);
					}
					break;
				}

				case MJ_EXIT_PROCESS:	//	post event
				{
					//	plug devise
					PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: MJ_EXIT_PROCESS (%d)", pStr->m_SrcPID));
					//	remove PID from local cash
					pStr->m_pHipsManager->m_LocalCash.DelPID(pStr->m_SrcPID);
					break;
				}

				case MJ_SYSTEM_WRITE_PROCESS_MEMORY:	//	== start process
				{
					//	write to memory
					PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::ThreadOtherEvCallback:: MJ_SYSTEM_WRITE_PROCESS_MEMORY"));

					////	check for system process
					//PSINGLE_PARAM pParamLuid = MKL_GetEventParam(pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_LUID, FALSE);
					//if (pParamLuid && (sizeof(LUID) == pParamLuid->ParamSize))
					//{
					//	PLUID pLuid = (PLUID) pParamLuid->ParamValue;
					//	LUID SystemLuid = SYSTEM_LUID;
					//	if (!memcmp( &SystemLuid, pLuid, sizeof(LUID) ))
					//	{
					//		PR_TRACE((g_root, prtNOTIFY, TR "wrm: system luid"));
					//	}
					//	else
					//	{
					//		PR_TRACE((g_root, prtNOTIFY, TR "wrm: not system luid"));
					//	}
					//}
					//else
					//{
					//	PR_TRACE((g_root, prtERROR, TR "HIPS::ThreadOtherEvCallback:: pParamLuid = %x", pParamLuid));
					//}

					PSINGLE_PARAM pParamProcId = MKL_GetEventParam(pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_DEST_ID, FALSE);
					PSINGLE_PARAM pParamOffset = MKL_GetEventParam(pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_BYTEOFFSET, FALSE);
					PSINGLE_PARAM pParamData = MKL_GetEventParam(pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_BINARYDATA, FALSE);
					//PSINGLE_PARAM pParamDataSize = MKL_GetEventParam(pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_DATALEN, FALSE);
					

					ULONG Offset = 0;
					ULONG DestProcId = 0;

					if (pParamOffset)
					{
						Offset = *(ULONG*)pParamOffset->ParamValue;
						if (Offset == 0x20000)
						{
							PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: Offset = %x", Offset));
							if (pParamProcId)
							{
								DestProcId = *(ULONG*)pParamProcId->ParamValue;
								PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: m_ProcessId(%d) write to DestProcId(%d)",
									pStr->m_SrcPID,  DestProcId));

								//	add to proc cash
								pStr->m_pHipsManager->m_LocalCash.AddNewProcess(DestProcId);


								CHipsAppItem * pSrcApp = pStr->m_pHipsManager->GetAppByPid(pStr->m_pContext, pStr->m_SrcPID);
								CHipsAppItem * pDestApp = pStr->m_pHipsManager->GetAppByPid(pStr->m_pContext, DestProcId);

								wchar_t * CmdLine = 0;
								if (pParamData)
								{
									//DebugBreak();
									impPROCESS_PARAMETERS* pProcessParams = (impPROCESS_PARAMETERS*) pParamData->ParamValue;
									PUNICODE_STRING puniCommandLine = &pProcessParams->CommandLine;
									
									PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: Start process - making environment block!"));

									if ((ULONG) puniCommandLine->Buffer < 0x1000)
									{
										BYTE* ptr = pParamData->ParamValue;
										ptr = ptr + (DWORD) puniCommandLine->Buffer;
										puniCommandLine->Buffer = (PWSTR) ptr;
									}
									if (((BYTE*)puniCommandLine->Buffer) + puniCommandLine->Length > ((BYTE*)pProcessParams) + pParamData->ParamSize)
									{
										PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: Process command line bad pointer"));
										puniCommandLine = NULL;
									}

									if (puniCommandLine)
									{
										PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: CommandLine = %S",
											puniCommandLine->Buffer ));
										CmdLine = puniCommandLine->Buffer;
									}
								}

								ResVerdict.m_VerdictFlags = pStr->m_pHipsManager->GetPermissionResult(pStr->m_SrcPID, pSrcApp, pDestApp, CmdLine, cPrivilegeInfo::ehptProcStart);
								PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: result m_VerdictFlags = %d", ResVerdict.m_VerdictFlags));
							}
						}//offset 0x20000
					}//pParamOffset
					else
					{
						PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: pParamOffset = 0"));
					}
					break;
				}//MJ_SYSTEM_WRITE_PROCESS_MEMORY
				case MJ_SYSTEM_TERMINATE_PROCESS:
				{
					//	
					PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: MJ_SYSTEM_WRITE_PROCESS_MEMORY"));
					break;
				}//MJ_SYSTEM_TERMINATE_PROCESS
				case MJ_SYSTEM_USER_SET_WINDOWS_HOOK_EX:
				{
					PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: MJ_SYSTEM_USER_SET_WINDOWS_HOOK_EX"));
					PSINGLE_PARAM pParamHookID = MKL_GetEventParam(pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_CLIENTID1, FALSE);
					PSINGLE_PARAM pParamThreadID = MKL_GetEventParam(pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_CLIENTID2, FALSE);
					PSINGLE_PARAM pParamDllName = MKL_GetEventParam(pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_URL_W, FALSE);
					DWORD ThreadID = 0;
					DWORD HookId = 0;
					wchar_t * DllName = 0; 
					if (pParamHookID)
					{
						HookId = *(ULONG*)pParamHookID->ParamValue;
						PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::ThreadOtherEvCallback:: HookId = %d", HookId));
						//WH_KEYBOARD
					}
					if (pParamThreadID)
					{
						ThreadID = *(ULONG*)pParamThreadID->ParamValue;
						PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::ThreadOtherEvCallback:: ThreadID = %x", ThreadID));
					}
					if (pParamDllName)
					{
						DllName = (wchar_t*)pParamDllName->ParamValue;
						PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::ThreadOtherEvCallback:: DllName = %S", DllName));
						cProcMonNotifyEx Rating;
						tERROR err = errOK;
						if (errOK == (err = pStr->m_pHipsTask->m_pProcMonObj->GetFileInfoEx(DllName, &Rating.m_SecRating)))
						{
							if (Rating.m_SecRating.m_Signature.m_nTrustFlags & (TF_SIGNED | TF_TRUSTED))
							{
								PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: Embedded dll(%S) is signed and trusted, skip this hook",
									DllName));
								break;
							}
							else
							{
								PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: Embedded dll(%S) not signed or not trusted (flags = 0x%x)",
									DllName,
									Rating.m_SecRating.m_Signature.m_nTrustFlags));
							}
						}
						else
						{
							PR_TRACE((g_root, prtERROR, TR "HIPS::ThreadOtherEvCallback:: m_pProcMonObj->GetFileInfoEx FAIL = 0x%x", err));
						}
					}

					CHipsAppItem * pSrcApp = pStr->m_pHipsManager->GetAppByPid(pStr->m_pContext, pStr->m_SrcPID);
					//CHipsAppItem * pDestApp = pStr->m_pHipsManager->GetAppByPid(pStr->m_pContext, DestPID);

					ResVerdict.m_VerdictFlags = pStr->m_pHipsManager->GetPermissionResult(pStr->m_SrcPID, pSrcApp, 0, DllName, cPrivilegeInfo::ehptSetHook);
					PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: result m_VerdictFlags = %d", ResVerdict.m_VerdictFlags));
					break;
				}//MJ_SYSTEM_USER_SET_WINDOWS_HOOK_EX
				case MJ_SYSTEM_CREATE_REMOTE_THREAD:
				{
					PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: MJ_SYSTEM_CREATE_REMOTE_THREAD"));
					PSINGLE_PARAM pParamDestPID = MKL_GetEventParam(pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_DEST_ID, FALSE);
					if (pParamDestPID)
					{
						DWORD DestPID = *(ULONG*)pParamDestPID->ParamValue;
						PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: Process (%d) create remote thread in (%d)",
							pStr->m_SrcPID, DestPID));
						if (!pStr->m_pHipsManager->m_LocalCash.IsFirstThreadStarted(DestPID))
						{
							//	no were first thread
							pStr->m_pHipsManager->m_LocalCash.SetFirstThreadStarted(DestPID, true);
							PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: creation of first remote thread in (%d), allow",
								DestPID));
							break;
						}
						CHipsAppItem * pSrcApp = pStr->m_pHipsManager->GetAppByPid(pStr->m_pContext, pStr->m_SrcPID);
						CHipsAppItem * pDestApp = pStr->m_pHipsManager->GetAppByPid(pStr->m_pContext, DestPID);

						ResVerdict.m_VerdictFlags = pStr->m_pHipsManager->GetPermissionResult(pStr->m_SrcPID, pSrcApp, pDestApp, 0, cPrivilegeInfo::ehptCodeInject/*ehptCreateRemThread*/);
						PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: result m_VerdictFlags = %d", ResVerdict.m_VerdictFlags));
					}
					else
					{
						PR_TRACE((g_root, prtERROR, TR "HIPS::ThreadOtherEvCallback:: pParamDestPID = %d", pParamDestPID));
					}
					break;
				}//MJ_SYSTEM_CREATE_REMOTE_THREAD

				case MJ_SYSTEM_SHUTDOWN:
				{
					PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: MJ_SYSTEM_SHUTDOWN"));
					PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: Process (%d) try to shutdown system",
						pStr->m_SrcPID));
					CHipsAppItem * pSrcApp = pStr->m_pHipsManager->GetAppByPid(pStr->m_pContext, pStr->m_SrcPID);

					ResVerdict.m_VerdictFlags = pStr->m_pHipsManager->GetPermissionResult(pStr->m_SrcPID, pSrcApp, 0, 0, cPrivilegeInfo::ehptWindowsShutDown);
					PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: result m_VerdictFlags = %d", ResVerdict.m_VerdictFlags));
					break;
				}
				
				case MJ_SYSTEM_KEYLOGGER_DETECTED:
				{
//#define KEYLOGGER_GETASYNCKEYSTATE			0 //valid pStr->m_SrcPID
//#define KEYLOGGER_GETMESSAGE_PATCH			1
//#define KEYLOGGER_PEEKMESSAGE_PATCH			2
//#define KEYLOGGER_ATTACHKBD					3
//#define KEYLOGGER_SPLICEONREADDISPATCH		4
					PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: MJ_SYSTEM_KEYLOGGER_DETECTED"));

					PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: pStr->m_SrcPID = %d", 
						pStr->m_SrcPID));

					PSINGLE_PARAM pParamDriverName = MKL_GetEventParam(pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_URL_W, FALSE);
					if (pParamDriverName)
					{
						PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: _PARAM_OBJECT_URL_W = %S", 
						(wchar_t*)pParamDriverName->ParamValue));
					}

					PSINGLE_PARAM pParamSrcID = MKL_GetEventParam(pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_SOURCE_ID, FALSE);
					if (pParamSrcID)
					{
						PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: _PARAM_OBJECT_SOURCE_ID = %d", 
						*(ULONG*)pParamSrcID->ParamValue));
					}

					PSINGLE_PARAM pParamKeyLogID = MKL_GetEventParam(pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_KEYLOGGERID, FALSE);
					DWORD KeyLogID = 0;
					if (pParamKeyLogID)
					{
						KeyLogID = *(ULONG*)pParamKeyLogID->ParamValue;
						PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: _PARAM_OBJECT_KEYLOGGERID = %d", 
						KeyLogID));
						if (KeyLogID == KEYLOGGER_GETASYNCKEYSTATE)
						{
							CHipsAppItem * pSrcApp = pStr->m_pHipsManager->GetAppByPid(pStr->m_pContext, pStr->m_SrcPID);

							ResVerdict.m_VerdictFlags = pStr->m_pHipsManager->GetPermissionResult(pStr->m_SrcPID, pSrcApp, 0, 0, cPrivilegeInfo::ehptKeyLogger/*AsyncKS*/);
							PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: result m_VerdictFlags = %d", ResVerdict.m_VerdictFlags));
						}
					}
					else
					{
						PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: pParamKeyLogID == 0"));
					}

					
					break;
				}
				case MJ_SYSTEM_SET_THREAD_CONTEXT:
				{
					PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: MJ_SYSTEM_SET_THREAD_CONTEXT"));
					PSINGLE_PARAM pParamDestPID = MKL_GetEventParam(pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_CLIENTID1, FALSE);
					if (pParamDestPID)
					{
						DWORD DestPID = *(ULONG*)pParamDestPID->ParamValue;
						PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: Process (%d) set thread context in process (%d)",
							pStr->m_SrcPID, DestPID));
						CHipsAppItem * pSrcApp = pStr->m_pHipsManager->GetAppByPid(pStr->m_pContext, pStr->m_SrcPID);
						CHipsAppItem * pDestApp = pStr->m_pHipsManager->GetAppByPid(pStr->m_pContext, DestPID);

						ResVerdict.m_VerdictFlags = pStr->m_pHipsManager->GetPermissionResult(pStr->m_SrcPID, pSrcApp, pDestApp, 0, cPrivilegeInfo::ehptCodeInject/*ehptSetThreadContext*/);
						PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: result m_VerdictFlags = %d", ResVerdict.m_VerdictFlags));
					}
					else
					{
						PR_TRACE((g_root, prtERROR, TR "HIPS::ThreadOtherEvCallback:: pParamDestPID = %d", pParamDestPID));
					}
					break;
				}
				case MJ_SYSTEM_OPEN_PROCESS:	//	== terminate process
				{
					PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::ThreadOtherEvCallback:: MJ_SYSTEM_OPEN_PROCESS"));
					PSINGLE_PARAM pParamDestProcId = MKL_GetEventParam(pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_DEST_ID, FALSE);
					PSINGLE_PARAM pParamFlags = MKL_GetEventParam(pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_ACCESSATTR, FALSE);
					DWORD DestPID = 0;
					DWORD Flags = 0;
					if (pParamDestProcId && pParamFlags)
					{
						DestPID = *(ULONG*)pParamDestProcId->ParamValue;
						Flags = *(ULONG*)pParamFlags->ParamValue;
						PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS::ThreadOtherEvCallback:: process(%d) open (%d) for (%x)",
							pStr->m_SrcPID, DestPID, Flags));
						if (Flags & PROCESS_TERMINATE)
						{
							//	open for terminate
							PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: process(%d) open (%d) for terminate (%x)",
								pStr->m_SrcPID, DestPID, Flags));
							CHipsAppItem * pSrcApp = pStr->m_pHipsManager->GetAppByPid(pStr->m_pContext, pStr->m_SrcPID);
							CHipsAppItem * pDestApp = pStr->m_pHipsManager->GetAppByPid(pStr->m_pContext, DestPID);

							ResVerdict.m_VerdictFlags = pStr->m_pHipsManager->GetPermissionResult(pStr->m_SrcPID, pSrcApp, pDestApp, 0, cPrivilegeInfo::ehptProcStop);						
							PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: result m_VerdictFlags = %d", ResVerdict.m_VerdictFlags));
						}
					}
					else
					{
						PR_TRACE((g_root, prtERROR, TR "HIPS::ThreadOtherEvCallback:: can`t get param !!! (pParamDestProcId = %x, pParamFlags = %x)",
							pParamDestProcId, pParamFlags));
					}
					break;
				}//MJ_SYSTEM_OPEN_PROCESS
				case IRP_MJ_SET_INFORMATION:
				{
					PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: IRP_MJ_SET_INFORMATION"));
					//_PARAM_OBJECT_VOLUME_NAME_W, _PARAM_OBJECT_VOLUME_NAME_DEST_W

					PSINGLE_PARAM pParamSrcName = MKL_GetEventParam(pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_URL_W, FALSE);
					PSINGLE_PARAM pParamDestName = MKL_GetEventParam(pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_URL_DEST_W, FALSE);
					if (pParamSrcName && pParamDestName)
					{
						PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: process (%d) set hardlink from %S to %S",
							pStr->m_SrcPID,
							(wchar_t*)pParamSrcName->ParamValue,
							(wchar_t*)pParamDestName->ParamValue));
						CHipsAppItem * pSrcApp = pStr->m_pHipsManager->GetAppByPid(pStr->m_pContext, pStr->m_SrcPID);

						PWCHAR pwchFileName = 0;
						ULONG FileNameLen = 0;
						cStringObj SrcStr; //cStringObj DstStr;
						if (((CHipsTask*)pStr->m_pHipsTask)->MakeFileName( 
											pStr->m_pContext,
											pStr->m_pMessage,
											pStr->m_MessageSize,
											pStr->m_ContextFlags,
											&pwchFileName, &FileNameLen, false ))
						{
							SrcStr = "from ";
							SrcStr += pwchFileName;
						}
						if (pwchFileName)
							pfMemFree( NULL, (void**) &pwchFileName );
						if (((CHipsTask*)pStr->m_pHipsTask)->MakeFileName(
											pStr->m_pContext,
											pStr->m_pMessage,
											pStr->m_MessageSize,
											pStr->m_ContextFlags,
											&pwchFileName, &FileNameLen, false ))
						{
							SrcStr += " to ";
							SrcStr += pwchFileName;
						}
						if (pwchFileName)
							pfMemFree( NULL, (void**) &pwchFileName );

						ResVerdict.m_VerdictFlags = pStr->m_pHipsManager->GetPermissionResult(pStr->m_SrcPID, pSrcApp, 0, SrcStr.c_str(cCP_UNICODE), cPrivilegeInfo::ehptSetHardLink);						
						PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: result m_VerdictFlags = %d", ResVerdict.m_VerdictFlags));
					}
					else
					{
						PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: pParamSrcName = %x, pParamDestName = %x",
							pParamSrcName, pParamDestName));
					}
					break;
				}


				
				//case MJ_SYSTEM_CREATE_THREAD:	//	post (all threads)
				//{
				//	PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: MJ_SYSTEM_CREATE_THREAD"));
				//	PSINGLE_PARAM pParamProcId = MKL_GetEventParam(pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_CLIENTID1, FALSE);
				//	PSINGLE_PARAM pParamThreadId = MKL_GetEventParam(pStr->m_pMessage, pStr->m_MessageSize, _PARAM_OBJECT_CLIENTID2, FALSE);
				//	if (pParamProcId && pParamThreadId)
				//	{
				//		DWORD PID = *(ULONG*)pParamProcId->ParamValue;
				//		DWORD TID = *(ULONG*)pParamThreadId->ParamValue;
				//		PR_TRACE((g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: m_SrcPID = %d, PID = %d, TID = %d",
				//			pStr->m_SrcPID, PID, TID));
				//	}
				//	else
				//	{
				//		PR_TRACE((g_root, prtERROR, TR "HIPS::ThreadOtherEvCallback:: pParamProcId = %d, pParamThreadId = %d"));
				//	}

				//	break;
				//}//MJ_SYSTEM_CREATE_THREAD
				default:
				{
					PR_TRACE((g_root, prtERROR, TR "HIPS::ThreadOtherEvCallback:: WRONG m_FunctionMj = %d !!!", pStr->m_FunctionMj));
					break;
				}
			};//switch (pStr->m_FunctionMj)
		}//	no in cash

		//	set verdict
		PR_TRACE(( g_root, prtNOT_IMPORTANT, TR "HIPS::ThreadOtherEvCallback:: before Set result verdict "));
		HRESULT hResult = MKL_SetVerdict( pStr->m_pContext , pStr->m_pMessage, &ResVerdict );
		if (!SUCCEEDED(hResult))
		{
			PR_TRACE((g_root, prtERROR, TR "HIPS::ThreadOtherEvCallback:: MKL_SetVerdict return error = %x!\n", hResult));
		}
		PR_TRACE(( g_root, prtNOT_IMPORTANT, TR "HIPS::ThreadOtherEvCallback:: after Set result verdict"));


		if (pStr->m_pMessage)
		{
			pfMemFree(NULL, &pStr->m_pMessage);
		}
		if (pStr)
		{
			//PR_TRACE(( g_root, prtIMPORTANT, TR "HIPS::ThreadOtherEvCallback:: before delete pStr(%x)", pStr));
			delete pStr;
		}
	}
	return errOK;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CHipsManager::TestEnumAppExclRules()
{
	cExclusion Excl;
	Excl.m_nType = cExclusion::ehexOAS;
	cResource * pResOAS = m_HipsRuleManager.m_Settings.m_Resource.FindResByContent(&Excl);
	Excl.m_nType = cExclusion::ehexPDM;
	cResource * pResPDM = m_HipsRuleManager.m_Settings.m_Resource.FindResByContent(&Excl);
	Excl.m_nType = cExclusion::ehexNetwork;
	cResource * pResNET = m_HipsRuleManager.m_Settings.m_Resource.FindResByContent(&Excl);

	CHipsRuleItem Rule;
	if (pResOAS && pResPDM && pResNET)
	{
		for (DWORD i = 0; i < m_HipsRuleManager.m_Settings.m_vAppList.size(); i++)
		{
			if (PR_SUCC(m_HipsRuleManager.m_Settings.GetRule(
							m_HipsRuleManager.m_Settings.m_vAppList[i].m_AppId,
							pResOAS->m_nID, 0, Rule)))
			{
				PR_TRACE((g_root, prtIMPORTANT, TR"App = 0x%x, Res = 0x%x, AM = 0x%x", 
					Rule.m_AppIdEx, Rule.m_ResIdEx, Rule.m_AccessFlag));
			}
			if (PR_SUCC(m_HipsRuleManager.m_Settings.GetRule(
							m_HipsRuleManager.m_Settings.m_vAppList[i].m_AppId,
							pResPDM->m_nID, 0, Rule)))
			{
				PR_TRACE((g_root, prtIMPORTANT, TR"App = 0x%x, Res = 0x%x, AM = 0x%x", 
					Rule.m_AppIdEx, Rule.m_ResIdEx, Rule.m_AccessFlag));
			}
			if (PR_SUCC(m_HipsRuleManager.m_Settings.GetRule(
							m_HipsRuleManager.m_Settings.m_vAppList[i].m_AppId,
							pResNET->m_nID, 0, Rule)))
			{
				PR_TRACE((g_root, prtIMPORTANT, TR"App = 0x%x, Res = 0x%x, AM = 0x%x", 
					Rule.m_AppIdEx, Rule.m_ResIdEx, Rule.m_AccessFlag));
			}
		}
	}
}
#endif	// _DEBUG
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void CHipsManager::TestFunc()
{
#ifndef _DEBUG
	return;
#endif //_DEBUG
	//WriteToLogAddApp(2716, L"test", 0);
	//TestEnumAppExclRules();
	//if (CheckRuleForAppNew(0))
	//{
	//	((CHipsTask*)m_pTask)->sysSendMsg(pmc_TASK_SETTINGS_CHANGED, 0, NULL, NULL, NULL);
	//}
	//BYTE ProcHash[HIPS_HASH_SIZE];
	//memset(ProcHash, 0, HIPS_HASH_SIZE);
	//								
	//CheckRuleForApp(L"c:\\Program Files\\Kaspersky Lab\\Kaspersky Anti-Virus 8.0\\avp.exe",
	//				&ProcHash[0], HIPS_HASH_SIZE, 0);
	//((CHipsTask*)m_pTask)->sysSendMsg(pmc_TASK_SETTINGS_CHANGED, 0, NULL, NULL, NULL);
	//CheckRuleForApp(L"c:\\Program Files\\Kaspersky Lab\\Kaspersky Anti-Virus 8.0\\avp.exe",
	//				&ProcHash[0], HIPS_HASH_SIZE, 0);
	//((CHipsTask*)m_pTask)->sysSendMsg(pmc_TASK_SETTINGS_CHANGED, 0, NULL, NULL, NULL);
	//cResource * pRes = m_HipsRuleManager.m_Settings.m_Resource.Find(6);
	//cResource * pTempRes = 0;
	//cResourceIterator2 it2(pRes);
	//bool IsNeedToSkip = false;
	//do
	//{
	//	pTempRes = it2.GetNext(IsNeedToSkip);
	//	//
	//} while (pTempRes);
	//m_AnswerCash.Add(1, 1, 1);
	//m_AnswerCash.Add(2, 2, 2);
	//m_AnswerCash.Add(1, 3, 3);
	//m_AnswerCash.Add(2, 4, 4);

	//DWORD Value = 0;
	//if (m_AnswerCash.IsInCash(0, 1, &Value))
	//	PR_TRACE((g_root, prtIMPORTANT, TR"%d", Value));
	//if (m_AnswerCash.IsInCash(1, 2, &Value))
	//	PR_TRACE((g_root, prtIMPORTANT, TR"%d", Value));
	//if (m_AnswerCash.IsInCash(1, 3, &Value))
	//	PR_TRACE((g_root, prtIMPORTANT, TR"%d", Value));
	//if (m_AnswerCash.IsInCash(2, 4, &Value))
	//	PR_TRACE((g_root, prtIMPORTANT, TR"%d", Value));

	//m_AnswerCash.DelPID(1);
	//m_AnswerCash.DelPID(2);

	//if (m_AnswerCash.IsInCash(2, 4, &Value))
	//	PR_TRACE((g_root, prtIMPORTANT, TR"%d", Value));

	//WriteToLog(L"{36fc9e60-c465-11cf-8056-444553540000}", L"USB\\CLASS_08", 0xf);
	//WriteToLog(L"{36fc9e60-c465-11cf-8056-444553540000}", L"USB\\CLASS_09", 0);

	//cDeviceInfo Device;
	//Device.m_sClassGUID = L"{36FC9E60-C465-11CF-8056-444553540000}";
	//Device.m_sTypeName = L"USB\\CLASS_08";
	//cResource * pRes = m_HipsRuleManager.m_Settings.m_Resource.FindResByContent(&Device);

	//cPrivilegeInfo Privilege;
	//Privilege.m_nType = cPrivilegeInfo::ehptProcStart;
	//cResource * pRes = m_HipsRuleManager.m_Settings.m_Resource.FindResByContent(&Privilege);
	//if (pRes)
	//{
	//	AskPermission(1896, 0, 0, pRes);
	//	//CallBackHipsStruct Context;
	//	//Context.m_pHipsManager = this;
	//	//ULONG Result = 0;
	//	//enApplyToAll ToAll;
	//	//if (GetUserAskAction(&Context, pRes, &Result, &ToAll))
	//	//{
	//	//	//
	//	//}
	//}

	//cResourceIterator it (&m_HipsRuleManager.m_Settings.m_Resource);
	//cResource * pRes = 0;
	//bool NeedSkip = false;
	//do
	//{
	//	pRes = it.GetNext(NeedSkip);
	//	if (pRes)
	//	{
	//		PR_TRACE((g_root, prtIMPORTANT, TR"%S",
	//			pRes->m_sName.data()));
	//	}
	//}
	//while (pRes);


	//CHipsRuleItem * pRealRule = 0;
	//CHipsRuleItem VirtRule;

	//////	try to find app in base
	//tERROR err = m_HipsRuleManager.m_Settings.GetRule(
	//						0xc , false,
	//						0xaa, false,
	//						&pRealRule, VirtRule);

	return;
	//WriteToLog(10, L"c:\\test\\test.exe", 10, 1, 1, L"c:\\test\\test.obj", 0);

}

