/*!
* (C) 2002 "Kaspersky Lab"
*
* \file naginst/EventsToServer.h
* \author ћихаил  армазин
* \date 2002
* \brief
*
*/

#ifndef __EVENTS_TO_SERVER_H_B1B4C9B6_507C_46c3_9FAF_27C70959887C__
#define __EVENTS_TO_SERVER_H_B1B4C9B6_507C_46c3_9FAF_27C70959887C__

#include <transport/ev/event.h>
#include <kca/prss/settingsstorage.h>
#include <kca/pres/eventsstorage.h>
#include <srvp/evp/eventprocessorclient.h>
#include <common/presserversforread.h>
#include <common/reporter.h>
#include <transport/ev/eventsource.h>
#include <nagent/naginst/naginst.h>
#include <transport/tr/transporttimeout.h>

#include <algorithm>
#include <map>
#include <set>
#include <vector>

#define	KLEVP_EVENTS_TO_SERVER_TSK_COMPL_PERIOD	300000	// 300 секунд

namespace KLEVP
{
    std::wstring GetSubscriptionFullName(
                    const KLPRSS::product_version_t&    prod,
                    const std::wstring&                 subscrId);

    typedef std::map<std::wstring, bool> map_subscr_handed_t;
    /*!
        Implementation must not be thread-safe: access must be serialized.
    */
    class KLSTD_NOVTABLE EvProcessorHook : public KLSTD::KLBaseQI
    {
    public:
        KLSTD_NOTHROW virtual void EvpHook_Initialize(
                        KLEVP::PRESServersForRead*  pPresServersForRead) throw() = 0;
        /*!
            Method is called to collect events to send to server. if
            nothing to send then empty vector must be returned.
        */
        KLSTD_NOTHROW virtual void EvpHook_OnProcessing(
                std::vector<EventInfo>&     package,
                void**                      ppContext,
                map_subscr_handed_t&        mapProcessed) throw() = 0;
        /*!
            This method is called ALWAYS if EvpHook_OnProcessing was called.
            If package was empty, then bSuccess must be ignored.
        */
        KLSTD_NOTHROW virtual void EvpHook_OnProcessed(
                void*                       pContext,
                bool                        bSuccess,
                map_subscr_handed_t&        mapProcessed) throw() = 0;
        

        /*!
          \brief	Checks whether hook ownd given subscription

          \param	wstrSubscriptionId - Subscription id
          \return	true if owns, false otherwise
        */
        KLSTD_NOTHROW virtual bool EvpHook_IfOwnsSubscription(
                    const std::wstring& wstrSubscriptionId) throw() = 0;

        KLSTD_NOTHROW virtual void EvpHook_GetSubscriptionNames(
                std::vector<std::wstring>&  vecSubscriptions) throw() = 0;

        /*!
          \brief	Checks whether hook is "idle" hook
        */       
        KLSTD_NOTHROW virtual bool EvpHook_IsIdleHook() throw() = 0;
    };

    typedef std::vector<KLSTD::KLAdapt<KLSTD::CAutoPtr<EvProcessorHook> > >
        evp_hooks_vector;
    
    typedef std::set< KLPRSS::product_version_t > ProductSet;

	typedef std::pair<std::wstring /*wstrMsg*/, int /*nSeverity*/> MsgForEventLog;
	class MapPRESProxyCache;

    class EventsToServer : public KLSTD::KLBaseImpl<KLSTD::KLBase>
    {
    public:
        EventsToServer();
        virtual ~EventsToServer();

	    void Initialize( const KLPRCI::ComponentId&     cidMaster,
                         KLEVP::EventsProcessorFactory* pEventsProcessorFactory,
                         KLSTD::Reporter*               pReporter,
                         const ProductSet&              productsToProcess,
                         const ProductSet&              productsToIgnore,
                         KLNAG::NagentProperties*       pNagentProperties,
                         const evp_hooks_vector&        vecHooks,
						 KLSTD::CAutoPtr<KLPRCI::ProductSettings> pProductSettings);

        void Deinitialize();

        void DoCheck();

	    void OnEvent(
		    const KLPRCI::ComponentId & subscriber, 
		    const KLPRCI::ComponentId & publisher,        
		    const KLEV::Event *event );

