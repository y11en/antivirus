// MyComModule.cpp: implementation of the CMyComModule class.
//
//////////////////////////////////////////////////////////////////////

#define PR_IMPEX_DEF

#include "stdafx.h"
#include "MyComModule.h"
#include "Utility.h"
#include "../../TheBatPlugin/remote_mbl.h"
#include "../../TheBatPlugin/PragueHelper.h"
#include "Tracing.h"

#include <Prague/pr_err.h>
#include <Prague/iface/e_loader.h>
#include <Prague/plugin/p_win32_reg.h>

#include <Mail/plugin/p_mctask.h>
#include <Mail/structs/s_antispam.h>

#include <PPP/structs/s_gui.h>

#include <atlbase.h>

#define IMPEX_IMPORT_LIB
#  include <Prague/iface/e_loader.h>
#  include <Prague/plugin/p_string.h>

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(import_table)
	#include <Prague/iface/e_loader.h> 
IMPORT_TABLE_END

namespace
{
	const size_t STRING_BUF_SIZE = 2048;

	LPCTSTR OUTLOOKEXPRESS_KEY = _T("SOFTWARE\\Microsoft\\Outlook Express");
	LPCTSTR OE_MEDIA_VALUE = _T("Media");
	LPCTSTR OE_MEDIAVER_VALUE = _T("MediaVer");
}

cPrTracer g_Tracer;

bool CMyComModule::LocInit()
{
	bool bRet = false;

	// делаем это на этом потоке, т.к. DLL_PROCESS_ATTACH теперь вызывается на r3hook потоке
	if(!g_hHook)
		g_hHook = SetWindowsHookEx(WH_CBT, CBT_Proc, 0, GetCurrentThreadId());
	m_hEnvironment = NULL;

	try
	{
		
		tERROR err;
		PLUGIN_LIST the_list;
		err = RemoteMBLInit(static_cast<HMODULE>(_Module.GetModuleInstance()), the_list);

		g_Tracer.Init(NULL, false, "oeplugin");

		if( PR_SUCC(err) ) err = g_root->RegisterCustomPropId(&g_propMessageIsIncoming, npMESSAGE_IS_INCOMING, pTYPE_BOOL | pCUSTOM_HERITABLE);
		if( PR_SUCC(err) ) err = g_root->RegisterCustomPropId(&g_propMailMessageOS_PID, npMAILMESSAGE_OS_PID, pTYPE_DWORD | pCUSTOM_HERITABLE);
		if( PR_SUCC(err) ) err = g_root->RegisterCustomPropId(&g_propMailerPID, npMAILER_PID, pTYPE_DWORD | pCUSTOM_HERITABLE);

		tDWORD result;
		if( PR_SUCC(err) ) g_root->ResolveImportTable(&result, import_table, PID_APPLICATION);

		LPCSTR sSourceName = {"HKLM\\" VER_PRODUCT_REGISTRY_PATH};
		if( PR_SUCC(err) ) err = g_root->sysCreateObject((hOBJECT*)&m_hEnvironment, IID_REGISTRY, PID_WIN32_REG);
		if( PR_SUCC(err) ) err = m_hEnvironment->propSetBool(plWOW64_32KEY, cTRUE);
		if( PR_SUCC(err) ) err = m_hEnvironment->propSetStr(0, pgROOT_POINT, (void *)sSourceName);
		if( PR_SUCC(err) ) err = m_hEnvironment->propSetBool(pgOBJECT_RO, cTRUE);
		if( PR_SUCC(err) ) err = m_hEnvironment->propSetStr(0, pgROOT_POINT, "environment", 12);
		if( PR_SUCC(err) ) err = m_hEnvironment->sysCreateObjectDone();
		if( PR_FAIL(err) )
		{
			if(m_hEnvironment)
				m_hEnvironment->sysCloseObject(), m_hEnvironment = NULL;
		}
		else if(PrSetEnviroment)
			PrSetEnviroment((hOBJECT)m_hEnvironment, NULL);

		bRet = PR_SUCC(err) && m_GuiLoader.CreateAndInitRoot(g_root, 0, this);

		if(bRet)
			ASTRACE(_T("Initialization completed successfully"));
		else
			ASTRACE(_T("Failed to initialize"));
	}
	catch (std::exception& ex)
	{
		// probably some Prague error
		ASTRACE(ex.what());
	}
	
	return bRet;
}

