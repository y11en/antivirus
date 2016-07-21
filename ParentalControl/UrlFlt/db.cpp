#include "db.h"

#include <Prague/pr_vector.h>
#include <ParentalControl/structs/s_urlflt.h>

// ########################################################################
// ########################################################################

//////////////////////////////////////////////////////////////////////////

///
/// Convert CatgRanges to CParctlDb::Result
///
static
void convertCatgRangesToResult( const _ParctlCatgRanges& catg, CParctlDb::Result& res )
{
	res.resize( catg.m_n );
	if( res.size() == catg.m_n )
	{
		for ( uint8_t i = 0, n = catg.m_n; i < n; ++i )
		{
			const _ParctlCatgRange& s = catg.m_a[ i ];

			CParctlDb::ResultCatg& d = res[ i ];
			d.m_id = s.m_id;
			d.m_weight = ( uint32_t ) s.m_weight;
		}
	}
	else
	{
		res.clear();
	}
}

//////////////////////////////////////////////////////////////////////////
///
/// MaxWeightSearchContext
///

///
/// Constructor
///
CParctlDb::MaxWeightSearchContext::MaxWeightSearchContext
	( 
		const CParctlDb& db, 
		SearchResult& searchRes
	):
	m_db( db ),
	m_searchRes( searchRes )
{
}

///
/// Find data in three
///
bool CParctlDb::MaxWeightSearchContext::FindDataInTree
	( 
		sSEARCHER_RT_CTX* tree_search_ctx, 
		const char* p_str, 
		uint32_t p_len 
	)
{
	return m_db.FindDataInTree
		( 
			tree_search_ctx, 
			p_str, 
			p_len, 
			m_searchRes
		);
}

///
/// Check if search should be continued
///
/// @param attr - item's attributes is about to be found
/// @return true if search should be continued, false - otherwise
///
bool CParctlDb::MaxWeightSearchContext::CheckForContinue
	( 
		const CUrlListCache::tItemAttr* const attr 
	)
{
	PR_ASSERT( attr != NULL );
	if ( attr == NULL ) // Incorrect item
	  return true;

	// Get associated category
	const _ParctlCatgRanges* catg = m_db.GetCatgRanges( attr->s_data );
	if ( catg == NULL ) // No categories
		return true;

	// Convert to results
	CParctlDb::Result res;
	convertCatgRangesToResult( *catg, res );

	// Add to total results
	for ( CParctlDb::Result::size_type i = 0, n = res.size(); i < n; ++i )
	{
		PR_ASSERT( i >= 0 && i < res.size() );
		const CParctlDb::ResultCatg& c = res[ i ];

		// Check if this category is enabled for checking
		if ( c.IsEnabled( m_searchRes.m_denyCategoriesMask ) )
		{
			if ( m_searchRes.m_info.IsEnabled() )
			{
				std::string str;
				CParctlDb::Result::convertResultsToString( res, str );
				m_searchRes.m_info.Trace( "%s: %s", attr->s_name, str.c_str() );
			}

			m_searchRes.m_res.AppendMax( c );
		}
	}

	// Check if it is possible (in theory) to get better results
	return IsBetterPossible();
}

///
/// Check if at least one match is found
///
/// @return true if at least one match is found, false - otherwise
///
bool CParctlDb::MaxWeightSearchContext::IsMatchFound()
{
	return !m_searchRes.m_res.empty();
}

///
/// Check if it is possible (in theory) to get better results
///
bool CParctlDb::MaxWeightSearchContext::IsBetterPossible() const
{
	// Return true to continue searching
	return !m_searchRes.m_res.StopAnalyzing( m_searchRes.m_denyCategoriesMask );
}

//////////////////////////////////////////////////////////////////////////

///
/// Tree search heap allocation function
///
static inline
void* _TREE_CALL _Alloc_( void*, unsigned int dwBytes ) 
{ 
	return calloc(sizeof(char), dwBytes); 
}

///
/// Tree search heap reallocation function
///
static inline
void* _TREE_CALL _Realloc_( void*, void* pMem, unsigned int dwBytes ) 
{ 
	return realloc(pMem, dwBytes); 
}

///
/// Tree search heap release function
///
static inline 
void  _TREE_CALL _Free_( void*, void* pMem ) 
{ 
	free(pMem); 
}

CParctlDb::CParctlDb()
{
	m_heap_ctx.pHeap = 0;
	m_heap_ctx.Alloc = _Alloc_;
	m_heap_ctx.Realloc = _Realloc_;
	m_heap_ctx.Free = _Free_;

	m_tree_ctx = NULL;
	TreeInit(&m_heap_ctx, &m_tree_ctx);
	m_tree_search_ctx = NULL;
}

