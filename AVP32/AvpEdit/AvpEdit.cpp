// AvpEdit.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AvpEdit.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "AvpEditDoc.h"
#include "AvpEditView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <ScanAPI/scanobj.h>
#include <ScanAPI/avp_dll.h>

#include "OptDlg.h"
#include <LoadKeys/key.h>

#include <Sign/sign_lib.h>
#include <Sign/ct_funcs.h>

#include <version/ver_avpedit.h>

CTN_Signature* OpenSgnFile( char* private_file, char* passw, HWND hwnd );

/////////////////////////////////////////////////////////////////////////////
// CAvpEditApp


BEGIN_MESSAGE_MAP(CAvpEditApp, CWinApp)
	//{{AFX_MSG_MAP(CAvpEditApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAvpEditApp construction

CAvpEditApp::CAvpEditApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAvpEditApp object

CAvpEditApp theApp;

COptDlg OptDlg;
CString PathOpen;
CString PathLink;
CString PathAutoFill;
CString PathStart;
CString KeyFile;

CRect WndPos;
CRect ScanWndPos;
CMainFrame* MainFramePtr;
BYTE Authenticity[0x40];

extern DWORD unsqu (BYTE *i_buf,BYTE *o_buf);

CTN_Signature* ctn_sk=NULL;

BOOL SignFile(const char* lpszPathName){
	return sign_file( ctn_sk, lpszPathName, 1 );
}
char* SignGetName()
{
	return (char*)&(ctn_sk->Private.UserName[0]) + 22;
}

