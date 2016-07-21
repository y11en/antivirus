
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Oct 29 14:58:43 2007
 */
/* Compiler settings for O:\CS AdminKit\development2\nap\nap_sdk\include\napsystemhealthvalidator.idl:
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

#ifndef __napsystemhealthvalidator_h__
#define __napsystemhealthvalidator_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __INapSystemHealthValidator_FWD_DEFINED__
#define __INapSystemHealthValidator_FWD_DEFINED__
typedef interface INapSystemHealthValidator INapSystemHealthValidator;
#endif 	/* __INapSystemHealthValidator_FWD_DEFINED__ */


#ifndef __INapServerCallback_FWD_DEFINED__
#define __INapServerCallback_FWD_DEFINED__
typedef interface INapServerCallback INapServerCallback;
#endif 	/* __INapServerCallback_FWD_DEFINED__ */


#ifndef __INapSystemHealthValidationRequest_FWD_DEFINED__
#define __INapSystemHealthValidationRequest_FWD_DEFINED__
typedef interface INapSystemHealthValidationRequest INapSystemHealthValidationRequest;
#endif 	/* __INapSystemHealthValidationRequest_FWD_DEFINED__ */


/* header files for imported files */
#include "NapTypes.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_napsystemhealthvalidator_0000 */
/* [local] */ 





extern RPC_IF_HANDLE __MIDL_itf_napsystemhealthvalidator_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_napsystemhealthvalidator_0000_v0_0_s_ifspec;

#ifndef __INapSystemHealthValidator_INTERFACE_DEFINED__
#define __INapSystemHealthValidator_INTERFACE_DEFINED__

