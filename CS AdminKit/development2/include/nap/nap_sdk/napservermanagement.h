
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Oct 29 17:32:24 2007
 */
/* Compiler settings for O:\CS AdminKit\development2\nap\nap_sdk\include\napservermanagement.idl:
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

#ifndef __napservermanagement_h__
#define __napservermanagement_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __INapServerInfo_FWD_DEFINED__
#define __INapServerInfo_FWD_DEFINED__
typedef interface INapServerInfo INapServerInfo;
#endif 	/* __INapServerInfo_FWD_DEFINED__ */


#ifndef __INapServerManagement_FWD_DEFINED__
#define __INapServerManagement_FWD_DEFINED__
typedef interface INapServerManagement INapServerManagement;
#endif 	/* __INapServerManagement_FWD_DEFINED__ */


/* header files for imported files */
#include "NapTypes.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __INapServerInfo_INTERFACE_DEFINED__
#define __INapServerInfo_INTERFACE_DEFINED__

/* interface INapServerInfo */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_INapServerInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("599F9021-5643-4965-9949-E88975EFFF0E")
    INapServerInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetNapServerInfo( 
            /* [out] */ CountedString **serverName,
            /* [out] */ CountedString **serverDescription,
            /* [out] */ CountedString **protocolVersion) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRegisteredSystemHealthValidators( 
            /* [out] */ SystemHealthEntityCount *count,
            /* [size_is][size_is][out] */ NapComponentRegistrationInfo **validators,
            /* [size_is][size_is][out] */ CLSID **validatorClsids) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFailureCategoryMappings( 
            /* [in] */ SystemHealthEntityId id,
            /* [out] */ FailureCategoryMapping *mapping) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INapServerInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INapServerInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INapServerInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INapServerInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetNapServerInfo )( 
            INapServerInfo * This,
            /* [out] */ CountedString **serverName,
            /* [out] */ CountedString **serverDescription,
            /* [out] */ CountedString **protocolVersion);
        
        HRESULT ( STDMETHODCALLTYPE *GetRegisteredSystemHealthValidators )( 
            INapServerInfo * This,
            /* [out] */ SystemHealthEntityCount *count,
            /* [size_is][size_is][out] */ NapComponentRegistrationInfo **validators,
            /* [size_is][size_is][out] */ CLSID **validatorClsids);
        
        HRESULT ( STDMETHODCALLTYPE *GetFailureCategoryMappings )( 
            INapServerInfo * This,
            /* [in] */ SystemHealthEntityId id,
            /* [out] */ FailureCategoryMapping *mapping);
        
        END_INTERFACE
    } INapServerInfoVtbl;

    interface INapServerInfo
    {
        CONST_VTBL struct INapServerInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INapServerInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INapServerInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INapServerInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INapServerInfo_GetNapServerInfo(This,serverName,serverDescription,protocolVersion)	\
    (This)->lpVtbl -> GetNapServerInfo(This,serverName,serverDescription,protocolVersion)

#define INapServerInfo_GetRegisteredSystemHealthValidators(This,count,validators,validatorClsids)	\
    (This)->lpVtbl -> GetRegisteredSystemHealthValidators(This,count,validators,validatorClsids)

#define INapServerInfo_GetFailureCategoryMappings(This,id,mapping)	\
    (This)->lpVtbl -> GetFailureCategoryMappings(This,id,mapping)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INapServerInfo_GetNapServerInfo_Proxy( 
    INapServerInfo * This,
    /* [out] */ CountedString **serverName,
    /* [out] */ CountedString **serverDescription,
    /* [out] */ CountedString **protocolVersion);


void __RPC_STUB INapServerInfo_GetNapServerInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapServerInfo_GetRegisteredSystemHealthValidators_Proxy( 
    INapServerInfo * This,
    /* [out] */ SystemHealthEntityCount *count,
    /* [size_is][size_is][out] */ NapComponentRegistrationInfo **validators,
    /* [size_is][size_is][out] */ CLSID **validatorClsids);


void __RPC_STUB INapServerInfo_GetRegisteredSystemHealthValidators_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapServerInfo_GetFailureCategoryMappings_Proxy( 
    INapServerInfo * This,
    /* [in] */ SystemHealthEntityId id,
    /* [out] */ FailureCategoryMapping *mapping);


void __RPC_STUB INapServerInfo_GetFailureCategoryMappings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INapServerInfo_INTERFACE_DEFINED__ */


#ifndef __INapServerManagement_INTERFACE_DEFINED__
#define __INapServerManagement_INTERFACE_DEFINED__

/* interface INapServerManagement */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_INapServerManagement;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9DE543E7-0F23-47e0-A8BC-971A894F86D4")
    INapServerManagement : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE RegisterSystemHealthValidator( 
            /* [in] */ const NapComponentRegistrationInfo *validator,
            /* [in] */ const CLSID *validatorClsid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnregisterSystemHealthValidator( 
            /* [in] */ SystemHealthEntityId id) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFailureCategoryMappings( 
            /* [in] */ SystemHealthEntityId id,
            /* [in] */ const FailureCategoryMapping mapping) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INapServerManagementVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INapServerManagement * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INapServerManagement * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INapServerManagement * This);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterSystemHealthValidator )( 
            INapServerManagement * This,
            /* [in] */ const NapComponentRegistrationInfo *validator,
            /* [in] */ const CLSID *validatorClsid);
        
        HRESULT ( STDMETHODCALLTYPE *UnregisterSystemHealthValidator )( 
            INapServerManagement * This,
            /* [in] */ SystemHealthEntityId id);
        
        HRESULT ( STDMETHODCALLTYPE *SetFailureCategoryMappings )( 
            INapServerManagement * This,
            /* [in] */ SystemHealthEntityId id,
            /* [in] */ const FailureCategoryMapping mapping);
        
        END_INTERFACE
    } INapServerManagementVtbl;

    interface INapServerManagement
    {
        CONST_VTBL struct INapServerManagementVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INapServerManagement_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INapServerManagement_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INapServerManagement_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INapServerManagement_RegisterSystemHealthValidator(This,validator,validatorClsid)	\
    (This)->lpVtbl -> RegisterSystemHealthValidator(This,validator,validatorClsid)

#define INapServerManagement_UnregisterSystemHealthValidator(This,id)	\
    (This)->lpVtbl -> UnregisterSystemHealthValidator(This,id)

#define INapServerManagement_SetFailureCategoryMappings(This,id,mapping)	\
    (This)->lpVtbl -> SetFailureCategoryMappings(This,id,mapping)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INapServerManagement_RegisterSystemHealthValidator_Proxy( 
    INapServerManagement * This,
    /* [in] */ const NapComponentRegistrationInfo *validator,
    /* [in] */ const CLSID *validatorClsid);


void __RPC_STUB INapServerManagement_RegisterSystemHealthValidator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapServerManagement_UnregisterSystemHealthValidator_Proxy( 
    INapServerManagement * This,
    /* [in] */ SystemHealthEntityId id);


void __RPC_STUB INapServerManagement_UnregisterSystemHealthValidator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapServerManagement_SetFailureCategoryMappings_Proxy( 
    INapServerManagement * This,
    /* [in] */ SystemHealthEntityId id,
    /* [in] */ const FailureCategoryMapping mapping);


void __RPC_STUB INapServerManagement_SetFailureCategoryMappings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INapServerManagement_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_napservermanagement_0256 */
/* [local] */ 

// Declarations of CLSIDs of objects provided  
// by the system. Link to uuid.lib to get them 
// defined.                                    
EXTERN_C const CLSID 
   CLSID_NapServerInfo;
EXTERN_C const CLSID 
   CLSID_NapServerManagement;


extern RPC_IF_HANDLE __MIDL_itf_napservermanagement_0256_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_napservermanagement_0256_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


