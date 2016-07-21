
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Tue Oct 30 15:55:03 2007
 */
/* Compiler settings for O:\CS AdminKit\development2\nap\nap_sdk\include\napenforcementclient.idl:
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


#include "napenforcementclient.h"

#define TYPE_FORMAT_STRING_SIZE   263                               
#define PROC_FORMAT_STRING_SIZE   119                               
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


extern const MIDL_SERVER_INFO INapEnforcementClientBinding_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapEnforcementClientBinding_ProxyInfo;


HRESULT STDMETHODCALLTYPE INapEnforcementClientBinding_Initialize_Proxy( 
    INapEnforcementClientBinding * This,
    /* [in] */ EnforcementEntityId id,
    /* [in] */ INapEnforcementClientCallback *callback)
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
                                           (unsigned char *)callback,
                                           (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[2] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *(( EnforcementEntityId * )_StubMsg.Buffer)++ = id;
            
            NdrInterfacePointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                         (unsigned char *)callback,
                                         (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[2] );
            
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

void __RPC_STUB INapEnforcementClientBinding_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    INapEnforcementClientCallback *callback;
    EnforcementEntityId id;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    id = 0;
    callback = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[0] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        
        if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        id = *(( EnforcementEntityId * )_StubMsg.Buffer)++;
        
        NdrInterfacePointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char * *)&callback,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[2],
                                       (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientBinding*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> Initialize(
              (INapEnforcementClientBinding *) ((CStdStubBuffer *)This)->pvServerObject,
              id,
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
                                 (unsigned char *)callback,
                                 &__MIDL_TypeFormatString.Format[2] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapEnforcementClientBinding_Uninitialize_Proxy( 
    INapEnforcementClientBinding * This)
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
        
        
        
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[8] );
            
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

void __RPC_STUB INapEnforcementClientBinding_Uninitialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    RpcTryFinally
        {
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientBinding*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> Uninitialize((INapEnforcementClientBinding *) ((CStdStubBuffer *)This)->pvServerObject);
        
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


HRESULT STDMETHODCALLTYPE INapEnforcementClientBinding_CreateConnection_Proxy( 
    INapEnforcementClientBinding * This,
    /* [out] */ INapEnforcementClientConnection **connection)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    if(connection)
        {
        MIDL_memset(
               connection,
               0,
               sizeof( INapEnforcementClientConnection ** ));
        }
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      5);
        
        
        
        if(!connection)
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[10] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&connection,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[20],
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[20],
                         ( void * )connection);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapEnforcementClientBinding_CreateConnection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    INapEnforcementClientConnection *_M0;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    INapEnforcementClientConnection **connection;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( INapEnforcementClientConnection ** )connection = 0;
    RpcTryFinally
        {
        connection = &_M0;
        _M0 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientBinding*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> CreateConnection((INapEnforcementClientBinding *) ((CStdStubBuffer *)This)->pvServerObject,connection);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)connection,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[20] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)connection,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[20] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)connection,
                        &__MIDL_TypeFormatString.Format[20] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapEnforcementClientBinding_GetSoHRequest_Proxy( 
    INapEnforcementClientBinding * This,
    /* [in] */ INapEnforcementClientConnection *connection,
    /* [out] */ BOOL *retriggerHint)
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
        
        
        
        if(!retriggerHint)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrInterfacePointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                           (unsigned char *)connection,
                                           (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[24] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrInterfacePointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                         (unsigned char *)connection,
                                         (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[24] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[16] );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *retriggerHint = *(( BOOL * )_StubMsg.Buffer)++;
            
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[42],
                         ( void * )retriggerHint);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapEnforcementClientBinding_GetSoHRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    BOOL _M1	=	0;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    INapEnforcementClientConnection *connection;
    BOOL *retriggerHint;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    connection = 0;
    ( BOOL * )retriggerHint = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[16] );
        
        NdrInterfacePointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char * *)&connection,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[24],
                                       (unsigned char)0 );
        
        retriggerHint = &_M1;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientBinding*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetSoHRequest(
                 (INapEnforcementClientBinding *) ((CStdStubBuffer *)This)->pvServerObject,
                 connection,
                 retriggerHint);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 36;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( BOOL * )_StubMsg.Buffer)++ = *retriggerHint;
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrInterfacePointerFree( &_StubMsg,
                                 (unsigned char *)connection,
                                 &__MIDL_TypeFormatString.Format[24] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapEnforcementClientBinding_ProcessSoHResponse_Proxy( 
    INapEnforcementClientBinding * This,
    /* [in] */ INapEnforcementClientConnection *connection)
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
        
        
        
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrInterfacePointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                           (unsigned char *)connection,
                                           (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[24] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrInterfacePointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                         (unsigned char *)connection,
                                         (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[24] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[26] );
            
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

void __RPC_STUB INapEnforcementClientBinding_ProcessSoHResponse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    INapEnforcementClientConnection *connection;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    connection = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[26] );
        
        NdrInterfacePointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char * *)&connection,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[24],
                                       (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientBinding*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> ProcessSoHResponse((INapEnforcementClientBinding *) ((CStdStubBuffer *)This)->pvServerObject,connection);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrInterfacePointerFree( &_StubMsg,
                                 (unsigned char *)connection,
                                 &__MIDL_TypeFormatString.Format[24] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapEnforcementClientBinding_NotifyConnectionStateDown_Proxy( 
    INapEnforcementClientBinding * This,
    /* [in] */ INapEnforcementClientConnection *downCxn)
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
                      8);
        
        
        
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrInterfacePointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                           (unsigned char *)downCxn,
                                           (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[24] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrInterfacePointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                         (unsigned char *)downCxn,
                                         (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[24] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[26] );
            
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

void __RPC_STUB INapEnforcementClientBinding_NotifyConnectionStateDown_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    INapEnforcementClientConnection *downCxn;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    downCxn = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[26] );
        
        NdrInterfacePointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char * *)&downCxn,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[24],
                                       (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientBinding*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> NotifyConnectionStateDown((INapEnforcementClientBinding *) ((CStdStubBuffer *)This)->pvServerObject,downCxn);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrInterfacePointerFree( &_StubMsg,
                                 (unsigned char *)downCxn,
                                 &__MIDL_TypeFormatString.Format[24] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapEnforcementClientBinding_NotifySoHChangeFailure_Proxy( 
    INapEnforcementClientBinding * This)
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
        
        
        
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[8] );
            
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

void __RPC_STUB INapEnforcementClientBinding_NotifySoHChangeFailure_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    RpcTryFinally
        {
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientBinding*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> NotifySoHChangeFailure((INapEnforcementClientBinding *) ((CStdStubBuffer *)This)->pvServerObject);
        
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


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO INapEnforcementClientCallback_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapEnforcementClientCallback_ProxyInfo;


HRESULT STDMETHODCALLTYPE INapEnforcementClientCallback_NotifySoHChange_Proxy( 
    INapEnforcementClientCallback * This)
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
            
            _StubMsg.BufferLength = 0;
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[8] );
            
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

void __RPC_STUB INapEnforcementClientCallback_NotifySoHChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    RpcTryFinally
        {
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientCallback*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> NotifySoHChange((INapEnforcementClientCallback *) ((CStdStubBuffer *)This)->pvServerObject);
        
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


HRESULT STDMETHODCALLTYPE INapEnforcementClientCallback_GetConnections_Proxy( 
    INapEnforcementClientCallback * This,
    /* [out] */ Connections **connections)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    if(connections)
        {
        *connections = 0;
        }
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      4);
        
        
        
        if(!connections)
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
                                  (unsigned char * *)&connections,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[46],
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[46],
                         ( void * )connections);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapEnforcementClientCallback_GetConnections_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    Connections *_M2;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    Connections **connections;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( Connections ** )connections = 0;
    RpcTryFinally
        {
        connections = &_M2;
        _M2 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientCallback*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetConnections((INapEnforcementClientCallback *) ((CStdStubBuffer *)This)->pvServerObject,connections);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)connections,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[46] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)connections,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[46] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)connections,
                        &__MIDL_TypeFormatString.Format[46] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO INapEnforcementClientConnection_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapEnforcementClientConnection_ProxyInfo;


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_Initialize_Proxy( 
    INapEnforcementClientConnection * This,
    /* [in] */ EnforcementEntityId id)
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
            NdrProxyGetBuffer(This, &_StubMsg);
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *(( EnforcementEntityId * )_StubMsg.Buffer)++ = id;
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[38] );
            
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

void __RPC_STUB INapEnforcementClientConnection_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    EnforcementEntityId id;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    id = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[38] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        
        if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        id = *(( EnforcementEntityId * )_StubMsg.Buffer)++;
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientConnection*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> Initialize((INapEnforcementClientConnection *) ((CStdStubBuffer *)This)->pvServerObject,id);
        
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


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_SetMaxSize_Proxy( 
    INapEnforcementClientConnection * This,
    /* [in] */ ProtocolMaxSize maxSize)
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
        
        
        
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 8;
            NdrProxyGetBuffer(This, &_StubMsg);
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *(( ProtocolMaxSize * )_StubMsg.Buffer)++ = maxSize;
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[38] );
            
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

