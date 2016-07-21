// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- task.cpp
// -------------------------------------------

#include <Prague/prague.h>
#include <Prague/pr_time.h>

#include "scaners.h"
#if defined (_WIN32)
#include "../../Extract/Disk/ntfsstream/stream_optr.h"
#endif

#define IMPEX_IMPORT_LIB
#include <Prague/plugin/p_win32_nfio.h>
#include <hashutil/hashutil.h>

// -------------------------------------------
// Object scaner

tPROPID	CObjectScanner::m_propVirtualName;

tERROR CObjectScanner::Init(hROOT root)
{
	return root->RegisterCustomPropId(&m_propVirtualName, npENGINE_VIRTUAL_OBJECT_NAME, pTYPE_STRING);
}

CObjectScanner * CObjectScanner::Create(CScannerThread * pThread, cScanObject& pObject)
{
	CObjectScanner * pScanner = NULL;
	switch(pObject.m_nObjType)
	{
	case OBJECT_TYPE_MY_COMPUTER:			pScanner = new CMyComputerScanner(); break;
	case OBJECT_TYPE_DRIVE:					pScanner = new CDriveScanner(); break;
	case OBJECT_TYPE_FOLDER:				pScanner = new CFolderScanner(); break;
	case OBJECT_TYPE_FILE:					pScanner = new CFileScanner(); break;
	case OBJECT_TYPE_MASKED:				pScanner = new CFolderScanner(); break;
	case OBJECT_TYPE_MEMORY:				pScanner = new CMemoryScanner(); break;
	case OBJECT_TYPE_STARTUP:				pScanner = new CStartUpObjectsScanner(); break;
	case OBJECT_TYPE_MAIL:					pScanner = new CMailSystemScanner(); break;
	case OBJECT_TYPE_BOOT_SECTORS:			pScanner = new CDrivesScanner(); break;
	case OBJECT_TYPE_ALL_DRIVES:			pScanner = new CDrivesScanner(); break;
	case OBJECT_TYPE_ALL_REMOVABLE_DRIVES:	pScanner = new CDrivesScanner(); break;
	case OBJECT_TYPE_ALL_FIXED_DRIVES:		pScanner = new CDrivesScanner(); break;
	case OBJECT_TYPE_ALL_NETWORK_DRIVES:	pScanner = new CDrivesScanner(); break;
	case OBJECT_TYPE_QUARANTINE:	        pScanner = new CQuarantineScanner(); break;
	case OBJECT_TYPE_SYSTEM_RESTORE:	    pScanner = new CSystemRestoreScanner(); break;
	default: break;
	}

	if( !pScanner )
		return NULL;

	pScanner->m_bStop = false;
	pScanner->m_bDoCount = true;
	pScanner->m_bNtfsCheker = false;

	pScanner->m_pScanObj = pObject;
	pScanner->m_nScannerType = (enScanObjType)pObject.m_nObjType;
	pScanner->m_pThread = pThread;
	pScanner->m_hTask = pThread->m_hTask;
	pScanner->m_Settings = pThread->m_hTask->m_Settings;
	pScanner->m_strObjectName = pObject.m_strObjName;
	pScanner->m_nCurrentMask = 0;
	pScanner->m_pDetectInfo = NULL;

	return pScanner;
}


bool CObjectScanner::CheckStop()
{
	if( m_bStop )
		return true;

	if( m_hTask->m_nState != TASK_STATE_STOPPED )
		return false;

	if( m_nCurrentMask & 0x80000000 )
		m_pThread->m_nReinitCompletion = m_hTask->GetCurCompletion();

	m_pThread->m_nReinitMask = m_nCurrentMask;
	m_pThread->m_sReinitName = m_strObjectName;

	m_bStop = true;
	return true;
}

bool CObjectScanner::ReinitCheck(tDWORD nMask)
{
	return !m_bStop && (m_pThread->m_nReinitMask & nMask);
}

bool CObjectScanner::ReinitCheckPoint(tDWORD nMask, bool bBegin)
{
	if( bBegin )
	{
		m_nCurrentMask |= nMask; 
		m_pThread->m_nReinitMask &= ~nMask;
		m_pThread->m_nReinitCompletion = m_hTask->GetCurCompletion();
	}
	else
		m_nCurrentMask &= ~nMask; 
	return true;
}


