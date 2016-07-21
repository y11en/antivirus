// KLH_VXD.H

#ifdef DEBUG
#define DEBUGPRINT
#endif

#include <vtoolscp.h>

#define DEVICE_CLASS        KLHDevice
#define KLH_VXD_DeviceID      UNDEFINED_DEVICE_ID
#define KLH_VXD_Init_Order    UNDEFINED_INIT_ORDER
#define KLH_VXD_Major     3
#define KLH_VXD_Minor     0
#define KLH_VXD_Aer       NULL

#ifdef LAYERED_DRIVER
#define DRIVER_Drp    KLH_VXD_Drp
#define DRIVER_Ilb    KLH_VXD_Ilb

#ifndef DEVICE_MAIN
extern DRP DRIVER_Drp;
extern ILB DRIVER_Ilb;
#endif //DEVICE_MAIN
#endif //LAYERED_DRIVER

class KLHDevice : public VDevice
{
public:
	PDDB AvpIoDDB; 
    
	KLHDevice();
    ~KLHDevice();
	
    virtual BOOL OnSysDynamicDeviceInit();
    virtual BOOL OnSysDynamicDeviceExit();
	virtual DWORD OnW32DeviceIoControl(PIOCTLPARAMS pDIOCParams);

                                                    // publicly used functions
protected:
};

class KLHVM : public VVirtualMachine
{
public:
    KLHVM(VMHANDLE hVM);
};

class KLHThread : public VThread
{
public:
    KLHThread(THREADHANDLE hThread);
};

