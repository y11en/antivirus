#ifndef _AG_REQUESTOBSERVER_H_
#define _AG_REQUESTOBSERVER_H_

namespace OnlineActivation
{
class AuthToken;
class ActivationError;
/**
 * Activation request observer.
 * This interface should be used by clients to receive notifications
 * about activation request progress.
 * @interface
 */
class RequestObserver
{
public:
	/**
	 * Fired when request execution is started. 
	 */
	virtual void OnRequestStarted() = 0;

	/**
	 * Fired when server authentication required. 
	 * @param[out] serverAuth authentication token to fill.
	 */
	virtual bool OnServerAuthRequired(AuthToken& serverAuth) = 0;

	/**
	 * Fired when proxy authentication required. 
	 * @param[out] proxyAuth authentication token to fill.
	 */
	virtual bool OnProxyAuthRequired(AuthToken& proxyAuth) = 0;

	/**
	 * Fired when resolving server name. 
	 * @param[in] szName server name.
	 */
	virtual void OnResolvingName(const char* szName) = 0;

	/**
	 * Fired when server name is resolved. 
	 * @param[in] szIpAddr IP address of server.
	 */
	virtual void OnNameResolved(const char* szIpAddr) = 0;

	/**
	 * Fired when connecting to server. 
	 * @param[in] szSockAddr server address.
	 */
	virtual void OnConnectingToServer(const char* szSockAddr) = 0;

	/**
	 * Fired when connected to server. 
	 * @param[in] szSockAddr server address.
	 */
	virtual void OnConnectedToServer(const char* szSockAddr) = 0;

	/**
	 * Fired when sending HTTP request. 
	 */
	virtual void OnSendingRequest() = 0;

	/**
	 * Fired when HTTP request is sent. 
	 * @param[in] nBytes bytes transferred.
	 */
	virtual void OnRequestSent(size_t nBytes) = 0;

	/**
	 * Fired when receiving HTTP response. 
	 */
	virtual void OnReceivingResponse() = 0;

	/**
	 * Fired when HTTP response is received. 
	 */
	virtual void OnResponseReceived(size_t nBytes) = 0;

	/**
	 * Fired when closing connection to server. 
	 */
	virtual void OnClosingConnection() = 0;

	/**
	 * Fired when connection to server is closed. 
	 */
	virtual void OnConnectionClosed() = 0;

	/**
	 * Fired when detecting proxy server. 
	 */
	virtual void OnDetectingProxy() = 0;

	/**
	 * Fired when request execution successfully completed.
	 */
	virtual void OnRequestCompleted() = 0;

	/**
	 * Fired when request execution failed.
	 * @param[in] error specifies failure reason.
	 */
	virtual void OnRequestFailed(const ActivationError& error) = 0;

	/**
	 * Fired when redirecting to new location. 
	 * @param[in] szUrl specifies new location.
	 */
	virtual void OnRedirect(const char* szUrl) = 0;

	/**
	 * Fired when intermediate response received.
	 */
	virtual void OnIntermediateResponse() = 0;

	/**
	 * Fired when moved between a secure (HTTPS) and a non secure (HTTP) site.
	 * @param[in] nFlags additional WinInet flags.
	 */
	virtual void OnStateChange(unsigned long nFlags) = 0;

	/**
	 * Fired when cookie is sent.
	 */
	virtual void OnCookieSent() = 0;

	/**
	 * Fired when cookie is received.
	 */
	virtual void OnCookieReceived() = 0;

	/**
	 * Fired when server requests client to wait.
	 * @param[in] nTimeout specifies timeout value.
	 */
	virtual void OnWaitTimeout(unsigned long nTimeout) = 0;
};

} // namespace OnlineActivation

#endif // _AG_REQUESTOBSERVER_H_
