#include <Stuff\cpointer.h>
#include <StuffIO\xfile.h>


#if !defined(_UNICODE)
#define _wcsnextc(_cpc) ((unsigned int) *(_cpc))
#define _wcsdec(_cpc1, _cpc2) ((_cpc1)>=(_cpc2) ? NULL : (_cpc2)-1)
#define _wcsninc(_pc, _sz) (((_pc)+(_sz)))
#define _wcsinc(_pc)    ((_pc)+1)
_CRTIMP wchar_t*  __cdecl wcscat(wchar_t *, const wchar_t *);
#endif

static IOS_ERROR_CODE  PureFileExtract( const wchar_t * pszFullFileName, wchar_t * pszFileName, uint uicchBufferLen );
static IOS_ERROR_CODE  FileExtract( const wchar_t * pszFullFileName, wchar_t * pszFileName, uint uicchBufferLen );
static wchar_t			  *FindFileExtention( const wchar_t *pszFileName, bool realExt );
static IOS_ERROR_CODE  RootDirExtract( const wchar_t * pszFileName, wchar_t * pszDirName, uint uicchBufferLen );
static IOS_ERROR_CODE	 DirExtract( const wchar_t * pszFileName, wchar_t * pszDirName, uint uicchBufferLen );

static DWORD dwAllocationGranularity = 0;

#if !defined(_UNICODE)
#include <Stuff\CPointer.h>

static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

// ---
static char *UnicodeToMbcs( const wchar_t *pSourceStr ) {
	DWORD dwSize = WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, (wchar_t *)pSourceStr, -1, NULL, 0, NULL, NULL );
	char *pDestStr = new char[dwSize];
	::WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, (wchar_t *)pSourceStr, -1, pDestStr, dwSize, NULL, NULL );
	return pDestStr;
}

// ---
static void MbcsToUnicode( char *pSourceStr, wchar_t *pDestStr, DWORD dwDestStrSize ) {
	::MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pSourceStr, -1, pDestStr, dwDestStrSize );
}
#endif


// ---
static wchar_t* DuplicateString( const wchar_t* s ) {
  return s ? wcscpy( new wchar_t[wcslen(s) + 1], s ) : 0;
}


// ---
static wchar_t* DuplicateFileName( const wchar_t* s ) {
  wchar_t *newName = ::DuplicateString( s );
  if ( newName ) {
    while ( _wcsnextc(newName) != L'\0' && _wcsnextc(newName) == L' ' ) 
      wcscpy( newName, _wcsinc(newName) );
    wchar_t *p = _wcsninc(newName, wcslen(newName) - 1);
    while ( p && p != newName && _wcsnextc(p) == L' ' ) {
      *p = L'\0'; 
			p = _wcsdec(newName, p);
		}
  }
  return newName;
}


//---
static void copyMem( void * pDst, void * pSrc, uint32 dwSize ) {
  switch ( dwSize ) {
    case 1:  
      *((uint8 *)pDst) = *((uint8 *)pSrc);   
      break;
    
    case 2:  
      *((uint16 *)pDst) = *((uint16 *)pSrc);   
      break;
    
    case 4:  
      *((uint32 *)pDst) = *((uint32 *)pSrc); 
      break;
    
    case 8:  
      *((uint32 *)pDst) = *((uint32 *)pSrc); 
      *(((uint32 *)pDst)+1) = *(((uint32 *)pSrc)+1); 
      break;
    
    default:
      ::CopyMemory( pDst, pSrc, dwSize );
      break;
  }
}


static const wchar_t *disableFileName[] = {
  L"PRN", 
  L"LPT1", L"LPT2", L"LPT3", L"LPT4", L"LPT5", L"LPT6", L"LPT7", L"LPT8", L"LPT9",
  L"COM1", L"COM2", L"COM3", L"COM4",
  L"AUX",
  NULL
};

// ---
static bool IsDisableName( const wchar_t *pszFileName ) {
  for ( int i=0; disableFileName[i]; i++ ) {
    if ( !_wcsicmp(disableFileName[i], pszFileName) )
      return true;
  }    
  return false;    
}

//---
XFileW::XFileW( LPCWSTR pszFileName, uint32 dwfAccessMode ) :
        m_dwfAccessMode ( dwfAccessMode ),
        m_dwcbPosition ( 0 ),
        m_dwcbFileOffset ( 0 ),
  			m_hFileMapping ( 0 ),
  			m_pvBaseAddress ( 0 ),
				m_szFileName( NULL ),
        m_hFile( INVALID_HANDLE_VALUE ) {

	if ( pszFileName && *pszFileName ) {
		uint32  dwfAttr;
		uint32  dwfAccess;
		uint32  dwfCreation;
		uint32  dwfShare;

		::IOFlagsToWin32Flags( dwfAccessMode, &dwfAccess, &dwfShare, &dwfCreation, &dwfAttr );

		Init( pszFileName, dwfAccess, dwfShare, NULL, dwfCreation, dwfAttr, NULL );
	}
	else
		::IOSSetLastError( IOS_ERR_INVALID_FILE_NAME );
}


//---
XFileW::XFileW( LPCWSTR  pszFileName,        // pointer to name of the file
						  DWORD  dwDesiredAccess,     // access (read-write) mode
						  DWORD  dwShareMode,					// share mode
						  SECURITY_ATTRIBUTES FAR*  lpSecurityAttributes,     // pointer to security descriptor
						  DWORD  dwCreationDisposition,      // how to create
						  DWORD  dwFlagsAndAttributes,        // file attributes
							HANDLE  hTemplateFile ) :						 // handle to file with attributes to copy
        m_dwfAccessMode ( dwDesiredAccess >> 28 ),
        m_dwcbPosition ( 0 ),
        m_dwcbFileOffset ( 0 ),
  			m_hFileMapping ( 0 ),
  			m_pvBaseAddress ( 0 ),
				m_szFileName( NULL ),
        m_hFile( INVALID_HANDLE_VALUE ) {

	if ( pszFileName && *pszFileName ) 
		Init( pszFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,
					dwFlagsAndAttributes, hTemplateFile );
	else
		::IOSSetLastError( IOS_ERR_INVALID_FILE_NAME );
}		                