CParctlDb::~CParctlDb()
{
	Clear();
	if( m_tree_ctx )
		TreeDone(m_tree_ctx);
}

tERROR CParctlDb::Load( hOBJECT tracer, cIO* stm )
{
	Clear();

	if( !stm )
		return errPARAMETER_INVALID;
	if( !m_tree_ctx )
		return PARCTL_DB_BADINTSTATE;

	tERROR res = _Load( tracer, stm );
	if( PR_FAIL(res) )
		Clear();
	return res;
}

void CParctlDb::Clear()
{
	if( m_tree_search_ctx )
		TreeDoneSeacherContext(m_tree_search_ctx), m_tree_search_ctx = NULL;
	if( m_tree_ctx )
		TreeUnload_Any(m_tree_ctx);
	m_catg = 0;
	m_catg_sz = 0;
	m_urlsBlack.Free();
	m_urlsWhite.Free();

	m_data.clean();
}

// ########################################################################

tERROR CParctlDb::CheckUrl
	(
		const CUrlListCache::tURL& p_url, 
		SearchResult& searchRes
	)
{
	bool l_find = false;

	searchRes.m_info.SetPrefix( "Database: white list: %s => ", p_url.m_url );
	l_find = m_urlsWhite.Find( p_url, searchRes.m_info );
	searchRes.m_info.ClearPrefix();

	if ( l_find )
	{
		return errOK_DECIDED;
	}

	// Try to find out the item with the max weight
	searchRes.m_res.clear();
	MaxWeightSearchContext searchContext( *this, searchRes );

	searchRes.m_info.SetPrefix( "Database: black list: %s => ", p_url.m_url );
	l_find = m_urlsBlack.Find( p_url, searchRes.m_info, &searchContext );
	searchRes.m_info.ClearPrefix();

	if ( !l_find )
	{
		return errOK;
	}

	/*
	if( l_find->s_data != -1 )
	{
		_ParctlCatgRanges * _catg = (_ParctlCatgRanges *)(m_catg + l_find->s_data);
		if( !_CheckCatgPtr(_catg) )
			return errOK;

		p_res.resize(_catg->m_n);
		if( p_res.size() == _catg->m_n )
		{
			for(uint8_t i = 0; i < _catg->m_n; i++)
			{
				_ParctlCatgRange& _s = _catg->m_a[i];

				ResultCatg& _d = p_res[i];
				_d.m_id = _s.m_id;
				_d.m_weight = (uint32_t)_s.m_weight;
			}
		}
	}
	*/

	return errACCESS_DENIED;
}

//////////////////////////////////////////////////////////////////////////

enum
{
	CATG_BITS_COUNT = 10
};

const tQWORD g_aCatgsBits[ CATG_BITS_COUNT ] =
{
	0,
	eUrlFltCategoryPorno,
	0,
	eUrlFltCategoryDrugs,
	eUrlFltCategoryViolence,
	eUrlFltCategoryUncens,
	eUrlFltCategoryWeapons,
	eUrlFltCategoryGambling,
	eUrlFltCategoryChat,
	eUrlFltCategoryWebMail
};

static inline
tQWORD _get_catg_bit( tDWORD id )
{
	return ( ( id >= 0 && id < CATG_BITS_COUNT ) ? g_aCatgsBits[ id ] : 0 );
}

//////////////////////////////////////////////////////////////////////////
///
/// CParctlDb::ResultCatg
///

///
/// Check if the category is enabled for the given mask
///
bool CParctlDb::ResultCatg::IsEnabled( const tQWORD& denyCategoriesMask ) const
{
	return ( ( denyCategoriesMask & _get_catg_bit( m_id ) ) ? true : false );
}

///
/// Check if no more analyzing is required for the given category
///
/// @return true if there is enough information for the given category 
///	to make a final decision
///
bool CParctlDb::ResultCatg::StopAnalyzing( const tQWORD& denyCategoriesMask ) const
{
	bool res = false;

	if ( IsEnabled( denyCategoriesMask ) )
	{
		res = ( m_weight >= 100 );
	}

	return res;
}

//////////////////////////////////////////////////////////////////////////
///
/// CParctlDb::Result
///

///
/// Accumulate the given result set
///
void CParctlDb::Result::AppendAccumulate( const Result& src )
{
	for ( size_type l = 0, m = src.size(); l < m; ++l )
	{
		AppendAccumulate( src[ l ] );
	}
}

