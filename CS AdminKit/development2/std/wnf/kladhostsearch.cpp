#include<std/base/klstd.h>
#include<std/win32/klos_win32v50.h>
#include<std/win32/klos_win32_errors.h>
#include<std/wnf/kladhostsearch.h>
#include <sstream>
#include <common/bin2hex.h>

#include <Iads.h>
#include <Adshlp.h>
#include <Dsgetdc.h>

#define KLCS_MODULENAME L"KLWNF"

using namespace KLSTD;
using namespace KLWNF;

namespace KLWNF
{
    void _GetADsObject(LPCWSTR pszObjectName, REFIID riid, void** ppIObject);
};

namespace
{
    void Column2String(
                IDirectorySearch*   pISearch, 
                ADS_SEARCH_HANDLE   hSearch,
                const wchar_t*      szwName,
                std::wstring&       wstrResult)
    {
		ADS_SEARCH_COLUMN col = {0};
        
        KLSTD_HRCHK(pISearch->GetColumn(hSearch, const_cast<wchar_t*>(szwName), &col));

        KLERR_TRY
            KLSTD_ASSERT_THROW(col.pADsValues);
            switch(col.pADsValues->dwType)
            {
                case ADSTYPE_CASE_EXACT_STRING:
                case ADSTYPE_CASE_IGNORE_STRING:
                case ADSTYPE_DN_STRING:
                    wstrResult = KLSTD::FixNullString((LPCWSTR)
                                col.pADsValues->CaseIgnoreString);
                    break;
                case ADSTYPE_OCTET_STRING: //guid
                        KLSTD_ASSERT_THROW(col.pADsValues);
                        KLSTD_ASSERT_THROW(col.pADsValues->OctetString.lpValue);
                        wstrResult = MakeHexDataW(
                                col.pADsValues->OctetString.lpValue,
                                col.pADsValues->OctetString.dwLength);
                        if(!wstrResult.empty())
                        {
                            std::vector<wchar_t> vecData(wstrResult.begin(), wstrResult.end());
                            CharLowerBuffW(&vecData[0], vecData.size());
                            wstrResult.assign(&vecData[0], vecData.size());
                        };
                    break;
                default:
                    KLSTD_THROW(KLSTD::STDE_BADFORMAT);
            };                
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(4, pError);
        KLERR_FINALLY
            pISearch->FreeColumn(&col);
            KLERR_RETHROW();
        KLERR_ENDTRY            
    };

    std::wstring AcquireDefaultNamingContext()
    {
        std::wstring wstrResult = L"LDAP://";

		CComPtr<IADs>			pRootDSE;
		CComVariant				varConfigNC;
		
		_GetADsObject(L"LDAP://RootDSE", __uuidof(IADs), (void**)&pRootDSE);
		KLSTD_HRCHK(pRootDSE->GetInfo());
		KLSTD_HRCHK(pRootDSE->Get(L"defaultNamingContext",&varConfigNC));
        wstrResult += KLSTD::FixNullString(varConfigNC.bstrVal);

        return wstrResult;
    };

    class CAdHostSearchImpl
        :   public KLSTD::KLBaseImpl<AdHostSearch>
    {
    public:
	    CAdHostSearchImpl()
            :   m_wstrDefaultNamingContext(AcquireDefaultNamingContext())
	    {
            ;
	    };

	    ~CAdHostSearchImpl()
	    {
            ;
	    };

        virtual void FindHostAD(const std::wstring& hostname, ADObjectInfo& info)
        {
            CComPtr<IDirectorySearch> pISearch;
            ADS_SEARCH_HANDLE hSearch = NULL;
            KLERR_TRY
			    _GetADsObject(
                        m_wstrDefaultNamingContext.c_str(), 
                        __uuidof(IDirectorySearch), 
                        (void**)&pISearch);
			    
                ADS_SEARCHPREF_INFO prefInfo[2];
			    prefInfo[0].dwSearchPref = ADS_SEARCHPREF_SEARCH_SCOPE;
			    prefInfo[0].vValue.dwType = ADSTYPE_INTEGER;
			    prefInfo[0].vValue.Integer = ADS_SCOPE_SUBTREE;
			    prefInfo[1].dwSearchPref = ADS_SEARCHPREF_PAGESIZE;
			    prefInfo[1].vValue.dwType = ADSTYPE_INTEGER;
			    prefInfo[1].vValue.Integer = 99;
			    KLSTD_HRCHK(pISearch->SetSearchPreference(prefInfo, KLSTD_COUNTOF(prefInfo)));

			    LPCWSTR pszAttr[]={
                    L"name", 
                    L"sAMAccountName", 
                    L"dNSHostName", 
                    L"distinguishedName",
                    L"objectGUID"
			    };

			    
			    const size_t nColumns = KLSTD_COUNTOF(pszAttr);
                std::wostringstream os;
            
                os << L"(&"
                        //L"(objectClass=computer)"
                        L"(objectCategory=computer)"
                        L"(userAccountControl:1.2.840.113556.1.4.803:=0)"
                        L"(dNSHostName=" << hostname << L")"
                      L")";

                KLSTD_HRCHK(pISearch->ExecuteSearch(
                                const_cast<wchar_t*>(os.str().c_str()), 
                                (LPWSTR*)&pszAttr[0], 
                                nColumns, 
                                &hSearch));

                HRESULT hr = pISearch->GetNextRow(hSearch);
                KLSTD_HRCHK(hr);
                if(hr != S_OK)
                {
                    KLSTD_THROW(KLSTD::STDE_NOTFOUND);
                };
                Column2String(pISearch, hSearch, pszAttr[0], info.name);
                Column2String(pISearch, hSearch, pszAttr[1], info.accountName);
                Column2String(pISearch, hSearch, pszAttr[2], info.dNSHostName);
                Column2String(pISearch, hSearch, pszAttr[3], info.distinguishedName);
                Column2String(pISearch, hSearch, pszAttr[4], info.guid);

                if(!info.accountName.empty() && L'$' == info.accountName[info.accountName.size()-1])
                    info.accountName.resize(info.accountName.size()-1);

                KLSTD_TRACE4(
                    4,  L"FindHostAD: Host found, "
                            L"accountName='%ls', dNSHostName='%ls', "
                            L"distinguishedName='%ls', info.guid='%ls'\n",
                        info.accountName.c_str(),
                        info.dNSHostName.c_str(),
                        info.distinguishedName.c_str(),
                        info.guid.c_str());

            KLERR_CATCH(pError)
                KLERR_SAY_FAILURE(4, pError);
            KLERR_FINALLY
                if(pISearch && hSearch)
                {
                    pISearch->CloseSearchHandle(hSearch);
                    hSearch = NULL;
                };
                KLERR_RETHROW();
            KLERR_ENDTRY
        };
        const std::wstring m_wstrDefaultNamingContext;
    };
};

void KLWNF_CreateAdHostSearch(KLWNF::AdHostSearch** pp)
{
    KLSTD_CHKOUTPTR(pp);
    KLSTD::CAutoPtr<KLWNF::AdHostSearch> pAdHostSearch;
    pAdHostSearch.Attach(new CAdHostSearchImpl);
    KLSTD_CHKMEM(pAdHostSearch);
    pAdHostSearch.CopyTo(pp);
};