// ---
void XFileW::Init( LPCWSTR  lpFileName,        // pointer to name of the file
									DWORD  dwDesiredAccess,     // access (read-write) mode
									DWORD  dwShareMode,					// share mode
									SECURITY_ATTRIBUTES FAR*  lpSecurityAttributes,     // pointer to security descriptor
									DWORD  dwCreationDisposition,      // how to create
									DWORD  dwFlagsAndAttributes,        // file attributes
									HANDLE  hTemplateFile ) {						 // handle to file with attributes to copy

  uint32  dwcbToMap;
  uint32  dwWin32Error;

  m_szFileName = ::DuplicateFileName( lpFileName );

  IOS_ERROR_CODE nameExtrErr = ::PureFileExtract( m_szFileName, m_szFileName, (uint)wcslen(m_szFileName) + 1 );
  if ( nameExtrErr == IOS_ERR_OK ) {
    if ( ::IsDisableName(m_szFileName) ) {
      ::IOSSetLastError( IOS_ERR_INVALID_FILE_NAME );
      return;
    }  
  }
  else {
    ::IOSSetLastError( nameExtrErr );
    return;
  }  

	if ( m_szFileName )
		delete [] m_szFileName;
  
  m_szFileName = ::DuplicateFileName( lpFileName );

  ::IOSSetLastError( IOS_ERR_OK );
  
  if ( !dwAllocationGranularity ) {
    SYSTEM_INFO sysInfo;
    ::GetSystemInfo( &sysInfo );
    dwAllocationGranularity = sysInfo.dwAllocationGranularity;
  }
  
  m_dwcbMapSize = 2 * dwAllocationGranularity;
  
  if ( !(m_dwfAccessMode & FIOAM_WRITE) ) { // read only
    
    // now try to open a file
#if defined(_UNICODE)
    m_hFile = ::CreateFile( lpFileName, dwDesiredAccess,//GENERIC_READ, 
														dwShareMode, lpSecurityAttributes, dwCreationDisposition, 
														dwFlagsAndAttributes, hTemplateFile );
#else
		if ( g_bUnicodePlatform )
			m_hFile = ::CreateFileW( lpFileName, dwDesiredAccess,//GENERIC_READ, 
															 dwShareMode, lpSecurityAttributes, dwCreationDisposition, 
															 dwFlagsAndAttributes, hTemplateFile );
		else {
			CAPointer<char> pConverted = ::UnicodeToMbcs( lpFileName );
			m_hFile = ::CreateFileA( pConverted, dwDesiredAccess,//GENERIC_READ, 
																dwShareMode, lpSecurityAttributes, dwCreationDisposition, 
																dwFlagsAndAttributes, hTemplateFile );
		}
#endif

    if( m_hFile == INVALID_HANDLE_VALUE ) {
      ::Win32ErrToIOSErr( ::GetLastError() );
      return;
    }

    // create file mapping
#if defined(_UNICODE)
    m_hFileMapping = ::CreateFileMapping( m_hFile, lpSecurityAttributes, 
																					PAGE_READONLY, 0, 0, NULL );
#else
		if ( g_bUnicodePlatform )
			m_hFileMapping = ::CreateFileMappingW( m_hFile, lpSecurityAttributes, 
																						 PAGE_READONLY, 0, 0, NULL );
		else
			m_hFileMapping = ::CreateFileMappingA( m_hFile, lpSecurityAttributes, 
																						 PAGE_READONLY, 0, 0, NULL );
#endif
			
    if( m_hFileMapping == NULL ) {
      dwWin32Error = ::GetLastError();
      ::CloseHandle(m_hFile);
      ::Win32ErrToIOSErr( dwWin32Error );
			m_hFile = INVALID_HANDLE_VALUE;
      return;
    }

    m_dwcbFileSize = ::GetFileSize( m_hFile, NULL );
    dwcbToMap = min( m_dwcbMapSize, m_dwcbFileSize );  

    m_pvBaseAddress = ::MapViewOfFile(m_hFileMapping, FILE_MAP_READ, 0, 0, dwcbToMap); 

    if ( m_pvBaseAddress == NULL ) {
      dwWin32Error = ::GetLastError();
      ::CloseHandle(m_hFileMapping);
      ::CloseHandle(m_hFile);
      ::Win32ErrToIOSErr( dwWin32Error );
			m_hFile = INVALID_HANDLE_VALUE;
      return;
    }

  }
  else { // read-write access requested
    // если открывается файл на сетевом устройстве,
    // открываем его с флагом FILE_FLAG_WRITE_THROUGH,
    // чтобы гарантировать запись на устройство
    CAPointer<wchar_t> pszRootDirName = new wchar_t[_MAX_PATH]; 
    if ( ::RootDirExtract( lpFileName, pszRootDirName, _MAX_PATH ) != IOS_ERR_OK )
      return;

#if defined(_UNICODE)
    if ( ::GetDriveType(pszRootDirName) == DRIVE_REMOTE	)
      dwFlagsAndAttributes |= FILE_FLAG_WRITE_THROUGH;
#else
		if ( g_bUnicodePlatform )	{
			if ( ::GetDriveTypeW(pszRootDirName) == DRIVE_REMOTE	)
				dwFlagsAndAttributes |= FILE_FLAG_WRITE_THROUGH;
		}
		else {
			CAPointer<char> pConverted = ::UnicodeToMbcs( pszRootDirName );
			if ( ::GetDriveTypeA(pConverted) == DRIVE_REMOTE	)
				dwFlagsAndAttributes |= FILE_FLAG_WRITE_THROUGH;
		}
#endif
      
    // now try to open a file
#if defined(_UNICODE)
    m_hFile = ::CreateFile( lpFileName, dwDesiredAccess | GENERIC_READ,//GENERIC_READ | GENERIC_WRITE, 
														dwShareMode, lpSecurityAttributes, dwCreationDisposition, 
														dwFlagsAndAttributes, hTemplateFile );
#else
		if ( g_bUnicodePlatform )
			m_hFile = ::CreateFileW( lpFileName, dwDesiredAccess | GENERIC_READ,//GENERIC_READ | GENERIC_WRITE, 
																dwShareMode, lpSecurityAttributes, dwCreationDisposition, 
																dwFlagsAndAttributes, hTemplateFile );
		else {
			CAPointer<char> pConverted = ::UnicodeToMbcs( lpFileName );
			m_hFile = ::CreateFileA( pConverted, dwDesiredAccess | GENERIC_READ,//GENERIC_READ | GENERIC_WRITE, 
																dwShareMode, lpSecurityAttributes, dwCreationDisposition, 
																dwFlagsAndAttributes, hTemplateFile );
		}
#endif

    if ( m_hFile == INVALID_HANDLE_VALUE ) {
      ::Win32ErrToIOSErr( ::GetLastError() );
      return;
    }

    // файл мог уже существовать
    m_dwcbFileSize = ::GetFileSize( m_hFile, NULL );
//    m_dwcbFileSize = 0;

    // create file mapping
#if defined(_UNICODE)
    m_hFileMapping = ::CreateFileMapping( m_hFile, lpSecurityAttributes, 
																					PAGE_READWRITE, 0, m_dwcbMapSize, NULL );
#else
		if ( g_bUnicodePlatform )
			m_hFileMapping = ::CreateFileMappingW( m_hFile, lpSecurityAttributes, 
																						 PAGE_READWRITE, 0, m_dwcbMapSize, NULL );
		else
			m_hFileMapping = ::CreateFileMappingA( m_hFile, lpSecurityAttributes, 
																						 PAGE_READWRITE, 0, m_dwcbMapSize, NULL );
#endif
    if ( m_hFileMapping == NULL ) {
      dwWin32Error = ::GetLastError();
      ::CloseHandle( m_hFile );
      ::Win32ErrToIOSErr( dwWin32Error );
			m_hFile = INVALID_HANDLE_VALUE;
      return;
    }

    m_pvBaseAddress = ::MapViewOfFile(m_hFileMapping, FILE_MAP_WRITE, 0, 0, m_dwcbMapSize); 

    if ( m_pvBaseAddress == NULL ) {
      dwWin32Error = ::GetLastError();
      ::CloseHandle( m_hFileMapping );
      ::CloseHandle( m_hFile );
			m_hFile = INVALID_HANDLE_VALUE;
      ::Win32ErrToIOSErr( dwWin32Error );
      return;
    }

  }
}

