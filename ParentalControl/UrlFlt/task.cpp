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



#define PR_IMPEX_DEF

// AVP Prague stamp begin( Interface version,  )
#define Task_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include "task.h"
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_remote.h>
#include <Prague/pr_time.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/e_loader.h>
#include <Prague/iface/i_csect.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_mutex.h>
#include <Prague/iface/i_reg.h>
#include <Prague/iface/i_root.h>

#include <Prague/iface/i_seqio.h>
#include <Extract/iface/i_transformer.h>
#include <Extract/plugin/p_inflate.h>

// iCountCRC32
#include <hashutil/hashutil.h>

#include <ParentalControl/plugin/p_urlflt.h>
#include <ParentalControl/structs/s_urlflt.h>

#include <ProductCore/environment.h>
// AVP Prague stamp end

#include <stdlib.h> 
#include <minmax.h>

#include <kl_byteswap.h>

#include "..\..\Components\Updater\Transport\PragueTransport\p_transportImplementation.h"
#include "..\..\Components\Updater\Transport\ProxyServerAddressDetector\p_proxyserveraddressdetectorimplementation.h"
#include "..\..\Components\Updater\KeepUp2Date\Client\PragueWrapper\UpdaterConfigurator\i_updaterconfigurator.h"
#include "..\..\Components\Updater\KeepUp2Date\Client\helper\localize.h"

#include <Prague/plugin/p_win32loader.h>

#include <PPP/structs/s_bl.h>

#include <ProductCore/plugin/p_processmonitor.h>
#include <ProductCore/structs/ProcMonM.h>
#include <Updater/updater.h>
#include <Version/ver_product.h>

// http:content_type
#include <TrafficMonitor2/ProtocollersEx/http/message.h>
// http::analyzer
#include <TrafficMonitor2/ProtocollersEx/http/analyzerimpl.h>

#include "CharsetDecode.h"

#pragma warning( disable: 4005 )
#include <boost/regex.hpp>
#pragma warning( default: 4005 )

// std::accumulate
#include <numeric>

// Debug utils
#include "urlflt_dbg.h"

//////////////////////////////////////////////////////////////////////////
///
/// Typedefs
///

typedef std::vector< tBYTE > tDataVector;


//////////////////////////////////////////////////////////////////////////

/*
#ifdef _DEBUG
void _TestWriteData(cIO * p_hDestination);
void _TestDownload(UrlFlt * _this);


struct CUrlFltTestThr : public cThreadTaskBase
{
	//static CUrlFltTestThr * _new(UrlFlt * pUrlFlt) { return new CUrlFltTestThr(pUrlFlt); }
	static CUrlFltTestThr* _new
		( 
			UrlFlt& urlFlt,
			const cUrlFilteringRequest& urlFltRequest,
			UrlFlt::tProfile* profile,
			tQWORD nDenyCategoriesMask,
			const cUrlFltReportEvent& evt
		) 
	{ 
		//return new CUrlFltTestThr(pUrlFlt); 
		return new CUrlFltTestThr
			( 
				urlFlt, 
				urlFltRequest,
				profile, 
				nDenyCategoriesMask, 
				evt
			); 
	}
	
	//CUrlFltTestThr(UrlFlt * pUrlFlt) : m_pUrlFlt(pUrlFlt) {}
	CUrlFltTestThr
		( 
			UrlFlt& urlFlt,
			const cUrlFilteringRequest& urlFltRequest, 
			UrlFlt::tProfile* profile,
			tQWORD nDenyCategoriesMask,
			const cUrlFltReportEvent& evt
		): 
		m_urlFlt( urlFlt ),
		m_urlFltRequest( urlFltRequest ),
		m_profile( profile ),
		m_nDenyCategoriesMask( nDenyCategoriesMask ),
		m_Evt( evt )
	{
		m_profile->addRef();
	}

	~CUrlFltTestThr()
	{
		if( m_profile )
		{
			m_profile->release();
			m_profile = NULL;
		}
	}
	
	void do_work()
	{
		/*
		cUrlFilteringRequest _url;
		_url.m_nPID = ::GetCurrentProcessId();
		*

/*
		_url.m_sURL = L"http://www.asdasdvasdv.ru";
		m_pUrlFlt->ProcessObject(&_url);

		_url.m_sURL = L"http://89.108.86.213/PARCTL_test1.html";
		m_pUrlFlt->ProcessObject(&_url);

		_url.m_sURL = L"http://sex.ru/PARCTL_test2.html";
		m_pUrlFlt->ProcessObject(&_url);

		_url.m_sURL = L"http://sex1.ru/PARCTL_test1.html";
		m_pUrlFlt->ProcessObject(&_url);

		_url.m_sURL = L"http://anonym:ascasc@194.67.57.26:80/svasvv";
		m_pUrlFlt->ProcessObject(&_url);

		_url.m_sURL = L"http://fotoxxx.ru/";
		m_pUrlFlt->ProcessObject(&_url);
*
	
		const int SITE_COUNT = 23;
		LPWSTR strTextSites[ SITE_COUNT ] =
		{
			L"http://francheska07.livejournal.com/",
			L"http://www.prastitutki.ru/35/anket815.html",
			L"http://www.prastitutki.ru",
			L"http://sex.hitlife.net/",
			L"sex.net",
			L"google.co.jp",
			L"http://yahoo.co.jp/",
			L"mail.ru",
			L"adult.com",
			L"isex.ru",
			L"adultxpix.com",
			L"cocku.ru",
			L"seksmagazin.ru",
			L"www.ya.ru",
			L"www.yandex.ru",
			L"http://a248.e.akamai.net/f/248/5462/3h/hints.netflame.cc/service/script/www.kaspersky.com",
			L"http://mustangranch.com/style/style1_9.css",
			L"http://adultxpix.com/favicon",
			L"cocku.ru",
			L"http://cocku.ru/style1.css",
			L"http://yandex.ru",
			L"http://bs.yandex.ru/count/OIyWn9sGYsi30BjUS457KTG5x05fMgA0HZ3Aj7r4gacp8CSCdKHcpDMkRz6dGNm4",
			L"http://img.yandex.net/i/icons.png",
		};

		::Sleep( 5000 );

		cAutoObj<cToken> hToken;
		cStrObj strUser;
		tDWORD nType = 0;
		tERROR err = m_urlFlt._GetUserByPID( m_urlFltRequest.m_nPID, hToken, strUser, nType);

		if ( PR_SUCC( err ) )
		{
			for ( int i = 0; i < SITE_COUNT; ++i )
			{
				m_urlFltRequest.m_sURL = strTextSites[ i ];

				hIO content = NULL;
				err = m_urlFlt.loadPage( m_urlFltRequest, &content, hToken );
				if ( PR_SUCC( err ) )
				{
					tERROR errCheck = m_urlFlt.DoProcessing
						(
							&m_urlFltRequest,
							content,
							m_profile,
							m_nDenyCategoriesMask,
							hToken, 
							m_Evt
						);

					PR_ASSERT( content != NULL );
					if ( content != NULL )
					{
						CALL_SYS_ObjectClose( content );
						content = NULL;
					}
				}

				::Sleep( 10000 );
			}
		}

		::Sleep( 5000 );
	}

	void on_exit() { delete this; }

	//UrlFlt * m_pUrlFlt;
	UrlFlt& m_urlFlt;
	cUrlFilteringRequest m_urlFltRequest;
	UrlFlt::tProfile* m_profile;
	tQWORD m_nDenyCategoriesMask;
	cUrlFltReportEvent m_Evt;
};

#endif // _DEBUG
*/

// #################################################################################
// #################################################################################

void	_A2W(const char * src, tDWORD nSrcLen, std::wstring& dst, tUINT cp)
{
	dst.resize(nSrcLen*2);
	int nChars = ::MultiByteToWideChar(cp, 0, src, nSrcLen, &dst[0], dst.size());
	dst.resize(nChars);
}

tDWORD	_GetTz()
{
	TIME_ZONE_INFORMATION _tz;
	if( ::GetTimeZoneInformation(&_tz) == TIME_ZONE_ID_INVALID )
		return 0;
	return _tz.Bias;
}

///
/// Convert time to local time
///
static
time_t _TimeToLocal(time_t nTime)
{
	tQWORD v = (_TQW((tDATETIME&)cDateTime(nTime)) - DATETIME_FILETIME_BASE)/10;
	FILETIME ft; ft.dwLowDateTime = (DWORD)v; ft.dwHighDateTime = (DWORD)(v >> 32);
	FILETIME ftLoc;
	::FileTimeToLocalFileTime(&ft, &ftLoc);
	v = (tQWORD)ftLoc.dwLowDateTime|((tQWORD)ftLoc.dwHighDateTime << 32);
	
	tDATETIME dt; _TQW(dt) = DATETIME_FILETIME_BASE + v*10;
	return (time_t)cDateTime(&dt);
}

///
/// Convert time to system time
///
static
void _TimeToSystem( time_t nTime, SYSTEMTIME& sysTime )
{
	tQWORD v = (_TQW((tDATETIME&)cDateTime(nTime)) - DATETIME_FILETIME_BASE)/10;
	FILETIME ft; ft.dwLowDateTime = (DWORD)v; ft.dwHighDateTime = (DWORD)(v >> 32);

	::ZeroMemory( &sysTime, sizeof( sysTime ) );
	::FileTimeToSystemTime( &ft, &sysTime );
}

bool	_Sock_Init()
{
	WSADATA wsaData;
	return !WSAStartup(MAKEWORD(2, 0), &wsaData);
}

void	_Sock_DeInit()
{
	WSACleanup();
}

bool	_Sock_GetHostNameByIP(const tCHAR * strIp, tCHAR * strHost, tDWORD nHost)
{
	if( !nHost )
		return false;
	
	strHost[0] = 0;
	
	unsigned int nAddr = inet_addr(strIp);
	hostent * _he = gethostbyaddr((char *)&nAddr, 4, AF_INET);
	if( _he )
	{
		strncpy(strHost, _he->h_name, nHost);
		return true;
	}
	
	//int errS = WSAGetLastError();

	strHost[0] = 0;
	return false;
}

/*
static char * __cdecl stristr (
					   const char * str1,
					   const char * str2
					   )
{
	char *cp = (char *) str1;
	char *s1, *s2;

	if ( !*str2 )
		return((char *)str1);

	while (*cp)
	{
		s1 = cp;
		s2 = (char *) str2;

		while ( *s1 && *s2 && !(toupper(*s1)-toupper(*s2)) )
			s1++, s2++;

		if (!*s2)
			return(cp);

		cp++;
	}

	return(NULL);

}
*/

//////////////////////////////////////////////////////////////////////////
///
/// Helper functions
///

///
/// Compare the given string with the given data
///
static 
bool _CompareData
	( 
		const tDataVector& data, 
		const tDWORD offset, 
		const int* const pattern, 
		const tDWORD patternSize 
	)
{
	bool res = false;

	// Check if there is enough chars in the source data for comparison
	if ( data.size() >= ( patternSize + offset ) )
	{
		tDWORD i = 0;
		for ( tDWORD j = offset; i < patternSize; ++i, ++j )
		{
			PR_ASSERT( i >= 0 && i < patternSize );
			PR_ASSERT( j >= 0 && j < data.size() );

			if ( pattern[ i ] != data[ j ] )
				break;
		}

		if ( i == patternSize )
			res = true;
	}

	return res;
}


///
/// Check if the given amount of bytes is available for reading
///
/// @param data - source available for reading
/// @param offset - source offset
/// @param bytes - count of bytes for checking
///
static inline 
bool bytesAvailable
	( 
		const tDataVector::size_type data_size, 
		const tDataVector::size_type offset,
		const tDataVector::size_type bytes
	)
{
	return ( offset + bytes <= data_size );
}

///
/// Analyze GZIP header
///
/// See RFC 1952 for additional information
/// http://www.faqs.org/rfcs/rfc1952.html
///
/// @param data - GZIP data
/// @param data - GZIP data size
/// @param compr_data_offset - offset of the compressed blocks within GZIP data
/// @param compr_data_size - size of the compressed blocks within GZIP data
/// @param uncompr_crc32 - output header's CRC32 for uncompressed data
/// @param uncompr_data_size - output header's size for uncompressed data
///
static
tERROR analyzeGzipHeader
	(
		const tDataVector& data,
		const tDataVector::size_type data_size,
		tDataVector::size_type& compr_data_offset, 
		tDataVector::size_type& compr_data_size, 
		tDWORD& uncompr_crc32, 
		tDWORD& uncompr_data_size 
	)
{
	PR_ASSERT( data.size() == data_size + 1 ); // Null-terminating character

	tDataVector::size_type next = 0;

	// ID1, ID2
	if ( !bytesAvailable( data_size, next, 2 ) )
		return errFOUND;

	const tBYTE id1 = data[ next++ ]; // +0
	const tBYTE id2 = data[ next++ ]; // +1
	if ( id1 != 31 || id2 != 139 )
		return errPARAMETER_INVALID;

	// Compression method: deflate
	if ( !bytesAvailable( data_size, next, 1 ) )
		return errPARAMETER_INVALID;

	const tBYTE compr_method = data[ next++ ]; // +2
	if ( compr_method != 8 ) // deflate
		return errPARAMETER_INVALID;

	// Flags
	if ( !bytesAvailable( data_size, next, 1 ) )
		return errPARAMETER_INVALID;

	const tBYTE flags = data[ next++ ]; // +3

	// MTIME, XFL, OS
	if ( !bytesAvailable( data_size, next, 6 ) )
		return errPARAMETER_INVALID;

	// Skip MTIME: 4 bytes
	// Skip XFL: 1 byte
	// Skip OS: 1 byte
	next += 6;

	// Check flags

	if ( flags & 0xE0 ) // Reserved: must be 0
		return errPARAMETER_INVALID;

	if ( flags & 0x4 ) // FEXTRA: Extra field
	{
		if ( !bytesAvailable( data_size, next, 2 ) )
			return errPARAMETER_INVALID;

		int len = data[ next++ ];
		len |= ( data[ next++ ] << 8 );
		// Skip
		next += len;
	}

	if ( flags & 0x8 ) // FNAME: File name
	{
		while ( ( next < data_size ) && ( data[ next ] != 0 ) )
		{
			// Skip
			++next;
		}
	}

	if ( flags & 0x10 ) // FCOMMENT: Comment
	{
		while ( ( next < data_size ) && ( data[ next ] != 0 ) )
		{
			// Skip
			++next;
		}
	}

	if ( flags & 0x2 ) // FHRC: Header CRC
	{
		// Skip
		next += 2;
	}

	// Currently we are on the compressed blocks
	compr_data_offset = next;

	compr_data_size = ( data_size - next - 8 );
	PR_ASSERT( compr_data_size >= 0 );

	// Skip
	next += compr_data_size;

	// CRC32
	if ( !bytesAvailable( data_size, next, 4 ) )
		return errPARAMETER_INVALID;

	uncompr_crc32 = data[ next++ ];
	uncompr_crc32 |= ( data[ next++ ] << 8 );
	uncompr_crc32 |= ( data[ next++ ] << 16 );
	uncompr_crc32 |= ( data[ next++ ] << 24 );

	// ISIZE: Input size
	if ( !bytesAvailable( data_size, next, 4 ) )
		return errPARAMETER_INVALID;

	uncompr_data_size = data[ next++ ];
	uncompr_data_size |= ( data[ next++ ] << 8 );
	uncompr_data_size |= ( data[ next++ ] << 16 );
	uncompr_data_size |= ( data[ next++ ] << 24 );

	return errOK;
}

///
/// Decompress gzip'ed data
///
/// @param tracer - trace object
/// @param data - gzip data
/// @param data_size - gzip data size
/// @param compr_data_offset - compressed data offset in the given gzip data
/// @param compr_data_size - compressed data size
/// @param uncompr_data - decompressed data
///
static
tERROR decompressGzipData
	(
		const hOBJECT tracer, 
		const tDataVector& data, 
		const tDataVector::size_type data_size,
		const tDataVector::size_type compr_data_offset, 
		const tDataVector::size_type compr_data_size,
		tDataVector& uncompr_data 
	)
{
	PR_ASSERT( data.size() == data_size + 1 ); // Null-terminating character
	if ( data.empty() || ( data_size == 0 ) )
		return errPARAMETER_INVALID;
	if ( compr_data_size <= 0 )
		return errPARAMETER_INVALID;
	if ( compr_data_offset + compr_data_size > data_size )
		return errPARAMETER_INVALID;

	tERROR err = errOK;

	// Create heap
	hHEAP hHeap = NULL;
	err = ( tERROR ) CALL_SYS_ObjectCreateQuick( tracer, ( hOBJECT* ) &hHeap, IID_HEAP, PID_ANY, 0 );

	if( PR_SUCC( err ) )
	{
		// Transform window
		tBYTE* Wnd = NULL;
		tDWORD WndSize = 32 * 1024;
		err = CALL_Heap_Alloc( hHeap, ( tPTR* )&Wnd, WndSize );

		if ( PR_SUCC( err ) )
		{
			// IO buffers
			tDWORD Size = 0;
			tBYTE* Buf = NULL;

			const int ZIP_RBUF_SIZE = 8 * 1024;
			const int ZIP_WBUF_SIZE = 8 * 1024;

			// Alloc IO buffers
			Size  = ( ZIP_RBUF_SIZE + ZIP_WBUF_SIZE );
			err = CALL_Heap_Alloc( hHeap, ( tPTR* )&Buf, Size );

			if ( PR_SUCC( err ) )
			{
				// Create inflater
				hTRANSFORMER hTrans = NULL;
				err = CALL_SYS_ObjectCreateQuick( tracer, &hTrans, IID_TRANSFORMER, PID_INFLATE, SUBTYPE_ANY );

				if ( PR_SUCC( err ) )
				{
					// Init transformer window
					CALL_SYS_PropertySet( hTrans, NULL, pgTRANSFORM_WINDOW, &Wnd, sizeof( Wnd ) );
					CALL_SYS_PropertySet( hTrans, NULL, pgTRANSFORM_WINDOW_SIZE, &WndSize, sizeof( WndSize ) );

					Size = ZIP_RBUF_SIZE; // ?
					CALL_SYS_PropertySet( hTrans, NULL, pgINPUT_BUFF, &Buf, sizeof( Buf ) );
					CALL_SYS_PropertySet( hTrans, NULL, pgINPUT_BUFF_SIZE, &Size, sizeof( Size ) );

					Buf += ZIP_RBUF_SIZE; // ?
					Size = ZIP_WBUF_SIZE;
					CALL_SYS_PropertySet( hTrans, NULL, pgOUTPUT_BUFF, &Buf, sizeof( Buf ) );
					CALL_SYS_PropertySet( hTrans, NULL, pgOUTPUT_BUFF_SIZE, &Size, sizeof( Size ) );

					tINT WndMul = -15; // ?
					CALL_SYS_PropertySet( hTrans, NULL, plINFLATE_WINDOW_MULTIPIER, &WndMul, sizeof( tINT ) );

					// Create output IO
					hIO hDstIO = NULL;
					err = CALL_SYS_ObjectCreate( tracer, &hDstIO, IID_IO,PID_TMPFILE, SUBTYPE_ANY );

					if ( PR_SUCC( err ) )
					{
						CALL_SYS_PropertySetDWord( ( hOBJECT ) hDstIO, pgOBJECT_ACCESS_MODE, fACCESS_RW );
						CALL_SYS_PropertySetDWord( ( hOBJECT ) hDstIO, pgOBJECT_OPEN_MODE, fOMODE_CREATE_ALWAYS | fOMODE_SHARE_DENY_READ );
						err = CALL_SYS_ObjectCreateDone( hDstIO );

						if ( PR_SUCC( err ) )
						{
							// Create output seq-io
							hSEQ_IO hDstSeqIO = NULL;
							err = CALL_SYS_ObjectCreateQuick( hDstIO, &hDstSeqIO, IID_SEQIO, PID_ANY, SUBTYPE_ANY );

							if ( PR_SUCC( err ) )
							{
								// Create input io
								hIO hSrcIO = NULL;
								err = CALL_SYS_ObjectCreate( tracer, &hSrcIO, IID_IO, PID_TMPFILE,SUBTYPE_ANY );

								if ( PR_SUCC( err ) )
								{
									CALL_SYS_PropertySetDWord( ( hOBJECT ) hSrcIO, pgOBJECT_ACCESS_MODE, fACCESS_RW );
									CALL_SYS_PropertySetDWord( ( hOBJECT ) hSrcIO, pgOBJECT_OPEN_MODE, fOMODE_CREATE_ALWAYS | fOMODE_SHARE_DENY_READ );
									err = CALL_SYS_ObjectCreateDone( hSrcIO );

									if ( PR_SUCC( err ) )
									{
										// Put data to the source IO
										PR_ASSERT( compr_data_offset + compr_data_size <= data_size );
										err = hSrcIO->SeekWrite( NULL, 0, ( tPTR* ) &data[ compr_data_offset ], compr_data_size );

										if ( PR_SUCC( err ) )
										{
											// Create input seq-io
											hSEQ_IO hSrcSeqIO = NULL;
											err = CALL_SYS_ObjectCreateQuick( hSrcIO, &hSrcSeqIO, IID_SEQIO, PID_ANY, SUBTYPE_ANY );

											if ( PR_SUCC( err ) )
											{
												// Setup transformer io
												CALL_SYS_PropertySet( hTrans, NULL, pgINPUT_SEQ_IO,  &hSrcSeqIO, sizeof( hSrcSeqIO ) );
												CALL_SYS_PropertySet( hTrans, NULL, pgOUTPUT_SEQ_IO, &hDstSeqIO, sizeof( hDstSeqIO ) );

												// Re-seek files
												CALL_SeqIO_Seek( hDstSeqIO, NULL, 0, cSEEK_SET );
												CALL_SeqIO_Seek( hSrcSeqIO, NULL, 0, cSEEK_SET );

												// Reset transformer
												CALL_Transformer_Reset( hTrans, INFLATE_RESET_STATE );

												// Setup transformer sizes
												tQWORD PSize = 0;
												tQWORD USize = 0xffffffffffffffff;
												CALL_SYS_PropertySet( hTrans, NULL, pgTRANSFORM_INPUT_SIZE, &PSize, sizeof( tQWORD ) );
												CALL_SYS_PropertySet( hTrans, NULL, pgTRANSFORM_SIZE, &USize, sizeof( tQWORD ) );

												/* transform object */
												err = CALL_Transformer_Decode( hTrans, NULL );
												//DG_DumpSeqIO( hDstSeqIO, "c:\\temp\\dumpseqio.txt" );

												// Get decompressed data
												if ( PR_SUCC( err ) )
												{
													tQWORD destSize = 0;
													err = hDstIO->GetSize( &destSize, NULL );
													if ( PR_SUCC( err ) && ( destSize > 0 ) )
													{
														uncompr_data.resize( static_cast< tDataVector::size_type >( destSize ) );
														if ( uncompr_data.size() == destSize )
														{
															err = hDstIO->SeekRead( NULL, 0, &uncompr_data[ 0 ], static_cast< tDWORD >( destSize ) );
														}
													}
												}

												CALL_SYS_ObjectClose( hSrcSeqIO );
												hSrcSeqIO = NULL;
											}
										}
									}

									CALL_SYS_ObjectClose( hSrcIO );
									hSrcIO = NULL;
								}

								CALL_SYS_ObjectClose( hDstSeqIO );
								hDstSeqIO = NULL;
							}
						}

						CALL_SYS_ObjectClose( hDstIO );
						hDstIO = NULL;
					}

					CALL_SYS_ObjectClose( hTrans );
					hTrans = NULL;
				}

				//CALL_Heap_Free( hHeap, Buf );
				Buf = NULL;
			}

			//CALL_Heap_Free( hHeap, Wnd );
			Wnd = NULL;
		}

		CALL_SYS_ObjectClose( hHeap );
		hHeap = NULL;
	}

	return err;
}

///
/// Check if it is an authorization request
///
/// @return true if it is an authorization request, false - otherwise
///
static inline
bool checkForInfoResponse( const tDWORD status_code )
{
	bool res = true;

	switch ( status_code )
	{
		case 0: // Not initialized (for any reason)
		case 200: // OK
			res = false;
			break;
	}

	return res;
}

///
/// Check cache control
///
/// @return true if no cache should be supplied, false - otherwise
///
static inline
bool checkForNoCache( const cUrlFilteringRequest* request )
{
	PR_ASSERT( request != NULL );
	if ( request == NULL )
		return true;

	bool res = checkForInfoResponse( request->m_StatusCode );

	if ( !res )
	{
		switch ( request->m_CacheControl )
		{
			case http::no_cache:
				res = true;
				break;
		}
	}

	return res;
}

///
/// Dump current database results
///
static
void DUMP_DB_RES( hOBJECT tracer, const CParctlDb::Result& dbRes )
{
	if ( !dbRes.empty() )
	{
		for( int i = 0, n = dbRes.size(); i < n; ++i )
		{
			const CParctlDb::ResultCatg& c = dbRes[ i ];
			PR_TRACE(( tracer, prtNOTIFY,
				"urlflt\tUrlFlt: Database results: category: %d, weight: %d",
				( int )c.m_id, ( int ) c.m_weight ));
		}
	}
}

///
/// Get site URL
///
/// @param url - the given URL is about to be examined
/// @param strSiteUrl - site variant of the given URL
/// @param trailing - true if trailing '/' is required, false, otherwise
///
static
void getSiteURL
	( 
		const CUrlListCache::tURL& url, 
		cStrObj& strSiteURL, 
		bool trailing = false 
	)
{
	// Get site URL
	strSiteURL.clear();

	CUrlListCache::STR _strSiteURL; 
	url.GetSiteUrl(_strSiteURL);
	if( !url.m_protoLen )
		strSiteURL.append("http://");
	if( _strSiteURL.m_n )
	{
		strSiteURL.append(_strSiteURL.m_p, cCP_ANSI, _strSiteURL.m_n);
		if ( trailing )
		{
			strSiteURL.append( '/' );
		}
	}
}

