#include "stdafx.h"
#include "SoftWareInfo.h"
#include "MemoryScan.h"
#include "AssociatedString.h"

extern OSVERSIONINFOEX			versionInfo;
extern SI_INSIDE_OS_NUMBER		os;

extern HMODULE					hKernel;
extern FILE*					LFile;

bool ReadOSVersion(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	TCHAR	strOS[MAX_PATH];
	TCHAR	strBuild[MAX_PATH];
	bool	Ret=false;
	__try
	{
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,1);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
		if (CountStruct) CountStruct->LocalStep=1;
		strcpy(strOS,"Microsoft Windows");
		switch (os)
		{
		case OS_WIN32:
			strcat(strOS," 3.1");
			break;
		case OS_WIN95:
			strcat(strOS," 95");
			if ((versionInfo.szCSDVersion[1]=='C')||(versionInfo.szCSDVersion[1]=='B')) strcat(strOS," OSR2");
			break;
		case OS_WIN98:
			strcat(strOS," 98");
			if (versionInfo.szCSDVersion[1]=='A') strcat(strOS," SE");
			break;
		case OS_WINME:
			strcat(strOS," ME");
			break;
		case OS_WINNT351:
			strcat(strOS," NT 3.51");
			if (versionInfo.wProductType==VER_NT_WORKSTATION) strcat(strOS," Workstation");
			if (versionInfo.wProductType==VER_NT_DOMAIN_CONTROLLER) strcat(strOS," Domain controller");
			if (versionInfo.wProductType==VER_NT_SERVER) strcat(strOS," Server");
			break;
		case OS_WINNT40:
			strcat(strOS," NT 4.0");
			if (versionInfo.wProductType==VER_NT_WORKSTATION) strcat(strOS," Workstation");
			if (versionInfo.wProductType==VER_NT_DOMAIN_CONTROLLER) strcat(strOS," Domain controller");
			if (versionInfo.wProductType==VER_NT_SERVER) strcat(strOS," Server");
			break;
		case OS_WIN2000:
			strcat(strOS," 2000");
			if (versionInfo.wProductType==VER_NT_WORKSTATION) strcat(strOS," Professional");
			if (versionInfo.wProductType==VER_NT_DOMAIN_CONTROLLER) strcat(strOS," Domain controller");
			if (versionInfo.wProductType==VER_NT_SERVER) 
			{
				if ((versionInfo.wSuiteMask&VER_SUITE_DATACENTER)==VER_SUITE_DATACENTER) strcat(strOS," DataCenter Server");
				else
				{
					if ((versionInfo.wSuiteMask&VER_SUITE_ENTERPRISE)==VER_SUITE_ENTERPRISE) strcat(strOS," Advanced Server");
					else
					{
						if ((versionInfo.wSuiteMask&VER_SUITE_BLADE)==VER_SUITE_BLADE) strcat(strOS," Web Server");
						else strcat(strOS," Server");
					}
				}
			}
			break;
		case OS_WINXP:
			strcat(strOS," XP");
			if (versionInfo.wProductType==VER_NT_WORKSTATION)
			{
				if ((versionInfo.wSuiteMask&VER_SUITE_PERSONAL)==VER_SUITE_PERSONAL) strcat(strOS," Home Etition");
				else strcat(strOS," Professional Edition");
			}
			if (versionInfo.wProductType==VER_NT_DOMAIN_CONTROLLER) strcat(strOS," Domain controller");
			if (versionInfo.wProductType==VER_NT_SERVER)
			{
				if ((versionInfo.wSuiteMask&VER_SUITE_DATACENTER)==VER_SUITE_DATACENTER) strcat(strOS," DataCenter Server");
				else
				{
					if ((versionInfo.wSuiteMask&VER_SUITE_ENTERPRISE)==VER_SUITE_ENTERPRISE) strcat(strOS," Enterprise Server");
					else
					{
						if ((versionInfo.wSuiteMask&VER_SUITE_BLADE)==VER_SUITE_BLADE) strcat(strOS," Web Server");
						else strcat(strOS," Server");
					}
				}
			}
			break;
		case OS_WINXPNET:
			strcat(strOS," .NET");
			if (versionInfo.wProductType==VER_NT_WORKSTATION)
			{
				if ((versionInfo.wSuiteMask&VER_SUITE_PERSONAL)==VER_SUITE_PERSONAL) strcat(strOS," Home Etition");
				else strcat(strOS," Professional Edition");
			}
			if (versionInfo.wProductType==VER_NT_DOMAIN_CONTROLLER) strcat(strOS," Domain controller");
			if (versionInfo.wProductType==VER_NT_SERVER)
			{
				if ((versionInfo.wSuiteMask&VER_SUITE_DATACENTER)==VER_SUITE_DATACENTER) strcat(strOS," DataCenter Server");
				else
				{
					if ((versionInfo.wSuiteMask&VER_SUITE_ENTERPRISE)==VER_SUITE_ENTERPRISE) strcat(strOS," Enterprise Server");
					else
					{
						if ((versionInfo.wSuiteMask&VER_SUITE_BLADE)==VER_SUITE_BLADE) strcat(strOS," Web Server");
						else strcat(strOS," Server");
					}
				}
			}
			break;
		default:
			strcpy(strOS,"Unknown OS");
			break;
		}
		_ltoa(versionInfo.dwMajorVersion,strBuild,10);
		strcat(strBuild,".");
		_ltoa(versionInfo.dwMinorVersion,strBuild+strlen(strBuild),10);
		strcat(strBuild,".");
		_ltoa((DWORD)LOWORD(versionInfo.dwBuildNumber),strBuild+strlen(strBuild),10);
		strcat(strcat(strBuild," "),versionInfo.szCSDVersion);
		fprintf(LFile,"Operation system:\n\t\t%s, %s\n\n",strOS,strBuild);
		fflush(LFile);
		Ret=CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv);
		return Ret;
	}
	__finally
	{
	}
	return Ret;
}

