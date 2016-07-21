/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	pr_client.cpp
 * \author	Pavel Mezhuev
 * \date	09.12.2002 17:34:41
 * \brief	Имплементация интерфейса Prague-client.
 * 
 */
//************************************************************************

#include "pr_common.h"

#if defined (_WIN32)
	#include "pr_process.h"
	WTSApi g_WTSApi;
	#define __TRY	__try
	#define __EXCEPT __except
#elif defined (__unix__)
	pid_t PRRemoteManagerImpl::m_ProccessId = getpid();
	#warning "Consideration is needed"
	#define __TRY if(1)	
	#define __EXCEPT else if
#endif

#include "rpc_connect.h"
#include "ver_mod.h"

#include <Prague/pr_loadr.h>
#define  __DECLARE_SER_CONSTANTS_ONLY
#include <Prague/pr_serializable.h>
#include <Prague/plugin/p_win32loader.h>

//#define STREAM_PACK_OPS fSTREAM_PACK_DEFAULT
#define STREAM_PACK_OPS 0

// #if !defined(_WIN64) && !defined (__unix__)
// 	#define USE_FSDRVLIB 
// 	#include "..\windows\hook\hook\FSDrvLib.h"
// #endif

PRRemoteManagerImpl	g_mng;
void*               g_module;
hROOT               g_root = NULL;
PRRemoteAPI         g_API;
long                g_UniqueId = 0;

PR_MAKE_TRACE_FUNC;

//************************************************************************
// Proxy object properties

extern tERROR pr_call Local_GetProperty( hOBJECT _this, tDWORD* out_size, tPROPID prop_id, tCHAR* buffer, tDWORD size );
extern tERROR pr_call Local_SetProperty( hOBJECT _this, tDWORD* out_size, tPROPID prop_id, tCHAR* buffer, tDWORD size );

mPROPERTY_TABLE(prop_table)
	mLOCAL_PROPERTY_BUF( pgPLUGIN_ID, PRRemoteProxy, m_pid, cPROP_BUFFER_READ )
	mLAST_CALL_PROPERTY( Local_GetProperty, Local_SetProperty )
mPROPERTY_TABLE_END(prop_table)

//************************************************************************
// Serialize objects callbacks

tERROR PRObjDirectConvertFunc(hOBJECT object, hOBJECT to_convert, tPTR converted, tPTR ctx)
{
	if( !ctx )
		*(rpcOBJECT*)converted = 0;
	else if( object == to_convert )
		*(rpcOBJECT*)converted = (rpcOBJECT)(-1);
	else if( PR_SUCC(CALL_SYS_ObjectValid(g_root, to_convert)) )
		*(rpcOBJECT*)converted = ((PRConnection*)ctx)->GetProxy(to_convert, cFALSE, cFALSE);
	else
		*(rpcOBJECT*)converted = 0;
	return errOK;
}

tERROR PRObjBackwardConvertFunc(hOBJECT object, tPTR to_convert, hOBJECT* converted, tPTR ctx)
{
	*converted = *(rpcOBJECT*)to_convert != (rpcOBJECT)(-1) ? *(hOBJECT*)to_convert : object;
	return errOK;
}

tERROR PRObjCheckCustomPropFunc(hOBJECT object, tPROPID prop_id, tPTR ctx)
{
	if( !ctx /*|| (prop_id & pCUSTOM_HERITABLE)*/ )
		return errNOT_OK;

	return errOK;
}

//************************************************************************
// Remote host

PRRemoteHost::PRRemoteHost(tDWORD end_point, const tCHAR* host)
{
	m_local = host ? 0 : end_point;
	m_port = host ? end_point : 0;
	if( host )
		strcpy(m_host, host);
}

string	 PRRemoteHost::GetEndPoint() const
{
	tCHAR buff[MAX_PATH];
	if( m_local )
		sprintf(buff, "PRRemote:%d", m_local);
	else
		sprintf(buff, "%d", m_port);
	return buff;
}

//************************************************************************
// Remote connection

void PRConnection::Init(hOBJECT p_this, const PRRemoteHost &host)
{
	m_ref = 1;
	m_this = p_this;
	m_connect = this;
	m_lockref = 0;
	m_rundown = cFALSE;

	*(PRRemoteHost*)this = host;
	((HRemoteObject*)m_this)->sys = (iSYSTEMVtbl*)&system_iface;

	if( !rpc_connect_to_server(host.GetProtocol(), host.GetHostName(), host.GetEndPoint().c_str(), &m_handle) )
	{
		CALL_SYS_RegisterMsgHandler(m_this, pmc_REMOTE_GLOBAL, rmhDECIDER, g_root, IID_ANY, PID_ANY, IID_ANY, PID_ANY);

		PR_TRACE((g_root, prtERROR, "rmt\tInitConnection: to server %s (current %d)", host.GetEndPoint().c_str(), g_mng.m_ProccessId));
		m_status = warnDEFAULT;
	}
	else
	{
		PR_TRACE((g_root, prtERROR, "rmt\tInitConnection: cannot connect to server %s", host.GetEndPoint().c_str()));
		m_status = errUNEXPECTED;
	}
}

bool PRConnection::IsConnected()
{
	return g_mng.IsActive() && m_object && !m_rundown;
}

void PRConnection::Close()
{
	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "rmt\tDisconnect from server..."));

	bool f_connected = !!m_object;
	m_object = 0;

	if( m_handle )
		rpc_disconnect_from_server(m_handle, f_connected, &m_context);

	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "rmt\tDisconnect from server done"));
}

bool PRConnection::Lock()
{
	InterlockedIncrement(&m_lockref);
	if( !IsConnected() )
	{
		InterlockedDecrement(&m_lockref);
		return false;
	}
	return true;
}

void PRConnection::Unlock()
{
	InterlockedDecrement(&m_lockref);
}

void PRConnection::AddRef()
{
	m_ref++;
}

void PRConnection::Release(tBOOL is_close)
{
	if( is_close )
	{
		if( !m_object )
			return;

		PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "rmt\tServer (%d) has been disconnected", m_local));

		m_object = 0;
		m_stub = 0;
		m_plocal = m_local;
		if( m_local )
			m_local = PR_PROCESS_ANY;
	}
	if( --m_ref <= 0 && !m_object )
          LOCAL_SYS->ObjectClose(m_this);
}

tERROR PRConnection::AddRefProxy(hOBJECT object)
{
	for(hOBJECT obj = object; obj; obj = CALL_SYS_ParentGet(obj, IID_ANY))
	{
		if( IS_ROOT_PROXY(obj) || obj == (hOBJECT)g_root )
			break;

		if( IS_OBJ_PROXY(obj) )
			CUST_OBJ(obj)->m_ref++;
	}
	AddRef();
	return errOK;
}

tERROR PRConnection::CreateProxy(hOBJECT parent, tBOOL is_ref, tProxyCreateInfo *info, rpcOBJECT *proxy)
{
	hOBJECT l_proxy_obj = NULL;
	tERROR l_error = CALL_SYS_ObjectCreate(parent, &l_proxy_obj, info->iid, PID_REMOTE_PRAGUE, 0);

	if( l_error == errINTERFACE_NOT_FOUND )
		l_error = CALL_SYS_ObjectCreate(parent, &l_proxy_obj, IID_REMOTE_PROXY, PID_REMOTE_PRAGUE, 0);

	if( PR_SUCC(l_error) )
	{
		PRRemoteProxy *l_proxy = CUST_PROXY(l_proxy_obj);
		l_proxy->m_connect = this;
		l_proxy->m_object = info->object;
		l_proxy->m_stub = info->stub;
		l_proxy->m_pid = info->pid;
		l_proxy->m_is_create_ref = is_ref;
		
		CALL_SYS_ObjectCreateDone(l_proxy_obj);

		((HRemoteObject*)l_proxy_obj)->sys = (iSYSTEMVtbl*)&system_iface;

		*proxy = (rpcOBJECT)l_proxy_obj;

		if( is_ref )
			AddRefProxy(l_proxy_obj);

		AddRefProxy(parent);
	}
	return l_error;
}

tERROR PRConnection::FindProxy(hOBJECT object, tBOOL is_ref, tBOOL is_full, rpcOBJECT *proxy)
{
	if( IS_OBJ_PROXY(object))
	{
		PRRemoteObject *l_remote = CUST_OBJ(object);
		if( l_remote->m_connect != this )
		{
			if( l_remote->m_is_create_remote )
			{
				return PR_INVOKE_BASE(ProxyCreateRemote, 0, object,
					PR_ARG2(tid_DWORD, prf_OBJECT|tid_POINTER), PR_ARG2(m_local, proxy));
			}

			PR_TRACE((g_root, prtERROR, "rmt\tRPC cann't create proxy"));
			*proxy = 0;
			return errNOT_OK;
		}
		*proxy = l_remote->m_object;
		return errOK;
	}

	*proxy = 0;
	ENUM_STUB_BEGIN(object)
		if( l_stub->m_connect == this )
		{
			if( is_ref )
				l_stub->m_ref++;

			if( is_full && !l_stub->m_is_full && PR_SUCC(SetProxyData(object, l_stub->m_object)) )
				l_stub->m_is_full = cTRUE;

			*proxy = l_stub->m_object;
		}
	ENUM_STUB_END()

	if( !*proxy )
		return errOBJECT_NOT_FOUND;

	if( is_ref )
		PR_INVOKE_BASE(ProxyAddRef, 0, m_this, PR_ARG1(prf_OBJECT), PR_ARG1(*proxy));
	return errOK;
}

