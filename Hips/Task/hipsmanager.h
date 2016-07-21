// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2007.
// -------------------------------------------
// Project     -- Hips
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Martynenko
// File Name   -- hipsmanager.h
// Date		   -- 2007.07.20 (creation)
// -------------------------------------------
#ifndef _HIPS_MANAGER_H_
#define _HIPS_MANAGER_H_

#include "local_inc.h"

#include "task.h"

#include "hipsrulemanager.h"
//#include "userinteractionqueue.h"

#include "cert_func.h"
#include "verifytrust.h"

#include "iface/i_threadpool.h"
#include "plugin/p_thpoolimp.h"


#define FILE_OP_WRITE	0x1
#define FILE_OP_READ	0x2
#define FILE_OP_ENUM	0x4
#define FILE_OP_DELETE  0x8

//////////////////////////////////////////////////////////////////////////
enum EnumHipsState {
	hsRun,
	hsStop
};
//////////////////////////////////////////////////////////////////////////
//	this ID equal to AppGrId in base
enum EnumSecRating {
	eSRTrusted			= 3,
	eSRSecurity			= 4,
	eSRRestrictedLow	= 5,
	eSRRestrictedHi		= 6,
	eSRUntrusted		= 7
};
//////////////////////////////////////////////////////////////////////////

void* __cdecl pfMemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag );


void __cdecl pfMemFree (
	PVOID pOpaquePtr,
	void** pptr );
//////////////////////////////////////////////////////////////////////////
//	struct for callback threadpool function (ask events)
//////////////////////////////////////////////////////////////////////////
//struct CallBackAskStruct
//{
//	PVOID m_pContext;
//	PVOID m_pMessage;
//	DWORD m_MessageSize;
//	ULONG m_ContextFlags;
//	CHipsManager * m_pHipsManager;
//	DWORD m_RuleID;
//	DWORD m_Action;
//	DWORD m_Pid;
//	wchar_t * m_pObjectName;//[500];
//	CallBackAskStruct()
//	{
//		m_pContext = 0;
//		m_pMessage = 0;
//		m_MessageSize = 0;
//		m_ContextFlags = 0;
//
//		m_pHipsManager = 0;
//		m_RuleID = 0;
//		m_Action = 0,
//		m_Pid = 0;
//		m_pObjectName = 0;
//	}
//	~CallBackAskStruct()
//	{
//		if (m_pObjectName)
//			delete [] m_pObjectName;
//		m_pObjectName = 0;
//	}
//};
//////////////////////////////////////////////////////////////////////////
//	struct for callback threadpool function (procnotify events)
//////////////////////////////////////////////////////////////////////////
//struct CallBackProcNotifyStruct
//{
//	PVOID m_pContext;
//	PVOID m_pMessage;
//	DWORD m_MessageSize;
//	CHipsManager * m_pHipsManager;
//	CHipsTask * m_pHipsTask;
//	wchar_t * m_pProcName;
//	BYTE m_ProcHash[HIPS_HASH_SIZE];
//
//	CallBackProcNotifyStruct()
//	{
//		m_pContext = 0;
//		m_pMessage = 0;
//		m_MessageSize = 0;
//		m_pHipsManager = 0;
//		m_pProcName = 0;
//		m_pHipsTask = 0;
//	}
//	~CallBackProcNotifyStruct()
//	{
//		if (m_pProcName)
//			delete [] m_pProcName;
//		m_pProcName = 0;
//	}
//};
//////////////////////////////////////////////////////////////////////////
//	struct for callback threadpool function (log events)
//////////////////////////////////////////////////////////////////////////
struct CallBackHipsStruct
{
	PVOID			m_pContext;
	ULONG			m_FunctionMj;
	ULONG			m_FunctionMi;
	PVOID			m_pMessage;
	DWORD			m_MessageSize;
	ULONG			m_ContextFlags;
	CHipsManager *	m_pHipsManager;
	CHipsTask *		m_pHipsTask;

