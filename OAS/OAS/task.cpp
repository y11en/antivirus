// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  19 February 2004,  15:38 --------
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

#define PR_IMPEX_DEF

// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end

// AVP Prague stamp begin( Includes for interface,  )
#include "task.h"
// AVP Prague stamp end

#ifdef _WIN32
#include <windows.h>
#endif

#include <Prague/iface/i_objptr.h> 

#include <Updater/updater.h>
#include <ProductCore/plugin/p_qb.h> 

#define IMPEX_IMPORT_LIB
#include <Prague/iface/e_loader.h> 

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
	#include <Prague/iface/e_loader.h> 
IMPORT_TABLE_END

#define  MAX_LOCKED_COUNTER		100
#define  TRY_LOCKED_TIMEOUT		1000

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Task". Static(shared) property table variables
// AVP Prague stamp end

OASImpl::OASImpl()
	: cThreadPoolBase( "OAS" ), m_PostponeThread( "OAS+POSTPONE" )
{
	m_PostponeThread.m_pTask = this;
}

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
tERROR OASImpl::ObjectInitDone() 
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInitDone method" );

	// Place your code here

	error = sysCreateObjectQuick((cObject**) &m_lock, IID_CRITICAL_SECTION);

	if( PR_SUCC(error) )
	    error = sysCreateObjectQuick((cObject**) &m_BanListLock, IID_CRITICAL_SECTION);

	if( PR_SUCC(error) )
		error = sysCreateObjectQuick((cObject**) &m_PostponeLock, IID_CRITICAL_SECTION);

	if( PR_SUCC(error) )
		error = m_PostponeThread.init( this, 1,  1000 * 30, TP_THREADPRIORITY_LOWEST, -1 );
	
	if( PR_SUCC(error) )
	    m_hTM->GetService(TASKID_TM_ITSELF, (cObject*)this, AVP_SERVICE_PROCESS_MONITOR, (cObject**)&m_hProcMon);

	if( PR_SUCC(error) )
	    error = sysCreateObject((hOBJECT*)&m_hAvpg, IID_AVPG2, PID_ANY);

	if( PR_SUCC(error) )
		error = sysRegisterMsgHandler(pmc_AVPG2, rmhDECIDER, m_hAvpg, IID_ANY, PID_ANY, IID_ANY, PID_ANY);

    if( PR_SUCC(error) )
        error = sysRegisterMsgHandler(pmc_PROCESSING, rmhDECIDER, sysGetRoot(), IID_ANY, PID_ANY, IID_ANY, PID_ANY);

	if( PR_SUCC(error) && m_hTM )
		error = sysRegisterMsgHandler(pmc_QBSTORAGE_REPLICATED, rmhLISTENER, m_hTM, IID_ANY, PID_ANY, IID_ANY, PID_ANY);

	if( PR_SUCC(error) )
		error = m_hAvpg->propSetDWord(pgWorkingThreadMaxCount, 60);

	if( PR_SUCC(error) )
		error = m_hAvpg->sysCreateObjectDone();

	if( PR_SUCC(error) )
		error = m_hTM->GetService(TASKID_TM_ITSELF, (cObject*)this, sid_TM_AVS, (cObject**)&m_hAvs);

	if( PR_SUCC(error) )
		error = m_hAvs->CreateSession(&m_avsSession, (hTASK)this, cFALSE, OBJECT_ORIGIN_GENERIC);

	if( PR_SUCC(error) )
		error = sysRegisterMsgHandler(pmc_AVS, rmhLISTENER, m_hAvs, IID_ANY, PID_ANY, IID_ANY, PID_ANY);

	if( PR_SUCC(error) )
		error = sysRegisterMsgHandler(pmc_UPDATER, rmhLISTENER, m_hTM, IID_ANY, PID_ANY, IID_ANY, PID_ANY);

	if( PR_SUCC(error) )
		error = cThreadPoolBase::init(this, 1, TRY_LOCKED_TIMEOUT);

    if( PR_SUCC(error) )
	    error = sysRegisterMsgHandler(pmc_REMOTE_GLOBAL, rmhLISTENER, g_root, IID_ANY, PID_ANY, IID_ANY, PID_ANY );

    if( PR_SUCC(error) )
		error = sysSendMsg(pmc_REMOTE_GLOBAL, pm_BANLIST_NEWRECEIVER, this, NULL, NULL);

	m_nNetScopeId = -1;
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
tERROR OASImpl::ObjectPreClose() 
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectPreClose method" );

	// Place your code here
	ClearPostpone();
	m_PostponeThread.de_init();

	if( m_hAvpg )
		m_hAvpg->sysCloseObject();

	cThreadPoolBase::de_init();

	if( m_avsSession && PR_SUCC(sysCheckObject((hOBJECT)m_avsSession)) )
		m_avsSession->sysCloseObject();

	if( m_hAvs && PR_SUCC(sysCheckObject((hOBJECT)m_hAvs)) )
		m_hTM->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)m_hAvs);

	if( m_hProcMon )
		m_hTM->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)m_hProcMon);

	ClearDriveCache();
	ClearLockedTasks();
	
	return error;
}
// AVP Prague stamp end