///
/// Accumulate the given category
///
void CParctlDb::Result::AppendAccumulate( const ResultCatg& _src )
{
	if ( _src.m_weight == 0 )
		return;

	const size_type index = Find( _src );
	if ( index == -1 )
	{
		Add( _src );
	}
	else
	{
		PR_ASSERT( index >= 0 && index < size() );
		ResultCatg& _d = at( index );
		_d.m_weight += _src.m_weight;
	}
}

///
/// Accumulate the given result set: perform max choice for each category
/// in the result set
///
void CParctlDb::Result::AppendMax( const Result& src )
{
	for ( size_type l = 0, m = src.size(); l < m; ++l )
	{
		AppendMax( src[ l ] );
	}
}

///
/// Accumulate the given category if there is no such one
/// or choose max one
///
void CParctlDb::Result::AppendMax( const ResultCatg& src )
{
	if ( src.m_weight == 0 )
		return;

	size_type index = Find( src );
	if ( index == -1 )
	{
		// Add a new category
		Add( src );
	}
	else
	{
		// Choose with the max weight
		PR_ASSERT( index >= 0 && index < size() );
		CParctlDb::ResultCatg& d = at( index );
		if ( d.m_weight < src.m_weight )
		{
			d.m_weight = src.m_weight;
		}
	}
}

///
/// Add the given result set
///
void CParctlDb::Result::Add( const Result& src )
{
	for ( size_type l = 0, m = src.size(); l < m; ++l )
	{
		Add( src[ l ] );
	}
}

///
/// Add the given category
///
void CParctlDb::Result::Add( const ResultCatg& src )
{
	if ( src.m_weight == 0 )
		return;

	PR_ASSERT( Find( src ) == -1 );
	push_back( src );
}


///
/// Check if the given category exists in the results set
///
bool CParctlDb::Result::Exists( const ResultCatg& catg ) const
{
	const size_type index = Find( catg );
	return ( index != static_cast< size_type >( -1 ) );
}

///
/// Find the given category
///
CParctlDb::Result::size_type CParctlDb::Result::Find( const ResultCatg& catg ) const
{
	for( size_type l = 0, m = size(); l < m; ++l )
	{
		const ResultCatg& _d = at( l );
		if( _d.m_id == catg.m_id )
		{
			return l;
		}
	}

	return static_cast< size_type >( -1 );
}

///
/// Check if analyzing is not required any more
///
/// @return true if there is enough information to make a final decision
///
bool CParctlDb::Result::StopAnalyzing( const tQWORD& denyCategoriesMask ) const
{
	bool res = false;

	for ( int i = 0, n = size(); i < n; ++i )
	{
		const CParctlDb::ResultCatg& c = at( i );

		// Check if this category is enabled for checking
		if ( c.StopAnalyzing( denyCategoriesMask ) )
		{
			res = true;
			break;
		}
	}

	return res;
}

///
/// Convert results to string representation
///
void CParctlDb::Result::convertResultsToString( const CParctlDb::Result& res, std::string& str )
{
	str.clear();

	if ( !res.empty() )
	{
		const CParctlDb::Result::size_type sz = res.size();
		for ( CParctlDb::Result::size_type i = 0; i < sz; ++i )
		{
			const CParctlDb::ResultCatg& catg = res[ i ];

			cStrObj buf;
			buf.format
				(
					cCP_ANSI,
					"%d(%d)", 
					( int ) catg.m_id, ( int ) catg.m_weight 
				);

			str += static_cast< const char* >( buf.c_str( cCP_ANSI ) );
		}
	}
	else
	{
		str = "empty";
	}
}

//////////////////////////////////////////////////////////////////////////
///
/// SearchResult
///

///
/// Constructor
///
CParctlDb::SearchResult::SearchResult
	(
		const tQWORD& denyCategoriesMask,
		ReportInfo& info
	):
	m_res(),
	m_denyCategoriesMask( denyCategoriesMask ),
	m_info( info )
{
}


//////////////////////////////////////////////////////////////////////////

struct _TreeFindSidElem
{
	_TreeFindSidElem():
		m_SignatureID(),
		m_Signature()
	{
	}

	static int _cmp(_TreeFindSidElem& p_el, const uint8_t * p_buf, uint32_t p_size)
	{
		uint32_t sz = p_el.m_Signature.size();
		if( sz < p_size ) return -1;
		if( sz > p_size ) return 1;
		return sz ? memcmp(&p_el.m_Signature[0], p_buf, sz) : 0;
	}

