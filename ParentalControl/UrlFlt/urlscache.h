#ifndef __UF_LIST_CACHE_
#define __UF_LIST_CACHE_

#include <string>
#include <vector>

#include <Prague/prague.h>
#include <Prague/pr_vector.h>

#include <klava/klavtree.h>


//////////////////////////////////////////////////////////////////////////
///
/// Forward reference
///

class ReportInfo;


// ########################################################################
// CUrlListCache

class CUrlListCache
{
public:
	struct tItemAttr
	{
		///
		/// Constructor
		///
		tItemAttr();
		
		tCHAR *		s_name;
		tDWORD		s_data;

		unsigned	s_domain_only : 1;
		unsigned	s_has_mask : 1;
		unsigned	s_proto : 1;
		unsigned	s_len : 29;	// max 536 870 912 characters
	};

	struct tExtListItem
	{
		///
		/// Constructor
		///
		tExtListItem();

		unsigned	s_enabled : 1;
		tVOID *		s_name;
		tDWORD		s_len;
		tDWORD		s_data;
	};

	struct tExtList
	{
		///
		/// Constructor
		///
		tExtList();

		unsigned	s_strobj : 1;
		unsigned	s_copydata : 1;

		virtual tDWORD	count() const = 0;
		virtual void	item(tDWORD idx, tExtListItem& item) const = 0;
	};

	struct STR
	{
		///
		/// Constructor
		///
		STR();

		tCHAR *		m_p;
		tDWORD      m_n;
	};
	
	struct tURL
	{
		///
		/// Constructor
		///
		tURL();

		enum flags { fSimplify = 0x01 };
		
		tDWORD BufSize(const cStrObj& strUrl);
		tDWORD Normalize( const cStrObj& strUrl, tCHAR * buf, tUINT bufSize, tDWORD nFlags );
		tDWORD Parse(tCHAR * strUrl, tDWORD len);
		void   Reoffset(tLONG nOffset);
		void   GetSiteUrl(STR& _SiteUrl) const;

		///
		/// Extract full URL
		///
		void GetFullUrl( STR& fullURL ) const;

		///
		/// Extract path URL (till the last '/' but before parameters)
		///
		void GetPathUrl( STR& pathUrl ) const;

		tCHAR *		m_proto;
		tDWORD      m_protoLen;
		tCHAR *		m_auth;
		tDWORD		m_authLen;
		tCHAR *		m_host;
		tDWORD		m_hostLen;
		tCHAR*		m_port;
		tDWORD		m_portLen;
		tCHAR *		m_path;
		tDWORD		m_pathLen;
		tCHAR *     m_url;
		tDWORD      m_urlLen;
	};

	///
	/// Search context
	///
	class tSearchContext
	{
	public:

		///
		/// Check if search should be continued
		///
		/// @param attr - item's attributes is about to be found
		/// @return true if search should be continued, false - otherwise
		///
		virtual bool CheckForContinue( const tItemAttr* attr ) = 0;

		///
		/// Check if at least one match is found
		///
		/// @return true if at least one match is found, false - otherwise
		///
		virtual bool IsMatchFound() = 0;
	};

	///
	/// Tree search context
	///
	class tTreeSearchContext: public tSearchContext
	{
	public:

		///
		/// Find data in three
		///
		virtual bool FindDataInTree
			( 
				sSEARCHER_RT_CTX* tree_search_ctx, 
				const char* p_str, 
				uint32_t p_len 
			) = 0;
	};

public:

	///
	/// Constructor
	///
	CUrlListCache();

	///
	/// Destructor
	///
	~CUrlListCache();

	const iObj*&	owner() { return m_pCacheData.owner(); }

	///
	/// Preprocess the given list
	///
	tERROR Build( hOBJECT tracer, tExtList& list );

	tERROR		Free();

	///
	/// Find the match for the given URL (linear search)
	///
	/// @param url - URL for searching
	/// @param context - search context
	/// @return item's attributes if it is found, false - otherwise
	///
	bool Find
		( 
			const tURL& url, 
			ReportInfo& info, 
			tTreeSearchContext* context = NULL 
		) const;

	void		Dump();

private:

	///
	/// Find the match for the given URL (linear search)
	///
	/// @param url - URL for searching
	/// @param context - search context
	/// @return item's attributes if it is found, false - otherwise
	///
	bool CompareWithPattern
		( 
			const tURL& url, 
			ReportInfo& info, 
			tSearchContext* context = NULL 
		) const;

	///
	/// Find the match for the given URL (quick search but can be used with
	/// simple URLs only)
	///
	/// @param url - URL for searching
	/// @param context - search context
	/// @return item's attributes if it is found, false - otherwise
	///
	bool FindInTree
		( 
			const tURL& url, 
			tTreeSearchContext* context
		) const;

private:

	cVectorSimple(tItemAttr)	m_vItems;
	cMemChunkAligned<0x400>		m_pCacheData;

