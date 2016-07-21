
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


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

#if !defined(_M_IA64) && !defined(_M_AMD64)

/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 440
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "napsystemhealthvalidator.h"

#define TYPE_FORMAT_STRING_SIZE   265                               
#define PROC_FORMAT_STRING_SIZE   67                                
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO INapSystemHealthValidator_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapSystemHealthValidator_ProxyInfo;


HRESULT STDMETHODCALLTYPE INapSystemHealthValidator_Validate_Proxy( 
    INapSystemHealthValidator * This,
    /* [in] */ INapSystemHealthValidationRequest *request,
    /* [in] */ UINT32 hintTimeOutInMsec,
    /* [in] */ INapServerCallback *callback)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      3);
        
        
        
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 8;
            NdrInterfacePointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                           (unsigned char *)request,
                                           (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[2] );
            
            NdrInterfacePointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                           (unsigned char *)callback,
                                           (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[20] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrInterfacePointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                         (unsigned char *)request,
                                         (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[2] );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *(( UINT32 * )_StubMsg.Buffer)++ = hintTimeOutInMsec;
            
            NdrInterfacePointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                         (unsigned char *)callback,
                                         (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[20] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[0] );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            
            if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            _RetVal = *(( HRESULT * )_StubMsg.Buffer)++;
            
            }
        RpcFinally
            {
            NdrProxyFreeBuffer(This, &_StubMsg);
            
            }
        RpcEndFinally
        
        }
    RpcExcept(_StubMsg.dwStubPhase != PROXY_SENDRECEIVE)
        {
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapSystemHealthValidator_Validate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    INapServerCallback *callback;
    UINT32 hintTimeOutInMsec;
    INapSystemHealthValidationRequest *request;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    request = 0;
    hintTimeOutInMsec = 0;
    callback = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[0] );
        
        NdrInterfacePointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char * *)&request,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[2],
                                       (unsigned char)0 );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        
        if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        hintTimeOutInMsec = *(( UINT32 * )_StubMsg.Buffer)++;
        
        NdrInterfacePointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char * *)&callback,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[20],
                                       (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSystemHealthValidator*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> Validate(
            (INapSystemHealthValidator *) ((CStdStubBuffer *)This)->pvServerObject,
            request,
            hintTimeOutInMsec,
            callback);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrInterfacePointerFree( &_StubMsg,
                                 (unsigned char *)request,
                                 &__MIDL_TypeFormatString.Format[2] );
        
        NdrInterfacePointerFree( &_StubMsg,
                                 (unsigned char *)callback,
                                 &__MIDL_TypeFormatString.Format[20] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO INapServerCallback_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapServerCallback_ProxyInfo;


HRESULT STDMETHODCALLTYPE INapServerCallback_OnComplete_Proxy( 
    INapServerCallback * This,
    /* [in] */ INapSystemHealthValidationRequest *request,
    /* [in] */ HRESULT errorCode)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      3);
        
        
        
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 8;
            NdrInterfacePointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                           (unsigned char *)request,
                                           (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[2] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrInterfacePointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                         (unsigned char *)request,
                                         (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[2] );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *(( HRESULT * )_StubMsg.Buffer)++ = errorCode;
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[12] );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            
            if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            _RetVal = *(( HRESULT * )_StubMsg.Buffer)++;
            
            }
        RpcFinally
            {
            NdrProxyFreeBuffer(This, &_StubMsg);
            
            }
        RpcEndFinally
        
        }
    RpcExcept(_StubMsg.dwStubPhase != PROXY_SENDRECEIVE)
        {
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapServerCallback_OnComplete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    HRESULT errorCode;
    INapSystemHealthValidationRequest *request;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    request = 0;
    errorCode = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[12] );
        
        NdrInterfacePointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char * *)&request,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[2],
                                       (unsigned char)0 );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        
        if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        errorCode = *(( HRESULT * )_StubMsg.Buffer)++;
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapServerCallback*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> OnComplete(
              (INapServerCallback *) ((CStdStubBuffer *)This)->pvServerObject,
              request,
              errorCode);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrInterfacePointerFree( &_StubMsg,
                                 (unsigned char *)request,
                                 &__MIDL_TypeFormatString.Format[2] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO INapSystemHealthValidationRequest_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapSystemHealthValidationRequest_ProxyInfo;


HRESULT STDMETHODCALLTYPE INapSystemHealthValidationRequest_GetCorrelationId_Proxy( 
    INapSystemHealthValidationRequest * This,
    /* [out] */ CorrelationId *correlationId)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    if(correlationId)
        {
        MIDL_memset(
               correlationId,
               0,
               sizeof( CorrelationId  ));
        }
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      3);
        
        
        
        if(!correlationId)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[20] );
            
            NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char * *)&correlationId,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[68],
                                       (unsigned char)0 );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            
            if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            _RetVal = *(( HRESULT * )_StubMsg.Buffer)++;
            
            }
        RpcFinally
            {
            NdrProxyFreeBuffer(This, &_StubMsg);
            
            }
        RpcEndFinally
        
        }
    RpcExcept(_StubMsg.dwStubPhase != PROXY_SENDRECEIVE)
        {
        NdrClearOutParameters(
                         ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[38],
                         ( void * )correlationId);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapSystemHealthValidationRequest_GetCorrelationId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    CorrelationId _correlationIdM;
    CorrelationId *correlationId;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( CorrelationId * )correlationId = 0;
    RpcTryFinally
        {
        correlationId = &_correlationIdM;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSystemHealthValidationRequest*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetCorrelationId((INapSystemHealthValidationRequest *) ((CStdStubBuffer *)This)->pvServerObject,correlationId);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 116;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                 (unsigned char *)correlationId,
                                 (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[68] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapSystemHealthValidationRequest_GetStringCorrelationId_Proxy( 
    INapSystemHealthValidationRequest * This,
    /* [out] */ StringCorrelationId **correlationId)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    if(correlationId)
        {
        *correlationId = 0;
        }
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      4);
        
        
        
        if(!correlationId)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[26] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&correlationId,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[82],
                                  (unsigned char)0 );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            
            if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            _RetVal = *(( HRESULT * )_StubMsg.Buffer)++;
            
            }
        RpcFinally
            {
            NdrProxyFreeBuffer(This, &_StubMsg);
            
            }
        RpcEndFinally
        
        }
    RpcExcept(_StubMsg.dwStubPhase != PROXY_SENDRECEIVE)
        {
        NdrClearOutParameters(
                         ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[82],
                         ( void * )correlationId);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapSystemHealthValidationRequest_GetStringCorrelationId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    StringCorrelationId *_M0;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    StringCorrelationId **correlationId;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( StringCorrelationId ** )correlationId = 0;
    RpcTryFinally
        {
        correlationId = &_M0;
        _M0 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSystemHealthValidationRequest*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetStringCorrelationId((INapSystemHealthValidationRequest *) ((CStdStubBuffer *)This)->pvServerObject,correlationId);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)correlationId,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[82] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)correlationId,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[82] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)correlationId,
                        &__MIDL_TypeFormatString.Format[82] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapSystemHealthValidationRequest_GetMachineName_Proxy( 
    INapSystemHealthValidationRequest * This,
    /* [out] */ CountedString **machineName)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    if(machineName)
        {
        *machineName = 0;
        }
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      5);
        
        
        
        if(!machineName)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[26] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&machineName,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[82],
                                  (unsigned char)0 );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            
            if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            _RetVal = *(( HRESULT * )_StubMsg.Buffer)++;
            
            }
        RpcFinally
            {
            NdrProxyFreeBuffer(This, &_StubMsg);
            
            }
        RpcEndFinally
        
        }
    RpcExcept(_StubMsg.dwStubPhase != PROXY_SENDRECEIVE)
        {
        NdrClearOutParameters(
                         ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[82],
                         ( void * )machineName);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapSystemHealthValidationRequest_GetMachineName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    CountedString *_M1;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    CountedString **machineName;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( CountedString ** )machineName = 0;
    RpcTryFinally
        {
        machineName = &_M1;
        _M1 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSystemHealthValidationRequest*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetMachineName((INapSystemHealthValidationRequest *) ((CStdStubBuffer *)This)->pvServerObject,machineName);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)machineName,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[82] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)machineName,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[82] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)machineName,
                        &__MIDL_TypeFormatString.Format[82] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapSystemHealthValidationRequest_GetSoHRequest_Proxy( 
    INapSystemHealthValidationRequest * This,
    /* [out] */ SoHRequest **sohRequest,
    /* [out] */ BOOL *napSystemGenerated)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    if(sohRequest)
        {
        *sohRequest = 0;
        }
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      6);
        
        
        
        if(!sohRequest)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        if(!napSystemGenerated)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[32] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&sohRequest,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[118],
                                  (unsigned char)0 );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *napSystemGenerated = *(( BOOL * )_StubMsg.Buffer)++;
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            
            if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            _RetVal = *(( HRESULT * )_StubMsg.Buffer)++;
            
            }
        RpcFinally
            {
            NdrProxyFreeBuffer(This, &_StubMsg);
            
            }
        RpcEndFinally
        
        }
    RpcExcept(_StubMsg.dwStubPhase != PROXY_SENDRECEIVE)
        {
        NdrClearOutParameters(
                         ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[118],
                         ( void * )sohRequest);
        NdrClearOutParameters(
                         ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[212],
                         ( void * )napSystemGenerated);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapSystemHealthValidationRequest_GetSoHRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    SoHRequest *_M2;
    BOOL _M3	=	0;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    BOOL *napSystemGenerated;
    SoHRequest **sohRequest;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( SoHRequest ** )sohRequest = 0;
    ( BOOL * )napSystemGenerated = 0;
    RpcTryFinally
        {
        sohRequest = &_M2;
        _M2 = 0;
        napSystemGenerated = &_M3;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSystemHealthValidationRequest*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetSoHRequest(
                 (INapSystemHealthValidationRequest *) ((CStdStubBuffer *)This)->pvServerObject,
                 sohRequest,
                 napSystemGenerated);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 36;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)sohRequest,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[118] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)sohRequest,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[118] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( BOOL * )_StubMsg.Buffer)++ = *napSystemGenerated;
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)sohRequest,
                        &__MIDL_TypeFormatString.Format[118] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapSystemHealthValidationRequest_SetSoHResponse_Proxy( 
    INapSystemHealthValidationRequest * This,
    /* [in] */ const SoHResponse *sohResponse)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      7);
        
        
        
        if(!sohResponse)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrSimpleStructBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char *)sohResponse,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[190] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                     (unsigned char *)sohResponse,
                                     (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[190] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[42] );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            
            if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            _RetVal = *(( HRESULT * )_StubMsg.Buffer)++;
            
            }
        RpcFinally
            {
            NdrProxyFreeBuffer(This, &_StubMsg);
            
            }
        RpcEndFinally
        
        }
    RpcExcept(_StubMsg.dwStubPhase != PROXY_SENDRECEIVE)
        {
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapSystemHealthValidationRequest_SetSoHResponse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    const SoHResponse *sohResponse;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( SoHResponse * )sohResponse = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[42] );
        
        NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char * *)&sohResponse,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[190],
                                   (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSystemHealthValidationRequest*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> SetSoHResponse((INapSystemHealthValidationRequest *) ((CStdStubBuffer *)This)->pvServerObject,sohResponse);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)sohResponse,
                        &__MIDL_TypeFormatString.Format[216] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapSystemHealthValidationRequest_GetSoHResponse_Proxy( 
    INapSystemHealthValidationRequest * This,
    /* [out] */ SoHResponse **sohResponse)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    if(sohResponse)
        {
        *sohResponse = 0;
        }
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      8);
        
        
        
        if(!sohResponse)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[48] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&sohResponse,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[118],
                                  (unsigned char)0 );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            
            if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            _RetVal = *(( HRESULT * )_StubMsg.Buffer)++;
            
            }
        RpcFinally
            {
            NdrProxyFreeBuffer(This, &_StubMsg);
            
            }
        RpcEndFinally
        
        }
    RpcExcept(_StubMsg.dwStubPhase != PROXY_SENDRECEIVE)
        {
        NdrClearOutParameters(
                         ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[118],
                         ( void * )sohResponse);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapSystemHealthValidationRequest_GetSoHResponse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    SoHResponse *_M4;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    SoHResponse **sohResponse;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( SoHResponse ** )sohResponse = 0;
    RpcTryFinally
        {
        sohResponse = &_M4;
        _M4 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSystemHealthValidationRequest*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetSoHResponse((INapSystemHealthValidationRequest *) ((CStdStubBuffer *)This)->pvServerObject,sohResponse);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)sohResponse,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[118] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)sohResponse,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[118] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)sohResponse,
                        &__MIDL_TypeFormatString.Format[118] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapSystemHealthValidationRequest_SetPrivateData_Proxy( 
    INapSystemHealthValidationRequest * This,
    /* [in] */ const PrivateData *privateData)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      9);
        
        
        
        if(!privateData)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrSimpleStructBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char *)privateData,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[234] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                     (unsigned char *)privateData,
                                     (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[234] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[54] );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            
            if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            _RetVal = *(( HRESULT * )_StubMsg.Buffer)++;
            
            }
        RpcFinally
            {
            NdrProxyFreeBuffer(This, &_StubMsg);
            
            }
        RpcEndFinally
        
        }
    RpcExcept(_StubMsg.dwStubPhase != PROXY_SENDRECEIVE)
        {
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapSystemHealthValidationRequest_SetPrivateData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    const PrivateData *privateData;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( PrivateData * )privateData = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[54] );
        
        NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char * *)&privateData,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[234],
                                   (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSystemHealthValidationRequest*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> SetPrivateData((INapSystemHealthValidationRequest *) ((CStdStubBuffer *)This)->pvServerObject,privateData);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)privateData,
                        &__MIDL_TypeFormatString.Format[220] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapSystemHealthValidationRequest_GetPrivateData_Proxy( 
    INapSystemHealthValidationRequest * This,
    /* [out] */ PrivateData **privateData)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    if(privateData)
        {
        *privateData = 0;
        }
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      10);
        
        
        
        if(!privateData)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[60] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&privateData,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[256],
                                  (unsigned char)0 );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            
            if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            _RetVal = *(( HRESULT * )_StubMsg.Buffer)++;
            
            }
        RpcFinally
            {
            NdrProxyFreeBuffer(This, &_StubMsg);
            
            }
        RpcEndFinally
        
        }
    RpcExcept(_StubMsg.dwStubPhase != PROXY_SENDRECEIVE)
        {
        NdrClearOutParameters(
                         ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[256],
                         ( void * )privateData);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapSystemHealthValidationRequest_GetPrivateData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    PrivateData *_M5;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    PrivateData **privateData;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( PrivateData ** )privateData = 0;
    RpcTryFinally
        {
        privateData = &_M5;
        _M5 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSystemHealthValidationRequest*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetPrivateData((INapSystemHealthValidationRequest *) ((CStdStubBuffer *)This)->pvServerObject,privateData);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)privateData,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[256] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)privateData,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[256] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)privateData,
                        &__MIDL_TypeFormatString.Format[256] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}



