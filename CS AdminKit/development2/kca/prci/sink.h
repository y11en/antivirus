#ifndef KLPRCI_SINK_H
#define KLPRCI_SINK_H

#include "kca/prci/componentid.h"
#include "transport/ev/event.h"
#include "transport/ev/eventsource.h"
#include "kca/prcp/componentproxy.h"

#include <map>

namespace KLPRCI
{

	class KLSTD_NOVTABLE AdviseSink: public KLSTD::KLBase
	{
	public:
		virtual void Initialize(
                    KLEV::SubscriptionId    idSubscr,
                    const std::wstring&     wstrUser)=0;
		virtual void Process(
					const KLPRCI::ComponentId&	subscriber,
					const KLPRCI::ComponentId&	publisher,
					const KLEV::Event*			event)=0;
	};

	template<class H>
	class CAdviseSinks{
	public:
		CAdviseSinks()
		{
			m_nLastUsed=0;
		};
        
        virtual ~CAdviseSinks()
        {
            Clear();
        };

		H addNew(KLPRCI::AdviseSink* p)
		{
			H handle=(H)(++m_nLastUsed);
#ifdef N_PLAT_NLM
//                m_Data[wstrKey] = item_data_t(pItem, idTask);
                m_data[handle] =  KLSTD::CAutoPtr<KLPRCI::AdviseSink>(p);
#else
            m_data.insert(KLSTD_TYPENAME sinks_t::value_type(handle, KLSTD::CAutoPtr<KLPRCI::AdviseSink>(p)));
#endif

			return handle;
		};

		bool remove(H handle, KLPRCI::AdviseSink** ppSink)
		{
			typename sinks_t::iterator it=m_data.find(handle);
			if(it==m_data.end())
				return false;
				//KLSTD_THROW(STDE_BADHANDLE);
            KLSTD::CAutoPtr<AdviseSink> pTmpSink = it->second.m_T;//due to STL map lock
			m_data.erase(it);
            if(ppSink)
                pTmpSink.CopyTo(ppSink);
            pTmpSink=NULL;
			return true;
		};

		AdviseSink* find(H handle)
		{
			typename sinks_t::iterator it=m_data.find(handle);
			if(it==m_data.end())
				KLSTD_THROW(KLSTD::STDE_BADHANDLE);
			return it->second;
		};
		
        void Clear(KLSTD::CriticalSection* pCS = NULL)
		{
            bool bNoSubs = false;
            while(!bNoSubs)
            {
                KLSTD::CAutoPtr<KLPRCI::AdviseSink> pAdviseSink;
		        {
                    KLSTD::AutoCriticalSection cs(pCS);
                    if(!m_data.empty())
                    {
                        pAdviseSink = m_data.begin()->second;
                        m_data.erase(m_data.begin());                        
                    };
                    bNoSubs = m_data.empty();
		        };
                pAdviseSink = NULL;
            };
		};
	protected:
        typedef std::map<H, KLSTD::KLAdapt<KLSTD::CAutoPtr<AdviseSink> > >	sinks_t;
		sinks_t										m_data;
		long										m_nLastUsed;
	};


	//! CSubscribeSink - обработка событий подписанных ComponentProxy::Subscribe
	class CSubscribeSink: public KLSTD::KLBaseImpl<AdviseSink>
	{
	public:
		CSubscribeSink(void* pContext,KLPRCP::EventCallback pCallback);
		virtual ~CSubscribeSink();
		void Initialize(
                    KLEV::SubscriptionId idSubscr,
                    const std::wstring& wstrUser)
        {
            m_idSubscr=idSubscr;
            const_cast<std::wstring&>(m_wstrUser)=wstrUser;
        };
		void Process(
			const KLPRCI::ComponentId&	subscriber,
			const KLPRCI::ComponentId&	publisher,
			const KLEV::Event*			event);
	protected:
		void*					m_pContext;
		KLPRCP::EventCallback	m_pCallback;
		KLEV::SubscriptionId	m_idSubscr;
        const std::wstring      m_wstrUser;
	};

	//! CNotifyChangesSink - обработка событий подписанных ProductSettings::SubscribeOnSettingsChange
	class CNotifyChangesSink: public KLSTD::KLBaseImpl<AdviseSink>
	{
	public:
		CNotifyChangesSink(void* pContext, KLPRCI::SettingsChangeCallback pCallback);
		virtual ~CNotifyChangesSink();
		void Initialize(
                    KLEV::SubscriptionId idSubscr,
                    const std::wstring& wstrUser)
        {
            m_idSubscr=idSubscr;
            const_cast<std::wstring&>(m_wstrUser)=wstrUser;
        };
		void Process(
			const KLPRCI::ComponentId&	subscriber,
			const KLPRCI::ComponentId&	publisher,
			const KLEV::Event*			event);
	protected:
		void*					m_pContext;
		SettingsChangeCallback	m_pCallback;
		KLEV::SubscriptionId	m_idSubscr;
        const std::wstring      m_wstrUser;
	};

	KLEV::SubscriptionId subscribe(
			const KLPRCI::ComponentId&			subscriber,
			const KLPRCI::ComponentId&			publisher,
			const std::wstring&					type,
			KLPAR::Params*						eventBodyFilter,
			AdviseSink*							pSink,
			const KLEV::SubscriptionOptions&	subOptions=KLEV::SubscriptionOptions(),
            bool                                bCurrentUserOnly=false);

};

#endif // KLPRCI_SINK_H

// Local Variables:
// mode: C++
// End:

