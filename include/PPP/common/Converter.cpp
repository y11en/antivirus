// Converter.cpp : Defines the entry point for the console application.
//

#include "Converter.h"

#include <kca\prss\prssconst.h>
#include <kca\prts\prxsmacros.h>
#include <kca\prts\tasksstorage.h>
#include <std\conv\klconv.h>

#include <Prague/pr_cpp.h>
#include <Prague/iface/e_ktrace.h>

#include <AHTasks/structs/s_ahfw.h>
#include <AntiPhishing/structs/s_antiphishing.h>
#include <AV/structs/s_scaner.h>
#include <ProductCore/structs/s_taskmanager.h>
#include <ProductCore/structs\s_settings.h>
#include <PDM/structs/s_pdm.h>
#include <PPP/structs/s_bl.h>

#include <stdio.h>

//////////////////////////////////////////////////////////////////////

#ifdef KLCS_MODULENAME
	#undef KLCS_MODULENAME
#endif
#define KLCS_MODULENAME L"converter"

#define SADDR(obj,dsc) ( ((tBYTE*)(obj)) + (dsc)->m_offset )

//////////////////////////////////////////////////////////////////////

//    ksa types
//    EMPTY_T,  /*!< пустое значение (значение не определено */
//    STRING_T, /*!< wchar_t строка */  
//    BOOL_T,   /*!< boolean значение */
//    INT_T,    /*!< 32-bit integer */
//    LONG_T,   /*!< 64-bit integer */
//    DATE_TIME_T,  /*!< time_t */
//    DATE_T,   /*!< строка в формате ССYY-MM-DD */
//    BINARY_T, /*!< бинарная последовательность */
//    FLOAT_T,  /*!< 32-bit IEEE float */
//    DOUBLE_T, /*!< 64-bit IEEE float */
//    PARAMS_T, /*!< Вложенный контейнер Params */
//    ARRAY_T   /*!< Вложенный контейнер Array */

struct typetable
{
	tTYPE_ID            m_nPrType;
	KLPAR::Value::Types m_nKcaType;
};

static typetable g_typestable[] =
{
	{tid_BYTE,        KLPAR::Value::INT_T},
	{tid_WORD,        KLPAR::Value::INT_T},
	{tid_DWORD,       KLPAR::Value::INT_T},
	{tid_BOOL,        KLPAR::Value::INT_T},
	{tid_CHAR,        KLPAR::Value::INT_T},
	{tid_WCHAR,       KLPAR::Value::INT_T},
	{tid_ERROR,       KLPAR::Value::INT_T},
//	{tid_PTR,         KLPAR::Value::INT_T},
	{tid_INT,         KLPAR::Value::INT_T},
	{tid_UINT,        KLPAR::Value::INT_T},
	{tid_SBYTE,       KLPAR::Value::INT_T},
	{tid_SHORT,       KLPAR::Value::INT_T},
	{tid_LONG,        KLPAR::Value::INT_T},
//	{tid_IID,         KLPAR::Value::INT_T},
//	{tid_PID,         KLPAR::Value::INT_T},
//	{tid_ORIG_ID,     KLPAR::Value::INT_T},
//	{tid_OS_ID,       KLPAR::Value::INT_T},
//	{tid_VID,         KLPAR::Value::INT_T},
//	{tid_PROPID,      KLPAR::Value::INT_T},
//	{tid_VERSION,     KLPAR::Value::INT_T},
//	{tid_CODEPAGE,    KLPAR::Value::INT_T},
//	{tid_LCID,        KLPAR::Value::INT_T},
//	{tid_DATA,        KLPAR::Value::INT_T},
//	{tid_FUNC_PTR,    KLPAR::Value::INT_T},
//	{tid_IFACEPTR,    KLPAR::Value::INT_T},
//	{tid_OBJECT,      KLPAR::Value::INT_T},
//	{tid_TRACE_LEVEL, KLPAR::Value::INT_T},
//	{tid_TYPE_ID,     KLPAR::Value::INT_T},
	{tid_BINARY,      KLPAR::Value::BINARY_T},
	{tid_BUFFER,      KLPAR::Value::BINARY_T},
	{tid_QWORD,       KLPAR::Value::BINARY_T},
	{tid_LONGLONG,    KLPAR::Value::BINARY_T},
	{tid_DATETIME,    KLPAR::Value::BINARY_T},
//	{tid_EXPORT,      KLPAR::Value::BINARY_T},
//	{tid_IMPORT,      KLPAR::Value::BINARY_T},
	{tid_DOUBLE,      KLPAR::Value::BINARY_T},
	{tid_STRING,      KLPAR::Value::STRING_T},
	{tid_WSTRING,     KLPAR::Value::STRING_T},
	{tid_STRING_OBJ,  KLPAR::Value::STRING_T}
};

bool FindUnsupported(cAKUnsupportedFields& unsup, tDWORD sid, const cStrObj& name)
{
	for (tDWORD i = 0, count = unsup.count(); i < count; ++i)
	{
		if (unsup[i].m_serId == sid && unsup[i].m_sFieldName == name)
			return true;
	}
	return false;
}

void AddUnsupported(cAKUnsupportedFields& unsup, tDWORD sid, const cStrObj& name)
{
	if (FindUnsupported(unsup, sid, name))
		return;
	cAKUnsupportedField& ufield = unsup.push_back();
	ufield.m_serId = sid;
	ufield.m_sFieldName = name;
}


//////////////////////////////////////////////////////////////////////
// KlPar

tERROR KlPar::SetValue(const wchar_t *strName, KLPAR::Value *pValue, bool bReplace)
{
	m_pParams->ReplaceValue(strName, pValue);
	return errOK;
}

tERROR KlPar::SetValue(const wchar_t *strName, const wchar_t *strValue)
{
	KLSTD::CAutoPtr<KLPAR::StringValue> pValue;
	KLPAR::CreateValue(strValue, &pValue);
	return SetValue(strName, pValue);
}

//tERROR KlPar::SetValue(const wchar_t *strName, bool bValue);
//tERROR KlPar::SetValue(const wchar_t *strName, int nValue);
//tERROR KlPar::SetValue(const wchar_t *strName, __int64 nValue);
//tERROR KlPar::SetValue(const wchar_t *strName, time_t tValue);
//tERROR KlPar::SetValue(const wchar_t *strName, void *pValue, size_t nSize);
//tERROR KlPar::SetValue(const wchar_t *strName, float nValue);
//tERROR KlPar::SetValue(const wchar_t *strName, double nValue);
//tERROR KlPar::SetValue(const wchar_t *strName, KLPAR::Params *pParams);

KLPAR::Value *KlPar::GetValue(const wchar_t *strName, KLPAR::Value::Types tValueType)
{
	KLPAR::Value *pVal = NULL;
	GetValue(strName, &pVal, tValueType);
	return pVal;
}

tERROR KlPar::GetValue(const wchar_t *strName, KLPAR::Value **ppValue, KLPAR::Value::Types tValueType)
{
	*ppValue = NULL;
	
	if( !m_pParams )
		return errPARAMETER_INVALID;
	
	if( !m_pParams->GetValueNoThrow(strName, ppValue) )
		return errNOT_FOUND;

	if( tValueType != KLPAR::Value::EMPTY_T )
	{
		if( (*ppValue)->GetType() != tValueType )
		{
			*ppValue = NULL;
			return errBAD_TYPE;
		}
	}
	
	return errOK; 
}

tERROR KlPar::GetValue(const wchar_t *strName, wstring &strValue)
{
	strValue.erase();
	
	KLPAR::StringValue *pVal = NULL;
	tERROR err = GetValue(strName, (KLPAR::Value **)&pVal, KLPAR::Value::STRING_T);
	if( PR_SUCC(err) )
		strValue = pVal->GetValue();
	
	return err;
}

