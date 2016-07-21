// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  18 December 2002,  14:00 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Andy Nikishin
// File Name   -- extra.c
// AVP Prague stamp end




// AVP Prague stamp begin( Includes for interface,  )
#include "arj.h"
#include "arj_i.h"

#include <Prague/prague.h>
#include <Prague/iface/e_ktime.h> 
#include <Prague/iface/e_loader.h>
#include <Prague/iface/i_seqio.h>

#if defined(_WIN64)
	#include <string.h>
#elif defined (_WIN32)
	#if !defined(_INC_STRING)
		size_t __cdecl strlen(const tCHAR *);
		#if !defined(_INC_MEMORY)
			void* __cdecl memcpy(void*, const void*, size_t);
		#endif
	#endif
#endif

static void __cdecl xtoa (unsigned long val, char *buf, unsigned radix, int is_neg)
{
char *p;                /* pointer to traverse string */
char *firstdig;         /* pointer to first digit */
char temp;              /* temp char */
unsigned digval;        /* value of digit */

	p = buf;

	if (is_neg) 
	{
		/* negative, so output '-' and negate */
		*p++ = '-';
		val = (unsigned long)(-(long)val);
	}

	firstdig = p;           /* save pointer to first digit */

	do 
	{
		digval = (unsigned) (val % radix);
		val /= radix;       /* get next digit */

		/* convert to ascii and store */
		if (digval > 9)
			*p++ = (char) (digval - 10 + 'a');  /* a letter */
		else
			*p++ = (char) (digval + '0');       /* a digit */
	} while (val > 0);

	/* We now have the digit of the number in the buffer, but in reverse
	order.  Thus we reverse them now. */

	*p-- = '\0';            /* terminate string; p points to last digit */

	do 
	{
		temp = *p;
		*p = *firstdig;
		*firstdig = temp;   /* swap *p and *firstdig */
		--p;
		++firstdig;         /* advance to next two digits */
	} while (firstdig < p); /* repeat until halfway */
}

/* Actual functions just call conversion helper with neg flag set correctly,
   and return pointer to buffer. */

char * __cdecl itoa (int val, char *buf, int radix)
{
	if (radix == 10 && val < 0)
		xtoa((unsigned long)val, buf, radix, 1);
	else
		xtoa((unsigned long)(unsigned int)val, buf, radix, 0);
	return buf;
}

__inline tBOOL arj_isdigit(tINT c)
{
	if(c < '0' || c > '9')
		return cFALSE;
	return cTRUE;
}

tLONG __cdecl atol(const tCHAR *nptr)
{
tINT c;              /* current char */
tLONG total;         /* current total */
tINT sign;           /* if '-', then negative, otherwise positive */

	/* skip whitespace */
//	while ( isspace((int)(unsigned char)*nptr) )
//		++nptr;

	c = (int)(unsigned char)*nptr++;
	sign = c;           /* save sign indication */
	if (c == '-' || c == '+')
		c = (int)(unsigned char)*nptr++;    /* skip sign */

	total = 0;

	while (arj_isdigit(c)) 
	{
		total = 10 * total + (c - '0');     /* accumulate digit */
		c = (int)(unsigned char)*nptr++;    /* get next char */
	}

	if (sign == '-')
		return -total;
	else
		return total;   /* return result, negated if necessary */
}



tINT __cdecl atoi(const tCHAR *nptr)
{
	return (tINT)atol(nptr);
}

tERROR SkipName(hSEQ_IO hIo, tDWORD *dwLen, tCHAR* szBuffer)
{
char c=0;
tDWORD dwRet;
tINT i;
	
	dwRet = 0;
	i = 0;
	// Skip Name
	do
	{
		dwRet++;
		CALL_SeqIO_ReadByte(hIo, &c);
		if(szBuffer)
		{
			szBuffer[i] = c;
		}
		if(c==0)
		{
			break;
		}
        i++;
        if (i > MAX_FILENAME_LEN) 
        {
            return errOBJECT_DATA_CORRUPTED;
        }

	}
    while(c!=0);

	if (dwLen) 
        *dwLen = dwRet;

    return errOK;
}


tERROR SkipNameIo(hIO hArcIO, tDWORD *dwLen, tCHAR* Buffer, tDWORD dwPos)
{
tERROR error;
hSEQ_IO hSeqIo;
	
	if(PR_FAIL(error = CALL_SYS_ObjectCreateQuick(hArcIO, &hSeqIo,IID_SEQIO, PID_ANY,0)))
	{
		return error;
	}
	CALL_SeqIO_Seek(hSeqIo, NULL, dwPos, cSEEK_SET);
	error = SkipName(hSeqIo, dwLen, Buffer);
	CALL_SYS_ObjectClose(hSeqIo);
	return error;
}

tINT GetVolumeNumber(tCHAR* szArcName, tUINT iSize)
{
tUINT i;
	
	if(iSize == 0)
	{
		iSize = _toui32(strlen(szArcName));
	}
	for(i = iSize; i > 0;i-- )
	{
		if(szArcName[i] == '.')
		{
			return atoi(&szArcName[i+2]);
			
		}
	}
	return -1;
}


tINT SetArchiveName(tCHAR* szArcName, tUINT iSize, tINT iVolumeNumber)
{
tUINT i;
	
	if(iSize == 0)
	{
		iSize = _toui32(strlen(szArcName));
	}
	for(i = iSize; i > 0;i-- )
	{
		if(szArcName[i] == '.')
		{
			if(szArcName[i+1] < 'Z')
				szArcName[i+1] = 'A';
			else
				szArcName[i+1] = 'a';
			if(iVolumeNumber == 0)
			{
				if(szArcName[i+1] == 'A')
				{
					szArcName[i+2] = 'R';
					szArcName[i+3] = 'J';
				}
				else
				{
					szArcName[i+2] = 'r';
					szArcName[i+3] = 'j';
				}
				return 0;
			}
			else
			{
				if(iVolumeNumber < 10)
				{
					szArcName[i+2] = '0';
					itoa(iVolumeNumber, &szArcName[i+3], 10);
				}
				else
				{
					itoa(iVolumeNumber, &szArcName[i+2], 10);
				}
				szArcName[i+4] = 0;;
				return 0;
			}
		}
	}
	return 1;
}

tVOID CreateDateTime(tWORD* time, tWORD*date)
{
tDWORD year  ;
tDWORD month ;
tDWORD day   ;
tDWORD hour  ;
tDWORD minute;
tDWORD second;
tDT dt	;
	
	year = 1990;
	month = 1;
	day = 1;
	hour = 0;
	minute = 0;
	second = 0;
	
	Now(&dt);
	DTGet((const tDT*)&dt, &year, &month, &day, &hour, &minute, &second, NULL);
	*time = (((tWORD)hour)<<11) + (((tWORD)minute)<<5) + ((tWORD)(second/2));
	*date = (((tWORD)year-1980)<<9) + (((tWORD)month)<<5) + (tWORD)(day);
}
