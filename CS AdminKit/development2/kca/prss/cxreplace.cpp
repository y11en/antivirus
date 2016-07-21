#include <std/base/klstd.h>
#include <std/par/params.h>
#include <std/par/helpersi.h>
#include <kca/prss/store.h>
#include <kca/prss/errors.h>
#include <kca/prss/helpersi.h>

#define KLCS_MODULENAME L"KLPRSS"

using namespace std;
using namespace KLSTD;
using namespace KLPAR;
using namespace KLERR;
using namespace KLPRSS;

namespace
{
    class CSelectValueCallback
        :   public KLPAR::SelectValueCallback
        ,   public KLPAR::NodeActionCallback
    {
    public:
        CSelectValueCallback(bool bIgnoreUpdateConflicts)
            :   m_bIgnoreUpdateConflicts(bIgnoreUpdateConflicts)
        {;};

        virtual ~CSelectValueCallback()
        {
            ;
        };

        KLSTD_INTERAFCE_MAP_BEGIN(KLPAR::SelectValueCallback)
            KLSTD_INTERAFCE_MAP_ENTRY(KLPAR::NodeActionCallback)
        KLSTD_INTERAFCE_MAP_END()

        unsigned long AddRef(){return 0;};

	    unsigned long Release(){return 0;};

        /*
            vecValues[0] is ss and vecValues[1] is argument
        */
        KLSTD::CAutoPtr<Value> DoSelectValue(
                        const wchar_t*                      szwName,
                        const std::vector<KLPAR::Value*>&   vecValues)
        {
            KLSTD::CAutoPtr<Value> pResult;
            if(vecValues[1])
                pResult = vecValues[1];
            else
                pResult = vecValues[0];
            return pResult;
        };

        bool DoSelectAction(
                        const wchar_t*                      szwName,
                        const std::vector<KLPAR::Params*>&  vecNodes)
        {
            bool bResult = true;
            if(!vecNodes[0] && vecNodes[1])
            {
                if(!m_bIgnoreUpdateConflicts)
                {
                    KLERR_THROW1(L"KLPRSS", KLPRSS::NOT_EXIST, szwName);
                }
                else
                    bResult = false;
            };
            return bResult;
        };
    protected:
        bool    m_bIgnoreUpdateConflicts;
    };
};

KLCSKCA_DECL void KLPRSS_SsMerge_ReplaceLeavesUpdateNodes(
        const wchar_t*  szwSsPath,
        const wchar_t*  szwProduct,
        const wchar_t*  szwVersion,
        const wchar_t*  szwSection,
        KLPAR::Params*  pData,
        bool            bIgnoreUpdateConflicts,
        long            lSsAccessTimeout)
{
    KLSTD_CHK(szwSsPath, szwSsPath && szwSsPath[0]);
    KLSTD_CHK(szwProduct, szwProduct && szwProduct[0]);
    KLSTD_CHK(szwVersion, szwVersion && szwVersion[0]);
    KLSTD_CHK(szwSection, szwSection && szwSection[0]);
    KLSTD_CHK(lSsAccessTimeout, lSsAccessTimeout >= 0 || lSsAccessTimeout == KLSTD_INFINITE);
    KLSTD_CHKINPTR(pData);
    ;
    CAutoPtr<Storage> pStore;
    KLPRSS::CreateStorage(
                KLPRSS_TypeToFileSystemPath(szwSsPath),
                CF_OPEN_EXISTING,
                AF_READ|AF_WRITE,
                &pStore);
    pStore->trans_begin(true, lSsAccessTimeout, true);
    KLERR_TRY
        const wchar_t* path[]=
        {
            szwProduct,
            szwVersion ,
            szwSection,
            NULL
        };
        KLPAR::ParamsPtr pSsData, pResult;
        pStore->region_load(path, true);
        pStore->entry_read(&pSsData);
        std::vector<KLPAR::Params*> vecParams;
        vecParams.resize(2);
        vecParams[0] = pSsData;
        vecParams[1] = pData;
        CSelectValueCallback callback(bIgnoreUpdateConflicts);
        KLPAR_CreateParams(&pResult);
        KLPAR::ProcessParamsRecursively(vecParams, &callback, pResult);
        pStore->entry_write(CF_CREATE_ALWAYS, pResult);
        KLPRSS::SS_OnSaving(pStore);
        pStore->trans_end(true);
    KLERR_CATCH(pError)
        KLERR_SAY_FAILURE(1, pError);
        KLERR_IGNORE(pStore->trans_end(false));
        KLERR_RETHROW();
    KLERR_ENDTRY
};