//---
XFileW::~XFileW() {
  ::IOSSetLastError( IOS_ERR_OK );

  if( m_hFile != INVALID_HANDLE_VALUE ) {
    // unmap file view to update the disk image
    if ( m_pvBaseAddress != NULL ) {
      if ( !::UnmapViewOfFile(m_pvBaseAddress) ) 
        ::Win32ErrToIOSErr( ::GetLastError() );
    }

    // close file mapping
    if ( m_hFileMapping != NULL ) {
      if ( !::CloseHandle(m_hFileMapping) ) 
        ::Win32ErrToIOSErr( ::GetLastError() );
    }

    // set end of file according to the current file size
    if ( m_dwfAccessMode & FIOAM_WRITE ) {
      SetFilePointer( m_hFile, (long)m_dwcbFileSize, 0, FILE_BEGIN );
      if ( !SetEndOfFile(m_hFile) ) 
        ::Win32ErrToIOSErr( ::GetLastError() );
    }

    // close file
    ::CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
  }

	if ( m_szFileName )
		delete [] m_szFileName;
}



//---
IOS_ERROR_CODE XFileW::UpdateFileMapping( void ) {
  long lNewOffset;
  uint32 fdwAccess;
  uint32 dwcbToMap;

  ::IOSSetLastError( IOS_ERR_OK );
  if ( m_dwcbPosition == m_dwcbFileOffset ) 
    return IOS_ERR_OK;

  // position is out of mapping
  if ( !::UnmapViewOfFile(m_pvBaseAddress) ) 
    return ::Win32ErrToIOSErr( ::GetLastError() );

  lNewOffset = (long)(m_dwcbPosition/m_dwcbMapSize)*m_dwcbMapSize;

  if ( (m_dwfAccessMode & FIOAM_WRITE) ) {
    // close file mapping and recreate it again to expand it
    
    if ( !::CloseHandle(m_hFileMapping) ) 
      return ::Win32ErrToIOSErr( ::GetLastError() );

    m_hFileMapping = ::CreateFileMapping(m_hFile, NULL, PAGE_READWRITE, 
                                              0, m_dwcbFileSize + m_dwcbMapSize, 
                                              NULL);
    if ( m_hFileMapping == NULL ) 
      return ::Win32ErrToIOSErr( ::GetLastError() );

    fdwAccess = FILE_MAP_WRITE;
    dwcbToMap = m_dwcbMapSize;
  }
  else {
    fdwAccess = FILE_MAP_READ;
    dwcbToMap = min(m_dwcbMapSize, m_dwcbFileSize - (uint32)lNewOffset);  
  }

  m_pvBaseAddress = ::MapViewOfFile(m_hFileMapping, fdwAccess, 
                                 0, (uint32)lNewOffset, dwcbToMap);

  if ( m_pvBaseAddress == NULL ) 
    return ::Win32ErrToIOSErr( ::GetLastError() );

  m_dwcbFileOffset = (uint32)lNewOffset;
  return IOS_ERR_OK;
}


