// TheBatPlugin.cpp : Defines the entry point for the DLL application.
//

#define PR_IMPEX_DEF

#include "stdafx.h"

#include <Prague/plugin/p_string.h>
#include <Mail/plugin/p_mctask.h>
#include <Mail/structs/s_mc.h>
#include <ProductCore/structs/s_profiles.h>
#include <PPP/structs/s_gui.h>

#include "TheBatPlugin.h"
#include "Constants.h"
#include "remote_mbl.h"
#include "PragueHelper.h"
#include "ComStreamIo/p_comstream.h"
#include "Register.h"
#include "Tracing.h"
#include <boost/scoped_array.hpp>
#include <atlconv.h>
#include <comdef.h>
#include "FSDrv.h"
#include "version/ver_product.h"

HANDLE g_hModule = NULL;

cPrTracer	g_Tracer;

namespace
{
	LPCSTR PLUGIN_NAME = "Kaspersky Anti-Virus plugin";
}

#define _PLUGIN_VERSION2(a, b, c, d) #a"."#b"."#c"."#d
#define _PLUGIN_VERSION(a, b, c, d) _PLUGIN_VERSION2(a, b, c, d)
#define PLUGIN_VERSION _PLUGIN_VERSION(VER_PRODUCTVERSION_HIGH, VER_PRODUCTVERSION_LOW, VER_PRODUCTVERSION_BUILD, VER_PRODUCTVERSION_COMPILATION)

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
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

/*----------------------BAV_Initialize---------------------------
Called on installing the plugin.
If an error occurs and is critical to the plugin's functionality
then BAV_GetStatus() must return an error code.
---------------------------------------------------------------*/
THEBATPLUGIN_API int WINAPI BAV_Initialize()
{
	try
	{
		//CTracer::Initialize("tb_av");

		

		USES_CONVERSION;
		std::string strStreamIOppl = T2A(const_cast<LPTSTR>(PragueHelper::GetProductPath().c_str()));
		strStreamIOppl += "ComStmIO.ppl";
		PLUGIN_LIST the_list;
		the_list.push_back(strStreamIOppl);
		
		PRAGUE_ASSERT(RemoteMBLInit(reinterpret_cast<HMODULE>(g_hModule), the_list));

		g_Tracer.Init(NULL, false, "tb_av");

		AVTRACE(_T(""));
		AVTRACE(_T("---------------------  TheBat! Antivirus Plugin started --------"));
		tstring strName(_T(PLUGIN_NAME));
		strName += _T(", ver.");
		strName += _T(PLUGIN_VERSION);
		AVTRACE(strName.c_str());

		//if (g_root)
		//{		
		//	g_root->propSetDWord(pgOUTPUT_FUNC, (tDWORD)&CTracer::Trace);
		//}

		PRAGUE_ASSERT(g_root->RegisterCustomPropId(&g_propMailerPID, npMAILER_PID, pTYPE_DWORD | pCUSTOM_HERITABLE));
		PRAGUE_ASSERT(g_root->RegisterCustomPropId(&g_propMessageIsIncoming, npMESSAGE_IS_INCOMING, pTYPE_BOOL | pCUSTOM_HERITABLE));
		PRAGUE_ASSERT(g_root->RegisterCustomPropId(&g_propCheckOnly, npMESSAGE_CHECK_ONLY, pTYPE_DWORD | pCUSTOM_HERITABLE));
		PRAGUE_ASSERT(g_root->RegisterCustomPropId(&g_propVirtualName, npENGINE_VIRTUAL_OBJECT_NAME, pTYPE_STRING));

		AVTRACE(_T("Initialization completed successfully"));
		AVTRACE(_T(""));
	}
	catch (std::exception& ex)
	{
		// probably some Prague error
		AVTRACE(ex.what());
		return TheBat::AVC_OK;
	}
	catch (...)
	{
		// all other errors
		AVTRACE_EX(_T("Unhandled exception"));
		return TheBat::AVC_OK;
	}
	
	return TheBat::AVC_OK;
}

/*----------------------BAV_Uninitialize-------------------------
Called on deleting the plugin.
---------------------------------------------------------------*/
THEBATPLUGIN_API int WINAPI BAV_Uninitialize()
{
	try
	{
		AVTRACE(_T("Deinitializing plugin..."));

		if (g_root)
		{
			g_root->propSetDWord( pgOUTPUT_FUNC, NULL);
		}

		RemoteMBLTerm();
		AVTRACE(_T("---------------------  session end -----------------------------"));
		AVTRACE(_T(""));
	}
	catch (...) {}

	AVTRACE(_T(""));
	//CTracer::UnInitialize();
	g_Tracer.Deinit();
	return TheBat::AVC_OK;
}

