#include "stdafx.h"
#include "SysInfo.h"
#include "FormatTimeString.h"

using namespace KLUTIL;

enum e_CPUVENDORS 
{ 
	VENDOR_UNKNOWN = 0, 
	VENDOR_INTEL, 
	VENDOR_AMD, 
	VENDOR_CYRIX, 
	VENDOR_CENTAUR, 
};

#define VENDOR_INTEL_STR		_T("GenuineIntel")
#define VENDOR_AMD_STR			_T("AuthenticAMD")
#define VENDOR_CYRIX_STR		_T("CyrixInstead")
#define VENDOR_CENTAUR_STR		_T("CentaurHauls")

#define BRANDTABLESIZE			4

#define NAMESTRING_FEATURE		0x80000004     // this is the namestring feature; goes from 0x80000002 to 0x80000004
#define MMX_FLAG				0x00800000     // [Bit 23] MMX supported                               

#define INFO_TYPE_COUNT					23
#define PROCESSOR_STEP_COUNT			8
#define HARD_NT_STEP_COUNT				8
#define HARD_NT_STEP2_STEP_COUNT		4
#define HARD_9X_STEP_COUNT				400510
#define FREEDISK_STEP_COUNT				78

enum SI_INSIDE_OS_NUMBER
{
	OS_WIN32=0, //VER_PLATFORM_WIN32s
	OS_WIN_UNKNOWN, //VER_PLATFORM_WIN32_WINDOWS unknown minor version
	OS_WIN95,
	OS_WIN98,
	OS_WINME,
	RESERVED5,
	RESERVED6,
	RESERVED7,
	RESERVED8,
	RESERVED9,
	OS_UNKNOWN, //unknown dwPlatformId or error
	OS_WINNT351,
	OS_WINNT40,
	OS_WIN2000,
	OS_WINXP,
	OS_WINXPNET,
};

static OSVERSIONINFOEX		versionInfo;
static bool				ExType;

typedef LANGID (WINAPI *pfnGetUserDefaultUILanguageType)(void);
typedef BOOL (WINAPI *GetCPInfoExPtr)(UINT CodePage,DWORD dwFlags,LPCPINFOEX lpCPInfoEx);

SI_INSIDE_OS_NUMBER IsRunningWindowsNT ()
{
	SI_INSIDE_OS_NUMBER	Ret=OS_UNKNOWN;
	_try
	{
		versionInfo.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
		if ((::GetVersionEx((OSVERSIONINFO*)&versionInfo))==0)
		{
			ExType=false;
			versionInfo.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
			if ((GetVersionEx((OSVERSIONINFO*)&versionInfo))==0) return Ret;
		}
		switch (versionInfo.dwPlatformId)
		{
		case VER_PLATFORM_WIN32_NT:
			if (versionInfo.dwMajorVersion<=3) Ret=OS_WINNT351; //WinNT
			else
			{
				if (versionInfo.dwMajorVersion==4) Ret=OS_WINNT40; //WinNT
				else
				{
					if ((versionInfo.dwMajorVersion==5)&&(versionInfo.dwMinorVersion==0)) Ret=OS_WIN2000; //Win2000
					else
					{
						if ((versionInfo.dwMajorVersion==5)&&(versionInfo.dwMinorVersion==1)) Ret=OS_WINXP; //WinXP
						else
						{
							if ((versionInfo.dwMajorVersion==5)&&(versionInfo.dwMinorVersion==2)) Ret=OS_WINXPNET; //WinXP
							else Ret=OS_WIN_UNKNOWN;
						}
					}
				}
			}
			break;
		case VER_PLATFORM_WIN32_WINDOWS:
			if ((versionInfo.dwMajorVersion==4)&&(versionInfo.dwMinorVersion>=90)) Ret=OS_WINME; //WinME
			else
			{
				if ((versionInfo.dwMajorVersion==4)&&(versionInfo.dwMinorVersion>=10)) Ret=OS_WIN98; //Win98
				else
				{
					if ((versionInfo.dwMajorVersion==4)&&(versionInfo.dwMinorVersion>=0)) Ret=OS_WIN95; //Win95
					else Ret=OS_WIN_UNKNOWN;
				}
			}
			break;
		case VER_PLATFORM_WIN32s:
			Ret=OS_WIN32;
			break;
		default:
			break;
		}
		return Ret;
	}
	__finally
	{
	}
	return Ret;
}

static SI_INSIDE_OS_NUMBER	os = IsRunningWindowsNT ();

