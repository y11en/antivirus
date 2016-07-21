// appwizardaw.cpp : implementation file
//

#include "stdafx.h"
#include "appwizard.h"
#include "appwizardaw.h"
#include "chooser.h"
#include "AppCommon.h"
#include "ObjModel\BLDGUID.h"

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This is called immediately after the custom AppWizard is loaded.  Initialize
//  the state of the custom AppWizard here.
void CAppwizardAppWiz::InitCustomAppWiz()
{
	// Create a new dialog chooser; CDialogChooser's constructor initializes
	//  its internal array with pointers to the steps.
	m_pChooser = new CDialogChooser;

	// Set the maximum number of steps.
	SetNumberOfSteps(LAST_DLG);

	// Inform AppWizard that we're making a DLL.
	m_Dictionary[_T("PROJTYPE_DLL")] = _T("1");
}

void CAppwizardAppWiz::ExitCustomAppWiz()
{
	// Deallocate memory used for the dialog chooser
	ASSERT(m_pChooser != NULL);
	delete m_pChooser;
	m_pChooser = NULL;
}

CAppWizStepDlg* CAppwizardAppWiz::Next(CAppWizStepDlg* pDlg)
{
	return m_pChooser->Next(pDlg);
}

CAppWizStepDlg* CAppwizardAppWiz::Back(CAppWizStepDlg* pDlg)
{
	return m_pChooser->Back(pDlg);
}

void CAppwizardAppWiz::CustomizeProject(IBuildProject* pProject)
{
	/*
	CComPtr<IApplication> comApplication;
	HRESULT hr=pProject->get_Application((IDispatch**)&comApplication);
	if(FAILED(hr))
		return;

	CComBSTR bstrPathname;
	pProject->get_FullName(&bstrPathname);

	CString strPathname(bstrPathname);
	CString strPath, strName;
	SplitPathname(strPathname, strPath, strName);

	BOOL bRet;
	bRet = CreateSubProject(comApplication, strPath, TRUE);
	bRet = CreateSubProject(comApplication, strPath, FALSE);
	*/
}

