#ifndef __KL_LOCKS_H__
#define __KL_LOCKS_H__

#include "./sync.h"
#include <queue>


long KLCSC_DECL KLSTD_InterlockedCompareExchange(
        long volatile*  Destination,
        long            Exchange,
        long            Comperand);

long KLCSC_DECL KLSTD_InterlockedIncrementIfGreaterOrEqualThanZero(
        volatile long* plTarget);

AVP_longlong KLCSC_DECL KLSTD_InterlockedRead64(
                    const AVP_longlong volatile * plData);

void KLCSC_DECL KLSTD_InterlockedWrite64(                    
                    AVP_longlong volatile*  plData,
                    AVP_longlong            lSrc);

KLCSC_DECL void KLSTD_GetAtomicLock(KLSTD::CriticalSection** ppCS);

namespace KLSTD
{
    class LockCount
    {
    public:
        LockCount()
            :   m_lCount(0)
        {;}

        ~LockCount()
        {
            ;
        }

        void Lock()
        {
            KLSTD_InterlockedIncrement(&m_lCount);
        }

        void Unlock()
        {
            KLSTD_InterlockedDecrement(&m_lCount);
        }
        
        void Wait()
        {
            while(m_lCount)
                KLSTD_Sleep(100);
        }

        operator long()
        {
            return m_lCount;
        }

    protected:
        volatile long   m_lCount;
    };

    class CAutoLockCount
    {
    public:
        CAutoLockCount(LockCount& obj)
            :   m_obj(obj)
        {
            m_obj.Lock();
        };
        ~CAutoLockCount()
        {
            m_obj.Unlock();
        }
    protected:
        LockCount&  m_obj;
    };
    
    class ModuleClass
    {
    public:
        ModuleClass(KLSTD::LockCount* pLockCount)
            :   m_pLockCount(pLockCount)
        {
            if(m_pLockCount)
                m_pLockCount->Lock();
        };

        virtual ~ModuleClass()
        {
            if(m_pLockCount)
                m_pLockCount->Unlock();
        };
    protected:
        KLSTD::LockCount* m_pLockCount;
    };
    
    class CAutoDecrementor
    {
    public:
        CAutoDecrementor(volatile long& lVar)
            :   m_lVar(lVar)
        {
            KLSTD_InterlockedIncrement(&m_lVar);
        }
        ~CAutoDecrementor()
        {
            KLSTD_InterlockedDecrement(&m_lVar);
        };
    protected:
        volatile long& m_lVar;
    };

    inline KLSTD::CAutoPtr<KLSTD::CriticalSection> CreateCS()
    {
        KLSTD::CAutoPtr<KLSTD::CriticalSection> pCS;
        KLSTD_CreateCriticalSection(&pCS);
        return pCS;
    };

    inline KLSTD::CAutoPtr<KLSTD::CriticalSection> GetAtomicLock()
    {
        KLSTD::CAutoPtr<KLSTD::CriticalSection> pResult;
        KLSTD_GetAtomicLock(&pResult);
        return pResult;
    };

    class ObjectLock
    {
    public:
        ObjectLock(KLSTD::CriticalSection* pCS = GetAtomicLock())
            :   m_lCount(0)
            ,   m_bAllowFlag(false)
            ,   m_pCS(pCS)
        {
            if(!m_pCS)
                KLSTD_CreateCriticalSection(&m_pCS);
        };

        ~ObjectLock()
        {
            ;
        }

        bool Lock()
        {
            KLSTD::AutoCriticalSection acs(m_pCS);
            if(!m_bAllowFlag)
                return false;
            ++m_lCount;
            return true;
        }

        void Unlock()
        {
            KLSTD::AutoCriticalSection acs(m_pCS);
            --m_lCount;
        };

        void Disallow()
        {
            {
                KLSTD::AutoCriticalSection acs(m_pCS);
                m_bAllowFlag = false;
            };
        }

        void Allow()
        {
            {
                KLSTD::AutoCriticalSection acs(m_pCS);
                m_bAllowFlag = true;
            };
        }
        
        void Wait()
        {
            for(;;)
            {
                {
                    KLSTD::AutoCriticalSection acs(m_pCS);
                    if(m_lCount == 0)
                        break;;
                };
                KLSTD_Sleep(100);
            };
        };
    protected:
        volatile long                           m_lCount;
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;
        bool                                    m_bAllowFlag;
    };

