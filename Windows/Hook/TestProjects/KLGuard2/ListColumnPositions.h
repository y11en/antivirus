// ListColumnPositions.h: interface for the CListColumnPositions class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTCOLUMNPOSITIONS_H__94B74006_6B74_4903_AA65_0533550786B9__INCLUDED_)
#define AFX_LISTCOLUMNPOSITIONS_H__94B74006_6B74_4903_AA65_0533550786B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CListColumnPositions  
{
public:
	CListColumnPositions(PTCHAR ptchName, CListCtrl* pListCtrl);
	virtual ~CListColumnPositions();

	void SavePositions();
	void RestorePositions();

private:
	CListCtrl* m_pListCtrl;
	PTCHAR m_ptchName;
};

#endif // !defined(AFX_LISTCOLUMNPOSITIONS_H__94B74006_6B74_4903_AA65_0533550786B9__INCLUDED_)