rpcOBJECT PRConnection::GetProxy(hOBJECT object, tBOOL is_ref, tBOOL is_full)
{
	rpcOBJECT l_proxy = 0;

	if( !object )
		return l_proxy;

	if( object == (hOBJECT)g_root || IS_ROOT_PROXY(object) )
		return m_object;

	if( !Lock() )
		return 0;

	g_mng.m_stub_lock.lock();

	tERROR l_error = FindProxy(object, is_ref, is_full, &l_proxy);
	if( l_error == errOBJECT_NOT_FOUND )
	{
		hOBJECT parent = CALL_SYS_ParentGet(object, IID_ANY);
		rpcOBJECT parent_proxy = GetProxy(parent, cFALSE, is_full);
		if( !parent_proxy )
			parent_proxy = m_object;

		hOBJECT l_proxy_stub = NULL;
		l_error = CALL_SYS_ObjectCreate(object, &l_proxy_stub, IID_REMOTE_STUB, PID_REMOTE_PRAGUE, 0);

		if( PR_SUCC(l_error) )
		{
			tProxyCreateInfo l_info;
			l_info.object = (rpcOBJECT)object;
			l_info.stub = (rpcOBJECT)l_proxy_stub;
			l_info.iid = CALL_SYS_PropertyGetDWord(object, pgINTERFACE_ID);
			l_info.pid = CALL_SYS_PropertyGetDWord(object, pgPLUGIN_ID);

			l_error = PR_INVOKE_BASE(ProxyCreate, 0, m_this,
				PR_ARG4(prf_OBJECT, tid_BOOL, tid_BINARY/*|prf_IN*/, prf_OBJECT|tid_POINTER),
				PR_ARG5(parent_proxy, is_ref, &l_info, sizeof(tProxyCreateInfo), &l_proxy));
				
		}

		if( PR_SUCC(l_error) )
		{
			object->sys = (iSYSTEMVtbl*)&system_iface_ex;

			PRRemoteObject *l_stub = CUST_OBJ(l_proxy_stub);
			l_stub->m_connect = this;
			l_stub->m_object = l_proxy;

			AddRef();
			if( is_ref )
				l_stub->m_ref++;

			if( is_full && PR_SUCC(SetProxyData(object, l_proxy)) )
				l_stub->m_is_full = cTRUE;
		}
		else if( l_proxy_stub )
		{
			CALL_SYS_ObjectClose(l_proxy_stub);
		}
	}

	g_mng.m_stub_lock.unlock();
	Unlock();
	return l_proxy;
}

tERROR PRConnection::SetProxyData(hOBJECT object, rpcOBJECT proxy)
{
	tDWORD l_data_size;
	tBYTE* l_data = NULL;

	tERROR l_error = CALL_SYS_ObjectGetInternalData(object, &l_data_size, sizeof(rpcOBJECT),
		NULL, 0, NULL, PRObjCheckCustomPropFunc, this);

	if( PR_SUCC(l_error) )
	{
		l_data = new tBYTE[l_data_size];
		l_error = CALL_SYS_ObjectGetInternalData(object, &l_data_size, sizeof(rpcOBJECT),
			(tBYTE*)l_data, l_data_size, PRObjDirectConvertFunc, PRObjCheckCustomPropFunc, this);
	}

	l_error = PR_INVOKE_BASE(ProxySetData, 0, m_this,
		PR_ARG2(prf_OBJECT, tid_BINARY|prf_IN),
		PR_ARG3(proxy, l_data, l_data_size));

	if( PR_SUCC(l_error) )
		CALL_SYS_PropertySetBool(object, psCALLBACK_SENDER, cTRUE);

	if( l_data )
		delete[] l_data;

	return l_error;
}

void PRConnection::ReleaseProxy(hOBJECT object, tBOOL is_close)
{
	hOBJECT proxy = object, next;
	if( is_close )
	{
		PRRemoteProxy *l_proxy = CUST_PROXY(object);
		if( !l_proxy->m_object )
			return;

		if( l_proxy->m_rd_clb )
			l_proxy->m_rd_clb(object, l_proxy->m_rd_ctx);

		CALL_SYS_SendMsg(object, pmc_REMOTE_PROXY_DISCONECTED, 0, NULL, NULL, NULL);

		l_proxy->m_object = 0;
		l_proxy->m_stub = 0;

		proxy = CALL_SYS_ParentGet(object, IID_ANY);

		if( l_proxy->m_ref <= 0 )
		{
			PR_TRACE((g_root, prtSPAM, "rmt\tProxy closing call: object=%08X", object));
			LOCAL_SYS->ObjectClose(object);
		}
	}

	for(; proxy; proxy = next)
	{
		next = CALL_SYS_ParentGet(proxy, IID_ANY);

		if( IS_ROOT_PROXY(proxy) || proxy == (hOBJECT)g_root )
			break;

		if( !IS_OBJ_PROXY(proxy) )
			continue;

		PRRemoteProxy *l_proxy = CUST_PROXY(proxy);

		if( --l_proxy->m_ref <= 0 && !l_proxy->m_object )
		{
			PR_TRACE((g_root, prtSPAM, "rmt\tProxy closing call: object=%08X", proxy));
			LOCAL_SYS->ObjectClose(proxy);
		}
	}

	Release(cFALSE);
}

void PRConnection::CloseProxy(rpcOBJECT object)
{
	if( IsConnected() && object )
	{
		PR_TRACE((g_root, prtSPAM, "rmt\tClosing proxy... handle=%08X, object=%08X", m_handle, object));
		PR_INVOKE_BASE(CloseProxy, 0, m_this, PR_ARG1(prf_OBJECT), PR_ARG1(object));
		PR_TRACE((g_root, prtSPAM, "rmt\tClosing proxy done"));
	}

	Release(cFALSE);
}

void PRConnection::ReleaseStub(hOBJECT object, tBOOL is_close)
{
	PRRemoteObject *l_stub = CUST_OBJ(object);

	if( is_close )
	{
		l_stub->m_object = 0;

		hOBJECT l_parent = CALL_SYS_ParentGet(object, IID_ANY);
		if( l_parent )
		{
			tDWORD l_size = sizeof(tDWORD);
			CALL_SYS_SendMsg(l_parent, pmc_REMOTE_PROXY_DETACHED, 0, NULL, &l_stub->m_ref, &l_size);
		}
	}

	if( --l_stub->m_ref < 0 || is_close )
		LOCAL_SYS->ObjectClose(object);
}

void PRConnection::CloseStub(rpcOBJECT object)
{
	if( IsConnected() && object )
	{
		PR_TRACE((g_root, prtSPAM, "rmt\tClosing stub... handle=%08X, object=%08X", m_handle, object));
		PR_INVOKE_BASE(CloseStub, 0, m_this, PR_ARG1(prf_OBJECT), PR_ARG1(object));
		PR_TRACE((g_root, prtSPAM, "rmt\tClosing stub done"));
	}
}

void PRConnection::ReleaseObject(hOBJECT object, tBOOL is_close)
{
	if( IS_OBJ_PROXY(object) )
		ReleaseProxy(object, is_close);
	else
		ReleaseStub(object, is_close);
}

//************************************************************************
// Remote manager

PRRemoteManagerImpl::PRRemoteManagerImpl()
        : m_plugin(NULL), m_ienum(NULL), m_init_ref(0), m_loader(NULL), m_init_flags(0)
{
}

PRRemoteManagerImpl::~PRRemoteManagerImpl()
{
}

void PRRemoteManagerImpl::DbgTrace(tSTRING string)
{
	OUTPUT_DEBUG_STRING(string);
	OUTPUT_DEBUG_STRING("\n");
}

// Initialization methods
tERROR PRRemoteManagerImpl::LoadCallback(hROOT root, tVOID* params)
{
	PRRemoteManagerImpl * pThis = (PRRemoteManagerImpl*)params;
	return pThis->InitLocalPrague(root);
}

tERROR PRRemoteManagerImpl::Initialize(tDWORD flags, PRInitParams* params)
{
	tERROR error = errOK;

	PRAutoLocker locker(m_lock);
	m_init_ref++;

	if( g_root )
		return error;

	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "rmt\tPrague loading..."));

#if defined (_WIN32)
	tCHAR module_name[MAX_PATH];
	GetModuleFileName((HINSTANCE)g_module, module_name, sizeof(module_name));
	g_UniqueId = VER_PRODUCTVERSION_COMPILATION;//iCountCRC32str(module_name);

	tCHAR* filename = strrchr(module_name, '\\');
	if( filename )
	{
		strcpy(++filename, PR_LOADER_MODULE_NAME);
		m_loader = LOAD_LIBRARY(module_name);
	}
#endif

	if( !m_loader )
		m_loader = LOAD_LIBRARY(PR_LOADER_MODULE_NAME);

#if defined (__unix__)
    if( m_loader )
	{
		tPluginInit aDllMain = (tPluginInit) GET_SYMBOL ( m_loader, "DllMain" );
		tERROR aReserved;
		if ( !aDllMain || !aDllMain ( m_loader, 1, &aReserved ) )
		{
			UNLOAD_LIBRARY ( m_loader );  
			m_loader = 0;
		}
    }
#endif

	if( !m_loader )
	{
		PR_TRACE((g_root, prtERROR , "rmt\tCannot load PrLoader"));
		return errMODULE_CANNOT_BE_LOADED;
	}

	CSharedTable::ECreationFlags st_flags = CSharedTable::CrUsual;
	if (flags & PR_FLAG_LOW_INTEGRITY_HELPER)
		st_flags = CSharedTable::CrStartPipe;
	if (flags & PR_FLAG_LOW_INTEGRITY_PROCESS)
		st_flags = CSharedTable::CrUsePipe;

	if( !CSharedTable::Init("PRObjects", sizeof(PRShareObject), false, true, st_flags) )
		return errOBJECT_CANNOT_BE_INITIALIZED;

	if( !g_shp.CSharedTable::Init("PRCustomProps", sizeof(PRShareProp), true, false, st_flags) )
		return errOBJECT_CANNOT_BE_INITIALIZED;

#ifdef USE_FSDRVLIB
	FSDrvInterceptorInit();
	FSDrvInterceptorRegisterInvisibleThread();
#endif

	PragueLoadFuncEx plf = (PragueLoadFuncEx)GET_SYMBOL (m_loader, PR_LOADER_LOAD_FUNC);

	if( flags == PR_FLAG_DEFAULT )
		flags = PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH | PR_LOADER_FLAG_FORCE_PLUGIN_LOAD_NATIVE;

	if( m_init_flags == 0 )
	{
		m_init_flags = flags;
	}
	else 
	{
		m_init_flags |= (flags & PR_FLAG_UNLOAD_LOADER);
	}

	tDSKMAppId* data = NULL;
	if( m_params = params )
	{
		flags |= PR_LOADER_FLAG_APP_ID;
		data = m_params->app_id;
	}

	if( plf )
		error = plf(&g_root, flags, LoadCallback, this, data);

	if( PR_SUCC(error) )
		error = InitRemotePrague();

	if( PR_SUCC(error) )
	{
		PRRegisterObject(PR_REMOTE_ROOT, (hOBJECT)g_root);
		PR_TRACE((g_root, prtALWAYS_REPORTED_MSG , "rmt\tPrague loaded" ));
		m_is_init = true;
	}
	else
	{
		PR_TRACE((g_root, prtERROR , "rmt\tInitPrague FAILED: %08X", error ));
		PRDeinitialize();
	}

#ifdef USE_FSDRVLIB
	FSDrvInterceptorUnregisterInvisibleThread();
#endif
	return error;
}

