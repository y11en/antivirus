
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Fri Nov 09 19:00:56 2007
 */
/* Compiler settings for O:\CS AdminKit\development2\include\nap\nap_sdk\napsystemhealthagent.idl:
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

#ifndef __napsystemhealthagent_h__
#define __napsystemhealthagent_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __INapSystemHealthAgentBinding_FWD_DEFINED__
#define __INapSystemHealthAgentBinding_FWD_DEFINED__
typedef interface INapSystemHealthAgentBinding INapSystemHealthAgentBinding;
#endif 	/* __INapSystemHealthAgentBinding_FWD_DEFINED__ */


#ifndef __INapSystemHealthAgentCallback_FWD_DEFINED__
#define __INapSystemHealthAgentCallback_FWD_DEFINED__
typedef interface INapSystemHealthAgentCallback INapSystemHealthAgentCallback;
#endif 	/* __INapSystemHealthAgentCallback_FWD_DEFINED__ */


#ifndef __INapSystemHealthAgentRequest_FWD_DEFINED__
#define __INapSystemHealthAgentRequest_FWD_DEFINED__
typedef interface INapSystemHealthAgentRequest INapSystemHealthAgentRequest;
#endif 	/* __INapSystemHealthAgentRequest_FWD_DEFINED__ */


/* header files for imported files */
#include "NapTypes.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_napsystemhealthagent_0000 */
/* [local] */ 




extern RPC_IF_HANDLE __MIDL_itf_napsystemhealthagent_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_napsystemhealthagent_0000_v0_0_s_ifspec;

#ifndef __INapSystemHealthAgentBinding_INTERFACE_DEFINED__
#define __INapSystemHealthAgentBinding_INTERFACE_DEFINED__

