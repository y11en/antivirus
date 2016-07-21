////////////////////////////////////////////////////////////////////
//
//  Xerrors.h
//  IO error code definitions
//  Generic purpose IO stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#ifndef __XERRORS_H
#define	__XERRORS_H

typedef enum {
  IOS_ERR_OK = 0,                            // Success
	IOS_ERR_CANCELED,													// Operation cancelled

	IOS_ERR_INVALID_PARAMETERS,
  IOS_ERR_OUT_OF_MEMORY,											
  IOS_ERR_OUT_OF_DISK_SPACE,

  IOS_ERR_SEEK_ERROR,												// File seek
	IOS_ERR_NO_MORE_DATA,                      // Success: no more data
	
	IOS_ERR_FILE_NOT_FOUND,
	IOS_ERR_INVALID_FILE_NAME,     						// File name is invalid for disk file system
	IOS_ERR_FILE_IN_USE,
	IOS_ERR_WRITE_PROTECT,
	IOS_ERR_INTERNAL_ERROR,                    // Internal error
  IOS_ERR_SYSTEM_ERROR,
  IOS_ERR_MEDIA_CORRUPTED,
	IOS_ERR_NETWORK_ERROR,
  IOS_ERR_TOO_MANY_OPEN_FILES,

  IOS_ERR_BAD_FILE_FORMAT,                   // File format seems to be invalid
  IOS_ERR_BAD_FILE_DATA,                     // File data cannot be interpreted

  IOS_ERR_READING_NOT_SUPPORTED,             // File loading operation is not supported
  IOS_ERR_WRITING_NOT_SUPPORTED,             // File saving operation is not supported
  IOS_ERR_WRITE_IMAGE_TOO_LARGE,             // Output image is too large for this format
  IOS_ERR_READ_IMAGE_TOO_LARGE,              // Input image is too large to be read

  IOS_ERR_FILE_WITH_PATH_NOT_FOUND,          // ‘айл с путем не найден

	IOS_ERR_ACCESS_DENIED,

	IOS_ERR_LAST
} IOS_ERROR_CODE, *PIOS_ERROR_CODE;


IOS_ERROR_CODE Win32ErrToIOSErr( DWORD dwWin32Error );

IOS_ERROR_CODE IOSGetLastError();
void IOSSetLastError( IOS_ERROR_CODE dwError );

DWORD IOSGetWin32LastError();
	
#endif //__ERRORS_H
