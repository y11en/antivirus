/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	policies.h
 * \author	Mikhail Karmazine
 * \date	09.04.2003 19:02:43
 * \brief	интерфейс дл€ редактировани€ Policy
 * 
 */


#ifndef __KLPOL_POLICIES_H__
#define __KLPOL_POLICIES_H__

#pragma warning  (disable : 4786 )
#include <vector>

#include <std/base/klbase.h>
#include <std/trc/trace.h>
#include <kca/prssp/prsspclient.h>

namespace KLPOL
{
    /*! 
        \brief PolicyId - тип, использующийс€ в качестве 
        уникального идентификатора дл€ policy */
    typedef long PolicyId;

    /*!
        \brief PolicyCommonData - общие данные дл€ policy. */
    struct PolicyCommonData
    {
        std::wstring wstrDisplayName;
        std::wstring wstrProductName;
        std::wstring wstrProductVersion;
        time_t tCreated;
        time_t tModified;
        bool bInherited;
        long nGroupSyncId; //< id групповой синхронизации
    };

    typedef std::pair<PolicyId, PolicyCommonData> PolicyIdAndCommonData;
    typedef std::vector<PolicyIdAndCommonData> PoliciesVector;

    class AddSuperPolicyCallback
    {
    public:
        virtual void OnPolicySsCreating(const wchar_t* szwPath) = 0;
    };

    /*!
        \brief Policies - интерфейс дл€ редактировани€ Policies */
    class Policies : public KLSTD::KLBaseQI
    {
    public:

        /*!
          \brief	GetPolicyCommonData - выдает общие данные всех политик 

          \param	 long nGroupId [in] - ID группы
          \param	 PoliciesVector & vectPolicies [out] - вектор принимает в себ€ общие 
                        данные дл€ всех policy данной группы. ID policy уникально среди
                        всех policy ¬—≈’ групп.
        */
        virtual void GetPolicyCommonData( long nGroupId, PoliciesVector & vectPolicies ) = 0;

        /*!
          \brief	GetPolicyCommonData - выдает общие данные одной политик 

          \param	 long nGroupId [in] - ID группы
          \param	 PolicyCommonData & policyCommonData [out] - ќбщие данные дл€ policy с заданым ID.
        */
        virtual void GetSinglePolicyCommonData(
            const PolicyId & policyId,
            PolicyCommonData & policyCommonData ) = 0;


        /*!
          \brief	AddPolicy - добавление policy.

          \param    long nGroupId [in] - ID группы, в которой надо создавать policy.
          \param	PolicyCommonData & policyCommonData [in/out] - общие данные дл€ новой policy.
          \return	PolicyId - ID новой policy.
        */
        virtual PolicyId AddPolicy( long nGroupId, PolicyCommonData & policyCommonData ) = 0;

        /*!
          \brief	UpdatePolicyCommonData - изменение общих данные политики 

          \param    policyId [in] - id политики, общие данные которой надо изменить.
          \param	PolicyCommonData & policyCommonData [in/out] - общие данные дл€ новой policy.
                »з атрибутов PolicyCommonData имеют значение только следующие:
                    wstrDisplayName [in]
                    tModified [out] !!! этот только после беты !!!
        */
        virtual void UpdatePolicyCommonData(
            const KLPOL::PolicyId & policyId,
            PolicyCommonData & policyCommonData ) = 0;

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
          \return	PolicyId - ID новой policy (реально убивает€ стара€ и создаетс€ нова€ политика)
        */
        virtual KLPOL::PolicyId MovePolicy( const KLPOL::PolicyId & policyId, long nNewGroupId ) = 0;

        /*!
          \brief    DeletePolicy - удаление policy.

          \param	const PolicyId & policyId [in] - ID удал€емой policy.
          \return	void 
        */
        virtual void DeletePolicy( const PolicyId & policyId ) = 0;


        /*!
          \brief    GetPolicySettings - получение настроек policy.

          \param	const PolicyId & policyId [in] - ID policy.
          \param	nTimeout - врем€ в мсек, в течении которого на сервере будет 
                        жить SettingsStorage.
          \return	CAutoPtr<KLPRSS::SettingsStorage> - интерфейс дл€ работы с настройками policy.
  
        */
        virtual KLSTD::CAutoPtr<KLPRSSP::SettingsStorageWithApply>
            GetPolicySettings( const PolicyId & policyId, long nTimeout ) = 0;


        /*!
          \brief	GetSSId - ‘ункиц€ дл€ внутреннего использовани€.

          \param	const PolicyId & policyId
          \param	long nTimeout
          \param	std::wstring wstrId
        */
        virtual void GetSSId(
            const PolicyId & policyId,
            long nTimeout,
            std::wstring & wstrId ) = 0;


        /*!
          \brief	GetSSRelease - ‘ункиц€ дл€ внутреннего использовани€.

          \param	const std::wstring & wstrId
        */
        //virtual void SSRelease( const std::wstring & wstrId ) = 0;

        virtual PolicyId RecreateSuperPolicy(
                                long nSuperGroupId,
                                const std::wstring& wstrProduct,
                                const std::wstring& wstrVersion,
                                AddSuperPolicyCallback* pCallback) = 0;

        virtual void DeleteSuperPolicy(
                                long nSuperGroupId,
                                const std::wstring& wstrProduct,
                                const std::wstring& wstrVersion) = 0;
    };
};

#endif //__KLPOL_POLICIES_H__
