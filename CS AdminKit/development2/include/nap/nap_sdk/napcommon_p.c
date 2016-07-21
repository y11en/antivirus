
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


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

#if !defined(_M_IA64) && !defined(_M_AMD64)

/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 440
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "napcommon.h"

#define TYPE_FORMAT_STRING_SIZE   113                               
#define PROC_FORMAT_STRING_SIZE   57                                
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   1            

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


extern const MIDL_SERVER_INFO INapComponentInfo_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapComponentInfo_ProxyInfo;


HRESULT STDMETHODCALLTYPE INapComponentInfo_GetFriendlyName_Proxy( 
    INapComponentInfo * This,
    /* [out] */ MessageId *friendlyName)
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
        
        
        
        if(!friendlyName)
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
            *friendlyName = *(( MessageId * )_StubMsg.Buffer)++;
            
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
                         ( void * )friendlyName);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapComponentInfo_GetFriendlyName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    MessageId _M0	=	0;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    MessageId *friendlyName;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( MessageId * )friendlyName = 0;
    RpcTryFinally
        {
        friendlyName = &_M0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapComponentInfo*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetFriendlyName((INapComponentInfo *) ((CStdStubBuffer *)This)->pvServerObject,friendlyName);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 36;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( MessageId * )_StubMsg.Buffer)++ = *friendlyName;
        
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


HRESULT STDMETHODCALLTYPE INapComponentInfo_GetDescription_Proxy( 
    INapComponentInfo * This,
    /* [out] */ MessageId *description)
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
        
        
        
        if(!description)
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
            *description = *(( MessageId * )_StubMsg.Buffer)++;
            
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
                         ( void * )description);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapComponentInfo_GetDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    MessageId _M1	=	0;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    MessageId *description;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( MessageId * )description = 0;
    RpcTryFinally
        {
        description = &_M1;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapComponentInfo*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetDescription((INapComponentInfo *) ((CStdStubBuffer *)This)->pvServerObject,description);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 36;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( MessageId * )_StubMsg.Buffer)++ = *description;
        
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


HRESULT STDMETHODCALLTYPE INapComponentInfo_GetVendorName_Proxy( 
    INapComponentInfo * This,
    /* [out] */ MessageId *vendorName)
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
        
        
        
        if(!vendorName)
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
            *vendorName = *(( MessageId * )_StubMsg.Buffer)++;
            
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
                         ( void * )vendorName);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapComponentInfo_GetVendorName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    MessageId _M2	=	0;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    MessageId *vendorName;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( MessageId * )vendorName = 0;
    RpcTryFinally
        {
        vendorName = &_M2;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapComponentInfo*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetVendorName((INapComponentInfo *) ((CStdStubBuffer *)This)->pvServerObject,vendorName);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 36;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( MessageId * )_StubMsg.Buffer)++ = *vendorName;
        
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


HRESULT STDMETHODCALLTYPE INapComponentInfo_GetVersion_Proxy( 
    INapComponentInfo * This,
    /* [out] */ MessageId *version)
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
        
        
        
        if(!version)
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
            *version = *(( MessageId * )_StubMsg.Buffer)++;
            
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
                         ( void * )version);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapComponentInfo_GetVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    MessageId _M3	=	0;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    MessageId *version;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( MessageId * )version = 0;
    RpcTryFinally
        {
        version = &_M3;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapComponentInfo*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetVersion((INapComponentInfo *) ((CStdStubBuffer *)This)->pvServerObject,version);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 36;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( MessageId * )_StubMsg.Buffer)++ = *version;
        
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


