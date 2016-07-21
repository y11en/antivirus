#include "urlscache.h"

#include <Prague/iface/i_io.h>

#include <minmax.h>

#include "urlflt_dbg.h"

// ########################################################################
// ########################################################################


static tCHAR *	_url_copy(const cStrObj& strUrl, tDWORD nFrom, tDWORD nTo, tCHAR * buf, bool bCorrect = true);
static tDWORD	_url_simplify_path(tCHAR * pszUrl);
static bool		_url_matchwithpattern(const char * Pattern, const char * Name, bool bPatternWithAsterics);

//////////////////////////////////////////////////////////////////////////

// ########################################################################


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

///
/// Constructor
///
CUrlListCache::CUrlListCache()
{
	// Initialize heap context
	m_heap_ctx.pHeap = 0;
	m_heap_ctx.Alloc = _Alloc_;
	m_heap_ctx.Realloc = _Realloc_;
	m_heap_ctx.Free = _Free_;

	// Initialize tree context
	m_tree_ctx = NULL;

	// Initialize tree
	const bool res = KLAV_SUCC( TreeInit( &m_heap_ctx, &m_tree_ctx) );
	PR_ASSERT( res );
}

///
/// Destructor
///
CUrlListCache::~CUrlListCache()
{
	Free();

	// Release tree context
	if ( m_tree_ctx != NULL )
	{
		TreeUnload_Any( m_tree_ctx );
		m_tree_ctx = NULL;
	}

	// Release tree
	if ( m_tree_ctx != NULL )
	{
		TreeDone( m_tree_ctx );
		m_tree_ctx = NULL;
	}
};

///
/// Determine type of URL: the result will influence on where the URL
/// will be placed: to a tree (for quick search) or to a simple array 
/// (for linear search)
///
static
bool IsComplexUrl( cStrObj& url )
{
	// Check for '?' anywhere
	bool res = ( url.find_first_of( "?" ) != cStrObj::npos );
	
	// Check for period ('.') at the beginning
	if ( !res && !url.empty() )
	{
		res = ( url[ 0 ] == '.' );
	}

	// Check for stars ('*')
	if ( !res )
	{
		const tDWORD pos1 = url.find_first_of( "*" );
		// URL with at least one '*' is complex
		res = ( pos1 != cStrObj::npos );

		// We found '*' at the beginning
		if ( pos1 == 0 )
		{
			const tDWORD pos2 = url.find_first_of( "*", pos1 + 1 );
			// We found '*' at the end
			if ( pos2 == url.length() - 1 )
			{
				// There are only two stars - at the beginning and
				// at the end of the URL, so it might be considered as
				// simple one if we remove stars
				url.erase( 0, 1 );
				url.erase( url.length() - 1, 1 );
				res = false;
			}
		}

	}

	return res;
}

