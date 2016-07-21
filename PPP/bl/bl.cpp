// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  09 April 2007,  15:29 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- bl.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define ProductLogic_VERSION ((tVERSION)1)
// AVP Prague stamp end



#if defined (_WIN32)
#include <windows.h>
#endif

#define IMPEX_IMPORT_LIB
#include <plugin/p_wmihlpr.h>
#include <plugin/p_win32_reg.h>
#include <plugin/p_win32_nfio.h>
#include <plugin/p_cryptohelper.h>
#include <plugin/p_hash_md5.h>

// AVP Prague stamp begin( Includes for interface,  )
#include "bl.h"
// AVP Prague stamp end


#if defined (_WIN32)
#include <../windows/hook/klop/kloplib/kloplib.h>
#include <../windows/hook/hook/FSSync.h>

typedef BOOL FS_PROC_EXPORT (FS_PROC *tFSDrv_Init)();
typedef void FS_PROC_EXPORT (FS_PROC *tFSDrv_Done)();
typedef BOOL FS_PROC_EXPORT (FS_PROC *tFSDrv_AddInvThreadByHandle)(HANDLE hThread);
typedef BOOL FS_PROC_EXPORT (FS_PROC *tFSDrv_DelInvThreadByHandle)(HANDLE hThread);
#endif

#include <environment.h>
#include <Updater/updater.h>
#include <structs/s_profiles.h>
#include <iface/i_mutex.h>
#include <iface/i_report.h>
#include <iface/i_qb.h>
#include <plugin/p_netdetect.h>
#include <structs/ProcMonM.h>
#include <NetDetails/structs/s_netdetails.h>
#include <structs/s_httpscan.h>


#define BL_SCHEDULE_SEND_MAIL_NOTIFICATION  0x84a7b93a
#define BL_SCHEDULE_REGISTER_DLL            0x84a7b93b
#define BL_SCHEDULE_CLEANUP                 0x84a7b93c
#define BL_SCHEDULE_CHECK_LIC               0x84a7b93d
#define BL_UPDATE_PRODUCT_STARTUP_MODE      0x84a7b93e
#define BL_SCHEDULE_CHECK_NAGENT            0x84a7b93f
#define BL_SCHEDULE_CHECK_NET_STATUS        0x84a7b940

#define timerREGISTER_DLL_FREQ    2*60*60
#define timerCLEANUP_FREQ         6*60*60
#define timerCHECK_LIC_FREQ       5*60
#define timerCHECK_NAGENT_FREQ    5*60

#define PRODUCT_REPORT            "eventlog"

const tCHAR* g_dlls[] = {"scrchpg.dll", "mcou.dll", "mcouas.dll", "ShellEx.dll"};

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "ProductLogic". Static(shared) property table variables
// AVP Prague stamp end



inline bool _cmp( const tCHAR* src, const tCHAR* to_cmp ) { return !strcmp(src, to_cmp); }

// ---
tERROR pr_call BlImpl::startup()
{
#if defined (_WIN32)
	cStringObj strPath; ExpandEnvironmentString("%ProductFolder%", strPath);
	strPath.erase(2);
	if( GetDriveType(strPath.c_str(cCP_ANSI)) == DRIVE_CDROM )
		m_bIsCDInstallation = cTRUE;
#elif defined (__unix__)
	#warning "Consideration is needed!"
#else
	#error "Platform is not supported"
#endif

	{
		tERROR err = m_tm->sysCreateObjectQuick((hOBJECT*)&m_pReport, IID_REPORTDB, PID_REPORTDB);
		
		if( PR_SUCC(err) ) err = m_pReport->InitDB(dbRTP, sizeof(tReportEvent));
		if( PR_SUCC(err) ) err = m_pReport->InitDB(dbScan, sizeof(tReportEvent));
		if( PR_SUCC(err) ) err = m_pReport->InitDB(dbUpdate, sizeof(tReportEvent));
		if( PR_SUCC(err) ) err = m_pReport->InitDB(dbNetworkPackets, sizeof(tSnifferFrameReport));
		if( PR_SUCC(err) ) err = m_pReport->InitDB(dbProcesses, sizeof(tProcessInfo));
		
		
		if( PR_FAIL(err) )
			PR_TRACE((this, prtIMPORTANT, "bl\tstartup: global report create error: 0x%08x", err));
	}

	m_tm->GetService(TASKID_TM_ITSELF, *this, sid_TM_LIC, (cObject**)&m_pLic);
	update_licensing_state();

	if( m_settings.m_Ins_InitMode && PrWmihInstall )
	{
		CGuiLoader _GuiLoader; CRootItem * pRoot = _GuiLoader.CreateAndInitRoot(::g_root, GUIFLAG_CFGSTG_RO, this);
		
		cStrObj strProductName;
		if( pRoot )
			pRoot->GetFormatedText(strProductName, pRoot->GetString(TOR_tString, PROFILE_LOCALIZE, NULL, "ProductName"), (cSerializable *)NULL);
		else
			strProductName = VER_PRODUCTNAME_STR;
		
		cStrObj strProductVersion; ExpandEnvironmentString("%ProductVersion%", strProductVersion);
		cStrObj strCompanyName;    ExpandEnvironmentString("%CompanyName%", strCompanyName);

		PrWmihInstall(VER_PRODUCT_WMIH_ID, VER_PRODUCT_WMIH_LEGACYREGKEY, strCompanyName.data(), strProductVersion.data(), strProductName.data());
	
		PrWmihUpdateStatus(VER_PRODUCT_WMIH_ID, wmicAntiVirus, 
			PR_SUCC(m_tm->GetProfileInfo(AVP_PROFILE_FILEMONITOR, &cProfileBase())) ? wmisInstalled : wmisUninstalled);
		PrWmihUpdateStatus(VER_PRODUCT_WMIH_ID, wmicAntiSpyWare,
			PR_SUCC(m_tm->GetProfileInfo(AVP_PROFILE_FILEMONITOR, &cProfileBase())) ? wmisInstalled : wmisUninstalled);
		PrWmihUpdateStatus(VER_PRODUCT_WMIH_ID, wmicAntiHaker,
			PR_SUCC(m_tm->GetProfileInfo(AVP_PROFILE_FIREWALL, &cProfileBase())) ? wmisInstalled : wmisUninstalled);
		
	}

	// BUG FIX 17529: Если номер версии увеличился, сбросим в лицензирование инфу об использовании trial-ключа
	if( m_settings.m_Ins_InitMode && m_hCryptoHelper)
	{
		struct CVer { tDWORD v1, v2, v3, v4; };

		CVer curver;
		cStrObj strVersion; ExpandEnvironmentString("%ProductVersion%", strVersion);
		sscanf((tSTRING)strVersion.c_str(cCP_ANSI), "%d.%d.%d.%d", &curver.v1, &curver.v2, &curver.v3, &curver.v4);

		CVer prevver = { 0, 0, 0, 0 };
		tDWORD size = sizeof(prevver);
		m_hCryptoHelper->GetData(esiLastTrialClearVer, &prevver, &size);
		if (curver.v1 > prevver.v1
			|| (curver.v1 == prevver.v1
				&& (curver.v2 > prevver.v2 || curver.v2 == prevver.v2 && curver.v3 > prevver.v3)))
		{
			if (m_pLic && PR_SUCC(m_pLic->ClearTrialLimitDate()))
			{
				m_hCryptoHelper->StoreData(esiLastTrialClearVer, &curver, sizeof(curver));
				PR_TRACE((this, prtIMPORTANT, "bl\ttrial period extended"));
			}
		}
	}

	if( m_settings.m_Ins_InitMode )
	{
		// Импортируем CustomerId
		if (m_settings.m_strCustomerId.empty())
			MsgReceive(pmc_LICENSING, pm_CUSTOMER_ID_OBTAINED, 0, 0, 0, 0, 0);
		PR_TRACE((this, prtIMPORTANT, "bl\tstartup(install mode)"));
//		m_tm->SetProfileState(AVP_PROFILE_SCAN_STARTUP, TASK_REQUEST_RUN, NULL, 0);
		return errNOT_OK;
	}

	PR_TRACE((this, prtIMPORTANT, "bl\tstartup(default mode)"));
	
    ckahum_reload(cTRUE, cTRUE);

	update_klop_state(true);

	m_tm->GetService(TASKID_TM_ITSELF, *this, sid_TM_AVS, (cObject**)&m_pAvs);
	m_tm->GetService(TASKID_TM_ITSELF, *this, sid_TM_SCHEDULER, (cObject**)&m_scheduler);
	m_tm->GetService(TASKID_TM_ITSELF, *this, AVP_SERVICE_PROCESS_MONITOR, (cObject**)&m_hProcMon);
	
	cThreadPoolBase::init(this, 1);

	init_schedule(BL_SCHEDULE_REGISTER_DLL, m_settings.m_nRegisterDllPeriod ?
		m_settings.m_nRegisterDllPeriod : timerREGISTER_DLL_FREQ, 60);

	init_schedule(BL_SCHEDULE_CLEANUP, timerCLEANUP_FREQ, 5*50);
	init_schedule(BL_SCHEDULE_CHECK_LIC, timerCHECK_LIC_FREQ, timerCHECK_LIC_FREQ);
	init_schedule(BL_UPDATE_PRODUCT_STARTUP_MODE, 60, 15);
	init_schedule(BL_SCHEDULE_CHECK_NAGENT, timerCHECK_NAGENT_FREQ, timerCHECK_NAGENT_FREQ);
	init_schedule(BL_SCHEDULE_CHECK_NET_STATUS, 60, 15);
	
	if ( m_settings.m_bQuarantineStartupScan ) {
		m_settings.m_bQuarantineStartupScan = cFALSE;
		m_settings.saveOneField( m_config, "settings", &m_settings.m_bQuarantineStartupScan, sizeof(m_settings.m_bQuarantineStartupScan) );
		run_qb_check();
	}
	
	if ( m_settings.m_Ins_FullScan ) {
		m_settings.m_Ins_FullScan = cFALSE;
		m_settings.saveOneField( m_config, "settings", &m_settings.m_Ins_FullScan, sizeof(m_settings.m_Ins_FullScan) );
		m_tm->SetProfileState(AVP_PROFILE_FULLSCAN, TASK_REQUEST_RUN, *this, NULL, NULL);
	}

	set_send_mail_schedule();

	{
		cTaskSchedule sch;
		if ( PR_SUCC(m_tm->GetProfileInfo(AVP_PROFILE_UPDATER, &sch)) && sch.auto_mode() )
			m_bWaitForUpdate = cTRUE;
	}

	bool bSafeBoot = PrIsSafeBoot && PrIsSafeBoot();

	PR_TRACE((this, prtIMPORTANT, "bl\tstartup: update_protection_state begin"));
	update_protection_state(0, bSafeBoot);
	PR_TRACE((this, prtIMPORTANT, "bl\tstartup: update_protection_state done"));

	PR_TRACE((this, prtIMPORTANT, "bl\tstartup: update_avs_state begin"));
	update_avs_state();
	PR_TRACE((this, prtIMPORTANT, "bl\tstartup: update_avs_state done"));

	m_ChartStatistics.Init();
	m_ChartStatistics.Start(this);

	update_as_state(true);

	update_admin_policy_state(NULL);

	if( RegisterCreationInfo )
	{
		RegisterCreationInfo(49000, PR_CREATE_IN_PARENTAL_PROCESS|PR_CREATE_IN_IMPERSONATED_PROCESS, PR_PROCESS_ANY, "avp.exe");
		RegisterCreationInfo(49001, PR_CREATE_IN_PARENTAL_PROCESS, PR_PROCESS_ANY, NULL);
	}

	//	temp call
	//	need to move to update function
	update_hips_settings();

	m_bStartedUp = cTRUE;
	return errOK;
}

tERROR BlImpl::init_components(cProfileEx& profile, cStrObj& strComponents)
{
    ///////////////////////////////////////////
    /// form component list
    cStringObj components = profile.m_sCategory;
	if(!components.empty())
	{
		if(!strComponents.empty())
			strComponents += L";";

	    strComponents += components;
	}

    strComponents.replace_all(";;", fSTRING_COMPARE_CASE_INSENSITIVE, ";");

    // parse all sublying components for current profile
    for(tDWORD i = 0; i < profile.count(); ++i)
	    init_components(*profile[i], strComponents);

	return errOK;
}

tERROR BlImpl::init_schedule(tDWORD sched_id, tDWORD period, tDWORD first_run)
{
	if( !m_scheduler )
		return errUNEXPECTED;

	cScheduleSettings schedule;
	schedule.m_TimeShift.dwSeconds = period;

	Now(&schedule.m_dtNextRun);
	DTTimeShift(&schedule.m_dtNextRun, 0, 0, first_run);
	return m_scheduler->SetSchedule(pmc_SCHEDULER, sched_id, &schedule, NULL);
}

tERROR BlImpl::prepare_to_stop()
{
#if defined (_WIN32)
	if( m_bIsKlopWorking )
		KLOPUM::KlopDone();
#endif
	if( !m_bStartedUp )
		update_product_status();
	else if( m_scheduler )
	{
		// if product update has been scheduled
		if (PR_SUCC(m_scheduler->GetSchedule(pmc_SCHEDULER, BL_UPDATE_PRODUCT_STARTUP_MODE, NULL, NULL)))
			update_product_status();
	}

	m_bIsAVActive = cFALSE;
	update_security_av_state();

	cAutoCS cs(m_lock, true);
	
	if( m_pAvs )
		m_tm->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)m_pAvs), m_pAvs = NULL;
	
	if( m_pLic )
		m_tm->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)m_pLic), m_pLic = NULL;
	
	if( m_scheduler )
	{
		sysUnregisterMsgHandler(pmc_SCHEDULER, m_tm);
		m_tm->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)m_scheduler), m_scheduler = NULL;
	}

	if( m_hProcMon )
		m_tm->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)m_hProcMon), m_hProcMon = NULL;

	m_bIsStopping = cTRUE;
	return errOK;
}

tERROR BlImpl::on_idle()
{
	return errOK;
}

// ---
tERROR pr_call BlImpl::cleanup() {
	cDateTime dt( cDateTime::current_utc );
	
	bool check_time = false;
	cSettingsCheck& rs = m_settings.m_ReportSpan;
	cSettingsCheck& qbs = m_settings.m_QBSpan;
	if ( rs.m_on && rs.m_val ) {
		check_time = true;
		dt.DateShift( 0, 0, -(tINT)m_settings.m_ReportSpan.m_val );
		m_tm->CleanupItems( cCLEAN_REPORTS, dt, *this );
	}
	if ( qbs.m_on && qbs.m_val )
	{
		if ( check_time )
			dt.MakeCurrentUTC();
		dt.DateShift( 0, 0, -(tINT)qbs.m_val );

		clean_QB(dt);
	}
	return errOK;
}

// ---
tERROR BlImpl::registerAllDlls()
{
#ifndef _DEBUG
	PR_TRACE((this, prtNOTIFY, "BL\tregisterAllDlls"));
	if ( !PrLoadLibrary || !PrGetProcAddress )
		return errUNEXPECTED;
	
	for( tUINT i=0; i<countof(g_dlls); ++i ) {
		const tCHAR* dll = g_dlls[i];
		
		// re-register some interceptor
		PrHMODULE hSCMod = PrLoadLibrary( dll, cCP_ANSI );
		if ( !hSCMod ) 
		{
			PR_TRACE(( this, prtERROR, "BL\tCannot find module \"%s\" for re-register", dll ));
		}
		else
		{
			cERROR err;
			typedef tLONG (__stdcall *tDllRegisterServer)(void);
			tDllRegisterServer pDllRegisterServer = (tDllRegisterServer)PrGetProcAddress(hSCMod, "DllRegisterServer");
			if ( !pDllRegisterServer ) 
			{
				PR_TRACE(( this, prtERROR, "BL\tCannot find \"DllRegisterServer\" in module \"%s\"", dll ));
			}
			else
			{
				tLONG hRes = pDllRegisterServer();
				if ( hRes < 0 ) 
				{
					PR_TRACE((this, prtERROR, "BL\tDllRegisterServer in module \"%s\" failed with 0x%08X (re-register)", dll, hRes));
				}
				else
				{
					PR_TRACE((this, prtNOTIFY, "BL\tModule \"%s\" re-registered", dll));
				}
			}
			if ( PrFreeLibrary )
				PrFreeLibrary( hSCMod );
		}
	}
#endif //_DEBUG
	return errOK;
}

void BlImpl::set_send_mail_schedule()
{
	cAutoCS cs(m_lock, true);
	
	if( !m_scheduler )
		return;
	
	cTaskSchedule *schedule = &m_settings.m_mail_schedule, sch;
	bool bAtStartup = (schedule->type() == schtOnStartup);

	if( !m_settings.m_nSendMode || bAtStartup && m_bMailShedule )
	{
		m_scheduler->DeleteSchedule(pmc_SCHEDULER, BL_SCHEDULE_SEND_MAIL_NOTIFICATION);
		return;
	}
	
	if( bAtStartup )
	{
		sch.m_eMode = schmEnabled;
		sch.m_eType = schtExactTime;
		sch.m_nFirstRunTime = cDateTime::now_local();
		sch.m_nFirstRunTime += schedule->m_nStartDelay * 60;
		sch.m_bRaiseIfSkipped = cTRUE;
		schedule = &sch;
	}
	m_scheduler->SetSchedule( pmc_SCHEDULER, BL_SCHEDULE_SEND_MAIL_NOTIFICATION, schedule, NULL );
	m_bMailShedule = cTRUE;
}

void BlImpl::send_mail_notification()
{
	cAutoCS cs(m_lock, true);
	if( m_strMailText.empty() )
		return;
	
	CSendMailInfo pInfo;
	pInfo.m_strSMTPServer = m_settings.m_SMTPServer;
	pInfo.m_nSMPTPort = m_settings.m_SMTPPort;
	pInfo.m_strFrom = m_settings.m_MailFrom;
	pInfo.m_aAddresses.push_back(m_settings.m_MailTo);
	pInfo.m_strSubject = m_strMailSubject;
	pInfo.m_strBody = m_strMailText;
	pInfo.m_strUserName = m_settings.m_Account;
	
	if( tDWORD dwSize = m_settings.m_MailPassword.length() )
	{
		cStrObj strPassw = m_settings.m_MailPassword;
		if( m_hCryptoHelper )
			m_hCryptoHelper->CryptPassword(&cSerString(strPassw), PSWD_DECRYPT);

		pInfo.m_strPassword = strPassw;
	}
	
	send_mail(pInfo);
	m_strMailText.clear();
}

tERROR BlImpl::update_product_status()
{
	cAutoCS cs(m_lock, true);
	tBOOL run = m_settings.m_bRunAtSystemStartup;
	bool bIns_InitMode = !!m_settings.m_Ins_InitMode;
	cs.unlock();

	if( bIns_InitMode )
	{
		PR_TRACE((this, prtIMPORTANT, "bl\tupdating startup mode (%s) skipped in INIT_MODE, waiting another minute...", run ? "true" : "false"));
		return errOK;
	}
	
	tDWORD size = sizeof(run);
	PR_TRACE((this, prtIMPORTANT, "bl\tupdating startup mode (%s)...", run ? "true" : "false"));
	if (PR_SUCC(sysSendMsg( pmc_PRODUCT_STARTUP_CHANGE, pm_PRODUCT_STARTUP_CHANGE, 0, &run, &size )))
	{
		PR_TRACE((this, prtIMPORTANT, "bl\tupdating startup mode succeeded"));
		if (m_scheduler)
			m_scheduler->DeleteSchedule(pmc_SCHEDULER, BL_UPDATE_PRODUCT_STARTUP_MODE);
	}
	else
	{
		PR_TRACE((this, prtIMPORTANT, "bl\tupdating startup mode failed, waiting another minute..."));
	}
	return errOK;
}

