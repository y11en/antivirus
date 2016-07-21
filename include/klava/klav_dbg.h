// klav_dbg.h
//
// Debug utilites
//
//

#ifndef klav_dbg_h_INCLUDED
#define klav_dbg_h_INCLUDED

#include <klava/klavdef.h>
#include <klava/klav_props.h>

#ifdef KLAVDBG_EXPORTS
# define KLAVDBG_API KLAV_DLL_EXPORT
#else  // KLAVDBG_EXPORTS
# define KLAVDBG_API
#endif // KLAVDBG_EXPORTS

////////////////////////////////////////////////////////////////
// Debug event group filter mask

enum
{
	KLAV_DEBUG_F_EVT_INTERNAL = 0x01,  // internal events
	KLAV_DEBUG_F_EVT_PROC     = 0x02,  // procedure calls
	KLAV_DEBUG_F_EVT_SCAN     = 0x10,  // signature scan / region manager events
	KLAV_DEBUG_F_EVT_ACTIONS  = 0x40,  // actions
	KLAV_DEBUG_F_EVT_OBJSYS   = 0x80,  // object systems
};

////////////////////////////////////////////////////////////////
// Debug event class: Internal events

struct KLAV_Internal_Event_Info
{
	uint32_t     m_id;   // event/procedure ID
	const char * m_name; // event/procedure name (if available)
};

#define KLAV_DEBUG_CLASS_INTERNAL    0x01

// debug event: LB event (info: KLAV_Internal_Event_Info)
#define KLAV_DEBUG_CODE_LB_EVENT  1

// debug event: procedure call (info: KLAV_Internal_Event_Info)
#define KLAV_DEBUG_CODE_LB_PROC   2

////////////////////////////////////////////////////////////////
// Debug event class: Tree Searcher

#define KLAV_DEBUG_CLASS_TREESEARCH  0x08

// debug event: tree search buffer (info: KLAV_Tree_Search_Parms)
#define KLAV_DEBUG_CODE_TREE_BUF_SEARCH 1
// debug event: tree signature found (parms not checked) (info: SigEvent)
#define KLAV_DEBUG_CODE_TREE_SIG_FOUND_RAW  2
// debug event: tree signature found (parms checked) (info: SigEvent)
#define KLAV_DEBUG_CODE_TREE_SIG_FOUND  3

////////////////////////////////////////////////////////////////
// Debug event class: Region Manager / Region Scanner

#define KLAV_DEBUG_CLASS_RGNMGR  0x10

// debug event: region create (info: KLAV_Region *)
#define KLAV_DEBUG_CODE_RGN_CREATE   1
// debug event: region destroy (info: KLAV_Region *)
#define KLAV_DEBUG_CODE_RGN_DESTROY  2

// debug event: region scan (info: KLAV_Region *)
#define KLAV_DEBUG_CODE_RGN_SCAN     3

////////////////////////////////////////////////////////////////
// Debug event class: Packers

struct KLAV_AVP3_Record_Info
{
	const char * m_name;
	const char * m_full_name;
};

#define KLAV_DEBUG_CLASS_PACKER 0x11

// debug event: packer found (info: KLAV_AVP3_Record_Info *)
#define KLAV_DEBUG_CODE_PACKER_FOUND 1
#define KLAV_DEBUG_CODE_PACKER_ERROR 2

////////////////////////////////////////////////////////////////
// Debug event class: Stream

#define KLAV_DEBUG_CLASS_STREAM 0x80

struct KLAV_Stream_Info
{
	const char * m_stream_id;
	const char * m_handler_id;
};

// debug event: stream created/destroyed (info: KLAV_Stream_Info *)
#define KLAV_DEBUG_CODE_STREAM_CREATE    0
#define KLAV_DEBUG_CODE_STREAM_DESTROY   1

// debug event: stream opened/closed (info: KLAV_Stream_Info *)
#define KLAV_DEBUG_CODE_STREAM_OPEN      4
#define KLAV_DEBUG_CODE_STREAM_CLOSE     5

// debug event: handler registered/unregistered (info: KLAV_Stream_Info *)
#define KLAV_DEBUG_CODE_STREAM_HDL_REG   8
#define KLAV_DEBUG_CODE_STREAM_HDL_UNREG 9

// debug event: handler opened/closed (info: KLAV_Stream_Info *)
#define KLAV_DEBUG_CODE_STREAM_HDL_OPEN  12
#define KLAV_DEBUG_CODE_STREAM_HDL_CLOSE 13

////////////////////////////////////////////////////////////////
// Debug event printer

KLAV_EXTERN_C
KLAVDBG_API
void KLAV_CALL KLAV_Print_Msg (
			KLAV_CONTEXT_TYPE ctx,
			unsigned int level,
			hKLAV_Message_Printer output,
			const char *fmt,
			...
		);

KLAV_EXTERN_C
KLAVDBG_API
void KLAV_CALL KLAV_Print_Debug_Event (
			uint32_t cb_code,             // callback code (should be KLAV_CALLBACK_DEBUG_EVENT)
			KLAV_Properties *props,       // callback properties
			KLAV_Message_Printer *output  // output channel
		);

