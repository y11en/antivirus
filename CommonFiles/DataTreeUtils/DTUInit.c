////////////////////////////////////////////////////////////////////
//
//  DTUInit.c
//  Library initialization module
//  AVP generic purpose stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <string.h>

#include <datatreeutils/dtutils.h>
#include <AVPPort.h>

void* (* DTUAllocator)(AVP_size_t) = NULL;
void  (* DTULiberator)(void*) = NULL;

void  (* DTULocker)(void*);
void  (* DTUUnlocker)(void*);
void* DTUContext; 


// ---
static DTUT_PROC AVP_bool DTUT_PARAM DTUT_Init_LibraryImp( void* (*alloc)(AVP_size_t), void (*free)(void*), AVP_bool bInitDATALib ) {
  if ( !alloc || !free || 
		   (DTUAllocator && alloc != DTUAllocator) ||
			 (DTULiberator && free  != DTULiberator) ) {
    _RPT0( _CRT_ASSERT, "DTUT : Library already inited" );
    return 0;
  }
  else {			  
		DTUAllocator = alloc;
		DTULiberator = free;

		return bInitDATALib ? DATA_Init_Library( alloc, free ) : 1;
  }
}

// ---
DTUT_PROC AVP_bool DTUT_PARAM DTUT_Init_Library( void* (*alloc)(AVP_size_t), void (*free)(void*) ) {
	return DTUT_Init_LibraryImp( alloc, free, 1 );
}

// ---
DTUT_PROC AVP_bool DTUT_PARAM DTUT_Init_LibraryEx( void* (*alloc)(AVP_size_t), void (*free)(void*), void (*lock)(void *), void (*unlock)(void *), void* cont) {
	if ( lock && unlock ) {
		AVP_bool res;
		
		lock( cont );
		
		res = DTUT_Init_LibraryImp( alloc, free, 0 );
		
		DTULocker = lock;
		DTUUnlocker = unlock;
		DTUContext = cont; 

		unlock( cont );

		return DATA_Init_LibraryEx( alloc, free, lock, unlock, cont );
	}
	return DTUT_Init_LibraryImp( alloc, free, 1 );
}



// ---
static DTUT_PROC AVP_bool DTUT_PARAM DTUT_Deinit_LibraryImp( AVP_bool bDeinitDATALib ) {
  if ( DTUAllocator && DTULiberator ) {
		DTUAllocator = 0;
		DTULiberator = 0;
		return bDeinitDATALib ? DATA_Deinit_Library() : 1;
  }
  else {
    _RPT0( _CRT_ASSERT, "DTUT : Library already deinited" );
    return 0;
  }
}

// ---
DTUT_PROC AVP_bool DTUT_PARAM DTUT_Deinit_Library ( AVP_bool bDeinitDATALib ) {
	if ( DTULocker && DTUUnlocker ) {
		AVP_bool res;
		typedef void  (* t_unlocker)(void*);
		t_unlocker _unlocker;

		DTULocker( DTUContext );

		res = DTUT_Deinit_LibraryImp( 0 );

		DTULocker = 0;
		_unlocker = DTUUnlocker;
		DTUUnlocker = 0;
		DTUContext = 0; 

		_unlocker( DTUContext );

		return bDeinitDATALib ? DATA_Deinit_Library() : res;
	}

	return DTUT_Deinit_LibraryImp( bDeinitDATALib );
}

