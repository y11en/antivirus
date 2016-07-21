#include <std/win32/klos_win32v50.h>
#include <kca/acdt/acdt_lib.h>
#include <std/par/par_conv.h>
#include <std/stress/st_prefix.h>
#include <common/measurer.h>
#include <std/win32/klos_win32_errors.h>
#include <std/hstd/hostdomain.h>
#include <kca/prss/settingsstorage.h>
#include <kca/prss/helpersi.h>
//#include <Msi.h>
//#include <delayimp.h>

#include <common/klregistry.h>

#include <sstream>
#include <set>

#include "../apps/klbackup/klbkutils.h"

#include <common/msi_wrappers.h>

#define KLCS_MODULENAME L"KLACDTLIB"

using namespace KLACDTAPI;
using namespace KLSTD;


namespace
{

    prodinfo_t products[] = 
    {   //@todo update L""
        //@todo add wks 46 
        {c_szwProductNagent, KLACDT_NAGENT_PRODNAME, KLACDT_NAGENT_PRODVER, KLACDT_NAGENT_UPGRCODE, L"Kaspersky Network Agent", L"klnagent"},
        {c_szwProductNagent, KLACDT_NAGENT_PRODNAME, KLACDT_NAGENT_PRODVER, KLACDT_NAGENT_MSI_UPGRCODE, L"Kaspersky Network Agent", L"klnagent"},
        {c_szwProductAdmServer, KLACDT_AK_PRODNAME, KLACDT_AK_PRODVER, KLACDT_AK_UPGRCODE, L"Kaspersky Administration Server", L"CSAdminServer"},
        {c_szwProductWks50, KLACDT_WORKSTATION_PRODNAME, KLACDT_WORKSTATION_PRODVER, KLACDT_WORKSTATION_UPGRCODE, L"Kaspersky Anti-Virus 5.0 for Workstations", L"KLBLMain"},
        {c_szwProductFs50, KLACDT_FILESERVER_PRODNAME, KLACDT_FILESERVER_PRODVER, KLACDT_FILESERVER_UPGRCODE, L"Kaspersky Anti-Virus 5.0 for File Servers", L"klfsblogic"},
        {c_szwProductWks60, KLACDT_WORKSTATION6_PRODNAME, KLACDT_WORKSTATION6_PRODVER, KLACDT_WORKSTATION6_UPGRCODE, L"Kaspersky Anti-Virus 6.0 for Workstations", L"AVP"},
        {c_szwProductWks60, KLACDT_WORKSTATION6_PRODNAME, KLACDT_WORKSTATION6_PRODVER, KLACDT_SOS6_UPGRCODE, L"Kaspersky Anti-Virus 6.0 SOS", L"AVP"},
        {c_szwProductFs60, KLACDT_FILESERVER6_PRODNAME, KLACDT_FILESERVER6_PRODVER, KLACDT_FILESERVER6_UPGRCODE, L"Kaspersky Anti-Virus 6.0 for File Servers", L"AVP"},
        {c_szwProductWks46, KLACDT_WORKSTATION_PRODNAME, KLACDT_WORKSTATION_PRODVER, KLACDT_WORKSTATION46_UPGRCODE, L"Kaspersky Anti-Virus 5.0 for Workstations", L"kavsvc"},
        {c_szwProductFsEe60, KLACDT_KAVFSEE_PRODNAME, KLACDT_KAVFSEE_PRODVER, KLACDT_KAVFSEE_UPGRCODE, L"Kaspersky Anti-Virus 6.0 for Windows Servers Enterprise Edition", L"kavfs"}
    };
};


namespace KLACDTAPI
{
    void SetTracingLevelI(
            const full_prdinf_t&       pi,
            int                     nLevel);
    
    void SendProductActionI(
            const full_prdinf_t&    pi, 
            KLACDT_PRODUCT_ACTION   nCode,
            long                    lTimeout);
    
    void ExecuteDiagI(
            const full_prdinf_t&       pi,
            long                       lTimeout);
    
    void GetProductParsI(
            const full_prdinf_t&        pi,
            std::wstring*               pwstrDisplayName,
            std::wstring*               pwstrBuild,
            std::wstring*               pwstrVersion,
            bool*                       pbIsRunning,
            std::vector<std::wstring>*  pvecTraceFiles,
            int*                        pnTraceLevel,
            std::wstring*               pwstrDiagLog,
            std::wstring*               pwstrDiagTrace,
            std::wstring*               pwstrInstallDir,
            AVP_dword*                  pdwProps);

    KLPAR::ParamsPtr GetProductsState_GetProductInfo(
                        const full_prdinf_t&   pi);

    KLPAR::ParamsPtr GetProductsState()
    {
        KLPAR::ParamsPtr pResultProducts;
        KLPAR_CreateParams(&pResultProducts);
        ;
        for(size_t i = 0; i < KLSTD_COUNTOF(products); ++i)
        {
        KLERR_BEGIN
            std::vector<TCHAR> szProductCode;
            szProductCode.resize(256);
            prodinfo_t pi = products[i];
            UINT nResult = KLMSIWRAPPER_MsiEnumRelatedProducts( 
                                    KLSTD_W2CT2(pi.m_szwUpgradeCode),
                                    0,
                                    0,
                                    &szProductCode[0]);

            if( nResult != ERROR_SUCCESS && 
                wcscmp(pi.m_szwUpgradeCode, KLACDT_NAGENT_UPGRCODE) == 0 && 
                KLMSIWRAPPER_MsiEnumRelatedProducts( 
                                    KLSTD_W2CT2(KLACDT_AK_UPGRCODE),
                                    0,
                                    0,
                                    &szProductCode[0]) == ERROR_SUCCESS ) 
            {
                pi.m_szwUpgradeCode = KLACDT_AK_UPGRCODE;
                nResult = ERROR_SUCCESS;
            };
                ;
            if(nResult != ERROR_SUCCESS )
            {
                KLSTD_TRACE2(   1, 
                                L"Failed to get product code from upgrade code %ls. "
                                L"Error code 0x%X.\n",
                                pi.m_szwUpgradeCode,
                                nResult);
            }
            else
            {
                KLSTD_TRACE2(
                        1, 
                        L"Product %ls (%ls) found\n", 
                        pi.m_szwID, 
                        pi.m_szwUpgradeCode);

                full_prdinf_t fpi;
                static_cast<prodinfo_t&>(fpi) = pi;
                fpi.m_wstrProductCode = KLSTD_T2CW2(&szProductCode[0]);
                ;
                KLPAR::ParamsPtr pInfo =  GetProductsState_GetProductInfo(fpi);
                KLSTD::CAutoPtr<KLPAR::ParamsValue> pVal;
                KLPAR::CreateValue(pInfo, &pVal);
                pResultProducts->ReplaceValue(pi.m_szwID, pVal);
            };
        KLERR_ENDT(1)
        };

        KLSTD::CAutoPtr<KLPAR::ArrayValue> p_arrELs;
        std::vector<std::wstring> vecELs;
        vecELs.reserve(10);
        KLERR_TRY
            std::wstring wstrSubKey;            
            KlRegKey key;
	        KLSTD_RGCHK(key.Open(
                        HKEY_LOCAL_MACHINE, 
                        L"SYSTEM\\CurrentControlSet\\Services\\Eventlog", 
                        KEY_READ ));

            for(    size_t index = 0; 
                    key.EnumKey(
                        index, 
                        wstrSubKey, 
		                NULL)==ERROR_SUCCESS; 
                    ++index)
            {
                std::wstring wstrDN = wstrSubKey;
                /*
                KLERR_TRY
                    KlRegKey keyEl;
                    keyEl.Open(key, &szBuffer[0], KEY_READ);
                KLERR_CATCH(pError)
                    ;
                KLERR_FINALLY
                    ;
                KLERR_ENDTRY
                */
                vecELs.push_back(EncodeFilePath(wstrSubKey.c_str(), wstrDN.c_str(), 0));
            };
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
        KLERR_ENDTRY
        KLPAR::CreateStringValueArray(vecELs, &p_arrELs);   
        /*
        {
            std::wstring wstrWorkingFolder;
            {
                std::vector<wchar_t> vecBuffer;
                vecBuffer.resize(MAX_PATH*2+1);
                KLSTD_LECHK(::GetCurrentDirectoryW(vecBuffer.size(), &vecBuffer[0]));
                wstrWorkingFolder = &vecBuffer[0];
            };
            std::wstring wstrMask, wstrEvDir;
            KLSTD_PathAppend(wstrWorkingFolder, KLACDT_EVENT_LOGS, wstrEvDir, true);
            KLSTD_PathAppend(wstrEvDir, L"*.evt", wstrMask, true);
            
            if(KLSTD_IfExists(wstrEvDir.c_str()))
            {
                KLERR_BEGIN
                    std::vector<std::wstring> vecNames;
                    KLSTD_GetFilesByMask(wstrMask, vecNames);
                    for(size_t i = 0; i < vecNames.size(); ++i)
                    {
                        KLSTD_TRACE1(1, L"EventLog '%ls' found\n", vecNames[i].c_str());
                        std::wstring wstrDir, wstrFile, wstrExt;
                        KLSTD_SplitPath(vecNames[i], wstrDir, wstrFile, wstrExt);
                        
                        std::wstring wstrFullName;
                        KLSTD_PathAppend(wstrEvDir, wstrFile + wstrExt, wstrFullName, true);
                        
                        KLSTD::CAutoPtr<KLPAR::StringValue> pStringValue;
                        KLPAR::CreateValue(wstrFullName.c_str(), &pStringValue);

                        KLSTD_TRACE2(1, L"'%ls'='%ls'\n", wstrFile.c_str(), vecNames[i].c_str());
                        pResultELs->ReplaceValue(wstrFile, pStringValue);
                    };
                KLERR_ENDT(1)
            };
        };
        */
        std::wstring wstrHostDN;
        KLERR_BEGIN
            std::wstring    wstrHost;
            std::wstring    wstrDomain;
            KLSTD::KlDomainType nType = KLSTD::KLDT_WIN_WORKGROUP;
            KLSTD_GetHostAndDomainName(wstrHost, &wstrDomain, &nType);
            if(nType != KLSTD::KLDT_WIN_WORKGROUP && !wstrDomain.empty())
            {
                wstrHostDN = wstrDomain + L"\\" + wstrHost;
            }
            else
                wstrHostDN = wstrHost;
        KLERR_ENDT(1)
        
        KLPAR::ParamsPtr pResult;
        KLPAR::param_entry_t par[] = 
        {
            KLPAR::param_entry_t(c_szwStateProducts,    pResultProducts),
            KLPAR::param_entry_t(c_szwStateEventLogs,   p_arrELs),
            KLPAR::param_entry_t(c_szwStateHostDN,      wstrHostDN.c_str())
        };
        KLPAR::CreateParamsBody(par, KLSTD_COUNTOF(par), &pResult);
        return pResult;
    };

