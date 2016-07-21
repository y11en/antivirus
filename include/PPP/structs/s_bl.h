#ifndef __S_BL_H
#define __S_BL_H

#include <Prague/pr_time.h>

#include <AV/structs/s_scaner.h>

#include <ProductCore/structs/s_report.h>
#include <ProductCore/structs/s_procmon.h>
#include <ProductCore/structs/s_scheduler.h>
#include <ProductCore/structs/s_taskmanager.h>

#include <PPP/plugin/p_bl.h>
#include <PPP/structs/s_licensing.h>


//+ ------------------------------------------------------------------------------------------
enum _eBL_IID
{
	blst_cBLSettings		=	0x001,
	blst_cBLStatistics		=	0x002,
	blst_schedp		=	0x003,
	blst_ah_set		=	0x080,
	blst_ah_stat	=	0x081,
	blst_bb_set	    =	0x082,
	blst_bb_stat    =	0x083,
	blst_updateexecutables,
	blst_cBLUpdateWhatsNew,
	blst_cBLNotifySettings,
    blst_cBLAppInfo,
    blst_cBLNotification,
	blst_cBLLocalizedNotification,
	blst_cUpdaterProductSpecific,
	blst_cScanerProductSpecific,
	blst_cProtectionNotify,
	blst_cBLSupportCustomLink,
	blst_cBLSupportCustomization,
	gst_cSvrMgrRequest,
	blst_cBLFSSyncRequest,
	blst_cBLState,
	blst_cBLReportSettings,
};

//-----------------------------------------------------------------------------

enum enNotifySeverity
{
	eNotifySeverityNone               = 0,
	eNotifySeverityCritical           = 1,
	eNotifySeverityError              = 2,
	eNotifySeverityImportant          = 3,
	eNotifySeverityNotImportant       = 4,
};

enum enNotifyIds
{
	eNotifyNone                       = 0,
	eNotifyThreats                    = 1,
	eNotifySuspicious                 = 2,
	eNotifyThreatNotCured             = 3,
	eNotifyThreatDeleted              = 4,
	eNotifyThreatQuarantined          = 5,
	eNotifyLicensing                  = 6,
	eNotifyProduct                    = 7,
	eNotifyUpdater                    = 8,
	eNotifyBases                      = 9,
	eNotifyAttackBlocked              = 10,
	eNotifyPswdArchive                = 11,
	eNotifyRuleTriggered              = 12,
	eNotifyPopups                     = 13,
	eNotifyProtocoller                = 14,
	eNotifySelfProtection             = 15,
	eNotifyBan                        = 16,
	eNotifySSL                        = 17,
	eNotifyThreatsUntreated           = 18,
	eNotifyMyComputerNotScaned        = 19,
	eNotifyUpdateNeedReboot           = 20,
	eNotifyParCtl                     = 21,
};

#define MAKE_NOTIFICATION(sev, id) ((sev << 16) + id)
#define NOTIFICATION_SERVERITY(ntf) (enNotifySeverity(((ntf) & 0xFFFF0000) >> 16))
#define NOTIFICATION_ID(ntf) (enNotifyIds((ntf) & 0xFFFF))

enum enNotification
{
	eNtfNone					= MAKE_NOTIFICATION(eNotifySeverityNone,         eNotifyNone),
	eNtfNotImportant			= MAKE_NOTIFICATION(eNotifySeverityNotImportant, eNotifyNone),
	eNtfImportant				= MAKE_NOTIFICATION(eNotifySeverityImportant,    eNotifyNone),
	eNtfError					= MAKE_NOTIFICATION(eNotifySeverityError,        eNotifyNone),
	eNtfCritical				= MAKE_NOTIFICATION(eNotifySeverityCritical,     eNotifyNone),

	// Типы сообытий, которые есть в ГУЕ:
	eNtfInfected				= MAKE_NOTIFICATION(eNotifySeverityCritical, eNotifyThreats),
	eNtfSuspicious				= MAKE_NOTIFICATION(eNotifySeverityCritical, eNotifySuspicious),
	eNtfDisinfectionImpossible	= MAKE_NOTIFICATION(eNotifySeverityCritical, eNotifyThreatNotCured),
	eNtfLicenseExpired			= MAKE_NOTIFICATION(eNotifySeverityCritical, eNotifyLicensing),
	eNtfAttack					= MAKE_NOTIFICATION(eNotifySeverityCritical, eNotifyAttackBlocked),
	eNtfAVBasesObsolete			= MAKE_NOTIFICATION(eNotifySeverityCritical, eNotifyBases),
	eNtfParCtlCritical			= MAKE_NOTIFICATION(eNotifySeverityCritical, eNotifyParCtl),

	eNtfLicenseProblems			= MAKE_NOTIFICATION(eNotifySeverityError, eNotifyLicensing),
	eNtfUpdateError				= MAKE_NOTIFICATION(eNotifySeverityError, eNotifyUpdater),
	eNtfTaskCanNotExecute		= MAKE_NOTIFICATION(eNotifySeverityError, eNotifyProduct),
	eNtfDatabaseCorrupted		= MAKE_NOTIFICATION(eNotifySeverityError, eNotifyBases),

