//Win32 implementation
#include <windows.h>
#include <dupmem/dupmem.h>
#include <..\windows\hook\hook\fsdrvlib.h>
#include <..\windows\hook\hook\avpgcom.h>
#include <stuff/dprintf.h>

#define MAX_THREAD_NUMBER     0x80
#define RESERVE_LINK_SIZE     0x80000 // 512K is it enough? usually takes ~ 90k
#define RESERVE_STATIC_SIZE   0x40000 // 256K usually takes ~ 50k
#define MIN_NUMBER_OF_SHADOWS 4

#ifdef _WIN32
#ifdef  DEBUG
#define DEBUG_INT_3 __asm int 3;
#else
#define DEBUG_INT_3
#endif
#endif

#define ENTER_MODE_ERROR     0
#define ENTER_MODE_EXCLUSIVE 1
#define ENTER_MODE_SHARE     2

static BOOL      DriverLoaded=0;
static HANDLE    BusySemaphore=NULL;
static CRITICAL_SECTION SemCS;

extern "C" long QueueCounter=0;
extern "C" long MemShareCounter=0;
extern "C" unsigned RegionSize[3]={0,0,0};
static char*    Region[3]={0,0,0}; //the last one is for killed previous.
static unsigned RegionLimit[2]={RESERVE_LINK_SIZE,RESERVE_STATIC_SIZE};
static int      RegionLock[2]={0,0};
static DWORD    TlsEnterModeIndex=0xFFFFFFFF;

void* DupAlloc(unsigned size, BOOL static_, BOOL* allocated_by_new)
{
	char* buf=NULL;
	if(MemShareCounter || !Region[static_])
	{
		if(allocated_by_new)
			*allocated_by_new=true;
		return new char[size];
	}
	if(allocated_by_new)
		*allocated_by_new=false;

	buf= Region[static_]+RegionSize[static_];
	RegionSize[static_]+=size;
	if(RegionSize[static_]>RegionLimit[static_]){
		DEBUG_INT_3
		return NULL;
	}
	return buf;
}

void  DupFree(void* ptr)
{
	if(ptr){
		if(Region[0] && ptr>=Region[0] && ptr<(Region[0]+RegionSize[0])) return;
		if(Region[1] && ptr>=Region[1] && ptr<(Region[1]+RegionSize[1])) return;
		if(Region[2] && ptr>=Region[2] && ptr<(Region[2]+RegionSize[2])) return;
		delete ptr;
	}
}

//***************************************
BOOL DupInit( BOOL multithread )
{
	BOOL error=0;

	InitializeCriticalSection(&SemCS);
	TlsEnterModeIndex=TlsAlloc();
	if(TlsEnterModeIndex == 0xFFFFFFFF)
		error=1;
	BusySemaphore=CreateSemaphore(NULL,MAX_THREAD_NUMBER,1000000,NULL);
	if(BusySemaphore==NULL)
		error=1;
	
	if(multithread){
		DriverLoaded=FSDrvInterceptorInitTsp();
		dprintf("\nFSDrvInterceptorInitTsp return %d\n",DriverLoaded);
		if(DriverLoaded){
			RegionLock[0]=0;
			RegionLock[1]=0;
			RegionSize[0]=0;
			RegionSize[1]=0;
			error=1;
			Region[0]=(char*)FSDrvTSPRegister(RegionLimit[0]);
			Region[1]=(char*)FSDrvTSPRegister(RegionLimit[1]);
			if(Region[0] && Region[1]){
				if(DupAlloc(sizeof(DWORD),0) && DupAlloc(sizeof(DWORD),1,0)){
					*(DWORD*)Region[0]=-1;
					*(DWORD*)Region[1]=-1;
					dprintf("Region[0] %08X\n",Region[0]);
					dprintf("Region[1] %08X\n",Region[1]);
					error=0;
				}
			}
		}
	}
	if(error){
		DupDestroy();
	}
	return !error;
	
}

void DupDestroy()
{
	
	if(Region[1]){
		FSDrvTSPUnregister(Region[1]);
		Region[1]=0;
	}
	if(Region[0]){
		FSDrvTSPUnregister(Region[0]);
		Region[0]=0;
	}
	if(DriverLoaded) {
		FSDrvInterceptorDone();
		DriverLoaded=0;
	}
	if(TlsEnterModeIndex != 0xFFFFFFFF){
		TlsFree(TlsEnterModeIndex);
		TlsEnterModeIndex = 0xFFFFFFFF;
	}
	if(BusySemaphore) CloseHandle(BusySemaphore);
	DeleteCriticalSection( &SemCS );
}

