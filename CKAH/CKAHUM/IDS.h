#if !defined(AFX_IDS_H__61F9C3A4_A2B4_4DEE_8614_18E1EFF360A9__INCLUDED_)
#define AFX_IDS_H__61F9C3A4_A2B4_4DEE_8614_18E1EFF360A9__INCLUDED_

#include <CKAH/ckahum.h>
#include "../../Windows/Hook/Hook/avpgcom.h"

class CIDSClient;

class CKAHUM_IDS
{
public:
	CKAHUM_IDS ();
	~CKAHUM_IDS ();

	bool Enable (bool bEnable);
	
	void SetAttackNotifyCallback (CKAHIDS::ATTACKNOTIFYPROC AttackNotifyCallback, LPVOID lpCallbackParam)
	{
		m_fnAttackNotifyCallback = AttackNotifyCallback;
		m_lpCallbackParam = lpCallbackParam;
	}

	virtual VERDICT OnEvent( PVOID pMessage );

private:
	CIDSClient *m_pIDSClient;
	CKAHIDS::ATTACKNOTIFYPROC m_fnAttackNotifyCallback;
	LPVOID m_lpCallbackParam;
};


#endif // !defined(AFX_IDS_H__61F9C3A4_A2B4_4DEE_8614_18E1EFF360A9__INCLUDED_)
