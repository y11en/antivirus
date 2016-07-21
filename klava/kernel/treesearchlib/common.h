// tree_common.h
//
//

#ifndef tree_common_h_INCLUDED
#define tree_common_h_INCLUDED

#ifndef ZeroMemory
#include <memory.h> 
#define ZeroMemory(ptr, size) memset(ptr, 0, size)
#endif

#ifdef TREE_STATIC_API
# define TREE_STATIC static
#else
# define TREE_STATIC
#endif

#include "debug.h"
#include "tree.h"
#include "treedefs.h"
#include "internal.h"

#endif // tree_common_h_INCLUDED