tERROR OASImpl::on_idle()
{
	if( !m_pLockedHead )
		return errOK;

	CLockedTask *pTask = m_pLockedHead;

	m_pLockedHead = m_pLockedHead->m_pNext;
	if( m_pLockedHead )
		m_pLockedHead->m_pPrev = NULL;
	else
		m_pLockedTail = NULL;
		
	if( errLOCKED == pTask->ProcessFile() && ++pTask->m_nLockedCount < MAX_LOCKED_COUNTER )
		AddLockedTask(pTask);
	else
		delete pTask;

	return errOK;
}

void  OASImpl::LoadPlugins()
{
#ifdef _WIN32
	static tBOOL bLoaded = cFALSE;
	if( bLoaded )
		return;

	bLoaded = cTRUE;

	tCHAR strPath[MAX_PATH];
	if( !GetModuleFileName(NULL, strPath, sizeof(strPath)) )
		return;

	cStrObj sExePath(strPath);
	cIOObj io(*this, cAutoString(sExePath), fACCESS_READ);
	if( PR_FAIL(io.last_error()) )
		return;

	cScanProcessInfo info;
	cOASSettings Settings(m_Settings);
	Settings.m_bScanUnchangedObjects = true;
	Settings.m_bUseIChecker = false;
	Settings.m_bUseIStreams = false;

	cUserInfo pUserInfo;

	tDWORD pos = sExePath.find_last_of(L"\\/");
	if( pos != cStrObj::npos )
		pUserInfo.m_sUserName.assign(sExePath, pos+1);
	pUserInfo.m_sMachineName = L"localhost";
	info.m_pUserCtx = &pUserInfo;
	info.m_nActionPID = -1;

	GetDriveTypeG(sExePath, NULL, (tDWORD*)&info.m_nObjectScopeId);
	m_avsSession->ProcessObject(io, &info, &Settings, NULL);
#endif
}

void  OASImpl::AddLockedTask(CLockedTask * pTask)
{
	pTask->m_pNext = NULL;
	pTask->m_pPrev = m_pLockedTail;
	if( m_pLockedTail )
		m_pLockedTail->m_pNext = pTask;
	m_pLockedTail = pTask;
	if( !m_pLockedHead )
		m_pLockedHead = pTask;
}

void  OASImpl::ClearLockedTasks()
{
	CLockedTask *pTask = m_pLockedHead, *pTaskToKill;
	m_pLockedHead=0;
	while( pTask )
	{
		pTaskToKill = pTask;
		pTask = pTask->m_pNext;
		delete pTaskToKill;
	}
}

tDWORD OASImpl::GetDriveTypeG(cStrObj& sName, hOBJECT hObject, tDWORD* pScope)
{
#ifdef _WIN32
	tDWORD nDriveType;

	if( sName.size() > 2 && sName.at(0) == '\\' && sName.at(1) == '\\' )
	{
		if( m_nNetScopeId == -1 )
		{
			cScanObject pObject;
			pObject.m_nObjType = OBJECT_TYPE_ALL_NETWORK_DRIVES;
			m_avsSession->RegisterScope(&m_nNetScopeId, &pObject);
		}

		if( pScope )
			*pScope = m_nNetScopeId;

		return sName.at(2) != '?' ? LL_DRIVE_REMOTE : LL_DRIVE_FIXED;
	}

	Lock();

	CDriveCache *pDrive;
	bool bNew = false;

	for(pDrive = m_DriveCache; pDrive; pDrive = pDrive->m_pNext)
		if( !pDrive->m_sDriveName.compare(0, 2, sName, 0, 2, fSTRING_COMPARE_CASE_INSENSITIVE) )
			break;

	if( !pDrive )
	{
		bNew = true;

		pDrive = new CDriveCache();
		pDrive->m_sDriveName.assign(sName, 0, 2);
		pDrive->m_nDriveType = -1;
		pDrive->m_nScopeId = -1;

		if( pDrive->m_sDriveName.at(0) != '\\' )
		{
			cScanObject pObject;
			if (pDrive->m_sDriveName.at(1) == ':')
			{
				pObject.m_strObjName = pDrive->m_sDriveName;
			}
			else
			{
				tDWORD nPos = sName.find("\\");
				if (nPos == cStrObj::npos)
					pObject.m_strObjName = sName;
				else
					pObject.m_strObjName.assign(sName, 0, nPos);
			}
			pObject.m_strObjName += L'\\';
			pObject.m_nObjType = OBJECT_TYPE_DRIVE;
			m_avsSession->RegisterScope(&pDrive->m_nScopeId, &pObject);
		}
	}

	if( pScope )
		*pScope = pDrive->m_nScopeId;

	nDriveType = pDrive->m_nDriveType;
	Unlock();

	if( nDriveType == -1 )
	{
        //exDiskExLogical_GetDriveType(cAutoString(pDrive->m_sDriveName), &nDriveType);
        nDriveType = GetDriveTypeW(pDrive->m_sDriveName.c_str(cCP_UNICODE));

		pDrive->m_nDriveType = nDriveType;

		if( bNew )
		{
			Lock();
			pDrive->m_pNext = m_DriveCache;
			m_DriveCache = pDrive;
			Unlock();
		}
	}

	return nDriveType;
#else
    if (pScope) *pScope = -1;
    return LL_DRIVE_FIXED;
#endif
}

void OASImpl::ClearDriveCache(bool bReset)
{
	Lock();

	CDriveCache *pDrive = m_DriveCache, *next;
	while(pDrive)
	{
		next = pDrive->m_pNext;
		if( bReset )
			pDrive->m_nDriveType = -1;
		else
			delete pDrive;
		pDrive = next;
	}
	if( !bReset )
		m_DriveCache = NULL;

	Unlock();
}

void OASImpl::PutObjectInQueue(CInterceptedObject &o)
{
	CDisinfectTask* pTask = new CDisinfectTask(o);

	if( !check_task_in_queue(pTask) )
		pTask->StartTask();
	else
		delete pTask;
}

void OASImpl::SendNotification(CInterceptedObject &o)
{
	if( !m_Settings.m_bSendNotification )
		return;

	Lock();
	o.m_NotifyInfo.m_strTemplate = m_Settings.m_sSendTemplate;
	o.m_NotifyInfo.m_strObjectName = o.m_sObjectName;
	Unlock();

	sysSendMsg(pmc_USERNOTIFY, 0, NULL, &o.m_NotifyInfo, SER_SENDMSG_PSIZE);
}

void
OASImpl::FreePostponeItem (
	cPostpone* pPostpone
	)
{
	pPostpone->m_User.cUserInfo::~cUserInfo();
	pPostpone->m_UserCr.cUserInfo::~cUserInfo();

	if ( pPostpone->m_pwchDeviceName )
	{
		pPostpone->m_hCtx->heapFree( pPostpone->m_pwchDeviceName );
		pPostpone->m_pwchDeviceName = NULL;
	}

	if (pPostpone->m_hToken)
	{
		pPostpone->m_hToken->sysCloseObject();
		pPostpone->m_hToken = NULL;
	}

	heapFree( pPostpone );
}

void
OASImpl::AddPostpone (
	hSTRING hObject
	)
{
	// check existing, add if not exist, return allow
	if (!m_PostponeThread.is_inited())
		return;

	tERROR error_tmp = errOK;
	m_PostponeLock->Enter( SHARE_LEVEL_WRITE);
	if (m_bPostponeDisabled)
	{
		m_PostponeLock->Leave( NULL );
		return;
	}
	
	cPostpone* pPostpone = m_pPostpone;
	while (pPostpone)
	{
		error_tmp = pPostpone->m_hCtx->Compare (
			cSTRING_WHOLE,
			hObject,
			cSTRING_WHOLE,
			fSTRING_COMPARE_CASE_INSENSITIVE
			);

		if (PR_SUCC( error_tmp ))
			break;

		pPostpone = pPostpone->m_pNext;
	}

	if (!pPostpone) // not found existing, add new item
	{
		error_tmp = heapAlloc( (tPTR*) &pPostpone, sizeof(cPostpone) );
		if (PR_SUCC( error_tmp ))
		{
			PR_TRACE(( hObject, prtIMPORTANT, "oas\tadd postpone '%tstr'", hObject ));

			new (&pPostpone->m_User) cUserInfo();
			new (&pPostpone->m_UserCr) cUserInfo();
			
			hSTRING hStrTmp = NULL;

			error_tmp = sysCreateObjectQuick( (hOBJECT*) &hStrTmp, IID_STRING );

			if (PR_SUCC( error_tmp ))
				error_tmp = hStrTmp->ImportFromStr( NULL, hObject, cSTRING_WHOLE );

			if (PR_SUCC( error_tmp ))
			{
				error_tmp = hStrTmp->sysCreateObjectQuick( (hOBJECT*) &pPostpone->m_hToken, IID_TOKEN);
				if (!PR_SUCC( error_tmp) )
					pPostpone->m_hToken = NULL;

				tDWORD dwSizeTmp = 0;
				error_tmp = hObject->propGetStr( &dwSizeTmp, m_propDosDeviceName, NULL, 0, cCP_UNICODE );
				if (PR_SUCC( error_tmp ) && dwSizeTmp)
				{
					error_tmp = hStrTmp->heapAlloc (
						(tPTR*) &pPostpone->m_pwchDeviceName,
						dwSizeTmp
						);

					if (PR_SUCC( error_tmp ))
					{
						error_tmp = hObject->propGetStr (
							&dwSizeTmp,
							m_propDosDeviceName,
							pPostpone->m_pwchDeviceName,
							dwSizeTmp,
							cCP_UNICODE
							);

						if (PR_SUCC( error_tmp ))
						{
							hObject->propSetStr (
								&dwSizeTmp,
								m_propDosDeviceName,
								pPostpone->m_pwchDeviceName,
								dwSizeTmp,
								cCP_UNICODE
								);
						}
					}
				}

				hStrTmp->propSetBool( m_propDelayedProp, cTRUE );
				pPostpone->m_hCtx = hStrTmp;

				tDWORD ProcessId = hObject->propGetDWord( m_propProcessID );
				hStrTmp->propSetDWord( m_propProcessID, ProcessId );

				cUserInfo* pUser = (cUserInfo*) hObject->propGetPtr( m_propAccessedUser );
				if (pUser)
				{
					pPostpone->m_User = *pUser;
					hStrTmp->propSetPtr( m_propAccessedUser, &pPostpone->m_User );
				}

				cUserInfo* pUserCr = (cUserInfo*) hObject->propGetPtr( m_propModifiedUser );
				if (pUserCr)
				{
					pPostpone->m_UserCr = *pUserCr;
					hStrTmp->propSetPtr( m_propModifiedUser, &pPostpone->m_UserCr );
				}

				pPostpone->m_pNext = m_pPostpone;
				m_pPostpone = pPostpone;

				hStrTmp = NULL; // dont free, will be in post processing
			}

			if (hStrTmp)
				hStrTmp->sysCloseObject();
		}
	}

	m_PostponeLock->Leave( NULL );
}