/* interface INapSystemHealthValidator */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_INapSystemHealthValidator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EF43D87C-5B6D-4820-8620-868907FE07D3")
    INapSystemHealthValidator : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Validate( 
            /* [in] */ INapSystemHealthValidationRequest *request,
            /* [in] */ UINT32 hintTimeOutInMsec,
            /* [in] */ INapServerCallback *callback) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INapSystemHealthValidatorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INapSystemHealthValidator * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INapSystemHealthValidator * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INapSystemHealthValidator * This);
        
        HRESULT ( STDMETHODCALLTYPE *Validate )( 
            INapSystemHealthValidator * This,
            /* [in] */ INapSystemHealthValidationRequest *request,
            /* [in] */ UINT32 hintTimeOutInMsec,
            /* [in] */ INapServerCallback *callback);
        
        END_INTERFACE
    } INapSystemHealthValidatorVtbl;

    interface INapSystemHealthValidator
    {
        CONST_VTBL struct INapSystemHealthValidatorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INapSystemHealthValidator_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INapSystemHealthValidator_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INapSystemHealthValidator_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INapSystemHealthValidator_Validate(This,request,hintTimeOutInMsec,callback)	\
    (This)->lpVtbl -> Validate(This,request,hintTimeOutInMsec,callback)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INapSystemHealthValidator_Validate_Proxy( 
    INapSystemHealthValidator * This,
    /* [in] */ INapSystemHealthValidationRequest *request,
    /* [in] */ UINT32 hintTimeOutInMsec,
    /* [in] */ INapServerCallback *callback);


void __RPC_STUB INapSystemHealthValidator_Validate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INapSystemHealthValidator_INTERFACE_DEFINED__ */


#ifndef __INapServerCallback_INTERFACE_DEFINED__
#define __INapServerCallback_INTERFACE_DEFINED__

/* interface INapServerCallback */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_INapServerCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("09C20568-F30C-489b-AE9C-4930AD7F165F")
    INapServerCallback : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnComplete( 
            /* [in] */ INapSystemHealthValidationRequest *request,
            /* [in] */ HRESULT errorCode) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INapServerCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INapServerCallback * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INapServerCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INapServerCallback * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnComplete )( 
            INapServerCallback * This,
            /* [in] */ INapSystemHealthValidationRequest *request,
            /* [in] */ HRESULT errorCode);
        
        END_INTERFACE
    } INapServerCallbackVtbl;

    interface INapServerCallback
    {
        CONST_VTBL struct INapServerCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INapServerCallback_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INapServerCallback_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INapServerCallback_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INapServerCallback_OnComplete(This,request,errorCode)	\
    (This)->lpVtbl -> OnComplete(This,request,errorCode)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INapServerCallback_OnComplete_Proxy( 
    INapServerCallback * This,
    /* [in] */ INapSystemHealthValidationRequest *request,
    /* [in] */ HRESULT errorCode);


void __RPC_STUB INapServerCallback_OnComplete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INapServerCallback_INTERFACE_DEFINED__ */


#ifndef __INapSystemHealthValidationRequest_INTERFACE_DEFINED__
#define __INapSystemHealthValidationRequest_INTERFACE_DEFINED__

/* interface INapSystemHealthValidationRequest */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_INapSystemHealthValidationRequest;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ADACB3A0-D4F4-4f17-8933-51D60FCCA606")
    INapSystemHealthValidationRequest : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCorrelationId( 
            /* [out] */ CorrelationId *correlationId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetStringCorrelationId( 
            /* [out] */ StringCorrelationId **correlationId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMachineName( 
            /* [out] */ CountedString **machineName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSoHRequest( 
            /* [out] */ SoHRequest **sohRequest,
            /* [out] */ BOOL *napSystemGenerated) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSoHResponse( 
            /* [in] */ const SoHResponse *sohResponse) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSoHResponse( 
            /* [out] */ SoHResponse **sohResponse) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPrivateData( 
            /* [in] */ const PrivateData *privateData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPrivateData( 
            /* [out] */ PrivateData **privateData) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INapSystemHealthValidationRequestVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INapSystemHealthValidationRequest * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INapSystemHealthValidationRequest * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INapSystemHealthValidationRequest * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCorrelationId )( 
            INapSystemHealthValidationRequest * This,
            /* [out] */ CorrelationId *correlationId);
        
        HRESULT ( STDMETHODCALLTYPE *GetStringCorrelationId )( 
            INapSystemHealthValidationRequest * This,
            /* [out] */ StringCorrelationId **correlationId);
        
        HRESULT ( STDMETHODCALLTYPE *GetMachineName )( 
            INapSystemHealthValidationRequest * This,
            /* [out] */ CountedString **machineName);
        
        HRESULT ( STDMETHODCALLTYPE *GetSoHRequest )( 
            INapSystemHealthValidationRequest * This,
            /* [out] */ SoHRequest **sohRequest,
            /* [out] */ BOOL *napSystemGenerated);
        
        HRESULT ( STDMETHODCALLTYPE *SetSoHResponse )( 
            INapSystemHealthValidationRequest * This,
            /* [in] */ const SoHResponse *sohResponse);
        
        HRESULT ( STDMETHODCALLTYPE *GetSoHResponse )( 
            INapSystemHealthValidationRequest * This,
            /* [out] */ SoHResponse **sohResponse);
        
        HRESULT ( STDMETHODCALLTYPE *SetPrivateData )( 
            INapSystemHealthValidationRequest * This,
            /* [in] */ const PrivateData *privateData);
        
        HRESULT ( STDMETHODCALLTYPE *GetPrivateData )( 
            INapSystemHealthValidationRequest * This,
            /* [out] */ PrivateData **privateData);
        
        END_INTERFACE
    } INapSystemHealthValidationRequestVtbl;

    interface INapSystemHealthValidationRequest
    {
        CONST_VTBL struct INapSystemHealthValidationRequestVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INapSystemHealthValidationRequest_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INapSystemHealthValidationRequest_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INapSystemHealthValidationRequest_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INapSystemHealthValidationRequest_GetCorrelationId(This,correlationId)	\
    (This)->lpVtbl -> GetCorrelationId(This,correlationId)

#define INapSystemHealthValidationRequest_GetStringCorrelationId(This,correlationId)	\
    (This)->lpVtbl -> GetStringCorrelationId(This,correlationId)

#define INapSystemHealthValidationRequest_GetMachineName(This,machineName)	\
    (This)->lpVtbl -> GetMachineName(This,machineName)

#define INapSystemHealthValidationRequest_GetSoHRequest(This,sohRequest,napSystemGenerated)	\
    (This)->lpVtbl -> GetSoHRequest(This,sohRequest,napSystemGenerated)

#define INapSystemHealthValidationRequest_SetSoHResponse(This,sohResponse)	\
    (This)->lpVtbl -> SetSoHResponse(This,sohResponse)

#define INapSystemHealthValidationRequest_GetSoHResponse(This,sohResponse)	\
    (This)->lpVtbl -> GetSoHResponse(This,sohResponse)

#define INapSystemHealthValidationRequest_SetPrivateData(This,privateData)	\
    (This)->lpVtbl -> SetPrivateData(This,privateData)

#define INapSystemHealthValidationRequest_GetPrivateData(This,privateData)	\
    (This)->lpVtbl -> GetPrivateData(This,privateData)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INapSystemHealthValidationRequest_GetCorrelationId_Proxy( 
    INapSystemHealthValidationRequest * This,
    /* [out] */ CorrelationId *correlationId);


void __RPC_STUB INapSystemHealthValidationRequest_GetCorrelationId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSystemHealthValidationRequest_GetStringCorrelationId_Proxy( 
    INapSystemHealthValidationRequest * This,
    /* [out] */ StringCorrelationId **correlationId);


void __RPC_STUB INapSystemHealthValidationRequest_GetStringCorrelationId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSystemHealthValidationRequest_GetMachineName_Proxy( 
    INapSystemHealthValidationRequest * This,
    /* [out] */ CountedString **machineName);


void __RPC_STUB INapSystemHealthValidationRequest_GetMachineName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSystemHealthValidationRequest_GetSoHRequest_Proxy( 
    INapSystemHealthValidationRequest * This,
    /* [out] */ SoHRequest **sohRequest,
    /* [out] */ BOOL *napSystemGenerated);


void __RPC_STUB INapSystemHealthValidationRequest_GetSoHRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSystemHealthValidationRequest_SetSoHResponse_Proxy( 
    INapSystemHealthValidationRequest * This,
    /* [in] */ const SoHResponse *sohResponse);


void __RPC_STUB INapSystemHealthValidationRequest_SetSoHResponse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSystemHealthValidationRequest_GetSoHResponse_Proxy( 
    INapSystemHealthValidationRequest * This,
    /* [out] */ SoHResponse **sohResponse);


void __RPC_STUB INapSystemHealthValidationRequest_GetSoHResponse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSystemHealthValidationRequest_SetPrivateData_Proxy( 
    INapSystemHealthValidationRequest * This,
    /* [in] */ const PrivateData *privateData);


void __RPC_STUB INapSystemHealthValidationRequest_SetPrivateData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSystemHealthValidationRequest_GetPrivateData_Proxy( 
    INapSystemHealthValidationRequest * This,
    /* [out] */ PrivateData **privateData);


void __RPC_STUB INapSystemHealthValidationRequest_GetPrivateData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INapSystemHealthValidationRequest_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