//tERROR KlPar::GetValue(const wchar_t *strName, bool &bValue);
//tERROR KlPar::GetValue(const wchar_t *strName, int &nValue);
//tERROR KlPar::GetValue(const wchar_t *strName, __int64 &nValue);
//tERROR KlPar::GetValue(const wchar_t *strName, time_t &tValue);
//tERROR KlPar::GetValue(const wchar_t *strName, void *pValue, size_t &nSize);
//tERROR KlPar::GetValue(const wchar_t *strName, float &nValue);
//tERROR KlPar::GetValue(const wchar_t *strName, double &nValue);

tERROR KlPar::GetValue(const wchar_t *strName, KLPAR::Params **ppParams)
{
	*ppParams = NULL;
	
	KLPAR::ParamsValue *pVal = NULL;
	tERROR err = GetValue(strName, (KLPAR::Value **)&pVal, KLPAR::Value::PARAMS_T);
	if( PR_SUCC(err) )
		*ppParams = pVal->GetValue();
	
	return err;
}

tERROR KlPar::GetArrayValue(KLPAR::ArrayValue *pArray, size_t nIdx, KLPAR::Value **ppValue, KLPAR::Value::Types tValueType)
{
	*ppValue = NULL;
	
	if( !pArray )
		return errPARAMETER_INVALID;

	if( nIdx >= pArray->GetSize() )
		return errPARAMETER_INVALID;

	*ppValue = const_cast<KLPAR::Value *>(pArray->GetAt(nIdx));

	if( tValueType != KLPAR::Value::EMPTY_T )
	{
		if( (*ppValue)->GetType() != tValueType )
		{
			*ppValue = NULL;
			return errBAD_TYPE;
		}
	}
	
	return errOK;
}

tERROR KlPar::GetArrayValue(KLPAR::ArrayValue *pArray, size_t nIdx, KLPAR::Params **ppParams)
{
	*ppParams = NULL;

	KLPAR::ParamsValue *pValue = NULL;
	tERROR err = GetArrayValue(pArray, nIdx, (KLPAR::Value **)&pValue, KLPAR::Value::PARAMS_T);
	if( PR_SUCC(err) )
		*ppParams = pValue->GetValue();

	return err;
}

KLPAR::Params * KlPar::Lookup(eFlags nFlags, const wchar_t *strParams)
{
	KLPAR::Params *pChild = NULL;
	tERROR err = GetValue(strParams, &pChild);
	if( PR_SUCC(err) )
		return pChild;

	if( nFlags & fCreateIfNotExist )
	{
	}
	return pChild;
}

KLPAR::Params * KlPar::Lookup(eFlags nFlags, size_t nArgs, ...)
{
	KlPar pCurent = *this;
	
	va_list args;
	va_start(args, nArgs);

	for(size_t i = 0; !!pCurent && i < nArgs; i++)
		pCurent = pCurent.Lookup(nFlags, va_arg(args, const wchar_t *));

	va_end(args);

	return pCurent;
}

//KLPAR::Params * KlPar::Lookup(eLookupFlags nFlags, wstrings &path);

ParamsPtr KlPar::Create()
{
	ParamsPtr par;
	KLPAR_CreateParams(&par);
	return par;
}

KLPAR::Params *KlPar::Create(const wchar_t *strName, eFlags nFlags)
{
	ParamsPtr pParams;
	if( !pParams && nFlags & fCreateIfNotExist )
	{
		KLSTD::CAutoPtr<KLPAR::ParamsValue> pValue;
		KLPAR::CreateValue(pParams = Create(), &pValue);
		if( PR_FAIL(SetValue(strName, pValue)) )
			pParams = NULL;
	}
	return pParams;
}
	
//void KlPar::Trace(const wchar_t *strFilePrefix, const wchar_t *strOp, const KLPAR::Params *pParams);
	
//////////////////////////////////////////////////////////////////////
// KLParams

class KLParamsAutoDeleteValue
{
public:
	KLParamsAutoDeleteValue() : m_pContainer(NULL), m_sName(NULL) {}
	~KLParamsAutoDeleteValue() { if( m_pContainer && m_sName ) m_pContainer->DeleteValue(m_sName); }
	void Init(KLParams *pContainer, const char *sName) { m_pContainer = pContainer; m_sName = sName; }

protected:
	KLParams   *m_pContainer;
	const char *m_sName;
};

void KLParams::Create()
{
	m_pParams.Release();
	KLPAR_CreateParams(&m_pParams);
}

KLParams KLParams::CreateSubValue(const char *sName, cSerializable* pStruct, bool bNameMustExist, bool bCreate, bool bMustExist)
{
	if( !(sName && *sName) )
		return bNameMustExist ? NULL : KLParams(m_pParams, pStruct, *this);

	KLSTD_USES_CONVERSION;
	const wchar_t *swName = KLSTD_A2CW(sName);
	
	KLSTD::CAutoPtr<KLPAR::ParamsValue> pValue = (KLPAR::ParamsValue *)GetValue(swName);
	if( pValue )
	{
		if( pValue->GetType() != KLPAR::Value::PARAMS_T )
			return NULL;
		return KLParams(pValue->GetValue(), pStruct, *this);
	}

	if( !bCreate )
	{
		if (bMustExist && m_pUnsupportedFields)
			AddUnsupported(*m_pUnsupportedFields, m_pStruct->getIID(), sName);
		return NULL;
	}

	if (bMustExist && m_pStruct && m_pUnsupportedFields && FindUnsupported(*m_pUnsupportedFields, m_pStruct->getIID(), sName))
		return NULL;

	KLSTD::CAutoPtr<KLPAR::ParamsValue> pParamsValue;
	KLSTD::CAutoPtr<KLPAR::ValuesFactory> pValuesFactory;
	KLPAR_CreateValuesFactory(&pValuesFactory);
	pValuesFactory->CreateParamsValue(&pParamsValue);
	
	SetValue(sName, pParamsValue);
	return KLParams(pParamsValue->GetValue(), pStruct, *this);
}

//////////////////////////////////////////////////////////////////////

tERROR KLParams::SetValue(const wchar_t *sName, KLPAR::Value *pValue)
{
	KLERR_TRY
		m_pParams->ReplaceValue(sName, pValue);
	KLERR_CATCH( pError )
		return errOBJECT_NOT_CREATED;
	KLERR_ENDTRY
	
	return errOK;
}

tERROR KLParams::SetValue(const char *sName, KLPAR::Value *pValue)
{
	KLSTD_USES_CONVERSION;
	
	if( !(m_nFlags & CNV_USE_POLICY) )
		return SetValue(KLSTD_A2W(sName), pValue);

	bool bLocked = false;
	bool bMandatoried = false;
	if( m_pStruct && m_pStruct->isBasedOn(cPolicySettings::eIID) )
	{
		cPolicySettings *pSett = (cPolicySettings *)m_pStruct;
		bLocked = pSett->IsLocked(sName);
		bMandatoried = pSett->IsMandatoried(sName);
	}
	if (m_bMandatoried)
		bMandatoried = true;

	if( (m_nFlags & CNV_SER_UNLOCKED_ONLY) && bLocked )
		return errOK;

	KLParams SPSval(m_pController, m_pParams, NULL, m_nFlags & ~CNV_USE_POLICY);
//	SPSval = SPSval.OpenSubValue(sName, true);
	SPSval = SPSval.CreateSubValue(sName, m_pStruct, true, true);
	if( !SPSval )
		return errOBJECT_NOT_CREATED;

	tERROR err = SPSval.SetValue(KLPRSS::c_szwSPS_Value, pValue);

	REPLACE_PARAMS_VALUE(SPSval.m_pParams, KLPRSS::c_szwSPS_Mandatory, BoolValue, bMandatoried);
	return err;
}

tERROR KLParams::SetValue(const char *sName, const char *Value)
{
	KLSTD_USES_CONVERSION;
	return SetValue(sName, KLSTD_A2W(Value));
}

tERROR KLParams::SetValue(const char *sName, const wchar_t *Value)
{
	KLSTD::CAutoPtr<KLPAR::StringValue> pValue;
	KLPAR::CreateValue(Value, &pValue);
	return SetValue(sName, pValue);
}

