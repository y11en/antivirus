/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	st_prefix.cpp
 * \author	Andrew Kazachkov
 * \date	29.10.2003 15:36:39
 * \brief	internal support for stress tests
 * 
 */

#ifdef _WIN32
#include <std/win32/klos_win32v40.h>
#endif //_WIN32

#include "avp/avp_data.h"

#include "std/base/klbase.h"
#include "std/base/klbaseqi_imp.h"

#include "std/io/klio.h"
#include "std/err/error.h"
#include "std/err/klerrors.h"
#include "std/conv/klconv.h"

#include "std/stress/st_prefix.h"

#ifdef __unix
#include "std/conf/std_conf.h"
//#include <iostream>
#endif

#define KLCS_MODULENAME L"STPREFIX"

//#include <prss/settingsstorage.h>
//#include "prss/prssconst.h"

using namespace KLSTD;
using namespace KLERR;
using namespace std;

#ifdef _WIN32
    #define KLCS_DEFAULT_SETTINGS_PATHA  "c:\\KlDbgKca"
    #define KLCS_DEFAULT_SETTINGS_PATHW  L"c:\\KlDbgKca"
    #define KLCS_DEFAULT_ETC_PATHA  KLCS_DEFAULT_SETTINGS_PATHA
    #define KLCS_DEFAULT_ETC_PATHW  KLCS_DEFAULT_SETTINGS_PATHW
#endif //_WIN32

#ifdef __unix__
#ifdef linux
    #define KLCS_DEFAULT_SETTINGS_PATHA  "/var/opt/kaspersky/klnagent"
    #define KLCS_DEFAULT_SETTINGS_PATHW  L"/var/opt/kaspersky/klnagent"
#else
    #define KLCS_DEFAULT_SETTINGS_PATHA  "/var/db/kaspersky/klnagent"
    #define KLCS_DEFAULT_SETTINGS_PATHW  L"/var/db/kaspersky/klnagent"
#endif
    #define KLCS_DEFAULT_ETC_PATHA  "/etc/opt/kaspersky/klnagent"
    #define KLCS_DEFAULT_ETC_PATHW  L"/etc/opt/kaspersky/klnagent"
#endif //__unix__

#ifdef N_PLAT_NLM
//#define KLCS_DEFAULT_SETTINGS_PATHA  "sys:/temp/adminkit/KlDbgKca"
//#define KLCS_DEFAULT_SETTINGS_PATHW  L"sys:/temp/adminkit/KlDbgKca"
#define KLCS_DEFAULT_SETTINGS_PATHA  "KlDbgKca"
#define KLCS_DEFAULT_SETTINGS_PATHW  L"KlDbgKca"
#include <wcharcrt.h>
#endif //N_PLAT_NLM

#define KLCS_SUPPORT_ST_PREFIXES

#ifdef KLCS_SUPPORT_ST_PREFIXES

#define KLSTD_ST_CMDLINE_PREFIXA "--stp"
#define KLSTD_ST_CMDLINE_PREFIXW L"--stp"

#define KLSTD_ST_PREFIXA "_"
#define KLSTD_ST_PREFIXW L"_"

namespace
{
    std::wstring g_wstrPrefix;
    std::string g_astrPrefix;
};

#endif //KLCS_SUPPORT_ST_PREFIXES

namespace
{   
    std::wstring    g_wstrSettingsRoot = KLCS_DEFAULT_SETTINGS_PATHW;
    std::string     g_strSettingsRoot = KLCS_DEFAULT_SETTINGS_PATHA;
    std::wstring    g_wstrEtcRoot = KLCS_DEFAULT_ETC_PATHW;
    std::string     g_strEtcRoot = KLCS_DEFAULT_ETC_PATHA;
};

namespace KLSTD
{
    int inline StCompare(const char* x1, const char* x2){
        return strcmp(x1, x2);
    };

    int inline StCompare(const wchar_t* x1, const wchar_t* x2){
        return wcscmp(x1, x2);
    };

    template<class T>
        static int StFind(T** argv, const T* name)
    {
        if(!(*argv))return -1;
        for(int i=1; argv[i]; ++i)
        {
            if(StCompare(name, argv[i]) == 0)
                return i+1;
        };
        return -1;
    };

