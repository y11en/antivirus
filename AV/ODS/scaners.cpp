// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- task.cpp
// -------------------------------------------

#include "scaners.h"
#include <Sign/SIGN.h>

#include <Prague/pr_time.h>
#include <Prague/plugin/p_tempfile.h>

#define IMPEX_IMPORT_LIB
#include <Prague/iface/e_loader.h> 
#include <Extract/plugin/p_windiskio.h>
#include <Prague/plugin/p_win32_nfio.h>

// -------------------------------------------
// File scaner

void CFileScanner::Process(bool bPreProcess, bool bIsOriginalObject)
{
	if( !bPreProcess && !PreProcessObject(true) )
	{
		cIOObj hFileIO(*m_hTask, cAutoString (m_strObjectName),
			fACCESS_READ|fACCESS_FORCE_READ|fACCESS_FORCE_OPEN|fACCESS_RESTORE_FILE_TIMES,
			fOMODE_OPEN_IF_EXIST|fOMODE_OPEN_IF_EXECUTABLE);

		if( PR_SUCC(hFileIO.last_error()) )
			ProcessObject((cObject*)hFileIO.relinquish(), true);
		else
			CheckOpenError(hFileIO.last_error());

		if( bIsOriginalObject && !CheckStop() && GetVirtualObject(m_strObjectName, m_strObjectName) )
		{
			PR_TRACE((0, prtNOTIFY, "ods\tAdding virtual object: %S", m_strObjectName.data()));
			Process(bPreProcess, false);
		}
	}
}

void CFileScanner::CheckOpenError(tERROR error)
{
	int nDesc = 0;
	switch(error)
	{
	case errLOCKED:           nDesc = NPREASON_LOCKED; break;
	case errACCESS_DENIED:    nDesc = NPREASON_NORIGHTS; break;
	case errOBJECT_NOT_FOUND: nDesc = NPREASON_NOTFOUND; break;
	}

	if( nDesc )
		SendReport(pmc_PROTECTION_EVENTS_NOTIFY, OBJSTATUS_NOTPROCESSED, nDesc);
}

// -------------------------------------------
// Folder scanner
void CFolderScanner::Process(bool bPreProcess, bool bIsOriginalObject)
{
	REINIT_CHECKPOINT(CFolderScanner, 0x80000001, bPreProcess)

	cAutoObj<cObjPtr> hFilePtr;
	tERROR error = m_hTask->sysCreateObject((hOBJECT*)&hFilePtr, IID_OBJPTR, PID_NATIVE_FIO);
	cStringObj strInitialObject = m_strObjectName;

	if( PR_SUCC(error) )
	{
		if( m_nScannerType == OBJECT_TYPE_MASKED )
		{
			tDWORD mask = m_strObjectName.find_last_of(L"\\/");
			if( mask != cStrObj::npos )
			{
				if( cStrObj::npos != m_strObjectName.find_first_of(L"*.", mask) )
					m_strMask.assign(m_strObjectName, mask + 1);
			}

			if( m_strMask.empty() )
				error = m_strObjectName.copy(hFilePtr, pgOBJECT_PATH);
			else
			{
				m_strMask.copy(hFilePtr, pgMASK);
				m_strObjectName = m_strObjectName.substr(0, mask);
				error = m_strObjectName.copy((iObj*)hFilePtr, pgOBJECT_PATH);
			}
		}
		else
			error = m_strObjectName.copy(hFilePtr, pgOBJECT_PATH);
	}

	if( PR_SUCC(error) )
		error = hFilePtr->sysCreateObjectDone();

	if( PR_SUCC(error) )
	{
		cStrObj strPathFrom;
		if( m_pThread->m_bReinited )
		{
			strPathFrom.assign(m_pThread->m_sReinitName, m_strObjectName.size());
			if( strPathFrom.at(0) == '\\' )
				strPathFrom.erase(0, 1);
			m_pThread->m_bReinited = false;
		}

		ProcessFolder(hFilePtr, strPathFrom);
	}
	else
		CheckOpenError(error);

	if( bIsOriginalObject && !CheckStop() && GetVirtualObject(strInitialObject, m_strObjectName) )
	{
		PR_TRACE((0, prtNOTIFY, "ods\tAdding virtual object: %S", m_strObjectName.data()));
		Process(bPreProcess, false);
	}
}

