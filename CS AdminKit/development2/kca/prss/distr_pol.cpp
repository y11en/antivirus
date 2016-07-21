/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	distr_pol.cpp
 * \author	Andrew Kazachkov
 * \date	09.09.2003 11:00:50
 * \brief	
 * 
 */

#include "std/err/klerrors.h"
#include "std/conv/klconv.h"
#include "kca/prss/settingsstorage.h"
#include "kca/prss/errors.h"
#include "kca/prss/store.h"
#include "kca/prci/simple_params.h"
#include "kca/prss/helpersi.h"
#include "./ss_bulkmodeimp.h"
#include <sstream>

#include <std/trc/trace.h>
#include "common/measurer.h"

#define KLCS_MODULENAME L"KLPRSS"

using namespace std;
using namespace KLSTD;
using namespace KLPAR;
using namespace KLERR;
using namespace KLPRSS;

static bool KLPRSS_PolIsLocked(KLPAR::Params* pPol)
{
    bool bLocked=pPol->DoesExist(c_szwSPS_Locked)
                        ?   GetBoolValue(pPol, c_szwSPS_Locked)
                        :   false;
    return bLocked;
}


static Value::Types KLPRSS_PolGetType(KLPAR::Params* pPol)
{
    return pPol->GetValue2(c_szwSPS_Value, true)->GetType();
};

static void KLPRSS_PolGetNames(KLPAR::Params* pPol, ParamsNames& names)
{
    CAutoPtr<Params> pData=GetParamsValue(pPol, c_szwSPS_Value);
    names.clear();
    if(pData)
        pData->GetNames(names);
};

static bool KLPRSS_PolGetEntry(
                                KLPAR::Params*  pPol,
                                std::wstring&   wstrName,
                                KLPAR::Params** ppEntry,
                                bool            bThrow)
{
    KLERR_TRY
        CAutoPtr<Params> pData=GetParamsValue(pPol, c_szwSPS_Value);    
        KLSTD_ASSERT_THROW(pData!=NULL);
        GetParamsValue(pData, wstrName).CopyTo(ppEntry);
    KLERR_CATCH(pError)
        if(bThrow)
            KLERR_RETHROW();
        return false;
    KLERR_ENDTRY
    return true;
};

static bool KLPRSS_PolPutEntry(
                                KLPAR::Params*  pPol,
                                const wchar_t*  szwName,
                                KLPAR::Params*  pEntry,
                                bool            bThrow)
{
    KLERR_TRY
        CAutoPtr<Params> pData=GetParamsValue(pPol, c_szwSPS_Value);
        KLSTD_ASSERT_THROW(pData!=NULL);
        CAutoPtr<ParamsValue> pParamsValue;
        CreateValue(pEntry, &pParamsValue);
        pData->ReplaceValue(szwName, pParamsValue);
    KLERR_CATCH(pError)
        if(bThrow)
            KLERR_RETHROW();
        return false;
    KLERR_ENDTRY
    return true;
};


static Value::Types KLPRSS_PolGetValue(KLPAR::Params* pPol);

namespace KLPRSS
{

    enum{
        DPA_DO_NOTHING,
        DPA_DISTRIBUTE_LOCK,
        DPA_DISTRIBUTE_UNLOCK,
        DPA_ERROR
    };

    struct dp_value_t
    {
        dp_value_t(const wchar_t* szwName, KLPAR::Value* pValue)
            :   m_szwName(szwName)
            ,   m_pValue(pValue)
        {
            KLSTD_ASSERT(m_szwName && m_szwName[0]);
        };
        dp_value_t()
            :   m_szwName(NULL)
            ,   m_pValue(NULL)
        {
            ;
        };

        dp_value_t(const dp_value_t& x)
            :   m_szwName(x.m_szwName)
            ,   m_pValue(x.m_pValue)
        {
            ;
        };

        const wchar_t*  m_szwName;
        KLPAR::Value*   m_pValue;
    };
};

static bool DistributePolicy(
                        KLPAR::Params*     pSrc,
                        KLPAR::Params*     pDst,
                        long            lSrcPolicyId,
                        long            lSrcPolicyGroupId);