//---
IOS_ERROR_CODE XFileW::XReadByte( uint8* pb ) {
  uint32 dwRead;
  IOS_ERROR_CODE errCode;

  errCode = XRead( pb, 1, &dwRead );
  if ( errCode != IOS_ERR_OK ) 
    return errCode;
    
  if ( dwRead != 1) 
    return IOS_ERR_NO_MORE_DATA;
    
  return IOS_ERR_OK;
}


//---
IOS_ERROR_CODE XFileW::XReadWord( uint16* pw ) {
  uint32 dwRead;
  IOS_ERROR_CODE errCode;

  errCode = XRead( pw, 2, &dwRead );
  if ( errCode != IOS_ERR_OK) 
    return errCode;
    
  if ( dwRead != 2 ) 
    return IOS_ERR_NO_MORE_DATA;
    
  return IOS_ERR_OK;
}


//---
IOS_ERROR_CODE XFileW::XReadDWord( uint32* pdw ) {
  uint32 dwRead;
  IOS_ERROR_CODE errCode;

  errCode = XRead( pdw, 4, &dwRead );
  if ( errCode != IOS_ERR_OK ) 
    return errCode;
    
  if ( dwRead != 4 ) 
    return IOS_ERR_NO_MORE_DATA;
    
  return IOS_ERR_OK;
}


//---
IOS_ERROR_CODE XFileW::XReadQWord( void* pq ) {
  uint32 dwRead;
  IOS_ERROR_CODE errCode;

  errCode = XRead( pq, 8, &dwRead );
  if ( errCode != IOS_ERR_OK ) 
    return(errCode);
    
  if (dwRead != 8 ) 
    return IOS_ERR_NO_MORE_DATA;
    
  return IOS_ERR_OK;
}


//---
IOS_ERROR_CODE XFileW::XReadString( wchar_t **pszBuff, uint* puiRead ) {
  uint32        dwRead;
  IOS_ERROR_CODE errCode;
  uint32        dwLength;
//  uint32        dwCurr;

	errCode = XReadDWord( &dwLength );
  if ( IOS_ERR_OK != errCode ) 
		return errCode;

	*pszBuff = new wchar_t[dwLength + 1];
	errCode = XRead( *pszBuff, dwLength, &dwRead );

  if ( IOS_ERR_OK != errCode ) 
		return errCode;

	if ( dwLength != dwRead )
      return IOS_ERR_NO_MORE_DATA;

  (*pszBuff)[dwRead] = L'\0';
  if ( puiRead )
    *puiRead = dwRead;
/*
	*pszBuff = new wchar_t[dwLength + 1];
  for ( dwCurr = 0; dwCurr < dwLength; dwCurr++ ) {
    errCode = XRead( (*pszBuff) + dwCurr, 1, &dwRead );
    if ( IOS_ERR_OK != errCode ) {
      if ( 0 == dwRead ) 
        return errCode;
      else 
        break; // last line
		}
    if ( 1 != dwRead ) 
      return IOS_ERR_NO_MORE_DATA;
  }
  if ( dwCurr < dwLength - 1 )
    ++dwCurr;
  (*pszBuff)[dwCurr] = '\0';
  if ( puiRead )
    *puiRead = dwCurr;
*/
  return IOS_ERR_OK;
}


//---
IOS_ERROR_CODE XFileW::XReadLine( wchar_t * pszBuff, uint uicBytes, uint* puiRead ) {
  uint32        dwRead;
  IOS_ERROR_CODE errCode;
  uint          uiRead;

  for ( uiRead = 0; uiRead < uicBytes; ++uiRead ) {
    errCode = XRead( pszBuff + uiRead, 1, &dwRead );
    if ( IOS_ERR_OK != errCode )
      if ( 0 == dwRead ) 
        return errCode;
      else 
        break; // last line
    if ( 1 != dwRead ) 
      return (IOS_ERR_NO_MORE_DATA);
    if ( L'\n' == pszBuff[uiRead] ) 
      break;
  }
  if ( uiRead < uicBytes - 1 )
    ++uiRead;
  pszBuff[uiRead] = L'\0';
  if ( puiRead )
    *puiRead = uiRead;
  return IOS_ERR_OK;
}


//---
IOS_ERROR_CODE XFileW::XWriteByte( uint8* pb ) {
  uint32 dwWritten;
  IOS_ERROR_CODE errCode;

  errCode = XWrite( pb, 1, &dwWritten );
  if ( errCode != IOS_ERR_OK ) 
    return errCode;
    
  if ( dwWritten != 1 ) 
    return IOS_ERR_OUT_OF_DISK_SPACE;
    
  return IOS_ERR_OK;
}


//---
IOS_ERROR_CODE XFileW::XWriteWord( uint16* pw ) {
  uint32 dwWritten;
  IOS_ERROR_CODE errCode;

  errCode = XWrite( pw, 2, &dwWritten );
  if ( errCode != IOS_ERR_OK ) 
    return errCode;
    
  if ( dwWritten != 2 ) 
    return IOS_ERR_NO_MORE_DATA;
    
  return IOS_ERR_OK;
}


//---
IOS_ERROR_CODE XFileW::XWriteDWord( uint32* pdw ) {
  uint32 dwWritten;
  IOS_ERROR_CODE errCode;

  errCode = XWrite( pdw, 4, &dwWritten );
  if ( errCode != IOS_ERR_OK ) 
    return errCode;
    
  if ( dwWritten != 4 ) 
    return IOS_ERR_NO_MORE_DATA;
    
  return IOS_ERR_OK;
}


