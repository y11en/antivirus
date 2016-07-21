/*-----------------02.05.99 12:57-------------------
* Project Prague                                 *
* Subproject Types definition                    *
* Author Andrew Andy Petrovitch                  *
* Copyright (c) Kaspersky Lab                    *
* Purpose Prague API                             *
*                                                *
--------------------------------------------------*/

#ifndef __PR_TYPES_H
#define __PR_TYPES_H

#if !defined(_SIZE_T_DEFINED)
	#if defined (__unix__)
		#include <sys/types.h>
	#elif defined( _WIN32 )
		#if defined(_M_X64) || defined(_M_IA64) || defined(_WIN64)
			typedef unsigned __int64    size_t;
		#elif defined( _W64 )
			typedef _W64 unsigned int   size_t;
		#else
			typedef unsigned int        size_t;
		#endif
		#define _SIZE_T_DEFINED
	#endif
#endif

#define num_to_str2(x) #x
#define num_to_str(x) num_to_str2(x)
#define _MSG(msg)  message( __FILE__ "(" num_to_str(__LINE__) "):" msg )
#define _WARN(msg) message( __FILE__ "(" num_to_str(__LINE__) "): warning: " msg )
#define _TODO(msg) message( __FILE__ "(" num_to_str(__LINE__) "): TODO: " msg )


#if defined(_WIN64)
	//#pragma _MSG( "64 bit compilation, integral type maximum size is " num_to_str(_INTEGRAL_MAX_BITS) )
#elif defined(_Wp64)
	//#pragma _MSG( "64 bit compatibility mode compilation, integral type maximum size is " num_to_str(_INTEGRAL_MAX_BITS) )
#else
	//#pragma _MSG( "32 bit compilation, integral type maximum size is " num_to_str(_INTEGRAL_MAX_BITS) )
#endif

// type definitions

#define cDATETIME_BYTE_SIZE 8


#if !defined(PR_tDWORD_defined)
#define PR_tDWORD_defined
typedef unsigned int         tDWORD;
#endif

typedef unsigned int         tPROPID;	// property identifier	
typedef void                 tVOID;
typedef unsigned char        tBYTE;
typedef unsigned short       tWORD;
typedef unsigned int         tBOOL;
typedef char                 tCHAR;
typedef char*                tSTRING;

#if 0 && defined (_WIN32) && !defined(_WIN64)
	typedef unsigned short     tWCHAR;
	typedef unsigned short*    tWSTRING;
#else
	#if !defined(_WCHAR_T_DEFINED) && !defined(__midl) 
		#if !defined(__unix__) || !defined (__cplusplus)
			#include <wchar.h>
		#endif
	#endif
	typedef wchar_t            tWCHAR;
	typedef wchar_t*           tWSTRING;
#endif

#if !defined(PR_tERROR_defined)
#define PR_tERROR_defined
typedef long                 tERROR;
#endif

#if defined(__midl)
	typedef unsigned           tPTR;
	typedef unsigned           tCPTR;
#else
	typedef void*              tPTR;
	typedef const void*        tCPTR;
#endif

typedef size_t               tSIZE_T;

typedef signed int           tINT;           // native signed type
typedef unsigned int         tUINT;          // native unsigned type
typedef unsigned long        tULONG;         // unsigned long type
typedef signed char          tSBYTE;
typedef signed short         tSHORT;
typedef signed long          tLONG;
typedef unsigned long        tIID;           // plugin interface identifier
typedef unsigned long        tPID;           // plugin identifier
typedef unsigned long        tIMPL_ID;       // implementaition identifier
typedef unsigned long        tMSG_ID;        // message identifier
typedef unsigned long        tMSGCLS_ID;     // message class identifier
typedef unsigned long        tORIG_ID;       // object origin identifier
typedef unsigned long        tOS_ID;         // os type identifier
typedef unsigned long        tVID;           // vendor ideftifier
typedef unsigned long        tVERSION;       // version of any identifier: High word - version, Low word - subversion
typedef unsigned long        tCODEPAGE;      // codepage identifier
typedef unsigned long        tLCID;          // locale identifier
typedef unsigned char        tDATETIME[cDATETIME_BYTE_SIZE];      // universal time and date storage
typedef double               tDOUBLE;        // double

// generic types
#if defined(_INTEGRAL_MAX_BITS) && (_INTEGRAL_MAX_BITS >= 64)
	typedef unsigned __int64   tQWORD;
	typedef signed __int64     tLONGLONG;
	#if defined(_WIN64) || /*defined(_Wp64) ||*/ defined(__LP64__)
		typedef tLONGLONG        tNATIVE_INT;
		typedef tQWORD           tNATIVE_UINT;
	#else
		typedef tINT             tNATIVE_INT;
		typedef tDWORD           tNATIVE_UINT;
	#endif
