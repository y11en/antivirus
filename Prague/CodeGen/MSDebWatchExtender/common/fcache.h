#ifndef __fcache_h
#define __fcache_h

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

typedef struct tag_CACHED_FILE {
	CHAR szFileName[MAX_PATH];
	WIN32_FILE_ATTRIBUTE_DATA sFAD;
	BYTE* pFileData;
} CACHED_FILE, *PCACHED_FILE;

BOOL OpenCachedFile(IN CHAR* szFileName, OUT CACHED_FILE* pCachedFile);
BOOL RefreshCachedFile(IN CACHED_FILE* pCachedFile);
BOOL CloseCachedFile(IN CACHED_FILE* pCachedFile);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __fcache_h