tERROR PRRemoteManagerImpl::Deinitialize()
{
	PRAutoLocker locker(m_lock);

	if( --m_init_ref && m_is_init )
		return errOK;

    CSharedTable::Done();

	StopServer();

	if( g_root )
	{
		PRUnregisterObject((hOBJECT)g_root);

		PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "rmt\tPrague unloading..." ));
		PragueUnloadFunc puf = (PragueUnloadFunc)GET_SYMBOL(m_loader, PR_LOADER_UNLOAD_FUNC);
		if( puf )
			puf( g_root );

		m_plugin = NULL;
		g_root = NULL;
	}

	if( m_loader )
	{
		if( m_init_flags & PR_FLAG_UNLOAD_LOADER ) 
		{
			UNLOAD_LIBRARY(m_loader);
		}
		m_loader = NULL;
	}

	g_shp.CSharedTable::Done();

#ifdef USE_FSDRVLIB
	FSDrvInterceptorDone();
#endif
	return errOK;
}

tERROR PRRemoteManagerImpl::StopServer()
{
	if( !m_is_init )
		return errOK;

	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "rmt\tRPC stoping server..."));
	m_is_init = false;
	rpc_stop_server();

	m_event.reset();
	return errOK;
}

// Cleanup methods
void PRRemoteManagerImpl::RunDownClient(hOBJECT root, tBOOL f_shutdown)
{
	PRAutoLocker locker(m_lock);
	if( !IS_VALID_REMOTE(root) )
		return;

	PRConnection *connect = CUST_ROOT(root);
	if( !connect )
		return;

	connect->m_rundown = cTRUE;
	while( connect->m_lockref )
	{
		PR_TRACE((g_root, prtIMPORTANT, "rmt\tWait for locked calls ref(%d)", connect->m_lockref));
		Sleep(50);
	}

	if( f_shutdown && connect->m_local )
	{
		ClearProcessRecords(connect->m_local);
		g_shp.ClearProcessRecords(connect->m_local);
	}

	if( connect->m_attached )
		CALL_SYS_SendMsg(g_root, pmc_REMOTE_CLOSE_REQUEST, 0, NULL, NULL, NULL);

	PRAutoLocker locker_proxy(m_proxy_lock);
	PRAutoLocker locker_stub(m_stub_lock);
	PRAutoLocker locker_validate(m_validate_lock);

	int count = (int)m_objects.size();
	for(int i = count-1; i >= 0; i--)
	{
		hOBJECT object = m_objects[i];
		if( IS_ROOT_PROXY(object) )
			continue;

		PRRemoteObject *l_remote = CUST_OBJ(object);
		if( l_remote->m_connect == connect )
			connect->ReleaseObject(object, cTRUE);
	}

	connect->Release(cTRUE);
	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "rmt\tClient has been rundown"));
}

// Registration methods
tERROR PRRemoteManagerImpl::RegisterRootIFace()
{
	return CALL_Root_RegisterIFace(
		g_root,
		IID_REMOTE_ROOT,						// interface identifier
		PID_REMOTE_PRAGUE,                      // plugin identifier
		0x00000000,                             // subtype identifier
		0x00000001,								// interface version
		VID_MEZHUEV,							// interface developer
		&internal_proxy,						// internal(kernel called) function table
		(sizeof(iINTERNAL)/sizeof(tPTR)),		// internal function table size
		&root_iface,							// external function table
		(sizeof(iRootVtbl)/sizeof(tPTR)),       // external function table size
		NULL,									// property table
		0,										// property table size
		sizeof(PRConnection),					// memory size
		0                                       // interface flags
	);
}

tERROR PRRemoteManagerImpl::RegisterClientIFace()
{
	return CALL_Root_RegisterIFace(
		g_root,
		IID_REMOTE_PROXY,						// interface identifier
		PID_REMOTE_PRAGUE,                      // plugin identifier
		0x00000000,                             // subtype identifier
		0x00000001,								// interface version
		VID_MEZHUEV,							// interface developer
		&internal_proxy,						// internal(kernel called) function table
		(sizeof(iINTERNAL)/sizeof(tPTR)),		// internal function table size
		NULL,									// external function table
		0,										// external function table size
		prop_table,								// property table
		mPROPERTY_TABLE_SIZE(prop_table),		// property table size
		sizeof(PRRemoteProxy),					// memory size
		0                                       // interface flags
	);
}

tERROR PRRemoteManagerImpl::RegisterServerIFace()
{
	// Register server remote proxy interface
	tERROR error = CALL_Root_RegisterIFace(
		g_root,
		IID_REMOTE_STUB,						// interface identifier
		PID_REMOTE_PRAGUE,                      // plugin identifier
		0x00000000,                             // subtype identifier
		0x00000001,								// interface version
		VID_MEZHUEV,							// interface developer
		&internal_stub,							// internal(kernel called) function table
		(sizeof(iINTERNAL)/sizeof(tPTR)),		// internal function table size
		NULL,									// external function table
		0,										// external function table size
		NULL,									// property table
		0,										// property table size
		sizeof(PRRemoteProxy),					// memory size
		0                                       // interface flags
	);

	// Start RPC server

	PRRemoteHost host(m_ProccessId);
	rpc_registry_server(host.GetProtocol(), host.GetEndPoint().c_str());
	return errOK;
}

void PRRemoteManagerImpl::RegisterIFace(tIID iid, tPTR vtbl, tDWORD vtbl_size, iINTERNAL *internal_table, tDWORD data_size, tIID iid_compat)
{
	if( internal_table )
	{
		internal_table->ObjectInit = internal_proxy.ObjectInit;
		internal_table->ObjectClose = internal_proxy.ObjectClose;
		internal_table->MsgReceive = internal_proxy.MsgReceive;
	}
	else
		internal_table = &internal_proxy;

	tDWORD mem_size = data_size ? data_size + sizeof(tDWORD)*PR_SYS_SIZE : sizeof(PRRemoteProxy);

	tERROR error = CALL_Root_RegisterIFaceEx(
		g_root,
		NULL,
		iid,									// interface identifier
		PID_REMOTE_PRAGUE,                      // plugin identifier
		0x00000000,                             // subtype identifier
		0x00000001,								// interface version
		VID_MEZHUEV,							// interface developer
		internal_table,							// internal(kernel called) function table
		(sizeof(iINTERNAL)/sizeof(tPTR)),		// internal function table size
		vtbl,									// external function table
		vtbl_size/sizeof(tPTR),                 // external function table size
		prop_table,								// property table
		mPROPERTY_TABLE_SIZE(prop_table),		// property table size
		mem_size,								// memory size
		0,                                      // interface flags
		iid_compat,
		NULL,
		0
	);

//	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "rmt\tRegisterIFace iid=%d, error=%x", iid, error));
}

tERROR PRRemoteManagerImpl::RegisterServerHost(tDWORD port, const tCHAR* host)
{
	tULONG size = sizeof(m_host);
	if( !GetComputerName(m_host, &size) )
		return errUNEXPECTED;

	m_port = port;

	if( host && !strcmp(host, m_host) )
		m_port++;

	return rpc_registry_server(GetProtocol(), GetEndPoint().c_str()) ? errOK : errNOT_OK;
}

tERROR PRRemoteManagerImpl::RegisterRemoteHost(const tCHAR* host, tDWORD port, tPROCESS *process)
{
	set<PRRemoteHost>::iterator it = m_hosts.insert(PRRemoteHost(port, host)).first;
	if( process )
		*process = *it;
	return errOK;
}

tERROR PRRemoteManagerImpl::InitLocalPrague(hROOT root)
{
	g_root = root;

	CALL_SYS_PropertySetPtr(root, pgOUTPUT_FUNC, (void*)DbgTrace);
	CALL_SYS_TraceLevelSet(root, tlsALL_OBJECTS, prtNOTIFY, prtMIN_TRACE_LEVEL);

	InitRootIFace();
	InitSystemIFace();

	tPROPID prop;
	tERROR error = CALL_Root_RegisterCustomPropId(root, &prop, PR_REMOTE_MANAGER_PROP, pTYPE_PTR);
	if( PR_SUCC(error) )
		error = CALL_SYS_PropertySetPtr(root, prop, this);

	error = CALL_Root_RegisterCustomPropId(root, &prop, cpnPRAGUE_REMOTE_API, pTYPE_PTR);
	if( PR_SUCC(error) )
		error = CALL_SYS_PropertySetPtr(root, prop, &g_API);

	error = RegisterRootIFace();

	if( PR_SUCC(error) && m_params && m_params->init_clb )
		m_params->init_clb(g_root, m_params->ctx, cFALSE);

	if( PR_SUCC(error) )
		error = RegisterClientIFace();

	if( PR_SUCC(error) )
		error = RegisterServerIFace();

	return error;
}

tERROR PRRemoteManagerImpl::InitRemotePrague()
{
	tERROR l_error = CALL_SYS_ObjectCreateQuick(g_root, (hOBJECT*)&m_ienum, IID_IFACE_ENUM, PID_ANY, 0);

	if( PR_SUCC(l_error) )
		l_error = CALL_SYS_ObjectCreate(g_root, (hOBJECT*)&m_plugin, IID_PLUGIN, PID_ANY, 0);

	if( PR_SUCC(l_error) )
	{
#if defined (_WIN32)
		tCHAR module_name[MAX_PATH];
		GetModuleFileName((HINSTANCE)g_module, module_name, sizeof(module_name));
#elif defined (__unix__)
		tCHAR module_name [] = PR_LOADER_MODULE_NAME;
#endif
		CALL_SYS_PropertySetStr(m_plugin, 0, pgMODULE_NAME, module_name, (tDWORD)strlen(module_name), cCP_ANSI);
		CALL_SYS_PropertySetDWord(m_plugin, pgMODULE_ID, PID_REMOTE_PRAGUE);
		CALL_SYS_PropertySetPtr(m_plugin, plMODULE_INSTANCE, g_module);
		CALL_SYS_PropertySetBool(m_plugin, pgUNLOADABLE, cFALSE);
		l_error = CALL_SYS_ObjectCreateDone(m_plugin);
	}

	if( PR_SUCC(l_error) )
		l_error = CALL_Plugin_Load(m_plugin);

	if( PR_SUCC(l_error) )
	{
		PR_TRACE((m_plugin, prtALWAYS_REPORTED_MSG, "rmt\tRegister server"));
		if( m_params && m_params->init_clb )
			m_params->init_clb(g_root, m_params->ctx, cTRUE);
	}

	rpc_start_server();

	m_event.init(m_ProccessId);
	m_event.signal();

	return l_error;
}