HRESULT STDMETHODCALLTYPE INapComponentInfo_GetIcon_Proxy( 
    INapComponentInfo * This,
    /* [out] */ CountedString **dllFilePath,
    /* [out] */ UINT32 *iconResourceId)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    if(dllFilePath)
        {
        *dllFilePath = 0;
        }
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      7);
        
        
        
        if(!dllFilePath)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        if(!iconResourceId)
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[6] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&dllFilePath,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6],
                                  (unsigned char)0 );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *iconResourceId = *(( UINT32 * )_StubMsg.Buffer)++;
            
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[6],
                         ( void * )dllFilePath);
        NdrClearOutParameters(
                         ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[2],
                         ( void * )iconResourceId);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapComponentInfo_GetIcon_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    CountedString *_M4;
    UINT32 _M5	=	0;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    CountedString **dllFilePath;
    UINT32 *iconResourceId;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( CountedString ** )dllFilePath = 0;
    ( UINT32 * )iconResourceId = 0;
    RpcTryFinally
        {
        dllFilePath = &_M4;
        _M4 = 0;
        iconResourceId = &_M5;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapComponentInfo*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetIcon(
           (INapComponentInfo *) ((CStdStubBuffer *)This)->pvServerObject,
           dllFilePath,
           iconResourceId);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 36;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)dllFilePath,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)dllFilePath,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( UINT32 * )_StubMsg.Buffer)++ = *iconResourceId;
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)dllFilePath,
                        &__MIDL_TypeFormatString.Format[6] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapComponentInfo_ConvertErrorCodeToMessageId_Proxy( 
    INapComponentInfo * This,
    /* [in] */ HRESULT errorCode,
    /* [out] */ MessageId *msgId)
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
        
        
        
        if(!msgId)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 8;
            NdrProxyGetBuffer(This, &_StubMsg);
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *(( HRESULT * )_StubMsg.Buffer)++ = errorCode;
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[16] );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *msgId = *(( MessageId * )_StubMsg.Buffer)++;
            
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
                         ( void * )msgId);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapComponentInfo_ConvertErrorCodeToMessageId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    MessageId _M6	=	0;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    HRESULT errorCode;
    MessageId *msgId;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    errorCode = 0;
    ( MessageId * )msgId = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[16] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        
        if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        errorCode = *(( HRESULT * )_StubMsg.Buffer)++;
        
        msgId = &_M6;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapComponentInfo*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> ConvertErrorCodeToMessageId(
                               (INapComponentInfo *) ((CStdStubBuffer *)This)->pvServerObject,
                               errorCode,
                               msgId);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 36;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( MessageId * )_StubMsg.Buffer)++ = *msgId;
        
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


HRESULT STDMETHODCALLTYPE INapComponentInfo_GetLocalizedString_Proxy( 
    INapComponentInfo * This,
    /* [in] */ MessageId msgId,
    /* [out] */ CountedString **string)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    if(string)
        {
        *string = 0;
        }
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      9);
        
        
        
        if(!string)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 8;
            NdrProxyGetBuffer(This, &_StubMsg);
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
            *(( MessageId * )_StubMsg.Buffer)++ = msgId;
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[24] );
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&string,
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[6],
                         ( void * )string);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapComponentInfo_GetLocalizedString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    CountedString *_M7;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    MessageId msgId;
    CountedString **string;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    msgId = 0;
    ( CountedString ** )string = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[24] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        
        if(_StubMsg.Buffer + 4 > _StubMsg.BufferEnd)
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        msgId = *(( MessageId * )_StubMsg.Buffer)++;
        
        string = &_M7;
        _M7 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapComponentInfo*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetLocalizedString(
                      (INapComponentInfo *) ((CStdStubBuffer *)This)->pvServerObject,
                      msgId,
                      string);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)string,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)string,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[6] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)string,
                        &__MIDL_TypeFormatString.Format[6] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO INapComponentConfig_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapComponentConfig_ProxyInfo;


HRESULT STDMETHODCALLTYPE INapComponentConfig_IsUISupported_Proxy( 
    INapComponentConfig * This,
    /* [out] */ BOOL *isSupported)
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
        
        
        
        if(!isSupported)
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
            *isSupported = *(( BOOL * )_StubMsg.Buffer)++;
            
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
                         ( void * )isSupported);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapComponentConfig_IsUISupported_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    BOOL _M8	=	0;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    BOOL *isSupported;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( BOOL * )isSupported = 0;
    RpcTryFinally
        {
        isSupported = &_M8;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapComponentConfig*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> IsUISupported((INapComponentConfig *) ((CStdStubBuffer *)This)->pvServerObject,isSupported);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 36;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( BOOL * )_StubMsg.Buffer)++ = *isSupported;
        
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