/*----------------------BAV_GetName------------------------------
Set the plugin name into the dest buffer.
This function uses the same format as other function that pass a string and size:
if the dest is NULL or size is 0,
just return length of the string (aka name),
otherwise copy size bytes of the source string into dest.
In either case, return the length of the plugin name.
TBSGetString is inlined inside of this header file for you to use.
---------------------------------------------------------------*/
THEBATPLUGIN_API int WINAPI BAV_GetName(char* lpDest, int nBufSize)
{
	if (lpDest && nBufSize > 0)
		strncpy_s(lpDest, nBufSize, PLUGIN_NAME, _TRUNCATE);
	return (int)strlen(PLUGIN_NAME) + 1;
}

/*----------------------BAV_GetVersion---------------------------
Return the version string.
---------------------------------------------------------------*/
THEBATPLUGIN_API int WINAPI BAV_GetVersion(char* lpDest, int nBufSize)
{
	if (lpDest && nBufSize > 0)
		strncpy_s(lpDest, nBufSize, PLUGIN_VERSION, _TRUNCATE);
	return (int)strlen(PLUGIN_VERSION) + 1;
}

/*-------------------BAV_GetStatus-------------------------------
This function should return 0 if everything is OK.
Otherwise, it should return an error code (which will be processed in future).
---------------------------------------------------------------*/
THEBATPLUGIN_API int WINAPI BAV_GetStatus()
{
	try
	{
		hOBJECT hBL = RemoteMBLGet();
		if (hBL)
		{
			tERROR err = hBL->sysSendMsg(pmc_MAILCHECKER_PROCESS, NULL, NULL, NULL, NULL);
			if (err == errOK_DECIDED)
				return TheBat::AVC_OK;
		}
	}
	catch (...)
	{}

	return TheBat::AVC_OK;
}

/*--------------------BAV_ConfigNeeded-----------------------------
If your plugin supports configuration/setup, return non-zero value
otherwise return zero.
Returning a non-zero value means that TBP_Setup will be called
when the plugin is "configured".
---------------------------------------------------------------*/
THEBATPLUGIN_API int WINAPI BAV_ConfigNeeded()
{
	return TheBat::AVC_OK;
//	return TheBat::AVC_NotSupported;	// no configuration dialogs yet
}

/*---------------------BAV_ComNeeded-------------------------------
Return a non-zero value if the plugin needs COM subsystem initialization.
If the plugin implements functions working with the ITBPDataProvider interface,
the return value of this function is ignored
because it requires COM initialisation anyway.
---------------------------------------------------------------*/
THEBATPLUGIN_API int WINAPI BAV_ComNeeded()
{
	return TheBat::AVC_OK;	// no COM support needed as of yet
}

/*---------------------BAV_Setup---------------------------------
If you support setup (aka return 1 in BAV_ConfigNeeded)
then you should return zero if configuration was successful.
Otherwise you should return a non-zero error code
---------------------------------------------------------------*/
THEBATPLUGIN_API int WINAPI BAV_Setup()
{
	hTASKMANAGER pTM = (hTASKMANAGER)RemoteMBLGet();
	if (!pTM)
	{
		AVTRACE(_T("Cannot show the settings dialog - couldn't get remote BL"));
		return TheBat::AVC_OK;
	}

	tERROR err = pTM->AskAction(TASKID_TM_ITSELF, cAskGuiAction::SHOW_SETTINGS_WND, 
		&cProfileAction(AVP_PROFILE_MAILMONITOR));

	if (PR_FAIL(err))
	{
		USES_CONVERSION;
		tstring msg(_T("Failed to call AskAction: "));
		msg += A2CT((boost::lexical_cast<std::string>(err)).c_str());
		AVTRACE(msg.c_str());
		return TheBat::AVC_OK;
	}

	AVTRACE(_T("Settings dialog has been successfully shown"));
	return TheBat::AVC_OK;
}

THEBATPLUGIN_API int WINAPI BAV_SetCfgData(void* ABuf, int ABufSize)
{ 
	return TheBat::AVC_NotSupported;
}

THEBATPLUGIN_API int WINAPI BAV_GetCfgData(void* ABuf, int* ABufSize)
{ 
	return TheBat::AVC_NotSupported;
}


