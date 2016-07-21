/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_serial.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	6/7/2005 6:50:22 PM
*		
*		Example:	
*		
*		
*		
*/		

#include "dskm.h"
#include "dskmdefs.h"
#include "dskmi.h"


#include "../SWManager/Swm.h"
#include "../_AVPIO.h"

#define BUFFER_SIZE (0x4000)




//! \fn				:  DSKMAPI DSKM_WriteRegHeaderToFile
//! \brief			:	
//! \return			: static AVP_bool 
//! \param          :  HDSKM hDSKM
//! \param          : HANDLE hFile
//! \param          : AVP_dword dwObjectsType
//! \param          : AVP_bool bSpecialReg
static AVP_bool  DSKMAPI DSKM_WriteRegHeaderToFile( HDSKM hDSKM, HANDLE hFile, AVP_dword dwObjectsType, AVP_bool bSpecialReg ) {
	DSKM_Io *pIo = DSKM_GetIO( hDSKM );
	if ( pIo ) {
		DWORD dwcbWritten;
		AVP_REG_Header rcKLHeader;
		AVP_bool bOk = 1;
		union {
			unsigned int  uiObjType;
			struct {
				unsigned int uiVersion   : 8;		// 0xff
				unsigned int uiBaseType  : 23;	// 0x7fffff
				unsigned int uiIsSpecial : 1;		// 0x1
			};
		} uiType;
		
		uiType.uiVersion = (unsigned int)DSKM_REG_VERSION;
		uiType.uiBaseType = (unsigned int)dwObjectsType;
		uiType.uiIsSpecial = (unsigned int)bSpecialReg;
		
		WriteDWordPtr( &rcKLHeader.uiMagic, AVP_REG_MAGIC );
		WriteDWordPtr( &rcKLHeader.uiObjType, uiType.uiObjType );
		
		if ( !DSKMWriteFile(pIo, hFile, &rcKLHeader.uiMagic, sizeof(rcKLHeader.uiMagic), &dwcbWritten, NULL) ||
				 dwcbWritten != sizeof(rcKLHeader.uiMagic) ) {
			bOk = 0;
		}
		
		if ( bOk ) {
			if ( !DSKMWriteFile(pIo, hFile, &rcKLHeader.uiObjType, sizeof(rcKLHeader.uiObjType), &dwcbWritten, NULL) ||
				dwcbWritten != sizeof(rcKLHeader.uiObjType) ) {
				bOk = 0;
			}
		}
		
		return bOk;
	}
	return 0;
}




//! \fn				: DSKMAPI DSKM_SaveToFileCallBack
//! \brief			:	
//! \return			: static AVP_bool 
//! \param          :  void* pBuffer
//! \param          : AVP_dword nSize
//! \param          : AVP_dword *pdwcbWritten
//! \param          : void* pUserData
static AVP_bool DSKM_SaveToFileCallBack( void* pBuffer, AVP_dword nSize, AVP_dword *pdwcbWritten, void* pUserData ) {
	AVP_bool  bOk = 1;
	AVP_dword dwcbWritten = 0;
	
	if ( pBuffer ) 
		bOk = DSKMWriteFile( ((DSKMCBP *)((SWM_StreamInfo *)pUserData)->pUserData)->m_pIo, ((DSKMCBP *)((SWM_StreamInfo *)pUserData)->pUserData)->m_hFile, pBuffer, nSize, (DWORD *)&dwcbWritten, NULL ) && dwcbWritten == nSize;
	
	if ( pdwcbWritten )
		*pdwcbWritten = dwcbWritten;
	
	return bOk;
}