    template<class T>
        static basic_string<T> StParse(T** argv, const T* name)
    {
#ifdef N_PLAT_NLM
        int x=StFind<T>(argv, name);
#else
        int x=StFind(argv, name);
#endif
        if(x < 0)
            return basic_string<T>();
        return argv[x] ? basic_string<T>(argv[x]) : basic_string<T>();
    };
};

void _InitPaths()
{    
    const char* szVal = getenv(KLCS_ENV_ROOT_PATH);
    if(szVal && szVal[0])
    {
        g_wstrEtcRoot = g_wstrSettingsRoot = KLSTD_A2CW2(szVal);
        g_strEtcRoot = g_strSettingsRoot = szVal;
    };
};

KLCSC_DECL void KLSTD_StParseCommandineW(wchar_t** argv)
{
#ifdef KLCS_SUPPORT_ST_PREFIXES
    g_wstrPrefix=StParse(argv, KLSTD_ST_CMDLINE_PREFIXW);
    KLSTD_USES_CONVERSION;
    g_astrPrefix=KLSTD_W2CA(g_wstrPrefix.c_str());
#endif
    _InitPaths();
};

KLCSC_DECL void KLSTD_StParseCommandineA(char** argv)
{
#ifdef KLCS_SUPPORT_ST_PREFIXES
    g_astrPrefix=StParse(argv, KLSTD_ST_CMDLINE_PREFIXA);
    g_wstrPrefix = (const wchar_t*)KLSTD_A2CW2(g_astrPrefix.c_str());
#endif
    _InitPaths();
};

KLCSC_DECL wstring KLSTD_StGetPrefixW()
{
#ifdef KLCS_SUPPORT_ST_PREFIXES
    if(!g_wstrPrefix.empty())
        return KLSTD_ST_PREFIXW + g_wstrPrefix;
    return L"";
#else
    return L"";
#endif
};

KLCSC_DECL string KLSTD_StGetPrefixA()
{
#ifdef KLCS_SUPPORT_ST_PREFIXES
    if(!g_astrPrefix.empty())
        return KLSTD_ST_PREFIXA + g_astrPrefix;
    return "";
#else
    return "";
#endif
};

KLCSC_DECL std::wstring KLSTD_StGetDefDirW()
{
#ifdef _WIN32
    return g_wstrSettingsRoot+ KLSTD_StGetPrefixW() + L"\\";
#else
 #ifdef N_PLAT_NLM
#error "initNovellPath is not thread-safe !!!"
	return std::wstring(initNovellPath(g_wstrSettingsRoot))+ KLSTD_StGetPrefixW()+ L"\\";
 #else
	return g_wstrSettingsRoot+ KLSTD_StGetPrefixW() + L"/";
 #endif
#endif
};

KLCSC_DECL std::string KLSTD_StGetDefDirA()
{
#ifdef _WIN32
    return g_strSettingsRoot + KLSTD_StGetPrefixA() + "\\";
#else
 #ifdef N_PLAT_NLM
	char path[256];
    #error "initNovellPath is not thread-safe !!!"
	wcstombs(path,initNovellPath(g_wstrSettingsRoot),256);
	return std::string(path) + KLSTD_StGetPrefixA() + "\\";
 #else
	return std::string(g_strSettingsRoot) + KLSTD_StGetPrefixA() + "/";
 #endif
#endif
};

KLCSC_DECL std::wstring KLSTD_StGetEtcDefDirW()
{
#ifdef _WIN32
    return g_wstrEtcRoot+ KLSTD_StGetPrefixW() + L"\\";
#else
 #ifdef N_PLAT_NLM
#error "initNovellPath is not thread-safe !!!"
	return std::wstring(initNovellPath(g_wstrEtcRoot))+ KLSTD_StGetPrefixW()+ L"\\";
 #else
	return g_wstrEtcRoot+ KLSTD_StGetPrefixW() + L"/";
 #endif
#endif
};

