//$(DebugDLL)_Sql.lib odbc32.lib ODBCCP32.LIB
//
// Sql.h: interface for the CSql class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_SQL_H__FBA7C00A_007C_4467_8E06_4621C2A3D850__INCLUDED_)
#define AFX_SQL_H__FBA7C00A_007C_4467_8E06_4621C2A3D850__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//-------------------------------------------------------------------------//

#include <sql.h>
					   
//-------------------------------------------------------------------------//

#define STATE_COMMUNICATION_LINK_FAILURE	_T("08S01")
#define STATE_SERIALIZATION_FAILURE			_T("40001")		// deadlock
#define STATE_CONNECTION_TIMEOUT			_T("HYT01")
#define STATE_TIMEOUT						_T("HYT00")
#define STATE_INVALID_CURSOR				_T("24000")
#define STATE_STATEMENT_NOT_PREPARED		_T("HY007")		// when server have not sent not columns

//-------------------------------------------------------------------------//
struct SFieldValue;
class  CSql;

class CSqlRecordset
{
	SFieldValue*	m_pArray;
	DWORD			m_dwArrSize;
	CSql*			m_pSql;

	void Allocate	(DWORD i_dwSize);
	void Clear		();
	void InitFields	();	

public:
	CSqlRecordset(DWORD i_dwSize = 0);
	~CSqlRecordset();

	DWORD GetSize();
	SFieldValue* operator[](DWORD i_nFldNmb);
	SFieldValue* operator[](TCHAR* i_pstrFldName);

	friend CSql;
};

//-------------------------------------------------------------------------//

#define COLNAMEMAX	96
#define _ERROR_BUF	4096

//-------------------------------------------------------------------------//

typedef struct SDescribeCol
{
	BOOL		bBindBookmark;
	SWORD		iTotalCol;
	UWORD		iCol;
	TCHAR		szColName[COLNAMEMAX];
	SWORD		pcbColName;
	SWORD		fSqlType;
	SQLUINTEGER cbColDef;
	SWORD		pibScale;
	SWORD		pfNullable;
	BOOL		bAutoIncrement;
	BOOL		bCaseSensitive;
	SDWORD		iDisplaySize;
	SQLTCHAR		szColumnLabel[COLNAMEMAX];
	SDWORD		iLength;
	BOOL		bMoney;
	SQLCHAR		szOwnerName[COLNAMEMAX];
	SQLCHAR		szQualifier[COLNAMEMAX];
	SDWORD		fSearchable;
	SQLCHAR		szTableName[COLNAMEMAX];
	SQLCHAR		szTypeName[COLNAMEMAX];
	BOOL		bUnsigned;
	SDWORD		fUpdatable;
}TSDescribeCol, *PTSDescribeCol;

//-------------------------------------------------------------------------//
typedef struct SBoundCol
{
	UWORD		iCol;
	SWORD		fSqlType;
	PVOID		pBuffer;
	SDWORD		BufferSize;
	SDWORD		cbValue;
}TSBoundCol, *PTSBoundCol;

//-------------------------------------------------------------------------//
struct SFieldValue
{
	typedef enum
	{
		VT_NULL = 0,
		VT_BOOL = 1,
		VT_UI1	= 2,
        VT_I2	= 3,
        VT_I4	= 4,
        VT_R4	= 5,
        VT_R8	= 6,
        VT_DATE = 7,
        VT_I1	= 8,
		VT_CREF = 9,
		VT_TEXT = 10,
        VT_UI2	= 11,
        VT_UI4	= 12,
        VT_INT	= 13,
        VT_UINT = 14,
        VT_BYREF = 15,
		VT_TIMESTAMP = 16,
		VT_TIME = 17,
		VT_WCREF = 18,
		VT_WTEXT = 19
	} FieldType;
	
	int		vt;							// VT_NULL
	PVOID	pOriginalBuffer;
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
    }
	Value;

	unsigned long FieldSize;

	SFieldValue();

	void	Init();

	DWORD   GetStringPresentationA(CHAR* o_strBuf, DWORD i_dwBufLen);
	DWORD   GetStringPresentationW(WCHAR* o_strBuf, DWORD i_dwBufLen);

	void	SetRef(TIMESTAMP_STRUCT& i_oTS);
	void	SetRefVarBin(BYTE* i_pByte, DWORD i_dwLen);
	void	SetRefText	(CHAR* i_str);
	void	SetRefTextW	(WCHAR* i_str);
	
protected:
	BOOL	GetFldTypes	(SWORD& o_nLocalType, SWORD& o_nSQLType);
	void*	GetFldBuffer();
	SDWORD	GetFldBufferSize();

	friend CSql;
};

//-------------------------------------------------------------------------//

