// SWManager.c - implementation of serialization wrapper's manager
// Main part of a code
//
#include <string.h>
#include "AVPPort.h"

#include "Manager.h"

void* (* SWAllocator)(AVP_size_t) = 0;
void  (* SWLiberator)(void*) = 0;

void  (* SWLocker)(void*);
void  (* SWUnlocker)(void*);
void* SWLContext; 


SWManager *gpSWManager = 0;


// ---
SWM_PROC AVP_bool SWM_PARAM SWM_Init_Library( void* (*alloc)(AVP_size_t), void (*free)(void*) ) {
  if ( !alloc || !free || 
       (SWAllocator && alloc != SWAllocator) ||
       (SWLiberator && free  != SWLiberator) ) {
    _RPT0( _CRT_ASSERT, "SWM : Library already inited" );
    return 0;
  }
  else {          
		SWAllocator = alloc;
		SWLiberator = free;
    return 1;
  }
}

// ---
SWM_PROC AVP_bool SWM_PARAM SWM_Init_LibraryEx( void* (*alloc)(AVP_size_t), void (*free)(void*), void (*lock)(void *), void (*unlock)(void *), void* cont) {
	if ( lock && unlock ) {
		AVP_bool res;
		
		lock( cont );
		
		res = SWM_Init_Library( alloc, free );
		
		SWLocker = lock;
		SWUnlocker = unlock;
		SWLContext = cont; 

		unlock( cont );
		return res;
	}
	return SWM_Init_Library( alloc, free );
}


// ---
static SWM_PROC AVP_bool SWM_PARAM SWM_Deinit_LibraryImp( void ) {
  if ( SWAllocator && SWLiberator ) {
		SWAllocator = 0;
		SWLiberator = 0;
		return 1;
  }
  else {
    _RPT0( _CRT_ASSERT, "SWM : Library already deinited" );
    return 0;
  }
}

// ---
SWM_PROC AVP_bool SWM_PARAM SWM_Deinit_Library () {
	if ( SWLocker && SWUnlocker ) {
		AVP_bool res;
		typedef void  (* t_unlocker)(void*);
		t_unlocker _unlocker;

		SWLocker( SWLContext );

		res = SWM_Deinit_LibraryImp();

		SWLocker = 0;
		_unlocker = SWUnlocker;
		SWUnlocker = 0;
		SWLContext = 0; 

		_unlocker( SWLContext );

		res;
	}

	return SWM_Deinit_LibraryImp();
}

// ---
SWM_PROC AVP_bool SWM_PARAM SWMAdvise( SWManager *pManager, SWControl *piSWControl ) {
  SWDescriptor *pSWTable;

  if ( !pManager )
    pManager = (gpSWManager ? gpSWManager : (gpSWManager = SWMCreateInstance(0)));

	if ( pManager ) {
		if ( !pManager->m_pSWTable ) {
			pManager->m_pSWTable = (SWDescriptor *)SWAllocator( sizeof(SWDescriptor) );

			if ( pManager->m_pSWTable ) {
				pManager->m_pSWTable->m_nSWId = (unsigned long)-1;
				pManager->m_pSWTable->m_piSWControl = 0;
				pManager->m_nSWTableCount = 1;
			}
			else
				return 0;
		}

		pSWTable = (SWDescriptor *)SWAllocator( sizeof(SWDescriptor) * (pManager->m_nSWTableCount + 1) );
		if ( pSWTable ) {
			memcpy( pSWTable + 1, pManager->m_pSWTable, sizeof(SWDescriptor) * pManager->m_nSWTableCount );
			SWLiberator( pManager->m_pSWTable );

			pManager->m_pSWTable = pSWTable;
			pManager->m_nSWTableCount++;

			pManager->m_pSWTable->m_nSWId = piSWControl->lpVtbl->SWGetId( piSWControl );
			pManager->m_pSWTable->m_piSWControl = piSWControl;

			return 1;
		}
	}
  return 0;
}


// ---
static unsigned int SWM_PARAM AddRef( SWManager *This ) {
  return ++This->m_cRef;
}

