#include <std/win32/klos_win32v50.h>
#include <kca/acdt/acdt_lib.h>
#include <std/par/par_conv.h>
#include <std/stress/st_prefix.h>
#include <common/measurer.h>
#include <std/win32/klos_win32_errors.h>
#include <std/hstd/hostdomain.h>
#include <kca/prss/settingsstorage.h>
#include <kca/prss/helpersi.h>

#include <cstdio>
#include "..\ft\cabsdk\mscompress.hpp"

#include <sstream>
#include <set>

#define KLCS_MODULENAME L"KLACDTLIB"

namespace mszip
{
    /** extract files from cabs */
    class my_cab_decompressor : public cab_helper
    {
    public:
	    /** ctor */
	    my_cab_decompressor(compression_type type
					    , long size
					    , std::string const& folder
					    , std::string const& name)
	    {
		    cab_helper::Init( type, size, folder, name, false );
	    }
	    ~my_cab_decompressor()
	    {
	    }
	    void extract( const std::string &extractPath )
	    {
		    if(!fn_fdi_copy_(hfdi_
							    , const_cast<char*>(name_.c_str())
							    , const_cast<char*>(folder_.c_str())
							    , 0
							    , decomp_fdi_notify
							    , NULL
							    , const_cast<char*>(extractPath.c_str()) ))
			    detail::_fci_issue_error(error_.erfOper);
	    };

	    static FNFDINOTIFY(decomp_fdi_notify)
	    {
		    if ( fdint==fdintCOPY_FILE )
		    {
			    std::string folderName( (char *)pfdin->pv );
			    int err = 0;
            
                const std::string& strFilePath = folderName + std::string(pfdin->psz1);
                {
                    std::wstring wstrDir, wstrFIle, wstrExt;
                    KLSTD_SplitPath(
                                (const wchar_t*)KLSTD_A2CW2(strFilePath.c_str()),
                                wstrDir, wstrFIle, wstrExt);

                    KLERR_IGNORE(KLSTD_CreatePath(wstrDir.c_str()));
                };

			    int fHandle = fci_open( (char *)strFilePath.c_str(), 
				    _O_CREAT | _O_BINARY | _O_RDWR, _S_IREAD | _S_IWRITE, &err,
				    NULL );
			    if ( err ) return (-1);
			    return fHandle;
		    }
		    if ( fdint==fdintCLOSE_FILE_INFO )
		    {
			    int err = 0;
			    fci_close( pfdin->hf, &err, NULL );
			    if ( err ) return (-1);
			    SetFileAttributesA( pfdin->psz1, pfdin->attribs );
			    return 1;
		    }
		    return 0;
	    };
    };
};


namespace KLACDTAPI
{
    void GetFilesFromDir(
                const std::wstring&         wstrDir, 
                std::vector<std::wstring>&  vecStrings, 
                const std::wstring&         wstrMask = L"*")
    {
    KL_TMEASURE_BEGIN(L"KLACDTAPI::GetFilesFromDir", 3)
        std::wstring wstrMaskedPath;
        KLSTD_PathAppend(wstrDir, wstrMask, wstrMaskedPath, true);
        KLSTD_GetFilesByMask(wstrMaskedPath, vecStrings);
    KL_TMEASURE_END()
    };

    void GetDirsFromDir(
                const std::wstring&         wstrDir, 
                std::vector<std::wstring>&  vecStrings, 
                const std::wstring&         wstrMask = L"*")
    {
    KL_TMEASURE_BEGIN(L"KLACDTAPI::GetDirsFromDir", 3)
        std::wstring wstrMaskedPath;
        KLSTD_PathAppend(wstrDir, wstrMask, wstrMaskedPath, true);
        KLSTD_GetSubdirsByMask(wstrMaskedPath, vecStrings);
    KL_TMEASURE_END()
    };