tERROR KLParams::SetValue(const char *sName, DWORD Value)
{
	KLSTD::CAutoPtr<KLPAR::IntValue> pValue;
	KLPAR::CreateValue(Value, &pValue);
	return SetValue(sName, pValue);
}

tERROR KLParams::SetValue(const char *sName, const void *Value, DWORD nSize)
{
	KLSTD::CAutoPtr<KLPAR::BinaryValue> pValue;
	KLPAR::CreateValue(KLPAR::binary_wrapper_t(Value, nSize), &pValue);
	return SetValue(sName, pValue);
}

tERROR KLParams::SetValue(const char *sName, tTYPE_ID nType, tPTR pValue, tDWORD nCount)
{
	switch(PrType2KcaType(nType))
	{
	case KLPAR::Value::INT_T:
		{
			tDWORD nValue;
			switch(pr_sizeometer(nType, 0))
			{
			case sizeof(tBYTE):  nValue = *(tBYTE  *)pValue; break;
			case sizeof(tWORD):	 nValue = *(tWORD  *)pValue; break;
			case sizeof(tDWORD): nValue = *(tDWORD *)pValue; break;
			default:             return errPARAMETER_INVALID;
			}

			return SetValue(sName, nValue);
		}

	case KLPAR::Value::STRING_T:
		switch(nType)
		{
		case tid_STRING:     return SetValue(sName, (const char *)pValue);
		case tid_WSTRING:    return SetValue(sName, (const wchar_t *)pValue);
		case tid_STRING_OBJ: return SetValue(sName, ((cStringObj *)pValue)->data());
		}
	
	case KLPAR::Value::BINARY_T:
		{
			tDWORD nSize = pr_sizeometer(nType, 0);
			return SetValue(sName, pValue, nSize == (tDWORD)-1 ? nCount : nSize);
		}
	}
	return errOBJECT_INCOMPATIBLE;
}

//////////////////////////////////////////////////////////////////////

KLPAR::Value *KLParams::GetValue(const wchar_t *sName) const
{
	KLSTD::CAutoPtr<KLPAR::Value> pValue;
	m_pParams->GetValueNoThrow(sName, &pValue);

	if((m_nFlags & CNV_USE_POLICY) && pValue && pValue->GetType() == KLPAR::Value::PARAMS_T)
	{
		PParams SubParam = ((KLPAR::ParamsValue *)pValue.operator->())->GetValue();

		bool bLocked = false;
		bool bMandatory = false;
		
		if(SubParam->DoesExist(KLPRSS::c_szwSPS_Locked))
			bLocked = KLPAR::GetBoolValue(SubParam, KLPRSS::c_szwSPS_Locked);
		if(SubParam->DoesExist(KLPRSS::c_szwSPS_Mandatory))
			bMandatory = KLPAR::GetBoolValue(SubParam, KLPRSS::c_szwSPS_Mandatory);

		if( m_pStruct && m_pStruct->isBasedOn(cPolicySettings::eIID) )
		{
			KLSTD_USES_CONVERSION;
			char *cname = KLSTD_W2A(sName);
			((cPolicySettings *)m_pStruct)->SetLocked(cname, bLocked);
			((cPolicySettings *)m_pStruct)->SetMandatoried(cname, bMandatory);
		}

		if(SubParam->DoesExist(KLPRSS::c_szwSPS_Value))
		{
			pValue.Release();
			SubParam->GetValueNoThrow(KLPRSS::c_szwSPS_Value, &pValue);
		}

		if((pValue->GetType() != KLPAR::Value::PARAMS_T) && (m_nFlags & CNV_DES_LOCKED_ONLY) && !bMandatory)
			return NULL;
	}
	
	return pValue;
}

KLPAR::Value *KLParams::GetValue(const char *sName) const
{
	KLSTD_USES_CONVERSION;
	return GetValue(KLSTD_A2W(sName));
}

tERROR KLParams::GetValue(const char *sName, wstring &str) const
{
	PValue pVal = GetValue(sName);
	if(!pVal)
		return errPARAMETER_INVALID;
	
	if(pVal->GetType() != KLPAR::Value::STRING_T)
		return errPARAMETER_INVALID;

	str = ((KLPAR::StringValue *)(pVal.operator->()))->GetValue();

	return errOK;
}

tERROR KLParams::GetValue(const char *sName, tTYPE_ID nType, tPTR pValue, tDWORD nCount) const
{
	PValue pVal = GetValue(sName);
	if( !pVal )
		return errNOT_FOUND;

	if( PrType2KcaType(nType) != pVal->GetType() )
		return errOBJECT_INCOMPATIBLE;
	
	switch(pVal->GetType())
	{
	case KLPAR::Value::INT_T:
		{
			tDWORD nValue = ((KLPAR::IntValue *)(pVal.operator->()))->GetValue();
			switch(pr_sizeometer(nType, 0))
			{
			case sizeof(tBYTE):  *(tBYTE  *)pValue = nValue; break;
			case sizeof(tWORD):	 *(tWORD  *)pValue = nValue; break;
			case sizeof(tDWORD): *(tDWORD *)pValue = nValue; break;
			default:             return errPARAMETER_INVALID;
			}
			return errOK;
		}
	case KLPAR::Value::BINARY_T:
		{
			tDWORD nSize = pr_sizeometer(nType, 0);
			nSize = nSize == (tDWORD)-1 ? nCount : nSize;
			
			KLPAR::BinaryValue *pBinVal = (KLPAR::BinaryValue *)pVal.operator->();
			if( nSize < pBinVal->GetSize() )
				return errBAD_SIZE;
			
			memcpy(pValue, pBinVal->GetValue(), nSize);
			return errOK;
		}
	case KLPAR::Value::STRING_T:
		{
			tCODEPAGE cp = cCP_UNICODE;
			wstring strValue = ((KLPAR::StringValue *)(pVal.operator->()))->GetValue();
			switch(nType)
			{
			case tid_STRING_OBJ:
				*(cStringObj *)pValue = strValue.c_str(); return errOK;			
			case tid_STRING:
			case tid_WSTRING:
				return errNOT_SUPPORTED;
			}
		}
	}
	return errOBJECT_INCOMPATIBLE;
}

tERROR KLParams::GetValueSize(const char *sName, tDWORD &nSize) const
{
	PValue pVal = GetValue(sName);
	if( !pVal )
		return errNOT_FOUND;

	if( pVal->GetType() != KLPAR::Value::BINARY_T )
		return errOBJECT_INCOMPATIBLE;

	KLPAR::BinaryValue *pBinVal = (KLPAR::BinaryValue *)pVal.operator->();

	nSize = pBinVal->GetSize();

	return errOK;
}

tERROR KLParams::DeleteValue(const char *sName)
{
	KLSTD_USES_CONVERSION;
	return m_pParams->DeleteValue(KLSTD_A2W(sName)) ? errOK : errNOT_OK;
}

bool KLParams::RemoveEmptyContainers()
{
	std::vector<std::string> names; GetNames(names);
	if( !names.size() )
		return true;

	bool bEmpty = true;
	for(size_t i = 0; i < names.size(); i++)
	{
		const char *sName = names[i].c_str();
		KLParams sub = CreateSubValue(sName, NULL, true, false);
		if( !sub )
		{
			bEmpty = false;
			continue;
		}
		if( sub.RemoveEmptyContainers() )
		{
			if( PR_FAIL(DeleteValue(sName)) )
				bEmpty = false;
		}
		else
			bEmpty = false;
	}
	return bEmpty;
}

//////////////////////////////////////////////////////////////////////