tERROR CUrlListCache::Build( hOBJECT tracer, tExtList& list )
{
	tERROR error = errOK;
	tDWORD i, c;

	Free();
	c = list.count();
	if( !c )
		return errOK;
	
	tURL _url;
	tDWORD nBufPos = 0;

	for(i = 0; i < c; i++)
	{
		tExtListItem _le; list.item(i, _le);
		if( !_le.s_enabled )
			continue;
		
		if( !_le.s_name )
		{
			error = errPARAMETER_INVALID;
			break;
		}
		
		/*
		if( list.s_strobj )
		{
			resName = (tCHAR *)nBufPos;

			cStrObj& _urlU = *(cStrObj *)_le.s_name;
			if( !m_pCacheData.reserve(nBufPos + _url.BufSize(_urlU), true) )
			{
				error = errNOT_ENOUGH_MEMORY;
				break;
			}

			tCHAR* const bufPtr = ( tCHAR* ) m_pCacheData.ptr() + nBufPos;
			const tUINT bufSize = m_pCacheData.size() - nBufPos;
			resSize = _url.Normalize( _urlU, bufPtr, bufSize, 0 );
			nBufPos += resSize;
		}
		else
		{
			resName = (tCHAR *)_le.s_name;
			resSize = _url.Parse(resName, _le.s_len);
		}
		*/

		cStrObj strUrl;
		if ( list.s_strobj )
		{
			strUrl = *(cStrObj *)_le.s_name;
		}
		else
		{
			strUrl = (tCHAR *)_le.s_name;
		}

		if ( strUrl.empty() )
		{
			PR_ASSERT( false );
			continue;
		}

		tCHAR * resName = NULL;
		tDWORD resSize = 0;

		// Linear search is used for complex URLs
		// (We don't optimize list items that use s_strobj set to 1 for now)
		const bool isCached = ( list.s_strobj || IsComplexUrl( strUrl ) );
		if ( isCached )
		{
			// Complex URL is added to cache
			resName = (tCHAR *)nBufPos;

			if( !m_pCacheData.reserve(nBufPos + _url.BufSize( strUrl ), true) )
			{
				error = errNOT_ENOUGH_MEMORY;
				break;
			}

			tCHAR* const bufPtr = ( tCHAR* ) m_pCacheData.ptr() + nBufPos;
			const tUINT bufSize = m_pCacheData.size() - nBufPos;
			resSize = _url.Normalize( strUrl, bufPtr, bufSize, 0 );
			nBufPos += resSize;
		}
		else
		{
			// Simple URL is stored in three
			resName = (tCHAR *)_le.s_name;
			resSize = _url.Parse(resName, _le.s_len);
		}

		if( !(resSize - 1) )
		{
			PR_ASSERT( false );
			continue;
		}

		if ( isCached )
		{
			// Use linear search for URLs that are masked in a complicated manner
			const bool hasMask = ( strpbrk( _url.m_host, "*?" ) != NULL );

			tItemAttr& _attr = m_vItems.push_back();
			_attr.s_name		= resName;
			_attr.s_len			= resSize - 1;
			_attr.s_data		= _le.s_data;
			_attr.s_domain_only	= !_url.m_pathLen && !_url.m_protoLen;
			_attr.s_proto		= !!_url.m_protoLen;
			
			//if( strpbrk(_url.m_host, "*?") )
			if( hasMask )
			{
				_attr.s_has_mask = 1;
				if( _attr.s_domain_only )
				{
					tDWORD dotcount = 0;
					for(tDWORD i = 0; i < _url.m_hostLen; i++)
					{
						if( _url.m_host[i] == '.' )
							dotcount++;
					}
					
					if( dotcount < 2 ) // this is not a domain or 1st level domain
						_attr.s_domain_only = 0;
				}
			}
		}
		else
		{
			// Use tree search
			typedef std::vector<unsigned char> tBin;

			tBin _sgn; 
			uint32_t dwRawDataSize = 0;
			TreeConvertRaw2Signature
				(
					( const uint8_t* ) static_cast< const char* >( strUrl.c_str( cCP_ANSI ) ), 
					strUrl.size(), 
					NULL, 
					0, 
					&dwRawDataSize
				);
			_sgn.resize(dwRawDataSize);
			if( !_sgn.size() || _sgn.size() != dwRawDataSize )
				return false;

			TreeConvertRaw2Signature
				(
					( const uint8_t* ) static_cast< const char* >( strUrl.c_str( cCP_ANSI ) ), 
					strUrl.size(), 
					&_sgn[0], 
					_sgn.size(), 
					&dwRawDataSize
				);

			// Set category offset
			uint32_t catRelOffset = _le.s_data;

			tSIGNATURE_ID res_id;
			const bool resAS = 
				KLAV_SUCC
					( 
						TreeAddSignature
							(
								m_tree_ctx, 
								(const uint8_t *)&_sgn[0], 
								_sgn.size(), 
								catRelOffset == -1 ? -1 : (catRelOffset + 1), 
								&res_id
							)
					);
			//zzz it should be checked in fact but there are problems with database for now!
			//PR_ASSERT( resAS );
			if ( !resAS )
			{
				PR_TRACE(( tracer, prtERROR, 
					"urlflt\tCUrlListCache::Build: Can't add signature for [%S] to tree!", 
					strUrl.data() ));
			}
		}
	}

	if( PR_SUCC(error) && nBufPos && !m_pCacheData.exact_size(nBufPos, true) )
		error = errNOT_ENOUGH_MEMORY;
	
	if( PR_FAIL(error) )
		Free();
	
	if( nBufPos )
	{
		tCHAR * bufBegin = (tCHAR *)m_pCacheData.ptr();
		for(i = 0, c = m_vItems.size(); i < c; i++)
		{
			tItemAttr& _attr = m_vItems[i];
			_attr.s_name = bufBegin + (tDWORD)_attr.s_name;
		}
	}

	return error;
}

tERROR CUrlListCache::Free()
{
	m_vItems.clear();
	m_pCacheData.clean();
	return errOK;
}

///
/// Check if search should be finished
///
/// @param context - search context
/// @param attr - found item 
///
static inline
bool stopSearching
	( 
		CUrlListCache::tSearchContext* const context, 
		const CUrlListCache::tItemAttr* const attr,
		ReportInfo& info
	)
{
	bool res = true;

	if ( context != NULL )
	{
		res = !context->CheckForContinue( attr );
	}
	else
	{
		PR_ASSERT( attr != NULL );
		if ( attr != NULL )
		{
			info.Trace( "%s", attr->s_name );
		}
	}

	return res;
}

