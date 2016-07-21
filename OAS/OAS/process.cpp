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

#include <Prague/iface/e_loader.h> 
#include <Prague/plugin/p_win32_nfio.h>

#include <Extract/plugin/p_btdisk.h>

// -------------------------------------------

tERROR CInterceptedObject::Prepare(hSTRING hObject)
{
	tERROR error = m_pTask->sysCheckObject((hOBJECT)hObject, IID_STRING);
	if( PR_FAIL(error) )
	{
		PR_TRACE((m_pTask, prtERROR, "OAS\tUnknown object in prepare"));
		return error;
	}

	m_hObject = hObject;
	m_sObjectName = hObject;
	m_bIsVolume = false;
	
	m_pUserCtx = (cUserInfo*)m_hObject->propGetDWord(OASImpl::m_propAccessedUser);
	m_pOwnerCtx = (cUserInfo*)m_hObject->propGetDWord(OASImpl::m_propModifiedUser);

	if( OID_AVP3_BOOT == m_hObject->propGetDWord(OASImpl::m_propProceedObjectOrigin) )
		m_bIsVolume = true;

	m_bIsDelayed = !!m_hObject->propGetBool(OASImpl::m_propDelayedProp);
	m_pSettings = m_bIsDelayed ? &m_TreatSettings : &m_DetectSettings;
	m_nActionPID = m_hObject->propGetDWord(OASImpl::m_propProcessID);

	if( m_SettingsId != m_pTask->m_SettingsId )
	{
		m_pTask->Lock();
		m_SettingsId = m_pTask->m_SettingsId;

		m_DetectSettings = m_pTask->m_Settings;
		m_DetectSettings.m_nScanAction = SCAN_ACTION_DETECT;

		if( m_DetectSettings.m_nPostponeSizeLimit.m_on )
		{
			if( !m_DetectSettings.m_nSizeLimit.m_on ||
					m_DetectSettings.m_nPostponeSizeLimit.m_val < m_DetectSettings.m_nSizeLimit.m_val )
				m_DetectSettings.m_nSizeLimit = m_DetectSettings.m_nPostponeSizeLimit;
		}

		m_TreatSettings = m_pTask->m_Settings;
		m_pTask->Unlock();
	}

	PR_TRACE((m_pTask, prtNOTIFY, "OAS\tPreparing to scan file(%S) - %s",
		m_sObjectName.data(), m_bIsDelayed ? "delayed" : "intercepted"));

	if( !m_bIsVolume )
		return errOK;

	cStringObj strDosName;
	if( PR_FAIL(strDosName.assign((iObj*)m_hObject, OASImpl::m_propDosDeviceName)) )
		return errOK;

	m_sObjectName.assign(strDosName, 0, 1);
	PR_TRACE((m_pTask, prtALWAYS_REPORTED_MSG, "OAS\tPreparing to scan  drive(%S)", m_sObjectName.data()));
	return errOK;
}

tERROR CInterceptedObject::PreProcess()
{
	tBOOL  bUseIStreams = cFALSE;
	tBOOL  bIncludeByMask = cFALSE;
	tDWORD nScopeId = -1;

	switch( m_pTask->GetDriveTypeG(m_sObjectName, (hOBJECT)m_hObject, &nScopeId) )
	{
		case LL_DRIVE_REMOVABLE:
		case LL_DRIVE_CDROM:
			if( (bIncludeByMask = !m_pSettings->m_bScanRemovable) && !m_pTask->m_bScanRemovable )
				return errOK_DECIDED;
			break;

		case LL_DRIVE_UNKNOWN:
		case LL_DRIVE_FIXED:
			bUseIStreams = m_pTask->m_Settings.m_bUseIStreams;
		case LL_DRIVE_RAMDISK:
			if( (bIncludeByMask = !m_pSettings->m_bScanFixed) && !m_pTask->m_bScanFixed )
				return errOK_DECIDED;
			break;

		case LL_DRIVE_REMOTE:
			if( (bIncludeByMask = !m_pSettings->m_bScanNetwork) && !m_pTask->m_bScanNetwork )
				return errOK_DECIDED;
			break;
	}

	m_nProcessStatusMask = 0;
	m_nObjectScopeId = nScopeId;

	m_pSettings->m_bUseIStreams = bUseIStreams;
	m_pSettings->m_bIncludeByMask = bIncludeByMask;

	m_hICheckObject = (hOBJECT)m_hObject;
	return m_pTask->m_avsSession->PreProcessObject(this, m_pSettings);
}

