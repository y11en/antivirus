// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  30 October 2003,  12:34 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Kernel interfaces
// Sub project -- Not defined
// Purpose     -- Kernel interfaces
// Author      -- petrovitch
// File Name   -- serializer.cpp
// Additional info
//    Kernel interface implementations
// -------------------------------------------
// AVP Prague stamp end


#include <Prague/prague.h>
#include "k_root.h"
#include <Prague/pr_cpp.h>
#include <Prague/pr_stream.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_buffer.h>
#include "ser.h"
#include "Prague/iface/e_ktrace.h"

#define IMPEX_IMPORT_LIB
#include <Prague/plugin/p_string.h>
#include <Prague/iface/e_loader.h>

PR_NEW_DECLARE;

// ---
cRegistrar*       cRegistrar::g_head;
hCRITICAL_SECTION cRegistrar::g_cs;

// ---
#if defined(_PRAGUE_TRACE_)
	#define SIZEOMETER( type ) { tid_ ## type, sizeof(t ## type), 0, 0, #type}
#else
	#define SIZEOMETER( type ) { tid_ ## type, sizeof(t ## type), 0}
#endif

static tVOID _str_obj_clean( tVOID* obj )   { ((cStrObj*&)obj)->clean(); }
static tVOID _ptr_clean( tVOID* obj )       { ::g_root->heapFree( obj ); }
static tVOID _mem_clean( tVOID* obj )       { ((cMemChunk*&)obj)->clean(); }
static tVOID _ser_clean( tVOID* obj )       { ((cSerializable*&)obj)->release(); }

// ---
struct tTypeSize {
	tTYPE_ID   tid;
	tUINT      size;
	tCleanFunc clean;
	#if defined(_PRAGUE_TRACE_)
		tDWORD		 counter;
		tCHAR*     name;
	#endif
} g_typeSize[] = {

	// -----------------------------------
	// last change 18.05.07
	// correspond to pr_types.h
	// -----------------------------------

	SIZEOMETER( DWORD ),
	SIZEOMETER( BOOL  ),

	{ tid_OBJECT,       sizeof(hOBJECT),        0 },
	{ tid_VECTOR,       sizeof(tMemChunk),      0 },
	{ tid_BUFFER,       sizeof(tMemChunk),      _mem_clean },
	{ tid_STRING,       sizeof(tSTRING),        _ptr_clean },
	{ tid_WSTRING,      sizeof(tWSTRING),       _ptr_clean },
	{ tid_STRING_OBJ,   sizeof(cStringObj),     _str_obj_clean },

	{ tid_VOID, 0        },   SIZEOMETER( BYTE     ),   SIZEOMETER( WORD     ),   
	SIZEOMETER( QWORD    ),   SIZEOMETER( CHAR     ),   SIZEOMETER( WCHAR    ),
	SIZEOMETER( ERROR    ),   SIZEOMETER( PTR      ),   SIZEOMETER( INT      ),   SIZEOMETER( UINT  ),
	SIZEOMETER( SBYTE    ),  
	
	SIZEOMETER( SHORT    ),   SIZEOMETER( LONG     ),   SIZEOMETER( LONGLONG ),
	SIZEOMETER( IID      ),   SIZEOMETER( PID      ),   SIZEOMETER( ORIG_ID  ),
	SIZEOMETER( OS_ID    ),   SIZEOMETER( VID      ),   SIZEOMETER( PROPID   ),
	SIZEOMETER( VERSION  ),   SIZEOMETER( CODEPAGE ),   SIZEOMETER( LCID     ),
	SIZEOMETER( DATA     ),   SIZEOMETER( DATETIME ),   SIZEOMETER( FUNC_PTR ),
	SIZEOMETER( EXPORT   ),   SIZEOMETER( IMPORT   ),   SIZEOMETER( TRACE_LEVEL), SIZEOMETER( TYPE_ID ),
	SIZEOMETER( DOUBLE   ),

	// --- new types (18.05.07)
	SIZEOMETER( IMPL_ID ),  SIZEOMETER( MSG_ID ),     SIZEOMETER( MSGCLS_ID ),

	// --- not listed earlier
	SIZEOMETER( SIZE_T ),
	{ tid_SERIALIZABLE, sizeof(cSerializable*), _ser_clean },
	{ tid_IFACEPTR,     sizeof(tPTR), 0 },
	{ tid_BINARY,       sizeof(tPTR), 0 },

	// -- known but not listed
	// tid_STR_DSC = 0x3bL,
};


// ---
EXTERN_C tUINT pr_call pr_sizeometer( tDWORD id, tCleanFunc* func ) {

	if ( func )
		*func = 0;
	
	if ( id < tid_LAST_TYPE ) {
		for( tUINT i=0; i<countof(g_typeSize); ++i ) {
			if ( ((tTYPE_ID)id) == g_typeSize[i].tid ) {
				#if defined(_PRAGUE_TRACE_)
					g_typeSize[i].counter++;
					if ( g_typeSize[i].counter && (0 == (g_typeSize[i].counter % 5000)) ) {
						tTypeSize* t = &g_typeSize[i];
//						PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "krn\tpr_sizeometer: counter:%d, tid - %d, %s", t->counter, t->tid, t->name ));
					}
				#endif
				if ( func )
					*func = g_typeSize[i].clean;
				return g_typeSize[i].size;
			}
		}
		PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "krn\tpr_sizeometer: tid not found"));
		return -1;
	}

	const cSerDescriptor* dsc = cRegistrar::find( id );
	if ( !dsc )
		return -1;
	if ( func )
		*func = (tCleanFunc)dsc->m_destructor;
	return dsc->m_size;
}




// ---
tERROR _check_integrated_ptr( tPTR& addr, const cSerDescriptorField* field, bool create ) {
	if ( !addr )
		return errNOT_ENOUGH_MEMORY;
	if ( !(field->m_flags & SDT_FLAG_POINTER) )
		return errOK;
	if ( !create ) {
		addr = *(tPTR*)addr;
		return errOK;
	}

	tERROR err;
	tDWORD size;
	if ( field->m_flags & SDT_FLAG_VECTOR ) 
		size = sizeof(tMemChunk);
	else
		size = sizeof(tPTR); //return errBAD_SIGNATURE;
	tPTR new_ptr;
	if ( PR_FAIL(err=::g_root->heapAlloc(&new_ptr,size)) )
		return err;
	*(tPTR*)addr = new_ptr;
	return errOK;
}