#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {
			
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/*  2 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/*  4 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/*  6 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/*  8 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */
/* 10 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 12 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 14 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 16 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 18 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 20 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 22 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */
/* 24 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 26 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 28 */	NdrFcShort( 0x52 ),	/* Type Offset=82 */
/* 30 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 32 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 34 */	NdrFcShort( 0x76 ),	/* Type Offset=118 */
/* 36 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 38 */	NdrFcShort( 0xd4 ),	/* Type Offset=212 */
/* 40 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 42 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 44 */	NdrFcShort( 0xd8 ),	/* Type Offset=216 */
/* 46 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 48 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 50 */	NdrFcShort( 0x76 ),	/* Type Offset=118 */
/* 52 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 54 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 56 */	NdrFcShort( 0xdc ),	/* Type Offset=220 */
/* 58 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 60 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 62 */	NdrFcShort( 0x100 ),	/* Type Offset=256 */
/* 64 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/*  4 */	NdrFcLong( 0xadacb3a0 ),	/* -1381190752 */
/*  8 */	NdrFcShort( 0xd4f4 ),	/* -11020 */
/* 10 */	NdrFcShort( 0x4f17 ),	/* 20247 */
/* 12 */	0x89,		/* 137 */
			0x33,		/* 51 */
/* 14 */	0x51,		/* 81 */
			0xd6,		/* 214 */
