/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Params.h
 * \author Андрей Казачков
 * \date 2002
 * \brief Реализация интерфейсов модуля Params.
 *
 */

#include "std/base/klstd.h"
#include "std/par/par_gsoap.h"
#include "std/err/error.h"
//#include "./internal.h"
#include <std/par/parserialize.h>
#include <std/par/paramslogger.h>
#include <common/measurer.h>

#include <map>

#define KLCS_MODULENAME L"KLPAR"

#ifdef _WIN32
	#pragma comment(lib, "Ws2_32.lib")
	#include <crtdbg.h>
#endif

#ifdef N_PLAT_NLM
    #include "std/conv/wcharcrt.h"
#endif

using namespace KLSTD;
using namespace std;
using namespace KLERR;

#define IMPLEMENT_SOAP_GETTYPE(_type) int _type::getType(){return SOAP_TYPE_##_type;};


IMPLEMENT_SOAP_GETTYPE(xsd__anyType);
IMPLEMENT_SOAP_GETTYPE(xsd__anySimpleType);
IMPLEMENT_SOAP_GETTYPE(xsd__anyURI_);
IMPLEMENT_SOAP_GETTYPE(xsd__boolean_);
IMPLEMENT_SOAP_GETTYPE(xsd__date_);
IMPLEMENT_SOAP_GETTYPE(xsd__dateTime_);
IMPLEMENT_SOAP_GETTYPE(xsd__double_);
IMPLEMENT_SOAP_GETTYPE(xsd__duration_);
IMPLEMENT_SOAP_GETTYPE(xsd__float_);
IMPLEMENT_SOAP_GETTYPE(xsd__time_);
IMPLEMENT_SOAP_GETTYPE(xsd__decimal_);
IMPLEMENT_SOAP_GETTYPE(xsd__integer_);
IMPLEMENT_SOAP_GETTYPE(xsd__long_);
IMPLEMENT_SOAP_GETTYPE(xsd__int_);
IMPLEMENT_SOAP_GETTYPE(xsd__short_);
IMPLEMENT_SOAP_GETTYPE(xsd__byte_);
IMPLEMENT_SOAP_GETTYPE(xsd__nonPositiveInteger_);
IMPLEMENT_SOAP_GETTYPE(xsd__negativeInteger_);
IMPLEMENT_SOAP_GETTYPE(xsd__nonNegativeInteger_);
IMPLEMENT_SOAP_GETTYPE(xsd__positiveInteger_);
IMPLEMENT_SOAP_GETTYPE(xsd__unsignedLong_);
IMPLEMENT_SOAP_GETTYPE(xsd__unsignedInt_);
IMPLEMENT_SOAP_GETTYPE(xsd__unsignedShort_);
IMPLEMENT_SOAP_GETTYPE(xsd__unsignedByte_);
IMPLEMENT_SOAP_GETTYPE(xsd__wstring_);
IMPLEMENT_SOAP_GETTYPE(xsd__string_);
IMPLEMENT_SOAP_GETTYPE(xsd__normalizedString_);
IMPLEMENT_SOAP_GETTYPE(xsd__token_);
IMPLEMENT_SOAP_GETTYPE(xsd__base64Binary);
IMPLEMENT_SOAP_GETTYPE(xsd__hexBinary);
IMPLEMENT_SOAP_GETTYPE(param__arrayvalue);
IMPLEMENT_SOAP_GETTYPE(param__params);

void KLPAR_SerializeToMemoryForSoap(
                                AVP_dword               lFlags,
                                KLPAR::Params*          pParams,
                                struct soap*            pSoap,
                                void*&                  pData,
                                int&                    nData,
                                AVP_dword*              pCRC32);

namespace KLPAR{

	static void* soap_memdup(struct soap* soap, void* src, size_t size)
	{
		KLSTD_ASSERT_THROW((src && size > 0) || (!src && !size));
//		if(!src)return NULL;
		void* dst=soap_malloc(soap, size+1);
		KLSTD_CHKMEM(dst);
		memcpy(dst, src, size);
		((char*)dst)[size]=0;
		return dst;
	};

#define FORSOAP_GET_STRING(_typeid, _xidtype, _type)						\
	case Value::_typeid:													\
	{																		\
		_xidtype* data=soap_instantiate_##_xidtype(soap, 1, NULL, NULL, NULL);	\
		KLSTD_CHKMEM(data);													\
		data->soap_default(soap);											\
		data->__item=soap_strdup2(soap, ((_type*)pValue)->GetValue());		\
		value.data=data;														\
	};																		\
	break;

#define FORSOAP_GET_SIMPLE(_typeid, _xidtype, _type)						\
	case Value::_typeid:													\
	{																		\
		_xidtype* data=soap_instantiate_##_xidtype(soap, 1, NULL, NULL, NULL);	\
		KLSTD_CHKMEM(data);													\
		data->soap_default(soap);											\
		data->__item=((_type*)pValue)->GetValue();							\
		value.data=data;														\
	};																		\
	break;