static bool EnumerateParams(
                        KLPAR::Params*  pSrc,
                        KLPAR::Params*  pDst,
                        long            lSrcPolicyId,
                        long            lSrcPolicyGroupId)
{
    bool bWasChanged=false;
    if(pSrc)
    {
        ParamsNames names;
        pSrc->GetNames(names);
        for(ParamsNames::iterator it=names.begin(); it != names.end(); ++it)
        {            
            CAutoPtr<Params> pSrc2=GetParamsValue(pSrc, *it);
            CAutoPtr<Params> pDst2;
            bool bRealDst2=false;
            if(pDst)
            {
                if(pDst->DoesExist(*it) && pDst->GetValue2(*it, true)->GetType()==Value::PARAMS_T)
                {
                    pDst2=((ParamsValue*)pDst->GetValue2(*it, true))->GetValue();
                    bRealDst2=true;
                }
                else
                {
                    KLPAR_CreateParams(&pDst2);
                    CAutoPtr<BoolValue> p_bFalse;
                    CreateValue(false, &p_bFalse);
                    pDst2->ReplaceValue(c_szwSPS_Mandatory, p_bFalse);
                };
            };
            bool bChanged=DistributePolicy(pSrc2, pDst2, lSrcPolicyId, lSrcPolicyGroupId);            
            if(pDst && bChanged && !bRealDst2)
            {
                CAutoPtr<ParamsValue> pDst2Value;
                CreateValue(pDst2, &pDst2Value);
                pDst->ReplaceValue(*it, pDst2Value);
            };
            bWasChanged=bWasChanged || bChanged;
        };
    };
    return bWasChanged;
};


static void SetForAllChildren(
                              KLPAR::Params*                    pRoot,
                              const std::vector<dp_value_t>&    values)
{
    KLSTD_ASSERT(pRoot != NULL);
    KLSTD_CHKINPTR(pRoot);

    Value* pItemValue=pRoot->GetValue2(c_szwSPS_Value, true);
    KLSTD_CHKINPTR(pItemValue);

    switch(pItemValue->GetType())
    {
    case KLPAR::Value::PARAMS_T:
        {
            ParamsNames names;
            KLPRSS_PolGetNames(pRoot, names);
            for(ParamsNames::iterator it=names.begin(); it != names.end(); ++it)
            {
                CAutoPtr<Params> pChild;
                KLPRSS_PolGetEntry(pRoot, *it, &pChild, true);
                if(pChild)
                {
                    for(unsigned int i=0; i < values.size(); ++i)
                    {
                        const dp_value_t& entry=values[i];
                        if(entry.m_pValue)
                            pChild->ReplaceValue(entry.m_szwName, entry.m_pValue);
                        else
                            pChild->DeleteValue(entry.m_szwName);
                    };
                    SetForAllChildren(pChild, values);
                };

            };
        };
        break;
    case KLPAR::Value::ARRAY_T:
        {
            ArrayValue* pArrayValue=static_cast<ArrayValue*>(pItemValue);
            const int nItems=pArrayValue->GetSize();
            for(int j=0; j < nItems; ++j)            
            {
                CAutoPtr<Value> pNewItemValue;
                pArrayValue->GetAt(j, &pNewItemValue);
                if(!pNewItemValue || !KLPRSS_HasArrayItemPolicyFmt(pNewItemValue))
                    continue;
                KLPAR_CHKTYPE(pNewItemValue, PARAMS_T, L"");
                Params* pChild=((ParamsValue*)(Value*)pNewItemValue)->GetValue();
                if(pChild)
                {
                    for(unsigned int i=0; i < values.size(); ++i)
                    {
                        const dp_value_t& entry=values[i];
                        if(entry.m_pValue)
                            pChild->ReplaceValue(entry.m_szwName, entry.m_pValue);
                        else
                            pChild->DeleteValue(entry.m_szwName);
                    };
                    SetForAllChildren(pChild, values);
                };
            };
        }
        break;
    default:
        {
            for(unsigned int i=0; i < values.size(); ++i)
            {
                const dp_value_t& entry=values[i];
                if(entry.m_pValue)
                    pRoot->ReplaceValue(entry.m_szwName, entry.m_pValue);
                else
                    pRoot->DeleteValue(entry.m_szwName);
            };            
        };
        break;
    };
}

