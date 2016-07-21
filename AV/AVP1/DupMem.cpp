#if defined (_WIN32)
//Implementation that allows multipy instances of DupMem
#define _WIN32_WINNT 0x0501

#include <windows.h>

#include "dupmem.h"

#include <Prague/prague.h>
#include <Prague/iface\i_root.h>

#include <hook/avpgcom.h>

#include <stuff/dprintf.h>

#define MAX_THREAD_NUMBER     0x400
#define RESERVE_LINK_SIZE     0x60000 // 512K is it enough? usually takes ~ 90k
#define RESERVE_STATIC_SIZE   0x20000 // 256K usually takes ~ 50k
#define MIN_NUMBER_OF_SHADOWS 4
#define NUM_CUSTOM_DWORDS 3

#ifdef _WIN32
#ifdef _DEBUG
#define DEBUG_INT_3 __asm int 3;
#else
#define DEBUG_INT_3
#endif
#endif

#undef  dprintf

static int dprintf(const char* msg, ...)
{
	char s[512];
	int ret=0;
	va_list ap;
	va_start(ap,msg);
	strcpy_s(s, countof(s), "DupMem\t");
	ret=wvsprintf(s+7, msg, ap);
	va_end(ap);

	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, s));
	return ret;
}

//***************************************

struct DUP_THREAD_DATA
{
	DWORD      ThreadId;
	DWORD      LeaveTime;
	ULONG_PTR* MemPages;
};

struct DUP_DATA
{
	BOOL       MemManager;
	DWORD      Instance;
	long       QueueCounter;
	long       MemShareCounter;
	unsigned   RegionSize;
	char*      Region;
	unsigned   RegionLimit;
	int        RegionLock;
	int        RegionPages;
	char*      RegionBase;
	DWORD*     CustomData;
	CRITICAL_SECTION SemCS;
	DUP_THREAD_DATA* CurData;
	DUP_THREAD_DATA* LastData;
	DUP_THREAD_DATA  DataCache[MAX_THREAD_NUMBER];
};

struct DUP_MEM
{
	DUP_MEM()
	{
		InitializeCriticalSection(&InitCS);
		TlsDupDataIndex=TlsAlloc();
		InstanceCounter=0;
		SystemPageSize=0;
	}

	~DUP_MEM()
	{
		if(TlsDupDataIndex != 0xFFFFFFFF)
			TlsFree(TlsDupDataIndex);

		DeleteCriticalSection(&InitCS);
	}

	BOOL InitMemManagment()
	{
		SYSTEM_INFO sSysInfo;
		GetSystemInfo(&sSysInfo);
		dprintf("GetSystemInfo return pagesize %d.\n",sSysInfo.dwPageSize);
		SystemPageSize = sSysInfo.dwPageSize;

		HMODULE hMod = GetModuleHandle("kernel32.dll");
		if(!hMod)
			return FALSE;

		*(void**)&fnMapUserPhysicalPages=GetProcAddress(hMod,"MapUserPhysicalPages");
		*(void**)&fnAllocateUserPhysicalPages=GetProcAddress(hMod,"AllocateUserPhysicalPages");
		*(void**)&fnFreeUserPhysicalPages=GetProcAddress(hMod,"FreeUserPhysicalPages");

		if( !fnMapUserPhysicalPages||
			!fnAllocateUserPhysicalPages||
			!fnFreeUserPhysicalPages)
			return FALSE;

		HANDLE hToken = NULL;
		BOOL Result = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);

		struct {DWORD Count; LUID_AND_ATTRIBUTES Privilege [1];} Info;
		if(Result)
		{
			Info.Count = 1;
			Info.Privilege[0].Attributes = SE_PRIVILEGE_ENABLED;
			Result = LookupPrivilegeValue(NULL, SE_LOCK_MEMORY_NAME, &(Info.Privilege[0].Luid));
		}

		if(Result)
			Result=AdjustTokenPrivileges(hToken, FALSE, (PTOKEN_PRIVILEGES) &Info, 0, NULL, NULL);
		if(Result)
			Result=GetLastError()==0;