bool DisplaySystemVersion (CStdString& out)
{
	CStdString strVersion;
	
	CStdString strOS;
	CStdString strBuild;
	
	strOS = _T("Microsoft Windows");
	switch (os)
	{
	case OS_WIN32:
		strOS += _T(" 3.1");
		break;
	case OS_WIN95:
		strOS += _T(" 95");
		if ((versionInfo.szCSDVersion[1]=='C')||(versionInfo.szCSDVersion[1]=='B')) strOS += _T(" OSR2");
		break;
	case OS_WIN98:
		strOS += _T(" 98");
		if (versionInfo.szCSDVersion[1]=='A') strOS += _T(" SE");
		break;
	case OS_WINME:
		strOS += _T(" ME");
		break;
	case OS_WINNT351:
		strOS += _T(" NT 3.51");
		if (versionInfo.wProductType==VER_NT_WORKSTATION) strOS += _T(" Workstation");
		if (versionInfo.wProductType==VER_NT_DOMAIN_CONTROLLER) strOS += _T(" Domain controller");
		if (versionInfo.wProductType==VER_NT_SERVER) strOS += _T(" Server");
		break;
	case OS_WINNT40:
		strOS += _T(" NT 4.0");
		if (versionInfo.wProductType==VER_NT_WORKSTATION) strOS += _T(" Workstation");
		if (versionInfo.wProductType==VER_NT_DOMAIN_CONTROLLER) strOS += _T(" Domain controller");
		if (versionInfo.wProductType==VER_NT_SERVER) strOS += _T(" Server");
		break;
	case OS_WIN2000:
		strOS += _T(" 2000");
		if (versionInfo.wProductType==VER_NT_WORKSTATION) strOS += _T(" Professional");
		if (versionInfo.wProductType==VER_NT_DOMAIN_CONTROLLER) strOS += _T(" Domain controller");
		if (versionInfo.wProductType==VER_NT_SERVER) 
		{
			if ((versionInfo.wSuiteMask&VER_SUITE_DATACENTER)==VER_SUITE_DATACENTER) strOS += _T(" DataCenter Server");
			else
			{
				if ((versionInfo.wSuiteMask&VER_SUITE_ENTERPRISE)==VER_SUITE_ENTERPRISE) strOS += _T(" Advanced Server");
				else
				{
					if ((versionInfo.wSuiteMask&VER_SUITE_BLADE)==VER_SUITE_BLADE) strOS += _T(" Web Server");
					else strOS += _T(" Server");
				}
			}
		}
		break;
	case OS_WINXP:
		strOS += _T(" XP");
		if (versionInfo.wProductType==VER_NT_WORKSTATION)
		{
			if ((versionInfo.wSuiteMask&VER_SUITE_PERSONAL)==VER_SUITE_PERSONAL) strOS += _T(" Home Etition");
			else strOS += _T(" Professional Edition");
		}
		if (versionInfo.wProductType==VER_NT_DOMAIN_CONTROLLER) strOS += _T(" Domain controller");
		if (versionInfo.wProductType==VER_NT_SERVER)
		{
			if ((versionInfo.wSuiteMask&VER_SUITE_DATACENTER)==VER_SUITE_DATACENTER) strOS += _T(" DataCenter Server");
			else
			{
				if ((versionInfo.wSuiteMask&VER_SUITE_ENTERPRISE)==VER_SUITE_ENTERPRISE) strOS += _T(" Enterprise Server");
				else
				{
					if ((versionInfo.wSuiteMask&VER_SUITE_BLADE)==VER_SUITE_BLADE) strOS += _T(" Web Server");
					else strOS += _T(" Server");
				}
			}
		}
		break;
	case OS_WINXPNET:
		strOS += _T(" .NET");
		if (versionInfo.wProductType==VER_NT_WORKSTATION)
		{
			if ((versionInfo.wSuiteMask&VER_SUITE_PERSONAL)==VER_SUITE_PERSONAL) strOS += _T(" Home Etition");
			else strOS += _T(" Professional Edition");
		}
		if (versionInfo.wProductType==VER_NT_DOMAIN_CONTROLLER) strOS += _T(" Domain controller");
		if (versionInfo.wProductType==VER_NT_SERVER)
		{
			if ((versionInfo.wSuiteMask&VER_SUITE_DATACENTER)==VER_SUITE_DATACENTER) strOS += _T(" DataCenter Server");
			else
			{
				if ((versionInfo.wSuiteMask&VER_SUITE_ENTERPRISE)==VER_SUITE_ENTERPRISE) strOS += _T(" Enterprise Server");
				else
				{
					if ((versionInfo.wSuiteMask&VER_SUITE_BLADE)==VER_SUITE_BLADE) strOS += _T(" Web Server");
					else strOS += _T(" Server");
				}
			}
		}
		break;
	default:
		strOS = _T("Unknown OS");
		break;
	}
	strBuild.Format("%u.%u.%u %s"
		, versionInfo.dwMajorVersion
		, versionInfo.dwMinorVersion
		, (DWORD)LOWORD(versionInfo.dwBuildNumber)
		, /*Unicode need !!! A2CT*/(versionInfo.szCSDVersion));
	
	strVersion.Format(_T("Operation system:\n\t%s, %s\n"), (LPCTSTR) strOS, (LPCTSTR) strBuild);
	
	out += strVersion;

	return true;
}

