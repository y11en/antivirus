// Dparameter.cpp : implementation file
//

#include "stdafx.h"
#include "iguard.h"
#include "DParameter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDParameter dialog


CDParameter::CDParameter(PFILTER_PARAM pSingleParam, CWnd* pParent /*=NULL*/)
	: CDialog(CDParameter::IDD, pParent)
{
	m_pSingleParam = pSingleParam;
	m_pSingleParamWork = (PFILTER_PARAM) m_pFilterBuffer;
	
	if (pSingleParam != NULL)
	{
		CopyMemory(m_pSingleParamWork, pSingleParam, sizeof(FILTER_PARAM) + pSingleParam->m_ParamSize);
	}
	else
		ZeroMemory(m_pSingleParamWork, FILTER_BUFFER_SIZE);

	//{{AFX_DATA_INIT(CDParameter)
	m_eByte = 0;
	m_eOffsetHi = 0;
	m_eOffsetLo = 0;
	//}}AFX_DATA_INIT
	m_eValue = _T("*");
	m_eWord = 0;

	m_chCachable = (m_pSingleParamWork->m_ParamFlags & _FILTER_PARAM_FLAG_CACHABLE) != 0 ? TRUE : FALSE;
	m_chCalcupReg = (m_pSingleParamWork->m_ParamFlags & _FILTER_PARAM_FLAG_CACHEUPREG) != 0 ? TRUE : FALSE;
	m_chInverseOp = (m_pSingleParamWork->m_ParamFlags & _FILTER_PARAM_FLAG_INVERS_OP) != 0 ? TRUE : FALSE;
	m_chMustExist = (m_pSingleParamWork->m_ParamFlags & _FILTER_PARAM_FLAG_MUSTEXIST) != 0 ? TRUE : FALSE;

	m_chLowerCase = TRUE;
}


void CDParameter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDParameter)
	DDX_Control(pDX, IDC_IPADDRESS4, m_IP4);
	DDX_Control(pDX, IDC_COMBO_OP_TYPE, m_cbOperationType);
	DDX_Control(pDX, IDC_COMBO_PARAMETER_ID, m_cbParameterID);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_eValue);
	DDV_MaxChars(pDX, m_eValue, 255);
	DDX_Check(pDX, IDC_CHECK_CACHABLE, m_chCachable);
	DDX_Check(pDX, IDC_CHECK_CALC_CACHID_UPREG, m_chCalcupReg);
	DDX_Check(pDX, IDC_CHECK_INVERSE_OP, m_chInverseOp);
	DDX_Check(pDX, IDC_CHECK_LOWERCASE, m_chLowerCase);
	DDX_Text(pDX, IDC_EDIT_WORD, m_eWord);
	DDV_MinMaxDWord(pDX, m_eWord, 0, 65535);
	DDX_Check(pDX, IDC_CHECK_MUST_EXIST, m_chMustExist);
	DDX_Text(pDX, IDC_EDIT_BYTE, m_eByte);
	DDV_MinMaxByte(pDX, m_eByte, 0, 255);
	DDX_Text(pDX, IDC_EDIT_OFFSET_HI, m_eOffsetHi);
	DDV_MinMaxUInt(pDX, m_eOffsetHi, 0, 4294967295);
	DDX_Text(pDX, IDC_EDIT_OFFSET_LO, m_eOffsetLo);
	DDV_MinMaxUInt(pDX, m_eOffsetLo, 0, 4294967295);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDParameter, CDialog)
	//{{AFX_MSG_MAP(CDParameter)
	ON_CBN_SELCHANGE(IDC_COMBO_PARAMETER_ID, OnSelchangeComboParameterId)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDParameter message handlers

