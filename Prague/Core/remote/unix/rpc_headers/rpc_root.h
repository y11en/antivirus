#if !defined (_RPC_ROOT_H_)
#define _RPC_ROOT_H_
#include <prague.h>
#include <iface/i_root.h>
#include <rpc/praguerpc.h>

using PragueRPC::ConnectionID;

extern PragueRPC::Interface PRrpc_root_ServerIfHandle;


typedef tDWORD          tFUP_ACTION;

typedef rpcOBJECT	rpcROOT;
typedef rpcOBJECT	rpcTRACER;
typedef rpcOBJECT	rpcPLUGIN;

extern "C" {
  //
  // Client prototypes
  //
  // ----------- Interface management ----------
  tERROR Root_RegisterIFace            ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid, /* [in, size_is(internal_table_size)] */ const tPTR* internal_table, tDWORD internal_table_size, /* [in, size_is(external_table_size)] */ const tPTR* external_table, tDWORD external_table_size, /* [in, size_is(prop_table_size*mPROPERTY_TABLE_RECORD_LEN)] */ const tDATA* prop_table, tDWORD prop_table_size, tDWORD memsize, tDWORD flags ); // -- Registers an interface;
  tERROR Root_UnregisterIFace          ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tIID iid, tPID pid, tDWORD subtype, tVID vid ); // -- Unregisters an interface;
  tERROR Root_RegisterIFaceEx          ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid, /* [in, size_is(internal_table_size)] */ const tPTR* internal_table, tDWORD internal_table_size, /* [in, size_is(external_table_size)] */ const tPTR* external_table, tDWORD external_table_size, /* [in, size_is(prop_table_size*mPROPERTY_TABLE_RECORD_LEN)] */ const tDATA* prop_table, tDWORD prop_table_size, tDWORD memsize, tDWORD flags, tIID iid_compat, rpcPLUGIN implemented_module, tDWORD implemented_index ); // -- Registers an interface;

  // ----------- Plugin management ----------
  tERROR Root_FindPlugins              ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tPID mfinder_pid, const tPTR param_pool, tDWORD param_pool_size, tDWORD loadflags ); // -- forces specific loader to find and autoload modules, register interfaces and load native or prague plugins by flags;
  tERROR Root_PluginUnloadNotification ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, rpcPLUGIN plugin_unloaded ); // -- outsiders notify root about plugin unloading;
  tERROR Root_PluginLoadNotification   ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, rpcPLUGIN plugin_loaded ); // -- outsiders notify root about plugin loading;
  tERROR Root_FreeUnusedPlugins        ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, /* [in, size_is(pid_array_count)] */ tPID* pid_array, tDWORD pid_array_count, tFUP_ACTION action ); // -- forces unload unsued plugins;
  tERROR Root_GetPluginInstance        ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, /* [mout] */ rpcPLUGIN* result, /* [in, mout] */ tDWORD* cookie, tPID pid ); // -- returns module matched the criteria;

  // ----------- Custom property methods ----------
  tERROR Root_RegisterCustomPropId     ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, /* [mout] */ tDWORD* result, tSTRING name, tDWORD type ); // -- allocates unique property id;
  tERROR Root_UnregisterCustomPropId   ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tSTRING name ); // -- deallocates unique property id;
  tERROR Root_GetCustomPropId          ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, /* [mout] */ tDWORD* result, tSTRING name, tDWORD type ); // -- returns id of the named property identifier;
  tERROR Root_SyncCustomPropValue      ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tPROPID prop_id ); // -- synchronize custom property values;

  // ----------- Tracing methods ----------
  tERROR Root_TraceLevelSet            ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tDWORD op, tIID iid, tPID pid, tDWORD subtype, tDWORD vid, tDWORD level, tDWORD min_level ); // -- set trace level by interface or/and by pid;
  tERROR Root_GetTracer                ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, /* [mout] */ rpcTRACER* result );           // -- returns current tracer;

  // ----------- import/export management ----------
  tERROR Root_RegisterExportTable      ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tDWORD* result, const tEXPORT* ex_tbl, tPID exporter_pid ); // -- register export function table;
  tERROR Root_RegisterExportFunc       ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, const tFUNC_PTR func_ptr, tDWORD func_cls, tDWORD func_id, tPID exporter_pid ); // -- register single function pointer;
  tERROR Root_RegisterExportFuncByName ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, const tFUNC_PTR func_ptr, tDWORD func_cls, tSTRING func_name, tPID exporter_pid ); // -- register single function pointer;
  tERROR Root_ResolveImportTable       ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tDWORD* result, const tIMPORT* imp_tbl, tPID importer_pid ); // -- resolve import table;
  tERROR Root_ResolveImportFunc        ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tFUNC_PTR* result, tDWORD func_cls, tDWORD func_id, tPID importer_pid ); // -- resolve single func pointer;
  tERROR Root_ResolveImportFuncByName  ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tFUNC_PTR* result, tDWORD func_cls, const tSTRING func_name, tPID importer_pid ); // -- resolve single func pointer;
  tERROR Root_ReleaseImportTable       ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tDWORD* result, const tIMPORT* imp_tbl, tPID importer_pid ); // -- releases import reference;
  tERROR Root_ReleaseImportFunc        ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tDWORD func_cls, tDWORD func_id, tPID importer_pid ); // -- releases single func pointer;
  tERROR Root_ReleaseImportFuncByName  ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tDWORD func_cls, const tSTRING func_name, tPID importer_pid ); // -- releases single func pointer;

  //
  // Server prototypes
  //

  // ----------- Interface management ----------
  tERROR PRRoot_RegisterIFace            ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid, /* [in, size_is(internal_table_size)] */ const tPTR* internal_table, tDWORD internal_table_size, /* [in, size_is(external_table_size)] */ const tPTR* external_table, tDWORD external_table_size, /* [in, size_is(prop_table_size*mPROPERTY_TABLE_RECORD_LEN)] */ const tDATA* prop_table, tDWORD prop_table_size, tDWORD memsize, tDWORD flags ); // -- Registers an interface;
  tERROR PRRoot_UnregisterIFace          ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tIID iid, tPID pid, tDWORD subtype, tVID vid ); // -- Unregisters an interface;
  tERROR PRRoot_RegisterIFaceEx          ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tIID iid, tPID pid, tDWORD subtype, tVERSION version, tVID vid, /* [in, size_is(internal_table_size)] */ const tPTR* internal_table, tDWORD internal_table_size, /* [in, size_is(external_table_size)] */ const tPTR* external_table, tDWORD external_table_size, /* [in, size_is(prop_table_size*mPROPERTY_TABLE_RECORD_LEN)] */ const tDATA* prop_table, tDWORD prop_table_size, tDWORD memsize, tDWORD flags, tIID iid_compat, rpcPLUGIN implemented_module, tDWORD implemented_index ); // -- Registers an interface;

  // ----------- Plugin management ----------
  tERROR PRRoot_FindPlugins              ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tPID mfinder_pid, const tPTR param_pool, tDWORD param_pool_size, tDWORD loadflags ); // -- forces specific loader to find and autoload modules, register interfaces and load native or prague plugins by flags;
  tERROR PRRoot_PluginUnloadNotification ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, rpcPLUGIN plugin_unloaded ); // -- outsiders notify root about plugin unloading;
  tERROR PRRoot_PluginLoadNotification   ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, rpcPLUGIN plugin_loaded ); // -- outsiders notify root about plugin loading;
  tERROR PRRoot_FreeUnusedPlugins        ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, /* [in, size_is(pid_array_count)] */ tPID* pid_array, tDWORD pid_array_count, tFUP_ACTION action ); // -- forces unload unsued plugins;
  tERROR PRRoot_GetPluginInstance        ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, /* [mout] */ rpcPLUGIN* result, /* [in, mout] */ tDWORD* cookie, tPID pid ); // -- returns module matched the criteria;

  // ----------- Custom property methods ----------
  tERROR PRRoot_RegisterCustomPropId     ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, /* [mout] */ tDWORD* result, tSTRING name, tDWORD type ); // -- allocates unique property id;
  tERROR PRRoot_UnregisterCustomPropId   ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tSTRING name ); // -- deallocates unique property id;
  tERROR PRRoot_GetCustomPropId          ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, /* [mout] */ tDWORD* result, tSTRING name, tDWORD type ); // -- returns id of the named property identifier;
  tERROR PRRoot_SyncCustomPropValue      ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tPROPID prop_id ); // -- synchronize custom property values;

  // ----------- Tracing methods ----------
  tERROR PRRoot_TraceLevelSet            ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tDWORD op, tIID iid, tPID pid, tDWORD subtype, tDWORD vid, tDWORD level, tDWORD min_level ); // -- set trace level by interface or/and by pid;
  tERROR PRRoot_GetTracer                ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, /* [mout] */ rpcTRACER* result );           // -- returns current tracer;

  // ----------- import/export management ----------
  tERROR PRRoot_RegisterExportTable      ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tDWORD* result, const tEXPORT* ex_tbl, tPID exporter_pid ); // -- register export function table;
  tERROR PRRoot_RegisterExportFunc       ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, const tFUNC_PTR func_ptr, tDWORD func_cls, tDWORD func_id, tPID exporter_pid ); // -- register single function pointer;
  tERROR PRRoot_RegisterExportFuncByName ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, const tFUNC_PTR func_ptr, tDWORD func_cls, tSTRING func_name, tPID exporter_pid ); // -- register single function pointer;
  tERROR PRRoot_ResolveImportTable       ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tDWORD* result, const tIMPORT* imp_tbl, tPID importer_pid ); // -- resolve import table;
  tERROR PRRoot_ResolveImportFunc        ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tFUNC_PTR* result, tDWORD func_cls, tDWORD func_id, tPID importer_pid ); // -- resolve single func pointer;
  tERROR PRRoot_ResolveImportFuncByName  ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tFUNC_PTR* result, tDWORD func_cls, const tSTRING func_name, tPID importer_pid ); // -- resolve single func pointer;
  tERROR PRRoot_ReleaseImportTable       ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tDWORD* result, const tIMPORT* imp_tbl, tPID importer_pid ); // -- releases import reference;
  tERROR PRRoot_ReleaseImportFunc        ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tDWORD func_cls, tDWORD func_id, tPID importer_pid ); // -- releases single func pointer;
  tERROR PRRoot_ReleaseImportFuncByName  ( /* [in] */ ConnectionID IDL_handle, rpcROOT _this, tDWORD func_cls, const tSTRING func_name, tPID importer_pid ); // -- releases single func pointer;
}

#endif //_RPC_ROOT_H_