		if(hToken)
			CloseHandle(hToken);
		return Result;
	}

	BOOL InitMemPages(DUP_DATA* ddata)
	{
		ULONG_PTR*& MemPages=ddata->CurData->MemPages;

		if(!ddata->MemManager)
		{
			if(!MemPages)
				MemPages=(ULONG_PTR*)malloc(ddata->RegionLimit);
			return !!MemPages;
		}

		ULONG_PTR NumberOfPages=ddata->RegionPages;

		BOOL error = FALSE;
		if(!MemPages)
		{
			MemPages=(ULONG_PTR*)malloc(NumberOfPages*sizeof(ULONG_PTR));
			if(!MemPages)
				return FALSE;

			HANDLE hToken = NULL;
			OpenThreadToken(GetCurrentThread(), TOKEN_ALL_ACCESS, TRUE, &hToken);
			if(hToken)
				RevertToSelf();

			if(!fnAllocateUserPhysicalPages(GetCurrentProcess(), &NumberOfPages, MemPages))
			{
				DEBUG_INT_3
				dprintf("AllocateUserPhysicalPages return %d\n",GetLastError());
				error=TRUE;
			}
			else if( NumberOfPages != ddata->RegionPages )
			{
				fnFreeUserPhysicalPages(GetCurrentProcess(), &NumberOfPages, MemPages);
				dprintf("AllocateUserPhysicalPages returned not all pages\n");
				error=TRUE;
			}

			if(hToken)
			{
				SetThreadToken(NULL, hToken);
				CloseHandle(hToken);
			}

			if( error )
			{
				free(MemPages);
				MemPages=NULL;
			}
		}

//		fnMapUserPhysicalPages(ddata->Region, ddata->RegionPages, NULL);
		if(!error && !fnMapUserPhysicalPages(ddata->Region, NumberOfPages, MemPages))
		{
			DEBUG_INT_3
			dprintf("MapUserPhysicalPages return %d\n",GetLastError());
			error=TRUE;
		}

		return !error;
	}

	BOOL FreeMemPages(DUP_DATA* ddata, DUP_THREAD_DATA* pData)
	{
		if(!pData->MemPages)
			return FALSE;

		if(ddata->MemManager)
		{
			ULONG_PTR NumberOfPages=ddata->RegionPages;
			fnFreeUserPhysicalPages(GetCurrentProcess(), &NumberOfPages, pData->MemPages);
		}
		free(pData->MemPages);
		pData->MemPages=NULL;
		return TRUE;
	}

	BOOL InitDupData(DUP_DATA* ddata, DWORD multithread)
	{
		SetDupData(NULL);

		EnterCriticalSection(&InitCS);
		if(InstanceCounter++==0)
			MemManagerInited=InitMemManagment();
		LeaveCriticalSection(&InitCS);

		ddata->MemManager=MemManagerInited;
		ddata->Instance=InstanceCounter;
		ddata->RegionLimit=RESERVE_LINK_SIZE+RESERVE_STATIC_SIZE;
		if(SystemPageSize)
			ddata->RegionPages=ddata->RegionLimit/SystemPageSize;

		SetDupData(ddata);
		return InitDupRegion(ddata);
	}

	void DeinitDupData(DUP_DATA* ddata)
	{
		DeinitDupRegion(ddata);

		EnterCriticalSection(&InitCS);
		--InstanceCounter;
		LeaveCriticalSection(&InitCS);
	}

	BOOL InitDupRegion(DUP_DATA* ddata)
	{
		ddata->RegionSize=0;
		ddata->RegionLock=0;
		
		if(ddata->MemManager)
			ddata->Region=(char*)VirtualAlloc(NULL, ddata->RegionLimit,
				MEM_RESERVE|MEM_PHYSICAL, PAGE_READWRITE);
		else
			ddata->Region=(char*)malloc(ddata->RegionLimit);

		if( !InitMemPages(ddata) )
			return FALSE;

		if(!ddata->Region)
		{
			dprintf("DupAlloc: FSDrvTSPRegister failed\n");
			DEBUG_INT_3
			return FALSE;
		}
		if(!DupAlloc(sizeof(DWORD),0,0))
			return FALSE;

		*(DWORD*)ddata->Region=-1;
		return TRUE;
	}

	BOOL DeinitDupRegion(DUP_DATA* ddata)
	{
		BOOL res = TRUE;
		if(ddata->Region)
			if(ddata->MemManager)
				res = VirtualFree(ddata->Region, 0, MEM_RELEASE);
			else
				free(ddata->Region);

		if(ddata->RegionBase)
			free(ddata->RegionBase);

		for(int i = 0; i < MAX_THREAD_NUMBER; i++)
			FreeMemPages(ddata, &ddata->DataCache[i]);

		if(!res)
		{
			dprintf("DeinitDupRegion: failed\n");
			DEBUG_INT_3
		}
		return res;
	}

	BOOL LockBaseRegion(DUP_DATA* ddata)
	{
		if(!ddata->Region)
			return FALSE;

		if( ddata->RegionSize > ddata->RegionLimit )
			return FALSE;

		if(ddata->RegionLock)
			return TRUE;

		ddata->RegionBase=(char*)malloc(ddata->RegionSize);
		if(!ddata->RegionBase)
			return FALSE;

		memcpy(ddata->RegionBase,ddata->Region,ddata->RegionSize);
		ddata->RegionLock=1;

		dprintf("RegionLock %08X\n",ddata->Region);
		return TRUE;
	}

	BOOL EnterDupRegion(DUP_DATA* ddata, BOOL process)
	{
		if(!ddata->RegionLock)
		{
			dprintf("EnterDupRegion region not locked\n");
			return FALSE;
		}

		DWORD ThreadId=GetCurrentThreadId();
		DWORD ToFindId=process ? 0 : ThreadId;
		DUP_THREAD_DATA* CurData=ddata->CurData;

		if(!ddata->MemManager && CurData->ThreadId && CurData->ThreadId!=ThreadId )
			memcpy(CurData->MemPages,ddata->Region,ddata->RegionSize);

		for(int i = 0; i < MAX_THREAD_NUMBER && ddata->CurData->ThreadId!=ToFindId; i++)
			ddata->CurData=&ddata->DataCache[i];

		ddata->CurData->ThreadId=ThreadId;

		if(ddata->CurData!=CurData && !InitMemPages(ddata))
		{
			ddata->CurData->ThreadId=0;
			ddata->CurData = CurData; // вернём CurData на валидный пул страниц
			return FALSE;
		}

		if( process )
		{
			if(ddata->CurData > ddata->LastData)
				ddata->LastData=ddata->CurData;
			else if(!ddata->LastData->ThreadId &&
					ddata->LastData->LeaveTime + 10000 < GetTickCount())
				FreeMemPages(ddata, ddata->LastData--);

			memcpy(ddata->Region,ddata->RegionBase,ddata->RegionSize);
		}
		else if(!ddata->MemManager)
			memcpy(ddata->Region,ddata->CurData->MemPages,ddata->RegionSize);

		return TRUE;
	}

	BOOL LeaveDupRegion(DUP_DATA* ddata, BOOL process)
	{
		if(!ddata->RegionLock)
			return FALSE;

		if(process)
		{
			ddata->CurData->ThreadId=0;
			ddata->CurData->LeaveTime=GetTickCount();
		}
		return TRUE;
	}

	DUP_DATA* GetDupData()
	{
		return (DUP_DATA*)TlsGetValue(TlsDupDataIndex);
	}

	void SetDupData(DUP_DATA* ddata)
	{
		TlsSetValue(TlsDupDataIndex,ddata);
	}

	BOOL      MemManagerInited;
	unsigned  InstanceCounter;
	DWORD     TlsDupDataIndex;
	DWORD     SystemPageSize;
	CRITICAL_SECTION InitCS;

	BOOL (WINAPI* fnMapUserPhysicalPages)(PVOID lpAddress,ULONG_PTR NumberOfPages,PULONG_PTR UserPfnArray);
	BOOL (WINAPI* fnAllocateUserPhysicalPages)(HANDLE hProcess,PULONG_PTR NumberOfPages,PULONG_PTR UserPfnArray);
	BOOL (WINAPI* fnFreeUserPhysicalPages)(HANDLE hProcess,PULONG_PTR NumberOfPages,PULONG_PTR UserPfnArray);
} g_d;

