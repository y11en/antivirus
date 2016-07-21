// KnownProcessList.cpp: implementation of the CKnownProcessList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KnownProcessList.h"
#include "servicestuff.h"
#include "AutoPTCHAR.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
 	data file struct
	1. ImagePath_Len	- DWORD
	2. ImagePath		- WCHAR arr, lenght == ImagePath_Len
	3. Hash				- md5_byte_t[16]

 */
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKnownProcessList::CKnownProcessList()
{
	m_hFile = INVALID_HANDLE_VALUE;
	if (!ReopenDataFile() || m_ProcList.size() == 0)
	{
		WCHAR FileNameMain[4096 * 2];
		WCHAR FileNameBackup[4096 * 2];

		if (GenerateName(FileNameMain, sizeof(FileNameMain)/sizeof(FileNameMain[0])) &&
			GenerateName(FileNameBackup, sizeof(FileNameBackup)/sizeof(FileNameBackup[0]), false))
		{
			if (CopyFile(FileNameBackup, FileNameMain, FALSE))
				ReopenDataFile();
		}
	}

	ResolveCmd();
}

CKnownProcessList::~CKnownProcessList()
{
	SaveList();
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
	
	FreeList();
}

size_t CKnownProcessList::GetKnownCount()
{
	CAutoLock auto_lock(&m_Sync);
	return m_ProcList.size();
}

void CKnownProcessList::FreeList()
{
	CAutoLock auto_lock(&m_Sync);

	KnownProcess Known;
	_KnownProcList::iterator _it;

	for (_it = m_ProcList.begin(); _it != m_ProcList.end(); ++_it)
	{
		Known = *_it;
		HeapFree(GetProcessHeap(), 0, Known.m_pwchImagePath);
	}

	m_ProcList.erase(m_ProcList.begin(), m_ProcList.end());
}

bool CKnownProcessList::GenerateName(PWCHAR pwszFileName, DWORD FileNameSizeInSymbols, bool bBackup)
{
	DWORD len = GetModuleFileName(0, pwszFileName, FileNameSizeInSymbols);
	while (len > 0)
	{
		if (pwszFileName[len] == L'.')
		{
			len++;
			pwszFileName[len] = 0;
			if (!bBackup)
				lstrcat(pwszFileName, _T("kpl"));
			else
				lstrcat(pwszFileName, _T("kpb"));
			break;
		}
		len--;
	}

	if (len == 0)
		return false;

	return true;
}

bool CKnownProcessList::ReopenDataFile()
{
	WCHAR FileName[4096 * 2];

	if (GenerateName(FileName, sizeof(FileName)/sizeof(FileName[0])))
	{
		m_hFile = CreateFile(FileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, NULL);
		
		if (ReloadList())
			return true;

		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
	return false;
}

bool CKnownProcessList::ReloadList()
{
	if (m_hFile == INVALID_HANDLE_VALUE)
		return false;

	bool bDataOk = true;
	DWORD dwR;
	
	DWORD dwLen;
	PWCHAR pwchImagePath;

	KnownProcess Known;

	{
		CAutoLock auto_lock(&m_Sync);
		m_ProcList.reserve(100);
		do
		{
			if (!ReadFile(m_hFile, &dwLen, sizeof(dwLen), &dwR, NULL))
				bDataOk = false;
			else
			{
				if (dwR != sizeof(dwLen))
					break;

				pwchImagePath = (PWCHAR) HeapAlloc(GetProcessHeap(), 0, dwLen);
				if (pwchImagePath == NULL)
					bDataOk = false;
				else
				{
					if (!ReadFile(m_hFile, pwchImagePath, dwLen, &dwR, NULL))
						bDataOk = false;
					else
					{
						pwchImagePath[(dwLen - 1) / sizeof(WCHAR)] = 0;
						if ((dwR == 0) || (dwR != dwLen) || ((lstrlen(pwchImagePath) + 1) * sizeof(WCHAR) != dwLen))
						{
							HeapFree(GetProcessHeap(), 0, pwchImagePath);
							bDataOk = false;
						}
						else
						{
							ReadFile(m_hFile, Known.m_Hash, sizeof(Known.m_Hash), &dwR, NULL);
							if (dwR != sizeof(Known.m_Hash))
							{
								HeapFree(GetProcessHeap(), 0, pwchImagePath);
								bDataOk = false;
							}
							else
							{
								Known.m_pwchImagePath = pwchImagePath;
								m_ProcList.push_back(Known);
							}
						}
					}
				}
			}

			if (bDataOk == false)
				break;
		} while(true);
	}

	if (!bDataOk)
	{
		FreeList();
		return false;
	}

	return true;
}

void CKnownProcessList::SaveList()
{
	if (m_hFile == INVALID_HANDLE_VALUE)
		return;

	WCHAR FileNameMain[4096 * 2];
	WCHAR FileNameBackup[4096 * 2];

	if (GenerateName(FileNameMain, sizeof(FileNameMain)/sizeof(FileNameMain[0])) &&
		GenerateName(FileNameBackup, sizeof(FileNameBackup)/sizeof(FileNameBackup[0]), true))
	{
		CopyFile(FileNameMain, FileNameBackup, FALSE);
	}

	DWORD dwW;
	
	DWORD dwLen;

	KnownProcess Known;
	_KnownProcList::iterator _it;

	CAutoLock auto_lock(&m_Sync);

	SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);
	SetEndOfFile(m_hFile);

	for (_it = m_ProcList.begin(); _it != m_ProcList.end(); ++_it)
	{
		Known = *_it;

		dwLen = (lstrlen(Known.m_pwchImagePath) + 1) * sizeof(WCHAR);
		WriteFile(m_hFile, &dwLen, sizeof(dwLen), &dwW, NULL);
		WriteFile(m_hFile, Known.m_pwchImagePath, dwLen , &dwW, NULL);
		WriteFile(m_hFile, Known.m_Hash, sizeof(Known.m_Hash), &dwW, NULL);
	}
}

