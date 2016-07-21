#ifndef UTIL_H
#define UTIL_H

#include "stdafx.h"

#include <boost/shared_ptr.hpp>

#include "core/updater.h"
#include "core/HttpProtocolImplementation.h"
#include <windows.h>
#include <process.h>

#include "helper/fileinfo.h"
using namespace KLUPD;

#include <string>
#include <vector>
using namespace std;


class doUpdater
{
	public:

		doUpdater (KLUPD::Updater* upd, HANDLE& hEv) : error (-1),
		  hThread ( (HANDLE)_beginthreadex (NULL, 0, doUpdate, upd, 0, NULL) )
		{
			hEv = hThread;
		};

		~doUpdater()
		{
			CloseHandle (hThread);
		};

		static unsigned __stdcall doUpdate (void* param)
		{
			KLUPD::Updater* upd = reinterpret_cast<KLUPD::Updater*>(param);
			
			return upd->doUpdate();
		};

		int GetError()
		{
			if (error == -1)
			{
				if (! GetExitCodeThread (hThread, reinterpret_cast<DWORD*>(&error) ) ) error = -1;
			}
			
			return error;
		};

	private:

		HANDLE hThread;
		int error;

};

class doSelfNetDownloader
{
	public:

		struct doSelfNetDownloader_par
		{
			FileInfo fi;
			Path relativeURLPath;
			Path pathToDownloadFiles;
			NoCaseString url;

			KLUPD::Updater* upd;
		};

		doSelfNetDownloader (KLUPD::Updater* upd, HANDLE& hEv, const FileInfo& fi, const Path& p1, const NoCaseString& p2, const NoCaseString& p3) : error (-1)
		{
			pSet = boost::shared_ptr<doSelfNetDownloader_par> (new doSelfNetDownloader_par());
			pSet->fi = fi;
			pSet->upd = upd;
			pSet->relativeURLPath = p1;
			pSet->pathToDownloadFiles = p2;
			pSet->url = p3;
			hThread = (HANDLE)_beginthreadex (NULL, 0, doDownload, pSet.get(), CREATE_SUSPENDED, NULL);
			hEv = hThread;

			upd->reportSourceSelected ();
			ResumeThread (hThread);
		};

		~doSelfNetDownloader()
		{
			CloseHandle (hThread);
		};

		static unsigned __stdcall doDownload (void* param)
		{
			doSelfNetDownloader_par* set = reinterpret_cast<doSelfNetDownloader_par*>(param);
			
			//return set->upd->DoGetRemoteFile_SelfNet(set->fi, set->relativeURLPath, set->pathToDownloadFiles, set->url);
			return set->upd->DoGetRemoteFile_SelfNet(set->fi.m_filename, set->pathToDownloadFiles, set->relativeURLPath, set->url);

		};

		int GetError()
		{
			if (error == -1)
			{
				if (! GetExitCodeThread (hThread, reinterpret_cast<DWORD*>(&error) ) ) error = -1;
			}
			
			return error;
		};

	private:

		HANDLE hThread;
		int error;
		boost::shared_ptr<doSelfNetDownloader_par> pSet;

};

class doNewNetDownloader
{
	public:

		struct doNewNetDownloader_par
		{
			FileInfo fi;
			NoCaseString relativeURLPath;
			NoCaseString localFolder;
			Source sourceIn;
			Signature6Checker* p_s6c;

			KLUPD::Updater* upd;
		};

		doNewNetDownloader (KLUPD::Updater* upd, HANDLE& hEv, const FileInfo& fi, const NoCaseString& p1, const NoCaseString& p2, const Source& p3, Signature6Checker* p_s6c) : error (-1)
		{
			pSet = boost::shared_ptr<doNewNetDownloader_par> (new doNewNetDownloader_par());
			pSet->fi = fi;
			pSet->upd = upd;
			pSet->relativeURLPath = p1;
			pSet->localFolder = p2;
			pSet->sourceIn = p3;
			pSet->p_s6c = p_s6c;
			hThread = (HANDLE)_beginthreadex (NULL, 0, doDownload, pSet.get(), CREATE_SUSPENDED, NULL);
			hEv = hThread;

			upd->reportSourceSelected ();
			ResumeThread (hThread);
		};

