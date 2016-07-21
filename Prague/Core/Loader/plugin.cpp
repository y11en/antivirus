// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  06 July 2006,  13:07 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Loader
// Purpose     -- Win32 plugin finder/loader
// Author      -- petrovitch
// File Name   -- plugin.cpp
// Additional info
//    This implementation finds loadable prague plugins in disk folder
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface version,  )
#define Plugin_VERSION ((tVERSION)1)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
// AVP Prague stamp end



#define IMPEX_EXPORT_LIB
#include <Prague/iface/e_loader.h>


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/pr_vtbl.h>
#include <Prague/pr_loadr.h>
#include "loader.h"
#include "mod_load.h"
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_ktrace.h>
#include <Prague/plugin/p_win32loader.h>
// AVP Prague stamp end



#if defined (OLD_METADATA_SUPPORT)
	#include "../IFaceInfo/AVPPveID.h"
	#include "../IFaceInfo/IFaceInfo.h"
	#include "prop_util.h"
#endif // OLD_METADATA_SUPPORT


#include <Prague/pr_time.h>
#include "plugin.h"
#include "../metadata/metadata.h"
#include <Prague/value_utils.h>


struct pr_novtable ModuleDATA;


// ---
class cPluginProtector {

protected:
	friend tERROR pr_call initPluginProtection();
	friend tVOID pr_call deinitPluginProtection();

#if defined( ADVANCED_PLUGIN_PROTECTOR )
	tUINT                   m_prot_counter;
	tProtector              m_prot;
	static CRITICAL_SECTION g_cs;     // synchro object to protect plugin's load process
#elif defined( GLOBAL_PLUGIN_PROTECTOR )
	static tProtector       g_prot;   // synchro object to protect plugin's load process
#else
	tProtector              m_prot;
#endif

public:
	cPluginProtector();
	
	tERROR g_init();
	tERROR g_deinit();

	tERROR init();
	tERROR deinit();

	tERROR lock( ModuleDATA& plugin );
	tERROR unlock( ModuleDATA& plugin );

	tERROR get( tProtector& prot );
	tVOID  release();
};



