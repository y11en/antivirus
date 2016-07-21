/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	initlib.c
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	6/6/2005 3:44:23 PM
*		
*		Example:	
*		
*		
*		
*/		

#include "dskm.h"
#include "dskmi.h"
#include "cl_util.h"

//! \fn				: DSKM_InitLibrary
//! \brief			:	
//! \return			: HDSKM DSKMAPI 
//! \param          : *pfnAlloc)(AVP_uint)
//! \param          : (*pfnFree)(void*)
//! \param          : AVP_bool bInitSubLib
HDSKM DSKMAPI DSKM_InitLibrary( void* (*pfnAlloc)(AVP_size_t), void (*pfnFree)(void*), AVP_bool bInitSubLib  ) {
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
					if ( DSKM_OK(DSKM_InitCriptoLibrary(hDSKM)) &&
							 DSKM_LoadIO(hDSKM) && DSKM_LoadFF(hDSKM) )	{
						DSKM_FTABLE_ADD(hDSKM);
						return hDSKM;
					}
					DATA_Remove( (HDATA)hDSKM, 0 );
					hDSKM = 0;
				}
			}
			return 0;
		}
	}
	DSKM_EXCEPT_SIZE
}



