#ifndef __KL1LIB_H__
#define __KL1LIB_H__


#define KL1_SUCC          (0)
#define KL1_TRUE          (1)
#define KL1_FALSE         (0)
#define KL1_ERR           (-1)
#define KL1_NOLOADER      (-2)

// registers module for start during system boot
int KL1_RegisterModule (const WCHAR* name, const WCHAR* path);

// unregisters previously registered module 
int KL1_UnregisterModule (const WCHAR *name);

// checks if module is registered
int KL1_IsModuleRegistered (const WCHAR *name, const WCHAR* path);

// loads and starts module 
int KL1_LoadModule (const WCHAR *name, const WCHAR* path);

// stops and unloads module
int KL1_UnloadModule (const WCHAR *name);

// checks if module is loaded
int KL1_IsModuleLoaded (const WCHAR *name);

#endif //__KL1LIB_H__