#ifndef __S_GUI_H
#define __S_GUI_H

#include <AV/structs/s_scaner.h>

#include <ParentalControl/structs/s_urlflt.h>

#include <Updater/updater.h>
#include <ProductCore/structs/s_taskmanager.h>
#include <ProductCore/structs/s_settings.h>

#include <Hips/structs/s_hips.h>

#include <PPP/plugin/p_gui.h>
#include <PPP/structs/s_bl.h>

enum enGuiStructTypes
{
	gst_AskGuiAction = 1,
	gst_cProfileAction,
	gst_cAskScanObjects,
	gst_cBrowseFile,
	gst_cGuiErrorInfo,
	gst_cLicErrorInfo,
	gst_cQBErrorInfo,
	gst_cFwAppRuleEx,
	gst_cAHFWProtocolProperty,
	gst_cAHFWProtocol,
	gst_cFwRulePreset,
	gst_cAHFWRulePredefines,
	gst_cFWAddressEx,
	gst_cFWLearnHlp,
	gst_cGuiReportExportInfo,
	gst_cGuiGlobals,
	gst_cLicExpirationInfo,
	gst_cGuiPauseProfile,
	gst_cGuiOgMacros,
	gst_cGuiReportExportQuery,
	gst_cResetItem,
	gst_cGuiResetArea,
	gst_cGuiAhNetActivity,
	gst_cGuiVariables,
	gst_cGuiMailwasherSettings,
	gst_cGuiMailwasher,
	gst_cRegKeyInfo,
	gst_cProgressInfo,
	gst_cGuiCurProfile,
	gst_cGuiEvent,
	gst_cDataFiles,
	gst_cDataFilesCleanup,
	gst_cGuiSettings,
	gst_CProfile,
	gst_CTaskProfile,
	gst_CTaskProfileAdm,
	gst_CProduct,
	gst_CGuiProfile,
	gst_CGuiSubProfile,
	gst_cObjInfo,
	gst_cGuiAskActionInfo,
	gst_cUpdateFilterListItem,
	gst_cMsgParams,
	gst_cProtectionNotify,
	gst_cMakeRescueDisk,
	gst_CAttachedFile,
	gst_CSendMailInfo,
	gst_CScanViewObject,
	gst_cConfigureProductDiscartedKeys,
	gst_cGuiLicCheckKeyInfo,
	gst_cGuiNewsItem,
	gst_cGuiNews,
	gst_cGuiUrlFltProfile,
	gst_cGuiSecurityRisks,
	gst_cDumpFile,
	gst_cDumpFiles,
    gst_cGuiMsgBox,
	gst_cGuiAppGroup,
	gst_cGuiResGroup,
	gst_cGuiResRule,
	gst_cGuiAvzScript,
	gst_cGuiScanStatistic,
	gst_cCmnStatistic,
};

//-----------------------------------------------------------------------------

struct cAskGuiAction : public cSerializable {
	DECLARE_IID( cAskGuiAction, cSerializable, PID_GUI, gst_AskGuiAction );
	enum Actions{ SHOW_MAIN_WND = eIID, SHOW_SETTINGS_WND, SHOW_RISK_WND };
};

//-----------------------------------------------------------------------------

struct cProfileAction : public cTaskHeader {

	cProfileAction(const tCHAR* strProfile=NULL, tTaskId nTaskId=TASKID_UNK, const tCHAR* strInfo=NULL) : 
		m_strProfile(strProfile),
		m_nTaskId(nTaskId),
		m_strInfo(strInfo)
	{}

	DECLARE_IID( cProfileAction, cTaskHeader, PID_GUI, gst_cProfileAction );

	enum Actions{ START_TASK = eIID, SHOW_REPORT, SHOW_SETTINGS };