//------------------- File checking and curing ------------------
THEBATPLUGIN_API int WINAPI BAV_FileChecking()
{
	return TheBat::AVC_NotSupported;
//	return TheBat::AVC_OK;
}

int _CheckFileImpl(char* AFileName, char* Msg, bool bCheckOnly)
{
	if (!AFileName)
	{
		AVTRACE("File checking error - empty file name passed");
		return TheBat::AVC_OK;
	}

	TheBat::AVC_RetCodes nRet = TheBat::AVC_OK;
	cMCProcessParams processParams;
	
	cFSSyncRequest FSSyncRequest(GetCurrentThreadId());

	tERROR err = errOK;
	hOBJECT hWorker = RemoteMBLGet(&err);
		
	try
	{
		USES_CONVERSION;
		if (bCheckOnly)
		{
			AVTRACE(A2CT((std::string("Checking file ") + AFileName).c_str()));
		}
		else
			AVTRACE(A2CT((std::string("Trying to cure file ") + AFileName).c_str()));

		if (!hWorker)
			throw std::runtime_error("Couldn't get remote MBL");

		hWorker->sysSendMsg(pmc_FSSYNC, pm_FSSYNC_RegisterInvisibleThread, NULL, &FSSyncRequest, SER_SENDMSG_PSIZE);

		// split AFileName to the name and the path
		tstring strFullName(AFileName);
		tstring strObjName;
		tstring strObjPath;

		{
			tstring::size_type nPos = strFullName.rfind(_T('/'));
			if (nPos == tstring::npos)
				nPos = strFullName.rfind(_T('\\'));
			
			if (nPos != tstring::npos)
			{
				strObjName.assign(&strFullName[nPos + 1]);
				strObjPath.assign(strFullName, 0, nPos);
			}
			else
			{
				strObjName = AFileName;
				strObjPath = _T(".");
			}
		}

		PragueHelper::CAutoObject hMyIO;
		PRAGUE_ASSERT(CALL_SYS_ObjectCreate(hWorker, &hMyIO, IID_IO, PID_NATIVE_FIO, 0));
		PRAGUE_ASSERT(CALL_SYS_PropertySetStr(hMyIO, 0, pgOBJECT_FULL_NAME, (void*)AFileName, 0, cCP_ANSI));
		PRAGUE_ASSERT(CALL_SYS_PropertySetStr(hMyIO, 0, pgOBJECT_NAME, (void*) strObjName.c_str(), 0, cCP_ANSI));
		PRAGUE_ASSERT(CALL_SYS_PropertySetStr(hMyIO, 0, pgOBJECT_PATH, (void*) strObjPath.c_str(), 0, cCP_ANSI));
		PRAGUE_ASSERT(CALL_SYS_ObjectCreateDone(hMyIO));
		PRAGUE_ASSERT(hMyIO->propSetDWord(g_propMailerPID, ::GetCurrentProcessId()));
		PRAGUE_ASSERT(hMyIO->propSetBool(g_propCheckOnly, bCheckOnly ? cTRUE : cFALSE));
		PRAGUE_ASSERT(hMyIO->propSetStr(0, g_propVirtualName, "The Bat! file"));
		PRAGUE_ASSERT(hMyIO->propSetBool(g_propMessageIsIncoming, cTRUE));
		PRAGUE_ASSERT(hWorker->sysSendMsg(pmc_MAILCHECKER_PROCESS, NULL, hMyIO, &processParams, SER_SENDMSG_PSIZE));
		
		if ((processParams.p_MailCheckerVerdicts & mcv_MESSAGE_DELETED) == mcv_MESSAGE_DELETED)
		{
			AVTRACE(_T("The IO was deleted in MC, detaching..."));
			hMyIO.Detach();	// it was closed in MC
		}

		if ((processParams.p_MailCheckerVerdicts & mcv_MESSAGE_CHECKED) != mcv_MESSAGE_CHECKED)
			throw std::runtime_error("The message was rejected by MC");
		
		nRet = TheBat::AVC_OK;
		if (processParams.p_MailCheckerVerdicts & mcv_MESSAGE_AV_INFECTED )
			nRet = TheBat::AVC_VirusFound;
		else if (processParams.p_MailCheckerVerdicts & mcv_MESSAGE_AV_WARNING)
			nRet = TheBat::AVC_Suspicious;

		tstring msg(_T("The file was checked successfully, ret code = "));
		msg += A2CT((boost::lexical_cast<std::string>(nRet)).c_str());
		AVTRACE(msg.c_str());
	}
	catch (std::exception& ex)
	{
		// probably some Prague error
		AVTRACE(ex.what());
		nRet = TheBat::AVC_OK;
	}
	catch (...)
	{
		// all other errors
		AVTRACE(_T("Unhandled exception"));
		nRet = TheBat::AVC_OK;
	}
	
	if (((nRet == TheBat::AVC_VirusFound) || (nRet == TheBat::AVC_Suspicious)) &&
		!processParams.p_szVirusName.empty())
	{
		tstring msg(_T("A virus was found in file! Virus name: "));

		cStrBuff tmp = processParams.p_szVirusName.c_str(cCP_ANSI);
		msg += A2CT( (tCHAR*)tmp );
		AVTRACE(msg.c_str());

		strcpy(Msg, (tCHAR*)tmp );
	}

	if(hWorker)
		hWorker->sysSendMsg(pmc_FSSYNC, pm_FSSYNC_UnregisterInvisibleThread, NULL, &FSSyncRequest, SER_SENDMSG_PSIZE);

	AVTRACE(_T(""));
	return nRet;
}

