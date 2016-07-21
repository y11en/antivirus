#include "enumctx.h"

#include <Prague/plugin/p_hash_md5.h>

#include <HOOK\avpgcom.h>
#include <HOOK\HookSpec.h>
#include <HOOK\IDriver.h>

extern
BOOL
OLD_DrvRequest (
	HANDLE hDevice,
	PEXTERNAL_DRV_REQUEST pInRequest,
	PVOID pOutRequest,
	ULONG* OutRequestSize
	);

EnumContext::EnumContext(StartUpEnum2* pStartUpEnum, hOBJECT pSendToObj)
{
	m_cRegEnumCtx=new cRegEnumCtx(pStartUpEnum,pStartUpEnum->m_AppID);
	
	m_pStartUpEnum = pStartUpEnum;
	m_pPrevContext = NULL;
	
	m_SendData.m_dAction=ScanAct;
	m_SendData.m_ObjType=OBJECT_UNKNOWN_TYPE;
	m_SendData.m_sFilePath=L"<unknown>";
	m_SendData.m_sRoot=L"<unknown>";
	m_SendData.m_sSection=L"<unknown>";
	m_SendData.m_sValue=L"<unknown>";
	
	pStartUpEnum->sysCreateObjectQuick((hOBJECT*)&m_hMD5, IID_HASH, PID_HASH_MD5);

	m_pSendToObj = pSendToObj;

	m_pHashClsid=_Hash_InitNew(*pStartUpEnum,8);
	m_pHashInput=_Hash_InitNew(*pStartUpEnum,8);
	m_pHashParse=_Hash_InitNew(*pStartUpEnum,8);
	pFileToDel=NULL;

	m_nLastModify = 0;
	m_dwFlags = 0;

	m_pCurrentWorkingDir = NULL;

	
	m_sFullFilePathName.reserve_len(MAX_PATH);
	m_bAdvancedDisinfection = false;
	m_bCLSIDScanning = false;
}

tERROR EnumContext::SetInfectedObjectName(hSTRING hStrInfectedObject)
{
	tERROR error = errOK;
	hash_t	CRC64;
	tDWORD dSize;

	if (!hStrInfectedObject)
		return errOK;
	m_dwFlags |= CURE_ACTION_ACTIVE;
	m_wFileNames = hStrInfectedObject;
	error=hStrInfectedObject->Length(&dSize);
	dSize++;
	if (PR_SUCC(error) && !m_sFullFilePathName.reserve_len(dSize/*+1*/)) 
		error = errNOT_ENOUGH_MEMORY;
	if(PR_SUCC(error))
		error=hStrInfectedObject->ExportToBuff(&dSize,cSTRING_WHOLE,m_sFullFilePathName,dSize*2,cCP_UNICODE,cSTRING_Z); 
	if(PR_SUCC(error))
		_wcslwr(m_sFullFilePathName);
	if (PR_SUCC(error))
		dSize=wcslen(m_sFullFilePathName);
	if (PR_SUCC(error))
		error = Hash_Calc_Quick(&CRC64, m_sFullFilePathName, dSize);
	if (PR_SUCC(error))
	{
		cAutoNativeCS _lock(&m_pStartUpEnum->m_sCommonCriticalSection);
		if (m_pStartUpEnum->m_bHashKnownFilesExist)
		{
			bool bFullScan=cFALSE;
			if (errHASH_FOUND != _Hash_Find(m_pStartUpEnum->m_pHashKnownFiles,CRC64.h1,CRC64.h2,&bFullScan))
			{
				PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<CleanUp> <%S> is not found in m_pHashKnownFiles ",(wchar_t*)m_sFullFilePathName));
				return error=errNOT_FOUND;
			}
			if (bFullScan)
				m_pStartUpEnum->m_bHashKnownFilesExist=0;
		}
	}
	PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<CleanUp> started for <%S> file",(wchar_t*)m_sFullFilePathName));
	wchar_t* ptr;
	if(ptr=wcsrchr(m_sFullFilePathName,'\\'))
	{
		m_sShortInfectedName = ptr+1;
		if(ptr=wcschr(m_sShortInfectedName, '.'))
			*ptr=0;
		if(ptr=wcschr(m_sShortInfectedName, '~'))
			*ptr=0;
	}

	m_pStartUpEnum->heapAlloc((tPTR*)&pFileToDel,sizeof(UNLOAD_PROFILE_NAMES));
	if (pFileToDel)
	{
		pFileToDel->Count=0;
		pFileToDel->cProfileName[0]=0;
	}
//	_Hash_Done(*m_pStartUpEnum,m_pHashInput);
	_Hash_Done(*m_pStartUpEnum,m_pHashParse);
//	m_pHashInput = NULL;
	m_pHashParse = NULL;
	return error;
}

EnumContext::~EnumContext()
{
	delete m_cRegEnumCtx;

	if (m_pPrevContext)
		return;

	_Hash_Done(*m_pStartUpEnum,m_pHashClsid);
	_Hash_Done(*m_pStartUpEnum,m_pHashInput);
	_Hash_Done(*m_pStartUpEnum,m_pHashParse);
	if (pFileToDel)
	{
		DWORD i;
		DWORD Count=pFileToDel->Count;
		for (i=0;i<Count;i++)
		{
			m_pStartUpEnum->heapFree((void*)pFileToDel->cProfileName[i]);
			pFileToDel->cProfileName[i]=NULL;
		}
		m_pStartUpEnum->heapFree(pFileToDel);
		pFileToDel=NULL;
	}

	if (m_hMD5)
		m_hMD5->sysCloseObject();
}

