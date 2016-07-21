// Register.cpp: implementation of the CRegister class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Register.h"
#include <boost/scoped_array.hpp>
#include "../TheBatPlugin/PragueHelper.h"
//#include "../TheBatPlugin/Tracer.h"
#include "Tracing.h"
#include "version/ver_product.h"
#include <vector>
#include <set>

#pragma warning(push)
#pragma warning(disable:4800)
#include <boost/lexical_cast.hpp>
#pragma warning(pop)

#include <Prague/pr_compl.h>	//
#include <Prague/pr_types.h>	// all these are included only for tERROR type support
#include <Prague/pr_err.h>		//

namespace TheBat
{
	LPCTSTR REG_KEY = _T("Software\\RIT\\The Bat!");
	LPCTSTR WORKINGDIR_VALUE = _T("Working Directory");
	LPCTSTR TBPLUGIN_INI = _T("TBPlugin.INI");
	
	LPCTSTR INI_ANTISPAM_SECTION = _T("AntiSpam");
	LPCTSTR ANTISPAM_JUNK = _T("Junk");
	LPCTSTR ANTISPAM_MARKJUNKTOJUNK = _T("MarkJunkToJunk");
	
	LPCTSTR INI_PLUGINS_SECTION = _T("Plugins");
	LPCTSTR PLUGIN_COUNT = _T("Count");
	LPCTSTR PLUGIN_NAME = _T("Plugin #");
	
	LPCTSTR EXEDRIVE_VALUE = _T("%EXEDRIVE%");
	LPCTSTR EXEPATH_VALUE = _T("EXE path");
	LPCTSTR PROGDIR_VALUE = _T("ProgramDir");

	LPCTSTR SHELLFOLDERS_KEY = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders");
	LPCTSTR APPDATA_VALUE = _T("AppData");
	LPCTSTR APPDATA_VAR = _T("%APPDATA%");
}

// may return something like "C:\Program Files\The Bat!\thebat.exe"
// or "C:\Program Files\The Bat!\"
tstring _GetEXEPath(HKEY hKeyParent)
{
	CRegKey keyTheBat;
	if (keyTheBat.Open(hKeyParent, TheBat::REG_KEY, KEY_READ) != ERROR_SUCCESS)
		return _T("");
	
	tstring strExePath(PragueHelper::GetRegStringValue(keyTheBat, TheBat::EXEPATH_VALUE));
	if (!strExePath.empty())
		return strExePath;
	
	tstring strProgramDir(PragueHelper::GetRegStringValue(keyTheBat, TheBat::PROGDIR_VALUE));
	if (!strProgramDir.empty())
		return strProgramDir;
	
	return _T("C:\\");
}

tstring _ExpandBatStrings(HKEY hKeyParent, LPCTSTR szSource)
{
	if (!szSource)
		return _T("");
	
	tstring strRet(szSource);
	tstring::size_type nPos = strRet.find(TheBat::EXEDRIVE_VALUE);
	if (nPos == tstring::npos)
		return szSource;
	
	tstring strEXEPath(_GetEXEPath(hKeyParent));
	if (strEXEPath.empty())
		return szSource;
	
	tstring strExeDrive(TheBat::EXEDRIVE_VALUE);
	strRet.replace(nPos, strExeDrive.size(), strEXEPath.substr(0, 2));
	
	return strRet;
}

tstring _GetAppDataValue(HKEY hKeyParent)
{
	CRegKey keyShellFolders;
	if (keyShellFolders.Open(hKeyParent, TheBat::SHELLFOLDERS_KEY, KEY_READ) != ERROR_SUCCESS)
		return _T("");
	
	return PragueHelper::GetRegStringValue(keyShellFolders, TheBat::APPDATA_VALUE);
}