        void OnE2SDataChangeAsync( KLSTD::CAutoPtr<KLEV::Event> pEvent );
        void OnEventAddAsync( KLSTD::CAutoPtr<KLEV::Event> pEvent );
		void OnAppStartStopAsync( KLSTD::CAutoPtr<KLEV::Event> pEvent );

		void ResetNFSectionChecksums(bool bDenyUpdateTillRestart);
		static void ResetNFSectionChecksums(
			//const ProductSet& productsToProcess,
			//const ProductSet& productsToIgnore, 
			const wchar_t* szSubscriptionDataFilePath = NULL);
		
		bool CheckAsyncCall();
		
		KLEVP::PRESServersForRead *GetPRESServersForRead();
		
		void SetTskComplPeriodMS(long nPeriodMS);
		
		void CheckAndClearDistributivePath(const std::wstring& wstrTaskID);

		void OnUserForceSyncRequest();
		
		static std::wstring GetSubscriptionDataFilePath(KLSTD::CAutoPtr<KLPRSS::SettingsStorage> pSSMain);

    private:
        void CallHooks(
                    bool                                        bRunFullCheck,
                    map_subscr_handed_t&                        mapChangedSubscrFullIds,
                    KLSTD::CAutoPtr<KLEVP::EventsProcessor>&    pEventsProcessor,
                    int&                                        nMaxEventsToInsertInPackage);
        //returns false if and only if attempt to connect failed
        KLSTD_NOTHROW bool CallIdleHooks(KLSTD::CAutoPtr<KLEVP::EventsProcessor>&    pEventsProcessor) throw();
        //returns false if unable to connect to server
        bool ProcessChunk(
                    KLSTD::CAutoPtr<KLEVP::EventsProcessor>&    pEventsProcessor,
                    const std::vector<EventInfo>&               packageEventInfo);
        evp_hooks_vector    m_vecHooks, m_vecIdleHooks;
        ProductSet m_productsToProcess;
        ProductSet m_productsToIgnore;

		long m_nTasksCompletionPercentsPeriodMS;

        KLSTD::CAutoPtr<KLNAG::NagentProperties>    m_pNagentProperties;
        KLEV::SubscriptionId    m_subscrIdOnE2SDataChange;
        KLEVP::PRESServersForRead presServersForRead;

	    KLPRCI::ComponentId m_cidMaster;
        KLSTD::CAutoPtr<KLEVP::EventsProcessorFactory> m_pEventsProcessorFactory;

		KLSTD::CAutoPtr<KLPRSS::SettingsStorage> m_pE2SSettingsStorage;
		std::wstring m_wstrSubscriptionDataFilePath;

        KLSTD::Reporter * m_pReporter; // may be NULL

	    /* std::wstring m_wstrServerAddress;
        unsigned short* m_pServerPorts; */
        
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;

        KLSTD::CAutoPtr<KLPRSS::SettingsStorage> m_pSSMain;

		struct SubscriptionProcessingData
		{
			SubscriptionProcessingData() :
				m_nFinalSentToServerEventId(0), m_nFinalLoggedEventId(0), m_bChanged(false), m_bCounterIdChanged(false) {}
            std::wstring m_wstrSubscr;
			std::wstring m_wstrCounterId;
            bool m_bChanged;
            bool m_bCounterIdChanged;
            long m_nFinalSentToServerEventId;
			long m_nFinalLoggedEventId;
		};

        struct SubcriptionItemData
        {
            SubcriptionItemData() : bNotificationInfoSourceCleared(false), m_nCheckSum(0) { };

			std::wstring wstrSubscrItemName;
			SubscriptionProcessingData m_SubscrToStore;
			SubscriptionProcessingData m_SubscrNotToStore;

            std::map<std::wstring, int> mapEventsForEL;
            
            bool bNotificationInfoSourceCleared;
            KLSTD::CAutoPtr<KLPAR::Params> pNotificationInfoSource;
			
			long m_nCheckSum;
        };
        typedef std::vector<SubcriptionItemData> SubcriptionItemDataVector;

        struct ProductData : KLPRSS::product_version_t
        {
            SubcriptionItemDataVector subcriptionItemDataVector;
            long nSeverity;
        };
        typedef std::vector<ProductData> ProductDataVector;

        struct ProductData_SeverityIsMoreImportant
        {
            bool operator()( const ProductData & _Left, const ProductData & _Right ) const
            {
                return _Left.nSeverity >= _Right.nSeverity;
            }
        };

