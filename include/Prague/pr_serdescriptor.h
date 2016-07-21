#ifndef __pr_serdescriptor_h
#define __pr_serdescriptor_h

#include <Prague/prague.h>
#include <Prague/iface/i_reg.h>

// ---
struct cCoolSerializer;
struct cSerializable;


// ---
typedef tVOID (*tSerConstructor)( cSerializable* obj, cSerializable* other );
typedef tVOID (*tSerDestructor)( cSerializable* obj );


// ---
#define SDT_FLAG_POINTER        0x0010
#define SDT_FLAG_VECTOR         0x0020
#define SDT_FLAG_KEY_NAME       0x0040
#define SDT_FLAG_BASE           0x0080
#define SDT_FLAG_VSTREAM        0x0100
#define SDT_FLAG_PASSWORD       0x0200
#define SDT_FLAG_NO_NAMESPACE   0x0400
#define SDT_FLAG_CANT_BE_LOCKED 0x0800
#define SDT_FLAG_FINALIZER      0x1000
#define SDT_FLAG_ENDPOINT       ((tDWORD)(-1))

 
// ---
//typedef union tag_cSerDescrId {
//	tDWORD   m_unique;
//	tTYPE_ID m_type;
//} tSerDescrId;


// ---
struct tag_cSerDescriptorField {
	tUINT                  m_id;
	const tCHAR*           m_name;
	tUINT                  m_flags; // type and flags
	tUINT                  m_offset;
	tUINT                  m_size;
	const cCoolSerializer* m_serializer;
};
typedef struct tag_cSerDescriptorField cSerDescriptorField;


// ---
struct tag_cSerDescriptor {
	tUINT                      m_unique;
	const tCHAR*               m_name;
	tUINT                      m_size;
	tSerConstructor            m_constructor;
	tSerDestructor             m_destructor;
	const cSerDescriptorField* m_fields;
};

typedef struct tag_cSerDescriptor cSerDescriptor;

#if defined( __cplusplus )

struct cSerRegistrar;
extern cSerRegistrar* g_serializable_registrar_head;

// ---
struct cSerRegistrar {
	const cSerDescriptor* m_dsc;
	const cSerRegistrar*  m_next;
	cSerRegistrar( const cSerDescriptor* dsc ) : m_dsc(dsc), m_next(g_serializable_registrar_head) { g_serializable_registrar_head = this;  }
};

// ---
class cSerRegistrator {
public:
	cSerRegistrator() {
		const cSerRegistrar* tmp = g_serializable_registrar_head;
		while( tmp ) {
			::g_root->RegisterSerializableDescriptor( tmp->m_dsc );
			tmp = tmp->m_next;
		}
	}
};

class cOutStream;
class cInStream;

#define PR_SER_CONSTRUCTOR( StructTypeName, obj ) \
	new ((void*)*obj) StructTypeName

#define PR_SER_DESTRUCTOR( StructTypeName, obj ) \
	((StructTypeName*)obj)->~StructTypeName()


// ---
struct cCoolSerializer {
	// returning errNOT_IMPLEMENTED forces kernel to do all necessary work by itself
	virtual tERROR StreamSerialize( const tVOID* field, const cSerDescriptorField* field_dsc, cOutStream* out ) const { return errNOT_IMPLEMENTED; /*CALL_Root_StreamSerializeField(::g_root,field,field_dsc,out,0x1);*/ }
	virtual tERROR StreamDeserialize( tVOID* field, const cSerDescriptorField* field_dsc, cInStream* in )       const { return errNOT_IMPLEMENTED; /*CALL_Root_StreamDeserializeField(::g_root,field,field_dsc,in,0x1);*/ }

	virtual tERROR RegSerialize( const tVOID* field, const cSerDescriptorField* field_dsc, hREGISTRY out, const tCHAR* out_key ) const { return errNOT_IMPLEMENTED; /*CALL_Root_RegSerializeField(::g_root,field,field_dsc,out,out_key,0x1);*/ }
	virtual tERROR RegDeserialize( tVOID* field, const cSerDescriptorField* field_dsc, hREGISTRY in, const tCHAR* in_key )       const { return errNOT_IMPLEMENTED; /*CALL_Root_RegDeserializeField(::g_root,field,field_dsc,in,in_key,0x1);*/ }
};


#else // !defined (__cplusplus)
  
typedef struct tag_cOutStream     cOutStream;
typedef struct tag_cInStream      cInStream;

// ---
struct cCoolSerializerVtbl {
	tERROR (*StreamSerialize)( tVOID* obj, cOutStream* out, const cSerDescriptor* dsc );
	tERROR (*StreamDeserialize)( tVOID* obj, cInStream* in, const cSerDescriptor* dsc );

	tERROR (*RegSerialize)( tVOID* obj, hREGISTRY out, const cSerDescriptor* dsc );
	tERROR (*RegDeserialize)( tVOID* obj, hREGISTRY in, const cSerDescriptor* dsc );
};
// ---
typedef struct tag_cCoolSerializer {
	struct cCoolSerializerVtbl* vtbl;
} *cCoolSerializer;
  
#endif // __cplusplus

#endif // __pr_serdescriptor_h
