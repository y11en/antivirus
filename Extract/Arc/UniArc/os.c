// -------- Monday,  23 October 2000,  15:05 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2000.
// -------------------------------------------
// Project     -- Unversal Extractor
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andy Nikishin
// File Name   -- os.c
// -------------------------------------------




#define OS_PRIVATE_DEFINITION

#include <Prague/prague.h>
#include <Prague/pr_pid.h>
#include <Prague/iface/i_compar.h>
#include <Prague/iface/i_heap.h>
#include <Prague/iface/i_ifenum.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_list.h>
#include <Prague/iface/i_objptr.h>
#include <Prague/iface/i_seqio.h>
#include <Prague/iface/i_string.h>
#include <Prague/iface/i_tree.h>

#include <Extract/iface/i_minarc.h>
#include <Extract/plugin/p_miniarc.h>

#define  IMPEX_IMPORT_LIB
#include <AV/plugin/p_avlib.h> 

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE( import_table )
#include <AV/plugin/p_avlib.h> 
IMPORT_TABLE_END


#include "StdCompare/plugin_stdcompare.h"

#include "os.h"
#include "objenum.h"
#include "io.h"
#include "uacback.h"

#include <string.h>

static tPID MyPropID;

#define SizeOfNameBuffer 512

char * itoa (int val, char *buf, int radix);

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Interface comment )
// --------------------------------------------
// --- 213d8b50_e06c_11d3_bf0e_00d0b7161fb8 ---
// --------------------------------------------
// OS interface implementation
// Short comments -- object system interface
// Extended comment
//   Interface defines behavior of an object system. It responds for
//     - enumerate IO and Folder objects by creating Folder objects.
//     - create and delete IO and Folder objects
//   There are two possible ways to create or open IO object on OS. First is three steps protocol:
//     -- call ObjectCreate system method and get new IO (Folder) object
//     -- dictate properties of new object
//     -- call ObjectCreateDone system method and have working object
//   Second is using FolderCreate and IOCreate methods which must have the same behivior i.e. FolderCreate and IOCreate are wrappers for convenience.
//   Advise: when caller invokes ObjectCreateDone method OS object receives ChildDone notification and can do all necessary job to process it.
//   
// AVP Prague stamp end( OS, Interface comment )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------

