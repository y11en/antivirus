#ifndef __AVPPVEID_H__
#define __AVPPVEID_H__


#include <property/property.h>
#include <avpcomid.h>

#define VE_APP_ID										AVP_APID_GLOBAL //(1) Repaced to AVPCommID.h

// AVPPattern IDs and PIDs

// Interfaces Root ID
#define VE_PID_ROOT_ID									(1)
#define VE_PID_ROOT											MAKE_AVP_PID(VE_PID_ROOT_ID,VE_APP_ID, avpt_dword, 0)

// Interface Properties Root node
#define VE_PID_IF_PROPERTIES_ID					(1)
#define VE_PID_IF_PROPERTIES						MAKE_AVP_PID(VE_PID_IF_PROPERTIES_ID,VE_APP_ID,avpt_nothing,0)

// Interface Methods Root node
#define VE_PID_IF_METHODS_ID						(2)
#define VE_PID_IF_METHODS								MAKE_AVP_PID(VE_PID_IF_METHODS_ID,VE_APP_ID,avpt_nothing,0)
/*
// Interface System Methods Root node
#define VE_PID_IF_SYSMETHODS_ID					(3)
#define VE_PID_IF_SYSMETHODS						MAKE_AVP_PID(VE_PID_IF_SYSMETHODS_ID,VE_APP_ID,avpt_nothing,0)
*/
// Interface Data Structures Root node
#define VE_PID_IF_DATASTRUCTURES_ID			(4)
#define VE_PID_IF_DATASTRUCTURES				MAKE_AVP_PID(VE_PID_IF_DATASTRUCTURES_ID,VE_APP_ID,avpt_nothing,0)

// Interface Data Structure Members Root node
#define VE_PID_IF_DATASTRUCTUREMEMBERS_ID		(5)
#define VE_PID_IF_DATASTRUCTUREMEMBERS			MAKE_AVP_PID(VE_PID_IF_DATASTRUCTUREMEMBERS_ID,VE_APP_ID,avpt_nothing,0)

// Interface Types node
#define VE_PID_IF_TYPES_ID							(6)
#define VE_PID_IF_TYPES									MAKE_AVP_PID(VE_PID_IF_TYPES_ID,VE_APP_ID,avpt_nothing,0)

// Interface Constants node
#define VE_PID_IF_CONSTANTS_ID					(7)
#define VE_PID_IF_CONSTANTS							MAKE_AVP_PID(VE_PID_IF_CONSTANTS_ID,VE_APP_ID,avpt_nothing,0)

// Interface System Methods Root node
#define VE_PID_IF_SYSTEMMETHODS_ID			(8)
#define VE_PID_IF_SYSTEMMETHODS					MAKE_AVP_PID(VE_PID_IF_SYSTEMMETHODS_ID,VE_APP_ID,avpt_nothing,0)

// Interface error Codes Root node
#define VE_PID_IF_ERRORCODES_ID			    (9)
#define VE_PID_IF_ERRORCODES					  MAKE_AVP_PID(VE_PID_IF_ERRORCODES_ID,VE_APP_ID,avpt_nothing,0)

// Plugin exports
#define VE_PID_PL_EXPORTS_ID            (10)
#define VE_PID_PL_EXPORTS               MAKE_AVP_PID(VE_PID_PL_EXPORTS_ID,VE_APP_ID,avpt_nothing,0)

// Plugin imports
#define VE_PID_PL_IMPORTS_ID            (11)
#define VE_PID_PL_IMPORTS               MAKE_AVP_PID(VE_PID_PL_IMPORTS_ID,VE_APP_ID,avpt_nothing,0)

// Interface messages
#define VE_PID_PL_MESSAGES_ID           (12)
#define VE_PID_PL_MESSAGES              MAKE_AVP_PID(VE_PID_PL_MESSAGES_ID,VE_APP_ID,avpt_nothing,0)



// ---------------------------------------------------------------------------------------------------------
// Interface node
// ID interface node
// a - unique uint value
#define VE_PID_INTERFACE(a)							MAKE_AVP_PID(a,VE_APP_ID,avpt_nothing,0)

// Interface name PROP
#define VE_PID_IF_INTERFACENAME_ID			(101)
#define VE_PID_IF_INTERFACENAME				  MAKE_AVP_PID(VE_PID_IF_INTERFACENAME_ID,VE_APP_ID,avpt_str,0)

// Interface mnemonic ID PROP
#define VE_PID_IF_MNEMONICID_ID					(102)
#define VE_PID_IF_MNEMONICID						MAKE_AVP_PID(VE_PID_IF_MNEMONICID_ID,VE_APP_ID,avpt_str,0)

// Interface digital ID PROP
#define VE_PID_IF_DIGITALID_ID				  (103)
#define VE_PID_IF_DIGITALID							MAKE_AVP_PID(VE_PID_IF_DIGITALID_ID,VE_APP_ID,avpt_dword,0)

// Interface revision ID PROP
#define VE_PID_IF_REVISIONID_ID					(104)
#define VE_PID_IF_REVISIONID						MAKE_AVP_PID(VE_PID_IF_REVISIONID_ID,VE_APP_ID,avpt_dword,0)

// Interface creation date PROP
#define VE_PID_IF_CREATIONDATE_ID				(105)
#define VE_PID_IF_CREATIONDATE					MAKE_AVP_PID(VE_PID_IF_CREATIONDATE_ID,VE_APP_ID,avpt_date,0)

// Interface include file name PROP
#define VE_PID_IF_INCLUDEFILE_ID				(106)
#define VE_PID_IF_INCLUDEFILE						MAKE_AVP_PID(VE_PID_IF_INCLUDEFILE_ID,VE_APP_ID,avpt_str,0)

// Interface object type name PROP
#define VE_PID_IF_OBJECTTYPE_ID					(107)
#define VE_PID_IF_OBJECTTYPE					  MAKE_AVP_PID(VE_PID_IF_OBJECTTYPE_ID,VE_APP_ID,avpt_str,0)

// Interface is cheked PROP
#define VE_PID_IF_SELECTED_ID						(110)
#define VE_PID_IF_SELECTED								MAKE_AVP_PID(VE_PID_IF_SELECTED_ID,VE_APP_ID,avpt_bool,0)

// Interface header GUID PROP
#define VE_PID_IF_HEADERGUID_ID					(111)
#define VE_PID_IF_HEADERGUID						MAKE_AVP_PID(VE_PID_IF_HEADERGUID_ID,VE_APP_ID,avpt_str,0)

// Interface is imported PROP
#define VE_PID_IF_IMPORTED_ID						(112)
#define VE_PID_IF_IMPORTED								MAKE_AVP_PID(VE_PID_IF_IMPORTED_ID,VE_APP_ID,avpt_bool,0)

