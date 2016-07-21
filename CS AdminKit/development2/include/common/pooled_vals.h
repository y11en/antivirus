#ifndef __POOLED_VALS_H__
#define __POOLED_VALS_H__

namespace KLSTDEX
{
    template <class T, class P>
    class PooledKLBaseImpl
        :   public T
    {
    public:
        PooledKLBaseImpl(P* pPool)
            :   m_cRef(1)
            ,   m_pPool(pPool)
        {
            ;
        }

        virtual ~PooledKLBaseImpl()
        {
            KLSTD_ASSERT(m_cRef == 0);
        }

		virtual unsigned long AddRef()
		{
            //KLSTD::Trace(4, L"foo", L"foo-AddRef() = %u, this= 0x%X\n", 1+m_cRef, (void*)this);
			return KLSTD_InterlockedIncrement((long*)&m_cRef);
		}

		virtual unsigned long Release()
		{
            //KLSTD::Trace(4, L"foo", L"foo-Release() = %u, this= 0x%X\n", m_cRef-1, (void*)this);
			long cRef = KLSTD_InterlockedDecrement((long*)&m_cRef);
            KLSTD_ASSERT( cRef >= 0 );
			if(!cRef)
				m_pPool->ReturnInstance(this);
			return cRef;
		}

        void DestroyPooledInstance()
        {
            ((T*)this)->Destroy();
            delete this;
        }

        void CleanPooledInstance()
        {
            ((T*)this)->Destroy();
        }
    protected:
        volatile long   m_cRef;
        P*              m_pPool;
    };
    
    template <class V>
        class ValuesPool
    {
        public:
            typedef PooledKLBaseImpl<V, ValuesPool<V> > pooled_val_t;
            typedef KLSTD::CAutoPtr<pooled_val_t> pooled_val_ptr_t;
            typedef std::vector<pooled_val_t*> vec_pooled_val_t;
            
            ValuesPool()
                :   m_nMaxSize(0)
            {
                ;
            };

            virtual ~ValuesPool()
            {
                while(!m_vecValues.empty())
                {
                    m_vecValues.back()->DestroyPooledInstance();
                    m_vecValues.pop_back();
                };
            };

            void ValuesPoolInitialize(
                    KLSTD::CriticalSection* pCS, 
                    size_t                  nMaxSize)
            {
                m_pDataCS = pCS;
                m_nMaxSize = nMaxSize;
                if(m_nMaxSize)
                    m_vecValues.reserve(m_nMaxSize);
            };

            virtual void ReturnInstance(pooled_val_t* p)
            {
                if(p && m_nMaxSize)
                {
                    KLSTD::AutoCriticalSection acs(m_pDataCS);
                    if(m_vecValues.size() < m_nMaxSize)
                    {
                        p->CleanPooledInstance();
                        m_vecValues.push_back(p);
                        p = NULL;
                    };
                };
                if(p)
                    p->DestroyPooledInstance();
            };

            virtual void AllocInstance(pooled_val_t** ppVal)
            {
                pooled_val_ptr_t pResult;
                if(m_nMaxSize)
                {
                    KLSTD::AutoCriticalSection acs(m_pDataCS);
                    if(!m_vecValues.empty())
                    {
                        pResult = m_vecValues.back();
                        m_vecValues.pop_back();
                    }
                };
                if(!pResult)
                    pResult.Attach(new pooled_val_t(this));
                //pResult.CopyTo(ppVal);
                *ppVal = pResult.Detach();
            };

            virtual KLSTD::CAutoPtr<V> AllocInstance2()
            {
                pooled_val_ptr_t pResult;
                if(m_nMaxSize)
                {
                    KLSTD::AutoCriticalSection acs(m_pDataCS);
                    if(!m_vecValues.empty())
                    {
                        pResult = m_vecValues.back();
                        m_vecValues.pop_back();
                    }
                };
                if(!pResult)
                    pResult.Attach(new pooled_val_t(this));
                return (KLSTD::CAutoPtr<V>)(V*)pResult;
            };

        protected:
            size_t m_nMaxSize;
        protected:
            KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pDataCS;
            vec_pooled_val_t    m_vecValues;
    };
}

#endif //__POOLED_VALS_H__
