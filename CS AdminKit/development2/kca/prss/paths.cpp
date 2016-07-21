/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	paths.cpp
 * \author	Andrew Kazachkov
 * \date	07.04.2003 12:44:34
 * \brief	
 * 
 */

#include <std/base/kldefs.h>
#include <std/conv/klconv.h>
#include <std/base/klbase.h>

#include <std/stress/st_prefix.h>
#include <std/err/klerrors.h>
#include <std/par/params.h>
#include "kca/prss/prssconst.h"
#include "kca/prss/settingsstorage.h"
#include "kca/prss/errors.h"
#include "nameconvertor.h"
#include "kca/prss/helpersi.h"
#include "std/stress/st_prefix.h"

#include <sstream>

#ifdef _WIN32
#include <atlbase.h>
#include <shlobj.h>
#include <avpregid.h>
#endif

#include <iostream>
#include "std/trc/trace.h"

#define KLCS_MODULENAME L"KLPRSS"

using namespace std;
using namespace KLSTD;
using namespace KLERR;
using namespace KLPAR;
using namespace KLPRSS;

#ifdef N_PLAT_NLM
#include "std/conv/wcharcrt.h"
#endif

#ifndef _WIN32

#include <common/bin2hex.h>
#include <openssl/md5.h>

KLCSKCA_DECL void KLPRSS_AcquireProductRoot(
                                const std::wstring&         wstrRootDir,
                                const std::wstring&         wstrProduct,
                                const std::wstring&         wstrVersion,                                
                                std::wstring&               wstrRoot)
{
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5_CTX md5;
    MD5_Init(&md5);
    MD5_Update(&md5, wstrProduct.c_str(), wstrProduct.size());
    MD5_Update(&md5, wstrVersion.c_str(), wstrVersion.size());
    MD5_Final(&digest[0], &md5);
    KLSTD_PathAppend(   wstrRootDir, 
                        KLSTD::MakeHexDataW(&digest[0], MD5_DIGEST_LENGTH),
                        wstrRoot,
                        true);
};


#endif // not WIN32

/*!
  \brief	Does platform-specific mapping of "global settings storage type" to
            file-system path.

  \param	wstrType    [in] global settings storage type. It may be one of
                        following constants defined in file prssconst.h:
                            c_szwSST_HostSS,
                            c_szwSST_CUserSS,
                            c_szwSST_ProdinfoSS,
                            c_szwSST_RuntimeSS,
                            c_szwSST_TestGS.

  \param	wstrResult  [out] file-system path
  \return	true if succeeded
  \exception    function doesn't throw any exceptions
*/
static bool MapGlobalSstypeToFspath(
                                    const std::wstring& wstrType,
                                    std::wstring&       wstrResult)
{    
    bool bResult=false;
    if(wstrType == c_szwSST_CUserSS)//user-specific ss
    {
        KLERR_BEGIN
            KLSTD_MakePath(
                            KLPRSS_MakeUserDataPath(),
                            std::wstring(c_szwUserData_FileName)+KLSTD_StGetPrefixW(),
                            c_szwFileExt_Data,
                            wstrResult);
            bResult=true;
        KLERR_ENDT(3)
    }
    else    
    if(wstrType == c_szwSST_TestGS)//ss for testing
    {
        wstrResult=KLPRSS_TEST_SS_BASE;
        bResult=true;
    }
    else//other cases
    {        
#ifdef _WIN32   //system registry is used for mapping on Windows
        KLSTD_USES_CONVERSION;
        TCHAR szBuffer[MAX_PATH];
	    KLERR_BEGIN
		    CRegKey key;        
		    const std::basic_string<TCHAR>& strKey=KLSTD_StGetRootKey();
		    long lResult=key.Open(HKEY_LOCAL_MACHINE, strKey.c_str(), KEY_READ);
		    if(lResult!=NO_ERROR)KLSTD_THROW_LASTERROR_CODE(lResult);
		    DWORD dwBuffer = (KLSTD_COUNTOF(szBuffer)-1)*sizeof(szBuffer[0]);
            lResult=key.QueryValue(szBuffer, KLSTD_W2CT(wstrType.c_str()), &dwBuffer);
            if(lResult!=NO_ERROR)KLSTD_THROW_LASTERROR_CODE(lResult);
            wstrResult=KLSTD_T2W(szBuffer);
            bResult=true;
	    KLERR_ENDT(3)
#else

    //@todo read config file instead !!!

    std::wostringstream os;

    os << KLSTD_StGetDefDirW() << wstrType << KLPRSS::c_szwFileExt_Data;

    wstrResult = os.str();

    bResult=true;

#endif
    };
    return bResult;
};

