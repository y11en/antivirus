
#include "nls_conv.h"


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

converter_info* nls_converter_info::create()
{
	return new nls_converter_info();
}

nls_converter_info::nls_converter_info()
{
}

nls_converter_info::~nls_converter_info()
{
}

static encodings_map* g_encodings;
static int g_argc;
static char** g_argv;

const char* get_cp_descr( int cp )
{
    switch( cp ) 
	{
    case 437: return "MS-DOS United States";
    case 708: return "Arabic (ASMO 708)";
    case 709: return "Arabic (ASMO 449+, BCON V4)";
    case 710: return "Arabic (Transparent Arabic)";
    case 720: return "Arabic (Transparent ASMO)";
    case 737: return "Greek (formerly 437G)";
    case 775: return "Baltic";
    case 850: return "MS-DOS Multilingual (Latin I)";
    case 852: return "MS-DOS Slavic (Latin II)";
    case 855: return "IBM Cyrillic (primarily Russian)";
    case 857: return "IBM Turkish";
    case 858: return "OEM - Multilingual Latin 1 + Euro";
    case 860: return "Portuguese (DOS)";
    case 861: return "MS-DOS Icelandic";
    case 862: return "Hebrew";
    case 863: return "French Canadian (DOS)";
    case 864: return "Arabic (864)";
    case 865: return "Nordic (DOS)";
    case 866: return "MS-DOS Russian (former USSR)";
    case 869: return "IBM Modern Greek";
    case 874: return "Thai";
    case 932: return "Japan";
    case 936: return "Chinese (PRC, Singapore)";
    case 949: return "Korean";
    case 950: return "Chinese (Taiwan; Hong Kong SAR, PRC)";
    case 1047: return "IBM EBCDIC - Latin-1/Open System";
    case 1200: return "Unicode (BMP of ISO 10646)";
    case 1250: return "Windows 3.1 Eastern European";
    case 1251: return "Windows 3.1 Cyrillic";
    case 1252: return "Windows 3.1 Latin 1 (US, Western Europe)";
    case 1253: return "Windows 3.1 Greek";
    case 1254: return "Windows 3.1 Turkish";
    case 1255: return "Hebrew";
    case 1256: return "Arabic";
    case 1257: return "Baltic";
    case 1258: return "Latin 1 (ANSI)";
    case 1361: return "Korean (Johab)";
    case 10000: return "(MAC - Roman)";
    case 10001: return "(MAC - Japanese)";
    case 10002: return "(MAC - Traditional Chinese Big5)";
    case 10003: return "(MAC - Korean)";
    case 10004: return "(MAC - Arabic)";
    case 10005: return "(MAC - Hebrew)";
    case 10006: return "(MAC - Greek I)";
    case 10007: return "(MAC - Cyrillic)";
    case 10008: return "(MAC - Simplified Chinese GB 2312)";
    case 10010: return "Romanian (Mac)";
    case 10017: return "Ukrainian (Mac)";
    case 10029: return "(MAC - Latin II)";
    case 10079: return "(MAC - Icelandic)";
    case 10081: return "(MAC - Turkish)";
    case 10082: return "Croatian (Mac)";
    case 20000: return "CNS - Taiwan";
    case 20001: return "TCA - Taiwan";
    case 20002: return "Eten - Taiwan";
    case 20003: return "IBM5550 - Taiwan";
    case 20004: return "TeleText - Taiwan";
    case 20005: return "Wang - Taiwan";
    case 20127: return "US ASCII";
    case 20261: return "T.61";
    case 20269: return "ISO-6937";
    case 20866: return "Ukrainian - KOI8-U";
    case 20924: return "IBM EBCDIC - Latin-1/Open System(1047 + Euro)";
    case 20932: return "Japanese (JIS 0208-1990 and 0212-1990)";
    case 20936: return "Chinese Simplified (GB2312-80)";
    case 20949: return "Korean Wansung";
    case 21027: return "Ext Alpha Lowercase";
    case 21866: return "Russian - KOI8";
    case 28591: return "ISO 8859-1 Latin I";
    case 28592: return "ISO 8859-2 Eastern Europe";
    case 28593: return "ISO 8859-3 Turkish";
    case 28594: return "ISO 8859-4 Baltic";
    case 28595: return "ISO 8859-5 Cyrillic";
    case 28596: return "ISO 8859-6 Arabic";
    case 28597: return "ISO 8859-7 Greek";
    case 28598: return "ISO 8859-8 Hebrew";
    case 28599: return "ISO 8859-9 Latin Alphabet No.5";
    case 29001: return "Europa 3";
    case 50227: return "Chinese Simplified (ISO-2022)";
    };
    return "";
}


