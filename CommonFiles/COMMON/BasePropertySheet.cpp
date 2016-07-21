// BasePropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "common.h"
#include "BasePropertySheet.h"

#if _MSC_VER >= 1300
	#include <..\src\mfc\afximpl.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBasePropertySheet

IMPLEMENT_DYNAMIC(CBasePropertySheet, CPropertySheet)

CBasePropertySheet::CBasePropertySheet()
{
}

CBasePropertySheet::CBasePropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CBasePropertySheet::CBasePropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CBasePropertySheet::~CBasePropertySheet()
{
}


BEGIN_MESSAGE_MAP(CBasePropertySheet, CPropertySheet)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBasePropertySheet message handlers

/*
 *	ниже идет хак MFC, чтобы она не меняла фонт у CPropertySheet
 */

// класс для доступа к защищенным членам CPropertyPage
class _CCPropertyPage : public CPropertyPage
{
	friend void CPropertyPage_PreProcessPageTemplate(_CCPropertyPage* pPage, PROPSHEETPAGE& psp, BOOL bWizard);
	friend class CBasePropertySheet;
};

static void CPropertyPage_PreProcessPageTemplate(_CCPropertyPage* pPage, PROPSHEETPAGE& psp, BOOL bWizard)
{
	const DLGTEMPLATE* pTemplate;

	if (psp.dwFlags & PSP_DLGINDIRECT)
	{
		pTemplate = psp.pResource;
	}
	else
	{
		HRSRC hResource = ::FindResource(psp.hInstance,
			psp.pszTemplate, RT_DIALOG);
		HGLOBAL hTemplate = LoadResource(psp.hInstance,
			hResource);
		pTemplate = (LPCDLGTEMPLATE)LockResource(hTemplate);
	}

	ASSERT(pTemplate != NULL);

#ifdef _DEBUG
	// WINBUG: Windows currently does not support DIALOGEX resources!
	// Assert that the template is *not* a DIALOGEX template.
	// DIALOGEX templates are not supported by the PropertySheet API.

	// To change a DIALOGEX template back to a DIALOG template,
	// remove the following:
	//  1. Extended styles on the dialog
	//  2. Help IDs on any control in the dialog
	//  3. Control IDs that are DWORDs
	//  4. Weight, italic, or charset attributes on the dialog's font

	if (((DLGTEMPLATEEX*)pTemplate)->signature == 0xFFFF)
	{
		// it's a DIALOGEX -- we'd better check
		DWORD dwVersion = ::GetVersion();
		if (dwVersion & 0x80000000)
		{
			// it's Win95 -- versions of COMCTL32.DLL that export
			// a function called DllGetVersion are okay

			HINSTANCE hInst = LoadLibrary(_T("COMCTL32.DLL"));
			ASSERT(hInst != NULL);
			if (hInst != NULL)
			{
				FARPROC proc = GetProcAddress(hInst, "DllGetVersion");
				if (proc == NULL)
					ASSERT(FALSE);
				FreeLibrary(hInst);
			}
		}
		else if (LOBYTE(LOWORD(dwVersion)) == 3)
		{
			// it's Windows NT 3.x; we have no hope of this working
			ASSERT(FALSE);
		}
	}
#endif // _DEBUG

#ifndef _AFX_NO_OCC_SUPPORT
	// if the dialog could contain OLE controls, deal with them now
	if (afxOccManager != NULL)
		pTemplate = pPage->InitDialogInfo(pTemplate);
#endif

	// set font of property page to same font used by property sheet
//	HGLOBAL hTemplate = _AfxChangePropPageFont(pTemplate, bWizard);

	if (pPage->m_hDialogTemplate != NULL)
	{
		GlobalFree(pPage->m_hDialogTemplate);
		pPage->m_hDialogTemplate = NULL;
	}

//	if (hTemplate != NULL)
//	{
//		pTemplate = (LPCDLGTEMPLATE)hTemplate;
//		m_hDialogTemplate = hTemplate;
//	}
	psp.pResource = pTemplate;
	psp.dwFlags |= PSP_DLGINDIRECT;
}