		~doNewNetDownloader()
		{
			CloseHandle (hThread);
		};

		static unsigned __stdcall doDownload (void* param)
		{
			doNewNetDownloader_par* set = reinterpret_cast<doNewNetDownloader_par*>(param);
			
			//return set->upd->DoGetRemoteFile_NewNet(set->fi, set->relativeURLPath, set->localFolder, set->sourceIn);
			//return set->upd->m_httpProtocol.getFile(set->fi.m_filename, set->fi.m_localPath, set->relativeURLPath,
			//		set->sourceIn, )
			return CORE_NO_ERROR;
		};

		int GetError()
		{
			if (error == -1)
			{
				if (! GetExitCodeThread (hThread, reinterpret_cast<DWORD*>(&error) ) ) error = -1;
			}
			
			return error;
		};

	private:

		HANDLE hThread;
		int error;
		boost::shared_ptr<doNewNetDownloader_par> pSet;

};

class doEntryDownloader
{
	public:

		struct doEntryDownloader_par
		{
			FileInfo fi;
			Path temporaryFolder;
			Signature6Checker* p_s6c;

			KLUPD::Updater* upd;
		};

		doEntryDownloader (KLUPD::Updater* upd, HANDLE& hEv, const FileInfo& fi, const Path& p1, Signature6Checker* p_s6c) : error (-1)
		{
			pSet = boost::shared_ptr<doEntryDownloader_par> (new doEntryDownloader_par());
			pSet->fi = fi;
			pSet->upd = upd;
			pSet->temporaryFolder = p1;
			pSet->p_s6c = p_s6c;
			hThread = (HANDLE)_beginthreadex (NULL, 0, doDownload, pSet.get(), CREATE_SUSPENDED , NULL);
			hEv = hThread;

			upd->reportSourceSelected ();
			ResumeThread (hThread);
		};

		~doEntryDownloader()
		{
			CloseHandle (hThread);
		};

		static unsigned __stdcall doDownload (void* param)
		{
			doEntryDownloader_par* set = reinterpret_cast<doEntryDownloader_par*>(param);
			
			return set->upd->downloadEntry(set->fi, set->temporaryFolder);
		};

		int GetError()
		{
			if (error == -1)
			{
				if (! GetExitCodeThread (hThread, reinterpret_cast<DWORD*>(&error) ) ) error = -1;
			}
			
			return error;
		};

	private:

		HANDLE hThread;
		int error;
		boost::shared_ptr<doEntryDownloader_par> pSet;

};

class doRollbacker
{
	public:

		doRollbacker (KLUPD::Updater* upd, HANDLE& hEv) : error (-1),
		  hThread ( (HANDLE)_beginthreadex (NULL, 0, doRollback, upd, 0, NULL) )
		{
			hEv = hThread;
		};

		~doRollbacker()
		{
			CloseHandle (hThread);
		};

		static unsigned __stdcall doRollback (void* param)
		{
			KLUPD::Updater* upd = reinterpret_cast<KLUPD::Updater*>(param);
			//true для отката ретрансляции
			return upd->doRollback(upd->m_callback.updaterConfiguration().retranslationRequired());
		};

		int GetError()
		{
			if (error == -1)
			{
				if (! GetExitCodeThread (hThread, reinterpret_cast<DWORD*>(&error) ) ) error = -1;
			}
			
			return error;
		};

	private:

		HANDLE hThread;
		int error;

};

class dataPoster
{
public:

