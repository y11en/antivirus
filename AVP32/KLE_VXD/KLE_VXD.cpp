////////////////////////////////////////////////////////////////////
//
//  AVP95.CPP
//  Engine VxD Wrapper, contains all entry points to the VxD.
//  AVP 3.0 Engine
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1996
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#define DEVICE_MAIN
//#define LAYERED_DRIVER
#include "stdafx.h"

#ifdef LAYERED_DRIVER
Declare_Layered_Driver(KLE_VXD, DRP_MISC_PD,"KLE_SCAN_driver", 1, 0, 0)
#else
Declare_Virtual_Device(KLE_VXD)
#endif

#undef DEVICE_MAIN

extern "C" size_t wcslen( const char *string ) {return strlen (string);};
extern "C" char*  wcscpy( char* dest, const char *src ) {return strcpy (dest,src);};
#undef memmove
extern "C" void* memmove( void *dest, const void *src, size_t count ){return memcpy(dest,src,count);};

#define MYSTACKSIZE 0x8000
static MEMHANDLE hMem;	// Variable to hold memory handle returned
static PVOID LinAddr;	// Variable to hold linear address returned

/*

void* _cdecl nt_malloc(unsigned size_){
    return new BYTE[size_];
}

void __cdecl nt_free(void* ptr_){
	delete ptr_;
}
*/

static VMutex	mBusy;

BOOL WaitIfBusy()
{
	mBusy.enter();
	return TRUE;
}

void ReleaseWait()
{
	mBusy.leave();
}



static BOOL CharToOem(LPCTSTR s,LPSTR d)
{
	WORD UniStr[0x200];
	_QWORD res;
	if(!s || !d) return FALSE;
	int l=strlen(s);
	if(l>0x1FF)l=0x1FF;
	BCSToUni(UniStr,(UCHAR*)s,l,BCS_WANSI,&res);
	UniToBCS((UCHAR*)d,UniStr,l*2,0x200,BCS_OEM,&res);
	d[l]=0;

	return TRUE;
}

BOOL _QueryAbort(DWORD refdata_);

DWORD _Sleep()
{

	Enable_VM_Ints();	// Optional, allows hardware interrupts (like STI)
	Begin_Nest_Exec();	// Create nested execution context
	Resume_Exec();	// Ring-3 transfer allows scheduler to run
	End_Nest_Exec();	// Terminate nested block 

	return 0;
}


BOOL LoadIO();

Avp95Device::Avp95Device()
{
    LoadIO();
	AvpCallback     =KLE_AvpCallback;
}

Avp95Device::~Avp95Device()
{
}

extern PDDB AvpIoDDB; 

BOOL Avp95Device::OnSysDynamicDeviceInit()
{

	if(!WaitIfBusy()) return 0;

	if(!BAvpInit())return FALSE;
	if(FALSE==PageAllocate(MYSTACKSIZE/0x1000, PG_SYS, 0, 0, 
		0,0,0, PAGELOCKED, &hMem, &LinAddr)
		){
		BAvpDestroy();
		return FALSE;
	}
	
	ReleaseWait();

	return TRUE;
}


BOOL Avp95Device::OnSysDynamicDeviceExit()
{

	if(!WaitIfBusy()) return 0;

	BAvpDestroy();
	PageFree(hMem,0);

	if(	AvpIoDDB){
		DWORD ret=VXDLDR_UnloadDevice(-1,"AVP_IO");
		AvpIoDDB=NULL;
	}

	ReleaseWait();

    return TRUE;
}


DWORD Avp95Device::OnW32DeviceIoControl(PIOCTLPARAMS pDIOCParams)
{
	switch(pDIOCParams->dioc_IOCtlCode)
	{
	case KLE_IOCTL_GET_FUNCTION_TABLE:
		if(pDIOCParams->dioc_cbOutBuf<sizeof(DWORD))return 1;
		*(DWORD*)(pDIOCParams->dioc_OutBuf)=(DWORD)&FunctionTable;
		if(pDIOCParams->dioc_bytesret!=NULL)
			*(pDIOCParams->dioc_bytesret)=sizeof(DWORD);
		break;
		
	default:
		return VDevice::OnW32DeviceIoControl(pDIOCParams);
	}

	return DEVIOCTL_NOERROR;
}
