// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  29 October 2004,  14:41 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- avstreats.cpp
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __avstreats_cpp__1ea2a129_15b5_488c_91c4_338dd6d565f5 )
#define __avstreats_cpp__1ea2a129_15b5_488c_91c4_338dd6d565f5
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_token.h>

#include <AV/plugin/p_avs.h>
// AVP Prague stamp end


struct AVSImpl;
struct AVSSession;
struct AVSTreats;

struct tTreatInfo
{
	tTreatInfo(){}
	tTreatInfo(tQWORD qwUnique, tDWORD nStoredPos, enObjectStatus nStatus, tDWORD nScopeId=-1) :
		m_qwUnique(qwUnique), m_nStoredPos(nStoredPos), m_nStatus(nStatus), m_nScopeId(nScopeId), m_pData(0){}

	tQWORD         m_qwUnique;
	tDWORD         m_nStoredPos;
	tDWORD         m_nStatus;
	tDWORD         m_nProcessIdx;
	tDWORD         m_nScopeId;
	tPTR           m_pData;
};

struct CProcessThreatsTask : public cThreadTaskBase
{
	CProcessThreatsTask() : m_pTreats(NULL), m_nCount(0), m_nPos(0){}

	tERROR Process(AVSTreats* pTreats, cAskProcessTreats* pScanInfo, tDWORD nFilter);

	virtual tERROR OnInit(){ return errOK; }
	virtual tERROR OnEnum(tDWORD nPos, tDWORD nIdx){ return errOK; }
	virtual tERROR OnProcess(){ return errOK; }

public:
	cAskProcessTreats* m_pScanInfo;
	AVSTreats*    m_pTreats;
	tDWORD        m_nCount;
	tDWORD        m_nPos;
};

struct CDiscardThreatsTask : public CProcessThreatsTask
{
	virtual tERROR OnEnum(tDWORD nPos, tDWORD nIdx);
	virtual tERROR OnProcess();

private:
	cDwordVector  m_vecPos;
};

struct CScanThreatsTask :
	public CProcessThreatsTask,
	public cProtectionSettings
{
	CScanThreatsTask() : m_hIO(NULL), m_pSession(NULL),
		m_hToken(NULL), m_bOwnSession(cTRUE), m_pEngine(NULL){}
	~CScanThreatsTask();

	void   UpdateThreatInfo(tTreatInfo& pInfo);
	void   GetInfo(cScanTreatsInfo* pInfo);
	void   SetCurPos(CObjectContext& pCtx);

	virtual tERROR OnInit();
	virtual tERROR OnEnum(tDWORD nPos, tDWORD nIdx);
	virtual tERROR OnProcess();

public:
	cIO *         m_hIO;
	AVSSession *  m_pSession;
	cToken*       m_hToken;
	CAVSEngine*   m_pEngine;
	cVector<tTreatInfo> m_aIdx;
	cScanProcessInfo m_ProcessInfo;
	tBOOL         m_bOwnSession;

	virtual void do_work();
	virtual void on_exit();
};

// AVP Prague stamp begin( C++ class declaration,  )
struct AVSTreats : public cAVSTreats
{
private:
// Internal function declarations
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();

// Property function declarations

public:
// External function declarations
	tERROR pr_call GetTreatInfo( tDWORD p_filter, tDWORD p_pos, cSerializable* p_info );
	tERROR pr_call Process( tDWORD p_filter, const cSerializable* p_process_info );
	tERROR pr_call GetInfo( cSerializable* p_info );
	tERROR pr_call ProcessCancel();

// Data declaration
// AVP Prague stamp end

	void ClearTreatInfo();
	bool InitUntreated();
	bool UpdateTreatInfo(AVSSession* pSession, tTreatInfo& pInfo, bool bNew);
	void SetCurrentObject(CObjectContext& pCtx);
	void GetCurrentObject(tDWORD nPos, cStrObj& strName);
	void ProcessEnd();

	cCriticalSection *  m_lock;
	AVSImpl*      m_pAVS;
	tDWORD        m_nTaskId;
	tDWORD        m_nRef;
	tDWORD        m_nIdxPos;
	cDwordVector  m_vecTreats;
	cDwordVector  m_vecUtreated;
	time_t        m_tmStamp;
	AVSSession *  m_pInitSession;

	CScanThreatsTask* m_pTask;
	
// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(AVSTreats)
};
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // avstreats_cpp
// AVP Prague stamp end

