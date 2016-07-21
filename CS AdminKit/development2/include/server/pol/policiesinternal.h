#ifndef __KL_POLICIESINTERNAL_H__
#define __KL_POLICIESINTERNAL_H__

#include <srvp/pol/policies.h>

namespace KLPOL
{
    class AddSuperPolicyCallback
    {
    public:
        virtual void OnPolicySsCreating(const wchar_t* szwPath) = 0;
    };

    class PoliciesInternal
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual KLPOL::PolicyId RecreateSuperPolicy(
                            KLDB::DbConnectionPtr   pCon,
                            long                    nSuperGroupId,
                            const std::wstring&     wstrProduct,
                            const std::wstring&     wstrVersion,
                            const std::wstring&     wstrDN,
                            AddSuperPolicyCallback* pCallback,
                            bool                    bIsRoaming,
                            bool                    bForceChildren2Distrib) = 0;

        virtual void DeleteSuperPolicy(
                            KLDB::DbConnectionPtr   pCon,
                            long                    nSuperGroupId,
                            const std::wstring&     wstrProduct,
                            const std::wstring&     wstrVersion,
                            bool                    bIsRoaming) = 0;

        virtual void OnGroupMoved(
                            KLDB::DbConnectionPtr   pCon,
                            long lGroup,
                            long lOldParent,
                            long lNewParent) = 0;
        
        virtual void BeforeVirusOutbreak(int nOutbreakType) = 0;

        virtual void AfterVirusOutbreak(int nOutbreakType) = 0;
        
        virtual void DeletePolicy(
                        KLDB::DbConnectionPtr   pCon,
                        const KLPOL::PolicyId & policyId ) = 0;
        
        virtual bool OnDeferredAction(
                            KLDB::DbConnectionPtr   pCon,
                            int                     nExecOrder,
                            int                     nModule, 
                            int                     nActionCode,
                            int                     nPar1,
                            int                     nPar2,
                            const wchar_t*          wstrPar1,
                            const wchar_t*          wstrPar2) = 0;
    };
};

#endif //__KL_POLICIESINTERNAL_H__