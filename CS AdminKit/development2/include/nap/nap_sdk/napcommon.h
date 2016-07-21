
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Wed Dec 05 11:12:06 2007
 */
/* Compiler settings for O:\CS AdminKit\development2\include\nap\nap_sdk\napcommon.idl:
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

#ifndef __napcommon_h__
#define __napcommon_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __INapComponentInfo_FWD_DEFINED__
#define __INapComponentInfo_FWD_DEFINED__
typedef interface INapComponentInfo INapComponentInfo;
#endif 	/* __INapComponentInfo_FWD_DEFINED__ */


#ifndef __INapComponentConfig_FWD_DEFINED__
#define __INapComponentConfig_FWD_DEFINED__
typedef interface INapComponentConfig INapComponentConfig;
#endif 	/* __INapComponentConfig_FWD_DEFINED__ */


/* header files for imported files */
#include "NapTypes.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __INapComponentInfo_INTERFACE_DEFINED__
#define __INapComponentInfo_INTERFACE_DEFINED__

/* interface INapComponentInfo */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_INapComponentInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B475F925-E3F7-414c-8C72-1CEE64B9D8F6")
    INapComponentInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetFriendlyName( 
            /* [out] */ MessageId *friendlyName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDescription( 
            /* [out] */ MessageId *description) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVendorName( 
            /* [out] */ MessageId *vendorName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetVersion( 
            /* [out] */ MessageId *version) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIcon( 
            /* [out] */ CountedString **dllFilePath,
            /* [out] */ UINT32 *iconResourceId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ConvertErrorCodeToMessageId( 
            /* [in] */ HRESULT errorCode,
            /* [out] */ MessageId *msgId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLocalizedString( 
            /* [in] */ MessageId msgId,
            /* [out] */ CountedString **string) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INapComponentInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INapComponentInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INapComponentInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INapComponentInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetFriendlyName )( 
            INapComponentInfo * This,
            /* [out] */ MessageId *friendlyName);
        
        HRESULT ( STDMETHODCALLTYPE *GetDescription )( 
            INapComponentInfo * This,
            /* [out] */ MessageId *description);
        
        HRESULT ( STDMETHODCALLTYPE *GetVendorName )( 
            INapComponentInfo * This,
            /* [out] */ MessageId *vendorName);
        
        HRESULT ( STDMETHODCALLTYPE *GetVersion )( 
            INapComponentInfo * This,
            /* [out] */ MessageId *version);
        
        HRESULT ( STDMETHODCALLTYPE *GetIcon )( 
            INapComponentInfo * This,
            /* [out] */ CountedString **dllFilePath,
            /* [out] */ UINT32 *iconResourceId);
        
        HRESULT ( STDMETHODCALLTYPE *ConvertErrorCodeToMessageId )( 
            INapComponentInfo * This,
            /* [in] */ HRESULT errorCode,
            /* [out] */ MessageId *msgId);
        
        HRESULT ( STDMETHODCALLTYPE *GetLocalizedString )( 
            INapComponentInfo * This,
            /* [in] */ MessageId msgId,
            /* [out] */ CountedString **string);
        
        END_INTERFACE
    } INapComponentInfoVtbl;

    interface INapComponentInfo
    {
        CONST_VTBL struct INapComponentInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INapComponentInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INapComponentInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INapComponentInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INapComponentInfo_GetFriendlyName(This,friendlyName)	\
    (This)->lpVtbl -> GetFriendlyName(This,friendlyName)

#define INapComponentInfo_GetDescription(This,description)	\
    (This)->lpVtbl -> GetDescription(This,description)

#define INapComponentInfo_GetVendorName(This,vendorName)	\
    (This)->lpVtbl -> GetVendorName(This,vendorName)

#define INapComponentInfo_GetVersion(This,version)	\
    (This)->lpVtbl -> GetVersion(This,version)

#define INapComponentInfo_GetIcon(This,dllFilePath,iconResourceId)	\
    (This)->lpVtbl -> GetIcon(This,dllFilePath,iconResourceId)

#define INapComponentInfo_ConvertErrorCodeToMessageId(This,errorCode,msgId)	\
    (This)->lpVtbl -> ConvertErrorCodeToMessageId(This,errorCode,msgId)

#define INapComponentInfo_GetLocalizedString(This,msgId,string)	\
    (This)->lpVtbl -> GetLocalizedString(This,msgId,string)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INapComponentInfo_GetFriendlyName_Proxy( 
    INapComponentInfo * This,
    /* [out] */ MessageId *friendlyName);


void __RPC_STUB INapComponentInfo_GetFriendlyName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapComponentInfo_GetDescription_Proxy( 
    INapComponentInfo * This,
    /* [out] */ MessageId *description);


void __RPC_STUB INapComponentInfo_GetDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapComponentInfo_GetVendorName_Proxy( 
    INapComponentInfo * This,
    /* [out] */ MessageId *vendorName);


void __RPC_STUB INapComponentInfo_GetVendorName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapComponentInfo_GetVersion_Proxy( 
    INapComponentInfo * This,
    /* [out] */ MessageId *version);


void __RPC_STUB INapComponentInfo_GetVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapComponentInfo_GetIcon_Proxy( 
    INapComponentInfo * This,
    /* [out] */ CountedString **dllFilePath,
    /* [out] */ UINT32 *iconResourceId);


void __RPC_STUB INapComponentInfo_GetIcon_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapComponentInfo_ConvertErrorCodeToMessageId_Proxy( 
    INapComponentInfo * This,
    /* [in] */ HRESULT errorCode,
    /* [out] */ MessageId *msgId);


void __RPC_STUB INapComponentInfo_ConvertErrorCodeToMessageId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapComponentInfo_GetLocalizedString_Proxy( 
    INapComponentInfo * This,
    /* [in] */ MessageId msgId,
    /* [out] */ CountedString **string);


void __RPC_STUB INapComponentInfo_GetLocalizedString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INapComponentInfo_INTERFACE_DEFINED__ */


#ifndef __INapComponentConfig_INTERFACE_DEFINED__
#define __INapComponentConfig_INTERFACE_DEFINED__

/* interface INapComponentConfig */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_INapComponentConfig;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("a9e0af1a-3713-478e-ae03-8edd272d21fa")
    INapComponentConfig : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE IsUISupported( 
            /* [out] */ BOOL *isSupported) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InvokeUI( 
            /* [unique][in] */ HWND hwndParent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetConfig( 
            /* [out] */ UINT16 *bCount,
            /* [size_is][size_is][out] */ BYTE **data) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetConfig( 
            /* [in] */ UINT16 bCount,
            /* [size_is][in] */ BYTE *data) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INapComponentConfigVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INapComponentConfig * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INapComponentConfig * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INapComponentConfig * This);
        
        HRESULT ( STDMETHODCALLTYPE *IsUISupported )( 
            INapComponentConfig * This,
            /* [out] */ BOOL *isSupported);
        
        HRESULT ( STDMETHODCALLTYPE *InvokeUI )( 
            INapComponentConfig * This,
            /* [unique][in] */ HWND hwndParent);
        
        HRESULT ( STDMETHODCALLTYPE *GetConfig )( 
            INapComponentConfig * This,
            /* [out] */ UINT16 *bCount,
            /* [size_is][size_is][out] */ BYTE **data);
        
        HRESULT ( STDMETHODCALLTYPE *SetConfig )( 
            INapComponentConfig * This,
            /* [in] */ UINT16 bCount,
            /* [size_is][in] */ BYTE *data);
        
        END_INTERFACE
    } INapComponentConfigVtbl;

    interface INapComponentConfig
    {
        CONST_VTBL struct INapComponentConfigVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INapComponentConfig_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INapComponentConfig_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INapComponentConfig_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INapComponentConfig_IsUISupported(This,isSupported)	\
    (This)->lpVtbl -> IsUISupported(This,isSupported)

#define INapComponentConfig_InvokeUI(This,hwndParent)	\
    (This)->lpVtbl -> InvokeUI(This,hwndParent)

#define INapComponentConfig_GetConfig(This,bCount,data)	\
    (This)->lpVtbl -> GetConfig(This,bCount,data)

#define INapComponentConfig_SetConfig(This,bCount,data)	\
    (This)->lpVtbl -> SetConfig(This,bCount,data)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INapComponentConfig_IsUISupported_Proxy( 
    INapComponentConfig * This,
    /* [out] */ BOOL *isSupported);


void __RPC_STUB INapComponentConfig_IsUISupported_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapComponentConfig_InvokeUI_Proxy( 
    INapComponentConfig * This,
    /* [unique][in] */ HWND hwndParent);


void __RPC_STUB INapComponentConfig_InvokeUI_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapComponentConfig_GetConfig_Proxy( 
    INapComponentConfig * This,
    /* [out] */ UINT16 *bCount,
    /* [size_is][size_is][out] */ BYTE **data);


void __RPC_STUB INapComponentConfig_GetConfig_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapComponentConfig_SetConfig_Proxy( 
    INapComponentConfig * This,
    /* [in] */ UINT16 bCount,
    /* [size_is][in] */ BYTE *data);


void __RPC_STUB INapComponentConfig_SetConfig_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INapComponentConfig_INTERFACE_DEFINED__ */


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


