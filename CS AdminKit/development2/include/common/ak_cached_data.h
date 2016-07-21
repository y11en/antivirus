#ifndef __AK_CACHED_DATA_H__
#define __AK_CACHED_DATA_H__

#include <std/base/klstd.h>
#include <hash_map>
#include <map>

namespace KLSTD
{
    template<class Data_, class Helper_, class Key_, class F_>
        class PairCachePropsHash
    {
    public:
        typedef typename Data_   Data;
        typedef typename Helper_ Helper;
        typedef typename Key_    Key;
        typedef typename F_      F;

        typedef std::hash_map<Key, Data>                    hmap_key2data_t;
        typedef typename hmap_key2data_t::iterator          it_hmap_key2data_t;
        typedef typename hmap_key2data_t::value_type        val_hmap_key2data_t;
    };

    template<class Data_, class Helper_, class Key_, class F_>
        class PairCachePropsBtree
    {
    public:
        typedef typename Data_   Data;
        typedef typename Helper_ Helper;
        typedef typename Key_    Key;
        typedef typename F_      F;

        typedef std::map<Key, Data>                         hmap_key2data_t;
        typedef typename hmap_key2data_t::iterator          it_hmap_key2data_t;
        typedef typename hmap_key2data_t::value_type        val_hmap_key2data_t;
    };

    //maps A1 to Data, Helper - helper arg
    template<class PP >
    class PairCache
    {
    public:
        PairCache(size_t nSize)
            :   m_mapKey2Data(nSize)
        {;};

        PairCache()
        {;};

        void Create(PP::F f, KLSTD::CriticalSection* pCS)
        {
            m_pDataCS = pCS;
            KLSTD_ASSERT(m_pDataCS);
        };

        PP::Data operator()(PP::Helper ah, const PP::Key& a)
        {
            KLSTD_ASSERT(m_pDataCS);
            PP::Data result = PP::Data();
            bool bAcquired = false;
            {
                KLSTD::AutoCriticalSection acs(m_pDataCS);
                PP::it_hmap_key2data_t it = m_mapKey2Data.find(a);
                if( m_mapKey2Data.end() != it )
                {
                    result = it->second;
                    bAcquired = true;
                };
            };
            if(!bAcquired)
            {
                result = m_f(ah, a);
                {
                    KLSTD::AutoCriticalSection acs(m_pDataCS);
                    m_mapKey2Data.insert(PP::val_hmap_key2data_t(a, result));
                };
            };
            return result;
        };

        void ResetCache(const PP::Key& a)
        {
            KLSTD_ASSERT(m_pDataCS);
            KLSTD::AutoCriticalSection acs(m_pDataCS);
            m_mapKey2Data.erase(a);
        };
        
        void ResetCache()
        {
            KLSTD_ASSERT(m_pDataCS);
            KLSTD::AutoCriticalSection acs(m_pDataCS);
            m_mapKey2Data.clear();
        };

        void UpdateCache(const PP::Key& a, const PP::Data& r)
        {
            KLSTD_ASSERT(m_pDataCS);
            PP::Data Old = PP::Data();
            {
                KLSTD::AutoCriticalSection acs(m_pDataCS);
                PP::it_hmap_key2data_t it = m_mapKey2Data.find(a);
                if(it != m_mapKey2Data.end())
                {
                    Old = it->second;
                    m_mapKey2Data.erase(it);
                };
                m_mapKey2Data.insert(PP::val_hmap_key2data_t(a, r));
            };
            Old = PP::Data();
        };
    protected:
        //cached data

        KLSTD::CAutoPtr<KLSTD::CriticalSection>     m_pDataCS;
        PP::hmap_key2data_t                         m_mapKey2Data;
        PP::F                                       m_f;
    };
};

#endif //__AK_CACHED_DATA_H__