HRESULT STDMETHODCALLTYPE INapComponentConfig_InvokeUI_Proxy( 
    INapComponentConfig * This,
    /* [unique][in] */ HWND hwndParent)
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
            NdrUserMarshalBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                      (unsigned char *)&hwndParent,
                                      (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[66] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            NdrUserMarshalMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                    (unsigned char *)&hwndParent,
                                    (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[66] );
            
            NdrProxySendReceive(This, &_StubMsg);
            
            _StubMsg.BufferStart = (unsigned char *) _RpcMessage.Buffer; 
            _StubMsg.BufferEnd   = _StubMsg.BufferStart + _RpcMessage.BufferLength;
            
            if ( (_RpcMessage.DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[32] );
            
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

void __RPC_STUB INapComponentConfig_InvokeUI_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    void *_p_hwndParent;
    HWND hwndParent;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    _p_hwndParent = &hwndParent;
    MIDL_memset(
               _p_hwndParent,
               0,
               sizeof( HWND  ));
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[32] );
        
        NdrUserMarshalUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&_p_hwndParent,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[66],
                                  (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapComponentConfig*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> InvokeUI((INapComponentConfig *) ((CStdStubBuffer *)This)->pvServerObject,hwndParent);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 8;
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrUserMarshalFree( &_StubMsg,
                            (unsigned char *)&hwndParent,
                            &__MIDL_TypeFormatString.Format[66] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapComponentConfig_GetConfig_Proxy( 
    INapComponentConfig * This,
    /* [out] */ UINT16 *bCount,
    /* [size_is][size_is][out] */ BYTE **data)
{

    HRESULT _RetVal;
    
    RPC_MESSAGE _RpcMessage;
    
    MIDL_STUB_MESSAGE _StubMsg;
    
    if(data)
        {
        *data = 0;
        }
    RpcTryExcept
        {
        NdrProxyInitialize(
                      ( void *  )This,
                      ( PRPC_MESSAGE  )&_RpcMessage,
                      ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                      ( PMIDL_STUB_DESC  )&Object_StubDesc,
                      5);
        
        
        
        if(!bCount)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        if(!data)
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
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[38] );
            
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 1) & ~ 0x1);
            *bCount = *(( UINT16 * )_StubMsg.Buffer)++;
            
            NdrPointerUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                  (unsigned char * *)&data,
                                  (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[80],
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
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[76],
                         ( void * )bCount);
        _StubMsg.MaxCount = ( unsigned long  )(bCount ? *bCount : 0);
        
        NdrClearOutParameters(
                         ( PMIDL_STUB_MESSAGE  )&_StubMsg,
                         ( PFORMAT_STRING  )&__MIDL_TypeFormatString.Format[80],
                         ( void * )data);
        _RetVal = NdrProxyErrorHandler(RpcExceptionCode());
        }
    RpcEndExcept
    return _RetVal;
}