tERROR KLParams::Serialize(cSerializable* pStruct, tDWORD unique)
{
	if(IsEmpty())
		Create();

	if( pStruct )
		m_pStruct = pStruct;

	if( !m_pStruct )
		return errPARAMETER_INVALID;

	if( unique == SERID_UNKNOWN )
		unique = m_pStruct->getIID();

	const cSerDescriptor* dsc = GetDescrByStructId(unique);
	if(!dsc)
		return errOBJECT_NOT_FOUND;

	tERROR err = SerializeStr(dsc->m_unique, false);

	if( PR_FAIL(err) )
	{
		PR_TRACE((g_root, prtERROR, "CNV\tKLParams::Serialize. failed (%terr)", err));
	}

	return err;
}

tERROR KLParams::Deserialize(cSerializable*& pStruct, tDWORD unique)
{
	cERROR   err;

	if(IsEmpty())
		return errPARAMETER_INVALID;

	m_pStruct = pStruct;

	if( unique == SERID_UNKNOWN )
	{
		if( m_pStruct )
			unique = m_pStruct->getIID();
		else
		{
			if( PR_FAIL(err = GetValue(SER_UNIQUE_KEY_NAME, tid_DWORD, (tPTR)&unique, sizeof(unique))) )
			{
				PR_TRACE((g_root, prtERROR, "CNV\tKLParams::Deserialize. invalid serid"));
				return err;
			}
		}
	}

	const cSerDescriptor *dsc = GetDescrByStructId(unique);
	if ( !dsc )
		return err = errOBJECT_NOT_FOUND;

	if( !m_pStruct && PR_FAIL(err = g_root->CreateSerializable(unique, &m_pStruct)) )
	{
		PR_TRACE((g_root, prtERROR, "CNV\tKLParams::Deserialize. can't create struct (%terr)", err));
		return err;
	}

	if( PR_SUCC(err = DeserializeStr(unique)) )
	{
		pStruct = m_pStruct;
	}
	else
	{
		PR_TRACE((g_root, prtERROR, "CNV\tKLParams::Deserialize. failed (%terr)", err));
	}

	return err;
}

//////////////////////////////////////////////////////////////////////

tERROR KLParams::SerializeStr(tUINT id, bool bPutId)
{
	if( IsEmpty() )
		return errNOT_FOUND;

	if( bPutId )
		SetValue(SER_UNIQUE_KEY_NAME, id);

	const cSerDescriptorField* field = GetFieldsByStructId(id);
	if( !field )
		return errNOT_FOUND;
	
	cERROR err;
	for(; PR_SUCC(err) && (field->m_flags != SDT_FLAG_ENDPOINT); field++)
		if( IsFieldSerializable(m_pStruct, field) )
			err = SerializeField(field);
	
	return err;
}

tERROR KLParams::SerializeField(const cSerDescriptorField* field)
{
	// --------------  special case for Product profile --------------
	// do not serialize child profiles of root profile (Protection)
	if( field->m_id == cProfileExList::eIID && m_pStruct->isBasedOn(cProfileEx::eIID) && ((cProfileEx*)m_pStruct)->m_sName == AVP_PROFILE_PRODUCT )
		if( !(m_nFlags & CNV_SER_CHILD_PROFILES) )
			return errOK;
	// --------------  special case for Product profile --------------

	bool  bBase    = !!(field->m_flags & SDT_FLAG_BASE);
	bool  bVector  = !!(field->m_flags & SDT_FLAG_VECTOR);
	bool  bPointer = !!(field->m_flags & SDT_FLAG_POINTER);
	bool  bStruct  = IS_SERIALIZABLE(field->m_id);
	tUINT nUnique  = field->m_id;
	tPTR  pData    = SADDR(m_pStruct, field);
	

	if( bVector )
	{
		if( field->m_id == cProfileEx::eIID )
			return SerializeProfiles(field);
		return SerializeVector(field);
	}
	else
	{
		if( !bStruct )
			return SerializeIntegrated(field);

		if( bPointer )
		{
			pData = *((tPTR *)pData);
			if( !pData )
				return errOK;
			nUnique = ((cSerializable *)pData)->getIID();
		}

		KLParams parStruct = CreateSubValue(field->m_name, (cSerializable *)pData, bPointer, true, true);
		if( !parStruct )
			return errOK;

		bool bMandatoried = false;
		if( m_pStruct && m_pStruct->isBasedOn(cPolicySettings::eIID) )
		{
			cPolicySettings *pSett = (cPolicySettings *)m_pStruct;
			if (pSett->IsMandatoried(field->m_name))
				parStruct.m_bMandatoried = true;
		}
		
		return parStruct.SerializeStr(nUnique, bPointer);
	}
	return errOK;
}

tERROR KLParams::SerializeVector(const cSerDescriptorField* field)
{
	if( !(field->m_name && *field->m_name) )
		return errOK;

	if (m_pUnsupportedFields && FindUnsupported(*m_pUnsupportedFields, m_pStruct->getIID(), field->m_name))
		return errOK;

	bool  bBase    = !!(field->m_flags & SDT_FLAG_BASE);
	bool  bVector  = !!(field->m_flags & SDT_FLAG_VECTOR);
	bool  bPointer = !!(field->m_flags & SDT_FLAG_POINTER);
	bool  bStruct  = IS_SERIALIZABLE(field->m_id);
	tUINT nUnique  = field->m_id;
	tPTR  pData    = SADDR(m_pStruct, field);
	tMemChunk &v   = *(tMemChunk *)pData;

	if( field->m_flags & SDT_FLAG_VSTREAM && !bStruct )
	{
		return SetValue(field->m_name, tid_BINARY, v.m_ptr, v.m_used);
	}
	
	KLSTD::CAutoPtr<KLPAR::ValuesFactory> pValuesFactory;
	KLPAR_CreateValuesFactory(&pValuesFactory);

	KLSTD::CAutoPtr<KLPAR::ArrayValue> pArrayValue;
	pValuesFactory->CreateArrayValue(&pArrayValue);

	tUINT nVectorElementSize = bPointer ? sizeof(tPTR) : pr_sizeometer(nUnique, 0);
	tUINT nVectorElementsNum = v.m_ptr ? (v.m_used / nVectorElementSize) : 0;

	if( nVectorElementsNum )
	{
		pArrayValue->SetSize(nVectorElementsNum);

		tERROR err = errOK;
		for(tUINT i = 0; PR_SUCC(err) && i < nVectorElementsNum; i++)
		{
			pData = (tPTR)((tBYTE *)v.m_ptr + i * nVectorElementSize);
			if( bPointer )
			{
				pData = *((tPTR *)pData);
				if( !pData )
					continue;
				nUnique = ((cSerializable *)pData)->getIID();
			}
			
			KLParams parVectorElement(NULL, NULL, 0, m_pController, m_pUnsupportedFields);
			parVectorElement.Create();
			if( bStruct )
			{
				KLParams parStruct = parVectorElement.CreateSubValue("value", NULL, true, true, true);
				parStruct.Serialize((cSerializable *)pData);
			}
			else
			{
				parVectorElement.SerializeIntegrated(field, "value", pData);
			}
			pArrayValue->SetAt(i, parVectorElement.GetValue(L"value"));
		}
	}

	SetValue(field->m_name, pArrayValue);

	return errOK;
}

