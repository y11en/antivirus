#include "processmonitor.h"
#include <Prague/iface/i_event.h>
#include <Prague/plugin/p_thpoolimp.h>
#include "wthlp.h"
#include "mscat.h"
#include "sha1.h"
#include <Prague/plugin/p_win32_nfio.h>

#define cCSR_TOTAL_TIMEOUT 30
#define cCSR_TASK_COUNT (countof(arrCSRTasks)) // number of parallel tasks in GetObjectSecurityRating

class cCSRContext
{
public:
	cCSRContext() :
	  m_bStop(false),
		  m_pInternalContext(0),
		  m_pProcMon(0),
		  m_pRating(0),
		  m_nTimeLimit(cCSR_TOTAL_TIMEOUT),
		  m_nTasksAdded(0),
		  m_nTasksCompleted(0),
		  m_hTaskCompletedEvent(NULL)
	  {
	  };

	  cCalcSecurityRatingProgress m_progress;
	  cSecurityRating* m_pRating;

	  bool m_bStop;
	  cDateTime m_nStartTime;
	  CProcMon* m_pProcMon;
	  sProcMonInternalContext* m_pInternalContext;
	  time_t m_nTimeLimit;
	  long m_nTasksAdded;
	  long m_nTasksCompleted;
	  cEvent* m_hTaskCompletedEvent;
};

tERROR pr_call CSR_TaskCallbackKLSRL(cCSRContext* pcsr_ctx);
tERROR pr_call CSR_TaskCallbackScan(cCSRContext* pcsr_ctx);
tERROR pr_call CSR_TaskCallbackModuleInfo(cCSRContext* pcsr_ctx);

typedef tERROR (pr_call *tCSR_TaskCallback)(cCSRContext* pcsr_ctx);

const tCSR_TaskCallback arrCSRTasks[] = 
{
	CSR_TaskCallbackKLSRL,
	CSR_TaskCallbackScan,
	CSR_TaskCallbackModuleInfo,
};

struct sCommonTaskData
{
	cCSRContext* pcsr_ctx;
	int task_no;
};

tERROR pr_call CSR_TaskCallbackCommon (
									 tThreadCallType CallType,
									 tPTR pThreadContext,
									 tPTR* ppTaskContext,
									 tPTR pTaskData,
									 tDWORD TaskDataLen
									 )
{
	tERROR error = errOK;
	sCommonTaskData* pData = (sCommonTaskData*)pTaskData;
	cCSRContext* pcsr_ctx = pData->pcsr_ctx;

	switch (CallType)
	{
	case TP_CBTYPE_TASK_INIT_CONTEXT:
		break;
	case TP_CBTYPE_TASK_PROCESS:
		if (pData->task_no >= cCSR_TASK_COUNT)
			return errPARAMETER_INVALID;
		return arrCSRTasks[pData->task_no](pcsr_ctx);
		break;
	case TP_CBTYPE_TASK_DONE_CONTEXT:
		PrInterlockedIncrement(&pcsr_ctx->m_nTasksCompleted);
		if (pcsr_ctx->m_hTaskCompletedEvent)
			pcsr_ctx->m_hTaskCompletedEvent->SetState(cTRUE);
	}
	return errOK;
}

BOOL __cdecl iCallbackGetSHA1Norm(void* pContext, BYTE sha1n[20])
{
	if (!pContext)
		return FALSE;
	sProcMonInternalContext* pIntCtx = (sProcMonInternalContext*)pContext;
	cFileHashSHA1Norm tmp;
	if (PR_FAIL(pIntCtx->m_pProcMon->GetObjectHashSHA1Norm(&tmp, *pIntCtx)))
		return FALSE;
	memcpy(sha1n, tmp.m_HashSHA1Norm, 20);
	return TRUE;
}

