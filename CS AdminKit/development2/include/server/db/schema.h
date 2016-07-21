#include <std/base/klstd.h>
#include <server/db/dbconnection.h>
#include <string>

namespace KLDBSQL
{
	#define KLDBCONST_UT_COMPONENT_NAME_COUNT	100

	#define KLDBCONST_UT_DISPLAY_NAME_COUNT	300

	#define KLDBCONST_UT_DOMAIN_NAME_COUNT	256

	#define KLDBCONST_UT_EVENT_PARAM_COUNT	512

	#define KLDBCONST_UT_EVP_EVENT_ID_COUNT	70

	#define KLDBCONST_UT_EXT_TASK_ID_COUNT	50

	#define KLDBCONST_UT_FIELD_NAME_COUNT	100

	#define KLDBCONST_UT_HOST_NAME_COUNT	256

	#define KLDBCONST_UT_HOST_DISPLAY_NAME_COUNT	256

	#define KLDBCONST_UT_PRODUCT_NAME_COUNT	100

	#define KLDBCONST_UT_PRODUCT_VERSION_COUNT	100

	#define KLDBCONST_UT_SECTION_NAME_COUNT	31

	#define KLDBCONST_UT_SRC_INST_ID_COUNT	50

	#define KLDBCONST_UT_TASK_NAME_COUNT	100

	#define KLDBCONST_UT_UPD_BUNDLE_ID_COUNT	300

    void kldb_pol_get_group_id (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_policy_id /*count = 0*/, 
        int& /*out*/    par_group_id /*count = 0*/);

    void kldb_hst_update_product_state_for_host (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/, 
        const std::wstring& /*in*/    par_product /*count = 32*/, 
        const std::wstring& /*in*/    par_version /*count = 32*/, 
        int /*in*/    par_state /*count = 0*/);

    void kldb_pol_get_roaming_policy_in_group (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/, 
        const std::wstring& /*in*/    par_product /*count = KLDBCONST_UT_PRODUCT_NAME_COUNT*/, 
        const std::wstring& /*in*/    par_version /*count = KLDBCONST_UT_PRODUCT_VERSION_COUNT*/, 
        int& /*out*/    par_policy_id /*count = 0*/);

    void kldb_pol_get_active_policy_in_group (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/, 
        const std::wstring& /*in*/    par_product /*count = KLDBCONST_UT_PRODUCT_NAME_COUNT*/, 
        const std::wstring& /*in*/    par_version /*count = KLDBCONST_UT_PRODUCT_VERSION_COUNT*/, 
        int& /*out*/    par_policy_id /*count = 0*/);

    void kldb_pol_get_policy_opt (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_policy_id /*count = 0*/, 
        int& /*out*/    par_opt /*count = 0*/, 
        int& /*out*/    par_sync_id /*count = 0*/);

    void kldb_pol_find_policy_by_gsyn (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_gsyn_id /*count = 0*/, 
        int& /*out*/    par_policy_id /*count = 0*/, 
        std::wstring& /*out*/    par_display_name /*count = KLDBCONST_UT_DISPLAY_NAME_COUNT*/);

    void kldb_pol_set_accept_parent (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_policy_id /*count = 0*/, 
        bool /*in*/    par_bAccept /*count = 0*/);

    void kldb_pol_set_accept_force_distrib2children (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_policy_id /*count = 0*/, 
        bool /*in*/    par_bForce /*count = 0*/);

    void kldb_pol_fix_accept_parent (
        KLDB::DbConnectionPtr    pCon);

