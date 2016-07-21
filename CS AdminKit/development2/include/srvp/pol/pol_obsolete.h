#ifndef __POL_OBSOLETE_H__
#define __POL_OBSOLETE_H__

#include <srvp/pol/policy_merging.h>

namespace KLPOL
{
    //! OBSOLETE
    typedef enum
    {
        PCDF_USE_DISPLAY_NAME = 0x1,
        PCDF_USE_ACTIVE = 0x2,
        PCDF_USE_ACCEPTANCE = 0x4,
        PCDF_DEFAULT = 0xFFFFFFFF
    }PcdFlags;


    //! OBSOLETE
    /*!
        \brief PolicyCommonData - общие данные для policy. */
    struct PolicyCommonData
    {
        PolicyCommonData()
            :   dwFlags(PCDF_DEFAULT)
            ,   dwVersion(5)
            ,   tCreated(KLSTD::c_tmInvalidTime)
            ,   tModified(KLSTD::c_tmInvalidTime)
            ,   bInherited(false)
            ,   nGroupSyncId(false)
            ,   bActive(false)
            ,   bRoaming(false)
            ,   bAcceptParent(true)
            ,   bForceDistrib2Children(false)
            ,   bForced(false)
            ,   nGroupId(-1L)
        {;};
        AVP_dword       dwFlags;
        AVP_dword       dwVersion;
        std::wstring    wstrDisplayName;
        std::wstring    wstrProductName;
        std::wstring    wstrProductVersion;
        time_t          tCreated;
        time_t          tModified;
        bool            bInherited;
        long            nGroupSyncId; //< id групповой синхронизации
        bool            bActive, bRoaming;
        bool            bAcceptParent; // policy may be changed by parent policy
        bool            bForceDistrib2Children; //sets bAcceptParent for all child policies
        bool            bForced; //readonly property -- bAcceptParent cannot be changed
        long            nGroupId;
    };

    typedef std::pair<PolicyId, PolicyCommonData> PolicyIdAndCommonData;
    typedef std::vector<PolicyIdAndCommonData> PoliciesVector;

    //! OBSOLETE
    /*!
        \brief Policies - интерфейс для редактирования Policies */
    class Policies : public KLSTD::KLBaseQI
    {
    public:

        /*!
          \brief	GetPolicyCommonData - выдает общие данные всех политик 

          \param	 long nGroupId [in] - ID группы
          \param	 PoliciesVector & vectPolicies [out] - вектор принимает в себя общие 
                        данные для всех policy данной группы. ID policy уникально среди
                        всех policy ВСЕХ групп.
        */
        virtual void GetPolicyCommonData( long nGroupId, PoliciesVector & vectPolicies ) = 0;

        /*!
          \brief	GetPolicyCommonData - выдает общие данные одной политик 

          \param	 long nGroupId [in] - ID группы
          \param	 PolicyCommonData & policyCommonData [out] - Общие данные для policy с заданым ID.
        */
        virtual void GetSinglePolicyCommonData(
            const PolicyId & policyId,
            PolicyCommonData & policyCommonData ) = 0;


        /*!
          \brief	AddPolicy - добавление policy.

          \param    long nGroupId [in] - ID группы, в которой надо создавать policy.
          \param	PolicyCommonData & policyCommonData [in/out] - общие данные для новой policy.
          \return	PolicyId - ID новой policy.
        */
        virtual PolicyId AddPolicy( long nGroupId, PolicyCommonData & policyCommonData ) = 0;

        /*!
          \brief	UpdatePolicyCommonData - updates specified policy propeties

          \param    policyId [in] - id of policy to change
          \param	policyCommonData [in] - attributes to change
                        Only following attributes may be changed if 
                        appropriate flags are set.
                        wstrDisplayName, 
                        bActive
        */
        virtual void UpdatePolicyCommonData(
            const KLPOL::PolicyId & policyId,
            const PolicyCommonData & policyCommonData ) = 0;

        /*!
          \brief	CopyPolicy - копирование policy.

          \param    policyId [in] - id политики, которую надо скопировать.
          \param    long nNewGroupId [in] - ID той группы, в которую надо скопировать policy.
          \return	PolicyId - ID новой policy.
        */
        virtual KLPOL::PolicyId CopyPolicy( const KLPOL::PolicyId & policyId, long nNewGroupId ) = 0;

        /*!
          \brief	MovePolicy - перемещение policy.

          \param    policyId [in] - id политики, которую надо переместить.
          \param    long nNewGroupId [in] - ID той группы, в которую надо скопировать policy.
          \return	PolicyId - ID новой policy (реально убиваетя старая и создается новая политика)
        */
        virtual KLPOL::PolicyId MovePolicy( const KLPOL::PolicyId & policyId, long nNewGroupId ) = 0;

        /*!
          \brief    DeletePolicy - удаление policy.

          \param	const PolicyId & policyId [in] - ID удаляемой policy.
          \return	void 
        */
        virtual void DeletePolicy( const PolicyId & policyId ) = 0;


        /*!
          \brief    GetPolicySettings - получение настроек policy.

          \param	const PolicyId & policyId [in] - ID policy.
          \param	nTimeout - время в мсек, в течении которого на сервере будет 
                        жить SettingsStorage.
          \return	CAutoPtr<KLPRSS::SettingsStorage> - интерфейс для работы с настройками policy.
  
        */
        virtual KLSTD::CAutoPtr<KLPRSSP::SettingsStorageWithApply>
            GetPolicySettings( const PolicyId & policyId, long nTimeout ) = 0;
    };
    
