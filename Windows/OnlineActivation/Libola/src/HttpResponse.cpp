#include "stdafx.h"
#include <algorithm>
#include <vector>
#include <tchar.h>
#include "../include/ActivationErrors.h"
#include "WinInternet.h"
#include "HttpResponse.h"

namespace OnlineActivation
{
///////////////////////////////////////////////////////////////////////////////
HttpResponse::HttpResponse(InternetHandle& hResponse) 
	: m_hResponse(hResponse)
{
	// allocate 4K buffer
	std::vector<unsigned char> buf(4096);
	DWORD dwBytesRead = 0;
	// sequentially read data
	while (dwBytesRead = WinInternet::ReadData(m_hResponse, &buf[0], buf.size()))
		m_data.insert(m_data.end(), buf.begin(), buf.begin() + dwBytesRead);
}
///////////////////////////////////////////////////////////////////////////////
unsigned short HttpResponse::GetStatusCode()
{
	DWORD dwStatusCode = 0;
	DWORD dwStatusLen = sizeof(dwStatusCode);

	return static_cast<unsigned short>
		(WinInternet::QueryStatusCode(m_hResponse));
}
///////////////////////////////////////////////////////////////////////////////
std::string HttpResponse::GetContentType()
{
	DWORD dwSize = 0;
	std::vector<char> buf;
	//query required buffer size
	if(!WinInternet::QueryContentType(m_hResponse, static_cast<char*>(0), &dwSize))
	{
		switch(DWORD dwError = GetLastError())
		{
		// expected case - resize buffer and query again
		case ERROR_INSUFFICIENT_BUFFER:
			// resize buffer to required size
			buf.resize(dwSize / sizeof(char));
			// query information again
			if(!WinInternet::QueryContentType(m_hResponse, &buf[0], &dwSize))
				throw SystemError(dwError);
			break;
		// content-length header is not found
		case ERROR_HTTP_HEADER_NOT_FOUND:
			// leave the buffer empty (unknown content type)
			break;
		// other errors
		default:
			throw SystemError(dwError);
			break;
		}
	}
	// find media-type parameter delimiter
	std::vector<char>::iterator end = std::find(buf.begin(), buf.begin() + dwSize, ';');
	// convert to lowercase
	std::transform(buf.begin(), end, buf.begin(), std::ptr_fun(tolower));
	// compose content type string
	return std::string(buf.begin(), end);
}
///////////////////////////////////////////////////////////////////////////////
const void* HttpResponse::GetData() const
{
	return m_data.empty() ? 0 : &m_data[0];
}
///////////////////////////////////////////////////////////////////////////////
size_t HttpResponse::GetDataSize() const
{
	return m_data.size();
}
///////////////////////////////////////////////////////////////////////////////
} // namespace OnlineActivation
