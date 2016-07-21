// Register.cpp: implementation of the CRegister class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Register.h"
#include <boost/scoped_array.hpp>
#include "PragueHelper.h"
#include <vector>
#include <set>
//#include "Tracer.h"
#include "Tracing.h"
#include "version/ver_product.h"

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
	LPCTSTR AVCONFIG_INI = _T("AVConfig.ini");

	LPCTSTR INI_GENERAL_SECTION = _T("General");
	LPCTSTR GENERAL_CHECKOPENATTACH = _T("CheckOpenAttach");
	LPCTSTR GENERAL_CHECKSAVEATTACH = _T("CheckSaveAttach");
	LPCTSTR GENERAL_CHECKOUTGOING = _T("CheckOutgoing");
	LPCTSTR GENERAL_CHECKMAIL = _T("CheckMail");
	LPCTSTR GENERAL_AVACTION = _T("Action");

	LPCTSTR INI_CHECKERS_SECTION = _T("Checkers");
	LPCTSTR CHECKERS_COUNT = _T("Count");
	LPCTSTR CHECKERS_NAME = _T("Checker #");
	LPCTSTR CHECKERS_AVDLL = _T("TAVDLLPlugin.");

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

tstring _GetAVConfigPath(HKEY hKeyParent)
{
	// get TheBat! working directory from registry
	CRegKey keyTheBat;
	if (keyTheBat.Open(hKeyParent, TheBat::REG_KEY, KEY_READ) != ERROR_SUCCESS)
		return _T("");
	
	tstring strWorkingDir(PragueHelper::GetRegStringValue(keyTheBat, TheBat::WORKINGDIR_VALUE));
	if (strWorkingDir.empty())
		return _T("");
	
	// prepare the AV-settings file name
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

	strRet += TheBat::AVCONFIG_INI;
	return strRet.c_str();
}

int _AddPluginToIni(HKEY hKeyParent, LPCTSTR szIniFile, LPCTSTR szPluginPath)
{
	tstring strAVConfigPath(szIniFile ? szIniFile : _T(""));
	if (strAVConfigPath.size() == 0)
		return errNOT_FOUND;

	// make sure AV-settings are set to 'Check all mail'
	AVTRACE(_T("Writing AV options to INI file..."));
	WritePrivateProfileString(TheBat::INI_GENERAL_SECTION, TheBat::GENERAL_CHECKOPENATTACH, 
		_T("0"), strAVConfigPath.c_str());
	WritePrivateProfileString(TheBat::INI_GENERAL_SECTION, TheBat::GENERAL_CHECKOUTGOING, 
		_T("1"), strAVConfigPath.c_str());
	WritePrivateProfileString(TheBat::INI_GENERAL_SECTION, TheBat::GENERAL_CHECKMAIL, 
		_T("1"), strAVConfigPath.c_str());
	
	// now see if we are installed as a Checker
	// first, get the number of checkers already installed
	tstring strPluginPath(szPluginPath);
	int nCheckers = GetPrivateProfileInt(TheBat::INI_CHECKERS_SECTION, TheBat::CHECKERS_COUNT, 0, strAVConfigPath.c_str());
	
	USES_CONVERSION;
	tstring msg = _T("Looking through ");
	msg += A2CT((boost::lexical_cast<std::string>(nCheckers)).c_str());
	msg += _T(" checkers");
	AVTRACE(msg.c_str());
	
	for (int n = 0; n < nCheckers; ++n)
	{
		// see if we are already installed in this key
		tstring strChkName(TheBat::CHECKERS_NAME);
		strChkName += boost::lexical_cast<tstring>(n + 1);
		
		TCHAR szBuf[1048] = {0};
		GetPrivateProfileString(TheBat::INI_CHECKERS_SECTION, strChkName.c_str(), _T(""),
			szBuf, 1048, strAVConfigPath.c_str());
		
		if (!szBuf)
			continue;
		
		strChkName = _ExpandBatStrings(hKeyParent, szBuf);
		if (strChkName.size() < strPluginPath.size())
			continue;
		
		// compare the Right(strPluginPath.size) 
		if (strChkName.substr(strChkName.size() - strPluginPath.size()) == szPluginPath)
		{
			AVTRACE(_T("We're already installed"));
			return errOK;	// found it!
		}
	}
	
	// our path was not found, add it now
	tstring strChkName(TheBat::CHECKERS_NAME);
	strChkName += boost::lexical_cast<tstring>(nCheckers + 1);
	
	strPluginPath = TheBat::CHECKERS_AVDLL;
	strPluginPath += szPluginPath;
	
	WritePrivateProfileString(TheBat::INI_CHECKERS_SECTION, strChkName.c_str(), strPluginPath.c_str(), 
		strAVConfigPath.c_str());
	WritePrivateProfileString(TheBat::INI_CHECKERS_SECTION, TheBat::CHECKERS_COUNT, 
		(boost::lexical_cast<tstring>(nCheckers + 1)).c_str(), strAVConfigPath.c_str());
	
	msg = _T("Added our plugin (");
	msg += strChkName;
	msg += _T(')');
	AVTRACE(msg.c_str());
	return errOK;
}

