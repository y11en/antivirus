#ifndef __OPCACHE_H
#define __OPCACHE_H

#include "types.h"
#include "lock.h"

#define OPCACHEITEMS 32

typedef struct tag_OpCacheItem
{
	bool     valid;
	void*    result;
	uint64_t uniq;
} OpCacheItem;

class cOpCache {
public:
	cOpCache();
	~cOpCache();

	bool Add(uint64_t uniq, bool op_result);
	bool Set(uint64_t uniq, bool op_result);
	bool Get(uint64_t uniq, bool& op_result);
	bool Remove(uint64_t uniq, bool& op_result);

	bool Add(uint64_t uniq, void* op_result);
	bool Set(uint64_t uniq, void* op_result);
	bool Get(uint64_t uniq, void** op_result);
	bool Remove(uint64_t uniq, void** op_result);

private:
	cLock m_lock;
	OpCacheItem m_items[OPCACHEITEMS];
	uint_fast32_t m_hint;
};

#endif // __OPCACHE_H
