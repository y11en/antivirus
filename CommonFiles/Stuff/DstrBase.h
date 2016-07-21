// Base.h: interface for the CDistrBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASE_H__C2566CA5_60FA_11D2_AA88_D89D687730FD__INCLUDED_)
#define AFX_BASE_H__C2566CA5_60FA_11D2_AA88_D89D687730FD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxdb.h>

// ---
class CDistrBase : public CRecordset {

public:
  static const CTime NULL_TIME;

	CDistrBase( CDatabase* pDatabase, const CString& connect_str );

// Field/Param Data
	//{{AFX_FIELD(CDistrBase, CRecordset)
	CString m_id;
  long    m_unique;
  CByteArray m_b_name;
	CString	m_norm_name;
  BOOL    m_valid;
  int     m_key_limit;
  CTime   m_expired;
	CString	m_connection;
	CString m_b1;
	CString m_b2;
	CString m_b3;
	CString m_b4;
	CTime	m_last_activity;
	//}}AFX_FIELD

	CString	m_s_name;

  void LastActivity();

protected :
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDistrBase)
	public:
	virtual CString GetDefaultConnect();    // Default connection string
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);  // RFX support
  virtual void Move( long nRows, WORD wFetchType );
	virtual void OnSetOptions(HSTMT hstmt);
	//}}AFX_VIRTUAL

  CString m_connect_str;

public :
  BOOL Add( const char* id, const char* name, const char* norm, const char* e_mail, ULONG unique, const char* b1, const char* b2, const char* b3, const char* b4, char* err_buff, int err_size );

};

#endif // !defined(AFX_BASE_H__C2566CA5_60FA_11D2_AA88_D89D687730FD__INCLUDED_)