EnumContext::EnumContext(EnumContext* pSrc)
{
	m_pStartUpEnum = pSrc->m_pStartUpEnum;
	m_cRegEnumCtx=new cRegEnumCtx(m_pStartUpEnum,m_pStartUpEnum->m_AppID);
	m_pPrevContext = pSrc;
	
	m_SendData = pSrc->m_SendData;
	
	m_sFileName = pSrc->m_sFileName;
	m_sFileNameWithExt = pSrc->m_sFileNameWithExt;
	m_sFullFilePathName = pSrc->m_sFullFilePathName;
	m_sLongNameConvert = pSrc->m_sLongNameConvert;
	
	m_sOriginalString = pSrc->m_sOriginalString;
	m_sDefaultValue = pSrc->m_sDefaultValue;
	
	m_hMD5 = pSrc->m_hMD5;
	m_pSendToObj = pSrc->m_pSendToObj;

	m_wFileNames = pSrc->m_wFileNames;

	m_pHashClsid = pSrc->m_pHashClsid;
	m_pHashInput = pSrc->m_pHashInput;
	m_pHashParse = pSrc->m_pHashParse;

	m_nLastModify = pSrc->m_nLastModify;
	m_dwFlags = pSrc->m_dwFlags;

	m_pCurrentWorkingDir = pSrc->m_pCurrentWorkingDir;
	m_bCLSIDScanning = false;
}

bool EnumContext::NeedRescan()
{
	if (m_nLastModify == m_pStartUpEnum->m_nModifyCounter)
		return false;
	m_nLastModify = m_pStartUpEnum->m_nModifyCounter;
	return true;
};

SECTIONS_TYPE EnumContext::GetSectionType(const tCHAR* sSectionName)
{
	if (_stricmp(sSectionName,"REGISTRY_TYPE")==0) return REGISTRY_TYPE;
	if (_stricmp(sSectionName,"INI_TYPE")==0) return INI_TYPE;
	if (_stricmp(sSectionName,"BAT_TYPE")==0) return BAT_TYPE;
	return UNKNOWN;
}


#define MAGIC_ASCC 0x43435341 // 'ASCC'

typedef struct tag_PREFETCH_DATA {
	DWORD dwVersion;           // 0x00 // = 0x11
	DWORD dwMagic;             // 0x04 // = 0x43435341 = 'ASCC'
	DWORD dwDataSize;          // 0x08 // total size
	WCHAR wcImageName[30];     // 0x0C // application image name, "NTOSBOOT" for boot process
	DWORD dwPathHash;          // 0x48 // hash or 0xB00DFAAD for boot process
	DWORD dwUnknown1;          // 0x4C // 1?
	DWORD dwEndOfNamesBlock;   // 0x50 // 
	DWORD dwUnknown2;          // 0x54 // names count?
	DWORD dwNamesOffset;       // 0x58 // start of names block
	DWORD dwNamesCount;        // 0x5C
	DWORD dwVolumesInfoOffset; // 0x60
	DWORD dwVolumesInfoCount;  // 0x64
	DWORD dwUnknown6;          // 0x68 // 0x8E=142
	DWORD dwUnknown7;          // 0x6C // 0
	DWORD dwUnknown8;          // 0x70 // 0xF8D8F200
	DWORD dwUnknown9;          // 0x74 // 0xFFFFFFFF
	DWORD dwUnknown10;         // 0x78 // 0x5650
	DWORD dwUnknown11;         // 0x7C // 0x59FC
	DWORD dwUnknown12;         // 0x80 // 0x1937
	DWORD dwUnknown13;         // 0x84 // 0x3FE
	DWORD dwUnknown14;         // 0x88 // 0x539
	DWORD dwUnknown15;         // 0x8C // 0
	DWORD dwUnknown16;         // 0x90 // 0
	DWORD dwUnknown17;         // 0x94 // 0
	DWORD dwUnknown18;         // 0x98 // 0
	DWORD dwUnknown19;         // 0x9C // 0
	FILETIME ftTimeStamp;      // 0xA0
} PREFETCH_DATA;

typedef struct tag_PREFETCH_NAME_DATA {
	WORD wSize;
	WORD wFlags;
	WCHAR wcName[1];
} PREFETCH_NAME_DATA;

#define PREFETCH_FLAG_DIRECTORY  1

int __cdecl EnumContext::compare_mangled_wstr(const void * arg1, const void * arg2) 
{
	return wcscmp(((tagUnmangledFileName*)arg1)->pData, ((tagUnmangledFileName*)arg2)->pData);
}

