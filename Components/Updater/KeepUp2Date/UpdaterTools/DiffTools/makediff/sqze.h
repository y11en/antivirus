#include <wtypes.h>

#define SQZE_HEADER_SIGNATURE 'EZQS' // packed format
#define SQZU_HEADER_SIGNATURE 'UZQS' // unpacked format
#define SQZE_CURRENT_VERSION  1

typedef struct tag_CSqueezeHeader
{
 DWORD dwSignature;
 DWORD dwVersion;
 DWORD dwHeaderSize;
 DWORD dwOriginalSize;
 DWORD dwCompressedSize;
 DWORD dwCrc;             // crc32 מf compressed פאיכא
} CSqueezeHeader;
