/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	parserialize.cpp
 * \author	Andrew Kazachkov
 * \date	04.12.2003 12:18:13
 * \brief	
 * 
 */

#include "std/par/params.h"
#include "std/par/valenum.h"
#include "common/measurer.h"
#include "std/base/klbaseqi_imp.h"
#include "./parbinser.h"

#include "./paramsimpl.h"


#define KLCS_MODULENAME L"KLPAR"

#ifdef _WIN32
#pragma inline_recursion( on )
#pragma inline_depth( 8 )
#endif

namespace KLPAR
{
    CParamsBinWriter::CParamsBinWriter(
            CBinWriter&             writer,
            const SerParamsFormat*  pFormat)
        :   m_Writer(writer)
        ,   m_pFormat(pFormat)
    {;};
        
    void CParamsBinWriter::Start(KLPAR::Params*  pParams)
    {
        WriteParams(pParams);
        m_Writer.Flush();
    }

    void CParamsBinWriter::WriteValue(KLPAR::Value* pValue)
    {
        if(!pValue)
        {
            //m_Writer << AVP_dword(SBVT_NULL);// it's not good
            WriteValueType(SBVT_NULL);
        }
        else
        {   
            switch(pValue->GetType())
            {
            case Value::EMPTY_T:
                WriteValueType(SBVT_EMPTY);
                break;
            case Value::STRING_T:
                WriteValueType(SBVT_STRING) << static_cast<StringValue*>(pValue)->GetValue();
                break;
            case Value::BOOL_T:
                WriteValueType(SBVT_BOOL) << static_cast<BoolValue*>(pValue)->GetValue();
                break;
            case Value::INT_T:
                WriteValueType(SBVT_INT) << (AVP_dword)(static_cast<IntValue*>(pValue)->GetValue());
                break;
            case Value::LONG_T:
                WriteValueType(SBVT_LONG) << (AVP_qword)(static_cast<LongValue*>(pValue)->GetValue());
                break;
            case Value::DATE_TIME_T:
                WriteValueType(SBVT_DATE_TIME) << (AVP_qword)((int)static_cast<DateTimeValue*>(pValue)->GetValue());
                break;
            case Value::DATE_T:
                WriteValueType(SBVT_DATE) << static_cast<DateValue*>(pValue)->GetValue();
                break;
            case Value::BINARY_T:
                WriteValueType(SBVT_BINARY) << AVP_dword(static_cast<BinaryValue*>(pValue)->GetSize());
                m_Writer.Write(
                            static_cast<BinaryValue*>(pValue)->GetValue(),
                            static_cast<BinaryValue*>(pValue)->GetSize());
                break;
            case Value::FLOAT_T:
                WriteValueType(SBVT_FLOAT) << static_cast<FloatValue*>(pValue)->GetValue();
                break;
            case Value::DOUBLE_T:
                WriteValueType(SBVT_DOUBLE) << static_cast<DoubleValue*>(pValue)->GetValue();
                break;
            case Value::PARAMS_T:
                WriteValueType(SBVT_PARAMS);
                WriteParams(static_cast<ParamsValue*>(pValue)->GetValue());
                break;
            case Value::ARRAY_T:
                WriteValueType(SBVT_ARRAY);
                {
                    ArrayValue* pArrayValue=static_cast<ArrayValue*>(pValue);                    
                    const int nSize=(AVP_dword)pArrayValue->GetSize();
                    m_Writer << AVP_dword(nSize);
                    for(int i=0; i < nSize; ++i)
                    {
                        KLSTD::CAutoPtr<Value> pItem;
                        pArrayValue->GetAt(i, &pItem);
                        WriteValue(pItem);
                    };
                };
                break;
            default:
                KLSTD_ASSERT(!"Unknown value type");
                KLSTD_THROW(KLSTD::STDE_BADFORMAT);
            };
        };
    };
    void CParamsBinWriter::OnEnumeratedValue(const wchar_t* szwName, KLPAR::Value* pValue)
    {
        KLSTD_ASSERT_THROW(szwName != NULL && szwName[0] != 0 && pValue != NULL);
        m_Writer << szwName;
        WriteValue(pValue);
    };

