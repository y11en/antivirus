#ifndef  _SPAMFILTER_H
#define  _SPAMFILTER_H

/*!
 * (C) 2003-2005 Ashmanov and Partners JSC
 *
 * \file 
 * \author A&P
 * \date 2003
 * \brief Filtering DLL
 *
 * Contain functions wich implements filtering algorithms
 * 
 *
 */

#ifndef __objidl_h__
#include <objidl.h> // IStream
#endif


#ifndef  _PROGRESS_H
#include "Progress.h" // Progress
#endif
#include "cfdata.h" // cfd_file_info

//! SpamTest recognition facilities: MainBase, User Samples, B/W lists
const DWORD STFACILITY_MASK    = 0xFF000000;
const DWORD STFACILITY_GENERAL = 0x00000000;
const DWORD STFACILITY_USER    = 0x80000000;
const DWORD STFACILITY_BLACK   = 0x40000000;
const DWORD STFACILITY_WHITE   = 0x20000000;
const DWORD STFACILITY_GSG1    = 0x10000000;
const DWORD STFACILITY_BAYES   = 0x08000000;
const DWORD STFACILITY_SERVER  = 0x04000000;
const DWORD STFACILITY_HEADERS = 0x02000000;
const DWORD STFACILITY_GSG2    = 0x01000000;

const char g_szFacilityUnknown[] = _T("Unknown");
const char g_szFacilityNone[]    = _T("Main");
const char g_szFacilityGeneral[] = _T(""); // empty string is important!
const char g_szFacilityBlack[]   = _T("BlackList");
const char g_szFacilityWhite[]   = _T("WhiteList");
const char g_szFacilityUser[]    = _T("UserSample");
const char g_szFacilityGSG1[]    = _T("GSG1");
const char g_szFacilityBayes[]   = _T("Bayes");
const char g_szFacilityServer[]  = _T("Server");
const char g_szFacilityHeaders[] = _T("Headers");
const char g_szFacilityGSG2[]    = _T("GSG2");

LPCTSTR GetFacilityName( UINT type );

inline UINT sptGetFacility( UINT type ) { return UINT(DWORD(type) & (DWORD(STFACILITY_MASK))); }
inline UINT sptGetType( UINT type ) { return UINT(DWORD(type) & (~DWORD(STFACILITY_MASK))); }

//! Information about message attachment
class ATTACHINFO
{
public:
    virtual ULONG STDMETHODCALLTYPE Release() = 0; //!< Should be called when object not need more

    IStream *m_lpStream;        //!< Stream to attachment itself
    LPCTSTR m_lpszFileName;     //!< PR_ATTACH_LONG_FILENAME 
    LPCTSTR m_lpszContentType;  //!< PR_ATTACH_MIME_TAG
    LONG m_lSize;               //!< PR_ATTACH_SIZE
    LPCTSTR m_lpszCharSet;      //!< PARAM_PART_CHARSET in MailChecker
    LPCTSTR m_lpszContentId;    //!< PR_ATTACH_CONTENT_ID in MessageInfo.h
};


#define ON_OUTLOOK_USERACTION    0 //<! IsSpamMessage called from OL plugin by user action
#define ON_OUTLOOK_DELIVERY      1 //<! IsSpamMessage called from OL plugin on mail delivery
#define ON_MAILCHECKER_DELIVERY  2 //<! IsSpamMessage called from OE plugin on mail delivery in Kaspersky mail checker

//! Information about message. Pointer may be NULL if property not exists in the letter
class MESSAGEINFO
{
public:
    // Pointers can be NULL if property not exists
    UINT   m_uType;        //!< type of call: ONOUTLOOK_USERACTION, ...

    LPCSTR m_pHeaders;     //!< PR_TRANSPORT_MESSAGE_HEADERS
    LPCSTR m_pSubject;     //!< PR_SUBJECT    
    LPCSTR m_pFrom;        //!< PR_SENDER_NAME
    LPCSTR m_pTo;          //!< ?? for Log
    LPCSTR m_pCC;          //!< ?? for Log
    LPCSTR m_pEmail;       //!< PR_SENDER_EMAIL_ADDRESS
    LPCSTR m_pAddrType;    //!< PR_SENDER_ADDRTYPE
    LPCSTR m_pMessageClass; //!< MAPI message class ("IPM.Note", ...)
    
