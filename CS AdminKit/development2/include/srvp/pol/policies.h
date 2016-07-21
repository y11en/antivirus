/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	policies.h
 * \author	Mikhail Karmazine, Andrew Kazachkov
 * \date	09.11.2007 14:29:26
 * \brief	policies control
 * 
 */


#ifndef __KLPOL_POLICIES_H__
#define __KLPOL_POLICIES_H__

#include <vector>

#include <std/base/klstd.h>
#include <srvp/prssp/prsspclient.h>

namespace KLPOL
{
    //! policy outbreak array
    const wchar_t c_szwPolOutbreakArray[] = L"KLPOL_POL_OUTBREAK"; //ARRAY_T

    //! policy id
    const wchar_t c_szwPolicyId[] = L"KLPOL_ID"; //INT_T

    //! display name
    const wchar_t c_szwDisplayName[] = L"KLPOL_DN"; //STRING_T

    //! product name
    const wchar_t c_szwProductName[] = L"KLPOL_PRODUCT"; //STRING_T

    //! product version
    const wchar_t c_szwVersionName[] = L"KLPOL_VERSION"; //STRING_T
        
    //! read-only property
    const wchar_t c_szwCreated[] = L"KLPOL_CREATED"; //DATETIME_T

    //! read-only property
    const wchar_t c_szwModified[] = L"KLPOL_MODIFIED"; //DATETIME_T

    //! read-only property
    const wchar_t c_szwInherited[] = L"KLPOL_INHERITED"; // BOOL_T

    //! read-only property
    const wchar_t c_szwGsynId[] = L"KLPOL_GSYN_ID"; //INT_T

    //! read-only property
    const wchar_t c_szwGroupId[] = L"KLPOL_GROUP_ID"; //INT_T

    //! read-only property
    const wchar_t c_szwActive[] = L"KLPOL_ACTIVE"; // BOOL_T

    //! read-only property
    const wchar_t c_szwRoaming[] = L"KLPOL_ROAMING"; // BOOL_T

    //! policy may be changed by parent policy, BOOL_T
    const wchar_t c_szwAcceptParent[] = L"KLPOL_ACCEPT_PARENT";

    //! sets bAcceptParent for all child policies, BOOL_T
    const wchar_t c_szwForceDistrib2Children[]  = L"KLPOL_FORCE_DISTRIB2CHILDREN";

    //! readonly property -- c_szwAcceptParent cannot be changed, BOOL_T
    const wchar_t c_szwForced[]  = L"KLPOL_FORCED";

    //! policy outbreaks, combination of EVirusOutbreakType 
    const wchar_t c_szwOutbreakMask[] = L"KLPOL_OUTBREAK_MASK"; //INT_T

    //! ARRAY_T
    const wchar_t c_szwPolicies[] = L"KLPOL_POLICIES"; //ARRAY_T


    typedef long PolicyId;

