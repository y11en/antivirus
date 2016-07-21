/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	kl_dll_port.h
 * \author	Andrew Kazachkov
 * \date	15.05.2003 12:42:51
 * \brief	Portable dll functions
 * 
 */

#ifndef __KL_DLL_PORT_H__
#define __KL_DLL_PORT_H__

#ifdef _WIN32
    #define DL_LOAD_LIBRARY(wpath)      LoadLibrary(KLSTD_W2CT2(wpath))
    #define DL_LOAD_LIBRARY_ALT(wpath)  LoadLibraryEx(KLSTD_W2CT2(wpath), NULL, LOAD_WITH_ALTERED_SEARCH_PATH)
    #define DL_FREE_LIBRARY(hlib)       FreeLibrary((HMODULE)hlib)    
    #define DL_GET_PROC_ADDR(hlib, sym) GetProcAddress((HMODULE)hlib, sym)
#endif

#ifdef __unix
	#include <dlfcn.h>
    #define DL_LOAD_LIBRARY(wpath)      dlopen(KLSTD_W2CA2(wpath), RTLD_NOW)
    #define DL_LOAD_LIBRARY_ALT(wpath)  dlopen(KLSTD_W2CA2(wpath), RTLD_NOW)
    #define DL_FREE_LIBRARY(hlib)       dlclose(hlib)    
    #define DL_GET_PROC_ADDR(hlib, sym) dlsym(hlib, sym)
#endif

#ifdef N_PLAT_NLM
//    #include <dlfcn.h>
#include <nwerrno.h>
#include <nwthread.h>
#include <nwadv.h>
#define N_PLAT_STATIC 


static void* LoadLibraryNW(char* path){
	
        int ret;
	unsigned int gDynLibHandle;

	ret = spawnlp(P_NOWAIT, path, NULL);

	if(ret == -1 && errno != EINUSE) {
//		printf("spawn errno=%x NwErrno=%x\n", errno, NetWareErrno);
           return 0;
	}

	delay(1000);
//#warning "Get Name NLM" 
	gDynLibHandle = FindNLMHandle("CONNTEST");
//printf("LoadLibraryNW gDynLibHandle=%d\n",gDynLibHandle);
	if(gDynLibHandle == 0) {
		return 0;
	}
return (void*) gDynLibHandle;	
}

static void* LoadLibraryExNW(char* path){
	int ret;
	unsigned int gDynLibHandle;

	ret = spawnlp(P_NOWAIT, path, NULL);
	if(ret == -1 && errno != EINUSE) {
//		printf("spawn errno=%x NwErrno=%x\n", errno, NetWareErrno);
           return 0;
	}

	delay(1000);
//#warning "Get Name NLM (Ex)" 
	gDynLibHandle = FindNLMHandle("CONNTEST");
	if(gDynLibHandle == 0) {
		return 0;
	}
return (void*) gDynLibHandle;	
}


#ifdef N_PLAT_STATIC
    #define DL_LOAD_LIBRARY(wpath)  ((void *)1)
    #define DL_LOAD_LIBRARY_ALT(wpath) ((void *)1) 
    #define DL_FREE_LIBRARY(hlib) 0          
    #define DL_GET_PROC_ADDR(hlib, sym) (sym)
#else
    #define DL_LOAD_LIBRARY(wpath)  LoadLibraryNW((char*)KLSTD_W2CA/*KLSTD_W2CT2*/(wpath))   
    #define DL_LOAD_LIBRARY_ALT(wpath) LoadLibraryExNW(KLSTD_W2CT2(wpath)) 
    #define DL_FREE_LIBRARY(hlib)           
    #define DL_GET_PROC_ADDR(hlib, sym) ImportSymbol((unsigned int)hlib, #sym)
#endif 

#endif
#endif //__KL_DLL_PORT_H__

 
