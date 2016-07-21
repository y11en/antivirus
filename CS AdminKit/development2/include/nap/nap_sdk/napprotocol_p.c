
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Oct 29 17:49:55 2007
 */
/* Compiler settings for O:\CS AdminKit\development2\nap\nap_sdk\include\napprotocol.idl:
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


#include "napprotocol.h"

#define TYPE_FORMAT_STRING_SIZE   415                               
#define PROC_FORMAT_STRING_SIZE   69                                
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


extern const MIDL_SERVER_INFO INapSoHConstructor_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapSoHConstructor_ProxyInfo;


HRESULT STDMETHODCALLTYPE INapSoHConstructor_Initialize_Proxy( 
    INapSoHConstructor * This,
    /* [in] */ SystemHealthEntityId id,
    /* [in] */ BOOL isRequest)
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
            
            _StubMsg.BufferLength = 16;
            NdrProxyGetBuffer(This, &_StubMsg);
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *(( SystemHealthEntityId * )_StubMsg.Buffer)++ = id;
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *(( BOOL * )_StubMsg.Buffer)++ = isRequest;
            
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

void __RPC_STUB INapSoHConstructor_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    SystemHealthEntityId id;
    BOOL isRequest;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    id = 0;
    isRequest = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[0] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        
        if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        id = *(( SystemHealthEntityId * )_StubMsg.Buffer)++;
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        
        if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        isRequest = *(( BOOL * )_StubMsg.Buffer)++;
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSoHConstructor*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> Initialize(
              (INapSoHConstructor *) ((CStdStubBuffer *)This)->pvServerObject,
              id,
              isRequest);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
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


HRESULT STDMETHODCALLTYPE INapSoHConstructor_AppendAttribute_Proxy( 
    INapSoHConstructor * This,
    /* [in] */ SoHAttributeType type,
    /* [switch_is][in] */ const SoHAttributeValue *value)
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
                      4);
        
        
        
        if(!value)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 6;
            _StubMsg.MaxCount = ( unsigned long  )type;
            
            NdrNonEncapsulatedUnionBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                               (unsigned char *)value,
                                               (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrSimpleTypeMarshall(
                         ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                         ( unsigned char * )&type,
                         13);
            _StubMsg.MaxCount = ( unsigned long  )type;
            
            NdrNonEncapsulatedUnionMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                             (unsigned char *)value,
                                             (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[6] );
            
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

void __RPC_STUB INapSoHConstructor_AppendAttribute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    SoHAttributeType type;
    const SoHAttributeValue *value;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    type = 0;
    ( SoHAttributeValue * )value = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[6] );
        
        NdrSimpleTypeUnmarshall(
                           ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                           ( unsigned char * )&type,
                           13);
        NdrNonEncapsulatedUnionUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                           (unsigned char * *)&value,
                                           (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6],
                                           (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSoHConstructor*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> AppendAttribute(
                   (INapSoHConstructor *) ((CStdStubBuffer *)This)->pvServerObject,
                   type,
                   value);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        _StubMsg.MaxCount = ( unsigned long  )type;
        
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)value,
                        &__MIDL_TypeFormatString.Format[2] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapSoHConstructor_GetSoH_Proxy( 
    INapSoHConstructor * This,
    /* [out] */ SoH **soh)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    if(soh)
        {
        *soh = 0;
        }
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      5);
        
        
        
        if(!soh)
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[14] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&soh,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[288],
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[288],
                         ( void * )soh);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapSoHConstructor_GetSoH_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    SoH *_M0;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    SoH **soh;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( SoH ** )soh = 0;
    RpcTryFinally
        {
        soh = &_M0;
        _M0 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSoHConstructor*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetSoH((INapSoHConstructor *) ((CStdStubBuffer *)This)->pvServerObject,soh);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)soh,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[288] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)soh,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[288] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)soh,
                        &__MIDL_TypeFormatString.Format[288] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapSoHConstructor_Validate_Proxy( 
    INapSoHConstructor * This,
    /* [in] */ const SoH *soh,
    /* [in] */ BOOL isRequest)
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
                      6);
        
        
        
        if(!soh)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 8;
            NdrSimpleStructBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char *)soh,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[360] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                     (unsigned char *)soh,
                                     (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[360] );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *(( BOOL * )_StubMsg.Buffer)++ = isRequest;
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[20] );
            
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

void __RPC_STUB INapSoHConstructor_Validate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    BOOL isRequest;
    const SoH *soh;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( SoH * )soh = 0;
    isRequest = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[20] );
        
        NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char * *)&soh,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[360],
                                   (unsigned char)0 );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        
        if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        isRequest = *(( BOOL * )_StubMsg.Buffer)++;
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSoHConstructor*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> Validate(
            (INapSoHConstructor *) ((CStdStubBuffer *)This)->pvServerObject,
            soh,
            isRequest);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)soh,
                        &__MIDL_TypeFormatString.Format[382] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO INapSoHProcessor_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapSoHProcessor_ProxyInfo;


