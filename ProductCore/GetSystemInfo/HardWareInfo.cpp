#include "stdafx.h"
#include "HardWareInfo.h"
#include <Mmsystem.h>
#include "HardDriveWork.h"
#include "AdditionPrinter.h"
#include "AssociatedString.h"
#include "MemoryScan.h"

extern SI_INSIDE_OS_NUMBER	os;

extern HMODULE				hKernel;
extern FILE*					LFile;

bool MemoryInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	MEMORYSTATUS	MemStat;
	bool			Ret=false;
	__try
	{
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,1);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
		if (CountStruct) CountStruct->LocalStepCount=1;
		memset(&MemStat,0,sizeof(MEMORYSTATUS));
		GlobalMemoryStatus(&MemStat);
		if ((MemStat.dwTotalPhys>>20)>0) fprintf(LFile,"Total phisical memory:\n\t\t%d,%0.3d Mb",MemStat.dwTotalPhys>>20,(MemStat.dwTotalPhys&0xffc00)>>10);
		else fprintf(LFile,"Total phisical memory:\n\t\t%d Kb",MemStat.dwTotalPhys>>10);
		if ((MemStat.dwTotalVirtual>>20)>0) fprintf(LFile,"\n\nTotal virtual memory:\n\t\t%d,%0.3d Mb",MemStat.dwTotalVirtual>>20,(MemStat.dwTotalVirtual&0xffc00)>>10);
		else fprintf(LFile,"\n\nTotal virtual memory:\n\t\t%d,%0.3d Kb",MemStat.dwTotalVirtual>>10);
		if ((MemStat.dwTotalPhys>>20)>0) fprintf(LFile,"\n\nAvailable phisical memory:\n\t\t%d,%0.3d Mb",MemStat.dwTotalPhys>>20,(MemStat.dwTotalPhys&0xffc00)>>10);
		else fprintf(LFile,"\n\nAvailable phisical memory:\n\t\t%d,%0.3d Kb",MemStat.dwTotalPhys>>10);
		if ((MemStat.dwAvailVirtual>>20)>0) fprintf(LFile,"\n\nAvailable virtual memory:\n\t\t%d,%0.3d Mb\n\n",MemStat.dwAvailVirtual>>20,(MemStat.dwAvailVirtual&0xffc00)>>10);
		else fprintf(LFile,"\n\nAvailable virtual memory:\n\t\t%d,%0.3d Kb\n\n",MemStat.dwAvailVirtual>>10);
		fflush(LFile);
		Ret=CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv);
		return Ret;
	}
	__finally
	{
	}
	return Ret;
}

bool ProcessorInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	TCHAR		strCpuInfo[MAX_PATH]="";
	TCHAR*		pstrCpuInfo=strCpuInfo;
	DWORD		dwSpeed=0;
	DWORD		ProcessorNum=0;
//	SYSTEM_INFO stCpuInfo;
	TCHAR		buf[20];

	TCHAR		VendorID[16]="Unknown";
	DWORD		CacheEax=0;
	DWORD		CacheEbx=0;
	DWORD		CacheEcx=0;
	DWORD		CacheEdx=0;
	DWORD		Signature=0;
	DWORD		FeatureEbx=0;
	DWORD		FeatureEcx=0;
	DWORD		Features=0;
	
	try
	{
		if (hProgressCtrl)
		{
			SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
			SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,PROCESSOR_STEP_COUNT);
			SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
		}
		if (CountStruct) CountStruct->LocalStepCount=PROCESSOR_STEP_COUNT;
		if (!determineCpuId()) 
		{
			if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,PROCESSOR_STEP_COUNT,0);
			CountStruct->LocalStep=CountStruct->LocalStepCount;
			if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) return true;
			return false;
		}
		if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) return true;
		DWORD dwHighestCpuId = determineHighestCpuId((TCHAR*)VendorID);
		if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) return true;
		switch (dwHighestCpuId)
		{
		default:
		case 2:     // intel cpu's find cache information here
			determineIntelCacheInfo(&CacheEax,&CacheEbx,&CacheEcx,&CacheEdx);
		case 1:     // x86 cpu's do processor identification here
			determineCpuIdentification(&Signature,&FeatureEbx,&FeatureEcx,&Features);
			break;
		case 0:     // don't do anything funky; return
			if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,PROCESSOR_STEP_COUNT-2,0);
			CountStruct->LocalStep=CountStruct->LocalStepCount;
			if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) return true;
			return false;
		}
		if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) return true;
		if (getVendor(VendorID)==VENDOR_INTEL) determineOldIntelName(dwHighestCpuId,Signature,CacheEax,CacheEbx,CacheEcx,CacheEdx,FeatureEbx,Features,&pstrCpuInfo);
		if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) return true;
		DWORD dwLargestExtendedFeature = determineLargestExtendedFeature(); 
		if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) return true;
		if (dwLargestExtendedFeature >= NAMESTRING_FEATURE) determineNameString(&pstrCpuInfo); 
		if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) return true;
		dwSpeed=determineCpuSpeed(getVendor(VendorID),Signature);
		if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) return true;
		if (dwSpeed!=0) strcat(strcat(strcat(strCpuInfo," "),_ltoa(dwSpeed,buf,10))," MHz");
		else strcat(strCpuInfo," Unknown speed");
		while (strCpuInfo[0]==' ') strcpy(strCpuInfo,strCpuInfo+1);
		fprintf(LFile,"Processor:\n\t\t%s\n\n",strCpuInfo);
		fflush(LFile);
		if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) return true;
	}
	catch(...){};
	return false;
}

DWORD determineCpuSpeed(DWORD VendorID,DWORD Signature)
{
	DWORD	dwSpeed=0;
	try
	{
		if ( ((VendorID==VENDOR_INTEL)&&(((Signature>>8)&0x0f)>4))||((VendorID==VENDOR_AMD)&&(((Signature>>8)&0x0f)>5)))
		{
			DWORD dwStartingPriority = GetPriorityClass(GetCurrentProcess());
			SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS);
			if ((dwSpeed=calculateCpuSpeed())==0)
			{
				dwSpeed=getCpuSpeedFromRegistry();
			}
			SetPriorityClass(GetCurrentProcess(),dwStartingPriority);
		}
	}
	catch(...){}
	return dwSpeed;
}

DWORD calculateCpuSpeed(void)
{
	DWORD dwStartTicks = 0;
	DWORD dwEndTicks = 0;
	DWORD dwTotalTicks = 0;
	DWORD dwCpuSpeed = 0;
	__int64 n64Frequency ;
	__int64 n64Start;
	__int64 n64Stop ;

	try
	{
		if (QueryPerformanceFrequency((LARGE_INTEGER*)&n64Frequency)) 
		{
			QueryPerformanceCounter((LARGE_INTEGER*)&n64Start);
			dwStartTicks=determineTimeStamp();
			Sleep(300);
			dwEndTicks=determineTimeStamp();
			QueryPerformanceCounter((LARGE_INTEGER*)&n64Stop);
			dwTotalTicks=dwEndTicks-dwStartTicks;
			dwCpuSpeed=(DWORD)(dwTotalTicks/((1000000*(n64Stop-n64Start))/n64Frequency));
		} 
		else dwCpuSpeed = 0;
	}
	catch(...){}
	return (dwCpuSpeed);
}

////DWORD calculateCpuSpeedMethod2(void)
////{
////	int   nTimeStart = 0;
////	int   nTimeStop = 0;
////	DWORD dwStartTicks = 0;
////	DWORD dwEndTicks = 0;
////	DWORD dwTotalTicks = 0;
////	DWORD dwCpuSpeed = 0;
////
////	try
////	{
//////		nTimeStart=timeGetTime();
////		for(;;)
////		{
//////			nTimeStop = timeGetTime();
////			if ((nTimeStop - nTimeStart) > 1)
////			{
////				dwStartTicks = determineTimeStamp();
////				break;
////			}
////		}
////		nTimeStart = nTimeStop;
////		for(;;)
////		{
//////			nTimeStop = timeGetTime();
////			if ((nTimeStop - nTimeStart) > 500)    // one-half second
////			{
////				dwEndTicks = determineTimeStamp();
////				break;
////			}
////		}
////		dwTotalTicks = dwEndTicks - dwStartTicks;
////		dwCpuSpeed = dwTotalTicks / 500000;
////	}
////	catch(...){}
////	return (dwCpuSpeed);
////}
////
DWORD getCpuSpeedFromRegistry(void)
{
	HKEY hKey=NULL;
	LONG result;
	DWORD dwSpeed = 0;
	DWORD dwType = 0;
	DWORD dwSpeedSize;
	
	try
	{
		result = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Hardware\\Description\\System\\CentralProcessor\\0",0,KEY_QUERY_VALUE,&hKey);
		if (result == ERROR_SUCCESS)
		{
			result = ::RegQueryValueEx(hKey,"~MHz", NULL, NULL, (LPBYTE)&dwSpeed, &dwSpeedSize);
			if (result != ERROR_SUCCESS) dwSpeed = 0;
		}
	}
	catch(...){}
	if (hKey!=NULL) RegCloseKey(hKey);
	return (dwSpeed);
}

DWORD determineTimeStamp(void)
{
	DWORD dwTickVal=0;
	__try
	{
		__asm
		{
			_emit 0Fh
			_emit 31h
			mov   dwTickVal, eax
		}
		return (dwTickVal);
	}
	__finally
	{
	}
	return (dwTickVal);
}

BOOL determineCpuId(void)
{
	DWORD	fKnowsCpuId=0;
	try
	{
		__asm
		{
			pushfd                      // save EFLAGS to stack.
			pop     eax                 // store EFLAGS in EAX.
			mov     edx, eax            // save in EBX for testing later.
			xor     eax, 0200000h       // switch bit 21.
			push    eax                 // copy "changed" value to stack.
			popfd                       // save "changed" EAX to EFLAGS.
			pushfd
			pop     eax
			xor     eax, edx            // See if bit changeable.
			jnz     has_cpuid     // if so, mark 
			mov     fKnowsCpuId, 0      // if not, put 0
			jmp     done
has_cpuid:
			mov     fKnowsCpuId, 1      // put 1
done:
		}
	}
	catch(...){}
	return ((BOOL)fKnowsCpuId);
}

DWORD determineHighestCpuId(TCHAR* VendorID)
{
	DWORD dwHighest = 0;
	char szTemp[16] = {0};
	
	try
	{
		__asm
		{
			mov      eax, 0
			CPUID
			
			test    eax,eax                   // 0 is highest function, then don't query more info
			jz      no_features
			
			mov     dwHighest, eax            // highest supported instruction
			mov     DWORD PTR [szTemp+0],ebx  // Stash the manufacturer string for later
			mov     DWORD PTR [szTemp+4],edx
			mov     DWORD PTR [szTemp+8],ecx
no_features:
		}
		if (dwHighest != 0) strcpy(VendorID,szTemp);
	}
	catch(...){}
	return (dwHighest);
}

