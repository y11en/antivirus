
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Tue Oct 30 15:53:36 2007
 */
/* Compiler settings for O:\CS AdminKit\development2\nap\nap_sdk\include\napmanagement.idl:
    Os, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if !defined(_M_IA64) && !defined(_M_AMD64)
#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 440
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "napmanagement.h"

#define TYPE_FORMAT_STRING_SIZE   265                               
#define PROC_FORMAT_STRING_SIZE   107                               
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


extern const MIDL_SERVER_INFO INapClientManagement_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapClientManagement_ProxyInfo;


HRESULT STDMETHODCALLTYPE INapClientManagement_GetNapClientInfo_Proxy( 
    INapClientManagement * This,
    /* [out] */ BOOL *isNapEnabled,
    /* [out] */ CountedString **clientName,
    /* [out] */ CountedString **clientDescription,
    /* [out] */ CountedString **protocolVersion)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    if(clientName)
        {
        *clientName = 0;
        }
    if(clientDescription)
        {
        *clientDescription = 0;
        }
    if(protocolVersion)
        {
        *protocolVersion = 0;
        }
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      3);
        
        
        
        if(!isNapEnabled)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        if(!clientName)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        if(!clientDescription)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        if(!protocolVersion)
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[0] );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *isNapEnabled = *(( BOOL * )_StubMsg.Buffer)++;
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&clientName,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6],
                                  (unsigned char)0 );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&clientDescription,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6],
                                  (unsigned char)0 );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&protocolVersion,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6],
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[2],
                         ( void * )isNapEnabled);
        NdrClearOutParameters(
                         ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[6],
                         ( void * )clientName);
        NdrClearOutParameters(
                         ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[6],
                         ( void * )clientDescription);
        NdrClearOutParameters(
                         ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[6],
                         ( void * )protocolVersion);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapClientManagement_GetNapClientInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    BOOL _M0	=	0;
    CountedString *_M1;
    CountedString *_M2;
    CountedString *_M3;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    CountedString **clientDescription;
    CountedString **clientName;
    BOOL *isNapEnabled;
    CountedString **protocolVersion;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( BOOL * )isNapEnabled = 0;
    ( CountedString ** )clientName = 0;
    ( CountedString ** )clientDescription = 0;
    ( CountedString ** )protocolVersion = 0;
    RpcTryFinally
        {
        isNapEnabled = &_M0;
        clientName = &_M1;
        _M1 = 0;
        clientDescription = &_M2;
        _M2 = 0;
        protocolVersion = &_M3;
        _M3 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapClientManagement*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetNapClientInfo(
                    (INapClientManagement *) ((CStdStubBuffer *)This)->pvServerObject,
                    isNapEnabled,
                    clientName,
                    clientDescription,
                    protocolVersion);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 36;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)clientName,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6] );
        
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)clientDescription,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6] );
        
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)protocolVersion,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( BOOL * )_StubMsg.Buffer)++ = *isNapEnabled;
        
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)clientName,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6] );
        
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)clientDescription,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6] );
        
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)protocolVersion,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)clientName,
                        &__MIDL_TypeFormatString.Format[6] );
        
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)clientDescription,
                        &__MIDL_TypeFormatString.Format[6] );
        
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)protocolVersion,
                        &__MIDL_TypeFormatString.Format[6] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapClientManagement_GetSystemIsolationInfo_Proxy( 
    INapClientManagement * This,
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
                      4);
        
        
        
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[18] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&isolationInfo,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[42],
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[42],
                         ( void * )isolationInfo);
        NdrClearOutParameters(
                         ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[2],
                         ( void * )unknownConnections);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapClientManagement_GetSystemIsolationInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    IsolationInfo *_M4;
    BOOL _M5	=	0;
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
        isolationInfo = &_M4;
        _M4 = 0;
        unknownConnections = &_M5;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapClientManagement*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetSystemIsolationInfo(
                          (INapClientManagement *) ((CStdStubBuffer *)This)->pvServerObject,
                          isolationInfo,
                          unknownConnections);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 36;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)isolationInfo,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[42] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)isolationInfo,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[42] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( BOOL * )_StubMsg.Buffer)++ = *unknownConnections;
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)isolationInfo,
                        &__MIDL_TypeFormatString.Format[42] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapClientManagement_RegisterSystemHealthAgent_Proxy( 
    INapClientManagement * This,
    /* [in] */ const NapComponentRegistrationInfo *agent)
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
        
        
        
        if(!agent)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrSimpleStructBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char *)agent,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[122] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                     (unsigned char *)agent,
                                     (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[122] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[28] );
            
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

