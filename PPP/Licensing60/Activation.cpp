#include <windows.h>
#include "OnlineActivation.h"
#include "Activation.h"
#include "ActivationErrors.h"
#include <vector>

using namespace OnlineActivation;

CActivation::CActivation(IActivationCallback* pActivationCallback, void* hTrace)
	: m_pActivationCallback(pActivationCallback)
	, m_evReqComplete(CreateEvent(NULL, TRUE, FALSE, NULL))
	, m_evStop(CreateEvent(NULL, TRUE, FALSE, NULL))
	, m_Result(None)
	, m_hTrace(hTrace)
{
	m_hLibola = LoadLibrary("libola.dll");
}

CActivation::~CActivation()
{
	SetEvent(m_evStop);

	CloseHandle(m_evStop);
	CloseHandle(m_evReqComplete);
	
	m_pResponse.reset();

	if (m_hLibola)
		FreeLibrary(m_hLibola);
}

bool CActivation::BeginActivation(const ActivationConfig& cfg,
								  const ApplicationId& applicationId,
								  const ActivationCode& code,
								  const CustomerId* pCustomerId)
{
	try
	{
		if (!m_hLibola)
			throw SystemError(ERROR_FILE_NOT_FOUND);

		typedef boost::shared_ptr<ActivationFactory> (*pfnCreateActivationFactory)(const ActivationConfig& config);
		pfnCreateActivationFactory _CreateActivationFactory
			= (pfnCreateActivationFactory) GetProcAddress(m_hLibola, "CreateActivationFactory");
		if (!_CreateActivationFactory)
			throw SystemError(GetLastError());

		boost::shared_ptr<OnlineActivation::ActivationFactory> pFactory = _CreateActivationFactory(cfg);
		if (!pFactory)
			throw LogicalError(LogicalError::UNEXPECTED_ERROR);

		boost::shared_ptr<Request> pRequest;
		if (pCustomerId && pCustomerId->id != 0)
			pRequest = pFactory->ComposeRequest(applicationId, code, *pCustomerId);
		else
			pRequest = pFactory->ComposeRequest(applicationId, code);

		if (!pRequest)
			throw LogicalError(LogicalError::UNEXPECTED_ERROR);

		m_eState = Begin;
		m_pResponse = pRequest->Submit(*this);
		return WaitResponse();
	}
	catch(const ActivationError& e)
	{
		m_Result = Error;
		m_ErrorType = e.GetErrorType();
		m_ErrorCode = e.GetErrorCode();
		return false;
	}
}

bool CActivation::ContinueActivation(const char* headers,
									 void* pBuffer,
									 unsigned long nBufLen)
{
	try
	{
		boost::shared_ptr<Request> pRequest = m_pResponse->ComposeReply(headers, strlen(headers), pBuffer, nBufLen);
		if (!pRequest)
			throw LogicalError(LogicalError::UNEXPECTED_ERROR);
		m_eState = Begin;
		m_pResponse = pRequest->Submit(*this);
		return WaitResponse();
	}
	catch(const ActivationError& e)
	{
		m_Result = Error;
		m_ErrorType = e.GetErrorType();
		m_ErrorCode = e.GetErrorCode();
		return false;
	}
}

void CActivation::Cancel()
{
	if (m_evStop)
		SetEvent(m_evStop);
}

// Реализован в licensing.cpp т.к. в этом сорце не должно быть Праги (в ней переопределён
// new и delete, а здесь это не нужно)
void OLATrace(void* hTrace, const char* pszMsg);

void CActivation::OnRequestStarted()
{
	OLATrace(m_hTrace, "lic\tola - RequestStarted");
}

bool CActivation::OnServerAuthRequired(OnlineActivation::AuthToken& serverAuth)
{
	OLATrace(m_hTrace, "lic\tola - ServerAuthRequired");
	return false;
}

bool CActivation::OnProxyAuthRequired(OnlineActivation::AuthToken& proxyAuth)
{
	char user[1024], password[1024];
	if (!m_pActivationCallback->GetProxyCredentials(user, sizeof(user), password, sizeof(password)))
		return false;
	proxyAuth.SetCredentials(user, password);
	memset(password, 0, sizeof(password));
	return true;
}

void CActivation::OnResolvingName(const TCHAR* szName)
{
	OLATrace(m_hTrace, "lic\tola - ResolvingName");
}

void CActivation::OnNameResolved(const TCHAR* szIpAddr)
{
	OLATrace(m_hTrace, "lic\tola - NameResolved");
}

void CActivation::OnConnectingToServer(const TCHAR* szSockAddr)
{
	OLATrace(m_hTrace, "lic\tola - ConnectingToServer");
}

