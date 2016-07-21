#include "defines.h"
#include "EnumFunc.h"
#include "Initialization.h"
#include "ParsLineForFileNames.h"

#include <Prague/iface/i_inifile.h>

#include <stdio.h>


extern int _wcscmpc(const wchar_t* sStr1, const wchar_t* sStr2, wchar_t wcStopChar);

tBOOL cRegEnumCtx::ReadKeyValueDword(HKEY BaseKeyEnter,const wchar_t KeyPath[],const wchar_t NameValue[],tDWORD* pDword)
{
	DWORD   RetCode;
	DWORD	dwType;
	DWORD   dwDataSize = sizeof(DWORD);
	tBOOL	Ret=0;
	HKEY    hKey;
	
	if (!pDword) 
		return cFALSE;

	RetCode = pfRegOpenKey(BaseKeyEnter,KeyPath,&hKey);
	if (RetCode != ERROR_SUCCESS)
		return cFALSE;
	RetCode=pfRegQueryValueEx(hKey,NameValue,&dwType,(BYTE*)pDword,&dwDataSize);
	pfRegCloseKey(hKey);
	return (RetCode == ERROR_SUCCESS);
}

tBOOL cRegEnumCtx::ReadKeyValueStr(HKEY BaseKeyEnter, const wchar_t KeyPath[], const wchar_t NameValue[],cPrStrW& sValue, bool NeedStr)
{
	HKEY	AddrHkey=NULL;
	DWORD	lB=sValue.allocated();
	DWORD	dataType;
	tBOOL	bRet = cFALSE;
	
	DWORD RetCode=pfRegOpenKey(BaseKeyEnter,KeyPath,&AddrHkey);
	if (RetCode == ERROR_SUCCESS)
	{
		RetCode=pfRegQueryValueEx(AddrHkey,NameValue,&dataType,(LPBYTE)(wchar_t*)sValue,&lB);
		if (ERROR_MORE_DATA == RetCode || (g_bIsWin9x ? lB + lB : lB)+4 > sValue.allocated())
		{
			if (g_bIsWin9x) 
				lB += lB;
			if (!sValue.reserve(lB+10))
			{
				pfRegCloseKey(AddrHkey);
				return cFALSE;
			}
			RetCode=pfRegQueryValueEx(AddrHkey,NameValue,NULL,(LPBYTE)(wchar_t*)sValue,&lB);
		}
		if (ERROR_SUCCESS == RetCode)
		{
			if (!NeedStr) 
				bRet=cTRUE;
			if ((dataType==REG_SZ)||(dataType==REG_MULTI_SZ)||(dataType==REG_EXPAND_SZ)) 
				bRet=cTRUE;
		}
		pfRegCloseKey(AddrHkey);
	}
	return bRet;
}

tERROR EnumContext::EnumStartUp()
{
	cPrStrW     sStartupFolder;
	cPrStrW     sProfileName;
	cPrStrW     sStrdShellStartUp;
	DWORD		d,retCodeEnum,nEnumErrors=0;
	tBOOL		bSturtUpFolderInited=cFALSE;
	wchar_t		LPathCommon[]=L"Software\\Microsoft\\Windows\\CurrentVersion\\explorer\\Shell Folders";
	wchar_t		LValueCommon[]=L"Common Startup";
	wchar_t		LValueDefault[]=L"Startup";

	PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<EnumStartUp> Parsing folder by <%S> key <%S> value",LPathCommon,LValueCommon));
	if (m_cRegEnumCtx->ReadKeyValueStr(HKEY_LOCAL_MACHINE,LPathCommon,LValueCommon,sStartupFolder)) 
	{
		PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<EnumStartUp> Parsing folder <%S>",(wchar_t*)sStartupFolder));
		ListFilesStartUp(sStartupFolder);
	}
	for (d=0;;d++)
	{
		retCodeEnum=m_cRegEnumCtx->pfRegEnumKey(HKEY_USERS,d,sProfileName);
		if (retCodeEnum==ERROR_NO_MORE_ITEMS) 
			break;
		if (retCodeEnum!=ERROR_SUCCESS)
		{
			if (nEnumErrors>5) 
				break;
			nEnumErrors++;
			continue;
		}
		if (!bSturtUpFolderInited)
			bSturtUpFolderInited=InitFolderForUser(sProfileName,sStrdShellStartUp);
		sProfileName.append_path(LPathCommon);

		PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<EnumStartUp> Parsing folder by <%S> key <%S> value",(wchar_t*)sProfileName,LValueDefault));
		if (m_cRegEnumCtx->ReadKeyValueStr(HKEY_USERS,sProfileName,LValueDefault,sStartupFolder))
		{
			PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<EnumStartUp> Parsing folder <%S>",(wchar_t*)sStartupFolder));
			ListFilesStartUp(sStartupFolder);
		}
	}
	if (!m_pStartUpEnum->m_bRegUsersLoaded && m_pStartUpEnum->pNotLoadedProfilesList && sStrdShellStartUp.getlen())
	{
		for (d=0;d<m_pStartUpEnum->pNotLoadedProfilesList->Count;d++)
		{
			sStartupFolder=(wchar_t*)m_pStartUpEnum->pNotLoadedProfilesList->cProfileName[d];
			sStartupFolder.append_path(sStrdShellStartUp);
			PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<EnumStartUp> Parsing folder <%S>",(wchar_t*)sStartupFolder));
			ListFilesStartUp(sStartupFolder);
		}
	}
	return errOK;
}

tBOOL EnumContext::InitFolderForUser(const wchar_t* sProfileName,cPrStrW& sStartUpSubFolder)
{
	cPrStrW     sStartupFolder;
	cPrStrW		sRegPath;
	wchar_t*	wcsSubFolderPoint=NULL;
	wchar_t		LValue[]=L"startup";
	wchar_t		LProfilePath[]=L"software\\microsoft\\windows\\currentversion\\ProfileReconciliation";
	wchar_t		LProfilePathValue[]=L"ProfileDirectory";

	if (!sProfileName || !_wcsicmp((wchar_t*)sProfileName,L".DEFAULT"))
		return cFALSE;
	sRegPath=sProfileName;
	sRegPath.append_path(L"software\\microsoft\\windows\\currentversion\\explorer\\user shell folders");
	if (m_cRegEnumCtx->ReadKeyValueStr(HKEY_USERS,sRegPath,LValue,sStartupFolder))
	{
		if (g_bIsWin9x)
		{
			DWORD cutNameSize=0;
			if (m_cRegEnumCtx->ReadKeyValueStr(HKEY_CURRENT_USER,LProfilePath,LProfilePathValue,sRegPath)) 
			{
				cutNameSize=sRegPath.getlen();
				if (!wcsncmp(sStartupFolder, sRegPath, cutNameSize))
				{
					wcsSubFolderPoint=(wchar_t*)sStartupFolder+cutNameSize;
					if (wcsSubFolderPoint[0]==L'\\') 
						wcsSubFolderPoint=wcsSubFolderPoint+1;
				}
			}
		}
		else
		{
			wcsSubFolderPoint=(wchar_t*)sStartupFolder;
			wcsSubFolderPoint=wcsrchr(sStartupFolder,L'%');
			if (wcsSubFolderPoint) 
				wcsSubFolderPoint++;
			else 
				wcsSubFolderPoint=(wchar_t*)sStartupFolder;
		}
	}
	if (!wcsSubFolderPoint)
		return cFALSE;
	sStartUpSubFolder=wcsSubFolderPoint;
	return cTRUE;
}