#else
	typedef double             tQWORD;
	typedef double             tLONGLONG;
	typedef tINT               tNATIVE_INT;
	typedef tUINT              tNATIVE_UINT;
	#pragma message ("Warning: tQWORD and tLONGLONG treating as double type, divide operation is not integral!")
#endif

#define _toi(a)             ((tNATIVE_INT)(a))
#define _toui(a)            ((tNATIVE_UINT)(a))
#define _toni(a)            ((tNATIVE_INT)(a))
#define _tonui(a)           ((tNATIVE_UINT)(a))
#define _toi16(a)           ((tSHORT)(a))
#define _toi32(a)           ((tINT)(a))
#define _toi64(a)           ((tLONGLONG)(a))
#define _toui16(a)          ((tWORD)(a))
#define _toui32(a)          ((tUINT)(a))
#define _toui64(a)          ((tQWORD)(a))
#define _toptr(a)           ((tPTR)_toni(a))
#define _2ptr(a)            _toptr(a)

typedef tNATIVE_UINT         tDATA;          // universal data storage
typedef tDATETIME            tDT;

#if !defined(__midl)
	typedef void (pr_call*     tFUNC_PTR)();   // universal function ptr
#else
	typedef void (*            tFUNC_PTR)();   // universal function ptr
#endif

#ifndef __APPLE__
#pragma warning( disable : 4201 )
#endif
typedef union tag_EXPORT {
	tDATA       v[3];
	struct {
		tFUNC_PTR func;
		tDATA     cls_id;
		tDATA     fnc_id;
	} /*f*/;
} tEXPORT;                                   // export table record
#ifndef __APPLE__
#pragma warning( default : 4201 )
#endif

typedef union tag_EXPORT tIMPORT;           // import table record


// ---
typedef struct tag_MemChunk {
	tVOID*  m_ptr;
	tDWORD  m_used;
	tDWORD  m_allocated;
} tMemChunk;

// ---
typedef struct tag_StrDsc {
  tPTR      m_ptr;
  tUINT     m_size;
  tCODEPAGE m_cp;
} tStrDsc;


#ifdef __cplusplus
	#define DECLARE_CPP_TYPE( base, type )                \
		struct type {																				\
			base _;																						\
			type() : _(0) {}																	\
			type( base p ) : _(p) {}													\
      type( const type& p ) : _(p._) {} 								\
      operator base() const { return _;	}								\
			operator base*()	{ return &_; }									\
			base operator = (base p) { return _=p; }					\
      base operator = (const type& p) { return _=p._; }	\
    }

	DECLARE_CPP_TYPE( tERROR,    cERROR    );
	DECLARE_CPP_TYPE( tIID,      cIID      );  // plugin interface identifier
	DECLARE_CPP_TYPE( tPID,      cPID      );  // plugin identifier
	DECLARE_CPP_TYPE( tVID,      cVID      );  // vendor ideftifier
	DECLARE_CPP_TYPE( tPROPID,   cPROPID   );  // property identifier
	DECLARE_CPP_TYPE( tCODEPAGE, cCODEPAGE );  // codepage identifier

	struct cSerializable;
	struct cSerString;

#else

	typedef tERROR    cERROR;
	typedef tIID      cIID;
	typedef tPID      cPID;       // plugin identifier
	typedef tVID      cVID;       // vendor ideftifier
	typedef tPROPID   cPROPID;    // property identifier
	typedef tCODEPAGE cCODEPAGE;  // codepage identifier
	
	typedef struct tag_cSerializable cSerializable;
	typedef struct tag_cSerString cSerString;
	
#endif


typedef struct tag_cSerDescriptor cSerDescriptor;
typedef struct tag_cSerDescriptorField cSerDescriptorField;

// global constant definitions
#define cFALSE            ((tBOOL)0)
#define cTRUE             ((tBOOL)(!cFALSE))

#define cMAX_BYTE         ((tUINT)0xff)
#define cMIN_BYTE         ((tUINT)0)

#define cMAX_WORD         ((tUINT)0xffff)
#define cMIN_WORD         ((tUINT)0)

#define cMAX_INT          ((tINT)0x7fffffff)
#define cMIN_INT          ((tINT)0x80000000)

#define cMAX_SHORT_INT    ((tUINT)0x7fff)
#define cMIN_SHORT_INT    ((tUINT)0x8000)

#define cMAX_UINT         ((tUINT)0xffffffff)
#define cMIN_UINT         ((tUINT)0)

#define cMAX_LONGLONG     ((tLONGLONG)LONG_LONG_CONST(0x7fffffffffffffff))
#define cMIN_LONGLONG     ((tLONGLONG)LONG_LONG_CONST(0x8000000000000000))

#define SUBTYPE_ANY       ((tDWORD)(0))

// NULL pointer
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((tPTR)0)
#endif
#endif