    KLPAR::ParamsPtr GetProductsState_GetProductInfo(
                            const full_prdinf_t&   pi)
    {
        KLPAR::ParamsPtr pResult;

        std::wstring    wstrDisplayName;
        std::wstring    wstrBuild;
        std::wstring    wstrVersion;
        bool            bIsRunning = false;
        std::vector<std::wstring> vecTraceFiles;
        //std::wstring    wstrTraceFile;
        int             nTraceLevel = c_nInvalidTraceLevel;
        std::wstring    wstrDiagLog;
        std::wstring    wstrDiagTrace;
        std::wstring    wstrInstPath;
        AVP_dword       dwProps = 0;

        GetProductParsI(
            pi, 
            &wstrDisplayName,
            &wstrBuild,
            &wstrVersion,
            &bIsRunning,
            &vecTraceFiles,
            &nTraceLevel,
            &wstrDiagLog,
            &wstrDiagTrace,
            &wstrInstPath,
            &dwProps);

        for(size_t ii = 0; ii < vecTraceFiles.size(); ++ii)
            vecTraceFiles[ii] = KLACDTAPI::EncodeFilePath2(vecTraceFiles[ii].c_str());
        wstrDiagLog = EncodeFilePath2(wstrDiagLog.c_str());
        wstrDiagTrace = EncodeFilePath2(wstrDiagTrace.c_str());

        KLSTD::CAutoPtr<KLPAR::ArrayValue> p_arrDumps;
        if(!wstrInstPath.empty())
        {
            std::set<std::wstring> setFiles;
            const wchar_t* c_szwMasks[] = {L"dmp*.tmp", L"*.dmp"};
            for(size_t i = 0; i < KLSTD_COUNTOF(c_szwMasks); ++i)
            {
            KLERR_BEGIN            
                std::wstring wstrMask;
                KLSTD_PathAppend(wstrInstPath, c_szwMasks[i], wstrMask, true);
                std::vector<std::wstring> vecNames;
                KLSTD_GetFilesByMask(wstrMask, vecNames);
                setFiles.insert(vecNames.begin(), vecNames.end());
            KLERR_ENDT(1)
            };
            std::vector<std::wstring> vecEncoded;
            vecEncoded.reserve(setFiles.size());
            for(    std::set<std::wstring>::iterator it = setFiles.begin(); 
                    it != setFiles.end(); 
                    ++it)
            {
                std::wstring wstrLocalPath;
                KLSTD_PathAppend(wstrInstPath, *it, wstrLocalPath, true);
                const std::wstring wstrEncoded = EncodeFilePath2(wstrLocalPath.c_str());
                vecEncoded.push_back(wstrEncoded);;                
            };
            KLPAR::CreateStringValueArray(vecEncoded, &p_arrDumps);
        };

        int nTraceMin = c_nInvalidTraceLevel;
        int nTraceMax = c_nInvalidTraceLevel;
        int nTraceDef = c_nInvalidTraceLevel;

        if(    wcscmp(pi.m_szwID, c_szwProductNagent) == 0 || 
               wcscmp(pi.m_szwID, c_szwProductAdmServer) == 0 )
        {
            nTraceMin = 1;
            nTraceMax = 5;
            nTraceDef = 4;
        }
        else if( wcscmp(pi.m_szwID, c_szwProductWks60) == 0 ||
                 wcscmp(pi.m_szwID, c_szwProductFs60) == 0)
        {
            nTraceMin = 0;
            nTraceMax = 1000;
            nTraceDef = 500;
        }
        else if( wcscmp(pi.m_szwID, c_szwProductWks46) == 0)
        {
            nTraceMin = 0;
            nTraceMax = 255;
            nTraceDef = 255;            
        }
        else if( wcscmp(pi.m_szwID, c_szwProductWks50) == 0 ||
                 wcscmp(pi.m_szwID, c_szwProductFs50) == 0)
        {
            nTraceMin = 1;
            nTraceMax = 10;
            nTraceDef = 4;
        }
        //@todo add acquiring traceinfo about other products

        KLSTD::CAutoPtr<KLPAR::ArrayValue> pTraceFiles;
        KLPAR::CreateStringValueArray(vecTraceFiles, &pTraceFiles);

        KLPAR::param_entry_t par[] = 
        {
            KLPAR::param_entry_t(c_szwStateProductDN,       wstrDisplayName.c_str()),
            KLPAR::param_entry_t(c_szwStateProductName,     pi.m_szwProduct),
            KLPAR::param_entry_t(c_szwStateProductVersion,  wstrVersion.c_str()),
            KLPAR::param_entry_t(c_szwStateProductBuild,    wstrBuild.c_str()),
            KLPAR::param_entry_t(c_szwStateProductRunning,  bIsRunning),
            KLPAR::param_entry_t(c_szwStateTraceFile,       pTraceFiles),
            KLPAR::param_entry_t(c_szwStateTraceLevel,      (long)nTraceLevel),
            KLPAR::param_entry_t(c_szwStateDiagLog,         wstrDiagLog.c_str()),
            KLPAR::param_entry_t(c_szwStateDiagTrace,       wstrDiagTrace.c_str()),
            KLPAR::param_entry_t(c_szwStateProdProps,       long(dwProps)),
            KLPAR::param_entry_t(c_szwStateTraceLimit_Min,  (long)nTraceMin),
            KLPAR::param_entry_t(c_szwStateTraceLimit_Max,  (long)nTraceMax),
            KLPAR::param_entry_t(c_szwStateTraceLimit_Default, (long)nTraceDef),
            KLPAR::param_entry_t(c_szwStateProductInstallPath,  wstrInstPath.c_str())
        };
        KLPAR::CreateParamsBody(par, KLSTD_COUNTOF(par), &pResult);
        return pResult;
    };

    
    //HKEY_LOCAL_MACHINE
    #define KLACDTAPI_MAINKEY   L"SOFTWARE\\KasperskyLab\\Components\\34";