void __RPC_STUB INapClientManagement_RegisterSystemHealthAgent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    const NapComponentRegistrationInfo *agent;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( NapComponentRegistrationInfo * )agent = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[28] );
        
        NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char * *)&agent,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[122],
                                   (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapClientManagement*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> RegisterSystemHealthAgent((INapClientManagement *) ((CStdStubBuffer *)This)->pvServerObject,agent);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)agent,
                        &__MIDL_TypeFormatString.Format[76] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapClientManagement_UnregisterSystemHealthAgent_Proxy( 
    INapClientManagement * This,
    /* [in] */ SystemHealthEntityId id)
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
            
            _StubMsg.BufferLength = 8;
            NdrProxyGetBuffer(This, &_StubMsg);
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *(( SystemHealthEntityId * )_StubMsg.Buffer)++ = id;
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[34] );
            
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

void __RPC_STUB INapClientManagement_UnregisterSystemHealthAgent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    SystemHealthEntityId id;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    id = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[34] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        
        if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        id = *(( SystemHealthEntityId * )_StubMsg.Buffer)++;
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapClientManagement*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> UnregisterSystemHealthAgent((INapClientManagement *) ((CStdStubBuffer *)This)->pvServerObject,id);
        
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


HRESULT STDMETHODCALLTYPE INapClientManagement_RegisterEnforcementClient_Proxy( 
    INapClientManagement * This,
    /* [in] */ const NapComponentRegistrationInfo *enforcer)
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
        
        
        
        if(!enforcer)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 0;
            NdrSimpleStructBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char *)enforcer,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[122] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                     (unsigned char *)enforcer,
                                     (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[122] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[28] );
            
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

void __RPC_STUB INapClientManagement_RegisterEnforcementClient_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    const NapComponentRegistrationInfo *enforcer;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( NapComponentRegistrationInfo * )enforcer = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[28] );
        
        NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char * *)&enforcer,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[122],
                                   (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapClientManagement*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> RegisterEnforcementClient((INapClientManagement *) ((CStdStubBuffer *)This)->pvServerObject,enforcer);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)enforcer,
                        &__MIDL_TypeFormatString.Format[76] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapClientManagement_UnregisterEnforcementClient_Proxy( 
    INapClientManagement * This,
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
                      8);
        
        
        
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[34] );
            
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