tERROR RebuildArchive(hi_OS _this);
hSEQ_IO CreateSeqIO(hIO hIO);
tERROR uCopyFile(hSEQ_IO hSrc, hSEQ_IO Dest, tBYTE* Buffer, tDWORD dwBufferSize);
tLISTNODE iFindInList(hi_OS _this, hLIST hList, tCHAR* szOldName, tCHAR* szNewName, tQWORD qwMagiq);
void iAddToList(hi_OS _this, hLIST* pList, tCHAR* szOldName, tCHAR* szNewName, tQWORD qwMagiq);
tERROR InitDone(hi_OS _this ) ;

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Internal method table prototypes )
// Interface "OS". Internal method table. Forward declarations
tERROR pr_call OS_Recognize( hOBJECT _that, tDWORD type);
tERROR pr_call OS_ObjectInit( hi_OS _this );
tERROR pr_call OS_ObjectInitDone( hi_OS _this );
tERROR pr_call OS_ObjectPreClose( hi_OS _this );
tERROR pr_call OS_ChildClose( hi_OS _this, hOBJECT child );
tERROR pr_call OS_MsgReceive( hi_OS _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tVOID* par_buf, tDWORD* par_buf_len );
// AVP Prague stamp end( OS, Internal method table prototypes )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Internal method table )
// Interface "OS". Internal method table.
static iINTERNAL i_OS_vtbl = 
{
	(tIntFnRecognize)        OS_Recognize,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       OS_ObjectInit,
	(tIntFnObjectInitDone)   OS_ObjectInitDone,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   OS_ObjectPreClose,
	(tIntFnObjectClose)      NULL,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       OS_ChildClose,
	(tIntFnMsgReceive)       OS_MsgReceive,
	(tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end( OS, Internal method table )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, interface function forward declarations )
typedef   tERROR (pr_call *fnpOS_PtrCreate) ( hi_OS _this, hObjPtr* result, hOBJECT name );              // -- opens ObjPtr object;
typedef   tERROR (pr_call *fnpOS_IOCreate)  ( hi_OS _this, hIO* result, hOBJECT name, tDWORD access_mode, tDWORD open_mode ); // -- opens IO object;
typedef   tERROR (pr_call *fnpOS_Copy)      ( hi_OS _this, hOBJECT src_name, hOBJECT dst_name, tBOOL overwrite ); // -- copies object inside OS;
typedef   tERROR (pr_call *fnpOS_Rename)    ( hi_OS _this, hOBJECT old_name, hOBJECT new_name, tBOOL overwrite ); // -- rename/move object by name inside OS;
typedef   tERROR (pr_call *fnpOS_Delete)    ( hi_OS _this, hOBJECT name );              // -- phisically deletes an entity on OS;
// AVP Prague stamp end( OS, interface function forward declarations )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, interface declaration )
struct iOSVtbl  
{
	fnpOS_PtrCreate  PtrCreate;
	fnpOS_IOCreate   IOCreate;
	fnpOS_Copy       Copy;
	fnpOS_Rename     Rename;
	fnpOS_Delete     Delete;
}; // OS
// AVP Prague stamp end( OS, interface declaration )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, External method table prototypes )
// Interface "OS". External method table. Forward declarations
tERROR pr_call OS_PtrCreate( hi_OS _this, hObjPtr* result, hOBJECT name );
tERROR pr_call OS_IOCreate( hi_OS _this, hIO* result, hOBJECT name, tDWORD access_mode, tDWORD open_mode );
tERROR pr_call OS_Copy( hi_OS _this, hOBJECT src_name, hOBJECT dst_name, tBOOL overwrite );
tERROR pr_call OS_Rename( hi_OS _this, hOBJECT old_name, hOBJECT new_name, tBOOL overwrite );
tERROR pr_call OS_Delete( hi_OS _this, hOBJECT name );
// AVP Prague stamp end( OS, External method table prototypes )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, External method table )
// Interface "OS". External method table.
static iOSVtbl e_OS_vtbl = 
{
	OS_PtrCreate,
	OS_IOCreate,
	OS_Copy,
	OS_Rename,
	OS_Delete
};
// AVP Prague stamp end( OS, External method table )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Forwarded property methods declarations )
// Interface "OS". Get/Set property methods
tERROR pr_call OS_PropertyGet( hi_OS _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
tERROR pr_call OS_PropertySet( hi_OS _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size );
// AVP Prague stamp end( OS, Forwarded property methods declarations )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Global property variable declaration )
// Interface "OS". Global(shared) property table variables
const tVERSION OS_Version = 2; // must be READ_ONLY at runtime
const tSTRING OS_Comment = "Universal Archiver"; // must be READ_ONLY at runtime
//const tDWORD OS_maxFileName = 260; // must be READ_ONLY at runtime
// AVP Prague stamp end( OS, Global property variable declaration )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Property table )
// Interface "OS". Property table
mPROPERTY_TABLE(OS_PropTable)
	mSHARED_PROPERTY_PTR( pgINTERFACE_VERSION, OS_Version, sizeof(OS_Version) )
	mLOCAL_PROPERTY_FN  ( pgOBJECT_AVAILABILITY, OS_PropertyGet, NULL )
	mLOCAL_PROPERTY_FN  ( pgINTERFACE_COMMENT, OS_PropertyGet, NULL )
	mLOCAL_PROPERTY_FN  ( pgNAME_MAX_LEN, OS_PropertyGet, NULL)
	mLOCAL_PROPERTY_FN  ( pgOBJECT_OS_TYPE, OS_PropertyGet, NULL )
	mLOCAL_PROPERTY_FN  ( pgOBJECT_NAME, OS_PropertyGet, NULL )
	mLOCAL_PROPERTY_FN  ( pgNAME_CASE_SENSITIVE, OS_PropertyGet, NULL )
	mLOCAL_PROPERTY_BUF ( pgOBJECT_BASED_ON, OS_Uni_Data, hBasedObject, cPROP_BUFFER_READ )
	mLOCAL_PROPERTY_FN  ( pgNAME_DELIMITER, OS_PropertyGet, OS_PropertySet  )
	mLOCAL_PROPERTY_FN  ( pgMULTIVOL_AS_SINGLE_SET, OS_PropertyGet, OS_PropertySet  )
	mLOCAL_PROPERTY_FN  ( pgOBJECT_HASH, OS_PropertyGet, NULL)
    mLOCAL_PROPERTY_BUF ( pgOBJECT_REOPEN_DATA, OS_Uni_Data, pidMiniArc, cPROP_BUFFER_READ_WRITE | cPROP_WRITABLE_ON_INIT )
//Local Properties
	mLOCAL_PROPERTY_BUF( ppMINI_ARCHIVER_OBJECT, OS_Uni_Data, hArc, cPROP_BUFFER_READ)
	mLOCAL_PROPERTY_BUF( ppMINI_ARCHIVER_PID, OS_Uni_Data, pidMiniArc, cPROP_BUFFER_READ_WRITE)

mPROPERTY_TABLE_END(OS_PropTable)
// AVP Prague stamp end( OS, Property table )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Registration call )
// Interface "OS". Register function
tERROR pr_call OS_Register( hROOT root ) 
{
tERROR error;

	PR_TRACE_A0( root, "Enter \"OS::Register\" method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_OS,                                 // interface identifier
		PID_UNIVERSAL_ARCHIVER,                 // plugin identifier
		0x00000000,                             // subtype identifier
		0x00000002,                             // interface version
		VID_ANDY,                               // interface developer
		&i_OS_vtbl,                             // internal(kernel called) function table
		(sizeof(i_OS_vtbl)/sizeof(tPTR)),       // internal function table size
		&e_OS_vtbl,                             // external function table
		(sizeof(e_OS_vtbl)/sizeof(tPTR)),       // external function table size
		OS_PropTable,                           // property table
		mPROPERTY_TABLE_SIZE(OS_PropTable),     // property table size
		sizeof(OS_Uni_Data),                        // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( error != errOK )
			PR_TRACE( (root,prtDANGER,"OS(IID_OS) registered [error=%u]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave \"OS::Register\" method, ret tERROR = %u", error );
	return error;
}
// AVP Prague stamp end( OS, Registration call )
// --------------------------------------------------------------------------------




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Property method implementation )
// Interface "OS". Method "Get" for property(s):
//  -- pgOBJECT_OS_TYPE
//  -- pgNAME_MAX_LEN
//  -- pgOBJECT_NAME
//  -- pgNAME_CASE_SENSITIVE
//  -- pgINTERFACE_COMMENT

tERROR pr_call OS_PropertyGet( hi_OS _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size ) 
{
tERROR error;

	error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method \"OS_PropertGet\" " );
	
	if(out_size)
		*out_size = 0;

	switch (prop)
	{
		case pgNAME_DELIMITER:
			if(_this->data->hArc == NULL)
			{
				if(buffer == NULL || size == 0)
				{
					if(out_size)
						*out_size = sizeof(tWORD);
					return errOK;
				}
				*(tBOOL*)buffer = _this->data->wDelimeter ;
				return errOK;
			}
			else
				return CALL_SYS_PropertyGet(_this->data->hArc, out_size, prop, buffer, size);
		case pgMULTIVOL_AS_SINGLE_SET:
			if(_this->data->hArc == NULL)
			{
				if(buffer == NULL || size == 0)
				{
					if(out_size)
						*out_size = sizeof(tBOOL);
					return errOK;
				}
				*(tBOOL*)buffer = _this->data->bScanAllVolumes ;
				return errOK;
			}
			else
				return CALL_SYS_PropertyGet(_this->data->hArc, out_size, prop, buffer, size);
			
		case pgINTERFACE_COMMENT:
			if(_this->data->hArc)
				return CALL_SYS_PropertyGetStr(_this->data->hArc, out_size, prop, buffer, size, cCP_ANSI);
			else 
				return errOBJECT_NOT_INITIALIZED;
		case pgOBJECT_NAME:
            if(_this->data->hARC_IO)
                return CALL_SYS_PropertyGet(_this->data->hARC_IO, out_size, prop, buffer, size);
            else
                return errOBJECT_NOT_INITIALIZED;
		case pgOBJECT_AVAILABILITY:
		case pgNAME_MAX_LEN:
		case pgOBJECT_OS_TYPE:
		case pgNAME_CASE_SENSITIVE:
		case pgOBJECT_HASH:
			if(/*_this->data->pidMiniArc != 0 &&*/ _this->data->bObjectInitDone == cFALSE)
			{
				error = InitDone(_this);
				if(PR_FAIL(error))
				{
					return error;
				}
			}
			if(_this->data->hArc)
				return CALL_SYS_PropertyGet(_this->data->hArc, out_size, prop, buffer, size);
			else 
				return errOBJECT_NOT_INITIALIZED;
        /*case pgOBJECT_REOPEN_DATA:
			if(buffer == NULL || size == 0)
			{
				if(out_size)
					*out_size = sizeof(tPID);
				return errOK;
			}
            else if (size < sizeof(tPID))
            {
				if(out_size)
					*out_size = sizeof(tPID);
                return errBUFFER_TOO_SMALL;
            }
			*(tPID*)buffer = _this->data->pidMiniArc;
			return errOK;*/
	}

	return error;
}
// AVP Prague stamp end( OS, Property method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Property method implementation )
// Interface "OS". Method "Set" for property(s):
//  -- OBJECT_NEW

tERROR pr_call OS_PropertySet( hi_OS _this, tUINT* out_size, tPROPID prop, tCHAR* buffer, tUINT size ) 
{
tERROR error;

	PR_TRACE_A0( _this, "Enter *SET* method \"OS_PropertySet\" for property \"pgOBJECT_NEW\"" );
	error = errPROPERTY_NOT_FOUND;
	
	if(out_size)
		*out_size = 0;
	
	switch (prop)
	{
		case pgNAME_DELIMITER:
			if(_this->data->hArc == NULL)
			{
				error = errOK;
				if(buffer == NULL || size == 0)
				{
					if(out_size)
						*out_size = sizeof(tWORD);
					break;
				}
				_this->data->wDelimeter = *(tWORD*)buffer;
			}
			else
				error = CALL_SYS_PropertySet(_this->data->hArc, out_size, prop, buffer, size);
			break;
			
		case pgMULTIVOL_AS_SINGLE_SET:
			if(_this->data->hArc == NULL)
			{
				error = errOK;
				if(buffer == NULL || size == 0)
				{
					if(out_size)
						*out_size = sizeof(tBOOL);
					break;
				}
				_this->data->bScanAllVolumes = *(tBOOL*)buffer;
			}
			else
				error = CALL_SYS_PropertySet(_this->data->hArc, out_size, prop, buffer, size);
			break;

        /*case pgOBJECT_REOPEN_DATA:
            error = errOK;
            if(buffer == NULL || size == 0)
            {
                if (out_size)
                    *out_size = sizeof(tPID);
                break;
            }
            else if (size < sizeof(tPID))
            {
				if(out_size)
					*out_size = sizeof(tPID);
                return errBUFFER_TOO_SMALL;
            }
            _this->data->pidMiniArc = *(tPID*)buffer;
            break;*/
	}

	PR_TRACE_A2( _this, "Leave *SET* method \"OS_PropertySet\" for property \"pgOBJECT_NEW\", ret tUINT = %u(size), error = %u", *out_size, error );
	return error;
}

// AVP Prague stamp end( OS, Property method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Internal (kernel called) interface method implementation )
// --- Interface "OS". Method "Recognize"
// Extended method comment
//   Static method. Kernel calls this method wihtout any created objects of this interface.
//   Responds to kernel request is it possible to create an this of the interface on a base of pointed this ("this" parameter)
//   Kernel considers absence of the implementation as errOK result for all objects
// Behaviour comment
//   It must be quick (preliminary) analysis
// Result comment
//   

tERROR pr_call OS_Recognize( hOBJECT _that, tDWORD type) 
{
tERROR error;
hIFACE_ENUM ienum;
tPID pid = 0;

	PR_TRACE_A2( 0, "Enter \"OS::Recognize\" method for object \"%p (iid=%u)\" ", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );
	error = errINTERFACE_INCOMPATIBLE;
	ienum = 0;

	if(IID_IO==CALL_SYS_PropertyGetDWord(_that, pgINTERFACE_ID))
	{
        if (type)
        {
            if(errOK == CALL_SYS_Recognize(_that, IID_MINIARCHIVER, type, 0))
            {
		        CALL_SYS_PropertySetDWord(_that, MyPropID, type);
		        error = errOK;
            }
        }
		else if ( PR_SUCC(CALL_SYS_ObjectCreateQuick(_that,&ienum,IID_IFACE_ENUM,PID_ANY,SUBTYPE_ANY)))
		{
			CALL_IFaceEnum_Init( ienum, IID_MINIARCHIVER, PID_ANY, VID_ANY, 0 );
			CALL_IFaceEnum_Reset( ienum );

			while( PR_SUCC(CALL_IFaceEnum_Next(ienum) ))
			{
				if(errOK == CALL_IFaceEnum_Recognize(ienum, _that, OS_TYPE_GENERIC))
				{
					if ( errOK == CALL_IFaceEnum_GetIFaceProp(ienum,0,pgPLUGIN_ID,&pid,sizeof(pid)) ) 
					{
						CALL_SYS_PropertySetDWord(_that, MyPropID, pid);
						error = errOK;
						break;
					}
				}
			}
			CALL_SYS_ObjectClose(ienum);
		}
	}

	PR_TRACE_A3( 0, "Leave \"OS::Recognize\" method for object \"%p (iid=%u)\", ret tBOOL = %u", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID), error );
	return error;
}
// AVP Prague stamp end( OS, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Internal (kernel called) interface method implementation )
// --- Interface "OS". Method "ObjectInit"
// Extended method comment
//   Kernel notifies an object about successful creating of object
// Behaviour comment
//   Initializes internal object data
// Result comment
//   Returns value differ from errOK if object cannot be initialized
tERROR pr_call OS_ObjectInit( hi_OS _this ) 
{
	return errOK;
}
// AVP Prague stamp end( OS, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------

tERROR InitDone(hi_OS _this ) 
{
tERROR error;
OS_Uni_Data* data;
hROOT hRoot;
tPID MyPropID;


	error = errOK;
	data=_this->data;
	if(data->bObjectInitDone)
	{
		return data->errObjectInitDone;
	}
	data->bObjectInitDone = cTRUE;

	if(data->pidMiniArc == 0)
	{
		hRoot = (hROOT)CALL_SYS_ParentGet(_this, IID_ROOT);
		CALL_Root_RegisterCustomPropId(hRoot, (tDWORD*)&MyPropID, cUNIARC_PROPERTY, pTYPE_PID);
		data->pidMiniArc = CALL_SYS_PropertyGetDWord(CALL_SYS_ParentGet(_this, IID_ANY), MyPropID);
	}
	
	if(data->pidMiniArc == 0)
	{
		error = OS_Recognize( CALL_SYS_ParentGet(_this, IID_ANY), 0);
		if(PR_SUCC(error))
		{
			error = errHANDLE_INVALID;
			
			data->pidMiniArc = CALL_SYS_PropertyGetDWord(CALL_SYS_ParentGet(_this, IID_ANY), MyPropID);
		}
	}
	data->hARC_IO = (hIO)CALL_SYS_ParentGet((hOBJECT)_this,IID_IO);
    data->hBasedObject = (hOBJECT)data->hARC_IO;
	
	if(data->pidMiniArc != 0 && data->hARC_IO != NULL)
	{
		error = CALL_SYS_ObjectCreate(_this, &data->hArc, IID_MINIARCHIVER, data->pidMiniArc, SUBTYPE_ANY);
		if(PR_SUCC(error))
		{
			tDWORD size;
			
			size = sizeof(tBOOL);
			CALL_SYS_PropertySet(data->hArc, NULL, pgMULTIVOL_AS_SINGLE_SET, &data->bScanAllVolumes, size);
			CALL_SYS_PropertySetWord(data->hArc, pgNAME_DELIMITER, data->wDelimeter);
		}
	}
    data->errObjectInitDone = error;
	return error;
}

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Internal (kernel called) interface method implementation )
// --- Interface "OS". Method "ObjectInitDone"
// Extended method comment
//   Kernel notifies an object about successful creating of object
// Behaviour comment
//   Initializes internal object data
// Result comment
//   Returns value differ from errOK if object cannot be initialized
tERROR pr_call OS_ObjectInitDone( hi_OS _this ) 
{
tERROR initerror, error;
OS_Uni_Data* data;
	
	error = errOK;
	PR_TRACE_A0( _this, "Enter \"OS::ObjectInitDone\" method" );
	data=_this->data;
	initerror = InitDone(_this);
	if(PR_SUCC(initerror))
	{
        if (data->hArc)
        {
		    error = CALL_SYS_ObjectCreateDone(data->hArc);
		    if(PR_SUCC(error))
		    {
			    CALL_SYS_RegisterMsgHandler( _this, msg_clsBROADCAST, rmhDECIDER, _this, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
			    data->bIsReadOnly = CALL_SYS_PropertyGetDWord(data->hArc, pgIS_READONLY);
			    if(!data->bIsReadOnly )
				    data->IsWritable = DONT_KNOW;
			    else
				    data->IsWritable = CANT_WRITE;
			    
			    CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&data->szNameBuffer, SizeOfNameBuffer);
			    CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&data->szNewNameBuffer, SizeOfNameBuffer);
			    if(data->szNameBuffer == NULL || data->szNewNameBuffer == NULL)
			    {
				    error = errNOT_ENOUGH_MEMORY;
			    }
		    }
        }
        else
        {
            error = errOBJECT_NOT_CREATED;
        }
	}
    if (error == errOK)
    {
        error = initerror; // we need to pass warning either from miniarchiver's init or initdone
    }

	PR_TRACE_A1( _this, "Leave \"OS::ObjectInitDone\" method, ret tERROR = %u", error );
	return error;
}
// AVP Prague stamp end( OS, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Internal (kernel called) interface method implementation )
// --- Interface "OS". Method "ObjectPreClose"
// Extended method comment
//   Kernel warns object it is going to close all children
// Behaviour comment
//   Object takes all necessary "before preclosing" actions
// Result comment
//   
tERROR pr_call OS_ObjectPreClose( hi_OS _this ) 
{
tERROR error;
hObjPtr objptr, objptrclose;

	PR_TRACE_A0( _this, "Enter \"OS::ObjectPreClose\" method" );
	error = errOK;

	error = CALL_SYS_ChildGetFirst(_this, (hOBJECT*)&objptr, IID_OBJPTR, PID_UNIVERSAL_ARCHIVER);
	while (PR_SUCC(error))
	{
        objptrclose = objptr;
		error = CALL_SYS_ObjectGetNext(objptr, (hOBJECT*)&objptr, IID_OBJPTR, PID_UNIVERSAL_ARCHIVER);
        CALL_SYS_ObjectClose(objptrclose);
	}
	if(error==errOBJECT_NOT_FOUND)
		error=errOK;


	if(_this->data->hNewTree!=NULL || 
		//_this->data->hChangeTree!=NULL || 
		_this->data->bRealChange ||
		_this->data->hDeletedTree!=NULL //|| 
		/*!_this->data->bIsReadOnly*/)
	{
		error = CALL_SYS_UnregisterMsgHandler( _this, msg_clsBROADCAST, _this);
		error = RebuildArchive((hi_OS)_this);
		if(_this->data->hNewTree)
		{
			CALL_SYS_ObjectClose(_this->data->hNewTree);
			_this->data->hNewTree = NULL;
		}
	}

	PR_TRACE_A1( _this, "Leave \"OS::ObjectPreClose\" method, ret tERROR = %u", error );
	return error;
}
// AVP Prague stamp end( OS, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Internal (kernel called) interface method implementation )
// --- Interface "OS". Method "ChildClose"
// Extended method comment
//   Notification about child is going to be destroyed. Parent can take considerable actions
// Behaviour comment
//   
// Result comment
//   Child will not be destroyed if error code is not errOK
// Parameter "child":
//   Child which is destroying
tERROR pr_call OS_ChildClose( hi_OS _this, hOBJECT child ) 
{
OS_Uni_Data* data;
tPID pid;
tTREENODE node;
hIO io, ioChild;
tDWORD dwBuf[3];

	PR_TRACE_A0( _this, "Enter \"OS::ChildClose\" method" );

	data=_this->data;

	pid=CALL_SYS_PropertyGetDWord(child, pgPLUGIN_ID );
	if(data->hNewTree && (PID_TMPFILE == pid))
	{
		CALL_Tree_First(data->hNewTree, &node);
		while(node)
		{
			CALL_Tree_NodeDataGet(data->hNewTree, NULL, node, dwBuf, sizeof(dwBuf));
			ioChild=*((hIO*) &dwBuf[2]);
			if((hOBJECT)ioChild == child)
			{
				if(PR_SUCC(CALL_SYS_ObjectCreate((hOBJECT)_this, (hOBJECT*)&io, IID_IO, PID_TMPFILE,0)))
				{
					CALL_SYS_PropertySetDWord( (hOBJECT)io, pgOBJECT_ACCESS_MODE, fACCESS_RW);
					CALL_SYS_PropertySetDWord( (hOBJECT)io, pgOBJECT_OPEN_MODE, fOMODE_CREATE_ALWAYS + fOMODE_SHARE_DENY_READ);
					if(PR_SUCC(CALL_SYS_ObjectCreateDone( io)))
					{
						CALL_SYS_ObjectSwap((hOBJECT)_this, child, io);
						*((hIO*) &dwBuf[2])=io;
						CALL_Tree_NodeDataSet(data->hNewTree, NULL, node, dwBuf, sizeof(dwBuf));
					}
				}
				break;
			}
			CALL_Tree_Next(data->hNewTree, &node, node);
		}
	}

	PR_TRACE_A1( _this, "Leave \"OS::ChildClose\" method, ret tERROR = %u", errOK );
	return errOK;
}
// AVP Prague stamp end( OS, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// Tree manipulation routines
hTREE OpenTree(hi_OS _this)
{
hTREE hTree;
tERROR error;
hCOMPARE hComp;

	error=(tERROR)CALL_SYS_ObjectCreate((hOBJECT)_this, (hOBJECT*)&hTree, IID_TREE, PID_ANY, 0);
	if(PR_SUCC(error))
	{
		CALL_SYS_PropertySetDWord(hTree, pgTREE_GRANULARITY, 512);

		error = (tERROR)CALL_SYS_ObjectCreate((hOBJECT)_this, &hComp, IID_COMPARE, PID_STDCOMPARE, 0);
		if(PR_SUCC(error))
		{
			if(PR_SUCC(CALL_SYS_ObjectCreateDone(hComp)))
				CALL_SYS_PropertySet(hTree, NULL, pgTREE_COMPARE_OBJECT, &hComp, sizeof(hCOMPARE));
		}
		error=CALL_SYS_ObjectCreateDone(hTree);
	}
	return hTree;
}