	DWORD			m_RuleID;
	DWORD			m_SrcPID;
	DWORD			m_Action;
	DWORD			m_Result;
	wchar_t *		m_SrcProcName;
	wchar_t *		m_ObjectName1;
	wchar_t *		m_ObjectName2;
	BYTE			m_ProcHash[HIPS_HASH_SIZE];

	CallBackHipsStruct()
	{
		m_pContext = 0;
		m_pMessage = 0;
		m_MessageSize = 0;
		m_ContextFlags = 0;
		m_pHipsManager = 0;
		m_pHipsTask = 0;

		m_RuleID = 0;
		m_SrcPID = 0;
		m_Action = 0;
		m_Result = 0;
		m_SrcProcName = 0;
		m_ObjectName1 = 0;
		m_ObjectName2 = 0;

	}
	~CallBackHipsStruct()
	{
		if (m_SrcProcName)
			delete [] m_SrcProcName;
		m_SrcProcName = 0;
		if (m_ObjectName1)
			delete [] m_ObjectName1;
		m_ObjectName1 = 0;
		if (m_ObjectName2)
			delete [] m_ObjectName2;
		m_ObjectName2 = 0;
	}
};
//////////////////////////////////////////////////////////////////////////
//	struct for callback threadpool function (other events)
//////////////////////////////////////////////////////////////////////////
//struct CallBackOtherEvStruct
//{
//	PVOID			m_pContext;
//	PVOID			m_pMessage;
//	DWORD			m_MessageSize;
//	CHipsManager *	m_pHipsManager;
//	CHipsTask *		m_pHipsTask;
//
//	ULONG			m_FunctionMj;
//	ULONG			m_FunctionMi;
//	DWORD			m_ProcessId;
//
//	CallBackOtherEvStruct()
//	{
//		m_pContext = 0;
//		m_pMessage = 0;
//		m_pHipsManager = 0;
//		m_pHipsTask = 0;
//
//		m_FunctionMj = 0;
//		m_FunctionMi = 0;
//		m_MessageSize = 0;
//		m_ProcessId = 0;
//	}
//};
//////////////////////////////////////////////////////////////////////////
tERROR pr_call OnThreadInitCallback (
									tThreadCallType CallType,
									tPTR pCommonThreadContext,
									tPTR* ppThreadContext
									);
//////////////////////////////////////////////////////////////////////////
tERROR pr_call ThreadInfoCallback (
	tThreadCallType CallType,
	tPTR pThreadContext,
	tPTR* ppTaskContext,
	tPTR pTaskData,
	tDWORD TaskDataLen
	);
//////////////////////////////////////////////////////////////////////////
tERROR pr_call ThreadProcNotifyCallback (
	tThreadCallType CallType,
	tPTR pThreadContext,
	tPTR* ppTaskContext,
	tPTR pTaskData,
	tDWORD TaskDataLen
	);
//////////////////////////////////////////////////////////////////////////
tERROR pr_call ThreadLogCallback (
	tThreadCallType CallType,
	tPTR pThreadContext,
	tPTR* ppTaskContext,
	tPTR pTaskData,
	tDWORD TaskDataLen
	);
//////////////////////////////////////////////////////////////////////////
tERROR pr_call ThreadOtherEvCallback (
	tThreadCallType CallType,
	tPTR pThreadContext,
	tPTR* ppTaskContext,
	tPTR pTaskData,
	tDWORD TaskDataLen
	);
//////////////////////////////////////////////////////////////////////////
class CHipsLocalCash
{
	typedef struct _tAnswerCash{
		DWORD f_PID;
		DWORD f_ResID;
		DWORD f_Value;
		_tAnswerCash(DWORD l_PID, DWORD l_ResID, DWORD l_Value) : f_PID(l_PID), f_ResID(l_ResID), f_Value(l_Value) {};
	} tAnswerCash;
	typedef std::list<tAnswerCash>::iterator tAnswerListIt;