PRConnection * PRRemoteManagerImpl::FindConnection(const PRRemoteHost &host, hROOT *root, enFindConnectType type, rpcOBJECT client_root)
{
	PRConnection *l_connect = NULL;
	bool f_created = false;

	m_lock.lock();

	hOBJECT _root;
	tERROR l_error = CALL_SYS_ChildGetFirst(g_root, &_root, IID_REMOTE_ROOT, PID_REMOTE_PRAGUE);
	while( PR_SUCC(l_error) )
	{
		PRConnection *_connect = CUST_ROOT(_root);
		if( *_connect == host )
		{
			l_connect = _connect;
			break;
		}
		l_error = CALL_SYS_ObjectGetNext(_root, &_root, IID_REMOTE_ROOT, PID_REMOTE_PRAGUE);
	}

	if( !l_connect && type != fctFindOnly )
	{
		if( PR_SUCC(CALL_SYS_ObjectCreateQuick(g_root, &_root, IID_REMOTE_ROOT, PID_REMOTE_PRAGUE, 0)) )
		{
			l_connect = CUST_ROOT(_root);
			l_connect->Init(_root, host);

			if( !host.m_local )
				RegisterRemoteHost(host.m_host, host.m_port, NULL);

			f_created = true;
		}
	}

	if( l_connect )
		l_connect->AddRef();

	m_lock.unlock();

	if( !l_connect )
		return NULL;

	l_error = errOK;
	if( f_created && l_connect->m_status == warnDEFAULT )
	{
		if( !client_root )
		{
			tSTRING host_name = NULL;
			tDWORD port = m_ProccessId;

			if( !l_connect->m_local )
			{
				if( !m_port )
					l_error = RegisterServerHost(host.m_port, host.m_host);

				port = m_port;
				host_name = m_host;
			}

			if( PR_SUCC(l_error) )
				PR_INVOKE_BASE(BindClient, prf_NOVALIDATE, l_connect->m_this,
					PR_ARG4(prf_OBJECT, tid_DWORD, tid_STRING, prf_OBJECT|tid_POINTER),
					PR_ARG4((rpcOBJECT)_root, port, host_name, &client_root));

			if( PR_FAIL(l_error) )
				PR_TRACE((g_root, prtERROR, "rmt\tRPC_BindClient failed"));
		}

		if( PR_SUCC(l_error) )
		{
			l_error = rpc_init_context_handle(l_connect->m_handle, &l_connect->m_context, (void*)client_root);

			if( PR_FAIL(l_error) )
				PR_TRACE((g_root, prtERROR, "rmt\tRPC_BindServer failed"));
		}

		if( PR_SUCC(l_error) )
		{
			PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "rmt\tConnection binded: remote_root=%d", (tDWORD)client_root));
			l_connect->m_object = client_root;
			l_connect->m_stub = client_root;
		}

		l_connect->m_status = l_error;
	}

	if( type != fctBindClient || client_root < (rpcOBJECT)_root ) // deadlock resolving
		while( l_connect->m_status == warnDEFAULT )
			Sleep(50);

	m_lock.lock();	

	l_connect->Release(cFALSE);

	if( PR_FAIL(l_connect->m_status) )
	{
		if( f_created )
			l_connect->Release(cTRUE);
		l_connect = NULL;
	}
	else if( root )
		*root = (hROOT)_root;

	m_lock.unlock();
	return l_connect;
}

handle_t PRRemoteManagerImpl::GetConnection(hOBJECT object)
{
	return CUST_OBJ(object)->m_connect->m_handle;
}

hOBJECT PRRemoteManagerImpl::GetObjectStub(tPROCESS process, hOBJECT object)
{
//	PRAutoLocker locker(&m_lock);
	if( process == m_ProccessId || process == PR_PROCESS_LOCAL )
		return NULL;

	PRRemoteProxy *l_proxy = IS_OBJ_PROXY(object) ? CUST_PROXY(object) : NULL;

	if( process == PR_PROCESS_ANY )
		return l_proxy ? object : NULL;

	PRConnection * l_connect = FindConnection(GetHost(process));
	if( !l_connect )
		return NULL;

	if( l_proxy && l_proxy->m_connect == l_connect )
		return object;

	g_mng.m_stub_lock.lock();

	hOBJECT stub = l_connect->m_this;
	if( l_connect->GetProxy(object) )
	{
		ENUM_STUB_BEGIN(object)
			if( l_stub->m_connect == l_connect )
			{
				stub = _stub;
				break;
			}
		ENUM_STUB_END()
	}

	g_mng.m_stub_lock.unlock();
	return stub;
}

rpcOBJECT PRRemoteManagerImpl::GetProxy(hOBJECT remote, hOBJECT object, tLONG ref_count)
{
	PRConnection * connect = CUST_OBJ(remote)->m_connect;
	rpcOBJECT proxy = connect->GetProxy(object, ref_count > 0);

	if( ref_count < 0 && IS_OBJ_PROXY(object) )
		connect->ReleaseObject(object, cFALSE);

	return proxy;
}

hOBJECT PRRemoteManagerImpl::ReleaseProxy(hOBJECT remote, rpcOBJECT object)
{
	PRAutoLocker locker(m_stub_lock);
	if( !IS_VALID_REMOTE(remote) )
		return NULL;

	PRConnection * connect = CUST_OBJ(remote)->m_connect;
	ENUM_STUB_BEGIN((hOBJECT)object)
		if( l_stub->m_connect == connect )
		{
			connect->ReleaseStub(_stub, cFALSE);
			break;
		}
	ENUM_STUB_END()
	return (hOBJECT)object;
}

PRRemoteHost PRRemoteManagerImpl::GetHost(tPROCESS process)
{
	for(set<PRRemoteHost>::iterator it = m_hosts.begin(); it != m_hosts.end(); ++it)
		if( process == *it )
			return *it;

	return process;
}

tPROCESS PRRemoteManagerImpl::GetObjectProcess(hOBJECT object)
{
	if( !IS_OBJ_PROXY(object) )
		return m_ProccessId;

	PRConnection * connect = CUST_PROXY(object)->m_connect;
	if( connect->m_local == PR_PROCESS_ANY )
		return connect->m_plocal;
	if( connect->m_local )
		return connect->m_local;

	set<PRRemoteHost>::iterator it = m_hosts.find(*connect);
	return it != m_hosts.end() ? (tPROCESS)*it : PR_PROCESS_ANY;
}

tERROR PRRemoteManagerImpl::BindObjectOpen( PRConnection *connect, rpcOBJECT object, tSTRING name, rpcOBJECT* remote_object)
{
	PR_INVOKE_BASE(BindObjectOpen, 0, connect->m_this,
		PR_ARG3(prf_OBJECT, tid_STRING, prf_OBJECT|tid_POINTER), PR_ARG3(object, name, remote_object));
	return errOK;
}

hOBJECT PRRemoteManagerImpl::GetObjectProxy(tPROCESS process, tSTRING name)
{
	rpcOBJECT l_object = NULL, l_remote_object = NULL;

	if( process == PR_PROCESS_LOCAL )
		process = m_ProccessId;

	bool fAny = (process == PR_PROCESS_ANY);

	PRRemoteHost host = GetHost(fAny ? 0 : process);

	if( host.m_local || fAny )
	{
		PRShareObjectKey l_key = {name, 0};
		PRShareObject sobj;
		hRecord l_record = FindRecord(&l_key, 0, host.m_local, &sobj);
		if( l_record != SHTR_INVALID_HANDLE )
		{
			l_object = sobj.m_hObject;
			if( fAny )
			{
				host.m_local = GetRecordProcess(l_record);
				fAny = false;
			}

			if( host.m_local == m_ProccessId )
				return (hOBJECT)l_object;
		}
		else if( !fAny )
		{
			PR_TRACE((g_root, prtERROR, "rmt\tobject '%s' not found in table", name));
			return NULL;
		}
	}

	if( !fAny )
	{
		PRConnection *l_connect = FindConnection(host);
		if( !l_connect )
		{
			if( !host.m_local )
				return NULL;

			PR_TRACE((g_root, prtERROR, "rmt\tcannot find connection to pid=%d for '%s'", host.m_local, name));
			if( host.m_local )
				ClearProcessRecords(host.m_local);
			return GetObjectProxy(process, name);
		}

		BindObjectOpen(l_connect, l_object, name, &l_remote_object);
	}
	else
	{
		for( set<PRRemoteHost>::iterator it = m_hosts.begin(); it != m_hosts.end(); ++it )
		{
			PRConnection *l_connect = FindConnection(*it);
			if( !l_connect )
				continue;

			BindObjectOpen(l_connect, l_object, name, &l_remote_object);

			if( l_remote_object )
				break;
		}
	}

	return (hOBJECT)l_remote_object;
}

//************************************************************************

tERROR PRRemoteManagerImpl::InvokeEx(hOBJECT _this, tDWORD flags, tDWORD func, ...)
{
	va_list args;
	va_start(args, func);

	tDWORD types[10], *_types = types;
	while(1) if( !(*_types++ = va_arg(args, tDWORD)) ) break;

	tERROR err = Invoke(_this, flags, func, types, args);
	va_end(args);
	return err;
}