    void CParamsBinWriter::WriteParams(KLPAR::Params* pParams)
    {
        KLSTD::CAutoPtr<KLPAR::EnumValues> pEnumValues;
        AVP_dword dwSize=c_dwNullParamsSize;
        if(pParams)
        {
            dwSize=(AVP_dword)pParams->GetSize();
            pParams->QueryInterface(
                            KLSTD_IIDOF(KLPAR::EnumValues),
                            (void**)&pEnumValues);
            if(!pEnumValues)
                KLSTD_NOTIMP();
        };
        m_Writer << dwSize;
        if(pEnumValues)
            pEnumValues->EnumerateContents(this);
    };

    CParamsBinReader::CParamsBinReader(
                    CBinReader&             reader,
                    const SerParamsFormat*  pFormat)
        :   m_Reader(reader)
        ,   m_pFormat(pFormat)
    {
        KLPAR_CreateValuesFactory(&m_pFactory);
        //m_wstrBuffer.reserve(1024);
    };
       
    void CParamsBinReader::Start(KLPAR::Params**  ppParams)
    {
        ReadParams(ppParams);
        m_Reader.Flush();
    };

    void SerCreateStringValue(const KLPAR::string_t& x, StringValue** pValue);
    KLCSC_DECL void SerCreateBinaryValue(KLSTD::MemoryChunkPtr pChunk, BinaryValue** pValue);

    KLSTD_INLINEONLY void KLSTD_FASTCALL CParamsBinReader::ReadValue(KLPAR::Value** ppValue)
    {
        KLSTD::CAutoPtr<Value> pValue;
        serbinval_t type;
        ReadValueType(type);
        switch(type)
        {
        case SBVT_NULL:
            break;
        //case SBVT_EMPTY:// there's no support for SBVT_EMPTY just now
            //break;
        case SBVT_STRING:
            {
                //m_wstrBuffer.clear();
                //m_Reader >> m_wstrBuffer;
                //CreateValue(m_wstrBuffer.c_str(), (StringValue**)&pValue);
                KLPAR::string_t wstrName;
                m_Reader >> wstrName;
                SerCreateStringValue(wstrName, (StringValue**)&pValue);
            }
            break;
        case SBVT_BOOL:
            {
                bool bX;
                m_Reader >> bX;
                CreateValue(bX, (BoolValue**)&pValue);
            }
            break;
        case SBVT_INT:
            {
                AVP_dword dwX;
                m_Reader >> dwX;
                CreateValue(long(dwX), (IntValue**)&pValue);
            }
            break;
        case SBVT_LONG:
            {
                AVP_qword qwX;
                m_Reader >> qwX;
                CreateValue(AVP_longlong(qwX), (LongValue**)&pValue);
            }
            break;
        case SBVT_DATE_TIME:
            {
                AVP_qword qwX;
                m_Reader >> qwX;
                CreateValue(time_t(qwX), (DateTimeValue**)&pValue);
            }
            break;
        case SBVT_DATE:
            {
                std::string strX;
                m_Reader >> strX;
                CreateValue(strX.c_str(), (DateValue**)&pValue);
            }
            break;
        case SBVT_BINARY:
            {
                AVP_dword dwSize=0;
                m_Reader >> dwSize;
                KLSTD_ASSERT(int(dwSize) >= 0);
                
                if(int(dwSize) < 0)
                    KLSTD_THROW(KLSTD::STDE_BADFORMAT);

                if(dwSize > m_Reader.GetObjectSize())
                    KLSTD_THROW(KLSTD::STDE_BADFORMAT);

                if(dwSize > 0)
                {
                    KLSTD::CAutoPtr<KLSTD::MemoryChunk> pChunk;
                //KL_TMEASURE_BEGIN(L"KLSTD_AllocMemoryChunk1", 4)
                    KLSTD_AllocMemoryChunk(dwSize, &pChunk);
                //KL_TMEASURE_END()
                    void* pData=pChunk->GetDataPtr();
                //KL_TMEASURE_BEGIN(L"m_Reader.Read1", 4)
                    m_Reader.Read(pData, dwSize);
                //KL_TMEASURE_END()
                    SerCreateBinaryValue(pChunk, (BinaryValue**)&pValue);
                    //m_pFactory->CreateBinaryValue((BinaryValue**)&pValue);
                    //((BinaryValue*)(Value*)pValue)->SetValue(pData, dwSize);
                }
                else
                {
                    m_pFactory->CreateBinaryValue((BinaryValue**)&pValue);
                    ((BinaryValue*)(Value*)pValue)->SetValue(NULL, 0);
                };
            }
            break;
        case SBVT_FLOAT:
            {
                float fX;
                m_Reader >> fX;
                CreateValue(fX, (FloatValue**)&pValue);
            };
            break;
        case SBVT_DOUBLE:
            {
                double lfX;
                m_Reader >> lfX;
                CreateValue(lfX, (DoubleValue**)&pValue);
            };
            break;
        case SBVT_PARAMS:
            {
                KLSTD::CAutoPtr<Params> pParams;
                ReadParams(&pParams);
                CreateValue(pParams, (ParamsValue**)&pValue);
            };
            break;
        case SBVT_ARRAY:
            {
                AVP_dword dwSize;
                m_Reader >> dwSize;
                KLSTD_ASSERT(int(dwSize) >= 0);
                
                if(int(dwSize) < 0)
                    KLSTD_THROW(KLSTD::STDE_BADFORMAT);

                if(dwSize > m_Reader.GetObjectSize())
                    KLSTD_THROW(KLSTD::STDE_BADFORMAT);

                KLSTD::CAutoPtr<ArrayValue> pArray;
                m_pFactory->CreateArrayValue(&pArray);
                pArray->SetSize(dwSize);
                const size_t c_nSize = size_t(dwSize);
                for(size_t i=0; i < c_nSize; ++i)
                {
                    KLSTD::CAutoPtr<Value> pItem;
                    ReadValue(&pItem);
                    pArray->SetAt(i, pItem);
                };
                pValue=(Value*)(ArrayValue*)pArray;
            };
            break;
        default:
            KLSTD_ASSERT(!"Unknown value type");
            KLSTD_THROW(KLSTD::STDE_BADFORMAT);
        };
        //pValue.CopyTo(ppValue);
        *ppValue = pValue.Detach();
    };