	typedef struct _tProcCash{
		DWORD	f_PID;
		bool	f_IsFirstThreadStarted;
		_tProcCash(DWORD l_PID, bool l_IsFirstThreadStarted) :
					f_PID(l_PID), f_IsFirstThreadStarted(l_IsFirstThreadStarted){};
	} tProcCash;
	typedef std::list<tProcCash>::iterator tProcListIt;

	std::list<tProcCash> m_ProcList;
	std::list<tAnswerCash> m_AnswerList;

public:
	CHipsLocalCash() 
	{
		m_AnswerList.clear();
		m_ProcList.clear();
	};
	~CHipsLocalCash() 
	{
		m_AnswerList.clear();
		m_ProcList.clear();
	};
	void AddAnswer(DWORD l_PID, DWORD l_ResID, DWORD l_Value)
	{
		m_AnswerList.push_back(tAnswerCash(l_PID, l_ResID, l_Value));
	}
	void AddNewProcess(DWORD l_PID, bool l_IsFirstThreadStarted = false)
	{
		DelPID(l_PID);
		m_ProcList.push_back(tProcCash(l_PID, l_IsFirstThreadStarted));
	}
	void DelPID(DWORD l_PID)
	{
		tAnswerListIt It = m_AnswerList.begin();
		while (It != m_AnswerList.end())
		{
			if (It->f_PID == l_PID)
				It = m_AnswerList.erase(It);
			else
				It++;
		}
		tProcListIt It2 = m_ProcList.begin();
		while (It2 != m_ProcList.end())
		{
			if (It2->f_PID == l_PID)
				It2 = m_ProcList.erase(It2);
			else
				It2++;
		}
	}
	bool IsInAnswerCash(DWORD l_PID, DWORD l_ResID, DWORD * l_pOutValue)
	{
		tAnswerListIt It = m_AnswerList.begin();
		while (It != m_AnswerList.end())
		{
			if ((It->f_PID == l_PID) &&
				(It->f_ResID == l_ResID))
			{
				if (l_pOutValue)
					*l_pOutValue = It->f_Value;
				return true;
			}
			else
				It++;
		}
		return false;
	}
	bool IsFirstThreadStarted(DWORD l_PID)
	{
		tProcListIt It = m_ProcList.begin();
		while (It != m_ProcList.end())
		{
			if (It->f_PID == l_PID)
			{
				return It->f_IsFirstThreadStarted;
			}
			else
				It++;
		}
		return false;
	}
	void SetFirstThreadStarted(DWORD l_PID, bool l_Value)
	{
		tProcListIt It = m_ProcList.begin();
		while (It != m_ProcList.end())
		{
			if (It->f_PID == l_PID)
			{
				It->f_IsFirstThreadStarted = l_Value;
				return;
			}
			else
				It++;
		}
	}
};
//////////////////////////////////////////////////////////////////////////
class CHipsManager
{
private:
	void * m_pTask;

	HVERIFY m_hVerify; // Signature Verification Service


public:
	cThreadPool*		m_ThPoolInfo;
	cThreadPool*		m_ThPoolProcNotify;
	cThreadPool*		m_ThPoolLog;
	cThreadPool*		m_ThPoolOtherEv;

	CHipsRuleManager m_HipsRuleManager;
	bool m_IsNeedToStopAll;
	PVOID m_pClientContext;

	CHipsLocalCash m_LocalCash;

	cHIPSStatistics m_HIPSStatistics;

public:
	CHipsManager(void);
	~CHipsManager(void);

	BOOL Initialize(void * l_pTask);
	BOOL InitThreadPools();
	void FreeThreadPools();

