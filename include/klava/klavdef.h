// klavdef.h
//

#ifndef klavdef_h_INCLUDED
#define klavdef_h_INCLUDED

#include <kl_types.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef __cplusplus
# define KLAV_EXTERN_C extern "C"
#else // __cplusplus
# define KLAV_EXTERN_C
#endif // __cplusplus

#ifndef KL_CDECL
# error KL_CDECL not defined
#endif

#define KLAV_CDECL KL_CDECL
#define KLAV_NO_VTABLE KL_NO_VTABLE

#define KLAV_CALL KLAV_CDECL
#define KLAV_METHOD(RETTYPE, NAME) virtual RETTYPE KLAV_CALL NAME

////////////////////////////////////////////////////////////////

# ifdef _MSC_VER
#  define KLAV_DLL_EXPORT __declspec(dllexport)
# else
#  define KLAV_DLL_EXPORT
# endif

////////////////////////////////////////////////////////////////
// ensure proper packing level on Win32/Win64 platforms

#if defined KL_PLATFORM_WIN64
# pragma pack(16)
#elif defined KL_PLATFORM_WIN32
# pragma pack(8)
#endif

////////////////////////////////////////////////////////////////
// default file extension for dynamic shared objects (DLLs)

// native conventions
#if defined KL_PLATFORM_WINDOWS
# define KLAV_DSO_PREFIX ""
# define KLAV_DSO_SUFFIX ".dll"
#elif defined KL_PLATFORM_MACOSX
# define KLAV_DSO_PREFIX "lib"
# define KLAV_DSO_SUFFIX ".dylib"
#else
# define KLAV_DSO_PREFIX "lib"
# define KLAV_DSO_SUFFIX ".so"
#endif

#define KLAV_DSO_NAME(NAME) NAME KLAV_DSO_SUFFIX
#define KLAV_DSO_PREFIX_NAME(NAME) KLAV_DSO_PREFIX NAME KLAV_DSO_SUFFIX

// KLAVA engine modules
#define KLAV_MOD_SUFFIX ".kdl"
#define KLAV_MOD_NAME(NAME) NAME KLAV_MOD_SUFFIX

////////////////////////////////////////////////////////////////
// Errors

#include <klava/klaverr.h>

////////////////////////////////////////////////////////////////
// Forward declarations

// Forward class declaration helper
#ifdef __cplusplus
# define KLAV_FORWARD_CLASS(NAME) class NAME
#else
// class declarartions are meaningless for C, just declare name to make compiler happy
# define KLAV_FORWARD_CLASS(NAME) typedef NAME
#endif

#define KLAV_IFACE_DECL(NAME) \
struct KLAV_##NAME; \
typedef struct KLAV_##NAME * hKLAV_##NAME;

KLAV_IFACE_DECL(Alloc)
KLAV_IFACE_DECL(Mutex)
KLAV_IFACE_DECL(RWLock)
KLAV_IFACE_DECL(CondVar)
KLAV_IFACE_DECL(Sync_Factory)
KLAV_IFACE_DECL(Properties)
KLAV_IFACE_DECL(Property_Bag)
KLAV_IFACE_DECL(Callback_Handler)
KLAV_IFACE_DECL(Message_Printer)
KLAV_IFACE_DECL(Yield_Handler)
KLAV_IFACE_DECL(Object_System_Manager)
KLAV_IFACE_DECL(DB_Loader)
KLAV_IFACE_DECL(DB_Manager)
KLAV_IFACE_DECL(DSO_Loader)
KLAV_IFACE_DECL(Virtual_Memory)
KLAV_IFACE_DECL(IO_Library)
KLAV_IFACE_DECL(Temp_Object_Manager)
KLAV_IFACE_DECL(Component)
KLAV_IFACE_DECL(Component_Factory)
KLAV_IFACE_DECL(DB_Fragment)
KLAV_IFACE_DECL(Context)
KLAV_IFACE_DECL(Kernel)
KLAV_IFACE_DECL(Proc_Resolver)

KLAV_IFACE_DECL(Debug_API)
KLAV_IFACE_DECL(Debug_Proc_Resolver)