void EnumContext::ListFilesStartUp(const wchar_t* StartUpPath)
{
	tERROR				fRet=warnOBJECT_FALSE;
	WIN32_FIND_DATAW	wfd;
	HANDLE				FFile=INVALID_HANDLE_VALUE;
	tDWORD              pos;

	if (StartUpPath==NULL) 
		return;
	if (StartUpPath[0]==0) 
		return;
	m_SendData.m_sRoot=const_cast<wchar_t*>(StartUpPath);
	m_SendData.m_sSection=NULL;
	m_SendData.m_sValue=NULL;
	if (m_dwFlags&CURE_ACTION_ACTIVE) 
		m_SendData.m_dAction=FileToDel;
	PR_TRACE((m_pStartUpEnum,prtSPAM,"startupenum2\t <ListFilesStartUp> Parsing StartUp folder <%S>",(wchar_t*)StartUpPath));
	cPrStrW sFullFilePathName;
	if (ParsEnviroment(StartUpPath,sFullFilePathName)==ERROR_NOT_ENOUGH_MEMORY)
		return;
	PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t <ListFilesStartUp> Parsing StartUp folder full name <%S>",(wchar_t*)sFullFilePathName));
	pos = sFullFilePathName.getlen();
	if (!pos) 
		return;
	if (!sFullFilePathName.reserve_len(pos + 1 + MAX_PATH + 2))
		return;
	if (sFullFilePathName[pos-1]!='\\') 
	{
		sFullFilePathName[pos] = '\\';
		pos++;
	}
	wcscpy(sFullFilePathName+pos,L"*.*");
	if (pfFindFirstFile(sFullFilePathName,&FFile,&wfd))
	{
		if (pfFindFirstFile(sFullFilePathName,&FFile,&wfd))
		{
			while(pfFindFirstFile(sFullFilePathName,&FFile,&wfd))
			{
				wcscpy(sFullFilePathName+pos,wfd.cFileName);
				m_sFullFilePathName = sFullFilePathName;
				fRet=_IOSFindFile();
			}
		}
		FindClose(FFile);
	}
	return;
}

#ifndef MAX_FEATURE_CHARS  
#define MAX_FEATURE_CHARS  38   // maximum chars in feature name (same as string GUID)
#endif

#define LNK_FLAG_ITEMLIST      0x00000001 // (1<<0)
#define LNK_FLAG_LOCATION      0x00000002 // (1<<1)
#define LNK_FLAG_DESCRIPTION   0x00000004 // (1<<2)
#define LNK_FLAG_RELATIVE_PATH 0x00000008 // (1<<3)
#define LNK_FLAG_WORKING_DIR   0x00000010 // (1<<4)
#define LNK_FLAG_CMD_LINE      0x00000020 // (1<<5)
#define LNK_FLAG_ICON_FILE     0x00000040 // (1<<6)
#define LNK_FLAG_UNICODE       0x00000080 // (1<<7)
#define LNK_FLAG_MSI           0x00005000

#define LNK_LOCATION_FLAG_LOCAL_PATH  0x00000001
#define LNK_LOCATION_FLAG_NET_PATH    0x00000002

const GUID CLSID_ShellLink = {0x00021401L,0,0, { 0xC0,0,0,0,0,0,0,0x46} };

#pragma pack(push, 1)
typedef struct _LNK_HEADER {
	DWORD			dwHeaderSize; // = sizeof(LNK_HEADER) = 0x4C
	GUID			guid;         // CLSID_ShellLink {00021401-0000-0000-C000-000000000046}
	DWORD			dwFlags;
	DWORD			dwFileAttributes;
	FILETIME		ftCreationTime;
	FILETIME		ftModificationTime;
	FILETIME		ftLastAccessTime;
	DWORD			dwFileSize;
	DWORD			dwIconNumber;
	DWORD			dwShowWnd;
	DWORD			dwHotKey;
	LARGE_INTEGER	nUnknown;
} LNK_HEADER,*PLNK_HEADER;

typedef struct _LNK_FILE_LOCATION {
	DWORD	dwSize;
	DWORD	m1;
	DWORD	dwFileLocFlags;
	DWORD	dwLocalVolumeInfoOffset;
	DWORD	dwBasePathNameOffset;
	DWORD	dwNetworkVolumeInfoOffset;
	DWORD	dwRemainPathOffset;
} LNK_FILE_LOCATION,*PLNK_FILE_LOCATION;

typedef struct _LNK_LOCAL_VOLUME_INFO
{
	DWORD   dwSize;
	DWORD   dwTypeOfVolume;
	DWORD   dwVolumeSerialNumber;
	DWORD   dwOffsetOfTheVolumeName;
	//CHAR    dwVolumeLabel[0];
} LNK_LOCAL_VOLUME_INFO;

typedef struct _LNK_NETWORK_VOLUME_INFO
{
	DWORD   dwSize;
	DWORD   dwUnknown; //  (always 2h?)
	DWORD   dwOffsetOfNetworkShareName;
	DWORD   dwUnknown2; // always zero?
	DWORD   dwUnknown3; // always 20000h?
	//CHAR    NetworkShareName[0];
} LNK_NETWORK_VOLUME_INFO;
#pragma pack(pop)

#define W(ptr) (*(WORD*)(ptr))
#define DW(ptr) (*(DWORD*)(ptr))

#define check_buffer_range(buffer_start, buffer_end, data_ptr, data_size) \
	  ((  ((char*)(data_ptr)) >= (char*)(buffer_start)                     \
	  && ((char*)(data_ptr)) <= (char*)(buffer_end)                       \
	  && ((char*)(data_ptr)) + (data_size) >= (char*)(buffer_start)     \
	  && ((char*)(data_ptr)) + (data_size) <= (char*)(buffer_end)       \
      ) ? true : (PR_TRACE((m_pStartUpEnum, prtERROR,"startupenum2\t<ParsWLink> out of buffer (bufsize:0x%X offs:0x%X datasize:0x%X)", (char*)buffer_end-(char*)buffer_start, (char*)data_ptr-(char*)buffer_start, data_size)), false))

