#include "util.h"
#include <crtdbg.h>

#define SELF_IP "172.16.1.185"
#define KASPERSKY_RU_IP "81.176.69.105"

#define SELF_IP_W L"172.16.1.185"
#define KASPERSKY_RU_IP_W L"81.176.69.105"

#define SELF_IP_IPV6 "dead::c596"
#define SELF_IP_IPV6_W L"[dead::c596]"

namespace _helper
{

using namespace std;

bool checkFileContent (const wstring& filename, const string& data)
{
	try
	{
		FILE* file = _wfopen (filename.c_str(), L"r");
		char buf[256];
		string from_file;
		int count = 1;
		while (count)
		{
			count = fread (buf, 1, 256, file);
			if (count != 0) from_file.append (buf, buf + count);
		}
		fclose (file);
		return data == from_file;
	}
	catch (...)
	{
		return false;
	}

}

bool DeleteDirectory (const wchar_t* dir)
{
	_ASSERTE (wcslen(dir) != 0);
	int ret=0;
	wchar_t name1[256];
	WIN32_FIND_DATA info;
    HANDLE hp;
    wchar_t *cp;
	
	swprintf(name1, L"%s\\*.*",dir);
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
        swprintf(name1,L"%s\\%s",dir,info.cFileName);
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

bool MakeDirectory (const wchar_t* dir)
{
	return (0 != CreateDirectory (dir, NULL) || GetLastError() == ERROR_ALREADY_EXISTS);
};

bool CopyFolder (const wchar_t* source, const wchar_t* destination)
{
	wchar_t buffer[256];
	swprintf (buffer, L"xcopy %s %s /E /Y /R", source, destination);

	STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );	
	
	if (!CreateProcess (NULL, buffer, 
						NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&si,&pi)) return false;

	int resWait = WaitForSingleObject (pi.hProcess, 10000);
	CloseHandle (pi.hThread);
	CloseHandle (pi.hProcess);
	
	return (resWait == WAIT_OBJECT_0) ? true : false;
}

bool IsFoldersEqual (const wstring& tool, const wstring& dir1, const wstring& dir2)
{
	wstring tool_exe = wstring (tool).append (L"\\ExamDiff.exe");
	wstring tool_rep = wstring (tool).append (L"\\report.txt");
	wstring tool_rep_old = wstring (tool).append (L"\\report_old.txt");
	wchar_t comm_line[512];

	wstring _dir1;
	wstring _dir2;
	if (* (dir1.end()-1) == '\\') _dir1 = dir1.substr (0, dir1.size()-1); else _dir1 = dir1;
	if (* (dir2.end()-1) == '\\') _dir2 = dir2.substr (0, dir2.size()-1); else _dir2 = dir2;

	wcscpy (comm_line, wstring (tool_exe).append(L" \"").append (_dir1).append(L"\" \"").append(_dir2).append(L"\" /r2 /!z /o:").append(tool_rep).c_str());
	
	CopyFile (tool_rep.c_str(), tool_rep_old.c_str(), false);
	DeleteFile (tool_rep.c_str());

	STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );	
	
	//wcout<<comm_line;
	if (!CreateProcess (NULL, comm_line, 
						NULL,NULL,FALSE,0,NULL,NULL,&si,&pi)) return false;

	int resWait = WaitForSingleObject (pi.hProcess, 10000);
	CloseHandle (pi.hThread);
	CloseHandle (pi.hProcess);
	
