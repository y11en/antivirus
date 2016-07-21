// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 1.0 --------
// -------- Sunday,  26 September 2004,  23:20 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Kernel interfaces
// Sub project -- Not defined
// Purpose     -- Kernel interfaces
// Author      -- petrovitch
// File Name   -- receiver.c
// Additional info
//    Kernel interface implementations
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define MsgReceiver_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include "receiver.h"
// AVP Prague stamp end



#include <Prague/iface/i_plugin.h>
#include <Prague/iface/i_iface.h>
#include "kernel.h"


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// MsgReceiver interface implementation
// Short comments -- special object for the registering as message receiver
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface MsgReceiver. Inner data structure

typedef struct tag_MsgRecvData {
	tIntFnMsgReceive recv_proc; // user procedure for receive message
  tPTR             client_id; // client_id
} MsgRecvData;

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_MsgReceiver {
	const iMsgReceiverVtbl* vtbl; // pointer to the "MsgReceiver" virtual table
	const iSYSTEMVTBL*      sys;  // pointer to the "SYSTEM" virtual table
	MsgRecvData*            data; // pointer to the "MsgReceiver" data structure
} *hi_MsgReceiver;

// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call MsgReceiver_ObjectPreClose( hi_MsgReceiver _this );
tERROR pr_call MsgReceiver_MsgReceive( hi_MsgReceiver _this, tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_MsgReceiver_vtbl = {
	(tIntFnRecognize)        NULL,
	(tIntFnObjectNew)        NULL,
	(tIntFnObjectInit)       NULL,
	(tIntFnObjectInitDone)   NULL,
	(tIntFnObjectCheck)      NULL,
	(tIntFnObjectPreClose)   MsgReceiver_ObjectPreClose,
	(tIntFnObjectClose)      NULL,
	(tIntFnChildNew)         NULL,
	(tIntFnChildInitDone)    NULL,
	(tIntFnChildClose)       NULL,
	(tIntFnMsgReceive)       MsgReceiver_MsgReceive,
	(tIntFnIFaceTransfer)    NULL
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )
typedef void (*fnpMsgReceiver_dummy)( hMSG_RECEIVER _this );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iMsgReceiverVtbl {
	fnpMsgReceiver_dummy  dummy;
}; // "MsgReceiver" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table prototypes,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "MsgReceiver". Static(shared) property table variables
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface version,  )
#define MsgReceiver_VERSION ((tVERSION)1)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(MsgReceiver_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, MsgReceiver_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "message receiver interface", 27 )
	mLOCAL_PROPERTY_BUF( pgRECEIVE_PROCEDURE, MsgRecvData, recv_proc, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY_BUF( pgRECEIVE_CLIENT_ID, MsgRecvData, client_id, cPROP_BUFFER_READ_WRITE )
mPROPERTY_TABLE_END(MsgReceiver_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )
// Interface "MsgReceiver". Register function
tERROR pr_call MsgReceiver_Register( hROOT root ) {
	tERROR error;

	PR_TRACE_A0( root, "Enter \"MsgReceiver::Register\" method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_MSG_RECEIVER,                       // interface identifier
		PID_KERNEL,                             // plugin identifier
		0x00000000,                             // subtype identifier
		MsgReceiver_VERSION,                    // interface version
		VID_PETROVITCH,                         // interface developer
		&i_MsgReceiver_vtbl,                    // internal(kernel called) function table
		(sizeof(i_MsgReceiver_vtbl)/sizeof(tPTR)),// internal function table size
		NULL,                                   // external function table
		0,                                      // external function table size
		MsgReceiver_PropTable,                  // property table
		mPROPERTY_TABLE_SIZE(MsgReceiver_PropTable),// property table size
		sizeof(MsgRecvData),                    // memory size
		IFACE_SYSTEM                            // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE(( root, prtDANGER, "krn\tMsgReceiver(IID_MSG_RECEIVER) registered [%terr]", error ));
	#endif // _PRAGUE_TRACE_

	PR_TRACE_A1( root, "Leave \"MsgReceiver::Register\" method, ret %terr", error );
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
tERROR pr_call MsgReceiver_ObjectPreClose( hi_MsgReceiver _this ) {
	tERROR error;
	PR_TRACE_A0( _this, "Enter MsgReceiver::ObjectPreClose method" );

	if ( _this->data->recv_proc )
		error = _this->data->recv_proc( (hOBJECT)_this, pmc_MSG_RECEIVER, pm_MR_OBJECT_PRECLOSE, (hOBJECT)_this, NULL, (hOBJECT)_this, NULL, NULL );
	error = errOK;

	PR_TRACE_A1( _this, "Leave MsgReceiver::ObjectPreClose method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
// Extended method comment
//  Receives message sent to the object or to the one of the object parents as broadcast
// Parameters are:
// "msg_cls_id"    : Message class identifier
// "msg_id"        : Message identifier
// "obj"           : Object as a reason of message
// "ctx"           : Context of the object processing
// "receive_point" : Point of distributing message (tree top for RegisterMsgHandler call
// "par_buf"       : Buffer of the parameters
// "par_buf_len"   : Lenght of the buffer of the parameters
tERROR pr_call MsgReceiver_MsgReceive( hi_MsgReceiver _this, tDWORD msg_cls_id, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tVOID* par_buf, tDWORD* par_buf_len ) {
	tERROR error;
	PR_TRACE_A0( _this, "Enter \"MsgReceiver::MsgReceive\" method" );

	if ( _this->data->recv_proc )
		error = _this->data->recv_proc( (hOBJECT)_this, msg_cls_id, msg_id, obj, ctx, receive_point, par_buf, par_buf_len );
	else
		error = errOK; // call next desider (before was -- warnCALL_NEXT_DECIDER)

	PR_TRACE_A1( _this, "Leave \"MsgReceiver::MsgReceive\" method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



