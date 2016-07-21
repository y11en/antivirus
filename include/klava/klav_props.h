// klav_props.h
//
//

#ifndef klav_props_h_INCLUDED
#define klav_props_h_INCLUDED

#include "klavdef.h"

////////////////////////////////////////////////////////////////
// Properties

typedef uint32_t klav_propid_t;

typedef enum
{
	KLAV_PROP_TYPE_INVALID = 0,
	KLAV_PROP_TYPE_INT     = 1,
	KLAV_PROP_TYPE_UINT    = 2,
	KLAV_PROP_TYPE_LONG    = 3,
	KLAV_PROP_TYPE_ULONG   = 4,
	KLAV_PROP_TYPE_TIME    = 5,
	KLAV_PROP_TYPE_STR     = 6,
	KLAV_PROP_TYPE_PTR     = 7
}
KLAV_PROP_TYPE;

// Inherited property flag
#define KLAV_PROP_F_INHERITABLE 0x008000000

#define KLAV_PROPID_INVALID ((klav_propid_t)0)

#define KLAV_PROPID_TYPE(PROPID) (((PROPID) >> 24) & 0x0F)
#define KLAV_PROPID_INHERITABLE(PROPID) ((PROPID) & KLAV_PROP_F_INHERITABLE)
#define KLAV_MAKE_PROPID(VALTYPE,ID) (((KLAV_PROP_TYPE_##VALTYPE) << 24) | (ID))

typedef union KLAV_PropVal
{
	int32_t      i;
	uint32_t     ui;
	int64_t      l;
	uint64_t     ul;
	const char * cs;
	char *       s;
	const void * cp;
	void *       p;

#ifdef __cplusplus
	inline void operator= (int32_t val)  { i = val; }
	inline void operator= (uint32_t val) { ui = val; }
	inline void operator= (int64_t val)  { l = val; }
	inline void operator= (uint64_t val) { ul = val; }
	inline void operator= (const char *val)  { cs = val; }
	inline void operator= (char *val) { s = val; }
	inline void operator= (const void *val)  { cp = val; }
	inline void operator= (void *val) { p = val; }
	inline void clear ()  { ul = 0; }
#endif

} klav_propval_t;

#ifdef __cplusplus

struct KLAV_Prop_Val
{
	klav_propval_t m_val;
	
	klav_propval_t& value () { return m_val; }
	const klav_propval_t& value () const { return m_val; }

 	inline KLAV_Prop_Val () { m_val.clear (); }
	inline KLAV_Prop_Val (int32_t val)  { m_val = val; }
	inline KLAV_Prop_Val (uint32_t val) { m_val = val; }
	inline KLAV_Prop_Val (int64_t val)  { m_val = val; }
	inline KLAV_Prop_Val (uint64_t val) { m_val = val; }
	inline KLAV_Prop_Val (const char *val)  { m_val = val; }
	inline KLAV_Prop_Val (char *val)        { m_val = val; }
	inline KLAV_Prop_Val (const void *val)  { m_val = val; }
	inline KLAV_Prop_Val (void *val)        { m_val = val; }

	inline void clear () { m_val.clear (); }

	inline KLAV_Prop_Val& operator= (int32_t val)
		{ m_val = val; return *this; }
	inline KLAV_Prop_Val& operator= (uint32_t val)
		{ m_val = val; return *this; }
	inline KLAV_Prop_Val& operator= (int64_t val)
		{ m_val = val; return *this; }
	inline KLAV_Prop_Val& operator= (uint64_t val)
		{ m_val = val; return *this; }
	inline KLAV_Prop_Val& operator= (const char *val)
		{ m_val = val; return *this; }
	inline KLAV_Prop_Val& operator= (char *val)
		{ m_val = val; return *this; }
	inline KLAV_Prop_Val& operator= (const void *val)
		{ m_val = val; return *this; }
	inline KLAV_Prop_Val& operator= (void *val)
		{ m_val = val; return *this; }
};

struct KLAV_NO_VTABLE KLAV_Properties
{
	virtual const klav_propval_t * KLAV_CALL get_property (klav_propid_t id) = 0;

	// returns KLAV_ENOTFOUND if the property is missing, KLAV_EINVAL if property value is invalid
	virtual KLAV_ERR KLAV_CALL set_property (klav_propid_t id, const klav_propval_t *val) = 0;