tERROR PRRemoteManagerImpl::Invoke(hOBJECT _this, tDWORD flags, tDWORD func, tDWORD* types, va_list args)
{
	PRRemoteObject* remote = CUST_OBJ(_this);
	handle_t connect = remote->m_connect->m_handle;
	rpcOBJECT stub;

	if( IS_OBJ_PROXY(_this) )
		stub = ((PRRemoteProxy*)remote)->m_stub;
	else
	{
		flags |= prf_STUBCALL;
		stub = remote->m_object;
	}

	tERROR error = errOK;
	tDWORD param_data[0x1000];
	tPTR   buff = param_data;
	tDWORD size_real = sizeof(param_data);

	tProxyCall* call = (tProxyCall*)buff;
	call->obj = stub;
	call->func = func;
	call->flags = flags;
	call->numprm = 0;

	tDWORD offset = offsetof(tProxyCall, params);
	tDWORD size_avail = size_real - sizeof(tProxyCall);
	rpcOBJECT tmpobj; tDWORD tmp1; tQWORD tmp2;

	tProxyParam* param = NULL;
	while(1)
	{
		tDWORD type = *types++;
		if( !type )
			break;

		call->numprm++;

		if( param )
			param->next = offset;

		bool bvalue = !(type & tid_POINTER);

		param = (tProxyParam*)((tBYTE*)buff + offset);
		param->type = type;
		param->next = 0;

		tPTR data_ptr = bvalue ? NULL : va_arg(args, tPTR);
		tPTR data = bvalue ? NULL : data_ptr;
		switch( TID_TYPE(type) )
		{
		case tid_BOOL:
		case tid_DWORD:
		case tid_PROPID:
		case tid_CODEPAGE:
			if( bvalue )
			{
				tmp1 = va_arg(args, tDWORD);
				data = &tmp1;
			}
			param->size = sizeof(tDWORD);
			break;

		case tid_DATETIME:
		case tid_QWORD: 
			if( bvalue )
			{
				tmp2 = va_arg(args, tQWORD);
				data = &tmp2; 
			}
			param->size = sizeof(tQWORD);
			break;

		case tid_STRING:
			data = va_arg(args, tSTRING);
			param->size = data ? (tDWORD)strlen((char*)data)+1 : 0;
			break;

		case tid_WSTRING:
			data = va_arg(args, tSTRING);
			param->size = data ? sizeof(tWCHAR)*((tDWORD)wcslen((tWCHAR*)data)+1) : 0;
			break;

		case tid_BINARY:
			data_ptr = data = va_arg(args, tPTR);
			if( type & prf_OUT )
				data = NULL;

			if( type & prf_SIZEPTR )
			{
				tDWORD* size_ptr = va_arg(args, tDWORD*);
				param->size = size_ptr ? *size_ptr : 0;
			}
			else
				param->size = va_arg(args, tDWORD);
			break;

		case tid_SERIALIZABLE:
			data_ptr = data = va_arg(args, tPTR);
			param->size = 0;

			if( data )
			{
				error = CALL_Root_StreamSerialize(g_root, (cSerializable*)data, SERID_UNKNOWN,
					param->data, size_avail, &param->size, STREAM_PACK_OPS);

				if( PR_SUCC(error) || error == errBUFFER_TOO_SMALL )
					param->size += SER_BUFFER;
			}
			break;

		case tid_OBJECT:
			tmpobj = 0;
			if( type & prf_INBUFF )
			{
				hOBJECT* pobj = va_arg(args, hOBJECT*);
				if( type & prf_IN )
					tmpobj = GetProxy(_this, *pobj, 0);
				else
					data_ptr = pobj, type &= ~prf_OUT;
			}
			else if( bvalue )
				tmpobj = GetProxy(_this, va_arg(args, hOBJECT), (type & prf_DEREF) ? -1 : 0);

			data = &tmpobj;
			param->size = sizeof(rpcOBJECT);
			break;
		}

		param->data_ptr = (rpcOBJECT)data_ptr;

		tDWORD param_size = param->size + sizeof(tProxyParam) - 1;
		if( size_avail < param_size )
		{
			tDWORD new_size = size_real +
				(param->size > sizeof(param_data) ? param->size : sizeof(param_data));

			if( buff == param_data )
			{
				buff = malloc(new_size);
				memcpy(buff, param_data, size_real);
			}
			else
				buff = realloc(buff, new_size);

			size_avail += new_size - size_real;
			size_real = new_size;

			call = (tProxyCall*)buff;
			param = (tProxyParam*)((tBYTE*)buff + offset);
		}

		if( TID_TYPE(type) == tid_SERIALIZABLE )
		{
			if( error == errBUFFER_TOO_SMALL )
			{
				CALL_Root_StreamSerialize(g_root, (cSerializable*)data, SERID_UNKNOWN,
					param->data, size_avail, NULL/*!!! &param->size*/, STREAM_PACK_OPS);
			}
		}
		else if( data && !(type & prf_OUT) )
		{
			memcpy(param->data, data, param->size);
		}

		size_avail -= param_size;
		offset += param_size;
	}

	if( !connect || !stub && !(flags & prf_NOVALIDATE) )
		error = errPROXY_STATE_INVALID;
	else
	{
		error = rpc_send_receive(connect, (tCHAR*)buff, offset);

		param = call->numprm ? call->params : NULL;
		while(param)
		{
			if( TID_TYPE(param->type) == tid_SERIALIZABLE )
			{
				if( param->size )
					CALL_Root_StreamDeserialize(g_root, (cSerializable**)&param->data_ptr, param->data, param->size, NULL);
			}
			else if( param->data_ptr && !(param->type & prf_IN) )
			{
				if( TID_TYPE(param->type) == tid_OBJECT )
					param->size = sizeof(hOBJECT);

				memcpy((tPTR)param->data_ptr, param->data, param->size);
			}

			param = param->next ? (tProxyParam*)((tBYTE*)buff + param->next) : NULL;
		}
	}

	if( buff != param_data )
		free(buff);

	return error;
}

long rpc_send_receive_server(handle_t handle, void* buff, ulong size)
{
	tProxyCall* call = (tProxyCall*)buff;

	hOBJECT obj = (hOBJECT)call->obj;
	tDWORD flags = call->flags;

	tERROR error = errOK;
	if( !(flags & prf_NOVALIDATE) )
		error = CALL_SYS_ValidateLock(g_root, obj, IID_ANY, (flags & prf_STUBCALL) ? PID_ANY : PID_REMOTE_PRAGUE);

	if( error == errOK && obj && !IS_ROOT_PROXY(obj) && !(flags & prf_STUBCALL) )
	{
		hOBJECT _obj = obj;
		__TRY{ obj = CALL_SYS_ParentGet(obj, IID_ANY); } __EXCEPT(1){ return errPROXY_STATE_INVALID; }

		if( !(flags & prf_NOVALIDATE) )
		{
			error = CALL_SYS_ValidateLock(g_root, obj, IID_ANY, PID_ANY);
			CALL_SYS_Release(_obj);
		}
	}

	if( error != errOK )
	{
		if( PR_SUCC(error) && obj )
			CALL_SYS_Release(obj);

		return errPROXY_STATE_INVALID;
	}

	if( flags & prf_IMPERSONATE )
		rpc_impersonate(handle, 1);

#if defined ( USE_FSDRVLIB )
	if( !TlsGetValue(g_mng.m_rpc_tls) )
	{
		TlsSetValue(g_mng.m_rpc_tls, g_module);
		FSDrvInterceptorRegisterInvisibleThread();
	}
#endif

	tPTR p[16];
	tBYTE* stack = (tBYTE*)p;

	*(hOBJECT*)stack = obj, stack += sizeof(hOBJECT);

	tProxyParam* param = call->numprm ? call->params : NULL;
	while(param)
	{
		tPTR data_ptr = param->data;
		bool is_ptr = !!(param->type & tid_POINTER), is_null = !param->data_ptr, is_size = false;
		switch( TID_TYPE(param->type) )
		{
		case tid_STRING:
		case tid_WSTRING:
			is_ptr = true;
			is_null = !param->size;
			break;

		case tid_BINARY:
			is_ptr = is_size = true;
			break;

		case tid_SERIALIZABLE:
			is_ptr = true;
			data_ptr = NULL;
			if( param->size )
				if( PR_SUCC(CALL_Root_StreamDeserialize(g_root, (cSerializable**)&data_ptr,
						param->data, param->size - SER_BUFFER, NULL)) )
					*(tPTR*)param->data = data_ptr;
				else
					param->size = 0;
			break;

		case tid_OBJECT:
			if( param->type & prf_INBUFF )
			{
				is_size = true;
				is_ptr = true;
				is_null = false;
			}
			param->size = sizeof(hOBJECT);
			break;
		}

		if( is_ptr )
		{
			*(tPTR*)stack = is_null ? NULL : data_ptr; stack += sizeof(tPTR);
			if( is_size )
			{
				if( param->type & prf_SIZEPTR )
					*(tPTR*)stack = &param->size, stack += sizeof(tPTR);
				else
					*(tPTR*)stack = (tPTR)param->size, stack += sizeof(tPTR);
			}
		}
		else
		{
			memcpy(stack, data_ptr, param->size);
			stack += param->size;

			if( param->size < sizeof(tPTR) )
			{
				memset(stack, 0, sizeof(tPTR)-param->size);
				stack += sizeof(tPTR)-param->size;
			}
		}

		param = param->next ? (tProxyParam*)((tBYTE*)buff + param->next) : NULL;
	}

	tPTR func_addr;
	call->func *= sizeof(tPTR);
	if( flags & prf_REMOTEAPI )
		func_addr = *(tPTR*)((tBYTE*)&remote_iface + call->func);
	else if( flags & prf_SYTEMAPI )
		func_addr = *(tPTR*)((tBYTE*)obj->sys + call->func);
	else
		func_addr = *(tPTR*)((tBYTE*)obj->vtbl + call->func);

	switch((tPTR*)stack-p)
	{
	case 1:  { tERROR (pr_call* f)(tPTR); *(tPTR*)&f = func_addr; error = f(p[0]); } break;
	case 2:  { tERROR (pr_call* f)(tPTR,tPTR); *(tPTR*)&f = func_addr; error = f(p[0],p[1]); } break;
	case 3:  { tERROR (pr_call* f)(tPTR,tPTR,tPTR); *(tPTR*)&f = func_addr; error = f(p[0],p[1],p[2]); } break;
	case 4:  { tERROR (pr_call* f)(tPTR,tPTR,tPTR,tPTR); *(tPTR*)&f = func_addr; error = f(p[0],p[1],p[2],p[3]); } break;
	case 5:  { tERROR (pr_call* f)(tPTR,tPTR,tPTR,tPTR,tPTR); *(tPTR*)&f = func_addr; error = f(p[0],p[1],p[2],p[3],p[4]); } break;
	case 6:  { tERROR (pr_call* f)(tPTR,tPTR,tPTR,tPTR,tPTR,tPTR); *(tPTR*)&f = func_addr; error = f(p[0],p[1],p[2],p[3],p[4],p[5]); } break;
	case 7:  { tERROR (pr_call* f)(tPTR,tPTR,tPTR,tPTR,tPTR,tPTR,tPTR); *(tPTR*)&f = func_addr; error = f(p[0],p[1],p[2],p[3],p[4],p[5],p[6]); } break;
	case 8:  { tERROR (pr_call* f)(tPTR,tPTR,tPTR,tPTR,tPTR,tPTR,tPTR,tPTR); *(tPTR*)&f = func_addr; error = f(p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]); } break;
	case 9:  { tERROR (pr_call* f)(tPTR,tPTR,tPTR,tPTR,tPTR,tPTR,tPTR,tPTR,tPTR); *(tPTR*)&f = func_addr; error = f(p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8]); } break;
	case 10: { tERROR (pr_call* f)(tPTR,tPTR,tPTR,tPTR,tPTR,tPTR,tPTR,tPTR,tPTR,tPTR); *(tPTR*)&f = func_addr; error = f(p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9]); } break;
	}

	param = call->numprm ? call->params : NULL;
	while(param)
	{
		switch( TID_TYPE(param->type) )
		{
		case tid_SERIALIZABLE:
			if( param->size )
			{
				cSerializable* data = *(cSerializable**)param->data;
				if( data )
				{
					tDWORD size = 0;
					tERROR err = CALL_Root_StreamSerialize(g_root, data, SERID_UNKNOWN,
						param->data, param->size, &param->size, STREAM_PACK_OPS);

					if( PR_FAIL(err) )
					{
						if( err == errBUFFER_TOO_SMALL )
						{
							tBYTE* buff = new tBYTE[param->size];
							err = CALL_Root_StreamSerialize(g_root, data, SERID_UNKNOWN,
								buff, param->size, &param->size, STREAM_PACK_OPS);

							PR_INVOKE_BASE(Deserialize, 0, call->obj, PR_ARG2(prf_OBJECT, tid_BINARY|prf_IN),
								PR_ARG3(param->data_ptr, buff, param->size));

							delete[] buff;
						}
						param->size = 0;
					}
					CALL_Root_DestroySerializable(g_root, data);
				}
			}
			break;

		case tid_OBJECT:
			if( param->type & prf_INBUFF )
			{
				if( param->type & prf_OUT )
					*(rpcOBJECT*)param->data = g_mng.GetProxy((hOBJECT)call->obj,
						*(hOBJECT*)param->data, 0);
			}
			else if( (param->type & tid_POINTER) && param->data_ptr )
			{
				hOBJECT obj = *(hOBJECT*)param->data;
				*(rpcOBJECT*)param->data = g_mng.GetProxy((hOBJECT)call->obj,
					obj, ((param->type & prf_REF) || obj && !IS_OBJ_PROXY(obj)) ? 1 : 0);

				if( obj && !*(rpcOBJECT*)param->data )
					error = errUNEXPECTED;
			}
			break;
		}

		param = param->next ? (tProxyParam*)((tBYTE*)buff + param->next) : NULL;
	}

	if( obj && !(flags & prf_NOVALIDATE) )
		CALL_SYS_Release(obj);

	if( flags & prf_IMPERSONATE )
		rpc_impersonate(handle, 0);

	return error;
}

