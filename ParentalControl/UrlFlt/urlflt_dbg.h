
#ifndef __URL_FLT_DBG_UTILS_H
#define __URL_FLT_DBG_UTILS_H

//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

	///
	/// Check pointer
	///
	template< typename T1, typename T2 >
	inline
	void CHECK_PTR( const T1* buf, const T2 bufSize, const T1* ptr )
	{
		_ASSERT( ptr >= buf && ptr < ( buf + bufSize ) );
	}

	///
	/// Check pointer range
	/// 
	template< typename T1 >
	inline
	void CHECK_PTR_RANGE( const T1* buf, const int bufSize, const T1* ptr1, const int ptrSize )
	{
		CHECK_PTR( buf, bufSize, ptr1 );
		CHECK_PTR( buf, bufSize, ptr1 + ptrSize );
	}

	///
	/// Check URL
	///
	inline
	void CHECK_URL( const tCHAR* buf, const tUINT bufSize, const CUrlListCache::tURL& url )
	{
		if ( url.m_proto || url.m_protoLen )
		{
			CHECK_PTR_RANGE( buf, bufSize, url.m_proto, url.m_protoLen );
		}

		if ( url.m_auth || url.m_authLen )
		{
			CHECK_PTR_RANGE( buf, bufSize, url.m_auth, url.m_authLen );
		}

		if ( url.m_host || url.m_hostLen )
		{
			CHECK_PTR_RANGE( buf, bufSize, url.m_host, url.m_hostLen );
		}

		if ( url.m_port || url.m_portLen )
		{
			CHECK_PTR_RANGE( buf, bufSize, url.m_port, url.m_portLen );
		}

		if ( url.m_path || url.m_pathLen )
		{
			CHECK_PTR_RANGE( buf, bufSize, url.m_path, url.m_pathLen );
		}

		if ( url.m_url || url.m_urlLen )
		{
			CHECK_PTR_RANGE( buf, bufSize, url.m_url, url.m_urlLen );
		}
	}
#else

	#define CHECK_PTR( buf, bufSize, ptr )
	#define CHECK_PTR_RANGE( buf, bufSize, ptr1, ptrSize )
	#define CHECK_URL( buf, bufSize, url )

#endif // _DEBUG


#endif // __URL_FLT_DBG_UTILS_H
