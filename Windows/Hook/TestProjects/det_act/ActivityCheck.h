// ActivityCheck.h: interface for the CActivityCheck class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTIVITYCHECK_H__E64C52C0_B8AA_42EE_8477_2F8611199730__INCLUDED_)
#define AFX_ACTIVITYCHECK_H__E64C52C0_B8AA_42EE_8477_2F8611199730__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Log.h"
#include "SelfCopyHistory.h"
#include "proclist.h"
#include "RegHistory.h"
#include "PathList.h"

class CActivityCheck  
{
public:
	CActivityCheck();
	virtual ~CActivityCheck();

public:
	bool CheckSelfCopy(CSingleProcess* pProcess, PWCHAR pwchDestinationPath);
	void RedirectedOutput(CSingleProcess* pProcess, PWCHAR pwchComandLine);
	void CheckAutorunKey(CSingleProcess* pProcess, PWCHAR pwchKeyName, PWCHAR pwchValueName, PWCHAR pwchValue = NULL);
	bool CheckP2PKey(CSingleProcess* pProcess, PWCHAR pwchKeyName, PWCHAR pwchValueName);
	bool CheckP2PRegistry();
	bool IsP2PKey(PWCHAR pwchKeyName, PWCHAR pwchValueName);
	CLog m_Log;

private:
	bool AddP2PPath(PWCHAR pwcsPath, PWCHAR* ppwcsLongPath = NULL);
	CSelfCopyHistory m_SelfCopyHistory;
	CPathList m_NetPaths;
	CPathList m_StartupPaths;
	CRegHistory m_RegHistory;
};

#endif // !defined(AFX_ACTIVITYCHECK_H__E64C52C0_B8AA_42EE_8477_2F8611199730__INCLUDED_)