bool DisplayIEVersion(CStdString& out)
{
    HKEY    hKey;
    TCHAR	buf_ver[256]= { _T('\0') }, buf_build[256] = { _T('\0') };
	DWORD	size_ver = sizeof (buf_ver), size_build = sizeof (buf_build);
    
    if (ERROR_SUCCESS == RegOpenKey(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Internet Explorer"), &hKey))
    {
        RegQueryValueEx (hKey, _T("Version"), NULL, NULL, (LPBYTE)buf_ver, &size_ver);
		RegQueryValueEx (hKey, _T("Build"), NULL, NULL, (LPBYTE)buf_build, &size_build);

        RegCloseKey(hKey);
    }

	if (buf_ver[0])
	{
		out += _T("Internet explorer version:\n\t");
		out += buf_ver;
		if (buf_build[0])
		{
			out += _T(" (Build ");
			out += buf_build;
			out += _T(")");
		}
		out += _T("\n");
	}		

    return true;
}

bool DisplaySystemTime(CStdString& out)
{
    TIME_ZONE_INFORMATION tz;

	GetTimeZoneInformation (&tz);

	CStdString strTime;
	strTime.Format (_T("%s (UTC%+d, %ls)")
		, KLUTIL::CSysTime::GetCurrentTime().GetDateTimeString().c_str()
		, -tz.Bias / 60
		, tz.StandardName);

    out += _T("System information\n") + strTime + _T("\n");
	out += SYS_INFO_SEP;

    return true;
}

bool DisplayMemInfo(CStdString& out)
{
    MEMORYSTATUS MemStat;
    
    memset(&MemStat,0,sizeof(MEMORYSTATUS));
    GlobalMemoryStatus (&MemStat);
    
    if ((MemStat.dwTotalPhys>>20) > 0) 
		out.AppendFormat(_T("Total physical memory:\n\t%d,%0.3d Mb")
			, MemStat.dwTotalPhys>>20, (MemStat.dwTotalPhys&0xffc00)>>10);
	else 
        out.AppendFormat(_T("Total physical memory:\n\t%d Kb"), MemStat.dwTotalPhys>>10);
		
    if ((MemStat.dwTotalVirtual>>20) > 0)         
        out.AppendFormat(_T("\nTotal virtual memory:\n\t%d,%0.3d Mb"), MemStat.dwTotalVirtual>>20,(MemStat.dwTotalVirtual&0xffc00)>>10);
	else 
        out.AppendFormat(_T("\nTotal virtual memory:\n\t%d,%0.3d Kb"), MemStat.dwTotalVirtual>>10);
		
    if ((MemStat.dwTotalPhys>>20) > 0) 
        out.AppendFormat(_T("\nAvailable physical memory:\n\t%d,%0.3d Mb"), MemStat.dwTotalPhys>>20,(MemStat.dwTotalPhys&0xffc00)>>10);
    else 
        out.AppendFormat(_T("\nAvailable physical memory:\n\t%d,%0.3d Kb"), MemStat.dwTotalPhys>>10);
		
    if ((MemStat.dwAvailVirtual>>20) > 0) 
        out.AppendFormat(_T("\nAvailable virtual memory:\n\t%d,%0.3d Mb\n\n"),MemStat.dwAvailVirtual>>20,(MemStat.dwAvailVirtual&0xffc00)>>10);
    else 
        out.AppendFormat(_T("\nAvailable virtual memory:\n\t%d,%0.3d Kb\n"),MemStat.dwAvailVirtual>>10);

    return true;
}

bool DisplayEmailClients(CStdString& out)
{
    HKEY    hKey;
    TCHAR   cSubKeyName[MAX_PATH];
    unsigned long    size = MAX_PATH;
    unsigned long    nKeyCount;
    TCHAR   DefaultClient[MAX_PATH];

    out += ("Registered email clients:\n");

    if (ERROR_SUCCESS == RegOpenKey(HKEY_LOCAL_MACHINE, _T("Software\\Clients\\Mail"), &hKey))
    {
        RegQueryValueEx(hKey, _T(""), NULL, NULL, (LPBYTE)DefaultClient, &size);

        if (ERROR_SUCCESS == RegQueryInfoKey(hKey,NULL,NULL,NULL,&nKeyCount,NULL,NULL,NULL,NULL,NULL,NULL,NULL))
        {
            for (unsigned int i = 0; i < nKeyCount; ++i)
            {
                if (ERROR_SUCCESS == RegEnumKey(hKey, i, cSubKeyName, 256))
                {
					out += _T("\t");
                    out += cSubKeyName;
                    if (_tcscmp (cSubKeyName, DefaultClient) == 0)
                        out += _T(" (default)\n");
                    else
                        out += _T("\n");
                }

            }
        }
        RegCloseKey(hKey);
    }
    
    return true;
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
		if (dwHighest != 0)
			_tcscpy(VendorID, szTemp);
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
		if ((_tcscmp(Vendor,VENDOR_INTEL_STR))==0) eVendor=VENDOR_INTEL;
		if (_tcscmp(Vendor,VENDOR_AMD_STR)==0) eVendor=VENDOR_AMD;
		if (_tcscmp(Vendor,VENDOR_CYRIX_STR)==0) eVendor=VENDOR_CYRIX;
		if (_tcscmp(Vendor,VENDOR_CENTAUR_STR)==0) eVendor=VENDOR_CENTAUR;
		return eVendor;
	}
	__finally
	{
	}
	return eVendor;
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

void determineOldIntelName(DWORD dwHighestCpuId,DWORD Signature,DWORD CacheEax,DWORD CacheEbx,DWORD CacheEcx,DWORD CacheEdx,DWORD FeatureEbx,DWORD Features,CStdString& strCpuInfo)
{
   struct brand_entry
   {
      long  lBrandValue;
      TCHAR* pszBrand;
   };

   struct brand_entry brand_table[BRANDTABLESIZE] = 
   {
      1, _T("Genuine Intel Celeron(TM) processor"),
      2, _T("Genuine Intel Pentium(R) III processor"),
      3, _T("Genuine Intel Pentium(R) III Xeon(TM) processor"),
      8, _T("Genuine Intel Pentium(R) 4 processor")
   };

	   if (dwHighestCpuId<NAMESTRING_FEATURE)
	   {
		  switch ((Signature>>8)&0x0f)
		  {
		  case 4:  // 486
			 switch((Signature>>4)&0x0f)
			 {
			 case 0:
			 case 1:
				strCpuInfo = _T("Intel486(TM) DX processor");
				break;
			 case 2:
				strCpuInfo = _T("Intel486(TM) SX processor");
				break;
			 case 3:
				strCpuInfo = _T("IntelDX2(TM) processor");
				break;
			 case 4:
				strCpuInfo = _T("Intel486(TM) processor");
				break;
			 case 5:
				strCpuInfo = _T("IntelSX2(TM) processor");
				break;
			 case 7:
				strCpuInfo = _T("Writeback Enhanced IntelDX2(TM) processor");
				break;
			 case 8:
				strCpuInfo = _T("IntelDX4(TM) processor");
				break;
			 default:
				strCpuInfo = _T("Intel 486 processor");
				break;
				}

				break;

		  case 5:  // pentium
			 strCpuInfo = _T("Intel Pentium(R) processor");
			 break;
			 
		  case 6:  // pentium II and family
			  switch ((Signature>>4)&0x0f)
			  {
			  case 1:
				  strCpuInfo = _T("Intel Pentium(R) Pro processor");
				  break;
			  case 3:
				  strCpuInfo = _T("Intel Pentium(R) II processor, model 3");
				  break;
			  case 5:
			  case 7:
				  if (determineCeleron(CacheEax,CacheEbx,CacheEcx,CacheEdx))
					  strCpuInfo = _T("Intel Celeron(TM) processor, model 5");
				  else
				  {
					  if (determineXeon(CacheEax,CacheEbx,CacheEcx,CacheEdx))
					  {
						  if (((Signature>>4)&0x0f)==5)
							  strCpuInfo = _T("Intel Pentium(R) II Xeon(TM) processor");
						  else
							  strCpuInfo = _T("Intel Pentium(R) III Xeon(TM) processor");
					  }
					  else
					  {
						  if (((Signature>>4)&0x0f)==5)
							  strCpuInfo = _T("Intel Pentium(R) II processor, model 5");
						  else
							  strCpuInfo = _T("Intel Pentium(R) III processor");
					  }
				  }
				  
				  break;
				  
			  case 6:
				  strCpuInfo = _T("Intel Celeron(TM) processor, model 6");
				  break;
			  case 8:
				  strCpuInfo = _T("Intel Pentium(R) III Coppermine processor");
				  break;
			  default:
				  {
					  int brand_index = 0;
					  while ((brand_index<BRANDTABLESIZE)&&((FeatureEbx&0xff)!=(DWORD)brand_table[brand_index].lBrandValue)) brand_index++;
					  if (brand_index<BRANDTABLESIZE) strCpuInfo=brand_table[brand_index].pszBrand;
					  else
						  strCpuInfo = _T("Unknown Genuine Intel processor");
					  
				  }
				  break;
			  } 
		  }
		  
		  if (Features&MMX_FLAG)
			  strCpuInfo += _T(" with MMX");
	   }
	   return;
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

void determineNameString(CStdString& strCpuInfo)
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
		strCpuInfo = szName;
		return;
	}
	__finally
	{
	}
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

DWORD getCpuSpeedFromRegistry(void)
{
	HKEY hKey=NULL;
	LONG result;
	DWORD dwSpeed = 0;
	DWORD dwType = 0;
	DWORD dwSpeedSize;
	
	try
	{
		result = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Hardware\\Description\\System\\CentralProcessor\\0"), 0, KEY_QUERY_VALUE,&hKey);
		if (result == ERROR_SUCCESS)
		{
			result = ::RegQueryValueEx(hKey,_T("~MHz"), NULL, NULL, (LPBYTE)&dwSpeed, &dwSpeedSize);
			if (result != ERROR_SUCCESS) dwSpeed = 0;
		}
	}
	catch(...){}
	if (hKey!=NULL) RegCloseKey(hKey);
	return (dwSpeed);
}

DWORD determineCpuSpeed(DWORD VendorID,DWORD Signature)
{
	DWORD	dwSpeed=0;
	try
	{
		if (((VendorID==VENDOR_INTEL)&&(((Signature>>8)&0x0f)>4))||((VendorID==VENDOR_AMD)&&(((Signature>>8)&0x0f)>5)))
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

bool DisplayProcessor (CStdString &strSysInfo)
{
	CStdString  strCpuInfo;
	TCHAR		VendorID[16] = _T("Unknown");

	DWORD		CacheEax=0;
	DWORD		CacheEbx=0;
	DWORD		CacheEcx=0;
	DWORD		CacheEdx=0;
	DWORD		Signature=0;
	DWORD		FeatureEbx=0;
	DWORD		FeatureEcx=0;
	DWORD		Features=0;
	DWORD		dwSpeed=0;

	if (!determineCpuId()) 
		return false;

	DWORD dwHighestCpuId = determineHighestCpuId ((TCHAR*)VendorID);

	switch (dwHighestCpuId)
	{
	default:
	case 2:     // intel cpu's find cache information here
		determineIntelCacheInfo(&CacheEax,&CacheEbx,&CacheEcx,&CacheEdx);
	case 1:     // x86 cpu's do processor identification here
		determineCpuIdentification(&Signature,&FeatureEbx,&FeatureEcx,&Features);
		break;
	case 0:     // don't do anything funky; return
		return false;
	}

	if (getVendor (VendorID) == VENDOR_INTEL)
		determineOldIntelName (dwHighestCpuId,Signature,CacheEax,CacheEbx,CacheEcx,CacheEdx,FeatureEbx,Features,strCpuInfo);

	DWORD dwLargestExtendedFeature = determineLargestExtendedFeature(); 

	if (dwLargestExtendedFeature >= NAMESTRING_FEATURE)
		determineNameString(strCpuInfo); 

	dwSpeed = determineCpuSpeed (getVendor (VendorID), Signature);

	if (dwSpeed != 0)
		strCpuInfo.AppendFormat(_T(", %i MHz"), dwSpeed);
	else
		strCpuInfo += _T(" Unknown speed");

	strCpuInfo.TrimLeft(_T(' '));

	SYSTEM_INFO stCpuInfo;
	GetSystemInfo (&stCpuInfo);

	CStdString strProcessor;
	strProcessor.Format (_T("Processor:\n\t%s (%d)\n"), (LPCTSTR)strCpuInfo, stCpuInfo.dwNumberOfProcessors);

	strSysInfo += strProcessor;

	return true;
}

bool DisplayBIOSInfo (CStdString &strSysInfo)
{
	CStdString strBIOS;

	CStdString strBIOSInfo(_T("Unknown"));
	HKEY	hKey = NULL;
	TCHAR	VerBuf[MAX_PATH];
	DWORD	ValType = REG_SZ;
	DWORD	Size = MAX_PATH;

	if (os>OS_UNKNOWN)
	{
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("HARDWARE\\DESCRIPTION\\System"),0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
		{
			if (os<OS_WIN2000)
			{
				ValType=REG_BINARY;
				if (RegQueryValueEx(hKey,_T("SystemBiosVersion"),NULL,&ValType,(LPBYTE)&VerBuf,&Size)==ERROR_SUCCESS)
					strBIOSInfo = VerBuf;
				else
					strBIOSInfo = _T("Unknown");
			}
			else
			{
				ValType=REG_MULTI_SZ;
				if (RegQueryValueEx(hKey,_T("SystemBiosVersion"),NULL,&ValType,(LPBYTE)&VerBuf,&Size)==ERROR_SUCCESS)
				{
					if (ValType == REG_MULTI_SZ)
					{
						while (VerBuf[_tcslen(VerBuf)+1] != 0)
							VerBuf[_tcslen(VerBuf)] = _T(' ');
					}
					strBIOSInfo = VerBuf;
					ValType = REG_SZ;
					Size = MAX_PATH;
					if (RegQueryValueEx(hKey, _T("SystemBiosDate"), NULL, &ValType, (LPBYTE)&VerBuf, &Size)==ERROR_SUCCESS)
						strBIOSInfo += _T(" BIOS Date: ") + CStdString(VerBuf);
				}
				else 
				{
					ValType = REG_SZ;
					Size = MAX_PATH;
					if (RegQueryValueEx(hKey, _T("SystemBiosDate") ,NULL,&ValType,(LPBYTE)&VerBuf,&Size)==ERROR_SUCCESS)
						strBIOSInfo = _T("BIOS Date: ") + CStdString(VerBuf);
					else
						strBIOSInfo = _T("Unknown");
				}
			}
		}
	}
	else
	{
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Enum\\Root\\*PNP0C01\\0000"), 0, KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
		{
			strBIOSInfo = _T("");
			if (RegQueryValueEx(hKey, _T("BIOSName"),NULL,&ValType,(LPBYTE)&VerBuf,&Size)==ERROR_SUCCESS)
				strBIOSInfo += VerBuf + CStdString(_T(" "));
			Size=MAX_PATH;
			if (RegQueryValueEx(hKey,_T("BIOSDate"),NULL,&ValType,(LPBYTE)&VerBuf,&Size)==ERROR_SUCCESS)
				strBIOSInfo += VerBuf + CStdString(_T(" "));
			Size=MAX_PATH;
			if (RegQueryValueEx(hKey, _T("BIOSVersion"), NULL,&ValType,(LPBYTE)&VerBuf,&Size)==ERROR_SUCCESS)
				strBIOSInfo += VerBuf;
			if (strBIOSInfo.IsEmpty())
				strBIOSInfo = _T("Unknown");
		}
	}
	
	strBIOS.Format (_T("BIOS:\n\t%s\n"), (LPCTSTR) strBIOSInfo);
	
	strSysInfo += strBIOS;

	if (hKey != NULL)
		RegCloseKey(hKey);

	return true;
}

bool GetInstalledAppInfo (CStdString &out)
{
	DWORD	i;
	DWORD	cSubKeysNumber=0;
	const DWORD dwBufSize = 256;
	DWORD	rSize=dwBufSize;
	DWORD	rType=REG_SZ;
	HKEY	hKey=NULL;
	HKEY	hKeyUn=NULL;
	TCHAR	chSubKeyName[dwBufSize];
	bool	Ret=false;
	out += _T("Installed applications/hotfixes:\n");
	
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"),0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
	{
		RegQueryInfoKey(hKey,NULL,NULL,NULL,&cSubKeysNumber,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
		for (i = 0; i < cSubKeysNumber; ++i)
		{
			if (ERROR_SUCCESS == RegEnumKey(hKey, i, chSubKeyName, dwBufSize)
				&& ERROR_SUCCESS == RegOpenKeyEx(hKey, chSubKeyName, 0, KEY_READ, &hKeyUn)) 
			{
				bool Find = false;
				rSize = dwBufSize;
				if (ERROR_SUCCESS == RegQueryValueEx(hKeyUn, _T("DisplayName"), NULL, &rType, (LPBYTE)chSubKeyName, &rSize))
					Find = true;
				else
				{
					rSize = dwBufSize;
					if (ERROR_SUCCESS == RegQueryValueEx(hKeyUn, _T("QuietDisplayName"), NULL, &rType, (unsigned char*)chSubKeyName,&rSize))
						Find=true;
				}
				if (Find)
					out.AppendFormat(_T("\t%s\n"), chSubKeyName);
				if (hKeyUn != NULL)
					RegCloseKey(hKeyUn);
				hKeyUn=NULL;
			}
		}
	}

	if (hKeyUn!=NULL) RegCloseKey(hKeyUn);
	if (hKey!=NULL) RegCloseKey(hKey);

	return Ret;
}

typedef struct __HKL{
	HKL	lpListItem[1];
}_HKL,*pHKL;

void GetCodePageName(UINT wCodePage, CStdString& stCodePageName)
{
	HMODULE hKernel = GetModuleHandle(_T("kernel32.dll"));
	if (!hKernel)
		return;

	CPINFOEX CPInfoEx;
	GetCPInfoExPtr hGetCPInfoEx = NULL;
	hGetCPInfoEx = (GetCPInfoExPtr) GetProcAddress(hKernel, _T("GetCPInfoExA"));
	if (hGetCPInfoEx != NULL
		&& hGetCPInfoEx(wCodePage, 0, &CPInfoEx) != 0)
	{
		stCodePageName = CPInfoEx.CodePageName;
		return;
	}
	switch (wCodePage)
	{
	case 037:
		stCodePageName = _T("EBCDIC");
		break;
	case 290:
		stCodePageName = _T("IBM Extended English Katakana");
		break;
	case 300:
		stCodePageName = _T("IBM Japanese Character Sets");
		break;
	case 437:
		stCodePageName = _T("MS-DOS United States");
		break;
	case 500:
		stCodePageName = _T("EBCDIC \"500V1\"");
		break;
	case 708:
		stCodePageName = _T("Arabic (ASMO 708)");
		break;
	case 709:
		stCodePageName = _T("Arabic (ASMO 449+, BCON V4)");
		break;
	case 710:
		stCodePageName = _T("Arabic (Transparent Arabic)");
		break;
	case 711:
		stCodePageName = _T("Arabic (Nafitha Enhanced)");
		break;
	case 720:
		stCodePageName = _T("Arabic (Transparent ASMO)");
		break;
	case 737:
		stCodePageName = _T("Greek (formerly 437G)");
		break;
	case 775:
		stCodePageName = _T("Baltic");
		break;
	case 833:
		stCodePageName = _T("IBM Hangul Extended Single-Byte");
		break;
	case 834:
		stCodePageName = _T("Korean Host Double-byte");
		break;
	case 835:
		stCodePageName = _T("IBM Traditional Chinese Character Sets");
		break;
	case 836:
		stCodePageName = _T("Simplified Chinese Single-Byte");
		break;
	case 837:
		stCodePageName = _T("IBM Simplified Chinese Character Sets");
		break;
	case 850:
		stCodePageName = _T("MS-DOS Multilingual (Latin I)");
		break;
	case 852:
		stCodePageName = _T("MS-DOS Slavic (Latin II)");
		break;
	case 855:
		stCodePageName = _T("IBM Cyrillic (primarily Russian)");
		break;
	case 857:
		stCodePageName = _T("IBM Turkish");
		break;
	case 860:
		stCodePageName = _T("MS-DOS Portuguese");
		break;
	case 861:
		stCodePageName = _T("MS-DOS Icelandic");
		break;
	case 862:
		stCodePageName = _T("Hebrew");
		break;
	case 863:
		stCodePageName = _T("MS-DOS Canadian-French");
		break;
	case 864:
		stCodePageName = _T("Arabic");
		break;
	case 865:
		stCodePageName = _T("MS-DOS Nordic");
		break;
	case 866:
		stCodePageName = _T("MS-DOS Russian");
		break;
	case 869:
		stCodePageName = _T("IBM Modern Greek");
		break;
	case 870:
		stCodePageName = _T("IBM EBCDIC Latin-2");
		break;
	case 874:
		stCodePageName = _T("Thai");
		break;
	case 875:
		stCodePageName = _T("EBCDIC");
		break;
	case 932:
		stCodePageName = _T("Japanese");
		break;
	case 936:
		stCodePageName = _T("Chinese (PRC, Singapore)");
		break;
	case 949:
		stCodePageName = _T("Korean");
		break;
	case 950:
		stCodePageName = _T("Chinese (Taiwan; Hong Kong SAR, PRC)");
		break;
	case 1026:
		stCodePageName = _T("EBCDIC");
		break;
	case 1027:
		stCodePageName = _T("IBM Extended Lowercase English");
		break;
	case 1200:
		stCodePageName = _T("Unicode (BMP of ISO 10646)");
		break;
	case 1201:
		stCodePageName = _T("Universal Alphabet (Unicode) (ISO-10646-UCS-2)");
		break;
	case 1250:
		stCodePageName = _T("Windows Eastern European");
		break;
	case 1251:
		stCodePageName = _T("Windows Cyrillic");
		break;
	case 1252:
		stCodePageName = _T("Windows US (ANSI)");
		break;
	case 1253:
		stCodePageName = _T("Windows Greek");
		break;
	case 1254:
		stCodePageName = _T("Windows Turkish");
		break;
	case 1255:
		stCodePageName = _T("Windows Hebrew");
		break;
	case 1256:
		stCodePageName = _T("Windows Arabic");
		break;
	case 1257:
		stCodePageName = _T("Windows Baltic");
		break;
	case 1258:
		stCodePageName = _T("Windows Vietnamese windows-1258");
		break;
	case 1361:
		stCodePageName = _T("Korean (Johab)");
		break;
	case 10000:
		stCodePageName = _T("Macintosh Roman");
		break;
	case 10001:
		stCodePageName = _T("Macintosh Japanese");
		break;
	case 10002:
		stCodePageName = _T("Macintosh Chinese");
		break;
	case 10003:
		stCodePageName = _T("Macintosh Korean");
		break;
	case 10004:
		stCodePageName = _T("Macintosh Arabic");
		break;
	case 10005:
		stCodePageName = _T("Macintosh Hebrew");
		break;
	case 10006:
		stCodePageName = _T("Macintosh Greek I");
		break;
	case 10007:
		stCodePageName = _T("Macintosh Cyrillic");
		break;
	case 10008:
		stCodePageName = _T("Macintosh");
		break;
	case 10010:
		stCodePageName = _T("Macintosh Romanian");
		break;
	case 10017:
		stCodePageName = _T("Macintosh Ukrainian");
		break;
	case 10029:
		stCodePageName = _T("Macintosh Latin 2");
		break;
	case 10079:
		stCodePageName = _T("Macintosh Icelandic");
		break;
	case 10081:
		stCodePageName = _T("Macintosh Turkish");
		break;
	case 10082:
		stCodePageName = _T("Macintosh Croatian");
		break;
	case 11009:
		stCodePageName = _T("Canadian Standard");
		break;
	case 20105:
		stCodePageName = _T("IA5 IRV");
		break;
	case 20106:
		stCodePageName = _T("IA6 (German)");
		break;
	case 20107:
		stCodePageName = _T("IA6 (Swedish)");
		break;
	case 20108:
		stCodePageName = _T("IA6 (Norwegian)");
		break;
	case 20261:
		stCodePageName = _T("T.61-8bit");
		break;
	case 20269:
		stCodePageName = _T("ISO-6937");
		break;
	case 20273:
		stCodePageName = _T("IBM EBCDIC Germany");
		break;
	case 20277:
		stCodePageName = _T("IBM EBCDIC Denmark/Norway");
		break;
	case 20278:
		stCodePageName = _T("IBM EBCDIC Finland/Sweden");
		break;
	case 20280:
		stCodePageName = _T("IBM EBCDIC Italy");
		break;
	case 20284:
		stCodePageName = _T("IBM EBCDIC Latin America/Spain");
		break;
	case 20285:
		stCodePageName = _T("IBM EBCDIC United Kingdom");
		break;
	case 20290:
		stCodePageName = _T("IBM EBCDIC Japanese IBM290");
		break;
	case 20297:
		stCodePageName = _T("IBM EBCDIC France");
		break;
	case 20405:
		stCodePageName = _T("Czech (Programmers)");
		break;
	case 20408:
		stCodePageName = _T("Greek Latin IBM 220");
		break;
	case 20409:
		stCodePageName = _T("United States-Dvorak");
		break;
	case 20420:
		stCodePageName = _T("IBM EBCDIC Arabic");
		break;
	case 20423:
		stCodePageName = _T("IBM EBCDIC Greek");
		break;
	case 20833:
		stCodePageName = _T("Korean (IBM EBCDIC)");
		break;
	case 20838:
		stCodePageName = _T("IBM EBCDIC Thai");
		break;
	case 20866:
		stCodePageName = _T("Russian - KOI8-R");
		break;
	case 20871:
		stCodePageName = _T("IBM EBCDIC Icelandic");
		break;
	case 20880:
		stCodePageName = _T("IBM EBCDIC Cyrillic");
		break;
	case 20905:
		stCodePageName = _T("IBM EBCDIC Turkish");
		break;
	case 21025:
		stCodePageName = _T("IBM EBCDIC Cyrillic");
		break;
	case 21027:
		stCodePageName = _T("Japanese");
		break;
	case 21866:
		stCodePageName = _T("Ukranian - KOI8-RU");
		break;
	case 21591:
		stCodePageName = _T("ISO 8859-1 Western");
		break;
	case 21592:
		stCodePageName = _T("ISO 8859-2 Eastern Europe");
		break;
	case 28593:
		stCodePageName = _T("ISO 8859-3 Turkish");
		break;
	case 28594:
		stCodePageName = _T("ISO 8859-4 Baltic");
		break;
	case 28595:
		stCodePageName = _T("ISO 8859-5 Cyrillic");
		break;
	case 28596:
		stCodePageName = _T("ISO 8859-6 Arabic");
		break;
	case 28597:
		stCodePageName = _T("ISO 8859-7 Greek");
		break;
	case 28598:
		stCodePageName = _T("ISO 8859-8 Hebrew");
		break;
	case 28599:
		stCodePageName = _T("ISO 8859-9");
		break;
	case 30408:
		stCodePageName = _T("Greek IBM 319");
		break;
	case 30409:
		stCodePageName = _T("United States-LH Dvorak");
		break;
	case 40408:
		stCodePageName = _T("Greek Latin IBM 319");
		break;
	case 40409:
		stCodePageName = _T("United States-RH Dvorak");
		break;
	case 50220:
		stCodePageName = _T("Japanese (JIS)");
		break;
	case 50221:
		stCodePageName = _T("Japanese (JIS)");
		break;
	case 50222:
		stCodePageName = _T("Japanese (JIS)");
		break;
	case 50225:
		stCodePageName = _T("Korean");
		break;
	case 50408:
		stCodePageName = _T("Greek Latin");
		break;
	case 50932:
		stCodePageName = _T("Japanese");
		break;
	case 50949:
		stCodePageName = _T("Korean");
		break;
	case 51932:
		stCodePageName = _T("Japanese");
		break;
	case 51949:
		stCodePageName = _T("Korean");
		break;
	case 52936:
		stCodePageName = _T("Simplified Chinese");
		break;
	case 65000:
		stCodePageName = _T("Unicode UTF-7");
		break;
	case 65001:
		stCodePageName = _T("Unicode UTF-8");
		break;
	default:
		stCodePageName = _T("");
		break;
	}
	return;
}

static CStdString strCodePages;

BOOL CALLBACK AnalyseCodePagesProc (LPTSTR lpCodePageString)
{
	CStdString chLangName;
	DWORD dCodePage = _tcstol(lpCodePageString, NULL, 10);
	GetCodePageName(dCodePage, chLangName);
	strCodePages.AppendFormat(_T("\t%d = %s\n"), dCodePage, (LPCTSTR) chLangName);

	return TRUE;
}

bool DisplayLocaleInfo (CStdString &out)
{
	TCHAR	LangName[MAX_PATH];

	out += _T("System default language:\n\t");
	if (GetLocaleInfo (LOCALE_SYSTEM_DEFAULT, LOCALE_SENGLANGUAGE, LangName, MAX_PATH) == 0)
		out += _T("Unknown");
	else
		out += LangName;

	out += _T("\n");

	bool	bGet=false;
	HKEY	hKey=NULL;
	DWORD	Size=MAX_PATH;
	DWORD	ValType=REG_SZ;
	LCID	OSLocaleleID;

	try
	{
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
				HMODULE hKernel=LoadLibrary("kernel32.dll");

				if (hKernel)
				{
					pfnGetUserDefaultUILanguageType	pfnGetUserDefaultUILanguage=NULL;
					pfnGetUserDefaultUILanguage=(pfnGetUserDefaultUILanguageType)GetProcAddress(hKernel,"GetUserDefaultUILanguage");
					if (pfnGetUserDefaultUILanguage!=NULL)
					{
						bGet=true;
						OSLocaleleID=pfnGetUserDefaultUILanguage();
					}

					FreeLibrary (hKernel);
				}
			}
		}
		if (bGet)
		{
			if (GetLocaleInfo(OSLocaleleID,LOCALE_SENGLANGUAGE,LangName,MAX_PATH)!=0) bGet=true;
		}
		if (!bGet) strcpy(LangName,"Unknown");

		out += _T("Default interface language of user:\n\t");
		out += LangName;
		out += _T("\n");
	}
	catch (...)
	{
	}

	if (hKey!=NULL)
		RegCloseKey(hKey);

	DWORD	i;
	pHKL	lpList=NULL;

	try
	{
		Size=GetKeyboardLayoutList(0,(HKL*)lpList);
		if (Size!=0)
		{
			lpList=(pHKL)malloc(Size*sizeof(HKL));
			if ((UINT)GetKeyboardLayoutList(Size,(HKL*)lpList)==Size)
			{
				out += "Keyboard layout languages:\n";
				for (i=0;i<Size;i++)
				{
					LangName[0]=0;
					LANGID langID=LOWORD((DWORD)(lpList->lpListItem[i]));
					LCID LocaleleID=MAKELCID(langID,SORT_DEFAULT);
					if (GetLocaleInfo(LocaleleID,LOCALE_SENGLANGUAGE,LangName,MAX_PATH)!=0) 
					{
						out += "\t";
						out += LangName;
						out += "\n";
					}
				}
			}
		}
	}
	catch (...)
	{
	}

	if (lpList!=NULL) free(lpList);

	out += "Installed code pages:\n";

	try
	{
		strCodePages.Empty ();
		EnumSystemCodePages(AnalyseCodePagesProc, CP_INSTALLED);
		out += strCodePages;
		strCodePages.Empty ();
	}
	catch (...)
	{
	}

	return true;
}

std::string KLUTIL::GetSysInfo ()
{
	CStdString strSysInfo;

    DisplaySystemTime (strSysInfo);

	strSysInfo += _T("\nHardware:\n");
	strSysInfo += SYS_INFO_SEP;

	DisplayProcessor (strSysInfo);

	DisplayBIOSInfo (strSysInfo);

    DisplayMemInfo (strSysInfo);

	strSysInfo += _T("Software:\n");
	strSysInfo += SYS_INFO_SEP;

    DisplaySystemVersion (strSysInfo);

	GetInstalledAppInfo (strSysInfo);

    DisplayIEVersion (strSysInfo);

    DisplayEmailClients (strSysInfo);

	DisplayLocaleInfo (strSysInfo);

	return std::string (strSysInfo);
}

