////////////////////////////////////////////////////////////////////
//
//  BASEWORK.H
//  CBaseWork Main class definitions
//  AVP 3.0 Engine
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __BASEWORK_H
#define __BASEWORK_H

#include <ScanAPI/Scanobj.h>
#include <Bases/Format/Records.h>
#include <pagememorymanager/pagememorymanager.h>
#include "Workarea.h"
#include "Clink.h"

#if defined (BASE_EMULATOR)
 #pragma pack (8)
#elif defined(__unix__)
 #pragma pack (1)
#endif

class CBaseWork;

// quick - start and end loop
typedef struct _tSybtype_range
{
	DWORD idx;
#ifdef _DEBUG
	DWORD nash_region_size;
#endif
	BYTE* pSubQueue;
}Sybtype_range;

class CBase
{
protected:
        char*   p_Names;
        UINT    n_Names;
		CBaseWork*  pBW;
		BOOL valid;
public:
        _TCHAR*		BaseName;
        CLink*      LinkTable[8];
        void*		RPtrTable[8];
        int         RNumTable[8];

        CBase();        // standard constructor
        ~CBase();

		BOOL IsValid(){ return valid;}

        int LoadBase(_TCHAR* baseName, CBaseWork* bw);

//        GetNameCount(){return n_Names;};
#ifdef _MAKE2BYTES
        void    Write2b();
#endif

		void*  DoRecords(WORD Type,CWorkArea* wa, R_File_CureData** curedata);

// !Sobko
		BOOL PrepareFileRecords();
		UINT GetSubtypeBitNo(BYTE SubType);
		Sybtype_range gsub_type[8+1]; // 8 possible SubTypes + 1 for SubType=0 or invalid (mixed) SubType
		R_File_Short* pFileShort;
// !Sobko end

		R_File_CureData* GetCureData(WORD index);
		size_t CureDataSize;
		R_File_CureData* CureData;
		WORD* CureDataIndex;
};

#define HT_MASK 0x0FFF

class CBaseWork
{

protected:
        BOOL Analyze();
        CPtrArray* BaseArray;
		BOOL valid;
public:
		BOOL IsValid(){ return valid;}
        CPtrArray* ExcludeArray;
        CPtrArray* MaskArray;
	int CheckWildcards(_TCHAR* name, CPtrArray* array);
        BOOL CheckFalseAlarm();
        CPtrArray** HT;
        BOOL  CreateRedundantHash();
        void  FreeRedundantHash();
        BOOL  ProcessRedundant(AVPScanObject* ScanObject);
        BOOL  NextVolume();
        BOOL  CheckMemory(AVPScanObject* ScanObject);
        void* DoRecords(WORD Type, R_File_CureData** curedata = NULL);
        int   ProcessArchive(DWORD offset);
        BOOL  ProcessPacked(AVPScanObject* ScanObject, R_Unpack* rp, CLink* link);
        int   AddMask(_TCHAR* ch,CPtrArray* array);
        int   CheckMask(_TCHAR* fullname);
		DWORD ErrorFlush();
        _TCHAR SetName[AVP_MAX_PATH];
		PageMemoryManagerNS::PageMemoryManager* m_pPageMemoryManager;
        CPtrArray   Allocations;

#if !defined (BASE_EMULATOR)
        DWORD _DoStdJump;
        DWORD _IsProgram;
        DWORD _GetFirstEntry;
        DWORD _GetNextEntry;
        DWORD _CureStdFile;
        DWORD _CureStdSector;
        DWORD _GetArchOffs;
		CHAR* _Psw;
		DWORD* _gr01;
		DWORD* pEmulFlags;
		DWORD _CleanUpEntry;

        CGlobalNames GlobalNames;
#endif /* !defined BASE_EMULATOR */

        CWorkArea* WorkArea;
//      AVPScanObject* ScanObject;->in WorkArea.


        CBaseWork();    // standard constructor
        ~CBaseWork();

        LONG LoadBase(_TCHAR* name, int format);
        LONG UnloadBase(_TCHAR* name, int format);
        LONG SetFlags(LPARAM lParam);
        LONG ProcessObject(AVPScanObject* ScanObject);
        LONG Dispatch(AVPScanObject* ScanObject, BYTE SubType);
	_TCHAR* MakeFullName(_TCHAR* dest,const _TCHAR* src);

        int  CureFile(R_File_Cutted* rp, R_File_CureData* curedata);
        int  CureSector(R_Sector* rp);
        DWORD DoJump(BYTE* Page);

};

#define CACHE_SIZE 257 //must be > 256 !!!

struct BData
{
	BYTE* DosMemory;
	DWORD*  Trace_Lines;
	DWORD*  Hook_Lines;
	WORD*   MCB_Lines;
	DWORD Num_Hook_Lines;
	DWORD Num_Trace_Lines;
	DWORD Num_MCB_Lines;
	WORD DOS_Mem;
	WORD BIOS_Mem;
	WORD Z_MCB;

	_TCHAR TempPath[512];

	DWORD CacheA[CACHE_SIZE];
	DWORD CacheB[CACHE_SIZE];
	DWORD CacheH[CACHE_SIZE];

	CBase* BasePtr;
    LONG CancelProcess;
	BOOL DiskFull;

	__int64 BombSize;
	BYTE  BP;
	DWORD BPCount;
	BYTE  xbp;
	DWORD ibp;
	CPtrArray* AllocRegistry;
	HANDLE MainObjectHandle;
	int WarLen;
	DWORD   ArchHeaderSum;
};

#ifdef USE_STATIC_BDATA
extern BData* bdata;
#define DupSetCustomDWord(x,y) bdata=(BData*)y
#else
#define bdata ((BData*)DupGetCustomDWord(0))
#endif

#if defined (BASE_EMULATOR) || defined(__unix__)
#pragma pack()
#endif

#endif//__BASEWORK_H
