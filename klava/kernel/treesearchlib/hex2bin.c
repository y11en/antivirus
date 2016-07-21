// hex2bin.c
//
//

#include "common.h"

size_t __cdecl strlen (const char *);

TREE_ERR _TREE_CALL TreeConvertHex2Signature(CONST uint8_t* pHexMask, uint32_t dwHexMaskSize, uint8_t* pSignature, uint32_t dwBufferSize, uint32_t* pdwSignatureSize)
{

//#define PUT_BIN_BYTE(b) {if (dwSignatureSize >= dwBufferSize) return errBUFFER_TOO_SMALL; pSignature[dwSignatureSize++] = b;}
#define PUT_BIN_BYTE(b) {if (dwSignatureSize < dwBufferSize) pSignature[dwSignatureSize] = b; dwSignatureSize++;}
#define RET_ERROR(msg) {ODS(msg); DBG_STOP; return TREE_EUNEXPECTED;}
	
	uint32_t dwHexPos = 0;
	uint32_t dwSignatureSize = 0;
	uint8_t c, c1, c2, _mask=0xFF, op;

	if (dwHexMaskSize == (uint32_t)-1)
		dwHexMaskSize = (uint32_t)strlen((char*)pHexMask);

	if (pHexMask==NULL || dwHexMaskSize==0 || (pSignature==NULL && dwBufferSize!=0))
		return TREE_EINVAL;
	
	while (1)
	{

		do 
		{
			if (dwHexPos >= dwHexMaskSize)
				break; // End of hex mask
			c1 = pHexMask[dwHexPos++];
		} while (c1 == ' ' || c1 == '\t');
	
		if (dwHexPos >= dwHexMaskSize)
			break; // End of hex mask
	
		_mask = 0xFF;

		if (c1 == '*')
		{
			unsigned int window = 0x100; // default window size
			if (pHexMask[dwHexPos] == '[') // parametrized '*'
			{
				window = 0;
				dwHexPos++;
				while (c1 = pHexMask[dwHexPos++])
				{
					if (c1 == ']')
						break;
					if (c1 > 0x60)
						c1 &= ~0x20; // lcase
					if (c1 >= 'A')
						c1 -= 'A';
					else if (c1 >= '0')
						c1 -= '0';
					if (c1 >= 0x10)
						RET_ERROR(("Hex mask error (condition declaration @%X)", dwHexPos-1));
					window <<= 4;
					window += c1;
				}
				if (window==0 || window>0xFFFF)
					RET_ERROR(("Hex mask error (condition declaration @%X)", dwHexPos-1));
			}
			PUT_BIN_BYTE(SIGNATURE_ESC_CHAR);
			PUT_BIN_BYTE(cCT_Aster);
			PUT_BIN_BYTE(window & 0xFF);
			PUT_BIN_BYTE((window >> 8) & 0xFF);
			continue;
		}

		if (c1 == '[')
		{
			if (dwHexPos+6 >= dwHexMaskSize)
				RET_ERROR(("Hex mask error (condition declaration @%X)", dwHexPos-1));
		
			c1 = pHexMask[dwHexPos++];
			switch (c1)
			{
			case '&':
				op = cCT_BitMaskAnd;
				break;
			case '|':
				op = cCT_BitMaskOr;
				break;
//			case '^':
//				op = cCT_BitMaskXor;
//				break;
			case 'R':
			case 'r':
				op = cCT_Range;
				break;
			default:
				RET_ERROR(("Hex mask error (condition declaration - unknown type @%X)", dwHexPos-1));
			}

			c1 = pHexMask[dwHexPos++];
			if (c1 > 0x60)
				c1 &= ~0x20; // lcase

			if (c1 < '0')
				RET_ERROR(("Hex mask error (wrong char @%X)", dwHexPos-1));

			c1 -= '0';
			if (c1 > 9)
			{
				c1 -= 7;
				if (c1 < 0x0A || c1 > 0x0F)
					RET_ERROR(("Hex mask error (wrong char @%X)", dwHexPos-1));
			}
			
			c2 = pHexMask[dwHexPos++];
			if (c2 > 0x60)
				c2 &= ~0x20; // lcase

			if (c2 < '0')
				RET_ERROR(("Hex mask error (wrong char @%X)", dwHexPos-1));
			c2 -= '0';
			if (c2 > 9)
			{
				c2 -= 7;
				if (c2 < 0x0A || c2 > 0x0F)
					RET_ERROR(("Hex mask error (wrong char @%X)", dwHexPos-1));
			}
			
			c1<<=4;
			_mask=c1+c2;

			c1 = pHexMask[dwHexPos++];
			if (c1!='=' && c1!='-')
				RET_ERROR(("Hex mask error (condition declaration - wrong delimiter @%X)", dwHexPos-1));


			c1 = pHexMask[dwHexPos++];
			if (c1 > 0x60)
				c1 &= ~0x20; // lcase
			if (c1 < '0')
				RET_ERROR(("Hex mask error (wrong char @%X)", dwHexPos-1));
			c1 -= '0';
			if (c1 > 9)
			{
				c1 -= 7;
				if (c1 < 0x0A || c1 > 0x0F)
					RET_ERROR(("Hex mask error (wrong char @%X)", dwHexPos-1));
			}
			
			c2 = pHexMask[dwHexPos++];
			if (c2 > 0x60)
				c2 &= ~0x20; // lcase

			if (c2 < '0')
				RET_ERROR(("Hex mask error (wrong char @%X)", dwHexPos-1));
			c2 -= '0';
			if (c2 > 9)
			{
				c2 -= 7;
				if (c2 < 0x0A || c2 > 0x0F)
					RET_ERROR(("Hex mask error (wrong char @%X)", dwHexPos-1));
			}
			
			c1<<=4;
			c=c1+c2;

			// verify condition
			switch (op)
			{
			case cCT_BitMaskAnd:
				if (c & (~_mask))
					RET_ERROR(("Hex mask error (invalid parameters for [&])", dwHexPos-1));
				break;
			case cCT_BitMaskOr:
				if ((c & _mask) != _mask)
					RET_ERROR(("Hex mask error (invalid parameters for [|])", dwHexPos-1));
				break;
//			case cCT_BitMaskXor:
//				{
//					tBOOL bOk = cFALSE;
//					tINT i;
//					for (i=0; i<256; i++)
//					{
//						if((i & _mask) == c)
//						{
//							bOk = cTRUE;
//							break;
//						}
//					}
//					if (!bOk)
//						RET_ERROR(("Hex mask error (invalid parameters for [|])", dwHexPos-1));
//				}
//				break;
			case cCT_Range:
				if (_mask > c)
					RET_ERROR(("Hex mask error (invalid parameters for [R], c1>c2)", dwHexPos-1));
				if (_mask == c)
					RET_ERROR(("Hex mask error (invalid parameters for [R], c1==c2)", dwHexPos-1));

			}

			PUT_BIN_BYTE(SIGNATURE_ESC_CHAR);
			PUT_BIN_BYTE(op);
			PUT_BIN_BYTE(_mask);
			PUT_BIN_BYTE(c);
			c1 = pHexMask[dwHexPos++];
			if (c1 != ']')
				RET_ERROR(("Hex mask error (condition declaration @%X)", dwHexPos-1));
			continue;
		}
		
		if (dwHexPos >= dwHexMaskSize)
			RET_ERROR(("Hex mask error (half-byte declaration)"));
		
		if (c1 == '?')
		{
			_mask = 0x0F;
			c1 = 0;
		}
		else
		{
			if (c1 > 0x60)
				c1 &= ~0x20; // lcase

			if (c1 < '0')
				RET_ERROR(("Hex mask error (wrong char @%X)", dwHexPos-1));

			c1 -= '0';
			if (c1 > 9)
			{
				c1 -= 7;
				if (c1 < 0x0A || c1 > 0x0F)
					RET_ERROR(("Hex mask error (wrong char @%X)", dwHexPos-1));
			}
		}

		c2 = pHexMask[dwHexPos++];
		if (c2 == '?')
		{
			_mask &= 0xF0;
			c2 = 0;
		}
		else
		{
			if (c2 > 0x60)
				c2 &= ~0x20; // lcase

			if (c2 < '0')
				RET_ERROR(("Hex mask error (wrong char @%X)", dwHexPos-1));
			c2 -= '0';
			if (c2 > 9)
			{
				c2 -= 7;
				if (c2 < 0x0A || c2 > 0x0F)
					RET_ERROR(("Hex mask error (wrong char @%X)", dwHexPos-1));
			}
		}

		c1<<=4;
		c=c1+c2;
		
		if (_mask != 0xFF) // Masked '??'
		{
			PUT_BIN_BYTE(SIGNATURE_ESC_CHAR);
			PUT_BIN_BYTE(cCT_BitMaskAnd);
			PUT_BIN_BYTE(_mask);
			PUT_BIN_BYTE(c);
			continue;
		}
		
		PUT_BIN_BYTE(c);
		if (c == SIGNATURE_ESC_CHAR) // ESC-char must be duplicated
			PUT_BIN_BYTE(c);
	}
	if (pdwSignatureSize != NULL)
		*pdwSignatureSize = dwSignatureSize;

	if (_mask == 0) // last byte was masked
		RET_ERROR(("Hex mask error (last byte cannot be ?)"));
	
	if (dwSignatureSize > dwBufferSize)
		return TREE_ESMALLBUF;
	return TREE_OK;
}

