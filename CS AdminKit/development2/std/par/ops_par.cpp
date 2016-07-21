/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	ops_par.cpp
 * \author	Andrew Kazachkov
 * \date	24.12.2003 16:08:00
 * \brief	
 * 
 */

#include <std/base/klstd.h>
#include <std/par/valenum.h>
#include <std/par/helpersi.h>

#include <set>

namespace KLPAR
{
    class CStringPtrWrapper
    {
    public:
        CStringPtrWrapper(const wchar_t* pX)
            :   m_pX(pX)
            ,   m_nX(wcslen(pX?pX:L""))
        {
            ;
        };

        CStringPtrWrapper(const CStringPtrWrapper& x)
            :   m_pX(x.c_str())
            ,   m_nX(x.size())
        {
            ;
        };

        bool operator < (const CStringPtrWrapper& x) const
        {
            int delta=size()-x.size();
            if(delta != 0)
                return delta < 0;
            else
                return wcscmp(c_str(), x.c_str()) < 0;
        };

        const wchar_t* c_str() const
        {
            return (m_pX?m_pX:L"");
        };

        int size() const
        {
            return m_nX;
        };
    protected:
        const wchar_t*  m_pX;
        int             m_nX;
    };

    typedef std::set<CStringPtrWrapper> strptrset_t;

    class CParamsToSetPusher : public EnumValuesCallback
    {
    public:
        strptrset_t m_set;
    protected://EnumValuesCallback implementation
        KLSTD_INTERAFCE_MAP_BEGIN(KLPAR::EnumValuesCallback)
        KLSTD_INTERAFCE_MAP_END()
        unsigned long AddRef(){return 0;};
	    unsigned long Release(){return 0;};
        void OnEnumeratedValue(const wchar_t* szwName, KLPAR::Value*)
        {
            m_set.insert(CStringPtrWrapper(szwName));
        }
    };

    void DoProcessParamsRecursively(
                            const std::vector<Params*>& vecData,
                            SelectValueCallback*        pCallback,
                            SelectValueCallback2*       pCallback2,
                            NodeActionCallback*         pNodeActionCallback,
                            Params*                     pResult,
                            bool                        bRemoveEmptyParams)
    {
        KLSTD_CHKINPTR(pResult);
        KLSTD_CHKINPTR(pCallback);
        CParamsToSetPusher theSet;
        const size_t nData=vecData.size();
        {            
            for(size_t i=0; i < nData; ++i)
            {                
                KLSTD::CAutoPtr<KLPAR::EnumValues> pEnum;
                KLPAR::Params* pItem = vecData[i];
                if(!pItem)
                    continue;
                pItem->QueryInterface(
                                KLSTD_IIDOF(KLPAR::EnumValues),
                                (void**)&pEnum);
                if(!pEnum)
                    continue;
                pEnum->EnumerateContents(&theSet);
            };
        };
        std::vector<KLPAR::Value*> vecValues;
        std::vector<KLPAR::Params*> vecParams;
        vecValues.resize(nData);
        vecParams.resize(nData);
        for(strptrset_t::iterator it=theSet.m_set.begin(); it != theSet.m_set.end(); ++it)
        {
            bool bAllParams=true;
            const CStringPtrWrapper& name = *it;
            for(size_t i=0; i < nData; ++i)
            {
                KLPAR::Params* pItem=vecData[i];
                if(!pItem)
                    vecValues[i] = NULL;
                else
                {
                    vecValues[i] = pItem->GetValue2(name.c_str(), false);
                    if(vecValues[i] && vecValues[i]->GetType() == Value::PARAMS_T)
                    {
                        vecParams[i]=((ParamsValue*)vecValues[i])->GetValue();
                    }
                    else
                    {
                        vecParams[i]=NULL;
                        if(vecValues[i] && bAllParams)
                            bAllParams=false;//if exists and not paramsvalue
                    };
                };
            };
            KLSTD::CAutoPtr<KLPAR::Value> pResultValue;
            if(!bAllParams)
            {
                if(!pCallback2)
                    pResultValue=pCallback->DoSelectValue(name.c_str(), vecValues);
                else
                    pResultValue=pCallback2->DoSelectValue2(name.c_str(), vecData, vecValues);
            }
            else
            {
                if( !pNodeActionCallback || 
                    pNodeActionCallback->DoSelectAction(
                        name.c_str(), 
                        vecParams))
                {
                    KLSTD::CAutoPtr<Params> pNewParams;
                    KLPAR_CreateParams(&pNewParams);
                    DoProcessParamsRecursively(
                                vecParams, 
                                pCallback, 
                                pCallback2,
                                pNodeActionCallback,
                                pNewParams, 
                                bRemoveEmptyParams);
                    if(!bRemoveEmptyParams || pNewParams->GetSize() !=0)
                        CreateValue(pNewParams, (KLPAR::ParamsValue**)&pResultValue);
                };
            };
            if(pResultValue)
                pResult->AddValue(name.c_str(), pResultValue);
        };
    };

