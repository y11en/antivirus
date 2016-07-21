#include <Stuff/_carray.h>
#include <ScanAPI/BaseAPI.h>
#include <ScanAPI/RetFlags.h>
#include <ScanAPI/avp_msg.h>
#include <ScanAPI/AVPioctl.h>

#define NT_DRIVER
#include <gkapi.h>
#if GATEKEEPER_API_VERSION!=0x0100
#include <fsavoid.h>
#endif

#ifdef GDATA
int dummy(int ret);
#define _FilterDeregisterThread(x) dummy(0)
#define _FilterRegisterThread(x) dummy(0)
#define _FilterRegister(x1,x2,x3,x4,x5,x6,x7) dummy(0)
#define _FilterDeregister(x1,x2) dummy(0)
#define _ChangeExtensions(x1) dummy(0)
#define _SendMessage(x1,x2,x3) dummy(0)
#define _FlushCache() dummy(0)
#define _FSAllowInternalAccess(x1)  dummy(0)
#define _FSRemoveInternalAccess(x1)  dummy(0)
#else
#define _FilterDeregisterThread(x) FilterDeregisterThread(x)
#define _FilterRegisterThread(x) FilterRegisterThread(x)
#define _FilterRegister(x1,x2,x3,x4,x5,x6,x7) FilterRegister(x1,x2,x3,x4,x5,x6,x7)
#define _FilterDeregister(x1,x2) FilterDeregister(x1,x2)
#define _ChangeExtensions(x1) ChangeExtensions(x1)
#define _SendMessage(x1,x2,x3) SendMessage(x1,x2,x3)
#define _FlushCache() FlushCache()
#define _FSAllowInternalAccess(x1)  FSAllowInternalAccess(x1)
#define _FSRemoveInternalAccess(x1)  FSRemoveInternalAccess(x1)
#endif

typedef struct _HandlerInfo{
	PCHAR uncFilename;
	PCHAR plainFilename;
	PPLUGINMESSAGE *pPluginMessage;
	BOOLEAN networkMount;
	BOOLEAN networkAccess;
	PBOOLEAN infected;
	ULONG mediaType;
	ULONG mediaInfo;
	PETHREAD petr;
	PDEVICE_OBJECT pDeviceObject; 
	WCHAR driveLetter; 
	DWORD dwTaskID;
	BOOL checkStopAfterDetect;
}HandlerInfo;

NTSTATUS BaseInit();
NTSTATUS BaseDestroy();
NTSTATUS BaseRegister();
NTSTATUS BaseDeregister();
NTSTATUS BaseLoad();
NTSTATUS BaseUnload();
NTSTATUS ProcessObject(
	AVPScanObject* so,
	HandlerInfo* hi=NULL,
	BOOL changed=FALSE
);

NTSTATUS Flush();

void DetectArrayAdd(AVPScanObject* p, char* realName, BOOL add_always);
void DetectSetEvent();

extern KUstring sRegistryPath;
extern ULONG Enable;
extern ULONG MFlags;
extern ULONG BFlags;
extern ULONG StaticMFlags;
extern KDriver* pFSAVPDriver;
extern DWORD LimitCompoundSize;

extern char* SetName;
extern AVPStat* Stat;
extern AVPBaseInfo* BaseInfo;
extern char* MessText[MESS_TEXT_MAX];
extern KMutex StatMutex;
#define __FSAVP_ID__ 10002L

#if		NTVERSION==350
#define NT_ROOT_PREFIX   "\\DosDevices\\"
#define LNT_ROOT_PREFIX L"\\DosDevices\\"
#elif	NTVERSION==351
#define NT_ROOT_PREFIX   "\\DosDevices\\"
#define LNT_ROOT_PREFIX L"\\DosDevices\\"
#else //	NTVERSION==400
#define NT_ROOT_PREFIX   "\\??\\"
#define LNT_ROOT_PREFIX L"\\??\\"
#endif
