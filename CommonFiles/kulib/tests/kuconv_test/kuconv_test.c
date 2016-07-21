

#include <kulib/kuinit.h>
#include <kulib/kuconv.h>
#include <kulib/kucase.h>

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>


static void __test_assert_failed( const char* file, int line, const char* exp )
{
	fprintf( stderr, "%s:%d: Assertion failed!: %s\n", file, line, exp );
	exit( 1 );
}

#define TEST_ASSERT( e ) \
	do \
	{ \
		if( !e ) \
		{ \
			__test_assert_failed( __FILE__, __LINE__, #e ); \
		} \
	} \
	while( 0 )

#define KU_ASSERT( ret ) TEST_ASSERT( KU_SUCCESS(ret) )

#ifdef _DEBUG
 #ifdef _MSC_VER
    #include <crtdbg.h>
 #endif
#endif


static void utf8_test( void );
static void utf16_test( void );
static void mbcs_test( void );

static void case_test( void );

int main( int argc, char* argv[] )
{
#if defined( _DEBUG )
	//  Enable this code for memeory leak testing
	int tmpDbgFlag;

	// Send all reports to STDOUT
	_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
	_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );

	tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tmpDbgFlag);
#endif

	setlocale( LC_ALL, "" /* ".OCP" */ );
	printf( "%s\n", setlocale( LC_ALL, NULL ) );

//	TEST_ASSERT( SetConsoleCP( 1251 ) == TRUE );
	TEST_ASSERT( SetConsoleOutputCP( 1251 ) == TRUE );
//	printf( "%u %u\n", GetConsoleCP(), GetConsoleOutputCP() );

	ku_init();

	mbcs_test();
	utf8_test();
	utf16_test();

//	case_test();


	ku_term();

	return 0;
}

static void mbcs_test( void )
{
	int ret;
	kuconv_t	cnv;

/*
	for( i = 0; i < 5; ++i )
	{
		ret = kuconv_open( "windows-1251", &cnv );
		kuconv_close( cnv );
	}
	for( i = 0; i < 5; ++i )
	{
		ret = kuconv_open( "windows-1252", &cnv );
		kuconv_close( cnv );
	}
	for( i = 0; i < 5; ++i )
	{
		ret = kuconv_open( "windows-932", &cnv );
		kuconv_close( cnv );
	}
*/

	ret = kuconv_open( "windows-1251", &cnv );
	KU_ASSERT( ret );

	{
		char src_buf[] = "Тест";
		ucs_t dst_buf[ 0x200 ];

		char* src = src_buf;
		ucs_t* dst = dst_buf;
		char* src_end = src_buf + sizeof(src_buf) / sizeof(src_buf[0]);
		ucs_t* dst_end = dst_buf + sizeof(dst_buf) / sizeof(dst_buf[0]);

/*
		{
			int i;
			for( i = 0; i < 0x100; ++i )
			{
				src_buf[ i ] = i;
			}
			src_buf[ i ] = '\0';
			src_end = src_buf + 0x101;
		}
*/
		ret = kuconv_to_ucs( cnv,
				&src, src_end,
				&dst, dst_end,
				1 );
		KU_ASSERT( ret );


		{
			ucs_t* s = dst_buf;

			printf( "'" );
			while( s < dst )
			{
				printf( "%C", *s );
			
				++s;
			}
			printf( "'\n" );
		}

	}


//	kuconv_set_from_ucs_callback( cnv, KUCONV_FROM_UCS_CALLBACK_SKIP, NULL, NULL, NULL );

	{
		ucs_t src_buf[] = { 0x1000000, 0x422, 0x435, 0x441, 0x442, 
							0x500, 0x422, 0x500, 0x435, 0x500, 0x441, 0x500, 0x442, 
							0 };
		char dst_buf[ 100 ];

		ucs_t* src = src_buf;
		char* dst = dst_buf;
		ucs_t* src_end = src_buf + sizeof(src_buf) / sizeof(src_buf[0]);
		char* dst_end = dst_buf + sizeof(dst_buf) / sizeof(dst_buf[0]);


		ret = kuconv_from_ucs( cnv,
				&src, src_end,
				&dst, dst_end,
				1 );
		KU_ASSERT( ret );


		{
			printf( "'%s'\n", dst_buf );
		}

	}


	kuconv_close( cnv );


}