    class KLSTD_NOVTABLE Policies2: public Policies
    {
    public:
        /*!
          \brief	Initiates settings modification according to the 
                    KLPOL_PP_MERGING_TYPE variable and starts 
                    synchronization for all host managed by the specified 
                    policy.

          \param	policyId
        */
        virtual void ModifySettings(const PolicyId & policyId) = 0;

        /*!
          \brief	Copies or moves policy 

          \param	policyId            IN id of policy to copy or move
          \param	policyCommonData    IN only fields wstrDisplayName and
                                            bActive are used if appropriate 
                                            flags are set
          \param	bMove               IN if true moves policy otherwise copies it
          \param	nNewGroupId         IN if of group where create policy will be put
          \return	KLPOL::PolicyId     id of a new policy
        */
        virtual KLPOL::PolicyId CopyOrMovePolicy( 
                    const KLPOL::PolicyId&          policyId, 
                    const KLPOL::PolicyCommonData&  policyCommonData, 
                    bool                            bMove,
                    long                            nNewGroupId) = 0;

        /*!
          \brief	Set or clears activity flag from specified policy

          \param	policyId IN id of policy to modify
          \param	bActive  IN if true the policy becomes active otherwise policy becomes inactive
          \return	true if activity flag was really modified
        */
        virtual bool MakePolicyActive(
                        const KLPOL::PolicyId&  policyId, 
                        bool bActive) = 0;
        /*!
          \brief	Sets policies list that become active in case of virus outbreak

          \param	vecPolicies IN vector of policies
        */
        virtual void SetOutbreakPolicies(
                        const std::vector<KLPOL::PolicyId>& vecPolicies) = 0;

        /*!
          \brief	Returns policies list that become active in case of 
                    virus outbreak.

          \param	vecPolicies OUT vector of policies
        */
        virtual void GetOutbreakPolicies(
                        std::vector<KLPOL::PolicyId>& vecPolicies) = 0;
        /*!
          \brief	Returns all active policies data for given group. If 
                    there's no policy for given product it will be taken 
                    from parent group (or grandparent and so on).

          \param	nGroupId IN group id
          \param	vectPolicies OUT policies for given group
        */
        virtual void GetActivePolicies( long nGroupId, PoliciesVector & vectPolicies ) = 0;
    };

    class KLSTD_NOVTABLE Policies3: public Policies2
    {
    public:
        /*!
          \brief	Changes specified policy state to roaming

          \param	policyId IN polict to change state
        */
        virtual bool MakePolicyRoaming(const KLPOL::PolicyId&  policyId) = 0;
    };    
    
    class KLSTD_NOVTABLE Policies4: public Policies3
    {
    public:
        /*!
          \brief	Returns policies from specified group and all its parents

          \param	nGroupId IN group id
          \param	vectPolicies OUT result vector
        */
        virtual void GetAllAcivePoliciesForGroup(
                            long nGroupId, 
                            PoliciesVector & vectPolicies ) = 0;
    };

    class KLSTD_NOVTABLE Policies5: public Policies4
    {
    public:
        /*!
          \brief	Sets list of policies activated by virus outbreak. 
                    Order is important !!!!
                    if Administration Server version is less then KLADMSRV_SV6_MP2, 
                    c_szwOutbreakMask is always saved as VOT_MASK

          \param	pDara IN contains array of params c_szwPolOutbreakArray 
                    each item containing at least c_szwPolicyId and 
                    c_szwOutbreakMask.
          
                    c_szwPolOutbreakArray
                    []
                     +--c_szwPolicyId        INT
                     +--c_szwOutbreakMask    INT
                     
            Sample:
                KLPOL::PolicyId pPol[] = {idPol0, idPol1, idPol2};
                ArrayValuePtr pOutbreakArray = CreateArrayValue();
                pOutbreakArray->SetSize(KLSTD_COUNTOF(pPol));
                for(size_t i = 0; i < KLSTD_COUNTOF(pPol); ++i)
                {
                    const KLPOL::PolicyId lPolicyId = pPol[i];
                    const AVP_dword lMask = KLEVP::VOT_FILESYSTEM;

                    param_entry_t par[]= 
                    {
                        param_entry_t(c_szwPolicyId,        (long)lPolicyId),
                        param_entry_t(c_szwOutbreakMask,    (long)lMask)
                    };
                    ParamsPtr pItem;
                    KLPAR::CreateParamsBody(par, KLSTD_COUNTOF(par), &pItem);
                    ParamsValuePtr p_valItem;
                    CreateValue(pItem, &p_valItem);
                    pOutbreakArray->SetAt(i, p_valItem);
                };
                ParamsPtr pResult;
                param_entry_t par[]= 
                {
                    param_entry_t(c_szwPolOutbreakArray, pOutbreakArray)
                };
                KLPAR::CreateParamsBody(par, KLSTD_COUNTOF(par), &pResult);

                pPolicies->SetOutbreakPolicies2(pResult);
        */
        virtual void SetOutbreakPolicies2(
                        KLPAR::Params* pData) = 0;

        /*!
          \brief	Returns list of policies activated by virus outbreak. 
                    Order is important !!!!
                    if Administration Server version is less then KLADMSRV_SV6_MP2, 
                    c_szwOutbreakMask is returned as VOT_MASK

          \param	ppData OUT contains array of params c_szwPolOutbreakArray 
                    each item containing at least c_szwPolicyId and 
                    c_szwOutbreakMask.
          
                    c_szwPolOutbreakArray
                    []
                     +--c_szwPolicyId        INT
                     +--c_szwOutbreakMask    INT
        */
        virtual void GetOutbreakPolicies2(
                        KLPAR::Params** ppData) = 0;
    };
};

#endif //__POL_OBSOLETE_H__