    //! Get plain text body stream if exists. PR_INTERNET_CHARSET_BODY or PR_BODY 
    virtual HRESULT STDMETHODCALLTYPE GetBodyText(IStream **pStream, BOOL *pbInternetBody, LPSTR szCharset, UINT nChars) = 0; 

    //! Get HTML body stream if exists. PR_BODY_HTML = PROP_TAG(PT_TSTRING, 0x1013)
    virtual HRESULT STDMETHODCALLTYPE GetBodyHTML(IStream **pStream, LPSTR szCharset, UINT nChars) = 0; 

    //! Get RTF body stream if exists. PR_RTF_COMPRESSED + WrapCompressedRTFStream.
    virtual HRESULT STDMETHODCALLTYPE GetBodyRTF(IStream **pStream) = 0; 

    //! Get information about message attachment with given number. Call (*ppAttachInfo)->Release() when information not need more
    virtual HRESULT STDMETHODCALLTYPE GetAttachment(UINT num, ATTACHINFO **ppAttachInfo) = 0;

    //! Add message headers, semicolon separated
    virtual HRESULT STDMETHODCALLTYPE AddHeaders(LPCSTR lpszHeaders) = 0;

    //! Check email in enemis/friends address list
    virtual HRESULT STDMETHODCALLTYPE GetEmail(BOOL bFriends, LPCSTR pEmail, LPCSTR pAddrType, LPSTR pName, DWORD nSize) = 0;
};

