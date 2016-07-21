#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <stuff/pathstr.h>
#include <stuffio/xfile.h>
#include <stuffio/ioutil.h>


// ---
IOS_ERROR_CODE IOSMoveFileOnReboot9x( LPCTSTR pszSrcName, LPCTSTR pszDestName ) {
	CPathStr pSrcPath( _MAX_PATH );
	CPathStr pDestPath( _MAX_PATH );
	::GetShortPathName( pszSrcName, pSrcPath, _MAX_PATH );

	IOS_ERROR_CODE dwLastError = IOS_ERR_OK;

	if ( pszDestName && _tcsnextc(pszDestName) != L'\0' ) {
	  if ( _taccess(pszDestName, 0) ) {
			HANDLE hFile = ::CreateFile( pszDestName, GENERIC_READ | GENERIC_WRITE, 0, NULL, FIOCD_CREATE_ALWAYS, 
										 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL );

			if ( hFile == INVALID_HANDLE_VALUE ) {
				return ::Win32ErrToIOSErr( ::GetLastError() );
			}	
			::CloseHandle( hFile );
		}
	  ::GetShortPathName( pszDestName, pDestPath, _MAX_PATH );
	}

	CHAR szRenameLine[1024];   
	 
  int cchRenameLine = wsprintfA( szRenameLine, "%hs=%hs\r\n", (pszDestName == NULL || !*pszDestName) ? _T("NUL") : LPCTSTR(pDestPath), LPCTSTR(pSrcPath) );

  CHAR szRenameSec[] = "[RENAME]\r\n";
  int cchRenameSec = sizeof(szRenameSec) / sizeof(szRenameSec[0]) - 1;
  HANDLE hFile, hFileMap;
  DWORD dwFileSize, dwRenameLinePos;
  TCHAR szPathnameWinInit[_MAX_PATH];

  // Construct the full pathname of the WININIT.INI file.
  ::GetWindowsDirectory( szPathnameWinInit, _MAX_PATH );
  _tcscat( szPathnameWinInit, _T("\\WinInit.Ini") );

  // Open/Create the WININIT.INI file.
  hFile = ::CreateFile( szPathnameWinInit,      
					 GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 
					 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL );

  if ( hFile == INVALID_HANDLE_VALUE ) {
		return ::Win32ErrToIOSErr( ::GetLastError() );
	}	

  // Create a file mapping object that is the current size of 
  // the WININIT.INI file plus the length of the additional string
  // that we're about to insert into it plus the length of the section
  // header (which we might have to add).
  dwFileSize = ::GetFileSize( hFile, NULL );
  hFileMap = ::CreateFileMapping( hFile, NULL, PAGE_READWRITE, 0, 
																	dwFileSize + cchRenameLine + cchRenameSec, NULL );

  if ( hFileMap != NULL ) {

     // Map the WININIT.INI file into memory.  Note: The contents 
     // of WININIT.INI are always ANSI; never Unicode.
		LPSTR pszWinInit = (LPSTR)::MapViewOfFile( hFileMap, FILE_MAP_WRITE, 0, 0, 0 );

    if (pszWinInit != NULL) {

			strupr( pszWinInit );

      // Search for the [Rename] section in the file.
      LPSTR pszRenameSecInFile = strstr( pszWinInit, szRenameSec );

      if (pszRenameSecInFile == NULL) {

         // There is no [Rename] section in the WININIT.INI file.
         // We must add the section too.
         dwFileSize += wsprintfA( &pszWinInit[dwFileSize], "%s", szRenameSec );
         dwRenameLinePos = dwFileSize;
      } 
			else {

         // We found the [Rename] section, shift all the lines down
         PSTR pszFirstRenameLine = strchr( pszRenameSecInFile, '\n' );
         // Shift the contents of the file down to make room for 
         // the newly added line.  The new line is always added 
         // to the top of the list.
         pszFirstRenameLine++;   // 1st char on the next line
         memmove( pszFirstRenameLine + cchRenameLine, pszFirstRenameLine, 
									pszWinInit + dwFileSize - pszFirstRenameLine);                  
         dwRenameLinePos = (DWORD)(pszFirstRenameLine - pszWinInit);
      }

      // Insert the new line
      memcpy( &pszWinInit[dwRenameLinePos], szRenameLine, cchRenameLine );

      ::UnmapViewOfFile( pszWinInit );

      // Calculate the true, new size of the file.
      dwFileSize += cchRenameLine;

      // Everything was successful.
    }

		::CloseHandle( hFileMap );
  }
	else {
		dwLastError = ::Win32ErrToIOSErr( ::GetLastError() );
	}
	
  // Force the end of the file to be the calculated, new size.
  ::SetFilePointer( hFile, dwFileSize, NULL, FILE_BEGIN );
  ::SetEndOfFile( hFile );

  ::CloseHandle( hFile );

	return dwLastError;
}


