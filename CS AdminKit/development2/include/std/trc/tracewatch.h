#ifndef __KLTRACEWATCH_H__
#define __KLTRACEWATCH_H__

#include "build/general.h"
#include "std/base/kldefs.h"
#include "std/err/klerrors.h"
#include "avp/avp_data.h"
#include "std/err/error.h"
#include "std/win32/klos_win32v40.h"
#include "std/tp/threadspool.h"

namespace KLSTD
{
    class CTraceWatch
    {
    public:
        CTraceWatch();
        virtual ~CTraceWatch();
        KLSTD_NOTHROW void Create(    const std::wstring& wstrProduct,
                        const std::wstring& wstrVersion,
                        int                 argc,
                        TCHAR**             argv,
                        AVP_dword           dwFlags) throw();
        KLSTD_NOTHROW void Destroy() throw();
        //this method must be called on the same thread where method Create was called
        KLSTD_NOTHROW void Check();
    protected:
        void ResetNotifier();
        void UpdateTracing(bool bFirstTime = false);
    protected:
        HANDLE                      m_hEvent;
        CRegKey                     m_key;
        ;
        int         m_argc;
        TCHAR**     m_argv;
        AVP_dword   m_dwFlags;
        ;
        std::wstring    m_wstrProduct;
        std::wstring    m_wstrVersion;
        std::wstring    m_wstrTraceFile;
        long            m_nTraceLevel;
        bool            m_bTraceStarted;
    };
};

#endif //__KLTRACEWATCH_H__