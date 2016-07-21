// common structures
#ifndef __COMMON_STRUCTURES
#define __COMMON_STRUCTURES

typedef enum TRANSFERTYPE
{
	_TRANSFER_REQUEST,
	_TRANSFER_ADD,
	_TRANSFER_DELETE,
	_TRANSFER_ADD2LOG,
	_TRANSFER_CLOSE_CONNECT,
};

typedef struct STRANSFERBLOCK
{
	DWORD dwTransferType;
	DWORD dwData;	// -1 if unknown
	DWORD dwCrc;
	char chAppName[256];
	char chObjName[256];
	char chTransfer[3072];
}TRANSFERBLOCK, *PTRANSFERBLOCK;

typedef struct _QUESTIONSETTING
{
	BOOL bQuestionVBScript;
	BOOL bQuestionJScript;
}QUESTIONSETTING, *PQUESTIONSETTING;

typedef enum _VIRUSSUSPICION
{
	_TYPE_CLEAN,
	_TYPE_EICAR,
	_TYPE_SUSPICION,
	_TYPE_POST,
	_TYPE_FILE,
	_TYPE_REG,
	_TYPE_AD,
}VIRUSSUSPICION, *PVIRUSSUSPICION;

typedef  BOOL (__cdecl *_pfCoclFunc)(LPSTR, DWORD, DWORD, TCHAR*, TCHAR*);
typedef  BOOL (__cdecl *_pfCoclFuncInfo)(TCHAR*, TCHAR*);

typedef  BOOL (__cdecl *_pfInit)();
typedef  BOOL (__cdecl *_pfDone)();

typedef DWORD (__cdecl *_pfGetResourceID)(DWORD _pCallbackValue, DWORD InterfaceID);
typedef DWORD (__cdecl *_pfShowRequest)(_pfGetResourceID pGetResourceID, PVIRUSSUSPICION pVirusSuspicion, HINSTANCE hDefaultInstance, TCHAR* pModuleName, TCHAR* pLangName, LPSTR lpScriptBody);

typedef struct _CALLBACK_INFO
{
	HINSTANCE hInstanceDefault;
	_pfGetResourceID pGetResourceID;
	TCHAR* pModuleName;
	TCHAR* pLangName;
	PVIRUSSUSPICION pVirusSuspicion;
	LPSTR lpScriptBody;
	HWND hWnd;
}CALLBACK_INFO, *PCALLBACK_INFO;

#ifdef _PRAGUE
#include <Prague/pr_remote.h>
#undef PR_API
#ifdef __cplusplus
#define PR_API //extern "C"
#else
#define PR_API
#endif
typedef struct tag_RemotePragueAPI {
	PR_API tERROR (pr_call *PRInitialize)(tDWORD flags);
	PR_API tERROR (pr_call *PRDeinitialize)();
	PR_API tERROR (pr_call *PRGetRoot)(hROOT *root);
	PR_API tERROR (pr_call *PRGetObjectProxy)(tPROCESS process, tSTRING name, hOBJECT *object);
	PR_API tERROR (pr_call *PRReleaseObjectProxy)(hOBJECT object);
	PR_API tERROR (pr_call *PRIsValidProxy)(hOBJECT object);
} tRemotePragueAPI;
#endif

#endif