    void PutFilesFromFolder(
            mszip::cab_compressor&  cf,
            const std::wstring&     wstrFsFolder,
            const std::wstring&     wstrFolderInCab,
            bool                    bRecursive = false)
    {
    KL_TMEASURE_BEGIN(L"KLACDTAPI::PutFilesFromFolder", 3)
        KLERR_TRY
            {
                std::vector<std::wstring>   vecStrings;
                GetFilesFromDir(wstrFsFolder, vecStrings);
                for(size_t i = 0; i < vecStrings.size(); ++i)
                {
                    std::wstring wstrFullName;
                    KLSTD_PathAppend(wstrFsFolder, vecStrings[i], wstrFullName, true);
                    cf.compress_file_wide(
                                wstrFullName.c_str(), 
                                wstrFolderInCab + L"\\" + vecStrings[i]);
                };
            };
            if(bRecursive)
            {
                std::vector<std::wstring>   vecStrings;
                GetDirsFromDir(wstrFsFolder, vecStrings);
                for(size_t i = 0; i < vecStrings.size(); ++i)
                {
                    std::wstring wstrFullName;
                    KLSTD_PathAppend(wstrFsFolder, vecStrings[i], wstrFullName, true);
                    PutFilesFromFolder(
                                cf,
                                wstrFullName.c_str(), 
                                wstrFolderInCab + L"\\" + vecStrings[i],
                                true);
                };
            };
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
        KLERR_FINALLY
            KLERR_RETHROW();
        KLERR_ENDTRY
    KL_TMEASURE_END()
    };

    void PackCommonData__PackProductSettings(
            mszip::cab_compressor&              cf,
            const KLPRSS::product_version_t&    prod)
    {
    KL_TMEASURE_BEGIN(L"KLACDTAPI::PackCommonData__PackProductSettings", 3)
        const std::wstring wstrFoldInCab = prod.product + L"__" + prod.version;
        KLERR_BEGIN
            const std::wstring & wstrType = KLPRSS_MakeTypeP(
                    KLPRSS::c_szwSST_HostSP,
                    prod.product.c_str(),
                    prod.version.c_str());
            const std::wstring& wstrPolicyPath = 
                    KLPRSS_TypeToFileSystemPath(wstrType);
            std::wstring wstrDir, wstrFile, wstrExt;
            KLSTD_SplitPath(
                    wstrPolicyPath.c_str(), 
                    wstrDir, 
                    wstrFile, 
                    wstrExt);
            PutFilesFromFolder(cf, wstrDir, wstrFoldInCab + L"\\Data");
        KLERR_ENDT(1)
            ;
        KLERR_BEGIN
            const std::wstring & wstrType = KLPRSS_MakeTypeP(
                    KLPRSS::c_szwSST_TasksSS,
                    prod.product.c_str(),
                    prod.version.c_str());
            const std::wstring& wstrTasksPath = 
                    KLPRSS_TypeToFileSystemPath(wstrType);
            PutFilesFromFolder(cf, wstrTasksPath, wstrFoldInCab + L"\\Tasks");
        KLERR_ENDT(1)
    KL_TMEASURE_END()
    };
    
