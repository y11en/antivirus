#ifndef __KL_TPCMDQUEUE_H__
#define __KL_TPCMDQUEUE_H__

#include <std/thr/locks.h>
#include <std/err/klerrors.h>
#include <std/tp/threadspool.h>

namespace KLCMDQUEUE
{

    /*
        template<class T, class A, class S>
        class CmdQueue
            :   public KLSTD::KLBaseImpl<KLTP::ThreadsPool::Worker>
        {
        public:
            CmdQueue(   KLSTD::CriticalSection* pDataCS = NULL, 
                        bool bSingleThreaded = false);

            virtual ~CmdQueue();

            void Create(S* pObject, func_t f);

            void Destroy();

            void Run();

            bool Send(A x);
        };
    */


    template<class T, class A, class S>
    class CmdQueue
    {
    protected:
        class CmdQueueWorker
        {
        public:
            virtual void Awake() = 0;
            virtual void OnDestroyed() = 0;
            virtual void Run() = 0;
        };

        template<class P>
        class CmdQueueWorkerTp
            :   public  KLSTD::KLBaseImpl<KLTP::ThreadsPool::Worker>
            ,   public  CmdQueueWorker
        {
        public:
            CmdQueueWorkerTp(P* pOwner)
                :   m_pOwner(pOwner)
            {
                ;
            };

            virtual ~CmdQueueWorkerTp()
            {
                ;
            };

            void Run()
            {
                ;
            };

            void Awake()
            {
                KLTP::ThreadsPool::WorkerId idWorker=0;
			    KLTP_GetThreadsPool()->AddWorker(
										    &idWorker,
										    L"KLCMDQUEUE::CmdQueueWorker",
										    this,
										    KLTP::ThreadsPool::RunOnce);
            };

            void OnDestroyed()
            {
                ;//do nothing
            };

        protected:
		    virtual int RunWorker( KLTP::ThreadsPool::WorkerId wId )
		    {
                m_pOwner->Invoke();
                return 0;
		    };
        protected:
            P*                  m_pOwner;
        };

        template<class P>
        class CmdQueueWorkerTh
            :   public  CmdQueueWorker
        {
        public:
            CmdQueueWorkerTh(P* pOwner)
                :   m_pOwner(pOwner)
                ,   m_cntAwake(0)
            {
                KLSTD_CreateSemaphore(&m_pSemaphore, 0);
            };

            virtual ~CmdQueueWorkerTh()
            {
                ;
            };

            void Run()
            {
                for(;;)
                {
                    m_pSemaphore->Wait(KLSTD_INFINITE);
                    if(KLSTD_InterlockedDecrement(&m_cntAwake) < 0)
                        break;
                    m_pOwner->Invoke();
                };
            };

            void Awake()
            {
                KLSTD_InterlockedIncrement(&m_cntAwake);
                m_pSemaphore->Post();
            };

            void OnDestroyed()
            {
                m_pSemaphore->Post();
            };
        
        protected:
            P*                                  m_pOwner;
            volatile long                       m_cntAwake;
            KLSTD::CAutoPtr<KLSTD::Semaphore>   m_pSemaphore;
        };
        typedef CmdQueueWorkerTp< CmdQueue<T, A, S> > tpworker_t;
        typedef CmdQueueWorkerTh< CmdQueue<T, A, S> > thworker_t;

    public:
        typedef void (S::*func_t)(A x);
        CmdQueue(KLSTD::CriticalSection* pDataCS = NULL, bool bSingleThreaded = false)
            :   m_pMethod(NULL)
            ,   m_bStopFlag(false)
            ,   m_lWorkers(0)
            ,   m_bInitiallyInactive(false)
        {
            if(pDataCS)
                m_pCS = pDataCS;
            else
                KLSTD_CreateCriticalSection(&m_pCS);
            if(bSingleThreaded)
            {
                m_pThWoker = new thworker_t(this);
                KLSTD_CHKMEM(m_pThWoker);
                m_pWorker = m_pThWoker;
            }
            else
            {
                m_pTpWoker.Attach(new tpworker_t(this));
                KLSTD_CHKMEM(m_pTpWoker);
                m_pWorker = m_pTpWoker;
            };            
        };

        virtual ~CmdQueue()
        {
            ;
        };

        void Create(S* pObject, func_t f, bool bInitiallyInactive = false)
        {
            m_pObject = pObject;
            m_pMethod = f;
            m_bInitiallyInactive = bInitiallyInactive;
        };

        void Destroy()
        {
            {
                KLSTD::AutoCriticalSection acs(m_pCS);
                m_bStopFlag = true;
            };
            for(;;)
            {
                {
                    KLSTD::AutoCriticalSection acs(m_pCS);
                    if(m_lWorkers == 0)
                        break;
                };
#ifdef N_PLAT_NLM
                ThreadSwitch();
#else
				KLSTD_Sleep(100);
#endif

            };
            m_pWorker->OnDestroyed();
            m_pObject = NULL;
            m_pMethod = NULL;
        };

        void Activate()
        {
            {
                KLSTD::AutoCriticalSection acs(m_pCS);
                if(m_bStopFlag)
                    return;
                m_bInitiallyInactive = false;
                if(m_Queue.empty())
                    return;
                //if(m_lWorkers != 0)
                    //return;
                ++m_lWorkers;
            };
            try
            {
                m_pWorker->Awake();
            }
            catch(...)
            {
                KLSTD::AutoCriticalSection acs(m_pCS);
                --m_lWorkers;
                throw;
            };
        };

        void Run()
        {
            m_pWorker->Run();
        };

        bool Send(A x)
        {
            {
                KLSTD::AutoCriticalSection acs(m_pCS);
                if(m_bStopFlag)
                    return false;
                m_Queue.push(x);
                if(m_lWorkers != 0 || m_bInitiallyInactive)
                    return true;
                ++m_lWorkers;
            };
            try
            {
                m_pWorker->Awake();
            }
            catch(...)
            {
                KLSTD::AutoCriticalSection acs(m_pCS);
                --m_lWorkers;
                throw;
            };
            return true;
        };

        void Invoke()
        {
            for(;;)
            {
                try
                {
					T cmd;
                    {
                        KLSTD::AutoCriticalSection acs(m_pCS);
                        if(m_Queue.empty()/*|| m_bStopFlag*/)
                        {
                            --m_lWorkers;
                            break;
                        };
                        cmd = m_Queue.front();
                        m_Queue.pop();
                    };
                    ((*m_pObject).*m_pMethod)(cmd);
                }
                catch(KLERR::Error* pError)
                {
                    if(pError)
                        pError->Release();
                };
            };
		};
		
		size_t GetLength()
		{
			KLSTD::AutoCriticalSection acs(m_pCS);
			return m_Queue.size();
		};
    protected:
        KLSTD::CAutoPtr<tpworker_t>                     m_pTpWoker;
        KLSTD::CPointer<thworker_t>                     m_pThWoker;
        CmdQueueWorker*                                 m_pWorker;

        volatile long                                   m_lWorkers;
		KLSTD::CAutoPtr<S>	                            m_pObject;
		func_t				                            m_pMethod;
        KLSTD::CAutoPtr<KLSTD::CriticalSection>         m_pCS;
        std::queue<T>                                   m_Queue;
        bool                                            m_bStopFlag;
        volatile bool                                   m_bInitiallyInactive;
    };
};


#endif //__KL_TPCMDQUEUE_H__
