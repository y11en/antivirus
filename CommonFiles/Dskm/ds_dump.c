/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_dump.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	6/21/2005 5:04:40 PM
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
#include <stdio.h>


#define BIN_MAX (0xffffffff)
#define VALUE_ADJUST 25

typedef struct tagDumpStr {
	unsigned int dwValue;
	AVP_char*		 pValueStr;
}	DumpStr;

#define MAKE_ENTRY(code)    { (unsigned int)code, (#code) }


// ---
static DumpStr gDumpNames[] = {
	MAKE_ENTRY(DSKM_OTYPE_KEY_KL_MASTER_PRIVATE),
	MAKE_ENTRY(DSKM_OTYPE_KEY_KL_MASTER_PUBLIC),
	MAKE_ENTRY(DSKM_OTYPE_KEY_KL_PRIVATE),
	MAKE_ENTRY(DSKM_OTYPE_KEY_KL_PUBLIC),
	MAKE_ENTRY(DSKM_OTYPE_KEY_OPERABLE_PRIVATE),
	MAKE_ENTRY(DSKM_OTYPE_KEY_OPERABLE_PUBLIC),
	MAKE_ENTRY(DSKM_OTYPE_ASSOC_FILE),
};

static int gnDumpNamesCount = sizeof(gDumpNames) / sizeof(gDumpNames[0]);


//! \fn				: DSKMAPI DSKM_DumpStr
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  DSKM_Io *pIo
//! \param          : HANDLE hFile
//! \param          : const AVP_char* pStr
static AVP_dword DSKMAPI DSKM_DumpStr( DSKM_Io *pIo, HANDLE hFile, const AVP_char* pStr ) {
	DWORD dwToWrite = ds_slen( pStr );
	DWORD dwWritten = 0;
	DSKMWriteFile( pIo, hFile, pStr, dwToWrite, &dwWritten, 0 );
	return dwWritten;
}




//! \fn				: DSKMAPI DSKM_DumpDWord
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  DSKM_Io *pIo
//! \param          : HANDLE hFile
//! \param          : const AVP_char* pFormat
//! \param          : AVP_dword dwValue
static AVP_dword DSKMAPI DSKM_DumpDWord( DSKM_Io *pIo, HANDLE hFile, const AVP_char* pFormat, AVP_dword dwValue ) {
	char pBuffer[100];
	sprintf( pBuffer, pFormat, dwValue );
	return DSKM_DumpStr( pIo, hFile, pBuffer );
}




//! \fn				: DSKMAPI DSKM_DumpByte
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  DSKM_Io *pIo
//! \param          : HANDLE hFile
//! \param          : AVP_char cValue
static AVP_dword DSKMAPI DSKM_DumpByte( DSKM_Io *pIo, HANDLE hFile, AVP_char cValue ) {
	char pBuffer[100];
	sprintf( pBuffer, "%.2X", (unsigned char)cValue );
	return DSKM_DumpStr( pIo, hFile, pBuffer );
}



//! \fn				: DSKMAPI DSKM_FindTypeStr
//! \brief			:	
//! \return			: static AVP_char* 
//! \param          : unsigned short dwType
static AVP_char* DSKMAPI DSKM_FindTypeStr( unsigned int dwType ) {
	int i;
	for ( i=0; i<gnDumpNamesCount; i++ ) {
		if ( gDumpNames[i].dwValue == dwType ) {
			return gDumpNames[i].pValueStr;
		}
	}
	return 0;
}





//! \fn				: DSKMAPI DSKM_DumpAdjust
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  DSKM_Io *pIo
//! \param          : HANDLE hFile
//! \param          : AVP_dword dwWritten
//! \param          : AVP_dword dwAdjust
static AVP_dword DSKMAPI DSKM_DumpAdjust( DSKM_Io *pIo, HANDLE hFile, AVP_dword dwWritten, AVP_dword dwAdjust ) {
	while ( dwWritten < dwAdjust ) {
		dwWritten += DSKM_DumpStr( pIo, hFile, " " );
	}
	return dwWritten;
}