int _DeletePluginFromIni(HKEY hKeyParent, LPCTSTR szIniFile, LPCTSTR szPluginPath)
{
	tstring strAVConfigPath(szIniFile ? szIniFile : _T(""));
	if (strAVConfigPath.size() == 0)
		return errNOT_FOUND;

	// store all checkers in an array
	// find a checker with our path and delete it
	// restore all checkers one by one
	typedef std::vector<tstring> CHECKERS_ARRAY;
	CHECKERS_ARRAY arr;
	bool bWeAreInstalled = false;
	
	tstring strPluginPath(szPluginPath);
	int nCheckers = GetPrivateProfileInt(TheBat::INI_CHECKERS_SECTION, TheBat::CHECKERS_COUNT, 0, strAVConfigPath.c_str());
	
	USES_CONVERSION;
	tstring msg = _T("Looking through ");
	msg += A2CT((boost::lexical_cast<std::string>(nCheckers)).c_str());
	msg += _T(" checkers");
	AVTRACE(msg.c_str());
	
	int n;
	for (n = 0; n < nCheckers; ++n)
	{
		tstring strChkName(TheBat::CHECKERS_NAME);
		strChkName += boost::lexical_cast<tstring>(n + 1);
		
		TCHAR szBuf[1048] = {0};
		GetPrivateProfileString(TheBat::INI_CHECKERS_SECTION, strChkName.c_str(), _T(""),
			szBuf, 1048, strAVConfigPath.c_str());
		
		if (!szBuf)
			continue;
		
		tstring strValue = _ExpandBatStrings(hKeyParent, szBuf);
		
		// now see if this checker is 'us' (by comparing the Right(strPluginPath.size))
		if ((strValue.size() >= strPluginPath.size()) && 
			(strValue.substr(strValue.size() - strPluginPath.size()) == szPluginPath))
		{
			// it's our plugin
			bWeAreInstalled = true;
			AVTRACE(_T("Found our plugin, skipping..."));
		}
		else
			arr.push_back(strValue);
		
		// delete this value from the INI-file
		WritePrivateProfileString(TheBat::INI_CHECKERS_SECTION, strChkName.c_str(), NULL, strAVConfigPath.c_str());
	}
	
	// now re-add all checkers (except our own)
	WritePrivateProfileString(TheBat::INI_CHECKERS_SECTION, TheBat::CHECKERS_COUNT, 
		(boost::lexical_cast<tstring>(arr.size())).c_str(), strAVConfigPath.c_str());
	
	size_t s;
	for (s = 0; s < arr.size(); ++s)
	{
		tstring strChkName(TheBat::CHECKERS_NAME);
		strChkName += boost::lexical_cast<tstring>(s + 1);
		
		WritePrivateProfileString(TheBat::INI_CHECKERS_SECTION, strChkName.c_str(), arr[s].c_str(), 
			strAVConfigPath.c_str());
	}

	return errOK;
}

int CRegister::RegisterAVPlugin(LPCTSTR szPluginPath)
{
	tstring msg(_T("Started registering ("));
	msg += szPluginPath ? szPluginPath : _T("NULL");
	msg += _T(')');
	AVTRACE(msg.c_str());

	if (!szPluginPath)
		return errPARAMETER_INVALID;

	// enumerate the users and inject ourselves wherever it would be possible
	msg = _T("Enumerating users...");
	AVTRACE(msg.c_str());

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

		tstring strAVConfigPath(_GetAVConfigPath(regUser));
		if (!strAVConfigPath.size() || arrPaths.count(strAVConfigPath) > 0)
			continue;

		arrPaths.insert(strAVConfigPath);
		msg = tstring(_T("Got AVConfig Path : ")) + strAVConfigPath;
		AVTRACE(msg.c_str());

		if(PR_SUCC(_AddPluginToIni(regUser, strAVConfigPath.c_str(), szPluginPath)))
		{
			// выставим флажок, говорящий сервису больше не пытаться регистрировать плагин
			// для этого аккаунта
			CRegKey regTB;
			if(regTB.Open(regUser, VER_PRODUCT_REGISTRY_PATH "\\TBPlugins", KEY_READ | KEY_WRITE) == ERROR_SUCCESS)
				regTB.SetDWORDValue(szThis, 1);
		}
	}
	
	return errOK;
}

int CRegister::UnregisterAVPlugin(LPCTSTR szPluginPath)
{
	tstring msg(_T("Started unregistering ("));
	msg += szPluginPath ? szPluginPath : _T("NULL");
	msg += _T(')');
	AVTRACE(msg.c_str());

	if (!szPluginPath)
		return errPARAMETER_INVALID;

	// enumerate the users and inject ourselves wherever it would be possible
	msg = _T("Enumerating users...");
	AVTRACE(msg.c_str());
	
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
		
		tstring strAVConfigPath(_GetAVConfigPath(regUser));
		if (!strAVConfigPath.size() || arrPaths.count(strAVConfigPath) > 0)
			continue;
		
		arrPaths.insert(strAVConfigPath);
		msg = tstring(_T("Got AVConfig Path : ")) + strAVConfigPath;
		AVTRACE(msg.c_str());
		
		_DeletePluginFromIni(regUser, strAVConfigPath.c_str(), szPluginPath);
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