static bool ClearParentalLocks(KLPAR::Params* pData)
{
    bool bResult=false;
    if(!pData)
        return bResult;

    bool bLocked=KLPRSS_PolIsLocked(pData);
    if(bLocked)
    {
        std::vector<dp_value_t> values;
        values.reserve(4);
        CAutoPtr<BoolValue> p_bMandatory;
        CreateValue(false, &p_bMandatory);
        values.push_back(dp_value_t(c_szwSPS_Mandatory, p_bMandatory));
        values.push_back(dp_value_t(c_szwSPS_Locked, NULL));
        values.push_back(dp_value_t(c_szwSPS_LockedPolicyName, NULL));
        values.push_back(dp_value_t(c_szwSPS_LockedPolicyId, NULL));
        values.push_back(dp_value_t(c_szwSPS_LockedGroupName, NULL));
        values.push_back(dp_value_t(c_szwSPS_LockedGroupId, NULL));
        SetForAllChildren(pData, values);
        pData->DeleteValue(c_szwSPS_Locked, false);
        pData->DeleteValue(c_szwSPS_LockedPolicyName, false);
        pData->DeleteValue(c_szwSPS_LockedPolicyId, false);
        pData->DeleteValue(c_szwSPS_LockedGroupName, false);
        pData->DeleteValue(c_szwSPS_LockedGroupId, false);
        bResult=true;
    }
    else
    {
        Value::Types type=KLPRSS_PolGetType(pData);
        switch(type)
        {
        case Value::PARAMS_T:
            {
                ParamsNames names;
                KLPRSS_PolGetNames(pData, names);
                for(unsigned int i=0; i < names.size(); ++i)
                {
                    CAutoPtr<Params> pEntry;
                    KLPRSS_PolGetEntry(pData, names[i], &pEntry, false);
                    if(!pEntry)
                        continue;
                    bResult=ClearParentalLocks(pEntry) || bResult;
                };
            };
            break;
        case Value::ARRAY_T:
            {
                CAutoPtr<ArrayValue> pVal=GetArrayValue(pData, c_szwSPS_Value);
                if(pVal)
                {
                    const int c_nSize=pVal->GetSize();
                    for(int i=0; i < c_nSize; ++i)
                    {
                        CAutoPtr<ParamsValue> pEntry;
                        pVal->GetAt(i,  (Value**)&pEntry);
                        if(
                                !pEntry ||
                                pEntry->GetType() != Value::PARAMS_T ||
                                !pEntry->GetValue() ||
                                !KLPRSS_HasArrayItemPolicyFmt(pEntry))
                        {
                            continue;
                        };
                        bResult = ClearParentalLocks(pEntry->GetValue()) || bResult;
                    };
                };
            }
            break;
        };
    };
    return bResult;
};