typedef hKLAV_Context KLAV_CONTEXT_TYPE;

////////////////////////////////////////////////////////////////
// Trace levels

typedef enum
{
	KLAV_TRACE_LEVEL_DEBUG,
	KLAV_TRACE_LEVEL_PERF,
	KLAV_TRACE_LEVEL_INFO,
	KLAV_TRACE_LEVEL_WARNING,
	KLAV_TRACE_LEVEL_ERROR
}
KLAV_TRACE_LEVEL;

////////////////////////////////////////////////////////////////
// Helpers for 64-bit structure member padding
////////////////////////////////////////////////////////////////

#define KLAV_PAD64(TYPE,NAME)  union { TYPE NAME; uint64_t NAME##_pad; }

// N must be a power of 2
#define KLAV_ALIGN(A,N) (((A) + ((N)-1)) & ~((N)-1))

////////////////////////////////////////////////////////////////
// Dummy interface method implementation
////////////////////////////////////////////////////////////////

#define KLAV_DUMMY_METHOD_DECL(NAME) \
	virtual KLAV_ERR KLAV_CALL NAME () = 0;

#define KLAV_DUMMY_METHOD_IMPL(NAME) \
	virtual KLAV_ERR KLAV_CALL NAME () { return KLAV_ENOTIMPL; }

#define KLAV_DUMMY_METHODS_DECL(IFC_NAME) \
	KLAV_DUMMY_METHOD_DECL(dummy01) \
	KLAV_DUMMY_METHOD_DECL(dummy02) \
	KLAV_DUMMY_METHOD_DECL(dummy03) \
	KLAV_DUMMY_METHOD_DECL(dummy04)

#ifdef __cplusplus
template <class IFC> struct KLAV_Iface_Impl : public IFC
{
	KLAV_DUMMY_METHOD_IMPL(dummy01)
	KLAV_DUMMY_METHOD_IMPL(dummy02)
	KLAV_DUMMY_METHOD_IMPL(dummy03)
	KLAV_DUMMY_METHOD_IMPL(dummy04)
};
#define KLAV_IFACE_IMPL(NAME) KLAV_Iface_Impl <NAME>
#endif // __cplusplus

////////////////////////////////////////////////////////////////
// Helpers for C interface declarations
////////////////////////////////////////////////////////////////

#ifndef __cplusplus

#define KLAV_DECLARE_C_INTERFACE_VTBL(NAME) \
struct NAME##_VTBL

#define KLAV_DECLARE_C_INTERFACE(NAME) \
struct NAME \
{ \
	struct NAME##_VTBL *vtbl; \
}

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// Load options
// TODO: these are obsolete, remove

#define KLAV_ENGINE_OPTION_F_MULTIFILE_DB 0x00001000
#define KLAV_ENGINE_OPTION_F_AVP_MANAGER  0x00002000

////////////////////////////////////////////////////////////////
// Common bool type replacement for C/C++

typedef int klav_bool_t;

////////////////////////////////////////////////////////////////
// File position data type

// TODO: create customized definitions for different platforms
typedef uint64_t klav_filepos_t;
typedef int64_t  klav_sfilepos_t;  // signed

////////////////////////////////////////////////////////////////

typedef uint32_t (KLAV_CALL * KLAV_PFN)(KLAV_CONTEXT_TYPE, ...);

////////////////////////////////////////////////////////////////
// Verdict confidence

// exact detection, (iEngine: SURE)
#define KLAV_VERDICT_CONFIDENCE_SURE             (0x0)
#define KLAV_VERDICT_CONFIDENCE_DETECT           KLAV_VERDICT_CONFIDENCE_SURE
// partial code match, (iEngine: PARTIAL)
#define KLAV_VERDICT_CONFIDENCE_PARTIAL          (0x1)
#define KLAV_VERDICT_CONFIDENCE_VARIANT          KLAV_VERDICT_CONFIDENCE_PARTIAL
// detection by heuristic algorithm
#define KLAV_VERDICT_CONFIDENCE_HEURISTIC        (0x2)
#define KLAV_VERDICT_CONFIDENCE_GENERIC          KLAV_VERDICT_CONFIDENCE_HEURISTIC
// probable code match
#define KLAV_VERDICT_CONFIDENCE_PROBABLE         (0x3)