void __RPC_STUB INapClientManagement_UnregisterEnforcementClient_Stub(
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
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[34] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        
        if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        id = *(( EnforcementEntityId * )_StubMsg.Buffer)++;
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapClientManagement*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> UnregisterEnforcementClient((INapClientManagement *) ((CStdStubBuffer *)This)->pvServerObject,id);
        
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

#if !(TARGET_IS_NT40_OR_LATER)
#error You need a Windows NT 4.0 or later to run this stub because it uses these features:
#error   -Oif or -Oicf.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {
			
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/*  2 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/*  4 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/*  6 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */
/*  8 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 10 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */
/* 12 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 14 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */
/* 16 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 18 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 20 */	NdrFcShort( 0x2a ),	/* Type Offset=42 */
/* 22 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 24 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 26 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 28 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 30 */	NdrFcShort( 0x4c ),	/* Type Offset=76 */
/* 32 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 34 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 36 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */

	/* Procedure GetRegisteredSystemHealthAgents */

/* 38 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 40 */	NdrFcLong( 0x0 ),	/* 0 */
/* 44 */	NdrFcShort( 0x9 ),	/* 9 */
/* 46 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 48 */	NdrFcShort( 0x0 ),	/* 0 */
/* 50 */	NdrFcShort( 0x22 ),	/* 34 */
/* 52 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter count */

/* 54 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 56 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 58 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter agents */

/* 60 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 62 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 64 */	NdrFcShort( 0xc8 ),	/* Type Offset=200 */

	/* Return value */

/* 66 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 68 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 70 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetRegisteredEnforcementClients */

/* 72 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 74 */	NdrFcLong( 0x0 ),	/* 0 */
/* 78 */	NdrFcShort( 0xa ),	/* 10 */
/* 80 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 82 */	NdrFcShort( 0x0 ),	/* 0 */
/* 84 */	NdrFcShort( 0x22 ),	/* 34 */
/* 86 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter count */

/* 88 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 90 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 92 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter enforcers */

/* 94 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 96 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 98 */	NdrFcShort( 0xc8 ),	/* Type Offset=200 */

	/* Return value */

/* 100 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 102 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 104 */	0x8,		/* FC_LONG */
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
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/*  4 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/*  8 */	NdrFcShort( 0x2 ),	/* Offset= 2 (10) */
/* 10 */	
			0x13, 0x0,	/* FC_OP */
/* 12 */	NdrFcShort( 0x8 ),	/* Offset= 8 (20) */
/* 14 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 16 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x57,		/* FC_ADD_1 */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 22 */	NdrFcShort( 0x8 ),	/* 8 */
/* 24 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 26 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 28 */	NdrFcShort( 0x4 ),	/* 4 */
/* 30 */	NdrFcShort( 0x4 ),	/* 4 */
/* 32 */	0x13, 0x0,	/* FC_OP */
/* 34 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (14) */
/* 36 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 38 */	0x3e,		/* FC_STRUCTPAD2 */
			0x8,		/* FC_LONG */
/* 40 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 42 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 44 */	NdrFcShort( 0x2 ),	/* Offset= 2 (46) */
/* 46 */	
			0x13, 0x0,	/* FC_OP */
/* 48 */	NdrFcShort( 0xa ),	/* Offset= 10 (58) */
/* 50 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 52 */	NdrFcShort( 0x8 ),	/* 8 */
/* 54 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 56 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 58 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 60 */	NdrFcShort( 0x14 ),	/* 20 */
/* 62 */	NdrFcShort( 0x0 ),	/* 0 */
/* 64 */	NdrFcShort( 0x0 ),	/* Offset= 0 (64) */
/* 66 */	0xd,		/* FC_ENUM16 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 68 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffed ),	/* Offset= -19 (50) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 72 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffcb ),	/* Offset= -53 (20) */
			0x5b,		/* FC_END */
/* 76 */	
			0x11, 0x0,	/* FC_RP */
/* 78 */	NdrFcShort( 0x2c ),	/* Offset= 44 (122) */
/* 80 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 82 */	NdrFcShort( 0x8 ),	/* 8 */
/* 84 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 86 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 88 */	NdrFcShort( 0x10 ),	/* 16 */
/* 90 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 92 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 94 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (80) */
			0x5b,		/* FC_END */
/* 98 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 100 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x57,		/* FC_ADD_1 */
/* 102 */	NdrFcShort( 0x4 ),	/* 4 */
/* 104 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 106 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x57,		/* FC_ADD_1 */
/* 108 */	NdrFcShort( 0xc ),	/* 12 */
/* 110 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 112 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x57,		/* FC_ADD_1 */
/* 114 */	NdrFcShort( 0x14 ),	/* 20 */
/* 116 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 118 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x57,		/* FC_ADD_1 */
/* 120 */	NdrFcShort( 0x1c ),	/* 28 */
/* 122 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 124 */	NdrFcShort( 0x50 ),	/* 80 */
/* 126 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 128 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 130 */	NdrFcShort( 0x8 ),	/* 8 */
/* 132 */	NdrFcShort( 0x8 ),	/* 8 */
/* 134 */	0x12, 0x0,	/* FC_UP */
/* 136 */	NdrFcShort( 0xffffffda ),	/* Offset= -38 (98) */
/* 138 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 140 */	NdrFcShort( 0x10 ),	/* 16 */
/* 142 */	NdrFcShort( 0x10 ),	/* 16 */
/* 144 */	0x12, 0x0,	/* FC_UP */
/* 146 */	NdrFcShort( 0xffffffd6 ),	/* Offset= -42 (104) */
/* 148 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 150 */	NdrFcShort( 0x18 ),	/* 24 */
/* 152 */	NdrFcShort( 0x18 ),	/* 24 */
/* 154 */	0x12, 0x0,	/* FC_UP */
/* 156 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (110) */
/* 158 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 160 */	NdrFcShort( 0x20 ),	/* 32 */
/* 162 */	NdrFcShort( 0x20 ),	/* 32 */
/* 164 */	0x12, 0x0,	/* FC_UP */
/* 166 */	NdrFcShort( 0xffffffce ),	/* Offset= -50 (116) */
/* 168 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 170 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 172 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 174 */	0x3e,		/* FC_STRUCTPAD2 */
			0x8,		/* FC_LONG */
/* 176 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 178 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 180 */	0x3e,		/* FC_STRUCTPAD2 */
			0x8,		/* FC_LONG */
/* 182 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 184 */	NdrFcShort( 0xffffff9e ),	/* Offset= -98 (86) */
/* 186 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 188 */	NdrFcShort( 0xffffff9a ),	/* Offset= -102 (86) */
/* 190 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 192 */	NdrFcShort( 0xffffff72 ),	/* Offset= -142 (50) */
/* 194 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 196 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 198 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 200 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 202 */	NdrFcShort( 0x2 ),	/* Offset= 2 (204) */
/* 204 */	
			0x13, 0x0,	/* FC_OP */
/* 206 */	NdrFcShort( 0x2 ),	/* Offset= 2 (208) */
/* 208 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 210 */	NdrFcShort( 0x50 ),	/* 80 */
/* 212 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x54,		/* FC_DEREFERENCE */
/* 214 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 216 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 218 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 220 */	NdrFcShort( 0x50 ),	/* 80 */
/* 222 */	NdrFcShort( 0x0 ),	/* 0 */
/* 224 */	NdrFcShort( 0x4 ),	/* 4 */
/* 226 */	NdrFcShort( 0x8 ),	/* 8 */
/* 228 */	NdrFcShort( 0x8 ),	/* 8 */
/* 230 */	0x13, 0x0,	/* FC_OP */
/* 232 */	NdrFcShort( 0xffffff7a ),	/* Offset= -134 (98) */
/* 234 */	NdrFcShort( 0x10 ),	/* 16 */
/* 236 */	NdrFcShort( 0x10 ),	/* 16 */
/* 238 */	0x13, 0x0,	/* FC_OP */
/* 240 */	NdrFcShort( 0xffffff78 ),	/* Offset= -136 (104) */
/* 242 */	NdrFcShort( 0x18 ),	/* 24 */
/* 244 */	NdrFcShort( 0x18 ),	/* 24 */
/* 246 */	0x13, 0x0,	/* FC_OP */
/* 248 */	NdrFcShort( 0xffffff76 ),	/* Offset= -138 (110) */
/* 250 */	NdrFcShort( 0x20 ),	/* 32 */
/* 252 */	NdrFcShort( 0x20 ),	/* 32 */
/* 254 */	0x13, 0x0,	/* FC_OP */
/* 256 */	NdrFcShort( 0xffffff74 ),	/* Offset= -140 (116) */
/* 258 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 260 */	0x0,		/* 0 */
			NdrFcShort( 0xffffff75 ),	/* Offset= -139 (122) */
			0x5b,		/* FC_END */

			0x0
        }
    };


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: INapClientManagement, ver. 0.0,
   GUID={0x432a1da5,0x3888,0x4b9a,{0xa7,0x34,0xcf,0xf1,0xe4,0x48,0xc5,0xb9}} */

