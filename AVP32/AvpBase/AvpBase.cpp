////////////////////////////////////////////////////////////////////
//
//  AVPBASE.CPP
//  Engine DLL Wrapper
//  AVP 3.0 Engine
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1996
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#include "stdafx.h"


#ifdef SYSTOS2
 #include "filefunc.h"
 HMODULE  modHandleIO;            //ў®§ўа й Ґ¬л© ®ЇЁб вҐ«м ¬®¤г«п
#else
 HINSTANCE hiIODll=NULL;
#endif

#include <mbctype.h>
#include <locale.h>
#include <stdio.h>
#include <io.h>

#ifndef NOAFX
#include "AvpBase.h"
#endif



#include <ScanAPI/BaseAPI.h>

#define AVP_DLL
#include <ScanAPI/avp_dll.h>

#include <AvpVndID.h>
#include <Sign/sign.h>
#include <serialize/kldtser.h>


#define SIGN_BUF_LEN 0x2000
#define  SignCloseHandle	CloseHandle
#define  SignReadFile		ReadFile
#define  SignCreateFile		CreateFile 
#include <Sign/a_chfile.c>

HWND s_hWnd=0;

#ifdef SYSTOS2
//#define AVP_MSGSTRING WM_USER+1
#define WM_AVP WM_USER+1000;
#else
const UINT       wm_AVP = RegisterWindowMessage( AVP_MSGSTRING );
#endif

#ifndef _WINAPI_
#define _WINAPI_  WINAPI
#endif

t_AvpCallback* cbf_ptr=NULL;


static void** vndArray=NULL;
static int vndArrayCount=0;

#ifdef _DEBUG
#define _NO_TRY_EXCEPT
#endif

#ifdef _NO_TRY_EXCEPT
#define __try if(1)
#define __except(x) else
#endif

IsSupportedException(DWORD exception_code)
{
	if(exception_code==0x80000003)
	{
//		char s[200];
//		char* lpFilePart;
//		GetModuleFileName(NULL,s,0x200);
//		GetFullPathName(s,0x200,s,&lpFilePart);
//		_AvpStrlwr(lpFilePart);
//		if(!strcmp(lpFilePart,"avpedit.exe"))

		{
			if(IDYES==MessageBox(NULL,"Debug breakpoint in AVP Engine\n\nContinue?",NULL,MB_YESNO|MB_ICONEXCLAMATION|MB_TOPMOST)){
				return EXCEPTION_CONTINUE_SEARCH;
			}
		}
	}
#ifdef _DEBUG
	return EXCEPTION_CONTINUE_SEARCH;
#else
	char buf[0x100];
	sprintf(buf,"Exception 0x%8X",exception_code);
	AvpCallback(AVP_CALLBACK_PUT_STRING,(long)buf);
//	__asm int 3;

	return EXCEPTION_EXECUTE_HANDLER;
#endif
}



static void VndDestroy()
{
	__try {
		if(vndArray){
			for(int i=0;i<vndArrayCount;i++)
				delete [] vndArray[i];
			delete [] vndArray;
		}
		vndArrayCount=0;
		vndArray=NULL;

	}
	__except( IsSupportedException(_exception_code()) ) {
		return;
	}
}

static BOOL VndInit(const char* filename)
{
	__try {
		BOOL ret=FALSE;
		VndDestroy();
		if(filename==NULL) return FALSE;
		if(sign_check_file(filename,1,0,0,0)) return FALSE;
		if(!KLDT_Init_Library(malloc,free)) return FALSE;
		if(!DATA_Init_Library(malloc,free)) return FALSE;

		HDATA hRootData=NULL;
		if(!KLDT_DeserializeUsingSWM( filename, &hRootData ))return FALSE;
		if(hRootData==NULL) return FALSE;
		if(AVP_PID_VNDROOT!=DATA_Get_Id( hRootData, 0 )){
			DATA_Remove( hRootData, 0 );
			return FALSE;
		}

		MAKE_ADDR1( hKernelAddr, AVP_PID_VNDKERNELDATA );

		HPROP hKernelProp;
		hKernelProp = DATA_Find_Prop( DATA_Find(hRootData, hKernelAddr), NULL, 0 );

		// Получить	количество элементов в массиве
		AVP_dword nCount;
		nCount = PROP_Arr_Count( hKernelProp );

		vndArray= (void**)new DWORD[nCount];
		if(vndArray==NULL)goto dr;
		
		// Сканировать массив
		AVP_dword i;
		for ( i=0; i<nCount; i++ ) {
			AVP_dword nItemSize = PROP_Arr_Get_Items( hKernelProp, i, 0, 0 );
			AVP_char *pBuffer = new AVP_char[nItemSize];
			if(pBuffer==NULL)break;
			PROP_Arr_Get_Items( hKernelProp, i, pBuffer, nItemSize );
			vndArray[i]=pBuffer;
		}
		vndArrayCount=i;
dr:
		// Удаление дерева
		DATA_Remove( hRootData, 0 );
		return ret;
	}
    __except( IsSupportedException(_exception_code()) ) {
		return 0;
	}
}



