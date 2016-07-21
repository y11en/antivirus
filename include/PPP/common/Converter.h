// Converter.h: interface for the Converter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONVERTER_H__C0485809_622A_4AE2_96D1_694B8B90F057__INCLUDED_)
#define AFX_CONVERTER_H__C0485809_622A_4AE2_96D1_694B8B90F057__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786)

#include <windows.h>
#include <tchar.h>

// Praga's includes
#include <Prague/prague.h>
#include <Prague/pr_serdescriptor.h>
#include <Prague/pr_serializable.h>

#include <ProductCore/structs/s_taskmanager.h>

// KCA's includes
#include <std/par/Params.h>
#include <string>
#include <vector>

//////////////////////////////////////////////////////////////////////

typedef std::wstring wstring;
typedef std::vector<std::wstring> wstrings;
typedef KLSTD::CAutoPtr<KLPAR::Params> PParams;
typedef KLSTD::CAutoPtr<KLPAR::Value> PValue;

//typedef KLPAR::Params Params;
//typedef KLPAR::Value Value;

typedef KLSTD::CAutoPtr<KLPAR::Params> ParamsPtr;
typedef KLSTD::CAutoPtr<KLPAR::Value> ValuePtr;

//////////////////////////////////////////////////////////////////////

#define CNV_USE_POLICY             0x01
#define CNV_DES_LOCKED_ONLY        0x02
#define CNV_SER_UNLOCKED_ONLY      0x04
#define CNV_SER_CHILD_PROFILES     0x08
#define CNV_SER_STATISTICS         0x10
#define CNV_REMOVE_DESERIALIZED    0x20
#define CNV_COLLECT_UNSUPPORTED    0x40


class KlPar
{
public:
	KlPar(bool bCreate = false) : m_pParams(NULL) { if(bCreate) m_pParams = KlPar::Create(); }
	KlPar(KLPAR::Params *pParams) : m_pParams(pParams) {}

	enum eFlags
	{
		fNone             = 0x0,
		fMustExist        = 0x1,
		fCreateIfNotExist = 0x2,
		fUsePolicyStruct  = 0x4,
	};

public:
	operator ParamsPtr() const          { return m_pParams; }
	operator KLPAR::Params *() const    { return m_pParams; }
	operator bool() const               { return !operator!(); }
	bool operator!() const              { return !m_pParams; }
	ParamsPtr &      ref()              { return m_pParams; }

public:
	tERROR           SetValue(const wchar_t *strName, KLPAR::Value *pValue, bool bReplace = true);
	tERROR           SetValue(const wchar_t *strName, const wchar_t *strValue);
//	tERROR           SetValue(const wchar_t *strName, bool bValue);
//	tERROR           SetValue(const wchar_t *strName, int nValue);
//	tERROR           SetValue(const wchar_t *strName, __int64 nValue);
//	tERROR           SetValue(const wchar_t *strName, time_t tValue);
//	tERROR           SetValue(const wchar_t *strName, void *pValue, size_t nSize);
//	tERROR           SetValue(const wchar_t *strName, float nValue);
//	tERROR           SetValue(const wchar_t *strName, double nValue);
//	tERROR           SetValue(const wchar_t *strName, KLPAR::Params *pParams);