#pragma code_seg(".orpc")
static const unsigned short INapClientManagement_FormatStringOffsetTable[] =
    {
    0,
    18,
    28,
    34,
    28,
    34,
    38,
    72
    };

static const MIDL_STUBLESS_PROXY_INFO INapClientManagement_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapClientManagement_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapClientManagement_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapClientManagement_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(11) _INapClientManagementProxyVtbl = 
{
    &INapClientManagement_ProxyInfo,
    &IID_INapClientManagement,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    INapClientManagement_GetNapClientInfo_Proxy ,
    INapClientManagement_GetSystemIsolationInfo_Proxy ,
    INapClientManagement_RegisterSystemHealthAgent_Proxy ,
    INapClientManagement_UnregisterSystemHealthAgent_Proxy ,
    INapClientManagement_RegisterEnforcementClient_Proxy ,
    INapClientManagement_UnregisterEnforcementClient_Proxy ,
    (void *) (INT_PTR) -1 /* INapClientManagement::GetRegisteredSystemHealthAgents */ ,
    (void *) (INT_PTR) -1 /* INapClientManagement::GetRegisteredEnforcementClients */
};


static const PRPC_STUB_FUNCTION INapClientManagement_table[] =
{
    INapClientManagement_GetNapClientInfo_Stub,
    INapClientManagement_GetSystemIsolationInfo_Stub,
    INapClientManagement_RegisterSystemHealthAgent_Stub,
    INapClientManagement_UnregisterSystemHealthAgent_Stub,
    INapClientManagement_RegisterEnforcementClient_Stub,
    INapClientManagement_UnregisterEnforcementClient_Stub,
    NdrStubCall2,
    NdrStubCall2
};

