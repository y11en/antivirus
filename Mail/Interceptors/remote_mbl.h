
#ifndef _REMOTE_MBL_H_
#define _REMOTE_MBL_H_

#include <windows.h>
#include <pr_remote.h>
#include <iface/i_loader.h>
#include <iface/i_root.h>

#undef PR_API
#ifdef __cplusplus
#define PR_API //extern "C"
#else
#define PR_API
#endif


extern PRRemoteAPI g_RP_API;
extern hROOT g_hRoot;

tERROR RemoteMBLInit(HMODULE hModule, tCHAR* plugins_list[], tINT plugin_count);
tERROR RemoteMBLTerm();
hOBJECT RemoteMBLGet();

#endif//_REMOTE_MBL_H_