//***************************************

BOOL DupGetDebugData( DWORD* MtDebug, DWORD* count)
{
	DUP_DATA* ddata=g_d.GetDupData();
	if(ddata && MtDebug && count && *count>=16)
	{
		MtDebug[0]=ddata->QueueCounter;
		MtDebug[1]=ddata->MemShareCounter;
		MtDebug[2]=ddata->RegionSize;
		*count=16;
		return TRUE;
	}
	return FALSE;
}

DWORD DupSetCustomDWord(unsigned index, DWORD data)
{
	DUP_DATA* ddata=g_d.GetDupData();
	if(ddata && ddata->CustomData && index<NUM_CUSTOM_DWORDS)
	{
		DWORD old_data=ddata->CustomData[index];
		ddata->CustomData[index]=data;
		return old_data;
	}
	return -1;
}

DWORD DupGetCustomDWord(unsigned index)
{
	DUP_DATA* ddata=g_d.GetDupData();
	if(ddata && ddata->CustomData && index<NUM_CUSTOM_DWORDS)
		return ddata->CustomData[index];
	return 0;
}

//***************************************

void* DupInit( BOOL multithread )
{
	DUP_DATA* ddata=(DUP_DATA*)malloc(sizeof(DUP_DATA));
	if( !ddata )
		return NULL;

	memset(ddata,0,sizeof(DUP_DATA));
	ddata->CurData=ddata->DataCache;
	InitializeCriticalSection(&ddata->SemCS);

	if(g_d.InitDupData(ddata, multithread))
		ddata->CustomData=(DWORD*)DupAlloc(NUM_CUSTOM_DWORDS*sizeof(DWORD),1,0);

	if(!ddata->CustomData)
	{
		DupDestroy(ddata);
		return NULL;
	}

	memset(ddata->CustomData,0,NUM_CUSTOM_DWORDS*sizeof(DWORD));
	return ddata;
}