HRESULT STDMETHODCALLTYPE INapSoHProcessor_Initialize_Proxy( 
    INapSoHProcessor * This,
    /* [in] */ const SoH *soh,
    /* [in] */ BOOL isRequest,
    /* [out] */ SystemHealthEntityId *id)
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
        
        
        
        if(!soh)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        if(!id)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 8;
            NdrSimpleStructBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char *)soh,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[360] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                     (unsigned char *)soh,
                                     (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[360] );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *(( BOOL * )_StubMsg.Buffer)++ = isRequest;
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[28] );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *id = *(( SystemHealthEntityId * )_StubMsg.Buffer)++;
            
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[386],
                         ( void * )id);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapSoHProcessor_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    SystemHealthEntityId _M1	=	0;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    SystemHealthEntityId *id;
    BOOL isRequest;
    const SoH *soh;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( SoH * )soh = 0;
    isRequest = 0;
    ( SystemHealthEntityId * )id = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[28] );
        
        NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char * *)&soh,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[360],
                                   (unsigned char)0 );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        
        if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        isRequest = *(( BOOL * )_StubMsg.Buffer)++;
        
        id = &_M1;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSoHProcessor*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> Initialize(
              (INapSoHProcessor *) ((CStdStubBuffer *)This)->pvServerObject,
              soh,
              isRequest,
              id);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 36;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( SystemHealthEntityId * )_StubMsg.Buffer)++ = *id;
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)soh,
                        &__MIDL_TypeFormatString.Format[382] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapSoHProcessor_FindNextAttribute_Proxy( 
    INapSoHProcessor * This,
    /* [in] */ UINT16 fromLocation,
    /* [in] */ SoHAttributeType type,
    /* [out] */ UINT16 *attributeLocation)
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
                      4);
        
        
        
        if(!attributeLocation)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 12;
            NdrProxyGetBuffer(This, &_StubMsg);
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 1) & ~ 0x1);
            *(( UINT16 * )_StubMsg.Buffer)++ = fromLocation;
            
            NdrSimpleTypeMarshall(
                         ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                         ( unsigned char * )&type,
                         13);
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[40] );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 1) & ~ 0x1);
            *attributeLocation = *(( UINT16 * )_StubMsg.Buffer)++;
            
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[390],
                         ( void * )attributeLocation);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapSoHProcessor_FindNextAttribute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    UINT16 _M2	=	0;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    UINT16 *attributeLocation;
    UINT16 fromLocation;
    SoHAttributeType type;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    fromLocation = 0;
    type = 0;
    ( UINT16 * )attributeLocation = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[40] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 1) & ~ 0x1);
        
        if(_StubMsg.Buffer + 2 > _StubMsg.BufferEnd)
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        fromLocation = *(( UINT16 * )_StubMsg.Buffer)++;
        
        NdrSimpleTypeUnmarshall(
                           ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                           ( unsigned char * )&type,
                           13);
        attributeLocation = &_M2;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSoHProcessor*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> FindNextAttribute(
                     (INapSoHProcessor *) ((CStdStubBuffer *)This)->pvServerObject,
                     fromLocation,
                     type,
                     attributeLocation);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 34;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 1) & ~ 0x1);
        *(( UINT16 * )_StubMsg.Buffer)++ = *attributeLocation;
        
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


HRESULT STDMETHODCALLTYPE INapSoHProcessor_GetAttribute_Proxy( 
    INapSoHProcessor * This,
    /* [in] */ UINT16 attributeLocation,
    /* [out] */ SoHAttributeType *type,
    /* [switch_is][out] */ SoHAttributeValue **value)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    if(value)
        {
        *value = 0;
        }
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      5);
        
        
        
        if(!type)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        if(!value)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 6;
            NdrProxyGetBuffer(This, &_StubMsg);
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 1) & ~ 0x1);
            *(( UINT16 * )_StubMsg.Buffer)++ = attributeLocation;
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[50] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&type,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[394],
                                  (unsigned char)0 );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&value,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[398],
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[394],
                         ( void * )type);
        _StubMsg.MaxCount = ( unsigned long  )(type ? *type : 0);
        
        NdrClearOutParameters(
                         ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[398],
                         ( void * )value);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapSoHProcessor_GetAttribute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    SoHAttributeType _M3	=	0;
    SoHAttributeValue *_M4;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    UINT16 attributeLocation;
    SoHAttributeType *type;
    SoHAttributeValue **value;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    attributeLocation = 0;
    ( SoHAttributeType * )type = 0;
    ( SoHAttributeValue ** )value = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[50] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 1) & ~ 0x1);
        
        if(_StubMsg.Buffer + 2 > _StubMsg.BufferEnd)
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        attributeLocation = *(( UINT16 * )_StubMsg.Buffer)++;
        
        type = &_M3;
        value = &_M4;
        _M4 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSoHProcessor*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetAttribute(
                (INapSoHProcessor *) ((CStdStubBuffer *)This)->pvServerObject,
                attributeLocation,
                type,
                value);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 34;
        _StubMsg.MaxCount = ( unsigned long  )(type ? *type : 0);
        
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)value,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[398] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)type,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[394] );
        
        _StubMsg.MaxCount = ( unsigned long  )(type ? *type : 0);
        
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)value,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[398] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        _StubMsg.MaxCount = ( unsigned long  )(type ? *type : 0);
        
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)value,
                        &__MIDL_TypeFormatString.Format[398] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapSoHProcessor_GetNumberOfAttributes_Proxy( 
    INapSoHProcessor * This,
    /* [out] */ UINT16 *attributeCount)
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
                      6);
        
        
        
        if(!attributeCount)
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[62] );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 1) & ~ 0x1);
            *attributeCount = *(( UINT16 * )_StubMsg.Buffer)++;
            
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[390],
                         ( void * )attributeCount);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapSoHProcessor_GetNumberOfAttributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    UINT16 _M5	=	0;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    UINT16 *attributeCount;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( UINT16 * )attributeCount = 0;
    RpcTryFinally
        {
        attributeCount = &_M5;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSoHProcessor*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetNumberOfAttributes((INapSoHProcessor *) ((CStdStubBuffer *)This)->pvServerObject,attributeCount);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 34;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 1) & ~ 0x1);
        *(( UINT16 * )_StubMsg.Buffer)++ = *attributeCount;
        
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



#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {
			0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/*  2 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/*  4 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/*  6 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xd,		/* FC_ENUM16 */
