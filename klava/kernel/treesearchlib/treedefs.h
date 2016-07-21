// treedefs.h
//

#ifndef treedefs_h_INCLUDED
#define treedefs_h_INCLUDED

#include <kl_types.h>

#if !defined(_INLINE)
#if defined(_WIN32)
#define _INLINE __inline
#define _FORCEINLINE __forceinline
#elif defined (__unix__)
#define __cdecl  __attribute__((cdecl))
#define __fastcall
#define _INLINE 
#define _FORCEINLINE
#else
#error "Unknown platform"
#endif
#endif

#if defined (_M_IX86) || defined (__i386__)
#define _TREE_CALL __cdecl
#else
#define _TREE_CALL
#endif

#if !defined(EXTERN_C) 
#if !defined(__cplusplus)
#define EXTERN_C
#else
#define EXTERN_C extern "C"
#endif
#endif

#ifndef IN
#define IN 
#endif
#ifndef OUT
#define OUT
#endif
#ifndef CONST
#define CONST const
#endif

typedef unsigned int bool_t;

#ifndef countof
  #define countof(array) (sizeof(array)/sizeof((array)[0]))
#endif


#define SIGNATURE_ESC_CHAR             ((uint8_t)(0x9B)) // most rare byte in virus signatures (933:1102296)

typedef uint32_t tSIGNATURE_ID;

// error code type, error codes
typedef uint32_t TREE_ERR;

#define TREE_MAKE_STANDARD_ERROR(CODE)  ((0x80000000U) | ((uint32_t)(CODE) & 0xFFFU))

#define TREE_SUCCEEDED(err)		(((TREE_ERR)(err) & 0x80000000) == 0)
#define TREE_FAILED(err)		(((TREE_ERR)(err) & 0x80000000) != 0)

// standard error codes
#define TREE_OK 0

// unexpected condition (errUNEXPECTED)
#define TREE_EUNEXPECTED		TREE_MAKE_STANDARD_ERROR(0x040)
// cannot allocate memory (errNOT_ENOUGH_MEMORY)
#define TREE_ENOMEM				TREE_MAKE_STANDARD_ERROR(0x041)
// not implemented (errNOT_IMPLEMENTED)
#define TREE_ENOIMPL			TREE_MAKE_STANDARD_ERROR(0x043)
// user buffer too small (errBUFFER_TOO_SMALL)
#define TREE_ESMALLBUF			TREE_MAKE_STANDARD_ERROR(0x044)
// invalid parameter value (errPARAMETER_INVALID)
#define TREE_EINVAL				TREE_MAKE_STANDARD_ERROR(0x046)
// generic unknown error (errNOT_OK)
#define TREE_EUNKNOWN			TREE_MAKE_STANDARD_ERROR(0x04B)
// not found (errNOT_FOUND)
#define TREE_ENOTFOUND			TREE_MAKE_STANDARD_ERROR(0x04C)
// duplicate object (errFOUND)
#define TREE_EDUPLICATE			TREE_MAKE_STANDARD_ERROR(0x04D)
// object not initialized propertly (errOBJECT_NOT_INITIALIZED)
#define TREE_ENOINIT			TREE_MAKE_STANDARD_ERROR(0x0C2)
// operation canceled by user (errOPERATION_CANCELED)
#define TREE_ECANCEL			TREE_MAKE_STANDARD_ERROR(0x051)
// data corrupted or otherwise inavailable (errOBJECT_DATA_CORRUPTED)
#define TREE_ECORRUPT		    TREE_MAKE_STANDARD_ERROR(0x0D8)

// typedefs

#ifdef _MSC_VER
typedef struct tag_NODE NODE;
typedef struct tag_sRT_CTX sRT_CTX;
typedef struct tag_sSEARCHER_RT_CTX sSEARCHER_RT_CTX;
#else
struct tag_NODE;
#define NODE struct tag_NODE
struct tag_sRT_CTX;
#define sRT_CTX struct tag_sRT_CTX
struct tag_sSEARCHER_RT_CTX;
#define sSEARCHER_RT_CTX struct tag_sSEARCHER_RT_CTX
#endif

#define cFORMAT_DYNAMIC                ((uint32_t)(1)) // Динамический формат записи.
#define cFORMAT_STATIC                 ((uint32_t)(2)) // Статический формат записи.

#define	cCT_BitMaskAnd	0x00
#define	cCT_BitMaskOr	0x01
  //#define	cCT_BitMaskXor	0x02
#define	cCT_Range	0x10
#define	cCT_Aster	0x11

#define	cCT_AnyByte     0xFF // now valid only in static tree if no other conditions in chain 
							// (TODO: always leave cCT_AnyByte at the end of chain, valid in both trees)
typedef struct tag_TREE_INFO
{
	uint32_t dwMagic;         
	tSIGNATURE_ID MinSignatureID;
	tSIGNATURE_ID MaxSignatureID;
	uint32_t dwFormat;				// cFORMAT_xxx
	uint32_t dwHashLevel;     
	uint32_t dwTotalSize;     
	uint32_t dwHashTableSize;
	uint32_t dwReserved1;     
} TREE_INFO;

typedef struct tag_sHEAP_CTX
{
	void* pHeap;
	void* (_TREE_CALL *Alloc) (void* pHeap, unsigned int dwBytes); // allocated memory must be zeroed
	void* (_TREE_CALL *Realloc) (void* pHeap, void* pMem, unsigned int dwBytes); // allocated memory must be zeroed
	void  (_TREE_CALL *Free) (void* pHeap, void* pMem);
} sHEAP_CTX;

// dwOrigin values
#define TREE_IO_CTX_SEEK_SET    0
#define TREE_IO_CTX_SEEK_CUR    1
#define TREE_IO_CTX_SEEK_END    2

typedef struct tag_sIO_CTX
{
	void* pIO;
	TREE_ERR (_TREE_CALL *Read) (void* pIO, uint32_t* pdwRead, uint8_t* pbBuffer, uint32_t dwSize);
	TREE_ERR (_TREE_CALL *Write) (void* pIO, uint32_t* pdwWritten, uint8_t* pbBuffer, uint32_t dwSize);
	TREE_ERR (_TREE_CALL *SeekRead) (void* pIO, uint32_t* pdwRead, uint64_t qwOffset, uint8_t* pbBuffer, uint32_t dwSize);
	TREE_ERR (_TREE_CALL *SeekWrite) (void* pIO, uint32_t* pdwWritten, uint64_t qwOffset, uint8_t* pbBuffer, uint32_t dwSize);
	TREE_ERR (_TREE_CALL *Seek) (void* pIO, uint64_t* pqwNewPosition, uint64_t qwPosition, uint32_t dwOrigin);
}
sIO_CTX;

#endif // treedefs_h_INCLUDED

