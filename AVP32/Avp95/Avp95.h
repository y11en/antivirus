// AVP95.h - include file for VxD AVP95

#ifdef DEBUG
#define DEBUGPRINT
//#define DEBUGPRINT_CALLBACKS
#endif

#ifndef VTOOLSD
#error VTOOLSD should be defined for project
#endif

#include <vtoolscp.h>

#define DEVICE_CLASS        Avp95Device
#define AVP95_DeviceID      UNDEFINED_DEVICE_ID
#define AVP95_Init_Order    UNDEFINED_INIT_ORDER
#define AVP95_Major     3
#define AVP95_Minor     0
#define AVP95_Aer       NULL

#ifdef LAYERED_DRIVER
#define DRIVER_Drp    AVP95_Drp
#define DRIVER_Ilb    AVP95_Ilb

#ifndef DEVICE_MAIN
extern DRP DRIVER_Drp;
extern ILB DRIVER_Ilb;
#endif //DEVICE_MAIN
#endif //LAYERED_DRIVER

#include "../fpi/fpiapi.h"
#include "../fpi/fpi_ids.h"
#include "avp95api.h"
#include <ScanAPI/AVPioctl.h>
#include <Version/ver_avp.h>


class Avp95Device : public VDevice
{
public:
    Avp95Device();

    virtual BOOL OnSysCriticalInit(VMHANDLE hSysVM, PCHAR pszCmdTail, PVOID refData);
    virtual BOOL OnDeviceInit(VMHANDLE hSysVM, PCHAR pszCmdTail);
    virtual VOID OnSystemExit(VMHANDLE hSysVM);
    virtual VOID OnSysCriticalExit();
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

                                                    // publicly used functions
    DWORD AVP95VXD_Get_Version();                   // major==HIBYTE(HIWORD(ver)), minor==LOBYTE(HIWORD(ver)), build==LOWORD(ver)
    DWORD AVP95VXD_Initialize(PCHAR dbname_);
    DWORD AVP95VXD_RegisterCallback(AVPCallback callback_);
    DWORD AVP95VXD_ProcessObject(AVPScanObject * obj_);
	PCHAR AVP95VXD_MakeFullName(PCHAR buf_, PCHAR ptr_);


protected:
};

class Avp95VM : public VVirtualMachine
{
public:
    Avp95VM(VMHANDLE hVM);
    virtual BOOL OnSysVMInit();
    virtual VOID OnSysVMTerminate();
};

class Avp95Thread : public VThread
{
public:
    Avp95Thread(THREADHANDLE hThread);
};

extern AVPStat* Stat;
extern AVPBaseInfo* BaseInfo;

#ifdef FPIAPI_VER_MAJOR
extern LPFPIFILEIOFNTABLE FPI_IO; 
extern LPFPICALLBACK FPI_report;
void doerror(WORD param, DWORD refdata);
void doerrormsg(WORD param, DWORD refdata, const char* msg);
extern FPICONTAINERHANDLE fpiParentGet(LPCSTR filename_);
extern Avp95Device* Avp95DeviceInstance;
extern DWORD MFlags;
#endif