    void PackCommonData(const wchar_t* szwDstFile, bool bCompress)
    {
    KL_TMEASURE_BEGIN(L"KLACDTAPI::PackCommonData", 3)
        KLSTD_CHK(szwDstFile, szwDstFile && szwDstFile[0]);
        KLSTD_Unlink(szwDstFile, false);
        KLERR_TRY
            mszip::cab_compressor cf;
            cf.InitWide(
                    bCompress
                        ?   mszip::MSZIP_TYPE
                        :   mszip::NONE_TYPE, 
                    std::numeric_limits<long>::max(),
                    L"",
                    szwDstFile);
            KLERR_BEGIN
                std::wstring wstrDir;
                {
                    CRegKey key;
                    KLSTD_RGCHK(key.Open(
                            HKEY_LOCAL_MACHINE, 
                            _T("SOFTWARE\\KasperskyLab\\Components\\34"),
                            KEY_READ));
                    
                    std::wstring wstrTempFname;
                    KLERR_BEGIN
                        KLSTD_GetTempFile(wstrTempFname);
                        KLSTD_W2CT2 tstrTempFname(wstrTempFname.c_str());
                        KLSTD_RGCHK(::RegSaveKey(key, tstrTempFname, NULL));
                        cf.compress_file_wide(wstrTempFname, L"registry/keyKasperskyLab.reg");
                    KLERR_ENDT(1)
                    if(!wstrTempFname.empty())
                    {
                        KLERR_IGNORE(KLSTD_Unlink(wstrTempFname.c_str(), false));
                    };
                }
                {
                    CRegKey key;
                    KLSTD_RGCHK(key.Open(
                            HKEY_LOCAL_MACHINE, 
                            _T("SOFTWARE\\KasperskyLab\\Components\\34"),
                            KEY_READ));
                    
                    std::vector<TCHAR> vecBuffer;
                    vecBuffer.resize(2*MAX_PATH+1);
                    DWORD dwCount = sizeof(TCHAR)*(vecBuffer.size()-1);
                    KLSTD_RGCHK(key.QueryValue(&vecBuffer[0], _T("SS_SETTINGS"), &dwCount)); 
                    std::wstring wstrFile, wstrExt;
                    KLSTD_SplitPath(
                            (const wchar_t*)KLSTD_T2CW2(&vecBuffer[0]), 
                            wstrDir, 
                            wstrFile, 
                            wstrExt);
                };
                PutFilesFromFolder(cf, wstrDir, L"SETTINGS");
            KLERR_ENDT(1)
                ;
            KLERR_BEGIN
                KLSTD::CAutoPtr<KLPRSS::SettingsStorage> pSs;
	            KLPRSS_CreateSettingsStorageDirect(
							            KLPRSS_GetSettingsStorageLocation(),
							            KLSTD::CF_OPEN_EXISTING,
							            KLSTD::AF_READ,
							            &pSs);
                KLPAR::ParamsPtr pData;
                pSs->Read(  KLPRSS_PRODUCT_CORE, 
                            KLPRSS_VERSION_INDEPENDENT, 
                            L"SubscriptionData", 
                            &pData);
                const std::wstring& wstrFolder = KLPAR::GetStringValue(
                                    pData, 
                                    L"LocalWritableFolder");
                PutFilesFromFolder(cf, wstrFolder, L"na_localdata");
            KLERR_ENDT(1)
                ;
            KLERR_BEGIN
                KLSTD::CAutoPtr<KLPRSS::SettingsStorage> pSs;
	            KLPRSS_CreateSettingsStorageDirect(
							            KLPRSS_GetSettingsStorageLocation(
                                                KLPRSS::SS_PRODINFO),
							            KLSTD::CF_OPEN_EXISTING,
							            KLSTD::AF_READ,
							            &pSs);

                std::vector<KLPRSS::product_version_t>	vecProducts;
                KLPRSS_GetInstalledProducts2(pSs, vecProducts);
                for(size_t i = 0; i < vecProducts.size(); ++i)
                {
                KLERR_BEGIN
                    const KLPRSS::product_version_t& prod = 
                            vecProducts[i];
                    PackCommonData__PackProductSettings(cf, prod);
                KLERR_ENDT(1)
                };
            KLERR_ENDT(1)

        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
        KLERR_FINALLY
            KLERR_RETHROW();
        KLERR_ENDTRY
    KL_TMEASURE_END()
    };

    void CompressFolder(
            const wchar_t*          szwFsFolder,
            const wchar_t*          szwFolderInCab,
            const wchar_t*          szwDstFile, 
            bool                    bRecursive,
            bool                    bCompress)
    {
    KL_TMEASURE_BEGIN(L"KLACDTAPI::CompressFolder", 3)
        KLSTD_CHK(szwFsFolder, szwFsFolder && szwFsFolder[0]);
        KLSTD_CHK(szwFolderInCab, szwFolderInCab && szwFolderInCab[0]);
        KLSTD_CHK(szwDstFile, szwDstFile && szwDstFile[0]);
        KLSTD_Unlink(szwDstFile, false);
        KLERR_TRY
            mszip::cab_compressor cf;
            cf.InitWide(
                    bCompress
                        ?   mszip::MSZIP_TYPE
                        :   mszip::NONE_TYPE, 
                    std::numeric_limits<long>::max(),
                    L"",
                    szwDstFile);
            PutFilesFromFolder(
                    cf,
                    szwFsFolder,
                    szwFolderInCab,
                    bRecursive);
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
        KLERR_FINALLY
            KLERR_RETHROW();
        KLERR_ENDTRY
    KL_TMEASURE_END()
    };

