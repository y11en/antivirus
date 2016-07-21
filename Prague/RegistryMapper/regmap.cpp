// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  05 March 2007,  13:38 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2007.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- presents key of some registry as registry itself
// Author      -- Doukhvalow
// File Name   -- regmap.cpp
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface version,  )
#define Registry_VERSION ((tVERSION)1)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_root.h>
#include <Prague/plugin/p_regmap.h>
// AVP Prague stamp end



#include <m_utils.h>


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable cRegMap : public cRegistry {
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call ObjectClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations
	tERROR pr_call get_ro( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call set_ro( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call get_max_name_len( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call get_save_on_close( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call set_save_on_close( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call get_parent_key_str( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call set_parent_key_str( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call get_parent_key( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call set_parent_key( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call get_parent_reg( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call set_parent_reg( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations
	tERROR pr_call OpenKey( tRegKey* result, tRegKey p_key, const tCHAR* p_szSubKeyName, tBOOL p_bCreateIfNotExist );
	tERROR pr_call OpenKeyByIndex( tRegKey* result, tRegKey p_key, tDWORD p_index, tBOOL p_bClosePrev );
	tERROR pr_call OpenNextKey( tRegKey* result, tRegKey p_key, tBOOL p_bClosePrev );
	tERROR pr_call CloseKey( tRegKey p_key );
	tERROR pr_call GetKeyName( tDWORD* result, tRegKey p_key, tCHAR* p_name_buff, tDWORD p_size, tBOOL p_bFullName );
	tERROR pr_call GetKeyNameByIndex( tDWORD* result, tRegKey p_key, tDWORD p_index, tCHAR* p_name_buff, tDWORD p_size, tBOOL p_bFullName );
	tERROR pr_call GetValue( tDWORD* result, tRegKey p_key, const tCHAR* p_szValueName, tTYPE_ID* p_type, tPTR p_pValue, tDWORD p_size );
	tERROR pr_call GetValueByIndex( tDWORD* result, tRegKey p_key, tDWORD p_index, tTYPE_ID* p_type, tPTR p_pValue, tDWORD p_size );
	tERROR pr_call GetValueNameByIndex( tDWORD* result, tRegKey p_key, tDWORD p_index, tCHAR* p_name_buff, tDWORD p_size );
	tERROR pr_call SetValue( tRegKey p_key, const tCHAR* p_szValueName, tTYPE_ID p_type, tPTR p_pValue, tDWORD p_size, tBOOL p_bCreateIfNotExist );
	tERROR pr_call SetValueByIndex( tRegKey p_key, tDWORD p_index, tTYPE_ID p_type, tPTR p_pValue, tDWORD p_size, tBOOL p_bCreateIfNotExist );
	tERROR pr_call GetKeyCount( tDWORD* result, tRegKey p_key );
	tERROR pr_call GetValueCount( tDWORD* result, tRegKey p_key );
	tERROR pr_call DeleteKey( tRegKey p_key, const tCHAR* p_szSubKeyName );
	tERROR pr_call DeleteKeyByInd( tRegKey p_key, tDWORD p_index );
	tERROR pr_call DeleteValue( tRegKey p_key, const tCHAR* p_szValueName );
	tERROR pr_call DeleteValueByInd( tRegKey p_key, tDWORD p_index );
	tERROR pr_call Clean();
	tERROR pr_call CopyKey( tRegKey p_src_key, hREGISTRY p_destination, tRegKey p_dest_key );
	tERROR pr_call CopyKeyByName( const tCHAR* p_src, hREGISTRY p_dst );
	tERROR pr_call SetRootKey( tRegKey p_root, tBOOL p_close_prev );
	tERROR pr_call SetRootStr( const tCHAR* p_root, tBOOL p_close_prev );
	tERROR pr_call Flush( tBOOL p_force );

// Data declaration
	tBOOL      m_ro;            // read only object
	cRegistry* m_preg;          // parent registry
	tRegKey    m_pkey;          // parent key
	tCHAR      m_pkey_buf[0x100]; // parent key name buffer
	tCHAR*     m_pkey_str;      // parent key name
	tDWORD     m_pkey_str_size; // --
	tBOOL      m_clean_on_init; // clean sub registry on init
// AVP Prague stamp end



private:
	// ---
	tVOID _clean_pkey_str() {
		if ( m_pkey_str ) {
			if ( m_pkey_str != m_pkey_buf )
				heapFree( m_pkey_str );
			m_pkey_str = 0;
		}
		m_pkey_str_size = 0;
	}
	// ---
	tERROR _alloc_pkey_str( tDWORD size ) {
		if ( size <= sizeof(m_pkey_buf) ) {
			_clean_pkey_str();
			m_pkey_str = m_pkey_buf;
			m_pkey_str_size = sizeof(m_pkey_buf);
			return errOK;
		}
		if ( size <= m_pkey_str_size )
			return errOK;
		tERROR err = heapAlloc( (tPTR*)&m_pkey_str, size );
		if ( PR_FAIL(err) )
			return err;
		m_pkey_str_size = size;
		return err;
	}

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(cRegMap)
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Registry". Static(shared) property table variables
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR cRegMap::ObjectInit() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Registry::ObjectInit method" );

	cObj* dad = sysGetParent( IID_ANY );
	if ( PR_SUCC(sysCheckObject(dad,IID_REGISTRY)) ) {
		m_preg = (cRegistry*)dad;
		propInitialized( plPARENT_REG, cTRUE );
	}
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR cRegMap::ObjectInitDone() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Registry::ObjectInitDone method" );

	if ( !m_preg )
		return error = errOBJECT_NOT_INITIALIZED;

	if ( PR_FAIL(sysCheckObject((cObj*)m_preg,IID_REGISTRY)) )
		return error = errOBJECT_NOT_INITIALIZED;

	if ( !m_pkey && !(m_pkey_str && *m_pkey_str) )
		return error = errOBJECT_NOT_INITIALIZED;

	if ( m_pkey && m_pkey_str && *m_pkey_str )
		return error = errPARAMETER_INVALID;

	if ( m_ro && m_clean_on_init )
		return error = errPARAMETER_INVALID;


	if ( m_pkey ) {
		tDWORD len = 0;
		error = m_preg->GetKeyName( &len, m_pkey, 0, 0, cTRUE );
		if ( PR_SUCC(error) )
			error = _alloc_pkey_str( len );
		if ( PR_SUCC(error) )
			error = m_preg->GetKeyName( &len, m_pkey, m_pkey_str, len, cTRUE );
		if ( PR_FAIL(error) )
			_clean_pkey_str();
	}
	else
		error = m_preg->OpenKey( &m_pkey, cRegRoot, m_pkey_str, cFALSE );

	if ( PR_SUCC(error) && m_clean_on_init )
		error = Clean();

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR cRegMap::ObjectPreClose() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Registry::ObjectPreClose method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR cRegMap::ObjectClose() {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Registry::ObjectClose method" );

	_clean_pkey_str();

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
// Extended method comment
//    Receives message sent to the object or to the one of the object parents as broadcast
// Parameters are:
//   "msg_cls_id"    : Message class identifier
//   "msg_id"        : Message identifier
//   "send_point"    : Object where the message was send initially
//   "ctx"           : Context of the object processing
//   "receive_point" : Point of distributing message (tree top for RegisterMsgHandler call
//   "par_buf"       : Buffer of the parameters
//   "par_buf_len"   : Lenght of the buffer of the parameters
tERROR cRegMap::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Registry::MsgReceive method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_ro )
// Interface "Registry". Method "Get" for property(s):
//  -- OBJECT_RO
tERROR cRegMap::get_ro( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Registry::get_pgOBJECT_RO" );

	if ( buffer ) {
		if ( size < sizeof(m_ro) ) {
			*out_size = 0;
			return error = errBUFFER_TOO_SMALL;
		}
		*(tBOOL*)buffer = m_ro;
	}
	*out_size = sizeof(m_ro);

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_ro )
// Interface "Registry". Method "Set" for property(s):
//  -- OBJECT_RO
tERROR cRegMap::set_ro( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Registry::set_pgOBJECT_RO" );

	if ( size < sizeof(m_ro) ) {
		*out_size = 0;
		return error = errBUFFER_TOO_SMALL;
	}
	m_ro = *(tBOOL*)buffer;
	*out_size = sizeof(m_ro);

	PR_TRACE_A2( this, "Leave *SET* method set_ro for property pgOBJECT_RO, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_max_name_len )
// Interface "Registry". Method "Get" for property(s):
//  -- MAX_NAME_LEN
tERROR cRegMap::get_max_name_len( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method get_max_name_len for property pgMAX_NAME_LEN" );

	error = m_preg->propGet( out_size, prop, buffer, size );

	PR_TRACE_A2( this, "Leave *GET* method get_max_name_len for property pgMAX_NAME_LEN, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_save_on_close )
// Interface "Registry". Method "Get" for property(s):
//  -- SAVE_RESULTS_ON_CLOSE
tERROR cRegMap::get_save_on_close( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method get_save_on_close for property pgSAVE_RESULTS_ON_CLOSE" );

	error = m_preg->propGet( out_size, prop, buffer, size );

	PR_TRACE_A2( this, "Leave *GET* method get_save_on_close for property pgSAVE_RESULTS_ON_CLOSE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_save_on_close )
// Interface "Registry". Method "Set" for property(s):
//  -- SAVE_RESULTS_ON_CLOSE
tERROR cRegMap::set_save_on_close( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method set_save_on_close for property pgSAVE_RESULTS_ON_CLOSE" );

	error = m_preg->propSet( out_size, prop, buffer, size );

	PR_TRACE_A2( this, "Leave *SET* method set_save_on_close for property pgSAVE_RESULTS_ON_CLOSE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_parent_reg )
// Interface "Registry". Method "Get" for property(s):
//  -- PARENT_REG
tERROR cRegMap::get_parent_reg( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Registry::get_plPARENT_REG" );

	if ( buffer ) {
		if ( size < sizeof(m_preg) ) {
			*out_size = 0;
			return error = errBUFFER_TOO_SMALL;
		}
		*(cRegistry**)buffer = m_preg;
	}
	*out_size = sizeof(m_preg);
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_parent_reg )
// Interface "Registry". Method "Set" for property(s):
//  -- PARENT_REG
tERROR cRegMap::set_parent_reg( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Registry::set_plPARENT_REG" );

	if ( size < sizeof(m_preg) ) {
		*out_size = 0;
		return error = errBUFFER_TOO_SMALL;
	}
	*out_size = sizeof(m_preg);
	m_preg = *(cRegistry**)buffer;
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_parent_key )
// Interface "Registry". Method "Get" for property(s):
//  -- PARENT_KEY
tERROR cRegMap::get_parent_key( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Registry::get_plPARENT_KEY" );

	if ( buffer ) {
		if ( size < sizeof(m_preg) ) {
			*out_size = 0;
			return error = errBUFFER_TOO_SMALL;
		}
		*(tRegKey*)buffer = m_pkey;
	}
	*out_size = sizeof(m_pkey);
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_parent_key )
// Interface "Registry". Method "Set" for property(s):
//  -- PARENT_KEY
tERROR cRegMap::set_parent_key( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Registry::set_plPARENT_REG" );

	if ( size < sizeof(m_preg) ) {
		*out_size = 0;
		return error = errBUFFER_TOO_SMALL;
	}
	*out_size = sizeof(m_pkey);
	m_pkey = *(tRegKey*)buffer;
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_parent_key_str )
// Interface "Registry". Method "Get" for property(s):
//  -- PARENT_KEY_STR
tERROR cRegMap::get_parent_key_str( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Registry::get_plPARENT_KEY_STR" );

	if ( !m_pkey_str )
		*out_size = 0;
	else
		*out_size = (tDWORD)sizeof(tCHAR) + (tDWORD)strlen( m_pkey_str );

	if ( buffer && *out_size ) {
		if ( size < *out_size ) {
			*out_size = 0;
			return error = errBUFFER_TOO_SMALL;
		}
		memcpy( buffer, m_pkey_str, *out_size );
	}
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_parent_key_str )
// Interface "Registry". Method "Set" for property(s):
//  -- PARENT_KEY_STR
tERROR cRegMap::set_parent_key_str( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Registry::set_plPARENT_KEY_STR" );

	if ( buffer && size ) {
		error = _alloc_pkey_str( size );
		memcpy( m_pkey_str, buffer, size );
	}
	*out_size = size;
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, OpenKey )
// Extended method comment
//    if parametr bCreateIfNotExist is cTRUE and key was really created method must return errKEY_NOT_EXISTS warning
// Parameters are:
tERROR cRegMap::OpenKey( tRegKey* result, tRegKey p_key, const tCHAR* p_szSubKeyName, tBOOL p_bCreateIfNotExist ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Registry::OpenKey method" );
	
	if ( m_ro && p_bCreateIfNotExist )
		return error = errOBJECT_READ_ONLY;
	if ( p_key == cRegRoot )
		p_key = m_pkey;
	return error = m_preg->OpenKey( result, p_key, p_szSubKeyName, p_bCreateIfNotExist );
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, OpenKeyByIndex )
// Extended method comment
//    if parametr bCreateIfNotExist is cTRUE and key was really created method must return errKEY_NOT_EXISTS warning
// Parameters are:
tERROR cRegMap::OpenKeyByIndex( tRegKey* result, tRegKey p_key, tDWORD p_index, tBOOL p_bClosePrev ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Registry::OpenKeyByIndex method" );
	
	if ( p_key == cRegRoot )
		p_key = m_pkey;
	return error = m_preg->OpenKeyByIndex( result, p_key, p_index, p_bClosePrev );
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, OpenNextKey )
// Parameters are:
tERROR cRegMap::OpenNextKey( tRegKey* result, tRegKey p_key, tBOOL p_bClosePrev ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Registry::OpenNextKey method" );
	
	if ( p_key == cRegRoot )
		p_key = m_pkey;
	return error = m_preg->OpenNextKey( result, p_key, p_bClosePrev );
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, CloseKey )
// Parameters are:
tERROR cRegMap::CloseKey( tRegKey p_key ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Registry::CloseKey method" );

	if ( p_key == cRegRoot )
		return error = errOK;
	return m_preg->CloseKey( p_key );
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetKeyName )
// Parameters are:
//   "name_buff" : if this parametr is NULL method must returns length of the buffer to returns name
tERROR cRegMap::GetKeyName( tDWORD* result, tRegKey p_key, tCHAR* p_name_buff, tDWORD p_size, tBOOL p_bFullName ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Registry::GetKeyName method" );

	if ( p_key == cRegRoot )
		p_key = m_pkey;
	return error = m_preg->GetKeyName( result, p_key, p_name_buff, p_size, p_bFullName );
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetKeyNameByIndex )
// Parameters are:
//   "name_buff" : if this parametr is NULL method must returns length of the buffer to returns name
tERROR cRegMap::GetKeyNameByIndex( tDWORD* result, tRegKey p_key, tDWORD p_index, tCHAR* p_name_buff, tDWORD p_size, tBOOL p_bFullName ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Registry::GetKeyNameByIndex method" );

	if ( p_key == cRegRoot )
		p_key = m_pkey;
	return error = m_preg->GetKeyNameByIndex( result, p_key, p_index, p_name_buff, p_size, p_bFullName );
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetValue )
// Parameters are:
tERROR cRegMap::GetValue( tDWORD* result, tRegKey p_key, const tCHAR* p_szValueName, tTYPE_ID* p_type, tPTR p_pValue, tDWORD p_size ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Registry::GetValue method" );

	if ( p_key == cRegRoot )
		p_key = m_pkey;
	return error = m_preg->GetValue( result, p_key, p_szValueName, p_type, p_pValue, p_size );
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetValueByIndex )
// Parameters are:
tERROR cRegMap::GetValueByIndex( tDWORD* result, tRegKey p_key, tDWORD p_index, tTYPE_ID* p_type, tPTR p_pValue, tDWORD p_size ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Registry::GetValueByIndex method" );

	if ( p_key == cRegRoot )
		p_key = m_pkey;
	return error = m_preg->GetValueByIndex( result, p_key, p_index, p_type, p_pValue, p_size );
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetValueNameByIndex )
// Parameters are:
tERROR cRegMap::GetValueNameByIndex( tDWORD* result, tRegKey p_key, tDWORD p_index, tCHAR* p_name_buff, tDWORD p_size ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Registry::GetValueNameByIndex method" );

	if ( p_key == cRegRoot )
		p_key = m_pkey;
	return error = m_preg->GetValueNameByIndex( result, p_key, p_index, p_name_buff, p_size );
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetValue )
// Parameters are:
tERROR cRegMap::SetValue( tRegKey p_key, const tCHAR* p_szValueName, tTYPE_ID p_type, tPTR p_pValue, tDWORD p_size, tBOOL p_bCreateIfNotExist ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Registry::SetValue method" );

	if ( m_ro )
		return error = errOBJECT_READ_ONLY;
	if ( p_key == cRegRoot )
		p_key = m_pkey;
	return error = m_preg->SetValue( p_key, p_szValueName, p_type, p_pValue, p_size, p_bCreateIfNotExist );
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetValueByIndex )
// Parameters are:
tERROR cRegMap::SetValueByIndex( tRegKey p_key, tDWORD p_index, tTYPE_ID p_type, tPTR p_pValue, tDWORD p_size, tBOOL p_bCreateIfNotExist ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Registry::SetValueByIndex method" );

	if ( m_ro )
		return error = errOBJECT_READ_ONLY;
	if ( p_key == cRegRoot )
		p_key = m_pkey;
	return error = m_preg->SetValueByIndex( p_key, p_index, p_type, p_pValue, p_size, p_bCreateIfNotExist );
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetKeyCount )
// Parameters are:
tERROR cRegMap::GetKeyCount( tDWORD* result, tRegKey p_key ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Registry::GetKeyCount method" );

	if ( p_key == cRegRoot )
		p_key = m_pkey;
	return error = m_preg->GetKeyCount( result, p_key );
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetValueCount )
// Parameters are:
tERROR cRegMap::GetValueCount( tDWORD* result, tRegKey p_key ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Registry::GetValueCount method" );

	if ( p_key == cRegRoot )
		p_key = m_pkey;
	return error = m_preg->GetValueCount( result, p_key );
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, DeleteKey )
// Parameters are:
tERROR cRegMap::DeleteKey( tRegKey p_key, const tCHAR* p_szSubKeyName ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Registry::DeleteKey method" );

	if ( m_ro )
		return error = errOBJECT_READ_ONLY;
	if ( p_key == cRegRoot )
		p_key = m_pkey;
	return error = m_preg->DeleteKey( p_key, p_szSubKeyName );
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, DeleteKeyByInd )
// Parameters are:
tERROR cRegMap::DeleteKeyByInd( tRegKey p_key, tDWORD p_index ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Registry::DeleteKeyByInd method" );

	if ( m_ro )
		return error = errOBJECT_READ_ONLY;
	if ( p_key == cRegRoot )
		p_key = m_pkey;
	return error = m_preg->DeleteKeyByInd( p_key, p_index );
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, DeleteValue )
// Parameters are:
tERROR cRegMap::DeleteValue( tRegKey p_key, const tCHAR* p_szValueName ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Registry::DeleteValue method" );

	if ( m_ro )
		return error = errOBJECT_READ_ONLY;
	if ( p_key == cRegRoot )
		p_key = m_pkey;
	return error = m_preg->DeleteValue( p_key, p_szValueName );
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, DeleteValueByInd )
// Parameters are:
tERROR cRegMap::DeleteValueByInd( tRegKey p_key, tDWORD p_index ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Registry::DeleteValueByInd method" );

	if ( m_ro )
		return error = errOBJECT_READ_ONLY;
	if ( p_key == cRegRoot )
		p_key = m_pkey;
	return error = m_preg->DeleteValueByInd( p_key, p_index );
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Clean )
// Parameters are:
tERROR cRegMap::Clean() {
	tERROR error;
	tUINT  i;
	tDWORD count = 0;

	PR_TRACE_FUNC_FRAME( "Registry::Clean method" );

	error = m_preg->GetKeyCount( &count, m_pkey );
	for( i=0; PR_SUCC(error) && (i<count); ++i )
		error = m_preg->DeleteKeyByInd( m_pkey, i );

	error = m_preg->GetValueCount( &count, m_pkey );
	for( i=0; PR_SUCC(error) && (i<count); ++i )
		error = m_preg->DeleteValueByInd( m_pkey, i );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, CopyKey )
// Parameters are:
tERROR cRegMap::CopyKey( tRegKey p_src_key, hREGISTRY p_destination, tRegKey p_dest_key ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Registry::CopyKey method" );

	if ( p_src_key == cRegRoot )
		p_src_key = m_pkey;
	return error = m_preg->CopyKey( p_src_key, p_destination, p_dest_key );
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, CopyKeyByName )
// Parameters are:
tERROR cRegMap::CopyKeyByName( const tCHAR* p_src, hREGISTRY p_dst ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "Registry::CopyKeyByName method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetRootKey )
// Parameters are:
tERROR cRegMap::SetRootKey( tRegKey p_root, tBOOL p_close_prev ) {
	tERROR error = errNOT_SUPPORTED;
	PR_TRACE_FUNC_FRAME( "Registry::SetRootKey method" );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetRootStr )
// Parameters are:
tERROR cRegMap::SetRootStr( const tCHAR* p_root, tBOOL p_close_prev ) {
	tERROR error = errNOT_SUPPORTED;
	PR_TRACE_FUNC_FRAME( "Registry::SetRootStr method" );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Flush )
// Parameters are:
tERROR cRegMap::Flush( tBOOL p_force ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "Registry::Flush method" );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Registry". Register function
tERROR cRegMap::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Registry_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Registry_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "presents key of some registry as registry itself", 49 )
	mpLOCAL_PROPERTY_FN( pgOBJECT_RO, FN_CUST(get_ro), FN_CUST(set_ro) )
	mpLOCAL_PROPERTY_FN( pgMAX_NAME_LEN, FN_CUST(get_max_name_len), NULL )
	mpLOCAL_PROPERTY_BUF( pgCLEAN, cRegMap, m_clean_on_init, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_FN( pgSAVE_RESULTS_ON_CLOSE, FN_CUST(get_save_on_close), FN_CUST(set_save_on_close) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgROOT_POINT, FN_CUST(get_parent_key_str), FN_CUST(set_parent_key_str) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgROOT_KEY, FN_CUST(get_parent_key), FN_CUST(set_parent_key) )
	mpLOCAL_PROPERTY_REQ_WRITABLE_OI_FN( plPARENT_REG, FN_CUST(get_parent_reg), FN_CUST(set_parent_reg) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( plPARENT_KEY, FN_CUST(get_parent_key), FN_CUST(set_parent_key) )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( plPARENT_KEY_STR, FN_CUST(get_parent_key_str), FN_CUST(set_parent_key_str) )
	mSHARED_PROPERTY( pgOBJECT_CODEPAGE, ((tCODEPAGE)(cCP_ANSI)) )
mpPROPERTY_TABLE_END(Registry_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Registry)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(ObjectClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(Registry)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(Registry)
	mEXTERNAL_METHOD(Registry, OpenKey)
	mEXTERNAL_METHOD(Registry, OpenKeyByIndex)
	mEXTERNAL_METHOD(Registry, OpenNextKey)
	mEXTERNAL_METHOD(Registry, CloseKey)
	mEXTERNAL_METHOD(Registry, GetKeyName)
	mEXTERNAL_METHOD(Registry, GetKeyNameByIndex)
	mEXTERNAL_METHOD(Registry, GetValue)
	mEXTERNAL_METHOD(Registry, GetValueByIndex)
	mEXTERNAL_METHOD(Registry, GetValueNameByIndex)
	mEXTERNAL_METHOD(Registry, SetValue)
	mEXTERNAL_METHOD(Registry, SetValueByIndex)
	mEXTERNAL_METHOD(Registry, GetKeyCount)
	mEXTERNAL_METHOD(Registry, GetValueCount)
	mEXTERNAL_METHOD(Registry, DeleteKey)
	mEXTERNAL_METHOD(Registry, DeleteKeyByInd)
	mEXTERNAL_METHOD(Registry, DeleteValue)
	mEXTERNAL_METHOD(Registry, DeleteValueByInd)
	mEXTERNAL_METHOD(Registry, Clean)
	mEXTERNAL_METHOD(Registry, CopyKey)
	mEXTERNAL_METHOD(Registry, CopyKeyByName)
	mEXTERNAL_METHOD(Registry, SetRootKey)
	mEXTERNAL_METHOD(Registry, SetRootStr)
	mEXTERNAL_METHOD(Registry, Flush)
mEXTERNAL_TABLE_END(Registry)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "Registry::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_REGISTRY,                           // interface identifier
		PID_REGMAP,                             // plugin identifier
		0x00000000,                             // subtype identifier
		Registry_VERSION,                       // interface version
		VID_PETROVITCH,                         // interface developer
		&i_Registry_vtbl,                       // internal(kernel called) function table
		(sizeof(i_Registry_vtbl)/sizeof(tPTR)), // internal function table size
		&e_Registry_vtbl,                       // external function table
		(sizeof(e_Registry_vtbl)/sizeof(tPTR)), // external function table size
		Registry_PropTable,                     // property table
		mPROPERTY_TABLE_SIZE(Registry_PropTable),// property table size
		sizeof(cRegMap)-sizeof(cObjImpl),       // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Registry(IID_REGISTRY) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call Registry_Register( hROOT root ) { return cRegMap::Register(root); }
// AVP Prague stamp end



