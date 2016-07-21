

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Tue Sep 18 15:51:22 2007
 */
/* Compiler settings for .\IDServer.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
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

#ifndef __IDServerInterface_h__
#define __IDServerInterface_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IPragueIDServer_FWD_DEFINED__
#define __IPragueIDServer_FWD_DEFINED__
typedef interface IPragueIDServer IPragueIDServer;
#endif 	/* __IPragueIDServer_FWD_DEFINED__ */


#ifndef __PragueIDServer_FWD_DEFINED__
#define __PragueIDServer_FWD_DEFINED__

#ifdef __cplusplus
typedef class PragueIDServer PragueIDServer;
#else
typedef struct PragueIDServer PragueIDServer;
#endif /* __cplusplus */

#endif 	/* __PragueIDServer_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_IDServer_0000 */
/* [local] */ 

typedef /* [public][public][public][public][public][v1_enum][helpstring][uuid] */  DECLSPEC_UUID("183EB460-06EF-11D4-96B1-00D0B71DDDF6") 
enum __MIDL___MIDL_itf_IDServer_0000_0001
    {	pic_Interface	= 0,
	pic_Plugin	= pic_Interface + 1,
	pic_Vendor	= pic_Plugin + 1
    } 	PgIDCategories;



extern RPC_IF_HANDLE __MIDL_itf_IDServer_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_IDServer_0000_v0_0_s_ifspec;

#ifndef __IPragueIDServer_INTERFACE_DEFINED__
#define __IPragueIDServer_INTERFACE_DEFINED__

