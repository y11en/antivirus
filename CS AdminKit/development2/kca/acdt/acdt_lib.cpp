#include <kca/acdt/acdt_lib.h>
#include <kca/prss/helpersi.h>

#include <sstream>

#define KLCS_MODULENAME L"KLACDTLIB"

namespace KLACDTAPI
{
    const wchar_t * c_szwNamesWks[] = 
    {
        c_szwProductWksAny,
        c_szwProductWks60,
        c_szwProductWks46,
        c_szwProductWks50,
        NULL        
    };

    const wchar_t * c_szwNamesFs[] = 
    {
        c_szwProductFsAny,
        c_szwProductFs60,
        c_szwProductFs50,
        NULL        
    };

    KLPAR::ParamsPtr GetProductInfo(
                        KLPAR::ParamsPtr    pData_, 
                        const wchar_t*      szwProduct, 
                        bool                bExceptIfNotFound)
    {
        KLSTD_CHKINPTR(pData_);
        KLSTD_CHK(szwProduct, szwProduct && szwProduct[0]);
        KLPAR::ParamsPtr    pData = KLPAR::GetParamsValue(pData_, c_szwStateProducts);

        KLPAR::ParamsPtr pResult;
        bool bUseAI = false;
        const wchar_t * const * pArray = NULL;
        if( wcscmp(szwProduct, c_szwProductWksAny) == 0 )
        {
            bUseAI = true;
            pArray = c_szwNamesWks;
        }
        else if( wcscmp(szwProduct, c_szwProductFsAny) == 0 )
        {
            bUseAI = true;
            pArray = c_szwNamesFs;
        }
        else
        {            
            KLERR_IGNORE(pResult = KLPAR::GetParamsValue(pData, szwProduct));
        };
        if(bUseAI)
        {
            for(size_t i = 0; !pResult && pArray[i]; ++i )
            {
                KLERR_IGNORE(pResult = KLPAR::GetParamsValue(pData, pArray[i]));
            };
        };
        if(!pResult && bExceptIfNotFound)
        {
            KLSTD_THROW(KLSTD::STDE_NOENT);
        };
        return pResult;
    };
    
    std::wstring GetSavedEventLog(
                        KLPAR::ParamsPtr    pData_, 
                        const wchar_t*      szwEventLog, 
                        bool                bExceptIfNotFound)
    {
        KLSTD_CHKINPTR(pData_);
        KLSTD_CHK(szwEventLog, szwEventLog && szwEventLog[0]);
        KLPAR::ParamsPtr    pData = KLPAR::GetParamsValue(pData_, c_szwStateEventLogs);

        std::wstring wstrResult;
        KLERR_IGNORE(wstrResult = KLPAR::GetStringValue(pData, szwEventLog));
        if(wstrResult.empty() && bExceptIfNotFound)
        {
            KLSTD_THROW(KLSTD::STDE_NOENT);
        };
        return wstrResult;
    };

    static void ReplaceToken( 
                    std::wstring & str, 
                    const std::wstring & strToken, 
                    const std::wstring & strValue )
    {
        std::wstring::size_type pos = str.find( strToken);
    
        while( pos != std::wstring::npos )
        {
            str.replace( pos, strToken.size(), strValue);
            pos = str.find( strToken, pos + strValue.size());
        };
    };

    KLSTD::CAutoPtr<KLPAR::Value> GetProductVal(
                        KLPAR::ParamsPtr    pData, 
                        const wchar_t*      szwProduct, 
                        const wchar_t*      szwValue,
                        bool                bExceptIfNotFound)
    {
        KLSTD_CHKINPTR(pData);
        KLSTD_CHK(szwProduct, szwProduct && szwProduct[0]);
        KLSTD_CHK(szwValue, szwValue && szwValue[0]);
        KLPAR::ParamsPtr pInfo = GetProductInfo(pData, szwProduct, bExceptIfNotFound);
        KLSTD::CAutoPtr<KLPAR::Value> pResult;
        if(pInfo)
            pInfo->GetValueNoThrow(szwValue, &pResult);
        if(!pResult && bExceptIfNotFound)
        {
            KLSTD_THROW(KLSTD::STDE_NOENT);
        };
        return pResult;
    };

    std::wstring MakeRemoteFileName(const wchar_t* szwName)
    {
        KLSTD_CHK(szwName, szwName && szwName[0]);
        std::wstring wstrRes = szwName;
        ReplaceToken(wstrRes, L":", L"$");
        return wstrRes;
    };

    long GetNumber(const wchar_t* szwString)
    {
        return _wtol(szwString);
    };

    const wchar_t c_szwFileDN[] = L"DN";
    const wchar_t c_szwFileSize[] = L"Size";
    const wchar_t c_szwRemotePath[] = L"path";

    void DecodeFilePath(const wchar_t* szwFile, std::wstring* pwstrDN, AVP_qword* pqwSize)
    {
        if(!szwFile || !szwFile[0])
            return;
        KLPAR::ParamsPtr pData;
        KLPRSS_ExpandType(szwFile, &pData);
        if(pwstrDN)
            *pwstrDN = KLPAR::GetStringValue(pData, c_szwFileDN);
        if(pqwSize)
        {
            std::wstring wstrSize = KLPAR::GetStringValue(pData, c_szwFileSize);
            *pqwSize = (AVP_qword)_wtoi64(wstrSize.c_str());
        };
    };

    std::wstring DecodeFilePath2(const wchar_t* szwFile)
    {
        if(!szwFile || !szwFile[0])
            return L"";
        KLPAR::ParamsPtr pData;
        KLPRSS_ExpandType(szwFile, &pData);
        return  KLPAR::GetStringValue(pData, c_szwRemotePath);
    };

    std::wstring EncodeFilePath(const wchar_t* szwFile, const wchar_t* szwDN, AVP_qword qwSize)
    {
        if(!szwFile || !szwFile[0])
            return L"";
        std::vector<wchar_t> vecBuffer;
        vecBuffer.resize(128);
        _ui64tow(qwSize, &vecBuffer[0], 10);

        std::wstring wstrRes;
        KLPAR::ParamsPtr pData;
        KLPAR::param_entry_t par[] = 
        {
            KLPAR::param_entry_t(KLPRSS::c_szwSSP_Type,   L"file"),
            KLPAR::param_entry_t(c_szwFileDN,     szwDN),
            KLPAR::param_entry_t(c_szwFileSize,   &vecBuffer[0]),
            KLPAR::param_entry_t(c_szwRemotePath, szwFile)
        };
        KLPAR::CreateParamsBody(par, KLSTD_COUNTOF(par), &pData);

        KLPRSS_UnexpandType(pData, wstrRes);
        return wstrRes;
    };

    std::wstring Int2Str(int nX)
    {
        std::wostringstream os;
        os << nX;
        return os.str();
    };

    int Str2Int(const wchar_t* szwX)
    {
        int nX = 0;
        bool bRes = false;
        if(szwX && szwX[0])
        {
            wchar_t* pEnd = NULL;
            long lResult = wcstol(szwX, &pEnd, 10);
            if(!pEnd || !pEnd[0])
            {
                nX = lResult;
                bRes = true;
            };
        };
        if(!bRes)
        {
            KLSTD_THROW(KLSTD::STDE_BADFORMAT);
        };
        return nX;
    };
};
