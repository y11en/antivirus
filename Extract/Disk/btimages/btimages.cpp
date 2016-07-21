// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  03 August 2004,  11:00 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- btimages
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Sobko
// File Name   -- btimages.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define BtImages_VERSION ((tVERSION)1)
// AVP Prague stamp end

#define PR_IMPEX_DEF


// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Extract/plugin/p_btimages.h>

#include "bti_imp.h"
// AVP Prague stamp end

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE( import_table )
#include <Prague/iface/e_loader.h>
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_ktrace.h>

#include <Extract/plugin/p_windiskio.h>
IMPORT_TABLE_END

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable BTImages : public cBtImages /*cObjImpl*/ 
{
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call ObjectClose();

// Property function declarations
	tERROR pr_call RepositoryName_Get( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
	tERROR pr_call RepositoryName_Set( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );

public:
// External function declarations
	tERROR pr_call Save( hOBJECT p_io );
	tERROR pr_call Restore( hOBJECT p_io, tDWORD p_index );
	tERROR pr_call GetImageDateTime( hOBJECT p_io, tDWORD p_index, tDATETIME* p_dt );

// Data declaration
	_bti_data m_Data; // --
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(BTImages)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "BtImages". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR BTImages::ObjectInit()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "BtImages::ObjectInit method" );

	// Place your code here
	error = sysCreateObjectQuick((hOBJECT*) &m_Data.m_pRepositoryName, IID_STRING);
	if (PR_SUCC(error))
		m_Data.m_pRepositoryName->SetCP(cCP_UNICODE);

	m_Data.m_pRepositoryIO = NULL;
	m_Data.m_pMutex = NULL;

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR BTImages::ObjectInitDone()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "BtImages::ObjectInitDone method" );

	// Place your code here
	if (!m_Data.m_pMutex)
	{
		error = errOBJECT_CANNOT_BE_INITIALIZED;
		PR_TRACE((this, prtERROR, "sync in bti not created"));
	}
	else
		error = bti_init(m_Data.m_pRepositoryName, &m_Data.m_pRepositoryIO);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR BTImages::ObjectPreClose()
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "BtImages::ObjectPreClose method" );

	// Place your code here
	error = errOK;
	if (m_Data.m_pRepositoryIO)
	{
		m_Data.m_pMutex->Lock(cSYNC_INFINITE);
		m_Data.m_pRepositoryIO->Flush();
		m_Data.m_pMutex->Release();

		m_Data.m_pRepositoryIO->sysCloseObject();
		m_Data.m_pRepositoryIO = NULL;
	}

	if (m_Data.m_pRepositoryName)
	{
		m_Data.m_pRepositoryName->sysCloseObject();
		m_Data.m_pRepositoryName = NULL;
	}

	if (m_Data.m_pMutex)
	{
		m_Data.m_pMutex->sysCloseObject();
		m_Data.m_pMutex = NULL;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//    Kernel warns object it must be closed
// Behaviour comment
//    Object takes all necessary "before closing" actions
// Parameters are:
tERROR BTImages::ObjectClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "BtImages::ObjectClose method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, RepositoryName_Get )
// Interface "BtImages". Method "Get" for property(s):
//  -- RepositoryName
tERROR BTImages::RepositoryName_Get( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *GET* method RepositoryName_Get for property pgRepositoryName" );

	error = m_Data.m_pRepositoryName->ExportToBuff(out_size, cSTRING_WHOLE, buffer, size, cCP_UNICODE, cSTRING_Z);

	PR_TRACE_A2( this, "Leave *GET* method RepositoryName_Get for property pgRepositoryName, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, RepositoryName_Set )
// Interface "BtImages". Method "Set" for property(s):
//  -- RepositoryName
tERROR BTImages::RepositoryName_Set( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( this, "Enter *SET* method RepositoryName_Set for property pgRepositoryName" );

	error = m_Data.m_pRepositoryName->ImportFromBuff(out_size, buffer, size, cCP_UNICODE, cSTRING_Z);
	if (m_Data.m_pMutex)
		m_Data.m_pMutex->sysCloseObject();
	
	m_Data.m_pMutex = bti_InitMutex(*this, CountCRC32(size, (unsigned char*) buffer, -1));

	PR_TRACE_A2( this, "Leave *SET* method RepositoryName_Set for property pgRepositoryName, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Save )
// Parameters are:
tERROR BTImages::Save( hOBJECT p_io )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "BtImages::Save method" );

	// Place your code here
	if (PR_FAIL(sysCheckObject(p_io, IID_IO, PID_WINDISKIO)))
		error = errPARAMETER_INVALID;
	else
	{
		if (PR_SUCC(error = m_Data.m_pMutex->Lock(cSYNC_INFINITE)))
		{
			error = bti_save(m_Data.m_pRepositoryIO, (hIO) p_io);
			m_Data.m_pMutex->Release();
		}
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Restore )
// Parameters are:
tERROR BTImages::Restore( hOBJECT p_io, tDWORD p_index )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "BtImages::Restore method" );

	// Place your code here
	if (PR_FAIL(sysCheckObject(p_io, IID_IO, PID_WINDISKIO)))
		error = errPARAMETER_INVALID;
	else
	{
		if (PR_SUCC(error = m_Data.m_pMutex->Lock(cSYNC_INFINITE)))
		{
			error = bti_restore(m_Data.m_pRepositoryIO, (hIO) p_io, p_index);
			m_Data.m_pMutex->Release();
		}
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetImageDateTime )
// Parameters are:
tERROR BTImages::GetImageDateTime( hOBJECT p_io, tDWORD p_index, tDATETIME* p_dt )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "BtImages::GetImageDateTime method" );

	// Place your code here
	if (PR_FAIL(sysCheckObject(p_io, IID_IO, PID_WINDISKIO)))
		error = errPARAMETER_INVALID;
	else
	{
		if (PR_SUCC(error = m_Data.m_pMutex->Lock(cSYNC_INFINITE)))
		{
			error = bti_getimagedatetime(m_Data.m_pRepositoryIO, (hIO) p_io, p_index, p_dt);
			m_Data.m_pMutex->Release();
		}
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "BtImages". Register function
tERROR BTImages::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end

	CALL_Root_ResolveImportTable( root, NULL, import_table, PID_BTIMAGES);
    if (!pr_sprintf)
		return errUNEXPECTED;

// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(BtImages_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, BtImages_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "boot area repository", 21 )
	mpLOCAL_PROPERTY_REQ_WRITABLE_OI_FN( pgRepositoryName, FN_CUST(RepositoryName_Get), FN_CUST(RepositoryName_Set) )
mpPROPERTY_TABLE_END(BtImages_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(BtImages)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(ObjectClose)
mINTERNAL_TABLE_END(BtImages)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(BtImages)
	mEXTERNAL_METHOD(BtImages, Save)
	mEXTERNAL_METHOD(BtImages, Restore)
	mEXTERNAL_METHOD(BtImages, GetImageDateTime)
mEXTERNAL_TABLE_END(BtImages)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "BtImages::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_BTIMAGES,                           // interface identifier
		PID_BTIMAGES,                           // plugin identifier
		0x00000000,                             // subtype identifier
		BtImages_VERSION,                       // interface version
		VID_SOBKO,                              // interface developer
		&i_BtImages_vtbl,                       // internal(kernel called) function table
		(sizeof(i_BtImages_vtbl)/sizeof(tPTR)), // internal function table size
		&e_BtImages_vtbl,                       // external function table
		(sizeof(e_BtImages_vtbl)/sizeof(tPTR)), // external function table size
		BtImages_PropTable,                     // property table
		mPROPERTY_TABLE_SIZE(BtImages_PropTable),// property table size
		sizeof(BTImages)-sizeof(cObjImpl),      // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"BtImages(IID_BTIMAGES) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call BtImages_Register( hROOT root ) { return BTImages::Register(root); }
// AVP Prague stamp end



