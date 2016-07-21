#ifndef _BASE64_H_17035C75_DE83_46ab_9836_2AEDDDE976C3
#define _BASE64_H_17035C75_DE83_46ab_9836_2AEDDDE976C3

#include "comdefs.h"

class CBase64
{
public:
    static unsigned int calculateRecquiredEncodeOutputBufferSize(const size_t byteCount);
    static unsigned int calculateRecquiredDecodeOutputBufferSize(const char *bufferString);
    
    static void encodeByteTriple(const unsigned char *inputBuffer, const unsigned int inputCharacters, char *outputBuffer);
    static unsigned int decodeByteQuartet(const char *inputBuffer, unsigned char *outputBuffer);
    
    KAVUPDCORE_API static std::vector<char> encodeBuffer(const unsigned char *inputBuffer, const size_t inputBufferLength);
    KAVUPDCORE_API static std::vector<unsigned char> decodeBuffer(const char *inputBufferString);
};

// base64P encoding
class Base64P
{
public:
    Base64P();

    // Warning: alphabet is modified, because it is not allowed to transfer some symbols in HTTP/FTP Headers
    bool decodeBase64PackedLongWithUpdaterAlphabet(unsigned long *value, const unsigned char *buffer, const size_t bufferSize, size_t *bytesRead);
    // Warning: alphabet is modified, because it is not allowed to transfer some symbols in HTTP/FTP Headers
    bool encodeBase64PackedLongWithUpdaterAlphabet(unsigned long value, unsigned char *buffer, const size_t nBuffSize, size_t *bytesWritten);

    bool decodeBase64(const unsigned char *base64, const size_t base64Size, unsigned char *buffer, const size_t bufferSize, size_t *bytes);
    bool encodeBase64(const unsigned char *buffer, const size_t bufferSize, unsigned char *base64, const size_t base64Size, size_t *bytes, bool usePadding);

private:
    unsigned char m_encodeArray[64];
    unsigned char m_decodeArray[256];
};

#endif  // #ifndef _BASE64_H_17035C75_DE83_46ab_9836_2AEDDDE976C3
