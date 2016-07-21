// SWManagerRead.c - implementation of serialization wrapper's manager
// Reading part of a code
//
#include <string.h>

#include "Manager.h"

// ---
SWM_PROC void * SWM_PARAM SWM_Prepare_To_Read( void *pBaseManager, SWM_Info *pSrcInfo, SWM_Info *pDestInfo ) {
	SWManager *pManager = 0;

	if ( pSrcInfo && pDestInfo ) {

		if ( !pBaseManager )
			pBaseManager = (gpSWManager ? gpSWManager : (gpSWManager = SWMCreateInstance(0)));

		if ( pBaseManager && ((SWManager *)pBaseManager)->lpVtbl ) {
			if ( !((SWManager *)pBaseManager)->lpVtbl->SWMIsSWRegistered(pBaseManager, AVP_SW_PID_RAWDATA) )
				SWM_Register_UnRawData( pBaseManager );

			if ( ((SWManager *)pBaseManager)->lpVtbl->SWMIsSWRegistered(pBaseManager, AVP_SW_PID_RAWDATA) ) {
				pManager = SWMCreateInstance( pBaseManager );

				if ( pManager ) {
					SWDescriptor *pSWTable = pManager->m_pSWTable;
					int i;

					pDestInfo->m_pProc = 0;

					for ( i=0; pSWTable->m_nSWId != (unsigned long)-1; i++,pSWTable++ ) {
						if ( pSWTable->m_nSWId == AVP_SW_PID_RAWDATA ) {
							pSWTable->m_piSWControl->m_pSWContext->m_nLevel = i + 1;

							pDestInfo->m_pProc	= pSWTable->m_piSWControl->m_pSWContext->m_pReadProc;

							pSWTable->m_piSWControl->m_pSWContext->m_pSiblingProc = pSrcInfo->m_pProc;
						}
						else
							pSWTable->m_piSWControl->m_pSWContext->m_nLevel = 0;
					}
				}
			}
		}
	}
	return pManager;
}



