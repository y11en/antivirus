#include "regar.h"
#include "srv.h"
#include "../eventmgr/include/eventhandler.h"
#include "../eventmgr/include/envhelper.h"
#include <memory>

void cRegAutoRun::clear()
{
	key.clear();
	tstrfree(value_name);
	tstrfree(value_data);
	tstrfree(value_data_part);
	tstrfree(filename);
	value_name = NULL;
	value_data = NULL;
	value_data_part = NULL;
	filename = NULL;
}

cRegAutoRun::cRegAutoRun()
{
	value_name = NULL;
	value_data = NULL;
	value_data_part = NULL;
	filename = NULL;
}

cRegAutoRun::~cRegAutoRun()
{
	tstrfree(value_name);
	tstrfree(value_data);
	tstrfree(value_data_part);
	tstrfree(filename);
}

bool cRegAutoRun::resolve(cEnvironmentHelper* pEnvironmentHelper)
{
	if (filename)
		return false;
	if (!value_data_part)
		return false;
	tstring file = pEnvironmentHelper->PathFindExecutable(value_data_part, NULL);
	if (file)
	{
		filename = tstrdup(cPath(pEnvironmentHelper, file).getFull());
		tstrfree(file);
		return true;
	}
	if (tstrchar(value_data_part) == '{' && tstrchar(tstrgetlastchar(value_data_part)) == '}')
	{
		tRegistryKey hkCLSID;
		if (pEnvironmentHelper->Reg_OpenKey(crHKEY_CLASSES_ROOT, _T("CLSID"), &hkCLSID))
		{
			tRegistryKey hkCLSID2;
			if (pEnvironmentHelper->Reg_OpenKey(hkCLSID, value_data_part, &hkCLSID2))
			{
				tRegistryKey hkServer32;
				if (pEnvironmentHelper->Reg_OpenKey(hkCLSID2, _T("InprocServer32"), &hkServer32)
					|| pEnvironmentHelper->Reg_OpenKey(hkCLSID2, _T("LocalServer32"), &hkServer32))
				{
					tstring sServer32 = pEnvironmentHelper->Reg_QueryValueStr(hkServer32, _T(""));
					if (sServer32)
					{
						filename = tstrdup(cPath(pEnvironmentHelper, sServer32).getFull());
						tstrfree(sServer32);
						return true;
					}
					pEnvironmentHelper->Reg_CloseKey(hkServer32);
				}
				pEnvironmentHelper->Reg_CloseKey(hkCLSID2);
			}
			pEnvironmentHelper->Reg_CloseKey(hkCLSID);
		}
	}
	return false;

}

cRegAutoRun& cRegAutoRun::operator =(cRegAutoRun& from)
{
	tstrfree(value_name);
	tstrfree(value_data);
	tstrfree(value_data_part);
	tstrfree(filename);

	key = from.key;
	value_name = tstrdup(value_name);
	value_data = tstrdup(value_data);
	value_data_part = tstrdup(value_data_part);
	filename = tstrdup(filename);
	return *this;
}

cRegAutoRunList::cRegAutoRunList()
{
}

cRegAutoRunList::~cRegAutoRunList()
{
	for (tautorunvector::iterator _it = m_list.begin(); _it != m_list.end(); _it++)
		delete *_it;
}

bool cRegAutoRunList::Init(cEnvironmentHelper* pEnvironmentHelper, cEventMgr* pEventMgr)
{
	m_pEnvironmentHelper = pEnvironmentHelper;
	m_pEventMgr = pEventMgr;
	return true;
}