	eNtfRiskwareDetected		= MAKE_NOTIFICATION(eNotifySeverityImportant, eNotifyThreats),
	eNtfLicenseExpiresSoon		= MAKE_NOTIFICATION(eNotifySeverityImportant, eNotifyLicensing),
	eNtfOtherImportantEvents	= MAKE_NOTIFICATION(eNotifySeverityImportant, eNotifyProduct),
	eNtfAVBasesOutOfDate		= MAKE_NOTIFICATION(eNotifySeverityImportant, eNotifyBases),
	eNtfSelfDefense				= MAKE_NOTIFICATION(eNotifySeverityImportant, eNotifySelfProtection),
	eNtfBan						= MAKE_NOTIFICATION(eNotifySeverityImportant, eNotifyBan),// fs only
	eNtfThreatsUntreated        = MAKE_NOTIFICATION(eNotifySeverityImportant, eNotifyThreatsUntreated),
	eNtfMyComputerNotScaned     = MAKE_NOTIFICATION(eNotifySeverityImportant, eNotifyMyComputerNotScaned),
	eNtfUpdateNeedReboot        = MAKE_NOTIFICATION(eNotifySeverityImportant, eNotifyUpdateNeedReboot),
	eNtfParCtlImportant			= MAKE_NOTIFICATION(eNotifySeverityImportant, eNotifyParCtl),
	eNtfRTPTaskOnOff			= MAKE_NOTIFICATION(eNotifySeverityImportant, eNotifyProduct),
	
	eNtfInfectedDisinfected		= MAKE_NOTIFICATION(eNotifySeverityNotImportant, eNotifyThreats),
	eNtfInfectedDeleted			= MAKE_NOTIFICATION(eNotifySeverityNotImportant, eNotifyThreatDeleted),
	eNtfQuarantined				= MAKE_NOTIFICATION(eNotifySeverityNotImportant, eNotifyThreatQuarantined),
	eNtfPasswordProtected		= MAKE_NOTIFICATION(eNotifySeverityNotImportant, eNotifyPswdArchive),
	eNtfUpdateComplete			= MAKE_NOTIFICATION(eNotifySeverityNotImportant, eNotifyUpdater),
	eNtfFirewallRule			= MAKE_NOTIFICATION(eNotifySeverityNotImportant, eNotifyRuleTriggered),
	eNtfPopupBlocked			= MAKE_NOTIFICATION(eNotifySeverityNotImportant, eNotifyPopups),
	eNtfProtocoller				= MAKE_NOTIFICATION(eNotifySeverityNotImportant, eNotifyProtocoller),
};
#undef MAKE_NOTIFICATION


//-----------------------------------------------------------------------------

struct cBLNotification : public cReportEvent
{
	cBLNotification() :
		m_bSound(cFALSE)
	{}

	DECLARE_IID( cBLNotification, cReportEvent, PID_BL, blst_cBLNotification );

	tDWORD statusId() { return (m_Verdict >= ePROTECTION && m_Verdict <= (ePROTECTION + 64)) ? m_Verdict - ePROTECTION : 0; }

