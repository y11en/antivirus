#if defined (__unix__)
//Implementation that allows multipy instances of DupMem
//#define _WIN32_WINNT 0x0501
//
//#include <windows.h>

#include <Prague/prague.h>
#include <Prague/pr_sys.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_csect.h>

#define BOOL tBOOL
#define DWORD tDWORD

#include "DupMem.h"

#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
typedef unsigned long * tULONG_PTR;

tDWORD GetTickCount ()
{
  static long theStartSeconds;

  struct timeval aTimeVal;
  memset ( &aTimeVal, 0, sizeof ( aTimeVal ) );
  if ( gettimeofday ( &aTimeVal, 0 ) )
    return 0;
  return ( aTimeVal.tv_sec - theStartSeconds ) * 1000 + aTimeVal.tv_usec / 1000;
}

//#include <hook/fsdrvlib.h>
//#include <hook/avpgcom.h>

#include <Stuff/dprintf.h>

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

#define DEBUG_INT_3

#undef  dprintf

static int dprintf(const char* msg, ...)
{
	char s[512];
	int ret=0;
	va_list ap;
	va_start(ap,msg);
	strcpy(s, "DupMem\t");
//	ret=wvsprintf(s+7, msg, ap);
	ret=vsprintf(s+7, msg, ap);
	va_end(ap);

	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, s));
	return ret;
}

//***************************************

struct DUP_THREAD_DATA
{
//	tDWORD      ThreadId;
	pthread_t      ThreadId;
	tDWORD      LeaveTime;
	tULONG_PTR* MemPages;
};

struct DUP_DATA
{
	tBOOL       TspDriver;
	tBOOL       MemManager;
	tDWORD      Instance;
	long       QueueCounter;
	long       MemShareCounter;
	unsigned   RegionSize;
	char*      Region;
	unsigned   RegionLimit;
	int        RegionLock;
	int        RegionPages;
	char*      RegionBase;
	tDWORD*     CustomData;
	
//	CRITICAL_SECTION SemCS;
	cCriticalSection *SemCS;
	
	DUP_THREAD_DATA* CurData;
	DUP_THREAD_DATA* LastData;
	DUP_THREAD_DATA  DataCache[MAX_THREAD_NUMBER];
};

struct DUP_MEM
{
	DUP_MEM()
	{
//		InitializeCriticalSection(&InitCS);
		pthread_mutex_init( &InitCS, 0);
		
		//TlsDupDataIndex=TlsAlloc();
		pthread_key_create(&TlsDupDataIndex, NULL);
		
		DriverLoaded=0;
		InstanceCounter=0;
		SystemPageSize=0;
	}

	~DUP_MEM()
	{
//		if(TlsDupDataIndex != 0xFFFFFFFF)
//			TlsFree(TlsDupDataIndex);
		pthread_key_delete(TlsDupDataIndex);

//		DeleteCriticalSection(&InitCS);
		pthread_mutex_destroy(&InitCS);
	}

	tBOOL InitMemManagment()
	{
//		SYSTEM_INFO sSysInfo;
//		GetSystemInfo(&sSysInfo);
//		dprintf("GetSystemInfo return pagesize %d.\n",sSysInfo.dwPageSize);
//		SystemPageSize = sSysInfo.dwPageSize;
		SystemPageSize = getpagesize();
		dprintf("getpagesize() return pagesize %d.\n", SystemPageSize );

//		HMODULE hMod = GetModuleHandle("kernel32.dll");
//		if(!hMod)
//			return false;
//
//		*(void**)&fnMapUserPhysicalPages=GetProcAddress(hMod,"MapUserPhysicalPages");
//		*(void**)&fnAllocateUserPhysicalPages=GetProcAddress(hMod,"AllocateUserPhysicalPages");
//		*(void**)&fnFreeUserPhysicalPages=GetProcAddress(hMod,"FreeUserPhysicalPages");
//
//		if( !fnMapUserPhysicalPages||
//			!fnAllocateUserPhysicalPages||
//			!fnFreeUserPhysicalPages)
//			return false;
//
//		HANDLE hToken = NULL;
//		tBOOL Result = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
//
//		struct {tDWORD Count; LUID_AND_ATTRIBUTES Privilege [1];} Info;
//		if(Result)
//		{
//			Info.Count = 1;
//			Info.Privilege[0].Attributes = SE_PRIVILEGE_ENABLED;
//			Result = LookupPrivilegeValue(NULL, SE_LOCK_MEMORY_NAME, &(Info.Privilege[0].Luid));
//		}
//
//		if(Result)
//			Result=AdjustTokenPrivileges(hToken, false, (PTOKEN_PRIVILEGES) &Info, 0, NULL, NULL);
//		if(Result)
//			Result=GetLastError()==0;
//
//		if(hToken)
//			CloseHandle(hToken);
//		return Result;
		return true;
	}

