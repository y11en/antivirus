// ScanObjects.cpp: implementation of the CScanObjects class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ScanObjects.h"
#if defined (_WIN32)
#include <shlobj.h>
#endif

#define STRID_Q_WANTDELETEITEMS       "WantDeleteItemsQuestion"

//////////////////////////////////////////////////////////////////////

bool CScanObjectsList::OnCanOperateItem(tIdx nItem, eOpCode nOp)
{
	cScanObject * item = (cScanObject *)GetRowData(nItem);
	return item && !item->m_bSystem;
}

tERROR CScanObjectsList::OnEditItem(CItemBase * pOpCtl)
{
	cScanObject &item = *(cScanObject *)GetEditRowData();
	bool bNew = IsEditNewItem();
	
	if( bNew )
		item.m_bSystem = cFALSE;
	
	CBrowseObjectInfo pObject("BrowseForScanObject", SHELL_OBJTYPE_CUSTOM|SHELL_OBJTYPE_FILE|SHELL_OBJTYPE_FOLDER|SHELL_OBJTYPE_MASK|SHELL_OBJTYPE_PERSONAL|SHELL_OBJTYPE_MYCOMPUTER);
	pObject.m_strName = item.m_strObjName;
	pObject.m_pParent = pOpCtl;
	pObject.m_pData = &item;
	pObject.m_pEnum = this;

	if( bNew && m_bEx )
		pObject.m_strName = g_hGui->GetGuiVariables()->m_strLastScanAddedObject;
	
	if( !bNew )
	{
		switch(item.m_nObjType)
		{
		case OBJECT_TYPE_MY_COMPUTER:      pObject.m_nType = SHELL_OBJTYPE_MYCOMPUTER; break;
		case OBJECT_TYPE_DRIVE:            pObject.m_nType = SHELL_OBJTYPE_DRIVE;      break;
		case OBJECT_TYPE_FOLDER:           pObject.m_nType = SHELL_OBJTYPE_FOLDER;     break;
		case OBJECT_TYPE_FILE:             pObject.m_nType = SHELL_OBJTYPE_FILE;       break;
		case OBJECT_TYPE_MASKED:           pObject.m_nType = SHELL_OBJTYPE_MASK;       break;
		
		default:
			pObject.m_nType = SHELL_OBJTYPE_CUSTOM;
			pObject.m_nCustomId = item.m_nObjType;
			break;
		}
	}
	
	if( !Root()->BrowseObject(pObject) )
		return warnFALSE;

	Obj2Data(pObject, item);
	
	if( m_bEx )
		g_hGui->GetGuiVariables()->m_strLastScanAddedObject = pObject.m_strName;
	
	return errOK;
}

void CScanObjectsList::OnAddObject(CBrowseObjectInfo& pObject)
{
	cScanObject Data;
	Obj2Data(pObject, Data);
	if( AddObject(Data, true, true) )
		Item()->SetChangedData();
	//Item()->UpdateView(UPDATE_FLAG_SOURCE);

	if( m_bEx )
		g_hGui->GetGuiVariables()->m_strLastScanAddedObject = pObject.m_strName;
}

bool CScanObjectsList::NextObject(tDWORD& nCustomId)
{
	if( m_bEx )
	{
		switch( nCustomId )
		{
		default:			                   nCustomId = OBJECT_TYPE_MAIL; break;
		case OBJECT_TYPE_MAIL:                 nCustomId = OBJECT_TYPE_MEMORY; break;
		case OBJECT_TYPE_MEMORY:               nCustomId = OBJECT_TYPE_STARTUP; break;
		case OBJECT_TYPE_STARTUP:              nCustomId = OBJECT_TYPE_BOOT_SECTORS; break;
		case OBJECT_TYPE_BOOT_SECTORS:         nCustomId = OBJECT_TYPE_SYSTEM_RESTORE; break;
		case OBJECT_TYPE_SYSTEM_RESTORE:       nCustomId = OBJECT_TYPE_QUARANTINE; break;
		case OBJECT_TYPE_QUARANTINE:           nCustomId = OBJECT_TYPE_ALL_REMOVABLE_DRIVES; break;
		case OBJECT_TYPE_ALL_REMOVABLE_DRIVES: nCustomId = OBJECT_TYPE_ALL_FIXED_DRIVES; break;
		case OBJECT_TYPE_ALL_FIXED_DRIVES:     nCustomId = OBJECT_TYPE_ALL_NETWORK_DRIVES; break;
		case OBJECT_TYPE_ALL_NETWORK_DRIVES:   return false;
		}
	
		return true;
	}
	
	switch( nCustomId )
	{
	default:			                   nCustomId = OBJECT_TYPE_ALL_REMOVABLE_DRIVES; break;
	case OBJECT_TYPE_ALL_REMOVABLE_DRIVES: nCustomId = OBJECT_TYPE_ALL_FIXED_DRIVES; break;
	case OBJECT_TYPE_ALL_FIXED_DRIVES:     nCustomId = OBJECT_TYPE_ALL_NETWORK_DRIVES; break;
	case OBJECT_TYPE_ALL_NETWORK_DRIVES:   return false;
	}
	
	return true;
}