void* DupDestroy(void* pdata)
{
	DUP_DATA* ddata=(DUP_DATA*)pdata;
	if( !ddata )
		return NULL;

	DeleteCriticalSection(&ddata->SemCS);

	if(ddata->CustomData)
		DupFree(ddata->CustomData);

	g_d.DeinitDupData(ddata);
	free(ddata);
	return NULL;
}

void DupRegisterThread(void* pdata)
{
	g_d.SetDupData((DUP_DATA*)pdata);
}

//***************************************

BOOL DupEnter(BOOL *recursed)
{
	DUP_DATA* ddata=g_d.GetDupData();

	if(!recursed)    //reload bases
		return TRUE;

	EnterCriticalSection(&ddata->SemCS);
	if(ddata->CurData->ThreadId==GetCurrentThreadId())
		return TRUE;

	if(!g_d.EnterDupRegion(ddata,!*recursed))
	{
		DEBUG_INT_3
		LeaveCriticalSection(&ddata->SemCS);

		return FALSE;
	}
	return TRUE;
}

BOOL DupExit(BOOL *recursed)
{
	DUP_DATA* ddata=g_d.GetDupData();

	if(!recursed)    //reload bases
		return g_d.LockBaseRegion(ddata);

	if(!*recursed)
		g_d.LeaveDupRegion(ddata,!*recursed);

	LeaveCriticalSection(&ddata->SemCS);
	
	return TRUE;
}

//***************************************

void* DupAlloc(unsigned size, BOOL static_, BOOL* allocated_by_new)
{
	DUP_DATA* ddata=g_d.GetDupData();

	if(!ddata->RegionLock)
	{
		if((ddata->RegionSize+size) > ddata->RegionLimit)
		{
			ddata->RegionSize+=size;
			dprintf("DupAlloc: Region size exceeded.\n");
//			DEBUG_INT_3
		}
		else
		{
			void* buf=ddata->Region+ddata->RegionSize;
			ddata->RegionSize+=size;
			if(allocated_by_new)
				*allocated_by_new=false;
			return buf;
		}
	}

	if(allocated_by_new)
		*allocated_by_new=true;

	return NULL;
}

void DupFree(void* ptr)
{
	if(!ptr)
		return;

	DUP_DATA* ddata=g_d.GetDupData();
	if(ddata && ddata->Region && ptr>=ddata->Region && ptr<(ddata->Region+ddata->RegionSize))
		return;

	delete ptr;
}

//***************************************
#endif // _WIN32
