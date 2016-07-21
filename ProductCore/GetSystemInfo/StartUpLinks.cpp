#include "stdafx.h"

//#include <objbase.h>
#include <shlobj.h>
//#include <shfolder.h>
#include "StartUpLinks.h"
#include "MemoryScan.h"
//#include <Shobjidl.h>

extern FILE*				LFile;
extern SI_INSIDE_OS_NUMBER	os;

typedef void (*m_pGetStartUpMenuFunction)(/*LPCTSTR StartUpPath*/);
typedef UINT (WINAPI *lpMsiGetShortcutTarget)(LPCSTR szShortcutPath,LPSTR szProductCode,LPSTR szFeatureId,LPSTR szComponentCode);
typedef INSTALLSTATE (WINAPI *lpMsiGetComponentPath)(LPCSTR szProduct,LPCSTR szComponent,LPSTR lpPathBuf,DWORD *pcchBuf);

//typedef UINT (*lpMsiGetShortcutTarget)(LPCTSTR szShortcutTarget,LPTSTR szProductCode,LPTSTR szFeatureId,LPTSTR szComponentCode,);
//typedef INSTALLSTATE (*lpMsiGetComponentPath)(LPCTSTR szProduct,LPCTSTR szComponent,LPTSTR lpPathBuf,DWORD *pcchBuf);


#define STARTUP_LINK_COUNT	32
#define MAX_FEATURE_CHARS	38

lpMsiGetShortcutTarget		pMsiGetShortcutTarget=NULL;
lpMsiGetComponentPath		pMsiGetComponentPath=NULL;