extern "C" { 

// typedef the pointers to the module functions for LoadLibrary/GetProcAddress DLL usage
typedef HRESULT (WINAPI ISSPAMMESSAGE)(MESSAGEINFO *pInfo, UINT *type, BOOL *log);
typedef HRESULT (WINAPI KEEPTAGSUBJECT)(MESSAGEINFO *pInfo, BOOL *keep);

typedef HRESULT (WINAPI INITBASE)(LPCSTR szPath);
typedef HRESULT (WINAPI DONEBASE)(void);

typedef HRESULT (WINAPI INITMKSAMPLE)(void);
typedef HRESULT (WINAPI MKSAMPLEMIME)( const char* pMIME, size_t ccMIME, const char* lpszUserCategory, DWORD dwFlags );
typedef HRESULT (WINAPI MKSAMPLEMAPI)( MESSAGEINFO *pInfo, const char* lpszUserCategory, DWORD dwFlags );

typedef DWORD   (WINAPI COMPILESAMPLE)( LPCTSTR szSDB, 
                    LPCTSTR szSamplesDir,
                    HINSTANCE hiLang/*=NULL*/, Progress* p_progress/*=NULL*/,
                    DWORD nBeg/*=1*/, DWORD nEnd/*=MAXDWORD*/ );

typedef HRESULT (WINAPI COMPILENEWSAMPLES)();

typedef DWORD   (WINAPI COMPILEUPDATE)( LPCTSTR out, 
                    cfd_file_info* fi, size_t fi_size, 
                    HINSTANCE hiLang/*=NULL*/, Progress* p_progress/*=NULL*/ );

typedef HRESULT (WINAPI CHECKOPEN)( LPCTSTR szPath );

typedef DWORD   (WINAPI RASPACKFILE)( LPCTSTR szSrcFile, LPCTSTR szDstFolder, LPCTSTR szDstName/*=NULL*/ );

typedef HRESULT (WINAPI CLEARSAMPLES)(void);

// prototypes not used - use typedefs and LoadLibrary/GetProcAddress technique
/*!
  \brief Check if message is spam message
  \param pInfo          [in] pointer to information about the message 
  \param type           [out] type of spam.
         0              Not spam.
         >0             Category of spam.
  \param log            [out] write to log file and statistics
  \return standard error codes or 0 on success
*/
HRESULT WINAPI IsSpamMessage(MESSAGEINFO *pInfo, UINT *type, BOOL *log);
HRESULT WINAPI KeepTagSubject(MESSAGEINFO *pInfo, BOOL *keep);

HRESULT WINAPI InitBase(LPCSTR szPath);
HRESULT WINAPI DoneBase();

HRESULT WINAPI MkSampleMIME( const char* pMIME, size_t ccMIME, const char* lpszUserCategory, DWORD dwFlags );
HRESULT WINAPI MkSampleMAPI( MESSAGEINFO *pInfo, const char* lpszUserCategory, DWORD dwFlags );
// MkSample options dwFlags:
#define MKS_MAKE_SAMPLE         0x00000001 // default - add to local spam samples database
#define MKS_TRAIN               0x00000002 // train euristic recognition
#define MKS_SPAM                0x00000004 // message is spam for euristic recognition

HRESULT WINAPI InitMkSample();

DWORD   WINAPI CompileSample( LPCTSTR szSDB, LPCTSTR szSamplesDir,
                              HINSTANCE hiLang/*=NULL*/,
                              Progress* p_progress/*=NULL*/,
                              DWORD nBeg/*=1*/, DWORD nEnd/*=MAXDWORD*/ );

// cf-compile returns:
#define CF_EXIT_OK           0 // success
#define CF_EXIT_WARN         1 // compilation warning (warning level 3)
#define CF_EXIT_ERROR        2 // compilation error
#define CF_EXIT_LAST         2

HRESULT WINAPI CompileNewSamples();

DWORD   WINAPI CompileUpdate( LPCTSTR out, 
                              cfd_file_info* fi, size_t fi_size, 
                              HINSTANCE hiLang/*=NULL*/, Progress* p_progress/*=NULL*/ );

HRESULT WINAPI CheckOpen( LPCTSTR szPath );

DWORD   WINAPI RaspackFile( LPCTSTR szSrcFile, LPCTSTR szPasswd, LPCTSTR szDstFolder, LPCTSTR szDstName/*=NULL*/ );

HRESULT WINAPI ClearSamples(void);

/*!
  \brief Create a configuration of source XML files and compile them into .pdb and .ldb binary files
  \param pszOut         [in] output filename (includes full path and without extention)
  \param pszUPKProfiles [in] pointer to the temporary directory where source and result files will be placed
  \param pszUPKPwd      [in] password for profiles.upk archive
  \param hiLang         [in] 
  \param p_progress     [in] progress object
  \return               standard error codes or 0 on success
*/
DWORD WINAPI CompileProfiles(LPCTSTR pszOut,
                             LPCTSTR pszUPKProfiles, LPCTSTR pszUPKPwd,
                             HINSTANCE hiLang/*=NULL*/, Progress* p_progress/*=NULL*/ );


///////////////////////////////////////////////////////////////////////////////
// CATNAMES.DLL interface

/*!
  \brief Get Category/Folder name for given spam type
  \param wLanguageId   [in] Language ID
  \param type          [in] Category of spam
         0             Spam Root Folder
         >0            Spam Category Folder
  \param buffer        [out] Pointer to a buffer that receives the name of the subfolder for spam for given category. Empty name means root of spam folders.
  \param nBuffer       Specifies the length, in CHARs, of the buffer buffer. If the length of the name exceeds this limit, the string is truncated. 
  \return standard error codes or 0 on success
*/
HRESULT WINAPI GetCatName(WORD wLanguageId, UINT type, LPSTR buffer, DWORD nBuffer);
typedef HRESULT (WINAPI GETCATNAME)(WORD wLanguageId, UINT type, LPSTR buffer, DWORD nBuffer);

UINT    WINAPI GetCatID(WORD wLangID, LPCSTR szName, DWORD ccName); // ccName may be 0 for strlen()
typedef UINT    (WINAPI GETCATID)(WORD wLangID, LPCSTR szName, DWORD ccName);

HRESULT WINAPI ProcessSubjectA(WORD wLanguageId, UINT type, LPCSTR subject, LPSTR buffer, DWORD nBuffer); // S_OK - changed, S_FALSE - not changed, other codes - errors
typedef HRESULT (WINAPI PROCESSSUBJECTA)(WORD wLanguageId, UINT type, LPCSTR subject, LPSTR buffer, DWORD nBuffer);

HRESULT WINAPI ProcessSubjectW(WORD wLanguageId, UINT type, LPCWSTR subject, LPWSTR buffer, DWORD nBuffer); // S_OK - changed, S_FALSE - not changed, other codes - errors
typedef HRESULT (WINAPI PROCESSSUBJECTW)(WORD wLanguageId, UINT type, LPCWSTR subject, LPWSTR buffer, DWORD nBuffer);

} // extern "C" 

#endif


