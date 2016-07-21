
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Fri Nov 09 19:00:56 2007
 */
/* Compiler settings for O:\CS AdminKit\development2\include\nap\nap_sdk\napsystemhealthagent.idl:
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


#include "napsystemhealthagent.h"

#define TYPE_FORMAT_STRING_SIZE   317                               
#define PROC_FORMAT_STRING_SIZE   95                                
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


extern const MIDL_SERVER_INFO INapSystemHealthAgentBinding_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapSystemHealthAgentBinding_ProxyInfo;


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentBinding_Initialize_Proxy( 
    INapSystemHealthAgentBinding * This,
    /* [in] */ SystemHealthEntityId id,
    /* [in] */ INapSystemHealthAgentCallback *callback)
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
            *(( SystemHealthEntityId * )_StubMsg.Buffer)++ = id;
            
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

void __RPC_STUB INapSystemHealthAgentBinding_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    INapSystemHealthAgentCallback *callback;
    SystemHealthEntityId id;
    
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
        id = *(( SystemHealthEntityId * )_StubMsg.Buffer)++;
        
        NdrInterfacePointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char * *)&callback,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[2],
                                       (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSystemHealthAgentBinding*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> Initialize(
              (INapSystemHealthAgentBinding *) ((CStdStubBuffer *)This)->pvServerObject,
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


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentBinding_Uninitialize_Proxy( 
    INapSystemHealthAgentBinding * This)
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

void __RPC_STUB INapSystemHealthAgentBinding_Uninitialize_Stub(
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
        _RetVal = (((INapSystemHealthAgentBinding*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> Uninitialize((INapSystemHealthAgentBinding *) ((CStdStubBuffer *)This)->pvServerObject);
        
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


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentBinding_NotifySoHChange_Proxy( 
    INapSystemHealthAgentBinding * This)
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

void __RPC_STUB INapSystemHealthAgentBinding_NotifySoHChange_Stub(
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
        _RetVal = (((INapSystemHealthAgentBinding*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> NotifySoHChange((INapSystemHealthAgentBinding *) ((CStdStubBuffer *)This)->pvServerObject);
        
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


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentBinding_GetSystemIsolationInfo_Proxy( 
    INapSystemHealthAgentBinding * This,
    /* [out] */ IsolationInfo **isolationInfo,
    /* [out] */ BOOL *unknownConnections)
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
                      6);
        
        
        
        if(!isolationInfo)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        if(!unknownConnections)
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
                                  (unsigned char * *)&isolationInfo,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[20],
                                  (unsigned char)0 );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *unknownConnections = *(( BOOL * )_StubMsg.Buffer)++;
            
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
                         ( void * )isolationInfo);
        NdrClearOutParameters(
                         ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[82],
                         ( void * )unknownConnections);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapSystemHealthAgentBinding_GetSystemIsolationInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    IsolationInfo *_M0;
    BOOL _M1	=	0;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    IsolationInfo **isolationInfo;
    BOOL *unknownConnections;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( IsolationInfo ** )isolationInfo = 0;
    ( BOOL * )unknownConnections = 0;
    RpcTryFinally
        {
        isolationInfo = &_M0;
        _M0 = 0;
        unknownConnections = &_M1;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSystemHealthAgentBinding*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetSystemIsolationInfo(
                          (INapSystemHealthAgentBinding *) ((CStdStubBuffer *)This)->pvServerObject,
                          isolationInfo,
                          unknownConnections);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 36;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)isolationInfo,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[20] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)isolationInfo,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[20] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( BOOL * )_StubMsg.Buffer)++ = *unknownConnections;
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)isolationInfo,
                        &__MIDL_TypeFormatString.Format[20] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentBinding_FlushCache_Proxy( 
    INapSystemHealthAgentBinding * This)
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

void __RPC_STUB INapSystemHealthAgentBinding_FlushCache_Stub(
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
        _RetVal = (((INapSystemHealthAgentBinding*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> FlushCache((INapSystemHealthAgentBinding *) ((CStdStubBuffer *)This)->pvServerObject);
        
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


extern const MIDL_SERVER_INFO INapSystemHealthAgentCallback_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapSystemHealthAgentCallback_ProxyInfo;


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentCallback_GetSoHRequest_Proxy( 
    INapSystemHealthAgentCallback * This,
    /* [in] */ INapSystemHealthAgentRequest *request)
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
            NdrInterfacePointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                           (unsigned char *)request,
                                           (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[86] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrInterfacePointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                         (unsigned char *)request,
                                         (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[86] );
            
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

void __RPC_STUB INapSystemHealthAgentCallback_GetSoHRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    INapSystemHealthAgentRequest *request;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    request = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[20] );
        
        NdrInterfacePointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char * *)&request,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[86],
                                       (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSystemHealthAgentCallback*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetSoHRequest((INapSystemHealthAgentCallback *) ((CStdStubBuffer *)This)->pvServerObject,request);
        
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
                                 &__MIDL_TypeFormatString.Format[86] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentCallback_ProcessSoHResponse_Proxy( 
    INapSystemHealthAgentCallback * This,
    /* [in] */ INapSystemHealthAgentRequest *request)
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
            NdrInterfacePointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                           (unsigned char *)request,
                                           (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[86] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrInterfacePointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                         (unsigned char *)request,
                                         (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[86] );
            
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

void __RPC_STUB INapSystemHealthAgentCallback_ProcessSoHResponse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    INapSystemHealthAgentRequest *request;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    request = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[20] );
        
        NdrInterfacePointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char * *)&request,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[86],
                                       (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSystemHealthAgentCallback*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> ProcessSoHResponse((INapSystemHealthAgentCallback *) ((CStdStubBuffer *)This)->pvServerObject,request);
        
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
                                 &__MIDL_TypeFormatString.Format[86] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentCallback_NotifySystemIsolationStateChange_Proxy( 
    INapSystemHealthAgentCallback * This)
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

void __RPC_STUB INapSystemHealthAgentCallback_NotifySystemIsolationStateChange_Stub(
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
        _RetVal = (((INapSystemHealthAgentCallback*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> NotifySystemIsolationStateChange((INapSystemHealthAgentCallback *) ((CStdStubBuffer *)This)->pvServerObject);
        
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


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentCallback_GetFixupInfo_Proxy( 
    INapSystemHealthAgentCallback * This,
    /* [out] */ FixupInfo **info)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    if(info)
        {
        *info = 0;
        }
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      6);
        
        
        
        if(!info)
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
                                  (unsigned char * *)&info,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[104],
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[104],
                         ( void * )info);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapSystemHealthAgentCallback_GetFixupInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    FixupInfo *_M2;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    FixupInfo **info;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( FixupInfo ** )info = 0;
    RpcTryFinally
        {
        info = &_M2;
        _M2 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSystemHealthAgentCallback*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetFixupInfo((INapSystemHealthAgentCallback *) ((CStdStubBuffer *)This)->pvServerObject,info);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)info,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[104] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)info,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[104] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)info,
                        &__MIDL_TypeFormatString.Format[104] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentCallback_CompareSoHRequests_Proxy( 
    INapSystemHealthAgentCallback * This,
    /* [in] */ const SoHRequest *lhs,
    /* [in] */ const SoHRequest *rhs,
    /* [out] */ BOOL *isEqual)
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
        
        
        
        if(!lhs)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        if(!rhs)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        if(!isEqual)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrSimpleStructBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char *)lhs,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[230] );
            
            NdrSimpleStructBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char *)rhs,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[230] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                     (unsigned char *)lhs,
                                     (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[230] );
            
            NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                     (unsigned char *)rhs,
                                     (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[230] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[32] );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *isEqual = *(( BOOL * )_StubMsg.Buffer)++;
            
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
                         ( void * )isEqual);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapSystemHealthAgentCallback_CompareSoHRequests_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    BOOL _M3	=	0;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    BOOL *isEqual;
    const SoHRequest *lhs;
    const SoHRequest *rhs;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( SoHRequest * )lhs = 0;
    ( SoHRequest * )rhs = 0;
    ( BOOL * )isEqual = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[32] );
        
        NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char * *)&lhs,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[230],
                                   (unsigned char)0 );
        
        NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char * *)&rhs,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[230],
                                   (unsigned char)0 );
        
        isEqual = &_M3;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSystemHealthAgentCallback*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> CompareSoHRequests(
                      (INapSystemHealthAgentCallback *) ((CStdStubBuffer *)This)->pvServerObject,
                      lhs,
                      rhs,
                      isEqual);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 36;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( BOOL * )_StubMsg.Buffer)++ = *isEqual;
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)lhs,
                        &__MIDL_TypeFormatString.Format[162] );
        
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)rhs,
                        &__MIDL_TypeFormatString.Format[162] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentCallback_NotifyOrphanedSoHRequest_Proxy( 
    INapSystemHealthAgentCallback * This,
    /* [in] */ const CorrelationId *correlationId)
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
        
        
        
        if(!correlationId)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 108;
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                     (unsigned char *)correlationId,
                                     (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[274] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[46] );
            
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

