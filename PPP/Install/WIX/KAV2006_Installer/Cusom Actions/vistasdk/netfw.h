

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0499 */
/* Compiler settings for netfw.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

/* verify that the <rpcsal.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__ 100
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

#ifndef __netfw_h__
#define __netfw_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __INetFwRemoteAdminSettings_FWD_DEFINED__
#define __INetFwRemoteAdminSettings_FWD_DEFINED__
typedef interface INetFwRemoteAdminSettings INetFwRemoteAdminSettings;
#endif 	/* __INetFwRemoteAdminSettings_FWD_DEFINED__ */


#ifndef __INetFwIcmpSettings_FWD_DEFINED__
#define __INetFwIcmpSettings_FWD_DEFINED__
typedef interface INetFwIcmpSettings INetFwIcmpSettings;
#endif 	/* __INetFwIcmpSettings_FWD_DEFINED__ */


#ifndef __INetFwOpenPort_FWD_DEFINED__
#define __INetFwOpenPort_FWD_DEFINED__
typedef interface INetFwOpenPort INetFwOpenPort;
#endif 	/* __INetFwOpenPort_FWD_DEFINED__ */


#ifndef __INetFwOpenPorts_FWD_DEFINED__
#define __INetFwOpenPorts_FWD_DEFINED__
typedef interface INetFwOpenPorts INetFwOpenPorts;
#endif 	/* __INetFwOpenPorts_FWD_DEFINED__ */


#ifndef __INetFwService_FWD_DEFINED__
#define __INetFwService_FWD_DEFINED__
typedef interface INetFwService INetFwService;
#endif 	/* __INetFwService_FWD_DEFINED__ */


#ifndef __INetFwServices_FWD_DEFINED__
#define __INetFwServices_FWD_DEFINED__
typedef interface INetFwServices INetFwServices;
#endif 	/* __INetFwServices_FWD_DEFINED__ */


#ifndef __INetFwAuthorizedApplication_FWD_DEFINED__
#define __INetFwAuthorizedApplication_FWD_DEFINED__
typedef interface INetFwAuthorizedApplication INetFwAuthorizedApplication;
#endif 	/* __INetFwAuthorizedApplication_FWD_DEFINED__ */


#ifndef __INetFwAuthorizedApplications_FWD_DEFINED__
#define __INetFwAuthorizedApplications_FWD_DEFINED__
typedef interface INetFwAuthorizedApplications INetFwAuthorizedApplications;
#endif 	/* __INetFwAuthorizedApplications_FWD_DEFINED__ */


#ifndef __INetFwRule_FWD_DEFINED__
#define __INetFwRule_FWD_DEFINED__
typedef interface INetFwRule INetFwRule;
#endif 	/* __INetFwRule_FWD_DEFINED__ */


#ifndef __INetFwRules_FWD_DEFINED__
#define __INetFwRules_FWD_DEFINED__
typedef interface INetFwRules INetFwRules;
#endif 	/* __INetFwRules_FWD_DEFINED__ */


#ifndef __INetFwServiceRestriction_FWD_DEFINED__
#define __INetFwServiceRestriction_FWD_DEFINED__
typedef interface INetFwServiceRestriction INetFwServiceRestriction;
#endif 	/* __INetFwServiceRestriction_FWD_DEFINED__ */


#ifndef __INetFwProfile_FWD_DEFINED__
#define __INetFwProfile_FWD_DEFINED__
typedef interface INetFwProfile INetFwProfile;
#endif 	/* __INetFwProfile_FWD_DEFINED__ */


#ifndef __INetFwPolicy_FWD_DEFINED__
#define __INetFwPolicy_FWD_DEFINED__
typedef interface INetFwPolicy INetFwPolicy;
#endif 	/* __INetFwPolicy_FWD_DEFINED__ */


#ifndef __INetFwPolicy2_FWD_DEFINED__
#define __INetFwPolicy2_FWD_DEFINED__
typedef interface INetFwPolicy2 INetFwPolicy2;
#endif 	/* __INetFwPolicy2_FWD_DEFINED__ */


#ifndef __INetFwMgr_FWD_DEFINED__
#define __INetFwMgr_FWD_DEFINED__
typedef interface INetFwMgr INetFwMgr;
#endif 	/* __INetFwMgr_FWD_DEFINED__ */


#ifndef __INetFwRemoteAdminSettings_FWD_DEFINED__
#define __INetFwRemoteAdminSettings_FWD_DEFINED__
typedef interface INetFwRemoteAdminSettings INetFwRemoteAdminSettings;
#endif 	/* __INetFwRemoteAdminSettings_FWD_DEFINED__ */


#ifndef __INetFwIcmpSettings_FWD_DEFINED__
#define __INetFwIcmpSettings_FWD_DEFINED__
typedef interface INetFwIcmpSettings INetFwIcmpSettings;
#endif 	/* __INetFwIcmpSettings_FWD_DEFINED__ */


#ifndef __INetFwOpenPort_FWD_DEFINED__
#define __INetFwOpenPort_FWD_DEFINED__
typedef interface INetFwOpenPort INetFwOpenPort;
#endif 	/* __INetFwOpenPort_FWD_DEFINED__ */


#ifndef __INetFwOpenPorts_FWD_DEFINED__
#define __INetFwOpenPorts_FWD_DEFINED__
typedef interface INetFwOpenPorts INetFwOpenPorts;
#endif 	/* __INetFwOpenPorts_FWD_DEFINED__ */


#ifndef __INetFwService_FWD_DEFINED__
#define __INetFwService_FWD_DEFINED__
typedef interface INetFwService INetFwService;
#endif 	/* __INetFwService_FWD_DEFINED__ */


#ifndef __INetFwServices_FWD_DEFINED__
#define __INetFwServices_FWD_DEFINED__
typedef interface INetFwServices INetFwServices;
#endif 	/* __INetFwServices_FWD_DEFINED__ */


#ifndef __INetFwAuthorizedApplication_FWD_DEFINED__
#define __INetFwAuthorizedApplication_FWD_DEFINED__
typedef interface INetFwAuthorizedApplication INetFwAuthorizedApplication;
#endif 	/* __INetFwAuthorizedApplication_FWD_DEFINED__ */


#ifndef __INetFwAuthorizedApplications_FWD_DEFINED__
#define __INetFwAuthorizedApplications_FWD_DEFINED__
typedef interface INetFwAuthorizedApplications INetFwAuthorizedApplications;
#endif 	/* __INetFwAuthorizedApplications_FWD_DEFINED__ */


#ifndef __INetFwServiceRestriction_FWD_DEFINED__
#define __INetFwServiceRestriction_FWD_DEFINED__
typedef interface INetFwServiceRestriction INetFwServiceRestriction;
#endif 	/* __INetFwServiceRestriction_FWD_DEFINED__ */


#ifndef __INetFwRule_FWD_DEFINED__
#define __INetFwRule_FWD_DEFINED__
typedef interface INetFwRule INetFwRule;
#endif 	/* __INetFwRule_FWD_DEFINED__ */


#ifndef __INetFwRules_FWD_DEFINED__
#define __INetFwRules_FWD_DEFINED__
typedef interface INetFwRules INetFwRules;
#endif 	/* __INetFwRules_FWD_DEFINED__ */


#ifndef __INetFwProfile_FWD_DEFINED__
#define __INetFwProfile_FWD_DEFINED__
typedef interface INetFwProfile INetFwProfile;
#endif 	/* __INetFwProfile_FWD_DEFINED__ */


#ifndef __INetFwPolicy_FWD_DEFINED__
#define __INetFwPolicy_FWD_DEFINED__
typedef interface INetFwPolicy INetFwPolicy;
#endif 	/* __INetFwPolicy_FWD_DEFINED__ */


#ifndef __INetFwPolicy2_FWD_DEFINED__
#define __INetFwPolicy2_FWD_DEFINED__
typedef interface INetFwPolicy2 INetFwPolicy2;
#endif 	/* __INetFwPolicy2_FWD_DEFINED__ */


#ifndef __INetFwMgr_FWD_DEFINED__
#define __INetFwMgr_FWD_DEFINED__
typedef interface INetFwMgr INetFwMgr;
#endif 	/* __INetFwMgr_FWD_DEFINED__ */


#ifndef __NetFwRule_FWD_DEFINED__
#define __NetFwRule_FWD_DEFINED__

#ifdef __cplusplus
typedef class NetFwRule NetFwRule;
#else
typedef struct NetFwRule NetFwRule;
#endif /* __cplusplus */

#endif 	/* __NetFwRule_FWD_DEFINED__ */


#ifndef __NetFwOpenPort_FWD_DEFINED__
#define __NetFwOpenPort_FWD_DEFINED__

#ifdef __cplusplus
typedef class NetFwOpenPort NetFwOpenPort;
#else
typedef struct NetFwOpenPort NetFwOpenPort;
#endif /* __cplusplus */

#endif 	/* __NetFwOpenPort_FWD_DEFINED__ */


#ifndef __NetFwAuthorizedApplication_FWD_DEFINED__
#define __NetFwAuthorizedApplication_FWD_DEFINED__

#ifdef __cplusplus
typedef class NetFwAuthorizedApplication NetFwAuthorizedApplication;
#else
typedef struct NetFwAuthorizedApplication NetFwAuthorizedApplication;
#endif /* __cplusplus */

#endif 	/* __NetFwAuthorizedApplication_FWD_DEFINED__ */


#ifndef __NetFwPolicy2_FWD_DEFINED__
#define __NetFwPolicy2_FWD_DEFINED__

#ifdef __cplusplus
typedef class NetFwPolicy2 NetFwPolicy2;
#else
typedef struct NetFwPolicy2 NetFwPolicy2;
#endif /* __cplusplus */

#endif 	/* __NetFwPolicy2_FWD_DEFINED__ */


#ifndef __NetFwMgr_FWD_DEFINED__
#define __NetFwMgr_FWD_DEFINED__

#ifdef __cplusplus
typedef class NetFwMgr NetFwMgr;
#else
typedef struct NetFwMgr NetFwMgr;
#endif /* __cplusplus */

#endif 	/* __NetFwMgr_FWD_DEFINED__ */


/* header files for imported files */
#include "icftypes.h"
#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __INetFwRemoteAdminSettings_INTERFACE_DEFINED__
#define __INetFwRemoteAdminSettings_INTERFACE_DEFINED__

/* interface INetFwRemoteAdminSettings */
/* [dual][uuid][object] */ 


