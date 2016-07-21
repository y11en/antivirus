
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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

#ifndef __napprotocol_h__
#define __napprotocol_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __INapSoHConstructor_FWD_DEFINED__
#define __INapSoHConstructor_FWD_DEFINED__
typedef interface INapSoHConstructor INapSoHConstructor;
#endif 	/* __INapSoHConstructor_FWD_DEFINED__ */


#ifndef __INapSoHProcessor_FWD_DEFINED__
#define __INapSoHProcessor_FWD_DEFINED__
typedef interface INapSoHProcessor INapSoHProcessor;
#endif 	/* __INapSoHProcessor_FWD_DEFINED__ */


/* header files for imported files */
#include "NapTypes.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __INapSoHTypes_INTERFACE_DEFINED__
#define __INapSoHTypes_INTERFACE_DEFINED__

/* interface INapSoHTypes */
/* [auto_handle][unique] */ 

typedef 
enum tagSoHAttributeType
    {	sohAttributeTypeSystemHealthId	= 2,
	sohAttributeTypeIpv4FixupServers	= 3,
	sohAttributeTypeComplianceResultCodes	= 4,
	sohAttributeTypeTimeOfLastUpdate	= 5,
	sohAttributeTypeClientId	= 6,
	sohAttributeTypeVendorSpecific	= 7,
	sohAttributeTypeHealthClass	= 8,
	sohAttributeTypeSoftwareVersion	= 9,
	sohAttributeTypeProductName	= 10,
	sohAttributeTypeHealthClassStatus	= 11,
	sohAttributeTypeSoHGenerationTime	= 12,
	sohAttributeTypeErrorCodes	= 13,
	sohAttributeTypeFailureCategory	= 14,
	sohAttributeTypeIpv6FixupServers	= 15
    } 	SoHAttributeType;

typedef 
enum tagHealthClassValue
    {	healthClassFirewall	= 0,
	healthClassPatchLevel	= 1,
	healthClassAntiVirus	= 2,
	healthClassCriticalUpdate	= 3,
	healthClassReserved	= 128
    } 	HealthClassValue;

typedef /* [switch_type] */ union tagSoHAttributeValue
    {
    /* [case()] */ SystemHealthEntityId idVal;
    /* [case()] */ struct tagIpv4Addresses
        {
        /* [range] */ UINT16 count;
        /* [size_is] */ Ipv4Address *addresses;
        } 	v4AddressesVal;
    /* [case()] */ struct tagIpv6Addresses
        {
        /* [range] */ UINT16 count;
        /* [size_is] */ Ipv6Address *addresses;
        } 	v6AddressesVal;
    /* [case()] */ ResultCodes codesVal;
    /* [case()] */ FILETIME dateTimeVal;
    /* [case()] */ struct tagVendorSpecific
        {
        UINT32 vendorId;
        /* [range] */ UINT16 size;
        /* [size_is] */ BYTE *vendorSpecificData;
        } 	vendorSpecificVal;
    /* [case()] */ UINT8 uint8Val;
    /* [default] */ struct tagOctetString
        {
        /* [range] */ UINT16 size;
        /* [size_is] */ BYTE *data;
        } 	octetStringVal;
    } 	SoHAttributeValue;



extern RPC_IF_HANDLE INapSoHTypes_v0_0_c_ifspec;
extern RPC_IF_HANDLE INapSoHTypes_v0_0_s_ifspec;
#endif /* __INapSoHTypes_INTERFACE_DEFINED__ */

#ifndef __INapSoHConstructor_INTERFACE_DEFINED__
#define __INapSoHConstructor_INTERFACE_DEFINED__