        ProductDataVector m_productDataVector;

        std::map<KLPRSS::product_version_t, AVP_dword> m_mapSSNFProcessedChecksums;
        std::map<KLPRSS::product_version_t, AVP_dword> m_mapSSNFCandidateChecksums;

        //bool m_bAllSSNFCheckedAfterStart;

		bool m_bSSNFSectionChanged;
		bool m_bInitialized;

		/*
        typedef std::map<
            KLPRSS::product_version_t,
            std::wstring> ProductToSubscriptionMap;
        ProductToSubscriptionMap m_prodToPredefinedSubscrMap;
		*/

        KLTRAP::TransportTimeout m_ProcessEventChunkTimeout;
		unsigned long m_ulLastTimeProcessEventChunk;
		unsigned long m_ulLastTimeReportToEventLogIfServerIsAnavailable;
		unsigned long m_ulLastTimeFullCheck;
		unsigned long m_ulLastTimeReadCurrentProductData;
		unsigned long m_ulLastTimeToCheckIfNeedReleaseSubscriptions;
		unsigned long m_ulLastTimeDeleteDistrPaths;
		std::map<std::wstring, bool> m_mapKnownSubscrFullNames;
		std::map<std::wstring, bool> m_mapChangedSubscrFullNames_ForServer;
		std::map<std::wstring, bool> m_mapChangedSubscrFullNames_ForEL;
		std::map<KLPRSS::product_version_t, bool> m_mapChangedProducts;
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCSChangedSubscrIds;
		std::map<KLPRSS::product_version_t, KLEV::SubscriptionId> m_subscrIdsOnEventAdd;
        KLEV::SubscriptionId    m_subscrIdOnProductStart;
        KLEV::SubscriptionId    m_subscrIdOnProductStop;
        KLEV::SubscriptionId    m_subscrIdOnEventTaskCompletion;
		KLEV::SubscriptionId	m_subscrIdOnTaskDeleted;
		bool	m_bFirstLSCall;

		std::map<std::wstring, std::wstring> m_ProdVerDisplString;
		KLPRCI::HSUBSCRIBE m_hSSUpdateEventCallback;
		unsigned long m_ulLastTimePrepareNFInfoFromLocalSS;
		unsigned long m_ulLastTimePrepareLSTaskIDs;

	    KLSTD::CAutoPtr<KLPRCI::ProductSettings> m_pProductSettings;
		
		volatile long m_lCheckAsyncCallCounter;

		bool m_bDenyNFUpdateTillRestart;

		struct TskCompletionInfo{
			KLSTD::precise_time_t m_Time;
			int m_nPercent;
		};
		/*
			Map TsId -> TskCompletionInfo.
			”даление задачи из taskStorage - нужно чистить мап.
			» еще чистить периодически, например - раз в 10 минут.
			ќтсылка TaskState должна чистить мап прогресса дл€ этой задачи.
			ƒалее дл€ "невычищенных" задач формируютс€ событи€ со списками (не более 100 задач, например), в параметрах событи€- мап ид-ра задачи на процент.
		 */
		std::map<std::wstring, TskCompletionInfo> m_mapTsIdToPercent;
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCSTsIdToPercent;
		unsigned long m_ulLastTimeTasksCompletionPercentsSent;

		void SubscribeForProduct(const KLPRSS::product_version_t& prod);
		
		void AddChangedSubscrId(std::wstring wstrSubscrFullId);
		void GetChangedSubscrIds(std::map<std::wstring, bool>* pMapChangedSubscrIds_ForServer, 
			std::map<std::wstring, bool>* pMapChangedSubscrIds_ForEL);
		void SetChangedSubscrIds(const std::map<std::wstring, bool>* pMapChangedSubscrIds_ForServer, 
			const std::map<std::wstring, bool>* pMapChangedSubscrIds_ForEL);

		void AddChangedProduct(const KLPRSS::product_version_t& prod);
		bool GetChangedProducts(std::map<KLPRSS::product_version_t, bool>& mapChangedProducts, bool& bSSNFSectionChanged);

        void ReadCurrentProductData();
		bool CheckSubscribtionInCurrentProductData(const std::wstring& wstrSubscrFullName);
		void CheckProductInCurrentProductData(const KLPRSS::product_version_t& prod);
        