void
OASImpl::ClearPostpone (
	)
{
	m_PostponeLock->Enter( SHARE_LEVEL_WRITE);

	m_bPostponeDisabled = cTRUE;
	
	cPostpone* pPostponePrev = NULL;
	cPostpone* pPostpone = m_pPostpone;
	while (pPostpone)
	{
		if (!pPostpone->m_bProcessing)
		{
			// not processed now, may clear
			if (pPostponePrev)
				pPostponePrev->m_pNext = pPostpone->m_pNext;
			else
				m_pPostpone = pPostpone->m_pNext;
			
			FreePostponeItem( pPostpone );
			pPostpone = m_pPostpone;

			continue;
		}

		pPostponePrev = pPostpone;
		pPostpone = pPostpone->m_pNext;
	}

	m_PostponeLock->Leave( NULL );
}

tERROR
cPostponeThread::on_idle (
	)
{
	PR_TRACE(( g_root, prtIMPORTANT, "oas\tpostpone idle" ));

	m_pTask->m_PostponeLock->Enter( SHARE_LEVEL_WRITE);

	cPostpone* pPostpone = m_pTask->m_pPostpone;
	if ( pPostpone )
	{
		pPostpone->m_TimeCount++;
		if (pPostpone->m_TimeCount < 5 )
			pPostpone = NULL;
		else
			pPostpone->m_bProcessing = cTRUE;
	}
	
	m_pTask->m_PostponeLock->Leave( NULL );

	if (!pPostpone)
		return errOK;

	PR_TRACE(( g_root, prtIMPORTANT, "oas\tpostpone check '%tstr'", pPostpone->m_hCtx) );

	if (pPostpone->m_hToken)
		pPostpone->m_hToken->Impersonate();

	CInterceptedObject Object(m_pTask);

	Object.Prepare( pPostpone->m_hCtx );

	if( Object.PreProcess() != errOK_DECIDED )
    {
	    hIO hIo = NULL;
	    if (PR_SUCC( m_pTask->m_hAvpg->CreateIO( (hOBJECT*) &hIo, pPostpone->m_hCtx ) ))
	    {
		    // process!
            m_pTask->Process( Object, (hOBJECT)hIo, NULL );
	    }
    }

	if (pPostpone->m_hToken)
		pPostpone->m_hToken->Revert();

	m_pTask->m_PostponeLock->Enter( SHARE_LEVEL_WRITE);

	cPostpone* pPostponeTmp = m_pTask->m_pPostpone;
	cPostpone* pPostponePrev = NULL;
	while (pPostponeTmp)
	{
		if (pPostponeTmp == pPostpone)
		{
			if (pPostponePrev)
				pPostponePrev->m_pNext = pPostponeTmp->m_pNext;
			else
				m_pTask->m_pPostpone = pPostponeTmp->m_pNext;
				
			break;
		}
		pPostponePrev = pPostponeTmp;
		pPostponeTmp = pPostponeTmp->m_pNext;
	}

	m_pTask->m_PostponeLock->Leave( NULL );
		
	m_pTask->FreePostponeItem( pPostpone );
	
	return errOK;
}