BOOL CAppwizardAppWiz::CreateSubProject(IApplication* i_pApp, LPCTSTR i_strRootFolder, BOOL i_bWin9x)
{
	// ---------------------------------------------------------
	// project folder
	CString strProjPath;

	if(i_bWin9x) strProjPath = CString(i_strRootFolder) + _T("win9x\\");
	else		 strProjPath = CString(i_strRootFolder) + _T("winnt\\");

	CString strRoot = m_Dictionary[_T("Root")];

	// ---------------------------------------------------------
	// project name
	CString strProjName;
	if(i_bWin9x) strProjName.Format(_T("%s9x"), strRoot);
	else		 strProjName.Format(_T("%snt"), strRoot);

	// ---------------------------------------------------------
	// add new sub project 
	HRESULT hr = i_pApp->AddProject(
					CComBSTR(strProjName), 
					CComBSTR(strProjPath), 
					CComBSTR(_T("Application")), 
					-1);

	if(FAILED(hr))
		return FALSE;

	// ---------------------------------------------------------
	// get sub project position

	CComPtr<IProjects> comProjects;
	hr = i_pApp->get_Projects((IDispatch**)&comProjects);
	if(FAILED(hr))
		return FALSE;

	LONG nCount;
	comProjects->get_Count(&nCount);

	// ---------------------------------------------------------
	// get IBuildProject ptr to configure sub project

	CComPtr<IGenericProject> comProject;
	CComVariant varIndex = nCount;

	hr=comProjects->Item(varIndex, &comProject);
	if(FAILED(hr))
		return FALSE;
	
	CComQIPtr<IBuildProject> comBuildProject(comProject);
	if(comBuildProject == NULL)
		return FALSE;

	// ---------------------------------------------------------
	// add files

	TCHAR* arrRootObjects[] = 
	{
		_T("g_precomp.h"),
		_T("handlers.h"),
		_T("handlers.c"),
	};

	TCHAR* arrProjObjects[] = 
	{
		_T("precomp.h"),
		_T("main.c"),
	};

	TCHAR* arrProjObjects9x[] = 
	{
		_T("PreBuild.cmd"),
		_T("PostBuild.cmd"),
		_T("$$root$$.vrc"),
		_T("$$root$$.def"),
	};

	TCHAR* arrProjObjectsNt[] = 
	{
		_T("PostBuild.cmd"),
	};

	// from root
	if(!AddFiles(comBuildProject, arrRootObjects, SIZE_OF(arrRootObjects), i_strRootFolder))
		return FALSE;

	// from sub project root
	if(!AddFiles(comBuildProject, arrProjObjects, SIZE_OF(arrProjObjects), strProjPath))
		return FALSE;

	// platform specific
	if(i_bWin9x)
	{
		if(!AddFiles(comBuildProject, arrProjObjects9x, SIZE_OF(arrProjObjects9x), strProjPath))
			return FALSE;
	}
	else
	{
		if(!AddFiles(comBuildProject, arrProjObjectsNt, SIZE_OF(arrProjObjectsNt), strProjPath))
			return FALSE;
	}

	// ---------------------------------------------------------
	// configure build options

	CComPtr<IConfigurations> pConfigs;
	hr=comBuildProject->get_Configurations(&pConfigs);
	if(FAILED(hr))
		return FALSE;

	long Count=0;
	pConfigs->get_Count(&Count);
	
	for(int i=1; i <= Count; i++)
	{
		CComVariant index = i;
		CComPtr<IConfiguration> pConfig;

		hr=pConfigs->Item(index, &pConfig);
		if(FAILED(hr))
			return FALSE;

		CString text;
		CComBSTR Name;
		VARIANT dummy = {0};
		CString output;

		pConfig->get_Name(&Name);
		text = Name;
		
		if (text.Find("Debug") == -1)
			output = "Release";
		else
			output = "Debug";
		
		text.Format("/out:\"%s/%s.exe\"",output,m_Dictionary["Root"]);
		pConfig->RemoveToolSettings(L"link.exe", text.AllocSysString(), dummy);
		text.Format("/out:\"%s/%s.sys\"",output,m_Dictionary["Root"]);
		pConfig->AddToolSettings(L"link.exe", text.AllocSysString(), dummy);
		
		pConfig->AddToolSettings(L"mfc", L"0", dummy);	//None : 0, Static Libraries : 1, Shared Dll : 2
		pConfig->RemoveToolSettings(L"link.exe", L"/subsystem:windows", dummy);
		pConfig->RemoveToolSettings(L"link.exe", L"/incremental:yes", dummy);
		
		//The NATIVE subsystem applies device drivers for Windows NT.
		pConfig->AddToolSettings(L"link.exe", L"/subsystem:native", dummy);
		pConfig->AddToolSettings(L"link.exe", L"/entry:\"DriverEntry\"", dummy);
		pConfig->AddToolSettings(L"link.exe", L"/driver", dummy);
		pConfig->AddToolSettings(L"link.exe", L"/incremental:no", dummy);
		
		// change the preprocessor definitions
		pConfig->RemoveToolSettings(L"cl.exe", L"/D \"_WINDOWS\"", dummy);
		pConfig->RemoveToolSettings(L"cl.exe", L"/D \"_MBCS\"", dummy);
		pConfig->RemoveToolSettings(L"cl.exe", L"/Od", dummy);
		pConfig->RemoveToolSettings(L"cl.exe", L"/ZI", dummy);	// Program Database for "Edit & Continue" can not be defined when /driver option is defined
		pConfig->RemoveToolSettings(L"cl.exe", L"/GZ", dummy);	//GZ initializes all local variables not explicitly initialized by the program. It fills all memory used by these variables with 0xCC
		pConfig->AddToolSettings(L"cl.exe", L"/Zi", dummy);	// Program Database
		pConfig->AddToolSettings(L"cl.exe", L"/Oi", dummy);	// 
		pConfig->AddToolSettings(L"cl.exe", L"/Gz", dummy);	// __stdcall calling convention
		pConfig->AddToolSettings(L"cl.exe", L"/D \"_X86_\"", dummy);
		pConfig->AddToolSettings(L"cl.exe", L"/D \"i386\"", dummy);
		pConfig->AddToolSettings(L"cl.exe", L"/D \"STD_CALL\"", dummy);
		pConfig->AddToolSettings(L"cl.exe", L"/D \"CONDITION_HANDLING\"", dummy);
		pConfig->AddToolSettings(L"cl.exe", L"/D \"WIN32_LEAN_AND_MEAN\"", dummy);
		pConfig->AddToolSettings(L"cl.exe", L"/D \"NT_UP\"", dummy);
		pConfig->AddToolSettings(L"cl.exe", L"/D \"SRVDBG\"", dummy);
		pConfig->AddToolSettings(L"cl.exe", L"/D \"DBG\"", dummy);
		pConfig->AddToolSettings(L"cl.exe", L"/D \"_IDWBUILD\"", dummy);
		pConfig->AddToolSettings(L"cl.exe", L"/D \"_WIN32_WINNT=0x0400\"", dummy);
		pConfig->AddToolSettings(L"cl.exe", L"/I \"$(ddkroot)\\inc\\ddk\"", dummy);
		pConfig->AddToolSettings(L"cl.exe", L"/I \"$(ddkroot)\\inc\"", dummy);

		//hr = pConfig->AddToolSettings(L"PostBuild_Cmds", L"PostBuild.cmd Debug $(TargetPath) $(TargetName)", dummy);
		//PostBuild_Cmds=PostBuild.cmd Debug $(TargetPath) $(TargetName)
		
		// Change the libraries
		pConfig->RemoveToolSettings(L"link.exe", L"kernel32.lib", dummy);
		pConfig->RemoveToolSettings(L"link.exe", L"user32.lib", dummy);
		pConfig->RemoveToolSettings(L"link.exe", L"gdi32.lib", dummy);
		pConfig->RemoveToolSettings(L"link.exe", L"winspool.lib", dummy);
		pConfig->RemoveToolSettings(L"link.exe", L"comdlg32.lib", dummy);
		pConfig->RemoveToolSettings(L"link.exe", L"advapi32.lib", dummy);
		pConfig->RemoveToolSettings(L"link.exe", L"shell32.lib", dummy);
		pConfig->RemoveToolSettings(L"link.exe", L"ole32.lib", dummy);
		pConfig->RemoveToolSettings(L"link.exe", L"oleaut32.lib", dummy);
		pConfig->RemoveToolSettings(L"link.exe", L"uuid.lib", dummy);
		pConfig->RemoveToolSettings(L"link.exe", L"odbc32.lib", dummy);
		pConfig->RemoveToolSettings(L"link.exe", L"odbccp32.lib", dummy);
		pConfig->AddToolSettings(L"link.exe", L"ntoskrnl.lib", dummy);
		pConfig->AddToolSettings(L"link.exe", L"hal.lib", dummy);
		//		pConfig->AddToolSettings(L"link.exe", L"int64.lib", dummy);
		
		if (output == "Debug")
		{
			pConfig->AddToolSettings(L"link.exe", L"/libpath:\"$(ddkroot)\\libchk\\i386\"", dummy);
			pConfig->AddToolSettings(L"link.exe", L"/libpath:\"$(ddkroot)\\lib\\i386\\checked\"", dummy);
		}
		else
		{
			pConfig->AddToolSettings(L"link.exe", L"/libpath:\"$(ddkroot)\\libfre\\i386\"", dummy);
			pConfig->AddToolSettings(L"link.exe", L"/libpath:\"$(ddkroot)\\lib\\i386\\free\"", dummy);
		}
		
		pConfig=NULL;
	}
	pConfigs=NULL;

	return TRUE;
}