/*  8 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 10 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 12 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 14 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 16 */	NdrFcShort( 0x120 ),	/* Type Offset=288 */
/* 18 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 20 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 22 */	NdrFcShort( 0x17e ),	/* Type Offset=382 */
/* 24 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 26 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 28 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 30 */	NdrFcShort( 0x17e ),	/* Type Offset=382 */
/* 32 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 34 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 36 */	NdrFcShort( 0x182 ),	/* Type Offset=386 */
/* 38 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 40 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x6,		/* FC_SHORT */
/* 42 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0xd,		/* FC_ENUM16 */
/* 44 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 46 */	NdrFcShort( 0x186 ),	/* Type Offset=390 */
/* 48 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 50 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x6,		/* FC_SHORT */
/* 52 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 54 */	NdrFcShort( 0x18a ),	/* Type Offset=394 */
/* 56 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 58 */	NdrFcShort( 0x18e ),	/* Type Offset=398 */
/* 60 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 62 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 64 */	NdrFcShort( 0x186 ),	/* Type Offset=390 */
/* 66 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
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
			0x11, 0x0,	/* FC_RP */
/*  4 */	NdrFcShort( 0x2 ),	/* Offset= 2 (6) */
/*  6 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/*  8 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 10 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 12 */	NdrFcShort( 0x2 ),	/* Offset= 2 (14) */
/* 14 */	NdrFcShort( 0xc ),	/* 12 */
/* 16 */	NdrFcShort( 0xa ),	/* 10 */
/* 18 */	NdrFcLong( 0x2 ),	/* 2 */
/* 22 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 24 */	NdrFcLong( 0x3 ),	/* 3 */
/* 28 */	NdrFcShort( 0x52 ),	/* Offset= 82 (110) */
/* 30 */	NdrFcLong( 0xf ),	/* 15 */
/* 34 */	NdrFcShort( 0x80 ),	/* Offset= 128 (162) */
/* 36 */	NdrFcLong( 0x4 ),	/* 4 */
/* 40 */	NdrFcShort( 0x9a ),	/* Offset= 154 (194) */
/* 42 */	NdrFcLong( 0xd ),	/* 13 */
/* 46 */	NdrFcShort( 0x94 ),	/* Offset= 148 (194) */
/* 48 */	NdrFcLong( 0x5 ),	/* 5 */
/* 52 */	NdrFcShort( 0xa4 ),	/* Offset= 164 (216) */
/* 54 */	NdrFcLong( 0xc ),	/* 12 */
/* 58 */	NdrFcShort( 0x9e ),	/* Offset= 158 (216) */
/* 60 */	NdrFcLong( 0x7 ),	/* 7 */
/* 64 */	NdrFcShort( 0xaa ),	/* Offset= 170 (234) */
/* 66 */	NdrFcLong( 0x8 ),	/* 8 */
/* 70 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 72 */	NdrFcLong( 0xe ),	/* 14 */
/* 76 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 78 */	NdrFcShort( 0xbc ),	/* Offset= 188 (266) */
/* 80 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 82 */	NdrFcShort( 0x4 ),	/* 4 */
/* 84 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 86 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 88 */	NdrFcShort( 0x4 ),	/* 4 */
/* 90 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 92 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (80) */
/* 94 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 96 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 98 */	NdrFcShort( 0x4 ),	/* 4 */
/* 100 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 102 */	NdrFcShort( 0x0 ),	/* 0 */
/* 104 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 106 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (86) */
/* 108 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 110 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 112 */	NdrFcShort( 0x8 ),	/* 8 */
/* 114 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 116 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 118 */	NdrFcShort( 0x4 ),	/* 4 */
/* 120 */	NdrFcShort( 0x4 ),	/* 4 */
/* 122 */	0x12, 0x0,	/* FC_UP */
/* 124 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (96) */
/* 126 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 128 */	0x3e,		/* FC_STRUCTPAD2 */
			0x8,		/* FC_LONG */
/* 130 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 132 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 134 */	NdrFcShort( 0x10 ),	/* 16 */
/* 136 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 138 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 140 */	NdrFcShort( 0x10 ),	/* 16 */
/* 142 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 144 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (132) */
/* 146 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 148 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 150 */	NdrFcShort( 0x10 ),	/* 16 */
/* 152 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 154 */	NdrFcShort( 0x0 ),	/* 0 */
/* 156 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 158 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (138) */
/* 160 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 162 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 164 */	NdrFcShort( 0x8 ),	/* 8 */
/* 166 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 168 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 170 */	NdrFcShort( 0x4 ),	/* 4 */
/* 172 */	NdrFcShort( 0x4 ),	/* 4 */
/* 174 */	0x12, 0x0,	/* FC_UP */
/* 176 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (148) */
/* 178 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 180 */	0x3e,		/* FC_STRUCTPAD2 */
			0x8,		/* FC_LONG */
/* 182 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 184 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 186 */	NdrFcShort( 0x4 ),	/* 4 */
/* 188 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 190 */	NdrFcShort( 0x0 ),	/* 0 */
/* 192 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 194 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 196 */	NdrFcShort( 0x8 ),	/* 8 */
/* 198 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 200 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 202 */	NdrFcShort( 0x4 ),	/* 4 */
/* 204 */	NdrFcShort( 0x4 ),	/* 4 */
/* 206 */	0x12, 0x0,	/* FC_UP */
/* 208 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (184) */
/* 210 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 212 */	0x3e,		/* FC_STRUCTPAD2 */
			0x8,		/* FC_LONG */
