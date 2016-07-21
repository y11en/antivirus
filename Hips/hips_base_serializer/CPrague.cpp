#include <windows.h>
#include "CPrague.h"

EXTERN_C hROOT g_root = NULL;

// -----------------------------------------------------------------------------------------
static tBOOL GetModulePath(tVOID* hInstance, tCHAR* szPath, tDWORD dwSize)
{
	int cou;
	szPath[0] = 0;
	int length = GetModuleFileNameA((HINSTANCE)hInstance, szPath, dwSize);
	if (length == 0)
		return FALSE;
	
	cou = length;
	while ((cou >= 0) && (szPath[cou] != '\\')) 
		cou--;
	szPath[cou+1] = '\0';
	
	return TRUE;
}

// -----------------------------------------------------------------------------------------
PR_MAKE_TRACE_FUNC;

static void Trace( tSTRING str ) {
	static char b[0x1000];
	int l = strlen( str );
	if ( str[l] != '\n' ) {
		memcpy( b, str, l );
		*(WORD*)(b + l) = MAKEWORD( '\n', 0 );
		str = b;
	}
	OutputDebugStringA(str);
}

// -----------------------------------------------------------------------------------------

static tERROR pr_call chk_root( hROOT hRoot, tVOID* params ) 
{
	if ( hRoot ) {
		CALL_SYS_PropertySetDWord( hRoot, pgOUTPUT_FUNC, (tDWORD)Trace );
#ifdef _DEBUG
		CALL_SYS_TraceLevelSet( hRoot, tlsALL_OBJECTS, prtERROR, 0 );
#else
		CALL_SYS_TraceLevelSet( hRoot, tlsTHIS_OBJECT, prtDANGER, 0 );
#endif
	}
	return errOK;
}

// -----------------------------------------------------------------------------------------

_CPrague::_CPrague()
{
	Init();
}

_CPrague::_CPrague(tCHAR* pchLoadPath)
{
	Init();
	LoadPrague(pchLoadPath);
}

_CPrague::~_CPrague()
{
	UnloadPrague();
}

tBOOL _CPrague::LoadPrague()
{
	return LoadPrague(NULL);
}

tBOOL _CPrague::LoadPrague(tCHAR* pchLoadPath)
{
	BOOL bRet = FALSE;
	const size_t LoadPathSize = MAX_PATH*sizeof(tCHAR);

	UnloadPrague();

	m_pchLoadPath = (tCHAR*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, LoadPathSize);
	if (pchLoadPath)
		strncpy(m_pchLoadPath, pchLoadPath, LoadPathSize);
	else
		GetModulePath(NULL, m_pchLoadPath, LoadPathSize);

	strncat( m_pchLoadPath, PR_LOADER_MODULE_NAME, LoadPathSize - strlen(m_pchLoadPath));
	m_hLoader = LoadLibraryA(m_pchLoadPath);
	if (m_hLoader == INVALID_HANDLE_VALUE)
	{
		//Loader not found
	}
	else
	{
		m_plf = (PragueLoadFuncEx) GetProcAddress((HINSTANCE)m_hLoader, PR_LOADER_LOAD_FUNC);
		m_puf = (PragueUnloadFunc) GetProcAddress((HINSTANCE)m_hLoader, PR_LOADER_UNLOAD_FUNC);
		
		if (m_plf == NULL)
		{
			// wrong loader
		}
		else
		{
			tERROR err = m_plf(&m_hRoot, PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH, chk_root, 0, 0);
			if (PR_FAIL(err))
			{
				// error - failed init prague
			}
			else
			{
				if (!g_root)
					g_root = m_hRoot;
				bRet = TRUE;
			}
		}
	}
	return bRet;
}

void _CPrague::Init()
{
	m_hLoader = (HINSTANCE) INVALID_HANDLE_VALUE;
	m_plf = 0;
	m_puf = 0;
	m_hRoot = 0;
	m_pchLoadPath = 0;
	m_dwInitFlags = 0;
}

void _CPrague::UnloadPrague()
{
	if (m_puf)
		m_puf(m_hRoot);
	if (g_root == m_hRoot)
		g_root = NULL;
	m_hRoot = NULL;

	if (m_hLoader != INVALID_HANDLE_VALUE)
		FreeLibrary((HINSTANCE)m_hLoader);
	m_hLoader = NULL;

	if (m_pchLoadPath)
		HeapFree(GetProcessHeap(), 0, m_pchLoadPath);
	m_pchLoadPath = NULL;
	
	Init();
}