//! \fn				: DSKMAPI DSKM_DumpType
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  DSKM_Io *pIo
//! \param          : HANDLE hFile
//! \param          : unsigned short dwType
static AVP_dword DSKMAPI DSKM_DumpType( DSKM_Io *pIo, HANDLE hFile, unsigned int dwType ) {
	AVP_dword dwWritten = 0;
	if ( dwWritten += DSKM_DumpStr(pIo, hFile, "Registry type ") ) {
		AVP_char* pTypeName = DSKM_FindTypeStr( dwType );
		if ( pTypeName ) {
			dwWritten += DSKM_DumpStr( pIo, hFile, pTypeName );
			dwWritten += DSKM_DumpStr( pIo, hFile, "\r\n" );
		}
		else {
			dwWritten += DSKM_DumpDWord( pIo, hFile, "%d", dwType );
			dwWritten += DSKM_DumpStr( pIo, hFile, "\r\n" );
		}

	}
	return dwWritten;
}



//! \fn				: DSKMAPI DSKM_DumpParamList
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  DSKM_Io *pIo
//! \param          : HANDLE hDumpFile
//! \param          : HDATA hCurrRegData
static AVP_dword DSKMAPI DSKM_DumpParamList( DSKM_Io *pIo, HANDLE hDumpFile, HDATA hCurrRegData ) {
	AVP_dword i = 0;
	AVP_dword dwParIndex = 0;
	AVP_dword dwWritten = 0;
	HPROP hCurrRegProp = DATA_Get_First_Prop( hCurrRegData, 0 );
	while ( hCurrRegProp ) {
		AVP_dword dwCurrPID = PROP_Get_Id( hCurrRegProp );
		dwWritten = 0;
		switch ( GET_AVP_PID_APP(dwCurrPID) ) {
			case DSKM_APP_DWORD_PARAM : {
				AVP_dword dwRegValue;
				PROP_Get_Val( hCurrRegProp, &dwRegValue, sizeof(dwRegValue) );
				
				dwWritten += DSKM_DumpStr( pIo, hDumpFile, "\tParam " );
				dwWritten += DSKM_DumpDWord( pIo, hDumpFile, "%d", dwParIndex++ );
				dwWritten += DSKM_DumpDWord( pIo, hDumpFile, " (ID=%d)", GET_AVP_PID_ID(dwCurrPID) );
				
				dwWritten += DSKM_DumpStr( pIo, hDumpFile, " (DWORD)" );
				
				DSKM_DumpAdjust( pIo, hDumpFile, dwWritten, VALUE_ADJUST );
				
				DSKM_DumpDWord( pIo, hDumpFile, "0x%x", dwRegValue );
				DSKM_DumpStr( pIo, hDumpFile, "\r\n" );
				break;
			}
			case DSKM_APP_STR_PARAM : {
				AVP_dword dwRegStrSize = 0;
				AVP_char *pRegStr = DSKM_GetPropValueAsString( hCurrRegProp, &dwRegStrSize );
				if ( pRegStr ) {
					dwWritten += DSKM_DumpStr( pIo, hDumpFile, "\tParam " );
					dwWritten += DSKM_DumpDWord( pIo, hDumpFile, "%d", dwParIndex++ );
					dwWritten += DSKM_DumpDWord( pIo, hDumpFile, " (ID=%d)", GET_AVP_PID_ID(dwCurrPID) );
					
					dwWritten += DSKM_DumpStr( pIo, hDumpFile, " (STR," );
					dwWritten += DSKM_DumpDWord( pIo, hDumpFile, "%d", dwRegStrSize );
					dwWritten += DSKM_DumpStr( pIo, hDumpFile, ")" );
					
					DSKM_DumpAdjust( pIo, hDumpFile, dwWritten, VALUE_ADJUST );
					
					DSKM_DumpStr( pIo, hDumpFile, pRegStr );
					DSKM_DumpStr( pIo, hDumpFile, "\r\n" );
					DSKMLiberator( pRegStr );
				}
				break;
			}
			case DSKM_APP_BPTR_PARAM : {
				AVP_dword dwRegBufferSize = PROP_Get_Val( hCurrRegProp, 0, 0 );
				if ( dwRegBufferSize ) {
					void *pRegBuffer = DSKMAllocator( dwRegBufferSize );
					if ( pRegBuffer ) {
						dwWritten += DSKM_DumpStr( pIo, hDumpFile, "\tParam " );
						dwWritten += DSKM_DumpDWord( pIo, hDumpFile, "%d", dwParIndex++ );
						dwWritten += DSKM_DumpDWord( pIo, hDumpFile, " (ID=%d)", GET_AVP_PID_ID(dwCurrPID) );
						
						dwWritten += DSKM_DumpStr( pIo, hDumpFile, " (BIN," );
						dwWritten += DSKM_DumpDWord( pIo, hDumpFile, "%d", dwRegBufferSize );
						dwWritten += DSKM_DumpStr( pIo, hDumpFile, ")" );
						
						DSKM_DumpAdjust( pIo, hDumpFile, dwWritten, VALUE_ADJUST );
						
						PROP_Get_Val( hCurrRegProp, pRegBuffer, dwRegBufferSize );
						for ( i=0; i<min(BIN_MAX,dwRegBufferSize); i++ ) {
							DSKM_DumpByte( pIo, hDumpFile, ((char *)pRegBuffer)[i] );
							DSKM_DumpStr( pIo, hDumpFile, " " );
						}
						
						DSKM_DumpStr( pIo, hDumpFile, "\r\n" );
						
						DSKMLiberator(pRegBuffer);
					}
				}
				break;
			}
		}
		hCurrRegProp = PROP_Get_Next( hCurrRegProp );
	}
	return DSKM_ERR_OK;
}	