//---
IOS_ERROR_CODE XFileW::XWriteQWord( void* pq )
{
  uint32 dwWritten;
  IOS_ERROR_CODE errCode;

  errCode = XWrite( pq, 8, &dwWritten );
  if ( errCode != IOS_ERR_OK ) 
    return errCode;
    
  if ( dwWritten != 8 ) 
    return IOS_ERR_NO_MORE_DATA;
    
  return IOS_ERR_OK;
}


//---
IOS_ERROR_CODE XFileW::XWriteString( const wchar_t * psz ) {
  uint32          dwWritten;
  uint32          dwToWrite;
  IOS_ERROR_CODE errCode;

	dwToWrite = psz ? (uint32)wcslen( psz ) : 0;
	errCode = XWriteDWord( &dwToWrite );
	if ( errCode != IOS_ERR_OK )
		return errCode;
	errCode = XWrite( (void *)psz, dwToWrite, &dwWritten );
	if ( errCode != IOS_ERR_OK )
		return errCode;
	if ( dwWritten != dwToWrite ) 
		return IOS_ERR_NO_MORE_DATA;
	return IOS_ERR_OK;
}


//---
IOS_ERROR_CODE XFileW::XWriteLine( const wchar_t * psz ) {
  uint32          dwWritten;
  uint32          dwToWrite;
  IOS_ERROR_CODE errCode;

  dwToWrite = psz ? (uint32)wcslen( psz ) : 0;
  errCode = XWrite( (void *)psz, dwToWrite, &dwWritten );
  if ( errCode != IOS_ERR_OK )
    return errCode;
  if ( dwWritten != dwToWrite ) 
    return IOS_ERR_NO_MORE_DATA;
  return IOS_ERR_OK;
}


//---
IOS_ERROR_CODE XFileW::XLength( uint32* pdwLength ) {
  if ( pdwLength ) {
    *pdwLength = m_dwcbFileSize;
    return IOS_ERR_OK;
  }

  return IOS_ERR_OUT_OF_MEMORY;
}


//---
IOS_ERROR_CODE XFileW::XSetSize( uint32 dwLength ) {
  ::IOSSetLastError( IOS_ERR_OK );

  IOS_ERROR_CODE errCode = UpdateFileMapping();
  if ( errCode != IOS_ERR_OK ) 
    return errCode;

  // unmap file view to update the disk image
  if ( m_pvBaseAddress != NULL ) {
    if ( !::UnmapViewOfFile(m_pvBaseAddress) ) 
      return ::Win32ErrToIOSErr( ::GetLastError() );
  }

  // close file mapping
  if ( m_hFileMapping != NULL ) {
    if ( !::CloseHandle(m_hFileMapping) ) 
      return ::Win32ErrToIOSErr( ::GetLastError() );
  }

  // set eond of file according to the current file size
  m_dwcbFileSize = dwLength;
  SetFilePointer( m_hFile, (long)m_dwcbFileSize, 0, FILE_BEGIN );
  if ( !SetEndOfFile(m_hFile) ) 
    return ::Win32ErrToIOSErr( ::GetLastError() );

  if ( m_dwcbFileOffset > m_dwcbFileSize ) {
    m_dwcbFileOffset  = dwLength;
    m_dwcbPosition = dwLength;
  }
  else
    if ( m_dwcbPosition > m_dwcbFileSize ) 
      m_dwcbPosition = dwLength;

  uint32 fdwAccess;
  uint32 dwcbToMap;
  long lNewOffset = (long)(m_dwcbPosition/m_dwcbMapSize)*m_dwcbMapSize;

  if ( (m_dwfAccessMode & FIOAM_WRITE) ) {
    m_hFileMapping = ::CreateFileMapping( m_hFile, NULL, PAGE_READWRITE, 
                                          0, m_dwcbFileSize + m_dwcbMapSize, NULL );
    if ( m_hFileMapping == NULL ) 
      return ::Win32ErrToIOSErr( ::GetLastError() );

    fdwAccess = FILE_MAP_WRITE;
    dwcbToMap = m_dwcbMapSize;
  }
  else {
    m_hFileMapping = ::CreateFileMapping( m_hFile, NULL, PAGE_READONLY, 0, m_dwcbFileSize, NULL );
    if ( m_hFileMapping == NULL ) 
      return ::Win32ErrToIOSErr( ::GetLastError() );

    fdwAccess = FILE_MAP_READ;
    dwcbToMap = min(m_dwcbMapSize, m_dwcbFileSize - (uint32)lNewOffset);  
  }

  m_pvBaseAddress = ::MapViewOfFile(m_hFileMapping, fdwAccess, 
                                 0, (uint32)lNewOffset, dwcbToMap);

  if ( m_pvBaseAddress == NULL ) 
    return ::Win32ErrToIOSErr( ::GetLastError() );

  return ::IOSGetLastError();
}


