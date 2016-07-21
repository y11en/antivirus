#ifndef __ENUM_CTX_H__
#define __ENUM_CTX_H__

#include "Initialization.h"
#include "defines.h"

typedef tERROR (__stdcall *EnumFuncPtr)(const wchar_t* sFileName,const tCHAR* sSection,const tCHAR* sValue,tDWORD dwFlag,EnumContext*	pContext);

class EnumContext
{
public:

	EnumContext(EnumContext* pSrc);
	EnumContext(StartUpEnum2* pStartUpEnum, hOBJECT pSendToObj);
	~EnumContext();

	StartUpEnum2*			m_pStartUpEnum;
	
	EnumContext*	        m_pPrevContext;
	
	cRegEnumCtx*			m_cRegEnumCtx;
	
	ACTION_MESSAGE_DATA	    m_SendData;
	

	cPrStrW                 m_sFileName;
	cPrStrW                 m_sFullFilePathName;
	cPrStrW                 m_sShortInfectedName;
	
	cPrStrW                 m_sOriginalString;
	cPrStrW                 m_sDefaultValue;

	cPrStrW 				m_sEnumRootObject;
	
	hHASH					m_hMD5;
	hOBJECT					m_pSendToObj;

	hSTRING					m_wFileNames;

	PHashTree				m_pHashClsid;
	PHashTree				m_pHashInput;
	PHashTree				m_pHashParse;
	pUNLOAD_PROFILE_NAMES	pFileToDel;

	LONG					m_nLastModify;
	tDWORD					m_dwFlags;

	wchar_t*                m_pCurrentWorkingDir;
	bool                    m_bAdvancedDisinfection;
	
	bool					m_bCLSIDScanning;

public:
	tERROR SetInfectedObjectName(hSTRING hStrInfectedObject);
	bool   NeedRescan();

	tERROR EnumTaskManager();
	tERROR EnumStartUp();
	tERROR LSPEnum();

	void   ListFilesStartUp(const wchar_t* StartUpPath);
	tERROR ParsWLink(const wchar_t* lnkFileName);
	
	tERROR CheckHosts();
	tERROR HostsFileAnalyze(const wchar_t* sFileName);
	
	long   IfFileExist(const wchar_t* sString, bool bWithoutSending = false, bool bDontLowercase = false);
	tERROR _IfFileExist(const wchar_t* ParsString,int* iIndex, bool bWithoutSending);
	tERROR IOSFindFilePath(const wchar_t* pszFileName, bool bWithoutSending);
	tERROR _IOSFindFilePath(const wchar_t* pszFileName,const wchar_t sExt[5], bool bWithoutSending);
	tERROR _IOSFindFile(bool bWithoutSending = false);

	tBOOL  InitOLE();
	tBOOL  InitTaskManager(ITaskScheduler** ppITS);

	long   iRegEnumCallback(LPVOID pEnumCallbackContext, HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType);
	long   iRegEnumCallbackErrMessage(LPVOID pEnumCallbackContext, const wchar_t* sPath, const wchar_t* sName, long dError, bool Error);
	long   iRegEnumCallbackHosts(LPVOID pEnumCallbackContext, HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType);
	long   iRegEnumCallbackErrMessageHosts(LPVOID pEnumCallbackContext, const wchar_t* sPath, const wchar_t* sName, long dError, bool Error);
	long   iRegEnumCallbackLSP(LPVOID pEnumCallbackContext, HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType);
	long   iRegEnumCallbackErrMessageLSP(LPVOID pEnumCallbackContext, const wchar_t* sPath, const wchar_t* sName, long dError, bool Error);
	long   iRegEnumCallbackLSP2(LPVOID pEnumCallbackContext, HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType);

	tERROR IniEnum(const wchar_t* sFileName,const tCHAR* m_sSection,const tCHAR* m_sValue,tDWORD dwFlag);
	tERROR BatEnum(const wchar_t* sFileName,const tCHAR* m_sSection,const tCHAR* m_sValue,tDWORD dwFlag);
	tERROR ParsDLink(const wchar_t* sFileName);
	tERROR EnumByMaskToCheck(const wchar_t* sFileName,const tCHAR* m_sSection,const tCHAR* m_sValue,tDWORD dwFlag, EnumFuncPtr pEnumFuncPtr);
	tERROR ScanCLSID(const wchar_t* wPoint);

	tERROR Scan(hSTRING p_DelFName);
	SECTIONS_TYPE GetSectionType(const tCHAR* sSectionName);
	tDWORD ParsFlags(const tCHAR* sData,tDWORD* dwExtFlags);
	ULONG  GetObjFromDriver(ULONG Ioctl, ULONG ProcessId, LPVOID* Buffer/*, ULONG BufferSize*/);

	tERROR SendFileFound();
	tERROR SendError(const wchar_t* pFileName, const wchar_t* pSection, const wchar_t* pValue, STURTUP_OBJ_TYPES nStartupObjType, tERROR error);

	tERROR CheckTMCommandLine(ITaskScheduler* pITS, const wchar_t* wsTaskName, const wchar_t* wsCmdLine);
	tERROR Hash_AddItem_Quick(PHashTree pHash, const wchar_t* str, tDWORD len);
	tERROR Hash_FindItem_Quick(PHashTree pHash, const wchar_t* str, tDWORD len);
	tERROR Hash_Calc_Quick(hash_t* pHash, const wchar_t* str, tDWORD len);
	tERROR ParsEnviroment(const wchar_t* sString, cPrStrW& sTarget,pRESTORE_ORIGIN_STRING* fRest=NULL,tDWORD* fRestCount=NULL);
	tERROR AddRestPoint(pRESTORE_ORIGIN_STRING* fRest,tDWORD* fRestCount, tDWORD nOrigPos, tDWORD nOrigSize, tDWORD nNewPos, tDWORD nNewSize);

	tERROR ScanProcessesFromDriver();
	tERROR ScanPrefetchFromDriver(tDWORD nProgressRange);

private:
	
	struct tagUnmangledFileName
	{
		tagUnmangledFileName() : pData(NULL), bFree(cFALSE){}
		WCHAR*	pData;
		tBOOL	bFree;
	};

	bool UnmangledName(WCHAR* pwchBuffer, tagUnmangledFileName& tagUnmangled);
	static int __cdecl compare_mangled_wstr(const void * arg1, const void * arg2);

	tBOOL	IfPipeName(/*tDWORD size, */const wchar_t* name);
	void    ConvertToLongName(cPrStrW& sFilePath);
	tBOOL	IsNewStringEmpty(cPrStrW* m_sOriginalString);
	long    intIfFileExist(const wchar_t* sString, bool bWithoutSending = false, bool bDontLowercase = true);
	tBOOL	InitFolderForUser(const wchar_t* sProfileName,cPrStrW& sStartUpSubFolder);
	cPrStrW m_sLongNameConvert; // used in ConvertToLongName as temporary string
	cPrStrW m_sFileNameWithExt; // used in _IOSFindFilePath as temporary string

};

tDWORD ParseIniString(tCHAR* sIniString, const tCHAR** pParams, tDWORD nParamsCount);

#endif //__ENUM_CTX_H__