//! \fn				: DSKMAPI DSKM_DumpKeyTree
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  DSKM_Io *pIo
//! \param          : HANDLE hDumpFile
//! \param          : HDATA hRegData
static AVP_dword DSKMAPI DSKM_DumpKeyTree( DSKM_Io *pIo, HANDLE hDumpFile, HDATA hRegData ) {
	AVP_dword i = 0;
	AVP_dword dwResult = DSKM_ERR_OK;
	HDATA hCurrRegData = DATA_Get_First( hRegData, 0 );
	while ( hCurrRegData && DSKM_OK(dwResult) ) {
		AVP_dword dwWritten = 0;
		{
			AVP_char* pTypeName = 0;
			AVP_dword dwRegObjType = 0;
			DATA_Get_Val( hCurrRegData, 0, DSKM_KEYREG_OBJTYPE_ID, &dwRegObjType, sizeof(dwRegObjType) );

			pTypeName = DSKM_FindTypeStr( (unsigned short)dwRegObjType );

			dwWritten += DSKM_DumpStr(pIo, hDumpFile, "\tObjects type" );
			DSKM_DumpAdjust( pIo,hDumpFile, dwWritten, VALUE_ADJUST );
			if ( pTypeName ) {
				dwWritten += DSKM_DumpStr( pIo, hDumpFile, pTypeName );
			}
			else {
				dwWritten += DSKM_DumpDWord( pIo, hDumpFile, "%d", dwRegObjType );
			}
			dwWritten += DSKM_DumpStr( pIo, hDumpFile, "\r\n" );
		}

		if ( dwWritten ) {
			AVP_dword dwKeySize = DATA_Get_Val( hCurrRegData, 0, 0, 0, 0 );
			dwWritten = 0;
			if ( dwKeySize ) {
				void *pKey = DSKMAllocator( dwKeySize );
				if ( pKey ) {
					dwWritten += DSKM_DumpStr( pIo, hDumpFile, "\tKey value" );
					DSKM_DumpAdjust( pIo, hDumpFile, dwWritten, VALUE_ADJUST );
					DATA_Get_Val( hCurrRegData, 0, 0, pKey, dwKeySize );
					for ( i=0; i<min(BIN_MAX,dwKeySize); i++ ) {
						DSKM_DumpByte( pIo, hDumpFile, ((char *)pKey)[i] );
						DSKM_DumpStr( pIo, hDumpFile, " " );
					}
					DSKMLiberator( pKey );
					dwWritten += DSKM_DumpStr( pIo, hDumpFile, "\r\n" );
				}
			}
			if ( dwWritten ) {
				DSKM_DumpParamList( pIo, hDumpFile, hCurrRegData );
			}
		}
		
		DSKM_DumpStr( pIo, hDumpFile, "\r\n" );
		hCurrRegData = DATA_Get_Next( hCurrRegData, 0 );
	}
	return dwResult;
}



