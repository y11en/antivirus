/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	s_params.cpp
 * \author	Andrew Kazachkov
 * \date	31.03.2003 12:38:32
 * \brief	
 * 
 */

#include <std/base/klstd.h>
#include <std/err/error.h>
#include <std/err/error_localized2.h>
#include <std/par/params.h>
#include <std/par/helpersi.h>
#include <std/par/par_gsoap.h>
#include "./valuesimpl.h"
#include "soapH.h"

#define KLCS_MODULENAME L"KLPAR"

using namespace KLPAR;
using namespace KLSTD;

namespace KLPAR
{

#define IMPLEMENT_makeArrayValue(_type, _id)								\
			KLCSC_DECL void Create##_id##Array(								\
						const std::vector<_type>&	data,					\
						KLPAR::ArrayValue**			ppArrayValue)			\
	{																		\
		KLSTD_CHKOUTPTR(ppArrayValue);										\
		KLSTD::CAutoPtr<ValuesFactory> pFactory;									\
		KLPAR_CreateValuesFactory(&pFactory);								\
		KLSTD::CAutoPtr<ArrayValue> pArrayValue;									\
		pFactory->CreateArrayValue(&pArrayValue);							\
		pArrayValue->SetSize(data.size());									\
		for(int i=0; i < (int)data.size(); ++i)								\
		{																	\
			KLSTD::CAutoPtr<_id> pValue;											\
			pFactory->Create##_id(&pValue);									\
			pValue->SetValue(data[i]);										\
			pArrayValue->SetAt(i, pValue);									\
		};																	\
		pArrayValue.CopyTo(ppArrayValue);									\
	};

#define IMPLEMENT_makeArrayValueStr(_type, _id)								\
			KLCSC_DECL void Create##_id##Array(								\
						const std::vector<_type>&	data,					\
						KLPAR::ArrayValue**				ppArrayValue)		\
	{																		\
		KLSTD_CHKOUTPTR(ppArrayValue);										\
		KLSTD::CAutoPtr<ValuesFactory> pFactory;									\
		KLPAR_CreateValuesFactory(&pFactory);								\
		KLSTD::CAutoPtr<ArrayValue> pArrayValue;									\
		pFactory->CreateArrayValue(&pArrayValue);							\
		pArrayValue->SetSize(data.size());									\
		for(int i=0; i < (int)data.size(); ++i)								\
		{																	\
			KLSTD::CAutoPtr<_id> pValue;											\
			pFactory->Create##_id(&pValue);									\
			pValue->SetValue(data[i].c_str());								\
			pArrayValue->SetAt(i, pValue);									\
		};																	\
		pArrayValue.CopyTo(ppArrayValue);									\
	};


	IMPLEMENT_makeArrayValue(const wchar_t*, StringValue)
	IMPLEMENT_makeArrayValueStr(std::wstring, StringValue)
	IMPLEMENT_makeArrayValue(long, IntValue)
	IMPLEMENT_makeArrayValue(AVP_longlong, LongValue)
	IMPLEMENT_makeArrayValue(bool, BoolValue)
	IMPLEMENT_makeArrayValue(time_t, DateTimeValue)
	IMPLEMENT_makeArrayValue(const char*, DateValue)
	IMPLEMENT_makeArrayValueStr(std::string, DateValue)
	IMPLEMENT_makeArrayValue(float, FloatValue)
	IMPLEMENT_makeArrayValue(double, DoubleValue)
    


    KLCSC_DECL void CreateStringValueArray(
                const wchar_t**             pData,
                size_t                      nData,
                KLPAR::ArrayValue**			ppArrayValue)
    {
        KLSTD_CHKOUTPTR(ppArrayValue);
        if(nData)
        {
            KLSTD_CHKINPTR(pData);
        };
        KLSTD::CAutoPtr<KLPAR::ArrayValue>      pArrVal;
        {
            KLSTD::CAutoPtr<KLPAR::ValuesFactory>   pFactory;
            KLPAR_CreateValuesFactory(&pFactory);
            pFactory->CreateArrayValue(&pArrVal);
        };        
        pArrVal->SetSize(nData);
        const size_t c_nItems = nData;
        for(size_t ii = 0; ii < c_nItems; ++ii)
        {
            KLSTD::CAutoPtr<KLPAR::StringValue> pString;
            KLPAR::CreateValue(
                        KLSTD::FixNullString(
                                    pData[ii]), &pString);
            pArrVal->SetAt(ii, pString);
        };
        pArrVal.CopyTo(ppArrayValue);
    };