void CActivation::OnConnectedToServer(const TCHAR* szSockAddr)
{
	OLATrace(m_hTrace, "lic\tola - ConnectedToServer");
}

void CActivation::OnSendingRequest()
{
	OLATrace(m_hTrace, "lic\tola - SendingRequest");
	if (m_eState != Begin)
		return;
	if (m_pActivationCallback)
		m_pActivationCallback->OnSendingRequest();
	m_eState = SendingRequest; // пытаемся избежать прихода OnRequestComplete сразу, в случае авторизации на прокси
}

void CActivation::OnRequestSent(size_t nBytes)
{
	OLATrace(m_hTrace, "lic\tola - RequestSent");
	if (m_eState != SendingRequest)
		return;
	if (m_pActivationCallback)
		m_pActivationCallback->OnRequestSent(nBytes);
	m_eState = RequestSent;
}

void CActivation::OnReceivingResponse()
{
	OLATrace(m_hTrace, "lic\tola - ReceivingResponse");
}

void CActivation::OnResponseReceived(size_t nBytes)
{
	OLATrace(m_hTrace, "lic\tola - ResponseReceived");
}

void CActivation::OnClosingConnection()
{
	OLATrace(m_hTrace, "lic\tola - ClosingConnection");
}

void CActivation::OnConnectionClosed()
{
	OLATrace(m_hTrace, "lic\tola - ConnectionClosed");
}

void CActivation::OnDetectingProxy()
{
	OLATrace(m_hTrace, "lic\tola - DetectingProxy");
}

void CActivation::OnRequestCompleted()
{
	OLATrace(m_hTrace, "lic\tola - RequestComplete");
	if (m_eState == RequestSent)
	{
		if (m_pActivationCallback) m_pActivationCallback->OnRequestComplete();
		m_eState = RequestComplete;
	}
	SetEvent(m_evReqComplete);
}

void CActivation::OnRequestFailed(const ActivationError& error)
{
	OLATrace(m_hTrace, "lic\tola - RequestFailed");
	m_eState = RequestComplete;
	m_Result = Error;
	m_ErrorType = error.GetErrorType();
	m_ErrorCode = error.GetErrorCode();
	SetEvent(m_evReqComplete);
}

void CActivation::OnRedirect(const TCHAR* szUrl)
{
	OLATrace(m_hTrace, "lic\tola - Redirect");
}

void CActivation::OnIntermediateResponse()
{
	OLATrace(m_hTrace, "lic\tola - IntermediateResponse");
}

void CActivation::OnStateChange(unsigned long nFlags)
{
	OLATrace(m_hTrace, "lic\tola - StateChange");
}

void CActivation::OnCookieSent()
{
	OLATrace(m_hTrace, "lic\tola - CookieSent");
}

void CActivation::OnCookieReceived()
{
	OLATrace(m_hTrace, "lic\tola - CookieReceived");
}

void CActivation::OnWaitTimeout(unsigned long nTimeout)
{
	OLATrace(m_hTrace, "lic\tola - WaitTimeout");
}


bool CActivation::WaitResponse()
{
	HANDLE evs[2] = {m_evReqComplete, m_evStop};
	if (WaitForMultipleObjects(2, evs, FALSE, 5*60*1000) != WAIT_OBJECT_0)
	{
		m_pResponse->Cancel();
		m_Result = Canceled;
		return false;
	}
	if (m_Result == Error)
		return false;

	boost::shared_ptr<Content> content = m_pResponse->GetContent();
	if (!content)
		throw ServerError(ServerError::INTERNAL_SERVER_ERROR);

	switch (content->GetType())
	{
	case Content::CONTENT_HTML:
		{
			std::vector<wchar_t> buffer;
			int nLen = MultiByteToWideChar(CP_UTF8, 0, (const char*)content->GetData(), content->GetDataSize(), NULL, 0);
			if (nLen > 0)
			{
				buffer.resize(nLen);
				MultiByteToWideChar(CP_UTF8, 0, (const char*)content->GetData(), content->GetDataSize(), &buffer[0], buffer.size());
			}
			m_Result = HtmlForm;
			m_data.assign(
				(const char*)&buffer[0],
				(const char*)&buffer[0] + buffer.size()*sizeof(wchar_t));
		}
		return true;
	case Content::CONTENT_BINARY:
		{
			m_Result = KeyFile;
			m_data.assign(
				(const char*)content->GetData(),
				(const char*)content->GetData() + content->GetDataSize());
		}
		return false;
	default:
		throw ServerError(ServerError::INTERNAL_SERVER_ERROR); // Incorrect content type
	}
}