tERROR CProcMon::GetFileSignature(cModuleInfoSignature* pInfo, sProcMonInternalContext& pContext)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	if (!m_pMSCatCache)
		return errOBJECT_NOT_INITIALIZED;
	if (pContext.m_hIo.obj())
	{
		if (pContext.m_hIo->propGetPID() == PID_NATIVE_FIO)
			hFile =  pContext.m_hIo->propGetPtr(plNATIVE_HANDLE);
	}

	cSigInfo* pSigInfo = NULL;
	LONG nWVTResult = TRUST_E_FAIL;
	cFileHashSHA1 sha1;
	bool bHashValid = false, bHashWasValid = false;
	DWORD dwFlags = 0; // TODO: check if is need to check CAT files and set MSC_DONT_LOOKUP_CATALOGS accordingly
	bool bSigned = m_pMSCatCache->IsSigned(pContext.m_sFileName.data(), 0, hFile, &pSigInfo, &nWVTResult, iCallbackGetSHA1Norm, &pContext);
	pInfo->clear();
	if (pSigInfo)
	{
		pInfo->m_sSignerCommonName = pSigInfo->GetSubjectName();
		LPWSTR pOrganization = pSigInfo->GetCertName(CERT_NAME_ATTR_TYPE, 0,  szOID_ORGANIZATION_NAME);
		if (pOrganization)
		{
			pInfo->m_sSignerOrganization = pOrganization;
			pSigInfo->FreeCertName(pOrganization);
		}
		pSigInfo->Release();
	}
	if (SUCCEEDED(nWVTResult))
	{
		pInfo->m_nTrustFlags |= TF_SIGNED | TF_TRUSTED;
	}
	else
	{
		switch(nWVTResult)
		{
		case TRUST_E_NOSIGNATURE: //  No signature was present in the subject.
			break;
		case TRUST_E_PROVIDER_UNKNOWN: //  Unknown trust provider.
		case TRUST_E_ACTION_UNKNOWN: //  The trust verification action specified is not supported by the specified trust provider.
		case TRUST_E_SUBJECT_FORM_UNKNOWN: //  The form specified for the subject is not one supported or known by the specified trust provider.
		case DIGSIG_E_ENCODE: //  Error due to problem in ASN.1 encoding process.
		case DIGSIG_E_DECODE: //  Error due to problem in ASN.1 decoding process.
		case DIGSIG_E_EXTENSIBILITY: //  Reading / writing Extensions where Attributes are appropriate, and visa versa.
		case DIGSIG_E_CRYPTO: //  Unspecified cryptographic failure.
		case PERSIST_E_SIZEDEFINITE: //  The size of the data could not be determined.
		case PERSIST_E_SIZEINDEFINITE: //  The size of the indefinite-sized data could not be determined.
		case PERSIST_E_NOTSELFSIZING: //  This object does not read and write self-sizing data.
		case CERT_E_REVOCATION_FAILURE: //  The revocation process could not continue - the certificate(s) could not be checked.
		case CERT_E_CRITICAL: //  A certificate contains an unknown extension that is marked 'critical'.
		// wrong usage
		case CERT_E_ROLE: //  A certificate that can only be used as an end-entity is being used as a CA or visa versa.
		case CERT_E_PATHLENCONST: //  A path length constraint in the certification chain has been violated.
		case CERT_E_PURPOSE: //  A certificate being used for a purpose other than the ones specified by its CA.
		case CERT_E_CN_NO_MATCH: //  The certificate's CN name does not match the passed value.
		case CERT_E_WRONG_USAGE: //  The certificate is not valid for the requested usage.
			pInfo->m_nTrustFlags |= TF_SIGNED | TF_FAILURE;
			break;
		case CERT_E_REVOKED: //  A certificate was explicitly revoked by its issuer.
			pInfo->m_nTrustFlags |= TF_SIGNED | TF_REVOKED;
			break;
		case CERT_E_EXPIRED: //  A required certificate is not within its validity period when verifying against the current system clock or the timestamp in the signed file.
			pInfo->m_nTrustFlags |= TF_SIGNED | TF_EXPIRED;
			break;
		case TRUST_E_SUBJECT_NOT_TRUSTED: //  The subject is not trusted for the specified action.
		case TRUST_E_EXPLICIT_DISTRUST: //  The certificate was explicitly marked as untrusted by the user.
		case CERT_E_UNTRUSTEDCA: //  A certification chain processed correctly, but one of the CA certificates is not trusted by the policy provider.
		case CERT_E_UNTRUSTEDROOT: //  A certificate chain processed, but terminated in a root certificate which is not trusted by the trust provider.
		case CERT_E_UNTRUSTEDTESTROOT: //  The certification path terminates with the test root which is not trusted with the current policy settings.
		case CERT_E_CHAINING: //  A certificate chain could not be built to a trusted root authority.
			pInfo->m_nTrustFlags |= TF_SIGNED | TF_UNTRUSTED;
			break;
		case CERT_E_INVALID_POLICY: //  The certificate has invalid policy.
		case CERT_E_INVALID_NAME: //  The certificate has an invalid name. The name is not included in the permitted list or is explicitly excluded.
		case CERT_E_ISSUERCHAINING: //  A parent of a given certificate in fact did not issue that child certificate.
		case CERT_E_MALFORMED: //  A certificate is missing or has an empty value for an important field, such as a subject or issuer name.
		case CERT_E_VALIDITYPERIODNESTING: //  The validity periods of the certification chain do not nest correctly.
			pInfo->m_nTrustFlags |= TF_SIGNED | TF_FAILURE | TF_INVALID;
			break;
		case TRUST_E_FAIL: //  Generic trust failure.
		default:
			pInfo->m_nTrustFlags |= TF_FAILURE;
			break;
		}
	}
	return errOK;
}