    bool GetProductParsI_IsServiceRunning(const wchar_t* szwService, AVP_dword& dwAccepted)
    {    
        KLSTD_CHKINPTR(szwService);
        bool bResult = false;
    #ifdef KLSTD_WINNT
        SC_HANDLE hService = NULL, hScm = NULL;
        KLERR_TRY
            hScm = OpenSCManagerW(
                        NULL,
                        NULL,
                        GENERIC_READ|GENERIC_EXECUTE);
            KLSTD_LECHK(hScm);
            
            hService = ::OpenServiceW(
                            hScm, 
                            szwService, 
                            GENERIC_READ|GENERIC_EXECUTE);
            KLSTD_LECHK(hService);
            
            SERVICE_STATUS ssStatus;
            KLSTD_LECHK(::QueryServiceStatus( hService, &ssStatus) );
            bResult = (SERVICE_STOPPED != ssStatus.dwCurrentState);
            
            dwAccepted |= PP_MAY_BE_STARTED;
            if(ssStatus.dwControlsAccepted & SERVICE_ACCEPT_STOP)
                dwAccepted |= PP_MAY_BE_STOPPED|PP_MAY_BE_RESTARTED;
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
        KLERR_FINALLY
            if(hScm)
            {
                ::CloseServiceHandle(hScm);
                hScm = NULL;
            };
            if(hService)
            {
                ::CloseServiceHandle(hService);
                hService = NULL;
            };
        KLERR_ENDTRY
    #else
        KLSTD_NOTIMP();
    #endif
        return bResult;    
    };

    std::wstring GetServiceName(const full_prdinf_t& pi)
    {
        std::wstring wstrResult = pi.m_szwServiceName;
        return wstrResult;        
    };

    namespace TRC_WKS46
    {
        typedef std::pair<const wchar_t*, const wchar_t*> cmp_name_t;
        const cmp_name_t pNames[] = 
        {
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\101",              L"_kavscan.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\102",              L"_kavmon.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\10a",              L"_kavahckr.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\12b",              L"_kavmchk.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\2c",               L"_kav_sc.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\3",                L"_kavupd.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\7",                L"_kavsvc.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\7\\Communicator",  L"_kavc.log")
        };
        const wchar_t szwValFile[] = L"TraceFile";
        const wchar_t szwValOldFile[] = L"TraceFile_old";
        const wchar_t szwValLevel[] = L"TraceLevel";
    };

    namespace TRC_WKS50
    {
        typedef std::pair<const wchar_t*, const wchar_t*> cmp_name_t;
        const cmp_name_t pNamesWks[] = 
        {
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\2A\\Workstation\\5.0.0.0\\bl", L"_kavbl.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\2A\\Workstation\\5.0.0.0\\p0", L"_kavp0.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\2A\\Workstation\\5.0.0.0\\p1", L"_kavp1.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\2A\\Workstation\\5.0.0.0\\p2", L"_kavp2.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\2A\\Workstation\\5.0.0.0\\p3", L"_kavp3.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\2A\\Workstation\\5.0.0.0\\p4", L"_kavp4.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\2A\\Workstation\\5.0.0.0\\p5", L"_kavp5.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\2A\\Workstation\\5.0.0.0\\p6", L"_kavp6.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\2A\\Workstation\\5.0.0.0\\p7", L"_kavp7.log")
        };

        const size_t c_nNamesWks = KLSTD_COUNTOF(pNamesWks);

        const cmp_name_t pNamesFs[] = 
        {
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\2A\\Fileserver\\5.0.0.0\\bl", L"_kavbl.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\2A\\Fileserver\\5.0.0.0\\p0", L"_kavp0.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\2A\\Fileserver\\5.0.0.0\\p1", L"_kavp1.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\2A\\Fileserver\\5.0.0.0\\p2", L"_kavp2.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\2A\\Fileserver\\5.0.0.0\\p3", L"_kavp3.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\2A\\Fileserver\\5.0.0.0\\p4", L"_kavp4.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\2A\\Fileserver\\5.0.0.0\\p5", L"_kavp5.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\2A\\Fileserver\\5.0.0.0\\p6", L"_kavp6.log"),
            cmp_name_t(L"SOFTWARE\\KasperskyLab\\Components\\2A\\Fileserver\\5.0.0.0\\p7", L"_kavp7.log")
        };

        const size_t c_nNamesFs = KLSTD_COUNTOF(pNamesFs);


        const wchar_t szwValFile[] = L"TraceFile";
        const wchar_t szwValLevel[] = L"TraceLevel";
    };