void CMyComModule::LocDeinit()
{
	m_GuiLoader.Unload();

	if( m_hEnvironment )
		m_hEnvironment->sysCloseObject(), m_hEnvironment = NULL;

	RemoteMBLTerm();

	g_Tracer.Deinit();
}

HICON CMyComModule::LoadIcon(LPCTSTR szIconName) const
{
	return ::LoadIcon(m_hInstResource, szIconName);
}

HCURSOR CMyComModule::LoadCursor(LPCTSTR szCursorName) const
{
	return ::LoadCursor(m_hInstResource, szCursorName);
}

HMENU CMyComModule::LoadMenu(LPCTSTR szMenuName) const
{
	return ::LoadMenu(m_hInstResource, szMenuName);
}

tstring CMyComModule::LoadString(UINT nStringID) const
{
	TCHAR szBuf[STRING_BUF_SIZE];
	szBuf[0] = _T('\0');
	
	int nRes = 0;
	nRes = ::LoadString(m_hInstResource, nStringID, szBuf, STRING_BUF_SIZE);
	
	return szBuf;
}

HBITMAP CMyComModule::LoadBitmap(LPCTSTR szBitmapName) const
{
	return ::LoadBitmap(m_hInstResource, szBitmapName);
}

bool CMyComModule::TeachAntiSpam(bool bIsSpam, const LPBYTE buf, UINT nBufSize)
{
	bool bRet = false;

	tstring msg(_T("Processing msg : "));
	msg += bIsSpam ? _T("spam") : _T("not spam");
	ASTRACE(msg.c_str());

	try
	{
		// try to get BL
		hOBJECT hWorker = RemoteMBLGet();
		if (!hWorker)
			throw std::runtime_error("Couldn't get remote MBL");
		
		// create TempIO for our buffer
		PragueHelper::CAutoIO hTempIO;
		PRAGUE_ASSERT(hWorker->sysCreateObjectQuick( (hOBJECT*)&hTempIO, IID_IO, PID_TMPFILE));
		
		tDWORD dwWritten = 0;
		PRAGUE_ASSERT(hTempIO->SeekWrite(&dwWritten, 0, buf, nBufSize));
		if (dwWritten <= 0)
			throw std::runtime_error("Couldn't write enough chars to continue");
		
		// set the parameters and call MC task
		PRAGUE_ASSERT(hTempIO->set_pgOBJECT_ORIGIN(OID_MAIL_MSG_MIME));
		PRAGUE_ASSERT(hTempIO->propSetDWord(g_propMailMessageOS_PID, PID_MSOE));
		PRAGUE_ASSERT(hTempIO->propSetBool(g_propMessageIsIncoming, cTRUE));
		PRAGUE_ASSERT(hTempIO->propSetDWord(g_propMailerPID, ::GetCurrentProcessId()));
		
		cAntispamTrainParams params;
		params.p_Action = bIsSpam ? cAntispamTrainParams::TrainAsSpam : cAntispamTrainParams::TrainAsHam;
		
		PRAGUE_ASSERT(hWorker->sysSendMsg(pmc_ANTISPAM_TRAIN, NULL, hTempIO, &params, SER_SENDMSG_PSIZE));

		ASTRACE(_T("The message was processed successfully"));
		bRet = true;
	}
	catch (std::exception& ex)
	{
		// probably some Prague error
		ASTRACE(ex.what());
	}
	catch (...)
	{
		// all other errors
		ASTRACE(_T("Unhandled exception"));
	}

	return bRet;
}

bool CMyComModule::_WriteOEVerToLog()
{
	CRegKey key;
	if (key.Open(HKEY_LOCAL_MACHINE, OUTLOOKEXPRESS_KEY, KEY_READ) != ERROR_SUCCESS)
	{
		ASTRACE(_T("Failed to open OE reg key"));
		return false;
	}

	tstring strMedia;
	DWORD dwBufSize = 256;
	TCHAR szBuf[256];
	if (key.QueryStringValue(OE_MEDIA_VALUE, szBuf, &dwBufSize) == ERROR_SUCCESS)
		strMedia = szBuf;

	tstring strMediaVer;
	dwBufSize = 256;
	if (key.QueryStringValue(OE_MEDIAVER_VALUE, szBuf, &dwBufSize) == ERROR_SUCCESS)
		strMediaVer = szBuf;

	tstring msg(_T("Media: "));
	msg += strMedia;
	msg += _T(", ");
	msg += _T("Ver: ");
	msg += strMediaVer;
	ASTRACE(msg.c_str());

	return true;
}