// Interface import file name PROP
#define VE_PID_IF_IMPORTFILENAME_ID			(113)
#define VE_PID_IF_IMPORTFILENAME				MAKE_AVP_PID(VE_PID_IF_IMPORTFILENAME_ID,VE_APP_ID,avpt_str,0)

// Interface SubType ID PROP
#define VE_PID_IF_SUBTYPEID_ID					(114)
#define VE_PID_IF_SUBTYPEID						  MAKE_AVP_PID(VE_PID_IF_SUBTYPEID_ID,VE_APP_ID,avpt_dword,0)

// Interface autor name PROP
#define VE_PID_IF_AUTORNAME_ID					(115)
#define VE_PID_IF_AUTORNAME							MAKE_AVP_PID(VE_PID_IF_AUTORNAME_ID,VE_APP_ID,avpt_str,0)

// Interface used include names PROP
#define VE_PID_IF_INCLUDENAMES_ID				(116)
#define VE_PID_IF_INCLUDENAMES					MAKE_AVP_PID(VE_PID_IF_INCLUDENAMES_ID,VE_APP_ID,avpt_str,1)

// Interface is static PROP
#define VE_PID_IF_STATIC_ID							(117)
#define VE_PID_IF_STATIC								MAKE_AVP_PID(VE_PID_IF_STATIC_ID,VE_APP_ID,avpt_bool,0)

// Interface transfer prop up PROP
#define VE_PID_IF_TRANSFERPROPUP_ID			(118)
#define VE_PID_IF_TRANSFERPROPUP				MAKE_AVP_PID(VE_PID_IF_TRANSFERPROPUP_ID,VE_APP_ID,avpt_bool,0)

// Interface last call prop PROP
#define VE_PID_IF_LASTCALLPROP_ID				(119)
#define VE_PID_IF_LASTCALLPROP					MAKE_AVP_PID(VE_PID_IF_LASTCALLPROP_ID,VE_APP_ID,avpt_bool,0)

// Interface last call prop read func PROP
#define VE_PID_IFLCP_READFUNC_ID				(120)
#define VE_PID_IFLCP_READFUNC						MAKE_AVP_PID(VE_PID_IFLCP_READFUNC_ID,VE_APP_ID,avpt_str,0)

// Interface last call prop write func PROP
#define VE_PID_IFLCP_WRITEFUNC_ID				(121)
#define VE_PID_IFLCP_WRITEFUNC					MAKE_AVP_PID(VE_PID_IFLCP_WRITEFUNC_ID,VE_APP_ID,avpt_str,0)

// Interface in non updatable PROP
#define VE_PID_IF_NONSWAPABLE_ID				(122)
#define VE_PID_IF_NONSWAPABLE					  MAKE_AVP_PID(VE_PID_IF_NONSWAPABLE_ID,VE_APP_ID,avpt_bool,0)

#define VE_IFF_PROTECTED_BY_NONE  0
#define VE_IFF_PROTECTED_BY_CS	  1
#define VE_IFF_PROTECTED_BY_MUTEX	2

// Interface protected by PROP
#define VE_PID_IF_PROTECTED_BY_ID		    (130)
#define VE_PID_IF_PROTECTED_BY				  MAKE_AVP_PID(VE_PID_IF_PROTECTED_BY_ID,VE_APP_ID,avpt_dword,0)

// Interface in non updatable PROP
#define VE_PID_IF_SYSTEM_ID				      (131)
#define VE_PID_IF_SYSTEM                MAKE_AVP_PID(VE_PID_IF_SYSTEM_ID,VE_APP_ID,avpt_bool,0)

// Interface is selected for implementation PROP
//#define VE_PID_IF_SELECTEDFORIMP_ID		  (123)
//#define VE_PID_IF_SELECTEDFORIMP				MAKE_AVP_PID(VE_PID_IF_SELECTEDFORIMP_ID,VE_APP_ID,avpt_bool,0)

#define VE_PID_IF_EXPLICIT_PARENT_ID	  (132)
#define VE_PID_IF_EXPLICIT_PARENT       MAKE_AVP_PID(VE_PID_IF_EXPLICIT_PARENT_ID,VE_APP_ID,avpt_bool,0)

// Interface vendor ID PROP
#define VE_PID_IF_VENDORID_ID						(124)
#define VE_PID_IF_VENDORID							MAKE_AVP_PID(VE_PID_IF_VENDORID_ID,VE_APP_ID,avpt_dword,0)

// Interface import relative file name PROP
#define VE_PID_IF_IMPORTRELATIVEFILENAME_ID			(125)
#define VE_PID_IF_IMPORTRELATIVEFILENAME				MAKE_AVP_PID(VE_PID_IF_IMPORTRELATIVEFILENAME_ID,VE_APP_ID,avpt_str,0)

// Interface output file name PROP
#define VE_PID_IF_OUTPUTFILE_ID					(127)
#define VE_PID_IF_OUTPUTFILE						MAKE_AVP_PID(VE_PID_IF_OUTPUTFILE_ID,VE_APP_ID,avpt_str,0)

// Linked unique IDs PROP
#define VE_PID_LINKEDUNIQUEID_ID				(128)
#define VE_PID_LINKEDUNIQUEID						MAKE_AVP_PID(VE_PID_LINKEDUNIQUEID_ID,VE_APP_ID,avpt_str,1)

// Interface used include UIDs PROP
#define VE_PID_IF_INCLUDEUIDS_ID				(129)
#define VE_PID_IF_INCLUDEUIDS					  MAKE_AVP_PID(VE_PID_IF_INCLUDEUIDS_ID,VE_APP_ID,avpt_str,1)

// System PROP
#define VE_PID_IF_LASTINTERFACESLEVEL_ID	(GET_AVP_PID_ID_MAX_VALUE)
#define VE_PID_IF_LASTINTERFACESLEVEL			MAKE_AVP_PID(VE_PID_IF_LASTINTERFACESLEVEL_ID,VE_APP_ID,avpt_nothing,0)

#define VE_PID_IF_LINKEDBYINTERFACE_ID	  (GET_AVP_PID_ID_MAX_VALUE - 1)
#define VE_PID_IF_LINKEDBYINTERFACE			  MAKE_AVP_PID(VE_PID_IF_LINKEDBYINTERFACE_ID,VE_APP_ID,avpt_nothing,0)

#define VE_PID_IF_CONTAINERNAME_ID				(GET_AVP_PID_ID_MAX_VALUE - 2)
#define VE_PID_IF_CONTAINERNAME						MAKE_AVP_PID(VE_PID_IF_CONTAINERNAME_ID,VE_APP_ID,avpt_str,0)