KLCSC_DECL std::string KLSTD_StGetEtcDefDirA()
{
#ifdef _WIN32
    return g_strEtcRoot + KLSTD_StGetPrefixA() + "\\";
#else
 #ifdef N_PLAT_NLM
	char path[256];
    #error "initNovellPath is not thread-safe !!!"
	wcstombs(path,initNovellPath(g_strEtcRoot),256);
	return std::string(path) + KLSTD_StGetPrefixA() + "\\";
 #else
	return std::string(g_strEtcRoot) + KLSTD_StGetPrefixA() + "/";
 #endif
#endif
};


#ifdef _WIN32

static std::basic_string<TCHAR> StGetRootKey()
{
    TCHAR szBuffer[MAX_PATH]=_T("");
	std::basic_string<TCHAR> strKey;
    strKey.reserve(256);
    strKey=KLSTD::c_szRegKey_Components;
	strKey+=_ltot(AVP_CID_ADMINKIT, szBuffer, 16);
    strKey+=KLSTD_StGetPrefix();
    return strKey;
};

KLCSC_DECL std::wstring KLSTD_StGetRootKeyW()
{
    return std::wstring((const wchar_t*)KLSTD_T2CW2(StGetRootKey().c_str()));
};

KLCSC_DECL std::string KLSTD_StGetRootKeyA()
{
    return std::string((const char*)KLSTD_T2CA2(StGetRootKey().c_str()));
};

KLCSC_DECL std::wstring KLSTD_StGetProductKeyW(const wchar_t* product, const wchar_t* version)
{	
    return KLSTD_StGetRootKeyW() + L"\\" + product + L"\\" + version;
};

KLCSC_DECL std::string KLSTD_StGetProductKeyA(const wchar_t* product, const wchar_t* version)
{
    return KLSTD_StGetRootKeyA() + "\\" + (const char*)KLSTD_W2CA2(product) + "\\" + (const char*)KLSTD_W2CA2(version);
};

#else // _WIN32

KLCSC_DECL std::wstring KLSTD_StGetProductKeyW(const wchar_t* product, const wchar_t* version)
{	
	return L"";
}

#endif

KLCSC_DECL std::wstring KLSTD_StGetCmdlnPostfixW()
{
#ifdef KLCS_SUPPORT_ST_PREFIXES
    if(g_wstrPrefix.empty())
        return L"";
    return std::wstring(L" ") + KLSTD_ST_CMDLINE_PREFIXW + L" " + g_wstrPrefix;
#else
    return L"";
#endif
}

KLCSC_DECL std::string KLSTD_StGetCmdlnPostfixA()
{
#ifdef KLCS_SUPPORT_ST_PREFIXES
    if(g_astrPrefix.empty())
        return "";
    return std::string(" ") + KLSTD_ST_CMDLINE_PREFIXA + " " + g_astrPrefix;
#else
    return "";
#endif
}

