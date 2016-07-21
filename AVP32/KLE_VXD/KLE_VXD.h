// AVP95.h - include file for VxD AVP95

#ifdef DEBUG
#define DEBUGPRINT
#endif

#include <vtoolscp.h>

#define DEVICE_CLASS        Avp95Device
#define KLE_VXD_DeviceID      UNDEFINED_DEVICE_ID
#define KLE_VXD_Init_Order    UNDEFINED_INIT_ORDER
#define KLE_VXD_Major     3
#define KLE_VXD_Minor     0
#define KLE_VXD_Aer       NULL

#ifdef LAYERED_DRIVER
#define DRIVER_Drp    KLE_VXD_Drp
#define DRIVER_Ilb    KLE_VXD_Ilb

#ifndef DEVICE_MAIN
extern DRP DRIVER_Drp;
extern ILB DRIVER_Ilb;
#endif //DEVICE_MAIN
#endif //LAYERED_DRIVER

class Avp95Device : public VDevice
{
public:
    Avp95Device();
    ~Avp95Device();

    virtual BOOL OnSysDynamicDeviceInit();
    virtual BOOL OnSysDynamicDeviceExit();
	virtual DWORD OnW32DeviceIoControl(PIOCTLPARAMS pDIOCParams);

                                                    // publicly used functions
protected:
};

class Avp95VM : public VVirtualMachine
{
public:
    Avp95VM(VMHANDLE hVM);
};

class Avp95Thread : public VThread
{
public:
    Avp95Thread(THREADHANDLE hThread);
};

