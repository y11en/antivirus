#include "rpc_root.h"
#include "pr_common.h"

#define PR_ROOT_STUB	((hROOT)PR_THIS_STUB)


// ---
extern "C" tERROR PRRoot_FindPlugins( handle_t IDL_handle, rpcROOT _this, tPID mfinder_pid, const tPTR param_pool, tDWORD param_pool_size, tDWORD loadflags ) {
	return CALL_Root_FindPlugins( PR_ROOT_STUB, mfinder_pid, param_pool, param_pool_size, loadflags );
}
// ---
extern "C" tERROR PRRoot_GetPluginInstance( handle_t IDL_handle, rpcROOT _this, /* [out] */ rpcPLUGIN __RPC_FAR *result, /* [out][in] */ tDWORD __RPC_FAR *cookie, tPID pid ) {
	return errNOT_SUPPORTED;// CALL_Root_GetPluginInstance( PR_ROOT_STUB, (hPLUGIN*)PR_OUT_REMOTE(result), cookie, pid );
}

// ---
extern "C" tERROR PRRoot_FreeUnusedPlugins( handle_t IDL_handle, rpcROOT _this, tPID* pid_array, tDWORD pid_array_count, tFUP_ACTION action, tDWORD during ) {
	return CALL_Root_FreeUnusedPlugins( PR_ROOT_STUB, pid_array, pid_array_count, action, during );
}

// ---
extern "C" tERROR PRRoot_TraceLevelSet( handle_t IDL_handle, rpcROOT _this, tDWORD op, tIID iid, tPID pid, tDWORD subtype, tDWORD vid, tDWORD level, tDWORD min_level ) {
	return CALL_Root_TraceLevelSet( PR_ROOT_STUB, op, iid, pid, subtype, vid, level, min_level );
}
// ---
extern "C" tERROR PRRoot_GetTracer( handle_t IDL_handle, rpcROOT _this, /* [out] */ rpcTRACER __RPC_FAR *result ) {
	return CALL_Root_GetTracer( PR_ROOT_STUB, (hTRACER*)PR_OUT_REMOTE(result) );
}

// ---
extern "C" tERROR PRRoot_RegisterCustomPropId( handle_t IDL_handle, rpcROOT _this, /* [out] */ tDWORD __RPC_FAR *result, tSTRING name, tDWORD type ) {
	return CALL_Root_RegisterCustomPropId( PR_ROOT_STUB, result, name, type );
}
// ---
extern "C" tERROR PRRoot_UnregisterCustomPropId( handle_t IDL_handle, rpcROOT _this, tSTRING name ) {
	return CALL_Root_UnregisterCustomPropId( PR_ROOT_STUB, name );
}
// ---
extern "C" tERROR PRRoot_GetCustomPropId( handle_t IDL_handle, rpcROOT _this, /* [out] */ tDWORD __RPC_FAR *result, tSTRING name, tDWORD type ) {
	return CALL_Root_GetCustomPropId( PR_ROOT_STUB, result, name, type );
}

extern tVOID PRCustPropSetNotification(hOBJECT _this, tPROPID prop_id, const tPTR buffer, tDWORD size, tCODEPAGE cp, tPTR ctx);

extern "C" tERROR PRRoot_SyncObjCustomPropValue(hOBJECT _this, PRConnection *connect, tPROPID prop_id)
{
	hOBJECT l_object;
	tERROR l_error = CALL_SYS_ChildGetFirst(_this, &l_object, IID_ANY, PID_ANY);
	while( PR_SUCC(l_error) )
	{
		PRRoot_SyncObjCustomPropValue(l_object, connect, prop_id);
		l_error = CALL_SYS_ObjectGetNext(l_object, &l_object, IID_ANY, PID_ANY);
	}

	if( !IS_OBJ_PROXY(_this) && (OBJ_IID(_this) != IID_REMOTE_STUB) )
		return errNOT_OK;

	PRRemoteObject *l_remote = CUST_OBJ(_this);
	if( l_remote->m_connect != connect )
		return errNOT_OK;

	hOBJECT l_obj = IS_OBJ_PROXY(_this) ? _this : CALL_SYS_ParentGet(_this, IID_ANY);

	tDWORD l_size;
	if( PR_FAIL(CALL_SYS_PropertyGet(l_obj, &l_size, prop_id, NULL, 0)) )
		return errNOT_OK;

	tBYTE l_buff[MAX_BUFF_SIZE];
	tBYTE *l_ptr = l_size > MAX_BUFF_SIZE ? new tBYTE[l_size] : l_buff;

	tCODEPAGE l_code_page;
	if( PR_PROP_TYPE(prop_id) == pTYPE_STRING )
	{
		CALL_SYS_PropertyGetStrCP(l_obj, &l_code_page, prop_id);
		CALL_SYS_PropertyGetStr(l_obj, NULL, prop_id, l_ptr, l_size, l_code_page);
	}
	else
	{
		l_code_page = cCP_ANSI;
		CALL_SYS_PropertyGet(l_obj, NULL, prop_id, l_ptr, l_size);
	}

	PRCustPropSetNotification(l_obj, prop_id, l_ptr, l_size, l_code_page, NULL);

	if( l_ptr != l_buff )
		delete [] l_ptr;

	return errOK;
}

// ---
extern "C" tERROR PRRoot_SyncCustomPropValue( handle_t IDL_handle, rpcROOT _this, tPROPID prop_id ) { // -- synchronize custom property values;
	return PRRoot_SyncObjCustomPropValue((hOBJECT)g_root, CUST_ROOT(_this), prop_id);
}
