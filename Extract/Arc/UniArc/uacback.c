// -------- Friday,  22 December 2000,  11:31 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2000.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andy Nikishin
// File Name   -- uacback.c
// -------------------------------------------



#define UniArchiverCallback_PRIVATE_DEFINITION

#include <Prague/prague.h>
#include <Prague/iface/i_heap.h>

#include <Extract/iface/i_uacall.h>
#include <Extract/plugin/p_miniarc.h>

#include "os.h"
#include "uacback.h"

#include <string.h>
#include <memory.h>



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, Interface comment )
// --------------------------------------------
// --- ee6a23e6_6292_4a74_bf65_60818b3fbe9f ---
// --------------------------------------------
// UniArchiverCallback interface implementation
// Short comments -- call Back interface to Uni Archiver to get access to internal lists
// AVP Prague stamp end( UniArchiverCallback, Interface comment )
// --------------------------------------------------------------------------------


#define cSizeOfBuffer 1024

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, Data structure )
// Interface UniArchiverCallback. Inner data structure
typedef struct tag_UACallBackData 
{
	OS_Uni_Data* OSdata; // Pointer to internal data of OS UniArc
} UACallBackData;
// AVP Prague stamp end( UniArchiverCallback, Data structure )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, object declaration )
typedef struct tag_hi_UniArchiverCallback 
{
	const iUniArchiverCallbackVtbl* vtbl; // pointer to the "UniArchiverCallback" virtual table
	const iSYSTEMVTBL*              sys;  // pointer to the "SYSTEM" virtual table
	UACallBackData*                data;   // pointer to the "UniArchiverCallback" data structure
} *hi_UniArchiverCallback;
// AVP Prague stamp end( UniArchiverCallback, object declaration )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, Internal method table prototypes )
// Interface "UniArchiverCallback". Internal method table. Forward declarations
tERROR pr_call UniArchiverCallback_ObjectInit( hi_UniArchiverCallback _this );
// AVP Prague stamp end( UniArchiverCallback, Internal method table prototypes )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, Internal method table )
// Interface "UniArchiverCallback". Internal method table.
static iINTERNAL i_UniArchiverCallback_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       UniArchiverCallback_ObjectInit,
	(tIntFnObjectInitDone)   NULL,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   NULL,
	(tIntFnObjectClose)      NULL,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       NULL,
	(tIntFnIFaceTransfer)    NULL 
};
// AVP Prague stamp end( UniArchiverCallback, Internal method table )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, interface function forward declarations )
typedef   tERROR (pr_call *fnpUniArchiverCallback_FindObjectInList)  ( hi_UniArchiverCallback _this, hOBJECT* result, tDWORD dwTable, tQWORD qwObjectId ); // -- ;
typedef   tERROR (pr_call *fnpUniArchiverCallback_GetFirstNewObject) ( hi_UniArchiverCallback _this, hNEWOBJECT* result );             // -- ;
typedef   tERROR (pr_call *fnpUniArchiverCallback_GetNextNewObject)  ( hi_UniArchiverCallback _this, hNEWOBJECT* result); // -- ;
typedef   tERROR (pr_call *fnpUniArchiverCallback_GetNewObjectIo)    ( hi_UniArchiverCallback _this, hIO* result, hNEWOBJECT node ); // -- ;
// AVP Prague stamp end( UniArchiverCallback, interface function forward declarations )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, interface declaration )
struct iUniArchiverCallbackVtbl  
{
	fnpUniArchiverCallback_FindObjectInList   FindObjectInList;
	fnpUniArchiverCallback_GetFirstNewObject  GetFirstNewObject;
	fnpUniArchiverCallback_GetNextNewObject   GetNextNewObject;
	fnpUniArchiverCallback_GetNewObjectIo     GetNewObjectIo;
}; // UniArchiverCallback
// AVP Prague stamp end( UniArchiverCallback, interface declaration )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, External method table prototypes )
// Interface "UniArchiverCallback". External method table. Forward declarations
tERROR pr_call UniArchiverCallback_FindObjectInList( hi_UniArchiverCallback _this, hOBJECT* result, tDWORD dwTable, tQWORD qwObjectId );
tERROR pr_call UniArchiverCallback_GetFirstNewObject( hi_UniArchiverCallback _this, hNEWOBJECT* result );
tERROR pr_call UniArchiverCallback_GetNextNewObject( hi_UniArchiverCallback _this, hNEWOBJECT* result);
tERROR pr_call UniArchiverCallback_GetNewObjectIo( hi_UniArchiverCallback _this, hIO* result, hNEWOBJECT node );
// AVP Prague stamp end( UniArchiverCallback, External method table prototypes )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, External method table )
// Interface "UniArchiverCallback". External method table.
static iUniArchiverCallbackVtbl e_UniArchiverCallback_vtbl = 
{
	UniArchiverCallback_FindObjectInList,
	UniArchiverCallback_GetFirstNewObject,
	UniArchiverCallback_GetNextNewObject,
	UniArchiverCallback_GetNewObjectIo
};
// AVP Prague stamp end( UniArchiverCallback, External method table )
// --------------------------------------------------------------------------------





// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, Forwarded property methods declarations )
// AVP Prague stamp end( UniArchiverCallback, Forwarded property methods declarations )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, Global property variable declaration )
// Interface "UniArchiverCallback". Global(shared) property table variables
const tVERSION dwVersion = 1; // must be READ_ONLY at runtime
const tSTRING szComment = "CallBack Interfase for UniArchiver"; // must be READ_ONLY at runtime
// AVP Prague stamp end( UniArchiverCallback, Global property variable declaration )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, Property table )
// Interface "UniArchiverCallback". Property table
mPROPERTY_TABLE(UniArchiverCallback_PropTable)
	mSHARED_PROPERTY_PTR( pgINTERFACE_VERSION, dwVersion, sizeof(dwVersion) )
	mSHARED_PROPERTY_VAR( pgINTERFACE_COMMENT, szComment, 35 )
mPROPERTY_TABLE_END(UniArchiverCallback_PropTable)
// AVP Prague stamp end( UniArchiverCallback, Property table )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, Registration call )
// Interface "UniArchiverCallback". Register function
tERROR pr_call UniArchiverCallback_Register( hROOT root ) 
{
	tERROR error;

	PR_TRACE_A0( root, "Enter \"UniArchiverCallback::Register\" method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_UNIARCHIVERCALLBACK,                // interface identifier
		PID_UNIVERSAL_ARCHIVER,                 // plugin identifier
		0x00000000,                             // subtype identifier
		0x00000001,                             // interface version
		VID_ANDY,                               // interface developer
		&i_UniArchiverCallback_vtbl,            // internal(kernel called) function table
		(sizeof(i_UniArchiverCallback_vtbl)/sizeof(tPTR)),// internal function table size
		&e_UniArchiverCallback_vtbl,            // external function table
		(sizeof(e_UniArchiverCallback_vtbl)/sizeof(tPTR)),// external function table size
		UniArchiverCallback_PropTable,          // property table
		mPROPERTY_TABLE_SIZE(UniArchiverCallback_PropTable),// property table size
		sizeof(UACallBackData),                 // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( error != errOK )
			PR_TRACE( (root,prtDANGER,"UniArchiverCallback(IID_UNIARCHIVERCALLBACK) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave \"UniArchiverCallback::Register\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end( UniArchiverCallback, Registration call )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, Internal (kernel called) interface method implementation )
// --- Interface "UniArchiverCallback". Method "ObjectInit"
// Extended method comment
//   Kernel notifies an object about successful creating of object
// Behaviour comment
//   Initializes internal object data
// Result comment
//   Returns value differ from errOK if object cannot be initialized

typedef struct tag_hi_Uni_OS 
{
	const iOSVtbl*     vtbl; // pointer to the "OS" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	OS_Uni_Data*       data; // pointer to the "OS" data structure
} *hi_Uni_OS;

tERROR pr_call UniArchiverCallback_ObjectInit( hi_UniArchiverCallback _this ) 
{
UACallBackData* data;
tERROR error;
hOBJECT hObject;

	error = errINTERFACE_INCOMPATIBLE;
	PR_TRACE_A0( _this, "Enter \"UniArchiverCallback::ObjectInit\" method" );

	hObject = CALL_SYS_ParentGet((hOBJECT)_this,IID_ANY);
	if(hObject != NULL)
	{
		if(PID_UNIVERSAL_ARCHIVER==CALL_SYS_PropertyGetDWord(hObject,pgPLUGIN_ID ))
		{

			error = errOK;
			data=_this->data;
			data->OSdata = ((hi_Uni_OS)hObject)->data;
		}
	}

	PR_TRACE_A1( _this, "Leave \"UniArchiverCallback::ObjectInit\" method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end( UniArchiverCallback, Internal (kernel called) interface method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, External (user called) interface method implementation )
// --- Interface "UniArchiverCallback". Method "FindObjectInList"
tERROR pr_call UniArchiverCallback_FindObjectInList( hi_UniArchiverCallback _this, hOBJECT* io, tDWORD dwTable, tQWORD qwObjectId ) 
{
hTREE hTree;
tTREENODE node;
tERROR error;
tDWORD dwBuf[3];

	if(io == NULL)
	{
		return errPARAMETER_INVALID;
	}

	error = errOBJECT_NOT_FOUND;
	hTree = NULL;
	*io = NULL;

    *(tQWORD*)&dwBuf[0] = qwObjectId;

	switch(dwTable)
	{
		case cCHANGED_OBJECT:
			hTree=_this->data->OSdata->hChangeTree;
			break;
		case cDELETED_OBJECT:
			hTree=_this->data->OSdata->hDeletedTree;
			break;
	}
	if(hTree)
	{
		CALL_Tree_Search(hTree, &node, dwBuf, sizeof(dwBuf) );
		if(node)
		{
			CALL_Tree_NodeDataGet(hTree, NULL, node, dwBuf, sizeof(dwBuf));

			*io=(hOBJECT)*((tDWORD*) &dwBuf[2]);
			error = errOK;
		}
	}
	return error;
}
// AVP Prague stamp end( UniArchiverCallback, External (user called) interface method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, External (user called) interface method implementation )
// --- Interface "UniArchiverCallback". Method "GetFirstNewObject"
tERROR pr_call UniArchiverCallback_GetFirstNewObject( hi_UniArchiverCallback _this, hNEWOBJECT* node ) 
{
tERROR error;
hTREE hNewTree;

	if(node == NULL)
	{
		return errPARAMETER_INVALID;
	}
	error = errOBJECT_NOT_FOUND;
	*node  = 0;
	hNewTree = _this->data->OSdata->hNewTree;
	if(hNewTree)
	{
		CALL_Tree_First(hNewTree, node);
		if(*node)
		{
			error = errOK;
		}
	}
	return error;
}
// AVP Prague stamp end( UniArchiverCallback, External (user called) interface method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, External (user called) interface method implementation )
// --- Interface "UniArchiverCallback". Method "GetNextNewObject"
tERROR pr_call UniArchiverCallback_GetNextNewObject( hi_UniArchiverCallback _this, hNEWOBJECT* node) 
{
tERROR error;
hTREE hNewTree;

	if(node == NULL)
	{
		return errPARAMETER_INVALID;
	}
	error = errOBJECT_NOT_FOUND;
	hNewTree = _this->data->OSdata->hNewTree;

    if(hNewTree)
    {
	    CALL_Tree_Next(hNewTree, node, *node);
	    if(*node)
	    {
			error = errOK;
	    }
    }
	return error;
}
// AVP Prague stamp end( UniArchiverCallback, External (user called) interface method implementation )
// --------------------------------------------------------------------------------



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( UniArchiverCallback, External (user called) interface method implementation )
// --- Interface "UniArchiverCallback". Method "GetNewObjectIo"
tERROR pr_call UniArchiverCallback_GetNewObjectIo( hi_UniArchiverCallback _this, hIO* io, hNEWOBJECT node ) 
{
tDWORD dwBuf[3];

	if(io == NULL && node == 0)
	{
		return errPARAMETER_INVALID;
	}
	if(io)
		*io = NULL;
	CALL_Tree_NodeDataGet(_this->data->OSdata->hNewTree, NULL, node, dwBuf, sizeof(dwBuf));
	if(io)
		*io=(hIO)*((tDWORD*) &dwBuf[2]);
	return errOK;
}
// AVP Prague stamp end( UniArchiverCallback, External (user called) interface method implementation )
// --------------------------------------------------------------------------------