    void GetProductPars_GetTraceInfoI(
            const full_prdinf_t&        pi,
            const std::wstring&         wstrInstallDir,
            std::vector<std::wstring>&  vecTraceFiles,
            int&                        nTraceLevel)
    {
    KL_TMEASURE_BEGIN(L"GetProductPars_GetTraceInfoI", 3)
        vecTraceFiles.clear();        
        ;
        if( wcscmp(pi.m_szwID, c_szwProductNagent) == 0 || 
            wcscmp(pi.m_szwID, c_szwProductAdmServer) == 0)
        {
            std::wstring wstrTraceFile;
        KLERR_TRY
            KlRegKey keyTrace;
            KLSTD_RGCHK(keyTrace.Open(
                            HKEY_LOCAL_MACHINE,                
                            (KLSTD_StGetProductKeyW(pi.m_szwProduct, pi.m_szwVersion) + L"\\Debug").c_str(),
                            KEY_READ));
            DWORD dwVal = 0;
            keyTrace.QueryValue(dwVal, L"TraceLevel");
            nTraceLevel = dwVal;
            KLSTD_RGCHK(keyTrace.QueryValue(wstrTraceFile, L"TraceFile"));
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
        KLERR_ENDTRY

            if( !wstrInstallDir.empty() && wstrTraceFile.empty())
            {
                KLSTD_TRACE0(1, L"Checking trace file\n");

                std::wstring wstrResult;
                if(wcscmp(pi.m_szwID, c_szwProductNagent) == 0)
                {
                    KLSTD_PathAppend(wstrInstallDir, L"$klnagent-1103.log", wstrResult, true);
                }
                else if(wcscmp(pi.m_szwID, c_szwProductAdmServer) == 0)
                {
                    KLSTD_PathAppend(wstrInstallDir, L"$klserver-1093.log", wstrResult, true);
                };

                KLSTD_TRACE1(1, L"Trace file may be '%ls'\n", wstrResult.c_str());
            
                KLERR_BEGIN
                    if(KLSTD_IfExists(wstrResult.c_str()))
                        wstrTraceFile = wstrResult;
                KLERR_ENDT(1)
            };

            if(!nTraceLevel)
                nTraceLevel = c_nInvalidTraceLevel;

            vecTraceFiles.push_back(wstrTraceFile);
        }
        else if( wcscmp(pi.m_szwID, c_szwProductWks60) == 0 ||
                 wcscmp(pi.m_szwID, c_szwProductFs60) == 0)
        {
            KLERR_TRY
                KlRegKey keyProfile;
                KLSTD_RGCHK(keyProfile.Open(
                                HKEY_LOCAL_MACHINE,                
                                L"SOFTWARE\\KasperskyLab\\AVP6\\Trace",
                                KEY_READ));
                std::wstring wstrProfile;
                KLSTD_RGCHK(keyProfile.QueryValue(wstrProfile, L"Settings"));

                KlRegKey keyTrace;
                KLSTD_RGCHK(keyTrace.Open(
                                HKEY_LOCAL_MACHINE,
                                std::wstring(L"SOFTWARE\\KasperskyLab\\AVP6\\Trace\\" + 
                                        wstrProfile).c_str(),
                                KEY_READ));

                DWORD dwTraceFileEnable = 0;
                keyTrace.QueryValue(dwTraceFileEnable, L"TraceFileEnable");
                if(dwTraceFileEnable)
                {
                    DWORD dwTraceFileMaxLevel = 0;
                    keyTrace.QueryValue(dwTraceFileMaxLevel, L"TraceFileMaxLevel");
                    nTraceLevel = dwTraceFileMaxLevel;
                }
                else
                {
                    nTraceLevel = c_nInvalidTraceLevel;
                };

                KlRegKey keyEnv;
                KLSTD_RGCHK(keyEnv.Open(
                                HKEY_LOCAL_MACHINE,
                                L"SOFTWARE\\KasperskyLab\\AVP6\\environment",
                                KEY_READ));

                std::wstring wstrTraceRoot;
                KLSTD_RGCHK(keyEnv.QueryValue(wstrTraceRoot, L"TraceRoot"));
                std::wstring wstrDataPath = DoExpandEnvStrings(wstrTraceRoot.c_str());

                std::vector<std::wstring> vecNames;
                KLSTD_GetFilesByMask(KLSTD::DoPathAppend(wstrDataPath, L"*"), vecNames);
                vecTraceFiles.reserve(vecNames.size());
                for(size_t jj =0; jj < vecNames.size(); ++jj)
                {                    
                KLERR_BEGIN
                    const std::wstring& wstrFileName = KLSTD::DoPathAppend(wstrDataPath, vecNames[jj]);
                    if(KLSTD_IfExists(wstrFileName.c_str()))
                        vecTraceFiles.push_back(wstrFileName);
                KLERR_ENDT(1)
                };
            KLERR_CATCH(pError)
                KLERR_SAY_FAILURE(4, pError);
            KLERR_ENDTRY
        }
        else if( wcscmp(pi.m_szwID, c_szwProductWks46) == 0 )
        {
            for(size_t ii =0; ii < KLSTD_COUNTOF(TRC_WKS46::pNames); ++ii)
            {
            KLERR_BEGIN
                int nLocalTraceLevel = c_nInvalidTraceLevel;
                KlRegKey keyTrace;
                KLSTD_RGCHK(keyTrace.Open(HKEY_LOCAL_MACHINE, TRC_WKS46::pNames[ii].first, KEY_READ));
                std::wstring wstrFileName, wstrOldFileName;
                DWORD dwTraceLevel = 255;

                keyTrace.QueryValue(dwTraceLevel, TRC_WKS46::szwValLevel);
                keyTrace.QueryValue(wstrFileName, TRC_WKS46::szwValFile);
                keyTrace.QueryValue(wstrOldFileName, TRC_WKS46::szwValOldFile);

                KLSTD_TRACE4(
                        1, 
                        L"RegKey = '%ls', filename='%ls', oldfilename='%ls', level = %u\n", 
                        TRC_WKS46::pNames[ii].first,
                        wstrFileName.c_str(), 
                        wstrOldFileName.c_str(),
                        dwTraceLevel);

                if(wstrFileName.empty())
                {
                    nLocalTraceLevel = c_nInvalidTraceLevel;
                    if( !wstrOldFileName.empty() && 
                        KLSTD_IfExists(wstrOldFileName.c_str()))
                    {
                        vecTraceFiles.push_back(wstrOldFileName);
                    };
                }
                else
                {
                    nLocalTraceLevel = dwTraceLevel;
                    if(KLSTD_IfExists(wstrFileName.c_str()))
                        vecTraceFiles.push_back(wstrFileName);
                };
                if(nTraceLevel != c_nInvalidTraceLevel && nLocalTraceLevel != c_nInvalidTraceLevel)
                    nTraceLevel = std::min(nTraceLevel, nLocalTraceLevel);
                else if(nTraceLevel == c_nInvalidTraceLevel)
                    nTraceLevel = nLocalTraceLevel;
            KLERR_ENDT(1)
            };
        }
        else if(    wcscmp(pi.m_szwID, c_szwProductWks50) == 0 ||
                    wcscmp(pi.m_szwID, c_szwProductFs50) == 0)
        {

            const TRC_WKS50::cmp_name_t* c_pNames = 
                    (wcscmp(pi.m_szwID, c_szwProductWks50) == 0)
                    ?   TRC_WKS50::pNamesWks
                    :   TRC_WKS50::pNamesFs;
            
            const size_t c_nNames = 
                    (wcscmp(pi.m_szwID, c_szwProductWks50) == 0)
                    ?   TRC_WKS50::c_nNamesWks
                    :   TRC_WKS50::c_nNamesFs;

            for(size_t ii =0; ii < c_nNames; ++ii)
            {
            KLERR_BEGIN
                int nLocalTraceLevel = c_nInvalidTraceLevel;
                KlRegKey keyTrace;
                KLSTD_RGCHK(keyTrace.Open(HKEY_LOCAL_MACHINE, c_pNames[ii].first, KEY_READ));
                std::wstring wstrFileName;
                DWORD dwTraceLevel = 0;

                keyTrace.QueryValue(dwTraceLevel, TRC_WKS50::szwValLevel);
                keyTrace.QueryValue(wstrFileName, TRC_WKS50::szwValFile);

                if(!wstrFileName.empty() && dwTraceLevel)
                {
                    nLocalTraceLevel = dwTraceLevel;
                }
                else
                    nLocalTraceLevel = c_nInvalidTraceLevel;

                if( !wstrFileName.empty() && 
                    KLSTD_IfExists(wstrFileName.c_str()))
                {
                    vecTraceFiles.push_back(wstrFileName);
                };

                if(nTraceLevel != c_nInvalidTraceLevel && nLocalTraceLevel != c_nInvalidTraceLevel)
                    nTraceLevel = std::min(nTraceLevel, nLocalTraceLevel);
                else if(nTraceLevel == c_nInvalidTraceLevel)
                    nTraceLevel = nLocalTraceLevel;
            KLERR_ENDT(1)
            };
        };
    KL_TMEASURE_END()

        //@todo add acquiring traceinfo about other products
    };