void determineIntelCacheInfo(DWORD* CacheEax,DWORD* CacheEbx,DWORD* CacheEcx,DWORD* CacheEdx)
{
	DWORD dwEax = 0;
	DWORD dwEbx = 0;
	DWORD dwEcx = 0;
	DWORD dwEdx = 0;
	
	try
	{
		__asm
		{
			mov      eax, 2
			CPUID
			cmp      al, 1
			jne      end_find_intel_cacheinfo
			
			// if one iteration isn't enough, this code won't produce anything meaningful!
			// this is from intel; look into changing it
			mov      dwEax, eax
			mov      dwEbx, ebx
			mov      dwEcx, ecx
			mov      dwEdx, edx
end_find_intel_cacheinfo:
		}
		if (CacheEax!=NULL) *CacheEax=dwEax;
		if (CacheEbx!=NULL) *CacheEbx=dwEbx;
		if (CacheEcx!=NULL) *CacheEcx=dwEcx;
		if (CacheEdx!=NULL) *CacheEdx=dwEdx;
	}
	catch(...){}
	return;
}

void determineCpuIdentification(DWORD* Signature,DWORD* FeatureEbx,DWORD* FeatureEcx,DWORD* Features)
{
	DWORD dwSignature = 0;
	DWORD dwFeatureEbx = 0;
	DWORD dwFeatureEcx = 0 ;
	DWORD dwFeatures = 0;
	
	__try
	{
		__asm
		{
			mov      eax, 1
			CPUID
			mov      [dwSignature], eax     // store CPU signature
			mov      [dwFeatureEbx], ebx    
			mov      [dwFeatureEcx], ecx
			mov      [dwFeatures], edx      // features
		}  
		if (Signature!=NULL) *Signature=dwSignature;
		if (FeatureEbx!=NULL) *FeatureEbx=dwFeatureEbx;
		if (FeatureEcx!=NULL) *FeatureEcx=dwFeatureEcx;
		if (Features!=NULL) *Features=dwFeatures;
		return;
	}
	__finally
	{
	}
}

e_CPUVENDORS getVendor(TCHAR Vendor[16])
{
	e_CPUVENDORS	eVendor=VENDOR_UNKNOWN;
	__try
	{
		if ((strcmp(Vendor,VENDOR_INTEL_STR))==0) eVendor=VENDOR_INTEL;
		if (strcmp(Vendor,VENDOR_AMD_STR)==0) eVendor=VENDOR_AMD;
		if (strcmp(Vendor,VENDOR_CYRIX_STR)==0) eVendor=VENDOR_CYRIX;
		if (strcmp(Vendor,VENDOR_CENTAUR_STR)==0) eVendor=VENDOR_CENTAUR;
		return eVendor;
	}
	__finally
	{
	}
	return eVendor;
}

void determineOldIntelName(DWORD dwHighestCpuId,DWORD Signature,DWORD CacheEax,DWORD CacheEbx,DWORD CacheEcx,DWORD CacheEdx,DWORD FeatureEbx,DWORD Features,char** strCpuInfo)
{
   struct brand_entry
   {
      long  lBrandValue;
      char* pszBrand;
   };

   struct brand_entry brand_table[BRANDTABLESIZE] = 
   {
      1, "Genuine Intel Celeron(TM) processor",
      2, "Genuine Intel Pentium(R) III processor",
      3, "Genuine Intel Pentium(R) III Xeon(TM) processor",
      8, "Genuine Intel Pentium(R) 4 processor"
   };

   __try
   {
	   if (dwHighestCpuId<NAMESTRING_FEATURE)
	   {
		  switch ((Signature>>8)&0x0f)
		  {
		  case 4:  // 486
			 switch((Signature>>4)&0x0f)
			 {
			 case 0:
			 case 1:
				strcpy(*strCpuInfo,"Intel486(TM) DX processor");
				break;
			 case 2:
				strcpy(*strCpuInfo,"Intel486(TM) SX processor");
				break;
			 case 3:
				strcpy(*strCpuInfo,"IntelDX2(TM) processor");
				break;
			 case 4:
				strcpy(*strCpuInfo,"Intel486(TM) processor");
				break;
			 case 5:
				strcpy(*strCpuInfo,"IntelSX2(TM) processor");
				break;
			 case 7:
				strcpy(*strCpuInfo,"Writeback Enhanced IntelDX2(TM) processor");
				break;
			 case 8:
				strcpy(*strCpuInfo,"IntelDX4(TM) processor");
				break;
			 default:
				strcpy(*strCpuInfo,"Intel 486 processor");
				break;
				}

				break;

		  case 5:  // pentium
			 strcpy(*strCpuInfo,"Intel Pentium(R) processor");
			 break;
			 
		  case 6:  // pentium II and family
			  switch ((Signature>>4)&0x0f)
			  {
			  case 1:
				  strcpy(*strCpuInfo,"Intel Pentium(R) Pro processor");
				  break;
			  case 3:
				  strcpy(*strCpuInfo,"Intel Pentium(R) II processor, model 3");
				  break;
			  case 5:
			  case 7:
				  if (determineCeleron(CacheEax,CacheEbx,CacheEcx,CacheEdx)) strcpy(*strCpuInfo,"Intel Celeron(TM) processor, model 5");
				  else
				  {
					  if (determineXeon(CacheEax,CacheEbx,CacheEcx,CacheEdx))
					  {
						  if (((Signature>>4)&0x0f)==5) strcpy(*strCpuInfo,"Intel Pentium(R) II Xeon(TM) processor");
						  else strcpy(*strCpuInfo,"Intel Pentium(R) III Xeon(TM) processor");
					  }
					  else
					  {
						  if (((Signature>>4)&0x0f)==5) strcpy(*strCpuInfo,"Intel Pentium(R) II processor, model 5");
						  else strcpy(*strCpuInfo,"Intel Pentium(R) III processor");
					  }
				  }
				  
				  break;
				  
			  case 6:
				  strcpy(*strCpuInfo,"Intel Celeron(TM) processor, model 6");
				  break;
			  case 8:
				  strcpy(*strCpuInfo,"Intel Pentium(R) III Coppermine processor");
				  break;
			  default:
				  {
					  int brand_index = 0;
					  while ((brand_index<BRANDTABLESIZE)&&((FeatureEbx&0xff)!=(DWORD)brand_table[brand_index].lBrandValue)) brand_index++;
					  if (brand_index<BRANDTABLESIZE) strcpy(*strCpuInfo,brand_table[brand_index].pszBrand);
					  else strcpy(*strCpuInfo,"Unknown Genuine Intel processor");
					  
				  }
				  break;
			  } 
		  }
		  
		  if ((Features&MMX_FLAG)==MMX_FLAG) strcat(*strCpuInfo," with MMX");
	   }
	   return;
   }
   __finally
   {
   }
}

bool determineCeleron(DWORD dwEax,DWORD dwEbx,DWORD dwEcx,DWORD dwEdx)
{
	bool	IsCeleron=false;
	
	__try
	{
		if (((dwEax&0xFF000000)==0x40000000)|((dwEbx&0xFF000000)==0x40000000)|((dwEcx&0xFF000000)==0x40000000)|((dwEdx&0xFF000000)==0x40000000)) IsCeleron=true;
		if (((dwEax&0xFF0000)==0x400000)|((dwEbx&0xFF0000)==0x400000)|((dwEcx&0xFF0000)==0x400000)|((dwEdx&0xFF0000)==0x400000)) IsCeleron=true;
		if (((dwEax&0xFF00)==0x4000)|((dwEbx&0xFF00)==0x4000)|((dwEcx&0xFF00)==0x4000)|((dwEdx&0xFF00)==0x4000)) IsCeleron=true;
		if ((dwEax&0xFF)==0x40000000) IsCeleron=true;
		return IsCeleron;
	}
	__finally
	{
	}
	return IsCeleron;
}

bool determineXeon(DWORD dwEax,DWORD dwEbx,DWORD dwEcx,DWORD dwEdx)
{
	bool	IsXeon=false;
	
	__try
	{
		if ((((dwEax&0xFF000000)>=0x44000000)&&((dwEax&0xFF000000)<=0x45000000))
			|(((dwEbx&0xFF000000)>=0x44000000)&&((dwEbx&0xFF000000)<=0x45000000))
			|(((dwEcx&0xFF000000)>=0x44000000)&&((dwEcx&0xFF000000)<=0x45000000))
			|(((dwEdx&0xFF000000)>=0x44000000)&&((dwEdx&0xFF000000)<=0x45000000))) IsXeon=true;
		if ((((dwEax&0xFF0000)>=0x440000)&&((dwEax&0xFF0000)<=0x450000))
			|(((dwEbx&0xFF0000)>=0x440000)&&((dwEbx&0xFF0000)<=0x450000))
			|(((dwEcx&0xFF0000)>=0x440000)&&((dwEcx&0xFF0000)<=0x450000))
			|(((dwEdx&0xFF0000)>=0x440000)&&((dwEdx&0xFF0000)<=0x450000))) IsXeon=true;
		if ((((dwEax&0xFF00)>=0x4400)&&((dwEax&0xFF00)<=0x4500))
			|(((dwEbx&0xFF00)>=0x4400)&&((dwEbx&0xFF00)<=0x4500))
			|(((dwEcx&0xFF00)>=0x4400)&&((dwEcx&0xFF00)<=0x4500))
			|(((dwEdx&0xFF00)>=0x4400)&&((dwEdx&0xFF00)<=0x4500))) IsXeon=true;
		if (((dwEax&0xFF)>=0x44000000)&&((dwEax&0xFF)<=0x45000000)) IsXeon=true;
		return IsXeon;
	}
	__finally
	{
	}
	return IsXeon;
}

DWORD determineLargestExtendedFeature(void)
{
	DWORD dwHighest = 0;
	
	_try
	{
		__asm
		{
			mov      eax, 0x80000000
			CPUID
			mov dwHighest, eax
		}
		return (dwHighest);
	}
	__finally
	{
	}
	return (dwHighest);
}

void determineNameString(char** strCpuInfo)
{
	char szName[64] = {0};

	__try
	{
		__asm
		{
			mov      eax, 0x80000002
			CPUID
			mov      DWORD PTR [szName+0],eax
			mov      DWORD PTR [szName+4],ebx
			mov      DWORD PTR [szName+8],ecx
			mov      DWORD PTR [szName+12],edx
			mov      eax, 0x80000003
			CPUID
			mov      DWORD PTR [szName+16],eax
			mov      DWORD PTR [szName+20],ebx
			mov      DWORD PTR [szName+24],ecx
			mov      DWORD PTR [szName+28],edx
			mov      eax, 0x80000004
			CPUID
			mov      DWORD PTR [szName+32],eax
			mov      DWORD PTR [szName+36],ebx
			mov      DWORD PTR [szName+40],ecx
			mov      DWORD PTR [szName+44],edx
		}
		strcpy(*strCpuInfo,szName);
		return;
	}
	__finally
	{
	}
}