	void ParamsForSoapI(
				/*[in]*/struct soap*	soap,
				/*[in]*/const Params*	pParams,
				/*[out]*/param__params&	params);

	KLCSC_DECL void ParamsForSoap(
				/*[in]*/struct soap*	soap,
				/*[in]*/const Params*	pParams,
				/*[out]*/param__params&	params,
                bool                    bIsTransportSoap)
    {
        params.soap_default(soap);
        //if( !bIsTransportSoap /*|| KLTR_GetTransport()->IsLocalConnection(soap)*/)
        if(false)
        {
            ParamsForSoapI(soap, pParams, params);
			//checksum
        //KL_TMEASURE_BEGIN(L"ParamsForSoap -- CalcParamsCheckSum", 4)
            params.checksum=KLPAR::CalcParamsCheckSum((KLPAR::Params*)pParams);
        //KL_TMEASURE_END()
        }
        else
        {
            AVP_dword dwCrc32=0;
			//checksum
            KLPAR_SerializeToMemoryForSoap(
                                KLPAR_SF_BINARY,
                                (KLPAR::Params*)pParams,
                                soap,
                                (void*&)params.binfmt.__ptr,
                                params.binfmt.__size,
								&dwCrc32);

            params.checksum=dwCrc32;
            /*
#ifdef _DEBUG
            KLPARLOG_LogParams(3, pParams);
            KLPAR::ParamsPtr pTest;
            KLPAR::ParamsFromSoap(params, &pTest);
#endif
            */
        };
    };

	void ParamsFromSoapI(
				/*[in]*/param__params&	params,
				/*[out]*/Params**		ppParams);


	KLCSC_DECL void ParamsFromSoap(
				/*[in]*/param__params&	params,
				/*[out]*/Params**		ppParams)
    {
    //KL_TMEASURE_BEGIN(L"ParamsFromSoap", 4)
        CAutoPtr<Params> pParams;
        if(!params.binfmt.__ptr)
        {
            ParamsFromSoapI(params, &pParams);
            if(params.checksum)
            {
                AVP_dword dwChecksum=KLPAR::CalcParamsCheckSum(pParams);
				//checksum
                KLSTD_ASSERT(AVP_dword(params.checksum) == dwChecksum);
                if(AVP_dword(params.checksum) != dwChecksum)
                    KLSTD_THROW(KLSTD::STDE_BADFORMAT);
            };
        }
        else
        {
            AVP_dword dwFlags = 0, dwCRC32 = 0;
            DeserializeFromMemory(
                        params.binfmt.__ptr,
                        params.binfmt.__size,
                        dwFlags,
                        &pParams,
                        &dwCRC32);
            /*
            if(KLSTD::IsTraceStarted())
            {
                KLPAR_SerializeToFileName2(
                        KLSTD_CreateGUIDString(),
                        KLPAR_SF_BINARY, 
                        pParams);
                
            };
            */
            //KLPARLOG_LogParams(3, pParams);
            if(params.checksum)
            {
                if(AVP_dword(params.checksum) != dwCRC32)
                {
                    KLSTD_TRACE2(
                            1, 
                            L"Checksum mismatch: %u instead of %u\n",
                            dwCRC32,
                            params.checksum);
                    KLPARLOG_LogParams(3, pParams);
                    KLSTD_ASSERT(!"Checksum mismatch");                    
                    KLSTD_THROW(KLSTD::STDE_BADFORMAT);
                };
            };
        };
        pParams.CopyTo(ppParams);
    //KL_TMEASURE_END()
    }


