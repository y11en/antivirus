// OwnSync.h: interface for the COwnSync class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OWNSYNC_H__AE7059D3_5964_4A19_B268_107390FA0F7F__INCLUDED_)
#define AFX_OWNSYNC_H__AE7059D3_5964_4A19_B268_107390FA0F7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class COwnSync
{
public:
	COwnSync();
	virtual ~COwnSync();
	
	void LockSync();
	void UnLockSync();
	
	CRITICAL_SECTION m_Sync;
	
	operator CRITICAL_SECTION*()
	{
		return &m_Sync;
	}
};

#endif // !defined(AFX_OWNSYNC_H__AE7059D3_5964_4A19_B268_107390FA0F7F__INCLUDED_)
