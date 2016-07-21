#if !defined(__kernel_serializer_h)
#define __kernel_serializer_h

#include <Prague/iface/i_csect.h>
#include <Prague/pr_serdescriptor.h>
#include <Prague/pr_serializable.h>

#include "kernel.h"

#define CALLER_IS_COOL_SERIALIZER   0x0001
#define VECTOR_REALLOC_CLUSTER      4

// ---
struct cRegistrar {
  tUINT                    m_id;
  const cSerDescriptor*    m_dsc;
  cRegistrar*              m_next;
	
  static cRegistrar*       g_head;
  static hCRITICAL_SECTION g_cs;
	
	struct cLocker {
		hCRITICAL_SECTION m_cs;
		cLocker( hCRITICAL_SECTION cs, bool exclusive = true ) : m_cs(cs) {
			if ( cs )
				cs->Enter( exclusive ? SHARE_LEVEL_WRITE : SHARE_LEVEL_READ );
		}
		~cLocker() {
			if ( m_cs )
				m_cs->Leave( 0 );
		}
	};
	
  static const cSerDescriptor* find( tUINT unique );
  static const cSerDescriptor* find( const tCHAR* unique );
  static tERROR                add( const cSerDescriptor* );
  static tERROR                free( const cSerDescriptor* );
};


// ---
tVOID                      _clean_ptr( tVOID*& obj, const cSerDescriptorField* f );
tVOID                      _clean_vector( tMemChunk& v, const cSerDescriptorField* field );                                             // vector of struct pointers
tERROR                     _check_integrated_ptr( tPTR& addr, const cSerDescriptorField* field, bool create );
const cSerDescriptorField* _find_key_field( const cSerDescriptorField* field );
tERROR                     _alloc_vect_el( tMemChunk& v, tPTR& ptr, tUINT el_size, const cSerDescriptorField* field, tSerializeCallback callback, tPTR params );
tERROR                     _alloc_vect_el( tMemChunk& v, tPTR& ptr, const cSerDescriptor** dsc, tBOOL pointer, const cSerDescriptorField* field, tSerializeCallback callback, tPTR params );
tERROR                     _alloc_vect_mem( tMemChunk& v, tUINT size, tUINT el_size );
bool                       _empty_vector( tMemChunk& v, const cSerDescriptor* dsc, tUINT el_size );
tERROR pr_call             _streamSerializeStruct( const cSerDescriptor* dsc, const tVOID* obj, cOutStream& out, tSerializeCallback func, tPTR params );
tERROR pr_call             _streamSerializeOneField( const cSerDescriptorField* field, const tVOID* obj, cOutStream& out, tDWORD flags, tSerializeCallback func, tPTR params );
tERROR pr_call             _streamDeserializeOneField( bool found_on, const cSerDescriptor*& dsc_to_find, const cSerDescriptorField* field, tVOID* obj, cInStream& in, tUINT end_offset, tDWORD flags, tSerializeCallback func, tPTR params );
tERROR pr_call             _regSerializeOneField( tPTR obj, hREGISTRY reg, const cAutoRegKey& key, const cSerDescriptorField* field, const cSerDescriptorField* key_field, bool& key_name_used, tDWORD flags, tSerializeCallback func, tPTR params );
tERROR pr_call             _regDeserializeOneField( tPTR obj, hREGISTRY reg, const cAutoRegKey& key, const tCHAR* key_name, bool& key_name_used, const tCHAR* value_name, const cSerDescriptorField* field, tDWORD flags, tSerializeCallback func, tPTR params, tBOOL& really_read );

#define QSADDR(obj,dsc)  (obj ? SADDR(obj,dsc) : 0)


// ---
inline tERROR _alloc_str( const cSerDescriptor** dsc, cSerializable** obj, const cSerDescriptorField* field, tSerializeCallback callback, tPTR params ) {
	if ( callback )	{
		tERROR err = callback( params, sa_alloc_ser_obj, obj, field, dsc );
		if ( err != warnDEFAULT )
			return err;
	}
	
	if ( !*obj ) {
		if ( !(*dsc)->m_size )
			return errBAD_SIZE;
		tERROR err = ::g_root->heapAlloc( (tPTR*)obj, (*dsc)->m_size );
		if ( PR_FAIL(err) )
			return err;
	}
	if ( (*dsc)->m_constructor )
		(*dsc)->m_constructor( *obj, 0 );
	return errOK;
}

// ---
inline tERROR _free_str( const cSerDescriptor* dsc, cSerializable* obj ) {
	if ( dsc->m_destructor )
		dsc->m_destructor( obj );
	::g_root->heapFree( (tPTR*)obj );
	return errOK;
}

// ---
inline const cSerDescriptorField* _go( tDWORD unique ) {
	const cSerDescriptor* str = cRegistrar::find( unique );
	if ( !str )
		return 0;
	return str->m_fields;
}

// ---
inline bool _check_err( cERROR& err ) {
	if ( (err == errKEY_NOT_FOUND) || (err==errKEY_INDEX_OUT_OF_RANGE) ) {
		err = errOK;
		return true;
	}
	return PR_FAIL(err);
}

// ---
inline tERROR _check_err_ptr( tERROR err, tPTR& obj, const cSerDescriptorField* field ) {
	if ( PR_FAIL(err) && obj && (field->m_flags & SDT_FLAG_POINTER) )
		_clean_ptr( obj, field );
	return err;
}


// ---
inline const cAutoRegKey& _check_key_field( cERROR& err, cRegistry* reg, const cAutoRegKey& base, const cSerDescriptorField* field, const cSerDescriptorField*& key_field, const tVOID* obj, cAutoRegKey& reserv ) {
	if ( key_field || (0 == (key_field=_find_key_field(field))) )
		return base;
	cStrBuff str( (const cStrObj*)SADDR(obj,key_field), cCP_ANSI );
	err = reserv.open( reg, base, str, cTRUE );
	return reserv;
}


#define VPTR(v) ((tBYTE*)v.m_ptr)


#endif
