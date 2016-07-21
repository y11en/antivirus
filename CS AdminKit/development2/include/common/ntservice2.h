#ifndef __KL_NTSERVICE2_H__
#define __KL_NTSERVICE2_H__

#include <std/base/klstd.h>
#include <common/reporter.h>
#include <common/measurer.h>
#include <wctype.h>

#ifdef _WIN32
#include <common/ntservice.h>
#endif

#ifdef _WIN32
#define TEXT_T(text)    _T(##text)
#else
#define DWORD           unsigned long
#define TEXT_T(text)    L##text
#define TCHAR           wchar_t
#define BOOL            bool
#define TRUE            true
#define FALSE           false
#define LPCTSTR         const TCHAR*
#define LPTSTR          TCHAR*
#define LPSTR           char*
#define LPCSTR          const char*
#endif

#define NTSRVC2_CMDLN_INSTALL   TEXT_T("i")
#define NTSRVC2_CMDLN_DEBUG     TEXT_T("d")
#define NTSRVC2_CMDLN_WINAPP    TEXT_T("w")
#define NTSRVC2_CMDLN_TCW       TEXT_T("f")
#define NTSRVC2_CMDLN_LOGIN     TEXT_T("l")
#define NTSRVC2_CMDLN_PASSWORD  TEXT_T("p")
#define NTSRVC2_CMDLN_UNINSTALL TEXT_T("u")
#define NTSRVC2_CMDLN_START     TEXT_T("s")
#define NTSRVC2_CMDLN_STOP      TEXT_T("e")
#define NTSRVC2_CMDLN_REGSERVER TEXT_T("regserver")
#define NTSRVC2_CMDLN_URGSERVER TEXT_T("unregserver")
#define NTSRVC2_CMDLN_UPGRADE   TEXT_T("upgrade")

#ifdef _WIN32
#define NTSRVC2_WAIT_FLAGS  \
    (QS_ALLEVENTS|QS_ALLINPUT|QS_ALLPOSTMESSAGE|QS_HOTKEY|QS_SENDMESSAGE|QS_TIMER)
#endif

namespace KLSTD
{
    const size_t c_nMaxServiceNameLen = 256;

    class CNTService2
        :   public KLSTD::Reporter
#ifdef _WIN32
        ,   public CNTService
#endif
	{
	public:	// construction
		CNTService2(LPCTSTR ServiceName, LPCTSTR DisplayName);
        virtual ~CNTService2();
	public:	// overridables
		virtual BOOL	RegisterService(int argc, TCHAR ** argv);
        virtual void	Stop() = 0;

        //just now called under unices only 
        virtual void	Reload(){;};
    protected:
        virtual void	Run(DWORD argc, LPTSTR * argv) = 0;
        virtual BOOL    OnRegServer() = 0;
        virtual BOOL    OnUnregServer() = 0;
        virtual BOOL    OnUpgradeServer(){return FALSE;};
        virtual void    OnShuttingDown() = 0;        

        virtual void AddToMessageLog2(
                        const wchar_t*  szwMessage, 
                        WORD            wEventType, 
                        DWORD           dwEventID,
                        const void*     pRawData,
                        size_t          nRawData);
//KLSTD::Reporter
	public: 
		KLSTD_NOTHROW virtual void ReportError(const wchar_t*	szwMessage) throw();
		KLSTD_NOTHROW virtual void ReportError(const char*		szaMessage) throw();
        KLSTD_NOTHROW virtual void ReportInfo(const wchar_t*    szwMessage) throw();
        KLSTD_NOTHROW virtual void ReportInfo(const char*       szaMessage) throw();
        KLSTD_NOTHROW virtual void ReportWarning(const wchar_t* szwMessage) throw();
        KLSTD_NOTHROW virtual void ReportWarning(const char*    szaMessage) throw();

        KLSTD_NOTHROW virtual void DoReport(
                        const wchar_t*  szwMessage, 
                        WORD            wEventType = EVENTLOG_ERROR_TYPE, 
                        DWORD           dwEventID = DWORD(-1)) throw();
	protected:
#ifdef _WIN32
		virtual BOOL	DebugService(int argc, TCHAR **argv, BOOL faceless);
        BOOL PumpMessages();
        static LRESULT CALLBACK WndProc(
                            HWND      hwnd,
                            UINT      uMsg,
                            WPARAM    wParam,
                            LPARAM    lParam);

        LRESULT OnMessage(  HWND      hwnd,
                            UINT      uMsg,
                            WPARAM    wParam,
                            LPARAM    lParam,
                            bool&     bHandled);

        KLSTD_NOTHROW void ProcessEndSessionShutdown() throw();
#endif
	protected:
		TCHAR				m_szServiceName[c_nMaxServiceNameLen];
		TCHAR				m_szDisplayName[c_nMaxServiceNameLen];
        std::string         m_strServiceName;
    protected:
		BOOL				m_bIsService, m_bShowConsole;
	};
};
#endif //__KL_NTSERVICE2_H__