BOOL CAppwizardAppWiz::AddFiles(IBuildProject* i_pProj, TCHAR** i_arrFiles, int i_nArrSize, LPCTSTR i_strProjFolder)
{
	for(int i=0; i<i_nArrSize; i++)
	{
		CString strRoot = m_Dictionary[_T("Root")];
		CString strName(i_arrFiles[i]);
		strName.Replace(_T("$$root$$"), strRoot);

		CString strPathname;
		strPathname.Format(_T("%s%s"), i_strProjFolder, strName);

		CComVariant varIndex = 0;

		HRESULT hr = 
			i_pProj->AddFile(CComBSTR(strPathname),varIndex);

		if(FAILED(hr))
			return FALSE;
	}

	return TRUE;
}

BOOL DoesStringEndOn(LPTSTR i_strStr, LPCTSTR i_strEndsOn)
{
	int nStrLen = lstrlen(i_strStr);
	int nEndLen = lstrlen(i_strEndsOn);

	if(nEndLen >= nStrLen)
		return FALSE;

	i_strStr += (nStrLen-nEndLen);

	if(0 == lstrcmp(i_strStr, i_strEndsOn))
	{
		i_strStr[0] = NULL;
		return TRUE;
	}

	return FALSE;
}

LPCTSTR CAppwizardAppWiz::LoadTemplate(LPCTSTR lpszTemplateName,
	DWORD& rdwSize, HINSTANCE hInstance)
{
	static BOOL s_bFirstCall = TRUE;

	/*
	POSITION pos = m_Dictionary.GetStartPosition();
	while(pos)
	{
		CString strKey, strVal;
		m_Dictionary.GetNextAssoc(pos, strKey, strVal);
		TRACE2(_T("%s %s\n"), strKey, strVal);
	}
	  */
	if(s_bFirstCall)
	{
		s_bFirstCall = FALSE;
	}

	TCHAR strTemplateName[1024];
	lstrcpy(strTemplateName, lpszTemplateName);

	LPCTSTR strEndsOn = NULL;
	LPCTSTR strPlatformNT = _T(":PLATFORM_NT");
	LPCTSTR strPlatform9x = _T(":PLATFORM_9X");

	if(DoesStringEndOn(strTemplateName, strPlatformNT))
		strEndsOn = strPlatformNT;
	else
	if(DoesStringEndOn(strTemplateName, strPlatform9x))
		strEndsOn = strPlatform9x;

	m_strParseMacro = NULL;
	if(strEndsOn)
		m_strParseMacro = strEndsOn+1;

	return CCustomAppWiz::LoadTemplate(strTemplateName, rdwSize, hInstance);
}

void CAppwizardAppWiz::CopyTemplate(LPCTSTR lpszInput, DWORD dwSize, OutputStream* pOutput)
{
	CCustomAppWiz::CopyTemplate(lpszInput, dwSize, pOutput);
}

void CAppwizardAppWiz::ProcessTemplate(LPCTSTR lpszInput, DWORD dwSize, OutputStream* pOutput)
{
	if(m_strParseMacro)
		m_Dictionary[m_strParseMacro] = _T("1");

	CCustomAppWiz::ProcessTemplate(lpszInput, dwSize, pOutput);

	if(m_strParseMacro)
		m_Dictionary[m_strParseMacro] = _T("");
}

void CAppwizardAppWiz::SetMacro(LPCTSTR i_strName, LPCTSTR i_strValue)
{
	m_Dictionary[i_strName] = i_strValue;
}


// Here we define one instance of the CAppwizardAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global Appwizardaw.
CAppwizardAppWiz Appwizardaw;

