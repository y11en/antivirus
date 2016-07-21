// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  25 January 2006,  11:15 --------
// File Name   -- (null)i_root.c
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_root__37b137ba_ae7b_11d4_b757_00d0b7170e50 )
#define __i_root__37b137ba_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_plugin.h>
#include <Prague/iface/i_iface.h>
#include <Prague/iface/i_reg.h>
// AVP Prague stamp end



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
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_ROOT  ((tIID)(2))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface typedefs,  )
typedef tDWORD                         tFUP_ACTION; // //rus:тип операции при вызове метода FreeUnusedPlugins
typedef tDWORD                         tSNS_ACTION; // //rus:тип уведомления о событиях kernel'а
//typedef tDWORD                         tSerializeCallback; // method which is called back from the kernel to control serialize/deserialize procces
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )

// ----------  Sync object availability constants  ----------
// //rus:флаги, представляющие значение свойства pgSYNCHRONIZATION_AVAILABLE
#define fPRAGUE_SYNC_CS_AVAIL          ((tDWORD)(0x00000001L)) // //rus:доступность интерфейса "CriticalSection"//eng:"CriticalSection" interface availability
#define fPRAGUE_SYNC_SEMAPHORE_AVAIL   ((tDWORD)(0x00000002L)) // //rus:доступность интерфейса "Semaphore"//eng:"Semaphore" interface availability
#define fPRAGUE_SYNC_MUTEX_AVAIL       ((tDWORD)(0x00000004L)) // //rus:доступность интерфейса "Mutex"//eng:"Mutex" interface availability
#define fPRAGUE_SYNC_CS2L_AVAIL        ((tDWORD)(0x00000008L)) // //rus:доступность интерфейса "CriticalSection с двумя уровнями доступа"//eng:"2 level CriticalSection" interface availability

// ----------  Free unused plugins constants  ----------
// //rus:константы, используемые при вызове метода FreeUnusedPlugins
#define cFUP_SINGLE_ACTION             ((tFUP_ACTION)(0x00000000L)) // //rus:освободить плагины неиспользуемые на момент вызова
#define cFUP_PERMANENT                 ((tFUP_ACTION)(0x00000001L)) // //rus:маркировать плагины как выгружаемые по мере освобождения объектов

// ----------  SetNotificationSink constants  ----------
// //rus:константы определяющие тип уведомления о событиях kernel'а
#define cSNS_HEAP_SET                  ((tDWORD)(0)) // //rus:уведомление об установке HEAP'а на объект
#define cSNS_CUSTOM_PROP_SET           ((tDWORD)(1)) // //rus:уведомление об установке CUSTOM_PROPERTY на объект
#define cSNS_CUSTOM_PROP_DELETE        ((tDWORD)(2)) // //rus:уведомление об удалении CUSTOM_PROPERTY

// ----------   StreamSerialize constants  ----------
#define fSTREAM_PACK_INTEGRALS         ((tDWORD)(0x00000001)) // pack tDWORDs, tQWORDs, etc values
#define fSTREAM_PACK_UC_STRINGS        ((tDWORD)(0x00000002)) // pack UNICODE strings to UTF8
#define fSTREAM_PACK_DEFAULT           ((tDWORD)(fSTREAM_PACK_INTEGRALS | fSTREAM_PACK_UC_STRINGS)) // default pack flags
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )

// message class
#define pmc_KERNEL 0x2ffe4995 // //rus:сообщения ядра "Праги"//eng:Prague kernel message class

	#define pm_OBJECT_CREATE_FAILED_IFACE_NOT_FOUND 0xe00c8db6 // (3758919094) -- интерфейс не зарегистрирован, объект создан быть не может
	// context comment
	//    //rus:не передается, значение NULL//eng:doesn't pass, value NULL
	// data content comment
	//    //rus:массив из трех tDWORD'ов
	//      0  -- идентификатор интерфейса
	//      1  -- идентификатор плагина
	//      2  -- подтип интерфейса

	#define pm_INTERFACE_REGISTERED 0x8ecd164c // (2395805260) -- //rus:интерфейс зарегистрирован
	// context comment
	//    //rus:не передается, NULL
	// data content comment
	//    //rus:массив восьми tDWORD'ов
	//      0  -- iid, идентификатор интерфейса
	//      1  -- pid, идентификатор плагина
	//      2  -- subtype, идентификатор подтипа
	//      3  -- version, версия интерфейса
	//      4  -- vid, идентификатор разработчика интерфейса
	//      5  -- flags, флаги регистрации интерфейса
	//      6  -- iid, идентификатор совместимого интерфейса
	//      7  -- index, индекс реализации интерфейса в плагине

	#define pm_INTERFACE_VERSION_CONFILCT 0x9d1cfcbf // (2635922623) -- интерфейс не может быть зарегистрирован, т.к. зарегистрирован интерфейс с версией и/или timestamp старше чем у регистрируемого

	#define pm_OBJECT_CHILD_CLOSE_FAILED 0x03ebea51 // (65792593) --

	#define pm_OBJECT_PRECLOSE_FAILED 0x2eb0221b // (783295003) --

	#define pm_OBJECT_CLOSE_FAILED 0xb49ecf7c // (3030306684) --

	#define pm_INTERFACE_LOAD_ERROR 0x6c8b097a // (1821051258) --
	// context comment
	//    NULL
	// data content comment
	//    pointer to loader error
	//

	#define pm_OBJECT_HAS_BEEN_CREATED 0x1e7a5976 // (511334774) --

	#define pm_OBJECT_HAS_BEEN_FREED 0xceed40f0 // (3471655152) --

	#define pm_OBJECT_IS_LOCKED_ON_CLOSE 0x5464d13c // (1415893308) --
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined errors,  )
#define errPROPERTY_ALREADY_REGISTERED           PR_MAKE_DECL_ERR(IID_ROOT, 0x001) // 0x80002001,-2147475455 (1) -- //rus:идентификатор свойства уже зарегистрирован//eng:named property alredy registerd
#define errROOT_OBJECT_CANNOT_BE_CREATED         PR_MAKE_DECL_ERR(IID_ROOT, 0x002) // 0x80002002,-2147475454 (2) -- ROOT object cannot be created
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hROOT;
#else
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface forward declaration,  )
struct iRootVtbl;
typedef struct iRootVtbl iRootVtbl;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cRoot;
	typedef cRoot* hROOT;
