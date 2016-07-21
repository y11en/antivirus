#include "StdAfx.h"
#include <windows.h>
#include <string>
#include <tchar.h>
#include <time.h>
#include <crtdbg.h>
#include "log_media.h"
#include "Registry.h"
#include "../stuff/StdString.h"
#include "../Service/sa.h"
#include "UnicodeConv.h"
#include "Thread.h"

using namespace boost;
using namespace KLUTIL;

#ifndef VERIFY
#	ifdef _DEBUG
#		define VERIFY(f) ASSERT(f)
#	else
#		define VERIFY(f) ((void)(f))
#	endif
#endif

namespace KLUTIL
{
	static DWORD g_dwThreadNameTlsId = TLS_OUT_OF_INDEXES;
	static LPTSTR pszUnknownThreadName = _T("Unknown");
	void InitializeLog()
	{
		static bool bInitialized = false;
		if (bInitialized)
			return;
		bInitialized = true;
		g_dwThreadNameTlsId = TlsAlloc();
		ASSERT(g_dwThreadNameTlsId != TLS_OUT_OF_INDEXES);
	}
	LPCTSTR GetLogThreadName()
	{
		ASSERT(g_dwThreadNameTlsId != TLS_OUT_OF_INDEXES);
		if (g_dwThreadNameTlsId == TLS_OUT_OF_INDEXES)
			return NULL;
		return static_cast<LPCTSTR>(TlsGetValue(g_dwThreadNameTlsId));
	}
	void FreeLogThreadName()
	{
		ASSERT(g_dwThreadNameTlsId != TLS_OUT_OF_INDEXES);
		if (g_dwThreadNameTlsId == TLS_OUT_OF_INDEXES)
			return;
		LPTSTR pThreadName = static_cast<LPTSTR>(TlsGetValue(g_dwThreadNameTlsId));
		if (pThreadName != pszUnknownThreadName)
		{
			free(pThreadName);
			VERIFY(TlsSetValue(g_dwThreadNameTlsId, pszUnknownThreadName));
		}
	}
	void SetLogThreadName(LPCTSTR pszThreadName)
	{
		ASSERT(g_dwThreadNameTlsId != TLS_OUT_OF_INDEXES);
		if (g_dwThreadNameTlsId == TLS_OUT_OF_INDEXES)
			return;
		FreeLogThreadName();
		VERIFY(TlsSetValue(g_dwThreadNameTlsId, LPVOID(pszThreadName != NULL ? _tcsdup(pszThreadName) : pszUnknownThreadName)));
		SetDebuggerThreadName(-1, __LPCSTR(pszThreadName));
	}

