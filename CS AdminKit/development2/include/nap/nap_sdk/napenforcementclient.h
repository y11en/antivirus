
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Tue Oct 30 15:55:03 2007
 */
/* Compiler settings for O:\CS AdminKit\development2\nap\nap_sdk\include\napenforcementclient.idl:
    Os, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __napenforcementclient_h__
#define __napenforcementclient_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __INapEnforcementClientBinding_FWD_DEFINED__
#define __INapEnforcementClientBinding_FWD_DEFINED__
typedef interface INapEnforcementClientBinding INapEnforcementClientBinding;
#endif 	/* __INapEnforcementClientBinding_FWD_DEFINED__ */


#ifndef __INapEnforcementClientCallback_FWD_DEFINED__
#define __INapEnforcementClientCallback_FWD_DEFINED__
typedef interface INapEnforcementClientCallback INapEnforcementClientCallback;
#endif 	/* __INapEnforcementClientCallback_FWD_DEFINED__ */


#ifndef __INapEnforcementClientConnection_FWD_DEFINED__
#define __INapEnforcementClientConnection_FWD_DEFINED__
typedef interface INapEnforcementClientConnection INapEnforcementClientConnection;
#endif 	/* __INapEnforcementClientConnection_FWD_DEFINED__ */


/* header files for imported files */
#include "NapTypes.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_napenforcementclient_0000 */
/* [local] */ 





extern RPC_IF_HANDLE __MIDL_itf_napenforcementclient_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_napenforcementclient_0000_v0_0_s_ifspec;

#ifndef __INapEnforcementClientBinding_INTERFACE_DEFINED__
#define __INapEnforcementClientBinding_INTERFACE_DEFINED__

