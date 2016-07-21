#include "calcsum.h"

unsigned long CalcSum(const unsigned char *mass, const unsigned int len)
{
    register unsigned long i = 0;
    register unsigned long s = 0;
    register const unsigned char *m = mass;
    if(len < 4)
    {
        for(i=0; i < len ; i++, m++)
        {
            s^=s<<8;
            s^=*m;
        }
        return s;
    }
    
    for(i=0; i<4 ; i++,m++)
    {
        s^=s<<8;
        s^=*m;
    }
    for( ;i < len; i++,m++)
    {
        s^=s<<1;
        s^=*m;
    }
    return s;
}
