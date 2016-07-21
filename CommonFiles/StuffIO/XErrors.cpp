////////////////////////////////////////////////////////////////////
//
//  XErrors.cpp
//  Wi32 to IO errors converter
//  Generic purpose IO stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <StuffIO\XErrors.h>

static IOS_ERROR_CODE gdwLastError = IOS_ERR_OK;
static DWORD          gdwWin32LastError = 0;

//---
IOS_ERROR_CODE Win32ErrToIOSErr( DWORD dwWin32Error ) {
	gdwWin32LastError = dwWin32Error;

  switch ( dwWin32Error ) {
    case ERROR_SUCCESS:
      gdwLastError = IOS_ERR_OK;
      break;

    case ERROR_NOT_ENOUGH_MEMORY:
    case ERROR_OUTOFMEMORY:
    case ERROR_NOT_ENOUGH_SERVER_MEMORY:
    case ERROR_OUT_OF_STRUCTURES:
      gdwLastError = IOS_ERR_OUT_OF_MEMORY;
      break;

    case ERROR_DRIVE_LOCKED:
    case ERROR_LOCK_VIOLATION:
    case ERROR_SHARING_VIOLATION:
      gdwLastError = IOS_ERR_FILE_IN_USE;
      break;

    case ERROR_ACCESS_DENIED:
    case ERROR_NETWORK_ACCESS_DENIED:
			gdwLastError = IOS_ERR_ACCESS_DENIED;
			break;

    case ERROR_FILE_CORRUPT:
    case ERROR_DISK_CORRUPT:
      gdwLastError = IOS_ERR_MEDIA_CORRUPTED;
      break;

    case ERROR_TOO_MANY_OPEN_FILES:
      gdwLastError = IOS_ERR_TOO_MANY_OPEN_FILES;
      break;
    
    case ERROR_CANNOT_MAKE: // cannot mkdir
    case ERROR_CURRENT_DIRECTORY: // cannot rmdir current dir
    case ERROR_DIR_NOT_EMPTY:    
      gdwLastError = IOS_ERR_SYSTEM_ERROR;
      break;

    case ERROR_WRITE_PROTECT:
      gdwLastError = IOS_ERR_WRITE_PROTECT;
      break;

    case ERROR_HANDLE_EOF:
      gdwLastError = IOS_ERR_NO_MORE_DATA;
      break;

    case ERROR_DISK_FULL:
    case ERROR_HANDLE_DISK_FULL:
      gdwLastError = IOS_ERR_OUT_OF_DISK_SPACE;
      break;

    case ERROR_BAD_PATHNAME:
    case ERROR_BAD_UNIT:
    case ERROR_FILENAME_EXCED_RANGE:
    case ERROR_INVALID_DRIVE:
    case ERROR_INVALID_NAME:
      gdwLastError = IOS_ERR_INVALID_FILE_NAME;
      break;


    case ERROR_FILE_NOT_FOUND:
      gdwLastError = IOS_ERR_FILE_NOT_FOUND;
      break;

    case ERROR_PATH_NOT_FOUND:
    case ERROR_INVALID_NETNAME:
    case ERROR_DEV_NOT_EXIST:
    case ERROR_BAD_NET_NAME:
    case ERROR_NO_NET_OR_BAD_PATH:
      gdwLastError = IOS_ERR_INVALID_FILE_NAME;
      break;

    case ERROR_CONNECTION_UNAVAIL:
    case ERROR_BAD_NET_RESP:
    case ERROR_BAD_NETPATH:
    case ERROR_BAD_PROFILE:
    case ERROR_BAD_PROVIDER:
    case ERROR_NET_WRITE_FAULT:
    case ERROR_NETWORK_BUSY:
    case ERROR_NO_BROWSER_SERVERS_FOUND:
    case ERROR_NO_NETWORK:
    case ERROR_NOT_SUPPORTED:
    case ERROR_SHARING_PAUSED:
    case ERROR_NETNAME_DELETED:
      gdwLastError = IOS_ERR_NETWORK_ERROR;
      break;

    case ERROR_DISK_OPERATION_FAILED:
    case ERROR_DISK_RECALIBRATE_FAILED:
    case ERROR_DISK_RESET_FAILED:
    case ERROR_DIRECTORY:
    case ERROR_CRC:
    case ERROR_FILE_INVALID:
    case ERROR_IO_DEVICE:
    case ERROR_NOT_READY:
    case ERROR_READ_FAULT:
    case ERROR_SECTOR_NOT_FOUND:
    case ERROR_SEEK:
    case ERROR_SEEK_ON_DEVICE:
      gdwLastError = IOS_ERR_MEDIA_CORRUPTED;
      break;

    default:
      gdwLastError = IOS_ERR_SYSTEM_ERROR;
      break;
  }

  return gdwLastError;
}


//---
DWORD IOSGetWin32LastError() {
  return gdwWin32LastError;
}

//---
IOS_ERROR_CODE IOSGetLastError() {
  return gdwLastError;
}

//---
void IOSSetLastError( IOS_ERROR_CODE dwError ) {
  gdwLastError = dwError;
}

