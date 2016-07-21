//$(DebugDLL)_Sql.lib odbc32.lib ODBCCP32.LIB

// Sql.h: interface for the CSql class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_SQL_H__FBA7C00A_007C_4467_8E06_4621C2A3D850__INCLUDED_)
#define AFX_SQL_H__FBA7C00A_007C_4467_8E06_4621C2A3D850__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

#include <sql.h>
#include <sqlext.h>
#include "sqlucode.h"
#include <odbcss.h>
#include <odbcinst.h>

#define RETCODE_IS_FAILURE(x) ((x) == SQL_ERROR || \
	(x) == SQL_INVALID_HANDLE)

#define RETCODE_IS_SUCCESSFUL(x) (!RETCODE_IS_FAILURE(x))

#define COLNAMEMAX	96
#define _ERROR_BUF	4096

typedef void (*BINDPROC) (DWORD /*UserValue*/, SWORD /*ParamID*/, SWORD* /*pLocalType*/, SWORD* /*pSqlType*/, SDWORD* /*pValueSize*/);
typedef void (*PUTDATA) (DWORD /*UserValue*/, SWORD /*ParamID*/, PVOID* /*Buffer*/, SDWORD* /*pBufferSize*/);

typedef struct SDescribeCol
{
	BOOL bBindBookmark;
	SWORD iTotalCol;
	UWORD iCol;
//SDWORD iCol;
	SQLCHAR szColName[COLNAMEMAX];
	SWORD pcbColName;
	SWORD fSqlType;
	SQLUINTEGER cbColDef;
	SWORD pibScale;
	SWORD pfNullable;
	BOOL bAutoIncrement;
	BOOL bCaseSensitive;
	SDWORD iDisplaySize;
	SQLCHAR szColumnLabel[COLNAMEMAX];
	SDWORD iLength;
	BOOL bMoney;
	SQLCHAR szOwnerName[COLNAMEMAX];
	SQLCHAR szQualifier[COLNAMEMAX];
	SDWORD fSearchable;
	SQLCHAR szTableName[COLNAMEMAX];
	SQLCHAR szTypeName[COLNAMEMAX];
	BOOL bUnsigned;
	SDWORD fUpdatable;
}TSDescribeCol, *PTSDescribeCol;

typedef struct SBoundCol
{
	UWORD iCol;
	SWORD fSqlType;
	PVOID pBuffer;
	SDWORD BufferSize;
	SDWORD cbValue;
}TSBoundCol, *PTSBoundCol;

typedef struct SFieldValue
{
	typedef enum
	{
		VT_NULL = 0,
		VT_BOOL = 1,
		VT_UI1 = 2,
        VT_I2 = 3,
        VT_I4 = 4,
        VT_R4 = 5,
        VT_R8 = 6,
        VT_DATE = 7,
        VT_I1 = 8,
		VT_CREF = 9,
        VT_UI2 = 10,
        VT_UI4 = 11,
        VT_INT = 12,
        VT_UINT = 13,
        VT_BYREF = 14,
		VT_TIMESTAMP = 15,
		VT_TIME = 16,
		VT_WCREF = 17,
	} FieldType;
	
	int vt;								// VT_NULL
	PVOID pOriginalBuffer;
	SDWORD* pcbValue;
	union _Value
	{
        BYTE*				pboolVal;	// VT_BOOL.
		char*				pbVal;		// VT_UI1.
        short*				piVal;		// VT_I2.
        long*				plVal;		// VT_I4.
        float*				pfltVal;	// VT_R4.
        double*				pdblVal;	// VT_R8.
		char*				pcVal;		// VT_CREF
        unsigned short*		puiVal;		// VT_UI2.
        unsigned long*		pulVal;		// VT_UI4.
        int*				pintVal;	// VT_INT.
        unsigned int*		puintVal;	// VT_UINT.
        PVOID				pVal;		// VT_BYREF
		TIMESTAMP_STRUCT*	pTimeStamp;	// VT_TIMESTAMP
		TIME_STRUCT*		pTime;		// VT_TIME
		DATE_STRUCT*		pDate;		// VT_DATE
		WCHAR*				pwcref;		// WCREF
    	}Value;
	unsigned long FieldSize;
}TFieldValue;