bool EnumContext::UnmangledName(WCHAR* pwchBuffer, tagUnmangledFileName& tagName)
{
	// delete unicode prefix
	if (*pwchBuffer && (pwchBuffer[1]=='?' || pwchBuffer[2]=='?'))
	{
		while (pwchBuffer[0] == '\\' || pwchBuffer[0] == '?')
			pwchBuffer++;
	}
	if (0 == _wcsnicmp(pwchBuffer, L"\\Device\\", 8) )
	{
		wchar_t	DrvName[MAX_PATH + 4];
		wchar_t	letter[3];
		
		PR_TRACE((g_root, prtNOTIFY, "startupenum2\t<ScanPrefetchFromDriver> Next image %S", pwchBuffer));
		
		letter[1]=L':';
		letter[2]=0;
		for (wchar_t drv='A';drv<'Z';drv++)
		{
			letter[0]=drv;
			DWORD len;
			if (pfQueryDosDevice(letter,DrvName,MAX_PATH))
			{
				len = wcslen(DrvName);
				if (0 == wcsncmp(pwchBuffer, DrvName, len) && len > 3)
				{
					pwchBuffer+=len;
					pwchBuffer-=2;
					pwchBuffer[0]=drv;
					pwchBuffer[1]=':';
					break;
				}
			}
		}
	}

	tagName.pData = pwchBuffer;
	tagName.bFree = cFALSE;

	if (_wcsnicmp(pwchBuffer, L"\\SystemRoot\\", sizeof ("\\SystemRoot\\")-1) == 0)
	{
		WCHAR buffer[MAX_PATH+4];
		DWORD dwSize = ExpandEnvironmentStringsW(L"%windir%", buffer, MAX_PATH);

		if (dwSize>1 && dwSize < MAX_PATH)
		{
			if (buffer[dwSize-2] != L'\\')
			{
				buffer[dwSize-1] = L'\\';
				buffer[dwSize] = L'\0';
				++dwSize;
			}

			tERROR error=m_pStartUpEnum->heapAlloc((tPTR*)&tagName.pData, (dwSize + wcslen(pwchBuffer) - (sizeof ("\\SystemRoot\\")-1)) * sizeof(WCHAR));
			if (PR_FAIL(error))
			{
				PR_TRACE((m_pStartUpEnum, prtERROR,"startupenum2\tUnmangledName - no memory"));
				return false;
			}

			wcscpy(tagName.pData, buffer);
			wcscat(tagName.pData, pwchBuffer + sizeof ("\\SystemRoot\\")-1);
			tagName.bFree = cTRUE;
		}
		
	}


	return true;
}

tERROR EnumContext::ScanPrefetchFromDriver(tDWORD nProgressRange)
{
	PREFETCH_DATA* pData = NULL;
	ULONG dwSize = 0;
	DWORD dwNameOffset;
	DWORD dwCount;
	DWORD dwScanned = 0;
	DWORD dwFileNamesCount = 0;
	tagUnmangledFileName* arrFileNames = NULL;
	tDWORD dwProgress;

	cAutoPrMemFree _freeData(*m_pStartUpEnum, (void**)&pData);
	cAutoPrMemFree _freeFileArray(*m_pStartUpEnum, (void**)&arrFileNames);

	if (!m_pStartUpEnum->m_hDevice)
	{
		PR_TRACE((m_pStartUpEnum, prtERROR, "startupenum2\tNo handle driver! (ScanPrefetchFromDriver)"));
		return errOBJECT_NOT_INITIALIZED;
	}

	dwSize = GetObjFromDriver(_AVPG_IOCTL_GET_PREFETCHINFO, 0, (LPVOID*) &pData);
	
	PR_TRACE((m_pStartUpEnum,prtIMPORTANT,"startupenum2\t<ScanPrefecthFromDriver> GetPrefecthFromDriver returned %d bytes", dwSize));
	if (!dwSize)
		return errOK;
	
	//////////////////////////////////////////////////////////////////////////
	
	LPVOID pDrvListBuffer = NULL;
	ULONG DrvListSize = GetObjFromDriver(_AVPG_IOCTL_GET_DRVLIST, 0,&pDrvListBuffer);
	
	PR_TRACE((m_pStartUpEnum, prtIMPORTANT,"startupenum2\t<ScanPrefecthFromDriver> GetDrvListFromDriver returned %d bytes", DrvListSize));
	cAutoPrMemFree _free_buff_drvlist(*m_pStartUpEnum, &pDrvListBuffer);

	DWORD dwDrvListCount = 0;
	size_t nStringOffset, nLastLen;
	for (nStringOffset = 0, nLastLen = 0; nLastLen = wcslen((PWCHAR)pDrvListBuffer + nStringOffset);)
	{
		nStringOffset+=nLastLen+1;
		if (DrvListSize>=nStringOffset*sizeof(WCHAR))
			++dwDrvListCount;
		else
			break;
	}
	//////////////////////////////////////////////////////////////////////////

	
	
	if (pData->dwMagic != MAGIC_ASCC && pData->dwMagic != 'ACCS')
		return errOBJECT_INCOMPATIBLE;
	if (pData->dwDataSize > dwSize)
		pData->dwDataSize = dwSize;
	if (pData->dwEndOfNamesBlock > dwSize)
		return errOBJECT_DATA_SIZE_UNDERSTATED;

	tERROR error=m_pStartUpEnum->heapAlloc((tPTR*)&arrFileNames, (pData->dwNamesCount + dwDrvListCount) * sizeof(arrFileNames[0]));
	if (PR_FAIL(error))
	{
		PR_TRACE((m_pStartUpEnum, prtERROR,"startupenum2\tScanPrefetchFromDriver - no memory"));
		return error;
	}

	for (dwCount=0, dwNameOffset=pData->dwNamesOffset; 
		dwNameOffset < pData->dwEndOfNamesBlock && dwCount < pData->dwNamesCount; 
		dwCount++)
	{
		if (dwNameOffset + sizeof(PREFETCH_NAME_DATA) > pData->dwEndOfNamesBlock)
			break;
		PREFETCH_NAME_DATA* pName = (PREFETCH_NAME_DATA*)((BYTE*)pData + dwNameOffset);
		if (dwNameOffset + sizeof(PREFETCH_NAME_DATA) + pName->wSize*sizeof(WCHAR) > pData->dwEndOfNamesBlock)
			break;

		PR_TRACE((m_pStartUpEnum, prtNOTIFY, "startupenum2\tScanPrefetchFromDriver #%d flags=%04X %s %S\n", dwCount, pName->wFlags, pName->wFlags & PREFETCH_FLAG_DIRECTORY ? "DIR " : "FILE",  pName->wcName));
		if (!(pName->wFlags & PREFETCH_FLAG_DIRECTORY))
		{
			UnmangledName(pName->wcName, arrFileNames[dwFileNamesCount]);
			dwFileNamesCount++;
		}
		dwNameOffset += pName->wSize*sizeof(WCHAR) + sizeof(PREFETCH_NAME_DATA);
	}

	for (nStringOffset = 0, nLastLen = 0; nLastLen = wcslen((PWCHAR)pDrvListBuffer + nStringOffset);)
	{
		if (DrvListSize > (nStringOffset + nLastLen)*sizeof(WCHAR))
		{
			UnmangledName((PWCHAR)pDrvListBuffer + nStringOffset, arrFileNames[dwFileNamesCount]);
			dwFileNamesCount++;
		}
		else
			break;

		nStringOffset += nLastLen + 1;
	}
	
	PR_TRACE((m_pStartUpEnum, prtIMPORTANT, "startupenum2\tScanPrefetchFromDriver: Before sort, files to scan %d", dwFileNamesCount));

	qsort(arrFileNames, dwFileNamesCount, sizeof(arrFileNames[0]), compare_mangled_wstr);

	PR_TRACE((m_pStartUpEnum, prtIMPORTANT, "startupenum2\tScanPrefetchFromDriver: Before scan (sort complete)"));

	dwProgress = m_pSendToObj->propGetDWord(m_pStartUpEnum->m_propId_PROGRESS);

	for (dwCount=0; dwCount < dwFileNamesCount; dwCount++)
	{
//		PR_TRACE((m_pStartUpEnum, prtNOTIFY, "startupenum2\t<ScanPrefetchFromDriver> image to ParsEnviroment: '%S'", arrFileNames[dwCount]));
//		error=ParsEnviroment(arrFileNames[dwCount],m_sFileName);
//		if (error==ERROR_NOT_ENOUGH_MEMORY)
//			return error;
//
		PR_TRACE((m_pStartUpEnum, prtNOTIFY, "startupenum2\t<ScanPrefetchFromDriver> image to scan: '%S'", arrFileNames[dwCount]));
//		_IfFileExist(m_sFileName, NULL, false);
		m_sFullFilePathName = arrFileNames[dwCount].pData;
		SendFileFound();
		dwScanned++;
		m_pSendToObj->propSetDWord(m_pStartUpEnum->m_propId_PROGRESS, dwProgress + dwCount*nProgressRange/dwFileNamesCount);

		if (arrFileNames[dwCount].bFree)
		{
			m_pStartUpEnum->heapFree(arrFileNames[dwCount].pData);
			arrFileNames[dwCount].pData = NULL;
			arrFileNames[dwCount].bFree = cFALSE;
		}
			
	}
	m_pSendToObj->propSetDWord(m_pStartUpEnum->m_propId_PROGRESS, dwProgress + nProgressRange);

	PR_TRACE((m_pStartUpEnum, prtIMPORTANT, "StartupEnum2\tScanPrefetchFromDriver finished, scanned=%d ", dwScanned));

	return errOK;
}