tERROR KLParams::SerializeProfiles(const cSerDescriptorField* field)
{
	// note: vector value should be created even if vector is empty
	// because of the mundatory & locked attributes should present
	
	bool  bBase    = !!(field->m_flags & SDT_FLAG_BASE);
	bool  bVector  = !!(field->m_flags & SDT_FLAG_VECTOR);
	bool  bPointer = !!(field->m_flags & SDT_FLAG_POINTER);
	bool  bStruct  = IS_SERIALIZABLE(field->m_id);
	tUINT nUnique  = field->m_id;
	tPTR  pData    = SADDR(m_pStruct, field);
	tMemChunk &v   = *(tMemChunk *)pData;

	if( !bStruct && !bPointer )
		return errOK;

	KLParams parVector = CreateSubValue(field->m_name, m_pStruct, !(bBase && bPointer), true, true);
	if( !parVector )
		return errOK;

	tUINT nVectorElementSize = bPointer ? sizeof(tPTR) : pr_sizeometer(nUnique, 0);
	tUINT nVectorElementsNum = v.m_ptr ? (v.m_used / nVectorElementSize) : 0;
	if( !nVectorElementsNum )
		return errOK;
	
	tERROR err = errOK;
	for(tUINT i = 0; PR_SUCC(err) && i < nVectorElementsNum; i++)
	{
		pData = (tPTR)((tBYTE *)v.m_ptr + i * nVectorElementSize);
		if( bPointer )
		{
			pData = *((tPTR *)pData);
			if( !pData )
				continue;
			nUnique = ((cSerializable *)pData)->getIID();
		}
		if( bStruct )
		{
			KLParams parVectorElement = parVector.CreateSubValue(GetKeyName(pData, i).c_str(), (cSerializable *)pData, true, true, true);
			if( !parVectorElement )
				return errOK;
		
			if (m_pUnsupportedFields && ((cSerializable*)pData)->isBasedOn(cProfile::eIID))
				parVectorElement.SetAKUnsupportedFields(&((cProfile*)pData)->m_cfg.m_aAKUnsupportedFields);

			// c PutId будут проблемы, т.к. а АК здесь лежит CProfileAdm, а в продукте
			// cProfileEx, соответственно не будут сходиться unique_id, что приводит к 
			// перевдвиганию политики!
			// PutId не нужен, т.к. при десериализации здесь будет cProfileEx
			err = parVectorElement.SerializeStr(nUnique, false);
		}
		else
		{
			err = parVector.SerializeIntegrated(field, GetKeyName(NULL, i).c_str(), pData);
		}
	}
	return err;
	return errOK;
}

tERROR KLParams::SerializeIntegrated(const cSerDescriptorField* field, const char *sName, tPTR pData)
{
	if (m_pStruct && m_pUnsupportedFields && FindUnsupported(*m_pUnsupportedFields, m_pStruct->getIID(), field->m_name))
		return errOK;

	if( field->m_flags & SDT_FLAG_POINTER )
	{
		PR_TRACE((g_root, prtERROR, "CNV\tKLParams::SerializeIntegrated. serialization integrated types by pointer is not supported"));
		return errOBJECT_INCOMPATIBLE;
	}

	if( !sName )
		sName = field->m_name;

	if( !(sName && *sName) )
		return errOK;

	if( !pData )
		pData = SADDR(m_pStruct, field);

	cStrObj sTmp;
	if( (field->m_flags & SDT_FLAG_PASSWORD) && m_pController )
	{
		sTmp = *(cStrObj*)pData;
		if( m_pController->CryptDecrypt(sTmp, cTRUE, cTRUE) )
			pData = &sTmp;
	}

	cStrObj str;
	pData = MapField(m_pStruct, field, pData, str, true);

	return SetValue(sName, (tTYPE_ID)field->m_id, pData, field->m_size);
}

//////////////////////////////////////////////////////////////////////

tERROR KLParams::DeserializeStr(tUINT id)
{
	if( id == cSerializable::eIID )
	{
		if( m_pStruct )
			id = m_pStruct->getIID();
		if( id == cSerializable::eIID )
			GetValue(SER_UNIQUE_KEY_NAME, tid_DWORD, (tPTR)&id, sizeof(id));
		if( id == cSerializable::eIID )
			return errPARAMETER_INVALID;
	}
	if( !m_pStruct )
		g_root->CreateSerializable(id, &m_pStruct);
	if( !m_pStruct )
	{
		PR_TRACE((g_root, prtERROR, "CNV\tKLParams::KLParams::DeserializeStr. pointer is null, serid undefined"));
		return errPARAMETER_INVALID;
	}
	
	const cSerDescriptorField* field = GetFieldsByStructId(id);
	if( !field )
		return errNOT_FOUND;

	cERROR err;
	for(; PR_SUCC(err) && (field->m_flags != SDT_FLAG_ENDPOINT); field++)
		if (IsFieldSerializable(m_pStruct, field))
			err = DeserializeField(field);
	
	return err;
}

tERROR KLParams::DeserializeField(const cSerDescriptorField* field)
{
	bool  bBase    = !!(field->m_flags & SDT_FLAG_BASE);
	bool  bVector  = !!(field->m_flags & SDT_FLAG_VECTOR);
	bool  bPointer = !!(field->m_flags & SDT_FLAG_POINTER);
	bool  bStruct  = IS_SERIALIZABLE(field->m_id);
	tUINT nUnique  = field->m_id;
	tPTR  pData    = SADDR(m_pStruct, field);
	
	if( bVector )
	{
		if( field->m_id == cProfileEx::eIID )
			return DeserializeProfiles(field);
		return DeserializeVector(field);
	}
	else
	{
		if( !bStruct )
			return DeserializeIntegrated(field);
		
		if( bPointer )
		{
			cSerializable *&pStruct = *((cSerializable **)pData);
			
			KLParams parStruct = CreateSubValue(field->m_name, pStruct, bPointer, false, false);
			if( !parStruct )
				return errOK;

			tERROR err = parStruct.DeserializeStr(cSerializable::eIID);
			if( !pStruct )
				pStruct = parStruct.m_pStruct;
			return err;
		}

		KLParams parStruct = CreateSubValue(field->m_name, (cSerializable *)pData, bPointer, false, true);
		if( !parStruct )
			return errOK;
		
		return parStruct.DeserializeStr(nUnique);
	}
	return errOK;
}

tERROR KLParams::DeserializeVector(const cSerDescriptorField* field)
{
	if( !(field->m_name && *field->m_name) )
		return errOK;

	KLPAR::ArrayValue *pArray = (KLPAR::ArrayValue *)GetValue(field->m_name);
	if( !pArray )
	{
		if (m_pUnsupportedFields)
			AddUnsupported(*m_pUnsupportedFields, m_pStruct->getIID(), field->m_name);
		return errOK;
	}

	KLParamsAutoDeleteValue auto_delete;
	if( m_nFlags & CNV_REMOVE_DESERIALIZED )
		auto_delete.Init(this, field->m_name);

	if( (m_nFlags & CNV_DES_LOCKED_ONLY) && m_pStruct && m_pStruct->isBasedOn(cPolicySettings::eIID) )
		if( !((cPolicySettings *)m_pStruct)->IsMandatoried(field->m_name) )
			return errOK;

	bool  bBase    = !!(field->m_flags & SDT_FLAG_BASE);
	bool  bVector  = !!(field->m_flags & SDT_FLAG_VECTOR);
	bool  bPointer = !!(field->m_flags & SDT_FLAG_POINTER);
	bool  bStruct  = IS_SERIALIZABLE(field->m_id);
	tUINT nUnique  = field->m_id;
	tPTR  pData    = SADDR(m_pStruct, field);
	tMemChunk &v   = *(tMemChunk *)pData;

	if( field->m_flags & SDT_FLAG_VSTREAM && !bStruct )
	{
		if( pArray->GetType() != KLPAR::Value::BINARY_T )
		{
			PR_TRACE((g_root, prtERROR, "CNV\tKLParams::DeserializeField. stream vector must have a BINARY_T type"));
			return errOBJECT_INCOMPATIBLE;
		}

		KLPAR::BinaryValue *pBinary = (KLPAR::BinaryValue *)pArray;

		v.m_used = v.m_allocated = pBinary->GetSize();
		if(v.m_allocated)
		{
			g_root->heapRealloc((tPTR*)&v.m_ptr, v.m_ptr, v.m_allocated);
			memcpy(v.m_ptr, pBinary->GetValue(), v.m_allocated);
		}
		else
		{
			g_root->heapFree(v.m_ptr);
			v.m_ptr = NULL;
		}
		return errOK;
	}

	if( pArray->GetType() != KLPAR::Value::ARRAY_T )
	{
		PR_TRACE((g_root, prtERROR, "CNV\tKLParams::DeserializeField. vector must have an ARRAY_T type"));
		return errOBJECT_INCOMPATIBLE;
	}

	tUINT nVectorElementSize = field->m_flags & SDT_FLAG_POINTER ? sizeof(tPTR) : pr_sizeometer(nUnique, 0);
	tUINT nVectorElementsNum = pArray->GetSize();
	
	ReallocateVector(field, &v, nVectorElementsNum);

	if( !nVectorElementsNum || !v.m_ptr )
		return errOK;
	
	for(tUINT i = 0; i < nVectorElementsNum; i++)
	{
		pData = (tPTR)((tBYTE *)v.m_ptr + i * nVectorElementSize);


		KLParams parVectorElement(NULL, NULL, 0, m_pController, m_pUnsupportedFields);
		parVectorElement.Create();
		parVectorElement.SetValue(L"value", (KLPAR::Value *)pArray->GetAt(i));

		if( bPointer )
		{
			pData = *((tPTR *)pData);
			nUnique = cSerializable::eIID;
		}
		if( bStruct )
		{
			KLParams parArray = parVectorElement.CreateSubValue("value", (cSerializable *)pData, true, false, false);
			if( parArray )
				parArray.DeserializeStr(nUnique);
		}
		else
		{
			parVectorElement.DeserializeIntegrated(field, "value", pData);
		}
	}
	return errOK;
}

