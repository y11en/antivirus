#include <Stuff\cpointer.h>
#include <StuffIO\xfile.h>

#if defined(_UNICODE) || defined(UNICODE)
#define TCSCLEN(p) (p ? _tcslen(p) : 0)
#else
#define TCSCLEN(p) (p ? strlen(p) : 0)
#endif



static IOS_ERROR_CODE  PureFileExtract( const TCHAR * pszFullFileName, TCHAR * pszFileName, uint uicchBufferLen );
static IOS_ERROR_CODE  FileExtract( const TCHAR * pszFullFileName, TCHAR * pszFileName, uint uicchBufferLen );
static TCHAR				  *FindFileExtention( const TCHAR *pszFileName, bool realExt );
static IOS_ERROR_CODE  RootDirExtract( const TCHAR * pszFileName, TCHAR * pszDirName, uint uicchBufferLen );
static IOS_ERROR_CODE	 DirExtract( const TCHAR * pszFileName, TCHAR * pszDirName, uint uicchBufferLen );

static DWORD dwAllocationGranularity = 0;

// ---
static TCHAR* DuplicateString( const TCHAR* s ) {
  return s ? _tcscpy( new TCHAR[TCSCLEN(s) + 1], s ) : 0;
}


// ---
static TCHAR* DuplicateFileName( const TCHAR* s ) {
  TCHAR *newName = ::DuplicateString( s );
  if ( newName ) {
    while ( _tcsnextc(newName) != L'\0' && _tcsnextc(newName) == L' ' ) 
      _tcscpy( newName, _tcsinc(newName) );
    TCHAR *p = _tcsninc(newName, _tcsclen(newName) - 1);
    while ( p && p != newName && _tcsnextc(p) == L' ' ) {
      *p = L'\0'; 
			p = _tcsdec(newName, p);
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


static const TCHAR *disableFileName[] = {
  _T("PRN"), 
  _T("LPT1"), _T("LPT2"), _T("LPT3"), _T("LPT4"), _T("LPT5"), _T("LPT6"), _T("LPT7"), _T("LPT8"), _T("LPT9"),
  _T("COM1"), _T("COM2"), _T("COM3"), _T("COM4"),
  _T("AUX"),
  NULL
};

// ---
static bool IsDisableName( const TCHAR *pszFileName ) {
  for ( int i=0; disableFileName[i]; i++ ) {
    if ( !_tcsicmp(disableFileName[i], pszFileName) )
      return true;
  }    
  return false;    
}

//---
XFile::XFile( LPCTSTR pszFileName, uint32 dwfAccessMode ) :
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
/*
  uint32  fdwAttr;
  uint32  dwfCreation = HIWORD(dwfAccessMode) & 0x000000ffl;
  uint32  dwfShare    = (HIWORD(dwfAccessMode) >> 8) & 0x000000ffl;

  if ( !(dwfAccessMode & FIOAM_WRITE) ) { // read only
    
    // we open normal, archive and readonly files
    fdwAttr = FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_READONLY;
    
    // set the appropriate flags for seq/random access
    if ( dwfAccessMode & FIOAM_RANDOM ) 
      fdwAttr |= FILE_FLAG_RANDOM_ACCESS;
    else
      fdwAttr |= FILE_FLAG_SEQUENTIAL_SCAN;

    if ( !dwfCreation )
      dwfCreation = OPEN_EXISTING;
      
    if ( !dwfShare )
      dwfShare = FILE_SHARE_READ;
    else 
      dwfShare >>= 1;
      
		Init( pszFileName, GENERIC_READ, dwfShare, NULL, dwfCreation, fdwAttr, NULL );
  }
  else { // read-write access requested
    // we open normal, archive and temporary files
    if ( dwfAccessMode & FIOAM_TEMPORARY ) 
      fdwAttr = FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE;
    else
      fdwAttr = FILE_ATTRIBUTE_ARCHIVE;
    
    // set the appropriate flags for seq/random access
    if ( dwfAccessMode & FIOAM_RANDOM ) 
      fdwAttr |= FILE_FLAG_RANDOM_ACCESS;
    else 
      fdwAttr |= FILE_FLAG_SEQUENTIAL_SCAN;
      
    if ( !dwfCreation )
      dwfCreation = CREATE_ALWAYS;
      
    if ( dwfShare )
      dwfShare >>= 1;
      
		Init( pszFileName, GENERIC_READ | GENERIC_WRITE, dwfShare, NULL, dwfCreation, fdwAttr, NULL );
  }
*/
}
/*
#define GENERIC_READ            0x80000000L
#define GENERIC_WRITE           0x40000000L
#define GENERIC_EXECUTE         0x20000000L
#define GENERIC_ALL             0x10000000L

#define FILE_SHARE_READ         0x00000001
#define FILE_SHARE_WRITE        0x00000002
#define FILE_SHARE_DELETE       0x00000004
#endif

#ifndef CREATE_NEW
#define CREATE_NEW              0x00000001
#define CREATE_ALWAYS           0x00000002
#define OPEN_EXISTING           0x00000003
#define OPEN_ALWAYS             0x00000004
#define TRUNCATE_EXISTING       0x00000005


#define FIOAM_READ       0x00000008l        // Открывать файл только на чтение
#define FIOAM_WRITE      0x00000004l        // Открывать файл на чтение и запись

#define FIOAM_TEMPORARY  0x00008000l        // Создавать временный файл (уничтожается после записи)

#define FIOAM_RANDOM     0x00000100l        // Случайный доступ в устройстве
#define FIOAM_SEQUENTIAL 0x00000000l        // Последовательный доступ в устройстве

#define FIOCD_CREATE_NEW    		0x00010000l	// Создавать новый файл. Возникает ошибка, если файл уже существует.
#define FIOCD_CREATE_ALWAYS			0x00020000l	// Создавать новый файл. Старый файл уничтожается
#define FIOCD_OPEN_EXISTING			0x00030000l	// Открывать файл. Возникает ошибка, если файла нет
#define FIOCD_OPEN_ALWAYS	  		0x00040000l	// Открывать файл. Если файла нет, то он создается
#define FIOCD_TRUNCATE_EXISTING	0x00050000l	// Открывать файл. После открытия файл обрезается до нулевой длины.  Файла должен открываться с атрибутом FIOAM_WRITE. Возникает ошибка, если файла нет.

#define FIOSM_SHARE_NONE        0x01000000l // Запретить доступ к файлу
#define FIOSM_SHARE_READ        0x02000000l // Разрешить доступ на чтение
#define FIOSM_SHARE_WRITE       0x04000000l // Разрешить доступ на запись
*/
//---
XFile::XFile( LPCTSTR  pszFileName,        // pointer to name of the file
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
void XFile::Init( LPCTSTR  lpFileName,        // pointer to name of the file
									DWORD  dwDesiredAccess,     // access (read-write) mode
									DWORD  dwShareMode,					// share mode
									SECURITY_ATTRIBUTES FAR*  lpSecurityAttributes,     // pointer to security descriptor
									DWORD  dwCreationDisposition,      // how to create
									DWORD  dwFlagsAndAttributes,        // file attributes
									HANDLE  hTemplateFile ) {						 // handle to file with attributes to copy

  uint32  dwcbToMap;
  uint32  dwWin32Error;

  m_szFileName = ::DuplicateFileName( lpFileName );

  IOS_ERROR_CODE nameExtrErr = ::PureFileExtract( m_szFileName, m_szFileName, (uint)_tcsclen(m_szFileName) + 1 );
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
    m_hFile = ::CreateFile( lpFileName, dwDesiredAccess,//GENERIC_READ, 
														dwShareMode, lpSecurityAttributes, dwCreationDisposition, 
														dwFlagsAndAttributes, hTemplateFile );

    if( m_hFile == INVALID_HANDLE_VALUE ) {
      ::Win32ErrToIOSErr( ::GetLastError() );
      return;
    }

    // create file mapping
    m_hFileMapping = ::CreateFileMapping( m_hFile, lpSecurityAttributes, 
																					PAGE_READONLY, 0, 0, NULL );

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
    CAPointer<TCHAR> pszRootDirName = new TCHAR[_MAX_PATH]; 
    if ( ::RootDirExtract( lpFileName, pszRootDirName, _MAX_PATH ) != IOS_ERR_OK )
      return;

    if ( ::GetDriveType(pszRootDirName) == DRIVE_REMOTE	)
      dwFlagsAndAttributes |= FILE_FLAG_WRITE_THROUGH;
      
    // now try to open a file
    m_hFile = ::CreateFile( lpFileName, dwDesiredAccess | GENERIC_READ,//GENERIC_READ | GENERIC_WRITE, 
														dwShareMode, lpSecurityAttributes, dwCreationDisposition, 
														dwFlagsAndAttributes, hTemplateFile );

    if ( m_hFile == INVALID_HANDLE_VALUE ) {
      ::Win32ErrToIOSErr( ::GetLastError() );
      return;
    }

    // файл мог уже существовать
    m_dwcbFileSize = ::GetFileSize( m_hFile, NULL );
//    m_dwcbFileSize = 0;

    // create file mapping
    m_hFileMapping = ::CreateFileMapping( m_hFile, lpSecurityAttributes, 
																					PAGE_READWRITE, 0, m_dwcbMapSize, NULL );
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
XFile::~XFile() {
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
IOS_ERROR_CODE XFile::UpdateFileMapping( void ) {
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
IOS_ERROR_CODE XFile::XReadByte( uint8* pb ) {
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
IOS_ERROR_CODE XFile::XReadWord( uint16* pw ) {
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
IOS_ERROR_CODE XFile::XReadDWord( uint32* pdw ) {
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
IOS_ERROR_CODE XFile::XReadQWord( void* pq ) {
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
IOS_ERROR_CODE XFile::XReadString( TCHAR **pszBuff, uint* puiRead ) {
  uint32        dwRead;
  IOS_ERROR_CODE errCode;
  uint32        dwLength;
//  uint32        dwCurr;

	errCode = XReadDWord( &dwLength );
  if ( IOS_ERR_OK != errCode ) 
		return errCode;

	*pszBuff = new TCHAR[dwLength + 1];
	errCode = XRead( *pszBuff, dwLength, &dwRead );

  if ( IOS_ERR_OK != errCode ) 
		return errCode;

	if ( dwLength != dwRead )
      return IOS_ERR_NO_MORE_DATA;

  (*pszBuff)[dwRead] = L'\0';
  if ( puiRead )
    *puiRead = dwRead;
/*
	*pszBuff = new TCHAR[dwLength + 1];
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
IOS_ERROR_CODE XFile::XReadLine( TCHAR * pszBuff, uint uicBytes, uint* puiRead ) {
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
IOS_ERROR_CODE XFile::XWriteByte( uint8* pb ) {
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
IOS_ERROR_CODE XFile::XWriteWord( uint16* pw ) {
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
IOS_ERROR_CODE XFile::XWriteDWord( uint32* pdw ) {
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
IOS_ERROR_CODE XFile::XWriteQWord( void* pq )
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
IOS_ERROR_CODE XFile::XWriteString( const TCHAR * psz ) {
  uint32          dwWritten;
  uint32          dwToWrite;
  IOS_ERROR_CODE errCode;

	dwToWrite = psz ? (uint32)TCSCLEN( psz ) : 0;
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
IOS_ERROR_CODE XFile::XWriteLine( const TCHAR * psz ) {
  uint32          dwWritten;
  uint32          dwToWrite;
  IOS_ERROR_CODE errCode;

  dwToWrite = psz ? (uint32)TCSCLEN( psz ) : 0;
  errCode = XWrite( (void *)psz, dwToWrite, &dwWritten );
  if ( errCode != IOS_ERR_OK )
    return errCode;
  if ( dwWritten != dwToWrite ) 
    return IOS_ERR_NO_MORE_DATA;
  return IOS_ERR_OK;
}


//---
IOS_ERROR_CODE XFile::XLength( uint32* pdwLength ) {
  if ( pdwLength ) {
    *pdwLength = m_dwcbFileSize;
    return IOS_ERR_OK;
  }

  return IOS_ERR_OUT_OF_MEMORY;
}


//---
IOS_ERROR_CODE XFile::XSetSize( uint32 dwLength ) {
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
IOS_ERROR_CODE XFile::XRead( void * pvBuffer, uint32 dwcbToRead, uint32 * pdwcbActuallyRead ) {
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
IOS_ERROR_CODE XFile::XWrite( void * pvBuffer, uint32 dwcbToWrite, uint32 * pdwcbActuallyWritten ) {
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
IOS_ERROR_CODE XFile::XSeek( long lDistance, IOS_SEEK_ORIGIN eSeekOrigin, uint32 * pdwNewPosition ) {
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
bool XFile::XEof() {
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
bool XFile::XSetTime( const FILETIME *  lpftCreation,	  // time the file was created 
    									const FILETIME *  lpftLastAccess,	// time the file was last accessed 
    									const FILETIME *  lpftLastWrite 	// time the file was last written 
                    ) {

  return !!::SetFileTime( m_hFile, lpftCreation, lpftLastAccess, lpftLastWrite );
}


#if defined(UNICODE) || defined(_UNICODE)
//---
IOS_ERROR_CODE XFile::XReadString( char **pszBuff, uint* puiRead ) {
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
IOS_ERROR_CODE XFile::XReadLine( char * pszBuff, uint uicBytes, uint* puiRead ) {
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
IOS_ERROR_CODE XFile::XWriteString( const char * psz ) {
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
IOS_ERROR_CODE XFile::XWriteLine( const char * psz ) {
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
static IOS_ERROR_CODE PureFileExtract( const TCHAR * pszFullFileName, TCHAR * pszFileName, uint uicchBufferLen ) {
  if ( pszFullFileName && pszFileName ) {
    CAPointer<TCHAR> pszFile = new TCHAR[_MAX_FNAME];
    if ( ::FileExtract(pszFullFileName, pszFile, _MAX_FNAME) == IOS_ERR_OK ) {
      TCHAR *psExt = ::FindFileExtention( pszFile, false );
      if ( psExt )
        *psExt = L'\0';
      if ( uint(TCSCLEN(pszFile)) < uicchBufferLen ) {
        _tcsncpy( pszFileName, pszFile, uicchBufferLen );
        return IOS_ERR_OK;
      }  
    }
    return IOS_ERR_OUT_OF_MEMORY;  
  }
  return IOS_ERR_INVALID_FILE_NAME;
}

//---
static IOS_ERROR_CODE FileExtract( const TCHAR * pszFullFileName, TCHAR * pszFileName, uint uicchBufferLen ) {
  if ( pszFullFileName && pszFileName ) {

    CAPointer<TCHAR> fFile = new TCHAR[_MAX_FNAME];
    CAPointer<TCHAR> fExt  = new TCHAR[_MAX_EXT];

  // выделить диск и каталог
    _tsplitpath( pszFullFileName, NULL, NULL, fFile, fExt);

    if ( uint(TCSCLEN(fFile) + TCSCLEN(fExt)) < uicchBufferLen ) {
      _tcsncpy( pszFileName, fFile, uicchBufferLen );
      uint uichLen = (uint)TCSCLEN( pszFileName );
      pszFileName = _tcsninc(pszFileName, _tcsclen( pszFileName )/*uichLen*/);
      _tcsncpy( pszFileName, fExt, uicchBufferLen - uichLen );
    }  
    else
      return IOS_ERR_OUT_OF_MEMORY;  

    return IOS_ERR_OK;
  }
  return IOS_ERR_INVALID_FILE_NAME;
}


// ---
static TCHAR * FindFileExtention( const TCHAR *pszFileName, bool realExt ) {
  if ( pszFileName && *pszFileName ) {
    TCHAR *ext = _tcsninc((TCHAR *)pszFileName, _tcsclen(pszFileName) - 1);
    while ( ext && ext != pszFileName ) {
      if ( _tcsnextc(ext) == L'.' )
        return _tcsninc(ext, !!realExt);
			int nNext = _tcsnextc(ext);
      if ( nNext == L'\\' || nNext == L'/' )
        break;
      ext = _tcsdec( pszFileName, ext );
    }
  }
  return NULL;
}

//---
static IOS_ERROR_CODE DirExtract( const TCHAR * pszFileName, TCHAR * pszDirName, uint uicchBufferLen ) {
  if ( pszFileName && pszDirName ) {
		
    CAPointer<TCHAR> fDrive = new TCHAR[_MAX_DRIVE];
    CAPointer<TCHAR> fDir   = new TCHAR[_MAX_DIR];
		
		// выделить диск и каталог
    _tsplitpath( pszFileName, fDrive, fDir, NULL, NULL);
    int dLen = (int)_tcsclen( fDir );
		TCHAR *pLast;
		
    if ( dLen > 1 && 
			(pLast = _tcsninc(fDir, dLen - 1)) && 
			(_tcsnextc(pLast) == L'\\') )
      *pLast = L'\0';
		
    if ( (size_t)(TCSCLEN(fDrive) + TCSCLEN(fDir)) < uicchBufferLen ) {
			int uichLen;
      _tcsncpy( pszDirName, fDrive, uicchBufferLen );
      uichLen = (int)TCSCLEN(pszDirName);
      pszDirName = _tcsninc( pszDirName, _tcsclen(pszDirName)/*uichLen*/ );
      _tcsncpy( pszDirName, fDir, uicchBufferLen - uichLen );
    }  
    else
      return IOS_ERR_OUT_OF_MEMORY;  
		
    return IOS_ERR_OK;
  }
  return IOS_ERR_INVALID_FILE_NAME;
}

//---
static IOS_ERROR_CODE RootDirExtract( const TCHAR * pszFileName, TCHAR * pszDirName, uint uicchBufferLen ) {

  IOS_ERROR_CODE fError = ::DirExtract( pszFileName, pszDirName, uicchBufferLen );

  if ( fError == IOS_ERR_OK ) {
    if ( _tcsnextc(pszDirName) == L'\\' ) {
    // новая нотация \\netdrive\drive\dir\..
      TCHAR *psEndOfDrive = _tcschr( _tcsninc(pszDirName, 2), L'\\' );
      if ( psEndOfDrive ) {
        TCHAR *psEndOfFirstDir = _tcschr( _tcsninc(psEndOfDrive, 1), L'\\' );
        if ( psEndOfFirstDir )
        // файл находился в подкаталоге сетевого устройства
        // отрежем подкаталог
          *_tcsninc(psEndOfFirstDir, 1) = L'\0';
        else
        // файл находился в корневом каталоге сетевого устройства
        // нельзя отпускать каталог без концевого '\'
          _tcscat( pszDirName, _T("\\") );
      }
    }
    else {
    // старая нотация c:\dir\..
      *_tcsninc(pszDirName, 2) = L'\0';
      _tcscat( pszDirName, _T("\\") );
    /*
      TCHAR *psEndOfDrive = _tcschr( pszDirName + 2, '\\' );
      if ( psEndOfDrive ) 
        *(psEndOfDrive + 1) = 0;
    */    
    }
  }

  return fError;
}

