////////////////////////////////////////////////////////////////////
//
//  WORKAREA.H
//  CWorkArea class definition
//  AVP 3.0 Engine
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1996
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __WORKAREA_H
#define __WORKAREA_H

#include <ScanAPI/defines.h>
#include <Bases/SIZES.h>
#if defined (BASE_EMULATOR) || defined (__unix__)
#pragma pack(8)
#else
# if !((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))))
#pragma pack(push,pack_wa)
# endif
#pragma pack(1)
#endif /* defined BASE_EMULATOR */

struct AVPScanObject;

typedef struct _CWorkArea{
        BYTE    Header[HEADER_SIZE];
        BYTE    Page_A[PAGE_A_SIZE];
        BYTE    Page_B[PAGE_B_SIZE];
        BYTE    Page_E[PAGE_E_SIZE];
        BYTE    Page_C[PAGE_C_SIZE];
        union{
                DWORD   Type;
                struct{
                        BYTE Disk;
                        BYTE Drive;
                        WORD SType;
#ifdef __unix__
                }WAs;
#else
                };
#endif
        };
        BYTE    SubType;
        char    FullName[AVP_MAX_PATH];
        char*   Name;
        char*   Ext;
                HANDLE  Handle;
        DWORD   Length;
        DWORD   RFlags;

                AVPScanObject* ScanObject;
        void*   AreaOwner;

                int             P_N;
                HANDLE  P_Handle;
                HANDLE  V_Handle;
                HANDLE  S_Handle;
//              CPtrArray* S_Array;
//                void*   S_Array;
                WORD    WarLen;
        char    ZipName[AVP_MAX_PATH];
                WORD    Disk_Max_CX;
                BYTE    Disk_Max_DH;
                WORD    AFlags;
				BYTE	AttrChanged;
				VOID*   CurRecord;
				DWORD   CurRecordType;
				DWORD   CurRecordStatus;
				VOID*   CurRecordLinkObj;
				DWORD   CurRecordLinkObjSize;
                DWORD   P_Position;
}CWorkArea;

#if defined (BASE_EMULATOR) || defined (__unix__)
#pragma pack()
#else
#pragma pack()
# if !((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))))
#pragma pack(pop,pack_wa)
# endif
#endif /* defined BASE_EMULATOR */

#endif//__WORKAREA_H

