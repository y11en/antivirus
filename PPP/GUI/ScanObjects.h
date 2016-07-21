// ScanObjects.h: interface for the CScanObjects class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCANOBJECTS_H__A94200AE_75AF_406C_89D6_880D6DC4630C__INCLUDED_)
#define AFX_SCANOBJECTS_H__A94200AE_75AF_406C_89D6_880D6DC4630C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ListsView.h"

//////////////////////////////////////////////////////////////////////

class CScanObjectsList :
	public CVectorView,
	public CBrowseCallBack
{
public:
	CScanObjectsList(bool bEx = true) : m_bEx(bEx) {}

	bool AddAllSystemObjects();
	void ProcessDeviceAction(CDeviceAction * pInfo);

	// CVectorView
protected:
	bool     OnCanOperateItem(tIdx nItem, eOpCode nOp);
	tERROR   OnEditItem(CItemBase * pOpCtl);

	// CBrowseCallBack
protected:
	bool     NextObject(tDWORD& nCustomId);
	void     OnAddObject(CBrowseObjectInfo& pObject);

protected:
	bool     IsNotAllSystemObjectsSelected();
	bool     AddObject(cScanObject& pObject, bool fUpdate = false, bool fSelect = false, bool bSystem = false);
	void     Obj2Data(CBrowseObjectInfo& pObject, cScanObject& _Data);

protected:
	unsigned m_bEx : 1;
};

//////////////////////////////////////////////////////////////////////

/*class CTaskObjStat : public CScanObjectsList
{
public:
	CTaskObjStat();
	
	bool    Bind(tDWORD nStatSerId);
	void    Reset();
	void    Refresh(cSerializable * pStat);
	bool    OnCanOperateItem(int nItem, int nOp) { return false; }
	
public: // CDataReader
	bool    OnDeleteRecord(CRecord& pRecord) { return false; }

public:
	bool m_bShowTotal;
};*/

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_SCANOBJECTS_H__A94200AE_75AF_406C_89D6_880D6DC4630C__INCLUDED_)