tERROR pr_call CSR_TaskCallbackKLSRL(cCSRContext* pcsr_ctx)
{
	tERROR error = pcsr_ctx->m_pProcMon->GetFileSignature(&pcsr_ctx->m_pRating->m_Signature, *pcsr_ctx->m_pInternalContext);
	if (pcsr_ctx->m_pRating->m_Signature.IsTrusted())
		return errOK;
	pcsr_ctx->m_pProcMon->GetKLSRLInfo(&pcsr_ctx->m_pRating->m_KlsrlInfo, *pcsr_ctx->m_pInternalContext);
	PR_TRACE((pcsr_ctx->m_pProcMon, prtIMPORTANT, TR "CSR KLSRL result %terr", pcsr_ctx->m_pRating->m_KlsrlInfo.m_errStatus));
	return errOK;
}

tERROR pr_call CSR_TaskCallbackModuleInfo(cCSRContext* pcsr_ctx)
{
	pcsr_ctx->m_pProcMon->GetModuleInfo(&pcsr_ctx->m_pRating->m_ModuleInfo, pcsr_ctx->m_pInternalContext->m_hIo);
	return errOK;
}

#define cSESSION_CLOSED  (hAVSSESSION)(-1)

tERROR CProcMon::GetAvsSession(hAVSSESSION* phAvsSession)
{
	tERROR error;
	if (!phAvsSession)
		return errPARAMETER_INVALID;

	cAutoCS cs(m_lock, true);
	if (m_hAvsSession == cSESSION_CLOSED)
		return errOBJECT_IS_CLOSED;

	if (!m_hAvs)
	{
		error = m_tm->GetService(TASKID_TM_ITSELF, *this, sid_TM_AVS, (cObject**)&m_hAvs);
		if (PR_FAIL(error))
			return error;
	}

	if (!m_hAvsSession)
	{
		error = m_hAvs->CreateSession(&m_hAvsSession, (hTASK)(hOBJECT)this, 0, OBJECT_ORIGIN_GENERIC);
		if (PR_FAIL(error))
			return error;
	}
	*phAvsSession = m_hAvsSession;
	return errOK;
}