/* 214 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 216 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 218 */	NdrFcShort( 0x8 ),	/* 8 */
/* 220 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 222 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 224 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 226 */	NdrFcShort( 0x1 ),	/* 1 */
/* 228 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 230 */	NdrFcShort( 0x4 ),	/* 4 */
/* 232 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 234 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 236 */	NdrFcShort( 0xc ),	/* 12 */
/* 238 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 240 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 242 */	NdrFcShort( 0x8 ),	/* 8 */
/* 244 */	NdrFcShort( 0x8 ),	/* 8 */
/* 246 */	0x12, 0x0,	/* FC_UP */
/* 248 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (224) */
/* 250 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 252 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 254 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 256 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 258 */	NdrFcShort( 0x1 ),	/* 1 */
/* 260 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 262 */	NdrFcShort( 0x0 ),	/* 0 */
/* 264 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 266 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 268 */	NdrFcShort( 0x8 ),	/* 8 */
/* 270 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 272 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 274 */	NdrFcShort( 0x4 ),	/* 4 */
/* 276 */	NdrFcShort( 0x4 ),	/* 4 */
/* 278 */	0x12, 0x0,	/* FC_UP */
/* 280 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (256) */
/* 282 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 284 */	0x3e,		/* FC_STRUCTPAD2 */
			0x8,		/* FC_LONG */
/* 286 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 288 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 290 */	NdrFcShort( 0x2 ),	/* Offset= 2 (292) */
/* 292 */	
			0x13, 0x0,	/* FC_OP */
/* 294 */	NdrFcShort( 0x42 ),	/* Offset= 66 (360) */
/* 296 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 298 */	NdrFcShort( 0x1 ),	/* 1 */
/* 300 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 302 */	NdrFcShort( 0x2 ),	/* 2 */
/* 304 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 306 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 308 */	NdrFcShort( 0x8 ),	/* 8 */
/* 310 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 312 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 314 */	NdrFcShort( 0x4 ),	/* 4 */
/* 316 */	NdrFcShort( 0x4 ),	/* 4 */
/* 318 */	0x13, 0x0,	/* FC_OP */
/* 320 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (296) */
/* 322 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 324 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 326 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 328 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 330 */	NdrFcShort( 0x8 ),	/* 8 */
/* 332 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 334 */	NdrFcShort( 0x0 ),	/* 0 */
/* 336 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 338 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 340 */	NdrFcShort( 0x8 ),	/* 8 */
/* 342 */	NdrFcShort( 0x0 ),	/* 0 */
/* 344 */	NdrFcShort( 0x1 ),	/* 1 */
/* 346 */	NdrFcShort( 0x4 ),	/* 4 */
/* 348 */	NdrFcShort( 0x4 ),	/* 4 */
/* 350 */	0x13, 0x0,	/* FC_OP */
/* 352 */	NdrFcShort( 0xffffffc8 ),	/* Offset= -56 (296) */
/* 354 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 356 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffcd ),	/* Offset= -51 (306) */
			0x5b,		/* FC_END */
/* 360 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 362 */	NdrFcShort( 0x8 ),	/* 8 */
/* 364 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 366 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 368 */	NdrFcShort( 0x4 ),	/* 4 */
/* 370 */	NdrFcShort( 0x4 ),	/* 4 */
/* 372 */	0x13, 0x0,	/* FC_OP */
/* 374 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (328) */
/* 376 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 378 */	0x3e,		/* FC_STRUCTPAD2 */
			0x8,		/* FC_LONG */
/* 380 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 382 */	
			0x11, 0x0,	/* FC_RP */
/* 384 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (360) */
/* 386 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 388 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 390 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 392 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 394 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 396 */	0xd,		/* FC_ENUM16 */
			0x5c,		/* FC_PAD */
/* 398 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 400 */	NdrFcShort( 0x2 ),	/* Offset= 2 (402) */
/* 402 */	
			0x13, 0x0,	/* FC_OP */
/* 404 */	NdrFcShort( 0x2 ),	/* Offset= 2 (406) */
/* 406 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 408 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x54,		/* FC_DEREFERENCE */
/* 410 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 412 */	NdrFcShort( 0xfffffe72 ),	/* Offset= -398 (14) */

			0x0
        }
    };


/* Standard interface: INapSoHTypes, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: INapSoHConstructor, ver. 0.0,
   GUID={0x35298344,0x96A6,0x45e7,{0x9B,0x6B,0x62,0xEC,0xC6,0xE0,0x99,0x20}} */

#pragma code_seg(".orpc")
static const unsigned short INapSoHConstructor_FormatStringOffsetTable[] =
    {
    0,
    6,
    14,
    20
    };

static const MIDL_STUBLESS_PROXY_INFO INapSoHConstructor_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapSoHConstructor_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapSoHConstructor_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapSoHConstructor_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
const CINTERFACE_PROXY_VTABLE(7) _INapSoHConstructorProxyVtbl = 
{
    &IID_INapSoHConstructor,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    INapSoHConstructor_Initialize_Proxy ,
    INapSoHConstructor_AppendAttribute_Proxy ,
    INapSoHConstructor_GetSoH_Proxy ,
    INapSoHConstructor_Validate_Proxy
};


static const PRPC_STUB_FUNCTION INapSoHConstructor_table[] =
{
    INapSoHConstructor_Initialize_Stub,
    INapSoHConstructor_AppendAttribute_Stub,
    INapSoHConstructor_GetSoH_Stub,
    INapSoHConstructor_Validate_Stub
};

const CInterfaceStubVtbl _INapSoHConstructorStubVtbl =
{
    &IID_INapSoHConstructor,
    &INapSoHConstructor_ServerInfo,
    7,
    &INapSoHConstructor_table[-3],
    CStdStubBuffer_METHODS
};


/* Object interface: INapSoHProcessor, ver. 0.0,
   GUID={0xFB2FA8B0,0x2CD5,0x457d,{0xAB,0xA8,0x43,0x76,0xF6,0x3E,0xA1,0xC0}} */

#pragma code_seg(".orpc")
static const unsigned short INapSoHProcessor_FormatStringOffsetTable[] =
    {
    28,
    40,
    50,
    62
    };