tERROR AddObjToTree(hi_OS _this, hTREE hTree, hOBJECT object)
{
tQWORD qwObjectId;
tDWORD dwBuff[3];
tERROR error = errOK;

    if (PR_FAIL(error = CALL_SYS_PropertyGet(object, NULL, ppOBJECT_TO_OPEN, &qwObjectId, sizeof(qwObjectId))))
    {
        return error;
    }

    *(tQWORD*)(&dwBuff[0]) = qwObjectId;
    *(tDWORD*)(&dwBuff[2]) = (tDWORD)object;
    
	CALL_Tree_Add(hTree, NULL, dwBuff, sizeof(dwBuff));

    return error;
}

tERROR AddIdToTree(hi_OS _this, hTREE hTree, tQWORD qwObjectId)
{
tDWORD dwBuff[3];
tERROR error = errOK;

    *(tQWORD*)(&dwBuff[0]) = qwObjectId;
    *(tDWORD*)(&dwBuff[2]) = 0;
    
	CALL_Tree_Add(hTree, NULL, dwBuff, sizeof(dwBuff));

    return error;
}

tBOOL DeleteObjFromTree(hi_OS _this, hTREE hTree, hOBJECT object)
{
tQWORD qwObjectId;
tDWORD dwBuff[3];
tTREENODE node;
tBOOL err;

	err=cFALSE;
    CALL_SYS_PropertyGet(object, NULL, ppOBJECT_TO_OPEN, &qwObjectId, sizeof(qwObjectId));

    *(tQWORD*)(&dwBuff[0]) = qwObjectId;
    *(tDWORD*)(&dwBuff[2]) = (tDWORD)object;

	CALL_Tree_Search(hTree, &node, dwBuff, sizeof(dwBuff));

	if(node)
	{
		CALL_Tree_Remove(hTree, node);
		err=cTRUE;
	}
	return err;
}

