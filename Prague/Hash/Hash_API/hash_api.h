#ifndef __HASH_API_H__
#define __HASH_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined (_WIN32)
#define HASH_API_CALL __stdcall
#else
#define HASH_API_CALL __attribute__((stdcall))
#endif
typedef struct tag_HASH_API {
	unsigned int (HASH_API_CALL *Init)(unsigned long alg_id, void* pCtx, unsigned long CtxSize, unsigned long dwHashSize, void* pInitValue);
	unsigned int (HASH_API_CALL *Update)(void* pCtx, void* pBuffer, unsigned long BufferSize);
	unsigned int (HASH_API_CALL *GetCurrentHash)(void* pCtx, unsigned long* pHashSize, void* pResult);
	unsigned int (HASH_API_CALL *Finalize)(void* pCtx, unsigned long* pHashSize, void* pResult);
	unsigned long ContextSize;
} HASH_API, *PHASH_API;

#ifdef __cplusplus
} // extern "C"
#endif
	
#endif