static const MIDL_STUBLESS_PROXY_INFO INapSoHProcessor_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapSoHProcessor_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapSoHProcessor_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapSoHProcessor_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
const CINTERFACE_PROXY_VTABLE(7) _INapSoHProcessorProxyVtbl = 
{
    &IID_INapSoHProcessor,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    INapSoHProcessor_Initialize_Proxy ,
    INapSoHProcessor_FindNextAttribute_Proxy ,
    INapSoHProcessor_GetAttribute_Proxy ,
    INapSoHProcessor_GetNumberOfAttributes_Proxy
};


static const PRPC_STUB_FUNCTION INapSoHProcessor_table[] =
{
    INapSoHProcessor_Initialize_Stub,
    INapSoHProcessor_FindNextAttribute_Stub,
    INapSoHProcessor_GetAttribute_Stub,
    INapSoHProcessor_GetNumberOfAttributes_Stub
};

const CInterfaceStubVtbl _INapSoHProcessorStubVtbl =
{
    &IID_INapSoHProcessor,
    &INapSoHProcessor_ServerInfo,
    7,
    &INapSoHProcessor_table[-3],
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_napprotocol_0257, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */

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

const CInterfaceProxyVtbl * _napprotocol_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_INapSoHConstructorProxyVtbl,
    ( CInterfaceProxyVtbl *) &_INapSoHProcessorProxyVtbl,
    0
};

const CInterfaceStubVtbl * _napprotocol_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_INapSoHConstructorStubVtbl,
    ( CInterfaceStubVtbl *) &_INapSoHProcessorStubVtbl,
    0
};

PCInterfaceName const _napprotocol_InterfaceNamesList[] = 
{
    "INapSoHConstructor",
    "INapSoHProcessor",
    0
};


#define _napprotocol_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _napprotocol, pIID, n)

int __stdcall _napprotocol_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _napprotocol, 2, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _napprotocol, 2, *pIndex )
    
}

const ExtendedProxyFileInfo napprotocol_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _napprotocol_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _napprotocol_StubVtblList,
    (const PCInterfaceName * ) & _napprotocol_InterfaceNamesList,
    0, // no delegation
    & _napprotocol_IID_Lookup, 
    2,
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
/* at Mon Oct 29 17:49:55 2007
 */
/* Compiler settings for O:\CS AdminKit\development2\nap\nap_sdk\include\napprotocol.idl:
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


#include "napprotocol.h"

#define TYPE_FORMAT_STRING_SIZE   477                               
#define PROC_FORMAT_STRING_SIZE   359                               
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


extern const MIDL_SERVER_INFO INapSoHConstructor_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapSoHConstructor_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO INapSoHProcessor_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapSoHProcessor_ProxyInfo;



#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure Initialize */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 10 */	NdrFcShort( 0x10 ),	/* 16 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 16 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter id */

/* 26 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 28 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 30 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter isRequest */

/* 32 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 34 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 36 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 38 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 40 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 42 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AppendAttribute */

/* 44 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 46 */	NdrFcLong( 0x0 ),	/* 0 */
/* 50 */	NdrFcShort( 0x4 ),	/* 4 */
/* 52 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 54 */	NdrFcShort( 0x6 ),	/* 6 */
/* 56 */	NdrFcShort( 0x8 ),	/* 8 */
/* 58 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 60 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 62 */	NdrFcShort( 0x0 ),	/* 0 */
/* 64 */	NdrFcShort( 0x6 ),	/* 6 */
/* 66 */	NdrFcShort( 0x0 ),	/* 0 */
/* 68 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter type */

/* 70 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 72 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 74 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter value */

/* 76 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 78 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 80 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 82 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 84 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 86 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSoH */

/* 88 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 90 */	NdrFcLong( 0x0 ),	/* 0 */
/* 94 */	NdrFcShort( 0x5 ),	/* 5 */
/* 96 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 98 */	NdrFcShort( 0x0 ),	/* 0 */
/* 100 */	NdrFcShort( 0x8 ),	/* 8 */
/* 102 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 104 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 106 */	NdrFcShort( 0x2 ),	/* 2 */
/* 108 */	NdrFcShort( 0x0 ),	/* 0 */
/* 110 */	NdrFcShort( 0x0 ),	/* 0 */
/* 112 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter soh */

/* 114 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 116 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 118 */	NdrFcShort( 0x154 ),	/* Type Offset=340 */

	/* Return value */

/* 120 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 122 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 124 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Validate */

/* 126 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 128 */	NdrFcLong( 0x0 ),	/* 0 */
/* 132 */	NdrFcShort( 0x6 ),	/* 6 */
/* 134 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 136 */	NdrFcShort( 0x8 ),	/* 8 */
/* 138 */	NdrFcShort( 0x8 ),	/* 8 */
/* 140 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 142 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 144 */	NdrFcShort( 0x0 ),	/* 0 */
/* 146 */	NdrFcShort( 0x2 ),	/* 2 */
/* 148 */	NdrFcShort( 0x0 ),	/* 0 */
/* 150 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter soh */

/* 152 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 154 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 156 */	NdrFcShort( 0x1a6 ),	/* Type Offset=422 */

	/* Parameter isRequest */

/* 158 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 160 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 162 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 164 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 166 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 168 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Initialize */

/* 170 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 172 */	NdrFcLong( 0x0 ),	/* 0 */
/* 176 */	NdrFcShort( 0x3 ),	/* 3 */
/* 178 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 180 */	NdrFcShort( 0x8 ),	/* 8 */
/* 182 */	NdrFcShort( 0x24 ),	/* 36 */
/* 184 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 186 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 188 */	NdrFcShort( 0x0 ),	/* 0 */
/* 190 */	NdrFcShort( 0x2 ),	/* 2 */
/* 192 */	NdrFcShort( 0x0 ),	/* 0 */
/* 194 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter soh */