#define KLAV_VERDICT_CONFIDENCE_FALSE_ALARM      (0x7)
#define KLAV_VERDICT_CONFIDENCE_NEGATIVE         KLAV_VERDICT_CONFIDENCE_FALSE_ALARM

// greater confidence levels have lesser values...
#define KLAV_VERDICT_CONFIDENCE_GREATER(A,B)     ((A)<(B))

// disinfectability status
#define KLAV_VERDICT_CURABILITY_INCURABLE        (0x0)
#define KLAV_VERDICT_CURABILITY_CURABLE          (0x1)

////////////////////////////////////////////////////////////////
// Standard kernel-callable interfaces

#define KLAV_STD_IFACE_COUNT 0x10

#define KLAV_IFACE_ID(NAME) KLAV_IFACE_##NAME

enum KLAV_STD_IFACES
{
	KLAV_IFACE_ID(Alloc)            = 0x00,  // KLAV_Alloc
	KLAV_IFACE_ID(Callback_Handler) = 0x01,  // KLAV_Callback_Handler
	KLAV_IFACE_ID(Sync_Factory)     = 0x02,  // KLAV_Sync_Factory
	KLAV_IFACE_ID(DB_Loader)        = 0x03,  // KLAV_DB_Loader
	KLAV_IFACE_ID(Message_Printer)  = 0x04,  // KLAV_Message_Printer
	KLAV_IFACE_ID(DSO_Loader)       = 0x05,  // KLAV_DSO_Loader
	KLAV_IFACE_ID(IO_Library)       = 0x06,  // KLAV_IO_Library
	KLAV_IFACE_ID(Yield_Handler)    = 0x07,  // KLAV_Yield_Handler
	KLAV_IFACE_ID(Object_System_Manager) = 0x08,  // hierarchial object system manager
	KLAV_IFACE_ID(Proc_Resolver)         = 0x09,  // procedure resolver
	KLAV_IFACE_ID(Debug_Proc_Resolver)   = 0x0A,  // debug procedure resolver
	KLAV_IFACE_ID(Temp_Object_Manager)   = 0x0B,  // temporary object manager
	KLAV_IFACE_ID(Component_Factory)     = 0x0C,  // component factory (pointer to function)
	KLAV_IFACE_ID(Virtual_Memory)        = 0x0D,  // virtual memory manager
	KLAV_IFACE_ID(DB_Manager)            = 0x0E,  // database file manager
	KLAV_IFACE_ID(Debug_API)             = 0x0F,  // debug API
};

#ifdef __cplusplus
# define KLAV_IFACE_CAST(NAME,PTR) (static_cast <hKLAV_##NAME>(PTR))
#else  // __cplusplus
# define KLAV_IFACE_CAST(NAME,PTR) ((hKLAV_##NAME)(PTR))
#endif // __cplusplus

#define KLAV_GET_IFACE(OBJ,NAME)     KLAV_IFACE_CAST (NAME, (OBJ)->get_iface (KLAV_IFACE_ID(NAME)))
#define KLAV_SET_IFACE(OBJ,NAME,PTR) (OBJ)->set_iface (KLAV_IFACE_ID(NAME), KLAV_IFACE_CAST(NAME,PTR))

////////////////////////////////////////////////////////////////
// Memory allocator

#ifdef __cplusplus

struct KLAV_NO_VTABLE KLAV_Alloc
{
public:
    virtual uint8_t * KLAV_CALL alloc (size_t size) = 0;
    virtual uint8_t * KLAV_CALL realloc (void *ptr, size_t newsize) = 0;
    virtual void      KLAV_CALL free (void *ptr) = 0;
    KLAV_DUMMY_METHODS_DECL(KLAV_Alloc)
};

struct KLAV_Alloc_Guard
{
	KLAV_Alloc_Guard(void* ptr, KLAV_Alloc* alloc) : m_ptr(ptr), m_alloc(alloc)
	{
	}

