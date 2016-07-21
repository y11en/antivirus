#ifndef _PRXSMACROS_H_
#define _PRXSMACROS_H_


#include <std/base/klstd.h>
#include <std/par/params.h>

//////////////////////////////////////////////////////////////////////////

#define DECLARE_LOCK		KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pcsLock;
#define IMPLEMENT_LOCK		KLSTD_CreateCriticalSection(&m_pcsLock);
#define LOCK	{ KLSTD::AutoCriticalSection _acs(m_pcsLock);
#define UNLOCK	}
#define AUTOLOCK	KLSTD::AutoCriticalSection _acs(m_pcsLock);

#define DECLARE_TRANSPORT	KLTRAP::TransportProxy m_TransportProxy; \
	std::wstring m_sDECLTR_ConnectionName; \
	std::wstring m_sDECLTR_LocalComponentName; \

#define INIT_TRANSPORT(cidLocalComponent, cidComponent) { \
	KLTRAP::ConvertComponentIdToTransportName(m_sDECLTR_ConnectionName, cidComponent); \
	KLTRAP::ConvertComponentIdToTransportName(m_sDECLTR_LocalComponentName, cidLocalComponent); \
	m_TransportProxy.Initialize(m_sDECLTR_LocalComponentName.c_str(), m_sDECLTR_ConnectionName.c_str()); \
}

#define BEGIN_SOAP_COMMAND(lock) \
	struct soap* pSoap = NULL; \
	try { \
		lock; \
		pSoap = m_TransportProxy.GetConnectionDesc(); \
		if (!pSoap) KLSTD_THROW(KLSTD::STDE_FAULT);

#define GET_LOCATION	NULL
#define GET_SOAP		pSoap

#define END_SOAP_COMMAND(unlock) unlock; } catch(...) { \
	if (pSoap) m_TransportProxy.ReleaseConnectionDesc(&pSoap); \
	throw; \
} \
if (pSoap) m_TransportProxy.ReleaseConnectionDesc(&pSoap); \

#define CALL_SOAP_COMMAND(command, rcError) \
command; \
m_TransportProxy.CheckResult(&pSoap); \
if (rcError.code != KLSTD::STDE_NOERROR) { \
	KLERR::Error* err = NULL; \
	KLPAR::ExceptionFromSoap(rcError, &err); \
	throw err; \
}

#define CID_TO_SOAP_CID(cid, soap_cid) \
	soap_cid.componentName = (xsd__wstring)cid.componentName.c_str(); \
	soap_cid.version = (xsd__wstring)cid.version.c_str(); \
	soap_cid.productName = (xsd__wstring)cid.productName.c_str(); \
	soap_cid.instanceId = (xsd__wstring)cid.instanceId.c_str();

//////////////////////////////////////////////////////////////////////////

#define GetParamsVal(params, name, type) \
	((KLPAR::type*)(params)->GetValue2(name, true))->GetValue()

#define GetParamsValNE(params, name, type, Var) { \
	KLPAR::type* pVal = ((KLPAR::type*)(params)->GetValue2(name, false)); \
	if (pVal) Var = pVal->GetValue(); \
}

//////////////////////////////////////////////////////////////////////////

#define ADD_PARAMS_VALUE(params, name, type, Var) { \
	KLSTD::CAutoPtr<KLPAR::type> pVal; \
	KLPAR::CreateValue(Var, &pVal); \
	(params)->AddValue(name, pVal); }

#define REPLACE_PARAMS_VALUE(params, name, type, Var) { \
	KLSTD::CAutoPtr<KLPAR::type> pVal; \
	KLPAR::CreateValue(Var, &pVal); \
	(params)->ReplaceValue(name, pVal); }

// !!! No Addref called for Var !!! Must be AutoPtr or manual addref needed
#define GET_PARAMS_VALUE(params, name, type, typeType, Var) { \
	KLSTD::CAutoPtr<KLPAR::type> pVal = (KLPAR::type*)params->GetValue2(name, true); \
	KLPAR_CHKTYPE(pVal, typeType, name); \
	Var = pVal->GetValue(); }

#define GET_PARAMS_VALUE2(params, name, type, typeType, Var) { \
	KLSTD::CAutoPtr<KLPAR::type> pVal = (KLPAR::type*)params->GetValue2(name, true); \
	KLPAR_CHKTYPE2(pVal, typeType, name); \
	Var = pVal->GetValue(); }