tERROR BlImpl::update_klop_state(bool bReinit)
{
#if defined (_WIN32)
	if( m_tmLastFullScanState == (tProfileState)666 )
		return errOK;

    if( m_settings.m_Ins_InitMode ) // no klop status change while in PIW, because of BSOD in KLOP/KLIM
        return errOK;

	cAutoCS cs(m_lock, true);

	tDWORD dwNow = PrGetTickCount();
	if( bReinit || m_bIsKlopLimited && m_dwLastKlopInit + 100000 < dwNow )
	{
		if( m_bIsKlopWorking )
		{
			KLOPUM::KlopDone();
			m_bIsKlopWorking = cFALSE;
		}

		cStrObj strProductType; ExpandEnvironmentString("%ProductType%", strProductType);
		if( m_LicInfo.m_KeyInfo.m_dwLicenseCount &&
			m_LicInfo.m_KeyInfo.m_dwKeyType != ektTrial &&
			m_LicInfo.m_KeyInfo.m_dwKeyType != ektBeta &&
			(strProductType != AVP_PRODUCT_WKS && strProductType != AVP_PRODUCT_FS) )
		{
			PVOID pUnique = &m_LicInfo.m_KeyInfo.m_KeySerNum.m_dwMemberID;
			DWORD dwSize = sizeof(cKeySerialNumber) - offsetof(cKeySerialNumber, m_dwMemberID);

			KLOPUM::OpResult ret = KLOPUM::KlopInit((PCHAR)pUnique, dwSize);
			if( ret == KLOPUM::srOK )
				ret = KLOPUM::KlopActivate(10000);

			if( ret == KLOPUM::srOK )
				m_bIsKlopWorking = cTRUE;
		}
		else
			m_bIsKlopLimited = cFALSE;

		m_dwLastKlopInit = dwNow;
	}
	else if( m_bIsKlopWorking && m_dwLastKlopInit && m_dwLastKlopInit + 10000 < dwNow )
	{
		tBOOL oldVal = m_bIsKlopLimited;

		KLOPUM::KlopGetCount((ULONG*)&m_nKlopKeyCount);
		if( m_nKlopKeyCount > m_LicInfo.m_KeyInfo.m_dwLicenseCount )
		{
			m_bIsKlopLimited = cTRUE;
			m_dwLastKlopInit = dwNow;

			m_bIsKlopWorking = cFALSE;
			KLOPUM::KlopDone();
		}
		else
		{
			m_bIsKlopLimited = cFALSE;
			m_dwLastKlopInit = 0;
		}

		if( oldVal != m_bIsKlopLimited )
			update_licensing_state();
	}
#endif

	return errOK;
}

tERROR BlImpl::update_licensing_state(bool bUpdateBlackList)
{
	cAutoCS cs(m_lock, true);
	if( m_bIsStopping )
		return errOK;

	cStrObj strProductType; ExpandEnvironmentString("%ProductType%", strProductType);
	if ( strProductType == AVP_PRODUCT_AVPTOOL )
		return errOK;
	
	tDWORD nOldState = m_State.m_KeyState;
	tDWORD nOldLevel = m_LicInfo.m_dwFuncLevel;
	tDWORD nOldDaysTillExpiration = m_LicInfo.m_dwDaysTillExpiration;

	m_State.m_KeyState = eKeyOk;

	if( m_bIsCDInstallation )
	{
		m_LicInfo.m_dwFuncLevel = eflFullFunctionality;
		return errOK;
	}

	m_LicInfo = cCheckInfo();
	m_LicInfo.m_dwFuncLevel = eflUnknown;
	m_LicInfo.m_KeyInfo.m_dwLicenseCount = 0;
	m_LicRenewStatus = eLrsOk;

	m_nLicKeyType            = m_LicInfo.m_KeyInfo.m_dwKeyType;
	m_nLicInvalidReason      = m_LicInfo.m_dwInvalidReason;
	m_nLicDaysTillExpiration = m_LicInfo.m_dwDaysTillExpiration;
	
	tERROR error = m_pLic ? m_pLic->CheckActiveKey(&m_LicInfo) : errNO_ACTIVE_KEY;
	if( PR_SUCC(error) )
	{
		m_nLicKeyType            = m_LicInfo.m_KeyInfo.m_dwKeyType;
		m_nLicInvalidReason      = m_LicInfo.m_dwInvalidReason;
		m_nLicDaysTillExpiration = m_LicInfo.m_dwDaysTillExpiration;

		bool bTrial = (m_LicInfo.m_KeyInfo.m_dwKeyType == ektTrial);
		m_State.m_KeyState = bTrial ? eKeyTrial : eKeyOk;
		
		if( m_bIsKlopLimited )
		{
			m_LicInfo.m_dwFuncLevel = eflNoFeatures;
			check_status_changed(eNtfLicenseProblems, &m_State.m_KeyState, nOldState, eKeyLimited);
		}
		else if( m_LicInfo.m_dwInvalidReason == ekirValid )
		{
			if( !bTrial )
			{
				if( m_LicInfo.m_dwDaysTillExpiration < m_settings.m_nKeyAboutExpirationPeriod )
					check_status_changed(eNtfLicenseExpiresSoon, &m_State.m_KeyState, nOldState, eKeyAboutExpiration);
				else if( m_LicInfo.m_dwDaysTillExpiration < m_settings.m_nKeyWillBeExpiredPeriod )
					check_status_changed(eNtfLicenseExpiresSoon, &m_State.m_KeyState, nOldState, eKeyWillBeExpired);
				}
			}
		else if( m_LicInfo.m_dwInvalidReason == ekirExpired )
		{
			if( bTrial )
				check_status_changed(eNtfLicenseExpired, &m_State.m_KeyState, nOldState, eKeyTrialExpired);
			else
				check_status_changed(eNtfLicenseExpired, &m_State.m_KeyState, nOldState, eKeyExpired);
		}
		else if( m_LicInfo.m_dwInvalidReason == ekirBlackListed )
			check_status_changed(eNtfLicenseProblems, &m_State.m_KeyState, nOldState, eKeyBlocked);
		else
		{
			check_status_changed(eNtfLicenseProblems, &m_State.m_KeyState, nOldState, eKeyInvalid);
			PR_TRACE((this, prtERROR, "bl\tCheckActiveKey: key invalid m_LicInfo.m_dwInvalidReason=%d", m_LicInfo.m_dwInvalidReason));
		}
		
		if( (m_LicInfo.m_KeyInfo.m_dwKeyType != ektOEM) &&
			(m_LicInfo.m_KeyInfo.m_dwKeyType != ektCommercial) )
			m_LicRenewStatus = eLrsBuy;
		else if( m_State.m_KeyState & (eKeyAboutExpiration|eKeyWillBeExpired|eKeyExpired) )
			m_LicRenewStatus = eLrsRenew;
	}
	else
	{
		if( m_pLic )
			PR_TRACE((this, prtERROR, "bl\tCheckActiveKey failed with %terr", error));

		m_LicRenewStatus = eLrsBuy;
		
		if( error == errNO_ACTIVE_KEY )
		{
			cLicInfo licInfo;
			if( m_pLic )
				m_pLic->GetInfo(&licInfo);

			if( licInfo.m_blTrialPeriodOver )
			{
				m_LicInfo.m_dwFuncLevel = eflNoFeatures;
				check_status_changed(eNtfLicenseExpired, &m_State.m_KeyState, nOldState, eKeyTrialExpired);
			}
			else
			{
				m_LicInfo.m_dwFuncLevel = m_bIsUpdatePerformed ? eflFunctionWithoutUpdates : eflFullFunctionality;
				check_status_changed(eNtfLicenseProblems, &m_State.m_KeyState, nOldState, eNoKeys);
			}
		}
		else
		{
			m_LicInfo.m_dwFuncLevel = eflNoFeatures;
			check_status_changed(eNtfLicenseProblems, &m_State.m_KeyState, nOldState, eKeyInvalid);
		}
	}

	if( m_hCryptoHelper && PR_FAIL(check_blacklist(NULL, bUpdateBlackList)) )
		if( m_LicInfo.m_dwInvalidReason == ekirValid )
		{
			m_nLicInvalidReason = m_LicInfo.m_dwInvalidReason = ekirInvalidBlacklist;
			check_status_changed(eNtfLicenseProblems, &m_State.m_KeyState, nOldState, eKeyInvalid);
		}
	
	if( m_bStartedUp && m_LicInfo.m_dwFuncLevel != nOldLevel )
	{
		if( m_LicInfo.m_dwFuncLevel == eflFullFunctionality || m_LicInfo.m_dwFuncLevel == eflFunctionWithoutUpdates )
		{
			PR_TRACE(( this, prtIMPORTANT, "bl\tLicence has updated, starting protection..." ));
			m_tm->SetProfileState( AVP_PROFILE_PRODUCT, TASK_REQUEST_RUN, *this, cREQUEST_DONOT_FORCE_RUN, NULL);
		}
		else
		{
			PR_TRACE(( this, prtIMPORTANT, "bl\tLicence has expired, stopping protection..." ));
			m_tm->SetProfileState( AVP_PROFILE_PRODUCT, TASK_REQUEST_STOP, *this, 0, NULL);
		}
	}

	PR_TRACE((this, prtIMPORTANT, "bl\tfunctionality level is %u", m_LicInfo.m_dwFuncLevel));
	if( nOldState != m_State.m_KeyState || nOldDaysTillExpiration != m_LicInfo.m_dwDaysTillExpiration )
		sysSendMsg(pmc_PRODUCT, pm_PRODUCT_STATE_CHANGED, NULL, NULL, NULL);
	
	return errOK;
}							

tERROR pr_call BlImpl::update_avs_state(bool bForUpdate) {
	if( !m_pAvs )
		return errOK;
	
	cAVSStatistics pStat;
	m_pAvs->GetStatistics(&pStat);
	
	cAutoCS cs(m_lock, true);
	
	tDWORD nOldUpdateState = m_State.m_UpdateState, nOldAutoState = m_State.m_UpdateState;
	tDWORD nOldThreatsState = m_State.m_ThreatsState;
	
	m_State.m_UpdateState &= ~(eBasesNotActual|eBasesOutOfDate|eBasesNotValid|eBasesCorrupted);
	if( bForUpdate )
	{
		nOldAutoState = m_State.m_UpdateState;
		m_bWaitForUpdate = cFALSE;
	}

	m_State.m_ThreatsState &= ~(eVulnerabilityDetected|eThreatsUntreated);
	
	m_errLoadBases = pStat.m_errLoadBases;

	if( pStat.m_nBasesState & bsCorrupted )
		check_status_changed(eNtfDatabaseCorrupted, &m_State.m_UpdateState, nOldAutoState, eBasesCorrupted);
	else if( pStat.m_nBasesState & bsInvalid )
		check_status_changed(eNtfDatabaseCorrupted, &m_State.m_UpdateState, nOldAutoState, eBasesNotValid);
	
	if( pStat.m_nNumThreats )
	{
		if( !pStat.m_nNumUntreated )
			m_State.m_ThreatsState &= ~eThreatsUntreated;
		else
			check_status_changed(eNtfThreatsUntreated, &m_State.m_ThreatsState, nOldThreatsState, eThreatsUntreated);
	}
	
	if( pStat.m_nBasesDateDaysOld == -1 || pStat.m_nBasesDateDaysOld > m_settings.m_nBasesNotActualPeriod )
		check_status_changed(eNtfAVBasesObsolete, &m_State.m_UpdateState, nOldAutoState, eBasesNotActual);
	else if( pStat.m_nBasesDateDaysOld > m_settings.m_nBasesOutOfDatePeriod )
		check_status_changed(eNtfAVBasesOutOfDate, &m_State.m_UpdateState, nOldAutoState, eBasesOutOfDate);
	
	if( (nOldUpdateState & (eBasesOutOfDate|eBasesNotActual)) != (m_State.m_UpdateState & (eBasesOutOfDate|eBasesNotActual)) )
		update_security_av_state();
	
	if( (nOldUpdateState & eBasesCorrupted) && !(m_State.m_UpdateState & eBasesCorrupted) )
		ensure_avs_tasks_running();
	
	if( nOldUpdateState != m_State.m_UpdateState || nOldThreatsState != m_State.m_ThreatsState )
	{
		if (nOldUpdateState != m_State.m_UpdateState)
			PR_TRACE((this, prtIMPORTANT, "bl\tupdate state changed from %08x to %08x", nOldUpdateState, m_State.m_UpdateState));
		sysSendMsg(pmc_PRODUCT, pm_PRODUCT_STATE_CHANGED, NULL, NULL, NULL);
	}
	return errOK;
}

tERROR pr_call BlImpl::run_qb_check()
{
	cQB* qb = 0;
	cERROR err;
	tDWORD qb_count = 1;
	if ( PR_SUCC(m_tm->GetService(TASKID_TM_ITSELF,*this,sid_TM_QUARANTINE,(cObject**)&qb)) && qb ) {
		qb->GetCount( &qb_count, cCOUNT_TOTAL );
		m_tm->ReleaseService( TASKID_TM_ITSELF, (cObj*)qb );
	}
	if ( qb_count )
		err = m_tm->SetProfileState( AVP_PROFILE_SCANQUARANTINE, TASK_REQUEST_RUN, NULL, 0, NULL);
	return err;
}



// ---
tERROR pr_call BlImpl::update_run_tasks_on_update_avs_bases()
{
	if( m_pUpdSett.m_bRescanQuarantineAfterUpdate )
		run_qb_check();

	cProfileEx ProfileTree(cCfg::fSchedule);
	if( PR_SUCC(m_tm->GetProfileInfo(NULL, &ProfileTree)) )
	{
		for(tUINT i = 0; i < ProfileTree.count(); i++)
		{
			cProfileEx *pProfile = ProfileTree[i];
			if( pProfile->m_sType != AVP_TASK_ONDEMANDSCANNER )
				continue;

			if( pProfile->m_cfg.m_schedule.m_eMode == schmEnabled &&
				pProfile->m_cfg.m_schedule.m_eType == schtAfterUpdate )
				m_tm->SetProfileState(pProfile->m_sName.c_str(cCP_ANSI), TASK_REQUEST_RUN, NULL, 0, NULL);
		}
	}
	return errOK;
}



tERROR pr_call BlImpl::update_as_state(bool bStopped)
{
	cAutoCS cs(m_lock, true);
	tDWORD nOldState = m_State.m_ThreatsState;
	tDWORD nOldHam = m_pASStat.m_bNeedMoreHamToTrain;
	
	if( bStopped || PR_FAIL(m_tm->GetProfileInfo(AVP_PROFILE_ANTISPAM, &m_pASStat)) )
		m_State.m_ThreatsState &= ~eAntiSpamNeedTraining;
	else if( m_pASStat.m_bNeedMoreHamToTrain /*|| m_pASStat.m_bNeedMoreSpamToTrain*/ )
		m_State.m_ThreatsState |= eAntiSpamNeedTraining;
	else
		m_State.m_ThreatsState &= ~eAntiSpamNeedTraining;
	
	if( nOldState != m_State.m_ThreatsState || nOldHam != m_pASStat.m_bNeedMoreHamToTrain )
		sysSendMsg(pmc_PRODUCT, pm_PRODUCT_STATE_CHANGED, NULL, NULL, NULL);
	return errOK;
}

// ---
tERROR pr_call BlImpl::ensure_avs_tasks_running()
{
	m_tm->SetProfileState( AVP_PROFILE_FILEMONITOR, TASK_REQUEST_RUN, *this, cREQUEST_DONOT_FORCE_RUN);
	m_tm->SetProfileState( AVP_PROFILE_MAILMONITOR, TASK_REQUEST_RUN, *this, cREQUEST_DONOT_FORCE_RUN);
	m_tm->SetProfileState( AVP_PROFILE_WEBMONITOR,  TASK_REQUEST_RUN, *this, cREQUEST_DONOT_FORCE_RUN);
	return errOK;
}


// ---
tERROR pr_call BlImpl::update_profile_state( cProfileEx& pInfo, tDWORD level )
{
	if( !level )
	{
		m_strScanningTask.clear();
		m_strScanningTaskId = -1;
		m_State.m_ProtectionState &= ~(eOnDemandTaskRunning|eTasksNotRunning|eTasksMalfunction);
		m_State.m_ProductState &= ~(eTasksDisabledByPolicy);
	}

	if ( level && pInfo.persistent() && !pInfo.isService() )
		m_State.m_ProtectionState &= ~eProtectionNotInstalled;

	if ( !pInfo.enabled() )
	{
		if( level <= 1 && pInfo.m_sName != AVP_PROFILE_STATISTICS )
		{
			m_State.m_ProtectionState |= eTasksDisabled;
			m_strTaskDisabled = pInfo.m_sName;

			if( pInfo.m_cfg.m_dwMandatoriedFields.IsBitSet(m_dwEnabledPolicyBit) )
				m_State.m_ProductState |= eTasksDisabledByPolicy;
		}
	}
	else if( !(pInfo.state() & STATE_FLAG_OPERATIONAL) )
	{
		if( pInfo.state() == PROFILE_STATE_FAILED && !pInfo.isService() && pInfo.persistent() )
		{
			m_State.m_ProtectionState |= eTasksMalfunction;
			m_errTaskMalfunction = pInfo.m_lastErr;
			m_strTaskMalfunction = pInfo.m_sName;
		}
		
		if ( !pInfo.isService() && pInfo.m_bPersistent && level <= 1 )
		{
			m_State.m_ProtectionState |= eTasksNotRunning;
			m_strTaskNotRunning = pInfo.m_sName;
		}
	}
	else
	{
		if ( pInfo.m_sType == AVP_TASK_ONDEMANDSCANNER || pInfo.m_sType == AVP_TASK_UPDATER )
		{
			m_State.m_ProtectionState |= eOnDemandTaskRunning;
			m_strScanningTask = pInfo.m_sName;
			m_strScanningTaskId = pInfo.m_nRuntimeId;
			if( m_strScanningTask == AVP_PROFILE_FULLSCAN )
				m_bFullScanStarted = cTRUE;
		}

		if( pInfo.m_sType == AVP_TASK_ONACCESSSCANNER/* ||
			pInfo.m_sType == AVP_TASK_MAILCHECKER ||
			pInfo.m_sType == AVP_TASK_WEBMONITOR ||
			pInfo.m_sType == AVP_TASK_PDM*/ )
			m_bIsAVActive = cTRUE;
	}
	
	tDWORD nCount = pInfo.count();
	for(tDWORD i = 0; i < nCount; i++)
	{
		cProfileEx* pChild = pInfo[i];
		if( pChild )
			update_profile_state(*pChild, level+1);
	}
	
	return errOK;
}

tERROR pr_call BlImpl::update_profile_state( cProfileBase* tp, tDWORD nOldUpdateState )
{
	tProfileState state = tp->state();

	enNotification nProfileNotification = eNtfNone;
	
	if( tp->isTask() )
	{
		tProfilesStates::iterator it = m_mProfilesStates.find(tp->m_sName.data());
		if( it == m_mProfilesStates.end() )
			it = m_mProfilesStates.insert(tProfilesStates::value_type(tp->m_sName.data(), (tProfileState)0)).first;
		tProfileState old_state = it->second;
		it->second = state;

		if( !(old_state & STATE_FLAG_MALFUNCTION) && (state & STATE_FLAG_MALFUNCTION) )
			if( !(state & STATE_FLAG_OPERATIONAL) && tp->m_sType != AVP_TASK_UPDATER )
				nProfileNotification = eNtfTaskCanNotExecute;
	}

	process_notification(nProfileNotification, tp);

	if( state == PROFILE_STATE_FAILED )
		update_avs_state();

	if( m_settings.m_bKeepRecentReports )
		if( state == PROFILE_STATE_STARTING /*&&
				tp->m_sType != AVP_TASK_UPDATER &&
				tp->m_sType != AVP_TASK_ROLLBACK */)
			m_tm->DeleteTaskReports(tp->m_nRuntimeId, cREPORT_DELETE_PREV);
		
	bool rollback = false;
	if( tp->m_sType == AVP_TASK_ONDEMANDSCANNER )
	{
		if( state == PROFILE_STATE_COMPLETED )
		{
			if( tp->m_sName == AVP_PROFILE_FULLSCAN && PR_SUCC(check_my_computer_was_scanned(tp)) )
			{
				PR_TRACE((this, prtIMPORTANT, "bl\tfull scan completed"));
				m_tmLastSuccessfulFullScan = cDateTime::now_utc();
				saveOneField( m_config, "data", &m_tmLastSuccessfulFullScan, sizeof(m_tmLastSuccessfulFullScan));

				tDWORD nSize = sizeof(m_tmLastSuccessfulFullScan);
				sysSendMsg(pmc_PRODUCT, pm_FULLSCAN_COMPLETED, NULL, &m_tmLastSuccessfulFullScan, &nSize);
			}
		}
	}	
	else if ( tp->m_sName == AVP_PROFILE_ANTISPAM )
	{
		if( state != PROFILE_STATE_STARTING )
			update_as_state(!(state & STATE_FLAG_ACTIVE));
	}
	else if ( tp->m_sName == AVP_PROFILE_ROLLBACK )
	{
		if ( state == PROFILE_STATE_COMPLETED )
		{
			m_tmLastSuccessfulUpdate = -1;
			update_licensing_state(true);
		}
	}
	else if ( tp->m_sType == AVP_TASK_UPDATER )
	{
		if( state == PROFILE_STATE_RUNNING )
		{
			m_tm->GetTaskInfo(tp->m_nRuntimeId, &m_pUpdSett);
		}
		else if( IS_TASK_FINISHED(state) )
		{
			cTaskSchedule sch;
			bool bAutoUpdate = PR_SUCC(m_tm->GetTaskInfo(tp->m_nRuntimeId, &sch)) && sch.auto_mode();

			if( state == PROFILE_STATE_FAILED )
			{
				if ( bAutoUpdate )
					m_bAutoUpdateFailed = cTRUE;
			}
			else if( state == PROFILE_STATE_COMPLETED )
			{
				m_bAutoUpdateFailed = cFALSE;
				m_tmLastSuccessfulUpdate = cDateTime::now_utc();
				saveOneField( m_config, "data", &m_tmLastSuccessfulUpdate, sizeof(m_tmLastSuccessfulUpdate));
			}

			time_t now = cDateTime::now_utc();
			if(state == PROFILE_STATE_COMPLETED ||  tp->m_lastErr == errBAD_SIGNATURE)
				m_tmNextAutoUpdate = now + m_dwUpdateAutoPeriodSuccess*60;
			else
				m_tmNextAutoUpdate = -1;

			saveOneField( m_config, "data", &m_tmNextAutoUpdate, sizeof(m_tmNextAutoUpdate));

			update_avs_state(!!m_bWaitForUpdate);

			if( bAutoUpdate )
				m_bWaitForUpdate = cTRUE;

//			update_licensing_state(tp->m_lastErr != errBLACKLIST_CORRUPTED);
		}
	}
	return errOK;
}

