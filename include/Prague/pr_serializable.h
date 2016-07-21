#if !defined( __pr_serializable_h )
#define __pr_serializable_h 

#if !defined(__DECLARE_SER_CONSTANTS_ONLY)

#include <Prague/pr_cpp.h>
#include <Prague/pr_registry.h>
#include <Prague/pr_serdescriptor.h>

#endif

#define SERID_UNKNOWN       ((tUINT)-1)
#define SER_SENDMSG_PSIZE   ((tDWORD*)-1)
#define SER_UNIQUE_KEY_NAME "unique_id"
#define SER_SETTINGS_ID     0x50
#define SER_STATISTICS_ID   0x60

#define SADDR(obj,dsc) ( ((tBYTE*)(obj)) + (dsc)->m_offset )

#define IS_SERIALIZABLE( _id_ )  ( (tDWORD)(_id_) > tid_LAST_TYPE )

#define MAKE_SER_ID( pid, ser_id )  (((pid)<<16) + ((ser_id)<<8))

#if !defined(__DECLARE_SER_CONSTANTS_ONLY)

// ---
#if !defined( cSerializable_struct )
#define cSerializable_struct


#if defined(__cplusplus)

// ---------------------------------------------------------------------------

#define DECLARE_CPP_STUFF( MYTYPE, BASETYPE )                                 \
	virtual bool pr_call isBasedOn( tUINT paramIID ) const {                    \
		if ( eIID == paramIID )                                                   \
			return true;                                                            \
		return BASETYPE::isBasedOn( paramIID );                                   \
	}                                                                           \
	virtual tERROR pr_call assign( const cSerializable& o, bool whole, bool agregate = false ) { \
		if ( o.isBasedOn(eIID) ) {                                                  \
			if( agregate ) *this += *(MYTYPE*)&o; else *this = *(MYTYPE*)&o;        \
			return errOK;                                                           \
		}                                                                         \
		return whole ? errOBJECT_INCOMPATIBLE : BASETYPE::assign( o, false, agregate ); \
	}                                                                           \
	virtual tERROR pr_call copy( cSerializable& o, bool whole ) const {         \
		tUINT eiid = o.getIID();                                                  \
		(void)eiid;										\
		if ( o.isBasedOn(eIID) ) {                                                \
			*(MYTYPE*)&o = *this;                                                   \
			return errOK;                                                           \
		}                                                                         \
		return whole ? errOBJECT_INCOMPATIBLE : BASETYPE::copy( o, false );       \
	}                                                                           \
	virtual tDWORD pr_call sizeOfStr()  const { return sizeof(*this); }         \
	virtual tDWORD pr_call getIID()     const { return eIID; }                  \
	virtual tERROR pr_call validate()         { return errOK; }

#define DECLARE_IID( MYTYPE, BASETYPE, pid, ser_id )            DECLARE_CPP_STUFF( MYTYPE, BASETYPE ); \
	enum { eIID = MAKE_SER_ID(pid,ser_id) }

#define DECLARE_IID_SETTINGS( MYTYPE, BASETYPE, pid )           DECLARE_IID( MYTYPE, BASETYPE, pid, SER_SETTINGS_ID )
#define DECLARE_IID_STATISTICS( MYTYPE, BASETYPE, pid )         DECLARE_IID( MYTYPE, BASETYPE, pid, SER_STATISTICS_ID )
#define DECLARE_IID_STATISTICS_EX( MYTYPE, BASETYPE, pid, num ) DECLARE_IID( MYTYPE, BASETYPE, pid, (SER_STATISTICS_ID+num) )

// ---
struct cSerializable {
protected:
	tLONG m_ref;

public:
	virtual ~cSerializable() {}

public:
	virtual tDWORD pr_call getIID()                                               const { return SERID_UNKNOWN; }
	virtual bool   pr_call isBasedOn( tUINT serId )                               const { return serId == cSerializable::eIID; }
	virtual tERROR pr_call assign( const cSerializable& /*o*/, bool /*whole*/, bool agregate = false ) { return errBAD_TYPE; }
	virtual tERROR pr_call copy( cSerializable& /*o*/, bool /*whole*/ )           const { return errBAD_TYPE; }
	virtual tDWORD pr_call sizeOfStr()                                            const { return sizeof(*this); }
	virtual tERROR pr_call validate()                                                   { return errOK; }
	virtual tVOID  pr_call clear()                                                      { ; }
	virtual tVOID  pr_call finalizeStreamDeser( tERROR err, tUINT field_num, const cSerDescriptor* dsc ) { ; }
	virtual tVOID  pr_call finalizeRegDeser( tERROR err, tUINT field_num, const cSerDescriptor* dsc )    { ; }
	