typedef void (*BINDPROC) (void* /*UserValue*/, SWORD /*ParamID*/, SWORD* /*pLocalType*/, SWORD* /*pSqlType*/, SDWORD* /*pValueSize*/);
typedef void (*PUTDATA) (void* /*UserValue*/, SWORD /*ParamID*/, PVOID* /*Buffer*/, SDWORD* /*pBufferSize*/);

//-------------------------------------------------------------------------//

#ifdef _UNICODE 
	#define DoSqlQuery		DoSqlQueryW
	#define DoSqlQueryEx	DoSqlQueryExW     
	#define DoBindQuery		DoBindQueryW
	#define DoBindQueryEx	DoBindQueryExW
	#define DoColumnInfo	DoColumnInfoW
	#define DoKeyInfo		DoKeyInfoW
#else
	#define DoSqlQuery		DoSqlQueryA
	#define DoSqlQueryEx	DoSqlQueryExA
	#define DoBindQuery		DoBindQueryA
	#define DoBindQueryEx	DoBindQueryExA
	#define DoColumnInfo	DoColumnInfoA
	#define DoKeyInfo		DoKeyInfoA
#endif

//-------------------------------------------------------------------------//
class CSql  
{
public:
	CSql();
	virtual ~CSql();

	enum eConcurrency
	{
		conc_READ_ONLY = 1,
		conc_LOCK,
		conc_ROWVER,
		conc_VALUES,
		conc_DEFAULT,
	};

	enum eCursorType
	{
		cursor_FORWARD_ONLY = 1,
		cursor_KEYSET_DRIVEN,
		cursor_DYNAMIC,    
		cursor_STATIC,     
		cursor_TYPE_DEFAULT
	};
									  
	enum eScrollability
	{
		scroll_NONSCROLLABLE = 1,
		scroll_SCROLLABLE
	};

	enum eCursorSensitivity
	{
		sensit_UNSPECIFIED  = 1,
		sensit_INSENSITIVE,
		sensit_SENSITIVE
	};

	enum eFetchOrientation
	{
		fetch_NEXT,      
		fetch_FIRST,     
		fetch_LAST,      
		fetch_PRIOR,     
		fetch_ABSOLUTE,  
		fetch_RELATIVE  
	};

	// connect
	BOOL	Connect			(TCHAR* db_name, TCHAR* user_name, TCHAR* password, BOOL bBrowse = FALSE);
	BOOL	Trusted_Connect	(TCHAR* i_strDSN, TCHAR* AppName, HWND hWnd, BOOL i_bNoPrompt=FALSE);
	BOOL	Trusted_Connect2(TCHAR* i_strServerName, TCHAR* i_strDatabase, TCHAR* AppName, HWND hWnd, BOOL i_bNoPrompt=FALSE);
	BOOL	ChildConnect	(CSql* pSqlParent);
	BOOL	DropConnect		();
	BOOL	IsConnected		();
	BOOL	IsChildConnect	();

	// data
	BOOL	SetAsync		(BOOL bAsync = false);
	BOOL	IsAsync			();
	BOOL	IsDataReady		();
	void	Cancel			();	// cancel execution 
	void	DropData		();

	BOOL	DoSqlQueryA		(const CHAR* query, BOOL i_bAllocateRetInt = FALSE);	// false - error, true - sucsesfull, 2 - with info
	BOOL	DoSqlQueryW		(const WCHAR* query, BOOL i_bAllocateRetInt = FALSE);	// unicode
	BOOL	DoSqlQueryExA	(const CHAR* query, BOOL i_bAllocateRetInt = FALSE);
	BOOL	DoSqlQueryExW	(const WCHAR* query, BOOL i_bAllocateRetInt = FALSE);
	BOOL	DoSqlQueryIn	(const CHAR* query, BOOL i_bAllocateRetInt = FALSE);
	BOOL	DoBindQueryA	(const CHAR* query, SWORD ParamCount, BINDPROC pBindProc, PUTDATA pPutData, void* UserValue, BOOL i_bAllocateRetInt = FALSE);
	BOOL	DoBindQueryW	(const WCHAR* query, SWORD ParamCount, BINDPROC pBindProc, PUTDATA pPutData, void* UserValue, BOOL i_bAllocateRetInt = FALSE);
	BOOL	DoBindQueryExA	(const CHAR* query,  CSqlRecordset& i_oParams, BOOL i_bAllocateRetInt = FALSE);
	BOOL	DoBindQueryExW	(const WCHAR* query, CSqlRecordset& i_oParams, BOOL i_bAllocateRetInt = FALSE);

	BOOL	BulkOperations	(SQLUSMALLINT Operation);
	BOOL	DoColumnInfoA	(CHAR* basename);
	BOOL	DoColumnInfoW	(WCHAR* basename);
	BOOL	DoKeyInfoA		(CHAR* basename);
	BOOL	DoKeyInfoW		(WCHAR* basename);

