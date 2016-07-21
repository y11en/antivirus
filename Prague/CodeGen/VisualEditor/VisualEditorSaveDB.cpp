// VisualEditorLoadSave.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include <serialize/kldtser.h>
#include "VisualEditor.h"
#include "VisualEditorDlg.h"
#include "PrepareDBTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// ---
void CVisualEditorDlg::SaveDescriptionDB() {
	if ( m_hTree && m_pIIServer ) {
		DWORD dwSize = 0;
		BYTE  *pBuffer = (BYTE *)::SerializeDTree( m_hTree, dwSize );
		TCHAR *pFileName = (TCHAR *)::CoTaskMemAlloc( m_FileName.GetLength() + 1 );
		lstrcpy( pFileName,	m_FileName );

		HRESULT hError = m_pIIServer->SaveDescriptionDB( (BYTE *)pFileName, dwSize, pBuffer );
		
	  ::CoTaskMemFree( pFileName );
		::CoTaskMemFree( pBuffer );

		m_FileName.ReleaseBuffer();

		if ( hError != S_OK ) {
			DisplayPISError( hError );
		}
	}
}

/* OLD TECHNOLOGY
// ---
void CVisualEditorDlg::SaveDescriptionDB() {
	CWaitCursor wait;

	if ( m_hTree ) {

		HDATA hResultData = ::PrepareDBTree( m_hTree );

		if ( hResultData ) {
			CString rcNewFileName = m_FileName;
			int nInd = rcNewFileName.ReverseFind( L'.' );
			if ( nInd >= 0 ) 
				rcNewFileName = rcNewFileName.Mid( 0, nInd );
			rcNewFileName += ".dsc";

			AVP_bool nResult = ::KLDT_SerializeUsingSWM( rcNewFileName, hResultData, "" );

			::DATA_Remove( hResultData, NULL );

			if ( !nResult ) {
				CString rcString;
				rcString.Format( IDS_ERR_WRITEFILE, LPCSTR(rcNewFileName) );
				::DlgMessageBox( this, rcString );
			}
		}
	}
}
*/

