#include "build/general.h"
#include "std/base/klbase.h"
#include "std/par/params.h"
#include "std/par/paramsi.h"
#include "std/par/errors.h"
#include "transport/tr/transport.h"
#include "transport/tr/transportproxy.h"
#include "kca/prci/componentid.h"
#include "kca/prss/settingsstorage.h"
#include "soapH.h"

#include "kca/pres/eventsstorage.h"
#include "pressubscriptionlist.h"
#include "presdefs.h"
#include "kca/prts/prxsmacros.h"
#include "localeventsstorage.h"

#ifdef N_PLAT_NLM
# include "std/conv/wcharcrt.h"
#endif

#define KLCS_MODULENAME L"KLPRES"

namespace KLPRES {

using namespace KLSTD;
using namespace KLPAR;
using namespace KLPRSS;

static long g_lSubscriptionInfoAdjustableMaxEvents = 2048;

long SubscriptionInfo_GetAdjustableMaxEvents()
{
	// think about using ::Interlocked...
	return g_lSubscriptionInfoAdjustableMaxEvents;
}

void SubscriptionInfo_SetAdjustableMaxEvents(long lAdjustableMaxEvents)
{
	// think about using ::Interlocked...
	g_lSubscriptionInfoAdjustableMaxEvents = lAdjustableMaxEvents;
}

/*!
* \brief »нтерфейс дл€ работы со списком событий, сохран€емых в долговременной 
*        либо оперативной пам€ти.
*  
*  ¬се компоненты на локальной машине работают с одним и тем же хранилищем событий.
*   омпонент может работать с хранилищем событий независимо от того, запущен на машине
*  агент администрировани€ или нет.  ’ранилище событий предоставл€ет следующую
*  функциональность:
*
*   1. ¬озможность дл€ компонента записывать требуемые событи€ в хранилище событий.
*   2. ¬озможность получать событи€ из хранилища событий.
*   3. ¬озможность временной либо посто€нной подписки на событи€.
*   4. ¬озможность сохранени€ событий либо в оперативной, либо в долговременной пам€ти.
*   5. ¬озможность просмотреть и редактировать список подписок дл€ компонента.
*  
*/

class CEventsStorage : public KLSTD::KLBaseImpl<EventsStorage>
{
public:
	CEventsStorage(const std::wstring& sServerObjectID, const KLPRCI::ComponentId& cidLocalComponent, const KLPRCI::ComponentId& cidComponent)
	{
		IMPLEMENT_LOCK;
		INIT_TRANSPORT(cidLocalComponent, cidComponent);
		m_sServerObjectID = sServerObjectID;
		m_nCurrentEventIndex = 0;
	}

	~CEventsStorage()
	{
        KLERR_BEGIN
		    BEGIN_SOAP_COMMAND(LOCK)
		    {
                if( ! m_wstrSubscrIteratorId.empty() )
                {
                    struct KLPRES_RELEASE_SUBSCRIPTIONS_ITERATOR_RESPONSE rcl;
                    CALL_SOAP_COMMAND(
                        soap_call_KLPRES_ReleaseSubscriptionsIterator(
                            GET_SOAP,
                            GET_LOCATION,
                            NULL, 
                            (wchar_t*)m_sServerObjectID.c_str(), 
                            (wchar_t*)m_wstrSubscrIteratorId.c_str(), 
                            rcl
                        ), 
                        rcl.rcError
                    );
                    m_wstrSubscrIteratorId.clear();// = L"";
                }
		    }
		    END_SOAP_COMMAND(UNLOCK)
        KLERR_END
	}