void CFolderScanner::ProcessFolder(hObjPtr hFilePtr, cStrObj& strPathFrom)
{
	tDWORD nFromSize = strPathFrom.size(), nFilePos = cStrObj::npos;

	if( nFromSize )
	{
		PR_TRACE((m_hTask, prtIMPORTANT, "ods\tCFolderScanner::ProcessFolder: strPathFrom=%S", strPathFrom.data()));
		nFilePos = strPathFrom.find_first_of("\\/");
	}
	else if( !m_pThread->m_bCounter )
	{
		m_strObjectName.check_last_slash();
		if( PreProcessObject(false, hFilePtr) )
			return;

		m_bNtfsCheker = (m_hTask->m_Settings.m_bRootkitScan && m_strMask.empty());
		ProcessNTFSStreams((hOBJECT)hFilePtr, true);
		hFilePtr->sysCreateObjectQuick(&m_ProcessInfo.m_hSessionObject, IID_BUFFER, PID_ANY);
	}
	else
		PreProcessObject();

	PR_TRACE((m_hTask, prtIMPORTANT, "ods\tCFolderScanner::ProcessFolder:%S", m_strObjectName.data()));

	// process files
	bool bIsFolders = true;
	if( nFilePos == cStrObj::npos )
	{
		tERROR errnext = errOK;
		bIsFolders = false;

		while(!CheckStop() && PR_SUCC(errnext = hFilePtr->Next()) )
		{
			m_strObjectName.assign(hFilePtr, pgOBJECT_FULL_NAME);

			if( m_bNtfsCheker )
				m_ntfsChecker.AddObject(m_strObjectName);

			if( hFilePtr->propGetBool(pgIS_FOLDER) )
			{
				bIsFolders = true;
				continue;
			}

			if( nFromSize && (m_strObjectName.size() < nFromSize ||
					m_strObjectName.compare(m_strObjectName.size()-nFromSize, nFromSize, strPathFrom)) )
				continue;

			nFromSize = 0;

			if( PreProcessObject(true, hFilePtr) )
				continue;

			cAutoObj<cIO> hScanIO;
			tERROR err = hFilePtr->IOCreate(&hScanIO, NULL,
				fACCESS_READ|fACCESS_FORCE_READ|fACCESS_FORCE_OPEN|fACCESS_RESTORE_FILE_TIMES|fACCESS_FILE_FLAG_NO_BUFFERING,
				fOMODE_OPEN_IF_EXIST|fOMODE_OPEN_IF_EXECUTABLE|fOMODE_OPEN_IF_EXIST);

			if( PR_SUCC(err) )
				ProcessObject((cObject*)hScanIO.relinquish(), true);
			else
				CheckOpenError(err);
		}

		if( errnext == errEND_OF_THE_LIST )
			ProcessHiddenObjects(hFilePtr);

		CheckOpenError(errnext);
		strPathFrom.erase();
	}

	m_bNtfsCheker = false;

	if( !m_pThread->m_bCounter )
		m_ProcessInfo.m_hSessionObject = NULL;

	if( !bIsFolders || (!m_pScanObj.m_bRecursive && m_nScannerType != OBJECT_TYPE_MY_COMPUTER) )
		return;

	// process folders
	hFilePtr->Reset(cFALSE);
	while(!CheckStop() && PR_SUCC(hFilePtr->Next()) )
	{
		if( !hFilePtr->propGetBool(pgIS_FOLDER) )
			continue;

		m_strObjectName.assign(hFilePtr, pgOBJECT_FULL_NAME);
		if( nFromSize && nFilePos != cStrObj::npos &&
				( m_strObjectName.size() < nFilePos || 
				  m_strObjectName.compare(m_strObjectName.size()-nFilePos, nFilePos, strPathFrom, 0, nFilePos)) )
			continue;

		cAutoObj<cObjPtr> hClone;
		tERROR error = hFilePtr->sysCreateObject((cObject**)&hClone, IID_OBJPTR, PID_NATIVE_FIO);

		if( PR_SUCC(error) )
			error = m_strObjectName.copy(hClone, pgOBJECT_PATH);

		if( PR_SUCC(error) && !m_strMask.empty() )
			m_strMask.copy(hClone, pgMASK);

		if( PR_SUCC(error) )
			error = hClone->sysCreateObjectDone();

		if( PR_SUCC(error) )
		{
			if( nFromSize )
				strPathFrom.erase(0, nFilePos+1);

			ProcessFolder(hClone, strPathFrom);
			nFromSize = 0;
		}
	}
}

// -------------------------------------------
// Sectors scaner

