// static.h for avp1 module
#ifndef _STATIC_H_
#define _STATIC_H_

#if defined( __cplusplus )
extern "C" {
#endif
	
#ifdef PLUGIN_STATIC_DATA
#define STATIC_EXTERN_INIT_ZERO(type,name) type name = (type)0
#else
#define STATIC_EXTERN_INIT_ZERO(type,name) extern type name
#endif
	
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_virtual_name );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_os_offset_list );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_os_offset );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_action_class_mask );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_action_class_mask_current );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_integral_parent_io );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_object_parent_io );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_object_executable_parent );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_hash_temp_list );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_hash_container );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_postprocess_list );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_os_hash );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_detect_state );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_detect_counter );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_detect_type );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_detect_name );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_detect_certanity );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_detect_disinfectability );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_detect_danger );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_detect_behaviour );

STATIC_EXTERN_INIT_ZERO( tPROPID, propid_object_infected_parent );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_object_readonly_error );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_object_set_write_access_error );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_object_readonly_object );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_handled_error );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_processing_error );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_object_session );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_object_main );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_uniarc_mini_pid );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_object_skip );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_creator_engine_pid );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_object_os_passed );

STATIC_EXTERN_INIT_ZERO( tPROPID, propid_npENGINE_OBJECT_OBTAINED_TRANSFORMER_NAME );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_npENGINE_OBJECT_TRANSFORMER_NAME );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_password_list );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_password_node );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_reopen_data_io );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_reopen_data_offset );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_reopen_user_data );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_auto_password_bool );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_boot_BytesPerSector );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_password_str );
//==============================================================
#ifdef KLAVPMGR
    STATIC_EXTERN_INIT_ZERO( tPROPID, propid_klavpmgr_action_name );
#endif // KLAVPMGR
//==============================================================
	

#if defined( __cplusplus )
} // extern "C"
#endif

#endif //_STATIC_H_
