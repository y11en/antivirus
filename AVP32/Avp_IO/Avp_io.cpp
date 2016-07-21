// AVP_IO.cpp - main module for VxD AVP_IO

#define DEVICE_MAIN
#include "avp_io.h"
#undef DEVICE_MAIN

/*
#ifdef Declare_Virtual_Device_Ex
#undef Declare_Virtual_Device_Ex
#endif

#define Declare_Virtual_Device_Ex(devName,RefData) \
	extern "C" VDevice* __cdecl _CreateDevice(void); \
	extern "C" VOID __stdcall v86_api_entry(void);	              \
	extern "C" VOID __stdcall pm_api_entry(void);		      \
	extern "C" VOID __stdcall localControlDispatcher(void);	      \
	extern "C" DDB The_DDB;					      \
	extern "C" void (*VXD_SERVICE_TABLE[])(void); 		      \
	extern "C" VDevice* TheDevice; \
	DDB The_DDB = {		      	      			      \
		0,						      \
		DDK_VERSION,					      \
		devName##_DeviceID,				      \
		devName##_Major,				      \
		devName##_Minor,				      \
		0,						      \
		{' ',' ',' ',' ',' ',' ',' ',' '},		      \
		devName##_Init_Order,				      \
		(DWORD)localControlDispatcher,			      \
		(DWORD)v86_api_entry,	      			      \
		(DWORD)pm_api_entry,				      \
		0,0,RefData,		 	 			      \
		(DWORD)VXD_SERVICE_TABLE,			      \
		0,					      	      \
		0,					      	      \
		__SIG__,				      	      \
	};							      \
	VDevice* __cdecl _CreateDevice(void)			      \
	{							      \
		return (VDevice*)new DEVICE_CLASS;		      \
	}	\
	extern "C" VOID __cdecl _DestroyDevice(void); \
	VOID __cdecl _DestroyDevice(void)	\
	{									\
		if ( TheDevice != NULL ) \
		delete (DEVICE_CLASS*)TheDevice;	\
		TheDevice = NULL;	\
	}


#ifdef Declare_Layered_Driver
#undef Declare_Layered_Driver
#endif
#define Declare_Layered_Driver(devName, _lgn, _asc, _rev, _fc, _ifr) \
	ILB devName##_Ilb;        \
	DRP devName##_Drp={   \
                {'X','X','X','X','X','X','X','X'}, \
                _lgn, \
                devName##_Aer, \
                &devName##_Ilb, \
                _asc, \
                _rev, \
                _fc,  \
                _ifr  };\
Declare_Virtual_Device_Ex(devName,(DWORD)&devName##_Drp)

//Declare_Virtual_Device(AVPIO)
*/
Declare_Layered_Driver(AVPIO, DRP_MISC_PD,"AVP_IO_VXD", 1, 0, 0)

#include <_avpio.h>
#include "../avp_iocl.h"



