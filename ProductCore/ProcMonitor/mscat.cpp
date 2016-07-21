#include <vector>
#include <list>
#include <windows.h>
#include <stdio.h>
#include <share.h>
#include "mscat.h"
#include "wthlp.h"

#define CAT_CACHE_VERSION 1
#define CAT_CACHE_MAGIC (0x001EFACA + (CAT_CACHE_VERSION << 24))

#define HASH_DWORD(pData) ((pData)[0] << 24) \
	+ ((pData)[1] << 16) \
	+ ((pData)[2] << 8)  \
	+ ((pData)[3] << 0)

using namespace std;

typedef struct tag_sha1hash
{
		byte data[20];
} tsha1hash;

typedef struct tag_HashInfo {
	union {
		byte  hash_b4[4];
		DWORD hash_dw;
		};
	DWORD offset;
} tHashInfo;

typedef vector<tsha1hash> tHashVector;
typedef vector<tHashInfo> tHashInfoVector;

class cAutoHandle {
public:
	cAutoHandle(HANDLE handle) : m_handle(handle), m_lpFileName(NULL), m_bOwn(false) {};
	cAutoHandle(
		LPCWSTR lpFileName,
		DWORD dwDesiredAccess,
		DWORD dwShareMode,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		DWORD dwCreationDisposition,
		DWORD dwFlagsAndAttributes,
		HANDLE hTemplateFile
		) : m_handle(INVALID_HANDLE_VALUE),
		m_lpFileName(lpFileName),
		m_dwDesiredAccess(dwDesiredAccess),
		m_dwShareMode(dwShareMode),
		m_lpSecurityAttributes(lpSecurityAttributes),
		m_dwCreationDisposition(dwCreationDisposition),
		m_dwFlagsAndAttributes(dwFlagsAndAttributes),
		m_hTemplateFile(hTemplateFile),
		m_bOwn(false)
	 {};

	~cAutoHandle() { close(); };
	operator HANDLE() { 
		if (m_handle == INVALID_HANDLE_VALUE && m_lpFileName != NULL)
		{
			m_handle = CreateFileW(m_lpFileName, m_dwDesiredAccess, m_dwShareMode, m_lpSecurityAttributes, m_dwCreationDisposition, m_dwFlagsAndAttributes, m_hTemplateFile);
			m_bOwn = true;
		}
		return m_handle; 
	};
	cAutoHandle& operator=(HANDLE handle) { close(); m_handle = handle; m_bOwn = false; m_lpFileName=NULL; return *this;};
	void close() { if (INVALID_HANDLE_VALUE != m_handle && m_bOwn) CloseHandle(m_handle); m_handle= INVALID_HANDLE_VALUE; m_bOwn = false; };

private:
	HANDLE m_handle;
	LPCWSTR m_lpFileName;
	DWORD m_dwDesiredAccess;
	DWORD m_dwShareMode;
	LPSECURITY_ATTRIBUTES m_lpSecurityAttributes;
	DWORD m_dwCreationDisposition;
	DWORD m_dwFlagsAndAttributes;
	HANDLE m_hTemplateFile;
	bool  m_bOwn;
};

class cCriticalSection
{
public:
	cCriticalSection() { InitializeCriticalSection(&m_cs); };
	~cCriticalSection() { DeleteCriticalSection(&m_cs); };
	operator CRITICAL_SECTION*() { return &m_cs; };
	operator CRITICAL_SECTION&() { return m_cs; };

	void Lock() { EnterCriticalSection(&m_cs); };
	void Release() { LeaveCriticalSection(&m_cs); };
private:
	CRITICAL_SECTION m_cs;
};

class cAutoNativeCS
{
public:
	cAutoNativeCS(CRITICAL_SECTION& cs)
	{
		m_cs = &cs;
		EnterCriticalSection(m_cs);
	}
	~cAutoNativeCS()
	{
		LeaveCriticalSection(m_cs);
	}
private:
	CRITICAL_SECTION* m_cs;
};

class cSigInfoImp : public cSigInfo
{
private:
	void Init()
	{
		m_hWVTStateData = INVALID_HANDLE_VALUE;
		m_bStateDataOwner = false;
		m_pSubjName = NULL;
		m_bSubjNameOwner = false;
		m_nRefCount = 1;
		memset(&m_ftCreationTime, 0, sizeof(FILETIME));
		memset(&m_ftLastWriteTime, 0, sizeof(FILETIME));
		m_bVerified = false;
		m_nWVTResult = 0;
	}
public:
 	cSigInfoImp() {
		Init();
	}

