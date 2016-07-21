

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Fri Nov 24 19:06:35 2006
 */
/* Compiler settings for .\PGIServer.idl:
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

#ifndef __PGIServerInterface_h__
#define __PGIServerInterface_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IPragueIServer_FWD_DEFINED__
#define __IPragueIServer_FWD_DEFINED__
typedef interface IPragueIServer IPragueIServer;
#endif 	/* __IPragueIServer_FWD_DEFINED__ */


#ifndef __PragueIServer_FWD_DEFINED__
#define __PragueIServer_FWD_DEFINED__

#ifdef __cplusplus
typedef class PragueIServer PragueIServer;
#else
typedef struct PragueIServer PragueIServer;
#endif /* __cplusplus */

#endif 	/* __PragueIServer_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_PGIServer_0000 */
/* [local] */ 

typedef /* [public][public][public][public][public][public][v1_enum][helpstring][uuid] */  DECLSPEC_UUID("FF078C60-B03F-11D4-96B2-444553540000") 
enum __MIDL___MIDL_itf_PGIServer_0000_0001
    {	itAny	= -1,
	itPrototype	= 0,
	itImplementation	= itPrototype + 1,
	itDataBase	= itImplementation + 1,
	itByContainer	= itDataBase + 1
    } 	ItType;



extern RPC_IF_HANDLE __MIDL_itf_PGIServer_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_PGIServer_0000_v0_0_s_ifspec;

#ifndef __IPragueIServer_INTERFACE_DEFINED__
#define __IPragueIServer_INTERFACE_DEFINED__