BOOL CDParameter::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	AddParam("Url (object name)", _PARAM_OBJECT_URL);
	AddParam("Url unicode (object name)", _PARAM_OBJECT_URL_W);
	AddParam("Binary data", _PARAM_OBJECT_BINARYDATA);
	AddParam("Url (destination name)", _PARAM_OBJECT_URL_DEST);
	AddParam("Url unicode (destination name)", _PARAM_OBJECT_URL_DEST_W);
	AddParam("Access attributes", _PARAM_OBJECT_ACCESSATTR);
	AddParam("Object's datalen", _PARAM_OBJECT_DATALEN);
	AddParam("Sector", _PARAM_OBJECT_SECTOR);
	AddParam("Original packet", _PARAM_OBJECT_ORIGINAL_PACKET);
	AddParam("Security identificator", _PARAM_OBJECT_SID);
	AddParam("Offest (in bytes)", _PARAM_OBJECT_BYTEOFFSET);

	AddParam("Source ID", _PARAM_OBJECT_SOURCE_ID);
	AddParam("Destination ID", _PARAM_OBJECT_DEST_ID);

	AddParam("Object handle", _PARAM_OBJECT_OBJECTHANDLE);
	AddParam("ClientID1", _PARAM_OBJECT_CLIENTID1);
	AddParam("ClientID2", _PARAM_OBJECT_CLIENTID2);

	AddParam("IP_TYPE_OF_SERVICE", ID_IP_TYPE_OF_SERVICE);
	AddParam("IP_PROTOCOL",ID_IP_PROTOCOL);
	
	AddParam("IP_SRC_ADDRESS", ID_IP_SRC_ADDRESS);
	AddParam("IP_DST_ADDRESS", ID_IP_DST_ADDRESS);
	AddParam("TCP_SRC_PORT", ID_TCP_SRC_PORT);
	AddParam("TCP_DST_PORT", ID_TCP_DST_PORT);
	AddParam("TCP_SEQ_NUMBER", ID_TCP_SEQ_NUMBER);
	
	AddParam("TCP_URG", ID_TCP_URG);
	AddParam("TCP_ACK", ID_TCP_ACK);
	AddParam("TCP_PSH", ID_TCP_PSH);
	AddParam("TCP_RST", ID_TCP_RST);
	AddParam("TCP_SYN", ID_TCP_SYN);
	AddParam("TCP_FIN", ID_TCP_FIN);
	AddParam("TCP_WIN", ID_TCP_WIN);
	
	AddParam("UDP_SRC_PORT", ID_UDP_SRC_PORT);
	AddParam("UDP_DST_PORT", ID_UDP_DST_PORT);
	
	AddParam("ICMP_TYPE", ID_ICMP_TYPE);
	AddParam("ICMP_CODE", ID_ICMP_CODE);