// ---
tERROR pr_call BlImpl::update_protection_state( cProfileBase* tp, bool bSafeMode )
{
	cProfileEx pInfo( 0 );
	tERROR error = m_tm->GetProfileInfo(AVP_PROFILE_PRODUCT, &pInfo);
	if( PR_FAIL(error) )
		return error;
	
	cAutoCS cs(m_lock, true);
	
	tDWORD nOldState = m_State.m_ProtectionState;
	tDWORD nOldUpdateState = m_State.m_UpdateState;
	tDWORD nOldProductState = m_State.m_ProductState;
	tBOOL  bIsAVActive = m_bIsAVActive;
	tBOOL  bIsProtectionStarted = m_bIsProtectionStarted;
	cStrObj strOldTask = m_strScanningTask;

	m_bIsAVActive = cFALSE;
	m_bIsProtectionStarted = cFALSE;
	
	m_State.m_ProtectionState = eProtectionNotInstalled;
	if( bSafeMode || (nOldState & eProtectionSafeMode) )
		m_State.m_ProtectionState |= eProtectionSafeMode;

	if( tp )
		update_profile_state(tp, nOldUpdateState);

	if( m_tmLastSuccessfulFullScan == -1 )
		m_State.m_ProtectionState |= eMyComputerNotScanned;

	update_profile_state(pInfo, 0);

	if( (m_State.m_ProtectionState & eMyComputerNotScanned) && !m_bFullScanStarted )
		check_status_changed(eNtfMyComputerNotScaned, &m_State.m_ProtectionState, nOldState, eMyComputerNotScanned);
	
	if( bSafeMode )
		check_status_changed(eNtfOtherImportantEvents, &m_State.m_ProtectionState, nOldState, eProtectionSafeMode);

	bool bLicOk = (m_LicInfo.m_dwFuncLevel != eflNoFeatures);

	if( bLicOk && !(m_State.m_ProtectionState & (eProtectionSafeMode|eProtectionNotInstalled)) )
	{
		if( !tp || tp->m_sName == AVP_PROFILE_PROTECTION )
		{
			if( !pInfo.enabled() )
				check_status_changed(eNtfRTPTaskOnOff, &m_State.m_ProtectionState, nOldState, eProtectionDisabled);
			else if( !(pInfo.state() & (STATE_FLAG_OPERATIONAL/*|STATE_FLAG_MALFUNCTION*/)) )
				check_status_changed(eNtfRTPTaskOnOff, &m_State.m_ProtectionState, nOldState, eProtectionNotRunnig);
			else if( pInfo.state() & STATE_FLAG_ACTIVE )
			{
				m_bIsProtectionStarted = cTRUE;
				if( !bIsProtectionStarted )
					process_notification(eNtfRTPTaskOnOff, tp);
			}
		}
		else
		{
			m_State.m_ProtectionState |= (nOldState & (eProtectionDisabled|eProtectionNotRunnig));
			m_bIsProtectionStarted = bIsProtectionStarted;
		}
	}
	
	if( !bLicOk || (m_State.m_ProtectionState & (eProtectionSafeMode|eProtectionNotRunnig|eProtectionDisabled|eProtectionNotInstalled)) )
	{
		m_State.m_ProtectionState &= ~(eTasksDisabled|eTasksNotRunning);
	}

	if( m_bIsAVActive != bIsAVActive )
		update_security_av_state();
	
	if( m_State.m_ProtectionState & eTasksDisabled )
		check_status_changed(eNtfRTPTaskOnOff, &m_State.m_ProtectionState, nOldState, eTasksDisabled);
//	else if( m_State.m_ProtectionState & eTasksNotRunning )
//		check_status_changed(eNtfRTPTaskOnOff, &m_State.m_ProtectionState, nOldState, eTasksNotRunning);

	//  For future implementation.....!!!!!!
	//	if( m_settings.m_BatterySafe.m_on && m_settings.m_BatterySafe.m_val && IsOnBattery() )
	//		check_status_changed(eNtfRTPTaskOnOff, &m_State.m_ProtectionState, nOldState, eOnBatteriesScanOff);
	
	if( (nOldState != m_State.m_ProtectionState) || (nOldUpdateState != m_State.m_UpdateState) || 
		(nOldProductState != m_State.m_ProductState) || 
		(strOldTask != m_strScanningTask) )
		sysSendMsg(pmc_PRODUCT, pm_PRODUCT_STATE_CHANGED, NULL, NULL, NULL);
	
	if( m_State.m_ProtectionState & eTasksMalfunction )
		check_status_changed(eNtfTaskCanNotExecute, &m_State.m_ProtectionState, nOldState, eTasksMalfunction);

	return errOK;
}

tERROR pr_call BlImpl::update_security_av_state()
{
	if( !m_bIsGuiConnected )
	{
		m_bIsNeedUpdateAVState = cTRUE;
		return errOK;
	}

	m_bIsNeedUpdateAVState = cFALSE;
	if( PrWmihUpdateStatus )
	{
		PR_TRACE((this, prtIMPORTANT, "bl\tcall PrWmihUpdateStatusAV begin"));

		tDWORD dwStatus = 0;
		if( m_bIsAVActive )
			dwStatus |= wmisEnabled;

		if( !(m_State.m_UpdateState & (eBasesOutOfDate|eBasesNotActual)) )
			dwStatus |= wmisSignaturesUpToDate;

		PrWmihUpdateStatus(VER_PRODUCT_WMIH_ID, wmicAntiVirus, dwStatus);

		PR_TRACE((this, prtIMPORTANT, "bl\tcall PrWmihUpdateStatusAV done"));
	}

	update_security_as_state();

	return errOK;
}

tERROR pr_call BlImpl::update_security_as_state()
{
	if( !m_bIsGuiConnected )
	{
		m_bIsNeedUpdateASState = cTRUE;
		return errOK;
	}

	m_bIsNeedUpdateASState = cFALSE;
	if( PrWmihUpdateStatus )
	{
		tDWORD dwASStatus = 0;
		if( m_bIsAVActive )
			dwASStatus |= ((m_nDetectMask&DETDANGER_MEDIUM) ? wmisEnabled : 0);

		if( !(m_State.m_UpdateState & (eBasesOutOfDate|eBasesNotActual)) )
			dwASStatus |= wmisSignaturesUpToDate;

		PrWmihUpdateStatus(VER_PRODUCT_WMIH_ID, wmicAntiSpyWare, dwASStatus );
	}

	return errOK;
}

tERROR pr_call BlImpl::update_security_state(cProfileBase* tp)
{
	if( !PrWmihUpdateStatus )
		return errOK;

	tDWORD dwStatus = (tp->m_nState == PROFILE_STATE_RUNNING) ? wmisEnabled : 0;
	if( tp->m_sName == AVP_PROFILE_FIREWALL )
		PrWmihUpdateStatus(VER_PRODUCT_WMIH_ID, wmicAntiHaker, dwStatus);
// 	else if( tp->m_sName == AVP_PROFILE_ANTISPY )
// 		PrWmihUpdateStatus(VER_PRODUCT_WMIH_ID, wmicAntiSpyWare, dwStatus);
	return errOK;
}

// ---
tERROR pr_call BlImpl::update_task_reboot_state()
{
	cAutoCS cs(m_lock, true);

	PR_TRACE((this, prtIMPORTANT, "BL\tupdate_task_reboot_state(): need reboot"));

	tDWORD nOldState = m_State.m_UpdateState;
	check_status_changed(eNtfUpdateNeedReboot, &m_State.m_UpdateState, nOldState & ~eUpdateNeedReboot, eUpdateNeedReboot);
	if( nOldState != m_State.m_UpdateState )
	{
		sysSendMsg(pmc_PRODUCT, pm_PRODUCT_STATE_CHANGED, NULL, NULL, NULL);
		// bf 24125
		if (m_data)
			PR_VERIFY_SUCC(::g_root->RegSerializeField( (cBLStatistics*)this, &m_State.m_UpdateState, m_data, NULL, 0 ));
	}
	return errOK;
}

// ---
tERROR pr_call BlImpl::update_customize_settings(const tCHAR* p_profile, cSerializable* p_settings, tCustomizeSettingsDirection p_direction, tBOOL *p_persistent)
{
	if (p_direction == csdFROM_GUI_TO_DATA)
	{
		cRegistry* regStorage = NULL;
		if (PR_SUCC(m_tm->GetProfilePersistentStorage(AVP_PROFILE_UPDATER, *this, &regStorage, cTRUE)))
		{ 
			regStorage->DeleteValue(cRegRoot, REG_LAST_SUCC_URL) ;
		}
	}
	
	return errDO_IT_YOURSELF ;
}

tERROR pr_call BlImpl::hips_customize_settings(const tCHAR* p_profile, CHipsSettings* p_settings, tCustomizeSettingsDirection p_direction, tBOOL *p_persistent)
{
	tERROR err;
	
	cBLTrustedApps pProcMonSett;
	if( PR_FAIL(err = m_tm->GetProfileInfo(AVP_SERVICE_PROCESS_MONITOR, &pProcMonSett)) )
		return err;

	cVector<cBLTrustedApp>& pTrustApps = pProcMonSett.m_aItems;
	
	pTrustApps.clear();

	cResource * pResOAS = p_settings->m_Resource.FindResByContent(&cExclusion(cExclusion::ehexOAS));
	cResource * pResPDM = p_settings->m_Resource.FindResByContent(&cExclusion(cExclusion::ehexPDM));
	cResource * pResNET = p_settings->m_Resource.FindResByContent(&cExclusion(cExclusion::ehexNetwork));

	if( pResOAS || pResPDM || pResNET )
	{
		CHipsRuleItem Rule;
		
		for(DWORD i = 0; i < p_settings->m_vAppList.size(); i++)
		{
			CHipsAppItem& pHipsApp = p_settings->m_vAppList[i];
			tDWORD nProcMonTriggers = 0;
			
			if( pResOAS && PR_SUCC(p_settings->GetRule(pHipsApp.m_AppId, pResOAS->m_nID, 0, Rule)) )
				if( (Rule.m_AccessFlag & HIPS_FLAG_DENY) != HIPS_FLAG_DENY )
					nProcMonTriggers |= cBLTrustedApp::fExclOpenFiles;
			
			if( pResPDM && PR_SUCC(p_settings->GetRule(pHipsApp.m_AppId, pResPDM->m_nID, 0, Rule)) )
				if( (Rule.m_AccessFlag & HIPS_FLAG_DENY) != HIPS_FLAG_DENY )
					nProcMonTriggers |= cBLTrustedApp::fExclBehavior|cBLTrustedApp::fExclRegistry;
			
			if( pResNET && PR_SUCC(p_settings->GetRule(pHipsApp.m_AppId, pResNET->m_nID, 0, Rule)) )
				if( (Rule.m_AccessFlag & HIPS_FLAG_DENY) != HIPS_FLAG_DENY )
					nProcMonTriggers |= cBLTrustedApp::fExclNet;

			if( !nProcMonTriggers )
				continue;
		
			cBLTrustedApp& pTrustApp = pTrustApps.push_back();
			pTrustApp.m_sImagePath = pHipsApp.m_AppName;
			pTrustApp.m_nTriggers = nProcMonTriggers;
		}
	}

	if( PR_FAIL(err = m_tm->SetProfileInfo(AVP_SERVICE_PROCESS_MONITOR, &pProcMonSett, NULL, 0)) )
		return err;
	
	cTrafficMonitorSettings pTrafMonSett;
	if( PR_FAIL(err = m_tm->GetProfileInfo(AVP_SERVICE_TRAFFICMONITOR, &pTrafMonSett)) )
		return err;
	
	// Перетряхиваем настройки AVP_SERVICE_TRAFFICMONITOR, чтобы он перечитал доверенные приложения у AVP_SERVICE_PROCESS_MONITOR
	if( PR_FAIL(err = m_tm->SetProfileInfo(AVP_SERVICE_TRAFFICMONITOR, &pTrafMonSett, NULL, 0)) )
		return err;

	return errDO_IT_YOURSELF;
}

// ---
// tERROR pr_call BlImpl::pdm_customize_settings(cPDMSettings *pSett, tCustomizeSettingsDirection eDirection)
// {
// 	if( eDirection == csdFROM_GUI_TO_DATA )
// 		return pdm_calc_hashes(pSett);
// 	
// 	if( eDirection == csdDATA_INIT )
// 	{
// 		cStrObj strRegIniFilePath; ExpandEnvironmentString(cENVIRONMENT_BASES, strRegIniFilePath);
// 		return updateRegGuardSettings(strRegIniFilePath, pSett);
// 	}
// 
// 	return errDO_IT_YOURSELF;
// }

// ---
// tERROR pr_call BlImpl::pdm_calc_hashes(cPDMSettings *pSett)
// {
// 	cProcessMonitor * pPM = NULL;
// 	tERROR err = m_tm->GetService(0, *this, AVP_SERVICE_PROCESS_MONITOR, (cObject **)&pPM);
// 	if( PR_FAIL(err) )
// 	{
// 		PR_TRACE(( this, prtERROR, "BL\tpdm_calc_hashes(): can't get process monitor service. %terr", err ));
// 		return err;
// 	}
// 
// 	for(tDWORD i = 0; i < pSett->m_RegGroup_List.size(); i++)
// 	{
// 		cVector<cPdmRegGroupApp_Item> &Rules = pSett->m_RegGroup_List[i].m_vRules;
// 		for(tDWORD j = 0; j < Rules.size(); j++)
// 		{
// 			cPdmAppMonitoring_Data &AppInfo = Rules[j].m_Data;
// 			if( !AppInfo.m_Hash )
// 				pPM->CalcObjectHash(cAutoString(AppInfo.m_ImagePath), &AppInfo.m_Hash);
// 		}
// 	}
// 	
// 	for(tDWORD i = 0; i < pSett->m_AppsMonitoring_List.size(); i++)
// 	{
// 		cPdmAppMonitoring_Data &AppInfo = pSett->m_AppsMonitoring_List[i].m_Data;
// 		if( !AppInfo.m_Hash )
// 			pPM->CalcObjectHash(cAutoString(AppInfo.m_ImagePath), &AppInfo.m_Hash);
// 
// 		cVector<cPdmAppMonitoring_Data> &TrustedImageList = pSett->m_AppsMonitoring_List[i].m_TrustedImageList;
// 		for(tDWORD j = 0; j < TrustedImageList.size(); j++)
// 		{
// 			cPdmAppMonitoring_Data &AppInfo = TrustedImageList[j];
// 			if( !AppInfo.m_Hash )
// 				pPM->CalcObjectHash(cAutoString(AppInfo.m_ImagePath), &AppInfo.m_Hash);
// 		}
// 	}
// 	
// 	for(tDWORD i = 0; i < pSett->m_TrustedImageList.size(); i++)
// 	{
// 		cPdmAppMonitoring_Data &AppInfo = pSett->m_TrustedImageList[i];
// 		if( !AppInfo.m_Hash )
// 			pPM->CalcObjectHash(cAutoString(AppInfo.m_ImagePath), &AppInfo.m_Hash);
// 	}
// 
// 	m_tm->ReleaseService(0, (cObject *)pPM);
// 
// 	return errOK;
// }

// ---
// tERROR pr_call BlImpl::pdm_update_hash(cPdmAppMonitoring_Data &moduleInfo, cProcessMonitor *pPM)
// {
// 	cModuleInfoHash mih;
// 	mih.m_sImagePath = moduleInfo.m_ImagePath;
// 	tERROR err = pPM->GetProcessInfo(&mih);
// 	if( PR_SUCC(err) )
// 	{
// 		moduleInfo.m_Version     = mih.m_sVersion;
// 		moduleInfo.m_Vendor      = mih.m_sVendor;
// 		moduleInfo.m_Description = mih.m_sDescription;
// 		moduleInfo.m_tCreation   = mih.m_tCreation;
// 		moduleInfo.m_tModify     = mih.m_tModify;
// 		moduleInfo.m_ObjectSize  = mih.m_nObjectSize;
// 		moduleInfo.m_Hash        = mih.m_nHash;
// 	}
// 	else
// 		PR_TRACE((this, prtERROR, "BL\tpdm_update_hash(). GetProcessInfo fails (%terr), module %S", err, moduleInfo.m_ImagePath.data()));
// 
// 	return err;
// }

// ---
// tERROR pr_call BlImpl::pdm_update_hashes(cUpdaterUpdatedFileList *pList)
// {
// 	bool bExecutableUpdated = false;
// 	for(size_t i = 0; i < pList->m_files.size(); i++)
// 	{
// 		cUpdaterUpdatedFile &file = pList->m_files[i];
// 		if( file.m_type == cUpdaterUpdatedFile::patch &&
// 			file.m_status == cUpdaterUpdatedFile::modified )
// 		{
// 			bExecutableUpdated = true;
// 			break;
// 		}
// 	}
// 	if( !bExecutableUpdated )
// 		return errOK;
// 
// 	PR_TRACE((this, prtNOTIFY, "BL\tUpdate AIC info about own changed modules"));
// 
// 	tERROR err;
// 	cPDMSettings pdm_settings;
// 	err = m_tm->GetProfileInfo(AVP_PROFILE_PDM, &pdm_settings);
// 	if( PR_FAIL(err) )
// 	{
// 		PR_TRACE((this, prtERROR, "BL\tpdm_update_hashes(): can't get PDM settings. %terr", err));
// 		return err;
// 	}
// 
// 	cProcessMonitor * pPM = NULL;
// 	err = m_tm->GetService(0, *this, AVP_SERVICE_PROCESS_MONITOR, (cObject **)&pPM);
// 	if( PR_FAIL(err) )
// 	{
// 		PR_TRACE((this, prtERROR, "BL\tpdm_update_hashes(): can't get process monitor service. %terr", err));
// 		return err;
// 	}
// 
// 	bool bSettingsUpdated = false;
// 	cVector<cPdmAppMonitoring_Data> &til = pdm_settings.m_TrustedImageList;
// 	for(size_t i = 0; i < pList->m_files.size(); i++)
// 	{
// 		cUpdaterUpdatedFile &file = pList->m_files[i];
// 		if( file.m_type == cUpdaterUpdatedFile::patch &&
// 			file.m_status == cUpdaterUpdatedFile::modified )
// 		{
// 			cStrObj strUpdatedFile; ExpandEnvironmentString("%ProductRoot%", strUpdatedFile);
// 			strUpdatedFile.check_last_slash(true);
// 			strUpdatedFile += file.m_filename;
// 			for(size_t j = 0; j < til.size(); j++)
// 			{
// 				cPdmAppMonitoring_Data &moduleInfo = til[j];
// 				if( moduleInfo.m_ImagePath.compare(strUpdatedFile, fSTRING_COMPARE_CASE_INSENSITIVE | fSTRING_COMPARE_SLASH_INSENSITIVE) == cSTRING_COMP_EQ )
// 				{
// 					if( PR_SUCC(pdm_update_hash(moduleInfo, pPM)) )
// 						bSettingsUpdated = true;
// 					break;
// 				}
// 			}
// 		}
// 	}
// 	
// 	m_tm->ReleaseService(0, (cObject *)pPM);
// 	
// 	if( bSettingsUpdated )
// 	{
// 		PR_TRACE((this, prtNOTIFY, "BL\tSome modules has been updated, module info updated"));
// 		return m_tm->SetProfileInfo(AVP_PROFILE_PDM, &pdm_settings, (hOBJECT)this, 0);
// 	}
// 	
// 	PR_TRACE((this, prtNOTIFY, "BL\tNo info about own modules updated"));
// 	return errOK;
// }

