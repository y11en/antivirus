#ifndef _AG_ASYNCRESPONSE_H_
#define _AG_ASYNCRESPONSE_H_

#include <utils/prague/scoped_handle.hpp>
#include "../include/Response.h"

namespace OnlineActivation
{
class Content;
class Request;
class RecurrentRequest;
class ActivationError;
/**
 * Asynchronous request implementation class.
 */
class FutureResponse : public Response
{
	typedef util::scoped_handle<HANDLE, BOOL (WINAPI*)(HANDLE), &::CloseHandle, 1> handle_t;
public:
	/**
	 * Creates new response object.
	 */
	FutureResponse(std::auto_ptr<RecurrentRequest>& pRequest);
	/**
	 * Destroys object and wait for thread completion.
	 */
	~FutureResponse();
	/**
	 * Cancels pending response.
	 */
	virtual void Cancel() const;
	/**
	 * Returns received content.
	 */
	virtual boost::shared_ptr<Content> GetContent() const;
	/**
	 * Composes reply to activation server.
	 */
	virtual boost::shared_ptr<Request> ComposeReply
	(
		const char* szHeaders, 
		size_t nHeadersLength, 
		const void* pData,
		size_t nDataSize
	);

private:
	/**
	 * Composes reply to activation server.
	 */
	void SendRequestDeferred();
	/**
	 * Performs error checking.
	 */
	void CheckErrors() const;
	/**
	 * Composes reply to activation server.
	 */
	static DWORD WINAPI ThreadProc(LPVOID lpContext);

private:
	boost::shared_ptr<HttpResponse>	m_pResponse;
	std::auto_ptr<RecurrentRequest>	m_pRequest;
	std::auto_ptr<ActivationError>	m_pError;
	handle_t m_hThread;
};

} // namespace OnlineActivation
#endif // _AG_ASYNCRESPONSE_H_