tERROR OASImpl::Process( CInterceptedObject& Object, hOBJECT hIOObject, tDWORD* pdwRetVal)
{
    tERROR error = errOK;

	if( !Object.IsIOToProcess((hIO)hIOObject) )
	{
		if (pdwRetVal)
            *pdwRetVal = cAVPG2_RC_ALLOW_ACCESS | cAVPG2_RC_CACHE;
		hIOObject->sysCloseObject();
		PR_TRACE((Object.m_hObject, prtIMPORTANT, "oas\t%s\t%S", "skipio", Object.m_sObjectName.data()));
	}
	else
	{
		error = m_avsSession->ProcessObject(hIOObject, &Object, Object.m_pSettings, &Object.m_NotifyInfo);

		bool bDetected = !!(Object.m_nProcessStatusMask & cScanProcessInfo::DETECTED);
		bool bNotVirus = (Object.m_NotifyInfo.m_nDetectDanger == DETDANGER_LOW);

		if( bDetected && !bNotVirus )
		{
			Object.BanUserIfNeeded();

			if( Object.m_bIsDelayed )
				SendNotification(Object);
		}

		if( Object.m_nProcessStatusMask & cScanProcessInfo::DELETED )
		{
			if (pdwRetVal)
                *pdwRetVal = cAVPG2_RC_DELETE_OBJECT;

            // hIOObject is aleady closed in AVS
		}
		else
		{
			if( hIOObject )
			{
				if( bDetected )
					Object.m_sObjectName.assign(hIOObject, pgOBJECT_FULL_NAME);
				hIOObject->sysCloseObject();
			}

			if( bDetected )
			{
				if( !Object.m_bIsDelayed )
					PutObjectInQueue(Object);

				if( !bNotVirus )
                {
                    if (pdwRetVal)
					    *pdwRetVal = cAVPG2_RC_DENY_ACCESS;
                }
				else
                {
                    if (pdwRetVal)
					    *pdwRetVal = cAVPG2_RC_ALLOW_ACCESS;
                }
			}
			else if( Object.m_nProcessStatusMask & cScanProcessInfo::CANCELED )
			{
				if( Object.IsNeedPostpone() )
				{
					AddPostpone( Object.m_hObject );
					if (pdwRetVal)
                        *pdwRetVal = cAVPG2_RC_ALLOW_ACCESS; // temporary
				}
				else
				{
                    if (pdwRetVal)
					    *pdwRetVal = cAVPG2_RC_ALLOW_ACCESS | cAVPG2_RC_CACHE;
					PR_TRACE((Object.m_hObject, prtIMPORTANT, "oas\tnot IsNeedPostpone\t%S", Object.m_sObjectName.data()));
				}
			}
			else
            {
                if (pdwRetVal)
				    *pdwRetVal = cAVPG2_RC_ALLOW_ACCESS | cAVPG2_RC_CACHE;
            }
		}
    }
    return error;
}

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
tERROR OASImpl::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len ) 
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::MsgReceive method" );

	// Place your code here

	if( p_msg_cls_id == pmc_AVS && p_msg_id == pm_SETTINGS_CHANGED ||
		p_msg_cls_id == pmc_QBSTORAGE_REPLICATED && p_msg_id == pm_QB_IO_RESTORE ||
		p_msg_cls_id == pmc_UPDATER && p_msg_id == pm_COMPONENT_UPDATED && 
		p_par_buf_len == SER_SENDMSG_PSIZE && p_par_buf && ((cSerializable *)p_par_buf)->isBasedOn(cUpdatedComponentName::eIID) &&
		((cUpdatedComponentName *)p_par_buf)->m_updatedComponentName.compare(cUPDATE_CATEGORY_AVS, fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ )
	{
		m_hAvpg->ResetDriverCache();
		return error;
	}

	if (p_msg_cls_id == pmc_PROCESSING)
	{
		tDWORD Timeout = 5;
		switch (p_msg_id)
		{
		case pm_PROCESSING_YIELD:
			if (p_par_buf && *p_par_buf_len >= sizeof(tDWORD) && *(tDWORD*)p_par_buf != 0)
				Timeout = *(tDWORD*)p_par_buf / 1000;
			if (m_hAvpg)
				m_hAvpg->YieldEvent(p_send_point, Timeout);
			return errOK;
		}
	}

	if( p_msg_cls_id == pmc_AVPG2 )
	{
		switch( p_msg_id )
		{

		case pm_AVPG2_NOTIFY_DRIVER_CONNECTION_IS_BROKEN :
			sysSendMsg(pmc_TASK_STATE_CHANGED, m_nState = TASK_STATE_FAILED, 0, 0, 0);
			return errOK_DECIDED;

		case pm_AVPG2_NOTIFY_VOLUME_MOUNTED :
			ClearDriveCache(true);
			return errOK;

		case pm_AVPG2_PROCEED_OBJECT:
			break;

		default: return error;
		}

		tDWORD dwRetVal = cAVPG2_RC_ALLOW_ACCESS;
		hSTRING hObject = (hSTRING)p_ctx;

		CInterceptedObject Object(this);

		error = Object.Prepare( hObject );

		if( PR_SUCC(error) && Object.m_bIsVolume )
		{
			PutObjectInQueue(Object);
			return errOK;
		}

		if( PR_SUCC(error) )
			error = Object.PreProcess();

		if( error == errOK_DECIDED )
		{
			dwRetVal = cAVPG2_RC_ALLOW_ACCESS | cAVPG2_RC_CACHE;
		}
		else if( PR_SUCC(error) )
		{
			hOBJECT hIOObject = NULL;
			error = m_hAvpg->CreateIO(&hIOObject, (hSTRING)hObject);

			if( PR_FAIL(error) )
			{
				PR_TRACE((hObject, prtNOTIFY, "oas\t%s\t%S - err %terr", "noio", Object.m_sObjectName.data(), error));
				if( error == errOBJECT_NOT_FOUND )
				{
					dwRetVal = cAVPG2_RC_ALLOW_ACCESS | cAVPG2_RC_CACHE;
					PR_TRACE((hObject, prtIMPORTANT, "oas\t%s(NotFound)\t%S - err %terr", "noio", Object.m_sObjectName.data(), error));
				}
			}
            else
            {
                error = Process( Object, hIOObject, &dwRetVal );
            }
		}

		if( p_par_buf && p_par_buf_len && *p_par_buf_len >= sizeof(tDWORD) ) 
			*(tDWORD*)p_par_buf = dwRetVal;
	}

	else if (pmc_REMOTE_GLOBAL == p_msg_cls_id)
	{
		if (pm_BANLIST_NEWRECEIVER == p_msg_id)
		{
			if (p_ctx && (p_ctx != *this))
			{
				sysRegisterMsgHandler(pmc_USERBAN_SYNC, rmhDECIDER, p_ctx, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
				p_ctx->sysRegisterMsgHandler(pmc_USERBAN_SYNC, rmhDECIDER, this, IID_ANY, PID_ANY, IID_ANY, PID_ANY );

				cUserBanList BanList;
				GetBanList(&BanList);
				sysSendMsg(pmc_USERBAN_SYNC, pm_BANLIST_SYNC, NULL, &BanList, SER_SENDMSG_PSIZE);
				error = errOK_DECIDED;
			}
		}
	}

	else if (pmc_USERBAN_SYNC == p_msg_cls_id)
	{
		switch (p_msg_id)
		{
		case pm_BANLIST_SYNC:
			PR_TRACE( (g_root, prtIMPORTANT,"oas\tBan income"));
			if (p_par_buf && (SER_SENDMSG_PSIZE == p_par_buf_len))
			{
				cUserBanList* pBanList = (cUserBanList*) p_par_buf;
				SetBanList(pBanList);
			}

			break;

		case pm_BANLIST_ITEM_ADD:
			if (p_ctx && (p_ctx != *this))
			{
				if (p_par_buf && (SER_SENDMSG_PSIZE == p_par_buf_len))
				{
					cUserBanItem* pBanItem = (cUserBanItem*) p_par_buf;
					AddUserBan(pBanItem, false);
					error = errOK;
				}
			}
			break;

		case pm_BANLIST_ITEM_DEL:
			if (p_ctx && (p_ctx != *this))
			{
				if (p_par_buf && (SER_SENDMSG_PSIZE == p_par_buf_len))
				{
					cUserBanItem* pBanItem = (cUserBanItem*) p_par_buf;
					DeleteUserBan(pBanItem, false);
					error = errOK;
				}
			}
			break;

		}
	}

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
tERROR OASImpl::SetSettings( const cSerializable* p_settings ) 
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetSettings method" );

	// Place your code here
	if( !p_settings || !p_settings->isBasedOn(cOASSettings::eIID) )
		return errPARAMETER_INVALID;

	Lock();

	m_Settings = *(cOASSettings*)p_settings;
	m_Settings.m_aIncludeList.clear();
	m_Settings.m_bIncludeByMask = cFALSE;

	tDWORD i, nCount = m_Settings.m_aScanObjects.size();
	for(i = 0; i < nCount; i++)
	{
		cScanObject& pObject = m_Settings.m_aScanObjects[i];
		switch( pObject.m_nObjType )
		{
		case OBJECT_TYPE_ALL_REMOVABLE_DRIVES: m_Settings.m_bScanRemovable = pObject.m_bEnabled; break;
		case OBJECT_TYPE_ALL_FIXED_DRIVES:     m_Settings.m_bScanFixed = pObject.m_bEnabled; break;
		case OBJECT_TYPE_ALL_NETWORK_DRIVES:   m_Settings.m_bScanNetwork = pObject.m_bEnabled; break;
		}
	}

	m_bScanRemovable = m_bScanFixed = m_bScanNetwork = cFALSE;

	for(i = 0; i < nCount; i++)
	{
		cScanObject& pObject = m_Settings.m_aScanObjects[i];

		if( !pObject.m_bEnabled || pObject.m_strObjName.empty() )
			continue;

		sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_SYSTEM_ONLY_STRING,
			(hOBJECT)cAutoString(pObject.m_strObjName), 0, 0);

		if( pObject.m_nObjType == OBJECT_TYPE_FOLDER || pObject.m_nObjType == OBJECT_TYPE_DRIVE )
		{
			pObject.m_strObjName.check_last_slash();
		}
		else if( pObject.m_nObjType == OBJECT_TYPE_MASKED )
		{
			cIOObj io(*this, cAutoString(pObject.m_strObjName), fACCESS_READ, fOMODE_FOLDER_AS_IO);
			if( PR_SUCC(io.last_error()) && (io->propGetDWord(pgOBJECT_ATTRIBUTES) & fATTRIBUTE_DIRECTORY) )
				pObject.m_strObjName.check_last_slash();
		}

		switch( GetDriveTypeG(pObject.m_strObjName) )
		{
			case LL_DRIVE_REMOVABLE:
			case LL_DRIVE_CDROM:     m_bScanRemovable = cTRUE; break;
			case LL_DRIVE_UNKNOWN:
			case LL_DRIVE_FIXED:
			case LL_DRIVE_RAMDISK:   m_bScanFixed = cTRUE;   break;
			case LL_DRIVE_REMOTE:    m_bScanNetwork = cTRUE; break;
		}

		cObjectPathMask& pInclude = m_Settings.m_aIncludeList.push_back();
		pInclude.m_strMask = pObject.m_strObjName;
		pInclude.m_bRecurse = pObject.m_bRecursive;
	}

	m_SettingsId++;
	Unlock();

    m_hAvpg->SetWorkingMode(m_Settings.m_nWorkingMode);
	
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
tERROR OASImpl::GetSettings( cSerializable* p_settings ) 
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::GetSettings method" );

	// Place your code here
	if( !p_settings || !p_settings->isBasedOn(cOASSettings::eIID) )
		return errPARAMETER_INVALID;

	*(cOASSettings*)p_settings = m_Settings;

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
tERROR OASImpl::AskAction( tActionId p_actionId, cSerializable* p_actionInfo ) 
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::AskAction method" );

	// Place your code here
	if( p_actionInfo && p_actionInfo->isBasedOn(cUserBanItem::eIID) && m_hAvpg )
    {
        DeleteUserBan((cUserBanItem*)p_actionInfo, true);
    }

	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
tERROR OASImpl::SetState( tTaskRequestState p_state ) 
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetState method" );

	// Place your code here
	switch( p_state )
	{
		case TASK_REQUEST_RUN:
			if( PR_FAIL(error = m_avsSession->SetState(p_state)) )
				return error;

			LoadPlugins();

			m_hAvpg->SetActivityMode(cAVPG2_CLIENT_ACTIVITY_MODE_ACTIVE);
			m_nState = TASK_STATE_RUNNING;
			break;
		case TASK_REQUEST_PAUSE:
			m_hAvpg->SetActivityMode(cAVPG2_CLIENT_ACTIVITY_MODE_SLEEP);
			m_nState = TASK_STATE_PAUSED;
			break;
		case TASK_REQUEST_STOP:
			m_hAvpg->SetActivityMode(cAVPG2_CLIENT_ACTIVITY_MODE_SLEEP);
			m_avsSession->SetState(p_state);

			m_nState = TASK_STATE_STOPPED;
			break;
		default:
			return errTASK_STATE_UNKNOWN;
	}

	sysSendMsg(pmc_TASK_STATE_CHANGED, m_nState, 0, 0, 0);
	return error;
}
// AVP Prague stamp end

// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
tERROR OASImpl::GetStatistics( cSerializable* p_statistics ) 
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Task::GetStatistics method" );

	// Place your code here
	error = m_avsSession->GetStatistic(p_statistics);
	if( PR_SUCC(error) && p_statistics->isBasedOn(cOASStatistics::eIID) )
	{
		cUserBanList& aBanList = ((cOASStatistics*)p_statistics)->m_aBanList;
        GetBanList(&aBanList);
	}

	return error;
}
// AVP Prague stamp end

void OASImpl::ReloadBanList()
{
    if (m_hAvpg)
    {
        m_hAvpg->ResetBanList();

	    int cou_all = m_BanList.m_Users.count();
	    for (int cou = 0; cou < cou_all; cou++)
	    {
		    cUserBanItem& BanItem = m_BanList.m_Users[cou];

            m_hAvpg->AddBan((cUserInfo*)&BanItem);
	    }
    }
}

void OASImpl::GetBanList(cUserBanList* pBanList)
{
//	PR_TRACE((g_root, prtIMPORTANT, "oas\tGetBanList"));

    m_BanListLock->Enter(SHARE_LEVEL_READ);
	
	pBanList->assign(m_BanList, true);

    m_BanListLock->Leave(NULL);

	time_t current = cDateTime::now_utc();

	int cou_all = pBanList->m_Users.count();
	for (int cou = 0; cou < cou_all; cou++)
	{
		cUserBanItem& BanItem = pBanList->m_Users[cou];

		BanItem.m_RemainTime = BanItem.m_StopTime - current;
	}
}

