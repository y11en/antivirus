/*!
* (C) 2002 Kaspersky Lab 
* 
* \file	SharedTable.cpp
* \author	Pavel Mezhuev
* \date	09.12.2002 17:34:41
* \brief	Интерфейс распределенной таблицы в памяти.
* 
*/

//************************************************************************

#include "SharedTable.h"
#include <stdio.h>
#include <version/ver_product.h>

struct tTABLE_DATA
{
	long			m_ViewCount;
	long			m_Count;
	unsigned char	m_Records[1];
};

struct tRECORD_DATA
{
	hRecord 		m_Record;
	long			m_UsageCount;
	long			m_CloseState;
	long			m_ProccessId;
	unsigned char	m_Data[1];
};

#define TABLE_DATA()		((tTABLE_DATA*)m_pTable)
#define TABLE_SIZE(count)	(sizeof(tTABLE_DATA) + count * m_RecordSize - 1)
#define RECORD_DATA(record)	((tRECORD_DATA*)(TABLE_DATA()->m_Records + record * m_RecordSize))
#define RECORD_NEXT(data)	data = (tRECORD_DATA*)((char*)data + m_RecordSize)

#define MAX_TABLE_COUNT		1000
#define CHUNCK_COUNT		10

extern long g_UniqueId;

#if defined (_WIN32)
#define LOCK() 	  WaitForSingleObject(m_hSyncObj, INFINITE);
#define UNLOCK()  ReleaseMutex(m_hSyncObj);
#elif defined (__unix__)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#include <sys/mman.h>

#include <string>

#if !defined(__FreeBSD__) && !defined(__OpenBSD__)
static struct flock _flw = { F_WRLCK, SEEK_SET, 0, 0, 0 };
//static struct flock _flr = { F_RDLCK, SEEK_SET, 0, 0, 0 };
static struct flock _flu = { F_UNLCK, SEEK_SET, 0, 0, 0 };
#else
static struct flock _flw = { 0, 0, 0, F_WRLCK, SEEK_SET };
//static struct flock _flr = { 0, 0, 0, F_RDLCK, SEEK_SET };
static struct flock _flu = { 0, 0, 0, F_UNLCK, SEEK_SET };
#endif

#define LOCK()  { \
	/*fprintf ( stderr, "lock %d %s\n", m_hFile, __FUNCTION__ );*/ \
	fcntl( m_hFile, F_SETLKW, &_flw ); pthread_mutex_lock (&m_hSyncObj); \
}
#define UNLOCK() { \
	/*fprintf ( stderr, "unlock %d %s\n", m_hFile, __FUNCTION__ );*/ \
	pthread_mutex_unlock (&m_hSyncObj); fcntl( m_hFile, F_SETLKW, &_flu ); \
}
#endif

//************************************************************************

#if defined (_WIN32)

long CSharedTable::m_nTablesCount = 0;
void* CSharedTable::m_pFileOpenServer = NULL;


#include <ShlObj.h>

