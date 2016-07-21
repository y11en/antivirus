/*
*/

#if !defined(__KL_DSKMWRAP_ALLOCATORS_H)
#define __KL_DSKMWRAP_ALLOCATORS_H

#include "types.h"

namespace KL  {
namespace DskmLibWrapper  {

//////////////////////////////////////////////////////////////////////////
class DefaultAllocator
{
public:
	static void* Allocate(size_t size)
		{ return new(std::nothrow) byte_t[size]; }
	static void Deallocate(void* p)
		{ delete [] static_cast<byte_t*>(p); }
};

//////////////////////////////////////////////////////////////////////////
class MallocAllocator
{
public:
	static void* Allocate(size_t size)
		{ return malloc(size); }
	static void Deallocate(void* p)
		{ free(p); }
};

}  // namespace DSKM
}  // namespace KL

#endif  // !defined(__KL_DSKMWRAP_ALLOCATORS_H)