// копия базового класса, но с подменой CPropertyPage::PreProcessPageTemplate
#if _MSC_VER >= 1300
void CBasePropertySheet::BuildPropPageArray()
{
	// delete existing prop page array
	free((void*)m_psh.ppsp);
	m_psh.ppsp = NULL;

	// determine size of PROPSHEETPAGE array
	int i;
	int nBytes = 0;
	for (i = 0; i < m_pages.GetSize(); i++)
	{
		CPropertyPage* pPage = GetPage(i);
		nBytes += pPage->m_psp.dwSize;
	}

	// build new PROPSHEETPAGE array
	PROPSHEETPAGE* ppsp = (PROPSHEETPAGE*)malloc(nBytes);
	BYTE* ppspOrigByte=reinterpret_cast<BYTE*>(ppsp);
	if (ppsp == NULL)
		AfxThrowMemoryException();
	BYTE* pPropSheetPagesArrEnd=ppspOrigByte + nBytes;
	ENSURE(pPropSheetPagesArrEnd >= ppspOrigByte);
	m_psh.ppsp = ppsp;
	BOOL bWizard = (m_psh.dwFlags & (PSH_WIZARD | PSH_WIZARD97));
	for (i = 0; i < m_pages.GetSize(); i++)
	{
		CPropertyPage* pPage = GetPage(i);
		BYTE* ppspByte=reinterpret_cast<BYTE*>(ppsp);		
		ENSURE_THROW(ppspByte >= ppspOrigByte && ppspByte <= pPropSheetPagesArrEnd,AfxThrowMemoryException());
		Checked::memcpy_s(ppsp, pPropSheetPagesArrEnd - reinterpret_cast<BYTE*>(ppsp), &pPage->m_psp, pPage->m_psp.dwSize);

		if (!((_CCPropertyPage*)pPage)->m_strHeaderTitle.IsEmpty())
		{
			ppsp->pszHeaderTitle = ((_CCPropertyPage*)pPage)->m_strHeaderTitle;
			ppsp->dwFlags |= PSP_USEHEADERTITLE;
		}
		if (!((_CCPropertyPage*)pPage)->m_strHeaderSubTitle.IsEmpty())
		{
			ppsp->pszHeaderSubTitle = ((_CCPropertyPage*)pPage)->m_strHeaderSubTitle;
			ppsp->dwFlags |= PSP_USEHEADERSUBTITLE;
		}
//		pPage->PreProcessPageTemplate(*ppsp, bWizard);
		CPropertyPage_PreProcessPageTemplate((_CCPropertyPage*)pPage, *ppsp, bWizard);
		(BYTE*&)ppsp += ppsp->dwSize;
	}
	m_psh.nPages = (int)m_pages.GetSize();
}
#else
void CBasePropertySheet::BuildPropPageArray()
{
	// delete existing prop page array
	delete[] (PROPSHEETPAGE*)m_psh.ppsp;
	m_psh.ppsp = NULL;

	// build new prop page array
	AFX_OLDPROPSHEETPAGE* ppsp = new AFX_OLDPROPSHEETPAGE[m_pages.GetSize()];
	m_psh.ppsp = (LPPROPSHEETPAGE)ppsp;
	BOOL bWizard = (m_psh.dwFlags & (PSH_WIZARD | PSH_WIZARD97));
	for (int i = 0; i < m_pages.GetSize(); i++)
	{
		CPropertyPage* pPage = GetPage(i);
		memcpy(&ppsp[i], &pPage->m_psp, sizeof(pPage->m_psp));
//		pPage->PreProcessPageTemplate((PROPSHEETPAGE&)ppsp[i], bWizard);
		CPropertyPage_PreProcessPageTemplate((_CCPropertyPage*)pPage, (PROPSHEETPAGE&)ppsp[i], bWizard);
	}
	m_psh.nPages = m_pages.GetSize();
}
#endif

