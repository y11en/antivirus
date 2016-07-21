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

#ifndef __pr_common_h
#define __pr_common_h

#include <Prague/pr_remote.h>
#include <Prague/pr_vtbl.h>
#include <Prague/pr_wrappers.h>
#include <Prague/iface/i_root.h>

#include "pr_manager.h"

#include "SharedTable.h"
#if defined (_WIN32)
#pragma warning(disable : 4786)
#else
#include <rpc.h>
#endif

#include <set>
#include <string>
#include <vector>

using namespace std;

#define prf_REMOTEAPI		0x00000002
#define prf_SYTEMAPI        0x00000004
#define prf_STUBCALL        0x00000008
#define prf_NOVALIDATE      0x00000010

#define TID_TYPE(t)         ((t) & 0xFF)
#define SER_BUFFER          0x1000

#define	MAX_PROXY_LEVEL		0x10
#define	MAX_MHL_SIZE		0x20
#define MAX_PROP_NAME_SIZE	0x40
#define MAX_PROCESS_COUNT	0x10
#define MAX_BUFF_SIZE		0x200
#define MAX_NAME_SIZE		0x20

#define PR_REMOTE_ROOT      "PRRoot"

struct PRConnection;

//************************************************************************

struct tProxyCreateInfo
{
	rpcOBJECT	object;
	rpcOBJECT	stub;
	tDWORD		offset;
	tDWORD		size;
	tIID		iid;
	tIID		pid;
};

struct tProxyParam
{
	tDWORD      type;
	tDWORD      size;
	tDWORD      next;
	rpcOBJECT   data_ptr;
	tBYTE       data[1];
};

struct tProxyCall
{
	rpcOBJECT   obj;
	tDWORD    	flags;
	tDWORD    	func;
	tDWORD      numprm;
	tDWORD      dummy;
	tProxyParam params[1];
};

struct tProxyMsgHandlerDescr
{
	rpcOBJECT   object;
	tDWORD      flags;
	tDWORD      msg_cls;
	tIID        obj_iid;
	tPID        obj_pid;
	tIID        ctx_iid;
	tPID        ctx_pid;
};

//************************************************************************

struct PRRemoteObject
{
	PRConnection *	m_connect;
	rpcOBJECT	m_object;
	tINT		m_ref;
	tBYTE       m_is_full;
	tBYTE       m_is_pidref;
	tBYTE       m_is_create_ref;
	tBYTE       m_is_create_remote;
};

struct PRRemoteProxy : public PRRemoteObject
{
	rpcOBJECT	m_stub;
	tPID		m_pid;
	void	  (*m_rd_clb)(hOBJECT object, void *context);
	void *		m_rd_ctx;
};

struct PRRemoteHost
{
	PRRemoteHost(tDWORD end_point = 0, const tCHAR* host = NULL);

	bool operator < ( const PRRemoteHost &c ) const { return m_port != c.m_port ? m_port < c.m_port : !!strcmp(m_host, c.m_host); }
	bool operator == ( const PRRemoteHost &c ) const { return m_local ? m_local == c.m_local : m_port == c.m_port && !strcmp(m_host, c.m_host); }

	operator tPROCESS() const { return (tPROCESS)this; }
    const tCHAR* GetProtocol() const { return m_local ? "ncalrpc" : "ncacn_ip_tcp"; }
	const tCHAR* GetHostName() const { return m_local ? NULL : m_host; }
	string	     GetEndPoint() const;

	tPROCESS	m_local;
	tDWORD		m_port;
	char		m_host[MAX_NAME_SIZE];
};

struct PRConnection : public PRRemoteProxy, public PRRemoteHost
{
	void		Init(hOBJECT p_this, const PRRemoteHost &host);
	void		Close();
	bool        IsConnected();

	bool        Lock();
	void        Unlock();

	void        AddRef();
	void		Release(tBOOL is_close);

	tERROR		CreateProxy(hOBJECT parent, tBOOL is_ref, tProxyCreateInfo *info, rpcOBJECT *proxy);
	tERROR		FindProxy(hOBJECT object, tBOOL is_ref, tBOOL is_full, rpcOBJECT *proxy);
	rpcOBJECT	GetProxy(hOBJECT object, tBOOL is_ref = cFALSE, tBOOL is_full = cTRUE);
	tERROR      SetProxyData(hOBJECT object, rpcOBJECT proxy);