//! \fn				: DSKMAPI DSKM_DumpAssocTree
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  DSKM_Io *pIo
//! \param          : HANDLE hDumpFile
//! \param          : HDATA hRegData
static AVP_dword DSKMAPI DSKM_DumpAssocTree( DSKM_Io *pIo, HANDLE hDumpFile, HDATA hRegData ) {
	AVP_dword dwResult = DSKM_ERR_OK;
	HDATA hCurrRegData = DATA_Get_First( hRegData, 0 );
	while ( hCurrRegData && DSKM_OK(dwResult) ) {
		AVP_dword dwWritten = 0;
		{
			AVP_char* pTypeName = 0;
			AVP_dword dwRegObjType = 0;
			DATA_Get_Val( hCurrRegData, 0, 0, &dwRegObjType, sizeof(dwRegObjType) );
			
			pTypeName = DSKM_FindTypeStr( (unsigned short)dwRegObjType );
			
			dwWritten += DSKM_DumpStr(pIo, hDumpFile, "\tObjects type" );
			DSKM_DumpAdjust( pIo,hDumpFile, dwWritten, VALUE_ADJUST );
			if ( pTypeName ) {
				dwWritten += DSKM_DumpStr( pIo, hDumpFile, pTypeName );
			}
			else {
				dwWritten += DSKM_DumpDWord( pIo, hDumpFile, "%d", dwRegObjType );
			}
			dwWritten += DSKM_DumpStr( pIo, hDumpFile, "\r\n" );
		}
			
		if ( dwWritten ) {
				DSKM_DumpParamList( pIo, hDumpFile, hCurrRegData );
		}
		
		DSKM_DumpStr( pIo, hDumpFile, "\r\n" );
		hCurrRegData = DATA_Get_Next( hCurrRegData, 0 );
	}
	return dwResult;
}