	const cSerDescriptor*      pr_call getDsc() const;
	const cSerDescriptorField* pr_call getField( const tVOID* addr, const cSerDescriptor* dsc = 0 ) const;
	const cSerDescriptorField* pr_call getFirstField() const;
	const cSerDescriptorField* pr_call getNextField( const cSerDescriptorField* field ) const;

	static const cSerDescriptorField* pr_call getFieldByOffset( const cSerDescriptor* dsc, tUINT offset );

	tBOOL  pr_call isEqual( const cSerializable* o, tDWORD flags = 0 ) const;
	tBOOL  pr_call isEqualBy( const cSerializable* o, tDWORD ser_id, tDWORD flags = 0 ) const;
	tVOID* pr_call getSubItem( tDWORD _id_ )                       const; // returns address of the item by SerializableId. \nIf it is PTR type it can point to NULL
	tVOID* pr_call getSubItem( const tCHAR* _id_ )                 const; // returns address of the item by name. if it is PTR type it can point to NULL
	tERROR pr_call getSubItemPtr( tDWORD _id_, tVOID*& ptr )       const; // returns address of the item. if it is PTR type it will be dereferenced (if possible)
	tERROR pr_call getSubItemPtr( const tCHAR* _id_, tVOID*& ptr ) const; // returns address of the item. if it is PTR type it will be dereferenced (if possible)
	tERROR pr_call saveOneField( cRegistry* reg, const tCHAR* root_ser_key, const tVOID* val, tUINT size );

	enum { eIID = SERID_UNKNOWN };
	tBOOL  pr_call operator == ( const cSerializable& o ) const { return isEqual(&o); }
	tBOOL  pr_call operator != ( const cSerializable& o ) const { return !isEqual(&o); }

	tLONG  pr_call getRef() const { return m_ref; }
	tERROR pr_call addRef()       { return CALL_Root_AddRefSerializable(::g_root, this); }
	tERROR pr_call release()      { return CALL_Root_DestroySerializable(::g_root, this); }

protected:
	cSerializable() : m_ref(1) {}
	cSerializable( const cSerializable& o ) : m_ref(1) {}
	void operator = ( const cSerializable& o ) {}
	void operator += ( const cSerializable& o ) {}
};

typedef cSerializable cSer;

// ---------------------------------------------------------------------------

#define SADDR(obj,dsc) ( ((tBYTE*)(obj)) + (dsc)->m_offset )

// ---
// returns descriptor of the serializable structure
inline const cSerDescriptor* pr_call cSerializable::getDsc() const {
	cSerDescriptor* item = 0;
	CALL_Root_FindSerializableDescriptor( ::g_root, &item, getIID() );
	return item;
}


// ---
// returns field descriptor by field address (offset)
inline const cSerDescriptorField* pr_call cSerializable::getField( const tVOID* addr, const cSerDescriptor* dsc ) const {
	if ( !dsc && (0 == (dsc=getDsc())) )
		return 0;
	const cSerDescriptorField* sub;
	const cSerDescriptorField* field = dsc->m_fields;
	for( ; field->m_flags != SDT_FLAG_ENDPOINT; ++field ) {
		tCPTR p = SADDR(this,field);
		if ( p == addr )
			return field;
		if ( (field->m_id < tid_LAST_TYPE) || (field->m_flags & SDT_FLAG_VECTOR) )
			continue;

		cSerializable* obj;
		if ( field->m_flags & SDT_FLAG_POINTER )
			*(cSerializable**)&obj = *(cSerializable**)p;
		else
			obj = (cSerializable*)p;
		if ( obj && PR_SUCC(CALL_Root_FindSerializableDescriptor(::g_root,(cSerDescriptor**)&dsc,field->m_id)) && (0 != (sub=obj->getField(addr,dsc))) )
			return sub;
	}
	return 0;
}


