#ifndef __S_URL_FILTERING_H_
#define __S_URL_FILTERING_H_

// message classes
#define pmc_URL_FILTER_ANTIPHISHING 0xe7928d7d // == pmc_ANTI_PHISHING_TASK
#define pmc_URL_FILTER_ADBLOCK      0xe8928d7d
#define pmc_URL_FILTER_WEBAV        0xe9928d7d
#define pmc_URL_FILTER_PARCTL       0xea928d7d

// messages
#define msg_URL_FILTER_CHECK_URL                 0xd369d708 // == msg_ANTI_PHISHING_CHECK_URL
#define msg_URL_FILTER_BLOCK_IF_NOT_WHITELISTED  0xd369d709 



#include <Prague/pr_cpp.h>
#include <Prague/pr_vector.h>

#include <AntiPhishing/structs/s_antiphishing.h>
#include <AV/structs/s_avs.h>
#include <ParentalControl/plugin/p_urlflt.h>
#include <ProductCore/structs/s_taskmanager.h>

//-----------------------------------------------------------------------------

enum
{
	PARCTL_PROFILEID_PARENT = 1,
	PARCTL_PROFILEID_TEEN = 2,
	PARCTL_PROFILEID_CHILD = 3
};

//-----------------------------------------------------------------------------

enum _urlflt_stid
{
	urlflt_begin_ = 0,
	
	urlflt_cUrlFltFilter,
	urlflt_cUrlFltProfile,
	urlflt_cUrlFltUserSettings,
	urlflt_cUrlFltAskAction,
	urlflt_cUrlFltReportEvent,
	urlflt_cUrlFltStatData,
	urlflt_cUrlFltProfileStat,
	urlflt_cUrlFltTimeRange,
	urlflt_cUrlFltProfileData,
	urlflt_cUrlFltCacheData,
	urlflt_cUrlFltPersistentProfileData,
	urlflt_cUrlFltPersistentCacheData,
	urlflt_cUrlFltFilterRequest
};

//-----------------------------------------------------------------------------

enum eUrlFltCategoryId
{
	eUrlFltCategoryIdPorno		= 1,
//	eUrlFltCategoryId			= 2,
	eUrlFltCategoryIdDrugs		= 3,
	eUrlFltCategoryIdViolence	= 4,
	eUrlFltCategoryIdUncens		= 5,
	eUrlFltCategoryIdWeapons	= 6,
	eUrlFltCategoryIdGambling	= 7,
	eUrlFltCategoryIdChat		= 8,
	eUrlFltCategoryIdWebMail	= 9,

	eUrlFltCategoryCount
};

enum eUrlFltCategory
{
	eUrlFltCategoryPorno		= 1 << (eUrlFltCategoryIdPorno - 1),
	eUrlFltCategoryDrugs		= 1 << (eUrlFltCategoryIdDrugs - 1),
	eUrlFltCategoryViolence		= 1 << (eUrlFltCategoryIdViolence - 1),
	eUrlFltCategoryUncens		= 1 << (eUrlFltCategoryIdUncens - 1),
	eUrlFltCategoryWeapons		= 1 << (eUrlFltCategoryIdWeapons - 1),
	eUrlFltCategoryGambling		= 1 << (eUrlFltCategoryIdGambling - 1),
	eUrlFltCategoryChat			= 1 << (eUrlFltCategoryIdChat - 1),
	eUrlFltCategoryWebMail		= 1 << (eUrlFltCategoryIdWebMail - 1),
};

//-----------------------------------------------------------------------------
// cUrlFltTimeRange

struct cUrlFltTimeRange : public cSerializable
{
	cUrlFltTimeRange(tDWORD nTimeFrom = 0, tDWORD nTimeTo = 0, tBOOL bEnabled = cTRUE) :
		m_bEnabled(bEnabled),
		m_nTimeFrom(nTimeFrom),
		m_nTimeTo(nTimeTo)
	{
	}