	~KLAV_Alloc_Guard()
	{
		if (m_ptr)
			m_alloc->free(m_ptr);
	}
	void* detach()
	{
		void* res = m_ptr;
		m_ptr = 0;
		return res;
	}

protected:
	void* m_ptr;
	KLAV_Alloc* m_alloc;

private:
	KLAV_Alloc_Guard(const KLAV_Alloc_Guard&){}
	KLAV_Alloc_Guard& operator = (const KLAV_Alloc_Guard&) {};
};

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL(KLAV_Alloc)
{
    uint8_t * (KLAV_CALL * alloc)   (struct KLAV_Alloc *, size_t size);
    uint8_t * (KLAV_CALL * realloc) (struct KLAV_Alloc *, void *ptr, size_t newsize);
    void      (KLAV_CALL * free)    (struct KLAV_Alloc *, void *ptr);
};

KLAV_DECLARE_C_INTERFACE(KLAV_Alloc);

#endif // __cplusplus

#ifdef __cplusplus

// klavstl-friendly allocator

class klav_allocator
{
public:
	klav_allocator (KLAV_Alloc *pa)
		: m_alloc (pa) {}

	inline uint8_t * alloc (size_t size)
		{ return m_alloc->alloc (size); }

	inline uint8_t * realloc (void *p, size_t nsz)
		{ return m_alloc->realloc (p, nsz); }

	inline void free (void *p)
		{ m_alloc->free (p); }

	inline operator KLAV_Alloc * () const
		{ return m_alloc; }

	inline KLAV_Alloc * operator-> () const
		{ return m_alloc; }

 static inline klav_allocator create (KLAV_Alloc *alloc)
	{ return klav_allocator (alloc); }

private:
	KLAV_Alloc * m_alloc;
};

////////////////////////////////////////////////////////////////
// adaptors for operator new() and delete()
////////////////////////////////////////////////////////////////

inline void* operator new(size_t size, const klav_allocator& alloc)
{
	return alloc->alloc (size);
}

inline void operator delete (void* p, const klav_allocator& alloc)
{
	alloc->free (p);
}

// disable vectored new/delete operator definitions under MS VC6
#if !defined (_MSC_VER) || _MSC_VER > 1200
inline void* operator new[](size_t size, const klav_allocator& alloc)
{
	return alloc->alloc (size);
}

inline void operator delete[] (void* p, const klav_allocator& alloc)
{
	alloc->free (p);
}
#endif

template <class T>
static inline void klav_delete (T* p, KLAV_Alloc* alloc)
{
	if (p != 0)
	{
		p->~T ();
		alloc->free (p);
	}
}

#define KLAV_NEW(alloc) new (klav_allocator::create (alloc))
#define KLAV_DELETE(p, alloc) klav_delete (p, alloc)

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// Message tracer

#ifdef __cplusplus

struct KLAV_NO_VTABLE KLAV_Message_Printer
{
	virtual KLAV_ERR KLAV_CALL print_message_v (
				KLAV_CONTEXT_TYPE context,
				uint32_t     level,
				const char * fmt,
				va_list ap
			) = 0;
    KLAV_DUMMY_METHODS_DECL(KLAV_Message_Printer)
};

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL(KLAV_Message_Printer)
{
	KLAV_ERR (KLAV_CALL * print_message_v) (
				struct KLAV_Message_Printer *printer,
				KLAV_CONTEXT_TYPE,
				uint32_t     level,
				const char * fmt,
				va_list ap
			);
};

KLAV_DECLARE_C_INTERFACE(KLAV_Message_Printer);

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// Yield handler

#ifdef __cplusplus

struct KLAV_NO_VTABLE KLAV_Yield_Handler
{
	virtual KLAV_ERR KLAV_CALL yield (
				KLAV_CONTEXT_TYPE context,
				uint32_t flags
			) = 0;
    KLAV_DUMMY_METHODS_DECL(KLAV_Yield_Handler)
};

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL(KLAV_Yield_Handler)
{
	KLAV_ERR (KLAV_CALL * yield) (
				struct KLAV_Yield_Handler *,
				KLAV_CONTEXT_TYPE,
				uint32_t
			);
};