ULONG EnumContext::GetObjFromDriver(ULONG Ioctl, ULONG ProcessId, LPVOID* Buffer/*, ULONG BufferSize*/)
{
	if (!m_pStartUpEnum->m_hDevice)
	{
		PR_TRACE((m_pStartUpEnum, prtERROR, "startupenum2\tNo handle driver! (GetObjFromDriver)"));
		return 0;
	}
	
	//	ZeroMemory(Buffer, BufferSize);
	
	tBYTE buffer[sizeof(EXTERNAL_DRV_REQUEST) + sizeof(ULONG)];
	PEXTERNAL_DRV_REQUEST pExtReq = (PEXTERNAL_DRV_REQUEST) buffer;
	
	pExtReq->m_DrvID = FLTTYPE_SYSTEM;
	pExtReq->m_IOCTL = Ioctl;
	pExtReq->m_AppID = m_pStartUpEnum->m_AppID;
	
	pExtReq->m_BufferSize = sizeof(ULONG);
	*(ULONG*)pExtReq->m_Buffer = ProcessId;
	
	ULONG OutRequestSize = 0;
	
	OLD_DrvRequest(m_pStartUpEnum->m_hDevice, pExtReq, NULL, &OutRequestSize);
	PR_TRACE((m_pStartUpEnum, prtIMPORTANT,"startupenum2\tGetObjFromDriver - need size 0x%x", OutRequestSize));
	
	if (!OutRequestSize)
		OutRequestSize = 1024 * 128;
	
	tERROR error=m_pStartUpEnum->heapAlloc((tPTR*)Buffer,OutRequestSize+10);
	if (PR_FAIL(error))
	{
		PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\t<GetObjFromDriver> no memory"));
		return error;
	}
	
	pExtReq->m_DrvID = FLTTYPE_SYSTEM;
	pExtReq->m_IOCTL = Ioctl;
	pExtReq->m_AppID = m_pStartUpEnum->m_AppID;
	
	pExtReq->m_BufferSize = sizeof(ULONG);
	*(ULONG*)pExtReq->m_Buffer = ProcessId;
	
	BOOL bRet = OLD_DrvRequest(m_pStartUpEnum->m_hDevice, pExtReq, *Buffer, &OutRequestSize);
	
	PR_TRACE((m_pStartUpEnum, prtIMPORTANT, "StartupEnum2\tGetObjFromDriver result(%s) - size %d", bRet ? "ok" : "failed", OutRequestSize));
	
	if (bRet && OutRequestSize)
		return OutRequestSize;
	
	return 0;
}

