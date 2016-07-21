#ifndef __AK_CACHED_DATA_H__
#define __AK_CACHED_DATA_H__

#include <std/base/klstd.h>
#include <hash_map>
#include <map>
#include <ctime>

namespace KLSTD
{
    template<class Data_, class Helper_, class Key_, class F_>
    class PairCacheTmtHash
    {
    public:
        typedef typename Data_   Data;
        typedef typename Helper_ Helper;
        typedef typename Key_    Key;
        typedef typename F_      F;

        typedef std::multimap<time_t, Key>                  map_time2key_t;//use less -- it is important !!!
        typedef KLSTD_TYPENAME map_time2key_t::iterator     it_map_time2key_t;
        typedef KLSTD_TYPENAME map_time2key_t::value_type   val_map_time2key_t;

        typedef std::pair<Data, it_map_time2key_t>          fulldata_t;
        typedef std::hash_map<Key, fulldata_t>              hmap_key2data_t;
        typedef KLSTD_TYPENAME hmap_key2data_t::iterator    it_hmap_key2data_t;
        typedef KLSTD_TYPENAME hmap_key2data_t::value_type  val_hmap_key2data_t;
    };

    template<class Data_, class Helper_, class Key_, class F_>
    class PairCacheTmtBtree
    {
    public:
        typedef typename Data_   Data;
        typedef typename Helper_ Helper;
        typedef typename Key_    Key;
        typedef typename F_      F;

        typedef std::multimap<time_t, Key>                  map_time2key_t;//use less -- it is important !!!
        typedef KLSTD_TYPENAME map_time2key_t::iterator     it_map_time2key_t;
        typedef KLSTD_TYPENAME map_time2key_t::value_type   val_map_time2key_t;

        typedef std::pair<Data, it_map_time2key_t>          fulldata_t;
        typedef std::map<Key, fulldata_t>                   hmap_key2data_t;
        typedef KLSTD_TYPENAME hmap_key2data_t::iterator    it_hmap_key2data_t;
        typedef KLSTD_TYPENAME hmap_key2data_t::value_type  val_hmap_key2data_t;
    };

    //maps A1 to Data, Helper - helper arg
    template<class PP>
    class PairCacheTmt
    {
    public:
        PairCacheTmt(size_t nSize)
            :   m_mapKey2Data(nSize)
        {;};

        PairCacheTmt()
        {;};
//#if 0

        void Create(PP::F f, KLSTD::CriticalSection* pCS)
        {
            m_pDataCS = pCS;
            KLSTD_ASSERT(m_pDataCS);
        };

        PP::Data operator()(PP::Helper ah, const PP::Key& a)
        {
            KLSTD_ASSERT(m_pDataCS);
            PP::Data result = PP::Data();
            bool bAcquired = false;;
            {
                KLSTD::AutoCriticalSection acs(m_pDataCS);
                PP::it_hmap_key2data_t it = m_mapKey2Data.find(a);
                if( m_mapKey2Data.end() != it )
                {   // already exists
                    result = it->second.first;
                    bAcquired = true;
                };
            };
            if(!bAcquired)
            {
                result = m_f(ah, a);
                AddItem(a, result);
            };
            return result;
        };

        void ResetCache(const PP::Key& k)
        {
            KLSTD_ASSERT(m_pDataCS);
            KLSTD::AutoCriticalSection acs(m_pDataCS);
            DelItemNoCS(k);
        };

        void ResetCache()
        {
            KLSTD_ASSERT(m_pDataCS);
            KLSTD::AutoCriticalSection acs(m_pDataCS);
            m_mapKey2Data.clear();
            m_mapTime2Key.clear();
        };

        void UpdateCache(const PP::Key& k, PP::Data r)
        {
            KLSTD_ASSERT(m_pDataCS);
            AddItem(k, r);
        };

        /*!
          \brief	Removes item older than t seconds
          \param	t IN timeout in seconds
          \return	true if removed smth
        */
        bool RemoveOld(size_t t)
        {
            KLSTD_ASSERT(m_pDataCS);
            bool bResult = false;
            if( 0 == t)
                t = 1;
            const time_t tmNow = time(NULL);
            while(true)
            {
                KLSTD::AutoCriticalSection acs(m_pDataCS);
                if(m_mapTime2Key.empty())
                    break;
                PP::it_map_time2key_t it = m_mapTime2Key.begin();
                const time_t tmItem = it->first;
                if( tmNow >= tmItem +  time_t(t) )
                {
                    DelItemNoCS(it->second);
                    bResult = true;
                }
                else
                    break;
            };
            return bResult;
        };

        bool IfExists(const PP::Key& k)
        {
            KLSTD_ASSERT(m_pDataCS);
            KLSTD::AutoCriticalSection acs(m_pDataCS);
            return (m_mapKey2Data.end() != m_mapKey2Data.find(k));
        };

        void Prolong(const PP::Key& k)
        {
            KLSTD_ASSERT(m_pDataCS);
            const time_t tmNow = time(NULL);
            KLSTD::AutoCriticalSection acs(m_pDataCS);
            PP::it_hmap_key2data_t it = m_mapKey2Data.find(k);
            if(it != m_mapKey2Data.end())
            {
                if(it->second.second != m_mapTime2Key.end())
                {
                    m_mapTime2Key.erase(it->second.second);
                    it->second.second = m_mapTime2Key.end();
                };
                it->second.second = m_mapTime2Key.insert(PP::val_map_time2key_t(tmNow, k));
            };
        };
//#endif
    protected:
//#if 0
        void AddItem(const PP::Key& k, const PP::Data& d)
        {
            const time_t tmNow = time(NULL);
            {
                KLSTD::AutoCriticalSection acs(m_pDataCS);
                PP::it_hmap_key2data_t it = m_mapKey2Data.find(k);
                if(it != m_mapKey2Data.end())
                    DelItemNoCS(it);
                it = m_mapKey2Data.insert(PP::val_hmap_key2data_t(k, PP::fulldata_t(d, m_mapTime2Key.end()))).first;
                it->second.second = m_mapTime2Key.insert(PP::val_map_time2key_t(tmNow, k));
            };
        };

        void DelItemNoCS(const PP::Key& k)
        {
            DelItemNoCS(m_mapKey2Data.find(k));
        };

        void DelItemNoCS(PP::it_hmap_key2data_t it)
        {
            if(it != m_mapKey2Data.end())
            {
                if(it->second.second != m_mapTime2Key.end())
                    m_mapTime2Key.erase(it->second.second);
                m_mapKey2Data.erase(it);
            };
        };
//#endif
    protected:        
        KLSTD::CAutoPtr<KLSTD::CriticalSection>     m_pDataCS;
        PP::hmap_key2data_t                         m_mapKey2Data;
        PP::map_time2key_t                          m_mapTime2Key;
        PP::F                                       m_f;
    };
};

#endif //__AK_CACHED_DATA_H__
