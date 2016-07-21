#pragma once

#include "ActivationTypes.h"
#include "ActivationErrors.h"
#include "RequestObserver.h"
#include <boost/shared_ptr.hpp>
#include <vector>
#include <wtypes.h>

namespace OnlineActivation
{
	class Response;
}

struct IActivationCallback
{
	virtual void OnSendingRequest() {}
	virtual void OnRequestSent(size_t nBytes) {}
	virtual void OnRequestComplete() {}
	virtual bool GetProxyCredentials(
		char* user, size_t user_size,
		char* password, size_t password_size) { return false; }
};

class CActivation: private OnlineActivation::RequestObserver
{
public:
	enum EResult { None, Canceled, Error, HtmlForm, KeyFile };

	CActivation(IActivationCallback* pActivationCallback, void* hTrace);
	~CActivation();
	bool BeginActivation(
		const OnlineActivation::ActivationConfig& cfg,
		const OnlineActivation::ApplicationId& applicationId,
		const OnlineActivation::ActivationCode& code,
		const OnlineActivation::CustomerId* pCustomerId);
	bool ContinueActivation(
		const char* headers,
		void* pBuffer,
		unsigned long nBufLen);
	void Cancel();
	EResult GetResult() const { return m_Result; }
	OnlineActivation::ActivationError::ErrorType GetErrorType() const { return m_ErrorType; }
	unsigned long GetErrorCode() const { return m_ErrorCode; }
	const void* GetData() const { return &m_data[0]; }
	unsigned long GetDataSize() const { return m_data.size(); }

private:
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

	bool WaitResponse();

	EResult       m_Result;
	OnlineActivation::ActivationError::ErrorType m_ErrorType;
	unsigned long m_ErrorCode;
	std::vector<char> m_data;

	boost::shared_ptr<OnlineActivation::Response> m_pResponse;
	IActivationCallback* m_pActivationCallback;
	HMODULE m_hLibola;
	HANDLE m_evReqComplete;
	HANDLE m_evStop;
	void* m_hTrace;
	enum { Begin, SendingRequest, RequestSent, RequestComplete } m_eState;
};