tERROR EnumContext::Scan(hSTRING p_DelFName)
{
	tERROR			error=errOK;
	tDWORD			dValuesCount=0;
	tDWORD			i=0,j;
	tCHAR*			sSectionName;
	tCHAR*			sValName;
	tCHAR*			sData;
	SECTIONS_TYPE	dSType;

	const tCHAR*	ParamsArray[5];
#define sPars1 ParamsArray[0]
#define sPars2 ParamsArray[1]
#define sPars3 ParamsArray[2]
#define sPars4 ParamsArray[3]
#define sPars5 ParamsArray[4]


	tDWORD			dSCount;
	tDWORD			CountToDo=5;
	tDWORD			CurCount=0;
	cPrStrA         sIniString;
	cPrStrW         sFileName;

	EnterCriticalSection(&m_pStartUpEnum->m_sCommonCriticalSection);
	cAutoInterlockedCounter _active(&m_pStartUpEnum->m_nScanActiv);
	error = m_pStartUpEnum->ReInitObjects();
	PR_TRACE((g_root, prtIMPORTANT, "startupenum2\tScan nScanActiv=%d, m_hDecodeIO=%x, m_hBaseIni=%x, %terr", m_pStartUpEnum->m_nScanActiv, m_pStartUpEnum->m_hDecodeIO, m_pStartUpEnum->m_hBaseIni, error));
	if (PR_FAIL(error)) 
		_active.destroy();
	LeaveCriticalSection(&m_pStartUpEnum->m_sCommonCriticalSection);
	if (PR_FAIL(error)) 
		return errOBJECT_NOT_INITIALIZED;
	
	error = m_pStartUpEnum->m_hBaseIni->GetSectionsCount(&dSCount);
	if (PR_FAIL(error))
	{
		PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot get sections count, %terr",error));
		return error;
	}
	for (i=0;i<dSCount;i++) 
	{
		error=m_pStartUpEnum->m_hBaseIni->GetValuesCount(i,&j);
		if (PR_SUCC(error)) CountToDo+=j;
	}

	i=0;
	while ((error=m_pStartUpEnum->m_hBaseIni->EnumSections(i,&sSectionName,&dValuesCount))==errOK)
	{
		dSType=GetSectionType(sSectionName);
		for (j=0;j<dValuesCount;j++)
		{
			error=m_pStartUpEnum->m_hBaseIni->EnumValues(i,j,&sValName,&sData);
			if (error==errEND_OF_THE_LIST)
			{
				error=errOK;
				break;
			}
			if (error!=errOK)
				continue;
//INT3;
			PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\tIni command <%s>, %terr",sData/*sNextData*/,error));
			sIniString = sData;
			memset(&ParamsArray, 0, sizeof(ParamsArray));
			tDWORD nParamsCount = ParseIniString(sIniString,&ParamsArray[0],countof(ParamsArray));

//HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\IniFileMapping\win.ini|AeDebug|*\Software\Microsoft\Windows NT\CurrentVersion\AEDebug|Debugger|FLAG_RESTORE_DEFAULT|"drwtsn32 -p %ld -e %ld -g"
//			SYS:Microsoft\Windows NT\CurrentVersion\AeDebug
//HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\IniFileMapping\system.ini\boot|Shell|*\Software\Microsoft\Windows NT\CurrentVersion\Winlogon|Shell|FLAG_RESTORE_DEFAULT|explorer.exe
//			SYS:Microsoft\Windows NT\CurrentVersion\Winlogon
//HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\IniFileMapping\win.ini\Windows|Load|
//HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\IniFileMapping\win.ini\Windows|Run|
//HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\IniFileMapping\win.ini\Windows|AppInit_DLLs|HKLM\Software\Microsoft\Windows NT\CurrentVersion\Windows|AppInit_DLLs|FLAG_CUT_DATA
//			SYS:Microsoft\Windows NT\CurrentVersion\Windows
//HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\IniFileMapping\system.ini|drivers|HKLM\Software\Microsoft\Windows NT\CurrentVersion\Drivers|*|FLAG_DEL_VALUE
//			#SYS:Microsoft\Windows NT\CurrentVersion\drivers
//HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\IniFileMapping\system.ini|drivers32|HKLM\Software\Microsoft\Windows NT\CurrentVersion\Drivers32|*|FLAG_DEL_VALUE
//			SYS:Microsoft\Windows NT\CurrentVersion\Drivers32
//HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\IniFileMapping\system.ini\boot|SCRNSAVE.EXE|HKCU\Control Panel\Desktop|SCRNSAVE.EXE|FLAG_RESTORE_DEFAULT|logon.scr
//			USR:Control Panel\Desktop

			tDWORD dFlag=0;
			m_dwFlags &= FLAG_CLEAR_ACTIONS_SENDING;

			switch (dSType)
			{
			case REGISTRY_TYPE:
				{
					//ini struct KEY|Value|Flag|Default
					if (nParamsCount < 2)
						continue;
					dFlag=ParsFlags(sPars3,&m_dwFlags);
					if ((dFlag&FLAG_ENUM_WINNT_ONLY)&&(g_bIsWin9x)) break;
					if ((dFlag&FLAG_ENUM_WIN9X_ONLY)&&(!g_bIsWin9x)) break;
					if (sPars4)
					{
						cStringObj sDefault(sPars4);
						m_pStartUpEnum->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString (sDefault), 0, 0);
						m_sDefaultValue = sDefault.data();
					}
					if (m_bAdvancedDisinfection)
					{
						cStringObj name(sPars1);
						tERROR error = m_pStartUpEnum->sysSendMsg(pmc_ADVANCED_DISINFECTION, pm_ADVANCED_DISINFECTION_LOCK_REGKEY, (hOBJECT)cAutoString(name), NULL, NULL);						PR_TRACE((m_pStartUpEnum,prtIMPORTANT,"startupenum2\tNotify pm_ADVANCED_DISINFECTION_LOCK_REGKEY <%S>, result %terr", name.data(), error));
					}
					tDWORD Ret=m_cRegEnumCtx->RegEnumAW(sPars1,sPars2,dFlag,(LPVOID)this,RegEnumCallback,RegEnumCallbackErrMessage);
					m_dwFlags=m_dwFlags&FLAG_CLEAR_REG_DETECTED;
					if (Ret!=ERROR_SUCCESS)
						PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot enum registry path <%s>, value <%s>, %terr",sPars1,sPars2,Ret));
					if ((error==errOPERATION_CANCELED) || (Ret == errOPERATION_CANCELED))
						return error;
				}
				break;
			case INI_TYPE:
				//ini struct Path|Section|Value|Flag|Default
				dFlag=ParsFlags(sPars4,&m_dwFlags);
				if (nParamsCount < 3)
					continue;
				if ((dFlag&FLAG_ENUM_WINNT_ONLY)&&(g_bIsWin9x)) break;
				if ((dFlag&FLAG_ENUM_WIN9X_ONLY)&&(!g_bIsWin9x)) break;
				if (sPars5)
					m_sDefaultValue = sPars5;
				sFileName = sPars1;
				if (m_bAdvancedDisinfection && sPars1)
				{
					cStringObj name(sPars1);
					tERROR error = m_pStartUpEnum->sysSendMsg(pmc_ADVANCED_DISINFECTION, pm_ADVANCED_DISINFECTION_LOCK_FILE, (hOBJECT)cAutoString(name), NULL, NULL);
					PR_TRACE((m_pStartUpEnum,prtIMPORTANT,"startupenum2\tNotify pm_ADVANCED_DISINFECTION_LOCK_FILE <%S>, result %terr", name.data(), error));
				}
				error=EnumByMaskToCheck(sFileName,sPars2,sPars3,dFlag,cbIniEnum);
				if (error!=errEND_OF_THE_LIST)
				{
					if (PR_FAIL(error))
						PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot enum ini file <%S>, section <%s>, value <%s>, %terr",sPars1,sPars2,sPars3,error));
				}
				if (error==errOPERATION_CANCELED) 
					return error;
				break;
			case BAT_TYPE:
				//ini struct Path|Flag|Default
				dFlag=ParsFlags(sPars2,&m_dwFlags);
				if (nParamsCount < 1)
					continue;
				if ((dFlag&FLAG_ENUM_WINNT_ONLY)&&(g_bIsWin9x)) break;
				if ((dFlag&FLAG_ENUM_WIN9X_ONLY)&&(!g_bIsWin9x)) break;
				if (sPars3)
					m_sDefaultValue = sPars3;
				sFileName = sPars1;
				if (m_bAdvancedDisinfection && sPars1)
				{
					tERROR error;
					cStringObj name(sPars1);
					error = m_pStartUpEnum->sysSendMsg(pmc_ADVANCED_DISINFECTION, pm_ADVANCED_DISINFECTION_LOCK_FILE, (hOBJECT)cAutoString(name), NULL, NULL);
					PR_TRACE((m_pStartUpEnum,prtIMPORTANT,"startupenum2\tNotify pm_ADVANCED_DISINFECTION_LOCK_FILE <%S>, result %terr", name.data(), error));
				}
				error=EnumByMaskToCheck(sFileName,NULL,NULL,dFlag,cbBatEnum);
				if (PR_FAIL(error))
					PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot enum bat file <%S>, %terr",sPars1,error));
				if (error==errOPERATION_CANCELED) 
					return error;
				break;
			default:
				break;
			}
			if (m_sDefaultValue) 
				m_sDefaultValue[0]=0;
			error = m_pSendToObj->propSetDWord(m_pStartUpEnum->m_propId_PROGRESS,(CurCount++)*55/CountToDo);
		}
		i++;
	}
	if (error==errEND_OF_THE_LIST) 
		error=errOK;
	
	// enum task manager
	m_SendData.m_ObjType=OBJECT_TASK_MANAGER_TYPE;
	PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<Scan> Start TaskManager enum"));
	do {
		error=EnumTaskManager();
	} while(PR_SUCC(error) && NeedRescan());
	if (error==errOPERATION_CANCELED) 
		return error;
	error=m_pSendToObj->propSetDWord(m_pStartUpEnum->m_propId_PROGRESS,
		m_pSendToObj->propGetDWord(m_pStartUpEnum->m_propId_PROGRESS) + 1);

	m_SendData.m_ObjType=OBJECT_TASK_MANAGER_TYPE;
	PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<Scan> Start LSP enum"));
	error=LSPEnum();
	if (error==errOPERATION_CANCELED) 
		return error;
	error=m_pSendToObj->propSetDWord(m_pStartUpEnum->m_propId_PROGRESS,
		m_pSendToObj->propGetDWord(m_pStartUpEnum->m_propId_PROGRESS) + 1);
	
	m_SendData.m_ObjType=OBJECT_START_UP_MENU_TYPE;
	PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<Scan> Start StartUpMenu enum"));
	error=EnumStartUp();
	if (error==errOPERATION_CANCELED) 
		return error;
	error=m_pSendToObj->propSetDWord(m_pStartUpEnum->m_propId_PROGRESS,
		m_pSendToObj->propGetDWord(m_pStartUpEnum->m_propId_PROGRESS) + 2);
	
	if (!(m_dwFlags&CURE_ACTION_ACTIVE))
	{
		m_SendData.m_ObjType=OBJECT_HOSTS_TYPE;
		PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<Scan> Start HOSTS analyse"));
		error=CheckHosts();
		if (error==errOPERATION_CANCELED) 
			return error;
		error=m_pSendToObj->propSetDWord(m_pStartUpEnum->m_propId_PROGRESS,
			m_pSendToObj->propGetDWord(m_pStartUpEnum->m_propId_PROGRESS) + 1);

		PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<Scan> Start ProcessesFromDriver enum"));
		error = ScanProcessesFromDriver();
		if (error==errOPERATION_CANCELED) 
			return error;
		error=m_pSendToObj->propSetDWord(m_pStartUpEnum->m_propId_PROGRESS,
			m_pSendToObj->propGetDWord(m_pStartUpEnum->m_propId_PROGRESS) + 5);

		PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<Scan> Start ProcessesFromDriver enum"));
		error = ScanPrefetchFromDriver(100 - m_pSendToObj->propGetDWord(m_pStartUpEnum->m_propId_PROGRESS));

//		m_pStartUpEnum->m_bHashKnownFilesExist = cTRUE;
	}
	m_pStartUpEnum->m_bHashKnownFilesExist = cTRUE;
	
	error=m_pSendToObj->propSetDWord(m_pStartUpEnum->m_propId_PROGRESS, 100);

