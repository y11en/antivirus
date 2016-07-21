// timing.h
//

#ifndef timing_h_INCLUDED
#define timing_h_INCLUDED

#include <kl_types.h>

#ifdef KL_ARCH_X86

typedef struct tag_klav_timer
{
	uint64_t tick_count;  // processor tick counter, reported by RDTSC
} klav_timer;

#if defined _MSC_VER

#pragma warning (disable : 4035)
inline uint64_t klavReadTickCount ()
{
  __asm rdtsc 
}
#pragma warning (default : 4035)

#elif defined __GNUC__
inline uint64_t klavReadTickCount ()
{
  uint64_t aResult;
  __asm__ __volatile__ ("rdtsc" : "=A"(aResult));
  return aResult;
}
#endif

inline void klavSetTimer (klav_timer *t)
{
	t->tick_count = klavReadTickCount ();
}

inline void klavDiffTimer (klav_timer *t)
{
	t->tick_count = klavReadTickCount () - t->tick_count;
}

inline uint32_t klavScaleTickCount (const uint64_t& t)
{
    union u32x64 {
	    uint32_t u[2];
        uint64_t x;
	};

	union u32x64 ux;

    ux.x = t;
    return (ux.u[1] << 22) | (ux.u[0] >> 10);
}

inline uint32_t klavReadTimer (const klav_timer *t)
{
	return klavScaleTickCount (t->tick_count);
}

#else // KL_ARCH_X86
# error no timing support for this platform yet...
#endif


#endif // timing_h_INCLUDED