// that's property has plugin default pseudo interface and only that interface
// that interface has for instance plugin global methods and has not property interfacename
#define VE_PID_IF_PLUGIN_STATIC_ID				(130)
#define VE_PID_IF_PLUGIN_STATIC					  MAKE_AVP_PID(VE_PID_IF_PLUGIN_STATIC_ID,VE_APP_ID,avpt_dword,1)

// ---------------------------------------------------------------------------------------------------------

// ID property node
// a - unique uint value
#define VE_PID_IF_PROPERTY(a)						MAKE_AVP_PID(a,VE_APP_ID,avpt_nothing,0)

// Interface property name PROP
#define VE_PID_IFP_PROPNAME_ID				  (201)
#define VE_PID_IFP_PROPNAME						  MAKE_AVP_PID(VE_PID_IFP_PROPNAME_ID,VE_APP_ID,avpt_str,0)

// Interface property digital ID PROP						
#define VE_PID_IFP_DIGITALID_ID			    (202)
#define VE_PID_IFP_DIGITALID				    MAKE_AVP_PID(VE_PID_IFP_DIGITALID_ID,VE_APP_ID,avpt_dword,0)

// Interface property type PROP
#define VE_PID_IFP_TYPE_ID							(203)
#define VE_PID_IFP_TYPE									MAKE_AVP_PID(VE_PID_IFP_TYPE_ID,VE_APP_ID,avpt_dword,0)

// Interface property type name PROP
#define VE_PID_IFP_TYPENAME_ID				  (204)
#define VE_PID_IFP_TYPENAME						  MAKE_AVP_PID(VE_PID_IFP_TYPENAME_ID,VE_APP_ID,avpt_str,0)

// Interface property scope PROP
#define VE_IFP_LOCAL	0
#define VE_IFP_SHARED 1

#define VE_PID_IFP_SCOPE_ID							(205)
#define VE_PID_IFP_SCOPE								MAKE_AVP_PID(VE_PID_IFP_SCOPE_ID,VE_APP_ID,avpt_dword,0)

// Interface property local scope required PROP
#define VE_PID_IFPLS_REQUIRED_ID				(206)
#define VE_PID_IFPLS_REQUIRED						MAKE_AVP_PID(VE_PID_IFPLS_REQUIRED_ID,VE_APP_ID,avpt_bool,0)

// Interface property local scope member PROP
#define VE_PID_IFPLS_MEMBER_ID					(207)
#define VE_PID_IFPLS_MEMBER							MAKE_AVP_PID(VE_PID_IFPLS_MEMBER_ID,VE_APP_ID,avpt_str,0)

// Interface property local scope mode PROP
#define VE_IFPLS_NONE				0
#define VE_IFPLS_READ				1
#define VE_IFPLS_WRITE			2
#define VE_IFPLS_READWRITE	3

#define VE_PID_IFPLS_MODE_ID						(208)
#define VE_PID_IFPLS_MODE								MAKE_AVP_PID(VE_PID_IFPLS_MODE_ID,VE_APP_ID,avpt_dword,0)

// Interface property local scope read func PROP
#define VE_PID_IFPLS_READFUNC_ID				(209)
#define VE_PID_IFPLS_READFUNC						MAKE_AVP_PID(VE_PID_IFPLS_READFUNC_ID,VE_APP_ID,avpt_str,0)

// Interface property local scope write func PROP
#define VE_PID_IFPLS_WRITEFUNC_ID				(210)
#define VE_PID_IFPLS_WRITEFUNC					MAKE_AVP_PID(VE_PID_IFPLS_WRITEFUNC_ID,VE_APP_ID,avpt_str,0)

// Interface property shared scope variable name PROP
#define VE_PID_IFPSS_VARIABLENAME_ID		(211)
#define VE_PID_IFPSS_VARIABLENAME				MAKE_AVP_PID(VE_PID_IFPSS_VARIABLENAME_ID,VE_APP_ID,avpt_str,0)

// Interface property shared scope variable defvalue PROP
#define VE_PID_IFPSS_VARIABLEDEFVALUE_ID		(212)
#define VE_PID_IFPSS_VARIABLEDEFVALUE				MAKE_AVP_PID(VE_PID_IFPSS_VARIABLEDEFVALUE_ID,VE_APP_ID,avpt_str,0)
/*
// Interface property shared scope pointer PROP
#define VE_PID_IFPSS_POINTER_ID					(213)
#define VE_PID_IFPSS_POINTER						MAKE_AVP_PID(VE_PID_IFPSS_POINTER_ID,VE_APP_ID,avpt_bool,0)
*/
// Interface property is native PROP
#define VE_PID_IFP_NATIVE_ID						(214)
#define VE_PID_IFP_NATIVE								MAKE_AVP_PID(VE_PID_IFP_NATIVE_ID,VE_APP_ID,avpt_bool,0)

// Interface property member mode PROP
#define VE_IFPLS_MEMBER_READ			1
#define VE_IFPLS_MEMBER_WRITE			2
#define VE_IFPLS_MEMBER_READWRITE	3

#define VE_PID_IFPLS_MEMBER_MODE_ID				(215)
#define VE_PID_IFPLS_MEMBER_MODE					MAKE_AVP_PID(VE_PID_IFPLS_MEMBER_MODE_ID,VE_APP_ID,avpt_dword,0)

// Interface property is predefined PROP
#define VE_PID_IFP_PREDEFINED_ID						(216)
#define VE_PID_IFP_PREDEFINED								MAKE_AVP_PID(VE_PID_IFP_PREDEFINED_ID,VE_APP_ID,avpt_bool,0)

// Interface property is hard predefined PROP
#define VE_PID_IFP_HARDPREDEFINED_ID				(217)
#define VE_PID_IFP_HARDPREDEFINED						MAKE_AVP_PID(VE_PID_IFP_HARDPREDEFINED_ID,VE_APP_ID,avpt_bool,0)

// Interface property shared scope variable typified defvalue PROP
#define VE_PID_IFPSS_VARTYPIFIEDDEFVALUE_ID		(218)
#define VE_PID_IFPSS_VARTYPIFIEDDEFVALUE(a)		MAKE_AVP_PID(VE_PID_IFPSS_VARTYPIFIEDDEFVALUE_ID,VE_APP_ID,a,0)

// Interface property is writable during init time only PROP
#define VE_PID_IFPLS_WRITABLEDURINGINIT_ID		(219)
#define VE_PID_IFPLS_WRITABLEDURINGINIT				MAKE_AVP_PID(VE_PID_IFPLS_WRITABLEDURINGINIT_ID,VE_APP_ID,avpt_bool,0)

// ---------------------------------------------------------------------------------------------------------

// ID method node
// a - unique uint value
#define VE_PID_IF_METHOD(a)							MAKE_AVP_PID(a,VE_APP_ID,avpt_nothing,0)