	void	_set(const uint8_t * p_buf, uint32_t p_size)
	{
		const uint32_t req_size = p_size + sizeof( wchar_t );
		m_Signature.resize( req_size ); // +1 to keep null-terminating character

		if ( m_Signature.size() == req_size )
		{
			for ( uint32_t i = 0; i < p_size; ++i )
			{
				m_Signature[ i ] = p_buf[ i ];
			}
		}
		else
		{
			m_Signature.clear();
		}
	}

	tSIGNATURE_ID			m_SignatureID;
	cVectorSimple(uint8_t)	m_Signature;
};

class _TreeFindSid : public cVectorSimple(_TreeFindSidElem)
{
public:
	_TreeFindSid():
		m_buf( NULL ),
		m_size( 0 )
	{
	}

	static TREE_ERR _TREE_CALL _Cb(IN CONST void* pCallbackCtx, IN tSIGNATURE_ID SignatureID, IN int32_t dwPosInBuffer, IN uint64_t qwPosLinear, IN uint32_t dwSignatureLen)
	{
		_TreeFindSid& _ctx = *(_TreeFindSid *)pCallbackCtx;
		const uint8_t * p_sgn = _ctx.m_buf + dwPosInBuffer;

		for(int i = 0, n = _ctx.size(); i < n; i++)
		{
			if( !_TreeFindSidElem::_cmp(_ctx[i], p_sgn, dwSignatureLen) )
				return KLAV_OK;
		}

		_TreeFindSidElem& _el = _ctx.push_back();
		_el._set(p_sgn, dwSignatureLen);
		_el.m_SignatureID = SignatureID;
		return TREE_OK;
	}

	const uint8_t *	m_buf;
	uint32_t		m_size;
};

///
/// Find the given string for presence in the database applying
/// the given categories mask
///
template< typename CharType >
static
bool _FindDataInTree
	(
		const CParctlDb& db,
		sSEARCHER_RT_CTX* tree_search_ctx,
		const CharType* p_str, 
		uint32_t p_len, 
		CParctlDb::SearchResult& searchRes
	)
{
	PR_ASSERT( tree_search_ctx != NULL );
	if( !tree_search_ctx )
		return false;

	TreeNewSearch(tree_search_ctx);

	_TreeFindSid _findSid;
	_findSid.m_buf = ( const uint8_t* ) p_str;
	//_findSid.m_size = sizeof(wchar_t)*p_len;
	_findSid.m_size = sizeof( CharType ) * p_len;

	if( TREE_FAILED(TreeSearchData(tree_search_ctx, _findSid.m_buf, _findSid.m_size, 0, _findSid.m_size, &_TreeFindSid::_Cb, &_findSid)) )
		return false;

	uint32_t i, n;
	for(i = 0, n = _findSid.size(); i < n; i++)
	{
		_TreeFindSidElem& _sid = _findSid[i];
		if( _sid.m_SignatureID == -1 )
			continue;
		
		/*
		_ParctlCatgRanges * _catg = (_ParctlCatgRanges *)(catg + _sid.m_SignatureID - 1);
		if( !_CheckCatgPtr(_catg) )
		{
			PR_ASSERT( false );
			continue;
		}
		*/
		const _ParctlCatgRanges* _catg = db.GetCatgRanges( _sid.m_SignatureID - 1 );
		
		/*
		for(int k = 0; k < _catg->m_n; k++)
		{
			_ParctlCatgRange& _s = _catg->m_a[k];
			
			ResultCatg _s1;
			_s1.m_id = _s.m_id;
			_s1.m_weight = (uint32_t)_s.m_weight;
			p_res.Append(_s1);
		}
		*/
		if ( _catg != NULL )
		{
			// Convert to results
			CParctlDb::Result res;
			convertCatgRangesToResult( *_catg, res );

			// Add to total results
			for ( CParctlDb::Result::size_type i = 0, n = res.size(); i < n; ++i )
			{
				PR_ASSERT( i >= 0 && i < res.size() );
				const CParctlDb::ResultCatg& c = res[ i ];

				// Check if this category is enabled for checking
				if ( c.IsEnabled( searchRes.m_denyCategoriesMask ) )
				{
					if ( searchRes.m_info.IsEnabled() )
					{
						const char* const format = 
							( sizeof( CharType ) == 1 )
							?
							"%s: %s"
							:
							"%S: %s"
							;

						std::string str;
						CParctlDb::Result::convertResultsToString( res, str );
						searchRes.m_info.Trace
							( 
								format, 
								reinterpret_cast< const char* >( &_sid.m_Signature[ 0 ] ),
								str.c_str()
							);
					}

					searchRes.m_res.AppendMax( c );
				}
			}
		}
		else
		{
			PR_ASSERT( false );
		}
	}

	return !_findSid.empty();
}