	if (WAIT_TIMEOUT != resWait)
	{
		AutoFile hFile (tool_rep.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
		char buf[22];
		DWORD dwRead;
		if (ReadFile (hFile.GetHandle(), buf, 21, &dwRead, NULL) && !(buf[21] = 0) && !strcmp (buf, "IDENTICAL DIRECTORIES"))
			return true;
	};
	return false;
};

int CloneFile (const wstring& lpExistingFileName, const wstring& lpNewFileName, BOOL bFailIfExists)
{
	SetFileAttributes (lpNewFileName.c_str(), FILE_ATTRIBUTE_NORMAL);
	return CopyFile (lpExistingFileName.c_str(), lpNewFileName.c_str(), bFailIfExists);
};

bool DeleteFile (const wstring& fname)
{
	SetFileAttributes (fname.c_str(), FILE_ATTRIBUTE_NORMAL);
	return (bool)( ::DeleteFile (fname.c_str()) );
}

bool AreFilesEqual (const wstring& file1, const wstring& file2)
{
	DWORD par1, par2, par1h, par2h;

	AutoFile hFile1 (file1, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
	AutoFile hFile2 (file2, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
	par1 = GetFileSize (hFile1.GetHandle(), &par1h);
	par2 = GetFileSize (hFile2.GetHandle(), &par2h);
	if (par1 != par2) return false;
	HANDLE hMap1 = CreateFileMapping (hFile1.GetHandle(), NULL, PAGE_READONLY, 0, 0, NULL);
	HANDLE hMap2 = CreateFileMapping (hFile2.GetHandle(), NULL, PAGE_READONLY, 0, 0, NULL);
	LPVOID hView1 = MapViewOfFile (hMap1, FILE_MAP_READ, 0, 0, 0);
	LPVOID hView2 = MapViewOfFile (hMap2, FILE_MAP_READ, 0, 0, 0);
	int i;
	for (i = 0; i < par1; ++i)
	{
		if (*(reinterpret_cast<char*>(hView1)+i) != *(reinterpret_cast<char*>(hView2)+i)) break;
	}
	UnmapViewOfFile (hView1);
	UnmapViewOfFile (hView2);
	CloseHandle (hMap1);
	CloseHandle (hMap2);

	return (i == par1);
}

bool RecreateDirectories (vector<wstring>& dirs)
{
	for (vector<wstring>::iterator it = dirs.begin(); it != dirs.end(); ++it)
	{
		//BOOST_MESSAGE (L"Deleting destination folder");
		//BOOST_WARN_MESSAGE (helper::DeleteDirectory (config_ft.destination.c_str()), config_ft.destination.c_str());
		//BOOST_MESSAGE (L"Creating destination folder");
		//BOOST_REQUIRE_MESSAGE (helper::MakeDirectory (config_ft.destination.c_str()), config_ft.destination.c_str());
		if (!it->c_str()) continue;
		DeleteDirectory (it->c_str());
		_ASSERTE ( MakeDirectory (it->c_str()) );
	};
	return true;
};

string GetSelfIp ()
{
	//get ip using winapi
	return SELF_IP;
}

string GetSelfIp_IpV6 ()
{
	//get ip using winapi
	return SELF_IP_IPV6;
}

string GetKasperskyRuIp ()
{
	//get ip using winapi
	return KASPERSKY_RU_IP;
}

wstring GetSelfIpW ()
{
	//get ip using winapi
	return SELF_IP_W;
}

wstring GetSelfIpW_IpV6 ()
{
	//get ip using winapi
	return SELF_IP_IPV6_W;
}

wstring GetKasperskyRuIpW ()
{
	//get ip using winapi
	return KASPERSKY_RU_IP_W;
}

AutoFile::AutoFile (const wstring& filename, DWORD desireAccess, DWORD dwShareMode, DWORD dwCreationDisposition)
: bLocked (false)
{
	SetFileAttributes (filename.c_str(), FILE_ATTRIBUTE_NORMAL);
	hFile = CreateFile (filename.c_str(), desireAccess, dwShareMode, NULL, dwCreationDisposition, NULL, NULL);
	//if (hFile == INVALID_HANDLE_VALUE) throw -1;
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

bool AutoFile::Exist ()
{
	return hFile != INVALID_HANDLE_VALUE;
};

int AutoFile::GetContent (char* buf, int length)
{
	DWORD dwBytesRead;
	ReadFile (hFile, buf, length, &dwBytesRead, NULL);
	return dwBytesRead;
}


}


bool FileInfo_Unit::operator== (const FileInfo& right)
{
	bool bRes;
	bRes = (m_comment == right.m_comment &&
			m_dateFromIndex == right.m_dateFromIndex &&
			m_filename == right.m_filename &&
			//m_signature == right.m_signature &&
			m_fromAppVersion == right.m_fromAppVersion &&
			//m_fromCompVersion == right.m_fromCompVersion &&
			m_localPath == right.m_localPath &&
			//m_primaryOrSubstitutedIndex == right.m_primaryOrSubstitutedIndex &&
			//m_rollbackChange == right.m_rollbackChange &&
			m_originalLocalPath == right.m_originalLocalPath &&
			m_relativeURLPath == right.m_relativeURLPath &&
			m_size == right.m_size &&
			m_toAppVersion == right.m_toAppVersion &&
			//m_toCompVersion == right.m_toCompVersion &&
			m_componentIdSet == right.m_componentIdSet &&
			m_obligatoryEntry == right.m_obligatoryEntry &&
			m_type == right.m_type &&
			m_language == right.m_language &&
			m_OSes.size() == right.m_OSes.size());
	if (bRes)
	{
		for (int i = 0; i< m_OSes.size(); ++i)
		{
			if ((m_OSes[i].m_name != right.m_OSes[i].m_name) || (m_OSes[i].m_version != right.m_OSes[i].m_version))
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