	//! Создаёт путь к файлу.
	KLUTIL_API bool CreateFileDir(LPCTSTR pszFilePath)
	{
		// Найдём полный путь
		TCHAR szBuffer[1024];
		LPTSTR pszFilePart;
		DWORD dwRes = GetFullPathName(pszFilePath, sizeof(szBuffer)/sizeof(*szBuffer), szBuffer, &pszFilePart);
		if (dwRes == 0
			|| dwRes >= sizeof(szBuffer)/sizeof(*szBuffer)
			|| pszFilePart == NULL)
			return false;
		
		// Отрежем имя файла
		*pszFilePart = _T('\0');
		
		CStdString sPath(szBuffer);
		sPath.Replace(_T('/'), _T('\\'));
		ASSERT(sPath.Right(1) == _T("\\"));
		
		int nPos = 4; // Пропустим имя диска
		while ( (nPos = sPath.Find(_T('\\'), nPos + 1)) != -1)
		{
			if (!CreateDirectory(sPath.Left(nPos), NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
				return false;
		}
		return true;
	}
}

void CLogBase::SetThreadName(LPCTSTR pszThreadName)
{
	SetLogThreadName(pszThreadName);
}

LPCTSTR CLogBase::GetThreadName()
{
	LPCTSTR pszName = GetLogThreadName();
	return pszName ? pszName : _T("Unknown");
}


TLogMediaPtr CLogBase::GetAppLogMedia()
{
	static TLogMediaPtr pAppLog(new CLogMediaProxy());
	return pAppLog;
}

void CLogBase::SetAppLogMedia(TLogMediaPtr pLog)
{
	ASSERT(pLog != GetAppLogMedia());
	if (pLog == GetAppLogMedia())
		return;
	static_pointer_cast<CLogMediaProxy>(GetAppLogMedia())->SetLog(pLog);
}

TLogMediaPtr CLogBase::CreateConsoleMedia()
{
	static TLogMediaPtr pConsoleMedia(new CConsoleMedia());
	if (pConsoleMedia->IsWorking())
		return pConsoleMedia;
	return TLogMediaPtr();
}

TLogMediaPtr CLogBase::CreateFileMedia(LPCTSTR pszFilename, bool bAppend, bool bFlush)
{
	TLogMediaPtr pLog(new CFileMedia(pszFilename, bAppend, bFlush));
	if (pLog->IsWorking())
		return pLog;
	return TLogMediaPtr();
}

TLogMediaPtr CLogBase::CreateDebugMedia(DWORD dwParam)
{
	TLogMediaPtr pDebugMedia(new CDebugMedia(dwParam));
	if (pDebugMedia->IsWorking())
		return pDebugMedia;
	return TLogMediaPtr();
}


//TraceType=[debug,info,warning,error]
//TraceLevel=[0,1,2...]
TLogMediaPtr ApplyFilterFromRegistry(CRegistry& reg,
									 TLogMediaPtr pLogMedia)
{
	shared_ptr<CFilterLogMedia> pFiltLog;
	
	CStdString temp;
	
	// CTypeLevelFilter
	DWORD lev = LMAX;
	ELogMessageType type = eLM_Debug;
	bool bHaveTraceType = reg.GetValue(_T("TraceType"), temp.GetBuffer(128), 128);
	if (bHaveTraceType)
	{
		temp.ReleaseBuffer();
		temp.ToLower();
		if (temp == _T("debug")) type = eLM_Debug;
		else if (temp == _T("info")) type = eLM_Info;
		else if (temp == _T("warning")) type = eLM_Warning;
		else if (temp == _T("error")) type = eLM_Error;
		else
		{
#ifdef _UNICODE
			_RPT1(_CRT_WARN, "[CreateMediaFromRegistry] Unknown message type \"%S\" found in registry\n", (LPCWSTR)temp);
#else
			_RPT1(_CRT_WARN, "[CreateMediaFromRegistry] Unknown message type \"%s\" found in registry\n", (LPCSTR)temp);
#endif
			bHaveTraceType = false;
		}
	}
	
	bool bHaveTraceLevel = reg.GetValue(_T("TraceLevel"), lev);
	if (bHaveTraceType || bHaveTraceLevel)
	{
		if (!pFiltLog)
			pFiltLog = shared_ptr<CFilterLogMedia>(new CFilterLogMedia(pLogMedia));
		pFiltLog->AddFilter(TFilterPtr(new CTypeLevelFilter(type, static_cast<ELogMessageLevel>(lev))));
	}
	
	return !pFiltLog ? pLogMedia : static_pointer_cast<ILogMedia>(pFiltLog);
}

TLogMediaPtr CLogBase::CreateMediaFromRegistry(HKEY hRoot,
											   LPCTSTR pszPath,
											   bool bAppLog)
{
	CStdString value;
	CRegistry reg(hRoot, pszPath);
	DWORD dwValue = 0;
	// Проверим что текщий ключ существует
	if (!reg.IsKeyExist())
		return TLogMediaPtr();

	int nMedia = 0;
	TLogMediaPtr pMedia;
	shared_ptr<CLogMediaColl> pMediaColl(new CLogMediaColl());

	if (!bAppLog && reg.GetValue(_T("TraceToApp"), dwValue) && dwValue != FALSE)
	{
		TLogMediaPtr p = GetAppLogMedia();
		if (p)
		{
			pMedia = p;
			++nMedia;
			pMediaColl->Add(pMedia);
		}
	}
	if (reg.GetValue(_T("TraceFile"), value.GetBuffer(128), 128))
	{
		value.ReleaseBuffer();
		dwValue = TRUE;
		reg.GetValue(_T("TraceFileAppend"), dwValue);
		DWORD dwFlush = FALSE;
		reg.GetValue(_T("TraceFileFlush"), dwFlush);

		TLogMediaPtr p = CreateFileMedia(value, dwValue != FALSE, dwFlush != FALSE);
		if (p)
		{
			pMedia = p;
			++nMedia;
			pMediaColl->Add(pMedia);
		}
	}
	if (reg.GetValue(_T("TraceToDebug"), dwValue) && dwValue != FALSE)
	{
		TLogMediaPtr p = CreateDebugMedia(dwValue);
		if (p)
		{
			pMedia = p;
			++nMedia;
			pMediaColl->Add(pMedia);
		}
	}
	if (reg.GetValue(_T("TraceToConsole"), dwValue) && dwValue != FALSE)
	{
		TLogMediaPtr p = CreateConsoleMedia();
		if (p)
		{
			pMedia = p;
			++nMedia;
			pMediaColl->Add(pMedia);
		}
	}
	for (int i = 1; ; ++i)
	{
		value.Format(_T("%s\\Trace%i"), pszPath, i);
		TLogMediaPtr p = CreateMediaFromRegistry(hRoot, value);
		if (!p)
			break;
		pMedia = p;
		++nMedia;
		pMediaColl->Add(pMedia);
	}

	if (nMedia != 0)
		pMedia = ApplyFilterFromRegistry(reg, nMedia == 1 ? pMedia : pMediaColl);

	if (bAppLog)
		CLog::SetAppLogMedia(pMedia);

	if (pMedia && reg.GetValue(_T("TraceHeader"), value.GetBuffer(1024), 1024))
	{
		value.ReleaseBuffer();
		CStdString line(value.GetLength() + 4, _T('='));
		CLocalLog log(pMedia, _T("HEADER"));
		log.Info(_T("%s"), line.c_str());
		log.Info(_T("= %s ="), value.c_str());
		log.Info(_T("%s"), line.c_str());
	}

	return pMedia;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

void FormatLogMessage(ELogMessageType type,
					  ELogMessageLevel nLevel,
					  LPCTSTR pszDate,
					  LPCTSTR pszTime,
					  LPCTSTR pszThreadId,
					  LPCTSTR pszThreadName,
					  LPCTSTR pszModule,
					  LPCTSTR pszMessage,
					  CStdString& output)
{
#if 1
	output.Empty();
	output.reserve(1024);
	
	output += pszTime;
	output += _T(" [");
	output += pszThreadId;
	output += _T(":");
	size_t nThreadNameLen = _tcslen(pszThreadName);
	if (nThreadNameLen > 12)
		output.append(pszThreadName, 12);
	else
	{
		output.append(12 - nThreadNameLen, _T(' '));
		output += pszThreadName;
	}
	output += _T("] ");
	
	switch (type)
	{
	case eLM_Info:
		output += _T('I');
		break;
	case eLM_Debug:
		output += _T('-');
		break;
	case eLM_Warning:
		output += _T('W');
		break;
	case eLM_Error:
		output += _T('E');
		break;
	default:
		ASSERT(false);
	}
	
	if (nLevel>0)
		output.AppendFormat(_T("%i"), nLevel);
	else
		output += _T(' ');
	output += _T(" : [");
	output += pszModule;
	output += _T("] ");
	output += pszMessage;
	output.TrimRight();
#else
	output.Empty();
	output.reserve(1024);
	
	output += pszDate;
	output += _T(' ');
	output += pszTime;
	output += _T("\t");
	output += pszThreadId;
	output += _T("\t");
	output += pszThreadName;
	output += _T("\t");
	output += pszModule;
	output += _T("\t");
	
	switch (type)
	{
	case eLM_Info:
		output += _T("Inf");
		break;
	case eLM_Debug:
		output += _T("Dbg");
		break;
	case eLM_Warning:
		output += _T("Wrn");
		break;
	case eLM_Error:
		output += _T("Err");
		break;
	default:
		ASSERT(false);
	}
	
	if (nLevel>0)
		output.AppendFormat(_T("%i"), nLevel);
	output += _T('\t');
	output += pszMessage;
	output.TrimRight();
#endif
}

CStdString GetSystemInformation()
{
	CStdString info = _T("Microsoft Windows ");
	
	OSVERSIONINFO rcOS;
	ZeroMemory(&rcOS, sizeof(rcOS));
	rcOS.dwOSVersionInfoSize = sizeof(rcOS);
	
	GetVersionEx(&rcOS);
	
	if (rcOS.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		switch (rcOS.dwMinorVersion)
		{
		case 0: info += _T("95"); break;
		case 10: info += _T("98"); break;
		case 90: info += _T("Me"); break;
		}
		if (!_tcscmp(rcOS.szCSDVersion, _T(" A"))) 
			info += _T(" Second Edition");
		else 
			if (!strcmp(rcOS.szCSDVersion, _T(" C"))) 
				info += _T(" OSR2");
	}
	else if (rcOS.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		switch (rcOS.dwMajorVersion)
		{
		case 3:	info += _T("NT 3.51"); break;
		case 4: info += _T("NT 4.0"); break;
		case 5:
			switch (rcOS.dwMinorVersion)
			{
			case 0: info += _T("2000"); break;
			case 1: info += _T("XP"); break;
			case 2: info += _T("2003 Server"); break;
			default: info += _T("future version"); break;
			}
			break;
		default: info += _T("future version"); break;
		}
		if (_tcslen(rcOS.szCSDVersion) > 0)
		{
			info += _T(", ");
			info += rcOS.szCSDVersion;
		}
	}
	return info;
}

CStdString GetModuleInformation()
{
	CStdString info;
	TCHAR szApp[MAX_PATH];
	if(!GetModuleFileName(NULL, szApp, MAX_PATH))
		return info;

	info = szApp;

	DWORD dwDummy;
	DWORD dwInfoSize;

	dwInfoSize = GetFileVersionInfoSize(szApp, &dwDummy);
	if(dwInfoSize)
	{
		LPVOID pInfoBuffer = _alloca(dwInfoSize);
		if(GetFileVersionInfo(szApp, 0, dwInfoSize, pInfoBuffer))
		{
			VS_FIXEDFILEINFO *pInfo = NULL;
			UINT InfoSize = 0;
			if(VerQueryValue (pInfoBuffer, _T("\\"), (LPVOID *)&pInfo, &InfoSize) && InfoSize == sizeof(VS_FIXEDFILEINFO))
			{
				CStdString strVersion;
				strVersion.Format(_T(" (%d.%d.%d.%d)"), 
							HIWORD(pInfo->dwFileVersionMS),
							LOWORD(pInfo->dwFileVersionMS),
							HIWORD(pInfo->dwFileVersionLS),
							LOWORD(pInfo->dwFileVersionLS)
						);
				info += strVersion;
			}
		}
	}
	
	return info;
}

//////////////////////////////////////////////////////////////////////////
// CConsoleMedia
//////////////////////////////////////////////////////////////////////////

CConsoleMedia::CConsoleMedia()
{
	m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (m_hConsole == NULL)
	{
		if (!AllocConsole() //|| !SetConsoleTitle(_T("Log"))
			|| (m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE)) == NULL)
			return;
	}
	GetConsoleScreenBufferInfo(m_hConsole, &m_info);
}

void CConsoleMedia::Write(ELogMessageType type,
						  ELogMessageLevel nLevel,
						  LPCTSTR pszDate,
						  LPCTSTR pszTime,
						  LPCTSTR pszThreadId,
						  LPCTSTR pszThreadName,
						  LPCTSTR pszModule,
						  LPCTSTR pszMessage)
{
	if (m_hConsole==NULL)
		return;
	WORD color = FOREGROUND_GREEN|FOREGROUND_INTENSITY;
	//FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY;
	switch (type)
	{
	case eLM_Debug:
		color = FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE;
		break;
	case eLM_Warning:
		color = FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
		break;
	case eLM_Error:
		color = FOREGROUND_RED|FOREGROUND_INTENSITY;
		break;
	}
	
	CStdString output;
	FormatLogMessage(type, nLevel, pszDate, pszTime, pszThreadId, pszThreadName, pszModule, pszMessage, output);
#ifndef _UNICODE
	output.AnsiToOem();
#endif
	
	CCritSec::CLock lock(m_cs);
	SetConsoleTextAttribute(m_hConsole, color);
	DWORD dwWritten;
	WriteConsole(m_hConsole, (LPCTSTR)output, output.size(), &dwWritten, NULL);
	
	SetConsoleTextAttribute(m_hConsole, m_info.wAttributes);
	WriteConsole(m_hConsole, _T("\n"), 1, &dwWritten, NULL);
}

//////////////////////////////////////////////////////////////////////////
// CFileMedia
//////////////////////////////////////////////////////////////////////////

class CSAttr
{
	HSA m_hSA;
public:
	CSAttr() { m_hSA = ::SA_Create(SA_AUTHORITY_EVERYONE, SA_ACCESS_MASK_ALL); }
	~CSAttr() { if (m_hSA)  ::SA_Destroy(m_hSA); }
	operator LPSECURITY_ATTRIBUTES () { return ::SA_Get(m_hSA); }
};

CFileMedia::CFileMedia(LPCTSTR pszFilename, bool bAppend, bool bFlush)
	: m_bFlush(bFlush)
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	CStdString sFilename(pszFilename);
	sFilename.Replace(_T("%DATETIME%"), _T("%DATE% %TIME%"));
	if (sFilename.Find(_T("%DATE%")) != -1)
	{
		TCHAR bufdate[128];
		_stprintf(bufdate, _T("%i-%02i-%02i"), st.wYear, st.wMonth, st.wDay);
		sFilename.Replace(_T("%DATE%"), bufdate);
	}
	if (sFilename.Find(_T("%TIME%")) != -1)
	{
		GetLocalTime(&st);
		TCHAR buftime[128];
		_stprintf(buftime, _T("%02i-%02i"), st.wHour, st.wMinute);
		sFilename.Replace(_T("%TIME%"), buftime);
	}

	if (!CreateFileDir(sFilename))
	{
		_RPT1(_CRT_ERROR, "FileMedia: Can't create folder '%s'", (LPCSTR) sFilename);
		return;
	}


	// Создадим для доступа к этому файлу мьютекс
	CStdString sMtx(sFilename);
	sMtx.ToUpper();
	for (CStdString::iterator it = sMtx.begin(), end = sMtx.end(); it != end; ++it)
	{
		if (!_istalnum(*it))
			*it = _T('_');
	}
	m_hMutex = CreateMutex(CSAttr(), TRUE, (LPCTSTR)sMtx);
	DWORD dwMtxError = GetLastError();
	
	m_hFile = CreateFile(sFilename,
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		(bAppend || dwMtxError == ERROR_ALREADY_EXISTS) ? OPEN_ALWAYS : CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		if (dwMtxError != ERROR_ALREADY_EXISTS)
			VERIFY(ReleaseMutex(m_hMutex));
		m_hMutex.Close();
		return;
	}

	if (dwMtxError != ERROR_ALREADY_EXISTS)
	{
		CStdString header;
		header.Format(
			_T("================================================\r\n")
			_T("=== Trace Log Started on %i-%02i-%02i %02i:%02i:%02i ===\r\n")
			_T("=== %s ===\r\n")
			_T("================================================\r\n")
			_T("\r\n%s\r\n\r\n")
			, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond
			, (LPCTSTR)GetSystemInformation()
			, (LPCTSTR)GetModuleInformation());
		
		LONG dwDistanceToMoveHigh = 0; // Нужен, чтобы писать в файлы больше 4Гб
		VERIFY(SetFilePointer(m_hFile, 0, &dwDistanceToMoveHigh, FILE_END) != INVALID_SET_FILE_POINTER || GetLastError() == NO_ERROR);
		DWORD dwWritten;
		VERIFY(WriteFile(m_hFile, (LPCTSTR)header, header.size() * sizeof(TCHAR), &dwWritten, NULL));
		if (m_hMutex != NULL)
			VERIFY(ReleaseMutex(m_hMutex));
	}
}

CFileMedia::~CFileMedia()
{
	if (m_hFile != INVALID_HANDLE_VALUE && WaitForSingleObject(m_hMutex, 1000) != WAIT_TIMEOUT)
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		CStdString header;
		header.Format(
			_T("=================================================\r\n")
			_T("=== Trace Log Finished on %i-%02i-%02i %02i:%02i:%02i ===\r\n")
			_T("=================================================\r\n")
			, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		
		LONG dwDistanceToMoveHigh = 0; // Нужен, чтобы писать в файлы больше 4Гб
		VERIFY(SetFilePointer(m_hFile, 0, &dwDistanceToMoveHigh, FILE_END) != INVALID_SET_FILE_POINTER || GetLastError() == NO_ERROR);
		DWORD dwWritten;
		VERIFY(WriteFile(m_hFile, (LPCTSTR)header, header.size() * sizeof(TCHAR), &dwWritten, NULL));
		VERIFY(ReleaseMutex(m_hMutex));
	}
}

bool CFileMedia::IsWorking() const
{
	return m_hFile != INVALID_HANDLE_VALUE;
}

void CFileMedia::Write(ELogMessageType type, ELogMessageLevel nLevel, LPCTSTR pszDate, LPCTSTR pszTime, LPCTSTR pszThreadId, LPCTSTR pszThreadName, LPCTSTR pszModule, LPCTSTR pszMessage)
{
	if (m_hFile == INVALID_HANDLE_VALUE
		|| WaitForSingleObject(m_hMutex, 1000) == WAIT_TIMEOUT)
		return;

	LONG dwDistanceToMoveHigh = 0; // Нужен, чтобы писать в файлы больше 4Гб
	if (SetFilePointer(m_hFile, 0, &dwDistanceToMoveHigh, FILE_END) == INVALID_SET_FILE_POINTER
		&& GetLastError() != NO_ERROR)
	{
		m_hFile.Close();
		VERIFY(ReleaseMutex(m_hMutex));
		_RPT0(_CRT_ERROR, "FileMedia: Can't set file pointer");
	}

	CStdString output;
	FormatLogMessage(type, nLevel, pszDate, pszTime, pszThreadId, pszThreadName, pszModule, pszMessage, output);
	output += _T("\r\n");
	DWORD dwWritten;
	VERIFY(WriteFile(m_hFile, (LPCTSTR)output, output.size() * sizeof(TCHAR), &dwWritten, NULL));
	if (m_bFlush)
		VERIFY(FlushFileBuffers(m_hFile));
	VERIFY(ReleaseMutex(m_hMutex));
}

//////////////////////////////////////////////////////////////////////////
// CDebugMedia
//////////////////////////////////////////////////////////////////////////

CDebugMedia::CDebugMedia(DWORD dwParam)
	: m_dwParam(dwParam)
{
}

extern "C" WINBASEAPI BOOL WINAPI IsDebuggerPresent ( VOID );

bool CDebugMedia::IsWorking() const
{
	return (m_dwParam & CLogBase::DEBUGMEDIA_FORCE) ? true : IsDebuggerPresent() != FALSE;
}

void CDebugMedia::Write(ELogMessageType type,
						ELogMessageLevel nLevel,
						LPCTSTR pszDate,
						LPCTSTR pszTime,
						LPCTSTR pszThreadId,
						LPCTSTR pszThreadName,
						LPCTSTR pszModule,
						LPCTSTR pszMessage)
{
	CStdString output;
	FormatLogMessage(type, nLevel, pszDate, pszTime, pszThreadId, pszThreadName, pszModule, pszMessage, output);
	output += _T('\n');
	
	OutputDebugString(output);
	if (type == eLM_Error && (m_dwParam & CLogBase::DEBUGMEDIA_REPORTERRORS))
	{
		_RPT1(_CRT_ERROR, "%s", output.c_str());
	}
}

//////////////////////////////////////////////////////////////////////////
// CLogMediaProxy
//////////////////////////////////////////////////////////////////////////

CLogMediaProxy::CLogMediaProxy(const TLogMediaPtr& pLog)
	: m_pLog(pLog)
{
}

CLogMediaProxy::~CLogMediaProxy()
{
}

void CLogMediaProxy::SetLog(const TLogMediaPtr& pLog)
{
	CCritSec::CLock lock(m_cs);
	m_pLog = pLog;
}

void CLogMediaProxy::Write(ELogMessageType type, ELogMessageLevel nLevel, LPCTSTR pszDate, LPCTSTR pszTime, LPCTSTR pszThreadId, LPCTSTR pszThreadName, LPCTSTR pszModule, LPCTSTR pszMessage)
{
	CCritSec::CLock lock(m_cs);
	if (m_pLog)
		m_pLog->Write(type, nLevel, pszDate, pszTime, pszThreadId, pszThreadName, pszModule, pszMessage);
}

bool CLogMediaProxy::Check(ELogMessageType type, ELogMessageLevel nLevel, LPCTSTR pszModule)
{
	CCritSec::CLock lock(m_cs);
	if (m_pLog)
		return m_pLog->Check(type, nLevel, pszModule);
	return false;
}

//////////////////////////////////////////////////////////////////////////
// CLogMediaColl
//////////////////////////////////////////////////////////////////////////

CLogMediaColl::CLogMediaColl()
{
}

CLogMediaColl::~CLogMediaColl()
{
}

void CLogMediaColl::Add(const TLogMediaPtr& pMedia)
{
	if (!pMedia)
		return;
	CCritSec::CLock lock(m_cs);
	m_MediaColl.push_back(pMedia);
}

void CLogMediaColl::Write(ELogMessageType type, ELogMessageLevel nLevel, LPCTSTR pszDate, LPCTSTR pszTime, LPCTSTR pszThreadId, LPCTSTR pszThreadName, LPCTSTR pszModule, LPCTSTR pszMessage)
{
	CCritSec::CLock lock(m_cs);
	for (TMediaColl::iterator it = m_MediaColl.begin(), end = m_MediaColl.end(); it != end; ++it)
		(*it)->Write(type, nLevel, pszDate, pszTime, pszThreadId, pszThreadName, pszModule, pszMessage);
}

bool CLogMediaColl::Check(ELogMessageType type, ELogMessageLevel nLevel, LPCTSTR pszModule)
{
	CCritSec::CLock lock(m_cs);
	// Если хотя бы один лог пропускает, то и мы пропускаем.
	for (TMediaColl::iterator it = m_MediaColl.begin(), end = m_MediaColl.end(); it != end; ++it)
		if ((*it)->Check(type, nLevel, pszModule))
			return true;
		return false;
};

//////////////////////////////////////////////////////////////////////////
// CFilterLogMedia
//////////////////////////////////////////////////////////////////////////

CFilterLogMedia::CFilterLogMedia(const TLogMediaPtr& pMedia)
	: m_pMedia(pMedia)
{
}

CFilterLogMedia::~CFilterLogMedia()
{
}

void CFilterLogMedia::AddFilter(TFilterPtr pFilter)
{
	CCritSec::CLock lock(m_cs);
	m_FilterColl.push_back(pFilter);
}

void CFilterLogMedia::Write(ELogMessageType type, ELogMessageLevel nLevel, LPCTSTR pszDate, LPCTSTR pszTime, LPCTSTR pszThreadId, LPCTSTR pszThreadName, LPCTSTR pszModule, LPCTSTR pszMessage)
{
	if (!m_pMedia)
		return;
	{
		CCritSec::CLock lock(m_cs);
		for (TFilterColl::iterator it = m_FilterColl.begin(), end = m_FilterColl.end(); it != end; ++it)
			if (!(*it)->Check(type, nLevel, pszModule))
				return;
	}
	m_pMedia->Write(type, nLevel, pszDate, pszTime, pszThreadId, pszThreadName, pszModule, pszMessage);
}

bool CFilterLogMedia::Check(ELogMessageType type, ELogMessageLevel nLevel, LPCTSTR pszModule)
{
	if (!m_pMedia)
		return false;
	{
		CCritSec::CLock lock(m_cs);
		for (TFilterColl::iterator it = m_FilterColl.begin(), end = m_FilterColl.end(); it != end; ++it)
			if (!(*it)->Check(type, nLevel, pszModule))
				return false;
	}
	return m_pMedia->Check(type, nLevel, pszModule);
}

//////////////////////////////////////////////////////////////////////////
// CLogBase
//////////////////////////////////////////////////////////////////////////

CLogBase::CLogBase(TLogMediaPtr pMedia,	LPCTSTR pszModule)
	: m_szModule(pszModule)
	, m_pMedia(pMedia)
{
	ASSERT(pszModule != NULL);
}

CLogBase::~CLogBase()
{
}

template <typename T, size_t nSize>
class CStackResizableBuf
{
	T loc_data[nSize];
	void Clear() { if (data != loc_data) delete[] data; }
public:
	CStackResizableBuf() { data = loc_data; size = nSize; }
	~CStackResizableBuf() { Clear(); }
	void Resize(size_t nNewSize) { Clear(); data = new T[nNewSize]; size = nNewSize; }

