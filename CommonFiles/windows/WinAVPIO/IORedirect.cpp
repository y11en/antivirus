#include <windows.h>

#define AVPIOSTATIC

#include <_avpio.h>
#include <StuffIO\XFile.h>


#if defined(_UNICODEONMBCS)
#include <Stuff\CPointer.h>

static BOOL g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

// ---
static char *UnicodeToMbcs( wchar_t *pSourceStr ) {
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
BOOL WINAPI WIOReadFile(
						HANDLE  hFile,      // handle of file to read
						LPVOID  lpBuffer,   // address of buffer that receives data
						DWORD  nNumberOfBytesToRead,        // number of bytes to read
						LPDWORD  lpNumberOfBytesRead,       // address of number of bytes read
						OVERLAPPED FAR*  lpOverlapped       // address of structure for data BYTE Disk, WORD Sector ,BYTE Head ,LPBYTE Buffer)
						)
{
	if ( hFile != INVALID_HANDLE_VALUE )
#if defined(_UNICODEONMBCS)
		return ((XFileW *)hFile)->XRead( lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead ) == IOS_ERR_OK;
#else
		return ((XFile *)hFile)->XRead( lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead ) == IOS_ERR_OK;
#endif
		
	return FALSE;
}

// ---
BOOL WINAPI WIOWriteFile(
						HANDLE  hFile,      // handle to file to write to
						LPCVOID  lpBuffer,  // pointer to data to write to file
						DWORD  nNumberOfBytesToWrite,       // number of bytes to write
						LPDWORD  lpNumberOfBytesWritten,    // pointer to number of bytes written
						OVERLAPPED FAR*  lpOverlapped       // addr. of structure needed for overlapped I/O
						)
{
	if ( hFile != INVALID_HANDLE_VALUE )
#if defined(_UNICODEONMBCS)
		return ((XFileW *)hFile)->XWrite( (void *)lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten ) == IOS_ERR_OK;
#else
		return ((XFile *)hFile)->XWrite( (void *)lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten ) == IOS_ERR_OK;
#endif

	return FALSE;
}


// ---
HANDLE WINAPI WIOCreateFile(
							LPCTSTR  lpFileName,        // pointer to name of the file
							DWORD  dwDesiredAccess,     // access (read-write) mode
							DWORD  dwShareMode, // share mode
							SECURITY_ATTRIBUTES FAR*  lpSecurityAttributes,     // pointer to security descriptor
							DWORD  dwCreationDistribution,      // how to create
							DWORD  dwFlagsAndAttributes,        // file attributes
							HANDLE  hTemplateFile       // handle to file with attributes to copy
							) 
{
#if defined(_UNICODEONMBCS)
	XFileW *pXFile = new XFileW( (wchar_t *)lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
															 dwCreationDistribution, dwFlagsAndAttributes, hTemplateFile );
#else
	XFile *pXFile = new XFile( lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
														 dwCreationDistribution, dwFlagsAndAttributes, hTemplateFile );
#endif	
	if ( ::IOSGetLastError() != IOS_ERR_OK ) {
		delete pXFile;
		return INVALID_HANDLE_VALUE;
	}

	return (HANDLE)pXFile;
}


// ---
BOOL WINAPI WIOCloseHandle(
						HANDLE  hFile     // handle to object to close
						) 
{
	if ( hFile != INVALID_HANDLE_VALUE ) {
#if defined(_UNICODEONMBCS)
		delete (XFileW *)hFile;
#else
		delete (XFile *)hFile;
#endif
		return TRUE;
	}

	return FALSE;
}

// ---
DWORD WINAPI WIOSetFilePointer(
							HANDLE  hFile,      // handle of file
							LONG  lDistanceToMove,      // number of bytes to move file pointer
							PLONG  lpDistanceToMoveHigh,        // address of high-order word of distance to move
							DWORD  dwMoveMethod         // how to move
							)
{
	if ( hFile != INVALID_HANDLE_VALUE ) {
		DWORD dwNewPosition;
#if defined(_UNICODEONMBCS)
    IOS_ERROR_CODE nCode = ((XFileW *)hFile)->XSeek( lDistanceToMove, (IOS_SEEK_ORIGIN)dwMoveMethod, &dwNewPosition );
#else
    IOS_ERROR_CODE nCode = ((XFile *)hFile)->XSeek( lDistanceToMove, (IOS_SEEK_ORIGIN)dwMoveMethod, &dwNewPosition );
#endif
		if ( nCode == IOS_ERR_OK ) 
		  return dwNewPosition;
	}

	return INVALID_SET_FILE_POINTER;
}

// ---
BOOL WINAPI WIOSetEndOfFile(
						HANDLE  hFile       // handle of file
						)
{
	if ( hFile != INVALID_HANDLE_VALUE ) {
		DWORD dwPosition;
#if defined(_UNICODEONMBCS)
    IOS_ERROR_CODE nCode = ((XFileW *)hFile)->XTell( &dwPosition );
		if ( nCode != IOS_ERR_OK ) {
			return ((XFileW *)hFile)->XSetSize( dwPosition ) == IOS_ERR_OK;
		}
#else
    IOS_ERROR_CODE nCode = ((XFile *)hFile)->XTell( &dwPosition );
		if ( nCode != IOS_ERR_OK ) {
			return ((XFile *)hFile)->XSetSize( dwPosition ) == IOS_ERR_OK;
		}
#endif
	}

	return FALSE;
}

// ---
DWORD WINAPI WIOGetFileSize(
							HANDLE  hFile,      // handle of file
							LPDWORD  lpHigh
							)
{
	if ( hFile != INVALID_HANDLE_VALUE ) {
		DWORD dwLength;
#if defined(_UNICODEONMBCS)
    if ( ((XFileW *)hFile)->XLength( &dwLength ) == IOS_ERR_OK )
#else
		if ( ((XFile *)hFile)->XLength( &dwLength ) == IOS_ERR_OK )
#endif
			return dwLength;
	}

	return 0xfffffff;
}

// ---
BOOL WINAPI WIOGetDiskFreeSpace(
						LPCTSTR lpRootPathName, // address of root path
						LPDWORD lpSectorsPerCluster,        // address of sectors per cluster
						LPDWORD lpBytesPerSector,   // address of bytes per sector
						LPDWORD lpNumberOfFreeClusters,     // address of number of free clusters
						LPDWORD lpTotalNumberOfClusters     // address of total number of clusters
						)
{
#if defined(_UNICODEONMBCS)
	if ( !g_bUnicodePlatform ) {
		CAPointer<char> pRootPathName = ::UnicodeToMbcs( (wchar_t *)lpRootPathName );
		return ::GetDiskFreeSpaceA( pRootPathName, lpSectorsPerCluster, lpBytesPerSector, lpNumberOfFreeClusters, lpTotalNumberOfClusters );
	}
	else
		return ::GetDiskFreeSpaceW( (wchar_t *)lpRootPathName, lpSectorsPerCluster, lpBytesPerSector, lpNumberOfFreeClusters, lpTotalNumberOfClusters );
#else
	return ::GetDiskFreeSpace( lpRootPathName, lpSectorsPerCluster, lpBytesPerSector, lpNumberOfFreeClusters, lpTotalNumberOfClusters );
#endif
}


// ---
DWORD WINAPI WIOGetFullPathName(
						LPCTSTR lpFileName,     // address of name of file to find path for
						DWORD nBufferLength,    // size, in characters, of path buffer
						LPTSTR lpBuffer,				// address of path buffer
						LPTSTR *lpFilePart			// address of filename in path
						)
{
#if defined(_UNICODEONMBCS)
	if ( !g_bUnicodePlatform ) {
		CAPointer<char> pFileName = ::UnicodeToMbcs( (wchar_t *)lpFileName );
		CAPointer<char> pBuffer = new char[nBufferLength + 1];
		*pBuffer = 0;
		char *pFilePart = NULL;
		DWORD dwResult = ::GetFullPathNameA( pFileName, nBufferLength, pBuffer, &pFilePart );
		::MbcsToUnicode( pBuffer, (wchar_t *)lpBuffer, nBufferLength );
		if ( lpFilePart ) {
		wchar_t *pWFilePart = wcsrchr( (wchar_t *)lpBuffer, L'\\' );
		if ( !pWFilePart )
			pWFilePart = wcsrchr( (wchar_t *)lpBuffer, L'/' );
			if ( pWFilePart )
				*(wchar_t **)lpFilePart = pWFilePart + 1;
			else
				*lpFilePart = NULL;
		}
		return dwResult;
	}
	else
		return ::GetFullPathNameW( (const wchar_t *)lpFileName, nBufferLength, (wchar_t *)lpBuffer, (wchar_t **)lpFilePart );
#else
	return ::GetFullPathName( lpFileName, nBufferLength, lpBuffer, lpFilePart );
#endif
}

// ---
BOOL WINAPI WIODeleteFile(
						LPCTSTR lpFileName      // pointer to name of file to delete
						)
{
#if defined(_UNICODEONMBCS)
	if ( !g_bUnicodePlatform ) {
		CAPointer<char> pFileName = ::UnicodeToMbcs( (wchar_t *)lpFileName );
		return ::DeleteFileA( pFileName );
	}
	else
		return ::DeleteFileW( (wchar_t *)lpFileName );
#else
	return ::DeleteFile( lpFileName );
#endif
}

/*
// ---
BOOL WINAPI WIOMoveFile(
						LPCTSTR lpExistingFileName,     // address of name of the existing file
						LPCTSTR lpNewFileName       // address of new name for the file
						)
{
	return MoveFile( lpExistingFileName, lpNewFileName );
}
*/
// ---
BOOL WINAPI WIOSetFileAttributes(
						LPCTSTR lpFileName,     // address of filename
						DWORD dwFileAttributes      // address of attributes to set
						)
{
#if defined(_UNICODEONMBCS)
	if ( !g_bUnicodePlatform ) {
		CAPointer<char> pFileName = ::UnicodeToMbcs( (wchar_t *)lpFileName );
		return ::SetFileAttributesA( pFileName, dwFileAttributes );
	}
	else
		return ::SetFileAttributesW( (wchar_t *)lpFileName, dwFileAttributes );
#else
	return ::SetFileAttributes( lpFileName, dwFileAttributes );
#endif
}

// ---
DWORD WINAPI WIOGetFileAttributes(
						 LPCTSTR lpFileName      // address of the name of a file or directory
						 )
{
#if defined(_UNICODEONMBCS)
	if ( !g_bUnicodePlatform ) {
		CAPointer<char> pFileName = ::UnicodeToMbcs( (wchar_t *)lpFileName );
		return ::GetFileAttributesA( pFileName );
	}
	else
		return ::GetFileAttributesW( (wchar_t *)lpFileName );
#else
	return ::GetFileAttributes( lpFileName );
#endif
}

// ---
DWORD WINAPI WIOGetTempPath(
						 DWORD nBufferLength, // size, in characters, of the buffer
						 LPTSTR lpBuffer      // address of buffer for temp. path
						 )
{
#if defined(_UNICODEONMBCS)
	if ( !g_bUnicodePlatform ) {
		CAPointer<char> pBuffer = new char[nBufferLength + 1];
		*pBuffer = 0;
		DWORD dwResult = ::GetTempPathA( nBufferLength, pBuffer );
		::MbcsToUnicode( pBuffer, (wchar_t *)lpBuffer, nBufferLength );
		return dwResult;
	}
	else
		return ::GetTempPathW( nBufferLength, (wchar_t *)lpBuffer );
#else
	return ::GetTempPath( nBufferLength, lpBuffer );
#endif
}


// ---
UINT WINAPI WIOGetTempFileName(
						LPCTSTR lpPathName,     // address of directory name for temporary file
						LPCTSTR lpPrefixString, // address of filename prefix
						UINT uUnique,						// number used to create temporary filename
						LPTSTR lpTempFileName   // address of buffer that receives the new filename
						) 
{
#if defined(_UNICODEONMBCS)
	if ( !g_bUnicodePlatform ) {
		CAPointer<char> pPathName = ::UnicodeToMbcs( (wchar_t *)lpPathName );
		CAPointer<char> pPrefixString = ::UnicodeToMbcs( (wchar_t *)lpPrefixString );
		CAPointer<char> pTempFileName = new char[_MAX_PATH];
		*pTempFileName = 0;
		DWORD dwResult = ::GetTempFileNameA( pPathName, pPrefixString, uUnique, pTempFileName );
		::MbcsToUnicode( pTempFileName, (wchar_t *)lpTempFileName, _MAX_PATH );
		return dwResult;
	}
	else
		return ::GetTempFileNameW( (const wchar_t *)lpPathName, (const wchar_t *)lpPrefixString, uUnique, (wchar_t *)lpTempFileName );
#else
	return ::GetTempFileName( lpPathName, lpPrefixString, uUnique, lpTempFileName );
#endif
}

/*
// ---
DWORD WINAPI WIOGetDosMemSize() {
	return GetDosMemSize();
}

// ---
DWORD WINAPI WIOGetFirstMcbSeg() {
	return GetFirstMcbSeg();
}


// ---
DWORD WINAPI WIOGetIfsApiHookTable(
						 DWORD* table,
						 DWORD size //size in DWORDS !!!!!!!!
						 ) 
{
	return GetIfsApiHookTable( table, size );
}

// ---
DWORD WINAPI WIOGetDosTraceTable(
							DWORD* table,
							DWORD size //size in DWORDS !!!!!!!!
			        )
{
	return GetDosTraceTable( table, size );
}


// ---
DWORD WINAPI WIOMemoryRead(
						 DWORD   dwOffset,     // offset
						 DWORD  dwSize,      // size in bytes
						 LPBYTE  lpBuffer    // pointer to buffer to read to
						) 
{
	return MemoryRead( dwOffset, dwSize, lpBuffer );
}

// ---
DWORD WINAPI WIOMemoryWrite(
						 DWORD   dwOffset,     // offset
						 DWORD  dwSize,      // size in bytes
						 LPBYTE  lpBuffer    // pointer to buffer to write from
						)
{
	return MemoryWrite( dwOffset, dwSize, lpBuffer );
}

// ---
BOOL WINAPI WIOGetDiskParam(
						BYTE   disk,
						BYTE   drive,
						WORD*  CX,
						BYTE*  DH
						)
{
	return GetDiskParam( disk, drive, CX, DH );
}

*/

// ---
BOOL LoadIO() {
  AvpCreateFile					= WIOCreateFile;
  AvpCloseHandle				= WIOCloseHandle;
  AvpReadFile						= WIOReadFile;
  AvpWriteFile					= WIOWriteFile;
  AvpGetFileSize				= WIOGetFileSize;
  AvpSetFilePointer			= WIOSetFilePointer;
  AvpSetEndOfFile				= WIOSetEndOfFile;
  AvpGetDiskFreeSpace		= WIOGetDiskFreeSpace;
  AvpGetFullPathName		= WIOGetFullPathName;
  AvpDeleteFile					= WIODeleteFile;
//  AvpMoveFile						= WIOMoveFile;
  AvpGetFileAttributes	= WIOGetFileAttributes;
  AvpSetFileAttributes	= WIOSetFileAttributes;
  AvpGetTempPath				= WIOGetTempPath;
  AvpGetTempFileName		= WIOGetTempFileName;
/*
  AvpMemoryRead					= WIOMemoryRead;
  AvpMemoryWrite				= WIOMemoryWrite;
  AvpGetDiskParam				= WIOGetDiskParam;
  AvpGetDosMemSize			= WIOGetDosMemSize;
  AvpGetFirstMcbSeg			= WIOGetFirstMcbSeg;
  AvpGetIfsApiHookTable	= WIOGetIfsApiHookTable;
  AvpGetDosTraceTable		= WIOGetDosTraceTable;
*/

	return TRUE;
}

