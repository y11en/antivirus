/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_io.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	9/2/2005 1:42:38 PM
*		
*		Example:	
*		
*		
*		
*/		




#include "dskm.h"
#include "dskmdefs.h"
#include "../_AVPIO.h"
#include "dskmi.h"



//! \fn				: int DSKM_AddRef
//! \brief			:	
//! \return			: static unsigned 
//! \param          : DSKM_Io *This
static unsigned int DSKM_AddRef( DSKM_Io *This ) {
  return ++This->m_cRef;
}


//! \fn				: int DSKM_Release
//! \brief			:	
//! \return			: static unsigned 
//! \param          : DSKM_Io *This
static unsigned int DSKM_Release( DSKM_Io *This ) {
  if ( --This->m_cRef == 0 ) {
		
    DSKMLiberator( This->lpVtbl );
    DSKMLiberator( This );
		
    return 0;
	}
  return This->m_cRef;
}



//! \fn				: DSKM_CreateFile
//! \brief			:	
//! \return			: static HANDLE 
//! \param          :  DSKM_Io *This
//! \param          : LPCTSTR lpFileName
//! \param          : DWORD dwDesiredAccess
//! \param          : DWORD dwShareMode
//! \param          : SECURITY_ATTRIBUTES FAR* lpSecurityAttributes
//! \param          : DWORD dwCreationDistribution
//! \param          : DWORD dwFlagsAndAttributes
//! \param          : HANDLE hTemplateFile
static HANDLE DSKM_CreateFile( DSKM_Io *This, LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, void* lpSecurityAttributes, DWORD dwCreationDistribution, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile ) {
	return AvpCreateFile( lpFileName, dwDesiredAccess, dwShareMode, (SECURITY_ATTRIBUTES FAR *)lpSecurityAttributes, dwCreationDistribution, dwFlagsAndAttributes, hTemplateFile );
}



//! \fn				: DSKM_CloseHandle
//! \brief			:	
//! \return			: static BOOL 
//! \param          :  DSKM_Io *This
//! \param          : HANDLE hObject
static BOOL DSKM_CloseHandle( DSKM_Io *This, HANDLE hObject ) {
	return AvpCloseHandle( hObject );
}



//! \fn				: DSKM_ReadFile
//! \brief			:	
//! \return			: static BOOL 
//! \param          :  DSKM_Io *This
//! \param          : HANDLE hFile
//! \param          : LPVOID lpBuffer
//! \param          : DWORD nNumberOfBytesToRead
//! \param          : LPDWORD lpNumberOfBytesRead
//! \param          : OVERLAPPED FAR* lpOverlapped
static BOOL DSKM_ReadFile( DSKM_Io *This, HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, void* lpOverlapped ) {
	return AvpReadFile( hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, (OVERLAPPED FAR *)lpOverlapped );
}



//! \fn				: DSKM_WriteFile
//! \brief			:	
//! \return			: static BOOL 
//! \param          :  DSKM_Io *This
//! \param          : HANDLE hFile
//! \param          : LPCVOID lpBuffer
//! \param          : DWORD nNumberOfBytesToWrite
//! \param          : LPDWORD lpNumberOfBytesWritten
//! \param          : OVERLAPPED FAR* lpOverlapped
static BOOL DSKM_WriteFile( DSKM_Io *This, HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, void* lpOverlapped ) {
	return AvpWriteFile( hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, (OVERLAPPED FAR *)lpOverlapped );
}



//! \fn				: DSKM_SetFilePointer
//! \brief			:	
//! \return			: static DWORD 
//! \param          :  DSKM_Io *This
//! \param          : HANDLE hFile
//! \param          : LONG lDistanceToMove
//! \param          : PLONG lpDistanceToMoveHigh
//! \param          : DWORD dwMoveMethod
static DWORD DSKM_SetFilePointer( DSKM_Io *This, HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod ) {
	return AvpSetFilePointer( hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
}



//! \fn				: DSKM_GetFileSize
//! \brief			:	
//! \return			: static DWORD 
//! \param          :  DSKM_Io *This
//! \param          : HANDLE hFile
//! \param          : LPDWORD lpHigh
static DWORD DSKM_GetFileSize( DSKM_Io *This, HANDLE hFile, LPDWORD lpHigh ) {
	return AvpGetFileSize( hFile, lpHigh );
}


//! \fn				: DSKM_LoadIO
//! \brief			:	
//! \return			: AVP_bool DSKMAPI 
//! \param          : HDSKM hDSKM
AVP_bool DSKMAPI DSKM_LoadIO( HDSKM hDSKM ) {
	HPROP hProp = DATA_Find_Prop( (HDATA)hDSKM, 0, DSKM_IO_ID );
	if ( !hProp ) {
		if ( LoadIO() ) {
			DSKM_Io_Vtbl *pVtbl = DSKMAllocator( sizeof(DSKM_Io_Vtbl) );
			if ( pVtbl ) {
				DSKM_Io *pIo = 0;

				ds_mset( pVtbl, 0, sizeof(DSKM_Io_Vtbl) );
				pVtbl->AddRef = DSKM_AddRef;
				pVtbl->Release = DSKM_Release;
				pVtbl->CreateFile = DSKM_CreateFile;
				pVtbl->CloseHandle = DSKM_CloseHandle;
				pVtbl->ReadFile = DSKM_ReadFile;
				pVtbl->WriteFile = DSKM_WriteFile; 
				pVtbl->SetFilePointer = DSKM_SetFilePointer;
				pVtbl->GetFileSize = DSKM_GetFileSize;

				pIo = DSKMAllocator( sizeof(DSKM_Io) );
				if ( pIo ) {
					ds_mset( pIo, 0, sizeof(DSKM_Io) );
					pIo->lpVtbl = pVtbl;

					DATA_Add_Prop( (HDATA)hDSKM, 0, DSKM_IO_ID, (AVP_size_t)pIo, 0 );

					pVtbl->AddRef( pIo );

					return 1;
				}
				DSKMLiberator( pVtbl );
			}
		}
		return 0;
	}
	
	return 1;
}


