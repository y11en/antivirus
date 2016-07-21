#define _WIN32_WINNT 0x0400
#include <wincrypt.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	HCRYPTPROV   hCryptProv;
	HCRYPTHASH   hHash;
	HCRYPTKEY    hKey;
} CRAPI_CTX;
 
BOOL CRAPIInit(CRAPI_CTX *ctx, ALG_ID alg_id);
BOOL CRAPIUpdate(CRAPI_CTX *ctx, unsigned char *data, unsigned int datalen);
BOOL CRAPIGetHash(HCRYPTHASH hOHash, unsigned long* hashlen, unsigned char* hashdata);
BOOL CRAPIFinal(CRAPI_CTX *ctx, unsigned long* hashlen, unsigned char* hashdata);

void ByteToStr(DWORD cb, void* pv, LPSTR sz);

#ifdef __cplusplus
}
#endif