//#pragma message("Fix me!!!")
/*KLCSC_DECL void StCallRegProduct(bool bForce)
{
    KLSTD_USES_CONVERSION;
    bool bRegRequired=false;
    if(bForce)
        bRegRequired=true;
    else
    {
        CRegKey key;
        long lResult=key.Open(
                        HKEY_LOCAL_MACHINE,
                        KLSTD_StGetRootKey().c_str(),
                        KEY_READ);
        if(lResult == ERROR_FILE_NOT_FOUND)
            bRegRequired=true;
        else if(lResult)
            KLSTD_THROW_LASTERROR_CODE2(lResult);
        if(!bRegRequired)        
            bRegRequired=(_taccess(KLSTD_StGetDefDir().c_str(), 00) == -1);
    };
    if(bRegRequired)
    {
        const std::wstring& wstrRoot=KLSTD_StGetDefDirW();
        KLSTD_CreatePath(wstrRoot.c_str());
        
        {
            std::wstring wstrSsSettings, wstrSsProdinfo, wstrSsRuntime;
            KLSTD_MakePath(wstrRoot, KLPRSS::c_szwSST_HostSS, KLPRSS::c_szwFileExt_Data, wstrSsSettings);
            KLSTD_MakePath(wstrRoot, KLPRSS::c_szwSST_ProdinfoSS, KLPRSS::c_szwFileExt_Data, wstrSsProdinfo);
            KLSTD_MakePath(wstrRoot, KLPRSS::c_szwSST_RuntimeSS, KLPRSS::c_szwFileExt_Data, wstrSsRuntime);

            CRegKey key;
            long lResult;
            lResult=key.Create(
                            HKEY_LOCAL_MACHINE,
                            KLSTD_StGetRootKey().c_str(),
                            NULL,
                            REG_OPTION_NON_VOLATILE,
                            KEY_WRITE);
            if(lResult)KLSTD_THROW_LASTERROR_CODE2(lResult);
            ;
            key.SetValue(KLSTD_W2CT(wstrSsSettings.c_str()), KLSTD_W2CT(KLPRSS::c_szwSST_HostSS));
            key.SetValue(KLSTD_W2CT(wstrSsProdinfo.c_str()), KLSTD_W2CT(KLPRSS::c_szwSST_ProdinfoSS));
            key.SetValue(KLSTD_W2CT(wstrSsRuntime.c_str()), KLSTD_W2CT(KLPRSS::c_szwSST_RuntimeSS));
            ;
            CAutoPtr<File> pFile;
            KLSTD_CreateFile(wstrSsSettings, 0, KLSTD::CF_CREATE_ALWAYS, KLSTD::AF_WRITE, 0, &pFile);
            pFile=NULL;
            KLSTD_CreateFile(wstrSsProdinfo, 0, KLSTD::CF_CREATE_ALWAYS, KLSTD::AF_WRITE, 0, &pFile);
            pFile=NULL;
            KLSTD_CreateFile(wstrSsRuntime, 0, KLSTD::CF_CREATE_ALWAYS, KLSTD::AF_WRITE, 0, &pFile);
            pFile=NULL;
        };
        {
            CRegKey key;
            long lResult;
            lResult=key.Create(
                            HKEY_LOCAL_MACHINE,
                            KLSTD_StGetProductKey(KLPRSS_PRODUCT_CORE, KLPRSS_VERSION_INDEPENDENT).c_str(),
                            NULL,
                            REG_OPTION_NON_VOLATILE,
                            KEY_WRITE);
            if(lResult)KLSTD_THROW_LASTERROR_CODE2(lResult);
            ;
            std::wstring wstrPolicy, wstrTasks;
            KLSTD_MakePath(wstrRoot, L"unipl", KLPRSS::c_szwFileExt_Data, wstrPolicy);
            KLSTD_MakePath(wstrRoot, L"unitsk", L"", wstrTasks);
            key.SetValue(KLSTD_W2CT(wstrPolicy.c_str()), KLSTD_W2CT(KLPRSS::c_szwSST_HostSP));
            key.SetValue(KLSTD_W2CT(wstrTasks.c_str()), KLSTD_W2CT(KLPRSS::c_szwSST_TasksSS));
            ;
            KLSTD_CreatePath(wstrTasks.c_str());
            CAutoPtr<File> pFile;
            KLSTD_CreateFile(wstrPolicy, 0, KLSTD::CF_CREATE_ALWAYS, KLSTD::AF_WRITE, 0, &pFile);
            pFile=NULL;
        };
    };

}*/

#ifdef __unix
#define _stricmp strcasecmp 
#ifdef __FreeBSD__
#include "std/conv/wcharcrt.h"
#else
#define wcsicmp wcscasecmp 
#endif
typedef wchar_t TCHAR;
#endif

static int find_argument(const char* token, char** argv)
{
	for(int i=0; argv[i]; ++i)
	{		
#ifdef N_PLAT_NLM
		if((argv[i][0]=='-' || argv[i][0]=='/') && stricmp(&argv[i][1], token)==0)
			return i;
#else
		if((argv[i][0]=='-' || argv[i][0]=='/') && _stricmp(&argv[i][1], token)==0)
			return i;
#endif
	};
	return -1;
};

static int find_argument(const wchar_t* token, wchar_t** argv)
{
	for(int i=0; argv[i]; ++i)
	{		
		if((argv[i][0]==L'-' || argv[i][0]==L'/') && wcsicmp(&argv[i][1], token)==0)
			return i;
	};
	return -1;
};