const CInterfaceStubVtbl _INapClientManagementStubVtbl =
{
    &IID_INapClientManagement,
    &INapClientManagement_ServerInfo,
    11,
    &INapClientManagement_table[-3],
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_napmanagement_0255, ver. 0.0,
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
    0x20000, /* Ndr library version */
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

const CInterfaceProxyVtbl * _napmanagement_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_INapClientManagementProxyVtbl,
    0
};

const CInterfaceStubVtbl * _napmanagement_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_INapClientManagementStubVtbl,
    0
};

PCInterfaceName const _napmanagement_InterfaceNamesList[] = 
{
    "INapClientManagement",
    0
};


#define _napmanagement_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _napmanagement, pIID, n)

int __stdcall _napmanagement_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_napmanagement_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo napmanagement_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _napmanagement_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _napmanagement_StubVtblList,
    (const PCInterfaceName * ) & _napmanagement_InterfaceNamesList,
    0, // no delegation
    & _napmanagement_IID_Lookup, 
    1,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/


#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Tue Oct 30 15:53:36 2007
 */
/* Compiler settings for O:\CS AdminKit\development2\nap\nap_sdk\include\napmanagement.idl:
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


#include "napmanagement.h"

#define TYPE_FORMAT_STRING_SIZE   207                               
#define PROC_FORMAT_STRING_SIZE   341                               
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


extern const MIDL_SERVER_INFO INapClientManagement_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapClientManagement_ProxyInfo;



#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure GetNapClientInfo */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x24 ),	/* 36 */
/* 14 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x5,		/* 5 */
/* 16 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 18 */	NdrFcShort( 0x3 ),	/* 3 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter isNapEnabled */

/* 26 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 28 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 30 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter clientName */

/* 32 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 34 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 36 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter clientDescription */

/* 38 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 40 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 42 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter protocolVersion */

/* 44 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 46 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 48 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 50 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 52 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 54 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSystemIsolationInfo */

/* 56 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 58 */	NdrFcLong( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x4 ),	/* 4 */
/* 64 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 66 */	NdrFcShort( 0x0 ),	/* 0 */
/* 68 */	NdrFcShort( 0x24 ),	/* 36 */
/* 70 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 72 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 74 */	NdrFcShort( 0x1 ),	/* 1 */
/* 76 */	NdrFcShort( 0x0 ),	/* 0 */
/* 78 */	NdrFcShort( 0x0 ),	/* 0 */
/* 80 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter isolationInfo */