/* interface IPragueIDServer */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IPragueIDServer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("60FE9D11-06EA-11D4-96B1-00D0B71DDDF6")
    IPragueIDServer : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetID( 
            /* [in] */ PgIDCategories eCategory,
            /* [out] */ DWORD *pdwID) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ReleaseID( 
            /* [in] */ PgIDCategories eCategory,
            /* [in] */ DWORD dwID) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetIDInUse( 
            /* [in] */ PgIDCategories eCategory,
            /* [in] */ DWORD dwID) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CheckIDInUse( 
            /* [in] */ PgIDCategories eCategory,
            /* [in] */ DWORD dwID) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetErrorString( 
            /* [in] */ HRESULT hErr,
            /* [string][out] */ BYTE **pErrString) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetVendorMnemonicID( 
            /* [in] */ DWORD dwSize,
            /* [size_is][out][in] */ BYTE *pMnemonicID) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetVendorName( 
            /* [in] */ DWORD dwSize,
            /* [size_is][out][in] */ BYTE *pVendorName) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ForceRegistryUse( 
            /* [in] */ DWORD dwSet) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPragueIDServerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPragueIDServer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPragueIDServer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPragueIDServer * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetID )( 
            IPragueIDServer * This,
            /* [in] */ PgIDCategories eCategory,
            /* [out] */ DWORD *pdwID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ReleaseID )( 
            IPragueIDServer * This,
            /* [in] */ PgIDCategories eCategory,
            /* [in] */ DWORD dwID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetIDInUse )( 
            IPragueIDServer * This,
            /* [in] */ PgIDCategories eCategory,
            /* [in] */ DWORD dwID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CheckIDInUse )( 
            IPragueIDServer * This,
            /* [in] */ PgIDCategories eCategory,
            /* [in] */ DWORD dwID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetErrorString )( 
            IPragueIDServer * This,
            /* [in] */ HRESULT hErr,
            /* [string][out] */ BYTE **pErrString);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetVendorMnemonicID )( 
            IPragueIDServer * This,
            /* [in] */ DWORD dwSize,
            /* [size_is][out][in] */ BYTE *pMnemonicID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetVendorName )( 
            IPragueIDServer * This,
            /* [in] */ DWORD dwSize,
            /* [size_is][out][in] */ BYTE *pVendorName);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ForceRegistryUse )( 
            IPragueIDServer * This,
            /* [in] */ DWORD dwSet);
        
        END_INTERFACE
    } IPragueIDServerVtbl;

    interface IPragueIDServer
    {
        CONST_VTBL struct IPragueIDServerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPragueIDServer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPragueIDServer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPragueIDServer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPragueIDServer_GetID(This,eCategory,pdwID)	\
    (This)->lpVtbl -> GetID(This,eCategory,pdwID)

#define IPragueIDServer_ReleaseID(This,eCategory,dwID)	\
    (This)->lpVtbl -> ReleaseID(This,eCategory,dwID)

#define IPragueIDServer_SetIDInUse(This,eCategory,dwID)	\
    (This)->lpVtbl -> SetIDInUse(This,eCategory,dwID)

#define IPragueIDServer_CheckIDInUse(This,eCategory,dwID)	\
    (This)->lpVtbl -> CheckIDInUse(This,eCategory,dwID)

#define IPragueIDServer_GetErrorString(This,hErr,pErrString)	\
    (This)->lpVtbl -> GetErrorString(This,hErr,pErrString)

#define IPragueIDServer_GetVendorMnemonicID(This,dwSize,pMnemonicID)	\
    (This)->lpVtbl -> GetVendorMnemonicID(This,dwSize,pMnemonicID)

#define IPragueIDServer_GetVendorName(This,dwSize,pVendorName)	\
    (This)->lpVtbl -> GetVendorName(This,dwSize,pVendorName)

#define IPragueIDServer_ForceRegistryUse(This,dwSet)	\
    (This)->lpVtbl -> ForceRegistryUse(This,dwSet)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIDServer_GetID_Proxy( 
    IPragueIDServer * This,
    /* [in] */ PgIDCategories eCategory,
    /* [out] */ DWORD *pdwID);


void __RPC_STUB IPragueIDServer_GetID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIDServer_ReleaseID_Proxy( 
    IPragueIDServer * This,
    /* [in] */ PgIDCategories eCategory,
    /* [in] */ DWORD dwID);


void __RPC_STUB IPragueIDServer_ReleaseID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIDServer_SetIDInUse_Proxy( 
    IPragueIDServer * This,
    /* [in] */ PgIDCategories eCategory,
    /* [in] */ DWORD dwID);


void __RPC_STUB IPragueIDServer_SetIDInUse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIDServer_CheckIDInUse_Proxy( 
    IPragueIDServer * This,
    /* [in] */ PgIDCategories eCategory,
    /* [in] */ DWORD dwID);


void __RPC_STUB IPragueIDServer_CheckIDInUse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIDServer_GetErrorString_Proxy( 
    IPragueIDServer * This,
    /* [in] */ HRESULT hErr,
    /* [string][out] */ BYTE **pErrString);


void __RPC_STUB IPragueIDServer_GetErrorString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIDServer_GetVendorMnemonicID_Proxy( 
    IPragueIDServer * This,
    /* [in] */ DWORD dwSize,
    /* [size_is][out][in] */ BYTE *pMnemonicID);


void __RPC_STUB IPragueIDServer_GetVendorMnemonicID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIDServer_GetVendorName_Proxy( 
    IPragueIDServer * This,
    /* [in] */ DWORD dwSize,
    /* [size_is][out][in] */ BYTE *pVendorName);


void __RPC_STUB IPragueIDServer_GetVendorName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIDServer_ForceRegistryUse_Proxy( 
    IPragueIDServer * This,
    /* [in] */ DWORD dwSet);


void __RPC_STUB IPragueIDServer_ForceRegistryUse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPragueIDServer_INTERFACE_DEFINED__ */



#ifndef __IDSERVERLib_LIBRARY_DEFINED__
#define __IDSERVERLib_LIBRARY_DEFINED__

/* library IDSERVERLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_IDSERVERLib;

EXTERN_C const CLSID CLSID_PragueIDServer;

#ifdef __cplusplus

class DECLSPEC_UUID("60FE9D12-06EA-11D4-96B1-00D0B71DDDF6")
PragueIDServer;
#endif
#endif /* __IDSERVERLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