    void ResetSubscriptionsIterator(const KLPRCI::ComponentId &subscriber, int nTimeout /* = 60 */ )
	{
        if( nTimeout <= 0 )
        {
            KLSTD_THROW_BADPARAM(nTimeout);
        }

        {        
            BEGIN_SOAP_COMMAND(LOCK)
                
                if( ! m_wstrSubscrIteratorId.empty() )
                {
                    struct KLPRES_RELEASE_SUBSCRIPTIONS_ITERATOR_RESPONSE rcl;
                    CALL_SOAP_COMMAND(
                        soap_call_KLPRES_ReleaseSubscriptionsIterator(
                        GET_SOAP,
                        GET_LOCATION,
                        NULL, 
                        (wchar_t*)m_sServerObjectID.c_str(),
                        (wchar_t*)m_wstrSubscrIteratorId.c_str(),
                        rcl
                        ), 
                        rcl.rcError
                        );
                    m_wstrSubscrIteratorId.clear();// = L"";
                }       
            END_SOAP_COMMAND(UNLOCK)
        }

        {        
            BEGIN_SOAP_COMMAND(LOCK)

			SOAPComponentId scid;
			CID_TO_SOAP_CID(subscriber, scid);

			struct KLPRES_RESET_SUBSCRIPTIONS_ITERATOR_RESPONSE rcl;

			CALL_SOAP_COMMAND(soap_call_KLPRES_ResetSubscriptionsIterator(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), scid, nTimeout, rcl), rcl.rcError);

            m_wstrSubscrIteratorId = rcl.sSubscrIteratorId;

			/* m_arSubscriptionNames.clear();
			m_itSubscriptionNames = m_arSubscriptionNames.begin();
			m_parSubscriptions = NULL;

			SOAPComponentId scid;
			CID_TO_SOAP_CID(subscriber, scid);

			struct KLPRES_RESET_SUBSCRIPTIONS_ITERATOR_RESPONSE rcl;

			CALL_SOAP_COMMAND(soap_call_KLPRES_ResetSubscriptionsIterator(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), scid, rcl), rcl.rcError);

			KLPAR::ParamsFromSoap(rcl.pparSubscriptionsParamsOut, &m_parSubscriptions);
			m_parSubscriptions->GetNames(m_arSubscriptionNames);
			m_itSubscriptionNames = m_arSubscriptionNames.begin(); */
		    END_SOAP_COMMAND(UNLOCK)
        }
	}

    virtual bool GetNextSubscription(SubscriptionInfo** info, std::wstring &subscription)
	{
		KLSTD_CHKOUTPTR(info);
		if (m_wstrSubscrIteratorId.empty()) KLSTD_NOINIT(L"KLPRES::m_wstrSubscrIteratorId");

        bool rc = false;

		BEGIN_SOAP_COMMAND(LOCK)
		{
            struct KLPRES_GET_NEXT_SUBSCRIPTION_RESPONSE rcl;

            CALL_SOAP_COMMAND(
                soap_call_KLPRES_GetNextSubscription(
                    GET_SOAP, 
                    GET_LOCATION, 
                    NULL, 
                    (wchar_t*)m_sServerObjectID.c_str(), 
                    (wchar_t*)m_wstrSubscrIteratorId.c_str(),
                    rcl
                ),
                rcl.rcError
            );

            rc = rcl.bOk;
            if( rc )
            {
                CAutoPtr<Params> parSubscription;
                KLPAR::ParamsFromSoap(rcl.parSubscription, & parSubscription );
                CSubscription::Deserialize(parSubscription, info);
                subscription = rcl.sSubscrId;
            }
		}
		END_SOAP_COMMAND(UNLOCK)

		return rc;
	}