bool OSLocaleInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	bool	bGet=false;
	HKEY	hKey=NULL;
	TCHAR	LangName[MAX_PATH];
	DWORD	Size=MAX_PATH;
	DWORD	ValType=REG_SZ;
	LCID	OSLocaleleID;
	bool	Ret=false;
	__try
	{
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,1);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
		if (CountStruct) CountStruct->LocalStep=1;
		if (os<OS_WINME)
		{
			if (RegOpenKeyEx(HKEY_CURRENT_USER,"Control Panel\\desktop\\ResourceLocale",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
			{
				if (RegQueryValueEx(hKey,NULL,NULL,&ValType,(LPBYTE)LangName,&Size)==ERROR_SUCCESS) 
				{
					bGet=true;
					OSLocaleleID=(LCID)strtol(LangName,NULL,16);
				}
			}
		}
		else
		{
			if ((os>OS_UNKNOWN)&(os<OS_WIN2000))
			{
				if (RegOpenKeyEx(HKEY_USERS,".DEFAULT\\Control Panel\\International",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
				{
					if (RegQueryValueEx(hKey,"Locale",NULL,&ValType,(LPBYTE)LangName,&Size)==ERROR_SUCCESS) 
					{
						bGet=true;
						OSLocaleleID=(LCID)strtol(LangName,NULL,16);
					}
				}
			}
			else
			{
				pfnGetUserDefaultUILanguageType	pfnGetUserDefaultUILanguage=NULL;
				pfnGetUserDefaultUILanguage=(pfnGetUserDefaultUILanguageType)GetProcAddress(hKernel,"GetUserDefaultUILanguage");
				if (pfnGetUserDefaultUILanguage!=NULL)
				{
					bGet=true;
					OSLocaleleID=pfnGetUserDefaultUILanguage();
				}
			}
		}
		if (bGet)
		{
			if (GetLocaleInfo(OSLocaleleID,LOCALE_SENGLANGUAGE,LangName,MAX_PATH)!=0) bGet=true;
		}
		if (!bGet) strcpy(LangName,"Unknown");
		fprintf(LFile,"Default interface language of user:\n\t\t%s\n\n",LangName);
		fflush(LFile);
		Ret=CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv);
		return Ret;
	}
	__finally
	{
		if (hKey!=NULL) RegCloseKey(hKey);
	}
	return Ret;
}

bool OSSysDefLang(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	TCHAR	LangName[MAX_PATH];
	bool	Ret=false;
	__try
	{
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,1);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
		if (CountStruct) CountStruct->LocalStep=1;
		if (GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,LOCALE_SENGLANGUAGE,LangName,MAX_PATH)==0) strcpy(LangName,"Unknown");
		fprintf(LFile,"System default language:\n\t\t%s\n\n",LangName);
		fflush(LFile);
		Ret=CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv);
		return Ret;
	}
	__finally
	{
	}
	return Ret;
}