#define DECLARE_TMP_CASE(_case, _type, _member)					\
				case Value::_case:								\
					{											\
						KLSTD::CAutoPtr<_type> pValue;			\
						CreateValue(pData[i]._member, &pValue);	\
						pTheValue=pValue;						\
					};											\
					break;


	KLCSC_DECL void CreateParamsBody(
									param_entry_t*	pData,
									int				nData,
									KLPAR::Params**	ppParams)
	{
		KLSTD_ASSERT(nData);
		KLSTD_CHKINPTR(pData);
		KLSTD_CHKOUTPTR(ppParams);
		KLSTD::CAutoPtr<Params> pParams;
		KLPAR_CreateParams(&pParams);
		for(int i=0; i < nData; ++i){
			KLSTD::CAutoPtr<Value> pTheValue;
			switch(pData[i].m_nType){
				DECLARE_TMP_CASE(STRING_T,		StringValue,	m_szwData);
				DECLARE_TMP_CASE(BOOL_T,		BoolValue,		m_bData);
				DECLARE_TMP_CASE(INT_T,			IntValue,		m_lData);
				DECLARE_TMP_CASE(LONG_T,		LongValue,		m_llData);
				DECLARE_TMP_CASE(DATE_TIME_T,	DateTimeValue,	m_tData);
				DECLARE_TMP_CASE(DATE_T,		DateValue,		m_szaData);
				DECLARE_TMP_CASE(FLOAT_T,		FloatValue,		m_fData);
				DECLARE_TMP_CASE(DOUBLE_T,		DoubleValue,	m_lfData);
				DECLARE_TMP_CASE(PARAMS_T,		ParamsValue,	m_paramsData);
			case Value::EMPTY_T:
				pTheValue=pData[i].m_valueData;
				break;
			default:
				KLSTD_ASSERT(false);
				KLSTD_THROW_BADPARAM(pData);
			};
			if(pTheValue)
				pParams->AddValue(pData[i].m_szwName, pTheValue);
		};
		pParams.CopyTo(ppParams);
	}

	#define IMPLEMENT_CreateValue(_type, _name)					\
		KLCSC_DECL void CreateValue(_type x, _name** ppValue)	\
		{														\
			KLSTD_CHKOUTPTR(ppValue);							\
			if(!g_pFactory)KLSTD_NOINIT(KLCS_MODULENAME);   	\
			*ppValue=NULL;										\
			g_pFactory->Create##_name(ppValue);					\
			(*ppValue)->SetValue(x);							\
		};

	IMPLEMENT_CreateValue(const wchar_t*, StringValue);
	IMPLEMENT_CreateValue(bool, BoolValue);
	IMPLEMENT_CreateValue(long, IntValue);
	IMPLEMENT_CreateValue(AVP_longlong, LongValue);
	IMPLEMENT_CreateValue(time_t, DateTimeValue);
	IMPLEMENT_CreateValue(const char *, DateValue);
	IMPLEMENT_CreateValue(float, FloatValue);
	IMPLEMENT_CreateValue(double, DoubleValue);
	IMPLEMENT_CreateValue(Params*, ParamsValue);

	#undef IMPLEMENT_CreateValue
	
	KLCSC_DECL void CreateValue(const binary_wrapper_t& x, BinaryValue** ppValue)
    {
		KLSTD_CHKOUTPTR(ppValue);
		if(!g_pFactory)KLSTD_NOINIT(KLCS_MODULENAME);
		*ppValue=NULL;
		g_pFactory->CreateBinaryValue(ppValue);
		(*ppValue)->SetValue(const_cast<void*>(x.m_pData), x.m_nData);
    };

	#define IMPLEMENT_GetValue(_T, _type)	\
		KLCSC_DECL void GetValue(Params* pParams, const wchar_t* name, _T** ppValue)\
		{\
            KLSTD_CHKINPTR(pParams);    \
            KLSTD_CHKINPTR(name);       \
            KLSTD_CHKOUTPTR(ppValue);   \
			pParams->GetValue(name, (Value**)ppValue);\
			KLPAR_CHKTYPE(*ppValue, _type, name);\
		};\
		KLCSC_DECL void GetValue(Params* pParams, const std::wstring& name, _T** ppValue)\
		{\
            GetValue(pParams, name.c_str(), ppValue);\
		};
 
	IMPLEMENT_GetValue(StringValue, STRING_T);
	IMPLEMENT_GetValue(BoolValue, BOOL_T);
	IMPLEMENT_GetValue(IntValue, INT_T);
	IMPLEMENT_GetValue(LongValue, LONG_T);
	IMPLEMENT_GetValue(DateTimeValue, DATE_TIME_T);
	IMPLEMENT_GetValue(DateValue, DATE_T);
	IMPLEMENT_GetValue(BinaryValue, BINARY_T);
	IMPLEMENT_GetValue(FloatValue, FLOAT_T);
	IMPLEMENT_GetValue(DoubleValue, DOUBLE_T);
	IMPLEMENT_GetValue(ParamsValue, PARAMS_T);
	IMPLEMENT_GetValue(ArrayValue, ARRAY_T);

	#undef IMPLEMENT_GetValue

	static void Clear(Params* pOldParams, Params* pNewParams)
	{
		if(!pNewParams)
			pOldParams->Clear();
		else{
			std::vector <std::wstring> names;
			pNewParams->GetNames(names);
			for(int i=0; i < (int)names.size(); ++i)
			{
                KLSTD::CAutoPtr<KLPAR::Value> pNewValue;
				pNewParams->GetValue(names[i], &pNewValue);
				if(pNewValue->GetType()!=Value::PARAMS_T)
				{
					pOldParams->DeleteValue(names[i], false);
				}
				else{				
                    KLSTD::CAutoPtr<KLPAR::ParamsValue> pNewValue2=(ParamsValue*)(Value*)pNewValue;
					KLSTD::CAutoPtr<KLPAR::Value> pOldValue;
					pOldParams->GetValue(names[i], &pOldValue);
					KLSTD::CAutoPtr<KLPAR::ParamsValue> pOldValue2;
                    if(pOldValue && pOldValue->GetType() == Value::PARAMS_T)
                        pOldValue2 =(ParamsValue*)(Value*)pOldValue;
					
					if(!pNewValue2->GetValue())
						pOldParams->DeleteValue(names[i], false);
					else if(!pOldValue2->GetValue())
						;//do nothing //KLERR_MYTHROW1(NOT_EXIST, names[i].c_str());
					else
						Clear(pOldValue2->GetValue(), pNewValue2->GetValue());
				};
			};
		};
	};

	KLCSC_DECL void Write(ValuesFactory* pFactory, Params* pOldParams, Params* pNewParams, AVP_dword dwFlags)
	{
		switch(dwFlags)
		{
        case KLSTD::CF_CLEAR: // delete
			{
				Clear(pOldParams, pNewParams);
			};
			break;
		case KLSTD::CF_CREATE_ALWAYS: // Clear
			{
                if(pOldParams != pNewParams)
                {
                    pOldParams->Clear();
				    {
                        KLSTD::CAutoPtr<KLPAR::Params> pTmpParams;
					    pNewParams->Duplicate(&pTmpParams);
					    pOldParams->MoveFrom(pTmpParams);
				    };
                };
				//pOldParams->CopyFrom(pNewParams);
			};
			break;
		case KLSTD::CF_OPEN_EXISTING:	//Update
		case KLSTD::CF_CREATE_NEW:		// Add
		case KLSTD::CF_OPEN_ALWAYS:// Replace
			{
				std::vector <std::wstring> names;
				pNewParams->GetNames(names);
				for(int i=0; i < (int)names.size(); ++i)
				{
                    KLSTD::CAutoPtr<KLPAR::Value> pNewValue;
					pNewParams->GetValue(names[i], &pNewValue);
					if(pNewValue->GetType()!=Value::PARAMS_T)
					{
						switch(dwFlags){
                        case KLSTD::CF_OPEN_EXISTING:
							pOldParams->SetValue(names[i], pNewValue);
							break;
						case KLSTD::CF_CREATE_NEW:
							pOldParams->AddValue(names[i], pNewValue);
							break;
						case KLSTD::CF_OPEN_ALWAYS:
							pOldParams->ReplaceValue(names[i], pNewValue);
							break;
						default:
							KLSTD_ASSERT(false);
							KLSTD_THROW_BADPARAM(dwFlags);
						};
					}
					else{
						KLSTD::CAutoPtr<ParamsValue> pOldValue2;
						if(pOldParams->DoesExist(names[i])){
							KLSTD::CAutoPtr<Value> pOldValue;
							pOldParams->GetValue(names[i], &pOldValue);
							if(pOldValue->GetType()!=Value::PARAMS_T)
								//KLERR_MYTHROW1(KLPAR::WRONG_VALUE_TYPE, names[i].c_str());
								KLPAR::CreateValue(NULL, &pOldValue2);
							else
								pOldValue2=(ParamsValue*)(Value*)pOldValue;
						}
						else{
							pFactory->CreateParamsValue(&pOldValue2);
							pOldParams->AddValue(names[i], pOldValue2);
						};
						KLSTD::CAutoPtr<ParamsValue> pNewValue2=(ParamsValue*)(Value*)pNewValue;
						
						if(!pOldValue2->GetValue() || !pNewValue2->GetValue())
							pOldValue2->SetValue(pNewValue2->GetValue());
		//				else
		//					if()continue;
						else
							Write(pFactory, pOldValue2->GetValue(), pNewValue2->GetValue(), dwFlags);
					};
				};
			};
			break;
		default:
			KLSTD_ASSERT(false);
			KLSTD_THROW_BADPARAM(dwFlags);
		};
	};
};