#define shift_in_buffer(buffer_start, buffer_end, data_ptr, shift_size, next_data_size) \
	( *(char**)&(data_ptr) = (char*)(data_ptr) + (shift_size), check_buffer_range(buffer_start, buffer_end, data_ptr, next_data_size) )


tERROR _LNKGetString(bool bUnicode, void* buffer, void* buffer_end, char*& pData, hOBJECT hObj, wchar_t** ppStringCopy)
{
	tERROR error;
	tDWORD data_size;
	tDWORD str_len;
	hOBJECT m_pStartUpEnum = hObj;
	if (!check_buffer_range(buffer, buffer_end, pData, sizeof(WORD)))
		return errBUFFER_TOO_SMALL;
	data_size = W(pData);
	str_len = data_size + 1;
	if (bUnicode)
		data_size *= sizeof(wchar_t);
	if (!shift_in_buffer(buffer, buffer_end, pData, sizeof(WORD), data_size))
		return errBUFFER_TOO_SMALL;
	if (ppStringCopy && m_pStartUpEnum)
	{
//INT3;
		if(PR_FAIL(error=m_pStartUpEnum->heapAlloc((tPTR*)ppStringCopy, (str_len + 1) * sizeof(WCHAR))))
			return error;
		memcpy(*ppStringCopy, pData, str_len);
		if (!bUnicode)
			FastAnsi2Uni((char*)*ppStringCopy, *ppStringCopy, str_len);
	}
	if (!shift_in_buffer(buffer, buffer_end, pData, data_size, 0))
		return errBUFFER_TOO_SMALL;
	return errOK;
}