// ---
const cSerDescriptorField* _find_key_field( const cSerDescriptorField* field ) {
	if ( !field )
		return 0;

	for( ; field->m_flags != SDT_FLAG_ENDPOINT; ++field  ) {
		if ( field->m_flags & SDT_FLAG_KEY_NAME )
			return field;
		if ( field->m_flags & SDT_FLAG_BASE ) {
			const cSerDescriptor* base = cRegistrar::find( field->m_id );
			if ( base ) {
				const cSerDescriptorField* base_field = _find_key_field( base->m_fields );
				if ( base_field )
					return base_field;
			}
		}
	}

	return 0;
}



// ---
tERROR _alloc_vect_el( tMemChunk& v, tPTR& ptr, tUINT el_size, const cSerDescriptorField* field, tSerializeCallback callback, tPTR params ) {
	tERROR err;
	if ( callback ) {
		err = callback( params, sa_alloc_simple_vector_el, &v, field, 0 );
		if ( err != warnDEFAULT ) {
			ptr = VPTR(v) + v.m_used;
			return err;
		}
	}

	if ( v.m_allocated <= v.m_used ) {
		el_size = v.m_used + (VECTOR_REALLOC_CLUSTER * el_size);
		if ( PR_FAIL(err=::g_root->heapRealloc((tPTR*)&v.m_ptr,v.m_ptr,el_size)) )
			return err;
		v.m_allocated = el_size;
	}
	ptr = VPTR(v) + v.m_used;
	return errOK;
}

// ---
tERROR _alloc_vect_mem( tMemChunk& v, tUINT size, tUINT el_size )
{
	tERROR err = errOK;
	
	v.m_used = 0;
	v.m_allocated = size * el_size;
	if( v.m_allocated )
		err = ::g_root->heapAlloc((tPTR *)&v.m_ptr, v.m_allocated);
	
	return err;
}

// ---
tERROR _alloc_vect_el( tMemChunk& v, tPTR& ptr, const cSerDescriptor** dsc, tBOOL pointer, const cSerDescriptorField* field, tSerializeCallback callback, tPTR params ) {
	tERROR err;

	if ( pointer || !(*dsc)->m_constructor )
		return _alloc_vect_el( v, (tPTR&)ptr, pointer ? sizeof(tPTR) : (*dsc)->m_size, field, callback, params );

	if ( callback )	{
		err = callback( params, sa_alloc_ser_vector_el, &v, field, dsc );
		if ( err != warnDEFAULT ) {
			ptr = VPTR(v) + v.m_used;
			return err;
		}
	}

	if ( v.m_allocated <= v.m_used ) {
		tBYTE* p;
		tUINT  i;
		tUINT  s = (*dsc)->m_size;
		tUINT  size = v.m_used + (VECTOR_REALLOC_CLUSTER * s);
		if ( PR_FAIL(err=::g_root->heapAlloc((tPTR*)&p,size)) )
			return 0;
		for( i=0; i<v.m_used; i+=s ) {
			(*dsc)->m_constructor( (cSerializable*)(p + i), (cSerializable*)(VPTR(v) + i) );
			(*dsc)->m_destructor( (cSerializable*)(VPTR(v) + i) );
		}
//		for( ; i<size; i+=s )
//			(*dsc)->m_constructor( (cSerializable*)(p + i), 0 );
		if ( VPTR(v) ) 
			::g_root->heapFree( VPTR(v) );
		v.m_ptr = p;
		v.m_allocated = size;
	}
	ptr = (cSerializable*)(VPTR(v) + v.m_used);
	(*dsc)->m_constructor( (cSerializable*)ptr, 0 );
	return errOK;
}


// ---
tVOID _clean_ptr( tVOID*& obj, const cSerDescriptorField* f ) {
	if ( !obj )
		return;

	if ( f->m_id < tid_LAST_TYPE ) {
		tCleanFunc clean;
		pr_sizeometer( f->m_id, &clean );
		if ( clean )
			clean( obj );
	}
	else {
		const cSerDescriptor* dsc = cRegistrar::find( ((cSerializable*)obj)->getIID() );
		if ( dsc && dsc->m_destructor )
			dsc->m_destructor( (cSerializable*)obj );
	}
	::g_root->heapFree( obj );
	obj = 0;
}



// ---
tVOID _clean_vector( tMemChunk& v, const cSerDescriptorField* f ) {
	if ( !VPTR(v) )
		return;

	tUINT offs;
	tBOOL ptr = !!(f->m_flags & SDT_FLAG_POINTER);
	cSerializable* el;
	const cSerDescriptor* dsc;

	if ( f->m_id < tid_LAST_TYPE ) {
		tCleanFunc clean;
		tUINT el_size = pr_sizeometer( f->m_id, &clean );
		if ( clean ) {
			tBYTE* el;
			for( offs=0,el=VPTR(v); offs<v.m_used; offs+=el_size,el=VPTR(v)+offs ) {
				if ( ptr )
					el = *(tBYTE**)el;
				clean( (tVOID*&)el );
			}
		}
	}
	else if ( ptr ) {
		for( offs=0; offs<v.m_used; offs+=sizeof(tPTR) ) {
			el=*(cSerializable**)(VPTR(v)+offs);
			dsc = cRegistrar::find( el->getIID() );
			if ( dsc && dsc->m_destructor )
				dsc->m_destructor( el );
		}
	}
	else if ( 0 == (dsc=cRegistrar::find(f->m_id)) )
		;
	else if ( !dsc->m_destructor )
		;
	else {
		for( offs=0,el=(cSerializable*)VPTR(v); offs<v.m_used; offs+=dsc->m_size,el=(cSerializable*)(VPTR(v)+offs) )
			dsc->m_destructor( el );
	}

	::g_root->heapFree( VPTR(v) );
	v.m_allocated = v.m_used = 0;
	v.m_ptr = 0;
}



