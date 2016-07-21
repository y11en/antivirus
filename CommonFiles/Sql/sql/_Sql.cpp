//-------------------------------------------------------------------------//
//$(DebugDLL)_Sql.lib odbc32.lib ODBCCP32.LIB

#include "stdafx.h"
#include "malloc.h"
#include "sqlext.h"
#include "sqlucode.h"
#include "odbcss.h"
#include "odbcinst.h"
#include "Shlwapi.h"

#define SQL_LIB_BILDING
#include "..\_Sql.h"

#define TRACE_LEVEL 5	      
#include "AvpDebug.h"

#define TRACE_QUERY	0

#define RETCODE_IS_SUCCESS(x) ((x) == SQL_SUCCESS || (x) == SQL_SUCCESS_WITH_INFO)
#define RETCODE_IS_FAILURE(x) ((x) == SQL_ERROR || (x) == SQL_INVALID_HANDLE)
#define RETCODE_IS_SUCCESSFUL(x) (!RETCODE_IS_FAILURE(x))

// empty branch warning
#pragma warning ( disable : 4390 )

//-------------------------------------------------------------------------//
#define W2A(w, a, cb)     WideCharToMultiByte( CP_ACP, 0, w, -1, a, cb, NULL, NULL)
#define A2W(a, w, cb)     MultiByteToWideChar( CP_ACP, 0, a, -1, w, cb )