	KLPAR::Value *   GetValue(const wchar_t *strName, KLPAR::Value::Types tValueType = KLPAR::Value::EMPTY_T);
	tERROR           GetValue(const wchar_t *strName, KLPAR::Value **ppValue, KLPAR::Value::Types tValueType = KLPAR::Value::EMPTY_T);
	tERROR           GetValue(const wchar_t *strName, wstring &strValue);
//	tERROR           GetValue(const wchar_t *strName, bool &bValue);
//	tERROR           GetValue(const wchar_t *strName, int &nValue);
//	tERROR           GetValue(const wchar_t *strName, __int64 &nValue);
//	tERROR           GetValue(const wchar_t *strName, time_t &tValue);
//	tERROR           GetValue(const wchar_t *strName, void *pValue, size_t &nSize);
//	tERROR           GetValue(const wchar_t *strName, float &nValue);
//	tERROR           GetValue(const wchar_t *strName, double &nValue);
	tERROR           GetValue(const wchar_t *strName, KLPAR::Params **ppParams);
	
public:
	static tERROR    GetArrayValue(KLPAR::ArrayValue *pArray, size_t nIdx, KLPAR::Value **ppValue, KLPAR::Value::Types tValueType = KLPAR::Value::EMPTY_T);
	static tERROR    GetArrayValue(KLPAR::ArrayValue *pArray, size_t nIdx, KLPAR::Params **ppParams);

public:
	KLPAR::Params *  Lookup(eFlags nFlags, const wchar_t *strParams);
	KLPAR::Params *  Lookup(eFlags nFlags, size_t nArgs, ...);
	KLPAR::Params *  Lookup(eFlags nFlags, wstrings &path);
	
public:
	static ParamsPtr Create();
	KLPAR::Params *  Create(const wchar_t *strName, eFlags nFlags = fCreateIfNotExist);

public:
	void             Trace(const wchar_t *strFilePrefix, const wchar_t *strOp, const KLPAR::Params *pParams);
	
protected:
	ParamsPtr m_pParams;
};

class KLParamsController
{
public:
	virtual bool CryptDecrypt(cStringObj &str, tBOOL bCrypt, tBOOL bRecrypt){ return false; }
};

// класс KLParams - враппер над KLPAR:Params контейнером
class KLParams
{
public:
	KLParams(KLParamsController* ctl, KLPAR::Params* pParams, cSerializable* pStuct = NULL, DWORD nFlags = 0, cAKUnsupportedFields* pUF = NULL) :
		m_pParams(pParams), m_pStruct(pStuct), m_nFlags(nFlags), m_pController(ctl), m_pUnsupportedFields(pUF), m_bMandatoried(false) {}

	KLParams(KLPAR::Params* pParams = NULL, cSerializable* pStuct = NULL, DWORD nFlags = 0, KLParamsController* ctl = NULL, cAKUnsupportedFields* pUF = NULL) :
		m_pParams(pParams), m_pStruct(pStuct), m_nFlags(nFlags), m_pController(ctl), m_pUnsupportedFields(pUF), m_bMandatoried(false) {}

	KLParams(KLPAR::Params* pParams, cSerializable* pStuct, KLParams& pClone) :
		m_pParams(pParams), m_pStruct(pStuct), m_nFlags(pClone.m_nFlags), m_pController(pClone.m_pController), m_pUnsupportedFields(pClone.m_pUnsupportedFields), m_bMandatoried(pClone.m_bMandatoried) {}

	operator PParams()  const { return m_pParams; }
	operator bool() const     { return !operator!(); }
	bool operator!() const    { return !m_pParams; }

	bool            IsEmpty() { return operator!(); }
	void            Create();

	tERROR			Serialize(cSerializable* pStruct, tDWORD unique = SERID_UNKNOWN);
	tERROR			Deserialize(cSerializable*& pStruct, tDWORD unique = SERID_UNKNOWN);

	// открывает либо создает вложенный контейнер Params,
	// возвращает указатель на него.
	// в случае, если имя name не определено возвращает указатель на собственный контейнер.
	KLParams        CreateSubValue(const char *sName, cSerializable* pStruct, bool bNameMustExist, bool bCreate, bool bMustExist = false);

	// методы SetValue создают новое значение, либо замещают старое
	tERROR			SetValue(const char *sName, const char *Value);
	tERROR			SetValue(const char *sName, const wchar_t *Value);
	tERROR			SetValue(const char *sName, DWORD Value);
	tERROR			SetValue(const char *sName, const void *Value, DWORD nSize);
	tERROR			SetValue(const char *sName, tTYPE_ID nType, tPTR pValue, tDWORD nCount);
	tERROR			SetValue(const char *sName, KLPAR::Value *pValue);
	tERROR			SetValue(const wchar_t *sName, KLPAR::Value *pValue);

	tERROR			GetValue(const char *sName, wstring &str) const;
	tERROR			GetValue(const char *sName, tTYPE_ID nType, tPTR pValue, tDWORD nCount) const;
	KLPAR::Value *	GetValue(const char *sName) const;
	KLPAR::Value *	GetValue(const wchar_t *sName) const;
	tERROR          GetValueSize(const char *sName, tDWORD &nSize) const;

