// TheBatAntispam.cpp : Defines the entry point for the DLL application.
//

#define PR_IMPEX_DEF

#include "stdafx.h"
#include "TheBatAntispam.h"
#include <boost/scoped_array.hpp>
#include <Prague/pr_compl.h>
#include <Prague/pr_types.h>
#include <Prague/pr_obj.h>
#include <Prague/plugin/p_string.h>
#include "../TheBatPlugin/PragueHelper.h"
#include "../TheBatPlugin/remote_mbl.h"
#include <Mail/iface/i_antispamfilter.h>
#include <Mail/structs/s_as.h>
#include <Mail/structs/s_as_trainsupport.h>
#include <Mail/plugin/p_mctask.h>
#include <Mail/structs/s_antispam.h>
#include "Register.h"
//#include "../OutlookPlugin/mcou_antispam/pmcMCOU_ANTISPAM.h"
#include <PPP/structs/s_gui.h>
#include <ProductCore/structs/s_profiles.h>
#include "CheckerThread.h"
//#include "../TheBatPlugin/Tracer.h"
#include "Tracing.h"
#include "../../Interceptors/TrafficMonitor2/TrafficProtocoller.h"
#include "version/ver_product.h"

HANDLE g_hModule = NULL;

cPrTracer g_Tracer;

namespace
{
	LPCSTR PLUGIN_NAME = "Kaspersky Anti-Spam plugin";
	PragueHelper::CAutoMsgReceiver g_hMsgReceiver;
	CCheckerThread g_CheckerThread;
}

#define _PLUGIN_VERSION2(a, b, c, d) #a"."#b"."#c"."#d
#define _PLUGIN_VERSION(a, b, c, d) _PLUGIN_VERSION2(a, b, c, d)
#define PLUGIN_VERSION _PLUGIN_VERSION(VER_PRODUCTVERSION_HIGH, VER_PRODUCTVERSION_LOW, VER_PRODUCTVERSION_BUILD, VER_PRODUCTVERSION_COMPILATION)