namespace KLPAR
{
	KLCSC_DECL char* soap_strdup2(struct soap* soap, const char* str)
	{
		KLSTD_ASSERT_THROW(soap != NULL);
		if(!str)
			str="";
		int nLen=strlen(str);
		char* r=(char*)soap_malloc(soap, nLen+1);
        KLSTD_CHKMEM(r);
		memcpy(r, str, nLen+1);
		return r;
	};

	KLCSC_DECL wchar_t* soap_strdup2(struct soap* soap, const wchar_t* str)
	{
		KLSTD_ASSERT_THROW(soap != NULL);
		if(!str)
			str=L"";
		int nLen=wcslen(str);
		wchar_t* r=(wchar_t*)soap_malloc(soap, sizeof(wchar_t)*(nLen+1));
		KLSTD_CHKMEM(r);
		memcpy(r, str, sizeof(wchar_t)*(nLen+1));
		return r;
	};

    KLCSC_DECL KLSTD_NOTHROW char* soap_strdup(struct soap* soap, const char* str) throw()
	{
		KLSTD_ASSERT(soap);
		if(!str)
			str="";
		int nLen=strlen(str);
		char* r=(char*)soap_malloc(soap, nLen+1);
		if(!r)
			return "";
		memcpy(r, str, nLen+1);
		return r;
	};