// Interface method name PROP
#define VE_PID_IFM_METHODNAME_ID				(301)
#define VE_PID_IFM_METHODNAME						MAKE_AVP_PID(VE_PID_IFM_METHODNAME_ID,VE_APP_ID,avpt_str,0)

// Interface method result value type PROP
#define VE_PID_IFM_RESULTTYPE_ID				(306)
#define VE_PID_IFM_RESULTTYPE						MAKE_AVP_PID(VE_PID_IFM_RESULTTYPE_ID,VE_APP_ID,avpt_dword,0)

// Interface method result value type name PROP
#define VE_PID_IFM_RESULTTYPENAME_ID		(307)
#define VE_PID_IFM_RESULTTYPENAME				MAKE_AVP_PID(VE_PID_IFM_RESULTTYPENAME_ID,VE_APP_ID,avpt_str,0)

// Interface method result value by default PROP
#define VE_PID_IFM_RESULTDEFVALUE_ID		(308)
#define VE_PID_IFM_RESULTDEFVALUE				MAKE_AVP_PID(VE_PID_IFM_RESULTDEFVALUE_ID,VE_APP_ID,avpt_str,0)

// Interface system method checked PROP
#define VE_PID_IFM_SELECTED_ID				 (309)
#define VE_PID_IFM_SELECTED					   MAKE_AVP_PID(VE_PID_IFM_SELECTED_ID,VE_APP_ID,avpt_bool,0)

// method id 
#define VE_PID_IFM_METHOD_ID_ID        (310)
#define VE_PID_IFM_METHOD_ID				   MAKE_AVP_PID(VE_PID_IFM_METHOD_ID_ID,VE_APP_ID,avpt_dword,0)

// Interface method extend flag
#define VE_PID_IFM_EXTENDED_ID        (311)
#define VE_PID_IFM_EXTENDED				   MAKE_AVP_PID(VE_PID_IFM_EXTENDED_ID,VE_APP_ID,avpt_dword,0)

// ---------------------------------------------------------------------------------------------------------

// ID method parameter node
// a - unique uint value
#define VE_PID_IFM_PARAMETER(a)					MAKE_AVP_PID(a,VE_APP_ID,avpt_nothing,0)

// Interface method parameter name PROP
#define VE_PID_IFMP_PARAMETERNAME_ID		(401)
#define VE_PID_IFMP_PARAMETERNAME				MAKE_AVP_PID(VE_PID_IFMP_PARAMETERNAME_ID,VE_APP_ID,avpt_str,0)

// Interface method parameter type name PROP
#define VE_PID_IFMP_TYPENAME_ID		      (402)
#define VE_PID_IFMP_TYPENAME				    MAKE_AVP_PID(VE_PID_IFMP_TYPENAME_ID,VE_APP_ID,avpt_str,0)

// Interface method parameter type PROP
#define VE_PID_IFMP_TYPE_ID							(403)
#define VE_PID_IFMP_TYPE								MAKE_AVP_PID(VE_PID_IFMP_TYPE_ID,VE_APP_ID,avpt_dword,0)

// Interface method parameter treated as const PROP
#define VE_PID_IFMP_ISCONST_ID						(405)
#define VE_PID_IFMP_ISCONST								MAKE_AVP_PID(VE_PID_IFMP_ISCONST_ID,VE_APP_ID,avpt_bool,0)

// Interface method parameter treated as pointer PROP
#define VE_PID_IFMP_ISPOINTER_ID				(406)
#define VE_PID_IFMP_ISPOINTER						MAKE_AVP_PID(VE_PID_IFMP_ISPOINTER_ID,VE_APP_ID,avpt_bool,0)

// Interface method parameter treated as pointer PROP
#define VE_PID_IFMP_IS_DOUBLE_POINTER_ID	(407)
#define VE_PID_IFMP_IS_DOUBLE_POINTER			MAKE_AVP_PID(VE_PID_IFMP_IS_DOUBLE_POINTER_ID,VE_APP_ID,avpt_bool,0)
// ---------------------------------------------------------------------------------------------------------

// ID interface data structure node
// a - unique uint value
#define VE_PID_IF_DATASTRUCTURE(a)						MAKE_AVP_PID(a,VE_APP_ID,avpt_nothing,0)

// Interface data structure name PROP
#define VE_PID_IFD_DATASTRUCTURENAME_ID				(501)
#define VE_PID_IFD_DATASTRUCTURENAME					MAKE_AVP_PID(VE_PID_IFD_DATASTRUCTURENAME_ID,VE_APP_ID,avpt_str,0)

// ---------------------------------------------------------------------------------------------------------

// ID interface data structure member node
// a - unique uint value
#define VE_PID_IFD_MEMBER(a)									MAKE_AVP_PID(a,VE_APP_ID,avpt_nothing,0)

// Interface member name PROP
#define VE_PID_IFDM_MEMBERNAME_ID							(601)
#define VE_PID_IFDM_MEMBERNAME								MAKE_AVP_PID(VE_PID_IFDM_MEMBERNAME_ID,VE_APP_ID,avpt_str,0)

// Interface member type name PROP
#define VE_PID_IFDM_TYPENAME_ID								(602)
#define VE_PID_IFDM_TYPENAME									MAKE_AVP_PID(VE_PID_IFDM_TYPENAME_ID,VE_APP_ID,avpt_str,0)

// Interface member is array PROP
#define VE_PID_IFDM_ISARRAY_ID								(604)
#define VE_PID_IFDM_ISARRAY										MAKE_AVP_PID(VE_PID_IFDM_ISARRAY_ID,VE_APP_ID,avpt_bool,0)

// Interface member array bound PROP
#define VE_PID_IFDM_ARRAYBOUND_ID							(605)
#define VE_PID_IFDM_ARRAYBOUND								MAKE_AVP_PID(VE_PID_IFDM_ARRAYBOUND_ID,VE_APP_ID,avpt_str,0)

// ---------------------------------------------------------------------------------------------------------
/*
// Interface system method checked PROP
#define VE_PID_IFSM_SELECTED_ID				 (701)
#define VE_PID_IFSM_SELECTED					 MAKE_AVP_PID(VE_PID_IFSM_SELECTED_ID,VE_APP_ID,avpt_bool,1)
*/
// ---------------------------------------------------------------------------------------------------------

// Plugin name PROP
#define VE_PID_PL_PLUGINNAME_ID					(801)
#define VE_PID_PL_PLUGINNAME						MAKE_AVP_PID(VE_PID_PL_PLUGINNAME_ID,VE_APP_ID,avpt_str,0)

// Plugin digital ID PROP
#define VE_PID_PL_DIGITALID_ID				  (802)
#define VE_PID_PL_DIGITALID							MAKE_AVP_PID(VE_PID_PL_DIGITALID_ID,VE_APP_ID,avpt_dword,0)

