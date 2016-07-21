#ifndef _DRV_EVENT_LIST
#define _DRV_EVENT_LIST

#include <windows.h>
#include "OwnSync.h"

#define _ERR_NONE			0x00000
#define _ERR_INTERNAL		0x00001	
#define _ERR_EVENT			0x00002
#define _ERR_LOG			0x00004
#define _ERR_ADDLOG			0x00008


#define _ERR_DRVFAULT		0x00100
#define _ERR_DRVFAULT_FLT	0x00200

class DrvEventList : public COwnSync
{
public:
	LONG m_Errors;
	ULONG m_ErrorFlags;
public:
	DrvEventList();
	virtual ~DrvEventList();

	void SetError(ULONG ErrFalg);

	BYTE* Alloc(ULONG size);
	void Free(BYTE* ptr);

	bool AddNew(BYTE* ptr);
	BYTE* GetFirst();
private:
	LONG m_HeapCounter;
	LIST_ENTRY m_EventList;
};


#endif //_DRV_EVENT_LIST