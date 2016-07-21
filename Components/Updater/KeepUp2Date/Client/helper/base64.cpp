#include <stdlib.h>     // div_t
#include <string.h>     // strlen

#include "base64.h"

const static unsigned int MAX_LINE_LENGTH = 76;

const static char BASE64_ALPHABET [64] = 
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', //   0 -   9
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', //  10 -  19
    'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', //  20 -  29
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', //  30 -  39
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', //  40 -  49
    'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', //  50 -  59
    '8', '9', '+', '/'                                //  60 -  63
};

const static char BASE64_DEALPHABET [128] = 
{
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //   0 -   9
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //  10 -  19
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //  20 -  29
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //  30 -  39
     0,  0,  0, 62,  0,  0,  0, 63, 52, 53, //  40 -  49
    54, 55, 56, 57, 58, 59, 60, 61,  0,  0, //  50 -  59
     0, 61,  0,  0,  0,  0,  1,  2,  3,  4, //  60 -  69
     5,  6,  7,  8,  9, 10, 11, 12, 13, 14, //  70 -  79
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, //  80 -  89
    25,  0,  0,  0,  0,  0,  0, 26, 27, 28, //  90 -  99
    29, 30, 31, 32, 33, 34, 35, 36, 37, 38, // 100 - 109
    39, 40, 41, 42, 43, 44, 45, 46, 47, 48, // 110 - 119
    49, 50, 51,  0,  0,  0,  0,  0          // 120 - 127
};


unsigned int CBase64::calculateRecquiredEncodeOutputBufferSize(const size_t inputByteCount)
{
    div_t result = div(inputByteCount, 3);

    unsigned int RecquiredBytes = 0;
    if(result.rem == 0)
    {
        // Number of encoded characters
        RecquiredBytes = result.quot * 4;

        // CRLF -> "\r\n" each 76 characters
        result = div (RecquiredBytes, 76);
        RecquiredBytes += result.quot * 2;

        // Terminating null for the Encoded String
        RecquiredBytes += 1;

        return RecquiredBytes;
    }
    else
    {
        // Number of encoded characters
        RecquiredBytes = result.quot * 4 + 4;

        // CRLF -> "\r\n" each 76 characters
        result = div (RecquiredBytes, 76);
        RecquiredBytes += result.quot * 2;

        // Terminating null for the Encoded String
        RecquiredBytes += 1;

        return RecquiredBytes;
    }
}

unsigned int CBase64::calculateRecquiredDecodeOutputBufferSize(const char *inputBufferString)
{
    unsigned int BufferLength = strlen(inputBufferString);

    div_t result = div (BufferLength, 4);

    if(inputBufferString[BufferLength - 1] != '=')
        return result.quot * 3;
    else
    {
        if(inputBufferString [BufferLength - 2] == '=')
            return result.quot * 3 - 2;
        return result.quot * 3 - 1;
    }
}

void CBase64::encodeByteTriple(const unsigned char *inputBuffer, const unsigned int inputCharacters, char *outputBuffer)
{
    const unsigned int mask = 0xfc000000;
    unsigned int buffer = 0;


    char *temp = (char *)&buffer;
    temp[3] = inputBuffer[0];
    if(inputCharacters > 1)
        temp[2] = inputBuffer[1];
    if(inputCharacters > 2)
        temp[1] = inputBuffer[2];

    switch(inputCharacters)
    {
        case 3:
            outputBuffer[0] = BASE64_ALPHABET[(buffer & mask) >> 26];
            buffer = buffer << 6;
            outputBuffer[1] = BASE64_ALPHABET[(buffer & mask) >> 26];
            buffer = buffer << 6;
            outputBuffer[2] = BASE64_ALPHABET[(buffer & mask) >> 26];
            buffer = buffer << 6;
            outputBuffer[3] = BASE64_ALPHABET[(buffer & mask) >> 26];
            break;
        case 2:
            outputBuffer[0] = BASE64_ALPHABET[(buffer & mask) >> 26];
            buffer = buffer << 6;
            outputBuffer[1] = BASE64_ALPHABET[(buffer & mask) >> 26];
            buffer = buffer << 6;
            outputBuffer[2] = BASE64_ALPHABET[(buffer & mask) >> 26];
            outputBuffer[3] = '=';
            break;
        case 1:
            outputBuffer[0] = BASE64_ALPHABET[(buffer & mask) >> 26];
            buffer = buffer << 6;
            outputBuffer[1] = BASE64_ALPHABET[(buffer & mask) >> 26];
            outputBuffer[2] = '=';
            outputBuffer[3] = '=';
            break;
    }
}