	void		ReleaseObject(hOBJECT object, tBOOL is_close);
	void        ReleaseProxy(hOBJECT object, tBOOL is_close);
	void        ReleaseStub(hOBJECT object, tBOOL is_close);
	tERROR      AddRefProxy(hOBJECT object);

	void		CloseProxy(rpcOBJECT object);
	void		CloseStub(rpcOBJECT object);

	void *		m_context;
	handle_t 	m_handle;
	hOBJECT		m_this;
	tBOOL       m_attached;
	tERROR      m_status;
	tDWORD      m_plocal;
	tLONG       m_lockref;
	tBOOL       m_rundown;
};

//************************************************************************

struct PRSharedProps : public CSharedTable
{
public:
	bool CompareKey(hRecord record, void *key, void *data);
	bool InitKeyData(hRecord record, void *key, void *data);
	void ClearProcess(hRecord record, void *data, long process);

	tERROR RegisterCustomPropId(hROOT _this, tDWORD* result, tSTRING name, tDWORD type);
	tERROR UnregisterCustomPropId(hROOT _this, tSTRING name);
	tERROR GetCustomPropId(hROOT _this, tDWORD* result, tSTRING name, tDWORD type);
	tERROR GetCustomPropName(hROOT _this, tDWORD* out_size, tPROPID prop_id, tPTR buffer, tDWORD size);
};

struct PRShareObjectKey
{
	char *		m_FindName;
	rpcOBJECT	m_hObject;
};

struct PRShareObject
{
	rpcOBJECT	m_hObject;
	char		m_idRegister[MAX_NAME_SIZE];
};

struct PRShareProp
{
	char		m_Name[MAX_PROP_NAME_SIZE];
	tDWORD		m_Count;
	tPROCESS	m_Process[MAX_PROCESS_COUNT];
};

struct PRCreationPIDInfo
{
	tPID        m_Pid;
	tDWORD      m_Flags;
	tPROCESS    m_ProcessId;
	string      m_ExeName;
};

struct PRRunningProcess
{
	tPROCESS     m_ProcessId;
	string       m_UserName;
	tDWORD       m_ref;
};

enum enFindConnectType
{
	fctFindOnly,
	fctConnect,
	fctBindClient,
};

#if defined (_WIN32)
struct PREvent
{
	PREvent() : m_evt(NULL){}
	~PREvent(){ done(); }

	operator bool() const{ return !m_evt; }

	void init(tPROCESS pid) {
		extern HANDLE CreateGlobalEvent(DWORD pid);

		if( !m_evt )
			m_evt = CreateGlobalEvent(pid);
	}

	void done()   { if( m_evt ) CloseHandle(m_evt); m_evt = NULL; }
	void signal() { if( m_evt ) SetEvent(m_evt); }
	void reset()  { if( m_evt ) ResetEvent(m_evt); }

	tERROR wait(tDWORD timeout)
	{
		DWORD result = m_evt ? WaitForSingleObject(m_evt, timeout) : WAIT_ABANDONED;
		switch(result)
		{
		case WAIT_OBJECT_0: return errOK;
		case WAIT_TIMEOUT: return errTIMEOUT;
		}
		return errUNEXPECTED;
	}

	HANDLE m_evt;
};

#elif defined (__unix__)

#define      InterlockedIncrement(val)             // todo
#define      InterlockedDecrement(val)             // todo
#define      Sleep(sec)                            usleep(sec*1000);
#define      IsBadWritePtr(ptr,len)                (0)
inline tBOOL GetComputerName(tCHAR* host, tULONG* size){ gethostname(host, *size); return cTRUE; }

struct PREvent
{
	operator bool() const{ return false; }

	void init(tPROCESS pid){}
	void done(){}
	void signal(){}
	void reset(){}
	tERROR wait(tDWORD timeout){ return errUNEXPECTED; }
};

#else
#error "Implementation is needed"
#endif

//************************************************************************