/* interface IPragueIServer */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IPragueIServer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E635B364-B186-11D4-96B2-444553540000")
    IPragueIServer : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetUIParent( 
            /* [in] */ HWND hWndParent) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Init( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ReInit( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Done( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetErrorString( 
            /* [in] */ HRESULT hError,
            /* [string][out] */ BYTE **pErrString) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE LoadContainer( 
            /* [string][in] */ BYTE *pContainerName,
            /* [in] */ BOOL bLockOnLoad,
            /* [out] */ DWORD *pdwSize,
            /* [size_is][size_is][out] */ BYTE **ppInterfaces) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE OpenContainer( 
            /* [string][out][in] */ BYTE **ppContainerName,
            /* [in] */ BOOL bLockOnLoad,
            /* [in] */ ItType eIType,
            /* [out] */ DWORD *pdwSize,
            /* [size_is][size_is][out] */ BYTE **ppInterfaces) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SaveContainer( 
            /* [string][out][in] */ BYTE **ppContainerName,
            /* [in] */ BOOL bSaveAs,
            /* [in] */ BOOL bLockOnSave,
            /* [in] */ DWORD dwSize,
            /* [size_is][in] */ BYTE *pInterfaces) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE LoadInterface( 
            /* [in] */ DWORD dwUIDSize,
            /* [size_is][in] */ BYTE *pUID,
            /* [in] */ BOOL bLockOnLoad,
            /* [in] */ ItType eIType,
            /* [out] */ DWORD *pdwSize,
            /* [size_is][size_is][out] */ BYTE **ppInterface) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EnumInterfaces( 
            /* [in] */ ItType eIType,
            /* [out] */ DWORD *pdwSize,
            /* [size_is][size_is][out] */ BYTE **ppInterfaces) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE LockContainer( 
            /* [string][in] */ BYTE *pContainerName,
            /* [in] */ BOOL bLock) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SaveDescriptionDB( 
            /* [string][in] */ BYTE *pContainerName,
            /* [in] */ DWORD dwSize,
            /* [size_is][in] */ BYTE *pInterfaces) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetStartingFolder( 
            /* [string][in] */ BYTE *pStartingFolder,
            /* [in] */ ItType eIType) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EnumContainers( 
            /* [in] */ ItType eIType,
            /* [out] */ DWORD *pdwSize,
            /* [size_is][size_is][out] */ BYTE **ppContainers) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPragueIServerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPragueIServer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPragueIServer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPragueIServer * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetUIParent )( 
            IPragueIServer * This,
            /* [in] */ HWND hWndParent);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Init )( 
            IPragueIServer * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ReInit )( 
            IPragueIServer * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Done )( 
            IPragueIServer * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetErrorString )( 
            IPragueIServer * This,
            /* [in] */ HRESULT hError,
            /* [string][out] */ BYTE **pErrString);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *LoadContainer )( 
            IPragueIServer * This,
            /* [string][in] */ BYTE *pContainerName,
            /* [in] */ BOOL bLockOnLoad,
            /* [out] */ DWORD *pdwSize,
            /* [size_is][size_is][out] */ BYTE **ppInterfaces);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *OpenContainer )( 
            IPragueIServer * This,
            /* [string][out][in] */ BYTE **ppContainerName,
            /* [in] */ BOOL bLockOnLoad,
            /* [in] */ ItType eIType,
            /* [out] */ DWORD *pdwSize,
            /* [size_is][size_is][out] */ BYTE **ppInterfaces);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SaveContainer )( 
            IPragueIServer * This,
            /* [string][out][in] */ BYTE **ppContainerName,
            /* [in] */ BOOL bSaveAs,
            /* [in] */ BOOL bLockOnSave,
            /* [in] */ DWORD dwSize,
            /* [size_is][in] */ BYTE *pInterfaces);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *LoadInterface )( 
            IPragueIServer * This,
            /* [in] */ DWORD dwUIDSize,
            /* [size_is][in] */ BYTE *pUID,
            /* [in] */ BOOL bLockOnLoad,
            /* [in] */ ItType eIType,
            /* [out] */ DWORD *pdwSize,
            /* [size_is][size_is][out] */ BYTE **ppInterface);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *EnumInterfaces )( 
            IPragueIServer * This,
            /* [in] */ ItType eIType,
            /* [out] */ DWORD *pdwSize,
            /* [size_is][size_is][out] */ BYTE **ppInterfaces);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *LockContainer )( 
            IPragueIServer * This,
            /* [string][in] */ BYTE *pContainerName,
            /* [in] */ BOOL bLock);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SaveDescriptionDB )( 
            IPragueIServer * This,
            /* [string][in] */ BYTE *pContainerName,
            /* [in] */ DWORD dwSize,
            /* [size_is][in] */ BYTE *pInterfaces);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetStartingFolder )( 
            IPragueIServer * This,
            /* [string][in] */ BYTE *pStartingFolder,
            /* [in] */ ItType eIType);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *EnumContainers )( 
            IPragueIServer * This,
            /* [in] */ ItType eIType,
            /* [out] */ DWORD *pdwSize,
            /* [size_is][size_is][out] */ BYTE **ppContainers);
        
        END_INTERFACE
    } IPragueIServerVtbl;

    interface IPragueIServer
    {
        CONST_VTBL struct IPragueIServerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPragueIServer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPragueIServer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPragueIServer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPragueIServer_SetUIParent(This,hWndParent)	\
    (This)->lpVtbl -> SetUIParent(This,hWndParent)

#define IPragueIServer_Init(This)	\
    (This)->lpVtbl -> Init(This)

#define IPragueIServer_ReInit(This)	\
    (This)->lpVtbl -> ReInit(This)

#define IPragueIServer_Done(This)	\
    (This)->lpVtbl -> Done(This)

#define IPragueIServer_GetErrorString(This,hError,pErrString)	\
    (This)->lpVtbl -> GetErrorString(This,hError,pErrString)

#define IPragueIServer_LoadContainer(This,pContainerName,bLockOnLoad,pdwSize,ppInterfaces)	\
    (This)->lpVtbl -> LoadContainer(This,pContainerName,bLockOnLoad,pdwSize,ppInterfaces)

#define IPragueIServer_OpenContainer(This,ppContainerName,bLockOnLoad,eIType,pdwSize,ppInterfaces)	\
    (This)->lpVtbl -> OpenContainer(This,ppContainerName,bLockOnLoad,eIType,pdwSize,ppInterfaces)

#define IPragueIServer_SaveContainer(This,ppContainerName,bSaveAs,bLockOnSave,dwSize,pInterfaces)	\
    (This)->lpVtbl -> SaveContainer(This,ppContainerName,bSaveAs,bLockOnSave,dwSize,pInterfaces)

#define IPragueIServer_LoadInterface(This,dwUIDSize,pUID,bLockOnLoad,eIType,pdwSize,ppInterface)	\
    (This)->lpVtbl -> LoadInterface(This,dwUIDSize,pUID,bLockOnLoad,eIType,pdwSize,ppInterface)

#define IPragueIServer_EnumInterfaces(This,eIType,pdwSize,ppInterfaces)	\
    (This)->lpVtbl -> EnumInterfaces(This,eIType,pdwSize,ppInterfaces)

#define IPragueIServer_LockContainer(This,pContainerName,bLock)	\
    (This)->lpVtbl -> LockContainer(This,pContainerName,bLock)

#define IPragueIServer_SaveDescriptionDB(This,pContainerName,dwSize,pInterfaces)	\
    (This)->lpVtbl -> SaveDescriptionDB(This,pContainerName,dwSize,pInterfaces)

#define IPragueIServer_SetStartingFolder(This,pStartingFolder,eIType)	\
    (This)->lpVtbl -> SetStartingFolder(This,pStartingFolder,eIType)

#define IPragueIServer_EnumContainers(This,eIType,pdwSize,ppContainers)	\
    (This)->lpVtbl -> EnumContainers(This,eIType,pdwSize,ppContainers)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIServer_SetUIParent_Proxy( 
    IPragueIServer * This,
    /* [in] */ HWND hWndParent);


void __RPC_STUB IPragueIServer_SetUIParent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIServer_Init_Proxy( 
    IPragueIServer * This);


void __RPC_STUB IPragueIServer_Init_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIServer_ReInit_Proxy( 
    IPragueIServer * This);


void __RPC_STUB IPragueIServer_ReInit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIServer_Done_Proxy( 
    IPragueIServer * This);


void __RPC_STUB IPragueIServer_Done_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIServer_GetErrorString_Proxy( 
    IPragueIServer * This,
    /* [in] */ HRESULT hError,
    /* [string][out] */ BYTE **pErrString);


void __RPC_STUB IPragueIServer_GetErrorString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIServer_LoadContainer_Proxy( 
    IPragueIServer * This,
    /* [string][in] */ BYTE *pContainerName,
    /* [in] */ BOOL bLockOnLoad,
    /* [out] */ DWORD *pdwSize,
    /* [size_is][size_is][out] */ BYTE **ppInterfaces);


void __RPC_STUB IPragueIServer_LoadContainer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIServer_OpenContainer_Proxy( 
    IPragueIServer * This,
    /* [string][out][in] */ BYTE **ppContainerName,
    /* [in] */ BOOL bLockOnLoad,
    /* [in] */ ItType eIType,
    /* [out] */ DWORD *pdwSize,
    /* [size_is][size_is][out] */ BYTE **ppInterfaces);


void __RPC_STUB IPragueIServer_OpenContainer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIServer_SaveContainer_Proxy( 
    IPragueIServer * This,
    /* [string][out][in] */ BYTE **ppContainerName,
    /* [in] */ BOOL bSaveAs,
    /* [in] */ BOOL bLockOnSave,
    /* [in] */ DWORD dwSize,
    /* [size_is][in] */ BYTE *pInterfaces);


void __RPC_STUB IPragueIServer_SaveContainer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIServer_LoadInterface_Proxy( 
    IPragueIServer * This,
    /* [in] */ DWORD dwUIDSize,
    /* [size_is][in] */ BYTE *pUID,
    /* [in] */ BOOL bLockOnLoad,
    /* [in] */ ItType eIType,
    /* [out] */ DWORD *pdwSize,
    /* [size_is][size_is][out] */ BYTE **ppInterface);


void __RPC_STUB IPragueIServer_LoadInterface_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIServer_EnumInterfaces_Proxy( 
    IPragueIServer * This,
    /* [in] */ ItType eIType,
    /* [out] */ DWORD *pdwSize,
    /* [size_is][size_is][out] */ BYTE **ppInterfaces);


void __RPC_STUB IPragueIServer_EnumInterfaces_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIServer_LockContainer_Proxy( 
    IPragueIServer * This,
    /* [string][in] */ BYTE *pContainerName,
    /* [in] */ BOOL bLock);


void __RPC_STUB IPragueIServer_LockContainer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIServer_SaveDescriptionDB_Proxy( 
    IPragueIServer * This,
    /* [string][in] */ BYTE *pContainerName,
    /* [in] */ DWORD dwSize,
    /* [size_is][in] */ BYTE *pInterfaces);


void __RPC_STUB IPragueIServer_SaveDescriptionDB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIServer_SetStartingFolder_Proxy( 
    IPragueIServer * This,
    /* [string][in] */ BYTE *pStartingFolder,
    /* [in] */ ItType eIType);


void __RPC_STUB IPragueIServer_SetStartingFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueIServer_EnumContainers_Proxy( 
    IPragueIServer * This,
    /* [in] */ ItType eIType,
    /* [out] */ DWORD *pdwSize,
    /* [size_is][size_is][out] */ BYTE **ppContainers);


void __RPC_STUB IPragueIServer_EnumContainers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPragueIServer_INTERFACE_DEFINED__ */



#ifndef __PGISERVERLib_LIBRARY_DEFINED__
#define __PGISERVERLib_LIBRARY_DEFINED__

/* library PGISERVERLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_PGISERVERLib;

EXTERN_C const CLSID CLSID_PragueIServer;

#ifdef __cplusplus

class DECLSPEC_UUID("661A3E92-B16B-11D4-96B2-444553540000")
PragueIServer;
#endif
#endif /* __PGISERVERLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  HWND_UserSize(     unsigned long *, unsigned long            , HWND * ); 
unsigned char * __RPC_USER  HWND_UserMarshal(  unsigned long *, unsigned char *, HWND * ); 
unsigned char * __RPC_USER  HWND_UserUnmarshal(unsigned long *, unsigned char *, HWND * ); 
void                      __RPC_USER  HWND_UserFree(     unsigned long *, HWND * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


