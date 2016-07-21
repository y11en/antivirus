
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Oct 29 17:32:24 2007
 */
/* Compiler settings for O:\CS AdminKit\development2\nap\nap_sdk\include\napservermanagement.idl:
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


#include "napservermanagement.h"

#define TYPE_FORMAT_STRING_SIZE   281                               
#define PROC_FORMAT_STRING_SIZE   85                                
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


extern const MIDL_SERVER_INFO INapServerInfo_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapServerInfo_ProxyInfo;


HRESULT STDMETHODCALLTYPE INapServerInfo_GetNapServerInfo_Proxy( 
    INapServerInfo * This,
    /* [out] */ CountedString **serverName,
    /* [out] */ CountedString **serverDescription,
    /* [out] */ CountedString **protocolVersion)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    if(serverName)
        {
        *serverName = 0;
        }
    if(serverDescription)
        {
        *serverDescription = 0;
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
        
        
        
        if(!serverName)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        if(!serverDescription)
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
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&serverName,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[2],
                                  (unsigned char)0 );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&serverDescription,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[2],
                                  (unsigned char)0 );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&protocolVersion,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[2],
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
                         ( void * )serverName);
        NdrClearOutParameters(
                         ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[2],
                         ( void * )serverDescription);
        NdrClearOutParameters(
                         ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[2],
                         ( void * )protocolVersion);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapServerInfo_GetNapServerInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    CountedString *_M0;
    CountedString *_M1;
    CountedString *_M2;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    CountedString **protocolVersion;
    CountedString **serverDescription;
    CountedString **serverName;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( CountedString ** )serverName = 0;
    ( CountedString ** )serverDescription = 0;
    ( CountedString ** )protocolVersion = 0;
    RpcTryFinally
        {
        serverName = &_M0;
        _M0 = 0;
        serverDescription = &_M1;
        _M1 = 0;
        protocolVersion = &_M2;
        _M2 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapServerInfo*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetNapServerInfo(
                    (INapServerInfo *) ((CStdStubBuffer *)This)->pvServerObject,
                    serverName,
                    serverDescription,
                    protocolVersion);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)serverName,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[2] );
        
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)serverDescription,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[2] );
        
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)protocolVersion,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[2] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)serverName,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[2] );
        
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)serverDescription,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[2] );
        
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)protocolVersion,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[2] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)serverName,
                        &__MIDL_TypeFormatString.Format[2] );
        
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)serverDescription,
                        &__MIDL_TypeFormatString.Format[2] );
        
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)protocolVersion,
                        &__MIDL_TypeFormatString.Format[2] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapServerInfo_GetFailureCategoryMappings_Proxy( 
    INapServerInfo * This,
    /* [in] */ SystemHealthEntityId id,
    /* [out] */ FailureCategoryMapping *mapping)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    if(mapping)
        {
        MIDL_memset(
               mapping,
               0,
               sizeof( FailureCategoryMapping  ));
        }
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      5);
        
        
        
        if(!mapping)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[54] );
            
            NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char * *)&mapping,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[262],
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[252],
                         ( void * )mapping);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapServerInfo_GetFailureCategoryMappings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    FailureCategoryMapping _mappingM;
    SystemHealthEntityId id;
    FailureCategoryMapping *mapping;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    id = 0;
    ( FailureCategoryMapping * )mapping = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[54] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        
        if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        id = *(( SystemHealthEntityId * )_StubMsg.Buffer)++;
        
        mapping = &_mappingM;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapServerInfo*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetFailureCategoryMappings(
                              (INapServerInfo *) ((CStdStubBuffer *)This)->pvServerObject,
                              id,
                              mapping);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 80;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                 (unsigned char *)mapping,
                                 (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[262] );
        
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


extern const MIDL_SERVER_INFO INapServerManagement_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapServerManagement_ProxyInfo;