#ifdef _WIN32
static long GetRegKey(
        const wchar_t*  szwProduct,
        const wchar_t*  szwVersion,
        CRegKey&        key)
{
    KLSTD_USES_CONVERSION;
    std::basic_string<TCHAR> keyname=KLSTD_StGetRootKey() + _SEP_;
    keyname+=KLSTD_W2CT(szwProduct);
    keyname+=_SEP_;
    keyname+=KLSTD_W2CT(szwVersion);
    keyname+=_SEP_;
    keyname+=_T("Debug");
    return key.Open(HKEY_LOCAL_MACHINE, keyname.c_str(), KEY_READ);
}
#endif

#ifdef __unix
namespace 
{
    const wchar_t c_szwTraceFile[] = L"TraceFile";
    const wchar_t c_szwTraceLevel[] = L"TraceLevel";

    class CReadConfigFilter
        :   public KLSTD::KLBaseImpl<KLSTD::ReadConfigFilter>
    {
    public:
        KLSTD_INTERAFCE_MAP_BEGIN(KLSTD::ReadConfigFilter)
        KLSTD_INTERAFCE_MAP_END()

        CReadConfigFilter(const wchar_t* szwSectionName)
            :   m_nTraceLevel(0)
            ,   m_wstrSectionName(szwSectionName)
            ,   m_bTraceFile(false)
            ,   m_bTraceLevel(false)
        {
            ;
        };

        virtual ~CReadConfigFilter()
        {
            ;
        };
        
        virtual bool Filter(
                        const wchar_t*  szwSection,
                        const wchar_t*  szwVariable,
                        const wchar_t*  szwValue,
                        KLPAR::Value**  ppValue)
        {
            KLSTD_CHKOUTPTR(ppValue);
            KLSTD::CAutoPtr<KLPAR::Value> pResult;
            /*
            std::wcout 
                    << L"szwSection=" 
                    << szwSection 
                    <<L", "
                    << L"szwVariable="
                    << szwValue
                    << std::endl;
            */
            if(wcsicmp(m_wstrSectionName.c_str(), szwSection) == 0)
            {
                if(wcsicmp(szwVariable, c_szwTraceLevel) == 0)
                {
                    wchar_t* pEnd = NULL;
                    long lResult = wcstol(szwValue, &pEnd, 10);
                    //KLPAR::CreateValue(lResult, (KLPAR::IntValue**)&pResult);
                    m_nTraceLevel = lResult;
                    m_bTraceLevel = true;
                }
                else if(wcsicmp(szwVariable, c_szwTraceFile) == 0)
                {                
                    //KLPAR::CreateValue(szwValue, (KLPAR::StringValue**)&pResult);
                    m_wstrTraceFile = szwValue;
                    m_bTraceFile = true;
                };
            };
            pResult.CopyTo(ppValue);
            return pResult != NULL;
        };
    public:
        std::wstring    m_wstrTraceFile;
        long            m_nTraceLevel;
        bool            m_bTraceFile, m_bTraceLevel;
    protected:
        std::wstring    m_wstrSectionName;
    };
};
#endif