void CScanObjectsList::Obj2Data(CBrowseObjectInfo& pObject, cScanObject& Data)
{
	Data.m_strObjName = pObject.m_strName;
	Data.m_bEnabled = true;
	Data.m_bSystem = false;
	
	if( pObject.m_pData )
		Data.m_bRecursive = ((cScanObject *)pObject.m_pData)->m_bRecursive;

	switch( pObject.m_nType )
	{
	case SHELL_OBJTYPE_MYCOMPUTER: Data.m_nObjType = OBJECT_TYPE_MY_COMPUTER;            break;
	case SHELL_OBJTYPE_DRIVE:      Data.m_nObjType = OBJECT_TYPE_DRIVE;                  break;
	case SHELL_OBJTYPE_FOLDER:     Data.m_nObjType = OBJECT_TYPE_FOLDER;                 break;
	case SHELL_OBJTYPE_FILE:       Data.m_nObjType = OBJECT_TYPE_FILE;                   break;
	case SHELL_OBJTYPE_MASK:       Data.m_nObjType = OBJECT_TYPE_MASKED;                 break;
	default:                       Data.m_nObjType = (enScanObjType)pObject.m_nCustomId; break;
	}
}

//////////////////////////////////////////////////////////////////////

bool CScanObjectsList::AddObject(cScanObject& pObject, bool fUpdate, bool fSelect, bool bSystem)
{
	pObject.m_bSystem = bSystem;
	tERROR err = CheckItemExist(&pObject, fUpdate, fSelect);
	if( err == errOK )
		return InsertItem(TOR(cRowId, ()), &pObject);
	return PR_SUCC(err);
}

bool CScanObjectsList::AddAllSystemObjects()
{
	tString strDrive;
	tDWORD nCookie = 0;
	while( g_pRoot->EnumDrives(strDrive, &nCookie) )
	{
		cScanObject obj (OBJECT_TYPE_DRIVE, (tVOID *)strDrive.c_str(), cCP_ANSI);
		AddObject(obj, false, false, true);
	}

	Item()->UpdateView(0);
	return true;
}

void CScanObjectsList::ProcessDeviceAction(CDeviceAction * pInfo)
{
	bool bFound = false;
	for(size_t i = 0; i < GetCount(); i++)
	{
		cScanObject &item = *(cScanObject *)GetRowData(i);
		if( item.m_nObjType != OBJECT_TYPE_DRIVE )
			continue;

		if( toupper(item.m_strObjName.at(0)) != pInfo->m_cDevice )
			continue;

		if( pInfo->m_nAction == CDeviceAction::Delete && !item.m_bEnabled )
			remove(i);
		
		bFound = true;
		break;
	}

	if( !bFound && pInfo->m_nAction != CDeviceAction::Delete )
	{
		tString strDrive;
		strDrive += pInfo->m_cDevice;
		strDrive += ":\\";
		cScanObject obj (OBJECT_TYPE_DRIVE, (tVOID *)strDrive.c_str(), cCP_ANSI);
		AddObject( obj, false, false, true);
		bFound = true;
	}

	if( bFound )
		Item()->UpdateView(0);
}

bool CScanObjectsList::IsNotAllSystemObjectsSelected()
{
	cRowNode::cRowId cookie;
	for(tIdx nIdx; (nIdx = Item()->GetSelectedItem(cookie)) != CListItem::invalidIndex;)
		if( cScanObject *pItem = (cScanObject *)GetRowData(nIdx) )
			if( !pItem->m_bSystem )
				return true;

	return false;
}

//////////////////////////////////////////////////////////////////////

/*CTaskObjStat::CTaskObjStat() :
	m_bShowTotal(false)
{
}

bool CTaskObjStat::Bind(tDWORD nStatSerId)
{
	Reset();

	if( nStatSerId == cProtectionStatisticsEx::eIID )
		nStatSerId = cScanObjectStatistics::eIID;

	m_nStatSerId = nStatSerId;
	
	return m_pBinding ? m_pBinding->Bind(*this, CScanViewObject::eIID) : false;
}

void CTaskObjStat::Reset()
{
	m_aItems.clear();
	Item()->ClearList();
	m_bShowTotal = false;
}

void CTaskObjStat::Refresh(cSerializable * pStat)
{
	if( m_nStatSerId == cSerializable::eIID || !pStat )
		return;

	cVector<cScanObjectStatistics>* objStatList = NULL;
	if( pStat->isBasedOn(cProtectionStatisticsEx::eIID) )
		objStatList = &((cProtectionStatisticsEx *)pStat)->m_aObjStat;
	
	int nObjCount = objStatList ? objStatList->size() : 0;
	int nObjTotal = nObjCount ? nObjCount + 1 : 1;
	int nCurCount = m_aItems.size();

	if( nObjTotal > nCurCount )
	{
		if( !nCurCount ) {
			cScanObject obj (OBJECT_TYPE_TOTAL);
			AddObject(obj, false, false, true);
		}

		for(int i = !nCurCount ? 0 : nCurCount-1; i < nObjCount; i++)
			AddObject(objStatList->at(i).m_pScanObj, false, false, true);
	}

	for(tDWORD i = 0; i < m_aItems.size(); i++)
	{
		cSerializable * pItemStat = (&m_aItems[i])->m_stat;
		cProtectionStatistics * pIStat = i ? &objStatList->at(i-1) : (cProtectionStatistics *)pStat;

		if( !pItemStat || !pIStat )
			continue;

		pItemStat->assign(*pIStat, false);
	}

	Item()->UpdateView(UPDATE_FLAG_REINIT);
}

//////////////////////////////////////////////////////////////////////
*/

//////////////////////////////////////////////////////////////////////