#define GET_PARAMS_VALUE_NO_THROW(params, name, type, typeType, Var) { \
	KLSTD::CAutoPtr<KLPAR::type> pVal = (KLPAR::type*)params->GetValue2(name, false); \
	if (pVal) { \
		KLPAR_CHKTYPE(pVal, typeType, name); \
		Var = pVal->GetValue(); \
	}}

#define GET_PARAMS_VALUE_NO_THROW_ALL(params, name, type, typeType, Var) { \
	KLSTD::CAutoPtr<KLPAR::type> pVal = (KLPAR::type*)params->GetValue2(name, false); \
	if (pVal && pVal->GetType() == KLPAR::Value::typeType) { \
		Var = pVal->GetValue(); \
	}}

#define ADD_ARRAY_PARAMS_VALUE(params, name, type, Var) { \
	KLSTD::CAutoPtr<KLPAR::ValuesFactory> pValuesFactory; \
	KLPAR_CreateValuesFactory(&pValuesFactory); \
	KLSTD::CAutoPtr<KLPAR::ArrayValue> valArray; \
	pValuesFactory->CreateArrayValue(&valArray); \
	valArray->SetSize(Var.size()); \
	for(size_t _i_ = 0;_i_ < Var.size();_i_++) { \
		KLSTD::CAutoPtr<KLPAR::type> Val; \
		KLPAR::CreateValue(Var[_i_], &Val); \
		valArray->SetAt(_i_, Val); \
	} \
	(params)->AddValue(name, valArray); \
}

#define REPLACE_ARRAY_PARAMS_VALUE(params, name, type, Var) { \
	KLSTD::CAutoPtr<KLPAR::ValuesFactory> pValuesFactory; \
	KLPAR_CreateValuesFactory(&pValuesFactory); \
	KLSTD::CAutoPtr<KLPAR::ArrayValue> valArray; \
	pValuesFactory->CreateArrayValue(&valArray); \
	valArray->SetSize(Var.size()); \
	for(size_t _i_ = 0;_i_ < Var.size();_i_++) { \
		KLSTD::CAutoPtr<KLPAR::type> Val; \
		KLPAR::CreateValue(Var[_i_], &Val); \
		valArray->SetAt(_i_, Val); \
	} \
	(params)->ReplaceValue(name, valArray); \
}

#define ADD_ARRAY_STR_PARAMS_VALUE(params, name, Var) { \
	if (Var.size() > 0) { \
		KLSTD::CAutoPtr<KLPAR::ValuesFactory> pValuesFactory; \
		KLPAR_CreateValuesFactory(&pValuesFactory); \
		KLSTD::CAutoPtr<KLPAR::ArrayValue> valArray; \
		pValuesFactory->CreateArrayValue(&valArray); \
		valArray->SetSize(Var.size()); \
		for(size_t _i_ = 0;_i_ < Var.size();_i_++) { \
			KLSTD::CAutoPtr<KLPAR::StringValue> Val; \
			KLPAR::CreateValue(Var[_i_].c_str(), &Val); \
			valArray->SetAt((int)_i_, Val); \
		} \
		(params)->AddValue(std::wstring(name), valArray); \
	} \
}

#ifndef N_PLAT_NLM
#define REPLACE_ARRAY_STR_PARAMS_VALUE(params, name, Var) { \
	if (Var.size() > 0) { \
		KLSTD::CAutoPtr<KLPAR::ValuesFactory> pValuesFactory; \
		KLPAR_CreateValuesFactory(&pValuesFactory); \
		KLSTD::CAutoPtr<KLPAR::ArrayValue> valArray; \
		pValuesFactory->CreateArrayValue(&valArray); \
		valArray->SetSize(Var.size()); \
		for(size_t _i_ = 0;_i_ < Var.size();_i_++) { \
			KLSTD::CAutoPtr<KLPAR::StringValue> Val; \
			KLPAR::CreateValue(Var[_i_].c_str(), &Val); \
			valArray->SetAt((int)_i_, Val); \
		} \
		(params)->ReplaceValue(std::wstring(name), valArray); \
	} \
}

