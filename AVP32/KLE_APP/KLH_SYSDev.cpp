// KLH_SYSDevice.cpp
// Implementation of KLH_SYSDevice device class

#include <vdw.h>

#include "KLH_SYS.h"
#include "KLH_SYSDev.h"
#include "KLH.h"


typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;
typedef int                 BOOL;
#include "../KLE/KLEAPI.H"

extern KLE_FUNCTION_TABLE* KLE_FT;

KMutex DetectArrayMutex(3);
KLH_REP_INFO* ri_head=0;
KLH_REP_INFO* ri_tail=0;

KLH_REP_INFO* RepGet()
{
	KLH_REP_INFO* p=ri_head;
	DetectArrayMutex.Wait();
	if(ri_head) ri_head=ri_head->Next;
	if(!ri_head)ri_tail=0;
	DetectArrayMutex.Release();
	return p;
}

void RepClean()
{
	KLH_REP_INFO* p;
	DetectArrayMutex.Wait();
	while(p=RepGet())
		delete p;
	DetectArrayMutex.Release();
}

void * RepAdd(unsigned Par1, unsigned Par2, const char* String1, const char* String2, const char* String3)
{
	KLH_REP_INFO *ro=new(POOL) KLH_REP_INFO;
	if(ro){
		memset(ro,0,sizeof(KLH_REP_INFO));
		ro->Param1=Par1;
		ro->Param2=Par2;
		if(String1)strncpy(ro->String1,String1,0x200);
		if(String2)strncpy(ro->String2,String2,0x200);
		if(String3)strncpy(ro->DebugStr,String3,0x200);
		DetectArrayMutex.Wait();

		ro->Next=0;
		if(ri_tail)
			ri_tail->Next=ro;
		ri_tail=ro;
		if(!ri_head)
			ri_head=ro;
		DetectArrayMutex.Release();
	}
	return ro;
}

tERROR KLE_CALL myCallback( KLE_SCAN_OBJECT* so, DWORD message, DWORD param, char* debugStr)
{
	if(so){
		char buf[0x200];
		DWORD size=0x200;
		if(KLE_FAILED(KLE_FT->GetPropString(so,KLE_PROP_OBJECT_NAME_STRING,buf,0,&size)))
			*buf=0;	

		char buf2[0x200];
		*buf2=0;
		size=0x200;
		switch(message){
		case KLE_CALLBACK_OBJECT_INFECTED:
			KLE_FT->GetPropString(so,KLE_PROP_VIRUS_NAME_STRING,buf2,0,&size);
			break;
		case KLE_CALLBACK_ARCHIVE_NEXT_NAME:
			KLE_FT->GetPropString(so,KLE_PROP_ARCHIVE_NAME_STRING,buf2,0,&size);
			break;
		default:
			break;
		}
		DWORD stat;
		if(KLE_FAILED(KLE_FT->GetPropDword(so,KLE_PROP_STATUS_DWORD,&stat)))
			stat=0;

		RepAdd(message, stat, buf, debugStr, buf2);
	}
	else{
		RepAdd(message, 0, 0, debugStr, param?(((LOAD_PROGRESS*)param)->curName):0);
	}

	return KLE_ERR_OK;		
}



#pragma code_seg("INIT")

// KLH_SYSDevice Constructor
//              The device constructor is typically responsible for allocating
//              any physical resources that are associated with the device.
//
//              The device constructor often reads the registry to setup
//              various configurable parameters.
//
KLH_SYSDevice::KLH_SYSDevice(ULONG Unit) :
	KDevice(
		KUnitizedName(L"KLH_SYSDevice", Unit),
		FILE_DEVICE_UNKNOWN,
		KUnitizedName(L"KLH_SYSDevice", Unit),
		0,
		DO_BUFFERED_IO)


	// Initialize KTrace object in device class
	,t(
		KUnitizedName(L"KLH_SYSDevice", Unit),
		defTraceDevice,
		defTraceOutputLevel,
		defTraceBreakLevel)
{
	if ( ! NT_SUCCESS(m_ConstructorStatus) )
	{
		_DebugTrace(TraceError, "Failed to create device KLH_SYSDevice unit number %d, status %x\n", Unit, m_ConstructorStatus);
		return;
	}
	m_Unit = Unit;

	m_RegPath = CreateRegistryPath(L"KLH_SYSDevice", Unit);
	if (m_RegPath == NULL)
	{
		// Error, cannot allocate memory for registry path
		_DebugTrace(TraceError, "Failed to create registry path\n");
		m_ConstructorStatus = STATUS_INSUFFICIENT_RESOURCES;
		return;
	}


}
#pragma code_seg()

