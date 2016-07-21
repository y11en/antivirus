// scanner_engine_impl.cpp
//

#include "scanner_engine_impl.h"
#include <klava/klav_utils.h>

#include <string.h>

void append_path (klav_stringbuf& path, const char *name)
{
	if (path.length () == 0)
	{
		path = name;
		return;
	}

	if (name != 0 && name [0] != 0)
	{
		unsigned char c = path [path.length () - 1];
		if (! KLAV_Fname_Is_Path_Separator (c))
			path += KLAV_Fname_Path_Separator ();

		path += name;
	}
}

KLAV_Scanner_Engine_Impl::KLAV_Scanner_Engine_Impl (KLAV_Alloc *alloc)
	: m_allocator (alloc),
	  m_engine (0),
	  m_hengine_dll (0),
	  m_ifc_refs (alloc),
      m_proc_flags (0),
	  m_stream_pkt_size (0),
	  m_stream_fmt (KLAV_STREAM_FORMAT_RAW),
	  m_heur_level (50),
	  m_sync_factory (0),
	  m_vmem (0),
	  m_db_loader (0),
	  m_sys_loader (0),
	  m_io_library (0),
	  m_kdl_loader (0),
	  m_temp_obj_mgr (0),
      m_klavdbg_lib (0),
	  m_dbg_api (0),
	  m_dbg_proc_resolver (0),
	  m_db_manager (0),
	  m_debug_flags (0),
	  m_old_kfb_loader (false),
	  m_kdl_sys_path (false),
	  m_kdl_sys_ldr (false)
{
	m_sync_factory = KLAVSYS_Get_Sync_Factory ();
	m_vmem = KLAVSYS_Get_Virtual_Memory ();
	m_sys_loader = KLAVSYS_Get_DSO_Loader ();
	m_io_library = KLAVSYS_Get_IO_Library ();
	m_temp_obj_mgr = KLAVSYS_Get_Temp_Object_Manager ();

	set_ifc_ref (KLAV_IFACE_Sync_Factory,   static_cast <KLAV_Sync_Factory *> (m_sync_factory));
	set_ifc_ref (KLAV_IFACE_Virtual_Memory, static_cast <KLAV_Virtual_Memory *> (m_vmem));
	set_ifc_ref (KLAV_IFACE_IO_Library,     static_cast <KLAV_IO_Library *> (m_io_library));
	set_ifc_ref (KLAV_IFACE_Temp_Object_Manager, static_cast <KLAV_Temp_Object_Manager *> (m_temp_obj_mgr));
	set_ifc_ref (KLAV_IFACE_Object_System_Manager, static_cast <KLAV_Object_System_Manager *> (this));
	set_ifc_ref (KLAV_IFACE_Proc_Resolver, static_cast <KLAV_Proc_Resolver*> (this));
}

KLAV_Scanner_Engine_Impl::~KLAV_Scanner_Engine_Impl ()
{
	cleanup ();
}

void KLAV_Scanner_Engine_Impl::cleanup ()
{
	if (m_engine != 0)
	{
		m_engine->unload ();
	}

	if (m_db_manager != 0)
	{
		KLAV_DBM_Close (m_db_manager);
	}

	if (m_db_loader != 0)
	{
		KLAVSYS_Close_DB_File (m_db_loader);
		m_db_loader = 0;
	}
	if (m_dbg_api != 0)
	{
		if (m_dbg_proc_resolver != 0)
			m_dbg_api->destroy_dbg_proc_resolver (m_dbg_proc_resolver);

		m_dbg_api->destroy ();
		m_dbg_api = 0;
	}

	m_dbg_proc_resolver = 0;
	
	if (m_klavdbg_lib != 0)
	{
		if (m_sys_loader != 0)
			m_sys_loader->dl_close (m_klavdbg_lib);

		m_klavdbg_lib = 0;
	}

	if (m_engine != 0)
	{
		m_engine->destroy ();
		m_engine = 0;
	}

	if (m_kdl_loader != 0 && m_hengine_dll != 0)
	{
		m_kdl_loader->dl_close (m_hengine_dll);
		m_hengine_dll = 0;
	}

	if (m_db_manager != 0)
	{
		KLAV_DBM_Destroy (m_db_manager);
		m_db_manager = 0;
	}

	if (m_kdl_loader != 0)
	{
		KLAV_KDL_Loader_Destroy (m_kdl_loader);
		m_kdl_loader = 0;
	}
	
	m_db_folder.clear ();
	m_db_file_name.clear ();
	m_db_base_name.clear ();
}

