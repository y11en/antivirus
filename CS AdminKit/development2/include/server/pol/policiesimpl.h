/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	policiesimpl.h
 * \author	Mikhail Karmazine
 * \date	10.04.2003 16:37:32
 * \brief	Серверная реализация интерфейса Policies
 * 
 */

//	REVISION HISTORY
//
//	  02.04.2004 - andkaz Fixed every-second reading disk files. Now we read it only once during initialization
//	  30.07.2004 - andkaz Fixed buffer overflows by using _snwprintf (KLSTD_SWPRINTF) instead of swprintf
//	  19.01.2005 - andkaz Fixed copy/move bugs
//	  09.02.2005 - andkaz Added support for server hierarchy
//	  01.07.2005 - andkaz Fixed distribution to child policies as it had not worked at all. 
//	  26.07.2005 - andkaz Moved to new db interface. Use SrvRemotePolicy instead Policy2.


#ifndef __KLPOL_POLICIESIMPL_H__
#define __KLPOL_POLICIESIMPL_H__

#include <std/base/klstd.h>
#include <std/thr/locks.h>
#include <srvp/pol/policies.h>
#include <server/gsyn/srv_gsyn.h>
#include <server/srvinst/srv_remote.h>
#include <server/srvinst/srvinstdata.h>

//#include <server/pol/polsoapsrv.h>
#include <server/pol/policiesinternal.h>
#include <server/prssp/ssproxycontrolsrv.h>
//#include <server/srvinst/klserver.h>
#include <server/evp/notificationpropertiessrv.h>
#include <common/ak_cached_data.h>

#include <common/queued_actions.h>

#include <set>
#include <map>


namespace KLSRVPOL
{
    struct ChangedSection
    {
        std::wstring wstrProductName;
        std::wstring wstrProductVersion;
        std::wstring wstrProductSection;

        bool operator ==( const ChangedSection & other )
        {
            return ( wstrProductName == other.wstrProductName ) &&
                   ( wstrProductVersion == other.wstrProductVersion ) &&
                   ( wstrProductSection == other.wstrProductSection );
        }
    };

    typedef std::vector< ChangedSection > VectorChangedSections;

    class PolicySSHolder : public KLSTD::KLBaseImpl<KLPRSSP::SSHolder>
    {
    public:
        PolicySSHolder()
            :   policyId(0)
            ,   nGroupId(KLGRP::GROUPID_INVALID_ID)
            ,   m_bReadOnly(false)
        {
        };

        KLPOL::PolicyId policyId;
        long nGroupId;        
        //KLPOL::PolicyCommonData policyCommonData;
        std::wstring wstrSSLocation;
        bool    m_bReadOnly;

        KLSTD::CAutoPtr<KLPRSS::SettingsStorage> GetSettingsStorage();
        long GetGroupId()const
        {
            return nGroupId;
        };
        long GetPolicyId()const
        {
            return policyId;
        };
        
        bool IsShallSync() 
        {
            return nGroupId != KLGRP::GROUPID_INVALID_ID; 
        };
    };

    enum pol_active_t
    {
        pat_pol_inactive,
        pat_pol_active,
        pat_pol_roaming
    };


    class PolicyQueuedActionsHandler
        :   public KLSTD::KLBase
    {
    public:
        virtual void Process_ProcessOutbreak() = 0;
        virtual void Process_ProcessDRs() = 0;
    };