tERROR CProcMon::CloseAvsSession()
{
	cAutoCS cs(m_lock, true);
	if (m_hAvsSession && m_hAvsSession != cSESSION_CLOSED )
	{
		m_hAvsSession->sysCloseObject();
		m_hAvsSession = cSESSION_CLOSED;
	}
	return errOK;
}

tERROR pr_call CSR_TaskCallbackScan(cCSRContext* pcsr_ctx)
{
	hAVS hAvs = NULL;
	hAVSSESSION hSession = NULL;
	
	pcsr_ctx->m_pRating->m_ScanError = pcsr_ctx->m_pProcMon->GetAvsSession(&hSession);
	if (PR_SUCC(pcsr_ctx->m_pRating->m_ScanError))
	{
		cScanProcessInfo result;
		cProtectionSettings settings;
		settings.m_nTimeLimit = cCSR_TOTAL_TIMEOUT;
		settings.m_bEmulEnable = true;
		settings.m_nEmulValue = 100;
		settings.m_nScanAction = SCAN_ACTION_DETECT;
		pcsr_ctx->m_pRating->m_ScanError = hSession->ProcessObject((hOBJECT)pcsr_ctx->m_pInternalContext->m_hIo, &result, &settings, &pcsr_ctx->m_pRating->m_ScanStat);
		pcsr_ctx->m_pRating->m_ScanDetected = !!(result.m_nProcessStatusMask & cScanProcessInfo::DETECTED);
		pcsr_ctx->m_pRating->m_EmulRating = result.m_nSecurityRating;
		if (pcsr_ctx->m_pRating->m_ScanDetected)
			pcsr_ctx->m_bStop = true;
	}
	PR_TRACE((pcsr_ctx->m_pProcMon, prtIMPORTANT, TR "CSR SCAN result %terr, d=%d, sr=%d", pcsr_ctx->m_pRating->m_ScanError, pcsr_ctx->m_pRating->m_ScanDetected, pcsr_ctx->m_pRating->m_EmulRating));
	//PrSleep(5000);
	return errOK;
}
/*
tERROR pr_call CSR_TaskCallbackSign(cCSRContext* pcsr_ctx)
{
	tERROR error = errOK;

	// TODO: implement DS

	return errOK;
}
*/


