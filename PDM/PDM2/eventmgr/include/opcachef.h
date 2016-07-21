#ifndef __OPCACHE_FILE_H
#define __OPCACHE_FILE_H

#include <assert.h>
#include "eventhandler.h"
#include "opcache.h"
#include "cpath.h"

#define MAKE_COMPLEX_UNIQ(file_uniq, pid) ((file_uniq) ^ ((pid) << 32) ^ ((pid) & FLAG_UNIQ_PID))

class cOpCacheFile : private cOpCache
{
public:
	bool DoPre(cEvent& event, cFile& file) {
		uint64_t uniq = file.getUniq();
		assert(uniq != 0);
		if (!uniq)
			return false;
		if (!file.isReadable())
			return false; // will be processed on Post
		uniq = MAKE_COMPLEX_UNIQ(uniq, event.m_pid);
		return Add(uniq, true);
	}
	
	bool DoPost(cEvent& event, cFile& file) {
		uint64_t uniq = file.getUniq();
		assert(uniq != 0);
		if (!uniq)
			return true;
		uniq = MAKE_COMPLEX_UNIQ(uniq, event.m_pid);
		bool processed_on_pre = false;
		if (Remove(uniq, processed_on_pre))
		{
			if (processed_on_pre == true)
				return false;
		}
		return true;
	}
};

#endif // __OPCACHE_FILE_H