	mutable sHEAP_CTX			m_heap_ctx;
	sRT_CTX*					m_tree_ctx;
};

// ########################################################################
// ########################################################################

//////////////////////////////////////////////////////////////////////////
///
/// tItemAttr
///

///
/// Constructor
///
inline
CUrlListCache::tItemAttr::tItemAttr():
	s_name( NULL ),
	s_data( 0 ),
	s_domain_only( 0 ),
	s_has_mask( 0 ),
	s_proto( 0 ),
	s_len( 0 )
{
}

//////////////////////////////////////////////////////////////////////////
///
/// tURL
///

///
/// Constructor
///
inline
CUrlListCache::tURL::tURL():
	m_proto( NULL ),
	m_protoLen( 0 ),
	m_auth( NULL ),
	m_authLen( 0 ),
	m_host( NULL ),
	m_hostLen( 0 ),
	m_port( NULL ),
	m_portLen( 0 ),
	m_path( NULL ),
	m_pathLen( 0 ),
	m_url( NULL ),
	m_urlLen( 0 )
{
}

//////////////////////////////////////////////////////////////////////////
///
/// tExtListItem
///

///
/// Constructor
///
inline
CUrlListCache::tExtListItem::tExtListItem():
	s_enabled( 0 ),
	s_name( NULL ),
	s_len( 0 ),
	s_data( 0 )
{
}

//////////////////////////////////////////////////////////////////////////
///
/// tExtList
///

///
/// Constructor
///
inline
CUrlListCache::tExtList::tExtList():
	s_strobj( 0 ),
	s_copydata( 0 )
{
}

//////////////////////////////////////////////////////////////////////////
///
/// STR
///

///
/// Constructor
///
inline
CUrlListCache::STR::STR():
	m_p( NULL ),
	m_n( 0 )
{
}

//////////////////////////////////////////////////////////////////////////
///
/// Cache
///
template< class Key, class Value >
class UrlFltTTLCache
{

private:

	///
	/// Cache item
	///
	struct CacheItem
	{
		CacheItem();

		Key m_key;
		Value m_value;

		time_t m_time;
		tDWORD m_ttl;
	};

public:

	///
	/// Size type
	///
	typedef typename std::vector< CacheItem >::size_type size_type;

	///
	/// Invalid position
	///
	static const size_type npos;

public:

	///
	/// Constructor
	///
	UrlFltTTLCache();

public:

	///
	/// Add the given item
	///
	void Add
		( 
			const Key& key, 
			const Value& value, 
			tDWORD ttl, 
			const time_t now
		);

	///
	/// Find the item with the given key
	///
	size_type Find( const Key& key, bool update );

	///
	/// Get key of the given item
	///
	Key& GetKey( size_type index );

	///
	/// Get value of the given item
	///
	Value& GetValue( size_type index );

public:

	///
	/// Clear expired cache items
	///
	void Clear( hOBJECT tracer, time_t now );

public:

	///
	/// Get size
	///
	size_type GetSize();

	///
	/// Get max size
	///
	size_type GetMaxSize();

	///
	/// Set max size
	///
	void SetMaxSize( size_type size );

private:

	///
	/// Find the item with the given key
	///
	size_type _find( const Key& key );

private:

	///
	/// Set of cached items
	/// They are aligned in the ascending order: from the least recent 
	/// (at the beginning) and up to the most recent (at the end)
	///
	std::vector< CacheItem > m_items;

	///
	/// Max possible size
	///
	size_type m_maxSize;
};

//////////////////////////////////////////////////////////////////////////

template< class Key, class Value >
inline
UrlFltTTLCache< Key, Value >::CacheItem::CacheItem():
	m_key(),
	m_value(),
	m_time(),
	m_ttl()
{
}

//////////////////////////////////////////////////////////////////////////

///
/// Invalid position
///
template< class Key, class Value >
const typename UrlFltTTLCache< Key, Value >::size_type UrlFltTTLCache< Key, Value >::npos = 
	UrlFltTTLCache< Key, Value >::size_type( -1 );

///
/// Constructor
///
template< class Key, class Value >
UrlFltTTLCache< Key, Value >::UrlFltTTLCache():
	m_items(),
	m_maxSize( 256 )
{
}

///
/// Add the given item
///
template< class Key, class Value >
void UrlFltTTLCache< Key, Value >::Add
	( 
		const Key& key, 
		const Value& value, 
		tDWORD ttl, 
		const time_t now
	)
{
	const size_type index = Find( key, false );
	if ( index == npos )
	{
		CacheItem item;
		item.m_key = key;
		item.m_value = value;
		item.m_ttl = ttl;
		item.m_time = now;

		const size_type size = m_items.size();
		if ( size >= m_maxSize )
		{
			if ( !m_items.empty() )
			{
				// Remove the least recent item
				m_items.erase( m_items.begin() );
			}
		}

		// Add as the most recent item
		m_items.push_back( item );
	}
}