#else
	/*typedef*/ struct tag_hROOT {
		const iRootVtbl*   vtbl; // pointer to the "Root" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	}; // *hROOT;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



#if defined(__cplusplus)
  class cOutStream;
  class cInStream;
#else
  typedef struct tag_cOutStream    cOutStream;
  typedef struct tag_cInStream     cInStream;
#endif

//typedef struct tag_cSerDescriptor cSerDescriptor;
//typedef struct tag_cSerDescriptorField cSerDescriptorField;


// ---
typedef enum tag_SerializeAction {
  sa_alloc_simple_vector_el = 1,
  sa_alloc_ser_vector_el,
  sa_alloc_ser_obj,
	sa_serialize_field,
	sa_deserialize_field,
} tSerializeAction;

typedef tERROR (pr_call* tSerializeCallback)( tPTR context, tSerializeAction action, tPTR obj, const cSerDescriptorField* field, tVOID* data );


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Root_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )



	// ----------- Interface management ----------
	typedef   tERROR (pr_call *fnpRoot_RegisterIFace)                    ( hROOT _this, tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid, const tVOID* internal_table, tDWORD internal_table_size, const tVOID* external_table, tDWORD external_table_size, const tDATA* prop_table, tDWORD prop_table_size, tDWORD memsize, tDWORD flags ); // -- //rus:Регистрация интерфейса//eng:Registers an interface;
	typedef   tERROR (pr_call *fnpRoot_UnregisterIFace)                  ( hROOT _this, tIID iid, tPID pid, tDWORD subtype, tVID vid ); // -- //rus:разрегистрация интерфейса//eng:Unregisters an interface;

	// ----------- Plugin management ----------
	typedef   tERROR (pr_call *fnpRoot_FindPlugins)                      ( hROOT _this, tPID mfinder_pid, const tVOID* param_pool, tDWORD param_pool_size, tDWORD load_flag_set ); // -- //rus:интегрирование модулей наденных указанным загрузчиком в систему//eng:integrate modules to the system by specific loader;

	// ----------- Custom property methods ----------
	typedef   tERROR (pr_call *fnpRoot_RegisterCustomPropId)             ( hROOT _this, tDWORD* result, tSTRING name, tDWORD type ); // -- //rus:распределяет уникальный идентификатор для пользовательского свойства//eng:allocates unique property id;
	typedef   tERROR (pr_call *fnpRoot_UnregisterCustomPropId)           ( hROOT _this, tSTRING name ); // -- //rus:освобождает уникальныйидентификатор свойсва//eng:deallocates unique property id;
	typedef   tERROR (pr_call *fnpRoot_GetCustomPropId)                  ( hROOT _this, tDWORD* result, tSTRING name, tDWORD type ); // -- //rus:возвращает, ранее зарегистрированный, идентификатор свойства//eng:returns id of the registered named property identifier;

	// ----------- Tracing methods ----------
	typedef   tERROR (pr_call *fnpRoot_TraceLevelSet)                    ( hROOT _this, tDWORD op, tIID iid, tPID pid, tDWORD subtype, tDWORD vid, tDWORD level, tDWORD min_level ); // -- //rus:устанавливает уровень трассировки на определенный интерфейс или плагин//eng:set trace level by interface or/and plugin;
	typedef   tERROR (pr_call *fnpRoot_GetTracer)                        ( hROOT _this, hTRACER* result );   // -- //rus:возвращает текущий трасер//eng:returns current tracer;

	// ----------- Plugin management ----------
	typedef   tERROR (pr_call *fnpRoot_GetPluginInstance)                ( hROOT _this, hPLUGIN* result, tDATA* cookie, tPID pid ); // -- //rus:поиск объект типа hMODULE по идентификатору плагина//eng:returns plugin by identifier;

	// ----------- Interface management ----------
	typedef   tERROR (pr_call *fnpRoot_RegisterIFaceEx)                  ( hROOT _this, hIFACE* result, tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid, const tVOID* internal_table, tDWORD internal_table_size, const tVOID* external_table, tDWORD external_table_size, const tDATA* prop_table, tDWORD prop_table_size, tDWORD memsize, tDWORD flags, tIID iid_compat, hPLUGIN implemented_module, tDWORD implemented_index ); // -- //rus:регистрация интерфейса с указанием плагина//eng:Registers an interface with plugin indication;

	// ----------- import/export management ----------
	typedef   tERROR (pr_call *fnpRoot_RegisterExportTable)              ( hROOT _this, tDWORD* result, const tEXPORT* ex_tbl, tPID exporter_pid ); // -- //rus:регистрирует таблицу экспортных функций плагина//eng:register export function table of the plugin;
	typedef   tERROR (pr_call *fnpRoot_RegisterExportFunc)               ( hROOT _this, const tFUNC_PTR func_ptr, tDWORD func_cls, tDWORD func_id, tPID exporter_pid ); // -- //rus:регистрирует экспортную функцию заданного плагина//eng:register export function of the plugin;
	typedef   tERROR (pr_call *fnpRoot_RegisterExportFuncByName)         ( hROOT _this, const tFUNC_PTR func_ptr, tDWORD func_cls, tSTRING func_name, tPID exporter_pid ); // -- //rus:регистрирует экспортную функцию для заданного плагина по имени//eng:register export function of the plugin by name;
	typedef   tERROR (pr_call *fnpRoot_ResolveImportTable)               ( hROOT _this, tDWORD* result, const tIMPORT* imp_tbl, tPID importer_pid ); // -- //rus:разрешает таблицу ссылок на экспортные функции//eng:resolve table of references to the export functions;
	typedef   tERROR (pr_call *fnpRoot_ResolveImportFunc)                ( hROOT _this, tFUNC_PTR* result, tDWORD func_cls, tDWORD func_id, tPID importer_pid ); // -- //rus:разрещает ссылку на экспортную функцию/eng:resolve reference to the export function;
	typedef   tERROR (pr_call *fnpRoot_ResolveImportFuncByName)          ( hROOT _this, tFUNC_PTR* result, tDWORD func_cls, const tSTRING func_name, tPID importer_pid ); // -- //rus:разрещает ссылку на экспортную функцию по имени/eng:resolve reference to the export function by name;
	typedef   tERROR (pr_call *fnpRoot_ReleaseImportTable)               ( hROOT _this, tDWORD* result, const tIMPORT* imp_tbl, tPID importer_pid ); // -- //rus:освобождает таблицу ссылок на экспортные функции//eng:releases table of the references;
	typedef   tERROR (pr_call *fnpRoot_ReleaseImportFunc)                ( hROOT _this, tDWORD func_cls, tDWORD func_id, tPID importer_pid ); // -- //rus:освобождает ссылку на экспортную функцию//eng:releases reference to the export function;
	typedef   tERROR (pr_call *fnpRoot_ReleaseImportFuncByName)          ( hROOT _this, tDWORD func_cls, const tSTRING func_name, tPID importer_pid ); // -- //rus:освобождает ссылку на экспортную функцию по имени//eng:releases reference to the export function by name;

	// ----------- Plugin management ----------
	typedef   tERROR (pr_call *fnpRoot_PluginUnloadNotification)         ( hROOT _this, hPLUGIN plugin_unloaded ); // -- //rus:информирование ядра Праги о выгрузке плагина из памяти//eng:outsiders notify root about plugin unloading;
	typedef   tERROR (pr_call *fnpRoot_PluginLoadNotification)           ( hROOT _this, hPLUGIN plugin_loaded ); // -- //rus:информирование ядра Праги о загрузке плагина в память//eng:outsiders notify root about plugin loading;
	typedef   tERROR (pr_call *fnpRoot_FreeUnusedPlugins)                ( hROOT _this, tPID* pid_array, tDWORD pid_array_count, tFUP_ACTION action, tDWORD during ); // -- forces unload unsued plugins;

	// ----------- Notification Sink management ----------
	typedef   tERROR (pr_call *fnpRoot_SetNotificationSink)              ( hROOT _this, tDWORD* id, tSNS_ACTION type, tFUNC_PTR func, tPTR ctx ); // -- //rus:установить функцию-приемник уведомления о событии kernel'a;
	typedef   tERROR (pr_call *fnpRoot_FreeNotificationSink)             ( hROOT _this, tDWORD id ); // -- //rus:снять функцию-приемник уведомления о событии kernel'a;

	// ----------- Interface management ----------
	typedef   tERROR (pr_call *fnpRoot_CheckIFace)                       ( hROOT _this, tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid, tDWORD flags, hPLUGIN implemented_module ); // -- //rus:проверка интерфейса//eng:Check an interface;

	// ----------- kernel internal synchronization object ----------
	typedef   tERROR (pr_call *fnpRoot_SetKernelSyncObject)              ( hROOT _this, tIID iid, tDWORD pid, tDWORD subtype ); // -- force kernel to use synchronization object selected by iid, pid, and subtype;
	typedef   tERROR (pr_call *fnpRoot_GetKernelSyncObject)              ( hROOT _this, hOBJECT* sync_object ); // -- kernel used synchronization object;

	// ----------- serializable structures support ----------
	typedef   tERROR (pr_call *fnpRoot_RegisterSerializableDescriptor)   ( hROOT _this, const cSerDescriptor* descriptor ); // -- ;
	typedef   tERROR (pr_call *fnpRoot_UnregisterSerializableDescriptor) ( hROOT _this, const cSerDescriptor* descriptor ); // -- ;
	typedef   tERROR (pr_call *fnpRoot_FindSerializableDescriptor)       ( hROOT _this, cSerDescriptor** descriptor, tDWORD uniqueId ); // -- ;
	typedef   tERROR (pr_call *fnpRoot_CreateSerializable)               ( hROOT _this, tDWORD unique_id, cSerializable** object ); // -- ;
	typedef   tERROR (pr_call *fnpRoot_DestroySerializable)              ( hROOT _this, cSerializable* object ); // -- ;
	typedef   tERROR (pr_call *fnpRoot_CopySerializable)                 ( hROOT _this, cSerializable** dst, const cSerializable* src ); // -- ;
	typedef   tERROR (pr_call *fnpRoot_StreamSerialize)                  ( hROOT _this, const cSerializable* object, tDWORD ser_id, tBYTE* buffer, tDWORD size, tDWORD* output_size, tDWORD flags ); // -- //eng:serialize instanse of serializable structure;
	typedef   tERROR (pr_call *fnpRoot_StreamDeserialize)                ( hROOT _this, cSerializable** object, const tBYTE* buffer, tDWORD size, tDWORD* accepted_size ); // -- //eng:deserialize instance of serializable structure;
	typedef   tERROR (pr_call *fnpRoot_RegSerialize)                     ( hROOT _this, const cSerializable* object, tDWORD ser_id, hREGISTRY registry, const tCHAR* path ); // -- //eng:serialize instanse of serializable structure;
	typedef   tERROR (pr_call *fnpRoot_RegDeserialize)                   ( hROOT _this, cSerializable** object, hREGISTRY registry, const tCHAR* path, tDWORD uniqueId ); // -- //eng:deserialize instance of serializable structure;

	// ----------- kernel locking !!! be careful !!! ----------
	typedef   tERROR (pr_call *fnpRoot_LockKernel)                       ( hROOT _this );   // -- ;
	typedef   tERROR (pr_call *fnpRoot_UnlockKernel)                     ( hROOT _this );   // -- ;

	// ----------- Plugin management ----------
	typedef   tERROR (pr_call *fnpRoot_LoadModule)                       ( hROOT _this, hPLUGIN* plugin, const tVOID* module_name, tDWORD name_size, tCODEPAGE cp ); // -- load module by name;

	// ----------- Custom property methods ----------
	typedef   tERROR (pr_call *fnpRoot_GetCustomPropName)                ( hROOT _this, tDWORD* out_size, tPROPID prop_id, tPTR buffer, tDWORD size ); // -- returns registered custom property name by identifier;

	// ----------- serializable structures support ----------
	typedef   tERROR (pr_call *fnpRoot_StreamSerializeField)             ( hROOT _this, const cSerializable* container, const tVOID* field, cOutStream* stream, tDWORD reserved ); // -- //eng:serialize one field of serializable structure;
	typedef   tERROR (pr_call *fnpRoot_RegSerializeField)                ( hROOT _this, const cSerializable* container, const tVOID* field, hREGISTRY registry, const tCHAR* path, tDWORD reserved ); // -- ;
	typedef   tERROR (pr_call *fnpRoot_RegDeserializeField)              ( hROOT _this, const cSerializable* container, tVOID* field, hREGISTRY registry, const tCHAR* path, tDWORD reserved ); // -- //eng:deserialize one field of serializable structure;
	typedef   tERROR (pr_call *fnpRoot_RegSerializeEx)                   ( hROOT _this, const cSerializable* object, tDWORD ser_id, hREGISTRY registry, const tCHAR* path, tSerializeCallback callback_func, tPTR callback_params ); // -- //eng:serialize instanse of serializable structure;
	typedef   tERROR (pr_call *fnpRoot_RegDeserializeEx)                 ( hROOT _this, cSerializable** object, hREGISTRY registry, const tCHAR* path, tDWORD uniqueId, tSerializeCallback callback_func, tPTR callback_params ); // -- //eng:deserialize instance of serializable structure;
	typedef   tERROR (pr_call *fnpRoot_CompareSerializable)              ( hROOT _this, const cSerializable* str1, const cSerializable* str2, tDWORD ser_id, tDWORD flags ); // -- ;
	typedef   tERROR (pr_call *fnpRoot_FindSerializableDescriptorByName) ( hROOT _this, cSerDescriptor** descriptor, const tCHAR* name ); // -- ;

	// ----------- serializable structures support ----------
	typedef   tERROR (pr_call *fnpRoot_AddRefSerializable)               ( hROOT _this, cSerializable* object ); // -- increase reference count of serializable object;


// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iRootVtbl {

// ----------- Interface management ----------
	fnpRoot_RegisterIFace                     RegisterIFace;
	fnpRoot_UnregisterIFace                   UnregisterIFace;

// ----------- Plugin management ----------
	fnpRoot_FindPlugins                       FindPlugins;

// ----------- Custom property methods ----------
	fnpRoot_RegisterCustomPropId              RegisterCustomPropId;
	fnpRoot_UnregisterCustomPropId            UnregisterCustomPropId;
	fnpRoot_GetCustomPropId                   GetCustomPropId;

// ----------- Tracing methods ----------
	fnpRoot_TraceLevelSet                     TraceLevelSet;
	fnpRoot_GetTracer                         GetTracer;

// ----------- Plugin management ----------
	fnpRoot_GetPluginInstance                 GetPluginInstance;

// ----------- Interface management ----------
	fnpRoot_RegisterIFaceEx                   RegisterIFaceEx;

// ----------- import/export management ----------
	fnpRoot_RegisterExportTable               RegisterExportTable;
	fnpRoot_RegisterExportFunc                RegisterExportFunc;
	fnpRoot_RegisterExportFuncByName          RegisterExportFuncByName;
	fnpRoot_ResolveImportTable                ResolveImportTable;
	fnpRoot_ResolveImportFunc                 ResolveImportFunc;
	fnpRoot_ResolveImportFuncByName           ResolveImportFuncByName;
	fnpRoot_ReleaseImportTable                ReleaseImportTable;
	fnpRoot_ReleaseImportFunc                 ReleaseImportFunc;
	fnpRoot_ReleaseImportFuncByName           ReleaseImportFuncByName;