void __RPC_STUB INapSystemHealthAgentCallback_NotifyOrphanedSoHRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    const CorrelationId *correlationId;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( CorrelationId * )correlationId = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[46] );
        
        NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char * *)&correlationId,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[274],
                                   (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSystemHealthAgentCallback*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> NotifyOrphanedSoHRequest((INapSystemHealthAgentCallback *) ((CStdStubBuffer *)This)->pvServerObject,correlationId);
        
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


extern const MIDL_SERVER_INFO INapSystemHealthAgentRequest_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapSystemHealthAgentRequest_ProxyInfo;


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentRequest_GetCorrelationId_Proxy( 
    INapSystemHealthAgentRequest * This,
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[52] );
            
            NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char * *)&correlationId,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[274],
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
                         ( void * )correlationId);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapSystemHealthAgentRequest_GetCorrelationId_Stub(
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
        _RetVal = (((INapSystemHealthAgentRequest*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetCorrelationId((INapSystemHealthAgentRequest *) ((CStdStubBuffer *)This)->pvServerObject,correlationId);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 116;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                 (unsigned char *)correlationId,
                                 (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[274] );
        
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


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentRequest_GetStringCorrelationId_Proxy( 
    INapSystemHealthAgentRequest * This,
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[58] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&correlationId,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[292],
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[292],
                         ( void * )correlationId);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapSystemHealthAgentRequest_GetStringCorrelationId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    StringCorrelationId *_M4;
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
        correlationId = &_M4;
        _M4 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSystemHealthAgentRequest*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetStringCorrelationId((INapSystemHealthAgentRequest *) ((CStdStubBuffer *)This)->pvServerObject,correlationId);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)correlationId,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[292] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)correlationId,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[292] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)correlationId,
                        &__MIDL_TypeFormatString.Format[292] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentRequest_SetSoHRequest_Proxy( 
    INapSystemHealthAgentRequest * This,
    /* [in] */ const SoHRequest *sohRequest,
    /* [in] */ BOOL cacheSohForLaterUse)
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
        
        
        
        if(!sohRequest)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 8;
            NdrSimpleStructBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char *)sohRequest,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[230] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                     (unsigned char *)sohRequest,
                                     (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[230] );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *(( BOOL * )_StubMsg.Buffer)++ = cacheSohForLaterUse;
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[64] );
            
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

void __RPC_STUB INapSystemHealthAgentRequest_SetSoHRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    BOOL cacheSohForLaterUse;
    const SoHRequest *sohRequest;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( SoHRequest * )sohRequest = 0;
    cacheSohForLaterUse = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[64] );
        
        NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char * *)&sohRequest,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[230],
                                   (unsigned char)0 );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        
        if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        cacheSohForLaterUse = *(( BOOL * )_StubMsg.Buffer)++;
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSystemHealthAgentRequest*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> SetSoHRequest(
                 (INapSystemHealthAgentRequest *) ((CStdStubBuffer *)This)->pvServerObject,
                 sohRequest,
                 cacheSohForLaterUse);
        
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
                        &__MIDL_TypeFormatString.Format[162] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentRequest_GetSoHRequest_Proxy( 
    INapSystemHealthAgentRequest * This,
    /* [out] */ SoHRequest **sohRequest)
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
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[72] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&sohRequest,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[300],
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[300],
                         ( void * )sohRequest);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapSystemHealthAgentRequest_GetSoHRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    SoHRequest *_M5;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    SoHRequest **sohRequest;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( SoHRequest ** )sohRequest = 0;
    RpcTryFinally
        {
        sohRequest = &_M5;
        _M5 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSystemHealthAgentRequest*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetSoHRequest((INapSystemHealthAgentRequest *) ((CStdStubBuffer *)This)->pvServerObject,sohRequest);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)sohRequest,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[300] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)sohRequest,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[300] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)sohRequest,
                        &__MIDL_TypeFormatString.Format[300] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentRequest_GetSoHResponse_Proxy( 
    INapSystemHealthAgentRequest * This,
    /* [out] */ SoHResponse **sohResponse,
    /* [out] */ UINT8 *flags)
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
                      7);
        
        
        
        if(!sohResponse)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[78] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&sohResponse,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[300],
                                  (unsigned char)0 );
            
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[300],
                         ( void * )sohResponse);
        NdrClearOutParameters(
                         ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[308],
                         ( void * )flags);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapSystemHealthAgentRequest_GetSoHResponse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    SoHResponse *_M6;
    UINT8 _M7	=	0;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    UINT8 *flags;
    SoHResponse **sohResponse;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( SoHResponse ** )sohResponse = 0;
    ( UINT8 * )flags = 0;
    RpcTryFinally
        {
        sohResponse = &_M6;
        _M6 = 0;
        flags = &_M7;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSystemHealthAgentRequest*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetSoHResponse(
                  (INapSystemHealthAgentRequest *) ((CStdStubBuffer *)This)->pvServerObject,
                  sohResponse,
                  flags);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 33;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)sohResponse,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[300] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)sohResponse,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[300] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 0) & ~ 0);
        *(( UINT8 * )_StubMsg.Buffer)++ = *flags;
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)sohResponse,
                        &__MIDL_TypeFormatString.Format[300] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapSystemHealthAgentRequest_GetCacheSoHFlag_Proxy( 
    INapSystemHealthAgentRequest * This,
    BOOL *cacheSohForLaterUse)
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
        
        
        
        if(!cacheSohForLaterUse)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 28;
            NdrProxyGetBuffer(This, &_StubMsg);
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *(( BOOL * )_StubMsg.Buffer)++ = *cacheSohForLaterUse;
            
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

