#include <memory.h>
#include "base64p.h"

static unsigned char aEncodeArray[64];
static unsigned char aDecodeArray[256];

void InitBase64P()
{
	size_t nIndex;
	static int bInited = 0;
	
	if (bInited)
		return;
    
	memset(aDecodeArray, 0xFF, sizeof(aDecodeArray)) ;
    
    for (nIndex = 0; nIndex < 64; ++nIndex)
    {
        int nValue ;
        if (nIndex < 26)
            nValue = nIndex + 'A' ;
        else if (nIndex < 52)
            nValue = (nIndex - 26) + 'a' ;
        else if (nIndex < 62)
            nValue = (nIndex - 52) + '0' ;
        else if (nIndex == 62)
            nValue = '+' ;
        else
            nValue = '/' ;
        
        aEncodeArray[nIndex] = (unsigned char)nValue ;
        aDecodeArray[nValue] = nIndex ;
    }

	// customized alphabet for KL servers ('+' & '/' prohibited in URLs)
	aEncodeArray[62] = '-' ;
	aEncodeArray[63] = '_' ;

    bInited = 1;
}

bool DecodeBase64PackedLong(unsigned long* pnValue, unsigned char* pBuff, size_t nBuffSize, size_t* pnBytesRead)
{
    size_t read = 0;
    unsigned long val = 0;
    unsigned long shift = 0;
    if (!pnValue)
        return false;
    if (pnValue)
        *pnValue = 0;
    if (pnBytesRead)
        *pnBytesRead = 0;
    while (read < nBuffSize)
    {
        unsigned char b = aDecodeArray[pBuff[read]];
        if (0xFF == b)
            return false; // invalid base64 sequence
        read++;
        val |= (b & 0x1F) << shift;
        shift += 5;
        if (0 == (b & 0x20))
        {
            // decoded ok
            if (pnBytesRead)
                *pnBytesRead = read;
            *pnValue = val;
            return true;
        }
    }
    // buffer too small
    return false;
}

bool EncodeBase64PackedLong(unsigned long nValue, unsigned char* pBuff, size_t nBuffSize, size_t* pnBytesWritten)
{
    size_t written = 0;
    unsigned long val = 0;
    unsigned char b;
    
    if (pnBytesWritten)
        *pnBytesWritten = 0;
    
    while (written < nBuffSize)
    {
        b = (unsigned char)(nValue & 0x1F);
        nValue >>= 5;
        if (nValue)
            b |= 0x20;
        pBuff[written] = aEncodeArray[b];
        written++;
        if (!nValue)
        {
            // encoded ok
            if (pnBytesWritten)
                *pnBytesWritten = written;
            return true;
        }
    };  
    // buffer too small
    return false;   
}

bool DecodeBase64(const unsigned char* pBase64, size_t nBase64Size, unsigned char* pBuff, size_t nBuffSize, size_t* pnBytes)
{
    size_t read = 0, written = 0;
    unsigned long val = 0;
    unsigned long bits = 0;
	unsigned char b;

	if (!pBase64 || !pBuff)
		return false;
	if (pnBytes)
		*pnBytes = 0;
    
	while (read < nBase64Size)
    {
        b = pBuff[read];
		if (b == '=' || b=='\r' || b=='\n')
		{
			// ignore them
			read++;
			continue;
		}
		b = aDecodeArray[b];
        if (0xFF == b)
            return false; // invalid base64 sequence
        val |= (b & 0x3F) << bits;
        bits += 6;
        if (bits >= 8)
        {
            // byte decoded
			if (nBuffSize == written)
				break; // buffer too small
			pBuff[written++] = (unsigned char)val;
			val >>= 8;
			bits -= 8;
        }
        read++;
    }

	if (bits) // partially decoded byte
	{
		if (nBuffSize == written)
			; // buffer too small
		else
			pBuff[written++] = (unsigned char)val;
	}
	if (pnBytes)
		*pnBytes = written;
    return (nBase64Size == read);
}

bool EncodeBase64(const unsigned char* pBuff, size_t nBuffSize, unsigned char* pBase64, size_t nBase64Size, size_t* pnBytes, bool bUsePadding)
{
    size_t read = 0, written = 0;
    unsigned long val = 0;
    unsigned long bits = 0;
    
	if (!pBase64 || !pBuff)
		return false;
	if (pnBytes)
		*pnBytes = 0;
    
    while (read < nBuffSize)
    {
		val |= pBuff[read] << bits;
		bits += 8;
        while (bits >= 6)
        {
            // byte ready to encode
			if (nBase64Size == written)
				break; // buffer too small
			pBase64[written++] = aEncodeArray[val & 0x3F];
			val >>= 6;
			bits -= 6;
        }
		read++;
    };  
	if (bits) // partially encoded byte
	{
		if (nBase64Size == written)
			; // buffer too small
		else
		{
			pBase64[written++] = aEncodeArray[val & 0x3F];
			if (bUsePadding)
			{
				while ((written % 4) != 0)
				{
					if (nBase64Size == written)
						break;
					pBase64[written++] = '=';
				}
			}
		}
	}
	if (pnBytes)
		*pnBytes = written;
    return (nBuffSize == read);
}