// End of Tree manipulation routines
///////////////////////////////////////////////////////////////////////////////


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, Internal (kernel called) interface method implementation )
// --- Interface "OS". Method "MsgReceive"
// Extended method comment
//   Receives message sent to the object or to the one of the object parents as broadcast
// Behaviour comment
//   
// Result comment
//   
// Parameter "msg_id":
//   Message identifier
// Parameter "param1":
//   First message parameter
// Parameter "param2":
//   Second message parameter
tERROR pr_call OS_MsgReceive( hi_OS _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tVOID* par_buf, tDWORD* par_buf_len )
//tERROR pr_call OS_MsgReceive( hi_OS _this, tDWORD msg_id, tDWORD param1, tDWORD param2 ) 
{
OS_Uni_Data* data;
tDWORD dwLen;
tERROR err;
tDWORD dwBuf[3];
/*
tTREENODE node;
tLISTNODE lnode;
tDWORD dwLenNew;
*/

	PR_TRACE_A0( _this, "Enter \"OS::MsgReceive\" method" );
	data=_this->data;
	if((msg_id != UNI_MSG_FILE_OPEN && msg_id != UNI_MSG_FILE_CLOSE) || msg_id == UNI_MSG_FILE_RW)
	{
		if(data->IsWritable == DONT_KNOW)
		{
			if(errOK == CALL_SYS_PropertySetDWord( (hOBJECT)data->hARC_IO, pgOBJECT_ACCESS_MODE, fACCESS_RW ))
				data->IsWritable = CAN_WRITE;
			else
				data->IsWritable = CANT_WRITE;
		}
		if(data->IsWritable == CANT_WRITE)
		{
			PR_TRACE_A1( _this, "Leave \"OS::MsgReceive\" method, ret tERROR = 0x%08x", errOBJECT_READ_ONLY );
			return errOBJECT_READ_ONLY;
		}
	}
	err = errOK;
	switch(msg_id)
	{
		case UNI_MSG_FILE_CLOSE_CHANGE:
			data->bRealChange = cTRUE;
		case UNI_MSG_FILE_CLOSE_KEEP_OPEN:
			
			if(!data->hChangeTree)
				data->hChangeTree = OpenTree(_this);
			if(data->hChangeTree)
				AddObjToTree(_this, data->hChangeTree, ctx);
			break;
		case UNI_MSG_FILE_DELETE_ON_CLOSE:
			if(data->hOpenTree)
				DeleteObjFromTree(_this, data->hOpenTree, ctx);
		case UNI_MSG_FILE_DELETE:
            /*
			switch(CALL_SYS_PropertyGetDWord(obj, pgINTERFACE_ID))
			{
				case IID_STRING:
					err = CALL_String_ExportToBuff( (hSTRING) obj, &dwLen, cSTRING_WHOLE, data->szNameBuffer, SizeOfNameBuffer, 0, cSTRING_Z);
					break;
				// !!! petrovitch 
				//case IID_OBJPTR:
				//	err = CALL_SYS_PropertyGet(((hObjEnum) obj), NULL, pgOBJECT_FULL_NAME, data->szNameBuffer, SizeOfNameBuffer);
				//	break;
				//
				default:
					err = CALL_SYS_PropertyGetStr(((hOBJECT) ctx), NULL, pgOBJECT_FULL_NAME, data->szNameBuffer, SizeOfNameBuffer, cCP_ANSI);
					break;
			}
			if(err != errOK)
				return err;
			dwLen = strlen(data->szNameBuffer)+1+sizeof(hOBJECT);

			if(data->hOpenTree)
			{
				if(PR_SUCC(CALL_Tree_Search(data->hOpenTree, NULL, data->szNameBuffer, dwLen)))
				{
					return errACCESS_DENIED;
				}
			}
			if(data->hRenamedList)
			{
// Check for New name
				lnode = iFindInList(_this, data->hRenamedList, NULL, data->szNameBuffer, 0);
				if(lnode)
				{
					CALL_List_Remove(data->hRenamedList, lnode);
					break;
				}
			}
            */

		case UNI_MSG_FILE_DELETE_NO_CHECK:
			if(!data->hDeletedTree)
				data->hDeletedTree = OpenTree(_this);
			if(data->hDeletedTree)
			{
				if(par_buf && *par_buf_len == sizeof(tQWORD))
				{
                    AddIdToTree(_this, data->hDeletedTree, *(tQWORD*)par_buf);
				}
				else
				{
				    AddObjToTree(_this, data->hDeletedTree, ctx);
				}
            }
            /*
			if(data->hNewTree)
			{
			tDWORD dwTmp;
				CALL_Tree_Search(data->hNewTree,&node , data->szNameBuffer , dwLen );
				while(node)
				{
					CALL_Tree_NodeDataGet(data->hNewTree, &dwTmp, node, data->szNameBuffer, SizeOfNameBuffer);
					if(dwTmp==dwLen)
					{
						CALL_Tree_Remove(data->hNewTree, node);
						if(PR_FAIL(CALL_Tree_Last(data->hNewTree, NULL)))
						{
							CALL_SYS_ObjectClose(data->hNewTree);
							data->hNewTree=NULL;
						}
						break;
					}
					CALL_Tree_Next(data->hNewTree, &node, node);
				}
			}
            */
			break;
		case UNI_MSG_FILE_NEW:
			if(!data->hNewTree)
				data->hNewTree=OpenTree(_this);
			if(data->hNewTree)
			{

				CALL_String_ExportToBuff( (hSTRING) ctx, &dwLen, cSTRING_WHOLE, data->szNameBuffer, SizeOfNameBuffer, 0, cSTRING_Z);

				CALL_SYS_PropertySetStr(obj, NULL, pgOBJECT_NAME, data->szNameBuffer, dwLen, cCP_ANSI);

                *(tQWORD*)(&dwBuf[0]) = data->qwMagiq++;
                *(tDWORD*)(&dwBuf[2]) = (tDWORD)obj;

				CALL_Tree_Add(data->hNewTree, NULL, dwBuf, sizeof(dwBuf));
			}
			break;
		case UNI_MSG_FILE_RENAME:
            /*
			// obj OldName
			// ctx NewName
			
			//Find At Open List
			//If present -- return error
			CALL_String_ExportToBuff( (hSTRING) obj, &dwLen, cSTRING_WHOLE, data->szNameBuffer, SizeOfNameBuffer, 0, cSTRING_Z);
			CALL_String_ExportToBuff( (hSTRING) ctx, &dwLenNew, cSTRING_WHOLE, data->szNewNameBuffer, SizeOfNameBuffer, 0, cSTRING_Z);
			if(data->hOpenTree)
			{
				if(PR_SUCC(CALL_Tree_Search(data->hOpenTree, NULL, data->szNameBuffer, dwLen)))
				{
					PR_TRACE_A1( _this, "Leave \"OS::MsgReceive\" method, ret tERROR = 0x%08x", errACCESS_DENIED );
					return errACCESS_DENIED;
				}
			}
			//Find in Deleted list
			//If present -- return error
			if(data->hDeletedTree)
			{
				if(PR_SUCC(CALL_Tree_Search(data->hDeletedTree, NULL, data->szNameBuffer, dwLen)))
				{
					PR_TRACE_A1( _this, "Leave \"OS::MsgReceive\" method, ret tERROR = 0x%08x", errOBJECT_NOT_FOUND );
					return errOBJECT_NOT_FOUND;
				}
			}
			// Check for Old Name -- may be file already renamed
			// In this case Old Name must be New name for One of the files
			lnode = iFindInList(_this, data->hRenamedList, NULL, data->szNameBuffer, 0);
			if(lnode)
			{
				tUINT uiDataSize;
				// Rename Renamed file
				// Just change Data in RenamedList
				uiDataSize = sizeof(tQWORD);
				*(tQWORD*)(data->szListData) = 0;
				
				memcpy(&data->szListData[uiDataSize], data->szNameBuffer, dwLen );
				uiDataSize += dwLen;
				data->szListData[uiDataSize] = 0;
				uiDataSize++;
				
				memcpy(&_this->data->szListData[uiDataSize], data->szNewNameBuffer, dwLenNew);
				uiDataSize += dwLenNew;
				data->szListData[uiDataSize] = 0;
				uiDataSize++;
				CALL_List_DataSet(data->hRenamedList, &lnode, lnode, data->szListData, uiDataSize);
				PR_TRACE_A1( _this, "Leave \"OS::MsgReceive\" method, ret tERROR = 0x%08x", errOK_DECIDED );
				return errOK_DECIDED;
			}
			
			iAddToList(_this, &data->hRenamedList, data->szNameBuffer, data->szNewNameBuffer, 0);
			
			//Add old name to deleted list 
			return OS_MsgReceive(_this, msg_clsBROADCAST, UNI_MSG_FILE_DELETE_NO_CHECK, obj, obj, 0, 0, 0);
            */
			break;
		case UNI_MSG_FILE_OPEN:
			if(!data->hOpenTree)
				data->hOpenTree=OpenTree(_this);
			if(data->hOpenTree)
				AddObjToTree(_this, data->hOpenTree, obj);
			break;
		case UNI_MSG_FILE_CLOSE:
			if(data->hOpenTree)
				DeleteObjFromTree(_this, data->hOpenTree, obj);
			break;
	}



	PR_TRACE_A1( _this, "Leave \"OS::MsgReceive\" method, ret tERROR = %u", errOK );
	return errOK_DECIDED;
}
// AVP Prague stamp end( OS, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------