void OASImpl::SetBanList(cUserBanList* pBanList)
{
	PR_TRACE((g_root, prtIMPORTANT, "oas\tSetBanList"));

    m_BanListLock->Enter(SHARE_LEVEL_WRITE);
	
	m_BanList.assign(*pBanList, true);
    ReloadBanList();
	
    m_BanListLock->Leave(NULL);
}

#define _user_ban_new		0x00
#define _user_ban_send		0x01
#define _user_ban_ext		0x02

void OASImpl::AddUserBan(cUserBanItem* pBanItem, bool bReSend)
{
	DeleteUserBan(pBanItem, bReSend);

	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "oas\tAddBanItem (%s %s) - %s",
        pBanItem->m_sUserName.c_str(cCP_ANSI).ptr(), pBanItem->m_sMachineName.c_str(cCP_ANSI).ptr(),
		bReSend ? "resend" : "internal msg"));

	m_BanListLock->Enter(SHARE_LEVEL_WRITE);
	
	pBanItem->m_LocalFlags = _user_ban_ext;
	m_BanList.m_Users.push_back(*pBanItem);

    ReloadBanList();

	m_BanListLock->Leave(NULL);

	if (bReSend)
	{
		PR_TRACE((g_root, prtIMPORTANT, "oas\tsending AddBanItem..."));
		sysSendMsg(pmc_USERBAN_SYNC, pm_BANLIST_ITEM_ADD, this, pBanItem, SER_SENDMSG_PSIZE);
	}
}