void CSectorsScanner::Process(bool bPreProcess, bool bIsOriginalObject)
{
	if( !m_hTask->m_Settings.m_bScanSectors )
		return;

	if( bPreProcess )
	{
		if( !m_pThread->m_pContext )
			m_pThread->m_pContext = (CSectorsScanner*)this;

		cStringObjVector& vecDrives = ((CSectorsScanner*)m_pThread->m_pContext)->m_vecDrives;
		for(tDWORD i = 0; i < vecDrives.size(); i++)
			if( vecDrives[i] == m_strObjectName )
				return;

		vecDrives.push_back(m_strObjectName);
		return;
	}

	CReinitCheckpoint<CSectorsScanner> ri(this, 0x00000002);

	tDWORD nCount = m_vecDrives.size();
	if( !nCount )
		return;

	if( ri.verify() )
	{
		bool bMbrToProcess = false;
		for(tDWORD i = 0; i < nCount; i++)
		{
			cAutoObj<cIO> hDiskIo;
			tERROR error = m_hTask->sysCreateObject((hOBJECT*)&hDiskIo, IID_IO, PID_WINDISKIO);
			if( PR_SUCC(error) )
				error = m_vecDrives[i].copy((iObj*)hDiskIo, pgOBJECT_NAME, 0, 1);

			if( PR_SUCC(error) )
				error = hDiskIo->sysCreateObjectDone();

			if( PR_FAIL(error) )
				continue;

			tDWORD dwSecPerTrac = hDiskIo->propGetDWord(plSectorsPerTrack);
			tDWORD dwMediaType = hDiskIo->propGetDWord(plMediaType);

			if( dwSecPerTrac && !PreProcessObject() )
				ProcessObject((cObject*)hDiskIo.relinquish());

			if( dwMediaType == cMEDIA_TYPE_FIXED )
				bMbrToProcess = true;
		}

		if( bMbrToProcess )
			ProcessPhysicalDisks();
	}

	m_vecDrives.clear();
}
	
tERROR CSectorsScanner::ProcessPhysicalDisks()
{
	cAutoObj<cObjPtr> hDiskPtr;
	tERROR error = m_hTask->sysCreateObjectQuick((hOBJECT*)&hDiskPtr, IID_OBJPTR, PID_WINDISKIO);
	if( PR_FAIL(error) )
		return error;

	while(!CheckStop() && PR_SUCC(hDiskPtr->Next()) )
	{
		if( PreProcessObject() )
			continue;

		cAutoObj<cIO> hDiskIo;
		error = hDiskPtr->IOCreate(&hDiskIo, NULL, fACCESS_READ, fOMODE_OPEN_IF_EXIST);
		if( PR_SUCC(error) )
		{
			tDWORD dwSecPerTrac = hDiskIo->propGetDWord(plSectorsPerTrack);
			if( dwSecPerTrac )
				ProcessObject((cObject*)hDiskIo.relinquish());
		}
	}
	return error;
}

// -------------------------------------------
// BTDisk scanner

#include <Extract/plugin/p_btdisk.h>

void CBTDisk::Process(bool bPreProcess, bool bIsOriginalObject)
{
	REINIT_CHECKPOINT(CBTDisk, 0x00000004, bPreProcess)

	cAutoObj<cOS> hBtOS;
	cAutoObj<cObjPtr> hPtr;

	tERROR error = m_hTask->sysCreateObject((cObject**) &hBtOS, IID_OS, PID_BTDISK);
	if( PR_SUCC(error) )
		error = m_strObjectName.copy((iObj*)hBtOS, pgOBJECT_NAME, 0, 1);
	if( PR_SUCC(error) )
		error = hBtOS->sysCreateObjectDone();

	if( PR_SUCC(error) )
		error = hBtOS->PtrCreate(&hPtr, NULL);

	if( PR_FAIL(error) )
		return;

	while( PR_SUCC(hPtr->Next()) )
	{
		if( PreProcessObject() )
			continue;

		cAutoObj<cIO> hDiskIo;
		if( PR_SUCC(hPtr->IOCreate(&hDiskIo, 0, fACCESS_READ, fOMODE_OPEN_IF_EXIST)) )
			ProcessObject((cObject*)hDiskIo.relinquish());
	}
}

// -------------------------------------------
// Drive scaner

