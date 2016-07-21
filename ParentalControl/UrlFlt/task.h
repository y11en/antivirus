// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  04 April 2007,  19:51 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Nechaev
// File Name   -- task.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __task_cpp__814c5c2a_17fc_4717_8b6b_7cac387c15db )
#define __task_cpp__814c5c2a_17fc_4717_8b6b_7cac387c15db
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_reg.h>
#include <Prague/iface/i_root.h>

#include <ParentalControl/plugin/p_urlflt.h>
// AVP Prague stamp end


#include <Prague/iface/i_csect.h>
#include <Prague/iface/i_threadpool.h>
#include <Prague/iface/i_token.h>

#include <ParentalControl/structs/s_urlflt.h>

#include "..\..\Components\Updater\Transport\PragueTransport\i_transport.h"
#include "..\..\Components\Updater\Transport\ProxyServerAddressDetector\i_proxyserveraddressdetector.h"

#include "db.h"


#define prMAKEWORD(h, l)      ((tWORD)(((tBYTE)((tDWORD)(l) & 0xff)) | ((tWORD)((tBYTE)((tDWORD)(h) & 0xff))) << 8))
#define prLOBYTE(w)           ((tBYTE)((tDWORD)(w) & 0xff))
#define prHIBYTE(w)           ((tBYTE)((tDWORD)(w) >> 8))