/* 16 */	0xf,		/* 15 */
			0xcc,		/* 204 */
/* 18 */	0xa6,		/* 166 */
			0x6,		/* 6 */
/* 20 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 22 */	NdrFcLong( 0x9c20568 ),	/* 163710312 */
/* 26 */	NdrFcShort( 0xf30c ),	/* -3316 */
/* 28 */	NdrFcShort( 0x489b ),	/* 18587 */
/* 30 */	0xae,		/* 174 */
			0x9c,		/* 156 */
/* 32 */	0x49,		/* 73 */
			0x30,		/* 48 */
/* 34 */	0xad,		/* 173 */
			0x7f,		/* 127 */
/* 36 */	0x16,		/* 22 */
			0x5f,		/* 95 */
/* 38 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 40 */	NdrFcShort( 0x1c ),	/* Offset= 28 (68) */
/* 42 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 44 */	NdrFcShort( 0x8 ),	/* 8 */
/* 46 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 48 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 50 */	NdrFcShort( 0x10 ),	/* 16 */
/* 52 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 54 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 56 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (42) */
			0x5b,		/* FC_END */
/* 60 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 62 */	NdrFcShort( 0x8 ),	/* 8 */
/* 64 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 66 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 68 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 70 */	NdrFcShort( 0x18 ),	/* 24 */
/* 72 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 74 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (48) */
/* 76 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 78 */	NdrFcShort( 0xffffffee ),	/* Offset= -18 (60) */
/* 80 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 82 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 84 */	NdrFcShort( 0x2 ),	/* Offset= 2 (86) */
/* 86 */	
			0x13, 0x0,	/* FC_OP */