//---
IOS_ERROR_CODE XFileW::XRead( void * pvBuffer, uint32 dwcbToRead, uint32 * pdwcbActuallyRead ) {
  ::IOSSetLastError( IOS_ERR_OK );

  uint32 dwcbBlockSize;
  long  lcbLeft;
  IOS_ERROR_CODE errCode;

  if ( dwcbToRead == 0 ) {
    if ( pdwcbActuallyRead != NULL ) 
      *pdwcbActuallyRead = 0;
		::IOSSetLastError( IOS_ERR_OK );
    return ::IOSGetLastError();
  }

  // calculate number of bytes to read so it does not span
  // beyond the file size
//  uint32 dwcbToReadOrg = dwcbToRead;
  
  if ( m_dwcbPosition + dwcbToRead >= m_dwcbFileSize )
    dwcbToRead = m_dwcbFileSize - m_dwcbPosition;

  if ( m_dwcbPosition < m_dwcbFileOffset ||
       m_dwcbPosition > m_dwcbFileOffset + m_dwcbMapSize ) {
    // update file mapping so the position became visible 
    errCode = UpdateFileMapping();
    if ( errCode != IOS_ERR_OK ) 
      return errCode;
  }

  lcbLeft = (long)dwcbToRead;
  while ( lcbLeft > 0 ) {

    if ( m_dwcbPosition + (uint32)lcbLeft > 
         m_dwcbFileOffset + m_dwcbMapSize ) {
      // block to read spans across the mapping boundary forwards

      // calculate how many bytes are visible
      dwcbBlockSize = m_dwcbFileOffset + m_dwcbMapSize - m_dwcbPosition;
      
      // transfer the visible part
      __try {
        // fault is possible - due to bad diskette, or whatever...
        copyMem(pvBuffer, ((uint8 *)m_pvBaseAddress) + m_dwcbPosition - 
                  m_dwcbFileOffset, (uint32)dwcbBlockSize);
      }
      __except ( EXCEPTION_EXECUTE_HANDLER ) {
        // copy fault
				::IOSSetLastError( IOS_ERR_MEDIA_CORRUPTED );
        return ::IOSGetLastError();
      }

      // update file position    
      m_dwcbPosition += dwcbBlockSize;

      // update 'bytes left' counter 
      lcbLeft -= (long)dwcbBlockSize;

      // update buffer position
      pvBuffer = (void *)(((uint8 *)pvBuffer) + dwcbBlockSize);

      // update file mapping 
      errCode = UpdateFileMapping();
      if ( errCode != IOS_ERR_OK ) 
        return errCode;
    } // end of span
    else { 
      // simple data transfer
    
      // transfer the data
      __try {
        // fault is possible - due to bad diskette, or whatever...
        copyMem(pvBuffer, ((uint8 *)m_pvBaseAddress) + 
                m_dwcbPosition - m_dwcbFileOffset, (uint32)lcbLeft);
      }
      __except (EXCEPTION_EXECUTE_HANDLER) {
        // copy fault
				::IOSSetLastError( IOS_ERR_MEDIA_CORRUPTED );
        return ::IOSGetLastError();
      }

      // update file position
      m_dwcbPosition += (uint32)lcbLeft;
      
      lcbLeft = 0;
    }
  } // end of while some bytes left

  if ( pdwcbActuallyRead != NULL ) {
    *pdwcbActuallyRead = dwcbToRead;
  }
/*
  if ( dwcbToRead != dwcbToReadOrg )
    s_LastError = IOS_ERR_NO_MORE_DATA;
*/  
  return ::IOSGetLastError();
}


//---
IOS_ERROR_CODE XFileW::XWrite( void * pvBuffer, uint32 dwcbToWrite, uint32 * pdwcbActuallyWritten ) {
	::IOSSetLastError( IOS_ERR_OK );

  uint32 dwcbBlockSize;
  long lcbLeft;
  IOS_ERROR_CODE errCode;

  if ( dwcbToWrite == 0 ) {
    if ( pdwcbActuallyWritten != NULL ) 
      *pdwcbActuallyWritten = 0;
		::IOSSetLastError( IOS_ERR_OK );
    return ::IOSGetLastError();
  }

  if ( m_dwcbPosition < m_dwcbFileOffset ||
       m_dwcbPosition >= m_dwcbFileOffset + m_dwcbMapSize ) {
    // update file mapping so the position became visible 
    errCode = UpdateFileMapping();
    if ( errCode != IOS_ERR_OK ) 
      return(errCode);
  }

  lcbLeft = (long)dwcbToWrite;
  while ( lcbLeft > 0 ) {

    if ( m_dwcbPosition + (uint32)lcbLeft > 
         m_dwcbFileOffset + m_dwcbMapSize ) {
      // block to write spans across the mapping boundary forwards

      // calculate how many bytes are visible
      dwcbBlockSize = m_dwcbFileOffset + m_dwcbMapSize - 
                      m_dwcbPosition;

      __try {
        copyMem(((uint8 *)m_pvBaseAddress) + m_dwcbPosition - 
                   m_dwcbFileOffset, pvBuffer, (uint32)dwcbBlockSize);
      }
      __except (EXCEPTION_EXECUTE_HANDLER) {
        // copy fault
				::IOSSetLastError( IOS_ERR_MEDIA_CORRUPTED );
        return ::IOSGetLastError();
      }
    
      // update file position    
      m_dwcbPosition += dwcbBlockSize;
      m_dwcbFileSize = max(m_dwcbFileSize, m_dwcbPosition);

      // update 'bytes left' counter 
      lcbLeft -= (long)dwcbBlockSize;

      // update buffer position
      pvBuffer = (void *)(((uint8 *)pvBuffer) + dwcbBlockSize);

      // update file mapping 
      errCode = UpdateFileMapping();
      if ( errCode != IOS_ERR_OK) 
        return errCode;
    } // end of span
    else { 
      // simple data transfer
    
      // transfer the data
      __try {
        copyMem(((uint8 *)m_pvBaseAddress) + m_dwcbPosition - 
                    m_dwcbFileOffset, pvBuffer, (uint32)lcbLeft);
      }
      __except (EXCEPTION_EXECUTE_HANDLER) {
        // copy fault
				::IOSSetLastError( IOS_ERR_MEDIA_CORRUPTED );
        return ::IOSGetLastError();
      }

      // update file position
      m_dwcbPosition += (uint32)lcbLeft;
      m_dwcbFileSize = max(m_dwcbFileSize, m_dwcbPosition);
      
      lcbLeft = 0;
    }
  } // end of while some bytes left
  
  if ( pdwcbActuallyWritten != NULL ) {
    *pdwcbActuallyWritten = dwcbToWrite;
  }
  
  return ::IOSGetLastError();
}