//************************************************************************
// Validate methods

void PRRemoteManagerImpl::UpdateValidateList(hOBJECT object, bool bnew)
{
	PRAutoLocker locker(m_validate_lock);
	if( bnew )
	{
		m_objects.push_back(object);
		return;
	}

	int count = (int)m_objects.size();
	for(int i = 0; i < count; i++)
		if( m_objects[i] == object )
		{
			m_objects.erase(m_objects.begin() + i);
			return;
		}
}

bool PRRemoteManagerImpl::IsObjectValid(hOBJECT object)
{
	PRAutoLocker locker(m_validate_lock);
	int count = (int)m_objects.size();
	for(int i = 0; i < count; i++)
		if( m_objects[i] == object )
			return true;

//	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "rmt\tObject validation failed: object=%08X", object));
	return false;
}

// Shared objects
bool PRRemoteManagerImpl::CompareKey(hRecord record, void *key, void *data)
{
	PRShareObjectKey *l_key = (PRShareObjectKey*)key;
	PRShareObject *l_obj = (PRShareObject*)data;
	return l_key->m_FindName ?
		!strcmp(l_key->m_FindName, l_obj->m_idRegister) :
		l_key->m_hObject == l_obj->m_hObject;
}

bool PRRemoteManagerImpl::InitKeyData(hRecord record, void *key, void *data)
{
	PRShareObjectKey *l_key = (PRShareObjectKey*)key;
	PRShareObject *l_obj = (PRShareObject*)data;
	l_obj->m_hObject = l_key->m_hObject;
	strcpy(l_obj->m_idRegister, l_key->m_FindName);
	return true;
}

//************************************************************************

bool PRRemoteManagerImpl::IsLocalCreation(hOBJECT& parent, tPID pid, bool bRecognize)
{
	PRCreationPIDInfo* pInfo = NULL;
	for(size_t i = 0; i < m_pids.size(); i++)
		if( m_pids[i].m_Pid == pid )
		{
			pInfo = &m_pids[i];
			break;
		}

	if( !pInfo )
		return true;

	if( IS_OBJ_PROXY(parent) )
		return !(pInfo->m_Flags & PR_CREATE_IN_PARENTAL_PROCESS);

	if( bRecognize )
		return true;

	if( pInfo->m_Flags & PR_CREATE_IN_SPECIFIED_PROCESS )
	{
		parent = GetObjectStub(pInfo->m_ProcessId, parent);
		return false;
	}

	if( !(pInfo->m_Flags & PR_CREATE_IN_IMPERSONATED_PROCESS) )
		return true;

	hTOKEN hToken = NULL;
	tCHAR user_name[MAX_PATH] = "";

	CALL_SYS_ObjectCreateQuick(g_root, &hToken, IID_TOKEN, PID_ANY, SUBTYPE_ANY);
	if( hToken )
	{
		CALL_SYS_PropertyGetStr(hToken, NULL, pgOBJECT_NAME, user_name, sizeof(user_name), cCP_ANSI);
		CALL_SYS_ObjectClose(hToken);
	}

	if( !*user_name )
		return true;

	PRAutoLocker locker(m_process_lock);
	PRRunningProcess pPInfo, *pFound = NULL;
	pPInfo.m_UserName = user_name;

	for(size_t i = 0; i < m_processes.size(); i++)
		if( m_processes[i].m_UserName == pPInfo.m_UserName )
		{
			pFound = &m_processes[i];
			break;
		}

	if( !pFound )
	{
		PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "rmt\tCreateProcess for user %s, pid(%d)",
			user_name, pid));

		if( PR_SUCC(PRCreateProcess(PR_SESSION_BY_USER_TOKEN, (tSTRING)pInfo->m_ExeName.c_str(),
				"-host -ne", 100000, cTRUE, &pPInfo.m_ProcessId)) )
			pFound = &pPInfo;
	}

	if( !pFound )
		return true;

	if( pFound == &pPInfo )
	{
		pPInfo.m_ref = 0;
		m_processes.push_back(pPInfo);
		pFound = &*m_processes.rbegin();
	}

	parent = GetObjectStub(pFound->m_ProcessId, parent);
	if( parent )
	{
		PRRemoteObject *l_remote = CUST_OBJ(parent);
		if( !l_remote->m_is_pidref )
		{
			l_remote->m_is_pidref = cTRUE;
			pFound->m_ref++;
		}
	}

	return false;
}

bool PRRemoteManagerImpl::ReleaseProcess(tPROCESS pid)
{
	PRAutoLocker locker(m_process_lock);

	for(size_t i = 0; i < m_processes.size(); i++)
		if( m_processes[i].m_ProcessId == pid )
		{
			if( !--m_processes[i].m_ref )
			{
				PRCloseProcessRequest(pid);
				m_processes.erase(m_processes.begin() + i);
			}
			return true;
		}

	return false;
}

//************************************************************************

tERROR PRBindClient(hOBJECT, rpcOBJECT root, tDWORD process, tSTRING host, rpcOBJECT *client_root)
{
	PRConnection *l_connect = g_mng.FindConnection(PRRemoteHost(process, host), (hROOT*)client_root, fctBindClient, root);
	return l_connect ? errOK : errNOT_OK;
}

tERROR PRBindObjectOpen(hOBJECT root, rpcOBJECT object, tSTRING name, rpcOBJECT *proxy)
{
	if( !root )
		return errUNEXPECTED;

	hOBJECT l_object = (hOBJECT)object;
	if( !l_object )
		l_object = g_mng.GetObjectProxy(PR_PROCESS_LOCAL, name);

	if( PR_FAIL(CALL_SYS_ValidateLock(g_root, l_object, IID_ANY, PID_ANY)) )
		return errOBJECT_INVALID;

	*proxy = CUST_ROOT(root)->GetProxy(l_object, cTRUE);
	CALL_SYS_Release(l_object);
	return errOK;
}

tERROR PRBindObjectClose(hOBJECT root, rpcOBJECT object)
{
	if( PR_SUCC(CALL_SYS_ObjectValid(g_root, (hOBJECT)object)) )
		CALL_SYS_ObjectClose((hOBJECT)object);
	return errOK;
}

long rpc_rundown_client(void* context_handle, long shutdown)
{
	g_mng.RunDownClient((hOBJECT)context_handle, shutdown);
	return 0;
}

//************************************************************************
// proxies/stubs methods

tERROR PRProxyCreate(hOBJECT root, rpcOBJECT parent, tBOOL is_ref, tProxyCreateInfo *info, tDWORD size, rpcOBJECT *proxy)
{
	PRAutoLocker locker(g_mng.m_proxy_lock);

	hOBJECT l_parent = (hOBJECT)parent;

	tERROR error = CALL_SYS_ValidateLock(g_root, l_parent, IID_ANY, PID_ANY);
	if( PR_SUCC(error) )
	{
		error = CUST_ROOT(root)->CreateProxy(l_parent, is_ref, info, proxy);
		CALL_SYS_Release(l_parent);
	}
	return error;
}

tERROR PRProxyCreateRemote(hOBJECT _this, tPROCESS pid, rpcOBJECT* proxy)
{
	if( !proxy )
		return errPARAMETER_INVALID;

	PRConnection *l_connect = g_mng.FindConnection(g_mng.GetHost(pid));
	if( l_connect )
		*proxy = l_connect->GetProxy(_this);

	return *proxy ? errOK : errNOT_FOUND;
}

tERROR PRProxySetData(hOBJECT root, rpcOBJECT proxy, tCHAR *data, tDWORD data_size)
{
	hOBJECT l_object = (hOBJECT)proxy;

	PRAutoLocker locker(g_mng.m_proxy_lock);
	if( IS_VALID_REMOTE(l_object) )
	{
		tDWORD l_data_size;
		CALL_SYS_ObjectSetInternalData(l_object, &l_data_size, sizeof(rpcOBJECT),
			(tBYTE*)data, data_size, PRObjBackwardConvertFunc, NULL);
		CALL_SYS_PropertySetBool(l_object, psCALLBACK_SENDER, cTRUE);
	}
	return errOK;
}

tERROR PRProxyAddRef(hOBJECT root, rpcOBJECT proxy)
{
	return PRAddRefObjectProxy((hOBJECT)proxy);
}