///
/// Get site URL
///
/// @param url - the given URL is about to be examined
/// @param strFullUrl - full variant of the given URL
///
static
void getFullURL( const CUrlListCache::tURL& url, cStrObj& strFullURL )
{
	/*
	// Get path URL
	{
		CUrlListCache::STR _strPathURL; url.GetPathUrl( _strPathURL );
		if ( !url.m_protoLen )
		{
			strPathURL.append( "http://" );
		}
		if ( _strPathURL.m_n )
		{
			strPathURL.append( _strPathURL.m_p, cCP_ANSI, _strPathURL.m_n );
		}
	}
	*/

	// Get full URL
	strFullURL.clear();

	CUrlListCache::STR _strFullURL; 
	url.GetFullUrl( _strFullURL );
	if ( !url.m_protoLen )
	{
		strFullURL.append( "http://" );
	}
	if ( _strFullURL.m_n )
	{
		strFullURL.append( _strFullURL.m_p, cCP_ANSI, _strFullURL.m_n );
	}
}

///
/// Check if the given content is empty
///
/// @param content - content for checking
/// @return true if the content is empty, false - otherwise
///
static
bool isContentEmpty( hIO content )
{
	bool res = true;

	if ( content != NULL )
	{
		tQWORD contentSize = 0;
		tERROR err = content->GetSize( &contentSize, 0 );
		if ( PR_SUCC( err ) )
		{
			if ( contentSize > 0 )
			{
				res = false;
			}
		}
	}

	return res;
}

///
/// Extract file name from the given full file name (i.e. with path)
///
/// @param full_file_name - file name with path
/// @param file_name - extracted file name
///
static
void getFileName( const cStrObj& full_file_name, cStrObj& file_name )
{
	tDWORD last_slash = full_file_name.find_last_of( "\\" );
	last_slash = ( last_slash == cSTRING_EMPTY_LENGTH ) ? 0 : last_slash + 1;
	file_name = full_file_name.substr( last_slash );
}

///
/// Parse the given string ("<file>[;<file>]")
///
/// @param str - string for parsing
/// @param vec - output vector with parsed files
/// @param file_name_only - true if it is required to put only short file names
///		into the output vector, false - otherwise
///
static
void parseFiles( const cStrObj& str, std::vector< cStrObj >& vec, bool file_name_only )
{
	const tDWORD len = str.length();
	tDWORD pos1 = 0;
	tDWORD pos2 = 0;
	while ( pos1 < len )
	{
		pos2 = str.find_first_of( ";", pos1 );

		if ( pos2 == cSTRING_EMPTY_LENGTH )
		{
			pos2 = len;
		}

		if ( pos1 < pos2 )
		{
			cStrObj file_name = str.substr( pos1, pos2 - pos1 );
			if ( file_name_only )
			{
				getFileName( file_name, file_name );
			}

			vec.push_back( file_name );
		}

		pos1 = pos2 + 1;
	}
}

///
/// Make list of updated files
///
/// @param updatedFiles - list of updated files
/// @param str - output string of updated files
///
static
void makeListOfUpdatedBases( cUpdaterUpdatedFileList* updatedFiles, cStrObj& str )
{
	std::vector< cStrObj > vec;
	parseFiles( str, vec, true );

	// Start from the beginning
	str.clear();

	// Add already existing files
	for ( std::vector< cStrObj >::size_type i = 0; i < vec.size(); ++i )
	{
		const cStrObj& file = vec[ i ];
		if ( file.find( ".xml" ) == cSTRING_EMPTY_LENGTH )
		{
			if ( !str.empty() )
			{
				str += ";";
			}
			str += "%Bases%\\";
			str += file;
		}
	}

	for ( tDWORD i = 0, size = updatedFiles->m_files.size(); i < size; ++i )
	{
		const cUpdaterUpdatedFile& file = updatedFiles->m_files[ i ];
		if( file.m_status != cUpdaterUpdatedFile::unchanged )
		{
			if ( file.m_filename.find( ".xml" ) == cSTRING_EMPTY_LENGTH )
			{
				// Check if file is not already added
				if ( std::find( vec.begin(), vec.end(), file.m_filename ) == vec.end() )
				{
					if ( !str.empty() )
					{
						str += ";";
					}
					str += "%Bases%\\";
					str += file.m_filename;
				}
			}
		}
	}

	//str += ";%Bases%\\parctlru.dat;%Bases%\\parctlen.dat";
}

//////////////////////////////////////////////////////////////////////////
/// UrlFlt

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Task". Static(shared) property table variables
// AVP Prague stamp end

UrlFlt::UrlFlt()
	:
#ifdef _DEBUG
	m_ThreadTest("URLFLT_TEST"),
#endif // _DEBUG
	/*
	m_hCS( NULL ),
	m_dbCS( NULL ),
	*/
	m_dbManager(),
	m_cacheCS( NULL ),
	m_hCsTrMgr( NULL ),
	m_fCheckMainPage( 0 ),
	m_fUseReportFile( 0 )
{
}

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR UrlFlt::ObjectInit()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInit method" );

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::ObjectInit: start"));

	// Place your code here
	m_nFilterMsgClass = 0;
	m_nTrMax = 100;
	m_nTranspNetTimeout = 15;
	m_fTrMgrWork = 0;
	m_fHeuristicSuccess = 0;

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::ObjectInit: end"));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR UrlFlt::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectInitDone method" );

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::ObjectInitDone: start"));

	if( PR_SUCC(error) )
		error = sysCreateObjectQuick((hOBJECT*)&m_hCS, IID_CRITICAL_SECTION);

	/*
	if ( PR_SUCC( error ) )
	{
		error = sysCreateObjectQuick( ( hOBJECT* ) &m_heurObjsCS, IID_CRITICAL_SECTION );
	}
	*/

	if ( PR_SUCC( error ) )
	{
		error = sysCreateObjectQuick( ( hOBJECT* ) &m_cacheCS, IID_CRITICAL_SECTION );
	}

	if ( PR_SUCC( error ) )
	{
		//error = sysCreateObjectQuick( ( hOBJECT* ) &m_dbCS, IID_CRITICAL_SECTION );
		error = m_dbManager.Init( *this );
		if ( PR_SUCC( error ) )
		{
			PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt: database manager successfully inited"));
		}
	}

	if ( PR_SUCC( error ) )
	{
		error = sysCreateObjectQuick( ( hOBJECT* ) &m_dnsCacheCS, IID_CRITICAL_SECTION );
	}

	if ( PR_SUCC( error ) )
	{
		error = sysCreateObjectQuick( ( hOBJECT* ) &m_procCache.cs, IID_CRITICAL_SECTION );
	}
	
	sysRegisterMsgHandler(pmcPROFILE,          rmhLISTENER, m_hTM, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	sysRegisterMsgHandler(pmc_PROCESS_MONITOR, rmhLISTENER, m_hTM, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	sysRegisterMsgHandler(pmcTM_EVENTS,        rmhLISTENER, m_hTM, IID_ANY, PID_ANY, IID_ANY, PID_ANY);

	CheckProxyChanged();
	
	if( PR_SUCC(error) )
		error = _TrMgr_Init();

#ifdef _DEBUG
// 	if( PR_SUCC(error) )
	//_TestDownload(this);
	m_ThreadTest.init(this);
#endif // _DEBUG

	m_SystemStartTime = static_cast< time_t >( cDateTime::now_utc() ) - PrGetTickCount() / 1000;
	m_TimeZone = _GetTz();
	
	DataProfilesSet(false);
	DataCacheSet(false);
	
	_Sock_Init();

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::ObjectInitDone: end"));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR UrlFlt::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::ObjectPreClose method" );
	
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::ObjectPreClose: start"));

#ifdef _DEBUG
	m_ThreadTest.de_init();
#endif // _DEBUG

	_Sock_DeInit();

	DataProfilesSet(true);
	DataCacheSet(true);

	{
		//cAutoCS _lock( m_dbCS, true );
		_Clear();
	}
	
	_TrMgr_DeInit();
	
	sysUnregisterMsgHandler(pmcTM_EVENTS, m_hTM);
	sysUnregisterMsgHandler(pmc_PROCESS_MONITOR, m_hTM);
	sysUnregisterMsgHandler(pmcPROFILE, m_hTM);
	sysUnregisterMsgHandler(pmc_UPDATER, m_hTM);
	if( m_nFilterMsgClass )
		sysUnregisterMsgHandler(m_nFilterMsgClass, m_hTM);

	if( m_hHTTP )
	{
		sysUnregisterMsgHandler(pmc_TASK_STATE_CHANGED, m_hTM);
		m_hTM->ReleaseService(TASKID_TM_ITSELF, m_hHTTP);
	}

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::ObjectPreClose: end"));

	return error;
}
// AVP Prague stamp end


// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
// Extended method comment
//    Receives message sent to the object or to the one of the object parents as broadcast
// Parameters are:
//   "msg_cls_id"    : Message class identifier
//   "msg_id"        : Message identifier
//   "send_point"    : Object where the message was send initially
//   "ctx"           : Context of the object processing
//   "receive_point" : Point of distributing message (tree top for RegisterMsgHandler call
//   "par_buf"       : Buffer of the parameters
//   "par_buf_len"   : Lenght of the buffer of the parameters
tERROR UrlFlt::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::MsgReceive method" );

	//PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::MsgReceive: Start"));

	if (
			p_msg_cls_id == pmc_TASK_STATE_CHANGED &&
			m_nTaskState == TASK_STATE_RUNNING &&
			m_hHTTP &&
			p_send_point->propGetPID() == m_hHTTP->propGetPID()
		)
	{
		m_nTaskState = (tTaskState)p_msg_id;
		sysSendMsg(pmc_TASK_STATE_CHANGED, m_nTaskState, NULL, NULL, NULL);
	}
	
	if( p_msg_cls_id == pmcTM_EVENTS && p_msg_id == pmTM_EVENT_IDLE && p_par_buf )
	{
		//PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::MsgReceive: Idle event: start"));

		IdleWork(*(tDWORD *)p_par_buf);

		//PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::MsgReceive: Idle event: end"));

		return errOK;
	}
	
	if( p_msg_cls_id == pmcPROFILE )
	{
		PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::MsgReceive: Profile event start"));

		if( p_msg_id == pmPROFILE_SETTINGS_MODIFIED && (p_par_buf_len == SER_SENDMSG_PSIZE) )
		{
			cProfileBase * pProfile = (cProfileBase *)p_par_buf;
			if( pProfile && pProfile->isBasedOn(cProfileBase::eIID) &&
				(pProfile->m_sName == AVP_PROFILE_PRODUCT) )
			{
				if( CheckProxyChanged() )
				{
					cAutoCS _lock(m_hCS, true);
					_ClearHeurCache();
					m_fHeuristicSuccess = 0;
				}
			}
		}

		PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::MsgReceive: Profile event end"));

		return errOK;
	}
	
	if( p_msg_cls_id == pmc_PROCESS_MONITOR )
	{
		//PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::MsgReceive: Process monitor start"));

		//PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::MsgReceive: Process monitor end"));

		// MBI !!!!!!!!!!!!!!!
		return errOK;
	}

	tDWORD nFilterMsgClass = 0;
	{
		cAutoCS _lock(m_hCS, false);
		nFilterMsgClass = m_nFilterMsgClass;
	}

	if( (p_msg_cls_id == nFilterMsgClass) && (p_msg_id == msg_URL_FILTER_CHECK_URL) )
	{
		if( m_nTaskState != TASK_STATE_RUNNING )
			return errOK;

		cUrlFilteringRequest * pUrlFlt = (cUrlFilteringRequest *)p_par_buf;
		if( p_par_buf_len != SER_SENDMSG_PSIZE || !pUrlFlt )
			return errPARAMETER_INVALID;

		PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::MsgReceive: URL FILTER CHECK start: [%S], %d",
			pUrlFlt->m_sURL.data(), ( int ) pUrlFlt->m_StatusCode ));
		const DWORD start = GetTickCount();

		error = ProcessObject(pUrlFlt, (hIO)p_ctx);
		
		// To allow for another component blocking URL
		if( error != errACCESS_DENIED )
			error = errOK;

		const DWORD end = GetTickCount();

		PR_TRACE((this, prtNOTIFY, 
			"urlflt\tUrlFlt::MsgReceive: URL FILTER CHECK end: time = %d for [%S]",
			( int ) end - start, pUrlFlt->m_sURL.data() ));

		return error;
	}

	if(p_msg_cls_id == pmc_UPDATER && p_msg_id == pm_COMPONENT_PREINSTALLED)
	{
		PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::MsgReceive: COMPONENT REINSTALLED start"));

		cSerializable *pSer = p_par_buf_len == SER_SENDMSG_PSIZE ? (cSerializable *)p_par_buf : NULL;
		cUpdaterUpdatedFileList *updatedFiles = pSer && pSer->getIID() == cUpdaterUpdatedFileList::eIID ?
			reinterpret_cast<cUpdaterUpdatedFileList *>(pSer) : NULL;
		if(!updatedFiles)
			return errPARAMETER_INVALID;

		cStrObj _sDatabaseUpdCat;
		{
			cAutoCS _lock(m_hCS, false);
			_sDatabaseUpdCat = m_sDatabaseUpdCat;
		}
		if(updatedFiles->m_selectedComponentIdentidier == _sDatabaseUpdCat)
		{
			//cAutoCS _lock( m_dbCS, true ); // protect local data, they can be changed by SetSettings

			PR_TRACE((this, prtIMPORTANT, "urlflt\tReloading databases..."));
			// возврат ошибки означает откат обновления!

			// Make list of updated files
			cStrObj strUpdatedDatabases = m_sDatabase;
			makeListOfUpdatedBases( updatedFiles, strUpdatedDatabases );

			PR_TRACE((this, prtIMPORTANT, "urlflt\tList of updated databases: [%S]",
				strUpdatedDatabases.data() ));

			// Expand environment variables
			cStrObj strExpUpdatedDatabases = strUpdatedDatabases;
			sysSendMsg
				(
					pmc_PRODUCT_ENVIRONMENT, 
					pm_EXPAND_ENVIRONMENT_STRING, 
					( hOBJECT ) cAutoString( strExpUpdatedDatabases ), 
					0, 
					0
				);

			// Update bases that are currently in use
			error = _ReloadDb( strExpUpdatedDatabases, false );
			if ( PR_FAIL( error ) )
				return error;

			// Update list of bases that are currently in use
			{
				cAutoCS _lock(m_hCS, true);
				m_sDatabase = strExpUpdatedDatabases;
			}

			// Clear cache of requested objects
			_ClearHeurCache();

			// Store the new list of updated files
			cUrlFltSettings sets;
			error = m_hTM->GetProfileInfo( AVP_PROFILE_PARENTALCONTROL, &sets );
			if ( PR_FAIL( error ) )
				return error;

			sets.m_sDataBase = strUpdatedDatabases;
			error = m_hTM->SetProfileInfo( AVP_PROFILE_PARENTALCONTROL, &sets, ( hOBJECT ) this, 0 );
			if ( PR_FAIL( error ) )
				return error;


			PR_TRACE((this, prtIMPORTANT, "urlflt\tDatabase reloaded (%terr)", error));
		}

		PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::MsgReceive: COMPONENT REINSTALLED end"));
	}

	//PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::MsgReceive: End"));

	return error;
}
// AVP Prague stamp end




// AVP Prague stamp begin( External (user called) interface method implementation, SetSettings )
// Parameters are:
tERROR UrlFlt::SetSettings( const cSerializable* p_settings )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetSettings method" );

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::SetSettings: start"));

	if( !p_settings || !p_settings->isBasedOn(cUrlFltSettings::eIID) )
	{
		if( p_settings )
			PR_TRACE((this, prtERROR, "urlflt\tSetSettings incompatible (%terr)", error ));
		return errPARAMETER_INVALID;
	}

	error = Reinit((cUrlFltSettings *)p_settings);
	if( PR_FAIL(error) )
		return error;
	
#ifdef _DEBUG
	// TEST !!!!!!!!!!!
/**/
	//CUrlFltTestThr::_new(this)->start(m_ThreadTest);
/**/
	// TEST !!!!!!!!!
	/*/
	{
		const char * l_teztTest = "fuck nacizm sex";

		CParctlDb::Result _dbRes;
		
		cAutoCS _lock(m_hCS, true);
		m_db.AnalyzeData((unsigned char *)l_teztTest, strlen(l_teztTest), _dbRes);
	}
	/**/
#endif
	
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::SetSettings: end"));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetSettings )
// Parameters are:
tERROR UrlFlt::GetSettings( cSerializable* p_settings )
{
	tERROR error = errNOT_SUPPORTED;	// get settings from BL
	PR_TRACE_FUNC_FRAME( "Task::GetSettings method" );

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AskAction )
// Parameters are:
tERROR UrlFlt::AskAction( tActionId p_actionId, cSerializable* p_actionInfo )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::AskAction method" );

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::AskAction: start"));

	if( p_actionId == pmcASK_ACTION && p_actionInfo && p_actionInfo->isBasedOn(cUrlFltAskAction::eIID) )
	{
		cUrlFltAskAction * pAskAction = (cUrlFltAskAction *)p_actionInfo;
		switch(pAskAction->m_nAction)
		{
		case cUrlFltAskAction::eSwitchProfile:
			return SwitchProfile(pAskAction->m_strUserId, pAskAction->m_nProfileId);
		}
	}

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::AskAction: end"));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetState )
// Parameters are:
tERROR UrlFlt::SetState( tTaskRequestState p_state )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::SetState method" );

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::SetState: start"));

	switch (p_state)
	{
	case TASK_REQUEST_RUN:
		m_nTaskState = TASK_STATE_RUNNING;
		break;
	case TASK_REQUEST_PAUSE:
		m_nTaskState = TASK_STATE_PAUSED;
		break;
	case TASK_REQUEST_STOP:
		_TrMgr_AbortJobs();
		m_nTaskState = TASK_STATE_STOPPED;
		break;
	default:
		return error = errPARAMETER_INVALID;
	}

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::SetState: end"));

	return error = warnTASK_STATE_CHANGED;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetStatistics )
