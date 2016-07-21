#ifndef __KLTRACE_USERNAME_H__
#define __KLTRACE_USERNAME_H__

namespace KLSTD
{
#ifdef _WIN32
    KLSTD_NOTHROW void TraceImpersonated(int nLevel = 4) throw();
#else
    inline KLSTD_NOTHROW void TraceImpersonated(int nLevel = 4) throw()
    {
        ;
    };
#endif
};

#endif //__KLTRACE_USERNAME_H__
