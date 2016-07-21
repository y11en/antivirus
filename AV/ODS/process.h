// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- task.cpp
// -------------------------------------------

#include "task.h"
#include "win32hlpr.h"

#include <Prague/iface/i_buffer.h>
#include <Prague/iface/i_csect.h>
#include <Prague/iface/i_token.h>

#include <AV/iface/i_engine.h>
#include <AV/structs/s_startupenum2.h>

// -------------------------------------------

class CObjectScanner : public cDetectObjectInfo, public cScanObjectStatistics
{
public:
	virtual ~CObjectScanner(){}

	// bIsOriginalObject - true для объектов, пришедших извне на сканирование
	virtual void Process(bool bPreProcess, bool bIsOriginalObject) = 0;

	bool ReinitCheck(tDWORD nMask);
	bool ReinitCheckPoint(tDWORD nMask, bool bBegin);

	static tERROR Init(hROOT root);
	static CObjectScanner * Create(CScannerThread* pThread, cScanObject& pObject);

protected:
	bool CheckStop();
	bool PreProcessObject(bool bUseINtfs = false, hObjPtr hCheckObj = NULL);
	bool ProcessObject(hOBJECT hObj, bool bIsNative = false);
	bool ProcessHiddenObjects(cObjPtr* obj);
	bool ProcessNTFSStreams(hOBJECT hObject, bool bFolder, bool bEnumOnly = false);

	tERROR SendReport(tDWORD nClass, enObjectStatus nStatus, int nDesc = 0);

public:
	static tPROPID		m_propVirtualName;

	bool				m_bStop;
	bool		        m_bDoCount;
	bool		        m_bNtfsCheker;

	cODSImpl *			m_hTask;
	CScannerThread *	m_pThread;
	cProtectionSettings	m_Settings;
	cScanProcessInfo	m_ProcessInfo;
	enScanObjType		m_nScannerType;
	tDWORD		        m_nCurrentMask;
	cDetectObjectInfo *	m_pDetectInfo;
	CNtfsChecker        m_ntfsChecker;
};

// -------------------------------------------

class CScannerThread : public cThreadTaskBase, public cODSStatistics
{
public:
	CScannerThread(cODSImpl * pTask, bool bCounter);
	~CScannerThread();

	void   Start();
	void   InitScanners();
	void   ReinitState(cODSStatistics& pStatistics);
	bool   IsShoudBeProcessed(cStrObj& strName);

	virtual void do_work();

public:
	cVector<CObjectScanner*> m_aScaners;

	cODSImpl *	m_hTask;

	bool		m_bCounter;
	bool		m_bCompleted;
	bool        m_bReinited;

	time_t		m_tmStart;
	tDWORD		m_nStartTicks;
	tDWORD		m_nObjCount;
	tPTR        m_pContext;

	cToken *	m_hToken;
//
	cVector<tDWORD>	m_aAlreadyScanObjectsCRC32;
};

// -------------------------------------------

template <typename T>
class CReinitCheckpoint
{
public:
	CReinitCheckpoint(T* pScaner, tDWORD nMask) : m_pScaner(pScaner), m_nMask(nMask){}
	~CReinitCheckpoint(){ m_pScaner->ReinitCheckPoint(m_nMask, false); }

	bool verify()
	{
		if( !m_pScaner->m_pThread->m_nReinitMask || m_pScaner->ReinitCheck(m_nMask) )
			return m_pScaner->ReinitCheckPoint(m_nMask, true);
		return false;
	}

private:
	T*      m_pScaner;
	tDWORD  m_nMask;
};

#define REINIT_CHECKPOINT(T,mask,check) \
	if( check ) return; \
	CReinitCheckpoint<T> ri(this,mask); \
	if( !bPreProcess && !ri.verify() ) return;


// -------------------------------------------