bool BIOSInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	TCHAR	strBIOSInfo[MAX_PATH]="Unknown";
	HKEY	hKey=NULL;
	BYTE	VerBuf[MAX_PATH];
	DWORD	ValType=REG_SZ;
	DWORD	Size=MAX_PATH;
	bool			Ret=false;
	__try
	{
		SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
		SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,1);
		SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
		if (CountStruct) CountStruct->LocalStepCount=1;
		if (os>OS_UNKNOWN)
		{
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"HARDWARE\\DESCRIPTION\\System",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
			{
				if (os<OS_WIN2000)
				{
					ValType=REG_BINARY;
					if (RegQueryValueEx(hKey,"SystemBiosVersion",NULL,&ValType,(LPBYTE)&strBIOSInfo,&Size)!=ERROR_SUCCESS) strcpy(strBIOSInfo,"Unknown");
				}
				else
				{
					ValType=REG_MULTI_SZ;
					if (RegQueryValueEx(hKey,"SystemBiosVersion",NULL,&ValType,(LPBYTE)&VerBuf,&Size)==ERROR_SUCCESS)
					{
						if (ValType==REG_MULTI_SZ)
						{
							while (((char*)VerBuf)[strlen((char*)VerBuf)+1]!=0) ((char*)VerBuf)[strlen((char*)VerBuf)]=' ';
						}
						strcpy(strBIOSInfo,(char*)VerBuf);
						ValType=REG_SZ;
						Size=MAX_PATH;
						if (RegQueryValueEx(hKey,"SystemBiosDate",NULL,&ValType,(LPBYTE)&VerBuf,&Size)==ERROR_SUCCESS)
						{
							strcat(strcat(strBIOSInfo," BIOS Date: "),(char*)VerBuf);
						}
					}
					else 
					{
						ValType=REG_SZ;
						Size=MAX_PATH;
						if (RegQueryValueEx(hKey,"SystemBiosDate",NULL,&ValType,(LPBYTE)&VerBuf,&Size)==ERROR_SUCCESS) strcat(strcpy(strBIOSInfo,"BIOS Date: "),(char*)VerBuf);
						else strcpy(strBIOSInfo,"Unknown");
					}
				}
			}
		}
		else
		{
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Enum\\Root\\*PNP0C01\\0000",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
			{
				strcpy(strBIOSInfo,"");
				if (RegQueryValueEx(hKey,"BIOSName",NULL,&ValType,(LPBYTE)&VerBuf,&Size)==ERROR_SUCCESS) strcat(strcat(strBIOSInfo,(char*)VerBuf)," ");
				Size=MAX_PATH;
				if (RegQueryValueEx(hKey,"BIOSDate",NULL,&ValType,(LPBYTE)&VerBuf,&Size)==ERROR_SUCCESS) strcat(strcat(strBIOSInfo,(char*)VerBuf)," ");
				Size=MAX_PATH;
				if (RegQueryValueEx(hKey,"BIOSVersion",NULL,&ValType,(LPBYTE)&VerBuf,&Size)==ERROR_SUCCESS) strcat(strBIOSInfo,(char*)VerBuf);
				if (strlen(strBIOSInfo)==0) strcpy(strBIOSInfo,"Unknown");
			}
		}
		fprintf(LFile,"BIOS:\n\t\t%s\n\n",strBIOSInfo);
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

bool ProcessNumberInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	SYSTEM_INFO stCpuInfo;
	bool			Ret=false;
	__try
	{
		SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
		SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,1);
		SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
		if (CountStruct) CountStruct->LocalStepCount=1;
		GetSystemInfo(&stCpuInfo);
		fprintf(LFile,"Processors number:\n\t\t%d\n\n",stCpuInfo.dwNumberOfProcessors);
		fflush(LFile);
		Ret=CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv);
		return Ret;
	}
	__finally
	{
	}
	return Ret;
}

bool GetFreeSystemDiskSizeInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	pfnGetDiskFreeSpaceExType	pfnGetDiskFreeSpaceEx=NULL;
	DWORD						Size=0;
	DWORD						SizeF=0;
	TCHAR						chWinDir[MAX_PATH];
	TCHAR						Drive[_MAX_DRIVE];
	UINT						DT;
	char						chFileSystemName[MAX_PATH];
	DWORD						dFileSystemFlags=0;
	int							i;
	UINT						PrevErrMode;
	bool						Ret=false;
	__try
	{
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,FREEDISK_STEP_COUNT);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
		if (CountStruct) CountStruct->LocalStepCount=FREEDISK_STEP_COUNT;
		fprintf(LFile,"Logical disks structure:\n");
		fflush(LFile);
		GetWindowsDirectory(chWinDir,MAX_PATH);
		_splitpath(chWinDir,Drive,NULL,NULL,NULL);
		pfnGetDiskFreeSpaceEx=(pfnGetDiskFreeSpaceExType)GetProcAddress(hKernel,"GetDiskFreeSpaceExA");
		PrevErrMode=SetErrorMode(SEM_FAILCRITICALERRORS);
		for (i='a';i<='z';i++)
		{
			chWinDir[0]=i;
			chWinDir[1]=':';
			chWinDir[2]='\\';
			chWinDir[3]=0;
			if (pfnGetDiskFreeSpaceEx!=NULL)
			{
				ULARGE_INTEGER	FreeBytesAvailable;
				ULARGE_INTEGER	TotalNumberOfBytes;
				if (pfnGetDiskFreeSpaceEx(chWinDir,&FreeBytesAvailable,&TotalNumberOfBytes,NULL)!=0)
				{
					FreeBytesAvailable.LowPart=FreeBytesAvailable.LowPart>>20;
					Size=FreeBytesAvailable.LowPart+((FreeBytesAvailable.HighPart&0xFFFFF)<<12);
					TotalNumberOfBytes.LowPart=TotalNumberOfBytes.LowPart>>20;
					SizeF=TotalNumberOfBytes.LowPart+((TotalNumberOfBytes.HighPart&0xFFFFF)<<12);
				}
			}
			else
			{
				DWORD dwSecPerClus, dwBytesPerSec, dwFreeClus, dwTotalClus;
				int nBytes = 0;
				if (GetDiskFreeSpace(chWinDir,&dwSecPerClus,&dwBytesPerSec,&dwFreeClus,&dwTotalClus))
				{
					nBytes=dwFreeClus*dwSecPerClus*dwBytesPerSec;
					nBytes=nBytes>>20;
					Size=nBytes;
					nBytes=dwTotalClus*dwSecPerClus*dwBytesPerSec;
					nBytes=nBytes>>20;
					SizeF=nBytes;
				}		
			}
			if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv))
			{
				Ret=true;
				break;
			}
			memset(chFileSystemName,0,MAX_PATH);
			GetVolumeInformation(chWinDir,NULL,0,NULL,NULL,&dFileSystemFlags,chFileSystemName,MAX_PATH);
			if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv))
			{
				Ret=true;
				break;
			}
			DT=GetDriveType(chWinDir);
			switch (DT)
			{
			case DRIVE_REMOTE:
				strcat(chWinDir," REMOTE");
				break;
			case DRIVE_CDROM:
				strcat(chWinDir," CDROM");
				break;
			case DRIVE_RAMDISK:
				strcat(chWinDir," RAMDISK");
				break;
			case DRIVE_REMOVABLE:
				strcat(chWinDir," REMOVABLE");
				break;
			case DRIVE_FIXED:
				if (_strnicmp(chWinDir,Drive,1)==0) strcat(chWinDir," FIXED SYSTEM");
				else strcat(chWinDir," FIXED");
				break;
			default:
				if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv))
				{
					i='z'+1;
					Ret=true;
				}
				continue;
			}
			fprintf(LFile,"\t\t%s ",chWinDir);
			fflush(LFile);
			fprintf(LFile,"Full size - %d Mb, Free size - %d Mb, File system - %s\n",SizeF,Size,chFileSystemName);
			fflush(LFile);
			if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv))
			{
				Ret=true;
				break;
			}
		}
		SetErrorMode(PrevErrMode);
		fprintf(LFile,"\n",chWinDir);
		fflush(LFile);
		return Ret;
	}
	__finally
	{
	}
	return Ret;
}

bool IfNTFS(const char chDisk[])
{
	char	chFileSystemName[MAX_PATH];
	DWORD	dFileSystemFlags=0;
	memset(chFileSystemName,0,MAX_PATH);
	GetVolumeInformation(chDisk,NULL,0,NULL,NULL,&dFileSystemFlags,chFileSystemName,MAX_PATH);
	if (_strnicmp(chFileSystemName,"NTFS",4)==0) return true;
	else return false;
}

bool GetPrinterInformation(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	DWORD	dwNeeded=0;
	DWORD	dwReturned=0;
	DWORD	i;
	TCHAR	cDefPrinter[MAX_PATH]="";
	char*	chDefPrinter=cDefPrinter;
	TCHAR	cDriver[MAX_PATH]="";
	char*	chDriver=cDriver;
	LPBYTE	ppi=NULL;
	bool	Ret=false;
	__try
	{
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
		if (os>OS_UNKNOWN) i=4;
		else i=2;
		EnumPrinters(PRINTER_ENUM_LOCAL|PRINTER_ENUM_CONNECTIONS,NULL,i,NULL,0,&dwNeeded,&dwReturned);
		ppi=(LPBYTE)malloc(dwNeeded);
		EnumPrinters(PRINTER_ENUM_LOCAL|PRINTER_ENUM_CONNECTIONS,NULL,i,(LPBYTE)ppi,dwNeeded,&dwNeeded,&dwReturned);
		dwNeeded=0;
		if (ppi!=NULL) free(ppi);
		ppi=NULL;
		dwReturned++;
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,dwReturned);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
		if (CountStruct&&dwReturned) CountStruct->LocalStepCount = dwReturned;
		dwReturned=0;
		fprintf(LFile,"Printers:\n");
		fflush(LFile);
		GetDefaulpPrinterName((char**)&chDefPrinter);
		GetPrinterDriverInfo(cDefPrinter,(char**)&chDriver);
		AddPrinter(cDefPrinter,cDriver,TRUE);
		if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
		{
			Ret=true;
			return Ret;
		}
		if (os>OS_UNKNOWN)
		{
			EnumPrinters(PRINTER_ENUM_LOCAL|PRINTER_ENUM_CONNECTIONS/*|PRINTER_ENUM_NETWORK*/,NULL,4,NULL,0,&dwNeeded,&dwReturned);
			if (dwNeeded!=0)
			{
				ppi=(LPBYTE)malloc(dwNeeded);
				if (ppi!=NULL)
				{
					if (EnumPrinters(PRINTER_ENUM_LOCAL|PRINTER_ENUM_CONNECTIONS,NULL,4,ppi,dwNeeded,&dwNeeded,&dwReturned)) 
					{
						for (i=0;i<dwReturned;i++)
						{
							if (_stricmp(((pPPTI4)ppi)->Ppti4[i].pPrinterName,cDefPrinter)!=0)
							{
								cDriver[0]=0;
								GetPrinterDriverInfo(((pPPTI4)ppi)->Ppti4[i].pPrinterName,(char**)&chDriver);
								AddPrinter(((pPPTI4)ppi)->Ppti4[i].pPrinterName,cDriver,FALSE);
							}
							if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
							{
								Ret=true;
								break;
							}
						}
					}
				}
			}
		}
		else
		{
			EnumPrinters(PRINTER_ENUM_LOCAL,NULL,2,NULL,0,&dwNeeded,&dwReturned);
			if (dwNeeded!=0)
			{
				ppi=(LPBYTE)malloc(dwNeeded);
				if (ppi!=NULL)
				{
					if (EnumPrinters(PRINTER_ENUM_LOCAL|PRINTER_ENUM_CONNECTIONS,NULL,2,(LPBYTE)ppi,dwNeeded,&dwNeeded,&dwReturned)) 
					{
						for (i=0;i<dwReturned;i++)
						{
							if (_stricmp(((pPPTI2)ppi)->Ppti2[i].pPrinterName,cDefPrinter)!=0)
							{
								cDriver[0]=0;
								GetPrinterDriverInfo(((pPPTI2)ppi)->Ppti2[i].pPrinterName,(char**)&chDriver);
								AddPrinter(((pPPTI2)ppi)->Ppti2[i].pPrinterName,cDriver,FALSE);
							}
							if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
							{
								Ret=true;
								break;
							}
						}
					}
				}
			}
		}
		fprintf(LFile,"\n");
		fflush(LFile);
		return Ret;
	}
	__finally
	{
		if (ppi!=NULL) free(ppi);
	}
	return Ret;
}