void __RPC_STUB INapSystemHealthAgentRequest_GetCacheSoHFlag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    BOOL *cacheSohForLaterUse;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( BOOL * )cacheSohForLaterUse = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[88] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        cacheSohForLaterUse = ( BOOL * )_StubMsg.Buffer;
        _StubMsg.Buffer += sizeof( BOOL  );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapSystemHealthAgentRequest*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetCacheSoHFlag((INapSystemHealthAgentRequest *) ((CStdStubBuffer *)This)->pvServerObject,cacheSohForLaterUse);
        
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
/* 14 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 16 */	NdrFcShort( 0x52 ),	/* Type Offset=82 */
/* 18 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 20 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 22 */	NdrFcShort( 0x56 ),	/* Type Offset=86 */
/* 24 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 26 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 28 */	NdrFcShort( 0x68 ),	/* Type Offset=104 */
/* 30 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 32 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 34 */	NdrFcShort( 0xa2 ),	/* Type Offset=162 */
/* 36 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 38 */	NdrFcShort( 0xa2 ),	/* Type Offset=162 */
/* 40 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 42 */	NdrFcShort( 0x52 ),	/* Type Offset=82 */
/* 44 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 46 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 48 */	NdrFcShort( 0xfc ),	/* Type Offset=252 */
/* 50 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 52 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 54 */	NdrFcShort( 0x120 ),	/* Type Offset=288 */
/* 56 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 58 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 60 */	NdrFcShort( 0x124 ),	/* Type Offset=292 */
/* 62 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 64 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 66 */	NdrFcShort( 0xa2 ),	/* Type Offset=162 */
/* 68 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 70 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 72 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 74 */	NdrFcShort( 0x12c ),	/* Type Offset=300 */
/* 76 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 78 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 80 */	NdrFcShort( 0x12c ),	/* Type Offset=300 */
/* 82 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 84 */	NdrFcShort( 0x134 ),	/* Type Offset=308 */
/* 86 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 88 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 90 */	NdrFcShort( 0x138 ),	/* Type Offset=312 */
/* 92 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
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
/*  4 */	NdrFcLong( 0xf1072a57 ),	/* -251188649 */
/*  8 */	NdrFcShort( 0x214f ),	/* 8527 */
/* 10 */	NdrFcShort( 0x4ee2 ),	/* 20194 */
/* 12 */	0x83,		/* 131 */
			0x77,		/* 119 */
/* 14 */	0x14,		/* 20 */
			0xef,		/* 239 */
/* 16 */	0x14,		/* 20 */
			0xc,		/* 12 */
/* 18 */	0xd9,		/* 217 */
			0xf3,		/* 243 */
/* 20 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 22 */	NdrFcShort( 0x2 ),	/* Offset= 2 (24) */
/* 24 */	
			0x13, 0x0,	/* FC_OP */
/* 26 */	NdrFcShort( 0x26 ),	/* Offset= 38 (64) */
/* 28 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 30 */	NdrFcShort( 0x8 ),	/* 8 */
/* 32 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 34 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 36 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 38 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x57,		/* FC_ADD_1 */
/* 40 */	NdrFcShort( 0x0 ),	/* 0 */
/* 42 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 44 */	NdrFcShort( 0x8 ),	/* 8 */
/* 46 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 48 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 50 */	NdrFcShort( 0x4 ),	/* 4 */
/* 52 */	NdrFcShort( 0x4 ),	/* 4 */
/* 54 */	0x13, 0x0,	/* FC_OP */
/* 56 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (36) */
/* 58 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 60 */	0x3e,		/* FC_STRUCTPAD2 */
			0x8,		/* FC_LONG */
/* 62 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 64 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 66 */	NdrFcShort( 0x14 ),	/* 20 */
/* 68 */	NdrFcShort( 0x0 ),	/* 0 */
/* 70 */	NdrFcShort( 0x0 ),	/* Offset= 0 (70) */
/* 72 */	0xd,		/* FC_ENUM16 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 74 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffd1 ),	/* Offset= -47 (28) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 78 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffdb ),	/* Offset= -37 (42) */
			0x5b,		/* FC_END */
/* 82 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 84 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 86 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 88 */	NdrFcLong( 0x5b360a69 ),	/* 1530268265 */
/* 92 */	NdrFcShort( 0x212d ),	/* 8493 */
/* 94 */	NdrFcShort( 0x440d ),	/* 17421 */
/* 96 */	0xb3,		/* 179 */
			0x98,		/* 152 */
/* 98 */	0x7e,		/* 126 */
			0xef,		/* 239 */
/* 100 */	0xd4,		/* 212 */
			0x97,		/* 151 */
/* 102 */	0x85,		/* 133 */
			0x3a,		/* 58 */
/* 104 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 106 */	NdrFcShort( 0x2 ),	/* Offset= 2 (108) */
/* 108 */	
			0x13, 0x0,	/* FC_OP */
/* 110 */	NdrFcShort( 0x22 ),	/* Offset= 34 (144) */
/* 112 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 114 */	NdrFcShort( 0x4 ),	/* 4 */
/* 116 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 118 */	NdrFcShort( 0x0 ),	/* 0 */
/* 120 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 122 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 124 */	NdrFcShort( 0x8 ),	/* 8 */
/* 126 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 128 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 130 */	NdrFcShort( 0x4 ),	/* 4 */
/* 132 */	NdrFcShort( 0x4 ),	/* 4 */
/* 134 */	0x13, 0x0,	/* FC_OP */
/* 136 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (112) */
/* 138 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 140 */	0x3e,		/* FC_STRUCTPAD2 */
			0x8,		/* FC_LONG */
/* 142 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 144 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 146 */	NdrFcShort( 0x14 ),	/* 20 */
/* 148 */	NdrFcShort( 0x0 ),	/* 0 */
/* 150 */	NdrFcShort( 0x0 ),	/* Offset= 0 (150) */
/* 152 */	0xd,		/* FC_ENUM16 */
			0x2,		/* FC_CHAR */
/* 154 */	0x3f,		/* FC_STRUCTPAD3 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 156 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffdd ),	/* Offset= -35 (122) */
			0x8,		/* FC_LONG */
/* 160 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 162 */	
			0x11, 0x0,	/* FC_RP */
/* 164 */	NdrFcShort( 0x42 ),	/* Offset= 66 (230) */
/* 166 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 168 */	NdrFcShort( 0x1 ),	/* 1 */
/* 170 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 172 */	NdrFcShort( 0x2 ),	/* 2 */
/* 174 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 176 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 178 */	NdrFcShort( 0x8 ),	/* 8 */
/* 180 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 182 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 184 */	NdrFcShort( 0x4 ),	/* 4 */
/* 186 */	NdrFcShort( 0x4 ),	/* 4 */
/* 188 */	0x12, 0x0,	/* FC_UP */
/* 190 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (166) */
/* 192 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 194 */	0x6,		/* FC_SHORT */
			0x8,		/* FC_LONG */
/* 196 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 198 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 200 */	NdrFcShort( 0x8 ),	/* 8 */
/* 202 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 204 */	NdrFcShort( 0x0 ),	/* 0 */
/* 206 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 208 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 210 */	NdrFcShort( 0x8 ),	/* 8 */
/* 212 */	NdrFcShort( 0x0 ),	/* 0 */
/* 214 */	NdrFcShort( 0x1 ),	/* 1 */
/* 216 */	NdrFcShort( 0x4 ),	/* 4 */
/* 218 */	NdrFcShort( 0x4 ),	/* 4 */
/* 220 */	0x12, 0x0,	/* FC_UP */
/* 222 */	NdrFcShort( 0xffffffc8 ),	/* Offset= -56 (166) */
/* 224 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 226 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffcd ),	/* Offset= -51 (176) */
			0x5b,		/* FC_END */