tERROR PRProxyObjectCreate( hOBJECT _this, tVOID* new_obj, tIID iid, tPID pid, tDWORD subtype )
{
	return LOCAL_SYS->ObjectCreate(_this, new_obj, iid, pid, subtype);
}

tERROR PRProxyObjectCreateQuick( hOBJECT _this, tVOID* new_obj, tIID iid, tPID pid, tDWORD subtype )
{
	return LOCAL_SYS->ObjectCreateQuick(_this, new_obj, iid, pid, subtype);
}

extern tRegisterMsgHandlerList		fRegisterMsgHandlerList;
extern tUnregisterMsgHandlerList	fUnregisterMsgHandlerList;

tERROR PRProxyRegisterMsgHandlerList(hOBJECT _this, const tProxyMsgHandlerDescr* handler_list, tDWORD handler_count)
{
	handler_count /= sizeof(tProxyMsgHandlerDescr);

	tMsgHandlerDescr l_reg_info[MAX_MHL_SIZE];
	for(tDWORD i = 0; i < handler_count; i++)
	{
		l_reg_info[i].object = (hOBJECT)handler_list[i].object;
		l_reg_info[i].flags = handler_list[i].flags;
		l_reg_info[i].msg_cls = handler_list[i].msg_cls;
		l_reg_info[i].obj_iid = handler_list[i].obj_iid;
		l_reg_info[i].obj_pid = handler_list[i].obj_pid;
		l_reg_info[i].ctx_iid = handler_list[i].ctx_iid;
		l_reg_info[i].ctx_pid = handler_list[i].ctx_pid;
	}
	return fRegisterMsgHandlerList(_this, l_reg_info, handler_count);
}

tERROR PRProxyUnregisterMsgHandlerList(hOBJECT _this, const tProxyMsgHandlerDescr* handler_list, tDWORD handler_count)
{
	handler_count /= sizeof(tProxyMsgHandlerDescr);

	tMsgHandlerDescr l_reg_info[MAX_MHL_SIZE];
	for(tDWORD i = 0; i < handler_count; i++)
	{
		l_reg_info[i].object = (hOBJECT)handler_list[i].object;
		l_reg_info[i].flags = handler_list[i].flags;
		l_reg_info[i].msg_cls = handler_list[i].msg_cls;
		l_reg_info[i].obj_iid = handler_list[i].obj_iid;
		l_reg_info[i].obj_pid = handler_list[i].obj_pid;
		l_reg_info[i].ctx_iid = handler_list[i].ctx_iid;
		l_reg_info[i].ctx_pid = handler_list[i].ctx_pid;
	}
	return fUnregisterMsgHandlerList(_this, l_reg_info, handler_count);
}

tERROR PRProxyMsgReceive(hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT recieve_point, tBYTE* pbuff, tDWORD* blen)
{
	if( msg_cls == pmc_REMOTE_GLOBAL )
		return CALL_SYS_SendMsg(g_mng.m_plugin, msg_cls, msg_id, ctx, pbuff, blen);

	return CALL_SYS_IntMsgReceive(_this, msg_cls, msg_id, send_point, ctx, recieve_point, pbuff, blen);
}

tERROR PRProxyMsgReceiveSer(hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT recieve_point, cSerializable* ser)
{
	if( msg_cls == pmc_REMOTE_GLOBAL )
		return CALL_SYS_SendMsg(g_mng.m_plugin, msg_cls, msg_id, ctx, ser, SER_SENDMSG_PSIZE);

	return CALL_SYS_IntMsgReceive(_this, msg_cls, msg_id, send_point, ctx, recieve_point,
		ser, SER_SENDMSG_PSIZE);
}

//************************************************************************

tERROR PRCloseProxy(hOBJECT root, rpcOBJECT proxy)
{
	hOBJECT l_object = (hOBJECT)proxy;

	PRAutoLocker locker(g_mng.m_proxy_lock);
	if( IS_VALID_REMOTE(l_object) && IS_OBJ_PROXY(l_object) )
		CUST_OBJ(proxy)->m_connect->ReleaseProxy(l_object, cTRUE);
	return errOK;
}

tERROR PRCloseStub(hOBJECT root, rpcOBJECT stub)
{
	hOBJECT l_object = (hOBJECT)stub;

	PRAutoLocker locker(g_mng.m_stub_lock);
	if( IS_VALID_REMOTE(l_object) && !IS_OBJ_PROXY(l_object) )
		CUST_OBJ(stub)->m_connect->ReleaseStub(l_object, cTRUE);
	return errOK;
}

//************************************************************************

tERROR PRAttachAsChild(hOBJECT root)
{
	if( !g_root )
		return errUNEXPECTED;

	CUST_ROOT(root)->m_attached = cTRUE;
	return errOK;
}

tERROR PRCloseRequest(hOBJECT root)
{
	if( !g_root )
		return errUNEXPECTED;
	return CALL_SYS_SendMsg(g_root, pmc_REMOTE_CLOSE_REQUEST, 0, NULL, NULL, NULL);
}

tERROR PRDeserialize(hOBJECT root, rpcOBJECT ser, tBYTE* buffer, tDWORD size)
{
	if( !buffer )
		return errOK;

	cSerializable* data = (cSerializable*)ser;
	CALL_Root_StreamDeserialize(g_root, &data, buffer, size, NULL);
	return errOK;
}

tERROR PRImpersonateAsAdminEx(hOBJECT proxy, tPROCESS pid, tQWORD hThread)
{
#if defined (_WIN32)
	HANDLE hProcess = NULL, hDupThread = NULL, hToken = NULL, hLinkedToken = NULL, hTokenNew = NULL;
	DWORD dwOutSize;
	tERROR error = errUNEXPECTED;

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_DUP_HANDLE, FALSE, pid);

	if( hProcess )
	{
		OpenProcessToken(hProcess, TOKEN_QUERY|TOKEN_IMPERSONATE|TOKEN_DUPLICATE, &hToken);
		DuplicateHandle(hProcess, (HANDLE)hThread, GetCurrentProcess(), &hDupThread, THREAD_ALL_ACCESS, FALSE, 0);
	}

	if( hToken )
		GetTokenInformation(hToken, (TOKEN_INFORMATION_CLASS)19, &hLinkedToken, sizeof(hLinkedToken), &dwOutSize);

	if( hLinkedToken )
		if( !DuplicateToken(hLinkedToken, SecurityImpersonation, &hTokenNew) )
			PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "rmt\tPRImpersonateAsAdminEx DuplicateToken failed(%x)", GetLastError()));

	if( hDupThread && hTokenNew )
		if( SetThreadToken(&hDupThread, hTokenNew) )
		{
			PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "rmt\tPRImpersonateAsAdminEx ok"));
			error = errOK;
		}
		else
			PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "rmt\tPRImpersonateAsAdminEx SetThreadToken failed(%x)", GetLastError()));

	if( hProcess )
		CloseHandle(hProcess);

	if( hDupThread )
		CloseHandle(hDupThread);

	if( hToken )
		CloseHandle(hToken);

	if( hLinkedToken )
		CloseHandle(hLinkedToken);

	if( hTokenNew )
		CloseHandle(hTokenNew);

	return error;
#endif //_WIN32
}


//************************************************************************

PRRemoteInterface remote_iface =
{
	PRBindClient,
	PRBindObjectOpen,
	PRBindObjectClose,

	PRProxyCreate,
	PRProxyCreateRemote,
	PRProxySetData,
	PRProxyAddRef,

	PRCloseProxy,
	PRCloseStub,

	PRProxyObjectCreate,
	PRProxyObjectCreateQuick,
	PRProxyRegisterMsgHandlerList,
	PRProxyUnregisterMsgHandlerList,
	PRProxyMsgReceive,
	PRProxyMsgReceiveSer,

	PRAttachAsChild,
	PRCloseRequest,

	PRDeserialize,

	PRImpersonateAsAdminEx
};

//************************************************************************
// Local prague

tERROR PRInitialize(tDWORD flags, PRInitParams* params)
{
	tERROR error = g_mng.Initialize(flags,params);
	return error;
}

tERROR PRDeinitialize()
{
	return g_mng.Deinitialize();
}

tERROR PRGetRoot(hROOT *root)
{
	*root = g_root;
	return errOK;
}

tERROR PRStopServer()
{
	return g_mng.StopServer();
}

tERROR PRRegisterHost(tSTRING host, tDWORD port, tPROCESS *process)
{
	return g_mng.RegisterRemoteHost(host, port, process);
}

tERROR PRRegisterServer(tDWORD port)
{
	return g_mng.RegisterServerHost(port);
}

tERROR PRRegisterObject(tSTRING name, hOBJECT object)
{
	PRShareObjectKey l_key = {name, (rpcOBJECT)object};
	hRecord l_record = g_mng.ReplaceRecord(&l_key, g_mng.m_ProccessId);
	return l_record == SHTR_INVALID_HANDLE ? errOBJECT_CANNOT_BE_INITIALIZED : errOK;
}

tERROR PRUnregisterObject(hOBJECT object)
{
	PRShareObjectKey l_key = {NULL, (rpcOBJECT)object};
	g_mng.DeleteRecord(&l_key, g_mng.m_ProccessId);
	return errOK;
}

tERROR PRCreateObject(tPROCESS process, hOBJECT object, hOBJECT* new_obj, tIID iid, tPID pid, tDWORD subtype)
{
	hOBJECT _this = g_mng.GetObjectStub(process, object);

	if( _this )
		return PR_INVOKE_BASE(ProxyObjectCreate, 0, _this, PR_ARG4(tid_OBJECT|tid_POINTER, tid_DWORD, tid_DWORD, tid_DWORD),
			PR_ARG4(new_obj, iid, pid, subtype));
	else
		return LOCAL_SYS->ObjectCreate(object, new_obj, iid, pid, subtype);
}

tERROR PRCreateObjectQuick(tPROCESS process, hOBJECT object, hOBJECT* new_obj, tIID iid, tPID pid, tDWORD subtype)
{
	hOBJECT _this = g_mng.GetObjectStub(process, object);

	if( _this )
		return PR_INVOKE_BASE(ProxyObjectCreateQuick, 0, _this, PR_ARG4(tid_OBJECT|tid_POINTER, tid_DWORD, tid_DWORD, tid_DWORD),
			PR_ARG4(new_obj, iid, pid, subtype));
	else
		return LOCAL_SYS->ObjectCreateQuick(object, new_obj, iid, pid, subtype);
}