void __RPC_STUB INapEnforcementClientConnection_SetMaxSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    ProtocolMaxSize maxSize;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    maxSize = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[38] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        
        if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        maxSize = *(( ProtocolMaxSize * )_StubMsg.Buffer)++;
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientConnection*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> SetMaxSize((INapEnforcementClientConnection *) ((CStdStubBuffer *)This)->pvServerObject,maxSize);
        
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


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_GetMaxSize_Proxy( 
    INapEnforcementClientConnection * This,
    /* [out] */ ProtocolMaxSize *maxSize)
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
                      5);
        
        
        
        if(!maxSize)
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[42] );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *maxSize = *(( ProtocolMaxSize * )_StubMsg.Buffer)++;
            
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[42],
                         ( void * )maxSize);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapEnforcementClientConnection_GetMaxSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    ProtocolMaxSize _M3	=	0;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    ProtocolMaxSize *maxSize;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( ProtocolMaxSize * )maxSize = 0;
    RpcTryFinally
        {
        maxSize = &_M3;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientConnection*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetMaxSize((INapEnforcementClientConnection *) ((CStdStubBuffer *)This)->pvServerObject,maxSize);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 36;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( ProtocolMaxSize * )_StubMsg.Buffer)++ = *maxSize;
        
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


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_SetFlags_Proxy( 
    INapEnforcementClientConnection * This,
    /* [in] */ UINT8 flags)
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
        
        
        
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 5;
            NdrProxyGetBuffer(This, &_StubMsg);
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 0) & ~ 0);
            *(( UINT8 * )_StubMsg.Buffer)++ = flags;
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[48] );
            
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

void __RPC_STUB INapEnforcementClientConnection_SetFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    UINT8 flags;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    flags = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[48] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 0) & ~ 0);
        
        if(_StubMsg.Buffer + 1 > _StubMsg.BufferEnd)
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        flags = *(( UINT8 * )_StubMsg.Buffer)++;
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientConnection*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> SetFlags((INapEnforcementClientConnection *) ((CStdStubBuffer *)This)->pvServerObject,flags);
        
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


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_GetFlags_Proxy( 
    INapEnforcementClientConnection * This,
    /* [out] */ UINT8 *flags)
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
        
        
        
        if(!flags)
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[52] );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 0) & ~ 0);
            *flags = *(( UINT8 * )_StubMsg.Buffer)++;
            
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[88],
                         ( void * )flags);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapEnforcementClientConnection_GetFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    UINT8 _M4	=	0;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    UINT8 *flags;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( UINT8 * )flags = 0;
    RpcTryFinally
        {
        flags = &_M4;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientConnection*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetFlags((INapEnforcementClientConnection *) ((CStdStubBuffer *)This)->pvServerObject,flags);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 33;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 0) & ~ 0);
        *(( UINT8 * )_StubMsg.Buffer)++ = *flags;
        
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


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_SetConnectionId_Proxy( 
    INapEnforcementClientConnection * This,
    /* [in] */ const ConnectionId *connectionId)
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
                      8);
        
        
        
        if(!connectionId)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 68;
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                     (unsigned char *)connectionId,
                                     (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[102] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[58] );
            
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

void __RPC_STUB INapEnforcementClientConnection_SetConnectionId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    const ConnectionId *connectionId;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( ConnectionId * )connectionId = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[58] );
        
        NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char * *)&connectionId,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[102],
                                   (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientConnection*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> SetConnectionId((INapEnforcementClientConnection *) ((CStdStubBuffer *)This)->pvServerObject,connectionId);
        
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


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_GetConnectionId_Proxy( 
    INapEnforcementClientConnection * This,
    /* [out] */ ConnectionId **connectionId)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    if(connectionId)
        {
        *connectionId = 0;
        }
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      9);
        
        
        
        if(!connectionId)
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[64] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&connectionId,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[114],
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[114],
                         ( void * )connectionId);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapEnforcementClientConnection_GetConnectionId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    ConnectionId *_M5;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    ConnectionId **connectionId;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( ConnectionId ** )connectionId = 0;
    RpcTryFinally
        {
        connectionId = &_M5;
        _M5 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientConnection*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetConnectionId((INapEnforcementClientConnection *) ((CStdStubBuffer *)This)->pvServerObject,connectionId);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 96;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)connectionId,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[114] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)connectionId,
                        &__MIDL_TypeFormatString.Format[114] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_GetCorrelationId_Proxy( 
    INapEnforcementClientConnection * This,
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
                      10);
        
        
        
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[70] );
            
            NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char * *)&correlationId,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[134],
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[122],
                         ( void * )correlationId);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapEnforcementClientConnection_GetCorrelationId_Stub(
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
        _RetVal = (((INapEnforcementClientConnection*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetCorrelationId((INapEnforcementClientConnection *) ((CStdStubBuffer *)This)->pvServerObject,correlationId);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 116;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                 (unsigned char *)correlationId,
                                 (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[134] );
        
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


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_GetStringCorrelationId_Proxy( 
    INapEnforcementClientConnection * This,
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
                      11);
        
        
        
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[76] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&correlationId,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[148],
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[148],
                         ( void * )correlationId);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapEnforcementClientConnection_GetStringCorrelationId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    StringCorrelationId *_M6;
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
        correlationId = &_M6;
        _M6 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientConnection*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetStringCorrelationId((INapEnforcementClientConnection *) ((CStdStubBuffer *)This)->pvServerObject,correlationId);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)correlationId,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[148] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)correlationId,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[148] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)correlationId,
                        &__MIDL_TypeFormatString.Format[148] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_SetCorrelationId_Proxy( 
    INapEnforcementClientConnection * This,
    /* [in] */ CorrelationId correlationId)
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
                      12);
        
        
        
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 88;
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                     (unsigned char *)&correlationId,
                                     (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[134] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[82] );
            
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

void __RPC_STUB INapEnforcementClientConnection_SetCorrelationId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    void *_p_correlationId;
    CorrelationId correlationId;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    _p_correlationId = &correlationId;
    MIDL_memset(
               _p_correlationId,
               0,
               sizeof( CorrelationId  ));
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[82] );
        
        NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char * *)&_p_correlationId,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[134],
                                   (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientConnection*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> SetCorrelationId((INapEnforcementClientConnection *) ((CStdStubBuffer *)This)->pvServerObject,correlationId);
        
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


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_SetSoHRequest_Proxy( 
    INapEnforcementClientConnection * This,
    /* [unique][in] */ const NetworkSoHRequest *sohRequest)
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
                      13);
        
        
        
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char *)sohRequest,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[184] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                (unsigned char *)sohRequest,
                                (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[184] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[88] );
            
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

void __RPC_STUB INapEnforcementClientConnection_SetSoHRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    const NetworkSoHRequest *sohRequest;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( NetworkSoHRequest * )sohRequest = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[88] );
        
        NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char * *)&sohRequest,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[184],
                              (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientConnection*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> SetSoHRequest((INapEnforcementClientConnection *) ((CStdStubBuffer *)This)->pvServerObject,sohRequest);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)sohRequest,
                        &__MIDL_TypeFormatString.Format[184] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_GetSoHRequest_Proxy( 
    INapEnforcementClientConnection * This,
    /* [out] */ NetworkSoHRequest **sohRequest)
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
                      14);
        
        
        
        if(!sohRequest)
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[94] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&sohRequest,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[220],
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[220],
                         ( void * )sohRequest);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapEnforcementClientConnection_GetSoHRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    NetworkSoHRequest *_M7;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    NetworkSoHRequest **sohRequest;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( NetworkSoHRequest ** )sohRequest = 0;
    RpcTryFinally
        {
        sohRequest = &_M7;
        _M7 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientConnection*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetSoHRequest((INapEnforcementClientConnection *) ((CStdStubBuffer *)This)->pvServerObject,sohRequest);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)sohRequest,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[220] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)sohRequest,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[220] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)sohRequest,
                        &__MIDL_TypeFormatString.Format[220] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_SetSoHResponse_Proxy( 
    INapEnforcementClientConnection * This,
    /* [in] */ const NetworkSoHResponse *sohResponse)
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
                      15);
        
        
        
        if(!sohResponse)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrSimpleStructBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char *)sohResponse,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[198] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                     (unsigned char *)sohResponse,
                                     (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[198] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[100] );
            
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