unsigned int CBase64::decodeByteQuartet(const char *inputBuffer, unsigned char *outputBuffer)
{
    unsigned int buffer = 0;

    if(inputBuffer[3] == '=')
    {
        if(inputBuffer[2] == '=')
        {
            buffer = (buffer | BASE64_DEALPHABET[static_cast<const size_t>(inputBuffer[0])]) << 6;
            buffer = (buffer | BASE64_DEALPHABET[static_cast<const size_t>(inputBuffer[1])]) << 6;
            buffer = buffer << 14;

            unsigned char *temp = reinterpret_cast<unsigned char *>(&buffer);
            outputBuffer[0] = temp[3];
            return 1;
        }

		buffer = (buffer | BASE64_DEALPHABET[static_cast<const size_t>(inputBuffer[0])]) << 6;
        buffer = (buffer | BASE64_DEALPHABET[static_cast<const size_t>(inputBuffer[1])]) << 6;
        buffer = (buffer | BASE64_DEALPHABET[static_cast<const size_t>(inputBuffer[2])]) << 6;
        buffer = buffer << 8;

        unsigned char *temp = reinterpret_cast<unsigned char *>(&buffer);
        outputBuffer[0] = temp[3];
        outputBuffer[1] = temp[2];
        
        return 2;
	}

    buffer = (buffer | BASE64_DEALPHABET[static_cast<const size_t>(inputBuffer[0])]) << 6;
    buffer = (buffer | BASE64_DEALPHABET[static_cast<const size_t>(inputBuffer[1])]) << 6;
    buffer = (buffer | BASE64_DEALPHABET[static_cast<const size_t>(inputBuffer[2])]) << 6;
    buffer = (buffer | BASE64_DEALPHABET[static_cast<const size_t>(inputBuffer[3])]) << 6; 
    buffer = buffer << 2;

    unsigned char *temp = reinterpret_cast<unsigned char *>(&buffer);
    outputBuffer[0] = temp[3];
    outputBuffer[1] = temp[2];
    outputBuffer[2] = temp[1];

    return 3;
}

std::vector<char> CBase64::encodeBuffer(const unsigned char *inputBuffer, const size_t inputBufferLength)
{
    std::vector<char> result;
    result.resize(calculateRecquiredEncodeOutputBufferSize(inputBufferLength));
    if(!result.size())
        return result;
    memset(&result[0], 0, result.size());

    unsigned int outputBufferIndex = 0;
    for(unsigned int inputBufferIndex = 0; inputBufferIndex < inputBufferLength; inputBufferIndex  += 3)
    {
        if(inputBufferLength - inputBufferIndex <= 2)
        {
            encodeByteTriple(inputBuffer + inputBufferIndex, inputBufferLength - inputBufferIndex, &result[outputBufferIndex]);
            break;
        }
        encodeByteTriple(inputBuffer + inputBufferIndex, 3, &result[outputBufferIndex]);
        outputBufferIndex += 4;
    }
    return result;
}

std::vector<unsigned char> CBase64::decodeBuffer(const char *inputBufferString)
{
    std::vector<unsigned char> result;
    unsigned int inputBufferLength = strlen(inputBufferString);

    char byteQuartet[4];

    for(unsigned int inputBufferIndex  = 0; inputBufferIndex < inputBufferLength;)
    {
        for(int i = 0; i < 4; ++i)
        {
            byteQuartet[i] = inputBufferString[inputBufferIndex];

            // Ignore all characters except the ones in BASE64_ALPHABET
            if((byteQuartet[i] >= 48 && byteQuartet[i] <=  57)
                || (byteQuartet[i] >= 65 && byteQuartet[i] <=  90)
                || (byteQuartet[i] >= 97 && byteQuartet[i] <= 122)
                || byteQuartet[i] == '+' || byteQuartet[i] == '/' || byteQuartet [i] == '=')
            {
            }
            // invalid character
            else
                --i;

            ++inputBufferIndex;
        }

        unsigned char decoded[4];
        const size_t decodedSize = decodeByteQuartet(byteQuartet, decoded);
        if(decodedSize)
        {
            const size_t currentResultSize = result.size();
            result.resize(currentResultSize + decodedSize);
            if(!result.empty())
                memcpy(&result[0] + currentResultSize, decoded, decodedSize);
        }
    }

    // the next position of the next decoded character inside our output buffer
    //  and thus represents the number of decoded characters in our buffer.
    return result;
}

// =============================================================================