// Plugin version ID PROP
#define VE_PID_PL_VERSIONID_ID					(803)
#define VE_PID_PL_VERSIONID							MAKE_AVP_PID(VE_PID_PL_VERSIONID_ID,VE_APP_ID,avpt_dword,0)

// Plugin mnemonic ID PROP
#define VE_PID_PL_MNEMONICID_ID					(806)
#define VE_PID_PL_MNEMONICID						MAKE_AVP_PID(VE_PID_PL_MNEMONICID_ID,VE_APP_ID,avpt_str,0)

// Plugin vendor ID PROP
#define VE_PID_PL_VENDORID_ID						(807)
#define VE_PID_PL_VENDORID							MAKE_AVP_PID(VE_PID_PL_VENDORID_ID,VE_APP_ID,avpt_dword,0)

// Plugin vendor mnemonic ID PROP
#define VE_PID_PL_VENDORMNEMONICID_ID		(808)
#define VE_PID_PL_VENDORMNEMONICID			MAKE_AVP_PID(VE_PID_PL_VENDORMNEMONICID_ID,VE_APP_ID,avpt_str,0)

// Plugin vendor name PROP
#define VE_PID_PL_VENDORNAME_ID					(809)
#define VE_PID_PL_VENDORNAME						MAKE_AVP_PID(VE_PID_PL_VENDORNAME_ID,VE_APP_ID,avpt_str,0)

// Plugin code page ID PROP
#define VE_PID_PL_CODEPAGEID_ID					(810)
#define VE_PID_PL_CODEPAGEID						MAKE_AVP_PID(VE_PID_PL_CODEPAGEID_ID,VE_APP_ID,avpt_dword,0)

// Plugin date format ID PROP
#define VE_PID_PL_DATEFORMATID_ID				(811)
#define VE_PID_PL_DATEFORMATID					MAKE_AVP_PID(VE_PID_PL_DATEFORMATID_ID,VE_APP_ID,avpt_dword,0)

// Plugin author PROP
#define VE_PID_PL_AUTHORNAME_ID					(812)
#define VE_PID_PL_AUTHORNAME						MAKE_AVP_PID(VE_PID_PL_AUTHORNAME_ID,VE_APP_ID,avpt_str,0)

// Plugin project PROP
#define VE_PID_PL_PROJECTNAME_ID				(813)
#define VE_PID_PL_PROJECTNAME						MAKE_AVP_PID(VE_PID_PL_PROJECTNAME_ID,VE_APP_ID,avpt_str,0)

// Plugin subproject PROP
#define VE_PID_PL_SUBPROJECTNAME_ID			(814)
#define VE_PID_PL_SUBPROJECTNAME				MAKE_AVP_PID(VE_PID_PL_SUBPROJECTNAME_ID,VE_APP_ID,avpt_str,0)

// Plugin is auto started PROP
#define VE_PID_PL_AUTOSTART_ID					(815)
#define VE_PID_PL_AUTOSTART							MAKE_AVP_PID(VE_PID_PL_AUTOSTART_ID,VE_APP_ID,avpt_bool,0)

// ---------------------------------------------------------------------------------------------------------

// ID type node
// a - unique uint value
#define VE_PID_IF_TYPE(a)								MAKE_AVP_PID(a,VE_APP_ID,avpt_nothing,0)

// Type name PROP
#define VE_PID_IFT_TYPENAME_ID					(901)
#define VE_PID_IFT_TYPENAME							MAKE_AVP_PID(VE_PID_IFT_TYPENAME_ID,VE_APP_ID,avpt_str,0)

// Type base type name PROP
#define VE_PID_IFT_BASETYPENAME_ID			(902)
#define VE_PID_IFT_BASETYPENAME					MAKE_AVP_PID(VE_PID_IFT_BASETYPENAME_ID,VE_APP_ID,avpt_str,0)

// Type base type PROP
#define VE_PID_IFT_BASETYPE_ID					(903)
#define VE_PID_IFT_BASETYPE							MAKE_AVP_PID(VE_PID_IFT_BASETYPE_ID,VE_APP_ID,avpt_dword,0)

// Type is native PROP
#define VE_PID_IFT_NATIVE_ID						(904)
#define VE_PID_IFT_NATIVE								MAKE_AVP_PID(VE_PID_IFT_NATIVE_ID,VE_APP_ID,avpt_bool,0)

// ---------------------------------------------------------------------------------------------------------

// ID message node
// ?????????????????

// ---------------------------------------------------------------------------------------------------------

// ID constant node
// a - unique uint value
#define VE_PID_IF_CONSTANT(a)						MAKE_AVP_PID(a,VE_APP_ID,avpt_nothing,0)

// Constant name PROP
#define VE_PID_IFC_CONSTANTNAME_ID			(1001)
#define VE_PID_IFC_CONSTANTNAME					MAKE_AVP_PID(VE_PID_IFC_CONSTANTNAME_ID,VE_APP_ID,avpt_str,0)

// Constant base type name PROP
#define VE_PID_IFC_BASETYPENAME_ID			(1002)
#define VE_PID_IFC_BASETYPENAME					MAKE_AVP_PID(VE_PID_IFC_BASETYPENAME_ID,VE_APP_ID,avpt_str,0)

// Constant base type PROP
#define VE_PID_IFC_BASETYPE_ID					(1003)
#define VE_PID_IFC_BASETYPE							MAKE_AVP_PID(VE_PID_IFC_BASETYPE_ID,VE_APP_ID,avpt_dword,0)

// Constant value PROP
#define VE_PID_IFC_VALUE_ID							(1004)
#define VE_PID_IFC_VALUE								MAKE_AVP_PID(VE_PID_IFC_VALUE_ID,VE_APP_ID,avpt_str,0)

// Interface constant is native PROP
#define VE_PID_IFC_NATIVE_ID						(1005)
#define VE_PID_IFC_NATIVE								MAKE_AVP_PID(VE_PID_IFC_NATIVE_ID,VE_APP_ID,avpt_bool,0)

// ---------------------------------------------------------------------------------------------------------
// ID error code node
// a - unique uint value
#define VE_PID_IF_ERRORCODE(a)					MAKE_AVP_PID(a,VE_APP_ID,avpt_nothing,0)

// Error code name PROP
#define VE_PID_IFE_ERRORCODENAME_ID			(1101)
#define VE_PID_IFE_ERRORCODENAME				MAKE_AVP_PID(VE_PID_IFE_ERRORCODENAME_ID,VE_APP_ID,avpt_str,0)

// Error code value PROP
#define VE_PID_IFE_ERRORCODEVALUE_ID		(1102)
#define VE_PID_IFE_ERRORCODEVALUE				MAKE_AVP_PID(VE_PID_IFE_ERRORCODEVALUE_ID,VE_APP_ID,avpt_dword,0)