	KLCSC_DECL KLSTD_NOTHROW wchar_t* soap_strdup(struct soap* soap, const wchar_t* str) throw()
	{
		KLSTD_ASSERT(soap);
		if(!str)
			str=L"";
		int nLen=wcslen(str);
		wchar_t* r=(wchar_t*)soap_malloc(soap, sizeof(wchar_t)*(nLen+1));
		if(!r)
			return L"";
		memcpy(r, str, sizeof(wchar_t)*(nLen+1));
		return r;
	};

	KLSTD_NOTHROW void ExceptionForSoap(
							struct soap*			soap,
                            /*[in]*/KLERR::Error*			pErr_,
							/*[out]*/param_error&	error)throw()
	{
        KLSTD_TRACE0(3, L"ExceptionForSoap. See errorinfo below\n");
        KLERR_SAY_FAILURE(3, pErr_);
        ;
		KLSTD_ASSERT(soap);
        soap_default_param_error(soap, &error);
        KLERR::ErrorPtr pError = pErr_;
		if(!pError)
        {
			KLERR_CreateError(
				&pError,
				L"KLSTD",
				KLSTD::STDE_NOMEMORY,
				__FILE__,
				__LINE__,
				NULL);
        };
		if(pError)
        {
			error.code = pError->GetId();
			error.file = KLPAR::soap_strdup(soap, pError->GetFileName());
			error.line = pError->GetLine();
			error.message = KLPAR::soap_strdup(soap, pError->GetMsg());
			error.module = KLPAR::soap_strdup(soap, pError->GetModuleName());
            if( pError && KLERR_IsErrorLocalized(pError) )
            {
                KLERR::ErrorLocalizationPtr pErrorLocalization;
                pError->GetError2()->QueryInterface(
                            KLSTD_IIDOF(KLERR::ErrorLocalization),
                            (void**)&pErrorLocalization);
                if(pErrorLocalization)
                {
                    error.locdata = soap_new_param_error_loc(soap, 1);
                    error.locdata->soap_default(soap);
                    error.locdata->format_id = pErrorLocalization->LocGetFormatId();
                    error.locdata->locmodule = KLPAR::soap_strdup(
                                            soap,
                                            pErrorLocalization->GetLocModuleName());
                    error.locdata->format = KLPAR::soap_strdup(
                                            soap,
                                            pErrorLocalization->LocGetFormatString());
                    const size_t nLen = pErrorLocalization->LocGetParCount();
                    if(nLen)
                    {
                        error.locdata->args.__ptr = (xsd__wstring*)
                                soap_malloc(soap, nLen*sizeof(xsd__string));
                        for(size_t i = 0; i < nLen; ++i)
                        {
                            error.locdata->args.__ptr[i] = 
                                KLPAR::soap_strdup(
                                    soap, 
                                    pErrorLocalization->LocGetPar(i+1));
                        };
                        error.locdata->args.__size = nLen;
                    };
                };
            };            
		}
		else
            error.code=KLSTD::STDE_NOMEMORY;
	};