bool CInterceptedObject::IsIOToProcess(hIO pIO)
{
	m_qwFileSize = 0;
	pIO->GetSize(&m_qwFileSize, IO_SIZE_TYPE_EXPLICIT);

	return m_qwFileSize >= 4;
}

bool CInterceptedObject::IsNeedPostpone()
{
	if( m_bIsDelayed )
		return false;

	if( m_pTask->m_Settings.m_nSizeLimit.m_on )
		if( m_qwFileSize > m_pTask->m_Settings.m_nSizeLimit.m_val * 0x100000 )
		{
			PR_TRACE((m_pTask, prtNOTIFY, "oas\tArchive %S skipped by size", m_sObjectName.data()));
			return false;
		}

	if( m_DetectSettings.m_nPostponeSizeLimit.m_on )
		if( m_qwFileSize > m_DetectSettings.m_nPostponeSizeLimit.m_val * 0x100000 )
		{
			PR_TRACE((m_pTask, prtIMPORTANT, "oas\tArchive %S postponed", m_sObjectName.data()));
			return true;
		}

	return false;
}

void CInterceptedObject::BanUserIfNeeded()
{
	if( !m_pSettings->m_nBanPeriod.m_on ||
		!m_pSettings->m_nBanPeriod.m_val )
	{
		PR_TRACE((m_pTask, prtSPAM, "oas\tBan is off"));
		return;
	}

	if( !m_pOwnerCtx || m_pOwnerCtx->m_LocalUser )
	{
		PR_TRACE((m_pTask, prtNOTIFY, "oas\tBan impossible - no owner or local user"));
		return;
	}

	cUserBanItem item;
	(cUserInfo&)item = *m_pOwnerCtx;
	item.m_StartTime = cDateTime::now_utc();
	item.m_StopTime = item.m_StartTime + m_pSettings->m_nBanPeriod.m_val*60*60;

    m_pTask->AddUserBan(&item, true);

	PR_TRACE((m_pTask, prtIMPORTANT, "oas\tBan sended"));
}

// -------------------------------------------

CDisinfectTask::CDisinfectTask(CInterceptedObject &o):
	CInterceptedObject(o.m_pTask),
	m_hToken(NULL)
{
	m_sObjectName = o.m_sObjectName;
	m_bIsVolume = o.m_bIsVolume;
	m_nObjectScopeId = o.m_nObjectScopeId;
	m_NotifyInfo= o.m_NotifyInfo;

	if( o.m_pUserCtx ) m_UserCtx = *o.m_pUserCtx, m_pUserCtx = &m_UserCtx;
	if( o.m_pOwnerCtx ) m_OwnerCtx = *o.m_pOwnerCtx, m_pOwnerCtx = &m_OwnerCtx;

	if( o.m_nActionSessionId != -1 )
	{
		m_nActionSessionId = o.m_nActionSessionId;
		m_nActionPID = o.m_nActionPID;
	}
	else
	{
		m_nActionPID = o.m_hObject->propGetDWord(OASImpl::m_propProcessID);
		m_nActionSessionId = PrSessionId(m_nActionPID);
	}

	m_pTask->sysCreateObjectQuick((hOBJECT*)&m_hToken, IID_TOKEN);

	m_pTask->Lock();
	m_TreatSettings = m_pTask->m_Settings;
	m_TreatSettings.m_nSizeLimit.m_on = cFALSE;
	m_pSettings = &m_TreatSettings;
	m_pTask->Unlock();
} 

CDisinfectTask::~CDisinfectTask()
{
	if( m_hToken )
		m_hToken->sysCloseObject();
}

bool CDisinfectTask::StartTask()
{
	PR_TRACE((m_pTask, prtNOTIFY, "OAS\tStarting delayed task"));
	cThreadTaskBase::start(*m_pTask);
	return true;
}

bool CDisinfectTask::is_equal(cThreadTaskBase* pTask)
{
	CDisinfectTask* pToCheck = (CDisinfectTask*)pTask;
	if( pToCheck->m_sObjectName == m_sObjectName && 
		pToCheck->m_bIsVolume == m_bIsVolume )
		return true;

	return false;
}

void CDisinfectTask::do_work()
{
	PR_TRACE((m_pTask, prtNOTIFY, "OAS\tOn run disinfect task"));
	if( m_bIsVolume )
		ProcessDrive();
	else
	{
		m_pTask->SendNotification(*this);

		if( ProcessFile() == errLOCKED )
			m_pTask->AddLockedTask(new CLockedTask(*this));
	}
}