/*!
  \brief    Does platform-specific mapping of "product-specific settings
            storage type" to file-system path.
  \param    wstrType    [in] product-specific settings storage type. It may be
                        one of following constants defined in file prssconst.h:
                            c_szwSST_HostSP,
                            c_szwSST_TasksSS
  \param    wstrProduct [in] product name
  \param    wstrVersion [in] version name
  \param    wstrResult  [out] file-system path
  \return   true if succeeded
  \exception    function doesn't throw any exceptions
*/
static bool MapProductSstypeToFspath(
                                  const std::wstring&   wstrType,
                                  const std::wstring&   wstrProduct,
                                  const std::wstring&   wstrVersion,
                                  std::wstring&         wstrResult)
{
    bool bResult=false;
#ifdef _WIN32
    KLSTD_USES_CONVERSION;
    TCHAR szBuffer[MAX_PATH];
	KLERR_BEGIN
		CRegKey key;
		std::basic_string<TCHAR>& strKey=KLSTD_StGetProductKey(wstrProduct.c_str(), wstrVersion.c_str());
		long lResult=key.Open(HKEY_LOCAL_MACHINE, strKey.c_str(), KEY_READ);
		if(lResult!=NO_ERROR)KLSTD_THROW_LASTERROR_CODE(lResult);
		DWORD dwBuffer = (KLSTD_COUNTOF(szBuffer)-1)*sizeof(szBuffer[0]);
        lResult=key.QueryValue(szBuffer, KLSTD_W2CT(wstrType.c_str()), &dwBuffer);
        if(lResult!=NO_ERROR)KLSTD_THROW_LASTERROR_CODE(lResult);
        wstrResult=KLSTD_T2W(szBuffer);
        bResult=true;
	KLERR_ENDT(3)
#else

    //@todo read config file instead !!!

    std::wstring wstrRoot;
    if( KLCS_PRODUCT_ADMSERVER == wstrProduct ||
        KLCS_PRODUCT_NAGENT == wstrProduct)
    {
        wstrRoot = KLSTD_StGetDefDirW();
    }
    else
    {
        KLPRSS_AcquireProductRoot(
                KLSTD_StGetDefDirW() + L"products",
                wstrProduct, 
                wstrVersion,
                wstrRoot);
    };
    std::wostringstream os;
    os << wstrType << L"-" << wstrProduct << KLPRSS::c_szwFileExt_Data;
    KLSTD_PathAppend( wstrRoot, os.str(), wstrResult, true);
    bResult=true;
#endif
    return bResult;
};


KLCSKCA_DECL std::wstring KLPRSS_TypeToFileSystemPath(const std::wstring& wstrType)
{
    if(wstrType[0] != c_chwSSP_Marker)
        return wstrType;
    CAutoPtr<Params> pData;
    KLPRSS_ExpandType(wstrType, &pData);
    std::wstring wstrRealType=GetStringValue(pData, c_szwSSP_Type);
    std::wstring wstrResult;
    KLPRSS_ResolveValue(wstrRealType, pData, wstrResult);
    return wstrResult;
};

KLCSKCA_DECL void KLPRSS_UnexpandType(KLPAR::Params* pData, std::wstring& wstrType)
{
    KLSTD_CHK(pData, pData && pData->DoesExist(c_szwSSP_Type));
    ;
    std::basic_ostringstream<wchar_t> os;
    os  << c_szwSSP_Marker;
    ParamsNames names;
    pData->GetNames(names);
    for(size_t i = 0; i < names.size(); ++i)
    {
        const std::wstring& wstrValue = KLPAR::GetStringValue(pData, names[i]);
        os << names[i] << L"=\"" << wstrValue << L"\"; ";
    };
    wstrType = os.str();
};


static bool KLPRSS_CheckType(const wchar_t* szwType, const wchar_t* szwPrefix, int nPrefix)
{
    KLSTD_ASSERT(szwPrefix && nPrefix > 0);
    if(!szwType || !szwType[0] || wcslen(szwType) < (unsigned)nPrefix)
        return false;
    return (wcsncmp(szwType, szwPrefix, nPrefix)==0);
};

KLCSKCA_DECL bool KLPRSS_IsSSTypeGlobal(const wchar_t* szwType)
{
    return KLPRSS_CheckType(
                        szwType,
                        KLPRSS_GT_PREFIX,
                        KLSTD_COUNTOF(KLPRSS_GT_PREFIX) - 1);
};

KLCSKCA_DECL std::wstring KLPRSS_MakeTypeG(const wchar_t* szwTypeName)
{    
    std::basic_ostringstream<wchar_t> os;    
    os  << c_szwSSP_Marker
        << c_szwSSP_Type << L"=\"" << szwTypeName << L"\"; ";
    return os.str();
};