DWORD WriteStartUpInfo(HWND ProgressItemHWnd,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
static	StartUpObjectList	StartUpTable[STARTUP_LINK_COUNT]={
		//OS_TYPE	ACTION_TYPE			Function									STRING																	PARAM
//		{OS_ALL,	ROOT_ADD,			NULL,										"INI",																	PARAM_NOTHING},														//
		{OS_9X,		PATH_SAVE_RET,		/*NULL,										*/"win.ini",															PARAM_NOTHING},														//0
//		{OS_9X,		SUBROOT_ADD,		NULL,										"",																		ADD_FROM_SAVE},//from PATH_SAVE_RET									//
		{OS_9X,		INI_ADD,			/*NULL,										*/"Windows",															SUBITEM_NEEDED},													//1
		{OS_ALL,	PATH_SAVE_RET,		/*NULL,										*/"system.ini",															PARAM_NOTHING},														//2
//		{OS_ALL,	SUBROOT_ADD,		NULL,										"",																		ADD_FROM_SAVE},//from PATH_SAVE_RET									//
		{OS_9X,		INI_ADD,			/*NULL,										*/"boot",																SUBITEM_NEEDED},													//3
		{OS_ALL,	INI_ADD,			/*NULL,										*/"386Enh",																SUBITEM_NEEDED},													//4
		{OS_ALL,	INI_ADD,			/*NULL,										*/"drivers",															SUBITEM_NEEDED},													//5
		{OS_ALL,	INI_ADD,			/*NULL,										*/"drivers32",															SUBITEM_NEEDED},													//6
//		{OS_ALL,	ROOT_ADD,			/*NULL,										*/"HKEY_CLASSES_ROOT",													PARAM_NOTHING},														//7
//		{OS_ALL,	HKCR_ADD,			&CSimpleProcessAPI::HKCRWrite,				"Associated extension",													HKEY_CLASSES_ROOT_BASE|SUBROOT_THE_SAME|SUBITEM_NEEDED|(MULTIPLE+2)},//
//		{OS_ALL,	HKCR_ADD,			&CSimpleProcessAPI::CLSIDWrite,				"COM and DCOM Interfaces",												SUBROOT_THE_SAME},													//8
		{OS_ALL,	ROOT_ADD,			/*NULL,										*/"HKEY_LOCAL_MACHINE",													PARAM_NOTHING},														//9
		{OS_NT,		REG_ADD,			/*NULL,										*/"Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows",			HKEY_LOCAL_MACHINE_BASE|SUBITEM_NEEDED},							//10
		{OS_NT,		REG_ADD,			/*NULL,										*/"Software\\Microsoft\\Windows NT\\CurrentVersion\\WOW\\Boot",			HKEY_LOCAL_MACHINE_BASE},											//11
		{OS_NT,		REG_ADD,			/*NULL,										*/"Software\\Microsoft\\Windows NT\\CurrentVersion\\Drivers",			HKEY_LOCAL_MACHINE_BASE},											//12
		{OS_NT,		REG_ADD,			/*NULL,										*/"Software\\Microsoft\\Windows NT\\CurrentVersion\\Drivers32",			HKEY_LOCAL_MACHINE_BASE},											//13
		{OS_NT,		REG_ADD,			/*NULL,										*/"Software\\Microsoft\\Windows NT\\CurrentVersion\\WOW\\NonWindowsApp",HKEY_LOCAL_MACHINE_BASE},											//14
		{OS_NT,		REG_ADD,			/*NULL,										*/"Software\\Microsoft\\Windows NT\\CurrentVersion\\WOW\\Standard",		HKEY_LOCAL_MACHINE_BASE},											//15
		{OS_NT,		REG_ADD,			/*NULL,										*/"Software\\Microsoft\\Windows NT\\CurrentVersion\\WinLogon",			HKEY_LOCAL_MACHINE_BASE},											//16
		{OS_ALL,	REG_ADD,			/*NULL,										*/"Software\\Microsoft\\Windows\\CurrentVersion\\Run",					HKEY_LOCAL_MACHINE_BASE},											//17
		{OS_ALL,	REG_ADD,			/*NULL,										*/"Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce",				HKEY_LOCAL_MACHINE_BASE},											//18
		{OS_ALL,	REG_ADD,			/*NULL,										*/"Software\\Microsoft\\Windows\\CurrentVersion\\RunOnceEx",			HKEY_LOCAL_MACHINE_BASE},											//19
		{OS_9X,		REG_ADD,			/*NULL,										*/"Software\\Microsoft\\Windows\\CurrentVersion\\RunServices",			HKEY_LOCAL_MACHINE_BASE},											//20
		{OS_9X,		REG_ADD,			/*NULL,										*/"Software\\Microsoft\\Windows\\CurrentVersion\\RunServicesOnce",		HKEY_LOCAL_MACHINE_BASE},											//21
		{OS_ALL,	ROOT_ADD,			/*NULL,										*/"HKEY_CURRENT_USER",													PARAM_NOTHING},														//21
		{OS_NT,		REG_ADD,			/*NULL,										*/"Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows",			HKEY_CURRENT_USER_BASE|SUBITEM_NEEDED},								//23
		{OS_ALL,	REG_ADD,			/*NULL,										*/"Software\\Microsoft\\Windows\\CurrentVersion\\Run",					HKEY_CURRENT_USER_BASE},											//24
		{OS_ALL,	REG_ADD,			/*NULL,										*/"Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce",				HKEY_CURRENT_USER_BASE},											//25
		{OS_ALL,	REG_ADD,			/*NULL,										*/"Software\\Mirabilis\\ICQ\\Agent\\Apps",								HKEY_CURRENT_USER_BASE},											//26
		{OS_ALL,	ROOT_ADD,			/*NULL,										*/"HKEY_USERS\\.DEFAULT",												PARAM_NOTHING},														//27
		{OS_ALL,	REG_ADD,			/*NULL,										*/".DEFAULT\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",		HKEY_USERS_BASE},													//28
		{OS_ALL,	REG_ADD,			/*NULL,										*/".DEFAULT\\Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce",	HKEY_USERS_BASE},													//29
//		{OS_ALL,	HKCR_ADD,			&CSimpleProcessAPI::_AddServicesAndVxD,		"",																		ROOT_FLAG},															//
		{OS_9X,		STARTUP_ADD,		/*NULL,										*/"StartUp menu",														PARAM_NOTHING},														//30
		{OS_NT,		STARTUP_ADD,		/*NULL,										*/"StartUp menu",														PARAM_NOTHING},														//31
	};
	
	OS_TYPE						NotOS;
	bool						SubKeyNeeded=false;
	DWORD						Ret=LONG_PROCESS_OK;
//	CString				tempBuf;
//	CString				SecBuf="";
	HKEY						AddrHkey;
	DWORD				/*cSubKeysProfile, */d, i/*, dCount, retCodeEnum*/;
	TCHAR						newBuf[MAX_PATH]="\0";
	HMODULE						hMsi=NULL;
//	TCHAR				WinFolders[MAX_PATH];
//	FILETIME					tTime;
	m_pGetStartUpMenuFunction	pGetStartUpMenuFunction;
	
	if (os>OS_UNKNOWN) NotOS=OS_9X;
	else NotOS=OS_NT;

	if (NotOS==OS_9X) pGetStartUpMenuFunction=_GetStartUpMenuNT;
	else pGetStartUpMenuFunction=_GetStartUpMenu9x;
	hMsi=LoadLibrary("msi.dll");
	if (hMsi!=NULL)
	{
		pMsiGetShortcutTarget=(lpMsiGetShortcutTarget)GetProcAddress(hMsi,"MsiGetShortcutTargetA");
		pMsiGetComponentPath=(lpMsiGetComponentPath)GetProcAddress(hMsi,"MsiGetComponentPathA");
	}

	d=0;
//	d+=EnumerateProcesses(FALSE);
	for (i=0;i<STARTUP_LINK_COUNT;i++)
	{
		if (StartUpTable[i].OsType!=NotOS)
		{
			SubKeyNeeded=false;
			AddrHkey=NULL;
			if ((StartUpTable[i].dParam&SUBITEM_NEEDED)!=0) SubKeyNeeded=true;
//			if ((StartUpTable[i].dParam&HKEY_CLASSES_ROOT_BASE)!=0) AddrHkey=HKEY_CLASSES_ROOT;
//			else
//			{
				if ((StartUpTable[i].dParam&HKEY_LOCAL_MACHINE_BASE)!=0) AddrHkey=HKEY_LOCAL_MACHINE;
				else
				{
					if ((StartUpTable[i].dParam&HKEY_CURRENT_USER_BASE)!=0) AddrHkey=HKEY_CURRENT_USER;
					else if ((StartUpTable[i].dParam&HKEY_USERS_BASE)!=0) AddrHkey=HKEY_USERS;
				}
//			}
//			if ((StartUpTable[i].dParam&MULTIPLE)>=MULTIPLE) dCount=(StartUpTable[i].dParam&MULTIPLE_MASK)-MULTIPLE;
			d+=Count(StartUpTable[i].chString,AddrHkey,SubKeyNeeded);
		}
	}

	if (ProgressItemHWnd)
	{
		SendMessage(GetDlgItem(ProgressItemHWnd,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
		SendMessage(GetDlgItem(ProgressItemHWnd,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,d);
		SendMessage(GetDlgItem(ProgressItemHWnd,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
	}
	if (CountStruct) CountStruct->LocalStepCount = d;
//	if (CheckStop(ProgressItemHWnd,IDC_PROGRESS_CURRENT,hEv)) return Ret;
	
//	if (ProgressItemHWnd) SendMessage(ProgressItemHWnd,PBM_STEPIT,0,0);
	if (CheckStop(ProgressItemHWnd,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) goto end;

	for (i=0;i<STARTUP_LINK_COUNT;i++)
	{
		if (StartUpTable[i].OsType!=NotOS)
		{
			switch (StartUpTable[i].ActionType)
			{
			case PATH_SAVE_RET:
				_GetIniPath(StartUpTable[i].chString,newBuf);
			case ROOT_ADD:
				fprintf(LFile,"\t%s\n",StartUpTable[i].chString);
				break;
			case INI_ADD:
				{
					fprintf(LFile,"\t\tSection=%s\n",StartUpTable[i].chString);
					ReadSystemProfile(StartUpTable[i].chString,newBuf);
//					if (ProgressItemHWnd) SendMessage(ProgressItemHWnd,PBM_STEPIT,0,0);
					if (CheckStop(ProgressItemHWnd,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) goto end;
				}
				break;
			case REG_ADD:
				{
					fprintf(LFile,"\t\t%s\n",StartUpTable[i].chString);
					SubKeyNeeded=false;
					AddrHkey=NULL;
					if ((StartUpTable[i].dParam&SUBITEM_NEEDED)!=0) SubKeyNeeded=true;
//					if ((StartUpTable[i].dParam&HKEY_CLASSES_ROOT_BASE)!=0) AddrHkey=HKEY_CLASSES_ROOT;
//					else
//					{
						if ((StartUpTable[i].dParam&HKEY_LOCAL_MACHINE_BASE)!=0) AddrHkey=HKEY_LOCAL_MACHINE;
						else
						{
							if ((StartUpTable[i].dParam&HKEY_CURRENT_USER_BASE)!=0) AddrHkey=HKEY_CURRENT_USER;
							else if ((StartUpTable[i].dParam&HKEY_USERS_BASE)!=0) AddrHkey=HKEY_USERS;
						}
//					}
					Ret=ReadRegistryValues(ProgressItemHWnd,CountStruct,StartUpTable[i].chString,AddrHkey,SubKeyNeeded,hEv);
					if(Ret==LONG_PROCESS_STOPPED) goto end;
				}
				break;
			case STARTUP_ADD:
				fprintf(LFile,"\tSystem StartUp menu\n");
				pGetStartUpMenuFunction();
//				if (ProgressItemHWnd) SendMessage(ProgressItemHWnd,PBM_STEPIT,0,0);
				if (CheckStop(ProgressItemHWnd,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) goto end;
				break;
			}
		}
	}
end:
	return Ret;
}

void ReadSystemProfile(const char SectionName[], const char FileName[])
{
	int		i,StartBuf/*,d*/;
	DWORD	d;
	TCHAR	InsideBuf[1024];
	char*	FFileName=NULL;
	char*	StartFileInfo=NULL;
	char*	TempIniFileName[MAX_PATH];
	char*	Value=NULL;
	bool	NeedToDel=true;
	char	NextAnalyzeStr[MAX_PATH];
	char*	pNextAnalyzeStr=NextAnalyzeStr;
	char	FullPathName[MAX_PATH];
	char*	pFullPathName=FullPathName;
	DWORD	CutLen=0;

	__try
	{
		d=_MAX_PATH;
		if ((GetTempPath(d,(char*)TempIniFileName))==0) strcpy((char*)TempIniFileName,"c:\\");
		strcat((char*)TempIniFileName,"~initmp.tmp");
		if (!CopyFile(FileName,(char*)TempIniFileName,FALSE)) 
		{
			NeedToDel=false;
			strcpy((char*)TempIniFileName,FileName);
		}
		memset(InsideBuf,0,1024);
		GetPrivateProfileSection (SectionName, InsideBuf, 1024, (const char*)TempIniFileName);
		StartBuf=0;
		if (InsideBuf[0]!=0)
		{
			for (i=0; i<=1024; i++)
			{
				if (InsideBuf[i]==0)	
				{
					FFileName=(char*)malloc(UINT(i-StartBuf+1));
					strncpy(FFileName,InsideBuf+StartBuf,UINT(i-StartBuf));
					FFileName[UINT(i-StartBuf)]=0;
					StartFileInfo=strchr(FFileName,'=');
					if (StartFileInfo!=NULL) StartFileInfo++;
					Value=(char*)malloc(StartFileInfo-FFileName);
					if (Value!=NULL) 
					{
						strncpy(Value,FFileName,StartFileInfo-FFileName-1);
						Value[StartFileInfo-FFileName-1]=0;
					}
					fprintf(LFile,"\t\t\t%s=%s\n",Value,StartFileInfo);
					strcpy(NextAnalyzeStr,StartFileInfo);
					while (strlen(pNextAnalyzeStr)!=0)
					{
						if (!GetExecutableName(NextAnalyzeStr,&pFullPathName,&pNextAnalyzeStr,&CutLen)) break;
						fprintf(LFile,"\t\t\t\tLINK TO FILE - %s, ",pFullPathName);
						GetFileInfoAsRepInfo(pFullPathName);
					}
					free(Value);
					Value=NULL;
					free(FFileName);
					FFileName=NULL;
					if (InsideBuf[i+1]==0) break;
					StartBuf=i+1;
				}
			}
		}
	}
	__finally
	{
		if (Value!=NULL) free(Value);
		if (FFileName!=NULL) free(FFileName);
		if (NeedToDel) DeleteFile((const char*)TempIniFileName);
	}
}

void _GetStartUpMenuNT(void)
{
	char		TmpBuf[MAX_PATH]="\0";
	char*		dupWinFild=NULL;
	char*		dupLastPath=NULL;
	char*		dupUserName=NULL;
	HKEY		AddrHkey;
	DWORD		d, retCodeEnum, cSubKeysProfile;

	__try
	{
		ReadKeyValue("Software\\Microsoft\\Windows\\CurrentVersion\\explorer\\Shell Folders","Common Startup",HKEY_LOCAL_MACHINE,MAX_PATH,TmpBuf);
		if (strlen(TmpBuf))
		{
			fprintf(LFile,"\t\tAll users\n");
			ListFilesStartUp(TmpBuf);
			fflush(LFile);
		}
		if (GetWindowsDirectory(TmpBuf,MAX_PATH)!=0)
		{
			if (TmpBuf[strlen(TmpBuf)-1]!='\\') strcat(TmpBuf,"\\");
			dupWinFild=_strdup(TmpBuf);
			TmpBuf[0]=0;
			ReadKeyValue("software\\microsoft\\windows\\currentversion\\explorer\\user shell folders","startup",HKEY_CURRENT_USER,MAX_PATH,TmpBuf);
			if (strlen(TmpBuf))
			{
				dupLastPath=_strdup((char*)(TmpBuf+13));
				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"software\\microsoft\\windows nt\\currentversion\\profilelist",0,KEY_READ,&AddrHkey)==ERROR_SUCCESS)
				{
					if (RegQueryInfoKey(AddrHkey,NULL,NULL,NULL,&cSubKeysProfile,NULL,NULL,NULL,NULL,NULL,NULL,NULL)==ERROR_SUCCESS)
					{
						for (d=0,retCodeEnum=ERROR_SUCCESS;d<cSubKeysProfile;) 
						{ 
							TmpBuf[0] = '\0'; 
							retCodeEnum=RegEnumKey(AddrHkey,d,TmpBuf,(MAX_PATH+1)); 
							if (retCodeEnum==(DWORD)ERROR_SUCCESS&&retCodeEnum!=ERROR_INSUFFICIENT_BUFFER) 
							{
								dupUserName=_strdup(TmpBuf);
								strcat(strcpy(TmpBuf,"software\\microsoft\\windows nt\\currentversion\\profilelist\\"),dupUserName);
								free(dupUserName);
								dupUserName=_strdup(TmpBuf);
								TmpBuf[0]=0;
								ReadKeyValue(dupUserName,"ProfileImagePath",HKEY_LOCAL_MACHINE,MAX_PATH,TmpBuf);
								free(dupUserName);
								if (TmpBuf[strlen(TmpBuf)-1]!='\\') strcat(TmpBuf,"\\");
								dupUserName=_strdup(TmpBuf+12);
								strcat(strcat(strcpy(TmpBuf,dupWinFild),dupUserName),dupLastPath);
								fprintf(LFile,"\t\t%s\n",dupUserName);
								ListFilesStartUp(TmpBuf);
								fflush(LFile);
								free(dupUserName);
								d=d++;
							}
						}
					}
				}
				RegCloseKey(AddrHkey);
			}
		}
	}
	__finally
	{
		if (dupLastPath!=NULL) free(dupLastPath);
		if (dupWinFild!=NULL) free(dupWinFild);
	}
}

void _GetStartUpMenu9x(void)
{
	char		TmpBuf[MAX_PATH]="\0";
	char*		dupWinFild=NULL;
	char*		dupLastPath=NULL;
	char*		dupUserName=NULL;
	HKEY		AddrHkey;
	DWORD		d, retCodeEnum, cSubKeysProfile;

	__try
	{
		ReadKeyValue("Software\\Microsoft\\Windows\\CurrentVersion\\explorer\\Shell Folders","Common Startup",HKEY_LOCAL_MACHINE,MAX_PATH,TmpBuf);
		if (strlen(TmpBuf))
		{
			fprintf(LFile,"\t\tAll users\n");
			ListFilesStartUp(TmpBuf);
		}
		TmpBuf[0]=0;
		ReadKeyValue("Software\\Microsoft\\Windows\\CurrentVersion\\ProfileReconciliation","ProfileDirectory",HKEY_CURRENT_USER,MAX_PATH,TmpBuf);
		d=strlen(TmpBuf);
		TmpBuf[0]=0;
		ReadKeyValue("software\\microsoft\\windows\\currentversion\\explorer\\user shell folders","startup",HKEY_CURRENT_USER,MAX_PATH,TmpBuf);
		if (strlen(TmpBuf))
		{
			dupWinFild=_strdup(TmpBuf+d);
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"software\\microsoft\\windows\\currentversion\\profilelist",0,KEY_READ,&AddrHkey)==ERROR_SUCCESS)
			{
				if (RegQueryInfoKey(AddrHkey,NULL,NULL,NULL,&cSubKeysProfile,NULL,NULL,NULL,NULL,NULL,NULL,NULL)==ERROR_SUCCESS)
				{
					for (d=0,retCodeEnum=ERROR_SUCCESS;d<cSubKeysProfile;) 
					{ 
						TmpBuf[0]='\0'; 
						retCodeEnum=RegEnumKey(AddrHkey,d,TmpBuf,(MAX_PATH+1)); 
						if (retCodeEnum == (DWORD)ERROR_SUCCESS && retCodeEnum != ERROR_INSUFFICIENT_BUFFER) 
						{
							dupUserName=_strdup(TmpBuf);
							strcat(strcpy(TmpBuf,"software\\microsoft\\windows\\currentversion\\profilelist\\"),dupUserName);
							dupLastPath=_strdup(TmpBuf);
							TmpBuf[0]='\0'; 
							ReadKeyValue(dupLastPath,"ProfileImagePath",HKEY_LOCAL_MACHINE,MAX_PATH,TmpBuf);
							free(dupLastPath);
							if (TmpBuf[strlen(TmpBuf)-1]!='\\') strcat(TmpBuf,"\\");
							strcat(TmpBuf,dupWinFild);
							fprintf(LFile,"\t\t%s\n",dupUserName);
							ListFilesStartUp(TmpBuf);
							free(dupUserName);
							d=d++;
						}
					}
				}
				RegCloseKey(AddrHkey);
			}
			else
			{
				TmpBuf[0]='\0'; 
				ReadKeyValue("software\\microsoft\\windows\\currentversion\\explorer\\shell folders","startup",HKEY_CURRENT_USER,MAX_PATH,TmpBuf);
				fprintf(LFile,"\tCurrent user\n");
				ListFilesStartUp(TmpBuf);
			}
		}
		else
		{
			TmpBuf[0]='\0'; 
			ReadKeyValue("software\\microsoft\\windows\\currentversion\\explorer\\shell folders","startup",HKEY_CURRENT_USER,MAX_PATH,TmpBuf);
			fprintf(LFile,"\tCurrent user\n");
			ListFilesStartUp(TmpBuf);
		}
	}
	__finally
	{
		if (dupWinFild!=NULL) free(dupWinFild);
	}
}

void ListFilesStartUp(LPCTSTR StartUpPath)
{
	char			szLinkPath[MAX_PATH];
	char			szGotPath[MAX_PATH];
	char*			pszGotPath=szGotPath;
	char			szDescription[MAX_PATH]="";
	char			FullFN[MAX_PATH];
	char*			pFullFN=FullFN;
	WIN32_FIND_DATA	FindFileStruct;
	WIN32_FIND_DATA	wfd;
	HANDLE			FFile=INVALID_HANDLE_VALUE;
	IShellLink		*psl;
	DWORD			dw;

	try
	{
		if (StartUpPath[strlen(StartUpPath)-1]=='\\') strcat(strcpy(szGotPath,StartUpPath),"*.*");
		else strcat(strcpy(szGotPath,StartUpPath),"\\*.*");
		FFile=FindFirstFile(szGotPath,&FindFileStruct);
		if (FFile!=INVALID_HANDLE_VALUE)
		{
			FindNextFile(FFile,&FindFileStruct);
			if (FFile!=INVALID_HANDLE_VALUE)
			{
				while(true)
				{
					if(FindNextFile(FFile,&FindFileStruct))
					{
						BOOL FileExist=FALSE;
						strcpy(szLinkPath,StartUpPath);
						if (StartUpPath[strlen(StartUpPath)-1]!='\\') strcat(szLinkPath,"\\");
						strcat(szLinkPath,FindFileStruct.cFileName);
						memset(FullFN,0,MAX_PATH);
						GetExecutableName(szLinkPath,&pFullFN,NULL,NULL);
						fprintf(LFile,"\t\t\t%s, ",FullFN);
						GetFileInfoAsRepInfo(pFullFN);
						if (strstr(FindFileStruct.cFileName,".lnk")!=NULL) 
						{
							if ((pMsiGetShortcutTarget!=NULL)&&(pMsiGetComponentPath!=NULL))
							{
								TCHAR tszComponentCode[MAX_FEATURE_CHARS+1]="";
								TCHAR tszProductCode[MAX_FEATURE_CHARS+1]="";
								if (pMsiGetShortcutTarget(FullFN,tszProductCode,NULL,tszComponentCode)==ERROR_SUCCESS)
								{
									dw=MAX_PATH;
									if (/*INSTALLSTATE_LOCAL*/8==pMsiGetComponentPath(tszProductCode,tszComponentCode,szGotPath,&dw))
									{
										GetExecutableName(szGotPath,&pFullFN,NULL,NULL);
										fprintf(LFile,"\t\t\t\tLINK TO FILE - %s, ",FullFN);
										GetFileInfoAsRepInfo(pFullFN);
										FileExist=TRUE;
									}
								}
							}
							if (!FileExist)
							{
								CoInitialize(NULL);
								if (CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(void **)&psl)==S_OK)
								{
									IPersistFile *ppf;
									if (psl->QueryInterface(IID_IPersistFile,(void **)&ppf)==S_OK)
									{
										wchar_t wsz[MAX_PATH]; 
										MultiByteToWideChar(CP_ACP,0,szLinkPath,-1,wsz,MAX_PATH);
										if (ppf->Load(wsz, STGM_READ)==S_OK)
										{
											strcpy(szGotPath,StartUpPath);
											if (StartUpPath[strlen(StartUpPath)-1]!='\\') strcat(szGotPath,"\\");
											strcat(szGotPath,FindFileStruct.cFileName);
											if (psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA *)&wfd, SLGP_SHORTPATH)==S_OK)
											{
												if (psl->GetArguments(szDescription, MAX_PATH)==S_OK) 
												{
													ConvertToLongName(szGotPath,&pszGotPath);
													memset(FullFN,0,MAX_PATH);
													GetExecutableName(szGotPath,&pFullFN,NULL,NULL);
													fprintf(LFile,"\t\t\t\tLINK TO FILE - %s, ",FullFN);
													GetFileInfoAsRepInfo(pFullFN);
												}
											}
										}
										ppf->Release();
									}
									psl->Release();
								}
								CoUninitialize();
							}
						}
					}
					else break;	
				}
			}
		}
	}
	catch(...)
	{
	}
	return;
}

void ConvertToLongName(LPSTR szShortFileName,char** pLongFileName)
{
//	CString	szLongFileName="";
	int		i,j;
	char	old_path[MAX_PATH];
	char	short_path[MAX_PATH];
	char	szFileName1[MAX_PATH];
	char	szFileName2[MAX_PATH];

	__try
	{
		if (pLongFileName==NULL) return;
		strcpy(short_path,szShortFileName);
		GetCurrentDirectory(sizeof(old_path),old_path);
		if(short_path[1]==':' && short_path[2]=='\\') 
		{
			(*pLongFileName)[0]=short_path[0];
			(*pLongFileName)[1]=0;
			strcat(*pLongFileName,":\\");
//			szLongFileName=szLongFileName+szShortFileName[0]+":\\";
			for(i=3,j=3;i<=lstrlen(short_path);i++)
			{
				if(short_path[i]=='\\' || short_path[i]==0)
				{
					ZeroMemory(szFileName1,sizeof(szFileName1));
					strncpy(szFileName1,&short_path[j],i-j);
					if(lstrlen(szFileName1)==0) return;
					SetCurrentDirectory(*pLongFileName);
					if(!iFindLongPath(szFileName1,szFileName2)) 
					{
						strcpy(*pLongFileName,short_path);
//						szLongFileName=szShortFileName;
						return;
					}
					strcat(strcat(*pLongFileName,szFileName2),"\\");
//					szLongFileName=szLongFileName+szFileName2+"\\";
					j=i+1;
				}
			}
			(*pLongFileName)[strlen(*pLongFileName)-1]=0;
//			szLongFileName.Delete(szLongFileName.GetLength()-1,1);
		}
		else strcpy(*pLongFileName,short_path);
	}
	__finally
	{
		SetCurrentDirectory(old_path);
	}
}

bool iFindLongPath(LPCSTR szIn, LPSTR szOut)
{
	WIN32_FIND_DATA	fd;
	HANDLE			hf=INVALID_HANDLE_VALUE;
	bool			Ret=false;
	
	__try
	{
		ZeroMemory(&fd,sizeof(WIN32_FIND_DATA));
		fd.dwFileAttributes=FILE_ATTRIBUTE_ARCHIVE|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_DIRECTORY;
		hf = FindFirstFile(szIn,&fd);
		if(hf!=INVALID_HANDLE_VALUE)
		{
			if(lstrlen(fd.cFileName)!=0) lstrcpy(szOut,fd.cFileName);
			else lstrcpy(szOut,fd.cAlternateFileName);
			Ret=true;
		}
	}
	__finally
	{
		if(hf!=INVALID_HANDLE_VALUE) FindClose(hf);
	}
	return Ret;
}

bool ReadKeyValue(char KeyPath[],char NameValue[],HKEY BaseKeyEnter,DWORD DataSize,char* pData)
{
	TCHAR	tempBuf[MAX_PATH];
	HKEY	AddrHkey=NULL;
	DWORD	lB=DataSize;
	bool	Ret=false;

	__try
	{
		if (pData==NULL) return Ret;
		DWORD RetCode=RegOpenKeyEx(BaseKeyEnter, KeyPath, 0,KEY_QUERY_VALUE, &AddrHkey);
		if (RetCode == ERROR_SUCCESS)
		{
			if (RegQueryValueEx( AddrHkey, NameValue, NULL, NULL, (LPBYTE)tempBuf, &lB) == ERROR_SUCCESS)
			{
				if (DataSize>=lB)
				{
					strcpy(pData,tempBuf);
					Ret=true;
				}
			}
		}
	}
	__finally
	{
		if (AddrHkey!=NULL) RegCloseKey(AddrHkey);
	}
	return Ret;
}

DWORD ReadRegistryValues(HWND ProgressItemHWnd,pCOUNTSTRUCT CountStruct,const char SubKey[],HKEY BaseAddr,const bool NeedSubKeys,HANDLE hEv)
{
	DWORD	Ret=LONG_PROCESS_OK;
	TCHAR	achKey[_MAX_PATH+1]; 
	HKEY	AddrHkeyE=NULL, AddrHkeyS;
	DWORD	cSubKeys, cValues, retCode, k; 
	
	__try
	{
		if (RegOpenKeyEx(BaseAddr,SubKey,0,KEY_READ,&AddrHkeyE)==ERROR_SUCCESS)
		{
			if (RegQueryInfoKey(AddrHkeyE,NULL,NULL,NULL,&cSubKeys,NULL,NULL,&cValues,NULL,NULL,NULL,NULL)==ERROR_SUCCESS) 
			{
				Ret=ReadAllKeyValues(cValues,AddrHkeyE,ProgressItemHWnd,CountStruct,hEv);
				if (Ret==LONG_PROCESS_STOPPED) cSubKeys=0;
				if (NeedSubKeys && cSubKeys)
				{
					for (k=0,retCode=ERROR_SUCCESS;k<cSubKeys;) 
					{ 
						DWORD	cchKey=_MAX_PATH+1;
						achKey[0]='\0'; 
						retCode=RegEnumKey(AddrHkeyE,k,achKey,cchKey); 
						if (retCode==(DWORD)ERROR_SUCCESS && retCode!=ERROR_INSUFFICIENT_BUFFER) 
						{
							fprintf(LFile,"\t\t\tSub key - %s\n",achKey);
							if (RegOpenKeyEx(AddrHkeyE,achKey,0,KEY_READ,&AddrHkeyS)==ERROR_SUCCESS)
							{	
								if (RegQueryInfoKey(AddrHkeyS,NULL,NULL,NULL,NULL,NULL,NULL,&cValues,NULL,NULL,NULL,NULL)==ERROR_SUCCESS) 
								{
									ReadAllKeyValues(cValues,AddrHkeyS,NULL,NULL,hEv);
									if (ProgressItemHWnd) 
									{
										SendMessage(ProgressItemHWnd,PBM_STEPIT,0,0);
										if (CheckStop(ProgressItemHWnd,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) break;
									}
								}
								RegCloseKey(AddrHkeyS);
							}
							k=k++;
						}
					}
				} 
			}
		}
	}
	__finally
	{
		if (AddrHkeyE!=NULL) RegCloseKey(AddrHkeyE);
	}
	return Ret;
}

DWORD ReadAllKeyValues(DWORD cValues,HKEY AddrHkey,HWND ProgressItemHWnd,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	char	achValue[_MAX_PATH];
	TCHAR	achDate[(_MAX_PATH+1)*2];
	DWORD	j, DateType, Ret=LONG_PROCESS_OK;
	DWORD	retValue;

	__try
	{
		for (j=0,retValue=ERROR_SUCCESS;j<cValues;) 
		{ 
			DWORD	cchValue=_MAX_PATH; 
			DWORD	cchDate=_MAX_PATH;
			achValue[0]='\0'; 
			retValue=RegEnumValue(AddrHkey,j,achValue,&cchValue,NULL,&DateType,(LPBYTE)achDate,&cchDate); 
			if (retValue==(DWORD)ERROR_SUCCESS&&retValue!=ERROR_INSUFFICIENT_BUFFER)
			{ 
				if (DateType==REG_SZ) 
				{
					char* CL=NULL;
					char NextFileName[MAX_PATH];
					char* pNextFileName=NextFileName;
					char LeavingStr[MAX_PATH*2];
					char* pLeavingStr=LeavingStr;
					fprintf(LFile,"\t\t\t%s=%s\n",achValue,achDate);
					strcpy(LeavingStr,achDate);
					CL=_strdup(LeavingStr);
					DWORD CutLen=0;
					if (GetExecutableName(CL,&pNextFileName,&pLeavingStr,&CutLen))
					{
						free(CL);
						fprintf(LFile,"\t\t\t\tLINK TO FILE - %s, ",NextFileName);
						GetFileInfoAsRepInfo(NextFileName);
						while (strlen(LeavingStr)!=0)
						{
							CL=_strdup(LeavingStr);
							DWORD CutLen=0;
							if (GetExecutableName(CL,&pNextFileName,&pLeavingStr,&CutLen))
							{
								fprintf(LFile,"\t\t\t\tLINK TO FILE - %s, ",NextFileName);
								GetFileInfoAsRepInfo(NextFileName);
							}
							else
							{
								if (strlen(CL)!=0) strcpy(LeavingStr,CL+1);
							}
							free(CL);
						}
					}
					else free(CL);
				}
				j=j++;
//				if (ProgressItemHWnd) SendMessage(ProgressItemHWnd,PBM_STEPIT,0,0);
				if (CheckStop(ProgressItemHWnd,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) break;
			}
		}
	}
	__finally
	{
	}
	return Ret;
}

DWORD Count(const char* KeyName, HKEY KeyBase, bool SubKeyCount)
{
	HKEY AddrHkeyE=NULL;
	DWORD cSubKeys=0,cValues=0;
	__try
	{
		if (KeyBase==NULL)
		{
			if (SubKeyCount) return 1;
			else return 0;
		}
		if (RegOpenKeyEx(KeyBase,KeyName,0,KEY_READ,&AddrHkeyE)==ERROR_SUCCESS)
		{
			RegQueryInfoKey(AddrHkeyE,NULL,NULL,NULL,&cSubKeys,NULL,NULL,&cValues,NULL,NULL,NULL,NULL);
		}
		if (SubKeyCount) cValues+=cSubKeys;
	}
	__finally
	{
		if (AddrHkeyE!=NULL) RegCloseKey(AddrHkeyE);
	}
	return (cValues);
}

char* _GetIniPath(const char IniName[],char IniPath[MAX_PATH])
{
	__try
	{
		GetWindowsDirectory(IniPath,MAX_PATH);
		if (IniPath[strlen(IniPath)-1]!='\\') strcat(IniPath,"\\");
		strcat(IniPath,IniName);
	}
	__finally
	{
	}
	return IniPath;
}

