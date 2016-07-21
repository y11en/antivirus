#include "helper.h"
#include "configuration.h"
#include "include/ver_mod.h"

shared_ptr<Logger> Logger::instance_ptr;
bool Logger::bInited = false;

bool helper::DeleteDirectory (const char* dir)
{
	int ret=0;
	char name1[256];
	WIN32_FIND_DATA info;
    HANDLE hp;
    char *cp;
	
	sprintf(name1, "%s\\*.*",dir);
    hp = FindFirstFile(name1,&info);
    if(!hp || hp==INVALID_HANDLE_VALUE)
        return (true);
    do
    {
		cp = info.cFileName;
        if(cp[1]==0 && *cp=='.')
            continue;
        else if(cp[2]==0 && *cp=='.' && cp[1]=='.')
            continue;
        sprintf(name1,"%s\\%s",dir,info.cFileName);
		if(info.dwFileAttributes&FILE_ATTRIBUTE_READONLY)
		{
			SetFileAttributes(name1,info.dwFileAttributes&~FILE_ATTRIBUTE_READONLY);
		}
		if(info.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			DeleteDirectory(name1);
			else DeleteFile(name1);

    }
    while(FindNextFile(hp,&info));

	FindClose(hp);
	if(info.dwFileAttributes&FILE_ATTRIBUTE_READONLY)
	{
		SetFileAttributes(dir,info.dwFileAttributes&~FILE_ATTRIBUTE_READONLY);
	}

	return (0 != RemoveDirectory(dir));
}

bool helper::MakeDirectory (const char* dir)
{
	return (0 != CreateDirectory (dir, NULL));
};

bool helper::IsFoldersEqual (const string& tool, const string& dir1, const string& dir2)
{
	string tool_exe = string (tool).append ("\\ExamDiff.exe");
	string tool_rep = string (tool).append ("\\report.txt");
	char* comm_line = strdup (string (tool_exe).append(" ").append (dir1).append(" ").append(dir2).append(" /r2 /!z /o:").append(tool_rep).c_str());
	
	DeleteFile (tool_rep.c_str());
	//ShellExecute (NULL, "open", tool_exe.c_str(),
	//			  string (dir1).append(" ").append(dir2).append(" /r2 /!z /o:").append(tool_rep).c_str(), 
	//			  NULL, SW_SHOWNORMAL);
	STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );	
	
	if (!CreateProcess (NULL, comm_line, 
						NULL,NULL,FALSE,0,NULL,NULL,&si,&pi)) return false;

	int resWait = WaitForSingleObject (pi.hProcess, 10000);
	CloseHandle (pi.hThread);
	CloseHandle (pi.hProcess);
	free (comm_line);
	
	if (WAIT_TIMEOUT != resWait)
	{
		AutoFile hFile (tool_rep.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
		char buf[21];
		DWORD dwRead;
		if (ReadFile (hFile.GetHandle(), buf, 21, &dwRead, NULL) && !(buf[21] = 0) && !strcmp (buf, "IDENTICAL DIRECTORIES"))
			return true;
	};
	return false;
};

void helper::StartNetworkAgent ()
{
	char line[] = "net start klnagent";

	STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );	
	
	if (CreateProcess (NULL, line, 
						NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&si,&pi))
	{
		int resWait = WaitForSingleObject (pi.hProcess, INFINITE);
		CloseHandle (pi.hThread);
		CloseHandle (pi.hProcess);
	};
	Sleep (2000);
};

void helper::StopNetworkAgent ()
{
	char line[] = "net stop klnagent";

	STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );	
	
	if (CreateProcess (NULL, line, 
						NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&si,&pi))
	{
		int resWait = WaitForSingleObject (pi.hProcess, INFINITE);
		CloseHandle (pi.hThread);
		CloseHandle (pi.hProcess);
	};
	Sleep (2000);
};

bool helper::RecreateDirectories (vector<string>& dirs)
{
	for (vector<string>::iterator it = dirs.begin(); it != dirs.end(); ++it)
	{
		//BOOST_MESSAGE ("Deleting destination folder");
		//BOOST_WARN_MESSAGE (helper::DeleteDirectory (config_ft.destination.c_str()), config_ft.destination.c_str());
		//BOOST_MESSAGE ("Creating destination folder");
		//BOOST_REQUIRE_MESSAGE (helper::MakeDirectory (config_ft.destination.c_str()), config_ft.destination.c_str());
		if (!it->c_str()) continue;
		helper::DeleteDirectory (it->c_str());
		helper::MakeDirectory (it->c_str());
	};
	return true;
};

int helper::CloneFile (LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bFailIfExists)
{
	SetFileAttributes (lpExistingFileName, FILE_ATTRIBUTE_NORMAL);
	return CopyFile (lpExistingFileName, lpNewFileName, bFailIfExists);
};


