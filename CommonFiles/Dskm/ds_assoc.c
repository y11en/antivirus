/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_assoc.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	7/7/2005 7:28:37 PM
*		
*		Example:	
*		
*		
*		
*/		


#include "dskm.h"
#include "dskmi.h"


//! \fn				: DSKM_SaveTypeAssociationToFile
//! \brief			:	Сохранить ассоциацию "object/key params => objects type" в файл
//! \return			: DSKMAPI 
//! \return			: Код ошибки
//! \param          : HDSKM hDSKM - контекст библиотеки. 
//! \param          : HDSKMLIST hObjectsList - список параметров
//! \param          : AVP_dword dwObjectsType	- тип объектов
//! \param          : const AVP_char* pszAssocFileName - имя файла ассоциаций (файл не подписывается)
AVP_dword  	DSKMAPI DSKM_SaveTypeAssociationToFile( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, const AVP_char* pszAssocFileName ) {
	DSKM_TRY {
		AVP_dword dwResult = DSKM_ERR_INVALID_PARAMETER;
		
		if ( !DSKMAllocator || !DSKMAllocator ) {
			return DSKM_ERR_NOT_INITIALIZED;
		}
		
		if ( hDSKM && hObjectsList ) {
			HDATA hAssocRootData = 0;
			dwResult = DSKM_DeserializeRegFile( hDSKM, pszAssocFileName, DSKM_OTYPE_ASSOC_FILE, &hAssocRootData );
			
			if ( !hAssocRootData && dwResult == DSKM_ERR_CANNOT_OPEN_REG_FILE )
				hAssocRootData = DATA_Add( 0, 0, DSKM_AS_HEAD_PID, 0, 0 );
			
			dwResult = DSKM_ERR_DATA_PROCESSING;
			if ( hAssocRootData ) {
				AVP_dword dwPID = DSKM_ASSOC_OTYPE_ID( 0 );
				dwPID = DSKM_FindUniquePID( (HDATA)hAssocRootData, dwPID );
				
				if ( dwPID ) {
					HDATA hNode = DATA_Add( hAssocRootData, 0, dwPID, dwObjectsType, 0 );
					if ( hNode ) {
						dwResult = DSKM_SaveParamsToTypeNode( hObjectsList, hNode );
					}
				}	

				if ( DSKM_OK(dwResult) )
					dwResult = DSKM_SerializeRegToFile( hDSKM, DSKM_OTYPE_ASSOC_FILE, hAssocRootData, pszAssocFileName );
					
				DATA_Remove( hAssocRootData, 0 );
			}
		}	

		return dwResult;
	}
	DSKM_EXCEPT
}