	KLSTD_NOTHROW bool ExceptionFromSoap(
						/*[in]*/const param_error&	error,
                        /*[out]*/KLERR::Error**			ppError) throw()
	{
		KLSTD_ASSERT(ppError && (*ppError)==NULL);
        *ppError = NULL;
        KLERR::ErrorPtr pResult;
		if(error.code)
        {
            KLERR_CreateError(
				&pResult,
				error.module,
				error.code,
				error.file,
				error.line,
				error.message);
            ;
            if( error.locdata && 
                    (   error.locdata->format_id || 
                        (   error.locdata->format && 
                            error.locdata->format[0]
                        )
                    )
            )
            {//has localization
                KLSTD::CAutoPtr<KLERR::ErrorLocalization2> pErrorLocalization;
                pResult->GetError2()->QueryInterface(
                        KLSTD_IIDOF(KLERR::ErrorLocalization2),
                        (void**)&pErrorLocalization);
                KLSTD_ASSERT_THROW(pErrorLocalization);
                pErrorLocalization->LocSetInfo2(
                    KLERR::ErrLocAdapt(
                            error.locdata->format_id,
                            (error.locdata->locmodule && error.locdata->locmodule[0] )
                                ?   error.locdata->locmodule
                                :   KLSTD::FixNullString(error.module),
                            error.locdata->args.__size >= 1 ? error.locdata->args.__ptr[0] : NULL,
                            error.locdata->args.__size >= 2 ? error.locdata->args.__ptr[1] : NULL,
                            error.locdata->args.__size >= 3 ? error.locdata->args.__ptr[2] : NULL,
                            error.locdata->args.__size >= 4 ? error.locdata->args.__ptr[3] : NULL,
                            error.locdata->args.__size >= 5 ? error.locdata->args.__ptr[4] : NULL,
                            error.locdata->args.__size >= 6 ? error.locdata->args.__ptr[5] : NULL,
                            error.locdata->args.__size >= 7 ? error.locdata->args.__ptr[6] : NULL,
                            error.locdata->args.__size >= 8 ? error.locdata->args.__ptr[7] : NULL,
                            error.locdata->args.__size >= 9 ? error.locdata->args.__ptr[8] : NULL),
                            KLSTD::FixNullString(error.locdata->format));
                /*
                KLERR_SetErrorLocalization(
                    pResult,
                    error.locdata->format_id,
                    error.locdata->locmodule,
                    error.locdata->args.__size >= 1 ? error.locdata->args.__ptr[0] : NULL,
                    error.locdata->args.__size >= 2 ? error.locdata->args.__ptr[1] : NULL,
                    error.locdata->args.__size >= 3 ? error.locdata->args.__ptr[2] : NULL,
                    error.locdata->args.__size >= 4 ? error.locdata->args.__ptr[3] : NULL,
                    error.locdata->args.__size >= 5 ? error.locdata->args.__ptr[4] : NULL,
                    error.locdata->args.__size >= 6 ? error.locdata->args.__ptr[5] : NULL,
                    error.locdata->args.__size >= 7 ? error.locdata->args.__ptr[6] : NULL,
                    error.locdata->args.__size >= 8 ? error.locdata->args.__ptr[7] : NULL,
                    error.locdata->args.__size >= 9 ? error.locdata->args.__ptr[8] : NULL);
                */
            };
            pResult.CopyTo(ppError);
		};
        if((*ppError))
        {
            KLSTD_TRACE0(3, L"ExceptionFromSoap. See errorinfo below\n");
            KLERR_SAY_FAILURE(3, (*ppError));
        };
		return (*ppError)!=NULL;
	};
};

