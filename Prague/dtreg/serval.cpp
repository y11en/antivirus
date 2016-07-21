#include <Prague/prague.h>
#include <Prague/pr_serializable.h>

#include <Serialize/KLDTSer.h>


// ---
extern "C" tERROR pr_call setStrVal( HPROP prop, tPTR pValue, tDWORD* out_size ) {
	if ( !pValue || !*(tVOID**)pValue ) {
		*out_size = ::PROP_Set_Val( prop, 0, 0 );
		return errOK;
	}

	if ( PR_FAIL(CALL_SYS_ObjectValid(::g_root,*(hOBJECT*)pValue)) ) { // it is cStrinObj
		cStringObj& str = *(cStringObj*)pValue;
		if ( str.length() ) {
			*out_size = ::PROP_Set_Val( prop, (AVP_size_t)str.data(), str.memsize(cCP_UNICODE) );
			if ( !*out_size )
				return errUNEXPECTED;
		}
		else
			*out_size = ::PROP_Set_Val( prop, 0, 0 );
	}
	else {
		cERROR e = ::g_root->sysCheckObject( *(hOBJECT*)pValue, IID_STRING, PID_ANY, SUBTYPE_ANY, cFALSE );
		if ( PR_FAIL(e) )
			return e;

		cString* str = *(hSTRING*)pValue;
		e = str->LengthEx( out_size, cSTRING_WHOLE, cCP_UNICODE, cSTRING_Z );
		if ( PR_FAIL(e) )
			return e;

		if ( *out_size ) {
			cChunkBuff buff( ::g_root );
			tPTR ptr = buff.reserve( *out_size, false );
			if ( !ptr )
				return errNOT_ENOUGH_MEMORY;
			e = str->ExportToBuff( out_size, cSTRING_WHOLE, buff, *out_size, cCP_UNICODE, cSTRING_Z );
			if ( PR_FAIL(e) )
				return e;
			*out_size = ::PROP_Set_Val( prop, (AVP_size_t)buff.ptr(), *out_size );
			if ( !*out_size )
				return errUNEXPECTED;
		}
		else
			*out_size = ::PROP_Set_Val( prop, 0, 0 );
	}

	return errOK;
}




// ---
extern "C" tERROR pr_call getStrVal( HPROP prop, tPTR pValue, tDWORD* out_size ) {
	*out_size = ::PROP_Get_Val( prop, 0, 0 );
	if ( !pValue )
		return errOK;

	cERROR err;

	if ( PR_FAIL(::g_root->sysIsValidObject(*(hOBJECT*)pValue)) ) {
		cStringObj& str = *((cStringObj*)pValue);
		if ( *out_size ) {
			err = str.resize( (*out_size / sizeof(tWCHAR)) - 1 );
			if ( PR_FAIL(err) )
				return err;
			*out_size = ::PROP_Get_Val( prop, (tPTR)str.data(), *out_size );
			if ( !*out_size )
				return errUNEXPECTED;
		}
		else
			str.deallocate();
		return errOK;
	}

	err = ::g_root->sysCheckObject( *(hOBJECT*)pValue, IID_STRING, PID_ANY, SUBTYPE_ANY, cTRUE );
	if ( PR_FAIL(err) )
		return err;

	cString& str = **(hSTRING*)pValue;
	if ( *out_size ) {
		cChunkBuff buff( ::g_root );
		tPTR ptr = buff.reserve( *out_size, false );
		if ( !ptr )
			return errNOT_ENOUGH_MEMORY;
		*out_size = ::PROP_Get_Val( prop, ptr, *out_size );
		if ( !*out_size )
			return errUNEXPECTED;
		return str.ImportFromBuff( out_size, buff, *out_size, cCP_UNICODE, cSTRING_Z );
	}
	return str.ImportFromBuff( out_size, (void *)L"", sizeof(tWCHAR), cCP_UNICODE, cSTRING_Z );
}




// ---
extern "C" tERROR pr_call setSerVal( HPROP prop, tPTR pValue, tDWORD* out_size ) {
	if ( !pValue ) {
		*out_size = ::PROP_Set_Val( prop, 0, 0 );
		return errOK;
	}
	tDWORD size = 0;
	cChunkBuff buff( ::g_root );
	cERROR err = ::g_root->StreamSerialize( (cSerializable*)pValue, SERID_UNKNOWN, (tBYTE*)buff.ptr(), buff.allocated(), &size );
	if ( err == errBUFFER_TOO_SMALL ) {
		if ( !buff.reserve(size,false) )
			return errNOT_ENOUGH_MEMORY;
		err = ::g_root->StreamSerialize( (cSerializable*)pValue, SERID_UNKNOWN, (tBYTE*)buff.ptr(), buff.allocated(), &size );
	}
	if ( PR_FAIL(err) )
		return err;
	*out_size = ::PROP_Set_Val( prop, (AVP_size_t)buff.ptr(), size );
	if ( !*out_size )
		return errUNEXPECTED;
	return errOK;
}




// ---
extern "C" tERROR pr_call getSerVal( HPROP prop, tPTR pValue, tDWORD* out_size ) {
	*out_size = ::PROP_Get_Val( prop, 0, 0 );
	if ( !*out_size )
		return errUNEXPECTED;
	if ( !pValue )
		return errOK;

	cChunkBuff buff( ::g_root );
	tPTR ptr = buff.reserve( *out_size, false );

	*out_size = ::PROP_Get_Val( prop, ptr, *out_size );
	if ( !*out_size )
		return errUNEXPECTED;
	return ::g_root->StreamDeserialize( (cSerializable**)&pValue, (tBYTE*)ptr, *out_size, 0 );
}




// ---
extern "C" tERROR pr_call setBufVal( HPROP prop, tPTR pValue, tDWORD* out_size ) {
	if ( !pValue ) {
		*out_size = ::PROP_Set_Val( prop, 0, 0 );
		return errOK;
	}

	cMemChunk* buf = (cMemChunk*)pValue;
	*out_size = ::PROP_Set_Val( prop, (AVP_size_t)buf->ptr(), buf->used() );
	if ( !*out_size )
		return errUNEXPECTED;
	return errOK;
}




// ---
extern "C" tERROR pr_call getBufVal( HPROP prop, tPTR pValue, tDWORD* out_size ) {
	*out_size = ::PROP_Get_Val( prop, 0, 0 );
	if ( !pValue )
		return errOK;
	if ( !*out_size ) {
		((cMemChunk*)pValue)->used( 0 );
		return errOK;
	}
	tPTR ptr = ((cMemChunk*)pValue)->reserve( *out_size, false );
	if ( !ptr )
		return errNOT_ENOUGH_MEMORY;
	if ( !::PROP_Get_Val(prop,ptr,*out_size) )
		return errUNEXPECTED;
	return errOK;
}