/* interface INapEnforcementClientBinding */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_INapEnforcementClientBinding;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("92B93223-7487-42d9-9A91-5B8507720384")
    INapEnforcementClientBinding : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ EnforcementEntityId id,
            /* [in] */ INapEnforcementClientCallback *callback) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Uninitialize( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateConnection( 
            /* [out] */ INapEnforcementClientConnection **connection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSoHRequest( 
            /* [in] */ INapEnforcementClientConnection *connection,
            /* [out] */ BOOL *retriggerHint) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ProcessSoHResponse( 
            /* [in] */ INapEnforcementClientConnection *connection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NotifyConnectionStateDown( 
            /* [in] */ INapEnforcementClientConnection *downCxn) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NotifySoHChangeFailure( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INapEnforcementClientBindingVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INapEnforcementClientBinding * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INapEnforcementClientBinding * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INapEnforcementClientBinding * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            INapEnforcementClientBinding * This,
            /* [in] */ EnforcementEntityId id,
            /* [in] */ INapEnforcementClientCallback *callback);
        
        HRESULT ( STDMETHODCALLTYPE *Uninitialize )( 
            INapEnforcementClientBinding * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateConnection )( 
            INapEnforcementClientBinding * This,
            /* [out] */ INapEnforcementClientConnection **connection);
        
        HRESULT ( STDMETHODCALLTYPE *GetSoHRequest )( 
            INapEnforcementClientBinding * This,
            /* [in] */ INapEnforcementClientConnection *connection,
            /* [out] */ BOOL *retriggerHint);
        
        HRESULT ( STDMETHODCALLTYPE *ProcessSoHResponse )( 
            INapEnforcementClientBinding * This,
            /* [in] */ INapEnforcementClientConnection *connection);
        
        HRESULT ( STDMETHODCALLTYPE *NotifyConnectionStateDown )( 
            INapEnforcementClientBinding * This,
            /* [in] */ INapEnforcementClientConnection *downCxn);
        
        HRESULT ( STDMETHODCALLTYPE *NotifySoHChangeFailure )( 
            INapEnforcementClientBinding * This);
        
        END_INTERFACE
    } INapEnforcementClientBindingVtbl;

    interface INapEnforcementClientBinding
    {
        CONST_VTBL struct INapEnforcementClientBindingVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INapEnforcementClientBinding_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INapEnforcementClientBinding_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INapEnforcementClientBinding_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INapEnforcementClientBinding_Initialize(This,id,callback)	\
    (This)->lpVtbl -> Initialize(This,id,callback)

#define INapEnforcementClientBinding_Uninitialize(This)	\
    (This)->lpVtbl -> Uninitialize(This)

#define INapEnforcementClientBinding_CreateConnection(This,connection)	\
    (This)->lpVtbl -> CreateConnection(This,connection)

#define INapEnforcementClientBinding_GetSoHRequest(This,connection,retriggerHint)	\
    (This)->lpVtbl -> GetSoHRequest(This,connection,retriggerHint)

#define INapEnforcementClientBinding_ProcessSoHResponse(This,connection)	\
    (This)->lpVtbl -> ProcessSoHResponse(This,connection)

#define INapEnforcementClientBinding_NotifyConnectionStateDown(This,downCxn)	\
    (This)->lpVtbl -> NotifyConnectionStateDown(This,downCxn)

#define INapEnforcementClientBinding_NotifySoHChangeFailure(This)	\
    (This)->lpVtbl -> NotifySoHChangeFailure(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INapEnforcementClientBinding_Initialize_Proxy( 
    INapEnforcementClientBinding * This,
    /* [in] */ EnforcementEntityId id,
    /* [in] */ INapEnforcementClientCallback *callback);


void __RPC_STUB INapEnforcementClientBinding_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientBinding_Uninitialize_Proxy( 
    INapEnforcementClientBinding * This);


void __RPC_STUB INapEnforcementClientBinding_Uninitialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientBinding_CreateConnection_Proxy( 
    INapEnforcementClientBinding * This,
    /* [out] */ INapEnforcementClientConnection **connection);


void __RPC_STUB INapEnforcementClientBinding_CreateConnection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientBinding_GetSoHRequest_Proxy( 
    INapEnforcementClientBinding * This,
    /* [in] */ INapEnforcementClientConnection *connection,
    /* [out] */ BOOL *retriggerHint);


void __RPC_STUB INapEnforcementClientBinding_GetSoHRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientBinding_ProcessSoHResponse_Proxy( 
    INapEnforcementClientBinding * This,
    /* [in] */ INapEnforcementClientConnection *connection);


void __RPC_STUB INapEnforcementClientBinding_ProcessSoHResponse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientBinding_NotifyConnectionStateDown_Proxy( 
    INapEnforcementClientBinding * This,
    /* [in] */ INapEnforcementClientConnection *downCxn);


void __RPC_STUB INapEnforcementClientBinding_NotifyConnectionStateDown_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientBinding_NotifySoHChangeFailure_Proxy( 
    INapEnforcementClientBinding * This);


void __RPC_STUB INapEnforcementClientBinding_NotifySoHChangeFailure_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INapEnforcementClientBinding_INTERFACE_DEFINED__ */


#ifndef __INapEnforcementClientCallback_INTERFACE_DEFINED__
#define __INapEnforcementClientCallback_INTERFACE_DEFINED__

/* interface INapEnforcementClientCallback */
/* [unique][uuid][object] */ 

typedef struct tagConnections
    {
    /* [range] */ UINT16 count;
    /* [size_is] */ INapEnforcementClientConnection **connections;
    } 	Connections;


EXTERN_C const IID IID_INapEnforcementClientCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F5A0B90A-83A1-4f76-BA3F-025418682814")
    INapEnforcementClientCallback : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE NotifySoHChange( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetConnections( 
            /* [out] */ Connections **connections) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INapEnforcementClientCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INapEnforcementClientCallback * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INapEnforcementClientCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INapEnforcementClientCallback * This);
        
        HRESULT ( STDMETHODCALLTYPE *NotifySoHChange )( 
            INapEnforcementClientCallback * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetConnections )( 
            INapEnforcementClientCallback * This,
            /* [out] */ Connections **connections);
        
        END_INTERFACE
    } INapEnforcementClientCallbackVtbl;

    interface INapEnforcementClientCallback
    {
        CONST_VTBL struct INapEnforcementClientCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INapEnforcementClientCallback_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INapEnforcementClientCallback_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INapEnforcementClientCallback_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INapEnforcementClientCallback_NotifySoHChange(This)	\
    (This)->lpVtbl -> NotifySoHChange(This)

#define INapEnforcementClientCallback_GetConnections(This,connections)	\
    (This)->lpVtbl -> GetConnections(This,connections)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INapEnforcementClientCallback_NotifySoHChange_Proxy( 
    INapEnforcementClientCallback * This);


void __RPC_STUB INapEnforcementClientCallback_NotifySoHChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientCallback_GetConnections_Proxy( 
    INapEnforcementClientCallback * This,
    /* [out] */ Connections **connections);


void __RPC_STUB INapEnforcementClientCallback_GetConnections_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INapEnforcementClientCallback_INTERFACE_DEFINED__ */


#ifndef __INapEnforcementClientConnection_INTERFACE_DEFINED__
#define __INapEnforcementClientConnection_INTERFACE_DEFINED__

/* interface INapEnforcementClientConnection */
/* [unique][uuid][object] */ 

#define	defaultProtocolMaxSize	( 4096 )

#define	minProtocolMaxSize	( 300 )

#define	maxProtocolMaxSize	( 0xffff )

typedef /* [range] */ UINT32 ProtocolMaxSize;


EXTERN_C const IID IID_INapEnforcementClientConnection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FB3A3505-DDB1-468a-B307-F328A57419D8")
    INapEnforcementClientConnection : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ EnforcementEntityId id) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMaxSize( 
            /* [in] */ ProtocolMaxSize maxSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMaxSize( 
            /* [out] */ ProtocolMaxSize *maxSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFlags( 
            /* [in] */ UINT8 flags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFlags( 
            /* [out] */ UINT8 *flags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetConnectionId( 
            /* [in] */ const ConnectionId *connectionId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetConnectionId( 
            /* [out] */ ConnectionId **connectionId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCorrelationId( 
            /* [out] */ CorrelationId *correlationId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetStringCorrelationId( 
            /* [out] */ StringCorrelationId **correlationId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCorrelationId( 
            /* [in] */ CorrelationId correlationId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSoHRequest( 
            /* [unique][in] */ const NetworkSoHRequest *sohRequest) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSoHRequest( 
            /* [out] */ NetworkSoHRequest **sohRequest) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSoHResponse( 
            /* [in] */ const NetworkSoHResponse *sohResponse) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSoHResponse( 
            /* [out] */ NetworkSoHResponse **sohResponse) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetIsolationInfo( 
            /* [in] */ const IsolationInfo *isolationInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIsolationInfo( 
            /* [out] */ IsolationInfo **isolationInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPrivateData( 
            /* [in] */ const PrivateData *privateData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPrivateData( 
            /* [out] */ PrivateData **privateData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetEnforcerPrivateData( 
            /* [in] */ const PrivateData *privateData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEnforcerPrivateData( 
            /* [out] */ PrivateData **privateData) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INapEnforcementClientConnectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INapEnforcementClientConnection * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INapEnforcementClientConnection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INapEnforcementClientConnection * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            INapEnforcementClientConnection * This,
            /* [in] */ EnforcementEntityId id);
        
        HRESULT ( STDMETHODCALLTYPE *SetMaxSize )( 
            INapEnforcementClientConnection * This,
            /* [in] */ ProtocolMaxSize maxSize);
        
        HRESULT ( STDMETHODCALLTYPE *GetMaxSize )( 
            INapEnforcementClientConnection * This,
            /* [out] */ ProtocolMaxSize *maxSize);
        
        HRESULT ( STDMETHODCALLTYPE *SetFlags )( 
            INapEnforcementClientConnection * This,
            /* [in] */ UINT8 flags);
        
        HRESULT ( STDMETHODCALLTYPE *GetFlags )( 
            INapEnforcementClientConnection * This,
            /* [out] */ UINT8 *flags);
        
        HRESULT ( STDMETHODCALLTYPE *SetConnectionId )( 
            INapEnforcementClientConnection * This,
            /* [in] */ const ConnectionId *connectionId);
        
        HRESULT ( STDMETHODCALLTYPE *GetConnectionId )( 
            INapEnforcementClientConnection * This,
            /* [out] */ ConnectionId **connectionId);
        
        HRESULT ( STDMETHODCALLTYPE *GetCorrelationId )( 
            INapEnforcementClientConnection * This,
            /* [out] */ CorrelationId *correlationId);
        
        HRESULT ( STDMETHODCALLTYPE *GetStringCorrelationId )( 
            INapEnforcementClientConnection * This,
            /* [out] */ StringCorrelationId **correlationId);
        
        HRESULT ( STDMETHODCALLTYPE *SetCorrelationId )( 
            INapEnforcementClientConnection * This,
            /* [in] */ CorrelationId correlationId);
        
        HRESULT ( STDMETHODCALLTYPE *SetSoHRequest )( 
            INapEnforcementClientConnection * This,
            /* [unique][in] */ const NetworkSoHRequest *sohRequest);
        
        HRESULT ( STDMETHODCALLTYPE *GetSoHRequest )( 
            INapEnforcementClientConnection * This,
            /* [out] */ NetworkSoHRequest **sohRequest);
        
        HRESULT ( STDMETHODCALLTYPE *SetSoHResponse )( 
            INapEnforcementClientConnection * This,
            /* [in] */ const NetworkSoHResponse *sohResponse);
        
        HRESULT ( STDMETHODCALLTYPE *GetSoHResponse )( 
            INapEnforcementClientConnection * This,
            /* [out] */ NetworkSoHResponse **sohResponse);
        
        HRESULT ( STDMETHODCALLTYPE *SetIsolationInfo )( 
            INapEnforcementClientConnection * This,
            /* [in] */ const IsolationInfo *isolationInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIsolationInfo )( 
            INapEnforcementClientConnection * This,
            /* [out] */ IsolationInfo **isolationInfo);
        
        HRESULT ( STDMETHODCALLTYPE *SetPrivateData )( 
            INapEnforcementClientConnection * This,
            /* [in] */ const PrivateData *privateData);
        
        HRESULT ( STDMETHODCALLTYPE *GetPrivateData )( 
            INapEnforcementClientConnection * This,
            /* [out] */ PrivateData **privateData);
        
        HRESULT ( STDMETHODCALLTYPE *SetEnforcerPrivateData )( 
            INapEnforcementClientConnection * This,
            /* [in] */ const PrivateData *privateData);
        
        HRESULT ( STDMETHODCALLTYPE *GetEnforcerPrivateData )( 
            INapEnforcementClientConnection * This,
            /* [out] */ PrivateData **privateData);
        
        END_INTERFACE
    } INapEnforcementClientConnectionVtbl;

    interface INapEnforcementClientConnection
    {
        CONST_VTBL struct INapEnforcementClientConnectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INapEnforcementClientConnection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INapEnforcementClientConnection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INapEnforcementClientConnection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INapEnforcementClientConnection_Initialize(This,id)	\
    (This)->lpVtbl -> Initialize(This,id)

#define INapEnforcementClientConnection_SetMaxSize(This,maxSize)	\
    (This)->lpVtbl -> SetMaxSize(This,maxSize)

#define INapEnforcementClientConnection_GetMaxSize(This,maxSize)	\
    (This)->lpVtbl -> GetMaxSize(This,maxSize)

#define INapEnforcementClientConnection_SetFlags(This,flags)	\
    (This)->lpVtbl -> SetFlags(This,flags)

#define INapEnforcementClientConnection_GetFlags(This,flags)	\
    (This)->lpVtbl -> GetFlags(This,flags)

#define INapEnforcementClientConnection_SetConnectionId(This,connectionId)	\
    (This)->lpVtbl -> SetConnectionId(This,connectionId)

#define INapEnforcementClientConnection_GetConnectionId(This,connectionId)	\
    (This)->lpVtbl -> GetConnectionId(This,connectionId)

#define INapEnforcementClientConnection_GetCorrelationId(This,correlationId)	\
    (This)->lpVtbl -> GetCorrelationId(This,correlationId)

#define INapEnforcementClientConnection_GetStringCorrelationId(This,correlationId)	\
    (This)->lpVtbl -> GetStringCorrelationId(This,correlationId)

#define INapEnforcementClientConnection_SetCorrelationId(This,correlationId)	\
    (This)->lpVtbl -> SetCorrelationId(This,correlationId)

#define INapEnforcementClientConnection_SetSoHRequest(This,sohRequest)	\
    (This)->lpVtbl -> SetSoHRequest(This,sohRequest)

#define INapEnforcementClientConnection_GetSoHRequest(This,sohRequest)	\
    (This)->lpVtbl -> GetSoHRequest(This,sohRequest)

#define INapEnforcementClientConnection_SetSoHResponse(This,sohResponse)	\
    (This)->lpVtbl -> SetSoHResponse(This,sohResponse)

#define INapEnforcementClientConnection_GetSoHResponse(This,sohResponse)	\
    (This)->lpVtbl -> GetSoHResponse(This,sohResponse)

#define INapEnforcementClientConnection_SetIsolationInfo(This,isolationInfo)	\
    (This)->lpVtbl -> SetIsolationInfo(This,isolationInfo)

#define INapEnforcementClientConnection_GetIsolationInfo(This,isolationInfo)	\
    (This)->lpVtbl -> GetIsolationInfo(This,isolationInfo)

#define INapEnforcementClientConnection_SetPrivateData(This,privateData)	\
    (This)->lpVtbl -> SetPrivateData(This,privateData)

#define INapEnforcementClientConnection_GetPrivateData(This,privateData)	\
    (This)->lpVtbl -> GetPrivateData(This,privateData)

#define INapEnforcementClientConnection_SetEnforcerPrivateData(This,privateData)	\
    (This)->lpVtbl -> SetEnforcerPrivateData(This,privateData)

#define INapEnforcementClientConnection_GetEnforcerPrivateData(This,privateData)	\
    (This)->lpVtbl -> GetEnforcerPrivateData(This,privateData)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_Initialize_Proxy( 
    INapEnforcementClientConnection * This,
    /* [in] */ EnforcementEntityId id);


void __RPC_STUB INapEnforcementClientConnection_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_SetMaxSize_Proxy( 
    INapEnforcementClientConnection * This,
    /* [in] */ ProtocolMaxSize maxSize);


void __RPC_STUB INapEnforcementClientConnection_SetMaxSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_GetMaxSize_Proxy( 
    INapEnforcementClientConnection * This,
    /* [out] */ ProtocolMaxSize *maxSize);


void __RPC_STUB INapEnforcementClientConnection_GetMaxSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_SetFlags_Proxy( 
    INapEnforcementClientConnection * This,
    /* [in] */ UINT8 flags);


void __RPC_STUB INapEnforcementClientConnection_SetFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_GetFlags_Proxy( 
    INapEnforcementClientConnection * This,
    /* [out] */ UINT8 *flags);


void __RPC_STUB INapEnforcementClientConnection_GetFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_SetConnectionId_Proxy( 
    INapEnforcementClientConnection * This,
    /* [in] */ const ConnectionId *connectionId);


void __RPC_STUB INapEnforcementClientConnection_SetConnectionId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_GetConnectionId_Proxy( 
    INapEnforcementClientConnection * This,
    /* [out] */ ConnectionId **connectionId);


void __RPC_STUB INapEnforcementClientConnection_GetConnectionId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_GetCorrelationId_Proxy( 
    INapEnforcementClientConnection * This,
    /* [out] */ CorrelationId *correlationId);


void __RPC_STUB INapEnforcementClientConnection_GetCorrelationId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_GetStringCorrelationId_Proxy( 
    INapEnforcementClientConnection * This,
    /* [out] */ StringCorrelationId **correlationId);


void __RPC_STUB INapEnforcementClientConnection_GetStringCorrelationId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_SetCorrelationId_Proxy( 
    INapEnforcementClientConnection * This,
    /* [in] */ CorrelationId correlationId);


void __RPC_STUB INapEnforcementClientConnection_SetCorrelationId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_SetSoHRequest_Proxy( 
    INapEnforcementClientConnection * This,
    /* [unique][in] */ const NetworkSoHRequest *sohRequest);


void __RPC_STUB INapEnforcementClientConnection_SetSoHRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_GetSoHRequest_Proxy( 
    INapEnforcementClientConnection * This,
    /* [out] */ NetworkSoHRequest **sohRequest);


void __RPC_STUB INapEnforcementClientConnection_GetSoHRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_SetSoHResponse_Proxy( 
    INapEnforcementClientConnection * This,
    /* [in] */ const NetworkSoHResponse *sohResponse);


void __RPC_STUB INapEnforcementClientConnection_SetSoHResponse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_GetSoHResponse_Proxy( 
    INapEnforcementClientConnection * This,
    /* [out] */ NetworkSoHResponse **sohResponse);


void __RPC_STUB INapEnforcementClientConnection_GetSoHResponse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_SetIsolationInfo_Proxy( 
    INapEnforcementClientConnection * This,
    /* [in] */ const IsolationInfo *isolationInfo);


void __RPC_STUB INapEnforcementClientConnection_SetIsolationInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_GetIsolationInfo_Proxy( 
    INapEnforcementClientConnection * This,
    /* [out] */ IsolationInfo **isolationInfo);


void __RPC_STUB INapEnforcementClientConnection_GetIsolationInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_SetPrivateData_Proxy( 
    INapEnforcementClientConnection * This,
    /* [in] */ const PrivateData *privateData);


void __RPC_STUB INapEnforcementClientConnection_SetPrivateData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_GetPrivateData_Proxy( 
    INapEnforcementClientConnection * This,
    /* [out] */ PrivateData **privateData);


void __RPC_STUB INapEnforcementClientConnection_GetPrivateData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_SetEnforcerPrivateData_Proxy( 
    INapEnforcementClientConnection * This,
    /* [in] */ const PrivateData *privateData);


void __RPC_STUB INapEnforcementClientConnection_SetEnforcerPrivateData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_GetEnforcerPrivateData_Proxy( 
    INapEnforcementClientConnection * This,
    /* [out] */ PrivateData **privateData);


void __RPC_STUB INapEnforcementClientConnection_GetEnforcerPrivateData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INapEnforcementClientConnection_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_napenforcementclient_0257 */
/* [local] */ 

// Declarations of CLSIDs of objects provided  
// by the system. Link to uuid.lib to get them 
// defined.                                    
EXTERN_C const CLSID 
   CLSID_NapEnforcementClientBinding;


extern RPC_IF_HANDLE __MIDL_itf_napenforcementclient_0257_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_napenforcementclient_0257_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