// ---
// tERROR pr_call BlImpl::regguard2_customize_settings(CRegSettings*pSett, tCustomizeSettingsDirection eDirection)
// {
// 	if( eDirection == csdDATA_INIT )
// 	{
// 		cStrObj strRegIniFilePath; ExpandEnvironmentString(cENVIRONMENT_BASES, strRegIniFilePath);
// 		return updateRegGuardSettings(strRegIniFilePath, pSett);
// 	}
// 
// 	return errDO_IT_YOURSELF;
// }

// ---
// tERROR pr_call BlImpl::fw_customize_settings(cAHFWSettings *pSett, tCustomizeSettingsDirection eDirection)
// {
// 	switch(eDirection)
// 	{
// 	case csdFROM_GUI_TO_DATA: return fw_calc_hashes(pSett);
// 	}
// 	return errDO_IT_YOURSELF;
// }

// ---
// tERROR pr_call BlImpl::fw_calc_hashes(cAHFWSettings *pSett)
// {
// 	if( !FillApplicationChecksum && PR_FAIL(g_root->ResolveImportFuncByName((tFUNC_PTR*)&FillApplicationChecksum, PID_AHFW, "FillApplicationChecksum", PID_BL)) )
// 		return errNOT_INITIALIZED;
// 	
// 	cFwAppRules &AppRules = pSett->m_AppRules;
// 	for(tUINT i = 0; i < AppRules.size(); i++)
// 	{
// 		cFwAppRule &Rule = AppRules[i];
// 		if( !Rule.IsHashEmpty() )
// 			continue;
// 		
// 		tDWORD nHashSize = FwHashSize;
// 		FillApplicationChecksum(&Rule.m_sAppName, Rule.m_nHash, &nHashSize);
// 	}
// 	return errOK;
// }

// ---
// tERROR pr_call BlImpl::fw_update_networks(cFwNets *pNetworks)
// {
// 	if( !ResolveNetworks && PR_FAIL(g_root->ResolveImportFuncByName((tFUNC_PTR*)&ResolveNetworks, PID_AHFW, "ResolveNetworks", PID_BL)) )
// 		return errNOT_INITIALIZED;
// 
//     ResolveNetworks(pNetworks);
// 	return errOK;
// }

// ---
// tERROR pr_call BlImpl::fw_add_network(cFwChangeNet *pChangeNet)
// {
// 	if( !AddNetwork && PR_FAIL(g_root->ResolveImportFuncByName((tFUNC_PTR*)&AddNetwork, PID_AHFW, "AddNetwork", PID_BL)) )
// 		return errNOT_INITIALIZED;
// 
//     AddNetwork(&pChangeNet->m_aNetworks, &pChangeNet->m_aNewNet, pChangeNet->m_aIndex);
//     return errOK;
// }

// ---
// tERROR pr_call BlImpl::fw_delete_network(cFwChangeNet *pChangeNet)
// {
// 	if( !DeleteNetwork && PR_FAIL(g_root->ResolveImportFuncByName((tFUNC_PTR*)&DeleteNetwork, PID_AHFW, "DeleteNetwork", PID_BL)) )
// 		return errNOT_INITIALIZED;
// 
//     DeleteNetwork(&pChangeNet->m_aNetworks, pChangeNet->m_aIndex);
//     return errOK;
// }

// ---
// tERROR pr_call BlImpl::fw_change_network(cFwChangeNet *pChangeNet)
// {
// 	if( !ChangeNetwork && PR_FAIL(g_root->ResolveImportFuncByName((tFUNC_PTR*)&ChangeNetwork, PID_AHFW, "ChangeNetwork", PID_BL)) )
// 		return errNOT_INITIALIZED;
// 
//     ChangeNetwork(&pChangeNet->m_aNetworks, pChangeNet->m_aIndex, &pChangeNet->m_aNewNet);
//     return errOK;
// }

// ---
tERROR pr_call BlImpl::bl_customize_settings(cBLSettings *pSett, tCustomizeSettingsDirection eDirection)
{
	if( eDirection == csdFROM_DATA_TO_TASK )
	{
		pSett->m_nCpuNumber = m_nCpuNumber;
	}
	else if( eDirection == csdDATA_INIT )
	{
		pSett->m_nOasInstances = pSett->m_nCpuNumber = m_nCpuNumber;

		cStrObj strProductType; ExpandEnvironmentString("%ProductType%", strProductType);
		if( strProductType != AVP_PRODUCT_FS )
			pSett->m_nOasInstances = 1;
	}
	
	return errOK;
}

tERROR pr_call BlImpl::update_admin_policy_state(cCfgEx *pPolicy)
{
	cCfgEx cfgPolicy(cCfgEx::fPolicy|cCfgEx::fSettings);
	if( !pPolicy )
	{
		pPolicy = &cfgPolicy;
		
		tERROR err = m_tm->GetProfileInfo(NULL, pPolicy);
		if( PR_FAIL(err) )
			return err;		
	}

	bool bPolicyExists = !!pPolicy->count();
	bool bOldAdminPolicyState = !!(m_State.m_ProductState & eProductAdminPolicy);

	if( bOldAdminPolicyState != bPolicyExists )
	{
		if( bPolicyExists )
			m_State.m_ProductState |= eProductAdminPolicy;
		else
			m_State.m_ProductState &= ~eProductAdminPolicy;
		
		sysSendMsg(pmc_PRODUCT, pm_PRODUCT_STATE_CHANGED, NULL, NULL, NULL);
	}
	return errOK;
}

// ---
tERROR pr_call BlImpl::check_schedule( const tCHAR* p_profile, cTaskSchedule& schedule ) {

	if( schedule.mode() == schmAuto )
	{
		schedule.m_eType = schtMinutely;
		schedule.m_nMinutes = m_dwUpdateAutoPeriodFail;
		schedule.m_bUseTime = cFALSE;
		schedule.m_bRaiseIfSkipped = cTRUE;
	}

	return errDO_IT_YOURSELF;
}

// ---
tERROR pr_call BlImpl::check_schedule_event( const tCHAR* p_profile, cTaskSchedule& schedule )
{
	if( !m_bStartedUp )
		return errOPERATION_CANCELED;

	if( ::_cmp(p_profile, AVP_PROFILE_UPDATER) && schedule.auto_mode() ) // this is updater, check for auto mode
	{
        tDWORD dwLastNetAddrListChange   = 0 ;     
        tBOOL blUseLastNetAddrListChange = cFALSE ;
        if (m_NetDetect && m_bAutoUpdateFailed)
        {
            m_NetDetect->GetLastIpAddrChange(&dwLastNetAddrListChange) ;
            blUseLastNetAddrListChange = cTRUE ;
            PR_TRACE((this, prtNOTIFY, "BL\tm_dwLastNetAddrListChange=%d  dwLastNetAddrListChange=%d", m_dwLastNetAddrListChange, dwLastNetAddrListChange)) ;
        }

		time_t now = cDateTime::now_utc();
		if ( m_tmNextAutoUpdate != -1 && 
            now < m_tmNextAutoUpdate &&
            (!blUseLastNetAddrListChange || dwLastNetAddrListChange <= m_dwLastNetAddrListChange))
			return errOPERATION_CANCELED;

		if( m_bAutoUpdateFailed )
			return warnDONT_CHANGE_TASKID;
	}
	
	tDWORD min_allowed_persent = m_settings.m_BatterySafe.val();
	if ( min_allowed_persent )
	{
		tDWORD nACLineStatus = AC_LINE_UNKNOWN;
		tDWORD batt_pers = BATTERY_PERCENTAGE_UNKNOWN;
		
		if ( PrGetSystemPowerStatus )
			PrGetSystemPowerStatus( &nACLineStatus, NULL, &batt_pers, NULL, NULL );
		
		if ( ((nACLineStatus == AC_LINE_UNKNOWN) || (nACLineStatus != AC_LINE_ONLINE)) &&
			(batt_pers != BATTERY_PERCENTAGE_UNKNOWN) ) {
			// check Battery Life Percent
			if ( batt_pers < min_allowed_persent )
				return errOPERATION_CANCELED; // don't start task
		}
	}
	
	return errOK;
}

tERROR pr_call BlImpl::check_task_run(cProfileBase* p_profile)
{
	static struct cACL { tSTRING sTask; tSTRING sProducts[6]; } g_ACL[] = {
		{AVP_PROFILE_AVMONITOR        , {"kis", "wks", "kav", "aol", "fs", NULL} },
		{AVP_PROFILE_FILEMONITOR      , {"kis", "wks", "kav", "aol", "fs", NULL} },
		{AVP_PROFILE_MAILMONITOR      , {"kis", "wks", "kav", "aol", NULL} },
		{AVP_PROFILE_WEBMONITOR       , {"kis", "wks", "kav", NULL} },
		{AVP_PROFILE_WEBMON_URLFLT    , {"kis", "wks", "kav", NULL} },
		{AVP_PROFILE_SCRIPTCHECKER    , {"kis", "wks", "kav", NULL} },
		{AVP_PROFILE_HTTPSCAN         , {"kis", "wks", "kav", NULL} },
		{AVP_PROFILE_BEHAVIORMONITOR  , {"kis", "wks", "kav", NULL} },
		{AVP_PROFILE_PDM              , {"kis", "wks", "kav", NULL} },
		{AVP_PROFILE_OFFICEGUARD      , {"kis", "wks", "kav", NULL} },
		{AVP_PROFILE_BEHAVIORMONITOR2 , {"kis", "wks", "kav", NULL} },
		{AVP_PROFILE_PDM2             , {"kis", "wks", "kav", NULL} },
		{AVP_PROFILE_REGGUARD2        , {"kis", "wks", "kav", NULL} },
		{AVP_PROFILE_HIPS             , {"kis", "wks", "kav", NULL} },
		{AVP_PROFILE_CONTENT_FILTER   , {"kis", "wks", "kav", NULL} },
		{AVP_PROFILE_ONLINE_SECURITY  , {"kis", "wks", "kav", NULL} },
        {AVP_PROFILE_STATISTICS       , {"kis", "wks", "kav", NULL} },
		{AVP_PROFILE_PRIVACYCONTROL   , {"kis", "wks", NULL, NULL} },
		{AVP_PROFILE_ANTIHACKER       , {"kis", "wks", NULL} },
		{AVP_PROFILE_FIREWALL         , {"kis", "wks", NULL} },
		{AVP_PROFILE_INTRUSIONDETECTOR, {"kis", "wks", NULL} },
		{AVP_PROFILE_ANTISPAM         , {"kis", "wks", NULL} },
		{AVP_PROFILE_ANTISPY          , {"kis", "wks", NULL} },
		{AVP_PROFILE_POPUPBLOCKER     , {"kis", "wks", NULL} },
		{AVP_PROFILE_ANTIDIAL         , {"kis", "wks", NULL} },
		{AVP_PROFILE_ANTIBANNER       , {"kis", "wks", NULL} },
		{AVP_PROFILE_ANTIPHISHING     , {"kis", "wks", NULL} },
		{AVP_PROFILE_PARENTALCONTROL  , {"kis", "wks", NULL} },
		{AVP_PROFILE_SYSTEM_WATCHER   , {"kis", "wks", "kav", "fs", NULL} },
		{"SystemWatch"                , {"kis", "wks", "kav", "fs", NULL} },
		{AVP_PROFILE_NETWORK_WATCHER  , {"kis", "wks", NULL} },
		{NULL}
	};

	if( p_profile->m_sName != AVP_PROFILE_PRODUCT && !p_profile->isService() && p_profile->persistent() )
	{
		bool bFound = false;
		for(cACL* acl = g_ACL; acl->sTask; acl++)
			if( p_profile->m_sName == acl->sTask )
			{
				cStrObj strProductType;
				if( m_LicInfo.m_KeyInfo.m_dwProductID == 754 )
					strProductType = "aol";
				else
					ExpandEnvironmentString("%ProductType%", strProductType);

				for(tSTRING* prd = acl->sProducts; *prd && !bFound; prd++)
					if( strProductType == *prd )
						bFound = true;
				break;
			}

		if( !bFound && m_tmLastFullScanState != (tProfileState)666 )
			return errNO_LICENCE;
	}

	if( p_profile->m_sType == AVP_TASK_LICENCE )
		return errOK;

	bool bForUpdate = p_profile->m_sType == AVP_TASK_UPDATER;

	if( !m_bStartedUp && !bForUpdate )
		return errOK;

	cAutoCS cs(m_lock, true);

	switch(m_LicInfo.m_dwFuncLevel)
	{
	case eflOnlyUpdates:
		if( bForUpdate || p_profile->m_sType == AVP_TASK_ROLLBACK )
			return errOK;
		break;

	case eflFunctionWithoutUpdates:
		if( !bForUpdate )
			return errOK;
		break;

	case eflFullFunctionality:
		return errOK;
	}

	return errNO_LICENCE;
}

tERROR pr_call BlImpl::check_blacklist(cDateTime* dtUpdateDate, bool bUpdateState)
{
	cStrObj strPath(m_settings.m_sBasePath);
	strPath.add_path_sect("black.lst");

	cIOObj hIo(*this, cAutoString(strPath), dtUpdateDate ? fACCESS_RW : fACCESS_READ);
	tERROR error = hIo.last_error();

	if( dtUpdateDate ) // for update
	{
		if( error == errLOCKED )
			return errLOCKED;

		if( m_ValidBlackList.m_tmDate && m_ValidBlackList.m_tmDate > (tDWORD)(time_t)*dtUpdateDate )
			return errNOT_OK;

		m_tmNewBlackListDate = (tDWORD)*dtUpdateDate;
		return errOK;
	}

	cAutoObj<cHash> hHash;
	tBYTE  buff[0x8000], data[32];
	tDWORD readed;
	tQWORD tmValidBlackListHash = 0;

	if( PR_SUCC(error) )
		error = sysCreateObjectQuick((hOBJECT*)&hHash, IID_HASH, PID_HASH_MD5);

	for(tQWORD offset = 0; PR_SUCC(error); offset += readed)
	{
		error = hIo->SeekRead(&readed, offset, buff, sizeof(buff));
		if( !readed )
			break;
		
		if( PR_SUCC(error) )
			error = hHash->Update(buff, readed);
	}

	if( PR_SUCC(error) )
		error = hHash->GetHash(data, sizeof(data));

	if( PR_SUCC(error) )
		tmValidBlackListHash = *(tQWORD*)(&data[0]) ^ *(tQWORD*)(&data[8]);

	if( PR_SUCC(error) && tmValidBlackListHash != m_ValidBlackList.m_qwHash )
	{
		if( bUpdateState || !m_ValidBlackList.m_qwHash )
		{
			m_ValidBlackList.m_qwHash = tmValidBlackListHash;
			m_ValidBlackList.m_tmDate = m_tmNewBlackListDate;
			m_hCryptoHelper->StoreData(esiValidBlackList, &m_ValidBlackList, sizeof(m_ValidBlackList));
		}
		else if( !m_tmNewBlackListDate )
			return errBLACKLIST_CORRUPTED;
		}

	if( bUpdateState )
		m_tmNewBlackListDate = 0;
	return errOK;
}

tERROR pr_call BlImpl::check_bases_toload( cAVP3BasesInfo* info, bool bByUpdate )
{
	if( m_bIsCDInstallation )
		return errOK;

	    cAutoCS cs(m_lock, true);

	if( (m_State.m_KeyState & (eNoKeys|eKeyBlocked)) || (m_LicInfo.m_dwInvalidReason == ekirInvalidBlacklist) )
	    {
		    if( !bByUpdate && m_tmValidBasesDate && m_tmValidBasesDate != (time_t)cDateTime(&info->m_dtBasesDate) )
			    return errOBJECT_DATA_CORRUPTED;
	    }
	    else
	    {
		    if( (time_t)cDateTime(&info->m_dtBasesDate) > (time_t)cDateTime(&m_LicInfo.m_dtAppLicenseExpDate) + 24*60*60 )
			    return errNO_LICENCE;
	    }
	return errOK;
}

tERROR pr_call BlImpl::check_my_computer_was_scanned(cProfileBase* tp)
{
	cODSSettings sett;
	tERROR err = m_tm->GetTaskInfo(tp->m_nRuntimeId, &sett);
	if( PR_FAIL(err) )
		return err;

	enum SystemObjectType
	{
		sotNone               = 0x0000,
		sotMemory             = 0x0001,
		sotStartup            = 0x0002,
		sotSystemRestore      = 0x0004,
		sotMail               = 0x0008,
		sotAllFixedDrives     = 0x0010,
		sotAllRemovableDrives = 0x0020,
		sotComputer           = sotMemory | sotStartup | sotSystemRestore | sotMail | sotAllFixedDrives | sotAllRemovableDrives,
	};
	
	SystemObjectType nScannedObjects = sotNone;
	for(size_t i = 0; i < sett.m_aScanObjects.size(); i++)
	{
		cScanObject &obj = sett.m_aScanObjects[i];
		if( !obj.m_bEnabled )
			continue;

		switch(obj.m_nObjType)
		{
		case OBJECT_TYPE_MY_COMPUTER:          return errOK;
		case OBJECT_TYPE_MEMORY:               (int &)nScannedObjects |= sotMemory;             break;
		case OBJECT_TYPE_STARTUP:              (int &)nScannedObjects |= sotStartup;            break;
		case OBJECT_TYPE_MAIL:                 (int &)nScannedObjects |= sotMail;               break;
		case OBJECT_TYPE_ALL_FIXED_DRIVES:     (int &)nScannedObjects |= sotAllFixedDrives;     break;
		case OBJECT_TYPE_ALL_REMOVABLE_DRIVES: (int &)nScannedObjects |= sotAllRemovableDrives; break;
		case OBJECT_TYPE_SYSTEM_RESTORE:       (int &)nScannedObjects |= sotSystemRestore;      break;
		}
		if( nScannedObjects == sotComputer )
			return errOK;
	}

	return errNOT_OK;
}

tERROR pr_call BlImpl::check_nagent_installed()
{
#if defined (_WIN32)
	HKEY hKey = NULL;
	LONG err = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\KasperskyLab\\Components\\34\\1103\\1.0.0.0", 0, KEY_READ, &hKey);
	if( err == ERROR_SUCCESS )
		RegCloseKey(hKey);
	if( err == ERROR_FILE_NOT_FOUND )
	{
		PR_TRACE((this, prtIMPORTANT, "BL\tBlImpl::check_nagent_installed. nagent is not installed."));
		return errNOT_OK;
	}
	return errOK;
#elif defined (__unix__)
	#warning "Consideration is needed!"
#else
	#error "Platform is not supported"
#endif
	return errNOT_OK;
}

tERROR pr_call BlImpl::remove_policy()
{
	cCfgEx oCfgEx(cCfgEx::fPolicy);
	tERROR err = m_tm->GetProfileInfo(NULL, &oCfgEx);
	if( PR_FAIL(err) )
		return err;
	
	for(tUINT i = 0; i < oCfgEx.count(); i++)
	{
		cCfgEx cfg(cCfgEx::fPolicy);
		cfg.m_sType = oCfgEx[i].m_sType;
		m_tm->SetProfileInfo(NULL, &cfg, *this, 0);
	}
	
	return errOK;
}