    class SrvPolicies
        :   public KLSTD::KLBaseQI
    {
    public:
        /*!
          \brief	GetPoliciesForGroup

          \param	nGroupId
          \param	bActiveOnly
          \param	bIncludeInherited
          \param	ppPolicies contains c_szwPolicies as array of params
        */
        virtual void GetPoliciesForGroup(
                                long nGroupId, 
                                KLPAR::Params** ppPolicies ) = 0;

        /*!
          \brief	Returns all active policies data for given group. If 
                    there's no policy for given product it will be taken 
                    from parent group (or grandparent and so on).

          \param	nGroupId IN group id
          \param	ppPolicies contains c_szwPolicies as array of params
        */
        virtual void GetActivePolicies( long nGroupId, KLPAR::Params** ppPolicies ) = 0;

        /*!
          \brief	Returns policies from specified group and all its parents
                    Server version KLADMSRV_SV6_MP1

          \param	nGroupId IN group id
          \param	ppPolicies OUT  contains c_szwPolicies as array of params
        */
        virtual void GetAllAcivePoliciesForGroup(
                            long nGroupId, 
                            KLPAR::Params** ppPolicies ) = 0;

        /*!
          \brief	GetPolicyData

          \param	nPolicy
          \param	ppPolicy
        */
        virtual void GetPolicyData(
                        PolicyId nPolicy,
                        KLPAR::Params** ppPolicy) = 0;

        /*!
          \brief	Creates new policy
          \param	pPolicy -- at least must contain fields:
                        c_szwDisplayName,
                        c_szwProductName,
                        c_szwVersionName,
                        c_szwGroupId

        */
        virtual PolicyId AddPolicy(
                        KLPAR::Params* pPolicy) = 0;

        /*!
          \brief	UpdatePolicyData

          \param	olicyId nPolicy
          \param	KLPAR::Params* pPolicy
        */
        virtual void UpdatePolicyData(
                        PolicyId nPolicy,
                        KLPAR::Params* pPolicy) = 0;

        /*!
          \brief	Copies or moves policy 

          \param	policyId            IN id of policy to copy or move
          \param	pPolicy             IN only fields wstrDisplayName and
                                            bActive are used if appropriate 
                                            flags are set, used only for KLADMSRV_SV_MP3 server version
          \param	bMove               IN if true moves policy otherwise copies it
          \param	nNewGroupId         IN if of group where create policy will be put
          \return	KLPOL::PolicyId     id of a new policy
        */
        virtual KLPOL::PolicyId CopyOrMovePolicy( 
                    KLPOL::PolicyId policyId, 
                    KLPAR::Params*  pPolicy,
                    bool            bMove,
                    long            nNewGroupId) = 0;

        /*!
          \brief    DeletePolicy - delete policy

          \param	policyId  ID of policy to delete
        */
        virtual void DeletePolicy(PolicyId policyId ) = 0;


        /*!
          \brief    GetPolicySettings - получение настроек policy.

          \param	const PolicyId & policyId [in] - ID policy.
          \param	nTimeout - время в мсек, в течении которого на сервере будет 
                        жить SettingsStorage.
          \param    ppSettingsStorageWithApply 
  
        */
        virtual void GetPolicySettings( 
                        PolicyId                            policyId, 
                        long                                nTimeout, 
                        KLPRSSP::SettingsStorageWithApply** ppSettingsStorageWithApply) = 0;

        /*!
          \brief	Set or clears activity flag from specified policy
                    Server version KLADMSRV_SV_MP3

          \param	policyId IN id of policy to modify
          \param	bActive  IN if true the policy becomes active otherwise policy becomes inactive
          \return	true if activity flag was really modified
        */
        virtual bool MakePolicyActive(
                        KLPOL::PolicyId policyId, 
                        bool            bActive) = 0;

        /*!
          \brief	Changes specified policy state to roaming
                    Server version KLADMSRV_SV_MP3

          \param	policyId IN polict to change state
        */
        virtual bool MakePolicyRoaming(KLPOL::PolicyId  policyId) = 0;

        /*!
          \brief	Sets list of policies activated by virus outbreak. 
                    Order is important !!!!
                    if Administration Server version is less then KLADMSRV_SV6_MP2, 
                    c_szwOutbreakMask is always saved as VOT_MASK

                    Server version KLADMSRV_SV_MP3

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
        virtual void SetOutbreakPolicies(
                        KLPAR::Params* pData) = 0;

        /*!
          \brief	Returns list of policies activated by virus outbreak. 
                    Order is important !!!!
                    if Administration Server version is less then KLADMSRV_SV6_MP2, 
                    c_szwOutbreakMask is returned as VOT_MASK

                    Server version KLADMSRV_SV_MP3

          \param	ppData OUT contains array of params c_szwPolOutbreakArray 
                    each item containing at least c_szwPolicyId and 
                    c_szwOutbreakMask.
          
                    c_szwPolOutbreakArray
                    []
                     +--c_szwPolicyId        INT
                     +--c_szwOutbreakMask    INT
        */
        virtual void GetOutbreakPolicies(
                        KLPAR::Params** ppData) = 0;
    };
};

#include <srvp/pol/pol_obsolete.h>

#endif //__KLPOL_POLICIES_H__