HRESULT STDMETHODCALLTYPE INapServerManagement_RegisterSystemHealthValidator_Proxy( 
    INapServerManagement * This,
    /* [in] */ const NapComponentRegistrationInfo *validator,
    /* [in] */ const CLSID *validatorClsid)
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
        
        
        
        if(!validator)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        if(!validatorClsid)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 68;
            NdrSimpleStructBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                       (unsigned char *)validator,
                                       (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[100] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                     (unsigned char *)validator,
                                     (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[100] );
            
            NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                     (unsigned char *)validatorClsid,
                                     (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[56] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[62] );
            
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

void __RPC_STUB INapServerManagement_RegisterSystemHealthValidator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    const NapComponentRegistrationInfo *validator;
    const CLSID *validatorClsid;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( NapComponentRegistrationInfo * )validator = 0;
    ( CLSID * )validatorClsid = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[62] );
        
        NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char * *)&validator,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[100],
                                   (unsigned char)0 );
        
        NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char * *)&validatorClsid,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[56],
                                   (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapServerManagement*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> RegisterSystemHealthValidator(
                                 (INapServerManagement *) ((CStdStubBuffer *)This)->pvServerObject,
                                 validator,
                                 validatorClsid);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)validator,
                        &__MIDL_TypeFormatString.Format[272] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapServerManagement_UnregisterSystemHealthValidator_Proxy( 
    INapServerManagement * This,
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
                      4);
        
        
        
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[72] );
            
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