// AVP Prague stamp begin( External (user called) interface method implementation, PtrCreate )
// Extended method comment
//  It responds for opening ObjPtr object by:
//    -- hString object holded folder name
//    -- hIO object's folder
//    -- another hObjPtr object (cloning)
//
// Behaviour comment
//  Must realise the same result as if client uses following three steps protocol:
//    -- call ObjectCreate system method and get new IO (Folder) object
//    -- dictate properties of new object
//    -- call ObjectCreateDone system method and have working object
//  Advise: when caller invokes ObjectCreateDone method OS object receives ChildDone notification and can do all necessary job to process it.
//  If parameter name is NULL must open *ROOT* folder
//
// Parameters are:
tERROR pr_call OS_PtrCreate( hi_OS _this, hObjPtr* result, hOBJECT name ) 
{
tERROR  error ;
hObjPtr ret_val;
	
	PR_TRACE_A0( _this, "Enter \"OS::PtrCreate\" method" );
	error = errOBJECT_INCOMPATIBLE;
	ret_val = NULL;
	
	if(name == NULL)
	{
		error = CALL_SYS_ObjectCreateQuick(_this, &ret_val,IID_OBJPTR, PID_UNIVERSAL_ARCHIVER, 0 );
	}
	else
	{
		switch(CALL_SYS_PropertyGetDWord(name, pgINTERFACE_ID))
		{
		case IID_OBJPTR:
			error = CALL_ObjPtr_Clone((hObjPtr)name, &ret_val);
			break;
		case IID_IO:
		case IID_STRING:
			error = CALL_SYS_ObjectCreateQuick(_this, &ret_val,IID_OBJPTR, PID_UNIVERSAL_ARCHIVER, 0 );
			if ( PR_SUCC(error) )
				error = CALL_ObjPtr_ChangeTo(ret_val, name);
			break;
		default:
			error = errOBJECT_INCOMPATIBLE;
			break;
		}
	}
	
	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave \"OS::PtrCreate\" method, ret hObjPtr = %p, error = 0x%08x", ret_val, error );
	return error;
}
// AVP Prague stamp end( OS, External (user called) interface method implementation )
// --------------------------------------------------------------------------------



