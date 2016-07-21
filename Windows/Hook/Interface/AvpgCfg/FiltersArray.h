#ifndef _FILTERSARRAY
#define _FILTERSARRAY

#include "stdafx.h"
#include <afxtempl.h>

//class CFiltersArray : public CPArray<PFILTER_TRANSMIT>
class CFiltersArray : public CTypedPtrArray<CPtrArray, PFILTER_TRANSMIT>
{
public:
	CFiltersArray();
	~CFiltersArray();
	
	ResetContent();
};

#endif