// ---
bool _empty_vector( tMemChunk& v, const cSerDescriptor* dsc, tUINT el_size )
{
	if( v.m_allocated && !v.m_used)
	{
//		this code block used to uses this '_alloc_vect_el' functions
//		if ( dsc && dsc->m_destructor )
//		{
//			for( tUINT i=0; i<v.m_allocated; i+=el_size )
//				dsc->m_destructor( (cSerializable*)(VPTR(v)+i) );
//		}
		::g_root->heapFree(VPTR(v));
		v.m_used = 0;
		v.m_allocated = 0;
		v.m_ptr = NULL;
		return true;
	}
	return false;
}



// ---
extern "C" tERROR pr_call _SerializerInit() {
	PR_TRACE_A0( 0, "Enter System::SerializerInit" );
	
	if ( cRegistrar::g_cs )
		return errOK;
	
	tPO* po;
	enter_lf();
	rlock(po);
	cERROR err = _SyncCreateAvailable( (hOBJECT)::g_root, &cRegistrar::g_cs );
	runlock(po);
	leave_lf();
	
	PR_TRACE_A1( 0, "Leave System::Serializer Init ret %terr", (tERROR)err );
	return err;
}



// ---
extern "C" tERROR pr_call _SerializerDeinit() {
	PR_TRACE_A0( 0, "Enter System::SerializerDeinit" );

	while( cRegistrar::g_head ) {
		cRegistrar* tmp = cRegistrar::g_head->m_next;
		PrFree( cRegistrar::g_head );
		cRegistrar::g_head = tmp;
	}

	if ( cRegistrar::g_cs )
		cRegistrar::g_cs = 0;

	PR_TRACE_A0( 0, "Leave System::SerializerDeinit Init ret errOK" );
	return errOK;
}



// ---
const cSerDescriptor* cRegistrar::find( tUINT unique ) {
	cRegistrar* reg = 0;
	cLocker locker(g_cs);
	
	reg = g_head;
	while ( reg ) {
		if ( reg->m_id == unique )
			break;
		reg = reg->m_next;
	}
	
	return reg ? reg->m_dsc : 0;
}

const cSerDescriptor* cRegistrar::find( const tCHAR* name ) {
	cRegistrar* reg = 0;
	cLocker locker(g_cs);

  reg = g_head;
  while ( reg ) {
		if ( reg->m_dsc->m_name && !strcmp(reg->m_dsc->m_name, name) )
			break;
		reg = reg->m_next;
  }

  return reg ? reg->m_dsc : 0;
}


// ---
tERROR cRegistrar::add( const cSerDescriptor* dsc ) {
	tERROR err;
  tUINT id = dsc->m_unique;
  cLocker locker(g_cs);
  cRegistrar* reg = g_head;
  while ( reg ) {
		if ( reg->m_id == id ) {
			PR_TRACE(( 
				::g_root, 
				prtDANGER, 
				"krn\tserializable id(%d) already registered! First was \"%s\", second \"%s\"", 
				id, 
				reg->m_dsc->m_name ? reg->m_dsc->m_name : "empty name", 
				dsc->m_name ? dsc->m_name : "empty name" 
			));
			PR_RPT(("Serializable id(0x%08x) already registered", id));
			return errOBJECT_ALREADY_EXISTS;
		}
		reg = reg->m_next;
  }

	if ( PR_SUCC(err=PrAlloc((tPTR*)&reg,sizeof(cRegistrar))) ) {
		reg->m_dsc = dsc;
		reg->m_id = dsc->m_unique;
		cLocker locker(g_cs,true);
		reg->m_next = g_head;
		g_head = reg;
	}
	return err;
}