THEBATPLUGIN_API int WINAPI BAV_CheckFile(char* AFileName, char* Msg)
{
	return _CheckFileImpl(AFileName, Msg, true);
}

THEBATPLUGIN_API int WINAPI BAV_CureFile(char* AFileName, char* Msg)
{
	return _CheckFileImpl(AFileName, Msg, false);
}
//////////////////////////////////////////////////////////////////////////


//------------------ Memory checking and curing -----------------
THEBATPLUGIN_API int WINAPI BAV_MemoryChecking()
{
	return TheBat::AVC_Error;	// probably won't be implemented (we'll implement IStreams support instead)
}

THEBATPLUGIN_API int WINAPI BAV_CheckMemory(void* ABuf,  int ABufSize, char* Msg)
{
	return TheBat::AVC_NotSupported;
}

THEBATPLUGIN_API int WINAPI BAV_CureMemory(void* ABuf, int* ABufSize, char* Msg)
{
	return TheBat::AVC_NotSupported;
}
//////////////////////////////////////////////////////////////////////////


//------------------ Streams checking and curing -----------------
THEBATPLUGIN_API int WINAPI BAV_StreamChecking()
{
	return TheBat::AVC_OK;
}

int _CheckStreamImpl(IStream* InStream, IN LPCTSTR szFileName, bool bCheckOnly, OUT char* Msg)
{
	if (!InStream)
	{
		AVTRACE("Stream checking error - an empty stream pointer was passed");
		return TheBat::AVC_Error;
	}

	TheBat::AVC_RetCodes nRet = TheBat::AVC_Error;
	cMCProcessParams processParams;

	try
	{
		AVTRACE(bCheckOnly? _T("Checking stream") : _T("Trying to cure stream"));

		tERROR err = errOK;
		hOBJECT hWorker = RemoteMBLGet(&err);
		if (!hWorker)
		{
			tstring msg(_T("Couldn't get remote MBL: "));
			msg += A2CT((boost::lexical_cast<std::string>(err)).c_str());
			throw std::runtime_error(msg.c_str());
		}

		// create the COMStreamIO object
		PragueHelper::CAutoObject hCOMStreamIO;
		PRAGUE_ASSERT(CALL_SYS_ObjectCreate(hWorker, &hCOMStreamIO, IID_IO, PID_COMSTREAM, 0));
		PRAGUE_ASSERT(CALL_SYS_PropertySetPtr(hCOMStreamIO, plStream, InStream));
		PRAGUE_ASSERT(CALL_SYS_PropertySetStr(hCOMStreamIO, 0, pgOBJECT_FULL_NAME, (void*)szFileName, 0, cCP_ANSI));
		PRAGUE_ASSERT(CALL_SYS_PropertySetStr(hCOMStreamIO, 0, pgOBJECT_NAME, (void*)szFileName, 0, cCP_ANSI));
		PRAGUE_ASSERT(CALL_SYS_PropertySetStr(hCOMStreamIO, 0, pgOBJECT_PATH, (void*) ".", 0, cCP_ANSI));
		PRAGUE_ASSERT(CALL_SYS_ObjectCreateDone(hCOMStreamIO));
		//////////////////////////////////////////////////////////////////////////
		
		PRAGUE_ASSERT(hCOMStreamIO->propSetDWord(g_propMailerPID, ::GetCurrentProcessId()));
		PRAGUE_ASSERT(hCOMStreamIO->propSetBool(g_propCheckOnly, bCheckOnly ? cTRUE : cFALSE));

		std::string strVirtualFileName;
		if (szFileName)
			strVirtualFileName = szFileName;

		if (strVirtualFileName.empty())
			strVirtualFileName = "The Bat! file";

		PRAGUE_ASSERT(hCOMStreamIO->propSetStr(0, g_propVirtualName, const_cast<LPSTR>(strVirtualFileName.c_str())));
		PRAGUE_ASSERT(hCOMStreamIO->propSetBool(g_propMessageIsIncoming, cTRUE));

		PRAGUE_ASSERT(hWorker->sysSendMsg(pmc_MAILCHECKER_PROCESS, NULL, hCOMStreamIO, &processParams, SER_SENDMSG_PSIZE));
		
		if ((processParams.p_MailCheckerVerdicts & mcv_MESSAGE_DELETED) == mcv_MESSAGE_DELETED)
		{
			AVTRACE(_T("The IO was deleted in MC, detaching..."));
			hCOMStreamIO.Detach();	// it was closed in MC
			
			if (!bCheckOnly)
			{
				// empty the stream
				ULARGE_INTEGER ulNewSize;
				ulNewSize.QuadPart = 0;
				InStream->SetSize(ulNewSize);
			}
		}
		
		if ((processParams.p_MailCheckerVerdicts & mcv_MESSAGE_CHECKED) != mcv_MESSAGE_CHECKED)
			throw std::runtime_error("The message was rejected by MC");

		nRet = TheBat::AVC_OK;
		if (processParams.p_MailCheckerVerdicts & mcv_MESSAGE_AV_INFECTED )
			nRet = TheBat::AVC_VirusFound;
		else if (processParams.p_MailCheckerVerdicts & mcv_MESSAGE_AV_WARNING)
			nRet = TheBat::AVC_Suspicious;

		tstring msg(_T("The stream was checked successfully, ret code = "));
		msg += A2CT((boost::lexical_cast<std::string>(nRet)).c_str());
		AVTRACE(msg.c_str());
	}
	catch (std::exception& ex)
	{
		// probably some Prague error
		AVTRACE(ex.what());
		nRet = TheBat::AVC_OK;
	}
	catch (...)
	{
		// all other errors
		AVTRACE(_T("Unhandled exception"));
		nRet = TheBat::AVC_OK;
	}

	if (((nRet == TheBat::AVC_VirusFound) || (nRet == TheBat::AVC_Suspicious)) &&
		!processParams.p_szVirusName.empty())
	{
		tstring msg(tstring(_T("A virus was found in the stream! Virus name: ")));
		
		cStrBuff tmp = processParams.p_szVirusName.c_str(cCP_ANSI);
		msg += A2CT( (tCHAR*)tmp );
		AVTRACE(msg.c_str());
		strcpy(Msg, (tCHAR*)tmp );
	}
	
	AVTRACE(_T(""));
	return nRet;
}

