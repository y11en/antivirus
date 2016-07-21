#if !defined(AFX_APPWIZARDAW_H__BF930727_2735_4CF4_89C1_EC2EF4294DBC__INCLUDED_)
#define AFX_APPWIZARDAW_H__BF930727_2735_4CF4_89C1_EC2EF4294DBC__INCLUDED_

// appwizardaw.h : header file
//

class CDialogChooser;

// All function calls made by mfcapwz.dll to this custom AppWizard (except for
//  GetCustomAppWizClass-- see appwizard.cpp) are through this class.  You may
//  choose to override more of the CCustomAppWiz virtual functions here to
//  further specialize the behavior of this custom AppWizard.
class CAppwizardAppWiz : public CCustomAppWiz
{
public:
	virtual CAppWizStepDlg* Next(CAppWizStepDlg* pDlg);
	virtual CAppWizStepDlg* Back(CAppWizStepDlg* pDlg);
		
	virtual void InitCustomAppWiz();
	virtual void ExitCustomAppWiz();
	virtual void CustomizeProject(IBuildProject* pProject);

	virtual LPCTSTR LoadTemplate(LPCTSTR lpszTemplateName,
		DWORD& rdwSize, HINSTANCE hInstance = NULL);

	virtual void CopyTemplate(LPCTSTR lpszInput, DWORD dwSize, OutputStream* pOutput);
	virtual void ProcessTemplate(LPCTSTR lpszInput, DWORD dwSize, OutputStream* pOutput);

	void SetMacro(LPCTSTR i_strName, LPCTSTR i_strValue);

protected:
	LPCTSTR m_strParseMacro;
	CDialogChooser* m_pChooser;

	BOOL CreateSubProject(IApplication* i_pApp, LPCTSTR i_strRootFolder, BOOL i_bWin9x);
	BOOL AddFiles(IBuildProject* i_pProj, TCHAR** i_arrFiles, int i_nArrSize, LPCTSTR i_strProjFolder);
};

extern CAppwizardAppWiz Appwizardaw;

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_APPWIZARDAW_H__BF930727_2735_4CF4_89C1_EC2EF4294DBC__INCLUDED_)
