/** 
 * @file
 * @brief	Implementation of Request interface.
 * @author	Andrey Guzhov
 * @date	19.04.2007
 */

#ifndef _AG_RECURRENTREQUEST_H_
#define _AG_RECURRENTREQUEST_H_

#include <vector>
#include <list>
#include <boost/weak_ptr.hpp>
#include "../include/Request.h"
#include "AuthTokenImp.h"
#include "InternetObserver.h"

namespace OnlineActivation
{
class HttpRequest;
class HttpResponse;
class InternetSession;
class RequestData;

/**
 * Recurrent request class.
 */
class RecurrentRequest	: private InternetObserver
						, private Request
{
public:
	/**
	 * Creates initial request object.
	 */
	static boost::shared_ptr<Request> CreateInitialRequest
	(
		const boost::shared_ptr<InternetSession>& pSession,
		const boost::shared_ptr<RequestData>& pData,
		const std::vector<std::string>& urls
	);
	/**
	 * Composes reply object.
	 */
	boost::shared_ptr<Request> ComposeReply(const boost::shared_ptr<RequestData>& pData);
	/**
	 * Sends request.
	 */
	boost::shared_ptr<HttpResponse> Send();
	/**
	 * Aborts request execution.
	 */
	void Abort();
	/**
	 * Returns true if request is aborted.
	 */
	bool IsAborted() const;
	/**
	 * Returns request observer object.
	 */
	RequestObserver* GetObserver() const;

private:
	/**
	 * Constructs request prototype object.
	 */
	RecurrentRequest
	(
		const boost::shared_ptr<InternetSession>& pSession,
		const boost::shared_ptr<RequestData>& pData,
		const std::vector<std::string>& urls
	);
	/**
	 * Constructs regular request object.
	 */
	RecurrentRequest
	(
		const boost::shared_ptr<InternetSession>& pSession,
		const boost::shared_ptr<RequestData>& pData,
		const std::list<std::string>& urls,
		const AuthTokenImp& authProxy,
		const AuthTokenImp& authServer,
		RequestObserver& pObserver
	);
	/**
	 * Constructs reply object.
	 */
	RecurrentRequest
	(
		const boost::shared_ptr<InternetSession>& pSession,
		const boost::shared_ptr<RequestData>& pData,
		const std::list<std::string>& urls,
		const AuthTokenImp& authProxy,
		const AuthTokenImp& authServer
	);
	/**
	 * Inherited request function implementation.
	 */
	virtual boost::shared_ptr<Response> Submit(RequestObserver& observer);
	/**
	 * Inherited internet observer function implementation.
	 */
	virtual void Update(DWORD dwStatus, LPVOID lpvInfo, DWORD dwInfoLen);
	/**
	 * Sends request and handles RETRY response.
	 */
	boost::shared_ptr<HttpResponse> SendHandleRetry();
	/**
	 * Sends request and handles server/proxy authentication.
	 */
	boost::shared_ptr<HttpResponse> SendHandleAuth(HttpRequest& httpRequest);
	/**
	 * Handles server response.
	 */
	void HandleServerResponse(HttpResponse& httpResponse);
	/**
	 * Sets proxy authentication data.
	 */
	void SetProxyCredentials(HttpRequest& httpRequest);
	/**
	 * Sets server authentication data.
	 */
	void SetServerCredentials(HttpRequest& httpRequest);

private:
	/**
	 * Observes the request execution progress.
	 */
	RequestObserver* m_pObserver;
	/**
	 * Internet session for request creation.
	 */
	boost::shared_ptr<InternetSession> m_pSession;
	/**
	 * Weak pointer to current request object.
	 */
	boost::weak_ptr<HttpRequest> m_pRequest;
	/**
	 * Proxy authentication token.
	 */
	AuthTokenImp m_authProxy;
	/**
	 * Server authentication token.
	 */
	AuthTokenImp m_authServer;
	/**
	 * Data to be sent.
	 */
	boost::shared_ptr<RequestData> m_pData;
	/**
	 * Current URL.
	 */
	std::string m_curUrl;
	/**
	 * Sending is aborted flag.
	 */
	volatile long m_bAborted;
	/**
	 * Activation URL list.
	 */
	std::list<std::string> m_urls;

	friend class boost::shared_ptr<Request>;
};

}
#endif // _AG_RECURRENTREQUEST_H_