KLCSKCA_DECL std::wstring KLPRSS_MakeTypeP(
                                        const wchar_t*      szwTypeName,
                                        const std::wstring& wstrProduct,
                                        const std::wstring& wstrVersion)
{
    std::basic_ostringstream<wchar_t> os;    
    os  << c_szwSSP_Marker
        << c_szwSSP_Type << L"=\"" << szwTypeName << L"\"; "
        << c_szwSSP_Product << L"=\"" << wstrProduct << L"\"; "
        << c_szwSSP_Version << L"=\"" << wstrVersion << L"\"; ";
    return os.str();
};

KLCSKCA_DECL bool KLPRSS_IsSSTypeProductSpecific(const wchar_t* szwType)
{
    return KLPRSS_CheckType(
                        szwType,
                        KLPRSS_PT_PREFIX,
                        KLSTD_COUNTOF(KLPRSS_PT_PREFIX) - 1);
};

KLCSKCA_DECL void KLPRSS_ExpandType(const std::wstring& wstType, KLPAR::Params** ppOutData)
{
    CNameConvertor conv; 
    CAutoPtr<Params> pData;
  
    conv.convert(wstType, &pData);  
    pData.CopyTo(ppOutData);  
};

KLCSKCA_DECL void KLPRSS_ResolveNames(KLPAR::Params* pInData, KLPAR::Params** ppOutData)
{
    KLSTD_CHKINPTR(pInData);
    KLSTD_CHKOUTPTR(ppOutData);
    ;
    CAutoPtr<Params> pResult;
    ParamsNames vecNames;
    pInData->GetNames(vecNames);
    KLPAR_CreateParams(&pResult);
    for(ParamsNames::iterator it=vecNames.begin(); it != vecNames.end(); ++it)
    {
        CAutoPtr<StringValue> p_wstrValue;
        KLPAR::GetValue(pInData, *it, &p_wstrValue);
        std::wstring wstrValue;
        KLSTD_ASSERT_THROW(p_wstrValue->GetValue() && p_wstrValue->GetValue()[0]);
        if(p_wstrValue->GetValue()[0] == c_chwSSP_Marker)
           KLPRSS_ResolveValue(&p_wstrValue->GetValue()[1], pInData, wstrValue);
        else
            wstrValue=p_wstrValue->GetValue();
        CAutoPtr<StringValue> p_wstrValue2;
        KLPAR::CreateValue(wstrValue.c_str(), &p_wstrValue2);
        pResult->AddValue(*it, p_wstrValue2);
    };
    pResult.CopyTo(ppOutData);
};


static bool GetGlobalStores(
                                    const std::wstring& wstrType,
                                    KLPAR::Params*      pInData,
                                    std::wstring&       wstrResult)
{
    bool bResult=MapGlobalSstypeToFspath(wstrType, wstrResult);

    if(pInData->DoesExist(c_szwSSP_RelPath))
    {            
        std::wstring wstrRelPath=GetStringValue(pInData, c_szwSSP_RelPath);
        std::wstring wstrTmp=wstrResult;
        wstrResult.clear();//=L"";
        KLSTD_PathAppend(wstrTmp, wstrRelPath, wstrResult, true);
    };
    return bResult;
};

static bool GetProductStores(
                              const std::wstring&   wstrType,
                              const std::wstring&   wstrProduct,
                              const std::wstring&   wstrVersion,
                              KLPAR::Params*        pInData,
                              std::wstring&         wstrResult)
{
    bool bResult=MapProductSstypeToFspath(
                            wstrType,
                            wstrProduct,
                            wstrVersion,
                            wstrResult);

    if(pInData->DoesExist(c_szwSSP_RelPath))
    {            
        std::wstring wstrRelPath=GetStringValue(pInData, c_szwSSP_RelPath);
        std::wstring wstrTmp=wstrResult;
        wstrResult.clear();//=L"";
        KLSTD_PathAppend(wstrTmp, wstrRelPath, wstrResult, true);
    };
    return bResult;
};

KLCSKCA_DECL void KLPRSS_ResolveValue(
                         const std::wstring&    wstrValue,
                         KLPAR::Params*         pInData,
                         std::wstring&          wstrResult)
{
    if(KLPRSS_IsSSTypeGlobal(wstrValue.c_str()))
    {
        if(!GetGlobalStores(wstrValue, pInData, wstrResult))
            KLERR_MYTHROW0(ERR_BADINSTALL);
    }
    else
    if(KLPRSS_IsSSTypeProductSpecific(wstrValue.c_str()))
    {
        const std::wstring wstrProduct=GetStringValue(pInData, c_szwSSP_Product);
        const std::wstring wstrVersion=GetStringValue(pInData, c_szwSSP_Version);
        if(!GetProductStores(wstrValue, wstrProduct, wstrVersion, pInData, wstrResult))
            KLERR_MYTHROW0(ERR_BADINSTALL);
    }
    else
        KLSTD_THROW(STDE_BADFORMAT);
};