KLAV_DECLARE_C_INTERFACE(KLAV_Yield_Handler);

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// Object system manager

// OS manager flags
enum
{
	// flag: subobject is not an object of its own,
	// just a part of original object
	KLAV_OBJSYS_F_SUBPART    = 0x0001,
	// flag: object is directly mapped to parent object data
	KLAV_OBJSYS_F_DIRECT_MAP = 0x0002,
	// flag: object can be reversely packed
	KLAV_OBJSYS_F_REVERSABLE = 0x0004,
};

#ifdef __cplusplus

struct KLAV_NO_VTABLE KLAV_Object_System_Manager
{
	virtual KLAV_ERR KLAV_CALL process_subobject (
			KLAV_CONTEXT_TYPE parent_context,
			void *            object,
			KLAV_Properties * props
		) = 0;
    KLAV_DUMMY_METHODS_DECL(KLAV_Object_System_Manager)
};

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL(KLAV_Object_System_Manager)
{
	KLAV_ERR (KLAV_CALL *process_subobject) (
			struct KLAV_Object_System_Manager *,
			KLAV_CONTEXT_TYPE,
			void *,
			struct KLAV_Properties *
		);
};

KLAV_DECLARE_C_INTERFACE(KLAV_Object_System_Manager);

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// KLAVA operation flags (PROC_FLAGS)

// disable false alarms
#define KLAV_PF_NO_FA              0x00000001
// stream mode
#define KLAV_PF_STREAM             0x00000002
// collect performance data
#define KLAV_PF_PERF_STAT          0x00000004

////////////////////////////////////////////////////////////////
// AVP3 operation flags

// use only AVP3 packers
#define KLAV_PF_AVP3_PACKERS_ONLY  0x00000001
// use only AVP3 bases
#define KLAV_PF_AVP3_ONLY          0x00000002

////////////////////////////////////////////////////////////////
// Mode flags (AVP3-compatible)

#define KLAV_MF_MASK_              0x0000000F
#define KLAV_MF_MASK_SMART         0x00000000
#define KLAV_MF_MASK_PROG          0x00000001
#define KLAV_MF_MASK_ALL           0x00000002
#define KLAV_MF_MASK_USER          0x00000003

#define KLAV_MF_SFX_ARCHIVED       0x00000400
#define KLAV_MF_PACKED             0x00010000
#define KLAV_MF_ARCHIVED           0x00020000
#define KLAV_MF_CA                 0x00040000
#define KLAV_MF_REDUNDANT          0x00080000
#define KLAV_MF_MAILBASES          0x00400000
#define KLAV_MF_MAILPLAIN          0x00800000
#define KLAV_MF_EMBEDDED           0x08000000

#define KLAV_MF_PARTLY_DETECT      0x10000000
#define KLAV_MF_SUSPICIOUS_PACKERS 0x20000000
#define KLAV_MF_MULTI_PACKED       0x40000000
#define KLAV_MF_UNKNOWN_PACKERS    0x80000000
#define KLAV_MF_GREY_PACKERS       0x00000800
#define KLAV_MF_VULNERABILITY      0x04000000
#define KLAV_MF_IGNORE_THREATS     0x02000000

////////////////////////////////////////////////////////////////
// RetFlags (AVP3-compatible)

#define KLAV_RF_DETECT                 0x00000001
#define KLAV_RF_CURE                   0x00000002
#define KLAV_RF_SUSPIC                 0x00000004
#define KLAV_RF_WARNING                0x00000008
#define KLAV_RF_CURE_FAIL              0x00000010
#define KLAV_RF_CORRUPT                0x00000020
#define KLAV_RF_INTERNAL_ERROR         0x00000040
#define KLAV_RF_NOT_A_PROGRAMM         0x00000080
#define KLAV_RF_DELETE                 0x00000100
#define KLAV_RF_DELETED                0x00000200
#define KLAV_RF_ENCRYPTED              0x00000400
#define KLAV_RF_PROTECTED              0x00000800