//! \fn				: DSKMAPI DSKM_DumpHashTree
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  DSKM_Io *pIo
//! \param          : HANDLE hDumpFile
//! \param          : HDATA hRegData
static AVP_dword DSKMAPI DSKM_DumpHashTree( DSKM_Io *pIo, HANDLE hDumpFile, HDATA hRegData ) {
	AVP_dword dwResult = DSKM_ERR_OK;
	HPROP hHashArrPop = DATA_Find_Prop( hRegData, 0, DSKM_OBJECT_REG_ARR_HASH_ID );
	if ( hHashArrPop ) {
		AVP_bool bFound = 0;
		AVP_dword i = 0;
		AVP_dword dwCount = PROP_Arr_Count( hHashArrPop );
		for( i=0; i<dwCount; i++ ) {
			AVP_dword dwHashSize = PROP_Arr_Get_Items( hHashArrPop, i, 0, 0 );
			DSKM_DumpStr( pIo, hDumpFile, "\tHash value\t" );
			if ( dwHashSize ) {
				void *pHash = DSKMAllocator( dwHashSize );
				if ( pHash ) {
					AVP_dword j=0;
					PROP_Arr_Get_Items( hHashArrPop, i, pHash, dwHashSize );
					for ( j=0; j<min(BIN_MAX,dwHashSize); j++ ) {
						DSKM_DumpByte( pIo, hDumpFile, ((char *)pHash)[j] );
						DSKM_DumpStr( pIo, hDumpFile, " " );
					}
					DSKMLiberator( pHash );
					if ( !DSKM_DumpStr(pIo, hDumpFile, "\r\n") ) 
						dwResult = DSKM_ERR_CANNOT_WRITE_REG_FILE;
				}
			}
		}
	}
	else {
		HDATA hCurrRegData = DATA_Get_First( hRegData, 0 );
		while ( hCurrRegData && DSKM_OK(dwResult) ) {
			AVP_dword dwHashSize = DATA_Get_Val( hCurrRegData, 0, 0, 0, 0 );
			DSKM_DumpStr( pIo, hDumpFile, "\tHash value\t" );
			if ( dwHashSize ) {
				void *pHash = DSKMAllocator( dwHashSize );
				if ( pHash ) {
					AVP_dword j=0;
					DATA_Get_Val( hCurrRegData, 0, 0, pHash, dwHashSize );
					for ( j=0; j<min(BIN_MAX,dwHashSize); j++ ) {
						DSKM_DumpByte( pIo, hDumpFile, ((char *)pHash)[j] );
						DSKM_DumpStr( pIo, hDumpFile, " " );
					}
					DSKMLiberator( pHash );
					if ( !DSKM_DumpStr(pIo, hDumpFile, "\r\n") ) 
						dwResult = DSKM_ERR_CANNOT_WRITE_REG_FILE;
				}
			}
			hCurrRegData = DATA_Get_Next( hCurrRegData, 0 );
		}
	}
	return dwResult;
}





//! \fn				: DSKMAPI DSKM_DumpRegDataTree
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  DSKM_Io *pIo
//! \param          : HANDLE hDumpFile
//! \param          : AVP_REG_Header *pKLHeader
//! \param          : HDATA hRegData
static AVP_dword DSKMAPI DSKM_DumpRegDataTree( DSKM_Io *pIo, HANDLE hDumpFile, AVP_REG_Header *pKLHeader, HDATA hRegData ) {
	AVP_dword dwResult = DSKM_ERR_OK;
	switch ( pKLHeader->uiBaseType ) {
		case (unsigned int)DSKM_OTYPE_KEY_KL_MASTER_PRIVATE	:
		case (unsigned int)DSKM_OTYPE_KEY_KL_MASTER_PUBLIC	:
		case (unsigned int)DSKM_OTYPE_KEY_KL_PRIVATE				:
		case (unsigned int)DSKM_OTYPE_KEY_KL_PUBLIC					:
		case (unsigned int)DSKM_OTYPE_KEY_OPERABLE_PRIVATE	:
		case (unsigned int)DSKM_OTYPE_KEY_OPERABLE_PUBLIC		: {
			while ( 1 ) {
				if ( DSKM_NOT_OK(dwResult = DSKM_DumpKeyTree(pIo, hDumpFile, hRegData)) )						{ break; }
				break;
			}
			break;
		}
		case (unsigned short)DSKM_OTYPE_LICENCE_KEY           :
		case (unsigned short)DSKM_OTYPE_LICENCE_KEY_ORDER     :  
			dwResult = DSKM_ERR_INVALID_OBJTYPE;
			break;
		case (unsigned short)DSKM_OTYPE_ASSOC_FILE : {
			while ( 1 ) {
				if ( DSKM_NOT_OK(dwResult = DSKM_DumpAssocTree(pIo, hDumpFile, hRegData)) )						{ break; }
				break;
			}
			break;
		}
		default: {
			while ( 1 ) {
				if ( DSKM_NOT_OK(dwResult = DSKM_DumpHashTree(pIo, hDumpFile, hRegData)) )						{ break; }
				break;
			}
			break;
		}
	}
	return dwResult;
}



