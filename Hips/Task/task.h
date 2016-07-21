// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  18 July 2007,  12:01 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __task_cpp__aceb5496_61a1_4fba_a196_0e4204412b7e )
#define __task_cpp__aceb5496_61a1_4fba_a196_0e4204412b7e
// AVP Prague stamp end

#define PR_IMPEX_DEF

#include "local_inc.h"
// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
#include <pr_vtbl.h>
#include <iface/i_reg.h>
#include <plugin/p_hips.h>
// AVP Prague stamp end

#include <plugin/p_processmonitor.h>
#include <structs/procmonm.h>
#include <structs/s_trmod.h>

#include "hook/avpgcom.h"
#include "mklif/mklapi/mklapi.h"
#include "NetRulesManager.h"
//#include "iface/i_threadpool.h"
//#include "plugin/p_thpoolimp.h"


class CHipsManager;
struct SHipsRuleItem;




// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CHipsTask : public cTask 
{
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations

public:
// External function declarations
	tERROR pr_call SetSettings( const cSerializable* p_settings );
	tERROR pr_call GetSettings( cSerializable* p_settings );
	tERROR pr_call AskAction( tActionId p_actionId, cSerializable* p_actionInfo );
	tERROR pr_call SetState( tTaskRequestState p_state );
	tERROR pr_call GetStatistics( cSerializable* p_statistics );

// Data declaration
// AVP Prague stamp end
	tTaskState m_TaskState; // --

	CHipsManager * GetHipsManager() {return m_pHipsManager;};
	DWORD ReloadAllRulesInDrv();
	DWORD ReloadRulesInDrv(HipsRulesList * l_pHipsRulesList);

	bool ConvertFilePathToNative(wchar_t * l_pSrcPath, wchar_t ** l_ppDstPath);
	bool UserRegPath2Kernel(wchar_t * UserRegPath, wchar_t ** KernelRegPath);
	bool KernelRegPath2User(wchar_t ** l_ppUserRegPath, wchar_t * l_pKernelRegPath);
	bool MakeFileName (
		PVOID pClientContext,
		PVOID pMessage,
		ULONG MessageSize,
		ULONG ContextFlags,
		PWCHAR* ppwchFileName,
		PULONG pFileNameLen,
		bool bDestination
		);
	cProcessMonitor*		m_pProcMonObj;


// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(CHipsTask)


private:
	CHipsManager * m_pHipsManager;
	CNetRulesManager * m_pNetRulesManager;

	DWORD ResetRules();
	DWORD ApplyRules();
	DWORD AddRuleToDrv(SHipsRuleItem * l_pRule);
	HANDLE m_JobThreadHandle;
	hTASKMANAGER	m_tm;
	bool CheckProcessOnStart();
};
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // task_cpp
// AVP Prague stamp end



