// genml.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <mlang.h>

static void PrintCodePages( IMultiLanguage* pML );

int main(int argc, char* argv[])
{
	HRESULT hr;

	if( FAILED( CoInitialize( NULL ) ) )
	{
		printf( "error: Can't initialize COM.\n" );
		return 1;
	}
	
	
	CComPtr<IMultiLanguage2>	cpML;


	hr = cpML.CoCreateInstance( CLSID_CMultiLanguage );
	if( FAILED( hr ) )
	{
		printf( "error: Can't create CLSID_CMultiLanguage object.\n" );
		
		CoUninitialize();
		return 1;
	}

//	PrintCodePages( cpML );

    DWORD dwMode = 0;
	WCHAR src[] = { 999 };
	UINT src_size = sizeof(src) / sizeof(src[0]);
	char dst[ 20 ];
	UINT dst_size = sizeof(dst) / sizeof(dst[0]);
	WCHAR def_char[ 3 ] = { 0 };
		
	hr = cpML->ConvertStringFromUnicodeEx(
		&dwMode, 1251,
		src, &src_size,
		dst, &dst_size, 
		MLCONVCHARF_USEDEFCHAR, def_char );
	

	cpML.Release();
	CoUninitialize();

	return 0;
}

static void PrintCodePages( IMultiLanguage* pML )
{
	HRESULT hr;
	MIMECPINFO	mcpi;

	CComPtr<IEnumCodePage> cpECP;

	hr = pML->EnumCodePages( 0, &cpECP );
	if( FAILED(hr) )
	{
		printf( "error: Can't enumerate codepages.\n" );
		return;
	}

	for(;;)
	{
		ULONG fetched;

		fetched = 0;
		hr = cpECP->Next( 1, &mcpi, &fetched );
		if( FAILED(hr) )
		{
			printf( "error: Can't get next codepage info.\n" );
			return;
		}


		if( hr == S_FALSE )		// if( fetched == 0 )
		{
			break;
		}

		printf( "---\n" );
//		printf( "%S\n", mcpi.wszDescription );
		printf( "%S\n", mcpi.wszWebCharset );
		printf( "%S\n", mcpi.wszHeaderCharset );
		printf( "%S\n", mcpi.wszBodyCharset );
//		printf( "%S\n", mcpi.wszFixedWidthFont );
//		printf( "%S\n", mcpi.wszProportionalFont );
		
		
	}


}