THEBATPLUGIN_API int WINAPI BAV_CheckStream(IStream* InStream, char* Msg)
{
	AVTRACE("Trying to check stream...");
	return _CheckStreamImpl(InStream, NULL, /* bCheckOnly */ true, Msg);
}

THEBATPLUGIN_API int WINAPI BAV_CureStream(IStream* InStream, IStream* OutStream, char* Msg)
{
	IStreamPtr spInStream(InStream);
	IStreamPtr spOutStream(OutStream);

	AVTRACE("Trying to cure stream...");
	if (!InStream || !OutStream || !Msg)
	{
		AVTRACE("Cure stream error - invalid parameters");
		return TheBat::AVC_Error;
	}

	// get the size of the input stream
	STATSTG pStat;
	if (FAILED(InStream->Stat(&pStat, STATFLAG_NONAME)))
	{
		AVTRACE("Cure stream error - couldn't get the size of incoming stream");
		return TheBat::AVC_Error;
	}
	
	// fill the OutStream from the InStream and pass the OutStream to MC
	ULARGE_INTEGER nBytesRead, nBytesWritten;
	if (FAILED(InStream->CopyTo(OutStream, pStat.cbSize, &nBytesRead, &nBytesWritten)))
	{
		AVTRACE("Cure stream error - couldn't copy streams");
		return TheBat::AVC_Error;
	}

	return _CheckStreamImpl(OutStream, NULL, /* bCheckOnly */ false, Msg);
}