typedef struct __HKL{
	HKL	lpListItem[1];
}_HKL,*pHKL;

bool GetKeyboardLocaleInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	DWORD	i;
	UINT	Size=0;
	pHKL	lpList=NULL;
	TCHAR	LangName[MAX_PATH];
	bool	Ret=false;
	__try
	{
		Size=GetKeyboardLayoutList(0,(HKL*)lpList);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,Size);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
		if (Size!=0)
		{
			if (CountStruct) CountStruct->LocalStep=Size;
			lpList=(pHKL)malloc(Size*sizeof(HKL));
			if ((UINT)GetKeyboardLayoutList(Size,(HKL*)lpList)==Size)
			{
				fprintf(LFile,"Keyboard layout languages:\n");
				fflush(LFile);
				for (i=0;i<Size;i++)
				{
					LangName[0]=0;
					LANGID langID=LOWORD((DWORD)(lpList->lpListItem[i]));
					LCID LocaleleID=MAKELCID(langID,SORT_DEFAULT);
					if (GetLocaleInfo(LocaleleID,LOCALE_SENGLANGUAGE,LangName,MAX_PATH)!=0) 
					{
						fprintf(LFile,"\t\t%s\n",LangName);
						fflush(LFile);
					}
					if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv))
					{
						Ret=true;
						break;
					}
				}
				fprintf(LFile,"\n");
				fflush(LFile);
			}
		}
		return Ret;
	}
	__finally
	{
		if (lpList!=NULL) free(lpList);
	}
	return Ret;
}

bool GetCodePageInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	bool			Ret=false;
	_try
	{
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,1);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
		if (CountStruct) CountStruct->LocalStep=1;
		fprintf(LFile,"Installed code pages:\n");
		fflush(LFile);
		EnumSystemCodePages(AnalyseCodePagesProc,CP_INSTALLED);
		Ret=CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv);
		return Ret;
	}
	__finally
	{
	}
	return Ret;
}

BOOL CALLBACK AnalyseCodePagesProc(LPTSTR lpCodePageString)
{
	TCHAR	LangName[MAX_PATH];
	char*	chLangName=LangName;
	__try
	{
		DWORD dCodePage=strtol(lpCodePageString,NULL,10);
		GetCodePageName(dCodePage,&chLangName);
		fprintf(LFile,"\t\t%d = %s\n",dCodePage,LangName);
		fflush(LFile);
		return TRUE;
	}
	__finally
	{
	}
	return TRUE;
}

bool GetInstalledAppInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	DWORD	i;
	DWORD	cSubKeysNumber=0;
	DWORD	retCode=ERROR_SUCCESS;
	DWORD	rSize=MAX_PATH;
	DWORD	rType=REG_SZ;
	HKEY	hKey=NULL;
	HKEY	hKeyUn=NULL;
	TCHAR	chSubKeyName[MAX_PATH];
	bool	Ret=false;
	__try
	{
		fprintf(LFile,"Installed applications/hotfixes:\n");
		fflush(LFile);
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
		{
			RegQueryInfoKey(hKey,NULL,NULL,NULL,&cSubKeysNumber,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
			if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
			if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,cSubKeysNumber);
			if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
			for (i=0;i<cSubKeysNumber;i++)
			{
				if (CountStruct) CountStruct->LocalStep=cSubKeysNumber;
				retCode=RegEnumKey(hKey,i,chSubKeyName,MAX_PATH); 
				if ((retCode==(DWORD)ERROR_SUCCESS)&&(retCode!=ERROR_INSUFFICIENT_BUFFER)) 
				{
					if (RegOpenKeyEx(hKey,chSubKeyName,0,KEY_READ,&hKeyUn)==ERROR_SUCCESS)
					{
						bool	Find=false;
						rSize=MAX_PATH;
						if (RegQueryValueEx(hKeyUn,"DisplayName",NULL,&rType,(unsigned char*)chSubKeyName,&rSize)==ERROR_SUCCESS) Find=true;
						else
						{
							rSize=MAX_PATH;
							if (RegQueryValueEx(hKeyUn,"QuietDisplayName",NULL,&rType,(unsigned char*)chSubKeyName,&rSize)==ERROR_SUCCESS) Find=true;
						}
						if (Find)
						{
							fprintf(LFile,"\t\t%s\n",chSubKeyName);
							fflush(LFile);
						}
						if (hKeyUn!=NULL) RegCloseKey(hKeyUn);
						hKeyUn=NULL;
					}
				}
				if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv))
				{
					Ret=true;
					break;
				}
			}
		}
		fprintf(LFile,"\n");
		fflush(LFile);
		return Ret;
	}
	__finally
	{
		if (hKeyUn!=NULL) RegCloseKey(hKeyUn);
		if (hKey!=NULL) RegCloseKey(hKey);
	}
	return Ret;
}