static bool DistributePolicy(
                        KLPAR::Params*  pSrc,
                        KLPAR::Params*  pDst,
                        long            lSrcPolicyId,
                        long            lSrcPolicyGroupId)
{    
    bool bLocked=KLPRSS_PolIsLocked(pSrc);
    bool bMandatory=GetBoolValue(pSrc, c_szwSPS_Mandatory);
    int nAction=DPA_DO_NOTHING;
    ;
    if(bMandatory)        
        nAction=DPA_DISTRIBUTE_LOCK;
    else if(!bLocked)
    {
        bool bShouldUnlock;
        if(!pDst)
            bShouldUnlock=
                    pSrc->DoesExist(c_szwSPS_LockedPolicyName) ||
                    pSrc->DoesExist(c_szwSPS_LockedGroupName);        
        else
            bShouldUnlock=KLPRSS_PolIsLocked(pDst);

        if(bShouldUnlock)//else do nothing
            nAction=DPA_DISTRIBUTE_UNLOCK;
    }
    else if(bLocked)
        nAction=DPA_ERROR;
    ;
    KLSTD_ASSERT(nAction!=DPA_ERROR);
    if(nAction==DPA_ERROR)
        nAction=DPA_DISTRIBUTE_UNLOCK;
    ;
    /*
        if DPA_DISTRIBUTE_LOCK then we should set child's bMandatory
        to true and bLocked, wstrPolicyName, wstrGroupName to parent's values;
        copy src to dst settings all bLocked into true

        if DPA_DISTRIBUTE_UNLOCK then we should set child's bMandatory
        to false and delete bLocked, wstrPolicyName, wstrGroupName;
        copy src to dst
    */
    bool bWasChanged=false;
    CAutoPtr<IntValue> p_nPolicyId, p_nGroupId;
    ;
    p_nPolicyId=(IntValue*)pSrc->GetValue2(c_szwSPS_LockedPolicyId, false);
    p_nGroupId=(IntValue*)pSrc->GetValue2(c_szwSPS_LockedGroupId, false);
    ;
    if(  p_nPolicyId  )
    {
        KLPAR_CHKTYPE( p_nPolicyId , INT_T, c_szwSPS_LockedPolicyId);
    };
    if(p_nGroupId)
    {
        KLPAR_CHKTYPE(p_nGroupId, INT_T, c_szwSPS_LockedGroupId);
    };

    ;
    CAutoPtr<StringValue> p_wstrPolicyName, p_wstrGroupName;
    p_wstrPolicyName=(StringValue*)pSrc->GetValue2(c_szwSPS_LockedPolicyName, false);
    p_wstrGroupName=(StringValue*)pSrc->GetValue2(c_szwSPS_LockedGroupName, false);
    ;
    if(p_wstrPolicyName)
    {
        KLPAR_CHKTYPE(p_wstrPolicyName, STRING_T, c_szwSPS_LockedPolicyName);
    };
    if(p_wstrGroupName)
    {
        KLPAR_CHKTYPE(p_wstrGroupName, STRING_T, c_szwSPS_LockedGroupName);
    };

    switch(nAction)
    {
    case DPA_DISTRIBUTE_LOCK:
        {
            if(pDst == NULL)
            {
                std::vector<dp_value_t> values;
                values.reserve(4);
                CAutoPtr<BoolValue> p_bMandatory, p_bLocked;            
                CreateValue(true, &p_bMandatory);
                if(bLocked)
                    CreateValue(bLocked, &p_bLocked);
                if(!p_nPolicyId)
                {
                    CreateValue(lSrcPolicyId, &p_nPolicyId);
                    pSrc->ReplaceValue(c_szwSPS_LockedPolicyId, p_nPolicyId);
                };
                if(!p_nGroupId)
                {
                    CreateValue(lSrcPolicyGroupId, &p_nGroupId);
                    pSrc->ReplaceValue(c_szwSPS_LockedGroupId, p_nGroupId);
                };
                if(!p_wstrPolicyName)
                {
                    CreateValue(L"", &p_wstrPolicyName);
                    pSrc->ReplaceValue(c_szwSPS_LockedPolicyName, p_wstrPolicyName);
                };
                if(!p_wstrGroupName)
                {
                    CreateValue(L"", &p_wstrGroupName);
                    pSrc->ReplaceValue(c_szwSPS_LockedGroupName, p_wstrGroupName);
                };
                values.push_back(dp_value_t(c_szwSPS_Mandatory, p_bMandatory));
                values.push_back(dp_value_t(c_szwSPS_Locked, p_bLocked));
                values.push_back(dp_value_t(c_szwSPS_LockedPolicyName, p_wstrPolicyName));
                values.push_back(dp_value_t(c_szwSPS_LockedGroupName, p_wstrGroupName));
                values.push_back(dp_value_t(c_szwSPS_LockedPolicyId, p_nPolicyId));
                values.push_back(dp_value_t(c_szwSPS_LockedGroupId, p_nGroupId));
                SetForAllChildren(pSrc, values);
            }
            else
            {
                CAutoPtr<Params> pTemp;
                pSrc->Duplicate(&pTemp);
                {
                    std::vector<dp_value_t> values;
                    values.reserve(1);
                    CAutoPtr<BoolValue> p_bLocked;
                    CreateValue(true, &p_bLocked);
                    values.push_back(dp_value_t(c_szwSPS_Locked, p_bLocked));
                    SetForAllChildren(pTemp, values);
                    pTemp->ReplaceValue(c_szwSPS_Locked, p_bLocked);
                };
                pDst->Clear();
                pDst->MoveFrom(pTemp);
            };
            bWasChanged=true;
        }
        break;
    case DPA_DISTRIBUTE_UNLOCK:
        {
            //<-- 07.08.2006 13:39:07 Fix for bug #10278
            KLPAR::Params* pData2Process = 
                    (pDst == NULL)
                        ?   pSrc
                        :   pDst;
            ;
            std::vector<dp_value_t> values;
            values.reserve(4);
            CAutoPtr<BoolValue> p_bMandatory;
            CreateValue(false, &p_bMandatory);
            values.push_back(dp_value_t(c_szwSPS_Mandatory, p_bMandatory));
            values.push_back(dp_value_t(c_szwSPS_Locked, NULL));
            values.push_back(dp_value_t(c_szwSPS_LockedPolicyName, NULL));
            values.push_back(dp_value_t(c_szwSPS_LockedGroupName, NULL));
            values.push_back(dp_value_t(c_szwSPS_LockedPolicyId, NULL));
            values.push_back(dp_value_t(c_szwSPS_LockedGroupId, NULL));
            SetForAllChildren(pData2Process, values);
            pData2Process->DeleteValue(c_szwSPS_Locked, false);
            pData2Process->DeleteValue(c_szwSPS_LockedPolicyName, false);
            pData2Process->DeleteValue(c_szwSPS_LockedGroupName, false);
            pData2Process->DeleteValue(c_szwSPS_LockedPolicyId, false);
            pData2Process->DeleteValue(c_szwSPS_LockedGroupId, false);
            if(pData2Process == pDst)
                pData2Process->ReplaceValue(c_szwSPS_Mandatory, p_bMandatory);
            ;
            /*
            //--> 07.08.2006 13:39:07 Fix for bug #10278
            if(pDst == NULL)
            {
                std::vector<dp_value_t> values;
                values.reserve(4);
                CAutoPtr<BoolValue> p_bMandatory;
                CreateValue(false, &p_bMandatory);
                values.push_back(dp_value_t(c_szwSPS_Mandatory, p_bMandatory));
                values.push_back(dp_value_t(c_szwSPS_Locked, NULL));
                values.push_back(dp_value_t(c_szwSPS_LockedPolicyName, NULL));
                values.push_back(dp_value_t(c_szwSPS_LockedGroupName, NULL));
                values.push_back(dp_value_t(c_szwSPS_LockedPolicyId, NULL));
                values.push_back(dp_value_t(c_szwSPS_LockedGroupId, NULL));
                SetForAllChildren(pSrc, values);
                pSrc->DeleteValue(c_szwSPS_Locked, false);
                pSrc->DeleteValue(c_szwSPS_LockedPolicyName, false);
                pSrc->DeleteValue(c_szwSPS_LockedGroupName, false);
                pSrc->DeleteValue(c_szwSPS_LockedPolicyId, false);
                pSrc->DeleteValue(c_szwSPS_LockedGroupId, false);
            }
            else
            {
                CAutoPtr<Params> pTemp;
                pSrc->Duplicate(&pTemp);
                pDst->Clear();
                pDst->MoveFrom(pTemp);                
            };
            //<-- 07.08.2006 13:39:07 Fix for bug #10278
            */
            //--> 07.08.2006 13:39:07 Fix for bug #10278
            bWasChanged=true;
        }
        break;
    case DPA_DO_NOTHING:
        {
            Value* pSrcValue=pSrc->GetValue2(c_szwSPS_Value, true);
            KLSTD_CHKINPTR(pSrcValue);


            switch(pSrcValue->GetType())
            {
            case KLPAR::Value::PARAMS_T:
                {
                    Params* pNewRoot=static_cast<ParamsValue*>(pSrcValue)->GetValue();
                    if(pNewRoot)
                    {
                        CAutoPtr<Params> pDstRoot;
                        CAutoPtr<ParamsValue> pDstValue;
                        if(pDst)
                        {
                            pDstValue=(ParamsValue*)pDst->GetValue2(
                                                        c_szwSPS_Value,
                                                        false);
                            if(
                                pDstValue &&
                                pDstValue->GetType()==Value::PARAMS_T &&
                                pDstValue->GetValue())
                            {
                                pDstRoot=pDstValue->GetValue();
                            }
                            else
                            {
                                KLPAR_CreateParams(&pDstRoot);
                                pDstValue=NULL;
                                CreateValue(pDstRoot, &pDstValue);
                            };
                        };
                        bWasChanged=EnumerateParams(pNewRoot, pDstRoot, lSrcPolicyId, lSrcPolicyGroupId);
                        if(bWasChanged && pDst)
                            pDst->ReplaceValue(c_szwSPS_Value, pDstValue);
                    };
                };
                break;
            case KLPAR::Value::ARRAY_T:
                {
                    ;
                }
                break;
            };
        };
        break;
    };
    return bWasChanged;
};