static void utf8_test( void )
{
	int ret;
	kuconv_t	cnv;

	ret = kuconv_open( "utf-8", &cnv );
	KU_ASSERT( ret );

//	kuconv_set_to_ucs_callback( cnv, KUCONV_TO_UCS_CALLBACK_SKIP, NULL, NULL, NULL );

	{
		char src_buf[] = /*"test";*/
		{
			(char) 0xc2, (char) 0x80,
			(char) 0xc4, (char) 0x80,
			(char) 0xc2, (char) 0x80,
			(char) 0xe4, (char) 0x80, /* (char) 0x80, */
			(char) 0xf4, (char) 0x80, (char) 0x80, (char) 0x80,
			(char) 0x80,
			(char) 0xe4, (char) 0x80, /* (char) 0x80, */
//			(char) 0
		};
		ucs_t dst_buf[ 100 ];
		char* src = src_buf;
		ucs_t* dst = dst_buf;
		char* src_end = src_buf + sizeof(src_buf) / sizeof(src_buf[0]);
		ucs_t* dst_end = dst_buf + sizeof(dst_buf) / sizeof(dst_buf[0]);

/*
		ret = kuconv_to_ucs( cnv,
				&src, src_buf + 0,
				&dst, dst_end,
				0 );
		KU_ASSERT( ret );

		ret = kuconv_to_ucs( cnv,
				&src, src_buf + 3,
				&dst, dst_end,
				0 );
		KU_ASSERT( ret );

		ret = kuconv_to_ucs( cnv,
				&src, src_buf + 8,
				&dst, dst_end,
				0 );
		KU_ASSERT( ret );

		ret = kuconv_to_ucs( cnv,
				&src, src_buf + 14,
				&dst, dst_end,
				0 );
		KU_ASSERT( ret );
*/
		ret = kuconv_to_ucs( cnv,
				&src, src_end,
				&dst, dst_end,
				1 );
		KU_ASSERT( ret );
	}


	{
		ucs_t src_buf[] = { 
				0x422, 0x435, 0x441, 0x442, 
				0x422, 0x435, 0x441, 0x442,
				0x10ffff,
				0x110000,
				0 };
		char dst_buf[ 100 ];

		ucs_t* src = src_buf;
		char* dst = dst_buf;
		ucs_t* src_end = src_buf + sizeof(src_buf) / sizeof(src_buf[0]);
		char* dst_end = dst_buf + sizeof(dst_buf) / sizeof(dst_buf[0]);

		ret = kuconv_from_ucs( cnv,
				&src, src_end,
				&dst, dst_end,
				1 );
		KU_ASSERT( ret );

		{
			ucs_t res_buf[ 100 ];
			ucs_t* res = res_buf;
			ucs_t* res_end = res_buf + sizeof(res_buf) / sizeof(res_buf[0]);

			dst_end = dst;
			dst = dst_buf;
			ret = kuconv_to_ucs( cnv,
					&dst, dst_end,
					&res, res_end,
					1 );
			KU_ASSERT( ret );

			TEST_ASSERT( memcmp( src_buf, res_buf, src_end - src_buf ) == 0 );
		}
	}

	kuconv_close( cnv );
}


