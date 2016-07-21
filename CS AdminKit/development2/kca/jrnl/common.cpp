/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	common.cpp
 * \author	Mikhail Karmazine
 * \date	22.04.2003 13:49:55
 * \brief	
 * 
 */

#include <stdio.h>
#include "common.h"

using namespace std;

char* fgets_bin_win32( char *string, int n, FILE *stream, bool* pbDelimFound /* = NULL */ )
{
    char* pszResult = fgets( string, n, stream );
    bool bDelimFound = false;

    if( pszResult )
    {
        char* pEndl = strstr( string, JRNL_DELIM );
        if( pEndl )
        {
            bDelimFound = true;
            *pEndl = 0;
        }
    }

    if( pbDelimFound )
    {
        *pbDelimFound = bDelimFound;
    }

    return pszResult;
}

std::wstring GenerateTempJournalName( const std::wstring & wstrSourceName )
{
    wstring wstrCentralFileDir;
    wstring wstrCentralFileName;
    wstring wstrCentralFileExt;

    KLSTD_SplitPath( wstrSourceName, wstrCentralFileDir, wstrCentralFileName, wstrCentralFileExt );

    wstring wstrResult;

    int nCounter = 0;
    bool bStop = false;
    while( ! bStop )
    {
        basic_stringstream<wchar_t> ssFileName;
        ssFileName << wstrCentralFileName << L"_temp";
        if( nCounter )
        {
            ssFileName << L"_" << nCounter;
        }
        ssFileName << wstrCentralFileExt;

        KLSTD_PathAppend(
            wstrCentralFileDir,
            ssFileName.str(),
            wstrResult,
            true
        );

        bStop = ! KLSTD_IfExists( wstrResult.c_str() );
        nCounter++;
    }

    return wstrResult;
}