//	m_pStartUpEnum->m_bHashKnownFilesExist = cTRUE;
	return error;
}


tERROR EnumContext::ScanProcessesFromDriver()
{
	tERROR error = errOK;
	if (g_bIsWin9x)
		return errOK;
	PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<ScanProcessesFromDriver> Start Search hidden processes"));
	PVOID pBuffer=NULL;

	// request sections for this ProcessID
	ULONG SecsSize=GetObjFromDriver(_AVPG_IOCTL_GET_SECTIONLIST, 0,&pBuffer/*,_sec_buffer_size*/);
	if (!SecsSize)
	{
		PR_TRACE((m_pStartUpEnum,prtIMPORTANT,"startupenum2\t<ScanProcessesFromDriver> GetSectionsFromDriver returned zero"));
		return errOK;
	}
	cAutoPrMemFree _free_buff(*m_pStartUpEnum, &pBuffer);

	wchar_t* pwchBuffer = (wchar_t*) pBuffer;
	wchar_t* pwchBufferEnd = (wchar_t*)((char*)pBuffer + (SecsSize & ~1));
	m_SendData.m_ObjType=OBJECT_TYPE_RUNNING_PROCESS;
	m_SendData.m_sSection=NULL;
	m_SendData.m_sValue=NULL;
	m_SendData.m_sRoot=NULL;
	
	while (pwchBuffer < pwchBufferEnd)
	{
		wchar_t* ptr;
		wchar_t* pName = NULL;
		// extract name
		for (ptr=pwchBuffer; ptr<pwchBufferEnd; ptr++)
		{
			if (0 == *ptr)
			{
				pName = pwchBuffer;
				pwchBuffer = ptr+1;
				break;
			}
		}

		if (!pName) // unexpected buffer end
			break;
		if (0 == pName[0]) // end of names
			break;

		PR_TRACE((m_pStartUpEnum, prtNOTIFY, "startupenum2\t<ScanProcessesFromDriver> Next image %S", pName));

		// delete unicode prefix
		if (pName[1]=='?' || pName[2]=='?')
		{
			while (pName[0] == '\\' || pName[0] == '?')
				pName++;
		}
		if (0 == _wcsnicmp(pName, L"\\Device\\", 8) )
		{
			wchar_t	DrvName[MAX_PATH + 4];
			wchar_t	letter[3];
			
			letter[1]=L':';
			letter[2]=0;
			for (wchar_t drv='A';drv<'Z';drv++)
			{
				letter[0]=drv;
				DWORD len;
				if (pfQueryDosDevice(letter,DrvName,MAX_PATH))
				{
					len = wcslen(DrvName);
					if (0 == wcsncmp(pName, DrvName, len) && len > 3)
					{
						pName+=len;
						pName-=2;
						pName[0]=drv;
						pName[1]=':';
						break;
					}
				}
			}
		}

//		PR_TRACE((m_pStartUpEnum, prtNOTIFY, "startupenum2\t<ScanProcessesFromDriver> image to ParsEnviroment: '%S'", pName));
//		error=ParsEnviroment(pName,m_sFileName);
//		if (error==ERROR_NOT_ENOUGH_MEMORY)
//			return ERROR_NOT_ENOUGH_MEMORY;
		PR_TRACE((m_pStartUpEnum, prtNOTIFY, "startupenum2\t<ScanProcessesFromDriver> image to scan: '%S'", (wchar_t*)m_sFileName));
//		_IfFileExist(m_sFileName, NULL, false);
		m_sFullFilePathName = pName;
		SendFileFound();
	};
	PR_TRACE((m_pStartUpEnum, prtNOTIFY, "startupenum2\t<ScanProcessesFromDriver> finished"));
	return error;
}