void __RPC_STUB INapComponentConfig_GetConfig_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    UINT16 _M10	=	0;
    BYTE *_M11;
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    UINT16 *bCount;
    BYTE **data;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    ( UINT16 * )bCount = 0;
    ( BYTE ** )data = 0;
    RpcTryFinally
        {
        bCount = &_M10;
        data = &_M11;
        _M11 = 0;
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapComponentConfig*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> GetConfig(
             (INapComponentConfig *) ((CStdStubBuffer *)This)->pvServerObject,
             bCount,
             data);
        
        *_pdwStubPhase = STUB_MARSHAL;
        
        _StubMsg.BufferLength = 34;
        _StubMsg.MaxCount = ( unsigned long  )(bCount ? *bCount : 0);
        
        NdrPointerBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                              (unsigned char *)data,
                              (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[80] );
        
        NdrStubGetBuffer(This, _pRpcChannelBuffer, &_StubMsg);
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 1) & ~ 0x1);
        *(( UINT16 * )_StubMsg.Buffer)++ = *bCount;
        
        _StubMsg.MaxCount = ( unsigned long  )(bCount ? *bCount : 0);
        
        NdrPointerMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                            (unsigned char *)data,
                            (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[80] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( HRESULT * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        _StubMsg.MaxCount = ( unsigned long  )(bCount ? *bCount : 0);
        
        NdrPointerFree( &_StubMsg,
                        (unsigned char *)data,
                        &__MIDL_TypeFormatString.Format[80] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}


HRESULT STDMETHODCALLTYPE INapComponentConfig_SetConfig_Proxy( 
    INapComponentConfig * This,
    /* [in] */ UINT16 bCount,
    /* [size_is][in] */ BYTE *data)
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
        
        
        
        if(!data)
            {
            RpcRaiseException(RPC_X_NULL_REF_POINTER);
            }
        RpcTryFinally
            {
            
            _StubMsg.BufferLength = 6;
            _StubMsg.MaxCount = ( unsigned long  )bCount;
            
            NdrConformantArrayBufferSize( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                          (unsigned char *)data,
                                          (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[102] );
            
            NdrProxyGetBuffer(This, &_StubMsg);
            _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 1) & ~ 0x1);
            *(( UINT16 * )_StubMsg.Buffer)++ = bCount;
            
            _StubMsg.MaxCount = ( unsigned long  )bCount;
            
            NdrConformantArrayMarshall( (PMIDL_STUB_MESSAGE)& _StubMsg,
                                        (unsigned char *)data,
                                        (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[102] );
            
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

void __RPC_STUB INapComponentConfig_SetConfig_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase)
{
    HRESULT _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    UINT16 bCount;
    BYTE *data;
    
NdrStubInitialize(
                     _pRpcMessage,
                     &_StubMsg,
                     &Object_StubDesc,
                     _pRpcChannelBuffer);
    bCount = 0;
    ( BYTE * )data = 0;
    RpcTryFinally
        {
        if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
            NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[48] );
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 1) & ~ 0x1);
        
        if(_StubMsg.Buffer + 2 > _StubMsg.BufferEnd)
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        bCount = *(( UINT16 * )_StubMsg.Buffer)++;
        
        NdrConformantArrayUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                      (unsigned char * *)&data,
                                      (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[102],
                                      (unsigned char)0 );
        
        
        *_pdwStubPhase = STUB_CALL_SERVER;
        _RetVal = (((INapComponentConfig*) ((CStdStubBuffer *)This)->pvServerObject)->lpVtbl) -> SetConfig(
             (INapComponentConfig *) ((CStdStubBuffer *)This)->pvServerObject,
             bCount,
             data);
        
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


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT40_OR_LATER)
#error You need a Windows NT 4.0 or later to run this stub because it uses these features:
#error   [wire_marshal] or [user_marshal] attribute.
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
/*  4 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/*  6 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/*  8 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */
/* 10 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 12 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 14 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 16 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 18 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 20 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 22 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 24 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 26 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 28 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */
/* 30 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 32 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 34 */	NdrFcShort( 0x42 ),	/* Type Offset=66 */
/* 36 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 38 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 40 */	NdrFcShort( 0x4c ),	/* Type Offset=76 */
/* 42 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 44 */	NdrFcShort( 0x50 ),	/* Type Offset=80 */
/* 46 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 48 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x6,		/* FC_SHORT */
/* 50 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 52 */	NdrFcShort( 0x62 ),	/* Type Offset=98 */
/* 54 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
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
			0x12, 0x0,	/* FC_UP */
/* 44 */	NdrFcShort( 0x2 ),	/* Offset= 2 (46) */
/* 46 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 48 */	NdrFcShort( 0x4 ),	/* 4 */
/* 50 */	NdrFcShort( 0x2 ),	/* 2 */
/* 52 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 56 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 58 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 62 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 64 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (63) */
/* 66 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 68 */	NdrFcShort( 0x0 ),	/* 0 */
/* 70 */	NdrFcShort( 0x4 ),	/* 4 */
/* 72 */	NdrFcShort( 0x0 ),	/* 0 */
/* 74 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (42) */
/* 76 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 78 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 80 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 82 */	NdrFcShort( 0x2 ),	/* Offset= 2 (84) */
/* 84 */	
			0x13, 0x0,	/* FC_OP */
/* 86 */	NdrFcShort( 0x2 ),	/* Offset= 2 (88) */
/* 88 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 90 */	NdrFcShort( 0x1 ),	/* 1 */
/* 92 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x54,		/* FC_DEREFERENCE */
/* 94 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 96 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 98 */	
			0x11, 0x0,	/* FC_RP */
/* 100 */	NdrFcShort( 0x2 ),	/* Offset= 2 (102) */
/* 102 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 104 */	NdrFcShort( 0x1 ),	/* 1 */
/* 106 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x0,		/*  */
/* 108 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 110 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            HWND_UserSize
            ,HWND_UserMarshal
            ,HWND_UserUnmarshal
            ,HWND_UserFree
            }

        };



/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: INapComponentInfo, ver. 0.0,
   GUID={0xB475F925,0xE3F7,0x414c,{0x8C,0x72,0x1C,0xEE,0x64,0xB9,0xD8,0xF6}} */

#pragma code_seg(".orpc")
static const unsigned short INapComponentInfo_FormatStringOffsetTable[] =
    {
    0,
    0,
    0,
    0,
    6,
    16,
    24
    };