/* 230 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 232 */	NdrFcShort( 0x8 ),	/* 8 */
/* 234 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 236 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 238 */	NdrFcShort( 0x4 ),	/* 4 */
/* 240 */	NdrFcShort( 0x4 ),	/* 4 */
/* 242 */	0x12, 0x0,	/* FC_UP */
/* 244 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (198) */
/* 246 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 248 */	0x3e,		/* FC_STRUCTPAD2 */
			0x8,		/* FC_LONG */
/* 250 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 252 */	
			0x11, 0x0,	/* FC_RP */
/* 254 */	NdrFcShort( 0x14 ),	/* Offset= 20 (274) */
/* 256 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 258 */	NdrFcShort( 0x8 ),	/* 8 */
/* 260 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 262 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 264 */	NdrFcShort( 0x10 ),	/* 16 */
/* 266 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 268 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 270 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (256) */
			0x5b,		/* FC_END */
/* 274 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 276 */	NdrFcShort( 0x18 ),	/* 24 */
/* 278 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 280 */	NdrFcShort( 0xffffffee ),	/* Offset= -18 (262) */
/* 282 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 284 */	NdrFcShort( 0xffffff00 ),	/* Offset= -256 (28) */
/* 286 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 288 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 290 */	NdrFcShort( 0xfffffff0 ),	/* Offset= -16 (274) */
/* 292 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 294 */	NdrFcShort( 0x2 ),	/* Offset= 2 (296) */
/* 296 */	
			0x13, 0x0,	/* FC_OP */
/* 298 */	NdrFcShort( 0xffffff00 ),	/* Offset= -256 (42) */
/* 300 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 302 */	NdrFcShort( 0x2 ),	/* Offset= 2 (304) */
/* 304 */	
			0x13, 0x0,	/* FC_OP */
/* 306 */	NdrFcShort( 0xffffffb4 ),	/* Offset= -76 (230) */
/* 308 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 310 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 312 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 314 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */

			0x0
        }
    };


/* Standard interface: __MIDL_itf_napsystemhealthagent_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: INapSystemHealthAgentBinding, ver. 0.0,
   GUID={0xA6894F43,0x9CC7,0x44c9,{0xA2,0x3F,0x19,0xDB,0xF3,0x6B,0xAD,0x28}} */

#pragma code_seg(".orpc")
static const unsigned short INapSystemHealthAgentBinding_FormatStringOffsetTable[] =
    {
    0,
    8,
    8,
    10,
    8
    };

static const MIDL_STUBLESS_PROXY_INFO INapSystemHealthAgentBinding_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapSystemHealthAgentBinding_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapSystemHealthAgentBinding_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapSystemHealthAgentBinding_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
const CINTERFACE_PROXY_VTABLE(8) _INapSystemHealthAgentBindingProxyVtbl = 
{
    &IID_INapSystemHealthAgentBinding,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    INapSystemHealthAgentBinding_Initialize_Proxy ,
    INapSystemHealthAgentBinding_Uninitialize_Proxy ,
    INapSystemHealthAgentBinding_NotifySoHChange_Proxy ,
    INapSystemHealthAgentBinding_GetSystemIsolationInfo_Proxy ,
    INapSystemHealthAgentBinding_FlushCache_Proxy
};


static const PRPC_STUB_FUNCTION INapSystemHealthAgentBinding_table[] =
{
    INapSystemHealthAgentBinding_Initialize_Stub,
    INapSystemHealthAgentBinding_Uninitialize_Stub,
    INapSystemHealthAgentBinding_NotifySoHChange_Stub,
    INapSystemHealthAgentBinding_GetSystemIsolationInfo_Stub,
    INapSystemHealthAgentBinding_FlushCache_Stub
};

const CInterfaceStubVtbl _INapSystemHealthAgentBindingStubVtbl =
{
    &IID_INapSystemHealthAgentBinding,
    &INapSystemHealthAgentBinding_ServerInfo,
    8,
    &INapSystemHealthAgentBinding_table[-3],
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_napsystemhealthagent_0255, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: INapSystemHealthAgentCallback, ver. 0.0,
   GUID={0xF1072A57,0x214F,0x4ee2,{0x83,0x77,0x14,0xEF,0x14,0x0C,0xD9,0xF3}} */

#pragma code_seg(".orpc")
static const unsigned short INapSystemHealthAgentCallback_FormatStringOffsetTable[] =
    {
    20,
    20,
    8,
    26,
    32,
    46
    };

static const MIDL_STUBLESS_PROXY_INFO INapSystemHealthAgentCallback_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapSystemHealthAgentCallback_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapSystemHealthAgentCallback_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapSystemHealthAgentCallback_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
const CINTERFACE_PROXY_VTABLE(9) _INapSystemHealthAgentCallbackProxyVtbl = 
{
    &IID_INapSystemHealthAgentCallback,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    INapSystemHealthAgentCallback_GetSoHRequest_Proxy ,
    INapSystemHealthAgentCallback_ProcessSoHResponse_Proxy ,
    INapSystemHealthAgentCallback_NotifySystemIsolationStateChange_Proxy ,
    INapSystemHealthAgentCallback_GetFixupInfo_Proxy ,
    INapSystemHealthAgentCallback_CompareSoHRequests_Proxy ,
    INapSystemHealthAgentCallback_NotifyOrphanedSoHRequest_Proxy
};


static const PRPC_STUB_FUNCTION INapSystemHealthAgentCallback_table[] =
{
    INapSystemHealthAgentCallback_GetSoHRequest_Stub,
    INapSystemHealthAgentCallback_ProcessSoHResponse_Stub,
    INapSystemHealthAgentCallback_NotifySystemIsolationStateChange_Stub,
    INapSystemHealthAgentCallback_GetFixupInfo_Stub,
    INapSystemHealthAgentCallback_CompareSoHRequests_Stub,
    INapSystemHealthAgentCallback_NotifyOrphanedSoHRequest_Stub
};

const CInterfaceStubVtbl _INapSystemHealthAgentCallbackStubVtbl =
{
    &IID_INapSystemHealthAgentCallback,
    &INapSystemHealthAgentCallback_ServerInfo,
    9,
    &INapSystemHealthAgentCallback_table[-3],
    CStdStubBuffer_METHODS
};


/* Object interface: INapSystemHealthAgentRequest, ver. 0.0,
   GUID={0x5B360A69,0x212D,0x440d,{0xB3,0x98,0x7E,0xEF,0xD4,0x97,0x85,0x3A}} */

#pragma code_seg(".orpc")
static const unsigned short INapSystemHealthAgentRequest_FormatStringOffsetTable[] =
    {
    52,
    58,
    64,
    72,
    78,
    88
    };

static const MIDL_STUBLESS_PROXY_INFO INapSystemHealthAgentRequest_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapSystemHealthAgentRequest_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapSystemHealthAgentRequest_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapSystemHealthAgentRequest_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
const CINTERFACE_PROXY_VTABLE(9) _INapSystemHealthAgentRequestProxyVtbl = 
{
    &IID_INapSystemHealthAgentRequest,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    INapSystemHealthAgentRequest_GetCorrelationId_Proxy ,
    INapSystemHealthAgentRequest_GetStringCorrelationId_Proxy ,
    INapSystemHealthAgentRequest_SetSoHRequest_Proxy ,
    INapSystemHealthAgentRequest_GetSoHRequest_Proxy ,
    INapSystemHealthAgentRequest_GetSoHResponse_Proxy ,
    INapSystemHealthAgentRequest_GetCacheSoHFlag_Proxy
};


static const PRPC_STUB_FUNCTION INapSystemHealthAgentRequest_table[] =
{
    INapSystemHealthAgentRequest_GetCorrelationId_Stub,
    INapSystemHealthAgentRequest_GetStringCorrelationId_Stub,
    INapSystemHealthAgentRequest_SetSoHRequest_Stub,
    INapSystemHealthAgentRequest_GetSoHRequest_Stub,
    INapSystemHealthAgentRequest_GetSoHResponse_Stub,
    INapSystemHealthAgentRequest_GetCacheSoHFlag_Stub
};

const CInterfaceStubVtbl _INapSystemHealthAgentRequestStubVtbl =
{
    &IID_INapSystemHealthAgentRequest,
    &INapSystemHealthAgentRequest_ServerInfo,
    9,
    &INapSystemHealthAgentRequest_table[-3],
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_napsystemhealthagent_0257, ver. 0.0,
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

const CInterfaceProxyVtbl * _napsystemhealthagent_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_INapSystemHealthAgentBindingProxyVtbl,
    ( CInterfaceProxyVtbl *) &_INapSystemHealthAgentCallbackProxyVtbl,
    ( CInterfaceProxyVtbl *) &_INapSystemHealthAgentRequestProxyVtbl,
    0
};

const CInterfaceStubVtbl * _napsystemhealthagent_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_INapSystemHealthAgentBindingStubVtbl,
    ( CInterfaceStubVtbl *) &_INapSystemHealthAgentCallbackStubVtbl,
    ( CInterfaceStubVtbl *) &_INapSystemHealthAgentRequestStubVtbl,
    0
};