    virtual void StartSubscription(const SubscriptionInfo &info, std::wstring &subscription)
	{
		BEGIN_SOAP_COMMAND(LOCK)
		{
			CAutoPtr<Params> parSubscription;
			CSubscription::Serialize(&info, &parSubscription);

			KLPRES_ADD_SUBSCRIPTION_RESPONSE rcl;
			param__params parparParams;

			KLPAR::ParamsForSoap(GET_SOAP, parSubscription, parparParams);

			CALL_SOAP_COMMAND(soap_call_KLPRES_AddSubscription(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)subscription.c_str(), info.isSubscriptionPersist, parparParams, rcl), rcl.rcError);

			subscription = rcl.sSubscriptionID;
		}
		END_SOAP_COMMAND(UNLOCK)
	}

    virtual void UpdateSubscription(const std::wstring subscription, const SubscriptionInfo &info)
	{
		BEGIN_SOAP_COMMAND(LOCK)
		{
			CAutoPtr<Params> parSubscription;
			CSubscription::Serialize(&info, &parSubscription);

			param_error rcl;
			param__params parparParams;

			KLPAR::ParamsForSoap(GET_SOAP, parSubscription, parparParams);

			CALL_SOAP_COMMAND(soap_call_KLPRES_UpdateSubscription(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)subscription.c_str(), parparParams, rcl), rcl);
		}
		END_SOAP_COMMAND(UNLOCK)
	}

	virtual void PauseSubscription(const std::wstring subscription)
	{
		BEGIN_SOAP_COMMAND(LOCK)
		{
			param_error rcl;
			CALL_SOAP_COMMAND(soap_call_KLPRES_PauseSubscription(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)subscription.c_str(), rcl), rcl);
		}
		END_SOAP_COMMAND(UNLOCK)
	}

	virtual void ResumeSubscription(const std::wstring subscription)
	{
		BEGIN_SOAP_COMMAND(LOCK)
		{
			param_error rcl;
			CALL_SOAP_COMMAND(soap_call_KLPRES_ResumeSubscription(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)subscription.c_str(), rcl), rcl);
		}
		END_SOAP_COMMAND(UNLOCK)
	}
	
    virtual void StopSubscription(const std::wstring subscription)
	{
		BEGIN_SOAP_COMMAND(LOCK)
		{
			param_error rcl;

			CALL_SOAP_COMMAND(soap_call_KLPRES_RemoveSubscription(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)subscription.c_str(), rcl), rcl);
		}
		END_SOAP_COMMAND(UNLOCK)
	}

    virtual void ResetEventsIterator(const std::wstring subscription)
	{
		BEGIN_SOAP_COMMAND(LOCK)
		{
			m_parEvents = NULL;
			m_valEventsArray = NULL;
			m_nCurrentEventIndex = 0;

			param_error rcl;
			CALL_SOAP_COMMAND(soap_call_KLPRES_ResetEventsIterator(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)subscription.c_str(), rcl), rcl);

			m_sIteratedSubscriptionID = subscription;
		}
		END_SOAP_COMMAND(UNLOCK)

		NextEvent();
	}

    virtual std::wstring ResetEventsIteratorEx(const std::wstring& subscription)
	{
		std::wstring rc;

		BEGIN_SOAP_COMMAND(LOCK)
		{
			m_parEvents = NULL;
			m_valEventsArray = NULL;
			m_nCurrentEventIndex = 0;

			KLPRES_RESET_EVENTS_ITERATOR_EX_RESPONSE rcl;
			CALL_SOAP_COMMAND(soap_call_KLPRES_ResetEventsIteratorEx(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)subscription.c_str(), rcl), rcl.rcError);

			m_sIteratedSubscriptionID = subscription;
			rc = rcl.sJournalID;
		}
		END_SOAP_COMMAND(UNLOCK)

		NextEvent();

		return rc;
	}

	virtual void SetEventsIterator(const std::wstring subscription, long eventID)
	{
		BEGIN_SOAP_COMMAND(LOCK)
		{
			m_parEvents = NULL;
			m_valEventsArray = NULL;
			m_nCurrentEventIndex = 0;

			param_error rcl;
			CALL_SOAP_COMMAND(soap_call_KLPRES_SetEventsIterator(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)subscription.c_str(), eventID, rcl), rcl);

			m_sIteratedSubscriptionID = subscription;
		}
		END_SOAP_COMMAND(UNLOCK)

		NextEvent();
	}

	virtual std::wstring SetEventsIteratorEx(const std::wstring subscription, long eventID)
	{
		std::wstring rc;
		
		BEGIN_SOAP_COMMAND(LOCK)
		{
			m_parEvents = NULL;
			m_valEventsArray = NULL;
			m_nCurrentEventIndex = 0;

			KLPRES_SET_EVENTS_ITERATOR_EX_RESPONSE rcl;
			CALL_SOAP_COMMAND(soap_call_KLPRES_SetEventsIteratorEx(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)subscription.c_str(), eventID, rcl), rcl.rcError);

			m_sIteratedSubscriptionID = subscription;
			rc = rcl.sJournalID;
		}
		END_SOAP_COMMAND(UNLOCK)

		NextEvent();

		return rc;
	}

	virtual bool IsEvent()
	{
		bool rc = false;

		LOCK
		{
			if (m_valEventsArray != NULL && size_t(m_nCurrentEventIndex) < m_valEventsArray->GetSize())
			{
				CAutoPtr<ParamsValue> valEventParams;
				m_valEventsArray->GetAt(m_nCurrentEventIndex, (Value**)&valEventParams);
				rc = (valEventParams != NULL);
			}
		}
		UNLOCK

		return rc;
	}

	virtual void MoveToEventWithIndex(const std::wstring subscription, long nIndex)
	{
		BEGIN_SOAP_COMMAND(LOCK)
		{
			m_parEvents = NULL;
			m_valEventsArray = NULL;
			m_nCurrentEventIndex = 0;

			param_error rcl;
			CALL_SOAP_COMMAND(soap_call_KLPRES_MoveToEventWithIndex(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)subscription.c_str(), nIndex, rcl), rcl);

			m_sIteratedSubscriptionID = subscription;
		}
		END_SOAP_COMMAND(UNLOCK)

		NextEvent();
	}

	virtual long GetEventsCount(const std::wstring subscription)
	{
		long rc = 0;

		BEGIN_SOAP_COMMAND(LOCK)
		{
			m_parEvents = NULL;
			m_valEventsArray = NULL;
			m_nCurrentEventIndex = 0;

			KLPRES_GET_EVENTS_COUNT_RESPONSE rcl;
			CALL_SOAP_COMMAND(soap_call_KLPRES_GetEventsCount(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)subscription.c_str(), rcl), rcl.rcError);
			rc = (long)rcl.nCount;
		}
		END_SOAP_COMMAND(UNLOCK)

		return rc;
	}

	virtual bool NextEvent(const std::wstring & subscription = L"")
	{
		bool rc = false;

		LOCK
		{
			++m_nCurrentEventIndex;
		}
		UNLOCK

		if (!IsEvent())
		{
			BEGIN_SOAP_COMMAND(LOCK)
			{
				m_parEvents = NULL;
				m_valEventsArray = NULL;

				KLPRES_GET_NEXT_EVENTS_CHUNK_RESPONSE rcl;

				CALL_SOAP_COMMAND(soap_call_KLPRES_GetNextEventsChunk(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)m_sIteratedSubscriptionID.c_str(), rcl), rcl.rcError);

				KLPAR::ParamsFromSoap(rcl.pparEventsParamsOut, &m_parEvents);
				m_valEventsArray = (ArrayValue*)m_parEvents->GetValue2(SS_VALUE_NAME_EVENTS_ARRAY, true);
				m_nCurrentEventIndex = 0;
			}
			END_SOAP_COMMAND(UNLOCK)
		}

		return IsEvent();
	}

    virtual bool GetEvent(KLPRCI::ComponentId &id, 
						  std::wstring &eventType, 
						  KLPAR::Params **eventBody, 
						  time_t &eventTime, 
						  long &eventID,
                          const std::wstring & subscription = L"")
	{
		KLSTD_CHKOUTPTR(eventBody);
		if (!m_valEventsArray) KLSTD_NOINIT(L"KLPRES::m_valEventsArray");

		bool rc = false;
		
		LOCK
		{
			CAutoPtr<ParamsValue> pParamsValue = (ParamsValue*)m_valEventsArray->GetAt(m_nCurrentEventIndex);
			if (pParamsValue)
			{
				CAutoPtr<Params> parEvent = pParamsValue->GetValue();
				CEventInfo EventInfo;

				EventInfo.DeserializeParams(parEvent);

				id = EventInfo.m_cidPublisher;
				eventType = EventInfo.m_sEventType;
				if (EventInfo.m_parEventBody) EventInfo.m_parEventBody.CopyTo(eventBody);
				eventTime = EventInfo.m_EventTime;
				eventID = EventInfo.m_nID;

				rc = true;
			}
		}
		UNLOCK

		return rc;
	}

	virtual void DeleteEvents(const std::wstring subscription, const long StopEventID)
	{
		BEGIN_SOAP_COMMAND(LOCK)
		{
			param_error rcl;

			CALL_SOAP_COMMAND(soap_call_KLPRES_DeleteEvents(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)subscription.c_str(), StopEventID, rcl), rcl);
		}
		END_SOAP_COMMAND(UNLOCK)
	}
	
	virtual void DeleteOneEventByID(const std::wstring subscription, const long nEventID)
	{
		BEGIN_SOAP_COMMAND(LOCK)
		{
			param_error rcl;

			CALL_SOAP_COMMAND(soap_call_KLPRES_DeleteOneEventByID(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)subscription.c_str(), nEventID, rcl), rcl);
		}
		END_SOAP_COMMAND(UNLOCK)
	}

    virtual void DeleteRanges(const std::wstring subscription, const RangeVector & vectRanges)
    {
        if( vectRanges.empty() )
        {
            return;
        }

		BEGIN_SOAP_COMMAND(LOCK)
		{
            param_error rcl;

            CAutoPtr<ValuesFactory> pFactory;
            KLPAR_CreateValuesFactory( & pFactory );

            CAutoPtr<ArrayValue> pArray;
            pFactory->CreateArrayValue( & pArray );

            pArray->SetSize( vectRanges.size() );
            for( unsigned int i = 0; i < vectRanges.size(); i++ )
            {
                CAutoPtr<Params> pParams;
                KLPAR_CreateParams( & pParams );
                ADD_PARAMS_VALUE( pParams, SS_RANGE_FIRST, IntValue, vectRanges[i].first );
                ADD_PARAMS_VALUE( pParams, SS_RANGE_SECOND, IntValue, vectRanges[i].second );

                CAutoPtr<ParamsValue> pVal;
                CreateValue( pParams, &pVal);
                pArray->SetAt(i, pVal);
            }

            CAutoPtr<Params> pRanges;
            param_entry_t pars[]=
            {
                param_entry_t( SS_RANGES, pArray )
            };
            KLPAR::CreateParamsBody( pars, KLSTD_COUNTOF(pars), & pRanges );

            param__params parRanges;
            ParamsForSoap( GET_SOAP, pRanges, parRanges );

            CALL_SOAP_COMMAND(soap_call_KLPRES_DeleteRanges(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)subscription.c_str(), parRanges, rcl), rcl);
        }
        END_SOAP_COMMAND(UNLOCK)
    }

    virtual void ClearEventsList(const std::wstring subscription)
	{
		BEGIN_SOAP_COMMAND(LOCK)
		{
			param_error rcl;

			CALL_SOAP_COMMAND(soap_call_KLPRES_ClearEventsList(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)subscription.c_str(), rcl), rcl);
		}
		END_SOAP_COMMAND(UNLOCK)
	}

	virtual void RemoveExpiredEvents (const std::wstring subscription)
	{
		BEGIN_SOAP_COMMAND(LOCK)
		{
			param_error rcl;

			CALL_SOAP_COMMAND(soap_call_KLPRES_RemoveExpiredEvents(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)subscription.c_str(), rcl), rcl);
		}
		END_SOAP_COMMAND(UNLOCK)
	}

	virtual AVP_longlong GetEventStorageFileSize(const std::wstring subscription)
	{
		AVP_longlong rc = 0;

		BEGIN_SOAP_COMMAND(LOCK)
		{
			KLPRES_GET_EVENT_STORAGE_FILE_SIZE_RESPONSE rcl;

			CALL_SOAP_COMMAND(soap_call_KLPRES_GetEventStorageFileSize(GET_SOAP, GET_LOCATION, NULL, (wchar_t*)m_sServerObjectID.c_str(), (wchar_t*)subscription.c_str(), rcl), rcl.rcError);

			rc = rcl.llStorageSize;
		}
		END_SOAP_COMMAND(UNLOCK)

		return rc;
	}