    KLCSC_DECL void ProcessParamsRecursively(
                            const std::vector<Params*>& vecData,
                            SelectValueCallback*        pCallback,
                            Params*                     pResult,
                            bool                        bRemoveEmptyParams)
    {
        KLSTD_CHKINPTR(pResult);
        KLSTD_CHKINPTR(pCallback);
        KLSTD::CAutoPtr<NodeActionCallback> pNodeActionCallback;
        pCallback->QueryInterface(
                KLSTD_IIDOF(KLPAR::NodeActionCallback),
                (void**)&pNodeActionCallback);

        KLSTD::CAutoPtr<SelectValueCallback2> pCallback2;
        pCallback->QueryInterface(
                KLSTD_IIDOF(KLPAR::SelectValueCallback2),
                (void**)&pCallback2);
        ;
        DoProcessParamsRecursively(
                    vecData, 
                    pCallback, 
                    pCallback2,
                    pNodeActionCallback,
                    pResult,
                    bRemoveEmptyParams);
    };
}

namespace KLPAR
{

    class CConcatenateSelectValueCallback : public KLPAR::SelectValueCallback
    {
    protected:
        KLSTD_INTERAFCE_MAP_BEGIN(KLPAR::SelectValueCallback)
        KLSTD_INTERAFCE_MAP_END()
        unsigned long AddRef(){return 0;};
	    unsigned long Release(){return 0;};

		KLSTD::CAutoPtr<Value> DoSelectValue(
                const wchar_t* szwName,
                const std::vector<KLPAR::Value*>&   vecValues)
        {
            KLSTD_ASSERT(vecValues.size() == 2u);
            Value* pMaster=vecValues[0];
            Value* pSlave=vecValues[1];
            if(pMaster)
                return pMaster;
            return pSlave;
        };
    };
}

KLCSC_DECL void KLPAR_ConcatenateNew(
				KLPAR::Params*	pMaster,
				KLPAR::Params*	pSlave,
				KLPAR::Params** ppResult)
{
    KLSTD_CHKOUTPTR(ppResult);
	if(!pMaster)
        KLSTD::CAutoPtr<KLPAR::Params>(pSlave).CopyTo(ppResult);
	else if(!pSlave)
		KLSTD::CAutoPtr<KLPAR::Params>(pMaster).CopyTo(ppResult);
	else
	{
        KLPAR::CConcatenateSelectValueCallback sel;
        std::vector<KLPAR::Params*> vecData;
        vecData.resize(2);
        vecData[0]=pMaster;
        vecData[1]=pSlave;
        KLSTD::CAutoPtr<KLPAR::Params> pResult;
        KLPAR_CreateParams(&pResult);
        KLPAR::ProcessParamsRecursively(vecData, &sel, pResult);
        pResult.CopyTo(ppResult);
    };
};

