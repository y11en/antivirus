/*!
 * (C) 2007 Kaspersky Lab 
 * 
 * \file	evs_queued.h
 * \author	Andrew Kazachkov
 * \date	20.02.2007 13:25:23
 * \brief	Queued eventsource wrapper
 * 
 */

#ifndef __KLEVS_QUEUED_H__
#define __KLEVS_QUEUED_H__

#include <transport/ev/eventsource.h>
#include <transport/ev/sinks_base.h>

namespace KLEVQ
{
    class EventSourceQueued;
};


/*!
  \brief	Returns pointer to KLEVQ::EventSourceQueued

  \param	pp OUT pointer to variable that receives pointer to EventSourceQueued
  \throw	KLSTD::STDE_NOINIT if module is not initialized
*/
KLCSTR_DECL void KLEVQ_GetEventSourceQueued(KLEVQ::EventSourceQueued** pp);

namespace KLEVQ
{
    //! Queue handle. Invalid value for it is NULL
    KLSTD_DECLARE_HANDLE(HSUBSQUEUE);

    //! Subscription handle. Invalid value for it is NULL
    KLSTD_DECLARE_HANDLE(HSUBSCRIPTION);

    //! for ev compatibility only
    const HSUBSCRIPTION UndefinedSubId = (HSUBSCRIPTION)(-1L);

    typedef KLEV::SubscriptionOptions   SubOptions;


    typedef struct 
    {
        KLPRCI::ComponentId idSubscriber;
		KLPRCI::ComponentId idPublisher;
        std::wstring        wstrType;
		KLPAR::ParamsPtr    pEventBodyFilter;
        KLEVQ::SubOptions   oSubOptions;
    }
    subscription_t;

    
    /*! 
        Subscriptions with individual queues. Prevents blocking of eventsource. 
    */

	class EventSourceQueued
        :   public KLSTD::KLBaseQI
	{
    public:
        /*!
          \brief	Creates a queue
          \return	hQueue handle
        */
        virtual HSUBSQUEUE CreateQueue() = 0;

        /*!
          \brief	Destroys a queue. All existing queues subscriptions becomes invalid. 
          \param	hQueue IN queue handle
          \param	bWaitForQueueEmpty IN if true waits until subscription's queue becomes 
                                        empty otherwise events from queue will be lost
          \return	false if hQueue is invalid
        */
        KLSTD_NOTHROW virtual bool DestroyQueue(
                            HSUBSQUEUE  hQueue, 
                            bool        bWaitForQueueEmpty = false) throw() = 0;

        /*!
          \brief    Creates a subscription for a given queue. 
                    For compatibility with EV. See EventSource::Subscribe. 
        */
		virtual HSUBSCRIPTION Subscribe(
                    HSUBSQUEUE                  hQueue,
                    const KLPRCI::ComponentId&  subscriber,
				    const KLPRCI::ComponentId&  publisher, 
                    const std::wstring&         type, 
				    KLPAR::Params*              eventBodyFilter, 
                    KLEV::EventCallback         pCallback,
				    void*                       pCallbackContext,
                    const KLEVQ::SubOptions&    subOptions 
                                                    = KLEVQ::SubOptions() )= 0;


        virtual HSUBSCRIPTION SubscribeMultiple(
                        HSUBSQUEUE                  hQueue,
                        subscription_t*             pOptions,
                        size_t                      nOptions,
                        KLEV::EventCallback         pCallback,
				        void*                       pCallbackContext) = 0;
        /*!
          \brief	Unsubscribes and waits until event callback returns . 

          \param	hSubscription       IN subscription id
          \param	bWaitForQueueEmpty  IN if true waits until subscription's queue becomes 
                                        empty otherwise events from queue will be lost
          \return	false if no such subscription
        */
        KLSTD_NOTHROW virtual bool Unsubscribe( 
                    HSUBSQUEUE              hQueue,
                    KLEVQ::HSUBSCRIPTION    hSubscription) throw() = 0;
    };

    typedef KLSTD::CAutoPtr<KLEVQ::EventSourceQueued> EventSourceQueuedPtr;


    /*!
      \brief	wrapper around KLEVQ_GetEventSourceQueued

      \return	EventSourceQueuedPtr pointer
    */
    inline EventSourceQueuedPtr GetEventSourceQueued()
    {
        EventSourceQueuedPtr pResult;
        KLEVQ_GetEventSourceQueued(&pResult);
        return pResult;
    }
};

#endif //__KLEVS_QUEUED_H__