BOOL IsGlobalNameSpace()
{
	DWORD dwVersion = GetVersion();
	if( dwVersion & 0x80000000 )
		return false;
	
	if( (DWORD)(LOBYTE(LOWORD(dwVersion))) >= 5 )
		return true;
	
	HKEY hKey = NULL;
    LONG res = RegOpenKeyA(HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Control\\ProductOptions", &hKey);
	
    CHAR  strSuite[0x1000];
	DWORD size = sizeof(strSuite), type = 0;
	
    if( res == ERROR_SUCCESS )
		res = RegQueryValueExA( hKey, "ProductSuite", NULL, &type, (PBYTE)strSuite, &size);
	
	if( type != REG_MULTI_SZ )
		res = E_FAIL;
	
	bool bIsSuite = false;
	for(CHAR* p = strSuite; res == ERROR_SUCCESS && *p; p += strlen(p)+1)
		if( lstrcmpA(p, "Terminal Server") == 0 )
		{
			bIsSuite = true;
			break;
		}
		
		if( hKey )
			RegCloseKey(hKey);
		
		return bIsSuite;
}

HANDLE CreateGlobalEvent(DWORD pid)
{
	HSA sa = SA_Create(SA_AUTHORITY_EVERYONE, SA_ACCESS_MASK_ALL);

	char event_name[100]="";
	sprintf(event_name,  "%sPREvent%d#%x", IsGlobalNameSpace() ? "Global\\" : "", pid, g_UniqueId);

	HANDLE evt = CreateEvent(SA_Get(sa), TRUE, FALSE, event_name);
	if( sa )
		SA_Destroy(sa);
	return evt;
}

static bool GetSharedFileName(const char* name, unsigned int uniqueId, char* pszResult)
{
	char szPath[MAX_PATH * 2];

	HKEY hKey = NULL;
#ifndef VER_PRODUCT_REGISTRY_PATH
	#define VER_PRODUCT_REGISTRY_PATH			VER_COMPANY_REGISTRY_ROOT "\\" VER_PRODUCT_REGISTRY_ROOT
#endif
	LONG res = RegOpenKeyA(HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\environment", &hKey);

	if( res == ERROR_SUCCESS )
	{
		DWORD size = sizeof(szPath), type = 0;
		res = RegQueryValueExA( hKey, "DataRoot", NULL, &type, (PBYTE)szPath, &size);

		if( type != REG_SZ )
			res = E_FAIL;

		if( res == ERROR_SUCCESS )
		{
			if( char *pLastSlash = strrchr(szPath, '\\') )
				*pLastSlash = 0;
		}

		if( hKey )
			RegCloseKey(hKey);
	}

	if( res != ERROR_SUCCESS )
	{
		HMODULE hShFolder = LoadLibraryA("shfolder.dll");
		if (!hShFolder)
			return false;
		typedef HRESULT (WINAPI *tSHGetFolderPath)(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPSTR pszPath);
		tSHGetFolderPath fnSHGetFolderPath = (tSHGetFolderPath)GetProcAddress(hShFolder, "SHGetFolderPathA");
		bool bHasAppData = fnSHGetFolderPath
			&& SUCCEEDED(fnSHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, szPath));
		FreeLibrary(hShFolder);
		if (!bHasAppData)
			return false;
		strcat(szPath, "\\Kaspersky Lab");
	}

	wsprintf(pszResult,  "%s\\~%s#%x.dat", szPath, name, uniqueId);

	return true;
}


namespace OpenFileService
{
	static const char* sPipeName = "\\\\.\\pipe\\sa_hlp_srv";
	struct CSOSrvRequest
	{
		unsigned long pid;
		unsigned long uniqueId;
		char name[128];
	};

	class COverlapped: public OVERLAPPED
	{
	public:
		COverlapped()
		{
			memset((OVERLAPPED*) this, 0, sizeof(OVERLAPPED));
			hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		}
		~COverlapped()
		{
			CloseHandle(hEvent);
			hEvent = NULL;
		}
		void PrepareOverlapped()
		{
			Internal = 0;
			Offset = 0;
			OffsetHigh = 0;
			Pointer = 0;
		}
	};

	class CServer
	{
		HSA m_pSA;
		HANDLE m_evStop;
		HANDLE m_hThreads[2];
	public:
		CServer()
		{
			m_pSA = SA_Create(SA_AUTHORITY_EVERYONE, SA_ACCESS_MASK_ALL);
			m_evStop = CreateEvent(NULL, TRUE, FALSE, NULL);
			memset(m_hThreads, 0, sizeof(m_hThreads));
		}
		~CServer()
		{
			Stop();
			CloseHandle(m_evStop);
			SA_Destroy(m_pSA);
		}
		void Start()
		{
			if (m_hThreads[0])
				return;
			DWORD id;
			// BUG 17897: Стартуем несколько потоков на случай нескольких клиентов
			// SharedTable создаётся 2 штуки одна за другой, один сервер может не
			// успеть принять второй запрос
			for (int i = 0; i < sizeof(m_hThreads)/sizeof(m_hThreads[0]); ++i)
				m_hThreads[i] = CreateThread(NULL, 0, ServerThread, this, 0, &id);
		}
		void Stop()
		{
			if (!m_hThreads[0])
				return;
			SetEvent(m_evStop);
			WaitForMultipleObjects(sizeof(m_hThreads)/sizeof(m_hThreads[0]),
				m_hThreads, TRUE, INFINITE);
			ResetEvent(m_evStop);
			memset(m_hThreads, 0, sizeof(m_hThreads));
		}
	private:
		static HANDLE GetFile(CSOSrvRequest& request)
		{
			char l_obj_name[MAX_PATH * 2];
			request.name[127] = 0; // <- overrun protection
			if (!GetSharedFileName(request.name, g_UniqueId, l_obj_name))
				return INVALID_HANDLE_VALUE;

			HANDLE hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, request.pid);
			if (!hProcess)
				return INVALID_HANDLE_VALUE;

			HANDLE hFile = CreateFile(l_obj_name, 
				GENERIC_READ | GENERIC_WRITE, 
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);

			HANDLE hResult;
			if (hFile == INVALID_HANDLE_VALUE
				|| !DuplicateHandle(GetCurrentProcess(), hFile, hProcess, &hResult, 0, FALSE, DUPLICATE_CLOSE_SOURCE|DUPLICATE_SAME_ACCESS))
			{
				hResult = INVALID_HANDLE_VALUE;
			}

			CloseHandle(hProcess);

			return hResult;
		}

		static DWORD WINAPI ServerThread(LPVOID pParam)
		{
			static_cast<CServer*>(pParam)->Server();
			return 0;
		}

		void Server()
		{
			COverlapped ovl;

			HANDLE hPipe = CreateNamedPipe( 
				sPipeName,             // pipe name 
				PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
				PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
				PIPE_UNLIMITED_INSTANCES, // max. instances  
				1024,                     // output buffer size 
				1024,                     // input buffer size 
				NMPWAIT_USE_DEFAULT_WAIT, // client time-out 
				SA_Get(m_pSA));           // default security attribute 
			if (hPipe == INVALID_HANDLE_VALUE) 
				return;
			for (;;) 
			{
				ovl.PrepareOverlapped();
				if (!ConnectNamedPipe(hPipe, &ovl) 
					|| GetLastError() != ERROR_PIPE_CONNECTED)
				{
					HANDLE evs[] = {ovl.hEvent, m_evStop};
					if (WaitForMultipleObjects(2, evs, FALSE, INFINITE) != WAIT_OBJECT_0)
					{
						CloseHandle(hPipe);
						break;
					}
				} 

				CSOSrvRequest request;
				DWORD count;
				ovl.PrepareOverlapped();
				BOOL bReadRes = ReadFile(hPipe, &request, sizeof(request), &count, &ovl);
				if (!bReadRes && GetLastError() == ERROR_IO_PENDING)
					bReadRes = GetOverlappedResult(hPipe, &ovl, &count, TRUE);
				if (bReadRes && count == sizeof(request)) 
				{
					HANDLE response = GetFile(request); 
					ovl.PrepareOverlapped();
					if (!WriteFile(hPipe, &response, sizeof(response), &count, &ovl)
						&& GetLastError() == ERROR_IO_PENDING)
						GetOverlappedResult(hPipe, &ovl, &count, TRUE);
				}
				FlushFileBuffers(hPipe); 
				DisconnectNamedPipe(hPipe); 
			} 
			CloseHandle(hPipe); 
		}
	};

	HANDLE GetFile(const char* sName, unsigned long uniqueId)
	{
		HANDLE hPipe = CreateFile(sPipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hPipe == INVALID_HANDLE_VALUE)
		{
			// BUG 17897: дадим время pipe-serverу подняться, чтобы принять запрос (см. также выше)
			Sleep(10);
			if (INVALID_HANDLE_VALUE == (hPipe = CreateFile(sPipeName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)))
				return INVALID_HANDLE_VALUE;
		}

		CSOSrvRequest req;
		req.pid = GetCurrentProcessId();
		req.uniqueId = uniqueId;
		strcpy(req.name, sName);

		__int64 hFile64 = 0;
		HANDLE hFile = INVALID_HANDLE_VALUE;
		DWORD count;
		if (WriteFile(hPipe, &req, sizeof(req), &count, NULL) && count == sizeof(req)
			&& ReadFile(hPipe, &hFile64, sizeof(hFile64), &count, NULL))
		{
			// BUG FIX 13942
			if (count == sizeof(hFile) || count == 4 || count == 8)
				hFile = (HANDLE)hFile64;
		}

		CloseHandle(hPipe);
		return hFile;
	}
}

