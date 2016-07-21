#ifndef __XFILE_H
#define __XFILE_H

#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include "../stuff/stddefs.h"
#include "xerrors.h"
#include "xflags.h"


// Класс для выполнения операций файлового ввода/вывода через
// Memory-Mapped Files
// ---
class XFile {
protected :
  HANDLE  m_hFile;
  HANDLE  m_hFileMapping;
  uint32  m_dwfAccessMode;
  uint32  m_dwcbPosition;
  uint32  m_dwcbFileSize;
  void *  m_pvBaseAddress;
  uint32  m_dwcbFileOffset;
  uint32  m_dwcbMapSize;
  
  TCHAR   *m_szFileName;
public:

  XFile( LPCTSTR  lpFileName, uint32 dwfAccessMode );
	XFile( LPCTSTR  lpFileName,        // pointer to name of the file
				 DWORD  dwDesiredAccess,     // access (read-write) mode
				 DWORD  dwShareMode,					// share mode
				 SECURITY_ATTRIBUTES FAR*  lpSecurityAttributes,     // pointer to security descriptor
				 DWORD  dwCreationDistribution,      // how to create
				 DWORD  dwFlagsAndAttributes,        // file attributes
				 HANDLE  hTemplateFile );						 // handle to file with attributes to copy

  ~XFile();

  IOS_ERROR_CODE XRead ( void * pvBuffer, uint32 dwcb, uint32 *pdwcbRead = NULL );
  IOS_ERROR_CODE XWrite( void * pvBuffer, uint32 dwcb, uint32 *pdwcbWritten = NULL );
  IOS_ERROR_CODE XSeek ( long lDistance, IOS_SEEK_ORIGIN eSeekOrigin, uint32 *pdwNewPosition = NULL );
  
  IOS_ERROR_CODE XTell ( uint32 *pdwCurrentPosition = NULL ) {
    return XSeek( 0, FIOSO_CURRENT, pdwCurrentPosition );
  }

  IOS_ERROR_CODE XReadByte( uint8* pb );
  IOS_ERROR_CODE XReadWord( uint16* pw );
  IOS_ERROR_CODE XReadDWord( uint32* pb );
  IOS_ERROR_CODE XReadQWord( void* pq );
  IOS_ERROR_CODE XReadString( TCHAR **pszBuff, uint *puiRead );
  IOS_ERROR_CODE XReadLine( TCHAR * pszBuff, uint uicBytes, uint *puiRead );

  IOS_ERROR_CODE XWriteByte( uint8* pb );
  IOS_ERROR_CODE XWriteWord( uint16* pw );
  IOS_ERROR_CODE XWriteDWord( uint32* pb );
  IOS_ERROR_CODE XWriteQWord( void* pq );
  IOS_ERROR_CODE XWriteString( const TCHAR * psz );
  IOS_ERROR_CODE XWriteLine( const TCHAR * psz );

  IOS_ERROR_CODE XLength( uint32* pdwLength );
  IOS_ERROR_CODE XSetSize( uint32 dwLength );
  
  IOS_ERROR_CODE XGetName( TCHAR * pszBuffer, uint uicb ) { 
    _tcsncpy(pszBuffer, m_szFileName, uicb);
    return IOS_ERR_OK;
  }
  const TCHAR* XGetName() const {
    return m_szFileName;
  }
  bool           XEof();
	bool 					 XSetTime( const FILETIME *lpftCreation,	  // time the file was created 
    											 const FILETIME *lpftLastAccess,	// time the file was last accessed 
    											 const FILETIME *lpftLastWrite );	// time the file was last written 
  
#if defined(UNICODE) || defined(_UNICODE)
	IOS_ERROR_CODE XReadString( char **pszBuff, uint *puiRead );
  IOS_ERROR_CODE XReadLine( char * pszBuff, uint uicBytes, uint *puiRead );
  IOS_ERROR_CODE XWriteString( const char * psz );
  IOS_ERROR_CODE XWriteLine( const char * psz );
#endif 

private:
  IOS_ERROR_CODE UpdateFileMapping( void );
protected :
	void Init( LPCTSTR  lpFileName,        // pointer to name of the file
						 DWORD  dwDesiredAccess,     // access (read-write) mode
						 DWORD  dwShareMode,					// share mode
						 SECURITY_ATTRIBUTES FAR*  lpSecurityAttributes,     // pointer to security descriptor
						 DWORD  dwCreationDistribution,      // how to create
						 DWORD  dwFlagsAndAttributes,        // file attributes
						 HANDLE  hTemplateFile );						 // handle to file with attributes to copy
};


