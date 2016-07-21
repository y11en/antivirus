#define PR_IMPEX_DEF

#include "advdisim.h"

#include "mklif/mklapi/mklapi.h"

void
ReparseKeyName (
	cStringObj* pStrKey
	)
{
	pStrKey->toupper();
	
	pStrKey->replace_one(L"HKEY_LOCAL_MACHINE", fSTRING_COMPARE_CASE_SENSITIVE, L"\\REGISTRY\\MACHINE");
	pStrKey->replace_one(L"HKLM\\", fSTRING_COMPARE_CASE_SENSITIVE, L"\\REGISTRY\\MACHINE\\");

	pStrKey->replace_one(L"HKEY_CURRENT_USER\\", fSTRING_COMPARE_CASE_SENSITIVE, L"\\REGISTRY\\USER\\*\\");
	pStrKey->replace_one(L"HKEY_USERS\\", fSTRING_COMPARE_CASE_SENSITIVE, L"\\REGISTRY\\USER\\");
	pStrKey->replace_one(L"HKCU\\", fSTRING_COMPARE_CASE_SENSITIVE, L"\\REGISTRY\\USER\\*\\");

	pStrKey->replace_one(L"HKEY_CURRENT_CONFIG", fSTRING_COMPARE_CASE_SENSITIVE, L"\\REGISTRY\\MACHINE\\SYSTEM\\CURRENTCONTROLSET");

	pStrKey->replace_one(L"\\REGISTRY\\MACHINE\\SYSTEM\\CURRENTCONTROLSET", fSTRING_COMPARE_CASE_SENSITIVE, 
		L"\\REGISTRY\\MACHINE\\SYSTEM\\CONTROLSET???");

	pStrKey->replace_one(L"\\REGISTRY\\MACHINE\\SYSTEM\\CURRENTCONTROLSET", fSTRING_COMPARE_CASE_SENSITIVE, 
		L"\\REGISTRY\\MACHINE\\SYSTEM\\CONTROLSET???");

	pStrKey->replace_one(L"HKCR\\", fSTRING_COMPARE_CASE_SENSITIVE, L"\\REGISTRY\\MACHINE\\SOFTWARE\\CLASSES\\");
	pStrKey->replace_one(L"HKEY_CLASSES_ROOT\\", fSTRING_COMPARE_CASE_SENSITIVE, L"\\REGISTRY\\MACHINE\\SOFTWARE\\CLASSES\\");
	
	pStrKey->check_last_slash(false);
}

LONG gMemCounter = 0;

void* __cdecl pfMemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag )
{
	cObject* pThis = (cObject*) pOpaquePtr;

	PVOID ptr;
	if (PR_SUCC( pThis->heapAlloc( &ptr, size ) ))
	{
		InterlockedIncrement( & gMemCounter );
		return ptr;
	}
	
	return NULL;
};

void __cdecl pfMemFree (
	PVOID pOpaquePtr,
	void** pptr)
{
	if (!*pptr)
		return;

	cObject* pThis = (cObject*) pOpaquePtr;
	pThis->heapFree( *pptr );
	*pptr = NULL;

	InterlockedDecrement( & gMemCounter );
};


cInternal::cInternal()
{
	m_pThis = 0;
	m_pClientContext = NULL;
}

cInternal::~cInternal()
{
	if (m_pClientContext)
		MKL_ClientUnregister( &m_pClientContext );
}


bool
cInternal::Init(cObject* pThis)
{
	m_pThis = pThis;
	HRESULT hResult = MKL_ClientRegister(
		&m_pClientContext, 
		AVPG_Driver_Specific,
		AVPG_INFOPRIORITY,
		_CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_WITHOUTWATCHDOG,
		0,
		DEADLOCKWDOG_TIMEOUT,
		pfMemAlloc,
		pfMemFree,
		(PVOID) pThis
		);

	if (SUCCEEDED( hResult ))
		return true;

	return false;
}

bool
cInternal::SetActive (
	bool bActive
	)
{
	HRESULT hResult = MKL_ChangeClientActiveStatus( m_pClientContext, bActive ? TRUE : FALSE );
	if (PR_SUCC( hResult ))
		return true;

	return false;
}