void CDriveScanner::Process(bool bPreProcess, bool bIsOriginalObject)
{
	REINIT_CHECKPOINT(CDriveScanner, 0x00000008, !exDiskExLogical_GetDriveType)

	tDWORD dwType = LL_DRIVE_UNKNOWN;
	exDiskExLogical_GetDriveType(cAutoString (m_strObjectName), &dwType);

	bool bScanBTDisk = false;
	switch( dwType )
	{
	case LL_DRIVE_REMOVABLE :
	case LL_DRIVE_CDROM :
		if( !m_hTask->m_Settings.m_bScanRemovable ||
			m_nScannerType == OBJECT_TYPE_ALL_FIXED_DRIVES ||
			m_nScannerType == OBJECT_TYPE_ALL_NETWORK_DRIVES )
			return;
		bScanBTDisk = true;
		break;
	case LL_DRIVE_UNKNOWN :
	case LL_DRIVE_FIXED   :
	case LL_DRIVE_RAMDISK :
		if( !m_hTask->m_Settings.m_bScanFixed ||
			m_nScannerType == OBJECT_TYPE_ALL_REMOVABLE_DRIVES ||
			m_nScannerType == OBJECT_TYPE_ALL_NETWORK_DRIVES )
			return;

		if( dwType == LL_DRIVE_FIXED && m_nScannerType == OBJECT_TYPE_MY_COMPUTER )
			if( exDiskExIsSubst && PR_SUCC(exDiskExIsSubst(cAutoString (m_strObjectName))) )
				return;

		break;
	case LL_DRIVE_REMOTE :
		if( !m_hTask->m_Settings.m_bScanNetwork ||
			m_nScannerType == OBJECT_TYPE_ALL_REMOVABLE_DRIVES ||
			m_nScannerType == OBJECT_TYPE_ALL_FIXED_DRIVES ||
			m_nScannerType == OBJECT_TYPE_MY_COMPUTER )
			return;
		break;
	default : return;
	}

    if( m_pThread->m_hToken )
        m_pThread->m_hToken->Revert();

	CSectorsScanner::Process(bPreProcess, false);

	if( bScanBTDisk )
		CBTDisk::Process(bPreProcess, false);

    if( m_pThread->m_hToken )
        m_pThread->m_hToken->Impersonate();

	if( !bPreProcess && m_nScannerType != OBJECT_TYPE_BOOT_SECTORS )
		CFolderScanner::Process(bPreProcess, false);
}

bool CDriveScanner::ReinitCheck(tDWORD nMask)
{
	if( !CObjectScanner::ReinitCheck(nMask) )
		return false;

	return nMask != 0x00000008 || m_strObjectName.at(0) == m_pThread->m_sReinitName.at(0);
}

// -------------------------------------------
// Drives scaner

void CDrivesScanner::Process(bool bPreProcess, bool bIsOriginalObject)
{
	REINIT_CHECKPOINT(CDrivesScanner, 0x00000010, !exDiskExLogical_Next)

	cAutoString strDrive;
	while( PR_SUCC(exDiskExLogical_Next(strDrive)) )
	{
		m_strObjectName.assign(strDrive);
		CDriveScanner::Process(bPreProcess, false);
	}
}

// -------------------------------------------
// MailSystem's scaner
#include <Extract/plugin/p_msoe.h>
#include <Extract/plugin/p_mdb.h>
#include <Extract/iface/i_mailmsg.h>

void CMailSystemScanner::Process(bool bPreProcess, bool bIsOriginalObject)
{
	REINIT_CHECKPOINT(CMailSystemScanner, 0x00000020, bPreProcess)

	tBOOL bPlainMail = m_Settings.m_bScanPlainMail;
	tBOOL bMailBases = m_Settings.m_bScanMailBases;

	m_Settings.m_bScanPlainMail = true;
	m_Settings.m_bScanMailBases = true;

	ProcessMailSystem(PID_MDB);  // Outlook Scanning
	ProcessMailSystem(PID_MSOE); // MSOE Scanning

	m_Settings.m_bScanPlainMail = bPlainMail;
	m_Settings.m_bScanMailBases = bMailBases;
}

tERROR CMailSystemScanner::ProcessMailSystem(tPID pid)
{
	if (CheckStop())
		return errOK;

	cAutoObj<cOS> hMailOs;
	if(PR_SUCC(m_hTask->sysCreateObject((hOBJECT*)&hMailOs, IID_OS, pid)) )
	{
		PR_TRACE((m_hTask, prtIMPORTANT, "mail os hObject 0x%x", (hOBJECT) hMailOs));

		hMailOs->propSetDWord(pgOBJECT_OS_TYPE, MAIL_OS_TYPE_SYSTEM);
		if(PR_SUCC(hMailOs->sysCreateObjectDone()) )
		{
			if (CheckStop())
				return errOK;

			cAutoObj<cObjPtr> hOptr;
			if( PR_SUCC(hMailOs->PtrCreate(&hOptr, 0)) )
				ProcessMailObjects(hOptr);
		}
	}
	return errOK;
}

tERROR CMailSystemScanner::ProcessMailObjects(hObjPtr oe)
{
	oe->Reset(cFALSE);
	while(!CheckStop() && PR_SUCC(oe->Next()) )
	{
		if( oe->propGetBool(pgIS_FOLDER) )
		{
			cAutoObj<cObjPtr> hClone;
			if( PR_SUCC(oe->Clone(&hClone)) )
				if( PR_SUCC(hClone->StepDown()) )
					ProcessMailObjects(hClone);
		}
		else
		{
			if( PreProcessObject() )
				continue;

			cAutoObj<cIO> hIo;
			if( PR_SUCC(oe->IOCreate(&hIo, 0, fACCESS_READ, fOMODE_OPEN_IF_EXIST)) )
				ProcessObject((cObject*)hIo.relinquish());
		}
	}
	return errOK;
}
// -------------------------------------------
// SturtUp object's scaner