// ---
class XFileW {
protected :
  HANDLE  m_hFile;
  HANDLE  m_hFileMapping;
  uint32  m_dwfAccessMode;
  uint32  m_dwcbPosition;
  uint32  m_dwcbFileSize;
  void *  m_pvBaseAddress;
  uint32  m_dwcbFileOffset;
  uint32  m_dwcbMapSize;
  
  wchar_t *m_szFileName;
public:
	
  XFileW( LPCWSTR  lpFileName, uint32 dwfAccessMode );
	XFileW( LPCWSTR  lpFileName,        // pointer to name of the file
		DWORD  dwDesiredAccess,     // access (read-write) mode
		DWORD  dwShareMode,					// share mode
		SECURITY_ATTRIBUTES FAR*  lpSecurityAttributes,     // pointer to security descriptor
		DWORD  dwCreationDistribution,      // how to create
		DWORD  dwFlagsAndAttributes,        // file attributes
		HANDLE  hTemplateFile );						 // handle to file with attributes to copy
	
  ~XFileW();
	
  IOS_ERROR_CODE XRead ( void * pvBuffer, uint32 dwcb, uint32 *pdwcbRead = NULL );
  IOS_ERROR_CODE XWrite( void * pvBuffer, uint32 dwcb, uint32 *pdwcbWritten = NULL );
  IOS_ERROR_CODE XSeek ( long lDistance, IOS_SEEK_ORIGIN eSeekOrigin, uint32 *pdwNewPosition = NULL );
  
  IOS_ERROR_CODE XTell ( uint32 *pdwCurrentPosition = NULL ) {
    return XSeek( 0, FIOSO_CURRENT, pdwCurrentPosition );
  }
	
  IOS_ERROR_CODE XReadByte( uint8* pb );
  IOS_ERROR_CODE XReadWord( uint16* pw );
  IOS_ERROR_CODE XReadDWord( uint32* pb );
  IOS_ERROR_CODE XReadQWord( void* pq );
  IOS_ERROR_CODE XReadString( wchar_t **pszBuff, uint *puiRead );
  IOS_ERROR_CODE XReadLine( wchar_t * pszBuff, uint uicBytes, uint *puiRead );
	
  IOS_ERROR_CODE XWriteByte( uint8* pb );
  IOS_ERROR_CODE XWriteWord( uint16* pw );
  IOS_ERROR_CODE XWriteDWord( uint32* pb );
  IOS_ERROR_CODE XWriteQWord( void* pq );
  IOS_ERROR_CODE XWriteString( const wchar_t * psz );
  IOS_ERROR_CODE XWriteLine( const wchar_t * psz );
	
  IOS_ERROR_CODE XLength( uint32* pdwLength );
  IOS_ERROR_CODE XSetSize( uint32 dwLength );
  
  IOS_ERROR_CODE XGetName( wchar_t * pszBuffer, uint uicb ) { 
    wcsncpy(pszBuffer, m_szFileName, uicb);
    return IOS_ERR_OK;
  }
  const wchar_t* XGetName() const {
    return m_szFileName;
  }
  bool           XEof();
	bool 					 XSetTime( const FILETIME *lpftCreation,	  // time the file was created 
													 const FILETIME *lpftLastAccess,	// time the file was last accessed 
													 const FILETIME *lpftLastWrite );	// time the file was last written 
  
#if defined(UNICODE) || defined(_UNICODE)
	IOS_ERROR_CODE XReadString( char **pszBuff, uint *puiRead );
  IOS_ERROR_CODE XReadLine( char * pszBuff, uint uicBytes, uint *puiRead );
  IOS_ERROR_CODE XWriteString( const char * psz );
  IOS_ERROR_CODE XWriteLine( const char * psz );
#endif 
	
private:
  IOS_ERROR_CODE UpdateFileMapping( void );
protected :
	void Init(  LPCWSTR  lpFileName,        // pointer to name of the file
							DWORD  dwDesiredAccess,     // access (read-write) mode
							DWORD  dwShareMode,					// share mode
							SECURITY_ATTRIBUTES FAR*  lpSecurityAttributes,     // pointer to security descriptor
							DWORD  dwCreationDistribution,      // how to create
							DWORD  dwFlagsAndAttributes,        // file attributes
							HANDLE  hTemplateFile );						 // handle to file with attributes to copy
};


#endif 