///
/// Find the match for the given URL (linear search)
///
/// @param url - URL for searching
/// @param context - search context
/// @return item's attributes if it is found, false - otherwise
///
bool CUrlListCache::CompareWithPattern
	( 
		const tURL& url, 
		ReportInfo& info,
		tSearchContext* const context 
	) const
{
	const tCHAR * pUrlWithoutProto = url.m_protoLen ? url.m_host : url.m_url;

	for(tDWORD i = 0, c = m_vItems.count(); i < c; i++)
	{
		const tItemAttr& _attr = m_vItems[i];

		if( _attr.s_domain_only )
		{
			const CUrlListCache::tItemAttr * _res = NULL;
			if( _attr.s_has_mask )
			{
				tCHAR * hce = url.m_host + url.m_hostLen;
				tCHAR hc = *hce;
				*hce = 0;
				
				if( _url_matchwithpattern(_attr.s_name, url.m_host, false) )
					_res = &_attr;

				if( _attr.s_len >= 2 && _attr.s_name[0] == '*' && _attr.s_name[1] == '.' )
					if( _url_matchwithpattern(_attr.s_name + 2, url.m_host, false) )
						_res = &_attr;
				
				*hce = hc;
			}
			else if( _attr.s_len && _attr.s_name[0] == '.' )
			{
				if( !strcmp(_attr.s_name + 1, url.m_host) )
					_res = &_attr;
			}
			else if( url.m_hostLen >= _attr.s_len )
			{
				if( url.m_hostLen > _attr.s_len ) // with sub-domains
				{
					if( url.m_host[url.m_hostLen - _attr.s_len - 1] != '.' )
						continue;
				}

				if( !strncmp(_attr.s_name, url.m_host + url.m_hostLen - _attr.s_len, _attr.s_len) )
					_res = &_attr;
			}
		
			if( _res )
			{
				if ( stopSearching( context, _res, info ) )
					return true;
			}
			
			continue;
		}

		const tCHAR * pUrlCompare = (_attr.s_proto ? url.m_url : pUrlWithoutProto);
		if( _attr.s_has_mask )
		{
			if( _url_matchwithpattern(_attr.s_name, pUrlCompare, false) )
			{
				if ( stopSearching( context, &_attr, info ) )
					return true;
			}
		}
		else
		{
			tDWORD proto_len = pUrlCompare - url.m_url;
			if( url.m_urlLen - proto_len != _attr.s_len )
				continue;
			
			if( !strcmp(_attr.s_name, pUrlCompare) )
			{
				if ( stopSearching( context, &_attr, info ) )
					return true;
			}
		}
	}
	
	return ( context != NULL ) ? context->IsMatchFound(): false;
}

///
/// Find the match for the given URL (quick search but can be used with
/// simple URLs only)
///
/// @param url - URL for searching
/// @param context - search context
/// @return item's attributes if it is found, false - otherwise
///
bool CUrlListCache::FindInTree
	( 
		const tURL& url, 
		tTreeSearchContext* context
	) const
{
	if ( context == NULL )
	{
		return false;
	}

	const tCHAR* search_url = NULL;
	tDWORD search_url_len = 0;
	if ( url.m_host != NULL ) 
	{
		search_url = url.m_host;
		search_url_len = url.m_hostLen;
	}
	else
	{
		search_url = url.m_url;
		search_url_len = url.m_urlLen;
	}

	bool found = false;

	PR_ASSERT( search_url != NULL && search_url_len > 0 );
	if ( search_url != NULL && search_url_len > 0 )
	{

		sSEARCHER_RT_CTX* tree_search_ctx = NULL;

		bool res = 
			KLAV_SUCC( TreeGetSeacherContext( m_tree_ctx, &m_heap_ctx, &tree_search_ctx ) );

		if( !tree_search_ctx )
		{
			return false;
		}

		found = context->FindDataInTree
			( 
				tree_search_ctx, 
				search_url, 
				static_cast< uint32_t >( search_url_len )
			);

		res = KLAV_SUCC( TreeDoneSeacherContext( tree_search_ctx ) );
		PR_ASSERT( res );
		tree_search_ctx = NULL;
	}

	return found;
}

///
/// Find the match for the given URL
///
/// @param url - URL for searching
/// @param context - search context
/// @return item's attributes if it is found, false - otherwise
///
bool CUrlListCache::Find
	( 
		const tURL& url, 
		ReportInfo& info,
		tTreeSearchContext* const context 
	) const
{
	// Search will be performed in two stages: the first stage is to find the URL
	// in tree, the second stage is to compare the URL with masks
	const bool resTree = FindInTree( url, context );

	// Try to find out the item with the max weight
	const bool resPattern = CompareWithPattern( url, info, context );

	return ( resTree || resPattern );
}

