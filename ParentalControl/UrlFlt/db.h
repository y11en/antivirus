
#ifndef __PARCTL_DB_H
#define __PARCTL_DB_H

// ########################################################################
// ########################################################################

#include <vector>

#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_csect.h>
#include <klava/klavtree.h>

#include "urlscache.h"
#include "dbfmt.h"

// ########################################################################

#define PARCTL_DB_NOTSUPP		errOBJECT_INCOMPATIBLE
#define PARCTL_DB_INVALID		errOBJECT_INVALID
#define PARCTL_DB_BADINTSTATE	errOBJECT_BAD_INTERNAL_STATE
#define PARCTL_DB_CANTREAD		errOBJECT_READ
#define PARCTL_DB_NOMEM			errNOT_ENOUGH_MEMORY

class CParctlDb
{
public:

	///
	/// struct ResultCatg
	///
	struct ResultCatg
	{
	public:

		///
		/// Constructor
		///
		ResultCatg();

		///
		/// Check if no more analyzing is required for the given category
		///
		/// @return true if there is enough information for the given category 
		///	to make a final decision
		///
		bool StopAnalyzing( const tQWORD& denyCategoriesMask ) const;

		///
		/// Check if the category is enabled for the given mask
		///
		bool IsEnabled( const tQWORD& denyCategoriesMask ) const;

	public:

		uint8_t		m_id;
		uint32_t	m_weight;
	};
	
	///
	/// class Result 
	///
	class Result : public std::vector<ResultCatg>
	{
	public:

		///
		/// Accumulate the given result set
		///
		void AppendAccumulate( const Result& src );

		///
		/// Accumulate the given category
		///
		void AppendAccumulate( const ResultCatg& src );

		///
		/// Accumulate the given result set: perform max choice for each category
		/// in the result set
		///
		void AppendMax( const Result& src );

		///
		/// Accumulate the given category if there is no such one
		/// or choose max one
		///
		void AppendMax( const ResultCatg& src );

		///
		/// Add the given result set
		///
		void Add( const Result& src );

		///
		/// Add the given category
		///
		void Add( const ResultCatg& src );

		///
		/// Check if the given category exists in the results set
		///
		bool Exists( const ResultCatg& catg ) const;

		///
		/// Find the given category
		///
		size_type Find( const ResultCatg& catg ) const;

		///
		/// Check if analyzing is not required any more
		///
		/// @return true if there is enough information to make a final decision
		///
		bool StopAnalyzing( const tQWORD& denyCategoriesMask ) const;

		///
		/// Convert results to string representation
		///
		static void convertResultsToString( const CParctlDb::Result& res, std::string& str );
	};

	///
	/// Database search result
	///
	struct SearchResult
	{
		///
		/// Constructor
		///
		SearchResult
			(
				const tQWORD& denyCategoriesMask,
				ReportInfo& info
			);

	private:

		///
		/// Restrict copying
		///
		SearchResult( const SearchResult& );

		///
		/// Restrict copying
		///
		SearchResult& operator=( const SearchResult& );

	public:

		///
		/// Category results
		///
		Result m_res;

		///
		/// Category mask used to get the results
		///
		const tQWORD& m_denyCategoriesMask;

		///
		/// Report to show the results
		///
		ReportInfo& m_info;
	};

public:

	///
	/// Constructor
	///
	CParctlDb();

	///
	/// Destructor
	///
	~CParctlDb();

private:

	///
	/// Restrict copying
	///
	CParctlDb( const CParctlDb& );

	///
	/// Restrict copying
	///
	CParctlDb& operator=( const CParctlDb& );

public:

	///
	/// Load data from the given store
	///
	tERROR	Load( hOBJECT tracer, cIO* stm );

	void	Clear();
	
public:

	///
	/// Check the given URL for presence in the database applying
	/// the given categories mask
	///
	tERROR CheckUrl
		(
			const CUrlListCache::tURL& p_url, 
			SearchResult& searchRes
		);

	///
	/// Analyze the given string for presence in the database applying
	/// the given categories mask
	///
	void AnalyzeData
		(
			const wchar_t * p_str, 
			uint32_t p_len, 
			SearchResult& searchRes
		);

	///
	/// Find the given string for presence in the database applying
	/// the given categories mask
	///
	bool FindDataInTree
		(
			sSEARCHER_RT_CTX* tree_search_ctx,
			const char* p_str, 
			uint32_t p_len, 
			SearchResult& searchRes
		) const;

