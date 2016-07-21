#if defined( _WINDOWS ) || defined( _Windows )
#  include <windows.h>
#elif defined( _WINVXD )
#endif
#if defined( __unix__ ) || defined( __TANDEM )
#  include <sys/times.h>
#endif
#if defined( VAX )
#  include <signal.h>
#  include <iodef.h>
#  include <descrip.h>
#endif
#include <time.h>
#include "CT_SUPP.h"

#define a 16807
#define m 2147483647L
#define q 127773L
#define r 2836

static CT_LONG Randomnum = 1;
static CT_UINT ofs;

static CT_LONG nextlongrand( CT_LONG seed )
{
    CT_ULONG lo, hi;

    lo = a * (CT_LONG)(seed & 0xFFFF);
    hi = a * (CT_LONG)((CT_ULONG)seed >> 16);
    lo += (hi & 0x7FFF) << 16;
    if (lo > m)
    {
        lo &= m;
        ++lo;
    }
    lo += hi >> 15;
    if (lo > m)
    {
        lo &= m;
        ++lo;
    }

    return (CT_LONG)lo;
}

CT_LONG CTAPI GetURandValue()
{
    return Randomnum;
}

void CTAPI SetURandValue( CT_LONG sr )
{
    Randomnum = sr;
}

void CTAPI Randomize()
{
#if defined( __unix__ ) || defined( __TANDEM )
    struct tms t;
#endif
#ifdef VAX
    CT_ULONG vms_clock_bits[2];
#endif

#if defined( __unix__ ) || defined( __TANDEM )
    times( &t );
#endif
#if defined( VAX )
    SYS$GETTIM( vms_clock_bits );
#endif
#if defined( _WINVXD )
	DWORD HiDWord;
	DWORD LoDWord;

	VTD_Get_Real_Time( &HiDWord, &LoDWord);
#elif defined( _WINSYS )
	LARGE_INTEGER ct;
	KeQuerySystemTime( &ct );
#endif

    Randomnum = 
#if defined( _WINSYS )
		(CT_LONG)ct.HighPart ^ (ofs++)
                      ^ ct.LowPart
#elif defined( _WINDOWS ) || defined( _Windows )
		(CT_LONG)time( (time_t *)0 ) ^ (ofs++)
                      ^ (CT_ULONG)GetTickCount()
#elif defined( MSDOS ) || defined( __MSDOS__ ) || defined( __OS2__ )
		(CT_LONG)time( (time_t *)0 ) ^ (ofs++)
                      ^ clock()
#elif defined( __unix__ ) || defined( __TANDEM )
		(CT_LONG)time( (time_t *)0 ) ^ (ofs++)
                      ^ (CT_ULONG)t.tms_stime
#elif defined( VAX )
		(CT_LONG)time( (time_t *)0 ) ^ (ofs++)
                      ^ (CT_ULONG)vms_clock_bits[1]
#elif defined( _WINVXD )
		(CT_LONG)HiDWord ^ (ofs++)
                      ^ LoDWord

#endif
;
}

CT_INT CTAPI URand( CT_INT num )
{
    Randomnum = nextlongrand( Randomnum );
    return (CT_INT)(Randomnum % num );
}

