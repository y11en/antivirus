// AVP_IO.h - include file for VxD AVP_IO

#include <vtoolscp.h>

#define DRIVER_Drp    AVPIO_Drp
#define DRIVER_Ilb    AVPIO_Ilb

#define DEVICE_CLASS		AvpioDevice
#define AVPIO_DeviceID		UNDEFINED_DEVICE_ID
#define AVPIO_Init_Order	UNDEFINED_INIT_ORDER
#define AVPIO_Major			1
#define AVPIO_Minor			0
#define AVPIO_Aer			NULL

class AvpioDevice : public VDevice
{
public:
	virtual BOOL OnDeviceInit(VMHANDLE hSysVM, PCHAR pszCmdTail);
	virtual BOOL OnSysDynamicDeviceInit();
	virtual BOOL OnSysDynamicDeviceExit();
	virtual DWORD OnW32DeviceIoControl(PIOCTLPARAMS pDIOCParams);
    
	virtual DWORD OnControlMessage(                 // control procedure overloaded for private message processing
                   DWORD _Edi, 
                   DWORD _Esi, 
                   DWORD _Ebp, 
                   DWORD _Esp,
                   DWORD _Ebx,
                   DWORD _Edx,
                   DWORD _Ecx,
                   DWORD _Eax);

};

class AvpioVM : public VVirtualMachine
{
public:
	AvpioVM(VMHANDLE hVM);
	virtual BOOL OnQueryDestroy();
};

class AvpioThread : public VThread
{
public:
	AvpioThread(THREADHANDLE hThread);
};

