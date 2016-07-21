//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#include "stdafx.h"
#include "RegMetadata.h"
#include "TreeImages.h"

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

enum ETypeEditMode
{
	eTypeEditDecimal,
	eTypeEditString,
	eTypeEditBinary,
	eTypeEditUnsupported
};

struct STypeMetadata
{
	tTYPE_ID		m_uiType;
	ETypeEditMode	m_eEditMode;
	UINT			m_uiTreeImage;
	LPCTSTR			m_strTitle;
};

static STypeMetadata s_arrTypeMetadata[] = 
{
	{ tid_VOID,		eTypeEditBinary,		TREE_DATA_OTHER,	_T("VOID")},
	{ tid_BYTE,		eTypeEditDecimal,		TREE_DATA_OTHER,	_T("BYTE")},
	{ tid_WORD,		eTypeEditDecimal,		TREE_DATA_OTHER,	_T("WORD")},
	{ tid_DWORD,	eTypeEditDecimal,		TREE_DATA_OTHER,	_T("DWORD")},
	{ tid_QWORD,	eTypeEditUnsupported,	TREE_DATA_OTHER,	_T("QWORD")},
	{ tid_BOOL,		eTypeEditDecimal,		TREE_DATA_OTHER,	_T("BOOL")},
	{ tid_CHAR,		eTypeEditString,		TREE_DATA_STR,		_T("CHAR")},
	{ tid_WCHAR,	eTypeEditString,		TREE_DATA_STR,		_T("WCHAR")},
	{ tid_STRING,	eTypeEditString,		TREE_DATA_STR,		_T("STRING")},
	{ tid_WSTRING,	eTypeEditString,		TREE_DATA_STR,		_T("WSTRING")},
	{ tid_ERROR,	eTypeEditDecimal,		TREE_DATA_OTHER,	_T("ERROR")},
	{ tid_PTR,		eTypeEditUnsupported,	TREE_DATA_OTHER,	_T("PTR")},
	{ tid_INT,		eTypeEditDecimal,		TREE_DATA_OTHER,	_T("INT")},
	{ tid_UINT,		eTypeEditDecimal,		TREE_DATA_OTHER,	_T("UINT")}, 
	{ tid_SBYTE,	eTypeEditDecimal,		TREE_DATA_OTHER,	_T("SBYTE")},
	{ tid_SHORT,	eTypeEditDecimal,		TREE_DATA_OTHER,	_T("SHORT")},
	{ tid_LONG,		eTypeEditDecimal,		TREE_DATA_OTHER,	_T("LONG")},
	{ tid_LONGLONG, eTypeEditDecimal,		TREE_DATA_OTHER,	_T("LONGLONG")},
	{ tid_IID,		eTypeEditUnsupported,	TREE_DATA_OTHER,	_T("IID")},  
	{ tid_PID,		eTypeEditUnsupported,	TREE_DATA_OTHER,	_T("PID")},   
	{ tid_ORIG_ID,	eTypeEditUnsupported,	TREE_DATA_OTHER,	_T("ORIG_ID")}, 
	{ tid_OS_ID,	eTypeEditUnsupported,	TREE_DATA_OTHER,	_T("OS_ID")},   
	{ tid_VID,		eTypeEditUnsupported,	TREE_DATA_OTHER,	_T("VID")},     
	{ tid_PROPID,	eTypeEditUnsupported,	TREE_DATA_OTHER,	_T("PROPID")},  
	{ tid_VERSION,	eTypeEditUnsupported,	TREE_DATA_OTHER,	_T("VERSION")}, 
	{ tid_CODEPAGE, eTypeEditUnsupported,	TREE_DATA_OTHER,	_T("CODEPAGE")},
	{ tid_LCID,		eTypeEditUnsupported,	TREE_DATA_OTHER,	_T("LCID")},    
	{ tid_DATA,		eTypeEditUnsupported,	TREE_DATA_OTHER,	_T("DATA")},    
	{ tid_DATETIME, eTypeEditUnsupported,	TREE_DATA_OTHER,	_T("DATETIME")},
	{ tid_FUNC_PTR, eTypeEditUnsupported,	TREE_DATA_OTHER,	_T("FUNC_PTR")},
	{ tid_BINARY,	eTypeEditBinary,		TREE_DATA_OTHER,	_T("BINARY")},
	{ tid_IFACEPTR, eTypeEditUnsupported,	TREE_DATA_OTHER,	_T("IFACEPTR")},
	{ tid_OBJECT,	eTypeEditUnsupported,	TREE_DATA_OTHER,	_T("OBJECT")},
	{ tid_EXPORT,	eTypeEditUnsupported,	TREE_DATA_OTHER,	_T("EXPORT")},
	{ tid_IMPORT,	eTypeEditUnsupported,	TREE_DATA_OTHER,	_T("IMPORT")},
	{ tid_TRACE_LEVEL, eTypeEditUnsupported,TREE_DATA_OTHER,	_T("TRACE_LEVEL")},
	{ tid_TYPE_ID,	eTypeEditUnsupported,	TREE_DATA_OTHER,	_T("TYPE_ID")},
	{ tid_VECTOR,	eTypeEditUnsupported,	TREE_DATA_OTHER,	_T("VECTOR")},
	{ tid_STRING_OBJ, eTypeEditUnsupported,	TREE_DATA_OTHER,	_T("STRING_OBJ")},
	{ tid_SERIALIZABLE, eTypeEditUnsupported,TREE_DATA_OTHER,	_T("SERIALIZABLE")}
};