	tBOOL InitMemPages(DUP_DATA* ddata)
	{
		tULONG_PTR*& MemPages=ddata->CurData->MemPages;

		if(!ddata->MemManager)
		{
			if(!MemPages)
				MemPages=(tULONG_PTR*)malloc(ddata->RegionLimit);
			return !!MemPages;
		}

//		tULONG_PTR NumberOfPages=ddata->RegionPages;
//
//		tBOOL error = false;
//		if(!MemPages)
//		{
//			MemPages=(tULONG_PTR*)malloc(NumberOfPages*sizeof(tULONG_PTR));
//			if(!MemPages)
//				return false;
//
//			HANDLE hToken = NULL;
//			OpenThreadToken(GetCurrentThread(), TOKEN_ALL_ACCESS, true, &hToken);
//			if(hToken)
//				RevertToSelf();
//
//			if(!fnAllocateUserPhysicalPages(GetCurrentProcess(), &NumberOfPages, MemPages))
//			{
//				DEBUG_INT_3
//				dprintf("AllocateUserPhysicalPages return %d\n",GetLastError());
//				error=true;
//			}
//			else if( NumberOfPages != ddata->RegionPages )
//			{
//				fnFreeUserPhysicalPages(GetCurrentProcess(), &NumberOfPages, MemPages);
//				dprintf("AllocateUserPhysicalPages returned not all pages\n");
//				error=true;
//			}
//
//			if(hToken)
//			{
//				SetThreadToken(NULL, hToken);
//				CloseHandle(hToken);
//			}
//
//			if( error )
//			{
//				free(MemPages);
//				MemPages=NULL;
//			}
//		}
//
////		fnMapUserPhysicalPages(ddata->Region, ddata->RegionPages, NULL);
//		if(!error && !fnMapUserPhysicalPages(ddata->Region, NumberOfPages, MemPages))
//		{
//			DEBUG_INT_3
//			dprintf("MapUserPhysicalPages return %d\n",GetLastError());
//			error=true;
//		}
//
//		return !error;
	}

	tBOOL FreeMemPages(DUP_DATA* ddata, DUP_THREAD_DATA* pData)
	{
		if(!pData->MemPages)
			return false;

//		if(ddata->MemManager)
//		{
//			tULONG_PTR NumberOfPages=ddata->RegionPages;
//			fnFreeUserPhysicalPages(GetCurrentProcess(), &NumberOfPages, pData->MemPages);
//		}
		free(pData->MemPages);
		pData->MemPages=NULL;
		return true;
	}

	tBOOL InitDupData(DUP_DATA* ddata, tDWORD multithread)
	{
		SetDupData(NULL);

		
//		EnterCriticalSection(&InitCS);
		pthread_mutex_lock(&InitCS);
		if(InstanceCounter++==0)
		{
			/*tDWORD dwVersion = GetVersion();
			if( !(dwVersion & 0x80000000) && (tDWORD)(LOBYTE(LOWORD(dwVersion))) < 5 ) // NT4
				DriverLoaded=false;
			else
				DriverLoaded=FSDrvInterceptorInitTsp();
			dprintf("\nFSDrvInterceptorInitTsp return %d\n",DriverLoaded);*/

			DriverLoaded=false;
			dprintf("TSP in driver is disable\n");

//			MemManagerInited=InitMemManagment();
			MemManagerInited=0; // use memcpy version
		}
//		LeaveCriticalSection(&InitCS);
		pthread_mutex_unlock(&InitCS);
		
		if( DriverLoaded && (multithread || !MemManagerInited) )
			ddata->TspDriver=DriverLoaded;
		else
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

		//EnterCriticalSection(&InitCS);
		pthread_mutex_lock(&InitCS);
		if(--InstanceCounter == 0 && DriverLoaded){
			//FSDrvInterceptorDone();
			DriverLoaded=0;
		}
		//LeaveCriticalSection(&InitCS);
		pthread_mutex_unlock(&InitCS);
	}

	tBOOL InitDupRegion(DUP_DATA* ddata)
	{
		ddata->RegionSize=0;
		ddata->RegionLock=0;
		if( ddata->TspDriver )
		{
//			ddata->Region=(char*)FSDrvTSPRegister(ddata->RegionLimit);
		}
		else
		{
			if(ddata->MemManager)
//				ddata->Region=(char*)VirtualAlloc(NULL, ddata->RegionLimit,
//					MEM_RESERVE|MEM_PHYSICAL, PAGE_READWRITE);
				NULL;
			else
				ddata->Region=(char*)malloc(ddata->RegionLimit);

			if( !InitMemPages(ddata) )
				return false;
		}

		if(!ddata->Region)
		{
			dprintf("DupAlloc: FSDrvTSPRegister failed\n");
			DEBUG_INT_3
			return false;
		}
		if(!DupAlloc(sizeof(tDWORD),0,0))
			return false;

		*(tDWORD*)ddata->Region=-1;
		return true;
	}