/* interface INapSystemHealthAgentBinding */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_INapSystemHealthAgentBinding;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A6894F43-9CC7-44c9-A23F-19DBF36BAD28")
    INapSystemHealthAgentBinding : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ SystemHealthEntityId id,
            /* [in] */ INapSystemHealthAgentCallback *callback) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Uninitialize( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NotifySoHChange( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSystemIsolationInfo( 
            /* [out] */ IsolationInfo **isolationInfo,
            /* [out] */ BOOL *unknownConnections) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FlushCache( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INapSystemHealthAgentBindingVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INapSystemHealthAgentBinding * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INapSystemHealthAgentBinding * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INapSystemHealthAgentBinding * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            INapSystemHealthAgentBinding * This,
            /* [in] */ SystemHealthEntityId id,
            /* [in] */ INapSystemHealthAgentCallback *callback);
        
        HRESULT ( STDMETHODCALLTYPE *Uninitialize )( 
            INapSystemHealthAgentBinding * This);
        
        HRESULT ( STDMETHODCALLTYPE *NotifySoHChange )( 
            INapSystemHealthAgentBinding * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSystemIsolationInfo )( 
            INapSystemHealthAgentBinding * This,
            /* [out] */ IsolationInfo **isolationInfo,
            /* [out] */ BOOL *unknownConnections);
        
        HRESULT ( STDMETHODCALLTYPE *FlushCache )( 
            INapSystemHealthAgentBinding * This);
        
        END_INTERFACE
    } INapSystemHealthAgentBindingVtbl;

    interface INapSystemHealthAgentBinding
    {
        CONST_VTBL struct INapSystemHealthAgentBindingVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INapSystemHealthAgentBinding_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INapSystemHealthAgentBinding_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INapSystemHealthAgentBinding_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INapSystemHealthAgentBinding_Initialize(This,id,callback)	\
    (This)->lpVtbl -> Initialize(This,id,callback)

#define INapSystemHealthAgentBinding_Uninitialize(This)	\
    (This)->lpVtbl -> Uninitialize(This)

#define INapSystemHealthAgentBinding_NotifySoHChange(This)	\
    (This)->lpVtbl -> NotifySoHChange(This)

#define INapSystemHealthAgentBinding_GetSystemIsolationInfo(This,isolationInfo,unknownConnections)	\
    (This)->lpVtbl -> GetSystemIsolationInfo(This,isolationInfo,unknownConnections)

#define INapSystemHealthAgentBinding_FlushCache(This)	\
    (This)->lpVtbl -> FlushCache(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INapSystemHealthAgentBinding_Initialize_Proxy( 
    INapSystemHealthAgentBinding * This,
    /* [in] */ SystemHealthEntityId id,
    /* [in] */ INapSystemHealthAgentCallback *callback);


void __RPC_STUB INapSystemHealthAgentBinding_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentBinding_Uninitialize_Proxy( 
    INapSystemHealthAgentBinding * This);


void __RPC_STUB INapSystemHealthAgentBinding_Uninitialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentBinding_NotifySoHChange_Proxy( 
    INapSystemHealthAgentBinding * This);


void __RPC_STUB INapSystemHealthAgentBinding_NotifySoHChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentBinding_GetSystemIsolationInfo_Proxy( 
    INapSystemHealthAgentBinding * This,
    /* [out] */ IsolationInfo **isolationInfo,
    /* [out] */ BOOL *unknownConnections);


void __RPC_STUB INapSystemHealthAgentBinding_GetSystemIsolationInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentBinding_FlushCache_Proxy( 
    INapSystemHealthAgentBinding * This);


void __RPC_STUB INapSystemHealthAgentBinding_FlushCache_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INapSystemHealthAgentBinding_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_napsystemhealthagent_0255 */
/* [local] */ 




extern RPC_IF_HANDLE __MIDL_itf_napsystemhealthagent_0255_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_napsystemhealthagent_0255_v0_0_s_ifspec;

#ifndef __INapSystemHealthAgentCallback_INTERFACE_DEFINED__
#define __INapSystemHealthAgentCallback_INTERFACE_DEFINED__

/* interface INapSystemHealthAgentCallback */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_INapSystemHealthAgentCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F1072A57-214F-4ee2-8377-14EF140CD9F3")
    INapSystemHealthAgentCallback : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetSoHRequest( 
            /* [in] */ INapSystemHealthAgentRequest *request) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ProcessSoHResponse( 
            /* [in] */ INapSystemHealthAgentRequest *request) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NotifySystemIsolationStateChange( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFixupInfo( 
            /* [out] */ FixupInfo **info) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CompareSoHRequests( 
            /* [in] */ const SoHRequest *lhs,
            /* [in] */ const SoHRequest *rhs,
            /* [out] */ BOOL *isEqual) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NotifyOrphanedSoHRequest( 
            /* [in] */ const CorrelationId *correlationId) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INapSystemHealthAgentCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INapSystemHealthAgentCallback * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INapSystemHealthAgentCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INapSystemHealthAgentCallback * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSoHRequest )( 
            INapSystemHealthAgentCallback * This,
            /* [in] */ INapSystemHealthAgentRequest *request);
        
        HRESULT ( STDMETHODCALLTYPE *ProcessSoHResponse )( 
            INapSystemHealthAgentCallback * This,
            /* [in] */ INapSystemHealthAgentRequest *request);
        
        HRESULT ( STDMETHODCALLTYPE *NotifySystemIsolationStateChange )( 
            INapSystemHealthAgentCallback * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetFixupInfo )( 
            INapSystemHealthAgentCallback * This,
            /* [out] */ FixupInfo **info);
        
        HRESULT ( STDMETHODCALLTYPE *CompareSoHRequests )( 
            INapSystemHealthAgentCallback * This,
            /* [in] */ const SoHRequest *lhs,
            /* [in] */ const SoHRequest *rhs,
            /* [out] */ BOOL *isEqual);
        
        HRESULT ( STDMETHODCALLTYPE *NotifyOrphanedSoHRequest )( 
            INapSystemHealthAgentCallback * This,
            /* [in] */ const CorrelationId *correlationId);
        
        END_INTERFACE
    } INapSystemHealthAgentCallbackVtbl;

    interface INapSystemHealthAgentCallback
    {
        CONST_VTBL struct INapSystemHealthAgentCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INapSystemHealthAgentCallback_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INapSystemHealthAgentCallback_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INapSystemHealthAgentCallback_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INapSystemHealthAgentCallback_GetSoHRequest(This,request)	\
    (This)->lpVtbl -> GetSoHRequest(This,request)

#define INapSystemHealthAgentCallback_ProcessSoHResponse(This,request)	\
    (This)->lpVtbl -> ProcessSoHResponse(This,request)

#define INapSystemHealthAgentCallback_NotifySystemIsolationStateChange(This)	\
    (This)->lpVtbl -> NotifySystemIsolationStateChange(This)

#define INapSystemHealthAgentCallback_GetFixupInfo(This,info)	\
    (This)->lpVtbl -> GetFixupInfo(This,info)

#define INapSystemHealthAgentCallback_CompareSoHRequests(This,lhs,rhs,isEqual)	\
    (This)->lpVtbl -> CompareSoHRequests(This,lhs,rhs,isEqual)

#define INapSystemHealthAgentCallback_NotifyOrphanedSoHRequest(This,correlationId)	\
    (This)->lpVtbl -> NotifyOrphanedSoHRequest(This,correlationId)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INapSystemHealthAgentCallback_GetSoHRequest_Proxy( 
    INapSystemHealthAgentCallback * This,
    /* [in] */ INapSystemHealthAgentRequest *request);


void __RPC_STUB INapSystemHealthAgentCallback_GetSoHRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentCallback_ProcessSoHResponse_Proxy( 
    INapSystemHealthAgentCallback * This,
    /* [in] */ INapSystemHealthAgentRequest *request);


void __RPC_STUB INapSystemHealthAgentCallback_ProcessSoHResponse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentCallback_NotifySystemIsolationStateChange_Proxy( 
    INapSystemHealthAgentCallback * This);


void __RPC_STUB INapSystemHealthAgentCallback_NotifySystemIsolationStateChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentCallback_GetFixupInfo_Proxy( 
    INapSystemHealthAgentCallback * This,
    /* [out] */ FixupInfo **info);


void __RPC_STUB INapSystemHealthAgentCallback_GetFixupInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentCallback_CompareSoHRequests_Proxy( 
    INapSystemHealthAgentCallback * This,
    /* [in] */ const SoHRequest *lhs,
    /* [in] */ const SoHRequest *rhs,
    /* [out] */ BOOL *isEqual);


void __RPC_STUB INapSystemHealthAgentCallback_CompareSoHRequests_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentCallback_NotifyOrphanedSoHRequest_Proxy( 
    INapSystemHealthAgentCallback * This,
    /* [in] */ const CorrelationId *correlationId);


void __RPC_STUB INapSystemHealthAgentCallback_NotifyOrphanedSoHRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INapSystemHealthAgentCallback_INTERFACE_DEFINED__ */


#ifndef __INapSystemHealthAgentRequest_INTERFACE_DEFINED__
#define __INapSystemHealthAgentRequest_INTERFACE_DEFINED__

/* interface INapSystemHealthAgentRequest */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_INapSystemHealthAgentRequest;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5B360A69-212D-440d-B398-7EEFD497853A")
    INapSystemHealthAgentRequest : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCorrelationId( 
            /* [out] */ CorrelationId *correlationId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetStringCorrelationId( 
            /* [out] */ StringCorrelationId **correlationId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSoHRequest( 
            /* [in] */ const SoHRequest *sohRequest,
            /* [in] */ BOOL cacheSohForLaterUse) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSoHRequest( 
            /* [out] */ SoHRequest **sohRequest) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSoHResponse( 
            /* [out] */ SoHResponse **sohResponse,
            /* [out] */ UINT8 *flags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCacheSoHFlag( 
            BOOL *cacheSohForLaterUse) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INapSystemHealthAgentRequestVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INapSystemHealthAgentRequest * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INapSystemHealthAgentRequest * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INapSystemHealthAgentRequest * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCorrelationId )( 
            INapSystemHealthAgentRequest * This,
            /* [out] */ CorrelationId *correlationId);
        
        HRESULT ( STDMETHODCALLTYPE *GetStringCorrelationId )( 
            INapSystemHealthAgentRequest * This,
            /* [out] */ StringCorrelationId **correlationId);
        
        HRESULT ( STDMETHODCALLTYPE *SetSoHRequest )( 
            INapSystemHealthAgentRequest * This,
            /* [in] */ const SoHRequest *sohRequest,
            /* [in] */ BOOL cacheSohForLaterUse);
        
        HRESULT ( STDMETHODCALLTYPE *GetSoHRequest )( 
            INapSystemHealthAgentRequest * This,
            /* [out] */ SoHRequest **sohRequest);
        
        HRESULT ( STDMETHODCALLTYPE *GetSoHResponse )( 
            INapSystemHealthAgentRequest * This,
            /* [out] */ SoHResponse **sohResponse,
            /* [out] */ UINT8 *flags);
        
        HRESULT ( STDMETHODCALLTYPE *GetCacheSoHFlag )( 
            INapSystemHealthAgentRequest * This,
            BOOL *cacheSohForLaterUse);
        
        END_INTERFACE
    } INapSystemHealthAgentRequestVtbl;

    interface INapSystemHealthAgentRequest
    {
        CONST_VTBL struct INapSystemHealthAgentRequestVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INapSystemHealthAgentRequest_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INapSystemHealthAgentRequest_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INapSystemHealthAgentRequest_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INapSystemHealthAgentRequest_GetCorrelationId(This,correlationId)	\
    (This)->lpVtbl -> GetCorrelationId(This,correlationId)

#define INapSystemHealthAgentRequest_GetStringCorrelationId(This,correlationId)	\
    (This)->lpVtbl -> GetStringCorrelationId(This,correlationId)

#define INapSystemHealthAgentRequest_SetSoHRequest(This,sohRequest,cacheSohForLaterUse)	\
    (This)->lpVtbl -> SetSoHRequest(This,sohRequest,cacheSohForLaterUse)

#define INapSystemHealthAgentRequest_GetSoHRequest(This,sohRequest)	\
    (This)->lpVtbl -> GetSoHRequest(This,sohRequest)

#define INapSystemHealthAgentRequest_GetSoHResponse(This,sohResponse,flags)	\
    (This)->lpVtbl -> GetSoHResponse(This,sohResponse,flags)

#define INapSystemHealthAgentRequest_GetCacheSoHFlag(This,cacheSohForLaterUse)	\
    (This)->lpVtbl -> GetCacheSoHFlag(This,cacheSohForLaterUse)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INapSystemHealthAgentRequest_GetCorrelationId_Proxy( 
    INapSystemHealthAgentRequest * This,
    /* [out] */ CorrelationId *correlationId);


void __RPC_STUB INapSystemHealthAgentRequest_GetCorrelationId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentRequest_GetStringCorrelationId_Proxy( 
    INapSystemHealthAgentRequest * This,
    /* [out] */ StringCorrelationId **correlationId);


void __RPC_STUB INapSystemHealthAgentRequest_GetStringCorrelationId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentRequest_SetSoHRequest_Proxy( 
    INapSystemHealthAgentRequest * This,
    /* [in] */ const SoHRequest *sohRequest,
    /* [in] */ BOOL cacheSohForLaterUse);


void __RPC_STUB INapSystemHealthAgentRequest_SetSoHRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentRequest_GetSoHRequest_Proxy( 
    INapSystemHealthAgentRequest * This,
    /* [out] */ SoHRequest **sohRequest);


void __RPC_STUB INapSystemHealthAgentRequest_GetSoHRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentRequest_GetSoHResponse_Proxy( 
    INapSystemHealthAgentRequest * This,
    /* [out] */ SoHResponse **sohResponse,
    /* [out] */ UINT8 *flags);


void __RPC_STUB INapSystemHealthAgentRequest_GetSoHResponse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentRequest_GetCacheSoHFlag_Proxy( 
    INapSystemHealthAgentRequest * This,
    BOOL *cacheSohForLaterUse);


void __RPC_STUB INapSystemHealthAgentRequest_GetCacheSoHFlag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INapSystemHealthAgentRequest_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_napsystemhealthagent_0257 */
/* [local] */ 

// Declarations of CLSIDs of objects provided  
// by the system. Link to uuid.lib to get them 
// defined.                                    
EXTERN_C const CLSID 
   CLSID_NapSystemHealthAgentBinding;


extern RPC_IF_HANDLE __MIDL_itf_napsystemhealthagent_0257_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_napsystemhealthagent_0257_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