void __RPC_STUB INapServerManagement_UnregisterSystemHealthValidator_Stub(
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
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[72] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        
        if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        id = *(( SystemHealthEntityId * )_StubMsg.Buffer)++;
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapServerManagement*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> UnregisterSystemHealthValidator((INapServerManagement *) ((CStdStubBuffer *)This)->pvServerObject,id);
        
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


HRESULT STDMETHODCALLTYPE INapServerManagement_SetFailureCategoryMappings_Proxy( 
    INapServerManagement * This,
    /* [in] */ SystemHealthEntityId id,
    /* [in] */ const FailureCategoryMapping mapping)
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
            
            _StubMsg.BufferLength = 60;
            NdrProxyGetBuffer(This, &_StubMsg);
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *(( SystemHealthEntityId * )_StubMsg.Buffer)++ = id;
            
            NdrSimpleStructMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                     (unsigned char *)&mapping,
                                     (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[262] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[76] );
            
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

void __RPC_STUB INapServerManagement_SetFailureCategoryMappings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    void *_p_mapping;
    SystemHealthEntityId id;
    FailureCategoryMapping mapping;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    id = 0;
    _p_mapping = &mapping;
    MIDL_memset(
               _p_mapping,
               0,
               sizeof( FailureCategoryMapping  ));
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[76] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        
        if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        id = *(( SystemHealthEntityId * )_StubMsg.Buffer)++;
        
        NdrSimpleStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                   (unsigned char * *)&_p_mapping,
                                   (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[262],
                                   (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapServerManagement*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> SetFailureCategoryMappings(
                              (INapServerManagement *) ((CStdStubBuffer *)This)->pvServerObject,
                              id,
                              mapping);
        
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
/*  6 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/*  8 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 10 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 12 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */

	/* Procedure GetRegisteredSystemHealthValidators */

/* 14 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 16 */	NdrFcLong( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x4 ),	/* 4 */
/* 22 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */
/* 26 */	NdrFcShort( 0x22 ),	/* 34 */
/* 28 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter count */

/* 30 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 32 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 34 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter validators */

/* 36 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 38 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 40 */	NdrFcShort( 0x2a ),	/* Type Offset=42 */

	/* Parameter validatorClsids */

/* 42 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 44 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 46 */	NdrFcShort( 0xe6 ),	/* Type Offset=230 */

	/* Return value */

/* 48 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 50 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 52 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */
/* 54 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 56 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 58 */	NdrFcShort( 0xfc ),	/* Type Offset=252 */
/* 60 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 62 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 64 */	NdrFcShort( 0x110 ),	/* Type Offset=272 */
/* 66 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 68 */	NdrFcShort( 0x114 ),	/* Type Offset=276 */
/* 70 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 72 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 74 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 76 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 78 */	
			0x4d,		/* FC_IN_PARAM */
			0x5,		/* x86 stack size = 5 */
/* 80 */	NdrFcShort( 0x106 ),	/* Type Offset=262 */
/* 82 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
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
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/*  4 */	NdrFcShort( 0x2 ),	/* Offset= 2 (6) */
/*  6 */	
			0x13, 0x0,	/* FC_OP */
/*  8 */	NdrFcShort( 0x8 ),	/* Offset= 8 (16) */
/* 10 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 12 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x57,		/* FC_ADD_1 */
/* 14 */	NdrFcShort( 0x0 ),	/* 0 */
/* 16 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 18 */	NdrFcShort( 0x8 ),	/* 8 */
/* 20 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 22 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 24 */	NdrFcShort( 0x4 ),	/* 4 */
/* 26 */	NdrFcShort( 0x4 ),	/* 4 */
/* 28 */	0x13, 0x0,	/* FC_OP */
/* 30 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (10) */
/* 32 */	
			0x5b,		/* FC_END */

			0x6,		/* FC_SHORT */
/* 34 */	0x3e,		/* FC_STRUCTPAD2 */
			0x8,		/* FC_LONG */
/* 36 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 38 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 40 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 42 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 44 */	NdrFcShort( 0x2 ),	/* Offset= 2 (46) */
/* 46 */	
			0x13, 0x0,	/* FC_OP */
/* 48 */	NdrFcShort( 0x7e ),	/* Offset= 126 (174) */
/* 50 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 52 */	NdrFcShort( 0x8 ),	/* 8 */
/* 54 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 56 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 58 */	NdrFcShort( 0x10 ),	/* 16 */
/* 60 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 62 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 64 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (50) */
			0x5b,		/* FC_END */
/* 68 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 70 */	NdrFcShort( 0x8 ),	/* 8 */
/* 72 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 74 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 76 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 78 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x57,		/* FC_ADD_1 */
/* 80 */	NdrFcShort( 0x4 ),	/* 4 */
/* 82 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 84 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x57,		/* FC_ADD_1 */
/* 86 */	NdrFcShort( 0xc ),	/* 12 */
/* 88 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 90 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x57,		/* FC_ADD_1 */
/* 92 */	NdrFcShort( 0x14 ),	/* 20 */
/* 94 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 96 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x57,		/* FC_ADD_1 */
/* 98 */	NdrFcShort( 0x1c ),	/* 28 */
/* 100 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 102 */	NdrFcShort( 0x50 ),	/* 80 */
/* 104 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 106 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 108 */	NdrFcShort( 0x8 ),	/* 8 */
/* 110 */	NdrFcShort( 0x8 ),	/* 8 */
/* 112 */	0x13, 0x0,	/* FC_OP */
/* 114 */	NdrFcShort( 0xffffffda ),	/* Offset= -38 (76) */
/* 116 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 118 */	NdrFcShort( 0x10 ),	/* 16 */
/* 120 */	NdrFcShort( 0x10 ),	/* 16 */
/* 122 */	0x13, 0x0,	/* FC_OP */
/* 124 */	NdrFcShort( 0xffffffd6 ),	/* Offset= -42 (82) */
/* 126 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 128 */	NdrFcShort( 0x18 ),	/* 24 */
/* 130 */	NdrFcShort( 0x18 ),	/* 24 */
/* 132 */	0x13, 0x0,	/* FC_OP */
/* 134 */	NdrFcShort( 0xffffffd2 ),	/* Offset= -46 (88) */
/* 136 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 138 */	NdrFcShort( 0x20 ),	/* 32 */
/* 140 */	NdrFcShort( 0x20 ),	/* 32 */
/* 142 */	0x13, 0x0,	/* FC_OP */
/* 144 */	NdrFcShort( 0xffffffce ),	/* Offset= -50 (94) */
/* 146 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 148 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 150 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 152 */	0x3e,		/* FC_STRUCTPAD2 */
			0x8,		/* FC_LONG */
/* 154 */	0x6,		/* FC_SHORT */
			0x3e,		/* FC_STRUCTPAD2 */
/* 156 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 158 */	0x3e,		/* FC_STRUCTPAD2 */
			0x8,		/* FC_LONG */
/* 160 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 162 */	NdrFcShort( 0xffffff96 ),	/* Offset= -106 (56) */
/* 164 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 166 */	NdrFcShort( 0xffffff92 ),	/* Offset= -110 (56) */
/* 168 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 170 */	NdrFcShort( 0xffffff9a ),	/* Offset= -102 (68) */
/* 172 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 174 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 176 */	NdrFcShort( 0x50 ),	/* 80 */
/* 178 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x54,		/* FC_DEREFERENCE */
/* 180 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 182 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 184 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 186 */	NdrFcShort( 0x50 ),	/* 80 */
/* 188 */	NdrFcShort( 0x0 ),	/* 0 */
/* 190 */	NdrFcShort( 0x4 ),	/* 4 */
/* 192 */	NdrFcShort( 0x8 ),	/* 8 */
/* 194 */	NdrFcShort( 0x8 ),	/* 8 */
/* 196 */	0x13, 0x0,	/* FC_OP */
/* 198 */	NdrFcShort( 0xffffff86 ),	/* Offset= -122 (76) */
/* 200 */	NdrFcShort( 0x10 ),	/* 16 */
/* 202 */	NdrFcShort( 0x10 ),	/* 16 */
/* 204 */	0x13, 0x0,	/* FC_OP */
/* 206 */	NdrFcShort( 0xffffff84 ),	/* Offset= -124 (82) */
/* 208 */	NdrFcShort( 0x18 ),	/* 24 */
/* 210 */	NdrFcShort( 0x18 ),	/* 24 */
/* 212 */	0x13, 0x0,	/* FC_OP */
/* 214 */	NdrFcShort( 0xffffff82 ),	/* Offset= -126 (88) */
/* 216 */	NdrFcShort( 0x20 ),	/* 32 */
/* 218 */	NdrFcShort( 0x20 ),	/* 32 */
/* 220 */	0x13, 0x0,	/* FC_OP */
/* 222 */	NdrFcShort( 0xffffff80 ),	/* Offset= -128 (94) */
/* 224 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 226 */	0x0,		/* 0 */
			NdrFcShort( 0xffffff81 ),	/* Offset= -127 (100) */
			0x5b,		/* FC_END */
/* 230 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 232 */	NdrFcShort( 0x2 ),	/* Offset= 2 (234) */
/* 234 */	
			0x13, 0x0,	/* FC_OP */
/* 236 */	NdrFcShort( 0x2 ),	/* Offset= 2 (238) */
/* 238 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 240 */	NdrFcShort( 0x10 ),	/* 16 */
/* 242 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x54,		/* FC_DEREFERENCE */
/* 244 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 246 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 248 */	NdrFcShort( 0xffffff40 ),	/* Offset= -192 (56) */
/* 250 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 252 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 254 */	NdrFcShort( 0x8 ),	/* Offset= 8 (262) */
/* 256 */	
			0x1d,		/* FC_SMFARRAY */
			0x3,		/* 3 */
/* 258 */	NdrFcShort( 0x14 ),	/* 20 */
/* 260 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 262 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 264 */	NdrFcShort( 0x14 ),	/* 20 */
/* 266 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 268 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (256) */
/* 270 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 272 */	
			0x11, 0x0,	/* FC_RP */
/* 274 */	NdrFcShort( 0xffffff52 ),	/* Offset= -174 (100) */
/* 276 */	
			0x11, 0x0,	/* FC_RP */
/* 278 */	NdrFcShort( 0xffffff22 ),	/* Offset= -222 (56) */

			0x0
        }
    };


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: INapServerInfo, ver. 0.0,
   GUID={0x599F9021,0x5643,0x4965,{0x99,0x49,0xE8,0x89,0x75,0xEF,0xFF,0x0E}} */

#pragma code_seg(".orpc")
static const unsigned short INapServerInfo_FormatStringOffsetTable[] =
    {
    0,
    14,
    54
    };

static const MIDL_STUBLESS_PROXY_INFO INapServerInfo_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapServerInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapServerInfo_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapServerInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(6) _INapServerInfoProxyVtbl = 
{
    &INapServerInfo_ProxyInfo,
    &IID_INapServerInfo,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    INapServerInfo_GetNapServerInfo_Proxy ,
    (void *) (INT_PTR) -1 /* INapServerInfo::GetRegisteredSystemHealthValidators */ ,
    INapServerInfo_GetFailureCategoryMappings_Proxy
};


static const PRPC_STUB_FUNCTION INapServerInfo_table[] =
{
    INapServerInfo_GetNapServerInfo_Stub,
    NdrStubCall2,
    INapServerInfo_GetFailureCategoryMappings_Stub
};

const CInterfaceStubVtbl _INapServerInfoStubVtbl =
{
    &IID_INapServerInfo,
    &INapServerInfo_ServerInfo,
    6,
    &INapServerInfo_table[-3],
    CStdStubBuffer_METHODS
};


/* Object interface: INapServerManagement, ver. 0.0,
   GUID={0x9DE543E7,0x0F23,0x47e0,{0xA8,0xBC,0x97,0x1A,0x89,0x4F,0x86,0xD4}} */

#pragma code_seg(".orpc")
static const unsigned short INapServerManagement_FormatStringOffsetTable[] =
    {
    62,
    72,
    76
    };

static const MIDL_STUBLESS_PROXY_INFO INapServerManagement_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapServerManagement_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapServerManagement_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapServerManagement_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
const CINTERFACE_PROXY_VTABLE(6) _INapServerManagementProxyVtbl = 
{
    0,    /* dummy for table ver 2 */
    &IID_INapServerManagement,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    INapServerManagement_RegisterSystemHealthValidator_Proxy ,
    INapServerManagement_UnregisterSystemHealthValidator_Proxy ,
    INapServerManagement_SetFailureCategoryMappings_Proxy
};


static const PRPC_STUB_FUNCTION INapServerManagement_table[] =
{
    INapServerManagement_RegisterSystemHealthValidator_Stub,
    INapServerManagement_UnregisterSystemHealthValidator_Stub,
    INapServerManagement_SetFailureCategoryMappings_Stub
};

const CInterfaceStubVtbl _INapServerManagementStubVtbl =
{
    &IID_INapServerManagement,
    &INapServerManagement_ServerInfo,
    6,
    &INapServerManagement_table[-3],
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_napservermanagement_0256, ver. 0.0,
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

const CInterfaceProxyVtbl * _napservermanagement_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_INapServerInfoProxyVtbl,
    ( CInterfaceProxyVtbl *) &_INapServerManagementProxyVtbl,
    0
};

const CInterfaceStubVtbl * _napservermanagement_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_INapServerInfoStubVtbl,
    ( CInterfaceStubVtbl *) &_INapServerManagementStubVtbl,
    0
};