tERROR KLParams::DeserializeProfiles(const cSerDescriptorField* field)
{
	bool  bBase    = !!(field->m_flags & SDT_FLAG_BASE);
	bool  bVector  = !!(field->m_flags & SDT_FLAG_VECTOR);
	bool  bPointer = !!(field->m_flags & SDT_FLAG_POINTER);
	bool  bStruct  = IS_SERIALIZABLE(field->m_id);
	tUINT nUnique  = field->m_id;
	tPTR  pData    = SADDR(m_pStruct, field);

	
	tMemChunk &v = *(tMemChunk *)pData;
	
	KLParams parVector = CreateSubValue(field->m_name, m_pStruct, !(bBase && bVector), false, false);
	if( !parVector )
		return errOK;

	std::vector<std::string> names;
	if( field->m_id == cProfileEx::eIID )
	{
		cVector<cProfileEx *> &aProfiles = *(cVector<cProfileEx *> *)&v;
		for(tUINT i = 0; i < aProfiles.size(); i++)
			names.push_back((char *)aProfiles[i]->m_sName.c_str(cCP_ANSI));

		if( !names.size() )
			parVector.GetNames(names);
	}
	else
		parVector.GetNames(names);

	tUINT nVectorElementSize = field->m_flags & SDT_FLAG_POINTER ? sizeof(tPTR) : pr_sizeometer(nUnique, 0);
	tUINT nVectorElementsNum = names.size();
	if( !nVectorElementsNum )
		return errOK;
	
	bool bVectorOfPofiles = field->m_id == cProfileEx::eIID;
	if( bVectorOfPofiles )
	{
		if( !v.m_used )
			ReallocateVector(field, &v, nVectorElementsNum);
	}
	else
		ReallocateVector(field, &v, nVectorElementsNum);
	
	if( !v.m_ptr )
		return errOK;

	for(tUINT i = 0; i < nVectorElementsNum; i++)
	{
		pData = (tPTR)((tBYTE *)v.m_ptr + i * nVectorElementSize);
		if( bPointer )
		{
			pData = *((tPTR *)pData);
			nUnique = cSerializable::eIID;
		}
		if( bStruct )
		{
			KLParams parVectorElement = parVector.CreateSubValue(names[i].c_str(), (cSerializable *)pData, true, false, false);
			if( !parVectorElement )
				return errOK;

			if( (m_nFlags & CNV_DES_LOCKED_ONLY) && !bVectorOfPofiles )
				parVectorElement.m_nFlags &= ~CNV_DES_LOCKED_ONLY;

			if (m_pUnsupportedFields && ((cSerializable*)pData)->isBasedOn(cProfile::eIID))
				parVectorElement.SetAKUnsupportedFields(&((cProfile*)pData)->m_cfg.m_aAKUnsupportedFields);

			parVectorElement.DeserializeStr(nUnique);

			SetKeyName(pData, names[i]);
		}
		else
		{
			parVector.DeserializeIntegrated(field, names[i].c_str(), pData);
		}
	}
	return errOK;
}

tERROR KLParams::DeserializeIntegrated(const cSerDescriptorField* field, const char *sName, tPTR pData)
{
	if( field->m_flags & SDT_FLAG_POINTER )
	{
		PR_TRACE((g_root, prtERROR, "CNV\tKLParams::SerializeIntegrated. serialization integrated types by pointer is not supported"));
		return errOBJECT_INCOMPATIBLE;
	}

	if( !sName )
		sName = field->m_name;

	if( !(sName && *sName) )
		return errOK;

	if( !pData )
		pData = SADDR(m_pStruct, field);

	tERROR err = GetValue(sName, (tTYPE_ID)field->m_id, pData, field->m_size);
	if (PR_FAIL(err) && m_pUnsupportedFields)
		AddUnsupported(*m_pUnsupportedFields, m_pStruct->getIID(), sName);

	if( (field->m_flags & SDT_FLAG_PASSWORD) && m_pController )
		m_pController->CryptDecrypt(*(cStrObj*)pData, cFALSE, cTRUE);

	MapField(m_pStruct, field, pData, *(cStrObj *)NULL, false);

	if( m_nFlags & CNV_REMOVE_DESERIALIZED )
		DeleteValue(sName);

	return err == errNOT_FOUND ? errOK : err;
}

//////////////////////////////////////////////////////////////////////

tPTR KLParams::GetKeyNamePtr(tPTR pStruct, tUINT nUnique)
{
	if( IS_SERIALIZABLE(nUnique) && nUnique != cSerializable::eIID )
	{
		for(const cSerDescriptorField* field = GetFieldsByStructId(nUnique); field->m_flags != SDT_FLAG_ENDPOINT; field++)
		{
			if( field->m_flags & SDT_FLAG_KEY_NAME && field->m_id == tid_STRING_OBJ )
			{
				return SADDR(pStruct, field);
			}
			if( IS_SERIALIZABLE(field->m_id) && (field->m_flags & SDT_FLAG_BASE) && !(field->m_flags & (SDT_FLAG_VECTOR|SDT_FLAG_POINTER)) )
			{
				return GetKeyNamePtr(SADDR(pStruct, field), field->m_id);
			}
		}
	}
	return NULL;
}

std::string KLParams::GetKeyName(tPTR pStruct, tUINT n)
{
	if( pStruct )
	{
		tPTR ptr = GetKeyNamePtr(pStruct, ((cSerializable *)pStruct)->getIID());
		if( ptr )
			return (const char *)((cStringObj *)ptr)->c_str(cCP_ANSI);
	}
	
	char name[10];
	sprintf(name, "%04d", n);
	return name;
}

void KLParams::SetKeyName(tPTR pStruct, std::string &sKeyName)
{
	if( tPTR ptr = GetKeyNamePtr(pStruct, ((cSerializable *)pStruct)->getIID()) )
		*((cStringObj *)ptr) = sKeyName.c_str();
}

KLPAR::Value::Types KLParams::PrType2KcaType(tTYPE_ID nType) const
{
	for(int i = 0; i < countof(g_typestable); i++)
		if(nType == g_typestable[i].m_nPrType)
			return g_typestable[i].m_nKcaType;
	
	PR_TRACE((g_root, prtERROR, "CNV\tKLParams::PrType2KcaType. unsupported data type: 0x%02x", nType));
	return KLPAR::Value::EMPTY_T;
}

