#include <std/base/klstd.h>
#include <std/thr/locks.h>
#include <std/tp/tpcmdqueue.h>


#ifndef __QUEUED_ACTIONS_H__
#define __QUEUED_ACTIONS_H__

namespace KLSTDQUEUE
{
    class QueuedAction
        :   public KLSTD::KLBase
    {
    public:
        virtual void Process() = 0;
    };

    typedef KLSTD::CAutoPtr<QueuedAction> QueuedActionPtr;

    typedef std::pair<  KLSTD::KLAdapt<QueuedActionPtr>,
                        KLSTD::KLAdapt< 
                            KLSTD::CAutoPtr<KLSTD::Semaphore> > >   
                                                        queued_action_t;

    class QueuedActionsHandler;

    typedef KLCMDQUEUE::CmdQueue<
                    queued_action_t, 
                    queued_action_t, 
                    QueuedActionsHandler> CActionsQueue;

    class QueuedActionsHandler
        :   public KLSTD::KLBase
    {
    public:
        QueuedActionsHandler();
        
        virtual ~QueuedActionsHandler();

        virtual void QueuedActions_Create(
                    KLSTD::ObjectLock&      lckExt, 
                    KLSTD::CriticalSection* pCS);

        virtual KLSTD_NOTHROW void QueuedActions_Destroy() throw();        

        virtual void PostCommand(QueuedActionPtr pCmd);

        virtual void SendCommand(QueuedActionPtr pCmd, long lTimeout = KLSTD_INFINITE);
    protected:

        virtual void OnCommand(queued_action_t oAction);
    protected:
        KLSTD::ObjectLock*                      m_plckExt;
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pDataCS;
        KLSTD::CPointer<CActionsQueue>          m_pEventQueue;
    };
    

    template<class T>
        class Cmd_CallMethod0
            :   public KLSTD::KLBaseImpl<KLSTDQUEUE::QueuedAction>
        {
        public:
            typedef void (T::*func_t)();

            Cmd_CallMethod0(     T*      pThis, 
                                func_t  pMethod )
                :   m_pThis(pThis)
                ,   m_pMethod(pMethod)
            {;};
            
            virtual ~Cmd_CallMethod0()
            {;};

            static QueuedActionPtr Instantiate(     
                                T* pThis, 
                                func_t  pMethod)
            {
                QueuedActionPtr pCmd;
                pCmd.Attach(new Cmd_CallMethod0(pThis, pMethod));
                //KLSTD_CHKMEM(pCmd);
                return pCmd;
            };

            KLSTD_SINGLE_INTERAFCE_MAP(KLSTDQUEUE::QueuedAction)

            virtual void Process()
            {
                ((*m_pThis).*m_pMethod)();
            };
        protected:
            T*      m_pThis;
            func_t  m_pMethod;
        };

    template<class T, class A1>
        class Cmd_CallMethod1
            :   public KLSTD::KLBaseImpl<KLSTDQUEUE::QueuedAction>
        {
        public:
            typedef void (T::* func_t )(A1 oArg1);


            Cmd_CallMethod1(     T*      pThis, 
                                func_t  pMethod,
                                A1      oArg1)
                :   m_pThis(pThis)
                ,   m_pMethod(pMethod)
                ,   m_oArg1(oArg1)
            {;};
            
            virtual ~Cmd_CallMethod1()
            {;};

            static QueuedActionPtr Instantiate(     
                                T*      pThis, 
                                func_t  pMethod,
                                A1      oArg1)
            {
                QueuedActionPtr pCmd;
                pCmd.Attach(new Cmd_CallMethod1(pThis, pMethod, oArg1));
                //KLSTD_CHKMEM(pCmd);
                return pCmd;
            };

            KLSTD_SINGLE_INTERAFCE_MAP(KLSTDQUEUE::QueuedAction)

            virtual void Process()
            {
                ((*m_pThis).*m_pMethod)(m_oArg1);
            };
        protected:            
            T*      m_pThis;
            func_t  m_pMethod;
            A1      m_oArg1;
        };
};

#endif //__QUEUED_ACTIONS_H__
