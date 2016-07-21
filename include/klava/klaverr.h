// klaverr.h
//

#ifndef klaverr_h_INCLUDED
#define klaverr_h_INCLUDED

#include <kl_types.h>

typedef uint32_t KLAV_ERR;

//
//  Error encoding is Prague-compatible
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +-+-----+-------------------------------+-----------------------+
//  |S| Fac.|  ID                           |     CODE              |
//  +-+-----+-------------------------------+-----------------------+
//
//  where
//
//      S - severity flag
//          0  - Success/Warning
//          1  - Error (negative KLAV_ERR values)
//      Fac  - Prague facility code
//      ID   - facility-specific ID
//      CODE - raw error code
//

// fac_PREDECLARED & ID==0
#define KLAV_MAKE_STANDARD_WARN(CODE)   ((uint32_t)(CODE) & 0xFFFU)
#define KLAV_MAKE_STANDARD_ERROR(CODE)  ((0x80000000U) | ((uint32_t)(CODE) & 0xFFFU))
// fac_IMPLEMENTATION_DECLARED & ID=44000 (0xABE0)
#define KLAV_MAKE_CUSTOM_WARN(CODE)     ((0x1ABE0000U) | ((uint32_t)(CODE) & 0xFFFU))
#define KLAV_MAKE_CUSTOM_ERROR(CODE)    ((0x9ABE0000U) | ((uint32_t)(CODE) & 0xFFFU))

////////////////////////////////////////////////////////////////

// fac_NATIVE_OS
#define KLAV_MAKE_SYSTEM_ERROR(ERR)  ((KLAV_ERR)(0xA0000000U | ((uint32_t)(ERR) & 0x0FFFFFFFU)))
#define KLAV_IS_SYSTEM_ERROR(ERR)    (((ERR) & 0xF0000000U) == 0xA0000000U)

////////////////////////////////////////////////////////////////

#define KLAV_SUCCEEDED(err)		(((KLAV_ERR)(err) & 0x80000000) == 0)
#define KLAV_FAILED(err)		(((KLAV_ERR)(err) & 0x80000000) != 0)

#define KLAV_SUCC(err) KLAV_SUCCEEDED(err)
#define KLAV_FAIL(err) KLAV_FAILED(err)

////////////////////////////////////////////////////////////////

// success codes
#define KLAV_OK	   0

// errEOF (not an error)
#define KLAV_EOF   KLAV_MAKE_STANDARD_WARN(0x041)

// Skip action (not an error)
#define KLAV_ACTION_SKIP KLAV_MAKE_CUSTOM_WARN(0x001)

// Cure action (not an error)
#define KLAV_ACTION_CURE KLAV_MAKE_CUSTOM_WARN(0x002)

////////////////////////////////////////////////////////////////
// Errors, mappable to the standard Prague errors

// unexpected condition (errUNEXPECTED)
#define KLAV_EUNEXPECTED		KLAV_MAKE_STANDARD_ERROR(0x040)

// cannot allocate memory (errNOT_ENOUGH_MEMORY)
#define KLAV_ENOMEM				KLAV_MAKE_STANDARD_ERROR(0x041)

// not implemented (errNOT_IMPLEMENTED)
#define KLAV_ENOIMPL			KLAV_MAKE_STANDARD_ERROR(0x043)
// deprecated
#define KLAV_ENOTIMPL			KLAV_ENOIMPL

// user buffer too small (errBUFFER_TOO_SMALL)
#define KLAV_ESMALLBUF			KLAV_MAKE_STANDARD_ERROR(0x044)

// access denied (errACCESS_DENIED)
#define KLAV_EACCESS			KLAV_MAKE_STANDARD_ERROR(0x045)

// invalid parameter value (errPARAMETER_INVALID)
#define KLAV_EINVAL				KLAV_MAKE_STANDARD_ERROR(0x046)

// generic unknown error (errNOT_OK)
#define KLAV_EUNKNOWN			KLAV_MAKE_STANDARD_ERROR(0x04B)

// not found (errNOT_FOUND)
#define KLAV_ENOTFOUND			KLAV_MAKE_STANDARD_ERROR(0x04C)

// duplicate object (errFOUND)
#define KLAV_EDUPLICATE			KLAV_MAKE_STANDARD_ERROR(0x04D)

// already done (errALREADY)
#define KLAV_EALREADY           KLAV_MAKE_STANDARD_ERROR(0x067)

// operation canceled by user (errOPERATION_CANCELED)
#define KLAV_ECANCEL			KLAV_MAKE_STANDARD_ERROR(0x051)

// not enough space on disk (errOUT_OF_SPACE)
#define KLAV_ENOSPACE			KLAV_MAKE_STANDARD_ERROR(0x062)

// object not initialized propertly (errOBJECT_NOT_INITIALIZED)
#define KLAV_ENOINIT			KLAV_MAKE_STANDARD_ERROR(0x0C2)

// generic read error (errOBJECT_READ)
#define KLAV_EREAD	 		    KLAV_MAKE_STANDARD_ERROR(0x0CA)

// generic write error (errOBJECT_WRITE)
#define KLAV_EWRITE 		    KLAV_MAKE_STANDARD_ERROR(0x0CB)

// data corrupted or otherwise inavailable (errOBJECT_DATA_CORRUPTED)
#define KLAV_ECORRUPT		    KLAV_MAKE_STANDARD_ERROR(0x0D8)

// timeout (errTIMEOUT)
#define KLAV_ETIMEOUT			KLAV_MAKE_STANDARD_ERROR(0x221)

////////////////////////////////////////////////////////////////
// Custom errors (without Prague equivalents)

// cannot create synchronization object
#define KLAV_ENOSYNC			KLAV_MAKE_CUSTOM_ERROR(0x01)

// invalid data format
#define KLAV_EFORMAT			KLAV_MAKE_CUSTOM_ERROR(0x02)

// conflicting parameter value
#define KLAV_ECONFLICT			KLAV_MAKE_CUSTOM_ERROR(0x03)

// network connection was refused
#define KLAV_ENOTCONN           KLAV_MAKE_CUSTOM_ERROR(0x04)

// security check failed
#define KLAV_ESECUR             KLAV_MAKE_CUSTOM_ERROR(0x05)

////////////////////////////////////////////////////////////////
// Utilites

KLAV_ERR klav_translate_errno (int e);

KLAV_ERR klav_return_err (KLAV_ERR err);

#ifdef KLAV_DEBUG_RETURN_ERR
# define KLAV_RETURN_ERR(ERR) return klav_return_err (ERR)
#else
# define KLAV_RETURN_ERR(ERR) return ERR
#endif // KLAV_DEBUG_RETURN_ERR

#define KLAV_CHECK_RETURN(EXPR) do { \
		KLAV_ERR err = EXPR; \
		if (KLAV_FAILED (err)) \
			KLAV_RETURN_ERR (err) \
	} while (0)

void klav_debug_break ();

#ifdef _DEBUG
# if defined(_MSC_VER)
#  if _MSC_VER >= 1300 && !defined(_CRT_PORTABLE)
#   define klav_debug_break() __debugbreak ()
#  elif defined (_M_IX86) && !defined(_CRT_PORTABLE)
#   define klav_debug_break() __asm { int 3 }
#  elif defined(_M_IA64) && !defined(_CRT_PORTABLE)
    void __break(int);
#   pragma intrinsic (__break)
#   define klav_debug_break __break(0x80016)
#  endif
# endif
#else // _DEBUG
# define klav_debug_break()
#endif // _DEBUG

#endif // klaverr_h_INCLUDED

