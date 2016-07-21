/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file EVP/EventsProcessorImpl.h
 * \author Михаил Кармазин
 * \date 2002
 * \brief Заголовок имплементации EventsProcessor
 */

#ifndef _EVENTSPROCESSORIMPL_H_0CD9F955_D3C1_4c94_8D10_063BB2D9905D__
#define _EVENTSPROCESSORIMPL_H_0CD9F955_D3C1_4c94_8D10_063BB2D9905D__

#include <server/evp/eventsprocessorsrv.h>
#include <transport/ev/eventsource.h>
#include <std/sch/schedule.h>
#include <std/sch/scheduler.h>
#include <std/sch/task.h>
#include <server/srvinst/klserver.h>
#include <server/tsk/commonsrvtasktools.h>
#include <srvp/pol/policies.h>
#include <server/evp/transeventsource.h>
#include <server/evp/notificationpropertiessrv.h>
#include <kca/basecomp/cmpbase.h>
#include <server/db/dbconnection.h>
#include <server/srvinst/srvinstdata.h>

namespace KLSRV
{
    class EventsProcessorImplBase :
        public KLEVP::EventsProcessorSrv,
        public KLSRV::Deinitializer{
	};

    class EventsProcessorImpl: public KLSTD::KLBaseImpl<EventsProcessorImplBase> {
    public:
        EventsProcessorImpl();

        ~EventsProcessorImpl();

        void Initialize( 
			KLSRV::SrvData* pSrvData,
            const KLPRCI::ComponentId & cidMaster, 
			KLSTD::CAutoPtr<NotificationPropertiesSrv> pNotificationPropertiesSrv,
            CommonSrvGroupTaskTools * pCommonSrvTaskTools,
            KLSRV::PoliciesSettings* pPoliciesSettings,
			KLBASECOMP::ComponentBase*	pComponent,
			KLSTD::CAutoPtr<KLPRCI::ProductSettings> pProductSettings);

        void Deinitialize();

        // EventsProcessor:

        void Process ( 
			KLDB::DbConnectionPtr pDbConn,
            const std::vector<KLEVP::EventInfo> & package,
            const std::wstring & deliveryId,
            const std::wstring & domain,
            const std::wstring & hostName,
			long nHostId,
            const std::wstring & agentId,
			std::vector<long>* pVecEventsDbIds);


		void ProcessHostNFSection(
			KLDB::DbConnectionPtr pDbConn, 
			const std::wstring& wstrHostName, 
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			KLPAR::ParamsPtr parHostNFSection);

		/*
		KLSTD::CAutoPtr<KLSTD::CriticalSection> GetCriticalSection()
		{
			return m_pCSProcessAndSubscr;
		}
		*/

        // non-virtual methods for calls from TransEventSourceImpl:
        void Subscribe( 
			const std::wstring&			wstrSubscriberPersistentId,
            const std::wstring&            eventType,
            const KLPAR::Params*        eventBodyFilter,
            KLEVP::TransactionalEventCallback    callback,
            void*                        context,
            KLEV::SubscriptionId&        subscriptionId );

        void StopSubscription( KLEV::SubscriptionId subscriptionId );

        void SendThreadProc();

		void OnSSOnChangeEventAsync();
    private:
        KLSTD::CAutoPtr<KLSRV::PoliciesSettings> m_pPoliciesSettings;
		KLBASECOMP::ComponentBase*	m_pComponent;
        KLPRCI::ComponentId m_cidMaster;
        double m_dTotalTimeInSec;
        int m_nEvents;
        int m_nPackets;
        CommonSrvGroupTaskTools * m_pCommonSrvTaskTools;
        KLSTD::CAutoPtr<NotificationPropertiesSrv> m_pNotificationProperties;
		KLEVP::NotificationLimits m_NotificationLimits;
		KLSTD::CAutoPtr<KLSTD::CriticalSection>	m_pCSNotificationLimits;
		KLSTD::CAutoPtr<KLSTD::CriticalSection>	m_pCSProcessAndSubscr;
		KLSTD::CAutoPtr<KLPRCI::ProductSettings> m_pProductSettings;
		KLPRCI::HSUBSCRIBE m_hSSOnChangeEventCallback;

		KLEVP::NotificationLimits GetNotificationLimits();
        