// Parameters are:
tERROR UrlFlt::GetStatistics( cSerializable* p_statistics )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Task::GetStatistics method" );

	//PR_TRACE((this, prtNOT_IMPORTANT, "urlflt\tUrlFlt::GetStatistics: start"));

	if( !p_statistics || !p_statistics->isBasedOn(cUrlFltStatistics::eIID) )
	{
		if( p_statistics )
			PR_TRACE((this, prtERROR, "urlflt\tGetStatistics incompatible (%terr)", error ));
		return errPARAMETER_INVALID;
	}

	cUrlFltStatistics * pStat = (cUrlFltStatistics *)p_statistics;

	/*
	PR_TRACE((this, prtNOT_IMPORTANT, 
		"urlflt\tUrlFlt::GetStatistics: Get statistics for user [%S], profile = %d",
		pStat->m_sUserId.data(), ( int ) pStat->m_nCurUserProfileId ));
	*/

	cAutoCS _lock(m_hCS, false);

	tProfile * _profile = NULL;
	{
		tUser * _user = _GetUser(pStat->m_sUserId);
		if( _user )
			_profile = _GetProfile(_user->m_nProfileId);
		if( !_profile )
			_profile = _GetProfile(m_defUser.m_nProfileId);
	}
	
	pStat->m_nCurUserProfileId = _profile ? _profile->m_nId : -1;
	pStat->m_Data = m_common.m_stat;
	pStat->m_aProfiles.resize(m_profiles.size());
	if( pStat->m_aProfiles.size() != m_profiles.size() )
		return errNOT_ENOUGH_MEMORY;
	
	for(tDWORD i = 0; i < pStat->m_aProfiles.size(); i++)
	{
		cUrlFltProfileStat& _v = pStat->m_aProfiles[i];
		tProfile& _p = *m_profiles[i];

		_v.m_nProfileId = _p.m_nId;
		_v.m_strProfile = _p.m_strName;
		_v.m_nLevel = _p.m_nLevel;
		_v.m_Data = _p.m_stat;
	}

	//PR_TRACE((this, prtNOT_IMPORTANT, "urlflt\tUrlFlt::GetStatistics: end"));

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Task". Register function
tERROR UrlFlt::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Task_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Task_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "", 1 )
	mpLOCAL_PROPERTY_BUF( pgTASK_TM, UrlFlt, m_hTM, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_STATE, UrlFlt, m_nTaskState, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_BUF( pgTASK_SESSION_ID, UrlFlt, m_nTaskSessionId, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_ID, UrlFlt, m_nTaskId, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_PARENT_ID, UrlFlt, m_nTaskParentId, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgTASK_PERSISTENT_STORAGE, UrlFlt, m_hPersData, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
mpPROPERTY_TABLE_END(Task_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Task)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(Task)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(Task)
	mEXTERNAL_METHOD(Task, SetSettings)
	mEXTERNAL_METHOD(Task, GetSettings)
	mEXTERNAL_METHOD(Task, AskAction)
	mEXTERNAL_METHOD(Task, SetState)
	mEXTERNAL_METHOD(Task, GetStatistics)
mEXTERNAL_TABLE_END(Task)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "Task::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_TASK,                               // interface identifier
		PID_URLFLT,                             // plugin identifier
		0x00000000,                             // subtype identifier
		Task_VERSION,                           // interface version
		VID_MIKE,                               // interface developer
		&i_Task_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Task_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Task_vtbl,                           // external function table
		(sizeof(e_Task_vtbl)/sizeof(tPTR)),     // external function table size
		Task_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Task_PropTable),   // property table size
		sizeof(UrlFlt)-sizeof(cObjImpl),        // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Task(IID_TASK) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call Task_Register( hROOT root ) { return UrlFlt::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgTASK_PERSISTENT_STORAGE
// AVP Prague stamp end



// #################################################################################
// #################################################################################

tERROR UrlFlt::Reinit(cUrlFltSettings * p_settings)
{
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::Reinit: start"));

	tERROR err = errOK;
	tDWORD i, n;
	
	if( p_settings->m_bLoadHTTP && !m_hHTTP )
	{
		err = m_hTM->GetService(TASKID_TM_ITSELF, *this, AVP_SERVICE_TRAFFICMONITOR_HTTP, &m_hHTTP, cREQUEST_SYNCHRONOUS);
		if( PR_SUCC(err) && m_hHTTP )
			sysRegisterMsgHandler(pmc_TASK_STATE_CHANGED, rmhLISTENER, m_hTM, IID_ANY, m_hHTTP->propGetPID(), IID_ANY, PID_ANY);
	}

	if( !m_nFilterMsgClass && p_settings->m_nFilterMsgClass )
		sysRegisterMsgHandler(p_settings->m_nFilterMsgClass, rmhDECIDER, m_hTM, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	if( p_settings->m_sDatabaseUpdCat.length() != 0 )
		sysRegisterMsgHandler(pmc_UPDATER, rmhDECIDER, m_hTM, IID_ANY, PID_ANY, IID_ANY, PID_ANY);

	cStrObj _sDatabaseNew = p_settings->m_sDataBase;
	sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(_sDatabaseNew), 0, 0);
	
	cAutoCS _lock(m_hCS, true);

	bool bReloadDb = false;
	if( m_sDatabase != _sDatabaseNew )
	{
		m_sDatabase = _sDatabaseNew;
		bReloadDb = true;
	}
	
	m_sDatabaseUpdCat = p_settings->m_sDatabaseUpdCat;
	if( !m_nFilterMsgClass )
		m_nFilterMsgClass = p_settings->m_nFilterMsgClass;
	
	m_nCache = p_settings->m_nCache;
	m_nCacheTTL = p_settings->m_nCacheTTL;
	m_nCacheErrTTL = p_settings->m_nCacheErrTTL;
	m_nMaxHeuristicDataSize = p_settings->m_nMaxHeuristicDataSize;
	m_nTrMax = p_settings->m_nTranspMax;
	m_nTranspNetTimeout = p_settings->m_nTranspNetTimeout;
	m_fHeuristic = p_settings->m_bHeuristic;
	m_fHeuristicOnly = p_settings->m_bHeuristicOnly;
	m_fHeuristicContentDescr = p_settings->m_bHeuristicContentDescr;
	m_fHeuristicContentUrls = p_settings->m_bHeuristicContentUrls;
	m_fProfiles = p_settings->m_bProfiles;
	m_fAskAction = p_settings->m_bAskAction;
	m_AskActionDef = p_settings->m_AskActionDef;
	m_fCheckMainPage = p_settings->m_bCheckMainPage;
	m_fUseReportFile = p_settings->m_bUseReportFile;
	
	// Set users
	{
		cVector<bool> _save; _save.resize(m_users.size());
		if( _save.size() != m_users.size() )
			return errNOT_ENOUGH_MEMORY;

		m_defUser.m_nProfileId = -1;
		for(i = 0, n = p_settings->m_users.size(); i < n; i++)
		{
			cTaskSettingsPerUserItem& _u = p_settings->m_users[i];
			cUrlFltUserSettings * pSett = (cUrlFltUserSettings *)_u.m_settings.ptr_ref();
			if( !pSett || !pSett->isBasedOn(cUrlFltUserSettings::eIID) )
			{
				PR_TRACE((this, prtERROR, "urlflt\tInvalid user item in settings!"));
				continue;
			}

			if( _u.isdefault() )
			{
				m_defUser.m_nProfileId = pSett->m_nProfileId;
				continue;
			}

			tDWORD nPos;
			tUser * _user = _GetUser(_u.m_sUserId, true, &nPos);
			if( !_user )
				return errNOT_ENOUGH_MEMORY;

			if( nPos < _save.size() )
				_save[nPos] = true;

			if( !_user->m_bFixProfile )
				_user->m_nProfileId = pSett->m_nProfileId;
		}

		for(i = _save.size(); i > 0; i--)
			if( !_save[i - 1] )
				m_users.remove(i - 1);
	}
	
	// Set profiles
	{
		cVector<bool> _save; _save.resize(m_profiles.size());
		if( _save.size() != m_profiles.size() )
			return errNOT_ENOUGH_MEMORY;

		for(i = 0, n = p_settings->m_aProfiles.size(); i < n; i++)
		{
			cUrlFltProfile& _p = p_settings->m_aProfiles[i];
			if( !_p.m_bEnabled )
				continue;

			tDWORD nPos;
			tProfile * _profile = _GetProfile(_p.m_nId, true, &nPos);
			if( !_profile )
				return errNOT_ENOUGH_MEMORY;

			if( nPos < _save.size() )
				_save[nPos] = true;

			_profile->m_strName = _p.m_sName;
			_profile->m_nLevel = _p.m_nLevel;
			_profile->m_bUseCommonFilter = _p.m_bUseCommonFilter;
			_profile->m_bDenyDetected = _p.m_bDenyBadURL;
			
			_profile->m_bUseDayTimeSpace = _p.m_bUseDayTimeSpace;
			_profile->m_nDayTimeSpace.resize(0);
			if( _p.m_bUseDayTimeSpace )
			{
				for(tDWORD i = 0; i < _p.m_aDayTimeSpace.size(); i++)
				{
					cUrlFltTimeRange& _r = _p.m_aDayTimeSpace[i];
					if( _r.m_bEnabled )
						_profile->m_nDayTimeSpace.push_back(_r);
				}
			}
			/*
			else
				_profile->m_nDayTimeSpace.push_back(cUrlFltTimeRange(0, 86399));
			*/
			
			_profile->m_bUseDayTimeLimit = _p.m_nDayTimeLimit.on();
			tDWORD nDayTimeLimit = _p.m_nDayTimeLimit.on() ? (tDWORD)_p.m_nDayTimeLimit.val() : 0xFFFFFFFF;
			if( _profile->m_nDayTimeLimit != nDayTimeLimit )
				_profile->m_nDayTime = 0;
			_profile->m_nDayTimeLimit = nDayTimeLimit;

			err = _profile->Build( *this, _p.m_Filter);
			if( PR_FAIL(err) )
			{
				PR_TRACE((this, prtERROR, "urlflt\tBuild profile filter error: %terr!", err));
				return err;
			}
		}

		for(i = _save.size(); i > 0; i--)
		{
			if( _save[i - 1] )
				continue;

			m_profiles[i - 1]->release();
			m_profiles.remove(i - 1);
		}
	}

	err = m_common.Build( *this, p_settings->m_Filter);
	if( PR_FAIL(err) )
	{
		PR_TRACE((this, prtERROR, "urlflt\tBuild common filter error: %terr!", err));
		return err;
	}

	if( bReloadDb )
	{
		//cAutoCS lock( m_dbCS, true );
		err = _ReloadDb(m_sDatabase, true);
	}

	// Clear cache of requested objects
	_ClearHeurCache();

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::Reinit: end"));

	return err;
}

//////////////////////////////////////////////////////////////////////////
///
/// Site data for analyzing
///
class _SiteData
{
public:

	///
	/// Meta types
	///
	enum MetaType
	{
		Keywords = 1,
		Description,
		Classification
	};

	struct STR
	{
		STR(): 
			m_p( NULL ), 
			m_n( 0 )
		{
		}

		tCHAR *	m_p;
		tDWORD	m_n;
	};

	struct LINK
	{
		CUrlListCache::tURL	m_url;
		std::wstring		m_name;
	};

public:

	///
	/// Constructor
	///
	_SiteData( hOBJECT tracer );

	///
	/// Read data from the given IO to the internal buffer
	///
	tERROR Set( hIO data );

	///
	/// Check if the internal buffer is empty
	///
	bool	IsEmpty();

	void Prepare();

	/*
	///
	/// Convert the given string to the appropriate charset
	///
	/// @param str - null-terminated string for processing
	///
	void _ProcessCharset( tCHAR* strData );

	void	_ProcessMetaContentWords(tCHAR * strHtml, tCHAR * strHtmlEnd, tCHAR * strTag);
	*/

	///
	/// Parse the given string and add the resulted words to the descriptions
	///
	/// @param str - string for parsing
	/// @param words - output vector with words
	///
	void addToDescription( const std::wstring& str, std::vector< std::wstring >& words );

	///
	/// Extract string starting from the given start position and up to the given end position
	///
	/// @param startPos - start position for extracting
	/// @param endPos - end position for extracting
	/// @param out_str - result string
	///
	void extractString( const tCHAR* startPos, const tCHAR* endPos, std::wstring& out_str );

	///
	/// Check if the given URL is a relative one
	///
	void checkForRelativeUrl( const cStrObj& urlPrefix, cStrObj& url );

	void ProcessDescriptionWords();

	tERROR ProcessURLs( const cStrObj& strSiteURL );

public:

	///
	/// Extract words from the description meta tag
	///
	/// @param words - output vector with words
	///
	void processDescriptionTag( std::vector< std::wstring >& words );

	///
	/// Extract words from the keywords meta tag
	///
	/// @param words - output vector with words
	///
	void processKeywordsTag( std::vector< std::wstring >& words );

	///
	/// Extract words from the classification meta tag
	///
	/// @param words - output vector with words
	///
	void processClassificationTag( std::vector< std::wstring >& words );

	///
	/// Extract words from the title tag
	///
	/// @param words - output vector with words
	///
	void processTitleTag( std::vector< std::wstring >& words );

public:

	///
	/// Encode the content if necessary
	///
	/// @param contentEncoding - content encoding
	/// @param content - the given content for encoding
	///
	tERROR checkForEncodedContent
		( 
			const hOBJECT tracer,
			const http::content_encoding contentEncodig
		);

	///
	/// Check the given content for being a binary stream (PNG, GIF, etc)
	///
	/// @param contentType - content type
	/// @param content - the given content for encoding
	///
	bool checkForBinaryContent( const http::content_type contentType );

public:

	static tDWORD _compact_spaces(tCHAR * str, tDWORD nLen, tDWORD * pWordsCount = NULL );

private:

	///
	/// Scan the given null-terminated string for the required meta tag
	///
	/// @param str - string for processing
	/// @param type - meta tag's type
	/// @param words - output vector with words from the given tag
	///
	void _processMetaTag( const tCHAR* str, MetaType type, std::vector< std::wstring >& words );

	///
	/// Scan the given null-terminated string for the title tag
	///
	/// @param str - string for processing
	/// @param words - output vector with words from the given tag
	///
	void _processTitleTag( const tCHAR* str, std::vector< std::wstring >& words );

	///
	/// Scan the given null-terminated string for the meta charset tag
	///
	/// @param str - string for processing
	///
	void _processMetaCharset( const tCHAR* str );

public:	// iIO

	hOBJECT 							m_tracer;

	/*
	unsigned							m_interuppted : 1;
	tDWORD								m_curPos;
	*/

	///
	/// Null-terminated data buffer (you can use this buffer freely for working 
	/// with functions expecting null-terminating strings).
	/// The true size of data is always m_buf.size() - 1. Or use m_bufSize 
	/// instead.
	///
	std::vector< tBYTE >				m_buf;

	///
	/// True buffer size (not taking into account null-terminating character)
	///
	std::vector< tBYTE >::size_type		m_bufSize;

	/*
	std::string							m_strHdr;
	*/

	CHARSET								m_chs;

	///
	/// Meta-words
	///
	//std::wstring						m_strDescriptionWords;
	//std::vector< std::wstring >			m_descriptionWords;

	cMemChunkAligned<0x400>				m_URLsBuf;
	std::vector<LINK>					m_aLinks;

};

///
/// Constructor
///
inline
_SiteData::_SiteData( hOBJECT tracer ): 
	m_tracer( tracer ),
	//m_curPos( 0 ), 
	//m_interuppted( 0 ),
	m_bufSize( 0 )
{
}

///
/// Read data from the given IO
///
tERROR _SiteData::Set( hIO data )
{
	tERROR err = errOK;

	tQWORD qwSize = 0;
	err = data->GetSize( &qwSize, 0 );

	if( PR_SUCC( err ) && qwSize != 0 )
	{
		m_bufSize = static_cast< tDataVector::size_type >( qwSize );
		m_buf.resize( m_bufSize + 1 ); // +1: Add null-terminated character

		if ( m_buf.size() == ( m_bufSize + 1 ) )
		{
			err = data->SeekRead
				( 
					NULL, 
					0, 
					&m_buf.front(), 
					static_cast< tDWORD >( m_bufSize ) 
				);
		}
		else
		{
			err = errFOUND;
		}
	}

	if ( PR_FAIL( err ) || ( qwSize == 0 ) )
	{
		m_bufSize = 0;
		m_buf.clear();
		m_buf.push_back( tBYTE() ); // Add null-terminated character
	}

	return err;
}

///
/// Check if the internal buffer is empty
///
inline
bool _SiteData::IsEmpty() 
{ 
	PR_ASSERT( m_buf.size() == m_bufSize + 1 );
	return ( m_bufSize == 0 ); 
}

void _SiteData::Prepare()
{
	PR_ASSERT( m_buf.size() == m_bufSize + 1 );
	if ( m_bufSize == 0 )
		return;

	m_chs.nType = CHARSET::eChsWindows;
	m_chs.nSubType = 0;
	m_chs.nCodepage = CP_ACP;

	/*
	if( m_strHdr.size() )
	{
		_ProcessCharset(&m_strHdr[0]);
	}
	*/

	if ( m_chs.nType == CHARSET::eChsWindows && m_chs.nSubType == 0 && m_chs.nCodepage == CP_ACP )
	{
		// Note: we can use buffer as a null-terminated string
		// as '\0' was added explicitly when the buffer was filled out
		//_ProcessCharset( ( tCHAR* ) &m_buf[ 0 ] );
		_processMetaCharset( ( tCHAR* ) &m_buf[ 0 ] );
	}
}

/*
///
/// Convert the given string to the appropriate charset
///
/// @param str - null-terminated string for processing
///
void _SiteData::_ProcessCharset( tCHAR* strData )
{
	tCHAR* str = NULL;
	if ( str = stristr( strData, "charset=" ) )
	{
		CHECK_PTR( ( tCHAR* ) &m_buf[ 0 ], ( int ) m_buf.size(), str );
		tCHAR strCharset[50]; 
		CHECK_PTR_RANGE( ( tCHAR* ) &m_buf[ 0 ], ( int ) m_buf.size(), str + sizeof("charset=") - 1, countof(strCharset) - 1 );
		strncpy(strCharset, str + sizeof("charset=") - 1, countof(strCharset) - 1);
		CHECK_PTR( strCharset, 50, strCharset + countof(strCharset) - 1 );
		strCharset[countof(strCharset) - 1] = 0;
		if( str = strstr(strCharset, "\"") )
		{
			CHECK_PTR( ( tCHAR* ) &m_buf[ 0 ], ( int ) m_buf.size(), str );
			*str = 0;
		}

		_ChsetDec_Parse(strCharset, m_chs);
	}
}
*/

/*
void	_ProcessMetaContentWords(tCHAR * strHtml, tCHAR * strHtmlEnd, tCHAR * strTag)
{
	tCHAR * str = stristr(strHtml, strTag);
	if( !str )
		return;

	{
		tCHAR * strTmp = str; tCHAR cTmp = *strTmp; *strTmp = 0;
		str = strrchr(strHtml, '<');
		*strTmp = cTmp;
	}

	if( str )
	{
		tCHAR * strTagBegin = str + 1;
		str = strrchr(strTagBegin, '>');
		if( !str )
			str = strHtmlEnd;

		tCHAR * strTmp = str; tCHAR cTmp = *strTmp; *strTmp = 0;

		if( str = stristr(strTagBegin, "content") )
		{
			if( str = strchr(str, '\"') )
			{
				tCHAR * strKeywordEnd = strstr(str + 1, "\"");
				if( !strKeywordEnd )
					strKeywordEnd = strHtmlEnd;

				STR _strKeywords;
				_strKeywords.m_p = str + 1;
				_strKeywords.m_n = strKeywordEnd - _strKeywords.m_p;

				// Compact and convert to WCHAR(UTF16)
				std::wstring strContent;
				_strKeywords.m_n = _compact_spaces(_strKeywords.m_p, _strKeywords.m_n);
				_ChsetDec_Str2Utf16(strContent, m_chs, _strKeywords.m_p, _strKeywords.m_n);

				if( strContent.size() )
				{
					if( m_strDescriptionWords.size() )
						m_strDescriptionWords.append(1, ' ');
					m_strDescriptionWords += strContent;
				}
			}
		}

		*strTmp = cTmp;
	}
}
*/

///
/// Parse the given string and add the resulted words to the descriptions
///
/// @param str - string for parsing
/// @param words - output vector with words
///
void _SiteData::addToDescription( const std::wstring& str, std::vector< std::wstring >& words )
{
	/*
	if( !m_strDescriptionWords.empty() )
	{
	m_strDescriptionWords.append( 1, L' ' );
	}

	m_strDescriptionWords += str;
	*/

	// Parse words in the given string
	std::wstring word;
	std::wstring::size_type pos = str.find_first_not_of( L' ', 0 );
	while ( pos != std::wstring::npos )
	{
		const std::wstring::size_type new_pos = str.find_first_of( L' ', pos );

		if ( new_pos != std::wstring::npos )
		{
			word = str.substr( pos, new_pos - pos );
		}
		else
		{
			word = str.substr( pos );
		}

		// The word should be long enough
		if ( word.length() > 2 )
		{
			_wcslwr( const_cast< wchar_t* >( word.c_str() ) );
			std::wstring sep_word( 1, L' ' );
			sep_word += word;
			sep_word += L' ';
			//m_descriptionWords.push_back( sep_word );
			words.push_back( sep_word );
		}

		pos = str.find_first_not_of( L' ', new_pos );
	}
}

///
/// Extract string starting from the given start position and up to the given end position
///
/// @param startPos - start position for extracting
/// @param endPos - end position for extracting
/// @param out_str - result string
///
void _SiteData::extractString( const tCHAR* startPos, const tCHAR* endPos, std::wstring& out_str )
{
	PR_ASSERT( startPos != NULL );
	PR_ASSERT( endPos != NULL );
	PR_ASSERT( startPos < endPos );

	if ( startPos == NULL || endPos == NULL )
		return;

	STR str;
	str.m_p = const_cast< tCHAR* >( startPos );
	str.m_n = endPos - startPos;

	// Compact and convert to WCHAR(UTF16)
	out_str.clear();

	str.m_n = _compact_spaces( str.m_p, str.m_n );
	_ChsetDec_Str2Utf16( out_str, m_chs, str.m_p, str.m_n );
}

///
/// Scan the given null-terminated string for the required meta tag
///
/// @param startHtml - start position for scanning
/// @param endHtml - end position for processing
/// @param type - meta tag's type
/// @param words - output vector with words from the given tag
///
void _SiteData::_processMetaTag( const tCHAR* str, MetaType type, std::vector< std::wstring >& words )
{
	PR_ASSERT( str != NULL );
	if ( str == NULL )
		return;

	const tCHAR* reg_expr = NULL;
	switch ( type )
	{
	case Keywords:
		//                                    1                                       2    3
		reg_expr = "<meta\\s[^>]*name\\s*=\\s*(\"?)keywords\\1\\s[^>]*content\\s*=\\s*(\"?)([^>]+?)\\2[^>]*>";
		break;

	case Description:
		//                                    1                                          2    3
		reg_expr = "<meta\\s[^>]*name\\s*=\\s*(\"?)description\\1\\s[^>]*content\\s*=\\s*(\"?)([^>]+?)\\2[^>]*>";
		break;

	case Classification:
		//                                    1                                             2    3
		reg_expr = "<meta\\s[^>]*name\\s*=\\s*(\"?)classification\\1\\s[^>]*content\\s*=\\s*(\"?)([^>]+?)\\2[^>]*>";
		break;
	}

	PR_ASSERT( reg_expr != NULL );
	if ( reg_expr == NULL )
		return;

	boost::regex regex( reg_expr, boost::regex::normal | boost::regex::icase );
	boost::cmatch match_res;

	CHECK_PTR( ( tCHAR* ) &m_buf[ 0 ], ( int ) m_buf.size(), str );
	if ( boost::regex_search( str, match_res, regex ) )
	{
		PR_ASSERT( match_res.size() == 1 + 3 );

		if ( match_res[ 3 ].matched )
		{
			const tCHAR* startPos = match_res[ 3 ].first;
			const tCHAR* endPos = match_res[ 3 ].second;
			CHECK_PTR( ( tCHAR* ) &m_buf[ 0 ], ( int ) m_buf.size(), startPos );
			CHECK_PTR( ( tCHAR* ) &m_buf[ 0 ], ( int ) m_buf.size(), endPos );
			PR_ASSERT( endPos > startPos );

			std::wstring content;
			extractString( startPos, endPos, content );

			/*
			PR_TRACE((m_this, prtNOTIFY, 
					"urlflt\t_SiteData::_processMetaTag: [%S]", 
					content.c_str() ));
			*/

			if( !content.empty() )
			{
				addToDescription( content, words );
			}
			else
			{
				PR_ASSERT( false );
			}
		}
	}
}

///
/// Scan the given null-terminated string for the title tag
///
/// @param str - string for scanning
/// @param words - output vector with words from the given tag
///
void _SiteData::_processTitleTag( const tCHAR* str, std::vector< std::wstring >& words )
{
	PR_ASSERT( str != NULL );

	if ( str == NULL )
		return;

	//                          1
	boost::regex regex( "<title>(.+?)</title>", boost::regex::normal | boost::regex::icase );
	boost::cmatch match_res;

	CHECK_PTR( ( tCHAR* ) &m_buf[ 0 ], ( int ) m_buf.size(), str );
	if ( boost::regex_search( str, match_res, regex ) )
	{
		PR_ASSERT( match_res.size() == 1 + 1 );

		if ( match_res[ 1 ].matched )
		{
			const tCHAR* startPos = match_res[ 1 ].first;
			const tCHAR* endPos = match_res[ 1 ].second;
			CHECK_PTR( ( tCHAR* ) &m_buf[ 0 ], ( int ) m_buf.size(), startPos );
			CHECK_PTR( ( tCHAR* ) &m_buf[ 0 ], ( int ) m_buf.size(), endPos );
			PR_ASSERT( endPos > startPos );

			std::wstring content;
			extractString( startPos, endPos, content );

			/*
			PR_TRACE((m_this, prtNOTIFY, 
				"urlflt\t_SiteData::_processTitleTag: [%S]", 
				content.c_str() ));
			*/

			if( !content.empty() )
			{
				addToDescription( content, words );
			}
			else
			{
				PR_ASSERT( false );
			}
		}
	}
}

///
/// Scan the given null-terminated string for the meta charset tag
///
/// @param str - string for processing
///
void _SiteData::_processMetaCharset( const tCHAR* str )
{
	PR_ASSERT( str != NULL );
	if ( str == NULL )
		return;

	boost::regex regex( "<meta\\s[^>]*charset\\s*=\\s*([^\\s;\">]+)[^>]*>", boost::regex::normal | boost::regex::icase );
	boost::cmatch match_res;

	CHECK_PTR( ( tCHAR* ) &m_buf[ 0 ], ( int ) m_buf.size(), str );
	if ( boost::regex_search( str, match_res, regex ) )
	{
		PR_ASSERT( match_res.size() == 1 + 1 );

		if ( match_res[ 1 ].matched )
		{
			const tCHAR* startPos = match_res[ 1 ].first;
			const tCHAR* endPos = match_res[ 1 ].second;
			CHECK_PTR( ( tCHAR* ) &m_buf[ 0 ], ( int ) m_buf.size(), startPos );
			CHECK_PTR( ( tCHAR* ) &m_buf[ 0 ], ( int ) m_buf.size(), endPos );
			PR_ASSERT( endPos > startPos );

			std::string content( startPos, endPos - startPos );

			/*
			PR_TRACE((m_this, prtNOTIFY, 
				"urlflt\t_SiteData::_processMetaCharset: [%s]", 
				content.c_str() ));
			*/

			if( !content.empty() )
			{
				_ChsetDec_Parse( content.c_str(), m_chs );
			}
			else
			{
				PR_ASSERT( false );
			}
		}
	}
}


///
/// Check if the given URL is a relative one
///
void _SiteData::checkForRelativeUrl( const cStrObj& urlPrefix, cStrObj& url )
{
	bool relativeUrl = false;

	if ( !url.empty() )
	{
		switch ( url[ 0 ] )
		{
		case '/':
			relativeUrl = true;
			break;
		}
	}

	if ( relativeUrl )
	{
		// Convert relative URL to the absolute one
		cStrObj t = urlPrefix;
		t += url;
		url = t;
	}
}

/*
void _SiteData::ProcessDescriptionWords()
{
	PR_ASSERT( m_buf.size() == m_bufSize + 1 );
	if ( m_bufSize == 0 )
		return;

	// Find keywords meta
	/*
	_ProcessMetaContentWords(strHtml, strHtmlEnd, "keywords");
	_ProcessMetaContentWords(strHtml, strHtmlEnd, "description");
	_ProcessMetaContentWords(strHtml, strHtmlEnd, "classification");
	*

	// Note: we can use buffer as a null-terminated string
	// as '\0' was added explicitly when the buffer was filled out
	_processMetaTag( ( tCHAR* ) &m_buf[ 0 ], Keywords, m_descriptionWords );
	_processMetaTag( ( tCHAR* ) &m_buf[ 0 ], Description, m_descriptionWords );
	_processMetaTag( ( tCHAR* ) &m_buf[ 0 ], Classification, m_descriptionWords );

	/*
	// Find title
	if( str = stristr(strHtml, "title") )
	{
	tCHAR * strTmp = strchr(str + sizeof("title") - 1, '>');
	if( strTmp )
	{
	STR _strTitle;
	_strTitle.m_p = strTmp + 1;

	strTmp = strchr(_strTitle.m_p, '<');
	if( strTmp )
	{
	_strTitle.m_n = strTmp - _strTitle.m_p;

	std::wstring strTitle;
	// Compact and convert to WCHAR(UTF16)
	_strTitle.m_n = _compact_spaces(_strTitle.m_p, _strTitle.m_n);
	_ChsetDec_Str2Utf16(strTitle, m_chs, _strTitle.m_p, _strTitle.m_n);

	if( m_strDescriptionWords.size() )
	m_strDescriptionWords.append(1, ' ');
	m_strDescriptionWords += strTitle;
	}
	}
	}
	*
	// Note: we can use buffer as a null-terminated string
	// as '\0' was added explicitly when the buffer was filled out
	_processTitleTag( ( tCHAR* ) &m_buf[ 0 ], m_descriptionWords );

	/*
	// Finalize
	if( m_strDescriptionWords.size() )
	{
	m_strDescriptionWords.insert(0, 1, L' ' );
	m_strDescriptionWords.append(1, L' ' );
	_wcslwr(&m_strDescriptionWords[0]);
	}
	*
}
*/

tERROR _SiteData::ProcessURLs( const cStrObj& strSiteURL )
{
	PR_ASSERT( m_buf.size() == m_bufSize + 1 );
	if ( m_bufSize == 0 )
		return errOK;

	tDWORD nBufPos = 0;

	// Note: we can use buffer as a null-terminated string
	// as '\0' was added explicitly when the buffer was filled out
	const tCHAR* str = ( tCHAR* ) &m_buf[ 0 ];

	// build regular expression to search content for anchor (A) tags and parse them
	//boost::regex regex("(<!--.*?-->)|((<[aA](\\s+.+?)??\\s+href\\s*=\\s*((\"(.*?)\")|(\'(.*?)\')|([^\\s]+?))(\\s+.+?)??\\s*>)(.*?)(</[aA]>))");
	//boost::cmatch match_res;
	/*
		Examples:
		1) <a href="test">test</a>, <a href='test'>test</a>, <a href=test>test</a>
		2) <a href="test">test<\/a>
	*/
	boost::regex regex
		( 
			//1                      2        3					     4
			"(<a\\s[^>]*href\\s*=\\s*([\"\']?)([^>\"\'\\s]+)\\2[^>]*>(.*?)<\\\\?/a>)", 
			boost::regex::normal | boost::regex::icase 
		);
	boost::cmatch match_res;
	enum
	{
		REG_EXP_HREF_INDEX = 3,
		REG_EXP_CONTENT_INDEX = 4,
		REG_EXP_EXPR_COUNT = 4
	};

	const tCHAR* p = str;
	CHECK_PTR( ( tCHAR* ) &m_buf[ 0 ], ( int ) m_buf.size(), p );

	while( boost::regex_search( p, match_res, regex ) )
	{
		CHECK_PTR( ( tCHAR* ) &m_buf[ 0 ], ( int ) m_buf.size(), p );
		PR_ASSERT( match_res.size() == 1 + REG_EXP_EXPR_COUNT );  // submatches number is hardly related to the expression being processed

		p = match_res[ 0 ].second;

		/*
		if( match_res[1].matched )    // skip comments
		continue;
		*/

		const tCHAR* strUrl = NULL;
		const tCHAR* strUrlEnd = NULL;

		// extract hyperlink from match results
		if ( match_res[ REG_EXP_HREF_INDEX ].matched )
		{
			strUrl = match_res[ REG_EXP_HREF_INDEX ].first;
			strUrlEnd = match_res[ REG_EXP_HREF_INDEX ].second;

		}

		// extract hyperlink content
		STR strLinkText;
		if ( match_res[ REG_EXP_CONTENT_INDEX ].matched )
		{
			strLinkText.m_p = const_cast< tCHAR* >( match_res[ REG_EXP_CONTENT_INDEX ].first );
			strLinkText.m_n = match_res[ REG_EXP_CONTENT_INDEX ].second - match_res[ REG_EXP_CONTENT_INDEX ].first;

			CHECK_PTR_RANGE( ( tCHAR* ) &m_buf[ 0 ], ( int ) m_buf.size(), strLinkText.m_p, strLinkText.m_n );
		}

		LINK& _link = *m_aLinks.insert(m_aLinks.end(), LINK());

		PR_ASSERT(strUrl != NULL && strUrlEnd != NULL);
		PR_ASSERT( strUrlEnd > strUrl );
		CHECK_PTR( ( tCHAR* ) &m_buf[ 0 ], ( int ) m_buf.size(), strUrl );
		CHECK_PTR( ( tCHAR* ) &m_buf[ 0 ], ( int ) m_buf.size(), strUrlEnd );
		cStrObj _urlU; _urlU.assign(strUrl, cCP_ANSI, strUrlEnd - strUrl);

		/*
		PR_TRACE((m_this, prtNOTIFY, 
			"urlflt\t_SiteData::ProcessURLs: link: [%S]", 
			_urlU.data() ));
		*/

		// Check for relative path
		checkForRelativeUrl( strSiteURL, _urlU );

		if( !m_URLsBuf.reserve(nBufPos + _link.m_url.BufSize(_urlU), true) )
			return errNOT_ENOUGH_MEMORY;

		tCHAR* bufPtr = ( tCHAR* ) m_URLsBuf.ptr() + nBufPos;
		const tUINT bufSize = m_URLsBuf.size() - nBufPos;
		tDWORD nSize = _link.m_url.Normalize
			(
				_urlU, 
				bufPtr,
				bufSize,
				CUrlListCache::tURL::fSimplify
			);
		if( !_link.m_url.m_hostLen )
		{
			m_aLinks.erase(m_aLinks.begin() + m_aLinks.size() - 1);
			continue;
		}

		nBufPos += nSize;
		_link.m_url.Reoffset(-(tLONG)m_URLsBuf.ptr());

		CHECK_PTR_RANGE( ( tCHAR* ) &m_buf[ 0 ], ( int ) m_buf.size(), strLinkText.m_p, strLinkText.m_n );
		strLinkText.m_n = _compact_spaces(strLinkText.m_p, strLinkText.m_n);
		if( strLinkText.m_n )
		{
			_ChsetDec_Str2Utf16(_link.m_name, m_chs, strLinkText.m_p, strLinkText.m_n);

			/*
			PR_TRACE((m_this, prtNOTIFY, 
				"urlflt\t_SiteData::ProcessURLs: text: [%S]", 
				_link.m_name.c_str() ));
			*/

			_link.m_name.insert(0, 1, ' ');
			_link.m_name.append(1, ' ');
			_wcslwr(&_link.m_name[0]);
		}
	}

	tCHAR * pBuf = (tCHAR *)m_URLsBuf.ptr();
	for(tDWORD i = 0; i < m_aLinks.size(); i++)
	{
		m_aLinks[i].m_url.Reoffset((tLONG)pBuf);
		CHECK_URL( ( tCHAR* ) m_URLsBuf.ptr(), m_URLsBuf.size(), m_aLinks[ i ].m_url );

		/*
		PR_TRACE((m_this, prtNOTIFY, 
			"urlflt\t_SiteData::ProcessURLs: after reoffset: link: [%s]", 
			m_aLinks[i].m_url.m_url ));
		*/
	}

	return errOK;
}

///
/// Extract words from the description meta tag
///
/// @param words - output vector with words
///
void _SiteData::processDescriptionTag( std::vector< std::wstring >& words )
{
	PR_ASSERT( m_buf.size() == m_bufSize + 1 );
	if ( m_bufSize == 0 )
		return;

	// Note: we can use buffer as a null-terminated string
	// as '\0' was added explicitly when the buffer was filled out
	_processMetaTag( ( tCHAR* ) &m_buf[ 0 ], Description, words );
}

///
/// Extract words from the keywords meta tag
///
/// @param words - output vector with words
///
void _SiteData::processKeywordsTag( std::vector< std::wstring >& words )
{
	PR_ASSERT( m_buf.size() == m_bufSize + 1 );
	if ( m_bufSize == 0 )
		return;

	// Note: we can use buffer as a null-terminated string
	// as '\0' was added explicitly when the buffer was filled out
	_processMetaTag( ( tCHAR* ) &m_buf[ 0 ], Keywords, words );
}

///
/// Extract words from the classification meta tag
///
/// @param words - output vector with words
///
void _SiteData::processClassificationTag( std::vector< std::wstring >& words )
{
	PR_ASSERT( m_buf.size() == m_bufSize + 1 );
	if ( m_bufSize == 0 )
		return;

	// Note: we can use buffer as a null-terminated string
	// as '\0' was added explicitly when the buffer was filled out
	_processMetaTag( ( tCHAR* ) &m_buf[ 0 ], Classification, words  );
}

///
/// Extract words from the title tag
///
/// @param words - output vector with words
///
void _SiteData::processTitleTag( std::vector< std::wstring >& words )
{
	PR_ASSERT( m_buf.size() == m_bufSize + 1 );
	if ( m_bufSize == 0 )
		return;

	// Note: we can use buffer as a null-terminated string
	// as '\0' was added explicitly when the buffer was filled out
	_processTitleTag( ( tCHAR* ) &m_buf[ 0 ], words );
}


tDWORD _SiteData::_compact_spaces(tCHAR * str, tDWORD nLen, tDWORD * pWordsCount )
{
	tDWORD k = 0, i = 0, wordsCount = 0;
	bool w = false, s = true;

	for(; i < nLen; i++)
	{
		PR_ASSERT( i >= 0 && i < nLen );
		tCHAR c = str[i];

		bool bSpace = false;
		switch(c)
		{
		case ' ': case '\t': case '\r': case '\n': case '.': case ',':
			bSpace = true;
			break;
		}

		if( !bSpace )
		{
			if( w )
			{
				if( s )
				{
					PR_ASSERT( k >= 0 && k < nLen );
					str[k++] = ' ';
					wordsCount++;
				}
			}
			else
			{
				w = true;
				wordsCount++;
			}

			s = false;

			PR_ASSERT( k >= 0 && k < nLen );
			str[k++] = c;
			continue;
		}
		else
			s = true;
	}

	if( pWordsCount )
		*pWordsCount = wordsCount;
	return k;
}

///
/// Encode the content if necessary
///
/// @param contentEncoding - content encoding
/// @param content - the given content for encoding
///
tERROR _SiteData::checkForEncodedContent
	( 
		const hOBJECT tracer,
		const http::content_encoding contentEncodig
	)
{
	PR_ASSERT( m_buf.size() == m_bufSize + 1 );
	if ( m_bufSize == 0 )
		return errPARAMETER_INVALID;

	tERROR err = errOK;

	switch ( contentEncodig )
	{
		case http::gzip_encoding:
			{
				PR_TRACE(( tracer, prtNOTIFY, 
					"urlflt\tUrlFlt::checkForEncodedContent: gzip encoding: start"));

				// Get compressed data
				const tDataVector& compr_data = m_buf;
				const tDataVector::size_type compr_data_size = m_bufSize;

				// Get GZIP header info
				tDataVector::size_type header_compr_data_offset = 0;
				tDataVector::size_type header_compr_data_size = 0;
				tDWORD header_decompr_crc32 = 0;
				tDWORD header_decompr_data_size = 0;
				err = analyzeGzipHeader
					( 
						compr_data,
						compr_data_size,
						header_compr_data_offset, 
						header_compr_data_size,
						header_decompr_crc32, 
						header_decompr_data_size 
					);
				if ( PR_FAIL( err ) )
					break;

				tDataVector decompr_data;
				err = decompressGzipData
					( 
						tracer, 
						compr_data, 
						compr_data_size,
						header_compr_data_offset, 
						header_compr_data_size,
						decompr_data 
					);
				if ( PR_FAIL( err ) )
					break;

				PR_ASSERT( header_decompr_data_size == decompr_data.size() );
				if ( header_decompr_data_size != decompr_data.size() )
				{
					PR_TRACE(( tracer, prtNOTIFY, 
						"urlflt\tUrlFlt::checkForEncodedContent: gzip encoding: data inconsistency: "
						"header size = %d, decompressed size = %d",
						( int ) header_decompr_data_size, ( int )decompr_data.size() ));

					err = errNOT_OK;
					break;
				}

				if ( !decompr_data.empty() )
				{
					// Check CRC32
					const tDWORD crc32 = iCountCRC32
						( 
							static_cast< int >( decompr_data.size() ), 
							&decompr_data[ 0 ], 
							0 
						);

					PR_ASSERT( header_decompr_crc32 == crc32 );
					if ( header_decompr_crc32 != crc32 )
					{
						PR_TRACE(( tracer, prtNOTIFY, 
							"urlflt\tUrlFlt::checkForEncodedContent: gzip encoding: data inconsistency: "
							"header crc = %u, decompressed data src = %u",
							( unsigned int ) header_decompr_crc32, ( unsigned int ) crc32 ));
						err = errNOT_OK;
						break;
					}
				}

				// Update buffer
				m_bufSize = decompr_data.size();

				// Add null-terminating character
				decompr_data.push_back( tBYTE() );
				m_buf = decompr_data;

				PR_TRACE(( tracer, prtNOTIFY, 
					"urlflt\tUrlFlt::checkForEncodedContent: gzip encoding: end: decoded content size = %d, %terr",
					( int ) m_bufSize, err ));
			}
			break;

		case http::compress_encoding:
		case http::deflate_encoding:
			err = errNOT_IMPLEMENTED;
			break;
	}

	return err;
}

///
/// Check the given content for being a binary stream (PNG, GIF, etc)
///
/// @param contentType - content type
/// @param content - the given content for encoding
///
bool _SiteData::checkForBinaryContent( const http::content_type contentType )
{
	bool textDetected = false;

	switch ( contentType )
	{
	case http::_text:
	case http::_html:
		textDetected = true;
		break;
	}

	if ( textDetected )
		return false; // No binary data is detected

	bool res = false;

	switch ( contentType )
	{
	case http::_binary:
	case http::_audio:
	case http::_image:
	case http::_video:
		res = true;
		break;
	}

	// Check for PNG
	if ( !res )
	{
		// PNG Signature: 137 80 78 71 13 10 26 10 (0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A)
		const int PNG_SIGNATURE_SIZE = 8;
		const int PNG_SIGNATURE[ PNG_SIGNATURE_SIZE ] = 
			{ 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };

		res = _CompareData
			( 
				m_buf, 
				0,
				PNG_SIGNATURE, 
				PNG_SIGNATURE_SIZE 
			);
	}

	// Check for GIF
	if ( !res )
	{
		// GIF signature: "GIF" + version ( "87a", "89a" ) = 6 bytes
		const int GIF_SIGNATURE_SIZE = 6;
		const int GIF_89a_SIGNATURE[] = { 'G', 'I', 'F', '8', '9', 'a' };
		const int GIF_87a_SIGNATURE[] = { 'G', 'I', 'F', '8', '7', 'a' };

		res = _CompareData( m_buf, 0, GIF_89a_SIGNATURE, GIF_SIGNATURE_SIZE );

		if ( !res )
			res = _CompareData( m_buf, 0, GIF_87a_SIGNATURE, GIF_SIGNATURE_SIZE );
	}

	// Check for JPEG
	if ( !res )
	{
		// JPEG signature: FF D8 FF E0 xx xx 4A 46 49 46 00
		const int JPEG_SIGNATURE1_SIZE = 4;
		const int JPEG_SIGNATURE1[ JPEG_SIGNATURE1_SIZE ] = { 0xFF, 0xD8, 0xFF, 0xE0 };
		const int JPEG_SIGNATURE2_SIZE = 5;
		const int JPEG_SIGNATURE2[ JPEG_SIGNATURE2_SIZE ] = { 0x4A, 0x46, 0x49, 0x46, 0x00 };

		if ( _CompareData( m_buf, 0, JPEG_SIGNATURE1, JPEG_SIGNATURE1_SIZE ) )
		{
			res = _CompareData
				( 
					m_buf, 
					JPEG_SIGNATURE1_SIZE + 2, 
					JPEG_SIGNATURE2, 
					JPEG_SIGNATURE2_SIZE 
				);
		}
	}

	// Check for Macromedia Flash
	if ( !res )
	{
		// Flash signature: 0x46, 0x57, 0x53 (FWS) or 0x46, 0x57, 0x43 (CWS) 	
		const int SWF_SIGNATURE_SIZE = 3;
		const int SWF_SIGNATURE[] = { 'F', 'W', 'S' };
		const int SWC_SIGNATURE[] = { 'C', 'W', 'S' }; // Compressed SWF

		res = _CompareData( m_buf, 0, SWF_SIGNATURE, SWF_SIGNATURE_SIZE );

		if ( !res )
			res = _CompareData( m_buf, 0, SWC_SIGNATURE, SWF_SIGNATURE_SIZE );
	}

	return res;
}

//////////////////////////////////////////////////////////////////////////

///
/// Update detection statistics
///
/// @param err - detection error code
/// @param evt - detection event
/// @param profile - current profile to be updated
///
void UrlFlt::updateStats
	( 
		const tDWORD errCode, 
		const cUrlFltReportEvent& evt, 
		tProfile* const profile
	)
{
	//PR_TRACE((this, prtNOT_IMPORTANT, "urlflt\tUrlFlt::updateStats: start"));

	cAutoCS _lock(m_hCS, true);

	m_common.m_stat.m_nURLsChecked++;

	const bool timeLimitDetection = 
		( evt.m_nURLDetectType == cUrlFltReportEvent::eDtTimeLimit );

	if( errCode != errOK )
	{
		// We don't count violations in case it is just time limitation
		if ( !timeLimitDetection )
		{
			m_common.m_stat.m_nURLsDetected++;
		}
	}

	if( profile )
	{
		profile->m_stat.m_nURLsChecked++;
		if( errCode != errOK )
		{
			// We don't count violations in case it is just time limitation
			if ( !timeLimitDetection )
			{
				profile->m_stat.m_nURLsDetected++;
			}
		}
	}

	//PR_TRACE((this, prtNOT_IMPORTANT, "urlflt\tUrlFlt::updateStats: end"));
}

///
/// Get severity code 
///
/// @param err - detection error code
/// @param evt - detection event
///
tDWORD UrlFlt::GetSeverity
	(
		const tDWORD errCode, 
		const cUrlFltReportEvent& evt
	) const
{
	tDWORD severity = 0;

	if 
		(
			( evt.m_nURLDetectType != cUrlFltReportEvent::eDtNone )
			&&
			( evt.m_nURLDetectType != cUrlFltReportEvent::eDtWhiteList )
		)
	{
		if ( errCode == errACCESS_DENIED )
		{
			severity = pmc_EVENTS_CRITICAL;
		}
		else
		{
			severity = pmc_EVENTS_IMPORTANT;
		}
	}
	else
	{
		severity = pmc_EVENTS_NOTIFY;
	}

	return severity;
}


///
/// Check the given URL in cache
///
/// @param strFullURL - full URL
/// @param _Evt - event with results of checking
/// @param errCode - error code
/// @param _dbRes - output category results for the given URL
/// @param lock - true if the call should be performed under lock
/// @return true if the given URL is found in cache, false - otherwise
///
UrlFlt::eCacheRes UrlFlt::findInCache
	( 
		const cStrObj& strFullURL,
		cUrlFltReportEvent& _Evt,
		tERROR& errCode,
		CParctlDb::Result& _dbRes,
		const bool lock
	)
{
	//PR_TRACE((this, prtNOT_IMPORTANT, "urlflt\tUrlFlt::findInCache: start"));

	// NOTE: The call should be protected with cache's CS
	if ( lock )
	{
		PR_ASSERT( m_cacheCS != NULL );
		m_cacheCS->Enter( SHARE_LEVEL_READ );
	}

	const time_t nNow = static_cast< time_t >( _Evt.m_tmTimeStamp );

	/*
	// Check URL by cache
	tHeurObj * pHeurObj = NULL;
	eCacheRes nCacheRes = eCrProcess;
	*/
	eCacheRes res = eCacheNone;

	// Ищем сайт в кеше
	{
		//cAutoCS lock( m_cacheCS, false );

		//for( int i = 0, n = m_cache.size(); i < n; ++i )
		// The most recent items are put at the end of the cache
		const int n = m_cache.size();
		for( int i = n - 1; i >= 0; --i )
		{
			tCasheObj& _c = m_cache[i];
			// See comments about cache usage below
			//if( strSiteURL.compare(0, cStrObj::whole, _c.m_url.c_str(), fSTRING_COMPARE_CASE_INSENSITIVE) )
			if( strFullURL.compare( 0, cStrObj::whole, _c.m_url.c_str(), fSTRING_COMPARE_CASE_INSENSITIVE ) != 0 )
				continue;

			// Perform cleaning procedure when idle message is received
			/*
			// Закешированные ОШИБОЧНЫЕ статусы используем не более m_nCacheErrTTL секунд !!!
			if( _c.m_err && ((tDWORD)(nNow - _c.m_time) > m_nCacheErrTTL) )
			{
				m_cache.remove(i);
				break;
			}
			*/
			res = ( _c.m_analyzed ? eCacheFound : eCacheNotAnalyzed );

			if ( _c.m_analyzed )
			{
				_dbRes = _c.m_dbRes;
				errCode = ( _c.m_err ? errACCESS_DENIED : errOK );
				_Evt.m_nURLDetectType = static_cast< cUrlFltReportEvent::eDetectType >( _c.m_URLDetectType );
			}
			/*
			nCacheRes = eCrFound;
			*/

			PR_TRACE((this, prtNOTIFY, 
				"urlflt\tUrlFlt::findInCache: Cache: site found: [%s] (cache index = %d, cache size = %d)",
				_c.m_url.c_str(), i, ( int ) m_cache.size() ));

			/*
			// Для ОШИБОЧНЫХ статусов НЕ нужно модифицировать время последнего обращения !!!
			if( _c.m_err )
			{
				/*
				bDeny = _c.m_err_deny;
				*
				break;
			}
			*/

			/*
			if ( i )
			{
				tCasheObj _t = _c;
				_t.m_time = nNow;
				m_cache.remove(i, i);
				m_cache.insert(0) = _t;
			}
			*/
			if ( _c.m_analyzed )
			{
				// Swap to make it the most recent item
				if ( i < n - 1 )
				{
					tCasheObj _t = _c;
					m_cache.remove( i, i );
					m_cache.push_back() = _t;
				}
			}
			else
			{
				// Don't cache the item too long if it wasn't analyzed before
				// (i.e. any error occured)
				if ( ( tDWORD )( nNow - _c.m_time ) > m_nCacheErrTTL )
				{
					PR_TRACE((this, prtNOTIFY, 
						"urlflt\tUrlFlt::findInCache: Cache: site removed: [%s] (cache index = %d, cache size = %d)",
						_c.m_url.c_str(), i, ( int ) m_cache.size() ));

					m_cache.remove( i );
				}
			}

			break;
		}
	}

	/*
	// Ищем сайт в текущих анализируемых сайтах
	if( nCacheRes == eCrProcess )
	{
		// Search-insert should be processed as single operation
		cAutoCS lock( m_heurObjsCS, false );

		//int n;
		for(int i = 0, n = m_heurObjs.size(); i < n; i++)
		{
			tHeurObj * _h = m_heurObjs[i];
			//if( !strSiteURL.compare(0, cStrObj::whole, _h->m_url.c_str(), fSTRING_COMPARE_CASE_INSENSITIVE) )
			if( !strFullURL.compare(0, cStrObj::whole, _h->m_url.c_str(), fSTRING_COMPARE_CASE_INSENSITIVE) )
			{
				nCacheRes = eCrProcessWait;
				pHeurObj = _h;
				pHeurObj->addRef();
				break;
			}
		}

		if( !pHeurObj )
		{
			pHeurObj = tHeurObj::_new();
			if( pHeurObj )
			{
				//pHeurObj->m_url = (tCHAR *)strSiteURL.c_str(cCP_ANSI);
				pHeurObj->m_url = (tCHAR *)strFullURL.c_str(cCP_ANSI);
				m_heurObjs.push_back(pHeurObj);
			}
			else
				errHeuristic = errNOT_ENOUGH_MEMORY;
		}
	}

	else if( nCacheRes == eCrProcessWait )
	{
		while( !pHeurObj->m_complete )
			PrSleep(100);

		_dbRes = pHeurObj->m_dbRes;
		bDeny = pHeurObj->m_deny;
	}

	if( pHeurObj )
		pHeurObj->release();
	*/

	/*
#ifdef _DEBUG
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::findInCache: Cache results: "));
	for( int i = 0, n = m_cache.size(); i < n; ++i )
	{
		const tCasheObj& _c = m_cache[i];
		// See comments about cache usage below
		//if( strSiteURL.compare(0, cStrObj::whole, _c.m_url.c_str(), fSTRING_COMPARE_CASE_INSENSITIVE) )
		PR_TRACE((this, prtNOTIFY, 
			"urlflt\tUrlFlt::findInCache: Cache: %d => [%s], %d",
			i, _c.m_url.c_str(), _c.m_analyzed ));
	}
#endif // _DEBUG
	*/

	//PR_TRACE((this, prtNOT_IMPORTANT, "urlflt\tUrlFlt::findInCache: end"));

	if ( lock )
	{
		PR_ASSERT( m_cacheCS != NULL );
		m_cacheCS->Leave( 0 );
	}

	return res;
}

///
/// Add the given URL to the cache
///
/// @param strFullURL - full URL
/// @param _Evt - event with results of checking
/// @param errCode - error code
/// @param _dbRes - output category results for the given URL
/// @param analyzed - true if the given item was analyzed, 
///		false - the item was not analyzed (any error could occurred)
///
void UrlFlt::addToCache
	( 
		const cStrObj& strFullURL, 
		cUrlFltReportEvent& _Evt,
		tERROR& errCode,
		CParctlDb::Result& _dbRes,
		bool analyzed
	)
{
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::addToCache: start"));

	// NOTE: The call should be protected with CS
	cAutoCS lock( m_cacheCS, true );

	const time_t nNow = static_cast< time_t >( _Evt.m_tmTimeStamp );

	// The results can be added to the cache after previous call to findInCache,
	// so, check it again
	if ( findInCache( strFullURL, _Evt, errCode, _dbRes, false ) != eCacheNone )
		return;

	{
		//cAutoCS _lock(m_hCS, true);
		
		// Add analyzed URL in cache
		// Не запоминаем результат эвристики при ошибке авторизации, что бы диалог авторизации показывался снова
		/*
		if
			( 
				( errHeuristic != PR_MAKE_DECL_ERR(IID_UPDATERCONFIGURATOR, KLUPD::CORE_PROXY_AUTH_ERROR) )
				&&
				storeInCache
			)
		*/
		{
			//cAutoCS lock( m_cacheCS, true );

			/*
			if( m_cache.size() >= m_nCache )
				m_cache.resize(m_nCache ? (m_nCache - 1) : 0);

			tCasheObj& _c = m_cache.insert(0);
			*/
			if ( m_cache.size() >= m_nCache )
			{
				// Delete the least recent item
				m_cache.remove( 0, 0 );
			}
			tCasheObj& _c = m_cache.push_back();

			// Note: we have to put the full URL to the cache.
			// Otherwise, the result of resolving the access
			// on the same domain name (with cache option enabled)
			// will depend on the order of visiting pages.
			// For instance: domain/site1 - good, domain/site2 - bad
			// If we visit domain/site1, then it will be 
			// always possible to visit domain/site2, 
			// and vice versa.
			//_c.m_url = (tCHAR *)strSiteURL.c_str(cCP_ANSI);
			_c.m_url = ( tCHAR* ) strFullURL.c_str( cCP_ANSI );

			_c.m_dbRes = _dbRes;
			_c.m_time = nNow;
			//_c.m_err = PR_FAIL(errHeuristic);
			_c.m_err = PR_FAIL( errCode );
			_c.m_URLDetectType = _Evt.m_nURLDetectType;
			/*
			_c.m_err_deny = bDeny;
			*/
			_c.m_analyzed = analyzed;

			PR_TRACE((this, prtNOTIFY, 
				"urlflt\tUrlFlt::addToCache: Cache: site added: [%s] (cache size = %d)", 
				_c.m_url.c_str(), ( int ) m_cache.size() ));
		}

		/*
		{
			cAutoCS lock( m_heurObjsCS, true );

			tDWORD i, n;
			for(i = 0, n = m_heurObjs.size(); i < n; i++)
			{
				if( m_heurObjs[i] == pHeurObj )
				{
					m_heurObjs.remove(i);
					break;
				}
			}
		}

		if( errHeuristic == errOK )
			m_fHeuristicSuccess = 1;
		*/
	}
	
	/*
    if( errHeuristic == errOK )
    {
	    pHeurObj->m_dbRes = _dbRes;
	    pHeurObj->m_deny = bDeny;
	    pHeurObj->m_complete = 1;
    }
	*/

	/*
#ifdef _DEBUG
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::addToCache: Cache results: "));
	for( int i = 0, n = m_cache.size(); i < n; ++i )
	{
		const tCasheObj& _c = m_cache[i];
		// See comments about cache usage below
		//if( strSiteURL.compare(0, cStrObj::whole, _c.m_url.c_str(), fSTRING_COMPARE_CASE_INSENSITIVE) )
		PR_TRACE((this, prtNOTIFY, 
			"urlflt\tUrlFlt::addToCache: Cache: %d => [%s], %d",
			i, _c.m_url.c_str(), ( int ) _c.m_analyzed ));
	}
#endif // _DEBUG
	*/

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::addToCache: end"));
}

void _copy_string_to_array( const std::string& str, TCHAR* buf, int bufSize )
{
	// Copy string
	int count = _MIN( ( int ) str.length(), bufSize - 1 );
	if ( count > 0 )
	{
		PR_ASSERT( count > 0 && count <= bufSize );
		strncpy( buf, str.c_str(), count );
	}

	// Add null-terminated character
	PR_ASSERT( count >= 0 && count < bufSize );
	buf[ count ] = 0;
}

///
/// Convert the given URL to the standard appearance
///
/// @param sUrl - given URL is about to be converted
/// @param buf - string buffer
/// @param url - output url with standard appearance
/// @param resolve - true if it is required to resolve IP address when given
/// @return error code
///
tERROR UrlFlt::normalizeUrl
	( 
		const cStrObj& sURL, 
		cStrBuff& buf, 
		CUrlListCache::tURL& url, 
		const bool resolve 
	)
{
	//cStrBuff buf;
	//CUrlListCache::tURL url;

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::normalizeUrl: start: m_sUrl = [%S]", sURL.data()));

	if( !buf.reserve(url.BufSize(sURL), true) )
		return errNOT_ENOUGH_MEMORY;

	url.Normalize( sURL, buf, buf.size(), CUrlListCache::tURL::fSimplify );
	if( !url.m_hostLen )
	{
		PR_TRACE((this, prtIMPORTANT, 
			"urlflt\tnormalizeUrl: URL = [%S] (normalized: [%s])", 
			sURL.data(), url.m_url ));
		return errFOUND;
	}

	if ( resolve )
	{
		// Check for IP
		/*
		if( url.m_host[0] >= '0' && url.m_host[0] <= '9' )
		*/
		if ( url.m_hostLen > 0 )
		{
			std::string host_name( url.m_host, url.m_hostLen );

			boost::regex regex
				( 
					"[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}", 
					boost::regex::normal | boost::regex::icase 
				);
			boost::cmatch match_res;

			if ( boost::regex_search( host_name.c_str(), match_res, regex ) )
			{
				const int strHostResolvedSize = 256;
				tCHAR strHostResolved[ strHostResolvedSize ];

				/*
				tCHAR _c = url.m_host[url.m_hostLen];
				url.m_host[url.m_hostLen] = 0;
				*/

				bool found = false;
				{
					cAutoCS lock( m_dnsCacheCS, false );
					const DnsCache::size_type index = m_dnsCache.Find( host_name, true );
					found = ( index != DnsCache::npos );

					if ( found )
					{
						const std::string& value = m_dnsCache.GetValue( index );
						_copy_string_to_array( value, strHostResolved, strHostResolvedSize );

						PR_TRACE((this, prtNOTIFY, 
							"urlflt\tUrlFlt::normalizeUrl: Cache: url found: [%s] => [%s] (cache size = %d)",
							host_name.c_str(), value.c_str(), ( int ) m_dnsCache.GetSize() ));
					}
				}

				if ( !found )
				{
					//_Sock_GetHostNameByIP(url.m_host, strHostResolved, sizeof(strHostResolved));
					_Sock_GetHostNameByIP( host_name.c_str(), strHostResolved, sizeof(strHostResolved));

					cAutoCS lock( m_dnsCacheCS, true );
					const time_t now = cDateTime::now_utc();
					m_dnsCache.Add( host_name, strHostResolved, 900, now );

					PR_TRACE((this, prtNOTIFY, 
						"urlflt\tUrlFlt::normalizeUrl: Cache: url added: [%s] => [%s] (cache size = %d)",
						host_name.c_str(), strHostResolved, ( int ) m_dnsCache.GetSize() ));
				}

#ifdef _DEBUG
				// Show DNS cache
				{
					cAutoCS lock( m_dnsCacheCS, true );
					const DnsCache::size_type count = m_dnsCache.GetSize();
					for ( DnsCache::size_type i = 0; i < count; ++i )
					{
						PR_TRACE((this, prtNOTIFY, 
							"urlflt\tUrlFlt::normalizeUrl: %d => [%s], [%s]", 
							( int ) i, m_dnsCache.GetKey( i ).c_str(), m_dnsCache.GetValue( i ).c_str() ));
					}
				}
#endif // _DEBUG

				/*
				url.m_host[url.m_hostLen] = _c;
				*/

				if( strHostResolved[0] )
				{
					std::string strResolvedURL;
					if( url.m_protoLen )
					{
						strResolvedURL.append(url.m_proto, url.m_protoLen);
						strResolvedURL.append("://");
					}

					if( url.m_authLen )
					{
						strResolvedURL.append(url.m_auth, url.m_authLen);
						strResolvedURL.append("@");
					}

					strResolvedURL.append(strHostResolved);

					if( url.m_portLen )
					{
						strResolvedURL.append(":");
						strResolvedURL.append(url.m_port, url.m_portLen);
					}

					if( url.m_pathLen )
					{
						strResolvedURL.append("/");
						strResolvedURL.append(url.m_path, url.m_pathLen);
					}

					cStrObj _urlW(strResolvedURL.c_str());
					if( !buf.reserve(url.BufSize(_urlW), true) )
						return errNOT_ENOUGH_MEMORY;

					url.Normalize( _urlW, buf, buf.size(), CUrlListCache::tURL::fSimplify );
				}
			}
		}
	}

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::normalizeUrl: end"));

	return errOK;
}

///
/// Analyze the given results to make the final decision
///
/// @param _dbRes - category results for checking
/// @param nDenyCategoriesMask - category masks
///
tERROR UrlFlt::checkDbRes
	( 
		const CParctlDb::Result& _dbRes,
		const tQWORD& denyCategoriesMask 
	)
{
	tERROR err = errOK;

	//PR_TRACE((this, prtNOT_IMPORTANT, "urlflt\tUrlFlt::checkDbRes: start"));

	// Check result catgrange with filter categories settings
	for(tDWORD i = 0, n = _dbRes.size(); i < n; i++)
	{
		const CParctlDb::ResultCatg& _c = _dbRes[i];

		PR_TRACE((this, prtNOTIFY, 
			"urlflt\tUrlFlt::checkDbRes: category: id = %d, weight = %d", 
			( int )_c.m_id, ( int )_c.m_weight));

		if ( _c.StopAnalyzing( denyCategoriesMask ) )
		{
			err = errACCESS_DENIED;
			break;
		}
	}

	//PR_TRACE((this, prtNOT_IMPORTANT, "urlflt\tUrlFlt::checkDbRes: end"));

	return err;
}

///
/// Join database results to get the best possible one
///
/// @param dbRes1 - category results
/// @param dbRes2 - category results
///
void UrlFlt::bestDbRes( const CParctlDb::Result& dbRes1, CParctlDb::Result& dbRes2 )
{
	PR_TRACE((this, prtNOTIFY, 
		"urlflt\tUrlFlt::bestDbRes: start" ));
	if ( !dbRes1.empty() )
	{
		PR_TRACE((this, prtNOTIFY, 
			"urlflt\tUrlFlt::bestDbRes: Database results 1" ));
		DUMP_DB_RES( ( hOBJECT ) this, dbRes1 );
	}
	if ( !dbRes2.empty() )
	{
		PR_TRACE((this, prtNOTIFY, 
			"urlflt\tUrlFlt::bestDbRes: Database results 2" ));
		DUMP_DB_RES( ( hOBJECT ) this, dbRes2 );
	}

	CParctlDb::bestDbRes( dbRes1, dbRes2 );

	if ( !dbRes2.empty() )
	{
		PR_TRACE((this, prtNOTIFY, 
			"urlflt\tUrlFlt::bestDbRes: Database results 2" ));
		DUMP_DB_RES( ( hOBJECT ) this, dbRes2 );
	}

	PR_TRACE((this, prtNOTIFY, 
		"urlflt\tUrlFlt::bestDbRes: end" ));
}

///
/// Find the given URL in currently processed URL's
///
/// @param url - the given URL
/// @param lock - true if the call should be under lock
///
bool UrlFlt::_findInProc( const cStrObj& url, bool lock )
{
	bool res = false;

	if ( lock )
	{
		PR_ASSERT( m_procCache.cs != NULL );
		m_procCache.cs->Enter( SHARE_LEVEL_READ );
	}

	const ProcCache::size_type size = m_procCache.cache.size(); 
	for( ProcCache::size_type i = 0; i < size; ++i )
	{
		if ( url == m_procCache.cache[ i ] )
		{
			PR_TRACE((this, prtNOTIFY, 
				"urlflt\tUrlFlt::findInProc: Proc: site found: [%S] (cache size = %d)", 
				url.data(), ( int ) m_procCache.cache.size() ));

			res = true;
			break;
		}
	}

	if ( lock )
	{
		PR_ASSERT( m_procCache.cs != NULL );
		m_procCache.cs->Leave( 0 );
	}

	return res;
}

///
/// Find the given URL in currently processed URL's
///
/// @param url - the given URL
///
bool UrlFlt::findInProc( const cStrObj& url )
{
	return _findInProc( url, true );
}

///
/// Mark the given URL as currently being processed
///
/// @param url - the given URL
///
void UrlFlt::addToProc( const cStrObj& url )
{
	// Search-insert should be performed as a single operation
	cAutoCS lock( m_procCache.cs, true );

	if ( _findInProc( url, false ) )
		return;

	m_procCache.cache.push_back( url );

	PR_TRACE((this, prtNOTIFY, 
		"urlflt\tUrlFlt::addToProc: Proc: site added: [%S] (cache size = %d)", 
		url.data(), ( int ) m_procCache.cache.size() ));
}

///
/// Mark the given URL as currently not being in process
///
/// @param url - the given URL
///
void UrlFlt::removeFromProc( const cStrObj& url )
{
	cAutoCS lock( m_procCache.cs, true );

	const ProcCache::size_type size = m_procCache.cache.size(); 
	for( ProcCache::size_type i = 0; i < size; ++i )
	{
		if ( url == m_procCache.cache[ i ] )
		{
			PR_TRACE((this, prtNOTIFY, 
				"urlflt\tUrlFlt::removeFromProc: Proc: site removed: [%S] (cache size = %d)", 
				url.data(), ( int ) m_procCache.cache.size() ));

			m_procCache.cache.erase( m_procCache.cache.begin() + i );
			break;
		}
	}
}

///
/// Check if the given URL is currently in processing; if so, 
/// wait until it is being processed
///
void UrlFlt::checkInProc( const cStrObj& url )
{
	int i = 0;
	while ( i < 100 ) // max wait is 25 seconds
	{
		if ( !findInProc( url ) )
		{
			break;
		}

		// the given URL is currently in process, wait
		::Sleep( 250 );

		// Try again, but not too many times
		++i;
	}
}

///
/// Load the content of the given URL
///
/// @param urlFlt - URL request information is about to be retrieved
/// @param pContent - content of the page
///
tERROR UrlFlt::loadPage( cUrlFilteringRequest& urlFlt, hIO* pContent, cToken* pToken )
{
	PR_ASSERT( pContent != NULL );
	if ( pContent == NULL )
		return errPARAMETER_INVALID;

	PR_TRACE(( this, prtNOTIFY, "urlflt\tUrlFlt::loadPage: start: [%S]",
		urlFlt.m_sURL.data() ));
	const DWORD start = GetTickCount();

	tERROR err = errOK;

	// Create input io
	hIO hSrcIO = NULL;
	err = CALL_SYS_ObjectCreate( this, &hSrcIO, IID_IO, PID_TMPFILE, SUBTYPE_ANY );

	if ( PR_SUCC( err ) )
	{
		CALL_SYS_PropertySetDWord
			( 
				( hOBJECT ) hSrcIO, 
				pgOBJECT_ACCESS_MODE, 
				fACCESS_RW 
			);
		CALL_SYS_PropertySetDWord
			( 
				( hOBJECT ) hSrcIO, 
				pgOBJECT_OPEN_MODE, 
				fOMODE_CREATE_ALWAYS | fOMODE_SHARE_DENY_READ 
			);

		err = CALL_SYS_ObjectCreateDone( hSrcIO );

		if ( PR_SUCC( err ) )
		{
			cTransport* pTransp = NULL; 
			err = _TrMgr_GetObj( &pTransp );

			if ( PR_SUCC( err ) )
			{
				// ?
				if( pToken != NULL )
				{
					err = pToken->Impersonate();
				}

				if ( PR_SUCC( err ) )
				{
					err = pTransp->httpGetFile( cAutoString( urlFlt.m_sURL ), hSrcIO );

					// ?
					if( pToken != NULL )
					{
						pToken->Revert();
					}
				}

				if ( PR_SUCC( err ) )
				{
					tDWORD propSize = 0;
					err = pTransp->propGetStr( &propSize, pgTRANSPORT_HTTP_HEADER, NULL, 0 );

					if ( PR_SUCC( err ) && ( propSize > 0 ) )
					{
						const std::vector< char >::size_type headers_size = 
							static_cast< std::vector< char >::size_type >( propSize );
						std::vector< char > headers( headers_size );
						if ( headers.size() == headers_size )
						{
							err = pTransp->get_pgTRANSPORT_HTTP_HEADER
								( 
									&headers[ 0 ], 
									static_cast< tDWORD >( headers_size ) 
								);
							if ( PR_SUCC( err ) )
							{
								*pContent = hSrcIO;

								// Analyze headers
								http::analyzer an;
								an.enqueue( &headers[ 0 ], &headers[ 0 ] + headers_size );
								std::auto_ptr< http::message > msg( an.extract() );

								if ( msg.get() )
								{
									urlFlt.init( msg.get() );
								}
								else
								{
									err = errFOUND;
								}
							}
						}
						else
						{
							err = errNOT_ENOUGH_MEMORY;
						}
					}
					else
					{
						err = errFOUND;
					}
				}

				_TrMgr_ReleaseObj( pTransp );
				pTransp = NULL;
			}
		}

		// It must be released later by the caller
		//CALL_SYS_Release( hSrcIO );
		//hSrcIO = NULL;
	}

	const tDWORD end = GetTickCount();

	PR_TRACE(( this, prtNOTIFY, "urlflt\tUrlFlt::loadPage: end: time = %d for [%S]",
		( int ) end - start, urlFlt.m_sURL.data() ));

	return err;
}

///
/// Analyze the given content
///
tERROR UrlFlt::AnalyzeObject
	(
		const cUrlFilteringRequest* pUrlFlt, 
		hIO content,
		const tProfile* _profile,
		const tQWORD& nDenyCategoriesMask,
		const CUrlListCache::tURL& url,
		cUrlFltReportEvent& _Evt,
		CParctlDb::Result& _dbRes
	)
{
	if ( !pUrlFlt )
		return errPARAMETER_INVALID;

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::AnalyzeObject: start"));

	// Note: it seems that authorization requests should be checked as well!
	// (Otherwise we get the response for other data on the page!)
	// The only thing that such kind of requests shouldn't be cached!
	const bool infoResponse = checkForInfoResponse( pUrlFlt->m_StatusCode );
	/*
	// Check for authorization request: no necessity in checking
	if ( checkForAuthRequest( pUrlFlt->m_StatusCode ) )
		return errOK;
	*/

	// See ProcessObject for details
	/*
	tERROR err;
	*/
	// See ProcessObject for details
	/*
	cStrBuff buf;
	CUrlListCache::tURL url;

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::ProcessObject: m_sUrl = [%S]", pUrlFlt->m_sURL.data()));

	if( !buf.reserve(url.BufSize(pUrlFlt->m_sURL), true) )
		return errNOT_ENOUGH_MEMORY;

	url.Normalize(pUrlFlt->m_sURL, buf, CUrlListCache::tURL::fSimplify);
	if( !url.m_hostLen )
	{
		PR_TRACE((this, prtIMPORTANT, "urlflt\tProcessObject: URL=[%S] (normalized: [%s]), PID=%u skipped becouse of empty host!", pUrlFlt->m_sURL.data(), url.m_url, pUrlFlt->m_nPID));
		return errOK;
	}

	// Check for IP
	if( url.m_host[0] >= '0' && url.m_host[0] <= '9' )
	{
		tCHAR strHostResolved[256];

		tCHAR _c = url.m_host[url.m_hostLen];
		url.m_host[url.m_hostLen] = 0;
		_Sock_GetHostNameByIP(url.m_host, strHostResolved, sizeof(strHostResolved));
		url.m_host[url.m_hostLen] = _c;

		if( strHostResolved[0] )
		{
			std::string strResolvedURL;
			if( url.m_protoLen )
			{
				strResolvedURL.append(url.m_proto, url.m_protoLen);
				strResolvedURL.append("://");
			}
			
			if( url.m_authLen )
			{
				strResolvedURL.append(url.m_auth, url.m_authLen);
				strResolvedURL.append("@");
			}
			
			strResolvedURL.append(strHostResolved);

			if( url.m_portLen )
			{
				strResolvedURL.append(":");
				strResolvedURL.append(url.m_port, url.m_portLen);
			}

			if( url.m_pathLen )
			{
				strResolvedURL.append("/");
				strResolvedURL.append(url.m_path, url.m_pathLen);
			}

			cStrObj _urlW(strResolvedURL.c_str());
			if( !buf.reserve(url.BufSize(_urlW), true) )
				return errNOT_ENOUGH_MEMORY;

			url.Normalize(_urlW, buf, CUrlListCache::tURL::fSimplify);
		}
	}
	*/

	// See ProcessObject for details
	/*
	time_t nNow = cDateTime::now_utc();
	*/
	const time_t nNow = static_cast< time_t >( _Evt.m_tmTimeStamp );

	bool bHeuristic = false;
	bool bHeuristicOnly = false;
	bool bProfiles = false;
	tHeurPrm stHeurPrm;
	bool bAskAction = false;
	bool bCheckMainPage = false;
	bool bUseReportFile = false;
	// See ProcessObject for details
	/*
	cUrlFltReportEvent _Evt(cUrlFltReportEvent::eProcess);
	*/

	{
		cAutoCS _lock(m_hCS, false);
		
		bHeuristic = m_fHeuristic;
		bHeuristicOnly = m_fHeuristicOnly;
		bProfiles = m_fProfiles;
		bAskAction = m_fAskAction;
		bCheckMainPage = m_fCheckMainPage;
		bUseReportFile = m_fUseReportFile;
		// See ProcessObject for details
		/*
		_Evt.assign(m_AskActionDef, false);
		*/
		
		stHeurPrm.m_fHeuristicContentDescr = m_fHeuristicContentDescr;
		stHeurPrm.m_fHeuristicContentUrls = m_fHeuristicContentUrls;
		stHeurPrm.m_nMaxDataSize = m_nMaxHeuristicDataSize;
		stHeurPrm.m_nTranspNetTimeout = m_nTranspNetTimeout;
	}

	stHeurPrm.m_strTranspUserAgent = L"Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)";

	// See ProcessObject for details
	/*
	cAutoObj<cToken> hToken;
	cStrObj strUser;
	if( bProfiles )
	{
		// For optimizing !!!!!!!!!!!!! cProcMonNotify pmc_PROCESS_MONITOR, pm_PROCESS_MONITOR_PROCESS_EXIT
		
		tDWORD nType;
		err = _GetUserByPID(pUrlFlt->m_nPID, hToken, strUser, nType);
		if( PR_FAIL(err) )
		{
			PR_TRACE((this, prtERROR, "urlflt\tProcessObject: GetUserByPID error: %terr", err));
		}
		else if( nType != TOKEN_TYPE_USER )
		{
			PR_TRACE((this, prtIMPORTANT, "urlflt\tProcessObject: URL=[%S] (normalized: [%s]), PID=%u, User=[%S]: is not a user account type=%u, skipped", pUrlFlt->m_sURL.data(), url.m_url, pUrlFlt->m_nPID, strUser.data(), nType));
			return errOK;
		}
	}
	*/

	// See ProcessObject for details
	/*
	_Evt.m_tmTimeStamp		= (__time32_t)nNow;
	_Evt.m_nPID             = pUrlFlt->m_nPID;
	_Evt.m_strUserId		= strUser;
#if	VER_PRODUCTVERSION_HIGH <= 7
	_Evt.m_strDetectObject	= pUrlFlt->m_sURL;
	_Evt.m_nDescription		= pUrlFlt->m_nPID;
#else
	_Evt.m_strObjectName	= pUrlFlt->m_sURL;
#endif	
	PR_TRACE((this, prtIMPORTANT, "urlflt\tProcessObject: URL=[%S] (normalized: [%s]), PID=%u, User=[%S]", pUrlFlt->m_sURL.data(), url.m_url, pUrlFlt->m_nPID, strUser.data()));
	*/
	PR_TRACE((this, prtIMPORTANT, 
		"urlflt\tAnalyzeObject: URL = [%S] (normalized: [%s]), PID=%u", 
		pUrlFlt->m_sURL.data(), url.m_url, pUrlFlt->m_nPID ));

	// Reports for analyzing
	ReportInfo info( *this, bUseReportFile );

	if ( info.IsEnabled() )
	{
		SYSTEMTIME sysTime;
		::ZeroMemory( &sysTime, sizeof( sysTime ) );
		const time_t nNowLoc = _TimeToLocal(nNow);
		_TimeToSystem( nNowLoc, sysTime );
		info.Trace
			( 
				"%02d:%02d:%02d",
				( int ) sysTime.wHour, 
				( int ) sysTime.wMinute, 
				( int ) sysTime.wSecond
			);
		info.Trace( "URL: %S, %d", pUrlFlt->m_sURL.data(), ( int ) pUrlFlt->m_StatusCode );
		info.Trace( "Normalized: %s", url.m_url );
	}

	// See ProcessObject for details
	/*
	tQWORD nDenyCategoriesMask = 0;
	*/
	tERROR errCheck = errOK;
	// See ProcessObject for details
	/*
	tProfile * _profile = NULL;
	bool bDenyDetected = true;
	*/
	
	// See ProcessObject for details
	/*
	// Get user profile
	{
		cAutoCS _lock(m_hCS, false);
		if( bProfiles )
		{
			_profile = _GetUserProfile(strUser);
			if( !_profile )
				_profile = _GetProfile(m_defUser.m_nProfileId);

			if( _profile )
			{
				_profile->addRef();
				_Evt.m_strUfProfile = _profile->m_strName;
				_Evt.m_nUfProfileId = _profile->m_nId;
			}
			else
			{
				PR_TRACE((this, prtIMPORTANT, "urlflt\tProcessObject: can't find (or disabled) profile for user [%S], deny request.", strUser.data()));
				errCheck = errACCESS_DENIED;
			}
		}
	}
	*/

	// See ProcessObject for details
	/*
	// Check time limits
	if( errCheck == errOK )
	{
		PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::ProcessObject: Time limit check start..."));

		cAutoCS _lock(m_hCS, true);
		if( _profile && !_CheckTimeLimit(_profile) )
		{
			errCheck = errACCESS_DENIED;
			_Evt.m_nURLDetectType = cUrlFltReportEvent::eDtTimeLimit;
			PR_TRACE((this, prtIMPORTANT, 
				"urlflt\tProcessObject: time limit detected (profile: [%S], "
				"cur day time: %u sec, day time limit: %u sec, day time space count: %u), deny request.", 
				_profile->m_strName.data(), _profile->m_nDayTime, 
				_profile->m_nDayTimeLimit, _profile->m_nDayTimeSpace.size() ));
		}

		PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::ProcessObject: Time limit check end..."));
	}
	*/

	// Check URL by white and black lists
	if( errCheck == errOK )
	{
		cAutoCS _lock(m_hCS, false);

		if( _profile )
		{
			// See ProcessObject for details
			/*
			if( _profile->m_bUseCommonFilter )
				nDenyCategoriesMask |= m_common.m_nDenyCategoriesMask;
			nDenyCategoriesMask |= _profile->m_nDenyCategoriesMask;

			if( !_profile->m_bDenyDetected )
				bDenyDetected = false;
			*/
		
			PR_TRACE((this, prtNOTIFY, 
				"urlflt\tUrlFlt::AnalyzeObject: (1.1) Check URL by profile's white and black lists: start"));

			errCheck = _profile->CheckUrl( url, info );
			if( errCheck == errOK && _profile->m_bUseCommonFilter )
				errCheck = m_common.CheckUrl( url, info );

			PR_TRACE((this, prtNOTIFY, 
				"urlflt\tUrlFlt::AnalyzeObject: (1.1) Check URL by profile's white and black lists: end, %terr", errCheck));
		}
		else
		{
			// See ProcessObject for details
			/*
			nDenyCategoriesMask |= m_common.m_nDenyCategoriesMask;
			*/

			PR_TRACE((this, prtNOTIFY, 
				"urlflt\tUrlFlt::AnalyzeObject: (1.2) Check URL by profile's white and black lists: start"));

			errCheck = m_common.CheckUrl( url, info );

			PR_TRACE((this, prtIMPORTANT, 
				"urlflt\tUrlFlt::AnalyzeObject: (1.2) Check URL by profile's white and black lists: end, %terr", 
				errCheck));
		}

		if( errCheck == errOK_DECIDED || errCheck == errACCESS_DENIED )
		{
			_Evt.m_nURLDetectType = errCheck == errACCESS_DENIED ? cUrlFltReportEvent::eDtBlackList : cUrlFltReportEvent::eDtWhiteList;
			PR_TRACE((this, prtIMPORTANT, "urlflt\tUrlFlt::AnalyzeObject: URL=[%S] is [%s] by blackwhite lists or invalid user profile assigned.", pUrlFlt->m_sURL.data(), errCheck == errOK_DECIDED ? "allowed" : "denied"));
		}
	}

	if( errCheck != errOK )
		bAskAction = false;
	else
	{
		// See ProcessObject for details
		/*
		CParctlDb::Result _dbRes;
		*/
		
		// Check URL by database black\white URL's masks
		if( !bHeuristicOnly )
		{
			//cAutoCS _lock(m_hCS, false);

			PR_TRACE((this, prtNOTIFY, 
				"urlflt\tUrlFlt::AnalyzeObject: (2) Check URL by database's white and black lists: start"));

			{
				/*
				cAutoCS _lock( m_dbCS, false );
				errCheck = m_db.CheckUrl(url, _dbRes);
				*/
				CParctlDb::SearchResult searchRes( nDenyCategoriesMask, info );
				errCheck = m_dbManager.CheckUrl( url, searchRes );
				_dbRes = searchRes.m_res;
			}

			DUMP_DB_RES( ( hOBJECT ) this, _dbRes );

			PR_TRACE((this, prtNOTIFY, 
				"urlflt\tUrlFlt::AnalyzeObject: (2) Check URL by database's white and black lists: end, %terr", 
				errCheck));

			if( errCheck == errOK_DECIDED || errCheck == errACCESS_DENIED )
				_Evt.m_nURLDetectType = cUrlFltReportEvent::eDtDatabase;
			
			/*
			// For later calculate _dbRes
			if( errCheck == errACCESS_DENIED && _dbRes.size() )
				errCheck = errOK;
			*/
			if ( errCheck == errACCESS_DENIED && !_dbRes.StopAnalyzing( nDenyCategoriesMask  ) )
			{
				// Continue analyzing
				errCheck = errOK;
			}

			if( errCheck == errOK_DECIDED )
				PR_TRACE((this, prtIMPORTANT, 
					"urlflt\tUrlFlt::AnalyzeObject: URL=[%S] is allowed by database white URL's masks.", 
					pUrlFlt->m_sURL.data() ));
		}

		if( errCheck == errOK )
		{
			// Check URL by heuristic
			bool bDeny = false;
			// Continue checking only when there are appropriate settings checked and 
			// there is not enough results from previous step to make a decision
			/*
			if( nDenyCategoriesMask && bHeuristic && !stopAnalyzing( _dbRes, nDenyCategoriesMask ) )
			*/

			PR_ASSERT( !_dbRes.StopAnalyzing( nDenyCategoriesMask ) );

			if ( bHeuristic )
			{
				// Get site and full variants of the URL
				cStrObj strSiteURL;
				getSiteURL( url, strSiteURL );
				cStrObj strFullURL;
				getFullURL( url, strFullURL );

				tERROR errHeuristic = errOK;
				tERROR errHeurImportance = errOK;

				{
					// See findInCache for details
					/*
					// Check URL by cache
					tHeurObj * pHeurObj = NULL;
					eCacheRes nCacheRes = eCrProcess;
					*/

					//PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::ProcessObject: (3) Check URL in cache: start"));
					{
/*
						cAutoCS _lock(m_hCS, true);

						tDWORD i, n;

						// Remove TTL exceeded cache items
						{
							for(i = m_cache.size(); i > 0; i--)
							{
								tCasheObj& _c = m_cache[i - 1];
								if( (tDWORD)(nNow - _c.m_time) <= m_nCacheTTL )
									break;
							}

							m_cache.remove(i, m_cache.size());
						}
*/

						// See findInCache for details
						/*
						// Ищем сайт в кеше
						{
							cAutoCS lock( m_cacheCS, false );
							for( int i = 0, n = m_cache.size(); i < n; ++i )
							{
								tCasheObj& _c = m_cache[i];
								// See comments about cache usage below
								//if( strSiteURL.compare(0, cStrObj::whole, _c.m_url.c_str(), fSTRING_COMPARE_CASE_INSENSITIVE) )
								if( strFullURL.compare( 0, cStrObj::whole, _c.m_url.c_str(), fSTRING_COMPARE_CASE_INSENSITIVE ) != 0 )
									continue;

								// Perform cleaning procedure when idle message is received
								/*
								// Закешированные ОШИБОЧНЫЕ статусы используем не более m_nCacheErrTTL секунд !!!
								if( _c.m_err && ((tDWORD)(nNow - _c.m_time) > m_nCacheErrTTL) )
								{
									m_cache.remove(i);
									break;
								}
								*
								
								_dbRes = _c.m_dbRes;
								nCacheRes = eCrFound;

								PR_TRACE((this, prtNOTIFY, 
									"urlflt\tUrlFlt::ProcessObject: Cache: site found: [%s] (cache index = %d, cache size = %d)",
									_c.m_url.c_str(), i, ( int ) m_cache.size() ));

								// Для ОШИБОЧНЫХ статусов НЕ нужно модифицировать время последнего обращения !!!
								if( _c.m_err )
								{
									bDeny = _c.m_err_deny;
									break;
								}

								if( i )
								{
									tCasheObj _t = _c;
									_t.m_time = nNow;
									m_cache.remove(i, i);
									m_cache.insert(0) = _t;
								}

								break;
							}
						}
						*/
					}

					//PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::ProcessObject: (3) Check URL in cache: end"));

					// See findInCache for details
					/*
						// Ищем сайт в текущих анализируемых сайтах
						if( nCacheRes == eCrProcess )
						{
							// Search-insert should be processed as single operation
							cAutoCS lock( m_heurObjsCS, false );

							//int n;
							for(int i = 0, n = m_heurObjs.size(); i < n; i++)
							{
								tHeurObj * _h = m_heurObjs[i];
								//if( !strSiteURL.compare(0, cStrObj::whole, _h->m_url.c_str(), fSTRING_COMPARE_CASE_INSENSITIVE) )
								if( !strFullURL.compare(0, cStrObj::whole, _h->m_url.c_str(), fSTRING_COMPARE_CASE_INSENSITIVE) )
								{
									nCacheRes = eCrProcessWait;
									pHeurObj = _h;
									pHeurObj->addRef();
									break;
								}
							}

							if( !pHeurObj )
							{
								pHeurObj = tHeurObj::_new();
								if( pHeurObj )
								{
									//pHeurObj->m_url = (tCHAR *)strSiteURL.c_str(cCP_ANSI);
									pHeurObj->m_url = (tCHAR *)strFullURL.c_str(cCP_ANSI);
									m_heurObjs.push_back(pHeurObj);
								}
								else
									errHeuristic = errNOT_ENOUGH_MEMORY;
							}
						}
					*/

					if( PR_SUCC(errHeuristic) )
					{
						// See findInCache for details
						//if( nCacheRes == eCrProcess )
						{
							PR_TRACE((this, prtIMPORTANT, 
								"urlflt\tAnalyzeObject: URL=[%S]: Heuristic analyze begin...", 
								pUrlFlt->m_sURL.data() ));

							PR_TRACE((this, prtNOTIFY, 
								"urlflt\tUrlFlt::AnalyzeObject: infoResponse = %d, no cache = %d",
								( int ) infoResponse, ( int ) checkForNoCache( pUrlFlt ) ));

							const int contentType = pUrlFlt->m_ContentType;
							if ( !infoResponse )
							{
								PR_TRACE((this, prtNOTIFY, 
									"urlflt\tUrlFlt::AnalyzeObject: (4) Check content with heuristic: start"));

								CParctlDb::SearchResult searchRes( nDenyCategoriesMask, info );
								errHeuristic = _HeuristicAnalyze
									(
										strSiteURL, 
										content, 
										*pUrlFlt, 
										stHeurPrm, 
										//hToken, 
										searchRes
									);
								_dbRes = searchRes.m_res;

								PR_TRACE((this, prtNOTIFY, 
									"urlflt\tUrlFlt::AnalyzeObject: (4) Check content with heuristic: end, %ld", 
									errHeuristic));
							}
							else
							{
								errHeuristic = errOK;
							}

							errHeurImportance = _GetHeuristicErrorImportance(errHeuristic);
							
							PR_TRACE((this, prtIMPORTANT, 
								"urlflt\tAnalyzeObject: URL=[%S]: Heuristic analyze end.", 
								pUrlFlt->m_sURL.data()));

							//bDeny = errHeurImportance != errOK;
							
							// See addToCache for details
							/*
							{
								cAutoCS _lock(m_hCS, true);
								
								// Add analyzed URL in cache
								// Не запоминаем результат эвристики при ошибке авторизации, что бы диалог авторизации показывался снова
								if
									( 
										( errHeuristic != PR_MAKE_DECL_ERR(IID_UPDATERCONFIGURATOR, KLUPD::CORE_PROXY_AUTH_ERROR) )
										&&
										storeInCache
									)
								{
									cAutoCS lock( m_cacheCS, true );

									if( m_cache.size() >= m_nCache )
										m_cache.resize(m_nCache ? (m_nCache - 1) : 0);

									tCasheObj& _c = m_cache.insert(0);

									// Note: we have to put the full URL to the cache.
									// Otherwise, the result of resolving the access
									// on the same domain name (with cache option enabled)
									// will depend on the order of visiting pages.
									// For instance: domain/site1 - good, domain/site2 - bad
									// If we visit domain/site1, then it will be 
									// always possible to visit domain/site2, 
									// and vice versa.
									//_c.m_url = (tCHAR *)strSiteURL.c_str(cCP_ANSI);
									_c.m_url = ( tCHAR* ) strFullURL.c_str( cCP_ANSI );

									_c.m_dbRes = _dbRes;
									_c.m_time = nNow;
									_c.m_err = PR_FAIL(errHeuristic);
									_c.m_err_deny = bDeny;

									PR_TRACE((this, prtNOTIFY, 
										"urlflt\tUrlFlt::ProcessObject: Cache: site added: [%s] (cache size = %d)", 
										_c.m_url.c_str(), ( int ) m_cache.size() ));
								}

								{
									cAutoCS lock( m_heurObjsCS, true );

									tDWORD i, n;
									for(i = 0, n = m_heurObjs.size(); i < n; i++)
									{
										if( m_heurObjs[i] == pHeurObj )
										{
											m_heurObjs.remove(i);
											break;
										}
									}
								}

								if( errHeuristic == errOK )
									m_fHeuristicSuccess = 1;
							}
							
                            if( errHeuristic == errOK )
                            {
							    pHeurObj->m_dbRes = _dbRes;
							    pHeurObj->m_deny = bDeny;
							    pHeurObj->m_complete = 1;
                            }
							*/
						}
						// See findInCache for details
						/*
						else if( nCacheRes == eCrProcessWait )
						{
							while( !pHeurObj->m_complete )
								PrSleep(100);

							_dbRes = pHeurObj->m_dbRes;
							bDeny = pHeurObj->m_deny;
						}
						*/
					}

					// See findInCache for details
					/*
					if( pHeurObj )
						pHeurObj->release();
					*/
				}

				if( _dbRes.size() )
					_Evt.m_nURLDetectType = cUrlFltReportEvent::eDtHeuristic;

				if( errHeuristic != errOK )
				{
					cUrlFltReportEvent _EvtHeuristic(cUrlFltReportEvent::eHeuristicStatus);
					_EvtHeuristic.m_ErrorCode		= errHeuristic;
					//_EvtHeuristic.m_strDetectObject	= strSiteURL;
					_EvtHeuristic.m_strDetectObject	= strFullURL;
					_EvtHeuristic.m_tmTimeStamp		= _Evt.m_tmTimeStamp;
					_EvtHeuristic.m_strUserId		= _Evt.m_strUserId;
					_EvtHeuristic.m_strUfProfile	= _Evt.m_strUfProfile;
					_EvtHeuristic.m_nUfProfileId	= _Evt.m_nUfProfileId;

					bool bHeuristicSuccess;
					{
						cAutoCS _lock(m_hCS, true);
						bHeuristicSuccess = m_fHeuristicSuccess;
					}
					
					if( errHeurImportance == errNOT_OK )
						_EvtHeuristic.m_nSeverity = pmc_EVENTS_CRITICAL;
					else if( bHeuristicSuccess )
					{
						if( errHeurImportance == warnFALSE )
							_EvtHeuristic.m_nSeverity = pmc_EVENTS_IMPORTANT;
						else
							_EvtHeuristic.m_nSeverity = pmc_EVENTS_NOTIFY;
					}
					else
						_EvtHeuristic.m_nSeverity = pmc_EVENTS_IMPORTANT;
					
					sysSendMsg(_EvtHeuristic.m_nSeverity, 0, NULL, &_EvtHeuristic, SER_SENDMSG_PSIZE);
				}
			}

			// See ProcessObject for details
			/*
			if( bDeny )
				errCheck = errACCESS_DENIED;
			else
			{
				// Check result catgrange with filter categories settings
				for(tDWORD i = 0, n = _dbRes.size(); i < n; i++)
				{
					CParctlDb::ResultCatg& _c = _dbRes[i];

					PR_TRACE((this, prtNOTIFY, 
						"urlflt\tUrlFlt::ProcessObject: category: id = %d, weight = %d", 
						( int )_c.m_id, ( int )_c.m_weight));

					if( !(nDenyCategoriesMask & _get_catg_bit(_c.m_id)) )
						continue;

					if ( stopAnalyzingForCat( _c ) )
					{
						errCheck = errACCESS_DENIED;
						break;
					}
				}
			}

			if( errCheck == errOK )
				_Evt.m_nURLDetectType = cUrlFltReportEvent::eDtNone;
			*/
		}
	}

	// See ProcessObject for details
	/*
	// Fill statistics
	updateStats( errCheck, _Evt, _profile );

	if( _profile )
		_profile->release();

	if( errCheck != errACCESS_DENIED )
		_Evt.m_nDetectType = DETYPE_UNKNOWN;
	
	if( !bDenyDetected &&
		(_Evt.m_nURLDetectType != cUrlFltReportEvent::eDtTimeLimit) &&
		errCheck != errOK )
	{
		errCheck = errOK_DECIDED;
	}
	
	if( bAskAction && errCheck == errACCESS_DENIED )
	{
		cAskObjectAction _AskAction; _AskAction.assign(_Evt, false);
		_AskAction.m_nResultAction = _AskAction.m_nDefaultAction;

		PR_TRACE((this, prtNOTIFY, "urlflt\tAskAction: URL=[%S], user request...", pUrlFlt->m_sURL.data()));
		err = sysSendMsg(pmcASK_ACTION, cAskObjectAction::DISINFECT, NULL, &_AskAction, SER_SENDMSG_PSIZE);
		PR_TRACE((this, prtNOTIFY, "urlflt\tAskAction: URL=[%S], user request deny=%d", pUrlFlt->m_sURL.data(), _AskAction.m_nResultAction == ACTION_DENY));

		errCheck = _AskAction.m_nResultAction == ACTION_DENY ? errACCESS_DENIED : errOK_DECIDED;
	}

	PR_TRACE((this, prtIMPORTANT, "urlflt\tProcessObject: URL=[%S]: Result: [%s].", pUrlFlt->m_sURL.data(), errCheck == errACCESS_DENIED ? "denied" : "allowed"));
	
	// report
	if( errCheck == errACCESS_DENIED )
	{
		_Evt.m_nResultAction = ACTION_DENY;
		_Evt.m_nObjectStatus = OBJSTATUS_SUSPICION;
	}
	else
	{
		_Evt.m_nResultAction = ACTION_ALLOW;
	}

	//_Evt.m_nSeverity = (_Evt.m_nURLDetectType != cUrlFltReportEvent::eDtNone && _Evt.m_nURLDetectType != cUrlFltReportEvent::eDtWhiteList) ? (errCheck == errACCESS_DENIED ? pmc_EVENTS_CRITICAL : pmc_EVENTS_IMPORTANT) : pmc_EVENTS_NOTIFY;
	_Evt.m_nSeverity = GetSeverity( errCheck, _Evt );
	sysSendMsg( _Evt.m_nSeverity, 0, NULL, &_Evt, SER_SENDMSG_PSIZE );
	*/

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::AnalyzeObject: end"));

	return errCheck;
}

///
/// Perform processing of the given content
///
tERROR UrlFlt::DoProcessing
	(
		const cUrlFilteringRequest* pUrlFlt, 
		hIO content,
		const tProfile* _profile,
		const tQWORD& nDenyCategoriesMask,
		cToken* pToken,
		cUrlFltReportEvent& _Evt
	)
{
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::DoProcessing: start"));

	tERROR errCheck = errOK;

	// Primary content's results
	CParctlDb::Result _dbRes;

	cStrBuff buf;
	CUrlListCache::tURL url;
	normalizeUrl( pUrlFlt->m_sURL, buf, url, true );

	// Check if the same request is about being processed; if so, try
	// waiting until it is ready. If it was processed correctly, then we will 
	// find its results in cache. If not, we will process it ourself
	bool addedToProc = false;
	checkInProc( pUrlFlt->m_sURL );

	eCacheRes found = findInCache( pUrlFlt->m_sURL, _Evt, errCheck, _dbRes, true );

	switch ( found )
	{
		case eCacheNone:
			{
				addToProc( pUrlFlt->m_sURL );
				addedToProc = true;

				errCheck = AnalyzeObject
					( 
						pUrlFlt, 
						content, 
						_profile, 
						nDenyCategoriesMask,
						url,
						_Evt, 
						_dbRes
					);
			}
			break;

		case eCacheNotAnalyzed:
			{
				// If the item is found in cache but it wasn't analyzed,
				// we shouldn't use the results
				errCheck = errFOUND;
			}
			break;

		case eCacheFound:
			break; // Nothing

		default:
			PR_ASSERT( false );
	}

	if ( errCheck == errOK )
	{
		errCheck = checkDbRes( _dbRes, nDenyCategoriesMask );

		if( errCheck == errOK )
			_Evt.m_nURLDetectType = cUrlFltReportEvent::eDtNone;
	}

	// Check for main page
	if ( errCheck == errOK )
	{
		bool checkMainPage = false;
		{
			cAutoCS _lock( m_hCS, false );
			checkMainPage = m_fCheckMainPage;
		}

		if ( checkMainPage )
		{
			cUrlFilteringRequest mainUrlFlt = *pUrlFlt;
			getSiteURL( url, mainUrlFlt.m_sURL, true );
			normalizeUrl( mainUrlFlt.m_sURL, buf, url, false );

			if ( mainUrlFlt.m_sURL != pUrlFlt->m_sURL )
			{
				CParctlDb::Result mainDbRes;
				tERROR err = errOK;

				// Check if the same request is about being processed; if so, try
				// waiting until it is ready. If it was processed correctly, then we will 
				// find its results in cache. If not, we will process it ourself
				checkInProc( mainUrlFlt.m_sURL );

				const eCacheRes found = findInCache
					( 
						mainUrlFlt.m_sURL, 
						_Evt, 
						errCheck, 
						mainDbRes, 
						true 
					);

				switch ( found )
				{
					case eCacheNone:
						{
							// Mark as being currently processed
							addToProc( mainUrlFlt.m_sURL );

							hIO mainPageContent = NULL;
							err = loadPage( mainUrlFlt, &mainPageContent, pToken );
							if ( PR_SUCC( err ) )
							{
								errCheck = AnalyzeObject
									( 
										&mainUrlFlt, 
										mainPageContent, 
										_profile, 
										nDenyCategoriesMask,
										url,
										_Evt, 
										mainDbRes
									);

								const bool noCache = checkForNoCache( &mainUrlFlt );
								if ( !noCache && !isContentEmpty( mainPageContent ) )
								{
									addToCache( mainUrlFlt.m_sURL, _Evt, errCheck, mainDbRes, true );
								}

								PR_ASSERT( mainPageContent != NULL );
								if ( mainPageContent != NULL )
								{
									CALL_SYS_ObjectClose( mainPageContent );
									mainPageContent = NULL;
								}
							}
							else
							{
								PR_TRACE((this, prtIMPORTANT, 
									"urlflt\tDoProcessing: Can't load page = [%S]", 
									mainUrlFlt.m_sURL.data() ));
								//zzz !!! send results to report !!!

								addToCache( mainUrlFlt.m_sURL, _Evt, errCheck, mainDbRes, false );
							}

							// Mark as processed
							removeFromProc( mainUrlFlt.m_sURL );
						}
						break;

					case eCacheNotAnalyzed:
						{
							// If the item is found in cache but it wasn't analyzed,
							// we shouldn't use the results
							err = errFOUND;
						}
						break;

					case eCacheFound:
						break; // Nothing

					default:
						PR_ASSERT( false );
				}

				// Join results only in case when the page was loaded successfully
				if ( PR_SUCC( err ) )
				{
					bestDbRes( mainDbRes, _dbRes );
					errCheck = checkDbRes( _dbRes, nDenyCategoriesMask );

					if( errCheck == errOK )
						_Evt.m_nURLDetectType = cUrlFltReportEvent::eDtNone;
				}
			}
		}
	}

	// Add primary results to the cache
	if ( found == eCacheNone )
	{
		const bool noCache = checkForNoCache( pUrlFlt );
		if ( !noCache && !isContentEmpty( content ) )
		{
			addToCache( pUrlFlt->m_sURL, _Evt, errCheck, _dbRes, true );
		}
	}

	if ( addedToProc )
	{
		// Mark as processed
		removeFromProc( pUrlFlt->m_sURL );
	}

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::DoProcessing: end"));

	return errCheck;
}

tERROR UrlFlt::ProcessObject( cUrlFilteringRequest* pUrlFlt, hIO content )
{
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::ProcessObject: start"));

	if ( !pUrlFlt )
		return errPARAMETER_INVALID;

	cUrlFltReportEvent _Evt( cUrlFltReportEvent::eProcess );
	tERROR errCheck = errOK;
	cStrObj strUser;

	tProfile* _profile = NULL;
	bool bAskAction = false;
	tQWORD nDenyCategoriesMask = 0;
	bool bDenyDetected = true;

	cAutoObj<cToken> hToken;

	// Get user profile
	{
		cAutoCS _lock(m_hCS, false);

		const bool bProfiles = m_fProfiles;
		bAskAction = m_fAskAction;

		_Evt.assign(m_AskActionDef, false);

		if( bProfiles )
		{
			// For optimizing !!!!!!!!!!!!! cProcMonNotify pmc_PROCESS_MONITOR, pm_PROCESS_MONITOR_PROCESS_EXIT

			tDWORD nType = 0;
			const tERROR err = _GetUserByPID(pUrlFlt->m_nPID, hToken, strUser, nType);
			if( PR_FAIL(err) )
			{
				PR_TRACE((this, prtERROR, "urlflt\tProcessObject: GetUserByPID error: %terr", err));
			}
			else if( nType != TOKEN_TYPE_USER )
			{
				PR_TRACE((this, prtIMPORTANT, 
					"urlflt\tProcessObject: PID = %u, User = [%S]: is not a user account type = %u, skipped", 
					pUrlFlt->m_nPID, strUser.data(), nType ));
				return errOK;
			}
		}

		if( bProfiles )
		{
			_profile = _GetUserProfile(strUser);
			if( !_profile )
				_profile = _GetProfile(m_defUser.m_nProfileId);

			if( _profile )
			{
				_profile->addRef();
				_Evt.m_strUfProfile = _profile->m_strName;
				_Evt.m_nUfProfileId = _profile->m_nId;
			}
			else
			{
				PR_TRACE((this, prtIMPORTANT, "urlflt\tProcessObject: can't find (or disabled) profile for user [%S], deny request.", strUser.data()));
				errCheck = errACCESS_DENIED;
			}
		}

		if ( _profile != NULL )
		{
			if( _profile->m_bUseCommonFilter )
				nDenyCategoriesMask |= m_common.m_nDenyCategoriesMask;
			nDenyCategoriesMask |= _profile->m_nDenyCategoriesMask;

			if( !_profile->m_bDenyDetected )
				bDenyDetected = false;
		}
		else
		{
			nDenyCategoriesMask |= m_common.m_nDenyCategoriesMask;
		}
	}

	const time_t nNow = cDateTime::now_utc();
	_Evt.m_tmTimeStamp		= (__time32_t)nNow;
	_Evt.m_nPID             = pUrlFlt->m_nPID;
	_Evt.m_strUserId		= strUser;
#if	VER_PRODUCTVERSION_HIGH <= 7
	_Evt.m_strDetectObject	= pUrlFlt->m_sURL;
	_Evt.m_nDescription		= pUrlFlt->m_nPID;
#else
	_Evt.m_strObjectName	= pUrlFlt->m_sURL;
#endif	

	PR_TRACE((this, prtIMPORTANT, 
		"urlflt\tProcessObject: PID = %u, User = [%S], profile: [%S]", 
		pUrlFlt->m_nPID, strUser.data(), _profile->m_strName.data() ));

	if ( _profile->m_nId != PARCTL_PROFILEID_PARENT )
	{
		// Check time limits
		if( errCheck == errOK )
		{
			PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::ProcessObject: Time limit check start..."));

			cAutoCS _lock(m_hCS, true);
			if( _profile && !_CheckTimeLimit(_profile) )
			{
				errCheck = errACCESS_DENIED;
				_Evt.m_nURLDetectType = cUrlFltReportEvent::eDtTimeLimit;
				PR_TRACE((this, prtIMPORTANT, 
					"urlflt\tProcessObject: time limit detected (profile: [%S], "
					"cur day time: %u sec, day time limit: %u sec, day time space count: %u, deny request.", 
					_profile->m_strName.data(), _profile->m_nDayTime, 
					_profile->m_nDayTimeLimit, _profile->m_nDayTimeSpace.size() ));
			}

			PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::ProcessObject: Time limit check end..."));
		}

		/*
	#ifdef _DEBUG
		static bool started = false;
		if ( !started )
		{
			CUrlFltTestThr::_new( *this, *pUrlFlt, _profile, nDenyCategoriesMask, _Evt )->start( m_ThreadTest );
			started = true;
		}
	#endif // _DEBUG
		*/

		if ( errCheck == errOK )
		{
			errCheck = DoProcessing
				(
					pUrlFlt,
					content,
					_profile,
					nDenyCategoriesMask,
					hToken, 
					_Evt
				);
		}
	}

	// We think that object is processed when the given URL is blocked
	// or it is not an information response (i.e not an authorization request 
	// (401, 407), non-modified response (304), etc)
	const bool processed = 
		( errCheck == errACCESS_DENIED ) 
		|| 
		!checkForInfoResponse( pUrlFlt->m_StatusCode )
		;

	// Fill statistics
	// See comments below about sending the notifications
	// Apply the same logic for updating the statistics
	if ( processed )
	{
		updateStats( errCheck, _Evt, _profile );
	}

	if( _profile )
		_profile->release();

	if( errCheck != errACCESS_DENIED )
		_Evt.m_nDetectType = DETYPE_UNKNOWN;

	if( !bDenyDetected &&
		(_Evt.m_nURLDetectType != cUrlFltReportEvent::eDtTimeLimit) &&
		errCheck != errOK )
	{
		errCheck = errOK_DECIDED;
	}

	if( bAskAction && errCheck == errACCESS_DENIED )
	{
		cAskObjectAction _AskAction; _AskAction.assign(_Evt, false);
		_AskAction.m_nResultAction = _AskAction.m_nDefaultAction;

		PR_TRACE((this, prtNOTIFY, "urlflt\tAskAction: URL=[%S], user request...", pUrlFlt->m_sURL.data()));
		const tERROR err = sysSendMsg(pmcASK_ACTION, cAskObjectAction::DISINFECT, NULL, &_AskAction, SER_SENDMSG_PSIZE);
		PR_TRACE((this, prtNOTIFY, "urlflt\tAskAction: URL=[%S], user request deny=%d", pUrlFlt->m_sURL.data(), _AskAction.m_nResultAction == ACTION_DENY));

		errCheck = _AskAction.m_nResultAction == ACTION_DENY ? errACCESS_DENIED : errOK_DECIDED;
	}

	PR_TRACE((this, prtIMPORTANT, "urlflt\tProcessObject: URL=[%S]: Result: [%s].", pUrlFlt->m_sURL.data(), errCheck == errACCESS_DENIED ? "denied" : "allowed"));

	// report
	if( errCheck == errACCESS_DENIED )
	{
		_Evt.m_nResultAction = ACTION_DENY;
		_Evt.m_nObjectStatus = OBJSTATUS_SUSPICION;
	}
	else
	{
		_Evt.m_nResultAction = ACTION_ALLOW;
	}

	//_Evt.m_nSeverity = (_Evt.m_nURLDetectType != cUrlFltReportEvent::eDtNone && _Evt.m_nURLDetectType != cUrlFltReportEvent::eDtWhiteList) ? (errCheck == errACCESS_DENIED ? pmc_EVENTS_CRITICAL : pmc_EVENTS_IMPORTANT) : pmc_EVENTS_NOTIFY;
	_Evt.m_nSeverity = GetSeverity( errCheck, _Evt );

	// The notifications should be sent just once: we do that for all negative
	// notifications and for all positive ones (that are not are authorization
	// requests)
	if ( processed )
	{
		sysSendMsg( _Evt.m_nSeverity, 0, NULL, &_Evt, SER_SENDMSG_PSIZE );
	}

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::ProcessObject: end"));

	return errCheck;
}

bool UrlFlt::_CheckTimeLimit(tProfile * _profile)
{
	//PR_TRACE((this, prtNOT_IMPORTANT, "urlflt\tUrlFlt::_CheckTimeLimit: start"));

	PR_ASSERT( _profile != NULL );
	if ( _profile == NULL )
		return false;

	if ( !_profile->m_bUseDayTimeLimit && !_profile->m_bUseDayTimeSpace )
	{
		// Neither time limit nor time space are set
		return true;
	}

	if( _GetTz() != m_TimeZone )
	{
		PR_TRACE((this, prtNOTIFY, 
			"urlflt\tUrlFlt::_CheckTimeLimit: time zone: %u, %u",
			( unsigned int ) _GetTz(), ( unsigned int ) m_TimeZone ));
		return false;
	}
	
	time_t nNow = m_SystemStartTime + PrGetTickCount()/1000;
	time_t nNowLoc = _TimeToLocal(nNow);
		
//#ifdef _DEBUG
	{
		SYSTEMTIME sysTime;
		::ZeroMemory( &sysTime, sizeof( sysTime ) );
		_TimeToSystem( nNow, sysTime );

		PR_TRACE((this, prtNOTIFY, 
			"urlflt\tUrlFlt::_CheckTimeLimit: time: %d, %d, %d, %d, %d, %d",
			( int ) sysTime.wYear, ( int ) sysTime.wMonth, ( int ) sysTime.wDay, 
			( int ) sysTime.wHour, ( int ) sysTime.wMinute, ( int ) sysTime.wSecond ));
	}

	{
		SYSTEMTIME sysLocalTime;
		::ZeroMemory( &sysLocalTime, sizeof( sysLocalTime ) );
		_TimeToSystem( nNowLoc, sysLocalTime );

		PR_TRACE((this, prtNOTIFY, 
			"urlflt\tUrlFlt::_CheckTimeLimit: local time: %d, %d, %d, %d, %d, %d",
			( int ) sysLocalTime.wYear, ( int ) sysLocalTime.wMonth, ( int ) sysLocalTime.wDay, 
			( int ) sysLocalTime.wHour, ( int ) sysLocalTime.wMinute, ( int ) sysLocalTime.wSecond ));
	}
//#endif // _DEBUG

//	time_t nNowTest = (time_t)cDateTime::now_utc();
//	time_t nNowLocTest = (time_t)cDateTime::now_local();

	if ( _profile->m_bUseDayTimeSpace )
	{
		// Check day time space
		tDWORD i = 0;
		const tDWORD nSpaceTestTime = static_cast< tDWORD >( nNowLoc % ( 60 * 60 * 24 ) );
		const tDWORD dayTimeSpaceCount = _profile->m_nDayTimeSpace.size();
		for(; i < dayTimeSpaceCount; i++)
		{
			cUrlFltTimeRange& _r = _profile->m_nDayTimeSpace[i];

			PR_TRACE((this, prtNOTIFY, 
				"urlflt\tUrlFlt::_CheckTimeLimit: From: %d, To: %d, Test: %d",
				( int ) _r.m_nTimeFrom, ( int ) _r.m_nTimeTo, ( int ) nSpaceTestTime ));

			if( _r.m_nTimeFrom <= nSpaceTestTime && _r.m_nTimeTo > nSpaceTestTime )
				break;
		}

		// If we are not within one of the ranges, the request should be prohibited
		if( i == dayTimeSpaceCount )
			return false;
	}

	// Время "серфинга" считается путем прибавления разницы
	// текущего времени от времени прошлого запроса, но не более 5 минут.
	// Если сутки закончились, то счетчик сбрасывается.
	
	#define _TIMET_DAYS(_n) ((_n)/(60*60*24))

	if( nNow < _profile->m_LastQueryTime )
	{
		PR_TRACE((this, prtNOTIFY, 
			"urlflt\tUrlFlt::_CheckTimeLimit: last query: %u, now = %u",
			( unsigned int ) _profile->m_LastQueryTime, ( unsigned int ) nNow ));
		return false;
	}
	
	// Check day time limit
	if ( _profile->m_bUseDayTimeLimit )
	{
		const time_t nLastQueryLoc = _TimeToLocal(_profile->m_LastQueryTime);
		if( _TIMET_DAYS(nNowLoc) == _TIMET_DAYS(nLastQueryLoc) )
		{
			time_t nTimeUsageFromLastQuery = nNow - _profile->m_LastQueryTime;
			if( nTimeUsageFromLastQuery > 5*60 )
				nTimeUsageFromLastQuery = 5*60;
			_profile->m_nDayTime += (tDWORD)nTimeUsageFromLastQuery;
		}
		else
			_profile->m_nDayTime = 0;
	}

	_profile->m_LastQueryTime = nNow;

	//PR_TRACE((this, prtNOT_IMPORTANT, "urlflt\tUrlFlt::_CheckTimeLimit: end"));

	return _profile->m_nDayTimeLimit && (_profile->m_nDayTime <= _profile->m_nDayTimeLimit);
}

///
/// Update counter for the given result categories
///
static inline
void _updateStats( const CParctlDb::Result& dbRes, std::vector< int >& badLinks )
{
	for ( int i = 0, n = ( int )dbRes.size(); i < n; ++i )
	{
		const CParctlDb::ResultCatg& cat = dbRes[ i ];
		PR_ASSERT( cat.m_id >= 0 && cat.m_id < badLinks.size() );
		++badLinks[ cat.m_id ];
	}
}

///
/// Perform analyzing of the given description words
///
void UrlFlt::analyzeWords
	( 
		const std::vector< std::wstring >& descriptionWords,
		CParctlDb::SearchResult& searchRes
		/*
		const tQWORD& denyCategoriesMask,
		CParctlDb::Result& dbRes,
		ReportInfo& info
		*/
	)
{
	// Note: the call should be under protection of db' critical section

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::analyzeWords: start"));

	// Currently the results for links will be accumulated there;
	// Afterwards they will be joined with the main results
	CParctlDb::Result dbWordsRes;
	// Calculate links for each category
	std::vector< int > badLinks( eUrlFltCategoryCount );

	const int wordCount = descriptionWords.size();
	for ( int i = 0; i < wordCount; ++i )
	{
		const std::wstring& word = descriptionWords[ i ];

		PR_TRACE((this, prtNOTIFY, 
			"urlflt\tUrlFlt::analyzeWords: Word (%d): [%S]",
			( int ) i, word.c_str() ));

		if ( !word.empty() )
		{
			//CParctlDb::Result wordRes;
			//m_db.AnalyzeData( word.c_str(), word.length(), wordRes );
			CParctlDb::SearchResult wordSearchRes( searchRes.m_denyCategoriesMask, searchRes.m_info );
			m_dbManager.AnalyzeData
				( 
					word.c_str(), 
					word.length(), 
					wordSearchRes
				);

			if ( !wordSearchRes.m_res.empty() )
			{
				dbWordsRes.AppendAccumulate( wordSearchRes.m_res );

				// Add counter for the appropriate link's group
				_updateStats( wordSearchRes.m_res, badLinks );

				PR_TRACE((this, prtNOTIFY, 
					"urlflt\tUrlFlt::analyzeWords: results:"));
				DUMP_DB_RES( ( hOBJECT ) this, wordSearchRes.m_res );
			}
		}
	}

	// Currently we have two different results: join them together
	if ( !dbWordsRes.empty() )
	{
		joinDBResults( dbWordsRes, badLinks, searchRes.m_res, wordCount, searchRes.m_info );
	}

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::analyzeWords: end"));
}

///
/// Join the currently accumulated results with the main results
///
/// @param dbSubRes - currently accumulated results
/// @param stats - statistics for currently accumulated results
/// @param dbRes - main results
/// @param totalCount - total count of processed objects to accumulate results
///
void UrlFlt::joinDBResults
	( 
		CParctlDb::Result& dbSubRes, 
		const std::vector< int >& stats, 
		CParctlDb::Result& dbRes,
		const int totalCount,
		ReportInfo& info
	)
{
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::joinDBResults: start"));

	// Calculate total count of records in the statistics
	const int totalSubCount = std::accumulate( stats.begin(), stats.end(), 0 );

	// (1) Recalculate all weights as average per link
	PR_TRACE((this, prtNOTIFY, 
		"urlflt\tUrlFlt::joinDBResults: total subcount = %d, total count = %d",
		( int ) totalSubCount, ( int ) totalCount ));

	PR_TRACE((this, prtNOTIFY, 
		"urlflt\tUrlFlt::joinDBResults: Sub results:"));
	DUMP_DB_RES( ( hOBJECT ) this, dbSubRes );

	// Report sub results
	if ( info.IsEnabled() && !dbRes.empty() )
	{
		std::string str;
		CParctlDb::Result::convertResultsToString( dbSubRes, str );
		info.Trace( "Sub results: %s", str.c_str() );
	}

	PR_TRACE((this, prtNOTIFY, 
		"urlflt\tUrlFlt::joinDBResults: Main results:"));
	DUMP_DB_RES( ( hOBJECT ) this, dbRes );

	// Report main results
	if ( info.IsEnabled() && !dbRes.empty() )
	{
		std::string str;
		CParctlDb::Result::convertResultsToString( dbRes, str );
		info.Trace( "Main results: %s", str.c_str() );
	}

	if ( totalSubCount > 0 )
	{
		// Report
		info.Trace( "= Calculate mean sub-results..." );

		// Scan all the results and get the average before joining
		for( tDWORD i = 0, n = dbSubRes.size(); i < n; ++i )
		{
			CParctlDb::ResultCatg& subCtg = dbSubRes[ i ];

			PR_ASSERT( subCtg.m_id >= 0 && subCtg.m_id < stats.size() );
			const int catCount = stats[ subCtg.m_id ];
			//const int catAvg = subCtg.m_weight / catCount;
			const int catWeight = subCtg.m_weight;

			// We have to determine which value should be used as denominator
			// to get the average. Currently we try to estimate "how many" 
			// "bad" results we have for each category: if there are
			// "too many" bad results, we force getting bad results. If there are
			// not so "many" bad results, the average probably will be low.
			const int percCount = catCount * 100 / totalCount;

			int denom = 0;
			if ( 
					( ( percCount >= 0 ) && ( percCount <= 5 ) ) 
					&& 
					( ( catWeight >= 0 ) && ( catWeight < 500 ) )
				) // few
			{
				// We won't ban bad results if there are not too many of them
				denom = totalCount;
			}
			else if ( ( percCount > 50 ) || ( catWeight > 1000 ) ) // many
			{
				// We will ban bad results if there are too many of them
				denom = 1;
			}
			else // neutral
			{
				denom = catCount;
			}
			
			PR_TRACE((this, prtNOTIFY, 
				"urlflt\tUrlFlt::joinDBResults: category = %d, weight = %d, count = %d => denom = %d", 
				( int ) subCtg.m_id, ( int ) subCtg.m_weight, ( int ) catCount, ( int ) denom ));

			// Report calculations
			if ( info.IsEnabled() )
			{
				info.Trace
					( 
						"= category = %d, weight = %d, count = %d => denom = %d",
						( int ) subCtg.m_id, 
						( int ) subCtg.m_weight, 
						( int ) catCount, 
						( int ) denom
					);
			}

			PR_ASSERT( denom > 0 );
			subCtg.m_weight /= denom;

			if ( subCtg.m_weight > 100 )
			{
				subCtg.m_weight = 100;
			}
		}

		// Report sub-results again
		if ( info.IsEnabled() && !dbRes.empty() )
		{
			std::string str;
			CParctlDb::Result::convertResultsToString( dbSubRes, str );
			info.Trace( "Mean sub results: %s", str.c_str() );
		}

		// (2) Join to the main results
		if ( !dbSubRes.empty() )
		{
			PR_TRACE((this, prtNOTIFY, 
				"urlflt\tUrlFlt::joinDBResults: Sub results:"));
			DUMP_DB_RES( ( hOBJECT ) this, dbSubRes );

			dbRes.AppendAccumulate( dbSubRes );

			PR_TRACE((this, prtNOTIFY, 
				"urlflt\tUrlFlt::joinDBResults: Main results:"));
			DUMP_DB_RES( ( hOBJECT ) this, dbRes );
		}
	}

	// Report main results after joining
	if ( info.IsEnabled() && !dbRes.empty() )
	{
		std::string str;
		CParctlDb::Result::convertResultsToString( dbRes, str );
		info.Trace( "Joined main results: %s", str.c_str() );
	}

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::joinDBResults: end"));
}

///
/// Perform heuristic analyze
///
tERROR UrlFlt::_HeuristicAnalyze
	(
		const cStrObj& strSiteURL, 
		hIO content, 
		const cUrlFilteringRequest& request,
		tHeurPrm& _Prm, 
		//cToken * pToken, 
		CParctlDb::SearchResult& searchRes
	)
{
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_HeuristicAnalyze: start"));

	_SiteData _analyzeData( *this );

#ifdef _DEBUG
	{
		tERROR err = 0;
		tQWORD qwSize = 0;
		err = content->GetSize( &qwSize, 0 );
		if ( PR_SUCC( err ) )
		{
			PR_TRACE((this, prtIMPORTANT, 
				"urlflt\tUrlFlt::_HeuristicAnalyze: content size = %d", 
				( int )qwSize));
		}
	}
#endif // _DEBUG

    if( PR_FAIL(_analyzeData.Set(content)) )
	{
		PR_TRACE((this, prtIMPORTANT, 
			"urlflt\tUrlFlt::_HeuristicAnalyze: Can't set data for [%S]", 
			strSiteURL.data()));

        return warnFALSE;
	}

	if( _analyzeData.IsEmpty() )
	{
		PR_TRACE((this, prtIMPORTANT, 
			"urlflt\tUrlFlt::_HeuristicAnalyze: Data is empty for [%S]", 
			strSiteURL.data()));

		return warnFALSE;
	}

	// Check if the content is encoded (e.q. compressed) and decode it
	const http::content_encoding contentEncodig = 
		static_cast< http::content_encoding >( request.m_ContentEncoding );
	if ( PR_FAIL( _analyzeData.checkForEncodedContent( ( hOBJECT ) this, contentEncodig ) ) )
	{
		PR_TRACE((this, prtIMPORTANT, 
			"urlflt\tUrlFlt::_HeuristicAnalyze: Can't decompress encoded data: %d", 
			( int ) contentEncodig ));
		//zzz !!! send results to report !!!
	}

	if( _analyzeData.IsEmpty() )
	{
		PR_TRACE((this, prtIMPORTANT, 
			"urlflt\tUrlFlt::_HeuristicAnalyze: Data is empty for [%S]", 
			strSiteURL.data()));

		return warnFALSE;
	}

#ifdef _DEBUG
	// Show first n-bytes of the given content
	if ( !_analyzeData.IsEmpty() )
	{
		cStringObj debugStr;
		//debugStr.resize( str.size() );
		for ( int i = 0, n = _MIN( ( int ) _analyzeData.m_buf.size(), 60 ); i < n; ++i )
		//for ( int i = 0, n = ( int ) _analyzeData.m_buf.size(); i < n; ++i )
		{
			tCHAR ch = _analyzeData.m_buf.at( i );
			const bool nonPrintableChar = ( ch >= 0 && ch <= 30 );

			if ( nonPrintableChar )
			{
				ch = '.';
			}

			debugStr += ch;
		}
		PR_TRACE((this, prtNOTIFY, 
			"UrlFlt\t_HeuristicAnalyze: Response given:\n[%S]", 
			debugStr.data()));
	}
#endif // _DEBUG

	// Check if it is binary data
	const http::content_type contentType = 
		static_cast< http::content_type >( request.m_ContentType );
	if ( _analyzeData.checkForBinaryContent( contentType ) )
	{
		PR_TRACE((this, prtNOTIFY, 
			"UrlFlt\t_HeuristicAnalyze: Binary data: skipped"));
		return errOK;
	}

	/*
	cStringObj str;
	str.assign(&_analyzeData.m_buf.front(), cCP_ANSI, _analyzeData.m_buf.size() );
	*/

    _analyzeData.Prepare();

	// Process keywords
	if( _Prm.m_fHeuristicContentDescr )
	{
		PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_HeuristicAnalyze: Words analyze start"));

		PR_TRACE((this, prtNOTIFY, 
			"urlflt\tUrlFlt::_HeuristicAnalyze: Description meta tag"));

		// Description
		{
			std::vector< std::wstring > words;
			_analyzeData.processDescriptionTag( words );

			if ( !words.empty() )
			{
				//cAutoCS _lock( m_dbCS, false );
				searchRes.m_info.Trace( "Analyze description tag..." );
				analyzeWords( words, searchRes );

				//DUMP_DB_RES( ( hOBJECT ) this, _dbRes );
			}
		}

		PR_TRACE((this, prtNOTIFY, 
			"urlflt\tUrlFlt::_HeuristicAnalyze: Classification meta tag"));

		// Classification
		{
			std::vector< std::wstring > words;
			_analyzeData.processClassificationTag( words );

			if ( !words.empty() )
			{
				//cAutoCS _lock( m_dbCS, false );
				searchRes.m_info.Trace( "Analyze classification tag..." );
				analyzeWords( words, searchRes );

				//DUMP_DB_RES( ( hOBJECT ) this, _dbRes );
			}
		}

		PR_TRACE((this, prtNOTIFY, 
			"urlflt\tUrlFlt::_HeuristicAnalyze: Keywords meta tag"));

		// Keywords
		{
			std::vector< std::wstring > words;
			_analyzeData.processKeywordsTag( words );

			if ( !words.empty() )
			{
				//cAutoCS _lock( m_dbCS, false );
				searchRes.m_info.Trace( "Analyze keywords tag..." );
				analyzeWords( words, searchRes );

				//DUMP_DB_RES( ( hOBJECT ) this, _dbRes );
			}
		}

		PR_TRACE((this, prtNOTIFY, 
			"urlflt\tUrlFlt::_HeuristicAnalyze: Title tag"));

		// Title
		{
			std::vector< std::wstring > words;
			_analyzeData.processTitleTag( words );

			if ( !words.empty() )
			{
				//cAutoCS _lock( m_dbCS, false );
				searchRes.m_info.Trace( "Analyze title tag..." );
				analyzeWords( words, searchRes );

				//DUMP_DB_RES( ( hOBJECT ) this, _dbRes );
			}
		}

		//

		/*
		_analyzeData.ProcessDescriptionWords();
		//if( _analyzeData.m_strDescriptionWords.size() )
		if ( !_analyzeData.m_descriptionWords.empty() )
		{
			//PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_HeuristicAnalyze: Description words: [%S]", 
			//	_analyzeData.m_strDescriptionWords.c_str() ));

			{
				//m_db.AnalyzeData(_analyzeData.m_strDescriptionWords.c_str(), _analyzeData.m_strDescriptionWords.size(), _dbRes);

				{
					cAutoCS _lock( m_dbCS, false );

					analyzeDescriptionWords( _analyzeData, _dbRes );
				}
				DUMP_DB_RES( ( hOBJECT ) this, _dbRes );
			}
		}
		*/
		PR_TRACE((this, prtNOTIFY, 
			"urlflt\tUrlFlt::_HeuristicAnalyze: Words results"));

		DUMP_DB_RES( ( hOBJECT ) this, searchRes.m_res );

		PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_HeuristicAnalyze: Words analyze end"));

		if ( searchRes.m_res.StopAnalyzing( searchRes.m_denyCategoriesMask ) )
			return errOK;
	}

	// Process page links
	if( _Prm.m_fHeuristicContentUrls )
	{
		PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_HeuristicAnalyze: Analyze links: start"));

		_analyzeData.ProcessURLs( strSiteURL );
		tDWORD nLinks = _analyzeData.m_aLinks.size();

		PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_HeuristicAnalyze: Link count: %d", 
			( int )nLinks));

		searchRes.m_info.Trace( "Analyze links..." );

		if( nLinks )
		{
			// Currently the results for links will be accumulated there;
			// Afterwards they will be joined with the main results
			CParctlDb::Result dbLinksRes;
			// Calculate links for each category
			std::vector< int > badLinks( eUrlFltCategoryCount );

			{
				//cAutoCS _lock( m_dbCS, false );

				for(tDWORD i = 0; i < _analyzeData.m_aLinks.size(); i++)
				{
					_SiteData::LINK& _link = _analyzeData.m_aLinks[i];

					/*
					PR_TRACE((this, prtNOTIFY, 
						"urlflt\tUrlFlt::_HeuristicAnalyze: Link (%d): [%s], [%S]", 
						( int )i, _link.m_url.m_url, _link.m_name.c_str() ));
					*/

					if ( _link.m_url.m_urlLen )
					{
						//CParctlDb::Result _dbUrlRes;
						//if( m_db.CheckUrl(_link.m_url, _dbUrlRes) == errACCESS_DENIED )
						CHECK_URL( ( tCHAR* ) _analyzeData.m_URLsBuf.ptr(), _analyzeData.m_URLsBuf.size(), _link.m_url );
						CParctlDb::SearchResult urlSearchRes( searchRes.m_denyCategoriesMask, searchRes.m_info );
						const tERROR err = m_dbManager.CheckUrl( _link.m_url, urlSearchRes );
						if ( err == errACCESS_DENIED )
						{
							dbLinksRes.AppendAccumulate( urlSearchRes.m_res );

							// Add counter for the appropriate link's group
							_updateStats( urlSearchRes.m_res, badLinks );

							PR_TRACE((this, prtNOTIFY, 
								"urlflt\tUrlFlt::_HeuristicAnalyze: [%s]: link analyze results",
								_link.m_url.m_url ));
							DUMP_DB_RES( ( hOBJECT ) this, urlSearchRes.m_res );
						}
					}

					if( _link.m_name.size() )
					{
						//CParctlDb::Result _dbTextRes;
						//m_db.AnalyzeData(_link.m_name.c_str(), _link.m_name.size(), _dbTextRes);
						CParctlDb::SearchResult textSearchRes( searchRes.m_denyCategoriesMask, searchRes.m_info );
						m_dbManager.AnalyzeData
							( 
								_link.m_name.c_str(), 
								_link.m_name.size(), 
								textSearchRes
							);
						if ( !textSearchRes.m_res.empty() )
						{
							dbLinksRes.AppendAccumulate( textSearchRes.m_res );

							// Add counter for the appropriate link's group
							_updateStats( textSearchRes.m_res, badLinks );

							PR_TRACE((this, prtNOTIFY, 
								"urlflt\tUrlFlt::_HeuristicAnalyze: [%S]: text analyze results",
								_link.m_name.c_str() ));
							DUMP_DB_RES( ( hOBJECT ) this, textSearchRes.m_res );
						}
					}
				}
			}

			// Currently we have two different results: join them together
			if ( !dbLinksRes.empty() )
			{
				joinDBResults( dbLinksRes, badLinks, searchRes.m_res, nLinks, searchRes.m_info );
			}
		}

		PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_HeuristicAnalyze: Analyze links: end"));
	}

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_HeuristicAnalyze: end"));

	return errOK;
}