THEBATPLUGIN_API int WINAPI BAV_CheckStreamEx(IStream* InStream, wchar_t* Msg, const wchar_t* FileName)
{
	AVTRACE("Trying to check stream (ex)...");

	if (!InStream || !Msg)
		return TheBat::AVC_Error;

	USES_CONVERSION;
	boost::scoped_array<char> buf(new char[1024]);
	buf[0] = '\0';
	int nRet = _CheckStreamImpl(InStream, (FileName ? W2A(FileName) : NULL), /* bCheckOnly */ true, buf.get());
	wcscpy(Msg, A2W(buf.get()));

	return nRet;
}

THEBATPLUGIN_API int WINAPI BAV_CureStreamEx(IStream* InStream, IStream* OutStream, wchar_t* Msg, const wchar_t* FileName)
{
	AVTRACE("Trying to cure stream (ex)...");

	IStreamPtr spInStream(InStream);
	IStreamPtr spOutStream(OutStream);

	if (!InStream || !OutStream || !Msg)
		return TheBat::AVC_Error;

	// get the size of the input stream
	STATSTG stat;
	HRESULT hRes = InStream->Stat(&stat, STATFLAG_NONAME);
	if (FAILED(hRes))
	{
		AVTRACE("CureEx stream error - couldn't get the size of incoming stream");
		return TheBat::AVC_Error;
	}
	
	// fill the OutStream from the InStream and pass the OutStream to MC
	ULARGE_INTEGER nBytesRead, nBytesWritten;
	if (FAILED(InStream->CopyTo(OutStream, stat.cbSize, &nBytesRead, &nBytesWritten)))
	{
		AVTRACE("CureEx stream error - couldn't copy streams");
		return TheBat::AVC_Error;
	}

	USES_CONVERSION;
	boost::scoped_array<char> buf(new char[1024]);
	buf[0] = '\0';
	int nRet = _CheckStreamImpl(OutStream, (FileName ? W2A(FileName) : NULL), /* bCheckOnly */ false, buf.get());
	wcscpy(Msg, A2W(buf.get()));

	return nRet;
}
//////////////////////////////////////////////////////////////////////////


//------------------ Plugin registration in TheBat! ---------------------------
THEBATPLUGIN_API int _stdcall Register(DWORD, DWORD)
{
	AVTRACE(_T("---------------------  Registering plugin... -------------------"));

	TCHAR szPath[MAX_PATH];
	if (!GetModuleFileName(reinterpret_cast<HMODULE>(g_hModule), szPath, MAX_PATH))
		return errUNEXPECTED;

	int nRet = CRegister::RegisterAVPlugin(szPath);

	USES_CONVERSION;
	tstring msg(_T("---------------------  Done registering, ret code: "));
	msg += A2CT((boost::lexical_cast<std::string>(nRet)).c_str());
	AVTRACE(msg.c_str());
	AVTRACE(_T(""));

	return nRet;
}

THEBATPLUGIN_API int _stdcall Unregister(DWORD, DWORD)
{
	AVTRACE(_T("---------------------  Unregistering plugin... -----------------"));

	TCHAR szPath[MAX_PATH];
	if (!GetModuleFileName(reinterpret_cast<HMODULE>(g_hModule), szPath, MAX_PATH))
		return errUNEXPECTED;
	
	int nRet = CRegister::UnregisterAVPlugin(szPath);

	USES_CONVERSION;
	tstring msg(_T("---------------------  Done unregistering, ret code: "));
	msg += A2CT((boost::lexical_cast<std::string>(nRet)).c_str());
	AVTRACE(msg.c_str());
	AVTRACE(_T(""));

	return nRet;
}

THEBATPLUGIN_API int _stdcall CleanRegFlag()
{
	AVTRACE(_T("---------------------  Clean Reg Flag... -----------------"));

	TCHAR szPath[MAX_PATH];
	if (!GetModuleFileName(reinterpret_cast<HMODULE>(g_hModule), szPath, MAX_PATH))
		return errUNEXPECTED;

	int nRet = CRegister::CleanRegistrationFlag(szPath);

	USES_CONVERSION;
	tstring msg(_T("---------------------  Done cleaning, ret code: "));
	msg += A2CT((boost::lexical_cast<std::string>(nRet)).c_str());
	AVTRACE(msg.c_str());
	AVTRACE(_T(""));

	return nRet;
}

//////////////////////////////////////////////////////////////////////////
