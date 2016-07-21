#include <wtypes.h>
#include "debug.h"
#include "decode.h"

static BYTE rgChoice[] = "\x0\x1\x2\x0\x1\x2\x1\x2\x2\x1\x2\x1\x0\x2\x1\x2\x0\x2\x1\x2\x0\x0\x1\x2\x2\x1\x0\x2\x1\x2\x2\x1\x0\x0\x2\x1\x2\x1\x2\x0\x2\x0\x0\x1\x2\x0\x2\x1\x0\x2\x1\x2\x0\x0\x1\x2\x2\x0\x0\x1\x2\x0\x2\x1";
/*
0120121221210212021200122102122100212120200120210212001220012021
*/

static BYTE DecodeTable[3][98] = {
	   ".GzVBj/&IA42[vrC89pEhqO\tbD#u<~>^@wJa]\"KoN;LPg*}tT+-,0nkf5%!dMRc?{x)(sY3mUS|:_eFX1ilZH'\\=$y7`Q 6W",
	   "-uR`q^I\\b})6 |zkc3+hQfv1dTC<:>~@E,*t'7DyY/o&rj9{?8wgSG4x]0#Z[lHUpi.L!$NP\tVs5aKX;W\"mM%(FJ2A=_OBen",
	   "20!)[83=X:5e9\\VsfNEkbYx^}Jmq<`>S@B'Hru17MR\"TjGd- .L]~loytC&v%$+(#A4\t*D?w;UiacPgQIOFh|6pnz/_KZ,W{" 
};
/*
	   [.GzVBj/&IA42[vrC89pEhqO bD#u<~>^@wJa]"KoN;LPg*}tT+-,0nkf5%!dMRc?{x)(sY3mUS|:_eFX1ilZH'\=$y7`Q 6W]
	   [-uR`q^I\b})6 |zkc3+hQfv1dTC<:>~@E,*t'7DyY/o&rj9{?8wgSG4x]0#Z[lHUpi.L!$NP Vs5aKX;W"mM%(FJ2A=_OBen]
	   [20!)[83=X:5e9\VsfNEkbYx^}Jmq<`>S@B'Hru17MR"TjGd- .L]~loytC&v%$+(#A4 *D?w;UiacPgQIOFh|6pnz/_KZ,W{]
*/


static __inline BYTE revert(BYTE c, WORD size)
{
	BYTE c1=0;
	WORD i;
	for (i=0; i<size; i++)
	{
		c1 <<= 1;
		if ((c&1) == 1)
			c1++;
		c >>= 1; 
	}
	return c1;
}	

static HRESULT ReadEncodedNumber(WCHAR* pStream, LPDWORD dwNum)
{
	BYTE b;
	DWORD dw;
	int i;
	char EncodedNum[6];
	
	dw = 0;
	
	for (i=0; i<6; i++)
	{
		EncodedNum[5-i] = (char)pStream[i];
	}
	for (i=0; i<6; i++)
	{
		b = EncodedNum[i];
		if (b == '+')
			b = 63;
		else if (b == '/')
			b = 62;
		else if (b >= '0' && b <= '9')
			b += 4;
		else if (b >= 'a' && b <= 'z')
			b -= 71;
		else
			b -= 65;
		b=revert(b,6);
#ifdef _WIN32
		dw <<= 6;
#else
		for (int j=0; j<6; j++)
			dw <<= 1;
#endif
		dw += b;
	}
	*dwNum=dw;
	for (i=0; i<4; i++)
	{
		((BYTE*)(dwNum))[i] = revert(((BYTE*)(dwNum))[i], 8);
	}
	if (pStream[6] == '=' && pStream[7] == '=') // '=='
		return S_OK;
	return E_FAIL;
}

void DecodeScript(WCHAR* pwcsScriptText, WCHAR* pwscBuffer, DWORD dwBufferSize)
{
	DWORD rgChoicePtr = 0;
	DWORD CheckSum = 0;
	DWORD dwEncodedSize = 0;
	WCHAR b;

	WCHAR* pSrc = pwcsScriptText;

	while (*pSrc) 
	{
		b = *pSrc;
		pSrc++;

		if (dwEncodedSize != 0)
		{
			if (b == 0x40) 
			{
				b = *pSrc;
				pSrc++;
				dwEncodedSize--;
				switch (b)
				{
				case 0x21:
					b = 0x3c; // '<'
					break;
				case 0x23:
					b = 0x0d;
					break;
				case 0x24:
					b = 0x40;
					break;
				case 0x26:
					b = 0x0a;
					break;
				case 0x2a:    // '>'
					b = 0x3e;
					break;
				}
			}
			else
			{
				if ((b >= 0x20 && b < 0x80) || (b == 9))
				{
					if (b == 9) 
						b = 0x80;
					b = DecodeTable[rgChoice[rgChoicePtr]][b-32];
				}
			}

			if (b<0x80)
			{
				CheckSum += b;
				if (++rgChoicePtr == 64) 
					rgChoicePtr = 0;
			}
			
			if (dwEncodedSize != -1)
				dwEncodedSize--;
			
			if (dwEncodedSize == -1)
			{
				if (pSrc[0] == '^' && pSrc[1] == '#' && pSrc[2] == '~' && pSrc[3] == '@') // '^#~@'
				{
					ODS(("Found end of encoded block (size was unknown)"));
					dwEncodedSize = 0;
				}
			}

			if (dwEncodedSize == 0) 
			{	// End of encoded script
				if (pSrc[0] != '^' || pSrc[1] != '#' || pSrc[2] != '~' || pSrc[3] != '@') // '^#~@'
				{
					DWORD _CheckSum = 0;
					if (ReadEncodedNumber(pSrc, &_CheckSum) == S_OK)
					{
						pSrc+=8;
						if (CheckSum == _CheckSum)
						{
							ODS(("Encrypted script CheckSum OK."));
						}
						else
						{
							ODS(("Encrypted script CheckSum error: %08X, must be %08X.", CheckSum, _CheckSum));
						}
					}
				}
				else
				{
					ODS(("Encrypted script has no checksum."));
				}

				if (pSrc[0] != '^' || pSrc[1] != '#' || pSrc[2] != '~' || pSrc[3] != '@') // '^#~@'
				{
					ODS(("Error: encrypted script end tag not found."));
				}
				else
				{
					pSrc+=4;
				}
			}
		}
		else if (b == '#')
		{
			if (pSrc[0] == '@' && pSrc[1] == '~' && pSrc[2] == '^') // '#@~^'
			{
				ODS(("Encrypted script start tag found."));
				rgChoicePtr = 0;
				CheckSum = 0;
				if (ReadEncodedNumber(pSrc+3, &dwEncodedSize) == S_OK)
				{
					ODS(("Script encoding found Len=%Xh(%d)", dwEncodedSize, dwEncodedSize));
					pSrc+=3+8;
					continue;
				}
				else
				{
					ODS(("Script encoding found, but len is unknown"));
					dwEncodedSize = (DWORD)-1;
					pSrc+=3+8;
					continue;
				}
			}
		}

		if (dwBufferSize >= sizeof(WCHAR))
		{
			*pwscBuffer = b;
			pwscBuffer++;
			dwBufferSize-=sizeof(WCHAR);
		}
	}
	
	// zero terminating
	if (dwBufferSize > sizeof(WCHAR))
	{
		*pwscBuffer = 0;
		pwscBuffer++;
	}
	
}
