/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	ds_inite.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	9/5/2005 6:29:43 PM
*		
*		Example:	
*		
*		
*		
*/		



#include "dskm.h"
#include "dskmi.h"
#include "cl_util.h"


//! \fn				: DSKM_InitLibraryEx
//! \brief			:	Инициирование библиотеки.
//! \return			: Контекст библиотеки. 0 - операция неуспешна. 
//! \param          : (*pfnAlloc)(AVP_uint) - указатель на функцию распределения памяти
//! \param          : void (*pfnFree((void *)	- указатель на функцию освобождения памяти
//! \param          : DSKM_Io *pIo - указатель на интерфейс ввода/вывода (НЕ может быть 0)
//! \param          : AVP_bool bInitSubLib - нужно ли инициировать вспомогательные библиотеки (Property.lib)
HDSKM	    DSKMAPI DSKM_InitLibraryEx( void* (*pfnAlloc)(AVP_size_t), void (*pfnFree)(void*), DSKM_Io *pIo, AVP_bool bInitSubLib ) {
	DSKM_TRY {
		if ( !pfnAlloc || !pfnFree ||
			(DSKMAllocator && pfnAlloc != DSKMAllocator) ||
			(DSKMLiberator && pfnFree  != DSKMLiberator) ) {
			return 0;
		}
		else {          
			DSKMAllocator = pfnAlloc;
			DSKMLiberator = pfnFree;
			if ( !bInitSubLib || DATA_Init_Library(pfnAlloc, pfnFree) ) {
				HDSKM hDSKM = (HDSKM)DATA_Add( 0, 0, DSKM_ROOT_PID, 0, 0 );
				if ( hDSKM ) {
					DSKM_FTABLE_ADD(hDSKM);

					if ( pIo && pIo->lpVtbl ) {
						DATA_Add_Prop( (HDATA)hDSKM, 0, DSKM_IO_ID, (AVP_size_t)pIo, 0 );
						pIo->lpVtbl->AddRef( pIo );
					}

					/*{
						HDATA hData = DATA_Add( 0, 0, DSKM_OBJECT_ERROR_ID, 0, 0 );
						DATA_Add( hData, 0, DSKM_OBJECT_ERROR_IDA(RET_OK), DSKM_ERR_OK, 0 );
						DATA_Add( hData, 0, DSKM_OBJECT_ERROR_IDA(RET_CONTEXTNOTCLEAR), DSKM_ERR_CRIPTO_LIB, 0 );
						DATA_Add( hData, 0, DSKM_OBJECT_ERROR_IDA(RET_CONTEXTNOTREADY), DSKM_ERR_CRIPTO_LIB, 0 );
						DATA_Add( hData, 0, DSKM_OBJECT_ERROR_IDA(RET_KEYNOTPRESENT), DSKM_ERR_KEY_NOT_FOUND, 0 );
						DATA_Add( hData, 0, DSKM_OBJECT_ERROR_IDA(RET_KEYALREADYPRESENT), DSKM_ERR_CRIPTO_LIB, 0 );
						DATA_Add( hData, 0, DSKM_OBJECT_ERROR_IDA(RET_INCORRECTSIGN), DSKM_ERR_INVALID_SIGN, 0 );
						DATA_Add( hData, 0, DSKM_OBJECT_ERROR_IDA(RET_ZERO_R), DSKM_ERR_CRIPTO_LIB, 0 );
						DATA_Add( hData, 0, DSKM_OBJECT_ERROR_IDA(RET_ZERO_S), DSKM_ERR_CRIPTO_LIB, 0 );
						DATA_Add( hData, 0, DSKM_OBJECT_ERROR_IDA(RET_BAD_R), DSKM_ERR_CRIPTO_LIB, 0 );
						DATA_Add( hData, 0, DSKM_OBJECT_ERROR_IDA(RET_BAD_S), DSKM_ERR_CRIPTO_LIB, 0 );
						DATA_Add( hData, 0, DSKM_OBJECT_ERROR_IDA(RET_FILE_OPEN), DSKM_ERR_CANNOT_OPEN_REG_FILE, 0 );
						DATA_Add( hData, 0, DSKM_OBJECT_ERROR_IDA(RET_FILE_SEEK), DSKM_ERR_CANNOT_READ_REG_FILE, 0 );
						DATA_Add( hData, 0, DSKM_OBJECT_ERROR_IDA(RET_FILE_READ), DSKM_ERR_CANNOT_READ_REG_FILE, 0 );
						DATA_Add( hData, 0, DSKM_OBJECT_ERROR_IDA(RET_FILE_CLOSE), DSKM_ERR_CRIPTO_LIB, 0 );
						DATA_Add( hData, 0, DSKM_OBJECT_ERROR_IDA(RET_INCORRECTCONTEXT), DSKM_ERR_CRIPTO_LIB, 0 );
						DATA_Add( hData, 0, DSKM_OBJECT_ERROR_IDA(RET_INVALIDPARAMETER), DSKM_ERR_INVALID_PARAMETER, 0 );
						DATA_Add( hData, 0, DSKM_OBJECT_ERROR_IDA(RET_INCORRECTSIZE), DSKM_ERR_INVALID_BUFFER, 0 );
						
						DSKM_SerializeRegToFile( hDSKM, 0, hData, "errtable.txt" );
						DATA_Remove( hData, 0 );
					}*/
						
						
					if ( DSKM_NOT_OK(DSKM_InitCriptoLibrary(hDSKM)) ) {
						DATA_Remove( (HDATA)hDSKM, 0 );
						hDSKM = 0;
					}
					return hDSKM;
				}
			}
			return 0;
		}
	}
	DSKM_EXCEPT_SIZE
}



