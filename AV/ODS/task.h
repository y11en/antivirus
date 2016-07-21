// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  27 April 2005,  15:19 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __task_cpp__28e63468_756d_4460_8aea_c7d6e27e838c )
#define __task_cpp__28e63468_756d_4460_8aea_c7d6e27e838c
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/e_loader.h> 
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_reg.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_objptr.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_threadpool.h>
#include <Prague/plugin/p_fsdrvplgn.h>

#include <AV/plugin/p_ods.h>
#include <AV/structs/s_scaner.h>

#include <ProductCore/plugin/p_report.h>
#include <ProductCore/plugin/p_tm.h>
// AVP Prague stamp end

class	CScannerThread;
struct	cODSImpl;

// AVP Prague stamp begin( C++ class declaration,  )
struct cODSImpl : public cTask, public cThreadPoolBase
{
private:
	cODSImpl(): cThreadPoolBase("ODS") {}

// Internal function declarations
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
	cRegistry *    m_hConfig;    // --
	cTaskManager * m_hTM;        // --
	tDWORD         m_nState;     // --
	tDWORD         m_nSessionId; // --
	tDWORD         m_nTaskId;    // --
	cAVS*          m_hAVS;       // --
	cRegistry*     m_hStorage;   // --
// AVP Prague stamp end

	tDWORD          GetCurCompletion(tDWORD* tmFinish=NULL);
	tERROR	        CheckPostponeScan();

	tERROR          OnTaskStarted();
	tERROR          OnTaskCompleted();

	cODSSettings	m_Settings;
	cODSRuntimeParams m_Params;
	cAVSSession *	m_avsSession;
	cAVSTreats *    m_avsTreats;
	bool            m_bSomeUntreated;
	tDWORD          m_nStartPersent;

	CScannerThread* m_hScanThread;
	CScannerThread* m_hCounterThread;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(cODSImpl)
};
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // task_cpp
// AVP Prague stamp end