/* 196 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 198 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 200 */	NdrFcShort( 0x1a6 ),	/* Type Offset=422 */

	/* Parameter isRequest */

/* 202 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 204 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 206 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter id */

/* 208 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 210 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 212 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 214 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 216 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 218 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure FindNextAttribute */

/* 220 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 222 */	NdrFcLong( 0x0 ),	/* 0 */
/* 226 */	NdrFcShort( 0x4 ),	/* 4 */
/* 228 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 230 */	NdrFcShort( 0xc ),	/* 12 */
/* 232 */	NdrFcShort( 0x22 ),	/* 34 */
/* 234 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 236 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 238 */	NdrFcShort( 0x0 ),	/* 0 */
/* 240 */	NdrFcShort( 0x0 ),	/* 0 */
/* 242 */	NdrFcShort( 0x0 ),	/* 0 */
/* 244 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter fromLocation */

/* 246 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 248 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 250 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter type */

/* 252 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 254 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 256 */	0xd,		/* FC_ENUM16 */
			0x0,		/* 0 */

	/* Parameter attributeLocation */

/* 258 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 260 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 262 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 264 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 266 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 268 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAttribute */

/* 270 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 272 */	NdrFcLong( 0x0 ),	/* 0 */
/* 276 */	NdrFcShort( 0x5 ),	/* 5 */
/* 278 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 280 */	NdrFcShort( 0x6 ),	/* 6 */
/* 282 */	NdrFcShort( 0x22 ),	/* 34 */
/* 284 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 286 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 288 */	NdrFcShort( 0x6 ),	/* 6 */
/* 290 */	NdrFcShort( 0x0 ),	/* 0 */
/* 292 */	NdrFcShort( 0x0 ),	/* 0 */
/* 294 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter attributeLocation */

/* 296 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 298 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 300 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter type */

/* 302 */	NdrFcShort( 0x2010 ),	/* Flags:  out, srv alloc size=8 */
/* 304 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 306 */	NdrFcShort( 0x1c6 ),	/* Type Offset=454 */

	/* Parameter value */

/* 308 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 310 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 312 */	NdrFcShort( 0x1ca ),	/* Type Offset=458 */

	/* Return value */

/* 314 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 316 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 318 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetNumberOfAttributes */

/* 320 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 322 */	NdrFcLong( 0x0 ),	/* 0 */
/* 326 */	NdrFcShort( 0x6 ),	/* 6 */
/* 328 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 330 */	NdrFcShort( 0x0 ),	/* 0 */
/* 332 */	NdrFcShort( 0x22 ),	/* 34 */
/* 334 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 336 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 338 */	NdrFcShort( 0x0 ),	/* 0 */
/* 340 */	NdrFcShort( 0x0 ),	/* 0 */
/* 342 */	NdrFcShort( 0x0 ),	/* 0 */
/* 344 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter attributeCount */

/* 346 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 348 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 350 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 352 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 354 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 356 */	0x8,		/* FC_LONG */
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
			0x11, 0x0,	/* FC_RP */
/*  4 */	NdrFcShort( 0x2 ),	/* Offset= 2 (6) */
/*  6 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/*  8 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x0,		/*  */
/* 10 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 12 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 14 */	NdrFcShort( 0x2 ),	/* Offset= 2 (16) */
/* 16 */	NdrFcShort( 0x10 ),	/* 16 */
/* 18 */	NdrFcShort( 0xa ),	/* 10 */
/* 20 */	NdrFcLong( 0x2 ),	/* 2 */
/* 24 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 26 */	NdrFcLong( 0x3 ),	/* 3 */
/* 30 */	NdrFcShort( 0x5e ),	/* Offset= 94 (124) */
/* 32 */	NdrFcLong( 0xf ),	/* 15 */
/* 36 */	NdrFcShort( 0x96 ),	/* Offset= 150 (186) */
/* 38 */	NdrFcLong( 0x4 ),	/* 4 */
/* 42 */	NdrFcShort( 0xba ),	/* Offset= 186 (228) */
/* 44 */	NdrFcLong( 0xd ),	/* 13 */
/* 48 */	NdrFcShort( 0xb4 ),	/* Offset= 180 (228) */
/* 50 */	NdrFcLong( 0x5 ),	/* 5 */
/* 54 */	NdrFcShort( 0xc2 ),	/* Offset= 194 (248) */
/* 56 */	NdrFcLong( 0xc ),	/* 12 */
/* 60 */	NdrFcShort( 0xbc ),	/* Offset= 188 (248) */
/* 62 */	NdrFcLong( 0x7 ),	/* 7 */
/* 66 */	NdrFcShort( 0xd4 ),	/* Offset= 212 (278) */
/* 68 */	NdrFcLong( 0x8 ),	/* 8 */
/* 72 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 74 */	NdrFcLong( 0xe ),	/* 14 */
/* 78 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 80 */	NdrFcShort( 0xf0 ),	/* Offset= 240 (320) */
/* 82 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 84 */	NdrFcLong( 0x1 ),	/* 1 */
/* 88 */	NdrFcLong( 0x3e8 ),	/* 1000 */
/* 92 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 94 */	NdrFcShort( 0x4 ),	/* 4 */
/* 96 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 98 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 100 */	NdrFcShort( 0x4 ),	/* 4 */
/* 102 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 104 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (92) */
/* 106 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 108 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 110 */	NdrFcShort( 0x4 ),	/* 4 */
/* 112 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 114 */	NdrFcShort( 0x0 ),	/* 0 */
/* 116 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 118 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 120 */	NdrFcShort( 0xffffffea ),	/* Offset= -22 (98) */
/* 122 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 124 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 126 */	NdrFcShort( 0x10 ),	/* 16 */
/* 128 */	NdrFcShort( 0x0 ),	/* 0 */
/* 130 */	NdrFcShort( 0xa ),	/* Offset= 10 (140) */
/* 132 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 134 */	NdrFcShort( 0xffffffcc ),	/* Offset= -52 (82) */
/* 136 */	0x42,		/* FC_STRUCTPAD6 */
			0x36,		/* FC_POINTER */
