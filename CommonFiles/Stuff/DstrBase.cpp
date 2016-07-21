// Base.cpp: implementation of the CDistrBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DstrBase.h"
#include <my_util.h>

const CTime CDistrBase::NULL_TIME( 1971, 1, 1, 0, 0, 0 );

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDistrBase::CDistrBase( CDatabase* pDatabase, const CString& connect_str )
	: CRecordset(pDatabase) {

	//{{AFX_FIELD_INIT(CDistrBase)
	m_id          = _T("");
  m_unique      = 0;
  m_norm_name   = _T("");
  m_valid       = FALSE;
  m_key_limit   = 0;
  m_expired     = NULL_TIME;
  m_connection  = _T("");
	m_b1 = m_b2 = m_b3 = m_b4 = _T("");
	m_nFields = 13;
	//}}AFX_FIELD_INIT

	m_s_name = _T("");

  m_strSort = "[Id]";
	m_nDefaultType = snapshot; //dynaset; //dynamic;

  m_connect_str = connect_str;
}

// ---
CString CDistrBase::GetDefaultConnect() {
  return m_connect_str;
}

// ---
CString CDistrBase::GetDefaultSQL() {
	return _T("[dbo].[Distribs]");
}

// ---
void CDistrBase::DoFieldExchange(CFieldExchange* pFX) {
	//{{AFX_FIELD_MAP(CDistrBase)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Text  ( pFX, _T("[Id]"),          m_id          );
	RFX_Long  ( pFX, _T("[Ident]"),       m_unique      );
	RFX_Binary( pFX, _T("[Name]"),        m_b_name      );
	RFX_Text  ( pFX, _T("[NormName]"),    m_norm_name   );
  RFX_Bool  ( pFX, _T("[Valid]"),       m_valid       );
  RFX_Int   ( pFX, _T("[KeyLimit]"),    m_key_limit   );
  RFX_Date  ( pFX, _T("[Expired]"),     m_expired     );
	RFX_Text  ( pFX, _T("[Connection]"),  m_connection  );
	RFX_Text  ( pFX, _T("[Sign_b1]"),     m_b1          );
	RFX_Text  ( pFX, _T("[Sign_b2]"),     m_b2          );
	RFX_Text  ( pFX, _T("[Sign_b3]"),     m_b3          );
	RFX_Text  ( pFX, _T("[Sign_b4]"),     m_b4          );
	RFX_Date(pFX, _T("[LastActivity]"), m_last_activity);
	//}}AFX_FIELD_MAP
}


// ---
BOOL CDistrBase::Add( const char* id, const char* name, const char* norm, const char* e_mail, ULONG unique, const char* b1, const char* b2, const char* b3, const char* b4, char* err_buff, int err_size ) {

  BOOL ret = FALSE;

  HANDLE mutex = ::CreateMutex( 0, FALSE, "CDistrBase::Add" );

  if ( mutex ) 
    ::WaitForSingleObject( mutex, INFINITE );

  try {

    AddNew();

    int len = ::lstrlen(name) + 1;

	  m_id          = id;
    m_unique      = unique;
    str2bin( m_b_name, name, 128 );
    m_norm_name   = norm;
    m_key_limit   = 0;
    m_expired     = NULL_TIME;
    m_connection  = e_mail;
    #ifdef RECOVER
      m_valid     = TRUE;
    #else
      m_valid     = FALSE;
    #endif
    m_last_activity = CTime::GetCurrentTime();
    
    m_b1          = b1;
    m_b2          = b2;
    m_b3          = b3;
    m_b4          = b4;
	  
	  //m_name        = trans_liter( name );

    SetFieldDirty( 0, TRUE );
    ClearNullFieldStatus( GetBoundFieldIndex(&m_id)-1 );
    ClearNullFieldStatus( GetBoundFieldIndex(&m_unique)-1 );
    ClearNullFieldStatus( GetBoundFieldIndex(&m_b_name)-1 );
    ClearNullFieldStatus( GetBoundFieldIndex(&m_valid)-1 );
    ClearNullFieldStatus( GetBoundFieldIndex(&m_key_limit)-1 );
    ClearNullFieldStatus( GetBoundFieldIndex(&m_expired)-1 );
    ClearNullFieldStatus( GetBoundFieldIndex(&m_connection)-1 );
    ClearNullFieldStatus( GetBoundFieldIndex(&m_last_activity)-1 );
    ClearNullFieldStatus( GetBoundFieldIndex(&m_b1)-1 );
    ClearNullFieldStatus( GetBoundFieldIndex(&m_b2)-1 );
    ClearNullFieldStatus( GetBoundFieldIndex(&m_b3)-1 );
    ClearNullFieldStatus( GetBoundFieldIndex(&m_b4)-1 );
    Update();
    ret = TRUE;
  }
  catch( CDBException* e ) {
    if ( err_buff )
      e->GetErrorMessage( err_buff, err_size );
    e->Delete();
  }
  catch( CMemoryException* e ) {
    if ( err_buff )
      e->GetErrorMessage( err_buff, err_size );
    e->Delete();
  }

  if ( mutex ) {
    ::ReleaseMutex( mutex );
    ::CloseHandle( mutex );
  }

  return ret;
}


