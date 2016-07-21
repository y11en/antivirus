#include "sq_u.h"

#define BS 0x1000

struct Unsqueeze
{
    BYTE  *s_fp;
    WORD  s_buf;
    BYTE  s_count;
    BYTE  *o_ptr;

    Unsqueeze(BYTE *i_buf,BYTE *o_buf)
    {
	    o_ptr=o_buf;
	    s_fp=i_buf;

	    s_buf=*((WORD*)s_fp);
	    s_fp+=2;
	    s_count=0x10;
    }

    BYTE getbyte() { return *s_fp++; }

    WORD getbit() 
    {
	    register WORD b;
	    b = s_buf & 1;
	    if(--s_count == 0)
	    {
		    s_buf=*((WORD*)s_fp);
		    s_fp+=2;
		    s_count= 0x10;
	    }
	    else
		    s_buf >>= 1;
    	
	    return b;
    }

    WORD process()
    {
	    short int len;
	    short int span;
	    if(getbit()) 
	    {
		    *o_ptr++=getbyte();
		    return 1;
	    }
    	
	    if(!getbit()) 
	    {
		    len=getbit()<<1;
		    len |= getbit();
		    len += 2;
		    span=getbyte() | 0xff00;
	    }
	    else 
	    {
		    span=getbyte();
		    len=getbyte();
		    span |= ((len & ~0x07)<<5) | 0xe000;
		    len = (len & 0x07)+2;
		    if (len==2) 
		    {
			    len=getbyte();
			    if(len==0)	return 0;// end mark of compreesed load module
			    if(len==1)	return 1;// segment change
			    else      	len++;
		    }
	    }
    	
	    for( ;len>0;len--,o_ptr++)	*o_ptr=*(o_ptr+span);
    	
	    return 1;
    }

};

DWORD Squeeze::unsqu (BYTE *i_buf,BYTE *o_buf)
{
	try
	{
		Unsqueeze u(i_buf, o_buf);
		while(u.process());
		return u.o_ptr-o_buf;
	}
	catch(...)
	{
	}
	
	return 0;
}