KLCSC_DECL void KLPAR_Concatenate(
				KLPAR::Params*	pMaster,
				KLPAR::Params*	pSlave,
				KLPAR::Params** ppResult)
{
    KLSTD_CHKOUTPTR(ppResult);
	if(!pMaster)
        KLSTD::CAutoPtr<KLPAR::Params>(pSlave).CopyTo(ppResult);
	else if(!pSlave)
		KLSTD::CAutoPtr<KLPAR::Params>(pMaster).CopyTo(ppResult);
	else
	{
		KLSTD::CAutoPtr<KLPAR::Params> pResult;		
		pSlave->Duplicate(&pResult);
        KLPAR_Write(pResult, pMaster, KLSTD::CF_OPEN_ALWAYS);
		pResult.CopyTo(ppResult);
	};
};

void KLPAR_GetParamsSubtree(
					KLPAR::Params* pSrc,
					KLPAR::Params* pFilter,
					KLPAR::Params** ppDst)
{		
	if(!pSrc)
	{
		KLPAR_CreateParams(ppDst);
		return;
	};
	if(!pFilter)
	{
		pSrc->Clone(ppDst);
		return;
	};
    KLSTD::CAutoPtr<KLPAR::Params> pDst;
	KLPAR_CreateParams(&pDst);
	KLPAR::ParamsNames names;
	pFilter->GetNames(names);
	for(unsigned int i=0; i < names.size(); ++i)
	{
		KLSTD::CAutoPtr<KLPAR::Value> pValue;
		if(pSrc->GetValueNoThrow(names[i], &pValue))
		{
			if(pValue->GetType() != KLPAR::Value::PARAMS_T)
			{
				pDst->AddValue(names[i], pValue);
			}
			else
			{
				KLSTD::CAutoPtr<KLPAR::Value> pFilterValue;
				pFilter->GetValue(names[i], &pFilterValue);
				KLSTD::CAutoPtr<KLPAR::Params> pNewSrc=((KLPAR::ParamsValue*)(KLPAR::Value*)pValue)->GetValue();
				if(pNewSrc && pFilterValue->GetType() == KLPAR::Value::PARAMS_T)
				{
					KLSTD::CAutoPtr<KLPAR::Params> pNewFilter=((KLPAR::ParamsValue*)(KLPAR::Value*)pFilterValue)->GetValue();
					KLSTD::CAutoPtr<KLPAR::Params> pNewDst;
					KLPAR_GetParamsSubtree(pNewSrc, pNewFilter, &pNewDst);
					KLSTD::CAutoPtr<KLPAR::ParamsValue> pValueToAdd;						
					if(pNewDst)
					{
						KLPAR::CreateValue(pNewDst, &pValueToAdd);
						pDst->AddValue(names[i], pValueToAdd);
					};
				};
			};
		};
	};
	pDst.CopyTo(ppDst);
};

KLCSC_DECL void KLPAR_CopyValues(
                    KLPAR::Params*  pSrc, 
                    KLPAR::Params*  pDst, 
                    const wchar_t** pszwNames, 
                    size_t          nNames, 
                    bool            bClone)
{
    KLSTD_CHKINPTR(pSrc);
    KLSTD_CHKINPTR(pDst);
    KLSTD_CHK(pszwNames, pszwNames || !nNames );
    if(!pszwNames || !nNames)
        return;
    
    for(size_t j = 0; j < nNames; ++j)
    {
        KLSTD::CAutoPtr<KLPAR::Value> pVal, pValToAdd;
        if(pSrc->GetValueNoThrow(pszwNames[j], &pVal) && pVal)
        {
            if(bClone)
                pVal->Clone(&pValToAdd);
            else
                pValToAdd = pVal;
            pDst->ReplaceValue(pszwNames[j], pValToAdd);
        };
    };
};
