/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	hst_move_rules.h
 * \author	Andrew Kazachkov
 * \date	21.08.2007 11:51:26
 * \brief	
 * 
 */

#include <srvp/hst/hosts.h>

#ifndef __KL_HST_MOVE_RULES_H__
#define __KL_HST_MOVE_RULES_H__

namespace KLHST
{
    //! Rule ID (KLHST::HMOVERULE)
    const wchar_t c_szwMR_ID[] = L"KLHST_MR_ID"; // INT_T
    
    //! Rule Unique Display Name (up to 255 symbols)
    const wchar_t c_szwMR_DN[] = L"KLHST_MR_DN"; //STRING_T

    //! Group ID
    const wchar_t c_szwMR_Group[] = L"KLHST_MR_Group"; // INT_T

    //! Rule options (one of MRF_* constants)
    const wchar_t c_szwMR_Flags[] = L"KLHST_MR_Options"; // INT_T

    //! Rule query string, same as for Groups2::FindHosts method
    const wchar_t c_szwMR_Query[] = L"KLHST_MR_Query"; // STRING_T

    //! Rule type, one of MRT_*
    const wchar_t c_szwMR_Type[] = L"KLHST_MR_Type"; // INT_T

    //! Custom options -- any params provided by rule creator saved with the rule
    const wchar_t c_szwMR_CustomOptions[] = L"KLHST_MR_Custom"; // PARAMS_T

    //! used in GetRules method
    const wchar_t c_szwMR_RulesArray[] = L"KLHST_MR_RULES";   //ARRAY_T|PARAMS_T    

    KLSTD_DECLARE_HANDLE(HMOVERULE); // KLHST::HMOVERULE

    typedef enum 
    {
        MRF_RUN_ONCE_FOR_HOST = 0,      //! rule is running only once for host
        MRF_RUN_ONCE_FOR_NAGENT = 1,    //! rule is running only once for nagent installed, if nagent is reinstalled rule will be executed once more. 
        MRF_RUN_ALWAYS = 2              //! rule runs periodically
    }
    MoveRuleOptions;

    typedef enum
    {
        MRT_CUSTOM = 0,                 //! custom rule defined by c_szwMR_Query string (default)
        MRT_LEGACY = 1,                 //! legacy rule automatically created while upgrading administration server from pre-6MP2 version
        MRT_REMOTE_INSTALL_HELPER = 2,  //! rule created indirectly by remote installation task for automatic moving hosts after remote installation
        MRT_AK_AUTOMATION = 3           //! rule created by user custom script via "Kaspersky Administration Kit Automation"
    }
    MoveRuleType;

    typedef enum
    {
        MREO_RUN_FOR_REQUIRED = 0,  //! rule is processed for hosts that need it
        MREO_RUN_FORCE = 1          //! execute even rule with MRF_RUN_ONCE_FOR_HOST or MRF_RUN_ONCE_FOR_NAGENT set has been already executed

    }
    MoveRuleExecuteOption;
    
    typedef enum
    {
        MRLT_UNASSIGNED = 1,    //! move from unassigned
        MRLT_DPNS = 2,          //! move from ip diapason
        MRLT_AD = 3             //! move from ad OU
    }MoveRuleLegacyType;

    //! source object id, INT_T
    const wchar_t c_szwMR_Legacy_nSrcObject[] = L"KLHST_MR_LEGACY_nSrcObject";

    //! destination group, INT_T
    const wchar_t c_szwMR_Legacy_nDstGroup[] = L"KLHST_MR_LEGACY_nDstGroup";

    //! one of MRLT_*, INT_T
    const wchar_t c_szwMR_Legacy_nType[] = L"KLHST_MR_LEGACY_nType";

    //! Move only new hosts, BOOL_T
    const wchar_t c_szwMR_Legacy_bNewOnly[] = L"KLHST_MR_LEGACY_bNewOnly";

    /*!
        c_szwMR_CustomOptions format for MRT_LEGACY

            |
            +--c_szwMR_Legacy_nSrcObject
            +--c_szwMR_Legacy_nDstGroup
            +--c_szwMR_Legacy_nType
            +--c_szwMR_Legacy_bNewOnly
    */

    /*!
        Administration server contains global list rules that may automatically move computers 
        Order of rules in the list is important. 
    */
    class KLSTD_NOVTABLE HstMoveRules
        :   public KLSTD::KLBaseQI
    {
    public:

        /*!
          \brief	Adds new rule

          \param	pRuleInfo IN rule parameters. 
                    Fields  c_szwMR_DN, c_szwMR_Group, c_szwMR_Flags, 
                            c_szwMR_Query are necessary. 
          \return	id of a new rule
        */
        virtual HMOVERULE AddRule(KLPAR::Params* pRuleInfo) = 0;
        
        /*!
          \brief	Updates rule properties

          \param	hRule IN rule id
          \param	pRuleInfo IN rule fields to modify
        */
        virtual void UpdateRule(HMOVERULE hRule, KLPAR::Params* pRuleInfo) = 0;

        /*!
          \brief	Deletes rule

          \param	hRule IN id of rule to delete
        */
        virtual void DeleteRule(HMOVERULE hRule) = 0;


        /*!
          \brief	Modifies order of specified rules in the global list. 
                    Order of rules not cotnained in pRules array will be 
                    indefinite. 

          \param	pRules IN array rule ids
          \param	nRules IN numer of rule ids in the array
        */
        virtual void SetRulesOrder(HMOVERULE* pRules, size_t nRules) = 0;

        /*!
          \brief	Get info for specified rule

          \param	hRule           IN rule id
          \param	ppRuleInfo      OUT info
        */
        virtual void GetRule(
                        HMOVERULE       hRule, 
                        KLPAR::Params** ppRuleInfo) = 0;


        /*!
          \brief	Enumerates all rules.
                    Returns specified info for rules in c_szwMR_RulesArray 
                    array of params containers. 

          \param	hRule           IN rule id
          \param	pFileds2Return  IN fields to return
          \param	nFileds2Return  IN number of fields to return
          \param	ppRuleInfo      OUT requested info
        */
        virtual void GetRules(
                        const wchar_t** pFileds2Return,
                        size_t          nFileds2Return,
                        KLPAR::Params** ppRules) = 0;

        /*!
          \brief	ExecuteRulesNow

          \param	lGroupId IN group 
          \param	pRules
          \param	nRules
          \param	dwOptions   IN   one of MREO_*
          \param	pSink
          \param	hSink
        */
        virtual void ExecuteRulesNow(
                        long                                lGroupId,
                        HMOVERULE*                          pRules, 
                        size_t                              nRules,
                        AVP_dword                           dwOptions,
                        KLADMSRV::AdmServerAdviseSink*      pSink,
                        KLADMSRV::HSINKID&                  hSink) = 0;
    };
};

#endif //__KL_HST_MOVE_RULES_H__