PCInterfaceName const _napservermanagement_InterfaceNamesList[] = 
{
    "INapServerInfo",
    "INapServerManagement",
    0
};


#define _napservermanagement_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _napservermanagement, pIID, n)

int __stdcall _napservermanagement_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _napservermanagement, 2, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _napservermanagement, 2, *pIndex )
    
}

const ExtendedProxyFileInfo napservermanagement_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _napservermanagement_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _napservermanagement_StubVtblList,
    (const PCInterfaceName * ) & _napservermanagement_InterfaceNamesList,
    0, // no delegation
    & _napservermanagement_IID_Lookup, 
    2,
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
/* at Mon Oct 29 17:32:24 2007
 */
/* Compiler settings for O:\CS AdminKit\development2\nap\nap_sdk\include\napservermanagement.idl:
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


#include "napservermanagement.h"

#define TYPE_FORMAT_STRING_SIZE   211                               
#define PROC_FORMAT_STRING_SIZE   271                               
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


extern const MIDL_SERVER_INFO INapServerInfo_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapServerInfo_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO INapServerManagement_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapServerManagement_ProxyInfo;



#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure GetNapServerInfo */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 16 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 18 */	NdrFcShort( 0x3 ),	/* 3 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter serverName */

/* 26 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 28 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 30 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter serverDescription */