bool CSharedTable::Init(const char *name, long data_size, bool key_unique, bool owned_process, ECreationFlags flags)
{
	m_bStopServer = false;
	m_DataSize = data_size;
	m_fKeyUnique = key_unique;
	m_fOwnedProcess = owned_process;
	m_hSecure = SA_Create(SA_AUTHORITY_EVERYONE, SA_ACCESS_MASK_ALL);
	
	m_RecordSize = sizeof(tRECORD_DATA) + m_DataSize - 1;
	m_RecordSize += 7 - (m_RecordSize - 1) % 8;
	
	m_ProccessId = GetCurrentProcessId();
	
	char l_obj_name[100]="", l_sync_name[100]="";
	
	sprintf(l_obj_name,  "%s%s#%x", IsGlobalNameSpace() ? "Global\\" : "", name, g_UniqueId);
	sprintf(l_sync_name, "%s:sync_obj", l_obj_name);
	m_hSyncObj = CreateMutex(SA_Get(m_hSecure), FALSE, l_sync_name);

	if( !m_hSyncObj )
		return false;

	// Для Vista используем ERROR_ALREADY_EXISTS мьютекса для определения того,
	// что таблица открывается первый раз, т.к. файл после закрытия таблицы не удаляется
	DWORD l_err = GetLastError();

	LOCK();
	
	m_hFile = 0;

	if((DWORD)(LOBYTE(LOWORD(GetVersion()))) <= 5)
	{
		m_hFile = CreateFileMapping(INVALID_HANDLE_VALUE, SA_Get(m_hSecure),
			PAGE_READWRITE, 0, TABLE_SIZE(MAX_TABLE_COUNT), l_obj_name);

		l_err = GetLastError();
	}
	else
	{
		if (flags == CrStartPipe && InterlockedIncrement(&CSharedTable::m_nTablesCount) == 1)
		{
			m_pFileOpenServer = new OpenFileService::CServer();
			((OpenFileService::CServer*)m_pFileOpenServer)->Start();
			m_bStopServer = true;
		}

		// попробуем через файл на диске под Vista
		if (flags == CrUsePipe)
		{
			// bug fix 20380
			// GetFile иногда зависает, вроде бы поправил (не передавался OVERLAPPED), но на
			// всякий случай всё равно снимаю лочку
			UNLOCK();
			m_hTempFile = OpenFileService::GetFile(name, g_UniqueId);
			l_err = ERROR_ALREADY_EXISTS;
			LOCK();
		}
		else if (GetSharedFileName(name, g_UniqueId, l_obj_name))
		{
			// Исправлено. Нельзя использовать 'delete on close'! Т.к. после закрытия одним из процессом,
			// их уже не сможет открыть другой. Файлы будут удалены при деинсталляции продукта.
			m_hTempFile = CreateFile(l_obj_name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_HIDDEN /*| FILE_FLAG_DELETE_ON_CLOSE*/, NULL);

			if (m_hTempFile != INVALID_HANDLE_VALUE)
			{
				// поставим Full Control для Everyone, чтобы не работал механизм виртуализации в Vista
				SetFileSecurity(l_obj_name, DACL_SECURITY_INFORMATION, SA_Get(m_hSecure) ? SA_Get(m_hSecure)->lpSecurityDescriptor : NULL);

				if (l_err != ERROR_ALREADY_EXISTS)
				{
					// Это первый компонент, который открывает файл - очищаем файл перед использованием
					SetEndOfFile(m_hTempFile);
				}
			}
		}
		if(m_hTempFile != INVALID_HANDLE_VALUE)
		{
			// создаем FileMapping на существующем файле
			m_hFile = CreateFileMapping(m_hTempFile, SA_Get(m_hSecure),
				PAGE_READWRITE, 0, TABLE_SIZE(MAX_TABLE_COUNT), NULL);
		}
	}
	
	if( m_hFile )
	{
		m_pTable = (LPBYTE)MapViewOfFile(m_hFile, FILE_MAP_WRITE, 0, 0, TABLE_SIZE(CHUNCK_COUNT));
		if( m_pTable )
		{
			m_RecordCount = CHUNCK_COUNT;
			if( l_err != ERROR_ALREADY_EXISTS )
			{
				ZeroMemory(m_pTable, TABLE_SIZE(CHUNCK_COUNT));
				TABLE_DATA()->m_ViewCount = CHUNCK_COUNT;
			}
			else if( TABLE_DATA()->m_ViewCount != m_RecordCount )
				ReviewTable(TABLE_DATA()->m_ViewCount);
		}
	}
	UNLOCK();
	return !!m_pTable;
}