void cRegAutoRunList::Register(cEvent& event, const cRegKey& sKey, tcstring sValueName, tcstring sValueData, uint32_t nARFlags)
{
	const int MAX_ADDITIONAL_PARAMS_COUNT = 10;
	tcstring params[0x20+MAX_ADDITIONAL_PARAMS_COUNT];
	tstring cmd_line = tstrdup(sValueData);
	size_t params_count = 0;
	if (!sValueName) // default value
		sValueName = _T("");
	if (sValueData)
	{
		if (tstrchar(sValueData) == '{' && tstrchar(tstrgetlastchar(sValueData)) == '}')
			params[params_count++] = sValueData; // CLSID
		else
			params_count = srvParseCmdLine(m_pEnvironmentHelper, cmd_line, NULL, params, countof(params)-MAX_ADDITIONAL_PARAMS_COUNT);
	}
	if (params_count > 1)
		params[params_count++] = sValueData; // full_cmd_line
	if (tstrchar(sValueName) == '{' && tstrchar(tstrgetlastchar(sValueName)) == '}')
		params[params_count++] = sValueName; // CLSID
	if (nARFlags & ARF_CLSID_KEY)
	{
		tcstring pKey = tstrrchr(sKey.m_name, '\\');
		if (pKey && pKey[1] == '{')
		{
			params[params_count++] = pKey+1; // CLSID
		}
	}
	for (size_t i=0; i<params_count; i++)
	{
		tcstring arg = params[i]; 
		if (!arg) // skip empty
			continue;
		if (tstrchar(arg) == 0) // skip empty
			continue;
		if (tstrchar(arg) == '/') // skip switches
			continue;
		std::auto_ptr<cRegAutoRun> ar(new cRegAutoRun);
		ar->key = sKey;
		ar->value_name = tstrdup(sValueName);
		ar->value_data = tstrdup(sValueData);
		ar->value_data_part = tstrdup(arg);
		m_pEventMgr->OnRegAutoRunRegister(event, sKey, sValueName, sValueData, arg);
		if (evtVerdict_Default != event.GetVerdict())
			break;
		if (ar->resolve(m_pEnvironmentHelper))
		{
			m_pEventMgr->OnRegAutoRunRegisterResolved(event, sKey, sValueName, sValueData, arg, ar->filename);
			if (evtVerdict_Default != event.GetVerdict())
				break;
		}
		cAutoLock lock(m_sync);
		m_list.push_back(ar.get());
		ar.release();
	}
	tstrfree(cmd_line);
	return;
}

bool cRegAutoRunList::Unregister(cPath& path)
{
	cAutoLock lock(m_sync);
	for (tautorunvector::iterator _it = m_list.begin(); _it != m_list.end(); )
	{
		if (0 == tstrcmp((*_it)->filename, path.getFull()))
		{
			tautorunvector::iterator _next = m_list.erase(_it);
			delete *_it;
			_it = _next;
		}
		else
			_it++;
	}
	return false;
}

bool cRegAutoRunList::IsRegistered(cEvent& event, cPath& path, cRegAutoRun* pRegAutoRun)
{
	cAutoLock lock(m_sync);
	for (tautorunvector::iterator _it = m_list.begin(); _it != m_list.end(); _it++)
	{
		cRegAutoRun* ar = *_it;
		if (!ar->filename)
		{
			ar->resolve(m_pEnvironmentHelper);
			if (!ar->filename)
				continue;

			m_pEventMgr->OnRegAutoRunRegisterResolved(event, ar->key, ar->value_name, ar->value_data, ar->value_data_part, ar->filename);
		}

		if (0 == tstrcmp(ar->filename, path.getFull()))
		{
			if (pRegAutoRun)
				*pRegAutoRun = *ar;
			return true;
		}
	}
	return false;
}

void cRegAutoRunList::ResolveAll(cEvent& event)
{
	cAutoLock lock(m_sync);
	for (tautorunvector::iterator _it = m_list.begin(); _it != m_list.end(); _it++)
	{
		cRegAutoRun* ar = *_it;
		if (ar->filename)
			continue;
		if (ar->resolve(m_pEnvironmentHelper))
			m_pEventMgr->OnRegAutoRunRegisterResolved(event, ar->key, ar->value_name, ar->value_data, ar->value_data_part, ar->filename);
	}
	return;
}

typedef struct _tControlledKeyStruct
{
	tcstring key;
	tcstring value;
	uint_fast32_t flags;
} ControlledKeyStruct;	

