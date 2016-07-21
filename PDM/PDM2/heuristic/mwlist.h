#ifndef __cMemWriteList_H_
#define __cMemWriteList_H_

#include <vector>
#include "../eventmgr/include/types.h"
#include "../eventmgr/include/lock.h"


typedef std::vector<struct _tWriteMemoryBlock> _pWriteProcessMemoryList;

class cMemWriteList
{
public:
	cMemWriteList();
	~cMemWriteList();

	bool add(tPid src_pid, tPid dest_pid, tAddr addr, size_t size);
	bool remove(tPid src_pid);
	bool find(tPid src_pid, tPid dest_pid, tAddr addr);

private: 
	_pWriteProcessMemoryList m_WriteMemoryList;
	cLock m_lock;
};


#endif // __cMemWriteList_H_
