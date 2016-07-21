#include <stdio.h>
#include "avsscan.h"
#include <plugin/p_tempfile.h>
#include <Prague/iface/i_os.h>
#include <Extract/plugin/p_memmod.h>
#include <Extract/plugin/p_memmodscan.h>

AVS_Scanner::AVS_Scanner () :
	m_hAVS (0),
	m_hAVSSession (0),
	m_hReceiver (0),
	m_hAsyncCtx(0),
	m_msg_handler_pfn (0),
	m_obj_handler_pfn (0),
	m_msg_handler_ctx (0),
	m_obj_handler_ctx (0),
	m_opt_flags (0),
	m_obj_actions (0),
	m_cure_mode (false)
{
}

AVS_Scanner::~AVS_Scanner ()
{
	cleanup ();
}

void AVS_Scanner::cleanup ()
{
	if (m_hAVSSession != 0)
	{
		m_hAVSSession->sysCloseObject ();
		m_hAVSSession = NULL;
	}

	if (m_hAVS != 0)
	{
		m_hAVS->sysCloseObject ();
		m_hAVS = NULL;
	}
}

tERROR AVS_Scanner::msg_receive (
		hOBJECT _this, tDWORD msg_cls,
		tDWORD msg_id, hOBJECT obj, hOBJECT ctx,
		hOBJECT receive_point, tPTR pbuff, tDWORD* blen)
{
	if (pbuff == 0)
		return errOK;

	if (msg_cls == pmc_AVS_OBJECT)
	{
		if (m_obj_handler_pfn != 0)
			return m_obj_handler_pfn (m_obj_handler_ctx, (cIO*)ctx, (cObjectInfo*)pbuff);

		return errOK;
	}

	if (msg_cls == pmcASK_ACTION)
	{
		return ask_object_action (msg_id, (cAskObjectAction *) pbuff);
	}

	const cDetectObjectInfo * info = (const cDetectObjectInfo *) pbuff;
	if ((m_opt_flags & OPT_PRINT_EVENTS) != 0)
	{
		print_object_event (info);
	}

	if (m_msg_handler_pfn != 0)
		return m_msg_handler_pfn (m_msg_handler_ctx, msg_cls, info);

	return errOK;
}

const kl_tchar_t* get_status_str(enObjectStatus status, const enDetectStatus* dstatus)
{
	switch(status)
	{
	case OBJSTATUS_OK                 : return KL_TEXT("ok");
	case OBJSTATUS_INFECTED           : return KL_TEXT("infected");
	case OBJSTATUS_SUSPICION          :
		return (dstatus && *dstatus == DSTATUS_PROBABLY) ? KL_TEXT("warning") : KL_TEXT("suspicion");
	case OBJSTATUS_ARCHIVE            : return KL_TEXT("archive");
	case OBJSTATUS_PACKED             : return KL_TEXT("packed");
	case OBJSTATUS_ENCRYPTED          : return KL_TEXT("encrypted");
	case OBJSTATUS_CORRUPTED          : return KL_TEXT("corrupted");
	case OBJSTATUS_BACKUPPED          : return KL_TEXT("backupped");
	case OBJSTATUS_PASSWORD_PROTECTED : return KL_TEXT("pswd protected");
	case OBJSTATUS_CANTCHANGEATTR     : return KL_TEXT("cant change attr");
	case OBJSTATUS_NOTPROCESSED       : return KL_TEXT("not processed");
	case OBJSTATUS_ADDEDBYUSER        : return KL_TEXT("changed by user");
	case OBJSTATUS_NOTDISINFECTED     : return KL_TEXT("not disinfected");
	case OBJSTATUS_NOTQUARANTINED     : return KL_TEXT("not quarantined");
	case OBJSTATUS_NOTBACKUPPED       : return KL_TEXT("not backupped");
	case OBJSTATUS_CANTBEDELETED      : return KL_TEXT("cant be deleted");
	case OBJSTATUS_PROCESSING_ERROR   : return KL_TEXT("processing error");
	case OBJSTATUS_DELETE_ON_REBOOT_FAILED : return KL_TEXT("delete on reboot failed");

	case OBJSTATUS_CURED	          : return KL_TEXT("cured");
	case OBJSTATUS_DISINFECTED        : return KL_TEXT("disinfected");
	case OBJSTATUS_DELETED            : return KL_TEXT("deleted");
	case OBJSTATUS_QUARANTINED        : return KL_TEXT("quarantined");
	case OBJSTATUS_OVERWRITED	      : return KL_TEXT("overwrited");
	case OBJSTATUS_ADDEDTOEXCLUDE     : return KL_TEXT("added to exclude");
	case OBJSTATUS_DELETED_ON_REBOOT  : return KL_TEXT("deleted on reboot");
	case OBJSTATUS_DISINFECTED_ON_REBOOT : return KL_TEXT("disinfected on reboot");
	case OBJSTATUS_QUARANTINED_ON_REBOOT : return KL_TEXT("quarantined on reboot");
	case OBJSTATUS_RESTORED           : return KL_TEXT("restored");
	case OBJSTATUS_DENIED             : return KL_TEXT("access denied");
	case OBJSTATUS_ALLOWED            : return KL_TEXT("access allowed");
	case OBJSTATUS_RENAMED            : return KL_TEXT("object renamed");
    case OBJSTATUS_FALSEALARM         : return KL_TEXT("false alarm");
    case OBJSTATUS_RENAME_FAILED      : return KL_TEXT("rename failed");
	default                           : return KL_TEXT("unknown");
	}
}

