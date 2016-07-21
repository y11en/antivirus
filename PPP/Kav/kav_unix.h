#if !defined(PPP_KAV50_H)
#define PPP_KAV50_H

#include <unix/tchar.h>
#include <pr_remote.h>
#include <plugin/p_tm.h>
#include <plugin/p_gui.h>
#include <plugin/p_win32_reg.h>
#include <plugin/p_dtreg.h>
#include <structs/s_gui.h>

#include "cmdln.h"

#define KAV_SERVICE_NAME            "AVP"
#define KAV_SERVICE_DISPLAY_NAME     KAV_SERVICE_NAME
#define KAV_SERVICE_DESCRIPTION     "Kaspersky Anti-Virus provides anti-virus services to applications and performs real-time protection of computer."

#ifndef RSP_SIMPLE_SERVICE
	#define RSP_SIMPLE_SERVICE 1
#endif
#ifndef RSP_UNREGISTER_SERVICE
	#define RSP_UNREGISTER_SERVICE 0
#endif

#define RELEASE_TRY
#define RELEASE_EXCEPT(a)

#define WP_GUI_SHOW 0

namespace AVP
{
	class cAVP
	{
	public://data
		tBOOL		     m_bService;
		tUINT		     m_nStartBlMode;
		tBOOL                m_bStartGui;
		tBOOL                m_bHost;
		tBOOL                m_bConsole;
		tBOOL                m_bShell;
		tBOOL                m_ShellRetCode;

		tBOOL                m_bStartSelfProtection;
		tBOOL                m_bCheckActivity;
		tDWORD               m_dwAffinityMask;
		
		tERROR              m_err;
		
		cRoot*              m_hRoot;
		cRegistry*          m_hReg;
		cRegistry*          m_hEnvironment;
		cMsgReceiver*       m_hMsgRcv;
		int                 m_hProductLock;
		int                 m_hBlLock;
		int                 m_hGuiLock;
		tUINT               m_WM_GUI;
		hPLUGIN             m_hPxstub;
		char*               m_pszGlobal;
		char*               m_pszLocal;

	public:	// construction
		cAVP();
        virtual ~cAVP();

		operator tERROR  () const { return (PR_FAIL(m_err) ? m_err : m_ShellRetCode) ; }
		operator hROOT   () const { return (hROOT)m_hRoot; }
		operator hOBJECT () const { return (hOBJECT)m_hRoot; }
		tBOOL                m_bTaskStarted;

		cTaskManager*       m_hTM;
		cObj*               m_hTMProxy;
		cTask*              m_hGUI;
		void*               m_ProtectionContext;
	public:	// overridables
		virtual void	Run(tDWORD, char**);
		virtual void	Shutdown();
		virtual void	Stop();
		virtual tBOOL	RegisterService(int argc, TCHAR ** argv);

		void RebootMessage();
        
		tERROR InitPrague(hROOT root, tBOOL remote);

		tBOOL   LoadSettings();
		tBOOL   SetServiceAcceptStop(tBOOL bAcceptStop);
		tBOOL   InitProductSettings();
		tBOOL   CheckProductActivity();
		tBOOL   CheckRestartTime(bool bReset, tDWORD dwType=0);

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
		tBOOL  PumpMessages();
		tBOOL  CreateTraceFile();
		tERROR GetEnvironmentString(hSTRING sName, hSTRING sValue);

		enum   tScope { enGlobal = 1, enLocal = 0 };

		int CreateMutex ( tBOOL bInitialOwner, const char* lpName, tScope scope );
		int OpenMutex ( const char* lpName, tScope scope );
		int CreateEvent ( tBOOL bManualReset, tBOOL bInitialState, const char* lpName, tScope scope );
		tERROR SetTracer(tFUNC_PTR TracerFunc);
	};
};

#endif // !defined(PPP_KAV50_H)