bool
cInternal::ProtectFileMask (
	PWCHAR pwchFileMask
	)
{
	PVOID pThis = (PVOID) m_pThis;

	ULONG SizeOf_Url = sizeof(WCHAR) * (lstrlenW( pwchFileMask ) + 1);
	PFILTER_PARAM ParamUrl = (PFILTER_PARAM) pfMemAlloc( pThis, sizeof(FILTER_PARAM) + SizeOf_Url, 0 );
	
	if (!ParamUrl)
		return false;

	ParamUrl->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
	ParamUrl->m_ParamFlt = FLT_PARAM_WILDCARD;
	ParamUrl->m_ParamID = _PARAM_OBJECT_URL_W;
	lstrcpyW( (PWCHAR)ParamUrl->m_ParamValue, pwchFileMask );
	ParamUrl->m_ParamSize = SizeOf_Url;

	ULONG SizeOf_Param = sizeof(ULONG);
	PFILTER_PARAM ParamParam = (PFILTER_PARAM) pfMemAlloc( pThis, 0, sizeof(FILTER_PARAM) + SizeOf_Param );
	if (!ParamParam)
	{
		pfMemFree( pThis, (void**) &ParamUrl );
		return false;
	}
	
	ParamParam->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
	ParamParam->m_ParamFlt = FLT_PARAM_AND;
	ParamParam->m_ParamID = _PARAM_OBJECT_ACCESSATTR;
	*(ULONG*)ParamParam->m_ParamValue = FILE_WRITE_DATA;
	ParamParam->m_ParamSize = SizeOf_Param;

	ULONG FltId = 0;
	HRESULT hResult = MKL_AddFilter( &FltId, m_pClientContext, "*", DEADLOCKWDOG_TIMEOUT, 
		FLT_A_DENY, FLTTYPE_NFIOR, IRP_MJ_CREATE, 0,
		0, PostProcessing, NULL, ParamUrl, ParamParam, NULL );

	pfMemFree( pThis, (void**) &ParamUrl );
	pfMemFree( pThis, (void**) &ParamParam );

	if (SUCCEEDED( hResult ))
		return true;

	return false;
}

bool
cInternal::ProtectKey (
	PWCHAR KeyPattern,
	PWCHAR ValuePattern )
{
	PVOID pThis = (PVOID) m_pThis;

	ULONG SizeOf_Url = sizeof(WCHAR) * (lstrlenW( KeyPattern ) + 1);
	PFILTER_PARAM ParamUrl = (PFILTER_PARAM) pfMemAlloc( pThis, sizeof(FILTER_PARAM) + SizeOf_Url, 0 );
	
	if (!ParamUrl)
		return false;

	ParamUrl->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
	ParamUrl->m_ParamFlt = FLT_PARAM_WILDCARD;
	ParamUrl->m_ParamID = _PARAM_OBJECT_URL_W;
	lstrcpyW( (PWCHAR)ParamUrl->m_ParamValue, KeyPattern );
	ParamUrl->m_ParamSize = SizeOf_Url;

	ULONG SizeOf_Param = sizeof(WCHAR) * (lstrlenW( ValuePattern ) + 1);
	PFILTER_PARAM ParamParam = (PFILTER_PARAM) pfMemAlloc( pThis, sizeof(FILTER_PARAM) + SizeOf_Param, 0 );
	
	if (!ParamParam)
	{
		pfMemFree( pThis, (void**) &ParamUrl);
		return false;
	}

	ParamParam->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
	ParamParam->m_ParamFlt = FLT_PARAM_WILDCARD;
	ParamParam->m_ParamID = _PARAM_OBJECT_URL_PARAM_W;
	lstrcpyW((PWCHAR)ParamParam->m_ParamValue, ValuePattern);
	ParamParam->m_ParamSize = SizeOf_Param;

	ULONG FltId = 0;
	HRESULT hResult = MKL_AddFilter( &FltId, m_pClientContext, "*", DEADLOCKWDOG_TIMEOUT, 
		FLT_A_DENY, FLTTYPE_REGS, Interceptor_SetValueKey, 0,
		0, PreProcessing, NULL, ParamUrl, ParamUrl, ParamParam, NULL );

	pfMemFree( pThis, (void**) &ParamUrl);
	pfMemFree( pThis, (void**) &ParamParam);

	if (SUCCEEDED( hResult ))
		return true;

	return false;
}