	KLCSC_DECL void ValueForSoap(
				/*[in]*/struct soap*	soap,
				/*[in]*/const Value*	pValue,
				/*[out]*/param__value&	value)
	{
		value.soap_default(soap);
		if(pValue)
		{
			switch(pValue->GetType())
			{
				FORSOAP_GET_STRING(STRING_T,	xsd__wstring_,	StringValue);
				FORSOAP_GET_SIMPLE(BOOL_T,		xsd__boolean_,	BoolValue);
				FORSOAP_GET_SIMPLE(INT_T,		xsd__int_,		IntValue);
				FORSOAP_GET_SIMPLE(LONG_T,		xsd__long_,		LongValue);
				//@bug GSOAP уродует time_t => придётся не уродовать самому
				//FORSOAP_GET_SIMPLE(DATE_TIME_T,	xsd__dateTime_,	DateTimeValue);
				FORSOAP_GET_STRING(DATE_T,		xsd__date_,		DateValue);
				FORSOAP_GET_SIMPLE(FLOAT_T,		xsd__float_,	FloatValue);
				FORSOAP_GET_SIMPLE(DOUBLE_T,	xsd__double_,	DoubleValue);
			case Value::DATE_TIME_T:
			{
				xsd__dateTime_* data=soap_instantiate_xsd__dateTime_(soap, 1, NULL, NULL, NULL);
				KLSTD_CHKMEM(data);
				data->soap_default(soap);
				;
				time_t t=((DateTimeValue*)pValue)->GetValue();
                struct tm gmtmbuffer;
                struct tm *pTm= ((int(t) >= 0) ? KLSTD_gmtime(&t, &gmtmbuffer) : ((tm*)NULL));
				char szaBuffer[KLSTD_COUNTOF("CCYY-MM-DDTHH:MM:SSZGXXXX") + 16]="";
				if(pTm)
					strftime(szaBuffer, KLSTD_COUNTOF(szaBuffer), "%Y-%m-%dT%H:%M:%SZ", pTm);
//				else
//					KLSTD_ASSERT(false);
				data->__item=soap_strdup2(soap, szaBuffer);
				value.data=data;
			};
			break;
			case Value::BINARY_T:
			{
				xsd__base64Binary* data=soap_instantiate_xsd__base64Binary(soap, 1, NULL, NULL, NULL);
				KLSTD_CHKMEM(data);
				data->soap_default(soap);
				void* src=((BinaryValue*)pValue)->GetValue();
				size_t size=((BinaryValue*)pValue)->GetSize();
				data->__ptr=(unsigned char*)soap_memdup(soap, src, size);
				data->__size=size+1;
				KLSTD_ASSERT_THROW(data->__ptr && data->__size);
				value.data=data;
			};
			break;
			case Value::PARAMS_T:
			{
				param__params* data=soap_instantiate_param__params(soap, 1, NULL, NULL, NULL);
				KLSTD_CHKMEM(data);
				data->soap_default(soap);
				Params* pNewParams=((ParamsValue*)pValue)->GetValue();
				ParamsForSoapI(soap, pNewParams, *data);
				value.data=data;
			};
			break;
			case Value::ARRAY_T:
			{			
				param__arrayvalue* data=soap_instantiate_param__arrayvalue(soap, 1, NULL, NULL, NULL);
				KLSTD_CHKMEM(data);
				data->soap_default(soap);
				;
				int size=((ArrayValue*)pValue)->GetSize();
				;			
				data->__ptr=soap_instantiate_param__value(soap, size, NULL, NULL, NULL);			
				KLSTD_CHKMEM(data->__ptr);
				data->__size=size;
				for(int j=0; j < size; ++j)
					data->__ptr[j].soap_default(soap);
				;			
				for(int i=0; i < size; ++i)
				{
					const Value* pNewValue=((ArrayValue*)pValue)->GetAt(i);
					ValueForSoap(soap, pNewValue, data->__ptr[i]);
				};
				value.data=data;
			};
			break;
			default:
				KLSTD_ASSERT(false);
				KLERR_MYTHROW1(WRONG_VALUE_TYPE, L"");
			};
		};
	};

	void ParamsForSoapI(
				/*[in]*/struct soap*	soap,
				/*[in]*/const Params*	pParams,
				/*[out]*/param__params&	params)
	{
		KLSTD_ASSERT_THROW(soap != NULL);
		params.soap_default(soap);
		if(pParams)
        {
			params.node=soap_instantiate_param__node(soap, 1, NULL, NULL, NULL);
			KLSTD_CHKMEM(params.node);
			params.node->soap_default(soap);
			;
			std::vector<std::wstring> names;
			pParams->GetNames(names);
            ;
			params.node->__ptr=soap_instantiate_param__entry(soap, names.size(), NULL, NULL, NULL);
			KLSTD_CHKMEM(params.node->__ptr);
			params.node->__size=names.size();
			for(int k=0; k < params.node->__size; ++k)
				params.node->__ptr->soap_default(soap);
			;
			for(int j=0; j < params.node->__size; ++j){
				params.node->__ptr[j].name=soap_strdup2(soap, names[j].c_str());
				KLSTD_CHKMEM(params.node->__ptr[j].name);
				const Value* pValue = pParams->GetValue(names[j], true);
				KLSTD_ASSERT_THROW(pValue != NULL);
				ValueForSoap(soap, pValue, params.node->__ptr[j].value);
                KLSTD_ASSERT_THROW(params.node->__ptr[j].value.data != NULL);
			};
		};
	};