void CUrlListCache::Dump()
{
	for(tDWORD i = 0, c = m_vItems.count(); i < c; i++)
	{
		tItemAttr& _attr = m_vItems[i];
		if( !_attr.s_len || !_attr.s_name )
		{
			PR_TRACE(((hOBJECT)m_pCacheData.owner(), prtNOT_IMPORTANT, "urlflt\tItem #d <empty>", i));
			continue;
		}
		
		PR_TRACE(((hOBJECT)m_pCacheData.owner(), prtNOT_IMPORTANT, "urlflt\tItem #%d (%s%s%s) %8d %s", i,
		_attr.s_domain_only ? "D" : "_",
		_attr.s_has_mask    ? "M" : "_",
		_attr.s_proto       ? "P" : "_",
		_attr.s_len,
		_attr.s_name));
	}
}

// ########################################################################

tDWORD CUrlListCache::tURL::BufSize(const cStrObj& strUrl)
{
	return strUrl.size()*5 + 3;
}

tDWORD CUrlListCache::tURL::Normalize
	(
		const cStrObj& strUrl, 
		tCHAR* buf, 
		const tUINT bufSize, 
		tDWORD nFlags
	)
{
	tCHAR * bufStart = buf;
	tDWORD nPos = 0;

	nPos = strUrl.find(L"://");
	if( nPos == cStrObj::npos )
	{
		nPos = strUrl.find(L":\\\\");
	}

	tDWORD nHostBegin = 0, nHostEnd = strUrl.size();
	tDWORD nPathBegin = strUrl.size(), nPathEnd = nPathBegin;
	
	m_url = buf;
	
	if( nPos != cStrObj::npos )
	{
		// Check if there is anything to copy
		if ( nPos > 0 )
		{
			//m_proto = buf, m_protoLen = nPos;
			m_proto = buf;
			CHECK_PTR( bufStart, bufSize, m_proto );
			buf = _url_copy(strUrl, 0, nPos, buf);
			m_protoLen = ( buf - m_proto ); // Could copy more than just nPos
			CHECK_PTR( bufStart, bufSize, buf );
			memcpy(buf, "://", 3), buf += 3;

			nHostBegin = nPos + 3;
		}
		else
		{
			PR_ASSERT( false );
		}
	}
	else
	{
		m_proto = NULL, m_protoLen = 0;
	}

	nPos = strUrl.find_first_of(L"/\\", nHostBegin);
	if( nPos != cStrObj::npos )
	{
		nHostEnd = nPos;
		nPathBegin = nPos + 1;
	}

	// Get auth info
	nPos = strUrl.find_first_of(L"@", nHostBegin, nHostEnd - nHostBegin);
	if( nPos != cStrObj::npos )
	{
		// Check if there is anything to copy
		//if ( nPos + 1 - nHostBegin > 0 )
		if ( nPos - nHostBegin > 0 )
		{
			//m_auth = buf, m_authLen = nPos - nHostBegin;
			m_auth = buf;
			CHECK_PTR( bufStart, bufSize, m_auth );
			//buf = _url_copy(strUrl, nHostBegin, nPos + 1, buf, false);
			buf = _url_copy(strUrl, nHostBegin, nPos, buf, false);
			m_authLen = ( buf - m_auth ); // Could copy more than (nPos-nHostBegin)
			CHECK_PTR( bufStart, bufSize, buf );

			nHostBegin = nPos + 1;
		}
		else
		{
			PR_ASSERT( false );
		}
	}
	else
	{
		m_auth = NULL, m_authLen = 0;
	}

	// Get port info
	//nPos = strUrl.find_last_of(L":", nHostBegin, nHostEnd - nHostBegin);
	nPos = strUrl.find_first_of(L":", nHostBegin, nHostEnd - nHostBegin);
	if( nPos != cStrObj::npos )
	{
		//m_port = NULL, m_portLen = nHostEnd - nPos - 1;
		m_port = NULL, m_portLen = nHostEnd - nPos;
		nHostEnd = nPos;
	}
	else
	{
		m_port = NULL, m_portLen = 0;
	}

	//m_host = buf, m_hostLen = nHostEnd - nHostBegin;
	m_host = buf;
	buf = _url_copy(strUrl, nHostBegin, nHostEnd, buf);
	m_hostLen = ( buf - m_host ); // Could copy more than (nHostEnd-nHostBegin)
	CHECK_PTR( bufStart, bufSize, buf );
	
	if( m_portLen )
	{
		//m_port = buf + 1;
		m_port = buf;
		CHECK_PTR( bufStart, bufSize, m_port );
		//buf = _url_copy(strUrl, nHostEnd, nHostEnd + m_portLen + 1, buf, false);
		buf = _url_copy(strUrl, nHostEnd, nHostEnd + m_portLen, buf, false);
		m_portLen = ( buf - m_port ); // Could copy more than just m_portLen
		CHECK_PTR( bufStart, bufSize, buf );
	}
	
	CHECK_PTR( bufStart, bufSize, buf );
	*buf++ = 0;

	if( nPathEnd - nPathBegin )
	{
		//m_path = buf, m_pathLen = nPathEnd - nPathBegin;
		m_path = buf;
		CHECK_PTR( bufStart, bufSize, m_path );
		m_path[-1] = '/'; // Always exist prev char because of host previous copied

		buf = _url_copy(strUrl, nPathBegin, nPathEnd, buf);
		m_pathLen = ( buf - m_path ); // Could copy more than (nPathEnd-nPathBegin)
		CHECK_PTR( bufStart, bufSize, buf );
		*buf++ = 0;

		if( nFlags & fSimplify )
		{
			m_pathLen = _url_simplify_path(m_path - 1);
			if(m_pathLen)
				m_pathLen--;
			// +1 to take care about -1 below (when m_urlLen is calculated)
			buf = m_path + m_pathLen + 1;
			CHECK_PTR( bufStart, bufSize, buf );

			if( !m_pathLen )
				m_path = NULL;
		}

		if(m_path && m_pathLen && (m_path[m_pathLen - 1] == '/' || m_path[m_pathLen - 1] == '\\'))
		{
			m_path[m_pathLen - 1] = 0, m_pathLen--, buf--;
		}

		// Check again if there is no path
		if( !m_pathLen )
		{
			m_path = NULL;
		}
	}
	else
	{
		m_path = NULL, m_pathLen = 0;
	}

	m_urlLen = buf - bufStart - 1;

	// Check the state is correct
	CHECK_URL( bufStart, bufSize, *this );

	return m_urlLen + 1;
}

