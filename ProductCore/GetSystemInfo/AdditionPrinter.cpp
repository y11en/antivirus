#include "stdafx.h"
#include "SysInfoDefines.h"
#include "AdditionPrinter.h"
#include "FileVersion.h"
#include <Winspool.h>
#include "MemoryScan.h"

extern SI_INSIDE_OS_NUMBER	os;

extern FILE*				LFile;

void GetDefaulpPrinterName(char** chName)
{
	DWORD	dwNeeded=0;
	DWORD	dwReturned=0;
	TCHAR	cBuffer[MAX_PATH];

	__try
	{
		if (chName==NULL) return;
		if (os<OS_UNKNOWN)
		{
			EnumPrinters(PRINTER_ENUM_DEFAULT,NULL,2,NULL,0,&dwNeeded,&dwReturned);
			if (dwNeeded!=0)
			{
				PRINTER_INFO_2* ppi2=NULL;
				ppi2=(PRINTER_INFO_2*)malloc(dwNeeded);
				if (ppi2!=NULL)
				{
					if (EnumPrinters(PRINTER_ENUM_DEFAULT,NULL,2,(LPBYTE)ppi2,dwNeeded,&dwNeeded,&dwReturned)) strcpy(*chName,ppi2->pPrinterName);
					free(ppi2);
				}
			}
		}
		else
		{
			if (GetProfileString("windows","device",",,,",cBuffer,MAX_PATH) <= 0) return;
			if (strtok(cBuffer,",")!=NULL) strcpy(*chName,cBuffer);
		}
	}
	__finally
	{
	}
}

void GetPrinterDriverInfo(char PrinterName[],char** chName)
{
	HANDLE			hPrinter=NULL;
	DWORD			dwReturned=0;
	DRIVER_INFO_2*	PrntDrInfo=NULL;
				
	__try
	{
		if (chName==NULL) return;
		if (OpenPrinter((char*)PrinterName,&hPrinter,NULL))
		{
			GetPrinterDriver(hPrinter,NULL,3,NULL,0,&dwReturned);
			if (dwReturned!=0)
			{
				PrntDrInfo=(DRIVER_INFO_2*)malloc(dwReturned);
				if (PrntDrInfo!=NULL)
				{
					if (GetPrinterDriver(hPrinter,NULL,2,(LPBYTE)PrntDrInfo,dwReturned,&dwReturned))
					{
						strcpy(*chName,PrntDrInfo->pDriverPath);
					}
				}
			}
		}
		return;
	}
	__finally
	{
		if (PrntDrInfo!=NULL) free(PrntDrInfo);
		if (hPrinter!=NULL) ClosePrinter(hPrinter);
	}
}

void AddPrinter(char chPrintName[],char chDriverName[],BOOL Default)
{
	TCHAR			chPrinterModel[MAX_PATH]="";
	TCHAR			chFileName[MAX_PATH]="";
	TCHAR			chExt[MAX_PATH]="";
	
	__try
	{
		if (strlen(chPrintName)==0) return;
		chFileName[0]=0;
		chExt[0]=0;
		_splitpath(chPrintName,NULL,NULL,chFileName,chExt);
		_makepath(chPrinterModel,NULL,NULL,chFileName,chExt);
		if (Default) fprintf(LFile,"\t\t%s - default printer\n\t\t\tDRIVER - %s, ",chPrinterModel,chDriverName);
		else fprintf(LFile,"\t\t%s\n\t\t\tDRIVER - %s, ",chPrinterModel,chDriverName);
		fflush(LFile);
		GetFileInfoAsRepInfo(chDriverName);
		fflush(LFile);
		return;
	}
	__finally
	{
	}
}