//	AddParam("", );
			

	m_cbParameterID.SetCurSel(0);

	if (g_FuncLevel < 20)
	{
		GetDlgItem(IDC_CHECK_CACHABLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CALC_CACHID_UPREG)->EnableWindow(FALSE);
	}

	static RULE rules[] = {
		// Action    Act-on							Relative-to				Offset
		// ------    ------							-----------				------
		{lMOVE,		lLEFT(IDC_EDIT_VALUE),		lLEFT(lPARENT)				, 3},
		{lSTRETCH,	lRIGHT(IDC_EDIT_VALUE),		lRIGHT(lPARENT)				,-3},
		
		{lMOVE,		lLEFT(IDC_EDIT_WORD),		lLEFT(lPARENT)				, 3},
		{lMOVE,		lLEFT(IDC_IPADDRESS4),		lLEFT(lPARENT)				, 3},
		{lMOVE,		lLEFT(IDC_EDIT_BYTE),		lLEFT(lPARENT)				, 3},
		
		{lMOVE,		lLEFT(IDC_EDIT_OFFSET_HI),	lLEFT(lPARENT)				, 3},
		{lMOVE,		lLEFT(IDC_EDIT_OFFSET_LO),	lRIGHT(IDC_EDIT_OFFSET_HI)	, 3},
		
		{lEND}
	};
	Layout_ComputeLayout(m_hWnd, rules);	
	
	SwitchParamInput(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDParameter::SwitchParamInput(BOOL bNew)
{
	UpdateData();
	
	m_cbOperationType.ResetContent();

	m_pSingleParamWork->m_ParamID = m_cbParameterID.GetItemData(m_cbParameterID.GetCurSel());	

	CHAR pName[4096];
	CHAR pSingleDescr[4096];
	DWORD dwType;
	
	GetDlgItem(IDC_EDIT_VALUE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_WORD)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_BYTE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_OFFSET_HI)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_OFFSET_LO)->ShowWindow(SW_HIDE);
	
	m_IP4.ShowWindow(SW_HIDE);
	
	GetDlgItem(IDC_CHECK_LOWERCASE)->ShowWindow(SW_HIDE);

	GetSingleParameterInfo(m_pSingleParamWork, pName, pSingleDescr, &dwType);

	switch(dwType)
	{
	case _PARAM_STR_:
	case _PARAM_STR_W:
		GetDlgItem(IDC_EDIT_VALUE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK_LOWERCASE)->ShowWindow(SW_SHOW);
		if (bNew) m_eValue = "*";
		else m_eValue = m_pSingleParamWork->m_ParamValue;

		AddOp("WILDCARD", 0);
		AddOp("WILDCARDSENS", 1);
		break;
	case _PARAM_BYTE_:
		GetDlgItem(IDC_EDIT_BYTE)->ShowWindow(SW_SHOW);
		if (bNew) m_eByte = 0;
		else m_eByte = *(BYTE*) m_pSingleParamWork->m_ParamValue;
		break;
	case _PARAM_DWORD_:
		GetDlgItem(IDC_EDIT_OFFSET_LO)->ShowWindow(SW_SHOW);
		if (bNew) m_eOffsetLo = 0;
		else m_eOffsetLo = *(DWORD*) m_pSingleParamWork->m_ParamValue;
		break;
	case _PARAM_WORD_:
		GetDlgItem(IDC_EDIT_WORD)->ShowWindow(SW_SHOW);
		if (bNew) m_eWord = 0;
		else m_eWord = *(WORD*) m_pSingleParamWork->m_ParamValue;
		break;
	case _PARAM_IP4_ADDRESS:
		m_IP4.ShowWindow(SW_SHOW);
		if (bNew) m_IP4.SetAddress(255, 255, 255, 255);
		else 
		{
			BYTE *pByte = (BYTE*) m_pSingleParamWork->m_ParamValue;
			m_IP4.SetAddress(pByte[3], pByte[2], pByte[1], pByte[0]);
		}
		break;
	case _PARAM_QDWORD_:
		GetDlgItem(IDC_EDIT_OFFSET_HI)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_OFFSET_LO)->ShowWindow(SW_SHOW);
		if (bNew) 
		{
			m_eOffsetHi = 0;
			m_eOffsetLo = 0;
		}
		break;
	}
	
	AddOp("EUA", 2);
	AddOp("AND", 3);
	AddOp("ABV", 4);
	AddOp("BEL", 5);
	AddOp("More", 7);
	AddOp("Less", 8);
	AddOp("NOP", 6);
	m_cbOperationType.SetCurSel(0);

	UpdateData(FALSE);
}

void CDParameter::AddParam(PCHAR ParamName, DWORD ParamID)
{
	CString strtmp;
	strtmp.Format("%s (%d)", ParamName, ParamID);
	int Idx = m_cbParameterID.AddString(strtmp);
	if ((Idx != CB_ERR) && (Idx != CB_ERRSPACE))
		m_cbParameterID.SetItemData(Idx, ParamID);
}

void CDParameter::AddOp(PCHAR OpName, DWORD ParamID)
{
	int Idx = m_cbOperationType.AddString(OpName);
	if ((Idx != CB_ERR) && (Idx != CB_ERRSPACE))
		m_cbOperationType.SetItemData(Idx, ParamID);
}

void CDParameter::OnSelchangeComboParameterId() 
{
	// TODO: Add your control notification handler code here
	SwitchParamInput(TRUE);
}