// ---
// returns field descriptor by field offset
inline const cSerDescriptorField* pr_call cSerializable::getFieldByOffset( const cSerDescriptor* dsc, tUINT offset )
{
	if ( !dsc )
		return NULL;
	
	const cSerDescriptorField* field = dsc->m_fields;
	for( ; field->m_flags != SDT_FLAG_ENDPOINT; ++field )
	{
		if ( field->m_offset == offset )
			return field;
		if ( (field->m_id < tid_LAST_TYPE) || (field->m_flags & SDT_FLAG_VECTOR) )
			continue;

		cSerDescriptor* subDsc = NULL;
		CALL_Root_FindSerializableDescriptor(::g_root, &subDsc, field->m_id);
		if( !subDsc )
			continue;

		const cSerDescriptorField* sub = getFieldByOffset(subDsc, offset);
		if( sub )
			return sub;
	}
	
	return NULL;
}

// ---
// returns descriptor of the first field of the serializable structure
inline const cSerDescriptorField* pr_call cSerializable::getFirstField() const {
	const cSerDescriptor* dsc = getDsc();
	if ( !dsc )
		return 0;
	const cSerDescriptorField* field = dsc->m_fields;
	if ( field->m_flags == SDT_FLAG_ENDPOINT )
		return 0;
	return field;
}


// ---
// returns descriptor of the next field
inline const cSerDescriptorField* pr_call cSerializable::getNextField( const cSerDescriptorField* field ) const {
	if ( !field || (field->m_flags == SDT_FLAG_ENDPOINT) || ((++field)->m_flags == SDT_FLAG_ENDPOINT) )
		return 0;
	return field;
}



// ---
inline tBOOL pr_call cSerializable::isEqual( const cSerializable* o, tDWORD flags ) const {
	return isEqualBy( o, cSerializable::eIID, flags );
}



// ---
inline tBOOL pr_call cSerializable::isEqualBy( const cSerializable* o, tDWORD ser_id, tDWORD flags ) const {
	return PR_SUCC(CALL_Root_CompareSerializable(::g_root,this,o,ser_id,flags));
}



// ---
// returns address of the item by SerializableId
// can ONLY find SERIALIZABLE fileds
// if returned item is PTR type it can point to NULL
inline tVOID* pr_call cSerializable::getSubItem( tDWORD _id_ ) const {
	const cSerDescriptor* dsc = getDsc();
	if ( !dsc )
		return 0;
	const cSerDescriptorField* field = dsc->m_fields;
	for( ; field->m_flags != SDT_FLAG_ENDPOINT; ++field ) {
		if ( field->m_id == _id_ )
			return SADDR(this,field);
	}
	return 0;
}


// ---
// returns address of the item by name
// can ONLY find SERIALIZABLE fileds
// if returned item is PTR type it can point to NULL
inline tVOID* pr_call cSerializable::getSubItem( const tCHAR* _id_ ) const {
	const cSerDescriptor* dsc = getDsc();
	if ( !dsc )
		return 0;
	const cSerDescriptorField* field = dsc->m_fields;
	cStrObj name_id( _id_ );
	for( ; field->m_flags != SDT_FLAG_ENDPOINT; ++field ) {
		if ( !name_id.compare(field->m_name) )
			return SADDR(this,field);
	}
	return 0;
}


// ---
inline tERROR pr_call cSerializable::getSubItemPtr( tDWORD _id_, tVOID*& ptr ) const { // returns address of the item. if it is PTR type it will be dereferenced (if possible)
	const cSerDescriptor* dsc = getDsc();
	if ( !dsc ) {
		ptr = 0;
		return errNOT_FOUND;
	}
	const cSerDescriptorField* field = dsc->m_fields;
	for( ; field->m_flags != SDT_FLAG_ENDPOINT; ++field ) {
		if ( field->m_id == _id_ ) {
			ptr = SADDR(this,field);
			if ( !(field->m_flags & SDT_FLAG_VECTOR) && (field->m_flags & SDT_FLAG_POINTER) )
				ptr = *(tVOID**)ptr;
			return errOK;
		}
	}
	ptr = 0;
	return errNOT_FOUND;
}


