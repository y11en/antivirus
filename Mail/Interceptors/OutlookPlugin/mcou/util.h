#ifndef __UTIL_H__
#define __UTIL_H__

#include <string>
#include <exchext.h>

namespace MCOU
{
	using namespace std;
	enum MCState
	{
		mcsNoRTP				= 0,	// WKS is not loaded or RTP is turned off
		mcsOPEnabledDuplex		= 1,	// RTP is on, OP is on, scan all mail
		mcsOPEnabledIncoming	= 2,	// RTP is on, OP is on, scan only incoming mail
		mcsOPDisabled			= 3		// RTP is on, OP is off
	};

	BOOL ProcessMessage(LPEXCHEXTCALLBACK lpeecb, BOOL bIsIncoming, BOOL bAsync, HRESULT &hrReturn);

	DWORD GetThisModuleFileName ( LPTSTR lpFilename, DWORD nSize );
	ULONG GetStoreSupportMask ( LPMESSAGE lpMessage );
	MCState CheckMCState ();

	bool DeleteTempFilesPrefix (LPCTSTR lpszPrefix, LPCTSTR lpszExtension = NULL);
	
	void StartTrace ();
	wstring GetFormatXString ( wstring &sName, wstring &sProto, wstring &sAddr);

	void CenterWindow(HWND hwnd, HWND hwndAlternateOwner = NULL);
	
	class CWaitCur
	{
	public:
		CWaitCur()
		{
			m_hcurPrev = ::SetCursor(::LoadCursor(NULL, IDC_WAIT));		
		}
		
		~CWaitCur()
		{
			::SetCursor(m_hcurPrev);
		}
		
	protected:
		HCURSOR m_hcurPrev;
	};
}

#endif