// Error code is warning PROP
#define VE_PID_IFE_ISWARNING_ID					(1103)
#define VE_PID_IFE_ISWARNING					MAKE_AVP_PID(VE_PID_IFE_ISWARNING_ID,VE_APP_ID,avpt_bool,0)

// Error code is native PROP
#define VE_PID_IFE_NATIVE_ID						(1104)
#define VE_PID_IFE_NATIVE								MAKE_AVP_PID(VE_PID_IFE_NATIVE_ID,VE_APP_ID,avpt_bool,0)

// ---------------------------------------------------------------------------------------------------------
// ID message class node
// a - unique uint value
#define VE_PID_IF_MESSAGECLASS(a)			  MAKE_AVP_PID(a,VE_APP_ID,avpt_nothing,0)

// Message class name PROP
#define VE_PID_IFM_MESSAGECLASSNAME_ID	(1151)
#define VE_PID_IFM_MESSAGECLASSNAME			MAKE_AVP_PID(VE_PID_IFM_MESSAGECLASSNAME_ID,VE_APP_ID,avpt_str,0)

// Message value PROP
#define VE_PID_IFM_MESSAGECLASSID_ID		(1152)
#define VE_PID_IFM_MESSAGECLASSID				MAKE_AVP_PID(VE_PID_IFM_MESSAGECLASSID_ID,VE_APP_ID,avpt_dword,0)

// Message class native PROP
#define VE_PID_IFMC_NATIVE_ID						(214)
#define VE_PID_IFMC_NATIVE							MAKE_AVP_PID(VE_PID_IFMC_NATIVE_ID,VE_APP_ID,avpt_bool,0)

// ---------------------------------------------------------------------------------------------------------
// ID message node
// a - unique uint value
#define VE_PID_IF_MESSAGE(a)						MAKE_AVP_PID(a,VE_APP_ID,avpt_nothing,0)

// Message name PROP
#define VE_PID_IFM_MESSAGENAME_ID				(1201)
#define VE_PID_IFM_MESSAGENAME					MAKE_AVP_PID(VE_PID_IFM_MESSAGENAME_ID,VE_APP_ID,avpt_str,0)

// Message value PROP
#define VE_PID_IFM_MESSAGEID_ID					(1202)
#define VE_PID_IFM_MESSAGEID						MAKE_AVP_PID(VE_PID_IFM_MESSAGEID_ID,VE_APP_ID,avpt_dword,0)

// Message context PROP
#define VE_PID_IFM_MESSAGECONTEXT_ID		(1203)
#define VE_PID_IFM_MESSAGECONTEXT				MAKE_AVP_PID(VE_PID_IFM_MESSAGECONTEXT_ID,VE_APP_ID,avpt_str,0)

// Message data PROP
#define VE_PID_IFM_MESSAGEDATA_ID				(1204)
#define VE_PID_IFM_MESSAGEDATA					MAKE_AVP_PID(VE_PID_IFM_MESSAGEDATA_ID,VE_APP_ID,avpt_str,0)

// Message send point PROP
#define VE_PID_IFM_MESSAGE_SEND_POINT_ID		(1205)
#define VE_PID_IFM_MESSAGE_SEND_POINT		MAKE_AVP_PID(VE_PID_IFM_MESSAGE_SEND_POINT_ID,VE_APP_ID,avpt_str,0)

// Message class native PROP
#define VE_PID_IFM_NATIVE_ID						(214)
#define VE_PID_IFM_NATIVE								MAKE_AVP_PID(VE_PID_IFM_NATIVE_ID,VE_APP_ID,avpt_bool,0)

// ---------------------------------------------------------------------------------------------------------
// Standard commetaries props section

// Short comment PROP
#define VE_PID_SHORTCOMMENT_ID		(10000)
#define VE_PID_SHORTCOMMENT			MAKE_AVP_PID(VE_PID_SHORTCOMMENT_ID,VE_APP_ID,avpt_str,0)

// Large comment PROP
#define VE_PID_LARGECOMMENT_ID		(10001)
#define VE_PID_LARGECOMMENT			MAKE_AVP_PID(VE_PID_LARGECOMMENT_ID,VE_APP_ID,avpt_str,0)

// Behavior descripton PROP
#define VE_PID_BEHAVIORCOMMENT_ID	(10002)
#define VE_PID_BEHAVIORCOMMENT		MAKE_AVP_PID(VE_PID_BEHAVIORCOMMENT_ID,VE_APP_ID,avpt_str,0)

// Value description PROP
#define VE_PID_VALUECOMMENT_ID		(10003)
#define VE_PID_VALUECOMMENT			MAKE_AVP_PID(VE_PID_VALUECOMMENT_ID,VE_APP_ID,avpt_str,0)

// Implementation comment PROP
#define VE_PID_IMPLCOMMENT_ID		(10004)
#define VE_PID_IMPLCOMMENT			MAKE_AVP_PID(VE_PID_IMPLCOMMENT_ID,VE_APP_ID,avpt_str,0)

// ---------------------------------------------------------------------------------------------------------
// Standard unique IDs props section

// Unique ID PROP
#define VE_PID_UNIQUEID_ID					(15000)
#define VE_PID_UNIQUEID							MAKE_AVP_PID(VE_PID_UNIQUEID_ID,VE_APP_ID,avpt_str,0)

// ---------------------------------------------------------------------------------------------------------
// Separator PROP
#define VE_PID_SEPARATOR_ID					(20000)
#define VE_PID_SEPARATOR						MAKE_AVP_PID(VE_PID_SEPARATOR_ID,VE_APP_ID,avpt_nothing,0)


// ---------------------------------------------------------------------------------------------------------
// Tree root properties

// Implementation prototype file name PROP
#define VE_PID_PROTOTYPEFILENAME_ID			(21000)
#define VE_PID_PROTOTYPEFILENAME				MAKE_AVP_PID(VE_PID_PROTOTYPEFILENAME_ID,VE_APP_ID,avpt_str,0)

// Implementation prototype relative file name PROP
#define VE_PID_PROTOTYPERELATIVEFILENAME_ID			(21001)
#define VE_PID_PROTOTYPERELATIVEFILENAME				MAKE_AVP_PID(VE_PID_PROTOTYPERELATIVEFILENAME_ID,VE_APP_ID,avpt_str,0)

// ---------------------------------------------------------------------------------------------------------
// Node type property