static CTime null_time( 1971, 1, 1, 0, 0, 0 );

// ---
void CDistrBase::Move( long nRows, WORD wFetchType ) {
  BOOL moved;

  try {
	  CRecordset::Move( nRows, wFetchType );
    moved = TRUE;
  }
  catch( CDBException* e ) {
    e->Delete();
    moved = FALSE;
  }
  catch( CMemoryException* e ) {
    e->Delete();
    moved = FALSE;
  }

  if ( !moved || IsEOF() ) {
	  m_id.Empty();
    m_s_name.Empty();
    m_norm_name.Empty();
    m_connection.Empty();
    m_last_activity = null_time;
    m_b1.Empty();
    m_b2.Empty();
    m_b3.Empty();
    m_b4.Empty();
  }
  else {
	  m_id.TrimRight();
	  m_s_name.TrimRight();
    bin2str( m_s_name, m_b_name );
	  m_norm_name.TrimRight();
	  m_connection.TrimRight();
    if ( IsFieldStatusNull(GetBoundFieldIndex(&m_last_activity)-1) )
      m_last_activity = null_time;
	  m_b1.TrimRight();
	  m_b2.TrimRight();
	  m_b3.TrimRight();
	  m_b4.TrimRight();
  }
}



// ---
void CDistrBase::LastActivity() {

  HANDLE mutex = ::CreateMutex( 0, FALSE, "CDistrBase::LastActivity" );
  if ( mutex ) 
    ::WaitForSingleObject( mutex, INFINITE );

  BOOL check = m_bCheckCacheForDirtyFields;
  m_bCheckCacheForDirtyFields = FALSE;

  try {
    Edit();
    m_last_activity = CTime::GetCurrentTime();
    SetFieldDirty( 0, FALSE );
    SetFieldDirty( &m_last_activity, TRUE );
    ClearNullFieldStatus( GetBoundFieldIndex(&m_last_activity)-1 );
    Update();
  }
  catch( CDBException* e ) {
    e->Delete();
  }
  catch( CMemoryException* e ) {
    e->Delete();
  }
  catch(...) {
  }
  
  m_bCheckCacheForDirtyFields = check;

  if ( mutex ) {
    ::ReleaseMutex( mutex );
    ::CloseHandle( mutex );
  }
}


// ---
void CDistrBase::OnSetOptions( HSTMT hstmt ) {

  LONG len;
  SQLUINTEGER val;

  if ( SQL_SUCCESS == ::SQLGetStmtAttr(hstmt,SQL_ATTR_QUERY_TIMEOUT,(SQLPOINTER)&val,0,&len) ) {
    if ( val ) 
      val *= 2;
    else
      val = 300;
    ::SQLSetStmtAttr( hstmt, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER)&val, 0 );
  }

	CRecordset::OnSetOptions(hstmt);

}