tERROR EnumContext::ParsWLink(const wchar_t* lnkFileName)
{
	tERROR			error		=errOK;
	hIO				hIo;
	tQWORD			qsize		=0;
	tDWORD			size		=0;
	tCHAR*			buffer		=NULL;
	tCHAR*			curoffs		=NULL;
	tCHAR*			endoffs		=NULL;
	tDWORD			CurSize		=0;
	bool bUnicode = false;
	cPrStrW			str;
	
	PR_TRACE((m_pStartUpEnum,prtIMPORTANT,"startupenum2\t<ParsWLink> Parsing Windows link file <%S>",lnkFileName));
	

	error = m_pStartUpEnum->sysCreateObject((hOBJECT*)&hIo, IID_IO, PID_NATIVE_FIO);
	if (PR_FAIL(error))
	{
		PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\t<ParsWLink> Cannot read link file <%S>, %terr", lnkFileName, error));
		return error;
	}
	error = hIo->set_pgOBJECT_FULL_NAME((tPTR)lnkFileName,0,cCP_UNICODE);
	if (PR_SUCC(error))
		error = hIo->set_pgOBJECT_OPEN_MODE(fOMODE_OPEN_IF_EXIST);
	if (PR_SUCC(error))
		error = hIo->set_pgOBJECT_ACCESS_MODE(fACCESS_READ);
	if (PR_SUCC(error))
		error = hIo->sysCreateObjectDone();
	if (PR_SUCC(error))
		error=hIo->GetSize(&qsize,IO_SIZE_TYPE_EXPLICIT);
	size=(tDWORD)qsize;	
	if (PR_SUCC(error))
		error=m_pStartUpEnum->heapAlloc((tPTR*)&buffer,size+sizeof(tWCHAR));
	cAutoPrMemFree _free_buf((hOBJECT)m_pStartUpEnum,(void**)&buffer);
	if(PR_SUCC(error))
		error=hIo->SeekRead(&size,0,buffer,size);
	hIo->sysCloseObject();
	hIo = NULL;
	if (PR_FAIL(error))
	{
		PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\t<ParsWLink> Cannot read link file <%S>, %terr", lnkFileName, error));
		return error;
	}
	
	if (size < sizeof(LNK_HEADER))
		return warnOBJECT_FALSE;


	PLNK_HEADER pLnkHeader=(LNK_HEADER*)buffer;
	if (pLnkHeader->dwHeaderSize != sizeof(LNK_HEADER))
		return warnOBJECT_FALSE;
		
	if ((pLnkHeader->dwFlags & LNK_FLAG_MSI) == LNK_FLAG_MSI || !(pLnkHeader->dwFlags & LNK_FLAG_LOCATION))
	{
		if (!m_pStartUpEnum->m_hMsiLib) 
		{
			if (!InitMsiAPI(&m_pStartUpEnum->m_hMsiLib)) 
			{
				PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\t<ParsWLink> Cannot init MSI API"));
				return errOBJECT_CANNOT_BE_INITIALIZED;
			}
		}
		wchar_t tszComponentCode[MAX_FEATURE_CHARS+1]=L"";
		wchar_t tszProductCode[MAX_FEATURE_CHARS+1]=L"";
		PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\tParsing link (with MSI) <%S>",lnkFileName));
		
		error  = pfMsiGetShortcutTarget(lnkFileName,tszProductCode,tszComponentCode);
		if (ERROR_SUCCESS != error)
		{
			PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\t<ParsWLink> pfMsiGetShortcutTarget failed with win32err=%08X", error));
			return warnOBJECT_FALSE;
		}
		
		DWORD dw = 0;
		error = pfMsiGetComponentPath(tszProductCode,tszComponentCode,NULL,&dw);
		if (0 == dw)
		{
			PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\t<ParsWLink> pfMsiGetComponentPath returned size=0 with win32err=%08X", error));
			return warnOBJECT_FALSE;
		}

		dw+=10;
		if (!m_sFullFilePathName.reserve_len(dw))
			return ERROR_NOT_ENOUGH_MEMORY;
		error = pfMsiGetComponentPath(tszProductCode,tszComponentCode,m_sFullFilePathName,&dw);
		if (ERROR_SUCCESS != error)
		{
			PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\t<ParsWLink> pfMsiGetComponentPath failed with win32err=%08X", error));
			return warnOBJECT_FALSE;
		}
		while (m_sFullFilePathName[m_sFullFilePathName.getlen()-1]==' ') 
			m_sFullFilePathName[m_sFullFilePathName.getlen()-1]=0;
		error=_IOSFindFile();
		m_sFullFilePathName[0]=0;
		return error;
	}

	endoffs=buffer+size;
	curoffs=buffer+pLnkHeader->dwHeaderSize;
	bUnicode = (pLnkHeader->dwFlags & LNK_FLAG_UNICODE) ? true : false;
	if (pLnkHeader->dwFlags & LNK_FLAG_ITEMLIST)
	{
		if (!check_buffer_range(buffer, endoffs, curoffs, sizeof(WORD)))
			return warnOBJECT_FALSE;
		if (!shift_in_buffer(buffer, endoffs, curoffs, W(curoffs) + sizeof(WORD), 0))
			return warnOBJECT_FALSE;
	}
	if (!check_buffer_range(buffer, endoffs, curoffs, sizeof(LNK_FILE_LOCATION)))
		return warnOBJECT_FALSE;
	PLNK_FILE_LOCATION pLocation=(PLNK_FILE_LOCATION)curoffs;
	if (0 == (pLocation->dwFileLocFlags & (LNK_LOCATION_FLAG_LOCAL_PATH | LNK_LOCATION_FLAG_NET_PATH)))
		return warnOBJECT_FALSE;
	if (!shift_in_buffer(buffer, endoffs, curoffs, pLocation->dwSize, 0))
		return warnOBJECT_FALSE;
//INT3;
	if (pLnkHeader->dwFlags & LNK_FLAG_DESCRIPTION) // Description string
	{
		if (PR_FAIL(error = _LNKGetString(bUnicode, buffer, endoffs,/* (wchar_t*)*/curoffs, NULL, NULL)))
			return error;
	}
	if (pLnkHeader->dwFlags & LNK_FLAG_RELATIVE_PATH) // Relative path string
	{
		if (PR_FAIL(error = _LNKGetString(bUnicode, buffer, endoffs,/* (wchar_t*)*/curoffs, NULL, NULL)))
			return error;
	}
	m_pCurrentWorkingDir = NULL;
	cAutoPrMemFree _free_work_dir((hOBJECT)m_pStartUpEnum,(void**)&m_pCurrentWorkingDir);
	if (pLnkHeader->dwFlags & LNK_FLAG_WORKING_DIR) // Working directory
	{
		if (PR_FAIL(error = _LNKGetString(bUnicode, buffer, endoffs,/* (wchar_t*)*/curoffs, *m_pStartUpEnum, &m_pCurrentWorkingDir)))
			return error;
	}
	
	if (pLocation->dwFileLocFlags & LNK_LOCATION_FLAG_LOCAL_PATH)
	{
		if (!check_buffer_range(buffer, endoffs, (char*)pLocation+pLocation->dwBasePathNameOffset, 0))
			return warnOBJECT_FALSE;
		str = (char*)pLocation + pLocation->dwBasePathNameOffset;
//		m_sFileName = (char*)pLocation + pLocation->dwBasePathNameOffset;
	}
	else
	{
		if (!check_buffer_range(buffer, endoffs, (char*)pLocation+pLocation->dwNetworkVolumeInfoOffset, sizeof(LNK_NETWORK_VOLUME_INFO)))
			return warnOBJECT_FALSE;
		LNK_NETWORK_VOLUME_INFO* pNetworkVolumeInfo = (LNK_NETWORK_VOLUME_INFO*)((char*)pLocation+pLocation->dwNetworkVolumeInfoOffset);
		if (!check_buffer_range(buffer, endoffs, (char*)pNetworkVolumeInfo+pNetworkVolumeInfo->dwOffsetOfNetworkShareName, 1))
			return warnOBJECT_FALSE;
		str = (char*)pNetworkVolumeInfo+pNetworkVolumeInfo->dwOffsetOfNetworkShareName;
		str.append(L"\\");
//		m_sFileName = (char*)pNetworkVolumeInfo+pNetworkVolumeInfo->dwOffsetOfNetworkShareName;
//		m_sFileName.append(L"\\");
	}
	if (!check_buffer_range(buffer, endoffs, (char*)pLocation+pLocation->dwRemainPathOffset, 1))
		return warnOBJECT_FALSE;
	str.append((char*)pLocation+pLocation->dwRemainPathOffset);
//	m_sFileName.append((char*)pLocation+pLocation->dwRemainPathOffset);
	error=IfFileExist(str);
//	error=IfFileExist(m_sFileName);
	if ((error==warnOBJECT_INFECTED) || (error==errOPERATION_CANCELED))
		return error;
	if (pLnkHeader->dwFlags & LNK_FLAG_CMD_LINE)
	{
		if (!check_buffer_range(buffer, endoffs, curoffs, sizeof(WORD)))
			return warnOBJECT_FALSE;
		tDWORD data_size = W(curoffs)*sizeof(WCHAR)+2;
		if (!shift_in_buffer(buffer, endoffs, curoffs, sizeof(WORD), data_size))
			return warnOBJECT_FALSE;
//		if (m_sFileName.reserve(data_size + sizeof(WCHAR)))
		if (str.reserve(data_size + sizeof(WCHAR)))
		{
//			memset((wchar_t*)m_sFileName,0,m_sFileName.allocated_len());
			memset((wchar_t*)str,0,str.allocated());
			if (bUnicode) 
				memcpy((wchar_t*)str, curoffs, data_size);
//				memcpy(m_sFileName, curoffs, data_size);
			else
			{
//				memcpy(m_sFileName, curoffs, data_size/sizeof(WCHAR));
				memcpy((wchar_t*)str, curoffs, data_size/sizeof(WCHAR));
//				FastAnsi2Uni((char*)((wchar_t*)m_sFileName),(wchar_t*)m_sFileName,-1);
				FastAnsi2Uni((char*)((wchar_t*)str),(wchar_t*)str,-1);
			}
//			m_sFileName[data_size/sizeof(WCHAR)] = 0;
			str[data_size/sizeof(WCHAR)] = 0;
//			error=IfFileExist(m_sFileName);
			error=IfFileExist(str);
			if ((error==warnOBJECT_INFECTED) || (error==errOPERATION_CANCELED))
				return error;
		}
		else
			error=ERROR_NOT_ENOUGH_MEMORY;
	}

	return error;
}

tERROR EnumContext::CheckTMCommandLine(ITaskScheduler*	pITS, const wchar_t* wsTaskName, const wchar_t* wsCmdLine)
{
	PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<EnumTaskManager> Task application path <%S>",wsCmdLine));
	tERROR fFindRet = IfFileExist(wsCmdLine);

	if (warnOBJECT_INFECTED != fFindRet)
		return fFindRet;
	if (!(m_dwFlags & CURE_ACTION_ACTIVE))
		return fFindRet;

	HRESULT hr=pITS->Delete(wsTaskName);
	if (FAILED(hr))
	{
		if (errOPERATION_CANCELED == SendError(wsTaskName, NULL, NULL, OBJECT_TASK_MANAGER_TYPE, hr))
			return errOPERATION_CANCELED;
		return warnOBJECT_INFECTED;
	}

	m_SendData.m_sRoot=const_cast<wchar_t*>(wsTaskName);
	m_SendData.m_sSection=m_pCurrentWorkingDir;
	m_SendData.m_sValue=const_cast<wchar_t*>(wsCmdLine);
	m_SendData.m_dAction=DelData;
	if (errOPERATION_CANCELED == SendFileFound()) 
		return errOPERATION_CANCELED;
	InterlockedIncrement(&m_pStartUpEnum->m_nModifyCounter);
	return warnOBJECT_DELETED;
}


