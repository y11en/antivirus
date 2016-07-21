// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Tuesday,  24 January 2006,  16:14 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Kernel interfaces
// Sub project -- Not defined
// Purpose     -- Kernel interfaces
// Author      -- petrovitch
// File Name   -- k_root.c
// Additional info
//    Kernel interface implementations
// -------------------------------------------
// AVP Prague stamp end

#define PR_IMPEX_DEF

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions,  )
#define Root_PRIVATE_DEFINITION
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Includes for interface,  )
#include "k_root.h"
// AVP Prague stamp end



#include <Prague/iface/i_loader.h>
#include "k_trace.h"

#define IMPEX_IMPORT_LIB
#include <Prague/iface/e_loader.h>
#include <Prague/plugin/p_string.h>

#define IMPEX_EXPORT_LIB
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_ktrace.h>


IMPORT_TABLE(import_loader_table)
	#define IMPEX_TABLE_CONTENT
	#include <Prague/iface/e_loader.h>
IMPORT_TABLE_END

IMPORT_TABLE(import_string_table)
	#define IMPEX_TABLE_CONTENT
	#include <Prague/plugin/p_string.h>
IMPORT_TABLE_END


#include <m_utils.h>


#define NAMED_CLUSTER 8

//static tERROR pr_call _DelCustomPropList( tPO* po, tRootData* data );


// list of the plugins loaded
// duiring ROOT closing process they
// must be closed AFTER all other objects in the system
tHANDLE* g_plugins;
tHANDLE* g_statics;
tHANDLE* g_specials;
hTRACER  g_tracer;



// handle of the plugin loading at the moment 
// to place to tINTERFACE::plugin field duiring iface registation
// to prevent the situation when really BAD module registers MORE interfaces than exposed in its description (DSC file)
hPLUGIN g_now_loading = 0;

// ---
iINTERNAL INTERNAL_NULL_TABLE = {
  (tIntFnRecognize)        NULL,
  (tIntFnObjectNew)        NULL,
  (tIntFnObjectInit)       NULL,
  (tIntFnObjectInitDone)   NULL,
  (tIntFnObjectCheck)      NULL,
  (tIntFnObjectPreClose)   NULL,
  (tIntFnObjectClose)      NULL,
  (tIntFnChildNew)         NULL,
  (tIntFnChildInitDone)    NULL,
  (tIntFnChildClose)       NULL,
  (tIntFnMsgReceive)       NULL,
  (tIntFnIFaceTransfer)    NULL
};



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Root interface implementation
// Short comments -- //rus:корневой интерфейс ядра //eng:kernel root interface
//    //rus:Интерфейс предоставляет базовые возможности ядра Праги, в дополнение к тем, которые определены в наборе системных методов
//      - Регистрация и разрегистрация интерфейсов
//      - Регистрация и разрегистрация идентификаторов свойств, определяемых пользователем
//      - Перебор модулей
//      - Регистрация экспортных функций, определяемых в модуле
//      - Импортирование функций, реализованных в дргих модулях
//      - Трассировка
//    //eng:Interface supplies common kernel facility in adition to System methods
// Short comments -- kernel root interface
// Extended comment -
//  Interface supplies common kernel facility
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// AVP Prague stamp end



