/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	srv_remote.h
 * \author	Andrew Kazachkov
 * \date	07.07.2005 14:07:58
 * \brief	Server's services available from remote hosts
 * 
 */


#ifndef __KL_SRV_REMOTE_H__
#define __KL_SRV_REMOTE_H__

#include <srvp/hst/hosts.h>
#include <srvp/hst/hst_move_rules.h>
#include <srvp/grp/groups.h>
#include <srvp/adhst/adhst.h>
#include <srvp/gsyn/groupsync.h>
#include <srvp/dpns/scandiapasons.h>
#include <server/ptch/patches.h>
#include <common/cleanercommon.h>
#include <common/inventory.h>

namespace KLSRV
{
    inline const wchar_t* StrPtr(const wchar_t* p)
    {
        return (p?p:L"");
    }
    
    class KLSTD_NOVTABLE SrvRemoteChunkAccessor
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual long ChunkAccessorGetTotalCount(
                                const std::wstring& wstrAccessor) = 0;
        virtual long ChunkAccessorGetChunk(
						const std::wstring& wstrAccessor,
						long				nStart,
						long				nCount,
						KLPAR::Params**		ppInfo) = 0;                        
        virtual void ChunkAccessorRelease(
                        const std::wstring& wstrAccessor) = 0;
	    virtual long ChunkAccessorGetNextChunk(
						const std::wstring	wstrIterator,
						long 				nCount,
						KLPAR::Params**		ppInfo,
						bool&				bEOF) = 0;
        
    };

    class KLSTD_NOVTABLE SrvRemoteSs
        :   public KLSTD::KLBaseQI
    {
    public:
		virtual void SS_Read(
						  const std::wstring&	wstrName,
						  const std::wstring&	wstrType,
						  const std::wstring&	wstrProduct,
						  const std::wstring&	wstrVersion,
						  const std::wstring&	wstrSection,
						  long					lTimeout,
						  KLPAR::Params**		ppParams)=0;

		virtual void SS_Write(
						  const std::wstring&	wstrName,
						  const std::wstring&	wstrType,
						  const std::wstring&	wstrProduct,
						  const std::wstring&	wstrVersion,
						  const std::wstring&	wstrSection,
						  AVP_dword				dwFlags,
						  KLPAR::Params*		pParams,
						  long					lTimeout)=0;

		virtual void SS_CreateSection(
						  const std::wstring&	wstrName,
						  const std::wstring&	wstrType,
						  const std::wstring&	wstrProduct,
						  const std::wstring&	wstrVersion,
						  const std::wstring&	wstrSection,
						  long					lTimeout)=0;

		virtual void SS_DeleteSection(
							const std::wstring&	wstrName,
							const std::wstring&	wstrType,
							const std::wstring&	wstrProduct,
							const std::wstring&	wstrVersion,
							const std::wstring&	wstrSection,
							long					lTimeout)=0;

		virtual void SS_GetNames(
							const std::wstring&	wstrName,
							const std::wstring&	wstrType,
							const std::wstring&	wstrProduct,
							const std::wstring&	wstrVersion,
							long					lTimeout,
							std::vector<std::wstring>& names) = 0;
    };

    class KLSTD_NOVTABLE SrvRemoteHstGrp
        :   public KLSTD::KLBaseQI
    {
    public:
		virtual void HstGrp_GetDomains (
						std::list<KLHST::domain_info_t>& domains)  = 0;

		virtual void HstGrp_GetDomainHosts(
						const std::wstring & domain,
						std::list<KLHST::host_info_t> & hosts)=0;

        virtual void HstGrp_GetHostInfo(
					    const std::wstring & name,
						const wchar_t**		fields,
						int					size,
						KLPAR::Params** info )=0;

		virtual void HstGrp_AddDomain(
						const std::wstring&	domain,
						KLHST::DomainType	nType)=0;

		virtual void HstGrp_DelDomain(
						const std::wstring&	domain)=0;

        virtual std::wstring HstGrp_AddHost(
						KLPAR::Params  * info)=0;

        virtual void HstGrp_ResolveAndMoveToGroup(
                        KLPAR::Params*  pInfo,
                        KLPAR::Params** ppResult)=0;

        virtual void HstGrp_DelHost(
						const std::wstring& name)=0;

        virtual void HstGrp_UpdateHost(
						const std::wstring & name,
						KLPAR::Params  * info )=0;

        virtual void HstGrp_GetHostProducts(
						const std::wstring&     name,
                        KLPAR::Params**         ppData)=0;

		virtual void HstGrp_GetHostTasks(
						const std::wstring&		name,
						const std::wstring&		product,
						const std::wstring&		version,						
						long					lLifetime) = 0;

        virtual void HstGrp_GetGroups ( 
							KLPAR::Params**	groups,
							long			parent,
							long			depth) = 0;

        virtual long HstGrp_GetUnassignedGroupId ()  = 0;

        virtual void HstGrp_GetGroupInfo(
							long id,
					        const wchar_t** fields,
					        int				size,
							KLPAR::Params** groups )  = 0;
        virtual void HstGrp_AddGroup(
							KLPAR::Params * info,
							long&			id)  = 0;

        virtual void HstGrp_DelGroup(long id) = 0;

        virtual void HstGrp_UpdateGroup(long id, KLPAR::Params * info)=0;

        virtual void HstGrp_RemoveHosts ( 
							const std::vector<std::wstring>&	hosts )  = 0;

        virtual void HstGrp_SetGroupHosts ( 
                        long						id, 
                        KLPAR::Params*				hosts)  = 0;
        
        virtual long HstGrp_GrpFindHosts(
                    const std::wstring&                 wstrFilter,
					const std::vector<std::wstring>&    vecFieldsToReturn,
                    const KLCSP::vec_field_order_t&     vecFieldsToOrder,
					long                                lMaxLifeTime,
                    std::wstring&		                wstrIterator,
                    const std::wstring&                 wstrLocalName,
                    const std::wstring&                 wstrRemoteName) = 0;

        virtual void HstGrp_GrpFindHostsAsync(
					const std::wstring&                 wstrRequestId,
                    const std::wstring&                 wstrFilter,
					const std::vector<std::wstring>&    vecFieldsToReturn,
                    const KLCSP::vec_field_order_t&     vecFieldsToOrder,
					KLPAR::Params*						pParams,
					long                                lMaxLifeTime,
                    const std::wstring&                 wstrLocalName,
                    const std::wstring&                 wstrRemoteName) = 0;

        virtual bool HstGrp_GrpFindHostsAsyncGetAccessor(
					const std::wstring&                 wstrRequestId,
					std::wstring&						wstrAccessor,
					KLPAR::Params**						ppFailedSlavesInfo,
					long&								lRecordsCount) = 0;

        virtual void HstGrp_GrpFindHostsAsyncCancel(
					const std::wstring&                 wstrRequestId) = 0;
        
        virtual void HstGrp_UseKeepConnection(
                    const wchar_t* szwHostId) = 0;
        
        virtual void NotifyTask(
                    const wchar_t* szwTaskName) = 0;
        
        virtual void HstGrp_DelGroupSubtree(
                    long        lId,
                    KLPRCI::ProductTasksControl* pTasksControl,
                    long                    lTaskId) = 0;
        
        virtual long HstGrp_GrpFindGroups(
                    const std::wstring&                 wstrFilter,
					const std::vector<std::wstring>&    vecFieldsToReturn,
                    const KLCSP::vec_field_order_t&     vecFieldsToOrder,
					long                                lMaxLifeTime,
                    std::wstring&		                wstrIterator,
                    const std::wstring&                 wstrLocalName,
                    const std::wstring&                 wstrRemoteName) = 0;

    };
    
    class KLSTD_NOVTABLE SrvRemoteAdHst
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void GetChildComputers(
                    long            idOU, 
                    const wchar_t** pFields,
                    size_t          nFields,
                    long            lTimeout,
                    std::wstring&   wstrIterator,
                    const std::wstring&                 wstrLocalName,
                    const std::wstring&                 wstrRemoteName) = 0;

        virtual void GetChildOUs(
                    long            idOU, 
                    const wchar_t** pFields,
                    size_t          nFields,
                    long            lTimeout,
                    std::wstring&   wstrIterator,
                    const std::wstring&                 wstrLocalName,
                    const std::wstring&                 wstrRemoteName) = 0;

        virtual void GetOU(
                    long            idOU, 
                    const wchar_t** pFields,
                    size_t          nFields,
                    KLPAR::Params** ppResult) = 0;
        

        virtual void UpdateOU(
                    long            idOU, 
                    KLPAR::Params*  pData) = 0;
    };

    class KLSTD_NOVTABLE SrvRemoteGsyn
        :   public KLSTD::KLBaseQI
    {
    public:
		virtual long Gsyn_AddSyncObjG(
						const wchar_t*			szwName,
					    const wchar_t*		    szwSyncObjLocation,
                        const wchar_t*		    szwProductName,
                        const wchar_t*		    szwProductVersion,
						KLGSYN::GSYN_DIRECTION	nDirection,
						long					lGroup,
						long					lElapse,
						bool					bCutOffChildren) =0;

		virtual long Gsyn_AddSyncObjH(
					const wchar_t*			szwName,
					const wchar_t*		    szwSyncObjLocation,
                    const wchar_t*		    szwProductName,
                    const wchar_t*		    szwProductVersion,
					KLGSYN::GSYN_DIRECTION	nDirection,
					KLGSYN::host_id_t*		pHosts,
					long					nHosts,
					long					lElapse) = 0;
		
		virtual void Gsyn_GetSyncInfo(
					long			lSync,
					const wchar_t**	pFields,
					int				nFields,
					KLPAR::Params**	ppInfo) = 0;
		
		virtual void Gsyn_RemoveSyncObj(long lSync) = 0;

        virtual size_t Gsyn_GetSyncHostsInfo(
					long				        lSync, 
					const std::vector<std::wstring>& vect_fields,
                    const KLCSP::vec_field_order_t& vect_fields_to_order,
					long				        lTimeout,
					std::wstring&		        wstrIterator,
                    const std::wstring&         wstrLocalName,
                    const std::wstring&         wstrRemoteName)  = 0;

		virtual size_t Gsyn_FindHosts(
					long				        lSync, 
					const wchar_t*		        szwFilter,
					const std::vector<std::wstring>& vect_fields,
					const KLCSP::vec_field_order_t&    vect_fields_to_order,
					long				        lTimeout,
					std::wstring&		        wstrIterator,
                    const std::wstring&         wstrLocalName,
                    const std::wstring&         wstrRemoteName)  = 0;
    };
    
    class KLSTD_NOVTABLE SrvRemoteNagentPings
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void OnPing(KLPAR::Params* pInfo, KLPAR::Params** ppResult) = 0;

        virtual void WatchdogPing() = 0;

        virtual void OnInstallerNotification(
                    KLPAR::Params*  pData,
                    KLPAR::Params** ppResult) = 0;
    };
    
    class KLSTD_NOVTABLE SrvRemoteLicInfo
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void GetKeysInfo(
                        const wchar_t**         ppFields,
                        int                     nFields,
                        const std::wstring&     wstrFilter,
                        std::wstring&           wstrIterator,
                        long                    lTimeout,
                        const std::wstring&     wstrLocalName,
                        const std::wstring&     wstrRemoteName) = 0;
        
		virtual long GetKeyInfo2(
						const std::vector<std::wstring>& vect_fields,
						const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
                        const std::wstring& wstrSerial,
                        std::wstring& wstrChunkAccessor,
                        long lTimeout,
                        const std::wstring& wstrLocalName,
                        const std::wstring& wstrRemoteName) = 0;

		virtual void DeleteKeyInfo(const std::wstring& wstrSerial) = 0;
    };
    
    class KLSTD_NOVTABLE SrvRemoteAdmLic
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void InstallKey(KLPAR::Params* pData) = 0;

        virtual void UninstallKey(KLPAR::Params* pData) = 0;

        virtual void GetKeyData(
                    KLPAR::Params*    pInData, 
                    KLPAR::Params**   ppResult) = 0;

        virtual void AdjustKey(
                            KLPAR::Params*  pData) = 0;
        
		virtual long EnumKeys(
						const std::vector<std::wstring>& vect_fields,
						const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
                        std::wstring& wstrChunkAccessor,
                        KLPAR::Params*  pData,
                        long lTimeout,
                        const std::wstring& wstrLocalName,
                        const std::wstring& wstrRemoteName) = 0;
    };

    class KLSTD_NOVTABLE SrvRemotePolicy
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void GetPolicyCommonData( 
                    long nGroupId, 
                    long lExpectedServerVersion,
                    KLPOL::PoliciesVector & vectPolicies ) = 0;

        virtual void GetSinglePolicyCommonData(
                const KLPOL::PolicyId & policyId,
                KLPOL::PolicyCommonData & policyCommonData ) = 0;

        virtual KLPOL::PolicyId AddPolicy(
                long nGroupId,
                KLPOL::PolicyCommonData & policyCommonData ) = 0;

        virtual KLPOL::PolicyId CopyPolicy(
                const KLPOL::PolicyId & policyId, 
                long nNewGroupId ) = 0;

        virtual KLPOL::PolicyId MovePolicy( 
                const KLPOL::PolicyId & policyId, 
                long nNewGroupId ) = 0;

        virtual void UpdatePolicyCommonData(
                const KLPOL::PolicyId & policyId,
                KLPOL::PolicyCommonData & policyCommonData ) = 0;

        virtual void DeletePolicy( const KLPOL::PolicyId & policyId ) = 0;

        virtual void GetSSId(
            const KLPOL::PolicyId&  policyId,
            long                    nTimeout,
            std::wstring &          wstrId ) = 0;
        
        virtual bool MakePolicyActive(
                        const KLPOL::PolicyId&  policyId, 
                        bool bActive) = 0;

        virtual void GetPoliciesForGroup( 
                    long nGroupId, 
                    long lExpectedServerVersion,
                    KLPOL::PoliciesVector & vectPolicies ) = 0;

        virtual KLPOL::PolicyId CopyOrMovePolicy( 
                const KLPOL::PolicyId&          policyId, 
                const KLPOL::PolicyCommonData&  policyCommonData, 
                bool                            bMove,
                long                            nNewGroupId) = 0;
        
        virtual void SetOutbreakPolicies(
                        const std::vector<KLPOL::PolicyId>& vecPolicies) = 0;

        virtual void GetOutbreakPolicies(
                        std::vector<KLPOL::PolicyId>& vecPolicies) = 0;
        
        virtual long GetPolicyGroupId(const KLPOL::PolicyId&  policyId) = 0;

        virtual bool MakePolicyRoaming(const KLPOL::PolicyId& policyId) = 0;
        
        virtual void GetAllAcivePoliciesForGroup( 
                    long nGroupId, 
                    KLPOL::PoliciesVector & vectPolicies ) = 0;
        
        virtual void SetOutbreakPolicies2(
                    KLPAR::Params* pInfo) = 0;

        virtual void GetOutbreakPolicies2(
                    KLPAR::Params** ppInfo) = 0;
    };
    
    class KLSTD_NOVTABLE SrvRemoteScanDiapasons
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual KLDPNS::diapason_id AddDiapason(
                            KLPAR::Params*  pData,
                            KLPAR::Params** ppInvalidIntervals) = 0;

        virtual bool UpdateDiapason(
                            KLDPNS::diapason_id id,
                            KLPAR::Params*      pData,
                            KLPAR::Params**     ppInvalidIntervals) = 0;
        
        virtual void RemoveDiapason(KLDPNS::diapason_id id) = 0;

        virtual void GetDiapason(
                            KLDPNS::diapason_id id,
                            const wchar_t**     pFields,
                            long                nFields,
                            KLPAR::Params**     ppResults) = 0;

        virtual void GetDiapasons(
                    const wchar_t**     pFields,
                    long                nFields,
                    long                lTimeout,
                    std::wstring&       wstrIterator,
                    const std::wstring& wstrLocalName,
                    const std::wstring& wstrRemoteName) = 0;
        
        virtual void NotifyDpnsTask() = 0;
    };

	/*
	 *	
		На данный момент данный интерфейс используется только для поддержки старой консоли
		 KLAK_VER_6_MP1
	 */
    class KLSTD_NOVTABLE SrvRemoteCleaner
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void GetCompetitorSoftwares(
							std::vector<std::wstring>& vecProducts) = 0;

		virtual void GetHostCompetitorSoftwares(
							long lHostId,
							std::vector<std::wstring>& vecProducts) = 0;
	};

	class KLSTD_NOVTABLE SrvRemotePatches
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual bool GetNagentLatestPatchInfo(
							KLPAR::Params*      pNagentData,
							KLPTCH::CPatchInfo&	patchInfo) = 0;
	};

	class KLSTD_NOVTABLE SrvRemoteInventory
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void GetHostInvProducts(
							long lHostId,
							std::vector<KLINV::CInvProductInfo>& vecProducts) = 0;
		virtual void GetHostInvPatches(
							long lHostId, 
							std::vector<KLINV::CInvPatchInfo>& vecInvPatches )  = 0;
		virtual void GetInvProducts(
							std::vector<KLINV::CInvProductInfo>& vecProducts) = 0;
		virtual void GetInvPatches(
							std::vector<KLINV::CInvPatchInfo>& vecInvPatches) = 0;
		virtual void GetSrvCompetitorIniFileInfoList(
							std::vector<KLCLEANER::CIniFileInfo>& vectIniInfo,
							const std::wstring& wstrType ) = 0;
	};
    
    class KLSTD_NOVTABLE SrvRulesExRemote
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual KLHST::HMOVERULE AddMoveRule(KLPAR::Params* pRuleInfo) = 0;
        
        virtual void UpdateMoveRule(KLHST::HMOVERULE hRule, KLPAR::Params* pRuleInfo) = 0;

        virtual void DeleteRule(KLHST::HMOVERULE hRule) = 0;

        virtual void SetRulesOrder(KLHST::HMOVERULE* pRules, size_t nRules) = 0;

        virtual void GetRule(
                        KLHST::HMOVERULE       hRule, 
                        KLPAR::Params** ppRuleInfo) = 0;

        virtual void GetRules(
                        const wchar_t** pFileds2Return,
                        size_t          nFileds2Return,
                        KLPAR::Params** ppRules) = 0;

        virtual void ExecuteRulesNow(
                        long                            lGroupId,
                        KLHST::HMOVERULE*               pRules, 
                        size_t                          nRules,
                        AVP_dword                       dwOptions,
                        long*                           plPercent) = 0;
    };
    
    class KLSTD_NOVTABLE SrvRemoteSrvHrchFind
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual long FindSlaveServers(
                    const std::wstring&                 wstrFilter,
					const std::vector<std::wstring>&    vecFieldsToReturn,
                    const KLCSP::vec_field_order_t&     vecFieldsToOrder,
					long                                lMaxLifeTime,
                    std::wstring&		                wstrIterator,
                    const std::wstring&                 wstrLocalName,
                    const std::wstring&                 wstrRemoteName) = 0;
    };
};
#endif //__KL_SRV_REMOTE_H__
