#include "stdafx.h"

#define REMOVABLETYPE_NUMBER	2
#define MAX_KEYNAME				4
#define GUID_SIZE				39

typedef struct _chRemovableDevicesParsInfo{
	char	chKeyName[MAX_KEYNAME];
	char	chGUID[GUID_SIZE];
}chRemovableDevicesParsInfo, *pchRemovableDevicesParsInfo;

bool			MemoryInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
bool			ProcessorInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
bool			BIOSInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
bool			ProcessNumberInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
bool			GetFreeSystemDiskSizeInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
bool			GetPrinterInformation(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
bool			GetMultimediaInformation(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
bool			GetHardDiskInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
bool			GetModemInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
bool			GetNetworkAdaptersInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
bool			GetVideoInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
bool			GetRemovablesInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
bool			GetDateInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv);
//void			GetDateInfo(HWND hProgressCtrl);


DWORD			determineCpuSpeed(DWORD VendorID,DWORD Signature);
DWORD			calculateCpuSpeed(void);
////DWORD calculateCpuSpeedMethod2(void);
DWORD			getCpuSpeedFromRegistry(void);
DWORD			determineTimeStamp(void);
BOOL			determineCpuId(void);
DWORD			determineHighestCpuId(TCHAR* VendorID);
void			determineIntelCacheInfo(DWORD* CacheEax,DWORD* CacheEbx,DWORD* CacheEcx,DWORD* CacheEdx);
void			determineCpuIdentification(DWORD* Signature,DWORD* FeatureEbx,DWORD* FeatureEcx,DWORD* Features);
e_CPUVENDORS	getVendor(TCHAR Vendor[16]);
void			determineOldIntelName(DWORD dwHighestCpuId,DWORD Signature,DWORD CacheEax,DWORD CacheEbx,DWORD CacheEcx,DWORD CacheEdx,DWORD FeatureEbx,DWORD Features,char** strCpuInfo);
bool			determineCeleron(DWORD dwEax,DWORD dwEbx,DWORD dwEcx,DWORD dwEdx);
bool			determineXeon(DWORD dwEax,DWORD dwEbx,DWORD dwEcx,DWORD dwEdx);
DWORD			determineLargestExtendedFeature(void);
void			determineNameString(char** strCpuInfo);

void			AddDriver(const char chDeviceName[],const char chDriver[]);