//! \fn				: DSKM_SerializeRegToFile
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : AVP_dword dwObjectsType
//! \param          : HDATA hRegData
//! \param          : const AVP_char* pRegFileName
AVP_dword DSKMAPI DSKM_SerializeRegToFile( HDSKM hDSKM, AVP_dword dwObjectsType, HDATA hRegData, const AVP_char* pRegFileName ) {
	DSKM_TRY {
		DSKM_Io *pIo = DSKM_GetIO( hDSKM );
		if ( pIo ) {
			AVP_bool bOk = 0;
			
			HANDLE hFile = DSKMCreateFile( pIo, pRegFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_RANDOM_ACCESS, NULL );
			
			if ( hFile == INVALID_HANDLE_VALUE ) {
				return DSKM_ERR_CANNOT_CREATE_REG_FILE;
			}
			{
				AVP_bool bSpecialReg = 0;
				HPROP hProp = DATA_Find_Prop( hRegData, 0, DSKM_KEYREG_ISSPEC_ID );
				if ( hProp ) {
					bSpecialReg = 1;
				}
				if ( !DSKM_WriteRegHeaderToFile(hDSKM, hFile, dwObjectsType, bSpecialReg) ) {
					return DSKM_ERR_CANNOT_WRITE_REG_FILE;
				}
			}

			{
				SWM_Info rcInInfo = { DSKM_SaveToFileCallBack };
				SWM_Info rcOutInfo = { 0 };
				void *pSWHandle;
				void *pSWManager;
				
				SWM_Init_Library( DSKMAllocator, DSKMLiberator ); 
				
				pSWManager = SWM_Create_Manager();
				
				//SWM_Register_CRCOutChecker( pSWManager );
				//SWM_Register_Packer( pSWManager );
				
				pSWHandle = SWM_Prepare_To_Write( pSWManager, &rcInInfo, &rcOutInfo );
				
				if ( pSWHandle && rcOutInfo.m_pProc ) {
					DSKMCBP rcCBP = {pIo, hFile};
					SWM_StreamInfo rcStreamInfo = { pSWHandle, &rcCBP };
					
					AVP_Serialize_Data rcSerData;
					AVP_dword nSerSize;
					AVP_dword nOutSize;
					AVP_dword nBufSize;
					AVP_byte *pBuffer;
					
					ds_mset( &rcSerData, 0, sizeof(rcSerData) );
					
					DATA_Serialize( hRegData, 0, &rcSerData, 0, 0, &nSerSize );
					
					nBufSize = min(nSerSize, BUFFER_SIZE);
					
					pBuffer = DSKMAllocator( nBufSize );
					
					if ( pBuffer ) {
						rcSerData.user_data = (void *)&rcStreamInfo;
						rcSerData.proc = (Buffer_Proc)rcOutInfo.m_pProc;
						
						bOk = DATA_Serialize( hRegData, 0, &rcSerData, pBuffer, nBufSize, &nOutSize ) && 
									nOutSize == nSerSize;
						
						if ( bOk )
							bOk = rcOutInfo.m_pProc( 0, (AVP_dword)-1, 0, (void *)&rcStreamInfo );
						
						DSKMLiberator( pBuffer );
					}
				}
				
				SWM_Shut_Down_Library( pSWHandle );
				SWM_Shut_Down_Library( pSWManager );
			}

			DSKMCloseHandle( pIo, hFile );

			if ( !bOk ) {
				return DSKM_ERR_CANNOT_WRITE_REG_FILE;
			}

			return DSKM_ERR_OK;
		}
		return DSKM_ERR_IO_NOT_INITIALIZED;
	}
	DSKM_EXCEPT
}



#define VALID_BUFFER (pBuffer && dwBufferSize)




//! \fn				: DSKMAPI DSKM_WriteRegHeaderToMemory
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  AVP_byte *pBuffer
//! \param          : AVP_dword dwBufferSize
//! \param          : AVP_dword dwObjectsType
//! \param          : AVP_bool bSpecialReg
static AVP_dword DSKMAPI DSKM_WriteRegHeaderToMemory( AVP_byte *pBuffer, AVP_dword dwBufferSize, AVP_dword dwObjectsType, AVP_bool bSpecialReg ) {
	AVP_dword				dwcbWritten = 0;
	AVP_dword				dwcbWillBeWritten;
	AVP_REG_Header	rcKLHeader;
	AVP_byte			 *pPointer = pBuffer;
	AVP_bool				bOk = 1;
	union {
		unsigned int  uiObjType;
		struct {
			unsigned int uiVersion   : 8;		// 0xff
			unsigned int uiBaseType  : 23;	// 0x7fffff
			unsigned int uiIsSpecial : 1;		// 0x1
		};
	} uiType;
	
	uiType.uiVersion = (unsigned int)DSKM_REG_VERSION;
	uiType.uiBaseType = (unsigned int)dwObjectsType;
	uiType.uiIsSpecial = (unsigned int)bSpecialReg;
	
	WriteDWordPtr( &rcKLHeader.uiMagic, AVP_REG_MAGIC );
	WriteDWordPtr( &rcKLHeader.uiObjType, uiType.uiObjType );
	
	
	dwcbWillBeWritten = sizeof( rcKLHeader.uiMagic );
	if ( !VALID_BUFFER || dwcbWritten + dwcbWillBeWritten <= (AVP_dword)dwBufferSize ) {
		dwcbWritten += dwcbWillBeWritten;
		if ( pBuffer ) {
			ds_mcpy( pPointer, &rcKLHeader.uiMagic, dwcbWillBeWritten );
			pPointer += dwcbWillBeWritten;
		}
	}
	
	dwcbWillBeWritten = sizeof( rcKLHeader.uiObjType );
	if ( !VALID_BUFFER || dwcbWritten + dwcbWillBeWritten <= (AVP_dword)dwBufferSize ) {
		dwcbWritten += dwcbWillBeWritten;
		if ( pBuffer ) {
			ds_mcpy( pPointer, &rcKLHeader.uiObjType, dwcbWillBeWritten );
			pPointer += dwcbWillBeWritten;
		}
	}
	
	return dwcbWritten;
}


// ---
typedef struct {
	AVP_char  *m_pBuffer;
	AVP_dword  m_nBufferSize;
	AVP_dword  m_nWritten;
} SaveToMemInfo;


