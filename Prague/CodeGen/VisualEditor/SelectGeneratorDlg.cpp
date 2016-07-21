// SelectGeneratorDlg.cpp : implementation file
//

#include "stdafx.h"
#include <comcat.h>
#include <Stuff\Layout.h>
#include <PropertySetGet.h>
#include <Stuff\COMUtils.h>
#include <Stuff\PathStr.h>
#include <StuffIO\IOUtil.h>
#include <property/property.h>
#include "Resource.h"
#include "VisualEditor.h"
#include "SelectInterfacesDlg.h"
#include "VisualEditorDlg.h"
#include "CodeGenTools.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define COM_INITDIR_REG_VALUE			_T("COMInitialDir")

void PrepareFilterIndex( OPENFILENAME &pOfn, const char *pDefExt );

// ---
BOOL CSelectInterfacesDlg::DestroyWindow() {
	CCLItemsArray *pItems = m_Objects.GetItemsArray();
	if ( pItems ) {
		for ( int i=0,c=pItems->GetSize(); i<c; i++ ) {
			CCheckListItem *item = (*pItems)[i];
			IPragueCodeGen *pInterface = CGPPD(item)->m_pInterface;
			if ( pInterface )
				pInterface->Release();
			delete CGPPD(item);
		}
	}
	::CoFreeUnusedLibraries();
	return CDialog::DestroyWindow();
}

// ---
void CSelectInterfacesDlg::FillGeneratorsList( CCLItemsArray *pItems ){

	m_nSelected = ::FillGeneratorsList(pItems, m_hRestoredData, m_rCLSID, m_PublicDir);

	m_Objects.SetItemsArray( pItems );
	if ( pItems->GetSize() )
		m_Objects.SetItemSelected( m_nSelected );
	else
		m_Objects.SetItemSelected( -1 );
	m_Objects.SetFocus();
}

// ---
bool CSelectInterfacesDlg::SelectGeneratorModule( CString &rcFileName ) {
	CString rcExtString;
	rcExtString.LoadString( IDS_CG_DEFEXTENTION );
	
	CString rcFltrString;
	rcFltrString.LoadString( IDS_CG_FILTER );
	
	CString rcInitDirRegValue = COM_INITDIR_REG_VALUE;
	
	CFileDialog rcOpenDlg( TRUE, rcExtString, NULL, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, rcFltrString, this );
	
	char pCurrDir[_MAX_PATH];
	char pFileName[_MAX_PATH];
	
	CRegStorage	rcRegStorage( CFG_REG_KEY );

	if ( rcFileName.IsEmpty() ) {
		if ( !rcRegStorage.GetValue(rcInitDirRegValue, pCurrDir, sizeof(pCurrDir)) ) 
			::IOSGetCurrentDirectory( pCurrDir, sizeof(pCurrDir) );
		
		rcOpenDlg.m_ofn.lpstrInitialDir = pCurrDir;
	}
	else {
		::IOSDirExtract( rcFileName, pCurrDir, sizeof(pCurrDir) );
		rcOpenDlg.m_ofn.lpstrInitialDir = pCurrDir;
		
		::IOSFileExtract( rcFileName, pFileName, _MAX_PATH );
		rcOpenDlg.m_ofn.lpstrFile = pFileName;
		rcOpenDlg.m_ofn.nMaxFile = sizeof(pFileName);
	}
	
	::PrepareFilterIndex( rcOpenDlg.m_ofn, rcExtString );
	
	bool bResult = rcOpenDlg.DoModal() == IDOK;
	
	if ( bResult ) {
		rcFileName = rcOpenDlg.GetPathName();
		
		::IOSDirExtract( rcFileName, pCurrDir, sizeof(pCurrDir) );
		rcRegStorage.PutValue( rcInitDirRegValue, pCurrDir );
	}
	
	return bResult;
}

// ---
static CString GetObjectVersion( REFCLSID rCLSID ) {
	BYTE		 pBuffer[20] ={0};
	OLECHAR *pWClsidStr;
	CHAR     pClsidStr[40] = {0};
	if ( SUCCEEDED(::StringFromCLSID(rCLSID, &pWClsidStr)) ) {
		::WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, pWClsidStr, -1, pClsidStr, sizeof(pClsidStr), 0, 0 );
		::CoTaskMemFree( pWClsidStr );

		CPathStr rcKeyPath( "CLSID" );

		rcKeyPath ^= pClsidStr;
		rcKeyPath ^= "Version";

		HKEY hKey = NULL;

		if ( ::RegOpenKeyEx(HKEY_CLASSES_ROOT, rcKeyPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS &&
				 hKey ) {
			DWORD dwSize = sizeof(pBuffer);
			::RegQueryValueEx( hKey, NULL, 0, NULL, (BYTE *)pBuffer, &dwSize );
			::RegCloseKey( hKey );
		}
	}
	return CString(pBuffer);
}

