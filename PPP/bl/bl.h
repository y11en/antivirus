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



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __bl_cpp__1ec56157_1021_45a0_9669_37f82a67d4f7 )
#define __bl_cpp__1ec56157_1021_45a0_9669_37f82a67d4f7
// AVP Prague stamp end



#pragma warning(disable : 4786)

// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
#include <pr_vtbl.h>
#include <pr_serializable.h>
#include <iface/i_taskmanager.h>
#include <plugin/p_bl.h>
// AVP Prague stamp end

#include <pr_remote.h>

#include <plugin/p_cryptohelper.h>
#include <plugin/p_reportdb.h>
#include <plugin/p_qb.h>

#include <iface/i_threadpool.h>
#include <iface/i_timer.h>
#include <iface/i_event.h>
#include <iface/i_token.h>
#include <iface/i_csect.h>
#include <iface/i_datacodec.h>
#include <iface/i_netdetect.h>
#include <iface/i_hash.h>
#include <structs/s_bl.h>
#include <structs/s_licensing.h>

#include <../../Components/Updater/KeepUp2Date/Client/PragueWrapper/UpdaterConfigurator/i_updaterconfigurator.h>
#include <Updater/transport.h>
#include <Updater/updater.h>

#include <structs/s_antispam.h>
#include <structs/s_avp3info.h>
#include <structs/s_ids.h>
#include <structs/s_pdm2rt.h>
#include <structs/s_gui.h>
#include <structs/s_mc.h>
#include <structs/s_mc_trafficmonitor.h>

// Deleted by DVi - нет такого файла #include <NetDetails/plugin/p_netdetails.h>
// Deleted by DVi - нет такого файла #include <NetDetails/structs/s_netdetails.h>

#include <Hips/structs/s_hips.h>
#include <plugin/p_propertiesmap.h>

#include "blloc.h"
#include "ver_mod.h"
#include "ChartStatistics.h"

enum eReportTrayMode {fReportEvent = 0x01, fReportTray = 0x02};