// ---
tERROR cRegistrar::free( const cSerDescriptor* dsc ) {
  tUINT id = dsc->m_unique;
  cLocker locker(g_cs);
  cRegistrar* reg = g_head;
  cRegistrar* prev = 0;
  while ( reg ) {
		if ( reg->m_id == id ) {
			cLocker locker(g_cs,true);
			if ( prev )
				prev->m_next = reg->m_next;
			else
				g_head = reg->m_next;
			PrFree( reg );
			return errOK;
		}
		prev = reg;
		reg = reg->m_next;
  }
	return errOBJECT_NOT_FOUND;
}


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegisterSerializableDescriptor )
// Parameters are:
extern "C" tERROR pr_call Root_RegisterSerializableDescriptor( hi_Root _this, const cSerDescriptor* p_descriptor ) {
	tERROR error;
	PR_TRACE_A0( _this, "Enter Root::RegisterSerializableDescriptor method" );

	_SerializerInit();
	if ( !p_descriptor )
		error = errPARAMETER_INVALID;
	else
		error = cRegistrar::add( p_descriptor );
	
	PR_TRACE_A1( _this, "Leave Root::RegisterSerializableDescriptor method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, UnregisterSerializableDescriptor )
// Parameters are:
extern "C" tERROR pr_call Root_UnregisterSerializableDescriptor( hi_Root _this, const cSerDescriptor* p_descriptor ) {
	tERROR error;
	PR_TRACE_A0( _this, "Enter Root::UnregisterSerializableDescriptor method" );
	
	if ( !p_descriptor )
		error = errPARAMETER_INVALID;
	else
		error = cRegistrar::free( p_descriptor );
	
	PR_TRACE_A1( _this, "Leave Root::UnregisterSerializableDescriptor method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, FindSerializableDescriptor )
// Parameters are:
extern "C" tERROR pr_call Root_FindSerializableDescriptor( hi_Root _this, cSerDescriptor** p_descriptor, tDWORD p_uniqueId ) {
	tERROR error;
	PR_TRACE_A0( _this, "Enter Root::FindSerializableDescriptor method" );
	
	const cSerDescriptor* dsc;
	if ( !p_descriptor )
		error = errPARAMETER_INVALID;
	else if ( 0 == (dsc=cRegistrar::find(p_uniqueId)) )
		error = errNOT_FOUND;
	else {
		if( p_descriptor )
			*p_descriptor = (cSerDescriptor*)dsc;
		error = errOK;
	}

	PR_TRACE_A1( _this, "Leave Root::FindSerializableDescriptor method, ret %terr", error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, CreateSerializable )
// Parameters are:
extern "C" tERROR pr_call Root_CreateSerializable( hi_Root _this, tDWORD p_unique_id, cSerializable** p_object ) {
	cERROR error;
	PR_TRACE_A0( _this, "Enter Serializer::CreateSerializable method" );

	const cSerDescriptor* dsc;
	if ( !p_object )
		error = errPARAMETER_INVALID;
	else if ( 0 == (dsc=cRegistrar::find(p_unique_id)) )
		error = errOBJECT_NOT_FOUND;
	else
		error = _alloc_str( &dsc, p_object, 0, 0, 0 );

	PR_TRACE_A1( _this, "Leave Serializer::CreateSerializable method, ret %terr", (tERROR)error );
	return error;
}
// AVP Prague stamp end


struct cSerializableRef : public cSerializable { 
	inline operator tLONG* () { return &m_ref; }
};


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, AddRefSerializable )
// Parameters are:
extern "C" tERROR pr_call Root_AddRefSerializable( hi_Root _this, cSerializable* p_object ) {
	tERROR error;
	PR_TRACE_A0( _this, "Enter Root::AddRefSerializable method" );

	if ( p_object ) {
		error = errOK;
		PrInterlockedIncrement( *(cSerializableRef*)p_object );	// Place your code here
	}
	else
		error = errPARAMETER_INVALID;

	PR_TRACE_A1( _this, "Leave Root::AddRefSerializable method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, DestroySerializable )
// Parameters are:
extern "C" tERROR pr_call Root_DestroySerializable( hi_Root _this, cSerializable* p_object ) {
	cERROR error;
	PR_TRACE_A0( _this, "Enter Serializer::DestroySerializable method" );

	const cSerDescriptor* dsc;
	if ( !p_object )
		error = errPARAMETER_INVALID;
	else if ( 0 < PrInterlockedDecrement(*(cSerializableRef*)p_object) )
		error = errOK;
	else if ( 0 == (dsc=cRegistrar::find(p_object->getIID())) )
		error = errOBJECT_NOT_FOUND;
	else
		_free_str( dsc, p_object );

	PR_TRACE_A1( _this, "Leave Serializer::DestroySerializable method, ret %terr", (tERROR)error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, CopySerializable )
// Parameters are:
extern "C" tERROR pr_call Root_CopySerializable( hi_Root _this, cSerializable** p_dst, const cSerializable* p_src ) {
	cERROR error;
	PR_TRACE_A0( _this, "Enter Serializer::CopySerializable method" );

	if ( !p_src )
		error = errPARAMETER_INVALID;
	else if ( !p_dst )
		error = errPARAMETER_INVALID;
	else if ( !*p_dst && PR_FAIL(error=Root_CreateSerializable(_this,p_src->getIID(),p_dst)) )
		;
	else
		error = (*p_dst)->assign( *p_src, true );

	PR_TRACE_A1( _this, "Leave Serializer::CopySerializable method, ret %terr", (tERROR)error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, StreamSerialize )
// Parameters are:
extern "C" tERROR pr_call Root_StreamSerialize( hi_Root _this, const cSerializable* p_object, tDWORD p_ser_id, tBYTE* p_buffer, tDWORD p_size, tDWORD* p_output_size, tDWORD p_flags ) {
	cERROR err;
	PR_TRACE_FUNC_FRAME_( (hOBJECT)_this, "Enter Serializer::StreamSerialize method", &err._ );
	
	if ( p_output_size )
		*p_output_size = 0;
	
	if ( !p_object ) {
		PR_TRACE(( _this, prtERROR, "krn\tStream serialize: parameter invalid" ));
		return err = errPARAMETER_INVALID;
	}
	
	tDWORD ser_id = p_object->getIID();
	const cSerDescriptor* dsc = cRegistrar::find( ser_id );
	if ( !dsc ) {
		PR_TRACE(( _this, prtERROR, "krn\tStream serialize: description(%tser) not found", ser_id ));
		return err = errOBJECT_NOT_FOUND;
	}
	
	tDWORD stream_flags = 0;
	if ( p_flags & fSTREAM_PACK_INTEGRALS )
		stream_flags |= STREAM_OPS_PACK_DWORD;
	if ( p_flags & fSTREAM_PACK_UC_STRINGS )
		stream_flags |= STREAM_OPS_PACK_WSTRING;
	cOutStreamPreallocatedBuff out( p_buffer, p_size, stream_flags );

	if ( PR_SUCC(err) )
		err = out << ser_id;
	if ( PR_SUCC(err) )
		err = _streamSerializeStruct( dsc, p_object, out, 0, 0 );

	if ( p_output_size )
		*p_output_size = out.size();
	if ( PR_SUCC(err) && p_buffer && !out.filled() )
		err = errBUFFER_TOO_SMALL;
	
	return err;
}
// AVP Prague stamp end



// ---
static const cSerDescriptor* _find_dsc( const cSerDescriptor* src, const cSerializable* obj ) {
	tUINT id = obj->getIID();
	if ( id == src->m_unique )
		return src;

	const cSerDescriptorField* field;
	for( field=src->m_fields; field->m_flags != SDT_FLAG_ENDPOINT; ++field ) {
		if ( !(field->m_flags & SDT_FLAG_BASE) )
			continue;
		src = cRegistrar::find( field->m_id );
		if ( !src )
			break;
		if ( id == src->m_unique )
			return src;
		return ::_find_dsc( src, obj );
	}
	return 0;
}
  

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, StreamDeserialize )
// Parameters are:
extern "C" tERROR pr_call Root_StreamDeserialize( hi_Root _this, cSerializable** p_object, const tBYTE* p_buffer, tDWORD p_size, tDWORD* p_accepted_size ) {
	cERROR err;
	PR_TRACE_A0( _this, "Enter Serializer::StreamDeserialize method" );
	
	if ( p_accepted_size )
		*p_accepted_size = 0;
	
	if ( !p_object || !p_buffer || !p_size ) {
		PR_TRACE(( _this, prtERROR, "krn\tStream deserialize: parameter invalid (obj=0x%08x, buff=0x%08x, size=%d", p_object, p_buffer, p_size ));
		err = errPARAMETER_INVALID;
	}
	
	else {
		tUINT                 unique;
		const cSerDescriptor* dsc;
		cInStream             in( p_buffer, p_size );
		
		if ( PR_FAIL(err=in.pop(unique,p_size)) )
			PR_TRACE(( _this, prtERROR, "krn\tStream deserialize(%terr): get ser_id form stream", (tERROR)err ));
		else {
			dsc = cRegistrar::find(unique);
			if ( !dsc ) {
				PR_TRACE(( _this, prtERROR, "krn\tStream deserialize: description not found (%tser)", unique ));
				err = errOBJECT_NOT_FOUND;
			}
		}

		if ( PR_SUCC(err) ) {
			// in case (*p_object != NULL) && (unique != (*p_object)->GetIID())
			// check if *p_object is descendant of unique
			const cSerDescriptor* descendant = 0;
			if ( *p_object && (0==(descendant=::_find_dsc(dsc,*p_object))) ) {
				PR_TRACE(( _this, prtERROR, "krn\tStream deserialize: stream doesn't match to serializable object" ));
				err = errPARAMETER_INVALID;
			}
			if ( !descendant )
				;
			else if ( descendant == dsc )
				descendant = 0;
			else
				descendant = descendant;

			bool alloced = false;
			
			if ( PR_SUCC(err) && !*p_object ) {
				err = _alloc_str( &dsc, p_object, 0, 0, 0 );
				if ( PR_SUCC(err) )
					alloced = true;
				else
					PR_TRACE(( _this, prtERROR, "krn\tStream deserialize(%terr): memory", (tERROR)err ));
			}

			tDWORD end_point;
			if ( PR_SUCC(err) )
				err = in.pop_bytes( &end_point, sizeof(tDWORD), in.size() );

			tUINT field_num = 0;
			const cSerDescriptorField* field;
			const cSerDescriptor* saved = descendant;
			for( field=dsc->m_fields; PR_SUCC(err) && (!saved || descendant) && (field->m_flags != SDT_FLAG_ENDPOINT); ++field,++field_num ) {
				err = _streamDeserializeOneField( true, descendant, field, SADDR(*p_object,field), in, end_point, 0, 0, 0 );
				if ( PR_FAIL(err) ) {
					PR_TRACE(( _this, prtERROR, "krn\tStream deserialize(%terr): \"%s::%s\" (Str::field)", (tERROR)err, dsc->m_name, field->m_name ));
				}
			}
			
			(*p_object)->finalizeStreamDeser( err, field_num, dsc );

			if ( PR_SUCC(err) ) {
				if ( p_accepted_size )
					*p_accepted_size = in.accepted();
			}
			else if ( alloced ) {
				_free_str( dsc, *p_object );
				*p_object = 0;
			}
		}
	}
	
	PR_TRACE_A1( _this, "Leave Serializer::StreamDeserialize method, ret %terr", (tERROR)err );
	return err;
}
// AVP Prague stamp end

extern "C" tERROR pr_call Root_RegSerializeEx( hi_Root _this, const cSerializable* p_object, tDWORD p_ser_id, hREGISTRY p_registry, const tCHAR* p_path, tSerializeCallback p_callback_func, tPTR p_callback_params );

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegSerialize )
// Parameters are:
extern "C" tERROR pr_call Root_RegSerialize( hi_Root _this, const cSerializable* p_object, tDWORD p_ser_id, hREGISTRY p_registry, const tCHAR* key_name ) {
	cERROR err;
	PR_TRACE_FUNC_FRAME_( (hOBJECT)_this, "Serializer::RegSerialize", &err._ );
	return err = Root_RegSerializeEx( _this, p_object, p_ser_id, p_registry, key_name, 0, 0 );
}
// AVP Prague stamp end


extern "C" tERROR pr_call Root_RegDeserializeEx( hi_Root _this, cSerializable** p_object, hREGISTRY p_registry, const tCHAR* p_path, tDWORD p_uniqueId, tSerializeCallback p_callback_func, tPTR p_callback_params );

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegDeserialize )
// Parameters are:
extern "C" tERROR pr_call Root_RegDeserialize( hi_Root _this, cSerializable** p_object, hREGISTRY p_registry, const tCHAR* key_name, tDWORD p_uniqueId ) {
	cERROR   err;
	PR_TRACE_FUNC_FRAME_( (hOBJECT)_this, "Serializer::RegDeserialize", &err._ );
	return err = Root_RegDeserializeEx( _this, p_object, p_registry, key_name, p_uniqueId, 0, 0 );
}
// AVP Prague stamp end


// ---
const cSerDescriptorField* _find_field_by_ptr( const cSerializable* p_container, const cSerDescriptorField* p_key_field, const tVOID* field_ptr, const cSerDescriptor* dsc, cStrObj& path ) {
	
	const cSerDescriptorField* field = dsc->m_fields;
	const cSerDescriptorField* key_field = _find_key_field( field );
	cStrObj* current;
	cStrObj  local_path;
	if ( key_field && (key_field != p_key_field) ) {
		local_path = path;
		local_path.add_path_sect( *(const cStrObj*)SADDR(p_container,key_field) );
		current = &local_path;
	}
	else
		current = &path;
	
	for( ; (field->m_flags != SDT_FLAG_ENDPOINT); ++field ) {
		tVOID* addr = SADDR( p_container, field );
		if ( addr == field_ptr ) {
			if ( current == &local_path )
				path = local_path;
			return field;
		}
		if ( addr > field_ptr )
			return 0;
		if ( field->m_id <= tid_LAST_TYPE ) // its not a serializable
			continue;
		if ( field->m_flags & (SDT_FLAG_POINTER|SDT_FLAG_VECTOR) ) // its not a serializable
			continue;
		const cSerDescriptor* embed_dsc = cRegistrar::find( field->m_id );
		if ( !embed_dsc )
			continue;
		cStrObj tmp;
		cStrObj* saved = 0;
		if ( field->m_name && *field->m_name ) {
			tmp = *current;
			tmp.add_path_sect( field->m_name, cCP_ANSI );
			saved = current;
			current = &tmp;
		}
		const cSerDescriptorField* embed_field = _find_field_by_ptr( (cSerializable*)SADDR(p_container,field), key_field, field_ptr, embed_dsc, *current );
		if ( embed_field ) {
			if ( current != &path )
				path = *current;
			return embed_field;
		}
		if ( saved )
			current = saved;
	}
	return 0;
}


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegSerializeField )
// Parameters are:
extern "C" tERROR pr_call Root_RegSerializeField( hi_Root _this, const cSerializable* p_container, const tVOID* p_field_ptr, hREGISTRY p_registry, const tCHAR* p_path, tDWORD p_reserved ) {
	cERROR err;
	PR_TRACE_FUNC_FRAME_( (hOBJECT)_this, "Serializer::RegSerializeField", &err._ );
	
	if ( !p_container || !p_field_ptr || !p_registry ) {
		PR_TRACE(( _this, prtERROR, "krn\tRegSerializeField: parameter invalid" ));
		return err = errPARAMETER_INVALID;
	}
	
	const cSerDescriptor* dsc = cRegistrar::find( p_container->getIID() );
	if ( !dsc ) {
		PR_TRACE(( _this, prtERROR, "krn\tRegSerializeField: description not found (%tser)", p_container->getIID() ));
		return err = errOBJECT_NOT_FOUND;
	}
	
	cStrObj path( p_path, cCP_ANSI );
	const cSerDescriptorField* field = _find_field_by_ptr( p_container, 0, p_field_ptr, dsc, path );
	if ( !field ) {
		PR_TRACE(( _this, prtERROR, "krn\tRegSerializeField: embedded description not found (%tser)", dsc->m_unique ));
		return err = errOBJECT_NOT_FOUND;
	}
	if ( field->m_flags & SDT_FLAG_KEY_NAME )
		return err = errNOT_SUPPORTED; // serialize field to key name is not supported

	cAutoRegKey key;
	cStrBuff name( path, cCP_ANSI );
	if ( PR_FAIL(err=key.open(p_registry,cRegRoot,name,cTRUE)) ) {
		PR_TRACE(( _this, prtERROR, "krn\tRegSerializeField (%terr): cannot open key (%s)", (tERROR)err, (tCHAR*)name ));
		return err;
	}

	bool key_name_used = false;
	err = _regSerializeOneField( (tVOID*)p_field_ptr, p_registry, key, field, 0, key_name_used, 0, 0, 0 );
	if ( PR_FAIL(err) )	{
		PR_TRACE(( _this, prtERROR, "krn\tRegSerializeField (%terr): \"%s::%s\", path:%s", (tERROR)err, dsc->m_name, field->m_name, (tCHAR*)name ));
	}

	return err;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegDeserializeField )
// Parameters are:
extern "C" tERROR pr_call Root_RegDeserializeField( hi_Root _this, const cSerializable* p_container, tVOID* p_field_ptr, hREGISTRY p_registry, const tCHAR* p_path, tDWORD p_reserved ) {
	cERROR err;
	tDWORD unique;
	PR_TRACE_FUNC_FRAME_( (hOBJECT)_this, "Serializer::RegDeserializeField", &err._ );

	if ( !p_registry || !p_container ) {
		PR_TRACE(( _this, prtERROR, "krn\tReg deserialize: parameter invalid" ));
		return errPARAMETER_INVALID;
	}
	
	unique = p_container->getIID();
	
	const cSerDescriptor* dsc = cRegistrar::find(unique);
	if ( !dsc ) {
		PR_TRACE(( _this, prtERROR, "krn\tReg deserialize: description not found (%tser)", unique ));
		return err = errOBJECT_NOT_FOUND;
	}
	
	cStrObj path( p_path, cCP_ANSI );
	const cSerDescriptorField* field = _find_field_by_ptr( p_container, 0, p_field_ptr, dsc, path );
	if ( !field ) {
		PR_TRACE(( _this, prtERROR, "krn\tRegSerializeField: embedded description not found (%tser)", dsc->m_unique ));
		return err = errOBJECT_NOT_FOUND;
	}
	
	cAutoRegKey key;
	cStrBuff name( path, cCP_ANSI );
	if ( PR_FAIL(err=key.open(p_registry,cRegRoot,name,cFALSE)) ) {
		PR_TRACE(( _this, prtERROR, "krn\tRegDeserializeField (%terr): cannot open key (%s)", (tERROR)err, (tCHAR*)name ));
		return err;
	}
	
	tBOOL really_read = cFALSE;
	bool key_name_used = false;
	err = _regDeserializeOneField( p_field_ptr, p_registry, key, (tCHAR*)name, key_name_used, field->m_name, field, 0, 0, 0, really_read );
	if ( PR_FAIL(err) )	{
		PR_TRACE(( _this, prtERROR, "krn\tRegDeserializeField (%terr): \"%s::%s\", path:%s", (tERROR)err, dsc->m_name, field->m_name, (tCHAR*)name ));
	}
	
	return err;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegSerializeEx )
// Parameters are:
extern "C" tERROR pr_call Root_RegSerializeEx( hi_Root _this, const cSerializable* p_object, tDWORD p_ser_id, hREGISTRY p_registry, const tCHAR* p_path, tSerializeCallback p_callback_func, tPTR p_callback_params ) {
	cERROR err;
	PR_TRACE_FUNC_FRAME_( (hOBJECT)_this, "Serializer::RegSerializeEx", &err._ );
	
	if ( !p_object || !p_registry ) {
		PR_TRACE(( _this, prtERROR, "krn\tReg serialize: parameter invalid" ));
		return err = errPARAMETER_INVALID;
	}
	
	const cSerDescriptor* dsc = cRegistrar::find( p_object->getIID() );
	if ( !dsc ) {
		PR_TRACE(( _this, prtERROR, "krn\tReg serialize: description not found (%tser)", p_object->getIID() ));
		return err = errOBJECT_NOT_FOUND;
	}
	
	cAutoRegKey key;
	if ( PR_FAIL(err=key.open(p_registry,cRegRoot,p_path,cTRUE)) ) {
		PR_TRACE(( _this, prtERROR, "krn\tReg serialize(%terr): cannot open key (%s)", (tERROR)err, p_path ));
		return err;
	}
	
	cAutoRegKey str_key;
	const cSerDescriptorField* key_field = 0;
	const cSerDescriptorField* field = dsc->m_fields;
	const cAutoRegKey& work = _check_key_field( err, p_registry, key, field, key_field, p_object, str_key );
	
	bool key_name_used = false;
	for( ; PR_SUCC(err) && (field->m_flags != SDT_FLAG_ENDPOINT); ++field ) {
		err = _regSerializeOneField( SADDR(p_object,field), p_registry, work, field, key_field, key_name_used, 0, p_callback_func, p_callback_params );
		if ( PR_FAIL(err) ) {
			PR_TRACE(( _this, prtERROR, "krn\tReg serialize(%terr): \"%s::%s\" (Str::field)", (tERROR)err, dsc->m_name, field->m_name ));
		}
	}
	
	return err;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegDeserializeEx )
// Parameters are:
extern "C" tERROR pr_call Root_RegDeserializeEx( hi_Root _this, cSerializable** p_object, hREGISTRY p_registry, const tCHAR* p_path, tDWORD p_uniqueId, tSerializeCallback p_callback_func, tPTR p_callback_params ) {
	cERROR err;
	tDWORD unique;
	PR_TRACE_FUNC_FRAME_( (hOBJECT)_this, "Serializer::RegDeserializeEx", &err._ );
	
	if ( !p_registry || !p_object ) {
		PR_TRACE(( _this, prtERROR, "krn\tReg deserialize: parameter invalid" ));
		return errPARAMETER_INVALID;
	}
	
	if ( p_uniqueId != SERID_UNKNOWN )
		unique = p_uniqueId;
	else if ( *p_object )
		unique = (*p_object)->getIID();
	else {
		tTYPE_ID type;
		if ( PR_FAIL(err=p_registry->GetValue(0,cRegRoot,SER_UNIQUE_KEY_NAME,&type,(tPTR)&unique,sizeof(unique))) ) {
			PR_TRACE(( _this, prtERROR, "krn\tReg deserialize: don't know unique id" ));
			return err;
		}
		if ( type != tid_DWORD ) {
			PR_TRACE(( _this, prtERROR, "krn\tReg deserialize: don't know unique id" ));
			return err = errOBJECT_INCOMPATIBLE;
		}
	}
	
	const cSerDescriptor* dsc = cRegistrar::find(unique);
	if ( !dsc ) {
		PR_TRACE(( _this, prtERROR, "krn\tReg deserialize: description not found (%tser)", unique ));
		return err = errOBJECT_NOT_FOUND;
	}

	bool alloced;
	if ( !*p_object ) {
		err = _alloc_str( &dsc, p_object, 0, p_callback_func, p_callback_params );
		if ( PR_FAIL(err) ) {
			PR_TRACE(( _this, prtERROR, "krn\tReg deserialize: memory" ));
			return err;
		}
		alloced = true;
	}
	else if ( !(*p_object)->isBasedOn(unique) ) {
		PR_TRACE(( _this, prtERROR, "krn\tReg deserialize: branch doesn't match to serializable" ));
		return err = errPARAMETER_INVALID;
	}
	else
		alloced = false;
	
	cAutoRegKey key;
	
	tUINT field_num = 0;
	err = key.open( p_registry, cRegRoot, p_path, cFALSE );
	if ( PR_SUCC(err) ) {
		const cSerDescriptorField* field;
		bool key_name_used = false;
		for( field=dsc->m_fields; PR_SUCC(err) && (field->m_flags != SDT_FLAG_ENDPOINT); ++field ) {
			tBOOL really_read = cFALSE;
			err = _regDeserializeOneField( SADDR(*p_object,field), p_registry, key, p_path, key_name_used, field->m_name, field, 0, p_callback_func, p_callback_params, really_read );
			_check_err( err );
			if ( PR_SUCC(err) ) {
				if ( really_read )
					++field_num;
			}
			else {
				PR_TRACE(( _this, prtERROR, "krn\tReg deserialize(%terr): \"%s::%s\" (Str::field)", (tERROR)err, dsc->m_name, field->m_name ));
			}
		}
	}
	else {
		PR_TRACE(( _this, (err != errKEY_NOT_FOUND) ? prtERROR : prtNOTIFY, "krn\tReg deserialize(%terr): cannot open key (%s)", (tERROR)err, p_path ));
	}
	
	if ( PR_SUCC(err) )
		(*p_object)->finalizeRegDeser( err, field_num, dsc );
	else if ( alloced ) {
		_free_str( dsc, *p_object );
		*p_object = 0;
	}
	
	return err;
}
// AVP Prague stamp end



// ---
static tERROR cmp_object( cObject* o1, cObject* o2 ) {

	if ( o1 == o2 )
		return errOK;

	tIID iid1 = o1->propGetDWord( pgINTERFACE_ID );
	tIID iid2 = o2->propGetDWord( pgINTERFACE_ID );
	if ( iid1 != iid2 )
		return errNOT_MATCHED;
	
	cERROR err = ::g_root->sysCheckObject( o1, IID_BUFFER );
	if ( PR_FAIL(err) && (err != errINTERFACE_INCOMPATIBLE) )
		return err;
	
	if ( PR_SUCC(err) ) {
		cBuffer* b1 = (cBuffer*)o1;
		cBuffer* b2 = (cBuffer*)o2;
		tDWORD   s1, s2;
		tPTR     p1, p2;
		
		if ( PR_FAIL(err=b1->Lock(&p1)) )
			return err;
		if ( PR_FAIL(err=b2->Lock(&p2)) )
			return b1->Unlock(), err;

		if ( PR_FAIL(err=b1->Lock(&p1)) )
			return err;
		if ( PR_FAIL(err=b2->Lock(&p2)) )
			return b1->Unlock(), err;

		err = b1->propGet( 0, pgOBJECT_SIZE, &s1, sizeof(s1) );
		if ( PR_SUCC(err) )
			err = b2->propGet( 0, pgOBJECT_SIZE, &s2, sizeof(s2) );
		if ( PR_SUCC(err) && (s1 != s2) || memcmp(p1,p2,s1) )
			err = errNOT_MATCHED;

		b1->Unlock();
		b2->Unlock();
		return err;
	}
	
	else if ( PR_SUCC(::g_root->sysCheckObject(o1,IID_STRING)) )
		err = ((cString*)o1)->Compare( cSTRING_WHOLE, (cString*)o2, cSTRING_WHOLE, 0 );
	
	else if ( PR_SUCC(::g_root->sysCheckObject(o1,IID_IO)) )
		err = errOBJECT_INCOMPATIBLE;

	else
		err = errOBJECT_INCOMPATIBLE;
	
	return err;
}



#define CMP_PTR(p1,p2)     \
	if ( !p1 != !p2 )        \
		return errNOT_MATCHED; \
	if ( !p1 )							 \
		return errOK


// ---
static tERROR pr_call _compareOneField( const cSerDescriptorField* field, const tVOID* p1, const tVOID* p2, tDWORD flags ) {

	cERROR err;

	if ( flags & SDT_FLAG_VECTOR ) {
		tUINT el_size;
		tUINT offs;
		tMemChunk& v1 = *(tMemChunk*)p1;
		tMemChunk& v2 = *(tMemChunk*)p2;
		if ( v1.m_used != v2.m_used )
			return errNOT_MATCHED;
		if ( !v1.m_used )
			return errOK;

		if ( field->m_flags & SDT_FLAG_POINTER )
			el_size = sizeof(tPTR);
		else if ( -1 == (el_size=pr_sizeometer(field->m_id,0)) )
			return errBAD_SIGNATURE;
		for( offs=0; PR_SUCC(err) && (offs<v1.m_used); offs+=el_size )
			err = _compareOneField( field, VPTR(v1)+offs, VPTR(v2)+offs, flags & ~SDT_FLAG_VECTOR );
		return err;
	}

	bool ptr;
	if ( flags & SDT_FLAG_POINTER ) {
		p1 = *(tVOID**)p1;
		p2 = *(tVOID**)p2;
		CMP_PTR( p1, p2 );
		ptr = true;
	}
	else
		ptr = false;

	if ( field->m_id > tid_LAST_TYPE ) {
		tUINT dsc_id;
		if ( !ptr )
			dsc_id = field->m_id;
		else if ( ((cSerializable*)p2)->isBasedOn(dsc_id=((cSerializable*)p1)->getIID()) )
			;
		else if ( !((cSerializable*)p1)->isBasedOn(dsc_id=((cSerializable*)p2)->getIID()) )
			return errNOT_MATCHED;

		const cSerDescriptor* dsc = cRegistrar::find( dsc_id );
		if ( !dsc )
			return errBAD_SIGNATURE;

		for( field=dsc->m_fields; PR_SUCC(err) && (field->m_flags != SDT_FLAG_ENDPOINT); ++field )
			err = _compareOneField( field, SADDR(p1,field), SADDR(p2,field), field->m_flags );
		return err;
	}

	tUINT s;
	switch( field->m_id ) {
		case tid_STRING     : CMP_PTR( p1, p2 );  return CmpStr( p1, 0, cCP_ANSI, p2, 0, cCP_ANSI, cSTRING_Z );
		case tid_WSTRING    : CMP_PTR( p1, p2 );  return CmpStr( p1, 0, cCP_UNICODE, p2, 0, cCP_UNICODE, cSTRING_Z );
		case tid_OBJECT     : CMP_PTR( p1, p2 );  return cmp_object( (cObject*)p1, (cObject*)p2 );
		case tid_STRING_OBJ : 
			if ( !((cStrObj*)p1)->compare(*(cStrObj*)p2) )
				return errOK;
			return errNOT_MATCHED;
		case tid_BUFFER :
			s = ((cMemChunk*)p1)->used();
			if ( s != ((cMemChunk*)p2)->used() )
				return errNOT_MATCHED;
			if ( memcmp(((cMemChunk*)p1)->ptr(),((cMemChunk*)p2)->ptr(),s) )
				return errNOT_MATCHED;
			return errOK;
		case tid_BINARY :
			if ( memcmp(p1,p2,field->m_size) )
				return errNOT_MATCHED;
			return errOK;
		default : 
			if ( -1 == (s=pr_sizeometer(field->m_id,0)) )
				return errBAD_SIGNATURE;
			if ( memcmp(p1,p2,s) )
				return errNOT_MATCHED;
			return errOK;
	}
}



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, CompareSerializable )
// Parameters are:
extern "C" tERROR pr_call Root_CompareSerializable( hi_Root _this, const cSerializable* str1, const cSerializable* str2, tDWORD ser_id, tDWORD flags ) {
	cERROR err;
	PR_TRACE_FUNC_FRAME_( (hOBJECT)_this, "Serializer::CompareSerializable", &err._ );
	
	if ( !str1 != !str2 )
		return err = errNOT_MATCHED;

	if ( !str1 )
		return err;

	if ( ser_id == cSerializable::eIID ) {
		ser_id = str1->getIID();
		if ( !str2->isBasedOn(ser_id) )
			return err = errNOT_MATCHED;
	}

	else {
		bool based = str1->isBasedOn( ser_id );
		if ( based != str2->isBasedOn(ser_id) )
			return err = errPARAMETER_INVALID;
		if ( !based )
			return err;
	}

	const cSerDescriptor* dsc = cRegistrar::find(ser_id);
	if ( !dsc )
		return err = errOBJECT_NOT_FOUND;

	for( const cSerDescriptorField* field=dsc->m_fields; PR_SUCC(err) && (field->m_flags != SDT_FLAG_ENDPOINT); ++field )
		err = _compareOneField( field, SADDR(str1,field), SADDR(str2,field), field->m_flags );
	
	return err;
}
// AVP Prague stamp end


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, FindSerializableDescriptorByName )
// Parameters are:
extern "C" tERROR pr_call Root_FindSerializableDescriptorByName( hi_Root _this, cSerDescriptor** p_descriptor, const tCHAR* name ) {
	cERROR error;
	PR_TRACE_FUNC_FRAME_( (hOBJECT)_this, "Serializer::FindSerializableDescriptorByName", &error._ );
	
	const cSerDescriptor* dsc;
	if ( !p_descriptor || !name || !*name )
		error = errPARAMETER_INVALID;
	else if ( 0 == (dsc=cRegistrar::find(name)) )
		error = errNOT_FOUND;
	else {
		if( p_descriptor )
			*p_descriptor = (cSerDescriptor*)dsc;
		error = errOK;
	}
	
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, StreamSerializeField )
// Parameters are:
extern "C" tERROR pr_call Root_StreamSerializeField( hi_Root _this, const cSerializable* p_container, const tVOID* p_field, cOutStream* p_stream, tDWORD p_reserved ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::StreamSerializeField method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::StreamSerializeField method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, StreamDeserializeField )
// Parameters are:
extern "C" tERROR pr_call Root_StreamDeserializeField( hi_Root _this, tVOID* p_field, const cSerDescriptorField* p_descriptor, const cInStream* p_stream, tDWORD p_reserved ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::StreamDeserializeField method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::StreamDeserializeField method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