//---
IOS_ERROR_CODE XFileW::XSeek( long lDistance, IOS_SEEK_ORIGIN eSeekOrigin, uint32 * pdwNewPosition ) {
	::IOSSetLastError( IOS_ERR_OK );

  uint32 dwNewPosition;

  long lPosition;

  switch ( eSeekOrigin ) {
    case FIOSO_START:
      if ( lDistance < 0 ) 
        lDistance = 0;
      dwNewPosition = (uint32)lDistance;
      if ( (m_dwfAccessMode & FIOAM_WRITE) && 
           dwNewPosition > m_dwcbFileSize) {
				::IOSSetLastError( IOS_ERR_SEEK_ERROR );
        return ::IOSGetLastError();
      }
      break;

    case FIOSO_CURRENT:
      lPosition = (long)m_dwcbPosition + lDistance;
      if ( lPosition < 0 ) 
        lPosition = 0;
      
      dwNewPosition = (uint32)lPosition;
      if ( !(m_dwfAccessMode & FIOAM_WRITE) && 
            dwNewPosition > m_dwcbFileSize ) {
				::IOSSetLastError( IOS_ERR_SEEK_ERROR );
        return ::IOSGetLastError();
      }  
      break;

    case FIOSO_END:
      lPosition = (long)m_dwcbFileSize + lDistance;
      if ( lPosition < 0 ) 
        lPosition = 0;
      dwNewPosition = (uint32)lPosition;  
      if ( !(m_dwfAccessMode & FIOAM_WRITE) && 
            dwNewPosition > m_dwcbFileSize ) {
				::IOSSetLastError( IOS_ERR_SEEK_ERROR );
        return ::IOSGetLastError();
      }
      break;

    default:
			::IOSSetLastError( IOS_ERR_SEEK_ERROR );
      return ::IOSGetLastError();
  }

  m_dwcbPosition = dwNewPosition;    
  if ( pdwNewPosition != NULL ) 
    *pdwNewPosition = m_dwcbPosition;
  
  return ::IOSGetLastError();
}


//---
bool XFileW::XEof() {
  uint32          dwCurPos;
  IOS_ERROR_CODE  CSErrCode;
  uint32          dwLength;

  CSErrCode = XTell( &dwCurPos );
  if ( CSErrCode != IOS_ERR_OK )
    return true;

  CSErrCode = XSeek( 0L, FIOSO_END, &dwLength );
  XSeek( dwCurPos, FIOSO_START, NULL );

  if ( CSErrCode != IOS_ERR_OK || (dwLength - dwCurPos) == 0 ) 
    return true;

  return false;
}

//---
bool XFileW::XSetTime( const FILETIME *  lpftCreation,	  // time the file was created 
    									const FILETIME *  lpftLastAccess,	// time the file was last accessed 
    									const FILETIME *  lpftLastWrite 	// time the file was last written 
                    ) {

  return !!::SetFileTime( m_hFile, lpftCreation, lpftLastAccess, lpftLastWrite );
}


#if defined(UNICODE) || defined(_UNICODE)
//---
IOS_ERROR_CODE XFileW::XReadString( char **pszBuff, uint* puiRead ) {
  uint32        dwRead;
  IOS_ERROR_CODE errCode;
  uint32        dwLength;
//  uint32        dwCurr;

	errCode = XReadDWord( &dwLength );
  if ( IOS_ERR_OK != errCode ) 
		return errCode;

	*pszBuff = new char[dwLength + 1];
	errCode = XRead( *pszBuff, dwLength, &dwRead );

  if ( IOS_ERR_OK != errCode ) 
		return errCode;

	if ( dwLength != dwRead )
      return IOS_ERR_NO_MORE_DATA;

  (*pszBuff)[dwRead] = '\0';
  if ( puiRead )
    *puiRead = dwRead;

  return IOS_ERR_OK;
}


//---
IOS_ERROR_CODE XFileW::XReadLine( char * pszBuff, uint uicBytes, uint* puiRead ) {
  uint32        dwRead;
  IOS_ERROR_CODE errCode;
  uint          uiRead;

  for ( uiRead = 0; uiRead < uicBytes; ++uiRead ) {
    errCode = XRead( pszBuff + uiRead, 1, &dwRead );
    if ( IOS_ERR_OK != errCode )
      if ( 0 == dwRead ) 
        return errCode;
      else 
        break; // last line
    if ( 1 != dwRead ) 
      return (IOS_ERR_NO_MORE_DATA);
    if ( L'\n' == pszBuff[uiRead] ) 
      break;
  }
  if ( uiRead < uicBytes - 1 )
    ++uiRead;
  pszBuff[uiRead] = L'\0';
  if ( puiRead )
    *puiRead = uiRead;
  return IOS_ERR_OK;
}


//---
IOS_ERROR_CODE XFileW::XWriteString( const char * psz ) {
  uint32          dwWritten;
  uint32          dwToWrite;
  IOS_ERROR_CODE errCode;
	
	dwToWrite = psz ? (uint32)strlen( psz ) : 0;
	errCode = XWriteDWord( &dwToWrite );
	if ( errCode != IOS_ERR_OK )
		return errCode;
	errCode = XWrite( (void *)psz, dwToWrite, &dwWritten );
	if ( errCode != IOS_ERR_OK )
		return errCode;
	if ( dwWritten != dwToWrite ) 
		return IOS_ERR_NO_MORE_DATA;
	return IOS_ERR_OK;
}