    void DoCreateParams(KLPAR::CParamsImpl** ppX);

    void KLSTD_FASTCALL CParamsBinReader::ReadParams(KLPAR::Params** ppParams)
    {
        KLSTD::CAutoPtr<KLPAR::Params> pParams_;
        AVP_dword dwSize=c_dwNullParamsSize;
        m_Reader >> dwSize;
        if(dwSize != c_dwNullParamsSize)
        {
            KLSTD_ASSERT(int(dwSize) >= 0);
            
            if(int(dwSize) < 0)
                KLSTD_THROW(KLSTD::STDE_BADFORMAT);
            
            if(dwSize > m_Reader.GetObjectSize())
                KLSTD_THROW(KLSTD::STDE_BADFORMAT);

            KLSTD::CAutoPtr<KLPAR::CParamsImpl> pParams; 
            DoCreateParams(&pParams);

            //KLPAR_CreateParams(&pParams);
            //std::wstring wstrName;
            //wstrName.reserve(100);
            const size_t c_nSize = size_t(dwSize);
            for(size_t i=0; i < c_nSize; ++i)
            {
                KLSTD::CAutoPtr<KLPAR::Value> pVal;
                //wstrName.clear();
                KLPAR::string_t wstrName;
                m_Reader >> wstrName;
                ReadValue(&pVal);
                KLSTD_ASSERT(!wstrName.empty() && pVal!=NULL);
                if(wstrName.empty() || !pVal)
                    KLSTD_THROW(KLSTD::STDE_BADFORMAT);
                //pParams->AddValue(wstrName.c_str(), pVal);
                pParams->entry_add(wstrName, pVal, false);
            };
            pParams_.Attach((KLPAR::Params*)(KLPAR::CParamsImpl*)pParams.Detach());
        };
        //pParams_.CopyTo(ppParams);
        *ppParams = pParams_.Detach();
    };
};