	BOOL GetFirstRule(pSHipsRuleItem * l_ppRule, HipsRulesList * l_pHipsRulesList) {return m_HipsRuleManager.GetFirstRule(l_ppRule, l_pHipsRulesList);};
	BOOL GetNextRule(pSHipsRuleItem * l_ppRule, HipsRulesList * l_pHipsRulesList) {return m_HipsRuleManager.GetNextRule(l_ppRule, l_pHipsRulesList);};

	BOOL GetFileDigitalSignature(wchar_t * l_ProcName, wchar_t ** l_pOutStr);

	bool CreateThreadPool (cThreadPool** ppThPool, tDWORD MaxLen, tThreadPriority Priority, tDWORD MinThreads, tDWORD MaxThreads);

	static wchar_t * GetStrRes(DWORD Value);
	//static cHipsReportInfo::enEvent GetEnumRes(DWORD Value);
	static enObjectStatus GetObjStatus(DWORD Value);
	bool GetResInfo(DWORD RuleID, enObjType * l_pOutObjType, wchar_t ** l_ppOutResGrDesc, wchar_t ** l_ppOutResDesc);
	enRDetectType GetDetectTypeByResType(enObjType ResType);
	enEventAction GetPermissionAction(cResource * l_pRes);

	enEventAction GetAction(DWORD Value);

	static wchar_t * GetStrDesiredOperation(DWORD Value, wchar_t * l_OutErrorMsg);

	bool CheckRuleForApp(wchar_t * l_ProcName, BYTE * l_pHash, DWORD l_HashSize, CHipsAppItem ** l_ppApp = 0);
	bool CheckRuleForAppNew(cProcMonNotifyEx * l_pProcMonNotifyEx, bool l_IsNeedToReloadToDrv = false);
	

	EnumSecRating GetSecRating(wchar_t * l_ProcName);

	//	file and reg log
	BOOL WriteToLog(CallBackHipsStruct * l_pStr, cResource * l_pResource);
	//	device log
	BOOL WriteToLog(wchar_t * l_pGUID, wchar_t * l_pType, DWORD l_AM);
	//	permisssion log
	BOOL WriteToLogPerm(DWORD l_SrcPID, CHipsAppItem * l_pDestApp, wchar_t * l_CmdLine, cResource * l_pResource, DWORD l_AM, DWORD l_Result);
	//	add app log
	BOOL WriteToLogAddApp(DWORD l_SrcPID, wchar_t * l_NewGrName, wchar_t * l_OldGrName);

	enObjType GetObjType(cResource * l_pResource);
	BOOL GetObjName(cResource * l_pResource, cStringObj & l_OutStr);
	BOOL GetObjName(wchar_t * l_pObjectName, enObjType l_ObjType, CallBackHipsStruct * l_CtxStruct, cStringObj & l_OutStr);
	DWORD GetUserAskAction(CHipsAppItem * l_pSrcApp, CallBackHipsStruct * l_pStr, cResource * l_pResource);

	CHipsAppItem * GetAppByPid(PVOID l_pContext, DWORD l_PID);
	DWORD GetPermissionFlag(CHipsAppItem * l_pSrcApp, CHipsAppItem * l_pDestApp, DWORD l_Permission, cResource ** l_ppOutRes);
	DWORD GetPermissionResult(DWORD l_SrcPID, CHipsAppItem * l_pSrcApp, CHipsAppItem * l_pDestApp, wchar_t * l_CmdLine, DWORD l_Permission);
	DWORD AskPermission(DWORD l_SrcPID, CHipsAppItem * l_pSrcApp, CHipsAppItem * l_pDestApp, cResource * l_pRes, wchar_t * l_CmdLine = 0);
	void TestFunc();
#ifdef _DEBUG
	void TestEnumAppExclRules();
#endif //_DEBUG
	//CUserInteractionQueue m_UserQueue;

};

//typedef std::pair<cResource *, size_t> tState;
//typedef std::vector<tState> tStates;



#endif // _HIPS_MANAGER_H_