bool GetMultimediaInformation(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	UINT		cbCaps;
	WAVEOUTCAPS wavCaps;
	HKEY		hKey=NULL;
	HKEY		hSubKey=NULL;
	HKEY		hSubKeyTwo=NULL;
	HKEY		hSubKeyThree=NULL;
	DWORD		cSubKeysNumber=0;
	TCHAR		cSubKeyName[MAX_PATH];
	TCHAR		chDeviceName[MAX_PATH];
	TCHAR		chDriver[MAX_PATH];
	char*		pchDeviceName=chDeviceName;
	DWORD		i,retCode;
	DWORD		dwType=REG_SZ;
	DWORD		dwCount=MAX_PATH;
	DWORD		StepCount=0;
	bool		Ret=false;
	
	__try
	{
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
		if (os<OS_UNKNOWN)
		{
			if (RegOpenKeyEx(HKEY_DYN_DATA,"Config Manager\\Enum",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS) RegQueryInfoKey(hKey,NULL,NULL,NULL,&StepCount,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
		}
		else
		{
			if (os<OS_WIN2000) StepCount=waveOutGetNumDevs();
			else
			{
				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E96C-E325-11CE-BFC1-08002BE10318}",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS) RegQueryInfoKey(hKey,NULL,NULL,NULL,&StepCount,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
			}
		}
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,StepCount);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
		if (CountStruct&&StepCount) CountStruct->LocalStepCount = StepCount;
		fprintf(LFile,"Multimedia:\n");
		fflush(LFile);
//Windows9x
//HKEY_DYN_DATA\ConfigManager\Enum\xxxx
//	HardWareKey
//		HKEY_LOCAL_MACHINE\Enum\<HardWareKey>
//			if (ClassGUID=="{4d36e96c-e325-11ce-bfc1-08002be10318}")
//				DeviceDesc
//				Driver
//					HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\Class\<Driver>
//						Driver (может содержать несколько драйверов)
//						??DriverDate
		if (os<OS_UNKNOWN)
		{
			if (RegOpenKeyEx(HKEY_DYN_DATA,"Config Manager\\Enum",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
			{
				RegQueryInfoKey(hKey,NULL,NULL,NULL,&cSubKeysNumber,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
				for (i=0;i<cSubKeysNumber;i++)
				{
					cSubKeyName[0]=0;
					retCode=RegEnumKey(hKey,i,cSubKeyName,MAX_PATH); 
					if ((retCode==(DWORD)ERROR_SUCCESS)&&(retCode!=ERROR_INSUFFICIENT_BUFFER)) 
					{
						if (RegOpenKeyEx(hKey,cSubKeyName,0,KEY_READ,&hSubKey)==ERROR_SUCCESS)
						{
							dwCount=MAX_PATH;
							chDeviceName[0]=0;
							if (RegQueryValueEx(hSubKey,"HardWareKey",NULL,&dwType,(LPBYTE)chDriver,&dwCount)==ERROR_SUCCESS)
							{
								strcat(strcpy(cSubKeyName,"Enum\\"),chDriver);
								chDriver[0]=0;
								if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,cSubKeyName,0,KEY_READ,&hSubKeyTwo)==ERROR_SUCCESS)
								{
									
									chDriver[0]=0;
									dwCount=MAX_PATH;
									if (RegQueryValueEx(hSubKeyTwo,"ClassGUID",NULL,&dwType,(LPBYTE)chDriver,&dwCount)==ERROR_SUCCESS)
									{
										if (_stricmp(chDriver,"{4d36e96c-e325-11ce-bfc1-08002be10318}")==0)
										{
											dwCount=MAX_PATH;
											chDeviceName[0]=0;
											if (RegQueryValueEx(hSubKeyTwo,"DriverDesc",NULL,&dwType,(LPBYTE)chDeviceName,&dwCount)!=ERROR_SUCCESS) cSubKeyName[0]=0;
											chDriver[0]=0;
											dwCount=MAX_PATH;
											if (RegQueryValueEx(hSubKeyTwo,"Driver",NULL,&dwType,(LPBYTE)chDriver,&dwCount)==ERROR_SUCCESS)
											{
												strcat(strcpy(cSubKeyName,"System\\CurrentControlSet\\Services\\Class"),chDriver);
												chDriver[0]=0;
												if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,cSubKeyName,0,KEY_READ,&hSubKeyThree)==ERROR_SUCCESS)
												{
													chDriver[0]=0;
													dwCount=MAX_PATH;
													if (RegQueryValueEx(hSubKeyThree,"Driver",NULL,&dwType,(LPBYTE)chDriver,&dwCount)!=ERROR_SUCCESS) chDriver[0]=0;
													AddDriver(chDeviceName,chDriver);
													RegCloseKey(hSubKeyThree);
													hSubKeyThree=NULL;
												}
											}
										}
									}
									RegCloseKey(hSubKeyTwo);
									hSubKeyTwo=NULL;
								}
							}
							RegCloseKey(hSubKey);
							hSubKey=NULL;
						}
					}
					if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
					{
						Ret=true;
						break;
					}
				}
				RegCloseKey(hKey);
				hKey=NULL;
			}
		}
		else
		{
			if (os<OS_WIN2000)
			{
				retCode=0;
				retCode=waveOutGetNumDevs();
				for (i=0;i<retCode;i++)
				{
					cbCaps=sizeof(AUXCAPS);
					if (waveOutGetDevCaps(i,&wavCaps,cbCaps)==MMSYSERR_NOERROR) 
					{
						GetAudioDevCompanyName(wavCaps.wMid,(char**)&pchDeviceName);
						strcat(chDeviceName," ");
						_ltoa((wavCaps.vDriverVersion>>8),chDeviceName+strlen(chDeviceName)+1,10);
						strcat(chDeviceName,".");
						_ltoa((wavCaps.vDriverVersion&0x00FF),chDeviceName+strlen(chDeviceName)+1,10);
						AddDriver(wavCaps.szPname,chDeviceName);
					}
					if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
					{
						Ret=true;
						break;
					}
				}
			}
			else
			{
//Windows 2000/XP
//HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Class\{4D36E96C-E325-11CE-BFC1-08002BE10318}\xxxx
//	if SetupPreferredAudioDevicesCount
//		DriverDesc
//		MatchingDeviceId
//			HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Enum\<MatchingDeviceId>
//				Service
//					HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\<Service>
//						ImagePath
				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E96C-E325-11CE-BFC1-08002BE10318}",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
				{
					RegQueryInfoKey(hKey,NULL,NULL,NULL,&cSubKeysNumber,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
					for (i=0;i<cSubKeysNumber;i++)
					{
						cSubKeyName[0]=0;
						retCode=RegEnumKey(hKey,i,cSubKeyName,MAX_PATH); 
						if ((retCode==(DWORD)ERROR_SUCCESS)&&(retCode!=ERROR_INSUFFICIENT_BUFFER)) 
						{
							if (RegOpenKeyEx(hKey,cSubKeyName,0,KEY_READ,&hSubKey)==ERROR_SUCCESS)
							{
								chDriver[0]=0;
								dwCount=sizeof(DWORD);
								dwType=REG_DWORD;
								if (RegQueryValueEx(hSubKey,"SetupPreferredAudioDevicesCount",NULL,&dwType,(LPBYTE)(&retCode),&dwCount)==ERROR_SUCCESS)
								{
									dwCount=MAX_PATH;
									chDeviceName[0]=0;
									if (RegQueryValueEx(hSubKey,"DeviceDesc",NULL,&dwType,(LPBYTE)chDeviceName,&dwCount)!=ERROR_SUCCESS) chDeviceName[0]=0;
									chDriver[0]=0;
									dwCount=MAX_PATH;
									if (RegQueryValueEx(hSubKey,"MatchingDeviceId",NULL,&dwType,(LPBYTE)chDriver,&dwCount)==ERROR_SUCCESS)
									{
										strcat(strcpy(cSubKeyName,"SYSTEM\\CurrentControlSet\\Enum\\"),chDriver);
										chDriver[0]=0;
										if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,cSubKeyName,0,KEY_READ,&hSubKeyTwo)==ERROR_SUCCESS)
										{
											chDriver[0]=0;
											dwCount=MAX_PATH;
											if (RegQueryValueEx(hSubKeyTwo,"Service",NULL,&dwType,(LPBYTE)chDriver,&dwCount)==ERROR_SUCCESS)
											{
												strcat(strcpy(cSubKeyName,"SYSTEM\\CurrentControlSet\\Services\\"),chDriver);
												chDriver[0]=0;
												if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,cSubKeyName,0,KEY_READ,&hSubKeyThree)==ERROR_SUCCESS)
												{
													chDriver[0]=0;
													dwCount=MAX_PATH;
													if (RegQueryValueEx(hSubKeyThree,"ImagePath",NULL,&dwType,(LPBYTE)chDriver,&dwCount)!=ERROR_SUCCESS) chDriver[0]=0;
													RegCloseKey(hSubKeyThree);
													hSubKeyThree=NULL;
												}
											}
											else chDriver[0]=0;
											RegCloseKey(hSubKeyTwo);
											hSubKeyTwo=NULL;
										}
									}
									else chDriver[0]=0;
									AddDriver(chDeviceName,chDriver);
								}
								RegCloseKey(hSubKey);
								hSubKey=NULL;
							}
						}
						if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
						{
							Ret=true;
							break;
						}
					}
					RegCloseKey(hKey);
					hKey=NULL;
				}
			}
		}
		fprintf(LFile,"\n");
		fflush(LFile);
		return Ret;
	}
	__finally
	{
		if (hSubKeyThree!=NULL) RegCloseKey(hSubKeyThree);
		if (hSubKeyTwo!=NULL) RegCloseKey(hSubKeyTwo);
		if (hSubKey!=NULL) RegCloseKey(hSubKey);
		if (hKey!=NULL) RegCloseKey(hKey);
	}
	return Ret;
}