/* 88 */	NdrFcShort( 0x8 ),	/* Offset= 8 (96) */
/* 90 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 92 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x57,		/* FC_ADD_1 */
/* 94 */	NdrFcShort( 0x0 ),	/* 0 */
/* 96 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 98 */	NdrFcShort( 0x8 ),	/* 8 */
/* 100 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 102 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 104 */	NdrFcShort( 0x4 ),	/* 4 */
/* 106 */	NdrFcShort( 0x4 ),	/* 4 */
/* 108 */	0x13, 0x0,	/* FC_OP */
/* 110 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (90) */
/* 112 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 114 */	0x3e,		/* FC_STRUCTPAD2 */
			0x8,		/* FC_LONG */
/* 116 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 118 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 120 */	NdrFcShort( 0x2 ),	/* Offset= 2 (122) */
/* 122 */	
			0x13, 0x0,	/* FC_OP */
/* 124 */	NdrFcShort( 0x42 ),	/* Offset= 66 (190) */
/* 126 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 128 */	NdrFcShort( 0x1 ),	/* 1 */
/* 130 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 132 */	NdrFcShort( 0x2 ),	/* 2 */
/* 134 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 136 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 138 */	NdrFcShort( 0x8 ),	/* 8 */
/* 140 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 142 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 144 */	NdrFcShort( 0x4 ),	/* 4 */
/* 146 */	NdrFcShort( 0x4 ),	/* 4 */
/* 148 */	0x13, 0x0,	/* FC_OP */
/* 150 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (126) */
/* 152 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 154 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 156 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 158 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 160 */	NdrFcShort( 0x8 ),	/* 8 */
/* 162 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 164 */	NdrFcShort( 0x0 ),	/* 0 */
/* 166 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 168 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 170 */	NdrFcShort( 0x8 ),	/* 8 */
/* 172 */	NdrFcShort( 0x0 ),	/* 0 */
/* 174 */	NdrFcShort( 0x1 ),	/* 1 */
/* 176 */	NdrFcShort( 0x4 ),	/* 4 */
/* 178 */	NdrFcShort( 0x4 ),	/* 4 */
/* 180 */	0x13, 0x0,	/* FC_OP */
/* 182 */	NdrFcShort( 0xffffffc8 ),	/* Offset= -56 (126) */
/* 184 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 186 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffcd ),	/* Offset= -51 (136) */
			0x5b,		/* FC_END */
/* 190 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 192 */	NdrFcShort( 0x8 ),	/* 8 */
/* 194 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 196 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 198 */	NdrFcShort( 0x4 ),	/* 4 */
/* 200 */	NdrFcShort( 0x4 ),	/* 4 */
/* 202 */	0x13, 0x0,	/* FC_OP */
/* 204 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (158) */
/* 206 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 208 */	0x3e,		/* FC_STRUCTPAD2 */
			0x8,		/* FC_LONG */
/* 210 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 212 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 214 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 216 */	
			0x11, 0x0,	/* FC_RP */
/* 218 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (190) */
/* 220 */	
			0x11, 0x0,	/* FC_RP */
/* 222 */	NdrFcShort( 0xc ),	/* Offset= 12 (234) */
/* 224 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 226 */	NdrFcShort( 0x1 ),	/* 1 */
/* 228 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 230 */	NdrFcShort( 0x0 ),	/* 0 */
/* 232 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 234 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 236 */	NdrFcShort( 0x8 ),	/* 8 */
/* 238 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 240 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 242 */	NdrFcShort( 0x4 ),	/* 4 */
/* 244 */	NdrFcShort( 0x4 ),	/* 4 */
/* 246 */	0x12, 0x0,	/* FC_UP */
/* 248 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (224) */
/* 250 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 252 */	0x3e,		/* FC_STRUCTPAD2 */
			0x8,		/* FC_LONG */
/* 254 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 256 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 258 */	NdrFcShort( 0x2 ),	/* Offset= 2 (260) */
/* 260 */	
			0x13, 0x0,	/* FC_OP */
/* 262 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (234) */

			0x0
        }
    };


/* Standard interface: __MIDL_itf_napsystemhealthvalidator_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: INapSystemHealthValidator, ver. 0.0,
   GUID={0xEF43D87C,0x5B6D,0x4820,{0x86,0x20,0x86,0x89,0x07,0xFE,0x07,0xD3}} */

#pragma code_seg(".orpc")
static const unsigned short INapSystemHealthValidator_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO INapSystemHealthValidator_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapSystemHealthValidator_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapSystemHealthValidator_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapSystemHealthValidator_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
const CINTERFACE_PROXY_VTABLE(4) _INapSystemHealthValidatorProxyVtbl = 
{
    &IID_INapSystemHealthValidator,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    INapSystemHealthValidator_Validate_Proxy
};


static const PRPC_STUB_FUNCTION INapSystemHealthValidator_table[] =
{
    INapSystemHealthValidator_Validate_Stub
};

const CInterfaceStubVtbl _INapSystemHealthValidatorStubVtbl =
{
    &IID_INapSystemHealthValidator,
    &INapSystemHealthValidator_ServerInfo,
    4,
    &INapSystemHealthValidator_table[-3],
    CStdStubBuffer_METHODS
};


/* Object interface: INapServerCallback, ver. 0.0,
   GUID={0x09C20568,0xF30C,0x489b,{0xAE,0x9C,0x49,0x30,0xAD,0x7F,0x16,0x5F}} */

#pragma code_seg(".orpc")
static const unsigned short INapServerCallback_FormatStringOffsetTable[] =
    {
    12
    };

static const MIDL_STUBLESS_PROXY_INFO INapServerCallback_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapServerCallback_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapServerCallback_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapServerCallback_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
const CINTERFACE_PROXY_VTABLE(4) _INapServerCallbackProxyVtbl = 
{
    &IID_INapServerCallback,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    INapServerCallback_OnComplete_Proxy
};


static const PRPC_STUB_FUNCTION INapServerCallback_table[] =
{
    INapServerCallback_OnComplete_Stub
};