/* 82 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 84 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 86 */	NdrFcShort( 0x34 ),	/* Type Offset=52 */

	/* Parameter unknownConnections */

/* 88 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 90 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 92 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 94 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 96 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 98 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RegisterSystemHealthAgent */

/* 100 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 102 */	NdrFcLong( 0x0 ),	/* 0 */
/* 106 */	NdrFcShort( 0x5 ),	/* 5 */
/* 108 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 110 */	NdrFcShort( 0x0 ),	/* 0 */
/* 112 */	NdrFcShort( 0x8 ),	/* 8 */
/* 114 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 116 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 118 */	NdrFcShort( 0x0 ),	/* 0 */
/* 120 */	NdrFcShort( 0x4 ),	/* 4 */
/* 122 */	NdrFcShort( 0x0 ),	/* 0 */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter agent */

/* 126 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 128 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 130 */	NdrFcShort( 0x6e ),	/* Type Offset=110 */

	/* Return value */

/* 132 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 134 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 136 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UnregisterSystemHealthAgent */

/* 138 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 140 */	NdrFcLong( 0x0 ),	/* 0 */
/* 144 */	NdrFcShort( 0x6 ),	/* 6 */
/* 146 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 148 */	NdrFcShort( 0x8 ),	/* 8 */
/* 150 */	NdrFcShort( 0x8 ),	/* 8 */
/* 152 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 154 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 156 */	NdrFcShort( 0x0 ),	/* 0 */
/* 158 */	NdrFcShort( 0x0 ),	/* 0 */
/* 160 */	NdrFcShort( 0x0 ),	/* 0 */
/* 162 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter id */

/* 164 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 166 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 168 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 170 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 172 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 174 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RegisterEnforcementClient */

/* 176 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 178 */	NdrFcLong( 0x0 ),	/* 0 */
/* 182 */	NdrFcShort( 0x7 ),	/* 7 */
/* 184 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 186 */	NdrFcShort( 0x0 ),	/* 0 */
/* 188 */	NdrFcShort( 0x8 ),	/* 8 */
/* 190 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 192 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 194 */	NdrFcShort( 0x0 ),	/* 0 */
/* 196 */	NdrFcShort( 0x4 ),	/* 4 */
/* 198 */	NdrFcShort( 0x0 ),	/* 0 */
/* 200 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter enforcer */

/* 202 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 204 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 206 */	NdrFcShort( 0x6e ),	/* Type Offset=110 */

	/* Return value */

/* 208 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 210 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 212 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UnregisterEnforcementClient */

/* 214 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 216 */	NdrFcLong( 0x0 ),	/* 0 */
/* 220 */	NdrFcShort( 0x8 ),	/* 8 */
/* 222 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 224 */	NdrFcShort( 0x8 ),	/* 8 */
/* 226 */	NdrFcShort( 0x8 ),	/* 8 */
/* 228 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 230 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 232 */	NdrFcShort( 0x0 ),	/* 0 */
/* 234 */	NdrFcShort( 0x0 ),	/* 0 */
/* 236 */	NdrFcShort( 0x0 ),	/* 0 */
/* 238 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter id */

/* 240 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 242 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 244 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 246 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 248 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 250 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetRegisteredSystemHealthAgents */

/* 252 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 254 */	NdrFcLong( 0x0 ),	/* 0 */
/* 258 */	NdrFcShort( 0x9 ),	/* 9 */
/* 260 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 262 */	NdrFcShort( 0x0 ),	/* 0 */
/* 264 */	NdrFcShort( 0x22 ),	/* 34 */
/* 266 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 268 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 270 */	NdrFcShort( 0x5 ),	/* 5 */
/* 272 */	NdrFcShort( 0x0 ),	/* 0 */
/* 274 */	NdrFcShort( 0x0 ),	/* 0 */
/* 276 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter count */

