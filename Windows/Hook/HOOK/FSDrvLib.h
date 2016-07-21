#ifndef __FSDrvLib_H
#define __FSDrvLib_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif


// Загрузка драйвера и регистрация приложения
// Не вызывать из DLLMain из под Service manager-а если драйвер не загружен
BOOL __cdecl FSDrvInterceptorInit(void);	

// Разрегистрация приложения и выгрузка драйвера
void __cdecl FSDrvInterceptorDone(void);	

// Регистрация текущего потока как невидимого для драйвера
BOOL __cdecl FSDrvInterceptorRegisterInvisibleThread(void);	

// Разрегистрация текущего потока как невидимого для драйвера
void __cdecl FSDrvInterceptorUnregisterInvisibleThread(void);

BOOL __cdecl FSDrvInterceptorSetActive(BOOL bActive);


// -----------------------------------------------------------------------------------------
// extended api
BOOL __cdecl FSDrvInterceptorInitEx(DWORD AppFlags, ULONG AppID, ULONG Priority, ULONG CacheSize);
BOOL __cdecl FSDrvInterceptorInitTsp(void);	
//BOOL FSDrvInterceptorThreadSubProcesssing(PVOID pPageSet);	//PTHREAD_PAGE_SET pPageSet
//BOOL FSDrvVirtualLock(LPVOID lpAddress,SIZE_T dwSize);
//BOOL FSDrvVirtualUnLock(LPVOID lpAddress,SIZE_T dwSize);
typedef void* ORIG_HANDLE; //базовый адрес в третьем кольце
ORIG_HANDLE __cdecl FSDrvTSPRegister(SIZE_T max_size); //резервирует непрерывную область памяти (In bytes)+ комиттит 
BOOL __cdecl FSDrvTSPUnregister(ORIG_HANDLE hOrig); // освобождает
BOOL __cdecl FSDrvTSPLock(ORIG_HANDLE hOrig,SIZE_T real_size,DWORD min_number_of_shadows); //декомит лишних страниц , создание указанного числа копий.
BOOL __cdecl FSDrvTSPThreadEnter(ORIG_HANDLE hOrig); // если есть свободные копии в пуле,использует. если нет - создает и копирует 
BOOL __cdecl FSDrvTSPThreadLeave(ORIG_HANDLE hOrig); // сохраняет число копий не меньше указанного
BOOL __cdecl FSDrvTSPSetMinShadows(ULONG MinShadows); // min shadow

BOOL __cdecl FSDrv_GetDiskStat(unsigned __int64* pTimeCurrent, unsigned __int64* pTotalWaitTime);
// -----------------------------------------------------------------------------------------
// Using this method is NOT recomended!
HANDLE __cdecl FSDrvGetDeviceHandle();
HANDLE __cdecl FSDrvGetWhistlerUpHandle();
ULONG __cdecl FSDrvGetAppID();


#ifdef __cplusplus
}
#endif

#endif __FSDrvLib_H