void CParctlDb::AnalyzeData
	(
		const wchar_t * p_str, 
		uint32_t p_len, 
		SearchResult& searchRes
	)
{
	searchRes.m_info.SetPrefix( "Database: words: %S => ", p_str );
	FindDataInTree
		( 
			m_tree_search_ctx, 
			p_str,
			p_len, 
			searchRes
		);
	searchRes.m_info.ClearPrefix();
}

///
/// Find the given string for presence in the database applying
/// the given categories mask
///
bool CParctlDb::FindDataInTree
	(
		sSEARCHER_RT_CTX* tree_search_ctx,
		const char* p_str, 
		uint32_t p_len, 
		SearchResult& searchRes
	) const
{
	return _FindDataInTree( *this, tree_search_ctx, p_str, p_len, searchRes );
}

///
/// Find the given wide char string for presence in the database applying
/// the given categories mask
///
bool CParctlDb::FindDataInTree
	(
		sSEARCHER_RT_CTX* tree_search_ctx,
		const wchar_t* p_str, 
		uint32_t p_len, 
		SearchResult& searchRes
	) const
{
	return _FindDataInTree( *this, tree_search_ctx, p_str, p_len, searchRes );
}


// ########################################################################

#define _DB_STM_SEEKREAD(_stm, _offs, _p, _size)	\
	{ tDWORD readRes = 0; tERROR err = stm->SeekRead(&readRes, _offs, _p, _size);	\
	if( PR_FAIL(err) ) return err;	\
	if( readRes != _size ) return PARCTL_DB_INVALID; };

