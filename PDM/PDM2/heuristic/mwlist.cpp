#include "mwlist.h"
#include "trace.h"
#include "../eventmgr/include/lock.h"

#define _PAGE_SIZE		0x1000L
#define _wr_arr_size	16

//#define ALIGN_ADDR(addr) ((addr) & (~(((tAddr)_PAGE_SIZE-1))))
//#define ALIGN_SIZE(size) (((size)&(_PAGE_SIZE-1))==0?(size):(((size) & (~((_PAGE_SIZE-1))))+_PAGE_SIZE))
#define ALIGN_ADDR(X) ((X)&(~(((tAddr)_PAGE_SIZE)-1)))
#define ALIGN_SIZE(X) (((X)+_PAGE_SIZE-1)&(~(_PAGE_SIZE-1)))

typedef struct _tWrMemItem
{
	tAddr	m_Offset;
	size_t	m_Size;
}tWrMemItem;

typedef struct _tWriteMemoryBlock
{
	tPid		m_PidSrc;
	tPid		m_PidDst;
	tAddr		m_ArrIdx;
	tWrMemItem	m_MemArr[_wr_arr_size];
}tWriteMemoryBlock;


cMemWriteList::cMemWriteList()
{
}
cMemWriteList::~cMemWriteList()
{
}

bool cMemWriteList::add(tPid src_pid, tPid dest_pid, tAddr addr, size_t size)
{
	_tWriteMemoryBlock* pmb;
	
	addr = ALIGN_ADDR(addr);
	size = ALIGN_SIZE(size);

	cAutoLock lock(m_lock);
	for (_pWriteProcessMemoryList::iterator _it = m_WriteMemoryList.begin(); _it != m_WriteMemoryList.end(); _it++)
	{
		pmb = &*_it;
		if (pmb->m_PidSrc == src_pid && pmb->m_PidDst == dest_pid)
		{
			bool bExist = false;
			for (int cou = 0; cou < _wr_arr_size; cou++)
			{
				if (pmb->m_MemArr[cou].m_Offset < addr && 
					pmb->m_MemArr[cou].m_Offset + pmb->m_MemArr[cou].m_Size > addr)
				{
					bExist = true;
					break;
				}
			}
			
			if (!bExist)
			{
				pmb->m_MemArr[pmb->m_ArrIdx].m_Offset = addr;
				pmb->m_MemArr[pmb->m_ArrIdx].m_Size = size;
				pmb->m_ArrIdx++;
				if (pmb->m_ArrIdx == _wr_arr_size)
					pmb->m_ArrIdx = 0;

				PR_TRACE((g_root, prtIMPORTANT, TR "Add2WriteProcessMemoryList pid %d, offset 0x%x size %d)", 
					ProcessId, addr, size));
			}

			return true;
		}
	}

	_tWriteMemoryBlock mb;
	memset(&mb, 0, sizeof(mb));

	mb.m_PidSrc = src_pid;
	mb.m_PidDst = dest_pid;
	mb.m_ArrIdx = 1;
	mb.m_MemArr[0].m_Offset = addr;
	mb.m_MemArr[0].m_Size = size;
	m_WriteMemoryList.push_back(mb);

	PR_TRACE((g_root, prtIMPORTANT, TR "Add2WriteProcessMemoryList pid %d, offset 0x%x size %d) (first block)", 
		ProcessId, addr, size));

	return true;
}

bool cMemWriteList::remove(tPid pid)
{
	_tWriteMemoryBlock* pmb;
	bool res = false;

	cAutoLock lock(m_lock);
	for (_pWriteProcessMemoryList::iterator _it = m_WriteMemoryList.begin(); _it != m_WriteMemoryList.end();)
	{
		pmb = &*_it;
		if (pmb->m_PidSrc == pid || pmb->m_PidDst == pid)
		{
			PR_TRACE((g_root, prtNOTIFY, TR "WasWriteFrom clear %d", ProcessId));
			_it = m_WriteMemoryList.erase(_it);
			res = true;
		}
		else
		{
			 ++_it;
		}
	}
	return res;
}

bool cMemWriteList::find(tPid src_pid, tPid dest_pid, tAddr addr)
{
	addr = ALIGN_ADDR(addr);

	PR_TRACE((g_root, prtIMPORTANT, TR "WasWriteFrom check pid %d offset 0x%x", ProcessId, addr));

	if (!addr)
		return false;

	_tWriteMemoryBlock* pmb;

	cAutoLock lock(m_lock);
	for (_pWriteProcessMemoryList::iterator _it = m_WriteMemoryList.begin(); _it != m_WriteMemoryList.end(); _it++)
	{
		pmb = &*_it;
		if (pmb->m_PidSrc == src_pid && pmb->m_PidDst == dest_pid)
		{
			for (int cou = 0; cou < _wr_arr_size; cou++)
			{
				if (pmb->m_MemArr[cou].m_Offset <= addr && 
					pmb->m_MemArr[cou].m_Offset + pmb->m_MemArr[cou].m_Size > addr)
				{
					PR_TRACE((g_root, prtIMPORTANT, TR "WasWriteFrom true (pid %d)", ProcessId));
					return true;
				}
			}
			
			return false;
		}
	}

	return false;
}