#define CONVERT_TO_WCHAR(strVarA, strVarW) \
	int nQLen##strVarA = lstrlenA((CHAR*)strVarA);	\
	WCHAR *strVarW = (WCHAR*) _alloca(sizeof(WCHAR) * (nQLen##strVarA + 1)); \
	A2W((CHAR*)strVarA, strVarW, nQLen##strVarA); \
	strVarW[nQLen##strVarA] = 0;

#define CONVERT_TO_CHAR(strVarW, strVarA) \
	int nQLen##strVarW = lstrlenW((WCHAR*)strVarW);	\
	CHAR *strVarA = (CHAR*) _alloca(sizeof(CHAR) * (nQLen##strVarW + 1)); \
	W2A((WCHAR*)strVarW, strVarA, nQLen##strVarW); \
	strVarA[nQLen##strVarW] = 0;

//-------------------------------------------------------------------------//
#ifdef _UNICODE
#define SQLTCHAR TCHAR
#else
#define SQLTCHAR SQLCHAR
#endif

//-------------------------------------------------------------------------//
#ifdef _DEBUG
#define _SQL_MM_Alloc(size)  (void*) new BYTE[size]
#define _SQL_MM_Free(ptr) delete ptr
#define _MM_Compact()
#else
LPVOID _SQL_MM_Alloc(DWORD size)
{
	LPVOID ptr = HeapAlloc(GetProcessHeap(), 0, size);
	return ptr;
}

void _SQL_MM_Free(LPVOID ptr)
{
	if (ptr != NULL)
		HeapFree(GetProcessHeap(), 0, ptr);
}

void _MM_Compact()
{
	HeapCompact(GetProcessHeap(), 0);
}
#endif
//-------------------------------------------------------------------------//

SQLPOINTER inlGetODBCConcurrencyId(CSql::eConcurrency i_eConcurrency)
{
	switch(i_eConcurrency)
	{
	case CSql::conc_READ_ONLY:	return (SQLPOINTER)SQL_CONCUR_READ_ONLY; 
	case CSql::conc_LOCK:		return (SQLPOINTER)SQL_CONCUR_LOCK; 
	case CSql::conc_ROWVER:		return (SQLPOINTER)SQL_CONCUR_ROWVER; 
	case CSql::conc_VALUES:		return (SQLPOINTER)SQL_CONCUR_VALUES; 
	case CSql::conc_DEFAULT:	return (SQLPOINTER)SQL_CONCUR_DEFAULT; 
	}
	return 0;
}

SQLPOINTER inlGetODBCCursotTypeId(CSql::eCursorType i_eCursorType)
{
	switch(i_eCursorType)
	{
	case CSql::cursor_FORWARD_ONLY:		return (SQLPOINTER)SQL_CURSOR_FORWARD_ONLY; 
	case CSql::cursor_KEYSET_DRIVEN:	return (SQLPOINTER)SQL_CURSOR_KEYSET_DRIVEN; 
	case CSql::cursor_DYNAMIC:			return (SQLPOINTER)SQL_CURSOR_DYNAMIC; 
	case CSql::cursor_STATIC:			return (SQLPOINTER)SQL_CURSOR_STATIC; 
	case CSql::cursor_TYPE_DEFAULT:		return (SQLPOINTER)SQL_CURSOR_TYPE_DEFAULT; 
	}
	return 0;
}

SQLPOINTER inlGetODBCScrollabilityId(CSql::eScrollability i_eScrollability)
{
	switch(i_eScrollability)
	{
	case CSql::scroll_NONSCROLLABLE:return (SQLPOINTER)SQL_NONSCROLLABLE; 
	case CSql::scroll_SCROLLABLE:	return (SQLPOINTER)SQL_SCROLLABLE; 
	}
	return 0;
}

SQLPOINTER inlGetODBCCursorSensitivityId(CSql::eCursorSensitivity i_eCursorSensitivity)
{
	switch(i_eCursorSensitivity)
	{
	case CSql::sensit_UNSPECIFIED:	return (SQLPOINTER)SQL_UNSPECIFIED; 
	case CSql::sensit_INSENSITIVE:	return (SQLPOINTER)SQL_INSENSITIVE; 
	case CSql::sensit_SENSITIVE:	return (SQLPOINTER)SQL_SENSITIVE; 
	}
	return 0;
}


SQLSMALLINT inlGetODBCFetchOrientationId(CSql::eFetchOrientation i_eFetchOrientation)
{
	switch(i_eFetchOrientation)
	{
	case CSql::fetch_FIRST:		return SQL_FETCH_FIRST; 
	case CSql::fetch_LAST:		return SQL_FETCH_LAST; 
	case CSql::fetch_PRIOR:		return SQL_FETCH_PRIOR; 
	case CSql::fetch_ABSOLUTE:	return SQL_FETCH_ABSOLUTE; 
	case CSql::fetch_RELATIVE:	return SQL_FETCH_RELATIVE; 
	}
	return SQL_FETCH_NEXT;
}

//-------------------------------------------------------------------------//
CSql::CSql()
{
	m_hdbc			= SQL_NULL_HDBC;
	m_henv			= SQL_NULL_HENV;
	m_hstmt			= SQL_NULL_HSTMT;
	pDescribeCol	= NULL;
	pBoundCol		= NULL;
	m_bConnected	= FALSE;
	m_bChildConnect = FALSE;
	m_bBusy			= FALSE;
	m_bDataReady	= FALSE;
	m_ulImageSize	= 0;
	m_ResultRowNumber = 1;
	m_bShowDeleted	= FALSE;
	m_Async			= FALSE;
	m_bDisableAutoTranslation = FALSE;

	m_eConcurrency	= eConcurrency(0);
	m_eCursorType	= eCursorType(0);
	m_eScrollability= eScrollability(0);
	m_eCursorSensitivity = eCursorSensitivity(0);
}

CSql::~CSql()
{
	DropConnect();
	Clean();
}

//-------------------------------------------------------------------------//
// wConfigMode = ODBC_USER_DSN | ODBC_SYSTEM_DSN | ODBC_BOTH_DSN
BOOL CSql::SetConfigMode(UWORD wConfigMode)
{
	return SQLSetConfigMode(wConfigMode);
}

BOOL CSql::GetConfigMode(UWORD* pwConfigMode)
{
	return SQLGetConfigMode(pwConfigMode);
}

//-------------------------------------------------------------------------//
HDBC CSql::PrepareHDBC(void)
{
	SQLRETURN rc;
	if (m_hdbc != SQL_NULL_HDBC)
	{
		KL_TRACE(_T("Prepare was already called"));
		return m_hdbc;
	}

	rc = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &m_henv);

	KL_ASSERT( RETCODE_IS_SUCCESSFUL(rc) );

	if (RETCODE_IS_SUCCESSFUL(rc))
	{
		rc = SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, SQL_IS_INTEGER);
		if (RETCODE_IS_SUCCESSFUL(rc))
		{
			rc = SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &m_hdbc);
			KL_ASSERT( RETCODE_IS_SUCCESSFUL(rc) );
			if (RETCODE_IS_SUCCESSFUL(rc)) 
				return m_hdbc;
		}
		else
			KL_TRACE(_T("incorrect version\n"));
		SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
	}

	return SQL_NULL_HDBC;
}

//-------------------------------------------------------------------------//
BOOL CSql::Connect(TCHAR* db_name, TCHAR* user_name, TCHAR* password, BOOL bBrowse)
{
	SQLRETURN rc;
	if (m_bChildConnect == TRUE)
	{
		m_bConnected = TRUE;
		return TRUE;
	}

	m_hdbc = PrepareHDBC();
	if (m_hdbc != SQL_NULL_HDBC)
	{
		if (bBrowse == FALSE)
		{
			rc = SQLConnect(m_hdbc, (SQLTCHAR*)db_name, SQL_NTS, (SQLTCHAR*)user_name, SQL_NTS, (SQLTCHAR*)password, SQL_NTS);
			if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
			{
				m_bConnected = TRUE;
				return TRUE;
			}
			KL_TRACE(_T("Connect failed..."));
		}
		else
		{
			// browse for connect
			TCHAR       szConnStrIn[MAX_PATH], szConnStrOut[MAX_PATH];
			SQLSMALLINT   cbConnStrOut;

			lstrcpy(szConnStrIn, _T("DRIVER={SQL Server};"));
			rc = SQLBrowseConnect(m_hdbc, (SQLTCHAR*)szConnStrIn, SQL_NTS, (SQLTCHAR*)szConnStrOut, MAX_PATH, &cbConnStrOut);
		}

		DropConnect();
	}

	return FALSE;
}



//-------------------------------------------------------------------------//
BOOL CSql::ConnectInternal	(TCHAR* strConnectionStr, HWND hwnd, BOOL i_bNoPrompt)
{
	DropConnect();

#ifdef UNICODE
	lstrcpy(m_strCmd, strConnectionStr);
#else
	CONVERT_TO_WCHAR(strConnectionStr, wstrConnectionStr);
	lstrcpyW(m_strCmd, wstrConnectionStr);
#endif

	m_hdbc = PrepareHDBC();

	if (m_hdbc == SQL_NULL_HDBC)
		return FALSE;

	SQLRETURN		rc;
	TCHAR*			szConnStrIn = strConnectionStr;
	SQLSMALLINT		cbConnStrIn;
	SQLSMALLINT		cbConnStrOutMax;
	SQLSMALLINT		cbConnStrOut;
	SQLUSMALLINT    fDriverCompletion;

	KL_TRACE1(_T("Try to connect - %s"), szConnStrIn);

	cbConnStrIn = lstrlen(szConnStrIn);
	cbConnStrOutMax = sizeof(SQLCHAR);
	cbConnStrOut = 0;
	fDriverCompletion = SQL_DRIVER_NOPROMPT;

	SQLSetConnectAttr(m_hdbc, SQL_COPT_SS_BCP, (void*) SQL_BCP_ON, SQL_IS_INTEGER);

	rc = SQLDriverConnect(m_hdbc, hwnd, (SQLTCHAR*) szConnStrIn, SQL_NTS, NULL, 0, NULL, 
					i_bNoPrompt ? SQL_DRIVER_NOPROMPT : SQL_DRIVER_COMPLETE); 

	if (RETCODE_IS_SUCCESSFUL(rc))
	{
		m_bConnected = TRUE;
		return TRUE;
	}

	return FALSE;
}

//-------------------------------------------------------------------------//
BOOL CSql::ChildConnect(CSql* pSqlParent)
{
	if (IsConnected() == FALSE)
	{
        m_henv = pSqlParent->m_henv;
        m_hdbc = pSqlParent->m_hdbc;
        m_bChildConnect = TRUE;
        if ((m_henv != NULL) && (m_hdbc != NULL))
            m_bConnected = TRUE;
        else
            m_bConnected = FALSE;
		return m_bConnected;
	}
	else
		return FALSE;
};

//-------------------------------------------------------------------------//
BOOL CSql::DropConnect()
{
	if(!IsConnected())
		return TRUE;

	DropData();

	if ((pDescribeCol != NULL) || (pBoundCol != NULL))
		KL_TRACE(_T("_sql: data is alive..."));

	if (m_bChildConnect)
		m_hstmt = SQL_NULL_HSTMT;
	else
	{
		KL_TRACE(_T("Dropping connection..."));

		if(m_hdbc != SQL_NULL_HDBC)
		{
			SQLDisconnect(m_hdbc);
			SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);
		}

		if(m_henv != SQL_NULL_HENV)
			SQLFreeHandle(SQL_HANDLE_ENV, m_henv);

		m_hdbc = SQL_NULL_HDBC;
		m_henv = SQL_NULL_HENV;
	}

	m_bConnected = FALSE;

	return TRUE;
}

//-------------------------------------------------------------------------//
void CSql::DropData()
{
	if (m_bBusy == TRUE)
		return;

	m_bDataReady = FALSE;
	if (m_hstmt != SQL_NULL_HSTMT)
	{
		SQLFreeStmt(m_hstmt, SQL_DROP);
		m_hstmt = SQL_NULL_HSTMT;
	}

	Clean();
}

// -----------------------------------------------------------------------------------------
#ifdef _DEBUG
void CSql::TraceDiagRecords()
{
	TCHAR	strState[6];
	int		nNativeError;
	TCHAR	strErrMsg[_ERROR_BUF];

	for(int k=1; GetDiagRec(TRUE, k, strState, nNativeError, strErrMsg, _ERROR_BUF); k++)
		KL_TRACE3(_T("SqlState: '%s', NativeErrorNmb: %d, Message: %s."), strState, nNativeError, strErrMsg);
}
#endif

// -----------------------------------------------------------------------------------------
void inlRemovePassage(LPTSTR o_strSrc, LPTSTR i_strPassage)
{
	int nPasLen  = lstrlen(i_strPassage);
	int nMaxChar = lstrlen(o_strSrc) - nPasLen + 1;

	for(int i=0; i<nMaxChar; i++)
	{
		int j;
		for(j=0; j<nPasLen; j++)
			if(o_strSrc[i+j] != i_strPassage[j])
				break;
	
		if(j==nPasLen)	
		{
			lstrcpy(o_strSrc+i, o_strSrc + i + nPasLen);
			return;		
		}
	}
}


BOOL CSql::GetDiagRec( BOOL i_bStatementHandle, int i_nRecNmb, LPTSTR o_strState, int& o_nNativeError, LPTSTR o_strMsg, int i_nMsgBufLen ) {
	SQLSMALLINT nHandleType = i_bStatementHandle ? SQL_HANDLE_STMT : SQL_HANDLE_DBC;
	SQLHANDLE	hHandle		= i_bStatementHandle ? m_hstmt : m_hdbc;
	SQLSMALLINT nRecNmb		= i_nRecNmb;
	SQLINTEGER	nNativeError;

	SQLRETURN nRetVal = SQLGetDiagRec(nHandleType, hHandle, nRecNmb, 
			(SQLTCHAR*)o_strState, &nNativeError, (SQLTCHAR*)o_strMsg, i_nMsgBufLen, NULL);

	if(SQL_SUCCESS != nRetVal && SQL_SUCCESS_WITH_INFO != nRetVal)
		return FALSE;

	o_nNativeError = nNativeError;
	inlRemovePassage(o_strMsg, _T("[Microsoft][ODBC SQL Server Driver]"));		
	inlRemovePassage(o_strMsg, _T("[Microsoft][ODBC Driver Manager]"));		
	
	return TRUE;
}

// -----------------------------------------------------------------------------------------
BOOL CSql::SetAsync(BOOL bAsync)
{
	SQLRETURN rc =
		SQLSetStmtOption(m_hstmt, SQL_ASYNC_ENABLE, 
			bAsync ? SQL_ASYNC_ENABLE_ON : SQL_ASYNC_ENABLE_OFF);
	return !RETCODE_IS_FAILURE(rc);
}

// -----------------------------------------------------------------------------------------
void CSql::Cancel()
{
	if (m_hstmt != SQL_NULL_HSTMT)
		SQLCancel(m_hstmt);
}

// -----------------------------------------------------------------------------------------
void CSql::BindAllColumns()
{
	SWORD cbCols;
	SDWORD cbValueMax;

	cbCols = pDescribeCol[0].iTotalCol;
	pBoundCol = (TSBoundCol*) _SQL_MM_Alloc(sizeof(TSBoundCol) * cbCols); 
	
	int cou;
	for (cou = 0; cou < cbCols; cou++)
	{
		pBoundCol[cou].iCol = pDescribeCol[cou].iCol;
		switch (pDescribeCol[cou].fSqlType)
		{
		case SQL_C_WCHAR:
		case SQL_WVARCHAR:
			pBoundCol[cou].fSqlType = SQL_C_WCHAR;
			cbValueMax = (pDescribeCol[cou].cbColDef + 1) * 2;
			break;
		case SQL_VARCHAR:
			pBoundCol[cou].fSqlType = SQL_C_CHAR;
			cbValueMax = pDescribeCol[cou].cbColDef + 1;
			break;

		case SQL_CHAR:
			pBoundCol[cou].fSqlType = SQL_C_CHAR;
			cbValueMax = pDescribeCol[cou].cbColDef + 1;
			break;

		case SQL_NUMERIC:
		case SQL_DECIMAL:
			pBoundCol[cou].fSqlType = SQL_C_DOUBLE;
			cbValueMax = sizeof(SDOUBLE);
			break;

		case SQL_BIGINT:
			pBoundCol[cou].fSqlType = SQL_C_CHAR;
			cbValueMax = pDescribeCol[cou].cbColDef;
			break;

		case SQL_INTEGER:
			if (pDescribeCol[cou].bUnsigned)
			{
				pBoundCol[cou].fSqlType = SQL_C_ULONG;
				cbValueMax = sizeof(SQLUINTEGER);
			}
			else
			{
				pBoundCol[cou].fSqlType = SQL_C_SLONG;
				cbValueMax = sizeof(SDWORD);
			}
			break;

		case SQL_SMALLINT:
			if (pDescribeCol[cou].bUnsigned)
			{
				pBoundCol[cou].fSqlType = SQL_C_USHORT;
				cbValueMax = sizeof(UWORD);
			}
			else
			{
				pBoundCol[cou].fSqlType = SQL_C_SSHORT;
				cbValueMax = sizeof(SWORD);
			}
			break;

		case SQL_FLOAT:
		case SQL_DOUBLE:
			pBoundCol[cou].fSqlType = SQL_C_DOUBLE;
			cbValueMax = sizeof(SDOUBLE);
			break;

		case SQL_REAL:
			pBoundCol[cou].fSqlType = SQL_C_FLOAT;
			cbValueMax = sizeof(SFLOAT);
			break;

		case SQL_DATE:
		case SQL_TYPE_DATE:
			pBoundCol[cou].fSqlType = SQL_C_TYPE_DATE;
			cbValueMax = sizeof(DATE_STRUCT);
			break;

		case SQL_TIME:
		case SQL_TYPE_TIME:
			pBoundCol[cou].fSqlType = SQL_C_TYPE_TIME;
			cbValueMax = sizeof(TIME_STRUCT);
			break;

		case SQL_TIMESTAMP:
		case SQL_TYPE_TIMESTAMP:
			pBoundCol[cou].fSqlType = SQL_C_TYPE_TIMESTAMP;
			cbValueMax = sizeof(TIMESTAMP_STRUCT);
			break;

		case SQL_BINARY:
			pBoundCol[cou].fSqlType = SQL_C_BINARY;
			cbValueMax = pDescribeCol[cou].cbColDef;
			break;

		case SQL_VARBINARY:
			cbValueMax = pDescribeCol[cou].cbColDef;
			if ((pDescribeCol[0].bBindBookmark) && (cou == 0))
				pBoundCol[cou].fSqlType = SQL_C_VARBOOKMARK;
			else
				pBoundCol[cou].fSqlType = SQL_C_BINARY;
			break;
		case SQL_LONGVARCHAR:
		case SQL_WLONGVARCHAR:
		case SQL_LONGVARBINARY:
			pBoundCol[cou].fSqlType = SQL_C_BINARY;
			cbValueMax = m_ulImageSize;
			break;
		case SQL_TINYINT:
			if (pDescribeCol[cou].bUnsigned)
			{
				pBoundCol[cou].fSqlType = SQL_C_UTINYINT;
				cbValueMax = sizeof(SQLCHAR);
			}
			else
			{
				pBoundCol[cou].fSqlType = SQL_C_STINYINT;
				cbValueMax = sizeof(SCHAR);
			}
			break;

		case SQL_BIT:
			pBoundCol[cou].fSqlType = SQL_C_BIT;
			cbValueMax = sizeof(SQLCHAR);
			break;

			/*case SQL_TIMESTAMP:
			pBoundCol[cou].fSqlType = SQL_C_BIT;
			cbValueMax = sizeof(SQLCHAR);
			break;*/

		default:
			pBoundCol[cou].fSqlType = SQL_C_DEFAULT;
			cbValueMax = pDescribeCol[cou].cbColDef + 3;
			break;
		}

		if (cbValueMax != 0)
			pBoundCol[cou].pBuffer = _SQL_MM_Alloc(cbValueMax);
		else
			pBoundCol[cou].pBuffer = NULL;
		pBoundCol[cou].BufferSize = cbValueMax;
	}
}

// -----------------------------------------------------------------------------------------
void CSql::UpdateColInfo(BOOL bBindBookmark)
{
	SQLRETURN rc;
	SDWORD iIntegerValue;

	Clean();

	SWORD cbCols = GetTotalColumns();
	if (bBindBookmark)
		cbCols++;
	if (cbCols > 0)
	{
		pDescribeCol = (PTSDescribeCol) _SQL_MM_Alloc(sizeof(SDescribeCol) * cbCols);
		pDescribeCol->bBindBookmark = bBindBookmark;

		int cou;
		int tmpcou;
	
		int Startcou = 1;
		int Maxcou = cbCols;
		if (bBindBookmark)
		{
			Startcou = 0;
			Maxcou--;
		}

		for (cou = Startcou; cou <= Maxcou; cou++)
		{
			tmpcou = cou - 1; 
			if (bBindBookmark)
				tmpcou = cou;
			
			pDescribeCol[tmpcou].iTotalCol = cbCols;

			pDescribeCol[tmpcou].iCol = (UWORD) cou;
			pDescribeCol[tmpcou].pcbColName = COLNAMEMAX;
			SQLDescribeCol(m_hstmt, cou, (SQLTCHAR*)pDescribeCol[tmpcou].szColName, COLNAMEMAX,&(pDescribeCol[tmpcou].pcbColName), 
				&(pDescribeCol[tmpcou].fSqlType), (SQLULEN *)&(pDescribeCol[tmpcou].cbColDef), &(pDescribeCol[tmpcou].pibScale),
				&(pDescribeCol[tmpcou].pfNullable));
			
	// Extended attributes
			rc = SQLColAttributes(m_hstmt, cou, SQL_DESC_CONCISE_TYPE, NULL, 0, NULL, (SQLLEN *)&iIntegerValue);

			rc = SQLColAttributes(m_hstmt, cou, SQL_COLUMN_AUTO_INCREMENT, NULL, 0, NULL, (SQLLEN *)&iIntegerValue);
			if (RETCODE_IS_SUCCESSFUL(rc))
				pDescribeCol[tmpcou].bAutoIncrement = (iIntegerValue == TRUE);

			rc = SQLColAttributes(m_hstmt, cou, SQL_COLUMN_CASE_SENSITIVE, NULL, 0, NULL, (SQLLEN *)&iIntegerValue);
			if (RETCODE_IS_SUCCESSFUL(rc))
				pDescribeCol[tmpcou].bCaseSensitive = (iIntegerValue == TRUE);

			rc = SQLColAttributes(m_hstmt, cou, SQL_COLUMN_DISPLAY_SIZE, NULL, 0, NULL, (SQLLEN *)&iIntegerValue);
			if (RETCODE_IS_SUCCESSFUL(rc))
				pDescribeCol[tmpcou].iDisplaySize = iIntegerValue;
			else
				pDescribeCol[tmpcou].iDisplaySize = 0;

			rc = SQLColAttributes(m_hstmt, cou, SQL_COLUMN_LABEL, pDescribeCol[tmpcou].szColumnLabel, 
				sizeof(pDescribeCol[tmpcou].szColumnLabel), 
				&(pDescribeCol[tmpcou].pcbColName), NULL);

			if (cou == 0)
				lstrcpy((TCHAR*)pDescribeCol[tmpcou].szColumnLabel, _T("Bookmark"));

			rc = SQLColAttributes(m_hstmt, cou, SQL_COLUMN_LENGTH, NULL, 0, NULL, (SQLLEN *)&iIntegerValue);
			if (RETCODE_IS_SUCCESSFUL(rc))
				pDescribeCol[tmpcou].iLength = iIntegerValue;
			else
				pDescribeCol[tmpcou].iLength = 0;

			rc = SQLColAttributes(m_hstmt, cou, SQL_COLUMN_MONEY, NULL, 0, NULL, (SQLLEN *)&iIntegerValue);
			if (RETCODE_IS_SUCCESSFUL(rc))
				pDescribeCol[tmpcou].bMoney = (iIntegerValue == TRUE);
			else
				pDescribeCol[tmpcou].bMoney = FALSE;
			
			rc = SQLColAttributes(m_hstmt, cou, SQL_COLUMN_OWNER_NAME, pDescribeCol[tmpcou].szOwnerName, 
				sizeof(pDescribeCol[tmpcou].szOwnerName), &(pDescribeCol[tmpcou].pcbColName), NULL);
			if(RETCODE_IS_FAILURE(rc))
				pDescribeCol[tmpcou].szOwnerName[0] = '\0';

			rc = SQLColAttributes(m_hstmt, cou,	SQL_COLUMN_QUALIFIER_NAME, pDescribeCol[tmpcou].szQualifier, 
				sizeof(pDescribeCol[tmpcou].szQualifier), &(pDescribeCol[tmpcou].pcbColName), NULL);

			rc = SQLColAttributes(m_hstmt, cou, SQL_COLUMN_SEARCHABLE, NULL, 0 , NULL, (SQLLEN *)&(pDescribeCol[tmpcou].fSearchable));

			rc = SQLColAttributes(m_hstmt, cou, SQL_COLUMN_TABLE_NAME, pDescribeCol[tmpcou].szTableName, 
				sizeof(pDescribeCol[tmpcou].szTableName), &(pDescribeCol[tmpcou].pcbColName), NULL);
			if(RETCODE_IS_FAILURE(rc))
				pDescribeCol[tmpcou].szTableName[0] = '\0';

			rc = SQLColAttributes(m_hstmt, cou, SQL_COLUMN_TYPE_NAME, pDescribeCol[tmpcou].szTypeName, 
				sizeof(pDescribeCol[tmpcou].szTypeName), &(pDescribeCol[tmpcou].pcbColName), NULL);
			if(RETCODE_IS_FAILURE(rc))
				pDescribeCol[tmpcou].szTypeName[0] = '\0';

			rc = SQLColAttributes(m_hstmt, cou, SQL_COLUMN_UNSIGNED, NULL, 0, NULL, (SQLLEN *)&iIntegerValue);
			if (RETCODE_IS_SUCCESSFUL(rc))
				pDescribeCol[tmpcou].bUnsigned = (iIntegerValue == TRUE);
			else
				pDescribeCol[tmpcou].bUnsigned = FALSE;

			rc = SQLColAttributes(m_hstmt, cou,
				SQL_COLUMN_UPDATABLE, NULL, 0, NULL, (SQLLEN *)&(pDescribeCol[tmpcou].fUpdatable));
	// End extended attributes
		}
		BindAllColumns();
	}
}

// -----------------------------------------------------------------------------------------
SWORD CSql::GetTotalColumns()
{
	if (m_hstmt == SQL_NULL_HSTMT)
		return -1; 

	SWORD cbCols = -1;
	SQLRETURN rc = SQLNumResultCols(m_hstmt, &cbCols);

	if(RETCODE_IS_FAILURE(rc))
	{
		TCHAR strState	[6];
		int nNativeError;

		if(GetDiagRec(TRUE, 1, strState, nNativeError, NULL, 0) &&
			0 == lstrcmp(STATE_STATEMENT_NOT_PREPARED, strState))
				cbCols = 0;
	}

	return cbCols;
}

// -----------------------------------------------------------------------------------------
BOOL CSql::CanUpdate()
{
	if (!IsConnected())
		return FALSE;

	SQLRETURN rc;
	SQLUINTEGER fPos;
	SWORD cbValue;

	rc = SQLGetInfo(m_hdbc, SQL_POSITIONED_STATEMENTS, &fPos, sizeof (fPos), &cbValue);
	return RETCODE_IS_SUCCESSFUL(rc);
}

// -----------------------------------------------------------------------------------------
BOOL CSql::CanUseBookmark()
{
	if (pDescribeCol == NULL)
		return FALSE;
	return pDescribeCol->bBindBookmark;
}

// -----------------------------------------------------------------------------------------
SWORD CSql::CanMultiOperator()
{
	SQLRETURN rc;
	SQLUINTEGER fPos;
	SWORD cbValue;

	if (!IsConnected())
		return 0;

	rc = SQLGetInfo(m_hdbc, SQL_ACTIVE_STATEMENTS, &fPos, sizeof (fPos), &cbValue);

	if (RETCODE_IS_SUCCESSFUL(rc))
		return cbValue;
	
	return 0;
}

// -----------------------------------------------------------------------------------------
BOOL CSql::GetQuoteIdentifier(SQLCHAR *pStr, SQLSMALLINT BufferLength)
{
	//SQL_ODBC_VER - string
	//SQL_POSITIONED_STATEMENTS - bit mask (SQL_PS_POSITIONED_UPDATE|SQL_PS_POSITIONED_DELETE|SQL_PS_SELECT_FOR_UPDATE)
	if (m_hdbc == SQL_NULL_HDBC)
		return FALSE;

	SQLSMALLINT StringLengthPtr;
	SQLRETURN  rc = SQLGetInfo(m_hdbc,  SQL_IDENTIFIER_QUOTE_CHAR, (SQLPOINTER) pStr, BufferLength, &StringLengthPtr);
	return RETCODE_IS_SUCCESSFUL(rc);
}

// -----------------------------------------------------------------------------------------
BOOL CSql::DoQueryA(const CHAR* query, BOOL ext, BOOL columninfo, BOOL keyinfo, BOOL i_bAllocateRetInt)
{
	CONVERT_TO_WCHAR(query, strQueryW)
	return DoQueryW(strQueryW, ext, columninfo, keyinfo, i_bAllocateRetInt);
}

BOOL CSql::DoQueryW(const WCHAR* query, BOOL ext, BOOL columninfo, BOOL keyinfo, BOOL i_bAllocateRetInt)
{
	ext;
	columninfo; 
	keyinfo;
	return DoBindQueryW(query, 0, NULL, NULL, NULL, i_bAllocateRetInt);
}

// -----------------------------------------------------------------------------------------
BOOL CSql::DoSqlQueryIn(const CHAR* query, BOOL i_bAllocateRetInt)
{						   
	return DoBindQueryA(query, 0, NULL, NULL, NULL, i_bAllocateRetInt);
}

// -----------------------------------------------------------------------------------------
BOOL CSql::DoBindQueryExA	(const CHAR* query, CSqlRecordset& i_oParams, BOOL i_bAllocateRetInt)
{
	CONVERT_TO_WCHAR(query, strQueryW);
	return DoBindQueryExW(strQueryW, i_oParams, i_bAllocateRetInt);
}

BOOL CSql::DoBindQueryExW	(const WCHAR* query, CSqlRecordset& i_oParams, BOOL i_bAllocateRetInt)
{
	// find out parameters count
	if(!query)
		return FALSE;

	// remove zero-length parameters
	//WCHAR strSql[4000];

	int nCount = 0;
	for(int i=lstrlenW(query); i>0; i--)
		if(L'?' == query[i-1])
			++ nCount;

	return DoBindQueryW(query, (SWORD) min(i_oParams.GetSize(), 
		WORD(nCount)), RecordsetBind, RecordsetPutData, (void*) &i_oParams, i_bAllocateRetInt);
}

// -----------------------------------------------------------------------------------------
BOOL CSql::DoBindQueryA(const CHAR* query, SWORD ParamCount, 
	BINDPROC pBindProc, PUTDATA pPutData, void* UserValue, BOOL i_bAllocateRetInt)
{
	CONVERT_TO_WCHAR(query, strQueryW)
	return DoBindQueryW(strQueryW, ParamCount, pBindProc, pPutData, UserValue, i_bAllocateRetInt);
}

// -----------------------------------------------------------------------------------------
BOOL CSql::DoBindQueryW(const WCHAR* query, SWORD ParamCount, 
	BINDPROC pBindProc, PUTDATA pPutData, void* UserValue, BOOL i_bAllocateRetInt)
{

#if TRACE_QUERY
#ifdef _DEBUG
#ifdef _UNICODE
	const WCHAR* wquery=query;
#else
	CONVERT_TO_CHAR(query, wquery);
#endif
	KL_TRACE(wquery);
#endif
#endif
	if (m_bBusy)
		return FALSE;

	lstrcpyW(m_strCmd, query);

	DropData();

	if(!AllocStmtHandle())
		return FALSE;

	m_nProcRetInt = 0xFFFFFFFF;

	SQLRETURN rc;
	BOOL bBindOutput = ParamCount>0 && NULL != pBindProc && NULL != pPutData;
	if(FALSE == bBindOutput)
		ParamCount = 0;

	if(i_bAllocateRetInt)
		++ParamCount;

	SWORD ParamID;
	for (ParamID = 1; ParamID <= ParamCount; ParamID++)
	{
		SWORD		LocalType;
		SWORD		SqlType;
		SDWORD		ValueSize = 0;
		SDWORD		cbSize;
		SQLSMALLINT nInOut = SQL_PARAM_INPUT;
		SQLPOINTER	pParamValPtr;

		if(ParamID == 1 && TRUE == i_bAllocateRetInt)
		{
			nInOut		= SQL_PARAM_OUTPUT;	
			LocalType	= SQL_C_SLONG;
			SqlType		= SQL_INTEGER;
			ValueSize	= sizeof(m_nProcRetInt);
			pParamValPtr= &m_nProcRetInt;
			cbSize		= 0;
		}
		else	
		{
			pBindProc(UserValue, 
				i_bAllocateRetInt ? (ParamID-1) : ParamID, 
				&LocalType, &SqlType, &ValueSize);

			pParamValPtr = (SQLPOINTER) ParamID;
			if (ValueSize == 0)
				ValueSize = 1;
			cbSize = SQL_LEN_DATA_AT_EXEC(ValueSize);
		}

		rc = SQLBindParameter(
			m_hstmt,
			ParamID,				// ipar
			nInOut,					// fParamType
			LocalType,              // fParamType
			SqlType,                // FSqlType
			ValueSize,              // cbColDef
			0,                      // ibScale
			pParamValPtr,			// 
			0,                      // cbValueMax
			(SQLLEN *)&cbSize);        

		if (RETCODE_IS_FAILURE(rc))
			return FALSE;
	}

	rc = SQLExecDirectW(m_hstmt, (SQLWCHAR*)(query), SQL_NTS);

    if (RETCODE_IS_FAILURE(rc))
   		return FALSE;
		
	if(rc == SQL_NEED_DATA && FALSE == bBindOutput)		
		return FALSE;

	if(bBindOutput)
	{
		PTR pParmID;
		rc = SQLParamData(m_hstmt, &pParmID);
		while (rc == SQL_NEED_DATA)
		{
			ParamID = (SWORD) pParmID;
			if(i_bAllocateRetInt)
				--ParamID;

			PVOID Buffer;
			SDWORD BufferSize;
			pPutData(UserValue, ParamID, &Buffer, &BufferSize);
			rc = SQLPutData(m_hstmt, Buffer, BufferSize);

			if(RETCODE_IS_FAILURE(rc))
				break;
			rc = SQLParamData(m_hstmt, &pParmID);
		}

		if (RETCODE_IS_FAILURE(rc))
			return FALSE;
	}

#ifdef _DEBUG
	if (rc == SQL_SUCCESS_WITH_INFO)
		TraceDiagRecords();
#endif

	SWORD dwCols = GetTotalColumns();
	if (0 < dwCols)
	{
		m_bDataReady = TRUE;
		UpdateColInfo(FALSE);
	}
	else
	if(-1 == dwCols)
		return FALSE;

	return TRUE;
}

// -----------------------------------------------------------------------------------------
BOOL CSql::BulkOperations(SQLUSMALLINT Operation)
{
	/*
	SQL_ADD
	SQL_UPDATE_BY_BOOKMARK
	SQL_DELETE_BY_BOOKMARK
	SQL_FETCH_BY_BOOKMARK
	*/

	SQLRETURN rc;
	rc = SQLBulkOperations(m_hstmt, Operation);
	return RETCODE_IS_SUCCESSFUL(rc);
}

// -----------------------------------------------------------------------------------------
SDWORD CSql::GetCurrentRow()
{
	SQLRETURN rc;
	SDWORD sdw = -1;
	if (pDescribeCol != NULL)
		rc = SQLGetStmtOption(m_hstmt, SQL_ROW_NUMBER, (PTR)&sdw);
	return sdw;
}

// -----------------------------------------------------------------------------------------
BOOL CSql::NextData()
{
	SQLRETURN rc;
	if (pDescribeCol != NULL)
	{
		rc = SQLFetch(m_hstmt);
		
		if (RETCODE_IS_FAILURE(rc))
			return FALSE;

		if (rc != SQL_NO_DATA_FOUND)
			return TRUE;
	}
	return FALSE;
}

// -----------------------------------------------------------------------------------------
BOOL CSql::NextDataEx(BOOKMARK* lpBookmark, SDWORD Offset, WORD MoveType /*= SQL_FETCH_NEXT*/)
{
	SQLRETURN rc;
	BOOL bRow = FALSE;
	BOOL bNext = TRUE;

	if (pDescribeCol != NULL)
	{
		while (bNext)
		{
			bRow = FALSE;
			bNext = FALSE;
//			rc = SQLExtendedFetch(m_hstmt, MoveType, Offset, &crow, &rgfRowStatus);
			if (lpBookmark != NULL)
				SetBookmarkValue(*lpBookmark);
			rc = SQLFetchScroll(m_hstmt, MoveType, 1);
			if (RETCODE_IS_FAILURE(rc))
				return FALSE;
			if (rc != SQL_NO_DATA_FOUND)
			{
				bRow = TRUE;
				if (m_RowStatus == SQL_ROW_DELETED)
				{
					*lpBookmark = -1;
					if (!m_bShowDeleted)
					{
						bNext = TRUE;
						bRow = FALSE;
					}
				}
				else
				{
					if (lpBookmark != NULL)
					{
						rc = SQLGetStmtOption(m_hstmt, SQL_GET_BOOKMARK, (PTR)lpBookmark);
						if (RETCODE_IS_FAILURE(rc))
							return FALSE;
					}

					if (m_RowStatus == SQL_ROW_ERROR)
					{
						KL_TRACE(_T("row error..."));
						bRow = FALSE;
					}
				}
			}
		}
	}
	return bRow;
}

// -----------------------------------------------------------------------------------------
SDWORD CSql::GetRowsCount()
{
	SDWORD rowcount;
	SQLRETURN rc;
	rc = SQLRowCount(m_hstmt, (SQLLEN *)&rowcount);
	if (RETCODE_IS_SUCCESSFUL(rc))
		return rowcount;
	return -1;
}

// -----------------------------------------------------------------------------------------
WORD CSql::GetColumnsCount()
{
	if (pDescribeCol != NULL)
		return pDescribeCol[0].iTotalCol;
	return 0;
}

// -----------------------------------------------------------------------------------------
TCHAR* CSql::GetColumnName(WORD col)
{
	if (pDescribeCol != NULL)
		if (pDescribeCol[0].iTotalCol > col)
			return (TCHAR*) pDescribeCol[col].szColumnLabel;
	return NULL;
}

// -----------------------------------------------------------------------------------------
SWORD CSql::GetColumnType(WORD col)
{
	SWORD type = SQL_UNKNOWN_TYPE;
	if (pDescribeCol != NULL)
		if (pDescribeCol[0].iTotalCol > col)
			type = pDescribeCol[col].fSqlType;
	return type;	
}

// -----------------------------------------------------------------------------------------
void CSql::SetBookmarkValue(BOOKMARK Boomark)
{
	if (pDescribeCol != NULL)
	{
		if (pDescribeCol->bBindBookmark)
		{
			SFieldValue dbvar;
			if (GetFieldValue(&dbvar, 0))
				*dbvar.Value.plVal = Boomark;
		}
	}
	m_BookMark = Boomark;
}

// -----------------------------------------------------------------------------------------
BOOL CSql::GetFieldValue(SFieldValue* pvar, SQLUINTEGER col)
{
	SQLRETURN nRetVal = GetFieldValue2(pvar, col);
	return RETCODE_IS_SUCCESS(nRetVal);
}

SQLRETURN CSql::GetFieldValue2(SFieldValue* pvar, SQLUINTEGER col)
{
	if (NULL == pvar || NULL == pBoundCol)
		return SQL_INVALID_HANDLE;

	pvar->vt = SFieldValue::VT_NULL;
	pvar->FieldSize = pBoundCol[col].BufferSize;
	pvar->pOriginalBuffer = (PVOID) &(pBoundCol[col].pBuffer);
	pvar->pcbValue = &(pBoundCol[col].cbValue);

	switch (pBoundCol[col].fSqlType)
	{
	case SQL_C_SLONG:
		pvar->vt = SFieldValue::VT_I4;
		pvar->Value.plVal = (long*)(pBoundCol[col].pBuffer);
		break;
	case SQL_C_CHAR:
		pvar->vt = SFieldValue::VT_CREF;
		pvar->Value.pcVal = (CHAR*)(pBoundCol[col].pBuffer);
		break;
	case SQL_C_WCHAR:
		pvar->vt = SFieldValue::VT_WCREF;
		pvar->Value.pwcref = (WCHAR*)(pBoundCol[col].pBuffer);
		break;
	case SQL_C_DOUBLE:
		pvar->vt = SFieldValue::VT_R8;
		pvar->Value.pdblVal = (double*)(pBoundCol[col].pBuffer);
		break;
	case SQL_C_SSHORT:
		pvar->vt = SFieldValue::VT_I2;
		pvar->Value.piVal = (short*)(pBoundCol[col].pBuffer);
		break;
	case SQL_C_BINARY:
//	case SQL_C_VARBOOKMARK: is similar to SQL_C_BINARY
		if (m_ulImageSize == 0)
		{
			SQLRETURN sqlret = 
				SQLGetData(m_hstmt, 
				(SQLUSMALLINT) (col + 1), 
				SQL_C_BINARY, 
				pvar->pOriginalBuffer, 
				0, 
				(SQLLEN *)pvar->pcbValue);

			if (SQL_NULL_DATA != *pvar->pcbValue &&	RETCODE_IS_SUCCESS(sqlret))
			{
				if (NULL == pBoundCol[col].pBuffer ||
					pBoundCol[col].BufferSize < *pvar->pcbValue)
				{
					if (pBoundCol[col].pBuffer != NULL)
						_SQL_MM_Free(pBoundCol[col].pBuffer);
					pvar->FieldSize = *pvar->pcbValue;
					pBoundCol[col].BufferSize = pvar->FieldSize;
					pBoundCol[col].pBuffer = _SQL_MM_Alloc(pBoundCol[col].cbValue + 1);
					((BYTE*)pBoundCol[col].pBuffer)[pBoundCol[col].cbValue] = 0;
				}
				else
					ZeroMemory(pBoundCol[col].pBuffer, pBoundCol[col].BufferSize);
				pvar->vt = SFieldValue::VT_BYREF;
			}
			else
				if(RETCODE_IS_FAILURE(sqlret))
					return sqlret;
		}
		pvar->Value.pVal = pBoundCol[col].pBuffer;
		break;
	case SQL_C_BIT:
		pvar->vt = SFieldValue::VT_BOOL;
		pvar->Value.pboolVal = (BYTE*)(pBoundCol[col].pBuffer);
		break;
	case SQL_C_UTINYINT:
		pvar->vt = SFieldValue::VT_UI2;
		pvar->Value.puiVal = (unsigned short*)(pBoundCol[col].pBuffer);
		break;
	case SQL_C_TYPE_TIMESTAMP:
		pvar->vt = SFieldValue::VT_TIMESTAMP;
		pvar->Value.pTimeStamp = (TIMESTAMP_STRUCT*)(pBoundCol[col].pBuffer);
		break;
	case SQL_C_TYPE_DATE:
		pvar->vt = SFieldValue::VT_DATE;
		pvar->Value.pDate = (DATE_STRUCT*)(pBoundCol[col].pBuffer);
		break;
	case SQL_C_TYPE_TIME:
		pvar->vt = SFieldValue::VT_TIME;
		pvar->Value.pTime = (TIME_STRUCT*)(pBoundCol[col].pBuffer);
		break;
	case SQL_C_FLOAT:
		pvar->vt = SFieldValue::VT_R4;
		pvar->Value.pfltVal = (float*)(pBoundCol[col].pBuffer);
		break;
	case SQL_C_DEFAULT:
	default:
		KL_TRACE(_T("Encountered not defined column!!!"));
		break;
	}

	if (pBoundCol[col].pBuffer != NULL)
	{
		SQLRETURN sqlret = SQLGetData(m_hstmt, 
				(SQLUSMALLINT) (col + 1), 
				pBoundCol[col].fSqlType, 
				pBoundCol[col].pBuffer, 
				pvar->FieldSize, 
				(SQLLEN *)pvar->pcbValue);

		if(	!RETCODE_IS_SUCCESS(sqlret) && sqlret != SQL_NO_DATA )
			return sqlret;
	}
	
	if (pBoundCol[col].cbValue == SQL_NULL_DATA)
		pvar->vt = SFieldValue::VT_NULL;

	if (pBoundCol[col].cbValue == SQL_NO_TOTAL)
	{
		pvar->vt = SFieldValue::VT_NULL;
		KL_TRACE(_T("_Sql: no total in result\n"));
	}

	return SQL_SUCCESS;
}

// -----------------------------------------------------------------------------------------
// returns (NO_DATA, DATA, error)
SQLRETURN CSql::Fetch(CSqlRecordset& o_oRecordset, eFetchOrientation i_eFetchOrientation, int i_nOffset)
{
	o_oRecordset.m_pSql = NULL;

	SQLRETURN nRetVal =
		SQLFetchScroll(m_hstmt, inlGetODBCFetchOrientationId(i_eFetchOrientation), i_nOffset);

	if(SQL_ERROR == nRetVal)
	{
		TCHAR strState	[6];
		int nNativeError;

		if(GetDiagRec(TRUE, 1, strState, nNativeError, NULL, 0) &&
			0 == lstrcmp(STATE_INVALID_CURSOR, strState))
			nRetVal = SQL_NO_DATA;
	}

	if(!RETCODE_IS_SUCCESS(nRetVal))      
		o_oRecordset.Allocate(0);
	else
	{
		int nFields = GetColumnsCount();
		o_oRecordset.Allocate(nFields);

		for(int i=0; i<nFields; i++)			
		{
			nRetVal = GetFieldValue(o_oRecordset[i], i);
			if(!RETCODE_IS_SUCCESS(nRetVal))
				return nRetVal;
		}

		o_oRecordset.m_pSql = this;
	}

	return nRetVal;
}

// -----------------------------------------------------------------------------------------
BOOL CSql::MoveNext(CSqlRecordset& o_oRecordset)
{
	SQLRETURN			nRetVal;
	BOOL bSuccess		= TRUE;
	m_bHasInfoRecords	= FALSE;

	for(;;)
	{
		nRetVal = SQLFetch(m_hstmt);

		if(SQL_ERROR == nRetVal)
		{
			TCHAR strState	[6];
			int nNativeError;

			if(GetDiagRec(TRUE, 1, strState, nNativeError, NULL, 0) &&
				0 == lstrcmp(STATE_INVALID_CURSOR, strState))
				nRetVal = SQL_NO_DATA;
		}

		if(SQL_SUCCESS == nRetVal)
			break;
		else
		if(SQL_SUCCESS_WITH_INFO == nRetVal)
		{
			m_bHasInfoRecords = TRUE;
			break;
		}
		else
		if(SQL_ERROR == nRetVal)
		{
			bSuccess = FALSE;
			m_bHasInfoRecords = TRUE;
			break;
		}
		else
		if(SQL_INVALID_HANDLE == nRetVal)
		{
			bSuccess = FALSE;
			break;
		}
		else
		if(SQL_NO_DATA == nRetVal)
		{
			// call SQLMoreResults
			nRetVal = SQLMoreResults(m_hstmt);

			if(SQL_SUCCESS == nRetVal)
				++m_nResultsetNmb;
			else
			if(SQL_SUCCESS_WITH_INFO == nRetVal)
			{
				++m_nResultsetNmb;
				m_bHasInfoRecords = TRUE;
			}
			else
			if(SQL_ERROR == nRetVal)
			{
				bSuccess = FALSE;
				m_bHasInfoRecords = TRUE;
				break;
			}
			else
			if(SQL_INVALID_HANDLE == nRetVal)
			{
				bSuccess = FALSE;
				break;
			}
			else
			if(SQL_NO_DATA == nRetVal)
				break;

			if (SQL_SUCCESS == nRetVal || SQL_SUCCESS_WITH_INFO == nRetVal )
			{
				BOOL bBindBookmark = FALSE; 
				if (pDescribeCol != NULL)
					bBindBookmark = pDescribeCol->bBindBookmark;

				UpdateColInfo(bBindBookmark);
			}
		}
	}
				
	int nFields = (bSuccess && (SQL_NO_DATA != nRetVal)) ? GetColumnsCount() : 0;
									  
	// allocate even no fields set - thus we clean o_oRecordset
	o_oRecordset.Allocate(nFields);

	if(bSuccess)      
	{
		for(int i=0; i<nFields && bSuccess; i++)			
			if(!GetFieldValue(o_oRecordset[i], i))
				bSuccess = FALSE;
	}

	o_oRecordset.m_pSql = bSuccess ? this : NULL;

	if(!bSuccess)
		o_oRecordset.Allocate(0);
	
	return bSuccess && (SQL_NO_DATA != nRetVal);
}

// -----------------------------------------------------------------------------------------
void CSql::RecordsetBind(void* UserValue, SWORD ParamID, SWORD* pLocalType, SWORD* pSqlType, SDWORD* pValueSize)
{
	KL_ASSERT(UserValue != NULL);
	CSqlRecordset& rRS = *(CSqlRecordset*) UserValue;

	SFieldValue* pFldVal = rRS[ParamID - 1];

	if(!pFldVal)
	{
		KL_TRACE(_T("Invalid field value ..."));
		return;
	}

	pFldVal->GetFldTypes(*pLocalType, *pSqlType);
	*pValueSize = pFldVal->GetFldBufferSize();
}

// -----------------------------------------------------------------------------------------
void CSql::RecordsetPutData(void* UserValue, SWORD ParamID, PVOID* Buffer, SDWORD* pBufferSize)
{									  
	KL_ASSERT(UserValue != NULL);
	CSqlRecordset& rRS = *(CSqlRecordset*) UserValue;

	SFieldValue* pFldVal = rRS[ParamID - 1];

	if(!pFldVal)
	{
		KL_TRACE(_T("Invalid field value ..."));
		return;
	}

	*Buffer		 = pFldVal->GetFldBuffer();
	*pBufferSize = pFldVal->GetFldBufferSize();
}

// -----------------------------------------------------------------------------------------
int	CSql::GetColumnNmb	(TCHAR* i_strColumnName)
{
	for(int i = GetColumnsCount(); i>0; i--)
		if(0 == lstrcmp(GetColumnName(i-1), i_strColumnName))	
			return (i-1);

	return -1;
}

// -----------------------------------------------------------------------------------------
void CSql::Clean()
{
	// binding first - descriptor has info about binding
	// so desriptor - the next step to free
	FreeColumnBinding();
	FreeColumnDescriptor();
	m_nResultsetNmb = 0;
}

// -----------------------------------------------------------------------------------------
void CSql::FreeColumnDescriptor()
{
	if (pDescribeCol)
	{
		_SQL_MM_Free(pDescribeCol);
		pDescribeCol = NULL;
	}
}

// -----------------------------------------------------------------------------------------
void CSql::FreeColumnBinding()
{
	if (pBoundCol)
	{
		// if there is a description - free fields buffers
		if(pDescribeCol)
		for (int cou = 0 ; cou < pDescribeCol[0].iTotalCol; cou++)
			_SQL_MM_Free(pBoundCol[cou].pBuffer);

		_SQL_MM_Free(pBoundCol);
		pBoundCol = NULL;
	}
}

// -----------------------------------------------------------------------------------------
BOOL CSql::AllocStmtHandle()
{
	if(SQL_NULL_HSTMT != m_hstmt)
		return FALSE;

	SQLRETURN nRetVal = SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &m_hstmt);

	if (RETCODE_IS_FAILURE(nRetVal))
		return FALSE;

	if(m_eConcurrency)
	{
		SQLPOINTER pConcur = inlGetODBCConcurrencyId(m_eConcurrency);
		nRetVal = SQLSetStmtAttr(m_hstmt, SQL_ATTR_CONCURRENCY, pConcur, 0);
		if(!RETCODE_IS_SUCCESS(nRetVal))
			return FALSE;
	}

	if(m_eCursorType)
	{
		SQLPOINTER pCursorType = inlGetODBCCursotTypeId(m_eCursorType);
		nRetVal = SQLSetStmtAttr(m_hstmt, SQL_ATTR_CURSOR_TYPE, pCursorType, 0);
		if(!RETCODE_IS_SUCCESS(nRetVal))
			return FALSE;
	}

	if(m_eScrollability)
	{
		SQLPOINTER pScrollability = inlGetODBCScrollabilityId(m_eScrollability);
		nRetVal = SQLSetStmtAttr(m_hstmt, SQL_ATTR_CURSOR_SCROLLABLE, pScrollability, 0);
		if(!RETCODE_IS_SUCCESS(nRetVal))
			return FALSE;
	}

	if(m_eCursorSensitivity)
	{
		SQLPOINTER pCursorSensitivity = inlGetODBCCursorSensitivityId(m_eCursorSensitivity);
		nRetVal = SQLSetStmtAttr(m_hstmt, SQL_ATTR_CURSOR_SENSITIVITY, pCursorSensitivity, 0);
		if(!RETCODE_IS_SUCCESS(nRetVal))
			return FALSE;
	}

	return TRUE;
}

// -----------------------------------------------------------------------------------------
SFieldValue::SFieldValue()
{
	Init();
}

void SFieldValue::Init()
{
	ZeroMemory(this, sizeof(SFieldValue));
}

// -----------------------------------------------------------------------------------------
DWORD SFieldValue::GetStringPresentationA(CHAR* o_strBuf, DWORD i_dwBufLen)
{
	if(!o_strBuf || 0==i_dwBufLen)
		return 0;

	WCHAR* strBuf = (WCHAR*)_alloca(sizeof(WCHAR) * (i_dwBufLen + 1));
	DWORD dwRet = GetStringPresentationW(strBuf, i_dwBufLen);
	W2A(strBuf, o_strBuf, i_dwBufLen);

	return dwRet;
}

// -----------------------------------------------------------------------------------------
// attention!!! this map is obviously incomplete! 
DWORD SFieldValue::GetStringPresentationW(WCHAR* o_strBuf, DWORD i_dwBufLen)
{
	if(!o_strBuf || 0==i_dwBufLen)
		return 0;

	o_strBuf[0] = 0;

	switch(vt)
	{
	case VT_NULL: 
		return 0;		

	case VT_CREF:	A2W(Value.pcVal, o_strBuf, i_dwBufLen);			break;		
	case VT_WCREF:  lstrcpynW(o_strBuf, Value.pwcref, i_dwBufLen);	break;		
					/*
	case VT_BOOL:	wnsprintfW(o_strBuf, i_dwBufLen, L"%d", int(*Value.pboolVal));break;
	case VT_UI2:	wnsprintfW(o_strBuf, i_dwBufLen, L"%d", int(*Value.pbVal));	break;
	case VT_I2:		wnsprintfW(o_strBuf, i_dwBufLen, L"%d", int(*Value.piVal));	break;
	case VT_I4:		wnsprintfW(o_strBuf, i_dwBufLen, L"%d", int(*Value.plVal));	break;
	case VT_R4:	    wnsprintfW(o_strBuf, i_dwBufLen, L"%f", *Value.pfltVal);	break;
	case VT_R8:		wnsprintfW(o_strBuf, i_dwBufLen, L"%lf", *Value.pdblVal);	break;
	case VT_UI4:  	wnsprintfW(o_strBuf, i_dwBufLen, L"%d", int(*Value.pulVal));break;
	case VT_INT:	wnsprintfW(o_strBuf, i_dwBufLen, L"%d", int(*Value.pintVal));break;
	case VT_UINT:	wnsprintfW(o_strBuf, i_dwBufLen, L"%l", Value.pintVal);		break;
					*/

	case VT_BOOL:	wsprintfW(o_strBuf, L"%d", int(*Value.pboolVal));break;
	case VT_UI2:	wsprintfW(o_strBuf, L"%d", int(*Value.pbVal));	break;
	case VT_I2:		wsprintfW(o_strBuf, L"%d", int(*Value.piVal));	break;
	case VT_I4:		wsprintfW(o_strBuf, L"%d", int(*Value.plVal));	break;
	case VT_R4:	    wsprintfW(o_strBuf, L"%f", *Value.pfltVal);	break;
	case VT_R8:		wsprintfW(o_strBuf, L"%lf", *Value.pdblVal);	break;
	case VT_UI4:  	wsprintfW(o_strBuf, L"%d", int(*Value.pulVal));break;
	case VT_INT:	wsprintfW(o_strBuf, L"%d", int(*Value.pintVal));break;
	case VT_UINT:	wsprintfW(o_strBuf, L"%l", Value.pintVal);		break;
		
	case VT_TIMESTAMP:
					wsprintfW(o_strBuf, L"%d.%d.%d, %dh:%dm", 
						Value.pTimeStamp->day, 
						Value.pTimeStamp->month,
						Value.pTimeStamp->year,
						Value.pTimeStamp->hour,
						Value.pTimeStamp->minute);
		break;		
	
	case VT_BYREF:  
	case VT_TIME:	
	case VT_DATE:	
	case VT_UI1:  
	case VT_I1:  
		KL_TRACE(_T("NOT DEFINED TYPE!"));
		return 0;
	break;
	}

	return lstrlenW(o_strBuf);
}

// -----------------------------------------------------------------------------------------
// attention!!! this map is probably incomplete! 
BOOL SFieldValue::GetFldTypes	(SWORD& o_nLocalType, SWORD& o_nSQLType)
{
	o_nLocalType = 0;
	o_nSQLType   = 0;

	switch(vt)
	{
	case VT_NULL: 
		KL_TRACE(_T("NULL field passed!"));
		return FALSE;		

	case VT_BOOL:	o_nLocalType = SQL_C_BIT; 		o_nSQLType = SQL_BIT;		break;		
	case VT_I2:		o_nLocalType = SQL_C_SSHORT;	o_nSQLType = SQL_SMALLINT;	break;		
	case VT_I4:		o_nLocalType = SQL_C_SLONG;		o_nSQLType = SQL_INTEGER;	break;		
	case VT_R8:		o_nLocalType = SQL_C_DOUBLE;	o_nSQLType = SQL_DECIMAL;	break;		
	case VT_DATE:	o_nLocalType = SQL_C_TYPE_DATE; o_nSQLType = SQL_DATE;		break;		
	case VT_CREF:	o_nLocalType = SQL_C_CHAR;		o_nSQLType = SQL_VARCHAR;	break;		
	case VT_TEXT:	o_nLocalType = SQL_C_CHAR;		o_nSQLType = SQL_LONGVARCHAR; break;		
	case VT_WTEXT:	o_nLocalType = SQL_C_WCHAR;		o_nSQLType = SQL_WLONGVARCHAR; break;		
	case VT_UI2:	o_nLocalType = SQL_C_UTINYINT;	o_nSQLType = SQL_TINYINT;	break;		
	case VT_BYREF:  o_nLocalType = SQL_C_BINARY; 	o_nSQLType = SQL_LONGVARBINARY;  break;		
	case VT_TIMESTAMP:
					o_nLocalType = SQL_C_TIMESTAMP; o_nSQLType = SQL_TIMESTAMP;  break;		
	case VT_TIME:	o_nLocalType = SQL_C_TIME;		o_nSQLType = SQL_TIME;		break;		
	case VT_WCREF:  o_nLocalType = SQL_C_WCHAR;		o_nSQLType = SQL_WVARCHAR;  break;		

	case VT_UI1:  
	case VT_I1:  
	case VT_R4:  
	case VT_UI4:  	
	case VT_INT:  
	case VT_UINT: 
		KL_TRACE(_T("NOT DEFINED TYPE!"));
		return FALSE;
	break;
	}

	return TRUE;
}

// -----------------------------------------------------------------------------------------
SDWORD SFieldValue::GetFldBufferSize()
{
	return SDWORD(FieldSize);
}

void*	SFieldValue::GetFldBuffer()
{
	return Value.pVal;
}

// -----------------------------------------------------------------------------------------
void	SFieldValue::SetRef(TIMESTAMP_STRUCT& i_oTS)
{
	vt = VT_TIMESTAMP;
	Value.pTimeStamp = &i_oTS;
	FieldSize = sizeof(TIMESTAMP_STRUCT);
}

void	SFieldValue::SetRefVarBin(BYTE* i_pByte, DWORD i_dwLen)
{
	vt = VT_BYREF;
	Value.pVal = i_pByte;
	FieldSize = i_pByte ? i_dwLen : 0;
}

void	SFieldValue::SetRefText	(CHAR* i_str)
{
	vt = VT_TEXT;
	Value.pcVal = i_str;			   
	FieldSize = lstrlenA(i_str);
}

void	SFieldValue::SetRefTextW	(WCHAR* i_str)
{
	vt = VT_WTEXT;
	Value.pwcref = i_str;
	FieldSize = sizeof(WCHAR)*lstrlenW(i_str);
}

// -----------------------------------------------------------------------------------------
CSqlRecordset::CSqlRecordset(DWORD i_dwSize)
{
	m_pArray = NULL;
	m_pSql = NULL;
	m_dwArrSize = 0;
	Allocate(i_dwSize);
}

CSqlRecordset::~CSqlRecordset()
{
	Clear();
}

void CSqlRecordset::Allocate(DWORD i_dwSize)
{
	if(GetSize() == i_dwSize)
		return; 

	Clear();

	m_dwArrSize = i_dwSize;

	if(m_dwArrSize)
	{
		m_pArray = (SFieldValue*) new SFieldValue[m_dwArrSize];
		InitFields();
	}
}

void CSqlRecordset::InitFields	()
{
	KL_ASSERT((GetSize()>0  && NULL != m_pArray) ||
			  (GetSize()==0 && NULL == m_pArray));

	for(DWORD i = 0; i<GetSize(); i++)
		m_pArray[i].Init();
}

void CSqlRecordset::Clear()
{
	if(m_pArray) delete [] m_pArray;
	m_pArray = NULL;
	m_dwArrSize = 0;
}

DWORD CSqlRecordset::GetSize()
{
	return m_dwArrSize;
}

SFieldValue* CSqlRecordset::operator[](DWORD i_dwFldIdx)
{
	if(GetSize() <= i_dwFldIdx)
	{
		KL_TRACE(_T("Invalid field index supplied!"));
		return FALSE;
	}

	KL_ASSERT(NULL != m_pArray);

	return m_pArray + i_dwFldIdx; 
}

SFieldValue* CSqlRecordset::operator[](TCHAR* i_pstrFldName)
{
	if( NULL == m_pSql)
	{
		KL_TRACE(_T("Call without MoveNext prior call is invalid!"));
		return NULL;
	}

	int nFldNmb = m_pSql->GetColumnNmb(i_pstrFldName);

	if(-1 == nFldNmb)
	{
		KL_TRACE(_T("Invalid field name supplied!"));
		return NULL;
	}

	return (*this)[DWORD(nFldNmb)];
}

// -----------------------------------------------------------------------------------------
/*
// -----------------------------------------------------------------------------------------
void CSql::ProcessLogMessages(SQLSMALLINT plm_handle_type, SQLHANDLE plm_handle, TCHAR *logstring, int ConnInd)
{
    RETCODE		plm_retcode = SQL_SUCCESS;
    TCHAR		plm_szSqlState[_ERROR_BUF] = _T(""), plm_szErrorMsg[_ERROR_BUF] = _T("");
    SDWORD		plm_pfNativeError = 0L;
    SWORD		plm_pcbErrorMsg = 0;
    SQLSMALLINT	plm_cRecNmbr = 1;
    SDWORD		plm_SS_MsgState = 0, plm_SS_Severity = 0;
    SQLINTEGER	plm_Rownumber = 0;
    USHORT		plm_SS_Line;
    SQLSMALLINT	plm_cbSS_Procname, plm_cbSS_Srvname;
    SQLCHAR		plm_SS_Procname[MAXNAME], plm_SS_Srvname[MAXNAME];

    while ((plm_retcode != SQL_NO_DATA_FOUND) && (plm_retcode == SQL_SUCCESS))
	{
        plm_retcode = SQLGetDiagRec(plm_handle_type, plm_handle, plm_cRecNmbr, 
				(SQLTCHAR*)plm_szSqlState, &plm_pfNativeError, (SQLTCHAR*)plm_szErrorMsg, _ERROR_BUF - 1, &plm_pcbErrorMsg);
        if (plm_retcode != SQL_NO_DATA_FOUND)
		{
			KL_TRACE(logstring);

            if (ConnInd)
			{
                plm_retcode = SQLGetDiagField(plm_handle_type, plm_handle, plm_cRecNmbr, SQL_DIAG_ROW_NUMBER, &plm_Rownumber, SQL_IS_INTEGER, NULL);
                plm_retcode = SQLGetDiagField(plm_handle_type, plm_handle, plm_cRecNmbr, SQL_DIAG_SS_LINE, &plm_SS_Line, SQL_IS_INTEGER, NULL);
                plm_retcode = SQLGetDiagField(plm_handle_type, plm_handle, plm_cRecNmbr, SQL_DIAG_SS_MSGSTATE, &plm_SS_MsgState, SQL_IS_INTEGER, NULL);
                plm_retcode = SQLGetDiagField(plm_handle_type, plm_handle, plm_cRecNmbr, SQL_DIAG_SS_SEVERITY, &plm_SS_Severity, SQL_IS_INTEGER, NULL);
                plm_retcode = SQLGetDiagField(plm_handle_type, plm_handle, plm_cRecNmbr, SQL_DIAG_SS_PROCNAME, &plm_SS_Procname, sizeof(plm_SS_Procname), &plm_cbSS_Procname);
                plm_retcode = SQLGetDiagField(plm_handle_type, plm_handle, plm_cRecNmbr, SQL_DIAG_SS_SRVNAME, &plm_SS_Srvname, sizeof(plm_SS_Srvname), &plm_cbSS_Srvname);
				
            }

			if(plm_cRecNmbr==1)
			{
				lstrcpy(szErrorMsg, plm_szErrorMsg);
				lstrcpy(szSqlState, plm_szSqlState);
			}

            KL_TRACE1(_T("szSqlState = %s"),plm_szSqlState);
            KL_TRACE1(_T("pfNativeError = %d"),plm_pfNativeError);
            KL_TRACE1(_T("szErrorMsg = %s"),szErrorMsg);
            KL_TRACE1(_T("pcbErrorMsg = %d"),plm_pcbErrorMsg);

            if (ConnInd)
			{
                KL_TRACE1(_T("ODBCRowNumber = %d"), plm_Rownumber);
                KL_TRACE1(_T("SSrvrLine = %d"), plm_Rownumber);
                KL_TRACE1(_T("SSrvrMsgState = %d"),plm_SS_MsgState);
                KL_TRACE1(_T("SSrvrSeverity = %d"),plm_SS_Severity);
                KL_TRACE1(_T("SSrvrProcname = %s"),plm_SS_Procname);
                KL_TRACE1(_T("SSrvrSrvname = %s"),plm_SS_Srvname);
            }
        }
        plm_cRecNmbr++; //Increment to next diagnostic record.
    } // End while.
}

  */

// -----------------------------------------------------------------------------------------
BOOL CSql::IsMoreResults()
{
	BOOL bBindBookmark = FALSE;
	SQLRETURN rc;

	rc = SQLMoreResults(m_hstmt); 

	if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
	{
#ifdef _DEBUG
		if (rc == SQL_SUCCESS_WITH_INFO)
			TraceDiagRecords();
#endif

		if (pDescribeCol != NULL)
			bBindBookmark = pDescribeCol->bBindBookmark;

		Clean();
		UpdateColInfo(bBindBookmark);
		return TRUE;
	}

	return FALSE;
}