void KLAV_CALL KLAV_Scanner_Engine_Impl::destroy ()
{
	KLAV_DELETE (this, allocator ());
}

KLAV_ERR KLAV_CALL KLAV_Scanner_Engine_Impl::set_option (
			const char *optname,
			const char *optval
		)
{
	KLAV_ERR err = KLAV_OK;
	bool is_dbg_opt = false;

	if (m_dbg_api != 0)
	{
		err = m_dbg_api->set_debug_option (optname, optval);
		if (err == KLAV_ENOTFOUND)
			err = KLAV_OK;
		else if (KLAV_FAIL (err))
			return err;
		else 
			is_dbg_opt = true;
	}

	if (strcmp (optname, "debug_api") == 0)
	{
		if (optval == 0)
			optval = KLAV_DSO_NAME("klavdbg");

		if (m_dbg_api != 0)
			return KLAV_EALREADY;

		err = load_debug_api (optval);
		if (KLAV_FAILED (err))
			return err;
	}
	else if (strcmp (optname, "debug_flags") == 0)
	{
		if (optval == 0)
		{
			m_debug_flags = 0xFFFF;
		}
		else
		{
			m_debug_flags = (uint32_t) strtoul (optval, 0, 0);
		}
	}
	else if (strcmp (optname, "perf") == 0)
	{
		klav_bool_t f = 1;
		if (optval != 0)
		{
			f = (strtoul (optval, 0, 0) != 0);
		}
		if (f)
			m_proc_flags |= KLAV_PF_PERF_STAT;
		else
			m_proc_flags &= ~KLAV_PF_PERF_STAT;
	}
	else if (strcmp (optname, "stream") == 0)
	{
		if (optval != 0)
		{
			m_stream_pkt_size = strtoul (optval, 0, 0);

			if (m_stream_pkt_size > KLAV_STREAM_MAX_PKT_SIZE)
				return KLAV_EINVAL;
		}
		m_proc_flags |= KLAV_PF_STREAM;
	}
	else if (strcmp (optname, "stream_fmt") == 0)
	{
		if (optval == 0 || optval [0] == 0)
			return KLAV_EINVAL;

		m_stream_fmt = strtoul (optval, 0, 0);
		m_proc_flags |= KLAV_PF_STREAM;
	}
	else if (strcmp (optname, "false_alarms") == 0)
	{
		if (optval != 0)
		{
			if (strtoul (optval, 0, 0) == 0)
			{
				m_proc_flags |= KLAV_PF_NO_FA;
			}
		}
	}
	else if (strcmp (optname, "heur_level") == 0)
	{
		if (optval == 0 || *optval == 0)
			return KLAV_EINVAL;

		m_heur_level = (uint32_t) (strtoul (optval, 0, 0));
	}
	else if (strcmp (optname, "debug_dll") == 0)
	{
		if (optval == 0)
			return KLAV_EINVAL;

		if (! m_debug_dll.assign (optval, allocator ()))
			return KLAV_ENOMEM;
	}
	else if (strcmp (optname, "engine") == 0)
	{
		if (optval == 0)
			return KLAV_EINVAL;

		if (! m_engine_name.assign (optval, allocator ()))
			return KLAV_ENOMEM;
	}
	else if (strcmp (optname, "kdl_dir") == 0)
	{
		if (optval == 0 || optval [0] == 0)
		{
			m_kdl_sys_path = true;
		}
		else
		{
			if (! m_kdl_dir.assign (optval, allocator ()))
				return KLAV_ENOMEM;
		}
	}
	else if (strcmp (optname, "old_kfb") == 0)
	{
		if (optval == 0 || strtoul (optval, 0, 0) != 0)
		{
			m_old_kfb_loader = true;
		}
	}
	else if (strcmp (optname, "sys_ldr") == 0)
	{
		if (optval == 0 || strtoul (optval, 0, 0) != 0)
		{
			m_kdl_sys_ldr = true;
		}
	}
	else
	{
		if (! is_dbg_opt)
			return KLAV_ENOTFOUND;
	}

	return KLAV_OK;
}