tERROR EnumContext::EnumTaskManager()
{
	LPWSTR*			ppNames=NULL;

	ITaskScheduler*	pITS=NULL;
	IEnumWorkItems*	pIEnum=NULL;
	ITask*			pITask=NULL;
	HRESULT			hr;

	if (!InitTaskManager(&pITS)) 
		return errPARAMETER_INVALID;
	if (!pITS) 
		return errHANDLE_INVALID;
	cAutoRelease _release_ITS(pITS);

	hr=pITS->Enum(&pIEnum);
	if (FAILED(hr)||(!pIEnum)) 
	{
		PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<EnumTaskManager> error Enum %X",hr));
		return errOK;
	}
	cAutoRelease _release_enum(pIEnum);

	do {
		hr=pIEnum->Next(1,&ppNames,NULL);
		if(hr != S_OK)
			break;
		else
		{
			tERROR		fRet;
			wchar_t*	lpwszApplicationName = NULL;
			wchar_t*	lpwszParams = NULL;
			cAutoCoTaskMemFreeArray _free_names(&ppNames, 1);
			cAutoCoTaskMemFree _free_work_dir(&m_pCurrentWorkingDir);
			cAutoCoTaskMemFree _free_params(&lpwszParams);
			cAutoCoTaskMemFree _free_app_name(&lpwszApplicationName);

			hr=pITS->Activate(*ppNames,IID_ITask,(IUnknown**)&pITask);
			if (FAILED(hr)) 
				continue;
			m_pCurrentWorkingDir = NULL; // to be sure
			hr=pITask->GetWorkingDirectory(&m_pCurrentWorkingDir);
			hr=pITask->GetApplicationName(&lpwszApplicationName);
			hr=pITask->GetParameters(&lpwszParams);
			pITask->Release();
			
			fRet = CheckTMCommandLine(pITS, *ppNames, lpwszApplicationName);
			if (errOPERATION_CANCELED == fRet) 
				return fRet;
			if (warnOBJECT_DELETED == fRet)
				continue;
			fRet = CheckTMCommandLine(pITS, *ppNames, lpwszParams);
			if (errOPERATION_CANCELED == fRet) 
				return fRet;
		}
	} while(true);
	return errOK;
}


tERROR EnumContext::CheckHosts()
{
	tERROR			fRet=errOK;
	
	//check by MD5 if we scanned this file?
	m_SendData.m_ObjType=OBJECT_HOSTS_TYPE;
	if (g_bIsWin9x) 
	{
		do {
			fRet=HostsFileAnalyze(L"%SystemRoot%\\hosts.sam");
		} while (PR_SUCC(fRet) && NeedRescan());
		return fRet;
	}

	// WinNT
	fRet=m_cRegEnumCtx->RegEnumAW("HKLM\\SYSTEM\\ControlSet???\\Services\\Tcpip\\Parameters","DataBasePath",0,this,RegEnumCallbackHosts,RegEnumCallbackErrMessage);
	if (fRet==errOPERATION_CANCELED) 
		return fRet;
	do {
		fRet=HostsFileAnalyze(L"%SystemRoot%\\system32\\drivers\\etc\\hosts");
	} while (PR_SUCC(fRet) && NeedRescan());
	return fRet;
}

long __stdcall RegEnumCallbackHosts(LPVOID pEnumCallbackContext, HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType)
{
	EnumContext*	pContext=(EnumContext*)pEnumCallbackContext;
	return pContext->iRegEnumCallbackHosts(pEnumCallbackContext, hKey, sPath, sName, pValue, dwSize, dwType);
}

long EnumContext::iRegEnumCallbackHosts(LPVOID pEnumCallbackContext, HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType)
{
	tERROR					OpRet=ERROR_SUCCESS;
	wchar_t*				pData=(wchar_t*)pValue;

	if (dwSize==0) 
		return ERROR_REGOP_TRUE;
	if (pData || *pData == 0)
		return ERROR_REGOP_TRUE;

	m_sFileName = pData;
	m_sFileName.append_path(L"hosts");
	do {
		OpRet=HostsFileAnalyze(m_sFileName);
	} while (PR_SUCC(OpRet) && NeedRescan());
	if (OpRet==errOPERATION_CANCELED) 
		return ERROR_REGOP_CANCELED;
	return ERROR_REGOP_TRUE;
}