static const ControlledKeyStruct arrAutoRunsKeyNames[] = 	{

	{_T("CONTROL PANEL\\DESKTOP"), _T("SCRNSAVE.EXE")},

	{_T("SOFTWARE\\MICROSOFT\\COMMAND PROCESSOR"), _T("AUTORUN")},

	{_T("SOFTWARE\\MICROSOFT\\INTERNET EXPLORER\\DESKTOP\\COMPONENTS\\*"), NULL},  // ?VALUES?
//  {_T("SOFTWARE\\MICROSOFT\\INTERNET EXPLORER\\EXPLORER BARS"), NULL, ???
//  {_T("SOFTWARE\\MICROSOFT\\INTERNET EXPLORER\\TOOLBAR"), NULL, ???
	{_T("SOFTWARE\\MICROSOFT\\INTERNET EXPLORER\\EXTENSIONS\\*"), _T("CLSID"), ARF_CLSID_VALUEDATA},
	{_T("SOFTWARE\\MICROSOFT\\INTERNET EXPLORER\\EXTENSIONS\\*"), _T("EXEC")},
	{_T("SOFTWARE\\MICROSOFT\\INTERNET EXPLORER\\PLUGINS\\EXTENSION\\*"), _T("LOCATION")},
	{_T("SOFTWARE\\MICROSOFT\\INTERNET EXPLORER\\URLSEARCHHOOKS"), NULL, ARF_CLSID_VALUENAME},
	
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUN"), NULL},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUN\\*"), NULL},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUNONCE"), NULL},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUNONCEEX"), NULL},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUNSERVICES"), NULL},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUNSERVICESONCE"), NULL},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\SHELLSERVICEOBJECTDELAYLOAD"), NULL, ARF_CLSID_VALUEDATA},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\SHELL EXTENSIONS\\APPROVED"), NULL, ARF_CLSID_VALUENAME},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\EXPLORER\\BROWSER HELPER OBJECTS\\*"), NULL, ARF_CLSID_KEY},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\EXPLORER\\SHAREDTASKSCHEDULER"), NULL, ARF_CLSID_VALUENAME},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\EXPLORER\\SHELLEXECUTEHOOKS"), NULL, ARF_CLSID_VALUENAME},
	
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\POLICIES\\EXPLORER\\RUN"), NULL},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\POLICIES\\SYSTEM"), _T("SHELL")},
	
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS NT\\CURRENTVERSION\\WINDOWS"), _T("APPINIT_DLLS")},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS NT\\CURRENTVERSION\\WINDOWS"), _T("LOAD")},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS NT\\CURRENTVERSION\\WINDOWS"), _T("RUN")},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS NT\\CURRENTVERSION\\WINLOGON"), NULL}, // * => APPSETUP, GINADLL, SHELL, SYSTEM, TASKMAN, UIHOST, USERINIT
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS NT\\CURRENTVERSION\\WINLOGON\\NOTIFY\\*"), _T("DLLNAME")}, 
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS NT\\CURRENTVERSION\\IMAGE FILE EXECUTION OPTIONS\\*"), _T("DEBUGGER")},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS NT\\CURRENTVERSION\\ACCESSIBILITY\\UTILITY MANAGER\\MAGNIFIER"), _T("APPLICATION PATH")},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS NT\\CURRENTVERSION\\ACCESSIBILITY\\UTILITY MANAGER\\NARRATOR"), _T("APPLICATION PATH")},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS NT\\CURRENTVERSION\\ACCESSIBILITY\\UTILITY MANAGER\\ON-SCREEN KEYBOARD"), _T("APPLICATION PATH")},
	
	{_T("SOFTWARE\\MICROSOFT\\ACTIVE SETUP\\INSTALLED COMPONENTS\\*"), _T("STUBPATH")}, // Q238441
	
	{_T("SOFTWARE\\POLICIES\\MICROSOFT\\WINDOWS\\SYSTEM\\SCRIPTS\\*"), _T("SCRIPT")}, // ???
	
	{_T("SOFTWARE\\CLASSES\\*\\SHELLEX\\*HANDLERS"), NULL, ARF_CLSID_VALUEDATA | ARF_CLSID_VALUENAME},
	{_T("SOFTWARE\\CLASSES\\???FILE\\SHELL\\OPEN\\COMMAND"), _T("")},
	{_T("SOFTWARE\\CLASSES\\???FILE\\SHELL\\RUNAS\\COMMAND"), _T("")},
	{_T("SOFTWARE\\CLASSES\\FOLDER\\SHELL\\*\\COMMAND"), _T("")},
	{_T("SOFTWARE\\CLASSES\\PROTOCOLS\\FILTER\\*"), _T("CLSID"), ARF_CLSID_VALUEDATA},
	{_T("SOFTWARE\\CLASSES\\PROTOCOLS\\HANDLER\\*"), _T("CLSID"), ARF_CLSID_VALUEDATA},

	{_T("SYSTEM\\CONTROLSET???\\CONTROL\\NETWORKPROVIDER\\NOTIFYEES"), NULL}, // DLL that implements the Connection Notification API http://msdn2.microsoft.com/en-us/library/aa374737.aspx

	{_T("SYSTEM\\CONTROLSET???\\CONTROL\\PRINT\\MONITORS\\*"), _T("DRIVER")},

	{_T("SYSTEM\\CONTROLSET???\\CONTROL\\SECURITYPROVIDERS"), _T("SECURITYPROVIDERS")},

	{_T("SYSTEM\\CONTROLSET???\\CONTROL\\SESSION MANAGER"), _T("BOOTEXECUTE"), ARF_MULTI_SZ}, // def="autocheck autochk *"
	{_T("SYSTEM\\CONTROLSET???\\CONTROL\\SESSION MANAGER\\BOOTVERIFICATIONPROGRAM"), _T("IMAGENAME")},
	{_T("SYSTEM\\CONTROLSET???\\CONTROL\\SESSION MANAGER\\KNOWNDLLS"), NULL},
	{_T("SYSTEM\\CONTROLSET???\\CONTROL\\SESSION MANAGER\\LSA"), _T("*PACKAGES"), ARF_MULTI_SZ}, // http://msdn2.microsoft.com/en-us/library/aa379376.aspx
	{_T("SYSTEM\\CONTROLSET???\\CONTROL\\SESSION MANAGER\\LSA\\*"), _T("AUTH*")}, // http://msdn2.microsoft.com/en-us/library/aa379395.aspx
	{_T("SYSTEM\\CONTROLSET???\\CONTROL\\SESSION MANAGER\\SUBSYSTEMS"), _T("WINDOWS")},

	{_T("SYSTEM\\CONTROLSET???\\CONTROL\\TERMINAL SERVER\\WDS\\RDPWD"), _T("STARTUPPROGRAMS")},

	{_T("SYSTEM\\CONTROLSET???\\CONTROL\\WOW"), _T("CMDLINE")},

	{_T("SYSTEM\\CONTROLSET???\\SERVICES\\*"), _T("IMAGEPATH")},
	{_T("SYSTEM\\CONTROLSET???\\SERVICES\\WINSOCK2\\PARAMETERS\\NAMESPACE_CATALOG5\\CATALOG_ENTRIES\\*"), _T("LIBRARYPATH")},
	{_T("SYSTEM\\CONTROLSET???\\SERVICES\\WINSOCK2\\PARAMETERS\\PROTOCOL_CATALOG9\\CATALOG_ENTRIES\\*"), _T("PACKEDCATALOGITEM")},
};

