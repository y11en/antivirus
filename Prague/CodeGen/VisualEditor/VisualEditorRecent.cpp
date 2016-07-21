////////////////////////////////////////////////////////////////////
//
//  VisualEditorRecent.cpp
//  Recent files support implementation module
//  Project VisualEditor
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <Stuff\CPointer.h>
#include <StuffIO\IOUtil.h>
#include "Resource.h"
#include "VisualEditorDlg.h"

// ---
void CRecentFiles::Shake( const char *pFileName ) {
	BOOL bFound = false;

	for ( int i=0,c=Count(); i<c; i++ ) {
		if ( !lstrcmp((*this)[i], pFileName) ) {
			char *pName = (*this)[i];
			DetachInd( i );
			Insert( 0, pName );
			bFound = true;
			break;
		}
	}
	if ( !bFound ) {
		if ( Count() == MAX_RECENT_FILES )
			RemoveInd( MaxIndex() );

		char *pNewName = new char[lstrlen(pFileName) + 1];
		lstrcpy( pNewName, pFileName );
		Insert( 0, pNewName );
	}
}
			 

// ---
BOOL CRecentFiles::Find( const char *pFileName ) {
	for ( int i=0,c=Count(); i<c; i++ ) {
		if ( !lstrcmp((*this)[i], pFileName) ) 
			return true;
	}
	return false;
}

// ---
static HMENU FindRecentMenu( HWND hWnd ) {
	HMENU hMenu = ::GetMenu( hWnd );
	if ( hMenu ) {
		int nNumItems = ::GetMenuItemCount( hMenu );
		for ( int i=0; i<nNumItems; i++ ) {
			HMENU hChildMenu = ::GetSubMenu( hMenu, i );
			if ( hChildMenu && ::GetMenuState(hChildMenu, ID_FILE_EXIT, MF_BYCOMMAND) != (UINT)-1 ) {
				nNumItems = ::GetMenuItemCount( hChildMenu );
				for ( i=0; i<nNumItems; i++ ) {
					char pName[_MAX_PATH];
					::GetMenuString( hChildMenu, i, pName, sizeof(pName), MF_BYPOSITION );
					if ( !lstrcmp(pName, "Recent &Files") ) 
						return ::GetSubMenu( hChildMenu, i );
				}
				return 0;
			}
		}
	}
	return 0;
}


// ---
static void DeleteRecentItems( HMENU hMenu ) {
  for ( int i=::GetMenuItemCount( hMenu )-1; i>=0; i-- ) 
    ::DeleteMenu( hMenu, i, MF_BYPOSITION );
}

// ---
static char *NameMenuItem( const char *pName, int nIndex ) {
  char *pNewName = new char[strlen(pName)+7];
  strcpy( pNewName, pName/*::CompressPath( pName, 40 )*/ );
  memmove( pNewName+3, pNewName, strlen(pNewName)+1 );
  *(pNewName+0) = '&';
  _itoa( nIndex + 1, pNewName + 1, 10 );
  *(pNewName+2) = ' ';
  return pNewName;
}


// ---
void CVisualEditorDlg::PreProcessMenu() {
  HMENU hMenu = ::FindRecentMenu( GetSafeHwnd() );
  if ( hMenu ) {
    ::DeleteRecentItems( hMenu );

    for ( int i=0,c=m_RecentFiles.Count(); i<c; i++ ) {
      CAPointer<char> pItemText = ::NameMenuItem( m_RecentFiles[i], i );
      ::InsertMenu( hMenu, i, MF_STRING|MF_BYPOSITION, ID_FIRSTRECENTFILE + i, pItemText );
    }
  }
}

// ---
void CVisualEditorDlg::ShakeRecentMenu() {
	m_RecentFiles.Shake( m_FileName );
	PreProcessMenu();
}

// ---
BOOL CVisualEditorDlg::LoadRecentFile( int nIndex ) {
	if ( nIndex < (int)m_RecentFiles.Count() ) {
		m_FileName = m_RecentFiles[nIndex];

		if ( LoadInterfaces(m_FileName, false) ) {
			switch ( m_nWorkMode ) {
				case wmt_IDefinition :
					m_IDefinitionFileName = m_FileName;
					break;
				case wmt_IImplementation :
					m_IImplementationFileName = m_FileName;
					break;
			}
			UpdateData( FALSE );
			ShakeRecentMenu();
			SetWindowTitle();

			CString rcInitDirRegValue;
			switch ( m_nWorkMode ) {
				case wmt_IDefinition :
					rcInitDirRegValue = PRT_INITDIR_REG_VALUE;
					break;
				case wmt_IImplementation :
					rcInitDirRegValue = IMP_INITDIR_REG_VALUE;
					break;
			}

			char pCurrDir[_MAX_PATH];
			::IOSDirExtract( m_FileName, pCurrDir, sizeof(pCurrDir) );
			m_RegStorage.PutValue( rcInitDirRegValue, pCurrDir );
		}

		m_Interfaces.SetFocus();
		m_Interfaces.SelectNode( 0 );

		return true;
	}

	return false;
}


// ---
BOOL CVisualEditorDlg::OnCommand( WPARAM wParam, LPARAM lParam ) {
	if ( !HIWORD(wParam) &&
			 (LOWORD(wParam) >= ID_FIRSTRECENTFILE && LOWORD(wParam) < (ID_FIRSTRECENTFILE + MAX_RECENT_FILES))	) {

		if ( SaveChangedRequest() ) {
			int nRecentIndex = LOWORD(wParam) - ID_FIRSTRECENTFILE;
			LoadRecentFile( nRecentIndex );
		}

		return 0;
	}
	return CDialog::OnCommand(wParam, lParam);
}


// ---
LRESULT CVisualEditorDlg::OnRestoreDlgState( WPARAM wParam, LPARAM lParam ) {
	if ( !m_bStateRestored ) {
		m_bStateRestored = true;

		CAPointer<char> pName;
		CString rcValueName;

		if ( *AfxGetApp()->m_lpCmdLine ) {
			rcValueName = AfxGetApp()->m_lpCmdLine;
			rcValueName.TrimLeft( "\"" );
			rcValueName.TrimRight( "\"" );
			pName = new char[rcValueName.GetLength() + 1];
			lstrcpy( pName, rcValueName );
			m_RecentFiles.Add( pName.Relinquish() );
		}

		for ( int i=0; i<MAX_RECENT_FILES; i++ ) {
			pName = new char[_MAX_PATH];
			rcValueName.Format( "RecentFile%d", i );
			if ( m_RegStorage.GetValue(rcValueName, pName, _MAX_PATH) ) {
				if ( !m_RecentFiles.Find(pName) )
					m_RecentFiles.Add( pName.Relinquish() );
			}
			else
				break;
		}

		PreProcessMenu();

		if ( LoadRecentFile(0) ) {
		}
	}
	return Default();
}