	///
	/// Compare two ranges
	///
	static int compare( const cUrlFltTimeRange& r1, const cUrlFltTimeRange& r2 )
	{
		int res = 0;

		if ( r1.m_nTimeFrom < r2.m_nTimeFrom )
			res = -1;
		else if ( r1.m_nTimeFrom > r2.m_nTimeFrom )
			res = 1;
		else // r1.m_nTimeFrom == r2.m_nTimeFrom
		{
			if ( r1.m_nTimeTo < r2.m_nTimeTo )
				res = -1;
			else if ( r1.m_nTimeTo > r2.m_nTimeTo )
				res = 1;
			// else r1.m_nTimeTo == r2.m_nTimeTo
			//	res = 0;
		}

		return res;
	}

	DECLARE_IID(cUrlFltTimeRange, cSerializable, PID_URLFLT, urlflt_cUrlFltTimeRange);

	tBOOL                               m_bEnabled;
	tDWORD                              m_nTimeFrom;
	tDWORD                              m_nTimeTo;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cUrlFltTimeRange, 0)
	SER_VALUE( m_bEnabled,              tid_BOOL,       "Enabled" )
	SER_VALUE( m_nTimeFrom,             tid_DWORD,      "TimeFrom" )
	SER_VALUE( m_nTimeTo,               tid_DWORD,      "TimeTo" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// cUrlFltFilter

struct cUrlFltFilter : public cSerializable
{
	cUrlFltFilter() :
		m_nDenyCategoriesMask(0)
	{
	}
	
	DECLARE_IID(cUrlFltFilter, cSerializable, PID_URLFLT, urlflt_cUrlFltFilter);

	cVector<cEnabledStrItem>	m_vWhiteList;
	cVector<cEnabledStrItem>	m_vBlackList;
	tQWORD						m_nDenyCategoriesMask;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cUrlFltFilter, 0)
	SER_VECTOR_STREAM(m_vWhiteList,			cEnabledStrItem::eIID,	"WhiteList")
	SER_VECTOR_STREAM(m_vBlackList,			cEnabledStrItem::eIID,	"BlackList")
	SER_VALUE(m_nDenyCategoriesMask,	    tid_QWORD,				"DenyCategoriesMask")
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// cUrlFltProfile

typedef cVector<cUrlFltTimeRange> tUrlFltTimeSpace;

struct cUrlFltProfile : public cEnabledStrItem
{
	cUrlFltProfile() :
		m_nId(0),
		m_bUseCommonFilter(cFALSE),
		m_nDayTimeLimit(0, cFALSE),
		m_nLevel(SETTINGS_LEVEL_DEFAULT),
		m_bDenyBadURL(cTRUE)
	{
	}
	
	DECLARE_IID(cUrlFltProfile, cEnabledStrItem, PID_URLFLT, urlflt_cUrlFltProfile);

	tDWORD				m_nId;
	cStrObj				m_strDescription;
	cUrlFltFilter		m_Filter;
	tBOOL				m_bUseCommonFilter;
	cStringObj          m_strPassword;

	tDWORD				m_nLevel;
	tBOOL				m_bDenyBadURL;

	tBOOL                       m_bUseDayTimeSpace;
	tUrlFltTimeSpace            m_aDayTimeSpace;
	cSettingsCheckT<__time32_t> m_nDayTimeLimit;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cUrlFltProfile, 0)
	SER_BASE(cEnabledStrItem, 0)
	SER_VALUE(m_nId,					tid_DWORD,				"Id")
	SER_VALUE(m_strDescription,			tid_STRING_OBJ,			"Description")
	SER_VALUE(m_Filter,					cUrlFltFilter::eIID,	"Filter")
	SER_VALUE(m_bUseCommonFilter,		tid_BOOL,				"UseCommonFilter")
	SER_VALUE(m_nLevel,					tid_DWORD,				"Level")
	SER_VALUE(m_bDenyBadURL,			tid_BOOL,				"DenyBadURL")
	SER_PASSWORD(m_strPassword,                                 "Password" )
	SER_VALUE(m_bUseDayTimeSpace,		tid_BOOL,               "UseDayTimeSpace")
	SER_VECTOR(m_aDayTimeSpace,	        cUrlFltTimeRange::eIID, "DayTimeSpace")
	SER_VALUE(m_nDayTimeLimit.m_on,		tid_BOOL,               "UseDayTimeLimit")
	SER_VALUE(m_nDayTimeLimit.m_val,	tid_DWORD,              "DayTimeLimit")
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// User Settings

struct cUrlFltUserSettings : public cSerializable
{
	cUrlFltUserSettings():
		m_nProfileId( -1 )
	{
	}

	DECLARE_IID(cUrlFltUserSettings, cSerializable, PID_URLFLT, urlflt_cUrlFltUserSettings);

	tDWORD				m_nProfileId;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cUrlFltUserSettings, 0)
	SER_VALUE(m_nProfileId,			tid_DWORD,			"ProfileId")
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// Settings

struct cUrlFltSettings : public cTaskSettingsPerUser
{
	cUrlFltSettings() :
		m_nFilterMsgClass(0),
		m_nCache(100),
		//m_nCacheTTL(15*24*60*60), // 15 days (in seconds)
		m_nCacheTTL( 60 * 60 ), // 1 hour = 60 * 60 (in seconds)
		m_nCacheErrTTL(15*60), // 15 minutes (in seconds)
		m_nMaxHeuristicDataSize(0x10000),
		m_nTranspMax(100),
		m_nTranspNetTimeout(15), // in seconds
		m_bHeuristic(cTRUE),
		m_bHeuristicOnly(cFALSE),
		m_bHeuristicContentDescr(cTRUE),
		m_bHeuristicContentUrls(cTRUE),
		m_bProfiles(cTRUE),
		m_bAskAction(cFALSE),
		m_bLoadHTTP(cFALSE),
		m_bCheckMainPage(cTRUE),
		m_bUseReportFile(cFALSE)
	{
	}
	
	DECLARE_IID_SETTINGS(cUrlFltSettings, cTaskSettingsPerUser, PID_URLFLT);
	
	tDWORD settings_byuser_getid() { return cUrlFltUserSettings::eIID; }

	cUrlFltProfile * get_profile(tDWORD nId)
	{
		for(size_t i = 0; i < m_aProfiles.size(); i++)
		{
			cUrlFltProfile& _p = m_aProfiles[i];
			if( _p.m_nId == nId )
				return &_p;
		}

		return NULL;
	}
	
	cStringObj					m_sDataBase;
	cStringObj					m_sDatabaseUpdCat;
	tDWORD						m_nFilterMsgClass; // use this class with msg_URL_FILTERING_CHECK_URL
	
	tBOOL						m_bHeuristic;	// use heuristic analyze
	tBOOL						m_bHeuristicOnly;	// use heuristic analyze only
	tBOOL						m_bHeuristicContentDescr;	// use heuristic analyze of content description, keywords, title
	tBOOL						m_bHeuristicContentUrls;	// use heuristic analyze of content URL's
	tDWORD						m_nCache;		// heuristic analyze session cache objects count
	tDWORD						m_nCacheTTL;	// heuristic analyze session cache object time-to-live
	tDWORD						m_nCacheErrTTL;	// heuristic bad analyze session cache object time-to-live
	tDWORD						m_nMaxHeuristicDataSize;	// heuristic analyze object buffer size
	tDWORD						m_nTranspMax;
	tDWORD						m_nTranspNetTimeout;

	tBOOL						m_bProfiles;	// use profiles subsystem
	cVector<cUrlFltProfile>		m_aProfiles;
	cUrlFltFilter				m_Filter;

	tBOOL						m_bAskAction;	// ask user action
	cAskObjectAction			m_AskActionDef;	// ask action defaults

	tBOOL						m_bLoadHTTP;         // load traffic monitor

	// Flag to indicate if it is required to check the main page along with 
	// the page is about to be processed
	tBOOL						m_bCheckMainPage;

	// Flag to indicate if it is required to report the results when 
	// URL- and content-analyze is performed
	tBOOL						m_bUseReportFile;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cUrlFltSettings, 0)
	SER_BASE(cTaskSettingsPerUser, 0)
	SER_VALUE(m_sDataBase,			tid_STRING_OBJ,			"DataBase")
	SER_VALUE(m_sDatabaseUpdCat,	tid_STRING_OBJ,			"UpdateCategory")
	SER_VALUE(m_nFilterMsgClass,	tid_DWORD,				"FilterMsgClass")
	SER_VALUE(m_bHeuristic,	        tid_BOOL,               "UseHeuristic")
	SER_VALUE(m_bHeuristicOnly,	    tid_BOOL,               "UseHeuristicOnly")
	SER_VALUE(m_bHeuristicContentDescr,	 tid_BOOL,          "UseHeuristicContentDescr")
	SER_VALUE(m_bHeuristicContentUrls,	 tid_BOOL,          "UseHeuristicContentUrls")
	SER_VALUE(m_nCache,	            tid_DWORD,              "CacheCount")
	SER_VALUE(m_nCacheTTL,	        tid_DWORD,              "CacheTTL")
	SER_VALUE(m_nCacheErrTTL,	    tid_DWORD,              "CacheErrTTL")
	SER_VALUE(m_nMaxHeuristicDataSize, tid_DWORD,           "MaxHeuristicDataSize")
	SER_VALUE(m_nTranspMax,         tid_DWORD,              "TranspMaxCount")
	SER_VALUE(m_nTranspNetTimeout,  tid_DWORD,              "TranspNetTimeout")
	SER_VALUE(m_bProfiles,	        tid_BOOL,               "UseProfiles")
	SER_VECTOR(m_aProfiles,			cUrlFltProfile::eIID,	"Profiles")
	SER_VALUE(m_Filter,				cUrlFltFilter::eIID,	"Filter")
	SER_VALUE(m_bAskAction,	        tid_BOOL,               "UseAskAction")
	SER_VALUE(m_AskActionDef,		cAskObjectAction::eIID,	"AskActionDef")
	SER_VALUE(m_bLoadHTTP,          tid_BOOL,               "LoadHTTPTrafMon" )
	SER_VALUE(m_bCheckMainPage,	    tid_BOOL,               "CheckMainPage")
	SER_VALUE(m_bUseReportFile,	    tid_BOOL,               "UseReportFile")
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// cUrlFltStatData

struct cUrlFltStatData : public cSerializable
{
	cUrlFltStatData() :
		m_nURLsChecked(0),
		m_nURLsDetected(0)
	{}

	DECLARE_IID(cUrlFltStatData, cSerializable, PID_URLFLT, urlflt_cUrlFltStatData);

	tDWORD             m_nURLsChecked;
	tDWORD             m_nURLsDetected;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cUrlFltStatData, 0)
	SER_VALUE_M(nURLsChecked,     tid_DWORD)
	SER_VALUE_M(nURLsDetected,    tid_DWORD)
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// Profile Statistics

struct cUrlFltProfileStat : public cSerializable
{
	cUrlFltProfileStat() : m_nProfileId(-1), m_nLevel(SETTINGS_LEVEL_DEFAULT) {}
	
	DECLARE_IID(cUrlFltProfileStat, cSerializable, PID_URLFLT, urlflt_cUrlFltProfileStat);

	tDWORD             m_nProfileId;
	cStrObj            m_strProfile;
	tDWORD             m_nLevel;
	cUrlFltStatData    m_Data;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cUrlFltProfileStat, 0)
	SER_VALUE_M(nProfileId,            tid_DWORD)
	SER_VALUE_M(strProfile,            tid_STRING_OBJ)
	SER_VALUE(m_nLevel,                tid_DWORD, "Level")
	SER_VALUE_NO_NAMESPACE(m_Data,     cUrlFltStatData::eIID, "Data")
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// Statistics

struct cUrlFltStatistics : public cTaskStatisticsPerUser
{
	cUrlFltStatistics() : m_nCurUserProfileId(-1) {}
	
	DECLARE_IID_STATISTICS(cUrlFltStatistics, cTaskStatisticsPerUser, PID_URLFLT);

	cUrlFltStatData             m_Data;
	tDWORD                      m_nCurUserProfileId;
	cVector<cUrlFltProfileStat> m_aProfiles;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cUrlFltStatistics, 0)
	SER_BASE(cTaskStatisticsPerUser, 0)
	SER_VALUE_NO_NAMESPACE(m_Data,     cUrlFltStatData::eIID, "Data")
	SER_VALUE_M(nCurUserProfileId,     tid_DWORD)
	SER_VECTOR(m_aProfiles,			   cUrlFltProfileStat::eIID, "ProfilesStat")
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// AskAction

struct cUrlFltAskAction : public cSerializable
{
	enum eAction{eUnk = -1, eSwitchProfile};

	cUrlFltAskAction(eAction nAction = eUnk) :
		m_nAction(nAction),
		m_nProfileId(0)
	{}

	DECLARE_IID(cUrlFltAskAction, cSerializable, PID_URLFLT, urlflt_cUrlFltAskAction);

	eAction            m_nAction;
	cStrObj            m_strUserId;
	tDWORD             m_nProfileId;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cUrlFltAskAction, 0)
	SER_VALUE_M(nAction,       tid_DWORD)
	SER_VALUE_M(strUserId,     tid_STRING_OBJ)
	SER_VALUE_M(nProfileId,    tid_DWORD)
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// ReportEvent

struct cUrlFltReportEvent : public cAskObjectAction
{
	enum eEvent{eUnk = -1, eProcess, eSwitchProfile, eHeuristicStatus};
	enum eDetectType{eDtNone, eDtTimeLimit, eDtWhiteList, eDtBlackList, eDtDatabase, eDtHeuristic};

	cUrlFltReportEvent(eEvent nEvent = eUnk) :
		m_nEvent(nEvent),
		m_nURLDetectType(eDtNone),
		m_nUfProfileId(-1)
	{}

	DECLARE_IID(cUrlFltReportEvent, cAskObjectAction, PID_URLFLT, urlflt_cUrlFltReportEvent);

	tDWORD             m_nSeverity;
	eEvent             m_nEvent;
	eDetectType        m_nURLDetectType;
	cStrObj            m_strUserId;
	cStrObj            m_strUfProfile;
	tDWORD             m_nUfProfileId;
	tERROR             m_ErrorCode;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cUrlFltReportEvent, 0)
	SER_BASE(cAskObjectAction, 0)
	SER_VALUE_M(nSeverity,         tid_DWORD)
	SER_VALUE_M(nEvent,            tid_DWORD)
	SER_VALUE_M(nURLDetectType,    tid_DWORD)
	SER_VALUE_M(strUserId,         tid_STRING_OBJ)
	SER_VALUE_M(strUfProfile,      tid_STRING_OBJ)
	SER_VALUE_M(nUfProfileId,      tid_DWORD)
	SER_VALUE_M(ErrorCode,         tid_ERROR)
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// cUrlFltProfileData

struct cUrlFltProfileData : public cSerializable
{
	cUrlFltProfileData() :
		m_nId(-1),
		m_tmLastQuery(0),
		m_nCurDayTime(0),
		m_nDayTimeLimit(0xFFFFFFFF)
	{}

	DECLARE_IID(cUrlFltProfileData, cSerializable, PID_URLFLT, urlflt_cUrlFltProfileData);

	tDWORD    m_nId;
	time_t    m_tmLastQuery;
	tDWORD    m_nCurDayTime;
	tDWORD    m_nDayTimeLimit;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cUrlFltProfileData, 0)
	SER_VALUE(m_nId,           tid_DWORD, "Id")
	SER_VALUE(m_tmLastQuery,   tid_DWORD, "LastQueryTime")
	SER_VALUE(m_nCurDayTime,   tid_DWORD, "CurDayTime")
	SER_VALUE(m_nDayTimeLimit, tid_DWORD, "DayTimeLimit")
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// cUrlFltCacheData

struct cUrlFltCacheData : public cSerializable
{
	enum Flags{fErr = 0x01, fErrDeny = 0x02, fAnalyzed = 0x04};
	
	cUrlFltCacheData() :
		m_nTime(0),
		m_nFlags(0)
	{}

	DECLARE_IID(cUrlFltCacheData, cSerializable, PID_URLFLT, urlflt_cUrlFltCacheData);

	cVector<tCHAR> m_strUrl;
	time_t         m_nTime;
	cVector<tWORD> m_Res;
	tBYTE          m_nFlags;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cUrlFltCacheData, 0)
	SER_VECTOR(m_strUrl, tid_CHAR,       "Url")
	SER_VALUE(m_nTime,   tid_DWORD,      "Time")
	SER_VECTOR(m_Res,	 tid_WORD,       "Res")
	SER_VALUE(m_nFlags,  tid_BYTE,       "Flags")
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// cUrlFltPersistentProfileData

struct cUrlFltPersistentProfileData : public cSerializable
{
	DECLARE_IID(cUrlFltPersistentProfileData, cSerializable, PID_URLFLT, urlflt_cUrlFltPersistentProfileData);

	cVector<cUrlFltProfileData> m_aProfiles;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cUrlFltPersistentProfileData, 0)
	SER_VECTOR_STREAM(m_aProfiles,	cUrlFltProfileData::eIID,	"Profiles")
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// cUrlFltPersistentCacheData

struct cUrlFltPersistentCacheData : public cSerializable
{
	DECLARE_IID(cUrlFltPersistentCacheData, cSerializable, PID_URLFLT, urlflt_cUrlFltPersistentCacheData);

	cVector<cUrlFltCacheData>   m_aCache;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cUrlFltPersistentCacheData, 0)
	SER_VECTOR_STREAM(m_aCache,		cUrlFltCacheData::eIID,	"Cache")
IMPLEMENT_SERIALIZABLE_END()


//-----------------------------------------------------------------------------
// Forward declaration
namespace http
{
	class message;
}

//-----------------------------------------------------------------------------
// cUrlFilteringRequest

struct cUrlFilteringRequest : public cSerializable
{
	cUrlFilteringRequest(): 
		m_sURL(), 
		m_nPID(), 
		m_ContentType(), 
		m_StatusCode(),
		m_ContentEncoding(),
		m_CacheControl()
	{
	}

    DECLARE_IID(cUrlFilteringRequest, cSerializable, PID_URLFLT, urlflt_cUrlFltFilterRequest);

	///
	/// Extract response information from the given message
	///
	/// @param msg - message is about to be analyzed
	/// @param info - response information
	///
	void init( const http::message* const msg );

    cStringObj        m_sURL;
	tDWORD            m_nPID;
	// http::contenttype
	tBYTE			  m_ContentType;
	// http::response_message::status_code
	tDWORD			  m_StatusCode;
	// http::content_encoding
	tBYTE			  m_ContentEncoding;
	// http::cache_control
	tBYTE			  m_CacheControl;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cUrlFilteringRequest, 0 )
    	SER_VALUE(          m_sURL,				tid_STRING_OBJ,		"URL" )
    	SER_VALUE(          m_nPID,				tid_DWORD,			"PID" )
		SER_VALUE(          m_ContentType,		tid_BYTE,			"ContentType" )
		SER_VALUE(          m_StatusCode,		tid_DWORD,			"StatusCode" )
		SER_VALUE(          m_ContentEncoding,  tid_BYTE,			"ContentEncoding" )
		SER_VALUE(          m_CacheControl,		tid_BYTE,			"CacheControl" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


#endif __S_URL_FILTERING_H_