bool GetHardDiskInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	bool Ret=false;
	__try
	{
		fprintf(LFile,"Hard drives:\n");
		fflush(LFile);
		if (os>OS_UNKNOWN)
		{
			if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
			if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,HARD_NT_STEP_COUNT);
			if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
			if (CountStruct) CountStruct->LocalStepCount = HARD_NT_STEP_COUNT;
			if (ReadPhysicalDriveInNT(hProgressCtrl,CountStruct,hEv,&Ret)) ReadIdeDriveAsScsiDriveInNT(hProgressCtrl,CountStruct,hEv,&Ret);
			else
			{
				if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,HARD_NT_STEP2_STEP_COUNT,0);
				CountStruct->LocalStep=CountStruct->LocalStepCount;
				if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) Ret=true;
			}
		}
		else 
		{
			if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
			if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,HARD_9X_STEP_COUNT);
			if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
			if (CountStruct) CountStruct->LocalStepCount=HARD_9X_STEP_COUNT;
			ReadDrivePortsInWin9X(hProgressCtrl,CountStruct,hEv,&Ret);
			if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) Ret=true;
		}
		fprintf(LFile,"\n");
		fflush(LFile);
		return Ret;
	}
	__finally
	{
	}
	return Ret;
}

bool GetModemInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	HKEY			hKey=NULL;
	HKEY			hSubKey=NULL;
	TCHAR			cSubKeyName[MAX_PATH];
	TCHAR			chDriver[MAX_PATH];
	DWORD			cSubKeysNumber=0;
	DWORD			retCode=ERROR_SUCCESS;
	DWORD			i;
	DWORD			dwType=0;
	DWORD			dwCount=MAX_PATH;
	TCHAR			chDriverFileName[MAX_PATH];
	char*			chFullName=chDriverFileName;
	bool			Ret=false;
	
	__try
	{
		fprintf(LFile,"Modems:\n");
		fflush(LFile);
		if (os<OS_UNKNOWN) strcpy(cSubKeyName,"SYSTEM\\CurrentControlSet\\Services\\Class\\Modem");
		else strcpy(cSubKeyName,"SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E96D-E325-11CE-BFC1-08002BE10318}");
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,cSubKeyName,0,KEY_READ,&hKey)==ERROR_SUCCESS)
		{
			RegQueryInfoKey(hKey,NULL,NULL,NULL,&cSubKeysNumber,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
			if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
			if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,cSubKeysNumber);
			if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
			for (i=0;i<cSubKeysNumber;i++)
			{
				if (CountStruct) CountStruct->LocalStepCount = cSubKeysNumber;
				cSubKeyName[0]=0;
				retCode=RegEnumKey(hKey,i,cSubKeyName,MAX_PATH); 
				if ((retCode==(DWORD)ERROR_SUCCESS)&&(retCode!=ERROR_INSUFFICIENT_BUFFER)) 
				{
					if (RegOpenKeyEx(hKey,cSubKeyName,0,KEY_READ,&hSubKey)==ERROR_SUCCESS)
					{
						dwCount=MAX_PATH;
						cSubKeyName[0]=0;
						if (RegQueryValueEx(hSubKey,"Model",NULL,&dwType,(LPBYTE)cSubKeyName,&dwCount)!=ERROR_SUCCESS) cSubKeyName[0]=0;
						chDriver[0]=0;
						dwCount=MAX_PATH;
						if (RegQueryValueEx(hSubKey,"DriverVersion",NULL,&dwType,(LPBYTE)chDriver,&dwCount)!=ERROR_SUCCESS) chDriver[0]=0;
						if (strlen(chDriver)==0)
						{
							if (RegQueryValueEx(hSubKey,"FriendlyDriver",NULL,&dwType,(LPBYTE)chDriver,&dwCount)!=ERROR_SUCCESS) chDriver[0]=0;
						}
						if (strlen(cSubKeyName)!=0)
						{
							if (strlen(chDriver)!=NULL)
							{
								GetExecutableName(chDriver,&chFullName,NULL,NULL);
								fprintf(LFile,"\t\t%s\t\t\tDRIVER - %s, ",cSubKeyName,chFullName);
								fflush(LFile);
								GetFileInfoAsRepInfo(chFullName);
								fflush(LFile);
							}
							else
							{
								fprintf(LFile,"\t\t%s\n",cSubKeyName);
								fflush(LFile);
							}
						}
						RegCloseKey(hSubKey);
						hSubKey=NULL;
					}
				}
				if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
				{
					Ret=true;
					break;
				}
			}
			RegCloseKey(hKey);
			hKey=NULL;
		}
		fprintf(LFile,"\n");
		fflush(LFile);
		return Ret;
	}
	__finally
	{
		if (hSubKey==NULL) RegCloseKey(hSubKey);
		if (hKey==NULL) RegCloseKey(hKey);
	}
	return Ret;
}

bool GetNetworkAdaptersInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	HKEY	hKey=NULL;
	HKEY	hSubKey=NULL;
	HKEY	hSubKeyTwo=NULL;
	HKEY	hSubKeyThree=NULL;
	HKEY	hSubKeyFour=NULL;
	DWORD	cSubKeysNumber=0;
	TCHAR	cSubKeyName[MAX_PATH];
	TCHAR	chDeviceName[MAX_PATH];
	TCHAR	chDriver[MAX_PATH];
	DWORD	i,retCode;
	DWORD	dwType=REG_SZ;
	DWORD	dwCount=MAX_PATH;
	DWORD	StepCount=0;
	bool	Ret=false;

	__try
	{
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
		if (os<OS_UNKNOWN)
		{
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"System\\CurrentControlSet\\Services\\Class\\Net",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS) RegQueryInfoKey(hKey,NULL,NULL,NULL,&StepCount,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
		}
		else
		{
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS) RegQueryInfoKey(hKey,NULL,NULL,NULL,&StepCount,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
		}
		if (hKey!=NULL) RegCloseKey(hKey);
		hKey=NULL;
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,StepCount);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
		if (CountStruct&&StepCount) CountStruct->LocalStepCount = StepCount;
		fprintf(LFile,"NetWork adapters:\n");
		fflush(LFile);
		if (os<OS_UNKNOWN)
		{
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"System\\CurrentControlSet\\Services\\Class\\Net",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
			{
				RegQueryInfoKey(hKey,NULL,NULL,NULL,&cSubKeysNumber,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
				for (i=0;i<cSubKeysNumber;i++)
				{
					cSubKeyName[0]=0;
					retCode=RegEnumKey(hKey,i,cSubKeyName,MAX_PATH); 
					if ((retCode==(DWORD)ERROR_SUCCESS)&&(retCode!=ERROR_INSUFFICIENT_BUFFER)) 
					{
						if (RegOpenKeyEx(hKey,cSubKeyName,0,KEY_READ,&hSubKey)==ERROR_SUCCESS)
						{
							dwCount=MAX_PATH;
							chDeviceName[0]=0;
							if (RegQueryValueEx(hSubKey,"DriverDesc",NULL,&dwType,(LPBYTE)chDeviceName,&dwCount)!=ERROR_SUCCESS) cSubKeyName[0]=0;
							chDriver[0]=0;
							dwCount=MAX_PATH;
							if (RegQueryValueEx(hSubKey,"DeviceVxDs",NULL,&dwType,(LPBYTE)chDriver,&dwCount)!=ERROR_SUCCESS) chDriver[0]=0;
							AddDriver(chDeviceName,chDriver);
							RegCloseKey(hSubKey);
							hSubKey=NULL;
						}
					}
					if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
					{
						Ret=true;
						break;
					}
				}
				RegCloseKey(hKey);
				hKey=NULL;
			}
		}
		else
		{
			if (os<OS_WIN2000)
			{
				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
				{
					RegQueryInfoKey(hKey,NULL,NULL,NULL,&cSubKeysNumber,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
					for (i=0;i<cSubKeysNumber;i++)
					{
						cSubKeyName[0]=0;
						retCode=RegEnumKey(hKey,i,cSubKeyName,MAX_PATH); 
						if ((retCode==(DWORD)ERROR_SUCCESS)&&(retCode!=ERROR_INSUFFICIENT_BUFFER)) 
						{
							if (RegOpenKeyEx(hKey,cSubKeyName,0,KEY_READ,&hSubKey)==ERROR_SUCCESS)
							{
								dwCount=MAX_PATH;
								chDeviceName[0]=0;
								if (RegQueryValueEx(hSubKey,"Description",NULL,&dwType,(LPBYTE)chDeviceName,&dwCount)!=ERROR_SUCCESS) cSubKeyName[0]=0;
								chDriver[0]=0;
								dwCount=MAX_PATH;
								if (RegQueryValueEx(hSubKey,"ProductName",NULL,&dwType,(LPBYTE)chDriver,&dwCount)==ERROR_SUCCESS)
								{
									strcat(strcpy(cSubKeyName,"SYSTEM\\CurrentControlSet\\Services\\"),chDriver);
									chDriver[0]=0;
									if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,cSubKeyName,0,KEY_READ,&hSubKeyTwo)==ERROR_SUCCESS)
									{
										chDriver[0]=0;
										dwCount=MAX_PATH;
										if (RegQueryValueEx(hSubKeyTwo,"ImagePath",NULL,&dwType,(LPBYTE)chDriver,&dwCount)!=ERROR_SUCCESS) chDriver[0]=0;
										RegCloseKey(hSubKeyTwo);
										hSubKeyTwo=NULL;
									}
								}
								else chDriver[0]=0;
								AddDriver(chDeviceName,chDriver);
								RegCloseKey(hSubKey);
								hSubKey=NULL;
							}
						}
						if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
						{
							Ret=true;
							break;
						}
					}
					RegCloseKey(hKey);
					hKey=NULL;
				}
			}
			else
			{
				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NetworkCards",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
				{
					RegQueryInfoKey(hKey,NULL,NULL,NULL,&cSubKeysNumber,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
					for (i=0;i<cSubKeysNumber;i++)
					{
						cSubKeyName[0]=0;
						retCode=RegEnumKey(hKey,i,cSubKeyName,MAX_PATH); 
						if ((retCode==(DWORD)ERROR_SUCCESS)&&(retCode!=ERROR_INSUFFICIENT_BUFFER)) 
						{
							if (RegOpenKeyEx(hKey,cSubKeyName,0,KEY_READ,&hSubKey)==ERROR_SUCCESS)
							{
								dwCount=MAX_PATH;
								chDeviceName[0]=0;
								if (RegQueryValueEx(hSubKey,"Description",NULL,&dwType,(LPBYTE)chDeviceName,&dwCount)!=ERROR_SUCCESS) cSubKeyName[0]=0;
								chDriver[0]=0;
								dwCount=MAX_PATH;
								if (RegQueryValueEx(hSubKey,"ServiceName",NULL,&dwType,(LPBYTE)chDriver,&dwCount)==ERROR_SUCCESS)
								{
									strcat(strcat(strcpy(cSubKeyName,"SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\"),chDriver),"\\Connection");
									chDriver[0]=0;
									if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,cSubKeyName,0,KEY_READ,&hSubKeyTwo)==ERROR_SUCCESS)
									{
										chDriver[0]=0;
										dwCount=MAX_PATH;
										if (RegQueryValueEx(hSubKeyTwo,"PnpInstanceID",NULL,&dwType,(LPBYTE)chDriver,&dwCount)==ERROR_SUCCESS)
										{
											strcat(strcpy(cSubKeyName,"SYSTEM\\CurrentControlSet\\Enum\\"),chDriver);
											chDriver[0]=0;
											if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,cSubKeyName,0,KEY_READ,&hSubKeyThree)==ERROR_SUCCESS)
											{
												chDriver[0]=0;
												dwCount=MAX_PATH;
												if (RegQueryValueEx(hSubKeyThree,"Service",NULL,&dwType,(LPBYTE)chDriver,&dwCount)==ERROR_SUCCESS)
												{
													strcat(strcpy(cSubKeyName,"SYSTEM\\CurrentControlSet\\Services\\"),chDriver);
													chDriver[0]=0;
													if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,cSubKeyName,0,KEY_READ,&hSubKeyFour)==ERROR_SUCCESS)
													{
														chDriver[0]=0;
														dwCount=MAX_PATH;
														if (RegQueryValueEx(hSubKeyFour,"ImagePath",NULL,&dwType,(LPBYTE)chDriver,&dwCount)!=ERROR_SUCCESS) chDriver[0]=0;
														RegCloseKey(hSubKeyFour);
														hSubKeyFour=NULL;
													}
												}
												else chDriver[0]=0;
												RegCloseKey(hSubKeyThree);
												hSubKeyThree=NULL;
											}
										}
										else chDriver[0]=0;
										RegCloseKey(hSubKeyTwo);
										hSubKeyTwo=NULL;
									}
								}
								else chDriver[0]=0;
								AddDriver(chDeviceName,chDriver);
								RegCloseKey(hSubKey);
								hSubKey=NULL;
							}
						}
						if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv)) 
						{
							Ret=true;
							break;
						}
					}
					RegCloseKey(hKey);
					hKey=NULL;
				}
			}
		}
		fprintf(LFile,"\n");
		fflush(LFile);
		return Ret;
	}
	__finally
	{
		if (hSubKeyFour!=NULL) RegCloseKey(hSubKeyFour);
		if (hSubKeyThree!=NULL) RegCloseKey(hSubKeyThree);
		if (hSubKeyTwo!=NULL) RegCloseKey(hSubKeyTwo);
		if (hSubKey!=NULL) RegCloseKey(hSubKey);
		if (hKey!=NULL) RegCloseKey(hKey);
	}
	return Ret;
}