LRESULT _WINAPI_ _AvpCallback(WPARAM wParam,LPARAM lParam)
{
	__try {
		switch(wParam){
		case AVP_CALLBACK_CHECK_FILE_INTEGRITY:
			return sign_check_file( (char*)lParam, 1, vndArray, vndArrayCount, 0);
		case AVP_CALLBACK_ROTATE:
		case AVP_CALLBACK_ROTATE_OFF:
			{
				static DWORD t_count;
				DWORD c_count=GetTickCount();
				if((t_count-c_count)<100)
					return 0;
				t_count=c_count;
			}
			
		default: break;
		}
		if(cbf_ptr) return (*cbf_ptr)(wParam,lParam);

		if(s_hWnd==0)return 0;

 #ifdef SYSTOS2
  WinSendMsg(s_hWnd,WM_USER+1000,MPFROMLONG(wParam),(void*)lParam);
  return 1;
 #else
  return SendMessage(s_hWnd,wm_AVP,wParam,lParam);
 #endif
	}
    __except( IsSupportedException(_exception_code()) ) {
		return 0;
	}
}

char* _TempPath=NULL;

BOOL AVP_SetTempPath(char* name)
{
	char* ptr=new char[strlen(name)+1];
	if(!ptr)return 0;
	strcpy(ptr,name);

	if (_access(ptr,0)){
		delete ptr;
		return FALSE;
	}

	if(_TempPath)
		delete _TempPath;
	_TempPath=ptr;
	return TRUE;
}


DWORD _WINAPI_ _AvpGetTempPath( DWORD nBufferLength, LPTSTR lpBuffer )
{
	DWORD ret;
	if(_TempPath)
		ret=strlen(strncpy(lpBuffer, _TempPath, nBufferLength));
	else{
		ret=GetTempPath(nBufferLength, lpBuffer);
		AVP_SetTempPath(lpBuffer);
	}
	return ret;

}

_TCHAR* _WINAPI_ _AvpConvertChar(_TCHAR* str_,INT ct,_TCHAR* cp_)
{
#ifndef SYSTOS2
	switch(ct)
	{
	case 1:
		break;
	case 5:
		if (cp_ && (*(WORD*)cp_==0xFFFE || *(WORD*)cp_==0xFEFF))
		{
			char buff[0x200];
			memset(buff,0x200,0);
			if(*(WORD*)cp_==0xFEFF){
				char *p=cp_+2;
				char *q=cp_+3;
				while( *p || *q){
					char c=*q;
					*(q++)=*p;
					*(p++)=c;
				}
			}

			if(WideCharToMultiByte(CP_ACP,0,(LPCWSTR)(cp_+2),-1,buff,0x200,NULL,NULL))
			{
				strcpy(str_,buff);
				break;
			}
		}
	default:
	case 0:
		OemToChar(str_,str_);
		break;
		
	}
#endif
	return str_;
}

_TCHAR* _WINAPI_ _AvpStrlwr(_TCHAR* str_)
{
#ifdef _MBCS
        return strlwr(str_);
#else
        if(str_)
        {
                _TCHAR* c=str_;
                while(*c){
                        *c|=0x20;
                        c++;
                }
        }
        return str_;
#endif
}

_TCHAR* _WINAPI_ _AvpStrrchr(_TCHAR* str_, INT chr_){

#ifdef _MBCS
        return strrchr(str_,chr_);
#else

        if(str_)
        {
                _TCHAR* c=str_ + strlen(str_);
                while(c>=str_)
                {
                        if(*c==chr_){
                                        return c;
                        }
                        c--;
                }
        }
        return NULL;
#endif
}





HINSTANCE hDllInst;