tERROR CParctlDb::_Load( hOBJECT tracer, cIO * stm)
{
	_ParctlDbHdr _hdr;
	_DB_STM_SEEKREAD(stm, 0, &_hdr, sizeof(_hdr));

	if( memcmp(_hdr.m_magic, "UFDB", sizeof(_hdr.m_magic)) )
		return PARCTL_DB_NOTSUPP;

	uint32_t	_ver = _ReadInt32(_hdr.m_ver);

	if( _HIWORD(_ver) != 1 )
		return PARCTL_DB_NOTSUPP;

	uint32_t	_ctagsOffset = _ReadInt32(_hdr.m_catgsOffset);
	uint32_t	_ctagsSize = _ReadInt32(_hdr.m_catgsSize);

	uint32_t	_wordsOffset = _ReadInt32(_hdr.m_wordsOffset);
	uint32_t	_wordsSize = _ReadInt32(_hdr.m_wordsSize);

	db_urls _urlsBlack;
	_urlsBlack.m_offset = _ReadInt32(_hdr.m_urlsBlack.m_offset);
	_urlsBlack.m_size = _ReadInt32(_hdr.m_urlsBlack.m_size);
	_urlsBlack.m_count = _ReadInt32(_hdr.m_urlsBlack.m_count);

	_urlsBlack.m_hashOffset = _ReadInt32(_hdr.m_urlsBlack.m_hashesOffset);
	_urlsBlack.m_hashSize = _ReadInt32(_hdr.m_urlsBlack.m_hashesSize);
	_urlsBlack.m_hashCount = _urlsBlack.m_hashSize/sizeof(_ParctlUrlHash);

	db_urls _urlsWhite;
	_urlsWhite.m_offset = _ReadInt32(_hdr.m_urlsWhite.m_offset);
	_urlsWhite.m_size = _ReadInt32(_hdr.m_urlsWhite.m_size);
	_urlsWhite.m_count = _ReadInt32(_hdr.m_urlsWhite.m_count);

	_urlsWhite.m_hashOffset = _ReadInt32(_hdr.m_urlsWhite.m_hashesOffset);
	_urlsWhite.m_hashSize = _ReadInt32(_hdr.m_urlsWhite.m_hashesSize);
	_urlsWhite.m_hashCount = _urlsWhite.m_hashSize/sizeof(_ParctlUrlHash);

	uint32_t	_urlsCatgsOffset = _ReadInt32(_hdr.m_urlsCatgsOffset);
	uint32_t	_urlsCatgsSize = _ReadInt32(_hdr.m_urlsCatgsSize);
	uint32_t	_urlsCatgsCount = _urlsCatgsSize/sizeof(uint32_t);

	if( _urlsCatgsSize%sizeof(uint32_t) )
		return PARCTL_DB_INVALID;
	if( _urlsBlack.m_count + _urlsBlack.m_hashCount + _urlsWhite.m_count + _urlsWhite.m_hashCount != _urlsCatgsCount )
		return PARCTL_DB_INVALID;

	// We don't keep black and white lists as they will be stored (locally) 
	// in the appropriate URL's cache
	//if( !m_data.reserve(_wordsSize + _ctagsSize + _urlsBlack.m_size + _urlsWhite.m_size, true) )
	if( !m_data.reserve( _wordsSize + _ctagsSize, true ) )
		return PARCTL_DB_NOMEM;

	// Read catgs
	if( _ctagsSize )
	{
		uint8_t * buf = (uint8_t *)m_data.ptr() + _wordsSize;

		_DB_STM_SEEKREAD(stm, _ctagsOffset, buf, _ctagsSize);

		m_catg = buf;
		m_catg_sz = _ctagsSize;

		// Check
		uint32_t _catg_sz = m_catg_sz;
		_ParctlCatgRanges * _catg = (_ParctlCatgRanges *)buf;
		while( _catg_sz )
		{
			uint32_t _cs = _ParctlCatgRanges_size(_catg->m_n);
			if( _cs > _catg_sz )
				return PARCTL_DB_INVALID;

			_catg_sz = (_catg_sz < _cs) ? 0 : (_catg_sz - _cs);
			_catg = (_ParctlCatgRanges *)((uint8_t *)_catg + _cs);
		}
	}

	// Read words
	if( _wordsSize )
	{
		uint8_t * buf = (uint8_t *)m_data.ptr();

		_DB_STM_SEEKREAD(stm, _wordsOffset, buf, _wordsSize);

		if( !KLAV_SUCC(TreeLoad_StaticOnMap(m_tree_ctx, buf, _wordsSize)) )
			return PARCTL_DB_INVALID;
	}

	// Read urls categories
	std::vector<uint32_t> _urlsCatgs;
	{
		_urlsCatgs.resize(_urlsCatgsCount);
		if( _urlsCatgs.size() != _urlsCatgsCount )
			return PARCTL_DB_NOMEM;

		_DB_STM_SEEKREAD(stm, _urlsCatgsOffset, &_urlsCatgs.front(), _urlsCatgsSize);

		for(uint32_t i = 0; i < _urlsCatgs.size(); i++)
			_urlsCatgs[i] = _ReadInt32((uint8_t *)&_urlsCatgs[i]);
	}

	// Read urls
	{
		db_urls * _urlsTmp[2] = {&_urlsBlack, &_urlsWhite};

		// Use temp buffer for reading: real data will be stored in the 
		// appropriate urlsCache's internal buffer
		//uint8_t * _bufTmp[2] = {(uint8_t *)m_data.ptr() + _wordsSize + _ctagsSize, (uint8_t *)m_data.ptr() + _wordsSize + _ctagsSize + _urlsBlack.m_size};
		cMemChunk data;
		if ( !data.reserve( _urlsBlack.m_size + _urlsWhite.m_size, true ) )
			return PARCTL_DB_NOMEM;

		//uint8_t * _bufTmp[2] = {(uint8_t *)m_data.ptr() + _wordsSize + _ctagsSize, (uint8_t *)m_data.ptr() + _wordsSize + _ctagsSize + _urlsBlack.m_size};
		uint8_t* _bufTmp[2] = { ( uint8_t* ) data.ptr(), ( uint8_t* ) data.ptr() + _urlsBlack.m_size };

		CUrlListCache * _urlsCacheTmp[2] = {&m_urlsBlack, &m_urlsWhite};
		uint32_t _ctgIdx = 0;
		
		for(int u = 0; u < countof(_urlsTmp); u++)
		{
			uint8_t * buf = _bufTmp[u];
			db_urls& __urls = *_urlsTmp[u];
			CUrlListCache& _urlsCache = *_urlsCacheTmp[u];
			
			if( __urls.m_size && __urls.m_count )
			{
				_DB_STM_SEEKREAD(stm, __urls.m_offset, buf, __urls.m_size);

				_ParctlXorBuf(buf, __urls.m_size, false, _LOWORD(_ver) == 0);
				buf[__urls.m_size - 1] = 0;

				Urls _urls; _urls.resize(__urls.m_count);
				if( _urls.size() != __urls.m_count )
					return PARCTL_DB_NOMEM;

				uint32_t cb = __urls.m_size;
				for(uint32_t i = 0; i < __urls.m_count; i++)
				{
					if( !cb )
						return PARCTL_DB_INVALID;

					Url& _url = _urls[i];
					_url.m_name = (char *)buf;
					_url.m_catgOffset = _urlsCatgs[_ctgIdx++];
					_url.m_len = strlen(_url.m_name);

					uint32_t size = _url.m_len + 1;
					buf += size;
					cb -= size;
				}

				tERROR err = _urlsCache.Build( tracer, _urls );
				if( PR_FAIL(err) )
				{
					if( err == errUNEXPECTED )
						err = PARCTL_DB_INVALID;
					return err;
				}
			}
		}
	}

	if( _wordsSize && !KLAV_SUCC(TreeGetSeacherContext(m_tree_ctx, &m_heap_ctx, &m_tree_search_ctx)) )
		return PARCTL_DB_INVALID;
	return errOK;
}