const CInterfaceStubVtbl _INapServerCallbackStubVtbl =
{
    &IID_INapServerCallback,
    &INapServerCallback_ServerInfo,
    4,
    &INapServerCallback_table[-3],
    CStdStubBuffer_METHODS
};


/* Object interface: INapSystemHealthValidationRequest, ver. 0.0,
   GUID={0xADACB3A0,0xD4F4,0x4f17,{0x89,0x33,0x51,0xD6,0x0F,0xCC,0xA6,0x06}} */

#pragma code_seg(".orpc")
static const unsigned short INapSystemHealthValidationRequest_FormatStringOffsetTable[] =
    {
    20,
    26,
    26,
    32,
    42,
    48,
    54,
    60
    };

static const MIDL_STUBLESS_PROXY_INFO INapSystemHealthValidationRequest_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapSystemHealthValidationRequest_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapSystemHealthValidationRequest_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapSystemHealthValidationRequest_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
const CINTERFACE_PROXY_VTABLE(11) _INapSystemHealthValidationRequestProxyVtbl = 
{
    &IID_INapSystemHealthValidationRequest,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    INapSystemHealthValidationRequest_GetCorrelationId_Proxy ,
    INapSystemHealthValidationRequest_GetStringCorrelationId_Proxy ,
    INapSystemHealthValidationRequest_GetMachineName_Proxy ,
    INapSystemHealthValidationRequest_GetSoHRequest_Proxy ,
    INapSystemHealthValidationRequest_SetSoHResponse_Proxy ,
    INapSystemHealthValidationRequest_GetSoHResponse_Proxy ,
    INapSystemHealthValidationRequest_SetPrivateData_Proxy ,
    INapSystemHealthValidationRequest_GetPrivateData_Proxy
};


static const PRPC_STUB_FUNCTION INapSystemHealthValidationRequest_table[] =
{
    INapSystemHealthValidationRequest_GetCorrelationId_Stub,
    INapSystemHealthValidationRequest_GetStringCorrelationId_Stub,
    INapSystemHealthValidationRequest_GetMachineName_Stub,
    INapSystemHealthValidationRequest_GetSoHRequest_Stub,
    INapSystemHealthValidationRequest_SetSoHResponse_Stub,
    INapSystemHealthValidationRequest_GetSoHResponse_Stub,
    INapSystemHealthValidationRequest_SetPrivateData_Stub,
    INapSystemHealthValidationRequest_GetPrivateData_Stub
};

const CInterfaceStubVtbl _INapSystemHealthValidationRequestStubVtbl =
{
    &IID_INapSystemHealthValidationRequest,
    &INapSystemHealthValidationRequest_ServerInfo,
    11,
    &INapSystemHealthValidationRequest_table[-3],
    CStdStubBuffer_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x10001, /* Ndr library version */
    0,
    0x600015b, /* MIDL Version 6.0.347 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0   /* Reserved5 */
    };

const CInterfaceProxyVtbl * _napsystemhealthvalidator_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_INapServerCallbackProxyVtbl,
    ( CInterfaceProxyVtbl *) &_INapSystemHealthValidatorProxyVtbl,
    ( CInterfaceProxyVtbl *) &_INapSystemHealthValidationRequestProxyVtbl,
    0
};

const CInterfaceStubVtbl * _napsystemhealthvalidator_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_INapServerCallbackStubVtbl,
    ( CInterfaceStubVtbl *) &_INapSystemHealthValidatorStubVtbl,
    ( CInterfaceStubVtbl *) &_INapSystemHealthValidationRequestStubVtbl,
    0
};

PCInterfaceName const _napsystemhealthvalidator_InterfaceNamesList[] = 
{
    "INapServerCallback",
    "INapSystemHealthValidator",
    "INapSystemHealthValidationRequest",
    0
};


#define _napsystemhealthvalidator_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _napsystemhealthvalidator, pIID, n)

int __stdcall _napsystemhealthvalidator_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _napsystemhealthvalidator, 3, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _napsystemhealthvalidator, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _napsystemhealthvalidator, 3, *pIndex )
    
}

const ExtendedProxyFileInfo napsystemhealthvalidator_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _napsystemhealthvalidator_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _napsystemhealthvalidator_StubVtblList,
    (const PCInterfaceName * ) & _napsystemhealthvalidator_InterfaceNamesList,
    0, // no delegation
    & _napsystemhealthvalidator_IID_Lookup, 
    3,
    1,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/


#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Oct 29 14:58:43 2007
 */
/* Compiler settings for O:\CS AdminKit\development2\nap\nap_sdk\include\napsystemhealthvalidator.idl:
    Oicf, W1, Zp8, env=Win64 (32b run,appending)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if defined(_M_IA64) || defined(_M_AMD64)
#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "napsystemhealthvalidator.h"

#define TYPE_FORMAT_STRING_SIZE   293                               
#define PROC_FORMAT_STRING_SIZE   405                               
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO INapSystemHealthValidator_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapSystemHealthValidator_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO INapServerCallback_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapServerCallback_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO INapSystemHealthValidationRequest_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapSystemHealthValidationRequest_ProxyInfo;



#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure Validate */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 10 */	NdrFcShort( 0x8 ),	/* 8 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 16 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter request */

/* 26 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 28 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 30 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter hintTimeOutInMsec */

/* 32 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 34 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 36 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter callback */

/* 38 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 40 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 42 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */

	/* Return value */

/* 44 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 46 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 48 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnComplete */

/* 50 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 52 */	NdrFcLong( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x3 ),	/* 3 */
/* 58 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 60 */	NdrFcShort( 0x8 ),	/* 8 */
/* 62 */	NdrFcShort( 0x8 ),	/* 8 */
/* 64 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 66 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 68 */	NdrFcShort( 0x0 ),	/* 0 */
/* 70 */	NdrFcShort( 0x0 ),	/* 0 */
/* 72 */	NdrFcShort( 0x0 ),	/* 0 */
/* 74 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter request */

/* 76 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 78 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 80 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter errorCode */

/* 82 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 84 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 86 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 88 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 90 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 92 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCorrelationId */

/* 94 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 96 */	NdrFcLong( 0x0 ),	/* 0 */
/* 100 */	NdrFcShort( 0x3 ),	/* 3 */
/* 102 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 104 */	NdrFcShort( 0x0 ),	/* 0 */
/* 106 */	NdrFcShort( 0x74 ),	/* 116 */
/* 108 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 110 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 112 */	NdrFcShort( 0x0 ),	/* 0 */
/* 114 */	NdrFcShort( 0x0 ),	/* 0 */
/* 116 */	NdrFcShort( 0x0 ),	/* 0 */
/* 118 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter correlationId */

/* 120 */	NdrFcShort( 0x6112 ),	/* Flags:  must free, out, simple ref, srv alloc size=24 */
/* 122 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 124 */	NdrFcShort( 0x44 ),	/* Type Offset=68 */

	/* Return value */

/* 126 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 128 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 130 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetStringCorrelationId */

/* 132 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 134 */	NdrFcLong( 0x0 ),	/* 0 */
/* 138 */	NdrFcShort( 0x4 ),	/* 4 */
/* 140 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 142 */	NdrFcShort( 0x0 ),	/* 0 */
/* 144 */	NdrFcShort( 0x8 ),	/* 8 */
/* 146 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 148 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 150 */	NdrFcShort( 0x1 ),	/* 1 */
/* 152 */	NdrFcShort( 0x0 ),	/* 0 */
/* 154 */	NdrFcShort( 0x0 ),	/* 0 */
/* 156 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter correlationId */

/* 158 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 160 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 162 */	NdrFcShort( 0x52 ),	/* Type Offset=82 */

	/* Return value */

/* 164 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 166 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 168 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMachineName */

/* 170 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 172 */	NdrFcLong( 0x0 ),	/* 0 */
/* 176 */	NdrFcShort( 0x5 ),	/* 5 */
/* 178 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 180 */	NdrFcShort( 0x0 ),	/* 0 */
/* 182 */	NdrFcShort( 0x8 ),	/* 8 */
/* 184 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 186 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 188 */	NdrFcShort( 0x1 ),	/* 1 */
/* 190 */	NdrFcShort( 0x0 ),	/* 0 */
/* 192 */	NdrFcShort( 0x0 ),	/* 0 */
/* 194 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter machineName */

/* 196 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 198 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 200 */	NdrFcShort( 0x52 ),	/* Type Offset=82 */

	/* Return value */

/* 202 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 204 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 206 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSoHRequest */

/* 208 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 210 */	NdrFcLong( 0x0 ),	/* 0 */
/* 214 */	NdrFcShort( 0x6 ),	/* 6 */
/* 216 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 218 */	NdrFcShort( 0x0 ),	/* 0 */
/* 220 */	NdrFcShort( 0x24 ),	/* 36 */
/* 222 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 224 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 226 */	NdrFcShort( 0x2 ),	/* 2 */
/* 228 */	NdrFcShort( 0x0 ),	/* 0 */
/* 230 */	NdrFcShort( 0x0 ),	/* 0 */
/* 232 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter sohRequest */

/* 234 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 236 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 238 */	NdrFcShort( 0x80 ),	/* Type Offset=128 */

	/* Parameter napSystemGenerated */

/* 240 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 242 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 244 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 246 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 248 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 250 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSoHResponse */

/* 252 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 254 */	NdrFcLong( 0x0 ),	/* 0 */
/* 258 */	NdrFcShort( 0x7 ),	/* 7 */
/* 260 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 262 */	NdrFcShort( 0x0 ),	/* 0 */
/* 264 */	NdrFcShort( 0x8 ),	/* 8 */
/* 266 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 268 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 270 */	NdrFcShort( 0x0 ),	/* 0 */
/* 272 */	NdrFcShort( 0x2 ),	/* 2 */
/* 274 */	NdrFcShort( 0x0 ),	/* 0 */
/* 276 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter sohResponse */

/* 278 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 280 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 282 */	NdrFcShort( 0xd2 ),	/* Type Offset=210 */

	/* Return value */

/* 284 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 286 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 288 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSoHResponse */

/* 290 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 292 */	NdrFcLong( 0x0 ),	/* 0 */
/* 296 */	NdrFcShort( 0x8 ),	/* 8 */
/* 298 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 300 */	NdrFcShort( 0x0 ),	/* 0 */
/* 302 */	NdrFcShort( 0x8 ),	/* 8 */
/* 304 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 306 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 308 */	NdrFcShort( 0x2 ),	/* 2 */
/* 310 */	NdrFcShort( 0x0 ),	/* 0 */
/* 312 */	NdrFcShort( 0x0 ),	/* 0 */
/* 314 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter sohResponse */

/* 316 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 318 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 320 */	NdrFcShort( 0x80 ),	/* Type Offset=128 */

	/* Return value */

/* 322 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 324 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 326 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetPrivateData */

/* 328 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 330 */	NdrFcLong( 0x0 ),	/* 0 */
/* 334 */	NdrFcShort( 0x9 ),	/* 9 */
/* 336 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 338 */	NdrFcShort( 0x0 ),	/* 0 */
/* 340 */	NdrFcShort( 0x8 ),	/* 8 */
/* 342 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 344 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 346 */	NdrFcShort( 0x0 ),	/* 0 */
/* 348 */	NdrFcShort( 0x1 ),	/* 1 */
/* 350 */	NdrFcShort( 0x0 ),	/* 0 */
/* 352 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter privateData */

/* 354 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 356 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 358 */	NdrFcShort( 0x108 ),	/* Type Offset=264 */

	/* Return value */

/* 360 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 362 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 364 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPrivateData */

/* 366 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 368 */	NdrFcLong( 0x0 ),	/* 0 */
/* 372 */	NdrFcShort( 0xa ),	/* 10 */
/* 374 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 376 */	NdrFcShort( 0x0 ),	/* 0 */
/* 378 */	NdrFcShort( 0x8 ),	/* 8 */
/* 380 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 382 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 384 */	NdrFcShort( 0x1 ),	/* 1 */
/* 386 */	NdrFcShort( 0x0 ),	/* 0 */
/* 388 */	NdrFcShort( 0x0 ),	/* 0 */
/* 390 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter privateData */