void __RPC_STUB INapEnforcementClientConnection_SetSoHResponse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    const NetworkSoHResponse *sohResponse;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( NetworkSoHResponse * )sohResponse = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[100] );
        
        NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char * *)&sohResponse,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[198],
                                   (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientConnection*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> SetSoHResponse((INapEnforcementClientConnection *) ((CStdStubBuffer *)This)->pvServerObject,sohResponse);
        
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
                        &__MIDL_TypeFormatString.Format[228] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_GetSoHResponse_Proxy( 
    INapEnforcementClientConnection * This,
    /* [out] */ NetworkSoHResponse **sohResponse)
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
                      16);
        
        
        
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[94] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&sohResponse,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[220],
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[220],
                         ( void * )sohResponse);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapEnforcementClientConnection_GetSoHResponse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    NetworkSoHResponse *_M8;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    NetworkSoHResponse **sohResponse;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( NetworkSoHResponse ** )sohResponse = 0;
    RpcTryFinally
        {
        sohResponse = &_M8;
        _M8 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientConnection*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetSoHResponse((INapEnforcementClientConnection *) ((CStdStubBuffer *)This)->pvServerObject,sohResponse);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)sohResponse,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[220] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)sohResponse,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[220] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)sohResponse,
                        &__MIDL_TypeFormatString.Format[220] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_SetIsolationInfo_Proxy( 
    INapEnforcementClientConnection * This,
    /* [in] */ const IsolationInfo *isolationInfo)
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
                      17);
        
        
        
        if(!isolationInfo)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrComplexStructBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                        (unsigned char *)isolationInfo,
                                        (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[236] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrComplexStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                      (unsigned char *)isolationInfo,
                                      (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[236] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[106] );
            
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

void __RPC_STUB INapEnforcementClientConnection_SetIsolationInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    const IsolationInfo *isolationInfo;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( IsolationInfo * )isolationInfo = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[106] );
        
        NdrComplexStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                    (unsigned char * *)&isolationInfo,
                                    (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[236],
                                    (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientConnection*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> SetIsolationInfo((INapEnforcementClientConnection *) ((CStdStubBuffer *)This)->pvServerObject,isolationInfo);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)isolationInfo,
                        &__MIDL_TypeFormatString.Format[232] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_GetIsolationInfo_Proxy( 
    INapEnforcementClientConnection * This,
    /* [out] */ IsolationInfo **isolationInfo)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    if(isolationInfo)
        {
        *isolationInfo = 0;
        }
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      18);
        
        
        
        if(!isolationInfo)
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[112] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&isolationInfo,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[254],
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[254],
                         ( void * )isolationInfo);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapEnforcementClientConnection_GetIsolationInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    IsolationInfo *_M9;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    IsolationInfo **isolationInfo;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( IsolationInfo ** )isolationInfo = 0;
    RpcTryFinally
        {
        isolationInfo = &_M9;
        _M9 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientConnection*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetIsolationInfo((INapEnforcementClientConnection *) ((CStdStubBuffer *)This)->pvServerObject,isolationInfo);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)isolationInfo,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[254] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)isolationInfo,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[254] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)isolationInfo,
                        &__MIDL_TypeFormatString.Format[254] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_SetPrivateData_Proxy( 
    INapEnforcementClientConnection * This,
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
                      19);
        
        
        
        if(!privateData)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrSimpleStructBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char *)privateData,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[198] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                     (unsigned char *)privateData,
                                     (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[198] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[100] );
            
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

void __RPC_STUB INapEnforcementClientConnection_SetPrivateData_Stub(
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
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[100] );
        
        NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char * *)&privateData,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[198],
                                   (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientConnection*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> SetPrivateData((INapEnforcementClientConnection *) ((CStdStubBuffer *)This)->pvServerObject,privateData);
        
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
                        &__MIDL_TypeFormatString.Format[228] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_GetPrivateData_Proxy( 
    INapEnforcementClientConnection * This,
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
                      20);
        
        
        
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[94] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&privateData,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[220],
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[220],
                         ( void * )privateData);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapEnforcementClientConnection_GetPrivateData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    PrivateData *_M10;
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
        privateData = &_M10;
        _M10 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientConnection*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetPrivateData((INapEnforcementClientConnection *) ((CStdStubBuffer *)This)->pvServerObject,privateData);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)privateData,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[220] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)privateData,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[220] );
        
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


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_SetEnforcerPrivateData_Proxy( 
    INapEnforcementClientConnection * This,
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
                      21);
        
        
        
        if(!privateData)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrSimpleStructBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char *)privateData,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[198] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                     (unsigned char *)privateData,
                                     (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[198] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[100] );
            
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

void __RPC_STUB INapEnforcementClientConnection_SetEnforcerPrivateData_Stub(
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
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[100] );
        
        NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char * *)&privateData,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[198],
                                   (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientConnection*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> SetEnforcerPrivateData((INapEnforcementClientConnection *) ((CStdStubBuffer *)This)->pvServerObject,privateData);
        
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
                        &__MIDL_TypeFormatString.Format[228] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapEnforcementClientConnection_GetEnforcerPrivateData_Proxy( 
    INapEnforcementClientConnection * This,
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
                      22);
        
        
        
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[94] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&privateData,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[220],
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[220],
                         ( void * )privateData);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapEnforcementClientConnection_GetEnforcerPrivateData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    PrivateData *_M11;
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
        privateData = &_M11;
        _M11 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapEnforcementClientConnection*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetEnforcerPrivateData((INapEnforcementClientConnection *) ((CStdStubBuffer *)This)->pvServerObject,privateData);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)privateData,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[220] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)privateData,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[220] );
        
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



#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {
			0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/*  2 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/*  4 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/*  6 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/*  8 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 10 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 12 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */
/* 14 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 16 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 18 */	NdrFcShort( 0x18 ),	/* Type Offset=24 */
/* 20 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 22 */	NdrFcShort( 0x2a ),	/* Type Offset=42 */
/* 24 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 26 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 28 */	NdrFcShort( 0x18 ),	/* Type Offset=24 */
/* 30 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 32 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 34 */	NdrFcShort( 0x2e ),	/* Type Offset=46 */
/* 36 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 38 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 40 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 42 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 44 */	NdrFcShort( 0x2a ),	/* Type Offset=42 */
/* 46 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 48 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x2,		/* FC_CHAR */
/* 50 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 52 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 54 */	NdrFcShort( 0x58 ),	/* Type Offset=88 */
/* 56 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 58 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 60 */	NdrFcShort( 0x5c ),	/* Type Offset=92 */
/* 62 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 64 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 66 */	NdrFcShort( 0x72 ),	/* Type Offset=114 */
/* 68 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 70 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 72 */	NdrFcShort( 0x7a ),	/* Type Offset=122 */
/* 74 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 76 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 78 */	NdrFcShort( 0x94 ),	/* Type Offset=148 */
/* 80 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 82 */	
			0x4d,		/* FC_IN_PARAM */
			0x6,		/* x86 stack size = 6 */
/* 84 */	NdrFcShort( 0x86 ),	/* Type Offset=134 */
/* 86 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 88 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 90 */	NdrFcShort( 0xb8 ),	/* Type Offset=184 */
/* 92 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 94 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 96 */	NdrFcShort( 0xdc ),	/* Type Offset=220 */
/* 98 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 100 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 102 */	NdrFcShort( 0xe4 ),	/* Type Offset=228 */
/* 104 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 106 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 108 */	NdrFcShort( 0xe8 ),	/* Type Offset=232 */
/* 110 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 112 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 114 */	NdrFcShort( 0xfe ),	/* Type Offset=254 */
/* 116 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
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
/*  4 */	NdrFcLong( 0xf5a0b90a ),	/* -174016246 */
/*  8 */	NdrFcShort( 0x83a1 ),	/* -31839 */
/* 10 */	NdrFcShort( 0x4f76 ),	/* 20342 */
/* 12 */	0xba,		/* 186 */
			0x3f,		/* 63 */
/* 14 */	0x2,		/* 2 */
			0x54,		/* 84 */
/* 16 */	0x18,		/* 24 */
			0x68,		/* 104 */
/* 18 */	0x28,		/* 40 */
			0x14,		/* 20 */
/* 20 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 22 */	NdrFcShort( 0x2 ),	/* Offset= 2 (24) */
/* 24 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 26 */	NdrFcLong( 0xfb3a3505 ),	/* -80071419 */
/* 30 */	NdrFcShort( 0xddb1 ),	/* -8783 */
/* 32 */	NdrFcShort( 0x468a ),	/* 18058 */
/* 34 */	0xb3,		/* 179 */
			0x7,		/* 7 */
/* 36 */	0xf3,		/* 243 */
			0x28,		/* 40 */
/* 38 */	0xa5,		/* 165 */
			0x74,		/* 116 */
/* 40 */	0x19,		/* 25 */
			0xd8,		/* 216 */
/* 42 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 44 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 46 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 48 */	NdrFcShort( 0x2 ),	/* Offset= 2 (50) */
/* 50 */	
			0x13, 0x0,	/* FC_OP */
/* 52 */	NdrFcShort( 0x14 ),	/* Offset= 20 (72) */
/* 54 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 56 */	NdrFcShort( 0x0 ),	/* 0 */
/* 58 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 66 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 68 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (24) */
/* 70 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 72 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 74 */	NdrFcShort( 0x8 ),	/* 8 */
/* 76 */	NdrFcShort( 0x0 ),	/* 0 */
/* 78 */	NdrFcShort( 0x6 ),	/* Offset= 6 (84) */
/* 80 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 82 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 84 */	
			0x13, 0x0,	/* FC_OP */
/* 86 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (54) */
/* 88 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 90 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 92 */	
			0x11, 0x0,	/* FC_RP */
/* 94 */	NdrFcShort( 0x8 ),	/* Offset= 8 (102) */
/* 96 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 98 */	NdrFcShort( 0x8 ),	/* 8 */
/* 100 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 102 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 104 */	NdrFcShort( 0x10 ),	/* 16 */
/* 106 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 108 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 110 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (96) */
			0x5b,		/* FC_END */
/* 114 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 116 */	NdrFcShort( 0x2 ),	/* Offset= 2 (118) */
/* 118 */	
			0x13, 0x0,	/* FC_OP */
/* 120 */	NdrFcShort( 0xffffffee ),	/* Offset= -18 (102) */
/* 122 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 124 */	NdrFcShort( 0xa ),	/* Offset= 10 (134) */
/* 126 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 128 */	NdrFcShort( 0x8 ),	/* 8 */
/* 130 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 132 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 134 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 136 */	NdrFcShort( 0x18 ),	/* 24 */
/* 138 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 140 */	NdrFcShort( 0xffffffda ),	/* Offset= -38 (102) */
/* 142 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 144 */	NdrFcShort( 0xffffffee ),	/* Offset= -18 (126) */
/* 146 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 148 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 150 */	NdrFcShort( 0x2 ),	/* Offset= 2 (152) */
/* 152 */	
			0x13, 0x0,	/* FC_OP */