DWORD AvpioDevice::OnW32DeviceIoControl(PIOCTLPARAMS pDIOCParams)
{
	DWORD retval=DEVIOCTL_NOERROR;
/*
#ifdef DEBUG
      dprintf("DevIOCtrl: %08X",pDIOCParams->dioc_IOCtlCode);
#endif
*/
    switch(pDIOCParams->dioc_IOCtlCode){

    case IOCTL_AVPIO_MEMORY_READ:
		if(pDIOCParams->dioc_cbInBuf < sizeof(FlatIoCb))return 1;
        {
            FlatIoCb* cb;
            cb=(FlatIoCb*)(pDIOCParams->dioc_InBuf);
            if(pDIOCParams->dioc_cbOutBuf < cb->size)return 1;
            if(cb->size!=_AvpMemoryRead(cb->offset,cb->size,(BYTE*)pDIOCParams->dioc_OutBuf))
				return 1;
			if(pDIOCParams->dioc_bytesret!=NULL)
                *(pDIOCParams->dioc_bytesret)=cb->size;
        }
        break;

    case IOCTL_AVPIO_MEMORY_WRITE:
        if(pDIOCParams->dioc_cbInBuf<sizeof(FlatIoCb))return 1;
        {
            FlatIoCb* cb;
            cb=(FlatIoCb*)(pDIOCParams->dioc_InBuf);
            if(pDIOCParams->dioc_cbInBuf<cb->size+sizeof(FlatIoCb))return 1;
            if(cb->size!=_AvpMemoryWrite(cb->offset,cb->size,cb->buffer))
				return 1;
        }
        break;

    case IOCTL_AVPIO_MEMORY_GET_DOS_MEM_SIZE:
        if(pDIOCParams->dioc_cbOutBuf<sizeof(DWORD))return 1;
        *(DWORD*)(pDIOCParams->dioc_OutBuf)=_AvpGetDosMemSize();
        if(pDIOCParams->dioc_bytesret!=NULL)
            *(pDIOCParams->dioc_bytesret)=sizeof(DWORD);
        break;

    case IOCTL_AVPIO_MEMORY_GET_FIRST_MCB_SEG:
        if(pDIOCParams->dioc_cbOutBuf<sizeof(DWORD))return 1;
        *(DWORD*)(pDIOCParams->dioc_OutBuf)=_AvpGetFirstMcbSeg();
        if(pDIOCParams->dioc_bytesret!=NULL)
            *(pDIOCParams->dioc_bytesret)=sizeof(DWORD);
        break;

    case IOCTL_AVPIO_MEMORY_GET_IFSAPI_HOOK_TABLE:
        if(pDIOCParams->dioc_cbOutBuf<sizeof(DWORD))return 1;
        {
            DWORD count=_AvpGetIfsApiHookTable(
                (DWORD*)(pDIOCParams->dioc_OutBuf),
                pDIOCParams->dioc_cbOutBuf/sizeof(DWORD));
			if(pDIOCParams->dioc_bytesret!=NULL)
				*(pDIOCParams->dioc_bytesret)=count*sizeof(DWORD);
		}
        break;

    case IOCTL_AVPIO_MEMORY_GET_DOS_TRACE_TABLE:
        if(pDIOCParams->dioc_cbOutBuf<sizeof(DWORD))return 1;
        {
            DWORD count=_AvpGetDosTraceTable(
                (DWORD*)(pDIOCParams->dioc_OutBuf),
                pDIOCParams->dioc_cbOutBuf/sizeof(DWORD));
			if(pDIOCParams->dioc_bytesret!=NULL)
				*(pDIOCParams->dioc_bytesret)=count*sizeof(DWORD);
		}
        break;

    case IOCTL_AVPIO_PHYSICAL_DISK_GET_PARAM:
        if(pDIOCParams->dioc_cbInBuf< 2*sizeof(BYTE))return 1;
        if(pDIOCParams->dioc_cbOutBuf<3)return 1;
		BOOL ret;
		ret=_AvpGetDiskParam(
			((BYTE*)pDIOCParams->dioc_InBuf)[0],
			((BYTE*)pDIOCParams->dioc_InBuf)[1],
			(WORD*)pDIOCParams->dioc_OutBuf,
			(BYTE*)pDIOCParams->dioc_OutBuf+2);
		if(pDIOCParams->dioc_bytesret!=NULL)
                *(pDIOCParams->dioc_bytesret)=3;
		if(!ret)return 1;
        break;


    case IOCTL_AVPIO_PHYSICAL_DISK_READ:
        if(pDIOCParams->dioc_cbInBuf < sizeof(DiskIoCb))return 1;
        {
                DiskIoCb* dcb;
                dcb=(DiskIoCb*)(pDIOCParams->dioc_InBuf);
                DWORD bytes= dcb->sectorSize * dcb->numSectors;
                if(pDIOCParams->dioc_cbOutBuf < bytes )return 1;

                if(!_AvpRead13((BYTE)dcb->disk, (WORD)dcb->sector, (BYTE)dcb->head,
                        (WORD)dcb->numSectors, (BYTE*)pDIOCParams->dioc_OutBuf))
					retval=1;
				else
					if(pDIOCParams->dioc_bytesret!=NULL)
							*(pDIOCParams->dioc_bytesret)=bytes;


        }
        break;
    case IOCTL_AVPIO_PHYSICAL_DISK_WRITE:
	    if(pDIOCParams->dioc_cbInBuf < sizeof(DiskIoCb))return 1;
        {
                DiskIoCb* dcb;
                dcb=(DiskIoCb*)(pDIOCParams->dioc_InBuf);
                DWORD bytes= dcb->sectorSize * dcb->numSectors;
                if(pDIOCParams->dioc_cbInBuf + sizeof(DiskIoCb) < bytes )return 1;

                if(!_AvpWrite13((BYTE)dcb->disk, (WORD)dcb->sector, (BYTE)dcb->head,
                        (WORD)dcb->numSectors, dcb->buffer))
                        retval=1;
        }
        break;

    case IOCTL_AVPIO_LOGICAL_DRIVE_READ:
	    if(pDIOCParams->dioc_cbInBuf < sizeof(DiskIoCb))return 1;
        {
                DiskIoCb* dcb;
                dcb=(DiskIoCb*)(pDIOCParams->dioc_InBuf);
                DWORD bytes= dcb->sectorSize * dcb->numSectors;
                if(pDIOCParams->dioc_cbOutBuf < bytes )return 1;

                if(!_AvpRead25((BYTE)dcb->disk, dcb->sector, (WORD)dcb->numSectors,
                        (BYTE*)pDIOCParams->dioc_OutBuf))
                    retval=1;
                else
					if(pDIOCParams->dioc_bytesret!=NULL)
                        *(pDIOCParams->dioc_bytesret)=bytes;
        }
        break;
    case IOCTL_AVPIO_LOGICAL_DRIVE_WRITE:
	    if(pDIOCParams->dioc_cbInBuf < sizeof(DiskIoCb))return 1;
        {
                DiskIoCb* dcb;
                dcb=(DiskIoCb*)(pDIOCParams->dioc_InBuf);
                DWORD bytes= dcb->sectorSize * dcb->numSectors;
                if((pDIOCParams->dioc_cbInBuf + sizeof(DiskIoCb)) < bytes )return 1;

                if(!_AvpWrite26((BYTE)dcb->disk, dcb->sector, (WORD)dcb->numSectors,
                        dcb->buffer))
                        retval=1;
        }
		break;

    default:
        return VDevice::OnW32DeviceIoControl(pDIOCParams);
    }
    return retval;
}

