// DAuditWizard.cpp : implementation file
//

#include "stdafx.h"
#include "avpgcfg.h"
#include "DAuditWizard.h"
#include "EnumProcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <hook\IDriver.h>

////////////////////////////////////////////////////////////////////////////
// CDAuditWizard dialog


CDAuditWizard::CDAuditWizard(CWnd* pParent /*=NULL*/)
	: CDialog(CDAuditWizard::IDD, pParent)
{
	char cd[MAX_PATH];
	GetCurrentDirectory(sizeof(cd), cd);
	if (cd[lstrlen(cd) - 1] != '\\')
		lstrcat(cd, "\\");
	m_eLogFilename.Format("%sOSA-tmp.log", cd);
	//{{AFX_DATA_INIT(CDAuditWizard)
	m_fo_close = FALSE;
	m_fo_create = FALSE;
	m_fo_read = FALSE;
	m_fo_write = FALSE;
	m_reg_create = FALSE;
	m_reg_open = FALSE;
	m_reg_query = FALSE;
	m_reg_setval = FALSE;
	m_fo_mask = _T("*");
	m_reg_mask = _T("*");
	m_ProcName = _T("");
	//}}AFX_DATA_INIT
}


void CDAuditWizard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDAuditWizard)
	DDX_Text(pDX, IDC_EDIT_LOGFILENAME, m_eLogFilename);
	DDV_MaxChars(pDX, m_eLogFilename, 255);
	DDX_Check(pDX, IDC_CHECK_FO_CLOSE, m_fo_close);
	DDX_Check(pDX, IDC_CHECK_FO_CREATE, m_fo_create);
	DDX_Check(pDX, IDC_CHECK_FO_READ, m_fo_read);
	DDX_Check(pDX, IDC_CHECK_FO_WRITE, m_fo_write);
	DDX_Check(pDX, IDC_CHECK_REG_CREATE, m_reg_create);
	DDX_Check(pDX, IDC_CHECK_REG_OPEN, m_reg_open);
	DDX_Check(pDX, IDC_CHECK_REG_QUERY, m_reg_query);
	DDX_Check(pDX, IDC_CHECK_REG_SETVALUE, m_reg_setval);
	DDX_Text(pDX, IDC_EDIT_FO_MASK, m_fo_mask);
	DDV_MaxChars(pDX, m_fo_mask, 255);
	DDX_Text(pDX, IDC_EDIT_REG_MASK, m_reg_mask);
	DDV_MaxChars(pDX, m_reg_mask, 255);
	DDX_CBString(pDX, IDC_COMBO_PROCESS, m_ProcName);
	DDV_MaxChars(pDX, m_ProcName, 32);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDAuditWizard, CDialog)
	//{{AFX_MSG_MAP(CDAuditWizard)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDAuditWizard message handlers