tERROR UrlFlt::SwitchProfile(const cStrObj& strUserId, tDWORD nProfileId)
{
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::SwitchProfile: start"));

	cUrlFltReportEvent _Evt(cUrlFltReportEvent::eSwitchProfile);
	_Evt.m_tmTimeStamp = (__time32_t)(time_t)cDateTime::now_utc();
	
	bool profileChanged = false;

	{
		cAutoCS _lock(m_hCS, true);

		tUser * _user = _GetUser(strUserId, true);
		if( !_user )
			return errOBJECT_NOT_FOUND;

		tProfile * pProfile = _GetProfile(nProfileId);
		if( !pProfile )
			return errOBJECT_NOT_FOUND;

		// To be sure that profile is really changed
		if ( _user->m_nProfileId != nProfileId )
		{
			_user->m_nProfileId = nProfileId;
			_user->m_bFixProfile = 1;

			_Evt.m_strUserId = strUserId;
			_Evt.m_strUfProfile = pProfile->m_strName;
			_Evt.m_nUfProfileId = pProfile->m_nId;

			profileChanged = true;
		}
	}
	
	if ( profileChanged )
	{
		_Evt.m_nSeverity = pmc_EVENTS_IMPORTANT;
		sysSendMsg(_Evt.m_nSeverity, 0, NULL, &_Evt, SER_SENDMSG_PSIZE);
	}

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::SwitchProfile: end"));

	return errOK;
}

