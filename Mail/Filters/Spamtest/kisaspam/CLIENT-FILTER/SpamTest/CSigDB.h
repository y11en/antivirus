/* Copyright (C) 2003-2005 Ashmanov and Partners
 *
 * PROJECT: Kaspersky Personal Anti-Spam
 *//*!
 * \file  CSigDB.h
 * \author Alexey P. Ivanov
 * \brief Content Filtration Library SigDB wrapper
 *//*
 * COMPILER: MS VC++ 6.0          TARGET: Win32
 *  LIB: none
 * XREF: none
 *==========================================================================
 * REVISION:
 * 26.06.03: v 1.00 created by API
 *==========================================================================
 */
#ifndef  _CSIGDB_H
#define  _CSIGDB_H

#include <Filtration/API/filter.h>

#pragma warning( push )
#pragma warning( disable : 4244 )
#include <Filtration/signatures/sigdb.h>
#pragma warning( pop )

class CFilterDll;

class CSigDB 
{
// Data:
protected:
  CFilterDll* m_pCFilterDll;
  lingproc*   m_lp;
  sigdb*      m_sdb;
  docimage*   m_tmpimage;
  signature*  m_tmpsig;
  sigdb_check_result* m_result;
  TCHAR       m_szPath[_MAX_PATH];
  double      m_fThreshold;
  DWORD       m_dwMode;
  DWORD       m_dwCount;
  DWORD       m_dwSize;
  enum tagLim { nDefLimit=4000 };
  DWORD       m_dwLimit;

  enum tagSigOrdinals { nTabSize=12, nOrdBeg=3010, nOrdInc=10 };
  FARPROC m_pfSig[nTabSize];

  CRITICAL_SECTION m_sect;

// Construction:
public:
  CSigDB();
  ~CSigDB();
  int    Init( CFilterDll* pCFilterDll );
  int    Done();
  sigdb* OpenSamples( LPCTSTR szDefaultSamples, DWORD mode=SIGDB_READ ); 
  sigdb* Open( LPCTSTR szName, DWORD mode=SIGDB_READ ); // SIGDB_READ | SIGDB_RW
  int    Close();

// Attributes:
public:
  LPCTSTR GetPath() const { return m_szPath; }
  DWORD  GetMode() const { return m_dwMode; }
  bool   IsValid() const { return this != 0 && m_pCFilterDll != NULL && m_pfSig[0] != 0; }
  bool   IsOpened() const { return (*m_szPath) != '\0'; }
  // NB: That's KASP's categories, not CFLib's!
  enum { NONE=0, SPAM=1, PROBSPAM=2, OBSCENE=3, FORMAL=4 };
  DWORD  SetLimit( DWORD dwLimit = nDefLimit ) { DWORD dwOld = m_dwLimit; m_dwLimit = dwLimit; return dwOld; }

// Operations:
public:
  int     Flush();
  sigdb*  Reopen( DWORD dwNewMode = MAXDWORD, bool bClearBase = false );
  int     AddSample( sig_cat catid, const letter& msg, DWORD dwTresh100=0 );
  sig_cat CheckSample( const letter& msg );

// Implementation:
protected:
  int     CreateSig( const letter& msg );
  void    DestroySig();
  int     AddSig( sig_cat catid );
  sig_cat Check(DWORD dwTresh100=0);

  void    FreeResult();
  DWORD   dwSetThreshold( DWORD dwThreshold = MAXDWORD );
};


#endif /*_CSIGDB_H*/
