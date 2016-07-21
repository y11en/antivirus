#define SQZE_HEADER_SIGNATURE 'EZQS' // packed format
#define SQZU_HEADER_SIGNATURE 'UZQS' // unpacked format
#define SQZE_CURRENT_VERSION  1

//change DWORD to unsigned int for UNIX support
struct CSqueezeHeader
{
    unsigned int dwSignature;
    unsigned int dwVersion;
    unsigned int dwHeaderSize;
    unsigned int dwOriginalSize;
    unsigned int dwCompressedSize;
    unsigned int dwCrc;             // crc32 מf compressed פאיכא
};