tERROR EnumContext::HostsFileAnalyze(const wchar_t* sFileName)
{
	tERROR		error=errOK;
	wchar_t*	point=NULL;
	wchar_t*	npoint=NULL;
	wchar_t		savech=0;

	hIO			hIo;
	tQWORD		qsize=0;
	tDWORD		size=0,i;
	tCHAR*		buffer=NULL;
	tDWORD		dRealRead=0;
	tDWORD		fpointer=0;
	tDWORD		readed=0;
	tDWORD		addition=0;
	tDWORD		dDataLen=0;
	tCHAR*		chLine=NULL;
	tCHAR*		chNext=NULL;
	tBOOL		bCure=0;
	tBOOL		bSendMsg=0;
	bool        bLocalHost = false;

	m_SendData.m_sRoot=NULL;
	m_SendData.m_sSection=NULL;
	m_SendData.m_sValue=NULL;
	
	error = IfFileExist(sFileName, false, true);
	if ((PR_FAIL(error))||(error==warnOBJECT_FALSE)) 
		return error;


	PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<HostsFileAnalyze> check update possibility <%S>",(wchar_t*)m_sFullFilePathName));

	error = m_pStartUpEnum->sysCreateObject((hOBJECT*)&hIo,IID_IO,PID_NATIVE_FIO);
	if (PR_SUCC(error))
		error = hIo->set_pgOBJECT_FULL_NAME(m_sFullFilePathName,0,cCP_UNICODE);
	if (PR_FAIL(error))
	{
		PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot create IO object for <%S> file, %terr",sFileName,error));
		return error;
	}
	cAutoObj<cIO> _close_io(hIo);
	cAutoPrMemFree _free_buff(*m_pStartUpEnum, (void**)&buffer);
	
	// locked op
	{
		cAutoNativeCS _lock(&m_pStartUpEnum->m_sCommonCriticalSection);
		error = hIo->sysCreateObjectDone();
		if(PR_SUCC(error))
			error=CALL_IO_GetSize(hIo,&qsize,IO_SIZE_TYPE_EXPLICIT);
		size=(tDWORD)qsize;	
		if(PR_SUCC(error))
			error=CALL_SYS_ObjHeapAlloc(m_pStartUpEnum,(tPTR*)&buffer,size+1);
		if(PR_SUCC(error))
			error=CALL_IO_SeekRead(hIo,&dRealRead,fpointer,buffer,size);
	}
	if(PR_FAIL(error))
	{
		PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot done IO object for <%S> file, %terr",sFileName,error));
		return error;
	}

	buffer[size]=0;
	chNext=buffer;
	chLine=buffer;
	while (readed=GetLine(chLine,&chNext,NULL,NULL))
	{
		tBOOL	bMissAdd=0;
		addition=0;
		if ((chLine[0]!=0)&(chLine[0]!='#'))
		{
			if (m_pStartUpEnum->m_HostsStrings.nCount !=0)
			{
				while (chLine[0]==' ' || chLine[0]=='\t') chLine=chLine+1;
				if (_strnicmp(chLine,"127.",4)==0)
				{
					tBOOL		bDetect=0;
					for (i=0;i<m_pStartUpEnum->m_HostsStrings.nCount;i++)
					{
						if (strstr(chLine,m_pStartUpEnum->m_HostsStrings.ppLinks[i])!=0)
						{
							bDetect=1;
							break;
						}
					}
					if (!bDetect)
					{
						if (strstr(chLine,"localhost")!=0)
							bLocalHost = true;
					}
					else //if (bDetect)
					{
						if (!bCure)
						{
							m_SendData.m_dAction=SuspiciousDetect;
							error=SendFileFound();
							m_SendData.m_dAction=ScanAct;
							if (error!=errOK_DECIDED) 
								break;
							bCure=1;
							if (m_bAdvancedDisinfection)
							{
								cStringObj name(sFileName);
								tERROR error = m_pStartUpEnum->sysSendMsg(pmc_ADVANCED_DISINFECTION, pm_ADVANCED_DISINFECTION_LOCK_FILE, (hOBJECT)cAutoString(name), NULL, NULL);
								PR_TRACE((m_pStartUpEnum,prtIMPORTANT,"startupenum2\tNotify pm_ADVANCED_DISINFECTION_LOCK_FILE <%S>, result %terr", name.data(), error));
							}
							EnterCriticalSection(&m_pStartUpEnum->m_sCommonCriticalSection);
						}
						if (bCure)
						{
							tCHAR* ToSaveBuf=NULL;
							wchar_t* sToCopy=NULL;
							tERROR	OpRet=errOK;
							if (size-fpointer-readed!=0)
							{
								if(PR_SUCC(OpRet=CALL_SYS_ObjHeapAlloc(hIo,(tPTR*)&ToSaveBuf,size-fpointer-readed+1))) 
								{
									if(PR_SUCC(OpRet=CALL_IO_SeekRead(hIo,&dRealRead,fpointer+readed,ToSaveBuf,size-fpointer-readed)))
									{
										ToSaveBuf[size-fpointer-readed]=0;
										tDWORD old_accessmode=hIo->get_pgOBJECT_ACCESS_MODE();
										hIo->set_pgOBJECT_ACCESS_MODE(old_accessmode | fACCESS_WRITE);
										if (ToSaveBuf[0]==0x0a) 
										{
											addition=1;
											sToCopy=(wchar_t*)(ToSaveBuf+1);
										}
										else 
											sToCopy=(wchar_t*)ToSaveBuf;
										OpRet=CALL_IO_SeekWrite(hIo,&dRealRead,fpointer,sToCopy,size-fpointer-readed-addition);
										if(PR_FAIL(OpRet)) 
											PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot write old data, %terr",OpRet));
										else 
										{
											size=size-readed-addition;
											if (addition) 
												fpointer--;
											bSendMsg=1;
											bMissAdd=1;
										}
										hIo->set_pgOBJECT_ACCESS_MODE(old_accessmode);
									}
									else 
										PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot read file, %terr",OpRet));
									CALL_SYS_ObjHeapFree(hIo,(tPTR)ToSaveBuf);
									ToSaveBuf=NULL;
								}
								else 
									PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\tCannot allocate memory, %terr",OpRet));
							}
							else
							{
								size=size-readed;
								bSendMsg=1;
								bMissAdd=1;
							}
						}
					}
				}
			}
		}
		if (!bMissAdd) 
			fpointer=fpointer+readed+addition;
		chLine=chNext;
		if (!chLine) 
			break;
	}
	if (bCure)
	{
		if (bSendMsg)
		{
			tDWORD old_accessmode=hIo->get_pgOBJECT_ACCESS_MODE();
			hIo->set_pgOBJECT_ACCESS_MODE(old_accessmode|fACCESS_WRITE);
			error=hIo->SetSize(size);
			if (!bLocalHost)
			{
				const tCHAR* sLocalHost = "\r\n127.0.0.1\tlocalhost\r\n";
				hIo->SeekWrite(NULL, size, (tPTR)sLocalHost, strlen(sLocalHost));
			}
			hIo->set_pgOBJECT_ACCESS_MODE(old_accessmode);
			ActionType dAction=m_SendData.m_dAction;
			m_SendData.m_dAction=DelData;
			error=SendFileFound();
			m_SendData.m_dAction=dAction;
			InterlockedIncrement(&m_pStartUpEnum->m_nModifyCounter);
		}
		LeaveCriticalSection(&m_pStartUpEnum->m_sCommonCriticalSection);
		// notify PDM about hosts file changes
		m_pStartUpEnum->sysSendMsg(pmc_STARTUP_CURE_MESSAGES, pm_STARTUP_HOSTS_MODIFIED, 0, 0, 0);
	}
	return error;
}