/* interface INapSoHConstructor */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_INapSoHConstructor;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("35298344-96A6-45e7-9B6B-62ECC6E09920")
    INapSoHConstructor : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ SystemHealthEntityId id,
            /* [in] */ BOOL isRequest) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AppendAttribute( 
            /* [in] */ SoHAttributeType type,
            /* [switch_is][in] */ const SoHAttributeValue *value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSoH( 
            /* [out] */ SoH **soh) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Validate( 
            /* [in] */ const SoH *soh,
            /* [in] */ BOOL isRequest) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INapSoHConstructorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INapSoHConstructor * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INapSoHConstructor * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INapSoHConstructor * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            INapSoHConstructor * This,
            /* [in] */ SystemHealthEntityId id,
            /* [in] */ BOOL isRequest);
        
        HRESULT ( STDMETHODCALLTYPE *AppendAttribute )( 
            INapSoHConstructor * This,
            /* [in] */ SoHAttributeType type,
            /* [switch_is][in] */ const SoHAttributeValue *value);
        
        HRESULT ( STDMETHODCALLTYPE *GetSoH )( 
            INapSoHConstructor * This,
            /* [out] */ SoH **soh);
        
        HRESULT ( STDMETHODCALLTYPE *Validate )( 
            INapSoHConstructor * This,
            /* [in] */ const SoH *soh,
            /* [in] */ BOOL isRequest);
        
        END_INTERFACE
    } INapSoHConstructorVtbl;

    interface INapSoHConstructor
    {
        CONST_VTBL struct INapSoHConstructorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INapSoHConstructor_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INapSoHConstructor_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INapSoHConstructor_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INapSoHConstructor_Initialize(This,id,isRequest)	\
    (This)->lpVtbl -> Initialize(This,id,isRequest)

#define INapSoHConstructor_AppendAttribute(This,type,value)	\
    (This)->lpVtbl -> AppendAttribute(This,type,value)

#define INapSoHConstructor_GetSoH(This,soh)	\
    (This)->lpVtbl -> GetSoH(This,soh)

#define INapSoHConstructor_Validate(This,soh,isRequest)	\
    (This)->lpVtbl -> Validate(This,soh,isRequest)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INapSoHConstructor_Initialize_Proxy( 
    INapSoHConstructor * This,
    /* [in] */ SystemHealthEntityId id,
    /* [in] */ BOOL isRequest);


void __RPC_STUB INapSoHConstructor_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSoHConstructor_AppendAttribute_Proxy( 
    INapSoHConstructor * This,
    /* [in] */ SoHAttributeType type,
    /* [switch_is][in] */ const SoHAttributeValue *value);


void __RPC_STUB INapSoHConstructor_AppendAttribute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSoHConstructor_GetSoH_Proxy( 
    INapSoHConstructor * This,
    /* [out] */ SoH **soh);


void __RPC_STUB INapSoHConstructor_GetSoH_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSoHConstructor_Validate_Proxy( 
    INapSoHConstructor * This,
    /* [in] */ const SoH *soh,
    /* [in] */ BOOL isRequest);


void __RPC_STUB INapSoHConstructor_Validate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INapSoHConstructor_INTERFACE_DEFINED__ */


#ifndef __INapSoHProcessor_INTERFACE_DEFINED__
#define __INapSoHProcessor_INTERFACE_DEFINED__

/* interface INapSoHProcessor */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_INapSoHProcessor;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FB2FA8B0-2CD5-457d-ABA8-4376F63EA1C0")
    INapSoHProcessor : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ const SoH *soh,
            /* [in] */ BOOL isRequest,
            /* [out] */ SystemHealthEntityId *id) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FindNextAttribute( 
            /* [in] */ UINT16 fromLocation,
            /* [in] */ SoHAttributeType type,
            /* [out] */ UINT16 *attributeLocation) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAttribute( 
            /* [in] */ UINT16 attributeLocation,
            /* [out] */ SoHAttributeType *type,
            /* [switch_is][out] */ SoHAttributeValue **value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNumberOfAttributes( 
            /* [out] */ UINT16 *attributeCount) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INapSoHProcessorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INapSoHProcessor * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INapSoHProcessor * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INapSoHProcessor * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            INapSoHProcessor * This,
            /* [in] */ const SoH *soh,
            /* [in] */ BOOL isRequest,
            /* [out] */ SystemHealthEntityId *id);
        
        HRESULT ( STDMETHODCALLTYPE *FindNextAttribute )( 
            INapSoHProcessor * This,
            /* [in] */ UINT16 fromLocation,
            /* [in] */ SoHAttributeType type,
            /* [out] */ UINT16 *attributeLocation);
        
        HRESULT ( STDMETHODCALLTYPE *GetAttribute )( 
            INapSoHProcessor * This,
            /* [in] */ UINT16 attributeLocation,
            /* [out] */ SoHAttributeType *type,
            /* [switch_is][out] */ SoHAttributeValue **value);
        
        HRESULT ( STDMETHODCALLTYPE *GetNumberOfAttributes )( 
            INapSoHProcessor * This,
            /* [out] */ UINT16 *attributeCount);
        
        END_INTERFACE
    } INapSoHProcessorVtbl;

    interface INapSoHProcessor
    {
        CONST_VTBL struct INapSoHProcessorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INapSoHProcessor_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INapSoHProcessor_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INapSoHProcessor_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INapSoHProcessor_Initialize(This,soh,isRequest,id)	\
    (This)->lpVtbl -> Initialize(This,soh,isRequest,id)

#define INapSoHProcessor_FindNextAttribute(This,fromLocation,type,attributeLocation)	\
    (This)->lpVtbl -> FindNextAttribute(This,fromLocation,type,attributeLocation)

#define INapSoHProcessor_GetAttribute(This,attributeLocation,type,value)	\
    (This)->lpVtbl -> GetAttribute(This,attributeLocation,type,value)

#define INapSoHProcessor_GetNumberOfAttributes(This,attributeCount)	\
    (This)->lpVtbl -> GetNumberOfAttributes(This,attributeCount)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INapSoHProcessor_Initialize_Proxy( 
    INapSoHProcessor * This,
    /* [in] */ const SoH *soh,
    /* [in] */ BOOL isRequest,
    /* [out] */ SystemHealthEntityId *id);


void __RPC_STUB INapSoHProcessor_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSoHProcessor_FindNextAttribute_Proxy( 
    INapSoHProcessor * This,
    /* [in] */ UINT16 fromLocation,
    /* [in] */ SoHAttributeType type,
    /* [out] */ UINT16 *attributeLocation);


void __RPC_STUB INapSoHProcessor_FindNextAttribute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSoHProcessor_GetAttribute_Proxy( 
    INapSoHProcessor * This,
    /* [in] */ UINT16 attributeLocation,
    /* [out] */ SoHAttributeType *type,
    /* [switch_is][out] */ SoHAttributeValue **value);


void __RPC_STUB INapSoHProcessor_GetAttribute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INapSoHProcessor_GetNumberOfAttributes_Proxy( 
    INapSoHProcessor * This,
    /* [out] */ UINT16 *attributeCount);


void __RPC_STUB INapSoHProcessor_GetNumberOfAttributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INapSoHProcessor_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_napprotocol_0257 */
/* [local] */ 

// Declarations of CLSIDs of objects provided  
// by the system. Link to uuid.lib to get them 
// defined.                                    
EXTERN_C const CLSID CLSID_NapSoHConstructor;
EXTERN_C const CLSID CLSID_NapSoHProcessor;


extern RPC_IF_HANDLE __MIDL_itf_napprotocol_0257_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_napprotocol_0257_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


