#if !defined(AFX_IDSCLIENT_H__9CEE4AC9_D4F5_4B44_872C_373F4D94C0EF__INCLUDED_)
#define AFX_IDSCLIENT_H__9CEE4AC9_D4F5_4B44_872C_373F4D94C0EF__INCLUDED_

#include "IDS.h"

class CIDSClient : public CGuardClient
{
public:
	CIDSClient (CKAHUM_IDS *pKAHUM = NULL);
	virtual ~CIDSClient ();

	bool Create (HANDLE hShutdownEvent);
	void StopClient ();

	VERDICT OnEvent( PVOID pMessage );

protected:

	void RemoveIDSFilter ();
	bool AddIDSFilter ();

	virtual void OnStopClient (BOOL bResidend);

private:
	 ULONG	m_uFilterID;
	 CKAHUM_IDS *m_pKAHUM;
};

#endif // !defined(AFX_IDSCLIENT_H__9CEE4AC9_D4F5_4B44_872C_373F4D94C0EF__INCLUDED_)