hOBJECT _RemoteMBLGet()
{
	hOBJECT hBL = RemoteMBLGet();
	if (g_hMsgReceiver && hBL)
	{
		tERROR err = g_hMsgReceiver->sysRegisterMsgHandler(pmc_ANTISPAM_ASK_PLUGIN, rmhDECIDER, hBL, 
			IID_ANY, PID_ANY, IID_ANY, PID_ANY);
//		if ( PR_SUCC(err) )
//			err = g_hMsgReceiver->sysRegisterMsgHandler(pmcMCOU_ANTISPAM, rmhDECIDER, hBL, 
//			IID_ANY, PID_ANY, IID_ANY, PID_ANY);
		if (PR_FAIL(err) && (err != -2147483439))
		{
			USES_CONVERSION;
			tstring msg(_T("sysRegisterMsgHandler() returned an error: "));
			msg += A2CT((boost::lexical_cast<std::string>(err)).c_str());
			ASTRACE(msg.c_str());
		}
	}

	return hBL;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			g_hModule = hModule;
			break;

		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

tERROR pr_call fMsgReceive( hOBJECT _this, tDWORD p_msg_cls_id, tDWORD p_msg_id, 
						   hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, 
						   tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	ASTRACE(_T("Entering fMsgReceive callback"));

	cERROR err = errOK;
	if ( 
//		p_msg_cls_id == pmcMCOU_ANTISPAM ||
		p_msg_cls_id == pmc_ANTISPAM_ASK_PLUGIN
		)
	{
		ASTRACE(_T("It's an antispam msg"));

		if ( p_par_buf_len == SER_SENDMSG_PSIZE )
		{
			ASTRACE(_T("buf_len = SER_SENDMSG_PSIZE"));
			
			if (p_par_buf)
			{
				tstring msg(((cSerializable*)p_par_buf)->isBasedOn(cAntispamAskPlugin::eIID) ? 
					_T("based on") : _T("not based on"));
				
				msg += _T(", ProcessID = ");
				msg += A2CT((boost::lexical_cast<std::string>(((cAntispamAskPlugin*)p_par_buf)->m_dwProcessID)).c_str());
				msg += _T(" (our ProcessID = ");
				msg += A2CT((boost::lexical_cast<std::string>(GetCurrentProcessId())).c_str());
				msg += _T("), proto = ");
				msg += A2CT((boost::lexical_cast<std::string>(((cAntispamAskPlugin*)p_par_buf)->m_dwProtocolType)).c_str());
				ASTRACE(msg.c_str());
			}
			
			//////////////////////////////////////////////////////////////////////////
			//
			// IMAP4 не обрабатывается в плагинах TheBat! - это ошибка RitLab. 
			// Поэтому оставляем его на обработку в трафикМониторе
			//
			if (((cSerializable*)p_par_buf)->isBasedOn(cAntispamAskPlugin::eIID) &&
				((cAntispamAskPlugin*)p_par_buf)->m_dwProcessID == GetCurrentProcessId())
			{
				if (((cAntispamAskPlugin*)p_par_buf)->m_dwProtocolType != TrafficProtocoller::po_IMAP4)
					err = errOK_DECIDED;
				else
					err = errOK;
			}

			//
			// IMAP4 не обрабатывается в плагинах TheBat! - это ошибка RitLab. 
			// Поэтому оставляем его на обработку в трафикМониторе
			//
			//////////////////////////////////////////////////////////////////////////
		}
		else
		{
			tDWORD dwPID = p_par_buf ? *(tDWORD*)p_par_buf : 0;
			if (dwPID == GetCurrentProcessId())
			{
				// Если файл качается нашим процессом - возвращаем errOK_DECIDED
				ASTRACE(_T("It's our PID"));
				err = errOK_DECIDED;
			}
		}
	}

	USES_CONVERSION;
	tstring msg(_T("Returning value: "));
	msg += A2CT((boost::lexical_cast<std::string>(err)).c_str());
	ASTRACE(msg.c_str());

	return err;
}

THEBATANTISPAM_API void WINAPI TBP_Initialize()
{
	try
	{
		//CTracer::Initialize("tb_as");

		PLUGIN_LIST the_list;
		PRAGUE_ASSERT(RemoteMBLInit(static_cast<HMODULE>(g_hModule), the_list));

		g_Tracer.Init(NULL, false, "tb_as");

		ASTRACE(_T(""));
		ASTRACE(_T("---------------------  TheBat! Antispam Plugin started --------"));
		tstring strName(_T(PLUGIN_NAME));
		strName += _T(", ver.");
		strName += _T(PLUGIN_VERSION);
		ASTRACE(strName.c_str());	

		//if (g_root)
		//{
		//	// Устанавливаем трасировку
		//	g_root->propSetDWord( pgOUTPUT_FUNC, (tDWORD)&CTracer::Trace);
		//}

		PRAGUE_ASSERT(g_root->RegisterCustomPropId(&g_propMessageIsIncoming, npMESSAGE_IS_INCOMING, pTYPE_BOOL | pCUSTOM_HERITABLE));
		PRAGUE_ASSERT(g_root->RegisterCustomPropId(&g_propMailMessageOS_PID, npMAILMESSAGE_OS_PID, pTYPE_DWORD | pCUSTOM_HERITABLE));
		PRAGUE_ASSERT(g_root->RegisterCustomPropId(&g_propMailerPID, npMAILER_PID, pTYPE_DWORD | pCUSTOM_HERITABLE));

		PRAGUE_ASSERT(g_root->sysCreateObject(reinterpret_cast<hOBJECT*>(g_hMsgReceiver.get()), IID_MSG_RECEIVER));
		PRAGUE_ASSERT(g_hMsgReceiver->set_pgRECEIVE_PROCEDURE((tFUNC_PTR) fMsgReceive));
		PRAGUE_ASSERT(g_hMsgReceiver->sysCreateObjectDone());

		g_CheckerThread.Start();

		ASTRACE(_T("Initialization completed successfully"));
		ASTRACE(_T(""));
	}
	catch (std::exception& ex)
	{
		// probably some Prague error
		ASTRACE(ex.what());
	}
	catch (...)
	{
		// all other errors
		ASTRACE_EX(_T("Unhandled exception"));
	}
}

THEBATANTISPAM_API void WINAPI TBP_Finalize()
{
	try
	{
		ASTRACE(_T("Deinitializing plugin..."));
		g_hMsgReceiver = NULL;
		g_CheckerThread.Stop();

		//if (g_root)
		//{
		//	g_root->propSetDWord(pgOUTPUT_FUNC, NULL);
		//}

		RemoteMBLTerm();
		ASTRACE(_T("---------------------  session end ----------------------------"));
		ASTRACE(_T(""));
	}
	catch (...) {}

	ASTRACE(_T(""));

	g_Tracer.Deinit();
	//CTracer::UnInitialize();
}

THEBATANTISPAM_API int WINAPI TBP_GetName(char* lpDest, int nBufSize)
{
	if (lpDest && nBufSize > 0)
		strncpy_s(lpDest, nBufSize, PLUGIN_NAME, _TRUNCATE);
	return (int)strlen(PLUGIN_NAME) + 1;
}

THEBATANTISPAM_API int WINAPI TBP_GetVersion(char* lpDest, int nBufSize)
{
	if (lpDest && nBufSize > 0)
		strncpy_s(lpDest, nBufSize, PLUGIN_VERSION, _TRUNCATE);
	return (int)strlen(PLUGIN_VERSION) + 1;
}

THEBATANTISPAM_API int WINAPI TBP_GetStatus()
{
	try
	{
		hOBJECT hBL = _RemoteMBLGet();
		if (hBL)
		{
			tERROR err = hBL->sysSendMsg(pmc_ANTISPAM_PROCESS, NULL, NULL, NULL, NULL);
			if (err == errOK_DECIDED)
				return TheBat::AVC_OK;
		}
	}
	catch (...)
	{}
	
	return TheBat::AVC_OK;
}

THEBATANTISPAM_API int WINAPI TBP_GetSpamScore(int MsgID, TBPGetDataProc GetData)
{
	if (!GetData)
	{
		ASTRACE("GetSpamScore error - empty GetDataProc was passed");
		return TheBat::AVC_Error;
	}
	
	int nRet = -1;	// was not processed (by default)

	USES_CONVERSION;
	tstring msg(_T("Processing msg (GetSpamScore) "));
	msg += A2CT((boost::lexical_cast<std::string>(MsgID)).c_str());
	ASTRACE(msg.c_str());

	try
	{
		// try to get BL
		hOBJECT hWorker = _RemoteMBLGet();
		if (!hWorker)
			throw std::runtime_error("Couldn't get remote MBL");

		// get the needed buffer size
		int nBufSize = GetData(MsgID, TheBat::mpidRawMessage, NULL, 0);
		msg = _T("Bytes count: ");
		msg += A2CT((boost::lexical_cast<tstring>(nBufSize)).c_str());
		
		// get the raw message body
		boost::scoped_array<char> buf(new char[nBufSize]);	
		int nRes = GetData(MsgID, TheBat::mpidRawMessage, buf.get(), nBufSize);
		if (nRes < 0)
		{
			ASTRACE(_T("Failed to get the raw message body"));
			return TheBat::AVC_Error;
		}

		// create TempIO for our buffer
		PragueHelper::CAutoIO hTempIO;
		PRAGUE_ASSERT(hWorker->sysCreateObjectQuick((hOBJECT*)&hTempIO, IID_IO, PID_TMPFILE));

		tDWORD dwWritten = 0;
		PRAGUE_ASSERT(hTempIO->SeekWrite(&dwWritten, 0, buf.get(), nBufSize));
		if (dwWritten <= 0)
			throw std::runtime_error("Couldn't write enough chars to continue");

		// set the parameters and call MC task
		PRAGUE_ASSERT(hTempIO->set_pgOBJECT_ORIGIN(OID_MAIL_MSG_MIME));
		PRAGUE_ASSERT(hTempIO->propSetDWord(g_propMailMessageOS_PID, PID_MSOE));
		PRAGUE_ASSERT(hTempIO->propSetBool(g_propMessageIsIncoming, cTRUE));
		PRAGUE_ASSERT(hTempIO->propSetDWord(g_propMailerPID, ::GetCurrentProcessId()));

		cMCProcessParams params;
		PRAGUE_ASSERT(hWorker->sysSendMsg(pmc_ANTISPAM_PROCESS, NULL, hTempIO, &params, SER_SENDMSG_PSIZE));

		if ((params.p_MailCheckerVerdicts & mcv_MESSAGE_DELETED) == mcv_MESSAGE_DELETED)
		{
			ASTRACE(_T("The IO was deleted in MC, detaching..."));
			hTempIO.Detach();	// it was closed in MC
		}

		if ((params.p_MailCheckerVerdicts & mcv_MESSAGE_CHECKED) != mcv_MESSAGE_CHECKED)
			throw std::runtime_error("The message was rejected by MC");

		// process the result from MC
		nRet = 0;	// not a spam
		if ((params.p_MailCheckerVerdicts & mcv_MESSAGE_AS_SPAM) == mcv_MESSAGE_AS_SPAM)
			nRet = 100;	// spam
		else if ((params.p_MailCheckerVerdicts & mcv_MESSAGE_AS_PROBABLE_SPAM) == mcv_MESSAGE_AS_PROBABLE_SPAM)
			nRet = 50;	// probably a spam

		msg = _T("The message was processed successfully, ret code = ");
		msg += A2CT((boost::lexical_cast<std::string>(nRet)).c_str());
		ASTRACE(msg.c_str());
	}
	catch (std::exception& ex)
	{
		// probably some Prague error
		ASTRACE(ex.what());
		nRet = -1;	// was not processed
	}
	catch (...)
	{
		// all other errors
		ASTRACE(_T("Unhandled exception"));
		nRet = -1;	// was not processed
	}

	ASTRACE(_T(""));
	return nRet;
}

THEBATANTISPAM_API int WINAPI TBP_FeedSpam(int MsgID, int IsSpam, TBPGetDataProc GetData)
{
	if (!GetData)
	{
		ASTRACE("FeedSpam error - empty GetDataProc was passed");
		return TheBat::AVC_Error;
	}

	USES_CONVERSION;
	tstring msg(_T("Processing msg (FeedSpam) "));
	msg += A2CT((boost::lexical_cast<std::string>(MsgID)).c_str());
	ASTRACE(msg.c_str());

	TheBat::AVC_RetCodes nRet = TheBat::AVC_OK;

	try
	{
		// try to get BL
		hOBJECT hWorker = _RemoteMBLGet();
		if (!hWorker)
			throw std::runtime_error("Couldn't get remote MBL");
		
		// get the needed buffer size
		int nBufSize = GetData(MsgID, TheBat::mpidRawMessage, NULL, 0);
		
		// get the raw message body
		boost::scoped_array<char> buf(new char[nBufSize]);	
		int nRes = GetData(MsgID, TheBat::mpidRawMessage, buf.get(), nBufSize);
		if (nRes < 0)
		{
			ASTRACE(_T("Failed to get the raw message body"));
			return TheBat::AVC_Error;
		}
		
		// create TempIO for our buffer
		PragueHelper::CAutoIO hTempIO;
		PRAGUE_ASSERT(hWorker->sysCreateObjectQuick( (hOBJECT*)&hTempIO, IID_IO, PID_TMPFILE));
		
		tDWORD dwWritten = 0;
		PRAGUE_ASSERT(hTempIO->SeekWrite(&dwWritten, 0, buf.get(), nBufSize));
		if (dwWritten <= 0)
			throw std::runtime_error("Couldn't write enough chars to continue");
		
		// set the parameters and call MC task
		PRAGUE_ASSERT(hTempIO->set_pgOBJECT_ORIGIN(OID_MAIL_MSG_MIME));
		PRAGUE_ASSERT(hTempIO->propSetDWord(g_propMailMessageOS_PID, PID_MSOE));
		PRAGUE_ASSERT(hTempIO->propSetBool(g_propMessageIsIncoming, cTRUE));
		PRAGUE_ASSERT(hTempIO->propSetDWord(g_propMailerPID, ::GetCurrentProcessId()));
		
		cAntispamTrainParams params;
		params.p_Action = IsSpam ? cAntispamTrainParams::TrainAsSpam : cAntispamTrainParams::TrainAsHam;

		msg = _T("Training as ");
		msg += (IsSpam ? _T("spam") : _T("ham"));
		ASTRACE(msg.c_str());

		PRAGUE_ASSERT(hWorker->sysSendMsg(pmc_ANTISPAM_TRAIN, NULL, hTempIO, &params, SER_SENDMSG_PSIZE));

		ASTRACE(_T("The message was processed successfully"));
	}
	catch (std::exception& ex)
	{
		// probably some Prague error
		ASTRACE(ex.what());
		nRet = TheBat::AVC_Error;	// was not processed
	}
	catch (...)
	{
		// all other errors
		ASTRACE(_T("Unhandled exception"));
		nRet = TheBat::AVC_Error;	// was not processed
	}

	ASTRACE(_T(""));
	return nRet;
}

//------------------ Plugin registration in TheBat! ---------------------------
THEBATANTISPAM_API int _stdcall Register(DWORD, DWORD)
{
	ASTRACE(_T("---------------------  Registering plugin... -------------------"));
	
	TCHAR szPath[MAX_PATH];
	if (!GetModuleFileName(reinterpret_cast<HMODULE>(g_hModule), szPath, MAX_PATH))
		return errUNEXPECTED;
	
	int nRet = CRegister::RegisterASPlugin(szPath);

	USES_CONVERSION;
	tstring msg(_T("---------------------  Done registering, ret code: "));
	msg += A2CT((boost::lexical_cast<std::string>(nRet)).c_str());
	ASTRACE(msg.c_str());
	ASTRACE(_T(""));

	return nRet;
}

THEBATANTISPAM_API int _stdcall Unregister(DWORD, DWORD)
{
	ASTRACE(_T("---------------------  Unregistering plugin... -----------------"));
	
	TCHAR szPath[MAX_PATH];
	if (!GetModuleFileName(reinterpret_cast<HMODULE>(g_hModule), szPath, MAX_PATH))
		return errUNEXPECTED;
	
	int nRet = CRegister::UnregisterASPlugin(szPath);

	USES_CONVERSION;
	tstring msg(_T("---------------------  Done unregistering, ret code: "));
	msg += A2CT((boost::lexical_cast<std::string>(nRet)).c_str());
	ASTRACE(msg.c_str());
	ASTRACE(_T(""));

	return nRet;
}

THEBATANTISPAM_API int _stdcall CleanRegFlag()
{
	ASTRACE(_T("---------------------  Clean Reg Flag... -----------------"));

	TCHAR szPath[MAX_PATH];
	if (!GetModuleFileName(reinterpret_cast<HMODULE>(g_hModule), szPath, MAX_PATH))
		return errUNEXPECTED;

	int nRet = CRegister::CleanRegistrationFlag(szPath);

	USES_CONVERSION;
	tstring msg(_T("---------------------  Done cleaning, ret code: "));
	msg += A2CT((boost::lexical_cast<std::string>(nRet)).c_str());
	ASTRACE(msg.c_str());
	ASTRACE(_T(""));

	return nRet;
}

//////////////////////////////////////////////////////////////////////////

//
// Return values:
// 0 - the Plug-in cannot be configured within The Bat!
// Non-zero - the Plug-in can be configured within The Bat!
THEBATANTISPAM_API int WINAPI TBP_NeedConfig()
{
	return 1;
}

THEBATANTISPAM_API int WINAPI TBP_Setup()
{
	hTASKMANAGER pTM = (hTASKMANAGER) _RemoteMBLGet();
	if (!pTM)
	{
		ASTRACE(_T("Cannot show the settings dialog - couldn't get remote BL"));
		return TheBat::AVC_OK;
	}

	tERROR err = pTM->AskAction(TASKID_TM_ITSELF, cAskGuiAction::SHOW_SETTINGS_WND, 
		&cProfileAction(AVP_PROFILE_ANTISPAM));

	if (PR_FAIL(err))
	{
		USES_CONVERSION;
		tstring msg(_T("Failed to call AskAction: "));
		msg += A2CT((boost::lexical_cast<std::string>(err)).c_str());
		ASTRACE(msg.c_str());
		return TheBat::AVC_OK;
	}

	ASTRACE(_T("Settings dialog has been successfully shown"));
	return TheBat::AVC_OK;
}