void AddDriver(const char chDeviceName[],const char chDriver[])
{
	char			chFullName[MAX_PATH];
	char*			pchFullName=chFullName;
	
	__try
	{
		if (strlen(chDeviceName)==0) return;
		if (!GetExecutableName(chDriver,&pchFullName,NULL,NULL)) strcpy(chFullName,chDriver);
		fprintf(LFile,"\t\t%s\n",chDeviceName);
		fflush(LFile);
		if (strlen(chFullName)!=0)
		{
			fprintf(LFile,"\t\t\tDRIVER - %s, ",chFullName);
			fflush(LFile);
			GetFileInfoAsRepInfo(chFullName);
			fflush(LFile);
		}
		return;
	}
	__finally
	{
	}
}

bool GetVideoInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	HKEY	hKey=NULL;
	HKEY	hSubKey=NULL;
	HKEY	hSubKeyTwo=NULL;
	HKEY	hSubKeyThree=NULL;
	HKEY	hSubKeyFour=NULL;
	DWORD	cSubKeysNumber=0;
	DWORD	cSubKeysNumberTwo=0;
	TCHAR	cSubKeyName[MAX_PATH];
	TCHAR	chDeviceName[MAX_PATH];
	TCHAR	chDriver[MAX_PATH];
	DWORD	i,j,retCode;
	DWORD	dwType=REG_SZ;
	DWORD	dwCount=MAX_PATH;
	DWORD	StepCount=0;
	bool	Ret=false;

	__try
	{
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
		if (os<OS_UNKNOWN)
		{
			if (RegOpenKeyEx(HKEY_DYN_DATA,"Config Manager\\Enum",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS) RegQueryInfoKey(hKey,NULL,NULL,NULL,&StepCount,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
		}
		else
		{
			if (os<OS_WIN2000)
			{
				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Enum\\Root",0,KEY_READ,&hKey)==ERROR_SUCCESS) RegQueryInfoKey(hKey,NULL,NULL,NULL,&StepCount,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
			}
			else
			{
				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E968-E325-11CE-BFC1-08002BE10318}",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS) RegQueryInfoKey(hKey,NULL,NULL,NULL,&StepCount,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
			}
		}
		if (hKey!=NULL) RegCloseKey(hKey);
		hKey=NULL;
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,StepCount);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
		if (CountStruct&&StepCount) CountStruct->LocalStepCount=StepCount;
		fprintf(LFile,"Video adapters:\n");
		fflush(LFile);
		if (os<OS_UNKNOWN)
		{
			if (RegOpenKeyEx(HKEY_DYN_DATA,"Config Manager\\Enum",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
			{
				RegQueryInfoKey(hKey,NULL,NULL,NULL,&cSubKeysNumber,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
				for (i=0;i<cSubKeysNumber;i++)
				{
					cSubKeyName[0]=0;
					retCode=RegEnumKey(hKey,i,cSubKeyName,MAX_PATH); 
					if ((retCode==(DWORD)ERROR_SUCCESS)&&(retCode!=ERROR_INSUFFICIENT_BUFFER)) 
					{
						if (RegOpenKeyEx(hKey,cSubKeyName,0,KEY_READ,&hSubKey)==ERROR_SUCCESS)
						{
							dwCount=MAX_PATH;
							chDeviceName[0]=0;
							if (RegQueryValueEx(hSubKey,"HardWareKey",NULL,&dwType,(LPBYTE)chDriver,&dwCount)==ERROR_SUCCESS)
							{
								strcat(strcpy(cSubKeyName,"Enum\\"),chDriver);
								chDriver[0]=0;
								if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,cSubKeyName,0,KEY_READ,&hSubKeyTwo)==ERROR_SUCCESS)
								{
									
									chDriver[0]=0;
									dwCount=MAX_PATH;
									if (RegQueryValueEx(hSubKeyTwo,"ClassGUID",NULL,&dwType,(LPBYTE)chDriver,&dwCount)==ERROR_SUCCESS)
									{
										if (_stricmp(chDriver,"{4d36e968-e325-11ce-bfc1-08002be10318}")==0)
										{
											chDriver[0]=0;
											dwCount=MAX_PATH;
											if (RegQueryValueEx(hSubKeyTwo,"Driver",NULL,&dwType,(LPBYTE)chDriver,&dwCount)==ERROR_SUCCESS)
											{
												strcat(strcpy(cSubKeyName,"System\\CurrentControlSet\\Services\\Class"),chDriver);
												chDriver[0]=0;
												if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,cSubKeyName,0,KEY_READ,&hSubKeyThree)==ERROR_SUCCESS)
												{
													dwCount=MAX_PATH;
													chDeviceName[0]=0;
													if (RegQueryValueEx(hSubKeyThree,"DriverDesc",NULL,&dwType,(LPBYTE)chDeviceName,&dwCount)!=ERROR_SUCCESS) cSubKeyName[0]=0;
													if (RegOpenKeyEx(hSubKeyThree,"DEFAULT",0,KEY_READ,&hSubKeyFour)==ERROR_SUCCESS)
													{
														chDriver[0]=0;
														dwCount=MAX_PATH;
														if (RegQueryValueEx(hSubKeyFour,"drv",NULL,&dwType,(LPBYTE)chDriver,&dwCount)!=ERROR_SUCCESS) chDriver[0]=0;
														RegCloseKey(hSubKeyFour);
														hSubKeyFour=NULL;
													}
													else chDriver[0]=0;
													AddDriver(chDeviceName,chDriver);
													RegCloseKey(hSubKeyThree);
													hSubKeyThree=NULL;
												}
											}
										}
									}
									RegCloseKey(hSubKeyTwo);
									hSubKeyTwo=NULL;
								}
							}
							RegCloseKey(hSubKey);
							hSubKey=NULL;
						}
					}
					if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv))
					{
						Ret=true;
						break;
					}
				}
				RegCloseKey(hKey);
				hKey=NULL;
			}
		}
		else
		{
			if (os<OS_WIN2000)
			{
				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Enum\\Root",0,KEY_READ,&hKey)==ERROR_SUCCESS)
				{
					RegQueryInfoKey(hKey,NULL,NULL,NULL,&cSubKeysNumber,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
					for (i=0;i<cSubKeysNumber;i++)
					{
						cSubKeyName[0]=0;
						retCode=RegEnumKey(hKey,i,cSubKeyName,MAX_PATH); 
						if ((retCode==(DWORD)ERROR_SUCCESS)&&(retCode!=ERROR_INSUFFICIENT_BUFFER)) 
						{
							if (RegOpenKeyEx(hKey,cSubKeyName,0,KEY_READ,&hSubKey)==ERROR_SUCCESS)
							{
								RegQueryInfoKey(hSubKey,NULL,NULL,NULL,&cSubKeysNumberTwo,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
								for (j=0;j<cSubKeysNumberTwo;j++)
								{
									cSubKeyName[0]=0;
									retCode=RegEnumKey(hSubKey,j,cSubKeyName,MAX_PATH); 
									if ((retCode==(DWORD)ERROR_SUCCESS)&&(retCode!=ERROR_INSUFFICIENT_BUFFER)) 
									{
										if (RegOpenKeyEx(hSubKey,cSubKeyName,0,KEY_READ,&hSubKeyTwo)==ERROR_SUCCESS)
										{
											dwCount=MAX_PATH;
											chDriver[0]=0;
											if (RegQueryValueEx(hSubKeyTwo,"ClassGUID",NULL,&dwType,(LPBYTE)chDriver,&dwCount)==ERROR_SUCCESS)
											{
												if (_stricmp(chDriver,"{4D36E968-E325-11CE-BFC1-08002BE10318}")==0)
												{
													dwCount=MAX_PATH;
													chDeviceName[0]=0;
													if (RegQueryValueEx(hSubKeyTwo,"DeviceDesc",NULL,&dwType,(LPBYTE)chDeviceName,&dwCount)!=ERROR_SUCCESS) cSubKeyName[0]=0;
													chDriver[0]=0;
													dwCount=MAX_PATH;
													if (RegQueryValueEx(hSubKeyTwo,"Service",NULL,&dwType,(LPBYTE)chDriver,&dwCount)==ERROR_SUCCESS)
													{
														strcat(strcpy(cSubKeyName,"SYSTEM\\CurrentControlSet\\Services\\"),chDriver);
														chDriver[0]=0;
														if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,cSubKeyName,0,KEY_READ,&hSubKeyThree)==ERROR_SUCCESS)
														{
															chDriver[0]=0;
															dwCount=MAX_PATH;
															if (RegQueryValueEx(hSubKeyThree,"ImagePath",NULL,&dwType,(LPBYTE)chDriver,&dwCount)!=ERROR_SUCCESS) chDriver[0]=0;
															RegCloseKey(hSubKeyThree);
															hSubKeyThree=NULL;
														}
													}
													else chDriver[0]=0;
													AddDriver(chDeviceName,chDriver);
												}
											}
											RegCloseKey(hSubKeyTwo);
											hSubKeyTwo=NULL;
										}
									}
								}
								RegCloseKey(hSubKey);
								hSubKey=NULL;
							}
						}
						if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv))
						{
							Ret=true;
							break;
						}
					}
					RegCloseKey(hKey);
					hKey=NULL;
				}
			}
			else
			{
				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E968-E325-11CE-BFC1-08002BE10318}",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
				{
					RegQueryInfoKey(hKey,NULL,NULL,NULL,&cSubKeysNumber,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
					for (i=0;i<cSubKeysNumber;i++)
					{
						cSubKeyName[0]=0;
						retCode=RegEnumKey(hKey,i,cSubKeyName,MAX_PATH); 
						if ((retCode==(DWORD)ERROR_SUCCESS)&&(retCode!=ERROR_INSUFFICIENT_BUFFER)) 
						{
							if (RegOpenKeyEx(hKey,cSubKeyName,0,KEY_READ,&hSubKey)==ERROR_SUCCESS)
							{
								chDriver[0]=0;
								dwCount=MAX_PATH;
								if (RegQueryValueEx(hSubKey,"MatchingDeviceId",NULL,&dwType,(LPBYTE)chDriver,&dwCount)==ERROR_SUCCESS)
								{
									strcat(strcpy(cSubKeyName,"SYSTEM\\CurrentControlSet\\Enum\\"),chDriver);
									chDriver[0]=0;
									if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,cSubKeyName,0,KEY_READ,&hSubKeyTwo)==ERROR_SUCCESS)
									{
										dwCount=MAX_PATH;
										chDeviceName[0]=0;
										if (RegQueryValueEx(hSubKeyTwo,"DeviceDesc",NULL,&dwType,(LPBYTE)chDeviceName,&dwCount)!=ERROR_SUCCESS) cSubKeyName[0]=0;
										chDriver[0]=0;
										dwCount=MAX_PATH;
										if (RegQueryValueEx(hSubKeyTwo,"Service",NULL,&dwType,(LPBYTE)chDriver,&dwCount)==ERROR_SUCCESS)
										{
											strcat(strcpy(cSubKeyName,"SYSTEM\\CurrentControlSet\\Services\\"),chDriver);
											chDriver[0]=0;
											if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,cSubKeyName,0,KEY_READ,&hSubKeyFour)==ERROR_SUCCESS)
											{
												chDriver[0]=0;
												dwCount=MAX_PATH;
												if (RegQueryValueEx(hSubKeyFour,"ImagePath",NULL,&dwType,(LPBYTE)chDriver,&dwCount)!=ERROR_SUCCESS) chDriver[0]=0;
												RegCloseKey(hSubKeyFour);
												hSubKeyFour=NULL;
											}
										}
										else chDriver[0]=0;
										AddDriver(chDeviceName,chDriver);
										RegCloseKey(hSubKeyTwo);
										hSubKeyTwo=NULL;
									}
								}
								RegCloseKey(hSubKey);
								hSubKey=NULL;
							}
						}
						if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv))
						{
							Ret=true;
							break;
						}
					}
					RegCloseKey(hKey);
					hKey=NULL;
				}
			}
		}
		fprintf(LFile,"\n");
		fflush(LFile);
		return Ret;
	}
	__finally
	{
		if (hSubKeyFour!=NULL) RegCloseKey(hSubKeyFour);
		if (hSubKeyThree!=NULL) RegCloseKey(hSubKeyThree);
		if (hSubKeyTwo!=NULL) RegCloseKey(hSubKeyTwo);
		if (hSubKey!=NULL) RegCloseKey(hSubKey);
		if (hKey!=NULL) RegCloseKey(hKey);
	}
	return Ret;
}

