// klaveng.h
//
// KLAVA engine interface
// version: 1.3
//

#ifndef klaveng_h_INCLUDED
#define klaveng_h_INCLUDED

#define KLAV_ENGINE_VERSION_MAJOR 1
#define KLAV_ENGINE_VERSION_MINOR 4

#if !defined(RC_INVOKED)

#include <klava/klavdef.h>
#include <klava/klav_props.h>

////////////////////////////////////////////////////////////////
// Engine versioning

#define KLAV_GET_ENGINE_VERSION_MAJOR(VER) ((VER) >> 16)
#define KLAV_GET_ENGINE_VERSION_MINOR(VER) ((VER) & 0xFFFF)
#define KLAV_MAKE_ENGINE_VERSION(MAJOR,MINOR) (((MAJOR) << 16) | (MINOR))

// engine versions

// current deployed engine version for clients (lowest version of released databases)
#define KLAV_ENGINE_VERSION_CURRENT  KLAV_MAKE_ENGINE_VERSION(0,1)

// engine version for development (bases, etc)
#define KLAV_ENGINE_VERSION  KLAV_MAKE_ENGINE_VERSION(KLAV_ENGINE_VERSION_MAJOR,KLAV_ENGINE_VERSION_MINOR)

////////////////////////////////////////////////////////////////
// Generic engine interface

struct KLAV_Engine;
typedef struct KLAV_Engine * hKLAV_Engine;

#ifdef __cplusplus

struct KLAV_NO_VTABLE KLAV_Engine
{
	virtual void KLAV_CALL destroy (
			) = 0;

	virtual KLAV_Properties * KLAV_CALL get_properties (
			) = 0;

	virtual KLAV_Property_Bag * KLAV_CALL create_property_bag (
			) = 0;

	virtual KLAV_ERR KLAV_CALL set_iface (
				uint32_t ifc_id,
				void *ifc
			) = 0;

	virtual void * KLAV_CALL get_iface (
				uint32_t ifc_id
			) = 0;

	virtual KLAV_ERR KLAV_CALL load (
				KLAV_Properties * parms
			) = 0;

	virtual KLAV_ERR KLAV_CALL unload (
			) = 0;

	virtual KLAV_CONTEXT_TYPE KLAV_CALL get_global_context (
			) = 0;

	virtual KLAV_ERR KLAV_CALL create_context (
				KLAV_Properties * parms,
				KLAV_CONTEXT_TYPE *pcontext
			) = 0;

	virtual KLAV_ERR KLAV_CALL destroy_context (
				KLAV_CONTEXT_TYPE context
			) = 0;

	virtual KLAV_ERR KLAV_CALL activate_context (
				KLAV_CONTEXT_TYPE context,
				void * object,
				KLAV_Properties * parms
			) = 0;
			
	virtual KLAV_ERR KLAV_CALL deactivate_context (
				KLAV_CONTEXT_TYPE context
			) = 0;

	virtual KLAV_ERR KLAV_CALL flush_context (
				KLAV_CONTEXT_TYPE context
			) = 0;

	virtual KLAV_ERR KLAV_CALL process_action (
				KLAV_CONTEXT_TYPE context,
				const char *action_name,
				void *action_object,
				KLAV_Properties *parms
			) = 0;

	virtual KLAV_ERR KLAV_CALL get_context_property (
				KLAV_CONTEXT_TYPE context,
				klav_propid_t prop_id,
				uint32_t      index,
				klav_propval_t *pval
			) = 0;

	virtual KLAV_ERR KLAV_CALL set_context_property (
				KLAV_CONTEXT_TYPE context,
				klav_propid_t prop_id,
				uint32_t      index,
				const klav_propval_t *val
			) = 0;

    KLAV_DUMMY_METHODS_DECL(KLAV_Engine)
};

// context object interface 
struct KLAV_NO_VTABLE KLAV_Context_If
{
	// 'not implemented' method
	virtual void KLAV_CALL not_implemented () = 0;
	// trace methods
	virtual void KLAV_CALL trace (KLAV_TRACE_LEVEL level, const char* format, ...) = 0;
	virtual void KLAV_CALL vtrace (KLAV_TRACE_LEVEL level, const char* format, va_list ap) = 0;
	// context property access
	virtual KLAV_ERR KLAV_CALL get_property (klav_propid_t propid, uint32_t index, klav_propval_t *pval) = 0;
	virtual KLAV_ERR KLAV_CALL set_property (klav_propid_t propid, uint32_t index, const klav_propval_t *pval) = 0;
	// callback interface
	virtual KLAV_ERR KLAV_CALL send_callback (uint32_t cb_code, hKLAV_Properties cb_props) = 0;
	// raise exception
	virtual KLAV_ERR KLAV_CALL raise_exception (uint32_t ex_code, const char *info) = 0;
	// trace point
	virtual KLAV_ERR KLAV_CALL trace_point (uint32_t id, const char *trace_info) = 0;
	// debug event
	virtual KLAV_ERR KLAV_CALL debug_event (uint32_t id, const char *extra_info) = 0;
};