	cSigInfoImp(HANDLE hWVTStateData/*, LONG nWVTResult*/)
	{
		Init();
		m_hWVTStateData = hWVTStateData;
	};

	~cSigInfoImp()
	{
		if (m_hWVTStateData && m_hWVTStateData != INVALID_HANDLE_VALUE)
			WTHlpFreeStateData(m_hWVTStateData);
		if (m_pSubjName)
			WTHlpFreeCertName(m_pSubjName);
	}

	LONG WVTResult()
	{
		return m_nWVTResult;
	}

	bool IsSigned()
	{
		cAutoNativeCS _lock(m_cs);
		if (!m_bVerified)
		{
			WTHlpVerifyEmbeddedSignature(NULL, m_filename.c_str(), &m_hWVTStateData, &m_nWVTResult);
			m_bVerified = true;
		}
		return (0 != m_hWVTStateData && INVALID_HANDLE_VALUE != m_hWVTStateData);
	}

	LPCWSTR GetSubjectName()
	{
		cAutoNativeCS _lock(m_cs);
		if (!m_pSubjName)
		{
			if (IsSigned())
				WTHlpGetCertName(m_hWVTStateData, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0,  0, &m_pSubjName);
		}
		return m_pSubjName;
	}
	
	LPWSTR GetCertName(DWORD dwType, DWORD dwFlags, void* pvTypePara)
	{
		LPWSTR pName = NULL;
		cAutoNativeCS _lock(m_cs);
		if (IsSigned())
			WTHlpGetCertName(m_hWVTStateData, dwType, dwFlags,  pvTypePara, &pName);
		return pName;
	}

	void FreeCertName(LPWSTR wcsCertName)
	{
		WTHlpFreeCertName(wcsCertName);
	}

	cSigInfoImp* GetRef()
	{
		InterlockedIncrement(&m_nRefCount);
		return this;
	}

	void Release()
	{
		if (0 == InterlockedDecrement(&m_nRefCount))
			delete this;
	}

public:
	wstring m_filename;
	FILETIME m_ftCreationTime;
	FILETIME m_ftLastWriteTime;
	tHashInfoVector hashes;

private:
	HANDLE m_hWVTStateData;
	bool   m_bStateDataOwner;
	LPWSTR m_pSubjName;
	bool   m_bSubjNameOwner;
	long   m_nRefCount;
	cCriticalSection m_cs;
	bool   m_bVerified;
	LONG   m_nWVTResult;
};

class cCatList : public list<cSigInfoImp*> 
{
public:
	~cCatList()
	{
		clear();
	}
	iterator erase(iterator _Where)
	{
		(*_Where)->Release();
		return list::erase(_Where);
	}
	void clear()
	{
		for (iterator it=begin(); it != end(); it++)
			(*it)->Release();
		list::clear();
	}
};

class cMSCatCacheImp : public cMSCatCache
{
public:
	cMSCatCacheImp(LPCWSTR wcsCacheFile = NULL);
	~cMSCatCacheImp();
	virtual bool IsSigned(LPCWSTR wcsFileName, DWORD dwFlags, OPTIONAL HANDLE hFileHandle = INVALID_HANDLE_VALUE, OPTIONAL OUT cSigInfo** ppSigInfo = NULL, LONG* pWVTResult = NULL, tCallbackGetSHA1Norm pfCallbackGetSHA1Norm = NULL, void* pCallbackContext = NULL);

private:
	bool RefreshCatalogs(bool bForce = false);
	bool RefreshCatalog(WCHAR* sDir);
	bool RefreshCatFile(WCHAR* sFilePath, WIN32_FIND_DATAW* pfd);
	cCriticalSection m_cs;
	LPWSTR m_pCatRoot;
	HANDLE m_hChangeNotification;
	cCatList m_files;
	size_t m_nFiles;
	size_t m_nHashes;
	bool LoadCacheFile();
	bool iLoadCacheFile(FILE* in);
	bool SaveCacheFile();
	bool iSaveCacheFile(FILE* out);
	LPWSTR m_wcsCacheFileName;
};

cMSCatCache* GetMSCatCache(LPCWSTR wcsCacheFile /* = NULL */)
{
	return new cMSCatCacheImp(wcsCacheFile);
}

