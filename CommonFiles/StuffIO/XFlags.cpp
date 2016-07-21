////////////////////////////////////////////////////////////////////
//
//  XFlags.h
//  IO flags to Win32 flags converter
//  Generic purpose IO stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <StuffIO\XFlags.h>

// ---
void IOFlagsToWin32Flags( DWORD dwIOFlags, 
												  DWORD *pdwDesiredAccess, 
													DWORD *pdwShareMode, 
													DWORD *pdwCreationDisposition, 
													DWORD *pdwFlagsAndAttributes ) {

  *pdwCreationDisposition = HIWORD(dwIOFlags) & 0x000000ffl;
  *pdwShareMode           = (HIWORD(dwIOFlags) >> 8) & 0x000000ffl;
	*pdwDesiredAccess				= (LOWORD(dwIOFlags) & 0x0000000fl) << 28;

  if ( !(dwIOFlags & FIOAM_WRITE) ) { // read only
    
    // we open normal, archive and readonly files
    *pdwFlagsAndAttributes = FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_READONLY;
    
    // set the appropriate flags for seq/random access
    if ( dwIOFlags & FIOAM_RANDOM ) 
      *pdwFlagsAndAttributes |= FILE_FLAG_RANDOM_ACCESS;
    else
      *pdwFlagsAndAttributes |= FILE_FLAG_SEQUENTIAL_SCAN;

    if ( !*pdwCreationDisposition )
      *pdwCreationDisposition = OPEN_EXISTING;
      
    if ( !*pdwShareMode )
      *pdwShareMode = FILE_SHARE_READ;
    else 
      *pdwShareMode >>= 1;
  }
  else { // read-write access requested
    // we open normal, archive and temporary files
    if ( dwIOFlags & FIOAM_TEMPORARY ) 
      *pdwFlagsAndAttributes = FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE;
    else
      *pdwFlagsAndAttributes = FILE_ATTRIBUTE_ARCHIVE;
    
    // set the appropriate flags for seq/random access
    if ( dwIOFlags & FIOAM_RANDOM ) 
      *pdwFlagsAndAttributes |= FILE_FLAG_RANDOM_ACCESS;
    else 
      *pdwFlagsAndAttributes |= FILE_FLAG_SEQUENTIAL_SCAN;
      
    if ( !*pdwCreationDisposition )
      *pdwCreationDisposition = CREATE_ALWAYS;
      
    if ( *pdwShareMode )
      *pdwShareMode >>= 1;
  }

}

// ---
void IOSeekCodeToWin32SeekCode( IOS_SEEK_ORIGIN dwIOSeek, DWORD *pdwWin32Seek ) {
	if ( pdwWin32Seek )
		*pdwWin32Seek = dwIOSeek;
}