bool UrlFlt::CheckProxyChanged()
{
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::CheckProxyChanged: start"));

	cBLSettings _blSett;
	tERROR err = m_hTM->GetProfileInfo(AVP_PROFILE_PRODUCT, &_blSett);
	if( PR_FAIL(err) )
		return false;

	cAutoCS _lock(m_hCS, true);
	bool fChanged = !m_ProxySettings.isEqual(&_blSett.m_ProxySettings);
	if( fChanged )
		m_ProxySettings = _blSett.m_ProxySettings;

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::CheckProxyChanged: end"));

	return fChanged;
}

tERROR UrlFlt::DataProfilesSet(bool bWrite)
{
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::DataProfilesSet: start"));

	if( !m_hPersData )
		return warnFALSE;
	
	tERROR err = errOK;
	cUrlFltPersistentProfileData _data;

	cSerDescriptor * pSerDsc = cSerializableObj::getDescriptor(cUrlFltCacheData::eIID);

	if( bWrite )
	{
		{
			cAutoCS _lock(m_hCS, false);

			tDWORD i = 0, n = m_profiles.size();

			_data.m_aProfiles.resize(n);
			if( _data.m_aProfiles.size() != n )
				return errNOT_ENOUGH_MEMORY;

			for(; i < n; i++)
				if( PR_FAIL(err = m_profiles[i]->DataSet(_data.m_aProfiles[i], true)) )
					return err;
		}

		return ::g_root->RegSerialize(&_data, cUrlFltPersistentProfileData::eIID, (hREGISTRY)m_hPersData, NULL);
	}

	cSerializable * pSer = &_data;
	if( PR_FAIL(err = ::g_root->RegDeserialize(&pSer, (hREGISTRY)m_hPersData, NULL, cUrlFltPersistentProfileData::eIID)) )
		return err;

	{
		cAutoCS _lock(m_hCS, true);

		tDWORD i = 0, n = _data.m_aProfiles.size();

		m_profiles.resize(n);
		if( m_profiles.size() != n )
			return errNOT_ENOUGH_MEMORY;

		for(; i < n; i++)
		{
			tProfile * _p = tProfile::_new();
			if( !_p )
			{
				err = errNOT_ENOUGH_MEMORY;
				break;
			}
			
			m_profiles[i] = _p;
			if( PR_FAIL(err = _p->DataSet(_data.m_aProfiles[i], false)) )
				break;
		}

		if( PR_FAIL(err) )
			tProfile::_clear_a(m_profiles);
	}

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::DataProfilesSet: end"));

	return err;
}