/* 32 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 34 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 36 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter protocolVersion */

/* 38 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 40 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 42 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Return value */

/* 44 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 46 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 48 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetRegisteredSystemHealthValidators */

/* 50 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 52 */	NdrFcLong( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x4 ),	/* 4 */
/* 58 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x22 ),	/* 34 */
/* 64 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x4,		/* 4 */
/* 66 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 68 */	NdrFcShort( 0x6 ),	/* 6 */
/* 70 */	NdrFcShort( 0x0 ),	/* 0 */
/* 72 */	NdrFcShort( 0x0 ),	/* 0 */
/* 74 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter count */

/* 76 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 78 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 80 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter validators */

/* 82 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 84 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 86 */	NdrFcShort( 0x3c ),	/* Type Offset=60 */

	/* Parameter validatorClsids */

/* 88 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 90 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 92 */	NdrFcShort( 0x9e ),	/* Type Offset=158 */

	/* Return value */

/* 94 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 96 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 98 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFailureCategoryMappings */

/* 100 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 102 */	NdrFcLong( 0x0 ),	/* 0 */
/* 106 */	NdrFcShort( 0x5 ),	/* 5 */
/* 108 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 110 */	NdrFcShort( 0x8 ),	/* 8 */
/* 112 */	NdrFcShort( 0x50 ),	/* 80 */
/* 114 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 116 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 118 */	NdrFcShort( 0x0 ),	/* 0 */
/* 120 */	NdrFcShort( 0x0 ),	/* 0 */
/* 122 */	NdrFcShort( 0x0 ),	/* 0 */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter id */