static void utf16_test( void )
{
	int ret;
	kuconv_t	cnv;

	ret = kuconv_open( "utf-16le", &cnv );
	KU_ASSERT( ret );

//	kuconv_set_to_ucs_callback( cnv, KUCONV_TO_UCS_CALLBACK_SKIP, NULL, NULL, NULL );

	{
		uint16_t src_buf[] = /*"test";*/
		{
			0x422, 0x435, 0x441, 0x442,    // 0
			0, 0xd800, 0xdc00, 0,          // 8
			0, 0xd800, 0, 0, 	           // 16
			0, 0xd800, 0, 0, 	           // 24
			0, 0xdc00, 0, 0,               // 32
			0, 0xd800, 0xd800, 0, 	       // 40
			0, 0xdc00, 0xdc00, 0, 	       // 48
			0
		};
		ucs_t dst_buf[ 100 ];
		char* src = (char*) src_buf;
		ucs_t* dst = dst_buf;
		char* src_end = (char*) src_buf + sizeof(src_buf);
		ucs_t* dst_end = dst_buf + sizeof(dst_buf) / sizeof(dst_buf[0]);

		ret = kuconv_to_ucs( cnv,
				&src, (char*) src_buf + 0,
				&dst, dst_end,
				0 );
		KU_ASSERT( ret );

		ret = kuconv_to_ucs( cnv,
				&src, (char*) src_buf + 11,
				&dst, dst_end,
				0 );
		KU_ASSERT( ret );

		ret = kuconv_to_ucs( cnv,
				&src, (char*) src_buf + 21,
				&dst, dst_end,
				0 );
		KU_ASSERT( ret );
		
		ret = kuconv_to_ucs( cnv,
				&src, (char*) src_buf + 28,
				&dst, dst_end,
				0 );
		KU_ASSERT( ret );

		ret = kuconv_to_ucs( cnv,
				&src, (char*) src_buf + 43,
				&dst, dst_end,
				0 );
		KU_ASSERT( ret );
		ret = kuconv_to_ucs( cnv,
				&src, (char*) src_buf + 44,
				&dst, dst_end,
				0 );
		KU_ASSERT( ret );
		ret = kuconv_to_ucs( cnv,
				&src, (char*) src_buf + 45,
				&dst, dst_end,
				0 );
		KU_ASSERT( ret );
		ret = kuconv_to_ucs( cnv,
				&src, (char*) src_buf + 46,
				&dst, dst_end,
				0 );
		KU_ASSERT( ret );

		ret = kuconv_to_ucs( cnv,
				&src, (char*) src_end,
				&dst, dst_end,
				1 );
		KU_ASSERT( ret );
	}


	{
		ucs_t src_buf[] = { 
				0x422, 0x435, 0x441, 0x442, 
				0x422, 0x435, 0x441, 0x442,
				0x10ffff,
//				0x110000,
				0 };
		uint16_t dst_buf[ 100 ];

		ucs_t* src = src_buf;
		char* dst = (char*) dst_buf;
		ucs_t* src_end = src_buf + sizeof(src_buf) / sizeof(src_buf[0]);
		char* dst_end = (char*) dst_buf + sizeof(dst_buf);

		ret = kuconv_from_ucs( cnv,
				&src, src_end,
				&dst, dst_end,
				1 );
		KU_ASSERT( ret );

		{
			ucs_t res_buf[ 100 ];
			ucs_t* res = res_buf;
			ucs_t* res_end = res_buf + sizeof(res_buf) / sizeof(res_buf[0]);

			dst_end = dst;
			dst = (char*) dst_buf;
			ret = kuconv_to_ucs( cnv,
					&dst, dst_end,
					&res, res_end,
					1 );
			KU_ASSERT( ret );

			TEST_ASSERT( memcmp( src_buf, res_buf, ( src_end - src_buf ) * sizeof(ucs_t) ) == 0 );
		}
	}

	kuconv_close( cnv );
}


static void case_test( void )
{
	ucs_t c, res;
	FILE* fp;
	char* out_filename = "case.map.test";

	fp = fopen( out_filename, "wt" );
	if( fp == NULL )	
	{
		fprintf( stderr, "error: can't open file '%s' for writing.\n", out_filename );
	}

	fprintf( fp, "\n");
	fprintf( fp, "BEGIN UPPERCASE\n");
	fprintf( fp, "\n");
	for( c = 0; c < 0x110000; ++c )
	{
		kucase_to_upper( &c, 1, &res, 1, NULL );

		if( c != res )
		{
			fprintf( fp, "U%04x\tU%04x\n", c, res );
		}
	}
	fprintf( fp, "\n");
	fprintf( fp, "END UPPERCASE\n");


	fprintf( fp, "\n");
	fprintf( fp, "\n");
	fprintf( fp, "BEGIN LOWERCASE\n");
	fprintf( fp, "\n");
	for( c = 0; c < 0x110000; ++c )
	{
		kucase_to_lower( &c, 1, &res, 1, NULL );

		if( c != res )
		{
			fprintf( fp, "U%04x\tU%04x\n", c, res );
		}
	}
	fprintf( fp, "\n");
	fprintf( fp, "END LOWERCASE\n");

	fclose( fp );
}