HINSTANCE LoadLibraryInst( LPCTSTR name, HINSTANCE hDllInst )
{
	HINSTANCE hi;
	char DllName[MAX_PATH];
	char* lpFilePart;
	GetModuleFileName(hDllInst,DllName,MAX_PATH);
	GetFullPathName(DllName,MAX_PATH,DllName,&lpFilePart);
	strcpy(lpFilePart,name);
	hi=LoadLibrary(DllName);
	if(hi) return hi;
	else return LoadLibrary(name);
}

DWORD WINAPI ExecSpecial(
		char* FuncName, 
		DWORD wParam, 
		DWORD lParam
		)
{
	char buf[0x200];
	char DllName[MAX_PATH];
	char* lpFilePart;
	if(!FuncName || !*FuncName) return 0;

	if(!strcmp(FuncName,"GetProcAddressEx"))
	{
		HMODULE hModule = NULL;
		hModule = GetModuleHandle((LPCSTR)wParam);
		if(hModule == NULL)
			hModule = LoadLibrary((LPCSTR)wParam);
		if(hModule != NULL)
			return (DWORD)GetProcAddress(hModule,(LPCSTR)lParam);
		return 0;
	}
	
	__try {
		GetModuleFileName(hDllInst,DllName,MAX_PATH);
		GetFullPathName(DllName,MAX_PATH,DllName,&lpFilePart);
		strcpy(lpFilePart,"AVC*.DLL");

		HINSTANCE hi;
		WIN32_FIND_DATA fd;

		HANDLE fi=FindFirstFile(  DllName, &fd);
		if(fi!=INVALID_HANDLE_VALUE)
		{
			do{
				strcpy(lpFilePart,fd.cFileName);
				hi=LoadLibrary(DllName);
				if(!hi){
					strcpy(lpFilePart,fd.cAlternateFileName);
					hi=LoadLibrary(DllName);
				}
				if(hi){
					void* ptr=GetProcAddress(hi,FuncName);
					if(ptr){
						DWORD ret=0;
						if( sign_check_file( DllName, 1, vndArray, vndArrayCount, 0) )
							_AvpCallback(AVP_CALLBACK_FILE_INTEGRITY_FAILED,(long)DllName);
						else
							ret=((DWORD (*)(DWORD,DWORD))ptr)(wParam, lParam);
						FreeLibrary(hi);
						return ret;
					}
				}
				FreeLibrary(hi);
			}while(FindNextFile(fi,&fd));
		}	
		sprintf(buf,"ExecSpecial can't resolve name: %s",FuncName);
		_AvpCallback(AVP_CALLBACK_PUT_STRING,(long)buf);
	}
	__except( IsSupportedException(_exception_code()) ) {
		return 0;
	}
	return 0;
}

//#define _DEBUG_15852

#ifdef _DEBUG_15852
#define WRITE_LIMIT 0x20000
static DWORD WriteCount=0;

BOOL WINAPI _AvpWriteFile (
						   HANDLE  hFile,      // handle to file to write to
						   LPCVOID  lpBuffer,  // pointer to data to write to file
						   DWORD  nNumberOfBytesToWrite,       // number of bytes to write
						   LPDWORD  lpNumberOfBytesWritten,    // pointer to number of bytes written
						   OVERLAPPED FAR*  lpOverlapped       // addr. of structure needed for overlapped I/O
						   )
{
	if(lpNumberOfBytesWritten)*lpNumberOfBytesWritten=0;
	WriteCount+=nNumberOfBytesToWrite;
	if(WriteCount>WRITE_LIMIT) 
		return FALSE;
	return WriteFile(hFile,lpBuffer,nNumberOfBytesToWrite,lpNumberOfBytesWritten,lpOverlapped);

}
HANDLE WINAPI _AvpCreateFile (
	LPCTSTR  lpFileName,        // pointer to name of the file
	DWORD  dwDesiredAccess,     // access (read-write)
	DWORD  dwShareMode, // share mode
	SECURITY_ATTRIBUTES FAR*  lpSecurityAttributes,     // pointer to security descriptor
	DWORD  dwCreationDistribution,      // how to create
	DWORD  dwFlagsAndAttributes,        // file attributes
	HANDLE  hTemplateFile       // handle to file with attributes to copy
    )
{
	WriteCount=0;
	return CreateFile(
		lpFileName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDistribution,
		dwFlagsAndAttributes,
		hTemplateFile);
}
#endif //_DEBUG_15852