tstring _GetTBPConfigPath(HKEY hKeyParent)
{
	// get TheBat! working directory from registry
	CRegKey keyTheBat;
	if (keyTheBat.Open(hKeyParent, TheBat::REG_KEY, KEY_READ) != ERROR_SUCCESS)
		return _T("");
	
	tstring strWorkingDir(PragueHelper::GetRegStringValue(keyTheBat, TheBat::WORKINGDIR_VALUE));
	if (strWorkingDir.empty())
		return _T("");
	
	// prepare the TBP-settings file name
	TCHAR szFullPath[2048] = {0};
	ExpandEnvironmentStrings(strWorkingDir.c_str(), szFullPath, 2048);
	if (!szFullPath)
		return _T("");
	
	tstring strRet(szFullPath);
	if(!strRet.empty())
		std::_USE(std::locale(), std::ctype<tstring::traits_type::char_type>).
			toupper(&strRet[0], &strRet[strRet.size()]);	// just in case...
	
	tstring::size_type nPos = strRet.find(TheBat::APPDATA_VAR);
	if (nPos != tstring::npos)
	{
		// found %APPDATA%. Hmm, obviously ExpandEnvironmentStrings couldn't do its job (we're under LocalSystem?)
		// try to retrieve AppData value by ourselves
		strWorkingDir = TheBat::APPDATA_VAR;	// to determine its size later
		tstring strAppData(_GetAppDataValue(hKeyParent));
		if (strAppData.size())
			strRet.replace(nPos, strWorkingDir.size(), strAppData);
	}	
	
	strRet += TheBat::TBPLUGIN_INI;
	return strRet.c_str();
}

int _AddPluginToIni(HKEY hKeyParent, LPCTSTR szIniFile, LPCTSTR szPluginPath)
{
	tstring strTBPConfigPath(szIniFile ? szIniFile : _T(""));
	if (strTBPConfigPath.size() == 0)
		return errNOT_FOUND;

	// make sure TBP-settings are set to 'Move spam to junk'
	ASTRACE(_T("Writing AntiSpam options to INI file..."));
	WritePrivateProfileString(TheBat::INI_ANTISPAM_SECTION, TheBat::ANTISPAM_JUNK, 
		_T("1"), strTBPConfigPath.c_str());
	WritePrivateProfileString(TheBat::INI_ANTISPAM_SECTION, TheBat::ANTISPAM_MARKJUNKTOJUNK, 
		_T("1"), strTBPConfigPath.c_str());
	
	// now see if we are installed as a AntiSpam-plugin
	// first, get the number of plugins already installed
	tstring strPluginPath(szPluginPath);
	int nCheckers = GetPrivateProfileInt(TheBat::INI_PLUGINS_SECTION, TheBat::PLUGIN_COUNT, 0, strTBPConfigPath.c_str());
	
	USES_CONVERSION;
	tstring msg = _T("Looking through ");
	msg += A2CT((boost::lexical_cast<std::string>(nCheckers)).c_str());
	msg += _T(" checkers");
	ASTRACE(msg.c_str());
	
	for (int n = 0; n < nCheckers; ++n)
	{
		// see if we are already installed in this key
		tstring strChkName(TheBat::PLUGIN_NAME);
		strChkName += boost::lexical_cast<tstring>(n + 1);
		
		TCHAR szBuf[1024] = {0};
		GetPrivateProfileString(TheBat::INI_PLUGINS_SECTION, strChkName.c_str(), _T(""),
			szBuf, 1024, strTBPConfigPath.c_str());
		
		if (!szBuf)
			continue;
		
		if (strPluginPath == _ExpandBatStrings(hKeyParent, szBuf))
		{
			ASTRACE(_T("We're already installed"));
			return errOK;	// found it! No need to register again
		}
	}
	
	// our path was not found, add it now
	tstring strChkName(TheBat::PLUGIN_NAME);
	strChkName += boost::lexical_cast<tstring>(nCheckers + 1);
	strPluginPath = szPluginPath;
	
	WritePrivateProfileString(TheBat::INI_PLUGINS_SECTION, strChkName.c_str(), strPluginPath.c_str(), 
		strTBPConfigPath.c_str());
	WritePrivateProfileString(TheBat::INI_PLUGINS_SECTION, TheBat::PLUGIN_COUNT, 
		(boost::lexical_cast<tstring>(nCheckers + 1)).c_str(), strTBPConfigPath.c_str());
	
	msg = _T("Added our plugin (");
	msg += strChkName;
	msg += _T(')');
	ASTRACE(msg.c_str());

	return errOK;
}