static void GetTraceInfo(
                        int             argcA,
                        char**          argvA,
                        int             argcW,
                        wchar_t**       argvW,
                        const wchar_t*  szwProduct,
                        const wchar_t*  szwVersion,
                        std::wstring&   wstrTraceFile,
                        long&           nTracelevel)
{
    if(!szwProduct || !szwProduct[0] || !szwVersion || !szwVersion[0])
        return;
    KLSTD_USES_CONVERSION;
    std::wstring   wstrTempTraceFile;
    long            nTempTracelevel=0;

    int nPos;
    ;
    if(argvA && argcA)
    {
        nPos=find_argument("tl", argvA);
        if(nPos != -1 && argvA[nPos+1])
            nTempTracelevel=strtol(argvA[nPos+1], NULL, 0);
        ;
        nPos=find_argument("tf", argvA);
        if(nPos != -1 && argvA[nPos+1])
            wstrTempTraceFile=KLSTD_A2CW(argvA[nPos+1]);
    }
    else
    if(argvW && argcW)
    {
        nPos=find_argument(L"tl", argvW);
        if(nPos != -1 && argvW[nPos+1])
            nTempTracelevel=wcstol(argvW[nPos+1], NULL, 0);
        ;
        nPos=find_argument(L"tf", argvW);
        if(nPos != -1 && argvW[nPos+1])
            wstrTempTraceFile=argvW[nPos+1];
    };
    ;
#ifdef _WIN32
    CRegKey key;    
    long lResult=GetRegKey(szwProduct, szwVersion, key);
    if(lResult == NO_ERROR)
    {
        DWORD dwTraceLevel=0;
        if(key.QueryValue(dwTraceLevel, _T("TraceLevel")) == NO_ERROR)
            nTempTracelevel=dwTraceLevel;
        ;
        TCHAR szFileBuffer[2*MAX_PATH]=_T("");
        DWORD dwFileBuffer=KLSTD_COUNTOF(szFileBuffer)-1;
        if(key.QueryValue(szFileBuffer, _T("TraceFile"), &dwFileBuffer) == NO_ERROR)
            wstrTempTraceFile=KLSTD_T2CW(szFileBuffer);
    };
#else
    KLSTD_Initialize();
    KLPAR_Initialize();
    KLERR_BEGIN
        const std::wstring& wstrFileName = KLSTD_StGetEtcDefDirW() + L"debug.conf";
        if(KLSTD_IfExists(wstrFileName.c_str()))
        {
            //std::wcout << L"File exists" << std::endl;
            const std::wstring& wstrSectionName = 
                std::wstring(L"_") + szwProduct;// + L"_" + szwVersion;
            const wchar_t* c_pSections[] = 
            {
                wstrSectionName.c_str(),
                NULL
            };
            //std::wcout << L"wstrSectionName=" << wstrSectionName << std::endl;
            KLSTD::CAutoPtr<KLSTD::ReadConfigFilter> pFilter;
            CReadConfigFilter* p = NULL;
            pFilter.Attach((p=new CReadConfigFilter(wstrSectionName.c_str())));
            KLSTD_CHKMEM(p);
            KLPAR::ParamsPtr pData;
            KLSTD_ReadConfig(
                        wstrFileName.c_str(), 
                        NULL,//c_pSections, 
                        pFilter, 
                        &pData);
            if(p->m_bTraceLevel)
                nTempTracelevel = p->m_nTraceLevel;
            if(p->m_bTraceFile)
                wstrTempTraceFile = p->m_wstrTraceFile;
        };
    KLERR_ENDT(1)
    KLPAR_Deinitialize();
    KLSTD_Deinitialize();
#endif

    if(nTempTracelevel < 0)
	    nTempTracelevel = 0;
    if(nTempTracelevel > 5)
	    nTempTracelevel = 5;
    if(nTempTracelevel != 0 && wstrTempTraceFile.empty())
    {
#ifdef _WIN32
        TCHAR szModuleName[MAX_PATH*2]=_T("");
        if(GetModuleFileName(NULL, szModuleName, KLSTD_COUNTOF(szModuleName)-1))
        {
            TCHAR drive[_MAX_DRIVE+1]=_T("");
            TCHAR dir[_MAX_DIR+1]=_T("");
            TCHAR fname[_MAX_FNAME]=_T("");           
            _tsplitpath(szModuleName, drive, dir, fname, NULL);
            basic_string<TCHAR> strFileName;
            strFileName=_T("$");
            strFileName+=fname;
            strFileName+=KLSTD_StGetPrefix();           
            strFileName+=_T("-");
            strFileName+=KLSTD_W2CT(szwProduct);
            _tmakepath(szModuleName, drive, dir, strFileName.c_str(), _T(".log"));
            wstrTempTraceFile=KLSTD_T2CW(szModuleName);
        }
        else
            nTempTracelevel=0;
#else
        std::wstring wstrDir, wstrName, wstrExt;
        if(argvA && argvA[0] && argvA[0][0])
        {
            KLSTD_SplitPath(
                    (const wchar_t*)KLSTD_A2CW2(argvA[0]), 
                    wstrDir, 
                    wstrName, 
                    wstrExt);
        }
        else if(argvW && argvW[0] && argvW[0][0])
        {
            KLSTD_SplitPath(argvW[0], wstrDir, wstrName, wstrExt);
        };
        if(!wstrName.empty())
        {  
           std::wstring strFileName = L"$";
           strFileName += wstrName;
           strFileName += KLSTD_StGetPrefix();
           strFileName += L"-";
           strFileName += szwProduct;
           wstrTraceFile.clear();
           KLSTD_MakePath(wstrDir, strFileName, L".log", wstrTraceFile);
        };
#endif
    };

    if(nTempTracelevel == 0)
        wstrTempTraceFile.clear();// = L"";

    if(!wstrTempTraceFile.empty())
        wstrTraceFile=wstrTempTraceFile;

    if(nTempTracelevel != 0)
        nTracelevel = nTempTracelevel;

}

