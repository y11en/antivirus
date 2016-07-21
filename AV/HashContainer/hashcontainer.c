// AVP Prague stamp begin( Interface header,  )
// -------- Thursday,  03 October 2002,  14:45 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Nikishin
// File Name   -- hashcontainer.c
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define HashContainer_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include <Extract/plugin/p_iwgen.h>

#include "hashcontainer.h"
#include "HCCOMPARE/compare.h"

#include <m_utils.h>
// AVP Prague stamp end




// AVP Prague stamp begin( Interface comment,  )
// HashContainer interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface HashContainer. Inner data structure

typedef struct tag_HashContainerData 
{
	hTREE hTree; // --
} HashContainerData;

// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_HashContainer 
{
	const iHashContainerVtbl* vtbl; // pointer to the "HashContainer" virtual table
	const iSYSTEMVTBL*        sys;  // pointer to the "SYSTEM" virtual table
	HashContainerData*        data; // pointer to the "HashContainer" data structure
} *hi_HashContainer;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call HashContainer_ObjectInitDone( hi_HashContainer _this );
// AVP Prague stamp end



tERROR OpenTree(hi_HashContainer _this, hTREE* hTree);
tERROR AddToTree(hi_HashContainer _this, hTREE hTree, tQWORD qwHash, tPTR pData, tDWORD dwSizeOfData);


// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_HashContainer_vtbl = 
{
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       NULL,
	(tIntFnObjectInitDone)   HashContainer_ObjectInitDone,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   NULL,
	(tIntFnObjectClose)      NULL,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       NULL,
	(tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpHashContainer_GetHash) ( hi_HashContainer _this, tQWORD qwHash, tPTR* ppData ); // -- Hash Value;
typedef   tERROR (pr_call *fnpHashContainer_SetHash) ( hi_HashContainer _this, tQWORD qwHash, tPTR pData ); // -- ;
typedef   tERROR (pr_call *fnpHashContainer_DeleteHash) ( hi_HashContainer _this, tQWORD qwHash); // -- ;
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iHashContainerVtbl 
{
	fnpHashContainer_GetHash  GetHash;
	fnpHashContainer_SetHash  SetHash;
	fnpHashContainer_DeleteHash DeleteHash;
}; // "HashContainer" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call HashContainer_GetHash( hi_HashContainer _this, tQWORD qwHash, tPTR* ppData);
tERROR pr_call HashContainer_SetHash( hi_HashContainer _this, tQWORD qwHash, tPTR pData);
tERROR pr_call HashContainer_DeleteHash( hi_HashContainer _this, tQWORD qwHash);
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
static iHashContainerVtbl e_HashContainer_vtbl = 
{
	HashContainer_GetHash,
	HashContainer_SetHash,
	HashContainer_DeleteHash
};
// AVP Prague stamp end



// AVP Prague stamp begin( Forwarded property methods declarations,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "HashContainer". Static(shared) property table variables
const tVERSION dwVersion = 1; // must be READ_ONLY at runtime
const tSTRING szComment = "Hash container"; // must be READ_ONLY at runtime
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(HashContainer_PropTable)
	mSHARED_PROPERTY_VAR( pgINTERFACE_VERSION, dwVersion, sizeof(dwVersion) )
	mSHARED_PROPERTY_VAR( pgINTERFACE_COMMENT, szComment, 14 )
mPROPERTY_TABLE_END(HashContainer_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "HashContainer". Register function
tERROR pr_call HashContainer_Register( hROOT root ) 
{
	tERROR error;

	PR_TRACE_A0( root, "Enter HashContainer::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_HASHCONTAINER,                      // interface identifier
		PID_HASHCONT,                           // plugin identifier
		0x00000000,                             // subtype identifier
		dwVersion,                              // interface version
		VID_ANDY,                               // interface developer
		&i_HashContainer_vtbl,                  // internal(kernel called) function table
		(sizeof(i_HashContainer_vtbl)/sizeof(tPTR)),// internal function table size
		&e_HashContainer_vtbl,                  // external function table
		(sizeof(e_HashContainer_vtbl)/sizeof(tPTR)),// external function table size
		HashContainer_PropTable,                // property table
		mPROPERTY_TABLE_SIZE(HashContainer_PropTable),// property table size
		sizeof(HashContainerData),              // memory size
		IFACE_SYSTEM                            // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"HashContainer(IID_HASHCONTAINER) registered [error=0x%08x]",error) );
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave HashContainer::Register method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call HashContainer_ObjectInitDone( hi_HashContainer _this )
{
tERROR error;

	PR_TRACE_A0( _this, "Enter HashContainer::ObjectInitDone method" );

	// Place your code here
	error = OpenTree(_this, &_this->data->hTree)

	PR_TRACE_A1( _this, "Leave HashContainer::ObjectInitDone method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetHash )
// Parameters are:
tERROR pr_call HashContainer_GetHash( hi_HashContainer _this, tQWORD qwHash, tPTR* ppData)
{
tERROR error;
tTREENODE node;
tDWORD res;
tBYTE byData[sizeof(tQWORD) + sizeof(tPTR)];

	PR_TRACE_A0( _this, "Enter HashContainer::GetHash method" );

	error = CALL_Tree_Search(_this->data->hTree, &node, &qwHash, sizeof(qwHash));
	if(PR_SUCC(error) && ppData != NULL )
	{
		error = CALL_Tree_NodeDataGet(_this->data->hTree, &res, node, byData, sizeof(byData) );
		*ppData = *(tPTR*)(&byData[sizeof(qwHash)]);
		//memcpy(ppData, &byData[sizeof(qwHash)], sizeof(tPTR));
	}

	PR_TRACE_A1( _this, "Leave HashContainer::GetHash method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetHash )
// Parameters are:
tERROR pr_call HashContainer_SetHash( hi_HashContainer _this, tQWORD qwHash, tPTR pData)
{
tERROR error ;
tTREENODE node;
tBYTE byData[sizeof(tQWORD) + sizeof(tPTR)];

	PR_TRACE_A0( _this, "Enter HashContainer::SetHash method" );
	*((tQWORD*)byData) = qwHash;
//	memcpy(byData, &qwHash, sizeof(qwHash));
	*((tPTR*)&byData[sizeof(qwHash)]) = pData;
//	memcpy(&byData[sizeof(qwHash)], &pData, sizeof(pData));

	error = CALL_Tree_Add(_this->data->hTree, &node, byData, sizeof(byData));
	
	PR_TRACE_A1( _this, "Leave HashContainer::SetHash method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end

tERROR pr_call HashContainer_DeleteHash( hi_HashContainer _this, tQWORD qwHash)
{
tERROR error ;
tTREENODE node;
	
	PR_TRACE_A0( _this, "Enter HashContainer::DeleteHash method" );
	
	error = CALL_Tree_Search(_this->data->hTree, &node, &qwHash, sizeof(qwHash));
	if(PR_SUCC(error) )
	{
		error = CALL_Tree_Remove(_this->data->hTree, node);
	}
	
	PR_TRACE_A1( _this, "Leave HashContainer::DeleteHash method, ret tERROR = 0x%08x", error );
	return error;
}



tERROR OpenTree(hi_HashContainer _this, hTREE* hTree)
{
tERROR error;
hCOMPARE hComp;
//tUINT uiNodeSize;

	error = (tERROR)CALL_SYS_ObjectCreate((hOBJECT)_this, (hOBJECT*)hTree, IID_TREE, PID_ANY, 0);

	if(error == errOK)
	{
		CALL_SYS_PropertySetDWord(*hTree, pgTREE_GRANULARITY, 512);
		
		error = (tERROR)CALL_SYS_ObjectCreate((hOBJECT)_this, &hComp, IID_COMPARE, PID_HASHCONT_COMPARE, 0);
		if(errOK == error )
		{
			if(errOK == CALL_SYS_ObjectCreateDone(hComp))
				CALL_SYS_PropertySet(*hTree, NULL, pgTREE_COMPARE_OBJECT, &hComp, sizeof(hCOMPARE));
		}
		error = CALL_SYS_ObjectCreateDone(*hTree);
	}
	return error;
}
