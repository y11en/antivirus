#include "IniLib.cpp"

#include "IniRead.h"

#include "../../AVP32/pagememorymanager/pagememorymanager.h"
#include "../../AVP32/pagememorymanager/pagememorymanager.cpp"

/////////////////////////////////////////////////////////////////////////////////////
// Read Only

sIniMm * IniReadMmCreate()
{
	return (sIniMm *)new PageMemoryManagerNS::PageMemoryManager();
}

tERROR IniReadMmLoad(sIniSection** head, sIniMm * mm, hIO io, tDWORD nFlags, tIniLoadFilter fltfunc, tPTR fltctx)
{
	if( !head || !mm || !io )
		return errPARAMETER_INVALID;

	struct _A : public sIniAllocator
	{
		tERROR Alloc(tPTR* result, tDWORD size) { *result = pmm->allocate(size); return *result ? errOK : errNOT_OK; }
		tERROR Free(tPTR ptr) { return warnFALSE; }

		PageMemoryManagerNS::PageMemoryManager* pmm;
	} _a;
	
	_a.pmm = (PageMemoryManagerNS::PageMemoryManager*)mm;
	return IniLoadEx(io, head, NULL, NULL, &_a, nFlags|INI_LOAD_SMALLALLOC, NULL, fltfunc, fltctx);
}

void IniReadMmClear(sIniMm * mm)
{
	PageMemoryManagerNS::PageMemoryManager* pmm = (PageMemoryManagerNS::PageMemoryManager *)mm;
	if( pmm )
		pmm->deallocate();
}

void IniReadMmDestroy(sIniMm * mm)
{
	if( mm )
		delete (PageMemoryManagerNS::PageMemoryManager *)mm;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