tERROR PRRegiterRemotePlugin(tPID pid)
{
	hRecord l_record = g_mng.InsertRecord(&pid, g_mng.m_ProccessId);
	return l_record == SHTR_INVALID_HANDLE ? errOBJECT_CANNOT_BE_INITIALIZED : errOK;
}

//************************************************************************
// Remote prague

tERROR PRGetRootProxy(tPROCESS process, hROOT *root)
{
	*root = NULL;

	if( process == PR_PROCESS_ANY )
		return errPARAMETER_INVALID;

	if( process == PR_PROCESS_LOCAL || process == g_mng.m_ProccessId )
	{
		*root = g_root;
		return errOK;
	}

	PRConnection *l_connect = g_mng.FindConnection(g_mng.GetHost(process), root);
	return l_connect ? errOK : errNOT_OK;
}

tERROR PRGetObjectProxy(tPROCESS process, tSTRING name, hOBJECT *object)
{
	if( !name )
		return errOBJECT_NOT_FOUND;

	hOBJECT l_object = g_mng.GetObjectProxy(process, name);
	if( !l_object )
		return errOBJECT_NOT_FOUND;

	*object = l_object;
	return errOK;
}

tERROR PRGetObjectProcess(hOBJECT object, tPROCESS *process)
{
	if( process )
		*process = g_mng.GetObjectProcess(object);
	return errOK;
}

tERROR PRReleaseObjectProxy(hOBJECT object)
{
	PRAutoLocker locker(g_mng.m_proxy_lock);
	if( !IS_VALID_REMOTE(object) )
		return errOBJECT_INVALID;

	CUST_OBJ(object)->m_connect->ReleaseProxy(object, cFALSE);
	return errOK;
}

tERROR PRAddRefObjectProxy(hOBJECT object)
{
	PRAutoLocker locker(g_mng.m_proxy_lock);
	if( !IS_VALID_REMOTE(object) )
		return errOBJECT_INVALID;

	return CUST_OBJ(object)->m_connect->AddRefProxy(object);
}

tERROR PRSetRunDownCallback(hOBJECT object, void (*callback)(hOBJECT object, void *context), void *context)
{
	if( !IS_OBJ_PROXY(object) )
		return errOK;

	PRRemoteProxy *l_proxy = CUST_PROXY(object);
	l_proxy->m_rd_ctx = context;
	l_proxy->m_rd_clb = callback;

	return errOK;
}

tERROR PRIsValidProxy(hOBJECT object)
{
	if( !object )
		return errNOT_OK;

	if( !IS_OBJ_PROXY(object) )
		return errOK;

	return CUST_PROXY(object)->m_object != 0 ? errOK : errNOT_OK;
}

//************************************************************************
// Create process api

tERROR PRWaitProcess(tPROCESS process_id, tDWORD timeout)
{
	if( !timeout || timeout == -1 )
		return errOK;

	PREvent event;
	event.init(process_id);
	return event.wait(timeout);
}

tERROR PRCreateProcess(tSESSION session_id, tSTRING exec_path, tSTRING exec_args, tDWORD timeout, tBOOL child, tPROCESS *process)
{
#if defined (_WIN32)
	tPROCESS process_id;
	if( !g_WTSApi.CreateProcess(session_id, exec_path, exec_args, &process_id, timeout == -1) )
		return errOBJECT_NOT_CREATED;

	if( process )
		*process = process_id;

	tERROR error = PRWaitProcess(process_id, timeout);
	if( PR_SUCC(error) && child )
	{
		PRConnection *l_connect = g_mng.FindConnection(g_mng.GetHost(process_id));
		if( l_connect )
			PR_INVOKE_BASE(AttachAsChild, 0, l_connect->m_this, PR_ARG0(), PR_ARG0());
	}
	return error;
#elif defined (__unix__)
#warning "Consideration is needed"
	return errOK;
#endif

}

tERROR PRCreateObjectProxy(tSESSION session_id, tSTRING exec_path, tSTRING exec_args, tDWORD timeout, tSTRING object_name, hOBJECT *proxy)
{
	if( PR_SUCC(PRGetObjectProxy(PR_PROCESS_ANY, object_name, (hOBJECT*)proxy)) )
		return errOK;
#if defined (_WIN32)
	tPROCESS process_id;
	if( !g_WTSApi.CreateProcess(session_id, exec_path, exec_args, &process_id) )
		return errOBJECT_NOT_CREATED;

	tERROR error = PRWaitProcess(process_id, timeout);
	if( PR_SUCC(error) )
		error = PRGetObjectProxy(PR_PROCESS_ANY, object_name, (hOBJECT*)proxy);
	return error;
#elif defined (__unix__)
#warning "Consideration is needed"
	return errOBJECT_NOT_FOUND;
#endif


}

tERROR PRCloseProcessRequest(tPROCESS process)
{
	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "rmt\tCloseProcessRequest for server %d", process));

	if( process == PR_PROCESS_ALL_IMPERSONATED )
	{
		PRAutoLocker locker(g_mng.m_process_lock);
		for(size_t i = 0; i < g_mng.m_processes.size(); i++)
			PRCloseProcessRequest(g_mng.m_processes[i].m_ProcessId);

		g_mng.m_processes.clear();
		return errOK;
	}

	if( process == PR_PROCESS_LOCAL )
		process = g_mng.m_ProccessId;

	long aProcesses[0x100], nCount;
	if( process == PR_PROCESS_ALL )
	{
		PRShareObjectKey l_key = {PR_REMOTE_ROOT, 0};
		g_mng.EnumRecords(&l_key, aProcesses, &nCount);
	}
	else
	{
		nCount = 1;
		aProcesses[0] = process;
	}

	for(long i = 0; i < nCount; i++)
	{
		if( aProcesses[i] == g_mng.m_ProccessId )
			continue;

		PRConnection *l_connect = g_mng.FindConnection(g_mng.GetHost(aProcesses[i]));
		if( l_connect )
			PR_INVOKE_BASE(CloseRequest, 0, l_connect->m_this, PR_ARG0(), PR_ARG0());
	}
	return errOK;
}

tERROR PREstablishConnections(tSTRING name)
{
	long aProcesses[0x100], nCount;

	PRShareObjectKey l_key = {name, 0};
	g_mng.EnumRecords(&l_key, aProcesses, &nCount);

	for(long i = 0; i < nCount; i++)
		if( aProcesses[i] != g_mng.m_ProccessId )
			g_mng.FindConnection(g_mng.GetHost(aProcesses[i]));

	return errOK;
}

tERROR PRIsSystemProcess(tPROCESS process)
{
#if defined (_WIN32)
	if( process != PR_PROCESS_LOCAL )
		return errUNEXPECTED;

	HANDLE hToken = g_WTSApi.GetSystemToken();
	if( !hToken )
		return errNOT_OK;

	CloseHandle(hToken);
#else
#warning "Consideration is needed"
#endif
	return errOK;
}

//************************************************************************

PR_API tERROR PRRegisterCreationInfo(tPID pid, tDWORD flags, tPROCESS process, tSTRING exe_name)
{
	PRCreationPIDInfo pInfo;
	pInfo.m_Pid = pid;
	pInfo.m_Flags = flags;
	pInfo.m_ProcessId = process;
	if( exe_name )
		pInfo.m_ExeName = exe_name;
	g_mng.m_pids.push_back(pInfo);
	return errOK;
}

PR_API tERROR PRUregisterCreationInfo(tPID pid)
{
	size_t pos;
	for(pos = 0; pos < g_mng.m_pids.size(); pos++)
		if( g_mng.m_pids[pos].m_Pid == pid )
			break;

	if( pos == g_mng.m_pids.size() )
		return errNOT_FOUND;

	if( g_mng.m_pids[pos].m_Flags & PR_CREATE_IN_IMPERSONATED_PROCESS )
		PRCloseProcessRequest(PR_PROCESS_ALL_IMPERSONATED);

	g_mng.m_pids.erase(g_mng.m_pids.begin()+pos);
	return errOK;
}

//************************************************************************

PR_API tERROR PRImpersonateAsAdmin(hOBJECT proxy)
{
#if defined (_WIN32)
	HANDLE hThread = NULL;
	DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &hThread, THREAD_ALL_ACCESS, FALSE, 0);
	if( !hThread )
		return errUNEXPECTED;

	return PR_INVOKE_BASE(ImpersonateAsAdmin, 0, proxy, PR_ARG2(tid_DWORD,tid_QWORD), PR_ARG2(g_mng.m_ProccessId, hThread));
#endif //_WIN32
}

//************************************************************************

tERROR PRGetAPI(PRRemoteAPI *api)
{
	api->Initialize			= PRInitialize;
	api->Deinitialize		= PRDeinitialize;
	api->GetRoot			= PRGetRoot;
	api->StopServer			= PRStopServer;
	api->RegisterServer		= PRRegisterServer;
	api->RegisterHost		= PRRegisterHost;
	api->RegisterObject		= PRRegisterObject;
	api->UnregisterObject	= PRUnregisterObject;
	api->CreateProcess		= PRCreateProcess;
	api->CreateObjectProxy	= PRCreateObjectProxy;
	api->CloseProcessRequest= PRCloseProcessRequest;
	api->CreateObject		= PRCreateObject;
	api->CreateObjectQuick	= PRCreateObjectQuick;
	api->GetRootProxy		= PRGetRootProxy;
	api->GetObjectProxy		= PRGetObjectProxy;
	api->GetObjectProcess	= PRGetObjectProcess;
	api->ReleaseObjectProxy	= PRReleaseObjectProxy;
	api->AddRefObjectProxy	= PRAddRefObjectProxy;
	api->SetRunDownCallback	= PRSetRunDownCallback;
	api->IsValidProxy		= PRIsValidProxy;
	api->EstablishConnections = PREstablishConnections;
	api->IsSystemProcess    = PRIsSystemProcess;
	api->RegisterCreationInfo  = PRRegisterCreationInfo;
	api->UregisterCreationInfo = PRUregisterCreationInfo;
	api->ImpersonateAsAdmin = PRImpersonateAsAdmin;
	return errOK;
}

//************************************************************************

PRRemoteManager * g_manager = &g_mng;
#if defined (_WIN32)
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       tERROR *pERROR
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			g_module = hModule;
			PRGetAPI(&g_API);
			break;
		case DLL_PROCESS_DETACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case PRAGUE_PLUGIN_LOAD :
			*pERROR = cTRUE;
			break;
		case PRAGUE_PLUGIN_UNLOAD :
			break;
    }
    return TRUE;
}

#elif defined (__unix__)
void __attribute__ ((constructor)) DllMain() 
{
	PRGetAPI(&g_API);
}
#endif
//************************************************************************
