

#include "mlang_conv.h"

converter_info* mlang_converter_info::create()
{
	return new mlang_converter_info();
}

mlang_converter_info::mlang_converter_info()
{
	HRESULT hr;

    hr = CoInitialize( NULL );
	if( FAILED(hr) )
	{
		fprintf( stderr, "Error initializing COM: %ld\n", hr );
		exit( 1 );
	}


    hr = CoCreateInstance(
        CLSID_CMultiLanguage,	// REFCLSID rclsid,
        NULL,					// LPUNKNOWN pUnkOuter,
        CLSCTX_INPROC_SERVER,	// DWORD dwClsContext,
        IID_IMultiLanguage2,	// REFIID riid,
        (void**) &m_ml			// LPVOID * ppv
        );

	if( FAILED(hr) )
	{
		fprintf( stderr, "Error creating MLang COM object: %ld\n", hr );
		exit( 1 );
	}

}

mlang_converter_info::~mlang_converter_info()
{
	m_ml->Release();
	m_ml = NULL;

	CoUninitialize();
}


void mlang_converter_info::get_encodings( encodings_map& encodings, int argc, char* argv[] )
{
	HRESULT hr;
	IEnumCodePage* enum_codepage;


	hr = m_ml->EnumCodePages(
			MIMECONTF_VALID_NLS,
			MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US ),
			&enum_codepage );


	encodings.clear();

	for(;;)
	{
		ULONG fetched;
		MIMECPINFO mcpi;
		char web_charset_name[ 100 ];
		char charset_description[ 100 ];
		DWORD mode;
		UINT dst_size;

		fetched = 0;
		hr = enum_codepage->Next( 1, &mcpi, &fetched );
		if( FAILED(hr) )
		{
			printf( "Error enumerating codepages: can't get next codepage info: %ld\n", hr );
			exit( 1 );
		}

		if( hr == S_FALSE )
		{
			break;
		}
		
		mode = 0;
		dst_size = sizeof(web_charset_name);
		m_ml->ConvertStringFromUnicode( &mode, 1252,
				mcpi.wszWebCharset, NULL, web_charset_name, &dst_size );
		web_charset_name[ dst_size ] = '\0';

		mode = 0;
		dst_size = sizeof(charset_description);
		m_ml->ConvertStringFromUnicode( &mode, 1252,
				mcpi.wszDescription, NULL, charset_description, &dst_size );
		charset_description[ dst_size ] = '\0';

//		printf( "%s - %s\n", web_charset_name, charset_description );		

		int use_encoding = 1;
		if( argc > 0 )
		{
			use_encoding = 0;
			for( int i = 0; i < argc; ++i )
			{
				if( strstr( web_charset_name, argv[ i ] ) != NULL )
				{
					use_encoding = 1;
					break;
				}
				else
				{
					int codepage = -1;
					sscanf( argv[ i ], "%d", &codepage );
					if( mcpi.uiCodePage == codepage )
					{
						use_encoding = 1;
						break;
					}
				}
			}
		}


		if( !use_encoding )
		{
			continue;
		}


		encoding_info* info = new encoding_info;
		strcpy( info->name, web_charset_name );
		strcpy( info->description, charset_description );
		
		char buf[ 20 ];
		sprintf( buf, "%u", mcpi.uiCodePage );

		if( encodings.find( buf ) != encodings.end() )
		{

		}

		encodings[ buf ] = info;
	}


	enum_codepage->Release();
}

int mlang_converter_info::is_ignorable_encoding( const std::string& cnv_id )
{
	int id = atoi( cnv_id.c_str() );

	if( id == 1200 || id == 1201 || id > 50000 )
	{
		return 1;
	}

	return 0;
}



converter* mlang_converter_info::create_converter( const std::string& cnv_id ) 
{
	return new mlang_converter( cnv_id, this );
}

void mlang_converter_info::destroy_converter( converter* cnv )
{
	delete static_cast<mlang_converter*>( cnv );
}


mlang_converter::mlang_converter( const std::string& cnv_id, mlang_converter_info* cnv_info )
{
	m_codepage = atoi( cnv_id.c_str() );

	m_ml = cnv_info->m_ml;
	m_ml->AddRef();
}

mlang_converter::~mlang_converter()
{
	m_ml->Release();
	m_ml = NULL;
}