tERROR pr_call BlImpl::update_avs_bases_info( cAVP3BasesInfo* info, bool bByUpdate )
{
	if( m_bIsCDInstallation )
		return errOK;

	if( !m_bIsTSPInited )
	{
		cAVSSettings sett;
		m_tm->GetProfileInfo(AVP_SERVICE_AVS, &sett);
		m_bIsTSPInited = sett.m_bMultiThreaded;
	}

	if( m_hCryptoHelper && bByUpdate && !m_bIsUpdatePerformed )
	{
		m_bIsUpdatePerformed = cTRUE;
		m_hCryptoHelper->StoreData(esiIsUpdatePerformed, &m_bIsUpdatePerformed, sizeof(tBOOL));
	}

	    time_t tmBases = cDateTime(&info->m_dtBasesDate);
	    if( tmBases == m_tmValidBasesDate )
		    return errOK;

	    m_tmValidBasesDate = tmBases;
	    if( m_hCryptoHelper )
		    m_hCryptoHelper->StoreData(esiValidBasesDate, &m_tmValidBasesDate, sizeof(time_t));
	return errOK;
}

tERROR pr_call BlImpl::process_init_task(cProfileBase* p_task_info)
{
	return errOK;
}

tERROR pr_call BlImpl::process_active_disifect(cAskObjectAction* pInfo)
{
	cProfileEx profile;
	profile.m_sName = AVP_PROFILE_ACTIVE_DISINFECT;
	if( PR_FAIL(m_tm->CloneProfile(AVP_PROFILE_SCAN_STARTUP, &profile, fPROFILE_TEMPORARY, *this)) )
		return errNOT_OK;

	cScanObjects* pSett = (cScanObjects*)profile.settings();
	if( !pSett || !pSett->isBasedOn(cScanObjects::eIID) )
		return errNOT_OK;

	hOBJECT hAdvDis = NULL;
	m_tm->GetService (
		TASKID_TM_ITSELF,
		*m_tm,
		AVP_SERVICE_ADVANCED_DISINFECTION,
		(hOBJECT*) &hAdvDis,cREQUEST_SYNCHRONOUS
		);



	pSett->m_aScanObjects.clear();

	cScanObject pObject;
	pObject.m_nObjType = OBJECT_TYPE_FILE;
	pObject.m_strObjName = pInfo->m_strObjectName;
	pSett->m_aScanObjects.push_back(pObject);

	pObject.m_nObjType = OBJECT_TYPE_MEMORY;
	pSett->m_aScanObjects.push_back(pObject);

	pObject.m_nObjType = OBJECT_TYPE_STARTUP;
	pSett->m_aScanObjects.push_back(pObject);

	pSett->m_bActiveDisinfection = cTRUE;
	if( !m_settings.m_bEnableInteraction || pInfo->m_nDescription == SCAN_ACTION_DISINFECT )
		pSett->m_nScanAction = SCAN_ACTION_DISINFECT;
	else
		pSett->m_nScanAction = SCAN_ACTION_ASKUSER;

	if( PR_FAIL(m_tm->SetProfileInfo(AVP_PROFILE_ACTIVE_DISINFECT, pSett, *this, 0)) )
		return errNOT_OK;

	tTaskId taskId;
	if( PR_FAIL(m_tm->OpenTask(&taskId, AVP_PROFILE_ACTIVE_DISINFECT, 0, *this)) )
		return errNOT_OK;

	cProfileBase pPdm;
	m_tm->GetProfileInfo(AVP_PROFILE_PDM, &pPdm);
//	if( pPdm.m_nState & STATE_FLAG_OPERATIONAL )
	{
		cODSRuntimeParams odsParams;
		odsParams.m_bEnableStopMode = cTRUE;
		odsParams.m_eStopMode = cODSRuntimeParams::stop_Restart;
		m_tm->AskTaskAction(taskId, cODSRuntimeParams::SET, &odsParams);
	}

	if( pInfo && pInfo->m_nTaskId && pInfo->m_strTaskType == AVP_TASK_ONDEMANDSCANNER )
		m_tm->SetTaskState(pInfo->m_nTaskId, TASK_REQUEST_STOP, NULL, 0);

	m_tm->SetProfileState(AVP_PROFILE_ACTIVE_DISINFECT, TASK_REQUEST_RUN, NULL, 0);

	if( m_settings.m_bEnableInteraction )
	{
		cProfileAction action(AVP_PROFILE_ACTIVE_DISINFECT, taskId);
		action.m_nSessionId = pInfo->m_nSessionId;

		m_tm->AskAction(TASKID_TM_ITSELF, cProfileAction::SHOW_REPORT, &action);
	}

	return errOK;
}

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR BlImpl::ObjectInit()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ProductLogic::ObjectInit method" );
	
	// Place your code here
	cBlLoc::m_bl = this;
	cBlLoc::init(true);

#if defined(_WIN32)
	m_hFsSync = NULL;
	if( PrLoadLibrary && PrGetProcAddress && PrFreeLibrary )
		if( m_hFsSync = PrLoadLibrary(L"fssync.dll", cCP_UNICODE) )
			if( tFSDrv_Init pFSDrv_Init = (tFSDrv_Init)PrGetProcAddress(m_hFsSync, "FSSync_Init") )
				pFSDrv_Init();
#endif // _WIN32

	m_nDetectMask = 0;

	m_pReport = NULL;
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR BlImpl::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ProductLogic::ObjectInitDone method" );

	m_dwEnabledPolicyBit = cPolicySettings::FindFieldBit(cCfg::eIID, "enabled", NULL);
	
	tMsgHandlerDescr hdls[] =
	{
#ifdef _DEBUG
		{ *this, rmhLISTENER, 0x7fd52e63,                         IID_ANY,     PID_ANY,  IID_ANY, PID_ANY },
#endif // _DEBUG
		{ *this, rmhLISTENER, pmcPROFILE,                         IID_ANY,     PID_ANY,  IID_ANY, PID_ANY },
		{ *this, rmhLISTENER, pmc_TASK_OS_REBOOT_REQUEST,         IID_ANY,     PID_ANY,  IID_ANY, PID_ANY },
		{ *this, rmhLISTENER, pmc_ANTISPAM_HAS_BEEN_TRAINED,      IID_ANY,     PID_ANY,  IID_ANY, PID_ANY },
		{ *this, rmhLISTENER, pmc_UPDATE_THREATS_LIST,            IID_ANY,     PID_ANY,  IID_ANY, PID_ANY },
// 		{ *this, rmhLISTENER, pmc_TASKAHFW_WARNING_EVENT,         IID_ANY,     PID_AHFW, IID_ANY, PID_ANY },
// 		{ *this, rmhLISTENER, pmc_TASKAHFWAPPCHANGED_REPORT_EVENT,IID_ANY,     PID_AHFW, IID_ANY, PID_ANY },
		{ *this, rmhLISTENER, pmc_SCHEDULER,                      IID_ANY,     PID_ANY,  IID_ANY, PID_ANY },
		{ *this, rmhLISTENER, pmc_FSSYNC,		                  IID_ANY,     PID_ANY,  IID_ANY, PID_ANY },
		
		{ *this, rmhDECIDER,  pmc_LOCALIZE_INFO,                  IID_ANY,     PID_ANY,  IID_ANY, PID_ANY },
		{ *this, rmhDECIDER,  pmcTM_EVENTS,                       IID_ANY,     PID_ANY,  IID_ANY, PID_ANY },
		{ *this, rmhDECIDER,  pmc_LICENSING,                      IID_ANY,     PID_ANY,  IID_ANY, PID_ANY },
		{ *this, rmhDECIDER,  pmc_AVS,                            IID_ANY,     PID_ANY,  IID_ANY, PID_ANY },
		{ *this, rmhDECIDER,  pmc_TRANSPORT,					  IID_ANY,     PID_ANY,  IID_ANY, PID_ANY },
		{ *this, rmhDECIDER,  pmc_UPDATER,						  IID_ANY,     PID_ANY,  IID_ANY, PID_ANY },
		{ *this, rmhDECIDER,  pmc_UPDATER_CONFIGURATOR,           IID_ANY,     PID_ANY,  IID_ANY, PID_ANY },
		{ *this, rmhDECIDER,  pmc_PRODUCT_OPERATION,              IID_ANY,     PID_ANY,  IID_ANY, PID_ANY },
		{ *this, rmhDECIDER,  pmc_USERNOTIFY,                     IID_ANY,     PID_ANY,  IID_ANY, PID_ANY },
		{ *this, rmhDECIDER,  pmc_USERBAN_SYNC,                   IID_ANY,     PID_ANY,  IID_ANY, PID_ANY },
		{ *this, rmhDECIDER,  pmc_SETTINGS,                       IID_ANY,     PID_ANY,  IID_ANY, PID_ANY },
	};
	error = m_tm->sysRegisterMsgHandlerList( hdls, countof(hdls) );
	if( PR_SUCC(error) )
		error = sysRegisterMsgHandler(pmc_SYSTEM_TIME_CHANGED, rmhLISTENER, g_root, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	if( PR_SUCC(error) )
		error = sysRegisterMsgHandler(pmc_REMOTE_GLOBAL, rmhLISTENER, g_root, IID_ANY, PID_ANY, IID_ANY, PID_ANY);

	if( PR_SUCC(error) )
		error = sysCreateObjectQuick((hOBJECT *)&m_lock, IID_CRITICAL_SECTION);

	if( m_data ) {
		cSerializable* ptr = this;
		::g_root->RegDeserialize( &ptr, m_data, 0, getIID() );
	}

	time_t tmNow = cDateTime::now_utc();
	tDWORD dwTicks = PrGetTickCount();
	if( (tINT)dwTicks < 0 ) // fix for WinME
		dwTicks = 0;

	bool bWasReboot = (tDWORD)m_tmLastStart + dwTicks / 1000 < (tDWORD)tmNow;
	bool bNeedReboot = (m_State.m_UpdateState & eUpdateNeedReboot) != 0;

	m_bFirstStartInSession = (m_tmLastStart == -1) || bWasReboot;

	m_State.m_ProductState = 0;
	m_State.m_UpdateState = 0;
	m_State.m_ThreatsState = 0;
	m_State.m_KeyState = 0;
	m_State.m_ProtectionState = eProtectionNotRunnig;

	PR_TRACE((this, prtIMPORTANT, "BL\tCurrent time is %s", ctime(&tmNow)));
	PR_TRACE((this, prtIMPORTANT, "BL\tLast start time is %s", ctime((time_t*)&m_tmLastStart)));
	PR_TRACE((this, prtIMPORTANT, "BL\t%d seconds since OS start", dwTicks / 1000));
	if(bNeedReboot)
		PR_TRACE((this, prtIMPORTANT, "BL\tReboot mark is set"));
	if(bWasReboot)
		PR_TRACE((this, prtIMPORTANT, "BL\tFirst start after reboot"));

	if(m_tmLastStart != -1 && bWasReboot && bNeedReboot)
	{
		PR_TRACE((this, prtIMPORTANT, "BL\tResetting reboot mark"));
		bNeedReboot = false;
		// bf 24125
		if (m_data)
			PR_VERIFY_SUCC(::g_root->RegSerializeField( (cBLStatistics*)this, &m_State.m_UpdateState, m_data, NULL, 0 ));
	}

	if(bNeedReboot)
	{
		PR_TRACE((this, prtIMPORTANT, "BL\tProduct needs reboot (LastStart=%x, Now=%x, Ticks=%x)",
			m_tmLastStart, tmNow, dwTicks));
		m_State.m_UpdateState |= eUpdateNeedReboot;
	}

    m_dwLastNetAddrListChange = 0 ;
    m_errLastUpdateError      = errOK ;
    m_NetDetect               = NULL ;
    if (PR_FAIL(sysCreateObjectQuick((hOBJECT*)&m_NetDetect, IID_NETDETECT, PID_NETDETECT)))
		PR_TRACE((this, prtERROR, "BL\tFailed to create net detect object"));

	if (PR_FAIL(sysCreateObjectQuick((hOBJECT*)&m_hCryptoHelper, IID_CRYPTOHELPER, PID_CRYPTOHELPER)))
		PR_TRACE((this, prtERROR, "BL\tFailed to create cryptohelper object"));

	m_tmLastStart = tmNow;
	saveOneField(m_config, "data", &m_tmLastStart, sizeof(m_tmLastStart));

	m_LicInfo.m_dwFuncLevel = eflFullFunctionality;

#if defined (_WIN32)
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	m_nCpuNumber = info.dwNumberOfProcessors;
#else
	#warning "Consideration is needed!"
#endif

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR BlImpl::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ProductLogic::ObjectPreClose method" );

	m_ChartStatistics.Stop();
	
    close_object( m_NetDetect );
	close_object( m_pReport );
	prepare_to_stop();
	
	cThreadPoolBase::de_init();
	
	if( m_data ) {
		tERROR error = ::g_root->RegSerialize( (cBLStatistics*)this, SERID_UNKNOWN, m_data, 0 );
		PR_TRACE((this, prtIMPORTANT, "BL\tSave statistics... error(%x)", error));
	}
	else
		PR_TRACE((this, prtERROR, "BL\tNo storage for statistics"));

	cBlLoc::done();
#if defined(_WIN32)
	if( m_hFsSync )
	{
		if( tFSDrv_Done pFSDrv_Done = (tFSDrv_Done)PrGetProcAddress(m_hFsSync, "FSSync_Done") )
			pFSDrv_Done();
		PrFreeLibrary(m_hFsSync);
		m_hFsSync = NULL;
	}
#endif // _WIN32

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR BlImpl::ObjectClose()
{
	
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ProductLogic::ObjectClose method" );
	
	// Place your code here
	
	return error;
}
// AVP Prague stamp end



#define REPORT_BALOON(_nNotification, _StateStg, _nState)	\
	if( _StateStg & _nState )	\
		check_status_changed(_nNotification, &_StateStg, 0, _nState, true);

tERROR pr_call BlImpl::gui_connected()
{
	tQWORD nStatusMask = LONG_LONG_CONST(0xFFFFFFFFFFFFFFFF);
	
	if( m_bIsKlopLimited )
	{
		REPORT_BALOON(eNtfLicenseProblems, m_State.m_KeyState, eKeyLimited);
		return errOK;
	}

	m_bIsGuiConnected = cTRUE;
	if( m_bIsNeedUpdateAVState )
		update_security_av_state();
	if( m_bIsNeedUpdateASState )
		update_security_as_state();

	return check_status(nStatusMask);
}

tERROR BlImpl::check_status(tQWORD nStatusMask)
{
	REPORT_BALOON(eNtfLicenseExpiresSoon,	m_State.m_KeyState,        eKeyAboutExpiration);
	REPORT_BALOON(eNtfLicenseExpired,		m_State.m_KeyState,        eKeyExpired);
	REPORT_BALOON(eNtfLicenseExpired,		m_State.m_KeyState,        eKeyTrialExpired);
	REPORT_BALOON(eNtfLicenseProblems,		m_State.m_KeyState,        eKeyBlocked);
	REPORT_BALOON(eNtfLicenseProblems,		m_State.m_KeyState,        eNoKeys);
	REPORT_BALOON(eNtfLicenseProblems,		m_State.m_KeyState,        eKeyInvalid);
	REPORT_BALOON(eNtfLicenseExpiresSoon,	m_State.m_KeyState,        eKeyWillBeExpired);

	REPORT_BALOON(eNtfSelfDefense,			m_State.m_ProductState,    eProductNotProtected);
	REPORT_BALOON(eNtfOtherImportantEvents,	m_State.m_ProtectionState, eProtectionSafeMode);
	REPORT_BALOON(eNtfMyComputerNotScaned,	m_State.m_ProtectionState, eMyComputerNotScanned);
	REPORT_BALOON(eNtfTaskCanNotExecute,	m_State.m_ProtectionState, eTasksMalfunction);
	REPORT_BALOON(eNtfThreatsUntreated,		m_State.m_ThreatsState,    eThreatsUntreated);
	REPORT_BALOON(eNtfUpdateNeedReboot,		m_State.m_UpdateState,     eUpdateNeedReboot);

	REPORT_BALOON(eNtfAVBasesObsolete,		m_State.m_UpdateState,     eBasesNotActual);
	REPORT_BALOON(eNtfAVBasesOutOfDate,		m_State.m_UpdateState,     eBasesOutOfDate);
	REPORT_BALOON(eNtfDatabaseCorrupted,	m_State.m_UpdateState,     eBasesNotValid);
	REPORT_BALOON(eNtfDatabaseCorrupted,	m_State.m_UpdateState,     eBasesCorrupted);

	return errOK;
}

void BlImpl::ExpandEnvironmentString(LPCSTR strName, cStringObj& strValue)
{
	strValue = strName;
	if( !ExpandEnvironmentString(strValue) )
		strValue.erase();
}

bool BlImpl::ExpandEnvironmentString(cStringObj& strName)
{
	return sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(strName), 0, 0) == errOK_DECIDED;
}

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
// Extended method comment
//    Receives message sent to the object or to the one of the object parents as broadcast
// Parameters are:
//   "msg_cls_id"    : Message class identifier
//   "msg_id"        : Message identifier
//   "send_point"    : Object where the message was send initially
//   "ctx"           : Context of the object processing
//   "receive_point" : Point of distributing message (tree top for RegisterMsgHandler call
//   "par_buf"       : Buffer of the parameters
//   "par_buf_len"   : Lenght of the buffer of the parameters
tERROR BlImpl::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ProductLogic::MsgReceive method" );
	
	cSerializable * pSer = p_par_buf_len == SER_SENDMSG_PSIZE ? (cSerializable *)p_par_buf : NULL;
	
	switch( p_msg_cls_id )
    {
    case pmc_TRANSPORT:
		switch(p_msg_id)
		{
        case pm_REQEST_PROXY_SETTINGS:
			{
				cProxySettings *proxySettings = pSer && pSer->getIID() == cProxySettings::eIID
                    ? reinterpret_cast<cProxySettings *>(pSer) : NULL;
                if(proxySettings)
                    proxySettings->assign(m_settings.m_ProxySettings, true);
                else
                {
                    return errPARAMETER_INVALID;
                	PR_TRACE((this, prtERROR, "Failed to get proxy server settings from Bussiness Logic"));
                }
			}
            break;
        }
        break;

	case pmc_UPDATER:
		switch(p_msg_id)
		{
		case pm_COMPONENT_PREINSTALLED:
			{
				cUpdaterUpdatedFileList *updatedFiles = pSer && pSer->getIID() == cUpdaterUpdatedFileList::eIID ?
					reinterpret_cast<cUpdaterUpdatedFileList *>(pSer) : NULL;
                if(updatedFiles)
				    error = checkReceivedComponentUpdate(updatedFiles);
			}
			break;
		case pm_COMPONENT_UPDATED:
			{
				cUpdatedComponentName *updatedComponentName
                    = pSer && pSer->getIID() == cUpdatedComponentName::eIID
                    ? reinterpret_cast<cUpdatedComponentName *>(pSer) : NULL;
				processReceivedComponentUpdate(updatedComponentName);
			}
			break;
// 		case pm_FILE_LIST_ON_SUCCESS_UPDATE:
// 			if( pSer && pSer->isBasedOn(cUpdaterUpdatedFileList::eIID) )
// 				pdm_update_hashes((cUpdaterUpdatedFileList *)pSer);
// 			break;
		}
		break;
    case pmc_UPDATER_CONFIGURATOR:
		switch(p_msg_id)
        {
        case pm_GET_SETTINGS:
            error = adjustUpdaterConfiguration(reinterpret_cast<cUpdaterConfigurator *>(p_ctx));
            break;
		case pm_UPDATE_RESULT:
			error = processReceivedUpdate(reinterpret_cast<cUpdaterConfigurator *>(p_ctx));
            break;
		case pm_UPDATE_SCHEDULE:
            error = updateAutomaticScheduleForUpdater(reinterpret_cast<cUpdaterConfigurator *>(p_ctx));
            break;
        case pm_ROLLBACK_AVAILABILITY:
            update_updater_rollback(reinterpret_cast<cUpdaterConfigurator *>(p_ctx)->get_pgrollbackAvailable());
            break;
		}
		break;
			
	case pmcPROFILE:
		switch ( p_msg_id ){
		case pmPROFILE_STATE_CHANDED:
			if( pSer && pSer->isBasedOn(cProfileBase::eIID) )
			{
//				if( ((cProfileBase *)pSer)->m_sType == AVP_TASK_WEBMONITOR )
//				{
//					int sadvasdvs = 0;
//				}

				if (!m_bIsStopping || ((cProfileBase *)pSer)->m_nState != PROFILE_STATE_FAILED)
				{
					update_security_state((cProfileBase *)pSer);
					update_protection_state((cProfileBase*)pSer, false);
				}
			}
			break;
		case pmPROFILE_SETTINGS_MODIFIED:
			update_protection_state( 0 /*(cProfileBase*)pSer*/, false  );
			break;
		case pmPROFILE_CANT_START:
			if (!m_bIsStopping)
			{
				if( pSer && pSer->isBasedOn(cProfileBase::eIID) )
				{
					cProfileBase *tp = (cProfileBase *)pSer;
					if( tp->isTask() )
					{
						tp->m_nState = PROFILE_STATE_UNK;
						process_notification(eNtfTaskCanNotExecute, tp);
					}
				}
			}
			break;
		}
		break;
				
	case pmcTM_EVENTS :
		switch( p_msg_id ) {
		case pmTM_EVENT_START                        : error = startup(); break;
		case pmTM_EVENT_STARTUP_TASKS_GOING_TO_START : break;
		case pmTM_EVENT_GUI_CONNECTED                : gui_connected(); break;
		case pmTM_EVENT_GOING_TO_STOP                : prepare_to_stop(); break;
		case pmTM_EVENT_IDLE                         : update_klop_state(false); clear_caches(); break;
		}
		break;
					
	case pmc_LICENSING:
		switch( p_msg_id ) {
		case pm_DATA_CHANGED:
		case pm_LICENSE_REPLACE:
			update_licensing_state();
			update_klop_state(true);
			break;
		case pm_CUSTOMER_ID_OBTAINED:
			if (m_pLic)
			{
				m_pLic->GetCustomerCredentials(cAutoString(m_settings.m_strCustomerId),
					cAutoString(m_settings.m_strCustomerPwd));
#ifdef _DEBUG
				tDWORD flags = PSWD_SIMPLE;
#else
				tDWORD flags = 0;
#endif
				if( m_hCryptoHelper )
					m_hCryptoHelper->CryptPassword(&cSerString(m_settings.m_strCustomerPwd), flags);

				sysSendMsg(pmc_TASK_SETTINGS_CHANGED, 0, NULL, NULL, NULL);
			}
			break;
		}
		break;

	case pmc_SYSTEM_TIME_CHANGED:
		update_avs_state();
		update_licensing_state();
		sysSendMsg(pmc_PRODUCT, pm_PRODUCT_STATE_CHANGED, NULL, NULL, NULL);
		break;
						
	case pmc_AVS:
		switch( p_msg_id ) {
		case pm_BASES_PRELOAD:
			error = check_bases_toload((cAVP3BasesInfo*)pSer, !p_ctx);
			break;

		case pm_BASES_LOADED:
			update_avs_bases_info((cAVP3BasesInfo*)pSer, !p_ctx);
			update_avs_state();
			break;

		case pm_THREATS_STATUS_CHANGED:
			update_avs_state();
			break;

		case pm_BASES_LOADFAILED:
			if( p_ctx ) // not for update
				update_avs_state();
			break;
		}
		break;
	
	case pmc_ANTISPAM_HAS_BEEN_TRAINED:
		update_as_state();
		break;
		
	case pmc_TASK_OS_REBOOT_REQUEST:
		update_task_reboot_state();
		break;
		
	case pmc_UPDATE_THREATS_LIST:
		//process_notification(eNtfNone, pSer, 0, p_msg_cls_id, p_msg_id);
		break;
		
#ifdef _DEBUG
	case 0x7fd52e63: // Test events from GUI
		process_notification(eNtfNone, pSer, 0, p_msg_cls_id, p_msg_id);
		break;
#endif // _DEBUG

// 	case pmc_TASKAHFW_WARNING_EVENT:
// 		if( pSer )
// 		{
// 			cSerializable * pData = pSer;
// 			if( pData->isBasedOn(cAHFWAppEventReport::eIID) ||
// 				pData->isBasedOn(cAHFWPacketEventReport::eIID) )
// 				process_notification(eNtfFirewallRule, pData);
// 		}
// 		break;
// 	
// 	case pmc_TASKAHFWAPPCHANGED_REPORT_EVENT:
// 		if( pSer )
// 		{
// 			cSerializable * pData = (cSerializable *)pSer;
// 			if( pData->isBasedOn(cAHFWAppChanged::eIID) )
// 				process_notification(eNtfFirewallRule, pData);
// 		}
// 		break;

	case pmc_PRODUCT_OPERATION:
		switch(p_msg_id)
		{
		case pm_PRODUCT_CHECK_STATUS:
			if( p_par_buf && p_par_buf_len && (*p_par_buf_len == sizeof(tQWORD)) )
				check_status(*(tQWORD *)p_par_buf);
			break;

// 		case pm_FW_UPDATE_NETWORKS:
// 			if( pSer && pSer->isBasedOn(cFwNets::eIID) )
// 			{
// 				error = fw_update_networks((cFwNets*)pSer);
// 			    if (error == errOK)
// 					error = errOK_DECIDED;
// 			}
// 			break;
// 
//         case pm_FW_ADD_NETWORK:
//             if (pSer && pSer->isBasedOn(cFwChangeNet::eIID) )
//             {
//                 error = fw_add_network((cFwChangeNet*)pSer);
// 			    if (error == errOK)
// 					error = errOK_DECIDED;
//             }
//             break;
// 
//         case pm_FW_DELETE_NETWORK:
//             if (pSer && pSer->isBasedOn(cFwChangeNet::eIID) )
//             {
//                 error = fw_delete_network((cFwChangeNet*)pSer);
// 			    if (error == errOK)
// 					error = errOK_DECIDED;
//             }
//             break;
// 
//         case pm_FW_CHANGE_NETWORK:
//             if (pSer && pSer->isBasedOn(cFwChangeNet::eIID) )
//             {
//                 error = fw_change_network((cFwChangeNet*)pSer);
//                 if (error == errOK)
//                     error = errOK_DECIDED;
//             }

		case pm_SRVMGR_REQUEST:
			if( pSer && pSer->isBasedOn(cSvrMgrRequest::eIID) )
				error = svrmgr_request((cSvrMgrRequest*)pSer);
			break;
		}
		break;
	
	case pmc_LOCALIZE_INFO:
		switch(p_msg_id)
		{
		case pm_LOCALIZE_NOTIFICATION:
			if( localize_info(pSer) )
				return errOK_DECIDED;
			break;

		case pm_LOCALIZE_PROFILE_NAME:
			if( pSer->isBasedOn(cSerString::eIID) )
				if( get_profile_name(*(cSerString*)pSer) )
					return errOK_DECIDED;
			break;
		}
		break;

	case pmc_USERNOTIFY:
		if( pSer )
		{
			cDetectNotifyInfo* pInfo = (cDetectNotifyInfo*)pSer;
			if( pInfo->isBasedOn(cDetectNotifyInfo::eIID) )
				send_net(*pInfo);
		}
		break;

	case pmc_SCHEDULER:
		switch (p_msg_id)
		{
		case BL_SCHEDULE_SEND_MAIL_NOTIFICATION: send_mail_notification(); break;
		case BL_SCHEDULE_REGISTER_DLL:           registerAllDlls(); update_avs_state(); break;
		case BL_SCHEDULE_CLEANUP:                cleanup(); break;
		case BL_SCHEDULE_CHECK_LIC:              update_licensing_state(); break;
		case BL_UPDATE_PRODUCT_STARTUP_MODE:     update_product_status(); break;
		case BL_SCHEDULE_CHECK_NET_STATUS:
			{
				tDWORD dwLastNetAddrListChange;
				if (!m_NetDetect)
					break;
				if (PR_FAIL(m_NetDetect->GetLastIpAddrChange(&dwLastNetAddrListChange)))
					break;
				if (m_dwLastNetStateChange != dwLastNetAddrListChange)
				{
					sysSendMsg(pmc_SYSTEM_STATE, pm_NETWORK_STATE_CHANGED, 0, NULL, NULL );
					m_dwLastNetStateChange = dwLastNetAddrListChange;
				}
			}
			break;
		case BL_SCHEDULE_CHECK_NAGENT:
			if( PR_FAIL(check_nagent_installed()) )
			{
				remove_policy();
				
				m_scheduler->DeleteSchedule(pmc_SCHEDULER, BL_SCHEDULE_CHECK_NAGENT);
			}
			break;
		}
		break;

	case pmc_REMOTE_GLOBAL:
		if( p_msg_id == pm_REMOTE_GLOBAL_SELFPROT_INFO )
		{
			cProtectionNotify* pNotify = (cProtectionNotify*)pSer;
			if( pNotify && pNotify->isBasedOn(cProtectionNotify::eIID) )
			{
				pNotify->m_strProfile = AVP_PROFILE_SELFPROT;
				pNotify->m_strTaskType = AVP_TASK_SELFPROT;
				process_notification(eNtfSelfDefense, pSer);
			}
		}
		break;

	case pmc_USERBAN_SYNC:
		if( p_msg_id == pm_BANLIST_ITEM_ADD ||
			p_msg_id == pm_BANLIST_ITEM_DEL )
		{
			if( pSer && pSer->isBasedOn(cUserBanItem::eIID) )
			{
				((cUserBanItem*)pSer)->m_Action = (p_msg_id == pm_BANLIST_ITEM_ADD) ?
					cUserBanItem::_eBan_Added : cUserBanItem::_eBan_Deleted;
				process_notification(eNtfNone, pSer);
			}
		}
		break;
	case pmc_SETTINGS:
		if (p_msg_id == pm_SETTINGS_UPGRADE)
		{
			if (!pSer || !pSer->isBasedOn(cProfileEx::eIID) || !((cProfileEx*)pSer)->isProtection())
				return  errPARAMETER_INVALID;
			if (upgrade_settings(*(cProfileEx*)pSer))
				return errOK_DECIDED;
			return errOK;
		}
		break;
	case pmc_FSSYNC:
#if defined (_WIN32)
		if((p_msg_id == pm_FSSYNC_RegisterInvisibleThread || p_msg_id == pm_FSSYNC_UnregisterInvisibleThread) &&
			pSer && pSer->isBasedOn(cFSSyncRequest::eIID))
		{
			error = errUNEXPECTED;
			if( m_hFsSync )
			{
				HANDLE TID = (HANDLE)((cFSSyncRequest *)pSer)->m_TID;
				if( p_msg_id == pm_FSSYNC_RegisterInvisibleThread )
				{
					if( tFSDrv_AddInvThreadByHandle pFSDrv_AddInvThreadByHandle = (tFSDrv_AddInvThreadByHandle)PrGetProcAddress(m_hFsSync, "FSSync_RTH") )
						error = pFSDrv_AddInvThreadByHandle(TID) ? errOK : errNOT_OK;
				}
				else
				{
					if( tFSDrv_DelInvThreadByHandle pFSDrv_DelInvThreadByHandle = (tFSDrv_DelInvThreadByHandle)PrGetProcAddress(m_hFsSync, "FSSync_RTHU") )
						error = pFSDrv_DelInvThreadByHandle(TID) ? errOK : errNOT_OK;
				}
			}
		}
#endif //_WIN32
		break;
			}

  return error;
}
// AVP Prague stamp end