/* 154 */	NdrFcShort( 0x8 ),	/* Offset= 8 (162) */
/* 156 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 158 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x57,		/* FC_ADD_1 */
/* 160 */	NdrFcShort( 0x0 ),	/* 0 */
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
/* 174 */	0x13, 0x0,	/* FC_OP */
/* 176 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (156) */
/* 178 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 180 */	0x3e,		/* FC_STRUCTPAD2 */
			0x8,		/* FC_LONG */
/* 182 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 184 */	
			0x12, 0x0,	/* FC_UP */
/* 186 */	NdrFcShort( 0xc ),	/* Offset= 12 (198) */
/* 188 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 190 */	NdrFcShort( 0x1 ),	/* 1 */
/* 192 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 194 */	NdrFcShort( 0x0 ),	/* 0 */
/* 196 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 198 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 200 */	NdrFcShort( 0x8 ),	/* 8 */
/* 202 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 204 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 206 */	NdrFcShort( 0x4 ),	/* 4 */
/* 208 */	NdrFcShort( 0x4 ),	/* 4 */
/* 210 */	0x12, 0x0,	/* FC_UP */
/* 212 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (188) */
/* 214 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 216 */	0x3e,		/* FC_STRUCTPAD2 */
			0x8,		/* FC_LONG */
/* 218 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 220 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 222 */	NdrFcShort( 0x2 ),	/* Offset= 2 (224) */
/* 224 */	
			0x13, 0x0,	/* FC_OP */
/* 226 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (198) */
/* 228 */	
			0x11, 0x0,	/* FC_RP */
/* 230 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (198) */
/* 232 */	
			0x11, 0x0,	/* FC_RP */
/* 234 */	NdrFcShort( 0x2 ),	/* Offset= 2 (236) */
/* 236 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 238 */	NdrFcShort( 0x14 ),	/* 20 */
/* 240 */	NdrFcShort( 0x0 ),	/* 0 */
/* 242 */	NdrFcShort( 0x0 ),	/* Offset= 0 (242) */
/* 244 */	0xd,		/* FC_ENUM16 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 246 */	0x0,		/* 0 */
			NdrFcShort( 0xffffff87 ),	/* Offset= -121 (126) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 250 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffa7 ),	/* Offset= -89 (162) */
			0x5b,		/* FC_END */
/* 254 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 256 */	NdrFcShort( 0x2 ),	/* Offset= 2 (258) */
/* 258 */	
			0x13, 0x0,	/* FC_OP */
/* 260 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (236) */

			0x0
        }
    };


/* Standard interface: __MIDL_itf_napenforcementclient_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: INapEnforcementClientBinding, ver. 0.0,
   GUID={0x92B93223,0x7487,0x42d9,{0x9A,0x91,0x5B,0x85,0x07,0x72,0x03,0x84}} */

#pragma code_seg(".orpc")
static const unsigned short INapEnforcementClientBinding_FormatStringOffsetTable[] =
    {
    0,
    8,
    10,
    16,
    26,
    26,
    8
    };

static const MIDL_STUBLESS_PROXY_INFO INapEnforcementClientBinding_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapEnforcementClientBinding_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapEnforcementClientBinding_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapEnforcementClientBinding_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
const CINTERFACE_PROXY_VTABLE(10) _INapEnforcementClientBindingProxyVtbl = 
{
    &IID_INapEnforcementClientBinding,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    INapEnforcementClientBinding_Initialize_Proxy ,
    INapEnforcementClientBinding_Uninitialize_Proxy ,
    INapEnforcementClientBinding_CreateConnection_Proxy ,
    INapEnforcementClientBinding_GetSoHRequest_Proxy ,
    INapEnforcementClientBinding_ProcessSoHResponse_Proxy ,
    INapEnforcementClientBinding_NotifyConnectionStateDown_Proxy ,
    INapEnforcementClientBinding_NotifySoHChangeFailure_Proxy
};


static const PRPC_STUB_FUNCTION INapEnforcementClientBinding_table[] =
{
    INapEnforcementClientBinding_Initialize_Stub,
    INapEnforcementClientBinding_Uninitialize_Stub,
    INapEnforcementClientBinding_CreateConnection_Stub,
    INapEnforcementClientBinding_GetSoHRequest_Stub,
    INapEnforcementClientBinding_ProcessSoHResponse_Stub,
    INapEnforcementClientBinding_NotifyConnectionStateDown_Stub,
    INapEnforcementClientBinding_NotifySoHChangeFailure_Stub
};

const CInterfaceStubVtbl _INapEnforcementClientBindingStubVtbl =
{
    &IID_INapEnforcementClientBinding,
    &INapEnforcementClientBinding_ServerInfo,
    10,
    &INapEnforcementClientBinding_table[-3],
    CStdStubBuffer_METHODS
};


/* Object interface: INapEnforcementClientCallback, ver. 0.0,
   GUID={0xF5A0B90A,0x83A1,0x4f76,{0xBA,0x3F,0x02,0x54,0x18,0x68,0x28,0x14}} */

#pragma code_seg(".orpc")
static const unsigned short INapEnforcementClientCallback_FormatStringOffsetTable[] =
    {
    8,
    32
    };

static const MIDL_STUBLESS_PROXY_INFO INapEnforcementClientCallback_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapEnforcementClientCallback_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapEnforcementClientCallback_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapEnforcementClientCallback_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
const CINTERFACE_PROXY_VTABLE(5) _INapEnforcementClientCallbackProxyVtbl = 
{
    &IID_INapEnforcementClientCallback,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    INapEnforcementClientCallback_NotifySoHChange_Proxy ,
    INapEnforcementClientCallback_GetConnections_Proxy
};


static const PRPC_STUB_FUNCTION INapEnforcementClientCallback_table[] =
{
    INapEnforcementClientCallback_NotifySoHChange_Stub,
    INapEnforcementClientCallback_GetConnections_Stub
};

const CInterfaceStubVtbl _INapEnforcementClientCallbackStubVtbl =
{
    &IID_INapEnforcementClientCallback,
    &INapEnforcementClientCallback_ServerInfo,
    5,
    &INapEnforcementClientCallback_table[-3],
    CStdStubBuffer_METHODS
};


/* Object interface: INapEnforcementClientConnection, ver. 0.0,
   GUID={0xFB3A3505,0xDDB1,0x468a,{0xB3,0x07,0xF3,0x28,0xA5,0x74,0x19,0xD8}} */

#pragma code_seg(".orpc")
static const unsigned short INapEnforcementClientConnection_FormatStringOffsetTable[] =
    {
    38,
    38,
    42,
    48,
    52,
    58,
    64,
    70,
    76,
    82,
    88,
    94,
    100,
    94,
    106,
    112,
    100,
    94,
    100,
    94
    };

static const MIDL_STUBLESS_PROXY_INFO INapEnforcementClientConnection_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapEnforcementClientConnection_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapEnforcementClientConnection_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapEnforcementClientConnection_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
const CINTERFACE_PROXY_VTABLE(23) _INapEnforcementClientConnectionProxyVtbl = 
{
    &IID_INapEnforcementClientConnection,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    INapEnforcementClientConnection_Initialize_Proxy ,
    INapEnforcementClientConnection_SetMaxSize_Proxy ,
    INapEnforcementClientConnection_GetMaxSize_Proxy ,
    INapEnforcementClientConnection_SetFlags_Proxy ,
    INapEnforcementClientConnection_GetFlags_Proxy ,
    INapEnforcementClientConnection_SetConnectionId_Proxy ,
    INapEnforcementClientConnection_GetConnectionId_Proxy ,
    INapEnforcementClientConnection_GetCorrelationId_Proxy ,
    INapEnforcementClientConnection_GetStringCorrelationId_Proxy ,
    INapEnforcementClientConnection_SetCorrelationId_Proxy ,
    INapEnforcementClientConnection_SetSoHRequest_Proxy ,
    INapEnforcementClientConnection_GetSoHRequest_Proxy ,
    INapEnforcementClientConnection_SetSoHResponse_Proxy ,
    INapEnforcementClientConnection_GetSoHResponse_Proxy ,
    INapEnforcementClientConnection_SetIsolationInfo_Proxy ,
    INapEnforcementClientConnection_GetIsolationInfo_Proxy ,
    INapEnforcementClientConnection_SetPrivateData_Proxy ,
    INapEnforcementClientConnection_GetPrivateData_Proxy ,
    INapEnforcementClientConnection_SetEnforcerPrivateData_Proxy ,
    INapEnforcementClientConnection_GetEnforcerPrivateData_Proxy
};


static const PRPC_STUB_FUNCTION INapEnforcementClientConnection_table[] =
{
    INapEnforcementClientConnection_Initialize_Stub,
    INapEnforcementClientConnection_SetMaxSize_Stub,
    INapEnforcementClientConnection_GetMaxSize_Stub,
    INapEnforcementClientConnection_SetFlags_Stub,
    INapEnforcementClientConnection_GetFlags_Stub,
    INapEnforcementClientConnection_SetConnectionId_Stub,
    INapEnforcementClientConnection_GetConnectionId_Stub,
    INapEnforcementClientConnection_GetCorrelationId_Stub,
    INapEnforcementClientConnection_GetStringCorrelationId_Stub,
    INapEnforcementClientConnection_SetCorrelationId_Stub,
    INapEnforcementClientConnection_SetSoHRequest_Stub,
    INapEnforcementClientConnection_GetSoHRequest_Stub,
    INapEnforcementClientConnection_SetSoHResponse_Stub,
    INapEnforcementClientConnection_GetSoHResponse_Stub,
    INapEnforcementClientConnection_SetIsolationInfo_Stub,
    INapEnforcementClientConnection_GetIsolationInfo_Stub,
    INapEnforcementClientConnection_SetPrivateData_Stub,
    INapEnforcementClientConnection_GetPrivateData_Stub,
    INapEnforcementClientConnection_SetEnforcerPrivateData_Stub,
    INapEnforcementClientConnection_GetEnforcerPrivateData_Stub
};

