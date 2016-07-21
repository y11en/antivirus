/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	ops_pol.cpp
 * \author	Andrew Kazachkov
 * \date	23.12.2003 16:21:00
 * \brief	
 * 
 */

#include "std/base/klbase.h"
#include "std/base/klbaseqi_imp.h"
#include <std/par/params.h>
#include <std/par/helpersi.h>
//#include "std/par/paramsi.h"
#include "kca/prss/helpersi.h"
#include "kca/prss/errors.h"
#include <set>
#include <list>
#include <algorithm>

using namespace KLSTD;
using namespace KLPAR;

bool KLPRSS_HasArrayItemPolicyFmt(KLPAR::Value* pVal)
{
    if(!pVal || pVal->GetType() != KLPAR::Value::PARAMS_T)
        return false;
    KLPAR::Params* pItem=((KLPAR::ParamsValue*)(KLPAR::Value*)pVal)->GetValue();
    return pItem && pItem->DoesExist(KLPRSS::c_szwSPS_Value);
};

namespace KLPRSS
{
    static const int c_nDefPolicyDepth=21;
    
    static void PolConvert(
                CAutoPtr<KLPAR::Params>&        pInData,
                CAutoPtr<KLPAR::Params>&        pMandatory,
                CAutoPtr<KLPAR::Params>&        pDefault,
                std::vector<const wchar_t*>&    vecPath,
                const bool*                     pbForce)
    {
        ParamsNames vecNames;
        pInData->GetNames(vecNames);

        for(ParamsNames::iterator it=vecNames.begin(); it != vecNames.end(); ++it)
        {
            CAutoPtr<Params> pEntry=KLPAR::GetParamsValue(pInData, *it);
            CAutoPtr<Value> pValue;
            pEntry->GetValue(c_szwSPS_Value, &pValue);
            if(!pValue)continue;
            CAutoPtr<Value> pValDefault, pValMandatory;
            
            switch(pValue->GetType())
            {
            case Value::PARAMS_T:
                {
                    CAutoPtr<Params> pParams=((ParamsValue*)(Value*)pValue)->GetValue();
                    if(pParams)
                    {
                        //add container params
                        {
                            bool bMandatory=pbForce?(*pbForce):GetBoolValue(pEntry, c_szwSPS_Mandatory);
                            CAutoPtr<ValuesFactory> pFactory;
                            KLPAR_CreateValuesFactory(&pFactory);
                            CAutoPtr<ParamsValue> pParamsValue;  
                            pFactory->CreateParamsValue(&pParamsValue);
                            if(bMandatory)
                            {
                                if(!pMandatory)
                                    KLPAR_CreateParams(&pMandatory);
                                KLPAR_ReplaceValue(pMandatory, &(*vecPath.begin()), *it, pParamsValue);
                            }
                            else
                            {
                                if(!pDefault)
                                    KLPAR_CreateParams(&pDefault);
                                KLPAR_ReplaceValue(pDefault, &(*vecPath.begin()), *it, pParamsValue);
                            };
                        };
                        vecPath.pop_back();//remove NULL
                        vecPath.push_back((*it).c_str());
                        vecPath.push_back(NULL);// add NULL
                        PolConvert(pParams, pMandatory, pDefault, vecPath, pbForce);
                        vecPath.pop_back();//remove NULL
                        vecPath.pop_back();//remove (*it).c_str()
                        vecPath.push_back(NULL);// add NULL
                    };
                };
                break;
            case Value::ARRAY_T:
                {
                    CAutoPtr<ArrayValue> pArray=(ArrayValue*)(Value*)pValue;
                    const int nSize=pArray->GetSize();
                    bool bMandatory=pbForce?(*pbForce):GetBoolValue(pEntry, c_szwSPS_Mandatory);
                    CAutoPtr<ArrayValue> pNewArray;
                    {
                        CAutoPtr<ValuesFactory> pFactory;
                        KLPAR_CreateValuesFactory(&pFactory);
                        pFactory->CreateArrayValue(&pNewArray);
                    };
                    pNewArray->SetSize(nSize);
                    for(int k=0; k < nSize; ++k)
                    {
                        CAutoPtr<Value> pSubValue;
                        pArray->GetAt(k, &pSubValue);
                        if(!pSubValue)continue;
                        if(KLPRSS_HasArrayItemPolicyFmt(pSubValue))//pSubValue->GetType() == KLPAR::Value::PARAMS_T)
                        {
                            CAutoPtr<Params> pTmpParams;
                            KLPAR_CreateParams(&pTmpParams);
                            pTmpParams->AddValue(L"_", pSubValue);
                            CAutoPtr<Params> pTmpMand, pTmpDef;
                            std::vector<const wchar_t*> vecTmpPath;
                            vecTmpPath.reserve(c_nDefPolicyDepth);
                            vecTmpPath.push_back(NULL);
                            PolConvert(pTmpParams, pTmpMand, pTmpDef, vecTmpPath, &bMandatory);
                            if(bMandatory)
                            {
                                KLSTD_ASSERT(pTmpDef == NULL);
                                pTmpParams=pTmpMand;
                            }
                            else
                            {
                                KLSTD_ASSERT(pTmpMand == NULL);
                                pTmpParams=pTmpDef;
                            }
                            KLSTD_ASSERT(pTmpParams!=NULL);
                            pSubValue=NULL;
                            if(pTmpParams)
                                pTmpParams->GetValue(L"_", &pSubValue);
                            /*
                            CAutoPtr<Params> pSubEntry=((ParamsValue*)(Value*)pSubValue)->GetValue();
                            if(!pSubEntry)continue;
                            pSubValue=NULL;
                            pSubEntry->GetValue(c_szwSPS_Value, &pSubValue);
                            */
                        };
                        pNewArray->SetAt(k, pSubValue);
                    };
                    if(bMandatory)
                    {
                        if(!pMandatory)
                            KLPAR_CreateParams(&pMandatory);
                        KLPAR_ReplaceValue(pMandatory, &(*vecPath.begin()), *it, pNewArray);
                    }
                    else
                    {
                        if(!pDefault)
                            KLPAR_CreateParams(&pDefault);
                        KLPAR_ReplaceValue(pDefault, &(*vecPath.begin()), *it, pNewArray);
                    };


                };
                break;
            default:
                {
                    bool bMandatory=pbForce?(*pbForce):GetBoolValue(pEntry, c_szwSPS_Mandatory);
                    if(bMandatory)
                    {
                        if(!pMandatory)
                            KLPAR_CreateParams(&pMandatory);
                        KLPAR_ReplaceValue(pMandatory, &(*vecPath.begin()), *it, pValue);
                    }
                    else
                    {
                        if(!pDefault)
                            KLPAR_CreateParams(&pDefault);
                        KLPAR_ReplaceValue(pDefault, &(*vecPath.begin()), *it, pValue);
                    };
                };
                break;
            };
        };
    };