///
/// Join database results to get the best possible one
///
/// @param dbRes1 - category results
/// @param dbRes2 - category results
///
void CParctlDb::bestDbRes( const CParctlDb::Result& dbRes1, CParctlDb::Result& dbRes2 )
{
	dbRes2.AppendMax( dbRes1 );
}


//////////////////////////////////////////////////////////////////////////
///
/// Database manager
///

///
/// Constructor
///
ParCtlDbManager::ParCtlDbManager():
	m_hCS( NULL ),
	//m_lock(),
	m_dataBases(),
	m_locked( false ),
	m_tracer( NULL )
{
}

///
/// Initialize
///
tERROR ParCtlDbManager::Init( hOBJECT tracer )
{
	const tERROR err = CALL_SYS_ObjectCreateQuick
		( 
			tracer, 
			( hOBJECT* ) &m_hCS, 
			IID_CRITICAL_SECTION, 
			PID_ANY, 
			0 
		);

	m_tracer = tracer;
	
	PR_TRACE(( m_tracer, prtIMPORTANT,
		"urlflt\tParCtlDbManager: inited" ));

	return err;
}

///
/// Destroy
///
void ParCtlDbManager::Destroy( hOBJECT tracer )
{
	Clear();

	PR_TRACE(( m_tracer, prtIMPORTANT,
		"urlflt\tParCtlDbManager: destroyed" ));

	m_tracer = NULL;

	if ( m_hCS != NULL )
	{
		CALL_SYS_ObjectClose( m_hCS );
		m_hCS = NULL;
	}
}


///
/// Add database
///
CParctlDb* ParCtlDbManager::CreateDatabase( const std::wstring& name )
{
	cAutoCS lock( m_hCS, true );
	//AutoWriteRWLock lock( m_hCS, m_lock );

	// Check for existence
	for ( std::vector< DBRecord >::iterator it = m_dataBases.begin(); it != m_dataBases.end(); ++it )
	{
		DBRecord& rec = *it;

		if ( rec.m_name == name )
		{
			PR_TRACE(( m_tracer, prtIMPORTANT,
				"urlflt\tParCtlDbManager: Database exists, can't create: [%S]",
				name.c_str() ));
			return NULL;
		}
	}

	m_dataBases.push_back( DBRecord() );
	DBRecord& rec = m_dataBases.back();

	// Critical section
	hCRITICAL_SECTION hCS = NULL;
	tERROR err = CALL_SYS_ObjectCreateQuick( m_tracer, ( hOBJECT* ) &hCS, IID_CRITICAL_SECTION, PID_ANY, 0 );
	if ( PR_SUCC( err ) )
	{
		rec.m_name = name;
		rec.m_db = new CParctlDb();
		rec.m_hCS = hCS;
	}

	PR_TRACE(( m_tracer, prtIMPORTANT,
		"urlflt\tParCtlDbManager: Database is created: [%S]",
		name.c_str() ));

	return rec.m_db;
}

///
/// Remove database
///
void ParCtlDbManager::RemoveDatabase( const std::wstring& name )
{
	cAutoCS lock( m_hCS, true );
	//AutoWriteRWLock lock( m_hCS, m_lock );

	for ( std::vector< DBRecord >::iterator it = m_dataBases.begin(); it != m_dataBases.end(); ++it )
	{
		DBRecord& rec = *it;

		if ( rec.m_name == name )
		{
			// Critical section
			if ( rec.m_hCS != NULL )
			{
				CALL_SYS_ObjectClose( ( hOBJECT ) rec.m_hCS );
				rec.m_hCS = NULL;
			}

			// Database
			if ( rec.m_db != NULL )
			{
				delete rec.m_db;
				rec.m_db = NULL;
			}

			PR_TRACE(( m_tracer, prtIMPORTANT,
				"urlflt\tParCtlDbManager: Database is removed: [%S]",
				name.c_str() ));

			// Name
			rec.m_name.clear();

			// Database record
			m_dataBases.erase( it );

			break;
		}
	}
}

