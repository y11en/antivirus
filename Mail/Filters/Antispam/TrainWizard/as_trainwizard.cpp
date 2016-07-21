// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  02 March 2005,  18:22 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Denisov
// File Name   -- as_trainwizard.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define AS_TrainWizard_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
#include <plugin/p_as_trainwizard.h>
// AVP Prague stamp end



#include "resource.h"
#include "TrainingWizard.h"
#include <structs/s_as_trainwizard.h>

#include <GuiLoader.h>

extern tPTR   g_hInstance;


// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable CAS_TrainWizard : public cAS_TrainWizard {
private:
// Internal function declarations
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations

public:
// External function declarations
	tERROR pr_call Run( cSerializable* p_pSettings );

// Data declaration
// AVP Prague stamp end


private:

	hAS_TRAINSUPPORT  m_hTrainingSupport;
	cASWizardSettings m_hASWizardSettings;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(CAS_TrainWizard)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "AS_TrainWizard". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR CAS_TrainWizard::ObjectInitDone() {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AS_TrainWizard::ObjectInitDone method" );

	// Place your code here
	error = sysCreateObjectQuick( (hOBJECT*)&m_hTrainingSupport, IID_AS_TRAINSUPPORT, PID_AS_TRAINSUPPORT );

	if ( PR_SUCC(error) )
		error = sysRegisterMsgHandler(pmc_AS_TRAIN_STATUS, rmhDECIDER, (hOBJECT)sysGetParent(IID_ANY), IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	if ( error == errOBJECT_ALREADY_EXISTS )
		error = errOK;
	
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR CAS_TrainWizard::ObjectPreClose() {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_FUNC_FRAME( "AS_TrainWizard::ObjectPreClose method" );

	CGuiLoader::Instance().Unload();

	if ( m_hTrainingSupport )
		m_hTrainingSupport->sysCloseObject();
	m_hTrainingSupport = NULL;

	return error;
}
// AVP Prague stamp end

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
tERROR CAS_TrainWizard::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len ) {
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainWizard::MsgReceive method" );

	// Place your code here
	if( p_msg_cls_id == pmc_AS_TRAIN_STATUS )
	{
//		CRootItem *pRoot = CGuiLoader::Instance().GetRoot();
//		if(pRoot)
//			error = pRoot->ProcessEvent(TrainStatus_t::eIID, (TrainStatus_t*)pParam, pRoot, true);
		
		if(g_pTW && g_pTW->m_pSink)
			error = ((CWzTrainig *)g_pTW->m_pSink)->OnTrainStatusEx(p_par_buf_len == SER_SENDMSG_PSIZE ? (TrainStatus_t*)p_par_buf : NULL);
	}
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Run )
// Parameters are:
//! tERROR CAS_TrainWizard::Run() {
tERROR CAS_TrainWizard::Run( cSerializable* p_pSettings ){
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "AS_TrainWizard::Run method" );

	error = m_hASWizardSettings.assign(*p_pSettings, false);
	
	CRootItem *pRoot = CGuiLoader::Instance().CreateAndInitRoot(g_root, 0, NULL);
	if(!pRoot)
		return errNOT_OK;

	if( g_pTW )
	{
		if( g_pTW->m_strItemType == GUI_ITEMT_DIALOG )
			((CDialogItem *)g_pTW)->Activate();
	}
	else
		pRoot->LoadItem(new CWzTrainig(m_hTrainingSupport), g_pTW, "OutlookPlugun.TrainingWizard", NULL, LOAD_FLAG_DESTROY_SINK);
	
	return errOK;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "AS_TrainWizard". Register function
tERROR CAS_TrainWizard::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(AS_TrainWizard)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(AS_TrainWizard)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(AS_TrainWizard)
	mEXTERNAL_METHOD(AS_TrainWizard, Run)
mEXTERNAL_TABLE_END(AS_TrainWizard)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "AS_TrainWizard::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_AS_TRAINWIZARD,                     // interface identifier
		PID_AS_TRAINWIZARD,                     // plugin identifier
		0x00000000,                             // subtype identifier
		AS_TrainWizard_VERSION,                 // interface version
		VID_DENISOV,                            // interface developer
		&i_AS_TrainWizard_vtbl,                 // internal(kernel called) function table
		(sizeof(i_AS_TrainWizard_vtbl)/sizeof(tPTR)),// internal function table size
		&e_AS_TrainWizard_vtbl,                 // external function table
		(sizeof(e_AS_TrainWizard_vtbl)/sizeof(tPTR)),// external function table size
		NULL,                                   // property table
		0,                                      // property table size
		sizeof(CAS_TrainWizard)-sizeof(cObjImpl),// memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"AS_TrainWizard(IID_AS_TRAINWIZARD) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call AS_TrainWizard_Register( hROOT root ) { return CAS_TrainWizard::Register(root); }
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgINTERFACE_VERSION
// You have to implement propetry: pgINTERFACE_COMMENT
// AVP Prague stamp end