inline STypeMetadata* FindMetadata(tTYPE_ID i_uiType)
{
	int nMetadataTbaleLen = sizeof(s_arrTypeMetadata) / sizeof(s_arrTypeMetadata[0]);
	for(int i=0; i<nMetadataTbaleLen; i++)
		if(s_arrTypeMetadata[i].m_uiType == i_uiType)
			return &s_arrTypeMetadata[i];
	return NULL;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL MD::IsTypeEditDecimal(tTYPE_ID i_nType)
{
	STypeMetadata* pMD = FindMetadata(i_nType);	
	ASSERT(NULL != pMD);
	return pMD->m_eEditMode == eTypeEditDecimal;
}

BOOL MD::IsTypeEditString(tTYPE_ID i_nType)
{
	STypeMetadata* pMD = FindMetadata(i_nType);	
	ASSERT(NULL != pMD);
	return pMD->m_eEditMode == eTypeEditString;
}

BOOL MD::IsTypeEditBinary(tTYPE_ID i_nType)
{
	STypeMetadata* pMD = FindMetadata(i_nType);	
	ASSERT(NULL != pMD);
	return pMD->m_eEditMode == eTypeEditBinary;
}

void MD::GetTypeImgAndTitle(tTYPE_ID i_uiType, UINT& o_uiImg, CString& o_strTypeTitle)
{	
	STypeMetadata* pMD = FindMetadata(i_uiType);	
	ASSERT(NULL != pMD);
	o_uiImg			= pMD->m_uiTreeImage;
	o_strTypeTitle	= pMD->m_strTitle;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
template<class T> void GetIntFormatted(T* i_nInt, CString& o_strRep)
{
	o_strRep.Format(_T("0x%08x (%d)"), *i_nInt, *i_nInt);
}

template<class T> void GetIntFormattedU(T* i_nInt, CString& o_strRep)
{
	o_strRep.Format(_T("0x%08x (%u)"), *i_nInt, *i_nInt);
}

void GetFormattedStr(LPCSTR i_strVal, DWORD i_dwSize, CString& o_strRep)
{
	lstrcpynA(o_strRep.GetBufferSetLength(4096), i_strVal, i_dwSize);
	o_strRep.ReleaseBuffer();
}

void GetFormattedStrW(LPCWSTR i_strVal, DWORD i_dwSize, CString& o_strRep)
{
	wsprintfA(o_strRep.GetBufferSetLength(4096), _T("%S"), i_strVal);
	o_strRep.ReleaseBuffer();
}

void GetFormattedBin(BYTE* i_pData, DWORD i_dwSize, CString& o_strRep)
{
	const DWORD cdwMaxItems = 100;
	if(cdwMaxItems < i_dwSize)
		i_dwSize = cdwMaxItems;

	LPTSTR strBuf = o_strRep.GetBufferSetLength(i_dwSize*3+1);
	for(DWORD i=0; i<i_dwSize; i++)
	{
		wsprintf(strBuf, _T("%02x "), i_pData[i]);
		strBuf += 3;
	}

	o_strRep.ReleaseBuffer();
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\// 
void MD::GetDataStrRep(tTYPE_ID i_uiType, void* i_pData, DWORD i_dwSize, 
	CString& o_strDataRep)
{
	switch(i_uiType)
	{
	case tid_VOID:		GetFormattedBin((BYTE*)i_pData, i_dwSize, o_strDataRep); break;
	case tid_BYTE:		GetIntFormattedU((tBYTE*)i_pData, o_strDataRep); break;
	case tid_WORD:		GetIntFormattedU((tWORD*)i_pData, o_strDataRep); break;
	case tid_DWORD:		GetIntFormattedU((tDWORD*)i_pData, o_strDataRep);  break;
	case tid_QWORD:		o_strDataRep = "DATA VALUE"; break;
	case tid_BOOL:		GetIntFormatted((tBOOL*)i_pData, o_strDataRep); break;
	case tid_CHAR:		GetFormattedStr((LPCSTR)i_pData, i_dwSize, o_strDataRep); break;
	case tid_WCHAR:		GetFormattedStrW((LPCWSTR)i_pData, i_dwSize, o_strDataRep); break;
	case tid_STRING:	GetFormattedStr((LPCSTR)i_pData, i_dwSize, o_strDataRep); break;
	case tid_WSTRING:	GetFormattedStrW((LPCWSTR)i_pData, i_dwSize, o_strDataRep); break;
	case tid_ERROR:		GetIntFormatted((tERROR*)i_pData, o_strDataRep);  break;
	case tid_PTR:		o_strDataRep = "DATA VALUE"; break;
	case tid_INT:		GetIntFormatted((tINT*)i_pData, o_strDataRep);  break; // native signed integer type
	case tid_UINT:		GetIntFormattedU((tUINT*)i_pData, o_strDataRep);  break;  // native unsigned integer type
	case tid_SBYTE:		GetIntFormatted((tSBYTE*)i_pData, o_strDataRep); break;
	case tid_SHORT:		GetIntFormatted((tSHORT*)i_pData, o_strDataRep);  break;
	case tid_LONG:		GetIntFormatted((tLONG*)i_pData, o_strDataRep);  break;
	case tid_LONGLONG:	GetIntFormatted((tLONG*)i_pData, o_strDataRep); break;
	case tid_IID:		o_strDataRep = "DATA VALUE"; break;   // plugin interface identifier
	case tid_PID:		o_strDataRep = "DATA VALUE"; break;     // plugin identifier
	case tid_ORIG_ID:	o_strDataRep = "DATA VALUE"; break; // object origin identifier
	case tid_OS_ID:		o_strDataRep = "DATA VALUE"; break;   // folder type identifier
	case tid_VID:		o_strDataRep = "DATA VALUE"; break;     // vendor ideftifier
	case tid_PROPID:	o_strDataRep = "DATA VALUE"; break;  // property identifier
	case tid_VERSION:	o_strDataRep = "DATA VALUE"; break; // version of any identifier: High word - version, Low word - subversion
	case tid_CODEPAGE:	o_strDataRep = "DATA VALUE"; break; // codepage identifier
	case tid_LCID:		o_strDataRep = "DATA VALUE"; break;    // codepage identifier
	case tid_DATA:		o_strDataRep = "DATA VALUE"; break;    // universaL, data storage
	case tid_DATETIME:	o_strDataRep = "DATA VALUE"; break;// universaL, date storage
	case tid_FUNC_PTR:	o_strDataRep = "DATA VALUE"; break; // universaL, function ptr
	case tid_BINARY:	GetFormattedBin((BYTE*)i_pData, i_dwSize, o_strDataRep); break;
	case tid_IFACEPTR:	o_strDataRep = "DATA VALUE"; break;
	case tid_OBJECT:	o_strDataRep = "DATA VALUE"; break;
	case tid_EXPORT:	o_strDataRep = "DATA VALUE"; break;
	case tid_IMPORT:	o_strDataRep = "DATA VALUE"; break;
	case tid_TRACE_LEVEL: o_strDataRep = "DATA VALUE"; break;
	case tid_TYPE_ID:	o_strDataRep = "DATA VALUE"; break;
	case tid_VECTOR:	o_strDataRep = "DATA VALUE"; break;
	case tid_STRING_OBJ: o_strDataRep = "DATA VALUE"; break;
	case tid_SERIALIZABLE: o_strDataRep = "DATA VALUE"; break;
	default:
		o_strDataRep = "";
	}
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\// 
BOOL MD::IsTypeEqShort(tTYPE_ID i_nType)
{
	return i_nType == tid_SHORT;
}	

BOOL MD::IsTypeEqInt(tTYPE_ID i_nType)	
{
	return i_nType == tid_INT;
}	

BOOL MD::IsTypeEqUINT(tTYPE_ID i_nType)	
{
	return i_nType == tid_UINT;
}	

BOOL MD::IsTypeEqLong(tTYPE_ID i_nType)	
{
	return i_nType == tid_LONG;
}	

BOOL MD::IsTypeEqBYTE(tTYPE_ID i_nType)	
{
	return i_nType == tid_BYTE;
}	

BOOL MD::IsTypeEqDWORD(tTYPE_ID i_nType)
{
	return i_nType == tid_DWORD;
}	
	
BOOL MD::IsTypeUnicode(tTYPE_ID i_nType)
{
	return i_nType == tid_WCHAR || i_nType == tid_WSTRING;
}	

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\// 