void AVS_Scanner::print_object_event (const cDetectObjectInfo *info)
{
	kl_ftprintf (stdout, KL_TEXT ("%s%s\t%s\t%s\n"),
				(m_cure_mode ? KL_TEXT("CURE:") : KL_TEXT("")),
				klav_tstring(info->m_strObjectName.data (), &g_malloc).c_str (),
				get_status_str(info->m_nObjectStatus, &info->m_nDetectStatus),
				klav_tstring(info->m_strDetectName.data (), &g_malloc).c_str ()
			);
	fflush(stdout);
}

tERROR AVS_Scanner::ask_object_action (tDWORD msg_id, cAskObjectAction *act)
{
	if (act == 0)
		return errOK;
	
	if (act->isBasedOn (cAskQBAction::eIID))
	{
		cAskQBAction *qb_action = (cAskQBAction *) act;
		if (msg_id == cAskQBAction::BACKUP)
		{
			qb_action->m_nResultAction = ACTION_OK;
			return errOK_DECIDED;
		}
	}

	if (act->isBasedOn (cAskObjectAction::eIID))
	{
		if ((act->m_nActionsMask & ACTION_DISINFECT) != 0)
		{
			if (act->m_nDetectStatus == DSTATUS_SURE)
			{
				m_obj_actions |= ACTION_DISINFECT;
			}
		}
	}

	return errOK;
}

tERROR AVS_Scanner::msg_receive_fn (
		hOBJECT _this, tDWORD msg_cls,
		tDWORD msg_id, hOBJECT obj, hOBJECT ctx,
		hOBJECT receive_point, tPTR pbuff, tDWORD* blen)
{
	AVS_Scanner *self = (AVS_Scanner *) _this->propGetPtr (pgRECEIVE_CLIENT_ID);
	if (self == 0)
		return errOK;

	return self->msg_receive (_this, msg_cls, msg_id, obj, ctx, receive_point, pbuff, blen);
}