        typedef std::queue<time_t> TimesOfEventQueue;
		typedef std::map<std::wstring, TimesOfEventQueue> TimesOfEventMap;
        void CleanOldEvents(
			TimesOfEventQueue & qu, 
			int nTestPeriod );

        void SendMessages(
			KLDB::DbConnectionPtr pDbConn,
            TimesOfEventMap& mpTimesOfSend, 
            const KLEVP::NotificationLimits & notificationLimits );

        typedef std::map<int, int> SeverityToEventCountMap;
        bool GetCommonInfoAboutCurrentEvents(
			KLDB::DbConnectionPtr pDbConn,
            const TimesOfEventQueue & qu,
            const KLEVP::NotificationLimits & notificationLimits,
            SeverityToEventCountMap & mapSeverityToEventCount,
            long & nMaxEventId );

        void* m_hSendThreadEvent;
        bool m_bStopSendThread;
        KLSTD::CPointer<KLSTD::Thread>        m_pSendThread;
        void InitializeSendThread();

        bool ReadNotificationDescr(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring & wstrHostName,
            const std::wstring & wstrProductName,
            const std::wstring & wstrProductVersion,
            const std::wstring & wstrEventType,
            long nEventSeverity,
            const std::wstring & wstrTaskId,
            KLSTD::CAutoPtr<KLPAR::Params> pEventBody,
            KLEVP::notification_descr_t2 & nd );
        
        struct TranEventSubscription
        {
			std::wstring						m_wstrSubscriberPersistentId;
            std::wstring                        eventType;

            KLSTD::CAutoPtr<KLPAR::Params>        pParEventBodyFilter;
            KLPAR::ParamsNames                    namesOfFilter;

            KLEVP::TransactionalEventCallback    callback;

            void*                                pContext;
        };

        typedef std::map< KLEV::SubscriptionId, TranEventSubscription > TranEventMap;
        typedef std::list<TranEventSubscription> TranEventList;

		KLSRV::SrvData* m_pSrvData;

		volatile long m_nCallSubscribersRecursiveCount;
		volatile bool m_bInCallSubscribersTransaction;

        TranEventMap                                m_tranEventMap;
        KLEV::SubscriptionId                        m_subscriptionIdCurrent;
        KLSTD::Reporter *                           m_pReporter;

		std::wstring	m_wstrServerFullVersion;

		KLSTD::CAutoPtr<KLSRV::RemoteInstallEventPreprocessor> m_pRemoteInstallEventPreprocessor;

		std::wstring m_wstrServerHostId;
		long m_nServerHostId;

        void PostponeEvent(
			KLDB::DbConnectionPtr pDbConn,
            int    nAttemptCount,
            const EventAndCallerInfo& eventAndCallerInfo,
            const std::wstring & errDescr );

        void ProcessEvent(
			KLDB::DbConnectionPtr pDbConn,
            int nAttemptCount,
            EventAndCallerInfo& eventAndCallerInfo );

        bool LogEvent( 
			KLDB::DbConnectionPtr pDbConn,
			EventAndCallerInfo& eventAndCallerInfo ); // может изменить eventAndCallerInfo

		bool CheckNotificationRules(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring & wstrTaskId,
            const std::wstring & wstrTaskDN,
			KLDB::DbRecordsetPtr pRecordset,
			bool bOriginIsPolicy,
			const EventAndCallerInfo& eventAndCallerInfo /*,
			const MessageArguments & msgArgs */);

        bool CallSubscribers(
			KLDB::DbConnectionPtr pDbConn,
            int    nAttemptCount,
            const TranEventList& eventSubscribersList,
            const EventAndCallerInfo& eventAndCallerInfo,
			const std::vector<std::wstring>& vecPreviouslySucceededSubscribers,
			std::wstring& wstrErrDescr,
			std::vector<std::wstring>& vecSucceededSubscribers);

        bool IsEventAlreadyProcessed(
			KLDB::DbConnectionPtr pDbConn,
			const EventAndCallerInfo& eventAndCallerInfo );

        bool CallOneSubscriber(
			KLDB::DbConnectionPtr pDbConn,
            const TranEventSubscription& tranEventSubscr,
            const EventAndCallerInfo& eventAndCallerInfo,
            std::wstring& wstrErrDescr );