BOOL LoadKey(const char* filename)
{

	char Name[512];
	BOOL ret=FALSE;
	CFile f;
	char* p;
	GetFullPathName(AfxGetApp()->m_pszHelpFilePath,512,Name,&p);
	strcpy(p,filename);

	ctn_sk=OpenSgnFile(Name,NULL,NULL);

	if(ctn_sk){
		ret=TRUE;
/*
	if(f.Open(filename,CFile::modeRead) || f.Open(Name,CFile::modeRead)){
		AVP_KeyHeader KeyHeader;
		f.Read(&KeyHeader,sizeof(AVP_KeyHeader));
		if(KeyHeader.Magic!=(DWORD)AVPEDIT_KEY_MAGIC)return FALSE;
		if(KeyHeader.UncompressedSize!=0x40)return FALSE;
		BYTE *cbuf=new BYTE[KeyHeader.CompressedSize];
		f.Read(cbuf,KeyHeader.CompressedSize);
		f.Close();
		if(KeyHeader.CompressedCRC==CalcSum(cbuf,KeyHeader.CompressedSize)){
			for(UINT i=0;i<KeyHeader.CompressedSize;i++) cbuf[i]^=(BYTE)i;
			if(KeyHeader.UncompressedSize==unsqu(cbuf,Authenticity))
				ret=TRUE;
		}
		delete cbuf;
*/
	}else{
//bad:
		char s[0x200];
		sprintf(s,"Secret Key (%s) absent or damaged.\n\n\
Use SIGNER.EXE or DTOOL.EXE to make your secret/public key pair.\n\
Send public key (DTOOL.PUB) to Kaspersky Lab (graf@avp.ru)\n\
with vendor registration request.",Name);

		AfxMessageBox(s);
	}
	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// CAvpEditApp initialization

BOOL CAvpEditApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

/*
	if(!DATA_Init_Library(malloc,free)){
		AfxMessageBox("DATA_Init_Library failed.");
		return FALSE;
	}
*/
	LoadStdProfileSettings(30);  // Load standard INI file options (including MRU)
/*
	LoadLibrary("Method.dll");
	LoadLibrary("AvpBase.dll");
*/
	GetCurrentDirectory(512,PathStart.GetBuffer(512));
	PathStart.ReleaseBuffer();


	KeyFile = GetProfileString("Base","SecretKeyFile","DTOOL.SGN");
	OptDlg.m_SetFile = GetProfileString("Base","SetName","AVP.SET");
	OptDlg.m_SepLine = GetProfileString("Options","SepLine","••••••••••••");
	OptDlg.m_FontName = GetProfileString("Options","FontName","");
	OptDlg.m_FontSize = GetProfileInt("Options","FontSize",100);

#if (defined (KL_MULTIPLATFORM_LINKS))
	OptDlg.m_LocalCVSRoot = GetProfileString("Options","LocalCVSRoot","");

    if (OptDlg.m_LocalCVSRoot.IsEmpty())
    {
        AfxMessageBox ("Local CVS Root is unknown. To set it up, go to View --> Options ");
    }
#endif

	PathOpen = GetProfileString("Options","PathOpen","");
	PathLink = GetProfileString("Options","PathLink","");
	PathAutoFill = GetProfileString("Options","PathAutoFill","");
	if(PathOpen.Right(1)=="\\")PathOpen+="*.ave";
	if(PathLink.Right(1)=="\\")PathLink+="*.*";
	if(PathAutoFill.Right(1)=="\\")PathAutoFill+="*.*";

	OptDlg.m_SkipPag = GetProfileInt("Options","SkipPag",1);
	OptDlg.m_SaveBackup = GetProfileInt("Options","SaveBackup",1);
	OptDlg.m_EscSaveRec = GetProfileInt("Options","EscSaveRec",1);
	OptDlg.m_LinkConfirm = GetProfileInt("Options","LinkConfirm",1);
	OptDlg.m_ShowSourceFile = GetProfileInt("Options","ShowSourceFile",1);
	OptDlg.m_MakeFullName = GetProfileInt("Options","MakeFullName",1);
	WndPos.left = GetProfileInt("Options","PosLeft",-1);
	WndPos.right = GetProfileInt("Options","PosRight",-1);
	WndPos.top = GetProfileInt("Options","PosTop",-1);
	WndPos.bottom = GetProfileInt("Options","PosBottom",-1);

	ScanWndPos.left = GetProfileInt("ScanDialog","PosLeft",-1);
	ScanWndPos.top = GetProfileInt("ScanDialog","PosTop",-1);

	if(!LoadKey(KeyFile)) return FALSE;

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_AVPEDTYPE,
		RUNTIME_CLASS(CAvpEditDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CAvpEditView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	MainFramePtr = pMainFrame;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if(cmdInfo.m_nShellCommand==CCommandLineInfo::FileOpen)
	{
		char* clp=this->m_lpCmdLine;
		int l=strlen(clp);
		BOOL quote=0;
		int arg=-1;

		for( int i=0; i<=l; i++)
		{
			switch(clp[i]){
			case 0:
			case 0x9:
			case ' ':
				if(!quote && arg!=-1){
					CString	s = clp+arg;
					cmdInfo.m_strFileName = s.Left(i-arg);
					ProcessShellCommand(cmdInfo);
					arg=-1;
				}
				break;
			case '"':
				if(quote) quote=0;
				else quote=1;
			default:
				if(arg==-1)
					arg=i;
				break;
			}
		}
	}
	else if(cmdInfo.m_nShellCommand==CCommandLineInfo::FilePrint)
	{
		CStdioFile f;
		if(f.Open(cmdInfo.m_strFileName,CFile::modeRead))
		{
			int i= 1 + cmdInfo.m_strFileName.ReverseFind('\\');
			CString path= i?cmdInfo.m_strFileName.Left(i):"";
			CString s;
			cmdInfo.m_nShellCommand=CCommandLineInfo::FileOpen;
			while(f.ReadString(s))
			{
				s.TrimLeft();
				if(s.GetLength() && s[0]!=';')
				{
					cmdInfo.m_strFileName=path + s;
					ProcessShellCommand(cmdInfo);
				}
			}
			f.Close();
		}
	}

	
	// The main window has been initialized, so show and update it.
	if(WndPos.left!=-1 && (WndPos.bottom-WndPos.top)>0x40){
		pMainFrame->SetWindowPos(0,
			WndPos.left,WndPos.top,WndPos.right-WndPos.left,WndPos.bottom-WndPos.top,
			SWP_NOREDRAW|SWP_NOACTIVATE|SWP_NOZORDER);
	}
	if(ScanWndPos.left!=-1){
		pMainFrame->ScanDialog.SetWindowPos(0,
			ScanWndPos.left,ScanWndPos.top,0,0,
			SWP_NOREDRAW|SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOSIZE);
	}
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CAvpEditApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CAvpEditApp commands

int CAvpEditApp::ExitInstance() 
{
	WriteProfileString("Base","SecretKeyFile",KeyFile);
	WriteProfileString("Base","SetName",OptDlg.m_SetFile);
	WriteProfileString("Options","SepLine",OptDlg.m_SepLine);
	WriteProfileString("Options","PathOpen",PathOpen);
	WriteProfileString("Options","PathLink",PathLink);
	WriteProfileString("Options","PathAutoFill",PathAutoFill);
	WriteProfileString("Options","FontName",OptDlg.m_FontName);
	WriteProfileInt("Options","FontSize",OptDlg.m_FontSize);

#if (defined (KL_MULTIPLATFORM_LINKS))
    WriteProfileString("Options","LocalCVSRoot",OptDlg.m_LocalCVSRoot);
#endif

	WriteProfileInt("Options","SkipPag",OptDlg.m_SkipPag);
	WriteProfileInt("Options","SaveBackup",OptDlg.m_SaveBackup);
	WriteProfileInt("Options","EscSaveRec",OptDlg.m_EscSaveRec);
	WriteProfileInt("Options","LinkConfirm",OptDlg.m_LinkConfirm);
	WriteProfileInt("Options","ShowSourceFile",OptDlg.m_ShowSourceFile);
	WriteProfileInt("Options","MakeFullName",OptDlg.m_MakeFullName);
	WriteProfileInt("Options","PosLeft",WndPos.left);
	WriteProfileInt("Options","PosRight",WndPos.right);
	WriteProfileInt("Options","PosTop",WndPos.top);
	WriteProfileInt("Options","PosBottom",WndPos.bottom);

	WriteProfileInt("ScanDialog","PosLeft",ScanWndPos.left);
	WriteProfileInt("ScanDialog","PosTop",ScanWndPos.top);
	
	if(ctn_sk){ CTN_SignDestructor(ctn_sk); ctn_sk=NULL; }
	AVP_Destroy();

	return CWinApp::ExitInstance();
}

void CAvpEditApp::OnFileOpen() 
{
	CString name=PathOpen;

	CFileDialog cfd( TRUE ,"ave", NULL,
		OFN_FILEMUSTEXIST
		|OFN_PATHMUSTEXIST
		|OFN_EXPLORER
		|OFN_READONLY
		|OFN_HIDEREADONLY
		|OFN_NOCHANGEDIR, 
		"AVP Editor files (*.ave)|*.ave||",MainFramePtr);

	cfd.m_ofn.lpstrTitle="Open antiviral database"; 
//	cfd.m_ofn.lpstrInitialDir=name;
	cfd.m_ofn.lpstrFile=name.GetBuffer(0x200);
	cfd.m_ofn.nMaxFile=0x200;

	int ret=cfd.DoModal();

	name.ReleaseBuffer();
	if(ret==IDOK){
		PathOpen=name;
		AddToRecentFileList(PathOpen);
		OpenDocumentFile( PathOpen );
	}
//	CWinApp::OnFileOpen();
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	GetDlgItem(IDC_COPYRIGHT)->SetWindowText(VER_LEGALCOPYRIGHT_STR "\n" VER_PORTIONCOPYRIGHT_STR VER_LC_STR);
	GetDlgItem(IDC_PRODUCTNAME)->SetWindowText(VER_PRODUCTNAME_STR);
	GetDlgItem(IDC_VERSION)->SetWindowText("version " VER_PRODUCTVERSION_STR);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