void CSharedTable::Done()
{
	if (m_bStopServer && InterlockedDecrement(&CSharedTable::m_nTablesCount) == 0)
	{
		delete (OpenFileService::CServer*)m_pFileOpenServer;
		m_pFileOpenServer = NULL;
	}

	if( m_pTable )
	{
		ClearProcessRecords(m_ProccessId);
		UnmapViewOfFile(m_pTable);
		m_pTable = NULL;
	}
	
	if( m_hFile )
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}
	
	if( m_hSyncObj )
	{
		CloseHandle(m_hSyncObj);
		m_hSyncObj = NULL;
	}
	
	if( m_hSecure )
	{
		SA_Destroy(m_hSecure);
		m_hSecure = NULL;
	}

	if( m_hTempFile != INVALID_HANDLE_VALUE )
	{
		CloseHandle(m_hTempFile);
		m_hTempFile = INVALID_HANDLE_VALUE;
	}
}

inline bool CSharedTable::ReviewTable(long count)
{
	UnmapViewOfFile(m_pTable);
	m_pTable = (LPBYTE)MapViewOfFile(m_hFile, FILE_MAP_WRITE, 0, 0, TABLE_SIZE(count));
	m_RecordCount = count;
	return m_pTable != NULL;
}

//************************************************************************