namespace UninstallPwd
{
	static const tCHAR* pszUninstPwdValue = "LocalizationId";

	cRegistry* GetUninstallPwdLocation()
	{
		cAutoObj<cRegistry> reg;
		if (PR_FAIL(g_root->sysCreateObject((hOBJECT *)&reg, IID_REGISTRY, PID_WIN32_REG)) || !reg)
			return NULL;

		cStrObj root = "HKLM\\" VER_PRODUCT_REGISTRY_PATH "\\Environment";
		root.copy((cObj*)reg, pgROOT_POINT);
		reg->propSetBool(pgOBJECT_RO, cFALSE);

		if (PR_FAIL(reg->sysCreateObjectDone()))
			return NULL;

		return reg.relinquish();
	}

	void DeleteUninstallPwd()
	{
		cAutoObj<cRegistry> reg(GetUninstallPwdLocation());
		if (!reg)
			return;

		tDWORD res = 0;
		tTYPE_ID type = tid_STRING;
		if (PR_FAIL(reg->GetValue(&res, cRegRoot, pszUninstPwdValue, &type, NULL, 0)))
			return;

		reg->DeleteValue(cRegRoot, pszUninstPwdValue);
	}

	std::string HashPasswd(const wchar_t* pwd, size_t charlen) 
	{
		const unsigned int MD5_DIGEST_LENGTH = 16;
		static const char c_szHexChars[] = "0123456789ABCDEF";

		std::string res;
		cAutoObj<cHash> hHash;
		tBYTE vpwd_hash[MD5_DIGEST_LENGTH];
		if (PR_FAIL(g_root->sysCreateObjectQuick((hOBJECT*)&hHash, IID_HASH, PID_HASH_MD5))
			|| PR_FAIL(hHash->Update((tBYTE*)pwd, charlen * sizeof(*pwd)))
			|| PR_FAIL(hHash->GetHash(vpwd_hash, sizeof(vpwd_hash))))
			return res;

		res.resize(MD5_DIGEST_LENGTH*2);
		for( int i = 0; i < MD5_DIGEST_LENGTH; i++ )
		{
			res.at(i*2  ) = c_szHexChars[(vpwd_hash[i] >> 4) & 0xF];
			res.at(i*2+1) = c_szHexChars[vpwd_hash[i] & 0xF];
		}
		return res;
	}


	void SetUninstallPwd(const cStrObj& pwd)
	{
		cAutoObj<cRegistry> reg(GetUninstallPwdLocation());
		if (!reg)
			return;
		std::string hash = "00" + HashPasswd(pwd.c_str(cCP_UNICODE), pwd.size());
		char oldhash[128] = {0, };
		tDWORD res = 0;
		tTYPE_ID type = tid_STRING;
		if (PR_SUCC(reg->GetValue(&res, cRegRoot, pszUninstPwdValue, &type, oldhash, sizeof(oldhash)))
			&& hash.compare(0, res, oldhash) == 0)
			return;
		reg->SetValue(cRegRoot, pszUninstPwdValue, tid_STRING, (tPTR)hash.c_str(), hash.size(), cTRUE);
	}
}

// Проверка, что если все вложенные профили защиты отключены, надо отключить и родительский профиль.
// Возвращает true - если что-то поменялось
bool CheckAllProfilesDisabled(cProfileEx& profile)
{
	if (profile.m_aChildren.empty())
		return false;
	bool res = false;
	bool hasChilds = false;
	for (tDWORD i = 0; i < profile.m_aChildren.size(); ++i)
	{
		cSerObj<cProfileEx>& child = profile.m_aChildren[i];
		if (!child->persistent() || child->isService())
			continue; // не относится к RTP
		hasChilds = true;
		if (CheckAllProfilesDisabled(*child))
			res = true;
		if (child->enabled())
			return res;
	}
	if (!profile.enabled())
		return res;
	// Не отключаем защиту, если нет ни одной задачи RTP.
	// Это нужно, чтобы в SOS Protection был enabled (для того, чтобы при
	// последующем апгрейде на WKS защита была включена)
	if (profile.isProtection() && !hasChilds)
		return res;
	profile.m_cfg.m_bEnabled = cFALSE; // все дети отключены
	return true;
}

// проверка соответствия версий профилей
// возвращает true - если что-то поменялось
bool CheckAllProfilesVersion(cProfileEx& profile)
{
	bool res = false;
	{
		cSerObj<cTaskSettings> pSer;
		cTaskSettings* settings = profile.settings();
		if(settings &&
				settings->isBasedOn(cTaskSettings::eIID) &&
				PR_SUCC(pSer.init(settings->getIID())) &&
				pSer->m_dwVersion != settings->m_dwVersion)
		{
			settings->m_dwVersion = pSer->m_dwVersion;
			res = true;
		}
	}
	for(tDWORD i = 0, size = profile.m_aChildren.size(); i < size; ++i)
	{
		if(CheckAllProfilesVersion(*profile.m_aChildren[i]))
			res = true;
	}
	return res;
}

bool BlImpl::upgrade_settings(cProfileEx& protection)
{
	bool res = false;
	cBLSettings* blset = (cBLSettings*) protection.settings();
	if (!blset || !blset->isBasedOn(cBLSettings::eIID))
		return res;

	// bf 21721
	if (CheckAllProfilesDisabled(protection))
		res = true;

	// bf 25033
	// КОСТЫЛЬ: вообще это надо делать под веткой if (v_major == 6 && v_minor == 0 && v_mp <= 2)
	// но т.к. при апгрейде с 303 и 411 SettingsVersion устанавливается в свежую версию, то
	// этот код не выполняется, и получается жопа.
	// Надо разобраться, в какое значение выставлять SettingsVersion в инсталлере и на основе
	// этого переделать эту функцию.
	cProfileEx* pUpdProfile = protection.find_profile(AVP_PROFILE_UPDATER);
	if (pUpdProfile)
	{
		cUpdaterSettings* pUpdSet = (cUpdaterSettings *) pUpdProfile->settings();
		if (pUpdSet && pUpdSet->isBasedOn(cUpdaterSettings::eIID))
		{
			bool ak_found = false, kl_found = false;
			for (tDWORD i = 0; i < pUpdSet->m_aSources.size(); ++i)
			{
				if (pUpdSet->m_aSources[i].m_dwType == UPDSRC_AK)
					ak_found = true;
				else if (pUpdSet->m_aSources[i].m_dwType == UPDSRC_KL)
					kl_found = true;
			}

			if (!ak_found)
			{
				cStrObj strProductType; ExpandEnvironmentString("%ProductType%", strProductType);
				if (strProductType == AVP_PRODUCT_WKS || strProductType == AVP_PRODUCT_FS)
				{
					cUpdaterSource& ak = pUpdSet->m_aSources.push_back();
					ak.m_bEnable = pUpdSet->m_blAKServerUse;
					ak.m_dwType = UPDSRC_AK;
					res = true;
				}
			}

			if (!kl_found)
			{
				cUpdaterSource& kl = pUpdSet->m_aSources.push_back();
				kl.m_bEnable = pUpdSet->m_bUseInternalSources;
				kl.m_dwType = UPDSRC_KL;
				res = true;
			}
		}
	}

	cStrObj strProductVersion; ExpandEnvironmentString("%ProductVersion%", strProductVersion);
	if (blset->m_SettingsVersion == strProductVersion)
	{
		PR_TRACE((this, prtNOTIFY, "bl\tno need in settings upgrade, settings is up to date"));
		return res;
	}

	res = true;

	if (blset->m_SettingsVersion.empty())
	{
		PR_TRACE((this, prtNOTIFY, "bl\tno need in settings upgrade, settings is up to date"));
		blset->m_SettingsVersion = strProductVersion;
		return res;
	}
	PR_TRACE((this, prtIMPORTANT, "bl\tupgading settings from '%S' to '%S'...", blset->m_SettingsVersion.data(), strProductVersion.data()));

	tDWORD v_major = 6, v_minor = 0, v_mp = 0, v_build = 0;
	sscanf((tSTRING)blset->m_SettingsVersion.c_str(cCP_ANSI), "%d.%d.%d.%d", &v_major, &v_minor, &v_mp, &v_build);
	blset->m_SettingsVersion = strProductVersion;

	if (v_major == 6 && v_minor == 0 && v_mp == 0)
	{
		// bf 16819
		for (tUINT i = 0; i < protection.count(); ++i)
		{
			cProfileEx& prof = *protection[i];
			if (prof.m_sType != AVP_TASK_ONDEMANDSCANNER)
				continue;
			cTaskSettings* pSet = prof.settings();
			if (!pSet || !pSet->m_pTaskBLSettings || !pSet->m_pTaskBLSettings->isBasedOn(cScanerProductSpecific::eIID))
				continue;
			tBOOL& bRunOnStartup = ((cScanerProductSpecific*)pSet->m_pTaskBLSettings.ptr_ref())->m_OBSOLETE_bRunOnStartup;
			if (bRunOnStartup != tBOOL(-1))
			{
				prof.schedule().m_eType = schtOnStartup;
				prof.schedule().m_eMode = bRunOnStartup ? schmEnabled : schmManual;
				bRunOnStartup = tBOOL(-1);
			}
		}

//		// bf 27255
// 		if (cProfileEx* pPdmProfile = protection.find_profile(AVP_PROFILE_PDM, false, true))
// 		{
// 			cPDMSettings* pPdmSett = (cPDMSettings*) pPdmProfile->settings();
// 			if (pPdmSett && pPdmSett->isBasedOn(cPDMSettings::eIID))
// 			{
// 				cVector<cPDMSettings_Item> defset = cPDMSettings().m_Set;
// 				for (tDWORD i = pPdmSett->m_Set.size(); i < defset.size(); ++i)
// 					pPdmSett->m_Set.push_back(defset[i]);
// 			}
// 		}
	}

	if (v_major == 6 && v_minor == 0 && v_mp <= 1)
	{
		// bf 16789
		blset->m_bAllowServiceStop = cFALSE;

		// bf 19205
		cProfileEx* pRetrProfile = protection.find_profile(AVP_PROFILE_RETRANSLATION);
		cProfileEx* pUpdProfile = protection.find_profile(AVP_PROFILE_UPDATER);
		if (pRetrProfile && pUpdProfile)
		{
			cUpdaterSettings* pRetrSet = (cUpdaterSettings *) pRetrProfile->settings();
			cUpdaterSettings* pUpdSet = (cUpdaterSettings *) pUpdProfile->settings();
			if (pRetrSet && pRetrSet->isBasedOn(cUpdaterSettings::eIID)
				&& pUpdSet && pUpdSet->isBasedOn(cUpdaterSettings::eIID))
			{
				pUpdSet->m_bUseRetr = pRetrProfile->m_cfg.enabled() && pRetrProfile->m_cfg.schedule().m_eMode != schmManual;
				pUpdSet->m_strRetrPath = pRetrSet->m_strRetrPath;
			}
		}
	}

	if (v_major == 6 && v_minor == 0 && v_mp <= 2)
	{
		// bf 22303 (see also 18845)
		cBLSettings bl_this_set;
		if (PR_SUCC(m_tm->GetProfileInfo(AVP_PROFILE_PROTECTION, &bl_this_set)))
			blset->m_NSettings = bl_this_set.m_NSettings;
	}

	// turn off HTTP stream scan
	if (cProfileEx* pHttpScanProfile = protection.find_profile(AVP_PROFILE_HTTPSCAN, false, true))
	{
		cHTTPScanSettings* pHttpScanSett = (cHTTPScanSettings*) pHttpScanProfile->settings();
		if (pHttpScanSett && pHttpScanSett->isBasedOn(cHTTPScanSettings::eIID))
		{
			if(pHttpScanSett->m_HTTPSettings.m_bUseStreamProcessor)
			{
				if(pHttpScanSett->m_HTTPSettings.m_bUseTimeout == cFALSE)
				{
					pHttpScanSett->m_HTTPSettings.m_bUseTimeout = cTRUE;
					pHttpScanSett->m_HTTPSettings.m_dwTimeout = 1;
				}
				pHttpScanSett->m_HTTPSettings.m_bUseStreamProcessor = cFALSE;
			}
		}
	}

	if (CheckAllProfilesVersion(protection))
		res = true;

	return res;
}
//bool merge_hips_groups(CHipsResourceGroupsItem * pCurGroup, CHipsResourceGroupsItem * pNewGroup)
//{
//	if (pCurGroup == 0 || pNewGroup == 0)
//		return false;
//	for (DWORD i = 0; i < pCurGroup->m_vChildResGroupList.size(); i++)
//	{
//		if (!IS_KL_RES_ID(pCurGroup->m_vChildResGroupList[i].m_ResGroupID))
//		{
//			//	not KL group, need to add
//			pNewGroup->m_vChildResGroupList.push_back(pCurGroup->m_vChildResGroupList[i]);
//		}
//		else
//		{
//			//	KL group, need to check childs
//			merge_hips_groups(
//				&pCurGroup->m_vChildResGroupList[i],
//				&pNewGroup->m_vChildResGroupList[i]
//				);
//		}
//	}
//	return true;
//}
bool merge_hips_groups(cAppGroup * pCurGroup, cAppGroup * pNewGroup)
{
	if (pCurGroup == 0 || pNewGroup == 0)
		return false;
	for (DWORD i = 0; i < pCurGroup->m_aChilds.size(); i++)
	{
		if (!IS_KL_RES_ID(pCurGroup->m_aChilds[i].m_nAppGrpID))
		{
			//	not KL group, need to add
			pNewGroup->m_aChilds.push_back(pCurGroup->m_aChilds[i]);
		}
		else
		{
			//	KL group, need to check childs
			merge_hips_groups(
				&pCurGroup->m_aChilds[i],
				&pNewGroup->m_aChilds[i]
				);
		}
	}
	return true;
}

