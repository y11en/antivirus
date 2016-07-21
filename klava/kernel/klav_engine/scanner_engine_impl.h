// scanner_engine_impl.h
//

#ifndef scanner_engine_impl_h_INCLUDED
#define scanner_engine_impl_h_INCLUDED

#include <klava/klaveng.h>
#include <klava/klavsys.h>

#include <klava/klav_string.h>
#include <klava/klavstl/vector.h>
#include <klava/klavstl/string.h>

#include <klava/klav_dbg.h>
#include <klava/stream_packet.h>

#include <klava/klavdb.h>

// 1.5K default stream packet
#define KLAV_STREAM_DEFAULT_PKT_SIZE 1500
#define KLAV_STREAM_MAX_PKT_SIZE 0x10000

// default stack quota (256K)
#define KLAV_STACK_QUOTA 0x10000000

////////////////////////////////////////////////////////////////

typedef klavstl::string <klav_allocator> klav_stringbuf;

////////////////////////////////////////////////////////////////
// Per-context user data

struct Context_Engine_Data
{
	Context_Engine_Data * m_root_data;
	Context_Engine_Data * m_parent_data;

	Context_Engine_Data ()
		: m_root_data (0), m_parent_data (0) {}
};

////////////////////////////////////////////////////////////////
// KLAV_Scanner_Engine_Impl

struct KLAV_Scanner_Engine_Impl :
		public KLAV_IFACE_IMPL(KLAV_Scanner_Engine),
		public KLAV_IFACE_IMPL(KLAV_Object_System_Manager),
		public KLAV_IFACE_IMPL(KLAV_Proc_Resolver)
{
public:
			KLAV_Scanner_Engine_Impl (KLAV_Alloc *alloc);
	virtual ~KLAV_Scanner_Engine_Impl ();

	virtual void KLAV_CALL destroy ();

	virtual KLAV_ERR KLAV_CALL set_option (
				const char *optname,
				const char *optval
			);

	virtual KLAV_Properties * KLAV_CALL get_properties (
			);

	virtual KLAV_Property_Bag * KLAV_CALL create_property_bag (
			);

	virtual KLAV_ERR KLAV_CALL set_iface (
				uint32_t ifc_id,
				void *ifc
			);

	virtual void * KLAV_CALL get_iface (
				uint32_t ifc_id
			);

	virtual KLAV_ERR KLAV_CALL load (
				const char * db_path,
				KLAV_Properties * props
			);

	virtual KLAV_ERR KLAV_CALL unload (
			);

	virtual KLAV_ERR KLAV_CALL process_file (
				const char * path,
				KLAV_Properties * props
			);

	virtual KLAV_Engine * KLAV_CALL get_engine (
			);

// Object System manager
	virtual KLAV_ERR KLAV_CALL process_subobject (
				KLAV_CONTEXT_TYPE context,
				void * object,
				KLAV_Properties *props
			);

	KLAV_Alloc * allocator () const
		{ return m_allocator; }

	KLAV_ERR scan_object_as_stream (
				KLAV_CONTEXT_TYPE context,
				KLAV_IO_Object *  object
			);

	// Proc resolver
	virtual KLAV_ERR KLAV_CALL resolve_procedure (
			KLAV_CONTEXT_TYPE,
			const KLAV_Proc_Info *proc,
			KLAV_PFN *res
		);

private:
	KLAV_Alloc  * m_allocator;
	KLAV_Engine * m_engine;
	void *        m_hengine_dll;

	klav_string m_engine_name;
	klav_string m_db_folder;
	klav_string m_db_file_name;
	klav_string m_db_base_name;
	klav_string m_debug_dll;
	klav_string m_kdl_dir;

	struct ifc_ref
	{
		void *   ifc_ptr;
		uint32_t ifc_id;

		ifc_ref () : ifc_ptr (0), ifc_id (0) {}
		ifc_ref (uint32_t id, void * ptr)
			: ifc_ptr (ptr), ifc_id (id) {}
	};
	
	typedef klavstl::trivial_vector <ifc_ref, klav_allocator> ifc_ref_array;
	ifc_ref_array m_ifc_refs;

	int  find_ifc_idx (uint32_t id) const;
	void set_ifc_ref (uint32_t id, void *ptr);

//	klav_string m_avp_set;
	uint32_t    m_proc_flags;
	uint32_t	m_stream_pkt_size;
	uint32_t    m_stream_fmt;
	uint32_t    m_heur_level;

	KLAV_Sync_Factory        * m_sync_factory;
	KLAV_Virtual_Memory      * m_vmem;
	KLAV_DB_Loader           * m_db_loader;
	KLAV_DSO_Loader          * m_sys_loader;
	KLAV_IO_Library          * m_io_library;
	KLAV_DSO_Loader          * m_kdl_loader;
	KLAV_Temp_Object_Manager * m_temp_obj_mgr;

	KLAV_DB_Manager          * m_db_manager;

	void *   m_klavdbg_lib;
	KLAV_Debug_API           * m_dbg_api;
	KLAV_Debug_Proc_Resolver * m_dbg_proc_resolver;

	uint32_t m_debug_flags;
	bool     m_old_kfb_loader;
	bool     m_kdl_sys_path;
	bool     m_kdl_sys_ldr;

	void cleanup ();

	bool use_old_kfb_loader ();

	KLAV_ERR do_load (
				const char * db_path,
				KLAV_Properties * props
			);

	KLAV_ERR do_process_object (
			KLAV_IO_Object *  object,
			KLAV_CONTEXT_TYPE parent_ctx,
			KLAV_Properties * props
		);

	KLAV_ERR do_process_stream (
			KLAV_IO_Object * object,
			KLAV_Properties *props
		);

	KLAV_ERR scan_stream_data (
			KLAV_CONTEXT_TYPE context,
			KLAV_IO_Object * io, 
			uint8_t * buf,
			size_t   bufsz,
			uint32_t stream_fmt
		);

	KLAV_ERR send_callback (
			KLAV_CONTEXT_TYPE context,
			KLAV_CALLBACK_CODE code,
			KLAV_Properties *props
		);

	KLAV_ERR load_debug_api (const char * klav_dbg_name);

	void print_msg (KLAV_CONTEXT_TYPE, int level, const char *fmt, ...);
	void print_msg_v (KLAV_CONTEXT_TYPE, int level, const char *fmt, va_list ap);
};

////////////////////////////////////////////////////////////////

#endif // scanner_engine_impl_h_INCLUDED