	T* data;
	size_t size;
};

bool CLogBase::IsFiltered(ELogMessageType type, ELogMessageLevel nLevel)
{
	return !m_pMedia || !m_pMedia->Check(type, nLevel, m_szModule.c_str());
}

void CLogBase::WriteVA(ELogMessageType type,
						ELogMessageLevel nLevel,
						LPCTSTR pszModule,
						LPCTSTR pszMessage,
						va_list args)  throw()
{
	if (IsFiltered(type, nLevel)) // не обрабатываем сообщение, если оно не попадает в лог
		return;
	// 75% времени тратится на new и delete, поэтому первую попытку попробуем сделать без него.
	CStackResizableBuf<TCHAR, 1024> buf;
	int pos;
	for (;;)
	{
		pos = _vsntprintf(buf.data, buf.size - 1, pszMessage, args);
		if (pos != -1)
			break;
		// BUG 16456 FIX, см. в конец WriteVAW почему
		if (buf.size >= 1024 * 256)
		{
			pos = buf.size - 1;
			break;
		}
		buf.Resize(buf.size * 2);
	}
	if (pos >= 0)
	{
		buf.data[pos] = 0;
		SYSTEMTIME st;
		GetLocalTime(&st);
		TCHAR bufdate[128], buftime[128], bufthread[128];
		_stprintf(bufdate, _T("%i-%02i-%02i"), st.wYear, st.wMonth, st.wDay);
		_stprintf(buftime, _T("%02i:%02i:%02i.%03d"), st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		_stprintf(bufthread, _T("%03x"), GetCurrentThreadId());
		m_pMedia->Write(type, nLevel, bufdate, buftime, bufthread, GetThreadName()
			, pszModule ? pszModule : m_szModule.c_str(), buf.data);
	}
}

void CLogBase::WriteVAW(ELogMessageType type,
						 ELogMessageLevel nLevel,
						 LPCWSTR pszModule,
						 LPCWSTR pszMessage,
						 va_list args)  throw()
{
#if defined(_UNICODE) || defined(UNICODE)
	WriteVA(type, nLevel, pszModule, pszMessage, args);
#else

	if (IsFiltered(type, nLevel)) // не обрабатываем сообщение, если оно не попадает в лог
		return;
	// 75% времени тратится на new и delete, поэтому первую попытку попробуем сделать без него.
	CStackResizableBuf<WCHAR, 1024> buf;
	int pos;
	for (;;)
	{
		pos = _vsnwprintf(buf.data, buf.size - 1, pszMessage, args);
		if (pos != -1)
			break;
		// BUG 16456 FIX, см. в конец WriteVAW почему
		if (buf.size >= 1024 * 256)
		{
			pos = buf.size - 1;
			break;
		}
		buf.Resize(buf.size * 2);
	}
	if (pos >= 0)
	{
		buf.data[pos] = 0;
		LPTSTR pszStr = static_cast<LPTSTR>(_alloca(buf.size));
		if (0 == WideCharToMultiByte(CP_ACP, 0/*WC_DEFAULTCHAR*/, buf.data, pos + 1, pszStr, buf.size, NULL, NULL))
		{
			_RPT1(_CRT_ERROR, "Can't convert Unicode string (error #0x%08X)", GetLastError());
			return;
		}

		LPCTSTR pszMod;
		if (pszModule == NULL)
			pszMod = m_szModule.c_str();
		else
		{
			size_t nModuleLen = wcslen(pszModule) + 1;
			LPTSTR pszModBuf = static_cast<LPTSTR>(_alloca(nModuleLen));
			if (0 == WideCharToMultiByte(CP_ACP, 0/*WC_DEFAULTCHAR*/, pszModule, nModuleLen, pszModBuf, nModuleLen, NULL, NULL))
			{
				_RPT1(_CRT_ERROR, "Can't convert Unicode string (error #0x%08X)", GetLastError());
				return;
			}
			pszMod = pszModBuf;
		}
		
		SYSTEMTIME st;
		GetLocalTime(&st);
		TCHAR bufdate[128], buftime[128], bufthread[128];
		_stprintf(bufdate, _T("%i-%02i-%02i"), st.wYear, st.wMonth, st.wDay);
		_stprintf(buftime, _T("%02i:%02i:%02i.%03d"), st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		_stprintf(bufthread, _T("%03x"), GetCurrentThreadId());
		m_pMedia->Write(type, nLevel, bufdate, buftime, bufthread, GetThreadName(), pszMod, pszStr);
	}
#endif
	/* BUG 16456 FIX
	_vsnwprintf спотыкается на строках с символом 0xFFFF,
	возвращает -1 при любой длине буфера. Соответсвтвенно
	буфер увеличивается (Resize) пока не хватит памяти,
	а потом вызывается _vsnwprintf на NULL.
	Решение: Т.к. _vsnwprintf, даже если нехватает буфера, начало заполняет,
			то просто ограничим буфер 256кб (а 0 на конце и так ставим на всякий случай)

	#include <stdarg.h>
	#include <stdio.h>
  
	void WriteVAW(wchar_t* pszMessage, va_list args)
	{
		wchar_t buf[1000];
		int n = _vsnwprintf(buf, 1000, pszMessage, args);
		printf("Return %i\nBuf is: \"%ls\"", n, buf); // n будет -1, хотя буфера хватает !!!
	}
	void WriteW(wchar_t* pszMessage, ...)
	{
		va_list ap;
		va_start(ap, pszMessage);
		WriteVAW(pszMessage, ap);
		va_end(ap);
	}
	void main()
	{
		WriteW(L"%ls!", L"123\xffff");
	}
	*/
}


//////////////////////////////////////////////////////////////////////////
// CLocalLog
//////////////////////////////////////////////////////////////////////////

CLocalLog::CLocalLog(const CLogParam& param, LPCTSTR pszModule)
	: CLogBase(param.m_pMedia, param.m_pszModule ? param.m_pszModule : pszModule)
{
}

CLocalLog::CLocalLog(TLogMediaPtr pMedia, LPCTSTR pszModule)
	: CLogBase(pMedia, pszModule)
{
	ASSERT(pszModule != NULL);
}

CLocalLog::CLocalLog(const CLogBase& log, LPCTSTR pszModule)
	: CLogBase(log.GetMedia(), pszModule)
{
}

CLocalLog::~CLocalLog()
{
}

//////////////////////////////////////////////////////////////////////////
// CLog
//////////////////////////////////////////////////////////////////////////

CLog::CLog(const CLogParam& param, LPCTSTR pszModule)
	: CLogBase(param.m_pMedia, param.m_pszModule ? param.m_pszModule : pszModule)
{
}

CLog::CLog(TLogMediaPtr pMedia, LPCTSTR pszModule)
	: CLogBase(pMedia, pszModule)
{
}

CLog::CLog(const CLogBase& log, LPCTSTR pszModule)
	: CLogBase(log.GetMedia(), pszModule)
{
}

CLog::~CLog()
{
}

void CLog::SetParams(TLogMediaPtr pMedia, LPCTSTR pszModule)
{
	CCritSec::CLock lock(m_cs);
	m_pMedia  = pMedia;
	if (pszModule != NULL)
		m_szModule = pszModule;
}

void CLog::WriteVA(ELogMessageType type,
					ELogMessageLevel nLevel,
					LPCTSTR pszModule,
					LPCTSTR pszMessage,
					va_list args) throw()
{
	CCritSec::CLock lock(m_cs);
	CLogBase::WriteVA(type, nLevel, pszModule, pszMessage, args);
}

void CLog::WriteVAW(ELogMessageType type,
					 ELogMessageLevel nLevel,
					 LPCWSTR pszModule,
					 LPCWSTR pszMessage,
					 va_list args) throw()
{
	CCritSec::CLock lock(m_cs);
	CLogBase::WriteVAW(type, nLevel, pszModule, pszMessage, args);
}

//////////////////////////////////////////////////////////////////////////
// CFilterLog
//////////////////////////////////////////////////////////////////////////

CFilterLog::~CFilterLog()
{
}
