/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	commonsrvtasktools.h
 * \author	Mikhail Karmazine
 * \date	13.05.2003 16:30:55
 * \brief	Общие методы для работы на сервере с групповыми задачами
 * 
 */


#ifndef __TSK_COMMONSRVTASKTOOLS_H__
#define __TSK_COMMONSRVTASKTOOLS_H__

#include <std/base/klstd.h>
#include <srvp/tsk/grouptaskcontrol.h>
#include <srvp/tsk/grouptaskstatus.h>
#include <server/srvinst/klserver.h>
#include <server/srv_common.h>
#include <server/tsk/wakeonlansubnet.h>
#include <srvp/tsk/klri_const.h>
#include <server/wnst/windowsinstall.h>

#define KLTSK_FT_TYPE L"KLTSK_FT_TYPE" // INT_T
enum {
	KLTSK_FT_TYPE_RI = 0,
	KLTSK_FT_TYPE_PKG_SYNC,
	KLTSK_FT_TYPE_DEF = KLTSK_FT_TYPE_RI
};

enum ESrvRiStartPhase{
	KLSRV_RI_START_NONE = 0,
	KLSRV_RI_START_FT = 1,
	KLSRV_RI_START_RPC = 2,
	KLSRV_RI_START_DONE = 4
};

namespace KLSRV{
	struct EventAndCallerInfo;
};

namespace KLSRV
{
	class BoolEx{
	public:
		enum EBoolEx{EB_FALSE, EB_TRUE, EB_UNKNOWN} m_val;
		BoolEx(EBoolEx b):m_val(b){}
		BoolEx(bool b):m_val(b? EB_TRUE : EB_FALSE){}
		bool IsDefined(){return m_val != EB_UNKNOWN;}
		bool IsTrue(){return m_val == EB_TRUE;}
		bool IsFalse(){return m_val == EB_FALSE;}
	};

    enum GroupTaskDestinatation { gtdServer, gtdServerAgent, gtdRemoteMachine };

    struct GroupTaskCommonData
    {
        GroupTaskCommonData()
            : nTaskId(0), nGroupId(0), nRemoteSyncId(0) {};

        int nTaskId;
        std::wstring wstrTaskName;
        KLPRCI::ComponentId cid;
        std::wstring wstrDisplayName;
        std::wstring wstrFileIdentity;
        long nGroupId;
        long nRemoteSyncId;
        
        GroupTaskDestinatation GetDestination() const
        {
            return GetDestination( wstrTaskName, cid );
        }

        static GroupTaskDestinatation GetDestination(
            const std::wstring & _wstrTaskName,
            const KLPRCI::ComponentId & _cid )
        {
			if( _cid.productName == KLCS_PRODUCT_ADMSERVER &&
				_cid.version == KLCS_VERSION_ADMSERVER )
			{
				if( _wstrTaskName == GTN_REMOTE_INSTALL || 
					_wstrTaskName == GTN_REMOTE_UNINSTALL || 
					_wstrTaskName == GTN_SYNC_PACKAGE ||
					_wstrTaskName == GTN_SRV_UPGRADE )
				{
					return gtdServer;
				} 
				else if( _cid.componentName != KLCS_COMPONENT_SERVER &&
					wcsncmp(_cid.componentName.c_str(), KLCS_TEST_COMPONENT_PREFIX, 
						wcslen(KLCS_TEST_COMPONENT_PREFIX)) != 0 )
				{
					return gtdServerAgent;
				}
			}
			return gtdRemoteMachine;
        }

        KLTSK::HostState GetDefaultState() const
        {
            return GetDefaultState( GetDestination() );
        }

        KLTSK::HostState GetDefaultStateForStatistics() const
        {
            return ( GetDestination() != gtdServerAgent ) ? KLTSK::hsNone : KLTSK::hsScheduled;
        }

        static KLTSK::HostState GetDefaultState( GroupTaskDestinatation gtd )
        {
            return ( gtd == gtdRemoteMachine ) ? KLTSK::hsNone : KLTSK::hsScheduled;
        }
    };

    struct HostIdentity
    {
    private:
        std::wstring wstrDomainName;
        std::wstring wstrHostName;

    public:
        HostIdentity() {};

        HostIdentity(
            const std::wstring & _wstrDomainName,
            const std::wstring & _wstrHostName )
        {
            SetDomainName( _wstrDomainName );
            SetHostName( _wstrHostName );
        };

        int Compare(const HostIdentity& x) const
        {
			/*
            int c=wstrDomainName.compare(x.wstrDomainName);
            if(c != 0)return c;
			*/
            return wstrHostName.compare(x.wstrHostName);
        };

		bool operator == (const HostIdentity& x) const
		{
            return Compare(x) == 0;
		};

		bool operator < (const HostIdentity& x) const
		{
            return Compare(x) < 0;
		};

        void SetDomainName( const std::wstring & _wstrDomainName )
        {
			//KLSRV::ToUpperCase(_wstrDomainName, wstrDomainName);
			wstrDomainName = _wstrDomainName;
        }

