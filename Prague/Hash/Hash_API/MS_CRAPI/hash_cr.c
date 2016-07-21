#define _WIN32_WINNT 0x0400
#include <windows.h>
#include "hash_cr.h"
#include "crapi.h"

#ifdef __cplusplus
extern "C" {
#endif

BOOL HASH_API_CALL CRAPI_Init(unsigned long alg_id, void* pCtx, unsigned long CtxSize, unsigned long dwHashSize, void* pInitValue)
{
	if (CtxSize < sizeof(CRAPI_CTX))
		return FALSE;
	if (pInitValue != NULL)
		return FALSE;
	return CRAPIInit((CRAPI_CTX*)pCtx, alg_id);
}

BOOL HASH_API_CALL CRAPI_Update(void* pCtx, void* pBuffer, unsigned long BufferSize)
{
	return CRAPIUpdate((CRAPI_CTX*)pCtx, (unsigned char*)pBuffer, (unsigned int)BufferSize);
}

BOOL HASH_API_CALL CRAPI_GetCurrentHash(void* pCtx, unsigned long* pHashSize, void* pResult)
{
	if (pHashSize==NULL || *pHashSize==0)
		return FALSE;
	if (pResult==NULL)
		return FALSE;
	return CRAPIGetHash(((CRAPI_CTX*)pCtx)->hHash, pHashSize, (unsigned char*)pResult);
}

BOOL HASH_API_CALL CRAPI_Finalize(void* pCtx, unsigned long* pHashSize, void* pResult)
{
	if (pHashSize==NULL || *pHashSize==0)
		return FALSE;
	if (pResult==NULL)
		return FALSE;
	return CRAPIFinal((CRAPI_CTX*)pCtx, pHashSize, (unsigned char*)pResult);
}

const HASH_API hash_api_crapi = {
	CRAPI_Init,
	CRAPI_Update,
	CRAPI_GetCurrentHash,
	CRAPI_Finalize,
	sizeof(CRAPI_CTX)
};

#ifdef __cplusplus
} // extern "C" {
#endif