/* 138 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 140 */	
			0x12, 0x0,	/* FC_UP */
/* 142 */	NdrFcShort( 0xffffffde ),	/* Offset= -34 (108) */
/* 144 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 146 */	NdrFcLong( 0x1 ),	/* 1 */
/* 150 */	NdrFcLong( 0xfa ),	/* 250 */
/* 154 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 156 */	NdrFcShort( 0x10 ),	/* 16 */
/* 158 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 160 */	
			0x15,		/* FC_STRUCT */
			0x0,		/* 0 */
/* 162 */	NdrFcShort( 0x10 ),	/* 16 */
/* 164 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 166 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (154) */
/* 168 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 170 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 172 */	NdrFcShort( 0x10 ),	/* 16 */
/* 174 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 176 */	NdrFcShort( 0x0 ),	/* 0 */
/* 178 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 180 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 182 */	NdrFcShort( 0xffffffea ),	/* Offset= -22 (160) */
/* 184 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 186 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 188 */	NdrFcShort( 0x10 ),	/* 16 */
/* 190 */	NdrFcShort( 0x0 ),	/* 0 */
/* 192 */	NdrFcShort( 0xa ),	/* Offset= 10 (202) */
/* 194 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 196 */	NdrFcShort( 0xffffffcc ),	/* Offset= -52 (144) */
/* 198 */	0x42,		/* FC_STRUCTPAD6 */
			0x36,		/* FC_POINTER */
/* 200 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 202 */	
			0x12, 0x0,	/* FC_UP */
/* 204 */	NdrFcShort( 0xffffffde ),	/* Offset= -34 (170) */
/* 206 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 208 */	NdrFcLong( 0x0 ),	/* 0 */
/* 212 */	NdrFcLong( 0x3e8 ),	/* 1000 */
/* 216 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 218 */	NdrFcShort( 0x4 ),	/* 4 */
/* 220 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 222 */	NdrFcShort( 0x0 ),	/* 0 */
/* 224 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 226 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 228 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 230 */	NdrFcShort( 0x10 ),	/* 16 */
/* 232 */	NdrFcShort( 0x0 ),	/* 0 */
/* 234 */	NdrFcShort( 0xa ),	/* Offset= 10 (244) */
/* 236 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 238 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (206) */
/* 240 */	0x42,		/* FC_STRUCTPAD6 */
			0x36,		/* FC_POINTER */
/* 242 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 244 */	
			0x12, 0x0,	/* FC_UP */
/* 246 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (216) */
/* 248 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 250 */	NdrFcShort( 0x8 ),	/* 8 */
/* 252 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 254 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 256 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 258 */	NdrFcLong( 0x0 ),	/* 0 */
/* 262 */	NdrFcLong( 0xf9c ),	/* 3996 */
/* 266 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 268 */	NdrFcShort( 0x1 ),	/* 1 */
/* 270 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 272 */	NdrFcShort( 0x4 ),	/* 4 */
/* 274 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 276 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 278 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 280 */	NdrFcShort( 0x10 ),	/* 16 */
/* 282 */	NdrFcShort( 0x0 ),	/* 0 */
/* 284 */	NdrFcShort( 0xa ),	/* Offset= 10 (294) */
/* 286 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 288 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffdf ),	/* Offset= -33 (256) */
			0x3e,		/* FC_STRUCTPAD2 */
/* 292 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 294 */	
			0x12, 0x0,	/* FC_UP */
/* 296 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (266) */
/* 298 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 300 */	NdrFcLong( 0x0 ),	/* 0 */
/* 304 */	NdrFcLong( 0xfa0 ),	/* 4000 */
/* 308 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 310 */	NdrFcShort( 0x1 ),	/* 1 */
/* 312 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 316 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 318 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 320 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 322 */	NdrFcShort( 0x10 ),	/* 16 */
/* 324 */	NdrFcShort( 0x0 ),	/* 0 */
/* 326 */	NdrFcShort( 0xa ),	/* Offset= 10 (336) */
/* 328 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 330 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (298) */
/* 332 */	0x42,		/* FC_STRUCTPAD6 */
			0x36,		/* FC_POINTER */
/* 334 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 336 */	
			0x12, 0x0,	/* FC_UP */
/* 338 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (308) */
/* 340 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 342 */	NdrFcShort( 0x2 ),	/* Offset= 2 (344) */
/* 344 */	
			0x13, 0x0,	/* FC_OP */
/* 346 */	NdrFcShort( 0x4c ),	/* Offset= 76 (422) */
/* 348 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 350 */	NdrFcLong( 0x0 ),	/* 0 */
/* 354 */	NdrFcLong( 0x64 ),	/* 100 */
/* 358 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 360 */	NdrFcLong( 0x0 ),	/* 0 */
/* 364 */	NdrFcLong( 0xfa0 ),	/* 4000 */
/* 368 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 370 */	NdrFcShort( 0x1 ),	/* 1 */
/* 372 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 374 */	NdrFcShort( 0x2 ),	/* 2 */
/* 376 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 378 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 380 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 382 */	NdrFcShort( 0x10 ),	/* 16 */
/* 384 */	NdrFcShort( 0x0 ),	/* 0 */
/* 386 */	NdrFcShort( 0xa ),	/* Offset= 10 (396) */
/* 388 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 390 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffdf ),	/* Offset= -33 (358) */
			0x40,		/* FC_STRUCTPAD4 */
