/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	ss_modifyss.cpp
 * \author	Andrew Kazachkov
 * \date	13.08.2007 11:34:20
 * \brief	
 * 
 */

#include "std/base/klstd.h"
#include "std/err/klerrors.h"
#include "std/base/klbaseqi_imp.h"
#include "std/par/params.h"
#include "std/par/helpersi.h"
#include "std/par/par_conv.h"
#include "kca/prss/settingsstorage.h"
#include "./ss_smartwrite.h"
#include "kca/prss/helpersi.h"

#include <common/measurer.h>



#define KLCS_MODULENAME L"KLPRSS"

using namespace KLPAR;

namespace KLPRSS
{
    /*
        We must modify settings if attribute 'mnd' set to false (or deleted) 
        in new for some value
    */

    class CModifySsSelectValueCallback
        :   public KLPAR::SelectValueCallback
        ,   public KLPAR::NodeActionCallback
    {
    public:
        CModifySsSelectValueCallback()
            :   m_bModified(false)
        {;};
        bool m_bModified;
    protected:
        KLSTD_INTERAFCE_MAP_BEGIN(KLPAR::SelectValueCallback)
            KLSTD_INTERAFCE_MAP_ENTRY(KLPAR::NodeActionCallback)
        KLSTD_INTERAFCE_MAP_END()
        unsigned long AddRef(){return 0;};
	    unsigned long Release(){return 0;};

    protected:
        KLSTD::CAutoPtr<Value> DoSelectValue(
                        const wchar_t*                      szwName,
                        const std::vector<KLPAR::Value*>&   vecValues)
        {
            KLSTD_ASSERT(vecValues.size() == 2u);
            KLSTD::CAutoPtr<Value> pResult;
            if(vecValues[1])
                pResult = vecValues[1];
            else
                pResult = vecValues[0];
            return pResult;
        };
    public:
        bool DoSelectAction(
                        const wchar_t*                      szwName,
                        const std::vector<KLPAR::Params*>&  vecNodes)
        {
            KLSTD_ASSERT(vecNodes.size() == 2u);

            if(wcscmp(c_szwSPS_Value, szwName) != 0)
            {
                KLPAR::ParamsPtr pParOld = vecNodes[0], pParNew = vecNodes[1];

                if( pParOld != NULL  && !pParNew ) //deleted
                {
                    KLSTD_TRACE1(3, L"Value '%ls' seems to be deleted\n", szwName);
                    m_bModified = true;
                }
                else if( !pParOld ) // created
                {
                    ;// do nothing
                }
                else
                {
                    KLSTD_ASSERT(pParNew);
                    KLSTD_ASSERT(pParOld);

                    bool bNewMandatory = ParVal(GetVal(pParNew, KLPRSS::c_szwSPS_Mandatory), false);
                    bool bOldMandatory = ParVal(GetVal(pParOld, KLPRSS::c_szwSPS_Mandatory), false);

                    if( bNewMandatory != bOldMandatory && !bNewMandatory ) //unlocked
                    {
                        KLSTD_TRACE3(
                                    3, 
                                    L"Mandatory for value '%ls' seems to be changed from %u to %u\n", 
                                    szwName, 
                                    bOldMandatory, 
                                    bNewMandatory);
                        m_bModified = true;
                    };
                };
            };
            return !m_bModified;
        };
    };

    class CCheckLocksExistence
        :   public KLPAR::SelectValueCallback
    {
    public:
        CCheckLocksExistence()
            :   m_nHasLocks(0)
        {;};
        size_t m_nHasLocks;
    protected:
        KLSTD_INTERAFCE_MAP_BEGIN(KLPAR::SelectValueCallback)
        KLSTD_INTERAFCE_MAP_END()
        unsigned long AddRef(){return 0;};
	    unsigned long Release(){return 0;};

    protected:
        KLSTD::CAutoPtr<Value> DoSelectValue(
                        const wchar_t*                      szwName,
                        const std::vector<KLPAR::Value*>&   vecValues)
        {
            KLSTD_ASSERT(vecValues.size() == 1u);
            KLSTD::CAutoPtr<Value> pResult;
            ;
            if(wcscmp(c_szwSPS_Mandatory, szwName) == 0)
            {
                bool bNewMandatory = ParVal(vecValues[0], false);
                if(bNewMandatory)
                    ++m_nHasLocks;
            };
            ;
            pResult = vecValues[0];
            return pResult;
        };
    };

    
    bool IfSectionContainsLocks(KLPAR::ParamsPtr pSection)
    {
        bool bResult = false;
    KL_TMEASURE_BEGIN(L"IfSectionContainsLocks", 3)
        if(pSection)
        {
            KLPAR::ParamsPtr pFoo;
            KLPAR_CreateParams(&pFoo);
            CCheckLocksExistence val;
            std::vector<KLPAR::Params*> vecData;
            vecData.resize(1);
            vecData[0] = pSection;
            KLPAR::ProcessParamsRecursively(vecData, &val, pFoo, false);
            KLSTD_TRACE1(4, L"Found %u locks\n", val.m_nHasLocks);
            bResult = (val.m_nHasLocks != 0);
        };
    KL_TMEASURE_END()
        return bResult;
    };
};

using namespace KLPRSS;

KLCSKCA_DECL bool KLPRSS_IfMustModifySettings(KLPAR::Params* pOldPolSection, KLPAR::Params* pNewPolSection)
{
    bool bResult = false;
    if( pOldPolSection != NULL && NULL == pNewPolSection)
    {
        //section deleted from policy
        KLSTD_TRACE0(3, L"Section seems to be deleted from policy\n");
        bResult = IfSectionContainsLocks(pOldPolSection); //must modify if section contains locks
    }
    else if( NULL == pOldPolSection ) //new section in policy
    {
        ;// do nothing
    }
    else
    {
        KLPAR::ParamsPtr pFoo;
        KLPAR_CreateParams(&pFoo);
        CModifySsSelectValueCallback val;
        std::vector<KLPAR::Params*> vecData;
        vecData.resize(2);
        vecData[0] = pOldPolSection;
        vecData[1] = pNewPolSection;
        val.DoSelectAction(L"/", vecData);
        if(!val.m_bModified)
            KLPAR::ProcessParamsRecursively(vecData, &val, pFoo, false);
        bResult = val.m_bModified;
    };
    return bResult;
};