    class PoliciesImpl
        :   public KLSRV::SrvRemotePolicy
        ,   public KLPRSSP::OperationCallbacks
        ,   public KLSRV::Deinitializer
        ,   public KLSRV::PoliciesSettings
        ,   public KLPOL::PoliciesInternal
        ,   public KLSTDQUEUE::QueuedActionsHandler
        ,   public KLSRVPOL::PolicyQueuedActionsHandler
    {
    public:
        KLSTD_INTERAFCE_MAP_REDIRECT(m_pOwner)
        /*
        KLSTD_INTERAFCE_MAP_BEGIN(KLPOL::Policies)
            KLSTD_INTERAFCE_MAP_ENTRY(KLPOL::Policies2)
            KLSTD_INTERAFCE_MAP_ENTRY(KLSRV::PoliciesSettings)
            KLSTD_INTERAFCE_MAP_ENTRY(KLPOL::PoliciesInternal)
        KLSTD_INTERAFCE_MAP_END()
        */
        PoliciesImpl();        
        virtual ~PoliciesImpl();

        void Initialize(
            KLSTD::KLBaseQI*                    pOwner,
            KLSRV::SrvData*                     pData,
            KLPRSSP::SSProxyControlSrv*         pSSProxyControlSrv,
            KLSRV::NotificationPropertiesSrv*   pNotificationProperties,
			KLPRCI::ComponentInstance*	        pServerComponentInstance);

        void AfterServerInitialized();

    //KLSRV::PoliciesSettings methods
        std::wstring GetDiskStorage();
        std::wstring GetRelativeSyncPath( long nPolicyId );
        std::wstring GetPolicyPath( long nPolicyId );

    //KLSRV::Deinitializer methods
        void Deinitialize();

    //KLPOL::SrvRemotePolicy methods
        void GetPolicyCommonData( 
                    long nGroupId, 
                    long lExpectedServerVersion,
                    KLPOL::PoliciesVector & vectPolicies );

        void GetSinglePolicyCommonData(
            const KLPOL::PolicyId & policyId,
            KLPOL::PolicyCommonData & policyCommonData );

        KLPOL::PolicyId AddPolicy(
            long nGroupId,
            KLPOL::PolicyCommonData & policyCommonData );

        KLPOL::PolicyId CopyPolicy( const KLPOL::PolicyId & policyId, long nNewGroupId );

        KLPOL::PolicyId MovePolicy( const KLPOL::PolicyId & policyId, long nNewGroupId );

        void UpdatePolicyCommonData(
            const KLPOL::PolicyId & policyId,
            KLPOL::PolicyCommonData & policyCommonData );

        void DeletePolicy( const KLPOL::PolicyId & policyId );
        
        bool OnDeferredAction(
                            KLDB::DbConnectionPtr   pCon,
                            int                     nExecOrder,
                            int                     nModule, 
                            int                     nActionCode,
                            int                     nPar1,
                            int                     nPar2,
                            const wchar_t*          wstrPar1,
                            const wchar_t*          wstrPar2);

        KLSTD::CAutoPtr<KLPRSSP::SettingsStorageWithApply>
            GetPolicySettings( const KLPOL::PolicyId & policyId, long nTimeout );

        void GetSSId(
            const KLPOL::PolicyId&  policyId,
            long                    nTimeout,
            std::wstring &          wstrId );

        bool MakePolicyActive(
                        const KLPOL::PolicyId&  policyId, 
                        bool bActive);

        void GetPoliciesForGroup( 
                    long nGroupId, 
                    long lExpectedServerVersion,
                    KLPOL::PoliciesVector & vectPolicies );

        KLPOL::PolicyId CopyOrMovePolicy( 
                const KLPOL::PolicyId&          policyId, 
                const KLPOL::PolicyCommonData&  policyCommonData, 
                bool                            bMove,
                long                            nNewGroupId);
        
        void SetOutbreakPolicies(
                        const std::vector<KLPOL::PolicyId>& vecPolicies);

        void GetOutbreakPolicies(
                        std::vector<KLPOL::PolicyId>& vecPolicies);

        void SetOutbreakPolicies2(
                    KLPAR::Params* pInfo);

        void GetOutbreakPolicies2(
                    KLPAR::Params** ppInfo);
        
        long GetPolicyGroupId(const KLPOL::PolicyId& policyId);

        bool MakePolicyRoaming(const KLPOL::PolicyId& policyId);

        void GetAllAcivePoliciesForGroup( 
                    long nGroupId, 
                    KLPOL::PoliciesVector & vectPolicies );
        
    //KLPOL::PoliciesInternal methods

        KLPOL::PolicyId RecreateSuperPolicy(
                                KLDB::DbConnectionPtr   pCon,
                                long nSuperGroupId,
                                const std::wstring& wstrProduct,
                                const std::wstring& wstrVersion,
                                const std::wstring& wstrDN,
                                KLPOL::AddSuperPolicyCallback* pCallback,
                                bool    bIsRoaming,
                                bool    bForceChildren2Distrib);

        void DeleteSuperPolicy(
                                KLDB::DbConnectionPtr   pCon,
                                long nSuperGroupId,
                                const std::wstring& wstrProduct,
                                const std::wstring& wstrVersion,
                                bool                    bIsRoaming);

        void OnGroupMoved(
                            KLDB::DbConnectionPtr   pCon,
                            long lGroup,
                            long lOldParent,
                            long lNewParent);

        void BeforeVirusOutbreak(int nOutbreakType);

        void AfterVirusOutbreak(int nOutbreakType);
        
        void DeletePolicy( 
                        KLDB::DbConnectionPtr   pCon, 
                        const KLPOL::PolicyId & policyId );

    //PolicyQueuedActionsHandler
        virtual void Process_ProcessOutbreak();
        virtual void Process_ProcessDRs();

    //OperationCallbacks methods
        void OnBeforeSSWrite(
            const KLPRSSP::SSHolder * pSSHolder, 
            const std::wstring & wstrName,
            const std::wstring & wstrVersion,
            const std::wstring & wstrSection,
            KLPAR::ParamsPtr& pData,
            int nOperationType );

        void OnAfterSSWrite(
            const KLPRSSP::SSHolder * pSSHolder, 
            const std::wstring & wstrName,
            const std::wstring & wstrVersion,
            const std::wstring & wstrSection,
            KLPAR::Params* pData,
            KLPAR::Params* pResult,
            int nOperationType );

        void OnAfterSSSectionOperation(
            const KLPRSSP::SSHolder * pSSHolder,
            const std::wstring & wstrName,
            const std::wstring & wstrVersion,
            const std::wstring & wstrSection,
            int nOperationType );

        void OnBeforeSSSectionOperation(
            const KLPRSSP::SSHolder * pSSHolder,
            const std::wstring & wstrName,
            const std::wstring & wstrVersion,
            const std::wstring & wstrSection,
            int nOperationType );

        void OnApplyChanges( const KLPRSSP::SSHolder * pSSHolder );

        void OnBeforeRemoveSS( const KLPRSSP::SSHolder * pSSHolder );

        void OnAfterSSRead(
            const KLPRSSP::SSHolder * pSSHolder,
            const std::wstring & wstrName,
            const std::wstring & wstrVersion,
            const std::wstring & wstrSection,
            KLPAR::ParamsPtr& pData);

    protected:
        bool MakePolicyActiveOrRoaming(
                        const KLPOL::PolicyId&  policyId, 
                        pol_active_t         nActivate);

        void PolicyReadFilter(
                    KLDB::DbConnectionPtr       pCon,
                    KLPAR::ParamsPtr&             pData);
        void PolicyWriteFilter(
                    KLDB::DbConnectionPtr       pCon,
                    KLPAR::ParamsPtr&             pData);
        void Call_ProcessOutbreakAsync();
    //internal methods
        //! must be called after policy was created or make active
        bool ActivatePolicy(
                    KLDB::DbConnectionPtr       pCon,
                    const KLPOL::PolicyId&      policyId,
                    KLPOL::PolicyCommonData*    pPcd,
                    long*                       plGroupId,
                    pol_active_t                nActivate,
                    bool                        bHiLevel);

        void ActivatePolicy_MakeInactive(
                    KLDB::DbConnectionPtr       pCon,
                    const KLPOL::PolicyId&      policyId,
                    const std::wstring&         wstrProduct,
                    const std::wstring&         wstrVersion,
                    long*                       plGsyn);
        
        /*!
          \brief	PolMakeInactive

          \param	LDB::DbConnectionPtr   pCon
          \param	int                     par_nIdPolicy
          \param	bool&                   par_bResult
          \return	true if really deactivated (bOK && removed gsync)
        */
        bool PolMakeInactive(
                    KLDB::DbConnectionPtr   pCon, 
                    int                     par_nIdPolicy,
                    bool&                   par_bResult );

        void ActivatePolicy_MakeActive(
                    KLDB::DbConnectionPtr       pCon,
                    const KLPOL::PolicyId&      policyId,
                    const std::wstring&         wstrProduct,
                    const std::wstring&         wstrVersion,
                    long                        nGroupId,
                    pol_active_t                nActivate,
                    long*                       plGsyn);

        void AfterPolicyAppeared(
                    KLDB::DbConnectionPtr       pCon,
                    const KLPOL::PolicyId&      policyId,
                    KLPOL::PolicyCommonData&    pcd,
                    long                        lGroupId);
        
        void AfterPolicyAcceptanceChanged(
                    KLDB::DbConnectionPtr       pCon,
                    const KLPOL::PolicyId&      policyId,
                    KLPOL::PolicyCommonData&    pcd,
                    long                        lGroupId);

        /*!
            retrieves single active policy for specified product for 
            specified group to send it to slave servers
        */
        KLPOL::PolicyId FindPolicyForProduct(
                    KLDB::DbConnectionPtr       pCon,
                    long                        lGroupId,
                    const std::wstring&         wstrProduct,
                    const std::wstring&         wstrVersion,
                    KLPOL::PolicyCommonData*    pPcd = NULL);

        void GetPolicyCommonDataI(
                    KLDB::DbConnectionPtr   pCon,
                    long                    nGroupId, 
                    long                    lExpectedServerVersion,
                    KLPOL::PoliciesVector & vectPolicies,
                    bool                    bActiveOnly);
        
        static void DistributePolicyCallback(
                            void* pContext, 
                            int nIndex, 
                            bool bResult, 
                            KLERR::Error* pError);
      
        //!checks whether so called 'server-policy' and if it doesn't create it.
        void CheckServerPolicy();

        //!copy/move policy
        KLPOL::PolicyId CopyPolicyI(
                    KLDB::DbConnectionPtr           pCon,
                    const KLPOL::PolicyId&          policyId, 
                    long                            nNewGroupId, 
                    bool                            bDeleteOrigin,
                    const KLPOL::PolicyCommonData*  pPcd);

        //!removes locked attribute and sets mandatory into false
        void ResetParentalLocks(
                    KLDB::DbConnectionPtr   pCon,
                    const std::wstring & wstrPolicyPath );

        //!creates policy in db
        KLPOL::PolicyId AddPolicyRecordToDB(
                    KLDB::DbConnectionPtr       pCon,
                    long                        nGroupId, 
                    KLPOL::PolicyCommonData &   policyCommonData );

        //!Updates some policy attributes (modification time...) must be called only if user changed policy
        time_t UpdatePolicyInternal(
                    KLDB::DbConnectionPtr   pCon,
                    const KLPOL::PolicyId & policyId,
                    const std::wstring & wstrDisplayName = L"",
                    bool bThrowIfError = false );

        //!Marks all policy sections in DB as changed
        void MarkAllPolicySectionsAsChanged(
                    KLDB::DbConnectionPtr   pCon,
                    KLPOL::PolicyId policyId );

        //!deletes policy
        void DeletePolicyI(
                    KLDB::DbConnectionPtr pCon,
                    const KLPOL::PolicyId & policyId);

        //!resets parental locks in child policies 
        /*
        void DeletePolicyI_ResetAllParentalLocksInChildred(
                    KLDB::DbConnectionPtr   pCon,
                    const KLPOL::PolicyId&  policyId );
        */

        //!returns policy common data and group id
        void GetSinglePolicyCommonDataI(
                    KLDB::DbConnectionPtr   pCon,
                    const KLPOL::PolicyId & policyId,
                    KLPOL::PolicyCommonData & policyCommonData,
                    long*   nGroupId = NULL);

        /*!
            1. creates gsyn object if required, 
            2. forces synch 
            3. adds distribution requests via AddDistributionRequests
            4. calls ProcessDRs
            5. removes changed section marks via kldb_pol_check_sub_policies
        */
        void DoSync( 
                    KLDB::DbConnectionPtr   pCon,
                    KLPOL::PolicyId policyId,
                    long nGroupId,
                    KLPOL::PolicyCommonData & policyCommonData,
                    bool bIfChaged = true);

        //! creates policy name
        std::wstring GetSyncName(
                    const std::wstring& wstrProductName,
                    const std::wstring& wstrProductVersion,
                    pol_active_t nType);

        //! creates policy path
        std::wstring GetSyncFilePath(
                    const KLPOL::PolicyId&  policyId,
                    const std::wstring&     wstrProductName,
                    const std::wstring&     wstrProductVersion,
                    pol_active_t     nType);

        //!creates policy in db
        void SavePolicySyncIdToDB(
                    KLDB::DbConnectionPtr   pCon,
                    long                    nPolicyId, 
                    long                    nSyncId,
                    pol_active_t            nActivate);

        //! retrieves pointer to SrvGroupSync
        KLSTD::CAutoPtr<KLSRV::SrvGroupSync> GetGroupSync();

        void GetParentPolicyId(
                    KLDB::DbConnectionPtr   pCon,
                    const KLPOL::PolicyId&  policyId,
                    KLPOL::PolicyId&        idParentPolicy,
                    long&                   idParentGroup);

        void GetParentLockingPolicyId(
                    KLDB::DbConnectionPtr   pCon,
                    const KLPOL::PolicyId&  policyId,
                    KLPOL::PolicyId&        idParentPolicy,
                    long&                   idParentGroup);

        /*!
            Adds distribution requests for all child (and grandchild)
            policies. Those request are removed in 
            ProcessDRs_RemovePolicyToSyncFromDB call.
        */
        void AddDistributionRequests(
                        KLDB::DbConnectionPtr   pCon,
                        const KLPOL::PolicyId & policyId,
                        std::vector<KLPOL::PolicyId>& vecAffectedPolicies);

        //
        void Call_ProcessDRs();
        void Call_ProcessDRsAsync();

        /*!
            If there's parent policy then retrieves all its section and calls
            DistributePolicy for parent policy, specified policy (as child)
            and those sections.
            Otherwise calls ResetParentalLocks for specified policy
        */
        void ApplyLocksFromParentPolicies(
                        KLDB::DbConnectionPtr   pCon,
                        const KLPOL::PolicyId & policyId );

        struct DistributeStuff
        {
            std::vector<int> vectOkAppliedPolicies;
            KLSTD::CAutoPtr<KLERR::Error> pErrorDuringPolicyDistr;
        };
        /*!
            Distributes locks for specified sections from policy 
            policyIdSource to specified child policies by calling 
            KLPRSS_DistributePolicy
        */
        void DistributePolicy(
                        KLDB::DbConnectionPtr   pCon,
                        const KLPOL::PolicyId & policyIdSource,
                        long                    lGroupId,
                        const VectorChangedSections & vectChangedSections,
                        const std::vector<KLPOL::PolicyId> & vectChildren,
                        DistributeStuff * pDistributeStuff );

        /*!
            USED BY ProcessDRs only
            1. Acquires changed sections for specified policyfrom the db
            2. For each specified child do following steps:
                2.1. Distributes policy by calling DistributePolicy
                2.3. Updates child policy modification time (via UpdatePolicyInternal)
                2.4. Calls ProcessDRs_StartSync 
                2.5. RemovePolicyToSyncFromDB
            distributes those sections
        */
        void ProcessDRs_ProcessSubPolicies(
                        KLDB::DbConnectionPtr               pCon,
                        const KLPOL::PolicyId &             polIdParent,
                        long                                lGroupId,
                        const std::vector<KLPOL::PolicyId>& vectChildren);

        /*!
            USED BY ProcessDRs_ProcessSubPolicies only
            Creates gsyn if required, increments and starts gsyn
        */
        long ProcessDRs_StartSync(
                        KLDB::DbConnectionPtr   pCon,
                        const KLPOL::PolicyId & policyId);

        /*!
            USED BY ProcessDRs_ProcessSubPolicies only
            Removes distribution request for specified pair 
            (pol_remove_policy_to_sync,policyIdChild) from db.
            Those request are added in call AddDistributionRequests. 
        */
        void ProcessDRs_RemovePolicyToSyncFromDB( 
                        KLDB::DbConnectionPtr   pCon,
                        const KLPOL::PolicyId & polIdParent,
                        const KLPOL::PolicyId & policyIdChild );

        /*! Throws exception if vectors distributeStuff and vectChildren 
            have different size
        */
        void CheckDistributeStuff(
                        const KLPOL::PolicyId & polIdParent,
                        DistributeStuff & distributeStuff,
                        const std::vector<KLPOL::PolicyId>& vectChildren );

        /*! 
            retrieves sections marked as changed in DB for specified policy
        */
        void GetChangedSectionsForPolicy(
                        KLDB::DbConnectionPtr   pCon,
                        const KLPOL::PolicyId & policyId,
                        VectorChangedSections & vectChangedSections );

        //!retrieves all sections of policy
        void GetAllSectionsOfPolicy(
                        const KLPOL::PolicyId & policyId,
                        VectorChangedSections & vectChangedSections );


        KLPOL::PolicyId AddPolicyI(
                        KLDB::DbConnectionPtr   pCon,
                        long nGroupId,
                        KLPOL::PolicyCommonData & policyCommonData,
                        KLPOL::AddSuperPolicyCallback* pCallback);

        //!Writes to the db extra information from the policy
        void ProcessSpecialSection(
                        KLDB::DbConnectionPtr   pCon,
                        KLPOL::PolicyId         idPolicy, 
                        long                    idGsyn, 
                        const std::wstring&     wstrProduct,
                        const std::wstring&     wstrVersion,
                        const std::wstring&     wstrSection,
                        KLPAR::Params*          pContents);

        //!Writes to the db extra information from the policy
        void FixSpecialSection( 
                        KLDB::DbConnectionPtr   pCon,
                        KLPOL::PolicyId         idPolicy, 
                        long                    idGsyn, 
                        const std::wstring&     wstrProduct,
                        const std::wstring&     wstrVersion,
                        const std::wstring&     wstrSection);
        
        //!Checks whether specified policy is allowed to modify
        void ValidateChange(
                        KLDB::DbConnectionPtr   pCon,
                        KLPOL::PolicyId         idPolicy,
                        long lGroupId = KLGRP::GROUPID_INVALID_ID);
    protected:
        long    GetPolicyGsyn(
                        KLDB::DbConnectionPtr   pCon,
                        KLPOL::PolicyId         idPolicy);
        void    ResetPolicyGsynCache(KLPOL::PolicyId idPolicy);
        bool IsRoamingPolicy(KLDB::DbConnectionPtr pCon, KLPOL::PolicyId idPolicy);
        bool IsActivePolicy(KLDB::DbConnectionPtr pCon, KLPOL::PolicyId idPolicy);
    protected:

        KLSTD::KLBaseQI*                                    m_pOwner;
        KLSRV::SrvData*                                     m_pSrvData;
        KLSTD::CAutoPtr<KLSRV::ServerHelpers>		        m_pServerHelpers;
		KLSTD::CAutoPtr<KLPRCI::ComponentInstance>		    m_pServerComponentInstance;
        KLSTD::CAutoPtr<KLPRSSP::SSProxyControlSrv>         m_pSSProxyControlSrv;
        KLSTD::CAutoPtr<KLSRV::NotificationPropertiesSrv>   m_pNotificationProperties;
        KLPRCI::ComponentId                                 m_cidServer;        
        ;
    public:
        //cached data
        class CCachedPolicyProps
            :   public KLSTD::KLBaseImpl<KLSTD::KLBase>
        {
        public:
            CCachedPolicyProps()
                :   m_lGroupId(KLGRP::GROUPID_INVALID_ID)
            {;};
            virtual ~CCachedPolicyProps()
            {;};
        //data
            KLPOL::PolicyCommonData m_CommonData;
            long                    m_lGroupId;
        };

    
        typedef KLSTD::CAutoPtr<CCachedPolicyProps> CCachedPolicyPropsPtr;

        class CCallPol2Gsyn
        {
        public:
            CCallPol2Gsyn()
            {;};
            CCachedPolicyPropsPtr operator() (KLDB::DbConnectionPtr pCon, KLPOL::PolicyId idPolicy);
        };
    protected:
        KLSTD::PairCache<
            KLSTD::PairCachePropsHash<
                CCachedPolicyPropsPtr, 
                KLDB::DbConnectionPtr, 
                KLPOL::PolicyId, 
                CCallPol2Gsyn> >                            m_oChachePol2Gsyn;

        KLSTD::CAutoPtr<KLSTD::CriticalSection>             m_pDataCS;
        KLSTD::CAutoPtr<KLSTD::ReadWriteLock>               m_lckProcessDRs;
        const std::wstring                                  m_wstrDatFolder;
        KLSTD::CReentProtect                                m_cntOutbreak;
    };
};
#endif //__KLPOL_POLICIESIMPL_H__