int _DeletePluginFromIni(HKEY hKeyParent, LPCTSTR szIniFile, LPCTSTR szPluginPath)
{
	tstring strTBPConfigPath(szIniFile ? szIniFile : _T(""));
	if (strTBPConfigPath.size() == 0)
		return errNOT_FOUND;

	// store all plugins in an array
	// find a plugin with our path and delete it (that is, don't store it in the array)
	// restore all other plugins one by one
	typedef std::vector<tstring> PLUGINS_ARRAY;
	PLUGINS_ARRAY arr;
	bool bWeAreInstalled = false;
	
	tstring strPluginPath(szPluginPath);
	int nCheckers = GetPrivateProfileInt(TheBat::INI_PLUGINS_SECTION, TheBat::PLUGIN_COUNT, 0, strTBPConfigPath.c_str());
	
	USES_CONVERSION;
	tstring msg = _T("Looking through ");
	msg += A2CT((boost::lexical_cast<std::string>(nCheckers)).c_str());
	msg += _T(" checkers");
	ASTRACE(msg.c_str());
	
	int n;
	for (n = 0; n < nCheckers; ++n)
	{
		tstring strChkName(TheBat::PLUGIN_NAME);
		strChkName += boost::lexical_cast<tstring>(n + 1);
		
		TCHAR szBuf[1024] = {0};
		GetPrivateProfileString(TheBat::INI_PLUGINS_SECTION, strChkName.c_str(), _T(""),
			szBuf, 1024, strTBPConfigPath.c_str());
		
		if (!szBuf)
			continue;
		
		// now see if this plugin is 'us'
		if (strPluginPath == _ExpandBatStrings(hKeyParent, szBuf))
		{
			// it's our plugin
			bWeAreInstalled = true;
			ASTRACE(_T("Found our plugin, skipping..."));
		}
		else
			arr.push_back(szBuf);
		
		// delete this value from the INI-file
		WritePrivateProfileString(TheBat::INI_PLUGINS_SECTION, strChkName.c_str(), NULL, strTBPConfigPath.c_str());
	}
	
	// now re-add all checkers (except our own)
	WritePrivateProfileString(TheBat::INI_PLUGINS_SECTION, TheBat::PLUGIN_COUNT, 
		(boost::lexical_cast<tstring>(arr.size())).c_str(), strTBPConfigPath.c_str());
	
	for (n = 0; n < arr.size(); ++n)
	{
		tstring strChkName(TheBat::PLUGIN_NAME);
		strChkName += boost::lexical_cast<tstring>(n + 1);
		
		WritePrivateProfileString(TheBat::INI_PLUGINS_SECTION, strChkName.c_str(), arr[n].c_str(), 
			strTBPConfigPath.c_str());
	}
	
	return errOK;
}

int CRegister::RegisterASPlugin(LPCTSTR szPluginPath)
{
	tstring msg(_T("Started registering ("));
	msg += szPluginPath ? szPluginPath : _T("NULL");
	msg += _T(')');
	ASTRACE(msg.c_str());

	if (!szPluginPath)
		return errPARAMETER_INVALID;

	// enumerate the users and inject ourselves wherever it would be possible
	msg = _T("Enumerating users...");
	ASTRACE(msg.c_str());
	
	std::set<tstring> arrPaths;
	TCHAR szSubkey[MAX_PATH];
	DWORD dwSubkeyLen = MAX_PATH;
	FILETIME ftDummy;
	
	for (DWORD dwIndex = 0; 
		RegEnumKeyEx(HKEY_USERS, dwIndex, szSubkey, &dwSubkeyLen, NULL, NULL, NULL, &ftDummy) == ERROR_SUCCESS;
		++dwIndex, dwSubkeyLen = MAX_PATH)
	{
		CRegKey regUser;
		if (regUser.Open(HKEY_USERS, szSubkey, KEY_READ) != ERROR_SUCCESS)
			continue;
		
		LPCTSTR pLastSlash = _tcsrchr(szPluginPath, '\\');
		LPCTSTR szThis = pLastSlash ? pLastSlash + 1 : szPluginPath;
		{
			CRegKey regTB;
			DWORD dwRegistered = 0;
			if(regTB.Open(regUser, VER_PRODUCT_REGISTRY_PATH "\\TBPlugins", KEY_READ) == ERROR_SUCCESS &&
				regTB.QueryDWORDValue(szThis, dwRegistered) == ERROR_SUCCESS && dwRegistered)
			{
				// уже был успешно зарегистрирован для этого аккаунта
				continue;
			}
		}

		tstring strASConfigPath(_GetTBPConfigPath(regUser));
		if (!strASConfigPath.size() || arrPaths.count(strASConfigPath) > 0)
			continue;
		
		arrPaths.insert(strASConfigPath);
		msg = tstring(_T("Got TBConfig Path : ")) + strASConfigPath;
		ASTRACE(msg.c_str());
		
		if(PR_SUCC(_AddPluginToIni(regUser, strASConfigPath.c_str(), szPluginPath)))
		{
			// выставим флажок, говорящий сервису больше не пытаться регистрировать плагин
			// для этого аккаунта
			CRegKey regTB;
			if(regTB.Open(regUser, VER_PRODUCT_REGISTRY_PATH "\\TBPlugins", KEY_READ | KEY_WRITE) == ERROR_SUCCESS)
			{
				LPCTSTR pLastSlash = _tcsrchr(szPluginPath, '\\');
				LPCTSTR szThis = pLastSlash ? pLastSlash + 1 : szPluginPath;
				regTB.SetDWORDValue(szThis, 1);
			}
		}
	}
	
	return errOK;
}