void ReleaseMSCatCache(cMSCatCache* pMSCatCache)
{
	delete (cMSCatCacheImp*)pMSCatCache;
}

int __cdecl compareHash(void* ctx, const void* p1, const void* p2)
{
	byte* hi1 = (byte*)p1;
	byte* hi2 = (byte*)p2;
	for (size_t i=0; i<sizeof(DWORD); i++)
	{
		byte b1 = *hi1++;
		byte b2 = *hi2++;
		if (b1 < b2)
			return -1;
		if (b1 > b2)
			return 1;
	}
	return 0;
}

cMSCatCacheImp::cMSCatCacheImp(LPCWSTR wcsCacheFile /* = NULL */) : 
	m_pCatRoot(NULL),
	m_hChangeNotification(INVALID_HANDLE_VALUE),
	m_nFiles(0),
	m_nHashes(0),
	m_wcsCacheFileName(NULL)
{
	m_pCatRoot = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, MAX_PATH*2*sizeof(WCHAR));
	if (!m_pCatRoot)
		return;
	if (0 == GetSystemDirectoryW(m_pCatRoot, MAX_PATH))
		return;
	wcscat(m_pCatRoot, L"\\CatRoot");
	m_hChangeNotification = FindFirstChangeNotificationW(m_pCatRoot, TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_FILE_NAME);
	if (wcsCacheFile)
	{
		m_wcsCacheFileName = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, (wcslen(wcsCacheFile)+4)*sizeof(WCHAR));
		if (m_wcsCacheFileName)
			wcscpy(m_wcsCacheFileName, wcsCacheFile);
	}
	RefreshCatalogs(LoadCacheFile());
}

cMSCatCacheImp::~cMSCatCacheImp()
{
	if (m_pCatRoot)
		HeapFree(GetProcessHeap(), 0, m_pCatRoot);
	if (m_wcsCacheFileName)
	{
		SaveCacheFile();
		HeapFree(GetProcessHeap(), 0, m_wcsCacheFileName);
	}
	if (INVALID_HANDLE_VALUE != m_hChangeNotification)
		FindCloseChangeNotification(m_hChangeNotification);
}

bool cMSCatCacheImp::RefreshCatFile(WCHAR* sFilePath, WIN32_FIND_DATAW* pfd)
{
	if (!pfd)
		return false;
	WCHAR* ext = wcsrchr(pfd->cFileName, '.');
	if (!ext)
		return false;
	if (0 != _wcsicmp(ext, L".cat"))
		return false;

	{
		cAutoNativeCS lock(m_cs);
		for (cCatList::iterator it=m_files.begin(); it != m_files.end(); it++)
		{
			cSigInfoImp* pSigInfo = *it;
			if (0 != wcscmp(pSigInfo->m_filename.c_str(), sFilePath))
				continue;
			if (0 == memcmp(&pSigInfo->m_ftCreationTime, &pfd->ftCreationTime, sizeof(FILETIME))
				&& 0 == memcmp(&pSigInfo->m_ftLastWriteTime, &pfd->ftLastWriteTime, sizeof(FILETIME)))
				return false; // up-to-date
			m_files.erase(it);
			break;
		}
	}


	const byte search_hash[] = { 0x0E, 0x03, 0x02, 0x1A, 0x05, 0x00, 0x04, 0x14 };
	const byte* pFileData = NULL;
	DWORD dwFileSize = 0;
	HANDLE hWVTStateData = INVALID_HANDLE_VALUE;
	LONG   nWVTResult;
	cAutoHandle hFile(sFilePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, 0);
	if (!WTHlpVerifyEmbeddedSignature(hFile, sFilePath, &hWVTStateData, &nWVTResult))
		return true;

	cSigInfoImp* pSigInfo = new cSigInfoImp(hWVTStateData);
	pSigInfo->m_filename = sFilePath;
	memcpy(&pSigInfo->m_ftCreationTime, &pfd->ftCreationTime, sizeof(FILETIME));
	memcpy(&pSigInfo->m_ftLastWriteTime, &pfd->ftLastWriteTime, sizeof(FILETIME));
	if (INVALID_HANDLE_VALUE != hFile)
	{
		dwFileSize = GetFileSize(hFile, 0);
		if (dwFileSize > sizeof(search_hash)+sizeof(tsha1hash) && dwFileSize < 32*1024*1024) // 32Mb
		{
			cAutoHandle hMapping = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, dwFileSize, 0);
			if (INVALID_HANDLE_VALUE != hMapping)
				pFileData = (const byte*)MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, dwFileSize);
		}
	}
	bool bSorted = true;
	if (pFileData)
	{
		DWORD dwLastHash = 0;
		if (pFileData[0] == 0x30)
		{
			dwFileSize -= sizeof(search_hash)+sizeof(tsha1hash);
			for (DWORD i=0; i<dwFileSize; i++)
			{
				if (pFileData[i] == search_hash[0]
					&& 0 == memcmp(pFileData+i, search_hash, sizeof(search_hash)))
				{
					i += sizeof(search_hash);
					tHashInfo hashinfo;
					memcpy(hashinfo.hash_b4, pFileData+i, _countof(hashinfo.hash_b4));
					DWORD hash1 = HASH_DWORD(pFileData+i);
					if (hash1 < dwLastHash)
						bSorted = false;
					dwLastHash = hash1;
					hashinfo.offset = i;
					pSigInfo->hashes.push_back(hashinfo);
#if 0
					printf("Hash: ");
					for (int j=0; j<20; j++)
						printf("%02X", pFileData[i+j]);
					printf("\n");
#endif
				}
			}
			
		}
		UnmapViewOfFile(pFileData);
	}
	printf("Total %d hashes [%s] in %S\n", pSigInfo->hashes.size(), bSorted ? "sorted" : "UNSORTED", pfd->cFileName);
	if (!bSorted)
		qsort_s(&pSigInfo->hashes[0], pSigInfo->hashes.size(), sizeof(pSigInfo->hashes[0]), compareHash, 0);
	