tERROR AVS_Scanner::init_avs (const cAVSSettings * settings, tDWORD engine_flags)
{
	tERROR error = errOK;
	m_engine_flags = engine_flags;
	
	error = g_root->sysCreateObjectQuick((hOBJECT*)&m_hAVS, IID_AVS, PID_AVS);
	if (PR_FAIL(error))
	{
		print_log (KL_TEXT ("Error creating AVS, error code 0x%08X\n"), error);
		return error;
	}

	error = m_hAVS->SetSettings (settings);
	if (PR_FAIL (error))
	{
		print_log (KL_TEXT ("Error applying AVS settings, error code 0x%08X\n"), error);
		return error;
	}
	
	if (PR_SUCC (error))
		error = m_hAVS->sysCreateObject ((hOBJECT*)&m_hReceiver, IID_MSG_RECEIVER);
	if (PR_SUCC (error))
		error = m_hReceiver->propSetPtr (pgRECEIVE_PROCEDURE, (void*)msg_receive_fn);
	if (PR_SUCC (error))
		error = m_hReceiver->propSetPtr (pgRECEIVE_CLIENT_ID, this);
	if (PR_SUCC (error))
		error = m_hReceiver->sysCreateObjectDone ();
	if (PR_FAIL (error))
	{
		print_log (KL_TEXT ("Error creating message receiver, error code 0x%08X\n"), error);
		return error;
	}

	tMsgHandlerDescr hdl[] = {
		{ m_hReceiver, rmhDECIDER, pmc_EVENTS_NOTIFY, IID_ANY, PID_ANY, IID_ANY, PID_ANY },
		{ m_hReceiver, rmhDECIDER, pmc_EVENTS_IMPORTANT, IID_ANY, PID_ANY, IID_ANY, PID_ANY },
		{ m_hReceiver, rmhDECIDER, pmc_EVENTS_CRITICAL, IID_ANY, PID_ANY, IID_ANY, PID_ANY },
		{ m_hReceiver, rmhDECIDER, pmc_AVS_OBJECT, IID_ANY, PID_ANY, IID_ANY, PID_ANY },
		{ m_hReceiver, rmhDECIDER, pmcASK_ACTION, IID_ANY, PID_ANY, IID_ANY, PID_ANY },
	};
	error = m_hReceiver->sysRegisterMsgHandlerList (hdl, countof(hdl));
	if (PR_FAIL (error))
	{
		print_log (KL_TEXT ("Error registering message receiver, error code 0x%08X\n"), error);
		return error;
	}

	error = m_hAVS->CreateSession (&m_hAVSSession, (cTask*)m_hReceiver, engine_flags, OBJECT_ORIGIN_GENERIC);
	if (PR_FAIL (error))
	{
		print_log (KL_TEXT ("Error creating AVS session, error code 0x%08X\n"), error);
		return error;
	}

	error = m_hAVSSession->SetState (TASK_REQUEST_RUN);
	if (PR_FAIL (error))
	{
		print_log (KL_TEXT ("Error starting AVS session, error code 0x%08X\n"), error);
		return error;
	}

	return error;
}

#define IO_COPY_BUFSIZE 0x10000

static tERROR copy_io (hIO src, hIO dst)
{
	tERROR error = errOK;
	tQWORD src_size = 0;

	error = src->GetSize (& src_size, IO_SIZE_TYPE_EXPLICIT);
	if (PR_FAIL (error))
		return error;

	error = dst->SetSize (src_size);
	if (PR_FAIL (error))
		return error;

	tPTR buf = 0;
	error = src->heapAlloc (& buf, IO_COPY_BUFSIZE);
	if (PR_FAIL (error))
		return error;

	tQWORD off = 0;

	while (off < src_size)
	{
		tDWORD portion = IO_COPY_BUFSIZE;
		if (portion >= (src_size - off))
			portion = (tDWORD) (src_size - off);

		tDWORD nrd = 0;
		error = src->SeekRead (& nrd, off, buf, portion);
		if (PR_FAIL (error))
			return error;
		if (nrd == 0)
			return errOBJECT_READ;

		tDWORD nwr = 0;
		error = dst->SeekWrite (& nwr, off, buf, nrd);
		if (PR_FAIL (error))
			return error;
		if (nwr != nrd)
			return errOBJECT_WRITE;

		off += nrd;
	}

	src->heapFree (buf);

	return error;
}

tERROR AVS_Scanner::test_cure_io (hOBJECT hio, Scan_Results& scan_results)
{
	tERROR error = errOK;

	error = m_hAVS->sysCreateObject ((hOBJECT *) & (scan_results.m_cured_io), IID_IO, PID_TEMPIO, 0);
	if (PR_FAIL (error))
		return error;

	// copy IO object
	error = copy_io ((hIO) hio, scan_results.m_cured_io);
	if (PR_FAIL (error))
		return error;

	cStrObj sName;
	sName.assign(hio, pgOBJECT_FULL_NAME);
//	sName.check_last_slash();
	sName.copy(scan_results.m_cured_io, pgOBJECT_FULL_NAME);

	cStrObj sCured(L"cured");
	sCured.copy(scan_results.m_cured_io, pgOBJECT_NAME); // zalipuha for rar.ppl

	// perform disinfection
	cScanProcessInfo  pScanInfo;
	cDetectObjectInfo pDetectInfo;
	cProtectionSettings pSettings;
	pSettings.m_nScanAction = SCAN_ACTION_DISINFECT;
	pSettings.m_bTryDelete = cFALSE;
	pSettings.m_bTryDisinfect = cTRUE;
	pSettings.m_bUseIChecker = cFALSE;
	pSettings.m_bUseIStreams = cFALSE;

	m_cure_mode = true;
	m_obj_actions = 0;

	error = m_hAVSSession->ProcessObject((hOBJECT) scan_results.m_cured_io, &pScanInfo, &pSettings, &pDetectInfo);

	if (m_obj_handler_pfn != 0)
	{
		pDetectInfo.m_strObjectName = sName;
		m_obj_handler_pfn (m_obj_handler_ctx, (cIO*)scan_results.m_cured_io, &pDetectInfo);
	}

	if (PR_FAIL (error))
	{
		return error;
	}

	if( (pScanInfo.m_nProcessStatusMask & cScanProcessInfo::DISINFECTED) != 0 ) // was disinfected
	{
		scan_results.m_disinfected = true;
	}

	return errOK;
}