Base64P::Base64P()
{
    memset(m_decodeArray, 0xFF, sizeof(m_decodeArray));

    for(size_t index = 0; index < 64; ++index)
    {
        unsigned char value = '/';
        if(index < 26)
            value = index + 'A';
        else if(index < 52)
            value = (index - 26) + 'a';
        else if(index < 62)
            value = (index - 52) + '0';
        else if(index == 62)
            value = '+';

        m_encodeArray[index] = value;
        m_decodeArray[value] = index;
    }

    // customized alphabet for KL servers ('+' & '/' prohibited in URLs)
    m_encodeArray[62] = '-';
    m_encodeArray[63] = '_';
}

bool Base64P::decodeBase64PackedLongWithUpdaterAlphabet(unsigned long *value, const unsigned char *pBuff, const size_t bufferSize, size_t *bytesRead)
{
    size_t read = 0;
    unsigned long val = 0;
    unsigned long shift = 0;
    if(!value)
        return false;
    if(value)
        *value = 0;
    if(bytesRead)
        *bytesRead = 0;
    while(read < bufferSize)
    {
        char charIndex = pBuff[read];
        // customized alphabet for KL servers ('+' & '/' prohibited in URLs)
        if(charIndex == '-')
            charIndex = '+';
        if(charIndex == '_')
            charIndex = '/';

        const unsigned char b = m_decodeArray[static_cast<size_t>(charIndex)];
        if(0xFF == b)
            return false; // invalid base64 sequence

        ++read;
        val |= (b & 0x1F) << shift;
        shift += 5;
        if(0 == (b & 0x20))
        {
            // decoded ok
            if(bytesRead)
                *bytesRead = read;
            *value = val;
            return true;
        }
    }
    // buffer too small
    return false;
}

bool Base64P::encodeBase64PackedLongWithUpdaterAlphabet(unsigned long value, unsigned char* pBuff, const size_t bufferSize, size_t* pnBytesWritten)
{
    size_t written = 0;
    if(pnBytesWritten)
        *pnBytesWritten = 0;
    
    while(written < bufferSize)
    {
        unsigned char b = (unsigned char)(value & 0x1F);
        value >>= 5;
        if(value)
            b |= 0x20;
        pBuff[written] = m_encodeArray[b];
        ++written;
        if(!value)
        {
            // encoded ok
            if(pnBytesWritten)
                *pnBytesWritten = written;
            return true;
        }
    }
    // buffer too small
    return false;   
}

bool Base64P::decodeBase64(const unsigned char* pBase64, size_t nBase64Size, unsigned char* pBuff, const size_t bufferSize, size_t* pnBytes)
{
	if(!pBase64 || !pBuff)
		return false;
	if(pnBytes)
		*pnBytes = 0;

    size_t read = 0;
    size_t written = 0;
    unsigned long val = 0;
    unsigned long bits = 0;
	unsigned char b;

	while(read < nBase64Size)
    {
        b = pBuff[read];
		if(b == '=' || b=='\r' || b=='\n')
		{
			// ignore them
			++read;
			continue;
		}
		b = m_decodeArray[b];
        if(0xFF == b)
            return false; // invalid base64 sequence
        val |= (b & 0x3F) << bits;
        bits += 6;
        if(bits >= 8)
        {
            // byte decoded
			if(bufferSize == written)
				break; // buffer too small
			pBuff[written++] = (unsigned char)val;
			val >>= 8;
			bits -= 8;
        }
        ++read;
    }

	if(bits) // partially decoded byte
	{
		if(bufferSize == written)
			; // buffer too small
		else
			pBuff[written++] = (unsigned char)val;
	}
	if(pnBytes)
		*pnBytes = written;
    return (nBase64Size == read);
}

bool Base64P::encodeBase64(const unsigned char* pBuff, const size_t bufferSize, unsigned char* pBase64, size_t nBase64Size, size_t* pnBytes, bool bUsePadding)
{
    size_t read = 0, written = 0;
    unsigned long val = 0;
    unsigned long bits = 0;
    
	if(!pBase64 || !pBuff)
		return false;
	if(pnBytes)
		*pnBytes = 0;
    
    while(read < bufferSize)
    {
		val |= pBuff[read] << bits;
		bits += 8;
        while(bits >= 6)
        {
            // byte ready to encode
			if(nBase64Size == written)
				break; // buffer too small
			pBase64[written++] = m_encodeArray[val & 0x3F];
			val >>= 6;
			bits -= 6;
        }
		++read;
    }

	if(bits) // partially encoded byte
	{
		if(nBase64Size == written)
			; // buffer too small
		else
		{
			pBase64[written++] = m_encodeArray[val & 0x3F];
			if(bUsePadding)
			{
				while((written % 4) != 0)
				{
					if(nBase64Size == written)
						break;
					pBase64[written++] = '=';
				}
			}
		}
	}
	if(pnBytes)
		*pnBytes = written;
    return(bufferSize == read);
}