const CInterfaceStubVtbl _INapEnforcementClientConnectionStubVtbl =
{
    &IID_INapEnforcementClientConnection,
    &INapEnforcementClientConnection_ServerInfo,
    23,
    &INapEnforcementClientConnection_table[-3],
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_napenforcementclient_0257, ver. 0.0,
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

const CInterfaceProxyVtbl * _napenforcementclient_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_INapEnforcementClientConnectionProxyVtbl,
    ( CInterfaceProxyVtbl *) &_INapEnforcementClientCallbackProxyVtbl,
    ( CInterfaceProxyVtbl *) &_INapEnforcementClientBindingProxyVtbl,
    0
};

const CInterfaceStubVtbl * _napenforcementclient_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_INapEnforcementClientConnectionStubVtbl,
    ( CInterfaceStubVtbl *) &_INapEnforcementClientCallbackStubVtbl,
    ( CInterfaceStubVtbl *) &_INapEnforcementClientBindingStubVtbl,
    0
};

PCInterfaceName const _napenforcementclient_InterfaceNamesList[] = 
{
    "INapEnforcementClientConnection",
    "INapEnforcementClientCallback",
    "INapEnforcementClientBinding",
    0
};


#define _napenforcementclient_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _napenforcementclient, pIID, n)

int __stdcall _napenforcementclient_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _napenforcementclient, 3, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _napenforcementclient, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _napenforcementclient, 3, *pIndex )
    
}

const ExtendedProxyFileInfo napenforcementclient_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _napenforcementclient_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _napenforcementclient_StubVtblList,
    (const PCInterfaceName * ) & _napenforcementclient_InterfaceNamesList,
    0, // no delegation
    & _napenforcementclient_IID_Lookup, 
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
/* at Tue Oct 30 15:55:03 2007
 */
/* Compiler settings for O:\CS AdminKit\development2\nap\nap_sdk\include\napenforcementclient.idl:
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


#include "napenforcementclient.h"

#define TYPE_FORMAT_STRING_SIZE   367                               
#define PROC_FORMAT_STRING_SIZE   1097                              
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


extern const MIDL_SERVER_INFO INapEnforcementClientBinding_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapEnforcementClientBinding_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO INapEnforcementClientCallback_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapEnforcementClientCallback_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO INapEnforcementClientConnection_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapEnforcementClientConnection_ProxyInfo;



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
/* 10 */	NdrFcShort( 0x8 ),	/* 8 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
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

	/* Parameter callback */

/* 32 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 34 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 36 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 38 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 40 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 42 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Uninitialize */

/* 44 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 46 */	NdrFcLong( 0x0 ),	/* 0 */
/* 50 */	NdrFcShort( 0x4 ),	/* 4 */
/* 52 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 54 */	NdrFcShort( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x8 ),	/* 8 */
/* 58 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 60 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 62 */	NdrFcShort( 0x0 ),	/* 0 */
/* 64 */	NdrFcShort( 0x0 ),	/* 0 */
/* 66 */	NdrFcShort( 0x0 ),	/* 0 */
/* 68 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 70 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 72 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 74 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateConnection */

/* 76 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 78 */	NdrFcLong( 0x0 ),	/* 0 */
/* 82 */	NdrFcShort( 0x5 ),	/* 5 */
/* 84 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 86 */	NdrFcShort( 0x0 ),	/* 0 */
/* 88 */	NdrFcShort( 0x8 ),	/* 8 */
/* 90 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 92 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 94 */	NdrFcShort( 0x0 ),	/* 0 */
/* 96 */	NdrFcShort( 0x0 ),	/* 0 */
/* 98 */	NdrFcShort( 0x0 ),	/* 0 */
/* 100 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter connection */

/* 102 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 104 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 106 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */

	/* Return value */

/* 108 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 110 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 112 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSoHRequest */

/* 114 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 116 */	NdrFcLong( 0x0 ),	/* 0 */
/* 120 */	NdrFcShort( 0x6 ),	/* 6 */
/* 122 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */
/* 126 */	NdrFcShort( 0x24 ),	/* 36 */
/* 128 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 130 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 132 */	NdrFcShort( 0x0 ),	/* 0 */
/* 134 */	NdrFcShort( 0x0 ),	/* 0 */
/* 136 */	NdrFcShort( 0x0 ),	/* 0 */
/* 138 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter connection */

/* 140 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 142 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 144 */	NdrFcShort( 0x18 ),	/* Type Offset=24 */

	/* Parameter retriggerHint */

/* 146 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 148 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 150 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 152 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 154 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 156 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ProcessSoHResponse */

/* 158 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 160 */	NdrFcLong( 0x0 ),	/* 0 */
/* 164 */	NdrFcShort( 0x7 ),	/* 7 */
/* 166 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 168 */	NdrFcShort( 0x0 ),	/* 0 */
/* 170 */	NdrFcShort( 0x8 ),	/* 8 */
/* 172 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 174 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 176 */	NdrFcShort( 0x0 ),	/* 0 */
/* 178 */	NdrFcShort( 0x0 ),	/* 0 */
/* 180 */	NdrFcShort( 0x0 ),	/* 0 */
/* 182 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter connection */

/* 184 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 186 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 188 */	NdrFcShort( 0x18 ),	/* Type Offset=24 */

	/* Return value */

/* 190 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 192 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 194 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NotifyConnectionStateDown */

/* 196 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 198 */	NdrFcLong( 0x0 ),	/* 0 */
/* 202 */	NdrFcShort( 0x8 ),	/* 8 */
/* 204 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 206 */	NdrFcShort( 0x0 ),	/* 0 */
/* 208 */	NdrFcShort( 0x8 ),	/* 8 */
/* 210 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 212 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 214 */	NdrFcShort( 0x0 ),	/* 0 */
/* 216 */	NdrFcShort( 0x0 ),	/* 0 */
/* 218 */	NdrFcShort( 0x0 ),	/* 0 */
/* 220 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter downCxn */

/* 222 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 224 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 226 */	NdrFcShort( 0x18 ),	/* Type Offset=24 */

	/* Return value */

/* 228 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 230 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 232 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NotifySoHChangeFailure */

/* 234 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 236 */	NdrFcLong( 0x0 ),	/* 0 */
/* 240 */	NdrFcShort( 0x9 ),	/* 9 */
/* 242 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 244 */	NdrFcShort( 0x0 ),	/* 0 */
/* 246 */	NdrFcShort( 0x8 ),	/* 8 */
/* 248 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 250 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 252 */	NdrFcShort( 0x0 ),	/* 0 */
/* 254 */	NdrFcShort( 0x0 ),	/* 0 */
/* 256 */	NdrFcShort( 0x0 ),	/* 0 */
/* 258 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 260 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 262 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 264 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NotifySoHChange */

/* 266 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 268 */	NdrFcLong( 0x0 ),	/* 0 */
/* 272 */	NdrFcShort( 0x3 ),	/* 3 */
/* 274 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 276 */	NdrFcShort( 0x0 ),	/* 0 */
/* 278 */	NdrFcShort( 0x8 ),	/* 8 */
/* 280 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 282 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 284 */	NdrFcShort( 0x0 ),	/* 0 */
/* 286 */	NdrFcShort( 0x0 ),	/* 0 */
/* 288 */	NdrFcShort( 0x0 ),	/* 0 */
/* 290 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 292 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 294 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 296 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetConnections */

/* 298 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 300 */	NdrFcLong( 0x0 ),	/* 0 */
/* 304 */	NdrFcShort( 0x4 ),	/* 4 */
/* 306 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 308 */	NdrFcShort( 0x0 ),	/* 0 */
/* 310 */	NdrFcShort( 0x8 ),	/* 8 */
/* 312 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 314 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 316 */	NdrFcShort( 0x1 ),	/* 1 */
/* 318 */	NdrFcShort( 0x0 ),	/* 0 */
/* 320 */	NdrFcShort( 0x0 ),	/* 0 */
/* 322 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter connections */

/* 324 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 326 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 328 */	NdrFcShort( 0x2e ),	/* Type Offset=46 */

	/* Return value */

/* 330 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 332 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 334 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Initialize */

/* 336 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 338 */	NdrFcLong( 0x0 ),	/* 0 */
/* 342 */	NdrFcShort( 0x3 ),	/* 3 */
/* 344 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 346 */	NdrFcShort( 0x8 ),	/* 8 */
/* 348 */	NdrFcShort( 0x8 ),	/* 8 */
/* 350 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 352 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 354 */	NdrFcShort( 0x0 ),	/* 0 */
/* 356 */	NdrFcShort( 0x0 ),	/* 0 */
/* 358 */	NdrFcShort( 0x0 ),	/* 0 */
/* 360 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter id */

/* 362 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 364 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 366 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 368 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 370 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 372 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetMaxSize */

/* 374 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 376 */	NdrFcLong( 0x0 ),	/* 0 */
/* 380 */	NdrFcShort( 0x4 ),	/* 4 */
/* 382 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 384 */	NdrFcShort( 0x8 ),	/* 8 */
/* 386 */	NdrFcShort( 0x8 ),	/* 8 */
/* 388 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 390 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 392 */	NdrFcShort( 0x0 ),	/* 0 */
/* 394 */	NdrFcShort( 0x0 ),	/* 0 */
/* 396 */	NdrFcShort( 0x0 ),	/* 0 */
/* 398 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter maxSize */

/* 400 */	NdrFcShort( 0x88 ),	/* Flags:  in, by val, */
/* 402 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 404 */	NdrFcShort( 0x6a ),	/* 106 */

	/* Return value */

/* 406 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 408 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 410 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMaxSize */

/* 412 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 414 */	NdrFcLong( 0x0 ),	/* 0 */
/* 418 */	NdrFcShort( 0x5 ),	/* 5 */
/* 420 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 422 */	NdrFcShort( 0x0 ),	/* 0 */
/* 424 */	NdrFcShort( 0x24 ),	/* 36 */
/* 426 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 428 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 430 */	NdrFcShort( 0x0 ),	/* 0 */
/* 432 */	NdrFcShort( 0x0 ),	/* 0 */
/* 434 */	NdrFcShort( 0x0 ),	/* 0 */
/* 436 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter maxSize */

/* 438 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 440 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 442 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 444 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 446 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 448 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetFlags */

/* 450 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 452 */	NdrFcLong( 0x0 ),	/* 0 */
/* 456 */	NdrFcShort( 0x6 ),	/* 6 */
/* 458 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 460 */	NdrFcShort( 0x5 ),	/* 5 */
/* 462 */	NdrFcShort( 0x8 ),	/* 8 */
/* 464 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 466 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 468 */	NdrFcShort( 0x0 ),	/* 0 */
/* 470 */	NdrFcShort( 0x0 ),	/* 0 */
/* 472 */	NdrFcShort( 0x0 ),	/* 0 */
/* 474 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter flags */

/* 476 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 478 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 480 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Return value */

/* 482 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 484 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 486 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFlags */

/* 488 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 490 */	NdrFcLong( 0x0 ),	/* 0 */
/* 494 */	NdrFcShort( 0x7 ),	/* 7 */
/* 496 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 498 */	NdrFcShort( 0x0 ),	/* 0 */
/* 500 */	NdrFcShort( 0x21 ),	/* 33 */
/* 502 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 504 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 506 */	NdrFcShort( 0x0 ),	/* 0 */
/* 508 */	NdrFcShort( 0x0 ),	/* 0 */
/* 510 */	NdrFcShort( 0x0 ),	/* 0 */
/* 512 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter flags */

/* 514 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 516 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 518 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Return value */

/* 520 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 522 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 524 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetConnectionId */

/* 526 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 528 */	NdrFcLong( 0x0 ),	/* 0 */
/* 532 */	NdrFcShort( 0x8 ),	/* 8 */
/* 534 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 536 */	NdrFcShort( 0x44 ),	/* 68 */
/* 538 */	NdrFcShort( 0x8 ),	/* 8 */
/* 540 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 542 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 544 */	NdrFcShort( 0x0 ),	/* 0 */
/* 546 */	NdrFcShort( 0x0 ),	/* 0 */
/* 548 */	NdrFcShort( 0x0 ),	/* 0 */
/* 550 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter connectionId */

/* 552 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 554 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 556 */	NdrFcShort( 0x8e ),	/* Type Offset=142 */

	/* Return value */

/* 558 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 560 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 562 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetConnectionId */

/* 564 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 566 */	NdrFcLong( 0x0 ),	/* 0 */
/* 570 */	NdrFcShort( 0x9 ),	/* 9 */
/* 572 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 574 */	NdrFcShort( 0x0 ),	/* 0 */
/* 576 */	NdrFcShort( 0x60 ),	/* 96 */
/* 578 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 580 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 582 */	NdrFcShort( 0x0 ),	/* 0 */
/* 584 */	NdrFcShort( 0x0 ),	/* 0 */
/* 586 */	NdrFcShort( 0x0 ),	/* 0 */
/* 588 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter connectionId */

/* 590 */	NdrFcShort( 0x2012 ),	/* Flags:  must free, out, srv alloc size=8 */
/* 592 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 594 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Return value */

/* 596 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 598 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 600 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCorrelationId */

/* 602 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 604 */	NdrFcLong( 0x0 ),	/* 0 */
/* 608 */	NdrFcShort( 0xa ),	/* 10 */
/* 610 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 612 */	NdrFcShort( 0x0 ),	/* 0 */
/* 614 */	NdrFcShort( 0x74 ),	/* 116 */
/* 616 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 618 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 620 */	NdrFcShort( 0x0 ),	/* 0 */
/* 622 */	NdrFcShort( 0x0 ),	/* 0 */
/* 624 */	NdrFcShort( 0x0 ),	/* 0 */
/* 626 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter correlationId */

/* 628 */	NdrFcShort( 0x6112 ),	/* Flags:  must free, out, simple ref, srv alloc size=24 */
/* 630 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 632 */	NdrFcShort( 0xae ),	/* Type Offset=174 */

	/* Return value */

/* 634 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 636 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 638 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetStringCorrelationId */

/* 640 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 642 */	NdrFcLong( 0x0 ),	/* 0 */
/* 646 */	NdrFcShort( 0xb ),	/* 11 */
/* 648 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 650 */	NdrFcShort( 0x0 ),	/* 0 */
/* 652 */	NdrFcShort( 0x8 ),	/* 8 */
/* 654 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 656 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 658 */	NdrFcShort( 0x1 ),	/* 1 */
/* 660 */	NdrFcShort( 0x0 ),	/* 0 */
/* 662 */	NdrFcShort( 0x0 ),	/* 0 */
/* 664 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter correlationId */

/* 666 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 668 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 670 */	NdrFcShort( 0xbc ),	/* Type Offset=188 */

	/* Return value */

/* 672 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 674 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 676 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetCorrelationId */

/* 678 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 680 */	NdrFcLong( 0x0 ),	/* 0 */
/* 684 */	NdrFcShort( 0xc ),	/* 12 */
/* 686 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 688 */	NdrFcShort( 0x58 ),	/* 88 */
/* 690 */	NdrFcShort( 0x8 ),	/* 8 */
/* 692 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 694 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 696 */	NdrFcShort( 0x0 ),	/* 0 */
/* 698 */	NdrFcShort( 0x0 ),	/* 0 */
/* 700 */	NdrFcShort( 0x0 ),	/* 0 */
/* 702 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter correlationId */

/* 704 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 706 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 708 */	NdrFcShort( 0xae ),	/* Type Offset=174 */

	/* Return value */

/* 710 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 712 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 714 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSoHRequest */

/* 716 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 718 */	NdrFcLong( 0x0 ),	/* 0 */
/* 722 */	NdrFcShort( 0xd ),	/* 13 */
/* 724 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 726 */	NdrFcShort( 0x0 ),	/* 0 */
/* 728 */	NdrFcShort( 0x8 ),	/* 8 */
/* 730 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 732 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 734 */	NdrFcShort( 0x0 ),	/* 0 */
/* 736 */	NdrFcShort( 0x1 ),	/* 1 */
/* 738 */	NdrFcShort( 0x0 ),	/* 0 */
/* 740 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter sohRequest */

/* 742 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 744 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 746 */	NdrFcShort( 0xea ),	/* Type Offset=234 */

	/* Return value */

/* 748 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 750 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 752 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSoHRequest */

/* 754 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 756 */	NdrFcLong( 0x0 ),	/* 0 */
/* 760 */	NdrFcShort( 0xe ),	/* 14 */
/* 762 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 764 */	NdrFcShort( 0x0 ),	/* 0 */
/* 766 */	NdrFcShort( 0x8 ),	/* 8 */
/* 768 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 770 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 772 */	NdrFcShort( 0x1 ),	/* 1 */
/* 774 */	NdrFcShort( 0x0 ),	/* 0 */
/* 776 */	NdrFcShort( 0x0 ),	/* 0 */
/* 778 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter sohRequest */

/* 780 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 782 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 784 */	NdrFcShort( 0x118 ),	/* Type Offset=280 */

	/* Return value */

/* 786 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 788 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 790 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSoHResponse */

/* 792 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 794 */	NdrFcLong( 0x0 ),	/* 0 */
/* 798 */	NdrFcShort( 0xf ),	/* 15 */
/* 800 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 802 */	NdrFcShort( 0x0 ),	/* 0 */
/* 804 */	NdrFcShort( 0x8 ),	/* 8 */
/* 806 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 808 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 810 */	NdrFcShort( 0x0 ),	/* 0 */
/* 812 */	NdrFcShort( 0x1 ),	/* 1 */
/* 814 */	NdrFcShort( 0x0 ),	/* 0 */
/* 816 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter sohResponse */

/* 818 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 820 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 822 */	NdrFcShort( 0x104 ),	/* Type Offset=260 */

	/* Return value */

/* 824 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 826 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 828 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSoHResponse */

/* 830 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 832 */	NdrFcLong( 0x0 ),	/* 0 */
/* 836 */	NdrFcShort( 0x10 ),	/* 16 */
/* 838 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 840 */	NdrFcShort( 0x0 ),	/* 0 */
/* 842 */	NdrFcShort( 0x8 ),	/* 8 */
/* 844 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 846 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 848 */	NdrFcShort( 0x1 ),	/* 1 */
/* 850 */	NdrFcShort( 0x0 ),	/* 0 */
/* 852 */	NdrFcShort( 0x0 ),	/* 0 */
/* 854 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter sohResponse */

/* 856 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 858 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 860 */	NdrFcShort( 0x118 ),	/* Type Offset=280 */

	/* Return value */

/* 862 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 864 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 866 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetIsolationInfo */

/* 868 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 870 */	NdrFcLong( 0x0 ),	/* 0 */
/* 874 */	NdrFcShort( 0x11 ),	/* 17 */
/* 876 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 878 */	NdrFcShort( 0x0 ),	/* 0 */
/* 880 */	NdrFcShort( 0x8 ),	/* 8 */
/* 882 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 884 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 886 */	NdrFcShort( 0x0 ),	/* 0 */
/* 888 */	NdrFcShort( 0x1 ),	/* 1 */
/* 890 */	NdrFcShort( 0x0 ),	/* 0 */
/* 892 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter isolationInfo */

/* 894 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 896 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 898 */	NdrFcShort( 0x128 ),	/* Type Offset=296 */

	/* Return value */

/* 900 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 902 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 904 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetIsolationInfo */

/* 906 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 908 */	NdrFcLong( 0x0 ),	/* 0 */
/* 912 */	NdrFcShort( 0x12 ),	/* 18 */
/* 914 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 916 */	NdrFcShort( 0x0 ),	/* 0 */
/* 918 */	NdrFcShort( 0x8 ),	/* 8 */
/* 920 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 922 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 924 */	NdrFcShort( 0x1 ),	/* 1 */
/* 926 */	NdrFcShort( 0x0 ),	/* 0 */
/* 928 */	NdrFcShort( 0x0 ),	/* 0 */
/* 930 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter isolationInfo */

/* 932 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 934 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 936 */	NdrFcShort( 0x13c ),	/* Type Offset=316 */

	/* Return value */

/* 938 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 940 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 942 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetPrivateData */

/* 944 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 946 */	NdrFcLong( 0x0 ),	/* 0 */
/* 950 */	NdrFcShort( 0x13 ),	/* 19 */
/* 952 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 954 */	NdrFcShort( 0x0 ),	/* 0 */
/* 956 */	NdrFcShort( 0x8 ),	/* 8 */
/* 958 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 960 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 962 */	NdrFcShort( 0x0 ),	/* 0 */
/* 964 */	NdrFcShort( 0x1 ),	/* 1 */
/* 966 */	NdrFcShort( 0x0 ),	/* 0 */
/* 968 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter privateData */

/* 970 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 972 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 974 */	NdrFcShort( 0x152 ),	/* Type Offset=338 */

	/* Return value */

/* 976 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 978 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 980 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPrivateData */

/* 982 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 984 */	NdrFcLong( 0x0 ),	/* 0 */
/* 988 */	NdrFcShort( 0x14 ),	/* 20 */
/* 990 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 992 */	NdrFcShort( 0x0 ),	/* 0 */
/* 994 */	NdrFcShort( 0x8 ),	/* 8 */
/* 996 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 998 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1000 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1002 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1004 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1006 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter privateData */

/* 1008 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 1010 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1012 */	NdrFcShort( 0x166 ),	/* Type Offset=358 */

	/* Return value */

/* 1014 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1016 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1018 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetEnforcerPrivateData */

/* 1020 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1022 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1026 */	NdrFcShort( 0x15 ),	/* 21 */
/* 1028 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1030 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1032 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1034 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 1036 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1038 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1040 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1042 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1044 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter privateData */

/* 1046 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1048 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1050 */	NdrFcShort( 0x152 ),	/* Type Offset=338 */

	/* Return value */

/* 1052 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1054 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1056 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetEnforcerPrivateData */

/* 1058 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1060 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1064 */	NdrFcShort( 0x16 ),	/* 22 */
/* 1066 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 1068 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1070 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1072 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 1074 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 1076 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1078 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1080 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1082 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter privateData */

/* 1084 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 1086 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 1088 */	NdrFcShort( 0x166 ),	/* Type Offset=358 */

	/* Return value */

/* 1090 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1092 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 1094 */	0x8,		/* FC_LONG */
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
/*  4 */	NdrFcLong( 0xf5a0b90a ),	/* -174016246 */
/*  8 */	NdrFcShort( 0x83a1 ),	/* -31839 */
/* 10 */	NdrFcShort( 0x4f76 ),	/* 20342 */
/* 12 */	0xba,		/* 186 */
			0x3f,		/* 63 */
/* 14 */	0x2,		/* 2 */
			0x54,		/* 84 */
/* 16 */	0x18,		/* 24 */
			0x68,		/* 104 */
/* 18 */	0x28,		/* 40 */
			0x14,		/* 20 */
/* 20 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 22 */	NdrFcShort( 0x2 ),	/* Offset= 2 (24) */
/* 24 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 26 */	NdrFcLong( 0xfb3a3505 ),	/* -80071419 */
/* 30 */	NdrFcShort( 0xddb1 ),	/* -8783 */
/* 32 */	NdrFcShort( 0x468a ),	/* 18058 */
/* 34 */	0xb3,		/* 179 */
			0x7,		/* 7 */
/* 36 */	0xf3,		/* 243 */
			0x28,		/* 40 */
/* 38 */	0xa5,		/* 165 */
			0x74,		/* 116 */
/* 40 */	0x19,		/* 25 */
			0xd8,		/* 216 */
/* 42 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 44 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 46 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 48 */	NdrFcShort( 0x2 ),	/* Offset= 2 (50) */
/* 50 */	
			0x13, 0x0,	/* FC_OP */
/* 52 */	NdrFcShort( 0x22 ),	/* Offset= 34 (86) */
/* 54 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 56 */	NdrFcLong( 0x0 ),	/* 0 */
/* 60 */	NdrFcLong( 0x14 ),	/* 20 */
/* 64 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 66 */	NdrFcShort( 0x0 ),	/* 0 */
/* 68 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 70 */	NdrFcShort( 0x0 ),	/* 0 */
/* 72 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 74 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 78 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 80 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 82 */	NdrFcShort( 0xffffffc6 ),	/* Offset= -58 (24) */
/* 84 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 86 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 88 */	NdrFcShort( 0x10 ),	/* 16 */
/* 90 */	NdrFcShort( 0x0 ),	/* 0 */
/* 92 */	NdrFcShort( 0xa ),	/* Offset= 10 (102) */
/* 94 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 96 */	NdrFcShort( 0xffffffd6 ),	/* Offset= -42 (54) */
/* 98 */	0x42,		/* FC_STRUCTPAD6 */
			0x36,		/* FC_POINTER */
/* 100 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 102 */	
			0x13, 0x0,	/* FC_OP */
/* 104 */	NdrFcShort( 0xffffffd8 ),	/* Offset= -40 (64) */
/* 106 */	0xb7,		/* FC_RANGE */
			0x8,		/* 8 */
/* 108 */	NdrFcLong( 0x12c ),	/* 300 */
/* 112 */	NdrFcLong( 0xffff ),	/* 65535 */
/* 116 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 118 */	0xb7,		/* FC_RANGE */
			0x8,		/* 8 */
/* 120 */	NdrFcLong( 0x12c ),	/* 300 */
/* 124 */	NdrFcLong( 0xffff ),	/* 65535 */
/* 128 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 130 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 132 */	
			0x11, 0x0,	/* FC_RP */
/* 134 */	NdrFcShort( 0x8 ),	/* Offset= 8 (142) */
/* 136 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 138 */	NdrFcShort( 0x8 ),	/* 8 */
/* 140 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 142 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 144 */	NdrFcShort( 0x10 ),	/* 16 */
/* 146 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 148 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 150 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (136) */
			0x5b,		/* FC_END */
/* 154 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 156 */	NdrFcShort( 0x2 ),	/* Offset= 2 (158) */
/* 158 */	
			0x13, 0x0,	/* FC_OP */
/* 160 */	NdrFcShort( 0xffffffee ),	/* Offset= -18 (142) */
/* 162 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 164 */	NdrFcShort( 0xa ),	/* Offset= 10 (174) */
/* 166 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 168 */	NdrFcShort( 0x8 ),	/* 8 */
/* 170 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 172 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 174 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 176 */	NdrFcShort( 0x18 ),	/* 24 */
/* 178 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 180 */	NdrFcShort( 0xffffffda ),	/* Offset= -38 (142) */
/* 182 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 184 */	NdrFcShort( 0xffffffee ),	/* Offset= -18 (166) */
/* 186 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 188 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 190 */	NdrFcShort( 0x2 ),	/* Offset= 2 (192) */
/* 192 */	
			0x13, 0x0,	/* FC_OP */
/* 194 */	NdrFcShort( 0x14 ),	/* Offset= 20 (214) */
/* 196 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 198 */	NdrFcLong( 0x0 ),	/* 0 */
/* 202 */	NdrFcLong( 0x400 ),	/* 1024 */
/* 206 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 208 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x57,		/* FC_ADD_1 */
/* 210 */	NdrFcShort( 0x0 ),	/* 0 */
/* 212 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 214 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 216 */	NdrFcShort( 0x10 ),	/* 16 */
/* 218 */	NdrFcShort( 0x0 ),	/* 0 */
/* 220 */	NdrFcShort( 0xa ),	/* Offset= 10 (230) */
/* 222 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 224 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (196) */
/* 226 */	0x42,		/* FC_STRUCTPAD6 */
			0x36,		/* FC_POINTER */
