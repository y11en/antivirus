#ifndef _AG_REQUESTPROGRESS_H_
#define _AG_REQUESTPROGRESS_H_

#include <OnlineActivation.h>

class COlaTestDlg;

class RequestProgress : public OnlineActivation::RequestObserver
{
public:
	RequestProgress(COlaTestDlg* pDialog);
	virtual void OnRequestStarted();
	virtual bool OnServerAuthRequired(OnlineActivation::AuthToken& serverAuth);
	virtual bool OnProxyAuthRequired(OnlineActivation::AuthToken& proxyAuth);
	virtual void OnResolvingName(const char* szName);
	virtual void OnNameResolved(const char* szIpAddr);
	virtual void OnConnectingToServer(const char* szSockAddr);
	virtual void OnConnectedToServer(const char* szSockAddr);
	virtual void OnSendingRequest();
	virtual void OnRequestSent(size_t nBytes);
	virtual void OnReceivingResponse();
	virtual void OnResponseReceived(size_t nBytes);
	virtual void OnClosingConnection();
	virtual void OnConnectionClosed();
	virtual void OnDetectingProxy();
	virtual void OnRequestCompleted();
	virtual void OnRequestFailed(const OnlineActivation::ActivationError& error);
	virtual void OnRedirect(const char* szUrl);
	virtual void OnIntermediateResponse();
	virtual void OnStateChange(unsigned long nFlags);
	virtual void OnCookieSent();
	virtual void OnCookieReceived();
	virtual void OnWaitTimeout(unsigned long nTimeout);

private:
	COlaTestDlg* m_pDialog;
};


#endif // _AG_REQUESTPROGRESS_H_