    void GetProductParsI(
            const full_prdinf_t&        pi,
            std::wstring*               pwstrDisplayName,
            std::wstring*               pwstrBuild,
            std::wstring*               pwstrVersion,
            bool*                       pbIsRunning,
            std::vector<std::wstring>*  pvecTraceFiles,
            int*                        pnTraceLevel,
            std::wstring*               pwstrDiagLog,
            std::wstring*               pwstrDiagTrace,
            std::wstring*               pwstrInstallDir,
            AVP_dword*                  pdwProps)
    {
    KL_TMEASURE_BEGIN(L"GetProductParsI", 4)
        if(pwstrVersion)
            *pwstrVersion = pi.m_szwVersion;
        if(pwstrDisplayName)
            *pwstrDisplayName = pi.m_szwDisplayName;
        if(pwstrBuild)
            *pwstrBuild = pi.m_szwVersion;

        std::vector<TCHAR> vecProperty;
        vecProperty.resize(MAX_PATH*4+1);

        std::vector<std::wstring>  vecTraceFiles;
        int             nTraceLevel = c_nInvalidTraceLevel;
        std::wstring    wstrInstallDir;

        // acquire installinfo
        bool bServerNagent = false;

        if( wcscmp(pi.m_szwUpgradeCode, KLACDT_AK_UPGRCODE) == 0 && 
            wcscmp(pi.m_szwID, c_szwProductNagent) == 0 )
        {
            bServerNagent = true;
        };

        if( pwstrInstallDir || 
            pwstrDisplayName || 
            pwstrBuild || 
            pvecTraceFiles || 
            pnTraceLevel || 
            pwstrDiagLog || 
            pwstrDiagTrace)
        {
            KLERR_TRY
                KLSTD_TRACE1(1, L"pi.m_wstrProductCode='%ls'\n", pi.m_wstrProductCode.c_str());

                KLSTD_W2CT2 tstrProductCode = pi.m_wstrProductCode.c_str();

                if(pwstrDisplayName && !bServerNagent)
                {
                KLERR_BEGIN
                    DWORD dwLen = vecProperty.size()-1;
                    KLSTD_RGCHK(KLMSIWRAPPER_MsiGetProductInfo(
                                    tstrProductCode, 
                                    INSTALLPROPERTY_INSTALLEDPRODUCTNAME,
                                    &vecProperty[0], 
                                    &dwLen));
                    KLSTD_TRACE1(1, L"ProductName is '%ls'\n", (LPCWSTR)KLSTD_T2CW2(&vecProperty[0]));
                    *pwstrDisplayName = (LPCWSTR)KLSTD_T2CW2(&vecProperty[0]);
                KLERR_ENDT(1)
                }
                else if(pwstrDisplayName && bServerNagent)
                {
                    *pwstrDisplayName = pi.m_szwDisplayName;
                };

                if(pwstrBuild)
                {
                KLERR_BEGIN
                    DWORD dwLen = vecProperty.size()-1;
                    KLSTD_RGCHK(KLMSIWRAPPER_MsiGetProductInfo(
                                    tstrProductCode, 
                                    INSTALLPROPERTY_VERSIONSTRING, 
                                    &vecProperty[0], 
                                    &dwLen));
                    KLSTD_TRACE1(1, L"ProductVersion is '%ls'\n", (LPCWSTR)(KLSTD_T2CW2)&vecProperty[0]);
                    *pwstrBuild = KLSTD_T2CW2(&vecProperty[0]);
                KLERR_ENDT(1)
                };

                KL_TMEASURE_BEGIN(L"MsiGetProductProperty(INSTALLDIR)", 1)
                {
                    DWORD dwLen = vecProperty.size()-1;

                    KLSTD_RGCHK(KLMSIWRAPPER_MsiGetProductInfo(
                                    tstrProductCode, 
                                    INSTALLPROPERTY_INSTALLLOCATION, 
                                    &vecProperty[0], 
                                    &dwLen));

                    KLSTD_TRACE1(1, L"INSTALLDIR is '%ls'\n", (LPCWSTR)(KLSTD_T2CW2)&vecProperty[0]);
                    wstrInstallDir = KLSTD_T2CW2(&vecProperty[0]);
                    if(bServerNagent)
                        wstrInstallDir = KLSTD::DoPathAppend(wstrInstallDir, L"Nagent");
                };
                KL_TMEASURE_END()
                if(pwstrInstallDir)
                    *pwstrInstallDir = wstrInstallDir;
            KLERR_CATCH(pError)
                KLERR_SAY_FAILURE(1, pError);
            KLERR_ENDTRY
        };

        bool        bIsRunning = false;
        AVP_dword   dwProps = 0;
        if(pbIsRunning || pdwProps)
        {
        #ifdef KLSTD_WINNT
            std::wstring wstrServiceName = GetServiceName(pi);
            if(!wstrServiceName.empty())
            {
                bIsRunning= GetProductParsI_IsServiceRunning(wstrServiceName.c_str(), dwProps);
            };
            if(pbIsRunning)
                *pbIsRunning = bIsRunning;
            if(pdwProps)
                *pdwProps = dwProps;
        #endif
        };

        //acquire traceinfo
        if( pvecTraceFiles || pnTraceLevel)
        {
            GetProductPars_GetTraceInfoI(pi, wstrInstallDir, vecTraceFiles, nTraceLevel);
        };

        //acquire diagnostics
        if( wcscmp(pi.m_szwID, c_szwProductNagent) == 0 && 
            (pwstrDiagLog || pwstrDiagTrace) &&
            !wstrInstallDir.empty() )
        {
            std::wstring wstrResult;
            if(pwstrDiagTrace)
            {
                KLSTD_PathAppend(wstrInstallDir, L"$klnagchk-1103.log", wstrResult, true);
                KLERR_BEGIN
                    if(KLSTD_IfExists(wstrResult.c_str()))
                        *pwstrDiagTrace = wstrResult;
                KLERR_ENDT(1)
            };
            if(pwstrDiagLog)
            {
                KLSTD_PathAppend(wstrInstallDir, L"klnagchk.log", wstrResult, true);
                KLERR_BEGIN
                    if(KLSTD_IfExists(wstrResult.c_str()))
                        *pwstrDiagLog = wstrResult;
                KLERR_ENDT(1)
            };
        };
        ;
        if(pvecTraceFiles)
            *pvecTraceFiles = vecTraceFiles;
        
        if(pnTraceLevel)
            *pnTraceLevel = nTraceLevel;

        if(pdwProps)
        {
            if(wcscmp(c_szwProductNagent, pi.m_szwID) == 0)
            {
                dwProps |= PP_MAY_RUN_DIAG|PP_MAY_USE_TRACE;
            }
            else if(    wcscmp(c_szwProductAdmServer, pi.m_szwID) == 0 ||
                        wcscmp(c_szwProductWks60, pi.m_szwID) == 0 ||
                        wcscmp(c_szwProductWks46, pi.m_szwID) == 0 ||
                        wcscmp(c_szwProductWks50, pi.m_szwID) == 0 ||
                        wcscmp(c_szwProductFs50,  pi.m_szwID) == 0 || 
                        wcscmp(c_szwProductFs60, pi.m_szwID) == 0) 
            {
                dwProps |= PP_MAY_USE_TRACE;
            };
            *pdwProps = dwProps;
        };
    KL_TMEASURE_END()
    };

    const wchar_t * c_szwNamesWksNames[] = 
    {
        c_szwProductWks60,
        c_szwProductWks46,
        c_szwProductWks50,
        NULL        
    };

    const wchar_t * c_szwNamesFsNames[] = 
    {
        c_szwProductFs60,
        c_szwProductFs50,
        NULL        
    };

    full_prdinf_t FindProduct(const wchar_t* szwProduct)
    {    
        bool bFound = false;
        full_prdinf_t fpi;
    KL_TMEASURE_BEGIN(L"FindProduct", 4)
        KLSTD_TRACE1(1, L"szwProduct=%ls\n", szwProduct);
        for(size_t i = 0; !bFound && i < KLSTD_COUNTOF(products); ++i)
        {
            bool bOK = true;
            if(wcscmp(products[i].m_szwID, szwProduct) != 0)
                bOK = false;
            
            if(!bOK)
                continue;
        KLERR_BEGIN
            std::vector<TCHAR> szProductCode;
            szProductCode.resize(256);      
            prodinfo_t pi = products[i];
            UINT nResult = KLMSIWRAPPER_MsiEnumRelatedProducts( 
                                    KLSTD_W2T2(pi.m_szwUpgradeCode),
                                    0,
                                    0,
                                    &szProductCode[0]);

            if( nResult != ERROR_SUCCESS && 
                wcscmp(pi.m_szwUpgradeCode, KLACDT_NAGENT_UPGRCODE) == 0 && 
                KLMSIWRAPPER_MsiEnumRelatedProducts( 
                                    KLSTD_W2CT2(KLACDT_AK_UPGRCODE),
                                    0,
                                    0,
                                    &szProductCode[0]) == ERROR_SUCCESS ) 
            {
                pi.m_szwUpgradeCode = KLACDT_AK_UPGRCODE;
                nResult = ERROR_SUCCESS;
            };

            if(nResult != ERROR_SUCCESS )
            {
                KLSTD_TRACE2(   1, 
                                L"Failed to get product code from upgrade code %ls. "
                                L"Error code 0x%X.\n",
                                pi.m_szwUpgradeCode,
                                nResult);
            }
            else
            {
                static_cast<prodinfo_t&>(fpi) = pi;
                fpi.m_wstrProductCode = KLSTD_T2CW2(&szProductCode[0]);
                bFound = true;

                KLSTD_TRACE3(
                        1, 
                        L"Product %ls (upgrd=%ls), %ls found\n", 
                        pi.m_szwID, 
                        pi.m_szwUpgradeCode,
                        fpi.m_wstrProductCode.c_str());
            };
        KLERR_ENDT(1)
        };
        if(!bFound)
        {
            KLSTD_THROW(KLSTD::STDE_NOENT);
        };
    KL_TMEASURE_END()
        return fpi;
    };