static BOOL CALLBACK EnumCodePagesProc( LPSTR lpCodePageString )
{
	int cp;

	cp = atoi( lpCodePageString );

	
	int use_encoding = 1;
	if( g_argc > 0 )
	{
		use_encoding = 0;
		for( int i = 0; i < g_argc; ++i )
		{
			int codepage = -1;
			sscanf( g_argv[ i ], "%d", &codepage );
			if( cp == codepage )
			{
				use_encoding = 1;
				break;
			}
		}
	}


	if( !use_encoding )
	{
		return TRUE;
	}


	encoding_info* info = new encoding_info;
	sprintf( info->name, "windows-%d", cp );
	strcpy( info->description, get_cp_descr( cp ) );
	
	char buf[ 20 ];
	sprintf( buf, "%d", cp );

	(*g_encodings)[ buf ] = info;

	return TRUE;
}


void nls_converter_info::get_encodings( encodings_map& encodings, int argc, char* argv[] )
{
	g_encodings = &encodings;
	g_argc = argc;
	g_argv = argv;

	EnumSystemCodePagesA( EnumCodePagesProc, CP_INSTALLED );

}

int nls_converter_info::is_ignorable_encoding( const std::string& cnv_id )
{
	int id = atoi( cnv_id.c_str() );

	if( id == 1200 || id == 1201 || id > 50000 )
	{
		return 1;
	}

	return 0;
}



converter* nls_converter_info::create_converter( const std::string& cnv_id )
{
	return new nls_converter( cnv_id, this );
}

void nls_converter_info::destroy_converter( converter* cnv )
{
	delete static_cast<nls_converter*>( cnv );
}


nls_converter::nls_converter( const std::string& cnv_id, nls_converter_info* cnv_info )
{
	m_codepage = atoi( cnv_id.c_str() );

}

nls_converter::~nls_converter()
{
}

int nls_converter::from_ucs( ucs_t c, char* dst, int dst_len )
{
	WCHAR wbuf[ 2 ];
	UINT wlen;
	int res_len;

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


	BOOL def_used = FALSE;
	res_len = WideCharToMultiByte( m_codepage, 0, wbuf, wlen, dst, dst_len, NULL, &def_used );
	if( res_len == 0 || def_used )
	{
		return 0;
	}

	return res_len;
}

int nls_converter::to_ucs( const char* src, int src_len, ucs_t* dst, int dst_len )
{
	WCHAR dst_buf[ 16 ];
	WCHAR* dst_buf_begin;
	WCHAR* dst_buf_end;
	int res_len;
	int wc_len;

	wc_len = MultiByteToWideChar( m_codepage, MB_ERR_INVALID_CHARS, src, src_len, dst_buf, sizeof(dst_buf) );
	if( wc_len == 0 )
	{
		return 0;
	}

	res_len = 0;
	dst_buf_begin = dst_buf;
	dst_buf_end = dst_buf + wc_len;
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

void nls_converter::get_codepage_info( codepage_info* cp_info )
{
	CPINFOEX cpi;

	GetCPInfoEx( m_codepage, 0, &cpi );

	strcpy( cp_info->defchar, (char*) cpi.DefaultChar );
}


const char* nls_converter::get_name()
{
	static char buf[ 100 ];

//	sprintf( buf, "windows-%d-nls", m_codepage );
	sprintf( buf, "windows-%d", m_codepage );

	return buf;
}

const char* nls_converter::get_interface_name()
{
	static char buf[] = "Win NLS";

	return buf;
}
