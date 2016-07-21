/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	pr_system_c.cpp
 * \author	Pavel Mezhuev
 * \date	09.12.2002 17:34:41
 * \brief	Prague-system-proxies.
 * 
 */
//************************************************************************

#include "../pr_common.h"

#define VALIDATE_THIS() if(_this->vtbl == &system_iface) _this = (hOBJECT)((tPTR*)_this-1)
#define VALIDATE_THIS_EX() if(_this->vtbl == &system_iface || _this->vtbl == &system_iface_ex || _this->vtbl == LOCAL_SYS) _this = (hOBJECT)((tPTR*)_this-1)

//************************************************************************
// obj management

tObjectCreate		fObjectCreate;
tObjectCreateQuick	fObjectCreateQuick;
tRecognize          fRecognize;

// --
tERROR pr_call Local_ObjectCreate( hOBJECT _this, tVOID* new_obj, tIID iid, tPID pid, tDWORD subtype ) {
	VALIDATE_THIS_EX();
	if( g_mng.IsLocalCreation(_this, pid) )
		return fObjectCreate( _this, new_obj, iid, pid, subtype );

	return PR_INVOKE_BASE(ProxyObjectCreate, 0, _this, PR_ARG4(tid_OBJECT|tid_POINTER, tid_DWORD, tid_DWORD, tid_DWORD),
		PR_ARG4(new_obj, iid, pid, subtype));
}
// ---
tERROR pr_call Local_ObjectCreateQuick( hOBJECT _this, tPTR new_obj, tIID iid, tPID pid, tDWORD subtype ) {
	VALIDATE_THIS_EX();
	if( g_mng.IsLocalCreation(_this, pid) )
		return fObjectCreateQuick( _this, new_obj, iid, pid, subtype );

	return PR_INVOKE_BASE(ProxyObjectCreateQuick, 0, _this, PR_ARG4(tid_OBJECT|tid_POINTER, tid_DWORD, tid_DWORD, tid_DWORD),
		PR_ARG4(new_obj, iid, pid, subtype));
}
// ---
tERROR pr_call Local_ObjectCreateDone( hOBJECT _this ) {
	VALIDATE_THIS();
	return PR_INVOKE_SYS(ObjectCreateDone, 0, PR_ARG0(), PR_ARG0());
}
// ---
tERROR pr_call Local_ObjectClose( hOBJECT _this ) {
	VALIDATE_THIS();
	PRRemoteProxy *l_proxy = CUST_PROXY(_this);
	tBOOL release_needed = l_proxy->m_is_create_ref;
	tERROR error = PR_INVOKE_SYS(ObjectClose, prf_NOVALIDATE, PR_ARG0(), PR_ARG0());

	if( release_needed )
		l_proxy->m_connect->ReleaseObject(_this, cFALSE);
	return error;
}

//************************************************************************
// recognize methods

tERROR pr_call Local_Recognize( hOBJECT _this, tIID by_iid, tPID by_pid, tDWORD type )
{
	VALIDATE_THIS_EX();
	if( g_mng.IsLocalCreation(_this, by_pid, true) )
		return fRecognize( _this, by_iid, by_pid, type );

	return PR_INVOKE_SYS(Recognize, 0, PR_ARG3(tid_DWORD,tid_DWORD,tid_DWORD), PR_ARG3(by_iid, by_pid, type));
}

/*
tERROR pr_call Local_RecognizeByList( hOBJECT _this, tPID* result, tIID by_iid, const tPID* by_pid_list, tDWORD count, tDWORD type )
{
	VALIDATE_THIS();

	tPID l_pid = CUST_PROXY(_this)->m_pid;
	tDWORD i, find_pos = -1;
	tERROR err;
	
	bool f_found = false;
	for(i = 0; i < count; i++)
		if( !g_mng.IsLocalCreation(_this, by_pid_list[i]) )
		{
			find_pos = i;
			break;
		}

	if( find_pos == -1 )
		return LOCAL_SYS->RecognizeByList( _this, result, by_iid, by_pid_list, count, type );

	err = PR_INVOKE_SYS(Recognize, 0, PR_ARG3(tid_DWORD,tid_DWORD,tid_DWORD), PR_ARG3(by_iid, l_pid, type));

	for(i = 0; PR_FAIL(err) && i < count; i++)
		if( i != find_pos )
			err = LOCAL_SYS->Recognize( _this, by_iid, by_pid_list[i], type );

	return err;
}
*/

