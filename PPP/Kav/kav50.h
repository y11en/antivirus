#if !defined(PPP_KAV50_H)
#define PPP_KAV50_H

#include <pr_remote.h>
#include <plugin/p_tm.h>
#include <plugin/p_gui.h>
#include <plugin/p_win32_reg.h>
#include <plugin/p_dtreg.h>
#include <structs/s_gui.h>

#include "ntservice.h"
#include "cmdln.h"
#include "ver_mod.h"
#include "service/sa.h"

#define KAV_SERVICE_NAME            "AVP"
#define KAV_SERVICE_DISPLAY_NAME     KAV_SERVICE_NAME
#define KAV_SERVICE_DESCRIPTION     "Kaspersky Anti-Virus provides anti-virus services to applications and performs real-time protection of computer."

#ifndef RSP_SIMPLE_SERVICE
	#define RSP_SIMPLE_SERVICE 1
#endif
#ifndef RSP_UNREGISTER_SERVICE
	#define RSP_UNREGISTER_SERVICE 0
#endif

#ifdef _DEBUG
#define RELEASE_TRY
#define RELEASE_EXCEPT(a)
#else
#define RELEASE_TRY         __try
#define RELEASE_EXCEPT(a)   __except(a)
#endif

#define WP_GUI_SHOW 0

namespace AVP
{
	typedef BOOL (__cdecl* UshataFunc)( BOOL );
	typedef BOOL (__cdecl* UshataFunc)( BOOL );
	typedef VOID (WINAPI *fnSetClientVerdict)(VERDICT Verdict);

	HANDLE				m_hStop;

	class CMutexLock
	{
	public:
		CMutexLock(HANDLE hMutex) : m_h(hMutex)
		{
			::WaitForSingleObject(m_h, INFINITE);
		}
		~CMutexLock()
		{
			::ReleaseMutex(m_h);
		}
	private:
		HANDLE m_h;
	};

//	int __cdecl ReportHook(int nReportType, char* szaMessage, int* pnReturnValue);
//	void SetupServiceDirectory();
	
	class cAVP : public CNTService
	{
	public://data
		BOOL				m_bService;
		UINT				m_nStartBlMode;
		BOOL				m_bStartGui;
		BOOL                m_bHost;
		BOOL                m_bConsole;
		BOOL                m_bShell;
		tINT                m_ShellRetCode;
		BOOL                m_bDeinstallation;

		BOOL                m_bStartSelfProtection;
        BOOL                m_bChangeSelfProtectionState;
		BOOL                m_bCheckActivity;
		DWORD               m_dwAffinityMask;
		
		tERROR              m_err;
		
		cRoot*              m_hRoot;
		cRegistry*          m_hReg;
		cRegistry*          m_hEnvironment;
		cMsgReceiver*       m_hMsgRcv;
		HANDLE              m_hProductMutex;
		HANDLE              m_hBlMutex;
		HANDLE              m_hGuiMutex;
		HANDLE              m_hRunMutex;
		HINSTANCE           m_hAvpgHook;
		HINSTANCE           m_hClldrHook;
		HSA			    	m_hSecure;
		UINT                m_WM_GUI;
		hPLUGIN             m_hPxstub;
		char*               m_pszGlobal;
		char*               m_pszLocal;
		HWND                m_hWnd;
		UshataFunc          m_ushata;

	public:	// construction
		cAVP();
        virtual ~cAVP();

		operator tERROR  () const { return (PR_FAIL(m_err) ? m_err : m_ShellRetCode) ; }
		operator hROOT   () const { return (hROOT)m_hRoot; }
		operator hOBJECT () const { return (hOBJECT)m_hRoot; }
		BOOL                m_bTaskStarted;

		cTaskManager*       m_hTM;
		cObj*               m_hTMProxy;
		cTask*              m_hGUI;
		PVOID				m_ProtectionContext;
	public:	// overridables
		virtual void	Run(DWORD, LPTSTR *);
		virtual void	Shutdown();
		virtual void	Stop();
		virtual BOOL	RegisterService(int argc, TCHAR ** argv);
		virtual BOOL	DebugService(int argc, TCHAR **argv,BOOL faceless);
		virtual BOOL	RemoveServiceEx();

		VOID RebootMessage();
        
        LRESULT         OnMessage(
			HWND      hwnd,
			UINT      uMsg,
			WPARAM    wParam,
			LPARAM    lParam,
			bool&     bHandled);

		tERROR InitPrague(hROOT root, tBOOL remote);

		BOOL   LoadSettings();
		BOOL   SetServiceAcceptStop(BOOL bAcceptStop);
		BOOL   InitProductSettings();
		BOOL   CheckProductActivity();
		BOOL   CheckRestartTime(bool bReset, DWORD dwType=0);
		tERROR RestartGui();

	protected:// internal
		void   PrintHelp();
		tERROR LoadPlugins(tCHAR* folder);
		tERROR CreateGUI();
		tERROR CreateBL();
		tERROR CreateConfig();
		tERROR DestroyConfig();
		tERROR DestroyGUI();
		tERROR DestroyBL();
		tERROR CleanupOnExit();
		void   WaitForRunComplete();
		tBOOL  PumpMessages();
		tBOOL  CreateTraceFile();
		tERROR GetEnvironmentString(hSTRING sName, hSTRING sValue);
		tBOOL  RegisterInOSRestartManager();
		tERROR RegisterTheBatPlugins();

		enum   tScope { enGlobal = 1, enLocal = 0 };

		HANDLE CreateMutex ( BOOL bInitialOwner, const char* lpName, tScope scope );
		HANDLE OpenMutex ( const char* lpName, tScope scope );
		HANDLE CreateEvent ( BOOL bManualReset, BOOL bInitialState, const char* lpName, tScope scope );
		tERROR SetTracer(tFUNC_PTR TracerFunc);

	private:
		BOOL m_bNeedToFreeServiceName;
	};
};

#endif // !defined(PPP_KAV50_H)