        void RecordResultToDB(
			KLDB::DbConnectionPtr pDbConn,
            bool bResultToRecord,
            int    nAttemptCount,
            const EventAndCallerInfo& eventAndCallerInfo,
            const std::wstring & errDescr,
			std::vector<std::wstring>& vecSucceededSubscribers);

        long PutEvent(
			KLDB::DbConnectionPtr pDbConn,
            long nSeverity,
			bool bOriginIsPolicy,
            const std::wstring& wstrTaskId,
            const std::wstring& wstrTaskDN,
            bool bNeedSend,
            const std::wstring& wstrDescripton,
			long nLocId,
            long nDaysToStore,
            const EventAndCallerInfo & eventAndCallerInfo,
			long nEventTypeId,
			long nKCALocId);

        void InitializeEventRepeat();
        KLSTD::CPointer<KLSCH::Scheduler>    m_pScheduler;
        KLSTD::CPointer<KLSTD::Thread>        m_pSchedulerThread;

        TimesOfEventQueue m_quVirusFoundEvents[KLEVP::VOT_COUNT];
		time_t m_tmLastOutbreakEventRiseTime[KLEVP::VOT_COUNT];
		time_t m_tmLastOutbreakDetectTime[KLEVP::VOT_COUNT];

		std::vector<std::wstring> m_vecEmailProtectApps;
		std::vector<std::wstring> m_vecPerifProtectApps;

        void CheckVirusOutbreak(const EventAndCallerInfo& eventAndCallerInfo);
		bool CheckVirusOutbreakType(KLEVP::VirusOutbreakSource& vo, const EventAndCallerInfo& eventAndCallerInfo);
		void CheckRemoteInstallResultEvent(
			KLDB::DbConnectionPtr pDbConn,
			EventAndCallerInfo& eventAndCallerInfo);

        struct SendSettings
        {
            std::wstring wstrSMTPServer;
            long nSMTPPort;
		    std::wstring wstrEmail;
            std::wstring wstrNetSend;
            std::wstring wstrScriptTarget;
            std::wstring wstrScriptWorkingFolder;
            std::wstring wstrMessageTemplate;
        };

        std::wstring GetNFSectionFilePath(
   			const std::wstring& wstrHostName, 
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion);

        std::wstring GetNFSectionStorageLocation(const std::wstring& wstrHostName);

		void GetHostNFSection(
			const std::wstring& wstrHostName, 
			const std::wstring& wstrProductName,
			const std::wstring& wstrProductVersion,
			KLPAR::Params** pparNFSection);

        long LookupNFRuleId(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring & wstrHostName,
            const std::wstring & wstrProductName,
            const std::wstring & wstrProductVersion,
            bool & bShallDetermineNFRules );

        long DetermineNFRules(
			KLDB::DbConnectionPtr pDbConn,
            const EventAndCallerInfo& eventAndCallerInfo,
            KLSTD::CAutoPtr<KLPAR::Params> pHostNFSection );

        KLPOL::PolicyId GetPolicyForHostProduct(
			KLDB::DbConnectionPtr pDbConn,
            const EventAndCallerInfo& eventAndCallerInfo );

        void ResetHostProduct(
			KLDB::DbConnectionPtr pDbConn,
            const std::wstring & wstrHostName,
            const std::wstring & wstrProductName,
            const std::wstring & wstrProductVersion );

        long MergeNFSections(
			KLDB::DbConnectionPtr pDbConn,
            const EventAndCallerInfo& eventAndCallerInfo,
            const KLPOL::PolicyId & policyId,
            KLPAR::Params * pPolicyNFSection,
            KLPAR::Params * pHostNFSection );

        long UsePolicyInfoForHost(
			KLDB::DbConnectionPtr pDbConn,
            const EventAndCallerInfo& eventAndCallerInfo,
            const KLPOL::PolicyId & policyId );
		
		void UpdateEventId(long nHostId, KLEVP::EventInfo& ei);
		
		void StartTasksOnViract(long lTypeMask);

    public:
        void EventRepeat();
    };
};

#endif // _EVENTSPROCESSORIMPL_H_0CD9F955_D3C1_4c94_8D10_063BB2D9905D__
