// Localize.cpp: implementation of the CLocalize class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Localize.h"

#include "Atlbase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLocalize::CLocalize(CString SearchMask, CReestr* pReestr, HINSTANCE hInstanceModule)
{
	m_hInstanceDefault = hInstanceModule;
	m_hLocalizeModule = m_hInstanceDefault;
	m_pReestr = pReestr;
	m_strCurrentLocale = _ENGLISH;
	m_SearchMask = SearchMask;
	CString MolduleName = m_pReestr->GetValue("CurrentLocale", m_strCurrentLocale);
	ChangeLocale(&MolduleName, false);
}

CLocalize::~CLocalize()
{
	if (m_hLocalizeModule != m_hInstanceDefault)
		FreeLibrary(m_hLocalizeModule);
}

BOOL CLocalize::GetModulePath(CString *pstr)
{
	char loc[1024];
	DWORD length = GetModuleFileName(m_hInstanceDefault, loc, sizeof(loc));
	if (length == 0)
		return FALSE;
	int cou = length;
	while ((cou >= 0) && (loc[cou] != '\\')) cou--;
	loc[cou+1] = '\0';
	*pstr = loc;
	
	return !pstr->IsEmpty();
}

BOOL CLocalize::ChangeLocale(CString *pLocale, bool bSave)
{
	if (m_strCurrentLocale == *pLocale)
		return FALSE;

	if (m_hLocalizeModule != m_hInstanceDefault)
	{
		if (!FreeLibrary(m_hLocalizeModule))
			DWORD dwErr = GetLastError();
		m_hLocalizeModule = m_hInstanceDefault;
	}

	CStringArray strArr, libArr;
	GetLMList(&strArr, &libArr);

	if (pLocale->IsEmpty())
		m_hLocalizeModule = m_hInstanceDefault;
	else
	{
		CString LocaleName = _T("");
		for (int cou = 0; cou < strArr.GetSize(); cou++)
			if (strArr.GetAt(cou) == *pLocale)
				LocaleName = libArr.GetAt(cou);
		if (LocaleName.IsEmpty())
		{
			m_hLocalizeModule = m_hInstanceDefault;
			m_strCurrentLocale = _ENGLISH;
		}
		else
		{
	//		ODS(("BUI Locale: ", LocaleName, "\n"));
			m_hLocalizeModule = LoadLibrary(LocaleName);
			if (m_hLocalizeModule != NULL)
			{
				m_strCurrentLocale = *pLocale;
			}
		}
		if (bSave == true)
			m_pReestr->SetValue("CurrentLocale", m_strCurrentLocale);

		strArr.RemoveAll();
		libArr.RemoveAll();

		return TRUE;
	}
	
	return FALSE;
}

CString CLocalize::GetLocalizeString(UINT uID)
{
	CString strTmp;
	BOOL bRes = GetLocalizeString(uID, &strTmp);
	
	return strTmp;
}

BOOL CLocalize::GetLocalizeString(UINT uID, CString* lpstrTarget)
{
//!!	m_CriticalSection.Lock();
	BOOL bRes = FALSE;
	CComBSTR bstr;
	if (bstr.LoadString(m_hLocalizeModule, uID))
	{
		*lpstrTarget = bstr;
		bRes = TRUE;
	}
	if (bRes == FALSE)
	{
		if (bstr.LoadString(m_hInstanceDefault, uID))
		{
			*lpstrTarget = bstr;
			bRes = TRUE;
		}
	}

//!!	m_CriticalSection.Unlock();

	return bRes;
}

BOOL CLocalize::GetLMList(CStringArray* pstrArr, CStringArray* ptrLibName)
{
	pstrArr->RemoveAll();
	CString LocaleName;
	CString strPath;
	if (!GetModulePath(&strPath))
		return FALSE;
	LocaleName = strPath + m_SearchMask;
//	ODS(((LPSTR)(LPCSTR)LocaleName, "\n"));
	
	CFileFind FileFind;
	CString strFind, strtmp;
	BOOL bWorking = FileFind.FindFile(LocaleName);
	bool bFindEnglish = false;
	while (bWorking)
	{
		bWorking = FileFind.FindNextFile();
		strFind = FileFind.GetFileName();
//		ODS(("Localize: File  - ", strFind, " ...\n"));
		HMODULE hLng = LoadLibrary(strPath + strFind);
		if (hLng != NULL)
		{
			FARPROC lngadr = GetProcAddress(hLng, "GetLanguage");
			if (lngadr != NULL)
			{
				strtmp = ((LPSTR (*)(void))lngadr)();
				if (strtmp == _ENGLISH)
					bFindEnglish = true;
				int element = pstrArr->Add(strtmp);
				ptrLibName->Add(strPath + strFind);
//				ODS(("Locale: Found - ", pstrArr->GetAt(element), "\n"));
			}
			FreeLibrary(hLng);
		}
	}

	if (bFindEnglish == false)
	{
		int element = pstrArr->Add(_ENGLISH);
		ptrLibName->Add("");
	}
	return pstrArr->GetSize();
}

bool CLocalize::IsDefaultLocale()
{
	if (m_hLocalizeModule == m_hInstanceDefault)
		return true;

	return false;
}