// ----------- Plugin management ----------
	fnpRoot_PluginUnloadNotification          PluginUnloadNotification;
	fnpRoot_PluginLoadNotification            PluginLoadNotification;
	fnpRoot_FreeUnusedPlugins                 FreeUnusedPlugins;

// ----------- Notification Sink management ----------
	fnpRoot_SetNotificationSink               SetNotificationSink;
	fnpRoot_FreeNotificationSink              FreeNotificationSink;

// ----------- Interface management ----------
	fnpRoot_CheckIFace                        CheckIFace;

// ----------- kernel internal synchronization object ----------
	fnpRoot_SetKernelSyncObject               SetKernelSyncObject;
	fnpRoot_GetKernelSyncObject               GetKernelSyncObject;

// ----------- serializable structures support ----------
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

// ----------- Custom property methods ----------
	fnpRoot_GetCustomPropName                 GetCustomPropName;

// ----------- serializable structures support ----------
	fnpRoot_StreamSerializeField              StreamSerializeField;
	fnpRoot_RegSerializeField                 RegSerializeField;
	fnpRoot_RegDeserializeField               RegDeserializeField;
	fnpRoot_RegSerializeEx                    RegSerializeEx;
	fnpRoot_RegDeserializeEx                  RegDeserializeEx;
	fnpRoot_CompareSerializable               CompareSerializable;
	fnpRoot_FindSerializableDescriptorByName  FindSerializableDescriptorByName;