tERROR UrlFlt::DataCacheSet(bool bWrite)
{
	if( !m_hPersData )
		return warnFALSE;

	tERROR err = errOK;
/*
	cUrlFltPersistentCacheData _data;

	if( bWrite )
	{
		{
			cAutoCS _lock(m_hCS, false);

			tDWORD i = 0, n = m_cache.size();

			_data.m_aCache.resize(n);
			if( _data.m_aCache.size() != n )
				return errNOT_ENOUGH_MEMORY;

			for(; i < n; i++)
				if( PR_FAIL(err = m_cache[i].DataSet(_data.m_aCache[i], true)) )
					return err;
		}

		return ::g_root->RegSerialize(&_data, cUrlFltPersistentCacheData::eIID, (hREGISTRY)m_hPersData, NULL);
	}

	cSerializable * pSer = &_data;
	if( PR_FAIL(err = ::g_root->RegDeserialize(&pSer, (hREGISTRY)m_hPersData, NULL, cUrlFltPersistentCacheData::eIID)) )
		return err;

	{
		cAutoCS _lock(m_hCS, true);

		tDWORD i = 0, n = _data.m_aCache.size();

		m_cache.resize(n);
		if( m_cache.size() != n )
			return errNOT_ENOUGH_MEMORY;

		for(; i < n; i++)
			if( PR_FAIL(err = m_cache[i].DataSet(_data.m_aCache[i], false)) )
				break;

		if( PR_FAIL(err) )
			m_cache.clear();
	}
*/

	return err;
}