// AVP Prague stamp begin( External (user called) interface method implementation, IOCreate )
// Extended method comment
//  It responds for opening IO object by:
//    -- hString held objects name
//    -- hIO object's name (cloning)
//    -- hObjPtr
//
// Behaviour comment
//  Must realise the same result as if client uses following three steps protocol:
//    -- call ObjectCreate system method and get new IO (Folder) object
//    -- dictate properties of new object
//    -- call ObjectCreateDone system method and have working object
//  Advise: when caller invokes ObjectCreateDone method OS object receives ChildDone notification and can do all necessary job to process it.
//
// Parameters are:
tERROR pr_call OS_IOCreate( hi_OS _this, hIO* result, hOBJECT name, tDWORD access_mode, tDWORD open_mode ) 
{
tERROR error;
hIO io;
tIID dwIID;
hObjPtr hPtr;
//tCHAR buffer[SizeOfNameBuffer];
//tDWORD dwLen;
	
	PR_TRACE_A0( _this, "Enter \"OS::IOCreate\" method" );
	
	dwIID = 0;
	if(name != NULL)
		dwIID = CALL_SYS_PropertyGetDWord(name, pgINTERFACE_ID);
	if(result == NULL || (dwIID != IID_IO && dwIID != IID_STRING))
	{
		PR_TRACE_A2( _this, "Leave \"OS::IOCreate\" method, ret hIO = %p, error = 0x%08x", NULL, errPARAMETER_INVALID );
		return errPARAMETER_INVALID;
	}
	io = NULL;
	error = errOK;
	
	if(open_mode & fOMODE_OPEN_IF_EXIST)
	{
		error = OS_PtrCreate(_this, &hPtr, name );
		if(hPtr && PR_SUCC(error))
		{
/*			CALL_ObjPtr_Reset(hPtr, cTRUE);
			CALL_String_ExportToBuff( (hSTRING) name, &dwLen, cSTRING_WHOLE, _this->data->szNameBuffer, SizeOfNameBuffer, 0, cSTRING_Z);
			
			while (PR_SUCC(error = CALL_ObjPtr_Next(hPtr)) )
			{
				error = CALL_SYS_PropertyGet(hPtr,0,pgOBJECT_NAME,buffer,sizeof(buffer));
				if(PR_SUCC (error) && 0 == strnicmp(buffer,_this->data->szNameBuffer,dwLen))
				{
					break;
				}
			}
			if(error != errOK)
			{
				error = errNOT_FOUND;
				goto loc_cont;
			}*/
			error = CALL_ObjPtr_IOCreate(hPtr, &io, NULL, access_mode, open_mode);
			if(open_mode & fOMODE_TRUNCATE && io != NULL)
			{
				error = CALL_IO_SetSize(io, 0);
			}
			goto loc_exit;
		}
	}
//loc_cont:
	if(open_mode & fOMODE_CREATE_IF_NOT_EXIST && io == NULL)
	{
		error = (tERROR)CALL_SYS_ObjectCreate((hOBJECT)_this, (hOBJECT*)&io, IID_IO, PID_TMPFILE,0);
		if(PR_SUCC(error))
		{
			CALL_SYS_PropertySetDWord( (hOBJECT)io, pgOBJECT_OPEN_MODE, open_mode );
			CALL_SYS_PropertySetDWord( (hOBJECT)io, pgOBJECT_ACCESS_MODE, access_mode );
			if(PR_SUCC(CALL_SYS_ObjectCreateDone( io)))
			{
				error = OS_MsgReceive(_this, msg_clsBROADCAST, UNI_MSG_FILE_NEW, (hOBJECT)io, (hOBJECT)name, 0, 0, 0);
                if (PR_SUCC(error))
                {
                    error = errOK;
                }
			}
			else
			{
				io = NULL;
				error = errOBJECT_NOT_CREATED;
			}
		}
	}
loc_exit:
	
	PR_TRACE_A2( _this, "Leave \"OS::IOCreate\" method, ret hIO = %p, error = 0x%08x", io, error );
	*result = io;
	return error;
}
// AVP Prague stamp end( OS, External (user called) interface method implementation )
// --------------------------------------------------------------------------------