        void ReadDataForProduct(
            const KLPRSS::product_version_t & productVersion,
            KLPAR::Params * pParamsTaskSubscriptions,
            const std::wstring & wstrInfoDescrName,
            long nSeverity );

        /* void UpdateProductSubscr(
            KLSTD::CAutoPtr<KLPRES::EventsStorage> pEventsStorage,
            KLPRSS::product_version_t productVersion,
            ProductDataMap::iterator & itProdData, // ≈сли на входе itProdData=end(), то на выходе он указывает на вставленный элемент.
            const std::wstring & wstrListIdNew,
            KLPAR::Params * pParamsNewSubcsrData ); */

        void StopPRESWork(
            const KLPRSS::product_version_t & productVersion,
            ProductData & productData );

        void DoCheckForSubscr(
            KLPRES::EventsStorage * pEventsStorage,
			const ProductData& productData,
			const SubcriptionItemData& subscrItemData,
            bool bToStore,
            const std::map<std::wstring, int> & mapEventsToLog,
			int& nMaxEventsCount,
			int& nMaxEventsForELCount,
			std::wstring& wstrCounterId,
			long & nFinalEventIdToServer,
			long & nFinalEventIdForEL,
            std::vector<KLEVP::EventInfo> & packageEventInfo,
            std::vector<MsgForEventLog>& vectMsgsForEL,
			bool& bEventsLeftForServer, //! \brief - true if no more unread events left in PRES
			bool& bEventsLeftForEL //! \brief - true if no more unread events left in PRES
			);

        void PrepareLocalTaskNFInfo( KLPAR::Params * pNotificationInfoSource, std::vector<KLEVP::EventInfo> & packageEventInfo );

        void PrepareNFInfoFromLocalSS( std::vector<KLEVP::EventInfo> & packageEventInfo, int nMaxEventsToInsertInPackage );

        void PrepareLSTaskIDs(
            bool bRunFullCheck,
            std::vector<KLEVP::EventInfo> & packageEventInfo,
            KLPAR::Params ** ppSuccTaskIDs,
			KLPAR::Params ** ppFailedTaskIDs,
			KLPAR::Params ** ppFailedTmpFilesToDeleteOnRestart);

        bool IsOkProduct( const KLPRSS::product_version_t & productVersion );
		static bool IsOkProduct( 
			const KLPRSS::product_version_t & productVersion, 
			const ProductSet& productsToProcess,
			const ProductSet& productsToIgnore);

		bool IfHooksOwnSubscription(const std::wstring& wstrSubscrFullName);

		std::wstring GetProdVerString(KLPRCI::ComponentId& id);
		
		void CheckIfNeedReleaseSubscriptions(MapPRESProxyCache& mapPRESProxyCache);

        void SubscribeToSSNFSectionChange();
		void UnsubscribeToSSChange();

		void SetSSNFSectionChanged();

		void OnSSOnChangeEventAsync();

		static void EventsToServer::SSOnChangeEventCallback(
				void*				context,
				const std::wstring&	productName, 
				const std::wstring&	version,
				const std::wstring&	section, 
				const std::wstring&	parameterName,
				const KLPAR::Value*	oldValue,
				const KLPAR::Value*	newValue);

		void DeleteDistribFolder(std::wstring wstrTaskId, std::wstring wstrDistribPath);

		bool GetTasksCompletionPercents(std::map<std::wstring, TskCompletionInfo>& mapTsIdToPercent);
		bool CheckTasksCompletionPercents();
		void SetTasksCompletionPercents(const std::map<std::wstring, TskCompletionInfo>& mapTsIdToPercent);
		void CreateTaskCompletionEvent(
			KLSTD::CAutoPtr<KLPAR::Params> parPercentParams,
			KLSTD::CAutoPtr<KLPAR::Params> parTimeParams,
			KLSTD::CAutoPtr<KLPAR::Params> parTimeMSParams,
			EventInfo& eventInfo);
		void DeleteKpdFile(std::wstring wstrDistribPath);
	public:
		void AddTaskCompletionPercent(KLSTD::CAutoPtr<KLEV::Event> pEvent);
		void ResetTaskCompletionPercent(const std::wstring wstrTsId, KLSTD::precise_time_t t);
    };
}

void GetHostAndDomainName( std::wstring& wstrHost,
				           std::wstring& wstrDomain );


#endif // __EVENTS_TO_SERVER_H_B1B4C9B6_507C_46c3_9FAF_27C70959887C__


