/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	sinks_stuff.h
 * \author	Andrew Kazachkov & Andrew Lashenkov
 * \date	06.02.2004 10:56:31
 * \brief	
 * 
 */

#ifndef __KL_SINKS_STUFF_H__
#define __KL_SINKS_STUFF_H__

#include <map>
#include <std/base/klbaseqi_imp.h>
#include <transport/ev/eventsource.h>
#include <transport/ev/sinks_base.h>

namespace KLEV
{
    class CEvSinkEntry : public KLSTD::KLBaseImpl<KLSTD::KLBase>
    {
    public:
        CEvSinkEntry(EvSink* pSink);
        void Unsubscribe();

        static void EventCallback(
            const KLPRCI::ComponentId&  subscriber, 
            const KLPRCI::ComponentId&  publisher,        
            const KLEV::Event*          event,
		    void*                       context);

        std::vector<KLEV::SubscriptionId>	m_vecIds;
    protected:
        KLSTD::CAutoPtr<EvSink>	m_pSink;        
    };

    typedef std::map<
		KLEV::HSINKID,
		KLSTD::KLAdapt<KLSTD::CAutoPtr<CEvSinkEntry> > > sink_entries_t;

	class CEvSinksImpl: public EvSinks{
	public:
		void Initialize(
			const KLPRCI::ComponentId& idServerComponent,
			const std::wstring& wstrProxyName,
			KLSTD::CAutoPtr<KLSTD::CriticalSection> pCS);

		void Deinitialize();

        void AddSubscription(
            std::vector<subscription_data_t>&	vecData,
            KLEV::EvSink*	pSink,
            KLEV::HSINKID&	hSink);

        KLSTD_NOTHROW bool RemoveSubscription(KLEV::HSINKID hSink) throw();

        void GetServerCID(KLPRCI::ComponentId& idComponent);

        void GetProxyCID(KLPRCI::ComponentId& idComponent);

	protected:
		sink_entries_t m_Sinks;
		KLPRCI::ComponentId m_idServerComponent;
		std::wstring m_wstrProxyName;
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;
	};
};

#endif //__KL_SINKS_STUFF_H__
