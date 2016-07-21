#include <string.h>
#include "hash_md5.h"
#include "md5.h"
#ifdef __cplusplus
extern "C" {
#endif

unsigned int HASH_API_CALL MD5_Init(unsigned long alg_id, void* pCtx, unsigned long CtxSize, unsigned long dwHashSize, void* pInitValue)
{
	if (CtxSize < sizeof(md5_state_t))
		return 0;
	if (dwHashSize != 16)
		return 0;
	if (pInitValue != 0)
		return 0;
	md5_init((md5_state_t*)pCtx);
	return 1;
}

unsigned int HASH_API_CALL MD5_Update(void* pCtx, void* pBuffer, unsigned long BufferSize)
{
	md5_append((md5_state_t*)pCtx, (const md5_byte_t*)pBuffer, (int)BufferSize);
	return 1;
}

unsigned int HASH_API_CALL MD5_GetCurrentHash(void* pCtx, unsigned long* pHashSize, void* pResult)
{
	md5_state_t sCtx;
	if (pHashSize==0 || *pHashSize<16)
		return 0;
	if (pResult==0)
		return 0;
	memcpy(&sCtx, pCtx, sizeof(md5_state_t));
	*pHashSize = 16;
	md5_finish(&sCtx, (md5_byte_t*)pResult);
	return 1;
}

unsigned int HASH_API_CALL MD5_Finalize(void* pCtx, unsigned long* pHashSize, void* pResult)
{
	if (pHashSize==0 || *pHashSize<16)
		return 0;
	if (pResult==0)
		return 0;
	*pHashSize = 16;
	md5_finish((md5_state_t*)pCtx, (md5_byte_t*)pResult);
	return 1;
}

const HASH_API hash_api_md5 = {
	MD5_Init,
	MD5_Update,
	MD5_GetCurrentHash,
	MD5_Finalize,
	sizeof(md5_state_t)
};

#ifdef __cplusplus
} // extern "C" {
#endif