static const MIDL_STUBLESS_PROXY_INFO INapComponentInfo_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapComponentInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapComponentInfo_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapComponentInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
const CINTERFACE_PROXY_VTABLE(10) _INapComponentInfoProxyVtbl = 
{
    &IID_INapComponentInfo,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    INapComponentInfo_GetFriendlyName_Proxy ,
    INapComponentInfo_GetDescription_Proxy ,
    INapComponentInfo_GetVendorName_Proxy ,
    INapComponentInfo_GetVersion_Proxy ,
    INapComponentInfo_GetIcon_Proxy ,
    INapComponentInfo_ConvertErrorCodeToMessageId_Proxy ,
    INapComponentInfo_GetLocalizedString_Proxy
};


static const PRPC_STUB_FUNCTION INapComponentInfo_table[] =
{
    INapComponentInfo_GetFriendlyName_Stub,
    INapComponentInfo_GetDescription_Stub,
    INapComponentInfo_GetVendorName_Stub,
    INapComponentInfo_GetVersion_Stub,
    INapComponentInfo_GetIcon_Stub,
    INapComponentInfo_ConvertErrorCodeToMessageId_Stub,
    INapComponentInfo_GetLocalizedString_Stub
};

const CInterfaceStubVtbl _INapComponentInfoStubVtbl =
{
    &IID_INapComponentInfo,
    &INapComponentInfo_ServerInfo,
    10,
    &INapComponentInfo_table[-3],
    CStdStubBuffer_METHODS
};


/* Object interface: INapComponentConfig, ver. 0.0,
   GUID={0xa9e0af1a,0x3713,0x478e,{0xae,0x03,0x8e,0xdd,0x27,0x2d,0x21,0xfa}} */

#pragma code_seg(".orpc")
static const unsigned short INapComponentConfig_FormatStringOffsetTable[] =
    {
    0,
    32,
    38,
    48
    };

static const MIDL_STUBLESS_PROXY_INFO INapComponentConfig_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapComponentConfig_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapComponentConfig_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapComponentConfig_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
const CINTERFACE_PROXY_VTABLE(7) _INapComponentConfigProxyVtbl = 
{
    &IID_INapComponentConfig,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    INapComponentConfig_IsUISupported_Proxy ,
    INapComponentConfig_InvokeUI_Proxy ,
    INapComponentConfig_GetConfig_Proxy ,
    INapComponentConfig_SetConfig_Proxy
};


static const PRPC_STUB_FUNCTION INapComponentConfig_table[] =
{
    INapComponentConfig_IsUISupported_Stub,
    INapComponentConfig_InvokeUI_Stub,
    INapComponentConfig_GetConfig_Stub,
    INapComponentConfig_SetConfig_Stub
};

const CInterfaceStubVtbl _INapComponentConfigStubVtbl =
{
    &IID_INapComponentConfig,
    &INapComponentConfig_ServerInfo,
    7,
    &INapComponentConfig_table[-3],
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
    0x20000, /* Ndr library version */
    0,
    0x600015b, /* MIDL Version 6.0.347 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0   /* Reserved5 */
    };

const CInterfaceProxyVtbl * _napcommon_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_INapComponentConfigProxyVtbl,
    ( CInterfaceProxyVtbl *) &_INapComponentInfoProxyVtbl,
    0
};

const CInterfaceStubVtbl * _napcommon_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_INapComponentConfigStubVtbl,
    ( CInterfaceStubVtbl *) &_INapComponentInfoStubVtbl,
    0
};

PCInterfaceName const _napcommon_InterfaceNamesList[] = 
{
    "INapComponentConfig",
    "INapComponentInfo",
    0
};


#define _napcommon_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _napcommon, pIID, n)

int __stdcall _napcommon_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _napcommon, 2, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _napcommon, 2, *pIndex )
    
}

const ExtendedProxyFileInfo napcommon_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _napcommon_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _napcommon_StubVtblList,
    (const PCInterfaceName * ) & _napcommon_InterfaceNamesList,
    0, // no delegation
    & _napcommon_IID_Lookup, 
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
/* at Wed Dec 05 11:12:06 2007
 */