    static void GetMandatoryAndDefault(
                KLPAR::Params*  pInData,
                KLPAR::Params** ppMandatory,
                KLPAR::Params** ppDefault)
    {
        CAutoPtr<Params> pMandatory, pDefault;
        if(pInData)
        {
            if(!pInData->DoesExist(c_szwSPS_Value))
                pDefault=pInData;
            else
            {
                CAutoPtr<Params> pInDataParams=GetParamsValue(pInData, c_szwSPS_Value);
                //KLPAR_CreateParams(&pDefault);
                std::vector<const wchar_t*> vecPath;
                vecPath.reserve(c_nDefPolicyDepth);
                vecPath.push_back(NULL);
                PolConvert(pInDataParams, pMandatory, pDefault, vecPath, NULL);
            };
        }
        pMandatory.CopyTo(ppMandatory);
        pDefault.CopyTo(ppDefault);
    };

}

KLCSKCA_DECL void KLPRSS_GetMandatoryAndDefault(
                        KLPAR::Params*  pInData,
                        KLPAR::Params** ppMandatory,
                        KLPAR::Params** ppDefault)
{
    KLPRSS::GetMandatoryAndDefault(pInData, ppMandatory, ppDefault);
};

namespace KLPRSS
{
    /*! Подготовка данных для "smart-записи" в SS, учитывающей политику.

        С - наличие переменной в сохраняемом контейнере Params
        Ц - наличие переменной в "целевом" контейнере Params
        M - наличие переменной в "обязательном" контейнере Params политики
        D - наличие переменной в "умолчательном" контейнере Params политики

        "1" означает наличие переменной
        "0" означает отсутствие переменной

        Обработка ведётся следующим образом:

        3210
        СMDЦ| |
        0000|Ц|0
        0001|Ц|0
        0010|D|1
        0011|Ц|0
        0100|M|2
        0101|Ц|0
        0110|M|2
        0111|Ц|0
        1000|С|3
        1001|С|3
        1010|С|3
        1011|С|3
        1100|M|2
        1101|Ц|0
        1110|M|2
        1111|Ц|0
 
    */
    const static unsigned char pMySelectTable[16]=
    {
        0, 0, 1, 0, 2, 0, 2, 0, 3, 3, 3, 3, 2, 0, 2, 0
    };


    class CMySelectValueCallback : public KLPAR::SelectValueCallback
    {
    protected:
        KLSTD_INTERAFCE_MAP_BEGIN(KLPAR::SelectValueCallback)
        KLSTD_INTERAFCE_MAP_END()
        unsigned long AddRef(){return 0;};
	    unsigned long Release(){return 0;};

		KLSTD::CAutoPtr<Value> DoSelectValue(
                                const wchar_t*                      szwName,
                                const std::vector<KLPAR::Value*>&   vecValues)

        {
            KLSTD_ASSERT(vecValues.size() == 4u);
            Value* pDst=vecValues[0];// Ц
            Value* pDef=vecValues[1];// D
            Value* pMnd=vecValues[2];// M
            Value* pSrc=vecValues[3];// С

            unsigned index=
                        (unsigned(pSrc!=NULL) << 3)|                        
                        (unsigned(pMnd!=NULL) << 2)|
                        (unsigned(pDef!=NULL) << 1)|
                        unsigned(pDst!=NULL);
            KLSTD_ASSERT(index < 16u);
            unsigned vecIndex=pMySelectTable[index];
            KLSTD_ASSERT(vecIndex < 4u);
            return vecValues[vecIndex];
        };
    };

    KLCSKCA_DECL void PrepareForSmartWrite(
                            KLPAR::Params*  pDst,
                            KLPAR::Params*  pDef,
                            KLPAR::Params*  pMnd,
                            KLPAR::Params*  pSrc,
                            Params**        ppResult)
    {
        KLSTD_CHKOUTPTR(ppResult);
        CMySelectValueCallback sel;
        std::vector<KLPAR::Params*> vecData;
        vecData.resize(4);
        vecData[0]=pDst;
        vecData[1]=pDef;
        vecData[2]=pMnd;
        vecData[3]=pSrc;
        KLSTD::CAutoPtr<KLPAR::Params> pResult;
        KLPAR_CreateParams(&pResult);
        KLPAR::ProcessParamsRecursively(vecData, &sel, pResult);
        pResult.CopyTo(ppResult);
    };
};