	///
	/// Find the given wide char string for presence in the database applying
	/// the given categories mask
	///
	bool FindDataInTree
		(
			sSEARCHER_RT_CTX* tree_search_ctx,
			const wchar_t* p_str, 
			uint32_t p_len, 
			SearchResult& searchRes
		) const;

public:

	///
	/// Get category ranges for the given data
	///
	const _ParctlCatgRanges* GetCatgRanges( tDWORD s_data ) const;

public:

	///
	/// Join database results to get the best possible one
	///
	/// @param dbRes1 - category results
	/// @param dbRes2 - category results
	///
	static void bestDbRes( const Result& dbRes1, Result& dbRes2 );

protected:

	///
	/// Load data from the given store
	///
	tERROR _Load( hOBJECT tracer, cIO * stm );

private:
	struct Url
	{
		///
		/// Constructor
		///
		Url();

		char *		m_name;
		uint32_t	m_len;
		uint32_t	m_catgOffset;
	};

	struct db_urls
	{
		///
		/// Constructor
		///
		db_urls();

		uint32_t	m_offset;
		uint32_t	m_size;
		uint32_t	m_count;

		uint32_t	m_hashOffset;
		uint32_t	m_hashSize;
		uint32_t	m_hashCount;
	};

	class Urls : public std::vector<Url>, public CUrlListCache::tExtList
	{
	public:
		Urls()
		{
			s_strobj = 0;
			s_copydata = 0;
		}
		
		tDWORD	count() const { return size(); }
		
		void	item(tDWORD idx, CUrlListCache::tExtListItem& item) const
		{
			const Url& _url = at(idx);
			item.s_name = _url.m_name;
			item.s_len = _url.m_len;
			item.s_data = _url.m_catgOffset;
			item.s_enabled = 1;
		}
	};

	///
	/// Helper class to find the item with the max weight
	///
	//class MaxWeightSearchContext: public tTreeSearchContext
	class MaxWeightSearchContext: public CUrlListCache::tTreeSearchContext
	{

	public:

		///
		/// Constructor
		///
		MaxWeightSearchContext
			( 
				const CParctlDb& db, 
				SearchResult& searchRes
			);

	public:

		///
		/// Find data in three
		///
		virtual bool FindDataInTree
			( 
				sSEARCHER_RT_CTX* tree_search_ctx, 
				const char* p_str, 
				uint32_t p_len 
			);

	public:

		///
		/// Check if search should be continued
		///
		/// @param attr - item's attributes is about to be found
		/// @return true if search should be continued, false - otherwise
		///
		virtual bool CheckForContinue( const CUrlListCache::tItemAttr* attr );

		///
		/// Check if at least one match is found
		///
		/// @return true if at least one match is found, false - otherwise
		///
		virtual bool IsMatchFound();

	public:

		///
		/// Check if it is possible (in theory) to get better results
		///
		bool IsBetterPossible() const;

	private:

		///
		/// Restrict copying 
		///
		MaxWeightSearchContext( const MaxWeightSearchContext& );

		///
		/// Restrict copying 
		///
		MaxWeightSearchContext& operator=( const MaxWeightSearchContext& );

	private:

		///
		/// Database
		///
		const CParctlDb& m_db;

		///
		/// Search results
		///
		SearchResult& m_searchRes;
	};

private:

	bool _CheckCatgPtr(_ParctlCatgRanges * p_catg) const
	{
		if( (uint32_t)((uint8_t *)p_catg - m_catg) >= m_catg_sz )
		{
			PR_ASSERT( false );
			return false;
		}
		return ((uint8_t *)p_catg - m_catg) + _ParctlCatgRanges_size(p_catg->m_n) <= m_catg_sz;
	}
	
private:
	sHEAP_CTX				m_heap_ctx;
	sRT_CTX *				m_tree_ctx;
	sSEARCHER_RT_CTX *		m_tree_search_ctx;
	cMemChunk				m_data;

	uint8_t *				m_catg;
	uint32_t				m_catg_sz;
	CUrlListCache			m_urlsBlack;
	CUrlListCache			m_urlsWhite;
};

//////////////////////////////////////////////////////////////////////////
///
/// class CParctlDb::ResultCatg
///

///
/// Constructor
///
inline
CParctlDb::ResultCatg::ResultCatg():
	m_id( 0 ),
	m_weight( 0 )
{
}

//////////////////////////////////////////////////////////////////////////
///
/// class CParctlDb::Url
///