	WORD	GetColumnsCount	();
	SDWORD	GetRowsCount	();
	SDWORD	GetCurrentRow	();
	BOOL	NextData		();
	BOOL	NextDataEx		(BOOKMARK* lpBookmark = NULL, SDWORD Offset = 1, WORD MoveType = SQL_FETCH_NEXT);
	void	SetBookmarkValue(BOOKMARK Boomark);

	// Environment
	void	SetImageFiled	(ULONG ulImageSize);  // какой максимальный размер LongBinary вы согласны принять
	void	SetResultRowNumber(ULONG ResultRowNumber); // for DoSqlQueryExt
	void	SetShowDeleted	(BOOL bShow);
	TCHAR*	GetColumnName	(WORD col);
	SWORD	GetColumnType	(WORD col);			   
	int		GetColumnNmb	(TCHAR* i_strColumnName);

	//	void ToBinHex(PVOID from, SIZE_T count, PVOID to);
	HDBC	GetHdbc	();
	HSTMT	GetHstmt();

	// system
	BOOL	SetConfigMode	(UWORD wConfigMode);
	BOOL	GetConfigMode	(UWORD* pwConfigMode);
	BOOL	GetQuoteIdentifier(SQLCHAR* pStr, SQLSMALLINT BufferLength);
	BOOL	CanUpdate		();
	SWORD	CanMultiOperator();
	BOOL	CanUseBookmark	();
	void	GetErrorString	(TCHAR* pBuf, DWORD BufLength);
	void	GetStateString	(TCHAR* pBuf);

	// getting rowset
	BOOL	MoveNext		(CSqlRecordset& o_oRecordset);
	SQLRETURN Fetch			(CSqlRecordset& o_oRecordset, eFetchOrientation i_eFetchOrientation, int i_nOffset);

	int		GetResultsetNumber();
	BOOL	HasInfoRecords	();

	void	IgnoreNoDataResult();
	void	DisableAutoTranslation(BOOL i_bDisable);

	BOOL	GetDiagRec		(BOOL i_bStatementHandle, int i_nRecNmb, LPTSTR o_strState,
								int& o_nNativeError, LPTSTR o_strMsg, int i_nMsgBufLen);

	void	SetConcurrency	(eConcurrency i_eConcurrency);
	void	SetCursorType	(eCursorType i_eCursorType);
	void	SetScrollability(eScrollability i_eScrollability);
	void	SetCursorSensitivity(eCursorSensitivity i_eCursorSensitivity);

	BOOL	  GetFieldValue	(SFieldValue* pvar, SQLUINTEGER col);
	SQLRETURN GetFieldValue2(SFieldValue* pvar, SQLUINTEGER col);

	int		GetProcRetInt	();
	BOOL	IsMoreResults	();

	WCHAR	m_strCmd[_ERROR_BUF];

#ifdef _DEBUG
	void	TraceDiagRecords();
#endif

protected:
							   
	HENV	m_henv;
	HDBC	m_hdbc;
	HSTMT	m_hstmt;
	BOOL	m_bConnected;
	BOOL	m_bChildConnect;
	BOOL	m_bDataReady;
	BOOL	m_bBusy;
	BOOL	m_Async;
	BOOL	m_bShowDeleted;
	ULONG	m_ulImageSize;
	ULONG	m_ResultRowNumber;
	BOOKMARK m_BookMark;
	SQLUSMALLINT m_RowStatus;
	int		m_nResultsetNmb;
	BOOL	m_bIgnoreNoData;
	BOOL	m_bDisableAutoTranslation;
	BOOL	m_bHasInfoRecords;

	eConcurrency	m_eConcurrency;
	eCursorType		m_eCursorType;
	eScrollability	m_eScrollability;
	eCursorSensitivity m_eCursorSensitivity;
	int		m_nProcRetInt;

	TSBoundCol* pBoundCol;
	TSDescribeCol* pDescribeCol;

	// internal
	HDBC	PrepareHDBC(void);

	// data
	void	BindAllColumns	();
	void	UpdateColInfo	(BOOL bBindBookmark);
	SWORD	GetTotalColumns	();

	// error
	BOOL	DoQueryA		(const CHAR* query, BOOL ext, BOOL columninfo, BOOL keyinfo, BOOL i_bAllocateRetInt=FALSE);
	BOOL	DoQueryW		(const WCHAR* query, BOOL ext, BOOL columninfo, BOOL keyinfo, BOOL i_bAllocateRetInt=FALSE);
	BOOL	ConnectInternal	(TCHAR* strConnectionStr, HWND hwnd, BOOL i_bNoPrompt );
	BOOL	AllocStmtHandle	();