//---
IOS_ERROR_CODE XFileW::XWriteLine( const char * psz ) {
  uint32          dwWritten;
  uint32          dwToWrite;
  IOS_ERROR_CODE errCode;
	
  dwToWrite = psz ? (uint32)strlen( psz ) : 0;
  errCode = XWrite( (void *)psz, dwToWrite, &dwWritten );
  if ( errCode != IOS_ERR_OK )
    return errCode;
  if ( dwWritten != dwToWrite ) 
    return IOS_ERR_NO_MORE_DATA;
  return IOS_ERR_OK;
}


#endif 



// Utility functions
//---
static IOS_ERROR_CODE PureFileExtract( const wchar_t * pszFullFileName, wchar_t * pszFileName, uint uicchBufferLen ) {
  if ( pszFullFileName && pszFileName ) {
    CAPointer<wchar_t> pszFile = new wchar_t[_MAX_FNAME];
    if ( ::FileExtract(pszFullFileName, pszFile, _MAX_FNAME) == IOS_ERR_OK ) {
      wchar_t *psExt = ::FindFileExtention( pszFile, false );
      if ( psExt )
        *psExt = L'\0';
      if ( uint(wcslen(pszFile)) < uicchBufferLen ) {
        wcsncpy( pszFileName, pszFile, uicchBufferLen );
        return IOS_ERR_OK;
      }  
    }
    return IOS_ERR_OUT_OF_MEMORY;  
  }
  return IOS_ERR_INVALID_FILE_NAME;
}

//---
static IOS_ERROR_CODE FileExtract( const wchar_t * pszFullFileName, wchar_t * pszFileName, uint uicchBufferLen ) {
  if ( pszFullFileName && pszFileName ) {

    CAPointer<wchar_t> fFile = new wchar_t[_MAX_FNAME];
    CAPointer<wchar_t> fExt  = new wchar_t[_MAX_EXT];

  // выделить диск и каталог
    _wsplitpath( pszFullFileName, NULL, NULL, fFile, fExt);

    if ( uint(wcslen(fFile) + wcslen(fExt)) < uicchBufferLen ) {
      wcsncpy( pszFileName, fFile, uicchBufferLen );
      uint uichLen = (uint)wcslen( pszFileName );
      pszFileName = _wcsninc(pszFileName, uichLen);
      wcsncpy( pszFileName, fExt, uicchBufferLen - uichLen );
    }  
    else
      return IOS_ERR_OUT_OF_MEMORY;  

    return IOS_ERR_OK;
  }
  return IOS_ERR_INVALID_FILE_NAME;
}


// ---
static wchar_t * FindFileExtention( const wchar_t *pszFileName, bool realExt ) {
  if ( pszFileName && *pszFileName ) {
    wchar_t *ext = _wcsninc((wchar_t *)pszFileName, wcslen(pszFileName) - 1);
    while ( ext && ext != pszFileName ) {
      if ( _wcsnextc(ext) == L'.' )
        return _wcsninc(ext, !!realExt);
			int nNext = _wcsnextc(ext);
      if ( nNext == L'\\' || nNext == L'/' )
        break;
      ext = _wcsdec( pszFileName, ext );
    }
  }
  return NULL;
}

//---
static IOS_ERROR_CODE DirExtract( const wchar_t * pszFileName, wchar_t * pszDirName, uint uicchBufferLen ) {
  if ( pszFileName && pszDirName ) {
		
    CAPointer<wchar_t> fDrive = new wchar_t[_MAX_DRIVE];
    CAPointer<wchar_t> fDir   = new wchar_t[_MAX_DIR];
		
		// выделить диск и каталог
    _wsplitpath( pszFileName, fDrive, fDir, NULL, NULL);
    int dLen = (int)wcslen( fDir );
		wchar_t *pLast;
		
    if ( dLen > 1 && 
			(pLast = _wcsninc(fDir, dLen - 1)) && 
			(_wcsnextc(pLast) == L'\\') )
      *pLast = L'\0';
		
    if ( (size_t)(wcslen(fDrive) + wcslen(fDir)) < uicchBufferLen ) {
			int uichLen;
      wcsncpy( pszDirName, fDrive, uicchBufferLen );
      uichLen = (int)wcslen(pszDirName);
      pszDirName = _wcsninc( pszDirName, uichLen );
      wcsncpy( pszDirName, fDir, uicchBufferLen - uichLen );
    }  
    else
      return IOS_ERR_OUT_OF_MEMORY;  
		
    return IOS_ERR_OK;
  }
  return IOS_ERR_INVALID_FILE_NAME;
}

//---
static IOS_ERROR_CODE RootDirExtract( const wchar_t * pszFileName, wchar_t * pszDirName, uint uicchBufferLen ) {

  IOS_ERROR_CODE fError = ::DirExtract( pszFileName, pszDirName, uicchBufferLen );

  if ( fError == IOS_ERR_OK ) {
    if ( _wcsnextc(pszDirName) == L'\\' ) {
    // новая нотация \\netdrive\drive\dir\..
      wchar_t *psEndOfDrive = wcschr( _wcsninc(pszDirName, 2), L'\\' );
      if ( psEndOfDrive ) {
        wchar_t *psEndOfFirstDir = wcschr( _wcsninc(psEndOfDrive, 1), L'\\' );
        if ( psEndOfFirstDir )
        // файл находился в подкаталоге сетевого устройства
        // отрежем подкаталог
          *_wcsninc(psEndOfFirstDir, 1) = L'\0';
        else
        // файл находился в корневом каталоге сетевого устройства
        // нельзя отпускать каталог без концевого '\'
          wcscat( pszDirName, L"\\" );
      }
    }
    else {
    // старая нотация c:\dir\..
      *_wcsninc(pszDirName, 2) = L'\0';
      wcscat( pszDirName, L"\\" );
    /*
      wchar_t *psEndOfDrive = wcschr( pszDirName + 2, '\\' );
      if ( psEndOfDrive ) 
        *(psEndOfDrive + 1) = 0;
    */    
    }
  }

  return fError;
}

