#include "stdafx.h"
#include <SWManager/Swm.h>
#include "NW_Wait.h"
#include "tracelog.h"

#define BUFF_SIZE 0x1000

// ---
static AVP_bool ReadFromFileCallBack( void* pBuffer, AVP_dword nSize, AVP_dword *nOutSize, void* pUserData ) {

  TRACE_THE_PROC( rt_num, _T("static AVP_bool ReadFromFileCallBack()") );

	AVP_bool  bOk = 0;
	if ( pBuffer ) {
		AVP_dword dwcbRead;
		bOk = ::ReadFile( (HANDLE)((SWM_StreamInfo *)pUserData)->pUserData, pBuffer, nSize, &dwcbRead, NULL );
		if ( nOutSize )
			*nOutSize = dwcbRead;
	}
	return bOk;
}


// ---
DWORD DeserializeFile( const TCHAR *pszSourceFile, const TCHAR *pszDestFileName, bool bReplaceReadOnly ) {

  TRACE_THE_PROC( rt_num, _T("DWORD DeserializeFile()") );

	DWORD nError = 0;

	::SWM_Init_Library( malloc, free ); 

	void *pManager = SWM_Create_Manager();

	::SWM_Register_CRCInChecker( pManager );
	::SWM_Register_Unpacker( pManager );
	::SWM_Register_Decrypter( pManager );
	
	HANDLE hInFile;
	HANDLE hOutFile;
	DWORD dwFileSize;
	DWORD dwcbRead;

  hInFile = ::CreateFile( pszSourceFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL );

	if ( hInFile != INVALID_HANDLE_VALUE ) {

    DWORD dest_attr = ::GetFileAttributes( pszDestFileName );
		if ( bReplaceReadOnly && (dest_attr & FILE_ATTRIBUTE_READONLY) )
			::SetFileAttributes( pszDestFileName, (dest_attr & ~FILE_ATTRIBUTE_READONLY) ); 

    hOutFile = INVALID_HANDLE_VALUE;
    for( int i=0; i<10 && hOutFile == INVALID_HANDLE_VALUE; i++ ) {
		  hOutFile = ::CreateFile( pszDestFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_RANDOM_ACCESS, NULL );
      if ( hOutFile == INVALID_HANDLE_VALUE )
        ::Condition_Wait( 200, 0, 0 );
    } 

		if ( hOutFile != INVALID_HANDLE_VALUE ) {

			char *pBuffer = new char[BUFF_SIZE];

			SWM_Info rcInInfo = { ReadFromFileCallBack };
			SWM_Info rcOutInfo;
			void *pHandle = ::SWM_Prepare_To_Read( pManager, &rcInInfo, &rcOutInfo );

			SWM_StreamInfo rcStreamInfo = { pHandle, hInFile };

			dwFileSize = ::GetFileSize( hInFile, NULL );

			int nResult = 1;
			dwcbRead = BUFF_SIZE;
			while( nResult && dwcbRead == BUFF_SIZE ) {

				nResult = rcOutInfo.m_pProc( pBuffer, dwcbRead, &dwcbRead, (void *)&rcStreamInfo );

				if ( nResult ) {
					DWORD dwcbWritten;
					nResult = (::WriteFile(hOutFile, pBuffer, dwcbRead, &dwcbWritten, NULL) &&
											dwcbWritten == dwcbRead);
				}
			}

      if ( !nResult )
			  nError = ::GetLastError();

			::CloseHandle( hOutFile );

			::SWM_Shut_Down_Library( pHandle );

			delete [] pBuffer;
		}
		else
			nError = ::GetLastError();

		if ( bReplaceReadOnly && (dest_attr & FILE_ATTRIBUTE_READONLY) )
			::SetFileAttributes( pszDestFileName, dest_attr ); 

		::CloseHandle( hInFile );
	}
	else
		nError = ::GetLastError();


	::SWM_Shut_Down_Library( pManager );

	return nError;
}
