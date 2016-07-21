#include "build/general.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "std/base/klbase.h"
#include "std/trc/trace.h"
#include "std/par/errors.h"
#include "./sink.h"
#include "prci_auth.h"

#define KLCS_MODULENAME L"KLPRCI"

using namespace KLSTD;
using namespace KLERR;
using namespace KLPAR;
using namespace KLPRSS;

namespace KLPRCI
{

	static void EventTranslator(
				const KLPRCI::ComponentId & subscriber,
				const KLPRCI::ComponentId & publisher,
				const KLEV::Event *event,
				void *context)
	{
		KLSTD_ASSERT(event && context);
		if(!event || !context)return;
		KLERR_BEGIN
			((AdviseSink*)context)->Process(subscriber, publisher, event);
		KLERR_END
	};

	KLEV::SubscriptionId subscribe(
			const KLPRCI::ComponentId&			subscriber,
			const KLPRCI::ComponentId&			publisher,
			const std::wstring&					type,
			KLPAR::Params*						eventBodyFilter,
			AdviseSink*							pSink,
			const KLEV::SubscriptionOptions&	subOptions,
            bool                                bCurrentUserOnly)
	{
		KLEV::EventSource* pEventSrc=KLEV_GetEventSource();
		KLSTD_ASSERT(pEventSrc);
        KLEV::SubscriptionOptions   newOptions;
        if(type == KLPRCP::c_EventConnectionBroken)
            newOptions.local = true;
        else
            newOptions = subOptions;
		KLEV::SubscriptionId idSubscr=pEventSrc->Subscribe(
									subscriber,
                                    (type == KLPRCP::c_EventConnectionBroken)
                                        ?subscriber
                                        :publisher,
									type,
									eventBodyFilter,
									EventTranslator,
									pSink,
									newOptions);
		pSink->Initialize(idSubscr, bCurrentUserOnly?KLPRCI_GetCurrentSID() : L"");
		return idSubscr;
	};


	CSubscribeSink::CSubscribeSink(void* pContext, KLPRCP::EventCallback pCallback):
						KLSTD::KLBaseImpl<AdviseSink>()
	{
		m_pContext=pContext;				
		m_pCallback=pCallback;
		m_idSubscr=0;
	};

	CSubscribeSink::~CSubscribeSink()
	{
		KLERR_BEGIN
			KLEV::EventSource* pEventSrc=KLEV_GetEventSource();
			pEventSrc->Unsubscribe(m_idSubscr);
		KLERR_END
	};

	void CSubscribeSink::Process(
			const KLPRCI::ComponentId&	subscriber,
			const KLPRCI::ComponentId&	publisher,
			const KLEV::Event*			event)
	{
    KLERR_BEGIN
        CAutoPtr<Params> pBody=event->GetParams();
        std::wstring wstrEventUser;
        if(!m_wstrUser.empty() && pBody && pBody->DoesExist(c_evpUserName))
            wstrEventUser=GetStringValue(pBody, c_evpUserName);
        if(wstrEventUser.empty() || m_wstrUser.empty() || wstrEventUser == m_wstrUser)
		    m_pCallback(
			    event->GetPublisher(),
			    event->GetType(),
			    pBody,
			    event->GetBirthTime(),
			    m_pContext);
        else
            KLSTD_TRACE3(
                        3,
                        L"Event '%ls' from user '%ls' was ignored for user '%ls'\n",
                        event->GetType().c_str(),
                        wstrEventUser.c_str(),
                        m_wstrUser.c_str());
    KLERR_END
	};

	CNotifyChangesSink::CNotifyChangesSink(void* pContext, KLPRCI::SettingsChangeCallback pCallback)
								:KLSTD::KLBaseImpl<AdviseSink>()
	{
		m_pContext=pContext;				
		m_pCallback=pCallback;
		m_idSubscr=0;
	};

	CNotifyChangesSink::~CNotifyChangesSink()
	{
		KLERR_BEGIN
			KLEV::EventSource* pEventSrc=KLEV_GetEventSource();
			pEventSrc->Unsubscribe(m_idSubscr);
		KLERR_END
	};

	void CNotifyChangesSink::Process(
					const KLPRCI::ComponentId&	subscriber,
					const KLPRCI::ComponentId&	publisher,
					const KLEV::Event*			event)
	{
		if(event->GetType()==c_EventSettingsChange)
		{
            KLERR_BEGIN
                CAutoPtr<Params> pBody=event->GetParams();
                std::wstring wstrEventUser;
                if(!m_wstrUser.empty() && pBody && pBody->DoesExist(c_evpUserName))
                    wstrEventUser=GetStringValue(pBody, c_evpUserName);
                if(wstrEventUser.empty() || m_wstrUser.empty() || wstrEventUser == m_wstrUser)
                {
			        KLPAR::Params* pParams=event->GetParams();
			        const Value* pValue=NULL;
			        
			        pValue=pParams->GetValue2(c_evpProductName, true);
			        KLPAR_CHKTYPE(pValue, STRING_T, c_evpProductName);
			        const wchar_t* szwProductName=((StringValue*)pValue)->GetValue();

			        pValue=pParams->GetValue2(c_evpVersion, true);
			        KLPAR_CHKTYPE(pValue, STRING_T, c_evpVersion);
			        const wchar_t* szwVersion=((StringValue*)pValue)->GetValue();

			        pValue=pParams->GetValue2(c_evpSection, true);
			        KLPAR_CHKTYPE(pValue, STRING_T, c_evpSection);
			        const wchar_t* szwSection=((StringValue*)pValue)->GetValue();

			        pValue=pParams->GetValue2(c_evpParameterName, false);
                    const wchar_t* szwParameterName = L"";
                    if(pValue)
                    {
                        KLPAR_CHKTYPE(pValue, STRING_T, c_evpParameterName);
                        szwParameterName = ((StringValue*)pValue)->GetValue();
                    };                        
			        const KLPAR::Value* pOldValue=pParams->GetValue2(c_evpOldValue, false);
			        const KLPAR::Value* pNewValue=pParams->GetValue2(c_evpNewValue, false);

			        m_pCallback(m_pContext, szwProductName, szwVersion, szwSection, szwParameterName, pOldValue, pNewValue);
                }
                else
                    KLSTD_TRACE3(
                                3,
                                L"Event '%ls' from user '%ls' was ignored for user '%ls'\n",
                                event->GetType().c_str(),
                                wstrEventUser.c_str(),
                                m_wstrUser.c_str());
            KLERR_ENDT(1)
		};
	};
};
