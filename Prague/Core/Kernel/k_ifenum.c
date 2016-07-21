// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Friday,  07 July 2006,  17:53 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Kernel interfaces
// Sub project -- Not defined
// Purpose     -- Kernel interfaces
// Author      -- petrovitch
// File Name   -- k_ifenum.c
// Additional info
//    Kernel interface implementations
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define IFaceEnum_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include "kernel.h"
#include "k_ifenum.h"
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_iface.h>
#include <Prague/iface/i_plugin.h>
#include <Prague/iface/i_root.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// IFaceEnum interface implementation
// Short comments -- registered interface enumerator interface
// Extended comment -
//  Supplies facility to enumerate registered interfaces
//  Also possible to get shared properties of registered iterface
// AVP Prague stamp end



// ---
typedef struct tag_IfaceId {
	tIID iid;
	tPID pid;
} tIFaceId;

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface IFaceEnum. Inner data structure

typedef struct tag_IFENUM_DATA {
  tIID        iid;       // interface id to find
  tPID        pid;       // plugin id to find
  tVID        vid;       // vendor id to find
  hOBJECT     object;    // object to recognize/create from
  tDWORD      type;      // type to pass to Recognize method
  tINTERFACE* iface;     // current interface
	tUINT       iface_load_count; // count iface load counter
	tUINT       include_index; // index of the inclue array
  tUINT       saved_pos; // interface table cookie
	tIFaceId*   includes;  // include array
	tUINT       include_count;
	tIFaceId*   excludes;  // exclude array
	tUINT       exclude_count;
} IFENUM_DATA;

// Interface "IFaceEnum". Inner data structure
// AVP Prague stamp end