DWORD AvpioDevice::OnControlMessage(
                               DWORD _Edi,
                               DWORD _Esi,
                               DWORD _Ebp,
                               DWORD _Esp,
                               DWORD _Ebx,
                               DWORD _Edx,
                               DWORD _Ecx,
                               DWORD _Eax)
{
    if (_Eax == BEGIN_RESERVED_PRIVATE_SYSTEM_CONTROL)
        {                                           // AVP95CTRL_GETPTR: return pointer to AVP95Device object or a function

        DWORD ret_pointer = 0L;                     // must return NULL if no pointer found

        switch (_Edx)
            {
            case AvpIo_MemoryRead:
                ret_pointer = (DWORD)_AvpMemoryRead;
                break;
            case AvpIo_MemoryWrite:
                ret_pointer = (DWORD)_AvpMemoryWrite;
                break;
            case AvpIo_GetDosMemSize:
                ret_pointer = (DWORD)_AvpGetDosMemSize;
                break;
            case AvpIo_GetFirstMcbSeg:
                ret_pointer = (DWORD)_AvpGetFirstMcbSeg;
                break;
            case AvpIo_GetIfsApiHookTable:
                ret_pointer = (DWORD)_AvpGetIfsApiHookTable;
                break;
            case AvpIo_GetDosTraceTable:
                ret_pointer = (DWORD)_AvpGetDosTraceTable;
                break;
            case AvpIo_GetDiskParam:
                ret_pointer = (DWORD)_AvpGetDiskParam;
                break;
            case AvpIo_Read13:
                ret_pointer = (DWORD)_AvpRead13;
                break;
            case AvpIo_Write13:
                ret_pointer = (DWORD)_AvpWrite13;
                break;
            case AvpIo_Read25:
                ret_pointer = (DWORD)_AvpRead25;
                break;
            case AvpIo_Write26:
                ret_pointer = (DWORD)_AvpWrite26;
                break;
            }

        _asm    mov edx, dword ptr [ret_pointer];   // pass the result in edx

        return TRUE;
        }

    return VDevice::OnControlMessage(_Edi, _Esi, _Ebp, _Esp, _Ebx, _Edx, _Ecx, _Eax);
}


AvpioVM::AvpioVM(VMHANDLE hVM) : VVirtualMachine(hVM) {}

AvpioThread::AvpioThread(THREADHANDLE hThread) : VThread(hThread) {}

BOOL AvpioDevice::OnDeviceInit(VMHANDLE hSysVM, PCHAR pszCmdTail)
{
        return TRUE;
}

BOOL AvpioDevice::OnSysDynamicDeviceInit()
{
        return TRUE;
}

BOOL AvpioDevice::OnSysDynamicDeviceExit()
{
        return TRUE;
}

BOOL AvpioVM::OnQueryDestroy()
{
        return TRUE;
}