TREE_ERR _TREE_CALL TreeConvertRaw2Signature(CONST uint8_t* pRawData, uint32_t dwRawDataSize, uint8_t* pSignature, uint32_t dwBufferSize, uint32_t* pdwSignatureSize)
{
	uint32_t dwRawPos = 0;
	uint32_t dwSignatureSize = 0;

	if (dwRawDataSize == (uint32_t)-1)
		dwRawDataSize = (uint32_t)strlen((char*)pRawData);

	if (pRawData==NULL || dwRawDataSize==0 || (pSignature==NULL && dwBufferSize!=0))
		return TREE_EINVAL;

	while (dwRawPos < dwRawDataSize)
	{
		if (pRawData[dwRawPos] == SIGNATURE_ESC_CHAR)
			PUT_BIN_BYTE(SIGNATURE_ESC_CHAR);
		PUT_BIN_BYTE(pRawData[dwRawPos]);
		dwRawPos++;
	}

	if (pdwSignatureSize != NULL)
		*pdwSignatureSize = dwSignatureSize;
	if (dwBufferSize && dwSignatureSize > dwBufferSize)
		return TREE_ESMALLBUF;

	return TREE_OK;
}
#undef PUT_BIN_BYTE

#define PUT_BIN_BYTE(b) {if (dwRawDataSize < dwBufferSize) pRawData[dwRawDataSize] = b; dwRawDataSize++;}
TREE_ERR _TREE_CALL TreeConvertSignature2Raw(CONST uint8_t* pSignature, uint32_t dwSignatureSize, uint8_t* pRawData, uint32_t dwBufferSize, uint32_t* pdwRawDataSize)
{
	uint32_t dwSigPos = 0;
	uint32_t dwRawDataSize = 0;

	if (pSignature==NULL || dwSignatureSize==0 || (pRawData==NULL && dwBufferSize!=0))
		return TREE_EINVAL;
	
	while (dwSigPos < dwSignatureSize)
	{
		if (pSignature[dwSigPos] == SIGNATURE_ESC_CHAR)
		{
			dwSigPos++;
			if (pSignature[dwSigPos] != SIGNATURE_ESC_CHAR)
				dwSigPos+=2;
		}
		PUT_BIN_BYTE(pSignature[dwSigPos]);
		dwSigPos++;
	}

	if (pdwRawDataSize != NULL)
		*pdwRawDataSize = dwRawDataSize;
	if (dwBufferSize && dwRawDataSize > dwBufferSize)
		return TREE_ESMALLBUF;
	return TREE_OK;
}
#undef PUT_BIN_BYTE