class PRRemoteManagerImpl :
	public PRRemoteManager,
	public PRRemoteHost,
	public CSharedTable
{
public:
	PRRemoteManagerImpl();
	~PRRemoteManagerImpl();

	bool IsActive(){ return m_is_init; }

	static void DbgTrace(tSTRING string);
	static tERROR LoadCallback(hROOT root, tVOID* params);

	tERROR Initialize(tDWORD flags, PRInitParams* params);
	tERROR Deinitialize();

	static void InitSystemIFace();
	static void InitRootIFace();

	tERROR InitLocalPrague(hROOT root);
	tERROR InitRemotePrague();
	tERROR StopServer();

// Cleanup methods
	void          RunDownClient(hOBJECT root, tBOOL f_shutdown);

// Registration methods
	tERROR        RegisterRootIFace();
	tERROR        RegisterClientIFace();
	tERROR        RegisterServerIFace();

	void          RegisterIFace(tIID iid, tPTR vtbl, tDWORD vtbl_size, iINTERNAL *internal_table, tDWORD data_size, tIID iid_compat);

	tERROR        RegisterServerHost(tDWORD port, const tCHAR* host = NULL);
	tERROR        RegisterRemoteHost(const tCHAR* host, tDWORD port, tPROCESS *process);

	PRRemoteHost  GetHost(tPROCESS process);
	tPROCESS      GetObjectProcess(hOBJECT object);

// Conections methods
	PRConnection* FindConnection(const PRRemoteHost &host, hROOT *root = NULL, enFindConnectType type = fctConnect, rpcOBJECT client_root = NULL);
	handle_t      GetConnection(hOBJECT object);

	rpcOBJECT     GetProxy(hOBJECT remote, hOBJECT object, tLONG ref_count=0);
	hOBJECT       ReleaseProxy(hOBJECT remote, rpcOBJECT object);

	hOBJECT       GetObjectStub(tPROCESS process, hOBJECT object);
	hOBJECT       GetObjectProxy(tPROCESS process, tSTRING name);

	tERROR        BindObjectOpen( PRConnection *connect, rpcOBJECT object, tSTRING name, rpcOBJECT* remote_object);

	tERROR        Invoke(hOBJECT _this, tDWORD flags, tDWORD func, tDWORD* types, va_list args);
	tERROR        InvokeEx(hOBJECT proxy, tDWORD flags, tDWORD func, ...);

// Validate methods
	bool          IsObjectValid(hOBJECT object);
	void          UpdateValidateList(hOBJECT object, bool bnew);

// Shared objects
	bool          CompareKey(hRecord record, void *key, void *data);
	bool          InitKeyData(hRecord record, void *key, void *data);

// Check methods
	bool          IsLocalCreation(hOBJECT& parent, tPID pid, bool bRecognize = false);
	bool          ReleaseProcess(tPROCESS pid);

public:
	bool		        m_is_init;
	PRInitParams*       m_params;
	hPLUGIN		        m_plugin;
	hIFACE_ENUM	        m_ienum;
	long		        m_init_ref;
	void*		        m_loader;

	PRTls		        m_rpc_tls;
	PRLocker            m_lock;
	PRLocker            m_stub_lock;
	PRLocker            m_proxy_lock;
	PRLocker            m_validate_lock;
	PREvent             m_event;

	PRLocker            m_process_lock;
	vector<PRCreationPIDInfo>  m_pids;
	vector<PRRunningProcess>   m_processes;

#if defined (__unix__)
	static pid_t        m_ProccessId;
#endif

	set<PRRemoteHost>	m_hosts;
	vector<hOBJECT>     m_objects;

	tDWORD              m_init_flags;
};

//************************************************************************

struct HRemoteObject
{
	const tPTR		vtbl;
	iSYSTEMVtbl *	sys;
	PRRemoteObject* data;
};

struct PRRemoteInterface
{
	tERROR (pr_call *BindClient)(hOBJECT , rpcOBJECT root, tDWORD process, tSTRING host, rpcOBJECT *client_root);
	tERROR (pr_call *BindObjectOpen)(hOBJECT root, rpcOBJECT object, tSTRING name, rpcOBJECT *proxy);
	tERROR (pr_call *BindObjectClose)(hOBJECT root, rpcOBJECT object);

	tERROR (pr_call *ProxyCreate)(hOBJECT root, rpcOBJECT parent, tBOOL is_ref, tProxyCreateInfo *info, tDWORD size, rpcOBJECT *proxy);
	tERROR (pr_call *ProxyCreateRemote)(hOBJECT _this, tPROCESS pid, rpcOBJECT* proxy);
	tERROR (pr_call *ProxySetData)(hOBJECT root, rpcOBJECT proxy, tCHAR *data, tDWORD data_size);
	tERROR (pr_call *ProxyAddRef)(hOBJECT root, rpcOBJECT proxy);