bool CObjectScanner::PreProcessObject(bool bUseINtfs, hObjPtr hCheckObj)
{
	if( CheckStop() )
		return true;

	if( m_bDoCount )
		m_pThread->m_nObjCount++;

	if( m_pThread->m_bCounter )
		return true;

	m_ProcessInfo.m_nProcessStatusMask = 0;
	m_Settings.m_bUseExcludes = (m_nScannerType != OBJECT_TYPE_FILE);

	if( !bUseINtfs && !hCheckObj )
		return false;

	tBOOL bUseIStreams = m_Settings.m_bUseIStreams;
	m_Settings.m_bUseIStreams = bUseIStreams && bUseINtfs;

	cAutoString strToCheck(m_strObjectName);
	m_ProcessInfo.m_hICheckObject = strToCheck;

	tERROR error = m_hTask->m_avsSession->PreProcessObject(&m_ProcessInfo, &m_Settings);

	m_Settings.m_bUseIStreams = bUseIStreams;
	m_ProcessInfo.m_hICheckObject = NULL;

	if( error != errOK_DECIDED )
		return false;

	if( bUseINtfs && hCheckObj )
		ProcessNTFSStreams((hOBJECT)hCheckObj, false);

	return true;
}

void ODS_ProcessResult(cObj* pObj, cScanProcessInfo* pPInfo, cDetectObjectInfo* pDInfo)
{
	CObjectScanner* pThis = (CObjectScanner*)pPInfo->m_pAsyncCtx;

	if( (pPInfo->m_nProcessStatusMask & cScanProcessInfo::DETECTED) &&
		!(pPInfo->m_nProcessStatusMask & cScanProcessInfo::DISINFECTED) )
		pThis->m_hTask->m_bSomeUntreated = true;

	if( !(pPInfo->m_nProcessStatusMask & cScanProcessInfo::DELETED) )
		pObj->sysCloseObject();
}

bool CObjectScanner::ProcessObject(hOBJECT hObj, bool bIsNative)
{
	if( CheckStop() )
		return false;

	tBOOL bPostpone = m_Settings.m_nScanAction == SCAN_ACTION_ASKUSER ? 
		m_hTask->m_Params.m_bEnableStopMode && !m_Settings.m_bActiveDisinfection : cFALSE;

	if( bPostpone )
		m_Settings.m_nScanAction = SCAN_ACTION_POSTPONE;

	m_Settings.m_nABSNumber = m_hTask->m_Settings.m_nABSNumber;

	m_ProcessInfo.m_nObjectScopeId = m_pThread->m_nCurObject;
	m_ProcessInfo.m_fnAsyncResult = NULL;
	m_ProcessInfo.m_pAsyncCtx = this;
	if( m_pDetectInfo )
		m_ProcessInfo.m_nProcessStatusMask |= cScanProcessInfo::EXTRENAL_DETECT;
	else if( bIsNative && (m_nScannerType != OBJECT_TYPE_FILE) )
		m_ProcessInfo.m_fnAsyncResult = ODS_ProcessResult;

	if( bIsNative )
		ProcessNTFSStreams(hObj, false);

	m_hTask->m_avsSession->ProcessObject(hObj, &m_ProcessInfo, &m_Settings, m_pDetectInfo);

	if( bPostpone )
		m_Settings.m_nScanAction = SCAN_ACTION_ASKUSER;

	if( !m_ProcessInfo.m_fnAsyncResult )
		ODS_ProcessResult(hObj, &m_ProcessInfo, m_pDetectInfo);

	return !CheckStop();
}

bool CObjectScanner::ProcessNTFSStreams(hOBJECT hObject, bool bFolder, bool bEnumOnly)
{
#if defined (_WIN32)
	if( !m_hTask->m_Settings.m_bScanNTFSStreams )
		return true;

	cAutoObj<cObjPtr> hStreamPtr;
	tERROR error = hObject->sysCreateObject((hOBJECT*)&hStreamPtr, IID_OBJPTR, PID_NTFSSTREAM);

	if( PR_SUCC(error) && bFolder )
	{
		tDWORD size = m_strObjectName.length()-1;
		if( m_strObjectName.at(m_strObjectName.size()-2) == ':' )
			size++;

		m_strObjectName.copy(hStreamPtr, plBASE_FULL_NAME, 0, size);
	}
	else if( hObject->propGetDWord(pgINTERFACE_ID) != IID_IO )
		m_strObjectName.copy(hStreamPtr, plBASE_FULL_NAME);

	if( PR_SUCC(error) )
		error = hStreamPtr->sysCreateObjectDone();

	if( PR_SUCC(error) )
		hObject->propDelete(m_propVirtualName);

	cStringObj strStreamName;

	while( !CheckStop() && PR_SUCC(error) && PR_SUCC(hStreamPtr->Next()) )
	{
		cAutoObj<cIO> hStreamIo;
		if( PR_SUCC(hStreamPtr->IOCreate(&hStreamIo, NULL, fACCESS_READ, fOMODE_OPEN_IF_EXIST)) )
		{
			strStreamName.assign(hStreamIo, pgOBJECT_NAME);
			if( strStreamName == "KAVICHS" )
				hStreamIo->set_pgOBJECT_DELETE_ON_CLOSE(cTRUE);
			else
			{
				if( m_bNtfsCheker )
					m_ntfsChecker.AddObject(m_strObjectName, &strStreamName);

				if( !bEnumOnly )
					ProcessObject((hOBJECT)hStreamIo.relinquish());
			}
		}
	}

#endif
	return true;
}

