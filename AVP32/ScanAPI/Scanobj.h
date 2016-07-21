////////////////////////////////////////////////////////////////////
//
//  SCANOBJ.H
//  AVPScanObject structure (for scan request) definition
//  AVP 3.0 Engine
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1996
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __SCANOBJ_H
#define __SCANOBJ_H


#include <ScanAPI/defines.h>
#include <ScanAPI/Types.h>
#include <ScanAPI/RetFlags.h>

#if !((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__)))||defined(__unix__))
 #pragma pack (push, pack_so)
#endif

#ifdef __unix__
#undef  AVP_SCANOBJ_USE_NAMED_UNION
#define AVP_SCANOBJ_USE_NAMED_UNION
#endif

#pragma pack (1)

typedef struct AVPScanObject{
	union{
		DWORD	Type;		//  ->
		struct{
			BYTE Disk;		
			BYTE Drive;		
			WORD SType;		//  -> OT_XXXXX
#ifdef  AVP_SCANOBJ_USE_NAMED_UNION
		}SO1;
#else
		};
#endif
	};
	union{
		DWORD	MFlags;		//  -> MF_XXX
		struct{
			WORD Mode;		//  -> 0- prog Fmt; 1- prog Ext; 2- All files; 3- User;
			WORD Flags;		//  -> MF_XXX
#ifdef  AVP_SCANOBJ_USE_NAMED_UNION
		}SO2;
#else
		};
#endif
		
	};
	DWORD	RFlags;			// <-

	char	Name[AVP_MAX_PATH];	//  ->
	char*	VirusName;		// <-
	char*	SuspicionName;	// <-
	char*	WarningName;	// <-
	DWORD	ArchHeaderSum;
	DWORD	Entry;			// <-  Number of entry (for callback codes)
	DWORD	Size;			//  -> Size of object
	DWORD	RefData;		// <->
	DWORD	NativeName;		// <->
	HANDLE	Handle;			//  -> For packers
	WORD	InPack;		    // <-
	WORD	InArc;		    // <- 
	BYTE	NameSpace;		//  -> 0-DOS 1-NW 2-MAC
	DWORD	Connection;		//  -> 
	DWORD	Attrib;

}AVPScanObject;


#define MF_MASK_              0x0000000F
#define MF_MASK_SMART         0x00000000
#define MF_MASK_PROG          0x00000001
#define MF_MASK_ALL           0x00000002
#define MF_MASK_USER          0x00000003

#define MF_MASK_EXCLUDE       0x00000010
#define MF_ARC_STOP_IF_DETECT 0x00000020

#define MF_DBG_NO_SCAN        0x00000100
#define MF_SCAN_STREAMS       0x00000200
#define MF_SFX_ARCHIVED       0x00000400
#define MF_FPI_MODE           0x00000800
  
#define MF_PACKED             0x00010000
#define MF_ARCHIVED           0x00020000 //means Sfx_archives and normal archives (for compatibility)
#define MF_CA                 0x00040000
#define MF_REDUNDANT          0x00080000
  
#define MF_WARNINGS           0x00100000
#define MF_NOXMS              0x00200000
#define MF_MAILBASES          0x00400000
#define MF_MAILPLAIN          0x00800000
  
#define MF_LINK_FRAME         0x01000000
#define MF_RESEVED            0x02000000
#define MF_DELETE_ARCHIVES    0x04000000
#define MF_SKIP_EMBEDDED      0x08000000
  
#define MF_ALLENTRY           0x10000000
#define MF_ALLWARNINGS        0x20000000
#define MF_CALLBACK_SAVE      0x40000000  //For editor only
#define MF_APC_CURE_REQUEST   0x80000000  //For APC cure request
  
#define MF_D_                 0x0000F000
#define MF_D_DENYACCESS       0x00001000
#define MF_D_CUREALL          0x00002000
#define MF_D_DELETEALL        0x00003000
#define MF_D_ASKCURE          0x00004000
#define MF_D_ASKCUREDEBUG     0x00005000
                   

#pragma pack()
  
#if !((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__)))||defined(__unix__))
 #pragma pack (pop, pack_so)
#endif

#endif//__SCANOBJ_H