#elif defined (__unix__)
bool CSharedTable::Init(const char *name, long data_size, bool key_unique, bool owned_process, ECreationFlags flags)
{
	m_DataSize = data_size;
	m_fKeyUnique = key_unique;
	m_fOwnedProcess = owned_process;
	
	m_RecordSize = sizeof(tRECORD_DATA) + m_DataSize - 1;
	m_RecordSize += 7 - (m_RecordSize - 1) % 8;
	
	m_ProccessId = getpid ();
	
	std::string l_obj_name = "/tmp/";
	l_obj_name += name;
	
	pthread_mutexattr_t l_attr;
	if ( pthread_mutexattr_init ( &l_attr ) ||
		pthread_mutexattr_settype( &l_attr, PTHREAD_MUTEX_RECURSIVE ) ||
		pthread_mutex_init ( &m_hSyncObj, &l_attr ) ||
		pthread_mutexattr_destroy( &l_attr ) )
		return false;

//	RM - fix for daemon/client access rights. File should be created allowing everyone read/right access
//	if ( ( m_hFile = open ( l_obj_name.c_str (), O_RDWR|O_CREAT, S_IRWXU ) ) < 0 )
//		return false;
	mode_t oldMode = umask( 0 );
	if ( ( m_hFile = open ( l_obj_name.c_str (), O_RDWR|O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH  ) ) < 0 )
	{
		umask( oldMode );
		return false;        
	}
	umask( oldMode );

	bool l_exists = false;
	
	if ( !flock ( m_hFile, LOCK_EX|LOCK_NB ) ) {
		ftruncate ( m_hFile, 0);
		ftruncate ( m_hFile, TABLE_SIZE(MAX_TABLE_COUNT));
	}
	else
		l_exists = true;
	
	flock ( m_hFile, LOCK_SH );
	
	LOCK();
	
	if( ReviewTable ( CHUNCK_COUNT ) ) {
		if( l_exists ) {
			if ( TABLE_DATA()->m_ViewCount != m_RecordCount )
				ReviewTable(TABLE_DATA()->m_ViewCount);
		}
		else {
			memset ( m_pTable, 0, TABLE_SIZE(CHUNCK_COUNT) );
			TABLE_DATA()->m_ViewCount = CHUNCK_COUNT;
		}
		
	}
	UNLOCK();
	return !!m_pTable;
}