/* 392 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 394 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 396 */	NdrFcShort( 0x11c ),	/* Type Offset=284 */

	/* Return value */

/* 398 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 400 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 402 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/*  4 */	NdrFcLong( 0xadacb3a0 ),	/* -1381190752 */
/*  8 */	NdrFcShort( 0xd4f4 ),	/* -11020 */
/* 10 */	NdrFcShort( 0x4f17 ),	/* 20247 */
/* 12 */	0x89,		/* 137 */
			0x33,		/* 51 */
/* 14 */	0x51,		/* 81 */
			0xd6,		/* 214 */
/* 16 */	0xf,		/* 15 */
			0xcc,		/* 204 */
/* 18 */	0xa6,		/* 166 */
			0x6,		/* 6 */
/* 20 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 22 */	NdrFcLong( 0x9c20568 ),	/* 163710312 */
/* 26 */	NdrFcShort( 0xf30c ),	/* -3316 */
/* 28 */	NdrFcShort( 0x489b ),	/* 18587 */
/* 30 */	0xae,		/* 174 */
			0x9c,		/* 156 */
/* 32 */	0x49,		/* 73 */
			0x30,		/* 48 */
/* 34 */	0xad,		/* 173 */
			0x7f,		/* 127 */
/* 36 */	0x16,		/* 22 */
			0x5f,		/* 95 */
/* 38 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 40 */	NdrFcShort( 0x1c ),	/* Offset= 28 (68) */
/* 42 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 44 */	NdrFcShort( 0x8 ),	/* 8 */
/* 46 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 48 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 50 */	NdrFcShort( 0x10 ),	/* 16 */
/* 52 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 54 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 56 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (42) */
			0x5b,		/* FC_END */
/* 60 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 62 */	NdrFcShort( 0x8 ),	/* 8 */
/* 64 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 66 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 68 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 70 */	NdrFcShort( 0x18 ),	/* 24 */
/* 72 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 74 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (48) */
/* 76 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 78 */	NdrFcShort( 0xffffffee ),	/* Offset= -18 (60) */
/* 80 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 82 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 84 */	NdrFcShort( 0x2 ),	/* Offset= 2 (86) */
/* 86 */	
			0x13, 0x0,	/* FC_OP */
/* 88 */	NdrFcShort( 0x14 ),	/* Offset= 20 (108) */
/* 90 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 92 */	NdrFcLong( 0x0 ),	/* 0 */
/* 96 */	NdrFcLong( 0x400 ),	/* 1024 */
/* 100 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 102 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x57,		/* FC_ADD_1 */
/* 104 */	NdrFcShort( 0x0 ),	/* 0 */
/* 106 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 108 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 110 */	NdrFcShort( 0x10 ),	/* 16 */
/* 112 */	NdrFcShort( 0x0 ),	/* 0 */
/* 114 */	NdrFcShort( 0xa ),	/* Offset= 10 (124) */
/* 116 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 118 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (90) */
/* 120 */	0x42,		/* FC_STRUCTPAD6 */
			0x36,		/* FC_POINTER */
/* 122 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 124 */	
			0x13, 0x0,	/* FC_OP */
/* 126 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (100) */
/* 128 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 130 */	NdrFcShort( 0x2 ),	/* Offset= 2 (132) */
/* 132 */	
			0x13, 0x0,	/* FC_OP */
/* 134 */	NdrFcShort( 0x4c ),	/* Offset= 76 (210) */
/* 136 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 138 */	NdrFcLong( 0x0 ),	/* 0 */
/* 142 */	NdrFcLong( 0x64 ),	/* 100 */
/* 146 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 148 */	NdrFcLong( 0x0 ),	/* 0 */
/* 152 */	NdrFcLong( 0xfa0 ),	/* 4000 */
/* 156 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 158 */	NdrFcShort( 0x1 ),	/* 1 */
/* 160 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 162 */	NdrFcShort( 0x2 ),	/* 2 */
/* 164 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 166 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 168 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 170 */	NdrFcShort( 0x10 ),	/* 16 */
/* 172 */	NdrFcShort( 0x0 ),	/* 0 */
/* 174 */	NdrFcShort( 0xa ),	/* Offset= 10 (184) */
/* 176 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 178 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffdf ),	/* Offset= -33 (146) */
			0x40,		/* FC_STRUCTPAD4 */
/* 182 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 184 */	
			0x13, 0x0,	/* FC_OP */
/* 186 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (156) */
/* 188 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 190 */	NdrFcShort( 0x0 ),	/* 0 */
/* 192 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 194 */	NdrFcShort( 0x0 ),	/* 0 */
/* 196 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 198 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 202 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 204 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 206 */	NdrFcShort( 0xffffffda ),	/* Offset= -38 (168) */
/* 208 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 210 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 212 */	NdrFcShort( 0x10 ),	/* 16 */
/* 214 */	NdrFcShort( 0x0 ),	/* 0 */
/* 216 */	NdrFcShort( 0xa ),	/* Offset= 10 (226) */
/* 218 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 220 */	NdrFcShort( 0xffffffac ),	/* Offset= -84 (136) */
/* 222 */	0x42,		/* FC_STRUCTPAD6 */
			0x36,		/* FC_POINTER */
/* 224 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 226 */	
			0x13, 0x0,	/* FC_OP */
/* 228 */	NdrFcShort( 0xffffffd8 ),	/* Offset= -40 (188) */
/* 230 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 232 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 234 */	
			0x11, 0x0,	/* FC_RP */
/* 236 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (210) */
/* 238 */	
			0x11, 0x0,	/* FC_RP */