// ---
inline tERROR pr_call cSerializable::getSubItemPtr( const tCHAR* _id_, tVOID*& ptr ) const { // returns address of the item. if it is PTR type it will be dereferenced (if possible)
	const cSerDescriptor* dsc = getDsc();
	if ( !dsc ) {
		ptr = 0;
		return errNOT_FOUND;
	}
	const cSerDescriptorField* field = dsc->m_fields;
	cStrObj name_id( _id_ );
	for( ; field->m_flags != SDT_FLAG_ENDPOINT; ++field ) {
		if ( !name_id.compare(field->m_name) ) {
			ptr = SADDR(this,field);
			if ( !(field->m_flags & SDT_FLAG_VECTOR) && (field->m_flags & SDT_FLAG_POINTER) )
				ptr = *(tVOID**)ptr;
			return errOK;
		}
	}
	return 0;
}


// ---
inline tERROR cSerializable::saveOneField( cRegistry* reg, const tCHAR* root_ser_key, const tVOID* val, tUINT size ) {
	
	if ( -1 == size )
		return errPARAMETER_INVALID;

	if( !reg )
		return errPARAMETER_INVALID;

	const cSerDescriptorField* field = getField( val );
	if ( !field )
		return errNOT_FOUND;
	
	cAutoRegKey key;
	cERROR err = key.open( reg, cRegRoot, root_ser_key, cFALSE/*, cFALSE*/ );

	if ( PR_SUCC(err) )
		err = reg->SetValue( key, field->m_name, (tTYPE_ID)field->m_id, (tVOID*)val, size, cTRUE );
	return err;
}

// ---------------------------------------------------------------------------

#if !defined(_USE_VTBL)
	struct cRegistry;
#else
	typedef struct tag_hREGISTRY cRegistry;
#endif // if defined( __cplusplus )

