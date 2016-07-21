// VisualEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include <serialize/kldtser.h>
#include <datatreeutils/dtutils.h>
#include <Stuff\CPointer.h>
#include "VisualEditorDlg.h"
#include "CodeGenWizard.h"
#include "HeaderGenWizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// ---
void CVisualEditorDlg::OnFileSrcgen() {
	if ( SaveChangedRequest() ) {
		AVP_dword idSelectedIFace = 0;
		int selected = m_Interfaces.GetSelectedIndex();
		CCTItemsArray *items = m_Interfaces.GetItemsArray();
		if ( items && (selected < items->GetSize()) ) {
			CCheckTreeItem *item = (*items)[selected];
			HDATA hSelecetedIFace = CPPD(item)->m_hData;
			if ( hSelecetedIFace ) {
				HPROP hIDProp = ::DATA_Find_Prop( hSelecetedIFace, NULL, VE_PID_IF_DIGITALID );
				if ( hIDProp )
					idSelectedIFace = ::GetPropNumericValueAsDWord( hIDProp );
			}
		}
			
		CPointer<CDialog> pDialog;
		switch ( m_nWorkMode ) {
			case wmt_IDefinition :
//				pDialog = new CHeaderGenWizard( m_hTree, idSelectedIFace );
				pDialog = new CCodeGenWizard( m_hTree, idSelectedIFace, FALSE );
				pDialog->DoModal();
				break;
			case wmt_IImplementation : 
				pDialog = new CCodeGenWizard( m_hTree, idSelectedIFace, TRUE );
				pDialog->DoModal();
				break;
		}
		::SetFocus( m_Interfaces );
	}
}
