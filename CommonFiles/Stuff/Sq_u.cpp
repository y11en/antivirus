////////////////////////////////////////////////////////////////////
//
//  SQ_U.CPP
//  Unpack proc for squish
//  General purpose
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#if (defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__)))
 #define NOAFX
 #include <os2.h>
 #include "../typedef.h"
 #include <tchar.h>
 #define AFXAPI
 #include "_carray.h"

 #include "../../avp32/retcode3.h"
 #include "../../avp32/scanApi/avp_msg.h"
#endif

#if defined (MACHINE_IS_BIG_ENDIAN) 
#define _getword() ((WORD)((unsigned char)s_fp[0]+(((unsigned char)s_fp[1])<<8)))
#else 
#define _getword() (*((WORD*)s_fp))
#endif	

#define do_yield() (yield && y_count-- < 0 ? (y_count=0x40, ((void (*)())yield)(), 0) : 0)
#define fillbitbuf() {s_buf=_getword(); s_count = 16; s_fp+=2; }

#define getbit( b ) { --s_count; b = s_buf & bits[s_count]; if (s_count == 0) fillbitbuf(); }

#define getbyte() (*(s_fp++))


DWORD unsqu(BYTE *i_buf,BYTE *o_buf)
{
	const unsigned int bits[16] = { 
		1<<15, 1<<14, 1<<13, 1<<12, 1<<11, 1<<10, 1<<9,  1<<8,
		1<<7,  1<<6,  1<<5,  1<<4,  1<<3,  1<<2,  1<<1,  1<<0
	};
    DWORD len;
    DWORD bit;
    int span;
	BYTE*        s_fp    = i_buf;
	BYTE*        o_ptr   = o_buf;
	unsigned int s_buf;
	int s_count = 0;
	fillbitbuf();

	while(1){
		getbit(bit);
        if(bit) {
			*o_ptr++=getbyte();
			continue;
        }
		getbit(bit);
        if(!bit) {
			// len 2-5, span 0-ff
			getbit(bit);
			if (bit)
				len=2;
			else
				len=0;
			getbit(bit);
			if (bit)
				len++;
			len += 2;
			span=getbyte() | 0xffffff00;
        } else {
			span=getbyte();
			len=getbyte();
			span |= ((len & ~0x07)<<5) | 0xffffe000;
			len = (len & 0x07)+2;
			if (len==2) {
				len=getbyte();
				if(len==0)      break; // end mark of compressed load module
				if(len==1)      continue; // segment change
				else            len++;
			}
        }
		// "for" works better then memcpy
		for( ;len>0;len--,o_ptr++) 
			*o_ptr=*(o_ptr+span);
	}

    return(o_ptr-o_buf);
}

// the only difference here is do_yield() call
DWORD unsqu_y (BYTE *i_buf,BYTE *o_buf, void* yield_)
{
	const unsigned int bits[16] = { 
		1<<15, 1<<14, 1<<13, 1<<12, 1<<11, 1<<10, 1<<9,  1<<8,
		1<<7,  1<<6,  1<<5,  1<<4,  1<<3,  1<<2,  1<<1,  1<<0
	};
    DWORD len;
    DWORD bit;
    int span;
	BYTE*        s_fp    = i_buf;
	BYTE*        o_ptr   = o_buf;
	void*        yield   = yield_;
	int          y_count = 4;
	unsigned int s_buf;
	int s_count = 0;

	if (0 == yield_)
		return unsqu(i_buf,o_buf);

	fillbitbuf();

	while(1){
		getbit(bit);
        if(bit) {
			*o_ptr++=getbyte();
			continue;
        }
		getbit(bit);
        if(!bit) {
			// len 2-5, span 0-ff
			getbit(bit);
			if (bit)
				len=2;
			else
				len=0;
			getbit(bit);
			if (bit)
				len++;
			len += 2;
			span=getbyte() | 0xffffff00;
        } else {
			span=getbyte();
			len=getbyte();
			span |= ((len & ~0x07)<<5) | 0xffffe000;
			len = (len & 0x07)+2;
			if (len==2) {
				len=getbyte();
				if(len==0)      break; // end mark of compressed load module
				if(len==1)      continue; // segment change
				else            len++;
			}
			do_yield();
        }
		// "for" works better then memcpy
		for( ;len>0;len--,o_ptr++) 
			*o_ptr=*(o_ptr+span);
	}

    return(o_ptr-o_buf);
}