void KLParams::ReallocateVector(const cSerDescriptorField* field, tPTR pData, tUINT nEl)
{
	tMemChunk &v = *(tMemChunk *)pData;

	bool bPointer = !!(field->m_flags & SDT_FLAG_POINTER);
	
	tUINT nVectorElementSize = bPointer ? sizeof(tPTR) : pr_sizeometer(field->m_id, 0);

	// cleanup
	const cSerDescriptor* dsc = GetDescrByStructId(field->m_id);
	if(dsc && dsc->m_destructor)
	{
		for(tUINT i = 0; i < v.m_used; i += nVectorElementSize)
		{
			tPTR pEl = (tBYTE*)v.m_ptr + i;
			if(bPointer)
			{
				g_root->DestroySerializable((cSerializable *)*(tPTR *)pEl);
			}
			else
			{
				dsc->m_destructor((cSerializable *)pEl);
			}
		}
	}

	// reallocate
	v.m_used = v.m_allocated = nEl * nVectorElementSize;
	if(v.m_allocated)
	{
		g_root->heapRealloc((tPTR*)&v.m_ptr, v.m_ptr, v.m_allocated);
		memset(v.m_ptr, 0, v.m_allocated);
	}
	else
	{
		g_root->heapFree(v.m_ptr);
		v.m_ptr = NULL;
	}
	
	// construct
	if(dsc && dsc->m_constructor)
	{
		for(tUINT i = 0; i < v.m_used; i += nVectorElementSize)
		{
			tPTR pEl = (tBYTE*)v.m_ptr + i;
			if(bPointer)
			{
				g_root->CreateSerializable(field->m_id, (cSerializable **)(tPTR *)pEl);
			}
			else
			{
				dsc->m_constructor((cSerializable*)pEl, NULL);
			}
		}
	}
}

void KLParams::GetNames(std::vector<std::string> &names)
{
	KLPAR::names_t wnames; m_pParams->GetNames(wnames);
	
	KLSTD_USES_CONVERSION;
	for(tUINT i = 0; i < wnames.m_nwstr; i++)
		names.push_back(KLSTD_W2CA(wnames.m_pwstr[i]));

	KLSTD_FreeArrayWSTR(wnames);
}

// по идее, признак в каких случаях нужно серелизовать полянки должен быть в описателе структруры.
// иторически сложилось так, что эта инфа захардкожена в фкнкции KLParams::IsFieldSerializable
bool KLParams::IsFieldSerializable(cSerializable *pSer, const cSerDescriptorField* field)
{
	if( field->m_flags & SDT_FLAG_BASE )
		return true;
	
	if( pSer->isBasedOn(cPolicySettings::eIID) )
	{
		if( field->m_name && !strcmp(field->m_name, "LockedFields") )
			return false;
		if( field->m_name && !strcmp(field->m_name, "MandatoriedFields") )
			return false;
	}

	if( pSer->isBasedOn(cProfileBase::eIID) )
	{
		if( field->m_name && !strcmp(field->m_name, "profiles") )
			return true;
		if( field->m_name && !strcmp(field->m_name, "type") )
			return true;
		if( field->m_name && !strcmp(field->m_name, "name") )
			return true;
		if( field->m_name && !strcmp(field->m_name, "statistics") )
			return !!(m_nFlags & CNV_SER_STATISTICS);
		if( field->m_name && !strcmp(field->m_name, "description") )
			return !!(m_nFlags & CNV_SER_STATISTICS);
		if( field->m_id == cCfg::eIID )
			return true;
		return false;
	}

	if( pSer->isBasedOn(cCfg::eIID) )
	{
		if( field->m_name && !strcmp(field->m_name, "enabled") )
			return true;
		if( field->m_name && !strcmp(field->m_name, "level") )
			return true;
		if( field->m_name && !strcmp(field->m_name, "settings") )
			return true;
		if( field->m_name && !strcmp(field->m_name, "smode") )
			return true;
		return false;
	}

	if( pSer->isBasedOn(cFwAppRule::eIID) )
	{
		if( field->m_name && !strcmp(field->m_name, "Hash") )
			return false;
		return true;
	}

	if( pSer->isBasedOn(cPdmAppMonitoring_Data::eIID) )
	{
		if( field->m_name && !strcmp(field->m_name, "Hash") )
			return false;
		if( field->m_name && !strcmp(field->m_name, "ObjectSize") )
			return false;
		if( field->m_name && !strcmp(field->m_name, "tCreation") )
			return false;
		if( field->m_name && !strcmp(field->m_name, "tModify") )
			return false;
		if( field->m_name && !strcmp(field->m_name, "Version") )
			return false;
		if( field->m_name && !strcmp(field->m_name, "Vendor") )
			return false;
		if( field->m_name && !strcmp(field->m_name, "Description") )
			return false;
		if( field->m_name && !strcmp(field->m_name, "MSCheck") )
			return false;
		return true;
	}

	if( pSer->isBasedOn(cODSSettings::eIID) )
	{
		if( field->m_name && !strcmp(field->m_name, "ScanObjects") )
			return !(m_nFlags & CNV_USE_POLICY);
		return true;
	}

	if( pSer->isBasedOn(cBLSettings::eIID) )
	{
		if( field->m_name && !strcmp(field->m_name, "AffinityMask") )
			return !(m_nFlags & CNV_USE_POLICY);
		if( field->m_name && !strcmp(field->m_name, "LicBuyLink") )
			return false;
		if( field->m_name && !strcmp(field->m_name, "LicRenewLink") )
			return false;
		if( field->m_name && !strcmp(field->m_name, "LicActivationLink") )
			return false;
		return true;
	}

	if( pSer->isBasedOn(cAntiPhishingSettings::eIID) )
		return !(m_nFlags & CNV_USE_POLICY);

	if( pSer->isBasedOn(cTaskSettingsPerUser::eIID) )
	{
		if( field->m_name && !strcmp(field->m_name, "users") )
			return false;
		return true;
	}

	return true;
}

tPTR KLParams::MapField(cSerializable *pSer, const cSerDescriptorField* field, tPTR pData, cStrObj &str, bool bSerialize)
{
	if( !pSer || !field || !field->m_name || !*field->m_name )
		return pData;

	if( pSer->isBasedOn(cProfileBase::eIID) )
	{
		// Эта конвертация сделана по ошибке, на самом деле надо конвертировать
		// поле "TASK_NAME", которое от сюда скорее всего вообще не видно.
		// Но поле TASK_NAME уже поздняк менять, т.к. вышел релиз, где в нём
		// передаётся "updater". Если теперь поменять на нужный "KLUPD_TA_UPDATE_TASK"
		// то возникнут проблемы с обратной совместимостью (например, при создании в новом
		// плагине задачи обновления с типом KLUPD_TA_UPDATE_TASK, старый WKS её не поймёт
		// или другой пример см. багу 18345, там как раз плагин создал задачу KLUPD_TA_UPDATE_TASK).
		// Что касается этой ошибочной конвертации, то её убирать нельзя, т.к. при создани,
		// например, политики здесь "updater" поменялся на "KLUPD_TA_UPDATE_TASK"
		// и если конвертацию убрать, то политика будет применяться к профилю KLUPD_TA_UPDATE_TASK
		// которого у нас нет.
		// см. баги 15011, 18242, 18345 
		if( !strcmp(field->m_name, "type") && field->m_id == tid_STRING_OBJ )
		{
			cStringObj &strType = *(cStringObj *)pData;
			if( bSerialize && strType == AVP_TASK_UPDATER )
			{
				str = KLPRTS::PRTS_PRODUCT_UPDATER_TASK_TYPE_NAME;
				return &str;
			}
			else if( !bSerialize && strType == KLPRTS::PRTS_PRODUCT_UPDATER_TASK_TYPE_NAME )
			{
				strType = AVP_TASK_UPDATER;
				return pData;
			}
		}
	}

	return pData;
}

//////////////////////////////////////////////////////////////////////

const cSerDescriptorField *KLParams::GetFieldsByStructId(tDWORD unique)
{
	const cSerDescriptor *pDesc = GetDescrByStructId(unique);
	if(pDesc)
		return pDesc->m_fields;
	return NULL;
}

const cSerDescriptor *KLParams::GetDescrByStructId(tDWORD unique)
{
	cSerDescriptor *pDesc;
	if(g_root->FindSerializableDescriptor(&pDesc, unique) == errOK)
		return pDesc;
	return NULL;
}