    void CompressFile(
        const wchar_t*          szwFsFile,
        const wchar_t*          szwFolderInCab,
        const wchar_t*          szwDstFile, 
        bool                    bCompress)
    {
    KL_TMEASURE_BEGIN(L"KLACDTAPI::CompressFile", 3)
        KLSTD_CHK(szwFsFile, szwFsFile && szwFsFile[0]);
        KLSTD_CHK(szwFolderInCab, szwFolderInCab && szwFolderInCab[0]);
        KLSTD_CHK(szwDstFile, szwDstFile && szwDstFile[0]);
        KLSTD_Unlink(szwDstFile, false);
        KLERR_TRY
            mszip::cab_compressor cf;
            cf.InitWide(
                    bCompress
                        ?   mszip::MSZIP_TYPE
                        :   mszip::NONE_TYPE, 
                    std::numeric_limits<long>::max(),
                    L"",
                    szwDstFile);
            std::wstring wstrDir, wstrFile, wstrExt;
            KLSTD_SplitPath(szwFsFile, wstrDir, wstrFile, wstrExt);
            cf.compress_file_wide(
                        szwFsFile, 
                        std::wstring(szwFolderInCab) + L"\\" + wstrFile + wstrExt);
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
        KLERR_FINALLY
            KLERR_RETHROW();
        KLERR_ENDTRY
    KL_TMEASURE_END()
    };

    std::wstring AppendSlah2Folder(const std::wstring& wstrPath)
    {
        KLSTD_CHK(wstrPath, !wstrPath.empty());
        std::wstring wstrRes = wstrPath;

        if( L'\\' != wstrRes[wstrRes.size()-1] && 
            L'/'  != wstrRes[wstrRes.size()-1])
        {
            wstrRes += L'\\';
        };
        return wstrRes;        
    };

    void DecompressFolder(
        const wchar_t*          szwCabFile,         
        const wchar_t*          szwDstFsFolder)
    {
    KL_TMEASURE_BEGIN(L"KLACDTAPI::DecompressFolder", 3)
        KLSTD_CHK(szwCabFile, szwCabFile && szwCabFile[0]);
        KLSTD_CHK(szwDstFsFolder, szwDstFsFolder && szwDstFsFolder[0]);
        
        KLSTD_DeletePath(szwDstFsFolder);
        KLSTD_CreatePath(szwDstFsFolder);

        KLERR_TRY
			std::wstring wstrDir, wstrName, wstrExt;
            KLSTD_SplitPath(szwCabFile, wstrDir, wstrName, wstrExt);
            
            KLSTD_W2CA2 strFolderInCab =    AppendSlah2Folder(wstrDir).c_str();
            KLSTD_W2CA2 strCabFile =        (wstrName + wstrExt).c_str();
            KLSTD_W2CA2 strDstFsFolder =    AppendSlah2Folder(szwDstFsFolder).c_str();

            KLSTD_TRACE3(
                    3, 
                    L"\n\tFolderInCab='%hs'"
                    L"\n\tCabFile='%hs'"
                    L"\n\tDstFsFolder='%hs'\n\n",
                    (const char*)strFolderInCab,
                    (const char*)strCabFile,
                    (const char*)strDstFsFolder);
            mszip::my_cab_decompressor cd(
                    mszip::NONE_TYPE, 
                    std::numeric_limits<long>::max(),
                    (const char*)strFolderInCab,
                    (const char*)strCabFile);
            cd.extract((const char*)strDstFsFolder);
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
            KLERR_IGNORE(KLSTD_DeletePath(szwDstFsFolder));
        KLERR_FINALLY            
            KLERR_RETHROW();
        KLERR_ENDTRY
    KL_TMEASURE_END()
    };
};
