////////////////////////////////////////////////////////////////////
//
//  SCANDIR.CPP
//  Folder enumerator/scaner
//  AVP Project
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1996
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "scandir.h"
#include <ScanAPI/scanobj.h>
//#include "../retcode.h"
#include <ScanAPI/avp_dll.h>
#include <ScanAPI/avp_msg.h>

const UINT	 wm_AVP = RegisterWindowMessage( AVP_MSGSTRING );



CScanDir::CScanDir(const char* dirName){
    Path=dirName;
	int i;
	while((i=Path.GetLength()))
	{
		i--;
		if(Path.ReverseFind('\\')!=i) break;
		Path=Path.Left(i);
	}
}

CScanDir::~CScanDir(){
    while(!DirList.IsEmpty()) delete (CString*) DirList.RemoveHead();
}

LRESULT CScanDir::Dispatch( CWnd* pWnd, int* killScan, DWORD MFlags, BOOL recurse){
	
	AVPScanObject ScanObject;
    WIN32_FIND_DATA fd;     
	fd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY |FILE_ATTRIBUTE_HIDDEN;
   	
	pWnd->SendMessage(wm_AVP, AVP_INTERNAL_CHECK_DIR,(long)(const char*)Path);

    HANDLE H=FindFirstFile( (const char*)(Path+"\\*.*"), &fd ); 
	if(H==INVALID_HANDLE_VALUE) return *killScan;

    do{
		if(*killScan) break;

		CString Name=Path + "\\" + fd.cFileName;
		if(!strlen(fd.cFileName)||(DWORD)-1==GetFileAttributes(Name))
		{
    		Name=Path + "\\" + fd.cAlternateFileName;
			if(!strlen(fd.cAlternateFileName)||(DWORD)-1==GetFileAttributes(Name))
				continue;
		}

		if(recurse && fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
			if(!strcmp(fd.cFileName,".")) continue;
			if(!strcmp(fd.cFileName,"..")) continue;
    	    DirList.AddTail( new CString(Name));
		}
		memset(&ScanObject,0,sizeof(ScanObject));
		strcpy(ScanObject.Name,Name);
		ScanObject.Type=OT_FILE;
		ScanObject.MFlags=MFlags | MF_SCAN_STREAMS;
		ScanObject.Size=fd.nFileSizeLow;
		LONG result=AVP_ProcessObject(&ScanObject);

    }while( FindNextFile(H, &fd ));

	FindClose(H);

    while(!DirList.IsEmpty() && !*killScan){
    	CString* name = (CString*) DirList.RemoveHead();
   		CScanDir* SD=new CScanDir((const char*)(*name));
   		SD->Dispatch(pWnd, killScan, MFlags, recurse);
    	delete SD;
    	delete (CString*)name;
    }
	return *killScan;
}		