static void NormalizePolicy(
                            Storage*            pSrcStorage,
                            const wchar_t***    ppszwSections,
                            bool                bFailIfSectionNotFound,
                            long                lSrcPolicyId,
                            long                lSrcPolicyGroupId)
{
    for(int iSection=0; ppszwSections[iSection]; ++iSection)
    {
        const wchar_t* entry[]=
        {
            ppszwSections[iSection][0],
            ppszwSections[iSection][1],
            ppszwSections[iSection][2],
            NULL
        };
        KLSTD_CHK(ppszwSections, entry[0] && entry[1] && entry[2]);
        CAutoPtr<Params>        pSrc, pDst;
        
        if(!pSrcStorage->region_load(entry, true, bFailIfSectionNotFound))
            continue;        
        pSrcStorage->entry_read(&pSrc);
        if(!pSrc)
            continue;

        if(DistributePolicy(pSrc, NULL, lSrcPolicyId, lSrcPolicyGroupId))
            pSrcStorage->entry_write(CF_CREATE_ALWAYS, pSrc);
    };
}

static void DistributePolicy(
                            Storage*            pSrcStorage,
                            const wchar_t***    ppszwSections,
                            Storage*            pDstStorage,
                            bool                bFailIfSectionNotFound,
                            long                lSrcPolicyId,
                            long                lSrcPolicyGroupId)
{
    for(int iSection=0; ppszwSections[iSection]; ++iSection)
    {
        const wchar_t* entry[]=
        {
            ppszwSections[iSection][0],
            ppszwSections[iSection][1],
            ppszwSections[iSection][2],
            NULL
        };
        KLSTD_CHK(ppszwSections, entry[0] && entry[1] && entry[2]);
        CAutoPtr<Params>        pSrc, pDst;
        
        if(!pSrcStorage->region_load(entry, false, bFailIfSectionNotFound))
            continue;        
        pSrcStorage->entry_read(&pSrc);
        if(!pSrc)
            continue;

        if(pDstStorage->region_load(entry, true, false))
            pDstStorage->entry_read(&pDst);
        if(!pDst)
        {
            KLPAR_CreateParams(&pDst);
            ParamsNames vecNames;
            pSrc->GetNames(vecNames);
            for(ParamsNames::iterator itName=vecNames.begin(); itName != vecNames.end(); ++itName)
            {
                if(wcscmp((*itName).c_str(), c_szwSPS_Value) == 0)continue;
                pDst->AddValue(*itName, pSrc->GetValue2(*itName, true));
            };
        };

        if(DistributePolicy(
                pSrc, 
                pDst, 
                lSrcPolicyId, 
                lSrcPolicyGroupId))
        {
            ForceCreateEntry(
                            pDstStorage,
                            ppszwSections[iSection][0],
                            ppszwSections[iSection][1],
                            ppszwSections[iSection][2]);
            pDstStorage->region_load(entry, true, true);
            pDstStorage->entry_write(CF_CREATE_ALWAYS, pDst);
        };            
    };
};