template<class Type> class cSerObj {
public:
	cSerObj() : m_ser(NULL) {}
	cSerObj(const Type* ser) : m_ser(NULL) {*this = ser;}
	cSerObj(const Type& ser) : m_ser(NULL) {*this = ser;}
	cSerObj(const cSerObj& c) : m_ser(NULL) {*this = c;}
	~cSerObj() {clear();}

	tERROR init(tDWORD ser_id) {
		clear();
		if( ser_id == cSerializable::eIID )
			return errUNEXPECTED;
		return ::g_root->CreateSerializable(ser_id, (cSerializable**)&m_ser);
	}
	tERROR serialize( const tBYTE* buffer, tDWORD size, tDWORD* output_size=0 ) const {
		if ( !m_ser )
			return errOBJECT_NOT_INITIALIZED;
		return ::g_root->StreamSerialize( m_ser, SERID_UNKNOWN, (tBYTE*)buffer, size, output_size );
	}
	tERROR serialize( cRegistry* reg, const tCHAR* branch=0 ) const {
		if ( !m_ser )
			return errOBJECT_NOT_INITIALIZED;
		return ::g_root->RegSerialize( m_ser, SERID_UNKNOWN, reg, branch );
	}
	tERROR serialize( cRegistry* reg, const cStringObj& branch ) const {
		cStrBuff buff( branch );
		return serialize( reg, buff );
	}
	tERROR deserialize( const tBYTE* buffer, tDWORD size, tDWORD* accepted_size=0 ) {
		clear();
		return ::g_root->StreamDeserialize( (cSerializable**)&m_ser, buffer, size, accepted_size );
	}
	tERROR deserialize( const cRegistry* reg, const tCHAR* branch=0, tDWORD ser_id = cSerializable::eIID ) {
		tDWORD eiid;
		if ( m_ser ) {
			if ( ser_id == cSerializable::eIID )
				eiid = m_ser->getIID();
			else if ( !m_ser->isBasedOn(ser_id) )
				return errPARAMETER_INVALID;
			else
				eiid = ser_id;
		}
		else if ( ser_id != cSerializable::eIID )
			eiid = ser_id;
		else
			eiid = Type::eIID;
		return ::g_root->RegDeserialize( (cSerializable**)&m_ser, const_cast<cRegistry*>(reg), branch, eiid );
	}
	tERROR deserialize( const cRegistry* reg, const cStringObj& branch, tDWORD ser_id = cSerializable::eIID ) {
		cStrBuff buff( branch );
		return deserialize( reg, buff, ser_id );
	}
	void clear() {
		if( m_ser ) ::g_root->DestroySerializable(m_ser);
		m_ser = NULL;
	}
	Type* relinquish() {
		Type* ser = m_ser; m_ser = 0;
		return ser;
	}

	tERROR assign( const cSerializable* src, bool ref = false ) {
		if ( !src ) {
			clear();
			return errOK;
		}
		return assign( *src, ref );
	}
	
	tERROR assign( const cSerializable& src, bool ref = false ) { 
		if ( Type::eIID == cSerializable::eIID ) {
			if( ref )
				return clear(), m_ser = (Type *)&src, m_ser->addRef();
			
			if( m_ser && src.getIID() != m_ser->getIID() )
				clear();
			return ::g_root->CopySerializable( (cSerializable**)&m_ser, &src );
		}

		if ( !src.isBasedOn(Type::eIID) )
			return errPARAMETER_INVALID;

		if( ref )
			return clear(), m_ser = (Type *)&src, m_ser->addRef();

		if( !m_ser ) {
			cERROR err = ::g_root->CreateSerializable( Type::eIID, (cSerializable**)&m_ser );
			if ( PR_FAIL(err) )
				return err;
		}
		return m_ser->assign( src, false );
	}
	
	bool operator = (const cSerObj& src)    { return PR_SUCC(assign(src.m_ser));	}
	bool operator = (const Type* src)       { return PR_SUCC(assign(src)); }
	bool operator = (const Type& src)       { return PR_SUCC(assign(src));	}
	
	static cSerDescriptor* getDescriptor(tDWORD ser_id) {
		cSerDescriptor* hDescriptor = NULL;
		::g_root->FindSerializableDescriptor(&hDescriptor, ser_id);
		return hDescriptor;
	}

	bool operator !()  const                { return m_ser==0; }

	Type* operator->()                      { return m_ser; }
	const Type* operator->() const          { return m_ser; }
	Type& operator *()                      { return *m_ser; }
	Type** operator&()                      { return &m_ser; }
	Type*&          ptr_ref_type()          { return m_ser; }
	cSerializable*& ptr_ref()               { return (cSerializable*&)m_ser; }
	operator Type*()                        { return m_ser; }
	operator const Type*() const            { return m_ser; }

	bool operator == ( const cSerializable* o ) const {
		if ( !m_ser != !o )
			return false;
		if ( !m_ser )
			return true;
		return m_ser->isEqual( o, 0 );
	}
	bool operator != ( const cSerializable* o ) const { return !operator == (o); }
	bool operator == ( const cSerObj& o )       const { return  operator == (o.m_ser);	}
	bool operator != ( const cSerObj& o )       const { return !operator == (o.m_ser);	}

protected:
	Type* m_ser;
};

typedef cSerObj<cSerializable> cSerializableObj;

// ---------------------------------------------------------------------------

struct cSerString : public cSerializable
{
	cSerString() : m_ostr(NULL){}
	cSerString(cStringObj& str) : m_ostr(&str){ m_str = str; }
	~cSerString(){ if( m_ostr ) *m_ostr = m_str; }

	DECLARE_IID(cSerString, cSerializable, PID_KERNEL, 1);

	cStringObj  m_str;
private:
	cStringObj* m_ostr;
};

// ---------------------------------------------------------------------------

#else
	#error C++ compiler required.
#endif // defined(__cplusplus)

// ---
typedef cSerializable* hSerializable;

// ---------------------------------------------------------------------------

#endif

#ifdef  __DECLARE_SERIALIZABLE