class CSql  
{
public:
	CSql();
	virtual ~CSql();
// connect
	BOOL Connect(char* db_name, char* user_name, char* password, BOOL bBrowse = FALSE);
	BOOL Trusted_Connect(char* db_name, char* AppName, HWND hWnd);
	BOOL ChildConnect(CSql* pSqlParent);
	BOOL DropConnect();
	BOOL IsConnected();
	BOOL IsChildConnect();

// data
	void SetAsync(BOOL bAsync = false);
	BOOL IsMoreResults();
	BOOL IsAsync();
	BOOL IsDataReady();
	void Cancel();	// cancel execution 
	void DropData();

	BOOL DoSqlQuery(char* query);	// false - error, true - sucsesfull, 2 - with info
	BOOL DoSqlQueryW(WCHAR* query);	// unicode
	BOOL DoSqlQueryEx(char* query);
	BOOL DoSqlQueryExW(WCHAR* query);
	
	BOOL DoSqlQueryIn(char* query);

	BOOL DoBindQuery(char* query, SWORD ParamCount, BINDPROC pBindProc, PUTDATA pPutData, DWORD UserValue, BOOL bUnicode = FALSE);

	BOOL BulkOperations(SQLUSMALLINT Operation);
	
	BOOL DoColumnInfo(char* basename);
	BOOL DoKeyInfo(char* basename);

	WORD GetColumnsCount();
	SDWORD GetRowsCount();
	SDWORD GetCurrentRow();

	BOOL NextData();
	BOOL NextDataEx(BOOKMARK* lpBookmark = NULL, SDWORD Offset = 1, WORD MoveType = SQL_FETCH_NEXT);

	BOOL GetFiledValue(SFieldValue* pvar, SQLUINTEGER col);
	void SetBookmarkValue(BOOKMARK Boomark);
	// Environment
	void SetImageFiled(ULONG ulImageSize);  // какой максимальный размер LongBinary вы согласны принять
	void SetResultRowNumber(ULONG ResultRowNumber); // for DoSqlQueryExt
	void SetShowDeleted(BOOL bShow);
	char* GetColumnName(WORD col);
	SWORD GetColumnType(WORD col);

//	void ToBinHex(PVOID from, SIZE_T count, PVOID to);
	HDBC GetHdbc();
	HSTMT GetHstmt();
	// system
	BOOL SetConfigMode(UWORD wConfigMode);
	BOOL GetConfigMode(UWORD* pwConfigMode);
	BOOL GetQuoteIdentifier(SQLCHAR* pStr, SQLSMALLINT BufferLength);
	BOOL CanUpdate();
	SWORD CanMultiOperator();
	BOOL CanUseBookmark();

	void GetErrorString(char* pBuf, DWORD BufLength);
private:
	HENV m_henv;
	HDBC m_hdbc;
	HSTMT m_hstmt;

	char m_strCmd[_ERROR_BUF];

	BOOL m_bConnected;
	BOOL m_bChildConnect;
	BOOL m_bDataReady;
	BOOL m_bBusy;
	BOOL m_Async;
	BOOL m_bShowDeleted;

	SQLUSMALLINT m_RowStatus;
	BOOKMARK m_BookMark;

	TSDescribeCol* pDescribeCol;
	TSBoundCol* pBoundCol;

// internal
	HDBC PrepareHDBC(void);
// error handling
	SQLCHAR szSqlState[_ERROR_BUF];
	SQLCHAR szErrorMsg[_ERROR_BUF];
	void ProcessLogMessages(SQLSMALLINT plm_handle_type, SQLHANDLE plm_handle, char *logstring, int ConnInd);
// data
	void BindAllColumns();
	void UpdateColInfo(BOOL bBindBookmark);
	SWORD GetTotalColumns();
// error
	BOOL GetError(HSTMT hstmt);
	BOOL GetState(HSTMT hstmt);

	BOOL DoQuery(char* query, BOOL ext, BOOL columninfo, BOOL keyinfo, BOOL async, BOOL bUnicode);

	ULONG m_ulImageSize;
	ULONG m_ResultRowNumber;
};

#endif // !defined(AFX_SQL_H__FBA7C00A_007C_4467_8E06_4621C2A3D850__INCLUDED_)