/* Compiler settings for O:\CS AdminKit\development2\include\nap\nap_sdk\napcommon.idl:
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


#include "napcommon.h"

#define TYPE_FORMAT_STRING_SIZE   127                               
#define PROC_FORMAT_STRING_SIZE   411                               
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   1            

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


extern const MIDL_SERVER_INFO INapComponentInfo_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapComponentInfo_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO INapComponentConfig_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO INapComponentConfig_ProxyInfo;


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
#endif

static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure IsUISupported */


	/* Procedure GetFriendlyName */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x24 ),	/* 36 */
/* 14 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 16 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter isSupported */


	/* Parameter friendlyName */

/* 26 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 28 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 30 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 32 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 34 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 36 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDescription */

/* 38 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 40 */	NdrFcLong( 0x0 ),	/* 0 */
/* 44 */	NdrFcShort( 0x4 ),	/* 4 */
/* 46 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 48 */	NdrFcShort( 0x0 ),	/* 0 */
/* 50 */	NdrFcShort( 0x24 ),	/* 36 */
/* 52 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 54 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 56 */	NdrFcShort( 0x0 ),	/* 0 */
/* 58 */	NdrFcShort( 0x0 ),	/* 0 */
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter description */

/* 64 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 66 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 68 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 70 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 72 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 74 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetVendorName */

/* 76 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 78 */	NdrFcLong( 0x0 ),	/* 0 */
/* 82 */	NdrFcShort( 0x5 ),	/* 5 */
/* 84 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 86 */	NdrFcShort( 0x0 ),	/* 0 */
/* 88 */	NdrFcShort( 0x24 ),	/* 36 */
/* 90 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 92 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 94 */	NdrFcShort( 0x0 ),	/* 0 */
/* 96 */	NdrFcShort( 0x0 ),	/* 0 */
/* 98 */	NdrFcShort( 0x0 ),	/* 0 */
/* 100 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter vendorName */

/* 102 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 104 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 106 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 108 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 110 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 112 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetVersion */

/* 114 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 116 */	NdrFcLong( 0x0 ),	/* 0 */
/* 120 */	NdrFcShort( 0x6 ),	/* 6 */
/* 122 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */
/* 126 */	NdrFcShort( 0x24 ),	/* 36 */
/* 128 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 130 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 132 */	NdrFcShort( 0x0 ),	/* 0 */
/* 134 */	NdrFcShort( 0x0 ),	/* 0 */
/* 136 */	NdrFcShort( 0x0 ),	/* 0 */
/* 138 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter version */

/* 140 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 142 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 144 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 146 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 148 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 150 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetIcon */

/* 152 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 154 */	NdrFcLong( 0x0 ),	/* 0 */
/* 158 */	NdrFcShort( 0x7 ),	/* 7 */
/* 160 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 162 */	NdrFcShort( 0x0 ),	/* 0 */
/* 164 */	NdrFcShort( 0x24 ),	/* 36 */
/* 166 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 168 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 170 */	NdrFcShort( 0x1 ),	/* 1 */
/* 172 */	NdrFcShort( 0x0 ),	/* 0 */
/* 174 */	NdrFcShort( 0x0 ),	/* 0 */
/* 176 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter dllFilePath */

/* 178 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 180 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 182 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter iconResourceId */

/* 184 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 186 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 188 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 190 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 192 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 194 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ConvertErrorCodeToMessageId */

/* 196 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 198 */	NdrFcLong( 0x0 ),	/* 0 */
/* 202 */	NdrFcShort( 0x8 ),	/* 8 */
/* 204 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 206 */	NdrFcShort( 0x8 ),	/* 8 */
/* 208 */	NdrFcShort( 0x24 ),	/* 36 */
/* 210 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 212 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 214 */	NdrFcShort( 0x0 ),	/* 0 */
/* 216 */	NdrFcShort( 0x0 ),	/* 0 */
/* 218 */	NdrFcShort( 0x0 ),	/* 0 */
/* 220 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter errorCode */

/* 222 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 224 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 226 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter msgId */

/* 228 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 230 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 232 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 234 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 236 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 238 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetLocalizedString */

/* 240 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 242 */	NdrFcLong( 0x0 ),	/* 0 */
/* 246 */	NdrFcShort( 0x9 ),	/* 9 */
/* 248 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 250 */	NdrFcShort( 0x8 ),	/* 8 */
/* 252 */	NdrFcShort( 0x8 ),	/* 8 */
/* 254 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 256 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 258 */	NdrFcShort( 0x1 ),	/* 1 */
/* 260 */	NdrFcShort( 0x0 ),	/* 0 */
/* 262 */	NdrFcShort( 0x0 ),	/* 0 */
/* 264 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter msgId */