AutoFile::AutoFile (const char* filename, DWORD desireAccess, DWORD dwShareMode, DWORD dwCreationDisposition)
: bLocked (false)
{
	SetFileAttributes (filename, FILE_ATTRIBUTE_NORMAL);
	hFile = CreateFile (filename, desireAccess, FILE_SHARE_READ, NULL, dwCreationDisposition, NULL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) throw -1;
};

AutoFile::~AutoFile ()
{
	if (hFile != INVALID_HANDLE_VALUE)
	{
		OVERLAPPED ovr;
		ZeroMemory (&ovr, sizeof(OVERLAPPED));
		if (bLocked) ::UnlockFileEx (hFile, 0, GetFileSize (), 0, &ovr);
		CloseHandle (hFile);
	};
};

int AutoFile::GetFileSize ()
{
	DWORD size;
	DWORD size_low = ::GetFileSize (hFile, &size);
	return size_low;
};

HANDLE AutoFile::GetHandle ()
{
	return hFile;
};

void AutoFile::LockFile ()
{ 
	OVERLAPPED ovr;
	ZeroMemory (&ovr, sizeof(OVERLAPPED));
	bLocked = ::LockFileEx (hFile, LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY, 0, GetFileSize (), 0, &ovr);
};

AuthorizationInformation_Unit::AuthorizationInformation_Unit (char* user, char* pass, char* request)
{
	m_userName = user;
	m_password = pass;
	//m_httpAuthorizationPhrase = request;
};

AuthorizationInformation_Unit::~AuthorizationInformation_Unit ()
{ 
};

bool AuthorizationInformation_Unit::operator== (const AuthorizationInformation& nai)
{
	return ((m_userName == nai.m_userName) && (m_password == nai.m_password)/* &&
				(m_httpAuthorizationPhrase == nai.m_httpAuthorizationPhrase)*/);
};

AddressInformation_Unit::AddressInformation_Unit (	Protocol m_protocol,
											std::string m_fullUrlFromUser,
											std::string m_hostName,
											std::string m_IPaddress,
											std::string m_relativeUrlPath,
											unsigned short m_port)
{
	this->m_protocol = m_protocol;
	this->m_fullUrlFromUser = m_fullUrlFromUser;
	this->m_hostName = m_hostName;
	//this->m_IPaddress = m_IPaddress;
	this->m_relativeUrlPath = m_relativeUrlPath;

	this->m_port = m_port;
};

AddressInformation_Unit::~AddressInformation_Unit ()
{
    sock = 0;
    data_sock = 0;
};

bool AddressInformation_Unit::operator== (const AddressInformation& nui)
{
	bool bResult =
		(	m_protocol == nui.m_protocol &&
			m_relativeUrlPath == nui.m_relativeUrlPath
		);
	
	if (bResult && !m_fullUrlFromUser.empty()) bResult &= (m_fullUrlFromUser == nui.m_fullUrlFromUser);
	if (bResult && !m_hostName.empty()) bResult &= (m_hostName == nui.m_hostName);
	//if (bResult && !m_IPaddress.empty()) bResult &= (m_IPaddress == nui.m_IPaddress);
	if (bResult && (m_protocol != AddressInformation::Protocol::fileProtocol) ) bResult &= (m_port == nui.m_port);


    /// pointer to socket through which this url's transmittions is worked
    //stdSocket *sock;
    /// the socket for data connection for passive ftp connections
    //stdSocket *data_sock;
	return bResult;
};


NetFileInfo_Unit::NetFileInfo_Unit (const char* filename, unsigned long size): NetFileInfo (filename)
{
	this->m_size = size;
};

bool NetFileInfo_Unit::operator== (const NetFileInfo& nfi)
{
	return ((m_fileName == nfi.m_fileName) && (m_size == nfi.m_size));
};

NetFileInfoPointer_Unit::NetFileInfoPointer_Unit (	const char *name,
													const char *rpath,
													const char *m_path,
													unsigned long rsize,
													unsigned long lsize,
													FILE *file
												)
{
	this->m_name = name;
	this->m_relativeServerPath = rpath;
	this->m_localPath = m_path;
	this->m_remoteSize = rsize;
	this->m_localSize = lsize;
	this->file = file;
};

NetFileInfoPointer_Unit::~NetFileInfoPointer_Unit()
{
	file = 0;
};

bool NetFileInfoPointer_Unit::operator == (const NetFileInfoPointer& nfip)
{
	return ((nfip.m_name == m_name) && (nfip.m_relativeServerPath == m_relativeServerPath)
			&& (nfip.m_localPath == m_localPath)
				&& (nfip.m_remoteSize == m_remoteSize) && (nfip.m_localSize == m_localSize));
};