tERROR EnumContext::LSPEnum()
{
	tERROR		error;
	LSP_CONTEXT	LSPContext;
	tWCHAR* pLspKey = L"HKLM\\SYSTEM\\ControlSet???\\Services\\WinSock2\\Parameters";
	m_SendData.m_ObjType=OBJECT_REGISTRY_TYPE;
	LSPContext.pContext=this;
	LSPContext.dType=0;
	LSPContext.NeedToRestoreCount=0;
	if (m_bAdvancedDisinfection)
	{
		cStringObj name(pLspKey);
		name.append(L"\\*");
		tERROR error = m_pStartUpEnum->sysSendMsg(pmc_ADVANCED_DISINFECTION, pm_ADVANCED_DISINFECTION_LOCK_REGKEY, (hOBJECT)cAutoString(name), NULL, NULL);
		PR_TRACE((m_pStartUpEnum,prtIMPORTANT,"startupenum2\tNotify pm_ADVANCED_DISINFECTION_LOCK_REGKEY <%S>, result %terr", name.data(), error));
	}
	error=m_cRegEnumCtx->RegEnumWW(pLspKey,L"Current_Protocol_Catalog",0,(LPVOID)&LSPContext,RegEnumCallbackLSP,RegEnumCallbackErrMessageLSP);
	LSPContext.dType=1;
	LSPContext.NeedToRestoreCount=0;
	error=m_cRegEnumCtx->RegEnumWW(pLspKey,L"Current_NameSpace_Catalog",0,(LPVOID)&LSPContext,RegEnumCallbackLSP,RegEnumCallbackErrMessageLSP);
	return error; 
}

long __stdcall	RegEnumCallbackLSP(LPVOID pEnumCallbackContext, HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType)
{
	pLSP_CONTEXT            pLspContext = (pLSP_CONTEXT)pEnumCallbackContext;
	EnumContext*	pContext=pLspContext->pContext;
	return pContext->iRegEnumCallbackLSP(pEnumCallbackContext, hKey, sPath, sName, pValue, dwSize, dwType);
}

long EnumContext::iRegEnumCallbackLSP(LPVOID pEnumCallbackContext, HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType)
{
	tERROR					error=ERROR_SUCCESS;
	pLSP_CONTEXT            pLspContext = (pLSP_CONTEXT)pEnumCallbackContext;
	wchar_t*				pData=(wchar_t*)pValue;
	cPrStrW                 sKeyName;
	DWORD					dCountSize=0;
	
//INT3;
	if (dwSize==0) 
		return ERROR_REGOP_TRUE;

	sKeyName = sPath;
	sKeyName.append_path(pData);

	pLspContext->dValueCount=0;
	if (m_dwFlags&CURE_ACTION_ACTIVE)
	{
		cAutoNativeCS _lock(&m_pStartUpEnum->m_sCommonCriticalSection);
		if (m_cRegEnumCtx->ReadKeyValueDword(HKEY_LOCAL_MACHINE, sKeyName+5, L"Num_Catalog_Entries", &pLspContext->dValueCount))
		{
			if (0 == pLspContext->dValueCount)
				return ERROR_REGOP_TRUE;
		}
	}
	sKeyName.append_path(L"Catalog_Entries\\*");
	PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<LSPEnum> Enum key <%S>",(wchar_t*)sKeyName));
	
	if (pLspContext->dType==0) 
		error=m_cRegEnumCtx->RegEnumWW(sKeyName,L"PackedCatalogItem",0/*REG_RESTORE_LSP|REG_BINARY_VALUE*/,pEnumCallbackContext,RegEnumCallbackLSP2,RegEnumCallbackErrMessageLSP);
	else 
		error=m_cRegEnumCtx->RegEnumWW(sKeyName,L"LibraryPath",0/*REG_RESTORE_LSP*/,pEnumCallbackContext,RegEnumCallbackLSP2,RegEnumCallbackErrMessageLSP);
	
	if (pLspContext->NeedToRestoreCount)
	{
		HKEY	AddrHkey=NULL;
		if (m_cRegEnumCtx->pfRegOpenKey(hKey,pData,&AddrHkey)==ERROR_SUCCESS)
		{
			LONG OpRes = m_cRegEnumCtx->pfRegSetValueEx(AddrHkey,L"Num_Catalog_Entries",REG_DWORD,(LPBYTE)&pLspContext->dValueCount,sizeof(DWORD));
			PR_TRACE((m_pStartUpEnum,prtNOTIFY,"startupenum2\t<LSPEnum> RestoreCount <%S> <%S> %d res=%d",(wchar_t*)sKeyName, pData, pLspContext->dValueCount, OpRes));
			m_cRegEnumCtx->pfRegCloseKey(AddrHkey);
			pLspContext->NeedToRestoreCount=0;
		}
		else
		{
			PR_TRACE((m_pStartUpEnum,prtERROR,"startupenum2\t<LSPEnum> RestoreCount failed <%S> <%S> %d",(wchar_t*)sKeyName, pData, pLspContext->dValueCount));
		}
	}

	if (error==errOPERATION_CANCELED) 
		return ERROR_REGOP_CANCELED;
	return ERROR_REGOP_TRUE;
}

long __stdcall	RegEnumCallbackErrMessageLSP(LPVOID pEnumCallbackContext, const wchar_t* sPath, const wchar_t* sName, long dError, bool Error)
{
	pLSP_CONTEXT    pLspContext = (pLSP_CONTEXT)pEnumCallbackContext;
	EnumContext*	pContext=pLspContext->pContext;
	return pContext->iRegEnumCallbackErrMessageLSP(pEnumCallbackContext, sPath, sName, dError, Error);
}

long EnumContext::iRegEnumCallbackErrMessageLSP(LPVOID pEnumCallbackContext, const wchar_t* sPath, const wchar_t* sName, long dError, bool Error)
{
	pERR_MESSAGE_DATA	pMess=NULL;
	LONG				OpRet;
	if (Error)
	{
		OpRet = SendError(sPath, NULL, sName, OBJECT_REGISTRY_TYPE, dError);
		if (OpRet==errOPERATION_CANCELED) 
			return ERROR_REGOP_CANCELED;
	}
	else
	{
		m_SendData.m_sRoot=const_cast<wchar_t*>(sPath);
		m_SendData.m_sValue=const_cast<wchar_t*>(sName);
		OpRet=SendFileFound();
		if (OpRet==errOPERATION_CANCELED) 
			return ERROR_REGOP_CANCELED;
	}
	return ERROR_REGOP_TRUE;
}


tBOOL cRegEnumCtx::pfRegDeleteKeyByName(const wchar_t* sLastKeyName)
{
	const wchar_t* sSubKey;
	HKEY hRoot = GetRegistryRoot(sLastKeyName, NULL, &sSubKey);
	if (!hRoot)
		return cFALSE;
	return (ERROR_SUCCESS == pfRegDeleteKey(hRoot, sSubKey));
}


tERROR cRegEnumCtx::pfRegRenameKey(pLSP_CONTEXT pLSPContext, const wchar_t* sFromKey, const wchar_t* sToKey)
{
	tERROR	error=errOK;
	pLSPContext->sCopy2Key = sToKey;
	pLSPContext->nOrigKeyPathLen = wcslen(sFromKey);
	error = RegEnumWW(sFromKey,L"*",FLAG_REG_WITH_SUBKEYS,(LPVOID)pLSPContext,RegEnumCallbackKeyRename,RegEnumCallbackErrMessageLSP);
	if (PR_FAIL(error))
		return error;
	if (!pfRegDeleteKeyByName(sFromKey))
		return errUNEXPECTED;
	return errOK;
}