KLCSKCA_DECL void KLPRSS_DistributePolicy(
                            const wchar_t*                      pszwSrcPath,
                            long                                lSrcTimeout,
                            const wchar_t***                    pszwSections,
                            const wchar_t**                     pszwDstPaths,
                            long                                lDstTimeout,
                            void*                               pContext,
                            KLPRSS::DistributePolicyCallback*   pCallback,
                            bool                                bFailIfSectionNotFound,
                            long                                lSrcPolicyId,
                            long                                lSrcPolicyGroupId)
{
    KLSTD_CHKINPTR(pszwSrcPath);
    KLSTD_CHKINPTR(pszwSections);
    KLSTD_CHKINPTR(pszwDstPaths);

    KL_TMEASURE_BEGIN(L"KLPRSS_DistributePolicy", 3);
    CAutoPtr<Storage> pSrcStore;

    ss_format_t fmt;
    fmt.id = SSF_ID_SEMIPACKED;

    KLPRSS::CreateStorage(pszwSrcPath, CF_OPEN_EXISTING, AF_READ|AF_WRITE, &pSrcStore, &fmt);
    pSrcStore->trans_begin(true, lSrcTimeout, true);
    NormalizePolicy(pSrcStore, pszwSections, bFailIfSectionNotFound, lSrcPolicyId, lSrcPolicyGroupId);
    for(int iStore=0; pszwDstPaths[iStore]; ++iStore)
    {
        bool bResult=true;
        CAutoPtr<Error> pResult;
        CAutoPtr<Storage> pDstStore;
        KLERR_TRY            
            KLPRSS::CreateStorage(pszwDstPaths[iStore], CF_OPEN_ALWAYS, AF_READ|AF_WRITE, &pDstStore, &fmt);
            pDstStore->trans_begin(true, lDstTimeout);
                DistributePolicy(
                            pSrcStore, 
                            pszwSections, 
                            pDstStore, 
                            bFailIfSectionNotFound, 
                            lSrcPolicyId, 
                            lSrcPolicyGroupId);
                KLPRSS::SS_OnSaving(pDstStore);
            pDstStore->trans_end(true);
        KLERR_CATCH(pError)
            KLERR_IGNORE(if(pDstStore)pDstStore->trans_end(false));
            KLERR_SAY_FAILURE(3, pError);            
            pResult=pError;
            bResult=false;            
        KLERR_ENDTRY
        if(pCallback)
        {
            KL_TMEASURE_BEGIN(L"KLPRSS::DistributePolicyCallback", 3);
            KLERR_IGNORE(pCallback(pContext, iStore, bResult, pResult));
            KL_TMEASURE_END();
        };
    };    
    pSrcStore->trans_end(true);
    KL_TMEASURE_END();
};


