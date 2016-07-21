#if !defined(AFX_PACKETCLIENT_H__1051C49E_1FCD_4B8C_82F9_06F24BC86BED__INCLUDED_)
#define AFX_PACKETCLIENT_H__1051C49E_1FCD_4B8C_82F9_06F24BC86BED__INCLUDED_

#include <CKAH/ckahum.h>

class CPacketClient : public CGuardClient
{
public:
	CPacketClient ();
	virtual ~CPacketClient ();

	bool Enable (bool bEnable);
	void SignalStop ();

	void SetPacketNotifyCallback (CKAHFW::PACKETRULENOTIFYCALLBACK PacketNotifyCallback, LPVOID lpCallbackParam)
	{
		m_fnPacketNotifyCallback = PacketNotifyCallback;
		m_lpCallbackParam = lpCallbackParam;
	}

	bool AddBlockFilters (bool bIsBlock);

protected:	
	bool Create (HANDLE hShutdownEvent);
	void StopClient ();

	virtual VERDICT OnEvent( PVOID pMessage );

	CKAHFW::PACKETRULENOTIFYCALLBACK m_fnPacketNotifyCallback;
	LPVOID m_lpCallbackParam;
};

#endif // !defined(AFX_PACKETCLIENT_H__1051C49E_1FCD_4B8C_82F9_06F24BC86BED__INCLUDED_)