/* 126 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 128 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 130 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter mapping */

/* 132 */	NdrFcShort( 0x6112 ),	/* Flags:  must free, out, simple ref, srv alloc size=24 */
/* 134 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 136 */	NdrFcShort( 0xc0 ),	/* Type Offset=192 */

	/* Return value */

/* 138 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 140 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RegisterSystemHealthValidator */

/* 144 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 146 */	NdrFcLong( 0x0 ),	/* 0 */
/* 150 */	NdrFcShort( 0x3 ),	/* 3 */
/* 152 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 154 */	NdrFcShort( 0x44 ),	/* 68 */
/* 156 */	NdrFcShort( 0x8 ),	/* 8 */
/* 158 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 160 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 162 */	NdrFcShort( 0x0 ),	/* 0 */
/* 164 */	NdrFcShort( 0x4 ),	/* 4 */
/* 166 */	NdrFcShort( 0x0 ),	/* 0 */
/* 168 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter validator */

/* 170 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 172 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 174 */	NdrFcShort( 0x5e ),	/* Type Offset=94 */

	/* Parameter validatorClsid */

/* 176 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 178 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 180 */	NdrFcShort( 0x4a ),	/* Type Offset=74 */

	/* Return value */

/* 182 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 184 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 186 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UnregisterSystemHealthValidator */

/* 188 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 190 */	NdrFcLong( 0x0 ),	/* 0 */
/* 194 */	NdrFcShort( 0x4 ),	/* 4 */
/* 196 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 198 */	NdrFcShort( 0x8 ),	/* 8 */
/* 200 */	NdrFcShort( 0x8 ),	/* 8 */
/* 202 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 204 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 206 */	NdrFcShort( 0x0 ),	/* 0 */
/* 208 */	NdrFcShort( 0x0 ),	/* 0 */
/* 210 */	NdrFcShort( 0x0 ),	/* 0 */
/* 212 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter id */

/* 214 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 216 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 218 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 220 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 222 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 224 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetFailureCategoryMappings */

/* 226 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 228 */	NdrFcLong( 0x0 ),	/* 0 */
/* 232 */	NdrFcShort( 0x5 ),	/* 5 */
/* 234 */	NdrFcShort( 0x30 ),	/* ia64 Stack size/offset = 48 */
/* 236 */	NdrFcShort( 0x3c ),	/* 60 */
/* 238 */	NdrFcShort( 0x8 ),	/* 8 */
/* 240 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 242 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 244 */	NdrFcShort( 0x0 ),	/* 0 */
/* 246 */	NdrFcShort( 0x0 ),	/* 0 */
/* 248 */	NdrFcShort( 0x0 ),	/* 0 */
/* 250 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter id */

/* 252 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 254 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 256 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter mapping */

/* 258 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 260 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 262 */	NdrFcShort( 0xc0 ),	/* Type Offset=192 */

	/* Return value */