//! \fn				: DSKMAPI DSKM_DumpRegData
//! \brief			:	
//! \return			: static AVP_dword 
//! \param          :  DSKM_Io *pIo
//! \param          : AVP_REG_Header *pKLHeader
//! \param          : HDATA hRegData
//! \param          : const AVP_char* pszRegFileName
//! \param          : const AVP_char* pszOutFileName
static AVP_dword DSKMAPI DSKM_DumpRegData( DSKM_Io *pIo, AVP_REG_Header *pKLHeader, HDATA hRegData, const AVP_char* pszRegFileName, const AVP_char* pszOutFileName ) {
	AVP_dword dwResult = DSKM_ERR_CANNOT_CREATE_REG_FILE;
	HANDLE hDumpFile = DSKMCreateFile( pIo, pszOutFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_RANDOM_ACCESS, NULL );
	if ( hDumpFile != INVALID_HANDLE_VALUE ) {
		dwResult = DSKM_ERR_CANNOT_WRITE_REG_FILE;
		while ( 1 ) {
			if ( !DSKM_DumpStr(pIo, hDumpFile, "Source file \"") )						{ break; }
			if ( !DSKM_DumpStr(pIo, hDumpFile, pszRegFileName) )							{ break; }
			if ( !DSKM_DumpStr(pIo, hDumpFile, "\"\r\n") )										{ break; }
			if ( !DSKM_DumpType(pIo, hDumpFile, pKLHeader->uiBaseType) )			{ break; }
			if ( !DSKM_DumpStr(pIo, hDumpFile, "Registry nodes\r\n") )				{ break; }
			if ( DSKM_NOT_OK(dwResult = DSKM_DumpRegDataTree(pIo, hDumpFile, pKLHeader, hRegData)) ) { break; }
			break;
		}
		DSKMCloseHandle( pIo, hDumpFile );
	}
	return dwResult;
}



//! \fn				:  DSKMAPI DSKM_DumpReg
//! \brief			:	
//! \return			: AVP_dword  
//! \param          :  HDSKM hDSKM
//! \param          : const AVP_char* pszRegFileName
//! \param          : const AVP_char* pszOutFileName
AVP_dword   DSKMAPI DSKM_DumpReg( HDSKM hDSKM, const AVP_char* pszRegFileName, const AVP_char* pszOutFileName ) {
	if ( pszRegFileName && *pszRegFileName && pszOutFileName && *pszOutFileName ) {
		DSKM_Io *pIo = DSKM_GetIO( hDSKM );
		if ( pIo ) {
			AVP_dword dwResult = DSKM_ERR_CANNOT_OPEN_REG_FILE;
			HANDLE	hRegFile = DSKMCreateFile( pIo, pszRegFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL );
			if ( hRegFile != INVALID_HANDLE_VALUE ) {
				AVP_REG_Header rcKLHeader;
				dwResult = DSKM_ReadRegHeaderFromFile( hDSKM, hRegFile, 0, &rcKLHeader );
				if ( DSKM_OK(dwResult) ) {
					HDATA hRegData = 0;
					dwResult = DSKM_DeserializeRegFile( hDSKM, pszRegFileName, 0, &hRegData );
					if ( DSKM_OK(dwResult) ) {
						dwResult = DSKM_DumpRegData( pIo, &rcKLHeader, hRegData, pszRegFileName, pszOutFileName );
						DATA_Remove( hRegData, 0 );
					}
				}
				DSKMCloseHandle( pIo, hRegFile );
			}
			return dwResult;
		}
		return DSKM_ERR_IO_NOT_INITIALIZED;
	}
	
	return DSKM_ERR_INVALID_PARAMETER;
}

