/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	pr_system_s.cpp
 * \author	Pavel Mezhuev
 * \date	09.12.2002 17:34:41
 * \brief	Prague-system-stubs.
 * 
 */
//************************************************************************

#include <prague.h>
#include "rpc_system.h"
#include "../pr_common.h"

//************************************************************************

// ---
extern "C" tERROR PRSYS_ObjectCreate( handle_t IDL_handle, rpcOBJECT _this, rpcOBJECT* new_obj, tIID iid, tPID pid, tDWORD subtype ) {
	hOBJECT l_this = (hOBJECT)_this;
	if( !IS_OBJ_PROXY(l_this) )
		l_this = PR_THIS_STUB;
	return CALL_SYS_ObjectCreate( l_this, PR_OUT_REMOTE(new_obj), iid, pid, subtype );
}
// ---
extern "C" tERROR PRSYS_ObjectCreateQuick( handle_t IDL_handle, rpcOBJECT _this, rpcOBJECT* new_obj, tIID iid, tPID pid, tDWORD subtype ) {
	hOBJECT l_this = (hOBJECT)_this;
	if( !IS_OBJ_PROXY(l_this) )
		l_this = PR_THIS_STUB;
	return CALL_SYS_ObjectCreateQuick( l_this, PR_OUT_REMOTE(new_obj), iid, pid, subtype );
}
// ---
extern "C" tERROR PRSYS_ObjectCreateDone( handle_t IDL_handle, rpcOBJECT _this ) {
	return CALL_SYS_ObjectCreateDone( PR_THIS_STUB );
}
// ---
extern "C" tERROR PRSYS_ObjectRevert( handle_t IDL_handle, rpcOBJECT _this, tBOOL reuse_custom_prop ) {
	return CALL_SYS_ObjectRevert( PR_THIS_STUB, reuse_custom_prop );
}
// ---
extern "C" tERROR PRSYS_ObjectClose( handle_t IDL_handle, rpcOBJECT _this ) {
#if defined (_WIN32)
	__try{
#endif
		return CALL_SYS_ObjectClose(CALL_SYS_ParentGet((hOBJECT)_this, IID_ANY));
#if defined (_WIN32)
	}
	__except(1) {
		return errUNEXPECTED;
	}
#endif
}

// ---
extern "C" tERROR PRSYS_Recognize( handle_t IDL_handle, rpcOBJECT _this, tIID by_iid, tPID by_pid, tDWORD type ) {
	return CALL_SYS_Recognize( PR_THIS_STUB, by_iid, by_pid, type );
}

// ---
extern "C" tERROR PRSYS_PropertyGet( handle_t IDL_handle, rpcOBJECT _this, tDWORD* out_size, tPROPID prop_id, /* [size_is][out] */ tCHAR* buffer, tDWORD size ) {
	tERROR err = CALL_SYS_PropertyGet( PR_THIS_STUB, out_size, prop_id, buffer, size );
	if ( buffer && PR_PROP_TYPE(prop_id) == pTYPE_OBJECT )
		*(rpcOBJECT*)buffer = PR_REMOTE( *(hOBJECT*)buffer);
	return err;
}
// ---
extern "C" tERROR PRSYS_PropertyGetStr( handle_t IDL_handle, rpcOBJECT _this, tDWORD* out_size, tPROPID prop_id, /* [size_is][out] */ tCHAR* buffer, tDWORD size, tCODEPAGE receive_cp ) {
	return CALL_SYS_PropertyGetStr( PR_THIS_STUB, out_size, prop_id, buffer, size, receive_cp );
}