	cStringObj      m_strProfile;
	tTaskId         m_nTaskId;
	cStringObj      m_strInfo;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cProfileAction, 0 )
	SER_BASE(  cTaskHeader, 0 )
	SER_VALUE( m_strProfile,     tid_STRING_OBJ, 0 )
	SER_VALUE( m_nTaskId,        tid_DWORD, 0 )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cAskScanObjects : public cTaskHeader {
	cAskScanObjects() :
		m_aScanObjects()
	{}

	DECLARE_IID( cAskScanObjects, cTaskHeader, PID_GUI, gst_cAskScanObjects );

	scanobjarr_t    m_aScanObjects;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAskScanObjects, 0 )
	SER_BASE(  cTaskHeader, 0 )
	SER_VECTOR( m_aScanObjects,   cScanObject::eIID, 0 )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cBrowseFile : public cSerializable {
	cBrowseFile() :
		m_PathName()
	{}

	DECLARE_IID( cBrowseFile, cSerializable, PID_GUI, gst_cBrowseFile );

	cStringObj      m_PathName;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cBrowseFile, 0 )
	SER_VALUE( m_PathName,       tid_STRING_OBJ,   "PathName" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cMsgParams : public cSerializable
{
	cMsgParams() :
		m_nRes(0),
		m_nVal1(0),
		m_nVal2(0),
		m_nVal3(0),
		m_nVal4(0),
		m_nVal5(0)
	{}

	DECLARE_IID( cMsgParams, cSerializable, PID_GUI, gst_cMsgParams );

	tPTR        m_nRes;
	tPTR        m_nVal1;
	tPTR        m_nVal2;
	tPTR        m_nVal3;
	tPTR        m_nVal4;
	tPTR        m_nVal5;
	cStrObj     m_strVal1;
	cStrObj     m_strVal2;
	cStrObj     m_strVal3;
	cStrObj     m_strVal4;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cMsgParams, 0 )
	SER_VALUE( m_nRes,       tid_PTR,        "nRes" )
	SER_VALUE( m_nVal1,      tid_PTR,        "nVal1" )
	SER_VALUE( m_nVal2,      tid_PTR,        "nVal2" )
	SER_VALUE( m_nVal3,      tid_PTR,        "nVal3" )
	SER_VALUE( m_nVal4,      tid_PTR,        "nVal4" )
	SER_VALUE( m_nVal5,      tid_PTR,        "nVal5" )
	SER_VALUE( m_strVal1,    tid_STRING_OBJ, "strVal1" )
	SER_VALUE( m_strVal2,    tid_STRING_OBJ, "strVal2" )
	SER_VALUE( m_strVal3,    tid_STRING_OBJ, "strVal3" )
	SER_VALUE( m_strVal4,    tid_STRING_OBJ, "strVal4" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

#define OPCODE_INTERNAL       1
#define OPCODE_SYSTEM         2
#define OPCODE_MAIL           3

struct cGuiErrorInfo : public cMsgParams
{
	cGuiErrorInfo(tERROR nErr = 0, tDWORD nOperationCode = 0) :
		m_nOperationCode(nOperationCode),
		m_nErr(nErr)
	{}
	
	DECLARE_IID( cGuiErrorInfo, cMsgParams, PID_GUI, gst_cGuiErrorInfo );

	tDWORD            m_nOperationCode;
	tERROR            m_nErr;
	cStringObj        m_strComponent;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiErrorInfo, 0 )
	SER_BASE( cMsgParams, 0 )
	SER_VALUE( m_nOperationCode,      tid_DWORD,        "OperationCode" )
	SER_VALUE( m_nErr,                tid_ERROR,        "ErrorCode" )
	SER_VALUE( m_strComponent,        tid_STRING_OBJ,   "Component" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

#define LICOPCODE_SYSTEM        1
#define LICOPCODE_ADD           2
#define LICOPCODE_DELETE        3
#define LICOPCODE_ADDONLINE     4

struct cLicErrorInfo : public cGuiErrorInfo
{
	DECLARE_IID( cLicErrorInfo, cGuiErrorInfo, PID_GUI, gst_cLicErrorInfo );

	cStringObj        m_strKeyFile;
	tDWORD            m_dwOnlineActSrvError;
	tDWORD            m_dwOnlineActSrvErrorSpecific;
	
	bool IsProxyError()
	{
		switch (m_nErr)
		{
		case errONLINE_SENDING:
		case errONLINE_CLNT_UNAUTH:
		case errONLINE_CLNT_FORBID:
		case errONLINE_CLNT_NOTFND:
		case errONLINE_CLNT_PRXAUTHRQ:
		case errONLINE_SERV_BADGATE:
		case errONLINE_SERV_GATIMO:
			return true;
		}

		return false;
	}
};

IMPLEMENT_SERIALIZABLE_BEGIN( cLicErrorInfo, 0 )
	SER_BASE( cGuiErrorInfo, 0 )
	SER_VALUE( m_strKeyFile,       tid_STRING_OBJ,   "KeyFile" )
	SER_VALUE( m_dwOnlineActSrvError, tid_DWORD,     "OnlineActSrvError" )
	SER_VALUE( m_dwOnlineActSrvErrorSpecific, tid_DWORD, "OnlineActSrvErrorSpecific" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

enum eLicExpirationType {
    eletValid,
    eletAlmostExpired,
    eletExpired,
    eletNoKey
};

struct cLicExpirationInfo : public cSerializable
{
	DECLARE_IID( cLicExpirationInfo, cSerializable, PID_GUI, gst_cLicExpirationInfo );

	eLicExpirationType  m_nExpType;
	tDWORD              m_nExpDays;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cLicExpirationInfo, 0 )
	SER_VALUE( m_nExpType,       tid_DWORD,   "ExpType" )
	SER_VALUE( m_nExpDays,       tid_DWORD,   "ExpDays" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

#define QBOPCODE_SYSTEM        1
#define QBOPCODE_INTERNAL      2
#define QBOPCODE_ADD           3
#define QBOPCODE_DELETE        4
#define QBOPCODE_MAIL          5
#define QBOPCODE_RESTORE       6
#define QBOPCODE_SENDFILETIME  7

struct cQBErrorInfo : public cGuiErrorInfo
{
	DECLARE_IID( cQBErrorInfo, cGuiErrorInfo, PID_GUI, gst_cQBErrorInfo );

	cStringObj        m_strObjectName;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cQBErrorInfo, 0 )
	SER_BASE( cGuiErrorInfo, 0 )
	SER_VALUE( m_strObjectName,  tid_STRING_OBJ,   "ObjectName" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cObjInfo : public cSerializable
{
	DECLARE_IID( cObjInfo, cSerializable, PID_GUI, gst_cObjInfo );

	cStringObj			                m_sDisplayName;
	cStringObj			                m_sCompanyName;
	cStringObj                          m_sProductVersion;
	cStringObj                          m_sFileVersion;
	cStringObj                          m_sDescription;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cObjInfo, 0 )
	SER_VALUE(  m_sDisplayName,         tid_STRING_OBJ,              "DisplayName" )
	SER_VALUE(  m_sCompanyName,         tid_STRING_OBJ,              "CompanyName" )
	SER_VALUE(  m_sProductVersion,      tid_STRING_OBJ,              "ProductVersion" )
	SER_VALUE(  m_sFileVersion,         tid_STRING_OBJ,              "FileVersion" )
	SER_VALUE(  m_sDescription,         tid_STRING_OBJ,              "Description" )
IMPLEMENT_SERIALIZABLE_END()


//-----------------------------------------------------------------------------

struct cGuiReportExportInfo : public cSerializable
{
	cGuiReportExportInfo() :
		m_nPercentCompleted(0)
		{
		}

	DECLARE_IID( cGuiReportExportInfo, cSerializable, PID_GUI, gst_cGuiReportExportInfo );

	tDWORD m_nPercentCompleted;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiReportExportInfo, 0 )
	SER_VALUE( m_nPercentCompleted, tid_DWORD, "Completion" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cGuiReportExportQuery : public cSerializable
{
	cGuiReportExportQuery() :
		m_pReader(NULL),
		m_pInfo(NULL),
		m_nResult(0)
		{
		}

	DECLARE_IID( cGuiReportExportQuery, cSerializable, PID_GUI, gst_cGuiReportExportQuery );

	tPTR      m_pReader;
	tPTR      m_pInfo;
	tDWORD    m_nResult;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiReportExportQuery, 0 )
	SER_VALUE( m_pReader, tid_PTR,   0 )
	SER_VALUE( m_pInfo,   tid_PTR,   0 )
	SER_VALUE( m_nResult, tid_DWORD, 0 )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cGuiNewsItem : public cSerializable
{
	DECLARE_IID( cGuiNewsItem, cSerializable, PID_GUI, gst_cGuiNewsItem );

	enum enSeverity
	{										   
		eAlarm          = 1,
		eWarning        = 3,
		eInformation    = 4
	};

	cStringObj  m_strId;
	enSeverity  m_Severity;
	cStringObj  m_strTitle;
	cStringObj  m_strAnnouncement;
	cStringObj  m_strText;
	cStringObj  m_strLink;
	cStringObj  m_strLinkUrl;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiNewsItem, 0 )
	SER_VALUE( m_strId,           tid_STRING_OBJ, "Id" )
	SER_VALUE( m_Severity,        tid_DWORD,      "Severity" )
	SER_VALUE( m_strTitle,        tid_STRING_OBJ, "Title" )
	SER_VALUE( m_strAnnouncement, tid_STRING_OBJ, "Announcement" )
	SER_VALUE( m_strText,         tid_STRING_OBJ, "Text" )
	SER_VALUE( m_strLink,         tid_STRING_OBJ, "Link" )
	SER_VALUE( m_strLinkUrl,      tid_STRING_OBJ, "URL" )
IMPLEMENT_SERIALIZABLE_END()

typedef cVector<cGuiNewsItem> cGuiNews;

//-----------------------------------------------------------------------------

struct cGuiGlobals : public cSerializable
{
	cGuiGlobals() :
		m_nOSVersion(0),
		m_nProcessorType(0),
		m_bWinPE(cFALSE)
	{
		m_LicInfo.m_dwFuncLevel = eflFullFunctionality;
	}
	
	DECLARE_IID( cGuiGlobals, cSerializable, PID_GUI, gst_cGuiGlobals );

	cStrObj                m_strProductVersion;
	cStrObj                m_strCopyright;
	cStrObj                m_strOSVersion;
	tDWORD                 m_nOSVersion;
	tDWORD                 m_nProcessorType;
	tBOOL                  m_bWinPE;

	cCheckInfo             m_LicInfo;
	cCheckInfo             m_ReservedLicInfo;
	
	cSerObj<cProfile>	   m_CurScanProfile;
	cSerObj<cProfile>	   m_CurUpdaterProfile;

	cSerObj<cODSStatistics>	   m_CurScanStatistics;
	cSerObj<cUpdaterStatistics>	   m_CurUpdaterStatistics;

	cGuiNews				m_News;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiGlobals, 0 )
	SER_VALUE( m_strProductVersion,     tid_STRING_OBJ, "ProductVersion" )
	SER_VALUE( m_strCopyright,          tid_STRING_OBJ, "Copyright" )
	SER_VALUE( m_strOSVersion,          tid_STRING_OBJ, "OSVersion" )
	SER_VALUE( m_nOSVersion,            tid_DWORD,      "OSVersionId" )
	SER_VALUE( m_nProcessorType,        tid_DWORD,      "ProcessorType" )
	SER_VALUE_M( bWinPE,                tid_BOOL )
	SER_VALUE( m_LicInfo,               cCheckInfo::eIID, "LicInfo" )
	SER_VALUE( m_ReservedLicInfo,       cCheckInfo::eIID, "ResLicInfo" )
	SER_VALUE_PTR( m_CurScanProfile,	cProfile::eIID, "ScansProfile" )
	SER_VALUE_PTR( m_CurUpdaterProfile,	cProfile::eIID, "UpdatersProfile" )
	SER_VALUE_PTR( m_CurScanStatistics,	cODSStatistics::eIID, "ScansStatistics" )
	SER_VALUE_PTR( m_CurUpdaterStatistics, cUpdaterStatistics::eIID, "UpdatersStatistics" )
	SER_VECTOR(	m_News,					cGuiNewsItem::eIID, "News")
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cGuiMailwasherSettings : public cSerializable
{
	cGuiMailwasherSettings() :
		m_bShowMailwasher(cTRUE),
		m_bShowCluges(cFALSE),
		m_bShowNewMessages(cTRUE),
		m_nMessagesToRead(25)
	{}

	DECLARE_IID( cGuiMailwasherSettings, cSerializable, PID_GUI, gst_cGuiMailwasherSettings );

	tBOOL           m_bShowMailwasher;
	tBOOL           m_bShowCluges;
	tBOOL           m_bShowNewMessages;
	tDWORD          m_nMessagesToRead;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiMailwasherSettings, 0 )
	SER_VALUE( m_bShowMailwasher,     tid_BOOL,       "ShowMailwasher" )
	SER_VALUE( m_bShowCluges,         tid_BOOL,       "ShowCluges" )
	SER_VALUE( m_bShowNewMessages,    tid_BOOL,       "ShowNewMessages" )
	SER_VALUE( m_nMessagesToRead,     tid_DWORD,      "MessagesToRead" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cGuiMailwasher : public cGuiMailwasherSettings
{
	cGuiMailwasher() :
		m_nTotalMessages(0),
		m_nNewMessages(0),
		m_nMessagesRecieved(0),
		m_nMessagesLeft(0)
	{}

	DECLARE_IID( cGuiMailwasher, cGuiMailwasherSettings, PID_GUI, gst_cGuiMailwasher );

	tDWORD          m_nTotalMessages;
	tDWORD          m_nNewMessages;
	tDWORD          m_nMessagesRecieved;
	tDWORD          m_nMessagesLeft;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiMailwasher, 0 )
	SER_BASE( cGuiMailwasherSettings, 0 )
	SER_VALUE( m_nTotalMessages,      tid_DWORD,      "TotalMessages" )
	SER_VALUE( m_nNewMessages,        tid_DWORD,      "NewMessages" )
	SER_VALUE( m_nMessagesRecieved,   tid_DWORD,      "MessagesRecieved" )
	SER_VALUE( m_nMessagesLeft,       tid_DWORD,      "MessagesLeft" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cGuiVariables : public cSerializable
{
	cGuiVariables() :
		m_qwStateFilterMask(0),
		m_bFwLearnCreateRule(cTRUE)
	{}
	
	DECLARE_IID( cGuiVariables, cSerializable, PID_GUI, gst_cGuiVariables );

	tQWORD                 m_qwStateFilterMask;
	cStringObj             m_strLastScanAddedObject;
	cGuiMailwasherSettings m_MailwasherSettings;
	tBOOL                  m_bFwLearnCreateRule;
	cVector<cStringObj>    m_HiddenNews;
	cBLState               m_SkipRisk;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiVariables, 0 )
	SER_VALUE( m_qwStateFilterMask,   tid_QWORD,                    "StateFilterMask" )
	SER_VALUE( m_strLastScanAddedObject,  tid_STRING_OBJ,           "LastScanAddedObject" )
	SER_VALUE( m_MailwasherSettings,  cGuiMailwasherSettings::eIID, "MailwasherSettings" )
	SER_VALUE( m_bFwLearnCreateRule,  tid_BOOL,                     "FwLearnCreateRule" )
	SER_VECTOR( m_HiddenNews,         tid_STRING_OBJ,               "HiddenNews" )
	SER_VALUE_M( SkipRisk,            cBLState::eIID )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cGuiSettings : public cSerializable
{
	cGuiSettings() :
		m_bEnableSkinPath(cFALSE),
		m_bEnableTrayIconAnimation(cTRUE),
		m_bEnableTransparentWindows(cTRUE),
		m_bEnableStandartGuiStyle(cTRUE),
		m_nTransparency(0xFF),
		m_bEnableNewsTrayIcon(cTRUE)
	{
	}

	DECLARE_IID( cGuiSettings, cSerializable, PID_GUI, gst_cGuiSettings );

	tBOOL           m_bEnableSkinPath;
	cStringObj      m_strSkinPath;
	tBOOL           m_bEnableTrayIconAnimation;
	tBOOL           m_bEnableTransparentWindows;
	tBOOL           m_bEnableStandartGuiStyle;
	tBYTE           m_nTransparency;
	tBOOL           m_bEnableNewsTrayIcon;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiSettings, 0 )
	SER_VALUE( m_bEnableSkinPath,	        tid_BOOL,	    "EnableSkinPath" )
	SER_VALUE( m_strSkinPath,               tid_STRING_OBJ, "SkinPath" )
	SER_VALUE( m_bEnableTrayIconAnimation,	tid_BOOL,	    "EnableTrayIconAnimation" )
	SER_VALUE( m_bEnableTransparentWindows,	tid_BOOL,	    "EnableTransparentWindows" )
	SER_VALUE( m_bEnableStandartGuiStyle,	tid_BOOL,	    "EnableStandartGuiStyle" )
	SER_VALUE( m_nTransparency,	            tid_BYTE,	    "TransparencyValue" )
	SER_VALUE( m_bEnableNewsTrayIcon,		tid_BOOL,	    "EnableNewsTrayIcon" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cGuiPauseProfile : public cSerializable
{
	cGuiPauseProfile() :
		m_nResumeTime(0),
		m_nResumeMode(0)
	{}
	
	DECLARE_IID( cGuiPauseProfile, cSerializable, PID_GUI, gst_cGuiPauseProfile );

	tDWORD   m_nResumeTime;
	tDWORD   m_nResumeMode;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiPauseProfile, 0 )
	SER_VALUE( m_nResumeTime,      tid_DWORD, "ResumeTime" )
	SER_VALUE( m_nResumeMode,      tid_DWORD, "ResumeMode" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cResetItem : public cEnabledStrItem
{
	cResetItem() :
		m_nIdent(0)
	{
	}
	
	DECLARE_IID( cResetItem, cEnabledStrItem, PID_GUI, gst_cResetItem );

	tDWORD          m_nIdent;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cResetItem, 0 )
	SER_BASE( cEnabledStrItem, 0 )
IMPLEMENT_SERIALIZABLE_END()

typedef cVector<cResetItem> cResetItems;

//-----------------------------------------------------------------------------

struct cGuiResetArea : public cSerializable
{
	DECLARE_IID( cGuiResetArea, cSerializable, PID_GUI, gst_cGuiResetArea );

	cResetItems m_aResetItems;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiResetArea, 0 )
	SER_VECTOR( m_aResetItems,        cResetItem::eIID, "ResetItems" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

/*enum eFwActivityType
{
	fwatOpenedPort = 0,
	fwatConnection = 1
};

struct cGuiAhNetActivity : public cAHFWConnection
{
	cGuiAhNetActivity() :
		m_nPID(0),
		m_eType(fwatOpenedPort)
	{}

	DECLARE_IID( cGuiAhNetActivity, cAHFWConnection, PID_GUI, gst_cGuiAhNetActivity );

	cStringObj      m_sAppName;
	cStringObj      m_sCmdLine;
	tDWORD          m_nPID;
	eFwActivityType m_eType;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiAhNetActivity, 0 )
	SER_BASE( cAHFWConnection, 0 )
	SER_VALUE( m_sAppName,            tid_STRING_OBJ, "AppName" )
	SER_VALUE( m_sCmdLine,            tid_STRING_OBJ, "CmdLine" )
	SER_VALUE( m_nPID,                tid_DWORD,      "PID" )
	SER_VALUE( m_eType,               tid_DWORD,      "Type" )
IMPLEMENT_SERIALIZABLE_END()*/

//-----------------------------------------------------------------------------

struct cProgressInfo : public cSerializable
{
	cProgressInfo() : 
		m_nProgressPercent(0)
	{}

	DECLARE_IID( cProgressInfo, cSerializable, PID_GUI, gst_cProgressInfo );

	tDWORD      m_nProgressPercent;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cProgressInfo, 0 )
	SER_VALUE( m_nProgressPercent,    tid_DWORD,	"ProgressPercent" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cGuiCurProfile : public cSerializable
{
	DECLARE_IID( cGuiCurProfile, cSerializable, PID_GUI, gst_cGuiCurProfile );

	cProfileBase m_CurProfile;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiCurProfile, 0 )
	SER_VALUE( m_CurProfile, cProfileBase::eIID, "CurrentProfile" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

class CRootItem;
class CItemBase;

struct cGuiEvent : public cMsgParams
{
	typedef void OpFunc(CRootItem * pRoot, CItemBase * pItem, cGuiEvent * pEvt);

	cGuiEvent(OpFunc * p_func = NULL) :
		m_pFunc(p_func),
		m_bSync(cFALSE),
		m_pItem(NULL)
	{}
	
	DECLARE_IID( cGuiEvent, cMsgParams, PID_GUI, gst_cGuiEvent );

	OpFunc *    m_pFunc;
	tBOOL       m_bSync;
	
	CItemBase * m_pItem;
	cStrObj     m_strItemId;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiEvent, 0 )
	SER_BASE( cMsgParams, 0 )
	SER_VALUE( m_pFunc,      tid_PTR,        0 )
	SER_VALUE( m_bSync,      tid_BOOL,       0 )
	SER_VALUE( m_pItem,      tid_PTR,        0 )
	SER_VALUE( m_strItemId,  tid_STRING_OBJ, 0 )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cDataFiles : public cSerializable
{
	cDataFiles() :
		m_nItems(0),
		m_nSize(0)
	{
	}
	
	DECLARE_IID( cDataFiles, cSerializable, PID_GUI, gst_cDataFiles );

	tDWORD      m_nItems;
	tDWORD      m_nSize;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cDataFiles, 0 )
	SER_VALUE( m_nItems,   tid_DWORD,      "ItemsCount" )
	SER_VALUE( m_nSize,    tid_DWORD,      "Size" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
	
struct cGuiAskActionInfo : public cSerializable
{
	cGuiAskActionInfo() :
		m_nActionId(-1)
	{}
	
	DECLARE_IID( cGuiAskActionInfo, cSerializable, PID_GUI, gst_cGuiAskActionInfo );

	enum { m_nButtonCount = 3 };

	tActionId m_nActionId;
	tDWORD    m_dwButtonAction[m_nButtonCount];
	tBOOL     m_dwButtonEnabled[m_nButtonCount];
	tBOOL     m_dwButtonDefault[m_nButtonCount];
};

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiAskActionInfo, 0 )
	SER_VALUE( m_nActionId,   tid_DWORD,      "ActionId" )
	SER_VALUE( m_dwButtonAction[0], tid_DWORD,  "Button1Action" )
	SER_VALUE( m_dwButtonEnabled[0], tid_BOOL,  "Button1Enabled" )
	SER_VALUE( m_dwButtonDefault[0], tid_BOOL,  "Button1Default" )
	SER_VALUE( m_dwButtonAction[1], tid_DWORD,  "Button2Action" )
	SER_VALUE( m_dwButtonEnabled[1], tid_BOOL,  "Button2Enabled" )
	SER_VALUE( m_dwButtonDefault[1], tid_BOOL,  "Button2Default" )
	SER_VALUE( m_dwButtonAction[2], tid_DWORD,  "Button3Action" )
	SER_VALUE( m_dwButtonEnabled[2], tid_BOOL,  "Button3Enabled" )
	SER_VALUE( m_dwButtonDefault[2], tid_BOOL,  "Button3Default" )
IMPLEMENT_SERIALIZABLE_END()
	
//--------------------------------------   ---------------------------------------
										   
enum enRdiskType						   
{										   
	eRdiskUnk              = 0x00000000,   
	eRdiskBartPE           = 0x00000001,   
	eRdiskWinPE            = 0x00000002,   
	eRdiskOther            = 0x00000003,
};

enum enRdiskTriggers
{
	wzRdiskBartPE_Prepare           = 0x00000001,
	wzRdiskBartPE_Prepare_Progress  = 0x00000002,
	wzRdiskBartPE_MakeIso           = 0x00000004,
	wzRdiskBartPE_MakeIso_Progress  = 0x00000008,
	wzRdiskBartPE_BurnCd            = 0x00000010,
	wzRdiskBartPE_BurnCd_Progress   = 0x00000020,
	
	wzRdiskWinPE_Prepare            = 0x00000040,
	wzRdiskWinPE_Prepare_Progress   = 0x00000080,
	wzRdiskWinPE_MakeIso            = 0x00000100,
	wzRdiskWinPE_MakeIso_Progress   = 0x00000200,
	
	wzRdiskOther_Prepare            = 0x00000400,
	wzRdiskOther_Prepare_Progress   = 0x00000800,
	
	wzRdisk_Progress                = wzRdiskBartPE_Prepare_Progress|wzRdiskBartPE_MakeIso_Progress|wzRdiskBartPE_BurnCd_Progress|wzRdiskWinPE_Prepare_Progress|wzRdiskWinPE_MakeIso_Progress|wzRdiskOther_Prepare_Progress,
};

struct cMakeRescueDisk : public cSerializable
{
	cMakeRescueDisk() :
		m_eType(eRdiskBartPE),
		m_eTriggers((enRdiskTriggers)0),
		m_Prepare(cTRUE),
		m_MakeIso(cTRUE),
		m_BurnIso(cTRUE),
		m_EraseDisk(cFALSE),
		m_bProcessing(cFALSE),
		m_UseIAMT(cTRUE)
	{}
	
	DECLARE_IID( cMakeRescueDisk, cSerializable, PID_GUI, gst_cMakeRescueDisk );

	enRdiskType        m_eType;
	enRdiskTriggers    m_eTriggers;
	cStringObj         m_strBartDestPath;
	cStringObj         m_strWinDestPath;
	cStringObj         m_strOtherDestPath;
	cStringObj         m_strBartToolPath;
	cStringObj         m_strWinToolPath;
	cStringObj         m_strXPCDPath;
	cStringObj         m_strBartIsoFile;
	cStringObj         m_strWinIsoFile;
	tBOOL              m_Prepare;
	tBOOL              m_MakeIso;
	tBOOL              m_BurnIso;
	tBOOL              m_EraseDisk;
	cStringObj         m_ScsiBurnDeviceId;
	tBOOL              m_bProcessing;
	tBOOL              m_UseIAMT;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cMakeRescueDisk, 0 )
	SER_VALUE_M( eType,                 tid_DWORD)
	SER_VALUE_M( eTriggers,             tid_DWORD)
	SER_VALUE_M( strBartDestPath,       tid_STRING_OBJ)
	SER_VALUE_M( strWinDestPath,        tid_STRING_OBJ)
	SER_VALUE_M( strOtherDestPath,      tid_STRING_OBJ)
	SER_VALUE_M( strBartToolPath,       tid_STRING_OBJ)
	SER_VALUE_M( strWinToolPath,        tid_STRING_OBJ)
	SER_VALUE_M( strXPCDPath,           tid_STRING_OBJ)
	SER_VALUE_M( strBartIsoFile,        tid_STRING_OBJ)
	SER_VALUE_M( strWinIsoFile,         tid_STRING_OBJ)
	SER_VALUE_M( Prepare,               tid_BOOL)
	SER_VALUE_M( MakeIso,               tid_BOOL)
	SER_VALUE_M( BurnIso,               tid_BOOL)
	SER_VALUE_M( EraseDisk,             tid_BOOL)
	SER_VALUE_M( ScsiBurnDeviceId,      tid_STRING_OBJ)
	SER_VALUE_M( bProcessing,           tid_BOOL)
	SER_VALUE_M( UseIAMT,            tid_BOOL)
IMPLEMENT_SERIALIZABLE_END()
										   
//-----------------------------------------------------------------------------

struct cGuiLicCheckKeyInfo : public cSerializable
{
	cGuiLicCheckKeyInfo() : m_errCheck(errOK) {}
	
	DECLARE_IID( cGuiLicCheckKeyInfo, cSerializable, PID_GUI, gst_cGuiLicCheckKeyInfo );
	
	cCheckInfo  m_LicInfo;
	tERROR      m_errCheck;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiLicCheckKeyInfo, 0 )
	SER_VALUE_M( LicInfo,  cCheckInfo::eIID )
	SER_VALUE_M( errCheck, tid_ERROR )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cConfigureProductDiscartedKeys : public cSerializable
{
	DECLARE_IID( cConfigureProductDiscartedKeys, cSerializable, PID_BL, gst_cConfigureProductDiscartedKeys );

	cVector<cGuiLicCheckKeyInfo> m_aKeys;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cConfigureProductDiscartedKeys, 0 )
	SER_VECTOR_M(aKeys,    cGuiLicCheckKeyInfo::eIID)
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cGuiUrlFltProfile : public cUrlFltProfile
{
	cGuiUrlFltProfile() : m_bDefault(cFALSE) {}
	
	DECLARE_IID( cGuiUrlFltProfile, cUrlFltProfile, PID_GUI, gst_cGuiUrlFltProfile );

	tBOOL	m_bDefault;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiUrlFltProfile, 0 )
	SER_BASE( cUrlFltProfile, 0 )
	SER_VALUE( m_bDefault,   tid_BOOL, "Default" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cDumpFile : public cSerializable
{
	DECLARE_IID( cDumpFile, cSerializable, PID_GUI, gst_cDumpFile );

	cDumpFile(): m_dwSize(0), m_bSend(cTRUE) {}

	tBOOL      m_bSysInfo;
	cStringObj m_strPath;
	tDWORD     m_dwSize;
	tDWORD     m_dwPackSize;
	tBOOL      m_bSend;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cDumpFile, 0 )
	SER_VALUE( m_bSysInfo, tid_BOOL,       "SysInfo" )
	SER_VALUE( m_strPath,  tid_STRING_OBJ, "Path" )
	SER_VALUE( m_dwSize,   tid_DWORD,      "Size" )
	SER_VALUE( m_dwPackSize, tid_DWORD,    "PackSize" )
	SER_VALUE( m_bSend,    tid_BOOL,       "Send" )
IMPLEMENT_SERIALIZABLE_END()

struct cDumpFiles : public cSerializable
{
	DECLARE_IID( cDumpFiles, cSerializable, PID_GUI, gst_cDumpFiles );
	
	cDumpFiles() : m_bSendDumps(cTRUE) {}

	tBOOL				m_bSendDumps;
	cVector<cDumpFile>  m_Dumps;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cDumpFiles, 0 )
    SER_VALUE ( m_bSendDumps,	tid_BOOL,		"SendDumps" )
	SER_VECTOR( m_Dumps,		cDumpFile::eIID,"Dumps" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cGuiAppGroup : public cAppGroup
{
	cGuiAppGroup() : m_pApp(NULL), m_pRule(NULL), m_nState(0) {}

	DECLARE_IID( cGuiAppGroup, cAppGroup, PID_GUI, gst_cGuiAppGroup );

	tDWORD          m_nResId;
	tDWORD          m_nState;
	CHipsAppItem *  m_pApp;
	CHipsRuleItem * m_pRule;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiAppGroup, 0 )
	SER_BASE( cAppGroup, 0 )
	SER_VALUE( m_nResId,    tid_DWORD,           "ResId" )
	SER_VALUE( m_nState,    tid_DWORD,           "State" )
	SER_VALUE_PTR( m_pApp,  CHipsAppItem::eIID,  "app" )
	SER_VALUE_PTR( m_pRule, CHipsRuleItem::eIID, "rule" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

//struct cGuiResGroup : public CHipsResourceGroupsItem
//{
//	DECLARE_IID( cGuiResGroup, CHipsResourceGroupsItem, PID_GUI, gst_cGuiResGroup );
//
//	CHipsRuleItem m_Rule;
//};
//
//IMPLEMENT_SERIALIZABLE_BEGIN( cGuiResGroup, 0 )
//	SER_BASE( CHipsResourceGroupsItem, 0 )
//	SER_VALUE( m_Rule, CHipsRuleItem::eIID, "rule" )
//IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cGuiResRule : public cResource
{
	DECLARE_IID( cGuiResRule, cResource, PID_GUI, gst_cGuiResRule );

	CHipsRuleItem m_Rule;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiResRule, 0 )
	SER_BASE( cResource, 0 )
	SER_VALUE( m_Rule, CHipsRuleItem::eIID, "Rule" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cGuiAvzScript : public cEnabledStrItem
{
	cGuiAvzScript() : m_nResult(0), m_bRevertable(cFALSE) {}

	DECLARE_IID( cGuiAvzScript, cEnabledStrItem, PID_GUI, gst_cGuiAvzScript );

	cStringObj      m_sDescription;
	cStringObj      m_sSideEffect;
	tDWORD          m_nResult;
	tBOOL           m_bRevertable;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiAvzScript, 0 )
	SER_BASE( cEnabledStrItem, 0 )
	SER_VALUE( m_sDescription, tid_STRING_OBJ,    "Description" )
	SER_VALUE( m_sSideEffect,  tid_STRING_OBJ,    "SideEffect" )
	SER_VALUE( m_nResult,      tid_DWORD,         "Result" )
	SER_VALUE( m_bRevertable,  tid_BOOL,          "Revertable" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cGuiScanStatistic : public cSerializable
{
	cGuiScanStatistic() { memset(&m_nObjectType, 0, sizeof(*this) - offsetof(cGuiScanStatistic, m_nObjectType)); }

	DECLARE_IID( cGuiScanStatistic, cSerializable, PID_GUI, gst_cGuiScanStatistic );

	tDWORD  m_nObjectType;
	tQWORD  m_nObjectId;

	tDWORD  m_nFlags;
	tLONG   m_nNumScaned;
	tLONG   m_nNumDetected;
	tLONG   m_nNumNotProcessed;
	tLONG   m_nNumArchived;
	tLONG   m_nNumPacked;
	tLONG   m_nNumPswProtected;
	tLONG   m_nNumCorrupted;
	tLONG   m_nNumScanErrors;
	tQWORD  m_nTimeSpend;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cGuiScanStatistic, 0 )
	SER_VALUE(m_nObjectType,      tid_DWORD,   "ObjectType")
	SER_VALUE(m_nObjectId,        tid_QWORD,   "ObjectId")
	SER_VALUE(m_nFlags,           tid_DWORD,   "Flags")
	SER_VALUE(m_nNumScaned,       tid_DWORD,   "Scaned")
	SER_VALUE(m_nNumDetected,     tid_DWORD,   "Detected")
	SER_VALUE(m_nNumNotProcessed, tid_DWORD,   "NotProcessed")
	SER_VALUE(m_nNumArchived,     tid_DWORD,   "Archived")
	SER_VALUE(m_nNumPacked,       tid_DWORD,   "Packed")
	SER_VALUE(m_nNumPswProtected, tid_DWORD,   "PswProtected")
	SER_VALUE(m_nNumCorrupted,    tid_DWORD,   "Corrupted")
	SER_VALUE(m_nNumScanErrors,   tid_DWORD,   "ScanErrors")
	SER_VALUE(m_nTimeSpend,       tid_QWORD,   "TimeSpend")
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cCmnStatisticItem : public cSerializable
{
	DECLARE_IID( cCmnStatisticItem, cSerializable, PID_GUI, gst_cCmnStatistic );

	cCmnStatisticItem(): m_Type(0), m_dwTotal(0), m_dwTreated(0) {}
	cCmnStatisticItem(tDWORD type, tDWORD total, tDWORD treated): m_Type(type), m_dwTotal(total), m_dwTreated(treated) {}

	tDWORD	m_Type;
	tDWORD	m_dwTotal;
	tDWORD	m_dwTreated;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cCmnStatisticItem, 0 )
	SER_VALUE( m_Type,		tid_DWORD,	"Type" )
	SER_VALUE( m_dwTotal,   tid_DWORD,	"Total" )
	SER_VALUE( m_dwTreated, tid_DWORD,	"Treated" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------



#endif//  __S_GUI_H
