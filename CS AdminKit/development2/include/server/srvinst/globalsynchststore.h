#ifndef __KL_GLOBALSYNCHSTSTORE_H__
#define __KL_GLOBALSYNCHSTSTORE_H__

#include <map>
#include <list>

#include ".\globalbcaster.h"

namespace KLSRVBCASTER
{
    const long c_nMaxErrors = 1;

    KLSTD_DECLARE_HANDLE(HLOCKEDHOST);

    class CHostInfo : public KLSTD::KLBaseImpl<KLSTD::KLBase>
    {
    public:
        CHostInfo()
            :   m_dwFlags(0)
            ,   m_bLocked(false)
            ,   m_lErrorCount(c_nMaxErrors)
            ,   m_clkLastErrorTime(0)
        {;};

        bool get_Locked()
        {
            return m_bLocked;
        };

        void put_Locked(bool bLocked)
        {
            m_bLocked = bLocked;
        };

        void put_flags(AVP_dword dwMask, AVP_dword dwFlags)
        {            
            m_dwFlags = (m_dwFlags & ~dwMask)|(dwMask & dwFlags);
        };

        AVP_dword get_flags()
        {
            return m_dwFlags;
        };
        long get_HostId(){return m_lHostId;};
        void put_HostId(long idHost){m_lHostId = idHost;};
        long get_ErrorCount(){return m_lErrorCount;};
        void put_ErrorCount(long lErrorCount)
        {
            m_lErrorCount = lErrorCount;
            m_clkLastErrorTime = (long)KLSTD::GetSysTickCount();
        };
        long get_LastErrorTime()
        {
            return m_clkLastErrorTime;
        };
    protected:
        long            m_clkLastErrorTime;
        long            m_lHostId;
        AVP_dword       m_dwFlags;
        long            m_lErrorCount;
        bool            m_bLocked;
    private:
        CHostInfo(const CHostInfo& x);
    };

    typedef KLSTD::CAutoPtr<CHostInfo> CHostInfoPtr;

    class CGlobalSyncHstStore
    {
    public:
        CGlobalSyncHstStore();
        ~CGlobalSyncHstStore();

        /*!
          \brief	Returns number of elements in storage. Very fast.
        */
        long get_Size()
        {
            return m_lSize;
        };

        /*!
          \brief	Returns true if storage has available (unlocked) hosts.
        */
        bool hasAvailable()
        {        
            KLSTD_ASSERT(m_lUnlocked >= 0);
            return m_lUnlocked != 0;
        }

        /*!
          \brief	Tries to lock first (according to priorities) host

          \param	idHost - locked host id
          \param	hHost - handle for host locked
          \return	bool - true if successful
        */
        bool TryToLockFirstAvailable(
                            long&           idHost,
                            HLOCKEDHOST&    hHost);

        /*!
          \brief	Tries to lock specified host

          \param	idHost  - id of host to lock
          \param	hHost - handle for host locked
          \return	bool - true if successful
        */
        bool TryToLockHost(long idHost, HLOCKEDHOST& hHost);

        /*!
          \brief	Unlocks host previosly locked by TryToLockFirstAvailable or TryToLockHost

          \param	hHost - handle for host locked
        */
        void UnlockHost(HLOCKEDHOST hHost);

        /*!
          \brief	Returns sync flags
        */       
        AVP_dword GetSyncFlag(HLOCKEDHOST hHost);

        /*!
          \brief	Changes sync flags
        */       
        void SetSyncFlag(
                        HLOCKEDHOST hHost,
                        AVP_dword dwMask,
                        AVP_dword dwFlags);

        //! equivalent to SetSyncFlag(hHost, dwFlagsToAdd, GetSyncFlag(hHost)|dwFlagsToAdd);
        void AddSyncFlag(HLOCKEDHOST hHost, AVP_dword dwFlagsToAdd);

        void ForceHost(HLOCKEDHOST hHost);        

        long GetHostId(HLOCKEDHOST hHost);
        
        //! increments error counter
        void SetErrorResult(HLOCKEDHOST hHost);

        void MarkHostForSync(
                        long        idHost,
                        AVP_dword   dwFlags,
                        bool        bForce);
    protected:
        typedef std::list<KLSTD::KLAdapt<CHostInfoPtr> >    hst_list_t;
        typedef std::map<long, hst_list_t::iterator>  hst_map_t;
        struct LockedHost
        {
            hst_list_t::iterator    m_itListPos;
            hst_map_t::iterator     m_itMapPos;
        };
        ;
        struct GluedHost
        {
            AVP_dword   m_dwFlags;
            bool        m_bForce;
        };
        typedef std::map<long, GluedHost>   glued_t;
        ;
        hst_list_t      m_lstHosts;
        hst_map_t       m_mapHosts;
        glued_t         m_mapGlued;
        ;
        volatile long   m_lSize, m_lLocked, m_lUnlocked;
        KLSTD::CAutoPtr<KLSTD::CriticalSection>     m_pCS;
    };

    class CHostAutoLocker
    {
    public:
        CHostAutoLocker(CGlobalSyncHstStore& x)
            :   m_store(x)
            ,   m_lHost(0)
            ,   m_hLock(NULL)
        {
            x.TryToLockFirstAvailable(m_lHost, m_hLock);
        };
        CHostAutoLocker(CGlobalSyncHstStore& x, long lHost)
            :   m_store(x)
            ,   m_lHost(lHost)
            ,   m_hLock(NULL)
        {
            x.TryToLockHost(lHost, m_hLock);
        };
        ~CHostAutoLocker()
        {
            if(m_hLock)
            {
                m_store.UnlockHost(m_hLock);
                m_hLock = NULL;
            };
        };
        operator bool()
        {
            return m_hLock != NULL;
        };
        long get_HostId(){return m_lHost;};
        HLOCKEDHOST get_Lock(){return m_hLock;};
    protected:
        CGlobalSyncHstStore&    m_store;
        HLOCKEDHOST             m_hLock;
        long                    m_lHost;
    };

};
#endif //__KL_GLOBALSYNCHSTSTORE_H__