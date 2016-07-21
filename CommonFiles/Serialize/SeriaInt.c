// KLSerializeInit.c - implementation of KLDataTreeSerialization
// Main part of a code
//
#include <string.h>
#include "AVPPort.h"

#include <Serialize/KLDTSer.h>
#include <_AVPIO.h>

void* (* DTAllocator)(AVP_size_t) = NULL;
void  (* DTLiberator)(void*) = NULL;

void  (* DTLocker)(void*);
void  (* DTUnlocker)(void*);
void* DTContext; 


// ---
static KLDT_PROC AVP_bool KLDT_PARAM KLDT_Init_LibraryImp( void* (*alloc)(AVP_size_t), void (*free)(void*), AVP_bool bInitDATALib ) {
  if ( !alloc || !free || 
		   (DTAllocator && alloc != DTAllocator) ||
			 (DTLiberator && free  != DTLiberator) ) {
    _RPT0( _CRT_ASSERT, "KLDT : Library already inited" );
    return FALSE;
  }
  else {			  
		DTAllocator = alloc;
		DTLiberator = free; 

		return bInitDATALib ? DATA_Init_Library( alloc, free ) : 1;
  }
}

// ---
KLDT_PROC AVP_bool KLDT_PARAM KLDT_Init_Library( void* (*alloc)(AVP_size_t), void (*free)(void*) ) {
	return KLDT_Init_LibraryImp( alloc, free, 1 );
}

// ---
KLDT_PROC AVP_bool KLDT_PARAM KLDT_Init_LibraryEx( void* (*alloc)(AVP_size_t), void (*free)(void*), void (*lock)(void *), void (*unlock)(void *), void* cont) {
	if ( lock && unlock ) {
		AVP_bool res;
		
		lock( cont );
		
		res = KLDT_Init_LibraryImp( alloc, free, 0 );
		
		DTLocker = lock;
		DTUnlocker = unlock;
		DTContext = cont; 

		unlock( cont );

		return DATA_Init_LibraryEx( alloc, free, lock, unlock, cont );
	}
	return KLDT_Init_LibraryImp( alloc, free, 1 );
}


// ---
static KLDT_PROC AVP_bool KLDT_PARAM KLDT_Deinit_LibraryImp( AVP_bool bDeinitDATALib ) {
  if ( DTAllocator && DTLiberator ) {
		DTAllocator = 0;
		DTLiberator = 0;
		return bDeinitDATALib ? DATA_Deinit_Library() : 1;
  }
  else {
    _RPT0( _CRT_ASSERT, _T("KLDT : Library already deinited") );
    return 0;
  }
}


// ---
KLDT_PROC AVP_bool KLDT_PARAM KLDT_Deinit_Library ( AVP_bool bDeinitDATALib ) {
	if ( DTLocker && DTUnlocker ) {
		AVP_bool res;
		typedef void  (* t_unlocker)(void*);
		t_unlocker _unlocker;

		DTLocker( DTContext );

		res = KLDT_Deinit_LibraryImp( 0 );

		DTLocker = 0;
		_unlocker = DTUnlocker;
		DTUnlocker = 0;
		DTContext = 0; 

		_unlocker( DTContext );

		return bDeinitDATALib ? DATA_Deinit_Library() : res;
	}

	return KLDT_Deinit_LibraryImp( bDeinitDATALib );
}