    /*
    void GetProductPars(
            const wchar_t*              szwProduct, 
            std::wstring*               pwstrDisplayName,
            std::wstring*               pwstrBuild,
            std::wstring*               pwstrVersion,
            bool*                       pbIsRunning,
            std::wstring*               pwstrTraceFile,
            int*                        pnTraceLevel,
            std::wstring*               pwstrDiagLog,
            std::wstring*               pwstrDiagTrace,
            AVP_dword*                  pdwProps,
            int*                        pnTraceMin,
            int*                        pnTraceMax)
    {   
        full_prdinf_t fpi = FindProduct(szwProduct);
        std::wstring wstrInstFolder;
        ;
        GetProductParsI(
            fpi,
            pwstrDisplayName,
            pwstrBuild,
            pwstrVersion,
            pbIsRunning,
            pwstrTraceFile,
            pnTraceLevel,
            pwstrDiagLog,
            pwstrDiagTrace,
            &wstrInstFolder,
            pdwProps);

        if(pnTraceMin)
            *pnTraceMin= 1;
        if(pnTraceMax)
            *pnTraceMax = 5;
    };
    */

    void SetTracingLevel(const wchar_t* szwProduct, int nLevel)
    {
        SetTracingLevelI(FindProduct(szwProduct), nLevel);
    };

    void SendProductAction(const wchar_t* szwProduct, KLACDT_PRODUCT_ACTION nCode, long Timeout)
    {
        SendProductActionI(FindProduct(szwProduct), nCode, Timeout);
    };

    void ExecuteDiagChk(const wchar_t* szwProduct, long Timeout)
    {
        ExecuteDiagI(FindProduct(szwProduct), Timeout);
    };

    void SaveEventLog(const wchar_t* szwEL, const wchar_t* szwFileName)
    {
        HANDLE hEL = NULL;
        KLERR_TRY
            KLSTD_LECHK(hEL = OpenEventLogW(NULL, szwEL));
            KLSTD_LECHK(BackupEventLogW(hEL, szwFileName));
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
        KLERR_FINALLY
            if(hEL)
                CloseEventLog(hEL);
            KLERR_RETHROW();
        KLERR_ENDTRY
    };

    void SetTracingLevelI(
            const full_prdinf_t&    pi,
            int                     nLevel)
    {
        if( wcscmp(pi.m_szwID, c_szwProductNagent) == 0 || 
            wcscmp(pi.m_szwID, c_szwProductAdmServer) == 0)
        {
            if( c_nInvalidTraceLevel == nLevel)
                nLevel = 0;
            KlRegKey keyTrace;
            KLSTD_RGCHK(keyTrace.Create(
                            HKEY_LOCAL_MACHINE,                
                            (KLSTD_StGetProductKeyW(pi.m_szwProduct, pi.m_szwVersion) + L"\\Debug").c_str(),
                            REG_NONE,
                            REG_OPTION_NON_VOLATILE,
                            KEY_WRITE));
            KLSTD_RGCHK(keyTrace.SetValue((DWORD)nLevel, L"TraceLevel"));
            KLSTD_Sleep(1500);
        }
        else if(    wcscmp(pi.m_szwID, c_szwProductWks60) == 0 ||
                    wcscmp(pi.m_szwID, c_szwProductFs60) == 0    )
        {            
            KLERR_TRY
                KlRegKey keyProfile;
                KLSTD_RGCHK(keyProfile.Open(
                                HKEY_LOCAL_MACHINE,                
                                L"SOFTWARE\\KasperskyLab\\AVP6\\Trace",
                                KEY_READ));

                std::wstring wstrProfile;
                KLSTD_RGCHK(keyProfile.QueryValue(wstrProfile, L"Settings"));

                KlRegKey keyTrace;
                KLSTD_RGCHK(keyTrace.Open(
                                HKEY_LOCAL_MACHINE,
                                std::wstring(L"SOFTWARE\\KasperskyLab\\AVP6\\Trace\\" + 
                                        wstrProfile).c_str(),
                                KEY_WRITE));

                DWORD dwTraceFileEnable = (c_nInvalidTraceLevel != nLevel);
                KLSTD_RGCHK(keyTrace.SetValue(dwTraceFileEnable, L"TraceFileEnable"));
                //if(dwTraceFileEnable)
                {
                    if( c_nInvalidTraceLevel == nLevel)
                        nLevel = 0;

                    DWORD dwTraceFileMaxLevel = nLevel;
                    KLSTD_RGCHK(keyTrace.SetValue(dwTraceFileMaxLevel, L"TraceFileMaxLevel"));
                };
                KLSTD_Sleep(1500);
            KLERR_CATCH(pError)
                KLERR_SAY_FAILURE(4, pError);
                KLERR_RETHROW();
            KLERR_ENDTRY            
        }
        else if( wcscmp(pi.m_szwID, c_szwProductWks46) == 0 )
        {
            //std::wstring wstrTempDir;
            //KLSTD_GetTempFile(wstrTempDir);

            for(size_t ii =0; ii < KLSTD_COUNTOF(TRC_WKS46::pNames); ++ii)
            {
            KLERR_BEGIN
                int nLocalTraceLevel = nLevel;
                KlRegKey keyTrace;
                KLSTD_RGCHK(keyTrace.Open(HKEY_LOCAL_MACHINE, TRC_WKS46::pNames[ii].first, KEY_READ|KEY_WRITE));
                std::wstring wstrFileName, wstrOldFileName;
                DWORD dwTraceLevel = 255;

                keyTrace.QueryValue(dwTraceLevel, TRC_WKS46::szwValLevel);
                keyTrace.QueryValue(wstrFileName, TRC_WKS46::szwValFile );
                keyTrace.QueryValue(wstrOldFileName, TRC_WKS46::szwValOldFile);

                KLSTD_TRACE4(
                        1, 
                        L"RegKey = '%ls', filename='%ls', oldfilename='%ls', level = %u\n", 
                        TRC_WKS46::pNames[ii].first,
                        wstrFileName.c_str(), 
                        wstrOldFileName.c_str(),
                        dwTraceLevel);

                if(nLevel != c_nInvalidTraceLevel && !wstrFileName.empty() && int(dwTraceLevel) == nLevel)
                {
                    //turn on trace that is already turned on on the proper level
                    ;//do nothing
                }
                else if(nLevel != c_nInvalidTraceLevel && !wstrFileName.empty() && int(dwTraceLevel) != nLevel)
                {
                    // change tracelevel
                    KLSTD_RGCHK(keyTrace.SetValue(nLevel, TRC_WKS46::szwValLevel));
                }
                else if(nLevel == c_nInvalidTraceLevel && wstrFileName.empty())
                {
                    //turn off trace that is already turned off
                    ;//do nothing
                }
                else if(nLevel == c_nInvalidTraceLevel && !wstrFileName.empty())
                {
                    //turn off trace
                    //save existing name
                    KLSTD_RGCHK(keyTrace.SetValue(wstrFileName.c_str(), TRC_WKS46::szwValOldFile));
                    //clear name
                    KLSTD_RGCHK(keyTrace.DeleteValue(TRC_WKS46::szwValFile));
                }
                else if(nLevel != c_nInvalidTraceLevel && wstrFileName.empty())
                {//turn on tracing that is turned off
                    
                    // change tracelevel
                    KLSTD_RGCHK(keyTrace.SetValue(nLevel, TRC_WKS46::szwValLevel));
                    
                    if(!wstrOldFileName.empty())
                        KLSTD_RGCHK(keyTrace.SetValue(wstrOldFileName.c_str(), TRC_WKS46::szwValFile));
                    else
                    {
                        std::vector<TCHAR> vecBuffer;
                        vecBuffer.resize(2*MAX_PATH+1);
                        KLSTD_LECHK(GetTempPath(vecBuffer.size()-1, &vecBuffer[0]));
                        wstrFileName = KLSTD::DoPathAppend(
                                    (const wchar_t*)KLSTD_T2CW2(&vecBuffer[0]), 
                                    TRC_WKS46::pNames[ii].second);
                        KLSTD_RGCHK(keyTrace.SetValue(wstrFileName.c_str(), TRC_WKS46::szwValFile));
                    };
                };
            KLERR_ENDT(1)
            };
        }
        else if(    wcscmp(pi.m_szwID, c_szwProductWks50) == 0 ||
                    wcscmp(pi.m_szwID, c_szwProductFs50) == 0)
        {
            const TRC_WKS50::cmp_name_t* c_pNames = 
                    (wcscmp(pi.m_szwID, c_szwProductWks50) == 0)
                    ?   TRC_WKS50::pNamesWks
                    :   TRC_WKS50::pNamesFs;
            
            const size_t c_nNames = 
                    (wcscmp(pi.m_szwID, c_szwProductWks50) == 0)
                    ?   TRC_WKS50::c_nNamesWks
                    :   TRC_WKS50::c_nNamesFs;


            for(size_t ii =0; ii < c_nNames; ++ii)
            {
            KLERR_BEGIN
                int nLocalTraceLevel = nLevel;
                KlRegKey keyTrace;
                KLSTD_RGCHK(keyTrace.Open(HKEY_LOCAL_MACHINE, c_pNames[ii].first, KEY_READ|KEY_WRITE));
                std::wstring wstrFileName;
                DWORD dwTraceLevel = (DWORD)c_nInvalidTraceLevel;

                keyTrace.QueryValue(dwTraceLevel, TRC_WKS50::szwValLevel);
                keyTrace.QueryValue(wstrFileName, TRC_WKS50::szwValFile );

                if(nLevel != c_nInvalidTraceLevel && !wstrFileName.empty() && int(dwTraceLevel) == nLevel)
                {
                    //turn on trace that is already turned on the proper level
                    ;//do nothing
                }
                else if(nLevel != c_nInvalidTraceLevel && !wstrFileName.empty() && int(dwTraceLevel) != nLevel)
                {
                    // change tracelevel
                    KLSTD_RGCHK(keyTrace.SetValue(nLevel, TRC_WKS50::szwValLevel));
                }
                else if(nLevel == c_nInvalidTraceLevel && (wstrFileName.empty() || !dwTraceLevel))
                {
                    //turn off trace that is already turned off
                    ;//do nothing
                }
                else if(nLevel == c_nInvalidTraceLevel && !wstrFileName.empty() && dwTraceLevel)
                {
                    //turn off trace
                    //clear tracelevel
                    KLSTD_RGCHK(keyTrace.DeleteValue(TRC_WKS50::szwValLevel));
                }
                else if(nLevel != c_nInvalidTraceLevel && (wstrFileName.empty() || !dwTraceLevel))
                {//turn on tracing that is turned off

                    // change tracelevel
                    KLSTD_RGCHK(keyTrace.SetValue(nLevel, TRC_WKS50::szwValLevel));
                    
                    if(wstrFileName.empty())
                    {
                        std::vector<TCHAR> vecBuffer;
                        vecBuffer.resize(2*MAX_PATH+1);
                        KLSTD_LECHK(GetTempPath(vecBuffer.size()-1, &vecBuffer[0]));
                        wstrFileName = KLSTD::DoPathAppend(
                                    (const wchar_t*)KLSTD_T2CW2(&vecBuffer[0]), 
                                    c_pNames[ii].second);
                        KLSTD_RGCHK(keyTrace.SetValue(wstrFileName.c_str(), TRC_WKS50::szwValFile));
                    };
                };
            KLERR_ENDT(1)
            };
        };
        //@todo settings traceinfo about other products
    };

#ifdef KLSTD_WINNT
    void DoStartService(  SC_HANDLE         hScm,                     
                        const wchar_t*      szwServiceName,
                        unsigned long       lTimeout)
    {
    KL_TMEASURE_BEGIN(L"DoStartService", 4)

        KLSTD_TRACE1( 1, L"Starting service '%ls'\n", szwServiceName );
        SC_HANDLE hService = NULL;
        KLERR_TRY
                KLSTD_LECHK(hService = ::OpenServiceW(
                                hScm, 
                                szwServiceName, 
                                GENERIC_READ|GENERIC_EXECUTE));
                SERVICE_STATUS ssStatus;
                KLSTD_LECHK(::QueryServiceStatus( hService, &ssStatus));
                if( ( ssStatus.dwCurrentState != SERVICE_RUNNING ) && 
                    ( ssStatus.dwCurrentState != SERVICE_START_PENDING ) )
                {
                    KLSTD_LECHK(::StartService( hService, 0, NULL ));
                    unsigned long clkT0 = KLSTD::GetSysTickCount();
                    while( ssStatus.dwCurrentState != SERVICE_RUNNING )
                    {
                        Sleep( 500 );
                        KLSTD_LECHK(::QueryServiceStatus( hService, & ssStatus ) );
                        if(lTimeout >= 0 && KLSTD::DiffTickCount(clkT0, KLSTD::GetSysTickCount()) > 1000*lTimeout )
                        {
                            KLSTD_THROW( KLSTD::STDE_TIMEOUT );
                        };
                        if(KLSTD_GetShutdownFlag())
                        {
                            KLSTD_THROW(KLSTD::STDE_CANCELED);
                        };
                    };
                };
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
        KLERR_FINALLY
            if(hService)
                CloseServiceHandle(hService);
            KLERR_RETHROW();
        KLERR_ENDTRY
    KL_TMEASURE_END()
    };