	struct dataPoster_par
	{
		dataPoster_par	  (KLUPD::HttpProtocolMemoryImplementation* _p_httpProtocol, const vector<unsigned char>& _data,
							const Path& _filename, const Path& _path, const string& _userAgent,
							const Address& _serverAddress, bool _useProxy = false,
							const Address& _proxyAddress = Address(), 
							const AuthorizationTypeList& _authList = AuthorizationTypeList()
						   ):
						p_httpProtocol (_p_httpProtocol), userAgent(_userAgent),
						data(_data), filename(_filename), path(_path), serverAddress(_serverAddress),
						useProxy(_useProxy), proxyAddress(_proxyAddress), authList(_authList)
						{
							full_filename = "";
							if (_path.toAscii().c_str()[0] != '/') full_filename = "/";
							full_filename.append (_path.toAscii());
							if (_path.toAscii().c_str()[_path.toAscii().size()-1] != '/')
								full_filename.append ("/");
							full_filename.append (_filename.toAscii());

							//временно?
							//if (full_filename[0]=='/') full_filename = full_filename.substr (1);
						};

		vector<unsigned char> data;
		Path filename;
		Path path;
		string full_filename;
		string userAgent;
		Address serverAddress;
		Address proxyAddress;
		AuthorizationTypeList authList;
		bool useProxy;
		HttpProtocolMemoryImplementation* p_httpProtocol;
	};

	dataPoster (HANDLE& hEv, dataPoster_par* p_param) : error (-1)
	{
		hThread = (HANDLE)_beginthreadex (NULL, 0, doPost, p_param, CREATE_SUSPENDED , NULL);
		hEv = hThread;

		ResumeThread (hThread);
	};

	~dataPoster()
	{
		CloseHandle (hThread);
	};

	static unsigned __stdcall doPost (void* param)
	{
		dataPoster_par* set = reinterpret_cast<dataPoster_par*>(param);

		return set->p_httpProtocol->postFile
				(set->data, set->filename, set->path, set->serverAddress, set->userAgent,
				 set->useProxy, set->proxyAddress, set->authList);
	};

	int GetError()
	{
		if (error == -1)
		{
			if (! GetExitCodeThread (hThread, reinterpret_cast<DWORD*>(&error) ) ) error = -1;
		}

		return error;
	};

private:

	HANDLE hThread;
	int error;
	boost::shared_ptr<dataPoster_par> pSet;

};

namespace _helper
{
	using namespace std;
	
	bool RecreateDirectories (vector<wstring>& dirs);
	bool DeleteDirectory (const wchar_t* dir);
	bool MakeDirectory (const wchar_t* dir);
	bool CopyFolder (const wchar_t* source, const wchar_t* destination);
	bool IsFoldersEqual (const wstring& tool, const wstring& dir1, const wstring& dir2);
	int CloneFile (const wstring& lpExistingFileName, const wstring& lpNewFileName, BOOL bFailIfExists);
	bool DeleteFile (const wstring& fname);
	bool AreFilesEqual (const wstring& file1, const wstring& file2);
	bool checkFileContent (const wstring& filename, const string& data);
	string GetSelfIp ();
	string GetSelfIp_IpV6 ();
	string GetKasperskyRuIp ();
	wstring GetSelfIpW ();
	wstring GetSelfIpW_IpV6 ();
	wstring GetKasperskyRuIpW ();

	class AutoFile
	{
	public:
		AutoFile (const wstring& filename, DWORD desireAccess = GENERIC_READ,
					DWORD dwShareMode = FILE_SHARE_READ, DWORD dwCreationDisposition = OPEN_EXISTING);
		~AutoFile ();

		int GetFileSize ();
		HANDLE GetHandle();
		bool Exist ();
		void LockFile ();
		int GetContent (char* buf, int length);

	private:
		HANDLE hFile;
		int bLocked;
	};
	
}

class FileInfo_Unit: public FileInfo
{
public:
	FileInfo_Unit (	const NoCaseString &filename  = NoCaseString(),
					const NoCaseString &relativeURLPath = NoCaseString(L"/"),
					const Type & updTyp= FileInfo::base,
					const bool primaryOrSubstitutedIndex = false):
		FileInfo (filename, relativeURLPath, updTyp, primaryOrSubstitutedIndex)  {};

	bool operator== (const FileInfo& right);

};

bool operator== (std::vector<FileInfo_Unit, std::allocator<FileInfo_Unit> >& fiu,
				 const std::vector<FileInfo, std::allocator<FileInfo> >& fi);


#endif