#if _MSC_VER >= 1300
void CBasePropertySheet::AddPage(CPropertyPage* pPage)
{
	ASSERT_VALID(this);
	ENSURE_VALID(pPage);
	ASSERT_KINDOF(CPropertyPage, pPage);

	// add page to internal list
	m_pages.Add(pPage);

	// add page externally
	if (m_hWnd != NULL)
	{
		// determine size of PROPSHEETPAGE array
		PROPSHEETPAGE* ppsp = const_cast<PROPSHEETPAGE*>(m_psh.ppsp);
		int nBytes = 0;
		int nNextBytes;
		for (UINT i = 0; i < m_psh.nPages; i++)
		{
			nNextBytes = nBytes + ppsp->dwSize;
			if ((nNextBytes < nBytes) || (nNextBytes < (int)ppsp->dwSize))
				AfxThrowMemoryException();
			nBytes = nNextBytes;
			(BYTE*&)ppsp += ppsp->dwSize;
		}

		nNextBytes = nBytes + pPage->m_psp.dwSize;
		if ((nNextBytes < nBytes) || (nNextBytes < (int)pPage->m_psp.dwSize))
			AfxThrowMemoryException();

		// build new prop page array
		ppsp = (PROPSHEETPAGE*)realloc((void*)m_psh.ppsp, nNextBytes);
		if (ppsp == NULL)
			AfxThrowMemoryException();
		m_psh.ppsp = ppsp;

		// copy processed PROPSHEETPAGE struct to end
		(BYTE*&)ppsp += nBytes;
		Checked::memcpy_s(ppsp, nNextBytes - nBytes , &pPage->m_psp, pPage->m_psp.dwSize);
//		pPage->PreProcessPageTemplate(*ppsp, IsWizard());
		CPropertyPage_PreProcessPageTemplate((_CCPropertyPage*)pPage, *ppsp, IsWizard());
		if (!((_CCPropertyPage*)pPage)->m_strHeaderTitle.IsEmpty())
		{
			ppsp->pszHeaderTitle = ((_CCPropertyPage*)pPage)->m_strHeaderTitle;
			ppsp->dwFlags |= PSP_USEHEADERTITLE;
		}
		if (!((_CCPropertyPage*)pPage)->m_strHeaderSubTitle.IsEmpty())
		{
			ppsp->pszHeaderSubTitle = ((_CCPropertyPage*)pPage)->m_strHeaderSubTitle;
			ppsp->dwFlags |= PSP_USEHEADERSUBTITLE;
		}
		HPROPSHEETPAGE hPSP = AfxCreatePropertySheetPage(ppsp);
		if (hPSP == NULL)
			AfxThrowMemoryException();

		if (!SendMessage(PSM_ADDPAGE, 0, (LPARAM)hPSP))
		{
			AfxDestroyPropertySheetPage(hPSP);
			AfxThrowMemoryException();
		}
		++m_psh.nPages;
	}
}
#else
void CBasePropertySheet::AddPage(CPropertyPage* pPage)
{
	ASSERT_VALID(this);
	ASSERT(pPage != NULL);
	ASSERT_KINDOF(CPropertyPage, pPage);
	ASSERT_VALID(pPage);

	// add page to internal list
	m_pages.Add(pPage);

	// add page externally
	if (m_hWnd != NULL)
	{
		// build new prop page array
		AFX_OLDPROPSHEETPAGE *ppsp = new AFX_OLDPROPSHEETPAGE[m_pages.GetSize()];
		memcpy(ppsp, m_psh.ppsp, sizeof(AFX_OLDPROPSHEETPAGE) * (m_pages.GetSize()-1));
		delete[] (PROPSHEETPAGE*)m_psh.ppsp;
		m_psh.ppsp = (PROPSHEETPAGE*)ppsp;
		ppsp += m_pages.GetSize()-1;

		// copy processed PROPSHEETPAGE struct to end
		memcpy(ppsp, &pPage->m_psp, sizeof(pPage->m_psp));
//		pPage->PreProcessPageTemplate((PROPSHEETPAGE&)*ppsp, IsWizard());
		CPropertyPage_PreProcessPageTemplate((_CCPropertyPage*)pPage, (PROPSHEETPAGE&)*ppsp, IsWizard());
		HPROPSHEETPAGE hPSP = CreatePropertySheetPage((PROPSHEETPAGE*)ppsp);
		if (hPSP == NULL)
			AfxThrowMemoryException();

		if (!SendMessage(PSM_ADDPAGE, 0, (LPARAM)hPSP))
		{
			DestroyPropertySheetPage(hPSP);
			AfxThrowMemoryException();
		}
	}
}
#endif

BOOL CBasePropertySheet::Create(CWnd* pParentWnd, DWORD dwStyle, DWORD dwExStyle)
{
	dwStyle |= DS_SETFONT;
	return CPropertySheet::Create(pParentWnd, dwStyle, dwExStyle);
}