PCInterfaceName const _napsystemhealthagent_InterfaceNamesList[] = 
{
    "INapSystemHealthAgentBinding",
    "INapSystemHealthAgentCallback",
    "INapSystemHealthAgentRequest",
    0
};


#define _napsystemhealthagent_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _napsystemhealthagent, pIID, n)

int __stdcall _napsystemhealthagent_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _napsystemhealthagent, 3, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _napsystemhealthagent, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _napsystemhealthagent, 3, *pIndex )
    
}

const ExtendedProxyFileInfo napsystemhealthagent_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _napsystemhealthagent_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _napsystemhealthagent_StubVtblList,
    (const PCInterfaceName * ) & _napsystemhealthagent_InterfaceNamesList,
    0, // no delegation
    & _napsystemhealthagent_IID_Lookup, 
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
/* at Fri Nov 09 19:00:56 2007
 */
/* Compiler settings for O:\CS AdminKit\development2\include\nap\nap_sdk\napsystemhealthagent.idl:
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


#include "napsystemhealthagent.h"

#define TYPE_FORMAT_STRING_SIZE   361                               
#define PROC_FORMAT_STRING_SIZE   627                               
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


extern const MIDL_SERVER_INFO INapSystemHealthAgentBinding_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapSystemHealthAgentBinding_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO INapSystemHealthAgentCallback_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapSystemHealthAgentCallback_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO INapSystemHealthAgentRequest_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapSystemHealthAgentRequest_ProxyInfo;



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

	/* Procedure NotifySystemIsolationStateChange */


	/* Procedure NotifySoHChange */

/* 76 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 78 */	NdrFcLong( 0x0 ),	/* 0 */
/* 82 */	NdrFcShort( 0x5 ),	/* 5 */
/* 84 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 86 */	NdrFcShort( 0x0 ),	/* 0 */
/* 88 */	NdrFcShort( 0x8 ),	/* 8 */
/* 90 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 92 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 94 */	NdrFcShort( 0x0 ),	/* 0 */
/* 96 */	NdrFcShort( 0x0 ),	/* 0 */
/* 98 */	NdrFcShort( 0x0 ),	/* 0 */
/* 100 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */


	/* Return value */

/* 102 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 104 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 106 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSystemIsolationInfo */

/* 108 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 110 */	NdrFcLong( 0x0 ),	/* 0 */
/* 114 */	NdrFcShort( 0x6 ),	/* 6 */
/* 116 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 118 */	NdrFcShort( 0x0 ),	/* 0 */
/* 120 */	NdrFcShort( 0x24 ),	/* 36 */
/* 122 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 124 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 126 */	NdrFcShort( 0x1 ),	/* 1 */
/* 128 */	NdrFcShort( 0x0 ),	/* 0 */
/* 130 */	NdrFcShort( 0x0 ),	/* 0 */
/* 132 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter isolationInfo */

/* 134 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 136 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 138 */	NdrFcShort( 0x14 ),	/* Type Offset=20 */

	/* Parameter unknownConnections */

/* 140 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 142 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 144 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 146 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 148 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 150 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure FlushCache */

/* 152 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 154 */	NdrFcLong( 0x0 ),	/* 0 */
/* 158 */	NdrFcShort( 0x7 ),	/* 7 */
/* 160 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 162 */	NdrFcShort( 0x0 ),	/* 0 */
/* 164 */	NdrFcShort( 0x8 ),	/* 8 */
/* 166 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 168 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 170 */	NdrFcShort( 0x0 ),	/* 0 */
/* 172 */	NdrFcShort( 0x0 ),	/* 0 */
/* 174 */	NdrFcShort( 0x0 ),	/* 0 */
/* 176 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 178 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 180 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 182 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSoHRequest */

/* 184 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 186 */	NdrFcLong( 0x0 ),	/* 0 */
/* 190 */	NdrFcShort( 0x3 ),	/* 3 */
/* 192 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 194 */	NdrFcShort( 0x0 ),	/* 0 */
/* 196 */	NdrFcShort( 0x8 ),	/* 8 */
/* 198 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 200 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 202 */	NdrFcShort( 0x0 ),	/* 0 */
/* 204 */	NdrFcShort( 0x0 ),	/* 0 */
/* 206 */	NdrFcShort( 0x0 ),	/* 0 */
/* 208 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter request */

/* 210 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 212 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 214 */	NdrFcShort( 0x62 ),	/* Type Offset=98 */

	/* Return value */

/* 216 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 218 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 220 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ProcessSoHResponse */

/* 222 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 224 */	NdrFcLong( 0x0 ),	/* 0 */
/* 228 */	NdrFcShort( 0x4 ),	/* 4 */
/* 230 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 232 */	NdrFcShort( 0x0 ),	/* 0 */
/* 234 */	NdrFcShort( 0x8 ),	/* 8 */
/* 236 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 238 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 240 */	NdrFcShort( 0x0 ),	/* 0 */
/* 242 */	NdrFcShort( 0x0 ),	/* 0 */
/* 244 */	NdrFcShort( 0x0 ),	/* 0 */
/* 246 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter request */

/* 248 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 250 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 252 */	NdrFcShort( 0x62 ),	/* Type Offset=98 */

	/* Return value */

/* 254 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 256 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 258 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFixupInfo */

/* 260 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 262 */	NdrFcLong( 0x0 ),	/* 0 */
/* 266 */	NdrFcShort( 0x6 ),	/* 6 */
/* 268 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 270 */	NdrFcShort( 0x0 ),	/* 0 */
/* 272 */	NdrFcShort( 0x8 ),	/* 8 */
/* 274 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 276 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 278 */	NdrFcShort( 0x1 ),	/* 1 */
/* 280 */	NdrFcShort( 0x0 ),	/* 0 */
/* 282 */	NdrFcShort( 0x0 ),	/* 0 */
/* 284 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter info */

/* 286 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 288 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 290 */	NdrFcShort( 0x74 ),	/* Type Offset=116 */

	/* Return value */

/* 292 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 294 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 296 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CompareSoHRequests */

/* 298 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 300 */	NdrFcLong( 0x0 ),	/* 0 */
/* 304 */	NdrFcShort( 0x7 ),	/* 7 */
/* 306 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 308 */	NdrFcShort( 0x0 ),	/* 0 */
/* 310 */	NdrFcShort( 0x24 ),	/* 36 */
/* 312 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 314 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 316 */	NdrFcShort( 0x0 ),	/* 0 */
/* 318 */	NdrFcShort( 0x4 ),	/* 4 */
/* 320 */	NdrFcShort( 0x0 ),	/* 0 */
/* 322 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter lhs */

/* 324 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 326 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 328 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Parameter rhs */

/* 330 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 332 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 334 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Parameter isEqual */

/* 336 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 338 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 340 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 342 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 344 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 346 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure NotifyOrphanedSoHRequest */

/* 348 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 350 */	NdrFcLong( 0x0 ),	/* 0 */
/* 354 */	NdrFcShort( 0x8 ),	/* 8 */
/* 356 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 358 */	NdrFcShort( 0x6c ),	/* 108 */
/* 360 */	NdrFcShort( 0x8 ),	/* 8 */
/* 362 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 364 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 366 */	NdrFcShort( 0x0 ),	/* 0 */
/* 368 */	NdrFcShort( 0x0 ),	/* 0 */
/* 370 */	NdrFcShort( 0x0 ),	/* 0 */
/* 372 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter correlationId */

/* 374 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 376 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 378 */	NdrFcShort( 0x13e ),	/* Type Offset=318 */

	/* Return value */

/* 380 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 382 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 384 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCorrelationId */

/* 386 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 388 */	NdrFcLong( 0x0 ),	/* 0 */
/* 392 */	NdrFcShort( 0x3 ),	/* 3 */
/* 394 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 396 */	NdrFcShort( 0x0 ),	/* 0 */
/* 398 */	NdrFcShort( 0x74 ),	/* 116 */
/* 400 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 402 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 404 */	NdrFcShort( 0x0 ),	/* 0 */
/* 406 */	NdrFcShort( 0x0 ),	/* 0 */
/* 408 */	NdrFcShort( 0x0 ),	/* 0 */
/* 410 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter correlationId */

/* 412 */	NdrFcShort( 0x6112 ),	/* Flags:  must free, out, simple ref, srv alloc size=24 */
/* 414 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 416 */	NdrFcShort( 0x13e ),	/* Type Offset=318 */

	/* Return value */

/* 418 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 420 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 422 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetStringCorrelationId */

/* 424 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 426 */	NdrFcLong( 0x0 ),	/* 0 */
/* 430 */	NdrFcShort( 0x4 ),	/* 4 */
/* 432 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 434 */	NdrFcShort( 0x0 ),	/* 0 */
/* 436 */	NdrFcShort( 0x8 ),	/* 8 */
/* 438 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 440 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 442 */	NdrFcShort( 0x1 ),	/* 1 */
/* 444 */	NdrFcShort( 0x0 ),	/* 0 */
/* 446 */	NdrFcShort( 0x0 ),	/* 0 */
/* 448 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter correlationId */

/* 450 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 452 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 454 */	NdrFcShort( 0x150 ),	/* Type Offset=336 */

	/* Return value */

/* 456 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 458 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 460 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetSoHRequest */

/* 462 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 464 */	NdrFcLong( 0x0 ),	/* 0 */
/* 468 */	NdrFcShort( 0x5 ),	/* 5 */
/* 470 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 472 */	NdrFcShort( 0x8 ),	/* 8 */
/* 474 */	NdrFcShort( 0x8 ),	/* 8 */
/* 476 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 478 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 480 */	NdrFcShort( 0x0 ),	/* 0 */
/* 482 */	NdrFcShort( 0x2 ),	/* 2 */
/* 484 */	NdrFcShort( 0x0 ),	/* 0 */
/* 486 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter sohRequest */

/* 488 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 490 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 492 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */

	/* Parameter cacheSohForLaterUse */

/* 494 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 496 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 498 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 500 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 502 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 504 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSoHRequest */

/* 506 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 508 */	NdrFcLong( 0x0 ),	/* 0 */
/* 512 */	NdrFcShort( 0x6 ),	/* 6 */
/* 514 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 516 */	NdrFcShort( 0x0 ),	/* 0 */
/* 518 */	NdrFcShort( 0x8 ),	/* 8 */
/* 520 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x2,		/* 2 */
/* 522 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 524 */	NdrFcShort( 0x2 ),	/* 2 */
/* 526 */	NdrFcShort( 0x0 ),	/* 0 */
/* 528 */	NdrFcShort( 0x0 ),	/* 0 */
/* 530 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter sohRequest */

/* 532 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 534 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 536 */	NdrFcShort( 0x158 ),	/* Type Offset=344 */

	/* Return value */

/* 538 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 540 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 542 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSoHResponse */

/* 544 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 546 */	NdrFcLong( 0x0 ),	/* 0 */
/* 550 */	NdrFcShort( 0x7 ),	/* 7 */
/* 552 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 554 */	NdrFcShort( 0x0 ),	/* 0 */
/* 556 */	NdrFcShort( 0x21 ),	/* 33 */
/* 558 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 560 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 562 */	NdrFcShort( 0x2 ),	/* 2 */
/* 564 */	NdrFcShort( 0x0 ),	/* 0 */
/* 566 */	NdrFcShort( 0x0 ),	/* 0 */
/* 568 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter sohResponse */

/* 570 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 572 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 574 */	NdrFcShort( 0x158 ),	/* Type Offset=344 */

	/* Parameter flags */

/* 576 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 578 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 580 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Return value */

/* 582 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 584 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 586 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCacheSoHFlag */

/* 588 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 590 */	NdrFcLong( 0x0 ),	/* 0 */
/* 594 */	NdrFcShort( 0x8 ),	/* 8 */
/* 596 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 598 */	NdrFcShort( 0x1c ),	/* 28 */
/* 600 */	NdrFcShort( 0x8 ),	/* 8 */
/* 602 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 604 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 606 */	NdrFcShort( 0x0 ),	/* 0 */
/* 608 */	NdrFcShort( 0x0 ),	/* 0 */
/* 610 */	NdrFcShort( 0x0 ),	/* 0 */
/* 612 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cacheSohForLaterUse */

/* 614 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 616 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 618 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 620 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 622 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 624 */	0x8,		/* FC_LONG */
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
/*  4 */	NdrFcLong( 0xf1072a57 ),	/* -251188649 */
/*  8 */	NdrFcShort( 0x214f ),	/* 8527 */
/* 10 */	NdrFcShort( 0x4ee2 ),	/* 20194 */
/* 12 */	0x83,		/* 131 */
			0x77,		/* 119 */