// ----------- serializable structures support ----------
	fnpRoot_AddRefSerializable                AddRefSerializable;
}; // "Root" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Root_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION            mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT            mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
//! #define pgOBJECT_NAME                  mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000040 )
#define pgOUTPUT_FUNC                  mPROPERTY_MAKE_GLOBAL( pTYPE_FUNC_PTR, 0x00001000 )
#define pgOBJECT_COUNT                 mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001004 )
#define pgEXCEPTION_CATCHING_AVAILABLE mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001005 )
#define pgSYNCHRONIZATION_AVAILABLE    mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001006 )
#define pgUNLOAD_UNUSED_DURING         mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001007 )
#define pgUNLOAD_UNUSED_FORCE          mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001008 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_Root_RegisterIFace( _this, iid, pid, subtype, version, vid, internal_table, internal_table_size, external_table, external_table_size, prop_table, prop_table_size, memsize, flags )                                                               ((_this)->vtbl->RegisterIFace( (_this), iid, pid, subtype, version, vid, internal_table, internal_table_size, external_table, external_table_size, prop_table, prop_table_size, memsize, flags ))
	#define CALL_Root_UnregisterIFace( _this, iid, pid, subtype, vid )                                                                                                                                                                                             ((_this)->vtbl->UnregisterIFace( (_this), iid, pid, subtype, vid ))
	#define CALL_Root_FindPlugins( _this, mfinder_pid, param_pool, param_pool_size, load_flag_set )                                                                                                                                                                ((_this)->vtbl->FindPlugins( (_this), mfinder_pid, param_pool, param_pool_size, load_flag_set ))
	#define CALL_Root_RegisterCustomPropId( _this, result, name, type )                                                                                                                                                                                            ((_this)->vtbl->RegisterCustomPropId( (_this), result, name, type ))
	#define CALL_Root_UnregisterCustomPropId( _this, name )                                                                                                                                                                                                        ((_this)->vtbl->UnregisterCustomPropId( (_this), name ))
	#define CALL_Root_GetCustomPropId( _this, result, name, type )                                                                                                                                                                                                 ((_this)->vtbl->GetCustomPropId( (_this), result, name, type ))
	#define CALL_Root_TraceLevelSet( _this, op, iid, pid, subtype, vid, level, min_level )                                                                                                                                                                         ((_this)->vtbl->TraceLevelSet( (_this), op, iid, pid, subtype, vid, level, min_level ))
	#define CALL_Root_GetTracer( _this, result )                                                                                                                                                                                                                   ((_this)->vtbl->GetTracer( (_this), result ))
	#define CALL_Root_GetPluginInstance( _this, result, cookie, pid )                                                                                                                                                                                              ((_this)->vtbl->GetPluginInstance( (_this), result, cookie, pid ))
	#define CALL_Root_RegisterIFaceEx( _this, result, iid, pid, subtype, version, vid, internal_table, internal_table_size, external_table, external_table_size, prop_table, prop_table_size, memsize, flags, iid_compat, implemented_module, implemented_index )  ((_this)->vtbl->RegisterIFaceEx( (_this), result, iid, pid, subtype, version, vid, internal_table, internal_table_size, external_table, external_table_size, prop_table, prop_table_size, memsize, flags, iid_compat, implemented_module, implemented_index ))
	#define CALL_Root_RegisterExportTable( _this, result, ex_tbl, exporter_pid )                                                                                                                                                                                   ((_this)->vtbl->RegisterExportTable( (_this), result, ex_tbl, exporter_pid ))
	#define CALL_Root_RegisterExportFunc( _this, func_ptr, func_cls, func_id, exporter_pid )                                                                                                                                                                       ((_this)->vtbl->RegisterExportFunc( (_this), func_ptr, func_cls, func_id, exporter_pid ))
	#define CALL_Root_RegisterExportFuncByName( _this, func_ptr, func_cls, func_name, exporter_pid )                                                                                                                                                               ((_this)->vtbl->RegisterExportFuncByName( (_this), func_ptr, func_cls, func_name, exporter_pid ))
	#define CALL_Root_ResolveImportTable( _this, result, imp_tbl, importer_pid )                                                                                                                                                                                   ((_this)->vtbl->ResolveImportTable( (_this), result, imp_tbl, importer_pid ))
	#define CALL_Root_ResolveImportFunc( _this, result, func_cls, func_id, importer_pid )                                                                                                                                                                          ((_this)->vtbl->ResolveImportFunc( (_this), result, func_cls, func_id, importer_pid ))
	#define CALL_Root_ResolveImportFuncByName( _this, result, func_cls, func_name, importer_pid )                                                                                                                                                                  ((_this)->vtbl->ResolveImportFuncByName( (_this), result, func_cls, func_name, importer_pid ))
	#define CALL_Root_ReleaseImportTable( _this, result, imp_tbl, importer_pid )                                                                                                                                                                                   ((_this)->vtbl->ReleaseImportTable( (_this), result, imp_tbl, importer_pid ))
	#define CALL_Root_ReleaseImportFunc( _this, func_cls, func_id, importer_pid )                                                                                                                                                                                  ((_this)->vtbl->ReleaseImportFunc( (_this), func_cls, func_id, importer_pid ))
	#define CALL_Root_ReleaseImportFuncByName( _this, func_cls, func_name, importer_pid )                                                                                                                                                                          ((_this)->vtbl->ReleaseImportFuncByName( (_this), func_cls, func_name, importer_pid ))
	#define CALL_Root_PluginUnloadNotification( _this, plugin_unloaded )                                                                                                                                                                                           ((_this)->vtbl->PluginUnloadNotification( (_this), plugin_unloaded ))
	#define CALL_Root_PluginLoadNotification( _this, plugin_loaded )                                                                                                                                                                                               ((_this)->vtbl->PluginLoadNotification( (_this), plugin_loaded ))
	#define CALL_Root_FreeUnusedPlugins( _this, pid_array, pid_array_count, action, during )                                                                                                                                                                       ((_this)->vtbl->FreeUnusedPlugins( (_this), pid_array, pid_array_count, action, during ))
	#define CALL_Root_SetNotificationSink( _this, id, type, func, ctx )                                                                                                                                                                                            ((_this)->vtbl->SetNotificationSink( (_this), id, type, func, ctx ))
	#define CALL_Root_FreeNotificationSink( _this, id )                                                                                                                                                                                                            ((_this)->vtbl->FreeNotificationSink( (_this), id ))
	#define CALL_Root_CheckIFace( _this, iid, pid, subtype, version, vid, flags, implemented_module )                                                                                                                                                              ((_this)->vtbl->CheckIFace( (_this), iid, pid, subtype, version, vid, flags, implemented_module ))
	#define CALL_Root_SetKernelSyncObject( _this, iid, pid, subtype )                                                                                                                                                                                              ((_this)->vtbl->SetKernelSyncObject( (_this), iid, pid, subtype ))
	#define CALL_Root_GetKernelSyncObject( _this, sync_object )                                                                                                                                                                                                    ((_this)->vtbl->GetKernelSyncObject( (_this), sync_object ))
	#define CALL_Root_RegisterSerializableDescriptor( _this, descriptor )                                                                                                                                                                                          ((_this)->vtbl->RegisterSerializableDescriptor( (_this), descriptor ))
	#define CALL_Root_UnregisterSerializableDescriptor( _this, descriptor )                                                                                                                                                                                        ((_this)->vtbl->UnregisterSerializableDescriptor( (_this), descriptor ))
	#define CALL_Root_FindSerializableDescriptor( _this, descriptor, uniqueId )                                                                                                                                                                                    ((_this)->vtbl->FindSerializableDescriptor( (_this), descriptor, uniqueId ))
	#define CALL_Root_CreateSerializable( _this, unique_id, object )                                                                                                                                                                                               ((_this)->vtbl->CreateSerializable( (_this), unique_id, object ))
	#define CALL_Root_DestroySerializable( _this, object )                                                                                                                                                                                                         ((_this)->vtbl->DestroySerializable( (_this), object ))
	#define CALL_Root_CopySerializable( _this, dst, src )                                                                                                                                                                                                          ((_this)->vtbl->CopySerializable( (_this), dst, src ))
	#define CALL_Root_StreamSerialize( _this, object, ser_id, buffer, size, output_size, flags )                                                                                                                                                                   ((_this)->vtbl->StreamSerialize( (_this), object, ser_id, buffer, size, output_size, flags ))
	#define CALL_Root_StreamDeserialize( _this, object, buffer, size, accepted_size )                                                                                                                                                                              ((_this)->vtbl->StreamDeserialize( (_this), object, buffer, size, accepted_size ))
	#define CALL_Root_RegSerialize( _this, object, ser_id, registry, path )                                                                                                                                                                                        ((_this)->vtbl->RegSerialize( (_this), object, ser_id, registry, path ))
	#define CALL_Root_RegDeserialize( _this, object, registry, path, uniqueId )                                                                                                                                                                                    ((_this)->vtbl->RegDeserialize( (_this), object, registry, path, uniqueId ))
	#define CALL_Root_LockKernel( _this )                                                                                                                                                                                                                          ((_this)->vtbl->LockKernel( (_this) ))
	#define CALL_Root_UnlockKernel( _this )                                                                                                                                                                                                                        ((_this)->vtbl->UnlockKernel( (_this) ))
	#define CALL_Root_LoadModule( _this, plugin, module_name, name_size, cp )                                                                                                                                                                                      ((_this)->vtbl->LoadModule( (_this), plugin, module_name, name_size, cp ))
	#define CALL_Root_GetCustomPropName( _this, out_size, prop_id, buffer, size )                                                                                                                                                                                  ((_this)->vtbl->GetCustomPropName( (_this), out_size, prop_id, buffer, size ))
	#define CALL_Root_StreamSerializeField( _this, container, field, stream, reserved )                                                                                                                                                                            ((_this)->vtbl->StreamSerializeField( (_this), container, field, stream, reserved ))
	#define CALL_Root_RegSerializeField( _this, container, field, registry, path, reserved )                                                                                                                                                                       ((_this)->vtbl->RegSerializeField( (_this), container, field, registry, path, reserved ))
	#define CALL_Root_RegDeserializeField( _this, container, field, registry, path, reserved )                                                                                                                                                                     ((_this)->vtbl->RegDeserializeField( (_this), container, field, registry, path, reserved ))
	#define CALL_Root_RegSerializeEx( _this, object, ser_id, registry, path, callback_func, callback_params )                                                                                                                                                      ((_this)->vtbl->RegSerializeEx( (_this), object, ser_id, registry, path, callback_func, callback_params ))
	#define CALL_Root_RegDeserializeEx( _this, object, registry, path, uniqueId, callback_func, callback_params )                                                                                                                                                  ((_this)->vtbl->RegDeserializeEx( (_this), object, registry, path, uniqueId, callback_func, callback_params ))
	#define CALL_Root_CompareSerializable( _this, str1, str2, ser_id, flags )                                                                                                                                                                                      ((_this)->vtbl->CompareSerializable( (_this), str1, str2, ser_id, flags ))
	#define CALL_Root_FindSerializableDescriptorByName( _this, descriptor, name )                                                                                                                                                                                  ((_this)->vtbl->FindSerializableDescriptorByName( (_this), descriptor, name ))
	#define CALL_Root_AddRefSerializable( _this, object )                                                                                                                                                                                                          ((_this)->vtbl->AddRefSerializable( (_this), object ))