// code page constants
#define cCP_NULL                       ((tCODEPAGE)((tDWORD)0))
#define cCP_SYSTEM_DEFAULT             ((tCODEPAGE)((tDWORD)1))
#define cCP_STRING_DEFAULT             ((tCODEPAGE)((tDWORD)2))
#define cCP_OEM                        ((tCODEPAGE)(((tDWORD)-1)-10))
#define cCP_ANSI                       ((tCODEPAGE)(((tDWORD)-1)-11))
#define cCP_IBM                        ((tCODEPAGE)(((tDWORD)-1)-12))
#define cCP_UTF8                       ((tCODEPAGE)(((tDWORD)-1)-13))
#define cCP_UNICODE                    ((tCODEPAGE)((tDWORD)1200))
#define cCP_UNICODE_BIGENDIAN          ((tCODEPAGE)((tDWORD)1201))

// message class constant
#define MC_ANY                         ((tDWORD)0)


// --- 
typedef enum tag_TRACE_LEVEL {
	prtALWAYS_REPORTED_MSG       = 0,
	prtFATAL                     = 100,
	prtDANGER                    = 200,
	prtERROR                     = 300,
	prtWARNING                   = 350,
	prtIMPORTANT                 = 400,
	prtNOTIFY                    = 500,
	prtNOT_IMPORTANT             = 600,
	prtSPAM                      = 700,
  prtSYNCLOCK                  = 750,
  prtANNOYING                  = 800,
	prtFUNCTION_FRAME            = 900,
	
	// trace level definitions (property value and TraceLevelSet, TraceLevelGet methods)
	prtMIN_TRACE_LEVEL           = 0,
	prtMAX_TRACE_LEVEL           = cMAX_INT,
	prtDO_NOT_CHANGE_TRACE_LEVEL = cMAX_UINT
} tTRACE_LEVEL;                               // trace level definitions (Trace method constants)


// propety types definition, maximum 255
typedef enum tag_TYPE_ID {

	// base types
	tid_VOID        = 0x01L,
	tid_BYTE        = 0x02L,
	tid_WORD        = 0x03L,
	tid_DWORD       = 0x04L,
	tid_QWORD       = 0x05L,
	tid_BOOL        = 0x06L,
	tid_CHAR        = 0x07L,
	tid_WCHAR       = 0x08L,
	tid_STRING      = 0x09L,
	tid_WSTRING     = 0x0aL,
	tid_ERROR       = 0x0bL,
	tid_PTR         = 0x0cL,
	tid_INT         = 0x0dL, // native signed integer type
	tid_UINT        = 0x0eL, // native unsigned integer type
	tid_SBYTE       = 0x0fL,

	tid_SHORT       = 0x11L,
	tid_LONG        = 0x12L,
	tid_LONGLONG    = 0x13L,
	tid_IID         = 0x14L, // plugin interface identifier
	tid_PID         = 0x15L, // plugin identifier
	tid_ORIG_ID     = 0x16L, // object origin identifier
	tid_OS_ID       = 0x17L, // folder type identifier
	tid_VID         = 0x18L, // vendor ideftifier
	tid_PROPID      = 0x19L, // property identifier
	tid_VERSION     = 0x1aL, // version of any identifier: High word - version, Low word - subversion
	tid_CODEPAGE    = 0x1bL, // codepage identifier
	tid_LCID        = 0x1cL, // codepage identifier
	tid_DATA        = 0x1dL, // universaL, data storage
	tid_DATETIME    = 0x1eL, // universaL, date storage
	tid_FUNC_PTR    = 0x1fL, // universaL, function ptr
	tid_DOUBLE      = 0x20L, // double

	// advanced types
	tid_BINARY        = 0x30L,
	tid_IFACEPTR      = 0x31L,
	tid_OBJECT        = 0x32L,
	tid_EXPORT        = 0x33L,
	tid_IMPORT        = 0x34L,
	tid_TRACE_LEVEL   = 0x35L,
	tid_TYPE_ID       = 0x36L,
	tid_VECTOR        = 0x37L,
	tid_STRING_OBJ    = 0x38L,
	tid_SERIALIZABLE  = 0x39L,
	tid_BUFFER        = 0x3aL,
	tid_STR_DSC       = 0x3bL,

	// ---
	tid_SIZE_T        = 0x3cL,
	tid_IMPL_ID       = 0x3dL,
	tid_MSG_ID        = 0x3eL,
	tid_MSGCLS_ID     = 0x3fL,

	// ---
	// last change 18.05.07
	// see also :
	// pr_prop.h:135 ( pTYPE_... defines )
	// serializer.cpp:58 ( g_typeSize[] )
	// dtreg.c:1819 ( check_type() )
	// -----------------------------------------
	// search phrase: "correspond to pr_types.h"

	tid_LAST_TYPE     = 0x40L,
	tid_POINTER       = 0x80000000L,
} tTYPE_ID;

#endif