	tERROR			DeleteValue(const char *sName);
	bool            RemoveEmptyContainers();

	tERROR          SerializeStr(tUINT id, bool bPutId);
	tERROR          SerializeField(const cSerDescriptorField* field);
	tERROR          SerializeVector(const cSerDescriptorField* field);
	tERROR          SerializeProfiles(const cSerDescriptorField* field);
	tERROR          SerializeIntegrated(const cSerDescriptorField* field, const char *sName = NULL, tPTR pData = NULL);
	
	tERROR          DeserializeStr(tUINT id);
	tERROR          DeserializeField(const cSerDescriptorField* field);
	tERROR          DeserializeVector(const cSerDescriptorField* field);
	tERROR          DeserializeProfiles(const cSerDescriptorField* field);
	tERROR          DeserializeIntegrated(const cSerDescriptorField* field, const char *sName = NULL, tPTR pData = NULL);

	void			SetAKUnsupportedFields(cAKUnsupportedFields* p) { m_pUnsupportedFields = p; }

protected:
	void            ReallocateVector(const cSerDescriptorField* field, tPTR pData, tUINT nEl);
	void            GetNames(std::vector<std::string> &names);
	bool            IsFieldSerializable(cSerializable *pSer, const cSerDescriptorField* field);
	tPTR            MapField(cSerializable *pSer, const cSerDescriptorField* field, tPTR pData, cStrObj &str, bool bSerialize);
	KLPAR::Value::Types PrType2KcaType(tTYPE_ID nType) const;

	static tPTR     GetKeyNamePtr(tPTR pStruct, tUINT nUnique);
	static std::string GetKeyName(tPTR pStruct, tUINT n);
	static void     SetKeyName(tPTR pStruct, std::string &sKeyName);

public:
	static const cSerDescriptorField *GetFieldsByStructId(tDWORD unique);
	static const cSerDescriptor *GetDescrByStructId(tDWORD unique);
	static PParams   LookupPath(const wchar_t *path, PParams pParams, bool bCreate = false);
	static PParams   LookupPath(wstrings &path, PParams pParams, bool bCreate = false);
	static void      Trace(const wchar_t *strFilePrefix, const wchar_t *strOp, const KLPAR::Params *pParams);

protected:
	PParams			m_pParams;
	DWORD           m_nFlags;
	bool            m_bMandatoried;
	cSerializable * m_pStruct;
	KLParamsController* m_pController;
	cAKUnsupportedFields* m_pUnsupportedFields;
};

//////////////////////////////////////////////////////////////////////

inline KLPAR::Value *GetParamsValue(const KLPAR::Params *pParams, const wchar_t *strName, KLPAR::Value::Types eType)
{
	if( !pParams )
		return NULL;
	
	KLPAR::Value *pValue = NULL;
	if( const_cast<KLPAR::Params *>(pParams)->GetValueNoThrow(strName, &pValue) && pValue && pValue->GetType() == eType )
		return pValue;
	
	return NULL;
}

inline KLPAR::Value *GetParamsArrayValue(KLPAR::ArrayValue *pArray, size_t nIndex, KLPAR::Value::Types eType)
{
	if( !pArray )
		return NULL;
	
	KLPAR::Value *pValue = const_cast<KLPAR::Value *>(pArray->GetAt(nIndex));
	if( pValue && pValue->GetType() == eType )
		return pValue;
	
	return NULL;
}

#define GET_PARAMS_VALUE_EX(params, name, type, typeType, Var) { \
	if( KLPAR::type *pVal = (KLPAR::type*)GetParamsValue(params, name, KLPAR::Value::typeType) ) \
		Var = pVal->GetValue(); }

#define GET_PARAMS_ARRAY_VALUE_EX(array, index, type, typeType, Var) { \
	if( KLPAR::type *pVal = (KLPAR::type*)GetParamsArrayValue(array, index, KLPAR::Value::typeType) ) \
		Var = pVal->GetValue(); }

//////////////////////////////////////////////////////////////////////

#define TraceParams KLParams::Trace

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_CONVERTER_H__C0485809_622A_4AE2_96D1_694B8B90F057__INCLUDED_)
