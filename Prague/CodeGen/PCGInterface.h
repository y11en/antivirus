

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Wed Jun 28 20:25:37 2006
 */
/* Compiler settings for .\CodeGen.idl:
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

#ifndef __PCGInterface_h__
#define __PCGInterface_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IPragueCodeGen_FWD_DEFINED__
#define __IPragueCodeGen_FWD_DEFINED__
typedef interface IPragueCodeGen IPragueCodeGen;
#endif 	/* __IPragueCodeGen_FWD_DEFINED__ */


#ifndef __PragueCodeGen_FWD_DEFINED__
#define __PragueCodeGen_FWD_DEFINED__

#ifdef __cplusplus
typedef class PragueCodeGen PragueCodeGen;
#else
typedef struct PragueCodeGen PragueCodeGen;
#endif /* __cplusplus */

#endif 	/* __PragueCodeGen_FWD_DEFINED__ */


#ifndef __PragueCodeGen2_FWD_DEFINED__
#define __PragueCodeGen2_FWD_DEFINED__

#ifdef __cplusplus
typedef class PragueCodeGen2 PragueCodeGen2;
#else
typedef struct PragueCodeGen2 PragueCodeGen2;
#endif /* __cplusplus */

#endif 	/* __PragueCodeGen2_FWD_DEFINED__ */


#ifndef __PragueCodeGenCPP_FWD_DEFINED__
#define __PragueCodeGenCPP_FWD_DEFINED__

#ifdef __cplusplus
typedef class PragueCodeGenCPP PragueCodeGenCPP;
#else
typedef struct PragueCodeGenCPP PragueCodeGenCPP;
#endif /* __cplusplus */

#endif 	/* __PragueCodeGenCPP_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IPragueCodeGen_INTERFACE_DEFINED__
#define __IPragueCodeGen_INTERFACE_DEFINED__