tDWORD CUrlListCache::tURL::Parse(tCHAR * buf, tDWORD len)
{
	tCHAR * bufStart = buf;
	tCHAR * nPos;

	nPos = strstr(buf, "://");
	if( !nPos )
		nPos = strstr(buf, ":\\\\");

	tCHAR * nHostBegin = bufStart, * nHostEnd = bufStart + len;
	tCHAR * nPathBegin = bufStart + len, * nPathEnd = nPathBegin;

	m_url = buf;

	if( nPos )
	{
		m_proto = buf, m_protoLen = nPos - buf;
		buf = nPos + 3;

		nHostBegin = buf;
	}
	else
		m_proto = NULL, m_protoLen = 0;

	nPos = strpbrk(buf, "/\\");
	if( nPos )
	{
		nHostEnd = nPos;
		nPathBegin = nPos + 1;
	}

	{
		tCHAR * hecP = nHostEnd;
		tCHAR hec = *nHostEnd;
		*hecP = 0;

		nPos = strchr(nHostBegin, '@');
		if( nPos )
			nHostBegin = nPos + 1;

		nPos = strrchr(nHostBegin, ':');
		if( nPos )
			nHostEnd = nPos - 1;

		*hecP = hec;
	}

	m_host = nHostBegin, m_hostLen = nHostEnd - nHostBegin;

	if( nPathEnd - nPathBegin )
		m_path = nPathBegin, m_pathLen = nPathEnd - nPathBegin;
	else
		m_path = NULL, m_pathLen = 0;

	m_urlLen = len;
	return len + 1;
}

void CUrlListCache::tURL::Reoffset(tLONG nOffset)
{
	if( m_protoLen )
		m_proto += nOffset;
	if ( m_authLen )
		m_auth += nOffset;
	if( m_hostLen )
		m_host += nOffset;
	if ( m_portLen )
		m_port += nOffset;
	if( m_pathLen )
		m_path += nOffset;
	if( m_urlLen )
		m_url += nOffset;
}

void CUrlListCache::tURL::GetSiteUrl(STR& _SiteUrl) const
{
	_SiteUrl.m_p = NULL;
	_SiteUrl.m_n = 0;
	
	if( !m_hostLen )
		return;

	_SiteUrl.m_p = m_url;
	if( m_pathLen )
		_SiteUrl.m_n = m_path - m_url - 1;
	else
		_SiteUrl.m_n = m_urlLen;
}

///
/// Extract full URL
///
void CUrlListCache::tURL::GetFullUrl( STR& fullURL ) const
{
	fullURL.m_p = NULL;
	fullURL.m_n = 0;

	if( !m_hostLen )
		return;

	fullURL.m_p = m_url;
	fullURL.m_n = m_urlLen;
}