///
/// Check if there are ancient records in the cache is about to be removed
///
tERROR UrlFlt::checkForCacheCleaning( const tDWORD counter )
{
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::checkForCacheCleaning: start"));

	tERROR err = errOK;

	tDWORD cacheTTL = 0;
	tDWORD cacheErrTTL = 0;
	{
		cAutoCS lock( m_hCS, false );
		cacheTTL = m_nCacheTTL;
		cacheErrTTL = m_nCacheErrTTL;
	}

	if ( ( cacheTTL == 0 ) || ( cacheErrTTL == 0 ) )
	{
		err = errPARAMETER_INVALID;
	}

	if ( PR_SUCC( err ) )
	{
		cAutoCS lock( m_cacheCS, true );

		const time_t timeNow = cDateTime::now_utc();

		int n = m_cache.size();
		int i = 0;
		while ( i < n )
		{
			PR_ASSERT( i >= 0 && i < ( int ) m_cache.size() );
			tCasheObj& _c = m_cache[ i ];

			const tDWORD timeDiff = ( timeNow - _c.m_time );

			// Check if it is time to remove ancient records in the cache
			bool remove = false;

			/*
			if ( _c.m_err )
			{
				if ( timeDiff >= cacheErrTTL )
				{
					remove = true;
				}
			}
			else
			*/
			if ( _c.m_analyzed )
			{
				if ( timeDiff >= cacheTTL )
				{
					remove = true;
				}
			}
			else
			{
				if ( timeDiff >= cacheErrTTL )
				{
					remove = true;
				}
			}

			if ( remove )
			{
				PR_TRACE((this, prtNOTIFY, 
					"urlflt\tUrlFlt::ProcessObject: Cache: site removed: [%s] (cache index = %d, cache size = %d)",
					_c.m_url.c_str(), i, ( int ) m_cache.size() - 1 ));

				m_cache.remove( i );
				--n;
			}
			else
			{
				++i;
			}
		}
	}

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::checkForCacheCleaning: end"));

	return err;
}