#undef IMPLEMENT_SERIALIZABLE_BEGIN
#undef IMPLEMENT_SERIALIZABLE_END
#undef IMPLEMENT_SERIALIZABLE_END2
#undef SER_BASE
#undef SER_BASE_EX
#undef SER_FIELD
#undef SER_VALUE
#undef SER_VALUE_STREAM
#undef SER_VALUE_M
#undef SER_VALUE_C
#undef SER_VALUE_STREAM_C
#undef SER_VALUE_NO_NAMESPACE
#undef SER_KEY_VALUE
#undef SER_KEY_VALUE_C
#undef SER_VALUE_PTR
#undef SER_VALUE_PTR_STREAM
#undef SER_VALUE_PTR_NO_NAMESPACE
#undef SER_VALUE_PTR_M
#undef SER_VALUE_PTR_C
#undef SER_VALUE_PTR_STREAM_C
#undef SER_VECTOR
#undef SER_VECTOR_STREAM
#undef SER_VECTOR_M
#undef SER_VECTOR_C
#undef SER_VECTOR_OF_PTR
#undef SER_VECTOR_OF_PTR_C
#undef SER_VECTOR_KEY
#undef SER_VECTOR_KEY_C
#undef SER_PASSWORD

// ---
#define IMPLEMENT_SERIALIZABLE_BEGIN( StructTypeName, RegistryName ) \
namespace StructTypeName ## _namespace { \
	typedef StructTypeName SerializableType; \
	const char * Name = RegistryName; \
	cSerDescriptorField field_descriptor[] = {


// ---
#define IMPLEMENT_SERIALIZABLE_END()                                         \
	{ 0, 0, SDT_FLAG_ENDPOINT, 0, 0 } };                                       \
	static tVOID constructor( cSerializable* obj, cSerializable* other ) {     \
		if ( other )                                                             \
			new (obj) SerializableType( *(SerializableType*)other );                                       \
		else                                                                     \
			new (obj) SerializableType;                                                        \
	}                                                                          \
	static tVOID destructor( cSerializable* obj ) { ((SerializableType*)obj)->~SerializableType(); }   \
                                                                             \
	static cSerDescriptor struct_descriptor = {SerializableType::eIID, Name, sizeof(SerializableType), \
		constructor, destructor, field_descriptor };                             \
	cSerRegistrar serializable_registrar( &struct_descriptor ); };

// ---
#define IMPLEMENT_SERIALIZABLE_END2( FIRST_BASE )                            \
	{ 0, 0, SDT_FLAG_ENDPOINT, 0, 0 } };                                       \
	static tVOID constructor( cSerializable* obj, cSerializable* other ) {     \
		if ( other )                                                             \
			new (obj) SerializableType( *(SerializableType*)(FIRST_BASE*)other );                          \
		else                                                                     \
			new (obj) SerializableType;                                                        \
	}                                                                          \
	static tVOID destructor( cSerializable* obj ) { ((SerializableType*)(FIRST_BASE*)obj)->~SerializableType(); }   \
                                                                             \
	static cSerDescriptor struct_descriptor = {SerializableType::eIID, Name, sizeof(SerializableType), \
		constructor, destructor, field_descriptor };                             \
	cSerRegistrar serializable_registrar( &struct_descriptor ); };

// ---
#define SER_BASE( BaseTypeName, RegistryName ) \
	{ BaseTypeName::eIID, RegistryName, SDT_FLAG_BASE, ( ((tUINT)static_cast<BaseTypeName*>( ((SerializableType*)1)))-1 ), 0 },

#define SER_BASE_EX( BaseTypeName, type_flags, _id_, RegistryName ) \
	{ (tDWORD)_id_, RegistryName, type_flags|SDT_FLAG_BASE, ( ((tUINT)static_cast<BaseTypeName*>( ((SerializableType*)1)))-1 ), 0 },

// ---
#define SER_FIELD( varName, SerTypeFlags, SerDescrId, RegistryName, cool_serializer_ptr ) \
	{ (tDWORD)SerDescrId, RegistryName, SerTypeFlags, (tUINT)&((SerializableType*)0)->varName, sizeof(((SerializableType*)0)->varName), cool_serializer_ptr },

// ---
#define SER_VALUE( varName, varType_or_ser_id, RegistryName ) \
	SER_FIELD( varName, 0, varType_or_ser_id, RegistryName, 0 )

// ---
#define SER_VALUE_STREAM( varName, varType_or_ser_id, RegistryName ) \
	SER_FIELD( varName, SDT_FLAG_VSTREAM, varType_or_ser_id, RegistryName, 0 )