BOOL LoadIO()
{
#ifdef _MBCS
        setlocale ( LC_ALL, "" );
        _setmbcp(_MB_CP_LOCALE);
#endif

        OSVERSIONINFO os;
        os.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
        GetVersionEx(&os);

		hiIODll=LoadLibraryInst((os.dwPlatformId==VER_PLATFORM_WIN32_NT)?"avp_iont.dll":"avp_io32.dll",hDllInst);

        if(hiIODll==NULL)       return FALSE;
        AvpRead13               =(t_AvpRead13*)GetProcAddress(hiIODll,"_AvpRead13");
        if(AvpRead13==NULL)     return FALSE;
        AvpWrite13              =(t_AvpWrite13*)GetProcAddress(hiIODll,"_AvpWrite13");
        if(AvpWrite13==NULL)    return FALSE;
        AvpRead25               =(t_AvpRead25*)GetProcAddress(hiIODll,"_AvpRead25");
        if(AvpRead25==NULL)     return FALSE;
        AvpWrite26              =(t_AvpWrite26*)GetProcAddress(hiIODll,"_AvpWrite26");
        if(AvpWrite26==NULL)    return FALSE;

        AvpGetFirstMcbSeg =(t_AvpGetFirstMcbSeg*)GetProcAddress(hiIODll,"_AvpGetFirstMcbSeg");
        if(AvpGetFirstMcbSeg==NULL)        return FALSE;
        AvpGetDosMemSize =(t_AvpGetDosMemSize*)GetProcAddress(hiIODll,"_AvpGetDosMemSize");
        if(AvpGetDosMemSize==NULL)        return FALSE;
        AvpGetIfsApiHookTable =(t_AvpGetIfsApiHookTable*)GetProcAddress(hiIODll,"_AvpGetIfsApiHookTable");
        if(AvpGetIfsApiHookTable==NULL)        return FALSE;
        AvpGetDosTraceTable =(t_AvpGetDosTraceTable*)GetProcAddress(hiIODll,"_AvpGetDosTraceTable");
        if(AvpGetDosTraceTable==NULL)        return FALSE;
        AvpMemoryRead  =(t_AvpMemoryRead*)GetProcAddress(hiIODll,"_AvpMemoryRead");
        if(AvpMemoryRead==NULL)      return FALSE;
        AvpMemoryWrite  =(t_AvpMemoryWrite*)GetProcAddress(hiIODll,"_AvpMemoryWrite");
        if(AvpMemoryWrite==NULL)     return FALSE;
        
		AvpGetDiskParam  =(t_AvpGetDiskParam*)GetProcAddress(hiIODll,"_AvpGetDiskParam");
        if(AvpGetDiskParam==NULL)       return FALSE;
	
	AvpCallback = _AvpCallback;
	AvpStrrchr = _AvpStrrchr;
	AvpStrlwr = _AvpStrlwr;
	AvpConvertChar = _AvpConvertChar;
	AvpGetTempPath = _AvpGetTempPath;

#ifdef _DEBUG_15852
	AvpWriteFile = _AvpWriteFile;
	AvpCreateFile = _AvpCreateFile;
#else
	AvpWriteFile = WriteFile;
	AvpCreateFile = CreateFile;
#endif //_DEBUG_15852

	AvpCloseHandle = CloseHandle;
	AvpReadFile = ReadFile;
	AvpGetFileSize = GetFileSize;
	AvpSetFilePointer = SetFilePointer;
	AvpSetEndOfFile = SetEndOfFile;
	AvpDeleteFile = DeleteFile;
	AvpGetFileAttributes = GetFileAttributes;
	AvpSetFileAttributes = SetFileAttributes;
	
	AvpGetDiskFreeSpace = GetDiskFreeSpace;
	AvpGetFullPathName  = GetFullPathName;
	AvpGetTempFileName = GetTempFileName;
	AvpExecSpecial = ExecSpecial;
	
    return TRUE;
}

#ifndef NOAFX
/////////////////////////////////////////////////////////////////////////////
// CAvpBaseApp

BEGIN_MESSAGE_MAP(CAvpBaseApp, CWinApp)
        //{{AFX_MSG_MAP(CAvpBaseApp)
                // NOTE - the ClassWizard will add and remove mapping macros here.
                //    DO NOT EDIT what you see in these blocks of generated code!
        //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//
