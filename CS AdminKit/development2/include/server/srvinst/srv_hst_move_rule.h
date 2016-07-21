/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	srv_hst_move_rule.h
 * \author	Andrew Kazachkov
 * \date	24.08.2007 14:00:15
 * \brief	Server-side implementation of extended host moving rules
 * 
 */

#include <srvp/hst/hst_move_rules.h>
#include <server/srvinst/klserver.h>

#ifndef __KL_SRV_HST_MOVE_RULE_H__
#define __KL_SRV_HST_MOVE_RULE_H__

namespace KLHST
{
    typedef std::vector<KLHST::HMOVERULE> vec_hstrules_t;
    typedef std::vector<long> vec_hstrule_groups_t;

    struct exec_hstrules_t
    {
        exec_hstrules_t()
            :   lGroupId(KLGRP::GROUPID_INVALID_ID)
            ,   dwOptions(0)
        {;};

        long                    lGroupId;
        vec_hstrules_t          vecRules; 
        AVP_dword               dwOptions;
        vec_hstrule_groups_t    vecGroups;
    };

    struct do_exec_hstrules_t
        :   exec_hstrules_t
    {
        do_exec_hstrules_t()
            :   plPercent(NULL)
        {;};
        long* plPercent;
    };

    class KLSTD_NOVTABLE SrvHstMoveRule
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual KLHST::HMOVERULE SrvAddMoveRule(
                    KLDB::DbConnectionPtr   pCon,
                    KLPAR::Params* pRuleInfo) = 0;
        
        virtual void SrvUpdateMoveRule(
                        KLDB::DbConnectionPtr   pCon,
                        KLHST::HMOVERULE hRule, 
                        KLPAR::Params* pRuleInfo) = 0;

        virtual void SrvDeleteRule(
                        KLDB::DbConnectionPtr   pCon,
                        KLHST::HMOVERULE hRule) = 0;

        virtual void SrvSetRulesOrder(
                        KLDB::DbConnectionPtr   pCon,
                        KLHST::HMOVERULE* pRules, size_t nRules) = 0;

        virtual void SrvGetRule(
                        KLDB::DbConnectionPtr   pCon,
                        KLHST::HMOVERULE       hRule, 
                        KLPAR::Params** ppRuleInfo) = 0;

        virtual void SrvGetRules(
                        KLDB::DbConnectionPtr   pCon,
                        const wchar_t** pFileds2Return,
                        size_t          nFileds2Return,
                        KLPAR::Params** ppRules) = 0;

        virtual void SendExecuteRules(
                        const do_exec_hstrules_t& data) = 0;
    };

    class KLSTD_NOVTABLE SrvRulesExCallbacks
        :   public KLSTD::KLBaseQI
    {
    public:
        virtual void OnAdmGroupChanged(
            KLDB::DbConnectionPtr       pCon,
            const KLSRV::AdmGrpChanged*        pData,
            size_t                      nData) = 0;
    };

};

#endif //__KL_SRV_HST_MOVE_RULE_H__