#define PROGRESS_MULTIPLIER  20

void CStartUpObjectsScanner::Process(bool bPreProcess, bool bIsOriginalObject)
{
	REINIT_CHECKPOINT(CStartUpObjectsScanner, 0x00000040, bPreProcess)

	if( m_pThread->m_bCounter )
	{
		m_pThread->m_nObjCount += 100 * PROGRESS_MULTIPLIER;
		return;
	}

	m_bDoCount = false;

	tERROR error = m_hTask->m_hTM->GetService(TASKID_TM_ITSELF, (hOBJECT)m_hTask, sid_TM_STARTUP_SVC, (hOBJECT*)&m_hStartupEnum);

	cAutoObj<cMsgReceiver> hMsgReciever;
	if( PR_SUCC(error) )
	{
		error = m_hTask->sysCreateObject((hOBJECT*)&hMsgReciever, IID_MSG_RECEIVER);

		if( PR_SUCC(error) )
			error = hMsgReciever->propSetPtr(pgRECEIVE_CLIENT_ID, this);
		if( PR_SUCC(error) )
			error = hMsgReciever->propSetPtr(pgRECEIVE_PROCEDURE, (void*)_MsgReceive);
		if( PR_SUCC(error) )
			hMsgReciever->sysCreateObjectDone();

		if( PR_SUCC(error) )
			error = hMsgReciever->sysRegisterMsgHandler(pmc_STARTUPENUM2, rmhDECIDER, (hOBJECT)hMsgReciever, IID_ANY,PID_ANY,IID_ANY,PID_ANY);
	}
	
	if( PR_SUCC(error) )
	{
		m_nInitObjCount = m_pThread->m_nObjCount;
		g_root->RegisterCustomPropId(&m_nCounterProp, npPROP_PROGRESS_COUNT_PROCENT, pTYPE_DWORD);
		
		error = m_hStartupEnum->StartEnum((hOBJECT)hMsgReciever, m_Settings.m_bActiveDisinfection);
	}

	if( m_hStartupEnum )
		m_hTask->m_hTM->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)m_hStartupEnum);

	m_bDoCount = true;
}

tERROR CStartUpObjectsScanner::ProcessFile(tWCHAR* strName, tDWORD nProgress)
{
	m_strObjectName = strName;
	if( !m_pThread->IsShoudBeProcessed(m_strObjectName) )
		return errOK;

	CFileScanner::Process(false, false);
	m_pThread->m_nObjCount = m_nInitObjCount + nProgress * PROGRESS_MULTIPLIER;
	return errOK;
}

tERROR CStartUpObjectsScanner::ProcessHost(tWCHAR* strName, tDWORD nAction)
{
	enObjectStatus nStatus;
	switch(nAction)
	{
	case SuspiciousDetect: nStatus = OBJSTATUS_SUSPICION; break;
	case DelData:          nStatus = OBJSTATUS_CURED; break;
	default: return errOK;
	}

	m_strTaskType = "vul";
	m_strObjectName = strName;
	m_strDetectName = "Host.Vulnerability";
	m_nDetectType = DETYPE_VULNERABILITY;
	m_nDetectStatus = DSTATUS_HEURISTIC;
	m_nDetectDanger = DETDANGER_MEDIUM;
	SendReport(pmc_PROTECTION_EVENTS_IMPORTANT, nStatus, 0);

	if( nStatus == OBJSTATUS_CURED )
		return errOK;

	if( m_Settings.m_nScanAction == SCAN_ACTION_DISINFECT )
		return m_Settings.m_bTryDisinfect ? errOK_DECIDED : errOK;

	if( m_Settings.m_nScanAction == SCAN_ACTION_REPORT )
		return errOK;

	cAskObjectAction askAction;
	(cDetectObjectInfo&)askAction = *(cDetectObjectInfo*)this;
	askAction.m_nActionsMask = ACTION_REPARE|ACTION_CANCEL;
	askAction.m_nActionsAll = ACTION_REPARE|ACTION_CANCEL;
	askAction.m_nApplyToAll = APPLYTOALL_NONE;
	askAction.m_nDefaultAction = ACTION_REPARE;

	tERROR error = m_hTask->sysSendMsg(pmcASK_ACTION, cAskObjectAction::DISINFECT, NULL, &askAction, SER_SENDMSG_PSIZE);
	if( error == errOK_DECIDED )
		error = askAction.m_nResultAction == ACTION_REPARE ? errOK_DECIDED : errNOT_OK;

	return error;
}