/* 266 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 268 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 270 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter string */

/* 272 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 274 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 276 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 278 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 280 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 282 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure InvokeUI */

/* 284 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 286 */	NdrFcLong( 0x0 ),	/* 0 */
/* 290 */	NdrFcShort( 0x4 ),	/* 4 */
/* 292 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 294 */	NdrFcShort( 0x0 ),	/* 0 */
/* 296 */	NdrFcShort( 0x8 ),	/* 8 */
/* 298 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 300 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 302 */	NdrFcShort( 0x0 ),	/* 0 */
/* 304 */	NdrFcShort( 0x1 ),	/* 1 */
/* 306 */	NdrFcShort( 0x0 ),	/* 0 */
/* 308 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hwndParent */

/* 310 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 312 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 314 */	NdrFcShort( 0x4c ),	/* Type Offset=76 */

	/* Return value */

/* 316 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 318 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 320 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetConfig */

/* 322 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 324 */	NdrFcLong( 0x0 ),	/* 0 */
/* 328 */	NdrFcShort( 0x5 ),	/* 5 */
/* 330 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 332 */	NdrFcShort( 0x0 ),	/* 0 */
/* 334 */	NdrFcShort( 0x22 ),	/* 34 */
/* 336 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 338 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 340 */	NdrFcShort( 0x1 ),	/* 1 */
/* 342 */	NdrFcShort( 0x0 ),	/* 0 */
/* 344 */	NdrFcShort( 0x0 ),	/* 0 */
/* 346 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bCount */

/* 348 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 350 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 352 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter data */

/* 354 */	NdrFcShort( 0x2013 ),	/* Flags:  must size, must free, out, srv alloc size=8 */
/* 356 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 358 */	NdrFcShort( 0x5a ),	/* Type Offset=90 */

	/* Return value */

/* 360 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 362 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 364 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetConfig */

/* 366 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 368 */	NdrFcLong( 0x0 ),	/* 0 */
/* 372 */	NdrFcShort( 0x6 ),	/* 6 */
/* 374 */	NdrFcShort( 0x20 ),	/* ia64 Stack size/offset = 32 */
/* 376 */	NdrFcShort( 0x6 ),	/* 6 */
/* 378 */	NdrFcShort( 0x8 ),	/* 8 */
/* 380 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 382 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 384 */	NdrFcShort( 0x0 ),	/* 0 */
/* 386 */	NdrFcShort( 0x1 ),	/* 1 */
/* 388 */	NdrFcShort( 0x0 ),	/* 0 */
/* 390 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter bCount */

/* 392 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 394 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 396 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter data */

/* 398 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 400 */	NdrFcShort( 0x10 ),	/* ia64 Stack size/offset = 16 */
/* 402 */	NdrFcShort( 0x72 ),	/* Type Offset=114 */

	/* Return value */

/* 404 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 406 */	NdrFcShort( 0x18 ),	/* ia64 Stack size/offset = 24 */
/* 408 */	0x8,		/* FC_LONG */
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
			0x12, 0x0,	/* FC_UP */
/* 54 */	NdrFcShort( 0x2 ),	/* Offset= 2 (56) */
/* 56 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x48,		/* 72 */
/* 58 */	NdrFcShort( 0x4 ),	/* 4 */
/* 60 */	NdrFcShort( 0x2 ),	/* 2 */
/* 62 */	NdrFcLong( 0x48746457 ),	/* 1215587415 */
/* 66 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 68 */	NdrFcLong( 0x52746457 ),	/* 1383359575 */
/* 72 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 74 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (73) */
/* 76 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 78 */	NdrFcShort( 0x0 ),	/* 0 */
/* 80 */	NdrFcShort( 0x8 ),	/* 8 */
/* 82 */	NdrFcShort( 0x0 ),	/* 0 */
/* 84 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (52) */
/* 86 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 88 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 90 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 92 */	NdrFcShort( 0x2 ),	/* Offset= 2 (94) */
/* 94 */	
			0x13, 0x0,	/* FC_OP */
/* 96 */	NdrFcShort( 0x2 ),	/* Offset= 2 (98) */
/* 98 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 100 */	NdrFcShort( 0x1 ),	/* 1 */
/* 102 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x54,		/* FC_DEREFERENCE */
/* 104 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 106 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 108 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 110 */	
			0x11, 0x0,	/* FC_RP */
