// KnownProcessList.h: interface for the CKnownProcessList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KNOWNPROCESSLIST_H__33A48F49_B172_4C29_A136_4CEA32A22A5A__INCLUDED_)
#define AFX_KNOWNPROCESSLIST_H__33A48F49_B172_4C29_A136_4CEA32A22A5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProcessRecognizer.h"
#include <vector>

typedef struct _tKnownProcess
{
	PWCHAR m_pwchImagePath;
	md5_byte_t m_Hash[16];
}KnownProcess, *PKnownProcess;

typedef std::vector<KnownProcess> _KnownProcList;

//+ ------------------------------------------------------------------------------------------

class CKnownProcessList : public COwnSync
{
public:
	CKnownProcessList();
	virtual ~CKnownProcessList();

	bool RegisterNewProcess(PWCHAR pwchImagePath, CProcessRecognizer* pRecognizedProcess);
	bool IsKnownProcess(CProcessRecognizer* pRecognizedProcess);
	bool ExistByName(PWCHAR pwchImagePath);

	_KnownProcList* QueryAndLockList()
	{
		LockSync();
		return &m_ProcList;
	}
	void ReleaseLockedList()
	{
		UnLockSync();
	}

	size_t GetKnownCount();

	void RemoveByName(PWCHAR pwchImagePath);

	CProcessRecognizer* GetCmd();
private:
	bool GenerateName(PWCHAR pwszFileName, DWORD FileNameSizeInSymbols, bool bBackup = false);
	bool ReopenDataFile();
	bool ReloadList();
	void SaveList();

	void FreeList();
	void RemoveByNameInternal(PWCHAR pwchImagePath);

	bool IsExist_Internal(CProcessRecognizer* pRecognizedProcess);
	
	HANDLE m_hFile;

	_KnownProcList m_ProcList;

	void ResolveCmd();
	CProcessRecognizer m_Cmd;
};

#endif // !defined(AFX_KNOWNPROCESSLIST_H__33A48F49_B172_4C29_A136_4CEA32A22A5A__INCLUDED_)