KLAV_Properties * KLAV_CALL KLAV_Scanner_Engine_Impl::get_properties ()
{
	return m_engine->get_properties ();
}

KLAV_Property_Bag * KLAV_CALL KLAV_Scanner_Engine_Impl::create_property_bag ()
{
	return m_engine->create_property_bag ();
}

int KLAV_Scanner_Engine_Impl::find_ifc_idx (uint32_t ifc_id) const
{
	size_t cnt = m_ifc_refs.size ();
	for (size_t i = 0; i < cnt; ++i)
	{
		if (m_ifc_refs [i].ifc_id == ifc_id)
			return (int) i;
	}
	
	return -1;
}

void KLAV_Scanner_Engine_Impl::set_ifc_ref (uint32_t ifc_id, void *ifc_ptr)
{
	int idx = find_ifc_idx (ifc_id);
	if (idx < 0)
		m_ifc_refs.push_back (ifc_ref (ifc_id, ifc_ptr));
	else
		m_ifc_refs [idx].ifc_ptr = ifc_ptr;
}

KLAV_ERR KLAV_CALL KLAV_Scanner_Engine_Impl::set_iface (uint32_t ifc_id, void *ifc_ptr)
{
	switch (ifc_id)
	{
	case KLAV_IFACE_ID (Alloc):
	case KLAV_IFACE_ID (Sync_Factory):
	case KLAV_IFACE_ID (DB_Loader):
		return KLAV_EINVAL;
	default:
		break;
	}

	if (m_engine != 0)
	{
		KLAV_ERR err = m_engine->set_iface (ifc_id, ifc_ptr);
		if (KLAV_FAILED (err))
			return err;
	}
	else
	{
		set_ifc_ref (ifc_id, ifc_ptr);
	}

	return KLAV_OK;
}

void * KLAV_CALL KLAV_Scanner_Engine_Impl::get_iface (uint32_t ifc_id)
{
	if (m_engine != 0)
		return m_engine->get_iface (ifc_id);

	int idx = find_ifc_idx (ifc_id);
	if (idx < 0)
		return 0;

	return m_ifc_refs [idx].ifc_ptr;
}

KLAV_ERR KLAV_CALL KLAV_Scanner_Engine_Impl::load (const char * db_path, KLAV_Properties *props)
{
	KLAV_ERR err = KLAV_OK;

	err = do_load (db_path, props);
	if (KLAV_FAILED (err))
	{
		cleanup ();
		return err;
	}

	return KLAV_OK;
}

static bool str_eqi (const char *s1, const char *s2)
{
	for (;;)
	{
		unsigned char c1 = *s1++;
		unsigned char c2 = *s2++;
		if (c1 >= 'A' && c1 <= 'Z') c1 += ('a' - 'A');
		if (c2 >= 'A' && c2 <= 'Z') c2 += ('a' - 'A');
		if (c1 != c2)
			return false;
		if (*s1 == 0)
			break;
	}
	return true;
}

bool KLAV_Scanner_Engine_Impl::use_old_kfb_loader ()
{
	return m_old_kfb_loader;
}