// AVP Prague stamp begin( External (user called) interface method implementation, Copy )
// Parameters are:
tERROR pr_call OS_Copy( hi_OS _this, hOBJECT src_name, hOBJECT dst_name, tBOOL overwrite ) 
{
tERROR error ;
hIO hDstio, hSrcio;
tDWORD open_mode;
hSEQ_IO hSrcSeqIo, hDstSeqIo;
tBYTE* pBuffer;
tQWORD qwSize;
	
	PR_TRACE_A0( _this, "Enter \"OS::Copy\" method" );
	
	hDstio = NULL;
	hSrcio = NULL;
	if(overwrite)
		open_mode = fOMODE_CREATE_ALWAYS;
	else
		open_mode = fOMODE_CREATE_IF_NOT_EXIST;
	
	error = OS_IOCreate(_this, &hDstio, dst_name, fACCESS_RW, open_mode);
	if(PR_SUCC(error))
	{
		error = OS_IOCreate(_this, &hSrcio, src_name, fACCESS_READ, fOMODE_OPEN_IF_EXIST);
		if(PR_SUCC(error))
		{
			hSrcSeqIo = CreateSeqIO(hSrcio);
			hDstSeqIo = CreateSeqIO(hDstio);
			CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&pBuffer, 0x1000);
			if(hSrcSeqIo && hDstSeqIo && pBuffer)
			{
				CALL_SeqIO_GetSize(hSrcSeqIo, &qwSize, IO_SIZE_TYPE_EXPLICIT);
				CALL_SeqIO_SetSize(hDstSeqIo, qwSize);
				error = uCopyFile(hSrcSeqIo, hDstSeqIo, pBuffer, 0x1000);
			}
			if(pBuffer)
				CALL_SYS_ObjHeapFree(_this, pBuffer);
//			error = OS_MsgReceive(_this, msg_clsBROADCAST, UNI_MSG_FILE_NEW, (hOBJECT)hDstio, (hOBJECT)dst_name, 0, 0, 0);
		}
	}
	if(hSrcio)
	{
		CALL_SYS_ObjectClose(hSrcio);
	}
	PR_TRACE_A1( _this, "Leave \"OS::Copy\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Rename )
// Extended method comment
//  Renames/moves object by name inside single OS
//  Must move object through folders if nessesary
// Parameters are:
// "new_name"  : Target name
tERROR pr_call OS_Rename( hi_OS _this, hOBJECT old_name, hOBJECT new_name, tBOOL overwrite ) 
{
tERROR error;
	
	PR_TRACE_A0( _this, "Enter \"OS::Rename\" method" );
	error = OS_MsgReceive(_this, msg_clsBROADCAST, UNI_MSG_FILE_RENAME, (hOBJECT)old_name, (hOBJECT)new_name, 0, 0, 0);
	
	PR_TRACE_A1( _this, "Leave \"OS::Rename\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, External (user called) interface method implementation )
// --- Interface "OS". Method "Delete"
// Extended method comment
//   phisically deletes an entity on OS
// Behaviour comment
//   if parameter "object" is really hIO or hObjEnum must close it and delete after close.
// Parameter "object":
//   name of the object to delete may be addressed by:
//     -- hString
//     -- hIO
//     -- hObjEnum
//   if it is hIO or hFolder it must be closed before deleting
tERROR pr_call OS_Delete( hi_OS _this, hOBJECT name ) 
{
	tERROR error;
    hObjPtr hPtr;
	
	PR_TRACE_A0( _this, "Enter \"OS::Delete\" method" );

    if (PR_FAIL(error = OS_PtrCreate(_this, &hPtr, name)))
    {
        return error;
    }

    error = CALL_ObjPtr_Delete(hPtr);

	PR_TRACE_A1( _this, "Leave \"OS::Delete\" method, ret tERROR = 0x%08x", error );
	return error;
	
}
// AVP Prague stamp end( OS, External (user called) interface method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( OS, plugin definitions )
#include <stdarg.h>
PR_MAKE_TRACE_FUNC;

hROOT g_root = NULL;

tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* error ) 
{
	switch( dwReason ) 
	{
		case DLL_PROCESS_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_ATTACH :
		case DLL_THREAD_DETACH : 
			break;
			
		case PRAGUE_PLUGIN_LOAD :
			// register my interfaces
			g_root = (hROOT)hInstance;
			if ( PR_FAIL(*error=CALL_Root_ResolveImportTable( g_root,0,import_table,PID_UNIVERSAL_ARCHIVER)))
				return cFALSE;
			if ( PR_FAIL(*error=CALL_Root_RegisterCustomPropId( g_root ,(tDWORD*)&MyPropID, cUNIARC_PROPERTY, pTYPE_PID)))
				return cFALSE;
			if ( PR_FAIL(*error=OS_Register( g_root ) ))
				return cFALSE;
			if ( PR_FAIL(*error=ObjPtr_Register( g_root ) ))
				return cFALSE;
			if ( PR_FAIL(*error=IO_Register( g_root ) ))
				return cFALSE;
			if ( PR_FAIL(*error=UniArchiverCallback_Register( g_root ) ))
				return cFALSE;
			
			// register my custom property ids
			// register my exports
			// resolve  my imports
			// use some system resources
			break;
			
		case PRAGUE_PLUGIN_UNLOAD :
			// free system resources
			// unregister my custom property ids -- you can drop it, kernel do it by itself
			// release    my imports		         -- you can drop it, kernel do it by itself
			// unregister my exports		         -- you can drop it, kernel do it by itself
			// unregister my interfaces          -- you can drop it, kernel do it by itself
			g_root = NULL;
			break;
	}
	return cTRUE;
}
// AVP Prague stamp end( OS, plugin definitions )
// --------------------------------------------------------------------------------


#define cSizeOfBuffer 0x1000

tERROR RebuildArchive(hi_OS _this)
{
tERROR error;
hUNIARCCALLBACK hCallback;
hIO hTempIO;
hIO hNewArcIO;
hSEQ_IO hOldArcSeqIO;
hSEQ_IO hNewArcSeqIO;
hOS hParentOS;
tBYTE* Buffer;
tQWORD qwSize;

	hNewArcSeqIO = NULL;
	hOldArcSeqIO = NULL;
	hNewArcIO = NULL;
	error = CALL_SYS_ObjectCreate((hOBJECT)_this, &hCallback, IID_UNIARCHIVERCALLBACK, PID_UNIVERSAL_ARCHIVER,0);
	if(PR_SUCC(error))
	{
		error = CALL_SYS_ObjectCreateDone(hCallback);
	}
	if(error!=errOK)
		return error;

	error = CALL_SYS_ObjectCreate((hOBJECT)_this, &hTempIO, IID_IO, PID_TMPFILE, 0);
	if(PR_SUCC(error))
	{
		CALL_SYS_PropertySetDWord( (hOBJECT)hTempIO, pgOBJECT_OPEN_MODE, fOMODE_CREATE_ALWAYS + fOMODE_SHARE_DENY_READ);
		CALL_SYS_PropertySetDWord( (hOBJECT)hTempIO, pgOBJECT_ACCESS_MODE, fACCESS_RW);
		error = CALL_SYS_ObjectCreateDone(hCallback);
	}
	if(error!=errOK)
		return error;

	error = CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&Buffer, cSizeOfBuffer);
	if(PR_FAIL(error))
	{
		return error;
	}

	if(error != errOK)
		return error;

	hParentOS = (hOS)CALL_SYS_ParentGet(_this, IID_OS);

	error = CALL_SYS_ObjectCreate(_this,(hOBJECT*)&hNewArcIO, IID_IO, PID_TMPFILE,0);
	if(PR_SUCC(error))
	{
		CALL_SYS_PropertySetDWord( (hOBJECT)hNewArcIO, pgOBJECT_ACCESS_MODE, fACCESS_RW);
		CALL_SYS_PropertySetDWord( (hOBJECT)hNewArcIO, pgOBJECT_OPEN_MODE,   fOMODE_CREATE_ALWAYS);// + fOMODE_SHARE_DENY_READ);
		error = CALL_SYS_ObjectCreateDone( hNewArcIO);
	}
	
	if(hNewArcIO == NULL)
		return error;


// Call to Mini Archiver;
	error = CALL_MiniArchiver_RebuildArchive(_this->data->hArc, hCallback, hNewArcIO);

	if(PR_SUCC(error))
	{

	//Reopen Original file

		CALL_SYS_PropertySetDWord( (hOBJECT)_this->data->hARC_IO, pgOBJECT_ACCESS_MODE, fACCESS_RW);

		hNewArcSeqIO = CreateSeqIO(hNewArcIO);
		hOldArcSeqIO = CreateSeqIO(_this->data->hARC_IO);
		
		if(!hNewArcSeqIO || !hOldArcSeqIO)
			return errOBJECT_NOT_CREATED;
		
	//Truncate original file to 0
		CALL_SeqIO_SetSize(hOldArcSeqIO, 0);
		CALL_SeqIO_GetSize(hNewArcSeqIO, &qwSize, IO_SIZE_TYPE_EXPLICIT);
		CALL_SeqIO_SetSize(hOldArcSeqIO, qwSize);

	//write tmp file to original archive 
		CALL_SeqIO_Seek(hOldArcSeqIO, NULL, 0, cSEEK_SET);
		uCopyFile(hNewArcSeqIO, hOldArcSeqIO, Buffer, cSizeOfBuffer);
		CALL_SeqIO_Flush(hOldArcSeqIO);
	}

	//close
	if(hNewArcIO)
	{
		if(hNewArcSeqIO)
			CALL_SYS_ObjectClose(hNewArcSeqIO);
		CALL_SYS_ObjectClose(hNewArcIO);
	}
	if(hOldArcSeqIO)
		CALL_SYS_ObjectClose(hOldArcSeqIO);

	return error;
}



hSEQ_IO CreateSeqIO(hIO hIO)
{
hSEQ_IO hSeqIo;

	CALL_SYS_ObjectCreate((hOBJECT)hIO,&hSeqIo,IID_SEQIO, PID_ANY,0);
	if(hSeqIo)
	{
		if(PR_FAIL(CALL_SYS_ObjectCreateDone( hSeqIo)))
		{
			CALL_SYS_ObjectClose(hSeqIo);
			hSeqIo=NULL;
		}
	}
	return hSeqIo;
}


tERROR uCopyFile(hSEQ_IO hSrc, hSEQ_IO Dest, tBYTE* Buffer, tDWORD dwBufferSize)
{
tDWORD read;
tERROR error;

	error = CALL_SeqIO_Read(hSrc, &read, Buffer,dwBufferSize);
	while(read != 0 )
	{
		error = CALL_SeqIO_Write(Dest,NULL, Buffer, read);
		if( PR_FAIL(error ))
			break;
		error = CALL_SeqIO_Read(hSrc,&read, Buffer,dwBufferSize);
	}
	if(error == errEOF)
		error = errOK;
	return error;
}

tLISTNODE iFindInList(hi_OS _this, hLIST hList, tCHAR* szOldName, tCHAR* szNewName, tQWORD qwMagiq)
{
tLISTNODE node;
tUINT uiDataSize;
	
	node = 0;
	if(_this->data->szListData == NULL)
	{
		CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&_this->data->szListData, SizeOfNameBuffer*2 + sizeof(tQWORD));
	}
	if(hList && _this->data->szListData)
	{
		CALL_List_First(hList, &node);
		while(node)
		{
			CALL_List_DataGet(hList, &uiDataSize, node, _this->data->szListData, SizeOfNameBuffer*2 + sizeof(tQWORD));
			if(uiDataSize != 0)
			{
				if(szOldName)
				{
					if(strcmp(&_this->data->szListData[sizeof(tQWORD)], szOldName) == 0)
					{
						break;
					}
				}
				if(szNewName)
				{
					tUINT uiPos;
					
					uiPos = _toui32(sizeof(tQWORD) + strlen(&_this->data->szListData[sizeof(tQWORD)])) ;
					if(strcmp(&_this->data->szListData[uiPos+1], szNewName) == 0)
					{
						break;
					}
				}
			}
			CALL_List_Next(hList, &node, node);
		}
	}
	return node;
}