BOOL CDAuditWizard::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CComboBox* pcbProcName;

	pcbProcName = (CComboBox*) GetDlgItem(IDC_COMBO_PROCESS);
	if (pcbProcName != NULL)
	{
		CStringList pr_list;
		CEnumProcess Enum(&pr_list);
		Enum.EnumProcesses();
		pr_list.AddTail("*");
		
		POSITION pos;

		
		for(pos = pr_list.GetHeadPosition(); pos != NULL;)
		{
			pcbProcName->AddString(pr_list.GetNext(pos));
		}
		pcbProcName->SetCurSel(0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDAuditWizard::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	APP_REGISTRATION tmpAppReg;
	
	char ProcName[MAX_PATH];
	char fo_mask[MAX_PATH];
	char ro_mask[MAX_PATH];
	HANDLE hDevice = NULL;
	HANDLE hWhistleup = NULL;
	HANDLE hWFChanged = NULL;
	char WhistleupName[MAX_PATH];
	char WFChangedName[MAX_PATH];

	tmpAppReg.m_CurProcId = GetCurrentProcessId();
	tmpAppReg.m_AppID = AVPG_Driver_Specific;
	tmpAppReg.m_Priority = AVPG_INFOPRIORITY;
	
	lstrcpy(tmpAppReg.m_LogFileName, m_eLogFilename);
	DbgPrint(1, "new log name %s", tmpAppReg.m_LogFileName);

	tmpAppReg.m_ClientFlags = _CLIENT_FLAG_NONE;
	tmpAppReg.m_BlueScreenTimeout = DEADLOCKWDOG_TIMEOUT;
	
	wsprintf(ProcName, "%s", m_ProcName);
	wsprintf(fo_mask, "%s", m_fo_mask);
	wsprintf(ro_mask, "%s", m_reg_mask);
	
	wsprintf(WhistleupName, "OASWhistleup%d_%d", tmpAppReg.m_AppID, GetCurrentThreadId());
	wsprintf(WFChangedName, "OADWFChanged%d_%d", tmpAppReg.m_AppID, GetCurrentThreadId());
	
	hDevice = RegisterApp(&tmpAppReg, &g_OSVer, &(hWhistleup), &(hWFChanged), WhistleupName, WFChangedName);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		DbgPrint(1, "FSDrvLib - init failed!\n");
		MessageBox("Sorry, but i can't init data for audit", "Error", MB_ICONINFORMATION);
	}
	else
	{
		switch (g_OSVer.dwPlatformId)
		{
		case VER_PLATFORM_WIN32_NT:

			// -----------------------------------------------------------------------------------------
			// file operations
			if (m_fo_create)	//open
			{
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_NFIOR, IRP_MJ_CREATE, 0, 0, PreProcessing, NULL);
			}

			if (m_fo_write)		// write
			{
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_NFIOR, IRP_MJ_WRITE, IRP_MN_NORMAL, 0, PreProcessing, NULL);
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_FIOR, FastIoWrite, 0, 0, PreProcessing, NULL);
			}
			
			if (m_fo_close)		// close
			{
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_NFIOR, IRP_MJ_CLEANUP, 0, 0, PreProcessing, NULL);
			}

			if (m_fo_read)		// read
			{
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_NFIOR, IRP_MJ_READ, IRP_MN_NORMAL, 0, PreProcessing, NULL);
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_FIOR, FastIoRead, 0, 0, PreProcessing, NULL);
			}
			break;
		default:
			if (m_fo_create)	//open
			{
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_IFS, IFSFN_OPEN, 0, 0, PreProcessing, NULL);
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_IFS, IFSFN_OPEN, 1, 0, PreProcessing, NULL);
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_IFS, IFSFN_OPEN, 2, 0, PreProcessing, NULL);
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_IFS, IFSFN_OPEN, 3, 0, PreProcessing, NULL);

				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_I21, 0x0f, 0, 0, PreProcessing, NULL);
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_I21, 0x16, 0, 0, PreProcessing, NULL);
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_I21, 0x3c, 0, 0, PreProcessing, NULL);
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_I21, 0x3d, 0, 0, PreProcessing, NULL);
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_I21, 0x5a, 0, 0, PreProcessing, NULL);
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_I21, 0x5b, 0, 0, PreProcessing, NULL);
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_I21, 0x6c, 0, 0, PreProcessing, NULL);
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_I21, 0x71, 0, 0, PreProcessing, NULL);
			}
			if (m_fo_write)		// write
			{
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_IFS, IFSFN_WRITE, 0, 0, PreProcessing, NULL);
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_I21, 0x40, 0, 0, PreProcessing, NULL);
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_I21, 0x28, 0, 0, PreProcessing, NULL);
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_I21, 0x22, 0, 0, PreProcessing, NULL);
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_I21, 0x15, 0, 0, PreProcessing, NULL);
			}
			
			if (m_fo_close)		// close
			{
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_IFS, IFSFN_CLOSE, 2, 0, PreProcessing, NULL);
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_I21, 0x10, 0, 0, PreProcessing, NULL);
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_I21, 0x3e, 0, 0, PreProcessing, NULL);
			}

			if (m_fo_read)		// read
			{
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_IFS, IFSFN_READ, 0, 0, PreProcessing, NULL);
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_I21, 0x3f, 0, 0, PreProcessing, NULL);
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_I21, 0x21, 0, 0, PreProcessing, NULL);
				AddFSFilter(hDevice, tmpAppReg.m_AppID, fo_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_I21, 0x27, 0, 0, PreProcessing, NULL);
			}
			break;
		}
		// -----------------------------------------------------------------------------------------
		// regestry

		if (m_reg_create)
		{
			AddFSFilter(hDevice, tmpAppReg.m_AppID, ro_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_REGS, CreateKey, 0, 0, PreProcessing, NULL);
		}
		if (m_reg_open)
		{
			AddFSFilter(hDevice, tmpAppReg.m_AppID, ro_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_REGS, OpenKey, 0, 0, PreProcessing, NULL);
			AddFSFilter(hDevice, tmpAppReg.m_AppID, ro_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_REGS, QueryKey, 0, 0, PreProcessing, NULL);
		}
		if (m_reg_query)
		{
			AddFSFilter(hDevice, tmpAppReg.m_AppID, ro_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_REGS, QueryMultipleValueKey, 0, 0, PreProcessing, NULL);
			AddFSFilter(hDevice, tmpAppReg.m_AppID, ro_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_REGS, QueryValueKey, 0, 0, PreProcessing, NULL);
		}
		if (m_reg_setval)
		{
			AddFSFilter(hDevice, tmpAppReg.m_AppID, ro_mask, ProcName, 1, FLT_A_LOG, FLTTYPE_REGS, SetValueKey, 0, 0, PreProcessing, NULL);
		}


		APPSTATE_REQUEST AppRequest;
		APPSTATE AppState;

		AppRequest = _AS_GoActive;
		IDriverState(hDevice, tmpAppReg.m_AppID, AppRequest, &AppState);

		UnRegisterApp(hDevice, &tmpAppReg, TRUE, &g_OSVer);
		CloseHandle(hDevice);
	}

	CDialog::OnOK();
}