	tERROR (pr_call *CloseProxy)(hOBJECT root, rpcOBJECT proxy);
	tERROR (pr_call *CloseStub)(hOBJECT root, rpcOBJECT stub);

	tERROR (pr_call *ProxyObjectCreate)(hOBJECT _this, tVOID* new_obj, tIID iid, tPID pid, tDWORD subtype);
	tERROR (pr_call *ProxyObjectCreateQuick)(hOBJECT _this, tVOID* new_obj, tIID iid, tPID pid, tDWORD subtype);
	tERROR (pr_call *ProxyRegisterMsgHandlerList)(hOBJECT _this, const tProxyMsgHandlerDescr* handler_list, tDWORD handler_count);
	tERROR (pr_call *ProxyUnregisterMsgHandlerList)(hOBJECT _this, const tProxyMsgHandlerDescr* handler_list, tDWORD handler_count);
	tERROR (pr_call *ProxyMsgReceive)(hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT recieve_point, tBYTE* pbuff, tDWORD* blen);
	tERROR (pr_call *ProxyMsgReceiveSer)(hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT recieve_point, cSerializable* ser);

	tERROR (pr_call *AttachAsChild)(hOBJECT root);
	tERROR (pr_call *CloseRequest)(hOBJECT root);

	tERROR (pr_call *Deserialize)(hOBJECT root, rpcOBJECT ser, tBYTE* buffer, tDWORD size);

	tERROR (pr_call *ImpersonateAsAdmin)(hOBJECT root, tPROCESS pid, tQWORD hThread);
};

#define PR_INVOKE_BASE(func, _flags_, _this_, _types_, _args_) \
	g_manager->InvokeEx((hOBJECT)_this_, prf_REMOTEAPI|(_flags_), offsetof(PRRemoteInterface,func)/sizeof(tPTR), _types_, _args_)

#define PR_INVOKE_SYS(func, _flags_, _types_, _args_) \
	g_manager->InvokeEx((hOBJECT)_this, prf_SYTEMAPI|(_flags_), offsetof(iSYSTEMVtbl,func)/sizeof(tPTR), _types_, _args_)

//************************************************************************

extern PRRemoteInterface    remote_iface;
extern void*	            g_module;
extern hROOT				g_root;
extern PRRemoteManagerImpl	g_mng;
extern PRSharedProps 		g_shp;

extern iSYSTEMVTBL	        system_iface;
extern iSYSTEMVTBL	        system_iface_ex;
extern iINTERNAL	        internal_stub;
extern iINTERNAL	        internal_proxy;
extern iRootVtbl	        root_iface;

//************************************************************************

struct tag_hROOT;
#define LOCAL_ROOT			((iRootVtbl*)g_root->vtbl)
#define LOCAL_SYS			((iSYSTEMVtbl*)g_root->sys)

#define CUST_OBJ(obj)		(((HRemoteObject*)obj)->data)
#define CUST_ROOT(obj)		((PRConnection*)((HRemoteObject*)obj)->data)
#define CUST_PROXY(obj)		((PRRemoteProxy*)((HRemoteObject*)obj)->data)

#define IS_OBJ_PROXY(obj)	(obj->sys == (iSYSTEMVtbl*)&system_iface)
#define IS_OBJ_HAS_PROXY(obj) (obj->sys == (iSYSTEMVtbl*)&system_iface_ex)
#define IS_ROOT_PROXY(obj)	(obj->vtbl == &root_iface)
#define IS_VALID_REMOTE(obj)( g_mng.IsObjectValid((hOBJECT)obj) )

#define	ENUM_STUB_BEGIN(obj) \
	g_mng.m_stub_lock.lock(); hOBJECT _stub; \
	tERROR _error = CALL_SYS_ChildGetFirst(obj, &_stub, IID_REMOTE_STUB, PID_REMOTE_PRAGUE); \
	while( PR_SUCC(_error) && _stub ) { PRRemoteObject *l_stub = CUST_OBJ(_stub);

#define	ENUM_STUB_END() \
	_error = CALL_SYS_ObjectGetNext(_stub, &_stub, IID_REMOTE_STUB, PID_REMOTE_PRAGUE); } \
	g_mng.m_stub_lock.unlock();

//************************************************************************

#endif // __pr_common_h

//************************************************************************