tERROR CProcMon::GetObjectSecurityRating( cSecurityRating* pInfo, sProcMonInternalContext& ctx)
{
	bool bNotificationWindowShown = false;
	tERROR error = errNOT_OK;
	if (!pInfo)
		return errPARAMETER_INVALID;

	PR_TRACE((this, prtNOTIFY, TR "CSR start for %S", ctx.m_sFileName.data()));

	if (PR_SUCC(GetCachedFileInfo(pInfo, ctx)))
	{
		PR_TRACE((this, prtNOTIFY, TR "CSR retrieved from cache for %S#%d", ctx.m_sFileName.data(), ctx.m_nCurrentFileRevision));
		return errOK;
	}

	time_t nStartTime = cDateTime::now_utc();

	cCSRContext csr_ctx;
	if (IS_UNIQ_PID(ctx.m_nPid))
	{
		cProcessPidInfo pidinfo;
		error = GetProcessInfoEx(ctx.m_nPid, &pidinfo);
		if (PR_FAIL(error))
		{
			PR_TRACE((this, prtERROR, TR "CSR cannot resolve native pid %terr", error));
			return error;
		}
		ctx.m_nPid = csr_ctx.m_progress.m_nPID = pidinfo.m_NativePid;
	}
	else
	{
		csr_ctx.m_progress.m_nPID = (tDWORD)ctx.m_nPid;
	}
	csr_ctx.m_progress.m_nSessionId = PrSessionId((tDWORD)ctx.m_nPid);
	csr_ctx.m_progress.m_nProgress = 1;
	csr_ctx.m_progress.m_nActionId = cCalcSecurityRatingProgress::eShowNotification;
	csr_ctx.m_pRating = pInfo;
	csr_ctx.m_pProcMon = this;
	csr_ctx.m_pInternalContext = &ctx;
	
	cAutoObj<cEvent> hTaskCompletedEvent ;
	error = sysCreateObject( (hOBJECT*)&hTaskCompletedEvent, IID_EVENT, PID_ANY );
	if (PR_SUCC( error ))
	{
		hTaskCompletedEvent->set_pgINITIAL_STATE(cFALSE);
		hTaskCompletedEvent->set_pgMANUAL_RESET(cFALSE);
		error = hTaskCompletedEvent->sysCreateObjectDone();
	}
	if (PR_FAIL( error ))
	{
		PR_TRACE((this, prtERROR, TR "CSR cannot create event %terr", error));
		return error;
	}
	csr_ctx.m_hTaskCompletedEvent = hTaskCompletedEvent;

	cAutoObj<cThreadPool> pThPool;

	error = sysCreateObject( (hOBJECT*)&pThPool, IID_THREADPOOL, PID_THPOOLIMP );
	if (PR_SUCC( error ))
	{
		pThPool->set_pgTP_DEFAULT_THREAD_PRIORITY( TP_THREADPRIORITY_NORMAL );
		pThPool->set_pgTP_MIN_THREADS( cCSR_TASK_COUNT );
		pThPool->set_pgTP_MAX_THREADS( cCSR_TASK_COUNT );
		pThPool->set_pgTP_TASK_QUEUE_MAX_LEN( cCSR_TASK_COUNT );
		pThPool->set_pgTP_QUICK_EXECUTE( cFALSE );
		pThPool->set_pgTP_QUICK_DONE( cFALSE );
	//	pThPool->set_pgTP_THREAD_CALLBACK( CSR_ThreadCallback );
	//	pThPool->set_pgTP_THREAD_CALLBACK_CONTEXT( &csr_ctx );
		pThPool->set_pgTP_NAME( L"CalcSR", 0, cCP_UNICODE );
		error = pThPool->sysCreateObjectDone();
	}
	if (PR_FAIL(error))
	{
		PR_TRACE((this, prtERROR, TR "CSR cannot create thpool %terr", error));
		return error;
	}

	sCommonTaskData task;
	task.pcsr_ctx = &csr_ctx;
	for (task.task_no = 0; task.task_no < cCSR_TASK_COUNT; task.task_no++)
	{
		tTaskId nTaskId;
		error = pThPool->AddTask(&nTaskId, CSR_TaskCallbackCommon, &task, sizeof(task), 0);
		if (PR_FAIL(error))
		{
			PR_TRACE((this, prtERROR, TR "CSR add task[%d] failed %terr", task.task_no, error));
			break;
		}
		csr_ctx.m_nTasksAdded++;
	}
	if (PR_FAIL(error))
	{
		PR_TRACE((this, prtERROR, TR "CSR failed, stopping tasks...", error));
		csr_ctx.m_bStop = true;
	}
	if (PR_SUCC(error))
	{
		tERROR wait_error = errOBJECT_NOT_INITIALIZED;
		while (PrInterlockedCompareExchange(&csr_ctx.m_nTasksCompleted, 0, 0) < csr_ctx.m_nTasksAdded)
		{
			if (csr_ctx.m_pInternalContext->m_bStop)
			{
				PR_TRACE((this, prtIMPORTANT, TR "CSR task stop signaled"));
				break;
			}

			PR_TRACE((this, prtNOTIFY, TR "CSR task wait start (%d of %d) %terr", PrInterlockedCompareExchange(&csr_ctx.m_nTasksCompleted, 0, 0), csr_ctx.m_nTasksAdded, wait_error));
			wait_error = hTaskCompletedEvent->Wait(1000);
			PR_TRACE((this, prtNOTIFY, TR "CSR task wait end   (%d of %d) %terr", PrInterlockedCompareExchange(&csr_ctx.m_nTasksCompleted, 0, 0), csr_ctx.m_nTasksAdded, wait_error));
			if (errTIMEOUT != wait_error && errOK != wait_error) 
				break;
			tDWORD nProgress = 0;
			time_t nTime = cDateTime::now_utc();
			nTime -= nStartTime;
			if (nTime < 2) // notification delay time
				continue;
			if (nTime > cCSR_TOTAL_TIMEOUT)
			{
				nProgress = 100;
				csr_ctx.m_bStop = true;
				PR_TRACE((this, prtIMPORTANT, TR "CSR total time exceeded(%d/%d)", nTime, cCSR_TOTAL_TIMEOUT));
			}
			else
			{
				nProgress = (tDWORD)((float)nTime * 100 / cCSR_TOTAL_TIMEOUT);
				nProgress = nProgress + (tDWORD)((float)PrInterlockedCompareExchange(&csr_ctx.m_nTasksCompleted, 0, 0) * (100 - nProgress) / cCSR_TASK_COUNT);
			}
			//if (!bNotificationWindowShown || nProgress != csr_ctx.m_progress.m_nProgress)
			{
				csr_ctx.m_progress.m_nProgress = nProgress;
				tERROR send_error = sysSendMsg(pmc_PROCESS_MONITOR, cCalcSecurityRatingProgress::eIID, NULL, &csr_ctx.m_progress, SER_SENDMSG_PSIZE);
				PR_TRACE((this, prtNOTIFY, TR "CSR show notification pid=%d, progress=%d, %terr", (tDWORD)csr_ctx.m_progress.m_nPID, nProgress, send_error));
				bNotificationWindowShown = true;
				csr_ctx.m_progress.m_nActionId = cCalcSecurityRatingProgress::eUpdataNotification;
			}
		}
		PR_TRACE((this, prtNOTIFY, TR "CSR wait exit  (%d of %d) %terr", PrInterlockedCompareExchange(&csr_ctx.m_nTasksCompleted, 0, 0), csr_ctx.m_nTasksAdded, wait_error));
	}

	csr_ctx.m_progress.m_nActionId = cCalcSecurityRatingProgress::eCloseNotification;
	if (bNotificationWindowShown)
	{
		tERROR send_error = sysSendMsg(pmc_PROCESS_MONITOR, cCalcSecurityRatingProgress::eIID, NULL, &csr_ctx.m_progress, SER_SENDMSG_PSIZE);
		PR_TRACE((this, prtNOTIFY, TR "CSR close notification pid=%d, %terr", csr_ctx.m_progress.m_nPID, send_error));
	}

	pThPool.clean(); // close 

	if (PR_SUCC(error) && PR_SUCC(SetCachedFileInfo(pInfo, ctx))) // cache it
		PR_TRACE((this, prtNOTIFY, TR "CSR data stored in cache for %S#%d", ctx.m_sFileName.data(), ctx.m_nCurrentFileRevision));
	return error;

}

