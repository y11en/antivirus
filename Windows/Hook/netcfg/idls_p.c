

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0364 */
/* Compiler settings for idls.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */
#pragma warning( disable: 4211 )  /* redefine extent to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma optimize("", off ) 

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "idls.h"

#define TYPE_FORMAT_STRING_SIZE   3                                 
#define PROC_FORMAT_STRING_SIZE   1                                 
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



#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need a Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */

			0x0
        }
    };


/* Standard interface: __MIDL_itf_idls_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IEnumNetCfgBindingInterface, ver. 0.0,
   GUID={0xC0E8AE90,0x306E,0x11D1,{0xAA,0xCF,0x00,0x80,0x5F,0xC1,0x27,0x0E}} */


/* Object interface: IEnumNetCfgBindingPath, ver. 0.0,
   GUID={0xC0E8AE91,0x306E,0x11D1,{0xAA,0xCF,0x00,0x80,0x5F,0xC1,0x27,0x0E}} */


/* Object interface: IEnumNetCfgComponent, ver. 0.0,
   GUID={0xC0E8AE92,0x306E,0x11D1,{0xAA,0xCF,0x00,0x80,0x5F,0xC1,0x27,0x0E}} */


/* Object interface: INetCfg, ver. 0.0,
   GUID={0xC0E8AE93,0x306E,0x11D1,{0xAA,0xCF,0x00,0x80,0x5F,0xC1,0x27,0x0E}} */


/* Object interface: INetCfgLock, ver. 0.0,
   GUID={0xC0E8AE9F,0x306E,0x11D1,{0xAA,0xCF,0x00,0x80,0x5F,0xC1,0x27,0x0E}} */


/* Object interface: INetCfgBindingInterface, ver. 0.0,
   GUID={0xC0E8AE94,0x306E,0x11D1,{0xAA,0xCF,0x00,0x80,0x5F,0xC1,0x27,0x0E}} */


/* Object interface: INetCfgBindingPath, ver. 0.0,
   GUID={0xC0E8AE96,0x306E,0x11D1,{0xAA,0xCF,0x00,0x80,0x5F,0xC1,0x27,0x0E}} */


/* Object interface: INetCfgClass, ver. 0.0,
   GUID={0xC0E8AE97,0x306E,0x11D1,{0xAA,0xCF,0x00,0x80,0x5F,0xC1,0x27,0x0E}} */


/* Object interface: INetCfgClassSetup, ver. 0.0,
   GUID={0xC0E8AE9D,0x306E,0x11D1,{0xAA,0xCF,0x00,0x80,0x5F,0xC1,0x27,0x0E}} */


/* Object interface: INetCfgComponent, ver. 0.0,
   GUID={0xC0E8AE99,0x306E,0x11D1,{0xAA,0xCF,0x00,0x80,0x5F,0xC1,0x27,0x0E}} */


/* Object interface: INetCfgComponentBindings, ver. 0.0,
   GUID={0xC0E8AE9E,0x306E,0x11D1,{0xAA,0xCF,0x00,0x80,0x5F,0xC1,0x27,0x0E}} */

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
    0x600016c, /* MIDL Version 6.0.364 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0   /* Reserved5 */
    };

const CInterfaceProxyVtbl * _idls_ProxyVtblList[] = 
{
    0
};

const CInterfaceStubVtbl * _idls_StubVtblList[] = 
{
    0
};

PCInterfaceName const _idls_InterfaceNamesList[] = 
{
    0
};


#define _idls_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _idls, pIID, n)

int __stdcall _idls_IID_Lookup( const IID * pIID, int * pIndex )
{
    return 0;
}

const ExtendedProxyFileInfo idls_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _idls_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _idls_StubVtblList,
    (const PCInterfaceName * ) & _idls_InterfaceNamesList,
    0, // no delegation
    & _idls_IID_Lookup, 
    0,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