/* 278 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 280 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 282 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter agents */

/* 284 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 286 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 288 */	NdrFcShort( 0xa4 ),	/* Type Offset=164 */

	/* Return value */

/* 290 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 292 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 294 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetRegisteredEnforcementClients */

/* 296 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 298 */	NdrFcLong( 0x0 ),	/* 0 */
/* 302 */	NdrFcShort( 0xa ),	/* 10 */
/* 304 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 306 */	NdrFcShort( 0x0 ),	/* 0 */
/* 308 */	NdrFcShort( 0x22 ),	/* 34 */
/* 310 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 312 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 314 */	NdrFcShort( 0x5 ),	/* 5 */
/* 316 */	NdrFcShort( 0x0 ),	/* 0 */
/* 318 */	NdrFcShort( 0x0 ),	/* 0 */
/* 320 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter count */

/* 322 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 324 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 326 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter enforcers */

/* 328 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 330 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 332 */	NdrFcShort( 0xa4 ),	/* Type Offset=164 */

	/* Return value */

/* 334 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 336 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 338 */	0x8,		/* FC_LONG */
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
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/*  4 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/*  8 */	NdrFcShort( 0x2 ),	/* Offset= 2 (10) */
/* 10 */	
			0x13, 0x0,	/* FC_OP */
/* 12 */	NdrFcShort( 0x14 ),	/* Offset= 20 (32) */
/* 14 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 16 */	NdrFcLong( 0x0 ),	/* 0 */
/* 20 */	NdrFcLong( 0x400 ),	/* 1024 */
/* 24 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 26 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x57,		/* FC_ADD_1 */
/* 28 */	NdrFcShort( 0x0 ),	/* 0 */
/* 30 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 32 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 34 */	NdrFcShort( 0x10 ),	/* 16 */
/* 36 */	NdrFcShort( 0x0 ),	/* 0 */
/* 38 */	NdrFcShort( 0xa ),	/* Offset= 10 (48) */
/* 40 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 42 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (14) */
/* 44 */	0x42,		/* FC_STRUCTPAD6 */
			0x36,		/* FC_POINTER */
/* 46 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 48 */	
			0x13, 0x0,	/* FC_OP */
/* 50 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (24) */
/* 52 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 54 */	NdrFcShort( 0x2 ),	/* Offset= 2 (56) */
/* 56 */	
			0x13, 0x0,	/* FC_OP */
/* 58 */	NdrFcShort( 0xa ),	/* Offset= 10 (68) */
/* 60 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 62 */	NdrFcShort( 0x8 ),	/* 8 */
/* 64 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 66 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 68 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 70 */	NdrFcShort( 0x20 ),	/* 32 */
/* 72 */	NdrFcShort( 0x0 ),	/* 0 */
/* 74 */	NdrFcShort( 0x0 ),	/* Offset= 0 (74) */
/* 76 */	0xd,		/* FC_ENUM16 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 78 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffed ),	/* Offset= -19 (60) */
			0x40,		/* FC_STRUCTPAD4 */
/* 82 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 84 */	NdrFcShort( 0xffffffcc ),	/* Offset= -52 (32) */
/* 86 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 88 */	
			0x11, 0x0,	/* FC_RP */
/* 90 */	NdrFcShort( 0x14 ),	/* Offset= 20 (110) */
/* 92 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 94 */	NdrFcShort( 0x8 ),	/* 8 */
/* 96 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 98 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 100 */	NdrFcShort( 0x10 ),	/* 16 */
/* 102 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 104 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 106 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (92) */
			0x5b,		/* FC_END */