tRootObject root_obj;


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call Root_ObjectInit( hi_Root _this );
tERROR pr_call Root_ObjectInitDone( hi_Root _this );
tERROR pr_call Root_ObjectPreClose( hi_Root _this );
tERROR pr_call Root_ObjectClose( hi_Root _this );
tERROR pr_call Root_MsgReceive( hi_Root _this, tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_Root_vtbl = {
	(tIntFnRecognize)         NULL,
	(tIntFnObjectNew)         NULL,
	(tIntFnObjectInit)        Root_ObjectInit,
	(tIntFnObjectInitDone)    Root_ObjectInitDone,
	(tIntFnObjectCheck)       NULL,
	(tIntFnObjectPreClose)    Root_ObjectPreClose,
	(tIntFnObjectClose)       Root_ObjectClose,
	(tIntFnChildNew)          NULL,
	(tIntFnChildInitDone)     NULL,
	(tIntFnChildClose)        NULL,
	(tIntFnMsgReceive)        Root_MsgReceive,
	(tIntFnIFaceTransfer)     NULL
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )
// ----------- Interface management ----------
typedef   tERROR (pr_call *fnpRoot_RegisterIFace)                  ( hi_Root _this, tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid, const tVOID* internal_table, tDWORD internal_table_size, const tVOID* external_table, tDWORD external_table_size, const tDATA* prop_table, tDWORD prop_table_size, tDWORD memsize, tDWORD flags ); // -- Registers an interface;
typedef   tERROR (pr_call *fnpRoot_UnregisterIFace)                ( hi_Root _this, tIID iid, tPID pid, tDWORD subtype, tVID vid ); // -- Unregisters an interface;
																														
// ----------- Plugin management ----------									
typedef   tERROR (pr_call *fnpRoot_FindPlugins)                    ( hi_Root _this, tPID mfinder_pid, const tVOID* param_pool, tDWORD param_pool_size, tDWORD load_flags ); // -- //rus:интегрирование модулей наденных указанным загрузчиком в систему//eng:integrate modules to the system by specific loader;
																														
// ----------- Custom property methods ----------						
typedef   tERROR (pr_call *fnpRoot_RegisterCustomPropId)           ( hi_Root _this, tDWORD* result, tSTRING name, tDWORD type ); // -- allocates unique property id;
typedef   tERROR (pr_call *fnpRoot_UnregisterCustomPropId)         ( hi_Root _this, tSTRING name ); // -- deallocates unique property id;
typedef   tERROR (pr_call *fnpRoot_GetCustomPropId)                ( hi_Root _this, tDWORD* result, tSTRING name, tDWORD type ); // -- returns id of the named property identifier;
																														
// ----------- Tracing methods ----------										
typedef   tERROR (pr_call *fnpRoot_TraceLevelSet)                  ( hi_Root _this, tDWORD op, tIID iid, tPID pid, tDWORD subtype, tDWORD vid, tDWORD level, tDWORD min_level ); // -- set trace level by interface or/and by pid;
typedef   tERROR (pr_call *fnpRoot_GetTracer)                      ( hi_Root _this, hTRACER* result );             // -- returns current tracer;
																														
// ----------- Plugin management ----------									
typedef   tERROR (pr_call *fnpRoot_GetPluginInstance)              ( hi_Root _this, hPLUGIN* result, tDATA* cookie, tPID pid ); // -- returns module matched the criteria;
																														
// ----------- Interface management ----------							
typedef   tERROR (pr_call *fnpRoot_RegisterIFaceEx)                ( hi_Root _this, hIFACE* result, tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid, const tVOID* internal_table, tDWORD internal_table_size, const tVOID* external_table, tDWORD external_table_size, const tDATA* prop_table, tDWORD prop_table_size, tDWORD memsize, tDWORD flags, tIID iid_compat, hPLUGIN implemented_module, tDWORD implemented_index ); // -- Registers an interface;
																														
// ----------- import/export management ----------					
typedef   tERROR (pr_call *fnpRoot_RegisterExportTable)            ( hi_Root _this, tDWORD* result, const tEXPORT* ex_tbl, tPID exporter_pid ); // -- register export function table;
typedef   tERROR (pr_call *fnpRoot_RegisterExportFunc)             ( hi_Root _this, const tFUNC_PTR func_ptr, tDWORD func_cls, tDWORD func_id, tPID exporter_pid ); // -- register single function pointer;
typedef   tERROR (pr_call *fnpRoot_RegisterExportFuncByName)       ( hi_Root _this, const tFUNC_PTR func_ptr, tDWORD func_cls, tSTRING func_name, tPID exporter_pid ); // -- register single function pointer;
typedef   tERROR (pr_call *fnpRoot_ResolveImportTable)             ( hi_Root _this, tDWORD* result, const tIMPORT* imp_tbl, tPID importer_pid ); // -- resolve import table;
typedef   tERROR (pr_call *fnpRoot_ResolveImportFunc)              ( hi_Root _this, tFUNC_PTR* result, tDWORD func_cls, tDWORD func_id, tPID pid ); // -- resolve single func pointer;
typedef   tERROR (pr_call *fnpRoot_ResolveImportFuncByName)        ( hi_Root _this, tFUNC_PTR* result, tDWORD func_cls, const tSTRING func_name, tPID importer_pid ); // -- resolve single func pointer;
typedef   tERROR (pr_call *fnpRoot_ReleaseImportTable)             ( hi_Root _this, tDWORD* result, const tIMPORT* imp_tbl, tPID importer_pid ); // -- releases import reference;
typedef   tERROR (pr_call *fnpRoot_ReleaseImportFunc)              ( hi_Root _this, tDWORD func_cls, tDWORD func_id, tPID pid ); // -- releases single func pointer;
typedef   tERROR (pr_call *fnpRoot_ReleaseImportFuncByName)        ( hi_Root _this, tDWORD func_cls, const tSTRING func_name, tPID importer_pid ); // -- releases single func pointer;
																														
// ----------- Plugin management ----------									
typedef   tERROR (pr_call *fnpRoot_PluginUnloadNotification)       ( hi_Root _this, hPLUGIN plugin_unloaded ); // -- outsiders notify impex manager about plugin unloading;
typedef   tERROR (pr_call *fnpRoot_PluginLoadNotification)         ( hi_Root _this, hPLUGIN plugin_loaded ); // -- outsiders notify root about plugin loading;
typedef   tERROR (pr_call *fnpRoot_FreeUnusedPlugins)              ( hi_Root _this, tPID* pid_array, tDWORD pid_array_count, tFUP_ACTION action, tDWORD during ); // -- forces unload unsued plugins;
																														
// ----------- Notification Sink management ----------			
typedef   tERROR (pr_call *fnpRoot_SetNotificationSink)            ( hi_Root _this, tDWORD* id, tSNS_ACTION type, tFUNC_PTR func, tPTR ctx ); // -- //rus:установить функцию-приемник уведомления о событии kernel'a;
typedef   tERROR (pr_call *fnpRoot_FreeNotificationSink)           ( hi_Root _this, tDWORD id ); // -- //rus:снять функцию-приемник уведомления о событии kernel'a;
																														
// ----------- Interface management ----------							
typedef   tERROR (pr_call *fnpRoot_CheckIFace)                     ( hi_Root _this, tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid, tDWORD flags, hPLUGIN implemented_module ); // -- //rus:проверка интерфейса//eng:Check an interface;

// ----------- kernel internal synchronization object ----------
typedef   tERROR (pr_call *fnpRoot_SetKernelSyncObject)            ( hi_Root _this, tIID iid, tDWORD pid, tDWORD subtype ); // -- force kernel to use synchronization object selected by iid, pid, and subtype;
typedef   tERROR (pr_call *fnpRoot_GetKernelSyncObject)            ( hi_Root _this, hOBJECT* sync_object ); // -- kernel used synchronization object;

// -----------  kernel support serializable structures ----------
typedef   tERROR (pr_call *fnpRoot_RegisterSerializableDescriptor)   ( hi_Root _this, const cSerDescriptor* descriptor ); // -- ;
typedef   tERROR (pr_call *fnpRoot_UnregisterSerializableDescriptor) ( hi_Root _this, const cSerDescriptor* descriptor ); // -- ;
typedef   tERROR (pr_call *fnpRoot_FindSerializableDescriptor)       ( hi_Root _this, cSerDescriptor** descriptor, tDWORD uniqueId ); // -- ;
typedef   tERROR (pr_call *fnpRoot_CreateSerializable)               ( hi_Root _this, tDWORD unique_id, cSerializable** object ); // -- ;
typedef   tERROR (pr_call *fnpRoot_DestroySerializable)              ( hi_Root _this, cSerializable* object ); // -- ;
typedef   tERROR (pr_call *fnpRoot_CopySerializable)                 ( hi_Root _this, cSerializable** dst, const cSerializable* src ); // -- ;
typedef   tERROR (pr_call *fnpRoot_StreamSerialize)                  ( hi_Root _this, const cSerializable* object, tDWORD ser_id, tBYTE* buffer, tDWORD size, tDWORD* output_size, tDWORD flags ); // -- ;
typedef   tERROR (pr_call *fnpRoot_StreamDeserialize)                ( hi_Root _this, cSerializable** object, const tBYTE* buffer, tDWORD size, tDWORD* accepted_size ); // -- ;
typedef   tERROR (pr_call *fnpRoot_RegSerialize)                     ( hi_Root _this, const cSerializable* object, tDWORD ser_id, hREGISTRY registry, const tCHAR* path ); // -- ;
typedef   tERROR (pr_call *fnpRoot_RegDeserialize)                   ( hi_Root _this, cSerializable** object, hREGISTRY registry, const tCHAR* path, tDWORD uniqueId ); // -- ;

// ----------- kernel locking !!! be careful !!! ----------
typedef   tERROR (pr_call *fnpRoot_LockKernel)                       ( hi_Root _this );   // -- ;
typedef   tERROR (pr_call *fnpRoot_UnlockKernel)                     ( hi_Root _this );   // -- ;

// ----------- Plugin management ----------
typedef   tERROR (pr_call *fnpRoot_LoadModule)                       ( hi_Root _this, hPLUGIN* plugin, const tVOID* module_name, tDWORD name_size, tCODEPAGE cp ); // -- load module by name;

typedef   tERROR (pr_call *fnpRoot_GetCustomPropName)                ( hi_Root _this, tDWORD* p_out_size, tPROPID p_prop_id, tPTR p_buffer, tDWORD p_size );

// ----------- kernel support serializable structures ----------
typedef   tERROR (pr_call *fnpRoot_StreamSerializeField)             ( hi_Root _this, const tVOID* field, const cSerDescriptorField* descriptor, cOutStream* stream, tDWORD reserved ); // -- //eng:serialize one field of serializable structure;
typedef   tERROR (pr_call *fnpRoot_RegSerializeField)                ( hi_Root _this, const tVOID* field, const cSerDescriptorField* descriptor, hREGISTRY registry, const tCHAR* path, tDWORD reserved ); // -- ;
typedef   tERROR (pr_call *fnpRoot_RegDeserializeField)              ( hi_Root _this, tVOID* field, const cSerDescriptorField* descriptor, hREGISTRY registry, const tCHAR* path, tDWORD reserved ); // -- //eng:deserialize one field of serializable structure;
typedef   tERROR (pr_call *fnpRoot_RegSerializeEx)                   ( hi_Root _this, const cSerializable* p_object, tDWORD ser_id, hREGISTRY p_registry, const tCHAR* p_path, tSerializeCallback p_callback_func, tPTR p_callback_params );
typedef   tERROR (pr_call *fnpRoot_RegDeserializeEx)                 ( hi_Root _this, cSerializable** p_object, hREGISTRY p_registry, const tCHAR* p_path, tDWORD p_uniqueId, tSerializeCallback p_callback_func, tPTR p_callback_params );
typedef   tERROR (pr_call *fnpRoot_CompareSerializable)              ( hi_Root _this, const cSerializable* str1, const cSerializable* str2, tDWORD ser_id, tDWORD flags );
typedef   tERROR (pr_call *fnpRoot_FindSerializableDescriptorByName) ( hi_Root _this, cSerDescriptor** descriptor, const tCHAR* name );
typedef   tERROR (pr_call *fnpRoot_AddRefSerializable)               ( hi_Root _this, cSerDescriptor* descriptor );
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
// ----------- Plugin management ----------

struct iRootVtbl {

// ----------- Interface management ----------
  fnpRoot_RegisterIFace             RegisterIFace;
  fnpRoot_UnregisterIFace           UnregisterIFace;

  // ----------- Plugin management ----------
  fnpRoot_FindPlugins               FindPlugins;

  // ----------- Custom property methods ----------
  fnpRoot_RegisterCustomPropId      RegisterCustomPropId;
  fnpRoot_UnregisterCustomPropId    UnregisterCustomPropId;
  fnpRoot_GetCustomPropId           GetCustomPropId;

  // ----------- Tracing methods ----------
  fnpRoot_TraceLevelSet             TraceLevelSet;
  fnpRoot_GetTracer                 GetTracer;

  // ----------- Plugin management ----------
  fnpRoot_GetPluginInstance         GetPluginInstance;

  // ----------- Interface management ----------
  fnpRoot_RegisterIFaceEx           RegisterIFaceEx;

  // ----------- import/export management ----------
  fnpRoot_RegisterExportTable       RegisterExportTable;
  fnpRoot_RegisterExportFunc        RegisterExportFunc;
  fnpRoot_RegisterExportFuncByName  RegisterExportFuncByName;
  fnpRoot_ResolveImportTable        ResolveImportTable;
  fnpRoot_ResolveImportFunc         ResolveImportFunc;
  fnpRoot_ResolveImportFuncByName   ResolveImportFuncByName;
  fnpRoot_ReleaseImportTable        ReleaseImportTable;
  fnpRoot_ReleaseImportFunc         ReleaseImportFunc;
  fnpRoot_ReleaseImportFuncByName   ReleaseImportFuncByName;

  // ----------- Plugin management ----------
  fnpRoot_PluginUnloadNotification  PluginUnloadNotification;
  fnpRoot_PluginLoadNotification    PluginLoadNotification;
  fnpRoot_FreeUnusedPlugins         FreeUnusedPlugins;

  // ----------- Notification Sink management ----------
  fnpRoot_SetNotificationSink       SetNotificationSink;
  fnpRoot_FreeNotificationSink      FreeNotificationSink;

	// ----------- Interface management ----------
  fnpRoot_CheckIFace                CheckIFace;
	
	// ----------- kernel internal synchronization object ----------
	fnpRoot_SetKernelSyncObject				SetKernelSyncObject;
	fnpRoot_GetKernelSyncObject       GetKernelSyncObject;
	
	// -----------  kernel support serializable structures ----------
	fnpRoot_RegisterSerializableDescriptor    RegisterSerializableDescriptor;
	fnpRoot_UnregisterSerializableDescriptor  UnregisterSerializableDescriptor;
	fnpRoot_FindSerializableDescriptor        FindSerializableDescriptor;
	fnpRoot_CreateSerializable                CreateSerializable;
	fnpRoot_DestroySerializable               DestroySerializable;
	fnpRoot_CopySerializable                  CopySerializable;
	fnpRoot_StreamSerialize                   StreamSerialize;
	fnpRoot_StreamDeserialize                 StreamDeserialize;
	fnpRoot_RegSerialize                      RegSerialize;
	fnpRoot_RegDeserialize                    RegDeserialize;
	
	// ----------- kernel locking !!! be careful !!! ----------
	fnpRoot_LockKernel                        LockKernel;
	fnpRoot_UnlockKernel                      UnlockKernel;
	
	// ----------- Plugin management ----------
	fnpRoot_LoadModule                        LoadModule;

	fnpRoot_GetCustomPropName                 Root_GetCustomPropName;
	
	// ----------- kernel support serializable structures ----------
	fnpRoot_StreamSerializeField              StreamSerializeField;
	fnpRoot_RegSerializeField                 RegSerializeField;
	fnpRoot_RegDeserializeField               RegDeserializeField;
	fnpRoot_RegSerializeEx                    RegSerializeEx;
	fnpRoot_RegDeserializeEx                  RegDeserializeEx;
	fnpRoot_CompareSerializable               CompareSerializable;
	fnpRoot_FindSerializableDescriptorByName  FindSerializableDescriptorByName;
  fnpRoot_AddRefSerializable                AddRefSerializable;
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table prototypes,  )

// методы для регистрации экспортных методов плагинов и импортирования методов, зарегистрированных как экспортные
// methods for module(plugin) management

// ----------  Interface management  ----------
// //rus:регистрация/разрегистрация интерфейсов //eng:register/unregister interfaces
// //rus:методы, регистрирующие интерфейсы у ядра Праги так, чтобы эти интерфейсы могли быть использованы для создания объектов
tERROR pr_call Root_RegisterIFace( hi_Root _this, tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid, const tVOID* internal_table, tDWORD internal_table_size, const tVOID* external_table, tDWORD external_table_size, const tDATA* prop_table, tDWORD prop_table_size, tDWORD memsize, tDWORD flags );
tERROR pr_call Root_UnregisterIFace( hi_Root _this, tIID iid, tPID pid, tDWORD subtype, tVID vid );

// ----------  Plugin management  ----------
// //rus:методы манипулирования плагинами//eng:methods for module(plugin) management
tERROR pr_call Root_FindPlugins( hi_Root _this, tPID mfinder_pid, const tVOID* param_pool, tDWORD param_pool_size, tDWORD load_flags );

// ----------  Custom property methods  ----------
// методы работы с идентификаторами пользовательских свойств
tERROR pr_call Root_RegisterCustomPropId( hi_Root _this, tDWORD* result, tSTRING name, tDWORD type );
tERROR pr_call Root_UnregisterCustomPropId( hi_Root _this, tSTRING name );
tERROR pr_call Root_GetCustomPropId( hi_Root _this, tDWORD* result, tSTRING name, tDWORD type );

// ----------  Tracing methods  ----------
// методы работы с отладочными сообщениями
tERROR pr_call Root_TraceLevelSet( hi_Root _this, tDWORD op, tIID iid, tPID pid, tDWORD subtype, tDWORD vid, tDWORD level, tDWORD min_level );
tERROR pr_call Root_GetTracer( hi_Root _this, hTRACER* result );

// ----------  Plugin management  ----------
// methods for module(plugin) management
tERROR pr_call Root_GetPluginInstance( hi_Root _this, hPLUGIN* result, tDATA* cookie, tPID pid );

// ----------  Interface management  ----------
// register/unregister interfaces
tERROR pr_call Root_RegisterIFaceEx( hi_Root _this, hIFACE* result, tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid, const tVOID* internal_table, tDWORD internal_table_size, const tVOID* external_table, tDWORD external_table_size, const tDATA* prop_table, tDWORD prop_table_size, tDWORD memsize, tDWORD flags, tIID iid_compat, hPLUGIN implemented_module, tDWORD implemented_index );

// ----------  import/export management  ----------
// методы для регистрации экспортных методов плагинов и импортирования методов, зарегистрированных как экспортные
tERROR pr_call Root_RegisterExportTable( hi_Root _this, tDWORD* result, const tEXPORT* ex_tbl, tPID exporter_pid );
tERROR pr_call Root_RegisterExportFunc( hi_Root _this, const tFUNC_PTR func_ptr, tDWORD func_cls, tDWORD func_id, tPID pid );
tERROR pr_call Root_RegisterExportFuncByName( hi_Root _this, const tFUNC_PTR func_ptr, tDWORD func_cls, tSTRING func_name, tPID pid );
tERROR pr_call Root_ResolveImportTable( hi_Root _this, tDWORD* result, const tIMPORT* imp_tbl, tPID importer_pid );
tERROR pr_call Root_ResolveImportFunc( hi_Root _this, tFUNC_PTR* result, tDWORD func_cls, tDWORD func_id, tPID pid );
tERROR pr_call Root_ResolveImportFuncByName( hi_Root _this, tFUNC_PTR* result, tDWORD func_cls, const tSTRING func_name, tPID pid );
tERROR pr_call Root_ReleaseImportTable( hi_Root _this, tDWORD* result, const tIMPORT* imp_tbl, tPID importer_pid );
tERROR pr_call Root_ReleaseImportFunc( hi_Root _this, tDWORD func_cls, tDWORD func_id, tPID pid );
tERROR pr_call Root_ReleaseImportFuncByName( hi_Root _this, tDWORD func_cls, const tSTRING func_name, tPID pid );

// ----------  Plugin management  ----------
// methods for module(plugin) management
tERROR pr_call Root_PluginUnloadNotification( hi_Root _this, hPLUGIN plugin_unloaded );
tERROR pr_call Root_PluginLoadNotification( hi_Root _this, hPLUGIN plugin_loaded );
tERROR pr_call Root_FreeUnusedPlugins( hi_Root _this, tPID* pid_array, tDWORD pid_array_count, tFUP_ACTION action, tDWORD during );

// ----------  Notification Sink management  ----------
// //rus:уведомления о событиях kernel'а
tERROR pr_call Root_SetNotificationSink( hi_Root _this, tDWORD* id, tSNS_ACTION type, tFUNC_PTR func, tPTR ctx );
tERROR pr_call Root_FreeNotificationSink( hi_Root _this, tDWORD id );

// ----------  Interface management  ----------
// register/unregister interfaces
tERROR pr_call Root_CheckIFace( hi_Root _this, tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid, tDWORD flags, hPLUGIN implemented_module );

// ----------- kernel internal synchronization object ----------
tERROR pr_call Root_SetKernelSyncObject            ( hi_Root _this, tIID iid, tDWORD pid, tDWORD subtype ); // -- force kernel to use synchronization object selected by iid, pid, and subtype;
tERROR pr_call Root_GetKernelSyncObject            ( hi_Root _this, hOBJECT* sync_object ); // -- kernel used synchronization object;

// -----------  kernel support serializable structures ----------
tERROR pr_call Root_RegisterSerializableDescriptor ( hi_Root _this, const cSerDescriptor* descriptor ); // -- ;
tERROR pr_call Root_UnregisterSerializableDescriptor ( hi_Root _this, const cSerDescriptor* descriptor ); // -- ;
tERROR pr_call Root_FindSerializableDescriptor       ( hi_Root _this, cSerDescriptor** descriptor, tDWORD uniqueId ); // -- ;
tERROR pr_call Root_CreateSerializable( hi_Root _this, tDWORD p_unique_id, cSerializable** p_object );
tERROR pr_call Root_DestroySerializable( hi_Root _this, cSerializable* p_object );
tERROR pr_call Root_CopySerializable( hi_Root _this, cSerializable** p_dst, const cSerializable* p_src );
tERROR pr_call Root_StreamSerialize( hi_Root _this, const cSerializable* p_object, tDWORD ser_id, tBYTE* p_buffer, tDWORD p_size, tDWORD* p_output_size, tDWORD flags );
tERROR pr_call Root_StreamDeserialize( hi_Root _this, cSerializable** p_object, const tBYTE* p_buffer, tDWORD p_size, tDWORD* p_accepted_size );
tERROR pr_call Root_RegSerialize( hi_Root _this, const cSerializable* p_object, tDWORD ser_id, hREGISTRY p_registry, const tCHAR* key );
tERROR pr_call Root_RegDeserialize( hi_Root _this, cSerializable** p_object, hREGISTRY p_registry, const tCHAR* key, tDWORD p_uniqueId );

// -----------  kernel locking !!! be careful !!! ----------
tERROR pr_call Root_LockKernel( hi_Root _this );
tERROR pr_call Root_UnlockKernel( hi_Root _this );

// ----------- Plugin management ----------
tERROR pr_call Root_LoadModule( hi_Root _this, hPLUGIN* plugin, const tVOID* module_name, tDWORD name_size, tCODEPAGE cp ); // -- load module by name;

tERROR pr_call Root_GetCustomPropName( hi_Root _this, tDWORD* p_out_size, tPROPID p_prop_id, tPTR p_buffer, tDWORD p_size );

tERROR pr_call Root_StreamSerializeField( hi_Root _this, const tVOID* p_field, const cSerDescriptorField* p_descriptor, cOutStream* p_stream, tDWORD p_reserved );
tERROR pr_call Root_RegSerializeField( hi_Root _this, const tVOID* p_field, const cSerDescriptorField* p_descriptor, hREGISTRY p_registry, const tCHAR* p_path, tDWORD p_reserved );
tERROR pr_call Root_RegDeserializeField( hi_Root _this, tVOID* p_field, const cSerDescriptorField* p_descriptor, hREGISTRY p_registry, const tCHAR* p_path, tDWORD p_reserved );
tERROR pr_call Root_RegSerializeEx( hi_Root _this, const cSerializable* p_object, tDWORD ser_id, hREGISTRY p_registry, const tCHAR* p_path, tSerializeCallback p_callback_func, tPTR p_callback_params );
tERROR pr_call Root_RegDeserializeEx( hi_Root _this, cSerializable** p_object, hREGISTRY p_registry, const tCHAR* p_path, tDWORD p_uniqueId, tSerializeCallback p_callback_func, tPTR p_callback_params );
tERROR pr_call Root_CompareSerializable( hi_Root _this, const cSerializable* str1, const cSerializable* str2, tDWORD ser_id, tDWORD flags );
tERROR pr_call Root_FindSerializableDescriptorByName( hi_Root _this, cSerDescriptor** p_descriptor, const tCHAR* name );
tERROR pr_call Root_AddRefSerializable( hi_Root _this, cSerDescriptor* p_descriptor );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External method table,  )
static iRootVtbl e_Root_vtbl = {
	(fnpRoot_RegisterIFace)   Root_RegisterIFace,
	(fnpRoot_UnregisterIFace) Root_UnregisterIFace,
	(fnpRoot_FindPlugins)     Root_FindPlugins,
	(fnpRoot_RegisterCustomPropId) Root_RegisterCustomPropId,
	(fnpRoot_UnregisterCustomPropId) Root_UnregisterCustomPropId,
	(fnpRoot_GetCustomPropId) Root_GetCustomPropId,
	(fnpRoot_TraceLevelSet)   Root_TraceLevelSet,
	(fnpRoot_GetTracer)       Root_GetTracer,
	(fnpRoot_GetPluginInstance) Root_GetPluginInstance,
	(fnpRoot_RegisterIFaceEx) Root_RegisterIFaceEx,
	(fnpRoot_RegisterExportTable) Root_RegisterExportTable,
	(fnpRoot_RegisterExportFunc) Root_RegisterExportFunc,
	(fnpRoot_RegisterExportFuncByName) Root_RegisterExportFuncByName,
	(fnpRoot_ResolveImportTable) Root_ResolveImportTable,
	(fnpRoot_ResolveImportFunc) Root_ResolveImportFunc,
	(fnpRoot_ResolveImportFuncByName) Root_ResolveImportFuncByName,
	(fnpRoot_ReleaseImportTable) Root_ReleaseImportTable,
	(fnpRoot_ReleaseImportFunc) Root_ReleaseImportFunc,
	(fnpRoot_ReleaseImportFuncByName) Root_ReleaseImportFuncByName,
	(fnpRoot_PluginUnloadNotification) Root_PluginUnloadNotification,
	(fnpRoot_PluginLoadNotification) Root_PluginLoadNotification,
	(fnpRoot_FreeUnusedPlugins) Root_FreeUnusedPlugins,
	(fnpRoot_SetNotificationSink) Root_SetNotificationSink,
	(fnpRoot_FreeNotificationSink) Root_FreeNotificationSink,
	(fnpRoot_CheckIFace)      Root_CheckIFace,
	(fnpRoot_SetKernelSyncObject) Root_SetKernelSyncObject,
	(fnpRoot_GetKernelSyncObject) Root_GetKernelSyncObject,
	(fnpRoot_RegisterSerializableDescriptor) Root_RegisterSerializableDescriptor,
	(fnpRoot_UnregisterSerializableDescriptor) Root_UnregisterSerializableDescriptor,
	(fnpRoot_FindSerializableDescriptor) Root_FindSerializableDescriptor,
	(fnpRoot_CreateSerializable) Root_CreateSerializable,
	(fnpRoot_DestroySerializable) Root_DestroySerializable,
	(fnpRoot_CopySerializable) Root_CopySerializable,
	(fnpRoot_StreamSerialize) Root_StreamSerialize,
	(fnpRoot_StreamDeserialize) Root_StreamDeserialize,
	(fnpRoot_RegSerialize)    Root_RegSerialize,
	(fnpRoot_RegDeserialize)  Root_RegDeserialize,
	(fnpRoot_LockKernel)      Root_LockKernel,
	(fnpRoot_UnlockKernel)    Root_UnlockKernel,
	(fnpRoot_LoadModule)      Root_LoadModule,
	(fnpRoot_GetCustomPropName) Root_GetCustomPropName,
	(fnpRoot_StreamSerializeField) Root_StreamSerializeField,
	(fnpRoot_RegSerializeField) Root_RegSerializeField,
	(fnpRoot_RegDeserializeField) Root_RegDeserializeField,
	(fnpRoot_RegSerializeEx)  Root_RegSerializeEx,
	(fnpRoot_RegDeserializeEx) Root_RegDeserializeEx,
	(fnpRoot_CompareSerializable) Root_CompareSerializable,
	(fnpRoot_FindSerializableDescriptorByName) Root_FindSerializableDescriptorByName,
	(fnpRoot_AddRefSerializable) Root_AddRefSerializable
};
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call Root_PROP_set_output_func( hi_Root _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call Root_PROP_get_obj_count( hi_Root _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call Root_PROP_loading_get( hi_Root _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call Root_PROP_loading_set( hi_Root _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call Root_PROP_set_unload_unused( hi_Root _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Root". Static(shared) property table variables
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface version,  )
#define Root_VERSION ((tVERSION)1)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Root_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, Root_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "kernel root interface", 22 )
	mSHARED_PROPERTY_PTR( pgOBJECT_NAME, "Root object", 12 )
	mLOCAL_PROPERTY( pgOUTPUT_FUNC, tRootData, output_func, cPROP_BUFFER_READ, NULL, Root_PROP_set_output_func )
	mLOCAL_PROPERTY_FN( pgOBJECT_COUNT, Root_PROP_get_obj_count, NULL )
	mSHARED_PROPERTY( pgEXCEPTION_CATCHING_AVAILABLE, ((tBOOL)(cFALSE)) )
	mSHARED_PROPERTY( pgSYNCHRONIZATION_AVAILABLE, ((tDWORD)(0)) )
	mLOCAL_PROPERTY_BUF( pgUNLOAD_UNUSED_DURING, tRootData, unuse_period, cPROP_BUFFER_READ_WRITE )
	mLOCAL_PROPERTY( pgUNLOAD_UNUSED_FORCE, tRootData, unload_unused_plugins, cPROP_BUFFER_READ, NULL, Root_PROP_set_unload_unused )
	mLOCAL_PROPERTY_FN( plMODULE_LOADING, Root_PROP_loading_get, Root_PROP_loading_set )
	mSHARED_PROPERTY( pgINTERFACE_CODEPAGE, ((tCODEPAGE)(cCP_ANSI)) )
	mSHARED_PROPERTY( pgOBJECT_CODEPAGE, ((tCODEPAGE)(cCP_ANSI)) )
mPROPERTY_TABLE_END(Root_PropTable)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call,  )
// Interface "Root". Register function
tERROR pr_call Root_Register( hROOT root ) {
  tERROR error;

  PR_TRACE_A0( 0, "Enter Root::Register" );
	error = Root_RegisterIFace(
		0,                                      // root object
		IID_ROOT,                               // interface identifier
		PID_KERNEL,                             // plugin identifier
		0x00000000,                             // subtype identifier
		Root_VERSION,                           // interface version
		VID_PETROVITCH,                         // interface developer
		&i_Root_vtbl,                           // internal(kernel called) function table
		(sizeof(i_Root_vtbl)/sizeof(tPTR)),     // internal function table size
		&e_Root_vtbl,                           // external function table
		(sizeof(e_Root_vtbl)/sizeof(tPTR)),     // external function table size
		Root_PropTable,                         // property table
		mPROPERTY_TABLE_SIZE(Root_PropTable),   // property table size
		sizeof(tRootData),                      // memory size
		IFACE_SYSTEM | IFACE_UNSWAPPABLE // interface flags
	);

  if ( error == errOK )
    PR_TRACE(( 0,prtNOT_IMPORTANT,"krn\tRoot interface (IID_ROOT) registered successfully" ));
  else {
    PR_TRACE(( 0, prtDANGER, "krn\tRoot interface (IID_ROOT) doesn't registered[%terr]", error ));
    error = errINTERFACE_ROOT_NOT_REGISTERED;
  }

  PR_TRACE_A1( 0, "Leave Root::Register ret t%terr", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_output_func )
// Interface "Root". Method "Set" for property(s):
//  -- OUTPUT_FUNC
tERROR pr_call Root_PROP_set_output_func( hi_Root _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
  tRootData* rd;
  PR_TRACE_A0( _this, "Enter *SET* method Root_PROP_set_output_func for property pgOUTPUT_FUNC" );

  rd = _this->data;
  if ( buffer ) {
    rd->output_func = *(tOUTPUT_FUNC*)buffer;
    if ( g_tracer )
      CALL_SYS_PropertySetPtr( g_tracer, pgOUTPUT_FUNC, rd->output_func );
  }
  *out_size = sizeof( rd->output_func );

  PR_TRACE_A1( _this, "Leave *SET* method Root_PROP_set_output_func for property pgOUTPUT_FUNC, ret tDWORD = %u(size), error = errOK", *out_size );
  return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, get_obj_count )
// Interface "Root". Method "Get" for property(s):
//  -- OBJECT_COUNT
tERROR pr_call Root_PROP_get_obj_count( hi_Root _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
  tERROR error = errOK;
  PR_TRACE_A0( _this, "Enter *GET* method Root_PROP_get_obj_count for property pgOBJECT_COUNT" );

  if ( buffer )
    *(tDWORD*)buffer = g_handle_count;
  *out_size = sizeof(tDWORD);

  PR_TRACE_A2( _this, "Leave *GET* method Root_PROP_get_obj_count for property pgOBJECT_COUNT, ret tDWORD = %u(size), error = %terr", *out_size, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, loading_get )
// Interface "Root". Method "Get" for property(s):
//  -- MODULE_LOADING
tERROR pr_call Root_PROP_loading_get( hi_Root _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
  PR_TRACE_A0( _this, "Enter *GET* method Root_PROP_loading_get for property plMODULE_LOADING" );

  if ( buffer )
    *(hPLUGIN*)buffer = g_now_loading;
  *out_size = sizeof(g_now_loading);

  PR_TRACE_A1( _this, "Leave *GET* method Root_PROP_loading_get for property plMODULE_LOADING, ret tDWORD = %u(size), error = errOK", *out_size );
  return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, loading_set )
// Interface "Root". Method "Set" for property(s):
//  -- MODULE_LOADING
tERROR pr_call Root_PROP_loading_set( hi_Root _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
  PR_TRACE_A0( _this, "Enter *SET* method Root_PROP_loading_set for property plMODULE_LOADING" );

  if ( buffer )
    g_now_loading = *(hPLUGIN*)buffer;
  *out_size = sizeof(g_now_loading);

  PR_TRACE_A1( _this, "Leave *SET* method Root_PROP_loading_set for property plMODULE_LOADING, ret tDWORD = %u(size), error = errOK", *out_size );
  return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property method implementation, set_unload_unused )
// Interface "Root". Method "Set" for property(s):
//  -- MODULE_LOADING
tERROR pr_call Root_PROP_set_unload_unused( hi_Root _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {
  
	tRootData* d;
	tBOOL      unload_before;
	PR_TRACE_A0( _this, "Enter *SET* method Root_PROP_set_unload_unused for property pgUNLOAD_UNUSED_FORCE" );

	d = _this->data;
	unload_before = d->unload_unused_plugins;
	d->unload_unused_plugins = *(tBOOL*)buffer ? cTRUE : cFALSE;
	if ( d->unload_unused_plugins ) {
		PrStartIdle();
		CALL_SYS_RegisterMsgHandler( _this, pmc_SYSTEM_IDLE_1_SECOND, rmhLISTENER, _this, IID_ANY, PID_ANY, IID_ANY, PID_ANY );
	}
	else if ( unload_before ) {
		PrStopIdle( cFALSE );
		CALL_SYS_UnregisterMsgHandler( _this, pmc_SYSTEM_IDLE_1_SECOND, _this );
	}

  *out_size = sizeof(d->unload_unused_plugins);

  PR_TRACE_A1( _this, "Leave *SET* method Root_PROP_set_unload_unused for property pgUNLOAD_UNUSED_FORCE, ret tDWORD = %u(size), error = errOK", *out_size );
  return errOK;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
tERROR pr_call Root_ObjectInit( hi_Root _this ) {
	PR_TRACE(( _this, prtERROR, "krn\tRoot object cannot be created by ObjectCreate method. You have to use PragueLoadFunction to load it !" ));
	return errROOT_OBJECT_CANNOT_BE_CREATED;
}
// AVP Prague stamp end




// ---
tERROR pr_call _ImportStringService() {
	static tBOOL imported;
	tERROR error;
	if ( imported )
		return errOK;
	if ( PR_SUCC(error=Root_ResolveImportTable((hi_Root)MakeObject(g_hRoot),0,import_string_table,PID_KERNEL)) )
		imported = cTRUE;
	return error;
}

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//  Notification that all necessary properties have been set and object must go to operation state
// Result comment
//  Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call Root_ObjectInitDone( hi_Root _this ) {
	tERROR error;
	tPO*   po;
	//tHANDLE* handle;
	PR_TRACE_A0( _this, "Enter Root::ObjectInitDone method" );
	
	enter_lf();
	error = _MemInitObject( po );
	if ( PR_SUCC(error) )
		_SyncInitObj( po );
	
	enter_lf();
	rlock(po);
	if ( PR_SUCC(error) )
		error = _ImpexInitObj( po );
	if ( PR_SUCC(error) )
		_SerializerInit();
	runlock(po);
	
	//_ImportStringService();
	
#if defined(_DEBUG) || defined(USE_DBG_INFO)
	CALL_Root_ResolveImportFunc( _this, (tFUNC_PTR*)&PrDbgGetCallerInfo, ECLSID_LOADER, FID_PrDbgGetCallerInfo, PID_KERNEL );
#endif
	
	//if ( _HC(handle,0,(hOBJECT)_this,&error) )
	//	handle->cp = cCP_ANSI;
	
	leave_lf();
	
	if ( PR_SUCC(error) )
		error = CALL_Root_ResolveImportTable( _this, 0, import_loader_table, PID_KERNEL );
	
	PR_TRACE_A1( _this, "Leave Root::ObjectInitDone method, ret t%terr", error );
	return error;
}
// AVP Prague stamp end



extern tPO g_po_array[0x10];

// ---
static tVOID pr_call _add_plugin_expt( hPLUGIN* expt, tUINT* count, hOBJECT obj ) {
  tHANDLE* tmp = MakeHandle(obj);
  if ( tmp && tmp->iface )
    expt[(*count)++] = tmp->iface->plugin;
}


// ---
static tBOOL pr_call _check_plugin_expt( hPLUGIN* expt, hPLUGIN obj ) {
  tUINT i;
  for( i=0; expt[i]; i++ ) {
    if ( expt[i] == obj )
      return cTRUE;
  }
  return cFALSE;
}



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
tERROR pr_call Root_ObjectPreClose( hi_Root _this ) {
	tERROR err;
  PR_TRACE_A0( _this, "Enter Root::ObjectPreClose" );
	if ( g_exit_now )
		err = errOK;
	else {
		err = errLOCKED;
		PR_TRACE(( _this, prtDANGER, "krn\tSomebody wants to close Root object !!!" ));
	}
  PR_TRACE_A0( _this, "Leave Root::ObjectPreClose" );
	return err;
}
// AVP Prague stamp end




// ---
static tBOOL _check_internal_plugin( tPO* po, tHANDLE* plugin ) {
	tDWORD size = 0;
	tPID   pid  = PID_ANY;
	if ( !PrIsIntegralPlugin )
		return cFALSE;
	_PropertyGetSync( po, plugin, &size, pgMODULE_ID, &pid, sizeof(pid) );
	return PrIsIntegralPlugin( pid );
}



// ---
static tERROR pr_call _CloseOneUnusedPlugin( tPO* po, tBOOL (pr_call *check)(tPO*,tHANDLE*) ) {

	tHANDLE* hdl = g_plugins;

	PR_TRACE(( g_root, prtIMPORTANT, "ldr\t_CloseOneUnusedPlugin -----------------------------------" ));

	while( hdl ) {
		tHANDLE* next = hdl->next;
		hPLUGIN  plg = (hPLUGIN)MakeObject( hdl );
		
#		if defined(_DEBUG) && defined(_PRAGUE_TRACE_)
			tDATA  size = 0;
			tUINT  ref = 0;
			tCHAR  buff[0x100] = {0};
			CALL_SYS_PropertyGetStr( plg, 0, pgOBJECT_NAME, buff, sizeof(buff), cCP_ANSI );
			_PropertyGetSync( po, hdl, &size, pgUSAGE_COUNT, &ref, sizeof(ref) );
			PR_TRACE(( g_root, prtIMPORTANT, "ldr\tTry unload - (ref=%d) %s", ref, buff ));
#   endif

		if ( (check && check(po,hdl)) || _check_plugin_exports_used(plg) ) {
			hdl = next;
			continue;
		}

		wlock(po);
		if ( hdl == g_plugins )
			g_plugins = hdl->next;
		_Remove( hdl );
		_AddToChildList( hdl, g_hRoot );
		wunlock(po);
		
		return CALL_SYS_ObjectClose( plg ); // but by the SYSTEM method because of synchronization
	}
	return errOBJECT_NOT_FOUND;
}


// ---
#if defined(_DEBUG) && defined(_PRAGUE_TRACE_)
typedef struct tag_PArray {
	tBOOL   all;
	tUINT   count;
	hPLUGIN arr[100];
} tPArray;

tVOID pr_call _enum_plugin_importers( hPLUGIN plugin, tVOID* ctx, tVOID (pr_call *call_back)(tVOID* ctx, tDWORD method_id, hPLUGIN) );

// ---
static tVOID pr_call _print_plugin_import( tVOID* ctx, tDWORD method_id, hPLUGIN plugin ) { 
	tUINT i;
	tCHAR buff[0x100] = {0};
	tPArray* pa = (tPArray*)ctx;
	tBOOL found = cFALSE;
	for( i=0; (i<pa->count) && !found; ++i )
		found = (pa->arr[i] == plugin);
	if ( !found || pa->all ) {
		if ( pa->count < countof(pa->arr) )
			pa->arr[ pa->count++ ] = plugin;
		CALL_SYS_PropertyGetStr( plugin, 0, pgOBJECT_NAME, buff, sizeof(buff), cCP_ANSI );
		if ( pa->all ) {
			PR_TRACE(( g_root, prtIMPORTANT, "ldr\t\t\t0x%08x - %s", method_id, buff ));
		}
		else {
			PR_TRACE(( g_root, prtIMPORTANT, "ldr\t\t\t%s", buff ));
		}
	}
}


static tVOID pr_call PRINT_PLUGINS( tPO* po, tBOOL verboose ) {
	tHANDLE* hdl = g_plugins;
	PR_TRACE(( g_root, prtIMPORTANT, "ldr\tPlugin list\n------------------------------------------------------------------" ));
	while( hdl ) {
		tDWORD ref;
		tDWORD size = 0;
		tCHAR buff[0x100] = {0};
		tPArray pa = {0};
		pa.all = verboose;
		CALL_SYS_PropertyGetStr( MakeObject(hdl), 0, pgOBJECT_NAME, buff, sizeof(buff), cCP_ANSI );
		_PropertyGetSync( po, hdl, &size, pgUSAGE_COUNT, &ref, sizeof(ref) );
		PR_TRACE(( g_root, prtIMPORTANT, "ldr\tPlugin - (ref=%d) %s", ref, buff ));
		if ( ref )
			_enum_plugin_importers( (hPLUGIN)MakeObject(hdl), &pa, _print_plugin_import );
		hdl = hdl->next;
	}
	PR_TRACE(( g_root, prtIMPORTANT, "ldr\t\n------------------------------------------------------------------" ));
}
#else

#define PRINT_PLUGINS( po, verboose )

#endif



// ---
static tERROR pr_call _CloseOneUnreferencedPlugin( tPO* po, tBOOL (pr_call *check)(tPO*,tHANDLE*) ) {
	
	tHANDLE* hdl = g_plugins;
	//*found_min = 0;

	PR_TRACE(( g_root, prtIMPORTANT, "ldr\t_CloseOneUnreferencedPlugin-----------------------------------" ));
	while( hdl ) {
		tHANDLE* next = hdl->next;
		tDATA    size = 0;
		tUINT    ref = 0;
		hOBJECT  obj = MakeObject( hdl );

#		if defined(_DEBUG) && defined(_PRAGUE_TRACE_)
			tCHAR buff[0x100] = {0};
			CALL_SYS_PropertyGetStr( obj, 0, pgOBJECT_NAME, buff, sizeof(buff), cCP_ANSI );
#		endif

		if ( check && check(po,hdl) ) {
			hdl = next;
			continue;
		}

		_PropertyGetSync( po, hdl, &size, pgUSAGE_COUNT, &ref, sizeof(ref) );
		//if ( *found_min > ref )
		//	*found_min = ref;

		if ( !ref /*|| (check_used && !check_used((hPLUGIN)obj)) || (ref <= min_ref)*/ ) {
			
//#     if defined(_PRAGUE_TRACE_)
//				if ( obj && (ref > 0) ) {
//#					if !defined(_DEBUG)
//						CALL_SYS_PropertyGetStr( obj, 0, pgOBJECT_NAME, buff, sizeof(buff), cCP_ANSI );
//#					endif
//					PR_TRACE(( g_root, prtERROR, "krn\tPlugin %s unloaded with %d ref count", buff, ref ));
//				}
//#			endif

			wlock(po);
			if ( hdl == g_plugins )
				g_plugins = hdl->next;
			_Remove( hdl );
			if ( obj )
				_AddToChildList( hdl, g_hRoot );
			wunlock(po);
			
			if ( obj )
				CALL_SYS_ObjectClose( obj ); // but by the SYSTEM method because of synchronization
			//*found_min = 0;
			return errOK;
		}
		
		hdl = next;
	}
	return errOBJECT_NOT_FOUND;
}



// ---
static tVOID pr_call _ClosePluginList( tPO* po ) {
	
	//tUINT  ref = 0;
	tERROR err = errOK;
	tHANDLE* hdl;

	while( PR_SUCC(err) ) {
		while( PR_SUCC(err) )
			err = _CloseOneUnreferencedPlugin( po, _check_internal_plugin );
		PRINT_PLUGINS( po, cFALSE );
		err = _CloseOneUnusedPlugin( po, _check_internal_plugin );
		PRINT_PLUGINS( po, cFALSE );
	}

	err = errOK;
	while( PR_SUCC(err) ) {
		while( PR_SUCC(err) )
			err = _CloseOneUnreferencedPlugin( po, 0 );
		PRINT_PLUGINS( po, cFALSE );
		err = _CloseOneUnusedPlugin( po, 0 );
		PRINT_PLUGINS( po, cFALSE );
	}

	hdl = g_plugins;
	while( hdl ) {
		tHANDLE* next = hdl->next;
#		if defined(_DEBUG) && defined(_PRAGUE_TRACE_)
			tCHAR buff[0x100] = {0};
			CALL_SYS_PropertyGetStr( MakeObject(hdl), 0, pgOBJECT_NAME, buff, sizeof(buff), cCP_ANSI );
#   endif

		wlock(po);
		if ( hdl == g_plugins )
			g_plugins = hdl->next;
		_Remove( hdl );
		_AddToChildList( hdl, g_hRoot );
		wunlock(po);
		
		CALL_SYS_ObjectClose( MakeObject(hdl) ); // but by the SYSTEM method because of synchronization
		hdl = next;
		PRINT_PLUGINS( po, cTRUE );
	}
}




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectClose )
// Extended method comment
//  Kernel warns object it must be closed
// Behaviour comment
//  Object takes all necessary "before closing" actions
// Parameters are:
tERROR pr_call Root_ObjectClose( hi_Root _this ) {

	tUINT      i, c;
  tSTRING*   p;
	tHANDLE*   handle;
  tHANDLE*   next;
  tHANDLE*   tmp;
	tHANDLEX*  ext;
  //TraceData* td;
  tPO*       po;
  tERROR     err;
  hPLUGIN    expt[10+countof(g_po_array)];
  tUINT      count = 0;


  PR_TRACE_A0( _this, "Enter Root::ObjectClose" );
  PR_TRACE(( _this, prtNOTIFY, "krn\tClosing root object" ));

  enter_lf();
  rlock(po);

  _HC( handle, 0, (hOBJECT)_this, &err );

//  if ( g_tracer )
//    td = CUST_TO_TraceData(g_tracer);
//  else
//    td = 0;

  for( i=0; i<g_sync_iids_count; i++ )
    g_sync_iids[i] = 0;

	_SerializerDeinit();
  _CustomPropertyDeleteList( po, g_hRoot );
  //_CloseList( po, g_statics );   // close static object list

  mset( expt, 0, sizeof(expt) );
  _add_plugin_expt( expt, &count, (hOBJECT)g_heap );
  for( i=0; (i<countof(g_po_array)) && g_po_array[i].cs; i++ )
    _add_plugin_expt( expt, &count, (hOBJECT)g_po_array[i].cs );

  next = g_plugins;
  for( next=g_plugins; next; next=tmp ) {
    tmp = next->next;
    if ( next->iface && _check_plugin_expt(expt,(hPLUGIN)MakeObject(next)) ) {
      if ( g_plugins == next )
        g_plugins = tmp;
      _Remove( next );
    }
  }

	#if defined(_DEBUG) || defined(USE_DBG_INFO)
		Root_ResolveImportFunc( _this, (tFUNC_PTR*)&PrDbgGetCallerInfo, ECLSID_LOADER, FID_PrDbgGetCallerInfo, PID_KERNEL );
	#endif
	Root_ReleaseImportTable( _this, 0, import_string_table, PID_KERNEL );
	Root_ResolveImportTable( _this, 0, import_loader_table, PID_KERNEL );

  _ClosePluginList( po );   // close plugin list

  //_DelCustomPropList( 0 );
  c = root_obj.d.named_count;
  p = root_obj.d.named_props;

  wlock(po);
  root_obj.d.named_count = 0;
  root_obj.d.named_props = 0;
  wunlock(po);

	tmp = MakeHandle( g_heap ); // string objects borne by root moved to this object
	if ( tmp ) {								// so if some of them leaved we will kill them all
		next = tmp->child;
		while( next ) {
			_ObjectClose( po, next, 0 );
			next = tmp->child;
		}
	}

  runlock(po);

	_SerializerDeinit();
  _SyncDeinitObj();
  _ImpexDeinitObj();

  if ( p ) {
    for( i=0; i<c; i++ ) {
      tSTRING s = p[i];
      PrFree( s );
    }
    PrFree( p );
  }

	tmp = MakeHandle( g_tracer );
	if ( tmp && PR_FAIL(_WrapperWithdraw(0,tmp,0,0)) )
		_set_vtbl( tmp, 0 );

  if ( PR_FAIL(_WrapperWithdraw(0,handle,0,0)) ) // remove all wrappers
		_set_vtbl( handle, 0 );
	
//	if ( td && td->cs ) {
//    hOBJECT cs = (hOBJECT)td->cs;
//    td->cs = 0;
//    CALL_SYS_ObjectClose( cs );
//	}

	ext = handle->ext;
	if ( ext ) {
		handle->ext = 0;
		if ( g_handle_ext_count )
			--g_handle_ext_count;
		if ( ext->mh_arr.parr ) {
			PrFree( ext->mh_arr.parr );
			ext->mh_arr.mcount = 0;
			ext->mh_arr.parr = 0;
		}
		if ( ext->mh_c_arr.parr ) {
			PrFree( ext->mh_c_arr.parr );
			ext->mh_c_arr.count = 0;
			ext->mh_c_arr.parr = 0;
		}
		PrFree( ext );
	}

	_InterfacePoolDeinit();
	_HandlePoolDeinit();
	_MemDeinitialize( 0 );
	_TracerDeinitObj( 0 );

	*hdata(handle) = 0;

	if ( Prot )
		Prot->Deinitialize();

	i = g_handle_count_peak;
	c = g_handle_ext_count_peak;
//	INT3;

	leave_lf();
  PR_TRACE_A0( _this, "Leave Root::ObjectClose" );
	return g_handle_count;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
// Extended method comment
//    Receives message sent to the object or to the one of the object parents as broadcast
// Parameters are:
//   "msg_cls_id"    : Message class identifier
//   "msg_id"        : Message identifier
//   "send_point"    : Object where the message was send initially
//   "ctx"           : Context of the object processing
//   "receive_point" : Point of distributing message (tree top for RegisterMsgHandler call
//   "par_buf"       : Buffer of the parameters
//   "par_buf_len"   : Lenght of the buffer of the parameters
tERROR pr_call Root_MsgReceive( hi_Root _this, tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len ) {
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter Root::MsgReceive method" );

	if ( p_msg_cls_id == pmc_SYSTEM_IDLE_1_SECOND ) {
		tDWORD period = _this->data->unuse_period;
		if ( !period )
			period = 60000;
		Root_FreeUnusedPlugins( _this, 0, 0, cFUP_SINGLE_ACTION, period );
	}

	PR_TRACE_A1( _this, "Leave Root::MsgReceive method, ret %terr", error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegisterIFace )
// Extended method comment
//   Registers new interface for using
// Parameter "internal_table_size":
//   Number of method pointers in the table.
// Parameter "external_table_size":
//   Number of method pointers in the table.
tERROR pr_call Root_RegisterIFace(
	hi_Root _this, tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid,
	const tVOID* internal_table, tDWORD internal_table_size,
	const tVOID* external_table, tDWORD external_table_size,
	const tDATA* prop_table,     tDWORD prop_table_size,
	tDWORD memsize, tDWORD flags
) {

	tIID     iface_id;
	tDWORD   iface_index;
	tERROR   error;
	hPLUGIN  implemented_module = 0;
	PR_TRACE_A0( _this, "Enter Root::RegisterIFace" );

	iface_id = IID_ANY;
	iface_index = 0;

	if ( PR_FAIL(Root_GetPluginInstance(_this,&implemented_module,0,pid)) || !implemented_module )
		implemented_module = g_now_loading;
	
	if ( implemented_module ) {
		tDWORD index = 0;
		while( 1 ) {
			tERROR err = CALL_Plugin_GetInterfaceProperty( implemented_module, 0, index, pgINTERFACE_ID, &iface_id, sizeof(iface_id) );
			if ( (err == errINTERFACE_NO_MORE_ENTRIES) || PR_FAIL(err) ) {
				iface_index = 0;
				break;
			}
			else if ( iface_id == iid ) {
				iface_index = index;
				if ( PR_FAIL(CALL_Plugin_GetInterfaceProperty(implemented_module,0,index,pgINTERFACE_COMPATIBILITY,&iface_id,sizeof(iface_id))) )
					iface_id = IID_ANY;
				break;
			}
			else
				index++;
		}
	}
	
	error = Root_RegisterIFaceEx(
		_this, 0, iid, pid, subtype, version, vid,
		internal_table, internal_table_size,
		external_table, external_table_size,
		prop_table, prop_table_size,
		memsize, flags, iface_id,
		implemented_module, iface_index
	);

	PR_TRACE_A1( _this, "Leave Root::RegisterIFace ret t%terr", error );
	return error;
}
// AVP Prague stamp end




// ---
static tERROR _replace_iface( tPO* po, tINTERFACE** iface, tIID iid, tPID pid, tVID vid, tVERSION version, hPLUGIN plugin ) {
	PR_TRACE(( MakeObject(g_hRoot), prtIMPORTANT, "krn\tReplacing interface(%tiid, %tpid, %tvid, ver=%u) by new(ver=%u) one", iid, pid, vid, (*iface)->version, version ));
  
  wlock(po);
	(*iface)->flags |= IFACE_CONDEMNED;         // drop it if version ok
	(*iface)->flags &= ~IFACE_LOADED;
  wunlock(po);

	if ( (*iface)->plugin && ((*iface)->plugin != plugin) )
		_InterfaceUnload( po, *iface );
	return _InterfaceAlloc( po, iface );
}




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegisterIFaceEx )
// Extended method comment
//  Registers new interface for using
// Parameters are:
// "internal_table_size" : Number of method pointers in the table.
// "external_table_size" : Number of method pointers in the table.
tERROR pr_call Root_RegisterIFaceEx( hi_Root _this, hIFACE* iface_res, tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid, const tVOID* internal_table, tDWORD internal_table_size, const tVOID* external_table, tDWORD external_table_size, const tDATA* prop_table, tDWORD prop_table_size, tDWORD memsize, tDWORD flags, tIID iid_compat, hPLUGIN implemented_module, tDWORD implemented_index ) {
  tINTERFACE* iface;
  tERROR error = errOK;
  tDATA  prop_maxsize = 0;
  tBOOL  code_page_implemented = cFALSE;
  tDATA* pid_user = 0;
  tPO*   po;

  PR_TRACE_A0( _this, "Enter Root::RegisterIFaceEx" );
  PR_TRACE(( _this, prtNOTIFY, "krn\tParameters are: %tiid, %tpid, subtype=%u, version=%u, %tvid", iid, pid, subtype, version, vid ));
  PR_TRACE(( _this, prtNOTIFY, "krn\t              : int_tbl=%p, int_tbl_s=%u, ext_tbl=%p, ext_tbl_s=%u", internal_table, internal_table_size, external_table, external_table_size ));
  PR_TRACE(( _this, prtNOTIFY, "krn\t              : prop_tbl=%p, prop_tbl_s=%u, mem_size=%u, flags=0x%08x", prop_table, prop_table_size, memsize, flags ));

  enter_lf();

  if ( iface_res )
    *iface_res = 0;

  if ( iid == IID_ANY || pid == PID_ANY || vid == VID_ANY /*|| external == NULL || funccount == 0*/ ) {
    PR_TRACE(( _this, prtERROR, "krn\tCannot register interface[invalid parameter](%tiid, %tpid, %tvid)", iid, pid, vid ));
    error = errPARAMETER_INVALID;
  }
  else if ( prop_table_size && (cTRUE != _PropTableCheck(prop_table,prop_table_size,&prop_maxsize,&code_page_implemented,&pid_user)) ) {
    rlock(po);
    iface = _InterfaceFind( 0, iid, pid, subtype, vid, cTRUE );
    if ( iface ) {
      wlock(po);
      iface->last_err = errPROPERTY_INVALID;
      wunlock(po);
    }
    runlock(po);
    PR_TRACE(( _this, prtERROR, "krn\tCannot register interface(%tiid, %tpid, %tvid). Bad property table", iid, pid, vid ));
    error = errPROPERTY_INVALID;
  }
  else {
		tINT just_allocated = cFALSE;

		rlock(po);

    // find apropriate entry in iface table
    error = _InterfaceCheck( &iface, iid, pid, subtype, version, vid, flags, implemented_module );

    if ( PR_FAIL(error) )
      ;
    else if ( (error == warnINTERFACE_VERSION_RECENT) || (error == warnINTERFACE_TIMESTAMT_RECENT) ) {
      if ( iface->external && (iface->external != _ObjNotInitializedFunctions) )
        error = _replace_iface( po, &iface, iid, pid, vid, version, implemented_module );
    }
		else if ( !iface && PR_SUCC(error=_InterfaceAlloc(po,&iface)) ) { /*if ( (flags & IFACE_UN_UPDATABLE) )*/ // interface mustn't be registered before
			just_allocated = cTRUE;
      flags |= IFACE_UNSWAPPABLE;
		}

    if ( iface ) {
      tCODEPAGE cp;
			tTraceLevel trace_level;

			if ( just_allocated ) {
				if ( g_tracer ) {
					tHANDLE* htr = MakeHandle(g_tracer);
					trace_level = htr->trace_level;
				}
				else {
					trace_level.min = prtMIN_TRACE_LEVEL;       // minimum trace level for this handle
					trace_level.max = prtMIN_TRACE_LEVEL;       // maximum trace level for this handle
				}
			}

      if ( !iface->plugin )
        cp = cCP_ANSI;
      else {
        tExceptionReg er;
        runlock(po);
        if ( errOK == si.EnterProtectedCode(0,&er) ) {
          if ( PR_SUCC(CALL_Plugin_GetInterfaceProperty(iface->plugin,0,iface->index,pgINTERFACE_CODEPAGE,&cp,sizeof(cp))) )
            ;
          else if ( PR_SUCC(CALL_Plugin_GetInterfaceProperty(iface->plugin,0,0,pgPLUGIN_CODEPAGE,&cp,sizeof(cp))) )
            ;
          else
            cp = cCP_ANSI;
        }
        si.LeaveProtectedCode( 0, &er );
        rlock(po);
      }

      wlock(po);
      if ( !(iface->flags & IFACE_LOADED) ) {
        if ( external_table || internal_table )
          flags |= IFACE_LOADED;

        if ( external_table == NULL ) {
          external_table = _ObjNotInitializedFunctions;
          external_table_size = sizeof(_ObjNotInitializedFunctions)/sizeof(_ObjNotInitializedFunctions[0]);
        }

        if ( internal_table == NULL ) {
          internal_table = &INTERNAL_NULL_TABLE;
          internal_table_size = sizeof(INTERNAL_NULL_TABLE)/sizeof(tPTR);
        }

        // fill in information
        iface->iid             = iid;                        // interface type
        iface->pid             = pid;                        // plugin ID
        iface->subtype         = subtype;                    // subtype of interface in plugin
        iface->version         = version;                    // version
        iface->vid             = vid;                        // vendor id
        iface->internal        = (iINTERNAL*)internal_table; // pointer for internal access functions
        iface->in_count        = internal_table_size;        // counter of internal access functions
        iface->external        = (tPTR)external_table;       // exported interface functions
        iface->ex_count        = external_table_size;        // function count in table above
        iface->proptable       = (tDATA*)prop_table;         // properties table
        iface->propcount       = prop_table_size;            // properties count maintained by kernel
        iface->propmaxsize     = prop_maxsize;               // required size for properties
        iface->memsize         = memsize;                    // required size for local data (include all properties)
        iface->flags           = flags;                      // interface flags
        //iface->obj_count       = 0;                          // count of objects interface creats at the moment
        iface->compat          = iid_compat;
        iface->last_err        = errOK;
        iface->pid_user        = pid_user;
      }

      if ( code_page_implemented )
        iface->flags |= IFACE_OWN_CP;

			if ( just_allocated )
				iface->trace_level = trace_level;

      if ( !iface->plugin && implemented_module ) {
        iface->plugin = implemented_module;
        iface->index = implemented_index;
      }

      iface->cp = cp;
      wunlock(po);

      if ( (IID_ANY == iface->compat) && PR_SUCC(_InterfaceGetProp(po,iface,0,pgINTERFACE_COMPATIBILITY,&iid_compat,sizeof(iid_compat))) && (iid_compat != IID_ANY) ) {
        wlock(po);
        iface->compat = iid_compat;
        wunlock(po);
      }

      if ( iface->external && (_ObjNotInitializedFunctions != iface->external) ) {
        if ( iid == IID_HEAP )
          _MemInitObject( po );
        else if ( pid == PID_STRING )
          _ImportStringService();
      }
    }
    else if ( pid == PID_KERNEL )
      ;
    else if ( error == errOK )
      error = errINTERFACE_NOT_FOUND;

    runlock(po);
  }

  leave_lf();
  PR_TRACE_A1( _this, "Leave Root::RegisterIFaceEx ret t%terr", error );
  return error;
}
// AVP Prague stamp end



/*
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegisterExportTable )
// Parameters are:
tERROR pr_call Root_RegisterExportTable( hi_Root _this, tDWORD* result, const tEXPORT* p_ex_tbl, tPID p_exporter_pid ) {
	tERROR error = errNOT_IMPLEMENTED;
	tDWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter Root::RegisterExportTable method" );

	// Place your code here

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave Root::RegisterExportTable method, ret tDWORD = %u, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegisterExportFunc )
// Parameters are:
tERROR pr_call Root_RegisterExportFunc( hi_Root _this, const tFUNC_PTR p_func_ptr, tDWORD p_func_cls, tDWORD p_func_id, tPID p_exporter_pid ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::RegisterExportFunc method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::RegisterExportFunc method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



	
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegisterExportFuncByName )
// Parameters are:
tERROR pr_call Root_RegisterExportFuncByName( hi_Root _this, const tFUNC_PTR p_func_ptr, tDWORD p_func_cls, tSTRING p_func_name, tPID p_exporter_pid ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::RegisterExportFuncByName method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::RegisterExportFuncByName method, ret %terr", error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ResolveImportTable )
// Parameters are:
tERROR pr_call Root_ResolveImportTable( hi_Root _this, tDWORD* result, const tIMPORT* p_imp_tbl, tPID p_importer_pid ) {
	tERROR error = errNOT_IMPLEMENTED;
	tDWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter Root::ResolveImportTable method" );

	// Place your code here

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave Root::ResolveImportTable method, ret tDWORD = %u, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ResolveImportFunc )
// Parameters are:
tERROR pr_call Root_ResolveImportFunc( hi_Root _this, tFUNC_PTR* result, tDWORD p_func_cls, tDWORD p_func_id, tPID p_importer_pid ) {
	tERROR    error = errNOT_IMPLEMENTED;
	tFUNC_PTR ret_val = 0;
	PR_TRACE_A0( _this, "Enter Root::ResolveImportFunc method" );

	// Place your code here

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave Root::ResolveImportFunc method, ret tFUNC_PTR = %p, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ResolveImportFuncByName )
// Parameters are:
tERROR pr_call Root_ResolveImportFuncByName( hi_Root _this, tFUNC_PTR* result, tDWORD p_func_cls, const tSTRING p_func_name, tPID p_importer_pid ) {
	tERROR    error = errNOT_IMPLEMENTED;
	tFUNC_PTR ret_val = 0;
	PR_TRACE_A0( _this, "Enter Root::ResolveImportFuncByName method" );

	// Place your code here

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave Root::ResolveImportFuncByName method, ret tFUNC_PTR = %p, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ReleaseImportTable )
// Parameters are:
tERROR pr_call Root_ReleaseImportTable( hi_Root _this, tDWORD* result, const tIMPORT* p_imp_tbl, tPID p_importer_pid ) {
	tERROR error = errNOT_IMPLEMENTED;
	tDWORD ret_val = 0;
	PR_TRACE_A0( _this, "Enter Root::ReleaseImportTable method" );

	// Place your code here

	if ( result )
		*result = ret_val;
	PR_TRACE_A2( _this, "Leave Root::ReleaseImportTable method, ret tDWORD = %u, %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ReleaseImportFunc )
// Parameters are:
tERROR pr_call Root_ReleaseImportFunc( hi_Root _this, tDWORD p_func_cls, tDWORD p_func_id, tPID p_importer_pid ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::ReleaseImportFunc method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::ReleaseImportFunc method, ret %terr", error );
	return error;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, ReleaseImportFuncByName )
// Parameters are:
tERROR pr_call Root_ReleaseImportFuncByName( hi_Root _this, tDWORD p_func_cls, const tSTRING p_func_name, tPID p_importer_pid ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::ReleaseImportFuncByName method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::ReleaseImportFuncByName method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, PluginUnloadNotification )
// Parameters are:
tERROR pr_call Root_PluginUnloadNotification( hi_Root _this, hPLUGIN p_plugin_unloaded ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::PluginUnloadNotification method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::PluginUnloadNotification method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, StreamSerializeField )
// Parameters are:
tERROR pr_call Root_StreamSerializeField( hi_Root _this, const cSerializable* p_container, const tVOID* p_field, cOutStream* p_stream, tDWORD p_reserved ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::StreamSerializeField method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::StreamSerializeField method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, StreamDeserializeField )
// Parameters are:
tERROR pr_call Root_StreamDeserializeField( hi_Root _this, tVOID* p_field, const cSerDescriptorField* p_descriptor, const cInStream* p_stream, tDWORD p_reserved ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::StreamDeserializeField method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::StreamDeserializeField method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegSerializeField )
// Parameters are:
tERROR pr_call Root_RegSerializeField( hi_Root _this, const cSerializable* p_container, const tVOID* p_field, hREGISTRY p_registry, const tCHAR* p_path, tDWORD p_reserved ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::RegSerializeField method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::RegSerializeField method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegDeserializeField )
// Parameters are:
tERROR pr_call Root_RegDeserializeField( hi_Root _this, const cSerializable* p_container, tVOID* p_field, hREGISTRY p_registry, const tCHAR* p_path, tDWORD p_reserved ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::RegDeserializeField method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::RegDeserializeField method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegSerializeEx )
// Parameters are:
tERROR pr_call Root_RegSerializeEx( hi_Root _this, const cSerializable* p_object, tDWORD p_ser_id, hREGISTRY p_registry, const tCHAR* p_path, tSerializeCallback p_callback_func, tPTR p_callback_params ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::RegSerializeEx method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::RegSerializeEx method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegDeserializeEx )
// Parameters are:
tERROR pr_call Root_RegDeserializeEx( hi_Root _this, cSerializable** p_object, hREGISTRY p_registry, const tCHAR* p_path, tDWORD p_uniqueId, tSerializeCallback p_callback_func, tPTR p_callback_params ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::RegDeserializeEx method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::RegDeserializeEx method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, CompareSerializable )
// Parameters are:
tERROR pr_call Root_CompareSerializable( hi_Root _this, const cSerializable* p_str1, const cSerializable* p_str2, tDWORD ser_id, tDWORD p_flags ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::CompareSerializable method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::CompareSerializable method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, FindSerializableDescriptorByName )
// Parameters are:
tERROR pr_call Root_FindSerializableDescriptorByName( hi_Root _this, cSerDescriptor** p_descriptor, const tCHAR* p_name ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::FindSerializableDescriptorByName method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::FindSerializableDescriptorByName method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, AddRefSerializable )
// Parameters are:
tERROR pr_call Root_AddRefSerializable( hi_Root _this, cSerializable* p_object ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::AddRefSerializable method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::AddRefSerializable method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



*/

#define I_AM_GLAD            (ROOT_KERNEL_SET|ROOT_LOADER_SET|ROOT_APPLICATION_SET|ROOT_STRING_SET)


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, PluginLoadNotification )
// Parameters are:
tERROR pr_call Root_PluginLoadNotification( hi_Root _this, hPLUGIN plugin_loaded ) {
	tRootData* d;
  tPO* po;
  PR_TRACE_A0( _this, "Enter Root::PluginLoadNotification method" );
  enter_lf();

  rlock(po);

	d = _this->data;
	if ( (d->load_flags & I_AM_GLAD) != I_AM_GLAD ) {
		tDWORD flag;
		tPID   pid = CALL_SYS_PropertyGetDWord( plugin_loaded, pgMODULE_ID );
		switch( pid ) {
			case PID_KERNEL :
				_ImpexInitObj( po );
				flag = ROOT_KERNEL_SET;
				break;
			case PID_LOADER      :
				flag = ROOT_LOADER_SET;
				break;
			case PID_APPLICATION :
				d->load_flags |= ROOT_APPLICATION_SET;
				flag = ROOT_APPLICATION_SET;
				break;
			case PID_STRING      :
				if ( PR_SUCC(_ImportStringService()) )
					d->load_flags |= ROOT_STRING_SET;
				flag = 0; /*ROOT_STRING_SET;*/
				break;
			default              : flag = 0; break;
		}
		if ( flag && !(d->load_flags & flag) ) {
			tDWORD cookie = 0;
			tINTERFACE* iface;
			while( 0 != (iface = _InterfaceFind(&cookie,IID_ANY,pid,SUBTYPE_ANY,VID_ANY,cTRUE)) ) {
				d->load_flags |= flag;
				iface->plugin = plugin_loaded;
			}
		}
	}
  runlock(po);
	
	leave_lf();
	PR_TRACE_A0( _this, "Leave Root::PluginLoadNotification method, ret tERROR = errOK" );
	return errOK;
}
// AVP Prague stamp end




// ---
static tBOOL pr_call _InterfaceFindByPid( tINTERFACE* iface, tPTR pid ) {
	if ( (_toi(iface->pid) == _toi(pid)) && (iface->flags & IFACE_LOADED) )
		return cTRUE;
	return cFALSE;
}



// ---
tERROR _add_iface_to_local_cache( tINTERFACE* iface, tINTERFACE*** iface_table, tUINT* count ) {
	tERROR       err = errOK;
	tINTERFACE** liface = 0;
	if ( *count && !((*count) % LOCAL_IFACE_CLUSTER) ) {
		if ( *count == LOCAL_IFACE_CLUSTER ) {
			if ( PR_FAIL(err=PrAlloc((tPTR*)&liface,2*LOCAL_IFACE_CLUSTER*sizeof(tINTERFACE*))) )
				return err;
			mcpy( liface, *iface_table, LOCAL_IFACE_CLUSTER*sizeof(tINTERFACE*) );
		}
		else if ( PR_FAIL(err=PrRealloc((tPTR*)&liface,*iface_table,(((*count)/LOCAL_IFACE_CLUSTER)+1)*LOCAL_IFACE_CLUSTER*sizeof(tINTERFACE*))) )
			return err;
		*iface_table = liface;
	}
	(*iface_table)[(*count)++] = iface;
	return err;
}




// ---
static tERROR _add_plugin_to_cache( hPLUGIN plugin, hPLUGIN** array, tUINT* count, tUINT cluster_size ) {
	if ( *count && !((*count) % cluster_size) ) {
		tERROR err;
		hPLUGIN* tmp;
		if ( *count == cluster_size ) {
			if ( PR_FAIL(err=PrAlloc((tPTR*)&tmp,2*cluster_size*sizeof(hPLUGIN*))) )
				return err;
			mcpy( tmp, *array, cluster_size*sizeof(hPLUGIN*) );
		}
		else if ( PR_FAIL(err=PrRealloc((tPTR*)&tmp,*array,(((*count)/cluster_size)+1)*cluster_size*sizeof(hPLUGIN*))) )
			return err;
		*array = tmp;
	}
	(*array)[(*count)++] = plugin;
	return errOK;
}


// ---
typedef struct tag_cPluginArray {
	hPLUGIN  array[20];
	hPLUGIN* ptr;
	tUINT    count;
} cPluginArray;


// ---
static tBOOL pr_call _make_plugin_list( tINTERFACE* iface, tPTR params ) {
	if ( iface->plugin /*&& !iface->obj_count*/ ) {
		tINT i;
		cPluginArray* arr = (cPluginArray*)params;
		for( i=arr->count-1; i>=0; --i ) {
			if ( arr->ptr[i] == iface->plugin )
				return cFALSE;
		}
		_add_plugin_to_cache( iface->plugin, &arr->ptr, &arr->count, countof(arr->array) );
	}
	return cFALSE;
}


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, FreeUnusedPlugins )
// Parameters are:
tERROR pr_call Root_FreeUnusedPlugins( hi_Root _this, tPID* pid_array, tDWORD pid_array_count, tFUP_ACTION action, tDWORD p_during ) {
	tUINT        i, c;
	tERROR       error = errOK;
  tPO*         po;
	cPluginArray plugins = { {0}, plugins.array, 0 };

  PR_TRACE_A0( _this, "Enter Root::FreeUnusedPlugins method" );
  enter_lf();

	PR_TRACE(( _this, prtIMPORTANT, "krn\tFreeUnusedPlugins must be reconstructed!" ));
	
	if ( !pid_array ) {
		rlock(po);
		_InterfaceFindInt( 0, _make_plugin_list, &plugins );    // find any alive iface implemented by the plugin
		runlock(po);
	}
	else {
		for( i=0,c=pid_array_count; i<c; i++ ) {
			if ( PID_ANY != pid_array[i] ) {
				tINTERFACE* iface;
				tDWORD cookie = 0;
				rlock(po);
				iface = _InterfaceFindInt( &cookie, _InterfaceFindByPid, _toptr(pid_array[i]) );    // find any alive iface implemented by the plugin
				if ( iface )
					_add_plugin_to_cache( iface->plugin, &plugins.ptr, &plugins.count, countof(plugins.array) );
				runlock(po);
			}
		}
	}

	for ( i=0; i<plugins.count; ++i )
		CALL_Plugin_UnloadUnused( plugins.ptr[i], p_during );
	if ( plugins.ptr != plugins.array )
		PrFree( plugins.ptr );

	leave_lf();
  PR_TRACE_A1( _this, "Leave Root::FreeUnusedPlugins method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetNotificationSink )
// Parameters are:
tERROR pr_call Root_SetNotificationSink( hi_Root _this, tDWORD* id, tSNS_ACTION type, tFUNC_PTR func, tPTR ctx ) {
	tERROR error;
  tPO*   po;

  PR_TRACE_A0( _this, "Enter Root::SetNotificationSink method" );
  enter_lf();

  error = _ns_add( po, type, id, func, ctx );

	leave_lf();
  PR_TRACE_A1( _this, "Leave Root::SetNotificationSink method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, FreeNotificationSink )
// Parameters are:
tERROR pr_call Root_FreeNotificationSink( hi_Root _this, tDWORD id ) {
	tERROR error;
  tPO*   po;

  PR_TRACE_A0( _this, "Enter Root::FreeNotificationSink method" );
  enter_lf();

  error = _ns_del( po, id );

	leave_lf();
  PR_TRACE_A1( _this, "Leave Root::FreeNotificationSink method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, CheckIFace )
// Extended method comment
//    //rus:позволяет проверить/зарегистрировать список интерфейсов с поддержкой транзакций
//
// Parameters are:
//   "flags"              : //rus:набор констант "IFACE_..." объединенных операцией "ИЛИ". Константы описаны в файле "i_ifenum.h"
//                          //eng:set of "IFACE_..." constants combained by "OR" operation. Constants are defined in "i_ifenum.h" header file
tERROR pr_call Root_CheckIFace( hi_Root _this, tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid, tDWORD flags, hPLUGIN implemented_module ) {
  tERROR error;
  tPO*   po;

  PR_TRACE_A0( _this, "Enter Root::CheckIFace method" );
  enter_lf();

  rlock(po);
  error = _InterfaceCheck( 0, iid, pid, subtype, version, vid, flags, implemented_module );
  runlock(po);

  leave_lf();
  PR_TRACE_A1( _this, "Leave Root::CheckIFace method, ret %terr", error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, SetKernelSyncObject )
// Parameters are:
tERROR pr_call Root_SetKernelSyncObject( hi_Root _this, tIID p_iid, tDWORD p_pid, tDWORD p_subtype ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::SetKernelSyncObject method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::SetKernelSyncObject method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetKernelSyncObject )
// Parameters are:
tERROR pr_call Root_GetKernelSyncObject( hi_Root _this, hOBJECT* p_sync_object ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::GetKernelSyncObject method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::GetKernelSyncObject method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



/*
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegisterSerializableDescriptor )
// Parameters are:
tERROR pr_call Root_RegisterSerializableDescriptor( hi_Root _this, const cSerDescriptor* p_descriptor ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::RegisterSerializableDescriptor method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::RegisterSerializableDescriptor method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, UnregisterSerializableDescriptor )
// Parameters are:
tERROR pr_call Root_UnregisterSerializableDescriptor( hi_Root _this, const cSerDescriptor* p_descriptor ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::UnregisterSerializableDescriptor method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::UnregisterSerializableDescriptor method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, FindSerializableDescriptor )
// Parameters are:
tERROR pr_call Root_FindSerializableDescriptor( hi_Root _this, cSerDescriptor** p_descriptor, tDWORD p_uniqueId ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::FindSerializableDescriptor method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::FindSerializableDescriptor method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, CreateSerializable )
// Parameters are:
tERROR pr_call Root_CreateSerializable( hi_Root _this, tDWORD p_unique_id, cSerializable** p_object ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::CreateSerializable method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::CreateSerializable method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, DestroySerializable )
// Parameters are:
tERROR pr_call Root_DestroySerializable( hi_Root _this, cSerializable* p_object ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::DestroySerializable method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::DestroySerializable method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, CopySerializable )
// Parameters are:
tERROR pr_call Root_CopySerializable( hi_Root _this, cSerializable** p_dst, const cSerializable* p_src ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::CopySerializable method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::CopySerializable method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, StreamSerialize )
// Parameters are:
tERROR pr_call Root_StreamSerialize( hi_Root _this, const cSerializable* p_object, tDWORD p_ser_id, tBYTE* p_buffer, tDWORD p_size, tDWORD* p_output_size, tDWORD p_flags ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::StreamSerialize method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::StreamSerialize method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, StreamDeserialize )
// Parameters are:
tERROR pr_call Root_StreamDeserialize( hi_Root _this, cSerializable** p_object, const tBYTE* p_buffer, tDWORD p_size, tDWORD* p_accepted_size ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::StreamDeserialize method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::StreamDeserialize method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegSerialize )
// Parameters are:
tERROR pr_call Root_RegSerialize( hi_Root _this, const cSerializable* p_object, tDWORD p_ser_id, hREGISTRY p_registry, const tCHAR* p_path ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::RegSerialize method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::RegSerialize method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegDeserialize )
// Parameters are:
tERROR pr_call Root_RegDeserialize( hi_Root _this, cSerializable** p_object, hREGISTRY p_registry, const tCHAR* p_path, tDWORD p_uniqueId ) {
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Root::RegDeserialize method" );

	// Place your code here

	PR_TRACE_A1( _this, "Leave Root::RegDeserialize method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



*/
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, LockKernel )
// Parameters are:
tERROR pr_call Root_LockKernel( hi_Root _this ) {
	tERROR err;
  tPO*   po;
	PR_TRACE_A0( _this, "Enter Root::LockKernel method" );
  enter_lf();
  if ( po && po->cs ) {
		++po->usage;
		err = po->cs->vtbl->Enter( po->cs, SHARE_LEVEL_WRITE );
	}
	else
		err = errOK;
	leave_lf();
  PR_TRACE_A1( _this, "Leave Root::LockKernel method, ret %terr", err );
	return err;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, UnlockKernel )
// Parameters are:
tERROR pr_call Root_UnlockKernel( hi_Root _this ) {
	tERROR err;
  tPO*   po;
	PR_TRACE_A0( _this, "Enter Root::UnlockKernel method" );
  enter_lf();
  if ( po && po->cs ) {
		--po->usage;
		err = po->cs->vtbl->Leave( po->cs, 0 );
	}
	else
		err = errOK;
	leave_lf();
  PR_TRACE_A1( _this, "Leave Root::UnlockKernel method, ret %terr", err );
	return err;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, LoadModule )
// Parameters are:
tERROR pr_call Root_LoadModule( hi_Root _this, hPLUGIN* p_plugin, const tVOID* p_module_name, tDWORD name_size, tCODEPAGE p_cp ) {
	tPO*     po;
	tHANDLE* handle;
	hPLUGIN  plugin = 0;
	hSTRING  pname;
	tERROR   error;
	PR_TRACE_A0( _this, "Enter Root::LoadModule method" );

	enter_lf();

	if ( PR_SUCC(error=CALL_SYS_ObjectCreateQuick(g_root,(hOBJECT*)&pname,IID_STRING,PID_ANY,SUBTYPE_ANY)) ) {
		if ( CmpStr && PR_SUCC(CmpStr(p_module_name,name_size,p_cp,"prstring.ppl",sizeof("prstring.ppl"),cCP_ANSI,fSTRING_COMPARE_CASE_INSENSITIVE)) ) {
			p_module_name = "prloader.dll";
			name_size = sizeof("prloader.dll");
			p_cp = cCP_ANSI;
		}
		rlock(po);
		for( handle=g_plugins; handle; handle=handle->next ) {
			tSTR_RANGE range;
			if ( PR_FAIL(CALL_String_ImportFromProp(pname,0,MakeObject(handle),pgOBJECT_NAME)) )
				continue;
			if ( PR_SUCC(CALL_String_FindOneOfBuff(pname,&range,cSTRING_WHOLE,"\\",1,cCP_ANSI,fSTRING_COMPARE_SLASH_INSENSITIVE|fSTRING_FIND_BACKWARD)) )
				range = STR_RANGE(STR_RANGE_POS(range)+1,cSTRING_WHOLE);
			else
				range = cSTRING_WHOLE;
			if ( PR_SUCC(CALL_String_CompareBuff(pname,range,(tVOID*)p_module_name,name_size,p_cp,fSTRING_COMPARE_CASE_INSENSITIVE)) ) {
				break;
			}
		}
		plugin = (hPLUGIN)MakeObject(handle);
		if ( plugin )
			CALL_SYS_ValidateLock( _this, (hOBJECT)plugin, IID_ANY, PID_ANY );
		runlock(po);
		if ( plugin ) {
			error = CALL_Plugin_Load( plugin );
			CALL_SYS_Release( plugin );
		}
		CALL_SYS_ObjectClose( pname );
	}

	if ( plugin )
		;
	else if ( PR_FAIL(error=CALL_SYS_ObjectCreate(g_root,(hOBJECT)&plugin,IID_PLUGIN,PID_ANY,SUBTYPE_ANY)) )
		;
	else {
		handle = MakeHandle( plugin );

    wlock(po);
    _Remove( handle );
		handle->parent = 0;
		if ( g_plugins )
			_AddBefore( handle, g_plugins );
		g_plugins = handle;
    wunlock(po);

		if ( PR_FAIL(error=CALL_SYS_PropertySetStr(plugin,0,pgMODULE_NAME,(const tPTR)p_module_name,name_size,p_cp)) )
			;
		else if ( PR_FAIL(error=CALL_SYS_ObjectCreateDone(plugin)) )
			;
		else
			error = CALL_Plugin_Load( plugin );
	}

	leave_lf();

	if ( PR_FAIL(error) && plugin ) {
		CALL_SYS_ObjectClose( plugin );
		plugin = 0;
	}

	if ( p_plugin )
		*p_plugin = plugin;

	PR_TRACE_A1( _this, "Leave Root::LoadModule method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, UnregisterIFace )
// Extended method comment
//  Unregisters an interface
// Parameters are:
tERROR pr_call Root_UnregisterIFace( hi_Root _this, tIID iid, tPID pid, tDWORD subtype, tVID vid ) {
	tINTERFACE*  iface;
	tDWORD       cookie = 0;
  tINTERFACE*  local_iface_table[LOCAL_IFACE_CLUSTER];
  tINTERFACE** pitable;
  tUINT        iface_count;
  tUINT        i;
  tPO*         po;

	PR_TRACE_A0( _this, "Enter Root::UnregisterIFace method" );
  enter_lf();

  pitable = local_iface_table;
	iface_count = 0;

  rlock(po);
	iface = _InterfaceFind( &cookie, iid, pid, subtype, vid, cTRUE );
	while( iface ) {
    _add_iface_to_local_cache(iface,&pitable,&iface_count);
    iface = _InterfaceFind( &cookie, iid, pid, subtype, vid, cTRUE );
  }

  wlock(po);
  for( i=0; i<iface_count; i++ ) {
    pitable[i]->flags &= ~IFACE_LOADED;
    pitable[i]->flags |= IFACE_CONDEMNED;
	}
  wunlock(po);

  for( i=0; i<iface_count; i++ ) {
    //if ( !pitable[i]->obj_count ) {
      _InterfaceUnload( po, pitable[i] );
      _InterfaceFree( po, pitable[i] );
      _Interface_0_Init( po, pitable[i] );
    //}
  }
  runlock(po);

  if ( pitable != local_iface_table )
    PrFree( pitable );

	leave_lf();
  PR_TRACE_A0( _this, "Leave Root::UnregisterIFace method, ret tERROR = errOK" );
	return errOK;
}
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, FindPlugins )
// Extended method comment
//  forces current loader to create module objects
//    parameter must be understandable to loader
//    parameter says where or/and which modules loader must process
// Parameters are:
// "param_pool"      : pool of parameters to force loader to create module objects
// "param_pool_size" : size of parameter pool
tERROR pr_call Root_FindPlugins( hi_Root _this, tPID mfinder_pid, const tVOID* param_pool, tDWORD param_pool_size, tDWORD load_flags ) {

	tERROR  error;
	hLOADER loader = 0;
		
	PR_TRACE_A0( _this, "Enter Root::FindModules" );

  if ( mfinder_pid == PID_ANY )
    mfinder_pid = PID_LOADER;

	if ( PR_FAIL(error=CALL_SYS_ObjectCreateQuick(_this,&loader,IID_LOADER,mfinder_pid,0)) ) {
		PR_TRACE(( _this, prtERROR, "krn\tcannot create plugin loader [%terr]", error ));
	}
	else if ( PR_FAIL(error=CALL_Loader_IntegrateModules(loader,param_pool,param_pool_size,load_flags)) ) {
		PR_TRACE(( _this, prtERROR, "krn\terror during integrate modules [%terr]", error ));
	}

  if ( loader )
		CALL_SYS_ObjectClose( loader );

	PR_TRACE_A1( _this, "Leave Root::FindModules ret t%terr", error );
	return error;
}
// AVP Prague stamp end



// ---
static tERROR pr_call _AddCustom( tPO* po, tRootData* data, tSTRING name, tUINT len, tUINT* ind ) {
	tERROR error;
  tPTR   str;

	if ( !data->named_count ) {
		if ( PR_FAIL(error=PrAlloc((tPTR*)&data->named_props,NAMED_CLUSTER*sizeof(tSTRING*))) )
			return error;
	}
	else if ( !(data->named_count % NAMED_CLUSTER) ) {
		if ( PR_FAIL(error=PrRealloc((tPTR*)&data->named_props,data->named_props,(data->named_count + NAMED_CLUSTER)*sizeof(tSTRING*))) )
			return error;
	}

	if ( PR_FAIL(error=PrAlloc(&str,len)) )
		return error;
  mcpy( str, name, len );
  
  wlock(po);
  data->named_props[*ind=data->named_count++] = str;
  wunlock(po);

	return errOK;
		
}




// ---
static tERROR pr_call _DelCustom( tPO* po, tRootData* data, tUINT ind ) {
	tERROR error = errOK;
  tPTR   to_free = 0;

  wlock(po);
  --data->named_count;

	if ( 0 == data->named_count ) {
    to_free = data->named_props;
		data->named_props = 0;
	}
	else {
		to_free = *( data->named_props + ind );
		if ( data->named_count-ind )
			mmove( data->named_props+ind, data->named_props+ind+1, (data->named_count-ind) * sizeof(tSTRING) );
		if ( (data->named_count % NAMED_CLUSTER) || PR_FAIL(error=PrRealloc((tPTR*)&data->named_props,data->named_props,data->named_count*sizeof(tSTRING*))) )
			*(data->named_props + data->named_count) = 0;
	}
  wunlock(po);
  
  if ( to_free )
		PrFree( to_free );

  return error;
}




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, RegisterCustomPropId )
// Extended method comment
//  The method registers and allocate new unique property identifier. The identifier can be used as propid parametert of mPROPERTY_MAKE_CUSTOM macro definition, i.e. the identifier can be used more than once but for different property types.
// Parameters are:
// "name" : name of the new property identifier
// "type" : type of new identifier. Returns just numeric id if type is pTYPE_NOTHING, and full id otherwise
tERROR pr_call Root_RegisterCustomPropId( hi_Root _this, tDWORD* result, tSTRING name, tDWORD type ) {

	tDWORD ret_val = 0;
	tERROR error = errOK;
  tPO*     po;

  PR_TRACE_A0( _this, "Enter Root::RegisterCustomPropId" );
  enter_lf();

	if ( (type != pTYPE_NOTHING) && !(type & pTYPE_MASK) ) {
		type <<= pTYPE_SHIFT;
		if ( !(type & pTYPE_MASK) )
			error = errPARAMETER_INVALID;
	}

	if ( PR_FAIL(error) )
		;
	else if ( !name || !*name )
		error = errPARAMETER_INVALID;
	else {
		tUINT      i;
		tDATA      l1 = slen( name );
		tRootData* data;
		tSTRING    named = 0;

    rlock(po);
    data = _this->data;
    for( i=0; i<data->named_count; i++ ) {
			tSTRING  curr = data->named_props[i];
			tDATA l2 = slen( curr );
			if ( (l1 == l2) && !mcmp(name,curr,l1) ) {
				named = curr;
				break;
			}
		}
		
		if ( named || PR_SUCC(error=_AddCustom(po,data,name,(tUINT)(l1+1),&i)) ) {
			if ( type != pTYPE_NOTHING )
				ret_val = mPROPERTY_MAKE_CUSTOM( type, cPROPERTY_NAMED_CUSTOM + i );
			else
				ret_val = cPROPERTY_NAMED_CUSTOM + i;
		}
    runlock(po);
	}

	if ( result )
    *result = ret_val;

  leave_lf();
  PR_TRACE_A2( _this, "Leave Root::RegisterCustomPropId method, ret tDWORD = %u, %terr", ret_val, error );
  return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, UnregisterCustomPropId )
// Extended method comment
//  The method unregisters and deallocate unique property identifier.
// Parameters are:
// "name" : name of the new property identifier
tERROR pr_call Root_UnregisterCustomPropId( hi_Root _this, tSTRING name ) {
  tERROR error;
  tPO*   po;
  PR_TRACE_A0( _this, "Enter Root::UnregisterCustomPropId" );
  enter_lf();

	if ( name && *name ) {
		tUINT      i;
		tDATA      l1 = slen( name );
		tRootData* data;

		error = errOBJECT_NOT_FOUND;
		
    rlock(po);
    data = _this->data;
    for( i=0; i<data->named_count; i++ ) {
			tSTRING  curr = data->named_props[i];
			tDATA l2 = slen( curr );
			if ( (l1 == l2) && !mcmp(name,curr,l1) ) {
				error = _DelCustom( po, data, i );
				break;
			}
		}
    runlock(po);
	}
	else
		error = errPARAMETER_INVALID;

	leave_lf();
  PR_TRACE_A1( _this, "Leave Root::UnregisterCustomPropId ret t%terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetCustomPropId )
// Extended method comment
//  Returns id of the named property identifier
// Behaviour comment
//  returns 0 if name not present
// Parameters are:
// "name" : name of the property identifier to get
tERROR pr_call Root_GetCustomPropId( hi_Root _this, tDWORD* result, tSTRING name, tDWORD type ) {
	tERROR error;
	tDWORD ret_val = 0;
  tPO*   po;
  PR_TRACE_A0( _this, "Enter Root::GetCustomPropId" );
  enter_lf();

	if ( name && *name ) {
		tUINT      i;
		tDATA      l1 = slen( name );
		tRootData* data;
		
		error = errOBJECT_NOT_FOUND;
		
    rlock(po);
    data = _this->data;
    for( i=0; i<data->named_count; i++ ) {
			tSTRING curr = data->named_props[i];
			tDATA l2 = slen( curr );
			if ( (l1 == l2) && !mcmp(name,curr,l1) ) {
				error = errOK;
				if ( type != pTYPE_NOTHING )
					ret_val = mPROPERTY_MAKE_CUSTOM( type, cPROPERTY_NAMED_CUSTOM + i );
				else
					ret_val = cPROPERTY_NAMED_CUSTOM + i;
				break;
			}
		}
    runlock(po);
	}
	else
		error = errPARAMETER_INVALID;
		
	if ( result )
		*result = ret_val;

	leave_lf();
  PR_TRACE_A2( _this, "Leave Root::GetCustomPropId ret tDWORD = %u (id), %terr", ret_val, error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetCustomPropName )
// Parameters are:
tERROR pr_call Root_GetCustomPropName( hi_Root _this, tDWORD* p_out_size, tPROPID p_prop_id, tPTR p_buffer, tDWORD p_size ) {
	tRootData* d;
  tPO*       po;
	tERROR     error = errOK;
	tDATA      len = 0;
	PR_TRACE_A0( _this, "Enter Root::GetCustomPropName method" );
	
	if ( p_buffer && p_size )
		*(tCHAR*)p_buffer = 0;

  enter_lf();
  rlock(po);

	p_prop_id &= pNUMBER_MASK;
	p_prop_id -= cPROPERTY_NAMED_CUSTOM;

  d = _this->data;
	if ( p_prop_id >= d->named_count )
		error = errPROPERTY_NOT_FOUND;
	else {
		tSTRING curr = d->named_props[p_prop_id];
		len = slen(curr) + 1;
		if ( p_buffer ) {
			if ( p_size >= len )
				mcpy( p_buffer, curr, len );
			else {
				if ( p_size ) {
					mcpy( p_buffer, curr, p_size-1 );
					*(((tCHAR*)p_buffer) + p_size - 1) = 0;
				}
				error = errBUFFER_TOO_SMALL;
			}
		}
	}

	if ( p_out_size )
		*p_out_size = (tDWORD)len;

  runlock(po);
	leave_lf();
	
	PR_TRACE_A1( _this, "Leave Root::GetCustomPropName method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, TraceLevelSet )
// Extended method comment
//  set trace level for all new or/and existing objects of specified interface or/and plugin
// Parameters are:
// "op"        : who is affected by this method. Possible values are tlgNEW_OBJECTS or tlgEXISTING_OBJECTS or both (see pr_sys.h tlg... flags)
// "iid"       : Includes interface id to a filter. Can be IID_ANY
// "pid"       : Includes plugin id to a filter. Can be PID_ANY
// "min_level" : minimum trace level for affected instances, prtMIN_TRACE_LEVEL by default
tERROR pr_call Root_TraceLevelSet( hi_Root _this, tDWORD op, tIID iid, tPID pid, tDWORD subtype, tDWORD vid, tDWORD level, tDWORD min_level ) {
	tERROR    error;
	PR_TRACE_A0( _this, "Enter Root::TraceLevelSet method" );

	if ( g_tracer )
		error = CALL_Tracer_TraceLevelSet( g_tracer, op, iid, pid, subtype, vid, level, min_level );
	else
		error = errOBJECT_NOT_FOUND;

	PR_TRACE_A1( _this, "Leave Root::TraceLevelSet method, ret t%terr", error );
	return error;
}
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetTracer )
// Parameters are:
tERROR pr_call Root_GetTracer( hi_Root _this, hTRACER* result ) {
  PR_TRACE_A0( _this, "Enter Root::GetTracer method" );
  if ( result )
		*result = g_tracer;
  PR_TRACE_A0( _this, "Leave Root::TraceLevelSet method, ret errOK" );
  return errOK;
}
// AVP Prague stamp end



// ---
static tBOOL CheckIFaceByPID( tINTERFACE* iface, tPTR param ) {
	if ( /*iface->plugin &&*/ (iface->pid == *(tPID*)param) )
		return cTRUE;
	return cFALSE;
}




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( External (user called) interface method implementation, GetPluginInstance )
// Parameters are:
tERROR pr_call Root_GetPluginInstance( hi_Root _this, hPLUGIN* result, tDATA* cookie, tPID pid ) {

	tHANDLE* pl;
	tERROR   err;
	tDATA    size;
	tPID     l_pid;
  tPO*     po;
	tBOOL    check_iface = cTRUE;

	PR_TRACE_A0( _this, "Enter Root::GetPluginInstance method" );
  enter_lf();

	err = errMODULE_NOT_FOUND;

  rlock(po);
	if ( !cookie || !*cookie )
		pl = g_plugins;
	else if ( *cookie == -1 ) {
		pl = 0;
		check_iface = cFALSE;
	}
	else
		pl = ((tHANDLE*)*cookie)->next;

	for ( ; pl; pl=pl->next ) {
		if ( (pid == PID_ANY) || (PR_SUCC(_PropertyGetSync(po,pl,&size,pgMODULE_ID,&l_pid,sizeof(l_pid))) && (pid == l_pid)) ) {
			err = errOK;
			break;
		}
	}

	if ( cookie ) {
		if ( pl )
			*cookie = (tDATA)pl;
		else
			*cookie = -1;
	}

	if ( !pl && check_iface ) {
		tINTERFACE* iface = _InterfaceFindInt( 0, CheckIFaceByPID, &pid );
		if ( iface && iface->plugin ) {
			pl = MakeHandle(iface->plugin);
			err = errOK;
		}
	}

  runlock(po);

	if ( result )
		*result = (hPLUGIN)MakeObject( pl );

	leave_lf();
  PR_TRACE_A2( _this, "Leave Root::GetPluginInstance method, ret hPLUGIN = 0x%08x, error = %terr", pl, err );
	return err;
}
// AVP Prague stamp end





