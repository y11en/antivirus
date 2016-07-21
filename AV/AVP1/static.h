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
	
#define Get_AVP1()    ((hENGINE)DupGetCustomDWord(1))
#define Set_AVP1(x)             DupSetCustomDWord(1,(DWORD)(x))
#define Get_CurrentIO()   ((hIO)DupGetCustomDWord(2))
#define Set_CurrentIO(x)        DupSetCustomDWord(2,(DWORD)(x))
	
STATIC_EXTERN_INIT_ZERO( hROOT,   g_root );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_scan_object );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_detect_name );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_detect_type );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_detect_danger );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_detect_behaviour );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_detect_certanity );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_detect_disinfectability );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_detect_counter );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_virtual_name );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_workarea_need_save );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_has_special_cure );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_recursed );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_processing_error );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_io_created );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_io_position_ptr );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_temp_path );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_action_class_mask );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_object_readonly_error );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_object_readonly_object );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_integral_parent_io );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_hash_container );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_os_hash );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_npENGINE_OBJECT_OBTAINED_TRANSFORMER_NAME );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_npENGINE_OBJECT_TRANSFORMER_NAME );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_object_skip );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_object_executable_parent );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_boot_BytesPerSector );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_boot_SectorsPerTrack );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_boot_TracksPerCylinder );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_boot_Cylinders );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_auto_password_bool );
STATIC_EXTERN_INIT_ZERO( tPROPID, propid_action_class_mask_current );



tVOID* obj_malloc(tUINT size);
tVOID  obj_free(tVOID* ptr);

#if defined( __cplusplus )
} // extern "C"
#endif

#endif //_STATIC_H_