long __stdcall RegEnumCallbackLSP2(LPVOID pEnumCallbackContext, HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType)
{
	pLSP_CONTEXT    pLspContext = (pLSP_CONTEXT)pEnumCallbackContext;
	EnumContext*	pContext=pLspContext->pContext;
	return pContext->iRegEnumCallbackLSP2(pEnumCallbackContext, hKey, sPath, sName, pValue, dwSize, dwType);
}

long EnumContext::iRegEnumCallbackLSP2(LPVOID pEnumCallbackContext, HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType)
{
	tERROR					error=ERROR_SUCCESS;
	pLSP_CONTEXT            pLspContext = (pLSP_CONTEXT)pEnumCallbackContext;
	tDWORD					dSize=0;
	cPrStrW					str;
	

	if (dwSize==0) 
		return ERROR_REGOP_TRUE;
	m_SendData.m_sRoot=const_cast<wchar_t*>(sPath);
	m_SendData.m_sSection=NULL;
	m_SendData.m_sValue=const_cast<wchar_t*>(sName);

	if (pLspContext->dType==0) 
		str = (char*)pValue; 
//		m_sFileName = (char*)pValue; 
	else 
		str = (wchar_t*)pValue; 
//		m_sFileName = (wchar_t*)pValue; 
	error=IfFileExist(str);
//	error=IfFileExist(m_sFileName);
	if (0 == (m_dwFlags&CURE_ACTION_ACTIVE))
	{
		if (NeedRescan()) 
			return ERROR_REGOP_RECOUNT_KEY;
		return ERROR_REGOP_TRUE;
	}
	PR_TRACE((m_pStartUpEnum,prtIMPORTANT,"startupenum2\tiRegEnumCallbackLSP2: IfFileExist(%S) result %terr", (wchar_t*)str, error));
	if (error==warnOBJECT_INFECTED)
	{
		cPrStrW sLastKeyName;
		m_SendData.m_dAction=DelKey;
				
		tBOOL bRes = m_cRegEnumCtx->pfRegDeleteKeyByName(sPath);
		PR_TRACE((m_pStartUpEnum,prtIMPORTANT,"startupenum2\tiRegEnumCallbackLSP2: DeleteKeyByName %S, res=%d", sPath, bRes));
		
		if (!sLastKeyName.reserve_len(wcslen(sPath) + 32))
			return ERROR_NOT_ENOUGH_MEMORY;
		sLastKeyName = sPath;
		wchar_t* endpoint=wcsrchr(sLastKeyName,'\\');
		const wchar_t* endpoint2=wcsrchr(sPath,'\\');
		PR_TRACE((m_pStartUpEnum,prtIMPORTANT,"startupenum2\tiRegEnumCallbackLSP2: endpoint=%S", endpoint ? endpoint : L"<NULL>"));
		if (endpoint && endpoint2)
		{
			//endpoint[1]=0;
			swprintf(endpoint+1,L"%.12d",pLspContext->dValueCount);
			PR_TRACE((m_pStartUpEnum,prtIMPORTANT,"startupenum2\tiRegEnumCallbackLSP2: cmp(%S, %S)", endpoint+1,endpoint2+1));
			if (_wcsicmp(endpoint+1,endpoint2+1)==0) 
			{
				pLspContext->dValueCount--;
				pLspContext->NeedToRestoreCount=1;
				PR_TRACE((m_pStartUpEnum,prtIMPORTANT,"startupenum2\tiRegEnumCallbackLSP2: Last - %S, %S, new value=%d, name=%S", endpoint, sPath, pLspContext->dValueCount, sName));
				InterlockedIncrement(&m_pStartUpEnum->m_nModifyCounter);
				return ERROR_REGOP_TRUE;
			}
			cAutoNativeCS _lock(&m_pStartUpEnum->m_sCommonCriticalSection);
			error=m_cRegEnumCtx->pfRegRenameKey(pLspContext, sLastKeyName, sPath);
			if (PR_SUCC(error))
			{
				HKEY	AddrHkey=NULL;
				pLspContext->dValueCount--;
				pLspContext->NeedToRestoreCount=1;
				m_SendData.m_sRoot=const_cast<wchar_t*>(sPath);
				m_SendData.m_sSection=NULL;
				m_SendData.m_sValue=const_cast<wchar_t*>(sName);
				error=SendFileFound();

				error=RegEnumCallbackErrMessageLSP(pEnumCallbackContext,sLastKeyName,endpoint+1,error,error != ERROR_SUCCESS);
				InterlockedIncrement(&m_pStartUpEnum->m_nModifyCounter);
				if ( error == ERROR_REGOP_FALSE || error==ERROR_REGOP_CANCELED ) 
					return error;
				return ERROR_REGOP_RECOUNT_KEY; //possible endless loop, possible extra message of deleting key
			}
		}
		else
			error = errUNEXPECTED;
		error = SendError(sPath, NULL, sName, OBJECT_REGISTRY_TYPE, error);
		if (error == errOPERATION_CANCELED) 
			return ERROR_REGOP_CANCELED;
	}
	return ERROR_REGOP_TRUE;
}


long __stdcall RegEnumCallbackKeyRename(LPVOID pEnumCallbackContext, HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType)
{
	tERROR			error=ERROR_SUCCESS;
	pLSP_CONTEXT	pLSPContext=(pLSP_CONTEXT)pEnumCallbackContext;
	cRegEnumCtx*	pRegEnumContext=pLSPContext->pContext->m_cRegEnumCtx;
	cPrStrW sNewKey;
	HKEY hKeyNew;
	const wchar_t* sSubKey;
	HKEY hRoot;
	
	sNewKey = pLSPContext->sCopy2Key;
	sNewKey.append_path(sPath + pLSPContext->nOrigKeyPathLen);
	hRoot = pRegEnumContext->GetRegistryRoot(sNewKey, NULL, &sSubKey);
	if (!hRoot)
		return ERROR_REGOP_FALSE;
	error=pRegEnumContext->pfRegCreateKey(hRoot, sSubKey, &hKeyNew);
	if (error!=ERROR_SUCCESS) 
		return ERROR_REGOP_FALSE;
	error=pRegEnumContext->pfRegSetValueEx(hKeyNew,sName,dwType,pValue,dwSize);
	pRegEnumContext->pfRegCloseKey(hKeyNew);
	if (error!=ERROR_SUCCESS) 
		return ERROR_REGOP_FALSE;
	return ERROR_REGOP_TRUE;
}