KLCSC_DECL void KLPAR_GetValue(Params* pMountPoint, const wchar_t** ppPath, const std::wstring& wstrName, Value** ppValue)
{
	KLSTD_CHKINPTR(pMountPoint);
	KLSTD_CHKINPTR(ppPath);
	KLSTD_CHKOUTPTR(ppValue);		
	Params* pRoot=pMountPoint;
	for(const wchar_t** ppCurName=ppPath; *ppCurName; ++ppCurName)
	{
		ParamsValue* pValue=(ParamsValue*)pRoot->GetValue2(*ppCurName, true);
		KLPAR_CHKTYPE(pValue, PARAMS_T, *ppCurName);
		pRoot=(Params*)pValue->GetValue();
	};
	pRoot->GetValue(wstrName, ppValue);
};

KLCSC_DECL void KLPAR_WriteValue(
					Params*				pMountPoint,
					const wchar_t**		ppPath,
					const std::wstring&	wstrName,
					Value*				pValueToAdd,
					AVP_dword			dwFlags)
{
	KLSTD::CAutoPtr<ValuesFactory> pFactory;
	KLPAR_CreateValuesFactory(&pFactory);
	KLSTD::CAutoPtr<Params> pMountPoint2;
	KLPAR_CreateParams(&pMountPoint2);
	KLSTD::CAutoPtr<Params> pRoot=pMountPoint2;		
	for(const wchar_t** ppName=ppPath; *ppName; ++ppName)
	{
		const wchar_t* &szwName=*ppName;
		KLSTD::CAutoPtr<ParamsValue> pValue;
		pFactory->CreateParamsValue(&pValue);
		pRoot->ReplaceValue(szwName, pValue);
		pRoot=pValue->GetValue();
	};
    pRoot->ReplaceValue(wstrName, pValueToAdd);
	/*
    switch(dwFlags)
	{
	case CF_OPEN_EXISTING:
		pRoot->SetValue(wstrName, pValueToAdd);
		break;
	case CF_CREATE_NEW:
		pRoot->AddValue(wstrName, pValueToAdd);
		break;
	case CF_CREATE_ALWAYS:
		pRoot->ReplaceValue(wstrName, pValueToAdd);
		break;
	};	
    */
	Write(pFactory, pMountPoint, pMountPoint2, CF_OPEN_ALWAYS);
    
    //Write(pFactory, pMountPoint, pMountPoint2, dwFlags);    
};

