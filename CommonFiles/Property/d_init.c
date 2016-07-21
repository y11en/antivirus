#include "prop_in.h"

void* (* allocator)(AVP_size_t);
void  (* liberator)(void*);

void  (* locker)(void*);
void  (* unlocker)(void*);
void* context; 

// ---
DATA_PROC AVP_bool DATA_PARAM DATA_Init_Library( void* (*alloc)(AVP_size_t), void (*free)(void*) ) {
  if ( !alloc || !free || ( allocator && allocator != alloc ) || ( liberator && liberator != free ) ) {
    _RPT0( _CRT_ASSERT, "Data library already inited" );
    return 0;
  }
  else {
		allocator = alloc;
		liberator = free;
    return 1;
  }
}

// ---
DATA_PROC AVP_bool DATA_PARAM DATA_Init_LibraryEx ( void* (*alloc)(AVP_size_t), void (*free)(void*), void (*lock)(void *), void (*unlock)(void *), void* cont) {
	if ( lock && unlock ) {
		AVP_bool res;
		
		lock( cont );
		
		res = DATA_Init_Library( alloc, free );
		
		locker = lock;
		unlocker = unlock;
		context = cont; 

		unlock( cont );
		return res;
	}
	return DATA_Init_Library( alloc, free );
}

// ---
static DATA_PROC AVP_bool DATA_PARAM DATA_Deinit_LibraryImp ( void ) {
  if ( allocator && liberator ) {
		allocator = 0;
		liberator = 0;
    return 1;
  }
  else {
    _RPT0( _CRT_ASSERT, "Data library already deinited" );
    return 0;
  }
}

// ---
DATA_PROC AVP_bool DATA_PARAM DATA_Deinit_Library ( void ) {
	if ( locker && unlocker ) {
		AVP_bool res;
		typedef void  (* t_unlocker)(void*);
		t_unlocker _unlocker;

		locker( context );

		res = DATA_Deinit_LibraryImp();

		locker = 0;
		_unlocker = unlocker;
		unlocker = 0;
		context = 0; 

		_unlocker( context );
		return res;
	}
	return DATA_Deinit_LibraryImp();
}