protected:
	DECLARE_TRANSPORT;
	DECLARE_LOCK;

	std::wstring m_sServerObjectID;

	// Subscriptions iterators
	/* std::vector<std::wstring> m_arSubscriptionNames;
	std::vector<std::wstring>::iterator m_itSubscriptionNames;
	CAutoPtr<KLPAR::Params> m_parSubscriptions; */
    std::wstring m_wstrSubscrIteratorId;
	std::wstring m_sIteratedSubscriptionID;

	// Events iterators
	CAutoPtr<KLPAR::Params> m_parEvents;
	long m_nCurrentEventIndex;
	CAutoPtr<ArrayValue> m_valEventsArray;
};

KLCSKCA_DECL std::wstring SubscriptionInfo_MakeSortMaskItem(std::vector<std::wstring> &arAddress)
{
	std::wstring rc;

	for(std::vector<std::wstring>::iterator i = arAddress.begin();i != arAddress.end();i++)
	{
		if (!rc.empty()) rc += KLPRES_EVENT_NAME_DIVIDER;
		rc += *i;
	}

	return rc;
}

}	// namespace KLPRES

KLCSKCA_DECL void KLPRES_CreateEventsStorageProxy(const std::wstring sServerObjectID, const KLPRCI::ComponentId& cidLocalComponent, const KLPRCI::ComponentId& cidComponent, KLPRES::EventsStorage** ppEventsStorage, bool bLocalProxy)
{
	KLSTD_CHKOUTPTR(ppEventsStorage);

	if (bLocalProxy)
		*ppEventsStorage = new KLPRES::CLocalEventsStorage(sServerObjectID);
	else
		*ppEventsStorage = new KLPRES::CEventsStorage(sServerObjectID, cidLocalComponent, cidComponent);

	KLSTD_CHKMEM(*ppEventsStorage);
}