bool FileInfo_Unit::operator== (const FileInfo& right)
{
	bool bRes;
	bRes = (m_comment == right.m_comment &&
			m_dateFromIndex == right.m_dateFromIndex &&
			m_filename == right.m_filename &&
			m_signature == right.m_signature &&
			m_fromAppVersion == right.m_fromAppVersion &&
			m_fromCompVersion == right.m_fromCompVersion &&
			m_localPath == right.m_localPath &&
			m_primaryOrSubstitutedIndex == right.m_primaryOrSubstitutedIndex &&
			m_rollbackChange == right.m_rollbackChange &&
			m_originalLocalPath == right.m_originalLocalPath &&
			m_relativeURLPath == right.m_relativeURLPath &&
			m_size == right.m_size &&
			m_toAppVersion == right.m_toAppVersion &&
			m_toCompVersion == right.m_toCompVersion &&
			m_componentIdSet == right.m_componentIdSet &&
			m_obligatoryEntry == right.m_obligatoryEntry &&
			m_type == right.m_type &&
			m_language == right.m_language &&
			m_OSes.size() == right.m_OSes.size());
	if (bRes)
	{
		for (int i = 0; i< m_OSes.size(); ++i)
		{
			if ((m_OSes[i].m_name != right.m_OSes[i].m_name) || (m_OSes[i].m_versionSet != right.m_OSes[i].m_versionSet))
					return false;
		};
	};
	return bRes;
};

bool operator== (std::vector<FileInfo_Unit, std::allocator<FileInfo_Unit> >& fiu,
				 const std::vector<FileInfo, std::allocator<FileInfo> >& fi)
{
	if (fiu.size() != fi.size()) return false;
	for (int i = 0; i < fiu.size(); ++i)
		if (!(fiu[i] == fi[i])) return false;
	return true;
};

bool SitesInfo_Unit::operator ==(const SitesInfo& right)
{
	return (
		m_RegionList == right.m_RegionList &&
		srvRegion == right.srvRegion &&
		srvURL == right.srvURL &&
		srvWeight == right.srvWeight
		);
}

bool operator== (std::vector<SitesInfo_Unit, std::allocator<SitesInfo_Unit> >& siu,
				 const std::vector<SitesInfo, std::allocator<SitesInfo> >& si)
{
	if (siu.size() != si.size()) return false;
	for (int i = 0; i < siu.size(); ++i)
		if (!(siu[i] == si[i])) return false;
	return true;
};


Type1Message_Unit::Type1Message_Unit (string hostname, string domenname): host(hostname), domen(domenname)
{

};

bool Type1Message_Unit::operator== (const Type1Message& t1m)
{
	const char *p = reinterpret_cast<const char *>(&t1m);
	return (
		string ("NTLMSSP") == reinterpret_cast<const char*>(t1m.protocol) &&
		1 == t1m.type &&
//		0xb203 == t1m.flags &&
		!memcmp (host.c_str(), p + t1m.host_off, t1m.host_len) &&
		!memcmp (domen.c_str(), p + t1m.dom_off, t1m.dom_len)
		);
};
	
bool Type2Message_Unit::operator== (const Type2Message& t2m)
{
	return (
		string ("NTLMSSP") == reinterpret_cast<const char*>(t2m.protocol) &&
		2 == t2m.type &&
		0x28 == t2m.msg_len
//		0x8201 == t2m.flags
		);
};

Type3Message_Unit::Type3Message_Unit (string hostname, string domenname, string username):
	host(hostname), domen(domenname), user(username)
{

};

bool Type3Message_Unit::operator== (const Type3Message& t3m)
{
	const char *p = reinterpret_cast<const char *>(&t3m);
	return (
		string ("NTLMSSP") == reinterpret_cast<const char*>(t3m.protocol) &&
		3 == t3m.type &&
		!memcmp (host.c_str(), p + t3m.host_off, t3m.host_len) &&
		!memcmp (domen.c_str(), p + t3m.dom_off, t3m.dom_len) &&
		!memcmp (user.c_str(), p + t3m.user_off, t3m.user_len)
		);
};

void BaseTest::Start ()
{
	TEST_START ts;
	ts.testName = testName;
	logger.LogTestStart (&ts);
};

void BaseTest::End ()
{
	TEST_END te;
	if (testResult) te.result = "Passed"; else te.result = "Failed";
	te.duration = ( GetTickCount() - startTick) / 1000;
	logger.LogTestEnd (&te);
};

void BaseTest::Step (string name, string reason)
{
	TEST_STEP ts;
	if (!reason.length()) ts.result = "Passed"; else ts.result = "Failed";
	ts.comment = reason;
	ts.name = name;
	logger.LogTestStep (&ts);
	testResult &= (reason == "");
};

BaseTest::BaseTest (string testname): testName (testname), logger (Logger::GetLogger()), testResult (true),
										startTick (GetTickCount()) {}