    #define KLBCP_SLEEP_AFTER_KILL  5000

    void DoStopService( SC_HANDLE           hScm,                     
                        const wchar_t*      szwServiceName,
                        unsigned long       lTimeout)
    {
    KL_TMEASURE_BEGIN(L"DoStopService", 4)

        KLSTD_TRACE1( 1, L"Starting service '%ls'\n", szwServiceName );
        SC_HANDLE hService = NULL;
        KLERR_TRY
                KLSTD_LECHK(hService = ::OpenServiceW(
                                hScm, 
                                szwServiceName, 
                                GENERIC_READ|GENERIC_EXECUTE));
            const unsigned long c_ulStart = KLSTD::GetSysTickCount();
            for(bool bStop = false; !bStop;)
            {
                if(KLSTD_GetShutdownFlag())
                {
                    KLSTD_THROW(KLSTD::STDE_CANCELED);
                };

                SERVICE_STATUS ssStatus;
                KLSTD_LECHK(::QueryServiceStatus( hService, & ssStatus));
                KLSTD_TRACE1(1, L"Service status: ssStatus.dwCurrentState = %u\n", ssStatus.dwCurrentState);
                switch(ssStatus.dwCurrentState)
                {
                case SERVICE_CONTINUE_PENDING:
                case SERVICE_PAUSE_PENDING:
                case SERVICE_START_PENDING:
                case SERVICE_STOP_PENDING:
                    ;// do nothing
                    break;
                case SERVICE_PAUSED:
                    KL_TMEASURE_BEGIN(L"Sending 'SERVICE_CONTROL_CONTINUE'", 1)
                        KLSTD_LECHK(::ControlService( hService, SERVICE_CONTROL_CONTINUE, &ssStatus));
                    KL_TMEASURE_END()
                    break;
                case SERVICE_RUNNING:
                    KL_TMEASURE_BEGIN(L"Sending 'SERVICE_CONTROL_STOP'", 1)
                        KLSTD_LECHK(::ControlService( hService, SERVICE_CONTROL_STOP, &ssStatus));
                    KL_TMEASURE_END()
                    break;
                case SERVICE_STOPPED:
                    bStop = true;
                    break;
                };
                if(!bStop)
                {
                    if( lTimeout >= 0 && KLSTD::DiffTickCount(KLSTD::GetSysTickCount(), c_ulStart) > 
                                    (1000*(lTimeout)) )
                    {
                        KLSTD_THROW(KLSTD::STDE_TIMEOUT);
                    }
                    else
                    {
                        KLSTD_Sleep(500);
                    };
                };
            };
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
        KLERR_FINALLY
            if(hService)
                CloseServiceHandle(hService);
            KLERR_RETHROW();
        KLERR_ENDTRY
    KL_TMEASURE_END()
    };
    