#else // if !defined( __cplusplus )
	#define CALL_Root_RegisterIFace( _this, iid, pid, subtype, version, vid, internal_table, internal_table_size, external_table, external_table_size, prop_table, prop_table_size, memsize, flags )                                                               ((_this)->RegisterIFace( iid, pid, subtype, version, vid, internal_table, internal_table_size, external_table, external_table_size, prop_table, prop_table_size, memsize, flags ))
	#define CALL_Root_UnregisterIFace( _this, iid, pid, subtype, vid )                                                                                                                                                                                             ((_this)->UnregisterIFace( iid, pid, subtype, vid ))
	#define CALL_Root_FindPlugins( _this, mfinder_pid, param_pool, param_pool_size, load_flag_set )                                                                                                                                                                ((_this)->FindPlugins( mfinder_pid, param_pool, param_pool_size, load_flag_set ))
	#define CALL_Root_RegisterCustomPropId( _this, result, name, type )                                                                                                                                                                                            ((_this)->RegisterCustomPropId( result, name, type ))
	#define CALL_Root_UnregisterCustomPropId( _this, name )                                                                                                                                                                                                        ((_this)->UnregisterCustomPropId( name ))
	#define CALL_Root_GetCustomPropId( _this, result, name, type )                                                                                                                                                                                                 ((_this)->GetCustomPropId( result, name, type ))
	#define CALL_Root_TraceLevelSet( _this, op, iid, pid, subtype, vid, level, min_level )                                                                                                                                                                         ((_this)->TraceLevelSet( op, iid, pid, subtype, vid, level, min_level ))
	#define CALL_Root_GetTracer( _this, result )                                                                                                                                                                                                                   ((_this)->GetTracer( result ))
	#define CALL_Root_GetPluginInstance( _this, result, cookie, pid )                                                                                                                                                                                              ((_this)->GetPluginInstance( result, cookie, pid ))
	#define CALL_Root_RegisterIFaceEx( _this, result, iid, pid, subtype, version, vid, internal_table, internal_table_size, external_table, external_table_size, prop_table, prop_table_size, memsize, flags, iid_compat, implemented_module, implemented_index )  ((_this)->RegisterIFaceEx( result, iid, pid, subtype, version, vid, internal_table, internal_table_size, external_table, external_table_size, prop_table, prop_table_size, memsize, flags, iid_compat, implemented_module, implemented_index ))
	#define CALL_Root_RegisterExportTable( _this, result, ex_tbl, exporter_pid )                                                                                                                                                                                   ((_this)->RegisterExportTable( result, ex_tbl, exporter_pid ))
	#define CALL_Root_RegisterExportFunc( _this, func_ptr, func_cls, func_id, exporter_pid )                                                                                                                                                                       ((_this)->RegisterExportFunc( func_ptr, func_cls, func_id, exporter_pid ))
	#define CALL_Root_RegisterExportFuncByName( _this, func_ptr, func_cls, func_name, exporter_pid )                                                                                                                                                               ((_this)->RegisterExportFuncByName( func_ptr, func_cls, func_name, exporter_pid ))
	#define CALL_Root_ResolveImportTable( _this, result, imp_tbl, importer_pid )                                                                                                                                                                                   ((_this)->ResolveImportTable( result, imp_tbl, importer_pid ))
	#define CALL_Root_ResolveImportFunc( _this, result, func_cls, func_id, importer_pid )                                                                                                                                                                          ((_this)->ResolveImportFunc( result, func_cls, func_id, importer_pid ))
	#define CALL_Root_ResolveImportFuncByName( _this, result, func_cls, func_name, importer_pid )                                                                                                                                                                  ((_this)->ResolveImportFuncByName( result, func_cls, func_name, importer_pid ))
	#define CALL_Root_ReleaseImportTable( _this, result, imp_tbl, importer_pid )                                                                                                                                                                                   ((_this)->ReleaseImportTable( result, imp_tbl, importer_pid ))
	#define CALL_Root_ReleaseImportFunc( _this, func_cls, func_id, importer_pid )                                                                                                                                                                                  ((_this)->ReleaseImportFunc( func_cls, func_id, importer_pid ))
	#define CALL_Root_ReleaseImportFuncByName( _this, func_cls, func_name, importer_pid )                                                                                                                                                                          ((_this)->ReleaseImportFuncByName( func_cls, func_name, importer_pid ))
	#define CALL_Root_PluginUnloadNotification( _this, plugin_unloaded )                                                                                                                                                                                           ((_this)->PluginUnloadNotification( plugin_unloaded ))
	#define CALL_Root_PluginLoadNotification( _this, plugin_loaded )                                                                                                                                                                                               ((_this)->PluginLoadNotification( plugin_loaded ))
	#define CALL_Root_FreeUnusedPlugins( _this, pid_array, pid_array_count, action, during )                                                                                                                                                                       ((_this)->FreeUnusedPlugins( pid_array, pid_array_count, action, during ))
	#define CALL_Root_SetNotificationSink( _this, id, type, func, ctx )                                                                                                                                                                                            ((_this)->SetNotificationSink( id, type, func, ctx ))
	#define CALL_Root_FreeNotificationSink( _this, id )                                                                                                                                                                                                            ((_this)->FreeNotificationSink( id ))
	#define CALL_Root_CheckIFace( _this, iid, pid, subtype, version, vid, flags, implemented_module )                                                                                                                                                              ((_this)->CheckIFace( iid, pid, subtype, version, vid, flags, implemented_module ))
	#define CALL_Root_SetKernelSyncObject( _this, iid, pid, subtype )                                                                                                                                                                                              ((_this)->SetKernelSyncObject( iid, pid, subtype ))
	#define CALL_Root_GetKernelSyncObject( _this, sync_object )                                                                                                                                                                                                    ((_this)->GetKernelSyncObject( sync_object ))
	#define CALL_Root_RegisterSerializableDescriptor( _this, descriptor )                                                                                                                                                                                          ((_this)->RegisterSerializableDescriptor( descriptor ))
	#define CALL_Root_UnregisterSerializableDescriptor( _this, descriptor )                                                                                                                                                                                        ((_this)->UnregisterSerializableDescriptor( descriptor ))
	#define CALL_Root_FindSerializableDescriptor( _this, descriptor, uniqueId )                                                                                                                                                                                    ((_this)->FindSerializableDescriptor( descriptor, uniqueId ))
	#define CALL_Root_CreateSerializable( _this, unique_id, object )                                                                                                                                                                                               ((_this)->CreateSerializable( unique_id, object ))
	#define CALL_Root_DestroySerializable( _this, object )                                                                                                                                                                                                         ((_this)->DestroySerializable( object ))
	#define CALL_Root_CopySerializable( _this, dst, src )                                                                                                                                                                                                          ((_this)->CopySerializable( dst, src ))
	#define CALL_Root_StreamSerialize( _this, object, ser_id, buffer, size, output_size, flags )                                                                                                                                                                   ((_this)->StreamSerialize( object, ser_id, buffer, size, output_size, flags ))
	#define CALL_Root_StreamDeserialize( _this, object, buffer, size, accepted_size )                                                                                                                                                                              ((_this)->StreamDeserialize( object, buffer, size, accepted_size ))
	#define CALL_Root_RegSerialize( _this, object, ser_id, registry, path )                                                                                                                                                                                        ((_this)->RegSerialize( object, ser_id, registry, path ))
	#define CALL_Root_RegDeserialize( _this, object, registry, path, uniqueId )                                                                                                                                                                                    ((_this)->RegDeserialize( object, registry, path, uniqueId ))
	#define CALL_Root_LockKernel( _this )                                                                                                                                                                                                                          ((_this)->LockKernel( ))
	#define CALL_Root_UnlockKernel( _this )                                                                                                                                                                                                                        ((_this)->UnlockKernel( ))
	#define CALL_Root_LoadModule( _this, plugin, module_name, name_size, cp )                                                                                                                                                                                      ((_this)->LoadModule( plugin, module_name, name_size, cp ))
	#define CALL_Root_GetCustomPropName( _this, out_size, prop_id, buffer, size )                                                                                                                                                                                  ((_this)->GetCustomPropName( out_size, prop_id, buffer, size ))
	#define CALL_Root_StreamSerializeField( _this, container, field, stream, reserved )                                                                                                                                                                            ((_this)->StreamSerializeField( container, field, stream, reserved ))
	#define CALL_Root_RegSerializeField( _this, container, field, registry, path, reserved )                                                                                                                                                                       ((_this)->RegSerializeField( container, field, registry, path, reserved ))
	#define CALL_Root_RegDeserializeField( _this, container, field, registry, path, reserved )                                                                                                                                                                     ((_this)->RegDeserializeField( container, field, registry, path, reserved ))
	#define CALL_Root_RegSerializeEx( _this, object, ser_id, registry, path, callback_func, callback_params )                                                                                                                                                      ((_this)->RegSerializeEx( object, ser_id, registry, path, callback_func, callback_params ))
	#define CALL_Root_RegDeserializeEx( _this, object, registry, path, uniqueId, callback_func, callback_params )                                                                                                                                                  ((_this)->RegDeserializeEx( object, registry, path, uniqueId, callback_func, callback_params ))
	#define CALL_Root_CompareSerializable( _this, str1, str2, ser_id, flags )                                                                                                                                                                                      ((_this)->CompareSerializable( str1, str2, ser_id, flags ))
	#define CALL_Root_FindSerializableDescriptorByName( _this, descriptor, name )                                                                                                                                                                                  ((_this)->FindSerializableDescriptorByName( descriptor, name ))
	#define CALL_Root_AddRefSerializable( _this, object )                                                                                                                                                                                                          ((_this)->AddRefSerializable( object ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iRoot {
		virtual tERROR pr_call RegisterIFace( tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid, const tVOID* internal_table, tDWORD internal_table_size, const tVOID* external_table, tDWORD external_table_size, const tDATA* prop_table, tDWORD prop_table_size, tDWORD memsize, tDWORD flags ) = 0; // -- //rus:Регистрация интерфейса//eng:Registers an interface
		virtual tERROR pr_call UnregisterIFace( tIID iid, tPID pid, tDWORD subtype, tVID vid ) = 0; // -- //rus:разрегистрация интерфейса//eng:Unregisters an interface
		virtual tERROR pr_call FindPlugins( tPID mfinder_pid, const tVOID* param_pool, tDWORD param_pool_size, tDWORD load_flag_set ) = 0; // -- //rus:интегрирование модулей наденных указанным загрузчиком в систему//eng:integrate modules to the system by specific loader
		virtual tERROR pr_call RegisterCustomPropId( tDWORD* result, tSTRING name, tDWORD type ) = 0; // -- //rus:распределяет уникальный идентификатор для пользовательского свойства//eng:allocates unique property id
		virtual tERROR pr_call UnregisterCustomPropId( tSTRING name ) = 0; // -- //rus:освобождает уникальныйидентификатор свойсва//eng:deallocates unique property id
		virtual tERROR pr_call GetCustomPropId( tDWORD* result, tSTRING name, tDWORD type ) = 0; // -- //rus:возвращает, ранее зарегистрированный, идентификатор свойства//eng:returns id of the registered named property identifier
		virtual tERROR pr_call TraceLevelSet( tDWORD op, tIID iid, tPID pid, tDWORD subtype, tDWORD vid, tDWORD level, tDWORD min_level ) = 0; // -- //rus:устанавливает уровень трассировки на определенный интерфейс или плагин//eng:set trace level by interface or/and plugin
		virtual tERROR pr_call GetTracer( hTRACER* result ) = 0; // -- //rus:возвращает текущий трасер//eng:returns current tracer
		virtual tERROR pr_call GetPluginInstance( hPLUGIN* result, tDATA* cookie, tPID pid ) = 0; // -- //rus:поиск объект типа hMODULE по идентификатору плагина//eng:returns plugin by identifier
		virtual tERROR pr_call RegisterIFaceEx( hIFACE* result, tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid, const tVOID* internal_table, tDWORD internal_table_size, const tVOID* external_table, tDWORD external_table_size, const tDATA* prop_table, tDWORD prop_table_size, tDWORD memsize, tDWORD flags, tIID iid_compat, hPLUGIN implemented_module, tDWORD implemented_index ) = 0; // -- //rus:регистрация интерфейса с указанием плагина//eng:Registers an interface with plugin indication
		virtual tERROR pr_call RegisterExportTable( tDWORD* result, const tEXPORT* ex_tbl, tPID exporter_pid ) = 0; // -- //rus:регистрирует таблицу экспортных функций плагина//eng:register export function table of the plugin
		virtual tERROR pr_call RegisterExportFunc( const tFUNC_PTR func_ptr, tDWORD func_cls, tDWORD func_id, tPID exporter_pid ) = 0; // -- //rus:регистрирует экспортную функцию заданного плагина//eng:register export function of the plugin
		virtual tERROR pr_call RegisterExportFuncByName( const tFUNC_PTR func_ptr, tDWORD func_cls, tSTRING func_name, tPID exporter_pid ) = 0; // -- //rus:регистрирует экспортную функцию для заданного плагина по имени//eng:register export function of the plugin by name
		virtual tERROR pr_call ResolveImportTable( tDWORD* result, const tIMPORT* imp_tbl, tPID importer_pid ) = 0; // -- //rus:разрешает таблицу ссылок на экспортные функции//eng:resolve table of references to the export functions
		virtual tERROR pr_call ResolveImportFunc( tFUNC_PTR* result, tDWORD func_cls, tDWORD func_id, tPID importer_pid ) = 0; // -- //rus:разрещает ссылку на экспортную функцию/eng:resolve reference to the export function
		virtual tERROR pr_call ResolveImportFuncByName( tFUNC_PTR* result, tDWORD func_cls, const tSTRING func_name, tPID importer_pid ) = 0; // -- //rus:разрещает ссылку на экспортную функцию по имени/eng:resolve reference to the export function by name
		virtual tERROR pr_call ReleaseImportTable( tDWORD* result, const tIMPORT* imp_tbl, tPID importer_pid ) = 0; // -- //rus:освобождает таблицу ссылок на экспортные функции//eng:releases table of the references
		virtual tERROR pr_call ReleaseImportFunc( tDWORD func_cls, tDWORD func_id, tPID importer_pid ) = 0; // -- //rus:освобождает ссылку на экспортную функцию//eng:releases reference to the export function
		virtual tERROR pr_call ReleaseImportFuncByName( tDWORD func_cls, const tSTRING func_name, tPID importer_pid ) = 0; // -- //rus:освобождает ссылку на экспортную функцию по имени//eng:releases reference to the export function by name
		virtual tERROR pr_call PluginUnloadNotification( hPLUGIN plugin_unloaded ) = 0; // -- //rus:информирование ядра Праги о выгрузке плагина из памяти//eng:outsiders notify root about plugin unloading
		virtual tERROR pr_call PluginLoadNotification( hPLUGIN plugin_loaded ) = 0; // -- //rus:информирование ядра Праги о загрузке плагина в память//eng:outsiders notify root about plugin loading
		virtual tERROR pr_call FreeUnusedPlugins( tPID* pid_array, tDWORD pid_array_count, tFUP_ACTION action, tDWORD during ) = 0; // -- forces unload unsued plugins
		virtual tERROR pr_call SetNotificationSink( tDWORD* id, tSNS_ACTION type, tFUNC_PTR func, tPTR ctx ) = 0; // -- //rus:установить функцию-приемник уведомления о событии kernel'a
		virtual tERROR pr_call FreeNotificationSink( tDWORD id ) = 0; // -- //rus:снять функцию-приемник уведомления о событии kernel'a
		virtual tERROR pr_call CheckIFace( tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid, tDWORD flags, hPLUGIN implemented_module ) = 0; // -- //rus:проверка интерфейса//eng:Check an interface
		virtual tERROR pr_call SetKernelSyncObject( tIID iid, tDWORD pid, tDWORD subtype ) = 0; // -- force kernel to use synchronization object selected by iid, pid, and subtype
		virtual tERROR pr_call GetKernelSyncObject( hOBJECT* sync_object ) = 0; // -- kernel used synchronization object
		virtual tERROR pr_call RegisterSerializableDescriptor( const cSerDescriptor* descriptor ) = 0;
		virtual tERROR pr_call UnregisterSerializableDescriptor( const cSerDescriptor* descriptor ) = 0;
		virtual tERROR pr_call FindSerializableDescriptor( cSerDescriptor** descriptor, tDWORD uniqueId ) = 0;
		virtual tERROR pr_call CreateSerializable( tDWORD unique_id, cSerializable** object ) = 0;
		virtual tERROR pr_call DestroySerializable( cSerializable* object ) = 0;
		virtual tERROR pr_call CopySerializable( cSerializable** dst, const cSerializable* src ) = 0;
		virtual tERROR pr_call StreamSerialize( const cSerializable* object, tDWORD ser_id, tBYTE* buffer, tDWORD size, tDWORD* output_size, tDWORD flags=fSTREAM_PACK_DEFAULT ) = 0; // -- //eng:serialize instanse of serializable structure
		virtual tERROR pr_call StreamDeserialize( cSerializable** object, const tBYTE* buffer, tDWORD size, tDWORD* accepted_size ) = 0; // -- //eng:deserialize instance of serializable structure
		virtual tERROR pr_call RegSerialize( const cSerializable* object, tDWORD ser_id, hREGISTRY registry, const tCHAR* path ) = 0; // -- //eng:serialize instanse of serializable structure
		virtual tERROR pr_call RegDeserialize( cSerializable** object, hREGISTRY registry, const tCHAR* path, tDWORD uniqueId ) = 0; // -- //eng:deserialize instance of serializable structure
		virtual tERROR pr_call LockKernel() = 0;
		virtual tERROR pr_call UnlockKernel() = 0;
		virtual tERROR pr_call LoadModule( hPLUGIN* plugin, const tVOID* module_name, tDWORD name_size, tCODEPAGE cp ) = 0; // -- load module by name
		virtual tERROR pr_call GetCustomPropName( tDWORD* out_size, tPROPID prop_id, tPTR buffer, tDWORD size ) = 0; // -- returns registered custom property name by identifier
		virtual tERROR pr_call StreamSerializeField( const cSerializable* container, const tVOID* field, cOutStream* stream, tDWORD reserved ) = 0; // -- //eng:serialize one field of serializable structure
		virtual tERROR pr_call RegSerializeField( const cSerializable* container, const tVOID* field, hREGISTRY registry, const tCHAR* path, tDWORD reserved ) = 0;
		virtual tERROR pr_call RegDeserializeField( const cSerializable* container, tVOID* field, hREGISTRY registry, const tCHAR* path, tDWORD reserved ) = 0; // -- //eng:deserialize one field of serializable structure
		virtual tERROR pr_call RegSerializeEx( const cSerializable* object, tDWORD ser_id, hREGISTRY registry, const tCHAR* path, tSerializeCallback callback_func, tPTR callback_params ) = 0; // -- //eng:serialize instanse of serializable structure
		virtual tERROR pr_call RegDeserializeEx( cSerializable** object, hREGISTRY registry, const tCHAR* path, tDWORD uniqueId, tSerializeCallback callback_func, tPTR callback_params ) = 0; // -- //eng:deserialize instance of serializable structure
		virtual tERROR pr_call CompareSerializable( const cSerializable* str1, const cSerializable* str2, tDWORD ser_id, tDWORD flags ) = 0;
		virtual tERROR pr_call FindSerializableDescriptorByName( cSerDescriptor** descriptor, const tCHAR* name ) = 0;
		virtual tERROR pr_call AddRefSerializable( cSerializable* object ) = 0; // -- increase reference count of serializable object
	};

	struct pr_abstract cRoot : public iRoot, public iObj {

		OBJ_IMPL( cRoot );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hROOT()   { return (hROOT)this; }

		tERROR pr_call get_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_NAME,buff,size,cp); }
		tERROR pr_call set_pgOBJECT_NAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_NAME,buff,size,cp); }

		tFUNC_PTR pr_call get_pgOUTPUT_FUNC() { return (tFUNC_PTR)getPtr(pgOUTPUT_FUNC); }
		tERROR pr_call set_pgOUTPUT_FUNC( tFUNC_PTR value ) { return setPtr(pgOUTPUT_FUNC,(tPTR)value); }

		tDWORD pr_call get_pgOBJECT_COUNT() { return (tDWORD)getDWord(pgOBJECT_COUNT); }
		tERROR pr_call set_pgOBJECT_COUNT( tDWORD value ) { return setDWord(pgOBJECT_COUNT,(tDWORD)value); }

		tBOOL pr_call get_pgEXCEPTION_CATCHING_AVAILABLE() { return (tBOOL)getBool(pgEXCEPTION_CATCHING_AVAILABLE); }
		tERROR pr_call set_pgEXCEPTION_CATCHING_AVAILABLE( tBOOL value ) { return setBool(pgEXCEPTION_CATCHING_AVAILABLE,(tBOOL)value); }

		tDWORD pr_call get_pgSYNCHRONIZATION_AVAILABLE() { return (tDWORD)getDWord(pgSYNCHRONIZATION_AVAILABLE); }
		tERROR pr_call set_pgSYNCHRONIZATION_AVAILABLE( tDWORD value ) { return setDWord(pgSYNCHRONIZATION_AVAILABLE,(tDWORD)value); }

		tDWORD pr_call get_pgUNLOAD_UNUSED_DURING() { return (tDWORD)getDWord(pgUNLOAD_UNUSED_DURING); }
		tERROR pr_call set_pgUNLOAD_UNUSED_DURING( tDWORD value ) { return setDWord(pgUNLOAD_UNUSED_DURING,(tDWORD)value); }

		tBOOL pr_call get_pgUNLOAD_UNUSED_FORCE() { return (tBOOL)getBool(pgUNLOAD_UNUSED_FORCE); }
		tERROR pr_call set_pgUNLOAD_UNUSED_FORCE( tBOOL value ) { return setBool(pgUNLOAD_UNUSED_FORCE,(tBOOL)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



EXTERN_C hROOT g_root;
  

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __i_root__37b137ba_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end