	virtual unsigned int  KLAV_CALL get_property_count () = 0;
	virtual klav_propid_t KLAV_CALL get_property_by_no (unsigned int idx, klav_propval_t *val) = 0;
};

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL(KLAV_Properties)
{
    const klav_propval_t * (KLAV_CALL *get_property) (struct KLAV_Properties *, klav_propid_t);
	KLAV_ERR (KLAV_CALL *set_property )(klav_propid_t id, const klav_propval_t *val);

	unsigned int  (KLAV_CALL * get_property_count) (struct KLAV_Properties *);
	klav_propid_t (KLAV_CALL * get_property_by_no) (struct KLAV_Properties *, unsigned int, klav_propval_t *);
};

KLAV_DECLARE_C_INTERFACE(KLAV_Properties);

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// Simple static KLAV_Properties implementation

#ifdef __cplusplus

struct KLAV_Properties_Impl : public KLAV_IFACE_IMPL(KLAV_Properties)
{
	struct prop_desc;

	KLAV_Properties * m_chain;
	prop_desc       * m_props;

	struct prop_desc
	{
		klav_propval_t m_val;
		prop_desc *    m_next;
		klav_propid_t  m_id;

		prop_desc (KLAV_Properties_Impl& bag, klav_propid_t id, int32_t val)
			{ m_val.i = val; m_next = bag.m_props; bag.m_props = this; m_id = id; }

		prop_desc (KLAV_Properties_Impl& bag, klav_propid_t id, uint32_t val)
			{ m_val.ui = val; m_next = bag.m_props; bag.m_props = this; m_id = id; }

		prop_desc (KLAV_Properties_Impl& bag, klav_propid_t id, int64_t val)
			{ m_val.l = val; m_next = bag.m_props; bag.m_props = this; m_id = id; }

		prop_desc (KLAV_Properties_Impl& bag, klav_propid_t id, uint64_t val)
			{ m_val.ul = val; m_next = bag.m_props; bag.m_props = this; m_id = id; }

		prop_desc (KLAV_Properties_Impl& bag, klav_propid_t id, const char *val)
			{ m_val.cs = val; m_next = bag.m_props; bag.m_props = this; m_id = id; }

		prop_desc (KLAV_Properties_Impl& bag, klav_propid_t id, char *val)
			{ m_val.s = val; m_next = bag.m_props; bag.m_props = this; m_id = id; }

		prop_desc (KLAV_Properties_Impl& bag, klav_propid_t id, const void *val)
			{ m_val.cp = val; m_next = bag.m_props; bag.m_props = this; m_id = id; }

		prop_desc (KLAV_Properties_Impl& bag, klav_propid_t id, void *val)
			{ m_val.p = val; m_next = bag.m_props; bag.m_props = this; m_id = id; }

		prop_desc (KLAV_Properties_Impl& bag, klav_propid_t id, const klav_propval_t& val)
			{ m_val = val; m_next = bag.m_props; bag.m_props = this; m_id = id; }
	};

	KLAV_Properties_Impl (KLAV_Properties *chain = 0)
		: m_chain (chain), m_props (0) {}

	virtual ~KLAV_Properties_Impl ();

	virtual const klav_propval_t * KLAV_CALL get_property (klav_propid_t id);
	virtual KLAV_ERR KLAV_CALL set_property (klav_propid_t id, const klav_propval_t *val);

	virtual unsigned int  KLAV_CALL get_property_count ();
	virtual klav_propid_t KLAV_CALL get_property_by_no (unsigned int idx, klav_propval_t *val);
};

#define KLAV_STATIC_PROPERTIES(NAME) KLAV_Properties_Impl NAME
#define KLAV_STATIC_PROPERTIES_CHAIN(NAME,CHAIN) KLAV_Properties_Impl NAME (CHAIN)

#define KLAV_ALIGN_UINTPTR(N) (((N) + sizeof (uintptr_t) - 1) & ~(sizeof (uintptr_t) - 1))
#define KLAV_STATIC_PROPBAG_SIZEOF KLAV_ALIGN_UINTPTR(sizeof (KLAV_Properties_Impl))

