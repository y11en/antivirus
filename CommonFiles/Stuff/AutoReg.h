////////////////////////////////////////////////////////////////////
//
//  AutoReg.h
//  AutoRegisterOCX function declaration
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#ifndef __AUTOREG_H__
#define __AUTOREG_H__


#include <windows.h>

// Standard AVP OCX's definitions

#define AVP_RVC_CLSID "{DD54D567-2FCA-11D3-96B0-00104B5B66AA}"
#define AVP_RVC_FILE  "AvpRVC.ocx"

#define AVP_CTT_CLSID "{D2B34AE8-B9CF-11D2-96B0-00104B5B66AA}"
#define AVP_CTT_FILE  "AvpCTT.ocx"

#define AVP_WAS_CLSID "{616BFB46-BAD6-11D2-96B0-00104B5B66AA}"
#define AVP_WAS_FILE  "AvpWAS.ocx"

#define AVP_NSL_CLSID "{2EA228FC-BA8D-11D2-96B0-00104B5B66AA}"
#define AVP_NSL_FILE  "AvpNSL.ocx"

#define AVP_NST_CLSID "{2EA228E6-BA8D-11D2-96B0-00104B5B66AA}"
#define AVP_NST_FILE  "AvpNST.ocx"

#define AVP_SVC_CLSID "{55C79F86-65DD-11D3-96B0-00104B5B66AA}"
#define AVP_SVC_FILE  "AvpSVC.ocx"

#define AVP_SPL_CLSID "{D482802A-8152-11D3-96B0-00104B5B66AA}"
#define AVP_SPL_FILE  "AvpSPL.ocx"

#define AVP_CBR_CLSID "{F2EA5D86-92C3-11D3-96B0-00104B5B66AA}"
#define AVP_CBR_FILE  "AvpCBR.ocx"

#define AVP_ABT_CLSID "{8C397FA8-6CE7-11D3-96B0-00104B5B66AA}"
#define AVP_ABT_FILE  "AvpABT.ocx"


// Check and register given OCX
// pBasePath		- pointer to path to folder containing OCX is being registered
//								NULL - causes function to search for OCX in folder registered as "AVP Shared Files"
// pCLSIDStr		- pointer to CLSID of OCX is being registered
// pOCXFileName - pointer to file name of OCX is being registered
// bLeaveLoaded - whether given OCX should be keepped in memory after registration
BOOL AutoRegisterOCX( const TCHAR *pBasePath, const TCHAR *pCLSIDStr, const TCHAR *pOCXFileName, BOOL bLeaveLoaded );


#endif //__AUTOREG_H__