int mlang_converter::from_ucs( ucs_t c, char* dst, int dst_len )
{
	HRESULT hr;
	WCHAR wbuf[ 2 ];
	UINT wlen;
    DWORD mode;
	WCHAR defchar;

	if( c < 0x10000 )
	{
		wbuf[ 0 ] = (WCHAR) c;
		wlen = 1;
	}
	else if( c <= 0x10ffff )
	{
        wbuf[ 0 ] = (WCHAR) ( ( c >> 10 ) + 0xd7c0 );
        wbuf[ 1 ] = (WCHAR) ( ( c & 0x3ff ) | 0xdc00 );
		wlen = 2;
	}
	else
	{
		return 0;
	}

#if 0
	
	UINT dst_len_buf = dst_len;
	defchar = 'A';
    mode = 0;
	hr = m_ml->ConvertStringFromUnicodeEx( 
			&mode, m_codepage,
			wbuf, &wlen, dst, &dst_len_buf,
			MLCONVCHARF_USEDEFCHAR, &defchar );
	if( FAILED(hr) )
	{
		return 0;
	}

	char dst2[ 20 ];
	UINT dst_len_buf2 = dst_len;
	defchar = 'B';
    mode = 0;
	hr = m_ml->ConvertStringFromUnicodeEx( 
			&mode, m_codepage,
			wbuf, &wlen, dst2, &dst_len_buf2,
			MLCONVCHARF_USEDEFCHAR, &defchar );
	if( FAILED(hr) )
	{
		return 0;
	}

	if( dst_len_buf != dst_len_buf2 || memcmp( dst, dst2, dst_len_buf ) != 0 )
	{
		// No conversion for this unicode character.
		return 0;
	}

#else

	UINT dst_len_buf = dst_len;
	defchar = 0;
    mode = 0;
	hr = m_ml->ConvertStringFromUnicodeEx( 
			&mode, m_codepage,
			wbuf, &wlen, dst, &dst_len_buf,
			MLCONVCHARF_USEDEFCHAR, &defchar );
	if( FAILED(hr) )
	{
		return 0;
	}

	if( wbuf[ 0 ] && *dst == '\0' && ( dst_len_buf == 1 || ( dst_len_buf == 2 && wlen == 2 ) )  )
	{
		return 0;
	}
	
#endif
	
	return dst_len_buf;
}


int mlang_converter::to_ucs( const char* src, int src_len, ucs_t* dst, int dst_len )
{
	HRESULT hr;
	char src_buf[ 16 ];
	UINT src_size;
	WCHAR dst_buf[ 16 ];
	UINT dst_size;


	memset( src_buf, 0, sizeof(src_buf) );
	memset( dst_buf, 0, sizeof(dst_buf) );

    DWORD mode = 0;
	memcpy( src_buf, src, src_len );
	src_size = src_len;
	dst_size = sizeof(dst_buf) / sizeof(dst_buf[0]);
    hr = m_ml->ConvertStringToUnicodeEx( 
		&mode, m_codepage,
        src_buf, &src_size,
        dst_buf, &dst_size,
        0, NULL );
	if( FAILED(hr) )
	{
		return 0;
	}

    if ( *src && dst_size == 1 && *dst_buf == '\0' )
    {
		return 0;
    }

	
	int res_len = 0;
	WCHAR* dst_buf_begin = dst_buf;
	WCHAR* dst_buf_end = dst_buf + dst_size;
	while( dst_buf_begin < dst_buf_end )
	{
		WCHAR c;

		c = *dst_buf_begin++;
		if( KU_IS_LEAD( c )
			&& ( dst_buf_begin < dst_buf_end ) && KU_IS_TRAIL( *dst_buf_begin ) )
		{
			*dst++ = KU_GET_SUPPLEMENTARY( c, *dst_buf_begin );

			++dst_buf_begin;
		}
		else
		{
			*dst++ = c;
		}

		++res_len;
	}

	return res_len;
}

void mlang_converter::get_codepage_info( codepage_info* cp_info )
{
	

}

const char* mlang_converter::get_name()
{
	static char buf[ 100 ];

//	sprintf( buf, "windows-%d-mlang", m_codepage );
	sprintf( buf, "windows-%d", m_codepage );

	return buf;
}

const char* mlang_converter::get_interface_name()
{
    static char interface_buffer[256];
    static char *pinterface = "IMultiLanguage";  // 5.50.4522.1800?
    if (interface_buffer != pinterface) {
        char mlang_dll[256] = "";
        GetSystemDirectory(mlang_dll, sizeof(mlang_dll));
        strcat(mlang_dll, "\\mlang.dll");
        DWORD dwSize;
        DWORD dwIgnore;
        LPVOID pBuf;
        VS_FIXEDFILEINFO* pLocalInfo;
        
        dwSize = GetFileVersionInfoSize(mlang_dll, &dwIgnore);
        pBuf = malloc(dwSize);
        if (pBuf && GetFileVersionInfo(mlang_dll, 0, dwSize, pBuf))
        {
            if (VerQueryValue( pBuf, "\\", (void**)&pLocalInfo, (UINT*)&dwIgnore))
            {
                sprintf(interface_buffer, "IMultiLanguage %d.%d.%d.%d",
                    HIWORD(pLocalInfo->dwFileVersionMS),
                    LOWORD(pLocalInfo->dwFileVersionMS),
                    HIWORD(pLocalInfo->dwFileVersionLS),
                    LOWORD(pLocalInfo->dwFileVersionLS));
                pinterface = interface_buffer;
            }
        }
        
        free(pBuf);
    }

    return pinterface;
}
