////////////////////////////////////////////////////////////////////
//
//  KLH_VXD.CPP
//  KLEngine Helper
//  Alexey de Mont de Rique
//  Copyright (c) Kaspersky Labs. 2001
//
////////////////////////////////////////////////////////////////////

#define DEVICE_MAIN

#include "KLH_VXD.h"
#include "../KLE/KLEAPI.H"
#include "KLH.h"


#ifdef LAYERED_DRIVER
Declare_Layered_Driver(KLH_VXD, DRP_MISC_PD,"KLH_Help_driver", 1, 0, 0)
#else
Declare_Virtual_Device(KLH_VXD)
#endif

#undef DEVICE_MAIN


KLE_FUNCTION_TABLE* KLE_FT=0;

#ifdef NT_DRIVER
KMutex DetectArrayMutex(3);
#define POOL (NonPagedPool)
#else
static VMutex	DetectArrayMutex;
#define Wait enter
#define Release leave
#define POOL
#endif

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
	KLH_REP_INFO *ro=new POOL KLH_REP_INFO;
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

KLHDevice::KLHDevice()
{
	AvpIoDDB=NULL; 
}

KLHDevice::~KLHDevice()
{
}

BOOL KLHDevice::OnSysDynamicDeviceInit()
{
	if(AvpIoDDB==NULL){
		PDEVICEINFO pHandle;
		VXDLDR_LoadDevice(KLE_DEVICE_NAME_9X ".VXD", VXDLDR_INIT_DEVICE, &pHandle, &AvpIoDDB);
		
		if (AvpIoDDB==NULL){
			dprintf("ERROR!: Can't init " KLE_DEVICE_NAME_9X);
			return 0;
		}
		
		ALLREGS regs;
		IOCTLPARAMS par;
		memset(&regs, 0, sizeof(regs));
		memset(&par, 0, sizeof(IOCTLPARAMS));
		regs.RESI =(DWORD)&par;
		par.dioc_IOCtlCode=KLE_IOCTL_GET_FUNCTION_TABLE;
		par.dioc_cbOutBuf=4;
		par.dioc_OutBuf=&KLE_FT;
		
		Directed_Sys_Control(AvpIoDDB, W32_DEVICEIOCONTROL, &regs);
	}
	return !!((BOOL)KLE_FT);
}


BOOL KLHDevice::OnSysDynamicDeviceExit()
{
	if(	AvpIoDDB){
		VXDLDR_UnloadDevice(-1,KLE_DEVICE_NAME_9X );
		AvpIoDDB=NULL;
	}
	RepClean();
    return TRUE;
}


DWORD KLHDevice::OnW32DeviceIoControl(PIOCTLPARAMS pDIOCParams)
{
	DWORD ret=DEVIOCTL_NOERROR;

	switch(pDIOCParams->dioc_IOCtlCode)
	{
	case KLH_IOCTL_GET_REP_INFO:
		if(pDIOCParams->dioc_cbOutBuf<sizeof(KLH_REP_INFO))return 1;

		KLH_REP_INFO* ptr;
		ptr=RepGet();
		if(ptr==NULL)
			return 1;
		memcpy(pDIOCParams->dioc_OutBuf,ptr,sizeof(KLH_REP_INFO));
		delete ptr;
		if(pDIOCParams->dioc_bytesret!=NULL)
			*(pDIOCParams->dioc_bytesret)=sizeof(KLH_REP_INFO);
		break;

	case KLH_IOCTL_LOAD_BASES:
		if(KLE_FAILED(KLE_FT->LoadBases( (char*)pDIOCParams->dioc_InBuf, 0, myCallback)))
			return 1;
		break;
	case KLH_IOCTL_SCAN_FILE:
		if(pDIOCParams->dioc_cbInBuf ==0 )return 1;

		KLE_SCAN_OBJECT* pSO;
		if(KLE_FAILED(KLE_FT->InitScanObject(&pSO)))return 1;
		if(KLE_FAILED(KLE_FT->SetPropDword(pSO, KLE_PROP_CALLBACK_PTR_DWORD,(DWORD)myCallback))){
			KLE_FT->CleanupScanObject(pSO);
			return 1;	
		}
		
		KLE_FT->SetPropString(pSO,KLE_PROP_TEMP_PATH_STRING,"c:\\!\\",0);
		
		if(KLE_FAILED(KLE_FT->ScanFile( pSO, (char*)pDIOCParams->dioc_InBuf, 0))){
			ret=4;
		}
		if(pDIOCParams->dioc_cbOutBuf>=4){
			KLE_FT->GetPropDword(pSO, KLE_PROP_STATUS_DWORD,(DWORD*)pDIOCParams->dioc_OutBuf);
			if(pDIOCParams->dioc_bytesret!=NULL)
				*(pDIOCParams->dioc_bytesret)=sizeof(DWORD);
		}
		
		KLE_FT->CleanupScanObject(pSO);
		break;

	default:
		return VDevice::OnW32DeviceIoControl(pDIOCParams);
	}

	return ret;
}