/*!
  \brief	Установка переменой.
 */
KLCSC_DECL void KLPAR_SetValue(
					KLPAR::Params*		pMountPoint,
					const wchar_t**		ppPath,
					const std::wstring&	wstrName,
					KLPAR::Value*		pValue)
{
	KLSTD_CHKINPTR(pMountPoint);
	KLSTD_CHKINPTR(ppPath);
	KLSTD_CHKINPTR(pValue);

	KLPAR_WriteValue(
				pMountPoint,
				ppPath,
				wstrName,
				pValue,
				CF_OPEN_EXISTING);
};

/*!
  \brief	Добавление переменой.
 */
KLCSC_DECL void KLPAR_AddValue(
					KLPAR::Params*		pMountPoint,
					const wchar_t**		ppPath,
					const std::wstring&	wstrName,
					KLPAR::Value*		pValue)
{
	KLSTD_CHKINPTR(pMountPoint);
	KLSTD_CHKINPTR(ppPath);
	KLSTD_CHKINPTR(pValue);

	KLPAR_WriteValue(
				pMountPoint,
				ppPath,
				wstrName,
				pValue,
				CF_CREATE_NEW);
};



/*!
  \brief	Замещение переменой.
 */
KLCSC_DECL void KLPAR_ReplaceValue(
					KLPAR::Params*		pMountPoint,
					const wchar_t**		ppPath,
					const std::wstring&	wstrName,
					KLPAR::Value*		pValue)
{
	KLSTD_CHKINPTR(pMountPoint);
	KLSTD_CHKINPTR(ppPath);
	KLSTD_CHKINPTR(pValue);

	KLPAR_WriteValue(
				pMountPoint,
				ppPath,
				wstrName,
				pValue,
				CF_CREATE_ALWAYS);
};

KLCSC_DECL void KLPAR_ReplaceValueByPath(
                    KLPAR::Params*  pMountPoint,
                    const wchar_t** ppPath,
                    const wchar_t*  szwName,
                    KLPAR::Value*   pValue)
{
    KLSTD_CHKINPTR(szwName);
    KLPAR_ReplaceValue(
                    pMountPoint,
                    ppPath,
                    szwName,
                    pValue);
};

KLCSC_DECL void KLPAR_GetValueByPath(
					KLPAR::Params*	pMountPoint,
					const wchar_t**	ppPath,
					const wchar_t*  szwName,
					KLPAR::Value**	ppValue)
{
    KLSTD_CHKINPTR(szwName);
    KLPAR_GetValue(
                    pMountPoint,
                    ppPath,
                    szwName,
                    ppValue);
};