//////////////////////////////////////////////////////
//	function to merge settings
//	pCurSettings - from user profile
//	pNewSettings - from xml base
//////////////////////////////////////////////////////
bool BlImpl::merge_hips_settings(CHipsSettings * pCurSettings, CHipsSettings * pNewSettings)
{
	if (pCurSettings == 0 || pNewSettings == 0)
		return false;

	//	merge common settings
	pNewSettings->m_FileAndRegState = pCurSettings->m_FileAndRegState;
	pNewSettings->m_DevicesState	= pCurSettings->m_DevicesState;
	pNewSettings->m_NetworkState	= pCurSettings->m_NetworkState;
	pNewSettings->m_PermissionsState = pCurSettings->m_PermissionsState;

	//	merge FW settings
	pNewSettings->m_FirewallSettings = pCurSettings->m_FirewallSettings;


	bool IsWasFound = false;
	//	merging resources
	//!!! need merge resource tree
	cResourceIterator It(&pCurSettings->m_Resource);
	cResource * pTempCurRes = 0;
	cResource * pTempRes = 0;
	bool bSkip = false;
	while (pTempCurRes = It.GetNext(bSkip))
	{
		if (pTempCurRes->m_nID & HIPS_USER_PREFIX)
		{
			//	need skip all child of this res (will be added automaticaly)
			bSkip = true;
			//	this is users resource, need to add
			pNewSettings->m_Resource.AddResource(*pTempCurRes);
			continue;
		}
		//	not user resource
		//	reset flag
		bSkip = false;

		if ((pTempCurRes->m_nFlags != 0) &&
			(!HIPS_GET_RES_ENABLED(pTempCurRes->m_nFlags)))
		{
			//	KL resource is disabled
			pTempRes = pNewSettings->m_Resource.Find(pTempCurRes->m_nID);
			if (pTempRes)
			{
				//	reset enabled flag
				pTempRes->m_nFlags &= ~0x1F; 
			}
		}

	};
	//for (DWORD i = 0; i < pCurSettings->m_vResList.count(); i++)
	//{
	//	if (IS_KL_RES_ID(pCurSettings->m_vResList[i].m_ResID))
	//	{
	//		if (pCurSettings->m_vResList[i].m_Flags == CHipsResourceItem::fDisabled)
	//		{
	//			//	need to disable res
	//			IsWasFound = false;
	//			for (DWORD j = 0; j < pNewSettings->m_vResList.count(); j++)
	//			{
	//				if (pCurSettings->m_vResList[i].m_ResID == pNewSettings->m_vResList[j].m_ResID)
	//				{
	//					pNewSettings->m_vResList[j].m_Flags = pCurSettings->m_vResList[i].m_Flags;
	//					IsWasFound = true;
	//					break;
	//				}
	//			}
	//			if (!IsWasFound)
	//			{
	//				PR_TRACE((this, prtNOT_IMPORTANT, "bl\t merge_hips_settings disabled KL resource with ID=%d was not found)",
	//					pCurSettings->m_vResList[i].m_ResID));
	//			}

	//		}
	//	}
	//	else
	//	{
	//		//	this is users resource, need to add
	//		pNewSettings->m_vResList.push_back(pCurSettings->m_vResList[i]);
	//	}
	//}
	//	merging common rules
	for (DWORD i = 0; i < pCurSettings->m_vRuleList.count(); i++)
	{
		if (IS_KL_RULE_ID(pCurSettings->m_vRuleList[i].m_RuleId))
		{
			//	need to disable rule
			IsWasFound = false;
			for (DWORD j = 0; j < pNewSettings->m_vRuleList.count(); j++)
			{
				if (pCurSettings->m_vRuleList[i].m_RuleId == pNewSettings->m_vRuleList[j].m_RuleId)
				{
					//	ths same rule finded
					if (pCurSettings->m_vRuleList[i].m_RuleState == CHipsRuleItem::fDisabled)
					{
						// set to user state
						pNewSettings->m_vRuleList[j].m_RuleState = pCurSettings->m_vRuleList[i].m_RuleState;
					}
					else if (pCurSettings->m_vRuleList[i].m_AccessFlag != pNewSettings->m_vRuleList[j].m_AccessFlag)
					{
						pNewSettings->m_vRuleList[j].m_AccessFlag = pCurSettings->m_vRuleList[i].m_AccessFlag;
					}
					IsWasFound = true;
					break;
				}
			}
			if (!IsWasFound)
			{
				//	this rule was excluded by KL
				PR_TRACE((this, prtNOT_IMPORTANT, "bl\t merge_hips_settings disabled KL rule with ID=%d was not found)",
					pCurSettings->m_vRuleList[i].m_RuleId));
			}
		}
		else
		{
			//	this is users rule, need to add
			pNewSettings->m_vRuleList.push_back(pCurSettings->m_vRuleList[i]);
		}
	}
	//	merging packet rules
	for (DWORD i = 0; i < pCurSettings->m_PacketRules.count(); i++)
	{
		if (IS_KL_RULE_ID(pCurSettings->m_PacketRules[i].m_RuleId))
		{
			//	need to disable rule
			IsWasFound = false;
			for (DWORD j = 0; j < pNewSettings->m_PacketRules.count(); j++)
			{
				if (pCurSettings->m_PacketRules[i].m_RuleId == pNewSettings->m_PacketRules[j].m_RuleId)
				{
					//	ths same rule finded
					if (pCurSettings->m_PacketRules[i].m_RuleState == CHipsRuleItem::fDisabled)
					{
						// set to user state
						pNewSettings->m_PacketRules[j].m_RuleState = pCurSettings->m_PacketRules[i].m_RuleState;
					}
					else if (pCurSettings->m_PacketRules[i].m_AccessFlag != pNewSettings->m_PacketRules[j].m_AccessFlag)
					{
						pNewSettings->m_PacketRules[j].m_AccessFlag = pCurSettings->m_PacketRules[i].m_AccessFlag;
					}
					IsWasFound = true;
					break;
				}
			}
			if (!IsWasFound)
			{
				//	this rule was excluded by KL
				PR_TRACE((this, prtNOT_IMPORTANT, "bl\t merge_hips_settings disabled KL rule with ID=%d was not found)",
					pCurSettings->m_PacketRules[i].m_RuleId));
			}
		}
		else
		{
			//	this is users rule, need to add
			pNewSettings->m_PacketRules.push_back(pCurSettings->m_PacketRules[i]);
		}
	}
	//	merging applications
	for (DWORD i = 0; i < pCurSettings->m_vAppList.count(); i++)
	{
		if (IS_KL_RES_ID(pCurSettings->m_vAppList[i].m_AppId))
		{
		}
		else
		{
			//	this is users rule, need to add
			pNewSettings->m_vAppList.push_back(pCurSettings->m_vAppList[i]);
		}
	}
	//	merging resource groups
	//merge_hips_groups(&pCurSettings->m_ResGroups, &pNewSettings->m_ResGroups);
	//	merging application groups
	merge_hips_groups(&pCurSettings->m_AppGroups, &pNewSettings->m_AppGroups);

	//
	//	hide empty sec-level app groups
	//
	cVector<tDWORD> SecLevelAppGrList;
	cAppGroup TempGr;

	//	fill sec-level application group list
	for (DWORD i = 0; i < pNewSettings->m_AppGroups.m_aChilds.size(); i++)
	{
		for (DWORD j = 0; j < pNewSettings->m_AppGroups.m_aChilds[i].m_aChilds.size(); j++)
		{
			pNewSettings->EnumAppGroups(
				pNewSettings->m_AppGroups.m_aChilds[i].m_aChilds[j],
				TempGr,
				&CHipsSettings::CollectAllKLAppGrID,
				(void*)&SecLevelAppGrList);
		}
	}
	DWORD LastAppGr = 0;
	bool IsFinded = false;
	DWORD j = 0;
	cAppGroup * pOutGr = 0;
	cAppGroup * pOutParentGr = 0;
	while (j < SecLevelAppGrList.size())
	{
		IsFinded = false;
		LastAppGr = 0;
		for (DWORD i = 0; i < pNewSettings->m_vAppList.count(); i++)
		{
			if (LastAppGr != pNewSettings->m_vAppList[i].m_AppGrId)
			{
				LastAppGr = pNewSettings->m_vAppList[i].m_AppGrId;
				if (LastAppGr == SecLevelAppGrList[j])
				{
					IsFinded = true;
					break;
				}
			}
		}
		if (!IsFinded)
		{
			pNewSettings->DeleteAppGrById(SecLevelAppGrList[j]);
			SecLevelAppGrList.remove(j, j);
		}
		else j++;
	}

	return true;
}
//////////////////////////////////////////////////////
//	function to apply default or merged settings
//////////////////////////////////////////////////////
bool BlImpl::update_hips_settings()
{
	if (m_tm == 0 || g_root == 0)
	{
		PR_TRACE((this, prtERROR, "bl\tupdate_hips_settings (m_tm = %x, g_root = %x )", m_tm, g_root));
		return false;
	}
	tERROR err;
	bool IsCurSettingsExist = true;
	CHipsSettings cur_hips_settings;
	//	get current settings
	err = m_tm->GetProfileInfo(AVP_PROFILE_HIPS, &cur_hips_settings);

	if (PR_FAIL(err))
	{
		PR_TRACE((this, prtERROR, "bl\tupdate_hips_settings m_tm->GetProfileInfo return %x", err));
		IsCurSettingsExist = false;
	}

	//	get settings from xml-file
	hREGISTRY pReg = 0;
	err = g_root->sysCreateObject ( reinterpret_cast<hOBJECT*> ( &pReg ), IID_REGISTRY, PID_PROPERTIESMAP, SUBTYPE_ANY );
	if ( PR_SUCC ( err ) && pReg) {
		cStrObj strHipsBaseFile;
		ExpandEnvironmentString("%Bases%", strHipsBaseFile);
		strHipsBaseFile.check_last_slash(true);
		strHipsBaseFile += "hipsDB.xml";

		//sprintf(BaseFileName, "%s\\%s", SelfFileName, "hips_base.xml");
		err = pReg->propSetStr ( 0 , pgOBJECT_NAME, strHipsBaseFile.c_str(cCP_ANSI), 0, cCP_ANSI );
	}
	else
	{
		PR_TRACE((this, prtERROR, "bl\tupdate_hips_settings g_root->sysCreateObject return %x", err));
	}
	if ( PR_SUCC ( err ) && pReg)
		err = pReg->sysCreateObjectDone ();

	if (PR_FAIL(err))
	{
		PR_TRACE((this, prtERROR, "bl\tupdate_hips_settings g_root->sysCreateObjectDone return %x", err));
	}

	CHipsSettings * pBaseSettings = 0;

	if ( PR_SUCC ( err ) && pReg)
	{
		err = g_root->RegDeserialize((cSerializable**)&pBaseSettings, pReg, 0, CHipsSettings::eIID);
		if (PR_FAIL(err))
		{
			PR_TRACE((this, prtERROR, "bl\tupdate_hips_settings g_root->RegDeserialize return %x, pReg=%x", err, pReg));
		}
	}

	if (pReg)
		pReg->sysCloseObject();

	if ( PR_SUCC ( err ) && pBaseSettings)
	{
		if (IsCurSettingsExist)
		{
			//	need to merge
			if (merge_hips_settings(&cur_hips_settings, pBaseSettings))
			{
				//pBaseSettings->m_PacketRules = cur_hips_settings.m_PacketRules;

				//	merging success, set merged settings
				err = m_tm->SetProfileInfo(AVP_PROFILE_HIPS, pBaseSettings, NULL, 0);
			}
			//	else auto apply current settings
		}
		else
		{
			//	apply settings from file
			err = m_tm->SetProfileInfo(AVP_PROFILE_HIPS, pBaseSettings, NULL, 0);
		}
	}

	if (pBaseSettings)
		pBaseSettings->release();

	return PR_SUCC(err);
}

// ---


// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR BlImpl::SetSettings( const cSerializable* p_settings )
{
	cERROR err;
	PR_TRACE_FUNC_FRAME__( "ProductLogic::SetSettings method", &err._ );
	
	if( !p_settings || !p_settings->isBasedOn(cBLSettings::eIID) )
		return errPARAMETER_INVALID;

	tBOOL  bEnableSelfProtection = 2;
	tBOOL  bAllowServiceStop = 2;
	tBOOL  bEnableCheckActivity = 2;
	tBOOL  bChangedOasInstances = cFALSE;
	tDWORD dwAffinityMask = 0;
	tBOOL  bWriteUninstallPwd = cFALSE;
	tBOOL  bChangedReportLifetime = cFALSE;
	cStrObj sUninstallPwd;

	cStrObj strProductType; ExpandEnvironmentString("%ProductType%", strProductType);

	{
		cAutoCS cs(m_lock, true);
		
		tDWORD nOldState = m_State.m_ProductState;

		cBLSettings* pSett = (cBLSettings*)p_settings;

		if( m_settings.m_nOasInstances != pSett->m_nOasInstances )
			bChangedOasInstances = cTRUE;

		if( m_settings.m_Ins_InitMode || m_settings.m_bEnableSelfProtection != pSett->m_bEnableSelfProtection )
		{
			m_State.m_ProductState &= ~eProductNotProtected;
			bEnableSelfProtection = pSett->m_bEnableSelfProtection;
		}

		if( m_settings.m_bAllowServiceStop != pSett->m_bAllowServiceStop )
			bAllowServiceStop = pSett->m_bAllowServiceStop;

		if( m_settings.m_bEnableCheckActivity != pSett->m_bEnableCheckActivity )
			bEnableCheckActivity = pSett->m_bEnableCheckActivity;

		if( m_settings.m_nAffinityMask != pSett->m_nAffinityMask )
			dwAffinityMask = pSett->m_nAffinityMask;

		if (m_settings.m_bUseUninstallPwd != pSett->m_bUseUninstallPwd
			|| m_settings.m_UninstallPwd != pSett->m_UninstallPwd)
		{
			bWriteUninstallPwd = cTRUE;
			if (pSett->m_bUseUninstallPwd)
				sUninstallPwd = pSett->m_UninstallPwd;
		}

		bChangedReportLifetime = m_settings.m_ReportSpan != pSett->m_ReportSpan
			|| m_settings.m_QBSpan != pSett->m_QBSpan;

		err = m_settings.assign(*p_settings, true);
		
		if ( PR_SUCC(err) && !m_settings.m_Ins_DefaultKey.empty() )
		{
			m_tm->AddLicKey( cAutoString (m_settings.m_Ins_DefaultKey) );
			m_settings.m_Ins_DefaultKey.clear();
			m_settings.saveOneField( m_config, "settings", &m_settings.m_Ins_DefaultKey, sizeof(m_settings.m_Ins_DefaultKey) );
		}
		
		if ( PR_SUCC(err) )
		{
			m_settings.m_sDataPath; ExpandEnvironmentString(cENVIRONMENT_DATA_ROOT, m_settings.m_sDataPath);
			m_settings.m_sBasePath; ExpandEnvironmentString(cENVIRONMENT_BASES, m_settings.m_sBasePath);

			if( m_settings.m_bRunAtSystemStartup )
				m_State.m_ProductState &= ~eProductNotAutoRun;
			else
				m_State.m_ProductState |= eProductNotAutoRun;
		}
		
		if (PR_SUCC(err) && m_scheduler)
		{
			// run immediately, but on scheduler thread to avoid waiting for SCM on this thread
			cScheduleSettings schedule;
			m_scheduler->SetSchedule(pmc_SCHEDULER, BL_UPDATE_PRODUCT_STARTUP_MODE, &schedule, NULL);
		}
		
		if( m_hCryptoHelper )
		{
			if( m_tmLastFullScanState == (tProfileState)777 )
			{
				tDWORD c = 0;
				m_hCryptoHelper->StoreData(esiLicensingData, &c, sizeof(c));
				m_hCryptoHelper->StoreData(esiIsUpdatePerformed, &c, sizeof(c));
				m_hCryptoHelper->StoreData(esiIsTrialKeyInstalled, &c, sizeof(c));
				m_hCryptoHelper->StoreData(esiLastTrialClearVer, &c, sizeof(c));				
				m_tmLastFullScanState = PROFILE_STATE_UNK;
				saveOneField( m_config, "data", &m_tmLastFullScanState, 0 );
			}

			tDWORD size = sizeof(tBOOL);
			m_hCryptoHelper->GetData(esiIsUpdatePerformed, &m_bIsUpdatePerformed, &size);

			if( m_settings.m_Ins_InitMode )
			{
				m_hCryptoHelper->StoreData(esiValidBasesDate, &m_tmValidBasesDate, sizeof(time_t));
				m_hCryptoHelper->StoreData(esiValidBlackList, &m_ValidBlackList, sizeof(m_ValidBlackList)); //!!!
			}
			else
			{
				size = sizeof(time_t);
				m_hCryptoHelper->GetData(esiValidBasesDate, &m_tmValidBasesDate, &size);

				size = sizeof(m_ValidBlackList);
				m_hCryptoHelper->GetData(esiValidBlackList, &m_ValidBlackList, &size); //!!!
			}
		}

		if( m_pReport )
		{
			cBLReportSettings& sett = m_settings.m_reportSettings;

			tDWORD maxDataSize = 0;
			if( sett.m_logSize.m_on )
				maxDataSize = sett.m_logSize.m_val*1024*1024;

			tDWORD maxPeriod = 0;
			if( sett.m_logPeriod.m_on )
				maxPeriod = sett.m_logPeriod.m_val*24*60*60;

			m_pReport->SetDBParams(dbAllDatabases, maxDataSize, maxPeriod, maxDataSize/64, 24*60*60);
		}

		if( m_bFirstStartInSession && !m_strProductHotfixNew.empty() )
		{
			m_strProductHotfix = m_strProductHotfixNew;
			m_strProductHotfixNew.clear();

			saveOneField( m_config, "data", &m_strProductHotfix, 0 );
			saveOneField( m_config, "data", &m_strProductHotfixNew, 0 );
		}
		
		set_send_mail_schedule();

        if (strProductType != "sos")
        {
		    if( !bEnableSelfProtection )
			    check_status_changed(eNtfSelfDefense, &m_State.m_ProductState, nOldState, eProductNotProtected);
        }

		if( nOldState != m_State.m_ProductState )
			sysSendMsg(pmc_PRODUCT, pm_PRODUCT_STATE_CHANGED, NULL, NULL, NULL);
	}

	if( bChangedOasInstances )
	{
		cCfg cfg(0);
		cfg.m_dwInstances = -1;
		m_tm->SetProfileInfo(AVP_PROFILE_FILEMONITOR, &cfg, *this, 0);
	}

	if( bEnableSelfProtection != 2 )
	{
		tDWORD len = sizeof(bEnableSelfProtection);
		m_settings.saveOneField( m_config, "settings", &m_settings.m_bEnableSelfProtection, sizeof(tBOOL) );
		sysSendMsg(pmc_REMOTE_GLOBAL, pm_REMOTE_GLOBAL_SELFPROT_ENABLED, NULL, &bEnableSelfProtection, &len);
	}

	if( bAllowServiceStop != 2 )
	{
		tDWORD len = sizeof(bAllowServiceStop);
		sysSendMsg(pmc_REMOTE_GLOBAL, pm_REMOTE_GLOBAL_SERVICECONTROL_ENABLED, NULL, &bAllowServiceStop, &len);
	}

	if( bEnableCheckActivity != 2 )
	{
		tDWORD len = sizeof(bEnableCheckActivity);
		sysSendMsg(pmc_REMOTE_GLOBAL, pm_REMOTE_GLOBAL_SET_CHECK_ACTIVITY, NULL, &bEnableCheckActivity, &len);
	}

	if( dwAffinityMask && !m_bIsTSPInited )
	{
		tDWORD len = sizeof(dwAffinityMask);
		sysSendMsg(pmc_REMOTE_GLOBAL, pm_REMOTE_GLOBAL_SET_AFFINITYMASK, NULL, &dwAffinityMask, &len);
	}

	if (m_bIsSetSettingsCalledOnce)
	{
		if (bWriteUninstallPwd)
		{
			if (!sUninstallPwd.empty() && m_hCryptoHelper)
			{
				m_hCryptoHelper->CryptPassword(&cSerString(sUninstallPwd), PSWD_DECRYPT);
				UninstallPwd::SetUninstallPwd(sUninstallPwd);
			}
			else
				UninstallPwd::DeleteUninstallPwd();
		}

		if (bChangedReportLifetime)
			cleanup();
	}
	else
	{
		m_bIsSetSettingsCalledOnce = cTRUE;
	}

	return err;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, CustomizeSettings )