int CRegister::UnregisterASPlugin(LPCTSTR szPluginPath)
{
	tstring msg(_T("Started unregistering ("));
	msg += szPluginPath ? szPluginPath : _T("NULL");
	msg += _T(')');
	ASTRACE(msg.c_str());

	if (!szPluginPath)
		return errPARAMETER_INVALID;
	
	// enumerate the users and inject ourselves wherever it would be possible
	msg = _T("Enumerating users...");
	ASTRACE(msg.c_str());
	
	std::set<tstring> arrPaths;
	TCHAR szSubkey[MAX_PATH];
	DWORD dwSubkeyLen = MAX_PATH;
	FILETIME ftDummy;
	
	for (DWORD dwIndex = 0; 
		RegEnumKeyEx(HKEY_USERS, dwIndex, szSubkey, &dwSubkeyLen, NULL, NULL, NULL, &ftDummy) == ERROR_SUCCESS;
		++dwIndex, dwSubkeyLen = MAX_PATH)
	{
		CRegKey regUser;
		if (regUser.Open(HKEY_USERS, szSubkey, KEY_READ) != ERROR_SUCCESS)
			continue;
		
		tstring strTBPConfigPath(_GetTBPConfigPath(regUser));
		if (!strTBPConfigPath.size() || arrPaths.count(strTBPConfigPath) > 0)
			continue;
		
		arrPaths.insert(strTBPConfigPath);
		msg = tstring(_T("Got TBPConfig Path : ")) + strTBPConfigPath;
		ASTRACE(msg.c_str());
		
		_DeletePluginFromIni(regUser, strTBPConfigPath.c_str(), szPluginPath);
	}
	
	return errOK;
}

int CRegister::CleanRegistrationFlag(LPCTSTR szPluginPath)
{
	if (!szPluginPath)
		return errPARAMETER_INVALID;

	TCHAR szSubkey[MAX_PATH];
	DWORD dwSubkeyLen = MAX_PATH;
	FILETIME ftDummy;

	for (DWORD dwIndex = 0; 
		RegEnumKeyEx(HKEY_USERS, dwIndex, szSubkey, &dwSubkeyLen, NULL, NULL, NULL, &ftDummy) == ERROR_SUCCESS;
		++dwIndex, dwSubkeyLen = MAX_PATH)
	{
		CRegKey regUser;
		if (regUser.Open(HKEY_USERS, szSubkey, KEY_READ) != ERROR_SUCCESS)
			continue;
		CRegKey regTB;
		if(regTB.Open(regUser, VER_PRODUCT_REGISTRY_PATH "\\TBPlugins", KEY_READ | KEY_WRITE) == ERROR_SUCCESS)
		{
			LPCTSTR pLastSlash = _tcsrchr(szPluginPath, '\\');
			LPCTSTR szThis = pLastSlash ? pLastSlash + 1 : szPluginPath;
			regTB.DeleteValue(szThis);
		}
	}

	return errOK;
}