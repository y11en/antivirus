#include "stdafx.h"
#include "VisualEditorDlg.h"
#include "PPOptions.h"

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void CVisualEditorDlg::OnUpdateOptions(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_bTechnicalEditting);	
}

void CVisualEditorDlg::OnOptions() 
{
	CPropertySheet oSheet;

	CPPOptions oOptions;
	oOptions.m_bUseRegistryNotSql = m_oOptions.m_bUseRegistryNotSql;

	oSheet.AddPage(&oOptions);
	oSheet.SetTitle(_T("Options"));

	if(IDCANCEL == oSheet.DoModal())
		return;

	if(m_pIIDServer && m_oOptions.m_bUseRegistryNotSql != oOptions.m_bUseRegistryNotSql)
		m_pIIDServer->ForceRegistryUse(oOptions.m_bUseRegistryNotSql);

	m_oOptions.m_bUseRegistryNotSql = oOptions.m_bUseRegistryNotSql;
	if(!m_oOptions.Save())
		AfxMessageBox(IDS_OPTIONS_SAVE_FAILED);

}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
SOptions::SOptions()
{
	m_bUseRegistryNotSql = FALSE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

const LPCTSTR cstrRegUseRegistry = _T("USE_REGISTRY_INSTEAD_OF_SQL");

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL SOptions::Load()
{
	DWORD dwValue;
	CRegStorage	oRegStorage(CFG_REG_KEY);

	if(oRegStorage.GetValue(cstrRegUseRegistry, dwValue))
		m_bUseRegistryNotSql = dwValue ? TRUE:FALSE;

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL SOptions::Save()
{
	CRegStorage	oRegStorage(CFG_REG_KEY);

	BOOL bRet = TRUE;

	if(!oRegStorage.PutValue(cstrRegUseRegistry, m_bUseRegistryNotSql))
		bRet = FALSE;

	return bRet;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
