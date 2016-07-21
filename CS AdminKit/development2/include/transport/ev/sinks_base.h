/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	sinks_base.h
 * \author	Andrew Kazachkov & Andrew Lashenkov
 * \date	06.02.2004 10:56:31
 * \brief	
 * 
 */

#ifndef __KL_SINKS_BASE_H__
#define __KL_SINKS_BASE_H__

#include <transport/ev/eventsource.h>

namespace KLEV
{
	KLSTD_DECLARE_HANDLE(HSINKID); // KLEV::HSINKID

    class KLSTD_NOVTABLE  EvSink: public KLSTD::KLBaseQI
    {
        public:
            virtual void OnEvent(
                        const KLPRCI::ComponentId&  subscriber, 
                        const KLPRCI::ComponentId&  publisher,        
                        const KLEV::Event*                event) = 0;
    };

    struct subscription_data_t        
    {
        subscription_data_t()
        {
            m_subOptions.useSubscriberConnectionsOnly = true;
            m_subOptions.useOnlyLocalConnection = false;
        };
        subscription_data_t(const subscription_data_t& x)
            :   m_wstrEventType(x.m_wstrEventType)
            ,   m_pEventbodyFilter(x.m_pEventbodyFilter)
            ,   m_subOptions(x.m_subOptions)
            ,   m_idPublisher(x.m_idPublisher)
        {}

        std::wstring                    m_wstrEventType;
        KLSTD::CAutoPtr<KLPAR::Params>  m_pEventbodyFilter;
        KLEV::SubscriptionOptions       m_subOptions;
        KLPRCI::ComponentId             m_idPublisher;
    };

    class KLSTD_NOVTABLE  EvSinks : public KLSTD::KLBaseQI
    {
        public:
            /*!
              \brief	Добавляет подписку
                wstrEventType       [in]  Тип события
                pEventbodyFilter    [in]  Фильтр тела события
                pSink               [in]
                hSink               [out] дескриптор
                subOptions          [in]  опции подписки (см. EV\EventSource.h)
                pPublisher          [in]  публикатор, если NULL, то сервер
              \return	
            */
            virtual void AddSubscription(
                std::vector<subscription_data_t>&	vecData,
                KLEV::EvSink*	pSink,
                KLEV::HSINKID&	hSink) = 0;

            KLSTD_NOTHROW virtual bool RemoveSubscription(KLEV::HSINKID hSink) throw()= 0;

            virtual void GetServerCID(KLPRCI::ComponentId& idComponent) = 0;
            virtual void GetProxyCID(KLPRCI::ComponentId& idComponent) = 0;
    };        

    class KLSTD_NOVTABLE  AdviseEvSink : public KLSTD::KLBaseQI
    {
    public:
        virtual void OnNotify(
            long                lId,
            KLSTD::KLBaseQI*    pResults,
            KLERR::Error*       pError) = 0;
    };

};

#endif //__KL_SINKS_BASE_H__
