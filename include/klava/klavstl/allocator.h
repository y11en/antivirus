// allocator.h
//

#ifndef allocator_h_INCLUDED
#define allocator_h_INCLUDED

#include <stdlib.h>
#include <string.h>

#include <kl_types.h>

#ifdef KLAVA
//	NB: in order to prevent definition conflict, a link may not include <new>, either directly, or indirectly
	inline void* operator new  (size_t size, void* ptr) throw() { return ptr; }
	inline void* operator new[](size_t size, void* ptr) throw() { return ptr; }
	inline void  operator delete  (void* ptr, void*) throw() {}
	inline void  operator delete[](void* ptr, void*) throw() {}
#else // KLAVA
#	include <new>
#endif // KLAVA

#ifdef _MSC_VER
# pragma intrinsic (memcpy,memset)
#endif

#ifdef _WIN32_WCE		// by ArtemK
	#if (_WIN32_WCE <= 400)	&& !defined(_STLP_INTERNAL_CONSTRUCT_H)	// WinCCe.NET does not need it
		inline void *__cdecl operator new (size_t, void *p) { return (p); }
	#else
		void * operator new (size_t, void *p);
	#endif
#endif

namespace klavstl
{

////////////////////////////////////////////////////////////////
// some base algorithms

#ifdef min
# undef min
#endif // min
#ifdef max
# undef max
#endif // max

template <class T>
inline void swap (T& a, T& b)
{ T tmp = a; a = b; b = tmp; }

template <class T>
inline const T& min (const T& a, const T& b)
{ return (b < a) ? b : a; }

template <class T>
inline const T& max (const T& a, const T& b)
{ return (a < b) ? b : a; }

////////////////////////////////////////////////////////////////
// standard malloc-based allocator implementation

class malloc_allocator
{
public:
	uint8_t * alloc (size_t size)
		{ return (uint8_t *)::malloc (size); }

	uint8_t * realloc (void *p, size_t nsz)
		{ return (uint8_t *)::realloc (p, nsz); }

	void free (void *p)
		{ ::free (p); }
};

typedef ptrdiff_t distance_type;

extern "C"
void memmove (void *dst, const void *src, size_t);

#define DECLARE_STD_TYPEDEFS(T) \
	typedef Allocator allocator_type;  \
	typedef size_t size_type;          \
	typedef ptrdiff_t difference_type; \
	typedef T * pointer;               \
	typedef const T * const_pointer;   \
	typedef T& reference;              \
	typedef const T& const_reference;  \
	typedef T value_type;              \
	typedef pointer iterator;          \
	typedef const_pointer const_iterator;

#define DECLARE_PARENT_TYPEDEFS(Parent) \
	typedef typename Parent::allocator_type allocator_type;   \
	typedef typename Parent::size_type size_type;             \
	typedef typename Parent::difference_type difference_type; \
	typedef typename Parent::pointer pointer;                 \
	typedef typename Parent::const_pointer const_pointer;     \
	typedef typename Parent::reference reference;             \
	typedef typename Parent::const_reference const_reference; \
	typedef typename Parent::value_type value_type;           \
	typedef typename Parent::iterator iterator;               \
	typedef typename Parent::const_iterator const_iterator;

} // namespace klavstl

#endif // allocator_h_INCLUDED