/* 394 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 396 */	
			0x13, 0x0,	/* FC_OP */
/* 398 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (368) */
/* 400 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 402 */	NdrFcShort( 0x0 ),	/* 0 */
/* 404 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 406 */	NdrFcShort( 0x0 ),	/* 0 */
/* 408 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 410 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 414 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 416 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 418 */	NdrFcShort( 0xffffffda ),	/* Offset= -38 (380) */
/* 420 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 422 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 424 */	NdrFcShort( 0x10 ),	/* 16 */
/* 426 */	NdrFcShort( 0x0 ),	/* 0 */
/* 428 */	NdrFcShort( 0xa ),	/* Offset= 10 (438) */
/* 430 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 432 */	NdrFcShort( 0xffffffac ),	/* Offset= -84 (348) */
/* 434 */	0x42,		/* FC_STRUCTPAD6 */
			0x36,		/* FC_POINTER */
/* 436 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 438 */	
			0x13, 0x0,	/* FC_OP */
/* 440 */	NdrFcShort( 0xffffffd8 ),	/* Offset= -40 (400) */
/* 442 */	
			0x11, 0x0,	/* FC_RP */
/* 444 */	NdrFcShort( 0xffffffea ),	/* Offset= -22 (422) */
/* 446 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 448 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 450 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 452 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 454 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 456 */	0xd,		/* FC_ENUM16 */
			0x5c,		/* FC_PAD */
/* 458 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 460 */	NdrFcShort( 0x2 ),	/* Offset= 2 (462) */
/* 462 */	
			0x13, 0x0,	/* FC_OP */
/* 464 */	NdrFcShort( 0x2 ),	/* Offset= 2 (466) */
/* 466 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0xd,		/* FC_ENUM16 */
/* 468 */	0x26,		/* Corr desc:  parameter, FC_SHORT */
			0x54,		/* FC_DEREFERENCE */
/* 470 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 472 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 474 */	NdrFcShort( 0xfffffe36 ),	/* Offset= -458 (16) */

			0x0
        }
    };


/* Standard interface: INapSoHTypes, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: INapSoHConstructor, ver. 0.0,
   GUID={0x35298344,0x96A6,0x45e7,{0x9B,0x6B,0x62,0xEC,0xC6,0xE0,0x99,0x20}} */

#pragma code_seg(".orpc")
static const unsigned short INapSoHConstructor_FormatStringOffsetTable[] =
    {
    0,
    44,
    88,
    126
    };

static const MIDL_STUBLESS_PROXY_INFO INapSoHConstructor_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapSoHConstructor_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapSoHConstructor_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapSoHConstructor_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _INapSoHConstructorProxyVtbl = 
{
    &INapSoHConstructor_ProxyInfo,
    &IID_INapSoHConstructor,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* INapSoHConstructor::Initialize */ ,
    (void *) (INT_PTR) -1 /* INapSoHConstructor::AppendAttribute */ ,
    (void *) (INT_PTR) -1 /* INapSoHConstructor::GetSoH */ ,
    (void *) (INT_PTR) -1 /* INapSoHConstructor::Validate */
};

const CInterfaceStubVtbl _INapSoHConstructorStubVtbl =
{
    &IID_INapSoHConstructor,
    &INapSoHConstructor_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: INapSoHProcessor, ver. 0.0,
   GUID={0xFB2FA8B0,0x2CD5,0x457d,{0xAB,0xA8,0x43,0x76,0xF6,0x3E,0xA1,0xC0}} */

#pragma code_seg(".orpc")
static const unsigned short INapSoHProcessor_FormatStringOffsetTable[] =
    {
    170,
    220,
    270,
    320
    };

static const MIDL_STUBLESS_PROXY_INFO INapSoHProcessor_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapSoHProcessor_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapSoHProcessor_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapSoHProcessor_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _INapSoHProcessorProxyVtbl = 
{
    &INapSoHProcessor_ProxyInfo,
    &IID_INapSoHProcessor,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* INapSoHProcessor::Initialize */ ,
    (void *) (INT_PTR) -1 /* INapSoHProcessor::FindNextAttribute */ ,
    (void *) (INT_PTR) -1 /* INapSoHProcessor::GetAttribute */ ,
    (void *) (INT_PTR) -1 /* INapSoHProcessor::GetNumberOfAttributes */
};

const CInterfaceStubVtbl _INapSoHProcessorStubVtbl =
{
    &IID_INapSoHProcessor,
    &INapSoHProcessor_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_napprotocol_0257, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */

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

const CInterfaceProxyVtbl * _napprotocol_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_INapSoHConstructorProxyVtbl,
    ( CInterfaceProxyVtbl *) &_INapSoHProcessorProxyVtbl,
    0
};

const CInterfaceStubVtbl * _napprotocol_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_INapSoHConstructorStubVtbl,
    ( CInterfaceStubVtbl *) &_INapSoHProcessorStubVtbl,
    0
};

PCInterfaceName const _napprotocol_InterfaceNamesList[] = 
{
    "INapSoHConstructor",
    "INapSoHProcessor",
    0
};


#define _napprotocol_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _napprotocol, pIID, n)

int __stdcall _napprotocol_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _napprotocol, 2, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _napprotocol, 2, *pIndex )
    
}

const ExtendedProxyFileInfo napprotocol_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _napprotocol_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _napprotocol_StubVtblList,
    (const PCInterfaceName * ) & _napprotocol_InterfaceNamesList,
    0, // no delegation
    & _napprotocol_IID_Lookup, 
    2,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* defined(_M_IA64) || defined(_M_AMD64)*/