#define KLAV_RF_DISK_WRITE_PROTECTED   0x00001000
#define KLAV_RF_DISK_NOT_READY         0x00002000
#define KLAV_RF_DISK_OUT_OF_SPACE      0x00004000
#define KLAV_RF_IO_ERROR               0x00008000
#define KLAV_RF_NOT_OPEN               0x00010000
#define KLAV_RF_OUT_OF_MEMORY          0x00020000
#define KLAV_RF_IN_CURABLE_EMBEDDED    0x00040000

#define KLAV_RF_INTERFACE_FAULT        0x00100000
#define KLAV_RF_KERNEL_FAULT           0x00200000
#define KLAV_RF_NOT_SCANNED            0x00400000

#define KLAV_RF_CURE_DELETE            0x00800000

#define KLAV_RF_REPORT_DONE            0x01000000
#define KLAV_RF_PACKED                 0x02000000
#define KLAV_RF_ARCHIVE                0x04000000
#define KLAV_RF_NONCURABLE             0x08000000

#define KLAV_RF_MODIFIED               0x10000000
#define KLAV_RF_EXECUTABLE_FORMAT      0x20000000

#define KLAV_RF_ERROR (KLAV_RF_DISK_NOT_READY | KLAV_RF_DISK_OUT_OF_SPACE | KLAV_RF_IO_ERROR | KLAV_RF_OUT_OF_MEMORY )

////////////////////////////////////////////////////////////////
// Object type (AVP3-compatible)

#define KLAV_OT_FILE                    0x00
#define KLAV_OT_MEMORY_FILE             0x40
#define KLAV_OT_MBR                     0x11
#define KLAV_OT_BOOT                    0x12

////////////////////////////////////////////////////////////////
// temp definitions (TODO: rename)

// values for KLAV_DE_EXCEPTION_CODE
#define KLAV_DE_E_NONE                  0
#define KLAV_DE_E_UNKNOWN               1
#define KLAV_DE_E_PROC_NOT_IMPLEMENTED  2
#define KLAV_DE_E_FILE_OPEN             3
#define KLAV_DE_E_SET                   4
#define KLAV_DE_E_BASE                  5
#define KLAV_DE_E_KERNEL_PROC           6
#define KLAV_DE_E_LINK                  7
#define KLAV_DE_E_SYMBOL_DUP            8
#define KLAV_DE_E_SYMBOL                9
#define KLAV_DE_E_FIXUPP               10
#define KLAV_DE_E_AUTHENTICATION	   11

// values for KLAV_DE_PF_F_MASK
#define KLAV_DE_PF_MASK_SMART           0
#define KLAV_DE_PF_MASK_PROG            1
#define KLAV_DE_PF_MASK_ALL             2
#define KLAV_DE_PF_MASK_USER            3

// values for KLAV_DE_PF_F_DETECT
#define KLAV_DE_PF_DETECT_NOACTION      0
#define KLAV_DE_PF_DETECT_DENYACCESS    1
#define KLAV_DE_PF_DETECT_CUREALL       2
#define KLAV_DE_PF_DETECT_DELETEALL     3
#define KLAV_DE_PF_DETECT_ASKCURE       4
#define KLAV_DE_PF_DETECT_ASKCUREDEBUG  5

////////////////////////////////////////////////////////////////
// Kernel error codes codes

typedef enum
{
	KLAV_EXCEPTION_UNKNOWN = 0,
	KLAV_EXCEPTION_PROC_NOT_IMPLEMENTED, // procedure not implemented
	KLAV_EXCEPTION_INVALID_PARM,         // invalid parameter
	KLAV_EXCEPTION_INVALID_DBFILE,       // invalid database file
	KLAV_EXCEPTION_DIVISION_BY_ZERO,
}
KLAV_EXCEPTION_CODE;

////////////////////////////////////////////////////////////////
// Debug event IDs

#define KLAV_DEBUG_EVENT_CLASS(ID)  (((ID) >> 16) & 0xFFFF)
#define KLAV_DEBUG_EVENT_CODE(ID)   ((ID) & 0xFFFF)

#define KLAV_DEBUG_EVENT_ID(CLASS,CODE)  (((CLASS) << 16) | (CODE))

////////////////////////////////////////////////////////////////

#endif // klavdef_h_INCLUDED

