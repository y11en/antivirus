#include "std/base/klbase.h"
#include "kca/prss/settingsstorage.h"
#include "kca/prss/errors.h"
#include "kca/prss/store.h"
#include "std/par/paramsi.h"
#include "kca/prss/ss_bulkmode.h"
#include <common/measurer.h>
#include <set>
#include <list>
#include <map>

#include "kca/prss/helpersi.h"

#include "nameconvertor.h"
#include "std/base/klbaseqi_imp.h"
#include "./ss_bulkmodeimp.h"

using namespace std;
using namespace KLSTD;
using namespace KLPAR;
using namespace KLERR;
using namespace KLPRSS;

#define KLCS_MODULENAME L"KLPRSS"


namespace KLPRSS
{
    bool KLSTD_FASTCALL CheckName(const std::wstring& name, bool bCheckEmpty = true);

    KLCSKCA_DECL void SsBulkMode_FastWriteSection(
                        KLSTD::KLBaseQI*        pSs, 
                        const wchar_t*          szwProduct, 
                        const wchar_t*          szwVersion,
                        const wchar_t*          szwSection,
                        SsBulkMode_WriteMode    nMode,
                        KLPAR::Params*          pSection)
    {
    KL_TMEASURE_BEGIN(L"SsBulkMode_FastWriteSection", 3)
        KLSTD_CHKINPTR(pSs);
        KLSTD_CHKINPTR(pSection);
        KLSTD_CHK(szwProduct,   szwProduct && CheckName(szwProduct));
        KLSTD_CHK(szwVersion,   szwVersion && CheckName(szwVersion));
        KLSTD_CHK(szwSection,   szwSection && CheckName(szwSection));
        KLSTD_CHK(nMode,        BMSS_Update == nMode    || 
                                BMSS_Add == nMode       || 
                                BMSS_Replace == nMode   || 
                                BMSS_Delete == nMode    || 
                                BMSS_Clear == nMode         ); 
        ;
        CAutoPtr<KLPRSS::SsBulkMode> pBulkSs;
        pSs->QueryInterface(KLSTD_IIDOF(KLPRSS::SsBulkMode), (void**)&pBulkSs);
        KLSTD_CHK(pSs, pBulkSs);
        ;
        CAutoPtr<ArrayValue> pArray = KLPAR::CreateArrayValue();
        {
            CAutoPtr<ParamsValue> pEntry;
            CreateValue(pSection, &pEntry);
            pArray->SetSize(1);
            pArray->SetAt(0, pEntry);
        }
        ;
        section_name_t name;
        sections_t sections;
        sections.m_vecProducts.push_back(szwProduct);
        sections.m_vecVersions.push_back(szwVersion);
        sections.m_vecSections.push_back(szwSection);
        name.m_nProduct = 0;
        name.m_nVersion = 0;
        name.m_nSection = 0;
        name.m_dwFlags = AVP_dword(nMode);
        sections.m_vecNames.push_back(name);
        ;
        pBulkSs->WriteSections(sections, pArray);
    KL_TMEASURE_END()
    };    
};