/* 112 */	NdrFcShort( 0x2 ),	/* Offset= 2 (114) */
/* 114 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 116 */	NdrFcShort( 0x1 ),	/* 1 */
/* 118 */	0x27,		/* Corr desc:  parameter, FC_USHORT */
			0x0,		/*  */
/* 120 */	NdrFcShort( 0x8 ),	/* ia64 Stack size/offset = 8 */
/* 122 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 124 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            HWND_UserSize
            ,HWND_UserMarshal
            ,HWND_UserUnmarshal
            ,HWND_UserFree
            }

        };



/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: INapComponentInfo, ver. 0.0,
   GUID={0xB475F925,0xE3F7,0x414c,{0x8C,0x72,0x1C,0xEE,0x64,0xB9,0xD8,0xF6}} */

#pragma code_seg(".orpc")
static const unsigned short INapComponentInfo_FormatStringOffsetTable[] =
    {
    0,
    38,
    76,
    114,
    152,
    196,
    240
    };

static const MIDL_STUBLESS_PROXY_INFO INapComponentInfo_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapComponentInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapComponentInfo_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapComponentInfo_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _INapComponentInfoProxyVtbl = 
{
    &INapComponentInfo_ProxyInfo,
    &IID_INapComponentInfo,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* INapComponentInfo::GetFriendlyName */ ,
    (void *) (INT_PTR) -1 /* INapComponentInfo::GetDescription */ ,
    (void *) (INT_PTR) -1 /* INapComponentInfo::GetVendorName */ ,
    (void *) (INT_PTR) -1 /* INapComponentInfo::GetVersion */ ,
    (void *) (INT_PTR) -1 /* INapComponentInfo::GetIcon */ ,
    (void *) (INT_PTR) -1 /* INapComponentInfo::ConvertErrorCodeToMessageId */ ,
    (void *) (INT_PTR) -1 /* INapComponentInfo::GetLocalizedString */
};

const CInterfaceStubVtbl _INapComponentInfoStubVtbl =
{
    &IID_INapComponentInfo,
    &INapComponentInfo_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: INapComponentConfig, ver. 0.0,
   GUID={0xa9e0af1a,0x3713,0x478e,{0xae,0x03,0x8e,0xdd,0x27,0x2d,0x21,0xfa}} */

#pragma code_seg(".orpc")
static const unsigned short INapComponentConfig_FormatStringOffsetTable[] =
    {
    0,
    284,
    322,
    366
    };

static const MIDL_STUBLESS_PROXY_INFO INapComponentConfig_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &INapComponentConfig_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO INapComponentConfig_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &INapComponentConfig_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _INapComponentConfigProxyVtbl = 
{
    &INapComponentConfig_ProxyInfo,
    &IID_INapComponentConfig,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* INapComponentConfig::IsUISupported */ ,
    (void *) (INT_PTR) -1 /* INapComponentConfig::InvokeUI */ ,
    (void *) (INT_PTR) -1 /* INapComponentConfig::GetConfig */ ,
    (void *) (INT_PTR) -1 /* INapComponentConfig::SetConfig */
};

const CInterfaceStubVtbl _INapComponentConfigStubVtbl =
{
    &IID_INapComponentConfig,
    &INapComponentConfig_ServerInfo,
    7,
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
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0   /* Reserved5 */
    };

const CInterfaceProxyVtbl * _napcommon_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_INapComponentConfigProxyVtbl,
    ( CInterfaceProxyVtbl *) &_INapComponentInfoProxyVtbl,
    0
};

const CInterfaceStubVtbl * _napcommon_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_INapComponentConfigStubVtbl,
    ( CInterfaceStubVtbl *) &_INapComponentInfoStubVtbl,
    0
};

PCInterfaceName const _napcommon_InterfaceNamesList[] = 
{
    "INapComponentConfig",
    "INapComponentInfo",
    0
};


#define _napcommon_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _napcommon, pIID, n)

int __stdcall _napcommon_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _napcommon, 2, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _napcommon, 2, *pIndex )
    
}

const ExtendedProxyFileInfo napcommon_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _napcommon_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _napcommon_StubVtblList,
    (const PCInterfaceName * ) & _napcommon_InterfaceNamesList,
    0, // no delegation
    & _napcommon_IID_Lookup, 
    2,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* defined(_M_IA64) || defined(_M_AMD64)*/

