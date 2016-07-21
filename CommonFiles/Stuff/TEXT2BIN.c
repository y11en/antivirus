/*-----------------22.12.98 13:43-------------------
 * TextToBin and BinToText converters             *
 *                                                *
 * source code                                    *
 *                                                *
 * Andrew Krukov, AVP Team                        *
 -------------------------------------------------*/

#include "../AVPPort.h"
#include "TEXT2BIN.h"

/* local NULL definition */
#ifndef NULL
#define NULL ((void*)(0))
#endif

/*
 * Overview:
 *
 * used charset:
 * (mime compatible)
 *
 * 0123456789
 * ABCDEFGHIJKLMNOPQRSTUVWXYZ
 * abcdefghijklmnopqrstuvwxyz
 * +/
 *
 */

#ifndef TEXT2BIN_STATIC
static void FillBuffer(unsigned char *buffer, unsigned int count, unsigned char value)
/*
 * FillBuffer
 *
 * internal procedure to create dynamic buffering
 */
{
    while ( count-- )
    {
        *buffer++ = value++;
    }
}
#endif


unsigned int TextToBin(const void *source, unsigned int scount, void *destination, unsigned int dcount)
/*
 * TextToBin procedure
 *
 * convert printable ASCII text to its original binary representation.
 *
 * parameters
 *      source - pointer to source buffer contaning already converted text
 *      scount - number of bytes of text to be converted form text to binary
 *      destination - pointer to destination buffer for storing binary data
 *      dcount - size of destination buffer in bytes. Buffer should be at
 *               least ((scount+3)/4)*3 bytes
 *
 * return values
 *      > 0 - number of bytes successfully written to destination buffer
 *
 *      0 - error, probably causes:
 *        - source or destination points to NULL
 *        - dcount smaller than ((scount+3)/4)*3 bytes
 */
{
    unsigned int rcode = 0;        /* return value of procedure */

    /* conversion table declaration */
    #ifdef TEXT2BIN_STATIC
    static const unsigned char T2Btable[256] =
    {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0x00 */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0x08 */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0x10 */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0x18 */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0x20 */
        0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0x3F,  /* 0x28 */
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,  /* 0x30 */
        0x08, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0x38 */
        0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,  /* 0x40 */
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,  /* 0x48 */
        0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,  /* 0x50 */
        0x21, 0x22, 0x23, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0x58 */
        0xFF, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A,  /* 0x60 */
        0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32,  /* 0x68 */
        0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A,  /* 0x70 */
        0x3B, 0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0x78 */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0x80 */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0x88 */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0x90 */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0x98 */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0xA0 */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0xA8 */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0xB0 */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0xB8 */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0xC0 */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0xC8 */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0xD0 */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0xD8 */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0xE0 */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0xE8 */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0xF0 */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  /* 0xF8 */
    };
    #else /* TEXT2BIN_STATIC */
    unsigned char T2Btable[256];
    #endif

    /* check source and destination pointers */
    if ( source != NULL && destination != NULL )
    {
        /* check size of destination buffer */
        if ( dcount >= T2B_LENGTH(scount) )
        {
            unsigned int value = 0;     /* temporary buffer for storing bit stream */
            int bits = 0;               /* bit count in temporary buffer */
            int sym;                    /* temporary byte */

            #ifndef TEXT2BIN_STATIC
            /* create dynamic conversion table */
            memset(T2Btable,0xFF,sizeof(T2Btable));
            FillBuffer(T2Btable+'0',0x0A,0x00);
            FillBuffer(T2Btable+'A',0x1A,0x0A);
            FillBuffer(T2Btable+'a',0x1A,0x24);
            T2Btable['+'] = 0x3E;
            T2Btable['/'] = 0x3F;
            #endif

            while ( scount-- )
            {
                /* get character from source stream */
                sym = T2Btable[*((unsigned char*)(source))];
		source=((unsigned char*)(source))+1;

                /* check for not allowed characters */
                if ( sym == 0xFF ) break;
                /* put 6 bits into output buffer */
                sym <<= bits;
                value |= sym;
                bits += 6;

                if ( bits >= 8 )
                {
                    *((unsigned char*)destination) = (unsigned char)(value);
		    destination=((unsigned char*)destination)+1;
		    
                    value >>= 8;
                    bits -= 8;
                    rcode++;
                }
            }

        }
    }
    return rcode;
}