// ---
static unsigned int SWM_PARAM Release( SWManager *This ) {
  if ( --This->m_cRef == 0 ) {

    if ( This->m_pSWTable != 0 ) {
			int i;
			SWDescriptor *pSWTable = This->m_pSWTable;
			for ( i=0; pSWTable->m_nSWId != (unsigned long)-1; i++,pSWTable++ ) 
			 pSWTable->m_piSWControl->lpVtbl->Release( pSWTable->m_piSWControl );
    }

    SWLiberator( This->m_pSWTable );
    This->m_pSWTable = 0;

    SWLiberator( This->lpVtbl );
    SWLiberator( This );

    return 0;
  }
  return This->m_cRef;
}

// ---
static SWContext* SWM_PARAM SWMGetContext( SWManager *This, unsigned long nId ) {
  if ( This->m_pSWTable != 0 ) {
		SWDescriptor *pSWTable = This->m_pSWTable;
		for ( ; pSWTable->m_nSWId != (unsigned long)-1; pSWTable++ ) {
			if ( pSWTable->m_nSWId == nId && 
				   pSWTable->m_piSWControl->m_pSWContext->m_nLevel == This->m_nCurrLevel )
				return pSWTable->m_piSWControl->m_pSWContext;
		}
  }

  return 0;
}

// ---
static void SWM_PARAM SWMEnterBufferProc( SWManager *This ) {
  This->m_nCurrLevel++;
}


// ---
static void SWM_PARAM SWMLeaveBufferProc( SWManager *This ) {
  This->m_nCurrLevel--;
}


// --- 
static SWControl * SWM_PARAM FindSiblingControl( SWManager *pManager, SWControl *piSWControl ) {
  SWDescriptor *pSWTable = pManager->m_pSWTable;
  for ( ; pSWTable->m_nSWId != (unsigned long)-1; pSWTable++ ) {
   if ( pSWTable->m_piSWControl != piSWControl &&
        pSWTable->m_piSWControl->m_pSWContext->m_nLevel == 
        piSWControl->m_pSWContext->m_nLevel ) {
     pSWTable->m_piSWControl->m_pSWContext->m_nLevel++;
     FindSiblingControl( pManager, pSWTable->m_piSWControl );
     return pSWTable->m_piSWControl;
   }
  }
  return 0;
}


// ---
static AVP_bool SWM_PARAM SWMAttachSW( SWManager *This, 
																			 unsigned long nNewId, unsigned long nOldId, 
                                       void *pBuffer, AVP_dword nSize, void* pUserData ) {
  AVP_bool bOk = 0;

  SWContext *pSWOldContext = This->lpVtbl->SWMGetContext( This, nOldId );
  if ( pSWOldContext ) {

		if ( This->m_pSWTable != 0 ) {
			SWControl *piSiblingControl = 0;
			SWDescriptor *pSWTable = This->m_pSWTable;
			for ( ; pSWTable->m_nSWId != (unsigned long)-1; pSWTable++ ) {
				if ( pSWTable->m_nSWId == nNewId ) {
					if ( !pSWTable->m_piSWControl->m_pSWContext->m_nLevel ) {
						pSWTable->m_piSWControl->m_pSWContext->m_nLevel = pSWOldContext->m_nLevel + 1;

						piSiblingControl = FindSiblingControl( This, pSWTable->m_piSWControl );

						if ( !piSiblingControl ) 
							pSWTable->m_piSWControl->m_pSWContext->m_pSiblingProc = pSWOldContext->m_pSiblingProc;
						else 
							pSWTable->m_piSWControl->m_pSWContext->m_pSiblingProc = piSiblingControl->m_pSWContext->m_pReadProc;

						pSWOldContext->m_pSiblingProc = pSWTable->m_piSWControl->m_pSWContext->m_pReadProc;

						bOk = pSWTable->m_piSWControl->lpVtbl->SWAcceptBuffer( pSWTable->m_piSWControl, 
																																	 This,  
																																	 pBuffer, 
																																	 nSize, 
																																	 pUserData );
						break;
					}
					else {
						SWControl *pSWControl = 0;

						if ( pSWTable->m_piSWControl->lpVtbl->SWCreateInstance(pSWTable->m_piSWControl, This, &pSWControl) &&
								 pSWControl ) {
							pSWTable = This->m_pSWTable;

							for ( ; pSWTable->m_nSWId != (unsigned long)-1; pSWTable++ ) {
								if ( pSWTable->m_nSWId == nNewId ) {
									if ( !pSWTable->m_piSWControl->m_pSWContext->m_nLevel ) {
										pSWTable->m_piSWControl->m_pSWContext->m_nLevel = pSWOldContext->m_nLevel + 1;

										piSiblingControl = FindSiblingControl( This, pSWTable->m_piSWControl );

										if ( !piSiblingControl ) 
											pSWTable->m_piSWControl->m_pSWContext->m_pSiblingProc = pSWOldContext->m_pSiblingProc;
										else 
											pSWTable->m_piSWControl->m_pSWContext->m_pSiblingProc = piSiblingControl->m_pSWContext->m_pReadProc;

										pSWOldContext->m_pSiblingProc = pSWTable->m_piSWControl->m_pSWContext->m_pReadProc;

										bOk = pSWTable->m_piSWControl->lpVtbl->SWAcceptBuffer( pSWTable->m_piSWControl, 
																																					 This,
																																					 pBuffer, 
																																				   nSize,
																																					 pUserData);
										break;
									}
								}
							}
						}
					}
					break;
				}
			}
		}
  }
  return bOk;
}