void GetFlag(const tCHAR* chFlag,tDWORD* dwFlag,tDWORD* dwExtFlag)
{
	if (!chFlag|!dwFlag|!dwExtFlag) return;
	if (_strnicmp(chFlag,"FLAG_REG_WITH_SUBKEYS",21)==0) 
	{
		*dwFlag=(*dwFlag)|FLAG_REG_WITH_SUBKEYS;
		return;
	}
	if (_strnicmp(chFlag,"FLAG_DEL_VALUE",14)==0)
	{
		*dwExtFlag=(*dwExtFlag)|FLAG_DEL_VALUE;
		return;
	}
	if (_strnicmp(chFlag,"FLAG_CUT_DATA",13)==0)
	{
		*dwExtFlag=(*dwExtFlag)|FLAG_CUT_DATA;
		return;
	}
	if (_strnicmp(chFlag,"FLAG_RESTORE_DEFAULT",20)==0)
	{
		*dwExtFlag=(*dwExtFlag)|FLAG_RESTORE_DEFAULT;
		return;
	}
	if (_strnicmp(chFlag,"FLAG_ENUM_VALUES",16)==0)
	{
		*dwExtFlag=(*dwExtFlag)|FLAG_ENUM_VALUES;
		return;
	}
	if (_strnicmp(chFlag,"FLAG_DEL_KEY",12)==0)
	{
		*dwExtFlag=(*dwExtFlag)|FLAG_DEL_KEY;
		return;
	}
	if (_strnicmp(chFlag,"FLAG_ENUM_KEYS",14)==0)
	{
		*dwExtFlag=(*dwExtFlag)|FLAG_ENUM_KEYS;
		*dwFlag=(*dwFlag)|FLAG_ENUM_KEYS|FLAG_REG_WITH_SUBKEYS;
		return;
	}
	if (_strnicmp(chFlag,"FLAG_ENUM_WINNT_ONLY",20)==0)
	{
		*dwFlag=(*dwFlag)|FLAG_ENUM_WINNT_ONLY;
		return;
	}
	if (_strnicmp(chFlag,"FLAG_ENUM_WIN9X_ONLY",20)==0)
	{
		*dwFlag=(*dwFlag)|FLAG_ENUM_WIN9X_ONLY;
		return;
	}
	return;
}