// KLH_SYSDevice Destructor
//
KLH_SYSDevice::~KLH_SYSDevice()
{

	RepClean();
	delete m_RegPath;

}


// Member functions of KLH_SYSDevice
//

// Major function handlers
//
NTSTATUS KLH_SYSDevice::DeviceControl(KIrp I)
{
//    t.Trace(TraceInfo, "Entering DeviceControl, IRP= %x, IOCTL code= %x\n",(PIRP)I, I.IoctlCode());
    NTSTATUS status = STATUS_SUCCESS;
	I.Information()=0;


// TODO: Handle Internal Device Control request
    switch (I.IoctlCode())
    {
	case KLH_IOCTL_LOAD_BASES:
		if(I.IoctlInputBufferSize() ==0 ){
			status = STATUS_BUFFER_TOO_SMALL;
		}
		else{
			if(KLE_FAILED(KLE_FT->LoadBases( (char*)I.IoctlBuffer(), 0, myCallback)))
				status = STATUS_INTERNAL_ERROR;
		}
		break;
	case KLH_IOCTL_SCAN_FILE:
		if(I.IoctlInputBufferSize() ==0 ){
			status = STATUS_BUFFER_TOO_SMALL;
		}
		else{
			KLE_SCAN_OBJECT* pSO;
			if(KLE_FAILED(KLE_FT->InitScanObject(&pSO))){
				status = STATUS_MEMORY_NOT_ALLOCATED;
				break;
			}
			if(KLE_FAILED(KLE_FT->SetPropDword(pSO, KLE_PROP_CALLBACK_PTR_DWORD,(DWORD)myCallback))){
				KLE_FT->CleanupScanObject(pSO);
				status = STATUS_MEMORY_NOT_ALLOCATED;
				break;
			}

			KLE_FT->SetPropString(pSO,KLE_PROP_TEMP_PATH_STRING,"\\??\\c:\\!\\",0);

			UNICODE_STRING uniFilename;
			ANSI_STRING ansiFilename;
			RtlInitAnsiString( &ansiFilename, (char*)I.IoctlBuffer() );
			RtlAnsiStringToUnicodeString( &uniFilename, &ansiFilename, TRUE);

			if(KLE_FAILED(KLE_FT->ScanFile( pSO, (char*)I.IoctlBuffer(), 0)))
//			if(KLE_FAILED(KLE_FT->ScanFile( pSO, uniFilename.Buffer, 1)))
				status = STATUS_INTERNAL_ERROR;

			RtlFreeUnicodeString( &uniFilename );
			
			KLE_FT->GetPropDword(pSO, KLE_PROP_STATUS_DWORD,(DWORD*)I.IoctlBuffer());
			I.Information() = sizeof(DWORD);
			
			KLE_FT->CleanupScanObject(pSO);
			break;
			status = STATUS_INTERNAL_ERROR;
		}
		break;
	case KLH_IOCTL_GET_REP_INFO:
		if(I.IoctlOutputBufferSize() < sizeof(KLH_REP_INFO)){
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}
		KLH_REP_INFO* ptr;
		ptr=RepGet();
		if(ptr==NULL){
			status = STATUS_UNSUCCESSFUL;
			I.Information() = 0;
			break;
		}
		memcpy(I.IoctlBuffer(),ptr,sizeof(KLH_REP_INFO));
		delete ptr;
		I.Information() = sizeof(KLH_REP_INFO);
		break;

	default:
	    status = STATUS_INVALID_PARAMETER;
		break;
    }

    return I.Complete(status);
}


NTSTATUS KLH_SYSDevice::CreateClose(KIrp I)
{
//	t.Trace(TraceInfo, "Entering CreateClose, IRP= %x, Major Function= %x\n",
//		   (PIRP)I, I.MajorFunction());
    if (I.MajorFunction() == IRP_MJ_CREATE)
	{
		// TODO: Insert CREATE handler code
	}
	else
	{
		// Must be CLOSE since we are in CREATECLOSE handler
		// TODO: Insert CLOSE handler code
	}

	I.Information() = 0;
	return I.Complete(STATUS_SUCCESS);
}