PParams KLParams::LookupPath(const wchar_t *path, PParams pParams, bool bCreate)
{
	KLERR_TRY
		PValue pParamsValue;
		if( pParams->GetValueNoThrow(path, &pParamsValue) )
		{
			if( pParamsValue->GetType() != KLPAR::Value::PARAMS_T )
				return NULL;
		}
		else
		{
			if( !bCreate )
				return NULL;
			
			PParams pNewParams; KLPAR_CreateParams(&pNewParams);
			KLPAR::CreateValue(pNewParams, (KLPAR::ParamsValue **)&pParamsValue);
			
			pParams->AddValue(path, pParamsValue);
			if( !wcscmp(path, KLPRSS::c_szwSPS_Value) )
				REPLACE_PARAMS_VALUE(pParams, KLPRSS::c_szwSPS_Mandatory, BoolValue, 0);
		}
		return ((KLPAR::ParamsValue *)pParamsValue.operator->())->GetValue();
	KLERR_CATCH(pError)
	KLERR_ENDTRY;
	
	return NULL;
}

PParams KLParams::LookupPath(wstrings &path, PParams pParams, bool bCreate)
{
	KLERR_TRY
		PParams pCurParams = pParams;
		for(size_t i = 0; i < path.size(); i++)
		{
			const wchar_t *strPath = path[i].c_str();

			PValue pParamsValue;
			if( !pCurParams->GetValueNoThrow(strPath, &pParamsValue) )
			{
				if( !bCreate )
					return NULL;
				
				PParams pNewParams;
				KLPAR_CreateParams(&pNewParams);
				KLPAR::CreateValue(pNewParams, (KLPAR::ParamsValue **)&pParamsValue);
				pCurParams->AddValue(strPath, pParamsValue);
				if( !wcscmp(strPath, KLPRSS::c_szwSPS_Value) )
					REPLACE_PARAMS_VALUE(pCurParams, KLPRSS::c_szwSPS_Mandatory, BoolValue, 0);					
			}
			if( pParamsValue->GetType() != KLPAR::Value::PARAMS_T )
				return NULL;

			pCurParams = ((KLPAR::ParamsValue *)pParamsValue.operator->())->GetValue();
//			pCurParams = pParamsValue->GetValue();
		}
		return pCurParams;
	KLERR_CATCH(pError)
	KLERR_ENDTRY;
	
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

#define PARLOG_PREFIX_PARAM			L"+---"
#define PARLOG_PREFIX_INDENT		L"|   "
#define PARLOG_PREFIX_INDENT_LAST	L"    "

static std::wstring _LogValue(std::wstring prefix, std::wstring name, KLPAR::Value *pVal, bool bIsLastChild)
{
	std::wstring out;
	if(prefix.empty())
		out += L"\n";
	out += prefix + PARLOG_PREFIX_PARAM + name;

	KLSTD_USES_CONVERSION;

	wchar_t tmp_buf[64];

	switch(pVal->GetType())
	{
	case KLPAR::Value::STRING_T:
		{
			out += L" = ";
			std::wstring strval = ((KLPAR::StringValue *)pVal)->GetValue();
			if(strval.empty())
				out += L"<empty string>";
			else
			{
				out += L"(string)";
				out += strval;
			}
			out += L"\n";
		}
		break;
	case KLPAR::Value::BOOL_T:
		out += L" = ";
		out += ((KLPAR::BoolValue *)pVal)->GetValue() ? L"true" : L"false";
		out += L"\n";
		break;
	case KLPAR::Value::INT_T:
		out += L" = (long)";
		out += _itow(((KLPAR::IntValue *)pVal)->GetValue(), tmp_buf, 10);
		out += L"\n";
		break;
	case KLPAR::Value::LONG_T:
		out += L" = (__int64)";
		out += _i64tow(((KLPAR::LongValue *)pVal)->GetValue(), tmp_buf, 10);
		out += L"\n";
		break;
	case KLPAR::Value::DATE_TIME_T:
		{
			time_t tmval = ((KLPAR::DateTimeValue *)pVal)->GetValue();
			wchar_t tm_buffer[256] = L"<unknown>";
			tm *tmtm = localtime(&tmval);
			if(tmtm)
				wcsftime(tm_buffer, 256, L"%x %X", tmtm);
			out += L" = ";
			out += tm_buffer;
			out += L"\n";
		}
		break;
	case KLPAR::Value::DATE_T:
		out += L" = (date)";
		out += KLSTD_A2W(((KLPAR::DateValue *)pVal)->GetValue());
		out += L"\n";
		break;
	case KLPAR::Value::BINARY_T:
		out += L" = ";
		out += std::wstring(L"BLOB (size = ") + _itow(((KLPAR::BinaryValue *)pVal)->GetSize(), tmp_buf, 10) + L")";
		out += L"\n";
		break;
	case KLPAR::Value::FLOAT_T:
		swprintf(tmp_buf, L"%f", ((KLPAR::FloatValue *)pVal)->GetValue());
		out += L" = (float)";
		out += tmp_buf;
		out += L"\n";
		break;
	case KLPAR::Value::DOUBLE_T:
		swprintf(tmp_buf, L"%f", ((KLPAR::DoubleValue *)pVal)->GetValue());
		out += L" = (double)";
		out += tmp_buf;
		out += L"\n";
		break;
	case KLPAR::Value::PARAMS_T:
		{
			KLPAR::Params *v = ((KLPAR::ParamsValue *)pVal)->GetValue();
			KLSTD::AKWSTRARR n;
			v->GetNames(n);
			out += L" (Params)\n";
			for(size_t i = 0, size = n.m_nwstr; i < size; ++i)
			{
				KLSTD::CAutoPtr<KLPAR::Value> p;
				v->GetValue(n.m_pwstr[i], &p);
				out += _LogValue(bIsLastChild ?
					(prefix + PARLOG_PREFIX_INDENT_LAST) :
				(prefix + PARLOG_PREFIX_INDENT), n.m_pwstr[i], p, i == size - 1);
			}
			KLSTD_FreeArrayWSTR(n);
		}
		break;
	case KLPAR::Value::ARRAY_T:
		{
			KLPAR::ArrayValue *v = (KLPAR::ArrayValue *)pVal;

			out += L" (Array)\n";
			for(size_t i = 0, size = v->GetSize(); i < size; ++i)
			{
				KLSTD::CAutoPtr<KLPAR::Value> p;
				v->GetAt(i, &p);
				wchar_t iw[20];
				out += _LogValue(bIsLastChild ?
					(prefix + PARLOG_PREFIX_INDENT_LAST) :
				(prefix + PARLOG_PREFIX_INDENT), _itow(i, iw, 10), p, i == size - 1);
			}
		}
		break;
	}
	return out;
}

//////////////////////////////////////////////////////////////////////////

void KLParams::Trace(const wchar_t *strFilePrefix, const wchar_t *strOp, const KLPAR::Params *pParams)
{
	SYSTEMTIME st; GetLocalTime(&st);

	wchar_t strFile[1024];
	swprintf(strFile, L"c:\\.log\\.tasks\\%02d.%02d.%02d.%03d.%s.%s.klt", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, strFilePrefix, strOp);

	FILE *hFile = _wfopen(strFile, L"wb");
	if( hFile )
	{
		KLSTD::CAutoPtr<KLPAR::ParamsValue> pVal;
		KLPAR::CreateValue((KLPAR::Params *)pParams, &pVal);
		std::wstring wsLog = _LogValue(L"", L"", pVal, true);
		fputws(wsLog.c_str(), hFile);

		KLERR_TRY
//			KLPAR_SerializeToFileID(hFile->_file, pParams);
			PR_TRACE((g_root, prtNOTIFY, "CNV\tKLParams::Trace. Parameters saved to file %S", strFile));
		KLERR_CATCH( pError )
		KLERR_ENDTRY
		
		fclose(hFile);
	}
}

//////////////////////////////////////////////////////////////////////
