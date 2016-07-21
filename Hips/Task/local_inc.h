#ifndef _LOCAL_HIPS_INC_H_
#define _LOCAL_HIPS_INC_H_

#include <windows.h>
#include <list>
//#include <structs/s_hips.h>
#include <stdio.h>
#include <tchar.h>
#include <queue>

#include <plugin/p_propertiesmap.h>
#include <structs/s_hips.h>

//#include <structs/s_hips.h>

extern "C" hROOT g_root;
extern LONG gMemCounter;

#define HIPS_DAT_FILE_NAME L"hips.dat"
#define HIPS_SER_DAT_FILE_NAME L"ser_hips.dat"

struct CHipsTask;


//////////////////////////////////////////////////////
//	hips manager error codes		
//////////////////////////////////////////////////////
#define HIPS_SUCCESS(res) (((BOOL)(res)) > 0)
//----------------------------------------------------
#define HIPS_EC_OK					1		//	ok
#define HIPS_EC_NO_MORE_ELEMENTS	2		//	no more elements in list
#define HIPS_EC_FAIL				-1		//	some error
#define HIPS_EC_INVALID_ARG			-2		//	wrong params
#define HIPS_EC_OUT_OF_MEMORY		-3		//
//////////////////////////////////////////////////////
const DWORD HIPS_MAX_RES_NAME = 500;
const DWORD HIPS_MAX_APP_NAME = MAX_PATH;
#define HIPS_APP_HASH_SIZE 16