/* 228 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 230 */	
			0x13, 0x0,	/* FC_OP */
/* 232 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (206) */
/* 234 */	
			0x12, 0x0,	/* FC_UP */
/* 236 */	NdrFcShort( 0x18 ),	/* Offset= 24 (260) */
/* 238 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 240 */	NdrFcLong( 0xc ),	/* 12 */
/* 244 */	NdrFcLong( 0xfa0 ),	/* 4000 */
/* 248 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 250 */	NdrFcShort( 0x1 ),	/* 1 */
/* 252 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 254 */	NdrFcShort( 0x0 ),	/* 0 */
/* 256 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 258 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 260 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 262 */	NdrFcShort( 0x10 ),	/* 16 */
/* 264 */	NdrFcShort( 0x0 ),	/* 0 */
/* 266 */	NdrFcShort( 0xa ),	/* Offset= 10 (276) */
/* 268 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 270 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (238) */
/* 272 */	0x42,		/* FC_STRUCTPAD6 */
			0x36,		/* FC_POINTER */
/* 274 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 276 */	
			0x12, 0x0,	/* FC_UP */
/* 278 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (248) */
/* 280 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 282 */	NdrFcShort( 0x2 ),	/* Offset= 2 (284) */
/* 284 */	
			0x13, 0x0,	/* FC_OP */
/* 286 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (260) */
/* 288 */	
			0x11, 0x0,	/* FC_RP */
/* 290 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (260) */
/* 292 */	
			0x11, 0x0,	/* FC_RP */
/* 294 */	NdrFcShort( 0x2 ),	/* Offset= 2 (296) */
/* 296 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 298 */	NdrFcShort( 0x20 ),	/* 32 */
/* 300 */	NdrFcShort( 0x0 ),	/* 0 */
/* 302 */	NdrFcShort( 0x0 ),	/* Offset= 0 (302) */
/* 304 */	0xd,		/* FC_ENUM16 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 306 */	0x0,		/* 0 */
			NdrFcShort( 0xffffff73 ),	/* Offset= -141 (166) */
			0x40,		/* FC_STRUCTPAD4 */
/* 310 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 312 */	NdrFcShort( 0xffffff9e ),	/* Offset= -98 (214) */
/* 314 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 316 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 318 */	NdrFcShort( 0x2 ),	/* Offset= 2 (320) */
/* 320 */	
			0x13, 0x0,	/* FC_OP */
/* 322 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (296) */
/* 324 */	
			0x11, 0x0,	/* FC_RP */
/* 326 */	NdrFcShort( 0xc ),	/* Offset= 12 (338) */
/* 328 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 330 */	NdrFcLong( 0x0 ),	/* 0 */
/* 334 */	NdrFcLong( 0xc8 ),	/* 200 */
/* 338 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 340 */	NdrFcShort( 0x10 ),	/* 16 */
/* 342 */	NdrFcShort( 0x0 ),	/* 0 */
/* 344 */	NdrFcShort( 0xa ),	/* Offset= 10 (354) */
/* 346 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 348 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (328) */
/* 350 */	0x42,		/* FC_STRUCTPAD6 */
			0x36,		/* FC_POINTER */
/* 352 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 354 */	
			0x12, 0x0,	/* FC_UP */
/* 356 */	NdrFcShort( 0xffffff94 ),	/* Offset= -108 (248) */
/* 358 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 360 */	NdrFcShort( 0x2 ),	/* Offset= 2 (362) */
/* 362 */	
			0x13, 0x0,	/* FC_OP */
/* 364 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (338) */

			0x0
        }
    };


/* Standard interface: __MIDL_itf_napenforcementclient_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: INapEnforcementClientBinding, ver. 0.0,
   GUID={0x92B93223,0x7487,0x42d9,{0x9A,0x91,0x5B,0x85,0x07,0x72,0x03,0x84}} */

#pragma code_seg(".orpc")
static const unsigned short INapEnforcementClientBinding_FormatStringOffsetTable[] =
    {
    0,
    44,
    76,
    114,
    158,
    196,
    234
    };

static const MIDL_STUBLESS_PROXY_INFO INapEnforcementClientBinding_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapEnforcementClientBinding_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapEnforcementClientBinding_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapEnforcementClientBinding_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _INapEnforcementClientBindingProxyVtbl = 
{
    &INapEnforcementClientBinding_ProxyInfo,
    &IID_INapEnforcementClientBinding,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientBinding::Initialize */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientBinding::Uninitialize */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientBinding::CreateConnection */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientBinding::GetSoHRequest */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientBinding::ProcessSoHResponse */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientBinding::NotifyConnectionStateDown */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientBinding::NotifySoHChangeFailure */
};

const CInterfaceStubVtbl _INapEnforcementClientBindingStubVtbl =
{
    &IID_INapEnforcementClientBinding,
    &INapEnforcementClientBinding_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: INapEnforcementClientCallback, ver. 0.0,
   GUID={0xF5A0B90A,0x83A1,0x4f76,{0xBA,0x3F,0x02,0x54,0x18,0x68,0x28,0x14}} */

#pragma code_seg(".orpc")
static const unsigned short INapEnforcementClientCallback_FormatStringOffsetTable[] =
    {
    266,
    298
    };

static const MIDL_STUBLESS_PROXY_INFO INapEnforcementClientCallback_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapEnforcementClientCallback_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapEnforcementClientCallback_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapEnforcementClientCallback_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _INapEnforcementClientCallbackProxyVtbl = 
{
    &INapEnforcementClientCallback_ProxyInfo,
    &IID_INapEnforcementClientCallback,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientCallback::NotifySoHChange */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientCallback::GetConnections */
};

const CInterfaceStubVtbl _INapEnforcementClientCallbackStubVtbl =
{
    &IID_INapEnforcementClientCallback,
    &INapEnforcementClientCallback_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: INapEnforcementClientConnection, ver. 0.0,
   GUID={0xFB3A3505,0xDDB1,0x468a,{0xB3,0x07,0xF3,0x28,0xA5,0x74,0x19,0xD8}} */

#pragma code_seg(".orpc")
static const unsigned short INapEnforcementClientConnection_FormatStringOffsetTable[] =
    {
    336,
    374,
    412,
    450,
    488,
    526,
    564,
    602,
    640,
    678,
    716,
    754,
    792,
    830,
    868,
    906,
    944,
    982,
    1020,
    1058
    };

static const MIDL_STUBLESS_PROXY_INFO INapEnforcementClientConnection_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapEnforcementClientConnection_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapEnforcementClientConnection_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapEnforcementClientConnection_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(23) _INapEnforcementClientConnectionProxyVtbl = 
{
    &INapEnforcementClientConnection_ProxyInfo,
    &IID_INapEnforcementClientConnection,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientConnection::Initialize */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientConnection::SetMaxSize */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientConnection::GetMaxSize */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientConnection::SetFlags */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientConnection::GetFlags */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientConnection::SetConnectionId */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientConnection::GetConnectionId */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientConnection::GetCorrelationId */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientConnection::GetStringCorrelationId */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientConnection::SetCorrelationId */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientConnection::SetSoHRequest */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientConnection::GetSoHRequest */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientConnection::SetSoHResponse */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientConnection::GetSoHResponse */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientConnection::SetIsolationInfo */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientConnection::GetIsolationInfo */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientConnection::SetPrivateData */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientConnection::GetPrivateData */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientConnection::SetEnforcerPrivateData */ ,
    (void *) (INT_PTR) -1 /* INapEnforcementClientConnection::GetEnforcerPrivateData */
};

const CInterfaceStubVtbl _INapEnforcementClientConnectionStubVtbl =
{
    &IID_INapEnforcementClientConnection,
    &INapEnforcementClientConnection_ServerInfo,
    23,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_napenforcementclient_0257, ver. 0.0,
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

const CInterfaceProxyVtbl * _napenforcementclient_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_INapEnforcementClientConnectionProxyVtbl,
    ( CInterfaceProxyVtbl *) &_INapEnforcementClientCallbackProxyVtbl,
    ( CInterfaceProxyVtbl *) &_INapEnforcementClientBindingProxyVtbl,
    0
};

const CInterfaceStubVtbl * _napenforcementclient_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_INapEnforcementClientConnectionStubVtbl,
    ( CInterfaceStubVtbl *) &_INapEnforcementClientCallbackStubVtbl,
    ( CInterfaceStubVtbl *) &_INapEnforcementClientBindingStubVtbl,
    0
};

PCInterfaceName const _napenforcementclient_InterfaceNamesList[] = 
{
    "INapEnforcementClientConnection",
    "INapEnforcementClientCallback",
    "INapEnforcementClientBinding",
    0
};


#define _napenforcementclient_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _napenforcementclient, pIID, n)

int __stdcall _napenforcementclient_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _napenforcementclient, 3, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _napenforcementclient, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _napenforcementclient, 3, *pIndex )
    
}

const ExtendedProxyFileInfo napenforcementclient_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _napenforcementclient_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _napenforcementclient_StubVtblList,
    (const PCInterfaceName * ) & _napenforcementclient_InterfaceNamesList,
    0, // no delegation
    & _napenforcementclient_IID_Lookup, 
    3,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* defined(_M_IA64) || defined(_M_AMD64)*/