///
/// LoadDatabase
///
tERROR ParCtlDbManager::LoadDatabase( const std::wstring& db_name, cIOObj& pIO )
{
	cAutoCS lock( m_hCS, true );
	//AutoWriteRWLock lock( m_hCS, m_lock );

	tERROR err = errOK;

	CParctlDb* db = CreateDatabase( db_name );

	if ( db != NULL )
	{
		err = db->Load( m_tracer, pIO );
	}
	else
	{
		err = errFOUND;
	}

	if ( PR_SUCC( err ) )
	{
		PR_TRACE(( m_tracer, prtIMPORTANT,
			"urlflt\tParCtlDbManager: Database is successfully loaded: [%S]",
			db_name.c_str() ));
	}

	return err;
}

///
/// Clear
///
void ParCtlDbManager::Clear()
{
	cAutoCS lock( m_hCS, true );
	//AutoWriteRWLock lock( m_hCS, m_lock );

	while ( !m_dataBases.empty() )
	{
		const DBRecord& rec = m_dataBases.back();
		RemoveDatabase( rec.m_name);
	}

	PR_TRACE(( m_tracer, prtIMPORTANT,
		"urlflt\tParCtlDbManager: All databases are removed" ));
}

///
/// Lock manager
///
void ParCtlDbManager::Lock()
{
	m_hCS->Enter( SHARE_LEVEL_WRITE );
	PR_ASSERT( !m_locked );
	m_locked = true;

	PR_TRACE(( m_tracer, prtNOTIFY,
		"urlflt\tParCtlDbManager: locked" ));
}

///
/// Unlock manager
///
void ParCtlDbManager::Unlock()
{
	PR_TRACE(( m_tracer, prtNOTIFY,
		"urlflt\tParCtlDbManager: unlocked" ));

	PR_ASSERT( m_locked );
	m_locked = false;
	m_hCS->Leave( 0 );
}

///
/// Check the given URL 
///
tERROR ParCtlDbManager::CheckUrl
	( 
		const CUrlListCache::tURL& p_url, 
		CParctlDb::SearchResult& searchRes
	)
{
	tERROR err = errOK;

	cAutoCS lock( m_hCS, false );
	//!!! tree is not supposed to be used in multiple threads?
	//AutoReadRWLock lock( m_hCS, m_lock );
	//AutoWriteRWLock lock( m_hCS, m_lock );

	for ( std::vector< DBRecord >::iterator it = m_dataBases.begin(); it != m_dataBases.end(); ++it )
	{
		const DBRecord& rec = *it;

		if ( rec.m_db != NULL )
		{
			cAutoCS lock( rec.m_hCS, false );

			CParctlDb::Result db_res;
			tERROR db_err = rec.m_db->CheckUrl( p_url, searchRes );

			// Get the best solution
			CParctlDb::bestDbRes( db_res, searchRes.m_res );

			if ( db_err != errOK )
			{
				err = db_err;
			}
		}
		else
		{
			err = errFOUND;
		}

		if ( err == errOK_DECIDED )
		{
			break;
		}
	}

	return err;
}

///
/// Analyze the given data
///
void ParCtlDbManager::AnalyzeData
	( 
		const wchar_t * p_str, 
		uint32_t p_len, 
		CParctlDb::SearchResult& searchRes
	)
{
	tERROR err = errOK;

	cAutoCS lock( m_hCS, false );
	//!!! tree is not supposed to be used in multiple threads?
	//AutoReadRWLock lock( m_hCS, m_lock );
	//AutoWriteRWLock lock( m_hCS, m_lock );

	for ( std::vector< DBRecord >::iterator it = m_dataBases.begin(); it != m_dataBases.end(); ++it )
	{
		const DBRecord& rec = *it;

		if ( rec.m_db != NULL )
		{
			cAutoCS lock( rec.m_hCS, false );

			CParctlDb::Result db_res;
			rec.m_db->AnalyzeData( p_str, p_len, searchRes );

			// Get the best solution
			CParctlDb::bestDbRes( db_res, searchRes.m_res );
		}
	}
}

///
/// Constructor
///
ParCtlDbManager::DBRecord::DBRecord():
	m_name(),
	m_db( NULL ),
	m_hCS()
{
}


// ########################################################################
// ########################################################################