tERROR CProcMon::GetIOHashMD5( hIO io, tBYTE md5[16])
{
	tERROR error;

	cAutoObj<cHash> hash;
	error = sysCreateObjectQuick((hOBJECT*)&hash, IID_HASH, PID_HASH_MD5);
	if (PR_FAIL(error))
		return error;

	tBYTE buff[0x8000];
	tDWORD nRead = 0;
	tQWORD nFileOffset = 0;

	for(; PR_SUCC(error); nFileOffset += nRead)
	{
		error = io->SeekRead(&nRead, nFileOffset, buff, sizeof(buff));
		if( !nRead )
			break;

		if( PR_SUCC(error) )
			error = hash->Update(buff, nRead);
	}
	if( PR_SUCC(error) ) 
		error = hash->GetHash(md5, 16);

	return error;
}


tERROR CProcMon::GetObjectHashMD5( cFileHashMD5* pInfo, sProcMonInternalContext& ctx)
{
	tERROR error;

	if (PR_SUCC(GetCachedFileInfo(pInfo, ctx)))
	{
		PR_TRACE((this, prtNOTIFY, TR "MD5 retrieved from cache for %S#%d", ctx.m_sFileName.data(), ctx.m_nCurrentFileRevision));
		return errOK;
	}

	pInfo->clear();
	error = GetIOHashMD5(ctx.m_hIo, pInfo->m_HashMD5);

	if (PR_SUCC(error) && PR_SUCC(SetCachedFileInfo(pInfo, ctx))) // cache it
		PR_TRACE((this, prtNOTIFY, TR "MD5 stored in cache for %S#%d", ctx.m_sFileName.data(), ctx.m_nCurrentFileRevision));
	return error;
}