typedef struct _tControlledKeyHintStruct
{
	tcstring    subkey;
	bool		bSubkeyMask;
	bool		bValueMask;
	uint_fast32_t arh_flags;
} ControlledKeyHintStruct;	

enum eARH_GLAGS {
	ARH_CONTROLSET = 1,
	ARH_SOFTWARE,     
	ARH_SOFTWARE_MICROSOFT,
	ARH_SOFTWARE_MICROSOFT_WINDOWS,
	ARH_SOFTWARE_MICROSOFT_WINDOWS_NT,
	ARH_SOFTWARE_MICROSOFT_INTERNET_XPLORER,
	ARH_SOFTWARE_CLASSES,
};


static ControlledKeyHintStruct arrAutoRunHints[countof(arrAutoRunsKeyNames)]; 
static bool bHintsInited = false;

void GetHint(tcstring* psubkey, bool* pbSubkeyMask, tcstring value, bool* pbValueMask, uint_fast32_t* parh_flags)
{
	tcstring subkey = *psubkey;
	if (pbSubkeyMask)
		*pbSubkeyMask = false;
	uint_fast32_t arh_flags = 0;
	if (tstrcmpinc(&subkey, _T("SOFTWARE\\")))
	{
		arh_flags = ARH_SOFTWARE;
		tstrcmpinc(&subkey, _T("WOW6432NODE\\"));
		if (tstrcmpinc(&subkey, _T("MICROSOFT\\")))
		{
			arh_flags = ARH_SOFTWARE_MICROSOFT;
			if (tstrcmpinc(&subkey, _T("WINDOWS\\")))
				arh_flags = ARH_SOFTWARE_MICROSOFT_WINDOWS;
			else if (tstrcmpinc(&subkey, _T("WINDOWS NT\\")))
				arh_flags = ARH_SOFTWARE_MICROSOFT_WINDOWS_NT;
			else if (tstrcmpinc(&subkey, _T("INTERNET EXPLORER\\")))
				arh_flags = ARH_SOFTWARE_MICROSOFT_INTERNET_XPLORER;
		}
		else if (tstrcmpinc(&subkey, _T("CLASSES\\")))
			arh_flags = ARH_SOFTWARE_CLASSES;
	}
	else if (tstrcmpinc(&subkey, _T("SYSTEM\\CONTROLSET???\\")))
	{
		arh_flags = ARH_CONTROLSET;
	}

	if (pbSubkeyMask)
	{
		if (tstrchr(subkey, '*') || tstrchr(subkey, '?'))
			*pbSubkeyMask = true;
	}
	if (pbValueMask && value)
	{
		if (tstrchr(value, '*') || tstrchr(value, '?'))
			*pbValueMask = true;
	}

	*psubkey = subkey;
	*parh_flags = arh_flags;
}