    void kldb_hst_add_competitor_product_to_srv (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_product_name /*count = 256*/, 
        int& /*out*/    par_idResult /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_hst_get_competitor_products (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_get_competitor_products (
        KLDB::DbConnectionPtr    pCon);

    void kldb_srvhrch_on_first_ping (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_wstrPublicKeyHash /*count = 32*/, 
        bool& /*out*/    par_is_first /*count = 0*/, 
        int& /*out*/    par_host_id /*count = 0*/, 
        int& /*out*/    par_num_version /*count = 0*/, 
        bool& /*out*/    par_bNewIntegrationRequired /*count = 0*/);

    void kldb_srvhrch_on_updating_gsyn (
        KLDB::DbConnectionPtr    pCon, 
        KLSTD::MemoryChunkPtr /*in*/    par_binKey /*count = 16*/, 
        const std::wstring& /*in*/    par_wstrType /*count = 512*/, 
        const std::wstring& /*in*/    par_wstrRelPath /*count = 260*/, 
        int /*in*/    par_nType /*count = 0*/);

    void kldb_srvhrch_on_updated_gsyn (
        KLDB::DbConnectionPtr    pCon, 
        KLSTD::MemoryChunkPtr /*in*/    par_binKey /*count = 16*/);

    void kldb_srvhrch_get_gsyn (
        KLDB::DbConnectionPtr    pCon, 
        KLSTD::MemoryChunkPtr /*in*/    par_binKey /*count = 16*/, 
        std::wstring& /*out*/    par_wstrType /*count = 512*/, 
        std::wstring& /*out*/    par_wstrRelPath /*count = 260*/, 
        int& /*out*/    par_nType /*count = 0*/);

    void kldb_srvhrch_on_removing_gsyn (
        KLDB::DbConnectionPtr    pCon, 
        KLSTD::MemoryChunkPtr /*in*/    par_binKey /*count = 16*/);

    void kldb_srvhrch_on_removed_gsyn (
        KLDB::DbConnectionPtr    pCon, 
        KLSTD::MemoryChunkPtr /*in*/    par_binKey /*count = 16*/);

    void kldb_hst_create_windomain (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_windomain /*count = 256*/, 
        int /*in*/    par_windomaintype /*count = 0*/, 
        int& /*out*/    par_nNtDomain /*count = 0*/);

    void kldb_hst_create_dnsdomain (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_dnsdomain /*count = 256*/, 
        int& /*out*/    par_nDnsDomain /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_hst_gethost_products (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/);

    void kldb_aksrv_priv_update_server_hostid (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/, 
        const std::wstring& /*in*/    par_strId /*count = 256*/);

    void kldb_hst_clear_host (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_name /*count = 256*/);

    void kldb_hst_clear_hosts (
        KLDB::DbConnectionPtr    pCon);

    void kldb_hst_update_pubkey (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/, 
        KLSTD::MemoryChunkPtr /*in*/    par_pubKey /*count = 0*/, 
        const std::wstring& /*in*/    par_hash /*count = 32*/);

    KLDB::DbRecordsetPtr kldb_hst_acquire_hosts_to_update_statuses (
        KLDB::DbConnectionPtr    pCon, 
        bool /*in*/    par_bForce /*count = 0*/);

    void kldb_hst_update_hosts_statuses (
        KLDB::DbConnectionPtr    pCon);

    void kldb_hst_mark_hosts_for_update (
        KLDB::DbConnectionPtr    pCon);

    void kldb_hst_mark_hosts_for_update_for_host (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/);

    void kldb_hst_mark_hosts_for_update_for_group (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_hst_set_group_for_hosts (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nSessionId /*count = 0*/, 
        int /*in*/    par_nGroupId /*count = 0*/);

    void kldb_hst_rule_ex_on_clear_host (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/);

    void kldb_hst_rule_ex_on_clear_hosts (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_hst_rule_ex_process (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_rule_id /*count = 0*/, 
        bool /*in*/    par_bForce /*count = 0*/, 
        bool& /*out*/    par_bSrcEmpty /*count = 0*/);

    void kldb_hst_rule_ex_initial_fill (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/);

    void kldb_hst_rule_ex_set_order (
        KLDB::DbConnectionPtr    pCon);

    void kldb_hst_rule_ex_autoorder (
        KLDB::DbConnectionPtr    pCon);

    void kldb_hst_rule_ex_addnew (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_wstrPreDisplayName /*count = 256*/, 
        int /*in*/    par_nGroup /*count = 0*/, 
        int /*in*/    par_nRunOptions /*count = 0*/, 
        int /*in*/    par_nRuleType /*count = 0*/, 
        const std::wstring& /*in*/    par_txtQuery /*count = 0*/, 
        KLSTD::MemoryChunkPtr /*in*/    par_binCustomData /*count = 0*/, 
        int& /*out*/    par_rule_id /*count = 0*/);

    void kldb_hst_rule_ex_update_customdata (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_rule_id /*count = 0*/, 
        KLSTD::MemoryChunkPtr /*in*/    par_binCustomData /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_hst_publish_hosts_statuses (
        KLDB::DbConnectionPtr    pCon);

    void kldb_hst_update_host_times (
        KLDB::DbConnectionPtr    pCon);

    void kldb_grp_translate_group_name_proc (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_idGroup /*count = 0*/, 
        std::wstring& /*out*/    par_name /*count = 256*/);

    void kldb_grp_find_group (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_wstrName /*count = 256*/, 
        int /*in*/    par_nParentId /*count = 0*/, 
        int& /*out*/    par_nId /*count = 0*/);

    void kldb_grp_create_new_group (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_wstrName /*count = 256*/, 
        int /*in*/    par_nParentId /*count = 0*/, 
        int& /*out*/    par_nNewId /*count = 0*/);

    void kldb_au_add_ticked_patch (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_ext_id /*count = KLDBCONST_UT_UPD_BUNDLE_ID_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_tick_type /*count = 0*/, 
        int& /*out*/    par_patch_id /*count = 0*/, 
        int& /*out*/    par_existed /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_hstunav_get_hst_unavailable (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nTimeout /*count = 0*/);

    void kldb_hstunav_set_published_flag (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_lHost /*count = 0*/);

    void kldb_hstunav_is_host_unavailable (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_lHost /*count = 0*/, 
        int /*in*/    par_nTimeout /*count = 0*/, 
        bool& /*out*/    par_bResult /*count = 0*/);

    void kldb_hstunav_check_host_mac (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nIdHost /*count = 0*/, 
        const std::wstring& /*in*/    par_strMac /*count = 32*/, 
        bool& /*out*/    par_bHas /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_hstunav_get_host_mac (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nIdHost /*count = 0*/);

    void kldb_hstunav_add_host_mac (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nIdHost /*count = 0*/, 
        const std::wstring& /*in*/    par_strMac /*count = 32*/);

    void kldb_hstunav_remove_host_mac (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nIdHost /*count = 0*/, 
        const std::wstring& /*in*/    par_strMac /*count = 32*/);

    void kldb_au_check_task (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_patch_id /*count = 0*/, 
        int /*in*/    par_task_id /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_patch_date /*count = 0*/);

    void kldb_au_delete_task_record (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/);

    void kldb_au_delete_ticked_patch (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_ext_id /*count = KLDBCONST_UT_UPD_BUNDLE_ID_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_tick_type /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_au_get_tasks_for_tick (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_ext_id /*count = KLDBCONST_UT_UPD_BUNDLE_ID_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_tick_type /*count = 0*/);

    void kldb_au_set_general_tick (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_tick_type /*count = 0*/, 
        int /*in*/    par_value /*count = 0*/);

    void kldb_gp_add_item (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/, 
        int /*in*/    par_is_applied_to_subgroups /*count = 0*/, 
        int /*in*/    par_is_descendant_can_forbid /*count = 0*/, 
        int& /*out*/    par_item_id /*count = 0*/);

    void kldb_gp_delete_item (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_item_id /*count = 0*/);

    void kldb_gp_forbid_item (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_item_id /*count = 0*/, 
        int /*in*/    par_group_id /*count = 0*/);

    void kldb_gp_populate_groups_for_item (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_gp_item_id /*count = 0*/, 
        int /*in*/    par_group_id /*count = 0*/);

    void kldb_grp_delete_group (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_grp_get_group_hosts (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/);

    void kldb_grp_get_host_group (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_domain_name /*count = KLDBCONST_UT_DOMAIN_NAME_COUNT*/, 
        const std::wstring& /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/, 
        int& /*out*/    par_group_id /*count = 0*/);

    void kldb_grp_move_group_hosts (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_src_id /*count = 0*/, 
        int /*in*/    par_dst_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_grp_movetogroup_by_any (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/, 
        const std::wstring& /*in*/    par_somename /*count = 256*/);

    KLDB::DbRecordsetPtr kldb_grp_movetogroup_by_ip (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/, 
        AVP_longlong /*in*/    par_ip /*count = 0*/);

    void kldb_grp_populate_child_chain_session (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/, 
        int /*in*/    par_sublevel /*count = 0*/, 
        int /*in*/    par_nSessionId /*count = 0*/);

    void kldb_grp_populate_child_chain (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/, 
        int /*in*/    par_sublevel /*count = 0*/);

    void kldb_grp_get_groups_and_hosts_count (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/, 
        int& /*out*/    par_nSubGroups /*count = 0*/, 
        int& /*out*/    par_nSubHosts /*count = 0*/);

    void kldb_grp_populate_parent_chain (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/);

    void kldb_grp_zero_vcount_for_group (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/);

    void kldb_grp_zero_vcount_for_hosts (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nSessionId /*count = 0*/);

    void kldb_gsyn_get_subgroups (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/);

    void kldb_gsyn_get_need_recalc (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_gsyn_id /*count = 0*/, 
        bool& /*out*/    par_bNeedRecalc /*count = 0*/);

    void kldb_gsyn_put_need_recalc (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_gsyn_id /*count = 0*/, 
        bool /*in*/    par_bNeedRecalc /*count = 0*/);

    void kldb_gsyn_put_need_recalc_with_parents (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_gsyn_id /*count = 0*/);

    void kldb_gsyn_get_subgroups_for_gsyn (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/, 
        const std::wstring& /*in*/    par_wstrName /*count = 256*/);

    void kldb_gsyn_increment_version (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_gsyn_id /*count = 0*/, 
        AVP_longlong& /*out*/    par_version /*count = 0*/);

    void kldb_gsyn_retrieve_version (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_gsyn_id /*count = 0*/, 
        AVP_longlong& /*out*/    par_version /*count = 0*/);

    void kldb_hst_add_new_host (
        KLDB::DbConnectionPtr    pCon, 
        KLSTD::precise_time_t /*in*/    par_lastvisible /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_lastinfoupdate /*count = 0*/, 
        int /*in*/    par_group /*count = 0*/, 
        const std::wstring& /*in*/    par_windomain /*count = 256*/, 
        int /*in*/    par_windomaintype /*count = 0*/, 
        const std::wstring& /*in*/    par_winhost /*count = 256*/, 
        const std::wstring& /*in*/    par_dnsname /*count = 256*/, 
        const std::wstring& /*in*/    par_dnsdomain /*count = 256*/, 
        int /*in*/    par_status /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_lastupdate /*count = 0*/, 
        const std::wstring& /*in*/    par_pre_displayname /*count = 256*/, 
        const std::wstring& /*in*/    par_hostname /*count = 256*/, 
        int /*in*/    par_nComputerType /*count = 0*/, 
        int /*in*/    par_nPlatformType /*count = 0*/, 
        int /*in*/    par_nOsVersion /*count = 0*/, 
        AVP_longlong /*in*/    par_nIpAddress /*count = 0*/, 
        const std::wstring& /*in*/    par_presumed_group /*count = 256*/, 
        int& /*out*/    par_nIdHost /*count = 0*/);

    void kldb_hst_addproduct (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_product /*count = 32*/, 
        const std::wstring& /*in*/    par_version /*count = 32*/, 
        int& /*out*/    par_idResult /*count = 0*/);

    void kldb_hst_addproductdn (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_product /*count = 300*/, 
        int& /*out*/    par_idResult /*count = 0*/);

    void kldb_hst_before_multidelete (
        KLDB::DbConnectionPtr    pCon);

    void kldb_hst_after_move_multiple (
        KLDB::DbConnectionPtr    pCon);

    void kldb_hst_before_delete (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_hostname /*count = 256*/);

    void kldb_hst_after_move_byname (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_hostname /*count = 256*/);

    void kldb_hst_get_host_data (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_domain_name /*count = KLDBCONST_UT_DOMAIN_NAME_COUNT*/, 
        const std::wstring& /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/);

    void kldb_hst_get_winnames (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_hostname /*count = 256*/, 
        KLDB::DbValuePtr& /*out*/    par_winhostname /*count = 256*/, 
        KLDB::DbValuePtr& /*out*/    par_windomainname /*count = 256*/, 
        KLDB::DbValuePtr& /*out*/    par_windomaintype /*count = 0*/);

    void kldb_hst_increment_virus_count (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_hostid /*count = 256*/, 
        AVP_longlong /*in*/    par_delta /*count = 0*/);

    void kldb_hst_move_to_unassigned (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_hostname /*count = 256*/, 
        bool /*in*/    par_delete_if_unassigned /*count = 0*/, 
        bool /*in*/    par_clear_nagent /*count = 0*/);

    void kldb_hst_update_host_rtp_state (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/, 
        int /*in*/    par_state /*count = 0*/, 
        int /*in*/    par_ecode /*count = 0*/);

    void kldb_hst_set_last_fscan_time (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_hostid /*count = 256*/, 
        KLSTD::precise_time_t /*in*/    par_last /*count = 0*/);

    void kldb_hst_sethostinfo (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_hostname /*count = 256*/, 
        int /*in*/    par_id_domain /*count = 0*/, 
        const std::wstring& /*in*/    par_windomainname /*count = 256*/, 
        int /*in*/    par_id_dnsdomain /*count = 0*/, 
        const std::wstring& /*in*/    par_winhostname /*count = 256*/, 
        const std::wstring& /*in*/    par_dnshostname /*count = 256*/, 
        bool /*in*/    par_is_visible /*count = 0*/, 
        bool& /*out*/    par_bInserted /*count = 0*/, 
        bool& /*out*/    par_bGrpChgNtf /*count = 0*/, 
        std::wstring& /*out*/    par_wstrResGroup /*count = 256*/);

    void kldb_hst_sethostinfo_ex (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/, 
        int /*in*/    par_group_id /*count = 0*/, 
        const std::wstring& /*in*/    par_dns_name /*count = 256*/, 
        const std::wstring& /*in*/    par_dns_domain /*count = 256*/, 
        int /*in*/    par_ctype /*count = 0*/, 
        int /*in*/    par_ptype /*count = 0*/, 
        int /*in*/    par_osversion /*count = 0*/, 
        AVP_longlong /*in*/    par_ip /*count = 0*/, 
        int /*in*/    par_is_visible /*count = 0*/, 
        bool /*in*/    par_pub_inv /*count = 0*/, 
        bool /*in*/    par_has_info /*count = 0*/, 
        const std::wstring& /*in*/    par_strMac /*count = 32*/, 
        bool& /*out*/    par_bGrpChgNtf /*count = 0*/);

    void kldb_hst_make_host_invisible (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_hst_acquire_hosts_to_move2unassigned (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strLocalName /*count = 256*/);

    KLDB::DbRecordsetPtr kldb_hst_acquire_hosts_to_publish_invisible (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strLocalName /*count = 256*/);

    void kldb_hst_set_published_invisible (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/);

    void kldb_hst_update_hosts_visibility (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_timeout /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_licsrv_check_100_110 (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_licsrv_check_90 (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_licsrv_check_more_110 (
        KLDB::DbConnectionPtr    pCon);

    void kldb_loc_load_string_to_db (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_id /*count = 0*/, 
        const std::wstring& /*in*/    par_str /*count = 256*/);

    void kldb_nf_add_rule (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_nf_rule_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_event_type /*count = 50*/, 
        KLDB::DbValuePtr /*in*/    par_severity /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_day_count_to_store /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_put_to_event_log /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_smtp_server /*count = 100*/, 
        KLDB::DbValuePtr /*in*/    par_smtp_port /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_email /*count = 100*/, 
        KLDB::DbValuePtr /*in*/    par_use_email /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_net_send /*count = 1024*/, 
        KLDB::DbValuePtr /*in*/    par_use_net_send /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_script_target /*count = 500*/, 
        KLDB::DbValuePtr /*in*/    par_script_working_folder /*count = 300*/, 
        KLDB::DbValuePtr /*in*/    par_use_script /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_message_template /*count = 1024*/, 
        KLDB::DbValuePtr /*in*/    par_body_filter /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_email_subj /*count = 1024*/, 
        KLDB::DbValuePtr /*in*/    par_email_from /*count = 300*/, 
        KLDB::DbValuePtr /*in*/    par_email_smpt_user_name /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_email_smpt_user_pswd /*count = 256*/);

    void kldb_nf_delete_rules (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_policy_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_task_id /*count = KLDBCONST_UT_EXT_TASK_ID_COUNT*/);

    KLDB::DbRecordsetPtr kldb_nf_get_default_settings (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_nf_get_rules (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_policy_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_task_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_nf_get_specific_rule (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_nf_rule_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_event_type /*count = 50*/, 
        KLDB::DbValuePtr /*in*/    par_severity /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_nf_get_specific_rule_for_server (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_event_type /*count = 50*/, 
        KLDB::DbValuePtr /*in*/    par_severity /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_nf_get_specific_rule_for_task (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_task_id /*count = KLDBCONST_UT_EXT_TASK_ID_COUNT*/, 
        const std::wstring& /*in*/    par_event_type /*count = 50*/, 
        int /*in*/    par_severity /*count = 0*/);

    void kldb_nf_lookup_rule_id (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_hostname /*count = 256*/, 
        const std::wstring& /*in*/    par_product_name /*count = KLDBCONST_UT_PRODUCT_NAME_COUNT*/, 
        const std::wstring& /*in*/    par_product_version /*count = KLDBCONST_UT_PRODUCT_VERSION_COUNT*/, 
        int& /*out*/    par_nf_rule_id /*count = 0*/, 
        int& /*out*/    par_is_record_exists /*count = 0*/);

    void kldb_nf_prepare_common_header (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_policy_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_task_id /*count = KLDBCONST_UT_EXT_TASK_ID_COUNT*/, 
        int& /*out*/    par_nf_rule_id /*count = 0*/);

    void kldb_nf_prepare_host_product_header (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/, 
        const std::wstring& /*in*/    par_product_name /*count = KLDBCONST_UT_PRODUCT_NAME_COUNT*/, 
        const std::wstring& /*in*/    par_product_version /*count = KLDBCONST_UT_PRODUCT_VERSION_COUNT*/, 
        int& /*out*/    par_nf_rule_id /*count = 0*/);

    void kldb_nf_reset_host_all_products (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/);

    void kldb_nf_check_reset_host_all_products_for_group (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/);

    void kldb_nf_check_reset_host_all_products (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/);

    void kldb_nf_reset_host_product (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/, 
        const std::wstring& /*in*/    par_product_name /*count = KLDBCONST_UT_PRODUCT_NAME_COUNT*/, 
        const std::wstring& /*in*/    par_product_version /*count = KLDBCONST_UT_PRODUCT_VERSION_COUNT*/);

    void kldb_nf_reset_policy_hosts (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_policy_id /*count = 0*/);

    void kldb_nf_use_policy_info_for_host (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/, 
        const std::wstring& /*in*/    par_product_name /*count = KLDBCONST_UT_PRODUCT_NAME_COUNT*/, 
        const std::wstring& /*in*/    par_product_version /*count = KLDBCONST_UT_PRODUCT_VERSION_COUNT*/, 
        int /*in*/    par_policy_id /*count = 0*/, 
        int& /*out*/    par_nf_rule_id /*count = 0*/);

    void kldb_ping_delete_product_states_for_host (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_ping_get_prdstates (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/);

    void kldb_ping_delete_prdstate (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/, 
        int /*in*/    par_prod_id /*count = 0*/);

    void kldb_hst_update_prdstate_has_rtp (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/, 
        int /*in*/    par_prod_id /*count = 0*/);

    void kldb_ping_update_prdstate (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/, 
        int /*in*/    par_prod_id /*count = 0*/, 
        int /*in*/    par_nState /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_tmAvbasesDate /*count = 0*/, 
        int /*in*/    par_nAvbasesRecords /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_tmInstallDate /*count = 0*/, 
        const std::wstring& /*in*/    par_wstrProductDisplVersion /*count = 32*/, 
        int /*in*/    par_nProdDN /*count = 0*/);

    void kldb_ping_get_host_by_hostname (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_host_name /*count = 256*/);

    void kldb_ping_get_host_by_nagentid (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_nagent_id /*count = 64*/);

    void kldb_ping_get_host_by_winnames (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_winhostname /*count = 256*/, 
        const std::wstring& /*in*/    par_windomainname /*count = 256*/);

    void kldb_ping_get_host_stores (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/);

    void kldb_ping_check_nagentauthid (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_nagent /*count = 256*/, 
        const std::wstring& /*in*/    par_authid /*count = 256*/, 
        bool& /*out*/    par_bOK /*count = 0*/, 
        std::wstring& /*out*/    par_real_authid /*count = 256*/);

    void kldb_ping_check_product_install_time_for_host (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_hst_hostid /*count = 0*/, 
        const std::wstring& /*in*/    par_product /*count = 32*/, 
        const std::wstring& /*in*/    par_version /*count = 32*/, 
        KLSTD::precise_time_t /*in*/    par_installtime /*count = 0*/);

    void kldb_ping_insert_product_state_for_host (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/, 
        const std::wstring& /*in*/    par_product /*count = 32*/, 
        const std::wstring& /*in*/    par_version /*count = 32*/, 
        const std::wstring& /*in*/    par_version_displ /*count = 32*/, 
        int /*in*/    par_state /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_lasttime /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_basestime /*count = 0*/, 
        int /*in*/    par_basesrecords /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_installtime /*count = 0*/);

    void kldb_ping_post_gsync (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_gshost_id /*count = 0*/, 
        AVP_longlong /*in*/    par_ver /*count = 0*/, 
        bool /*in*/    par_success /*count = 0*/, 
        const std::wstring& /*in*/    par_descr /*count = 256*/, 
        int /*in*/    par_error_id /*count = 0*/, 
        bool& /*out*/    par_bSaved /*count = 0*/);

    void kldb_ping_pre_gsync (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/, 
        int /*in*/    par_gsyn_id /*count = 0*/, 
        int& /*out*/    par_gshost_id /*count = 0*/);

    void kldb_ping_remove_host_store (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/, 
        const std::wstring& /*in*/    par_storename /*count = 256*/);

    void kldb_ping_replace_host_store (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/, 
        const std::wstring& /*in*/    par_storename /*count = 256*/, 
        bool /*in*/    par_is_new /*count = 0*/, 
        bool /*in*/    par_is_changed /*count = 0*/, 
        const std::wstring& /*in*/    par_nagent /*count = 64*/, 
        const std::wstring& /*in*/    par_server /*count = 64*/);

    void kldb_ping_set_rtp_state (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/, 
        int /*in*/    par_status /*count = 0*/, 
        int /*in*/    par_errorcode /*count = 0*/);

    void kldb_ping_try_to_cleanup_removed_syncs (
        KLDB::DbConnectionPtr    pCon);

    void kldb_hst_clean_oldnames (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nSecPeriod /*count = 0*/);

    void kldb_pkg_add_package (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_pkg_name /*count = 256*/, 
        const std::wstring& /*in*/    par_pkg_guid /*count = 256*/, 
        const std::wstring& /*in*/    par_product_name /*count = 256*/, 
        const std::wstring& /*in*/    par_product_version /*count = 50*/, 
        const std::wstring& /*in*/    par_path /*count = 1024*/, 
        int /*in*/    par_size_low /*count = 0*/, 
        int /*in*/    par_size_high /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_creation_time /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_modification_time /*count = 0*/, 
        const std::wstring& /*in*/    par_settings_path /*count = 1024*/, 
        const std::wstring& /*in*/    par_product_displ_name /*count = 256*/, 
        const std::wstring& /*in*/    par_product_displ_version /*count = 50*/, 
        int& /*out*/    par_package_id /*count = 0*/);

    void kldb_pol_add_policy (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/, 
        const std::wstring& /*in*/    par_display_name /*count = KLDBCONST_UT_DISPLAY_NAME_COUNT*/, 
        const std::wstring& /*in*/    par_product_name /*count = KLDBCONST_UT_PRODUCT_NAME_COUNT*/, 
        const std::wstring& /*in*/    par_product_version /*count = KLDBCONST_UT_PRODUCT_VERSION_COUNT*/, 
        KLSTD::precise_time_t /*in*/    par_creation_time /*count = 0*/, 
        bool /*in*/    par_bAcceptParent /*count = 0*/, 
        bool /*in*/    par_bForceDistrib2Children /*count = 0*/, 
        int& /*out*/    par_policy_id /*count = 0*/);

    void kldb_pol_add_section_to_sync (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_policy_id /*count = 0*/, 
        const std::wstring& /*in*/    par_product_name /*count = KLDBCONST_UT_SECTION_NAME_COUNT*/, 
        const std::wstring& /*in*/    par_product_version /*count = KLDBCONST_UT_SECTION_NAME_COUNT*/, 
        const std::wstring& /*in*/    par_section_name /*count = KLDBCONST_UT_SECTION_NAME_COUNT*/);

    void kldb_pol_check_sub_policies (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_policy_id /*count = 0*/);

    void kldb_pol_delete_policy (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_policy_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_pol_get_changed_sections (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_policy_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_pol_get_child_active_policies (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_policy_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_pol_get_group_policies (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_pol_get_all_group_active_policies (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/, 
        bool /*in*/    par_bIncludeCurrent /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_pol_get_group_active_policies (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/);

    void kldb_pol_get_parent_policy (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_policy_id /*count = 0*/, 
        int& /*out*/    par_parent_policy_id /*count = 0*/, 
        int& /*out*/    par_parent_group_id /*count = 0*/);

    void kldb_pol_get_locking_parent_policy (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_policy_id /*count = 0*/, 
        int& /*out*/    par_parent_policy_id /*count = 0*/, 
        int& /*out*/    par_parent_group_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_pol_get_policies_to_sync (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_pol_get_policy (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_policy_id /*count = 0*/);

    void kldb_pol_get_policy_for_host_product (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_domain_name /*count = 256*/, 
        const std::wstring& /*in*/    par_hostname /*count = 256*/, 
        const std::wstring& /*in*/    par_product_name /*count = KLDBCONST_UT_PRODUCT_NAME_COUNT*/, 
        const std::wstring& /*in*/    par_product_version /*count = KLDBCONST_UT_PRODUCT_VERSION_COUNT*/, 
        int& /*out*/    par_policy_id /*count = 0*/);

    void kldb_pol_remove_policy_to_sync (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_parent_policy_id /*count = 0*/, 
        int /*in*/    par_sub_policy_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_pol_reset_child_policies_to_sync (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_policy_id /*count = 0*/);

    void kldb_pol_redistribute_all_policies (
        KLDB::DbConnectionPtr    pCon);

    void kldb_pol_set_policy_sync_id (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_policy_id /*count = 0*/, 
        int /*in*/    par_sync_id /*count = 0*/, 
        int /*in*/    par_type /*count = 0*/);

    void kldb_pol_update_policy (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_policy_id /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_modification_time /*count = 0*/, 
        const std::wstring& /*in*/    par_display_name /*count = KLDBCONST_UT_DISPLAY_NAME_COUNT*/);

    void kldb_rpt_add_field (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/, 
        const std::wstring& /*in*/    par_field_name /*count = KLDBCONST_UT_FIELD_NAME_COUNT*/, 
        int /*in*/    par_field_kind /*count = 0*/);

    void kldb_rpt_add_host (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/, 
        const std::wstring& /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/);

    void kldb_rpt_add_order (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/, 
        const std::wstring& /*in*/    par_field_name /*count = KLDBCONST_UT_FIELD_NAME_COUNT*/, 
        int /*in*/    par_field_kind /*count = 0*/, 
        int /*in*/    par_is_asc /*count = 0*/);

    void kldb_rpt_add_report (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_report_type /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_display_name /*count = KLDBCONST_UT_DISPLAY_NAME_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_start_time /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_end_time /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_creation_time /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_group_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_max_details_records /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_params /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_parFilter /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_strFilterHash /*count = 32*/, 
        KLDB::DbValuePtr /*in*/    par_nRecursionLevelLimit /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_nPeriodToUpdateCacheInMinutes /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_strCacheStructId /*count = 50*/, 
        int& /*out*/    par_report_id /*count = 0*/);

    void kldb_rpt_delete_report (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/, 
        const std::wstring& /*in*/    par_strCacheStructId /*count = 50*/);

    void kldb_rpt_delete_stuff (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/);

    void kldb_rpt_exec_avdb_ver (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/, 
        const std::wstring& /*in*/    par_condition /*count = 4000*/);

    void kldb_rpt_exec_avdb_ver_ttl (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_avdb_ver_slv (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_errors (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/, 
        const std::wstring& /*in*/    par_condition /*count = 4000*/, 
        KLSTD::precise_time_t /*in*/    par_start_time /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_end_time /*count = 0*/);

    void kldb_rpt_exec_errors_slv (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_errors_ttl (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_lic (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/, 
        const std::wstring& /*in*/    par_condition /*count = 4000*/);

    void kldb_rpt_exec_lic_slv (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_lic_ttl (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_most_infected (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/, 
        const std::wstring& /*in*/    par_condition /*count = 4000*/, 
        KLSTD::precise_time_t /*in*/    par_start_time /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_end_time /*count = 0*/);

    void kldb_rpt_exec_most_infected_slv (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_most_infected_ttl (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_protection (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/, 
        const std::wstring& /*in*/    par_condition /*count = 4000*/);

    void kldb_rpt_exec_protection_slv (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_protection_ttl (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_soft_ver (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/, 
        const std::wstring& /*in*/    par_condition /*count = 4000*/);

    void kldb_rpt_exec_soft_ver_ttl (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_soft_ver_slv (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_competitor_soft (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/, 
        const std::wstring& /*in*/    par_condition /*count = 4000*/);

    void kldb_rpt_exec_competitor_soft_ttl (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_competitor_soft_slv (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_inventory (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/, 
        const std::wstring& /*in*/    par_condition /*count = 4000*/);

    void kldb_rpt_exec_inventory_ttl (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_inventory_slv (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_test (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/, 
        const std::wstring& /*in*/    par_condition /*count = 4000*/, 
        KLSTD::precise_time_t /*in*/    par_start_time /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_end_time /*count = 0*/);

    void kldb_rpt_viract_clean (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_viract_inc_update_ix (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nEventUpdateFrom /*count = 0*/, 
        int& /*out*/    par_nPrevious /*count = 0*/);

    void kldb_rpt_exec_viract (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/, 
        const std::wstring& /*in*/    par_condition /*count = 4000*/, 
        KLSTD::precise_time_t /*in*/    par_start_time /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_end_time /*count = 0*/);

    void kldb_rpt_exec_viract_slv (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_viract_ttl (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_rpt_get_common_data (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_rpt_get_detailed_data (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_rpt_get_reports (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_update_report (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_report_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_display_name /*count = KLDBCONST_UT_DISPLAY_NAME_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_start_time /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_end_time /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_modification_time /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_group_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_max_details_records /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_params /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_parFilter /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_strFilterHash /*count = 32*/, 
        KLDB::DbValuePtr /*in*/    par_nRecursionLevelLimit /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_nPeriodToUpdateCacheInMinutes /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_strCacheStructId /*count = 50*/);

    void kldb_sp_ev_clean_old_events (
        KLDB::DbConnectionPtr    pCon, 
        KLSTD::precise_time_t /*in*/    par_curr_time /*count = 0*/, 
        int& /*out*/    par_rest_count /*count = 0*/);

    void kldb_sp_ev_delete_event (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_id /*count = 0*/);

    void kldb_sp_ev_put_event (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_severity /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_origin_is_policy /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_task_id /*count = KLDBCONST_UT_EXT_TASK_ID_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_task_display_name /*count = 100*/, 
        KLDB::DbValuePtr /*in*/    par_hostname /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_product_name /*count = KLDBCONST_UT_PRODUCT_NAME_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_product_version /*count = KLDBCONST_UT_PRODUCT_VERSION_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_product_displ_version /*count = KLDBCONST_UT_PRODUCT_VERSION_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_event_type /*count = 50*/, 
        KLDB::DbValuePtr /*in*/    par_event_type_display_name /*count = 100*/, 
        KLDB::DbValuePtr /*in*/    par_rise_time /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_rise_time_ms /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_registration_time /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_registration_time_ms /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_days_to_store /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_descr /*count = 1000*/, 
        KLDB::DbValuePtr /*in*/    par_body /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_loc_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_is_need_send /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_task_old_state /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_task_new_state /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_par1 /*count = KLDBCONST_UT_EVENT_PARAM_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_par2 /*count = KLDBCONST_UT_EVENT_PARAM_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_par3 /*count = KLDBCONST_UT_EVENT_PARAM_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_par4 /*count = KLDBCONST_UT_EVENT_PARAM_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_par5 /*count = KLDBCONST_UT_EVENT_PARAM_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_par6 /*count = KLDBCONST_UT_EVENT_PARAM_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_par7 /*count = KLDBCONST_UT_EVENT_PARAM_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_par8 /*count = KLDBCONST_UT_EVENT_PARAM_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_par9 /*count = KLDBCONST_UT_EVENT_PARAM_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_event_session_number /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_kca_loc_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_event_type_id /*count = 0*/, 
        int& /*out*/    par_event_id /*count = 0*/);

    void kldb_sp_ev_set_need_send_flag (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_event_id /*count = 0*/, 
        int /*in*/    par_is_need_send /*count = 0*/);

    void kldb_sp_ev_reset_need_send_flag_and_body (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_event_id /*count = 0*/);

    void kldb_sp_te_event_succeeded (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_event_id /*count = KLDBCONST_UT_EVP_EVENT_ID_COUNT*/, 
        KLSTD::precise_time_t /*in*/    par_record_time /*count = 0*/);

    void kldb_sp_te_give_event_up (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_event_id /*count = KLDBCONST_UT_EVP_EVENT_ID_COUNT*/, 
        const std::wstring& /*in*/    par_hostdomain /*count = 50*/, 
        const std::wstring& /*in*/    par_hostname /*count = 50*/, 
        const std::wstring& /*in*/    par_component_name /*count = 50*/, 
        const std::wstring& /*in*/    par_detailed_info /*count = 1000*/, 
        KLSTD::precise_time_t /*in*/    par_curr_time /*count = 0*/);

    void kldb_sp_te_is_known_event (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_event_id /*count = KLDBCONST_UT_EVP_EVENT_ID_COUNT*/, 
        int& /*out*/    par_is_known_event /*count = 0*/);

    void kldb_sp_te_postpone_event (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_event_id /*count = KLDBCONST_UT_EVP_EVENT_ID_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_attempt_count /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_product_name /*count = 50*/, 
        KLDB::DbValuePtr /*in*/    par_version /*count = 50*/, 
        KLDB::DbValuePtr /*in*/    par_component_name /*count = 50*/, 
        KLDB::DbValuePtr /*in*/    par_instance_id /*count = 50*/, 
        KLDB::DbValuePtr /*in*/    par_event_type /*count = 50*/, 
        KLDB::DbValuePtr /*in*/    par_body /*count = 6000*/, 
        KLDB::DbValuePtr /*in*/    par_event_time /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_subscription /*count = 50*/, 
        KLDB::DbValuePtr /*in*/    par_delivery_id /*count = 100*/, 
        KLDB::DbValuePtr /*in*/    par_hostdomain /*count = 50*/, 
        KLDB::DbValuePtr /*in*/    par_hostname /*count = 50*/, 
        KLDB::DbValuePtr /*in*/    par_agent_id /*count = 50*/, 
        KLDB::DbValuePtr /*in*/    par_detailed_info /*count = 1000*/, 
        KLDB::DbValuePtr /*in*/    par_curr_time /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_sp_te_read_events_to_process (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_minutes_threshold /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_curr_time /*count = 0*/);

    void kldb_tsk_add_folder_to_clean (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_task_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_host_id /*count = KLDBCONST_UT_HOST_NAME_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_path /*count = 300*/, 
        KLDB::DbValuePtr /*in*/    par_deadline /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_params /*count = 0*/, 
        int& /*out*/    par_path_id /*count = 0*/);

    void kldb_tsk_add_host_to_temp_list (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_list_id /*count = 0*/, 
        const std::wstring& /*in*/    par_hostname /*count = 256*/, 
        AVP_longlong /*in*/    par_ip /*count = 0*/);

    void kldb_tsk_add_task (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_product_name /*count = KLDBCONST_UT_PRODUCT_NAME_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_product_version /*count = KLDBCONST_UT_PRODUCT_VERSION_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_component_name /*count = KLDBCONST_UT_COMPONENT_NAME_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_task_file_identity /*count = 100*/, 
        KLDB::DbValuePtr /*in*/    par_task_name /*count = KLDBCONST_UT_TASK_NAME_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_task_display_name /*count = 100*/, 
        KLDB::DbValuePtr /*in*/    par_group_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_temp_host_list_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_must_be_unique /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_package_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_is_remote_install /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_start_condition /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_start_condition_data /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_start_condition_data_ex /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_run_missed /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_target_type /*count = 0*/, 
        int& /*out*/    par_task_id /*count = 0*/);

    void kldb_tsk_delete_task (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/);

    void kldb_tsk_delete_temp_host_list (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_list_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_all_tasks_for_host (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_catched_tasks_for_host (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_hosts_for_task (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_task_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_paths_in_progress (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_tsk_get_state_statistics (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        int /*in*/    par_bIgnoreSlaveServers /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_state_statistics_ex (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        int /*in*/    par_bIgnoreSlaveServers /*count = 0*/, 
        int& /*out*/    par_nCompletedPercent /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_state_statistics_ri (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        int /*in*/    par_bIgnoreSlaveServers /*count = 0*/, 
        int& /*out*/    par_nCompletedPercent /*count = 0*/, 
        int& /*out*/    par_nVirtualProgressActive /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_task (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_task_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_task_file_identity /*count = 100*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_task_host_data (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_sync_id /*count = 0*/, 
        int /*in*/    par_host_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_tasks (
        KLDB::DbConnectionPtr    pCon);

    void kldb_tsk_remove_folder_to_clean (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_path_id /*count = 0*/, 
        int /*in*/    par_clean_tsk_host_state /*count = 0*/);

    void kldb_tsk_set_host_progress (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_task_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_hostname /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_state_descr /*count = 1000*/, 
        KLDB::DbValuePtr /*in*/    par_completed_percent /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_event_time /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_event_time_ms /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_src_inst_id /*count = KLDBCONST_UT_SRC_INST_ID_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_src_event_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_db_event_id /*count = 0*/);

    void kldb_tsk_set_state_for_all_task_hosts (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        int /*in*/    par_state_code /*count = 0*/, 
        int /*in*/    par_fail_reason /*count = 0*/, 
        const std::wstring& /*in*/    par_state_descr /*count = 1000*/, 
        int /*in*/    par_state_code_to_exclude1 /*count = 0*/, 
        int /*in*/    par_state_code_to_exclude2 /*count = 0*/, 
        int /*in*/    par_state_code_to_exclude3 /*count = 0*/);

    void kldb_tsk_set_task_completed_cond (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_state_code /*count = 0*/, 
        int /*in*/    par_task_id /*count = 0*/, 
        const std::wstring& /*in*/    par_hostname /*count = 256*/);

    void kldb_tsk_set_state_for_host (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_state_code /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_task_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_hostname /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_path_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_deadline /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_change_deadline /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_try_count /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_state_descr /*count = 1000*/, 
        KLDB::DbValuePtr /*in*/    par_fail_reason /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_event_time /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_event_time_ms /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_src_inst_id /*count = KLDBCONST_UT_SRC_INST_ID_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_src_event_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_db_event_id /*count = 0*/);

    void kldb_tsk_set_state_for_host_ex (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_state_code /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_task_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_hostname /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_path_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_deadline /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_change_deadline /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_try_count_delta /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_state_descr /*count = 1000*/, 
        KLDB::DbValuePtr /*in*/    par_fail_reason /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_completed_percent /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_event_time /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_event_time_ms /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_src_inst_id /*count = KLDBCONST_UT_SRC_INST_ID_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_src_event_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_db_event_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_need_reboot /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_timed_finish_percent /*count = 0*/);

    void kldb_tsk_update_task (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_task_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_task_display_name /*count = 100*/, 
        KLDB::DbValuePtr /*in*/    par_new_file_identity /*count = 100*/, 
        KLDB::DbValuePtr /*in*/    par_remote_sync_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_temp_host_list_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_package_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_is_remote_install /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_start_condition /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_start_condition_data /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_start_condition_data_ex /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_run_missed /*count = 0*/);

    void kldb_hst_is_unassigned (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_host_name /*count = 256*/, 
        bool& /*out*/    par_is_unassigned /*count = 0*/);

    void kldb_gsf_priv_calc_gshosts_for_gsyn_g (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_gsyn_id /*count = 0*/);

    void kldb_gsf_gsyncs_clear_state (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_gsyn_id /*count = 0*/);

    void kldb_gsf_priv_update_gshosts_for_gsyn (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_gsyn_id /*count = 0*/);

    void kldb_gsf_priv_find_parent_gsyncs (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_gsyn_id /*count = 0*/);

    void kldb_gsf_update_gshosts_for_group (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/);

    void kldb_gsf_update_gshosts_for_gsyn (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_gsyn_id /*count = 0*/);

    void kldb_gsf_update_gshosts_for_host (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/);

    void kldb_gsyn_priv_addhost_for_gsupdate (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_gsyn_update_gshosts_for_explicit_gsyn (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_gsyn_id /*count = 0*/, 
        int /*in*/    par_action_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_gsf_find_hosts_to_synchronize (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_gsf_find_hosts_for_gsyn (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_gsyn_id /*count = 0*/, 
        bool /*in*/    par_bOnlyRequired /*count = 0*/, 
        bool /*in*/    par_bIncludeMobile /*count = 0*/);

    void kldb_gsf_find_hosts_for_gsyn_count (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_gsyn_id /*count = 0*/, 
        bool /*in*/    par_bOnlyRequired /*count = 0*/, 
        int& /*out*/    par_count /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_gsf_find_gsyn_for_host (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/, 
        bool /*in*/    par_bOnlyRequired /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_gsf_find_files_to_delete (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/);

    void kldb_gsf_acquire_con_data (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/, 
        std::wstring& /*out*/    par_inst_name /*count = 64*/, 
        std::wstring& /*out*/    par_address /*count = 256*/, 
        std::wstring& /*out*/    par_host_name /*count = 256*/, 
        bool& /*out*/    par_keep_conn /*count = 0*/, 
        bool& /*out*/    par_is_nagent /*count = 0*/);

    void kldb_ping_on_nagent_reinstalled (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/);

    void kldb_ping_post_run_new_integration (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/);

    void kldb_ping_resolve_host (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_ping_hostname /*count = 256*/, 
        const std::wstring& /*in*/    par_ping_nagentid /*count = 64*/, 
        const std::wstring& /*in*/    par_ping_winhostname /*count = 256*/, 
        const std::wstring& /*in*/    par_ping_windomain /*count = 256*/, 
        int /*in*/    par_ping_windomaintype /*count = 0*/, 
        const std::wstring& /*in*/    par_ping_presumed_dn /*count = 256*/, 
        const std::wstring& /*in*/    par_ping_presumed_group /*count = 256*/, 
        const std::wstring& /*in*/    par_ping_dnsname /*count = 256*/, 
        const std::wstring& /*in*/    par_ping_dnsdomain /*count = 256*/, 
        const std::wstring& /*in*/    par_ping_address /*count = 256*/, 
        KLSTD::precise_time_t /*in*/    par_ping_lastupdate /*count = 0*/, 
        AVP_longlong /*in*/    par_ping_connect_ip /*count = 0*/, 
        const std::wstring& /*in*/    par_presumed_hostname /*count = 256*/, 
        AVP_longlong /*in*/    par_ip_nagent /*count = 0*/, 
        bool /*in*/    par_bUseMacAddresses /*count = 0*/, 
        bool /*in*/    par_bHasRtp /*count = 0*/, 
        int& /*out*/    par_hst_hostid /*count = 0*/, 
        std::wstring& /*out*/    par_hst_hostname /*count = 256*/, 
        std::wstring& /*out*/    par_hst_nagentid /*count = 64*/, 
        std::wstring& /*out*/    par_hst_winhostname /*count = 256*/, 
        std::wstring& /*out*/    par_hst_windomain /*count = 256*/, 
        int& /*out*/    par_hst_ntdomain /*count = 0*/, 
        int& /*out*/    par_hst_windomaintype /*count = 0*/, 
        bool& /*out*/    par_hst_keepcon /*count = 0*/, 
        bool& /*out*/    par_hst_was_inserted /*count = 0*/, 
        bool& /*out*/    par_hst_must_change_name /*count = 0*/, 
        bool& /*out*/    par_hst_must_run_new_conf /*count = 0*/, 
        bool& /*out*/    par_hst_conflict1 /*count = 0*/, 
        std::wstring& /*out*/    par_ping_hstkeyhash /*count = 32*/, 
        bool& /*out*/    par_bNeedMacAddr /*count = 0*/, 
        bool& /*out*/    par_bGrpChgNtf /*count = 0*/);

    void kldb_hst_find_host_id (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_hostname /*count = 256*/, 
        KLDB::DbValuePtr& /*out*/    par_host_id /*count = 0*/);

    void kldb_hst_find_host_by_dn (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_wstrDisplayName /*count = KLDBCONST_UT_HOST_DISPLAY_NAME_COUNT*/, 
        KLDB::DbValuePtr& /*out*/    par_host_id /*count = 0*/);

    void kldb_tsk_on_gshosts_insert_delete (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_gsyn_id /*count = 0*/);

    void kldb_tsk_on_gshosts_state_reset (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_gsyn_id /*count = 0*/);

    void kldb_tsk_on_gshosts_insert_delete_for_hostid (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/);

    void kldb_tsk_generate_events_for_all_task_hosts (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        int /*in*/    par_severity /*count = 0*/, 
        const std::wstring& /*in*/    par_event_type /*count = 50*/, 
        const std::wstring& /*in*/    par_descr /*count = 1000*/, 
        int /*in*/    par_state_code /*count = 0*/, 
        int /*in*/    par_state_code_old /*count = 0*/, 
        int /*in*/    par_state_code_to_exclude1 /*count = 0*/, 
        int /*in*/    par_state_code_to_exclude2 /*count = 0*/, 
        int /*in*/    par_state_code_to_exclude3 /*count = 0*/, 
        const std::wstring& /*in*/    par_product_name /*count = KLDBCONST_UT_PRODUCT_NAME_COUNT*/, 
        const std::wstring& /*in*/    par_product_version /*count = KLDBCONST_UT_PRODUCT_VERSION_COUNT*/, 
        const std::wstring& /*in*/    par_product_displ_version /*count = KLDBCONST_UT_PRODUCT_VERSION_COUNT*/, 
        int /*in*/    par_is_for_hosts /*count = 0*/, 
        int /*in*/    par_is_for_slave_servers /*count = 0*/, 
        int /*in*/    par_days_to_store /*count = 0*/, 
        int /*in*/    par_need_send /*count = 0*/);

    void kldb_tsk_get_id_by_file_identity (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_task_file_identity /*count = 100*/, 
        KLDB::DbValuePtr& /*out*/    par_task_id /*count = 0*/);

    void kldb_ads_find_org_unit (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strObjectGUD /*count = 40*/, 
        int& /*out*/    par_nId /*count = 0*/, 
        bool& /*out*/    par_bEnabledAdScan /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_ads_find_org_unit_by_dn_hash (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strDistNameHash /*count = 32*/);

    KLDB::DbRecordsetPtr kldb_ads_get_computers (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nIdUnit /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_ads_get_subunits (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nIdUnit /*count = 0*/);

    void kldb_ads_add_org_unit (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_parent_id /*count = 0*/, 
        const std::wstring& /*in*/    par_wstrName /*count = 512*/, 
        const std::wstring& /*in*/    par_strObjectGUD /*count = 40*/, 
        const std::wstring& /*in*/    par_wstrDistName /*count = 0*/, 
        const std::wstring& /*in*/    par_strDistNameHash /*count = 32*/);

    void kldb_ads_remove_org_unit (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strObjectGUD /*count = 40*/);

    void kldb_ads_update_org_unit (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_wstrName /*count = 512*/, 
        const std::wstring& /*in*/    par_strObjectGUD /*count = 40*/, 
        const std::wstring& /*in*/    par_wstrDistName /*count = 0*/, 
        const std::wstring& /*in*/    par_strDistNameHash /*count = 32*/);

    void kldb_ads_add_computer (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_parent_id /*count = 0*/, 
        const std::wstring& /*in*/    par_wstrName /*count = 512*/, 
        const std::wstring& /*in*/    par_strObjectGUD /*count = 40*/, 
        int /*in*/    par_nNtDomain /*count = 0*/);

    void kldb_ads_remove_computer (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strObjectGUD /*count = 40*/);

    void kldb_ads_update_computer (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_wstrName /*count = 512*/, 
        const std::wstring& /*in*/    par_strObjectGUD /*count = 40*/, 
        int /*in*/    par_nNtDomain /*count = 0*/);

    void kldb_grp_normalize_unassigned (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_srvhrch_get_child_servers (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nGroupId /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_gsyn_check_slaves (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_srvhrch_get_child_servers_rec (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nGroupId /*count = 0*/, 
        int /*in*/    par_nSessionId /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_srvhrch_get_child_server_group_id (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nServerId /*count = 0*/);

    void kldb_srvhrch_register_server (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_server_display_name /*count = KLDBCONST_UT_HOST_DISPLAY_NAME_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_group_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_server_certificate /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_server_public_key_hash /*count = 32*/, 
        KLDB::DbValuePtr /*in*/    par_server_address /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_server_params /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_server_version /*count = 32*/, 
        KLDB::DbValuePtr /*in*/    par_server_instance_id /*count = 64*/, 
        KLDB::DbValuePtr /*in*/    par_host_name /*count = KLDBCONST_UT_HOST_NAME_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_strHierarchyCacheVersion /*count = 50*/, 
        int& /*out*/    par_server_id /*count = 0*/);

    void kldb_srvhrch_del_server (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_server_id /*count = 0*/, 
        const std::wstring& /*in*/    par_strHierarchyCacheVersion /*count = 50*/, 
        std::wstring& /*out*/    par_server_instance_id /*count = 64*/);

    void kldb_srvhrch_update_server (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_server_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_server_display_name /*count = KLDBCONST_UT_DISPLAY_NAME_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_group_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_server_certificate /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_server_public_key_hash /*count = 32*/, 
        KLDB::DbValuePtr /*in*/    par_server_address /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_server_params /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_server_version /*count = 32*/, 
        KLDB::DbValuePtr /*in*/    par_server_instance_id /*count = 64*/, 
        KLDB::DbValuePtr /*in*/    par_server_status /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_strHierarchyCacheVersion /*count = 50*/);

    void kldb_srvhrch_check_child_server_instance_id (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_server_public_key_hash /*count = 32*/, 
        const std::wstring& /*in*/    par_server_version /*count = 32*/, 
        const std::wstring& /*in*/    par_server_instance_id /*count = 64*/, 
        int /*in*/    par_server_num_version /*count = 0*/, 
        const std::wstring& /*in*/    par_strHierarchyCacheVersion /*count = 50*/, 
        int& /*out*/    par_server_id /*count = 0*/, 
        int& /*out*/    par_host_id /*count = 0*/);

    void kldb_licsrv_delete_key (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strSerial /*count = 32*/, 
        bool& /*out*/    par_bNotPermitted /*count = 0*/);

    void kldb_gsyn_get_host_pubkey (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/, 
        KLSTD::MemoryChunkPtr& /*out*/    par_binPublicKey /*count = 4096*/);

    void kldb_tsk_on_host_move (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_hostname /*count = 256*/);

    void kldb_tsk_on_hosts_move (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_src_group_id /*count = 0*/, 
        int /*in*/    par_group_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_glbl_task_hosts (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        int /*in*/    par_include_servers /*count = 0*/);

    void kldb_dbtest_proc_1 (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nValI /*count = 0*/, 
        int& /*out*/    par_nValO /*count = 0*/, 
        KLSTD::MemoryChunkPtr /*in*/    par_imValI /*count = 0*/, 
        const std::wstring& /*in*/    par_txtVal /*count = 0*/, 
        KLSTD::MemoryChunkPtr /*in*/    par_binValI /*count = 1024*/, 
        KLSTD::MemoryChunkPtr& /*out*/    par_binValO /*count = 1024*/);

    KLDB::DbRecordsetPtr kldb_dbtest_proc_2 (
        KLDB::DbConnectionPtr    pCon);

    void kldb_dbtest_proc_3 (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_nX /*count = 0*/, 
        KLDB::DbValuePtr& /*out*/    par_nY /*count = 0*/);

    void kldb_grp_get_group_name (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nGroupId /*count = 0*/, 
        std::wstring& /*out*/    par_wstrName /*count = 256*/);

    void kldb_evp_get_max_event_id (
        KLDB::DbConnectionPtr    pCon, 
        int& /*out*/    par_id /*count = 0*/);

    void kldb_test_hst_set_nag_props (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/, 
        const std::wstring& /*in*/    par_strAddress /*count = 256*/, 
        const std::wstring& /*in*/    par_strConnection /*count = 256*/);

    void kldb_hst_locate_host_ext_attr (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/);

    void kldb_hst_update_host_comment (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/, 
        const std::wstring& /*in*/    par_wstrComment /*count = 256*/);

    void kldb_hst_update_nagent_install_id (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/, 
        const std::wstring& /*in*/    par_wstrNagentInstallId /*count = 50*/);

    void kldb_gsyn_change_path (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/, 
        const std::wstring& /*in*/    par_wstrNewPath /*count = 512*/);

    void kldb_pol_try_switch_to_active (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nIdPolicy /*count = 0*/, 
        const std::wstring& /*in*/    par_wstrPolicyPath /*count = 512*/, 
        int& /*out*/    par_gsyn_id /*count = 0*/, 
        bool& /*out*/    par_bResult /*count = 0*/, 
        int& /*out*/    par_policy_existing /*count = 0*/, 
        bool& /*out*/    par_notperm /*count = 0*/);

    void kldb_pol_try_switch_to_roaming (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nIdPolicy /*count = 0*/, 
        const std::wstring& /*in*/    par_wstrPolicyPath /*count = 512*/, 
        int& /*out*/    par_gsyn_id /*count = 0*/, 
        bool& /*out*/    par_bResult /*count = 0*/, 
        int& /*out*/    par_policy_existing /*count = 0*/, 
        bool& /*out*/    par_notperm /*count = 0*/);

    void kldb_pol_make_inactive (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nIdPolicy /*count = 0*/, 
        int& /*out*/    par_gsyn_id /*count = 0*/, 
        bool& /*out*/    par_bResult /*count = 0*/);

    void kldb_pol_clean_inactive_from_changed_sections (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nIdPolicy /*count = 0*/);

    void kldb_pol_clean_inactive_from_pol_sub_policy_to_sync (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nIdPolicy /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_grp_get_parent_chain (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/, 
        bool& /*out*/    par_bResult /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_grp_get_parent_chain_nag (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_nagent_id /*count = 64*/, 
        bool& /*out*/    par_bResult /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_grp_get_parent_chain_hst (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strName /*count = 64*/, 
        bool& /*out*/    par_bResult /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_pol_get_outbreak_policy (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nTypes /*count = 0*/);

    void kldb_rpt_get_stat_network (
        KLDB::DbConnectionPtr    pCon, 
        KLSTD::precise_time_t /*in*/    par_tNetScanBegin /*count = 0*/, 
        int& /*out*/    par_nUnassignedHostsCnt /*count = 0*/, 
        int& /*out*/    par_nNewFoundHostsCnt /*count = 0*/);

    void kldb_rpt_get_stat_infected_obj_count (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nSessionId /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_tNetScanBegin /*count = 0*/, 
        int& /*out*/    par_nInfectedObjectsCnt /*count = 0*/);

    void kldb_rpt_get_stat_hst_status (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nSessionId /*count = 0*/, 
        int& /*out*/    par_nGroupsHostsCnt /*count = 0*/, 
        int& /*out*/    par_nCrtHostsCnt /*count = 0*/, 
        int& /*out*/    par_nWrnHostsCnt /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_rpt_get_stat_groups_conn_hist (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nSessionId /*count = 0*/);

    void kldb_rpt_get_stat_client_updates (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nSessionId /*count = 0*/, 
        int /*in*/    par_nCurrentAVSRecordsCount /*count = 0*/, 
        int& /*out*/    par_nActualCnt /*count = 0*/, 
        int& /*out*/    par_nDayCnt /*count = 0*/, 
        int& /*out*/    par_n3DayCnt /*count = 0*/, 
        int& /*out*/    par_nWeekCnt /*count = 0*/, 
        int& /*out*/    par_nOldCnt /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_rpt_get_stat_full_scan_histogram (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nSessionId /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_rpt_get_stat_rtp_states_histogram (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nSessionId /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_rpt_get_stat_viruses_histogram (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nSessionId /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_tmBegin /*count = 0*/);

    void kldb_hst_update_tsk_states (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nHostId /*count = 0*/, 
        bool /*in*/    par_bFullSend /*count = 0*/);

    void kldb_hst_set_tsk_states (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nHostId /*count = 0*/, 
        const std::wstring& /*in*/    par_product /*count = 32*/, 
        const std::wstring& /*in*/    par_version /*count = 32*/, 
        int /*in*/    par_nState /*count = 0*/, 
        const std::wstring& /*in*/    par_strTask /*count = 64*/);

    void kldb_hst_use_keep_conn (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nIdHost /*count = 0*/);

    void kldb_hst_limit_keep_conn (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nMax /*count = 0*/);

    void kldb_hst_set_keep_conn (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nIdHost /*count = 0*/, 
        int /*in*/    par_nMax /*count = 0*/, 
        bool /*in*/    par_bKeep /*count = 0*/);

    void kldb_nsa_get_diapason_dn (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/, 
        std::wstring& /*out*/    par_wstrDisplayName /*count = KLDBCONST_UT_DISPLAY_NAME_COUNT*/);

    KLDB::DbRecordsetPtr kldb_nsa_get_intervals (
        KLDB::DbConnectionPtr    pCon, 
        AVP_longlong& /*out*/    par_nFullRange /*count = 0*/);

    void kldb_nsa_update_diapason_intervals (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/);

    void kldb_nsa_add_diapason (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_wstrDisplayName /*count = KLDBCONST_UT_DISPLAY_NAME_COUNT*/, 
        int /*in*/    par_nLifeTime /*count = 0*/, 
        bool /*in*/    par_bEnabled /*count = 0*/, 
        int& /*out*/    par_nId /*count = 0*/);

    void kldb_nsa_remove_diapason (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/);

    void kldb_nsa_update_diapason (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/, 
        const std::wstring& /*in*/    par_wstrDisplayName /*count = KLDBCONST_UT_DISPLAY_NAME_COUNT*/, 
        int /*in*/    par_nLifeTime /*count = 0*/, 
        bool /*in*/    par_bEnabled /*count = 0*/, 
        bool /*in*/    par_bUseEnabled /*count = 0*/);

    void kldb_nsa_add_host (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strHostName /*count = 256*/, 
        const std::wstring& /*in*/    par_strMac /*count = 32*/, 
        bool /*in*/    par_bResolved /*count = 0*/, 
        const std::wstring& /*in*/    par_strDN /*count = 256*/, 
        const std::wstring& /*in*/    par_strDnsName /*count = 256*/, 
        const std::wstring& /*in*/    par_strDnsDomain /*count = 256*/, 
        const std::wstring& /*in*/    par_strAnyName /*count = 256*/, 
        AVP_longlong /*in*/    par_ip /*count = 0*/, 
        int /*in*/    par_nDiapason /*count = 0*/, 
        AVP_longlong /*in*/    par_nIpHost /*count = 0*/, 
        bool& /*out*/    par_bConflict /*count = 0*/, 
        bool /*in*/    par_bHasWinInfo /*count = 0*/, 
        const std::wstring& /*in*/    par_strWinHostName /*count = 256*/, 
        const std::wstring& /*in*/    par_strWinDomain /*count = 256*/, 
        int /*in*/    par_ctype /*count = 0*/, 
        int /*in*/    par_ptype /*count = 0*/, 
        int /*in*/    par_osversion /*count = 0*/, 
        bool& /*out*/    par_bAdded /*count = 0*/, 
        bool& /*out*/    par_bGrpChgNtf /*count = 0*/, 
        int& /*out*/    par_nHostId /*count = 0*/, 
        std::wstring& /*out*/    par_wstrResGroup /*count = 256*/);

    void kldb_nsa_clear_hosts (
        KLDB::DbConnectionPtr    pCon);

    void kldb_nl_list_add (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strListName /*count = 256*/, 
        const std::wstring& /*in*/    par_strComponentName /*count = 256*/);

    void kldb_nl_list_delete (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strListName /*count = 256*/);

    void kldb_nl_items_add_item (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_strId /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_strListName /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_strComponentName /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_tmCreation /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_params /*count = 0*/);

    void kldb_nl_items_add_q_item (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_strId /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_strListName /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_strComponentName /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_tmCreation /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_params /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_ObjName /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_szFileTitle /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_iObjStatus /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_tmStoreTime /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_VirusName /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_iObjSize /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_szRestorePath /*count = 1024*/, 
        KLDB::DbValuePtr /*in*/    par_szUser /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_szDescription /*count = 1024*/, 
        KLDB::DbValuePtr /*in*/    par_tmLastCheckBaseDate /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_tmSentToKLDate /*count = 0*/);

    void kldb_nl_items_update_item (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_strId /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_strListName /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_tmCreation /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_params /*count = 0*/);

    void kldb_nl_items_update_q_item (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_strId /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/, 
        KLDB::DbValuePtr /*in*/    par_strListName /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_tmCreation /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_params /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_ObjName /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_szFileTitle /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_iObjStatus /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_tmStoreTime /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_VirusName /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_iObjSize /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_szRestorePath /*count = 1024*/, 
        KLDB::DbValuePtr /*in*/    par_szUser /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_szDescription /*count = 1024*/, 
        KLDB::DbValuePtr /*in*/    par_tmLastCheckBaseDate /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_tmSentToKLDate /*count = 0*/);

    void kldb_nl_items_get_item_str_id_and_list_name (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_item_id /*count = 0*/, 
        std::wstring& /*out*/    par_strId /*count = 256*/, 
        std::wstring& /*out*/    par_listName /*count = 256*/, 
        std::wstring& /*out*/    par_hostName /*count = 256*/);

    void kldb_nl_items_delete_item (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strId /*count = 256*/, 
        const std::wstring& /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/, 
        const std::wstring& /*in*/    par_strListName /*count = 256*/);

    void kldb_nl_items_delete_q_item (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strId /*count = 256*/, 
        const std::wstring& /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/, 
        const std::wstring& /*in*/    par_strListName /*count = 256*/);

    void kldb_nl_items_get_max_id (
        KLDB::DbConnectionPtr    pCon, 
        int& /*out*/    par_id /*count = 0*/);

    void kldb_nl_tasks_add_item (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_item_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_taskName /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_taskStatus /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_params /*count = 0*/, 
        int& /*out*/    par_nHostId /*count = 0*/, 
        std::wstring& /*out*/    par_list_name /*count = 256*/);

    void kldb_nl_tasks_add_item_for_host (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_item_id /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_taskName /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_taskStatus /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_params /*count = 0*/, 
        int& /*out*/    par_nHostId /*count = 0*/, 
        std::wstring& /*out*/    par_list_name /*count = 256*/);

    void kldb_nl_tasks_delete_item (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strId /*count = 256*/, 
        const std::wstring& /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/, 
        int /*in*/    par_checkStatus /*count = 0*/, 
        int& /*out*/    par_nHostIdNotHasTask /*count = 0*/);

    void kldb_nl_delete_list_items (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_ngSyncId /*count = 0*/, 
        const std::wstring& /*in*/    par_strListName /*count = 256*/);

    void kldb_nl_delete_list_item_for_host (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_hostId /*count = 0*/, 
        const std::wstring& /*in*/    par_strListName /*count = 256*/);

    void kldb_nl_delete_list_item_for_multi_hosts_delete (
        KLDB::DbConnectionPtr    pCon);

    void kldb_nl_delete_list_item_for_multi_hosts_clear (
        KLDB::DbConnectionPtr    pCon);

    void kldb_nl_check_host_product_states (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_idHost /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_hst_get_host_mac_by_name (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strHostId /*count = 256*/, 
        int& /*out*/    par_nCount /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_hst_get_host_mac_by_id (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nHostId /*count = 0*/, 
        int& /*out*/    par_nCount /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_nl_tasks_get_tasks (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nHostId /*count = 0*/);

    void kldb_nl_clear_list_for_hosts_without_products (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nHostId /*count = 0*/);

    void kldb_ping_add_tmp_mac (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strMac /*count = 32*/);

    void kldb_hst_get_host_dn (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/, 
        std::wstring& /*out*/    par_wstrName /*count = 256*/);

    void kldb_grp_get_hst_group_name (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nHostId /*count = 0*/, 
        std::wstring& /*out*/    par_wstrName /*count = 256*/);

    void kldb_hst_get_group_id (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nHostId /*count = 0*/, 
        int& /*out*/    par_nGroup /*count = 0*/);

    void kldb_grp_get_parent_id (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/, 
        int& /*out*/    par_nParent /*count = 0*/);

    void kldb_loc_localize_groups (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_grp_get_parent_names_chain_desc (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/, 
        bool /*in*/    par_bIncludeSuper /*count = 0*/, 
        bool& /*out*/    par_bResult /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_adhst_get_parent_names_chain_desc (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_unit_id /*count = 0*/, 
        bool& /*out*/    par_bResult /*count = 0*/);

    void kldb_pol_get_policy_name (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_id /*count = 0*/, 
        std::wstring& /*out*/    par_name /*count = KLDBCONST_UT_DISPLAY_NAME_COUNT*/);

    void kldb_ping_udp_find_host_key (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strHostName /*count = 256*/, 
        KLSTD::MemoryChunkPtr& /*out*/    par_binPublicKey /*count = 4096*/, 
        int& /*out*/    par_nHostId /*count = 0*/);

    void kldb_grp_update_grp_status_opt (
        KLDB::DbConnectionPtr    pCon, 
        bool /*in*/    par_bIsPolicy /*count = 0*/, 
        int /*in*/    par_nType /*count = 0*/, 
        int /*in*/    par_nMask /*count = 0*/, 
        int /*in*/    par_nPar3 /*count = 0*/, 
        int /*in*/    par_nPar4 /*count = 0*/, 
        int /*in*/    par_nPar6 /*count = 0*/, 
        int /*in*/    par_nPar7 /*count = 0*/, 
        int /*in*/    par_nPar8 /*count = 0*/, 
        int /*in*/    par_nPar9 /*count = 0*/, 
        int /*in*/    par_nPar10 /*count = 0*/);

    void kldb_grp_if_locked_statusopt_c (
        KLDB::DbConnectionPtr    pCon, 
        bool& /*out*/    par_bLocked /*count = 0*/);

    void kldb_grp_if_locked_statusopt_w (
        KLDB::DbConnectionPtr    pCon, 
        bool& /*out*/    par_bLocked /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_grp_get_status_opt (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nGroup /*count = 0*/, 
        int /*in*/    par_nType /*count = 0*/);

    void kldb_grp_clear_unused_status (
        KLDB::DbConnectionPtr    pCon);

    void kldb_grp_set_status_opt_inherited (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nGroup /*count = 0*/, 
        int /*in*/    par_nType /*count = 0*/, 
        bool /*in*/    par_bChildrenOnly /*count = 0*/);

    void kldb_grp_set_status_opt (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nGroup /*count = 0*/, 
        int /*in*/    par_nType /*count = 0*/, 
        int /*in*/    par_nMask /*count = 0*/, 
        int /*in*/    par_nPar3 /*count = 0*/, 
        int /*in*/    par_nPar4 /*count = 0*/, 
        int /*in*/    par_nPar6 /*count = 0*/, 
        int /*in*/    par_nPar7 /*count = 0*/, 
        int /*in*/    par_nPar8 /*count = 0*/, 
        int /*in*/    par_nPar9 /*count = 0*/, 
        int /*in*/    par_nPar10 /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_hst_get_hosts_for_fscan (
        KLDB::DbConnectionPtr    pCon);

    void kldb_tsk_resolve_host_id (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_strNetworkName /*count = 256*/, 
        KLDB::DbValuePtr /*in*/    par_nIp /*count = 0*/, 
        std::wstring& /*out*/    par_strHostId /*count = 256*/);

    void kldb_tsk_get_host_data (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nTaskId /*count = 0*/, 
        const std::wstring& /*in*/    par_strHostId /*count = 256*/, 
        int /*in*/    par_bUseFQDNByDefault /*count = 0*/, 
        std::wstring& /*out*/    par_strNetworkName /*count = 256*/, 
        AVP_longlong& /*out*/    par_nIp /*count = 0*/);

    void kldb_tsk_update_glb_ri_hosts (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nTaskId /*count = 0*/);

    void kldb_tsk_get_current_host_id (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strHostId /*count = 256*/, 
        std::wstring& /*out*/    par_strHostIdOut /*count = 256*/);

    void kldb_tsk_set_ri_host_start (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        const std::wstring& /*in*/    par_hostname /*count = 256*/, 
        int /*in*/    par_start_phase /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_ri_host_for_task_start (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        int /*in*/    par_get_optimal /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_ri_host_for_task_start_for_tsk_completed (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        int /*in*/    par_condition_task_id /*count = 0*/, 
        int /*in*/    par_get_optimal /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_ri_host_for_task_start_state (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        const std::wstring& /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/, 
        int /*in*/    par_condition_task_id /*count = 0*/);

    void kldb_tsk_clean_old_glued_hosts (
        KLDB::DbConnectionPtr    pCon, 
        KLSTD::precise_time_t /*in*/    par_tLimitDate /*count = 0*/);

    void kldb_tsk_clean_old_ri_hosts_starts (
        KLDB::DbConnectionPtr    pCon, 
        KLSTD::precise_time_t /*in*/    par_tLimitDate /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_srvhrch_get_server_groups_chain (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_server_instance_id /*count = 64*/);

    void kldb_tsk_check_glued_hosts (
        KLDB::DbConnectionPtr    pCon, 
        KLSTD::precise_time_t /*in*/    par_tGlueTime /*count = 0*/);

    void kldb_hst_acquire_connection (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strName /*count = 256*/, 
        std::wstring& /*out*/    par_strConnectionName /*count = 64*/, 
        std::wstring& /*out*/    par_strAddress /*count = 256*/);

    void kldb_rpt_exec_net_attacks_priv (
        KLDB::DbConnectionPtr    pCon, 
        KLSTD::precise_time_t /*in*/    par_start_time /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_end_time /*count = 0*/);

    void kldb_rpt_exec_net_attacks (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/, 
        const std::wstring& /*in*/    par_condition /*count = 4000*/, 
        KLSTD::precise_time_t /*in*/    par_start_time /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_end_time /*count = 0*/);

    void kldb_rpt_exec_net_attacks_slv (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_net_attacks_ttl (
        KLDB::DbConnectionPtr    pCon);

    void kldb_hst_update_computer_info (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nHostId /*count = 0*/, 
        int /*in*/    par_nCtype /*count = 0*/, 
        int /*in*/    par_nPType /*count = 0*/, 
        int /*in*/    par_nOsVersion /*count = 0*/);

    void kldb_ak_acquire_last_inserted (
        KLDB::DbConnectionPtr    pCon, 
        int& /*out*/    par_nId /*count = 0*/);

    void kldb_gsyn_acquire_count_for_status (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nGsynId /*count = 0*/, 
        int /*in*/    par_nStatus /*count = 0*/, 
        int& /*out*/    par_nCount /*count = 0*/);

    void kldb_fts_uagents_update (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nAgentHostId /*count = 0*/, 
        int /*in*/    par_nGroupId /*count = 0*/, 
        const std::wstring& /*in*/    par_strAddr /*count = 256*/, 
        const std::wstring& /*in*/    par_strNonSSLAddress /*count = 256*/, 
        const std::wstring& /*in*/    par_strMulticastAddr /*count = 256*/, 
        KLSTD::MemoryChunkPtr /*in*/    par_imCert /*count = 0*/);

    void kldb_fts_uagents_mark_for_delete (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nAgentHostId /*count = 0*/);

    void kldb_fts_uagents_mark_for_delete_multi (
        KLDB::DbConnectionPtr    pCon);

    void kldb_fts_uagents_mark_for_delete_clear (
        KLDB::DbConnectionPtr    pCon);

    void kldb_fts_uagents_delete (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nAgentHostId /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_fts_uagents_get (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nAgentHostId /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_fts_uagents_get_for_group (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nGroupId /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_fts_uagents_get_notregistred_hosts (
        KLDB::DbConnectionPtr    pCon);

    void kldb_fts_uagents_host_registred (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nAgentHostId /*count = 0*/);

    void kldb_fts_file_add (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strFileId /*count = 256*/, 
        const std::wstring& /*in*/    par_strFileName /*count = 256*/, 
        const std::wstring& /*in*/    par_strDescription /*count = 256*/, 
        int /*in*/    par_nGroupId /*count = 0*/, 
        int /*in*/    par_iStatus /*count = 0*/, 
        int /*in*/    par_iFullSize /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_tmCreation /*count = 0*/, 
        int /*in*/    par_bIsDirectory /*count = 0*/, 
        int /*in*/    par_fileType /*count = 0*/, 
        KLSTD::MemoryChunkPtr /*in*/    par_imgParContext /*count = 0*/, 
        int /*in*/    par_iMaxUploadingClients /*count = 0*/, 
        int& /*out*/    par_nId /*count = 0*/);

    void kldb_fts_file_delete (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strFileId /*count = 256*/);

    void kldb_fts_filehosts_add (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strFileId /*count = 256*/, 
        int /*in*/    par_nHostId /*count = 0*/, 
        const std::wstring& /*in*/    par_strConnName /*count = 256*/, 
        int /*in*/    par_onlyForUA /*count = 0*/, 
        int& /*out*/    par_nId /*count = 0*/, 
        std::wstring& /*out*/    par_strHostName /*count = 256*/, 
        std::wstring& /*out*/    par_strHostDnsName /*count = 256*/, 
        std::wstring& /*out*/    par_strHostConnName /*count = 256*/);

    void kldb_fts_filegroups_add (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strFileId /*count = 256*/, 
        int /*in*/    par_nGroupId /*count = 0*/, 
        int /*in*/    par_excludeFlag /*count = 0*/);

    void kldb_fts_filegroups_clear (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strFileId /*count = 256*/);

    void kldb_fts_file_update_status (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strFileId /*count = 256*/, 
        int /*in*/    par_iStatus /*count = 0*/);

    void kldb_fts_file_update_context (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strFileId /*count = 256*/, 
        KLSTD::MemoryChunkPtr /*in*/    par_imgParContext /*count = 0*/);

    void kldb_fts_filehosts_delete (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strFileId /*count = 256*/, 
        int /*in*/    par_nHostId /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_fts_files_get (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_fts_file_get_hosts (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strFileId /*count = 256*/);

    KLDB::DbRecordsetPtr kldb_fts_file_get_groups (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strFileId /*count = 256*/);

    KLDB::DbRecordsetPtr kldb_fts_file_get_ua_hosts (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strFileId /*count = 256*/);

    KLDB::DbRecordsetPtr kldb_fts_file_get_ua_hosts_for_host (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strFileId /*count = 256*/, 
        int /*in*/    par_nHostId /*count = 0*/);

    void kldb_fts_filehosts_update_status (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strFileId /*count = 256*/, 
        int /*in*/    par_nHostId /*count = 0*/, 
        int /*in*/    par_iStatus /*count = 0*/, 
        int /*in*/    par_forUA /*count = 0*/);

    void kldb_fts_file_find_for_host (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nHostId /*count = 0*/, 
        std::wstring& /*out*/    par_strFileId /*count = 256*/, 
        int& /*out*/    par_actionFlag /*count = 0*/);

    void kldb_fts_file_make_updateagents_list (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strFileId /*count = 256*/);

    void kldb_fts_file_groups_make_updateagents_list (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strFileId /*count = 256*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_ri_hosts_locations (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        const std::wstring& /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/, 
        int /*in*/    par_limit_try_count /*count = 0*/, 
        int /*in*/    par_skip_running /*count = 0*/, 
        const std::wstring& /*in*/    par_product_name /*count = KLDBCONST_UT_PRODUCT_NAME_COUNT*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_all_tasks_for_group (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_pkg_get_package_data (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_package_id /*count = 0*/);

    void kldb_tsk_get_ri_last_action (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        std::wstring& /*out*/    par_action_id /*count = 256*/);

    void kldb_tsk_set_ri_last_action (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        const std::wstring& /*in*/    par_action_id /*count = 256*/);

    KLDB::DbRecordsetPtr kldb_pkg_get_package_data_by_guid (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strPackageGUID /*count = 256*/);

    void kldb_hst_get_nagent_version (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/, 
        std::wstring& /*out*/    par_strVersion /*count = 32*/);

    void kldb_hst_get_slave_version (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/, 
        int& /*out*/    par_nVersion /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_slaves_locations (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/);

    void kldb_hst_product_component (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_wstrComponentName /*count = KLDBCONST_UT_COMPONENT_NAME_COUNT*/, 
        const std::wstring& /*in*/    par_wstrComponentVersion /*count = 32*/, 
        int& /*out*/    par_idResult /*count = 0*/);

    void kldb_ping_update_host_product_component (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nHostId /*count = 0*/, 
        int /*in*/    par_nProductId /*count = 0*/, 
        int /*in*/    par_nProdComponentId /*count = 0*/);

    void kldb_ping_delete_host_product_component (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nHostId /*count = 0*/, 
        int /*in*/    par_nProductId /*count = 0*/, 
        int /*in*/    par_nProdComponentId /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_ping_hst_get_product_components (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nHostId /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_grp_get_all_hosts_to_sync (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/);

    void kldb_tsk_initialize_state_for_all_task_hosts (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        int /*in*/    par_state_code /*count = 0*/, 
        const std::wstring& /*in*/    par_state_descr /*count = 1000*/, 
        int /*in*/    par_is_for_slave_servers /*count = 0*/);

    void kldb_evp_add_TransEventsSuceededSubscrs (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_event_id /*count = KLDBCONST_UT_EVP_EVENT_ID_COUNT*/, 
        const std::wstring& /*in*/    par_subscr_id /*count = 50*/);

    void kldb_evp_delete_TransEventsSuceededSubscrs (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_subscr_id /*count = 50*/);

    KLDB::DbRecordsetPtr kldb_evp_get_TransEventsSuceededSubscrs (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_event_id /*count = KLDBCONST_UT_EVP_EVENT_ID_COUNT*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_tasks_for_package (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nPackageId /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_ri_tasks_with_null_package (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_tsk_ri_expired_tasks (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_grp_acquire_changed_groups (
        KLDB::DbConnectionPtr    pCon);

    void kldb_grp_set_changed_groups (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nGroup1 /*count = 0*/, 
        int /*in*/    par_nGroup2 /*count = 0*/, 
        int /*in*/    par_nMask /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_hstgrp_remove_hosts (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nSessionId /*count = 0*/, 
        const std::wstring& /*in*/    par_strLocalHost /*count = 256*/);

    KLDB::DbRecordsetPtr kldb_hst_get_host_notification_info (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strName /*count = 256*/);

    void kldb_hst_set_host_visible (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strHostName /*count = 256*/);

    KLDB::DbRecordsetPtr kldb_grp_get_group_tree_objects (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nSessionId /*count = 0*/, 
        int /*in*/    par_group_id /*count = 0*/, 
        bool& /*out*/    par_bMayRemove /*count = 0*/);

    void kldb_tsk_mark_tsk_folder_to_clean_unused (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_clear_time /*count = 0*/);

    void kldb_tsk_upgrdfix_get_gsyn_path (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_gsyn_id /*count = 0*/, 
        std::wstring& /*out*/    par_wstrPath /*count = 512*/);

    void kldb_tsk_upgrdfix_set_gsyn_path (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_gsyn_id /*count = 0*/, 
        const std::wstring& /*in*/    par_wstrPath /*count = 512*/);

    void kldb_hst_set_syncstate (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/, 
        bool /*in*/    par_bSyncState /*count = 0*/);

    void kldb_hst_get_syncstate (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/, 
        bool& /*out*/    par_bSyncState /*count = 0*/);

    void kldb_rpt_exec_avp_prod_fs (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/, 
        const std::wstring& /*in*/    par_condition /*count = 4000*/, 
        const std::wstring& /*in*/    par_condition2 /*count = 4000*/, 
        KLSTD::precise_time_t /*in*/    par_start_time /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_end_time /*count = 0*/);

    void kldb_rpt_exec_avp_prod_fs_slv (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_avp_prod_fs_ttl (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_avp_prod_eml (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/, 
        const std::wstring& /*in*/    par_condition /*count = 4000*/, 
        const std::wstring& /*in*/    par_condition2 /*count = 4000*/, 
        KLSTD::precise_time_t /*in*/    par_start_time /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_end_time /*count = 0*/);

    void kldb_rpt_exec_avp_prod_eml_slv (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_avp_prod_eml_ttl (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_avp_prod_ph (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/, 
        const std::wstring& /*in*/    par_condition /*count = 4000*/, 
        const std::wstring& /*in*/    par_condition2 /*count = 4000*/, 
        KLSTD::precise_time_t /*in*/    par_start_time /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_end_time /*count = 0*/);

    void kldb_rpt_exec_avp_prod_ph_slv (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_avp_prod_ph_ttl (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_avp_prod (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/, 
        const std::wstring& /*in*/    par_condition /*count = 4000*/, 
        const std::wstring& /*in*/    par_condition2 /*count = 4000*/, 
        KLSTD::precise_time_t /*in*/    par_start_time /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_end_time /*count = 0*/);

    void kldb_rpt_exec_avp_prod_slv (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_avp_prod_ttl (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_tsk_get_tasks_for_condition (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_start_condition /*count = 0*/);

    void kldb_ak_add_deferred_action (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nModuleCode /*count = 0*/, 
        int /*in*/    par_nExecOrder /*count = 0*/, 
        int /*in*/    par_nActionCode /*count = 0*/, 
        int /*in*/    par_nPar1 /*count = 0*/, 
        int /*in*/    par_nPar2 /*count = 0*/, 
        const std::wstring& /*in*/    par_wstrPar1 /*count = 256*/, 
        const std::wstring& /*in*/    par_wstrPar2 /*count = 256*/, 
        int& /*out*/    par_nId /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_ak_acquire_deferred_actions (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nExecOrder /*count = 0*/, 
        int /*in*/    par_nModuleCode /*count = 0*/);

    void kldb_ak_remove_deferred_action (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_group_tasks (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/, 
        int /*in*/    par_include_supergroups /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_sp_ev_get_min_id (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nEventsToKeep /*count = 0*/);

    void kldb_sp_ev_clean_events (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nMinId /*count = 0*/);

    void kldb_srvhrch_get_server_instance_by_hostid (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_server_host_id /*count = KLDBCONST_UT_HOST_NAME_COUNT*/, 
        std::wstring& /*out*/    par_server_instance_id /*count = 64*/);

    void kldb_srvhrch_get_slave_version (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/, 
        int& /*out*/    par_nVersion /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_prod_inst_info (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_host /*count = KLDBCONST_UT_HOST_NAME_COUNT*/, 
        const std::wstring& /*in*/    par_product /*count = KLDBCONST_UT_PRODUCT_NAME_COUNT*/);

    void kldb_hst_get_mobile_device (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strDeviceId /*count = 64*/, 
        bool /*in*/    par_bUpdateVisibility /*count = 0*/, 
        int& /*out*/    par_nHostId /*count = 0*/, 
        std::wstring& /*out*/    par_strHostId /*count = 256*/, 
        bool& /*out*/    par_bGrpChgNtf /*count = 0*/);

    void kldb_hst_create_mobile_device (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strDeviceId /*count = 64*/, 
        const std::wstring& /*in*/    par_pre_displayname /*count = 256*/, 
        const std::wstring& /*in*/    par_presumed_group /*count = 256*/, 
        const std::wstring& /*in*/    par_strHostId /*count = 256*/, 
        int /*in*/    par_ptype /*count = 0*/, 
        int /*in*/    par_ctype /*count = 0*/, 
        int /*in*/    par_osver /*count = 0*/, 
        int& /*out*/    par_nHostId /*count = 0*/, 
        bool& /*out*/    par_bGrpChgNtf /*count = 0*/);

    void kldb_hst_get_active_policy_gsyn_for_host (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nHostId /*count = 0*/, 
        const std::wstring& /*in*/    par_product /*count = 32*/, 
        const std::wstring& /*in*/    par_version /*count = 32*/, 
        int& /*out*/    par_nGsynId /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_gsyn_get_sync_fields (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/);

    void kldb_gsyn_after_delete_gshost (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/);

    void kldb_tsk_update_tsk_task_on_upgrade (
        KLDB::DbConnectionPtr    pCon);

    void kldb_tsk_set_task_start_id (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        const std::wstring& /*in*/    par_start_id /*count = 100*/);

    void kldb_tsk_get_task_start_id (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        std::wstring& /*out*/    par_start_id /*count = 100*/);

    void kldb_tsk_reset_rised_start_conditions (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        const std::wstring& /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/);

    void kldb_tsk_set_visibility_as_rised_start_condition (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        const std::wstring& /*in*/    par_hostname /*count = KLDBCONST_UT_HOST_NAME_COUNT*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_missed_tasks (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_tsk_get_conditional_tasks (
        KLDB::DbConnectionPtr    pCon);

    void kldb_srvh_set_products_info (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nServerHostId /*count = 0*/);

    void kldb_srvh_set_components_info (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nServerHostId /*count = 0*/);

    void kldb_hst_acquire_hostinfo (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/, 
        int& /*out*/    par_nRecordType /*count = 0*/, 
        std::wstring& /*out*/    par_wstrHostId /*count = 256*/, 
        bool& /*out*/    par_bKeepConnection /*count = 0*/, 
        std::wstring& /*out*/    par_strConnectionName /*count = 64*/);

    void kldb_hst_mobile_increment_vircount (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/);

    void kldb_tsk_mark_task_deleted (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_deleted_tasks (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_tsk_get_pending_master_gsyncs (
        KLDB::DbConnectionPtr    pCon);

    void kldb_aud_get_localization_product_name (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_wstrProductName /*count = 512*/, 
        std::wstring& /*out*/    par_wstrLocProductName /*count = 512*/);

    void kldb_tsk_set_pending_master_gsync (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_task_file_identity /*count = 100*/, 
        const std::wstring& /*in*/    par_ss_type /*count = 768*/);

    void kldb_tsk_reset_pending_master_gsync (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_task_file_identity /*count = 100*/);

    void kldb_tsk_set_slaves_ (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_task_file_identity /*count = 100*/);

    KLDB::DbRecordsetPtr kldb_get_ak_patches (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_get_application_ak_patches (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_wstrAppName /*count = 256*/);

    void kldb_tsk_mark_tsk_folder_to_clean_frozen (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/);

    void kldb_tsk_should_process_sync_events (
        KLDB::DbConnectionPtr    pCon, 
        int& /*out*/    par_should_process /*count = 0*/);

    void kldb_tsk_process_sync_events (
        KLDB::DbConnectionPtr    pCon, 
        int& /*out*/    par_should_repeat /*count = 0*/);

    void kldb_pkg_set_products_info (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nPkgId /*count = 0*/);

    void kldb_pkg_set_components_info (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nPkgId /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_upd_get_installed_products_and_components (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_include_slave_servers /*count = 0*/, 
        int /*in*/    par_include_packages /*count = 0*/);

    void kldb_rpt_exec_most_infecting_usr (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/, 
        const std::wstring& /*in*/    par_condition /*count = 4000*/, 
        KLSTD::precise_time_t /*in*/    par_start_time /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_end_time /*count = 0*/);

    void kldb_rpt_exec_most_infecting_usr_slv (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_exec_most_infecting_usr_ttl (
        KLDB::DbConnectionPtr    pCon);

    void kldb_hst_prodfix_addnew (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nHostId /*count = 0*/, 
        int /*in*/    par_nProductId /*count = 0*/, 
        const std::wstring& /*in*/    par_wstrDN /*count = 256*/, 
        const std::wstring& /*in*/    par_strID /*count = 64*/, 
        int /*in*/    par_nOrder /*count = 0*/);

    void kldb_hst_prodfix_update (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nHostId /*count = 0*/, 
        int /*in*/    par_nProductId /*count = 0*/, 
        const std::wstring& /*in*/    par_wstrDN /*count = 256*/, 
        const std::wstring& /*in*/    par_strID /*count = 64*/, 
        int /*in*/    par_nOrder /*count = 0*/);

    void kldb_hst_prodfix_delete (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nHostId /*count = 0*/, 
        int /*in*/    par_nProductId /*count = 0*/, 
        const std::wstring& /*in*/    par_strID /*count = 64*/);

    KLDB::DbRecordsetPtr kldb_hst_prodfix_select (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nHostId /*count = 0*/);

    void kldb_trf_restrictions_replace (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/, 
        int /*in*/    par_nTotalLimit /*count = 0*/, 
        int /*in*/    par_nTimedLimit /*count = 0*/, 
        int /*in*/    par_nTimedStartHour /*count = 0*/, 
        int /*in*/    par_nTimedStartMin /*count = 0*/, 
        int /*in*/    par_nTimedEndHour /*count = 0*/, 
        int /*in*/    par_nTimedEndMin /*count = 0*/, 
        int /*in*/    par_nMaskOrLo /*count = 0*/, 
        int /*in*/    par_nSubnetOrHi /*count = 0*/, 
        bool /*in*/    par_bIsSubnet /*count = 0*/, 
        int& /*out*/    par_nOutId /*count = 0*/);

    void kldb_trf_restrictions_delete (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_trf_restrictions_get (
        KLDB::DbConnectionPtr    pCon);

    void kldb_trf_get_host_group_by_conn_name (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strHostConnName /*count = 25*/, 
        int& /*out*/    par_nGroupId /*count = 0*/);

    void kldb_ak_on_server_start (
        KLDB::DbConnectionPtr    pCon);

    void kldb_srvhrch_set_cache_version (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strSrvInstanceId /*count = 50*/, 
        const std::wstring& /*in*/    par_strCacheVersion /*count = 50*/);

    void kldb_srvhrch_get_cache_version (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strSrvInstanceId /*count = 50*/, 
        std::wstring& /*out*/    par_strCacheVersion /*count = 50*/);

    void kldb_srvhrch_set_hrch_version (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strHrchVersion /*count = 50*/);

    void kldb_srvhrch_get_hrch_version (
        KLDB::DbConnectionPtr    pCon, 
        std::wstring& /*out*/    par_strHrchVersion /*count = 50*/);

    void kldb_srvhrch_populate_cache_child_chain (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strInstanceId /*count = 50*/, 
        int /*in*/    par_sublevel /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_srvhrch_get_cache_for_group (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nGroupId /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_srvhrch_get_cache_subtree (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strInstanceId /*count = 50*/);

    void kldb_srvhrch_get_cache_dn_path (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strLeafInstanceId /*count = 50*/, 
        const std::wstring& /*in*/    par_strRootInstanceId /*count = 50*/, 
        const std::wstring& /*in*/    par_wstrFelimeter /*count = 10*/, 
        std::wstring& /*out*/    par_wstrPath /*count = 4000*/);

    void kldb_srvhrch_clear_cache (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strInstanceId /*count = 50*/);

    KLDB::DbRecordsetPtr kldb_srvhrch_get_cache (
        KLDB::DbConnectionPtr    pCon);

    void kldb_srvhrch_clear_cache_entry (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strInstanceId /*count = 50*/);

    void kldb_srvhrch_add_cache_entry (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strInstanceId /*count = 50*/, 
        const std::wstring& /*in*/    par_strParentSrvInstanceId /*count = 50*/, 
        const std::wstring& /*in*/    par_wstrDisplayName /*count = KLDBCONST_UT_DISPLAY_NAME_COUNT*/);

    void kldb_srvhrch_update_cache_entry (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strInstanceId /*count = 50*/, 
        const std::wstring& /*in*/    par_strParentSrvInstanceId /*count = 50*/, 
        const std::wstring& /*in*/    par_wstrDisplayName /*count = KLDBCONST_UT_DISPLAY_NAME_COUNT*/);

    void kldb_rpt_get_report_result_from_cache_id (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strServerInstanceId /*count = 50*/, 
        int /*in*/    par_nReportType /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_tmDesiredStartTime /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_tmDesiredEndTime /*count = 0*/, 
        const std::wstring& /*in*/    par_strFilterHash /*count = 32*/, 
        int /*in*/    par_bCheckTimeInterval /*count = 0*/, 
        int& /*out*/    par_nId /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_rpt_get_report_result_from_cache (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strServerInstanceId /*count = 50*/, 
        int /*in*/    par_nReportType /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_tmDesiredStartTime /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_tmDesiredEndTime /*count = 0*/, 
        const std::wstring& /*in*/    par_strFilterHash /*count = 32*/, 
        int /*in*/    par_bCheckTimeInterval /*count = 0*/);

    void kldb_rpt_add_report_result_to_cache (
        KLDB::DbConnectionPtr    pCon, 
        KLDB::DbValuePtr /*in*/    par_strServerInstanceId /*count = 50*/, 
        KLDB::DbValuePtr /*in*/    par_nReportType /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_tmDesiredStartTime /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_tmDesiredEndTime /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_strFilterHash /*count = 32*/, 
        KLDB::DbValuePtr /*in*/    par_tmStartTime /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_tmEndTime /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_tmGenerationTime /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_parDeliveryParams /*count = 0*/, 
        KLDB::DbValuePtr /*in*/    par_parReportResult /*count = 0*/);

    void kldb_rpt_get_report_to_cache_id (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nReportType /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_tmDesiredStartTime /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_tmDesiredEndTime /*count = 0*/, 
        KLSTD::MemoryChunkPtr /*in*/    par_parFilter /*count = 0*/, 
        const std::wstring& /*in*/    par_strFilterHash /*count = 32*/, 
        int /*in*/    par_nRecursionLevelLimit /*count = 0*/, 
        int /*in*/    par_nPeriodToUpdateCacheInMinutes /*count = 0*/, 
        const std::wstring& /*in*/    par_strCacheStructId /*count = 50*/, 
        int& /*out*/    par_nId /*count = 0*/);

    void kldb_rpt_set_cache_version (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strRptCacheVersion /*count = 50*/);

    void kldb_rpt_get_cache_version (
        KLDB::DbConnectionPtr    pCon, 
        std::wstring& /*out*/    par_strRptCacheVersion /*count = 50*/);

    void kldb_rpt_set_master_cache_version (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strRptMasterCacheVersion /*count = 50*/);

    void kldb_rpt_get_master_cache_version (
        KLDB::DbConnectionPtr    pCon, 
        std::wstring& /*out*/    par_strRptMasterCacheVersion /*count = 50*/);

    KLDB::DbRecordsetPtr kldb_rpt_get_cache_struct (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_bIncludeOwn /*count = 0*/);

    void kldb_srvhrch_on_remove_master (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_add_report_to_cache (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strId /*count = 50*/, 
        int /*in*/    par_nReportType /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_tmDesiredStartTime /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_tmDesiredEndTime /*count = 0*/, 
        KLSTD::MemoryChunkPtr /*in*/    par_parFilter /*count = 0*/, 
        const std::wstring& /*in*/    par_strFilterHash /*count = 32*/, 
        int /*in*/    par_nRecursionLevelLimit /*count = 0*/, 
        int /*in*/    par_nPeriodToUpdateCacheInMinutes /*count = 0*/, 
        int& /*out*/    par_nId /*count = 0*/);

    void kldb_rpt_delete_report_to_cache (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strId /*count = 50*/);

    KLDB::DbRecordsetPtr kldb_rpt_get_report_to_cache (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_rpt_get_report_cache_to_send (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_set_report_cache_sent (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/);

    void kldb_tsk_on_hosts_move_multiple (
        KLDB::DbConnectionPtr    pCon);

    void kldb_nf_reset_host_all_products_multiple (
        KLDB::DbConnectionPtr    pCon);

    void kldb_hst_update_inventory_product_to_srv (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_str_id /*count = 64*/, 
        bool /*in*/    par_is_msi /*count = 0*/, 
        const std::wstring& /*in*/    par_display_name /*count = 256*/, 
        const std::wstring& /*in*/    par_display_version /*count = 256*/, 
        const std::wstring& /*in*/    par_publisher /*count = 256*/, 
        int /*in*/    par_lang_id /*count = 0*/, 
        const std::wstring& /*in*/    par_comments /*count = 256*/, 
        const std::wstring& /*in*/    par_help_link /*count = 256*/, 
        const std::wstring& /*in*/    par_help_phone /*count = 256*/, 
        const std::wstring& /*in*/    par_arp_regkey /*count = 256*/, 
        const std::wstring& /*in*/    par_competitor_product /*count = 256*/, 
        int& /*out*/    par_idResult /*count = 0*/);

    void kldb_hst_add_inventory_product (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_lHost /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_install_date /*count = 0*/, 
        const std::wstring& /*in*/    par_package_code /*count = 64*/, 
        const std::wstring& /*in*/    par_install_dir /*count = 256*/, 
        const std::wstring& /*in*/    par_str_id /*count = 64*/, 
        bool /*in*/    par_is_msi /*count = 0*/, 
        const std::wstring& /*in*/    par_display_name /*count = 256*/, 
        const std::wstring& /*in*/    par_display_version /*count = 256*/, 
        const std::wstring& /*in*/    par_publisher /*count = 256*/, 
        int /*in*/    par_lang_id /*count = 0*/, 
        const std::wstring& /*in*/    par_comments /*count = 256*/, 
        const std::wstring& /*in*/    par_help_link /*count = 256*/, 
        const std::wstring& /*in*/    par_help_phone /*count = 256*/, 
        const std::wstring& /*in*/    par_arp_regkey /*count = 256*/, 
        const std::wstring& /*in*/    par_competitor_product /*count = 256*/);

    void kldb_hst_update_inventory_product (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_lHost /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_install_date /*count = 0*/, 
        const std::wstring& /*in*/    par_package_code /*count = 64*/, 
        const std::wstring& /*in*/    par_install_dir /*count = 256*/, 
        const std::wstring& /*in*/    par_str_id /*count = 64*/, 
        bool /*in*/    par_is_msi /*count = 0*/, 
        const std::wstring& /*in*/    par_display_name /*count = 256*/, 
        const std::wstring& /*in*/    par_display_version /*count = 256*/, 
        const std::wstring& /*in*/    par_publisher /*count = 256*/, 
        int /*in*/    par_lang_id /*count = 0*/, 
        const std::wstring& /*in*/    par_comments /*count = 256*/, 
        const std::wstring& /*in*/    par_help_link /*count = 256*/, 
        const std::wstring& /*in*/    par_help_phone /*count = 256*/, 
        const std::wstring& /*in*/    par_arp_regkey /*count = 256*/, 
        const std::wstring& /*in*/    par_competitor_product /*count = 256*/);

    void kldb_hst_delete_inventory_product (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_lHost /*count = 0*/, 
        const std::wstring& /*in*/    par_str_id /*count = 64*/);

    void kldb_hst_delete_all_inventory_products (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_lHost /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_hst_get_inventory_products (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_get_inventory_products (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_get_inventory_patches (
        KLDB::DbConnectionPtr    pCon);

    void kldb_hst_update_inventory_patch_to_srv (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_str_id /*count = 64*/, 
        bool /*in*/    par_is_msi /*count = 0*/, 
        int /*in*/    par_productId /*count = 0*/, 
        const std::wstring& /*in*/    par_display_name /*count = 256*/, 
        const std::wstring& /*in*/    par_display_version /*count = 256*/, 
        const std::wstring& /*in*/    par_publisher /*count = 256*/, 
        const std::wstring& /*in*/    par_comments /*count = 256*/, 
        const std::wstring& /*in*/    par_help_link /*count = 256*/, 
        const std::wstring& /*in*/    par_help_phone /*count = 256*/, 
        const std::wstring& /*in*/    par_classification /*count = 256*/, 
        const std::wstring& /*in*/    par_more_info_url /*count = 256*/, 
        int& /*out*/    par_idResult /*count = 0*/);

    void kldb_hst_add_inventory_pacth (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_lHost /*count = 0*/, 
        const std::wstring& /*in*/    par_str_id /*count = 64*/, 
        bool /*in*/    par_is_msi /*count = 0*/, 
        const std::wstring& /*in*/    par_parent_id /*count = 64*/, 
        const std::wstring& /*in*/    par_display_name /*count = 256*/, 
        const std::wstring& /*in*/    par_display_version /*count = 256*/, 
        const std::wstring& /*in*/    par_publisher /*count = 256*/, 
        const std::wstring& /*in*/    par_comments /*count = 256*/, 
        const std::wstring& /*in*/    par_help_link /*count = 256*/, 
        const std::wstring& /*in*/    par_help_phone /*count = 256*/, 
        const std::wstring& /*in*/    par_classification /*count = 256*/, 
        const std::wstring& /*in*/    par_more_info_url /*count = 256*/);

    void kldb_hst_update_inventory_patch (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_lHost /*count = 0*/, 
        const std::wstring& /*in*/    par_str_id /*count = 64*/, 
        bool /*in*/    par_is_msi /*count = 0*/, 
        const std::wstring& /*in*/    par_parent_id /*count = 64*/, 
        const std::wstring& /*in*/    par_display_name /*count = 256*/, 
        const std::wstring& /*in*/    par_display_version /*count = 256*/, 
        const std::wstring& /*in*/    par_publisher /*count = 256*/, 
        const std::wstring& /*in*/    par_comments /*count = 256*/, 
        const std::wstring& /*in*/    par_help_link /*count = 256*/, 
        const std::wstring& /*in*/    par_help_phone /*count = 256*/, 
        const std::wstring& /*in*/    par_classification /*count = 256*/, 
        const std::wstring& /*in*/    par_more_info_url /*count = 256*/);

    void kldb_hst_delete_inventory_patch (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_lHost /*count = 0*/, 
        const std::wstring& /*in*/    par_str_id /*count = 64*/);

    KLDB::DbRecordsetPtr kldb_hst_get_inventory_patches (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_host_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_rpt_get_viract_extra (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nMaxCount /*count = 0*/);

    void kldb_rpt_on_viract_extra_absent (
        KLDB::DbConnectionPtr    pCon);

    void kldb_srvhrch_get_last_visible (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nSlaveSrvId /*count = 0*/, 
        KLSTD::precise_time_t& /*out*/    par_tmLastVisible /*count = 0*/);

    void kldb_wol_ipconfig_clear (
        KLDB::DbConnectionPtr    pCon);

    void kldb_wol_ipconfig_add (
        KLDB::DbConnectionPtr    pCon, 
        AVP_longlong /*in*/    par_nIp /*count = 0*/, 
        AVP_longlong /*in*/    par_nSubnetMask /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_wol_tsk_get_subnets (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nTaskId /*count = 0*/, 
        AVP_longlong& /*out*/    par_minSubnetMask /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_wol_tsk_select_subnet_hosts (
        KLDB::DbConnectionPtr    pCon, 
        AVP_longlong /*in*/    par_nSubnetMask /*count = 0*/, 
        AVP_longlong /*in*/    par_nSubnetIp /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_wol_tsk_select_subnet_hosts_from_task (
        KLDB::DbConnectionPtr    pCon, 
        AVP_longlong /*in*/    par_nSubnetMask /*count = 0*/, 
        AVP_longlong /*in*/    par_nSubnetIp /*count = 0*/, 
        int /*in*/    par_nTaskId /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_wol_tsk_get_ua_macs (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nTaskId /*count = 0*/);

    void kldb_adhst_populate_child_chain (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_unit_id /*count = 0*/, 
        int /*in*/    par_sublevel /*count = 0*/);

    void kldb_hst_register_key (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nAppId /*count = 0*/, 
        const std::wstring& /*in*/    par_strSerial /*count = 32*/, 
        const std::wstring& /*in*/    par_strName /*count = 256*/, 
        int /*in*/    par_nLicCount /*count = 0*/, 
        int /*in*/    par_nPeriod /*count = 0*/, 
        int /*in*/    par_nType /*count = 0*/, 
        int /*in*/    par_nProdSuiteId /*count = 0*/, 
        const std::wstring& /*in*/    par_strMajVer /*count = 128*/, 
        int& /*out*/    par_nId /*count = 0*/);

    void kldb_hst_update_roaming_mode_bit (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nId /*count = 0*/, 
        bool /*in*/    par_bInRoamingMode /*count = 0*/);

    void kldb_hst_set_visibility_timeout (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nVisibilityTimeout /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_hst_resolve_and_move_to_group (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nSessionId /*count = 0*/, 
        int /*in*/    par_nGroupId /*count = 0*/);

    void kldb_licsrv_install_key (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nAppId /*count = 0*/, 
        const std::wstring& /*in*/    par_strSerial /*count = 32*/, 
        const std::wstring& /*in*/    par_strName /*count = 256*/, 
        int /*in*/    par_nLicCount /*count = 0*/, 
        int /*in*/    par_nPeriod /*count = 0*/, 
        int /*in*/    par_nType /*count = 0*/, 
        int /*in*/    par_nProdSuiteId /*count = 0*/, 
        const std::wstring& /*in*/    par_strMajVer /*count = 128*/, 
        KLSTD::MemoryChunkPtr /*in*/    par_binKeyData /*count = 0*/, 
        bool /*in*/    par_bIsFlexible /*count = 0*/, 
        int /*in*/    par_nEffectivePeriod /*count = 0*/, 
        int /*in*/    par_nEffectiveLicNumber /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_tmCreation /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_tmExpirationLimit /*count = 0*/);

    void kldb_licsrv_uninstall_key (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_strSerial /*count = 32*/);

    KLDB::DbRecordsetPtr kldb_hst_recalc_autokey (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_hst_get_autokeys_info (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nHostId /*count = 0*/);

    void kldb_hst_set_autokeys_hstver (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nHostId /*count = 0*/, 
        int /*in*/    par_nKeyId /*count = 0*/, 
        AVP_longlong /*in*/    par_nHstVersion /*count = 0*/);

    void kldb_grp_add_full_group_name (
        KLDB::DbConnectionPtr    pCon);

    void kldb_acm_add (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nSessionId /*count = 0*/, 
        int /*in*/    par_nGroupId /*count = 0*/, 
        bool /*in*/    par_bRead /*count = 0*/, 
        bool /*in*/    par_bWrite /*count = 0*/, 
        int /*in*/    par_nAccessMask /*count = 0*/);

    void kldb_acm_delete_for_session (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nSessionId /*count = 0*/);

    void kldb_acm_delete_all (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_grp_get_childs (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nParentGroupId /*count = 0*/);

    void kldb_sts_get_inst_status (
        KLDB::DbConnectionPtr    pCon, 
        int& /*out*/    par_ttl_hst_cnt /*count = 0*/, 
        int& /*out*/    par_avp_hst_cnt /*count = 0*/, 
        int& /*out*/    par_server_hst_in_groups /*count = 0*/, 
        int& /*out*/    par_server_hst_has_avp /*count = 0*/);

    void kldb_sts_get_lic_status (
        KLDB::DbConnectionPtr    pCon, 
        KLSTD::precise_time_t /*in*/    par_limit_expiration /*count = 0*/, 
        int& /*out*/    par_expired_cnt /*count = 0*/, 
        int& /*out*/    par_exparing_cnt /*count = 0*/, 
        int& /*out*/    par_exparing_lic_id /*count = 0*/, 
        std::wstring& /*out*/    par_exparing_lic_serial /*count = 32*/, 
        KLSTD::precise_time_t& /*out*/    par_exparing_time /*count = 0*/);

    void kldb_sts_set_last_task_viewed (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_time /*count = 0*/);

    void kldb_sts_get_ri_status (
        KLDB::DbConnectionPtr    pCon, 
        int& /*out*/    par_task_id /*count = 0*/, 
        std::wstring& /*out*/    par_task_name /*count = KLDBCONST_UT_TASK_NAME_COUNT*/, 
        int& /*out*/    par_running_percent /*count = 0*/, 
        int& /*out*/    par_failed_cnt /*count = 0*/, 
        int& /*out*/    par_need_rbt_cnt /*count = 0*/, 
        int& /*out*/    par_ok_cnt /*count = 0*/, 
        int& /*out*/    par_total_cnt /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_sts_get_hst_cnt_by_status (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nStatusId /*count = 0*/, 
        int /*in*/    par_nStatusMask /*count = 0*/, 
        int& /*out*/    par_nHostsCnt /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_sts_get_hst_cnt_by_status_mask (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nSessionId /*count = 0*/, 
        int /*in*/    par_nStatusMask /*count = 0*/, 
        int& /*out*/    par_nHostsCnt /*count = 0*/);

    void kldb_sts_get_adm_info (
        KLDB::DbConnectionPtr    pCon, 
        int& /*out*/    par_nGroupsCnt /*count = 0*/, 
        int& /*out*/    par_nAssignedHstsCnt /*count = 0*/, 
        int& /*out*/    par_nUnassignedHostsCnt /*count = 0*/);

    void kldb_sts_check_names_conflict (
        KLDB::DbConnectionPtr    pCon, 
        int& /*out*/    par_nConflict /*count = 0*/);

    void kldb_sts_check_crt_srv_events (
        KLDB::DbConnectionPtr    pCon, 
        int& /*out*/    par_nCnt /*count = 0*/);

    void kldb_sts_set_crt_srv_events_viewed (
        KLDB::DbConnectionPtr    pCon);

    void kldb_sts_get_srv_task_failed (
        KLDB::DbConnectionPtr    pCon, 
        int& /*out*/    par_task_id /*count = 0*/, 
        std::wstring& /*out*/    par_task_name /*count = KLDBCONST_UT_TASK_NAME_COUNT*/);

    KLDB::DbRecordsetPtr kldb_sts_get_hst_never_scanned_first_days_cnt (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nDays /*count = 0*/, 
        int& /*out*/    par_nHostsCnt /*count = 0*/);

    void kldb_tsk_get_host_move_rule_id (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        int& /*out*/    par_hst_move_rule_id /*count = 0*/);

    void kldb_tsk_set_host_move_rule_id (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        int /*in*/    par_hst_move_rule_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_dsh_prepare_viruses_and_net_attack_statistics (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_dsh_get_virus_statistics (
        KLDB::DbConnectionPtr    pCon, 
        KLSTD::precise_time_t /*in*/    par_start_time /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_end_time /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_dsh_get_net_attack_statistics (
        KLDB::DbConnectionPtr    pCon, 
        KLSTD::precise_time_t /*in*/    par_start_time /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_end_time /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_sts_get_srv_upd_task_state (
        KLDB::DbConnectionPtr    pCon, 
        int& /*out*/    par_state /*count = 0*/, 
        KLSTD::precise_time_t& /*out*/    par_rise_time /*count = 0*/, 
        int& /*out*/    par_percent /*count = 0*/, 
        std::wstring& /*out*/    par_strCurrentStateDescr /*count = 1000*/);

    KLDB::DbRecordsetPtr kldb_sts_get_srv_bkp_task_state (
        KLDB::DbConnectionPtr    pCon, 
        int& /*out*/    par_state /*count = 0*/, 
        KLSTD::precise_time_t& /*out*/    par_rise_time /*count = 0*/, 
        std::wstring& /*out*/    par_strCurrentStateDescr /*count = 1000*/);

    KLDB::DbRecordsetPtr kldb_tsk_get_ri_staff (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/);

    void kldb_tsk_set_ri_staff (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        int /*in*/    par_virtual_percent /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_virtual_percent_start_time /*count = 0*/, 
        int /*in*/    par_virtual_percent_time_seconds /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_dsh_get_event_histogram (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_wstrProductName /*count = 32*/, 
        const std::wstring& /*in*/    par_wstrProductVersion /*count = 32*/, 
        const std::wstring& /*in*/    par_strEventType /*count = 50*/, 
        int /*in*/    par_nSeverity /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_tmFinish /*count = 0*/, 
        int /*in*/    par_nPeriodSec /*count = 0*/, 
        int /*in*/    par_nRowCount /*count = 0*/, 
        int /*in*/    par_nSessionId /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_dsh_get_avp_viract_histogram (
        KLDB::DbConnectionPtr    pCon, 
        KLSTD::precise_time_t /*in*/    par_tmFinish /*count = 0*/, 
        int /*in*/    par_nPeriodSec /*count = 0*/, 
        int /*in*/    par_nRowCount /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_dsh_get_grp_tsk_histogram (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_task_id /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_tmFinish /*count = 0*/, 
        int /*in*/    par_nPeriodSec /*count = 0*/, 
        int /*in*/    par_nRowCount /*count = 0*/);

    void kldb_rpt_initialize_avp_prod_types (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_reset_avp_prod_types (
        KLDB::DbConnectionPtr    pCon);

    void kldb_rpt_add_avp_prod_type (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_wstrProductName /*count = 32*/, 
        int /*in*/    par_nType /*count = 0*/);

    void kldb_rpt_initialize_rpt_viract_results (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_dsh_get_av_records_histogram (
        KLDB::DbConnectionPtr    pCon, 
        KLSTD::precise_time_t /*in*/    par_tmFinish /*count = 0*/, 
        int /*in*/    par_nPeriodSec /*count = 0*/, 
        int /*in*/    par_nRowCount /*count = 0*/);

    void kldb_rpt_add_av_records (
        KLDB::DbConnectionPtr    pCon, 
        AVP_longlong /*in*/    par_lCount /*count = 0*/);

    void kldb_rpt_update_avp_stat_history (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_dsh_get_avp_histogram (
        KLDB::DbConnectionPtr    pCon, 
        KLSTD::precise_time_t /*in*/    par_tmFinish /*count = 0*/, 
        int /*in*/    par_nPeriodSec /*count = 0*/, 
        int /*in*/    par_nRowCount /*count = 0*/);

    void kldb_rpt_exec_viract_base (
        KLDB::DbConnectionPtr    pCon, 
        const std::wstring& /*in*/    par_condition /*count = 4000*/, 
        KLSTD::precise_time_t /*in*/    par_start_time /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_end_time /*count = 0*/);

    void kldb_rpt_fill_rpt_host_tmp_nogroup (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_report_id /*count = 0*/, 
        int /*in*/    par_session_id /*count = 0*/);

    void kldb_rpt_fill_rpt_host_tmp_group (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_group_id /*count = 0*/, 
        int /*in*/    par_session_id /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_dsh_get_viract_act_by_vir (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_dsh_get_viract_vir_by_avp (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_dsh_get_viract_act_by_avp (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_dsh_get_viract_mi_hosts (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_dsh_get_viract_mi_users (
        KLDB::DbConnectionPtr    pCon);

    KLDB::DbRecordsetPtr kldb_dsh_get_prod_versions (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nSessionId /*count = 0*/, 
        const std::wstring& /*in*/    par_wstrProductName /*count = 32*/, 
        const std::wstring& /*in*/    par_wstrProductVersion /*count = 32*/);

    KLDB::DbRecordsetPtr kldb_dsh_get_errors (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nSessionId /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_start_time /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_end_time /*count = 0*/, 
        const std::wstring& /*in*/    par_strTaskFailed /*count = 256*/);

    KLDB::DbRecordsetPtr kldb_dsh_get_net_attacks (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nSessionId /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_start_time /*count = 0*/, 
        KLSTD::precise_time_t /*in*/    par_end_time /*count = 0*/);

    KLDB::DbRecordsetPtr kldb_dsh_lic (
        KLDB::DbConnectionPtr    pCon);

    void kldb_tsk_ri_add_subpkg_start (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nTaskId /*count = 0*/, 
        const std::wstring& /*in*/    par_strHost /*count = 256*/, 
        int /*in*/    par_nManualStart /*count = 0*/, 
        int /*in*/    par_nFirstPkgSkipped /*count = 0*/);

    void kldb_tsk_ri_get_subpkg_start (
        KLDB::DbConnectionPtr    pCon, 
        int& /*out*/    par_nTaskId /*count = 0*/, 
        std::wstring& /*out*/    par_strHost /*count = 256*/, 
        int& /*out*/    par_nManualStart /*count = 0*/, 
        int& /*out*/    par_nFirstPkgSkipped /*count = 0*/);

    void kldb_tsk_ri_reset_subpkg_start (
        KLDB::DbConnectionPtr    pCon, 
        int /*in*/    par_nTaskId /*count = 0*/, 
        const std::wstring& /*in*/    par_strHost /*count = 256*/);

};