//CAvpBaseApp::CAvpBaseApp()
//{
//	    LoadIO();
//		BAvpInit();
//}
//
//CAvpBaseApp::~CAvpBaseApp()
//{
//    FreeLibrary(hiIODll);
//}
//CAvpBaseApp theApp;
#else


#ifdef SYSTOS2
#define DLL_PROCESS_ATTACH 1
#define DLL_THREAD_ATTACH  2
#define DLL_THREAD_DETACH  3
#define DLL_PROCESS_DETACH 0
BOOL WINAPI DllMain(DWORD  fdwReason,LPVOID lpvReserved)
#else
BOOL WINAPI DllMain(HINSTANCE hDLLInst,DWORD  fdwReason,LPVOID lpvReserved)
#endif
{
        switch (fdwReason)
        {
        case DLL_PROCESS_ATTACH:
			hDllInst=hDLLInst;
			__try {
			    if(!LoadIO())return 0;
			    if(!BAvpInit())return 0;
			}
			__except( IsSupportedException(_exception_code()) ) {
				return 0;
			}
            return 1;

        case DLL_PROCESS_DETACH:
				AVP_Destroy();
#ifdef SYSTOS2
                DosFreeModule(modHandleIO);
#else
                FreeLibrary(hiIODll);
#endif
                break;

        case DLL_THREAD_ATTACH: 
			break;
        case DLL_THREAD_DETACH: 
			break;
        }
        return TRUE;
}
#endif


/*
void AVP_Start(HWND hWnd)
 {
  if(LoadIO()==FALSE) return;//!!!!!!!!!!!!
  if(BAvpInit()==FALSE) return;//!!!!!!!!!!!!

  s_hWnd=hWnd;
 }
*/

#include "NTFSStream.h"

NTFS_ENV g_NTFSEnv;
int NTFSEnvStat=0;
BOOL killScan=0;

void ProcessStreams(AVPScanObject* so)
{
	AVPScanObject ScanObject;
	switch(NTFSEnvStat)
	{
	case 0:
		OSVERSIONINFO os;
		os.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
		GetVersionEx(&os);
		if(os.dwPlatformId!=VER_PLATFORM_WIN32_NT)
		{
			NTFSEnvStat=2;
			return;
		}

		g_NTFSEnv.nqif = NULL;
		g_NTFSEnv.hNtdll = NULL;
		g_NTFSEnv.hNtdll = LoadLibrary("ntdll.dll");
		//if (hNtdll < (HINSTANCE) 33)
		if (g_NTFSEnv.hNtdll == NULL)
		{
			NTFSEnvStat = 2;
			return;
		}
		else
		{
			g_NTFSEnv.nqif = (NQIF) GetProcAddress(g_NTFSEnv.hNtdll, "NtQueryInformationFile");
			if (g_NTFSEnv.nqif == NULL)
			{
				FreeLibrary(g_NTFSEnv.hNtdll);
				NTFSEnvStat = 2;
				return;
			}
		}
		NTFSEnvStat = 1;
		break;
	case 1:
		break;
	case 2: //error;
	default:
		return;
	}


	HANDLE hFile;
	DWORD rc;

	IO_STATUS_BLOCK iosb;
	static BYTE fsibuf[32768];
	FILE_STREAM_INFORMATION *fsi;
	char* lpTmpStr;

//  VIK 03.10.03
//	hFile = CreateFile(so->Name, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	hFile = CreateFile(so->Name, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;
	rc = g_NTFSEnv.nqif(hFile, &iosb, &fsibuf, sizeof(fsibuf), FileStreamInformation);
	CloseHandle(hFile);
	if (rc != 0)
		return;
	
	fsi = (FILE_STREAM_INFORMATION *) fsibuf;

	char FileName[MAX_PATH*2];
	lpTmpStr=FileName + wsprintf(FileName, "%s", so->Name);

	while(!killScan)
	{
		//  VIK 03.10.03
		//int l=WideCharToMultiByte(CP_ACP, 0, fsi->StreamName, -1, lpTmpStr, MAX_PATH, NULL, NULL);
		//if(l>7) lpTmpStr[l-7]=0; //cut ":data"
		//if(l!=8) //if stream name name is not empty (default body of file)

	  // fsi->StreamNameLength - размер имени в байтах
		int l=WideCharToMultiByte(CP_ACP, 0, fsi->StreamName, fsi->StreamNameLength / sizeof(wchar_t), lpTmpStr, MAX_PATH, NULL, NULL);
		lpTmpStr[l]=0;
		if(l>6) lpTmpStr[l-6]=0; //cut ":$DATA"
		if(l!=7) //if stream name name is not empty (default body of file)
		{
			memset(&ScanObject,0,sizeof(ScanObject));
			strcpy(ScanObject.Name,FileName);
			ScanObject.Type=OT_FILE;
			ScanObject.MFlags=so->MFlags;
			LONG result=BAvpProcessObject(&ScanObject);
		}
		if (fsi->NextEntryOffset == 0)
			break;

		fsi = (FILE_STREAM_INFORMATION *) ( fsi->NextEntryOffset + (BYTE *) fsi);
	}
}