void InitHints()
{
	bHintsInited = true;
	ControlledKeyHintStruct* pHint = arrAutoRunHints;
	const ControlledKeyStruct*  pKey = arrAutoRunsKeyNames;
	for (size_t i=0; i<countof(arrAutoRunHints); i++, pHint++, pKey++)
	{
		pHint->subkey = pKey->key;
		GetHint(&pHint->subkey, &pHint->bSubkeyMask, pKey->value, &pHint->bValueMask, &pHint->arh_flags);
	}
}

bool cRegAutoRunList::IsAutoRun(const cRegKey& sKey, tcstring sValueName, uint32_t* pnARFlags )
{
	if (!bHintsInited)
		InitHints();

	if (pnARFlags)
		*pnARFlags = 0;

	if (!sValueName) // default value
		sValueName = _T("");

	tcstring sKeyName = sKey.m_name;

	// Terminal Server handles this situation by providing an install mode. You enter install mode by 
	// issuing the command "change user /install" at a command prompt or by selecting "All users begin
	// with common application settings" from the Add/Remove Programs wizard. Once you have entered 
	// install mode, you can install the application. Each registry setting that is written to 
	// HKEY_CURRENT_USER will be mirrored in a special location within HKEY_LOCAL_MACHINE. When your
	// installation is complete, the "change user /execute" command takes you out of install mode. 
	// If you're using the Add/Remove Programs wizard, you are automatically taken out of install mode
	// after you click the finish button. Terminal Server propagates the mirrored information in 
	// HKEY_LOCAL_MACHINE to each user's HKEY_CURRENT_USER when the information is needed. The location
	// of the mirrored registry entries is: 
	// HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\TerminalServer\Install.
	tstrcmpinc(&sKeyName, _T("SOFTWARE\\MICROSOFT\\WINDOWS NT\\CURRENTVERSION\\TERMINALSERVER\\INSTALL\\"));

	uint_fast32_t arh_flags = 0;
	bool bSubkeyMask = false;
	if (sKey.m_controlset_subkey)
	{
		sKeyName = sKey.m_controlset_subkey;
		arh_flags = ARH_CONTROLSET;
	}
	else if (sKey.m_root == crHKEY_CLASSES_ROOT)
	{
		arh_flags = ARH_SOFTWARE_CLASSES;
	}
	else
		GetHint(&sKeyName, NULL, NULL, NULL, &arh_flags);

	for (int i=0; i<countof(arrAutoRunsKeyNames); i++)
	{
		if (arh_flags != arrAutoRunHints[i].arh_flags)
			continue;

		// check value first - it faster
		tcstring value = arrAutoRunsKeyNames[i].value;
		if (value)
		{
			if (arrAutoRunHints[i].bValueMask)
			{
				if (!tstrMatchWithPattern(sValueName, value, false))
					continue;
			}
			else
			{
				if (0 != tstricmp(sValueName, value))
					continue;
			}
		}
		// check key
		if (arrAutoRunHints[i].bSubkeyMask)
		{
			if (!tstrMatchWithPattern(sKeyName, arrAutoRunHints[i].subkey, true))
				continue;
		}
		else
		{
			if (0 != tstrcmp(sKeyName, arrAutoRunHints[i].subkey))
				continue;
		}
		if (pnARFlags)
			*pnARFlags = arrAutoRunsKeyNames[i].flags;
		return true;
	}
	return false;
}