/* 14 */	0x14,		/* 20 */
			0xef,		/* 239 */
/* 16 */	0x14,		/* 20 */
			0xc,		/* 12 */
/* 18 */	0xd9,		/* 217 */
			0xf3,		/* 243 */
/* 20 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 22 */	NdrFcShort( 0x2 ),	/* Offset= 2 (24) */
/* 24 */	
			0x13, 0x0,	/* FC_OP */
/* 26 */	NdrFcShort( 0x30 ),	/* Offset= 48 (74) */
/* 28 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 30 */	NdrFcShort( 0x8 ),	/* 8 */
/* 32 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 34 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 36 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 38 */	NdrFcLong( 0x0 ),	/* 0 */
/* 42 */	NdrFcLong( 0x400 ),	/* 1024 */
/* 46 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 48 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x57,		/* FC_ADD_1 */
/* 50 */	NdrFcShort( 0x0 ),	/* 0 */
/* 52 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 54 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 56 */	NdrFcShort( 0x10 ),	/* 16 */
/* 58 */	NdrFcShort( 0x0 ),	/* 0 */
/* 60 */	NdrFcShort( 0xa ),	/* Offset= 10 (70) */
/* 62 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 64 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (36) */
/* 66 */	0x42,		/* FC_STRUCTPAD6 */
			0x36,		/* FC_POINTER */
/* 68 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 70 */	
			0x13, 0x0,	/* FC_OP */
/* 72 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (46) */
/* 74 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 76 */	NdrFcShort( 0x20 ),	/* 32 */
/* 78 */	NdrFcShort( 0x0 ),	/* 0 */
/* 80 */	NdrFcShort( 0x0 ),	/* Offset= 0 (80) */
/* 82 */	0xd,		/* FC_ENUM16 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 84 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffc7 ),	/* Offset= -57 (28) */
			0x40,		/* FC_STRUCTPAD4 */
/* 88 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 90 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (54) */
/* 92 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 94 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 96 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 98 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 100 */	NdrFcLong( 0x5b360a69 ),	/* 1530268265 */
/* 104 */	NdrFcShort( 0x212d ),	/* 8493 */
/* 106 */	NdrFcShort( 0x440d ),	/* 17421 */
/* 108 */	0xb3,		/* 179 */
			0x98,		/* 152 */
/* 110 */	0x7e,		/* 126 */
			0xef,		/* 239 */
/* 112 */	0xd4,		/* 212 */
			0x97,		/* 151 */
/* 114 */	0x85,		/* 133 */
			0x3a,		/* 58 */
/* 116 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 118 */	NdrFcShort( 0x2 ),	/* Offset= 2 (120) */
/* 120 */	
			0x13, 0x0,	/* FC_OP */
/* 122 */	NdrFcShort( 0x36 ),	/* Offset= 54 (176) */
/* 124 */	0xb7,		/* FC_RANGE */
			0x2,		/* 2 */
/* 126 */	NdrFcLong( 0x0 ),	/* 0 */
/* 130 */	NdrFcLong( 0x65 ),	/* 101 */
/* 134 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 136 */	NdrFcLong( 0x0 ),	/* 0 */
/* 140 */	NdrFcLong( 0x3e8 ),	/* 1000 */
/* 144 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 146 */	NdrFcShort( 0x4 ),	/* 4 */
/* 148 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 150 */	NdrFcShort( 0x0 ),	/* 0 */
/* 152 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 154 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 156 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 158 */	NdrFcShort( 0x10 ),	/* 16 */
/* 160 */	NdrFcShort( 0x0 ),	/* 0 */
/* 162 */	NdrFcShort( 0xa ),	/* Offset= 10 (172) */
/* 164 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 166 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (134) */
/* 168 */	0x42,		/* FC_STRUCTPAD6 */
			0x36,		/* FC_POINTER */
/* 170 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 172 */	
			0x13, 0x0,	/* FC_OP */
/* 174 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (144) */
/* 176 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 178 */	NdrFcShort( 0x20 ),	/* 32 */
/* 180 */	NdrFcShort( 0x0 ),	/* 0 */
/* 182 */	NdrFcShort( 0x0 ),	/* Offset= 0 (182) */
/* 184 */	0xd,		/* FC_ENUM16 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 186 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffc1 ),	/* Offset= -63 (124) */
			0x3f,		/* FC_STRUCTPAD3 */
/* 190 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 192 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (156) */
/* 194 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 196 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 198 */	
			0x11, 0x0,	/* FC_RP */
/* 200 */	NdrFcShort( 0x4c ),	/* Offset= 76 (276) */
/* 202 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 204 */	NdrFcLong( 0x0 ),	/* 0 */
/* 208 */	NdrFcLong( 0x64 ),	/* 100 */
/* 212 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 214 */	NdrFcLong( 0x0 ),	/* 0 */
/* 218 */	NdrFcLong( 0xfa0 ),	/* 4000 */
/* 222 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 224 */	NdrFcShort( 0x1 ),	/* 1 */
/* 226 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 228 */	NdrFcShort( 0x2 ),	/* 2 */
/* 230 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 232 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 234 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 236 */	NdrFcShort( 0x10 ),	/* 16 */
/* 238 */	NdrFcShort( 0x0 ),	/* 0 */
/* 240 */	NdrFcShort( 0xa ),	/* Offset= 10 (250) */
/* 242 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 244 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffdf ),	/* Offset= -33 (212) */
			0x40,		/* FC_STRUCTPAD4 */
/* 248 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 250 */	
			0x12, 0x0,	/* FC_UP */
/* 252 */	NdrFcShort( 0xffffffe2 ),	/* Offset= -30 (222) */
/* 254 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 256 */	NdrFcShort( 0x0 ),	/* 0 */
/* 258 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x0,		/*  */
/* 260 */	NdrFcShort( 0x0 ),	/* 0 */
/* 262 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 264 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 268 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 270 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 272 */	NdrFcShort( 0xffffffda ),	/* Offset= -38 (234) */
/* 274 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 276 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 278 */	NdrFcShort( 0x10 ),	/* 16 */
/* 280 */	NdrFcShort( 0x0 ),	/* 0 */
/* 282 */	NdrFcShort( 0xa ),	/* Offset= 10 (292) */
/* 284 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 286 */	NdrFcShort( 0xffffffac ),	/* Offset= -84 (202) */
/* 288 */	0x42,		/* FC_STRUCTPAD6 */
			0x36,		/* FC_POINTER */
/* 290 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 292 */	
			0x12, 0x0,	/* FC_UP */
/* 294 */	NdrFcShort( 0xffffffd8 ),	/* Offset= -40 (254) */
/* 296 */	
			0x11, 0x0,	/* FC_RP */
/* 298 */	NdrFcShort( 0x14 ),	/* Offset= 20 (318) */
/* 300 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 302 */	NdrFcShort( 0x8 ),	/* 8 */
/* 304 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 306 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 308 */	NdrFcShort( 0x10 ),	/* 16 */
/* 310 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 312 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 314 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (300) */
			0x5b,		/* FC_END */
/* 318 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 320 */	NdrFcShort( 0x18 ),	/* 24 */
/* 322 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 324 */	NdrFcShort( 0xffffffee ),	/* Offset= -18 (306) */
/* 326 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 328 */	NdrFcShort( 0xfffffed4 ),	/* Offset= -300 (28) */
/* 330 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 332 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 334 */	NdrFcShort( 0xfffffff0 ),	/* Offset= -16 (318) */
/* 336 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 338 */	NdrFcShort( 0x2 ),	/* Offset= 2 (340) */
/* 340 */	
			0x13, 0x0,	/* FC_OP */