void CSharedTable::Done()
{
	if( m_pTable ) {
		ClearProcessRecords(m_ProccessId);
		munmap ( m_pTable, TABLE_SIZE(m_RecordCount));
		m_pTable = 0;
	}
	pthread_mutex_destroy ( &m_hSyncObj );
	flock ( m_hFile, LOCK_UN );
	close (m_hFile );
	m_hFile = -1;
	
}

inline bool CSharedTable::ReviewTable(long count)
{
	if ( m_pTable )
		munmap ( m_pTable, TABLE_SIZE (m_RecordCount) );
	m_pTable = (char*)mmap ( 0, TABLE_SIZE(count), PROT_READ|PROT_WRITE, MAP_SHARED, m_hFile, 0 );
	m_RecordCount = count;
	return m_pTable != 0;
}

#endif

//************************************************************************

void CSharedTable::Dump()
{
	int l_count = TABLE_DATA()->m_Count;
	
	tRECORD_DATA *l_data = RECORD_DATA(0);
	for( int l_pos = 0; l_pos < l_count; l_pos++, RECORD_NEXT(l_data) )
	{
		char buf[1000];
		sprintf(buf, "%s %d\n", (char*)((char*)l_data->m_Data + 4), l_data->m_ProccessId);
#if defined (_WIN32)
		OutputDebugString(buf);
#elif defined (__unix__)
		fprintf ( stderr, buf );
#endif
	}
}

hRecord	CSharedTable::AddRecord(void *key, long process)
{
	if(process && process != m_ProccessId)
		return SHTR_INVALID_HANDLE;
	
	tRECORD_DATA *l_data = RECORD_DATA(0);
	int l_pos;
	for( l_pos = 0; l_data->m_UsageCount && l_pos < MAX_TABLE_COUNT; l_pos++ )
		RECORD_NEXT(l_data);
	
	if( l_pos >= MAX_TABLE_COUNT )
		return SHTR_INVALID_HANDLE;
	
	tTABLE_DATA *l_table = TABLE_DATA();
	if( l_pos >= l_table->m_Count )
	{
		l_table->m_Count = l_pos + 1;
		
		if( l_table->m_Count == l_table->m_ViewCount )
		{
			long l_count = l_table->m_ViewCount + CHUNCK_COUNT;
			if( !ReviewTable(l_count) )
				return SHTR_INVALID_HANDLE;
			
			TABLE_DATA()->m_ViewCount = l_count;
			l_data = RECORD_DATA(l_pos);
		}
	}
	
	l_data->m_Record = l_pos;
	l_data->m_UsageCount = 1;
	l_data->m_CloseState = 0;
	l_data->m_ProccessId = m_ProccessId;
	memset(l_data->m_Data, 0, m_DataSize);
	
	if( !InitKeyData(l_pos, key, l_data->m_Data) )
	{
		l_data->m_UsageCount = 0;
		l_pos = SHTR_INVALID_HANDLE;
	}
	return l_pos;
}

hRecord	CSharedTable::FindRecord(void *key, long flags, long process, void* data)
{
	if( !m_pTable )
		return SHTR_INVALID_HANDLE;
	
	if( !(flags & SHTR_FLAG_NOLOCK) )
		LOCK();
	
	if( TABLE_DATA()->m_ViewCount != m_RecordCount )
		if( !ReviewTable(TABLE_DATA()->m_ViewCount) )
		{
			if( !(flags & SHTR_FLAG_NOLOCK) )
				UNLOCK();
			return SHTR_INVALID_HANDLE;
		}
		
		int l_count = TABLE_DATA()->m_Count;
		
		tRECORD_DATA *l_data = RECORD_DATA(0), *l_find_data = NULL;
		for( int l_pos = 0; l_pos < l_count; l_pos++, RECORD_NEXT(l_data) )
		{
			if( !l_data->m_UsageCount || l_data->m_CloseState )
				continue;
			
			if( !CompareKey(l_pos, key, l_data->m_Data) )
				continue;
			
			if( process && process != l_data->m_ProccessId )
				continue;
			
			if( l_data->m_ProccessId == process || l_data->m_ProccessId == m_ProccessId || m_fKeyUnique )
			{
				l_find_data = l_data;
				break;
			}
			
			if( !l_find_data || l_find_data->m_UsageCount > l_data->m_UsageCount )
				l_find_data = l_data;
		}
		
		hRecord l_record = SHTR_INVALID_HANDLE;
		
		if( l_find_data )
		{
			l_record = l_find_data->m_Record;
			if( data )
				memcpy(data, l_find_data->m_Data, m_DataSize);
			
			if( flags & SHTR_FLAG_REF )
				l_find_data->m_UsageCount++;
			else if( flags & SHTR_FLAG_DEREF )
				l_find_data->m_UsageCount--;
		}
		
		if( !(flags & SHTR_FLAG_NOLOCK) )
			UNLOCK();
		return l_record;
}