	tBOOL            m_bSound;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cBLNotification, 0 )
	SER_BASE( cReportEvent, 0 )
	SER_VALUE( m_bSound,	tid_BOOL,   "CheckSound" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cBLLocalizedNotification : public cBLNotification
{
	DECLARE_IID( cBLLocalizedNotification, cBLNotification, PID_BL, blst_cBLLocalizedNotification );

	cStringObj       m_sEventText;
	cStringObj       m_sEventType;
	cStringObj       m_sTaskName;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cBLLocalizedNotification, 0 )
	SER_BASE( cBLNotification, 0 )
	SER_VALUE( m_sEventText, tid_STRING_OBJ, "LocEventText" )
	SER_VALUE( m_sEventType, tid_STRING_OBJ, "LocEventType" )
	SER_VALUE( m_sTaskName,  tid_STRING_OBJ, "LocTaskName" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cBLSupportCustomLink : public cSerializable
{
	DECLARE_IID( cBLSupportCustomLink, cSerializable, PID_BL, blst_cBLSupportCustomLink );

	cStrObj         m_strName;
	cStrObj         m_strURL;
	cStrObj         m_strDescription;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cBLSupportCustomLink, 0 )
	SER_VALUE_M( strName,        tid_STRING_OBJ )
	SER_VALUE_M( strURL,         tid_STRING_OBJ )
	SER_VALUE_M( strDescription, tid_STRING_OBJ )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cBLSupportCustomization : public cPolicySettings
{
	DECLARE_IID( cBLSupportCustomization, cPolicySettings, PID_BL, blst_cBLSupportCustomization );

	cStrObj                       m_strDescription;
	cVector<cBLSupportCustomLink> m_aLinks;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cBLSupportCustomization, 0 )
	SER_BASE( cPolicySettings, 0 )
	SER_VALUE_M( strDescription, tid_STRING_OBJ )
	SER_VECTOR_M( aLinks,	     cBLSupportCustomLink::eIID )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cBLReportSettings : public cPolicySettings
{
	cBLReportSettings() : m_logSize(5), m_logPeriod(5), m_statSize(5), m_statPeriod(5){}

	DECLARE_IID( cBLReportSettings, cPolicySettings, PID_BL, blst_cBLReportSettings );

	cSettingsCheck  m_logSize;
	cSettingsCheck  m_logPeriod;

	cSettingsCheck  m_statSize;
	cSettingsCheck  m_statPeriod;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cBLReportSettings, 0 )
	SER_BASE( cPolicySettings, 0 )

	SER_VALUE( m_logSize.m_on,         tid_BOOL,       "logSizeOn" )
	SER_VALUE( m_logSize.m_val,        tid_DWORD,      "logSizeVal" )

	SER_VALUE( m_logPeriod.m_on,       tid_BOOL,       "logPeriodOn" )
	SER_VALUE( m_logPeriod.m_val,      tid_DWORD,      "logPeriodVal" )

	SER_VALUE( m_statSize.m_on,        tid_BOOL,       "statSizeOn" )
	SER_VALUE( m_statSize.m_val,       tid_DWORD,      "statSizeVal" )

	SER_VALUE( m_statPeriod.m_on,      tid_BOOL,       "statPeriodOn" )
	SER_VALUE( m_statPeriod.m_val,     tid_DWORD,      "statPeriodVal" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cBLNotifySettings : public cSerializable
{
	enum Triggers
	{
		fTray   = 0x01,
		fMail   = 0x02,
		fSound  = 0x04,
		fELog   = 0x08,
	};
	
	cBLNotifySettings() :
		m_EventID(eNotifyNone),
		m_Severity(eNotifySeverityNone),
		m_bTray(cTRUE),
		m_bSound(cTRUE),
		m_bMail(cFALSE),
		m_bELog(cFALSE)
	{}
	
	DECLARE_IID( cBLNotifySettings, cSerializable, PID_BL, blst_cBLNotifySettings );
		
	cBLNotifySettings * find(enNotifySeverity nSeverity, enNotifyIds nEventId)
	{
		if( m_Severity != nSeverity && m_Severity != eNotifyNone )
			return NULL;
		
		if( m_Severity != eNotifyNone && m_EventID == nEventId )
			return this;
		
		for(tDWORD i = 0; i < m_vChilds.size(); i++)
			if( cBLNotifySettings * cfg = m_vChilds[i].find(nSeverity, nEventId) )
				return cfg;
			
		return NULL;
	}

	void enable(tDWORD nFlags, bool bEnable = true)
	{
		if( nFlags & fTray )  m_bTray  = !!bEnable;
		if( nFlags & fMail )  m_bMail  = !!bEnable;
		if( nFlags & fSound ) m_bSound = !!bEnable;
		if( nFlags & fELog )  m_bELog  = !!bEnable;
		
		for(tDWORD i = 0; i < m_vChilds.size(); i++)
			m_vChilds[i].enable(nFlags, bEnable);
	}

	enNotifyIds      m_EventID;	
	enNotifySeverity m_Severity;
	tBOOL            m_bTray;
	tBOOL            m_bMail;
	tBOOL            m_bSound;
	tBOOL            m_bELog;
	cVector<cBLNotifySettings>  m_vChilds;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cBLNotifySettings, 0 )
	SER_VALUE( m_EventID,	tid_DWORD, "EventID" )
	SER_VALUE( m_Severity,	tid_DWORD, "Severity" )
	SER_VALUE( m_bTray,		tid_BOOL,  "CheckTray" )
	SER_VALUE( m_bMail,		tid_BOOL,  "CheckMail" )
	SER_VALUE( m_bSound,	tid_BOOL,  "CheckSound" )
	SER_VALUE( m_bELog,	    tid_BOOL,  "CheckELog" )
	SER_VECTOR( m_vChilds,	cBLNotifySettings::eIID, "Childs" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// cBLSettings

#define BL_INITMODE_INIT				0x01
#define BL_INITMODE_SILENT				0x02
#define BL_INITMODE_LOAD_SETTINGS		0x04
#define BL_INITMODE_NOREBOOT			0x08
#define BL_INITMODE_NEEDREBOOT			0x10
#define BL_INITMODE_EXPESS				0x20
#define BL_INITMODE_UPGRADE				0x40
#define BL_INITMODE_USE_REG_LIC			0x80

enum tComponentFlags {
	iciScan            = 0x00000001, //- устанавливать компонент Scan
	iciFileMonitor     = 0x00000002, //- устанавливать компонент File Monitor
	iciMailMonitor     = 0x00000004, //- устанавливать компонент Mail Monitor
	iciWebMonitor      = 0x00000008, //- устанавливать компонент Web Monitor
	iciPdm             = 0x00000010, //- устанавливать компонент Pdm
	iciAntiSpy         = 0x00000020, //- устанавливать компонент Anti Spy
	iciAntiHacker      = 0x00000040, //- устанавливать компонент Anti Hacker
	iciAntiSpam        = 0x00000080, //- устанавливать компонент Anti Spam
	iciScanMyComputer  = 0x00000100, //- устанавливать задачу Scan My Computer
	iciScanStartup     = 0x00000200, //- устанавливать задачу Scan Startup
	iciScanCritical    = 0x00000400, //- устанавливать задачу Scan Critical Areas
	iciUpdater         = 0x00000800, //- устанавливать задачу Updater
	iciEnvironment     = 0x00001000, //- добавить путь к avp.com в переменную среды окружения %Path%
	iciMsExclusions    = 0x00002000, //- какая-то хрень для Fs от MS
	iciSelfProtection  = 0x00004000, //- включать защиту до установки
	iciRetranslation   = 0x00008000,
	iciParentalControl = 0x00010000,
};

// ---
struct cBLSettings : public cTaskSettings {
	cBLSettings() :
		m_bRunAtSystemStartup(cTRUE),
		m_bEnableAlerts(cFALSE),
		m_bEnableSelfProtection(cTRUE),
		m_bEnableCheckActivity(cTRUE),
		m_bAllowServiceStop(cFALSE),
		m_bEnableInfoEvents(cTRUE),
		m_bSystemEventLog(cTRUE),
		m_bNotifyNewNetwork(cTRUE),
		m_bQuarantineStartupScan(cFALSE),
		m_BatterySafe(100),
		m_ReportSpan(30),
		m_QBSpan(30),
		m_strTechEmail(), 
		m_sDataPath(),
		m_Ins_InitMode(0),
		m_Ins_Update(0),
		m_Ins_FullScan(0),
		m_Ins_CompanyName(),
		m_Ins_DisplayName(),
		m_Ins_DefaultKey(),
		m_Ins_Date(0),
		m_bFullReport(cTRUE),
		m_bKeepRecentReports(cFALSE),
		m_nKeyAboutExpirationPeriod(7),
		m_nKeyWillBeExpiredPeriod(30),
		m_nBasesNotActualPeriod(3),
		m_nBasesOutOfDatePeriod(30),
		// Service
		m_bEnableNotifications(cTRUE),
		m_bEnableLoginShow(cTRUE),
		m_bEnablePswrdProtect(cFALSE),
		m_bEnableInteraction(cTRUE),
		m_bAskPswrdAlwaysExeptsAlerts(cTRUE),
		m_bAskPswrdOnSaveSetting(cFALSE),
		m_bAskPswrdOnExit(cFALSE),
		m_bAskPswrdOnStop(cFALSE),
		// Service [EMail_Settings]
		m_MailFrom(),
		m_SMTPServer(),
		m_SMTPPort(25),
		m_Account(),
		m_MailTo(),
		m_nSendMode(0),
		m_mail_schedule(),
		m_MailPassword(),
		m_PrtctPassword(),
		m_nCpuNumber(1),
		m_nAffinityMask(-1),
		m_nOasInstances(1),
		m_bUseUninstallPwd(cFALSE),
		m_UninstallPwd(),
		m_SettingsVersion(L"6.0.0.0"),
		m_bUseVirtualKdb(cFALSE),
		m_MinShowDumpVer((8 << 16) + 30 ) // 8._._.30
	{
		::memset(&m_LastShownDumpsTime, 0, sizeof(m_LastShownDumpsTime)); 
		::memset(&m_LastShownTracesTime, 0, sizeof(m_LastShownTracesTime)); 
	}

	DECLARE_IID( cBLSettings, cTaskSettings, PID_BL, blst_cBLSettings );

	tBOOL           m_bRunAtSystemStartup;// 
	tBOOL           m_bEnableAlerts;      // is application allowed to Ask-Action
	tBOOL           m_bEnableSelfProtection;// enable Self-Protection
	tBOOL           m_bEnableCheckActivity;// enable Self-Protection
	tBOOL           m_bAllowServiceStop;
	tBOOL           m_bSystemEventLog;

	tBOOL           m_bEnableInfoEvents;  // Info events logging
	tBOOL           m_bNotifyNewNetwork;

	tBOOL           m_bQuarantineStartupScan;    // scan quarantine at startup

	cSettingsCheck  m_BatterySafe;        // don't start scheduled tasks if battery charge
	                                      // less than m_nBatterySafeLimit percents
	cSettingsCheck  m_ReportSpan;         // delete old reports if switched on
	cSettingsCheck  m_QBSpan;             // delete old backups and quarantined if switched on

	cStrObj         m_strTechEmail;       // technical e-mail address
	cStrObj         m_sDataPath;          // product data path
	cStrObj         m_sBasePath;          // base data path

	cStrObj         m_Ins_ProductPath;    // product path
	tDWORD          m_Ins_InitMode;       // init mode (PIW, Wizard, Silent init)
	tBOOL           m_Ins_Update;         // need update on run
	tBOOL           m_Ins_FullScan;       // need full rescan on run
	cStrObj         m_Ins_CompanyName;    // company name
	cStrObj         m_Ins_DisplayName;    // display name
	cStrObj         m_Ins_DefaultKey;     // default key
	cStrObj         m_Ins_UserEMail;	    // 
	cStrObj         m_Ins_UserInfoName;	  // 
	cStrObj         m_Ins_UserInfoOrg;	  // 
	tDWORD          m_Ins_Date;

	tBOOL           m_bFullReport;        // detailed report if TRUE
	tBOOL           m_bKeepRecentReports; //

	tDWORD          m_nKeyAboutExpirationPeriod;
	tDWORD          m_nKeyWillBeExpiredPeriod;
	tDWORD          m_nBasesNotActualPeriod;
	tDWORD          m_nBasesOutOfDatePeriod;
	tDWORD          m_nRegisterDllPeriod;

	/////// Service ////////////////////////////////////////////
	tBOOL           m_bEnableNotifications;
	tBOOL           m_bEnableLoginShow;
	tBOOL           m_bEnablePswrdProtect;
	tBOOL           m_bEnableInteraction;

	// Support costomization
	cBLSupportCustomization m_SupportCustom;

	// password asking cases
	tBOOL           m_bAskPswrdAlwaysExeptsAlerts;
	tBOOL           m_bAskPswrdOnSaveSetting;
	tBOOL           m_bAskPswrdOnExit;
	tBOOL           m_bAskPswrdOnStop;
	// Service [EMail_Settings]
	cStrObj         m_MailFrom;  
	cStrObj         m_SMTPServer;  
	tDWORD          m_SMTPPort;
	cStrObj         m_Account;
	cStrObj         m_MailTo;
	tDWORD          m_nSendMode;
	cTaskSchedule   m_mail_schedule;
	// temporary Passwords realization
	cStrObj         m_MailPassword; // temporary used
	cStrObj         m_PrtctPassword;
	// Service [Event_Settings]
	cBLNotifySettings  m_NSettings; // event notifications
	
	cStrObj         m_LicBuyLink;
	cStrObj         m_LicRenewLink;

	union
	{
		tQWORD      m_nAffinity;
		struct
		{
			tDWORD  m_nCpuNumber;
			tDWORD  m_nAffinityMask;
		};
	};
	tDWORD          m_nOasInstances;

	cStrObj         m_LicActivationLink;
	cStrObj         m_TrialActCode;
	cProxySettings  m_ProxySettings;

	tBOOL           m_bUseUninstallPwd;
	cStrObj         m_UninstallPwd;

	cStrObj         m_SettingsVersion;
	tDATETIME       m_LastShownDumpsTime;
	tDATETIME       m_LastShownTracesTime;
	tDWORD          m_MinShowDumpVer;
	cStrObj         m_strCustomerId;
	cStrObj         m_strCustomerPwd;

	tBOOL           m_bUseVirtualKdb;

	cBLReportSettings m_reportSettings;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cBLSettings, 0 )
	SER_BASE( cTaskSettings, 0 )

//	SER_VALUE( m_bEnableSounds,           tid_BOOL,       "EnableSounds" )
	SER_VALUE( m_bRunAtSystemStartup,     tid_BOOL,       "RunAtSystemStartup" )
	SER_VALUE( m_bEnableAlerts,           tid_BOOL,       "EnableAlerts" )
	SER_VALUE( m_bEnableSelfProtection,   tid_BOOL,       "EnableSelfProtection" )
	SER_VALUE( m_bEnableCheckActivity,    tid_BOOL,       "EnableCheckActivity" )
	SER_VALUE( m_bAllowServiceStop,       tid_BOOL,       "AllowServiceStop" )
	SER_VALUE( m_bSystemEventLog,         tid_BOOL,       "SystemEventLog" )

	SER_VALUE( m_bEnableInfoEvents,       tid_BOOL,       "EnableInfoEvents" )
	SER_VALUE( m_bNotifyNewNetwork,       tid_UINT,       "NotifyNewNetwork" )

	SER_VALUE( m_bQuarantineStartupScan,  tid_BOOL,       "QuarantineStartupScan" )

	SER_VALUE( m_BatterySafe.m_on,        tid_BOOL,       "BatterySafe" )
	SER_VALUE( m_BatterySafe.m_val,       tid_DWORD,      "BatterySafeLimit" )

	SER_VALUE( m_ReportSpan.m_on,         tid_BOOL,       "ReportSpan" )
	SER_VALUE( m_ReportSpan.m_val,        tid_DWORD,      "ReportSpanPeriod" )
	
	SER_VALUE( m_QBSpan.m_on,             tid_BOOL,       "QBSpan" )
	SER_VALUE( m_QBSpan.m_val,            tid_DWORD,      "QBSpanPeriod" )
	
	SER_VALUE( m_strTechEmail,            tid_STRING_OBJ, "TechEmail" )

	SER_VALUE_M( Ins_ProductPath,         tid_STRING_OBJ )
	SER_VALUE_M( Ins_InitMode,            tid_DWORD )
	SER_VALUE_M( Ins_Update,              tid_BOOL )
	SER_VALUE_M( Ins_FullScan,            tid_BOOL )
	SER_VALUE_M( Ins_CompanyName,         tid_STRING_OBJ )
	SER_VALUE_M( Ins_DisplayName,         tid_STRING_OBJ )
	SER_VALUE_M( Ins_DefaultKey,          tid_STRING_OBJ )
	SER_VALUE_M( Ins_UserEMail,           tid_STRING_OBJ )
	SER_VALUE_M( Ins_UserInfoName,        tid_STRING_OBJ )
	SER_VALUE_M( Ins_UserInfoOrg,         tid_STRING_OBJ )
	SER_VALUE_M( Ins_Date,                tid_DWORD )

	SER_VALUE( m_bFullReport,               tid_BOOL,     "FullReport" )
	SER_VALUE( m_bKeepRecentReports,        tid_BOOL,     "KeepRecentReports" )
	SER_VALUE( m_nKeyAboutExpirationPeriod, tid_DWORD,    "KeyAboutExpirationPeriod" )
	SER_VALUE( m_nKeyWillBeExpiredPeriod,   tid_DWORD,    "KeyWillBeExpiredPeriod" )
	SER_VALUE( m_nBasesNotActualPeriod,     tid_DWORD,    "BasesNotActualPeriod" )
	SER_VALUE( m_nBasesOutOfDatePeriod,     tid_DWORD,    "BasesOutOfDatePeriod" )
	SER_VALUE( m_nRegisterDllPeriod,        tid_DWORD,    "RegisterDllPeriod" )
	//----- Service ----------------------------------------------------------
	SER_VALUE( m_bEnableNotifications,		tid_BOOL,	"EnableNotifications" )
	SER_VALUE( m_bEnableLoginShow,	        tid_BOOL,	"EnableLoginShow" )
	SER_VALUE( m_bEnablePswrdProtect,		tid_BOOL,	"EnablePswrdProtect" )
	SER_VALUE( m_bEnableInteraction,		tid_BOOL,	"EnableInteraction" )

	SER_VALUE( m_SupportCustom, cBLSupportCustomization::eIID, "SupportCustom" )

	// password asking cases
	SER_VALUE( m_bAskPswrdAlwaysExeptsAlerts, tid_BOOL,"AskPswrdAlwaysExeptsAlerts" )
	SER_VALUE( m_bAskPswrdOnSaveSetting,	tid_BOOL,	"AskWhenSaveSetting" )
	SER_VALUE( m_bAskPswrdOnExit,			tid_BOOL,	"AskWhenExitApp" )
	SER_VALUE( m_bAskPswrdOnStop,			tid_BOOL,	"AskWhenStopProtection" )
	// Service [EMail_Settings]
	SER_VALUE( m_MailFrom,		tid_STRING_OBJ,	"MailFrom" )
	SER_VALUE( m_SMTPServer,	tid_STRING_OBJ,	"SMTPServer" )
	SER_VALUE( m_SMTPPort,	    tid_DWORD,	    "SMTPPort" )
	SER_VALUE( m_Account,		tid_STRING_OBJ,	"UsrAccount" )
	SER_VALUE( m_MailTo,		tid_STRING_OBJ,	"MailTo" )
	SER_VALUE( m_nSendMode,     tid_DWORD,		"CurrentSendMode" )
	SER_VALUE( m_mail_schedule, cTaskSchedule::eIID, "mail_schedule" )
	// temporary Passwords realization
	SER_PASSWORD( m_MailPassword,	"PSEM" )
	SER_PASSWORD( m_PrtctPassword,	"OPEP" )
	SER_VALUE( m_NSettings,     cBLNotifySettings::eIID, "NSettings" )
	
	SER_VALUE( m_LicBuyLink,	tid_STRING_OBJ,	"LicBuyLink" )
	SER_VALUE( m_LicRenewLink,	tid_STRING_OBJ,	"LicRenewLink" )

	SER_VALUE( m_nAffinity,     tid_QWORD,		"AffinityMask" )
	SER_VALUE( m_nOasInstances, tid_DWORD,      "OasInstances" )

	SER_VALUE( m_LicActivationLink,	tid_STRING_OBJ,	  "LicActivationLink" )
	SER_VALUE( m_ProxySettings, cProxySettings::eIID, "ProxySettings" )
	
	SER_VALUE( m_bUseUninstallPwd,			tid_BOOL,	"EnableUPswrdProtect" )
	SER_PASSWORD( m_UninstallPwd,	"OPEP2" )

	SER_VALUE_M( SettingsVersion,         tid_STRING_OBJ )
	SER_VALUE_M( LastShownDumpsTime,      tid_DATETIME )
	SER_VALUE_M( LastShownTracesTime,     tid_DATETIME )
	SER_VALUE_M( MinShowDumpVer,          tid_DWORD )
	SER_VALUE( m_strCustomerId,				tid_STRING_OBJ,	"CustomerId" )
	SER_PASSWORD( m_strCustomerPwd,			"CustomerPwd" )

	SER_VALUE( m_bUseVirtualKdb,tid_BOOL,       "UseVirtualKdb" )
	SER_VALUE( m_TrialActCode,	tid_STRING_OBJ, "TrialActCode" )

	SER_VALUE( m_reportSettings, cBLReportSettings::eIID, "Report" )

IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cUpdateResultInfo : public cSerializable {
	tUINT m_recordCount;
	//cDateTime m_time;
};

enum enLicRenewStatus
{
	eLrsOk              = 0x00,
	eLrsRenew           = 0x01,
	eLrsBuy             = 0x02,
};


#define MAX_STATES_COUNT   10
#define MAX_STATES_MASK    0x3FF

enum enProductState
{
	eProductOk              = 0x00,
	eProductNews            = 0x01,
	eProductNotAutoRun      = 0x02,
	eProductNotActivated    = 0x04,
	eProductAdminPolicy     = 0x08,
	eProductNotProtected    = 0x40,
	eTasksDisabledByPolicy  = 0x80,
};

enum enKeyState
{
	eKeyOk                  = 0x00,   // done
	eKeyTrial               = 0x01,   // done
	eKeyWillBeExpired       = 0x02,   // done
	eKeyWillBeBlocked       = 0x04,   // done
	eKeyAboutExpiration     = 0x08,   // done
	eKeyExpired             = 0x10,   // done
	eKeyTrialExpired        = 0x20,   // done
	eKeyBlocked             = 0x40,   // done
	eNoKeys                 = 0x80,   // done
	eKeyInvalid             = 0x100,   // done
	eKeyLimited             = 0x200,   // done
};

enum enProtectionState
{
	eProtectionOk           = 0x00,   // done
	eOnDemandTaskRunning    = 0x02,   // done
	eMyComputerNotScanned   = 0x04,   // done
	eTasksNotRunning        = 0x08,   // done
	eTasksDisabled          = 0x10,   // done
	eTasksMalfunction       = 0x20,   // done
	eProtectionNotRunnig    = 0x40,   // done
	eProtectionDisabled     = 0x80,   // done
	eProtectionSafeMode     = 0x100,  // done
	eProtectionNotInstalled = 0x200,  // done
};

enum enUpdateState
{
	eUpdateOk               = 0x00,
	eUpdateNotAuto          = 0x01,
	eBasesNotActual         = 0x08,   // done
	eBasesNotValid          = 0x10,
	eBasesOutOfDate         = 0x20,   // done
	eUpdateNeedReboot       = 0x40,
	eBasesCorrupted         = 0x100,
};

enum enThreatsState
{
	eThreatsOk              = 0x00,
	eVulnerabilityDetected  = 0x01,
	eAntiSpamNeedTraining   = 0x10,
	eThreatsUntreated       = 0x20,   // done
	eThreatsNeedReboot      = 0x40,
};

//-----------------------------------------------------------------------------

struct cBLState : public cSerializable
{
	cBLState() :
		m_ProductState(0),
		m_KeyState(0),
		m_ProtectionState(0),
		m_UpdateState(0),
		m_ThreatsState(0)
	{}

	DECLARE_IID( cBLState, cSerializable, PID_BL, blst_cBLState );

	tDWORD           m_ProductState;
	tDWORD           m_KeyState;
	tDWORD           m_ProtectionState;
	tDWORD           m_UpdateState;
	tDWORD           m_ThreatsState;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cBLState, 0 )
	SER_VALUE_M( ProductState,     tid_DWORD )
	SER_VALUE_M( ProtectionState,  tid_DWORD )
	SER_VALUE_M( KeyState,         tid_DWORD )
	SER_VALUE_M( UpdateState,      tid_DWORD )
	SER_VALUE_M( ThreatsState,     tid_DWORD )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cBLStatistics : public cTaskStatistics
{
	cBLStatistics() :
		m_StateMask(0),
		m_LicRenewStatus(eLrsOk),
		m_nKlopKeyCount(0),
		m_nLicInvalidReason(ekirUnknown),
		m_nLicDaysTillExpiration(0),
		m_nLicKeyType(ektUnknown),
		m_tmLastStart(-1),
		m_tmLastSuccessfulFullScan(-1),
		m_tmLastSuccessfulUpdate(-1),
		m_tmNextAutoUpdate(-1),
		m_tmLastFullScanState(PROFILE_STATE_UNK),
		m_bRollbackAllowed(cFALSE),
		m_dwUpdateAutoPeriodFail(5),
		m_dwUpdateAutoPeriodSuccess(2*60),
		m_bIsUpdatePerformed(cFALSE),
		m_strScanningTaskId(-1)
	{}

	DECLARE_IID( cBLStatistics, cTaskStatistics, PID_BL, blst_cBLStatistics);

	cBLState         m_State;
	tQWORD           m_StateMask;
				    
	__time32_t       m_tmLastStart;
	__time32_t       m_tmLastSuccessfulFullScan;
	__time32_t       m_tmLastSuccessfulUpdate;
	__time32_t       m_tmNextAutoUpdate;
	tProfileState    m_tmLastFullScanState;

	enLicRenewStatus m_LicRenewStatus;
	tDWORD           m_nKlopKeyCount;
	tDWORD           m_nLicInvalidReason;
	tDWORD           m_nLicDaysTillExpiration;
	tDWORD           m_nLicKeyType;
	
	cStringObj       m_strScanningTask;
	tDWORD           m_strScanningTaskId;
	cStringObj       m_strBlockedPort;
	cERROR           m_errTaskMalfunction;
	cStringObj       m_strTaskMalfunction;
	cStringObj       m_strTaskDisabled;
	cStringObj       m_strTaskNotRunning;
	tBOOL            m_bRollbackAllowed;
	cERROR           m_errLoadBases;

    cStrObj          m_strProductHotfix;
	cStrObj          m_strProductHotfixNew;

    tUINT            m_dwUpdateAutoPeriodFail;
	tUINT            m_dwUpdateAutoPeriodSuccess;

	tBOOL            m_bIsUpdatePerformed;
				    
	void make_state(tDWORD& nStateMask, tDWORD nFilterMask = 0)
	{
		m_StateMask |= ((tQWORD)nStateMask) << (&nStateMask-&m_State.m_ProductState)*MAX_STATES_COUNT;
		for(int i = MAX_STATES_COUNT-1; i >=0; i--)
			if( nStateMask & (1 << i) & ~nFilterMask )
				{ nStateMask = (1 << i); break; }
	}

	void get_full_state(tDWORD& nState)
	{
		nState = (tDWORD)(m_StateMask >> (&nState-&m_State.m_ProductState)*MAX_STATES_COUNT) & MAX_STATES_MASK;
	}

	ptrdiff_t get_statusid(tDWORD& nStateMask, tDWORD id)
	{
		int i = 0;
		for(i = 0; i < MAX_STATES_COUNT; i++)
			if( id == (tDWORD)(1 << i) ) break;
		return (&nStateMask-&m_State.m_ProductState)*MAX_STATES_COUNT+i;
	}
};

IMPLEMENT_SERIALIZABLE_BEGIN( cBLStatistics, "ProductStatistics" )
	SER_BASE( cTaskStatistics, 0 )

	SER_VALUE( m_State.m_ProductState,    tid_DWORD, "ProductState" )
	SER_VALUE( m_State.m_ProtectionState, tid_DWORD, "ProtectionState" )
	SER_VALUE( m_State.m_KeyState,        tid_DWORD, "KeyState" )
	SER_VALUE( m_State.m_UpdateState,     tid_DWORD, "UpdateState" )
	SER_VALUE( m_State.m_ThreatsState,    tid_DWORD, "ThreatsState" )
	SER_VALUE( m_StateMask,               tid_QWORD, "ProductStateMask" )

	SER_VALUE( m_tmLastStart,               tid_DWORD,        "LastStart" )
	SER_VALUE( m_tmLastSuccessfulFullScan,  tid_DWORD,        "LastSuccessfulFullScan" )
	SER_VALUE( m_tmLastSuccessfulUpdate,    tid_DWORD,        "LastSuccessfulUpdate" )
	SER_VALUE( m_tmNextAutoUpdate,          tid_DWORD,        "NextAutoUpdate" )
	SER_VALUE( m_tmLastFullScanState,       tid_DWORD,        "LastFullScanState" )
	SER_VALUE( m_errLoadBases,              tid_ERROR,        "BasesError" )

	SER_VALUE( m_LicRenewStatus,            tid_DWORD,        "LicRenewStatus" )
	SER_VALUE( m_nKlopKeyCount,             tid_DWORD,        "KlopKeyCount" )
	SER_VALUE( m_nLicDaysTillExpiration,    tid_DWORD,        "LicDaysTillExpiration" )
	SER_VALUE( m_nLicInvalidReason,         tid_DWORD,        "LicInvalidReason" )
	SER_VALUE( m_nLicKeyType,               tid_DWORD,        "LicKeyType" )
	
	SER_VALUE( m_strBlockedPort,            tid_STRING_OBJ,   "BlockedPort" )
	SER_VALUE( m_errTaskMalfunction,        tid_ERROR,        "Error" )
	SER_VALUE( m_strTaskMalfunction,        tid_STRING_OBJ,   "TaskMalfunction" )
	SER_VALUE( m_strTaskDisabled,           tid_STRING_OBJ,   "TaskDisabled" )
	SER_VALUE( m_strTaskNotRunning,         tid_STRING_OBJ,   "TaskNotRunning" )
	SER_VALUE( m_strScanningTask,           tid_STRING_OBJ,   "ScanningTask" )
	SER_VALUE( m_strScanningTaskId,         tid_DWORD,        "ScanningTaskId" )

	SER_VALUE_M( bRollbackAllowed,          tid_BOOL )

    SER_VALUE( m_strProductHotfix,          tid_STRING_OBJ,   "ProductHotfix" )
	SER_VALUE( m_strProductHotfixNew,       tid_STRING_OBJ,   "ProductHotfixNew" )

    SER_VALUE( m_dwUpdateAutoPeriodFail,    tid_DWORD,        "AutoModeUpdatePeriod" )
	SER_VALUE( m_dwUpdateAutoPeriodSuccess, tid_DWORD,        "AutoModeMinutePeriod" )

	SER_VALUE( m_bIsUpdatePerformed,        tid_BOOL,         "IsUpdatePerformed" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
	
struct cBLUpdateWhatsNew : public cSerializable
{
	DECLARE_IID( cBLUpdateWhatsNew, cSerializable, PID_BL, blst_cBLUpdateWhatsNew );

	cStringObj  m_sContent;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cBLUpdateWhatsNew, 0 )
	SER_VALUE( m_sContent, tid_STRING_OBJ, "Content" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cBLAppInfo : public cSerializable
{
    cBLAppInfo() : m_dwAppID(0), m_dwSerial1(0), m_dwSerial2(0), m_dwSerial3(0), m_dwInstallID(0) {}
	
    DECLARE_IID( cBLAppInfo, cSerializable, PID_BL, blst_cBLAppInfo );
	
    tDWORD          m_dwAppID ;
    tDWORD          m_dwSerial1 ;
    tDWORD          m_dwSerial2 ;
    tDWORD          m_dwSerial3 ;
    tDWORD          m_dwInstallID ;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cBLAppInfo, 0 )
    SER_VALUE( m_dwAppID,       tid_DWORD,  "AppID"     )
    SER_VALUE( m_dwSerial1,     tid_DWORD,  "Serial1"   )
    SER_VALUE( m_dwSerial2,     tid_DWORD,  "Serial2"   )
    SER_VALUE( m_dwSerial3,     tid_DWORD,  "Serial3"   )
    SER_VALUE( m_dwInstallID,   tid_DWORD,  "InstallID" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// cUpdaterProductSpecific

struct cUpdaterProductSpecific : public cTaskBLSettings
{
	cUpdaterProductSpecific() :
		m_bUpdateExecutables(cFALSE),
		m_bUpdateAHDeprecated(cTRUE),
		m_bRescanQuarantineAfterUpdate(cTRUE)
	{}

	DECLARE_IID( cUpdaterProductSpecific, cTaskBLSettings, PID_BL, blst_cUpdaterProductSpecific );

	tBOOL               m_bUpdateExecutables;
	tBOOL               m_bUpdateAHDeprecated;
	tBOOL               m_bRescanQuarantineAfterUpdate;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cUpdaterProductSpecific, 0 )
	SER_BASE(  cTaskBLSettings,                0)
	SER_VALUE( m_bUpdateExecutables,           tid_BOOL,  "UpdateExecutables" )
	SER_VALUE( m_bUpdateAHDeprecated,          tid_BOOL,  "UpdateAH" )
	SER_VALUE( m_bRescanQuarantineAfterUpdate, tid_BOOL,  "RescanQuarantineAfterUpdate" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// cScanerProductSpecific

struct cScanerProductSpecific : public cTaskBLSettings
{
	cScanerProductSpecific() :
		m_bRescanAfterUpdate(cFALSE),
		m_OBSOLETE_bRunOnStartup(-1) // bf 16819
	{}

	DECLARE_IID( cScanerProductSpecific, cTaskBLSettings, PID_BL, blst_cScanerProductSpecific );

	tBOOL               m_bRescanAfterUpdate;
	tBOOL               m_OBSOLETE_bRunOnStartup;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cScanerProductSpecific, 0 )
	SER_BASE(  cTaskBLSettings,                0)
	SER_VALUE( m_bRescanAfterUpdate,           tid_BOOL,  "RescanAfterUpdate" )
	SER_VALUE( m_OBSOLETE_bRunOnStartup,       tid_BOOL,  "RunOnStartup" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

enum enSelfProtectionNotifyType
{
	_espAccessToUnknown = 0,
	_espAccessToProcess = 1,
	_espAccessToFile = 2,
	_espAccessToReg = 3,
};

struct cProtectionNotify : public cTaskHeader
{
	cProtectionNotify () :
		m_eNotifyType(_espAccessToUnknown),
		m_nOurPID(0),
		m_tEvt(0),
		m_ImagePath(),
		m_RegKey(),
		m_RegVal(),
		m_FileName()
	{}

	DECLARE_IID( cProtectionNotify, cTaskHeader, PID_BL, blst_cProtectionNotify );

	enSelfProtectionNotifyType m_eNotifyType;
	tDWORD     m_nOurPID;

	__time32_t m_tEvt;
	cStringObj m_ImagePath;
	cStringObj m_RegKey;
	cStringObj m_RegVal;
	cStringObj m_FileName;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cProtectionNotify, 0 )
    SER_BASE( cTaskHeader, 0 )
	SER_VALUE_M(eNotifyType,    tid_DWORD)
	SER_VALUE_M(nOurPID,		tid_DWORD)
	SER_VALUE_M(tEvt,			tid_DWORD)
	SER_VALUE_M(ImagePath,		tid_STRING_OBJ)
	SER_VALUE_M(RegKey,			tid_STRING_OBJ)
	SER_VALUE_M(RegVal,			tid_STRING_OBJ)
	SER_VALUE_M(FileName,		tid_STRING_OBJ)
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cSvrMgrRequest : public cSerializable
{
	cSvrMgrRequest(tWCHAR* sService = NULL, tDWORD request = 0) :
		m_Service(sService), m_Request(request){}

	DECLARE_IID( cSvrMgrRequest, cSerializable, PID_BL, gst_cSvrMgrRequest );

	cStrObj  m_Service;
	tDWORD   m_Request;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cSvrMgrRequest, 0 )
	SER_VALUE_M( Service,               tid_STRING_OBJ)
	SER_VALUE_M( Request,               tid_DWORD)
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cFSSyncRequest : public cSerializable
{
	cFSSyncRequest(tDWORD TID = -1) : m_TID(TID){}

	DECLARE_IID( cFSSyncRequest, cSerializable, PID_BL, blst_cBLFSSyncRequest );

	tDWORD   m_TID;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cFSSyncRequest, 0 )
	SER_VALUE_M( TID,					tid_DWORD)
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

#endif//  __S_BL_H