//////////////////////////////////////////////////////////////////////////

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable UrlFlt : public cTask 
{
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations

public:
// External function declarations
	tERROR pr_call SetSettings( const cSerializable* p_settings );
	tERROR pr_call GetSettings( cSerializable* p_settings );
	tERROR pr_call AskAction( tActionId p_actionId, cSerializable* p_actionInfo );
	tERROR pr_call SetState( tTaskRequestState p_state );
	tERROR pr_call GetStatistics( cSerializable* p_statistics );

// Data declaration
	hTASKMANAGER m_hTM;            // --
	tTaskState   m_nTaskState;     // --
	tDWORD       m_nTaskSessionId; // --
	tDWORD       m_nTaskId;        // --
	tDWORD       m_nTaskParentId;  // --
	hREGISTRY    m_hPersData;            // --
// AVP Prague stamp end

public:

	///
	/// tEnabledStrItemsList struct
	///
	struct tEnabledStrItemsList: public CUrlListCache::tExtList
	{
		tEnabledStrItemsList(cVector<cEnabledStrItem>& _p) : m_p(_p) { s_strobj = 1; }

		tDWORD	count() const { return m_p.size(); }

		void	item(tDWORD idx, CUrlListCache::tExtListItem& item) const
		{
			const cEnabledStrItem& _i = m_p[idx];
			item.s_name = (tPTR)&_i.m_sName;
			item.s_len = _i.m_sName.size();
			item.s_data = 0;
			item.s_enabled = _i.m_bEnabled;
		}

		cVector<cEnabledStrItem>& m_p;
	};

	///
	/// tUser struct
	///
	struct tUser
	{
		tUser() : m_nProfileId(-1), m_bFixProfile(0) {}
		
		cStrObj		m_user;
		tDWORD		m_nProfileId;
		unsigned	m_bFixProfile : 1;
	};
	typedef cVectorSimple(tUser)	tUsers;

	///
	/// tRefObj struct
	///
	struct tRefObj
	{
	public:
		tRefObj() : m_ref(1) {}
		virtual ~tRefObj() {}
		
		tLONG m_ref;

		void addRef() { PrInterlockedIncrement(&m_ref); }
		void release() { if( !PrInterlockedDecrement(&m_ref) ) delete this; }
	};

	///
	/// tProfile struct
	///
	struct tProfile: public tRefObj
	{
	public:
		static tProfile * _new(tDWORD nId = 0) { return new tProfile(nId); }
		static void _clear_a(cVectorSimple(tProfile *)& _a) { for(tDWORD i = 0; i < _a.size(); i++) _a[i]->release(); _a.clear(); }

	public:
		tProfile(tDWORD nId = 0) :
			m_nId(nId), m_bUseCommonFilter(0), m_nLevel(SETTINGS_LEVEL_DEFAULT),
			m_LastQueryTime(0), m_nDayTime(0),
			m_nDayTimeLimit(0xFFFFFFFF),
			m_bUseDayTimeSpace( 0 ),
			m_bUseDayTimeLimit( 0 )
		{
			m_urlsWhite.owner() = m_urlsBlack.owner() = (hOBJECT)g_root;
		}

		tERROR Build( hOBJECT tracer, cUrlFltFilter& _flt )
		{
			tERROR err = m_urlsWhite.Build( tracer, tEnabledStrItemsList( _flt.m_vWhiteList ) );
			if( PR_FAIL(err) )
				return err;

			err = m_urlsBlack.Build( tracer, tEnabledStrItemsList( _flt.m_vBlackList ) );
			if( PR_FAIL(err) )
				return err;

			m_nDenyCategoriesMask = _flt.m_nDenyCategoriesMask;
			return errOK;
		}

		tERROR CheckUrl(const CUrlListCache::tURL& p_url, ReportInfo& info ) const
		{
			info.SetPrefix( "Profile: white list: %s => ", p_url.m_url );
			bool found = m_urlsWhite.Find( p_url, info );
			info.ClearPrefix();

			if( found )
			{
				return errOK_DECIDED;
			}

			info.SetPrefix( "Profile: black list: %s => ", p_url.m_url );
			found = m_urlsBlack.Find( p_url, info );
			info.ClearPrefix();

			if ( found )
			{
				return errACCESS_DENIED;
			}

			return errOK;
		}

		void	Clear()
		{
			m_urlsWhite.Free();
			m_urlsBlack.Free();
		}

		tERROR	DataSet(cUrlFltProfileData& _d, bool bTo)
		{
			if( bTo )
			{
				_d.m_nId = m_nId;
				_d.m_tmLastQuery = m_LastQueryTime;
				_d.m_nCurDayTime = m_nDayTime;
				_d.m_nDayTimeLimit = m_nDayTimeLimit;
			}
			else
			{
				m_nId = _d.m_nId;
				m_LastQueryTime = _d.m_tmLastQuery;
				m_nDayTime = _d.m_nCurDayTime;
				m_nDayTimeLimit = _d.m_nDayTimeLimit;
			}
		
			return errOK;
		}

		tDWORD						m_nId;
		cStrObj						m_strName;
		tDWORD						m_nLevel;
		unsigned					m_bUseCommonFilter : 1;
		unsigned					m_bDenyDetected : 1;

		CUrlListCache				m_urlsWhite;
		CUrlListCache				m_urlsBlack;
		tQWORD						m_nDenyCategoriesMask;

		tDWORD						m_bUseDayTimeSpace : 1;
		tUrlFltTimeSpace			m_nDayTimeSpace;
		unsigned					m_bUseDayTimeLimit : 1;
		tDWORD						m_nDayTimeLimit;

		time_t						m_LastQueryTime;
		tDWORD						m_nDayTime;
		
		cUrlFltStatData				m_stat;
	};
	typedef cVectorSimple(tProfile *)	tProfiles;

	///
	/// tCasheObj struct
	///
	struct tCasheObj
	{
		tCasheObj(): 
			m_err(0),
			//m_err_deny(0) {}
			m_analyzed(0),
			m_URLDetectType(0)
		{
		}
		
		tERROR	DataSet(cUrlFltCacheData& _d, bool bTo)
		{
			if( bTo )
			{
				{
					_d.m_strUrl.resize(m_url.size());
					if( _d.m_strUrl.size() != m_url.size() )
						return errNOT_ENOUGH_MEMORY;

					if( _d.m_strUrl.size() )
						memcpy(&_d.m_strUrl[0], m_url.c_str(), _d.m_strUrl.size());
				}
				
				{
					tDWORD i = 0, n = m_dbRes.size();

					_d.m_Res.resize(n);
					if( _d.m_Res.size() != n )
						return errNOT_ENOUGH_MEMORY;
					
					for(; i < n; i++)
					{
						CParctlDb::ResultCatg& _c = m_dbRes[i];
						_d.m_Res[i] = prMAKEWORD(_c.m_id, _c.m_weight <= 100 ? _c.m_weight : 100);
					}
				}

				_d.m_nTime = m_time;
				//_d.m_nFlags = (m_err ? cUrlFltCacheData::fErr : 0)|(m_err_deny ? cUrlFltCacheData::fErrDeny : 0);
				_d.m_nFlags = 
					( m_err ? cUrlFltCacheData::fErr : 0 ) 
					|
					( m_analyzed ? cUrlFltCacheData::fAnalyzed : 0 );
			}
			else
			{
				{
					m_url.resize(_d.m_strUrl.size());
					if( m_url.size() != _d.m_strUrl.size() )
						return errNOT_ENOUGH_MEMORY;
					
					if( m_url.size() )
						memcpy(&m_url[0], &_d.m_strUrl[0], m_url.size());
				}
				
				{
					tDWORD i = 0, n = _d.m_Res.size();

					m_dbRes.resize(n);
					if( m_dbRes.size() != n )
						return errNOT_ENOUGH_MEMORY;

					for(; i < n; i++)
					{
						CParctlDb::ResultCatg& _c = m_dbRes[i];
						tWORD& _w = _d.m_Res[i];
						_c.m_id = prHIBYTE(_w);
						_c.m_weight = prLOBYTE(_w);
					}
				}

				m_time = _d.m_nTime;
				m_err = _d.m_nFlags & cUrlFltCacheData::fErr;
				//m_err_deny = _d.m_nFlags & cUrlFltCacheData::fErrDeny;
				m_analyzed = _d.m_nFlags & cUrlFltCacheData::fAnalyzed;
			}
		
			return errOK;
		}

		std::string			m_url;
		CParctlDb::Result	m_dbRes;
		time_t				m_time;
		unsigned			m_err : 1;
		//unsigned			m_err_deny : 1;
		unsigned			m_analyzed : 1;
		unsigned			m_URLDetectType;
	};
	typedef cVectorSimple(tCasheObj)	tCasheObjs;

	///
	/// Results of searching in cache
	///
	//enum eCacheRes {eCrProcess, eCrProcessWait, eCrFound};
	enum eCacheRes 
	{ 
		eCacheNone,			// Item is not found
		eCacheFound,		// Item is found
		eCacheNotAnalyzed	// Item is found but it was not analyzed before placing in cache
	};
	
	///
	/// tHeurObj struct
	///
	struct tHeurObj: public tRefObj
	{
		static tHeurObj * _new() { return new tHeurObj(); }
		
		tHeurObj() : m_complete(0), m_deny(0) {}
		
		std::string			m_url;
		CParctlDb::Result	m_dbRes;
		volatile unsigned	m_complete : 1;
		unsigned			m_deny : 1;
	};
	typedef cVectorSimple(tHeurObj *)	tHeurObjs;

	///
	/// tTrObj struct
	///
	struct tTrObj
	{
		tTrObj() : m_used(1) {}

		cAutoObj<cTransport>	m_tr;
		volatile unsigned		m_used : 1;
	};
	typedef cVectorSimple(tTrObj)	tTrObjs;

	///
	/// tHeurPrm struct
	///
	struct tHeurPrm
	{
		tHeurPrm() : m_fHeuristicContentDescr(1), m_fHeuristicContentUrls(1) {}
		
		tDWORD		m_nMaxDataSize;
		tDWORD		m_nTranspNetTimeout;
		cStrObj		m_strTranspUserAgent;
		unsigned	m_fHeuristicContentDescr : 1;
		unsigned	m_fHeuristicContentUrls : 1;
	};

public:

	UrlFlt();
	
	tERROR		Reinit(cUrlFltSettings * p_settings);
	tERROR		ProcessObject(cUrlFilteringRequest * pUrlFlt, hIO content);
	tERROR		SwitchProfile(const cStrObj& strUserId, tDWORD nProfileId);
	bool		CheckProxyChanged();
	tERROR		IdleWork(tDWORD counter);
	tERROR		DataProfilesSet(bool bWrite);
	tERROR		DataCacheSet(bool bWrite);
    // lockLoadBasesMutex [in] - indicates whether bases mutex shold be locked,
     // Note: it should not be locked if pm_COMPONENT_PREINSTALLED message received, because updater locks the mutex itself
	tERROR		_ReloadDb(const cStrObj& strDbFile, const bool lockLoadBasesMutex);
	tERROR		_GetUserByPID(tULONG nPID, cToken ** pToken, cStrObj& szUser, tDWORD& nType);
	void		_Clear();
	void		_ClearHeurCache();

public:

	tUser *		_GetUser(const cStrObj& strUserId, bool bCreate = false, tDWORD * pPos = NULL);
	tProfile *	_GetProfile(tDWORD nId, bool bCreate = false, tDWORD * pPos = NULL);
	tProfile *	_GetUserProfile(const cStrObj& strUserId);
	bool		_CheckTimeLimit(tProfile * _profile);

	///
	/// Perform heuristic analyze
	///
	tERROR _HeuristicAnalyze
		(
			const cStrObj& strSiteURL, 
			hIO content, 
			const cUrlFilteringRequest& request,
			tHeurPrm& _Prm, 
			//cToken * pToken, 
			CParctlDb::SearchResult& searchRes
		);

	tERROR		_GetHeuristicErrorImportance(tERROR errHeuristic);

private:

	///
	/// Perform transport-related objects initialization
	///
	tERROR		_TrMgr_Init();

	///
	/// Perform transport-related objects cleaning
	///
	void		_TrMgr_DeInit();

	///
	/// Get transport object for use
	///
	//tERROR		_TrMgr_GetObj(cTransport ** ppTransp, tDWORD nTranspNetTimeout, const cStrObj& strUserAgent);
	tERROR		_TrMgr_GetObj(cTransport ** ppTransp );

	///
	/// Release transport object when there is no need for it
	///
	void		_TrMgr_ReleaseObj(cTransport * pTransp);

	///
	/// Perform emergency cleaning of transport-related objects
	///
	void		_TrMgr_AbortJobs();

private:

	///
	/// Perform processing of the given content
	///
	tERROR DoProcessing
		(
			const cUrlFilteringRequest* pUrlFlt, 
			hIO content,
			const tProfile* _profile,
			const tQWORD& nDenyCategoriesMask,
			cToken* pToken,
			cUrlFltReportEvent& _Evt
		);

	///
	/// Analyze the given content
	///
	tERROR AnalyzeObject
		( 
			const cUrlFilteringRequest* pUrlFlt, 
			hIO content,
			const tProfile* _profile,
			const tQWORD& nDenyCategoriesMask,
			const CUrlListCache::tURL& url,
			cUrlFltReportEvent& _Evt,
			CParctlDb::Result& _dbRes
		);

private:

	///
	/// Check the given URL in cache
	///
	/// @param strFullURL - full URL
	/// @param _Evt - event with results of checking
	/// @param errCode - error code
	/// @param _dbRes - output category results for the given URL
	/// @param analyzed - true if the given item was analyzed, 
	///		false - the item was not analyzed (any error occurred)
	/// @param lock - true if the call should be performed under lock
	/// @return true if the given URL is found in cache, false - otherwise
	///
	eCacheRes findInCache
		( 
			const cStrObj& strFullURL,
			cUrlFltReportEvent& _Evt,
			tERROR& err,
			CParctlDb::Result& _dbRes,
			bool lock
		);

	///
	/// Add the given URL to the cache
	///
	/// @param strFullURL - full URL
	/// @param _Evt - event with results of checking
	/// @param errCode - error code
	/// @param _dbRes - output category results for the given URL
	/// @param analyzed - true if the given item was analyzed, 
	///		false - the item was not analyzed (any error occurred)
	///
	void addToCache
		( 
			const cStrObj& strFullURL,
			cUrlFltReportEvent& _Evt,
			tERROR& err,
			CParctlDb::Result& _dbRes,
			bool analyzed
		);

	///
	/// Check if there are ancient records in the cache is about to be removed
	///
	tERROR checkForCacheCleaning( tDWORD counter );

private:

	///
	/// Convert the given URL to the standard appearance
	///
	/// @param sURL - given URL is about to be converted
	/// @param buf - string buffer
	/// @param url - output url with standard appearance
	/// @param resolve - true if it is required to resolve IP address when given
	/// @return error code
	///
	tERROR normalizeUrl
		( 
			const cStrObj& sURL, 
			cStrBuff& buf, 
			CUrlListCache::tURL& url,
			bool checkForIp
		);

private:

	///
	/// Perform analyzing of given the description words
	///
	void analyzeWords
		( 
			const std::vector< std::wstring >& descriptionWords,
			CParctlDb::SearchResult& searchRes
		);

private:

	///
	/// Join the currently accumulated results with the main results
	///
	/// @param dbSubRes - currently accumulated results
	/// @param stats - statistics for currently accumulated results
	/// @param dbRes - main results
	/// @param totalCount - total count of processed objects to accumulate results
	///
	void joinDBResults
		( 
			CParctlDb::Result& dbSubRes, 
			const std::vector< int >& stats, 
			CParctlDb::Result& dbRes,
			int totalCount,
			ReportInfo& info
		);

	///
	/// Analyze the given results to make the final decision
	///
	/// @param _dbRes - category results for checking
	/// @param nDenyCategoriesMask - category masks
	///
	tERROR checkDbRes
		( 
			const CParctlDb::Result& _dbRes, 
			const tQWORD& denyCategoriesMask 
		);

	///
	/// Join database results to get the best possible one
	///
	/// @param dbRes1 - category results
	/// @param dbRes2 - category results
	///
	void bestDbRes( const CParctlDb::Result& dbRes1, CParctlDb::Result& dbRes2 );

private:

	///
	/// Update detection statistics
	///
	/// @param err - detection error code
	/// @param evt - detection event
	/// @param profile - current profile to be updated
	///
	void updateStats
		( 
			tDWORD err, 
			const cUrlFltReportEvent& evt, 
			tProfile* profile 
		);

private:

	///
	/// Get severity code 
	///
	/// @param err - detection error code
	/// @param evt - detection event
	///
	tDWORD GetSeverity( tDWORD errCode, const cUrlFltReportEvent& evt ) const;

private:

	///
	/// Load the content of the given URL
	///
	/// @param urlFlt - URL request information is about to be retrieved
	/// @param pContent - content of the page
	///
	tERROR loadPage( cUrlFilteringRequest& urlFlt, hIO* pContent, cToken* pToken );

private:

	///
	/// Find the given URL in currently processed URL's
	///
	/// @param url - the given URL
	/// @param lock - true if the call should be under lock
	///
	bool _findInProc( const cStrObj& url, bool lock );

	///
	/// Find the given URL in currently processed URL's
	///
	/// @param url - the given URL
	///
	bool findInProc( const cStrObj& url );

	///
	/// Mark the given URL as currently being processed
	///
	/// @param url - the given URL
	///
	void addToProc( const cStrObj& url );

	///
	/// Mark the given URL as currently not being in process
	///
	/// @param url - the given URL
	///
	void removeFromProc( const cStrObj& url );

	///
	/// Check if the given URL is currently in processing; if so, 
	/// wait until it is being processed
	///
	void checkInProc( const cStrObj& url );

public:
	hCRITICAL_SECTION	m_hCS;

	/*
	hCRITICAL_SECTION	m_dbCS;
	CParctlDb			m_db;
	*/
	ParCtlDbManager m_dbManager;

	// Cached requests
	tCasheObjs			m_cache;
	hCRITICAL_SECTION	m_cacheCS;

	/*
	// Pending requests for heuristic analyze
	hCRITICAL_SECTION	m_heurObjsCS;
	tHeurObjs			m_heurObjs;
	*/
	// Cache of URL's that are currently being processed
	// It is important to mark requests as "being processed" especially for
	// slow routines (for instance, that could spend enough time while retrieving 
	// the page with fault). In that case it will help to reduce amount of 
	// requests for this page (all other requests will use the cooked results 
	// from cache) and reduce total amount of time spending while request 
	// is being in process.
	typedef std::vector< cStrObj > ProcCache;
	struct ProcCacheType
	{
		///
		/// Constructor
		///
		ProcCacheType();

		ProcCache cache;
		hCRITICAL_SECTION cs;
	} m_procCache;

	tDWORD				m_nCache;
	tDWORD				m_nCacheTTL;
	tDWORD				m_nCacheErrTTL;
	tDWORD				m_nMaxHeuristicDataSize;
	hOBJECT				m_hHTTP;
	time_t				m_SystemStartTime;
	tDWORD				m_TimeZone;
	cProxySettings		m_ProxySettings;
	
	tUsers				m_users;
	tUser				m_defUser;
	
	tProfiles			m_profiles;
	tProfile			m_common;

	cStrObj				m_sDatabase;
	cStrObj				m_sDatabaseUpdCat;
	tDWORD				m_nFilterMsgClass;

	cAskObjectAction	m_AskActionDef;

	unsigned			m_fHeuristicSuccess : 1;
	unsigned			m_fHeuristic : 1;
	unsigned			m_fHeuristicOnly : 1;
	unsigned			m_fHeuristicContentDescr : 1;
	unsigned			m_fHeuristicContentUrls : 1;
	unsigned			m_fProfiles : 1;
	unsigned			m_fAskAction : 1;
	unsigned			m_fCheckMainPage : 1;
	unsigned			m_fUseReportFile : 1;

	hCRITICAL_SECTION	m_hCsTrMgr;
	cAutoObj<cProxyServerAddressDetector> m_proxydetect;
	tTrObjs				m_TrObjs;
	tDWORD				m_nTrMax;
	tDWORD				m_nTranspNetTimeout;
	unsigned			m_fTrMgrWork : 1;

#ifdef _DEBUG
	cThreadPoolBase     m_ThreadTest;
#endif // _DEBUG

	typedef UrlFltTTLCache< std::string, std::string > DnsCache;
	DnsCache m_dnsCache;
	hCRITICAL_SECTION m_dnsCacheCS;


// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(UrlFlt)
};
// AVP Prague stamp end


///
/// Constructor
///
inline
UrlFlt::ProcCacheType::ProcCacheType():
	cache(),
	cs( NULL )
{
}


// AVP Prague stamp begin( Header endif,  )
#endif // task_cpp
// AVP Prague stamp end
