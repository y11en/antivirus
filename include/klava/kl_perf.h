// kl_perf.h
//

#ifndef kl_perf_h_INCLUDED
#define kl_perf_h_INCLUDED

#include <kl_types.h>

////////////////////////////////////////////////////////////////
// X86

#ifdef KL_ARCH_X86

#ifdef __cplusplus

#if defined(_MSC_VER)
#pragma warning (disable : 4035)
inline uint64_t kl_read_tick_count ()
{
  __asm rdtsc 
}
#pragma warning (default : 4035)

#elif defined(__GNUC__)

inline uint64_t kl_read_tick_count ()
{
  uint64_t aResult;
  __asm__ __volatile__ ("rdtsc" : "=A"(aResult));
  return aResult;
}

#else
# error kl_perf is not supported for this compiler
#endif

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// PowerPC

#elif defined (KL_ARCH_PPC)

#ifdef __cplusplus

#if defined(__GNUC__)

inline uint64_t kl_read_tick_count ()
{
  uint64_t aResult = 0;
  //  __asm__  __volatile__ ( ".byte 0x0f,0x31" : "=r" (aResult) );
  return aResult;
}

#else
# error kl_perf is not supported for this compiler
#endif

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// Sparc

#elif defined (KL_ARCH_SPARC)

#ifdef __cplusplus

#if defined(__GNUC__)

inline uint64_t kl_read_tick_count ()
{
  uint64_t aResult = 0;
  //  __asm__  __volatile__ ( ".byte 0x0f,0x31" : "=r" (aResult) );
  return aResult;
}

#else
# error kl_perf is not supported for this compiler
#endif

#endif // __cplusplus
////////////////////////////////////////////////////////////////
// ARM

#elif defined (KL_ARCH_ARM)

#ifdef __cplusplus

#if defined(__GNUC__)

inline uint64_t kl_read_tick_count ()
{
  uint64_t aResult = 0;
  //  __asm__  __volatile__ ( ".byte 0x0f,0x31" : "=r" (aResult) );
  return aResult;
}

#else
# error kl_perf is not supported for this compiler
#endif

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// AMD64

#elif defined (KL_ARCH_AMD64)

#ifdef __cplusplus

#if defined(_MSC_VER)
#pragma warning (disable : 4035)
inline uint64_t kl_read_tick_count ()
{
//  __asm rdtsc 
  return 0;
}
#pragma warning (default : 4035)

#elif defined(__GNUC__)

inline uint64_t kl_read_tick_count ()
{
  uint64_t aResult;
  __asm__ __volatile__ ("rdtsc" : "=A"(aResult));
  return aResult;
}

#else
# error kl_perf is not supported for this compiler
#endif

#endif // __cplusplus

#else // KL_ARCH_X86
# error no timing support for this platform yet...
#endif


////////////////////////////////////////////////////////////////

struct KL_Perf_Counter
{
	uint64_t tick_count;  // tick counter, reported by RDTSC

#ifdef __cplusplus
	KL_Perf_Counter () { set (); }

	inline void set ()
		{ tick_count = kl_read_tick_count (); }

	inline uint32_t reset ()
		{ KL_Perf_Counter b;
		  uint32_t dt = b.diff (*this);
		  *this = b;
		  return dt;
		}
		  
	inline uint32_t read ()
		{ KL_Perf_Counter b;
		  return b.diff (*this);
		}

	inline uint32_t diff (const KL_Perf_Counter& b) const
	{
	  union u32x64 {
		uint32_t u[2];
		uint64_t x;
	  };

	  union u32x64 ux;

      ux.x = tick_count - b.tick_count;
      return (ux.u[1] << 22) | (ux.u[0] >> 10);
	}

#endif // __cplusplus
};

#endif // kl_perf_h_INCLUDED