/* 110 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 112 */	NdrFcShort( 0x78 ),	/* 120 */
/* 114 */	NdrFcShort( 0x0 ),	/* 0 */
/* 116 */	NdrFcShort( 0x0 ),	/* Offset= 0 (116) */
/* 118 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 120 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 122 */	NdrFcShort( 0xffffffa6 ),	/* Offset= -90 (32) */
/* 124 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 126 */	NdrFcShort( 0xffffffa2 ),	/* Offset= -94 (32) */
/* 128 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 130 */	NdrFcShort( 0xffffff9e ),	/* Offset= -98 (32) */
/* 132 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 134 */	NdrFcShort( 0xffffff9a ),	/* Offset= -102 (32) */
/* 136 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 138 */	NdrFcShort( 0xffffffd8 ),	/* Offset= -40 (98) */
/* 140 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 142 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (98) */
/* 144 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 146 */	NdrFcShort( 0xffffffaa ),	/* Offset= -86 (60) */
/* 148 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 150 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 152 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 154 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 156 */	NdrFcLong( 0x0 ),	/* 0 */
/* 160 */	NdrFcLong( 0x14 ),	/* 20 */
/* 164 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 166 */	NdrFcShort( 0x2 ),	/* Offset= 2 (168) */
/* 168 */	
			0x13, 0x0,	/* FC_OP */
/* 170 */	NdrFcShort( 0x2 ),	/* Offset= 2 (172) */
/* 172 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 174 */	NdrFcShort( 0x0 ),	/* 0 */
/* 176 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x54,		/* FC_DEREFERENCE */
/* 178 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 180 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 182 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 186 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 188 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 190 */	NdrFcShort( 0xffffffb0 ),	/* Offset= -80 (110) */
/* 192 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 194 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 196 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 198 */	NdrFcLong( 0x0 ),	/* 0 */
/* 202 */	NdrFcLong( 0x14 ),	/* 20 */

			0x0
        }
    };


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: INapClientManagement, ver. 0.0,
   GUID={0x432a1da5,0x3888,0x4b9a,{0xa7,0x34,0xcf,0xf1,0xe4,0x48,0xc5,0xb9}} */

#pragma code_seg(".orpc")
static const unsigned short INapClientManagement_FormatStringOffsetTable[] =
    {
    0,
    56,
    100,
    138,
    176,
    214,
    252,
    296
    };

static const MIDL_STUBLESS_PROXY_INFO INapClientManagement_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapClientManagement_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapClientManagement_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapClientManagement_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(11) _INapClientManagementProxyVtbl = 
{
    &INapClientManagement_ProxyInfo,
    &IID_INapClientManagement,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* INapClientManagement::GetNapClientInfo */ ,
    (void *) (INT_PTR) -1 /* INapClientManagement::GetSystemIsolationInfo */ ,
    (void *) (INT_PTR) -1 /* INapClientManagement::RegisterSystemHealthAgent */ ,
    (void *) (INT_PTR) -1 /* INapClientManagement::UnregisterSystemHealthAgent */ ,
    (void *) (INT_PTR) -1 /* INapClientManagement::RegisterEnforcementClient */ ,
    (void *) (INT_PTR) -1 /* INapClientManagement::UnregisterEnforcementClient */ ,
    (void *) (INT_PTR) -1 /* INapClientManagement::GetRegisteredSystemHealthAgents */ ,
    (void *) (INT_PTR) -1 /* INapClientManagement::GetRegisteredEnforcementClients */
};

const CInterfaceStubVtbl _INapClientManagementStubVtbl =
{
    &IID_INapClientManagement,
    &INapClientManagement_ServerInfo,
    11,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_napmanagement_0255, ver. 0.0,
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

const CInterfaceProxyVtbl * _napmanagement_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_INapClientManagementProxyVtbl,
    0
};

const CInterfaceStubVtbl * _napmanagement_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_INapClientManagementStubVtbl,
    0
};

PCInterfaceName const _napmanagement_InterfaceNamesList[] = 
{
    "INapClientManagement",
    0
};


#define _napmanagement_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _napmanagement, pIID, n)

int __stdcall _napmanagement_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_napmanagement_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo napmanagement_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _napmanagement_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _napmanagement_StubVtblList,
    (const PCInterfaceName * ) & _napmanagement_InterfaceNamesList,
    0, // no delegation
    & _napmanagement_IID_Lookup, 
    1,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* defined(_M_IA64) || defined(_M_AMD64)*/