// ---
#define SER_VALUE_M( varName, varType_or_ser_id ) \
	SER_VALUE( m_##varName, varType_or_ser_id, #varName )

// ---
#define SER_VALUE_C( varName, varType_or_ser_id, RegistryName, cool_serializer_ptr ) \
	SER_FIELD( varName, 0, varType_or_ser_id, RegistryName, cool_serializer_ptr )

// ---
#define SER_VALUE_STREAM_C( varName, varType_or_ser_id, RegistryName, cool_serializer_ptr ) \
	SER_FIELD( varName, SDT_FLAG_VSTREAM, varType_or_ser_id, RegistryName, cool_serializer_ptr )

// ---
#define SER_KEY_VALUE( varName, Name ) \
	SER_FIELD( varName, SDT_FLAG_KEY_NAME, tid_STRING_OBJ, Name, 0 )

// ---
#define SER_KEY_VALUE_C( varName, cool_serializer_ptr ) \
	SER_FIELD( varName, SDT_FLAG_KEY_NAME, tid_STRING_OBJ, "", cool_serializer_ptr )

// ---
#define SER_VALUE_NO_NAMESPACE( varName, varType_or_ser_id, RegistryName ) \
	SER_FIELD( varName, SDT_FLAG_NO_NAMESPACE, varType_or_ser_id, RegistryName, 0 )

// ---
#define SER_VALUE_PTR( varName, varType_or_ser_id, RegistryName ) \
	SER_FIELD( varName, SDT_FLAG_POINTER, varType_or_ser_id, RegistryName, 0 )

// ---
#define SER_VALUE_PTR_STREAM( varName, varType_or_ser_id, RegistryName ) \
	SER_FIELD( varName, SDT_FLAG_POINTER|SDT_FLAG_VSTREAM, varType_or_ser_id, RegistryName, 0 )

// ---
#define SER_VALUE_PTR_NO_NAMESPACE( varName, varType_or_ser_id, RegistryName ) \
	SER_FIELD( varName, SDT_FLAG_POINTER|SDT_FLAG_NO_NAMESPACE, varType_or_ser_id, RegistryName, 0 )

// ---
#define SER_VALUE_PTR_M( varName, varType_or_ser_id ) \
	SER_VALUE_PTR( m_##varName, varType_or_ser_id, #varName )

// ---
#define SER_VALUE_PTR_C( varName, varType_or_ser_id, RegistryName, cool_serializer_ptr ) \
	SER_FIELD( varName, SDT_FLAG_POINTER, varType_or_ser_id, RegistryName, cool_serializer_ptr )

// ---
#define SER_VALUE_PTR_STREAM_C( varName, varType_or_ser_id, RegistryName, cool_serializer_ptr ) \
	SER_FIELD( varName, SDT_FLAG_POINTER|SDT_FLAG_VSTREAM, varType_or_ser_id, RegistryName, cool_serializer_ptr )

// ---
#define SER_VECTOR( varName, varType_or_ser_id, RegistryName ) \
	SER_FIELD( varName, SDT_FLAG_VECTOR, varType_or_ser_id, RegistryName, 0 )

// ---
#define SER_VECTOR_STREAM( varName, varType_or_ser_id, RegistryName ) \
	SER_FIELD( varName, SDT_FLAG_VECTOR|SDT_FLAG_VSTREAM, varType_or_ser_id, RegistryName, 0 )

// ---
#define SER_VECTOR_M( varName, varType_or_ser_id ) \
	SER_VECTOR( m_##varName, varType_or_ser_id, #varName )

// ---
#define SER_VECTOR_C( varName, varType_or_ser_id, RegistryName, cool_serializer_ptr ) \
	SER_FIELD( varName, SDT_FLAG_VECTOR, varType_or_ser_id, RegistryName, cool_serializer_ptr )

// ---
#define SER_VECTOR_OF_PTR( varName, varType_or_ser_id, RegistryName ) \
	SER_FIELD( varName, SDT_FLAG_VECTOR|SDT_FLAG_POINTER, varType_or_ser_id, RegistryName, 0 )

// ---
#define SER_VECTOR_OF_PTR_C( varName, varType_or_ser_id, RegistryName, cool_serializer_ptr ) \
	SER_FIELD( varName, SDT_FLAG_VECTOR|SDT_FLAG_POINTER, varType_or_ser_id, RegistryName, cool_serializer_ptr )

// ---
#define SER_VECTOR_KEY( varName, varType_or_ser_id, RegistryName ) \
	SER_FIELD( varName, SDT_FLAG_VECTOR|SDT_FLAG_KEY_NAME, varType_or_ser_id, RegistryName, 0 )

// ---
#define SER_VECTOR_KEY_C( varName, varType_or_ser_id, RegistryName, cool_serializer_ptr ) \
	SER_FIELD( varName, SDT_FLAG_VECTOR|SDT_FLAG_KEY_NAME, varType_or_ser_id, RegistryName, cool_serializer_ptr )

// ---
#define SER_PASSWORD( varName, RegistryName ) \
	SER_FIELD( varName, SDT_FLAG_PASSWORD, tid_STRING_OBJ, RegistryName, 0 )


#ifdef  __DECLARE_SERIALIZABLE_BASE

IMPLEMENT_SERIALIZABLE_BEGIN( cSerString, 0 )
	SER_VALUE( m_str,  tid_STRING_OBJ, "str" )
IMPLEMENT_SERIALIZABLE_END( );

#endif // __DECLARE_SERIALIZABLE_BASE

#else

#define IMPLEMENT_SERIALIZABLE_BEGIN( StructTypeName, RegistryName )
#define IMPLEMENT_SERIALIZABLE_END()
#define IMPLEMENT_SERIALIZABLE_END2( FIRST_BASE )
#define SER_BASE( BaseTypeName, RegistryName )
#define SER_BASE_EX( BaseTypeName, type_flags, _id_, RegistryName )
#define SER_FIELD( varName, SerTypeFlags, SerDescrId, RegistryName, cool_serializer_ptr )
#define SER_VALUE( varName, varType_or_ser_id, RegistryName )
#define SER_VALUE_STREAM( varName, varType_or_ser_id, RegistryName )
#define SER_VALUE_M( varName, varType_or_ser_id )
#define SER_VALUE_C( varName, varType_or_ser_id, RegistryName, cool_serializer_ptr )
#define SER_VALUE_STREAM_C( varName, varType_or_ser_id, RegistryName, cool_serializer_ptr )
#define SER_VALUE_NO_NAMESPACE( varName, varType_or_ser_id, RegistryName )
#define SER_KEY_VALUE( varName, Name )
#define SER_KEY_VALUE_C( varName, cool_serializer_ptr )
#define SER_VALUE_PTR( varName, varType_or_ser_id, RegistryName )
#define SER_VALUE_PTR_STREAM( varName, varType_or_ser_id, RegistryName )
#define SER_VALUE_PTR_NO_NAMESPACE( varName, varType_or_ser_id, RegistryName )
#define SER_VALUE_PTR_M( varName, varType_or_ser_id )
#define SER_VALUE_PTR_C( varName, varType_or_ser_id, RegistryName, cool_serializer_ptr )
#define SER_VALUE_PTR_STREAM_C( varName, varType_or_ser_id, RegistryName, cool_serializer_ptr )
#define SER_VECTOR( varName, varType_or_ser_id, RegistryName )
#define SER_VECTOR_STREAM( varName, varType_or_ser_id, RegistryName )
#define SER_VECTOR_M( varName, varType_or_ser_id )
#define SER_VECTOR_C( varName, varType_or_ser_id, RegistryName, cool_serializer_ptr )
#define SER_VECTOR_OF_PTR( varName, varType_or_ser_id, RegistryName )
#define SER_VECTOR_OF_PTR_C( varName, varType_or_ser_id, RegistryName, cool_serializer_ptr )
#define SER_VECTOR_KEY( varName, varType_or_ser_id, RegistryName )
#define SER_VECTOR_KEY_C( varName, varType_or_ser_id, RegistryName, cool_serializer_ptr )
#define SER_PASSWORD( varName, RegistryName )


// ---------------------------------------------------------------------------

#endif // __DECLARE_SERIALIZABLE

#endif // __DECLARE_SER_CONSTANTS_ONLY

#endif // __pr_serializable_h