#define VE_NT_PLUGIN   					  (0 ) // Interface
#define VE_NT_INTERFACE					  (1 ) // Interface
#define VE_NT_PROPERTY						(2 ) // Interface Property
#define VE_NT_METHOD							(3 ) // Interface Method
#define VE_NT_METHODPARAM		      (4 ) // Interface Method Paremeter
#define VE_NT_DATASTRUCTURE				(5 ) // Interface Data Structure
#define VE_NT_DATASTRUCTUREMEMBER	(6 ) // Interface Data Structure Member
#define VE_NT_TYPE								(7 ) // Interface Type
#define VE_NT_CONSTANT						(8 ) // Interface Constant
#define VE_NT_SYSTEMMETHOD				(9 ) // Interface System Method
#define VE_NT_SYSTEMMETHODPARAM		(10) // Interface System Method  Paremeter
#define VE_NT_ERRORCODE						(11) // Interface error Code
#define VE_NT_MESSAGECLASS				(12) // Interface message class
#define VE_NT_MESSAGE						  (13) // Interface message


// Node type PROP
#define VE_PID_NODETYPE_ID					(22000)
#define VE_PID_NODETYPE							MAKE_AVP_PID(VE_PID_NODETYPE_ID,VE_APP_ID,avpt_dword,0)


// ---------------------------------------------------------------------------------------------------------
// Node type type property

#define VE_NTT_GLOBALTYPE			    (0 ) // Global type
#define VE_NTT_USERTYPE					  (1 ) // User type
#define VE_NTT_INTERFACE				  (2 ) // Interface


// Node type PROP
#define VE_PID_NODETYPETYPE_ID			(23000)
#define VE_PID_NODETYPETYPE					MAKE_AVP_PID(VE_PID_NODETYPETYPE_ID,VE_APP_ID,avpt_dword,0)


// ---------------------------------------------------------------------------------------------------------
// Prague predefined types tree section

#define TT_APP_ID (VE_APP_ID + 1)

// Types tree Root ID
#define TT_PID_ROOT_ID									(30001)
#define TT_PID_ROOT											MAKE_AVP_PID(TT_PID_ROOT_ID,TT_APP_ID, avpt_str, 1)

// Type ID PROP
#define TT_PID_TYPEID_ID								(1)
#define TT_PID_TYPEID										MAKE_AVP_PID(TT_PID_TYPEID_ID,TT_APP_ID,avpt_dword,1)

// Type DefineName PROP
#define TT_PID_TYPEDEFNAME_ID						(2)
#define TT_PID_TYPEDEFNAME							MAKE_AVP_PID(TT_PID_TYPEDEFNAME_ID,TT_APP_ID,avpt_str,1)

// Type DT type ID PROP
#define TT_PID_DTTYPEID_ID							(3)
#define TT_PID_DTTYPEID									MAKE_AVP_PID(TT_PID_DTTYPEID_ID,TT_APP_ID,avpt_dword,1)

// Type sizeof ID PROP
#define TT_PID_SIZEOF_ID							  (4)
#define TT_PID_SIZEOF									  MAKE_AVP_PID(TT_PID_SIZEOF_ID,TT_APP_ID,avpt_dword,1)



// Property types tree Root ID
#define PT_PID_ROOT_ID									(30002)
#define PT_PID_ROOT											MAKE_AVP_PID(PT_PID_ROOT_ID,TT_APP_ID, avpt_str, 1)

// Property type ID PROP
#define PT_PID_TYPEID_ID								(1)
#define PT_PID_TYPEID										MAKE_AVP_PID(PT_PID_TYPEID_ID,TT_APP_ID,avpt_dword,1)

// Property type DefineName PROP
#define PT_PID_TYPEDEFNAME_ID						(2)
#define PT_PID_TYPEDEFNAME							MAKE_AVP_PID(PT_PID_TYPEDEFNAME_ID,TT_APP_ID,avpt_str,1)

// Property type DT type ID PROP
#define PT_PID_DTTYPEID_ID							(3)
#define PT_PID_DTTYPEID									MAKE_AVP_PID(PT_PID_DTTYPEID_ID,TT_APP_ID,avpt_dword,1)

// Property sizeof ID PROP
#define PT_PID_SIZEOF_ID							  (4)
#define PT_PID_SIZEOF									  MAKE_AVP_PID(PT_PID_SIZEOF_ID,TT_APP_ID,avpt_dword,1)



// "Hard" properties tree Root ID
#define HP_PID_ROOT_ID									(30003)
#define HP_PID_ROOT											MAKE_AVP_PID(HP_PID_ROOT_ID,TT_APP_ID, avpt_str, 1)

// "Hard" property ID PROP
#define HP_PID_TYPEID_ID								(1)
#define HP_PID_TYPEID										MAKE_AVP_PID(HP_PID_TYPEID_ID,TT_APP_ID,avpt_dword,1)

// "Hard" property DefineName PROP
#define HP_PID_TYPEDEFNAME_ID						(2)
#define HP_PID_TYPEDEFNAME							MAKE_AVP_PID(HP_PID_TYPEDEFNAME_ID,TT_APP_ID,avpt_str,1)

// "Hard" property DT type ID PROP
#define HP_PID_DTTYPEID_ID							(3)
#define HP_PID_DTTYPEID									MAKE_AVP_PID(HP_PID_DTTYPEID_ID,TT_APP_ID,avpt_dword,1)

// "Hard" property should be shared PROP
#define HP_PID_SHOULDBESHARED_ID				(4)
#define HP_PID_SHOULDBESHARED						MAKE_AVP_PID(HP_PID_SHOULDBESHARED_ID,TT_APP_ID,avpt_bool,1)

// "Hard" property def value PROP
#define HP_PID_SHAREDDEFVALUE_ID				(5)
#define HP_PID_SHAREDDEFVALUE						MAKE_AVP_PID(HP_PID_SHAREDDEFVALUE_ID,TT_APP_ID,avpt_str,1)

// "Hard" property Short comment PROP
#define HP_PID_SHORTCOMMENT_ID					(6)
#define HP_PID_SHORTCOMMENT							MAKE_AVP_PID(HP_PID_SHORTCOMMENT_ID,TT_APP_ID,avpt_str,1)

// "Hard" property Large comment PROP
#define HP_PID_LARGECOMMENT_ID					(7)
#define HP_PID_LARGECOMMENT							MAKE_AVP_PID(HP_PID_LARGECOMMENT_ID,TT_APP_ID,avpt_str,1)

// "Hard" property Behavior descripton PROP
#define HP_PID_BEHAVIORCOMMENT_ID				(8)
#define HP_PID_BEHAVIORCOMMENT					MAKE_AVP_PID(HP_PID_BEHAVIORCOMMENT_ID,TT_APP_ID,avpt_str,1)


// "Soft" properties tree Root ID
#define SP_PID_ROOT_ID									(30004)
#define SP_PID_ROOT											MAKE_AVP_PID(SP_PID_ROOT_ID,TT_APP_ID, avpt_str, 1)

// "Soft" property ID PROP
#define SP_PID_TYPEID_ID								(1)
#define SP_PID_TYPEID										MAKE_AVP_PID(SP_PID_TYPEID_ID,TT_APP_ID,avpt_dword,1)