/* interface IPragueCodeGen */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IPragueCodeGen;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A11E88F1-D95A-11D3-BF0D-00D0B7161FB8")
    IPragueCodeGen : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDescription( 
            /* [string][out] */ BYTE **description_str) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetVendor( 
            /* [string][out] */ BYTE **vendor_str) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetVersion( 
            /* [string][out] */ BYTE **version_str) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetInfo( 
            /* [out] */ DWORD *len,
            /* [size_is][size_is][out] */ BYTE **info) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetPrototypeOptionTemplate( 
            /* [out] */ DWORD *len,
            /* [size_is][size_is][out] */ BYTE **options) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetImplementationOptionTemplate( 
            /* [out] */ DWORD *len,
            /* [size_is][size_is][out] */ BYTE **options) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetProjectOptions( 
            /* [in] */ DWORD *len,
            /* [size_is][size_is][out] */ BYTE **buffer) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetObjectTypes( 
            /* [in] */ DWORD len,
            /* [size_is][in] */ BYTE *objtypes) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE StartProject( 
            /* [in] */ DWORD len,
            /* [size_is][in] */ BYTE *buffer) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EndProject( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE PrototypeGenerate( 
            /* [in] */ DWORD clen,
            /* [size_is][in] */ BYTE *contents,
            /* [in] */ DWORD olen,
            /* [size_is][in] */ BYTE *options) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ImplementationGenerate( 
            /* [in] */ DWORD clen,
            /* [size_is][in] */ BYTE *contents,
            /* [in] */ DWORD olen,
            /* [size_is][in] */ BYTE *options) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EditNode( 
            /* [in] */ HWND dad,
            /* [out][in] */ DWORD *len,
            /* [size_is][size_is][out][in] */ BYTE **node) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetErrorString( 
            /* [in] */ HRESULT err,
            /* [string][out] */ BYTE **err_str) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPragueCodeGenVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPragueCodeGen * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPragueCodeGen * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPragueCodeGen * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetDescription )( 
            IPragueCodeGen * This,
            /* [string][out] */ BYTE **description_str);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetVendor )( 
            IPragueCodeGen * This,
            /* [string][out] */ BYTE **vendor_str);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetVersion )( 
            IPragueCodeGen * This,
            /* [string][out] */ BYTE **version_str);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IPragueCodeGen * This,
            /* [out] */ DWORD *len,
            /* [size_is][size_is][out] */ BYTE **info);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetPrototypeOptionTemplate )( 
            IPragueCodeGen * This,
            /* [out] */ DWORD *len,
            /* [size_is][size_is][out] */ BYTE **options);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetImplementationOptionTemplate )( 
            IPragueCodeGen * This,
            /* [out] */ DWORD *len,
            /* [size_is][size_is][out] */ BYTE **options);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetProjectOptions )( 
            IPragueCodeGen * This,
            /* [in] */ DWORD *len,
            /* [size_is][size_is][out] */ BYTE **buffer);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetObjectTypes )( 
            IPragueCodeGen * This,
            /* [in] */ DWORD len,
            /* [size_is][in] */ BYTE *objtypes);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *StartProject )( 
            IPragueCodeGen * This,
            /* [in] */ DWORD len,
            /* [size_is][in] */ BYTE *buffer);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *EndProject )( 
            IPragueCodeGen * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *PrototypeGenerate )( 
            IPragueCodeGen * This,
            /* [in] */ DWORD clen,
            /* [size_is][in] */ BYTE *contents,
            /* [in] */ DWORD olen,
            /* [size_is][in] */ BYTE *options);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ImplementationGenerate )( 
            IPragueCodeGen * This,
            /* [in] */ DWORD clen,
            /* [size_is][in] */ BYTE *contents,
            /* [in] */ DWORD olen,
            /* [size_is][in] */ BYTE *options);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *EditNode )( 
            IPragueCodeGen * This,
            /* [in] */ HWND dad,
            /* [out][in] */ DWORD *len,
            /* [size_is][size_is][out][in] */ BYTE **node);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetErrorString )( 
            IPragueCodeGen * This,
            /* [in] */ HRESULT err,
            /* [string][out] */ BYTE **err_str);
        
        END_INTERFACE
    } IPragueCodeGenVtbl;

    interface IPragueCodeGen
    {
        CONST_VTBL struct IPragueCodeGenVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPragueCodeGen_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPragueCodeGen_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPragueCodeGen_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPragueCodeGen_GetDescription(This,description_str)	\
    (This)->lpVtbl -> GetDescription(This,description_str)

#define IPragueCodeGen_GetVendor(This,vendor_str)	\
    (This)->lpVtbl -> GetVendor(This,vendor_str)

#define IPragueCodeGen_GetVersion(This,version_str)	\
    (This)->lpVtbl -> GetVersion(This,version_str)

#define IPragueCodeGen_GetInfo(This,len,info)	\
    (This)->lpVtbl -> GetInfo(This,len,info)

#define IPragueCodeGen_GetPrototypeOptionTemplate(This,len,options)	\
    (This)->lpVtbl -> GetPrototypeOptionTemplate(This,len,options)

#define IPragueCodeGen_GetImplementationOptionTemplate(This,len,options)	\
    (This)->lpVtbl -> GetImplementationOptionTemplate(This,len,options)

#define IPragueCodeGen_GetProjectOptions(This,len,buffer)	\
    (This)->lpVtbl -> GetProjectOptions(This,len,buffer)

#define IPragueCodeGen_SetObjectTypes(This,len,objtypes)	\
    (This)->lpVtbl -> SetObjectTypes(This,len,objtypes)

#define IPragueCodeGen_StartProject(This,len,buffer)	\
    (This)->lpVtbl -> StartProject(This,len,buffer)

#define IPragueCodeGen_EndProject(This)	\
    (This)->lpVtbl -> EndProject(This)

#define IPragueCodeGen_PrototypeGenerate(This,clen,contents,olen,options)	\
    (This)->lpVtbl -> PrototypeGenerate(This,clen,contents,olen,options)

#define IPragueCodeGen_ImplementationGenerate(This,clen,contents,olen,options)	\
    (This)->lpVtbl -> ImplementationGenerate(This,clen,contents,olen,options)

#define IPragueCodeGen_EditNode(This,dad,len,node)	\
    (This)->lpVtbl -> EditNode(This,dad,len,node)

#define IPragueCodeGen_GetErrorString(This,err,err_str)	\
    (This)->lpVtbl -> GetErrorString(This,err,err_str)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueCodeGen_GetDescription_Proxy( 
    IPragueCodeGen * This,
    /* [string][out] */ BYTE **description_str);


void __RPC_STUB IPragueCodeGen_GetDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueCodeGen_GetVendor_Proxy( 
    IPragueCodeGen * This,
    /* [string][out] */ BYTE **vendor_str);


void __RPC_STUB IPragueCodeGen_GetVendor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueCodeGen_GetVersion_Proxy( 
    IPragueCodeGen * This,
    /* [string][out] */ BYTE **version_str);


void __RPC_STUB IPragueCodeGen_GetVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueCodeGen_GetInfo_Proxy( 
    IPragueCodeGen * This,
    /* [out] */ DWORD *len,
    /* [size_is][size_is][out] */ BYTE **info);


void __RPC_STUB IPragueCodeGen_GetInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueCodeGen_GetPrototypeOptionTemplate_Proxy( 
    IPragueCodeGen * This,
    /* [out] */ DWORD *len,
    /* [size_is][size_is][out] */ BYTE **options);


void __RPC_STUB IPragueCodeGen_GetPrototypeOptionTemplate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueCodeGen_GetImplementationOptionTemplate_Proxy( 
    IPragueCodeGen * This,
    /* [out] */ DWORD *len,
    /* [size_is][size_is][out] */ BYTE **options);


void __RPC_STUB IPragueCodeGen_GetImplementationOptionTemplate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueCodeGen_GetProjectOptions_Proxy( 
    IPragueCodeGen * This,
    /* [in] */ DWORD *len,
    /* [size_is][size_is][out] */ BYTE **buffer);


void __RPC_STUB IPragueCodeGen_GetProjectOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueCodeGen_SetObjectTypes_Proxy( 
    IPragueCodeGen * This,
    /* [in] */ DWORD len,
    /* [size_is][in] */ BYTE *objtypes);


void __RPC_STUB IPragueCodeGen_SetObjectTypes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueCodeGen_StartProject_Proxy( 
    IPragueCodeGen * This,
    /* [in] */ DWORD len,
    /* [size_is][in] */ BYTE *buffer);


void __RPC_STUB IPragueCodeGen_StartProject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueCodeGen_EndProject_Proxy( 
    IPragueCodeGen * This);


void __RPC_STUB IPragueCodeGen_EndProject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueCodeGen_PrototypeGenerate_Proxy( 
    IPragueCodeGen * This,
    /* [in] */ DWORD clen,
    /* [size_is][in] */ BYTE *contents,
    /* [in] */ DWORD olen,
    /* [size_is][in] */ BYTE *options);


void __RPC_STUB IPragueCodeGen_PrototypeGenerate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueCodeGen_ImplementationGenerate_Proxy( 
    IPragueCodeGen * This,
    /* [in] */ DWORD clen,
    /* [size_is][in] */ BYTE *contents,
    /* [in] */ DWORD olen,
    /* [size_is][in] */ BYTE *options);


void __RPC_STUB IPragueCodeGen_ImplementationGenerate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueCodeGen_EditNode_Proxy( 
    IPragueCodeGen * This,
    /* [in] */ HWND dad,
    /* [out][in] */ DWORD *len,
    /* [size_is][size_is][out][in] */ BYTE **node);


void __RPC_STUB IPragueCodeGen_EditNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPragueCodeGen_GetErrorString_Proxy( 
    IPragueCodeGen * This,
    /* [in] */ HRESULT err,
    /* [string][out] */ BYTE **err_str);


void __RPC_STUB IPragueCodeGen_GetErrorString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPragueCodeGen_INTERFACE_DEFINED__ */



#ifndef __CODEGENLib_LIBRARY_DEFINED__
#define __CODEGENLib_LIBRARY_DEFINED__

/* library CODEGENLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_CODEGENLib;

EXTERN_C const CLSID CLSID_PragueCodeGen;

#ifdef __cplusplus

class DECLSPEC_UUID("A11E88F2-D95A-11D3-BF0D-00D0B7161FB8")
PragueCodeGen;
#endif

EXTERN_C const CLSID CLSID_PragueCodeGen2;

#ifdef __cplusplus

class DECLSPEC_UUID("54D5E27B-3C4B-4525-8034-7AEF3D9A25C2")
PragueCodeGen2;
#endif

EXTERN_C const CLSID CLSID_PragueCodeGenCPP;

#ifdef __cplusplus

class DECLSPEC_UUID("083B0272-8EC4-4330-919D-F20B74637DDC")
PragueCodeGenCPP;
#endif
#endif /* __CODEGENLib_LIBRARY_DEFINED__ */

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


