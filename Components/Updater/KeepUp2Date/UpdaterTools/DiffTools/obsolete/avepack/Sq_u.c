////////////////////////////////////////////////////////////////////
//
//  SQ_U.CPP
//  Unpack proc for squish
//  General purpose
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include "fakeoop.h"

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

static BYTE* s_fp=NULL;
static BYTE* o_ptr=NULL;
static void* yield=NULL;
static WORD  s_buf=0;
static BYTE  s_count=0;

static int  y_count=0;

void fillbitbuf()
{
#if defined (MACHINE_IS_BIG_ENDIAN)
	s_buf=(WORD)((unsigned char)s_fp[0]+(((unsigned char)s_fp[1])<<8));
#else
	s_buf=*((WORD*)s_fp);
#endif	
	s_fp+=2;
	s_count= 0x10;
	
	if(yield && y_count-- < 0){
		y_count=4;
		((void (*)())yield)();
	}
}


#define getbit( b )\
{\
	b = s_buf & 1;\
	if(--s_count) s_buf >>= 1;\
	else fillbitbuf();\
}\
	
#define getbyte() (*(s_fp++))


#if ((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__)))||defined(__unix__))
void init()
#else
static void init()
#endif
{
#if defined (MACHINE_IS_BIG_ENDIAN)
	s_buf=(WORD)((unsigned char)s_fp[0]+(((unsigned char)s_fp[1])<<8));
#else
	s_buf=*((WORD*)s_fp);
#endif	
	s_fp+=2;
	s_count=0x10;
}

static int process(){
    DWORD len;
    DWORD bit;
    short int span;
	while(1){
		getbit(bit);
        if(bit) {
			*o_ptr++=getbyte();
			continue;
        }
		getbit(bit);
        if(!bit) {
			getbit(len);
			len<<=1;
			getbit(bit);
			len |= bit;
			len += 2;
			span=getbyte() | 0xff00;
        } else {
			span=getbyte();
			len=getbyte();
			span |= ((len & ~0x07)<<5) | 0xe000;
			len = (len & 0x07)+2;
			if (len==2) {
				len=getbyte();
				if(len==0)      return 0;// end mark of compreesed load module
				if(len==1)      continue;// segment change
				else            len++;
			}
        }
        for( ;len>0;len--,o_ptr++)      *o_ptr=*(o_ptr+span);
	}
	return 1;
}

DWORD unsqu (BYTE *i_buf,BYTE *o_buf)
{
    o_ptr=o_buf;
    s_fp=i_buf;
    init();
    process();
    return(o_ptr-o_buf);
}

DWORD unsqu_y (BYTE *i_buf,BYTE *o_buf, void* yield_)
{
	yield=yield_;
	return unsqu(i_buf,o_buf);
}