/* 264 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 266 */	NdrFcShort( 0x28 ),	/* ia64 Stack size/offset = 40 */
/* 268 */	0x8,		/* FC_LONG */
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
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/*  4 */	NdrFcShort( 0x2 ),	/* Offset= 2 (6) */
/*  6 */	
			0x13, 0x0,	/* FC_OP */
/*  8 */	NdrFcShort( 0x14 ),	/* Offset= 20 (28) */
/* 10 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 12 */	NdrFcLong( 0x0 ),	/* 0 */
/* 16 */	NdrFcLong( 0x400 ),	/* 1024 */
/* 20 */	
			0x25,		/* FC_C_WSTRING */
			0x44,		/* FC_STRING_SIZED */
/* 22 */	0x17,		/* Corr desc:  field pointer, FC_USHORT */
			0x57,		/* FC_ADD_1 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */
/* 26 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 28 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 30 */	NdrFcShort( 0x10 ),	/* 16 */
/* 32 */	NdrFcShort( 0x0 ),	/* 0 */
/* 34 */	NdrFcShort( 0xa ),	/* Offset= 10 (44) */
/* 36 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 38 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (10) */
/* 40 */	0x42,		/* FC_STRUCTPAD6 */
			0x36,		/* FC_POINTER */
/* 42 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 44 */	
			0x13, 0x0,	/* FC_OP */
/* 46 */	NdrFcShort( 0xffffffe6 ),	/* Offset= -26 (20) */
/* 48 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 50 */	0xb7,		/* FC_RANGE */
			0x6,		/* 6 */
/* 52 */	NdrFcLong( 0x0 ),	/* 0 */
/* 56 */	NdrFcLong( 0x14 ),	/* 20 */
/* 60 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 62 */	NdrFcShort( 0x2 ),	/* Offset= 2 (64) */
/* 64 */	
			0x13, 0x0,	/* FC_OP */
/* 66 */	NdrFcShort( 0x46 ),	/* Offset= 70 (136) */
/* 68 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 70 */	NdrFcShort( 0x8 ),	/* 8 */
/* 72 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 74 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 76 */	NdrFcShort( 0x10 ),	/* 16 */
/* 78 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 80 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 82 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (68) */
			0x5b,		/* FC_END */
/* 86 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 88 */	NdrFcShort( 0x8 ),	/* 8 */
/* 90 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 92 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 94 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 96 */	NdrFcShort( 0x78 ),	/* 120 */
/* 98 */	NdrFcShort( 0x0 ),	/* 0 */
/* 100 */	NdrFcShort( 0x0 ),	/* Offset= 0 (100) */
/* 102 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 104 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 106 */	NdrFcShort( 0xffffffb2 ),	/* Offset= -78 (28) */
/* 108 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 110 */	NdrFcShort( 0xffffffae ),	/* Offset= -82 (28) */
/* 112 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 114 */	NdrFcShort( 0xffffffaa ),	/* Offset= -86 (28) */
/* 116 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 118 */	NdrFcShort( 0xffffffa6 ),	/* Offset= -90 (28) */
/* 120 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 122 */	NdrFcShort( 0xffffffd0 ),	/* Offset= -48 (74) */
/* 124 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 126 */	NdrFcShort( 0xffffffcc ),	/* Offset= -52 (74) */
/* 128 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 130 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (86) */
/* 132 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 134 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 136 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 140 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x54,		/* FC_DEREFERENCE */
/* 142 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 144 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 146 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 150 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 152 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 154 */	NdrFcShort( 0xffffffc4 ),	/* Offset= -60 (94) */
/* 156 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 158 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 160 */	NdrFcShort( 0x2 ),	/* Offset= 2 (162) */
/* 162 */	
			0x13, 0x0,	/* FC_OP */
/* 164 */	NdrFcShort( 0x2 ),	/* Offset= 2 (166) */
/* 166 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 168 */	NdrFcShort( 0x10 ),	/* 16 */
/* 170 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x54,		/* FC_DEREFERENCE */
/* 172 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 174 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 176 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 178 */	NdrFcShort( 0xffffff98 ),	/* Offset= -104 (74) */
/* 180 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 182 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 184 */	NdrFcShort( 0x8 ),	/* Offset= 8 (192) */
/* 186 */	
			0x1d,		/* FC_SMFARRAY */
			0x3,		/* 3 */