//************************************************************************
// property management

tERROR pr_call Local_GetProperty( hOBJECT _this, tDWORD* out_size, tPROPID prop_id, tCHAR* buffer, tDWORD size ) 
{
	switch( PR_PROP_TYPE(prop_id) )
	{
	case pTYPE_OBJECT:
		return PR_INVOKE_SYS(PropertyGet, 0, PR_ARG3(tid_DWORD|tid_POINTER,tid_DWORD,tid_OBJECT|prf_OUT|prf_INBUFF),
			PR_ARG3(out_size, prop_id, buffer));
	case pTYPE_STRING:
		return PR_INVOKE_SYS(PropertyGetStr, 0, PR_ARG4(tid_DWORD|tid_POINTER,tid_DWORD,tid_BINARY/*|prf_OUT*/,tid_DWORD),
			PR_ARG5(out_size, prop_id, buffer, size, cCP_ANSI));
	case pTYPE_CODEPAGE:
		if( out_size )
			*out_size = sizeof(tCODEPAGE);
		if( buffer )
			*(tCODEPAGE*)buffer = cCP_ANSI;
		return errOK;
	default:
		return PR_INVOKE_SYS(PropertyGet, 0, PR_ARG3(tid_DWORD|tid_POINTER,tid_DWORD,tid_BINARY|prf_OUT),
			PR_ARG4(out_size, prop_id, buffer, size));
	}
}

tERROR pr_call Local_SetProperty( hOBJECT _this, tDWORD* out_size, tPROPID prop_id, tCHAR* buffer, tDWORD size ) 
{
	if( prop_id == pgPROXY_CREATE_REMOTE )
	{
		CUST_PROXY(_this)->m_is_create_remote = cTRUE;
		return errOK;
	}

	switch( PR_PROP_TYPE(prop_id) )
	{
	case pTYPE_OBJECT:
		return PR_INVOKE_SYS(PropertySet, 0, PR_ARG3(tid_DWORD|tid_POINTER,tid_DWORD,tid_OBJECT|prf_IN|prf_INBUFF),
			PR_ARG3(out_size, prop_id, buffer));
	case pTYPE_STRING:
		return PR_INVOKE_SYS(PropertySetStr, 0, PR_ARG4(tid_DWORD|tid_POINTER,tid_DWORD,tid_BINARY|prf_IN,tid_DWORD),
			PR_ARG5(out_size, prop_id, buffer, size, cCP_ANSI));
	default:
		return PR_INVOKE_SYS(PropertySet, 0, PR_ARG3(tid_DWORD|tid_POINTER,tid_DWORD,tid_BINARY|prf_IN),
			PR_ARG4(out_size, prop_id, buffer, size));
	}
}

