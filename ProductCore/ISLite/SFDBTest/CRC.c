#include <windows.h>
#ifdef _WIN32
static DWORD CountCRC32(int size, LPBYTE ptr,DWORD dwOldCRC32)
{
    dwOldCRC32 = ~dwOldCRC32;
    while(size-- != 0)
    {
        unsigned long i;
        unsigned long c = *ptr++;
        for( i=0; i<8; i++)
        {
            if (((dwOldCRC32^c)&1)!=0)
                dwOldCRC32 = (dwOldCRC32>>1) ^ 0xEDB88320;
            else
                dwOldCRC32 = (dwOldCRC32>>1);
            c>>=1;
        }
    }
    return ~dwOldCRC32;
}

#else
static DWORD FAR CountCRC32(int size, LPBYTE ptr, DWORD dwOldCRC32)
{
    dwOldCRC32 = ~dwOldCRC32;
    while(size-- != 0)
    {
        unsigned long i;
        unsigned long c = *ptr++;
        for( i=0; i<8; i++)
        {
            if (((dwOldCRC32^c)&1)!=0)
                dwOldCRC32 = (dwOldCRC32>>1) ^ 0xEDB88320;
            else
                dwOldCRC32 = (dwOldCRC32>>1);
            c>>=1;
        }
    }
    return ~dwOldCRC32;
}

#endif