bool CObjectScanner::ProcessHiddenObjects(cObjPtr* obj)
{
	if( !m_bNtfsCheker )
		return false;

	cVector<cStrObj> vecHidden;
	while( !CheckStop() && m_ntfsChecker.CheckHiddenObjects(obj,
		!!m_hTask->m_Settings.m_bScanNTFSStreams, !!m_hTask->m_Settings.m_bDeepRootkitScan) )
	{
		if( m_ntfsChecker.GetVerdict(vecHidden) )
			break;

		obj->Reset(cFALSE);
		m_strObjectName.assign(obj, pgOBJECT_PATH);
		m_strObjectName.check_last_slash();

		ProcessNTFSStreams((cObj*)obj, true, true);

		while( !CheckStop() && PR_SUCC(obj->Next()) )
		{
			m_strObjectName.assign(obj, pgOBJECT_FULL_NAME);
			m_ntfsChecker.AddObject(m_strObjectName);
			ProcessNTFSStreams((cObj*)obj, false, true);
		}
	}

	if( vecHidden.empty() )
		return false;

	cDetectObjectInfo info;
	info.m_strDetectName = "Hidden.Object";
	info.m_nDetectType = DETYPE_UNDETECT;
	info.m_nDetectStatus = DSTATUS_HEURISTIC;
	info.m_nDetectDanger = DETDANGER_MEDIUM;

	m_pDetectInfo = &info;
	for(size_t i = 0; i < vecHidden.size(); i++)
	{
		m_strObjectName.assign(obj, pgOBJECT_PATH);
		m_strObjectName.check_last_slash();

		tDWORD len = m_strObjectName.size();
		if( vecHidden[i].at(0) == ':' && m_strObjectName[len-2] != ':' )
			m_strObjectName.erase(len-1);

		m_strObjectName += vecHidden[i];

		PR_TRACE((m_hTask, prtALWAYS_REPORTED_MSG, "ODS\tHidden object: %S", m_strObjectName.data()));

		cAutoString name(m_strObjectName);

		cAutoObj<cIO> io;

		cIOObj hFileIO(*m_hTask, name,
			fACCESS_READ|fACCESS_FORCE_READ|fACCESS_FORCE_OPEN|fACCESS_RESTORE_FILE_TIMES,
			fOMODE_OPEN_IF_EXIST|fOMODE_OPEN_IF_EXECUTABLE);

/*		if( PR_SUCC(hFileIO.last_error()) )
			io = hFileIO.relinquish();
		else
*/		{
			PR_TRACE((m_hTask, prtALWAYS_REPORTED_MSG, "ODS\tCannot open hidden object %S: err(%x)",
				m_strObjectName.data(), hFileIO.last_error()));

			cAutoObj<cIO> hFileIO;
			tERROR err = m_hTask->sysCreateObject(hFileIO, IID_IO, PID_ODS);
			if( PR_SUCC(err) )
				err = m_strObjectName.copy(hFileIO, pgOBJECT_FULL_NAME);
			if( PR_SUCC(err) )
				err = hFileIO->propSetPtr(plNTFS, m_ntfsChecker.m_hntfs);
			if( PR_SUCC(err) )
				err = hFileIO->sysCreateObjectDone();

			if( PR_SUCC(err) )
				io = hFileIO.relinquish();
			else
			{
				cIOObj hTmpIO(*m_hTask, name, fACCESS_RW, 0, PID_TMPFILE);
				if( PR_SUCC(hTmpIO.last_error()) )
					io = hTmpIO.relinquish();
			}
		}

		ProcessObject((cObject*)io.relinquish());
	}
	m_pDetectInfo = NULL;
	return true;
}

tERROR CObjectScanner::SendReport(tDWORD nClass, enObjectStatus nStatus, int nDesc)
{
	if( m_pThread->m_bCounter )
		return errOK;

	m_tmTimeStamp = cDateTime::now_utc();
	m_nObjectStatus = nStatus;
	m_nDescription = nDesc;
	return m_hTask->sysSendMsg(nClass, nStatus, NULL, (cDetectObjectInfo*)this, SER_SENDMSG_PSIZE);
}