	tBOOL DeinitDupRegion(DUP_DATA* ddata)
	{
		tBOOL res = true;
		if(ddata->Region)
			if(ddata->TspDriver)
//				res = FSDrvTSPUnregister(ddata->Region);
				NULL;
			else if(ddata->MemManager)
//				res = VirtualFree(ddata->Region, 0, MEM_RELEASE);
				NULL;
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

	tBOOL LockBaseRegion(DUP_DATA* ddata)
	{
		if(!ddata->Region)
			return false;

		if(ddata->RegionLock)
			return true;

		if(ddata->TspDriver)
//			ddata->RegionLock=FSDrvTSPLock(ddata->Region,ddata->RegionSize,MIN_NUMBER_OF_SHADOWS);
			NULL;
		else
		{
			ddata->RegionBase=(char*)malloc(ddata->RegionSize);
			if(!ddata->RegionBase)
				return false;

			memcpy(ddata->RegionBase,ddata->Region,ddata->RegionSize);
			ddata->RegionLock=1;
		}

		dprintf("RegionLock %08X\n",ddata->Region);
		return true;
	}

	tBOOL EnterDupRegion(DUP_DATA* ddata, tBOOL process)
	{
		if(!ddata->RegionLock)
		{
			dprintf("EnterDupRegion region not locked\n");
			return false;
		}

//		tDWORD ThreadId=GetCurrentThreadId();
		pthread_t ThreadId = pthread_self();
		
		if(ddata->TspDriver)
		{
//			if(!FSDrvTSPThreadEnter(ddata->Region))
//			{
//				dprintf("FSDrvTSPThreadEnter failed %08X %d\n", ddata->Region, GetLastError());
//
//				FSDrvTSPThreadLeave(ddata->Region);
//				if(!FSDrvTSPThreadEnter(ddata->Region))
//				{
//					DEBUG_INT_3
//					return false;
//				}
//			}
//			*(tDWORD*)ddata->Region=ThreadId;
//			InterlockedIncrement(&ddata->MemShareCounter);
		}
		else
		{
//			tDWORD ToFindId=process ? 0 : ThreadId;
			pthread_t ToFindId=process ? 0 : ThreadId;
			
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
				return false;
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
		}
		return true;
	}

	tBOOL LeaveDupRegion(DUP_DATA* ddata, tBOOL process)
	{
		if(!ddata->RegionLock)
			return false;

		if(ddata->TspDriver)
		{
//			InterlockedDecrement(&ddata->MemShareCounter);
//			return FSDrvTSPThreadLeave(ddata->Region);
		}

		if(process)
		{
			ddata->CurData->ThreadId=0;
			ddata->CurData->LeaveTime=GetTickCount();
		}
		return true;
	}

	DUP_DATA* GetDupData()
	{
//		return (DUP_DATA*)TlsGetValue(TlsDupDataIndex);
		return (DUP_DATA*)pthread_getspecific(TlsDupDataIndex);
	}

	void SetDupData(DUP_DATA* ddata)
	{
//		TlsSetValue(TlsDupDataIndex,ddata);
		pthread_setspecific(TlsDupDataIndex,ddata);
	}

	tBOOL      DriverLoaded;
	tBOOL      MemManagerInited;
	unsigned  InstanceCounter;

//	tDWORD     TlsDupDataIndex;
	pthread_key_t	TlsDupDataIndex;

	tDWORD     SystemPageSize;
	
//	CRITICAL_SECTION InitCS;
//	cCriticalSection* InitCS;
	pthread_mutex_t 	InitCS;
	
//	tBOOL (WINAPI* fnMapUserPhysicalPages)(PVOID lpAddress,tULONG_PTR NumberOfPages,PULONG_PTR UserPfnArray);
//	tBOOL (WINAPI* fnAllocateUserPhysicalPages)(HANDLE hProcess,PULONG_PTR NumberOfPages,PULONG_PTR UserPfnArray);
//	tBOOL (WINAPI* fnFreeUserPhysicalPages)(HANDLE hProcess,PULONG_PTR NumberOfPages,PULONG_PTR UserPfnArray);
	tBOOL (* fnMapUserPhysicalPages)(void * lpAddress,tULONG_PTR NumberOfPages, unsigned long ** UserPfnArray);
	tBOOL (* fnAllocateUserPhysicalPages)(hOBJECT hProcess,unsigned long ** NumberOfPages,unsigned long ** UserPfnArray);
	tBOOL (* fnFreeUserPhysicalPages)(hOBJECT hProcess,unsigned long ** NumberOfPages,unsigned long ** UserPfnArray);

} g_d;

//***************************************

tBOOL DupGetDebugData( tDWORD* MtDebug, tDWORD* count)
{
	DUP_DATA* ddata=g_d.GetDupData();
	if(ddata && MtDebug && count && *count>=16)
	{
		MtDebug[0]=ddata->QueueCounter;
		MtDebug[1]=ddata->MemShareCounter;
		MtDebug[2]=ddata->RegionSize;
		*count=16;
		return true;
	}
	return false;
}

tDWORD DupSetCustomDWord(unsigned index, tDWORD data)
{
	DUP_DATA* ddata=g_d.GetDupData();
	if(ddata && ddata->CustomData && index<NUM_CUSTOM_DWORDS)
	{
		tDWORD old_data=ddata->CustomData[index];
		ddata->CustomData[index]=data;
		return old_data;
	}
	return -1;
}

tDWORD DupGetCustomDWord(unsigned index)
{
	DUP_DATA* ddata=g_d.GetDupData();
	if(ddata && ddata->CustomData && index<NUM_CUSTOM_DWORDS)
		return ddata->CustomData[index];
	return 0;
}

//***************************************

void* DupInit( tBOOL multithread )
{
	DUP_DATA* ddata=(DUP_DATA*)malloc(sizeof(DUP_DATA));
	if( !ddata )
		return NULL;

	memset(ddata,0,sizeof(DUP_DATA));
	ddata->CurData=ddata->DataCache;

//	InitializeCriticalSection(&ddata->SemCS);
	#warning error handling!!!!
	g_root->sysCreateObjectQuick((hOBJECT *)(&ddata->SemCS), IID_CRITICAL_SECTION);

	if(g_d.InitDupData(ddata, multithread))
		ddata->CustomData=(tDWORD*)DupAlloc(NUM_CUSTOM_DWORDS*sizeof(tDWORD),1,0);

	if(!ddata->CustomData)
	{
		DupDestroy(ddata);
		return NULL;
	}

	memset(ddata->CustomData,0,NUM_CUSTOM_DWORDS*sizeof(tDWORD));
	return ddata;
}

void* DupDestroy(void* pdata)
{
	DUP_DATA* ddata=(DUP_DATA*)pdata;
	if( !ddata )
		return NULL;

//	DeleteCriticalSection(&ddata->SemCS);
	if (ddata->SemCS)
	{
		ddata->SemCS->sysCloseObject();
		ddata->SemCS = NULL;
	}

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

tBOOL DupEnter(tBOOL *recursed)
{
	DUP_DATA* ddata=g_d.GetDupData();

	if(!recursed)    //reload bases
		return true;

	if(ddata->TspDriver)
	{
		if(*recursed)
			return true;
	}
	else
	{
//		EnterCriticalSection(&ddata->SemCS);
		ddata->SemCS->Enter(SHARE_LEVEL_WRITE);
//		if(ddata->CurData->ThreadId==GetCurrentThreadId())
		if(ddata->CurData->ThreadId==pthread_self())
			return true;
	}

	if(!g_d.EnterDupRegion(ddata,!*recursed))
	{
		DEBUG_INT_3
		if(!ddata->TspDriver)
		{
//			LeaveCriticalSection(&ddata->SemCS);
			tSHARE_LEVEL prev_level;
			ddata->SemCS->Leave(&prev_level);
		}
		return false;
	}
	return true;
}

tBOOL DupExit(tBOOL *recursed)
{
	DUP_DATA* ddata=g_d.GetDupData();

	if(!recursed)    //reload bases
		return g_d.LockBaseRegion(ddata);

/*	if(!ddata->TspDriver)
	{
		if(ddata->CurData->ThreadId!=GetCurrentThreadId())
		{
			DEBUG_INT_3
			return;
		}
		if(ddata->SemCS.RecursionCount==0)
		{
			DEBUG_INT_3
			return;
		}
	}
*/
	if(!*recursed || !ddata->TspDriver)
		g_d.LeaveDupRegion(ddata,!*recursed);

	if(!ddata->TspDriver)
	{
//		LeaveCriticalSection(&ddata->SemCS);
		tSHARE_LEVEL prev_level;
		ddata->SemCS->Leave(&prev_level);
	}
	return true;
}

//***************************************

void* DupAlloc(unsigned size, tBOOL static_, tBOOL* allocated_by_new)
{
	DUP_DATA* ddata=g_d.GetDupData();

	if(!ddata->RegionLock)
	{
		if((ddata->RegionSize+size) > ddata->RegionLimit)
		{
			dprintf("DupAlloc: Region size exceeded.\n");
			DEBUG_INT_3
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

	return new char[size];
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
#endif // __unix__