/* 342 */	NdrFcShort( 0xfffffee0 ),	/* Offset= -288 (54) */
/* 344 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 346 */	NdrFcShort( 0x2 ),	/* Offset= 2 (348) */
/* 348 */	
			0x13, 0x0,	/* FC_OP */
/* 350 */	NdrFcShort( 0xffffffb6 ),	/* Offset= -74 (276) */
/* 352 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 354 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 356 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 358 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */

			0x0
        }
    };


/* Standard interface: __MIDL_itf_napsystemhealthagent_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: INapSystemHealthAgentBinding, ver. 0.0,
   GUID={0xA6894F43,0x9CC7,0x44c9,{0xA2,0x3F,0x19,0xDB,0xF3,0x6B,0xAD,0x28}} */

#pragma code_seg(".orpc")
static const unsigned short INapSystemHealthAgentBinding_FormatStringOffsetTable[] =
    {
    0,
    44,
    76,
    108,
    152
    };

static const MIDL_STUBLESS_PROXY_INFO INapSystemHealthAgentBinding_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapSystemHealthAgentBinding_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapSystemHealthAgentBinding_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapSystemHealthAgentBinding_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _INapSystemHealthAgentBindingProxyVtbl = 
{
    &INapSystemHealthAgentBinding_ProxyInfo,
    &IID_INapSystemHealthAgentBinding,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* INapSystemHealthAgentBinding::Initialize */ ,
    (void *) (INT_PTR) -1 /* INapSystemHealthAgentBinding::Uninitialize */ ,
    (void *) (INT_PTR) -1 /* INapSystemHealthAgentBinding::NotifySoHChange */ ,
    (void *) (INT_PTR) -1 /* INapSystemHealthAgentBinding::GetSystemIsolationInfo */ ,
    (void *) (INT_PTR) -1 /* INapSystemHealthAgentBinding::FlushCache */
};

const CInterfaceStubVtbl _INapSystemHealthAgentBindingStubVtbl =
{
    &IID_INapSystemHealthAgentBinding,
    &INapSystemHealthAgentBinding_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_napsystemhealthagent_0255, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: INapSystemHealthAgentCallback, ver. 0.0,
   GUID={0xF1072A57,0x214F,0x4ee2,{0x83,0x77,0x14,0xEF,0x14,0x0C,0xD9,0xF3}} */

#pragma code_seg(".orpc")
static const unsigned short INapSystemHealthAgentCallback_FormatStringOffsetTable[] =
    {
    184,
    222,
    76,
    260,
    298,
    348
    };

static const MIDL_STUBLESS_PROXY_INFO INapSystemHealthAgentCallback_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapSystemHealthAgentCallback_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapSystemHealthAgentCallback_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapSystemHealthAgentCallback_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _INapSystemHealthAgentCallbackProxyVtbl = 
{
    &INapSystemHealthAgentCallback_ProxyInfo,
    &IID_INapSystemHealthAgentCallback,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* INapSystemHealthAgentCallback::GetSoHRequest */ ,
    (void *) (INT_PTR) -1 /* INapSystemHealthAgentCallback::ProcessSoHResponse */ ,
    (void *) (INT_PTR) -1 /* INapSystemHealthAgentCallback::NotifySystemIsolationStateChange */ ,
    (void *) (INT_PTR) -1 /* INapSystemHealthAgentCallback::GetFixupInfo */ ,
    (void *) (INT_PTR) -1 /* INapSystemHealthAgentCallback::CompareSoHRequests */ ,
    (void *) (INT_PTR) -1 /* INapSystemHealthAgentCallback::NotifyOrphanedSoHRequest */
};

const CInterfaceStubVtbl _INapSystemHealthAgentCallbackStubVtbl =
{
    &IID_INapSystemHealthAgentCallback,
    &INapSystemHealthAgentCallback_ServerInfo,
    9,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: INapSystemHealthAgentRequest, ver. 0.0,
   GUID={0x5B360A69,0x212D,0x440d,{0xB3,0x98,0x7E,0xEF,0xD4,0x97,0x85,0x3A}} */

#pragma code_seg(".orpc")
static const unsigned short INapSystemHealthAgentRequest_FormatStringOffsetTable[] =
    {
    386,
    424,
    462,
    506,
    544,
    588
    };

static const MIDL_STUBLESS_PROXY_INFO INapSystemHealthAgentRequest_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapSystemHealthAgentRequest_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapSystemHealthAgentRequest_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapSystemHealthAgentRequest_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _INapSystemHealthAgentRequestProxyVtbl = 
{
    &INapSystemHealthAgentRequest_ProxyInfo,
    &IID_INapSystemHealthAgentRequest,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* INapSystemHealthAgentRequest::GetCorrelationId */ ,
    (void *) (INT_PTR) -1 /* INapSystemHealthAgentRequest::GetStringCorrelationId */ ,
    (void *) (INT_PTR) -1 /* INapSystemHealthAgentRequest::SetSoHRequest */ ,
    (void *) (INT_PTR) -1 /* INapSystemHealthAgentRequest::GetSoHRequest */ ,
    (void *) (INT_PTR) -1 /* INapSystemHealthAgentRequest::GetSoHResponse */ ,
    (void *) (INT_PTR) -1 /* INapSystemHealthAgentRequest::GetCacheSoHFlag */
};

const CInterfaceStubVtbl _INapSystemHealthAgentRequestStubVtbl =
{
    &IID_INapSystemHealthAgentRequest,
    &INapSystemHealthAgentRequest_ServerInfo,
    9,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_napsystemhealthagent_0257, ver. 0.0,
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

const CInterfaceProxyVtbl * _napsystemhealthagent_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_INapSystemHealthAgentBindingProxyVtbl,
    ( CInterfaceProxyVtbl *) &_INapSystemHealthAgentCallbackProxyVtbl,
    ( CInterfaceProxyVtbl *) &_INapSystemHealthAgentRequestProxyVtbl,
    0
};

const CInterfaceStubVtbl * _napsystemhealthagent_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_INapSystemHealthAgentBindingStubVtbl,
    ( CInterfaceStubVtbl *) &_INapSystemHealthAgentCallbackStubVtbl,
    ( CInterfaceStubVtbl *) &_INapSystemHealthAgentRequestStubVtbl,
    0
};

PCInterfaceName const _napsystemhealthagent_InterfaceNamesList[] = 
{
    "INapSystemHealthAgentBinding",
    "INapSystemHealthAgentCallback",
    "INapSystemHealthAgentRequest",
    0
};


#define _napsystemhealthagent_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _napsystemhealthagent, pIID, n)

int __stdcall _napsystemhealthagent_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _napsystemhealthagent, 3, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _napsystemhealthagent, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _napsystemhealthagent, 3, *pIndex )
    
}

const ExtendedProxyFileInfo napsystemhealthagent_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _napsystemhealthagent_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _napsystemhealthagent_StubVtblList,
    (const PCInterfaceName * ) & _napsystemhealthagent_InterfaceNamesList,
    0, // no delegation
    & _napsystemhealthagent_IID_Lookup, 
    3,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* defined(_M_IA64) || defined(_M_AMD64)*/