// ---
static AVP_bool SWM_PARAM SWMIsSWRegistered( SWManager *pManager, unsigned long nId ) {
	if ( pManager->m_pSWTable != 0 ) {
		SWDescriptor *pSWTable = pManager->m_pSWTable;
		int i;
		for ( i=0; pSWTable->m_nSWId != (unsigned long)-1; i++,pSWTable++ ) {
			if ( pSWTable->m_nSWId == nId )
				return 1;
		}
	}
	return 0;
}


// ---
SWManager *SWMCreateInstance( SWManager *pOldManager ) {
  SWManager *pManager = (SWManager *)SWAllocator( sizeof(SWManager) );

	if ( pManager ) {
		pManager->m_cRef = 1;
		pManager->m_nCurrLevel = 0;
		pManager->m_pSWTable = 0;
		pManager->m_nSWTableCount = 0;
		pManager->lpVtbl = (SWManagerVtbl *)SWAllocator( sizeof(SWManagerVtbl) );
		if ( pManager->lpVtbl ) {
			pManager->lpVtbl->AddRef = AddRef;
			pManager->lpVtbl->Release = Release;
			pManager->lpVtbl->SWMGetContext = SWMGetContext;
			pManager->lpVtbl->SWMAttachSW = SWMAttachSW;
			pManager->lpVtbl->SWMEnterBufferProc = SWMEnterBufferProc; 
			pManager->lpVtbl->SWMLeaveBufferProc = SWMLeaveBufferProc; 
			pManager->lpVtbl->SWMIsSWRegistered = SWMIsSWRegistered;

			if ( pOldManager ) {
				if ( pOldManager->m_pSWTable && pOldManager->m_nSWTableCount >= 2 ) {
					SWDescriptor *pSWTable = pOldManager->m_pSWTable;
					int i;
					for ( i=pOldManager->m_nSWTableCount-2; i>=0; i-- ) {
						SWControl *piSWControl = 0;
						if ( !(pSWTable[i].m_piSWControl->lpVtbl->SWDuplicate(pSWTable[i].m_piSWControl, &piSWControl) &&
									 piSWControl &&
									 SWMAdvise(pManager, piSWControl)) ) {
							if ( piSWControl && piSWControl->lpVtbl )
								piSWControl->lpVtbl->Release( piSWControl );

							pManager->lpVtbl->Release( pManager );
							pManager = 0;
							break;
						}
					}
				}
				else {
					pManager->lpVtbl->Release( pManager );
					pManager = 0;
				}
			}
		}
		else {
			SWLiberator( pManager );
			pManager = 0;
		}
	}
  return pManager;
}


// ---
SWM_PROC void *  SWM_PARAM SWM_Create_Manager( void ) {
  return SWMCreateInstance( 0 );
}

// ---
SWM_PROC void SWM_PARAM SWM_Shut_Down_Library( void *pManager ) {
  if ( pManager != 0 ) 
    ((SWManager *)pManager)->lpVtbl->Release( (SWManager *)pManager );
  else 
		if ( gpSWManager != 0 ) {
			gpSWManager->lpVtbl->Release( gpSWManager );
			gpSWManager = 0;
		}
}