#define VALID_INFO_BUFFER(a) (((SaveToMemInfo *)((SWM_StreamInfo *)a)->pUserData)->m_pBuffer && ((SaveToMemInfo *)((SWM_StreamInfo *)a)->pUserData)->m_nBufferSize)
#define INFO_POINTER(a)			 (((SaveToMemInfo *)((SWM_StreamInfo *)a)->pUserData)->m_pBuffer)
#define INFO_SIZE(a)				 (((SaveToMemInfo *)((SWM_StreamInfo *)a)->pUserData)->m_nBufferSize)
#define INFO_WRITTEN(a)			 (((SaveToMemInfo *)((SWM_StreamInfo *)a)->pUserData)->m_nWritten)


//! \fn				: DSKMAPI DSKM_SaveToMemoryCallBack
//! \brief			:	
//! \return			: static AVP_bool 
//! \param          :  void* pBuffer
//! \param          : AVP_dword nSize
//! \param          : AVP_dword *pdwcbWritten
//! \param          : void* pUserData
static AVP_bool DSKM_SaveToMemoryCallBack( void* pBuffer, AVP_dword nSize, AVP_dword *pdwcbWritten, void* pUserData ) {
	AVP_bool  bOk = 1;
	
	if ( pBuffer ) {
		if ( !VALID_INFO_BUFFER(pUserData) || (INFO_WRITTEN(pUserData) + nSize) <= INFO_SIZE(pUserData) ) {
			if ( INFO_POINTER(pUserData) )
				ds_mcpy( INFO_POINTER(pUserData) + INFO_WRITTEN(pUserData), pBuffer, nSize );
			
			INFO_WRITTEN(pUserData) += nSize;
		}
		else
			bOk = 0;
	}
	
	if ( bOk && pdwcbWritten )
		*pdwcbWritten = nSize;
	
	return bOk;
}




//! \fn				: DSKM_SerializeRegToBuffer
//! \brief			:	
//! \return			: AVP_dword DSKMAPI 
//! \param          :  HDSKM hDSKM
//! \param          : AVP_dword dwObjectsType
//! \param          : HDATA hRegData
//! \param          : AVP_byte *pBuffer
//! \param          : AVP_dword dwBufferSize
AVP_dword DSKMAPI DSKM_SerializeRegToBuffer( HDSKM hDSKM, AVP_dword dwObjectsType, HDATA hRegData, AVP_byte *pBuffer, AVP_dword dwBufferSize ) {
	DSKM_TRY {
		AVP_dword	nWritten = 0;
		
		SWM_Info rcInInfo = { DSKM_SaveToMemoryCallBack };
		SWM_Info rcOutInfo = { 0 };
		void *pSWHandle;
		void *pSWManager;
			
		SWM_Init_Library( DSKMAllocator, DSKMLiberator ); 
			
		pSWManager = SWM_Create_Manager();
		
		//SWM_Register_CRCOutChecker( pSWManager );
		//SWM_Register_Packer( pSWManager );
		
		pSWHandle = SWM_Prepare_To_Write( pSWManager, &rcInInfo, &rcOutInfo );
		
		if ( pSWHandle && rcOutInfo.m_pProc ) {
			AVP_bool bSpecialReg = 0;
			HPROP hProp = DATA_Find_Prop( hRegData, 0, DSKM_KEYREG_ISSPEC_ID );
			if ( hProp ) {
				bSpecialReg = 1;
			}
			nWritten = DSKM_WriteRegHeaderToMemory( pBuffer, dwBufferSize, dwObjectsType, bSpecialReg );

			if ( nWritten ) {
				AVP_byte          *pTempBuffer;
				AVP_dword					 nTempSize;

				nTempSize = BUFFER_SIZE;
				
				pTempBuffer = DSKMAllocator( nTempSize );

				if ( pTempBuffer ) {
					AVP_bool bOk;
					SWM_StreamInfo rcStreamInfo = { pSWHandle, 0 };
					
					AVP_Serialize_Data rcSerData;
					SaveToMemInfo      rcSaveInfo;
					AVP_dword					 nSerSize;
					
					ds_mset( &rcSerData, 0, sizeof(rcSerData) );
					
					rcSaveInfo.m_pBuffer		 = pBuffer;
					rcSaveInfo.m_nBufferSize = dwBufferSize;
					rcSaveInfo.m_nWritten		 = nWritten;
					
					rcStreamInfo.pUserData = &rcSaveInfo;

					rcSerData.user_data = (void *)&rcStreamInfo;
					rcSerData.proc = (Buffer_Proc)rcOutInfo.m_pProc;

					bOk = DATA_Serialize( hRegData, 0, &rcSerData, pTempBuffer, nTempSize, (AVP_dword *)&nSerSize );

					if ( bOk )
						bOk = rcOutInfo.m_pProc( 0, (AVP_dword)-1, 0, (void *)&rcStreamInfo );

					nWritten = rcSaveInfo.m_nWritten;

					if ( !bOk )
						nWritten = 0;

					DSKMLiberator( pTempBuffer );
				}
			}
		}
		
		SWM_Shut_Down_Library( pSWHandle );
		SWM_Shut_Down_Library( pSWManager );
		
		return nWritten;
	}
	DSKM_EXCEPT_SIZE
}