tERROR CStartUpObjectsScanner::_MsgReceive(hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT recieve_point, tPTR pbuff, tDWORD* blen)
{
	CStartUpObjectsScanner *pThis = (CStartUpObjectsScanner*)_this->propGetPtr(pgRECEIVE_CLIENT_ID);
	if( pThis->CheckStop() )
		return errOPERATION_CANCELED;

	if( msg_cls == pmc_STARTUPENUM2 && msg_id == mc_FILE_FOUNDED )
	{
		pACTION_MESSAGE_DATA pData = (pACTION_MESSAGE_DATA)pbuff;
		if( pData->m_ObjType == OBJECT_HOSTS_TYPE )
			return pThis->ProcessHost(pData->m_sFilePath, pData->m_dAction);
		else if( pData->m_dAction == ScanAct )
			return pThis->ProcessFile(pData->m_sFilePath, _this->propGetDWord(pThis->m_nCounterProp));
	}

	return errOK;;
}

// -------------------------------------------
// System Restore Scanner

void CSystemRestoreScanner::Process(bool bPreProcess, bool bIsOriginalObject)
{
	REINIT_CHECKPOINT(CSystemRestoreScanner, 0x00000080,
		bPreProcess || !exDiskExLogical_Next || (PrGetOsVersion() == OSVERSION_9X))

	if( m_pThread->m_hToken )
		m_pThread->m_hToken->Revert();

	cAutoString strDrive;
	while( PR_SUCC(exDiskExLogical_Next(strDrive)))
	{
		tDWORD dwType = LL_DRIVE_UNKNOWN;
		exDiskExLogical_GetDriveType(strDrive, &dwType);

		if( dwType != LL_DRIVE_FIXED )
			continue;

		m_strObjectName = strDrive;
		m_strObjectName += "System Volume Information";

		CFolderScanner::Process(bPreProcess, false);
	}

	if( m_pThread->m_hToken )
		m_pThread->m_hToken->Impersonate();
}

// -------------------------------------------
// Memory scaner
#include <MemScan/os.h>
#include <MemScan/objptr.h>
#include <MemScan/io.h>
#include <MemModScan/os.h>
#include <MemModScan/objptr.h>
#include <MemModScan/io.h>

void CMemoryScanner::Process(bool bPreProcess, bool bIsOriginalObject)
{
	REINIT_CHECKPOINT(CMemoryScanner, 0x00000100, bPreProcess)

	if( m_pThread->m_hToken )
		m_pThread->m_hToken->Revert();

	ProcessDOSMemory();

	cAutoObj<cOS> hProcessesOS;
	tERROR error = m_hTask->sysCreateObjectQuick((hOBJECT*)&hProcessesOS, IID_OS, PID_MEMSCAN);

	cAutoObj<cObjPtr> hProcessesObjPtr;
	if( PR_SUCC(error) )
		error = hProcessesOS->PtrCreate(&hProcessesObjPtr, NULL);

	while( !CheckStop() && PR_SUCC(error) && PR_SUCC(hProcessesObjPtr->Next()) )
	{ 
		if( hProcessesObjPtr->propGetDWord(plPID) == PrGetProcessId() )
			continue;

		cAutoObj<cOS> hModulesOS;
		if( PR_FAIL(hProcessesObjPtr->sysCreateObjectQuick((hOBJECT*)&hModulesOS, IID_OS, PID_MEMMODSCAN)) )
			continue;

		cAutoObj<cObjPtr> hModulesObjPtr;
		if( PR_SUCC(hModulesOS->PtrCreate(&hModulesObjPtr, 0)) )
		{
			//Modules in process loop
			for(m_nModuleNumber = 0; !CheckStop() && PR_SUCC(hModulesObjPtr->Next()); m_nModuleNumber++ )
			{
				ProcessMemoryModule(hModulesObjPtr);
				ProcessMemoryFile(hModulesObjPtr);
			}
		}
	}

	if( !m_hTask->m_bSomeUntreated && !m_pThread->m_bCounter && !CheckStop())
		m_hTask->sysSendMsg(pmc_ODS_SCANNERS, pm_MEMORY_SCANNED, NULL, NULL, NULL);

	if( m_pThread->m_hToken )
		m_pThread->m_hToken->Impersonate();
}

tERROR CMemoryScanner::ProcessMemoryModule(hObjPtr oe)
{
	m_strObjectName.assign(oe, pgOBJECT_FULL_NAME);
	if( PreProcessObject() )
		return errOK;

	cAutoObj<cIO> hModuleIO;
	if( PR_SUCC(oe->IOCreate(&hModuleIO, 0, fACCESS_RW, fOMODE_OPEN_IF_EXIST )) )
		ProcessObject((cObject*)hModuleIO.relinquish());

/*	int nResult = SignCheck((hOBJECT)(tSTRING)strProcessName.c_str(cCP_ANSI), cTRUE, NULL, 0, cTRUE, NULL);
	if( nResult == SIGN_OK )
	{
		SendReport(pmc_PROTECTION_EVENTS_NOTIFY, OBJSTATUS_NOTPROCESSED, NPREASON_BUDDY);
		return errNOT_SUPPORTED;
	}
*/
	return errOK;
}