// ---
extern "C" tERROR PRSYS_PropertySet( handle_t IDL_handle, rpcOBJECT _this, tDWORD* out_size, tPROPID prop_id, /* [size_is][in] */ const tCHAR* buffer, tDWORD size ) {
	hOBJECT l_this = (hOBJECT)_this;

	if( !IS_OBJ_PROXY(l_this) )
	{
		l_this = PR_THIS_STUB;
		if( IS_PROP_CUSTOM() )
		{
			ENUM_STUB_BEGIN(l_this)
				if( _stub != (hOBJECT)_this )
					SYS_PropertySet(l_stub->m_connect->m_handle, l_stub->m_object, NULL, prop_id, (tCHAR*)buffer, size);
			ENUM_STUB_END()
			prop_id |= pCALLBACK_TMP_STOP;
		}
	}

	hOBJECT object;
	if( buffer && (PR_PROP_TYPE(prop_id) == pTYPE_OBJECT) )
	{
		object = PR_LOCAL(*(rpcOBJECT*)buffer);
		buffer = (const tCHAR*)&object;
		size = sizeof(hOBJECT);
	}

	return CALL_SYS_PropertySet( l_this, out_size, prop_id, (tPTR)buffer, size );
}
// ---
extern "C" tERROR PRSYS_PropertySetStr( handle_t IDL_handle, rpcOBJECT _this, tDWORD* out_size, tPROPID prop_id, /* [size_is][in] */ const tCHAR* pStrz, tDWORD size, tCODEPAGE src_cp ) {
	hOBJECT l_this = (hOBJECT)_this;

	if( !IS_OBJ_PROXY(l_this) )
	{
		l_this = PR_THIS_STUB;
		if( IS_PROP_CUSTOM() )
		{
			ENUM_STUB_BEGIN(l_this)
				if( _stub != (hOBJECT)_this )
					SYS_PropertySetStr(l_stub->m_connect->m_handle, l_stub->m_object, NULL, prop_id, pStrz, size, src_cp);
			ENUM_STUB_END()
			prop_id |= pCALLBACK_TMP_STOP;
		}
	}

	return CALL_SYS_PropertySetStr( l_this, out_size, prop_id, (tPTR)pStrz, size, src_cp );
}

extern "C" tERROR PRSYS_PropertySetHeap( handle_t IDL_handle, rpcOBJECT _this, tDWORD heap_pid )
{
	hOBJECT l_this = (hOBJECT)_this;

	if( !IS_OBJ_PROXY(l_this) )
	{
		l_this = PR_THIS_STUB;
		ENUM_STUB_BEGIN(l_this)
			if( _stub != (hOBJECT)_this )
				SYS_PropertySetHeap(l_stub->m_connect->m_handle, l_stub->m_object, heap_pid);
		ENUM_STUB_END()
	}

	hOBJECT l_heap;
	tERROR l_error;

	if( PR_FAIL( l_error = CALL_SYS_ObjectCreateQuick(l_this, &l_heap, IID_HEAP, heap_pid, 0)) )
		return l_error;

	return CALL_SYS_PropertySetObj(l_this, pgOBJECT_HEAP | pCALLBACK_TMP_STOP, l_heap);
}


// ---
extern "C" tERROR PRSYS_PropertyGetStrCP( handle_t IDL_handle, rpcOBJECT _this, tCODEPAGE* cp, tPROPID prop_id ) {
	return CALL_SYS_PropertyGetStrCP( PR_THIS_STUB, cp, prop_id );
}
// ---
extern "C" tERROR PRSYS_PropertyIsPresent( handle_t IDL_handle, rpcOBJECT _this, tPROPID prop_id ) {
	return CALL_SYS_PropertyIsPresent( PR_THIS_STUB, prop_id );
}
// ---
extern "C" tERROR PRSYS_PropertyDelete( handle_t IDL_handle, rpcOBJECT _this, tPROPID prop_id ) {
	hOBJECT l_this = (hOBJECT)_this;

	if( !IS_OBJ_PROXY(l_this) )
	{
		l_this = PR_THIS_STUB;
		if( IS_PROP_CUSTOM() )
		{
			ENUM_STUB_BEGIN(l_this)
				if( _stub != (hOBJECT)_this )
					SYS_PropertyDelete(l_stub->m_connect->m_handle, l_stub->m_object, prop_id);
			ENUM_STUB_END()
		}
	}

	return CALL_SYS_PropertyDelete( l_this, prop_id | pCALLBACK_TMP_STOP );
}
// ---
extern "C" tDWORD PRSYS_PropertySize( handle_t IDL_handle, rpcOBJECT _this, tPROPID prop_id ) {
	return CALL_SYS_PropertySize( PR_THIS_STUB, prop_id );
}

extern "C" tERROR PRSYS_PropertyEnumId( handle_t IDL_handle, rpcOBJECT _this, tPROPID* prop_id_result, tDWORD* cookie ) {
	return CALL_SYS_PropertyEnumId( PR_THIS_STUB, prop_id_result, cookie );
}

// ---
extern "C" tERROR PRSYS_UserDataGet( handle_t IDL_handle, rpcOBJECT _this, tDWORD* data ) {
	return PR_THIS_STUB->sys->UserDataGet( PR_THIS_STUB, data );
}
// ---
extern "C" tERROR PRSYS_UserDataSet( handle_t IDL_handle, rpcOBJECT _this, tDWORD data ) {
	return PR_THIS_STUB->sys->UserDataSet( PR_THIS_STUB, data );
}