KLAV_EXTERN_C
KLAVDBG_API
void KLAV_CALL KLAV_Region_Manager_Dump (
			KLAV_CONTEXT_TYPE klav_context,
			struct KLAV_Region_Manager *rm,
			KLAV_Message_Printer *output
		);

KLAV_EXTERN_C
KLAVDBG_API
void KLAV_CALL KLAV_Print_Performance_Data (
			uint32_t cb_code,             // callback code (should be KLAV_CALLBACK_DEBUG_EVENT)
			KLAV_Properties *props,       // callback properties
			KLAV_Message_Printer *output  // output channel
		);

////////////////////////////////////////////////////////////////
// Debug proc resolver

KLAV_EXTERN_C
KLAVDBG_API
hKLAV_Debug_Proc_Resolver KLAV_CALL KLAV_Create_Debug_Proc_Resolver (
			hKLAV_Alloc      alloc,
			hKLAV_DSO_Loader dso_loader,
			hKLAV_IO_Library io_library,
			hKLAV_Message_Printer msg_printer,
			const char * dll_fname,
			const char * proc_map_fname
		);
			
KLAV_EXTERN_C
KLAVDBG_API
KLAV_ERR KLAV_CALL KLAV_Destroy_Debug_Proc_Resolver (
			hKLAV_Debug_Proc_Resolver obj
		);

////////////////////////////////////////////////////////////////
// Debug API module

struct KLAV_Debug_API;
typedef struct KLAV_Debug_API * hKLAV_Debug_API;

#define KLAV_DBGAPI_VERSION_CURRENT 0x0100

enum
{
	KLAV_DBGAPI_IFACE_PROC_RESOLVER = 0x01,
	KLAV_DBGAPI_IFACE_TRACER        = 0x02,
};

#ifdef __cplusplus

struct KLAV_NO_VTABLE KLAV_Debug_API
{
public:
	virtual void KLAV_CALL destroy () = 0;

	virtual uint32_t KLAV_CALL get_version () = 0;

	virtual void * KLAV_CALL get_iface (
			uint32_t ifc_id
		) = 0;

	virtual KLAV_ERR KLAV_CALL set_iface (
			uint32_t ifc_id,
			void *   ifc_ptr
		) = 0;

	// returns KLAV_ENOTFOUND if unknown option
	virtual KLAV_ERR KLAV_CALL set_debug_option (
			const char * opt_name,
			const char * opt_value
		) = 0;

	virtual hKLAV_Debug_Proc_Resolver KLAV_CALL create_dbg_proc_resolver (
			hKLAV_DSO_Loader dso_loader,
			hKLAV_IO_Library io_library,
			hKLAV_Message_Printer msg_printer,
			const char * dll_fname,
			const char * proc_map_fname
		) = 0;

	virtual void KLAV_CALL destroy_dbg_proc_resolver (
			hKLAV_Debug_Proc_Resolver proc_resolver
		) = 0;

	virtual KLAV_ERR KLAV_CALL get_property (
			klav_propid_t prop_id,
			uint32_t      index,
			klav_propval_t *pval
		) = 0;

	virtual KLAV_ERR KLAV_CALL set_property (
			klav_propid_t prop_id,
			uint32_t      index,
			const klav_propval_t *val
		) = 0;

    KLAV_DUMMY_METHODS_DECL(KLAV_Debug_API)
};

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL (KLAV_Debug_API)
{
	void  (KLAV_CALL *destroy) (
			hKLAV_Debug_API
		);

	uint32_t (KLAV_CALL *get_version) (
			hKLAV_Debug_API
		);

	void * (KLAV_CALL *get_iface) (
			hKLAV_Debug_API,
			uint32_t
		);

	KLAV_ERR (KLAV_CALL *set_iface) (
			hKLAV_Debug_API,
			uint32_t,
			void *
		);

	KLAV_ERR (KLAV_CALL *set_debug_option) (
			hKLAV_Debug_API,
			const char *,
			const char *
		);

	hKLAV_Debug_Proc_Resolver (KLAV_CALL *create_dbg_proc_resolver) (
			hKLAV_Debug_API,
			hKLAV_DSO_Loader dso_loader,
			hKLAV_IO_Library io_library,
			hKLAV_Message_Printer msg_printer,
			const char * dll_fname,
			const char * proc_map_fname
		);

	void (KLAV_CALL *destroy_dbg_proc_resolver) (
			hKLAV_Debug_API,
			hKLAV_Debug_Proc_Resolver
		);

	KLAV_ERR (KLAV_CALL *get_property) (
			hKLAV_Debug_API,
			klav_propid_t,
			uint32_t,
			klav_propval_t *
		);

	KLAV_ERR (KLAV_CALL *set_property) (
			hKLAV_Debug_API,
			klav_propid_t,
			uint32_t,
			const klav_propval_t *
		);
};

KLAV_DECLARE_C_INTERFACE (KLAV_Debug_API);

#endif // __cplusplus

typedef KLAV_ERR (KLAV_CALL * PFN_KLAV_Debug_API_Create)(hKLAV_Alloc alloc, hKLAV_Debug_API *);

#define KLAV_DEBUG_API_CREATE_FN   KLAV_Debug_API_Create
#define KLAV_DEBUG_API_CREATE_NAME "KLAV_Debug_API_Create"


#endif // klav_dbg_h_INCLUDED