// Parameters are:
tERROR BlImpl::CustomizeSettings( const tCHAR* p_profile, cSerializable* p_settings, tDWORD p_level, tCustomizeSettingsDirection p_direction, tBOOL* p_make_persistent )
{
	cERROR err = errDO_IT_YOURSELF;
	PR_TRACE_FUNC_FRAME__( "ProductLogic::CustomizeSettings method", &err._ );
	
	if( !p_settings )
		return err;

	if( p_direction == csdFROM_GUI_TO_DATA && p_settings->isBasedOn(CHipsSettings::eIID) )
		return err = hips_customize_settings(p_profile, (CHipsSettings *)p_settings, p_direction, p_make_persistent) ;
	
	if( p_direction == csdFROM_DATA_TO_TASK && p_settings->isBasedOn(cAVSSettings::eIID) )
	{
		if( ((cAVSSettings *)p_settings)->m_nDetectMask != m_nDetectMask )
		{
			m_nDetectMask = ((cAVSSettings *)p_settings)->m_nDetectMask;
			update_security_as_state();
		}
	}
	
	if( p_direction == csdFROM_POLICY_TO_DATA )
	{
		if( p_settings->isBasedOn(cCfgEx::eIID) )
			return err = update_admin_policy_state((cCfgEx *)p_settings);
		return err = errPARAMETER_INVALID;
	}
	
	if( p_settings->isBasedOn(cCfg::eIID) )
	{
		cCfg* cfg = (cCfg*)p_settings;
		if( _cmp(p_profile, AVP_PROFILE_FILEMONITOR) )
		{
			if( cfg->m_dwInstances == -1 )
				m_tm->GetProfileInfo(AVP_PROFILE_FILEMONITOR, cfg);

			cfg->m_dwInstances = m_settings.m_nOasInstances;
		}

		return err = errOK;
	}

	if( p_settings->isBasedOn(cBLSettings::eIID) )
	{
		return err = bl_customize_settings((cBLSettings*)p_settings, p_direction);
	}
	
	if( p_direction == csdFROM_DATA_TO_TASK )
	{
		if( p_settings->isBasedOn(cProtectionSettings::eIID) )
		{
//			cProtectionSettings* pSett = (cProtectionSettings*)p_settings;
//			pSett->m_bPartlyDetect = (pSett->m_nEmulValue >= 70);
		}
		else if( p_settings->isBasedOn(cAntispamSettings::eIID) )
		{
			cAntispamSettings *pSett = (cAntispamSettings *)p_settings;
			if( !pSett->m_bCheckExternalPluginMessages && !pSett->m_bCheckTrafficMonitorMessages )
			{
				pSett->m_bCheckExternalPluginMessages = cTRUE;
				pSett->m_bCheckTrafficMonitorMessages = cTRUE;
				*p_make_persistent = cTRUE;
			}
		}
		else
		if( p_settings->isBasedOn(cMCSettings::eIID) )
		{
			cMCSettings *pSett = (cMCSettings*)p_settings;
			if( !pSett->m_bCheckExternalPluginMessages && !pSett->m_bCheckTrafficMonitorMessages )
			{
				pSett->m_bCheckExternalPluginMessages = cTRUE;
				pSett->m_bCheckTrafficMonitorMessages = cTRUE;
				*p_make_persistent = cTRUE;
			}
		}
	}
	else
	{
// 		if ( p_settings->isBasedOn(cAHFWSettings::eIID) )
// 			return err = fw_customize_settings((cAHFWSettings *)p_settings, p_direction);
		
// 		if ( p_settings->isBasedOn(cPDMSettings::eIID) )
// 			return err = pdm_customize_settings((cPDMSettings *)p_settings, p_direction);

// 		if ( p_settings->isBasedOn(CRegSettings::eIID) )
// 			return err = regguard2_customize_settings((CRegSettings*)p_settings, p_direction);
		
		if ( (p_direction == csdFROM_GUI_TO_DATA) && p_settings->isBasedOn(cUpdaterSettings::eIID) )
			return err = update_customize_settings(p_profile, p_settings, p_direction, p_make_persistent) ;
		
		if ( p_settings->isBasedOn(cTaskSchedule::eIID) )
			return err = check_schedule( p_profile, *(cTaskSchedule*)p_settings );
	}
	
	return err;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SelectSettings )
// Parameters are:
tERROR BlImpl::SelectSettings( const tCHAR* p_profile, const cSerializable* p_settings, const tCHAR* p_sub_profile, cSerializable* p_sub_settings )
{
	tERROR error = errDO_IT_YOURSELF;
	PR_TRACE_FUNC_FRAME( "ProductLogic::SelectSettings method" );
	
	// Place your code here
	
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR BlImpl::GetStatistics( const tCHAR* p_profile, cSerializable* p_statistics )
{
	tERROR error = errDO_IT_YOURSELF;
	PR_TRACE_FUNC_FRAME( "ProductLogic::GetStatistics method" );
	
	// Place your code here
	if( !p_statistics )
		return error;

	if( p_statistics->isBasedOn(cCheckInfo::eIID) )
	{
		cAutoCS cs(m_lock, true);
		return p_statistics->assign(m_LicInfo, true);
	}
	else
	if( p_statistics->isBasedOn(cBLStatistics::eIID) )
	{
		cAutoCS cs(m_lock, true);
		
		cBLStatistics* stat = (cBLStatistics*)p_statistics;
		error = stat->assign(*(cBLStatistics*)this, false);
		stat->make_state(stat->m_State.m_ProductState);
		stat->make_state(stat->m_State.m_ProtectionState);
		stat->make_state(stat->m_State.m_KeyState);
		stat->make_state(stat->m_State.m_UpdateState);
		stat->make_state(stat->m_State.m_ThreatsState, eAntiSpamNeedTraining);
		return errOK;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, CalcStatistics )
// Parameters are:
tERROR BlImpl::CalcStatistics( const tCHAR* p_profile_name, const cSerializable* p_preliminary_data, cSerializable* p_statistics )
{
	tERROR error = errDO_IT_YOURSELF;
	PR_TRACE_FUNC_FRAME( "ProductLogic::CalcStatistics method" );
	
	// Place your code here
	
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetService )
// Parameters are:
tERROR BlImpl::GetService( tTaskId p_task_id, hOBJECT p_client, tServiceId p_service_id, hOBJECT* p_service )
{
	tERROR error = errDO_IT_YOURSELF;
	PR_TRACE_FUNC_FRAME( "ProductLogic::GetService method" );
	
	if (_cmp(CRYPTOHELPER_SERVICE_NAME, p_service_id))
	{
		if( !m_hCryptoHelper )
			return errUNEXPECTED;

		*p_service = (hOBJECT)m_hCryptoHelper;
		return errOK;
	}
	
	if (_cmp(REPORTDB_SERVICE_NAME, p_service_id))
	{
		if( !m_pReport )
			return errUNEXPECTED;

		*p_service = (hOBJECT)m_pReport;
		return errOK;
	}

	if (_cmp(sid_TM_QUARANTINE, p_service_id))
	{
		if( PR_FAIL(init_qb()) )
			return errUNEXPECTED;

		*p_service = (hOBJECT)m_qb;
		return errOK;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ReleaseService )
// Parameters are:
tERROR BlImpl::ReleaseService( tTaskId p_task_id, hOBJECT p_service )
{
	tERROR error = errDO_IT_YOURSELF;
	PR_TRACE_FUNC_FRAME( "ProductLogic::ReleaseService method" );
	
	// Place your code here
	
	return error;
}
// AVP Prague stamp end



/*
tERROR BlImpl::ShowAlert(tActionId action_id, cSerializable* info)
{
	HWINSTA old = GetProcessWindowStation();
	HWINSTA sta = OpenWindowStation("WinSta0", TRUE, WINSTA_ALL_ACCESS);
	if( !old || !sta )
		return errDO_IT_YOURSELF;

	SetProcessWindowStation(sta);

	HDESK old_dsk = GetThreadDesktop(GetCurrentThreadId());
	HDESK act_dsk = OpenInputDesktop(0, TRUE, GENERIC_ALL);

	HANDLE hToken = NULL;
	OpenThreadToken(GetCurrentThread(), THREAD_ALL_ACCESS, TRUE, &hToken);
	RevertToSelf();

	HDESK new_dsk = OpenDesktop("Winlogon", 0, TRUE, GENERIC_ALL);

	SetThreadDesktop(new_dsk);
	SwitchDesktop(new_dsk);

	cAutoObj<cTask> gui;
	tERROR err = sysCreateObject(gui, IID_TASK, PID_GUI);

	if( PR_SUCC(err) )
	{
		CHAR sSourceName[MAX_PATH];
		lstrcpy(sSourceName, VER_PRODUCT_REGISTRY_PATH);
		if( PR_SUCC(err) )
			err = gui->propSetStr(0, plGUI_USER_SETTINGS_PATH, sSourceName);
	}

	if( PR_SUCC(err) )
		err = gui->sysCreateObjectDone();

	if( PR_SUCC(err) )
		err = gui->SetState(TASK_REQUEST_RUN);

	if( PR_SUCC(err) )
		err = gui->AskAction(action_id, info);

	gui->SetState(TASK_REQUEST_STOP);

	SwitchDesktop(act_dsk);
	SetThreadDesktop(old_dsk);
	SetProcessWindowStation(old);

	if( hToken )
		SetThreadToken(NULL, hToken);

	if( PR_FAIL(err) )
		return errDO_IT_YOURSELF;

	return err;
}
*/

// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR BlImpl::AskAction( const cSerializable* p_task_info, tActionId p_action_id, cSerializable* p_action_info )
{
	cERROR err = errDO_IT_YOURSELF;
	PR_TRACE_FUNC_FRAME__( "ProductLogic::AskAction method", &err._ );
	
	switch(p_action_id)
	{
	case cProfile::PREPROCESS_START_TASK:
		return check_task_run((cProfileBase*)p_task_info);

	case cProfile::PREPROCESS_RESTART_TASK:
		return m_settings.m_bEnableCheckActivity ? errOK : errNOT_OK;

	case cProfile::PREPROCESS_INIT_TASK:
		return process_init_task((cProfileBase*)p_task_info);
		
	case cProfile::PREPROCESS_SHEDULED_EVENT:
		return check_schedule_event(((cProfileBase*)p_task_info)->m_sName.c_str(cCP_ANSI), *(cTaskSchedule*)p_action_info);

	case cProfile::PREPROCESS_SLEEPING_MODE:
		if( !m_hProcMon )
			m_tm->GetService(TASKID_TM_ITSELF, *this, AVP_SERVICE_PROCESS_MONITOR, (cObject**)&m_hProcMon);

		return sysSendMsg(pmc_PROCESS_MONITOR, pm_PROCESS_MONITOR_CHECKINTASKLIST, NULL, p_action_info, SER_SENDMSG_PSIZE);

	case cAskObjectAction::ACTIVE_DISINFECT:
		return process_active_disifect((cAskObjectAction*)p_action_info);
	
	case cAskQBAction::QUARANTINE :
	case cAskObjectAction::QUARANTINE :
	case cAskQBAction::BACKUP :
	case cAskObjectAction::BACKUP :

		if( !p_action_info->isBasedOn(cInfectedObjectInfo::eIID) )
			err = errPARAMETER_INVALID;
		else
		{
			if( PR_SUCC(err = store_qb_object((cInfectedObjectInfo*)p_action_info)) )
				err = errOK_DECIDED;
		}
		return err;
	}

	if( p_action_info && p_action_info->isBasedOn(cAskObjectAction::eIID) )
	{
		enNotifySeverity nSeverity = eNotifySeverityNotImportant;
		
		cAskObjectAction* pInfo = (cAskObjectAction*)p_action_info;
		switch(pInfo->m_nDetectDanger)
		{
		case DETDANGER_HIGH:   nSeverity = eNotifySeverityCritical; break;
		case DETDANGER_MEDIUM: nSeverity = eNotifySeverityImportant; break;
		}
		
		cBLNotifySettings * pCfg = m_settings.m_NSettings.find(nSeverity, eNotifyThreats);
		if( pCfg )
			pInfo->m_bSound = pCfg->m_bSound && m_settings.m_bEnableNotifications;

//		if( pInfo->m_strTaskType == AVP_TASK_ONDEMANDSCANNER )
//			return ShowAlert(p_action_id, p_action_info);
	}

	return err;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, TestUpdaterConnection )
// Parameters are:
tERROR BlImpl::TestUpdaterConnection( const cSerializable* p_Settings )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "ProductLogic::TestUpdaterConnection method" );
	
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, FilterTaskMessage )
// Parameters are:
tERROR BlImpl::FilterTaskMessage( const cSerializable* p_task_info, const cSerializable* p_report_info, tDWORD p_msg_cls, tDWORD p_msg_id, cSerializable* p_params )
{
	cERROR err;
	PR_TRACE_FUNC_FRAME__( "ProductLogic::FilterTaskMessage method", &err._ );
	
	switch( p_msg_cls )	{
	case pmc_MATCH_EXCLUDES:
	case pmc_UPDATE_THREATS_LIST:
		return errNOT_OK;
	}
	
	process_notification(eNtfNone, p_params, 0, p_msg_cls, p_msg_id);
					return errNOT_OK;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR BlImpl::GetSettings( cSerializable* p_settings ) 
{
	tERROR error = errPARAMETER_INVALID;
	PR_TRACE_FUNC_FRAME( "ProductLogic::GetSettings method" );

	if (p_settings && p_settings->isBasedOn(cBLSettings::eIID))
	{
		cAutoCS cs(m_lock, true);
		error = p_settings->assign(m_settings, cTRUE);
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "ProductLogic". Register function
tERROR BlImpl::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(ProductLogic_PropTable)
	mpLOCAL_PROPERTY_BUF( pgPROFILE_CONFIG, BlImpl, m_config, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_BUF( pgTM_OBJECT, BlImpl, m_tm, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
	mpLOCAL_PROPERTY_BUF( pgBL_PRODUCT_NAME, BlImpl, m_settings, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgBL_PRODUCT_VERSION, BlImpl, m_settings, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgBL_PRODUCT_PATH, BlImpl, m_settings, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgBL_PRODUCT_DATA_PATH, BlImpl, m_settings, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgBL_COMPANY_NAME, BlImpl, m_settings, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgBL_PRODUCT_DATA_STORAGE, BlImpl, m_data, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT | cPROP_REQUIRED )
mpPROPERTY_TABLE_END(ProductLogic_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(ProductLogic)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(ObjectClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(ProductLogic)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(ProductLogic)
	mEXTERNAL_METHOD(ProductLogic, SetSettings)
	mEXTERNAL_METHOD(ProductLogic, CustomizeSettings)
	mEXTERNAL_METHOD(ProductLogic, SelectSettings)
	mEXTERNAL_METHOD(ProductLogic, GetStatistics)
	mEXTERNAL_METHOD(ProductLogic, CalcStatistics)
	mEXTERNAL_METHOD(ProductLogic, GetService)
	mEXTERNAL_METHOD(ProductLogic, ReleaseService)
	mEXTERNAL_METHOD(ProductLogic, AskAction)
	mEXTERNAL_METHOD(ProductLogic, TestUpdaterConnection)
	mEXTERNAL_METHOD(ProductLogic, FilterTaskMessage)
	mEXTERNAL_METHOD(ProductLogic, GetSettings)
mEXTERNAL_TABLE_END(ProductLogic)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "ProductLogic::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_PRODUCTLOGIC,                       // interface identifier
		PID_BL,                                 // plugin identifier
		0x00000000,                             // subtype identifier
		ProductLogic_VERSION,                   // interface version
		VID_PETROVITCH,                         // interface developer
		&i_ProductLogic_vtbl,                   // internal(kernel called) function table
		(sizeof(i_ProductLogic_vtbl)/sizeof(tPTR)),// internal function table size
		&e_ProductLogic_vtbl,                   // external function table
		(sizeof(e_ProductLogic_vtbl)/sizeof(tPTR)),// external function table size
		ProductLogic_PropTable,                 // property table
		mPROPERTY_TABLE_SIZE(ProductLogic_PropTable),// property table size
		sizeof(BlImpl)-sizeof(cObjImpl),        // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"ProductLogic(IID_PRODUCTLOGIC) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call ProductLogic_Register( hROOT root ) { return BlImpl::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgINTERFACE_VERSION
// You have to implement propetry: pgINTERFACE_COMMENT
// AVP Prague stamp end