void iAddToList(hi_OS _this, hLIST* pList, tCHAR* szOldName, tCHAR* szNewName, tQWORD qwMagiq)
{
tERROR error;
tLISTNODE node;
tUINT uiDataSize;
tUINT uiTmp;
	
	error = errOK;
	if(*pList == NULL)
	{
		error=(tERROR)CALL_SYS_ObjectCreate((hOBJECT)_this, (hOBJECT*)pList, IID_LIST, PID_ANY, 0);
		if(PR_SUCC(error))
		{
			CALL_SYS_PropertySetDWord(*pList, pgLIST_GRANULARITY, (SizeOfNameBuffer*2+sizeof(tQWORD))*4);
			CALL_SYS_PropertySetDWord(*pList, pgLIST_NODE_SIZE, (SizeOfNameBuffer*2+sizeof(tQWORD)));
			error=CALL_SYS_ObjectCreateDone(*pList);
		}
	}
	if(_this->data->szListData == NULL && PR_SUCC(error))
	{
		error = CALL_SYS_ObjHeapAlloc(_this, (tPTR*)&_this->data->szListData, SizeOfNameBuffer*2 + sizeof(tQWORD));
	}
	if(PR_SUCC(error))
	{
		uiDataSize = sizeof(tQWORD);
		*(tQWORD*)(_this->data->szListData) = qwMagiq;
		
		uiTmp = _toui32(strlen(szOldName));
		memcpy(&_this->data->szListData[uiDataSize], szOldName, uiTmp );
		uiDataSize += uiTmp;
		_this->data->szListData[uiDataSize] = 0;
		uiDataSize++;
		
		uiTmp = _toui32(strlen(szNewName));
		memcpy(&_this->data->szListData[uiDataSize], szNewName, uiTmp);
		uiDataSize += uiTmp;
		_this->data->szListData[uiDataSize] = 0;
		uiDataSize++;
		
		error = CALL_List_Add(*pList, &node, _this->data->szListData, uiDataSize, 0, cLIST_LAST); 
	}
}


static void __cdecl xtoa (unsigned long val, char *buf, unsigned radix, int is_neg)
{
char *p;                /* pointer to traverse string */
char *firstdig;         /* pointer to first digit */
char temp;              /* temp char */
unsigned digval;        /* value of digit */

	p = buf;

	if (is_neg) 
	{
		/* negative, so output '-' and negate */
		*p++ = '-';
		val = (unsigned long)(-(long)val);
	}

	firstdig = p;           /* save pointer to first digit */

	do 
	{
		digval = (unsigned) (val % radix);
		val /= radix;       /* get next digit */

		/* convert to ascii and store */
		if (digval > 9)
			*p++ = (char) (digval - 10 + 'a');  /* a letter */
		else
			*p++ = (char) (digval + '0');       /* a digit */
	} while (val > 0);

	/* We now have the digit of the number in the buffer, but in reverse
	order.  Thus we reverse them now. */

	*p-- = '\0';            /* terminate string; p points to last digit */

	do 
	{
		temp = *p;
		*p = *firstdig;
		*firstdig = temp;   /* swap *p and *firstdig */
		--p;
		++firstdig;         /* advance to next two digits */
	} while (firstdig < p); /* repeat until halfway */
}

/* Actual functions just call conversion helper with neg flag set correctly,
   and return pointer to buffer. */

char * itoa (int val, char *buf, int radix)
{
	if (radix == 10 && val < 0)
		xtoa((unsigned long)val, buf, radix, 1);
	else
		xtoa((unsigned long)(unsigned int)val, buf, radix, 0);
	return buf;
}
/*
char * __cdecl ltoa (long val, char *buf, int radix)
{
	xtoa((unsigned long)val, buf, radix, (radix == 10 && val < 0));
	return buf;
}

char * __cdecl ultoa (unsigned long val, char *buf, int radix)
{
	xtoa(val, buf, radix, 0);
	return buf;
}
*/
