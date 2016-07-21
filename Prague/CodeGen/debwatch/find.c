#include "custview.h"

// ---
tName* find( tName* arr, tUINT count, DWORD id ) {
	tUINT i;
	for( i=0; i<count; arr++,i++ ) {
		if ( arr->id == id ) 
			return arr;
	}
	return 0;
}



// ---
char* find_name( tName* arr, tUINT count, DWORD id, char* def ) {
	tUINT i;
	for( i=0; i<count; arr++,i++ ) {
		if ( arr->id == id ) 
			return arr->name;
	}
	return def;
}



