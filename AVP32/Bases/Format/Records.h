////////////////////////////////////////////////////////////////////
//
//  RECORDS.H
//  AVP 3.0 database records definition
//  AVP 3.0 Engine
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1996
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __RECORDS_H
#define __RECORDS_H

// Some fields has different meanings in the editor, file or memory.
#if !((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || defined(__unix__))
 #pragma pack (push, pack_records)
#endif
#pragma pack (1)

typedef struct _R_KERNEL{
        WORD    LinkIdx;                //      Idx             Idx
} R_Kernel;

typedef struct _R_JUMP{
        BYTE    Len1;
        DWORD   BitMask1;               //if ==0 ->Link or ERROR
        DWORD   BitMask2;               //if ==0 don't process this half
        WORD    ControlWord;    //first 2 bytes (after use mask)
        DWORD   Sum1;                   //if (BitMask2) sum 64 bytes. else 32 bytes only.
        WORD    LinkIdx;                //      ---             Idx
        BYTE    Method;
        WORD    Data1;
        WORD    Data2;
        WORD    Data3;
} R_Jump;

typedef struct _R_FILE_SHORT
{
        BYTE    Len1;
        WORD    Off1;                   //                      If Len1==0  LinkIdx
        WORD    ControlWord;
} R_File_Short;

typedef struct _R_FILE_CUTTED
{
        BYTE    Type;
// dos xms part
        DWORD   Sum1;
        BYTE    Len2;
        WORD    Off2;
        DWORD   Sum2;
        WORD    LinkIdx;                //      ---             Idx
        DWORD   NameIdx;                //      ---             Idx             PTR?
//      cure part
        BYTE    CureMethod;             // 8th bit set means Page=File
} R_File_Cutted;

typedef struct _R_FILE_CUREDATA
{
	WORD    CureData1;
	WORD    CureData2;
	WORD    CureData3;
	WORD    CureData4;
	WORD    CureData5;
} R_File_CureData;

typedef struct _R_FILE
{
        BYTE    Len1;
        WORD    Off1;                   //                      If Len1==0  LinkIdx
        WORD    ControlWord;
        BYTE    Type;
// dos xms part
        DWORD   Sum1;
        BYTE    Len2;
        WORD    Off2;
        DWORD   Sum2;
        WORD    LinkIdx;                //      ---             Idx
        DWORD   NameIdx;                //      ---             Idx             PTR?
//      cure part
        BYTE    CureMethod;             // 8th bit set means Page=File
        WORD    CureData1;
        WORD    CureData2;
        WORD    CureData3;
        WORD    CureData4;
        WORD    CureData5;
} R_File;

typedef struct _R_SECTOR
{
        BYTE    Len1;
        WORD    Off1;                   //                      If Len1==0  LinkIdx
        WORD    ControlWord;
// dos xms part
        DWORD   Sum1;
        BYTE    Len2;
        WORD    Off2;
        DWORD   Sum2;
        WORD    LinkIdx;                //      ---             Idx
        DWORD   NameIdx;                //      ---             Idx             PTR?
//      cure part
        BYTE    CureObjAndMethod1;
        WORD    CureAddr11;
        WORD    CureAddr12;
        BYTE    CureData1;
        BYTE    CureObjAndMethod2;
        WORD    CureAddr21;
        WORD    CureAddr22;
        BYTE    CureData2;
} R_Sector;

typedef struct _R_MEMORY
{
        BYTE    Method;
        WORD    Offset;
        WORD    Segment;
        BYTE    Len1;
        DWORD   Sum1;
        BYTE    ControlByte;
        WORD    LinkIdx;                //      ---             Idx
        DWORD   NameIdx;                //      ---             Idx             PTR?
//      cure part
        WORD    CureOffset;
        BYTE    CureLength;
        BYTE    CureData[5];
} R_Memory;

typedef struct _R_CA
{
        BYTE    Len1;
        WORD    Off1;                   //                      If Len1==0  LinkIdx
        WORD    ControlWord;
        BYTE    Type;
// dos xms part
        DWORD   Sum1;
        BYTE    Len2;                   //                      If !=0 then false alarm.
        WORD    Off2;
        DWORD   Sum2;
        WORD    LinkIdx;                //      ---             Idx
        DWORD   NameIdx;                //      ---             Idx             PTR?
} R_CA;

typedef struct _R_UNPACK
{
        BYTE    Len1;
        WORD    Off1;                   //                      If Len1==0  LinkIdx
        WORD    ControlWord;
        BYTE    Type;
// dos xms part
        DWORD   Sum1;
        BYTE    Len2;
        WORD    Off2;
        DWORD   Sum2;
        WORD    LinkIdx;                //      ---             Idx
        DWORD   NameIdx;                //      ---             Idx             PTR?
} R_Unpack;

typedef struct _R_EXTRACT
{
        BYTE    Len1;
        WORD    Off1;                   //                      If Len1==0  LinkIdx
        WORD    ControlWord;
        BYTE    Type;
// dos xms part
        DWORD   Sum1;
        BYTE    Len2;
        WORD    Off2;
        DWORD   Sum2;
        WORD    LinkIdx;                //      ---             Idx
        DWORD   NameIdx;                //      ---             Idx             PTR?
} R_Extract;

typedef struct _R_EDIT
{
        WORD    Type;           // RT_*    (see base.h)
        DWORD   NameIdx;                //              Idx
        DWORD   CommentIdx;             //              Idx
        DWORD   RecordIdx;              //                      Idx
        DWORD   Link16Idx;              //                      Idx
        DWORD   Link32Idx;              //                      Idx


} R_Edit;


#if !((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || defined(__unix__))
 #pragma pack (pop, pack_records)
#endif
#pragma pack()

#endif//__RECORDS_H