///
/// Extract path URL (till the last '/' but before parameters)
///
void CUrlListCache::tURL::GetPathUrl( STR& pathUrl ) const
{
	pathUrl.m_p = NULL;
	pathUrl.m_n = 0;

	if( !m_hostLen )
		return;

	PR_ASSERT( m_host != NULL );
	// Find the position where parameters begin
	const char* param_ptr = strchr( m_host, '?' );
	if ( param_ptr == NULL )
	{
		param_ptr = m_url + m_urlLen;
	}

	// Find path's end
	const char* path_ptr = param_ptr;
	while ( path_ptr != m_host )
	{
		const tCHAR ch = *path_ptr;

		if ( ch == '/' || ch == '\\' )
		{
			break;
		}

		--path_ptr;
	}

	if ( path_ptr != m_host )
	{
		pathUrl.m_p = m_url;
		pathUrl.m_n = path_ptr - m_url;
	}
	else
	{
		pathUrl.m_p = m_url;
		pathUrl.m_n = m_urlLen;
	}
}


// ########################################################################

#define R  ( urlchr_reserved )
#define U  ( urlchr_unsafe )
#define RU ( ( R ) | ( U ) )

#define urlchr_test( c, mask ) ( g_urlchr_table[ ( unsigned char )( c ) ] & ( mask ) )

#define XDIGIT_TO_XCHAR(x)	(((x) < 10) ? ((x) + '0') : ((x) - 10 + 'A'))

///
/// Convert the given character into digit
///
/// @param x - character is about to be converted
/// @return digit after character is converted
///
static
bool XCHAR_TO_XDIGIT( tWCHAR& x )
{
	bool res = true;

	if ( x >= 'A' && x <= 'F' )
	{
		x -= 'A';
		x += 10;
	}
	else if ( x >= 'a' && x <= 'f' )
	{
		x -= 'a';
		x += 10;
	}
	else if ( x >= '0' && x <= '9' )
	{
		x -= '0';
	}
	else
	{
		// Incorrect character
		//PR_ASSERT( false ); 
		res = false;
	}

	return res;
}

enum
{
	urlchr_reserved = 1,
	urlchr_unsafe   = 2
};

const static unsigned char g_urlchr_table[256] =
{
	U,  U,  U,  U,   U,  U,  U,  U,   /* NUL SOH STX ETX  EOT ENQ ACK BEL */
	U,  U,  U,  U,   U,  U,  U,  U,   /* BS  HT  LF  VT   FF  CR  SO  SI  */
	U,  U,  U,  U,   U,  U,  U,  U,   /* DLE DC1 DC2 DC3  DC4 NAK SYN ETB */
	U,  U,  U,  U,   U,  U,  U,  U,   /* CAN EM  SUB ESC  FS  GS  RS  US  */
	U,  0,  U,  U,   0,  R,  R,  0,   /* SP  !   "   #    $   %   &   '   */
	0,  0,  0,  R,   0,  0,  0,  R,   /* (   )   *   +    ,   -   .   /   */
	0,  0,  0,  0,   0,  0,  0,  0,   /* 0   1   2   3    4   5   6   7   */
	0,  0,  R,  R,   U,  R,  U,  R,   /* 8   9   :   ;    <   =   >   ?   */
	RU, 0,  0,  0,   0,  0,  0,  0,   /* @   A   B   C    D   E   F   G   */
	0,  0,  0,  0,   0,  0,  0,  0,   /* H   I   J   K    L   M   N   O   */
	0,  0,  0,  0,   0,  0,  0,  0,   /* P   Q   R   S    T   U   V   W   */
	0,  0,  0,  U,   U,  U,  U,  0,   /* X   Y   Z   [    \   ]   ^   _   */
	U,  0,  0,  0,   0,  0,  0,  0,   /* `   a   b   c    d   e   f   g   */
	0,  0,  0,  0,   0,  0,  0,  0,   /* h   i   j   k    l   m   n   o   */
	0,  0,  0,  0,   0,  0,  0,  0,   /* p   q   r   s    t   u   v   w   */
	0,  0,  0,  U,   U,  U,  R,  U,   /* x   y   z   {    |   }   ~   DEL */

	U,  U,  U,  U,   U,  U,  U,  U,   U,  U,  U,  U,   U,  U,  U,  U,
	U,  U,  U,  U,   U,  U,  U,  U,   U,  U,  U,  U,   U,  U,  U,  U,
	U,  U,  U,  U,   U,  U,  U,  U,   U,  U,  U,  U,   U,  U,  U,  U,
	U,  U,  U,  U,   U,  U,  U,  U,   U,  U,  U,  U,   U,  U,  U,  U,

	U,  U,  U,  U,   U,  U,  U,  U,   U,  U,  U,  U,   U,  U,  U,  U,
	U,  U,  U,  U,   U,  U,  U,  U,   U,  U,  U,  U,   U,  U,  U,  U,
	U,  U,  U,  U,   U,  U,  U,  U,   U,  U,  U,  U,   U,  U,  U,  U,
	U,  U,  U,  U,   U,  U,  U,  U,   U,  U,  U,  U,   U,  U,  U,  U,
};

