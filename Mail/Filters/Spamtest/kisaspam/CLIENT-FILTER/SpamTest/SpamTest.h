/* Copyright (C) 2003-2005 Ashmanov and Partners
 *
 * PROJECT: Kaspersky Personal Anti-Spam
 *//*!
 * \file  SpamTest.h
 * \author Alexey P. Ivanov
 * \brief Content Filtration processing
 *//*
 * COMPILER: MS VC++ 6.0          TARGET: Win32
 *  LIB: none
 * XREF: none
 *==========================================================================
 * REVISION:
 * 17.11.03: v 1.00 created by API
 *==========================================================================
 */
#ifndef __SPAMTEST_H__
#define __SPAMTEST_H__

#ifndef  _SPAMFILTER_H
#include "spamfilter.h"
#endif

const size_t g_SIGLEN = 16;
const size_t g_MAXVAL = 128;
const UINT   g_MAX_ATTACHMENTS = 16;
const size_t g_MAXSUBJECT = 1023;

const char g_szSignatureHeader[] = _T("X-SpamTestP-Signature");
const char g_szCategoryHeader[]  = _T("X-SpamTestP-Categories");
const char g_szFacilityHeader[]  = _T("X-SpamTestP-Facility");
const char g_szTrainHeader[]     = _T("X-SpamTestP-Train");
const char g_szServerStatusHeader[] = _T("X-SpamTest-Status");

extern HINSTANCE g_hinst;
extern HINSTANCE g_hiLang;

bool GetMIMEHeader( LPCSTR  szBuffer,  LPCSTR szHeader,
                    LPCSTR* pVal=NULL, UINT*  ccVal=NULL );

bool HeaderSignature( LPCSTR szBuffer, LPCSTR szCat=NULL, 
                      LPSTR szSign=NULL, size_t ccSign=0 );

HRESULT PreProcess( MESSAGEINFO* pInfo, UINT* type, BOOL* pbLog );
HRESULT PostProcess( MESSAGEINFO *pInfo, UINT type );

void BeginWait();
void EndWait();

extern BOOL  g_bRegistryChanged;   //!< Registry options changed. Paths may be changed
//extern BOOL  g_bFilesChanged;    //!< Files with examples changed
extern BOOL  g_bBaseChanged;       //!< Updater successfully update base
extern BOOL  g_bSamplesDirChanged; //!< User changed Samples Direcory
extern BOOL  g_bSamplesClear;      //!< User cleared Samples Direcory

extern DWORD g_dwLastNo;           //!< Last compiled sample

#ifdef __SPAMSTUB
typedef LPSTR message;
#else
#include "CFilterDll.h"
#include "cfdata.h"
#endif __SPAMSTUB

struct __message_with_headers {
	message msg;
	const char *m_pHeaders;
	const char *m_pEmail;
	unsigned int m_MsgSize;
    BOOL m_bNotInternetTextBody;
};

typedef __message_with_headers message_with_headers;


extern "C" {
  HRESULT WINAPI InitSpamTestBase(LPCSTR szPath);
  HRESULT WINAPI DoneSpamTestBase();
  HRESULT WINAPI InitSamplesBase();
};

HRESULT FillMessage( message_with_headers& m, MESSAGEINFO *pInfo );

unsigned SpamTest( message_with_headers& m );

void CreateMessage( message_with_headers& m, MESSAGEINFO* pInfo );
int  AddAttach( message_with_headers& m, IStream* pStream, 
                LPCTSTR szContentType=NULL, LPCTSTR szCharSet=NULL, 
                LONG lSize=-1, LPCTSTR szFileName=NULL );
void DestroyMessage( message_with_headers& m );

void OnSamplesChanged( LPCTSTR szFile=NULL );

// Facility: CFLIB
HRESULT InitCFLIB(LPCSTR szPath);
HRESULT DoneCFLIB();
UINT    CheckCFLIB( message& szBuffer );
BOOL    KeepTagSubjectCFLIB(MESSAGEINFO *pInfo);
UINT    CheckServerCFLIB(MESSAGEINFO *pInfo);
UINT    CheckProfile( message_with_headers& m );

unsigned CheckUser( message& msg );

DWORD   GetPersonality();

LPCTSTR cf_err_message(cfd_error error);
LPCTSTR cf_warn_message(unsigned int code);
LPCTSTR cf_info_message(unsigned int code);

TCHAR*  LoadLangString(UINT uID);

#endif//__SPAMTEST_H__