// -------------------------------------------

CScannerThread::CScannerThread(cODSImpl * pTask, bool bCounter) :
	m_hTask(pTask),
	m_nObjCount(0),
	m_bCounter(bCounter),
	m_bCompleted(false),
	m_bReinited(false),
	m_pContext(NULL),
	m_hToken(NULL)
{
	if( m_bCounter )
		m_nObjCount++;

	m_tmStart = cDateTime::now_utc();
	m_nStartTicks = PrGetTickCount();

	m_aAlreadyScanObjectsCRC32.alloc(2*1024);
}

CScannerThread::~CScannerThread()
{
	wait_for_stop();

	for(tDWORD i = 0; i < m_aScaners.size(); i++)
		delete m_aScaners[i];

	if( m_hToken )
		m_hToken->sysCloseObject();
}

void CScannerThread::Start()
{
	m_hTask->sysCreateObjectQuick((hOBJECT*)&m_hToken, IID_TOKEN);
	cThreadTaskBase::start(*m_hTask, TP_THREADPRIORITY_NORMAL);
}

void CScannerThread::InitScanners()
{
	int i, nCount = m_hTask->m_Settings.m_aScanObjects.size();

	for(i = 0; i < nCount; i++)
	{
		cScanObject& pObject = m_hTask->m_Settings.m_aScanObjects[i];
		if( !m_bCounter )
			if( pObject.m_bEnabled )
				PR_TRACE((m_hTask, prtALWAYS_REPORTED_MSG, "ODS\tObject %S(%d) added to scan", pObject.m_strObjName.data(), pObject.m_nObjType));
			else
				PR_TRACE((m_hTask, prtALWAYS_REPORTED_MSG, "ODS\tObject %S(%d) not enabled", pObject.m_strObjName.data(), pObject.m_nObjType));

		if( !pObject.m_bEnabled )
			continue;

		CObjectScanner* pScanner = CObjectScanner::Create(this, pObject);
		if( !pScanner )
			continue;

		if( !m_bCounter )
			m_hTask->m_avsSession->RegisterScope(NULL, &pObject);

		m_aScaners.push_back(pScanner);
	}
}

void CScannerThread::ReinitState(cODSStatistics& pStatistics)
{
	if( pStatistics.m_nCurObject >= m_aScaners.size() )
		return;

	m_nCurObject = pStatistics.m_nCurObject;
	m_sReinitName = pStatistics.m_sReinitName;
	m_nReinitMask = pStatistics.m_nReinitMask;
	m_bReinited = true;

	if( !m_bCounter )
	{
		m_hTask->m_nStartPersent = pStatistics.m_nReinitCompletion;
		m_hTask->m_avsSession->ResetStatistic(&pStatistics, cTRUE);
	}
}

void CScannerThread::do_work()
{
	if( !m_bCounter && PR_FAIL(m_hTask->OnTaskStarted()) )
		return;

	if( m_hToken )
		m_hToken->Impersonate();

	tDWORD i, nCount = m_aScaners.size();

	for(i = 0; i < nCount; i++)
	{
		CObjectScanner* pScanner = m_aScaners[i];

		m_hTask->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_SYSTEM_ONLY_STRING,
			(hOBJECT)cAutoString(pScanner->m_strObjectName), 0, 0);

		pScanner->Process(true, true);
	}

	for( ;m_nCurObject < nCount; m_nCurObject++)
	{
		CObjectScanner* pScanner = m_aScaners[m_nCurObject];

		pScanner->Process(false, true);
		if( pScanner->m_bStop )
			break;
	}

	if( !m_bCounter )
		m_hTask->OnTaskCompleted();

	m_bCompleted = true;
}

bool CScannerThread::IsShoudBeProcessed(cStrObj& strName)
{
	if ( !m_hTask->m_Settings.m_bUseIChecker )
		return true;

	tDWORD name_crc = iCountCRC32(strName.size()*sizeof(tWCHAR), (const unsigned char*)strName.data(), 0);
	tDWORD dwSize = m_aAlreadyScanObjectsCRC32.size();
	
	bool bFound = false;
	for (tDWORD i = 0; i<dwSize; ++i)
		if (m_aAlreadyScanObjectsCRC32[i] == name_crc)
			return false;

	m_aAlreadyScanObjectsCRC32.push_back(name_crc);

	return true;
}

// -------------------------------------------
