#ifndef __vassist__h
#define __vassist__h

#define VISUAL_ASSIST_STRING_DELIMITER 0xA7

#include "windows.h"

#ifdef __cplusplus
extern "C" {
#endif

DWORD GetVisualAssistHash(CHAR* pStr);
BOOL GetVisualAssistCachePath(LPBYTE lpVACachePath, LPDWORD lpcbPath);
INT GetVisualAssistCacheFile(char* pCacheFileName, char* pSourceFileName);
INT GetTypeFromVisualAssistDefinition(CHAR* pTypeStr, DWORD dwMaxLen, CHAR* pDefinition);
INT GetDefinitionFromVisualAssistCacheFile(char* pDefinition, DWORD dwMaxLen, char* pSymbolName, char* pSourceFileName);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __vassist__h