bool CMyComModule::IsKAVRunning() const
{
	bool bRet = false;
	
	try
	{
		// try to get BL
		hOBJECT hWorker = RemoteMBLGet();
		if (!hWorker)
			throw std::runtime_error("Couldn't get remote MBL");

		bRet = true;
	}
	catch (std::exception&)
	{
	}
	catch (...)
	{
	}
	
	return bRet;
}

bool CMyComModule::IsAntiSpamTaskRunning() const
{
	bool bRet = false;
	
	try
	{
		// try to get BL
		hOBJECT hWorker = RemoteMBLGet();
		if (!hWorker)
			throw std::runtime_error("Couldn't get remote MBL");
		
		tERROR err = hWorker->sysSendMsg(pmc_ANTISPAM_PROCESS, NULL, NULL, NULL, NULL);
		if (err == errOK_DECIDED)
			bRet = true;

		PRAGUE_ASSERT(err);
	}
	catch (std::exception& ex)
	{
		// probably some Prague error
		ASTRACE(ex.what());
	}
	catch (...)
	{
		// all other errors
	}
	
	return bRet;
}

tstring CMyComModule::LoadString(LPCSTR szSection, LPCSTR szID)
{
	if (!szSection || !szID)
		return _T("");

	CRootItem* pGuiRoot = m_GuiLoader.GetRoot();
	if (!pGuiRoot)
		return _T("");
	
	USES_CONVERSION;
#if _BASEGUI_VER >= 0x0200
	return A2T(const_cast<LPSTR>(pGuiRoot->GetString(TOR_tString, PROFILE_LOCALIZE, szSection, szID)));
#else
	return A2T(const_cast<LPSTR>(pGuiRoot->GetStringBuf(PROFILE_LOCALIZE, szSection, szID)));
#endif
}

std::wstring CMyComModule::LoadStringW(LPCSTR szSection, LPCSTR szID)
{
	if (!szSection || !szID)
		return L"";

	CRootItem* pGuiRoot = m_GuiLoader.GetRoot();
	if (!pGuiRoot)
		return L"";

#if _BASEGUI_VER >= 0x0200
	return pGuiRoot->LocalizeStr(TOR_cStrObj, pGuiRoot->GetString(TOR_tString, PROFILE_LOCALIZE, szSection, szID));
#else
	return pGuiRoot->GetStringBufW(PROFILE_LOCALIZE, szSection, szID);
#endif
}

int CMyComModule::MessageBox(CItemBase* pParent, LPCTSTR szTitle, LPCTSTR szText, UINT uType)
{
	if (!szTitle || !szText)
		return IDCANCEL;

	CRootItem* pGuiRoot = m_GuiLoader.GetRoot();
	if (!pGuiRoot)
		return IDCANCEL;
	
	return pGuiRoot->ShowMsgBox(pParent, szText, szTitle, uType);
}

bool CMyComModule::ShowAntiSpamSettings() const
{
	bool bRet = false;

	try
	{
		// try to get BL
		hTASKMANAGER tm = (hTASKMANAGER) RemoteMBLGet();
		if (!tm)
			throw std::runtime_error("Couldn't get remote MBL");
		
		tERROR err = tm->AskAction(TASKID_TM_ITSELF, cAskGuiAction::SHOW_SETTINGS_WND, 
			&cProfileAction(AVP_PROFILE_ANTISPAM));
		if (err == errOK)
			bRet = true;
	}
	catch (std::exception&)
	{
		// probably some Prague error
	}
	catch (...)
	{
		// all other errors
	}

	return bRet;
}

void CMyComModule::GetHelpStorageId(cStrObj& strHelpStorageId)
{
	strHelpStorageId = "%HelpFile%";
	if( ::g_root->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(strHelpStorageId), 0, 0) != errOK_DECIDED )
		strHelpStorageId.erase();
}