tDWORD EnumContext::ParsFlags(const tCHAR* sData,tDWORD* dwExtFlags)
{
	tDWORD			dwFlags=0;
	const tCHAR*	sCurPoint=sData;

	if(!sData)
		return 0;

	sCurPoint--;
	do
	{
		sCurPoint++;
		GetFlag(sCurPoint,&dwFlags,dwExtFlags);
	}
	while ((sCurPoint=strchr(sCurPoint,','))!=NULL);
	return dwFlags;
}

tDWORD ParseIniString(tCHAR* sIniString, const tCHAR** pParams, tDWORD nParamsCount)
{
	tERROR	error=errOK;
	tCHAR* sCurPoint=sIniString;
	tCHAR* pNext = NULL;
	tDWORD	dSize=0;
	tDWORD  nCount = 0;
	if (!pParams) 
		return 0;
	if (!nParamsCount) 
		return 0;
	do {
		if (sCurPoint[0]=='\"') 
		{
			sCurPoint++;
			pNext = sCurPoint;
			while (pNext)
			{
				if (pNext[1]=='|') break;
				if (pNext[1]==0) break;
				pNext++;
				pNext=strchr(pNext,'\"');
			}
		}
		else 
			pNext=strchr(sCurPoint,'|');
		if (*sCurPoint)
		{
			if (pNext)
			{
				*pNext = 0;
				pNext++;
			}
			if (nCount == nParamsCount)
				return nCount;
			pParams[nCount] = sCurPoint;
			nCount++;
		}
		sCurPoint = pNext;
	} while (pNext);
	return nCount;
}

tERROR EnumContext::Hash_Calc_Quick(hash_t* pHash, const wchar_t* str, tDWORD len)
{
	tERROR  error = errOK;
	char	md5hash[16];
	if (len>4)
	{
		if (wcsncmp(str, L"\\\\?\\", 4) == 0
			|| wcsncmp(str, L"\\??\\", 4) == 0) 
		{
			// skip unicode prefix
			str+=4;
			len-=4;
		}
	}
	if (len>4)
	{
		if (!m_hMD5)
			return errOBJECT_NOT_INITIALIZED;
		if (PR_FAIL(error = m_hMD5->Reset()))
			return error;
		if (PR_FAIL(error = m_hMD5->Update((BYTE*)str,len*sizeof(wchar_t))))
			return error;
		if (PR_FAIL(error = m_hMD5->GetHash((unsigned char*)md5hash,16)))
			return error;
		pHash->h1=*(DWORD*)&md5hash[0];
		pHash->h1^=*(DWORD*)&md5hash[8];
		pHash->h2=*(DWORD*)&md5hash[4];
		pHash->h2^=*(DWORD*)&md5hash[12];
	}
	else
	{
		// len <= 4
		pHash->h1=(DWORD)(str[0]);
		if (len<3) 
			pHash->h2 = 0;
		else
			pHash->h2=(DWORD)(str[2]);
	}
	return error;
}

tERROR EnumContext::Hash_AddItem_Quick(PHashTree pHash, const wchar_t* str, tDWORD len)
{
	tERROR  error = errOK;
	hash_t	CRC64;
	if (!pHash)
		return errOBJECT_NOT_INITIALIZED;
	if (PR_FAIL(error = Hash_Calc_Quick(&CRC64, str, len)))
		return error;
	if (errHASH_FOUND == _Hash_AddItem(*m_pStartUpEnum,pHash,CRC64.h1,CRC64.h2,!!m_pPrevContext))
		return errHASH_FOUND;
	return errOK;
}

tERROR EnumContext::Hash_FindItem_Quick(PHashTree pHash, const wchar_t* str, tDWORD len)
{
	tERROR  error = errOK;
	hash_t	CRC64;
	if (!pHash)
		return errOBJECT_NOT_INITIALIZED;
	if (PR_FAIL(error = Hash_Calc_Quick(&CRC64, str, len)))
		return error;
	if (errHASH_FOUND == _Hash_Find(pHash,CRC64.h1,CRC64.h2,NULL))
		return errHASH_FOUND;
	return errOK;
}