#include <ProductCore/plugin/p_processmonitor.h>
#include <ProductCore/structs/ProcMonM.h>

tERROR CMemoryScanner::ProcessMemoryFile(hObjPtr oe)
{
	m_strObjectName.assign(oe, pgOBJECT_PATH);

	tDWORD len = m_strObjectName.length() + 1;

	if (m_strObjectName.CP() == cCP_UNICODE)
		len = len * 2;

	m_hTask->sysSendMsg (
		pmc_PROCESS_MONITOR,
		pm_PROCESS_MONITOR_ADD_ACTIVE_IMAGE,
		NULL,
		(tPTR) m_strObjectName.data(),
		&len
		);

	if( !m_pThread->IsShoudBeProcessed(m_strObjectName) )
		return errOK;

	if( m_pThread->m_hToken )
		m_pThread->m_hToken->Impersonate();

	CFileScanner::Process(false, false);

	if( m_pThread->m_hToken )
		m_pThread->m_hToken->Revert();
	return errOK;
}

tERROR CMemoryScanner::ProcessDOSMemory()
{
	if( PrGetOsVersion() != OSVERSION_9X )
		return errOK;

	if( PreProcessObject() )
		return errOK;

	cAutoObj<cIO> hDOSIO;
	tERROR error = m_hTask->sysCreateObject((hOBJECT*)&hDOSIO, IID_IO, TEMPIO_ID);
	if( PR_SUCC(error) )
		error = hDOSIO->propSetDWord(pgOBJECT_ACCESS_MODE, fACCESS_RW);
	if( PR_SUCC(error) )
		error = hDOSIO->propSetDWord(pgOBJECT_ORIGIN, OID_AVP3_DOS_MEMORY);
	if( PR_SUCC(error) )
		error = hDOSIO->sysCreateObjectDone();

	if( PR_SUCC(error) )
		ProcessObject((cObject*)hDOSIO.relinquish());

	return error;
}

// -------------------------------------------

void CMyComputerScanner::Process(bool bPreProcess, bool bIsOriginalObject)
{
	REINIT_CHECKPOINT(CMyComputerScanner, 0x00000200, false)

	CMemoryScanner::Process(bPreProcess, false);
	CStartUpObjectsScanner::Process(bPreProcess, false);
	CDrivesScanner::Process(bPreProcess, false);
}

// -------------------------------------------

#include <ProductCore/structs/s_qb.h>

