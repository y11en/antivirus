// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  30 March 2006,  12:36 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- tmhost.cpp
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __tmhost_cpp__57dcc869_1f25_421e_9bc5_78b2ffb3de8e )
#define __tmhost_cpp__57dcc869_1f25_421e_9bc5_78b2ffb3de8e
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
#include <plugin/p_tm.h>
// AVP Prague stamp end

#include <pr_remote.h>
#include <iface/i_csect.h>
#include <structs/s_taskmanager.h>

#define pm_REMOTE_GLOBAL_TM_INITED 0xca94f5e7
#define pgTM_HOST "TmHost"

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable cTmHostImpl : public cTmHost, public CPRRemotePrague
{
private:
// Internal function declarations
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations

public:
// External function declarations
	tERROR pr_call CreateTask( hOBJECT p_host, cSerializable* p_info, hOBJECT* p_task );
	tERROR pr_call DestroyTask( hOBJECT p_task );

// Data declaration
// AVP Prague stamp end

	tERROR pr_call AttachObject(cObj* obj, cObj* parent=NULL);
	tERROR pr_call DetachObject(cObj* obj);

	struct cTHTask : public cTaskInfo
	{
		cTHTask(cObj* task = NULL) : m_task(task){}
		bool operator == (const cTHTask& c){ return c.m_task == m_task; }

		cObj*   m_task;
	};

	cVector<cTHTask>  m_tasks;
	cCriticalSection* m_lock;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(cTmHostImpl)
};
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // tmhost_cpp
// AVP Prague stamp end