bool GetRemovablesInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	HKEY						hKey=NULL;
	HKEY						hSubKey=NULL;
	HKEY						hSubKeyTwo=NULL;
	HKEY						hSubKeyThree=NULL;
	HKEY						hSubKeyFour=NULL;
	DWORD						cSubKeysNumber=0;
	DWORD						cSubKeysNumberTwo=0;
	DWORD						cSubKeysNumberThree=0;
	DWORD						cSubKeysNumberFour=0;
	TCHAR						cSubKeyName[MAX_PATH];
	TCHAR						chDeviceName[MAX_PATH];
	TCHAR						chDriver[MAX_PATH];
	DWORD						i,j,k,l,retCode;
	DWORD						dwType=REG_SZ;
	BOOL						Removable;
	DWORD						dwCount=MAX_PATH;
	DWORD						StepCount=0;
	bool						Ret=false;
	chRemovableDevicesParsInfo	chRemovableDevices[REMOVABLETYPE_NUMBER]={
		{"FDC","{4D36E980-E325-11CE-BFC1-08002BE10318}"},
		{"IDE","{4D36E965-E325-11CE-BFC1-08002BE10318}"}
	};
				
	__try
	{
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
		if (os<OS_UNKNOWN)
		{
			if (RegOpenKeyEx(HKEY_DYN_DATA,"Config Manager\\Enum",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS) RegQueryInfoKey(hKey,NULL,NULL,NULL,&StepCount,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
		}
		else
		{
			if (os<OS_WIN2000)
			{
				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"HARDWARE\\DEVICEMAP\\Scsi",0,KEY_READ,&hKey)==ERROR_SUCCESS) RegQueryInfoKey(hKey,NULL,NULL,NULL,&StepCount,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
			}
			else
			{
				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Enum",0,KEY_READ,&hKey)==ERROR_SUCCESS)
				{
					for (i=0;i<REMOVABLETYPE_NUMBER;i++)
					{
						if (RegOpenKeyEx(hKey,chRemovableDevices[i].chKeyName,0,KEY_READ,&hSubKey)==ERROR_SUCCESS)
						{
							RegQueryInfoKey(hSubKey,NULL,NULL,NULL,&cSubKeysNumber,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
							StepCount=StepCount+cSubKeysNumber;
							if (hSubKey!=NULL) RegCloseKey(hSubKey);
						}
					}
					cSubKeysNumber=0;
				}
			}
		}
		if (hKey!=NULL) RegCloseKey(hKey);
		hKey=NULL;
		hSubKey=NULL;
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,StepCount);
		if (hProgressCtrl) SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
		if (CountStruct&&StepCount) CountStruct->LocalStepCount=StepCount;
		fprintf(LFile,"Removable devices:\n");
		fflush(LFile);
		if (os<OS_UNKNOWN)
		{
			if (RegOpenKeyEx(HKEY_DYN_DATA,"Config Manager\\Enum",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
			{
				RegQueryInfoKey(hKey,NULL,NULL,NULL,&cSubKeysNumber,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
				for (i=0;i<cSubKeysNumber;i++)
				{
					cSubKeyName[0]=0;
					retCode=RegEnumKey(hKey,i,cSubKeyName,MAX_PATH); 
					if ((retCode==(DWORD)ERROR_SUCCESS)&&(retCode!=ERROR_INSUFFICIENT_BUFFER)) 
					{
						if (RegOpenKeyEx(hKey,cSubKeyName,0,KEY_READ,&hSubKey)==ERROR_SUCCESS)
						{
							dwType=REG_SZ;
							dwCount=MAX_PATH;
							chDeviceName[0]=0;
							if (RegQueryValueEx(hSubKey,"HardWareKey",NULL,&dwType,(LPBYTE)chDriver,&dwCount)==ERROR_SUCCESS)
							{
								strcat(strcpy(cSubKeyName,"Enum\\"),chDriver);
								chDriver[0]=0;
								if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,cSubKeyName,0,KEY_READ,&hSubKeyTwo)==ERROR_SUCCESS)
								{
									
									chDriver[0]=0;
									dwCount=MAX_PATH;
									if (RegQueryValueEx(hSubKeyTwo,"ClassGUID",NULL,&dwType,(LPBYTE)chDriver,&dwCount)==ERROR_SUCCESS)
									{
										if ((_stricmp(chDriver,"{4d36e967-e325-11ce-bfc1-08002be10318}")==0)|(_stricmp(chDriver,"{4d36e965-e325-11ce-bfc1-08002be10318}")==0))
										{
											dwCount=sizeof(DWORD);
											dwType=REG_BINARY;
											Removable=FALSE;
											if (RegQueryValueEx(hSubKeyTwo,"Removable",NULL,&dwType,(LPBYTE)&Removable,&dwCount)==ERROR_SUCCESS)
											{
												if (Removable)
												{
													dwType=REG_SZ;
													dwCount=MAX_PATH;
													chDeviceName[0]=0;
													if (RegQueryValueEx(hSubKeyTwo,"DeviceDesc",NULL,&dwType,(LPBYTE)chDeviceName,&dwCount)==ERROR_SUCCESS)
													{
														chDriver[0]=0;
														dwCount=MAX_PATH;
														if (RegQueryValueEx(hSubKeyTwo,"Driver",NULL,&dwType,(LPBYTE)chDriver,&dwCount)==ERROR_SUCCESS)
														{
															strcat(strcpy(cSubKeyName,"System\\CurrentControlSet\\Services\\Class\\"),chDriver);
															chDriver[0]=0;
															if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,cSubKeyName,0,KEY_READ,&hSubKeyThree)==ERROR_SUCCESS)
															{
																dwCount=MAX_PATH;
																chDriver[0]=0;
																if (RegQueryValueEx(hSubKeyThree,"Driver",NULL,&dwType,(LPBYTE)chDriver,&dwCount)!=ERROR_SUCCESS)
																{
																	chDriver[0]=0;
																	dwCount=MAX_PATH;
																	if (RegQueryValueEx(hSubKeyThree,"DriverDate",NULL,&dwType,(LPBYTE)chDriver,&dwCount)!=ERROR_SUCCESS) chDriver[0]=0;
																}
																RegCloseKey(hSubKeyThree);
																hSubKeyThree=NULL;
															}
														}
													}
													else chDriver[0]=0;
													AddDriver(chDeviceName,chDriver);
												}
											}
										}
									}
									RegCloseKey(hSubKeyTwo);
									hSubKeyTwo=NULL;
								}
							}
							RegCloseKey(hSubKey);
							hSubKey=NULL;
						}
					}
					if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv))
					{
						Ret=true;
						break;
					}
				}
				RegCloseKey(hKey);
				hKey=NULL;
			}
		}
		else
		{
			if (os<OS_WIN2000)
			{
				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"HARDWARE\\DEVICEMAP\\Scsi",0,KEY_READ,&hKey)==ERROR_SUCCESS)
				{
					RegQueryInfoKey(hKey,NULL,NULL,NULL,&cSubKeysNumber,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
					for (i=0;i<cSubKeysNumber;i++)
					{
						cSubKeyName[0]=0;
						retCode=RegEnumKey(hKey,i,cSubKeyName,MAX_PATH); 
						if ((retCode==(DWORD)ERROR_SUCCESS)&&(retCode!=ERROR_INSUFFICIENT_BUFFER)) 
						{
							if (RegOpenKeyEx(hKey,cSubKeyName,0,KEY_READ,&hSubKey)==ERROR_SUCCESS)
							{
								RegQueryInfoKey(hSubKey,NULL,NULL,NULL,&cSubKeysNumberTwo,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
								for (j=0;j<cSubKeysNumberTwo;j++)
								{
									cSubKeyName[0]=0;
									retCode=RegEnumKey(hSubKey,j,cSubKeyName,MAX_PATH); 
									if ((retCode==(DWORD)ERROR_SUCCESS)&&(retCode!=ERROR_INSUFFICIENT_BUFFER)) 
									{
										if (RegOpenKeyEx(hSubKey,cSubKeyName,0,KEY_READ,&hSubKeyTwo)==ERROR_SUCCESS)
										{
											RegQueryInfoKey(hSubKeyTwo,NULL,NULL,NULL,&cSubKeysNumberThree,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
											for (k=0;k<cSubKeysNumberThree;k++)
											{
												cSubKeyName[0]=0;
												retCode=RegEnumKey(hSubKeyTwo,k,cSubKeyName,MAX_PATH); 
												if ((retCode==(DWORD)ERROR_SUCCESS)&&(retCode!=ERROR_INSUFFICIENT_BUFFER)) 
												{
													if (RegOpenKeyEx(hSubKeyTwo,cSubKeyName,0,KEY_READ,&hSubKeyThree)==ERROR_SUCCESS)
													{
														RegQueryInfoKey(hSubKeyThree,NULL,NULL,NULL,&cSubKeysNumberFour,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
														for (l=0;l<cSubKeysNumberFour;l++)
														{
															cSubKeyName[0]=0;
															retCode=RegEnumKey(hSubKeyThree,l,cSubKeyName,MAX_PATH); 
															if ((retCode==(DWORD)ERROR_SUCCESS)&&(retCode!=ERROR_INSUFFICIENT_BUFFER)) 
															{
																if (RegOpenKeyEx(hSubKeyThree,cSubKeyName,0,KEY_READ,&hSubKeyFour)==ERROR_SUCCESS)
																{
																	dwCount=MAX_PATH;
																	chDeviceName[0]=0;
																	if (RegQueryValueEx(hSubKeyFour,"Type",NULL,&dwType,(LPBYTE)cSubKeyName,&dwCount)==ERROR_SUCCESS)
																	{
																		if (_stricmp(cSubKeyName,"CdRomPeripheral")==0)
																		{
																			dwCount=MAX_PATH;
																			chDeviceName[0]=0;
																			chDriver[0]=0;
																			if (RegQueryValueEx(hSubKeyFour,"Identifier",NULL,&dwType,(LPBYTE)chDeviceName,&dwCount)!=ERROR_SUCCESS) chDeviceName[0]=0;
																			AddDriver(chDeviceName,chDriver);
																		}
																	}
																}
																RegCloseKey(hSubKeyFour);
																hSubKeyFour=NULL;
															}
														}
														RegCloseKey(hSubKeyThree);
														hSubKeyThree=NULL;
													}
												}
											}
											RegCloseKey(hSubKeyTwo);
											hSubKeyTwo=NULL;
										}
									}
								}
								RegCloseKey(hSubKey);
								hSubKey=NULL;
							}
						}
						if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv))
						{
							Ret=true;
							break;
						}
					}
					RegCloseKey(hKey);
					hKey=NULL;
				}
			}
			else
			{
				if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Enum",0,KEY_READ,&hKey)==ERROR_SUCCESS)
				{
					for (i=0;i<REMOVABLETYPE_NUMBER;i++)
					{
						if (RegOpenKeyEx(hKey,chRemovableDevices[i].chKeyName,0,KEY_READ,&hSubKey)==ERROR_SUCCESS)
						{
							RegQueryInfoKey(hSubKey,NULL,NULL,NULL,&cSubKeysNumber,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
							for (j=0;j<cSubKeysNumber;j++)
							{
								cSubKeyName[0]=0;
								retCode=RegEnumKey(hSubKey,j,cSubKeyName,MAX_PATH); 
								if ((retCode==(DWORD)ERROR_SUCCESS)&&(retCode!=ERROR_INSUFFICIENT_BUFFER)) 
								{
									if (RegOpenKeyEx(hSubKey,cSubKeyName,0,KEY_READ,&hSubKeyTwo)==ERROR_SUCCESS)
									{
										RegQueryInfoKey(hSubKeyTwo,NULL,NULL,NULL,&cSubKeysNumberTwo,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
										for (k=0;k<cSubKeysNumberTwo;k++)
										{
											cSubKeyName[0]=0;
											retCode=RegEnumKey(hSubKeyTwo,k,cSubKeyName,MAX_PATH); 
											if ((retCode==(DWORD)ERROR_SUCCESS)&&(retCode!=ERROR_INSUFFICIENT_BUFFER)) 
											{
												if (RegOpenKeyEx(hSubKeyTwo,cSubKeyName,0,KEY_READ,&hSubKeyThree)==ERROR_SUCCESS)
												{
													dwCount=MAX_PATH;
													chDeviceName[0]=0;
													if (RegQueryValueEx(hSubKeyThree,"ClassGUID",NULL,&dwType,(LPBYTE)chDeviceName,&dwCount)==ERROR_SUCCESS)
													{
														if (_stricmp(chDeviceName,chRemovableDevices[i].chGUID)==0)
														{
															dwCount=MAX_PATH;
															chDeviceName[0]=0;
															if (RegQueryValueEx(hSubKeyThree,"FriendlyName",NULL,&dwType,(LPBYTE)chDeviceName,&dwCount)!=ERROR_SUCCESS) chDeviceName[0]=0;
															if (strlen(chDeviceName)==0)
															{
																dwCount=MAX_PATH;
																chDeviceName[0]=0;
																if (RegQueryValueEx(hSubKeyThree,"DeviceDesc",NULL,&dwType,(LPBYTE)chDeviceName,&dwCount)!=ERROR_SUCCESS) chDeviceName[0]=0;
															}
															chDriver[0]=0;
															dwCount=MAX_PATH;
															if (RegQueryValueEx(hSubKeyThree,"Service",NULL,&dwType,(LPBYTE)chDriver,&dwCount)==ERROR_SUCCESS)
															{
																strcat(strcpy(cSubKeyName,"SYSTEM\\CurrentControlSet\\Services\\"),chDriver);
																chDriver[0]=0;
																if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,cSubKeyName,0,KEY_READ,&hSubKeyFour)==ERROR_SUCCESS)
																{
																	chDriver[0]=0;
																	dwCount=MAX_PATH;
																	if (RegQueryValueEx(hSubKeyFour,"ImagePath",NULL,&dwType,(LPBYTE)chDriver,&dwCount)!=ERROR_SUCCESS) chDriver[0]=0;
																	RegCloseKey(hSubKeyFour);
																	hSubKeyFour=NULL;
																}
															}
															else chDriver[0]=0;
															AddDriver(chDeviceName,chDriver);
														}
													}
													RegCloseKey(hSubKeyThree);
													hSubKeyThree=NULL;
												}
											}
										}
										RegCloseKey(hSubKeyTwo);
										hSubKeyTwo=NULL;
									}
								}
								if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv))
								{
									Ret=true;
									break;
								}
							}
							RegCloseKey(hSubKey);
							hSubKey=NULL;
						}
						else
						{
							if (CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv))
							{
								Ret=true;
								break;
							}
						}
					}
					RegCloseKey(hKey);
					hKey=NULL;
				}
			}
		}
		fprintf(LFile,"\n");
		fflush(LFile);
		return Ret;
	}
	__finally
	{
		if (hSubKeyFour!=NULL) RegCloseKey(hSubKeyFour);
		if (hSubKeyThree!=NULL) RegCloseKey(hSubKeyThree);
		if (hSubKeyTwo!=NULL) RegCloseKey(hSubKeyTwo);
		if (hSubKey!=NULL) RegCloseKey(hSubKey);
		if (hKey!=NULL) RegCloseKey(hKey);
	}
	return Ret;
}

bool GetDateInfo(HWND hProgressCtrl,pCOUNTSTRUCT CountStruct,HANDLE hEv)
{
	SYSTEMTIME	lpSystemTime;
	bool		Ret=false;
	__try
	{
		SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETPOS,0,0);
		SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETRANGE32,0,1);
		SendMessage(GetDlgItem(hProgressCtrl,IDC_PROGRESS_CURRENT),PBM_SETSTEP,1,0);
		if (CountStruct) CountStruct->LocalStepCount=1;
		GetSystemTime(&lpSystemTime);
		fprintf(LFile,"System Date:\n\t\t%0.2d/%0.2d/%0.4d (dd/mm/yyyy)\n\n",lpSystemTime.wDay,lpSystemTime.wMonth,lpSystemTime.wYear);
		fflush(LFile);
		Ret=CheckStop(hProgressCtrl,IDC_PROGRESS_CURRENT,CountStruct,false,hEv);
		return Ret;
	}
	__finally
	{
	}
	return Ret;
}



