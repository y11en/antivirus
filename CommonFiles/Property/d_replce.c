#include "prop_in.h"

// ---
DATA_PROC HDATA DATA_PARAM DATA_Replace( HDATA dad, AVP_dword* addr, AVP_dword pid, AVP_size_t val, AVP_dword size ) {
	AVP_Data* data;
	AVP_dword new_size;
	AVP_Data* new_data;
	AVP_Data* next;
	AVP_Linked_Property* lp;

	_ASSERTE( allocator && liberator );

	FIND_DATA_EX( data, dad, addr );

	new_size = sizeof(AVP_Data) - sizeof(AVP_Property) + PROP_Predict_Size( pid );
	new_data = (AVP_Data*)allocator( new_size );
	_ASSERT( new_data );
	memset( new_data, 0, new_size );

	PROP_Init( &new_data->value.data, pid, val, size );

	new_data->child       = data->child;      data->child = 0;
	new_data->dad         = data->dad;        data->dad   = 0;
	new_data->next        = data->next;       data->next  = 0;
	new_data->value.next  = data->value.next; data->value.next = 0;

	next = new_data->child;
	while ( next ) {
		next->dad = new_data;
		next = next->next;
	}

	if ( new_data->dad ) {
		if ( new_data->dad->child == data )
			new_data->dad->child = new_data;
		else {
			next = new_data->dad->child;
			while( next && next->next != data )
				next = next->next;
			if ( next->next == data )
				next->next = new_data;
			else {
				_RPT0( _CRT_ASSERT, "Bad data tree" );
				return 0;
			}
		}
	}

	lp = new_data->value.next;
	while( lp->next )
		lp = lp->next;
	if ( PROP_ID(&lp->data) == AVP_PID_END )
		((AVP_DWORD_Property*)&lp->data)->val = (AVP_dword)new_data;
	else {
		_RPT0( _CRT_ASSERT, "Bad property list" );
		return 0;
	}

	DATA_Remove( DATA_HANDLE(data), 0 );
	return DATA_HANDLE( new_data );
}



// ---
DATA_PROC HPROP DATA_PARAM PROP_Replace( HPROP prop, AVP_dword pid, AVP_size_t val, AVP_dword size ) {
	HDATA data = PROP_Get_Dad( prop );
	return DATA_Replace_Prop( data, 0, PROP_ID(prop), pid, val, size );
}