    class CAutoObjectLock
    {
    public:
        CAutoObjectLock(ObjectLock& obj)
            :   m_obj(obj)
        {
            m_bResult = m_obj.Lock();
        };
        operator bool()
        {
            return m_bResult;
        }
        ~CAutoObjectLock()
        {
            if(m_bResult)
                m_obj.Unlock();
        }
    protected:
        ObjectLock& m_obj;
        bool        m_bResult;
    };

    class CReentProtect
    {
     public:
        CReentProtect()
         :  m_lCounter(0)
        {;};

        long Enter()
        {
            return (KLSTD_InterlockedCompareExchange(&m_lCounter, 1, 0) == 0) ? 1 : 0;
        };

        void Leave(long lResult)
        {
            if(lResult)
                KLSTD_InterlockedDecrement(&m_lCounter);
        };
     protected:
        volatile long  m_lCounter;
    };

    class CAutoReentProtect
    {
    public:
        CAutoReentProtect(CReentProtect& rp)
            :   m_rp(rp)
            ,   m_lResult(m_rp.Enter())
        {;}
        ~CAutoReentProtect()
        {
            m_rp.Leave(m_lResult);
        };
        operator bool () const
        {
            return m_lResult != 0;
        };
    protected:
        CReentProtect&  m_rp;
        const long      m_lResult;
    };

    class CReentProtectRW
    {
     public:
        CReentProtectRW()
         :  m_lCounter(0)
        {;};

        long Enter(bool bExclusive)
        {
            if(bExclusive)
                return (KLSTD_InterlockedCompareExchange(&m_lCounter, -1, 0) == 0) ? -1 : 0;
            else
                return (KLSTD_InterlockedIncrementIfGreaterOrEqualThanZero(&m_lCounter) >= 0) ? 1 : 0;
        };

        void Leave(long lResult)
        {            
            if(lResult > 0)
            {
                KLSTD_ASSERT(m_lCounter >= 0);
                KLSTD_InterlockedDecrement(&m_lCounter);
            }
            else if(lResult < 0)
            {
                KLSTD_ASSERT(m_lCounter == -1);
                KLSTD_InterlockedIncrement(&m_lCounter);
            };
        };
     protected:
        volatile long  m_lCounter;
    };

    class CAutoReentProtectRW
    {
    public:
        CAutoReentProtectRW(CReentProtectRW& rp, bool bExclusive)
            :   m_rp(rp)
            ,   m_lResult(m_rp.Enter(bExclusive))
        {;}
        ~CAutoReentProtectRW()
        {
            m_rp.Leave(m_lResult);
        };
        operator bool () const
        {
            return m_lResult != 0;
        };
    protected:
        CReentProtectRW&    m_rp;
        const long          m_lResult;
    };
    


    //! state of MtFlag object
    class MtFlagState
    {
        friend class MtFlag;
    public:
        MtFlagState()
            :   m_lVer(0)
            ,   m_lVerReq(0)
        {;};
        bool IfSet()
        {
            return m_lVer < m_lVerReq;
        };
    protected:        
        long m_lVer;
        long m_lVerReq;
    };

    //! multi threaded flag: supports one or more notifiers (who sets flag) and ONE processor ( who clears flag with Done() )
    class MtFlag
    {        
    public:
        MtFlag()
            :   m_lVer(0)
            ,   m_lVerReq(0)
        {;};
        ~MtFlag()
        {
            ;
        };

        //! reconstructs the object
        void Reset()
        {
            m_lVer = m_lVerReq = 0;
        };

        //! marks object to active state
        void Notify()
        {
            KLSTD_InterlockedIncrement(&m_lVerReq);
        };

        //! returns object state
        MtFlagState GetVal()
        {
            MtFlagState res;
            res.m_lVer = m_lVer;
            res.m_lVerReq = m_lVerReq;//must be last
            return res;
        };

        //! marks to inactive state relative to corresponding GetVal()
        void Done(MtFlagState& val)
        {
            if(val.IfSet())
                m_lVer = val.m_lVerReq;
        };
    protected:
        volatile long m_lVer;
        volatile long m_lVerReq;
    };

    class CAutoMtFlag
    {
    public:
        CAutoMtFlag(MtFlag& oMtFlag)
            :   m_oMtFlag(oMtFlag)
            ,   m_oVal(oMtFlag.GetVal())
        {
            ;
        };
        ~CAutoMtFlag()
        {
            ;
        };
        operator bool()
        {
            return m_oVal.IfSet();
        };
        void Done()
        {
            m_oMtFlag.Done(m_oVal);
        };
    protected:
        MtFlag&     m_oMtFlag;
        MtFlagState m_oVal;
    };
};

#endif //__KL_LOCKS_H__