// "Soft" property DefineName PROP
#define SP_PID_TYPEDEFNAME_ID						(2)
#define SP_PID_TYPEDEFNAME							MAKE_AVP_PID(SP_PID_TYPEDEFNAME_ID,TT_APP_ID,avpt_str,1)

// "Soft" property DT type ID PROP
#define SP_PID_DTTYPEID_ID							(3)
#define SP_PID_DTTYPEID									MAKE_AVP_PID(SP_PID_DTTYPEID_ID,TT_APP_ID,avpt_dword,1)

// "Soft" property should be shared PROP
#define SP_PID_SHOULDBESHARED_ID				(4)
#define SP_PID_SHOULDBESHARED						MAKE_AVP_PID(SP_PID_SHOULDBESHARED_ID,TT_APP_ID,avpt_bool,1)

// "Sort" property def value PROP
#define SP_PID_SHAREDDEFVALUE_ID				(5)
#define SP_PID_SHAREDDEFVALUE						MAKE_AVP_PID(SP_PID_SHAREDDEFVALUE_ID,TT_APP_ID,avpt_str,1)

// "Soft" property Short comment PROP
#define SP_PID_SHORTCOMMENT_ID					(6)
#define SP_PID_SHORTCOMMENT							MAKE_AVP_PID(SP_PID_SHORTCOMMENT_ID,TT_APP_ID,avpt_str,1)

// "Soft" property Large comment PROP
#define SP_PID_LARGECOMMENT_ID					(7)
#define SP_PID_LARGECOMMENT							MAKE_AVP_PID(SP_PID_LARGECOMMENT_ID,TT_APP_ID,avpt_str,1)

// "Soft" property Behavior descripton PROP
#define SP_PID_BEHAVIORCOMMENT_ID				(8)
#define SP_PID_BEHAVIORCOMMENT					MAKE_AVP_PID(SP_PID_BEHAVIORCOMMENT_ID,TT_APP_ID,avpt_str,1)


// Type contants tree Root ID
#define TC_PID_ROOT_ID									(30005)
#define TC_PID_ROOT											MAKE_AVP_PID(TC_PID_ROOT_ID,TT_APP_ID, avpt_nothing, 0)

// Type ID PROP	
#define TC_PID_TYPEID(a)								MAKE_AVP_PID(a,TT_APP_ID,avpt_dword,0)

// Constant name PROP	of TC_PID_TYPEID
#define TC_PID_CONSTANTNAME_ID					(1001)
#define TC_PID_CONSTANTNAME							MAKE_AVP_PID(TC_PID_CONSTANTNAME_ID,TT_APP_ID,avpt_str,1)

// Constant value PROP	of TC_PID_TYPEID
#define TC_PID_CONSTANTVALUE_ID					(1002)
#define TC_PID_CONSTANTVALUE						MAKE_AVP_PID(TC_PID_CONSTANTVALUE_ID,TT_APP_ID,avpt_qword,1)

// ---------------------------------------------------------------------------------------------------------


// System PROP
#define VE_PID_NODECLIPTYPE_ID					(GET_AVP_PID_ID_MAX_VALUE - 1)
#define VE_PID_NODECLIPTYPE							MAKE_AVP_PID(VE_PID_NODECLIPTYPE_ID,VE_APP_ID,avpt_dword,0)

#define VE_PID_NODEEXPANDED_ID					(GET_AVP_PID_ID_MAX_VALUE - 2)
#define VE_PID_NODEEXPANDED							MAKE_AVP_PID(VE_PID_NODEEXPANDED_ID,VE_APP_ID,avpt_bool,0)

// Code generator options tree root
#define VE_PID_CGIOPTIONSROOT_ID				(1)
#define VE_PID_CGIOPTIONSROOT						MAKE_AVP_PID(VE_PID_CGIOPTIONSROOT_ID,VE_APP_ID,avpt_dword,0)

// In single file flag PROP 
#define VE_PID_CGIOINSINGLEFILE_ID			(1)
#define VE_PID_CGIOINSINGLEFILE					MAKE_AVP_PID(VE_PID_CGIOINSINGLEFILE_ID,VE_APP_ID,avpt_bool,0)

// Generator CLSID PROP
#define VE_PID_CGIOGENERATORCLSID_ID		(2)
#define VE_PID_CGIOGENERATORCLSID				MAKE_AVP_PID(VE_PID_CGIOGENERATORCLSID_ID,VE_APP_ID,avpt_bin,0)

// Code generator project info root
#define VE_PID_CGPI_ROOT_ID							(1)
#define VE_PID_CGPI_ROOT								MAKE_AVP_PID(VE_PID_CGPI_ROOT_ID,VE_APP_ID,avpt_nothing,0)

// Source file directory PROP
#define VE_PID_CGPI_PRIVATEDIR_ID				(5)
#define VE_PID_CGPI_PRIVATEDIR					MAKE_AVP_PID(VE_PID_CGPI_PRIVATEDIR_ID,VE_APP_ID,avpt_str,0)


// Include  file directory PROP
#define VE_PID_CGPI_PUBLICDIR_ID				(4)
#define VE_PID_CGPI_PUBLICDIR						MAKE_AVP_PID(VE_PID_CGPI_PUBLICDIR_ID,VE_APP_ID,avpt_str,0)

// --------------------------------------------------------------------------------------------------------
// method class identifier
#define VE_PID_PL_IMPEX_METHOD_CLASS_ID (1)
#define VE_PID_PL_IMPEX_METHOD_CLASS    MAKE_AVP_PID(VE_PID_PL_IMPEX_METHOD_CLASS_ID,VE_APP_ID,avpt_dword,0)

// method identifier
#define VE_PID_PL_IMPEX_METHOD_FUNC_ID  (2)
#define VE_PID_PL_IMPEX_METHOD_FUNC     MAKE_AVP_PID(VE_PID_PL_IMPEX_METHOD_FUNC_ID,VE_APP_ID,avpt_dword,0)

// method symbol name
#define VE_PID_PL_IMPEX_METHOD_NAME_ID  (3)
#define VE_PID_PL_IMPEX_METHOD_NAME     MAKE_AVP_PID(VE_PID_PL_IMPEX_METHOD_NAME_ID,VE_APP_ID,avpt_str,0)

// method module name
#define VE_PID_PL_IMPEX_METHOD_MODULE_NAME_ID (4)
#define VE_PID_PL_IMPEX_METHOD_MODULE_NAME    MAKE_AVP_PID(VE_PID_PL_IMPEX_METHOD_MODULE_NAME_ID,VE_APP_ID,avpt_str,0)

#define VE_PID_NATIVE_ID						(3000)
#define VE_PID_NATIVE								MAKE_AVP_PID(VE_PID_NATIVE_ID,VE_APP_ID,avpt_bool,0)


#endif