// macro for using static properties from links (requires klavcore.h)
#define KLAV_STATIC_PROPERTIES_EX(NAME) \
	uintptr_t NAME##_data [KLAV_STATIC_PROPBAG_SIZEOF / sizeof (uintptr_t)]; \
	KLAV_Properties_Impl& NAME = * KLAV_PROC_PTR(KLAV_Static_PropBag_Construct)(KLAV_CONTEXT, &NAME##_data, KLAV_STATIC_PROPBAG_SIZEOF, 0)

#define KLAV_STATIC_PROPBAG_CHAIN_EX(NAME,CHAIN) \
	uintptr_t NAME##_data [KLAV_STATIC_PROPBAG_SIZEOF / sizeof (uintptr_t)]; \
	KLAV_Properties_Impl& NAME = * KLAV_PROC_PTR(KLAV_Static_PropBag_Construct)(KLAV_CONTEXT, &NAME##_data, KLAV_STATIC_PROPBAG_SIZEOF, (CHAIN))

#define KLAV_STATIC_PROPERTY_NAME_CONCAT2(A,B) A ## B
#define KLAV_STATIC_PROPERTY_NAME(LINE) KLAV_STATIC_PROPERTY_NAME_CONCAT2(KLAV_STATIC_PROPERTY_,LINE)

#define KLAV_STATIC_PROPERTY(BAG,ID,VAL) \
	KLAV_Properties_Impl::prop_desc KLAV_STATIC_PROPERTY_NAME(__LINE__) (BAG,ID,VAL)

#endif // __cplusplus

////////////////////////////////////////////////////////////////

#ifdef __cplusplus

struct KLAV_NO_VTABLE KLAV_Property_Bag : public KLAV_Properties
{
	// appends properties to the existing set
	virtual KLAV_ERR KLAV_CALL set_properties (KLAV_Properties *props) = 0;
	virtual void     KLAV_CALL clear_properties () = 0;

	virtual void     KLAV_CALL destroy () = 0;

    KLAV_DUMMY_METHODS_DECL(KLAV_Property_Bag)
};

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL(KLAV_Property_Bag)
{
    const klav_propval_t * (KLAV_CALL *get_property) (struct KLAV_Property_Bag *, klav_propid_t);
	KLAV_ERR (KLAV_CALL * set_property) (struct KLAV_Property_Bag *, klav_propid_t, const klav_propval_t *);

	unsigned int  (KLAV_CALL * get_property_count) (struct KLAV_Property_Bag *);
	klav_propid_t (KLAV_CALL * get_property_by_no) (struct KLAV_Property_Bag *, unsigned int, klav_propval_t *);

	KLAV_ERR (KLAV_CALL * set_properties) (struct KLAV_Property_Bag *, struct KLAV_Properties *);
	void     (KLAV_CALL * clear_properties) (struct KLAV_Property_Bag *);

	void     (KLAV_CALL * destroy) (struct KLAV_Property_Bag *);
};

KLAV_DECLARE_C_INTERFACE(KLAV_Property_Bag);

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// Property access helpers

#ifdef __cplusplus

inline int32_t KLAV_Get_Property_Int (KLAV_Properties *props, klav_propid_t id)
{
	const klav_propval_t *pv = props->get_property (id);
	return (pv == 0) ? 0 : pv->i;
}

inline int64_t KLAV_Get_Property_Long (KLAV_Properties *props, klav_propid_t id)
{
	const klav_propval_t *pv = props->get_property (id);
	return (pv == 0) ? 0 : pv->l;
}

inline int64_t KLAV_Get_Property_Time (KLAV_Properties *props, klav_propid_t id)
{
	const klav_propval_t *pv = props->get_property (id);
	return (pv == 0) ? 0 : pv->l;
}

inline const char * KLAV_Get_Property_Str (KLAV_Properties *props, klav_propid_t id)
{
	const klav_propval_t *pv = props->get_property (id);
	return (pv == 0) ? 0 : pv->cs;
}

inline const void * KLAV_Get_Property_Ptr (KLAV_Properties *props, klav_propid_t id)
{
	const klav_propval_t *pv = props->get_property (id);
	return (pv == 0) ? 0 : pv->cp;
}

inline KLAV_ERR KLAV_Set_Property_Int (KLAV_Properties *props, klav_propid_t id, int32_t val)
{
	klav_propval_t pv; pv.i = val;
	return props->set_property (id, &pv);
}

inline KLAV_ERR KLAV_Set_Property_Long (KLAV_Properties *props, klav_propid_t id, int64_t val)
{
	klav_propval_t pv; pv.l = val;
	return props->set_property (id, &pv);
}

inline KLAV_ERR KLAV_Set_Property_Time (KLAV_Properties *props, klav_propid_t id, int64_t val)
{
	klav_propval_t pv; pv.l = val;
	return props->set_property (id, &pv);
}

inline KLAV_ERR KLAV_Set_Property_Str (KLAV_Properties *props, klav_propid_t id, const char *val)
{
	klav_propval_t pv; pv.cs = val;
	return props->set_property (id, &pv);
}

inline KLAV_ERR KLAV_Set_Property_Ptr (KLAV_Properties *props, klav_propid_t id, const void *val)
{
	klav_propval_t pv; pv.cp = val;
	return props->set_property (id, &pv);
}

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// Known property IDs

enum KLAV_PROPID
{
#define KLAV_PROPERTY_DEF(NAME,TYPE,ID) KLAV_PROPID_##NAME = KLAV_MAKE_PROPID(TYPE,ID),
#include "klav_props.h"
};

////////////////////////////////////////////////////////////////
// Callback handler

typedef uint32_t KLAV_CALLBACK_CODE;

#ifdef __cplusplus

struct KLAV_NO_VTABLE KLAV_Callback_Handler
{
	virtual KLAV_ERR KLAV_CALL handle_callback (
				KLAV_CONTEXT_TYPE  context,
				KLAV_CALLBACK_CODE code,
				KLAV_Properties *  props
			) = 0;

    KLAV_DUMMY_METHODS_DECL(KLAV_Callback_Handler)
};

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL(KLAV_Callback_Handler)
{
	KLAV_ERR (KLAV_CALL * handle_callback) (
				struct KLAV_Callback_Handler * handler,
				KLAV_CALLBACK_CODE             code,
				struct KLAV_Properties *       props
			);
};

KLAV_DECLARE_C_INTERFACE(KLAV_Callback_Handler);

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// Callback codes

enum KLAV_CALLBACK
{
#define KLAV_CALLBACK_DEF(NAME,ID) KLAV_CALLBACK_##NAME = ID,
#include "klav_props.h"
};

////////////////////////////////////////////////////////////////
// Performance slots

typedef uint64_t klav_perf_slot;

enum KLAV_PERF_SLOT
{
#define KLAV_PERF_SLOT_DEF(ID,TYPE,NAME) KLAV_PERF_SLOT_##NAME = ID,
#include "klav_props.h"
};

#endif // klav_props_h_INCLUDED

////////////////////////////////////////////////////////////////
// PROPERTY DEFINITIONS

#ifdef KLAV_PROPERTY_DEF

#ifndef KLAV_PROPERTY_INH
# define KLAV_PROPERTY_INH(NAME,TYPE,ID) KLAV_PROPERTY_DEF(NAME,TYPE,((ID) | KLAV_PROP_F_INHERITABLE))
#endif // KLAV_PROPERTY_INH

KLAV_PROPERTY_DEF(CONTEXT,         PTR,  0x0001)
KLAV_PROPERTY_DEF(TRACE_LEVEL,     UINT, 0x0002)

KLAV_PROPERTY_DEF(ERROR_CODE,      UINT, 0x0004)
KLAV_PROPERTY_DEF(ERROR_INFO,      STR,  0x0005)

// aliases for trace point events
KLAV_PROPERTY_DEF(TRACE_ID,        UINT, 0x0004)
KLAV_PROPERTY_DEF(TRACE_INFO,      STR,  0x0005)

// debug event parameters
KLAV_PROPERTY_DEF(DEBUG_EVENT_ID,   UINT, 0x0008)
KLAV_PROPERTY_DEF(DEBUG_EVENT_INFO, PTR,  0x0009)

// performance data parameters
KLAV_PROPERTY_DEF(PERF_DATA,       PTR,  0x000A)

// external user-specified per-context data
KLAV_PROPERTY_DEF(USER_DATA,       PTR,  0x000C)
// ptr to engine interface
KLAV_PROPERTY_DEF(ENGINE,          PTR,  0x000D)

// context creation attributes (used in create_context () method)
// external context-private heap allocator (KLAV_Alloc *)
KLAV_PROPERTY_DEF(CONTEXT_HEAP,    PTR,  0x0010)

// hierarchical object system information (for object manager)
KLAV_PROPERTY_DEF(PARENT_CONTEXT,  PTR,  0x0020)

// prague context
KLAV_PROPERTY_DEF(PRAGUE_CONTEXT,  PTR,  0x0021)

// reopen data
//KLAV_PROPERTY_DEF(REOPEN_DATA,     STR,  0x0022)
// subobject flags: KLAV_OBJSYS_F_XXX
KLAV_PROPERTY_DEF(OBJSYS_FLAGS,    UINT, 0x0023)

// parent object context (excluding sub-part)
KLAV_PROPERTY_DEF(MASTER_PARENT_CONTEXT, PTR, 0x0024)
// root parent context (top-level parent object)
KLAV_PROPERTY_DEF(ROOT_PARENT_CONTEXT,   PTR, 0x0025)

// packer/archiver related properties
// user-readable name
KLAV_PROPERTY_DEF(TRANSFORMER_NAME, STR, 0x0028)
// extended information
KLAV_PROPERTY_DEF(TRANSFORMER_INFO, STR, 0x0029)
// flags (transformer type, etc)
KLAV_PROPERTY_DEF(TRANSFORMER_FLAGS, UINT, 0x002A)

// object-related properties
KLAV_PROPERTY_DEF(OBJECT,           PTR,  0x0100)

KLAV_PROPERTY_DEF(OBJECT_NAME_ANSI, STR,  0x0101)  // obsolete, for compatibility
KLAV_PROPERTY_DEF(OBJECT_NAME,      STR,  0x0101)  // alias for OBJECT_NAME_ANSI
//KLAV_PROPERTY_DEF(OBJECT_NAME_UTF8, STR,  0x0102)
KLAV_PROPERTY_DEF(OBJECT_PATH,      STR,  0x0102)  // alias for OBJECT_NAME_UTF8

KLAV_PROPERTY_DEF(OBJECT_ENTRY_NO,  UINT, 0x0108)
KLAV_PROPERTY_DEF(OBJECT_MFLAGS,    UINT, 0x0110)
KLAV_PROPERTY_DEF(OBJECT_RFLAGS,    UINT, 0x0120)
KLAV_PROPERTY_DEF(OBJECT_HASH,      LONG, 0x0121)
KLAV_PROPERTY_DEF(OBJECT_OFFSET,    LONG, 0x0122)
KLAV_PROPERTY_DEF(OBJECT_ORIGIN,    LONG, 0x0123)

KLAV_PROPERTY_INH(OBJECT_INHER_FLAGS, UINT, 0x0130)

KLAV_PROPERTY_DEF(OBJECT_AVZSR,     UINT, 0x0140)

// link tracing
//KLAV_PROPERTY_DEF(LINK_ID,         UINT, 0x0201)
//KLAV_PROPERTY_DEF(LINK_NAME,       STR,  0x0202)
//KLAV_PROPERTY_DEF(LINK_DATA,       PTR,  0x0203)

// detect-related properties
KLAV_PROPERTY_DEF(VERDICT_NAME,       STR,  0x0301)
KLAV_PROPERTY_DEF(VERDICT_SHORT_NAME, STR,  0x0302)
KLAV_PROPERTY_DEF(VERDICT_ID,         UINT, 0x0303)
KLAV_PROPERTY_DEF(VERDICT_RECORD_ID,  UINT, 0x0304)
KLAV_PROPERTY_DEF(VERDICT_TYPE,       UINT, 0x0305)
KLAV_PROPERTY_DEF(VERDICT_DANGER,     UINT, 0x0306)
KLAV_PROPERTY_DEF(VERDICT_CONFIDENCE, UINT, 0x0307)
KLAV_PROPERTY_DEF(VERDICT_CURABILITY, UINT, 0x0308)
KLAV_PROPERTY_DEF(VERDICT_BEHAVIOUR,  UINT, 0x0309)

// Engine configuration properties
// debug flags (enable debug events)
KLAV_PROPERTY_DEF(DEBUG_FLAGS,        UINT, 0x1001)
// processing flags (KLAV_PF_XXX)
KLAV_PROPERTY_DEF(PROC_FLAGS,         UINT, 0x1002)
// database manifest name
KLAV_PROPERTY_DEF(DB_NAME,            STR,  0x1003)
// database load flags
KLAV_PROPERTY_DEF(DB_FLAGS,           UINT, 0x1004)
// database release timestamp (64-bit time_t)
KLAV_PROPERTY_DEF(DB_TIMESTAMP,       ULONG, 0x1005)
// database record count
KLAV_PROPERTY_DEF(DB_RECORD_COUNT,    UINT, 0x1006)
// Client API version
KLAV_PROPERTY_DEF(CLIENT_API_VERSION, UINT, 0x1010)
// Engine version
KLAV_PROPERTY_DEF(ENGINE_VERSION,     UINT, 0x1011)
// Engine version
KLAV_PROPERTY_DEF(ENGINE_KERNEL_INFO, UINT, 0x1012)

// AVP3 engine callback parameter (OBSOLETE, TO BE REMOVED)
KLAV_PROPERTY_DEF(AVP3_CALLBACK_CODE, UINT, 0x1030)
KLAV_PROPERTY_DEF(AVP3_CALLBACK_PARM, PTR,  0x1031)
KLAV_PROPERTY_DEF(AVP3_WORKAREA,      PTR,  0x1032)

// Scripting engine properties
KLAV_PROPERTY_DEF(SCRIPT_FORMAT,      UINT, 0x1100)
KLAV_PROPERTY_DEF(SCRIPT_FLAGS,       UINT, 0x1101)

// Heuristic engine properties
KLAV_PROPERTY_DEF(HEURISTIC_LEVEL,    UINT, 0x1200)

// Password property
KLAV_PROPERTY_DEF(PASSWORD,           UINT, 0x1300)

// GETAPI property
KLAV_PROPERTY_DEF(GETAPI_LIBRARY,     STR,  0x1310)
KLAV_PROPERTY_DEF(GETAPI_PROC,        STR,  0x1311)

// Hierarchy
KLAV_PROPERTY_DEF(PARENT_LEVEL,       UINT, 0x1400)

// Disk properties
KLAV_PROPERTY_DEF(DISK,               UINT, 0x1501)
KLAV_PROPERTY_DEF(DISK_NUMBER,        UINT, 0x1500)
KLAV_PROPERTY_DEF(DISK_OBJECT_LOGICAL,       PTR,  0x1502)
KLAV_PROPERTY_DEF(DISK_OBJECT_PHYSICAL,      PTR,  0x1503)
KLAV_PROPERTY_DEF(DISK_BYTES_PER_SECTOR,     UINT, 0x1504)
KLAV_PROPERTY_DEF(DISK_TRACKS_PER_CYLINDER,  UINT, 0x1505)
KLAV_PROPERTY_DEF(DISK_SECTORS_PER_TRACK,    UINT, 0x1506)
KLAV_PROPERTY_DEF(DISK_CYLINDERS,            UINT, 0x1507)

KLAV_PROPERTY_DEF(VLNS_SECURE_VERSION,      STR,  0x1601)
KLAV_PROPERTY_DEF(VLNS_CURRENT_VERSION,		STR,  0x1602)
KLAV_PROPERTY_DEF(VLNS_END_OF_LIFE,			UINT,  0x1603)

#undef KLAV_PROPERTY_INH
#undef KLAV_PROPERTY_DEF

#endif // KLAV_PROPERTY_DEF

////////////////////////////////////////////////////////////////
// CALLBACK DEFINITIONS

#ifdef KLAV_CALLBACK_DEF

// Rotate operations
//KLAV_CALLBACK_DEF(ROTATE,             0x0102) // force rotate (e.g. in tight loops)
//KLAV_CALLBACK_DEF(ROTATE_OFF,         0x0103) // may rotate (low priority)

// Error reporting
// Additional properties:
// KLAV_PROPID_ERROR_CODE
// KLAV_PROPID_ERROR_INFO

KLAV_CALLBACK_DEF(EXCEPTION,         0x0120)

// Tracing ($trace)
// Additional properties:
// KLAV_PROPID_TRACE_ID
// KLAV_PROPID_TRACE_INFO

KLAV_CALLBACK_DEF(TRACEPOINT,        0x0130)

// Debug events
// Additional properties:
// KLAV_PROPID_DEBUG_EVENT_CLASS
// KLAV_PROPID_DEBUG_EVENT_CODE
// KLAV_PROPID_DEBUG_EVENT_INFO
// other properties 
KLAV_CALLBACK_DEF(DEBUG_EVENT,       0x0134)

// Performance data
// Additional propeties:
// KLAV_PROPID_PERF_DATA
KLAV_CALLBACK_DEF(PERF_DATA,         0x0138)

// Object life cycle
// Additional properties:
//   KLAV_PROPID_OBJECT
//   KLAV_PROPID_OBJECT_NAME
//   KLAV_PROPID_OBJECT_FULL_NAME
//   KLAV_PROPID_OBJECT_RFLAGS
KLAV_CALLBACK_DEF(OBJECT_BEGIN, 	 0x020B)
KLAV_CALLBACK_DEF(OBJECT_DONE,       0x0200)

//KLAV_CALLBACK_DEF(OBJECT_MESS,       0x0201) // ScanObject* (ScanObject->VirName is *message)

// Detection
// Additional properties:
//   KLAV_PROPID_VERDICT_NAME
//   KLAV_PROPID_VERDICT_SHORT_NAME
//   KLAV_PROPID_VERDICT_ID
//   KLAV_PROPID_VERDICT_RECORD_ID
//   KLAV_PROPID_VERDICT_TYPE
//   KLAV_PROPID_VERDICT_DANGER
//   KLAV_PROPID_VERDICT_CONFIDENCE
//   KLAV_PROPID_VERDICT_CURABILITY
//
// Return value: nonzero - cancel processing
KLAV_CALLBACK_DEF(OBJECT_DETECT,       0x0202)

KLAV_CALLBACK_DEF(OBJECT_NAME,         0x0203)

KLAV_CALLBACK_DEF(OBJECT_CHECK,        0x0204)

// Archive:
//   KLAV_PROPID_OBJECT_OFFSET
KLAV_CALLBACK_DEF(OBJECT_ARCHIVE_BEGIN,0x0206)

//   KLAV_PROPID_TRANSFORMER_NAME
//   KLAV_PROPID_OBJECT_HASH
KLAV_CALLBACK_DEF(OBJECT_ARCHIVE,      0x0207)

//   no props
KLAV_CALLBACK_DEF(OBJECT_ARCHIVE_DONE, 0x020C)

// Status:
KLAV_CALLBACK_DEF(OBJECT_CORRUPTED,    0x020D)
KLAV_CALLBACK_DEF(OBJECT_ENCRYPTED,    0x020E)

// Password:
//   KLAV_PROPID_PASSWORD
KLAV_CALLBACK_DEF(REGISTER_PASSWORD,   0x020F) //char* password
KLAV_CALLBACK_DEF(GET_PASSWORD,        0x0211) //char* password

// GetApi:
//   KLAV_PROPID_GETAPI_LIBRARY
//   KLAV_PROPID_GETAPI_PROC
KLAV_CALLBACK_DEF(GETAPI,              0x0210)

// Cure
KLAV_CALLBACK_DEF(OBJECT_CURED,        0x0300)
KLAV_CALLBACK_DEF(OBJECT_CURE_FAILED,  0x0301)

// Heuristic engine callbacks
KLAV_CALLBACK_DEF(HEURISTIC_PROCESS_PARENT, 0x0500)

KLAV_CALLBACK_DEF(PARENT_GET_INFO,     0x0600)

// Get sector image
KLAV_CALLBACK_DEF(GET_SECTOR_IMAGE,    0x0700)

// AVP3 callback retranslation
// Additional properties:
// AVP3_CALLBACK_CODE
// AVP3_CALLBACK_PARM
// AVP3_WORKAREA
KLAV_CALLBACK_DEF(AVP3,                  0x0700)


////////////////////////////////////////////////////////////////
//  !!!!!!!!!

//KLAV_CALLBACK_DEF(OBJECT_CURE,         0x0203) // ScanObject*
//KLAV_CALLBACK_DEF(OBJECT_WARNING,      0x0204) // ScanObject* // OBSOLETE, TO BE REMOVED
//KLAV_CALLBACK_DEF(OBJECT_SUSP,         0x0205) // ScanObject* // OBSOLETE, TO BE REMOVED

//KLAV_CALLBACK_DEF(ASK_CURE,           0x0107) // ScanObject*                     0-skip; 1-try to disinfect; 2-delete; 
//KLAV_CALLBACK_DEF(LOAD_PROGRESS,      0x0108) // LOAD_PROGRESS_STRUCT*
//KLAV_CALLBACK_DEF(LINK_NAME,          0x0109) // const char* sourcename DEBUG!
//KLAV_CALLBACK_DEF(ASK_DELETE,         0x010A) // ScanObject* (For DELETE method) 2-delete; other-skip
//KLAV_CALLBACK_DEF(GET_SECTOR_IMAGE,   0x010B) // -  (For overwriting sectors)    BYTE* SectorImage
//KLAV_CALLBACK_DEF(LOAD_RECORD_NAME,   0x0110) // const char* string
//KLAV_CALLBACK_DEF(LOAD_BASE,          0x0111) // KLAV_LOAD_PROGRESS_STRUCT*  0-load, 1-skip this base.
//KLAV_CALLBACK_DEF(LOAD_BASE_DONE,     0x0112) // KLAV_LOAD_PROGRESS_STRUCT* 
//KLAV_CALLBACK_DEF(ASK_CURE_MEMORY,    0x0113) // ScanObject*           !!!!!!!!! 0-disinfect; 1-disinfect&clear detection 2-skip; 

//KLAV_CALLBACK_DEF(OBJ_NAME,           0x010C) // char* name (!!!! if it returns 17 - engine will skip file)

// Packed object:
// Additional properties
//   KLAV_PROPID_TRANSFORMER_NAME
//KLAV_CALLBACK_DEF(OBJECT_PACKED,       0x0206)
//KLAV_CALLBACK_DEF(OBJECT_PACKED_DONE,  0x0240)

//KLAV_CALLBACK_DEF(OBJECT_DELETE,       0x0208) // ScanObject*
//KLAV_CALLBACK_DEF(OBJECT_UNKNOWN,      0x0209) // ScanObject*
//KLAV_CALLBACK_DEF(OBJECT_CURE_FAIL,    0x020A) // ScanObject*
//KLAV_CALLBACK_DEF(OBJECT_ARCHIVE_NAME, 0x020D) // ScanObject* (ScanObject->VirName is *name)(if it returns nonzero - engine will skip file)

//KLAV_CALLBACK_DEF(OBJECT_SKIP_REQEST,  0x0241) // ScanObject* return:bitfield 1 - skip AV, 2- skip extract.

//KLAV_CALLBACK_DEF(WA_SUSP,             0x0210) // WorkArea*
//KLAV_CALLBACK_DEF(WA_CORRUPT,          0x0211) // WorkArea*
//KLAV_CALLBACK_DEF(WA_ARCHIVE_NAME,     0x0212) // WorkArea*
//KLAV_CALLBACK_DEF(WA_PACKED_NAME,      0x0213) // WorkArea*
//KLAV_CALLBACK_DEF(WA_PACKED,           0x0214) // WorkArea*
//KLAV_CALLBACK_DEF(WA_ARCHIVE,          0x0215) // WorkArea*

//KLAV_CALLBACK_DEF(WA_PROCESS_ARCHIVE,  0x030E) // WorkArea* (transfer to external extractors)(if it returns nonzero - engine will not call internal extractors)
//KLAV_CALLBACK_DEF(WA_PROCESS_OBJECT,   0x030F) // WorkArea* (transfer to external scaner)(if it returns nonzero - engine will not call internal processing)

//KLAV_CALLBACK_DEF(CHECK_FILE_INTEGRITY,      0x0800) // char* filename (returns: 0 - OK, other - failed)
//KLAV_CALLBACK_DEF(FILE_INTEGRITY_FAILED,     0x0801) // char* filename (returns: 0 - continue, other - stop operation)
//KLAV_CALLBACK_DEF(SETFILE_INTEGRITY_FAILED,  0x0802) // char* filename (returns: 0 - continue, other - stop operation)

//KLAV_CALLBACK_DEF(FILE_OPEN_FAILED_TRY_AGAIN,    0x0806) // char* filename (returns: 0 - skip, other - try again)
//KLAV_CALLBACK_DEF(SETFILE_OPEN_FAILED_TRY_AGAIN, 0x0807) // char* filename (returns: 0 - skip, other - try again)

//KLAV_CALLBACK_DEF(INTERNAL,                   0x1000)
//KLAV_CALLBACK_DEF(INTERNAL_CHECK_DIR,         0x1001)
//KLAV_CALLBACK_DEF(INTERNAL_MESSAGE,           0x1002)
//KLAV_CALLBACK_DEF(INTERNAL_SHOW,              0x1003)
//KLAV_CALLBACK_DEF(INTERNAL_NOTIFY,            0x1004)

//KLAV_CALLBACK_DEF(SAVE_PAGES,          0x1011) // WorkArea*
//KLAV_CALLBACK_DEF(SAVE_PACKED,         0x1012) // WorkArea*
//KLAV_CALLBACK_DEF(SAVE_ARCHIVED,       0x1013) // WorkArea*

//KLAV_CALLBACK_DEF(SAVE_MARKS,          0x1014) // KLAV_SAVE_MARK_STRUCT*

//KLAV_CALLBACK_DEF(WA_RECORD,           0x2000)
//KLAV_CALLBACK_DEF(IS_PRAGUE_HERE,      0x3000)
//KLAV_CALLBACK_DEF(LINK_BEGIN,          0x4000) //Void* function code
//KLAV_CALLBACK_DEF(LINK_DONE,           0x4001) //Void* function code

//  !!!!!!!!!
////////////////////////////////////////////////////////////////

#undef KLAV_CALLBACK_DEF
#endif // KLAV_CALLBACK_DEF

////////////////////////////////////////////////////////////////
// Performance slot definitions

#ifdef KLAV_PERF_SLOT_DEF

KLAV_PERF_SLOT_DEF(0x00, COUNTER, COUNT)
KLAV_PERF_SLOT_DEF(0x01, COUNTER, TREE_BYTES_SCANNED)
KLAV_PERF_SLOT_DEF(0x02, COUNTER, TREE_SIG_FOUND)
KLAV_PERF_SLOT_DEF(0x03, COUNTER, RGN_SIG_FOUND)
KLAV_PERF_SLOT_DEF(0x04, COUNTER, RGN_SIG_MISS)

KLAV_PERF_SLOT_DEF(0x10, COUNTER, LAST)

#undef KLAV_PERF_SLOT_DEF
#endif // KLAV_PERF_SLOT_DEF

////////////////////////////////////////////////////////////////