#undef R
#undef U
#undef RU

tCHAR * _url_copy(const cStrObj& str, tDWORD nFrom, tDWORD nTo, tCHAR * buf, bool bCorrect)
{
	tWCHAR c = 0;
	tCHAR l = 0;

	tDWORD i = nFrom;
	while ( i < nTo )
	{
		if( !bCorrect )
		{
			*buf++ = (tCHAR)str[i];
		}
		else
		{
			c = str.at(i, cCP_UNICODE, cSTRING_TO_LOWER);

			if ( c == '%' )
			{
				bool unicode = false;
				if ( i + 1 < nTo )
				{
					// Check for unicode encoding (%Uxxxx)
					const tWCHAR ch = str.at( i + 1, cCP_ANSI, cSTRING_TO_UPPER );
					unicode = ( ch == 'U' );
				}

				if ( !unicode )
				{
					// Decode character %xx -> ASCII
					if ( i + 2 < nTo )
					{
						tWCHAR ch1 = str[ i + 1 ];
						tWCHAR ch2 = str[ i + 2 ];
						if ( XCHAR_TO_XDIGIT( ch1 ) && XCHAR_TO_XDIGIT( ch2 ) )
						{
							c = ch1 * 16 + ch2;

							// Skip two (already processed) characters
							i += 2;
						}
					}
				}
			}

			if( (c < 0xFF) && !urlchr_test( (tBYTE)c, urlchr_unsafe ) )
			{
				*buf++ = (tCHAR)c;
			}
			else
			{
				*buf++ = '%';
				*buf++ = 'U';
				l = ((c >> 0xC) & 0xF); *buf++ = XDIGIT_TO_XCHAR(l);
				l = ((c >> 0x8) & 0xF); *buf++ = XDIGIT_TO_XCHAR(l);
				l = ((c >> 0x4) & 0xF); *buf++ = XDIGIT_TO_XCHAR(l);
				l = ((c >> 0x0) & 0xF); *buf++ = XDIGIT_TO_XCHAR(l);
			}
		}

		++i;
	}

	return buf;
}

tDWORD    _url_simplify_path(tCHAR * pszUrl)
{
	tCHAR *wk1 = pszUrl + 1, *wk2 = wk1, *mp, *root = pszUrl;
	tDWORD nLen, nWkLen;

	/* Now at root location, cannot back up any more. */
	/* "root" will point at the '/' */
	while (*wk1)
	{
		//					PR_TRACE((this, prtNOTIFY, "wk1=%c\n", (tCHAR)*wk1));
		mp = strchr(wk1, '/');
		if (!mp)
		{
			nWkLen = strlen(wk1);
			memcpy(wk2, wk1, (nWkLen + 1) * sizeof(tCHAR));
			wk1 += nWkLen;
			wk2 += nWkLen;
			continue;
		}
		
		nLen = mp - wk1 + 1;
		memcpy(wk2, wk1, nLen * sizeof(tCHAR));
		wk2 += nLen;
		wk1 += nLen;
		if (*wk1 == '.')
		{
			//						TRACE("found '/.'\n");
			if (*(wk1+1) == '/')
			{
				/* case of /./ -> skip the ./ */
				wk1 += 2;
			}
			else if (*(wk1+1) == '.')
			{
				/* found /..  look for next / */
				//							TRACE("found '/..'\n");
				if (*(wk1+2) == '/' || *(wk1+2) == '?' || *(wk1+2) == '#' || *(wk1+2) == 0)
				{
					/* case /../ -> need to backup wk2 */
					//								TRACE("found '/../'\n");
					*(wk2-1) = '\0';  /* set end of string */
					mp = strrchr(root, '/');
					if (mp && (mp >= root))
					{
						/* found valid backup point */
						wk2 = mp + 1;
						if(*(wk1+2) != '/')
							wk1 += 2;
						else
							wk1 += 3;
					}
					else
					{
						/* did not find point, restore '/' */
						*(wk2-1) = '/';
					}
				}
			}
		}
	}
	
	*wk2 = '\0';
	return wk2 - pszUrl;
}