unsigned int BinToText(const void *source, unsigned int scount, void *destination, unsigned int dcount)
/*
 * BinToText procedure
 *
 * convert binary data to printable ASCII text
 *
 * parameters
 *      source - pointer to source buffer contaning data to be converted.
 *      scount - number of bytes to convert to text.
 *      destination - pointer to destination buffer for storing resulting text
 *      dcount - size of destination buffer in bytes. Buffer should be at
 *               least ((scount+2)/3)*4 bytes
 *
 * return values
 *      > 0 - number of bytes successfully written to destination buffer
 *
 *      0 - error, no data was written. Probably causes:
 *        - source or destination points to NULL
 *        - dcount smaller than ((scount+2)/3)*4 bytes
 */
{
    unsigned int rcode = 0;        /* return value of procedure */

    #ifdef TEXT2BIN_STATIC
    static const unsigned char B2Ttable[64] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7', /* 0x00 */
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', /* 0x08 */
        'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', /* 0x10 */
        'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', /* 0x18 */
        'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', /* 0x20 */
        'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', /* 0x28 */
        'm', 'n', 'o', 'p', 'q', 'r', 's', 't', /* 0x30 */
        'u', 'v', 'w', 'x', 'y', 'z', '+', '/', /* 0x38 */
    };
    #else  /* TEXT2BIN_STATIC */
    unsigned char B2Ttable[64];
    #endif

    /* check source and destination pointers */
    if ( source != NULL && destination != NULL && source != destination )
    {
        if ( dcount >= B2T_LENGTH(scount) )
        {
            unsigned int value = 0;         /* temporary buffer for storing bit stream */
            int bits = 0;               /* bit count in temporary buffer */
            int sym;                    /* temporary byte */

            rcode = 0;

            #ifndef TEXT2BIN_STATIC
            /* create dynamic conversion table */
            FillBuffer(B2Ttable+0x00,0x0A,'0');
            FillBuffer(B2Ttable+0x0A,0x1A,'A');
            FillBuffer(B2Ttable+0x24,0x1A,'a');
            T2Btable[0x3E'+'] = '+';
            T2Btable[0x3F'/'] = '/';
            #endif

            while ( scount || bits > 0 )
            {
                /* get 6 bits from source stream */
                if ( bits < 6 )
                {
                    /* extract next byte from input stream */
                    if ( scount )
                    {
                        sym = *((unsigned char*)(source));
			source=((unsigned char*)source)+1;
                        sym <<= bits;
                        value |= sym;
                        bits += 8;
                        scount--;
                    }
                }
                *((unsigned char*)destination) = B2Ttable[value & 0x3F];
		destination=((unsigned char*)destination)+1;
                rcode++;
                value >>= 6;
                bits -= 6;
            }
        }
    }

    return rcode;
}


#if !defined (_SIZE_T_DEFINED) && !defined (__unix__)
	typedef unsigned int size_t;
	#define _SIZE_T_DEFINED
#endif
void *  __cdecl memcpy( void*, const void*, size_t );

// ---
unsigned int BinToTextEx(const void *source, unsigned int scount, void *destination, unsigned int dcount)
/*
 * This function allows to use overlapped input/output buffers for expanding to text
 */
{
  if ( 
    (source <= destination && destination <= (void*)(((unsigned char*)source)+scount)) ||
    (destination <= source && source <= (void*)(((unsigned char*)destination)+dcount))
  ) {

    if ( scount && (dcount >= B2T_LENGTH(scount)) ) {
      int            val;
      unsigned char* src;
      unsigned char* dst;
      unsigned char* wrk;
      unsigned char  tmp[12];

      val = scount % sizeof(tmp);
      src = ((unsigned char*)source) + (scount - val);
      dst = ((unsigned char*)destination) + (scount / sizeof(tmp) * (sizeof(tmp)/3*4));

      if ( val ) {
        int aval = B2T_LENGTH(val);
        wrk = src + val;
        if ( (dst < wrk && wrk <= dst+aval) || (dst <= src && src <= dst+aval) )
          wrk = memcpy( tmp, src, val );
        else
          wrk = src;
        val = BinToText( wrk, val, dst, aval );
      }

      while ( (void*)src > source ) {
        src -= sizeof(tmp);
        dst -= sizeof(tmp) / 3 * 4;
        wrk = src + sizeof(tmp);
        if ( (dst < wrk && wrk <= dst+B2T_LENGTH(sizeof(tmp))) || (dst < src && src <= dst+B2T_LENGTH(sizeof(tmp))) )
          wrk = memcpy( tmp, src, sizeof(tmp) );
        else
          wrk = src;
        val += BinToText( wrk, sizeof(tmp), dst, sizeof(tmp)/3*4 );
      }

      return val;
    }
    else
      return 0;
  }
  else
    return BinToText( source, scount, destination, dcount );
}