// ---
static void FillObjectInfo(  CCLItemsArray &items, HDATA hAddInfo ) {
	HDATA hCurrData = ::DATA_Get_First( hAddInfo, NULL );
	while ( hCurrData ) {
		HPROP hProp = ::DATA_Get_First_Prop ( hCurrData, NULL );
		if ( hProp ) {
			CCheckListItem *item = new CCheckListItem;

			CAPointer<char> pText = ::GetPropValueAsString( hProp, NULL );
			item->SetText( LPSTR(pText) );
		
			CStringList lSubTexts;

			hProp = ::PROP_Get_Next( hProp );
			while( hProp ) {
				pText = ::GetPropValueAsString( hProp, NULL );
				lSubTexts.AddTail( LPCSTR(pText) );
				hProp = ::PROP_Get_Next( hProp );
			}

			item->SetSubItemsTexts( lSubTexts );
		
			items.Add( item );
		}
		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}
}


// ---
void CSelectInterfacesDlg::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if ( (pNMListView->uChanged & LVIF_STATE) && 
			 (pNMListView->uNewState & LVIS_FOCUSED) &&
			 !pNMListView->uOldState ) {

		CCLItemsArray *pItems = m_Objects.GetItemsArray();
		CCheckListItem *item;

		if ( pItems && m_nSelected < pItems->GetSize() ) {
			item = (*pItems)[m_nSelected];
			if ( m_pInterface )
				CGPPD(item)->m_pInterface = m_pInterface;
		}

		m_nSelected = pNMListView->iItem;

		if ( pItems && m_nSelected < pItems->GetSize() ) {
			item = (*pItems)[m_nSelected];
			m_pInterface = CGPPD(item)->m_pInterface;
			m_rCLSID = CGPPD(item)->m_CLSID;
			CGPPD(item)->m_pInterface = NULL;
			
			SCodeGenItem* pCacheItem = GetGenCacheItem(m_rCLSID);
			if(pCacheItem){
				if(!pCacheItem->m_strPublicDir.IsEmpty())
					m_rcPublicDirName = m_PublicDir = pCacheItem->m_strPublicDir;

				UpdateData(FALSE);
			}
		}
	}
	
	*pResult = 0;
}


// ---
void CSelectInterfacesDlg::OnAddGenerator() {
	CString rcFileName;
	if ( SelectGeneratorModule(rcFileName) && RegisterCOM(rcFileName) ) {
		CCLItemsArray *pItems = new CCLItemsArray( true );
		LoadCodeGenItemsCache(m_hRestoredData);
		FillGeneratorsList( pItems );
	}
}


// ---
void CSelectInterfacesDlg::OnUnregGenerator() {
	CCLItemsArray* pItems = m_Objects.GetItemsArray();

	if ( !pItems || (m_nSelected >= pItems->GetSize()) ) 
		return;

	CCheckListItem *item = (*pItems)[m_nSelected];
	if ( !item )
		return;

	LPOLESTR wstr;
	::StringFromCLSID( CGPPD(item)->m_CLSID, &wstr );

	char pFileName[_MAX_PATH];
	char skey[MAX_PATH];

	::lstrcpy( skey, _T("CLSID\\") );
	DWORD len = ::lstrlen( skey );
	::wcstombs( skey+len, wstr, sizeof(skey)-len );
	::CoTaskMemFree( wstr );
	::lstrcat( skey, "\\InprocServer32" );
	
	CRegStorage	rcRegStorage( HKEY_CLASSES_ROOT, skey );
	len = sizeof(pFileName);
	if ( !rcRegStorage.GetValue(0,pFileName,len) )
		return;
	
	if ( !UnregisterCOM(pFileName) )
		return;

	pItems = new CCLItemsArray( true );
	LoadCodeGenItemsCache(m_hRestoredData);
	FillGeneratorsList( pItems );
}

void CSelectInterfacesDlg::OnCgUpdateGeneratorList() {
	CCLItemsArray* pItems = new CCLItemsArray( true );
	LoadCodeGenItemsCache(m_hRestoredData);
	FillGeneratorsList( pItems );
}
