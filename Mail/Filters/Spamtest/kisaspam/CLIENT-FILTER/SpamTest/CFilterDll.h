/* Copyright (C) 2003-2005 Ashmanov and Partners
 *
 * PROJECT: Kaspersky Personal Anti-Spam
 *//*!
 * \file  CFilterDll.h
 * \author Alexey P. Ivanov
 * \brief Content Filtration Library DLL calls wrapper
 *//*
 * COMPILER: MS VC++ 6.0          TARGET: Win32
 *  LIB: none
 * XREF: none
 *==========================================================================
 * REVISION:
 * 26.06.03: v 1.00 created by API
 *==========================================================================
 */
#ifndef  _CFILTERDLL_H
#define  _CFILTERDLL_H

#include <Filtration/API/filter.h>
#include "CSigDB.h"

HANDLE CreateMonitorEvent();

extern "C" {
    typedef content_filter*   (*CF_OPEN_t)(const char* name);
    typedef int               (*CF_CLOSE_t)(content_filter* cf, unsigned int flags);
    typedef const rubricator* (*CF_RUBRICATOR_t)(content_filter* cf);
    typedef const category*   (*CF_RUBSEARCH_t)(const rubricator* r, unsigned int CID);
    typedef void              (*CF_RESULTFREE_t)(filter_result* fr);
    typedef filter_result*    (*CF_CHECK_t)(content_filter* cf, message* m, filter_result* fr, unsigned int flags);
    typedef filter_result*    (*CF_CHECKTEXT_t)(content_filter *cf, const unsigned char *data, size_t length, 
				                                unsigned int helpers, filter_result *fr, unsigned int flags);
    typedef int               (*CF_CREATEMIME_t)(message* msg, const char* msg_text, unsigned long msg_length);
    typedef int               (*CF_ADDATTACHMENT_t)(message* msg, msg_attachment *att);
    typedef void              (*CF_MESSAGEDESTROY_t)(message* msg);
};

//! Access to CFILTER.DLL
class CFilterDll {
protected:
    CF_OPEN_t           m_popen;
    CF_CLOSE_t          m_pclose;
    CF_CHECK_t          m_pcheck;
    CF_CHECKTEXT_t      m_pchecktext;
    CF_RUBRICATOR_t     m_prubricator;
    CF_RUBSEARCH_t      m_prsearch;
    CF_RESULTFREE_t     m_presfree;
    CF_CREATEMIME_t     m_pcreatemime;
    CF_ADDATTACHMENT_t  m_paddattachment;
    CF_MESSAGEDESTROY_t m_pmessagedestroy;

    HINSTANCE           m_hDll;        //<! Handle to CFILTER.DLL Library
    content_filter*     m_cf;          //<! Filter database

    unsigned            m_RootCID[4];
    unsigned long       m_nMessageCount;
    TCHAR               m_szMainBasePath[_MAX_PATH];

    HANDLE              m_hEvent;      //<! Handle of the event to signal

public:
    CSigDB m_sdb;

// Construction:
public:
    CFilterDll();                   //!< Constructor
    int  Init(HINSTANCE hinst=NULL);//!< Obtain cfilter path from registry or directory based on the module instance
    int  Init(LPCTSTR lpszDllName); //!< Load CFILTER.DLL and initialize functions pointers
    int  Done();                    //!< Free DLL
    ~CFilterDll();                  //!< Destructor

// API:
public:
    content_filter* Open();
    content_filter* Open(const char* name);
    bool CheckOpen(const char* name);
    int  Close(unsigned int flags=0);

// Attributes:
    enum _tagRootCatOrder { SPAM=0, OBSCENE, FORMAL, PROBSPAM };
    content_filter*   GetFilter() { return m_cf; }
    const rubricator* GetRubricator() const;

    bool IsValid() const { return m_hDll != NULL && m_cf != NULL; }
    LPCTSTR   MainBasePath() const { return m_szMainBasePath; }
    HINSTANCE GetInstance() const { return m_hDll; }
    HANDLE    GetEvent() { return m_hEvent ? m_hEvent : (m_hEvent = CreateMonitorEvent()); }

// Operations:
    filter_result* Check(message* m, filter_result* fr, 
                         unsigned int flags=FILTER_DEFAULTS);
    filter_result* CheckText(
                   const unsigned char *data, size_t length, 
                   filter_result *fr, unsigned int helpers=0, 
                   unsigned int flags=FILTER_DEFAULTS );

    void ResultFree(filter_result* fr) const;

    const category* RubSearch(unsigned int CID) const;

    int  MessageFromMIME(message* msg, const char* msg_text, 
                         unsigned long msg_length) const;
    int  MessageAdd(message* msg, msg_attachment *att) const;
    void MessageDestroy(message* msg) const;

    int  MapCID( unsigned CID ) const;

// Implementation
protected:
};

extern CFilterDll g_CFilterDll; //!< CFILTER.DLL global object


#endif /*_CFILTERDLL_H*/