#if 0
	{
		size_t i;
		pSigInfo->hashes.clear();
		tHashInfo hi;
		hi.offset = 0;
		hi.hash1 = 1;
		for (i=0; i<100; i++)
			pSigInfo->hashes.push_back(hi);
		hi.hash1 = 2;
		pSigInfo->hashes.push_back(hi);
		hi.hash1 = 3;
		for (i=0; i<1000; i++)
			pSigInfo->hashes.push_back(hi);
	}
#endif

	cAutoNativeCS lock(m_cs);
	m_files.push_back(pSigInfo);
	return true;
}
	
bool cMSCatCacheImp::RefreshCatalog(WCHAR* sDir)
{
	WCHAR* pCatFile = sDir + wcslen(sDir);
	pCatFile[0] = '\\';
	pCatFile++;
	wcscpy(pCatFile, L"*.*");
	WIN32_FIND_DATAW fd;
	HANDLE hFind = FindFirstFileW(sDir, &fd);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	bool bRes = false;
	do {
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;
		wcscpy(pCatFile, fd.cFileName);
		bRes |= RefreshCatFile(sDir, &fd);
	} while (FindNextFileW(hFind, &fd));
	FindClose(hFind);
	return bRes;
}

bool cMSCatCacheImp::RefreshCatalogs(bool bForce /*= false*/)
{
	static DWORD dwLastRefresh = 0;
	if (!m_pCatRoot)
		return false;
	DWORD dwTickCount = GetTickCount();
	if (dwLastRefresh && !bForce)
	{
		if (dwTickCount - dwLastRefresh < 1000)
			return false;
		if (INVALID_HANDLE_VALUE != m_hChangeNotification)
		{
			DWORD dwWaitResult = WaitForSingleObject(m_hChangeNotification, 0);
			if (dwWaitResult == WAIT_TIMEOUT)
				return false;
			FindNextChangeNotification(m_hChangeNotification);
		}
		else
		{
			if (dwTickCount - dwLastRefresh < 10*60*1000) // < 10 min
				return false;
		}
	}
	dwLastRefresh = dwTickCount;
	WCHAR sDir[MAX_PATH*2];
	wcscpy(sDir, m_pCatRoot);
	WCHAR* pCatDir = sDir + wcslen(sDir);
	pCatDir[0] = '\\';
	pCatDir++;
	wcscpy(pCatDir, L"*.*");
	WIN32_FIND_DATAW fd;
	HANDLE hFind = FindFirstFileW(sDir, &fd);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	bool bRes = false;
	do {
		if (0 == (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			continue;
		if (fd.cFileName[0] != '{')
			continue;
		wcscpy(pCatDir, fd.cFileName);
		bRes |= RefreshCatalog(sDir);
	} while (FindNextFileW(hFind, &fd));
	FindClose(hFind);

	// refresh statistic
	{
		cAutoNativeCS lock(m_cs);
		m_nFiles=m_files.size();
		m_nHashes = 0;
		for (cCatList::const_iterator it=m_files.begin(); it != m_files.end(); it++)
			m_nHashes += (*it)->hashes.size();
	}
#if 1
	printf("Total %d hashes (%d bytes) in %d files\n", m_nHashes, m_nHashes*sizeof(tHashInfo), m_nFiles);
#endif
	return bRes;
}


bool cMSCatCacheImp::IsSigned(LPCWSTR wcsFileName, DWORD dwFlags, OPTIONAL HANDLE hFileHandle, OPTIONAL OUT cSigInfo** ppSigInfo, LONG* pWVTResult, tCallbackGetSHA1Norm pfCallbackGetSHA1Norm, void* pCallbackContext)
{
	if (ppSigInfo)
		*ppSigInfo = NULL;
	RefreshCatalogs();
	{
		cAutoNativeCS lock(m_cs);
		if (0 == m_files.size())
			return false;
	}
	tsha1hash hash;
	// calc hash
	BOOL bHashCalculated = FALSE;
	DWORD dwHashSize = sizeof(hash.data); // SHA1 is always 20 bytes
	{
		cAutoHandle hFile(wcsFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, 0);
		if (hFileHandle != INVALID_HANDLE_VALUE)
			hFile = hFileHandle;
		if (0 == (dwFlags & MSC_DONT_LOOKUP_EMBEDDED_SIG))
		{
			HANDLE hWVTStateData = INVALID_HANDLE_VALUE;
			if (WTHlpVerifyEmbeddedSignature(hFile, wcsFileName, ppSigInfo ? &hWVTStateData : NULL, pWVTResult))
			{
				if (ppSigInfo)
					*ppSigInfo = new cSigInfoImp(hWVTStateData);
				return true;
			}
		}
		if (0 == (dwFlags & MSC_DONT_LOOKUP_CATALOGS))
		{
			if (pfCallbackGetSHA1Norm)
				bHashCalculated = pfCallbackGetSHA1Norm(pCallbackContext, hash.data);
			if (!bHashCalculated)
				bHashCalculated = WTHlpCalcHashFromFileHandle(hFile, &dwHashSize, hash.data);
		}
	}	
	cSigInfoImp* pResultSigInfo = NULL;
	if (bHashCalculated)
	{
		cAutoNativeCS lock(m_cs);
		for (cCatList::iterator it=m_files.begin(); it != m_files.end() && !pResultSigInfo; it++)
		{
			tHashInfo* phi = NULL;
			cSigInfoImp* pSigInfo = *it;
			DWORD hash_dw = *(DWORD*)hash.data;
			if (pSigInfo->hashes.size())
				phi = (tHashInfo*)bsearch_s(hash.data, &pSigInfo->hashes[0], pSigInfo->hashes.size(), sizeof(pSigInfo->hashes[0]), compareHash, 0);
			if (!phi)
				continue;

			const tHashInfo* pbegin = &pSigInfo->hashes[0];
			const tHashInfo* pend = pbegin + pSigInfo->hashes.size();

			// scroll up
			while (phi != pbegin && phi[-1].hash_dw == hash_dw)
				phi--;

			cAutoHandle hFile(pSigInfo->m_filename.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, 0);
			if (INVALID_HANDLE_VALUE == hFile) // hmm... file is locked or deleted - cannot verify
				continue;

			for (;phi<pend && phi->hash_dw == hash_dw; phi++)
			{
				tsha1hash cathash;
				DWORD dwBytesRead;
				SetFilePointer(hFile, phi->offset, NULL, SEEK_SET);
				if (!ReadFile(hFile, &cathash.data, sizeof(cathash.data), &dwBytesRead, 0) || dwBytesRead != sizeof(cathash.data))
					continue;
				if (0 != memcmp(hash.data, &cathash.data, dwHashSize))
					continue;
				// match found
				pResultSigInfo = pSigInfo->GetRef();
				break;
			}
		}
	}
	if (!pResultSigInfo)
		return false;
	// verify cat-file
	if (!pResultSigInfo->IsSigned())
	{
		cAutoNativeCS lock(m_cs);
		for (cCatList::iterator it=m_files.begin(); it != m_files.end(); it++)
		{
			cSigInfoImp* pSigInfo = *it;
			if (pSigInfo == pResultSigInfo)
			{
				m_files.erase(it);
				break;
			}
		}
		return false;
	}
	if (pWVTResult)
		*pWVTResult = pResultSigInfo->WVTResult();
	if (ppSigInfo)
		*ppSigInfo = pResultSigInfo;
	return true;
}


// always called under lock
bool cMSCatCacheImp::LoadCacheFile()
{
	FILE* in = NULL;
	cAutoNativeCS _lock(m_cs);
	if (!m_wcsCacheFileName)
		return false;
	_wfopen_s(&in, m_wcsCacheFileName, L"rb");
	if (!in)
		return false;
	bool res = iLoadCacheFile(in);
	if (!res)
		m_files.clear();
	fclose(in);
	return res;
}

// always called under lock
bool cMSCatCacheImp::iLoadCacheFile(FILE* in)
{
	if (!m_wcsCacheFileName)
		return false;
	if (!in)
		return false;
	size_t nMagic;
	if (!fread(&nMagic, sizeof(nMagic), 1, in))
		return false;
	if (nMagic != CAT_CACHE_MAGIC)
		return false;
	size_t nFiles;
	if (!fread(&nFiles, sizeof(nFiles), 1, in))
		return false;
	if (nFiles > 10000)
		return false;
	m_files.clear();
	for (size_t i=0; i<nFiles; i++)
	{
		cSigInfoImp* pSigInfo = new cSigInfoImp();
		m_files.push_back(pSigInfo);
		size_t nLen;
		if (!fread(&nLen, sizeof(nLen), 1, in))
			return false;
		if (nLen > 0xFFFF)
			return false;
		wchar_t* pname = new wchar_t[nLen+1];
		if (!pname)
			return false;
		if (!fread(pname, sizeof(wchar_t), nLen, in))
			return false;
		pname[nLen] = 0;
		pSigInfo->m_filename = pname;
		delete[] pname;
		if (!fread(&pSigInfo->m_ftCreationTime, sizeof(FILETIME), 1, in))
			return false;
		if (!fread(&pSigInfo->m_ftLastWriteTime, sizeof(FILETIME), 1, in))
			return false;
		size_t nHashes;
		if (!fread(&nHashes, sizeof(nHashes), 1, in))
			return false;
		if (nHashes > 100000)
			return false;
		if (nHashes)
		{
			pSigInfo->hashes.resize(nHashes);
			if (!fread(&pSigInfo->hashes[0], sizeof(pSigInfo->hashes[i]), nHashes, in))
				return false;
		}
	}
	return true;
}

bool cMSCatCacheImp::SaveCacheFile()
{
	FILE* out = NULL;
	if (!m_wcsCacheFileName)
		return false;
	cAutoNativeCS lock(m_cs);
	out = _wfsopen(m_wcsCacheFileName, L"w+b", _SH_DENYNO);
	setvbuf(out, 0, _IONBF, 0);
	if (!out)
		return false;
	bool res = iSaveCacheFile(out);
	fclose(out);
	return res;
}

bool cMSCatCacheImp::iSaveCacheFile(FILE* out)
{
	size_t magic = CAT_CACHE_MAGIC;
	if (!fwrite(&magic, sizeof(magic), 1, out))
		return false;
	size_t filecount = m_files.size();
	if (!fwrite(&filecount, sizeof(filecount), 1, out))
		return false;
	
	for (cCatList::const_iterator it = m_files.begin(); it != m_files.end(); it++)
	{
		cSigInfoImp* pSigInfo = *it;
		size_t namelen = pSigInfo->m_filename.length();
		if (!fwrite(&namelen, sizeof(namelen), 1, out))
			return false;
		if (!fwrite(pSigInfo->m_filename.c_str(), sizeof(wchar_t), namelen, out))
			return false;

		if (!fwrite(&pSigInfo->m_ftCreationTime, sizeof(FILETIME), 1, out))
			return false;
		if (!fwrite(&pSigInfo->m_ftLastWriteTime, sizeof(FILETIME), 1, out))
			return false;

		size_t hashescount = pSigInfo->hashes.size();
		if (!fwrite(&hashescount, sizeof(hashescount), 1, out))
			return false;
		
		if (hashescount)
		{
			if (!fwrite(&pSigInfo->hashes[0], sizeof(pSigInfo->hashes[0]), hashescount, out))
				return false;
		}
	}
	return true;
}