tERROR CDisinfectTask::ProcessFile()
{
	if( m_hToken )
		m_hToken->Impersonate();

	cIOObj hFileIO(*m_pTask, cAutoString (m_sObjectName), fACCESS_READ, fOMODE_OPEN_IF_EXIST|fOMODE_OPEN_IF_EXECUTABLE);

	tERROR error = hFileIO.last_error();
	if( PR_SUCC(error) )
	{
		cInfectedObjectInfo pInfo;
		error = m_pTask->m_avsSession->ProcessObject((hOBJECT)hFileIO, this, m_pSettings, &pInfo);
	}

	if( m_nProcessStatusMask & cScanProcessInfo::DELETED )
		hFileIO.relinquish();
	else
		hFileIO.clean();

	if( m_hToken )
		m_hToken->Revert();
	return error;
}

tERROR CDisinfectTask::ProcessDrive()
{
	if( !m_pSettings->m_bScanSectors )
		return errOK;

	cIOObj hDiskIo(*m_pTask, cAutoString (m_sObjectName), fACCESS_READ, fOMODE_OPEN_IF_EXIST, PID_WINDISKIO);
	if( PR_FAIL(hDiskIo.last_error()) )
		return hDiskIo.last_error();

	tDWORD dwSecPerTrac = hDiskIo->propGetDWord(plSectorsPerTrack);
	if( dwSecPerTrac )
	{
		cStrObj strDevName(m_sObjectName);
		strDevName += L":\\";
		m_pTask->GetDriveTypeG(strDevName, NULL, (tDWORD*)&m_nObjectScopeId);

		m_pTask->m_avsSession->ProcessObject((hOBJECT)hDiskIo, this, m_pSettings, NULL);

		tDWORD dwDriveType = hDiskIo->propGetDWord(plDriveType);

		if( (m_nProcessStatusMask & cScanProcessInfo::DETECTED) &&
			(dwDriveType == cFILE_DEVICE_UNKNOWN || dwDriveType == cFILE_DEVICE_DISK) )
			ProcessPhysicalDisks();

		if( dwDriveType == cFILE_DEVICE_CD_ROM || dwDriveType == cFILE_DEVICE_DVD )
			ProcessBTDisks();
	}

	return errOK;
}

tERROR CDisinfectTask::ProcessPhysicalDisks()
{
	cAutoObj<cObjPtr> hDiskPtr;
	tERROR error = m_pTask->sysCreateObjectQuick((hOBJECT*)&hDiskPtr, IID_OBJPTR, PID_WINDISKIO);
	if( PR_FAIL(error) )
		return error;

	while( PR_SUCC(hDiskPtr->Next()) )
	{
		cAutoObj<cIO> hDiskIo;
		error = hDiskPtr->IOCreate(&hDiskIo, NULL, fACCESS_READ, fOMODE_OPEN_IF_EXIST);
		if( PR_SUCC(error) )
		{
			tDWORD dwSecPerTrac = hDiskIo->propGetDWord(plSectorsPerTrack);
			if( dwSecPerTrac )
				m_pTask->m_avsSession->ProcessObject((hOBJECT)hDiskIo, this, m_pSettings, NULL);
		}
	}
	return error;
}

tERROR CDisinfectTask::ProcessBTDisks()
{
	cAutoObj<cOS> hBtOS;
	cAutoObj<cObjPtr> hPtr;

	tERROR error = m_pTask->sysCreateObject((cObject**) &hBtOS, IID_OS, PID_BTDISK);
	if( PR_SUCC(error) )
		error = m_sObjectName.copy( hBtOS.obj(), pgOBJECT_NAME, 0, 1 );
	if( PR_SUCC(error) )
		error = hBtOS->sysCreateObjectDone();

	if( PR_SUCC(error) )
		error = hBtOS->PtrCreate(&hPtr, NULL);

	if( PR_FAIL(error) )
		return error;

	while( PR_SUCC(hPtr->Next()) )
	{
		cAutoObj<cIO> hDiskIo;
		if( PR_SUCC(hPtr->IOCreate(&hDiskIo, 0, fACCESS_READ, fOMODE_OPEN_IF_EXIST)) )
			m_pTask->m_avsSession->ProcessObject((hOBJECT)hDiskIo, this, m_pSettings, NULL);
	}
	return errOK;
}

// -------------------------------------------