void Logger::Init (string& filename)
{
	if (!bInited)
	{
		instance_ptr = shared_ptr<Logger>(new Logger (filename));
		bInited = true;
	};
};

Logger& Logger::GetLogger()
{
	if (!bInited) Init (string(""));
	return *instance_ptr;
};

Logger::Logger (string filename): fileName (filename)
{
	if (filename == "") fileName = "td_import.txt";
	tdtool = Configuration::Init(""). GetFileTransactionConfig().tdtool;

	log = fopen (fileName.c_str(), "wb");

	DWORD size;
	char buf[128];
	size = 64;
	GetUserName (buf, &size);
	_strupr (buf);
	user = buf;

	size = 64;
	GetComputerName (buf, &size);
	_strupr (buf);
	host = buf;

	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	os = "Undefined";

	if (GetVersionEx (&osvi))
	{
		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) os = "XP";
		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0) os = "2000";
		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0) os = "NT4";
		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90) os = "ME";
		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10) os = "ME";
	};

};

Logger::~Logger ()
{
	if (log) fclose (log);
	RunImport();
};

bool Logger::RunImport ()
{
	string tool_exe = "cscript.exe";
	string tool0 = string (tdtool).append ("\\otaclient80.dll");
	string tool1 = string (tdtool).append ("\\td_import.vbs");
	string comm_line0 = string ("regsvr32").append(" ").append(tool0).append (" /s");
	string comm_line1 = string (tool_exe).append(" ").append (tool1).append(" Updater Kiryukhin 1 ").append (fileName);
	char cl[256];

	STARTUPINFO si;
    PROCESS_INFORMATION pi;
	
	//register
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );	
	
	strcpy (cl, comm_line0.c_str());
	if (!CreateProcess (NULL, cl, 
						NULL,NULL,FALSE,0,NULL,NULL,&si,&pi)) return false;

	int resWait = WaitForSingleObject (pi.hProcess, 10000);
	CloseHandle (pi.hThread);
	CloseHandle (pi.hProcess);
	
	if (WAIT_TIMEOUT == resWait) return false;
	
	//import
	ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );	
	
	strcpy (cl, comm_line1.c_str());
	if (!CreateProcess (NULL, cl, 
						NULL,NULL,FALSE,0,NULL,NULL,&si,&pi)) return false;

	resWait = WaitForSingleObject (pi.hProcess, 10000);
	CloseHandle (pi.hThread);
	CloseHandle (pi.hProcess);
	
	if (WAIT_TIMEOUT == resWait) return false;

	return true;
};

void Logger::LogTestStart (TEST_START* str)
{
	if (log)
	{
		string to_file;
		to_file.append ("TEST\t").append (user).append("\t").append(os).append("\t");
		to_file.append (UPD_VER_MAIN).append ("\t").append(host).append("\t").append ("Updater 1.2\t");
		to_file.append(str->testName).append ("\t").append (os).append ("\t");
		SYSTEMTIME st;
		char buf[10];
		GetLocalTime (&st);
		to_file.append (itoa (st.wMonth, buf, 10)).append (".");
		to_file.append (itoa (st.wDay, buf, 10)).append (".");
		to_file.append (itoa (st.wYear, buf, 10)).append ("\t");
		to_file.append (itoa (st.wHour, buf, 10)).append (":");
		to_file.append (itoa (st.wMinute, buf, 10)).append (":");
		to_file.append (itoa (st.wSecond, buf, 10)).append ("\r\n");

		fwrite (to_file.c_str(), 1, to_file.length(), log);
	};
};

void Logger::LogTestEnd (TEST_END* str)
{
	if (log)
	{
		string to_file = "END\t";
		to_file.append (str->result).append ("\t");
		char buf[16];
		itoa (str->duration, buf, 10);
		to_file.append (buf).append ("\r\n");

		fwrite (to_file.c_str(), 1, to_file.length(), log);
	};
};

void Logger::LogTestStep (TEST_STEP* str)
{
	if (log)
	{
		string to_file = "STEP\t";
		to_file.append (str->name).append ("\t");
		
		SYSTEMTIME st;
		char buf[10];
		GetLocalTime (&st);
		to_file.append (itoa (st.wMonth, buf, 10)).append (".");
		to_file.append (itoa (st.wDay, buf, 10)).append (".");
		to_file.append (itoa (st.wYear, buf, 10)).append ("\t");
		to_file.append (itoa (st.wHour, buf, 10)).append (":");
		to_file.append (itoa (st.wMinute, buf, 10)).append (":");
		to_file.append (itoa (st.wSecond, buf, 10)).append ("\t");

		to_file.append (str->result).append("\t").append(str->comment).append ("\r\n");

		fwrite (to_file.c_str(), 1, to_file.length(), log);
	};
};