// ---
//class cSafeCounter {
//	volatile LONG m_val;
//public:
//	
//	cSafeCounter( LONG arg = 0 ) : m_val(arg) {}
//	~cSafeCounter(){}
//	
//	operator LONG () const { return InterlockedExchangeAdd( (LONG*)&m_val, 0 ); }
//
//	LONG operator ++ ( int )      { return InterlockedIncrement( &m_val );         } // only postfix form can be implemented
//	LONG operator -- ( int )      { return InterlockedDecrement( &m_val );         } // only postfix form can be implemented
//	LONG operator += ( LONG val ) { return InterlockedExchangeAdd( &m_val, val );  }
//	LONG operator -= ( LONG val ) { return InterlockedExchangeAdd( &m_val, -val ); }
//	LONG operator =  ( LONG val ) { return InterlockedExchange( &m_val, val );     }
//	
//};
typedef tUINT cSafeCounter;


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable ModuleDATA : public cPlugin {

private:
	ModuleDATA();

private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call ObjectClose();

// Property function declarations
	tERROR pr_call IsLoaded( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call get_name( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call set_name( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call auto_start( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call is_prague( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call prop_iface_count( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call get_time( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call prop_export_count( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call get_integral( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations
	tERROR pr_call GetInterfaceProperty( tDWORD* result, tDWORD p_index, tPROPID p_prop_id, tPTR p_buffer, tDWORD p_count );
	tERROR pr_call Load();
	tERROR pr_call Unload( tBOOL p_force );
	tERROR pr_call RegisterInterfaces( tDWORD* result );
	tERROR pr_call UnregisterInterfaces();
	tERROR pr_call CheckInterfaces();
	tERROR pr_call UnloadUnused( tDWORD p_during );

// Data declaration
	cStrObj         m_name;         // name of the plugin to load
	tPID            m_pid;          // plugin identificator
	hPLUGININSTANCE m_instance;     // plugin memory instance
	tDWORD          m_prague_format; // is plugin converted to prague format
	cSafeCounter    m_usage_count;  // usage count
	tPluginInit     m_plugin_init;  // plugin initialize function
	tBOOL           m_initialized;  // plugin initialized successfully
	tBOOL           m_unloadable;   // unload flag
	cDateTime       m_unused_since; // unused since, has ZERO value if plugin is used
// AVP Prague stamp end

	cERROR          m_load_err;

	const cStrObj& name() const { return m_name; }

	typedef void (__cdecl* tPluginFinish)(void);
	#if defined( ELF_SUPPORT )
		tPluginFinish m_finish_proc;
	#endif

	#if defined (OLD_METADATA_SUPPORT)
		HDATA         m_mod_info;     // description of the plugin in tree form
		tDWORD        m_iface_count;  // interface count
		tDWORD        m_export_count; // count of plugin exports
		tBOOL         m_auto_load;
		tBOOL         m_old_metadata;
	#endif //OLD_METADATA_SUPPORT 

	PluginMetadata  m_metadata;
	tBOOL           m_metadata_loaded;

private:

	cPluginProtector m_prot; // synchro object to protect plugin load/unload

	// private utils
	bool   pr_call integral()    { return cTRUE == ::PrIsIntegralPlugin(m_pid); }
	bool   pr_call unloadable();

	tERROR pr_call UnloadReal();
	tERROR pr_call GetInfo();

#if defined (OLD_METADATA_SUPPORT)
	HDATA  pr_call GetOldMetadataInfo();
	HDATA  pr_call GetIface( int ind );
	HDATA  pr_call GetStaticIface();
	tBOOL  pr_call IsStaticIface( HDATA iface );
	tERROR pr_call GetPluginProp( tDWORD* result, tPROPID prop_id, tPTR buffer, tDWORD count );
	tERROR pr_call GetStaticIfacePropValByID( HDATA iface, tPID prop_id, tPTR buffer, tDWORD count, ULONG* olen );
	tERROR pr_call GetInterfaceProperty_old( tDWORD* result, tDWORD index, tPROPID prop_id, tPTR buffer, tDWORD count );
	tERROR pr_call enum_interfaces_old( tBOOL regiface, tDWORD* p_iface_count );
	tERROR pr_call prop_export_count_old( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call prop_iface_count_old( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call UnregisterInterfaces_old();
	tERROR pr_call RegisterExportFunctions_old( tDWORD* result );
	tERROR pr_call DropInfo_old();
#endif //OLD_METADATA_SUPPORT

	tERROR pr_call DropInfo();
	tERROR pr_call enum_interfaces( tBOOL regiface, tDWORD* iface_count );
	tERROR pr_call RegisterExportFunctions( tDWORD* result );
	tERROR pr_call init_checked();

	static tERROR pr_call load_module( ModuleDATA* d, tDWORD reserved1, void(__cdecl* reserdev2)(void) );
	
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(ModuleDATA)
};
// AVP Prague stamp end


#include <plugin_inc.c>


// ---
ModuleDATA::ModuleDATA() : m_unused_since(cDateTime::zero) {
}




// ---
inline bool pr_call ModuleDATA::unloadable() {

	if ( integral() )
		return false;

	if ( m_usage_count )
		return false;

	//	if ( m_metadata.theAutoStart )
	//		return false;
	//
	//	#if defined(OLD_METADATA_SUPPORT)
	//		if ( m_auto_load )
	//			return false;
	//	#endif

	if ( !m_unloadable || !m_instance || !m_initialized )
		return false;
	return true;
}





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR ModuleDATA::ObjectInit() {
	PR_TRACE_A0( this, "Enter Plugin::ObjectInit method" );

	m_name.init( ::g_root );
	m_unloadable = cTRUE;
	m_prague_format = cNATIVE_PLUGIN_FOUND;

	cERROR err = m_prot.init();

#if defined (OLD_METADATA_SUPPORT)
	m_export_count = static_cast<tDWORD>(-1);
#endif // OLD_METADATA_SUPPORT

	PR_TRACE_A1( this, "Leave Plugin::ObjectInit method, ret %terr", (tERROR)err );
	return err;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR ModuleDATA::ObjectInitDone() {
	tERROR error = errOK;

	PR_TRACE_A0( this, "Enter Plugin::ObjectInitDone method" );

#if defined (_WIN32)
	m_name.tolower();
#endif

	if ( m_name.empty() ) {
		error = errOBJECT_NOT_INITIALIZED;
		PR_TRACE(( this, prtERROR, "ldr\tModule not initialized properly" ));
	}
	else if ( cStrObj::npos == m_name.find_last_of(L"\\/") ) {
		cStrObj tmp = m_name;
		m_name.assign( ::g_plugins_path, cCP );
		m_name.add_path_sect( tmp );
	}

	if ( PR_FAIL(error) )
		;

#if !defined(NOT_VERIFY) && !defined(LATE_CHECK) || defined (__unix__)
	else if ( m_pid == PID_APPLICATION )
		;
#endif

#if !defined(NOT_VERIFY) && !defined(LATE_CHECK)
#if defined(NEW_SIGN_LIB)
	else if ( IID_LOADER == sysGetParent(IID_ANY)->propGetIID() )
		;
#endif
	else if ( PR_FAIL(error=_check_by_mask(m_name.data(),m_name.CP())) )
		;
#endif

	else if ( (m_prague_format=CheckPluginByName(m_name.data(),m_name.CP())) == cUNEXPECTED_ERROR ) {
		if ( m_pid == PID_APPLICATION )
			error = errOK;
		else {
			error = errMODULE_NOT_FOUND;
			PR_TRACE(( this, prtERROR, "ldr\tCannot load plugin (\"%S\")", m_name.data() ));
		}
	}
	else if ( !known_plugin_type(m_prague_format) )
		error = errMODULE_UNKNOWN_FORMAT;
	else {
		error = GetInfo ();
#if defined (OLD_METADATA_SUPPORT)
		if ( PR_FAIL (error) ) {
			if ( m_pid == PID_APPLICATION )
				error = errOK;
			else if ( m_prague_format == cPRAGUE_PLUGIN_FOUND )
				error = ( GetOldMetadataInfo() ) ? errOK : errUNEXPECTED;
			else {
				PR_TRACE(( this, prtDANGER, "ldr\tPlugin \"%S\" has no metadata", m_name.data() ));
				//INT3;
				error = errOK;
			}
		}
#else
		if ( PR_FAIL (error) && (m_pid == PID_APPLICATION) )
			error = errOK;
#endif //OLD_METADATA_SUPPORT
	}

#if defined (OLD_METADATA_SUPPORT)
	if ( (m_pid == PID_ANY) && (PR_FAIL(error) || PR_FAIL(GetInterfaceProperty(0,0,pgPLUGIN_ID,&m_pid,sizeof(m_pid)))) )
		m_pid = PID_NONE;
#endif //OLD_METADATA_SUPPORT

	if ( PR_SUCC(error) )
		PR_TRACE(( this, prtNOTIFY, "ldr\tPlugin %tpid, \"%S\" created", m_pid, m_name.data() ));
	else
		PR_TRACE(( this, prtIMPORTANT, "ldr\tPlugin %tpid, \"%S\" not created %terr!", m_pid, m_name.data(), error ));
	PR_TRACE_A1( this, "Leave Plugin::ObjectInitDone method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
tERROR ModuleDATA::ObjectPreClose() {
	PR_TRACE_A0( this, "Enter Plugin::ObjectPreClose method" );
	PR_TRACE_A0( this, "Leave Plugin::ObjectClose method, ret errOK" );
	return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR ModuleDATA::ObjectClose() {

	tERROR error;
	PR_TRACE_A0( this, "Enter Plugin::ObjectClose method" );

	error = m_prot.lock( *this );
	if ( PR_SUCC(error) ) {
		if ( m_usage_count )
			m_usage_count = 0;
		error = UnloadReal(); // unload will unlock plugin
		DropInfo();
	}
	m_name.clear();

#if defined (OLD_METADATA_SUPPORT)
	DropInfo_old ();
#endif // OLD_METADATA_SUPPORT

	PR_TRACE_A1( this, "Leave Plugin::ObjectClose method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, IsLoaded )
// Interface "Plugin". Method "Get" for property(s):
//  -- IS_LOADED
tERROR ModuleDATA::IsLoaded( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method ObjectClose for property pgIS_LOADED" );

	tBOOL loaded;
	if ( integral() ) {
		error = errOK;
		loaded = cTRUE;
	}
	else {
		error = m_prot.lock( *this );
		if ( PR_SUCC(error) ) {
			loaded = m_instance ? cTRUE : cFALSE;
			m_prot.unlock( *this );
		}
	}
	if ( PR_SUCC(error) )
		error = getValue( buffer, size, out_size, loaded );

	PR_TRACE_A2( this, "Leave *GET* method ObjectClose for property pgIS_LOADED, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// ---
tERROR getValue ( tPTR buffer, tDWORD size, tDWORD* out_size, tCODEPAGE cp, cStringObj& value ) {
	tDWORD aSize = value.memsize (cp);

	if ( out_size )
		*out_size = aSize;

	if ( !buffer ) {
		if ( !size ) 
			return errOK;
		return errPARAMETER_INVALID;
	}

	if ( size < aSize ) 
		return errBUFFER_TOO_SMALL;

	return value.copy ( buffer, aSize, cp );
}


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_name )
// Interface "Plugin". Method "Get" for property(s):
//  -- OBJECT_NAME
//  -- MODULE_NAME
tERROR ModuleDATA::get_name( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* multyproperty method get_name" );
	error = getValue ( buffer, size, out_size, cCP, m_name );
	PR_TRACE_A2( this, "Leave *GET* multyproperty method get_name, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_name )
// Interface "Plugin". Method "Set" for property(s):
//  -- MODULE_NAME
tERROR ModuleDATA::set_name( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method set_name for property pgMODULE_NAME" );

	//*out_size = 0;
	error = m_name.assign ( buffer, cCP, size );
	*out_size = m_name.memsize( cCP );
	PR_TRACE_A2( this, "Leave *SET* method set_name for property pgMODULE_NAME, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, auto_start )
// Interface "Plugin". Method "Get" for property(s):
//  -- AUTO_START
tERROR ModuleDATA::auto_start( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method ObjectClose for property pgAUTO_START" );

	if ( PR_SUCC (  error = GetInfo () ) )
		error = getValue ( buffer, size, out_size, m_metadata.theAutoStart );
#if defined (OLD_METADATA_SUPPORT)
	else
		error = getValue ( buffer, size, out_size, m_auto_load );
#endif // OLD_METADATA_SUPPORT
	PR_TRACE_A2( this, "Leave *GET* method ObjectClose for property pgAUTO_START, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, is_prague )
// Interface "Plugin". Method "Get" for property(s):
//  -- IS_PRAGUE_FORMAT
tERROR ModuleDATA::is_prague( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method ObjectClose for property pgIS_PRAGUE_FORMAT" );

	tBOOL isPrague = ( m_prague_format == cPRAGUE_PLUGIN_FOUND ) ? cTRUE : cFALSE;
	error = getValue ( buffer, size, out_size, isPrague );

	PR_TRACE_A2( this, "Leave *GET* method ObjectClose for property pgIS_PRAGUE_FORMAT, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, prop_iface_count )
// Interface "Plugin". Method "Get" for property(s):
//  -- INTERFACE_COUNT
tERROR ModuleDATA::prop_iface_count( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method ObjectClose for property pgINTERFACE_COUNT" );

	if ( PR_SUCC ( error = GetInfo () ) )
		error = getValue ( buffer, size, out_size, m_metadata.theInterfacesCount );
#if defined (OLD_METADATA_SUPPORT)
	else
		error = prop_iface_count_old (out_size, prop,buffer,size );
#endif // OLD_METADATA_SUPPORT
	PR_TRACE_A2( this, "Leave *GET* method ObjectClose for property pgINTERFACE_COUNT, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, prop_export_count )
// Interface "Plugin". Method "Get" for property(s):
//  -- PLUGIN_EXPORT_COUNT
tERROR ModuleDATA::prop_export_count( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method ObjectClose for property pgPLUGIN_EXPORT_COUNT" );

	if ( PR_SUCC ( error = GetInfo () ) )
		error = getValue ( buffer, size, out_size, m_metadata.theExportsCount );
#if defined (OLD_METADATA_SUPPORT)
	else
		error = prop_export_count_old(out_size,prop,buffer,size );
#endif // OLD_METADATA_SUPPORT
	PR_TRACE_A2( this, "Leave *GET* method ObjectClose for property pgPLUGIN_EXPORT_COUNT, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_integral )
// Interface "Plugin". Method "Get" for property(s):
//  -- IS_INTEGRAL
tERROR ModuleDATA::get_integral( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method get_integral for property pgIS_INTEGRAL" );

	*out_size = sizeof(tBOOL);
	if ( buffer )
		*(tBOOL*)buffer = integral();

	PR_TRACE_A2( this, "Leave *GET* method get_integral for property pgIS_INTEGRAL, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_time )
// Interface "Plugin". Method "Get" for property(s):
//  -- TIME_STAMP
tERROR ModuleDATA::get_time( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method ObjectClose for property pgTIME_STAMP" );

	if ( m_name.empty() )
		error = errOBJECT_BAD_INTERNAL_STATE;

	tDATETIME aBuffer;
	if ( PR_SUCC(error) )
		error = getFileTime ( m_name, (tDT*)&aBuffer );

	if ( PR_SUCC ( error ) )
		error = getValue ( buffer, size, out_size, aBuffer );

	PR_TRACE_A2( this, "Leave *GET* method ObjectClose for property pgTIME_STAMP, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



tERROR getValue ( tPTR aBuffer, tDWORD aSize, tDWORD* anOutSize, StaticProperty& aValue )
{
	if ( !aBuffer && !anOutSize ) 
		return errPARAMETER_INVALID;

	if ( anOutSize )
		*anOutSize = aValue.thePropertySize;

	if ( aSize < aValue.thePropertySize ) 
		return errBUFFER_TOO_SMALL;

	memcpy ( aBuffer, aValue.thePropertyValue, aValue.thePropertySize );
	return errOK;
}

#define PLUGIN_LEVEL(p) ( ((p)==pgPLUGIN_VERSION) || ((p)==pgPLUGIN_ID) || ((p)==pgVENDOR_ID) || ((p)==pgVENDOR_NAME) || ((p)==pgPLUGIN_NAME) )

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetInterfaceProperty )
// Parameters are:
tERROR ModuleDATA::GetInterfaceProperty( tDWORD* result, tDWORD index, tPROPID prop_id, tPTR buffer, tDWORD count ) {
	tERROR error;
	tDWORD ret_val = 0;
	PR_TRACE_A0( this, "Enter Plugin::GetInterfaceProperty method" );

	if(result)
		*result=0;

	error = GetInfo ();
#if defined (OLD_METADATA_SUPPORT)
	if ( PR_FAIL ( error ))
		return GetInterfaceProperty_old(result,index, prop_id, buffer, count );
#endif // OLD_METADATA_SUPPORT

	if ( (index == 0) || PLUGIN_LEVEL(prop_id) ) {
		if ( PR_SUCC ( error ) ) {
			switch( prop_id ) {
			case pgPLUGIN_NAME:
				error = getValue ( buffer, count, result, cCP, m_name );
				break;
			case pgPLUGIN_ID:
				error = getValue ( buffer, count, result, m_pid );
				break;
			case pgAUTO_START:
				error = getValue ( buffer, count, result, m_metadata.theAutoStart );
				break;
			case pgPLUGIN_CODEPAGE :
				error = getValue ( buffer, count, result, m_metadata.theCodePage );
				break;
			case pgPLUGIN_COMMENT  :
			case pgVENDOR_NAME :
			case pgPLUGIN_VERSION  :
			case pgVENDOR_ID       : error =  errPROPERTY_NOT_FOUND; break;
			default : error = propGet( &count, prop_id, buffer, count );
			}
		}
	}
	else {
		if ( index > m_metadata.theInterfacesCount )
			error = errINTERFACE_NO_MORE_ENTRIES;
		else {
			--index;
			switch( prop_id ) {
			case pgINTERFACE_ID       :
				error = getValue ( buffer, count, result, m_metadata.theInterfaces [ index ].theIID );
				break;
			case pgINTERFACE_SUBTYPE  :
				error = getValue ( buffer, count, result, m_metadata.theInterfaces [ index ].theSub );
				break;
			case pgINTERFACE_FLAGS    :
				error = getValue ( buffer, count, result, m_metadata.theInterfaces [ index ].theFlags );
				break;
			case pgINTERFACE_CODEPAGE :
				error = getValue ( buffer, count, result, m_metadata.theInterfaces [ index ].theCodePage );
				break;
			case pgINTERFACE_VERSION :
				error = getValue ( buffer, count, result, m_metadata.theInterfaces [ index ].theVersion );
				break;
			case pgINTERFACE_COMPATIBILITY :
				error = getValue ( buffer, count, result, m_metadata.theInterfaces [ index ].theCompat );
				break;
			default :
				if ( prop_id & pRANGE_GLOBAL ) {
					error = errPROPERTY_NOT_FOUND;
					for ( unsigned int i = 0; i < m_metadata.theInterfaces [ index ].theStaticPropertiesCount; i++ ) {
						if ( m_metadata.theInterfaces [ index ].theStaticProperties [ i ].thePropertyID == prop_id ) {
							error = getValue ( buffer, count, result, m_metadata.theInterfaces [ index ].theStaticProperties [ i ] );
							break;
						}
					}
				}
				else
					error = propGet( &count, prop_id, buffer, count );
			}
		}
	}
	PR_TRACE_A2( this, "Leave Plugin::GetInterfaceProperty method, ret tDWORD = %u, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end




// ---
tERROR pr_call ModuleDATA::load_module( ModuleDATA* d, tDWORD reserved1, void(__cdecl* reserdev2)(void) ) {
	tERROR error = errOK;

	if ( d->m_name.empty() )
		error = errOBJECT_NOT_INITIALIZED;

#if !defined(NOT_VERIFY) && defined(LATE_CHECK)
	else if ( d->m_pid == PID_APPLICATION )
		;
	else {
		cStrBuff name( d->m_name, cCP );
		error = _check_by_mask ( name, cCP );
	}
#endif

	if ( PR_SUCC(error) && !d->integral() ) {
		tPluginInit     init;
		tPluginFinish   finish = 0;
		hPLUGININSTANCE inst;
		tProtector      prot;
#if defined (_WIN32)
		prot = 0;
#endif
		
		if ( d->m_prague_format == cNATIVE_PLUGIN_FOUND ) 
			d->m_prot.get( prot );

		d->m_load_err = error = LoadPlugin( *d, d->m_name.data(), cCP_UNICODE, d->m_prague_format, &inst, &init, &finish, &prot );

#if defined (_WIN32)
		if ( prot )
#else
		if ( d->m_prague_format == cNATIVE_PLUGIN_FOUND ) 
#endif
			d->m_prot.release();

		if ( PR_SUCC(error) && !d->m_instance ) {
			d->m_instance = inst;
			d->m_plugin_init = init;
			#if defined( ELF_SUPPORT )
				d->m_finish_proc  = finish;
			#endif
		}
	}

	return error;
}



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Load )
// Parameters are:
tERROR ModuleDATA::Load() {
	cERROR error;
	tBOOL  notify;

	PR_TRACE_A0( this, "Enter Plugin::Load method" );

	if ( integral() ) {
		PR_TRACE_A0( this, "Leave Plugin::Load method, ret errOK" );
		m_usage_count++;
		return errOK;
	}

	if ( PR_FAIL(m_load_err) ) {
		PR_TRACE(( this, prtERROR, "ldr\tSaved error(%terr) during plugin loading -- \"%S\"", (tERROR)m_load_err, m_name.data() ));
		return m_load_err;
	}

	notify = cFALSE;
	error = m_prot.lock( *this );
	if ( PR_SUCC(error) ) {
		m_unused_since.Init( cDateTime::zero );
		if ( !m_instance )
			error = call_func_on_an_invisible_thread( load_module, this, 0, 0 );
		if ( PR_SUCC(error) ) {
			if ( m_initialized )
				m_usage_count++;
			else {
				notify = cTRUE;
				error = init_checked();
			}
		}
		if ( PR_SUCC(error) || !notify ) {
			if ( error != errSYNCHRONIZATION_TIMEOUT )
				m_prot.unlock( *this );
		}
		else {
			notify = cFALSE;
			UnloadReal();
		}
	}

	if ( notify )
		::g_root->PluginLoadNotification( (hPLUGIN)this );

	PR_TRACE_A1( this, "Leave Plugin::Load method, ret %terr", (tERROR)error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// cSafeCounter version
// AVP Prague stamp begin( External (user called) interface method implementation, Load )
// Parameters are:
//tERROR ModuleDATA::Load() {
//
//	PR_TRACE_A0( this, "Enter Plugin::Load method" );
//
//	if ( integral() ) {
//		PR_TRACE_A0( this, "Leave Plugin::Load method, ret errOK" );
//		return errOK;
//	}
//
//	cERROR error = errOK;
//	tBOOL  notify = cFALSE;
//	if ( !m_instance ) {
//		error = m_prot.lock( *this );
//		if ( PR_SUCC(error) && !m_instance ) {
//			m_unused_since.Init( cDateTime::zero );
//			error = call_func_on_an_invisible_thread( load_module, this, 0, 0 );
//			if ( PR_SUCC(error) ) {
//				if ( m_initialized )
//					m_usage_count++;
//				else {
//					notify = cTRUE;
//					error = init_checked();
//				}
//			}
//			if ( PR_SUCC(error) || !notify ) {
//				if ( error != errSYNCHRONIZATION_TIMEOUT )
//					m_prot.unlock( *this );
//			}
//			else {
//				notify = cFALSE;
//				UnloadReal();
//			}
//		}
//		else if( error != errSYNCHRONIZATION_TIMEOUT ) {
//			m_usage_count++;
//			m_prot.unlock( *this );
//			m_unused_since.Init( cDateTime::zero );
//		}
//	}
//	else {
//		m_usage_count++;
//		m_unused_since.Init( cDateTime::zero );
//	}
//	
//	if ( notify )
//		::g_root->PluginLoadNotification( (hPLUGIN)this );
//
//	PR_TRACE_A1( this, "Leave Plugin::Load method, ret %terr", (tERROR)error );
//	return error;
//}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Unload )
// Parameters are:
tERROR ModuleDATA::Unload( tBOOL p_force ) {
	tERROR error;
	PR_TRACE_A0( this, "Enter Plugin::Unload method" );

	error = m_prot.lock( *this );
	if ( PR_SUCC(error) ) {
#if defined(_PR_PLUGIN_DONT_REAL_UNLOAD_)
		if ( m_usage_count )
			m_usage_count--;
		m_prot.unlock( *this );
#else
		if ( m_usage_count )
			m_usage_count--;
		if ( p_force && unloadable() )
			error = UnloadReal(); // unload will unlock plugin
		else {
			if ( !m_usage_count )
				m_unused_since.Init( cDateTime::current_local );
			m_prot.unlock( *this );
		}
#endif
	}

	PR_TRACE_A1( this, "Leave Plugin::Unload method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// cSafeCounter version
// AVP Prague stamp begin( External (user called) interface method implementation, Unload )
// Parameters are:
//tERROR ModuleDATA::Unload( tBOOL p_force ) {
//	tERROR error;
//	PR_TRACE_A0( this, "Enter Plugin::Unload method" );
//
//	if ( m_usage_count ) {
//		error = m_prot.lock( *this );
//		if ( PR_SUCC(error) ) {
//			bool real_unload = false;
//			if ( m_usage_count ) {
//				m_usage_count--;
//		#if !defined(_PR_PLUGIN_DONT_REAL_UNLOAD_)
//				real_unload = p_force && unloadable();
//				if ( real_unload )
//					error = UnloadReal(); // unload will unlock plugin
//				else if ( !m_usage_count )
//					m_unused_since.Init( cDateTime::current_local );
//		#endif
//			}
//			if ( !real_unload )
//				m_prot.unlock( *this );
//		}
//	}
//
//	PR_TRACE_A1( this, "Leave Plugin::Unload method, ret %terr", error );
//	return error;
//}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegisterInterfaces )
// Parameters are:
tERROR ModuleDATA::RegisterInterfaces( tDWORD* result ) {
	tERROR error;
	PR_TRACE_A0( this, "Enter Plugin::RegisterInterfaces method" );

	error = enum_interfaces( cTRUE, result );
	if ( PR_SUCC(error) )
		RegisterExportFunctions( 0 );

#if defined (OLD_METADATA_SUPPORT)
	DropInfo_old();
#endif // OLD_METADATA_SUPPORT
	PR_TRACE_A1( this, "Leave Plugin::RegisterInterfaces method, %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, UnregisterInterfaces )
// Parameters are:
tERROR ModuleDATA::UnregisterInterfaces() {
	PR_TRACE_A0( this, "Enter Plugin::UnregisterInterfaces method" );

	tERROR error = GetInfo ();

	if ( PR_SUCC ( error ) )
		for( tDWORD i=0 ; i < m_metadata.theInterfacesCount; i++ )
			::g_root->UnregisterIFace ( m_metadata.theInterfaces[i].theIID,
			m_metadata.theInterfaces[i].thePID,
			m_metadata.theInterfaces[i].theSub,
			m_metadata.theInterfaces[i].theVID );
#if defined (OLD_METADATA_SUPPORT)
	else
		error = UnregisterInterfaces_old ();
#endif // OLD_METADATA_SUPPORT
	PR_TRACE_A1( this, "Leave Plugin::UnregisterInterfaces method, ret %terr", error );
	return error;
}



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, CheckInterfaces )
// Parameters are:
tERROR ModuleDATA::CheckInterfaces() {
	tERROR error;
	PR_TRACE_A0( this, "Enter Plugin::CheckInterfaces method" );

	error = enum_interfaces( cFALSE, 0 );

	PR_TRACE_A1( this, "Leave Plugin::CheckInterfaces method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, UnloadUnused )
// Parameters are:
tERROR ModuleDATA::UnloadUnused( tDWORD p_during ) {
	tERROR error;
	PR_TRACE_FUNC_FRAME( "Plugin::UnloadUnused method" );

	error = m_prot.lock( *this );
	if ( PR_FAIL(error) )
		;
	else if ( !unloadable() )
		m_prot.unlock( *this );
	else if ( !p_during )
		error = UnloadReal();
	else {
		cDateTime now( cDateTime::current_local );
		tLONGLONG diff = now.Diff( &m_unused_since );
		if ( diff > p_during )
			error = UnloadReal();
		else
			m_prot.unlock( *this );
	}

	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegisterExportFunctions )
// Result comment
//    number of export function registered
// Parameters are:
tERROR ModuleDATA::RegisterExportFunctions( tDWORD* result ) {
	cBuff<tEXPORT,8> arr( this );
	tEXPORT* exp;

	tERROR error = GetInfo ();
	if ( PR_FAIL ( error ) ) {
#if defined (OLD_METADATA_SUPPORT)
		error = RegisterExportFunctions_old ( result );
#endif // OLD_METADATA_SUPPORT
		return error;
	}
	tDWORD anExportCount = m_metadata.theExportsCount;
	if (result != 0)
		*result = anExportCount;

	if ( !anExportCount )
		return errOK;

	for ( tDWORD i = 0; i < anExportCount; i++) {
		exp = arr.once_more();
		exp->func = 0;
		exp->cls_id = m_pid;
		exp->fnc_id = m_metadata.theExports [ i ];
	}

	exp = arr.once_more();
	exp->func = 0;
	exp->cls_id = 0;
	exp->fnc_id = 0;

	return ::g_root->RegisterExportTable( (tDWORD*)&anExportCount, arr.ptr(), m_pid );
}
// AVP Prague stamp end



extern "C" tCODEPAGE g_cp_system_default;


/*
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Plugin". Static(shared) property table variables
const tCODEPAGE ::g_cp_system_default = cCP_UNICODE; // must be READ_ONLY at runtime
// AVP Prague stamp end



*/
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class regitration,  )
// Interface "Plugin". Register function
tERROR ModuleDATA::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(Plugin_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Plugin_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "interface to load/unload plugins", 33 )
	mSHARED_PROPERTY_PTR( pgDEFAULT_CODEPAGE, ::g_cp_system_default, sizeof(::g_cp_system_default) )
	mSHARED_PROPERTY( pgDEFAULT_DATE_FORMAT, ((tDWORD)(1)) )
	mpLOCAL_PROPERTY_FN( pgIS_LOADED, FN_CUST(IsLoaded), NULL )
	mpLOCAL_PROPERTY_WRITABLE_OI_FN( pgOBJECT_NAME, FN_CUST(get_name), FN_CUST(set_name) )
	mpLOCAL_PROPERTY_REQ_WRITABLE_OI_FN( pgMODULE_NAME, FN_CUST(get_name), FN_CUST(set_name) )
	mpLOCAL_PROPERTY_BUF( pgMODULE_ID, ModuleDATA, m_pid, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_FN( pgAUTO_START, FN_CUST(auto_start), NULL )
	mpLOCAL_PROPERTY_FN( pgIS_PRAGUE_FORMAT, FN_CUST(is_prague), NULL )
	mpLOCAL_PROPERTY_FN( pgINTERFACE_COUNT, FN_CUST(prop_iface_count), NULL )
	mpLOCAL_PROPERTY_FN( pgTIME_STAMP, FN_CUST(get_time), NULL )
	mpLOCAL_PROPERTY_BUF( pgUNLOADABLE, ModuleDATA, m_unloadable, cPROP_BUFFER_READ_WRITE )
	mpLOCAL_PROPERTY_BUF( pgUSAGE_COUNT, ModuleDATA, m_usage_count, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_FN( pgPLUGIN_EXPORT_COUNT, FN_CUST(prop_export_count), NULL )
	mpLOCAL_PROPERTY_BUF( pgUNUSED_SINCE, ModuleDATA, m_unused_since, cPROP_BUFFER_READ )
	mpLOCAL_PROPERTY_FN( pgIS_INTEGRAL, FN_CUST(get_integral), NULL )
	mpLOCAL_PROPERTY_BUF( plMODULE_INSTANCE, ModuleDATA, m_instance, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
	mpLOCAL_PROPERTY_BUF( pgLOAD_ERROR, ModuleDATA, m_load_err, cPROP_BUFFER_READ_WRITE )
	mSHARED_PROPERTY_PTR( pgOBJECT_CODEPAGE, ::g_cp_system_default, sizeof(::g_cp_system_default) )
mpPROPERTY_TABLE_END(Plugin_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(Plugin)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(ObjectClose)
mINTERNAL_TABLE_END(Plugin)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(Plugin)
	mEXTERNAL_METHOD(Plugin, GetInterfaceProperty)
	mEXTERNAL_METHOD(Plugin, Load)
	mEXTERNAL_METHOD(Plugin, Unload)
	mEXTERNAL_METHOD(Plugin, RegisterInterfaces)
	mEXTERNAL_METHOD(Plugin, UnregisterInterfaces)
	mEXTERNAL_METHOD(Plugin, CheckInterfaces)
	mEXTERNAL_METHOD(Plugin, UnloadUnused)
mEXTERNAL_TABLE_END(Plugin)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "Plugin::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_PLUGIN,                             // interface identifier
		PID_WIN32LOADER,                        // plugin identifier
		0x00000000,                             // subtype identifier
		Plugin_VERSION,                         // interface version
		VID_PETROVITCH,                         // interface developer
		&i_Plugin_vtbl,                         // internal(kernel called) function table
		(sizeof(i_Plugin_vtbl)/sizeof(tPTR)),   // internal function table size
		&e_Plugin_vtbl,                         // external function table
		(sizeof(e_Plugin_vtbl)/sizeof(tPTR)),   // external function table size
		Plugin_PropTable,                       // property table
		mPROPERTY_TABLE_SIZE(Plugin_PropTable), // property table size
		sizeof(ModuleDATA)-sizeof(cObjImpl),    // memory size
		IFACE_SYSTEM | IFACE_UNSWAPPABLE        // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"Plugin(IID_PLUGIN) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call Plugin_Register( hROOT root ) { return ModuleDATA::Register(root); }
// AVP Prague stamp end





// ---
tERROR ModuleDATA::UnloadReal() {

	tERROR error = errOK;
	m_usage_count = 0;

	if ( m_plugin_init && !(m_plugin_init)(g_root,PRAGUE_PLUGIN_PRE_UNLOAD,&error) ) {
		PR_TRACE(( this, prtERROR, "ldr\tError during plugin PRAGUE_PLUGIN_PRE_UNLOAD -- \"%S\" - %terr", m_name.data(), error ));
	}

	::g_root->PluginUnloadNotification( (hPLUGIN)this );

	if ( !unloadable() ) {
		m_prot.unlock( *this );
		return errOK;
	}

	PR_TRACE(( this, prtIMPORTANT, "ldr\tPlugin is about unloading \"%S\"", m_name.data() ));

	if ( m_plugin_init ) {
		tLONG reserved;
		if ( !(m_plugin_init)(m_instance,DLL_THREAD_DETACH,&reserved) ) {
			PR_TRACE(( this, prtERROR, "ldr\tError during plugin DLL_THREAD_DETACH -- \"%S\"", m_name.data() ));
		}
		if ( !(m_plugin_init)(g_root,PRAGUE_PLUGIN_UNLOAD,&error) ) {
			PR_TRACE(( this, prtERROR, "ldr\tError during plugin PRAGUE_PLUGIN_UNLOAD -- \"%S\" - %terr", m_name.data(), error ));
		}
		m_plugin_init = 0;
		m_initialized = cFALSE;
	}

	tPTR instance = m_instance;
	m_instance = 0;

	#if defined( ELF_SUPPORT )
		tPluginFinish finish = m_finish_proc;
		m_finish_proc = 0;
	#else
		tPluginFinish finish = 0;
	#endif

	m_prot.unlock( *this );

	tERROR e = call_func_on_an_invisible_thread( UnloadPlugin, instance, m_prague_format, finish );
	if ( PR_SUCC(error) )
		error = e;

	PR_TRACE(( this, prtIMPORTANT, "ldr\tplugin(base:0x%p) unloaded \"%S\"", instance, m_name.data() ));
	return error;
}



// ---
tERROR ModuleDATA::GetInfo() {
#if defined (OLD_METADATA_SUPPORT)  
	if ( m_old_metadata )
		return errUNEXPECTED;
#endif // OLD_METADATA_SUPPORT
	if ( m_metadata_loaded )
		return errOK;

	if ( m_name.empty() ) 
		return errOBJECT_NOT_INITIALIZED;

#if defined(_WIN32)
	if ( ::g_bUnderNT ) {
		cStrBuff fName( m_name, cCP_UNICODE );
		if ( !load ( (tWCHAR*)fName, m_metadata ) )
			return errUNEXPECTED;
	}
	else
#endif
	{
		cStrBuff fName( m_name, cCP_ANSI );
		if ( !load ( (tCHAR*)fName, m_metadata ) )
			return errUNEXPECTED;
	}

	m_metadata_loaded = cTRUE;
	if ( m_pid == PID_ANY )
		m_pid = m_metadata.thePluginID;
	return errOK;
}

// ---
tERROR pr_call ModuleDATA::DropInfo() {
	if ( m_metadata_loaded  )
		m_metadata.clear ();
	m_metadata_loaded = cFALSE;
	return errOK;
}

// ---
tERROR ModuleDATA::enum_interfaces( tBOOL regiface, tDWORD* p_iface_count ) {
	if ( m_instance ) { // interface already loaded so all interfaces already registered
		PR_TRACE(( this, prtNOT_IMPORTANT, "ldr\tInterfaces for plugin \"%S\" already %sed", m_name.data(), regiface ? "registered" : "checked" ));
		return errOK;
	}

	tERROR error = GetInfo ();

	if ( PR_FAIL ( error ) ) {
#if defined (OLD_METADATA_SUPPORT)
		error = enum_interfaces_old(regiface,p_iface_count);
#endif // OLD_METADATA_SUPPORT
		return error;
	}
	if (p_iface_count != 0)
		*p_iface_count = m_metadata.theInterfacesCount;

	for ( tDWORD i = 0; i < m_metadata.theInterfacesCount; i++) {
		tPID     pid = m_metadata.theInterfaces[i].thePID; ;
		tDWORD   sub = m_metadata.theInterfaces[i].theSub;
		tVERSION ver = m_metadata.theInterfaces[i].theVersion;
		tVID     vid = m_metadata.theInterfaces[i].theVID;
		tDWORD   flags = m_metadata.theInterfaces[i].theFlags;
		tIID     compat = m_metadata.theInterfaces[i].theCompat;
		tIID     iid = m_metadata.theInterfaces[i].theIID;

		if ( ( iid == IID_ANY) || ( iid == IID_NONE ) ) { // interface type
			PR_TRACE(( this, prtERROR, "ldr\tBad iface iid in metadata - (plugin = \"%S\", iface number = %u)", m_name.data(), i-1 ));
			return errINTERFACE_INCOMPATIBLE;
		}

		if ( regiface ) 
			error =::g_root->RegisterIFaceEx(0,iid,pid,sub,ver,vid,0,0,0,0,0,0,0,flags,compat,(hPLUGIN)this, i + 1);
		else 
			error=::g_root->CheckIFace(iid,pid,sub,ver,vid,flags,(hPLUGIN)this);

		if ( PR_FAIL ( error ) )
			return error;
	}

	return errOK;
}


// ---
tERROR pr_call ModuleDATA::init_checked() {

	cERROR error;
	if ( !m_plugin_init )
		m_plugin_init = GetStartAddress( m_instance );

	if ( m_plugin_init ) {
		tBOOL   result;
		hPLUGIN loading_before;
		if ( m_prague_format == cPRAGUE_PLUGIN_FOUND ) {
			tLONG reserved = 0;
			if ( !(m_plugin_init)(m_instance,DLL_PROCESS_ATTACH,&reserved) ) {
				PR_TRACE(( this, prtERROR, "ldr\tError during plugin DLL_PROCESS_ATTACH -- \"%S\"", m_name.data() ));
				return m_load_err = errMODULE_CANNOT_BE_LOADED;
			}
			else if ( !(m_plugin_init)(m_instance,DLL_THREAD_ATTACH,&reserved) ) {
				PR_TRACE(( this, prtERROR, "ldr\tError during plugin DLL_THREAD_ATTACH -- \"%S\"", m_name.data() ));
			}
		}

#define plMODULE_LOADING mPROPERTY_MAKE_LOCAL( pTYPE_OBJECT, 0x00002000 )
		loading_before = (hPLUGIN)::g_root->propGetObj( plMODULE_LOADING );
		::g_root->propSetObj( plMODULE_LOADING, *this );
		result = (m_plugin_init)( ::g_root, PRAGUE_PLUGIN_LOAD, error );
		::g_root->propSetObj( plMODULE_LOADING, (cObj*)loading_before );
#undef plMODULE_LOADING

		if ( result ) {
			//m_initialized = cTRUE;
			PR_TRACE(( this, prtNOTIFY, "ldr\tInitialized properly -- \"%S\"", m_name.data() ));
		}
		else {
			PR_TRACE(( this, prtERROR, "ldr\tError during plugin initialization -- \"%S\" - %terr", m_name.data(), (tERROR)error ));
			if ( PR_SUCC(error) )
				error = errMODULE_CANNOT_BE_INITIALIZED;
			m_load_err = error;
		}
	}

	else {
		//m_initialized = cTRUE;
		error = errOK; //errMODULE_HAS_NO_INIT_ENTRY;
		// ??? нужен он кому-нибудь без инициализации ???
		// UnloadPlugin( m_instance );
		// m_instance = 0;
		PR_TRACE(( this, prtERROR, "ldr\tPlugin \"%S\" has no entry point", m_name.data() ));
	}

	if ( PR_SUCC(error) ) {
		m_initialized = cTRUE;
		m_usage_count++;
	}

	if ( PR_FAIL(error) ) {
		PR_TRACE(( this, prtERROR, "ldr\tPlugin \"%S\" cannot be initialized properly (%terr)!", m_name.data(), (tERROR)error ));
	}
	return error;
}

#if defined (OLD_METADATA_SUPPORT)
tERROR ModuleDATA::GetInterfaceProperty_old( tDWORD* result, tDWORD index, tPROPID prop_id, tPTR buffer, tDWORD count ) {
	tERROR error;
	tDWORD ret_val = 0;
	PR_TRACE_A0( this, "Enter Plugin::GetInterfaceProperty method" );

	error = errOK;
	if ( (index == 0) || PLUGIN_LEVEL(prop_id) )
		error = GetPluginProp( &ret_val, prop_id, buffer, count );
	else {
		HDATA info = GetIface( index-1 );
		if ( !info )
			error = errINTERFACE_NO_MORE_ENTRIES; //errINTERFACE_NOT_FOUND;

		else {
			switch( prop_id ) {
				case pgINTERFACE_ID       :
				case pgINTERFACE_SUBTYPE  :
				case pgINTERFACE_FLAGS    :
				case pgINTERFACE_CODEPAGE :
					if ( buffer && (count < sizeof(AVP_dword)) )
						error = errBUFFER_TOO_SMALL;
					break;
			}

			if ( error == errOK ) {
				tDWORD val;
				switch( prop_id ) {
					case pgINTERFACE_ID       :
						if ( !GetDwordPropVal(info,0,VE_PID_IF_DIGITALID,(AVP_dword*)buffer) )
							error = errPROPERTY_NOT_FOUND;
						break;
					case pgINTERFACE_SUBTYPE  :
						if ( !GetDwordPropVal(info,0,VE_PID_IF_SUBTYPEID,(AVP_dword*)buffer) )
							error = errPROPERTY_NOT_FOUND;
						break;
					case pgINTERFACE_FLAGS    :
						// IFACE_STATIC             0x00000001L // interface is static
						// IFACE_UNSWAPPABLE        0x00000002L // interface is not subject to swap
						// IFACE_PROP_TRANSFER      0x00000004L // unknown properties must be transfered to parent
						// IFACE_PROTECTED_BY_CS    0x00000010L // interface is protected by critical section
						// IFACE_PROTECTED_BY_MUTEX 0x00000040L // interface is protected by mutex
						// not used IFACE_OWN_CP    0x00000080L // interface has own pgOBJECT_CODEPAGE implementation
						// not used IFACE_LOADED    0x00040000L // interface is loaded, registerred and ready
						// not used IFACE_CONDEMNED 0x00020000L // interface is condemned to unload
						*((tDWORD*)buffer) = 0;
						//if ( GetBoolPropVal(info,0,VE_PID_IF_STATIC) )
						//  *((tDWORD*)buffer) |= IFACE_STATIC;
						if ( GetBoolPropVal(info,0,VE_PID_IF_SYSTEM) )
							*((tDWORD*)buffer) |= IFACE_SYSTEM;
						if ( GetBoolPropVal(info,0,VE_PID_IF_TRANSFERPROPUP) )
							*((tDWORD*)buffer) |= IFACE_PROP_TRANSFER;
						if ( GetBoolPropVal(info,0,VE_PID_IF_NONSWAPABLE) )
							*((tDWORD*)buffer) |= IFACE_UNSWAPPABLE;
						if ( GetDwordPropVal(info,0,VE_PID_IF_PROTECTED_BY,(ULONG*)&val) ) {
							switch( val ) {
					case VE_IFF_PROTECTED_BY_CS    : *((tDWORD*)buffer) |= IFACE_PROTECTED_BY_CS;    break;
					case VE_IFF_PROTECTED_BY_MUTEX : *((tDWORD*)buffer) |= IFACE_PROTECTED_BY_MUTEX; break;
					case VE_IFF_PROTECTED_BY_NONE  :
					default                        : break;
							}
						}
						break;
					default :
						error = GetStaticIfacePropValByID( info, prop_id, buffer, count, (ULONG*)&count );
						break;
				}
			}
		}
	}

	if ( result )
		*result = PR_SUCC(error) ? count : 0;

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( this, "Leave Plugin::GetInterfaceProperty method, ret tDWORD = %u, %terr", ret_val, error );
	return error;
}

HDATA ModuleDATA::GetOldMetadataInfo() {

	if ( !m_mod_info && !m_name.empty() ) {

		m_mod_info = (HDATA)1;

		cERROR err;
		switch ( m_prague_format ) {
			case cPRAGUE_PLUGIN_FOUND:
				err = ::GetPrMetaData( m_name.data(), cCP_UNICODE, &m_mod_info );
				break;
			case cNATIVE_PLUGIN_FOUND:
				err = ::GetDllMetaData( m_name.data(), cCP_UNICODE, &m_mod_info );
				break;
			default:
				err = errOBJECT_INCOMPATIBLE;
		}
		if ( PR_SUCC(err) )
			m_auto_load = GetBoolPropVal( m_mod_info, 0, VE_PID_PL_AUTOSTART );
		else 
			PR_TRACE(( this, prtERROR, "ldr\tMetadata for module \"%S\" not found !", m_name.data() ));
	}
	if ( !m_mod_info || (m_mod_info == (HDATA)1) ) {
		PR_TRACE(( this, prtERROR, "ldr\tPlugin \"%S\" doesn't have metadata (resource section ?)", m_name.data() ));
		return 0;
	}
	else {
		m_old_metadata = cTRUE;
		return m_mod_info;
	}
}

// ---
tERROR pr_call ModuleDATA::DropInfo_old() {
	if ( m_mod_info && (m_mod_info != (HDATA)1) ) {
		::DATA_Remove( m_mod_info, 0 );
		m_mod_info = 0;
	}
	return errOK;
}

tERROR ModuleDATA::RegisterExportFunctions_old( tDWORD* result ) {
	tERROR error;
	tDWORD   exp_count = 0;
	PR_TRACE_A0( this, "Enter Plugin::RegisterExportFunctions method" );

	error = errOK;
	exp_count = 0;
	HDATA iface_data = GetStaticIface();
	if ( iface_data ) {
		tEXPORT* exp;
		tUINT  mcount = 0;
		CFaceInfo iface( iface_data, false );
		CPubMethodInfo method( iface );
		cBuff<tEXPORT,8> arr( this );

		while( method ) {
			exp = arr.once_more();
			exp->func = 0;
			exp->cls_id = m_pid;
			exp->fnc_id = method.MethodID();
			mcount++;
			method.GoNext();
		}

		if ( mcount ) {
			exp = arr.once_more();
			exp->func = 0;
			exp->cls_id = 0;
			exp->fnc_id = 0;
			error = ::g_root->RegisterExportTable( &exp_count, arr.ptr(), m_pid/*(tPID)this*/ );
		}
		if ( m_export_count == static_cast<tDWORD>(-1) )
			m_export_count = mcount;
	}

	if ( result )
		*result = exp_count;
	PR_TRACE_A2( this, "Leave Plugin::RegisterExportFunctions method, ret tDWORD = %u, %terr", exp_count, error );
	return error;
}


tERROR ModuleDATA::UnregisterInterfaces_old() {
	tERROR error;
	tDWORD size;
	tDWORD icount;
	tDWORD i;
	PR_TRACE_A0( this, "Enter Plugin::UnregisterInterfaces method" );

	error = errOK;

	prop_iface_count( &size, pgINTERFACE_COUNT, (tCHAR*)&icount, sizeof(tDWORD) );
	for( i=1; i<=icount; i++ ) { // go through all interfaces in the plugin

		tIID iid;

		error = GetInterfaceProperty( 0, i, pgINTERFACE_ID, &iid, sizeof(iid) ); // index zero is for the plugin properties
		/*
		if ( error == errINTERFACE_NO_MORE_ENTRIES ) { // got to last iface ?
		error = errOK;
		PR_TRACE(( _this, prtERROR, "ldr\tError reading iface description (plugin = \"%S\", iface item number - %u, %terr)", m_name.data(), i-1, error ));
		break;
		}
		else */ if ( PR_FAIL(error) ) { // error == errMODULE_NOT_VERIFIED
			PR_TRACE(( this, prtERROR, "ldr\tError reading iface metadata (plugin = \"%S\", iface item number - %u, %terr)", m_name.data(), i-1, error ));
		}

		else if ( (iid == IID_ANY) || (iid == IID_NONE) ) { // interface type
			PR_TRACE(( this, prtERROR, "ldr\tBad iface iid in metadata - (plugin = \"%S\", iface number = %u)", m_name.data(), i-1 ));
			error = errINTERFACE_INCOMPATIBLE;
		}

		else { // collect iface properties for interface table
			tPID     pid;
			tDWORD   sub;
			tVID     vid;

			GetInterfaceProperty( 0, i, pgPLUGIN_ID,         &pid, sizeof(pid) ); // plugin ID
			GetInterfaceProperty( 0, i, pgINTERFACE_SUBTYPE, &sub, sizeof(sub) ); // subtype of interface in plugin
			GetInterfaceProperty( 0, i, pgVENDOR_ID,         &vid, sizeof(vid) ); // vendor id

			if ( (pid == 0) || (vid == 0) )
				PR_TRACE(( this, prtERROR, "ldr\tBad iface description - (pid==0||vid==0)(plugin = \"%S\", iface number = %u, %tpid, %tvid", m_name.data(), i-1, pid, vid ) );

			else {
				if ( PR_FAIL(error=::g_root->UnregisterIFace(iid,pid,sub,vid)) ) { // are you happy ?
					PR_TRACE(( this, prtERROR, "ldr\tCannot unregister iface(%terr) - (plugin = \"%S\", iface number = %u, %tiid, %tpid, %tvid", error, m_name.data(), i-1, iid, pid, vid ) );
				}
			}
		}
	}

	PR_TRACE_A1( this, "Leave Plugin::UnregisterInterfaces method, ret %terr", error );
	return error;
}

tERROR ModuleDATA::prop_iface_count_old( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	HDATA info;
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method ObjectClose for property pgINTERFACE_COUNT" );

	if ( buffer ) {
		if ( !m_iface_count && (0 != (info=GetOldMetadataInfo())) ) {
			info = ::DATA_Get_FirstEx( info, 0, DATA_IF_ROOT_CHILDREN );
			for( ; info; info=::DATA_Get_Next(info,0) ) {
				if ( !IsStaticIface(info) )
					m_iface_count++;
			}
		}
		*(tDWORD*)buffer = m_iface_count;
	}
	*out_size = sizeof(tDWORD);

	PR_TRACE_A2( this, "Leave *GET* method ObjectClose for property pgINTERFACE_COUNT, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}

tERROR ModuleDATA::prop_export_count_old( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method ObjectClose for property pgPLUGIN_EXPORT_COUNT" );

	*out_size = sizeof(tDWORD);
	if ( buffer ) {
		if ( m_export_count != static_cast<tDWORD>(-1) )
			*(tDWORD*)buffer = m_export_count;
		else if ( !GetOldMetadataInfo() ) {
			*(tDWORD*)buffer = 0;
			error = errPROPERTY_NOT_FOUND;
		}
		else {
			m_export_count = 0;
			HDATA data = GetStaticIface();
			if( data ) {
				CFaceInfo iface( data, false );
				CPubMethodInfo method( iface );
				while( method ) {
					m_export_count++;
					method.GoNext();
				}
			}
			*(tDWORD*)buffer = m_export_count;
		}
	}

	PR_TRACE_A2( this, "Leave *GET* method ObjectClose for property pgPLUGIN_EXPORT_COUNT, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}


// ---
tERROR ModuleDATA::GetStaticIfacePropValByID( HDATA iface, tPID prop_id, tPTR buffer, tDWORD count, ULONG* olen ) {

	ULONG     addr[] = { VE_PID_IF_PROPERTIES, 0 };
	HDATA     prop_data = ::DATA_Get_FirstEx( iface, addr, DATA_IF_ROOT_CHILDREN );
	AVP_dword prop_type = (prop_id & pTYPE_MASK) >> 20;

	prop_id &= ~( pRANGE_MASK | pTYPE_MASK );

	while( prop_data ) {

		AVP_dword val;

		if (
			GetDwordPropVal(prop_data,0,VE_PID_IFP_SCOPE,&val)     && (val == VE_IFP_SHARED) &&
			GetDwordPropVal(prop_data,0,VE_PID_IFP_TYPE,&val)      && (val == prop_type) &&
			GetDwordPropVal(prop_data,0,VE_PID_IFP_DIGITALID,&val) && (val == prop_id)
			) {

				AVP_dword id;
				HPROP     prop;
				tDWORD    size;

				switch( prop_type ) {

				case tid_CHAR    :
				case tid_BYTE    :
				case tid_SBYTE   :
					id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_byte );
					size = sizeof( AVP_byte );
					break;

				case tid_WORD    :
				case tid_WCHAR   :
				case tid_SHORT   :
					id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_word );
					size = sizeof( AVP_word );
					break;

				case tid_LONGLONG:
				case tid_QWORD   :
					id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_qword );
					size = sizeof( AVP_qword );
					break;

				case tid_STRING  :
					id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_str );
					size = 0;
					break;

				case tid_WSTRING :
					id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_wstr );
					size = 0;
					break;

				case tid_BINARY :
					id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_bin );
					size = 0;
					break;

				default :
					id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_dword );
					size = sizeof( AVP_dword );;
					break;

				}

				prop = DATA_Find_Prop( prop_data, 0, id );
				if ( !prop )
					return errPROPERTY_NOT_FOUND;

				if ( !size )
					size = PROP_Get_Val( prop, 0, 0 );

				if ( !olen )
					olen = &val;

				switch( prop_type ) {

			case tid_CHAR    :
			case tid_SBYTE   :
			case tid_BYTE    :
			case tid_SHORT   :
			case tid_WORD    :
			case tid_WCHAR   :
			case tid_INT     :
			case tid_LONG    :
			case tid_BOOL    :
			case tid_DWORD   :
			case tid_CODEPAGE:
			case tid_ERROR   :
			case tid_UINT    :
			case tid_IID     :
			case tid_PID     :
			case tid_ORIG_ID :
			case tid_OS_ID   :
			case tid_VID     :
			case tid_PROPID  :
			case tid_VERSION :
			case tid_DATA    :
			case tid_LONGLONG :
			case tid_QWORD    :
			case tid_DATETIME :
				*olen = size;
				if ( buffer ) {
					if ( count < size ) {
						*olen = 0;
						return errBUFFER_TOO_SMALL;
					}
					if ( !PROP_Get_Val(prop,buffer,size) )
						return errUNEXPECTED;
				}
				return errOK;

			case tid_STRING  :
			case tid_WSTRING :
			case tid_BINARY  :
				if ( buffer ) {
					if ( count > size )
						count = size;
					if ( !(*olen = PROP_Get_Val(prop,buffer,count)) )
						return errUNEXPECTED;
				}
				else
					*olen = size;
				return errOK;

			case tid_IFACEPTR :
			case tid_PTR      :
			case tid_OBJECT   :
				*olen = 0;
				return errINTERFACE_NOT_LOADED;

			default           :
				*olen = 0;
				return errNOT_IMPLEMENTED;
				}
		}

		prop_data = DATA_Get_Next( prop_data, 0 );
	}

	return errPROPERTY_NOT_FOUND;
}

tERROR ModuleDATA::enum_interfaces_old( tBOOL regiface, tDWORD* p_iface_count ) {

	tERROR error;
	tDWORD size;
	tDWORD icount;
	tDWORD i;

	if ( m_instance ) { // interface already loaded so all interfaces already registered
		PR_TRACE(( this, prtNOT_IMPORTANT, "ldr\tInterfaces for plugin \"%S\" already %sed", m_name.data(), regiface ? "registered" : "checked" ));
		return errOK;
	}

	error = errOK;

	if ( p_iface_count )
		*p_iface_count = 0;

	prop_iface_count( &size, pgINTERFACE_COUNT, (tCHAR*)&icount, sizeof(tDWORD) );

	for( i=1; i<=icount; i++ ) { // go through all interfaces in the plugin

		tIID iid;

		error = GetInterfaceProperty( 0, i, pgINTERFACE_ID, &iid, sizeof(iid) ); // index zero is for the plugin properties
		/*
		if ( error == errINTERFACE_NO_MORE_ENTRIES ) { // got to last iface ?
		error = errOK;
		PR_TRACE(( this, prtERROR, "ldr\tError reading iface description (plugin = \"%S\", iface item number - %u, %terr)", m_name.data(), i-1, error ));
		break;
		}
		else */ if ( PR_FAIL(error) ) { // error == errMODULE_NOT_VERIFIED
			PR_TRACE(( this, prtERROR, "ldr\tError reading iface description (plugin = \"%S\", iface item number - %u, %terr)", m_name.data(), i-1, error ));
		}

		else if ( (iid == IID_ANY) || (iid == IID_NONE) ) { // interface type
			PR_TRACE(( this, prtERROR, "ldr\tBad iface iid in metadata - (plugin = \"%S\", iface number = %u)", m_name.data(), i-1 ));
			error = errINTERFACE_INCOMPATIBLE;
		}

		else { // collect iface properties for interface table
			tPID     pid;
			tDWORD   sub;
			tVERSION ver;
			tVID     vid;
			tDWORD   flags;
			tIID     compat;
			tCHAR    comment[30];

#ifdef _DEBUG
			if ( PR_SUCC(error=GetInterfaceProperty(0,i,pgINTERFACE_COMMENT,&comment,sizeof(comment))) ) // interface comment
				;
			else
#endif
				*comment = 0;

			if ( PR_FAIL(error=GetInterfaceProperty(0,i,pgPLUGIN_ID,&pid,sizeof(pid))) ) // plugin ID
				PR_TRACE(( this, prtERROR, "ldr\t(%terr): error in metadata: cannot get pid (plugin = \"%S\"(\"%s\"), iface number = %u, %tiid", error, m_name.data(), comment, i-1, iid ) );

			else {

				if ( PR_FAIL(error=GetInterfaceProperty(0,i,pgINTERFACE_SUBTYPE,&sub,sizeof(sub))) ) {// subtype of interface in plugin
					sub = 0;
					PR_TRACE(( this, prtERROR, "ldr\t(%terr): error in metadata: cannot get subtype (plugin = \"%S\"(\"%s\"), iface number = %u, %tiid, %tpid", error, m_name.data(), comment, i-1, iid, pid ) );
				}

				if ( PR_FAIL(error=GetInterfaceProperty(0,i,pgINTERFACE_VERSION,&ver,sizeof(ver))) ) { // version
					ver = 1;
					PR_TRACE(( this, prtIMPORTANT, "ldr\t(%terr): error in metadata: cannot get iface VERSION from metadata (plugin = \"%S\"(\"%s\"), iface number = %u, %tiid, %tpid", error, m_name.data(), comment, i-1, iid, pid ) );
				}

				if ( PR_FAIL(error=GetInterfaceProperty(0,i,pgVENDOR_ID,&vid,sizeof(vid))) ) { // vendor id
					vid = VID_KASPERSKY_LAB;
					PR_TRACE(( this, prtNOTIFY, "ldr\t(%terr): error in metadata: cannot get VENDOR ID from metadata (plugin = \"%S\"(\"%s\"), iface number = %u, %tiid, %tpid", error, m_name.data(), comment, i-1, iid, pid ) );
				}

				if ( PR_FAIL(error=GetInterfaceProperty(0,i,pgINTERFACE_FLAGS,&flags,sizeof(flags))) ) { // interface flags
					flags = 0;
					PR_TRACE(( this, prtNOTIFY, "ldr\t(%terr): error in metadata: cannot get iface FLAGS from metadata (plugin = \"%S\"(\"%s\"), iface number = %u, %tiid, %tpid", error, m_name.data(), comment, i-1, iid, pid ) );
				}

				if ( PR_FAIL(error=GetInterfaceProperty(0,i,pgINTERFACE_COMPATIBILITY,&compat,sizeof(compat))) ) { // interface compatibility
					compat = PID_ANY;
					PR_TRACE(( this, prtNOTIFY, "ldr\t(%terr): cannot get iface COMPATIBILITY id from metadata (plugin = \"%S\"(\"%s\"), iface number = %u, %tiid, %tpid", error, m_name.data(), comment, i-1, iid, pid ) );
				}

				if ( regiface ) {
					if ( PR_SUCC(error=::g_root->RegisterIFaceEx(0,iid,pid,sub,ver,vid,0,0,0,0,0,0,0,flags,compat,(hPLUGIN)this,i)) ) { // are you happy ?
						if ( p_iface_count )
							(*p_iface_count)++;
					}
					else {
						PR_TRACE(( this, prtERROR, "ldr\t(%terr): cannot register iface - (plugin = \"%S\"(\"%s\"), iface number = %u, %tiid, %tpid, %tvid", error, m_name.data(), comment, i-1, iid, pid, vid ) );
					}
				}
				else if ( PR_SUCC(error=::g_root->CheckIFace(iid,pid,sub,ver,vid,flags,(hPLUGIN)this)) ) { // are you happy ?
					if ( p_iface_count )
						(*p_iface_count)++;
				}
				else {
					PR_TRACE(( this, prtERROR, "ldr\t(%terr): cannot check iface - (plugin = \"%S\"(\"%s\"), iface number = %u, %tiid, %tpid, %tvid", error, m_name.data(), comment, i-1, iid, pid, vid ) );
				}
			}
		}
	} // for( i=1; i<=icount; i++ )  // go through all interfaces in the plugin

	return error;
}




// ---
HDATA ModuleDATA::GetIface( int ind ) {
	int i = 0;
	HDATA info = GetOldMetadataInfo();
	if ( !info )
		return 0;

	info = ::DATA_Get_FirstEx( info, 0, DATA_IF_ROOT_CHILDREN );
	while( info ) {
		if ( IsStaticIface(info) )
			;
		else if ( i == ind )
			return info;
		else
			i++;
		info = ::DATA_Get_Next( info, 0 );
	}
	return 0;
}




// ---
HDATA ModuleDATA::GetStaticIface() {
	HDATA info = GetOldMetadataInfo();
	if ( !info )
		return 0;

	info = DATA_Get_FirstEx( info, 0, DATA_IF_ROOT_CHILDREN );
	while( info ) {
		if ( IsStaticIface(info) )
			return info;
		info = DATA_Get_Next( info, 0 );
	}
	return 0;
}



// ---
tBOOL ModuleDATA::IsStaticIface( HDATA iface ) {
	if ( !::DATA_Find_Prop(iface,0,VE_PID_IF_PLUGIN_STATIC) )
		return cFALSE;
	if ( ::DATA_Find_Prop(iface,0,VE_PID_IF_DIGITALID) )
		return cFALSE;
	return cTRUE;
}



// ---
tERROR ModuleDATA::GetPluginProp( tDWORD* result, tPROPID prop_id, tPTR buffer, tDWORD count ) {

	AVP_dword pid;

	if ( (pTYPE_MASK & prop_id) == pTYPE_STRING ) {
		switch( prop_id ) {
			case pgPLUGIN_COMMENT  : 
				pid = VE_PID_SHORTCOMMENT; break;
			case pgVENDOR_NAME : 
				pid = VE_PID_PL_AUTHORNAME; break; // ??? VE_PID_PL_VENDORNAME;
			case pgPLUGIN_NAME     :
				if ( buffer && count ) {
					*result = m_name.length() + sizeof(tCHAR);
					if ( count >= *result )
						return m_name.copy( (tCHAR*)buffer, count );
					return errBUFFER_TOO_SMALL;
				}
				if ( !buffer && !count ) {
					*result = m_name.length() + sizeof(tCHAR);
					return errOK;
				}
				*result = 0;
				return errPARAMETER_INVALID;

			default : 
				return propGet( result, prop_id, buffer, count ); //goto exit; // this->sys->PropertyGet( (hOBJECT)this, prop_id, buffer, count );
		}
		if ( !GetOldMetadataInfo() || !GetStrPropVal(m_mod_info,0,pid,(char*)buffer,count,(ULONG*)result) )
			return errPROPERTY_NOT_FOUND;
		else
			return errOK;
	}
	switch( prop_id ) {
		case pgPLUGIN_ID      :
			if ( !m_pid || (PID_NONE == m_pid)) {
				pid = VE_PID_PL_DIGITALID;
				break;
			}
			if ( !buffer ) {
				*result = sizeof(tPID);
				return errOK;
			}
			if ( count < sizeof(tPID) ) {
				*result = sizeof(tPID);
				return errPARAMETER_INVALID;
			}
			*(tPID*)buffer = m_pid;
			*result = sizeof(tPID);
			return errOK;

		case pgPLUGIN_CODEPAGE : pid = VE_PID_PL_CODEPAGEID; break;
		case pgPLUGIN_VERSION  : pid = VE_PID_PL_VERSIONID;  break;
		case pgVENDOR_ID       : pid = VE_PID_PL_VENDORID;   break;
		case pgAUTO_START      : pid = VE_PID_PL_AUTOSTART;  break;
		default                : return propGet( &count, prop_id, buffer, count );
	}
	if ( !buffer ) {
		if ( GET_AVP_PID_TYPE(pid) == avpt_dword ) {
			*result = sizeof(tDWORD);
			return errOK;
		}
		if ( GET_AVP_PID_TYPE(pid) == avpt_bool ) {
			*result = sizeof(tBOOL);
			return errOK;
		}
		*result = 0;
		return errPROPERTY_NOT_FOUND;
	}
	if ( GET_AVP_PID_TYPE(pid) == avpt_dword ) {
		if ( count < sizeof(tDWORD) ) {
			*result = sizeof(tDWORD);
			return errBUFFER_TOO_SMALL;
		}
		if ( !GetOldMetadataInfo() || !GetDwordPropVal(m_mod_info,0,pid,(ULONG*)buffer) ) {
			*result = 0;
			return errPROPERTY_NOT_FOUND;
		}

		if ( prop_id == pgPLUGIN_ID )
			m_pid = *(tPID*)buffer;
		else if ( (prop_id == pgPLUGIN_CODEPAGE) && (*(tCODEPAGE*)buffer == 0) )
			*(tCODEPAGE*)buffer = cCP_ANSI;
		*result = sizeof(tDWORD);
		return errOK;
	}

	if ( GET_AVP_PID_TYPE(pid) == avpt_bool ) {
		*result = sizeof(tBOOL);
		if ( count < sizeof(tBOOL) )
			return errBUFFER_TOO_SMALL;
		if ( GetOldMetadataInfo() )
			*(tBOOL*)buffer = (AVP_bool)GetBoolPropVal( m_mod_info, 0, pid );
		else if ( prop_id == pgAUTO_START )
			*(tBOOL*)buffer = cTRUE; // if we have no metadata -- just mark plugin autostarted
		else
			*(tBOOL*)buffer = cFALSE;
		return errOK;
	}
	return errPROPERTY_NOT_FOUND;
}

#endif // OLD_METADATA_SUPPORT


