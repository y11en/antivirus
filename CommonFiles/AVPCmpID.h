////////////////////////////////////////////////////////////////////
//
//  AVPCmpID.h
//  Declares the entry point for the AvpCompnentInfo DLL application
//  AVP Component info collecting DLL
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#ifndef __AVPCMPID_H_
#define __AVPCMPID_H_

#include <windows.h>
#include <Property/Property.h>
#include <AVPComID.h>

#ifndef __unix__
	#define  CMPINFO_PARAM 
#else
	#define  CMPINFO_PARAM  __cdecl
#endif


#ifdef COMPINFO_EXP
	#ifdef __cplusplus
		#define CMPINFO_PROC extern "C" __declspec(dllexport)
	#else
		#define CMPINFO_PROC __declspec(dllexport)
	#endif
#else
	#ifdef __cplusplus
		#define CMPINFO_PROC extern "C" __declspec(dllimport)
	#else
		#define CMPINFO_PROC __declspec(dllimport)
	#endif
#endif

// Collect component About info into HDATA
// dwComponentType	- standard AVP component type (AVPRegID.h)
// phInfoData				- pointer to HDATA to be created 
//                    In case of Zero value - new data tree will be inserted into it
//           					In case of NON Zero value - new data tree will be attached	to it
// Return value     - TRUE - in case of success, FALSE - in case of any error occurs
CMPINFO_PROC BOOL CMPINFO_PARAM  CollectAboutInfoH( DWORD dwComponentType, HDATA *phInfoData );

// Collect and serialize component About info 
// dwComponentType	- standard AVP component type (AVPRegID.h)
// pvInfoBuffer			- pointer to pointer to buffer for serialized data
// pdwSize					- pointer to result data size	value
// Return value     - TRUE - in case of success, FALSE - in case of any error occurs
CMPINFO_PROC BOOL CMPINFO_PARAM  CollectAboutInfoS( DWORD dwComponentType, BYTE **pvInfoBuffer, DWORD *pdwSize );

// Typedefs for dynamic DLL loading
typedef BOOL (*COLLECT_ABOUT_INFO_H)( DWORD dwComponentType, HDATA *phInfoData );
typedef BOOL (*COLLECT_ABOUT_INFO_S)( DWORD dwComponentType, BYTE **pvInfoBuffer, DWORD *pdwSize );


/*
	Structure of data tree

	---- AB_PID_ROOT
	  |
		+---- Found key	data tree
		|
		+---- Found key	dada tree
		|
		...

	Description of structure of key data tree see CommonFiles\Structures.doc
*/

//#define AB_APP_ID (0x5)	Placed to AVPComID.h

// About info root DATA
#define AB_PID_ROOT_ID						(10000)
#define AB_PID_ROOT								MAKE_AVP_PID(AB_PID_ROOT_ID,AB_APP_ID,avpt_nothing,0)

// Application ID PROP
#define AB_PID_APPID_ID           (1)
#define AB_PID_APPID							MAKE_AVP_PID(AB_PID_APPID_ID,AB_APP_ID,avpt_dword,0)

// Application Name PROP
#define AB_PID_APPNAME_ID         (2)
#define AB_PID_APPNAME						MAKE_AVP_PID(AB_PID_APPNAME_ID,AB_APP_ID,avpt_str,0)

// Application Version PROP
#define AB_PID_APPVERSION_ID      (3)
#define AB_PID_APPVERSION					MAKE_AVP_PID(AB_PID_APPVERSION_ID,AB_APP_ID,avpt_str,0)

// Application Registered Name PROP
#define AB_PID_REGNAME_ID					(4)
#define AB_PID_REGNAME						MAKE_AVP_PID(AB_PID_REGNAME_ID,AB_APP_ID,avpt_str,0)

// Application Registered Organization PROP
#define AB_PID_REGCOMPANY_ID		  (5)
#define AB_PID_REGCOMPANY			    MAKE_AVP_PID(AB_PID_REGCOMPANY_ID,AB_APP_ID,avpt_str,0)

// Last update PROP
#define AB_PID_LASTUPDATE_ID        (6)
#define AB_PID_LASTUPDATE						MAKE_AVP_PID(AB_PID_LASTUPDATE_ID,AB_APP_ID,avpt_date,0)

// Viruses PROP
#define AB_PID_VIRUSES_ID						(7)
#define AB_PID_VIRUSES							MAKE_AVP_PID(AB_PID_VIRUSES_ID,AB_APP_ID,avpt_dword,0)

// Application Copyright1 PROP
#define AB_PID_COPYRIGHT1_ID				(8)
#define AB_PID_COPYRIGHT1						MAKE_AVP_PID(AB_PID_COPYRIGHT1_ID,AB_APP_ID,avpt_str,0)

// Application Copyright2 PROP
#define AB_PID_COPYRIGHT2_ID				(9)
#define AB_PID_COPYRIGHT2						MAKE_AVP_PID(AB_PID_COPYRIGHT2_ID,AB_APP_ID,avpt_str,0)

// Application About Picture PROP
#define AB_PID_APPABOUTBMP_ID       (11)
#define AB_PID_APPABOUTBMP					MAKE_AVP_PID(AB_PID_APPABOUTBMP_ID,AB_APP_ID,avpt_bin,1)

// Application Splash Picture PROP
#define AB_PID_APPSPLASHBMP_ID      (12)
#define AB_PID_APPSPLASHBMP					MAKE_AVP_PID(AB_PID_APPSPLASHBMP_ID,AB_APP_ID,avpt_bin,1)

// Application Support Url PROP
#define AB_PID_SUPPORTURL_ID        (13)
#define AB_PID_SUPPORTURL					  MAKE_AVP_PID(AB_PID_SUPPORTURL_ID,AB_APP_ID,avpt_str,0)

// Application Modules PROP
#define AB_PID_MODULES_ID						(14)
#define AB_PID_MODULES							MAKE_AVP_PID(AB_PID_MODULES_ID,AB_APP_ID,avpt_str,1)

// Application Module Versions PROP
#define AB_PID_MODULEVERSIONS_ID		(15)
#define AB_PID_MODULEVERSIONS				MAKE_AVP_PID(AB_PID_MODULEVERSIONS_ID,AB_APP_ID,avpt_str,1)

// Application Module Dates PROP
#define AB_PID_MODULEDATES_ID				(16)
#define AB_PID_MODULEDATES					MAKE_AVP_PID(AB_PID_MODULEDATES_ID,AB_APP_ID,avpt_datetime,1)

// Application IDs PROP
#define AB_PID_APPIDS_ID						(17)
#define AB_PID_APPIDS								MAKE_AVP_PID(AB_PID_APPIDS_ID,AB_APP_ID,avpt_dword,1)


// Key file name PROP	(added to each key structure tree root)
#define AB_PID_KEYFILENAME_ID				(1)
#define AB_PID_KEYFILENAME					MAKE_AVP_PID(AB_PID_KEYFILENAME_ID,AB_APP_ID,avpt_str,0)




#endif