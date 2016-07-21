// KLE_APP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <winioctl.h>
#include <conio.h>
#include <io.h>

#include "klh.h"

BOOL NTControlService(char* name_, int func_);
void PrintRepInfo( HANDLE hfile);
	
int main(int argc, char* argv[])
{
	HANDLE hfile;
	DWORD count = 0L;
	BOOL unloadKLH=0;
	BOOL unloadKLE=0;
	DWORD result= 0L;
	
	cprintf("\nKLE Test APP started.\n");

	if(argc<2 || _access(argv[1],4)){
		cprintf("\nCommand Line: KLE_APP filename\n");
		goto err_drv;
	}

/*	char buf_vi[0x400];
	GetFileVersionInfo("e:\\avp32\\release\\kle_vxd.vxd",0,0x400,buf_vi);
	result=GetLastError();
*/	

	OSVERSIONINFO os;
	os.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&os);
	
	if(os.dwPlatformId==VER_PLATFORM_WIN32_NT)
	{
		if(!NTControlService("KLE",2)){
			if(!NTControlService("KLE",1)){
				cprintf("Engine Driver can't be loaded.\n");
				goto err_drv;
			}
			unloadKLE=1;
			cprintf("Engine Driver Loaded.\n");
		}
		if(!NTControlService("KLH",2)){
			if(!NTControlService("KLH",1)){
				cprintf("Helper Driver can't be loaded.\n");
				goto err_drv;
			}
			unloadKLH=1;
			cprintf("Helper Driver Loaded.\n");
		}
		hfile=CreateFile(KLH_DEVICE_NAME_NT,
			GENERIC_READ|GENERIC_WRITE,	FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,OPEN_EXISTING,0,NULL);
	}else{
		char* lp;
		char dName[0x200];
		strcpy(dName,"\\\\.\\");
		GetModuleFileName(0,dName+4,0x1FB);
		GetShortPathName(dName+4,dName+4,0x1FB);//!!!
		lp=strrchr(dName,'\\');
		lp=lp?(lp+1):(dName+4);
		strcpy(lp, KLH_DEVICE_NAME_9X ".VXD");

		CharToOem(dName,dName); //!!!!!!!
		hfile=CreateFile(dName,
			0, FILE_SHARE_READ|FILE_SHARE_WRITE, 
			NULL,OPEN_EXISTING,FILE_FLAG_DELETE_ON_CLOSE,NULL);
	}
	if (hfile != INVALID_HANDLE_VALUE ){
		char setname[]="AVP.SET";
		char my_setname[]="C:\\Program Files\\Common Files\\AVP Shared Files\\Avpbases\\avp.set";
		
		cprintf("Helper Driver Opened.\n");
		if(!DeviceIoControl(hfile, 
			KLH_IOCTL_LOAD_BASES, 
			setname,strlen(setname)+1,0,0,&count,0))
		{
			if(!DeviceIoControl(hfile, 
				KLH_IOCTL_LOAD_BASES, 
				my_setname,strlen(my_setname)+1,0,0,&count,0))
			{
				cprintf("Base Loading FAILED.\n");
				goto cloh;
			}
		}

		PrintRepInfo(hfile);
		
		cprintf("Scaning %s.\n",argv[1]);
		if(!DeviceIoControl(hfile, 
			KLH_IOCTL_SCAN_FILE, 
			argv[1],strlen(argv[1])+1,&result,sizeof(DWORD)*128,&count,0))
		{
			cprintf("Scaning FAILED.\n");
			goto cloh;
		}
		cprintf("Scan result=%08X  %s\n",result,((result)&0xF)?"INFECTED":"");

cloh:
		
		PrintRepInfo(hfile);

		CloseHandle(hfile);
	}
err_drv:
	if(unloadKLH)
	{
		NTControlService("KLH",0);
		cprintf("Helper Driver Unloaded.\n");
	}
	if(unloadKLE)
	{
		NTControlService("KLE",0);
		cprintf("Engine Driver Unloaded.\n");
	}
	cprintf("Press any key...\n");
	getch();
	return (int)result;
}


void PrintRepInfo( HANDLE hfile){
	KLH_REP_INFO ro;
	DWORD count;
	while(DeviceIoControl(hfile, 
		KLH_IOCTL_GET_REP_INFO, 
		0,0,&ro,sizeof(KLH_REP_INFO),&count,0))
	{
		cprintf("CB %08X %08X: %s %s %s\n", ro.Param1, ro.Param2 ,ro.String1, ro.String2, ro.DebugStr);
	}
}


BOOL NTControlService(char* name_, int func_)
{
    SC_HANDLE schSCManager;
    SC_HANDLE  schService;
    SERVICE_STATUS ss;
    BOOL ret=FALSE;
	BOOL loaded=FALSE;
	
    schSCManager=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	
    if(!schSCManager)      return FALSE;

	//    schService=OpenService(schSCManager,name_,SERVICE_QUERY_STATUS);
    schService=OpenService(schSCManager,name_,SERVICE_ALL_ACCESS);
    if(!schService) goto retSCM;
    
	if (!QueryServiceStatus(schService, &ss))
		goto retS;
	
	if (ss.dwCurrentState == SERVICE_START_PENDING 
		|| ss.dwCurrentState == SERVICE_RUNNING 
		|| ss.dwCurrentState == SERVICE_CONTINUE_PENDING)
		loaded = TRUE;
	
	switch(func_)
	{
	case 1: //start service
		if(loaded) ret=TRUE;
		else	ret= StartService(schService,0,NULL);
		break;
	case 0: //stop service
		if(!loaded)ret=TRUE;
		else if (ControlService(schService,SERVICE_CONTROL_STOP,&ss))
		{
			int i = 0;
			do{
				Sleep(500);
				QueryServiceStatus(schService, &ss);
				i++;
			} while (ss.dwCurrentState == SERVICE_STOP_PENDING && i < 30); // Default go-out time for a service is 30 secs
		}
		break;
	default: //return service status
		ret=loaded;
		break;
    }
	
retS:
	CloseServiceHandle(schService);
retSCM:
	CloseServiceHandle(schSCManager);
    return ret;
}