//////////////////////////////////////////////////////
inline void StrAssign(wchar_t * l_pIsStr, wchar_t ** l_ppOutStr, DWORD MaxStr = HIPS_MAX_RES_NAME)
{
	if (l_ppOutStr)
		*l_ppOutStr = 0;
	if (l_pIsStr)
	{
		DWORD tempLen = (DWORD)wcslen(l_pIsStr);
		if ((tempLen > 0) && (tempLen < MaxStr/*HIPS_MAX_RES_NAME*/))
		{
			*l_ppOutStr = new wchar_t[tempLen + 1];
			wcscpy(*l_ppOutStr, l_pIsStr);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
#define HIPS_BASE_TYPE_STANDARD		0
#define HIPS_BASE_TYPE_CURRENT		1

//////////////////////////////////////////////////////////////////////////
enum EnumHipsRuleItemState {
	hrisDisable,
	hrisEnable
};
//////////////////////////////////////////////////////////////////////////
struct HipsAppGrFindMask {
	wchar_t * ServStr;
	DWORD	ServStrType;
	cAppGroup * pFindedGr;
	cAppGroup * pFindedParentGr;
	HipsAppGrFindMask()
	{
		pFindedGr = 0;
		pFindedParentGr = 0;
	}
};
//////////////////////////////////////////////////////
//enum EnumHipsResType {
//	hrtUnknown  = 1,
//	hrtFile		= 2,
//	hrtRegKey	= 3,
//	hrtDevice	= 4
//};

//////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////
typedef struct HipsResourceItem_ {
	DWORD			f_ResID;
	//DWORD			f_ResGrID;
	DWORD			f_ResType;
	wchar_t	*		f_ResName;		//	allocated here, free in destructor
	wchar_t	*		f_ResNativeName;
	wchar_t *		f_ResValue;
	//wchar_t *		f_ResDesc;
	DWORD			f_Flags;
	DWORD			f_Severity;
	HipsResourceItem_()
	{
		f_Flags = 0;
		f_ResID = 0;
		//f_ResGrID = 0;
		f_ResType = cResource::ehrtUnknown;
		f_ResName = 0;
		f_ResValue = 0;
		//f_ResDesc = 0;
		f_ResNativeName = 0;
		f_Severity = 0;
	}
	HipsResourceItem_(DWORD	l_ResID, /*DWORD l_ResGrID, */DWORD l_ResType, wchar_t * l_ResName, wchar_t * l_NativeName, wchar_t * l_ResValue, /*wchar_t * l_ResDesc, */DWORD l_Flags)
	{
		f_ResID = l_ResID;
		f_ResType = l_ResType;
		//f_ResGrID = l_ResGrID;
		StrAssign(l_ResName, &f_ResName);
		StrAssign(l_ResValue, &f_ResValue);
		//StrAssign(l_ResDesc, &f_ResDesc);
		StrAssign(l_NativeName, &f_ResNativeName);
		f_Flags = l_Flags;
		f_Severity = 0;
	}
	HipsResourceItem_(DWORD	l_ResID, DWORD l_ResType, DWORD l_Severity)
	{
		f_ResID = l_ResID;
		f_ResType = l_ResType;
		f_Severity = l_Severity;

		f_ResName = 0;
		f_ResValue = 0;
		f_ResNativeName = 0;
	}
	~HipsResourceItem_()
	{
		if (f_ResName)
			delete [] f_ResName;
		f_ResName = 0;
		if (f_ResValue)
			delete [] f_ResValue;
		f_ResValue = 0;
		//if (f_ResDesc)
		//	delete [] f_ResDesc;
		//f_ResDesc = 0;
		if (f_ResNativeName)
			delete [] f_ResNativeName;
		f_ResNativeName = 0;
		return;
	}
}SHipsResourceItem, *pSHipsResourceItem;
//////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////
typedef struct HipsAppItem_ {
	DWORD					f_AppID;
	DWORD					f_AppGrID;
	wchar_t	*				f_AppName;	//	allocated here, free in destructor
	wchar_t	*				f_AppNativeName;	//	allocated here, free in destructor
	BYTE					f_AppHash[HIPS_APP_HASH_SIZE];
	DWORD					f_AppFlags;
	HipsAppItem_() {
		f_AppID = 0;
		f_AppGrID = 0;
		f_AppName = 0;
		memset(f_AppHash, 0, HIPS_APP_HASH_SIZE);
		f_AppFlags = 0;
	}
	HipsAppItem_(DWORD l_AppID, DWORD l_AppGrID, wchar_t * l_AppName, wchar_t * l_AppNativeName, BYTE * l_pAppHash, DWORD l_AppFlags) {
		f_AppID = l_AppID;
		f_AppGrID = l_AppGrID;
		f_AppFlags = l_AppFlags;
		if (l_pAppHash) {
			memcpy(f_AppHash, l_pAppHash, HIPS_APP_HASH_SIZE);
		}
		else {
			memset(f_AppHash, 0, HIPS_APP_HASH_SIZE);
		}
		StrAssign(l_AppName, &f_AppName);
		StrAssign(l_AppNativeName, &f_AppNativeName);
	}
	~HipsAppItem_() {
		if (f_AppName)
			delete [] f_AppName;
		f_AppName = 0;
		if (f_AppNativeName)
			delete [] f_AppNativeName;
		f_AppNativeName = 0;
		return;
	}
}SHipsAppItem, *pSHipsAppItem;
//////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////
typedef struct SHipsRuleItem {
	DWORD					f_RuleID;		//	internal rule ID
	EnumHipsRuleItemState	f_State;		//	rule state
	DWORD					f_RuleGrID;		//
	DWORD					f_AppID;		//
	DWORD					f_ResID;		//	
	DWORD					f_ResType;		//
	DWORD					f_AccessFlag;	//	access flag
	wchar_t *				f_pAppName;		//	pointer to another list item
	wchar_t *				f_pAppNativeName;		//	pointer to another list item
	BYTE *					f_pAppHash;		//	pointer to another list item
	wchar_t *				f_pResName;		//	pointer to another list item
	wchar_t *				f_pResNativeName;		//	pointer to another list item
	wchar_t *				f_pResValue;	//	pointer to another list item
	//wchar_t *				f_pResDesc;		//	pointer to another list item
	//<<	temp
	wchar_t *				f_DevGuid;
	wchar_t *				f_DevType;
	wchar_t *				f_DevDesc;
	//>>	temp
	LONGLONG				f_DrvRuleID;	//	rule ID from driver
	DWORD					f_Severity;
	SHipsRuleItem() {
		f_RuleID = 0;
		f_State = hrisDisable;
		f_RuleGrID = 0;
		f_AppID = 0;
		f_ResID = 0;
		f_ResType = cResource::ehrtUnknown;
		f_AccessFlag = 0;
		f_pAppName = 0;
		f_pAppNativeName = 0;
		f_pAppHash = 0;
		f_pResName = 0;
		f_pResNativeName = 0;
		f_pResValue = 0;
		f_DrvRuleID = 0;

		f_DevGuid = 0;
		f_DevType = 0;
		f_DevDesc = 0;

		f_Severity = 0;
	}
	SHipsRuleItem(DWORD l_RuleID, EnumHipsRuleItemState l_State, DWORD l_RuleGrID, DWORD l_AppID, DWORD l_ResID, DWORD l_AccessFlag) {
		f_RuleID = l_RuleID;
		f_State = l_State;
		f_RuleGrID = l_RuleGrID;
		f_AppID = l_AppID;
		f_ResID = l_ResID;
		f_ResType = cResource::ehrtUnknown;
		f_AccessFlag = l_AccessFlag;
		f_pAppName = 0;
		f_pAppNativeName = 0;
		f_pAppHash = 0;
		f_pResName = 0;
		f_pResNativeName = 0;
		f_pResValue = 0;
		f_DrvRuleID = 0;

		f_DevGuid = 0;
		f_DevType = 0;
		f_DevDesc = 0;

		f_Severity = 0;
	}
	~SHipsRuleItem()
	{
		if (f_DevGuid)
			delete [] f_DevGuid;
		f_DevGuid = 0;
		if (f_DevType)
			delete [] f_DevType;
		f_DevType = 0;
		if (f_DevDesc)
			delete [] f_DevDesc;
		f_DevDesc = 0;
	}
}*pSHipsRuleItem;
//////////////////////////////////////////////////////
typedef struct HipsResGroupItem_ {
	DWORD			f_ResGrID;
	DWORD			f_ResGrParentID;
	wchar_t			f_ResGrName[100];
}SHipsResGroupItem, *pSHipsResGroupItem;
//////////////////////////////////////////////////////
typedef std::list<pSHipsResourceItem>	HipsResourceList;
typedef std::list<pSHipsResGroupItem>	HipsResourceGrList;
typedef std::list<pSHipsAppItem>		HipsAppList;
typedef std::list<pSHipsRuleItem>		HipsRulesList;


typedef std::list<pSHipsResourceItem>::iterator	HipsResourceListIt;
//typedef std::list<pSHipsResourceGrItem>::iterator	HipsResourceListGrIt;
typedef std::list<pSHipsAppItem>::iterator		HipsAppListIt;
typedef std::list<pSHipsRuleItem>::iterator		HipsRulesListIt;
typedef std::list<pSHipsResGroupItem>::iterator	HipsResGrListIt;


typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _impPROCESS_PARAMETERS
{
	ULONG AllocationSize;
	ULONG Size;
	ULONG Flags;
	ULONG reserved1;
	LONG Console;
	ULONG ProcessGroup;
	HANDLE hStdInput;
	HANDLE hStdOutput;
	HANDLE hStdError;
	UNICODE_STRING CurrentDirectoryName;
	HANDLE CurrentDirectoryHandle;
	UNICODE_STRING DllPath;
	UNICODE_STRING ImageFile;
	UNICODE_STRING CommandLine;
	PWSTR Environment;
	ULONG dwX;
	ULONG dwY;
	ULONG dwXSize;
	ULONG dwYSize;
	ULONG dwXCountChars;
	ULONG dwYCountChars;
	ULONG dwFillAttribite;
	ULONG dwFlags;
	ULONG wShowWindow;
	UNICODE_STRING WindowTitle;
	UNICODE_STRING Desktop;
	UNICODE_STRING reserved2;
	UNICODE_STRING reserved3;
}impPROCESS_PARAMETERS;

#endif