#define KLAV_CONTEXT_TRACE(CTX,ARGS) if (CTX != 0) ((KLAV_Context_If *)(CTX))->trace ARGS
#define KLAV_CONTEXT_GET_PROPERTY(CTX,CODE,INDEX,VAL) (((CTX) != 0) ? ((KLAV_Context_If *)(CTX))->get_property (CODE,INDEX,VAL) : KLAV_ENOTIMPL)
#define KLAV_CONTEXT_SET_PROPERTY(CTX,CODE,INDEX,VAL) (((CTX) != 0) ? ((KLAV_Context_If *)(CTX))->set_property (CODE,INDEX,VAL) : KLAV_ENOTIMPL)
#define KLAV_CONTEXT_SEND_CALLBACK(CTX,CODE,PROPS) (((CTX) != 0) ? ((KLAV_Context_If *)(CTX))->send_callback (CODE,PROPS) : KLAV_ENOTIMPL)

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL (KLAV_Engine)
{
	void     (KLAV_CALL *destroy) (struct KLAV_Engine *);

	struct KLAV_Properties * (KLAV_CALL *get_properties) (struct KLAV_Engine *);
	struct KLAV_Property_Bag * (KLAV_CALL *create_property_bag) (struct KLAV_Engine *);

	KLAV_ERR (KLAV_CALL *set_iface)  (struct KLAV_Engine *, uint32_t, void *);
	void *   (KLAV_CALL *get_iface)  (struct KLAV_Engine *, uint32_t);

	KLAV_ERR (KLAV_CALL *load) (
			struct KLAV_Engine *,
			struct KLAV_Properties *
		);

	KLAV_ERR (KLAV_CALL *unload) (
			struct KLAV_Engine *
		);

	KLAV_CONTEXT_TYPE (KLAV_CALL *get_global_context) (
			struct KLAV_Engine *
		);

	KLAV_ERR (KLAV_CALL *create_context)  (
			struct KLAV_Engine *,
			struct KLAV_Properties *,
			KLAV_CONTEXT_TYPE *
		);

	KLAV_ERR (KLAV_CALL *destroy_context) (
			struct KLAV_Engine *,
			KLAV_CONTEXT_TYPE
		);

	KLAV_ERR (KLAV_CALL *activate_context) (
			struct KLAV_Engine *,
			KLAV_CONTEXT_TYPE,
			void *,
			struct KLAV_Properties *
		);

	KLAV_ERR (KLAV_CALL *deactivate_context) (
			struct KLAV_Engine *,
			KLAV_CONTEXT_TYPE
		);

	KLAV_ERR (KLAV_CALL *flush_context) (
			struct KLAV_Engine *,
			KLAV_CONTEXT_TYPE
		);

	KLAV_ERR (KLAV_CALL *process_action) (
			struct KLAV_Engine *,
			KLAV_CONTEXT_TYPE,
			const char *,
			void *,
			struct KLAV_Properties *
		);

	KLAV_ERR (KLAV_CALL *get_context_property) (
			struct KLAV_Engine *,
			KLAV_CONTEXT_TYPE,
			klav_propid_t,
			uint32_t,
			klav_propval_t *
		);

	KLAV_ERR (KLAV_CALL *set_context_property) (
			struct KLAV_Engine *,
			KLAV_CONTEXT_TYPE,
			klav_propid_t,
			uint32_t,
			const klav_propval_t *
		);
};

KLAV_DECLARE_C_INTERFACE (KLAV_Engine);

#endif // __cplusplus

/////////////////////////////////////////////////////////////////
// Factory

typedef KLAV_ERR (KLAV_CALL * PFN_KLAV_Engine_Create)(
		uint32_t version,   // usually KLAV_ENGINE_VERSION_CURRENT
		hKLAV_Alloc alloc,  // memory allocator to use
		hKLAV_Engine *      // returned handle to engine object
	);

#define KLAV_ENGINE_CREATE_FN   KLAV_Engine_Create
#define KLAV_ENGINE_CREATE_NAME "KLAV_Engine_Create"

////////////////////////////////////////////////////////////////
// Database load flags (load() method, KLAV_PROPID_DB_FLAGS)

// dont load, just get info
#define KLAV_DBF_GET_INFO         0x0001

////////////////////////////////////////////////////////////////
// Kernel signature for debugging

struct KLAV_Kernel_Info
{
	union Signature
	{
		uint8_t  b [8];
		uint64_t ull;  // for proper alignment
	}         signature;
	uint64_t  db_timestamp;    // DB timestamp (time_t)
	void *    global_context;  // global context
	uint32_t  engine_api_ver;  // kernel API version
	uint32_t  client_api_ver;  // client API version
	uint32_t  db_release_no;   // DB release number
	uint32_t  struct_size;     // sizeof (KLAV_Kernel_Info)
	void *    db_code_addr;    // code fragment load address
	uint32_t  db_code_size;    // code fragment size
};