/* 188 */	NdrFcShort( 0x14 ),	/* 20 */
/* 190 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 192 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 194 */	NdrFcShort( 0x14 ),	/* 20 */
/* 196 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 198 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (186) */
/* 200 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 202 */	
			0x11, 0x0,	/* FC_RP */
/* 204 */	NdrFcShort( 0xffffff92 ),	/* Offset= -110 (94) */
/* 206 */	
			0x11, 0x0,	/* FC_RP */
/* 208 */	NdrFcShort( 0xffffff7a ),	/* Offset= -134 (74) */

			0x0
        }
    };


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: INapServerInfo, ver. 0.0,
   GUID={0x599F9021,0x5643,0x4965,{0x99,0x49,0xE8,0x89,0x75,0xEF,0xFF,0x0E}} */

#pragma code_seg(".orpc")
static const unsigned short INapServerInfo_FormatStringOffsetTable[] =
    {
    0,
    50,
    100
    };

static const MIDL_STUBLESS_PROXY_INFO INapServerInfo_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapServerInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapServerInfo_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapServerInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(6) _INapServerInfoProxyVtbl = 
{
    &INapServerInfo_ProxyInfo,
    &IID_INapServerInfo,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* INapServerInfo::GetNapServerInfo */ ,
    (void *) (INT_PTR) -1 /* INapServerInfo::GetRegisteredSystemHealthValidators */ ,
    (void *) (INT_PTR) -1 /* INapServerInfo::GetFailureCategoryMappings */
};

const CInterfaceStubVtbl _INapServerInfoStubVtbl =
{
    &IID_INapServerInfo,
    &INapServerInfo_ServerInfo,
    6,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: INapServerManagement, ver. 0.0,
   GUID={0x9DE543E7,0x0F23,0x47e0,{0xA8,0xBC,0x97,0x1A,0x89,0x4F,0x86,0xD4}} */

#pragma code_seg(".orpc")
static const unsigned short INapServerManagement_FormatStringOffsetTable[] =
    {
    144,
    188,
    226
    };

static const MIDL_STUBLESS_PROXY_INFO INapServerManagement_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapServerManagement_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapServerManagement_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapServerManagement_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(6) _INapServerManagementProxyVtbl = 
{
    &INapServerManagement_ProxyInfo,
    &IID_INapServerManagement,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* INapServerManagement::RegisterSystemHealthValidator */ ,
    (void *) (INT_PTR) -1 /* INapServerManagement::UnregisterSystemHealthValidator */ ,
    (void *) (INT_PTR) -1 /* INapServerManagement::SetFailureCategoryMappings */
};

const CInterfaceStubVtbl _INapServerManagementStubVtbl =
{
    &IID_INapServerManagement,
    &INapServerManagement_ServerInfo,
    6,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_napservermanagement_0256, ver. 0.0,
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

const CInterfaceProxyVtbl * _napservermanagement_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_INapServerInfoProxyVtbl,
    ( CInterfaceProxyVtbl *) &_INapServerManagementProxyVtbl,
    0
};

const CInterfaceStubVtbl * _napservermanagement_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_INapServerInfoStubVtbl,
    ( CInterfaceStubVtbl *) &_INapServerManagementStubVtbl,
    0
};

PCInterfaceName const _napservermanagement_InterfaceNamesList[] = 
{
    "INapServerInfo",
    "INapServerManagement",
    0
};


#define _napservermanagement_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _napservermanagement, pIID, n)

int __stdcall _napservermanagement_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _napservermanagement, 2, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _napservermanagement, 2, *pIndex )
    
}

const ExtendedProxyFileInfo napservermanagement_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _napservermanagement_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _napservermanagement_StubVtblList,
    (const PCInterfaceName * ) & _napservermanagement_InterfaceNamesList,
    0, // no delegation
    & _napservermanagement_IID_Lookup, 
    2,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* defined(_M_IA64) || defined(_M_AMD64)*/