// ---
static tERROR pr_call _iface_load( IFENUM_DATA* d, tPO* po );
static tERROR pr_call _iface_unload( IFENUM_DATA* d, tPO* po );


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_IFaceEnum {
	const iIFaceEnumVtbl* vtbl; // pointer to the "IFaceEnum" virtual table
	const iSYSTEMVTBL*    sys;  // pointer to the "SYSTEM" virtual table
	IFENUM_DATA*          data; // pointer to the "IFaceEnum" data structure
} *hi_IFaceEnum;

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call IFaceEnum_ObjectClose( hi_IFaceEnum _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_IFaceEnum_vtbl = {
	(tIntFnRecognize)         NULL,
	(tIntFnObjectNew)         NULL,
	(tIntFnObjectInit)        NULL,
	(tIntFnObjectInitDone)    NULL,
	(tIntFnObjectCheck)       NULL,
	(tIntFnObjectPreClose)    NULL,
	(tIntFnObjectClose)       IFaceEnum_ObjectClose,
	(tIntFnChildNew)          NULL,
	(tIntFnChildInitDone)     NULL,
	(tIntFnChildClose)        NULL,
	(tIntFnMsgReceive)        NULL,
	(tIntFnIFaceTransfer)     NULL
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpIFaceEnum_Init)         ( hi_IFaceEnum _this, tIID iid, tPID pid, tVID vid, hOBJECT object ); // -- Initialization procedure;
typedef   tERROR (pr_call *fnpIFaceEnum_Clone)        ( hi_IFaceEnum _this, hIFACE_ENUM* result, hOBJECT dad );     // -- Clone current enumerator;
typedef   tERROR (pr_call *fnpIFaceEnum_Reset)        ( hi_IFaceEnum _this );                  // -- Resets current position of enumerator;
typedef   tERROR (pr_call *fnpIFaceEnum_Next)         ( hi_IFaceEnum _this );                  // -- Advance current pointer;
typedef   tERROR (pr_call *fnpIFaceEnum_GetIFaceProp) ( hi_IFaceEnum _this, tDWORD* result, tPROPID prop_id, tPTR buffer, tDWORD size ); // -- Get static properties of current interface;
typedef   tERROR (pr_call *fnpIFaceEnum_ObjectCreate) ( hi_IFaceEnum _this, hOBJECT* result, hOBJECT parent );  // -- Create instance of current interface;
typedef   tERROR (pr_call *fnpIFaceEnum_Recognize)    ( hi_IFaceEnum _this, hOBJECT object, tDWORD type ); // -- regonize the "object";
typedef   tERROR (pr_call *fnpIFaceEnum_Load)         ( hi_IFaceEnum _this );                  // -- load the interface and increment objects count;
typedef   tERROR (pr_call *fnpIFaceEnum_Free)         ( hi_IFaceEnum _this );                  // -- decrements objects count and unload the interface if possible;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iIFaceEnumVtbl {
	fnpIFaceEnum_Init          Init;
	fnpIFaceEnum_Clone         Clone;
	fnpIFaceEnum_Reset         Reset;
	fnpIFaceEnum_Next          Next;
	fnpIFaceEnum_GetIFaceProp  GetIFaceProp;
	fnpIFaceEnum_ObjectCreate  ObjectCreate;
	fnpIFaceEnum_Recognize     Recognize;
	fnpIFaceEnum_Load          Load;
	fnpIFaceEnum_Free          Free;
}; // "IFaceEnum" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table prototypes,  )
// Interface "IFaceEnum". External method table. Forward declarations
tERROR pr_call IFaceEnum_Init( hi_IFaceEnum _this, tIID iid, tPID pid, tVID vid, hOBJECT object );
tERROR pr_call IFaceEnum_Clone( hi_IFaceEnum _this, hIFACE_ENUM* result, hOBJECT dad );
tERROR pr_call IFaceEnum_Reset( hi_IFaceEnum _this );
tERROR pr_call IFaceEnum_Next( hi_IFaceEnum _this );
tERROR pr_call IFaceEnum_GetIFaceProp( hi_IFaceEnum _this, tDWORD* result, tPROPID prop_id, tPTR buffer, tDWORD size );
tERROR pr_call IFaceEnum_ObjectCreate( hi_IFaceEnum _this, hOBJECT* result, hOBJECT parent );
tERROR pr_call IFaceEnum_Recognize( hi_IFaceEnum _that, hOBJECT object, tDWORD type );
tERROR pr_call IFaceEnum_Load( hi_IFaceEnum _this );
tERROR pr_call IFaceEnum_Free( hi_IFaceEnum _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
static iIFaceEnumVtbl e_IFaceEnum_vtbl = {
	(fnpIFaceEnum_Init)       IFaceEnum_Init,
	(fnpIFaceEnum_Clone)      IFaceEnum_Clone,
	(fnpIFaceEnum_Reset)      IFaceEnum_Reset,
	(fnpIFaceEnum_Next)       IFaceEnum_Next,
	(fnpIFaceEnum_GetIFaceProp) IFaceEnum_GetIFaceProp,
	(fnpIFaceEnum_ObjectCreate) IFaceEnum_ObjectCreate,
	(fnpIFaceEnum_Recognize)  IFaceEnum_Recognize,
	(fnpIFaceEnum_Load)       IFaceEnum_Load,
	(fnpIFaceEnum_Free)       IFaceEnum_Free
};
// AVP Prague stamp end





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call IFaceEnum_PROP_get_plugin( hi_IFaceEnum _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IFaceEnum_PROP_get_index( hi_IFaceEnum _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IFaceEnum_PROP_get_flags( hi_IFaceEnum _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IFaceEnum_PROP_get_load_err( hi_IFaceEnum _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
//! tERROR pr_call IFaceEnum_PROP_get_recognize( hi_IFaceEnum _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IFaceEnum_PROP_get_last( hi_IFaceEnum _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IFaceEnum_PROP_get_include( hi_IFaceEnum _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IFaceEnum_PROP_set_include( hi_IFaceEnum _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IFaceEnum_PROP_get_exclude( hi_IFaceEnum _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call IFaceEnum_PROP_set_exclude( hi_IFaceEnum _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "IFaceEnum". Static(shared) property table variables
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR pr_call IFaceEnum_ObjectClose( hi_IFaceEnum _this ) {

	tERROR error = errOK;
	IFENUM_DATA* d = _this->data;
	tPO* po;
	PR_TRACE_A0( _this, "Enter IFaceEnum::ObjectClose method" );

	enter_lf();
	_iface_unload( d, po );
	leave_lf();

	if ( d->includes ) {
		CALL_SYS_ObjHeapFree( _this, d->includes );
		d->includes = 0;
	}

	if ( d->excludes ) {
		CALL_SYS_ObjHeapFree( _this, d->excludes );
		d->excludes = 0;
	}

	PR_TRACE_A1( _this, "Leave IFaceEnum::ObjectClose method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface version,  )
#define IFaceEnum_VERSION ((tVERSION)1)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(IFaceEnum_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, IFaceEnum_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "registered interface enumerator interface", 42 )
	mLOCAL_PROPERTY_BUF( pgPRESCRIBED_IID, IFENUM_DATA, iid, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgPRESCRIBED_PID, IFENUM_DATA, pid, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgPRESCRIBED_VID, IFENUM_DATA, vid, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_BUF( pgPRESCRIBED_OBJECT, IFENUM_DATA, object, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( pgPRESCRIBED_TYPE, IFENUM_DATA, type, cPROP_BUFFER_READ_WRITE )
	mSHARED_PROPERTY_PTR( pgOBJECT_NAME, "Interface enumerator object", 28 )
	mLOCAL_PROPERTY_FN( pgPLUGIN, IFaceEnum_PROP_get_plugin, NULL )
	mLOCAL_PROPERTY_FN( pgINDEX, IFaceEnum_PROP_get_index, NULL )
	mLOCAL_PROPERTY_FN( pgIFACE_FLAGS, IFaceEnum_PROP_get_flags, NULL )
	mLOCAL_PROPERTY_FN( pgIFACE_LOAD_ERR, IFaceEnum_PROP_get_load_err, NULL )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgINCLUDE_INTERFACES, IFaceEnum_PROP_get_include, IFaceEnum_PROP_set_include )
	mLOCAL_PROPERTY_WRITABLE_OI_FN( pgEXCLUDE_INTERFACES, IFaceEnum_PROP_get_exclude, IFaceEnum_PROP_set_exclude )
	mSHARED_PROPERTY( pgINTERFACE_COMMENT_CP, ((tCODEPAGE)(cCP_ANSI)) )
	mSHARED_PROPERTY( pgOBJECT_NAME_CP, ((tCODEPAGE)(cCP_ANSI)) )
	mLAST_CALL_PROPERTY( IFaceEnum_PROP_get_last, NULL )
	//! mLOCAL_PROPERTY_FN( pgIFACE_RECOGNIZE_FUNC, IFaceEnum_PROP_get_recognize, NULL )
mPROPERTY_TABLE_END(IFaceEnum_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )
// Interface "IFaceEnum". Register function
tERROR pr_call IFaceEnum_Register( hROOT root ) {
	tERROR error;
	
	PR_TRACE_A0( root, "Enter \"IFaceEnum::Register\" method" );
	
	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_IFACE_ENUM,                         // interface identifier
		PID_KERNEL,                             // plugin identifier
		0x00000000,                             // subtype identifier
		IFaceEnum_VERSION,                      // interface version
		VID_PETROVITCH,                         // interface developer
		&i_IFaceEnum_vtbl,                      // internal(kernel called) function table
		(sizeof(i_IFaceEnum_vtbl)/sizeof(tPTR)),// internal function table size
		&e_IFaceEnum_vtbl,                      // external function table
		(sizeof(e_IFaceEnum_vtbl)/sizeof(tPTR)),// external function table size
		IFaceEnum_PropTable,                    // property table
		mPROPERTY_TABLE_SIZE(IFaceEnum_PropTable),// property table size
		sizeof(IFENUM_DATA),                    // memory size
		IFACE_SYSTEM | IFACE_UNSWAPPABLE        // interface flags
	);
	
	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE(( root, prtDANGER, "krn\tIFaceEnum(IID_IFACE_ENUM) registered [%terr]", error ));
	#endif // _PRAGUE_TRACE_
	

	PR_TRACE_A1( root, "Leave \"IFaceEnum::Register\" method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Caution !!!,  )
// Last call property is checked, but neither *GET* nor *SET* function names are not defined
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_plugin )
// Interface "IFaceEnum". Method "Get" for property(s):
//  -- PLUGIN
tERROR pr_call IFaceEnum_PROP_get_plugin( hi_IFaceEnum _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method \"IFaceEnum_PROP_get_plugin\" for property \"pgPLUGIN\"" );

	if ( buffer ) {
		IFENUM_DATA* d = _this->data;
		if ( d->iface ) {
			*(hOBJECT*)buffer = (hOBJECT)d->iface->plugin;
			*out_size = sizeof(hOBJECT);
		}
		else {
			error = errOBJECT_BAD_INTERNAL_STATE;
			*out_size = 0;
		}
	}
	else
		*out_size = sizeof(hOBJECT);

	PR_TRACE_A2( _this, "Leave *GET* method \"IFaceEnum_PROP_get_plugin\" for property \"pgPLUGIN\", ret tDWORD = %u(size), error = %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_index )
// Interface "IFaceEnum". Method "Get" for property(s):
//  -- INDEX
tERROR pr_call IFaceEnum_PROP_get_index( hi_IFaceEnum _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method \"IFaceEnum_PROP_get_index\" for property \"pgINDEX\"" );

	if ( buffer ) {
		IFENUM_DATA* d = _this->data;
		if ( d->iface ) {
			*(tDWORD*)buffer = d->iface->index;
			*out_size = sizeof(tDWORD);
		}
		else {
			error = errOBJECT_BAD_INTERNAL_STATE;
			*out_size = 0;
		}
	}
	else
		*out_size = sizeof(tDWORD);
	
	PR_TRACE_A2( _this, "Leave *GET* method \"IFaceEnum_PROP_get_index\" for property \"pgINDEX\", ret tDWORD = %u(size), error = %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_flags )
// Interface "IFaceEnum". Method "Get" for property(s):
//  -- IFACE_FLAGS
tERROR pr_call IFaceEnum_PROP_get_flags( hi_IFaceEnum _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method \"IFaceEnum_PROP_get_flags\" for property \"pgIFACE_FLAGS\"" );

	if ( buffer ) {
		IFENUM_DATA* d = _this->data;
		if ( d->iface ) {
			*(tDWORD*)buffer = d->iface->flags;
			*out_size = sizeof(tDWORD);
		}
		else {
			error = errOBJECT_BAD_INTERNAL_STATE;
			*out_size = 0;
		}
	}
	else
		*out_size = sizeof(tDWORD);

	PR_TRACE_A2( _this, "Leave *GET* method \"IFaceEnum_PROP_get_flags\" for property \"pgIFACE_FLAGS\", ret tDWORD = %u(size), error = %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_load_err )
// Interface "IFaceEnum". Method "Get" for property(s):
//  -- IFACE_LOAD_ERROR
tERROR pr_call IFaceEnum_PROP_get_load_err( hi_IFaceEnum _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method \"IFaceEnum_PROP_get_load_err\" for property \"pgIFACE_LOAD_ERROR\"" );

	if ( buffer ) {
		IFENUM_DATA* d = _this->data;
		if ( d->iface ) {
			*(tERROR*)buffer = d->iface->last_err;
			*out_size = sizeof(tERROR);
		}
		else {
			error = errOBJECT_BAD_INTERNAL_STATE;
			*out_size = 0;
		}
	}
	else
		*out_size = sizeof(tDWORD);
	
	PR_TRACE_A2( _this, "Leave *GET* method \"IFaceEnum_PROP_get_load_err\" for property \"pgIFACE_LOAD_ERROR\", ret tDWORD = %u(size), error = %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_include )
// Interface "IFaceEnum". Method "Get" for property(s):
//  -- INCLUDE_INTERFACES
tERROR pr_call IFaceEnum_PROP_get_include( hi_IFaceEnum _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	IFENUM_DATA* d = _this->data;
	PR_TRACE_A0( _this, "Enter *GET* method get_include for property pgINCLUDE_INTERFACES" );

	*out_size = sizeof(tIFaceId) * (1+d->include_count);
	if ( buffer ) {
		if ( *out_size > size )
			error = errBUFFER_TOO_SMALL;
		else {
			tIFaceId* dst = (tIFaceId*)buffer;
			dst->iid = d->iid;
			dst->pid = d->pid;
			if ( d->include_count ) {
				tUINT i, c = d->include_count;
				tIFaceId* src = d->includes;
				for( i=0,++dst; i<c; ++i,++src,++dst )
					*src = *dst;
			}
		}
	}

	PR_TRACE_A2( _this, "Leave *GET* method get_include for property pgINCLUDE_INTERFACES, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_include )
// Interface "IFaceEnum". Method "Set" for property(s):
//  -- INCLUDE_INTERFACES
tERROR pr_call IFaceEnum_PROP_set_include( hi_IFaceEnum _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tUINT count;
	tERROR error = errOK;
	IFENUM_DATA* d = _this->data;
	PR_TRACE_A0( _this, "Enter *SET* method set_include for property pgINCLUDE_INTERFACES" );

	count = size / sizeof(tIFaceId);
	if ( count != d->include_count )
		error = CALL_SYS_ObjHeapRealloc( _this, &d->includes, d->includes, count * sizeof(tIFaceId) );
	if ( PR_SUCC(error) ) {
		tUINT i;
		tIFaceId* src = (tIFaceId*)buffer;
		tIFaceId* dst = d->includes;
		for( i=0; i<count; ++i,++src,++dst )
			*src = *dst;
		d->include_count = count;
		*out_size = count * sizeof(tIFaceId);
	}
	else
		*out_size = 0;

	PR_TRACE_A2( _this, "Leave *SET* method set_include for property pgINCLUDE_INTERFACES, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_exclude )
// Interface "IFaceEnum". Method "Get" for property(s):
//  -- EXCLUDE_INTERFACES
tERROR pr_call IFaceEnum_PROP_get_exclude( hi_IFaceEnum _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
	IFENUM_DATA* d = _this->data;
	PR_TRACE_A0( _this, "Enter *GET* method get_exclude for property pgEXCLUDE_INTERFACES" );

	*out_size = sizeof(tIFaceId) * (d->exclude_count ? d->exclude_count : 1);
	if ( buffer ) {
		if ( *out_size > size )
			error = errBUFFER_TOO_SMALL;
		else if ( d->exclude_count ) {
			tUINT i, c = d->exclude_count;
			tIFaceId* dst = (tIFaceId*)buffer;
			tIFaceId* src = d->excludes;
			for( i=0,++dst; i<c; ++i,++src,++dst )
				*src = *dst;
		}
		else {
			tIFaceId* dst = (tIFaceId*)buffer;
			dst->iid = IID_NONE;
			dst->pid = PID_NONE;
		}
	}

	PR_TRACE_A2( _this, "Leave *GET* method get_exclude for property pgEXCLUDE_INTERFACES, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_exclude )
// Interface "IFaceEnum". Method "Set" for property(s):
//  -- EXCLUDE_INTERFACES
tERROR pr_call IFaceEnum_PROP_set_exclude( hi_IFaceEnum _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {

	tUINT count;
	tERROR error = errOK;
	IFENUM_DATA* d = _this->data;
	PR_TRACE_A0( _this, "Enter *SET* method set_exclude for property pgEXCLUDE_INTERFACES" );

	count = size / sizeof(tIFaceId);
	if ( count != d->exclude_count )
		error = CALL_SYS_ObjHeapRealloc( _this, &d->excludes, d->excludes, count * sizeof(tIFaceId) );
	if ( PR_SUCC(error) ) {
		tUINT i;
		tIFaceId* src = (tIFaceId*)buffer;
		tIFaceId* dst = d->excludes;
		for( i=0; i<count; ++i,++src,++dst )
			*src = *dst;
		d->exclude_count = count;
		*out_size = count * sizeof(tIFaceId);
	}
	else
		*out_size = 0;

	PR_TRACE_A2( _this, "Leave *SET* method set_exclude for property pgEXCLUDE_INTERFACES, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_last )
// Interface "IFaceEnum". Method "Get" for property(s):
//  -- PROP_LAST_CALL
tERROR pr_call IFaceEnum_PROP_get_last( hi_IFaceEnum _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
	tERROR error = errOK;
  tPO*   po;

  PR_TRACE_A0( _this, "Enter *GET* method for \"*LAST CALL*\" property " );
  enter_lf();

  if ( _this->data->iface ) {
		tDATA os;
		rlock(po);
    error = _InterfaceGetProp( po, _this->data->iface, &os, prop, buffer, size );
		runlock(po);
		*out_size = (tDWORD)os;
	}
  else {
		*out_size = 0;
    error = errOBJECT_BAD_INTERNAL_STATE;
	}

  leave_lf();
	PR_TRACE_A2( _this, "Leave *GET* method for property \"*LAST CALL*\", ret tDWORD = %u, error = %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Init )
// --- Interface "IFaceEnum". Method "Init"
// Extended method comment
//   Initialization of interface enumerator interface
// Behaviour comment
//   Constructor analog.
//   After Init or Reset method current position must be *BEFORE* first matching interface I.e. first call of Next method must advance current position to *FIRST* matching interface
// Parameter "iid":
//   Identifier inerface to enumerate.
//   May be IID_ANY
// Parameter "pid":
//   Plugin id to enumerate.
//   May be PID_ANY
// Parameter "vid":
//   Vendor id to enumerate
//   May be VID_ANY
// Parameter "object":
//   Object to try to Recognize by enumetated interfaces
//   May be NULL
tERROR pr_call IFaceEnum_Init( hi_IFaceEnum _this, tIID iid, tPID pid, tVID vid, hOBJECT object ) {
  tERROR error = errOK;
  PR_TRACE_A0( _this, "Enter \"IFaceEnum::Init\"" );

  if ( (object == NULL) || PR_SUCC(error=CALL_SYS_ObjectValid(_this,object)) ) {
		tPO* po;
    IFENUM_DATA* d = _this->data;
		
		enter_lf();
		_iface_unload( d, po );
		leave_lf();

    d->iid        = iid;
    d->pid        = pid;
    d->vid        = vid;
    d->object     = object;
    d->saved_pos  = 0;
    d->iface      = NULL;
  }
	else {
		PR_TRACE(( _this, prtERROR, "krn\tIFace_Enum object not initialized - %terr", error ));
	}

  PR_TRACE(( _this, prtSPAM, "krn\tIFace_Enum object initialized[%terr]", error ));
  PR_TRACE_A0( _this, "Leave \"IFaceEnum::Init\"" );
  return error;
	
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Clone )
// --- Interface "IFaceEnum". Method "Clone"
// Extended method comment
//   Clone current enumerator.
// Behaviour comment
//   Current position must be the same as an original enumerator
// Result comment
//   Must return enumerator with the same attributes ( see Init method ) as an original enumerator
// Parameter "dad":
//   parent for new object
tERROR pr_call IFaceEnum_Clone( hi_IFaceEnum _this, hIFACE_ENUM* result, hOBJECT dad ) {
  tERROR error;
	tHANDLE* h;
  hi_IFaceEnum obj = NULL;
  PR_TRACE_A0( _this, "Enter \"IFaceEnum::Clone\"" );

  if ( !dad )
    dad = _this->sys->ParentGet( (hOBJECT)_this, IID_ANY );

  if ( _HC(h,0,dad,&error) && PR_SUCC(error=CALL_SYS_ObjectCreateQuick(dad,(hOBJECT*)&obj,IID_IFACE_ENUM,PID_ANY,SUBTYPE_ANY)) ) {
    IFENUM_DATA* d = _this->data;
		if ( errOK == (error=CALL_IFaceEnum_Init(obj,d->iid,d->pid,d->vid,d->object)) ) {
			if ( PID_KERNEL == CALL_SYS_PropertyGetDWord(obj,pgPLUGIN_ID) )
				((hi_IFaceEnum)obj)->data->saved_pos = _this->data->saved_pos;
		}
    else {
      obj->sys->ObjectClose( (hOBJECT)obj );
      obj = 0;
    }
  }

  if ( result )
    *result = (hIFACE_ENUM)obj;

  PR_TRACE(( _this, prtSPAM, "krn\tIFace_Enum object cloned[%terr, new obj = 0x%x]", error, obj ));
  PR_TRACE_A2( _this, "Leave \"IFaceEnum::Clone\" method, ret hIFACE_ENUM = %p, %terr", obj, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Reset )
// --- Interface "IFaceEnum". Method "Reset"
// Extended method comment
//   Resets current position of enumerator
// Behaviour comment
//   After Init or Reset method current position must be *BEFORE* first matching interface I.e. first call of Next method must advance current position to *FIRST* matching interface
tERROR pr_call IFaceEnum_Reset( hi_IFaceEnum _this ) {
	IFENUM_DATA* d;
  tPO*         po;
  PR_TRACE_A0( _this, "Enter \"IFaceEnum::Reset\"" );
	enter_lf();

	d = _this->data;
	if ( d->iface ) {
		_iface_unload( d, po );
		d->iface = 0;
	}
  d->saved_pos = 0;

	leave_lf();
  PR_TRACE(( _this, prtSPAM, "krn\tIFace_Enum object reseted" ));
  PR_TRACE_A0( _this, "Leave \"IFaceEnum::Reset\" method, ret tVOID, error = errOK" );
  return errOK;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Next )
// --- Interface "IFaceEnum". Method "Next"
// Extended method comment
//   Advance current pointer to next matching position
// Behaviour comment
//   First call of Next method must advance current pointer to *FIRST* matching interface. I.e. after Init or Reset method current position must be *BEFORE* first matching interface
// Result comment
//   cTRUE if next matching interface had been found
tERROR pr_call IFaceEnum_Next( hi_IFaceEnum _this ) {
	
  tINTERFACE*  iface;
  tERROR       error;
	tHANDLE*     handle;
  tPO*         po;

  PR_TRACE_A0( _this, "Enter \"IFaceEnum::Next\"" );
  enter_lf();

  _HC( handle, po, (hOBJECT)_this, &error );
	if ( handle ) {
		tINTERFACE*   l_ifaces[LOCAL_IFACE_CLUSTER];
		tINTERFACE**  pifaces = l_ifaces;
		tUINT         i;
		tUINT         count = 0;
		tBOOL         loaded = cFALSE;
		IFENUM_DATA*  data = _this->data;
		tExceptionReg er;

		error = errOK;
		for ( ;; ) {
			iface = _InterfaceFind( &data->saved_pos, data->iid, data->pid, SUBTYPE_ANY, data->vid, cTRUE );
			if ( !iface )
				break;
			if ( !data->object ) // if object is not prescribed we found it !
				break;
			if ( PR_FAIL(error=_InterfaceLoad(po,iface)) ) {
				iface = 0;
				break;
			}
			loaded = cTRUE;
			if ( !iface->internal->Recognize ) { // if iface has no Reconize method -> it is not an our case
				_InterfaceUnload( po, iface );
				continue;
			}
			if ( PR_FAIL(error=_add_iface_to_local_cache(iface,&pifaces,&count)) ) {
				iface = 0;
				break;
			}
		}
		runlock(po);

		if ( !iface && PR_SUCC(error) && count ) {
			error = errINTERFACE_NOT_FOUND;
			if ( errOK == si.EnterProtectedCode(0,&er) ) {
				tINTERFACE** table = pifaces;
				for( i=0; i<count; i++,table++ ) {
					if ( !iface )
						error = (*table)->internal->Recognize( data->object, data->type );
					if ( !iface && PR_SUCC(error) ) // if Recognize returns cTRUE this object is applicable for the interface
						iface = *table;
					else {
						rlock(po);
						_InterfaceUnload( po, *table );
						runlock(po);
					}
				}
			}
			else
				error = errUNEXPECTED;
			si.LeaveProtectedCode( 0, &er );
		}
		
		if ( pifaces != l_ifaces )
			PrFree( pifaces );

    rlock(po);
		if ( data->iface != iface ) {
			_iface_unload( data, po );
	
			data->iface = iface;
			if ( iface )
				data->iface_load_count = !!loaded;
			else if ( PR_SUCC(error) )
				error = errINTERFACE_NOT_FOUND;
		}
    runlock(po);
	}
	
  leave_lf();
  PR_TRACE_A2( _this, "Leave \"IFaceEnum::Next\" ret %terr, iface = %p", error, iface );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetIFaceProp )
// --- Interface "IFaceEnum". Method "GetIFaceProp"
// Extended method comment
//   Get static properties of current position(interface)
// Result comment
//   the same as PropertyGet system method
// Parameter "prop_id":
//   property identifier to get
// Parameter "buffer":
//   buffer to put value of property
//   if buffer==NULL method must return complete size of buffer to get value
// Parameter "size":
//   size of buffer to get value, must be 0 if buffer==NULL
tERROR pr_call IFaceEnum_GetIFaceProp( hi_IFaceEnum _this, tDWORD* result, tPROPID prop_id, tPTR buffer, tDWORD size ) {
  tERROR error;
  tPO*   po;
  PR_TRACE_A0( _this, "Enter \"IFaceEnum::GetIFaceProp\"" );
  enter_lf();

  if ( _this->data->iface ) {
		tDATA res;
		rlock(po);
    error = _InterfaceGetProp( po, _this->data->iface, &res, prop_id, buffer, size );
		runlock(po);
		if ( result )
			*result = (tDWORD)res;
  }
  else
    error = errOBJECT_BAD_INTERNAL_STATE;

  leave_lf();
  PR_TRACE_A1( _this, "Leave \"IFaceEnum::GetIFaceProp\" ret tERROR = %terr", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ObjectCreate )
// --- Interface "IFaceEnum". Method "ObjectCreate"
// Extended method comment
//   Create instance of interface under current position
// Parameter "parent":
//   Create new object under this parent if hasn't given another in Init's parameter
tERROR pr_call IFaceEnum_ObjectCreate( hi_IFaceEnum _this, hOBJECT* result, hOBJECT parent ) {

  tERROR       error;
  tHANDLE*     handle;
	tHANDLE*     new_handle;
  IFENUM_DATA* data;
  tPO*         po;

  PR_TRACE_A0( _this, "Enter \"IFaceEnum::ObjectCreate\"" );
  enter_lf();

  data = _this->data;
	new_handle = 0;

  if ( !data->iface ) {
    error = errINTERFACE_NOT_FOUND;
    PR_TRACE(( _this, prtERROR, "krn\tIFace_Enum cannot create an object - interface not found" ));
  }

  else {

    if ( !parent )              // if ( data->object )
      parent = data->object;    //   parent = data->object;

    _HC( handle, po, parent, &error );
    if ( handle ) {
      error = _ObjectCreate( po, handle, &new_handle, data->iface );
      runlock(po);
    }
    else {
      error = errOBJECT_INCOMPATIBLE;
      PR_TRACE(( _this, prtERROR, "krn\tIFace_Enum cannot create an object - bad parent" ));
    }
  }

  if ( result )
    *result = MakeObject(new_handle);

  leave_lf();
  PR_TRACE_A2( _this, "Leave \"IFaceEnum::ObjectCreate\" ret hOBJECT = %p, %terr", MakeObject(new_handle), error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Recognize )
// --- Interface "IFaceEnum". Method "Recognize"
// Extended method comment
//   check is the current interface can create a new instance on parameter "object" base
// Parameter "object":
//   object for check
tERROR pr_call IFaceEnum_Recognize( hi_IFaceEnum _this, hOBJECT object, tDWORD type ) {

  tERROR       error;
  IFENUM_DATA* d = _this->data;
  tPO*         po;

  PR_TRACE_A0( _this, "Enter \"IFaceEnum::Recognize\"" );
  enter_lf();

  if ( !d->iface ) {
    PR_TRACE(( _this, prtERROR, "krn\tIFace_Enum::Recognize[interface enumerator doesn't have an interface]" ));
    error = errINTERFACE_NOT_FOUND;
  }

	else if ( !(d->iface->flags & IFACE_LOADED) && PR_FAIL(error=_iface_load(d,po)) ) {
		PR_TRACE(( _this, prtERROR, "krn\tIFace_Enum::Recognize[cannot load interface(%tiid,%tpid)]", d->iid, d->pid ));
	}

	else if ( !d->iface->internal->Recognize ) {
		PR_TRACE(( _this, prtERROR, "krn\tIFace_Enum::Recognize[interface(%tiid,%tpid) has no Recognize method]", d->iid, d->pid ));
    error = errINTERFACE_HAS_NO_RECOGNIZER;
	}

  else {
    tExceptionReg er;
    if ( errOK == si.EnterProtectedCode(0,&er) )
			error = d->iface->internal->Recognize( object, type );
		else
			error = errUNEXPECTED;
		si.LeaveProtectedCode( 0, &er );

		PR_TRACE(( _this, prtNOTIFY, "krn\tIFace_Enum::Recognize(%tiid)[interface recognizer error = %terr]", GET_IID_C(MakeHandle(object)), error ));
	}

  leave_lf();
  PR_TRACE_A1( _this, "Leave \"IFaceEnum::Recognize\" ret %terr", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Load )
// Parameters are:
tERROR pr_call IFaceEnum_Load( hi_IFaceEnum _this ) {
  tERROR       error;
  IFENUM_DATA* d;
  tPO*         po;
  PR_TRACE_A0( _this, "Enter \"IFaceEnum::Load\" method" );
  enter_lf();

  d = _this->data;
  if ( d->iface ) {
    rlock(po);

    error = _iface_load( d, 0 );
    if (  PR_FAIL(error) || !(d->iface->flags & IFACE_LOADED) ) {
      if ( PR_FAIL(d->iface->last_err) )
        error = d->iface->last_err;
      else if ( PR_SUCC(error) )
        error = errINTERFACE_NOT_LOADED;
      PR_TRACE(( _this, prtERROR, "krn\tInterface(%tiid) cannot be loaded. %terr", d->iface->iid, error ));
    }
    runlock(po);
  }
  else {
    PR_TRACE(( _this, prtIMPORTANT, "krn\tIFace enumerator has no interface found" ));
    error = errOBJECT_BAD_INTERNAL_STATE;
  }

  leave_lf();
  PR_TRACE_A1( _this, "Leave \"IFaceEnum::Load\" method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, Free )
// Parameters are:
tERROR pr_call IFaceEnum_Free( hi_IFaceEnum _this ) {
	tERROR       error;
	IFENUM_DATA* d;
  tPO*         po;
  PR_TRACE_A0( _this, "Enter \"IFaceEnum::Free\" method" );
  enter_lf();

	d = _this->data;
	if ( d->iface ) {
		_iface_unload( d, po );
    error = errOK;
	}
	else {
		PR_TRACE(( _this, prtIMPORTANT, "krn\tIFace enumerator has no interface found" ));
		error = errOBJECT_BAD_INTERNAL_STATE;
	}
	
	leave_lf();
  PR_TRACE_A1( _this, "Leave \"IFaceEnum::Free\" method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// ---
static tERROR pr_call _iface_load( IFENUM_DATA* d, tPO* po ) {
	tERROR err;
	if ( !d->iface )
		return errOK;

	if ( po )
		rlock(po);
	err = _InterfaceLoad( 0, d->iface );
	if ( po )
		runlock(po);
	++d->iface_load_count;
	return err;
}



// ---
static tERROR pr_call _iface_unload( IFENUM_DATA* d, tPO* po ) {
	tERROR err = errOK;
	if ( d->iface && d->iface_load_count ) {
		rlock(po);
		while( d->iface_load_count ) {
			err = _InterfaceUnload( 0, d->iface );
			if ( PR_FAIL(err) )
				break;
			--d->iface_load_count;
		}
		runlock(po);
	}
	return err;
}


