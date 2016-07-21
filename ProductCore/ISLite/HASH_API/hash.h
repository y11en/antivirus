#ifndef __HASH_API_H__
#define __HASH_API_H__

#include <Prague/prague.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HASH_API_CALL __stdcall
typedef struct tag_HASH_API {
	tBOOL (HASH_API_CALL *Init)(unsigned long alg_id, void* pCtx, unsigned long CtxSize, unsigned long dwHashSize, void* pInitValue);
	tBOOL (HASH_API_CALL *Update)(void* pCtx, void* pBuffer, unsigned long BufferSize);
	tBOOL (HASH_API_CALL *GetCurrentHash)(void* pCtx, unsigned long* pHashSize, void* pResult);
	tBOOL (HASH_API_CALL *Finalize)(void* pCtx, unsigned long* pHashSize, void* pResult);
	unsigned long ContextSize;
} HASH_API, *PHASH_API;

#ifdef __cplusplus
} // extern "C"
#endif
	
#endif