LONG AVP_LoadBase(const char* name, int format)
{
	__try {
		if(name && strlen(name)){
			char filename[0x200];
			char* n;
			strcpy(filename,name);
			n=AvpStrrchr(filename,'\\');
			if(n) n++;
			else n=filename;
			strcpy(n,"AVP.VND");
			VndInit(filename);
		}
		return BAvpLoadBase((_TCHAR*)name);
	}
	__except( IsSupportedException(_exception_code()) ) {
		return 0;
	}
}

LONG AVP_UnloadBase(const char* name, int format)
{
	__try {
	    return BAvpLoadBase("");
	}
	__except( IsSupportedException(_exception_code()) ) {
		return 0;
	}
}

LONG AVP_ProcessObject(AVPScanObject* object)
{
	LONG ret;
	__try {
		killScan=0;
		ret=BAvpProcessObject(object);
		if(object->MFlags & (MF_SCAN_STREAMS))
			ProcessStreams(object);
	}
	__except( IsSupportedException(_exception_code()) ) {
		BAvpErrorFlush();
		object->RFlags|=RF_KERNEL_FAULT;
		_AvpCallback(AVP_CALLBACK_OBJECT_DONE,(DWORD)object);
		return 1;
	}
	return ret;
}

void AVP_CancelProcessObject(LONG code)
{
	killScan=1;
    BAvpCancelProcessObject(code);
}

int AVP_CheckFileIntegrity(const char* name)
{
	__try {
		return sign_check_file( name, 1, vndArray, vndArrayCount, 0);
	}
	__except( IsSupportedException(_exception_code()) ) {
		return SIGN_UNKNOWN_ERROR;
	}
}

extern "C" BOOL callbackNames;
void AVP_SetCallbackNames(BOOL cn)
{
	callbackNames=cn;
}

extern "C" DWORD callbackRecords;
void AVP_SetCallbackRecords(DWORD cn)
{
	callbackRecords=cn;
}

LONG AVP_Dispatch(AVPScanObject* object, BYTE SubType)
{
	__try {
	    return BAvpDispatch(object, SubType);
	}
	__except( IsSupportedException(_exception_code()) ) {
		return 0;
	}
}

int AVP_AddExcludeMask(const char* mask)
{
	__try {
	    return BAvpAddExcludeMask((_TCHAR*)mask);
	}
	__except( IsSupportedException(_exception_code()) ) {
		return 0;
	}
}

int AVP_AddMask(const char* mask)
{
	__try {
	    return BAvpAddMask((_TCHAR*)mask);
	}
	__except( IsSupportedException(_exception_code()) ) {
		return 0;
	}
}

LONG AVP_RegisterCallback(HWND hWnd)
{
    s_hWnd=hWnd;
    return 0;
}

LONG AVP_RegisterCallbackPtr(t_AvpCallback* cbf_ptr_)
{
    cbf_ptr=cbf_ptr_;
    return 0;
}


char* AVP_MakeFullName(char* dest,const char* src)
{
	__try {
	    return BAvpMakeFullName(dest, (char*)src);
	}
	__except( IsSupportedException(_exception_code()) ) {
		strcpy(dest,"NAME_PARSING_ERROR");
		return dest;
	}
}

extern BYTE* AVP_PagePtr;

BYTE* AVP_GetPagePtr()
{
	return AVP_PagePtr;
}

void AVP_Destroy()
{
	__try {
		VndDestroy();
		BAvpDestroy();
		if(_TempPath)
		{
			delete _TempPath;
			_TempPath=NULL;
		}
	}
	__except( IsSupportedException(_exception_code()) ) {
		return;
	}
}
