// DHexInfo.cpp : implementation file
//

#include "stdafx.h"
#include "iguard.h"
#include "DHexInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void GetHexString(CString* pResult, BYTE *From,DWORD Len)
{
	DWORD i,j,k;
	
	char tmp[1024];
	
	pResult->Empty();
	
	wsprintf(tmp, "%08X\r\n", From);
	*pResult += tmp;
	for(i=0;i<Len;) {
		wsprintf(tmp, "%08X ",i);
		*pResult += tmp;
		for(j=0;j<16 && i<Len;j++,i++)
		{
			wsprintf(tmp, "%c%02X",j && !(j%4)?(!(j%8)?' | ':'-'):' ',From[i]);
			*pResult += tmp;
		}
		for(k=16-j;k!=0;k--)
		{
			wsprintf(tmp, "   ");
			*pResult += tmp;
		}
		*pResult += "  ";
		for(;j!=0;j--)
			if(From[i-j]>=0x20)
			{
				wsprintf(tmp, "%c",From[i-j]);
				*pResult += tmp;
			}
			else
			{
				wsprintf(tmp, ".",From[i-j]);
				*pResult += tmp;
			}
			*pResult += "\r\n";
	}
	*pResult += "\r\n";
}
/////////////////////////////////////////////////////////////////////////////
// CDHexInfo dialog


CDHexInfo::CDHexInfo(CWnd* pParent /*=NULL*/)
: CDialog(CDHexInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDHexInfo)
	m_eHex = _T("");
	//}}AFX_DATA_INIT
	m_ValueAddress = NULL;
	m_DataSize = 0;
}


void CDHexInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDHexInfo)
	DDX_Text(pDX, IDC_EDIT_HEXINFO, m_eHex);
	DDV_MaxChars(pDX, m_eHex, 85535);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDHexInfo, CDialog)
//{{AFX_MSG_MAP(CDHexInfo)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDHexInfo message handlers
void CDHexInfo::SetHexData(PVOID ValueAddress, DWORD Size)
{
	m_ValueAddress = (LPBYTE) ValueAddress;
	m_DataSize = Size;
}

BOOL CDHexInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	LOGFONT logfont;
	char charset[] = "MS Sans Serif";

	logfont.lfHeight = -11;
	logfont.lfWidth = 0;
	logfont.lfEscapement = 0;
	logfont.lfOrientation = 0;
	logfont.lfWeight = 400;
	logfont.lfItalic = 0;
	logfont.lfUnderline = 0;
	logfont.lfStrikeOut = 0;
	logfont.lfCharSet =  RUSSIAN_CHARSET;
	logfont.lfOutPrecision = OUT_STROKE_PRECIS;
	logfont.lfClipPrecision = CLIP_STROKE_PRECIS;
	logfont.lfQuality = PROOF_QUALITY;
	logfont.lfPitchAndFamily = FIXED_PITCH | FF_MODERN; 
	int i = 0;
	while (i < sizeof(charset))
		logfont.lfFaceName[i] = charset[i++];
	while (i < sizeof(logfont.lfFaceName))
		logfont.lfFaceName[i++] = 0;

	m_LocalFont.CreateFontIndirect(&logfont);

	
	SetFont(&m_LocalFont);
	if ((m_ValueAddress != NULL) && (m_DataSize != 0))
	{
		GetHexString(&m_eHex, m_ValueAddress, m_DataSize);
		UpdateData(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