tERROR CProcMon::GetObjectHashSHA1( cFileHashSHA1* pInfo, sProcMonInternalContext& ctx)
{
	tERROR error;

	if (PR_SUCC(GetCachedFileInfo(pInfo, ctx)))
	{
		PR_TRACE((this, prtNOTIFY, TR "SHA1 retrieved from cache for %S#%d", ctx.m_sFileName.data(), ctx.m_nCurrentFileRevision));
		return errOK;
	}

	memset(pInfo->m_HashSHA1, 0, sizeof(pInfo->m_HashSHA1));

	SHA1Context sha;
	tBYTE buff[0x8000];
	tDWORD nRead = 0;
	tQWORD nFileOffset = 0;

	SHA1Reset(&sha);
	for(; ; nFileOffset += nRead)
	{
		error = ctx.m_hIo->SeekRead(&nRead, nFileOffset, buff, sizeof(buff));
		if( PR_FAIL(error) )
			return error;
		if( !nRead )
			break;
		SHA1Input(&sha, buff, nRead);
	}
	if (!SHA1Result(&sha))
		return errUNEXPECTED;

	PBYTE hash = pInfo->m_HashSHA1;
	for (size_t i=0; i<5; i++)
	{
		PBYTE p = (PBYTE)&sha.Message_Digest[i];
		*hash++ = p[3];
		*hash++ = p[2];
		*hash++ = p[1];
		*hash++ = p[0];
	}

	if (PR_SUCC(error) && PR_SUCC(SetCachedFileInfo(pInfo, ctx))) // cache it
		PR_TRACE((this, prtNOTIFY, TR "SHA1 stored in cache for %S#%d", ctx.m_sFileName.data(), ctx.m_nCurrentFileRevision));
	return error;
}


tERROR CProcMon::GetObjectHashSHA1Norm( cFileHashSHA1Norm* pInfo, sProcMonInternalContext& ctx)
{
	if (PR_SUCC(GetCachedFileInfo(pInfo, ctx)))
	{
		PR_TRACE((this, prtNOTIFY, TR "SHA1N retrieved from cache for %S#%d", ctx.m_sFileName.data(), ctx.m_nCurrentFileRevision));
		return errOK;
	}

	HANDLE hFile;
	memset(pInfo->m_HashSHA1Norm, 0, sizeof(pInfo->m_HashSHA1Norm));

	if (!ctx.m_hIo.obj())
		return errUNEXPECTED;
	if (ctx.m_hIo->propGetPID() == PID_NATIVE_FIO)
		return errOBJECT_INCOMPATIBLE;
	DWORD hash_size = 20;
	hFile = ctx.m_hIo->propGetPtr(plNATIVE_HANDLE);
	if (!WTHlpCalcHashFromFileHandle(hFile, &hash_size, pInfo->m_HashSHA1Norm))
		return errNOT_OK;

	if (PR_SUCC(SetCachedFileInfo(pInfo, ctx))) // cache it
		PR_TRACE((this, prtNOTIFY, TR "SHA1N stored in cache for %S#%d", ctx.m_sFileName.data(), ctx.m_nCurrentFileRevision));

	return errOK;
}