// It seems that these two functions (below) are duplicated in different 
// projects
inline 
bool MatchOkayA( const char* pattern )
{
	PR_ASSERT( pattern != NULL );

	bool res = false;
	if ( pattern != NULL )
	{
		if ( *pattern == '*' )
			++pattern;
		if ( *pattern )
			res = false;
		else
			res = true;
	}

	return res;
}

bool _url_matchwithpattern(const char * Pattern, const char * Name, bool bPatternWithAsterics)
{
	char ch;
	char pc;

	if (!Pattern || !Name)
		return false;

	if(*Pattern=='*')
	{
		Pattern++;
		while(*Name && *Pattern)
		{
			pc = *Pattern;
			ch = *Name;
			if ((ch == '*') || (pc == ch) || (pc == '?'))
			{
				if(_url_matchwithpattern(Pattern+1,Name+1, bPatternWithAsterics)) 
					return true;
			}

			Name++;
		}
		return MatchOkayA( Pattern );
	} 

	while(*Name && *Pattern != '*')
	{
		pc = *Pattern;
		ch = *Name;
		if(pc == ch || pc == '?')
		{
			Pattern++;
			Name++;
		} 
		else
		{
			if (pc == 0 && bPatternWithAsterics)
			{
				if (ch == '\\' || ch == '/')
					return true;
			}
			return false;
		}

	}
	if(*Name)
		return _url_matchwithpattern(Pattern,Name,bPatternWithAsterics);

	return MatchOkayA( Pattern );
}

// ########################################################################
// ########################################################################

//////////////////////////////////////////////////////////////////////////
///
/// UrlFltResults
///

///
/// Constructor
///
/// @param enabled - true if results should be stored into file, false - otherwise
///
ReportInfo::ReportInfo( const hOBJECT tracer, const bool enabled ):
	m_tracer( tracer ),
	m_enabled( enabled ),
	m_results(),
	m_prefix()
{
}

///
/// Destructor
///
ReportInfo::~ReportInfo()
{
	if ( m_enabled )
	{
		const cStrObj report_file = "%Temp%\\parctl.res";

		// Expand environment variables
		m_tracer->sysSendMsg
			(
				pmc_PRODUCT_ENVIRONMENT, 
				pm_EXPAND_ENVIRONMENT_STRING, 
				( hOBJECT ) cAutoString( report_file  ), 
				0, 
				0
			);

		cIOObj pIO( m_tracer, cAutoString( report_file ), fACCESS_WRITE, fOMODE_OPEN_ALWAYS );
		tERROR err = pIO.last_error();

		if ( PR_SUCC( err ) )
		{
			// Position for writing
			tQWORD pos = 0;

			if( PR_SUCC( err ) )
			{
				err = pIO->GetSize( &pos, 0 );
			}

			if ( PR_SUCC( err ) )
			{
				tDWORD count = 0;
				const tDWORD results_size = 
					static_cast< tDWORD >( m_results.length() * sizeof( wchar_t ) );
				err = pIO->SeekWrite
					( 
						&count, 
						pos, 
						( tPTR ) m_results.c_str(), 
						results_size
					);

				if ( PR_SUCC( err ) )
				{
					pos += count;
				}
			}

			if ( PR_SUCC( err ) )
			{
				// Footer
				const wchar_t* footer = L"\r\n\r\n\r\n";
				const tDWORD footer_size = 
					static_cast< tDWORD >( wcslen( footer ) * sizeof( wchar_t ) );
				err = pIO->SeekWrite( NULL, pos, ( tPTR ) footer, footer_size );
			}
		}
		else
		{
			PR_TRACE((this, prtIMPORTANT, "urlflt\tUrlFlt::AnalyzeObject: Can't open result file: [%S]",
				report_file.data() ));
		}
	}
}

///
/// Trace with formatting
///
void ReportInfo::Trace( const char* const format, ... )
{
	if ( m_enabled )
	{
		va_list ap;
		va_start( ap, format );

			cStrObj buf;
			buf.format_v( cCP_ANSI, format, ap );

			_Trace( buf );

		va_end( ap );
	}
}

///
/// Set prefix is about to be outputted along with the appropriate text
///
void ReportInfo::SetPrefix( const char* const prefix, ... )
{
	if ( m_enabled )
	{
		va_list ap;
		va_start( ap, prefix );

			cStrObj buf;
			buf.format_v( cCP_ANSI, prefix, ap );

			m_prefix = buf.data();

		va_end( ap );
	}
}

///
/// Trace the given buffer with the given level
///
void ReportInfo::_Trace( const cStrObj& buf )
{
	std::wstring str;
	if ( !m_prefix.empty() )
	{
		str += m_prefix;
	}

	str += buf.data();
	str += L"\r\n";

	m_results += str.c_str();
}
