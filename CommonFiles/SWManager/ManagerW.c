// SWManagerWrite.c - implementation of serialization wrapper's manager
// Writing part of a code
//
#include <string.h>

#include "Manager.h"

// ---
SWM_PROC void * SWM_PARAM SWM_Prepare_To_Write( void *pBaseManager, SWM_Info *pSrcInfo, SWM_Info *pDestInfo ) {
	SWManager *pManager = 0;

	if ( pSrcInfo && pDestInfo ) {

		if ( !pBaseManager )
			pBaseManager = (gpSWManager ? gpSWManager : (gpSWManager = SWMCreateInstance(0)));

		if ( pBaseManager && ((SWManager *)pBaseManager)->lpVtbl ) {
			if ( !((SWManager *)pBaseManager)->lpVtbl->SWMIsSWRegistered(pBaseManager, AVP_SW_PID_RAWDATA) )
				if ( !SWM_Register_RawData(pBaseManager) )
					return 0;

			pManager = SWMCreateInstance( pBaseManager );

			if ( pManager ) {
				SWDescriptor *pSWTable = pManager->m_pSWTable;
				int i;
				for ( i=0; pSWTable->m_nSWId != (unsigned long)-1; i++,pSWTable++ ) {
					pSWTable->m_piSWControl->m_pSWContext->m_nLevel = i + 1;
					if ( i == 0 ) {
						pDestInfo->m_pProc = pSWTable->m_piSWControl->m_pSWContext->m_pWriteProc;
					}

					if ( pManager->m_pSWTable[i + 1].m_nSWId != (unsigned long)-1 ) {
						pSWTable->m_piSWControl->m_pSWContext->m_pSiblingProc = pManager->m_pSWTable[i+1].m_piSWControl->m_pSWContext->m_pWriteProc;
					}
					else {
						pSWTable->m_piSWControl->m_pSWContext->m_pSiblingProc = pSrcInfo->m_pProc;
					}
				}
			}
		}
	}
	return pManager;
}



