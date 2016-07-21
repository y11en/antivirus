
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Tue Oct 30 15:53:36 2007
 */
/* Compiler settings for O:\CS AdminKit\development2\nap\nap_sdk\include\napmanagement.idl:
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

#ifndef __napmanagement_h__
#define __napmanagement_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __INapClientManagement_FWD_DEFINED__
#define __INapClientManagement_FWD_DEFINED__
typedef interface INapClientManagement INapClientManagement;
#endif 	/* __INapClientManagement_FWD_DEFINED__ */


/* header files for imported files */
#include "NapTypes.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __INapClientManagement_INTERFACE_DEFINED__
#define __INapClientManagement_INTERFACE_DEFINED__

/* interface INapClientManagement */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_INapClientManagement;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("432a1da5-3888-4b9a-a734-cff1e448c5b9")
    INapClientManagement : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetNapClientInfo( 
            /* [out] */ BOOL *isNapEnabled,
            /* [out] */ CountedString **clientName,
            /* [out] */ CountedString **clientDescription,
            /* [out] */ CountedString **protocolVersion) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSystemIsolationInfo( 
            /* [out] */ IsolationInfo **isolationInfo,
            /* [out] */ BOOL *unknownConnections) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RegisterSystemHealthAgent( 
            /* [in] */ const NapComponentRegistrationInfo *agent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnregisterSystemHealthAgent( 
            /* [in] */ SystemHealthEntityId id) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RegisterEnforcementClient( 
            /* [in] */ const NapComponentRegistrationInfo *enforcer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnregisterEnforcementClient( 
            /* [in] */ EnforcementEntityId id) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRegisteredSystemHealthAgents( 
            /* [out] */ SystemHealthEntityCount *count,
            /* [size_is][size_is][out] */ NapComponentRegistrationInfo **agents) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRegisteredEnforcementClients( 
            /* [out] */ EnforcementEntityCount *count,
            /* [size_is][size_is][out] */ NapComponentRegistrationInfo **enforcers) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INapClientManagementVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INapClientManagement * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INapClientManagement * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INapClientManagement * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetNapClientInfo )( 
            INapClientManagement * This,
            /* [out] */ BOOL *isNapEnabled,
            /* [out] */ CountedString **clientName,
            /* [out] */ CountedString **clientDescription,
            /* [out] */ CountedString **protocolVersion);
        
        HRESULT ( STDMETHODCALLTYPE *GetSystemIsolationInfo )( 
            INapClientManagement * This,
            /* [out] */ IsolationInfo **isolationInfo,
            /* [out] */ BOOL *unknownConnections);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterSystemHealthAgent )( 
            INapClientManagement * This,
            /* [in] */ const NapComponentRegistrationInfo *agent);
        
        HRESULT ( STDMETHODCALLTYPE *UnregisterSystemHealthAgent )( 
            INapClientManagement * This,
            /* [in] */ SystemHealthEntityId id);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterEnforcementClient )( 
            INapClientManagement * This,
            /* [in] */ const NapComponentRegistrationInfo *enforcer);
        
        HRESULT ( STDMETHODCALLTYPE *UnregisterEnforcementClient )( 
            INapClientManagement * This,
            /* [in] */ EnforcementEntityId id);
        
        HRESULT ( STDMETHODCALLTYPE *GetRegisteredSystemHealthAgents )( 
            INapClientManagement * This,
            /* [out] */ SystemHealthEntityCount *count,
            /* [size_is][size_is][out] */ NapComponentRegistrationInfo **agents);
        
        HRESULT ( STDMETHODCALLTYPE *GetRegisteredEnforcementClients )( 
            INapClientManagement * This,
            /* [out] */ EnforcementEntityCount *count,
            /* [size_is][size_is][out] */ NapComponentRegistrationInfo **enforcers);
        
        END_INTERFACE
    } INapClientManagementVtbl;

    interface INapClientManagement
    {
        CONST_VTBL struct INapClientManagementVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INapClientManagement_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INapClientManagement_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INapClientManagement_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INapClientManagement_GetNapClientInfo(This,isNapEnabled,clientName,clientDescription,protocolVersion)	\
    (This)->lpVtbl -> GetNapClientInfo(This,isNapEnabled,clientName,clientDescription,protocolVersion)

#define INapClientManagement_GetSystemIsolationInfo(This,isolationInfo,unknownConnections)	\
    (This)->lpVtbl -> GetSystemIsolationInfo(This,isolationInfo,unknownConnections)

#define INapClientManagement_RegisterSystemHealthAgent(This,agent)	\
    (This)->lpVtbl -> RegisterSystemHealthAgent(This,agent)

#define INapClientManagement_UnregisterSystemHealthAgent(This,id)	\
    (This)->lpVtbl -> UnregisterSystemHealthAgent(This,id)

#define INapClientManagement_RegisterEnforcementClient(This,enforcer)	\
    (This)->lpVtbl -> RegisterEnforcementClient(This,enforcer)

#define INapClientManagement_UnregisterEnforcementClient(This,id)	\
    (This)->lpVtbl -> UnregisterEnforcementClient(This,id)

#define INapClientManagement_GetRegisteredSystemHealthAgents(This,count,agents)	\
    (This)->lpVtbl -> GetRegisteredSystemHealthAgents(This,count,agents)

#define INapClientManagement_GetRegisteredEnforcementClients(This,count,enforcers)	\
    (This)->lpVtbl -> GetRegisteredEnforcementClients(This,count,enforcers)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INapClientManagement_GetNapClientInfo_Proxy( 
    INapClientManagement * This,
    /* [out] */ BOOL *isNapEnabled,
    /* [out] */ CountedString **clientName,
    /* [out] */ CountedString **clientDescription,
    /* [out] */ CountedString **protocolVersion);


void __RPC_STUB INapClientManagement_GetNapClientInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapClientManagement_GetSystemIsolationInfo_Proxy( 
    INapClientManagement * This,
    /* [out] */ IsolationInfo **isolationInfo,
    /* [out] */ BOOL *unknownConnections);


void __RPC_STUB INapClientManagement_GetSystemIsolationInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapClientManagement_RegisterSystemHealthAgent_Proxy( 
    INapClientManagement * This,
    /* [in] */ const NapComponentRegistrationInfo *agent);


void __RPC_STUB INapClientManagement_RegisterSystemHealthAgent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapClientManagement_UnregisterSystemHealthAgent_Proxy( 
    INapClientManagement * This,
    /* [in] */ SystemHealthEntityId id);


void __RPC_STUB INapClientManagement_UnregisterSystemHealthAgent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapClientManagement_RegisterEnforcementClient_Proxy( 
    INapClientManagement * This,
    /* [in] */ const NapComponentRegistrationInfo *enforcer);


void __RPC_STUB INapClientManagement_RegisterEnforcementClient_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapClientManagement_UnregisterEnforcementClient_Proxy( 
    INapClientManagement * This,
    /* [in] */ EnforcementEntityId id);


void __RPC_STUB INapClientManagement_UnregisterEnforcementClient_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapClientManagement_GetRegisteredSystemHealthAgents_Proxy( 
    INapClientManagement * This,
    /* [out] */ SystemHealthEntityCount *count,
    /* [size_is][size_is][out] */ NapComponentRegistrationInfo **agents);


void __RPC_STUB INapClientManagement_GetRegisteredSystemHealthAgents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapClientManagement_GetRegisteredEnforcementClients_Proxy( 
    INapClientManagement * This,
    /* [out] */ EnforcementEntityCount *count,
    /* [size_is][size_is][out] */ NapComponentRegistrationInfo **enforcers);


void __RPC_STUB INapClientManagement_GetRegisteredEnforcementClients_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INapClientManagement_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_napmanagement_0255 */
/* [local] */ 

// Declarations of CLSIDs of objects provided  
// by the system. Link to uuid.lib to get them 
// defined.                                    
EXTERN_C const CLSID 
   CLSID_NapClientManagement;


extern RPC_IF_HANDLE __MIDL_itf_napmanagement_0255_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_napmanagement_0255_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