KLCSC_DECL void KLSTD_GetTraceInfoA(
                        int             argc,
                        char**          argv,
                        const wchar_t*  szwProduct,
                        const wchar_t*  szwVersion,
                        std::wstring&   wstrTraceFile,
                        long&           nTracelevel)
{
    GetTraceInfo(
                argc,
                argv,
                0,
                NULL,
                szwProduct,
                szwVersion,
                wstrTraceFile,
                nTracelevel);
}

KLCSC_DECL void KLSTD_GetTraceInfoW(
                        int             argc,
                        wchar_t**       argv,
                        const wchar_t*  szwProduct,
                        const wchar_t*  szwVersion,
                        std::wstring&   wstrTraceFile,
                        long&           nTracelevel)
{
    GetTraceInfo(
                0,
                NULL,
                argc,
                argv,
                szwProduct,
                szwVersion,
                wstrTraceFile,
                nTracelevel);
}

static void GetDiagOptions(
                        int             argcA,
                        char**          argvA,
                        int             argcW,
                        wchar_t**       argvW,
                        const wchar_t*  szwProduct,
                        const wchar_t*  szwVersion,
                        AVP_dword&      dwExcptFlags)
{
    if(!szwProduct || !szwProduct[0] || !szwVersion || !szwVersion[0])
        return;
    KLSTD_USES_CONVERSION;
    AVP_dword   dwTempExcptFlags = dwExcptFlags;

    int nPos;
    ;
    if(argvA && argcA)
    {
        nPos=find_argument("df", argvA);
        if(nPos != -1 && argvA[nPos+1])
            dwTempExcptFlags=strtoul(argvA[nPos+1], NULL, 0);
    }
    else
    if(argvW && argcW)
    {
        nPos=find_argument(L"df", argvW);
        if(nPos != -1 && argvW[nPos+1])
            dwTempExcptFlags=wcstoul(argvW[nPos+1], NULL, 0);
    };
    ;
#ifdef _WIN32
    CRegKey key;
    long lResult=GetRegKey(szwProduct, szwVersion, key);
    if(lResult == NO_ERROR)
    {
        DWORD dwExcptFlags2=0;
        if(key.QueryValue(dwExcptFlags2, _T("ExcptFlags")) == NO_ERROR)
            dwTempExcptFlags=dwExcptFlags2;
    };
#endif
    dwExcptFlags = dwTempExcptFlags;

}
KLCSC_DECL void KLSTD_GetDiagOptionsA(
                        int             argc,
                        char**          argv,
                        const wchar_t*  szwProduct,
                        const wchar_t*  szwVersion,
                        AVP_dword&      dwExcptFlags)
{
    GetDiagOptions(
                argc,
                argv,
                0,
                NULL,
                szwProduct,
                szwVersion,
                dwExcptFlags);
}


KLCSC_DECL void KLSTD_GetDiagOptionsW(
                        int             argc,
                        wchar_t**       argv,
                        const wchar_t*  szwProduct,
                        const wchar_t*  szwVersion,
                        AVP_dword&      dwExcptFlags)
{
    GetDiagOptions(
                0,
                NULL,
                argc,
                argv,
                szwProduct,
                szwVersion,
                dwExcptFlags);
}
                        