        void SetHostName( const std::wstring & _wstrHostName )
        {
            wstrHostName = _wstrHostName;
            //ToLower( wstrHostName ); now it's guid, no sense to make it lower.
        };

        const std::wstring & GetDomainName() const { return wstrDomainName; };

        const std::wstring & GetHostName() const { return wstrHostName; };
    };
    typedef std::set<HostIdentity> HostSet;

	/*
    struct HostInfo
    {
        std::wstring wstrHostUser;
        std::wstring wstrHostPwd;
        std::wstring wstrHostShare;
    };
    typedef std::map<HostIdentity,HostInfo> HostInfoMap;
	*/

    struct HostInfoState //: HostInfo
    {
        KLTSK::HostState hsHostState;
        long nTryCount;
        std::wstring wstrStateDescr;
		bool bStateReallyExists;
		int nPercentCompleted;
		HostInfoState(): hsHostState(KLTSK::hsNone), nTryCount(0), bStateReallyExists(false), nPercentCompleted(0){}
    };
    typedef std::map<HostIdentity,HostInfoState> HostInfoStateMap;

    class ServerHelpers;


	class KLSTD_NOVTABLE RemoteInstallEventPreprocessor: public KLSTD::KLBase{
	public:
		virtual void CheckRemoteInstallResultEvent(
			KLDB::DbConnectionPtr pDbConn,
			KLSRV::EventAndCallerInfo& eventAndCallerInfo) = 0;
	};

    class CommonSrvGroupTaskTools
    {
    public:
        void Initialize(
			KLSRV::SrvData* pSrvData,
			const KLPRCI::ComponentId & cidServer,            
            KLSRV::ServerHelpers* pServerHelpers );

        void Deinitialize();

        struct TaskOnHost
        {
            int nTaskId;
            std::wstring wstrTaskName;
            KLTSK::HostState hostState;
        };

        void GetCatchedTasksForHost(
			KLDB::DbConnectionPtr pDbConn, 
            const std::wstring & wstrHostName,
            std::vector<TaskOnHost> & vectTasks );

        void GetAllTasksForHost(
			KLDB::DbConnectionPtr pDbConn, 
            const std::wstring & wstrHostName,
            std::vector<int> & vectTaskIds );
        
        void GetHostsForGroup(
			KLDB::DbConnectionPtr pDbConn,
			long nGroupId, 
			HostSet & setHosts );

        void GetHostsFromTaskParameters(
            KLPAR::Params * parTaskParams,
            KLPAR::Params * parTaskInfo,
            HostSet & setHosts );

        void GetHostsForTask(
			KLDB::DbConnectionPtr pDbConn, 
            long nTaskId,
            const HostIdentity & singleHostToPerform,
            HostInfoStateMap & mapHostInfoState );

        void SetStateInDBForTaskAndHost(
			KLDB::DbConnectionPtr pDbConn, 
            KLTSK::HostState  hsHostState,
            long nTaskId,
            std::wstring wstrHostName,
            long nPathId,
            time_t tDeadline,
            std::wstring wstrErrorReport,
            long nTryCount, // 0 - значение TryCount не будет изменено
			long nDbEventId,
			time_t tmTime = 0,
			long lTimeMS = 0,
            const std::wstring & wstrSrcInstId = L"",
            AVP_longlong llSrcEventId = 0 );

        void SetHostProgress(
			KLDB::DbConnectionPtr pDbConn,
            long nTaskId,
            std::wstring wstrHostName,
            std::wstring wstrDescr,
            long nProgress,
			KLSTD::precise_time_t tLastTime,
			const std::wstring & wstrSrcInstId /*= L""*/,
			AVP_longlong llSrcEventId /*= 0*/,
			long nDbEventId);  // nProgress - выполненный процент

        void SetServerAsHostList( KLPAR::Params * pTaskInfo );

        std::wstring GetThisHostName();

        GroupTaskCommonData GetTaskCommonData(
			KLDB::DbConnectionPtr pDbConn,
			long nTaskId, 
			const std::wstring& wstrFileIdentity );
        
        std::wstring GetTSIdFromGTCId(
			KLDB::DbConnectionPtr pDbConn, 
			int nGTCId );

        int GetGTCIdFromTSId(
			KLDB::DbConnectionPtr pDbConn,
			const std::wstring & wstrTSId );

        std::wstring GetWinHostNameFromHostGUID(KLDB::DbConnectionPtr pDbConn, const std::wstring & wstrGUID );

		void MakeBroadcastLocations(
			KLDB::DbConnectionPtr pDbConn, 
			const std::vector<std::wstring>& vecIds,
			KLSRV::BcastLocs&          vecLocations,
			std::vector<std::wstring>* pVecLocatedIds = NULL,
            std::vector<std::wstring>* pVecFailedIds = NULL);

