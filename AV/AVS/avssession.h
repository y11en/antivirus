// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  30 January 2007,  14:21 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- avssession.cpp
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __avssession_cpp__6e57e4a3_50cb_4fdc_b9cd_9ba164a8e09d )
#define __avssession_cpp__6e57e4a3_50cb_4fdc_b9cd_9ba164a8e09d
// AVP Prague stamp end


// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_reg.h>

#include <AV/iface/i_engine.h>
#include <AV/plugin/p_avs.h>
#include <AV/structs/s_startupenum2.h>

#include <ProductCore/iface/i_report.h>
// AVP Prague stamp end


//public cAVSSession, public cProtectionStatisticsEx
// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable AVSSession : public cAVSSession, public cProtectionStatisticsEx
{
private:
// Internal function declarations
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations

public:
// External function declarations
	tERROR pr_call PreProcessObject( cSerializable* p_process_info, const cSerializable* p_settings );
	tERROR pr_call ProcessObject( hOBJECT p_object, cSerializable* p_process_info, const cSerializable* p_settings, cSerializable* p_statistics );
	tERROR pr_call GetStatistic( cSerializable* p_statistics );
	tERROR pr_call SetState( tTaskRequestState p_state );
	tERROR pr_call ResetStatistic( cSerializable* p_statistics, tBOOL p_in );
	tERROR pr_call RegisterScope( tDWORD* p_scope, const cSerializable* p_info );
	tERROR pr_call ProcessStream( tDWORD p_chunk_type, tPTR p_chunk, tDWORD p_size, cSerializable* p_process_info, const cSerializable* p_settings, cSerializable* p_statistic );

// Data declaration
// AVP Prague stamp end

	void        PerformABS(tDWORD nCurrentTime, tDWORD nABSNumber);
	void		SetCurrentObject(CObjectContext& pCtx);
	void		IncrementStatistics(tDWORD nScope, tDWORD nOffset, tINT nNum=1);
	bool		UpdateDetectedInfo(cInfectedObjectInfo* pInfInfo);
	tERROR      SendReport(tDWORD nMsgClass, cDetectObjectInfo* pData, cObj* pObject);
	void        CleanupStartupObject(cStringObj& strObjectName, hOBJECT pObject, cStringObj& strDetectName);
	bool        MSVerify(cStrObj& strFName);

	tERROR      FindAskResult(cAskObjectAction &pRequest);
	enActions   AskObjectAction(tActionId nAction, cAskObjectAction &pRequest, bool bUseCache);

public:
	AVSSession() : m_lock(NULL), m_hCrypter(NULL), m_pStartupSvc(NULL), m_pTreats(NULL){}

	cTask *             m_task;
	AVSImpl *	        m_pAVS;
	AVSTreats *         m_pTreats;
	cStartUpEnum2*      m_pStartupSvc;
	cCryptoHelper *     m_hCrypter;

	cStringObj          m_strTaskType;
	tTaskId             m_nTaskId;
	enTaskState	        m_nState;
	enObjectOrigin      m_nOrigin;
	tMsgHandlerDescr	m_hndList[5];

	tDWORD              m_eEngineMask;
	enEngineType        m_eBaseEngine;
	tLONG               m_nProcessCount;
	tBOOL               m_bEngineLocked;
	tBOOL               m_bVerifyThreats;
	tBOOL               m_bActiveDisinfection;

	tBOOL               m_nSleepTimeout;
	tDWORD              m_nYeildCounter;
	tDWORD              m_nLastABSRequest;
	tQWORD              m_qwTimeCurrent;
	tQWORD              m_qwTotalWaitTime;

	struct tAskResult
	{
		tAskResult(tActionId a=0, int m=0) :
			m_nAction(a), m_nActionsMask(m),
			m_nResult(ACTION_UNKNOWN), m_nDetectType(DETYPE_UNKNOWN){}

		tActionId		m_nAction;
		int				m_nActionsMask;
		enActions		m_nResult;
		enDetectType    m_nDetectType;
	};

	cVector<tAskResult> m_vecAskResults;

private:
	cCriticalSection *  m_lock;
	cCriticalSection *  m_ask_lock;

	static tPROPID      m_propTaskType;
	static tPROPID      m_propTaskId;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION_NO_DELETE(AVSSession)
};
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // avssession_cpp
// AVP Prague stamp end