bool OASImpl::DeleteUserBan(cUserBanItem* pBanItem, bool bReSend)
{
	tERROR error = errNOT_FOUND;

	PR_TRACE((g_root, prtIMPORTANT, "oas\tDeleteBanItem (%s %s) - %s",
		pBanItem->m_sUserName.c_str(cCP_ANSI).ptr(), pBanItem->m_sMachineName.c_str(cCP_ANSI).ptr(),
		bReSend ? "resend" : "internal msg"));

	m_BanListLock->Enter(SHARE_LEVEL_WRITE);
	
	int cou_all = m_BanList.m_Users.count();
	for (int cou = 0; cou < cou_all; cou++)
	{
		cUserBanItem& BanItem = m_BanList.m_Users[cou];
		if (BanItem.IsEqual(*pBanItem))
		{
			m_BanList.m_Users.remove(cou);
            ReloadBanList();
			error = errOK;
			break;
		}
	}

	m_BanListLock->Leave(NULL);

	if (errOK == error)
	{
		if (bReSend)
		{
			PR_TRACE((g_root, prtIMPORTANT, "oas\tsending DeleteBanItem..."));
			sysSendMsg(pmc_USERBAN_SYNC, pm_BANLIST_ITEM_DEL, this, pBanItem, SER_SENDMSG_PSIZE);
		}
	}

	return error == errOK;
}



tPROPID OASImpl::m_propProceedObjectOrigin;
tPROPID	OASImpl::m_propDosDeviceName;
tPROPID	OASImpl::m_propProcessID;
tPROPID	OASImpl::m_propDelayedProp;
tPROPID OASImpl::m_propAccessedUser;
tPROPID OASImpl::m_propModifiedUser;

// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR OASImpl::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end

// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "OAS Implementation", 19 )
	mSHARED_PROPERTY( pgTASK_TYPE, ((tDWORD)(TASK_OAS)) )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, OASImpl, m_hTM, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_BUF( pgTASK_STATE, OASImpl, m_nState, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_SESSION_ID, OASImpl, m_nSessionId, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_BUF( pgTASK_ID, OASImpl, m_nTaskId, cPROP_BUFFER_READ_WRITE )
mpPROPERTY_TABLE_END(Task_PropTable)
// AVP Prague stamp end

// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Task)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(Task)
// AVP Prague stamp end

// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(Task)
	mEXTERNAL_METHOD(Task, SetSettings)
	mEXTERNAL_METHOD(Task, GetSettings)
	mEXTERNAL_METHOD(Task, AskAction)
	mEXTERNAL_METHOD(Task, SetState)
	mEXTERNAL_METHOD(Task, GetStatistics)
mEXTERNAL_TABLE_END(Task)
// AVP Prague stamp end

// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "Task::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_TASK,                               // interface identifier
		PID_OAS,                                // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                           // interface version
		VID_MEZHUEV,                            // interface developer
		&i_Task_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Task_vtbl,                           // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),     // external function table size
		Task_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable),   // property table size
		sizeof(OASImpl)-sizeof(cObjImpl),       // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Task(IID_TASK) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_

// AVP Prague stamp end

	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(&m_propProceedObjectOrigin, cAVPG2_PROP_CUSTOM_ORIGIN, pTYPE_ORIG_ID);
	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(&m_propAccessedUser, cAVPG2_PROP_USER_ACCESSED, pTYPE_PTR);
	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(&m_propModifiedUser, cAVPG2_PROP_USER_CREATED, pTYPE_PTR);
	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(&m_propDosDeviceName, cAVPG2_PROP_DOS_DEVICE_NAME, pTYPE_STRING);
	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(&m_propProcessID, cAVPG2_PROP_PROCESS_ID, pTYPE_DWORD);
	if( PR_SUCC(error) )
		error = root->RegisterCustomPropId(&m_propDelayedProp, cAVPG2_PROP_BACKGROUND_PROCESSING, pTYPE_BOOL);

	if( PR_SUCC(error) )
		error = root->ResolveImportTable(NULL, import_table, PID_OAS);

// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call Task_Register( hROOT root ) { return OASImpl::Register(root); }
// AVP Prague stamp end