void CQuarantineScanner::Process(bool bPreProcess, bool bIsOriginalObject)
{
	if( bPreProcess || !m_hTask->m_hTM )
		return;

	cQB* m_hQB;
	tERROR error = m_hTask->m_hTM->GetService(TASKID_TM_ITSELF, (hOBJECT)m_hTask, sid_TM_QUARANTINE, (hOBJECT*)&m_hQB);
	if( PR_FAIL(error) )
	{
		PR_TRACE((m_hTask, prtIMPORTANT, "Cannot get QB Service", error));
		return;
	}

	cQwordVector aIds;
	bool bAll = true;

	tDWORD nSize = m_strObjectName.size(), nCount = 0;
	if( !nSize )
		m_hQB->GetCount(&nCount, cCOUNT_TOTAL | cCOUNT_REFRESH);
	else
	{
		nCount = (nSize++) / 16; bAll = false;

		tCHAR* pIds = new tCHAR[nSize];
		m_strObjectName.copy(pIds, nSize);

		tQWORD qwId = 0;
		for(tDWORD i = 0; i < nSize; i++)
		{
			if( i && !(i % 16) )
				aIds.push_back(qwId), qwId = 0;

			tCHAR ch = pIds[i];
			ch -= ch >= '0' && ch <= '9' ? '0' : 'a' - 10;;
			*((tBYTE*)&qwId + 7 - (i % 16) / 2) |= (i & 1) ? ch : (ch << 4);
		}
		delete[] pIds;
	}

	for(tDWORD i = 0; i < nCount && !CheckStop(); i++)
	{
		if( PreProcessObject() )
			continue;

		cPosIO_SP*  hQBIO = NULL;
		cQBObject   pObjInfo;
		tOBJECT_ID  qwObjectID;

		if( bAll )
			error = m_hQB->GetObjectByIndex(&hQBIO, &qwObjectID, i, cFALSE, &pObjInfo);
		else
			error = m_hQB->GetObject(&hQBIO, qwObjectID = aIds[i], cFALSE, &pObjInfo);

		if( PR_FAIL(error) )
		{
			PR_TRACE((m_hTask, prtIMPORTANT, "Cannot open QB Object", error));
			continue;
		}

		hQBIO->propSetDWord(pgOBJECT_ORIGIN, OID_QUARANTINED_OBJECT);
		pObjInfo.m_strObjectName.copy(hQBIO, m_propVirtualName);

		enObjectStatus nOldStatus = pObjInfo.m_nObjectStatus, nNewStatus = nOldStatus;
		time_t tmReport = pObjInfo.m_tmTimeStamp;
		time_t tmBases = pObjInfo.m_tmScanningBases;

		if( nOldStatus != OBJSTATUS_SUSPICION )
			m_Settings.m_nAskActions |= ACTION_QUARANTINE;
		else
			m_Settings.m_nAskActions &= ~ACTION_QUARANTINE;

		m_ProcessInfo.m_nObjectScopeId = m_pThread->m_nCurObject;
		m_hTask->m_avsSession->ProcessObject((hOBJECT)hQBIO, &m_ProcessInfo, &m_Settings, &pObjInfo);

		if( m_ProcessInfo.m_nProcessStatusMask & cScanProcessInfo::DELETED )
			continue;

		hQBIO->sysCloseObject();

		if( m_ProcessInfo.m_nProcessStatusMask & cScanProcessInfo::CANCELED )
			continue;

		if( m_ProcessInfo.m_nProcessStatusMask & cScanProcessInfo::DETECTED )
		{
			if( m_ProcessInfo.m_nProcessStatusMask & cScanProcessInfo::DISINFECTED )
				nNewStatus = OBJSTATUS_DISINFECTED;
			else if( m_ProcessInfo.m_nProcessStatusMask & cScanProcessInfo::DETECTED_SURE )
				nNewStatus = OBJSTATUS_INFECTED;
			else
				nNewStatus = OBJSTATUS_SUSPICION;
		}
		else if( nOldStatus != OBJSTATUS_DISINFECTED
			&& nOldStatus != OBJSTATUS_FALSEALARM 
			&& !(m_ProcessInfo.m_nProcessStatusMask & cScanProcessInfo::CANCELED) )
		{
			if (nOldStatus == OBJSTATUS_INFECTED || nOldStatus == OBJSTATUS_SUSPICION)
				nNewStatus = OBJSTATUS_FALSEALARM;
			else 
				nNewStatus = OBJSTATUS_OK;
		}

		if( nNewStatus != nOldStatus )
		{
			if( (nNewStatus == OBJSTATUS_OK || nNewStatus == OBJSTATUS_FALSEALARM) && tmBases == -1 )
			{
				time_t tmNow = cDateTime::now_utc();
				if( tmReport + 3*24*60*60 > tmNow )
					continue;
			}

			pObjInfo.m_nObjectStatus = nNewStatus;
			m_hQB->UpdateObjectInfo(qwObjectID, &pObjInfo);

			if( nNewStatus != OBJSTATUS_OK && nNewStatus != OBJSTATUS_DISINFECTED && nNewStatus != OBJSTATUS_FALSEALARM )
				continue;

			if( m_Settings.m_nScanAction == SCAN_ACTION_DISINFECT )
			{
				if( PR_SUCC(m_hQB->RestoreObject(qwObjectID, (tWSTRING)pObjInfo.m_strObjectName.data(), cTRUE)) )
				{
					m_strObjectName = pObjInfo.m_strObjectName;
					SendReport(pmc_PROTECTION_EVENTS_NOTIFY, OBJSTATUS_RESTORED);
					m_hQB->DeleteObject(qwObjectID);
				}
			}
			else 
			{
				cAskObjectAction askAction;
				(cDetectObjectInfo&)askAction = pObjInfo;
				askAction.m_qwUniqueId = qwObjectID;
				askAction.m_nActionsAll = askAction.m_nActionsMask = ACTION_OK|ACTION_CANCEL;
				askAction.m_nDefaultAction = ACTION_CANCEL;
				m_hTask->sysSendMsg(pmcASK_ACTION, cAskObjectAction::RESTORE, NULL, &askAction, SER_SENDMSG_PSIZE);

				if( askAction.m_nResultAction == ACTION_OK )
				{
					m_strObjectName = pObjInfo.m_strObjectName;
					SendReport(pmc_PROTECTION_EVENTS_NOTIFY, OBJSTATUS_RESTORED);
				}
			}
		}
	}

	m_hTask->m_hTM->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)m_hQB);
}

// -------------------------------------------