tERROR UrlFlt::IdleWork(tDWORD counter)
{
	tERROR err = errOK;
	if( PR_SUCC(err) && ( ( counter % 12 ) == 0 ) /*per each 2 minute*/  )
		err = DataProfilesSet(true);
	if( PR_SUCC(err) && ( ( counter % 180 ) == 0 )/*per each 30 minutes*/ )
		err = DataCacheSet(true);

	// Processed URL's cache
	if ( PR_SUCC( err ) && ( ( counter % 90 ) == 0 ) ) // each 15 minutes
	{
		// Check if some of cache's records should be removed
		err = checkForCacheCleaning( counter );
	}

	// DNS cache
	if ( PR_SUCC( err ) && ( ( counter % 90 ) == 0 ) ) // each 15 minutes
	{
		cAutoCS lock( m_dnsCacheCS, true );
		const time_t timeNow = cDateTime::now_utc();
		m_dnsCache.Clear( ( hOBJECT ) this, timeNow );
	}

	return err;
}

void UrlFlt::_Clear()
{
	// Note: the call should be under protection of db' critical section
	//cAutoCS _lock( m_dbCS, true );

	//m_db.Clear();
	m_dbManager.Destroy( *this );
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt: database manager successfully destroyed"));

	m_users.clear();
	tProfile::_clear_a(m_profiles);
	m_common.Clear();
}

tERROR UrlFlt::_ReloadDb(const cStrObj& strDbFile, const bool lockLoadBasesMutex)
{
	// Note: the call should be under protection of db' critical section
	//cAutoCS _lock( m_dbCS, true );
	m_dbManager.Lock();

	m_dbManager.Clear();

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_ReloadDb: start"));

	cBasesLoadLocker LoadLocker((cObject *)this, m_sDatabaseUpdCat.c_str(cCP_ANSI),
        lockLoadBasesMutex ? cBasesLoadLocker::withLock : cBasesLoadLocker::noLock);

	tERROR err = errOK;

	// Extract database file names and load each of them
	std::vector< cStringObj > db_files;
	parseFiles( strDbFile, db_files, false );

	for ( std::vector< cStringObj >::size_type i = 0; i < db_files.size(); ++i )
	{
		// Current database file name
		const cStringObj& strDbFile = db_files[ i ];

		cIOObj pIO( *this, cAutoString( strDbFile ), fACCESS_READ, fOMODE_OPEN_IF_EXIST );
		err = pIO.last_error();

		if( PR_FAIL( err ) )
		{
			break;
		}

		// Get file name
		cStringObj db_file;
		getFileName( strDbFile, db_file );

		err = m_dbManager.LoadDatabase( db_file.data(), pIO );
		//zzz send to report that database is loaded!!!

		if( PR_FAIL( err ) )
		{
			break;
		}
	}

	/*
	cIOObj pIO(*this, cAutoString(strDbFile), fACCESS_READ, fOMODE_OPEN_IF_EXIST);
	err = pIO.last_error();
	if( PR_FAIL(err) )
		return err;

	err = m_db.Load(pIO);
	if( PR_FAIL(err) )
		return err;
	*/

	if ( PR_FAIL( err ) )
	{
		m_dbManager.Clear();
	}

	m_dbManager.Unlock();
	
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_ReloadDb: end: %terr", err ));

	return err;
}

tERROR UrlFlt::_GetUserByPID(tULONG nPID, cToken ** pToken, cStrObj& szUser, tDWORD& nType)
{
	//PR_TRACE((this, prtNOT_IMPORTANT, "urlflt\tUrlFlt::_GetUserByPID: start"));

	cERROR err = errOK;
	szUser.clear();
	nType = TOKEN_TYPE_UNKNOWN;

	cAutoObj<cToken> hToken;
	if( PR_FAIL(err = sysCreateObject(hToken, IID_TOKEN)) )
		return err;

	hToken->propSetDWord(plPROCESS_ID, nPID);

	if( PR_FAIL(err = hToken->sysCreateObjectDone()) )
		return err;

	nType = hToken->propGetDWord(plTOKEN_TYPE);
	if( PR_FAIL(err = szUser.assign(hToken, pgOBJECT_FULL_NAME)) )
		return err;
	
	if( pToken )
		*pToken = hToken.relinquish();

	//PR_TRACE((this, prtNOT_IMPORTANT, "urlflt\tUrlFlt::_GetUserByPID: end"));

	return errOK;
}

UrlFlt::tUser * UrlFlt::_GetUser(const cStrObj& strUserId, bool bCreate, tDWORD * pPos)
{
	//PR_TRACE((this, prtNOT_IMPORTANT, "urlflt\tUrlFlt::_GetUser: start"));

	tUser * _user = NULL;
	
	tDWORD i = 0, n = m_users.size();
	for(; i < n; i++)
	{
		_user = &m_users[i];
		if( _user->m_user == strUserId )
			break;
	}

	if( i == n )
	{
		if( bCreate )
		{
			_user = &m_users.push_back();
			_user->m_user = strUserId;
			_user->m_nProfileId = m_defUser.m_nProfileId;
		}
		else
			_user = NULL;
	}

	if( pPos )
		*pPos = i;

	//PR_TRACE((this, prtNOT_IMPORTANT, "urlflt\tUrlFlt::_GetUser: end"));

	return _user;
}

UrlFlt::tProfile * UrlFlt::_GetProfile(tDWORD nId, bool bCreate, tDWORD * pPos)
{
	//PR_TRACE((this, prtNOT_IMPORTANT, "urlflt\tUrlFlt::_GetProfile: start"));

	tProfile * _p = NULL;
	
	tDWORD i = 0, n = m_profiles.size();
	for(; i < n; i++)
	{
		_p = m_profiles[i];
		if( _p->m_nId == nId )
			break;
	}

	if( i == n )
	{
		if( bCreate )
		{
			_p = tProfile::_new(nId);
			m_profiles.push_back(_p);
		}
		else
			_p = NULL;
	}
	
	if( pPos )
		*pPos = i;

	//PR_TRACE((this, prtNOT_IMPORTANT, "urlflt\tUrlFlt::_GetProfile: end"));

	return _p;
}

UrlFlt::tProfile * UrlFlt::_GetUserProfile(const cStrObj& strUserId)
{
	//PR_TRACE((this, prtNOT_IMPORTANT, "urlflt\tUrlFlt::_GetUserProfile: start"));

	tUser * _user = _GetUser(strUserId);
	if( !_user )
		return NULL;

	//PR_TRACE((this, prtNOT_IMPORTANT, "urlflt\tUrlFlt::_GetUserProfile: end"));

	return _GetProfile(_user->m_nProfileId);
}

void UrlFlt::_ClearHeurCache()
{
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_ClearHeurCache: start"));

	cAutoCS lock( m_cacheCS, true );
	m_cache.clear();
	PR_TRACE((this, prtIMPORTANT, "urlflt\tHeuristic cache was cleared."));

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_ClearHeurCache: end"));
}

tERROR UrlFlt::_GetHeuristicErrorImportance(tERROR errHeuristic)
{
	if( PR_ERR_ID(errHeuristic) == IID_UPDATERCONFIGURATOR )
	{
		switch( PR_ERR_CODE(errHeuristic) )
		{
		case KLUPD::CORE_CANT_CONNECT_TO_PROXY:
		case KLUPD::CORE_PROXY_AUTH_ERROR:
		case KLUPD::CORE_CANT_RESOLVE_PROXY:
		case KLUPD::CORE_FailedToCreateFolder:
		case KLUPD::CORE_NotEnoughPermissions:
		case KLUPD::CORE_WRONG_KEY:
		case KLUPD::CORE_ADMKIT_FAILURE:
			return errNOT_OK;

		case KLUPD::CORE_CANCELLED:
			//case KLUPD::CORE_SERVER_AUTH_ERROR:
			//case KLUPD::CORE_CANT_CONNECT_INET_SERVER:
			return warnFALSE;
		}
	}
	else
	{
		if( errHeuristic == errABANDONED )
			return warnFALSE;
		if( errHeuristic == warnFALSE )
			return errOK;
		
		if( PR_FAIL(errHeuristic) )
			return errNOT_OK;
		if( errHeuristic != errOK && PR_SUCC(errHeuristic) )
			return warnFALSE;
	}

	return errOK;
}

// #################################################################################

///
/// Perform transport-related objects initialization
///
tERROR UrlFlt::_TrMgr_Init()
{
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_TrMgr_Init: start"));

	tERROR err = errOK;
	err = sysCreateObjectQuick(m_proxydetect, IID_PROXYSERVERADDRESSDETECTOR, PID_PROXYSERVERADDRESSDETECTORIMPLEMENTATION);
	if( PR_SUCC(err) )
		err = sysCreateObjectQuick((hOBJECT*)&m_hCsTrMgr, IID_CRITICAL_SECTION);
	m_fTrMgrWork = 1;

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_TrMgr_Init: end"));

	return err;
}

///
/// Perform transport-related objects cleaning
///
void UrlFlt::_TrMgr_DeInit()
{
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_TrMgr_DeInit: start"));

	cVectorSimple(cAutoObj<cTransport>) _objs;
	{
		cAutoCS _lock(m_hCsTrMgr, true);
		_objs.resize(m_TrObjs.size());
		for(tDWORD i = 0; i < m_TrObjs.size(); i++)
			_objs[i] = m_TrObjs[i].m_tr.relinquish();
		m_TrObjs.clear();
	}
	
	_objs.clear();
	
	m_proxydetect.clean();

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_TrMgr_DeInit: end"));
}

///
/// Get transport object for use
///
//tERROR UrlFlt::_TrMgr_GetObj(cTransport ** ppTransp, tDWORD nTranspNetTimeout, const cStrObj& strUserAgent)
tERROR UrlFlt::_TrMgr_GetObj( cTransport** ppTransp )
{
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_TrMgr_GetObj: start"));

	if( !ppTransp )
		return errPARAMETER_INVALID;
	
	cAutoCS _lock(m_hCsTrMgr, true);
	if( !m_fTrMgrWork )
		return errABANDONED;

	cTransport * pTransp = NULL;
	const tDWORD size = m_TrObjs.size();
	for(tDWORD i = 0; i < size; i++)
	{
		tTrObj& _t = m_TrObjs[i];
		if( !_t.m_used )
		{
			_t.m_used = 1;
			pTransp = _t.m_tr;
			break;
		}
	}

	if( !pTransp )
	{
		PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_TrMgr_GetObj: create new transport"));

		tERROR err = sysCreateObjectQuick((hOBJECT *)&pTransp, IID_TRANSPORT, PID_TRANSPORTIMPLEMENTATION);
        if( PR_FAIL(err) )
		    PR_TRACE((this, prtIMPORTANT, "urlflt\tFailed to create transport object"));

		if( PR_SUCC(err) )
        {
			err = pTransp->set_pgTRANSPORT_TM(reinterpret_cast<cObj *>(m_hTM));
            if( PR_FAIL(err) )
    		    PR_TRACE((this, prtIMPORTANT, "urlflt\tFailed to set task manager parameter on transport object"));
        }

		if( PR_SUCC(err) )
        {
            err = pTransp->set_pgTRANSPORT_TASK(reinterpret_cast<cObj *>(this));
            if( PR_FAIL(err) )
    		    PR_TRACE((this, prtIMPORTANT, "urlflt\tFailed to set task parameter on transport object"));
        }

		if( PR_SUCC(err) )
        {
			err = pTransp->set_pgPROXY_DETECTOR(static_cast<cObj *>(m_proxydetect));
            if( PR_FAIL(err) )
    		    PR_TRACE((this, prtIMPORTANT, "urlflt\tFailed to set proxy detector parameter on transport object"));
        }

		/*
		if( PR_SUCC(err) )
		{
			err = strUserAgent.copy(pTransp, pgTRANSPORT_USER_AGENT);
			if( PR_FAIL(err) )
				PR_TRACE((this, prtIMPORTANT, "urlflt\tFailed to set user agent parameter on transport object"));
		}
		*/

		if( PR_SUCC(err) )
        {
			const tDWORD nTranspNetTimeout = 15;

            // reduce timeout, because we do not want Client's Browser to be too
             // much slow in case Parental Control component is incorrectly configured
			err = pTransp->set_pgTRANSPORT_NETWORK_TIMEOUT_SECONDS(nTranspNetTimeout);
            if( PR_FAIL(err) )
    		    PR_TRACE((this, prtIMPORTANT, "urlflt\tFailed to set network timeout parameter on transport object"));
        }

		if( PR_FAIL(err) )
		{
			if( pTransp )
				pTransp->sysCloseObject();
			return err;
		}
		
		m_TrObjs.push_back().m_tr = pTransp;
	}
	else
	{
		PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_TrMgr_GetObj: re-use transport"));
	}
	
	*ppTransp = pTransp;
	sysValidateLock(**ppTransp);

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_TrMgr_GetObj: end"));

	return errOK;
}

///
/// Release transport object when there is no need for it
///
void UrlFlt::_TrMgr_ReleaseObj(cTransport * pTransp)
{
	if( !pTransp )
		return;
	
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_TrMgr_ReleaseObj: start"));

	cTransport * pTranspClose = NULL;
	{
		cAutoCS _lock(m_hCsTrMgr, true);

		const tDWORD size = m_TrObjs.size();
		for(tDWORD i = 0; i < size; i++)
		{
			tTrObj& _t = m_TrObjs[i];
			if( _t.m_tr == pTransp )
			{
				if( m_nTrMax < m_TrObjs.size() )
				{
					pTranspClose = _t.m_tr.relinquish();
					m_TrObjs.remove(i);
				}
				else
				{
					_t.m_used = 0;
				}
				
				break;
			}
		}
	}

	pTransp->sysRelease();
	if( pTranspClose )
	{
		PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_TrMgr_ReleaseObj: destroy transport"));

		pTranspClose->sysCloseObject();
		pTranspClose = NULL;
	}

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_TrMgr_ReleaseObj: end"));
}

///
/// Perform emergency cleaning of transport-related objects
///
void UrlFlt::_TrMgr_AbortJobs()
{
	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_TrMgr_AbortJobs: start"));

	cAutoCS _lock(m_hCsTrMgr, true);

	for(tDWORD i = 0; i < m_TrObjs.size(); i++)
	{
		tTrObj& _t = m_TrObjs[i];
		_t.m_tr->httpCloseSession();
	}

	m_fTrMgrWork = 0;

	PR_TRACE((this, prtNOTIFY, "urlflt\tUrlFlt::_TrMgr_AbortJobs: end"));
}


// #################################################################################
// #################################################################################

#ifdef _DEBUG

void _TestWriteData(cIO * p_hDestination)
{
	tCHAR g_szTestContent[] =
	{
		"<html>\r\n"
		"<head>\r\n"
		"<title>Пупсик.Ru - Здесь не играют в куклы - здесь играют по-взрослому!</title>\r\n"
		"<meta name=\"keywords\" http-equiv=\"keywords\" content=\"эротика развлекательный сайт секс девочки азиатки попки грудь супер развлечения\">\r\n"
		//		"<meta name=\"keywords\" http-equiv=\"keywords\" content=\" а б  в \">\r\n"
		"<meta name=\"description\" http-equiv=\"description\" content=\"Пупсик.Ru - Здесь не играют в куклы\">\r\n"
		"<meta http-equiv=\"Content-Type\" content=\"text/html; CharSet=windows-1251\">\r\n"
		"<style type=\"text/css\">\r\n"
		"<!--\r\n"
		".regular {font-size: 12pt}\r\n"
		".small {font-size: 10pt}\r\n"
		".menu { color : #000000; font-size: 9pt; font-family: Tahoma, Arial; font-weight: bold;}\r\n"
		".add { color : #0000ff; font-size: 10pt; font-family: Tahoma, Arial; font-weight: bold;}\r\n"
		"a:link {color : #000000; text-decoration : underline}\r\n"
		"a:visited {color : #000000; text-decoration : underline}\r\n"
		"a:active {color : #000000; text-decoration : underline}\r\n"
		"a:hover {color : #000000; text-decoration : none}\r\n"
		".port {width: 80pt; font-size: 9pt; font-weight: bold; font-family: Verdana, Tahoma, Sans-Serif; border: 1 outset #000000;}\r\n"
		"-->\r\n"
		"	</style>\r\n"
		"\r\n"
		"	<!--<script language=\"JavaScript\" src=\"http://fomenko.ru/cgi-bin/ip2country.fcgi\"></script>\r\n"
		"<a href=\"http://afisha.nu\" target=\"_blank\"><img "
		"<a href=http://prastitutki.ru/10/enter.html target=_blank>Проститутки</a><br>"
	};

	tDWORD nDataSize = sizeof(g_szTestContent);
	tDWORD chunkSize = 100;
	for(tDWORD nPos = 0; nPos < nDataSize; )
	{
		tDWORD res;
		tDWORD nWriteSize = (nDataSize - nPos < chunkSize) ? (nDataSize - nPos) : chunkSize;
		if( PR_FAIL(p_hDestination->SeekWrite(&res, nPos, g_szTestContent + nPos, nWriteSize)) )
			break;

		nPos += nWriteSize;
	}
}

void _TestDownload(UrlFlt * _this)
{
/*	tERROR err; cStrObj strTestUrl;

	LPWSTR strTextSites[] =
	{
		//L"http://www.rambler.ru",
		L"http://sex.ru",
		L"http://u708.16.spylog.com",
		L"http://pics.rbc.ru",
		L"http://images.rambler.ru",
		L"http://hit2.hotlog.ru",
		L"http://counting.kmindex.ru",
		L"http://counter.rambler.ru",
	};
	
	cTransport * pTransp = NULL; _this->_TrMgr_GetObj(&pTransp, 15, L"Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)");

	for(tDWORD i = 0; i < countof(strTextSites); i++)
	{
		LPWSTR strTextSite = strTextSites[i];
		_analyzeData.m_curPos = 0;
		PR_TRACE((_this, prtIMPORTANT, "urlflt\tTest dowload (%S) begin...", strTextSite));
		err = pTransp->httpGetFile(cAutoString(cStrObj(strTextSite)), &_analyzeData);
		PR_TRACE((_this, prtIMPORTANT, "urlflt\tTest dowload (%S) end: 0x%08x", strTextSite, err));
	}

	_this->_TrMgr_ReleaseObj(pTransp);
*/
}

#endif // _DEBUG

// #################################################################################
// #################################################################################