bool CKnownProcessList::IsKnownProcess(CProcessRecognizer* pRecognizedProcess)
{
	CAutoLock auto_lock(&m_Sync);

	return IsExist_Internal(pRecognizedProcess);
}

bool CKnownProcessList::IsExist_Internal(CProcessRecognizer* pRecognizedProcess)
{
	bool bFound = false;
	KnownProcess Known;
	_KnownProcList::iterator _it;

	for (_it = m_ProcList.begin(); (_it != m_ProcList.end()) && (bFound == false); ++_it)
	{
		Known = *_it;
		if (memcmp(pRecognizedProcess->GetHash(), Known.m_Hash, sizeof(Known.m_Hash)) == 0)
			bFound = true;
	}

	return bFound;
}

void CKnownProcessList::RemoveByName(PWCHAR pwchImagePath)
{
	CAutoLock auto_lock(&m_Sync);
	RemoveByNameInternal(pwchImagePath);
}

void CKnownProcessList::RemoveByNameInternal(PWCHAR pwchImagePath)
{
	KnownProcess Known;
	_KnownProcList::iterator _it;

	CAutoLock auto_lock(&m_Sync);

	for (_it = m_ProcList.begin(); _it != m_ProcList.end(); ++_it)
	{
		Known = *_it;
		if (lstrcmp(Known.m_pwchImagePath, pwchImagePath) == 0)
		{
			m_ProcList.erase(_it);
			HeapFree(GetProcessHeap(), 0, Known.m_pwchImagePath);
			return;
		}
	}
}

bool CKnownProcessList::RegisterNewProcess(PWCHAR pwchImagePath, CProcessRecognizer* pRecognizedProcess)
{
	CAutoLock auto_lock(&m_Sync);

	if (!pRecognizedProcess->IsRecognized())
		return false;

	if (IsExist_Internal(pRecognizedProcess))
		return true;

	PWCHAR pwchImagePathTmp = (PWCHAR) HeapAlloc(GetProcessHeap(), 0, (lstrlen(pwchImagePath) + 1) * sizeof(WCHAR));
	if (pwchImagePathTmp == NULL)
		return false;

	RemoveByNameInternal(pwchImagePath);
	lstrcpy(pwchImagePathTmp, pwchImagePath);

	KnownProcess Known;

	Known.m_pwchImagePath = pwchImagePathTmp;
	memcpy(Known.m_Hash, pRecognizedProcess->GetHash(), sizeof(Known.m_Hash));
	m_ProcList.push_back(Known);

	return true;
}

bool CKnownProcessList::ExistByName(PWCHAR pwchImagePath)
{
	KnownProcess Known;
	_KnownProcList::iterator _it;

	CAutoLock auto_lock(&m_Sync);

	for (_it = m_ProcList.begin(); _it != m_ProcList.end(); ++_it)
	{
		Known = *_it;
		if (lstrcmp(Known.m_pwchImagePath, pwchImagePath) == 0)
			return true;
	}

	return false;
}

void CKnownProcessList::ResolveCmd()
{
	CString strtmp;
	strtmp.Empty();
	CString strExpanded;
	{
		AutoPTCHAR auptchExpanded(&strtmp, 4096);
		if (auptchExpanded.IsValid())
		{
			if (ExpandEnvironmentStrings(_T("%ComSpec%"), auptchExpanded, auptchExpanded.GetBufferLength()))
				m_Cmd.Recognize(auptchExpanded);
		}
	}
}

CProcessRecognizer* CKnownProcessList::GetCmd()
{
	return &m_Cmd;
}