BOOL DupEnter(BOOL *exclusive)
{
	BOOL error=FALSE;
	BOOL lock=exclusive?(*exclusive):1;
	if(!Region[0])lock=1;
	InterlockedIncrement(&QueueCounter);

	if(lock){// switch to monopole mode
		EnterCriticalSection(&SemCS);
lock:
		for(DWORD count=0;count<MAX_THREAD_NUMBER;count++)
				WaitForSingleObject(BusySemaphore,INFINITE);
		
		if(DriverLoaded){
			if(Region[0]){
				if(FSDrvTSPUnregister(Region[0])){
					Region[2]=Region[0];
					RegionSize[2]=RegionSize[0];
					Region[0]=NULL;
					RegionSize[0]=0;
					RegionSize[0]=0;
				}
				else{
					error=TRUE;
					dprintf("DupEnter: FSDrvTSPUnregister failed\n");
					DEBUG_INT_3
				}
				
			}
			if(!error){
				Region[0]=(char*)FSDrvTSPRegister(RegionLimit[0]);
				if(Region[0]){
					if(DupAlloc(sizeof(DWORD),0,0))
						*(DWORD*)Region[0]=-1;
				}
				else{
					error=TRUE;
					dprintf("DupEnter: FSDrvTSPRegister failed\n");
					DEBUG_INT_3
				}
			}
		}
		if(error){
			ReleaseSemaphore(BusySemaphore,MAX_THREAD_NUMBER,NULL);
			TlsSetValue(TlsEnterModeIndex,(LPVOID)ENTER_MODE_ERROR);
		}
		else{
			TlsSetValue(TlsEnterModeIndex,(LPVOID)ENTER_MODE_EXCLUSIVE);
		}
		LeaveCriticalSection(&SemCS);
	}
	else{ //share mode
		EnterCriticalSection(&SemCS);
		WaitForSingleObject(BusySemaphore,INFINITE);

		if(!RegionLock[0]){
			RegionLock[0]=FSDrvTSPLock(Region[0],RegionSize[0],MIN_NUMBER_OF_SHADOWS);
			dprintf("RegionLock[0] %08X\n",RegionLock[0]);
			if(!RegionLock[0]){
				DEBUG_INT_3
					ReleaseSemaphore(BusySemaphore,1,NULL);
				lock=1;
				goto lock;
			}
		}
		
		if(!RegionLock[1]){
			RegionLock[1]=FSDrvTSPLock(Region[1],RegionSize[1],MIN_NUMBER_OF_SHADOWS);
			dprintf("RegionLock[1] %08X\n",RegionLock[1]);
			if(!RegionLock[1]){
				DEBUG_INT_3
					ReleaseSemaphore(BusySemaphore,1,NULL);
				lock=1;
				goto lock;
			}
		}
		
		if(FSDrvTSPThreadEnter(Region[0])){
			if(FSDrvTSPThreadEnter(Region[1])){
				*(DWORD*)Region[0]=GetCurrentThreadId();
				*(DWORD*)Region[1]=GetCurrentThreadId();
			}else{
				FSDrvTSPThreadLeave(Region[0]);
				error=TRUE;
			}
		}else
			error=TRUE;
		
		if(error){
			ReleaseSemaphore(BusySemaphore,1,NULL);
			TlsSetValue(TlsEnterModeIndex,(LPVOID)ENTER_MODE_ERROR);
		}
		else{
			InterlockedIncrement(&MemShareCounter);
			TlsSetValue(TlsEnterModeIndex,(LPVOID)ENTER_MODE_SHARE);
		}
		LeaveCriticalSection(&SemCS);
	}
	
	InterlockedDecrement(&QueueCounter);
	if(exclusive)*exclusive=lock;
	return TRUE;
}


void DupExit(BOOL *exclusive)
{
	switch ((DWORD)TlsGetValue(TlsEnterModeIndex)){
	case ENTER_MODE_SHARE:
		FSDrvTSPThreadLeave(Region[0]);
		FSDrvTSPThreadLeave(Region[1]);
		InterlockedDecrement(&MemShareCounter);
		ReleaseSemaphore(BusySemaphore,1,NULL);
	case ENTER_MODE_EXCLUSIVE:
		ReleaseSemaphore(BusySemaphore,MAX_THREAD_NUMBER,NULL);
		break;
	default:
		dprintf("DupExit: Sync logic error\n");
		DEBUG_INT_3
			break;
	}
	TlsSetValue(TlsEnterModeIndex,(LPVOID)ENTER_MODE_ERROR);
}