// ---
tERROR pr_call Local_PropertyIsPresent( hOBJECT _this, tPROPID prop_id ) {
	VALIDATE_THIS();
	return PR_INVOKE_SYS(PropertyIsPresent, 0, PR_ARG1(tid_DWORD), PR_ARG1(prop_id));
}
// ---
tDWORD pr_call Local_PropertySize( hOBJECT _this, tPROPID prop_id ) {
	VALIDATE_THIS();
	if( IS_PROP_RANGE(prop_id, CUSTOM) )
		return LOCAL_SYS->PropertySize(_this, prop_id);
	return PR_INVOKE_SYS(PropertySize, 0, PR_ARG1(tid_DWORD), PR_ARG1(prop_id));
}
// ---
tERROR pr_call Local_PropertyEnumId( hOBJECT _this, tPROPID* prop_id_result, tDWORD* cookie ) {
	VALIDATE_THIS();
	return PR_INVOKE_SYS(PropertyEnumId, 0, PR_ARG2(tid_DWORD|tid_POINTER,tid_DWORD|tid_POINTER), PR_ARG2(prop_id_result, cookie));
}
// ---
tERROR pr_call Local_PropertyGetStrCP( hOBJECT _this, tCODEPAGE* cp, tPROPID prop_id ) {
	VALIDATE_THIS();
	if( IS_PROP_RANGE(prop_id, CUSTOM) )
		return LOCAL_SYS->PropertyGetStrCP(_this, cp, prop_id);
	return PR_INVOKE_SYS(PropertyGetStrCP, 0, PR_ARG2(tid_DWORD|tid_POINTER,tid_DWORD), PR_ARG2(cp, prop_id));
}

//************************************************************************
// Message registration

tRegisterMsgHandlerList		fRegisterMsgHandlerList;
tUnregisterMsgHandlerList	fUnregisterMsgHandlerList;

tERROR pr_call Remote_ManageHandlerList(hOBJECT _this, const tMsgHandlerDescr* handler_list, tDWORD handler_count, tBOOL bregister)
{
	if( !g_mng.IsActive() )
		return errOK;

	tProxyMsgHandlerDescr l_handlers[MAX_MHL_SIZE];
	for(tDWORD i = 0; i < handler_count; i++)
	{
		l_handlers[i].object = CUST_OBJ(_this)->m_connect->GetProxy(handler_list[i].object, cFALSE, cFALSE);
		l_handlers[i].flags = handler_list[i].flags;
		l_handlers[i].msg_cls = handler_list[i].msg_cls;
		l_handlers[i].obj_iid = handler_list[i].obj_iid;
		l_handlers[i].obj_pid = handler_list[i].obj_pid;
		l_handlers[i].ctx_iid = handler_list[i].ctx_iid;
		l_handlers[i].ctx_pid = handler_list[i].ctx_pid;
	}

	if( bregister )
		return PR_INVOKE_BASE(ProxyRegisterMsgHandlerList, 0, _this, PR_ARG1(tid_BINARY|prf_IN),
			PR_ARG2(l_handlers, handler_count*sizeof(tProxyMsgHandlerDescr)));
	else
		return PR_INVOKE_BASE(ProxyUnregisterMsgHandlerList, 0, _this, PR_ARG1(tid_BINARY|prf_IN),
			PR_ARG2(l_handlers, handler_count*sizeof(tProxyMsgHandlerDescr)));
}

tERROR pr_call Remote_ManageHandlerListEx(hOBJECT _this, const tMsgHandlerDescr* handler_list, tDWORD handler_count, tBOOL bregister)
{
	vector<hOBJECT> to_sync;
	ENUM_STUB_BEGIN(_this)
		if( l_stub->m_connect->Lock() )
			if( CALL_SYS_ValidateLock(g_root, _stub, IID_ANY, PID_ANY) == errOK )
				to_sync.push_back(_stub);
			else
				l_stub->m_connect->Unlock();
	ENUM_STUB_END()

	for(size_t i = 0; i < to_sync.size(); i++)
	{
		Remote_ManageHandlerList(to_sync[i], handler_list, handler_count, bregister);
		CUST_OBJ(to_sync[i])->m_connect->Unlock();
		CALL_SYS_Release(to_sync[i]);
	}
	return errOK;
}

tERROR pr_call Local_RegisterMsgHandlerList(hOBJECT _this, const tMsgHandlerDescr* handler_list, tDWORD handler_count)
{
	VALIDATE_THIS_EX();

	if( IS_OBJ_PROXY(_this) )
		Remote_ManageHandlerList(_this, handler_list, handler_count, cTRUE);
	else if( IS_OBJ_HAS_PROXY(_this) )
		Remote_ManageHandlerListEx(_this, handler_list, handler_count, cTRUE);

	return fRegisterMsgHandlerList(_this, handler_list, handler_count);
}

