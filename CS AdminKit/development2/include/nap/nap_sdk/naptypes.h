
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Oct 29 17:32:30 2007
 */
/* Compiler settings for O:\CS AdminKit\development2\nap\nap_sdk\include\naptypes.idl:
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

#ifndef __naptypes_h__
#define __naptypes_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_naptypes_0000 */
/* [local] */ 





extern RPC_IF_HANDLE __MIDL_itf_naptypes_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_naptypes_0000_v0_0_s_ifspec;

#ifndef __INapTypes_INTERFACE_DEFINED__
#define __INapTypes_INTERFACE_DEFINED__

/* interface INapTypes */
/* [auto_handle][unique] */ 

#define	maxSoHAttributeCount	( 100 )

#define	maxSoHAttributeSize	( 4000 )

#define	minNetworkSoHSize	( 12 )

#define	maxNetworkSoHSize	( 4000 )

#define	maxDwordCountPerSoHAttribute	( maxSoHAttributeSize / sizeof( DWORD  ) )

#define	maxIpv4CountPerSoHAttribute	( maxSoHAttributeSize / 4 )

#define	maxIpv6CountPerSoHAttribute	( maxSoHAttributeSize / 16 )

#define	maxStringLength	( 1024 )

#define	maxStringLengthInBytes	( (maxStringLength + 1) * sizeof( WCHAR  ) )

#define	maxSystemHealthEntityCount	( 20 )

#define	maxEnforcerCount	( 20 )

#define	maxPrivateDataSize	( 200 )

#define	maxConnectionCountPerEnforcer	( 20 )

#define	maxCachedSoHCount	( maxSystemHealthEntityCount * maxEnforcerCount * maxConnectionCountPerEnforcer )

#define	freshSoHRequest	( 0x1 )

#define	shaFixup	( 0x1 )

typedef 
enum tagIsolationState
    {	isolationStateNotRestricted	= 1,
	isolationStateInProbation	= 2,
	isolationStateRestrictedAccess	= 3
    } 	IsolationState;

typedef 
enum tagNapTracingLevel
    {	tracingLevelUndefined	= 0,
	tracingLevelBasic	= 1,
	tracingLevelAdvanced	= 2,
	tracingLevelDebug	= 3
    } 	NapTracingLevel;

typedef FILETIME ProbationTime;

typedef struct tagCountedString
    {
    /* [range] */ UINT16 length;
    /* [string][size_is] */ WCHAR *string;
    } 	CountedString;

typedef struct tagIsolationInfo
    {
    IsolationState isolationState;
    ProbationTime probEndTime;
    CountedString failureUrl;
    } 	IsolationInfo;

#define	failureCategoryCount	( 5 )

typedef 
enum tagFailureCategory
    {	failureCategoryNone	= 0,
	failureCategoryOther	= 1,
	failureCategoryClientComponent	= 2,
	failureCategoryClientCommunication	= 3,
	failureCategoryServerComponent	= 4,
	failureCategoryServerCommunication	= 5
    } 	FailureCategory;

typedef struct tagFailureCategoryMapping
    {
    BOOL mappingCompliance[ 5 ];
    } 	FailureCategoryMapping;

typedef UINT32 NapComponentId;

typedef NapComponentId SystemHealthEntityId;

typedef NapComponentId EnforcementEntityId;

#define	ComponentTypeEnforcementClientSoH	( 0x1 )

#define	ComponentTypeEnforcementClientRp	( 0x2 )

typedef /* [range] */ UINT16 SystemHealthEntityCount;

typedef /* [range] */ UINT16 EnforcementEntityCount;

typedef struct tagCorrelationId
    {
    GUID connId;
    FILETIME timeStamp;
    } 	CorrelationId;

typedef CountedString StringCorrelationId;

typedef GUID ConnectionId;

#define	percentageNotSupported	( 101 )

typedef /* [range] */ UINT8 Percentage;

typedef UINT32 MessageId;

typedef struct tagResultCodes
    {
    /* [range] */ UINT16 count;
    /* [size_is] */ HRESULT *results;
    } 	ResultCodes;

typedef struct tagIpv4Address
    {
    BYTE addr[ 4 ];
    } 	Ipv4Address;

typedef struct tagIpv6Address
    {
    BYTE addr[ 16 ];
    } 	Ipv6Address;

typedef 
enum tagFixupState
    {	fixupStateSuccess	= 0,
	fixupStateInProgress	= 1,
	fixupStateCouldNotUpdate	= 2
    } 	FixupState;

typedef struct tagFixupInfo
    {
    FixupState state;
    Percentage percentage;
    ResultCodes resultCodes;
    MessageId fixupMsgId;
    } 	FixupInfo;

typedef 
enum tagNapNotifyType
    {	napNotifyTypeUnknown	= 0,
	napNotifyTypeServiceState	= 1,
	napNotifyTypeQuarState	= 2
    } 	NapNotifyType;

typedef struct tagSystemHealthAgentState
    {
    SystemHealthEntityId id;
    ResultCodes shaResultCodes;
    FailureCategory failureCategory;
    FixupInfo fixupInfo;
    } 	SystemHealthAgentState;

typedef struct tagSoHAttribute
    {
    UINT16 type;
    /* [range] */ UINT16 size;
    /* [size_is] */ BYTE *value;
    } 	SoHAttribute;

typedef struct tagSoH
    {
    /* [range] */ UINT16 count;
    /* [size_is] */ SoHAttribute *attributes;
    } 	SoH;

typedef struct tagSoH SoHRequest;

typedef struct tagSoH SoHResponse;

typedef struct tagNetworkSoH
    {
    /* [range] */ UINT16 size;
    /* [size_is] */ BYTE *data;
    } 	NetworkSoH;

typedef struct tagNetworkSoH NetworkSoHRequest;

typedef struct tagNetworkSoH NetworkSoHResponse;

typedef struct tagPrivateData
    {
    /* [range] */ UINT16 size;
    /* [size_is] */ BYTE *data;
    } 	PrivateData;

typedef struct tagNapComponentRegistrationInfo
    {
    NapComponentId id;
    CountedString friendlyName;
    CountedString description;
    CountedString version;
    CountedString vendorName;
    CLSID infoClsid;
    CLSID configClsid;
    FILETIME registrationDate;
    UINT32 componentType;
    } 	NapComponentRegistrationInfo;



extern RPC_IF_HANDLE INapTypes_v0_0_c_ifspec;
extern RPC_IF_HANDLE INapTypes_v0_0_s_ifspec;
#endif /* __INapTypes_INTERFACE_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


