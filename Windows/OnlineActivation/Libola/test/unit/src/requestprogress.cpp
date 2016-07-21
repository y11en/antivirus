#include "StdAfx.h"
#include <string>
#include <sstream>

#include "OlaTest.h"
#include "OlaTestDlg.h"
#include "RequestProgress.h"

//-----------------------------------------------------------------------------

RequestProgress::RequestProgress(COlaTestDlg* pDialog) : m_pDialog(pDialog)
{
}

//-----------------------------------------------------------------------------
void RequestProgress::OnRequestStarted()
{
	m_pDialog->HandleRequestStarted();

}
//-----------------------------------------------------------------------------
void RequestProgress::OnResolvingName(const char* szName)
{
	std::string data;

	data.append("Resolving name: ");
	data.append(szName);

	m_pDialog->WriteProgressMsg(data.c_str());
}

//-----------------------------------------------------------------------------

void RequestProgress::OnNameResolved(const char* szIpAddr)
{
	std::string data;

	data.append("Name resolved: ");
	data.append(szIpAddr);

	m_pDialog->WriteProgressMsg(data.c_str());
}

//-----------------------------------------------------------------------------

void RequestProgress::OnConnectingToServer(const char* szSockAddr)
{
	std::string data;

	data.append("Connecting to server: ");
	data.append(szSockAddr);

	m_pDialog->WriteProgressMsg(data.c_str());
}

//-----------------------------------------------------------------------------

void RequestProgress::OnConnectedToServer(const char* szSockAddr)
{
	std::string data;

	data.append("Connected to server: ");
	data.append(szSockAddr);

	m_pDialog->WriteProgressMsg(data.c_str());
}

//-----------------------------------------------------------------------------

void RequestProgress::OnSendingRequest()
{
	m_pDialog->WriteProgressMsg("Sending request...");
}

//-----------------------------------------------------------------------------

void RequestProgress::OnRequestSent(size_t nBytes)
{
	std::ostringstream os;

	os << "Request sent. Bytes transmitted: " << nBytes;

	m_pDialog->WriteProgressMsg(os.str().c_str());
}

//-----------------------------------------------------------------------------

void RequestProgress::OnReceivingResponse()
{
	m_pDialog->WriteProgressMsg("Receiving response...");
}

//-----------------------------------------------------------------------------

void RequestProgress::OnResponseReceived(size_t nBytes)
{
	std::ostringstream os;

	os << "Response received. Bytes transmitted: " << nBytes;

	m_pDialog->WriteProgressMsg(os.str().c_str());
}

//-----------------------------------------------------------------------------

void RequestProgress::OnClosingConnection()
{
	m_pDialog->WriteProgressMsg("Closing connection...");
}

//-----------------------------------------------------------------------------

void RequestProgress::OnConnectionClosed()
{
	m_pDialog->WriteProgressMsg("Connection closed.");
}

//-----------------------------------------------------------------------------

void RequestProgress::OnDetectingProxy()
{
	m_pDialog->WriteProgressMsg("Proxy detected.");
}

//-----------------------------------------------------------------------------

void RequestProgress::OnRequestCompleted()
{
	m_pDialog->WriteProgressMsg("Request successfully completed.");
	m_pDialog->HandleRequestFinished(true);
}

//-----------------------------------------------------------------------------

void RequestProgress::OnRequestFailed(const OnlineActivation::ActivationError& error)
{
	std::ostringstream os;
	os	<< "Error during Request execution. Type: " << error.GetErrorType()
		<< " Code: " << error.GetErrorCode();
	m_pDialog->WriteProgressMsg(os.str().c_str());
	m_pDialog->HandleRequestFinished(false);
}

//-----------------------------------------------------------------------------

void RequestProgress::OnRedirect(const char* szUrl)
{
	std::string data;

	data.append("Redirecting to: ");
	data.append(szUrl);

	m_pDialog->WriteProgressMsg(data.c_str());
}

//-----------------------------------------------------------------------------

void RequestProgress::OnIntermediateResponse()
{
	m_pDialog->WriteProgressMsg("Intermediate response received.");
}

//-----------------------------------------------------------------------------

void RequestProgress::OnStateChange(unsigned long nFlags)
{
	std::ostringstream os;

	os << "State change detected. Flags: " << nFlags;

	m_pDialog->WriteProgressMsg(os.str().c_str());
}

//-----------------------------------------------------------------------------

void RequestProgress::OnCookieSent()
{
	m_pDialog->WriteProgressMsg("Cookie sent.");
}

//-----------------------------------------------------------------------------

void RequestProgress::OnCookieReceived()
{
	m_pDialog->WriteProgressMsg("Cookie received.");
}

//-----------------------------------------------------------------------------
void RequestProgress::OnWaitTimeout(unsigned long nTimeout)
{
	std::ostringstream os;
	os << "Waiting. Timeout: " << nTimeout;
	m_pDialog->WriteProgressMsg(os.str().c_str());
}
//-----------------------------------------------------------------------------
bool RequestProgress::OnServerAuthRequired(OnlineActivation::AuthToken& serverAuth)
{
	return false;
}
//-----------------------------------------------------------------------------
bool RequestProgress::OnProxyAuthRequired(OnlineActivation::AuthToken& proxyAuth)
{
	std::string user, pass;
	if (m_pDialog->GetProxyAuth(user, pass))
	{
		proxyAuth.SetCredentials(user.c_str(), pass.c_str());
		return true;
	}
	return false;
}
//-----------------------------------------------------------------------------