KLAV_ERR KLAV_Scanner_Engine_Impl::do_load (const char * db_path, KLAV_Properties *props)
{
	KLAV_ERR err = KLAV_OK;

	if (db_path == 0 || db_path [0] == 0)
		return KLAV_EINVAL;

	// split db path into folder and name
	size_t fname_off = KLAV_Fname_File_Offset (db_path);
	const char * db_fname = db_path + fname_off;
	size_t ext_off = KLAV_Fname_Ext_Offset (db_fname);

	if (fname_off > 0)
		m_db_folder.assign (db_path, fname_off, allocator ());
	else
		m_db_folder.assign (".", allocator ());
		
	m_db_file_name.assign (db_fname, allocator ());
	m_db_base_name.assign (db_fname, ext_off, allocator ());

	// initialize database manager
	KLAV_DBM_Init_Parms dbm_parms;
	memset (& dbm_parms, 0, sizeof (dbm_parms));
	dbm_parms.struct_size = sizeof (dbm_parms);
	dbm_parms.allocator  = m_allocator;
	dbm_parms.io_library = m_io_library;
	dbm_parms.tracer = static_cast <KLAV_Message_Printer *> (get_iface (KLAV_IFACE_Message_Printer));
	err = KLAV_DBM_Create (& dbm_parms, & m_db_manager);
	if (KLAV_FAIL (err))
	{
		print_msg (0, KLAV_TRACE_LEVEL_ERROR, "error creating DB manager");
		return err;
	}

	err = KLAV_DBM_Open (m_db_manager, m_db_folder.c_str ());
	if (KLAV_FAIL (err))
	{
		print_msg (0, KLAV_TRACE_LEVEL_ERROR, "error initializing database: %s", m_db_folder.c_str ());
		return err;
	}

	klav_stringbuf engine_name (m_engine_name.c_str (), allocator ());
	if (engine_name.length () == 0)
		engine_name = m_db_base_name.c_str ();

	if (KLAV_Fname_Ext_Offset (engine_name.c_str ()) == engine_name.length ())
		engine_name += KLAV_MOD_SUFFIX;

	KLAV_KDL_Loader_Parms ldr_parms;
	memset (& ldr_parms, 0, sizeof (ldr_parms));

	ldr_parms.m_alloc = m_allocator;
	ldr_parms.m_iolib = m_io_library;
	ldr_parms.m_sysldr = m_sys_loader;
	ldr_parms.m_vmem = m_vmem;
	if (m_kdl_sys_path || m_kdl_dir.length () != 0)
		ldr_parms.m_dbdir = m_kdl_dir.c_str ();
	else
		ldr_parms.m_dbdir = m_db_folder.c_str ();

	if (ldr_parms.m_dbdir == 0 || ldr_parms.m_dbdir [0] == 0 || m_kdl_sys_ldr)
		ldr_parms.m_flags |= KLAV_KDL_LOADER_SYSTEM;

	err = KLAV_KDL_Loader_Create (& ldr_parms, & m_kdl_loader);
	if (KLAV_FAIL (err))
	{
		// TODO: trace...
		return err;
	}

	// load engine
	err = m_kdl_loader->dl_open (engine_name.c_str (), & m_hengine_dll);
	if (KLAV_FAIL (err))
	{
		print_msg (0, KLAV_TRACE_LEVEL_ERROR, "error loading engine: %s", engine_name.c_str ());
		return err;
	}

	PFN_KLAV_Engine_Create pfn_creator = (PFN_KLAV_Engine_Create)
		 m_kdl_loader->dl_sym (m_hengine_dll, KLAV_ENGINE_CREATE_NAME);
	if (pfn_creator == 0)
		return KLAV_EINVAL;

	err = pfn_creator (KLAV_ENGINE_VERSION_CURRENT, m_allocator, & m_engine);
	if (KLAV_FAILED (err) || m_engine == 0)
		return (err == KLAV_OK) ? KLAV_EUNKNOWN : err;

	// set engine interfaces
	size_t ifc_cnt = m_ifc_refs.size ();
	for (size_t ifc_idx = 0; ifc_idx < ifc_cnt; ++ifc_idx)
	{
		const ifc_ref& ifc = m_ifc_refs [ifc_idx];
		err = m_engine->set_iface (ifc.ifc_id, ifc.ifc_ptr);
		if (KLAV_FAILED (err))
			return err;
	}

	KLAV_SET_IFACE (m_engine, Debug_API,  m_dbg_api);
	KLAV_SET_IFACE (m_engine, DB_Manager, m_db_manager);
	KLAV_SET_IFACE (m_engine, DSO_Loader, m_kdl_loader);

	KLAV_Property_Bag_Holder props2 (m_engine->create_property_bag ());
	if (! props2)
		return KLAV_ENOMEM;
 
	if (props != 0)
	{
		err = props2->set_properties (props);
		if (KLAV_FAILED (err))
			return err;
	}

	if (m_dbg_api != 0 && m_debug_dll.length () != 0)
	{
		KLAV_Message_Printer * msg_printer = KLAV_GET_IFACE (m_engine, Message_Printer);

		m_dbg_proc_resolver = m_dbg_api->create_dbg_proc_resolver (
								m_kdl_loader,
								m_io_library,
								msg_printer,
								m_debug_dll.c_str (),
								0);
		if (m_dbg_proc_resolver == 0)
			return KLAV_EUNKNOWN;

		KLAV_SET_IFACE (m_engine, Debug_Proc_Resolver, m_dbg_proc_resolver);
	}

	if (use_old_kfb_loader ())
	{
		err = KLAVSYS_Open_DB_File (db_path, & m_db_loader);
		if (KLAV_FAILED (err))
		{
			print_msg (0, KLAV_TRACE_LEVEL_ERROR, "error opening database: %s", db_path);
			return err;
		}

		KLAV_SET_IFACE (m_engine, DB_Loader, m_db_loader);
	}

	m_proc_flags |= KLAV_Get_Property_Int (props2, KLAV_PROPID_PROC_FLAGS);
	KLAV_Set_Property_Int (props2, KLAV_PROPID_PROC_FLAGS, m_proc_flags);
	KLAV_Set_Property_Int (props2, KLAV_PROPID_DEBUG_FLAGS, m_debug_flags);
	KLAV_Set_Property_Str (props2, KLAV_PROPID_DB_NAME, m_db_file_name.c_str ());
//	KLAV_Set_Property_Int (props2, KLAV_PROPID_DB_FLAGS, 0);

	err = m_engine->load (props2);
	if (KLAV_FAILED (err))
	{
		print_msg (0, KLAV_TRACE_LEVEL_ERROR, "error loading engine");
		return err;
	}

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Scanner_Engine_Impl::unload ()
{
	cleanup ();

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Scanner_Engine_Impl::process_file (
			const char * path,
			KLAV_Properties * props
		)
{
	if (path == 0 || path [0] == 0)
		return KLAV_EINVAL;

	KLAV_ERR err = KLAV_OK;

	KLAV_IO_Library * iolib = KLAV_GET_IFACE (m_engine, IO_Library);
	if (iolib == 0)
		return KLAV_ENOINIT;

	KLAV_IO_Object_Holder io_object;
	err = iolib->create_file (0, path, KLAV_IO_READONLY, KLAV_FILE_OPEN_EXISTING, 0, &io_object.ptr ());
	if (KLAV_FAILED (err))
		return err;

	const char *obj_path = 0;
		
	if (props != 0)
	{
		obj_path = KLAV_Get_Property_Str (props, KLAV_PROPID_OBJECT_PATH);
	}

	if (obj_path == 0)
		obj_path = path;

	KLAV_STATIC_PROPERTIES_CHAIN (props2, props);
	KLAV_STATIC_PROPERTY (props2, KLAV_PROPID_OBJECT_PATH, obj_path);

	KLAV_Set_Property_Int (io_object->get_properties (), KLAV_PROPID_HEURISTIC_LEVEL, m_heur_level);

	if ((m_proc_flags & KLAV_PF_STREAM) != 0)
	{
		// stream mode
		err = do_process_stream (io_object.get (), & props2);
	}
	else
	{
		// file mode
		err = do_process_object (io_object.get (), 0, & props2);
	}

	return err;
}

KLAV_Engine * KLAV_CALL KLAV_Scanner_Engine_Impl::get_engine ()
{
	return m_engine;
}

KLAV_ERR KLAV_CALL KLAV_Scanner_Engine_Impl::resolve_procedure (
		KLAV_CONTEXT_TYPE,
		const KLAV_Proc_Info *proc_info,
		KLAV_PFN *res
	)
{
	*res = 0;

	if (proc_info->type == KLAV_PROCTYPE_APP)
	{
		uint32_t id = proc_info->parm1.num;

		switch (id)
		{
		case KLAV_APP_SYMBOL_ID_memset:
			*res = KLAV_PFN(memset);
			return KLAV_OK;
		case KLAV_APP_SYMBOL_ID_memcpy:
			*res = KLAV_PFN(memcpy);
			return KLAV_OK;
		case KLAV_APP_SYMBOL_ID_memmove:
			*res = KLAV_PFN(memmove);
			return KLAV_OK;
		case KLAV_APP_SYMBOL_ID_memcmp:
			*res = KLAV_PFN(memcmp);
			return KLAV_OK;
		}
	}

	return KLAV_ENOTFOUND;
}

KLAV_ERR KLAV_CALL KLAV_Scanner_Engine_Impl::process_subobject (
			KLAV_CONTEXT_TYPE context,
			void *   object,
			KLAV_Properties *props
		)
{
	return do_process_object ((KLAV_IO_Object *)object, context, props);
}

KLAV_ERR KLAV_Scanner_Engine_Impl::do_process_object (
			KLAV_IO_Object *  object,
			KLAV_CONTEXT_TYPE parent_ctx,
			KLAV_Properties *props
		)
{
	KLAV_ERR err = KLAV_OK;

	KLAV_CONTEXT_TYPE context = 0;

	err = m_engine->create_context (props, & context);
	if (KLAV_FAILED (err))
		return err;

	Context_Engine_Data frame_data;
	frame_data.m_root_data = & frame_data;

	if (parent_ctx != 0)
	{
		klav_propval_t val;
		m_engine->get_context_property (parent_ctx, KLAV_PROPID_USER_DATA, 0, &val);
		if (val.p != 0)
		{
			Context_Engine_Data * parent_data = (Context_Engine_Data *) val.p;

			// check stack
			const uint8_t * stk_beg = (const uint8_t *) parent_data->m_root_data;
			const uint8_t * stk_top = (const uint8_t *) & frame_data;
			
			size_t stk_size = (stk_beg < stk_top) ? stk_top - stk_beg : stk_beg - stk_top;
			
			if (stk_size > KLAV_STACK_QUOTA)
			{
				// bail out with error
				// TODO: report something
				return KLAV_EUNEXPECTED;
			}

			frame_data.m_root_data = parent_data->m_root_data;
			frame_data.m_parent_data = parent_data;
		}
	}
	
	{
		klav_propval_t val;
		val.p = & frame_data;
		m_engine->set_context_property (context, KLAV_PROPID_USER_DATA, 0, & val);
	}

	err = m_engine->activate_context (context, object, props);
	if (KLAV_FAILED (err))
	{
		m_engine->destroy_context (context);
		return err;
	}

	{
		klav_propval_t obj_path;
		m_engine->get_context_property (context, KLAV_PROPID_OBJECT_PATH, 0, & obj_path);

		KLAV_Properties * io_props = object->get_properties ();

		const char * transformer_name = KLAV_Get_Property_Str (io_props, KLAV_PROPID_TRANSFORMER_NAME);
		if (transformer_name == 0 && props != 0)
			transformer_name = KLAV_Get_Property_Str (props, KLAV_PROPID_TRANSFORMER_NAME);

		KLAV_STATIC_PROPERTIES (props2);
		KLAV_STATIC_PROPERTY (props2, KLAV_PROPID_OBJECT_PATH, obj_path);
		KLAV_STATIC_PROPERTY (props2, KLAV_PROPID_TRANSFORMER_NAME, transformer_name);

		err = send_callback (context, KLAV_CALLBACK_OBJECT_BEGIN, &props2);
	}
	
	if (err == KLAV_OK)
	{
		err = m_engine->process_action (context, "PROCESS_OBJECT", 0, props);
	}
	else
	{
		if (err == KLAV_ACTION_SKIP)
			err = KLAV_OK;
	}

	{
		klav_propval_t obj_path;
		m_engine->get_context_property (context, KLAV_PROPID_OBJECT_PATH, 0, &obj_path);

		KLAV_STATIC_PROPERTIES (props2);
		KLAV_STATIC_PROPERTY (props2, KLAV_PROPID_OBJECT_PATH, obj_path);

		const char * xform_name = (props == 0) ? 0 : KLAV_Get_Property_Str(props, KLAV_PROPID_TRANSFORMER_NAME);
		KLAV_STATIC_PROPERTY (props2, KLAV_PROPID_TRANSFORMER_NAME, xform_name);

		err = send_callback (context, KLAV_CALLBACK_OBJECT_DONE, &props2);
	}

	m_engine->deactivate_context (context);
	m_engine->destroy_context (context);

	return err;
}

KLAV_ERR KLAV_Scanner_Engine_Impl::send_callback (
			KLAV_CONTEXT_TYPE context,
			KLAV_CALLBACK_CODE code,
			KLAV_Properties * props
		)
{
	KLAV_Callback_Handler * pcb = KLAV_GET_IFACE (m_engine, Callback_Handler);
	if (pcb == 0)
		return KLAV_OK;
	
	KLAV_STATIC_PROPERTIES_CHAIN (props2, props);
	if (props == 0)
		props = & props2;

	return pcb->handle_callback (context, code, props);
}

KLAV_ERR KLAV_Scanner_Engine_Impl::load_debug_api (const char * klavdbg_name)
{
	KLAV_ERR err = KLAV_OK;

	if (m_dbg_api != 0)
		return KLAV_OK;

	if (m_sys_loader == 0)
		return KLAV_ENOTIMPL;

	if (m_klavdbg_lib == 0)
	{
		err = m_sys_loader->dl_open (klavdbg_name, & m_klavdbg_lib);
		if (KLAV_FAILED (err))
		{
			print_msg (0, KLAV_TRACE_LEVEL_ERROR, "error loading module: %s", klavdbg_name);
			return err;
		}
	}

	PFN_KLAV_Debug_API_Create pfn = (PFN_KLAV_Debug_API_Create)
		m_sys_loader->dl_sym (m_klavdbg_lib, KLAV_DEBUG_API_CREATE_NAME);
	if (pfn == 0)
	{
		print_msg (0, KLAV_TRACE_LEVEL_ERROR, "invalid module exports: %s", klavdbg_name);
		m_sys_loader->dl_close (m_klavdbg_lib);
		m_klavdbg_lib = 0;
		return KLAV_ENOTIMPL;
	}

	err = pfn (allocator (), & m_dbg_api);
	return err;
}

void KLAV_Scanner_Engine_Impl::print_msg (KLAV_CONTEXT_TYPE ctx, int level, const char *fmt, ...)
{
	va_list ap;
	va_start (ap, fmt);
	print_msg_v (ctx, level, fmt, ap);
	va_end (ap);
}

void KLAV_Scanner_Engine_Impl::print_msg_v (KLAV_CONTEXT_TYPE ctx, int level, const char *fmt, va_list ap)
{
	KLAV_Message_Printer * msg_printer = (KLAV_Message_Printer *) get_iface (KLAV_IFACE_Message_Printer);

	if (msg_printer != 0)
	{
		msg_printer->print_message_v (ctx, level, fmt, ap);
	}
}