	#define FROMSOAP_GET_SIMPLE(_xidtype, _type)						\
			case SOAP_TYPE_##_xidtype:										\
			{															\
				CAutoPtr<_type> pValue;									\
				CreateValue(((_xidtype*)value.data)->__item, &pValue);	\
				pValue.CopyTo((_type**)ppValue);									\
			};															\
			break;


	KLCSC_DECL void ValueFromSoap(
				/*[in]*/param__value&	value,
				/*[out]*/Value**		ppValue)
	{
		KLSTD_CHKOUTPTR(ppValue);
		*ppValue=NULL;
		if(!value.data)
			return;
		switch(value.data->getType())
		{
			FROMSOAP_GET_SIMPLE(xsd__boolean_, BoolValue);
			FROMSOAP_GET_SIMPLE(xsd__wstring_, StringValue);
			FROMSOAP_GET_SIMPLE(xsd__int_, IntValue);
			FROMSOAP_GET_SIMPLE(xsd__long_, LongValue);
			//@bug GSOAP уродует time_t => придётся не уродовать самому
			//FROMSOAP_GET_SIMPLE(xsd__dateTime_, DateTimeValue);
			FROMSOAP_GET_SIMPLE(xsd__date_, DateValue);
			FROMSOAP_GET_SIMPLE(xsd__float_, FloatValue);
			FROMSOAP_GET_SIMPLE(xsd__double_, DoubleValue);
		case SOAP_TYPE_xsd__dateTime_:
			{
				CAutoPtr<DateTimeValue> pValue;
				CAutoPtr<ValuesFactory> pFactory;
				::KLPAR_CreateValuesFactory(&pFactory);
				pFactory->CreateDateTimeValue(&pValue);
				char* szaBuffer=((xsd__dateTime_*)value.data)->__item;
				struct tm T={0};
				time_t t=-1;
				if(szaBuffer && szaBuffer[0])
				{
					sscanf(szaBuffer, "%d-%d-%dT%d:%d:%d", &T.tm_year, &T.tm_mon, &T.tm_mday, &T.tm_hour, &T.tm_min, &T.tm_sec);
					if (T.tm_year <= 1901)
					  t = INT_MIN;
					else if (T.tm_year >= 2038)
					  t = INT_MAX;
					else
					{ T.tm_year -= 1900;
					  T.tm_mon--;
					  t = KLSTD_mkgmtime(&T);
					}
				};
				pValue->SetValue(t);
				pValue.CopyTo((DateTimeValue**)ppValue);
			};
			break;
		case SOAP_TYPE_xsd__base64Binary:
			{
				CAutoPtr<BinaryValue> pValue;
				CAutoPtr<ValuesFactory> pFactory;
				::KLPAR_CreateValuesFactory(&pFactory);
				pFactory->CreateBinaryValue(&pValue);
				void *pdata=((xsd__base64Binary*)value.data)->__ptr;
				int size=((xsd__base64Binary*)value.data)->__size-1;
				KLSTD_ASSERT(pdata);
				pValue->SetValue((size? pdata : NULL), size);
				pValue.CopyTo((BinaryValue**)ppValue);
			};
			break;
		case SOAP_TYPE_param__params:
			{
				CAutoPtr<ParamsValue> pValue;
				CAutoPtr<Params> pParams;
				ParamsFromSoapI((param__params&)*value.data, &pParams);
				CreateValue(pParams, &pValue);
				pValue.CopyTo((ParamsValue**)ppValue);
			};
			break;
		case SOAP_TYPE_param__arrayvalue:
			{
				CAutoPtr<ArrayValue> pValue;
				CAutoPtr<ValuesFactory> pFactory;
				::KLPAR_CreateValuesFactory(&pFactory);
				pFactory->CreateArrayValue(&pValue);
				pValue->SetSize( ((param__arrayvalue*)value.data)->__size);
				for(int i=0; i < ((param__arrayvalue*)value.data)->__size; ++i)
				{
					CAutoPtr<Value> pNewValue;
					ValueFromSoap(
						((param__arrayvalue*)value.data)->__ptr[i],
						&pNewValue);
					pValue->SetAt(i, pNewValue);
				};
				pValue.CopyTo((ArrayValue**)ppValue);
			};
			break;
		default:
			KLSTD_ASSERT(false);
			KLERR_MYTHROW1(WRONG_VALUE_TYPE, L"");
		};
	};