/* 240 */	NdrFcShort( 0x18 ),	/* Offset= 24 (264) */
/* 242 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 244 */	NdrFcLong( 0x0 ),	/* 0 */
/* 248 */	NdrFcLong( 0xc8 ),	/* 200 */
/* 252 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 254 */	NdrFcShort( 0x1 ),	/* 1 */
/* 256 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 258 */	NdrFcShort( 0x0 ),	/* 0 */
/* 260 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 262 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 264 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 266 */	NdrFcShort( 0x10 ),	/* 16 */
/* 268 */	NdrFcShort( 0x0 ),	/* 0 */
/* 270 */	NdrFcShort( 0xa ),	/* Offset= 10 (280) */
/* 272 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 274 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (242) */
/* 276 */	0x42,		/* FC_STRUCTPAD6 */
			0x36,		/* FC_POINTER */
/* 278 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 280 */	
			0x12, 0x0,	/* FC_UP */
/* 282 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (252) */
/* 284 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 286 */	NdrFcShort( 0x2 ),	/* Offset= 2 (288) */
/* 288 */	
			0x13, 0x0,	/* FC_OP */
/* 290 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (264) */

			0x0
        }
    };


/* Standard interface: __MIDL_itf_napsystemhealthvalidator_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: INapSystemHealthValidator, ver. 0.0,
   GUID={0xEF43D87C,0x5B6D,0x4820,{0x86,0x20,0x86,0x89,0x07,0xFE,0x07,0xD3}} */

#pragma code_seg(".orpc")
static const unsigned short INapSystemHealthValidator_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO INapSystemHealthValidator_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapSystemHealthValidator_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapSystemHealthValidator_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapSystemHealthValidator_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _INapSystemHealthValidatorProxyVtbl = 
{
    &INapSystemHealthValidator_ProxyInfo,
    &IID_INapSystemHealthValidator,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* INapSystemHealthValidator::Validate */
};

const CInterfaceStubVtbl _INapSystemHealthValidatorStubVtbl =
{
    &IID_INapSystemHealthValidator,
    &INapSystemHealthValidator_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: INapServerCallback, ver. 0.0,
   GUID={0x09C20568,0xF30C,0x489b,{0xAE,0x9C,0x49,0x30,0xAD,0x7F,0x16,0x5F}} */

#pragma code_seg(".orpc")
static const unsigned short INapServerCallback_FormatStringOffsetTable[] =
    {
    50
    };

static const MIDL_STUBLESS_PROXY_INFO INapServerCallback_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapServerCallback_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapServerCallback_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapServerCallback_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _INapServerCallbackProxyVtbl = 
{
    &INapServerCallback_ProxyInfo,
    &IID_INapServerCallback,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* INapServerCallback::OnComplete */
};

const CInterfaceStubVtbl _INapServerCallbackStubVtbl =
{
    &IID_INapServerCallback,
    &INapServerCallback_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: INapSystemHealthValidationRequest, ver. 0.0,
   GUID={0xADACB3A0,0xD4F4,0x4f17,{0x89,0x33,0x51,0xD6,0x0F,0xCC,0xA6,0x06}} */

#pragma code_seg(".orpc")
static const unsigned short INapSystemHealthValidationRequest_FormatStringOffsetTable[] =
    {
    94,
    132,
    170,
    208,
    252,
    290,
    328,
    366
    };

static const MIDL_STUBLESS_PROXY_INFO INapSystemHealthValidationRequest_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapSystemHealthValidationRequest_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapSystemHealthValidationRequest_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapSystemHealthValidationRequest_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(11) _INapSystemHealthValidationRequestProxyVtbl = 
{
    &INapSystemHealthValidationRequest_ProxyInfo,
    &IID_INapSystemHealthValidationRequest,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* INapSystemHealthValidationRequest::GetCorrelationId */ ,
    (void *) (INT_PTR) -1 /* INapSystemHealthValidationRequest::GetStringCorrelationId */ ,
    (void *) (INT_PTR) -1 /* INapSystemHealthValidationRequest::GetMachineName */ ,
    (void *) (INT_PTR) -1 /* INapSystemHealthValidationRequest::GetSoHRequest */ ,
    (void *) (INT_PTR) -1 /* INapSystemHealthValidationRequest::SetSoHResponse */ ,
    (void *) (INT_PTR) -1 /* INapSystemHealthValidationRequest::GetSoHResponse */ ,
    (void *) (INT_PTR) -1 /* INapSystemHealthValidationRequest::SetPrivateData */ ,
    (void *) (INT_PTR) -1 /* INapSystemHealthValidationRequest::GetPrivateData */
};

const CInterfaceStubVtbl _INapSystemHealthValidationRequestStubVtbl =
{
    &IID_INapSystemHealthValidationRequest,
    &INapSystemHealthValidationRequest_ServerInfo,
    11,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x600015b, /* MIDL Version 6.0.347 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0   /* Reserved5 */
    };

const CInterfaceProxyVtbl * _napsystemhealthvalidator_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_INapServerCallbackProxyVtbl,
    ( CInterfaceProxyVtbl *) &_INapSystemHealthValidatorProxyVtbl,
    ( CInterfaceProxyVtbl *) &_INapSystemHealthValidationRequestProxyVtbl,
    0
};

const CInterfaceStubVtbl * _napsystemhealthvalidator_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_INapServerCallbackStubVtbl,
    ( CInterfaceStubVtbl *) &_INapSystemHealthValidatorStubVtbl,
    ( CInterfaceStubVtbl *) &_INapSystemHealthValidationRequestStubVtbl,
    0
};

PCInterfaceName const _napsystemhealthvalidator_InterfaceNamesList[] = 
{
    "INapServerCallback",
    "INapSystemHealthValidator",
    "INapSystemHealthValidationRequest",
    0
};


#define _napsystemhealthvalidator_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _napsystemhealthvalidator, pIID, n)

int __stdcall _napsystemhealthvalidator_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _napsystemhealthvalidator, 3, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _napsystemhealthvalidator, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _napsystemhealthvalidator, 3, *pIndex )
    
}

const ExtendedProxyFileInfo napsystemhealthvalidator_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _napsystemhealthvalidator_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _napsystemhealthvalidator_StubVtblList,
    (const PCInterfaceName * ) & _napsystemhealthvalidator_InterfaceNamesList,
    0, // no delegation
    & _napsystemhealthvalidator_IID_Lookup, 
    3,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* defined(_M_IA64) || defined(_M_AMD64)*/