void CSharedTable::EnumRecords(void *key, long* processes, long* count)
{
	if( !m_pTable )
		return;
	LOCK()
	if( TABLE_DATA()->m_ViewCount != m_RecordCount )
		if( !ReviewTable(TABLE_DATA()->m_ViewCount) )
		{
			UNLOCK()
			return;
		}
		
	int l_pos, i, l_count = TABLE_DATA()->m_Count;
	
	tRECORD_DATA *l_data = RECORD_DATA(0);
	for( l_pos = 0, i = 0; l_pos < l_count && i < 0x100; l_pos++, RECORD_NEXT(l_data) )
	{
		if( !l_data->m_UsageCount || l_data->m_CloseState )
			continue;
		
		if( CompareKey(l_pos, key, l_data->m_Data) )
			processes[i++] = l_data->m_ProccessId;
	}
	*count = i;
	UNLOCK()
}

hRecord	CSharedTable::InsertRecord(void *key, long process)
{
	if( !m_pTable )
		return SHTR_INVALID_HANDLE;
	
	LOCK();
	
	hRecord l_record = FindRecord(key, SHTR_FLAG_NOLOCK, process);
	if( l_record == SHTR_INVALID_HANDLE )
		l_record = AddRecord(key, process);
	
	UNLOCK();
	return l_record;
}

hRecord	CSharedTable::ReplaceRecord(void *key, long process)
{
	if( !m_pTable )
		return SHTR_INVALID_HANDLE;
	
	LOCK();

	DeleteRecord(key, process);
	hRecord l_record = AddRecord(key, process);
	
	UNLOCK();
	return l_record;
}

void	CSharedTable::DeleteRecord(void *key, long process)
{
	if( !m_pTable )
		return;
	
	LOCK();
	
	hRecord l_record = FindRecord(key, SHTR_FLAG_DEREF | SHTR_FLAG_NOLOCK, process);
	if( l_record != SHTR_INVALID_HANDLE )
	{
		tRECORD_DATA *l_data = RECORD_DATA(l_record);
		DeinitData(l_record, l_data->m_Data);
		l_data->m_CloseState = 1;
	}
	
	UNLOCK();
}

long	CSharedTable::GetRecordProcess(hRecord record)
{
	return RECORD_DATA(record)->m_ProccessId;
}

void *	CSharedTable::GetRecordData(hRecord record, void* data)
{
	void* res = NULL;
	LOCK();
	if( record > 0 && record < TABLE_DATA()->m_Count )
	{
		memcpy(data, RECORD_DATA(record)->m_Data, m_DataSize);
		res = data;
	}
	UNLOCK();
	return res;
}

void *	CSharedTable::LockRecordData(hRecord record)
{
	LOCK();
	return RECORD_DATA(record)->m_Data;
}

void	CSharedTable::UnlockRecordData(hRecord record)
{
	UNLOCK();
}

void	CSharedTable::ClearProcessRecords(long process)
{
	if( !m_pTable )
		return;
	
	LOCK();
	
	if( TABLE_DATA()->m_ViewCount != m_RecordCount )
		if( !ReviewTable(TABLE_DATA()->m_ViewCount) )
		{
			UNLOCK()
				return;
		}
		
		int l_count = TABLE_DATA()->m_Count;
		
		tRECORD_DATA *l_data = RECORD_DATA(0);
		for( int l_pos = 0; l_pos < l_count; l_pos++, RECORD_NEXT(l_data) )
			if( l_data->m_ProccessId == process && m_fOwnedProcess )
				l_data->m_UsageCount = 0;
			else
				ClearProcess(l_pos, l_data->m_Data, process);
			
			UNLOCK();
}

//************************************************************************