	void ParamsFromSoapI(
				/*[in]*/param__params&	params,
				/*[out]*/Params**		ppParams)
	{
		KLSTD_CHKOUTPTR(ppParams);
		CAutoPtr<Params> pParams;				
		if(params.node)
        {
            ::KLPAR_CreateParams(&pParams);
			for(int i=0; i < params.node->__size; ++i)
			{
				CAutoPtr<Value> pValue;
				ValueFromSoap(params.node->__ptr[i].value, &pValue);
#ifndef KLCSC_STATIC
//                if(!pValue)
  //                  KLTR_FlushRequestsDump();
#endif
                KLSTD_ASSERT_THROW(pValue != NULL);
				pParams->AddValue(params.node->__ptr[i].name, pValue);
			};
		};
		pParams.CopyTo(ppParams);
	};
};

//!\brief Функции серализация структуры заголовка SOAP
void soap_mark_message_desc(struct soap* soap, const struct message_desc_t *desc)
{
	soap_embedded(soap, &desc->requestFlag, SOAP_TYPE_xsd__int);
	soap_embedded(soap, &desc->messageId, SOAP_TYPE_xsd__int);
}

void soap_default_message_desc(struct soap* soap, struct message_desc_t *desc)
{
	desc->requestFlag = 0;
	desc->messageId = 0;
}

int soap_out_message_desc(struct soap* soap, const char*tag, int id, 
						   const struct message_desc_t *desc, const char* type)
{
	soap_element_begin_out(soap, tag, id, type); // print XML beginning tag
	soap_out_xsd__int(soap, c_RequestFlagTag, -1, &desc->requestFlag, "xsd:int");	
	soap_out_xsd__int(soap, c_MessageIdTag, -1, &desc->messageId, "xsd:int");
	soap_element_end_out(soap, tag); // print XML ending tag 

	return SOAP_OK;
}

struct message_desc_t *soap_in_message_desc(struct soap* soap, const char* tag, 
											struct message_desc_t *desc, const char* type)
{
	short soap_flag_code = 1, soap_flag_module = 1, soap_flag_file = 1, soap_flag_line = 1, soap_flag_message = 1;
	if (soap_element_begin_in(soap, tag, 0))
		return NULL;	
	if (soap->null)
	{	
		soap->error = SOAP_NULL;
		return NULL;
	}
	if (*soap->type && soap_match_tag(soap, soap->type, type)) 
	{ 
		soap->error = SOAP_TAG_MISMATCH; 
		return NULL; // type mismatch 
	} 	

	if (*soap->href) 
	{
		desc = (struct message_desc_t *)soap_id_forward(soap, soap->href, desc, SOAP_TYPE_message_desc, 
				SOAP_TYPE_message_desc, sizeof(struct message_desc_t), 0, NULL );
	}
	else if (soap->body) 
	{ 
		soap_default_message_desc(soap, desc );
		for (;;)
		{	soap->error = SOAP_TAG_MISMATCH;
			if (soap_flag_code && soap->error == SOAP_TAG_MISMATCH)
				if (soap_in_xsd__int(soap, c_RequestFlagTag, &desc->requestFlag, "xsd:int"))
				{	soap_flag_code = 0;
					continue;
				}
			if (soap_flag_module && soap->error == SOAP_TAG_MISMATCH)
				if (soap_in_xsd__int(soap, c_MessageIdTag, &desc->messageId, "xsd:int"))
				{	soap_flag_module = 0;
					continue;
				}
			if (soap->error == SOAP_TAG_MISMATCH)
				soap->error = soap_ignore_element(soap);
			if (soap->error == SOAP_NO_TAG)
				break;
			if (soap->error)
			{	return NULL;
			}
		}
	} 

	if (soap->body && soap_element_end_in(soap, tag)) 
      return NULL; 

	if ( desc->requestFlag == 1 ) desc->requestFlag = 0;

	return desc;
}
