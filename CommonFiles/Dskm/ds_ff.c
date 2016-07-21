/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_ff.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	9/2/2005 1:47:43 PM
*		
*		Example:	
*		
*		
*		
*/		

#if defined (_WIN32)
#include "dskm.h"
#include "dskmdefs.h"
#include "../_avpff.h"
#include "dskmi.h"



//! \fn				: DSKM_FindFirstFile
//! \brief			:	
//! \return			: static HANDLE 
//! \param          :  DSKM_Io *This
//! \param          : LPCTSTR lpFileFolder
//! \param          : LPTSTR lpFoundFileName
//! \param          : DWORD dwFileNameSize
static HANDLE DSKM_FindFirstFile( DSKM_Io *This, LPCTSTR lpFileFolder, LPTSTR lpFoundFileName, DWORD dwFileNameSize ) {
	return AvpFindFirstFile( lpFileFolder, lpFoundFileName,	dwFileNameSize );
}


//! \fn				: DSKM_FindNextFile
//! \brief			:	
//! \return			: static BOOL 
//! \param          :  DSKM_Io *This
//! \param          : HANDLE hFindFile
//! \param          : LPTSTR lpFoundFileName
//! \param          : DWORD dwFileNameSize
static BOOL DSKM_FindNextFile( DSKM_Io *This, HANDLE hFindFile, LPTSTR lpFoundFileName, DWORD dwFileNameSize ) {
	return AvpFindNextFile( hFindFile, lpFoundFileName, dwFileNameSize );
}




//! \fn				: DSKM_FindClose
//! \brief			:	
//! \return			: static BOOL 
//! \param          :  DSKM_Io *This
//! \param          : HANDLE hFindFile
static BOOL DSKM_FindClose( DSKM_Io *This, HANDLE hFindFile ) {
	return AvpFindClose( hFindFile ); 
}



//! \fn				: DSKM_LoadFF
//! \brief			:	
//! \return			: AVP_bool DSKMAPI 
//! \param          : HDSKM hDSKM
AVP_bool DSKMAPI DSKM_LoadFF( HDSKM hDSKM ) {
	HPROP hProp = DATA_Find_Prop( (HDATA)hDSKM, 0, DSKM_IO_ID );
	if ( hProp ) {
		DSKM_Io *pIo;
		PROP_Get_Val( hProp, &pIo, sizeof(avpt_dword) );
		if ( pIo && pIo->lpVtbl ) {
			if ( !pIo->lpVtbl->FindFirstFile && !pIo->lpVtbl->FindNextFile && !pIo->lpVtbl->FindClose ) {
				if ( LoadFF() ) {
					pIo->lpVtbl->FindFirstFile = DSKM_FindFirstFile;
					pIo->lpVtbl->FindNextFile = DSKM_FindNextFile;
					pIo->lpVtbl->FindClose = DSKM_FindClose;

					return 1;
				}
			}
			if ( pIo->lpVtbl->FindFirstFile && pIo->lpVtbl->FindNextFile && pIo->lpVtbl->FindClose ) {
				return 1;
			}
		}
	}
	return 0;
}

#endif // _WIN32
