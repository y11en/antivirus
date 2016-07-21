#include "stdafx.h"
#include "FiltersArray.h"

CFiltersArray::CFiltersArray()
{
}

CFiltersArray::~CFiltersArray()
{
}

CFiltersArray::ResetContent()
{
	PFILTER_TRANSMIT pFilter;
	int Count = GetSize();
	for (int cou = 0; cou < Count; cou++)
	{
		pFilter = ElementAt(cou);
		delete pFilter;
	}
	RemoveAll();
}