template<class T>
void ReplaceArrayStrValue(T itBegin, 
						  T itEnd, 
						  size_t nCount, 
						  const std::wstring& wstrName, 
						  KLPAR::Params* parDest)	
{
	KLSTD_CHKINPTR(parDest);

	if (nCount > 0) 
	{
		KLSTD::CAutoPtr<KLPAR::ValuesFactory> pValuesFactory;
		KLPAR_CreateValuesFactory(&pValuesFactory);
		KLSTD::CAutoPtr<KLPAR::ArrayValue> valArray;
		pValuesFactory->CreateArrayValue(&valArray);
		valArray->SetSize(nCount);
		size_t n = 0;
		for(T i = itBegin;i != itEnd;++i) 
		{
			KLSTD::CAutoPtr<KLPAR::StringValue> Val;
			KLPAR::CreateValue((*i).c_str(), &Val);
			valArray->SetAt(n, Val);
			++n;
		}
		parDest->ReplaceValue(wstrName, valArray);
	}
}
#endif
#define GET_ARRAY_PARAMS_VALUE(params, name, type, Var) { \
	KLSTD::CAutoPtr<KLPAR::ArrayValue> valArray = (KLPAR::ArrayValue*)params->GetValue2(name, true); \
	KLPAR_CHKTYPE(valArray, ARRAY_T, name); \
	Var.clear(); \
	Var.reserve(valArray->GetSize()); \
	for(size_t _i_ = 0;_i_ < valArray->GetSize();_i_++) { \
		KLSTD::CAutoPtr<KLPAR::type> Val = (KLPAR::type*)valArray->GetAt(_i_); \
		Var.push_back(Val->GetValue()); \
	} \
}

#define GET_ARRAY_PARAMS_VALUE_NO_THROW(params, name, type, Var) { \
	Var.clear(); \
	KLSTD::CAutoPtr<KLPAR::ArrayValue> valArray = (KLPAR::ArrayValue*)params->GetValue2(name, false); \
	if (valArray) { \
		KLPAR_CHKTYPE(valArray, ARRAY_T, name); \
		for(size_t _i_ = 0;_i_ < (int)valArray->GetSize();_i_++) { \
			KLSTD::CAutoPtr<KLPAR::type> Val = (KLPAR::type*)valArray->GetAt(_i_); \
			Var.push_back(Val->GetValue()); \
		} \
	} \
}

#define COPY_PARAMS_VALUE(parDst, parSrc, name) {\
	KLSTD::CAutoPtr<KLPAR::Value> pVal = parSrc->GetValue2(name, true); \
	if (pVal) parDst->ReplaceValue(name, pVal); }

#define COPY_PARAMS_VALUE_NO_THROW(parDst, parSrc, name) {\
	KLSTD::CAutoPtr<KLPAR::Value> pVal = parSrc->GetValue2(name, false); \
	if (pVal) parDst->ReplaceValue(name, pVal); }

#define SYNC_PARAMS_VALUE_NO_THROW(parDst, parSrc, name) {\
	KLSTD::CAutoPtr<KLPAR::Value> pVal = parSrc->GetValue2(name, false); \
	if (pVal) parDst->ReplaceValue(name, pVal); else parDst->DeleteValue(name);}

#define ADD_CID_PARAMS_VALUE(params, prefix, Var) \
	ADD_PARAMS_VALUE(params, prefix##_PRODUCT_NAME, StringValue, Var.productName.c_str()); \
	ADD_PARAMS_VALUE(params, prefix##_VERSION, StringValue, Var.version.c_str()); \
	ADD_PARAMS_VALUE(params, prefix##_COMPONENT_NAME, StringValue, Var.componentName.c_str()); \
	ADD_PARAMS_VALUE(params, prefix##_INSTANCE_ID, StringValue, Var.instanceId.c_str());

#define GET_CID_PARAMS_VALUE(params, prefix, Var) \
	GET_PARAMS_VALUE(params, prefix##_PRODUCT_NAME, StringValue, STRING_T, Var.productName); \
	GET_PARAMS_VALUE(params, prefix##_VERSION, StringValue, STRING_T, Var.version); \
	GET_PARAMS_VALUE(params, prefix##_COMPONENT_NAME, StringValue, STRING_T, Var.componentName); \
	GET_PARAMS_VALUE(params, prefix##_INSTANCE_ID, StringValue, STRING_T, Var.instanceId);

#define COMPARE_COMPONENT_ID(cid1, cid2) \
	(cid1.productName == cid2.productName && \
	 cid1.version == cid2.version && \
	 cid1.componentName == cid2.componentName && \
	 cid1.instanceId == cid2.instanceId)

#endif