// AVP Prague stamp begin( C++ class declaration,  )
struct BlImpl :
	public cProductLogic,
	public cThreadPoolBase,
	public cBLStatistics,
	public cBlLoc,
	public CPRRemotePrague
{
private:
	BlImpl(): cThreadPoolBase("BL") {}

// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call ObjectClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations

public:
// External function declarations
	tERROR pr_call SetSettings( const cSerializable* p_settings );
	tERROR pr_call CustomizeSettings( const tCHAR* p_profile, cSerializable* p_settings, tDWORD p_level, tCustomizeSettingsDirection p_direction, tBOOL* p_make_persistent );
	tERROR pr_call SelectSettings( const tCHAR* p_profile, const cSerializable* p_settings, const tCHAR* p_sub_profile, cSerializable* p_sub_settings );
	tERROR pr_call GetStatistics( const tCHAR* p_profile, cSerializable* p_statistics );
	tERROR pr_call CalcStatistics( const tCHAR* p_profile_name, const cSerializable* p_preliminary_data, cSerializable* p_statistics );
	tERROR pr_call GetService( tTaskId p_task_id, hOBJECT p_client, tServiceId p_service_id, hOBJECT* p_service );
	tERROR pr_call ReleaseService( tTaskId p_task_id, hOBJECT p_service );
	tERROR pr_call AskAction( const cSerializable* p_task_info, tActionId p_action_id, cSerializable* p_action_info );
	tERROR pr_call TestUpdaterConnection( const cSerializable* p_Settings );
	tERROR pr_call FilterTaskMessage( const cSerializable* p_task_info, const cSerializable* p_report_info, tDWORD p_msg_cls, tDWORD p_msg_id, cSerializable* p_params );
	tERROR pr_call GetSettings( cSerializable* p_settings );

// Data declaration
	cTaskManager* m_tm;       // --
	cRegistry*    m_config;   // --
	cBLSettings   m_settings; // --
	cRegistry*    m_data;     // --
// AVP Prague stamp end

	typedef std::map<std::wstring, tProfileState> tProfilesStates;

protected:
	friend class cBlLoc;
	friend class CChartStatistics;

	cCheckInfo  m_LicInfo;

	cLicensing* m_pLic;
	cAVS*       m_pAvs;
	cScheduler* m_scheduler;
	cReportDB*  m_pReport;
	cQB*        m_qb;
	
	tBOOL       m_bStartedUp;
	tBOOL       m_bAutoUpdateFailed;
	tBOOL       m_bWaitForUpdate;
	tBOOL       m_bFullScanStarted;
	time_t      m_tmValidBasesDate;

	struct ValidBlackList
	{
		tQWORD m_qwHash;
		tDWORD m_tmDate;
	}           m_ValidBlackList;
	tDWORD      m_tmNewBlackListDate;

	tBOOL       m_bIsKlopWorking;
	tBOOL       m_bIsKlopLimited;
	tDWORD      m_dwKlopWorkingStart;
	tDWORD      m_dwLastKlopInit;
	tBOOL       m_bFirstStartInSession;
	tBOOL       m_bIsCDInstallation;
	tBOOL       m_bIsProtectionStarted;
	
	cStringObj  m_strMailText;
	tBOOL       m_bMailShedule;
	
	cCryptoHelper*          m_hCryptoHelper;
	cProcessMonitor*        m_hProcMon;
	cAntispamStatistics     m_pASStat;
	cUpdaterProductSpecific	m_pUpdSett;
	
	cCriticalSection* m_lock;

    cNetDetect* m_NetDetect ;
    tDWORD      m_dwLastNetAddrListChange ;
    tERROR      m_errLastUpdateError ;

	tBOOL       m_bIsTSPInited;
	tBOOL       m_bIsAVActive;
	tBOOL       m_bIsStopping;

	tBOOL       m_bIsGuiConnected;
	tBOOL       m_bIsNeedUpdateAVState;
	tBOOL       m_bIsNeedUpdateASState;

	tProfilesStates m_mProfilesStates;
	tDWORD      m_nCpuNumber;

	tDWORD      m_dwEnabledPolicyBit;
	PrHMODULE   m_hFsSync;
	tDWORD		m_nDetectMask;
	tDWORD      m_dwLastNetStateChange;

public:
	CChartStatistics m_ChartStatistics;
	tBOOL       m_bIsSetSettingsCalledOnce;

protected:

	tERROR on_idle();
	
	tERROR pr_call startup();
	tERROR pr_call prepare_to_stop();
	tERROR pr_call updater();
	tERROR pr_call full_scan();
	tERROR pr_call cleanup();
	tERROR pr_call registerAllDlls();
	tERROR pr_call init_schedule(tDWORD sched_id, tDWORD period, tDWORD first_run);
    // checkComponentsToExcludeForUpdate [in] - check if component need not being updated
	tERROR pr_call init_components(cProfileEx& profile, cStrObj& strComponents);

	//  tERROR pr_call update_check_info_find_execute_category( cUpdateInfo* info, cUpdateCategory*& executables );
	//  tERROR pr_call update_check_info_get_file_localization( const cUpdateObjectInfo* info, cStrObj& localization );
	//  bool   pr_call update_check_info_is_my_file( const cUpdateObjectInfo* info, const tCHAR* fname, const tCHAR* localization );
	//  cUpdateObjectInfo* pr_call update_check_info_find_file( cUpdateCategory* category, const tCHAR* fname, const tCHAR* localization );
	tERROR pr_call update_product_status();
	tERROR pr_call update_licensing_state(bool bUpdateBlackList=false);
	tERROR pr_call update_klop_state(bool bReinit);
	tERROR pr_call update_avs_state(bool bForUpdate = false);
	tERROR pr_call update_as_state(bool bStopped = false);
	tERROR pr_call update_protection_state( cProfileBase* tp, bool bSafeModeh );
	tERROR pr_call update_profile_state(cProfileEx& pInfo, tDWORD level);
	tERROR pr_call update_profile_state(cProfileBase* tp, tDWORD nOldUpdateState);
	tERROR pr_call update_security_av_state();
	tERROR pr_call update_security_as_state();
	tERROR pr_call update_security_state(cProfileBase* tp);
	tERROR pr_call update_task_reboot_state();
	tERROR pr_call update_admin_policy_state(cCfgEx *pPolicy);
	tERROR pr_call updateUpdaterStatus(cUpdaterConfigurator *);
	tERROR pr_call update_updater_rollback(tBOOL bAllowed);
	tERROR pr_call update_customize_settings(const tCHAR* p_profile, cSerializable* p_settings, tCustomizeSettingsDirection p_direction, tBOOL *p_persistent);
	tERROR pr_call hips_customize_settings(const tCHAR* p_profile, CHipsSettings* p_settings, tCustomizeSettingsDirection p_direction, tBOOL *p_persistent);
	tERROR pr_call update_run_tasks_on_update_avs_bases();
	tERROR pr_call updateRegGuardSettings(cStrObj& strRegIniFilePath, cSerializable* pSett);
    tERROR pr_call updateAutomaticScheduleForUpdater(cUpdaterConfigurator *);
    tERROR pr_call update_fixname(const char *fixNameDelimitedString);
	tERROR pr_call task_state_changed( cProfileBase& tp );
	
// 	tERROR pr_call pdm_customize_settings(cPDMSettings *pSett, tCustomizeSettingsDirection eDirection);
// 	tERROR pr_call pdm_calc_hashes(cPDMSettings *pSett);
// 	tERROR pr_call pdm_update_hash(cPdmAppMonitoring_Data &moduleInfo, cProcessMonitor *pPM);
// 	tERROR pr_call pdm_update_hashes(cUpdaterUpdatedFileList *pList);
	
//	tERROR pr_call regguard2_customize_settings(CRegSettings*pSett, tCustomizeSettingsDirection eDirection);
	
// 	tERROR pr_call fw_customize_settings(cAHFWSettings *pSett, tCustomizeSettingsDirection eDirection);
// 	tERROR pr_call fw_calc_hashes(cAHFWSettings *pSett);
// 	tERROR pr_call fw_update_networks(cFwNets *pNetworks);
//	tERROR pr_call fw_add_network(cFwChangeNet *pChangeNet);
//	tERROR pr_call fw_delete_network(cFwChangeNet *pChangeNet);
//	tERROR pr_call fw_change_network(cFwChangeNet *pChangeNet);

    tERROR pr_call ckahum_reload(tBOOL bI386Updated, tBOOL bX64Updated);

	tERROR pr_call bl_customize_settings(cBLSettings *pSett, tCustomizeSettingsDirection eDirection);
	
	tERROR pr_call ensure_avs_tasks_running();
	
	tERROR pr_call start_ods_tasks();
	
	tERROR pr_call check_schedule( const tCHAR* p_profile, cTaskSchedule& schedule );
	tERROR pr_call check_schedule_event( const tCHAR* p_profile, cTaskSchedule& schedule );
	tERROR pr_call adjustUpdaterConfiguration(cUpdaterConfigurator *);
	tERROR pr_call checkReceivedComponentUpdate(cUpdaterUpdatedFileList *);
	tERROR pr_call processReceivedComponentUpdate(cUpdatedComponentName *);
	tERROR pr_call processReceivedUpdate(cUpdaterConfigurator *);

	tERROR pr_call check_my_computer_was_scanned(cProfileBase* tp);
	tERROR pr_call check_nagent_installed();
	tERROR pr_call remove_policy();

	tERROR pr_call update_avs_bases_info( cAVP3BasesInfo* info, bool bByUpdate );
	tERROR pr_call check_bases_toload( cAVP3BasesInfo* info, bool bByUpdate );
	tERROR pr_call check_blacklist(cDateTime* dtUpdateDate, bool bUpdateState = false);
	
	tERROR pr_call process_init_task(cProfileBase* p_task_info);
	tERROR pr_call check_task_run(cProfileBase* p_profile);
	
	tERROR pr_call process_active_disifect(cAskObjectAction* pInfo);

	tERROR pr_call run_qb_check();
	tERROR pr_call init_qb();
	tERROR pr_call store_qb_object(cInfectedObjectInfo* info);
	tERROR pr_call clean_QB(const tDATETIME* p_time_stamp);
	
	tERROR pr_call gui_connected();
	tERROR pr_call check_status(tQWORD nStatusMask);
	
	void process_notification(enNotification nNotification, cSerializable * pInfo, tDWORD nStatusId = 0, tDWORD nMsgCls = 0, tDWORD nMsgId = 0, tDWORD nTrayMode = fReportEvent);
	void check_status_changed(enNotification nNotification, tDWORD* pStatePtr, tDWORD nOldState, tDWORD nState, bool bTrayForce = false);
	
	void push_mail_notification(cBLNotification& pNotification);
	void send_mail_notification();
	void set_send_mail_schedule();
	
    void restartComponent(const char *profileName);
	bool upgrade_settings(cProfileEx& protection);

	bool update_hips_settings();
	bool merge_hips_settings(CHipsSettings * pCurSettings, CHipsSettings * pNewSettings);

	void ExpandEnvironmentString(LPCSTR strName, cStringObj& strValue);
	bool ExpandEnvironmentString(cStringObj& strName);

//	tERROR ShowAlert(tActionId action_id, cSerializable* info);

	// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION_NO_DELETE(BlImpl)
};
// AVP Prague stamp end



//mDECLARE_INITIALIZATION_NO_DELETE(BlImpl)


// AVP Prague stamp begin( Header endif,  )
#endif // bl_cpp
// AVP Prague stamp end