///
/// Constructor
///
inline
CParctlDb::Url::Url():
	m_name( NULL ),
	m_len( 0 ),
	m_catgOffset( 0 )
{
}

///
/// Constructor
///
inline
CParctlDb::db_urls::db_urls():
	m_offset( 0),
	m_size( 0 ),
	m_count( 0 ),
	m_hashOffset( 0 ),
	m_hashSize( 0 ),
	m_hashCount( 0 )
{
}

//////////////////////////////////////////////////////////////////////////
///
/// class CParctlDb
///

///
/// Get category ranges for the given data
///
inline
const _ParctlCatgRanges* CParctlDb::GetCatgRanges( tDWORD s_data ) const
{
	if ( s_data == -1 )
		return NULL;

	_ParctlCatgRanges * catg = ( _ParctlCatgRanges* )( m_catg + s_data );
	if( !_CheckCatgPtr( catg ) ) 
		return NULL;

	return catg;
}

//////////////////////////////////////////////////////////////////////////

/*
///
/// class RWLock
///
class RWLock
{
public:

	///
	/// Constructor
	///
	RWLock();

	///
	/// Lock for reading
	///
	void LockForRead( hCRITICAL_SECTION& cs );

	///
	/// Lock for writing
	///
	void LockForWrite( hCRITICAL_SECTION& cs );

	///
	/// Unlock for reading
	///
	void UnlockForRead( hCRITICAL_SECTION& cs );

	///
	/// Unlock for writing
	///
	void UnlockForWrite( hCRITICAL_SECTION& cs );

private:

	int m_readers;
	int m_writers;
};


///
/// class AutoReadRWLock
///
class AutoReadRWLock
{
public:

	AutoReadRWLock( hCRITICAL_SECTION& cs, RWLock& lock ):
		m_cs( cs ),
		m_lock( lock )
	{
		m_lock.LockForRead( m_cs );
	}

	~AutoReadRWLock()
	{
		m_lock.UnlockForRead( m_cs );
	}

private:

	hCRITICAL_SECTION& m_cs;
	RWLock& m_lock;

};

///
/// class AutoWriteRWLock
///
class AutoWriteRWLock
{
public:

	AutoWriteRWLock( hCRITICAL_SECTION& cs, RWLock& lock ):
		m_cs( cs ),
		m_lock( lock )
	{
		m_lock.LockForWrite( m_cs );
	}

	~AutoWriteRWLock()
	{
		m_lock.UnlockForWrite( m_cs );
	}

private:

	hCRITICAL_SECTION& m_cs;
	RWLock& m_lock;

};
*/

///
/// Database manager
///
class ParCtlDbManager
{

public:

	///
	/// Constructor
	///
	ParCtlDbManager();

	///
	/// Initialize
	///
	tERROR Init( hOBJECT tracer );

	///
	/// Destroy
	///
	void Destroy( hOBJECT tracer );

private:

	///
	/// Prohibit copying
	///
	ParCtlDbManager( const ParCtlDbManager& );

	///
	/// Prohibit copying
	///
	ParCtlDbManager& operator=( const ParCtlDbManager& );

public:

	///
	/// LoadDatabase
	///
	tERROR LoadDatabase( const std::wstring& db_name, cIOObj& pIO );

	///
	/// Clear
	///
	void Clear();

public:

	///
	/// Lock manager
	///
	void Lock();

	///
	/// Unlock manager
	///
	void Unlock();

public:

	///
	/// Check the given URL 
	///
	tERROR CheckUrl
		( 
			const CUrlListCache::tURL& p_url, 
			CParctlDb::SearchResult& searchRes
		);

	///
	/// Analyze the given data
	///
	void AnalyzeData
		( 
			const wchar_t * p_str, 
			uint32_t p_len, 
			CParctlDb::SearchResult& searchRes
		);

private:

	///
	/// Add database
	///
	CParctlDb* CreateDatabase( const std::wstring& name );

	///
	/// Remove database
	///
	void RemoveDatabase( const std::wstring& name );

private:

	struct DBRecord
	{

	public:

		///
		/// Constructor
		///
		DBRecord();

	public:

		std::wstring m_name;
		CParctlDb* m_db;
		hCRITICAL_SECTION m_hCS;

	};

private:

	hCRITICAL_SECTION m_hCS;
	//RWLock m_lock;
	std::vector< DBRecord > m_dataBases;
	bool m_locked;
	hOBJECT m_tracer;

};


// ########################################################################
// ########################################################################

#endif // __PARCTL_DB_H