KLCSC_DECL void KLPAR_Write(
									Params* pOldParams,
									Params* pNewParams,
									AVP_dword dwFlags)
{
	KLSTD::CAutoPtr<KLPAR::ValuesFactory> pFactory;
	::KLPAR_CreateValuesFactory(&pFactory);
	KLPAR::Write(pFactory, pOldParams, pNewParams, dwFlags);
};

namespace KLPAR
{
	KLCSC_DECL std::wstring GetStringValue(Params* pParams, const wchar_t* name)
	{
        KLSTD_CHKINPTR(pParams);
		KLSTD::CAutoPtr<StringValue> pResult;
		GetValue(pParams, name, &pResult);
		const wchar_t* szwResult=pResult->GetValue();
		return  szwResult ? szwResult : L"";
	};

	KLCSC_DECL bool GetBoolValue(Params* pParams, const wchar_t* name)
	{
        KLSTD_CHKINPTR(pParams);
		KLSTD::CAutoPtr<BoolValue> pResult;
		GetValue(pParams, name, &pResult);
		return pResult->GetValue();
	};

	KLCSC_DECL long GetIntValue(Params* pParams, const wchar_t* name)
	{
        KLSTD_CHKINPTR(pParams);
		KLSTD::CAutoPtr<IntValue> pResult;
		GetValue(pParams, name, &pResult);
		return pResult->GetValue();
	};

	KLCSC_DECL AVP_longlong GetLongValue(Params* pParams, const wchar_t* name)
	{
        KLSTD_CHKINPTR(pParams);
		KLSTD::CAutoPtr<LongValue> pResult;
		GetValue(pParams, name, &pResult);
		return pResult->GetValue();
	};

	KLCSC_DECL time_t GetDateTimeValue(Params* pParams, const wchar_t* name)
	{
        KLSTD_CHKINPTR(pParams);
		KLSTD::CAutoPtr<DateTimeValue> pResult;
		GetValue(pParams, name, &pResult);
		return pResult->GetValue();
	};

	KLCSC_DECL std::string GetDateValue(Params* pParams, const wchar_t* name)
	{
        KLSTD_CHKINPTR(pParams);
		KLSTD::CAutoPtr<DateValue> pResult;
		GetValue(pParams, name, &pResult);
		const char* szResult=pResult->GetValue();
		return szResult ? szResult : "";
	};

	KLCSC_DECL float GetFloatValue(Params* pParams, const wchar_t* name)
	{
        KLSTD_CHKINPTR(pParams);
		KLSTD::CAutoPtr<FloatValue> pResult;
		GetValue(pParams, name, &pResult);
		return pResult->GetValue();
	};

	KLCSC_DECL double GetDoubleValue(Params* pParams, const wchar_t* name)
	{
        KLSTD_CHKINPTR(pParams);
		KLSTD::CAutoPtr<DoubleValue> pResult;
		GetValue(pParams, name, &pResult);
		return pResult->GetValue();
	};

	KLCSC_DECL KLSTD::CAutoPtr<Params> GetParamsValue(Params* pParams, const wchar_t* name)
	{
        KLSTD_CHKINPTR(pParams);
		KLSTD::CAutoPtr<ParamsValue> pResult;
		GetValue(pParams, name, &pResult);
		KLSTD::CAutoPtr<Params> pParVal;
		if(pResult)
			pParVal=pResult->GetValue();
		return pParVal;
	};

	KLCSC_DECL KLSTD::CAutoPtr<ArrayValue> GetArrayValue(Params* pParams, const wchar_t* name)
	{
        KLSTD_CHKINPTR(pParams);
		KLSTD::CAutoPtr<ArrayValue> pResult;
		GetValue(pParams, name, &pResult);
		return pResult;
	};
};