KLCSKCA_DECL void KLPRSS_ResetParentalLocks(
                            const wchar_t*      szwSsFsPath,
                            long                lSsTimeout,
                            sections_t*         pSections)
{
    KLSTD_CHK(szwSsFsPath, szwSsFsPath && szwSsFsPath[0] && szwSsFsPath[0] != c_chwSSP_Marker);
    KLSTD_CHK(lSsTimeout, lSsTimeout == KLSTD_INFINITE || lSsTimeout >= 0);
    ;
    KL_TMEASURE_BEGIN(L"KLPRSS_ResetParentalLocks", 3);

        ss_format_t fmt;
        fmt.id = SSF_ID_SEMIPACKED;

        CAutoPtr<Storage> pStore;
        KLPRSS::CreateStorage(
                    szwSsFsPath,
                    CF_OPEN_EXISTING,
                    AF_READ|AF_WRITE,
                    &pStore,
                    &fmt);

        pStore->trans_begin(true, lSsTimeout, true);
        KLERR_TRY
            KLPRSS::sections_t tmpsects;
            if(!pSections)
            {
                pSections=&tmpsects;
                CBulkMode bm;
                bm.Initialize(NULL, NULL, pStore, &lSsTimeout);            
                bm.EnumAllWSectionsI(tmpsects, false);
            };
            KLPRSS::sections_t& sects=(*pSections);
            for(unsigned int i=0; i < sects.m_vecNames.size(); ++i)
            {
                section_name_t& sec=sects.m_vecNames[i];
                const wchar_t* path[]=
                {
                    sects.m_vecProducts[sec.m_nProduct].c_str(),
                    sects.m_vecVersions[sec.m_nVersion].c_str(),
                    sects.m_vecSections[sec.m_nSection].c_str(),
                    NULL
                };
                if(sects.m_vecSections[sec.m_nSection] == KLPRSS_VERSION_INFO)
                    continue;
                pStore->region_load(path, true);
                CAutoPtr<Params> pData;
                pStore->entry_read(&pData);
                if(ClearParentalLocks(pData))
                    pStore->entry_write(CF_CREATE_ALWAYS, pData);
            };
            KLPRSS::SS_OnSaving(pStore);
            pStore->trans_end(true);
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(2, pError);
            pStore->trans_end(false);
            KLERR_RETHROW();
        KLERR_ENDTRY
    KL_TMEASURE_END();
}