EXTERN_C const IID IID_INetFwRemoteAdminSettings;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D4BECDDF-6F73-4A83-B832-9C66874CD20E")
    INetFwRemoteAdminSettings : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_IpVersion( 
            /* [retval][out] */ __RPC__out NET_FW_IP_VERSION *ipVersion) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_IpVersion( 
            /* [in] */ NET_FW_IP_VERSION ipVersion) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Scope( 
            /* [retval][out] */ __RPC__out NET_FW_SCOPE *scope) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Scope( 
            /* [in] */ NET_FW_SCOPE scope) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_RemoteAddresses( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *remoteAddrs) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_RemoteAddresses( 
            /* [in] */ __RPC__in BSTR remoteAddrs) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Enabled( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *enabled) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Enabled( 
            /* [in] */ VARIANT_BOOL enabled) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwRemoteAdminSettingsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetFwRemoteAdminSettings * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetFwRemoteAdminSettings * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetFwRemoteAdminSettings * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INetFwRemoteAdminSettings * This,
            /* [out] */ __RPC__out UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INetFwRemoteAdminSettings * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INetFwRemoteAdminSettings * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INetFwRemoteAdminSettings * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_IpVersion )( 
            INetFwRemoteAdminSettings * This,
            /* [retval][out] */ __RPC__out NET_FW_IP_VERSION *ipVersion);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_IpVersion )( 
            INetFwRemoteAdminSettings * This,
            /* [in] */ NET_FW_IP_VERSION ipVersion);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Scope )( 
            INetFwRemoteAdminSettings * This,
            /* [retval][out] */ __RPC__out NET_FW_SCOPE *scope);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Scope )( 
            INetFwRemoteAdminSettings * This,
            /* [in] */ NET_FW_SCOPE scope);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_RemoteAddresses )( 
            INetFwRemoteAdminSettings * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *remoteAddrs);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_RemoteAddresses )( 
            INetFwRemoteAdminSettings * This,
            /* [in] */ __RPC__in BSTR remoteAddrs);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Enabled )( 
            INetFwRemoteAdminSettings * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *enabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Enabled )( 
            INetFwRemoteAdminSettings * This,
            /* [in] */ VARIANT_BOOL enabled);
        
        END_INTERFACE
    } INetFwRemoteAdminSettingsVtbl;

    interface INetFwRemoteAdminSettings
    {
        CONST_VTBL struct INetFwRemoteAdminSettingsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwRemoteAdminSettings_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INetFwRemoteAdminSettings_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INetFwRemoteAdminSettings_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INetFwRemoteAdminSettings_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INetFwRemoteAdminSettings_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INetFwRemoteAdminSettings_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INetFwRemoteAdminSettings_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INetFwRemoteAdminSettings_get_IpVersion(This,ipVersion)	\
    ( (This)->lpVtbl -> get_IpVersion(This,ipVersion) ) 

#define INetFwRemoteAdminSettings_put_IpVersion(This,ipVersion)	\
    ( (This)->lpVtbl -> put_IpVersion(This,ipVersion) ) 

#define INetFwRemoteAdminSettings_get_Scope(This,scope)	\
    ( (This)->lpVtbl -> get_Scope(This,scope) ) 

#define INetFwRemoteAdminSettings_put_Scope(This,scope)	\
    ( (This)->lpVtbl -> put_Scope(This,scope) ) 

#define INetFwRemoteAdminSettings_get_RemoteAddresses(This,remoteAddrs)	\
    ( (This)->lpVtbl -> get_RemoteAddresses(This,remoteAddrs) ) 

#define INetFwRemoteAdminSettings_put_RemoteAddresses(This,remoteAddrs)	\
    ( (This)->lpVtbl -> put_RemoteAddresses(This,remoteAddrs) ) 

#define INetFwRemoteAdminSettings_get_Enabled(This,enabled)	\
    ( (This)->lpVtbl -> get_Enabled(This,enabled) ) 

#define INetFwRemoteAdminSettings_put_Enabled(This,enabled)	\
    ( (This)->lpVtbl -> put_Enabled(This,enabled) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INetFwRemoteAdminSettings_INTERFACE_DEFINED__ */


#ifndef __INetFwIcmpSettings_INTERFACE_DEFINED__
#define __INetFwIcmpSettings_INTERFACE_DEFINED__

/* interface INetFwIcmpSettings */
/* [dual][uuid][object] */ 


EXTERN_C const IID IID_INetFwIcmpSettings;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A6207B2E-7CDD-426A-951E-5E1CBC5AFEAD")
    INetFwIcmpSettings : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowOutboundDestinationUnreachable( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *allow) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowOutboundDestinationUnreachable( 
            /* [in] */ VARIANT_BOOL allow) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowRedirect( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *allow) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowRedirect( 
            /* [in] */ VARIANT_BOOL allow) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowInboundEchoRequest( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *allow) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowInboundEchoRequest( 
            /* [in] */ VARIANT_BOOL allow) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowOutboundTimeExceeded( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *allow) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowOutboundTimeExceeded( 
            /* [in] */ VARIANT_BOOL allow) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowOutboundParameterProblem( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *allow) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowOutboundParameterProblem( 
            /* [in] */ VARIANT_BOOL allow) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowOutboundSourceQuench( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *allow) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowOutboundSourceQuench( 
            /* [in] */ VARIANT_BOOL allow) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowInboundRouterRequest( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *allow) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowInboundRouterRequest( 
            /* [in] */ VARIANT_BOOL allow) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowInboundTimestampRequest( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *allow) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowInboundTimestampRequest( 
            /* [in] */ VARIANT_BOOL allow) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowInboundMaskRequest( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *allow) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowInboundMaskRequest( 
            /* [in] */ VARIANT_BOOL allow) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AllowOutboundPacketTooBig( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *allow) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_AllowOutboundPacketTooBig( 
            /* [in] */ VARIANT_BOOL allow) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwIcmpSettingsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetFwIcmpSettings * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetFwIcmpSettings * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetFwIcmpSettings * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INetFwIcmpSettings * This,
            /* [out] */ __RPC__out UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INetFwIcmpSettings * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INetFwIcmpSettings * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INetFwIcmpSettings * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AllowOutboundDestinationUnreachable )( 
            INetFwIcmpSettings * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *allow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AllowOutboundDestinationUnreachable )( 
            INetFwIcmpSettings * This,
            /* [in] */ VARIANT_BOOL allow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AllowRedirect )( 
            INetFwIcmpSettings * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *allow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AllowRedirect )( 
            INetFwIcmpSettings * This,
            /* [in] */ VARIANT_BOOL allow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AllowInboundEchoRequest )( 
            INetFwIcmpSettings * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *allow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AllowInboundEchoRequest )( 
            INetFwIcmpSettings * This,
            /* [in] */ VARIANT_BOOL allow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AllowOutboundTimeExceeded )( 
            INetFwIcmpSettings * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *allow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AllowOutboundTimeExceeded )( 
            INetFwIcmpSettings * This,
            /* [in] */ VARIANT_BOOL allow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AllowOutboundParameterProblem )( 
            INetFwIcmpSettings * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *allow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AllowOutboundParameterProblem )( 
            INetFwIcmpSettings * This,
            /* [in] */ VARIANT_BOOL allow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AllowOutboundSourceQuench )( 
            INetFwIcmpSettings * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *allow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AllowOutboundSourceQuench )( 
            INetFwIcmpSettings * This,
            /* [in] */ VARIANT_BOOL allow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AllowInboundRouterRequest )( 
            INetFwIcmpSettings * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *allow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AllowInboundRouterRequest )( 
            INetFwIcmpSettings * This,
            /* [in] */ VARIANT_BOOL allow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AllowInboundTimestampRequest )( 
            INetFwIcmpSettings * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *allow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AllowInboundTimestampRequest )( 
            INetFwIcmpSettings * This,
            /* [in] */ VARIANT_BOOL allow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AllowInboundMaskRequest )( 
            INetFwIcmpSettings * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *allow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AllowInboundMaskRequest )( 
            INetFwIcmpSettings * This,
            /* [in] */ VARIANT_BOOL allow);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AllowOutboundPacketTooBig )( 
            INetFwIcmpSettings * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *allow);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AllowOutboundPacketTooBig )( 
            INetFwIcmpSettings * This,
            /* [in] */ VARIANT_BOOL allow);
        
        END_INTERFACE
    } INetFwIcmpSettingsVtbl;

    interface INetFwIcmpSettings
    {
        CONST_VTBL struct INetFwIcmpSettingsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwIcmpSettings_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INetFwIcmpSettings_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INetFwIcmpSettings_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INetFwIcmpSettings_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INetFwIcmpSettings_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INetFwIcmpSettings_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INetFwIcmpSettings_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INetFwIcmpSettings_get_AllowOutboundDestinationUnreachable(This,allow)	\
    ( (This)->lpVtbl -> get_AllowOutboundDestinationUnreachable(This,allow) ) 

#define INetFwIcmpSettings_put_AllowOutboundDestinationUnreachable(This,allow)	\
    ( (This)->lpVtbl -> put_AllowOutboundDestinationUnreachable(This,allow) ) 

#define INetFwIcmpSettings_get_AllowRedirect(This,allow)	\
    ( (This)->lpVtbl -> get_AllowRedirect(This,allow) ) 

#define INetFwIcmpSettings_put_AllowRedirect(This,allow)	\
    ( (This)->lpVtbl -> put_AllowRedirect(This,allow) ) 

#define INetFwIcmpSettings_get_AllowInboundEchoRequest(This,allow)	\
    ( (This)->lpVtbl -> get_AllowInboundEchoRequest(This,allow) ) 

#define INetFwIcmpSettings_put_AllowInboundEchoRequest(This,allow)	\
    ( (This)->lpVtbl -> put_AllowInboundEchoRequest(This,allow) ) 

#define INetFwIcmpSettings_get_AllowOutboundTimeExceeded(This,allow)	\
    ( (This)->lpVtbl -> get_AllowOutboundTimeExceeded(This,allow) ) 

#define INetFwIcmpSettings_put_AllowOutboundTimeExceeded(This,allow)	\
    ( (This)->lpVtbl -> put_AllowOutboundTimeExceeded(This,allow) ) 

#define INetFwIcmpSettings_get_AllowOutboundParameterProblem(This,allow)	\
    ( (This)->lpVtbl -> get_AllowOutboundParameterProblem(This,allow) ) 

#define INetFwIcmpSettings_put_AllowOutboundParameterProblem(This,allow)	\
    ( (This)->lpVtbl -> put_AllowOutboundParameterProblem(This,allow) ) 

#define INetFwIcmpSettings_get_AllowOutboundSourceQuench(This,allow)	\
    ( (This)->lpVtbl -> get_AllowOutboundSourceQuench(This,allow) ) 

#define INetFwIcmpSettings_put_AllowOutboundSourceQuench(This,allow)	\
    ( (This)->lpVtbl -> put_AllowOutboundSourceQuench(This,allow) ) 

#define INetFwIcmpSettings_get_AllowInboundRouterRequest(This,allow)	\
    ( (This)->lpVtbl -> get_AllowInboundRouterRequest(This,allow) ) 

#define INetFwIcmpSettings_put_AllowInboundRouterRequest(This,allow)	\
    ( (This)->lpVtbl -> put_AllowInboundRouterRequest(This,allow) ) 

#define INetFwIcmpSettings_get_AllowInboundTimestampRequest(This,allow)	\
    ( (This)->lpVtbl -> get_AllowInboundTimestampRequest(This,allow) ) 

#define INetFwIcmpSettings_put_AllowInboundTimestampRequest(This,allow)	\
    ( (This)->lpVtbl -> put_AllowInboundTimestampRequest(This,allow) ) 

#define INetFwIcmpSettings_get_AllowInboundMaskRequest(This,allow)	\
    ( (This)->lpVtbl -> get_AllowInboundMaskRequest(This,allow) ) 

#define INetFwIcmpSettings_put_AllowInboundMaskRequest(This,allow)	\
    ( (This)->lpVtbl -> put_AllowInboundMaskRequest(This,allow) ) 

#define INetFwIcmpSettings_get_AllowOutboundPacketTooBig(This,allow)	\
    ( (This)->lpVtbl -> get_AllowOutboundPacketTooBig(This,allow) ) 

#define INetFwIcmpSettings_put_AllowOutboundPacketTooBig(This,allow)	\
    ( (This)->lpVtbl -> put_AllowOutboundPacketTooBig(This,allow) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INetFwIcmpSettings_INTERFACE_DEFINED__ */


#ifndef __INetFwOpenPort_INTERFACE_DEFINED__
#define __INetFwOpenPort_INTERFACE_DEFINED__

/* interface INetFwOpenPort */
/* [dual][uuid][object] */ 


EXTERN_C const IID IID_INetFwOpenPort;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E0483BA0-47FF-4D9C-A6D6-7741D0B195F7")
    INetFwOpenPort : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *name) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ __RPC__in BSTR name) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_IpVersion( 
            /* [retval][out] */ __RPC__out NET_FW_IP_VERSION *ipVersion) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_IpVersion( 
            /* [in] */ NET_FW_IP_VERSION ipVersion) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Protocol( 
            /* [retval][out] */ __RPC__out NET_FW_IP_PROTOCOL *ipProtocol) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Protocol( 
            /* [in] */ NET_FW_IP_PROTOCOL ipProtocol) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Port( 
            /* [retval][out] */ __RPC__out LONG *portNumber) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Port( 
            /* [in] */ LONG portNumber) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Scope( 
            /* [retval][out] */ __RPC__out NET_FW_SCOPE *scope) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Scope( 
            /* [in] */ NET_FW_SCOPE scope) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_RemoteAddresses( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *remoteAddrs) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_RemoteAddresses( 
            /* [in] */ __RPC__in BSTR remoteAddrs) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Enabled( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *enabled) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Enabled( 
            /* [in] */ VARIANT_BOOL enabled) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_BuiltIn( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *builtIn) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwOpenPortVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetFwOpenPort * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetFwOpenPort * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetFwOpenPort * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INetFwOpenPort * This,
            /* [out] */ __RPC__out UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INetFwOpenPort * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INetFwOpenPort * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INetFwOpenPort * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            INetFwOpenPort * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *name);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            INetFwOpenPort * This,
            /* [in] */ __RPC__in BSTR name);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_IpVersion )( 
            INetFwOpenPort * This,
            /* [retval][out] */ __RPC__out NET_FW_IP_VERSION *ipVersion);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_IpVersion )( 
            INetFwOpenPort * This,
            /* [in] */ NET_FW_IP_VERSION ipVersion);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Protocol )( 
            INetFwOpenPort * This,
            /* [retval][out] */ __RPC__out NET_FW_IP_PROTOCOL *ipProtocol);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Protocol )( 
            INetFwOpenPort * This,
            /* [in] */ NET_FW_IP_PROTOCOL ipProtocol);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Port )( 
            INetFwOpenPort * This,
            /* [retval][out] */ __RPC__out LONG *portNumber);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Port )( 
            INetFwOpenPort * This,
            /* [in] */ LONG portNumber);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Scope )( 
            INetFwOpenPort * This,
            /* [retval][out] */ __RPC__out NET_FW_SCOPE *scope);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Scope )( 
            INetFwOpenPort * This,
            /* [in] */ NET_FW_SCOPE scope);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_RemoteAddresses )( 
            INetFwOpenPort * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *remoteAddrs);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_RemoteAddresses )( 
            INetFwOpenPort * This,
            /* [in] */ __RPC__in BSTR remoteAddrs);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Enabled )( 
            INetFwOpenPort * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *enabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Enabled )( 
            INetFwOpenPort * This,
            /* [in] */ VARIANT_BOOL enabled);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_BuiltIn )( 
            INetFwOpenPort * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *builtIn);
        
        END_INTERFACE
    } INetFwOpenPortVtbl;

    interface INetFwOpenPort
    {
        CONST_VTBL struct INetFwOpenPortVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwOpenPort_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INetFwOpenPort_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INetFwOpenPort_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INetFwOpenPort_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INetFwOpenPort_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INetFwOpenPort_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INetFwOpenPort_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INetFwOpenPort_get_Name(This,name)	\
    ( (This)->lpVtbl -> get_Name(This,name) ) 

#define INetFwOpenPort_put_Name(This,name)	\
    ( (This)->lpVtbl -> put_Name(This,name) ) 

#define INetFwOpenPort_get_IpVersion(This,ipVersion)	\
    ( (This)->lpVtbl -> get_IpVersion(This,ipVersion) ) 

#define INetFwOpenPort_put_IpVersion(This,ipVersion)	\
    ( (This)->lpVtbl -> put_IpVersion(This,ipVersion) ) 

#define INetFwOpenPort_get_Protocol(This,ipProtocol)	\
    ( (This)->lpVtbl -> get_Protocol(This,ipProtocol) ) 

#define INetFwOpenPort_put_Protocol(This,ipProtocol)	\
    ( (This)->lpVtbl -> put_Protocol(This,ipProtocol) ) 

#define INetFwOpenPort_get_Port(This,portNumber)	\
    ( (This)->lpVtbl -> get_Port(This,portNumber) ) 

#define INetFwOpenPort_put_Port(This,portNumber)	\
    ( (This)->lpVtbl -> put_Port(This,portNumber) ) 

#define INetFwOpenPort_get_Scope(This,scope)	\
    ( (This)->lpVtbl -> get_Scope(This,scope) ) 

#define INetFwOpenPort_put_Scope(This,scope)	\
    ( (This)->lpVtbl -> put_Scope(This,scope) ) 

#define INetFwOpenPort_get_RemoteAddresses(This,remoteAddrs)	\
    ( (This)->lpVtbl -> get_RemoteAddresses(This,remoteAddrs) ) 

#define INetFwOpenPort_put_RemoteAddresses(This,remoteAddrs)	\
    ( (This)->lpVtbl -> put_RemoteAddresses(This,remoteAddrs) ) 

#define INetFwOpenPort_get_Enabled(This,enabled)	\
    ( (This)->lpVtbl -> get_Enabled(This,enabled) ) 

#define INetFwOpenPort_put_Enabled(This,enabled)	\
    ( (This)->lpVtbl -> put_Enabled(This,enabled) ) 

#define INetFwOpenPort_get_BuiltIn(This,builtIn)	\
    ( (This)->lpVtbl -> get_BuiltIn(This,builtIn) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INetFwOpenPort_INTERFACE_DEFINED__ */


#ifndef __INetFwOpenPorts_INTERFACE_DEFINED__
#define __INetFwOpenPorts_INTERFACE_DEFINED__

/* interface INetFwOpenPorts */
/* [dual][uuid][object] */ 


EXTERN_C const IID IID_INetFwOpenPorts;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C0E9D7FA-E07E-430A-B19A-090CE82D92E2")
    INetFwOpenPorts : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ __RPC__out long *count) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ __RPC__in_opt INetFwOpenPort *port) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ LONG portNumber,
            /* [in] */ NET_FW_IP_PROTOCOL ipProtocol) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ LONG portNumber,
            /* [in] */ NET_FW_IP_PROTOCOL ipProtocol,
            /* [retval][out] */ __RPC__deref_out_opt INetFwOpenPort **openPort) = 0;
        
        virtual /* [restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ __RPC__deref_out_opt IUnknown **newEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwOpenPortsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetFwOpenPorts * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetFwOpenPorts * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetFwOpenPorts * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INetFwOpenPorts * This,
            /* [out] */ __RPC__out UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INetFwOpenPorts * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INetFwOpenPorts * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INetFwOpenPorts * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            INetFwOpenPorts * This,
            /* [retval][out] */ __RPC__out long *count);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            INetFwOpenPorts * This,
            /* [in] */ __RPC__in_opt INetFwOpenPort *port);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            INetFwOpenPorts * This,
            /* [in] */ LONG portNumber,
            /* [in] */ NET_FW_IP_PROTOCOL ipProtocol);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            INetFwOpenPorts * This,
            /* [in] */ LONG portNumber,
            /* [in] */ NET_FW_IP_PROTOCOL ipProtocol,
            /* [retval][out] */ __RPC__deref_out_opt INetFwOpenPort **openPort);
        
        /* [restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            INetFwOpenPorts * This,
            /* [retval][out] */ __RPC__deref_out_opt IUnknown **newEnum);
        
        END_INTERFACE
    } INetFwOpenPortsVtbl;

    interface INetFwOpenPorts
    {
        CONST_VTBL struct INetFwOpenPortsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwOpenPorts_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INetFwOpenPorts_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INetFwOpenPorts_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INetFwOpenPorts_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INetFwOpenPorts_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INetFwOpenPorts_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INetFwOpenPorts_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INetFwOpenPorts_get_Count(This,count)	\
    ( (This)->lpVtbl -> get_Count(This,count) ) 

#define INetFwOpenPorts_Add(This,port)	\
    ( (This)->lpVtbl -> Add(This,port) ) 

#define INetFwOpenPorts_Remove(This,portNumber,ipProtocol)	\
    ( (This)->lpVtbl -> Remove(This,portNumber,ipProtocol) ) 

#define INetFwOpenPorts_Item(This,portNumber,ipProtocol,openPort)	\
    ( (This)->lpVtbl -> Item(This,portNumber,ipProtocol,openPort) ) 

#define INetFwOpenPorts_get__NewEnum(This,newEnum)	\
    ( (This)->lpVtbl -> get__NewEnum(This,newEnum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INetFwOpenPorts_INTERFACE_DEFINED__ */


#ifndef __INetFwService_INTERFACE_DEFINED__
#define __INetFwService_INTERFACE_DEFINED__

/* interface INetFwService */
/* [dual][uuid][object] */ 


EXTERN_C const IID IID_INetFwService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79FD57C8-908E-4A36-9888-D5B3F0A444CF")
    INetFwService : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *name) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ __RPC__out NET_FW_SERVICE_TYPE *type) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Customized( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *customized) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_IpVersion( 
            /* [retval][out] */ __RPC__out NET_FW_IP_VERSION *ipVersion) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_IpVersion( 
            /* [in] */ NET_FW_IP_VERSION ipVersion) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Scope( 
            /* [retval][out] */ __RPC__out NET_FW_SCOPE *scope) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Scope( 
            /* [in] */ NET_FW_SCOPE scope) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_RemoteAddresses( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *remoteAddrs) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_RemoteAddresses( 
            /* [in] */ __RPC__in BSTR remoteAddrs) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Enabled( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *enabled) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Enabled( 
            /* [in] */ VARIANT_BOOL enabled) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_GloballyOpenPorts( 
            /* [retval][out] */ __RPC__deref_out_opt INetFwOpenPorts **openPorts) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetFwService * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetFwService * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetFwService * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INetFwService * This,
            /* [out] */ __RPC__out UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INetFwService * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INetFwService * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INetFwService * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            INetFwService * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *name);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Type )( 
            INetFwService * This,
            /* [retval][out] */ __RPC__out NET_FW_SERVICE_TYPE *type);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Customized )( 
            INetFwService * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *customized);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_IpVersion )( 
            INetFwService * This,
            /* [retval][out] */ __RPC__out NET_FW_IP_VERSION *ipVersion);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_IpVersion )( 
            INetFwService * This,
            /* [in] */ NET_FW_IP_VERSION ipVersion);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Scope )( 
            INetFwService * This,
            /* [retval][out] */ __RPC__out NET_FW_SCOPE *scope);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Scope )( 
            INetFwService * This,
            /* [in] */ NET_FW_SCOPE scope);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_RemoteAddresses )( 
            INetFwService * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *remoteAddrs);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_RemoteAddresses )( 
            INetFwService * This,
            /* [in] */ __RPC__in BSTR remoteAddrs);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Enabled )( 
            INetFwService * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *enabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Enabled )( 
            INetFwService * This,
            /* [in] */ VARIANT_BOOL enabled);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_GloballyOpenPorts )( 
            INetFwService * This,
            /* [retval][out] */ __RPC__deref_out_opt INetFwOpenPorts **openPorts);
        
        END_INTERFACE
    } INetFwServiceVtbl;

    interface INetFwService
    {
        CONST_VTBL struct INetFwServiceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwService_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INetFwService_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INetFwService_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INetFwService_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INetFwService_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INetFwService_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INetFwService_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INetFwService_get_Name(This,name)	\
    ( (This)->lpVtbl -> get_Name(This,name) ) 

#define INetFwService_get_Type(This,type)	\
    ( (This)->lpVtbl -> get_Type(This,type) ) 

#define INetFwService_get_Customized(This,customized)	\
    ( (This)->lpVtbl -> get_Customized(This,customized) ) 

#define INetFwService_get_IpVersion(This,ipVersion)	\
    ( (This)->lpVtbl -> get_IpVersion(This,ipVersion) ) 

#define INetFwService_put_IpVersion(This,ipVersion)	\
    ( (This)->lpVtbl -> put_IpVersion(This,ipVersion) ) 

#define INetFwService_get_Scope(This,scope)	\
    ( (This)->lpVtbl -> get_Scope(This,scope) ) 

#define INetFwService_put_Scope(This,scope)	\
    ( (This)->lpVtbl -> put_Scope(This,scope) ) 

#define INetFwService_get_RemoteAddresses(This,remoteAddrs)	\
    ( (This)->lpVtbl -> get_RemoteAddresses(This,remoteAddrs) ) 

#define INetFwService_put_RemoteAddresses(This,remoteAddrs)	\
    ( (This)->lpVtbl -> put_RemoteAddresses(This,remoteAddrs) ) 

#define INetFwService_get_Enabled(This,enabled)	\
    ( (This)->lpVtbl -> get_Enabled(This,enabled) ) 

#define INetFwService_put_Enabled(This,enabled)	\
    ( (This)->lpVtbl -> put_Enabled(This,enabled) ) 

#define INetFwService_get_GloballyOpenPorts(This,openPorts)	\
    ( (This)->lpVtbl -> get_GloballyOpenPorts(This,openPorts) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INetFwService_INTERFACE_DEFINED__ */


#ifndef __INetFwServices_INTERFACE_DEFINED__
#define __INetFwServices_INTERFACE_DEFINED__

/* interface INetFwServices */
/* [dual][uuid][object] */ 


EXTERN_C const IID IID_INetFwServices;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79649BB4-903E-421B-94C9-79848E79F6EE")
    INetFwServices : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ __RPC__out long *count) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ NET_FW_SERVICE_TYPE svcType,
            /* [retval][out] */ __RPC__deref_out_opt INetFwService **service) = 0;
        
        virtual /* [restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ __RPC__deref_out_opt IUnknown **newEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwServicesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetFwServices * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetFwServices * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetFwServices * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INetFwServices * This,
            /* [out] */ __RPC__out UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INetFwServices * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INetFwServices * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INetFwServices * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            INetFwServices * This,
            /* [retval][out] */ __RPC__out long *count);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            INetFwServices * This,
            /* [in] */ NET_FW_SERVICE_TYPE svcType,
            /* [retval][out] */ __RPC__deref_out_opt INetFwService **service);
        
        /* [restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            INetFwServices * This,
            /* [retval][out] */ __RPC__deref_out_opt IUnknown **newEnum);
        
        END_INTERFACE
    } INetFwServicesVtbl;

    interface INetFwServices
    {
        CONST_VTBL struct INetFwServicesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwServices_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INetFwServices_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INetFwServices_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INetFwServices_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INetFwServices_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INetFwServices_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INetFwServices_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INetFwServices_get_Count(This,count)	\
    ( (This)->lpVtbl -> get_Count(This,count) ) 

#define INetFwServices_Item(This,svcType,service)	\
    ( (This)->lpVtbl -> Item(This,svcType,service) ) 

#define INetFwServices_get__NewEnum(This,newEnum)	\
    ( (This)->lpVtbl -> get__NewEnum(This,newEnum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INetFwServices_INTERFACE_DEFINED__ */


#ifndef __INetFwAuthorizedApplication_INTERFACE_DEFINED__
#define __INetFwAuthorizedApplication_INTERFACE_DEFINED__

/* interface INetFwAuthorizedApplication */
/* [dual][uuid][object] */ 


EXTERN_C const IID IID_INetFwAuthorizedApplication;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B5E64FFA-C2C5-444E-A301-FB5E00018050")
    INetFwAuthorizedApplication : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *name) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ __RPC__in BSTR name) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ProcessImageFileName( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *imageFileName) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_ProcessImageFileName( 
            /* [in] */ __RPC__in BSTR imageFileName) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_IpVersion( 
            /* [retval][out] */ __RPC__out NET_FW_IP_VERSION *ipVersion) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_IpVersion( 
            /* [in] */ NET_FW_IP_VERSION ipVersion) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Scope( 
            /* [retval][out] */ __RPC__out NET_FW_SCOPE *scope) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Scope( 
            /* [in] */ NET_FW_SCOPE scope) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_RemoteAddresses( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *remoteAddrs) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_RemoteAddresses( 
            /* [in] */ __RPC__in BSTR remoteAddrs) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Enabled( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *enabled) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Enabled( 
            /* [in] */ VARIANT_BOOL enabled) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwAuthorizedApplicationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetFwAuthorizedApplication * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetFwAuthorizedApplication * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetFwAuthorizedApplication * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INetFwAuthorizedApplication * This,
            /* [out] */ __RPC__out UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INetFwAuthorizedApplication * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INetFwAuthorizedApplication * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INetFwAuthorizedApplication * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            INetFwAuthorizedApplication * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *name);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            INetFwAuthorizedApplication * This,
            /* [in] */ __RPC__in BSTR name);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ProcessImageFileName )( 
            INetFwAuthorizedApplication * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *imageFileName);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ProcessImageFileName )( 
            INetFwAuthorizedApplication * This,
            /* [in] */ __RPC__in BSTR imageFileName);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_IpVersion )( 
            INetFwAuthorizedApplication * This,
            /* [retval][out] */ __RPC__out NET_FW_IP_VERSION *ipVersion);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_IpVersion )( 
            INetFwAuthorizedApplication * This,
            /* [in] */ NET_FW_IP_VERSION ipVersion);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Scope )( 
            INetFwAuthorizedApplication * This,
            /* [retval][out] */ __RPC__out NET_FW_SCOPE *scope);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Scope )( 
            INetFwAuthorizedApplication * This,
            /* [in] */ NET_FW_SCOPE scope);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_RemoteAddresses )( 
            INetFwAuthorizedApplication * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *remoteAddrs);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_RemoteAddresses )( 
            INetFwAuthorizedApplication * This,
            /* [in] */ __RPC__in BSTR remoteAddrs);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Enabled )( 
            INetFwAuthorizedApplication * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *enabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Enabled )( 
            INetFwAuthorizedApplication * This,
            /* [in] */ VARIANT_BOOL enabled);
        
        END_INTERFACE
    } INetFwAuthorizedApplicationVtbl;

    interface INetFwAuthorizedApplication
    {
        CONST_VTBL struct INetFwAuthorizedApplicationVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwAuthorizedApplication_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INetFwAuthorizedApplication_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INetFwAuthorizedApplication_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INetFwAuthorizedApplication_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INetFwAuthorizedApplication_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INetFwAuthorizedApplication_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INetFwAuthorizedApplication_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INetFwAuthorizedApplication_get_Name(This,name)	\
    ( (This)->lpVtbl -> get_Name(This,name) ) 

#define INetFwAuthorizedApplication_put_Name(This,name)	\
    ( (This)->lpVtbl -> put_Name(This,name) ) 

#define INetFwAuthorizedApplication_get_ProcessImageFileName(This,imageFileName)	\
    ( (This)->lpVtbl -> get_ProcessImageFileName(This,imageFileName) ) 

#define INetFwAuthorizedApplication_put_ProcessImageFileName(This,imageFileName)	\
    ( (This)->lpVtbl -> put_ProcessImageFileName(This,imageFileName) ) 

#define INetFwAuthorizedApplication_get_IpVersion(This,ipVersion)	\
    ( (This)->lpVtbl -> get_IpVersion(This,ipVersion) ) 

#define INetFwAuthorizedApplication_put_IpVersion(This,ipVersion)	\
    ( (This)->lpVtbl -> put_IpVersion(This,ipVersion) ) 

#define INetFwAuthorizedApplication_get_Scope(This,scope)	\
    ( (This)->lpVtbl -> get_Scope(This,scope) ) 

#define INetFwAuthorizedApplication_put_Scope(This,scope)	\
    ( (This)->lpVtbl -> put_Scope(This,scope) ) 

#define INetFwAuthorizedApplication_get_RemoteAddresses(This,remoteAddrs)	\
    ( (This)->lpVtbl -> get_RemoteAddresses(This,remoteAddrs) ) 

#define INetFwAuthorizedApplication_put_RemoteAddresses(This,remoteAddrs)	\
    ( (This)->lpVtbl -> put_RemoteAddresses(This,remoteAddrs) ) 

#define INetFwAuthorizedApplication_get_Enabled(This,enabled)	\
    ( (This)->lpVtbl -> get_Enabled(This,enabled) ) 

#define INetFwAuthorizedApplication_put_Enabled(This,enabled)	\
    ( (This)->lpVtbl -> put_Enabled(This,enabled) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INetFwAuthorizedApplication_INTERFACE_DEFINED__ */


#ifndef __INetFwAuthorizedApplications_INTERFACE_DEFINED__
#define __INetFwAuthorizedApplications_INTERFACE_DEFINED__

/* interface INetFwAuthorizedApplications */
/* [dual][uuid][object] */ 


EXTERN_C const IID IID_INetFwAuthorizedApplications;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("644EFD52-CCF9-486C-97A2-39F352570B30")
    INetFwAuthorizedApplications : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ __RPC__out long *count) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ __RPC__in_opt INetFwAuthorizedApplication *app) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ __RPC__in BSTR imageFileName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ __RPC__in BSTR imageFileName,
            /* [retval][out] */ __RPC__deref_out_opt INetFwAuthorizedApplication **app) = 0;
        
        virtual /* [restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ __RPC__deref_out_opt IUnknown **newEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwAuthorizedApplicationsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetFwAuthorizedApplications * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetFwAuthorizedApplications * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetFwAuthorizedApplications * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INetFwAuthorizedApplications * This,
            /* [out] */ __RPC__out UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INetFwAuthorizedApplications * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INetFwAuthorizedApplications * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INetFwAuthorizedApplications * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            INetFwAuthorizedApplications * This,
            /* [retval][out] */ __RPC__out long *count);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            INetFwAuthorizedApplications * This,
            /* [in] */ __RPC__in_opt INetFwAuthorizedApplication *app);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            INetFwAuthorizedApplications * This,
            /* [in] */ __RPC__in BSTR imageFileName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            INetFwAuthorizedApplications * This,
            /* [in] */ __RPC__in BSTR imageFileName,
            /* [retval][out] */ __RPC__deref_out_opt INetFwAuthorizedApplication **app);
        
        /* [restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            INetFwAuthorizedApplications * This,
            /* [retval][out] */ __RPC__deref_out_opt IUnknown **newEnum);
        
        END_INTERFACE
    } INetFwAuthorizedApplicationsVtbl;

    interface INetFwAuthorizedApplications
    {
        CONST_VTBL struct INetFwAuthorizedApplicationsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwAuthorizedApplications_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INetFwAuthorizedApplications_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INetFwAuthorizedApplications_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INetFwAuthorizedApplications_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INetFwAuthorizedApplications_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INetFwAuthorizedApplications_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INetFwAuthorizedApplications_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INetFwAuthorizedApplications_get_Count(This,count)	\
    ( (This)->lpVtbl -> get_Count(This,count) ) 

#define INetFwAuthorizedApplications_Add(This,app)	\
    ( (This)->lpVtbl -> Add(This,app) ) 

#define INetFwAuthorizedApplications_Remove(This,imageFileName)	\
    ( (This)->lpVtbl -> Remove(This,imageFileName) ) 

#define INetFwAuthorizedApplications_Item(This,imageFileName,app)	\
    ( (This)->lpVtbl -> Item(This,imageFileName,app) ) 

#define INetFwAuthorizedApplications_get__NewEnum(This,newEnum)	\
    ( (This)->lpVtbl -> get__NewEnum(This,newEnum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INetFwAuthorizedApplications_INTERFACE_DEFINED__ */


#ifndef __INetFwRule_INTERFACE_DEFINED__
#define __INetFwRule_INTERFACE_DEFINED__

/* interface INetFwRule */
/* [dual][uuid][object] */ 


EXTERN_C const IID IID_INetFwRule;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AF230D27-BABA-4E42-ACED-F524F22CFCE2")
    INetFwRule : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *name) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ __RPC__in BSTR name) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *desc) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Description( 
            /* [in] */ __RPC__in BSTR desc) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ApplicationName( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *imageFileName) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_ApplicationName( 
            /* [in] */ __RPC__in BSTR imageFileName) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ServiceName( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *serviceName) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_ServiceName( 
            /* [in] */ __RPC__in BSTR serviceName) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Protocol( 
            /* [retval][out] */ __RPC__out LONG *protocol) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Protocol( 
            /* [in] */ LONG protocol) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_LocalPorts( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *portNumbers) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_LocalPorts( 
            /* [in] */ __RPC__in BSTR portNumbers) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_RemotePorts( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *portNumbers) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_RemotePorts( 
            /* [in] */ __RPC__in BSTR portNumbers) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_LocalAddresses( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *localAddrs) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_LocalAddresses( 
            /* [in] */ __RPC__in BSTR localAddrs) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_RemoteAddresses( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *remoteAddrs) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_RemoteAddresses( 
            /* [in] */ __RPC__in BSTR remoteAddrs) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_IcmpTypesAndCodes( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *icmpTypesAndCodes) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_IcmpTypesAndCodes( 
            /* [in] */ __RPC__in BSTR icmpTypesAndCodes) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Direction( 
            /* [retval][out] */ __RPC__out NET_FW_RULE_DIRECTION *dir) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Direction( 
            /* [in] */ NET_FW_RULE_DIRECTION dir) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Interfaces( 
            /* [retval][out] */ __RPC__out VARIANT *interfaces) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Interfaces( 
            /* [in] */ VARIANT interfaces) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_InterfaceTypes( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *interfaceTypes) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_InterfaceTypes( 
            /* [in] */ __RPC__in BSTR interfaceTypes) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Enabled( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *enabled) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Enabled( 
            /* [in] */ VARIANT_BOOL enabled) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Grouping( 
            /* [retval][out] */ __RPC__deref_out_opt BSTR *context) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Grouping( 
            /* [in] */ __RPC__in BSTR context) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Profiles( 
            /* [retval][out] */ __RPC__out long *profileTypesBitmask) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Profiles( 
            /* [in] */ long profileTypesBitmask) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_EdgeTraversal( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *enabled) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_EdgeTraversal( 
            /* [in] */ VARIANT_BOOL enabled) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Action( 
            /* [retval][out] */ __RPC__out NET_FW_ACTION *action) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Action( 
            /* [in] */ NET_FW_ACTION action) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwRuleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetFwRule * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetFwRule * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetFwRule * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INetFwRule * This,
            /* [out] */ __RPC__out UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INetFwRule * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INetFwRule * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INetFwRule * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            INetFwRule * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *name);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            INetFwRule * This,
            /* [in] */ __RPC__in BSTR name);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Description )( 
            INetFwRule * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *desc);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Description )( 
            INetFwRule * This,
            /* [in] */ __RPC__in BSTR desc);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ApplicationName )( 
            INetFwRule * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *imageFileName);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ApplicationName )( 
            INetFwRule * This,
            /* [in] */ __RPC__in BSTR imageFileName);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ServiceName )( 
            INetFwRule * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *serviceName);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ServiceName )( 
            INetFwRule * This,
            /* [in] */ __RPC__in BSTR serviceName);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Protocol )( 
            INetFwRule * This,
            /* [retval][out] */ __RPC__out LONG *protocol);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Protocol )( 
            INetFwRule * This,
            /* [in] */ LONG protocol);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_LocalPorts )( 
            INetFwRule * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *portNumbers);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_LocalPorts )( 
            INetFwRule * This,
            /* [in] */ __RPC__in BSTR portNumbers);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_RemotePorts )( 
            INetFwRule * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *portNumbers);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_RemotePorts )( 
            INetFwRule * This,
            /* [in] */ __RPC__in BSTR portNumbers);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_LocalAddresses )( 
            INetFwRule * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *localAddrs);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_LocalAddresses )( 
            INetFwRule * This,
            /* [in] */ __RPC__in BSTR localAddrs);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_RemoteAddresses )( 
            INetFwRule * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *remoteAddrs);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_RemoteAddresses )( 
            INetFwRule * This,
            /* [in] */ __RPC__in BSTR remoteAddrs);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_IcmpTypesAndCodes )( 
            INetFwRule * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *icmpTypesAndCodes);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_IcmpTypesAndCodes )( 
            INetFwRule * This,
            /* [in] */ __RPC__in BSTR icmpTypesAndCodes);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Direction )( 
            INetFwRule * This,
            /* [retval][out] */ __RPC__out NET_FW_RULE_DIRECTION *dir);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Direction )( 
            INetFwRule * This,
            /* [in] */ NET_FW_RULE_DIRECTION dir);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Interfaces )( 
            INetFwRule * This,
            /* [retval][out] */ __RPC__out VARIANT *interfaces);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Interfaces )( 
            INetFwRule * This,
            /* [in] */ VARIANT interfaces);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_InterfaceTypes )( 
            INetFwRule * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *interfaceTypes);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_InterfaceTypes )( 
            INetFwRule * This,
            /* [in] */ __RPC__in BSTR interfaceTypes);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Enabled )( 
            INetFwRule * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *enabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Enabled )( 
            INetFwRule * This,
            /* [in] */ VARIANT_BOOL enabled);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Grouping )( 
            INetFwRule * This,
            /* [retval][out] */ __RPC__deref_out_opt BSTR *context);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Grouping )( 
            INetFwRule * This,
            /* [in] */ __RPC__in BSTR context);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Profiles )( 
            INetFwRule * This,
            /* [retval][out] */ __RPC__out long *profileTypesBitmask);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Profiles )( 
            INetFwRule * This,
            /* [in] */ long profileTypesBitmask);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_EdgeTraversal )( 
            INetFwRule * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *enabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_EdgeTraversal )( 
            INetFwRule * This,
            /* [in] */ VARIANT_BOOL enabled);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Action )( 
            INetFwRule * This,
            /* [retval][out] */ __RPC__out NET_FW_ACTION *action);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Action )( 
            INetFwRule * This,
            /* [in] */ NET_FW_ACTION action);
        
        END_INTERFACE
    } INetFwRuleVtbl;

    interface INetFwRule
    {
        CONST_VTBL struct INetFwRuleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwRule_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INetFwRule_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INetFwRule_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INetFwRule_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INetFwRule_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INetFwRule_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INetFwRule_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INetFwRule_get_Name(This,name)	\
    ( (This)->lpVtbl -> get_Name(This,name) ) 

#define INetFwRule_put_Name(This,name)	\
    ( (This)->lpVtbl -> put_Name(This,name) ) 

#define INetFwRule_get_Description(This,desc)	\
    ( (This)->lpVtbl -> get_Description(This,desc) ) 

#define INetFwRule_put_Description(This,desc)	\
    ( (This)->lpVtbl -> put_Description(This,desc) ) 

#define INetFwRule_get_ApplicationName(This,imageFileName)	\
    ( (This)->lpVtbl -> get_ApplicationName(This,imageFileName) ) 

#define INetFwRule_put_ApplicationName(This,imageFileName)	\
    ( (This)->lpVtbl -> put_ApplicationName(This,imageFileName) ) 

#define INetFwRule_get_ServiceName(This,serviceName)	\
    ( (This)->lpVtbl -> get_ServiceName(This,serviceName) ) 

#define INetFwRule_put_ServiceName(This,serviceName)	\
    ( (This)->lpVtbl -> put_ServiceName(This,serviceName) ) 

#define INetFwRule_get_Protocol(This,protocol)	\
    ( (This)->lpVtbl -> get_Protocol(This,protocol) ) 

#define INetFwRule_put_Protocol(This,protocol)	\
    ( (This)->lpVtbl -> put_Protocol(This,protocol) ) 

#define INetFwRule_get_LocalPorts(This,portNumbers)	\
    ( (This)->lpVtbl -> get_LocalPorts(This,portNumbers) ) 

#define INetFwRule_put_LocalPorts(This,portNumbers)	\
    ( (This)->lpVtbl -> put_LocalPorts(This,portNumbers) ) 

#define INetFwRule_get_RemotePorts(This,portNumbers)	\
    ( (This)->lpVtbl -> get_RemotePorts(This,portNumbers) ) 

#define INetFwRule_put_RemotePorts(This,portNumbers)	\
    ( (This)->lpVtbl -> put_RemotePorts(This,portNumbers) ) 

#define INetFwRule_get_LocalAddresses(This,localAddrs)	\
    ( (This)->lpVtbl -> get_LocalAddresses(This,localAddrs) ) 

#define INetFwRule_put_LocalAddresses(This,localAddrs)	\
    ( (This)->lpVtbl -> put_LocalAddresses(This,localAddrs) ) 

#define INetFwRule_get_RemoteAddresses(This,remoteAddrs)	\
    ( (This)->lpVtbl -> get_RemoteAddresses(This,remoteAddrs) ) 

#define INetFwRule_put_RemoteAddresses(This,remoteAddrs)	\
    ( (This)->lpVtbl -> put_RemoteAddresses(This,remoteAddrs) ) 

#define INetFwRule_get_IcmpTypesAndCodes(This,icmpTypesAndCodes)	\
    ( (This)->lpVtbl -> get_IcmpTypesAndCodes(This,icmpTypesAndCodes) ) 

#define INetFwRule_put_IcmpTypesAndCodes(This,icmpTypesAndCodes)	\
    ( (This)->lpVtbl -> put_IcmpTypesAndCodes(This,icmpTypesAndCodes) ) 

#define INetFwRule_get_Direction(This,dir)	\
    ( (This)->lpVtbl -> get_Direction(This,dir) ) 

#define INetFwRule_put_Direction(This,dir)	\
    ( (This)->lpVtbl -> put_Direction(This,dir) ) 

#define INetFwRule_get_Interfaces(This,interfaces)	\
    ( (This)->lpVtbl -> get_Interfaces(This,interfaces) ) 

#define INetFwRule_put_Interfaces(This,interfaces)	\
    ( (This)->lpVtbl -> put_Interfaces(This,interfaces) ) 

#define INetFwRule_get_InterfaceTypes(This,interfaceTypes)	\
    ( (This)->lpVtbl -> get_InterfaceTypes(This,interfaceTypes) ) 

#define INetFwRule_put_InterfaceTypes(This,interfaceTypes)	\
    ( (This)->lpVtbl -> put_InterfaceTypes(This,interfaceTypes) ) 

#define INetFwRule_get_Enabled(This,enabled)	\
    ( (This)->lpVtbl -> get_Enabled(This,enabled) ) 

#define INetFwRule_put_Enabled(This,enabled)	\
    ( (This)->lpVtbl -> put_Enabled(This,enabled) ) 

#define INetFwRule_get_Grouping(This,context)	\
    ( (This)->lpVtbl -> get_Grouping(This,context) ) 

#define INetFwRule_put_Grouping(This,context)	\
    ( (This)->lpVtbl -> put_Grouping(This,context) ) 

#define INetFwRule_get_Profiles(This,profileTypesBitmask)	\
    ( (This)->lpVtbl -> get_Profiles(This,profileTypesBitmask) ) 

#define INetFwRule_put_Profiles(This,profileTypesBitmask)	\
    ( (This)->lpVtbl -> put_Profiles(This,profileTypesBitmask) ) 

#define INetFwRule_get_EdgeTraversal(This,enabled)	\
    ( (This)->lpVtbl -> get_EdgeTraversal(This,enabled) ) 

#define INetFwRule_put_EdgeTraversal(This,enabled)	\
    ( (This)->lpVtbl -> put_EdgeTraversal(This,enabled) ) 

#define INetFwRule_get_Action(This,action)	\
    ( (This)->lpVtbl -> get_Action(This,action) ) 

#define INetFwRule_put_Action(This,action)	\
    ( (This)->lpVtbl -> put_Action(This,action) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INetFwRule_INTERFACE_DEFINED__ */


#ifndef __INetFwRules_INTERFACE_DEFINED__
#define __INetFwRules_INTERFACE_DEFINED__

/* interface INetFwRules */
/* [dual][uuid][object] */ 


EXTERN_C const IID IID_INetFwRules;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9C4C6277-5027-441E-AFAE-CA1F542DA009")
    INetFwRules : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ __RPC__out long *count) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ __RPC__in_opt INetFwRule *rule) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ __RPC__in BSTR name) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ __RPC__in BSTR name,
            /* [retval][out] */ __RPC__deref_out_opt INetFwRule **rule) = 0;
        
        virtual /* [restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ __RPC__deref_out_opt IUnknown **newEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwRulesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetFwRules * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetFwRules * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetFwRules * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INetFwRules * This,
            /* [out] */ __RPC__out UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INetFwRules * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INetFwRules * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INetFwRules * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            INetFwRules * This,
            /* [retval][out] */ __RPC__out long *count);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            INetFwRules * This,
            /* [in] */ __RPC__in_opt INetFwRule *rule);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Remove )( 
            INetFwRules * This,
            /* [in] */ __RPC__in BSTR name);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            INetFwRules * This,
            /* [in] */ __RPC__in BSTR name,
            /* [retval][out] */ __RPC__deref_out_opt INetFwRule **rule);
        
        /* [restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get__NewEnum )( 
            INetFwRules * This,
            /* [retval][out] */ __RPC__deref_out_opt IUnknown **newEnum);
        
        END_INTERFACE
    } INetFwRulesVtbl;

    interface INetFwRules
    {
        CONST_VTBL struct INetFwRulesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwRules_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INetFwRules_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INetFwRules_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INetFwRules_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INetFwRules_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INetFwRules_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INetFwRules_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INetFwRules_get_Count(This,count)	\
    ( (This)->lpVtbl -> get_Count(This,count) ) 

#define INetFwRules_Add(This,rule)	\
    ( (This)->lpVtbl -> Add(This,rule) ) 

#define INetFwRules_Remove(This,name)	\
    ( (This)->lpVtbl -> Remove(This,name) ) 

#define INetFwRules_Item(This,name,rule)	\
    ( (This)->lpVtbl -> Item(This,name,rule) ) 

#define INetFwRules_get__NewEnum(This,newEnum)	\
    ( (This)->lpVtbl -> get__NewEnum(This,newEnum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INetFwRules_INTERFACE_DEFINED__ */


#ifndef __INetFwServiceRestriction_INTERFACE_DEFINED__
#define __INetFwServiceRestriction_INTERFACE_DEFINED__

/* interface INetFwServiceRestriction */
/* [dual][uuid][object] */ 


EXTERN_C const IID IID_INetFwServiceRestriction;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8267BBE3-F890-491C-B7B6-2DB1EF0E5D2B")
    INetFwServiceRestriction : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RestrictService( 
            /* [in] */ __RPC__in BSTR serviceName,
            /* [in] */ __RPC__in BSTR appName,
            /* [in] */ VARIANT_BOOL restrictService,
            /* [in] */ VARIANT_BOOL serviceSidRestricted) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ServiceRestricted( 
            /* [in] */ __RPC__in BSTR serviceName,
            /* [in] */ __RPC__in BSTR appName,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *serviceRestricted) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Rules( 
            /* [retval][out] */ __RPC__deref_out_opt INetFwRules **rules) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwServiceRestrictionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetFwServiceRestriction * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetFwServiceRestriction * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetFwServiceRestriction * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INetFwServiceRestriction * This,
            /* [out] */ __RPC__out UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INetFwServiceRestriction * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INetFwServiceRestriction * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INetFwServiceRestriction * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RestrictService )( 
            INetFwServiceRestriction * This,
            /* [in] */ __RPC__in BSTR serviceName,
            /* [in] */ __RPC__in BSTR appName,
            /* [in] */ VARIANT_BOOL restrictService,
            /* [in] */ VARIANT_BOOL serviceSidRestricted);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ServiceRestricted )( 
            INetFwServiceRestriction * This,
            /* [in] */ __RPC__in BSTR serviceName,
            /* [in] */ __RPC__in BSTR appName,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *serviceRestricted);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Rules )( 
            INetFwServiceRestriction * This,
            /* [retval][out] */ __RPC__deref_out_opt INetFwRules **rules);
        
        END_INTERFACE
    } INetFwServiceRestrictionVtbl;

    interface INetFwServiceRestriction
    {
        CONST_VTBL struct INetFwServiceRestrictionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwServiceRestriction_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INetFwServiceRestriction_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INetFwServiceRestriction_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INetFwServiceRestriction_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INetFwServiceRestriction_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INetFwServiceRestriction_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INetFwServiceRestriction_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INetFwServiceRestriction_RestrictService(This,serviceName,appName,restrictService,serviceSidRestricted)	\
    ( (This)->lpVtbl -> RestrictService(This,serviceName,appName,restrictService,serviceSidRestricted) ) 

#define INetFwServiceRestriction_ServiceRestricted(This,serviceName,appName,serviceRestricted)	\
    ( (This)->lpVtbl -> ServiceRestricted(This,serviceName,appName,serviceRestricted) ) 

#define INetFwServiceRestriction_get_Rules(This,rules)	\
    ( (This)->lpVtbl -> get_Rules(This,rules) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INetFwServiceRestriction_INTERFACE_DEFINED__ */


#ifndef __INetFwProfile_INTERFACE_DEFINED__
#define __INetFwProfile_INTERFACE_DEFINED__

/* interface INetFwProfile */
/* [dual][uuid][object] */ 


EXTERN_C const IID IID_INetFwProfile;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("174A0DDA-E9F9-449D-993B-21AB667CA456")
    INetFwProfile : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ __RPC__out NET_FW_PROFILE_TYPE *type) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_FirewallEnabled( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *enabled) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_FirewallEnabled( 
            /* [in] */ VARIANT_BOOL enabled) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ExceptionsNotAllowed( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *notAllowed) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_ExceptionsNotAllowed( 
            /* [in] */ VARIANT_BOOL notAllowed) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_NotificationsDisabled( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *disabled) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_NotificationsDisabled( 
            /* [in] */ VARIANT_BOOL disabled) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_UnicastResponsesToMulticastBroadcastDisabled( 
            /* [retval][out] */ __RPC__out VARIANT_BOOL *disabled) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_UnicastResponsesToMulticastBroadcastDisabled( 
            /* [in] */ VARIANT_BOOL disabled) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_RemoteAdminSettings( 
            /* [retval][out] */ __RPC__deref_out_opt INetFwRemoteAdminSettings **remoteAdminSettings) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_IcmpSettings( 
            /* [retval][out] */ __RPC__deref_out_opt INetFwIcmpSettings **icmpSettings) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_GloballyOpenPorts( 
            /* [retval][out] */ __RPC__deref_out_opt INetFwOpenPorts **openPorts) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Services( 
            /* [retval][out] */ __RPC__deref_out_opt INetFwServices **services) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_AuthorizedApplications( 
            /* [retval][out] */ __RPC__deref_out_opt INetFwAuthorizedApplications **apps) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwProfileVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetFwProfile * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetFwProfile * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetFwProfile * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INetFwProfile * This,
            /* [out] */ __RPC__out UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INetFwProfile * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INetFwProfile * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INetFwProfile * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Type )( 
            INetFwProfile * This,
            /* [retval][out] */ __RPC__out NET_FW_PROFILE_TYPE *type);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_FirewallEnabled )( 
            INetFwProfile * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *enabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_FirewallEnabled )( 
            INetFwProfile * This,
            /* [in] */ VARIANT_BOOL enabled);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ExceptionsNotAllowed )( 
            INetFwProfile * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *notAllowed);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ExceptionsNotAllowed )( 
            INetFwProfile * This,
            /* [in] */ VARIANT_BOOL notAllowed);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_NotificationsDisabled )( 
            INetFwProfile * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *disabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_NotificationsDisabled )( 
            INetFwProfile * This,
            /* [in] */ VARIANT_BOOL disabled);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_UnicastResponsesToMulticastBroadcastDisabled )( 
            INetFwProfile * This,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *disabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_UnicastResponsesToMulticastBroadcastDisabled )( 
            INetFwProfile * This,
            /* [in] */ VARIANT_BOOL disabled);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_RemoteAdminSettings )( 
            INetFwProfile * This,
            /* [retval][out] */ __RPC__deref_out_opt INetFwRemoteAdminSettings **remoteAdminSettings);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_IcmpSettings )( 
            INetFwProfile * This,
            /* [retval][out] */ __RPC__deref_out_opt INetFwIcmpSettings **icmpSettings);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_GloballyOpenPorts )( 
            INetFwProfile * This,
            /* [retval][out] */ __RPC__deref_out_opt INetFwOpenPorts **openPorts);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Services )( 
            INetFwProfile * This,
            /* [retval][out] */ __RPC__deref_out_opt INetFwServices **services);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AuthorizedApplications )( 
            INetFwProfile * This,
            /* [retval][out] */ __RPC__deref_out_opt INetFwAuthorizedApplications **apps);
        
        END_INTERFACE
    } INetFwProfileVtbl;

    interface INetFwProfile
    {
        CONST_VTBL struct INetFwProfileVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwProfile_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INetFwProfile_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INetFwProfile_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INetFwProfile_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INetFwProfile_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INetFwProfile_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INetFwProfile_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INetFwProfile_get_Type(This,type)	\
    ( (This)->lpVtbl -> get_Type(This,type) ) 

#define INetFwProfile_get_FirewallEnabled(This,enabled)	\
    ( (This)->lpVtbl -> get_FirewallEnabled(This,enabled) ) 

#define INetFwProfile_put_FirewallEnabled(This,enabled)	\
    ( (This)->lpVtbl -> put_FirewallEnabled(This,enabled) ) 

#define INetFwProfile_get_ExceptionsNotAllowed(This,notAllowed)	\
    ( (This)->lpVtbl -> get_ExceptionsNotAllowed(This,notAllowed) ) 

#define INetFwProfile_put_ExceptionsNotAllowed(This,notAllowed)	\
    ( (This)->lpVtbl -> put_ExceptionsNotAllowed(This,notAllowed) ) 

#define INetFwProfile_get_NotificationsDisabled(This,disabled)	\
    ( (This)->lpVtbl -> get_NotificationsDisabled(This,disabled) ) 

#define INetFwProfile_put_NotificationsDisabled(This,disabled)	\
    ( (This)->lpVtbl -> put_NotificationsDisabled(This,disabled) ) 

#define INetFwProfile_get_UnicastResponsesToMulticastBroadcastDisabled(This,disabled)	\
    ( (This)->lpVtbl -> get_UnicastResponsesToMulticastBroadcastDisabled(This,disabled) ) 

#define INetFwProfile_put_UnicastResponsesToMulticastBroadcastDisabled(This,disabled)	\
    ( (This)->lpVtbl -> put_UnicastResponsesToMulticastBroadcastDisabled(This,disabled) ) 

#define INetFwProfile_get_RemoteAdminSettings(This,remoteAdminSettings)	\
    ( (This)->lpVtbl -> get_RemoteAdminSettings(This,remoteAdminSettings) ) 

#define INetFwProfile_get_IcmpSettings(This,icmpSettings)	\
    ( (This)->lpVtbl -> get_IcmpSettings(This,icmpSettings) ) 

#define INetFwProfile_get_GloballyOpenPorts(This,openPorts)	\
    ( (This)->lpVtbl -> get_GloballyOpenPorts(This,openPorts) ) 

#define INetFwProfile_get_Services(This,services)	\
    ( (This)->lpVtbl -> get_Services(This,services) ) 

#define INetFwProfile_get_AuthorizedApplications(This,apps)	\
    ( (This)->lpVtbl -> get_AuthorizedApplications(This,apps) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INetFwProfile_INTERFACE_DEFINED__ */


#ifndef __INetFwPolicy_INTERFACE_DEFINED__
#define __INetFwPolicy_INTERFACE_DEFINED__

/* interface INetFwPolicy */
/* [dual][uuid][object] */ 


EXTERN_C const IID IID_INetFwPolicy;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D46D2478-9AC9-4008-9DC7-5563CE5536CC")
    INetFwPolicy : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentProfile( 
            /* [retval][out] */ __RPC__deref_out_opt INetFwProfile **profile) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetProfileByType( 
            /* [in] */ NET_FW_PROFILE_TYPE profileType,
            /* [retval][out] */ __RPC__deref_out_opt INetFwProfile **profile) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwPolicyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetFwPolicy * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetFwPolicy * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetFwPolicy * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INetFwPolicy * This,
            /* [out] */ __RPC__out UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INetFwPolicy * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INetFwPolicy * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INetFwPolicy * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_CurrentProfile )( 
            INetFwPolicy * This,
            /* [retval][out] */ __RPC__deref_out_opt INetFwProfile **profile);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetProfileByType )( 
            INetFwPolicy * This,
            /* [in] */ NET_FW_PROFILE_TYPE profileType,
            /* [retval][out] */ __RPC__deref_out_opt INetFwProfile **profile);
        
        END_INTERFACE
    } INetFwPolicyVtbl;

    interface INetFwPolicy
    {
        CONST_VTBL struct INetFwPolicyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwPolicy_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INetFwPolicy_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INetFwPolicy_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INetFwPolicy_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INetFwPolicy_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INetFwPolicy_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INetFwPolicy_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INetFwPolicy_get_CurrentProfile(This,profile)	\
    ( (This)->lpVtbl -> get_CurrentProfile(This,profile) ) 

#define INetFwPolicy_GetProfileByType(This,profileType,profile)	\
    ( (This)->lpVtbl -> GetProfileByType(This,profileType,profile) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INetFwPolicy_INTERFACE_DEFINED__ */


#ifndef __INetFwPolicy2_INTERFACE_DEFINED__
#define __INetFwPolicy2_INTERFACE_DEFINED__

/* interface INetFwPolicy2 */
/* [dual][uuid][object] */ 


EXTERN_C const IID IID_INetFwPolicy2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("98325047-C671-4174-8D81-DEFCD3F03186")
    INetFwPolicy2 : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentProfileTypes( 
            /* [retval][out] */ __RPC__out long *profileTypesBitmask) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_FirewallEnabled( 
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *enabled) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_FirewallEnabled( 
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [in] */ VARIANT_BOOL enabled) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ExcludedInterfaces( 
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [retval][out] */ __RPC__out VARIANT *interfaces) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_ExcludedInterfaces( 
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [in] */ VARIANT interfaces) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_BlockAllInboundTraffic( 
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *Block) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_BlockAllInboundTraffic( 
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [in] */ VARIANT_BOOL Block) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_NotificationsDisabled( 
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *disabled) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_NotificationsDisabled( 
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [in] */ VARIANT_BOOL disabled) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_UnicastResponsesToMulticastBroadcastDisabled( 
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *disabled) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_UnicastResponsesToMulticastBroadcastDisabled( 
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [in] */ VARIANT_BOOL disabled) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Rules( 
            /* [retval][out] */ __RPC__deref_out_opt INetFwRules **rules) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_ServiceRestriction( 
            /* [retval][out] */ __RPC__deref_out_opt INetFwServiceRestriction **ServiceRestriction) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE EnableRuleGroup( 
            /* [in] */ long profileTypesBitmask,
            /* [in] */ __RPC__in BSTR group,
            /* [in] */ VARIANT_BOOL enable) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsRuleGroupEnabled( 
            /* [in] */ long profileTypesBitmask,
            /* [in] */ __RPC__in BSTR group,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *enabled) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RestoreLocalFirewallDefaults( void) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_DefaultInboundAction( 
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [retval][out] */ __RPC__out NET_FW_ACTION *action) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_DefaultInboundAction( 
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [in] */ NET_FW_ACTION action) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_DefaultOutboundAction( 
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [retval][out] */ __RPC__out NET_FW_ACTION *action) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_DefaultOutboundAction( 
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [in] */ NET_FW_ACTION action) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_IsRuleGroupCurrentlyEnabled( 
            /* [in] */ __RPC__in BSTR group,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *enabled) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_LocalPolicyModifyState( 
            /* [retval][out] */ __RPC__out NET_FW_MODIFY_STATE *modifyState) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwPolicy2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetFwPolicy2 * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetFwPolicy2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetFwPolicy2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INetFwPolicy2 * This,
            /* [out] */ __RPC__out UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INetFwPolicy2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INetFwPolicy2 * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INetFwPolicy2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_CurrentProfileTypes )( 
            INetFwPolicy2 * This,
            /* [retval][out] */ __RPC__out long *profileTypesBitmask);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_FirewallEnabled )( 
            INetFwPolicy2 * This,
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *enabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_FirewallEnabled )( 
            INetFwPolicy2 * This,
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [in] */ VARIANT_BOOL enabled);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ExcludedInterfaces )( 
            INetFwPolicy2 * This,
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [retval][out] */ __RPC__out VARIANT *interfaces);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ExcludedInterfaces )( 
            INetFwPolicy2 * This,
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [in] */ VARIANT interfaces);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_BlockAllInboundTraffic )( 
            INetFwPolicy2 * This,
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *Block);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_BlockAllInboundTraffic )( 
            INetFwPolicy2 * This,
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [in] */ VARIANT_BOOL Block);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_NotificationsDisabled )( 
            INetFwPolicy2 * This,
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *disabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_NotificationsDisabled )( 
            INetFwPolicy2 * This,
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [in] */ VARIANT_BOOL disabled);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_UnicastResponsesToMulticastBroadcastDisabled )( 
            INetFwPolicy2 * This,
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *disabled);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_UnicastResponsesToMulticastBroadcastDisabled )( 
            INetFwPolicy2 * This,
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [in] */ VARIANT_BOOL disabled);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Rules )( 
            INetFwPolicy2 * This,
            /* [retval][out] */ __RPC__deref_out_opt INetFwRules **rules);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ServiceRestriction )( 
            INetFwPolicy2 * This,
            /* [retval][out] */ __RPC__deref_out_opt INetFwServiceRestriction **ServiceRestriction);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *EnableRuleGroup )( 
            INetFwPolicy2 * This,
            /* [in] */ long profileTypesBitmask,
            /* [in] */ __RPC__in BSTR group,
            /* [in] */ VARIANT_BOOL enable);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *IsRuleGroupEnabled )( 
            INetFwPolicy2 * This,
            /* [in] */ long profileTypesBitmask,
            /* [in] */ __RPC__in BSTR group,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *enabled);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RestoreLocalFirewallDefaults )( 
            INetFwPolicy2 * This);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_DefaultInboundAction )( 
            INetFwPolicy2 * This,
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [retval][out] */ __RPC__out NET_FW_ACTION *action);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_DefaultInboundAction )( 
            INetFwPolicy2 * This,
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [in] */ NET_FW_ACTION action);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_DefaultOutboundAction )( 
            INetFwPolicy2 * This,
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [retval][out] */ __RPC__out NET_FW_ACTION *action);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_DefaultOutboundAction )( 
            INetFwPolicy2 * This,
            /* [in] */ NET_FW_PROFILE_TYPE2 profileType,
            /* [in] */ NET_FW_ACTION action);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_IsRuleGroupCurrentlyEnabled )( 
            INetFwPolicy2 * This,
            /* [in] */ __RPC__in BSTR group,
            /* [retval][out] */ __RPC__out VARIANT_BOOL *enabled);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_LocalPolicyModifyState )( 
            INetFwPolicy2 * This,
            /* [retval][out] */ __RPC__out NET_FW_MODIFY_STATE *modifyState);
        
        END_INTERFACE
    } INetFwPolicy2Vtbl;

    interface INetFwPolicy2
    {
        CONST_VTBL struct INetFwPolicy2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwPolicy2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INetFwPolicy2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INetFwPolicy2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INetFwPolicy2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INetFwPolicy2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INetFwPolicy2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INetFwPolicy2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INetFwPolicy2_get_CurrentProfileTypes(This,profileTypesBitmask)	\
    ( (This)->lpVtbl -> get_CurrentProfileTypes(This,profileTypesBitmask) ) 

#define INetFwPolicy2_get_FirewallEnabled(This,profileType,enabled)	\
    ( (This)->lpVtbl -> get_FirewallEnabled(This,profileType,enabled) ) 

#define INetFwPolicy2_put_FirewallEnabled(This,profileType,enabled)	\
    ( (This)->lpVtbl -> put_FirewallEnabled(This,profileType,enabled) ) 

#define INetFwPolicy2_get_ExcludedInterfaces(This,profileType,interfaces)	\
    ( (This)->lpVtbl -> get_ExcludedInterfaces(This,profileType,interfaces) ) 

#define INetFwPolicy2_put_ExcludedInterfaces(This,profileType,interfaces)	\
    ( (This)->lpVtbl -> put_ExcludedInterfaces(This,profileType,interfaces) ) 

#define INetFwPolicy2_get_BlockAllInboundTraffic(This,profileType,Block)	\
    ( (This)->lpVtbl -> get_BlockAllInboundTraffic(This,profileType,Block) ) 

#define INetFwPolicy2_put_BlockAllInboundTraffic(This,profileType,Block)	\
    ( (This)->lpVtbl -> put_BlockAllInboundTraffic(This,profileType,Block) ) 

#define INetFwPolicy2_get_NotificationsDisabled(This,profileType,disabled)	\
    ( (This)->lpVtbl -> get_NotificationsDisabled(This,profileType,disabled) ) 

#define INetFwPolicy2_put_NotificationsDisabled(This,profileType,disabled)	\
    ( (This)->lpVtbl -> put_NotificationsDisabled(This,profileType,disabled) ) 

#define INetFwPolicy2_get_UnicastResponsesToMulticastBroadcastDisabled(This,profileType,disabled)	\
    ( (This)->lpVtbl -> get_UnicastResponsesToMulticastBroadcastDisabled(This,profileType,disabled) ) 

#define INetFwPolicy2_put_UnicastResponsesToMulticastBroadcastDisabled(This,profileType,disabled)	\
    ( (This)->lpVtbl -> put_UnicastResponsesToMulticastBroadcastDisabled(This,profileType,disabled) ) 

#define INetFwPolicy2_get_Rules(This,rules)	\
    ( (This)->lpVtbl -> get_Rules(This,rules) ) 

#define INetFwPolicy2_get_ServiceRestriction(This,ServiceRestriction)	\
    ( (This)->lpVtbl -> get_ServiceRestriction(This,ServiceRestriction) ) 

#define INetFwPolicy2_EnableRuleGroup(This,profileTypesBitmask,group,enable)	\
    ( (This)->lpVtbl -> EnableRuleGroup(This,profileTypesBitmask,group,enable) ) 

#define INetFwPolicy2_IsRuleGroupEnabled(This,profileTypesBitmask,group,enabled)	\
    ( (This)->lpVtbl -> IsRuleGroupEnabled(This,profileTypesBitmask,group,enabled) ) 

#define INetFwPolicy2_RestoreLocalFirewallDefaults(This)	\
    ( (This)->lpVtbl -> RestoreLocalFirewallDefaults(This) ) 

#define INetFwPolicy2_get_DefaultInboundAction(This,profileType,action)	\
    ( (This)->lpVtbl -> get_DefaultInboundAction(This,profileType,action) ) 

#define INetFwPolicy2_put_DefaultInboundAction(This,profileType,action)	\
    ( (This)->lpVtbl -> put_DefaultInboundAction(This,profileType,action) ) 

#define INetFwPolicy2_get_DefaultOutboundAction(This,profileType,action)	\
    ( (This)->lpVtbl -> get_DefaultOutboundAction(This,profileType,action) ) 

#define INetFwPolicy2_put_DefaultOutboundAction(This,profileType,action)	\
    ( (This)->lpVtbl -> put_DefaultOutboundAction(This,profileType,action) ) 

#define INetFwPolicy2_get_IsRuleGroupCurrentlyEnabled(This,group,enabled)	\
    ( (This)->lpVtbl -> get_IsRuleGroupCurrentlyEnabled(This,group,enabled) ) 

#define INetFwPolicy2_get_LocalPolicyModifyState(This,modifyState)	\
    ( (This)->lpVtbl -> get_LocalPolicyModifyState(This,modifyState) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INetFwPolicy2_INTERFACE_DEFINED__ */


#ifndef __INetFwMgr_INTERFACE_DEFINED__
#define __INetFwMgr_INTERFACE_DEFINED__

/* interface INetFwMgr */
/* [dual][uuid][object] */ 


EXTERN_C const IID IID_INetFwMgr;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F7898AF5-CAC4-4632-A2EC-DA06E5111AF2")
    INetFwMgr : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_LocalPolicy( 
            /* [retval][out] */ __RPC__deref_out_opt INetFwPolicy **localPolicy) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_CurrentProfileType( 
            /* [retval][out] */ __RPC__out NET_FW_PROFILE_TYPE *profileType) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RestoreDefaults( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsPortAllowed( 
            /* [in] */ __RPC__in BSTR imageFileName,
            /* [in] */ NET_FW_IP_VERSION ipVersion,
            /* [in] */ LONG portNumber,
            /* [in] */ __RPC__in BSTR localAddress,
            /* [in] */ NET_FW_IP_PROTOCOL ipProtocol,
            /* [out] */ __RPC__out VARIANT *allowed,
            /* [out] */ __RPC__out VARIANT *restricted) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsIcmpTypeAllowed( 
            /* [in] */ NET_FW_IP_VERSION ipVersion,
            /* [in] */ __RPC__in BSTR localAddress,
            /* [in] */ BYTE type,
            /* [out] */ __RPC__out VARIANT *allowed,
            /* [out] */ __RPC__out VARIANT *restricted) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetFwMgrVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetFwMgr * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetFwMgr * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetFwMgr * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INetFwMgr * This,
            /* [out] */ __RPC__out UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INetFwMgr * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INetFwMgr * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INetFwMgr * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_LocalPolicy )( 
            INetFwMgr * This,
            /* [retval][out] */ __RPC__deref_out_opt INetFwPolicy **localPolicy);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_CurrentProfileType )( 
            INetFwMgr * This,
            /* [retval][out] */ __RPC__out NET_FW_PROFILE_TYPE *profileType);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RestoreDefaults )( 
            INetFwMgr * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *IsPortAllowed )( 
            INetFwMgr * This,
            /* [in] */ __RPC__in BSTR imageFileName,
            /* [in] */ NET_FW_IP_VERSION ipVersion,
            /* [in] */ LONG portNumber,
            /* [in] */ __RPC__in BSTR localAddress,
            /* [in] */ NET_FW_IP_PROTOCOL ipProtocol,
            /* [out] */ __RPC__out VARIANT *allowed,
            /* [out] */ __RPC__out VARIANT *restricted);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *IsIcmpTypeAllowed )( 
            INetFwMgr * This,
            /* [in] */ NET_FW_IP_VERSION ipVersion,
            /* [in] */ __RPC__in BSTR localAddress,
            /* [in] */ BYTE type,
            /* [out] */ __RPC__out VARIANT *allowed,
            /* [out] */ __RPC__out VARIANT *restricted);
        
        END_INTERFACE
    } INetFwMgrVtbl;

    interface INetFwMgr
    {
        CONST_VTBL struct INetFwMgrVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetFwMgr_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INetFwMgr_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INetFwMgr_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INetFwMgr_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define INetFwMgr_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define INetFwMgr_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define INetFwMgr_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define INetFwMgr_get_LocalPolicy(This,localPolicy)	\
    ( (This)->lpVtbl -> get_LocalPolicy(This,localPolicy) ) 

#define INetFwMgr_get_CurrentProfileType(This,profileType)	\
    ( (This)->lpVtbl -> get_CurrentProfileType(This,profileType) ) 

#define INetFwMgr_RestoreDefaults(This)	\
    ( (This)->lpVtbl -> RestoreDefaults(This) ) 

#define INetFwMgr_IsPortAllowed(This,imageFileName,ipVersion,portNumber,localAddress,ipProtocol,allowed,restricted)	\
    ( (This)->lpVtbl -> IsPortAllowed(This,imageFileName,ipVersion,portNumber,localAddress,ipProtocol,allowed,restricted) ) 

#define INetFwMgr_IsIcmpTypeAllowed(This,ipVersion,localAddress,type,allowed,restricted)	\
    ( (This)->lpVtbl -> IsIcmpTypeAllowed(This,ipVersion,localAddress,type,allowed,restricted) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INetFwMgr_INTERFACE_DEFINED__ */



#ifndef __NetFwPublicTypeLib_LIBRARY_DEFINED__
#define __NetFwPublicTypeLib_LIBRARY_DEFINED__

/* library NetFwPublicTypeLib */
/* [version][uuid] */ 

















EXTERN_C const IID LIBID_NetFwPublicTypeLib;

EXTERN_C const CLSID CLSID_NetFwRule;

#ifdef __cplusplus

class DECLSPEC_UUID("2C5BC43E-3369-4C33-AB0C-BE9469677AF4")
NetFwRule;
#endif

EXTERN_C const CLSID CLSID_NetFwOpenPort;

#ifdef __cplusplus

class DECLSPEC_UUID("0CA545C6-37AD-4A6C-BF92-9F7610067EF5")
NetFwOpenPort;
#endif

EXTERN_C const CLSID CLSID_NetFwAuthorizedApplication;

#ifdef __cplusplus

class DECLSPEC_UUID("EC9846B3-2762-4A6B-A214-6ACB603462D2")
NetFwAuthorizedApplication;
#endif

EXTERN_C const CLSID CLSID_NetFwPolicy2;

#ifdef __cplusplus

class DECLSPEC_UUID("E2B3C97F-6AE1-41AC-817A-F6F92166D7DD")
NetFwPolicy2;
#endif

EXTERN_C const CLSID CLSID_NetFwMgr;

#ifdef __cplusplus

class DECLSPEC_UUID("304CE942-6E39-40D8-943A-B913C40C9CD4")
NetFwMgr;
#endif
#endif /* __NetFwPublicTypeLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

unsigned long             __RPC_USER  BSTR_UserSize64(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal64(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal64(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree64(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize64(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal64(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal64(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree64(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