bool GetNetProtocolInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	HMODULE					hWsock=NULL;
	WSCEnumProtocolsPtr		m_pWSCEnumProtocols=NULL;
	pPROTOCOL_INFOW_ARRAY	pBuf=NULL;
	DWORD					dSize=0;
	int						iError=0;
	int						ProtocolItemNumber=0;
	DWORD					i;
	bool					Ret=false;

	__try
	{
		hWsock=LoadLibrary("Ws2_32.dll");
		if (hWsock!=NULL)
		{
			m_pWSCEnumProtocols=(WSCEnumProtocolsPtr)GetProcAddress(hWsock,"WSCEnumProtocols");
			if (m_pWSCEnumProtocols!=NULL)
			{
				m_pWSCEnumProtocols(NULL,(WSAPROTOCOL_INFOW*)pBuf,&dSize,&iError);
				if (dSize>0)
				{
					pBuf=(pPROTOCOL_INFOW_ARRAY)malloc(dSize);
					ProtocolItemNumber=m_pWSCEnumProtocols(NULL,(WSAPROTOCOL_INFOW*)pBuf,&dSize,&iError);
					if (dSize>0)
					{
						if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
						if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,ProtocolItemNumber);
						if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
						if (ProtocolItemNumber!=SOCKET_ERROR)
						{
							if (CountStruct) CountStruct->LocalStep=ProtocolItemNumber;
							fprintf(LFile,"Installed network protocols:\n");
							fflush(LFile);
							for (i=0;i<(DWORD)ProtocolItemNumber;i++)
							{
								TCHAR	chName[MAX_PATH];
								if (WideCharToMultiByte(CP_ACP,0,pBuf->lpBuf[i].szProtocol,-1/*WSAPROTOCOL_LEN*/,chName,MAX_PATH,NULL,NULL)!=0)
								{
									fprintf(LFile,"\t\t%s\n",chName);
									fflush(LFile);
								}
								if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv))
								{
									Ret=true;
									break;
								}
							}
							fprintf(LFile,"\n");
							fflush(LFile);
						}
					}
				}
			}
		}
		return Ret;
	}
	__finally
	{
		if (pBuf!=NULL) free(pBuf);
		if (hWsock!=NULL) FreeLibrary(hWsock);
	}
	return Ret;
}

bool GetEnvironmentVariables(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	LPTSTR	lpszVariable; 
	LPVOID	lpvEnv=NULL; 
	bool	Ret=false;
	
	__try
	{
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,1);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
		if (CountStruct) CountStruct->LocalStep=1;
		lpvEnv=GetEnvironmentStrings();
		fprintf(LFile,"Environment variables:\n");
		fflush(LFile);
		for (lpszVariable=(LPTSTR)lpvEnv;*lpszVariable;lpszVariable++) 
		{
			fprintf(LFile,"\t\t%s\n",lpszVariable);
			fflush(LFile);
			lpszVariable=(LPTSTR)((DWORD)lpszVariable+strlen(lpszVariable));
		}
		fprintf(LFile,"\n");
		fflush(LFile);
		Ret=CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv);
		return Ret;
	}
	__finally
	{
		FreeEnvironmentStrings((LPTSTR)lpvEnv);
	}
	return Ret;
}