void AVS_AsyncResult(cObj* pObj, cScanProcessInfo* pPInfo, cDetectObjectInfo* pDInfo)
{
	pObj->sysCloseObject();
}

void AVS_Scanner::process_io (
			cIOObj & hIO,
			const Scan_Options& scan_opts,
			Scan_Results& scan_results
		)
{
	tERROR error = errOK;
	scan_results.clear ();

	m_opt_flags = scan_opts.m_opt_flags;

	m_cure_mode = false;
	m_obj_actions = 0;

	bool cure_test_mode = (m_opt_flags & OPT_CURE_TEST) != 0;

//	m_statistics.cnt_scanned ++;

	if (PR_FAIL (hIO.last_error()))
	{
		if((m_opt_flags & OPT_DSUT_MODE) != 0)
			return;

		scan_results.m_error = hIO.last_error();
		scan_results.m_error_text.assign (KL_TEXT ("cannot open file"), & g_malloc);
		m_statistics.cnt_errors ++;
		return;
	}

	if ((m_opt_flags & OPT_DSUT_MODE) != 0 && m_obj_handler_pfn != 0)
		m_obj_handler_pfn (m_obj_handler_ctx, (cIO*)hIO, NULL);

	cScanProcessInfo  pScanInfo;
	cDetectObjectInfo pDetectInfo;
	cProtectionSettings pSettings;
	if( m_opt_flags & OPT_STOP_ON_DETECT )
		pSettings.m_nScanAction = SCAN_ACTION_DETECT;
	else
		pSettings.m_nScanAction = SCAN_ACTION_ASKUSER;
	pSettings.m_bTryDelete = cFALSE;
	pSettings.m_bTryDisinfect = cFALSE;
	pSettings.m_bUseIChecker = cFALSE;
	pSettings.m_bUseIStreams = cFALSE;

	if ((m_opt_flags & OPT_USE_IMUL) != 0)
	{
		pSettings.m_bEmulEnable = 1;
		pSettings.m_nEmulValue = scan_opts.m_heur_level;
	}
	else
		pSettings.m_nEmulValue = 7;

	if (scan_opts.m_time_limit)
		pSettings.m_nTimeLimit = scan_opts.m_time_limit;

	if( m_opt_flags & OPT_ASYNC_SCAN )
		pScanInfo.m_fnAsyncResult = AVS_AsyncResult;

	scan_results.m_error = m_hAVSSession->ProcessObject(hIO, &pScanInfo, &pSettings, &pDetectInfo);

	if( m_opt_flags & OPT_ASYNC_SCAN )
	{
		hIO.relinquish();
		return;
	}

	if( m_opt_flags & OPT_DSUT_MODE )
		return;

	if( pScanInfo.m_nProcessStatusMask & cScanProcessInfo::DETECTED ) // was detection
	{
		scan_results.m_detected = true;
		scan_results.m_detect_name.assign (pDetectInfo.m_strDetectName.data (), & g_malloc);
		m_statistics.cnt_detected ++;
		if( pScanInfo.m_nProcessStatusMask & cScanProcessInfo::DETECTED_SURE ) // exact detection
			m_statistics.cnt_detected_exact ++;
	}

	if ((m_obj_actions & ACTION_DISINFECT) != 0 && (m_opt_flags & OPT_CURE_TEST) != 0)
	{
		// cure-test mode: create TEMP IO, cure the object
		error = test_cure_io (hIO, scan_results);
		if (PR_FAIL (error))
		{
			scan_results.m_error_text.assign (KL_TEXT ("error disinfecting file"), & g_malloc);
			m_statistics.cnt_disinfect_errors ++;
		}
		else if (scan_results.m_disinfected)
		{
			m_statistics.cnt_disinfected ++;
		}
	}

	if (PR_FAIL (scan_results.m_error))
	{
		scan_results.m_error_text.assign (KL_TEXT ("processing error"), & g_malloc);
		m_statistics.cnt_errors ++;
	}
}