// signature 'KDB1NF0\0'
#define KLAV_KERNEL_INFO_SIG_0 'K'
#define KLAV_KERNEL_INFO_SIG_1 'D'
#define KLAV_KERNEL_INFO_SIG_2 'B'
#define KLAV_KERNEL_INFO_SIG_3 '1'
#define KLAV_KERNEL_INFO_SIG_4 'N'
#define KLAV_KERNEL_INFO_SIG_5 'F'
#define KLAV_KERNEL_INFO_SIG_6 '0'
#define KLAV_KERNEL_INFO_SIG_7 '\0'

#define KLAV_KERNEL_STACK_MARK 0xBEDABEDA

////////////////////////////////////////////////////////////////
// File scan engine (scanner plugin) interface

struct KLAV_Scanner_Engine;
typedef struct KLAV_Scanner_Engine * hKLAV_Scanner_Engine;

#ifdef __cplusplus

struct KLAV_Scanner_Engine
{
public:
	virtual void KLAV_CALL destroy (
			) = 0;

	virtual KLAV_ERR KLAV_CALL set_option (
				const char *optname,
				const char *optval
			) = 0;

	virtual KLAV_Properties * KLAV_CALL get_properties (
			) = 0;

	virtual KLAV_Property_Bag * KLAV_CALL create_property_bag (
			) = 0;

	virtual KLAV_ERR KLAV_CALL set_iface (
				uint32_t ifc_id,
				void *ifc
			) = 0;

	virtual void * KLAV_CALL get_iface (
				uint32_t ifc_id
			) = 0;

	virtual KLAV_ERR KLAV_CALL load (
				const char * db_path,
				KLAV_Properties * props
			) = 0;

	virtual KLAV_ERR KLAV_CALL unload (
			) = 0;

	virtual KLAV_ERR KLAV_CALL process_file (
				const char * path,
				KLAV_Properties * props
			) = 0;

	virtual KLAV_Engine * KLAV_CALL get_engine (
			) = 0;
};

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL (KLAV_Scanner_Engine)
{
	void     (KLAV_CALL *destroy) (struct KLAV_Scanner_Engine *);

	KLAV_ERR (KLAV_CALL *set_option) (struct KLAV_Scanner_Engine *, const char *, const char *);

	struct KLAV_Properties * (KLAV_CALL *get_properties) (struct KLAV_Scanner_Engine *);
	struct KLAV_Property_Bag * (KLAV_CALL *create_property_bag) (struct KLAV_Scanner_Engine *);

	KLAV_ERR (KLAV_CALL *set_iface)  (struct KLAV_Scanner_Engine *, uint32_t, void *);
	void *   (KLAV_CALL *get_iface)  (struct KLAV_Scanner_Engine *, uint32_t);
	
	KLAV_ERR (KLAV_CALL *load) (
			struct KLAV_Scanner_Engine *,
			const char *,
			struct KLAV_Properties *
		);
	
	KLAV_ERR (KLAV_CALL *unload) (
			struct KLAV_Scanner_Engine *
		);

	KLAV_ERR (KLAV_CALL *process_file)  (
			struct KLAV_Engine *,
			const char *,
			struct KLAV_Properties *
		);

	KLAV_ERR (KLAV_CALL *get_engine)  (
			struct KLAV_Engine *
		);
};

KLAV_DECLARE_C_INTERFACE (KLAV_Scanner_Engine);

#endif // __cplusplus

typedef KLAV_ERR (KLAV_CALL * PFN_KLAV_Scanner_Engine_Create)(hKLAV_Scanner_Engine *);

#define KLAV_SCANNER_ENGINE_CREATE_FN   KLAV_Scanner_Engine_Create
#define KLAV_SCANNER_ENGINE_CREATE_NAME "KLAV_Scanner_Engine_Create"

// Application provided symbols

typedef uint32_t KLAV_APP_SYM_ID;
#define MAKE_KLAV_APP_SYM_ID(A,B,C,D) \
	((((((((KLAV_APP_SYM_ID)(A)) << 8) | (B)) << 8) | (C)) << 8) | (D))

// well-known core symbols

#define KLAV_APP_SYMBOL_ID_GLOBAL_CONTEXT MAKE_KDBID('!','G','l','C')
#define KLAV_APP_SYMBOL_ID_memset         MAKE_KDBID('F',0,'m','s')
#define KLAV_APP_SYMBOL_ID_memcpy         MAKE_KDBID('F',0,'m','c')
#define KLAV_APP_SYMBOL_ID_memmove        MAKE_KDBID('F',0,'m','m')
#define KLAV_APP_SYMBOL_ID_memcmp         MAKE_KDBID('F',0,'m','=')

////////////////////////////////////////////////////////////////

#endif // RC_INVOKED
#endif // klaveng_h_INCLUDED

