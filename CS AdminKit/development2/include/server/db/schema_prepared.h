#include <std/base/klstd.h>
#include <server/db/dbconnection.h>
#include <string>


namespace KLDBPST
{
    class DbSqlPst : public KLSTD::KLBaseQI
    {
    public:
        virtual KLDB::DbRecordsetPtr pstmt_evp_SendMessages_GetTop_NoLimits(
                KLDB::DbConnectionPtr    pCon,
                int arg1) = 0;
        virtual KLDB::DbRecordsetPtr pstmt_evp_SendMessages_GetTop_WithLimits(
                KLDB::DbConnectionPtr    pCon,
                int arg1,
                int arg2) = 0;
        virtual void pstmt_nsa_create_tmp_nsa_interval(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual KLDB::DbRecordsetPtr pstmt_grp_get_super(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual KLDB::DbRecordsetPtr pstmt_grp_can_remove_group(
                KLDB::DbConnectionPtr    pCon,
                int arg1) = 0;
        virtual void pstmt_hst_create_tmp_hst_task_states(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_evp_create_tmp_templist(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_gsyn_create_tmp_gsyn_hosts(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_tsk_create_tmp_host_list(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_nlst_create_tmp_nlst_chunk(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_evp_create_tmp_evp_chunk(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_ping_create_temp_hst_mac(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_rpt_create_rpt_host_tmp(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_rpt_create_rpt_prod_loc_tmp(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_rpt_fill_rpt_prod_loc(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_rpt_fill_rpt_prod_grp_loc(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_rpt_protection_preexec(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_rpt_viract_preexec(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_rpt_avdbver_preexec(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_rpt_softver_preexec(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_rpt_errors_preexec(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_rpt_lic_preexec(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_rpt_most_infected_preexec(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_rpt_most_net_attack_preexec(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_rpt_most_test_preexec(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_rpt_create_rpt_stat_histogram(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_rpt_competitor_soft_preexec(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_rpt_inventory_preexec(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_hst_create_hosts(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual KLDB::DbRecordsetPtr pstmt_pkg_get_count_with_lock_by_id(
                KLDB::DbConnectionPtr    pCon,
                int arg1) = 0;
        virtual KLDB::DbRecordsetPtr pstmt_pkg_get_count_with_lock_by_name(
                KLDB::DbConnectionPtr    pCon,
                const std::wstring& arg1) = 0;
        virtual KLDB::DbRecordsetPtr pstmt_bkp_clean_gshosts(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_gsyn_create_tmp_addhost_for_gsupdate(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_hst_create_tmp_hosts_to_multiremove(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_hst_add_host_to_remove(
                KLDB::DbConnectionPtr    pCon,
                const std::wstring& arg1) = 0;
        virtual void pstmt_hst_drop_tmp_hosts_to_multiremove(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_hst_create_tmp_hst_multi_move(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_hst_drop_tmp_hst_multi_move(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual KLDB::DbRecordsetPtr pstmt_tsk_get_global_tasks(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_rpt_avp_prod_fs_preexec(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_rpt_avp_prod_eml_preexec(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_rpt_avp_prod_ph_preexec(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_rpt_avp_prod_preexec(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_tsk_upd_delete_all_au_tasks(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_tsk_upd_delete_all_au_patches(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_tsk_create_tmp_visible_host_list(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_srvh_create_tmp_products(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_srvh_create_tmp_components(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual KLDB::DbRecordsetPtr pstmt_gsyn_acquire_to_recalc(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_hst_create_tmp_hst_hosts2calc_statuses(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_hst_drop_tmp_hst_hosts2calc_statuses(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_hst_create_tmp_hst_host_times_table(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_hst_drop_tmp_hst_host_times_table(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_hst_create_tmp_hst_set_group_for_hosts(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_hst_drop_tmp_hst_set_group_for_hosts(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_hst_tmp_update_last_update(
                KLDB::DbConnectionPtr    pCon,
                KLSTD::precise_time_t arg1,
                int arg2) = 0;
        virtual void pstmt_pkg_create_tmp_products(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_pkg_create_tmp_components(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_rpt_most_infecting_usr_preexec(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_hst_hst_move_and_resolve_temp_table(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_hst_move_rule_ex_create_temp_tables(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_hst_move_rule_ex_drop_temp_tables(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_hst_create_tmp_hst_rule_ex_order(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_hst_drop_tmp_hst_rule_ex_order(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_grp_create_grp_child_chain(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_grp_drop_grp_child_chain(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_grp_create_adhst_child_chain(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_grp_drop_adhst_child_chain(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_invlst_create_tmp_invlst_chunk(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_hst_create_tmp_grp_found_groups(
                KLDB::DbConnectionPtr    pCon) = 0;
        virtual void pstmt_hst_drop_tmp_grp_found_groups(
                KLDB::DbConnectionPtr    pCon) = 0;
    };
};