void AVS_Scanner::process_file (
			const kl_tchar_t * fname,
			const Scan_Options& scan_opts,
			Scan_Results& scan_results
		)
{
	tERROR error = errOK;
	scan_results.clear ();

	m_opt_flags = scan_opts.m_opt_flags;

	m_cure_mode = false;
	m_obj_actions = 0;

	bool cure_test_mode = (m_opt_flags & OPT_CURE_TEST) != 0;

	m_statistics.cnt_scanned ++;

	cObj* pParent = (cObj*)m_hAVS;
	if( m_opt_flags & OPT_ASYNC_SCAN )
	{
		if( !m_hAsyncCtx || !(m_statistics.cnt_scanned % 500) )
		{
			if( m_hAsyncCtx )
				m_hAsyncCtx->sysCloseObject();
			m_hAVS->sysCreateObject((hOBJECT*)&m_hAsyncCtx, IID_MSG_RECEIVER);
		}
		pParent = m_hAsyncCtx;
	}

	tDWORD flags = fACCESS_READ;
	if( m_engine_flags & ENGINE_KLAV )
		flags |= fACCESS_FILE_FLAG_NO_BUFFERING;

	cIOObj hIO(pParent, cAutoString(cStrObj(fname)), flags, fOMODE_OPEN_IF_EXIST);

	process_io(hIO, scan_opts, scan_results);
}

void AVS_Scanner::process_running_processes (
								const Scan_Options& scan_opts,
								Scan_Results& scan_results
								)
{
	tERROR error = errOK;
	scan_results.clear ();

	m_opt_flags = scan_opts.m_opt_flags;

	m_cure_mode = false;
	m_obj_actions = 0;

	bool cure_test_mode = (m_opt_flags & OPT_CURE_TEST) != 0;

	m_statistics.cnt_scanned ++;

	//cIOObj hIO((cObj*)m_hAVS, cAutoString(cStrObj(fname)), fACCESS_READ, fOMODE_OPEN_IF_EXIST);

	cAutoObj<cOS> hProcessesOS;
	error = m_hAVSSession->sysCreateObjectQuick((hOBJECT*)&hProcessesOS, IID_OS, PID_MEMSCAN);

	cAutoObj<cObjPtr> hProcessesObjPtr;
	if( PR_SUCC(error) )
		error = hProcessesOS->PtrCreate(&hProcessesObjPtr, NULL);

	while(PR_SUCC(error) && PR_SUCC(hProcessesObjPtr->Next()) )
	{ 
		bool own_process = (hProcessesObjPtr->propGetDWord(plPID) == PrGetProcessId());

		cAutoObj<cOS> hModulesOS;
		if( PR_FAIL(hProcessesObjPtr->sysCreateObjectQuick((hOBJECT*)&hModulesOS, IID_OS, PID_MEMMODSCAN)) )
			continue;

		cAutoObj<cObjPtr> hModulesObjPtr;
		if( PR_SUCC(hModulesOS->PtrCreate(&hModulesObjPtr, 0)) )
		{
			if( own_process )
			{
				// skip EXE file
				// (on error skip whole process)
				if (PR_FAIL(hModulesObjPtr->Next()))
					continue;
			}

			//Modules in process loop
			while(PR_SUCC(hModulesObjPtr->Next()))
			{
				cIOObj hModuleIO;
				if( PR_SUCC(hModulesObjPtr->IOCreate(&hModuleIO, 0, fACCESS_READ, fOMODE_OPEN_IF_EXIST )) )
					process_io(hModuleIO, scan_opts, scan_results);

				process_file(cStrObj(hModulesObjPtr, pgOBJECT_PATH).c_str(cCP_UNICODE), scan_opts, scan_results);

				if( !own_process )
					break;
			}
		}
	}
}
