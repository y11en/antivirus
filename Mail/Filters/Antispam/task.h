// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  05 July 2007,  17:25 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Denisov
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __task_cpp__e60d019f_d8a3_4fd4_89b2_fcbd61071348 )
#define __task_cpp__e60d019f_d8a3_4fd4_89b2_fcbd61071348
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_reg.h>
#include <Prague/iface/i_root.h>

#include <Mail/iface/i_antispamfilter.h>
#include <Mail/plugin/p_antispamtask.h>
#include <Mail/structs/s_antispam.h>

#include <ProductCore/iface/i_taskmanager.h>
// AVP Prague stamp end

#include "../../ThreadImp.h"
#include "FilterManager.h"

#include <windows.h>
#include <wtsapi32.h>



// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CTask : public cTask {
private:
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
	tDWORD              m_state;               // --
	hTASKMANAGER        m_tm;                  // --
	tDWORD              m_session_id;          // --
	cAntispamSettings   m_settings;            // --
	cAntispamStatistics m_statistics;          // --
	hREGISTRY           m_persistance_storage; // --
// AVP Prague stamp end



	tERROR GetServices( tBOOL bIncomingOnly, tBOOL bRelease );
	CFilterManager*		m_hAntispamFilter;
	tIID m_iidToHandle;

	tERROR CheckTrainingWizard(tDWORD dwPID, tBOOL bIsUnknownTrainState);

public:
	cCriticalSection* m_lock; //!< Критическая секция для корректного разведения настроек
	
// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(CTask)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // task_cpp
// AVP Prague stamp end