		void FillLocationEx(
			KLTRAP::Transport::LocationEx&  loc,
			const std::wstring&             wstrLocation,
			const std::wstring&             wstrNagentInstaceId,
			const std::wstring&             wstrServerTrName,
			long                            idHost);

        bool GetTaskHostData(
			KLDB::DbConnectionPtr pDbConn, 
            long nSyncId,
            long nHostId,
            long & nTaskId,
            std::wstring & wstrFileIdentity,
            std::wstring & wstrDomainName,
            std::wstring & wstrHostName,
			std::wstring & wstrTskDisplName,
			std::wstring & wstrTskTypeName,
			KLPRCI::ComponentId& cid);

        void SetStateInDBForTaskAndHostEx(
			KLDB::DbConnectionPtr pDbConn, 
            KLTSK::HostState  hsHostState,
			KLTSK::RIFailReason eFailReason,
			BoolEx bNeedReboot,
            long nTaskId,
            std::wstring wstrHostName,
            long nPathId,
            time_t tDeadline,
            std::wstring wstrErrorReport,
            int nTryCountDelta,
			long lProgress,
			long nDbEventId,
			time_t tmTime = 0,
			long lTimeMS = 0,
            const std::wstring & wstrSrcInstId = L"",
            AVP_longlong llSrcEventId = 0,
			int nTimedFinishPercent = 0);

		int GetCurrentPackageNumberForHost(
			long nTaskId,
			KLPAR::Params * parTaskParams,
			KLPAR::Params * parTaskInfo,
			const KLSRV::HostIdentity& hiCurHost);

		void SetRemoteInstallEventPreprocessor(KLSTD::CAutoPtr<RemoteInstallEventPreprocessor> pRemoteInstallEventPreprocessor);
		KLSTD::CAutoPtr<RemoteInstallEventPreprocessor> GetRemoteInstallEventPreprocessor();

		void GetGlblTaskTargetHosts(
			KLDB::DbConnectionPtr pDbConn, 
			long nTaskId, 
			bool bIncludeSlaveServers,
			KLSTD::CAutoPtr<KLPAR::ArrayValue>& pArrayValue);

		void GetTaskTargetHosts(
			KLDB::DbConnectionPtr pDbConn, 
			long nTaskId, 
			bool bIncludeSlaveServers,
			std::vector<long>& vecHosts);
		
		void SendWakeOnLanToSubnets(
			KLDB::DbConnectionPtr pDbConn, 
			long nTaskId,
			long timeout );
		
     private:
        KLPRCI::ComponentId m_cidServer;
        KLSTD::CAutoPtr<KLSRV::ServerHelpers> m_pServerHelpers;

		KLSTD::CAutoPtr<RemoteInstallEventPreprocessor> m_pRemoteInstallEventPreprocessor;

		KLSTD::CAutoPtr<KLTSK::WakeOnLanSubnet> m_pWakeOnLanSubnets;

    };
}

void KLSRV_StorePrivateStringValue(KLSTD::CAutoPtr<KLPAR::Params> pParams, 
	const std::wstring& wstrPrivateParam, const std::wstring& wstrValue);

bool KLSRV_StorePrivateString(KLSTD::CAutoPtr<KLPAR::Params> pParams, 
	const std::wstring& wstrParamName, const std::wstring& wstrPrivateParamName);

bool KLSRV_RestorePrivateStringValue(KLSTD::CAutoPtr<KLPAR::Params> pParams, 
	const std::wstring& wstrPrivateParam, std::wstring& wstrValue);

bool KLSRV_RestorePrivateString(KLSTD::CAutoPtr<KLPAR::Params> pParams, 
	const std::wstring& wstrParamName, const std::wstring& wstrPrivateParamName, bool bDeleteKeyFromParams);

bool KLSRV_GetTaskAccounts(KLSTD::CAutoPtr<KLPAR::Params> parTaskParams, std::vector<KLWNST::SecurityCtx>& vecAccounts, KLSTD::CAutoPtr<KLSTD::CriticalSection> pCS);

bool KLSRV_SetTaskAccounts(const std::vector<KLWNST::SecurityCtx>& vecAccounts, KLSTD::CAutoPtr<KLPAR::Params> parTaskParams, KLSTD::CAutoPtr<KLSTD::CriticalSection> pCS);

long KLSRV_GetPackagesCount(const KLSTD::CAutoPtr<KLPAR::Params> parTaskParams, KLSTD::CAutoPtr<KLSTD::CriticalSection> pCS);

bool KLSRV_IsRemoteInstallTask(const KLSRV::GroupTaskCommonData& taskCommonData);

bool KLSRV_IsRemoteInstallTask(const KLPRCI::ComponentId& cid, const std::wstring& wstrTaskName);

bool KLSRV_IsSyncPackageTask(const KLPRCI::ComponentId& cid, const std::wstring& wstrTaskName);

bool KLSRV_IsServerUpgradeTask(const KLPRCI::ComponentId& cid, const std::wstring& wstrTaskName);

#endif //__TSK_COMMONSRVTASKTOOLS_H__
