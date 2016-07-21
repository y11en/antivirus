

#include <kuhash.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef _DEBUG
 #ifdef _MSC_VER
    #include <crtdbg.h>
 #endif
#endif

static void hash_test( void );

int main( int argc, char* argv[] )
{
#if defined(_DEBUG)
	//  Enable this code for memory leak testing
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

	hash_test();
	


	return 0;
}


static void hash_test( void )
{
	int ret;
	int32_t pos;
	kuhash_t hash;
	const kuhash_elem_t* e;

	ret = kuhash_create( kuhash_chars, kuhash_compare_chars, 100, GROW_ONLY, &hash );
	if( KU_FAILURE( ret ) )
	{
		fprintf( stderr, "error: unable to create hash.\n" );
		exit( 1 );
	}

	kuhash_set( hash, "key", "value" );
	kuhash_set( hash, "key1", "value1" );
	kuhash_set( hash, "key2", "value2" );
	kuhash_set( hash, "windows-1251", "test" );

	printf( "hash size: %ld\n", (long) kuhash_size( hash ) );


	pos = -1;
	while( ( e = kuhash_next_elem( hash, &pos ) ) != NULL )
	{
		printf( "'%s' = '%s'\n", e->key, e->value );
	}






	e = kuhash_find( hash, "key" );
	if( e != NULL )
	{
		printf( "value = '%s'\n", e->value );
	}
	
	e = kuhash_find( hash, "key1" );
	if( e != NULL )
	{
		printf( "value = '%s'\n", e->value );
	}

	e = kuhash_find( hash, "key2" );
	if( e != NULL )
	{
		printf( "value = '%s'\n", e->value );
	}

	e = kuhash_find( hash, "windows-1251" );
	if( e != NULL )
	{
		printf( "value = '%s'\n", e->value );
	}


	kuhash_destroy( hash );
}