// -----------------------------------------------------------------------------------------
void CDParameter::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	
	CHAR pName[4096];
	CHAR pSingleDescr[4096];
	DWORD dwType;
	
	GetSingleParameterInfo(m_pSingleParamWork, pName, pSingleDescr, &dwType);
	
	m_pSingleParamWork->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
	
	if (m_chCachable) m_pSingleParamWork->m_ParamFlags |= _FILTER_PARAM_FLAG_CACHABLE;
	if (m_chCalcupReg) m_pSingleParamWork->m_ParamFlags |= _FILTER_PARAM_FLAG_CACHEUPREG;
	if (m_chInverseOp) m_pSingleParamWork->m_ParamFlags |= _FILTER_PARAM_FLAG_INVERS_OP;
	if (m_chMustExist) m_pSingleParamWork->m_ParamFlags |= _FILTER_PARAM_FLAG_MUSTEXIST;
		
	m_pSingleParamWork->m_ParamSize = 0;
	
	switch(dwType)
	{
	case _PARAM_BYTE_:
		m_pSingleParamWork->m_ParamSize = sizeof(BYTE);
		CopyMemory(m_pSingleParamWork->m_ParamValue, &m_eByte, m_pSingleParamWork->m_ParamSize);
		break;
	case _PARAM_STR_:
		m_pSingleParamWork->m_ParamSize = m_eValue.GetLength() + 1;
		CopyMemory(m_pSingleParamWork->m_ParamValue, (LPSTR)(LPCSTR) m_eValue, m_pSingleParamWork->m_ParamSize);
		break;
	case _PARAM_DWORD_:
		m_pSingleParamWork->m_ParamSize = sizeof(DWORD);
		CopyMemory(m_pSingleParamWork->m_ParamValue, &m_eOffsetLo, m_pSingleParamWork->m_ParamSize);
		break;
	case _PARAM_WORD_:
		m_pSingleParamWork->m_ParamSize = sizeof(WORD);
		CopyMemory(m_pSingleParamWork->m_ParamValue, &m_eWord, m_pSingleParamWork->m_ParamSize);
		break;
	case _PARAM_STR_W:
		if (_plstrlenW != NULL)
		{
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_eValue, -1, (PWCHAR) m_pSingleParamWork->m_ParamValue, FILTER_BUFFER_SIZE);
			m_pSingleParamWork->m_ParamSize = (lstrlenW((PWCHAR)m_pSingleParamWork->m_ParamValue) + 1) * sizeof(WCHAR);
		}
		else
		{
			MessageBox("Sorry - unicode inaccessible", "Warning", MB_ICONINFORMATION);
			return;
		}
		break;
	case _PARAM_IP4_ADDRESS:
		{
			DWORD dwAddress;
			BYTE *pByteAddress = (BYTE*) &dwAddress;
			BYTE *pByte = (BYTE*) m_pSingleParamWork->m_ParamValue;
			m_IP4.GetAddress(dwAddress);
			pByte[0] = pByteAddress[0];
			pByte[1] = pByteAddress[1];
			pByte[2] = pByteAddress[2];
			pByte[3] = pByteAddress[3];
		}
		m_pSingleParamWork->m_ParamSize = 4;
		break;
	case _PARAM_QDWORD_:
		{
			PLARGE_INTEGER pli = (PLARGE_INTEGER) m_pSingleParamWork->m_ParamValue;
			pli->HighPart = m_eOffsetHi;
			pli->LowPart = m_eOffsetLo;
		}
		m_pSingleParamWork->m_ParamSize = sizeof(LARGE_INTEGER);
		break;
	}
	
	m_pSingleParamWork->m_ParamID = m_cbParameterID.GetItemData(m_cbParameterID.GetCurSel());
	m_pSingleParamWork->m_ParamFlt = (FLT_PARAM_OPERATION) (m_cbOperationType.GetItemData(m_cbOperationType.GetCurSel()));
	
	if (m_chLowerCase == TRUE && (dwType & _PARAM_STR_))
		CharLowerBuff((CHAR*) m_pSingleParamWork->m_ParamValue, m_pSingleParamWork->m_ParamSize);
	
	CDialog::OnOK();
}
