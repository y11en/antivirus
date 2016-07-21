// TestParse.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../ObjParse.h"
#include "conio.h"


void callback(DWORD type,char* name, DWORD UserValue)
{
	switch(type){
	case OBJPARSE_EXPORT:
		cprintf("\nEXPORT  ");
		break;
	case OBJPARSE_IMPORT:
		cprintf("\nIMPORT  ");
		break;
	case OBJPARSE_DATA:
		cprintf("\nDATA    ");	//! error
		break;
	case OBJPARSE_BSS:
		cprintf("\nBSS     ");	//! error
		break;
	default:
		cprintf("\nUNKNOWN ");	//! error
		break;
	}
	cprintf(name);
}


int main(int argc, char* argv[])
{
	HANDLE h;
	if(argc>1 && INVALID_HANDLE_VALUE!=(h=CreateFile(argv[1],GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0)))
	{
		cprintf("\nStart parsing of %s\n",argv[1]);
		DWORD len=GetFileSize(h,0);
		if(len){
			BYTE* body=new BYTE[len];
			if(body){
				if(ReadFile(h,body,len,&len,0)){
					ObjParse(body,len,callback,0);
				}
				delete body;
			}
		}
		cprintf("\n\nAll done.\n");
		CloseHandle(h);
	}
	else{
		cprintf("\nUsage: TestParse objectmodulename\n",argv[1]);
	}
	cprintf("\nPress Any Key...\n");
	getch();
	return 0;
}