    void SendProductActionI(
            const full_prdinf_t&    pi, 
            KLACDT_PRODUCT_ACTION   nCode,
            long                    lTimeout)
    {
    KL_TMEASURE_BEGIN(L"SendProductActionI", 1)
        std::wstring wstrService = GetServiceName(pi);
        if(wstrService.empty())
        {
            KLSTD_NOTIMP();
        };
        SC_HANDLE hScm = NULL;
        KLERR_TRY
            KLSTD_LECHK(hScm = OpenSCManagerW(
                        NULL,
                        NULL,
                        GENERIC_READ|GENERIC_EXECUTE));
            switch(nCode)
            {
            case KLACDT_PA_START:
                DoStartService(hScm, wstrService.c_str(), lTimeout);
                break;
            case KLACDT_PA_STOP:
                DoStopService(hScm, wstrService.c_str(), lTimeout);
                break;
            case KLACDT_PA_RESTART:
                DoStopService(hScm, wstrService.c_str(), lTimeout);
                DoStartService(hScm, wstrService.c_str(), lTimeout);
                break;
            };
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
        KLERR_FINALLY
            if(hScm)
            {
                ::CloseServiceHandle(hScm);
                hScm = NULL;
            };
            KLERR_RETHROW();
        KLERR_ENDTRY
    KL_TMEASURE_END()
    };
#else
    void SendProductActionI(
            const full_prdinf_t&    pi, 
            KLACDT_PRODUCT_ACTION   nCode,
            long                    lTimeout)
    {
    KL_TMEASURE_BEGIN(L"SendProductActionI", 1)
        KLSTD_NOTIMP();
    KL_TMEASURE_END()
    };
#endif

    void DoRunProcess(
                const wchar_t*  szwCommandLine, 
                unsigned&       nExitCode,
                const wchar_t*  szwCurrentDir,            
                long            lTimeout,
                const void*     lpEnvironment,
                const wchar_t*  szwStdOut)
    {
        KLSTD_CHK(szwCommandLine, szwCommandLine && szwCommandLine[0]);
    KL_TMEASURE_BEGIN(L"DoRunProcess", 1)

        const std::wstring& wstrCmdLine = DoExpandEnvStrings(szwCommandLine);

        KLSTD_TRACE4(
                1, 
                L"szwCommandLine='%ls' ('%ls'), szwCurrentDir='%ls', lTimeout=%u\n", 
                szwCommandLine, 
                wstrCmdLine.c_str(),
                szwCurrentDir, 
                lTimeout);

        PROCESS_INFORMATION pi;
        KLSTD_ZEROSTRUCT(pi);

        STARTUPINFO si;
        KLSTD_ZEROSTRUCT(si);
        si.cb = sizeof(si);

        HANDLE hOut = NULL;

        const unsigned long c_ulStart = KLSTD::GetSysTickCount();
        KLERR_TRY
            if(szwStdOut && szwStdOut[0])
            {
                SECURITY_ATTRIBUTES sa;
                KLSTD_ZEROSTRUCT(sa);
                sa.bInheritHandle = TRUE;

                hOut = CreateFile(
                            KLSTD_W2CT2(szwStdOut),
                            GENERIC_WRITE|GENERIC_READ,
                            FILE_SHARE_READ|FILE_SHARE_WRITE,
                            &sa,
                            CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);
                KLSTD_LECHK(hOut && hOut != INVALID_HANDLE_VALUE);
                si.dwFlags = STARTF_USESTDHANDLES;
                si.hStdInput = si.hStdOutput = si.hStdError = hOut;
            };

        #ifdef UNICODE
            DWORD dwEnvFlag = CREATE_UNICODE_ENVIRONMENT;
        #else
            DWORD dwEnvFlag = 0;
        #endif
            KLSTD_LECHK(CreateProcess(
                        NULL,
                        KLSTD_W2T2(const_cast<wchar_t*>(wstrCmdLine.c_str())),
                        NULL,
                        NULL,
                        TRUE,
                        /*CREATE_NO_WINDOW|DETACHED_PROCESS|*/dwEnvFlag, //userdump fails under XP if CREATE_NO_WINDOW|DETACHED_PROCESS specified
                        const_cast<void*>(lpEnvironment),
                        szwCurrentDir
                            ?   (LPCTSTR)KLSTD_W2CT2(szwCurrentDir)
                            :   NULL,
                        &si,
                        &pi));
            for(bool bStop = false; !bStop; )
            {
                if(KLSTD_GetShutdownFlag())
                {
                    TerminateProcess(pi.hProcess, -4);
                    KLSTD_THROW(KLSTD::STDE_CANCELED);
                };
                DWORD dwResult = WaitForSingleObject(
                            pi.hProcess, 
                            1000);
                switch(dwResult)
                {
                case WAIT_OBJECT_0://procecss terminated
                    {
                        DWORD dwExitCode = 0;
                        KLSTD_LECHK(GetExitCodeProcess(pi.hProcess, &dwExitCode));
                        nExitCode = dwExitCode;
                        KLSTD_TRACE1(1, L"nExitCode=%u\n", nExitCode);
                    };
                    bStop = true;
                    break;
                case WAIT_FAILED:
                    KLSTD_LECHK(false);
                    break;
                default:
                    break;
                };
                if( lTimeout >= 0 && 
                        KLSTD::DiffTickCount(KLSTD::GetSysTickCount(), c_ulStart) > 
                                (unsigned long)(1000*(lTimeout)) )
                {
                    TerminateProcess(pi.hProcess, -5);
                    KLSTD_THROW(KLSTD::STDE_TIMEOUT);
                };
            };
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
        KLERR_FINALLY
            if(pi.hProcess)
                CloseHandle(pi.hProcess);
            if(pi.hThread)
                CloseHandle(pi.hThread);
            if(hOut && hOut != INVALID_HANDLE_VALUE)
                CloseHandle(hOut);
            KLERR_RETHROW();
        KLERR_ENDTRY
    KL_TMEASURE_END()
    };

    
    void ExecuteDiagI(
            const full_prdinf_t&       pi,
            long                       lTimeout)
    {
    KL_TMEASURE_BEGIN(L"ExecuteDiagI", 4)
        KLSTD_CHK(pi.m_wstrProductCode, !pi.m_wstrProductCode.empty());
        if(wcscmp(pi.m_szwID, c_szwProductNagent) != 0)
        {
            KLSTD_NOTIMP();
        };

        std::wstring    wstrInstallDir;
        
        GetProductParsI(
            pi, 
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            &wstrInstallDir,
            NULL);

        KLSTD_ASSERT_THROW(!wstrInstallDir.empty());

        std::wstring wstrExeName, wstrLogFile;
        KLSTD_PathAppend(wstrInstallDir, L"klnagchk.exe", wstrExeName, true);

        KLSTD_PathAppend(wstrInstallDir, L"klnagchk.log", wstrLogFile, true);

        std::wostringstream os;
        os << L"\"" << wstrExeName << L"\" -tl 4 -logfile \""  << wstrLogFile << L"\"";
        //create process
        unsigned nExitCode = 0;
        DoRunProcess(os.str().c_str(), nExitCode, wstrInstallDir.c_str(), lTimeout, NULL);
    KL_TMEASURE_END()
    };


    std::wstring DoExpandEnvStrings(const wchar_t* szwX)
    {
        std::vector<TCHAR> vecBuffer;
        vecBuffer.resize(MAX_PATH*4+1);
        ExpandEnvironmentStrings(KLSTD_W2CT2(szwX), &vecBuffer[0], vecBuffer.size()-1);
        return KLSTD_T2CW2(&vecBuffer[0]);
    };

    std::wstring GetNagentInstallFolder()
    {
        std::wstring    wstrInstallDir;        
        KLERR_BEGIN
            GetProductParsI(
                FindProduct(c_szwProductNagent), 
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                &wstrInstallDir,
                NULL);
        KLERR_ENDT(1)
        return wstrInstallDir;
    };
};

namespace KLACDTAPI
{
    std::wstring EncodeFilePath2(const wchar_t* szwFile)
    {
        if(!szwFile || !szwFile[0])
            return L"";
        std::wstring wstrDir, wstrFile, wstrExt;
        KLSTD_SplitPath(szwFile, wstrDir, wstrFile, wstrExt);
        std::wstring wstrDN = wstrFile + wstrExt;
        AVP_qword qwSize = 0;
        KLERR_BEGIN
            WIN32_FILE_ATTRIBUTE_DATA fad;
            KLSTD_ZEROSTRUCT(fad);
            KLSTD_LECHK(GetFileAttributesEx(
                        KLSTD_W2CT2(szwFile),
                        GetFileExInfoStandard,
                        &fad));
            qwSize = (AVP_qword(fad.nFileSizeHigh) << 32) | AVP_qword(fad.nFileSizeLow);
        KLERR_ENDT(1)
        return EncodeFilePath(szwFile, wstrDN.c_str(), qwSize);        
    };

};
