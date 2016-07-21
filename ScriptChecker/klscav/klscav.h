#ifndef __KLSCAV_H_
#define __KLSCAV_H_

#include "..\SC_Interceptor2\scplugin.h"

// Dispatch structures

#define SPD_IsBanner   0x00010001
#define SPD_IsWLBanner 0x00010002
typedef struct tag_SPD_IsBannerData {
	WCHAR* pwchHostUrl;
	WCHAR* pwchBannerUrl;
	WCHAR* pwchTag;
	BOOL   bDocumentWrite;
} SPD_IsBannerData;

#define SPD_IENavigateHidden   0x00020001
#define pmc_IE_NAVIGATE_HIDDEN 0x1E938242
#define msg_IE_NAVIGATE_HIDDEN 0x1E938243
typedef struct tag_SPD_IENavigateHiddenData {
	WCHAR*   pUrl;
	VARIANT* pPost;
	VARIANT* pHeader;
} SPD_IENavigateHiddenData;

#define pmc_PStore               0x1E948250
#define msg_PStoreCreateInstance 0x1E948251
typedef struct tag_SPD_PStoreCreateInstanceData {
	HMODULE  hModule;
	TCHAR*   szModuleName;
	bool     bExecutable;
	void*    ret_addr;
} SPD_PStoreCreateInstanceData;

#endif __KLSCAV_H_