///
/// Find the item with the given key
///
template< class Key, class Value >
typename UrlFltTTLCache< Key, Value >::size_type UrlFltTTLCache< Key, Value >::Find
	( 
		const Key& key, 
		const bool update 
	)
{
	size_type index = _find( key );

	if ( index != npos )
	{
		if ( update )
		{
			if ( index < m_items.size() - 1 )
			{
				// Re-add the most recent item
				if ( !m_items.empty() )
				{
					PR_ASSERT( index >= 0 && index < m_items.size() );
					CacheItem t = m_items[ index ];
					m_items.erase( m_items.begin() + index );
					m_items.push_back( t );
					index = m_items.size() - 1;
				}
			}
		}
	}

	return index;
}

///
/// Get key of the given item
///
template< class Key, class Value >
Key& UrlFltTTLCache< Key, Value >::GetKey( const size_type index )
{
	PR_ASSERT( index >= 0 && index < m_items.size() );
	return m_items[ index ].m_key;
}

///
/// Get value of the given item
///
template< class Key, class Value >
Value& UrlFltTTLCache< Key, Value >::GetValue( const size_type index )
{
	PR_ASSERT( index >= 0 && index < m_items.size() );
	return m_items[ index ].m_value;
}

//zzz join with the same class! get rid of template!

///
/// Clear expired cache items
///
template< class Key, class Value >
void UrlFltTTLCache< Key, Value >::Clear( hOBJECT tracer, time_t now )
{
	const size_type size = m_items.size();
	size_type i = m_items.size() - 1;
	while ( i >= 0 && i < m_items.size() )
	{
		PR_ASSERT( i >= 0 && i < m_items.size() );
		const CacheItem& item = m_items[ i ];

		const tDWORD time_diff = now - item.m_time;
		if ( time_diff > item.m_ttl )
		{
			PR_TRACE(( tracer, prtNOTIFY, 
				"urlflt\tUrlFlt::UrlFltTTLCache: Cache: url removed: [%s] => [%s] (cache size = %d)",
				item.m_key.c_str(), item.m_value.c_str(), ( int ) m_items.size() ));

			m_items.erase( m_items.begin() + i );
			if ( i == m_items.size() )
			{
				--i;
			}
		}
		else
		{
			--i;
		}
	}
}

///
/// Get size
///
template< class Key, class Value >
inline
typename UrlFltTTLCache< Key, Value >::size_type UrlFltTTLCache< Key, Value >::GetSize()
{
	return m_items.size();
}

///
/// Get max size
///
template< class Key, class Value >
inline
typename UrlFltTTLCache< Key, Value >::size_type UrlFltTTLCache< Key, Value >::GetMaxSize()
{
	return m_maxSize;
}

///
/// Set max size
///
template< class Key, class Value >
inline
void UrlFltTTLCache< Key, Value >::SetMaxSize( const size_type size )
{
	m_maxSize = size;
}


///
/// Find the item with the given key
///
template< class Key, class Value >
typename UrlFltTTLCache< Key, Value >::size_type UrlFltTTLCache< Key, Value >::_find( const Key& key )
{
	const size_type size = m_items.size();
	for ( size_type i = size - 1; i + 1 > 0; --i )
	{
		PR_ASSERT( i >= 0 && i < m_items.size() );
		const CacheItem& item = m_items[ i ];

		if ( item.m_key == key )
		{
			return i;
		}
	}

	return npos;
}

//////////////////////////////////////////////////////////////////////////
///
/// Results tracer
///
class ReportInfo
{
public:

	///
	/// Constructor
	///
	/// @param enabled - true if results should be stored into file, false - otherwise
	///
	ReportInfo(  const hOBJECT tracer, bool enabled );

	///
	/// Destructor
	///
	~ReportInfo();

public:

	///
	/// Check if results tracer is enabled
	///
	bool IsEnabled() const;

	///
	/// Trace with formatting
	///
	void Trace( const char* format, ... );

public:

	///
	/// Set prefix is about to be outputted along with the appropriate text
	///
	void SetPrefix( const char* prefix, ... );

	///
	/// Clear prefix
	///
	void ClearPrefix();

protected:

	///
	/// Trace the given buffer with the given level
	///
	virtual void _Trace( const cStrObj& buf );

private:

	///
	/// Restrict copying
	///
	ReportInfo( const ReportInfo& );

	///
	/// Restrict copying
	///
	ReportInfo& operator=( const ReportInfo& );

private:

	const hOBJECT m_tracer;

	bool m_enabled;

	typedef std::wstring Results;
	Results m_results;

	std::wstring m_prefix;
};

///
/// Check if result tracer is enabled
///
inline
bool ReportInfo::IsEnabled() const
{
	return m_enabled;
}

///
/// Clear prefix
///
inline
void ReportInfo::ClearPrefix()
{
	SetPrefix( "" );
}


#endif // __UF_LIST_CACHE_
