// FiltersStorage.h: interface for the CFiltersStorage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTERSSTORAGE_H__4F3C6AB7_80F9_4E15_8C7D_D2A4C4EE5D60__INCLUDED_)
#define AFX_FILTERSSTORAGE_H__4F3C6AB7_80F9_4E15_8C7D_D2A4C4EE5D60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FiltersArray.h"

class CFiltersStorage
{
public:
	CFiltersStorage(HANDLE hDevice);
	virtual ~CFiltersStorage();

	void ResetState();
	void ReloadFiltersArray(void);
	bool DeleteFilter(PFILTER_TRANSMIT pFilter, bool bAlternate = false);
	bool AddNewFilter(PFILTER_TRANSMIT pFilter, DWORD* pAdditionSite);
	bool IsFiltered(PFILTER_TRANSMIT pFilter);

	CFiltersArray m_FiltersArray;
	HANDLE m_hDevice;

	BOOL FilterTransmit(HANDLE hDevice, PFILTER_TRANSMIT pInFilter, FILTER_TRANSMIT* pOutFilter);
private:
	bool m_bIsModified;
};

int SwitchProProt(CFiltersStorage* pFiltersStorage, int Request);
BOOL AddProProt(CFiltersStorage* pFiltersStorage);

#endif // !defined(AFX_FILTERSSTORAGE_H__4F3C6AB7_80F9_4E15_8C7D_D2A4C4EE5D60__INCLUDED_)
