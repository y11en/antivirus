#if !defined(AFX_CKAHMAIN_H__A823A9DD_52D2_4380_9D1E_E1D2DE63F05F__INCLUDED_)
#define AFX_CKAHMAIN_H__A823A9DD_52D2_4380_9D1E_E1D2DE63F05F__INCLUDED_

#include "CKAHStorage.h"
extern LPCKAHSTORAGE g_lpStorage;

class CExternalLogger
{
    Guard m_guardLogger;

    typedef std::list<CKAHUM::EXTERNALLOGGERPROC> EXTERNALLOGGERLIST;
    EXTERNALLOGGERLIST m_logger;

public:
    CExternalLogger();
    ~CExternalLogger();

    void AddLogger(CKAHUM::EXTERNALLOGGERPROC logger);
    void RemoveLogger(CKAHUM::EXTERNALLOGGERPROC logger);

    void LoggerFunc (DWORD dwEventType, LPCSTR szString);
};

extern CExternalLogger g_externalLogger;

extern volatile long g_refcnt;

#endif // !defined(AFX_CKAHMAIN_H__A823A9DD_52D2_4380_9D1E_E1D2DE63F05F__INCLUDED_)