	void	Clean			();
	void	FreeColumnDescriptor();
	void	FreeColumnBinding();

	static  void RecordsetBind(void* UserValue, SWORD ParamID, SWORD* pLocalType, SWORD* pSqlType, SDWORD* pValueSize);
	static  void RecordsetPutData(void* UserValue, SWORD ParamID, PVOID* Buffer, SDWORD* pBufferSize);
};

//-------------------------------------------------------------------------//
// inlines 
inline BOOL CSql::DoColumnInfoA(CHAR* basename){
	return DoQueryA(basename, FALSE, TRUE, FALSE);
}

inline BOOL CSql::DoColumnInfoW(WCHAR* basename){
	return DoQueryW(basename, FALSE, TRUE, FALSE);
}

inline BOOL CSql::DoKeyInfoA(CHAR* basename){
	return DoQueryA(basename, FALSE, FALSE, TRUE);
}

inline BOOL CSql::DoKeyInfoW(WCHAR* basename){
	return DoQueryW(basename, FALSE, FALSE, TRUE);
}

inline BOOL CSql::DoSqlQueryA(const CHAR* query, BOOL i_bAllocateRetInt){
	return DoQueryA(query, FALSE, FALSE, FALSE, i_bAllocateRetInt);
}

inline BOOL CSql::DoSqlQueryW(const WCHAR* query, BOOL i_bAllocateRetInt){
	return DoQueryW(query, FALSE, FALSE, FALSE, i_bAllocateRetInt);
}

inline BOOL CSql::DoSqlQueryExA(const CHAR* query, BOOL i_bAllocateRetInt){
	return DoQueryA(query, TRUE, FALSE, FALSE, i_bAllocateRetInt);
}

inline BOOL CSql::DoSqlQueryExW(const WCHAR* query, BOOL i_bAllocateRetInt){
	return DoQueryW(query, TRUE, FALSE, FALSE, i_bAllocateRetInt);
}

inline void CSql::SetImageFiled(ULONG ulImageSize){
	m_ulImageSize = ulImageSize;
}

inline void CSql::SetResultRowNumber(ULONG ResultRowNumber){
	m_ResultRowNumber = ResultRowNumber;
}

inline void CSql::SetShowDeleted(BOOL bShow){
	m_bShowDeleted = bShow;
}

inline void CSql::IgnoreNoDataResult(){
	m_bIgnoreNoData = TRUE;
}

inline void CSql::DisableAutoTranslation(BOOL i_bDisable){
	m_bDisableAutoTranslation = i_bDisable;
}

inline void CSql::SetConcurrency(eConcurrency i_eConcurrency){
	m_eConcurrency = i_eConcurrency;
}

inline void CSql::SetCursorType	(eCursorType i_eCursorType){
	m_eCursorType = i_eCursorType;
}

inline void CSql::SetScrollability(eScrollability i_eScrollability){
	m_eScrollability = i_eScrollability;
}

inline void	CSql::SetCursorSensitivity(eCursorSensitivity i_eCursorSensitivity){
	m_eCursorSensitivity = i_eCursorSensitivity;
}

inline int CSql::GetProcRetInt(){
	return m_nProcRetInt;
}

//-------------------------------------------------------------------------//

inline HDBC		CSql::GetHdbc()			{ return m_hdbc; }
inline HSTMT	CSql::GetHstmt()		{	return m_hstmt;		}
inline BOOL		CSql::IsAsync()			{	return m_Async;		}
inline BOOL		CSql::IsDataReady()		{	return m_bDataReady;}
inline BOOL		CSql::IsConnected()		{	return m_bConnected;}
inline BOOL		CSql::IsChildConnect()	{	return m_bChildConnect;}
inline int		CSql::GetResultsetNumber()	{	return m_nResultsetNmb;}
inline BOOL		CSql::HasInfoRecords()	{	return m_bHasInfoRecords;}

//-------------------------------------------------------------------------//
								      
#pragma comment(lib , "odbc32.lib")
#pragma comment(lib , "odbccp32.lib")

#ifndef SQL_LIB_BILDING
	#ifdef _DEBUG
		#ifdef _UNICODE
		#pragma comment(lib , "O:\\CommonFiles\\Debug\\_sqlu.lib")
		#else	
		#pragma comment(lib , "O:\\CommonFiles\\Debug\\_sql.lib")
		#endif
	#else
		#ifdef _UNICODE
		#pragma comment(lib , "O:\\CommonFiles\\Release\\_sqlu.lib")
		#else	
		#pragma comment(lib , "O:\\CommonFiles\\Release\\_sql.lib")
		#endif
	#endif
#endif

//-------------------------------------------------------------------------//
#endif // !defined(AFX_SQL_H__FBA7C00A_007C_4467_8E06_4621C2A3D850__INCLUDED_)