tERROR pr_call Local_RegisterMsgHandler(hOBJECT _this, tDWORD msg_cls, tDWORD flags, tPTR tree_top, tIID iid_obj, tPID pid_obj, tIID iid_ctx, tPID pid_ctx)
{
	VALIDATE_THIS_EX();
	tMsgHandlerDescr l_reg_info;
	l_reg_info.object = _this;
	l_reg_info.flags = flags;
	l_reg_info.msg_cls = msg_cls;
	l_reg_info.obj_iid = iid_obj;
	l_reg_info.obj_pid = pid_obj;
	l_reg_info.ctx_iid = iid_ctx;
	l_reg_info.ctx_pid = pid_ctx;

	return Local_RegisterMsgHandlerList((hOBJECT)tree_top, &l_reg_info, 1);
}

tERROR pr_call Local_UnregisterMsgHandlerList(hOBJECT _this, const tMsgHandlerDescr* handler_list, tDWORD handler_count)
{
	VALIDATE_THIS_EX();
	if( IS_OBJ_PROXY(_this) )
		Remote_ManageHandlerList(_this, handler_list, handler_count, cFALSE);
	else if( IS_OBJ_HAS_PROXY(_this) )
		Remote_ManageHandlerListEx(_this, handler_list, handler_count, cFALSE);

	return fUnregisterMsgHandlerList(_this, handler_list, handler_count);
}

tERROR pr_call Local_UnregisterMsgHandler(hOBJECT _this, tDWORD msg_cls, tPTR tree_top)
{
	VALIDATE_THIS_EX();
	tMsgHandlerDescr l_unreg_info;
	l_unreg_info.object = _this;
	l_unreg_info.msg_cls = msg_cls;

	return Local_UnregisterMsgHandlerList((hOBJECT)tree_top, &l_unreg_info, 1);
}

//************************************************************************
// System table initialization

iSYSTEMVTBL system_iface;
iSYSTEMVTBL system_iface_ex;

void PRRemoteManagerImpl::InitSystemIFace()
{
	fObjectCreate = LOCAL_SYS->ObjectCreate;
	fObjectCreateQuick = LOCAL_SYS->ObjectCreateQuick;
	fRecognize = LOCAL_SYS->Recognize;
	fRegisterMsgHandlerList = LOCAL_SYS->RegisterMsgHandlerList;
	fUnregisterMsgHandlerList = LOCAL_SYS->UnregisterMsgHandlerList;

	LOCAL_SYS->ObjectCreate = Local_ObjectCreate;
	LOCAL_SYS->ObjectCreateQuick = Local_ObjectCreateQuick;
	LOCAL_SYS->Recognize = Local_Recognize;
	LOCAL_SYS->RegisterMsgHandler = Local_RegisterMsgHandler;
	LOCAL_SYS->UnregisterMsgHandler = Local_UnregisterMsgHandler;
	LOCAL_SYS->RegisterMsgHandlerList = (tRegisterMsgHandlerList)Local_RegisterMsgHandlerList;
	LOCAL_SYS->UnregisterMsgHandlerList = (tUnregisterMsgHandlerList)Local_UnregisterMsgHandlerList;

	memcpy(&system_iface, g_root->sys, sizeof(system_iface));
	memcpy(&system_iface_ex, g_root->sys, sizeof(system_iface));

	system_iface.ObjectCreateDone = Local_ObjectCreateDone;
	system_iface.ObjectClose = Local_ObjectClose;

	system_iface.PropertySize = Local_PropertySize;
	system_iface.PropertyGetStrCP = Local_PropertyGetStrCP;
	system_iface.PropertyIsPresent = Local_PropertyIsPresent;
	system_iface.PropertyEnumId = Local_PropertyEnumId;
}

//************************************************************************
