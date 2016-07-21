// dllmain.cpp
//
// Interprocess communication library (IPC)
//
// DLL entry points
//
// (C) 2003 Kaspersky Labs
//
// Author: Vyacheslav A. Batenin (Vyacheslav.Batenin@kaspersky.com)
//

#include "ipc_impl.h"
#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <crtdbg.h>
#include "version.h"
#include "../../../Service/sa.h"

#ifdef _DEBUG
#define verify(f)          assert(f)
#else
#define verify(f)          ((void)(f))
#endif

class IIpc
{
public:
	virtual DWORD GetVersion() = 0;
	virtual HIPCSERVER ServerStart (const char *epName) = 0;
	virtual BOOL ServerStop (HIPCSERVER hServer) = 0;
	virtual HIPCCONNECTION ServerWaitForConnection (HIPCSERVER hServer, DWORD dwTimeout, HANDLE hBreakEvent /*= NULL*/) = 0;
	virtual HIPCCONNECTION Connect (char *pszServerName, DWORD dwTimeout) = 0;
	virtual BOOL CloseConnection (HIPCCONNECTION hConnection) = 0;
	virtual DWORD Recv (HIPCCONNECTION hConnection, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout) = 0;
	virtual DWORD Send( HIPCCONNECTION hConnection, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout ) = 0;
	virtual BOOL SetEvents( HIPCCONNECTION hConnection, HANDLE *pUserEvents, DWORD dwUserEventsCount, HANDLE *pIPCEvents ) = 0;
	virtual BOOL GetEvents( HIPCCONNECTION hConnection, DWORD *pdwUserEvents, DWORD *pdwIPCEvents ) = 0;
	virtual BOOL ResetEvents( HIPCCONNECTION hConnection ) = 0;
	virtual DWORD GetConnectionLastErr( HIPCCONNECTION hConnection ) = 0;
};

class CMemoryMappedIpc: public IIpc
{
public:
	virtual DWORD GetVersion()
	{ return IPC_Runtime::instance().getVersion(); }

	virtual HIPCSERVER ServerStart (const char *epName)
	{ return IPC_Runtime::instance().serverStart (epName); }

	virtual BOOL ServerStop (HIPCSERVER hServer)
	{ return IPC_Runtime::instance().serverStop (hServer); }

	virtual HIPCCONNECTION ServerWaitForConnection (HIPCSERVER hServer, DWORD dwTimeout, HANDLE hBreakEvent /*= NULL*/)
	{ return IPC_Runtime::instance().serverWaitForConnection (hServer, dwTimeout, hBreakEvent); }

	virtual HIPCCONNECTION Connect (char *pszServerName, DWORD dwTimeout)
	{ return IPC_Runtime::instance().connect (pszServerName, dwTimeout); }

	virtual BOOL CloseConnection (HIPCCONNECTION hConnection)
	{ return IPC_Runtime::instance().closeConnection (hConnection); }

	virtual DWORD Recv (HIPCCONNECTION hConnection, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout)
	{ return IPC_Runtime::instance().recv (hConnection, pvBuf, dwBufSize, dwTimeout); }

	virtual DWORD Send( HIPCCONNECTION hConnection, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout )
	{ return IPC_Runtime::instance().send (hConnection, pvBuf, dwBufSize, dwTimeout); }

	virtual BOOL SetEvents( HIPCCONNECTION hConnection, HANDLE *pUserEvents, DWORD dwUserEventsCount, HANDLE *pIPCEvents )
	{ return IPC_Runtime::instance().setUserEvent (hConnection, *pUserEvents); }

	virtual BOOL GetEvents( HIPCCONNECTION hConnection, DWORD *pdwUserEvents, DWORD *pdwIPCEvents )
	{ return IPC_Runtime::instance().getUserEvent (hConnection, (HANDLE *)pdwUserEvents); }

	virtual BOOL ResetEvents( HIPCCONNECTION hConnection )
	{ return IPC_Runtime::instance().resetUserEvent (hConnection); }

	virtual DWORD GetConnectionLastErr( HIPCCONNECTION hConnection )
	{ return IPC_Runtime::instance().getConnectionLastErr (hConnection); }
};

///////////////////////////////////////////////////////////////////////////////////////
// CTimeout
///////////////////////////////////////////////////////////////////////////////////////

//! Класс помогает следить за тайм-аутом.
class CTimeout
{
	DWORD m_dwDeadline;
	bool m_bInfintine;
public:
	CTimeout(
		DWORD dwTimeoutMs = INFINITE	//!< [in] Время в миллисекундах за которым надо следить. INFINITE - тайм-аут никогда не сработает.
		);
	//! Возвращает оставшееся время в миллисекундах или INFINITE если нет тайм-аута.
	DWORD GetTimeLeft() const;
	//! Признак наступления тайм-аута, true - тайм-аут наступил.
	operator bool() const;
};

CTimeout::CTimeout(DWORD dwTimeoutMs)
{
	m_dwDeadline = GetTickCount() + dwTimeoutMs;
	m_bInfintine = dwTimeoutMs==INFINITE;
}
DWORD CTimeout::GetTimeLeft() const
{
	if(m_bInfintine)
		return INFINITE;
	DWORD dwCur = GetTickCount();
	if(dwCur<=m_dwDeadline)
		return m_dwDeadline-dwCur;
	return 0;
}
CTimeout::operator bool() const
{
	return !m_bInfintine && GetTickCount()>m_dwDeadline;
}

struct CCritSec
{
	CCritSec() { InitializeCriticalSection(&m_cs); }
	~CCritSec() { DeleteCriticalSection(&m_cs); }
	CRITICAL_SECTION m_cs;
};

class CCSLock
{
	CCritSec& m_cs;
public:
	CCSLock(CCritSec& cs): m_cs(cs) { EnterCriticalSection(&m_cs.m_cs); }
	~CCSLock() { LeaveCriticalSection(&m_cs.m_cs); }
};

#define PIPE_READ_BUF_SIZE (1024*4)
#define PIPE_WAIT_TIMEOUT 60000
#define PIPE_PREFIX "\\\\.\\pipe\\klipc_"
#define EVENT_SR2R_PREFIX "Global\\klipc_sr2r"
#define EVENT_CR2R_PREFIX "Global\\klipc_cr2r"

class CPipeTransport
{
	HANDLE m_hPipe;
	HANDLE m_evStop;
	CCritSec m_cs;
	DWORD m_dwLastError;
	int m_nHandleCount;
	HANDLE* m_arrUserHandles;
	OVERLAPPED m_ovlRecv, m_ovlSend;
	HANDLE m_evImReadyToRcv;
	HANDLE m_evHeReadyToRcv;

	DWORD SetError(DWORD dwError)
	{
		m_dwLastError = dwError;
		return dwError == IPC_ERR_TIMEOUT ? IPC_RC_TIMEOUT : IPC_RC_ERROR;
	}

public:
	CPipeTransport()
		: m_hPipe(INVALID_HANDLE_VALUE)
		, m_dwLastError(0)
		, m_nHandleCount(0)
		, m_arrUserHandles(NULL)
		, m_evImReadyToRcv(NULL)
		, m_evHeReadyToRcv(NULL)
	{
		assert(_CrtIsValidHeapPointer(this));
		m_ovlRecv.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_ovlSend.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_evStop = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	CPipeTransport(HANDLE hPipe, HANDLE evImReadyToRcv, HANDLE evHeReadyToRcv)
		: m_hPipe(hPipe)
		, m_dwLastError(0)
		, m_nHandleCount(0)
		, m_arrUserHandles(NULL)
		, m_evImReadyToRcv(evImReadyToRcv)
		, m_evHeReadyToRcv(evHeReadyToRcv)
	{
		assert(_CrtIsValidHeapPointer(this));
		m_ovlRecv.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_ovlSend.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_evStop = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	~CPipeTransport()
	{
		assert(_CrtIsValidHeapPointer(this));

		SetEvent(m_evStop);

		CCSLock lock(m_cs);

		verify(CloseHandle(m_evStop));
		m_evStop = NULL;

		verify(CloseHandle(m_ovlRecv.hEvent));
		verify(CloseHandle(m_ovlSend.hEvent));

		if (m_hPipe != INVALID_HANDLE_VALUE)
		{
			verify(CloseHandle(m_hPipe));
			m_hPipe = INVALID_HANDLE_VALUE;
		}
		if (m_arrUserHandles)
		{
			delete[] m_arrUserHandles;
			m_nHandleCount = 0;
			m_arrUserHandles = NULL;
		}

		SetEvent(m_evImReadyToRcv); // чтобы другая сторона могла в Send определить, что мы отвалились

		if (m_evImReadyToRcv)
			verify(CloseHandle(m_evImReadyToRcv));
		if (m_evHeReadyToRcv)
			verify(CloseHandle(m_evHeReadyToRcv));
	}

	HIPCCONNECTION Connect(const char *pszServerName, DWORD dwTimeout)
	{
		assert(_CrtIsValidHeapPointer(this));
		CCSLock lock(m_cs);
		assert(_CrtIsValidHeapPointer(this));

		assert(m_hPipe == INVALID_HANDLE_VALUE && strlen(pszServerName) < 100);
		if (m_hPipe != INVALID_HANDLE_VALUE)
			return (HIPCCONNECTION) IPC_RC_INVALID_HANDLE;

		CTimeout timeout(dwTimeout);

		char pipeName[MAX_PATH] = PIPE_PREFIX;
		strcat(pipeName, pszServerName);
		
		for (;;)
		{
			if (WaitNamedPipe(pipeName, min(3000,timeout.GetTimeLeft())))
			{
				m_hPipe = CreateFile(
					pipeName,
					GENERIC_READ|GENERIC_WRITE,
					0,
					NULL,
					OPEN_EXISTING,
					FILE_FLAG_OVERLAPPED,
					NULL);
				if (m_hPipe != INVALID_HANDLE_VALUE)
				{
					DWORD dwMode = PIPE_READMODE_MESSAGE;
					if (SetNamedPipeHandleState(m_hPipe, &dwMode, NULL, NULL))
						break;
					verify(CloseHandle(m_hPipe));
					m_hPipe = INVALID_HANDLE_VALUE;
					return (HIPCCONNECTION) IPC_RC_INVALID_HANDLE; // Невозможно установить режим PIPE_READMODE_MESSAGE
				}
				if (GetLastError() != ERROR_PIPE_BUSY)
					return (HIPCCONNECTION) IPC_RC_INVALID_HANDLE; // Невозможно установить соединение
			}

			HANDLE* ev = (HANDLE*) _alloca(sizeof(HANDLE) * (m_nHandleCount + 1));
			ev[0] = m_evStop;
			memcpy(ev + 1, m_arrUserHandles, sizeof(HANDLE) * m_nHandleCount);
			if (WaitForMultipleObjects(m_nHandleCount + 1, ev, FALSE, min(200,timeout.GetTimeLeft())) != WAIT_TIMEOUT
				|| timeout)
				return (HIPCCONNECTION) IPC_RC_TIMEOUT;
		}
		
		int nCounter = 0;

		OVERLAPPED ovl = { 0, };
		ovl.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		DWORD dwRead = 0;
		ReadFile(m_hPipe, &nCounter, sizeof(nCounter), &dwRead, &ovl);
		WaitForSingleObject(ovl.hEvent, INFINITE);
		CloseHandle(ovl.hEvent);

		char szEvName[MAX_PATH];
		sprintf(szEvName, "%s_%s_%i", EVENT_SR2R_PREFIX, pszServerName, nCounter);
		m_evHeReadyToRcv = OpenEvent(EVENT_ALL_ACCESS, FALSE, szEvName);
		sprintf(szEvName, "%s_%s_%i", EVENT_CR2R_PREFIX, pszServerName, nCounter);
		m_evImReadyToRcv = OpenEvent(EVENT_ALL_ACCESS, FALSE, szEvName);

		assert(m_evHeReadyToRcv && m_evImReadyToRcv);

		return (HIPCCONNECTION) this;
	}

	DWORD Recv(void *pvBuf, DWORD dwBufSize, DWORD dwTimeout)
	{
		assert(_CrtIsValidHeapPointer(this));
		CCSLock lock(m_cs);
		assert(_CrtIsValidHeapPointer(this));

		assert(m_hPipe != INVALID_HANDLE_VALUE);
		if (m_hPipe == INVALID_HANDLE_VALUE)
			return SetError(IPC_ERR_BROKEN); // Транспорт не открыт или был закрыт

		CTimeout timeout(dwTimeout);

		SetEvent(m_evImReadyToRcv);

		DWORD dwTotalReaded = 0;

		bool bMore;
		do
		{
			m_ovlRecv.Offset = m_ovlRecv.OffsetHigh = 0;
			DWORD dwBytesReaded;
			BYTE buf[PIPE_READ_BUF_SIZE];
			if (!ReadFile(m_hPipe, buf, PIPE_READ_BUF_SIZE, &dwBytesReaded, &m_ovlRecv))
			{
				DWORD dwErr = GetLastError();
				if (dwErr != ERROR_MORE_DATA && dwErr != ERROR_IO_PENDING)
				{
					ResetEvent(m_evImReadyToRcv);
					CloseHandle(m_hPipe);
					m_hPipe = INVALID_HANDLE_VALUE;
					return SetError(dwErr == ERROR_BROKEN_PIPE ? IPC_ERR_CLOSED : IPC_ERR_UNKNOWN /* Ошибка чтения */); 
				}
			}

TryAgain:
			bMore = false;
			if (!GetOverlappedResult(m_hPipe, &m_ovlRecv, &dwBytesReaded, FALSE))
			{
				DWORD dwErr = GetLastError();
				switch(dwErr)
				{
				case ERROR_IO_INCOMPLETE:
					{
						if (timeout)
						{
							ResetEvent(m_evImReadyToRcv);
							return SetError(IPC_ERR_TIMEOUT);
						}
						HANDLE* ev = (HANDLE*) _alloca(sizeof(HANDLE) * (m_nHandleCount + 2));
						ev[0] = m_ovlRecv.hEvent;
						ev[1] = m_evStop;
						memcpy(ev + 2, m_arrUserHandles, sizeof(HANDLE) * m_nHandleCount);
						switch (WaitForMultipleObjects(m_nHandleCount + 2, ev, FALSE, timeout.GetTimeLeft()))
						{
						case WAIT_OBJECT_0: break;
						case WAIT_OBJECT_0 + 1:
							ResetEvent(m_evImReadyToRcv);
							CancelIo(m_hPipe);
							return SetError(IPC_ERR_UNKNOWN);
						case WAIT_TIMEOUT:
							ResetEvent(m_evImReadyToRcv);
							CancelIo(m_hPipe);
							return SetError(IPC_ERR_TIMEOUT);
						default:
							ResetEvent(m_evImReadyToRcv);
							CancelIo(m_hPipe);
							return SetError(IPC_ERR_USER_EVENT_SET);
						}

						bMore = true;
						goto TryAgain;
					}
				case ERROR_MORE_DATA:
					bMore = true;
					break;
				default:
					ResetEvent(m_evImReadyToRcv);
					CloseHandle(m_hPipe);
					m_hPipe = INVALID_HANDLE_VALUE;
					if (dwErr == ERROR_BROKEN_PIPE)
						return SetError(IPC_ERR_CLOSED); // Соединение было закрыто
					return SetError(IPC_ERR_UNKNOWN); // Невозможно получить сообщение
				}
			}

			assert(dwBufSize >= dwBytesReaded);
			DWORD dwLen = min(dwBufSize, dwBytesReaded);
			memcpy(pvBuf, buf, dwLen);
			reinterpret_cast<BYTE*&>(pvBuf) += dwLen;
			dwBufSize -= dwLen;
			dwTotalReaded += dwLen;
		} while(bMore);

		ResetEvent(m_evImReadyToRcv);

		return dwTotalReaded;
	}

	DWORD Send(void *pvBuf, DWORD dwBufSize, DWORD dwTimeout)
	{
		assert(_CrtIsValidHeapPointer(this));
		CCSLock lock(m_cs);
		assert(_CrtIsValidHeapPointer(this));

		assert(m_hPipe != INVALID_HANDLE_VALUE);
		if (m_hPipe == INVALID_HANDLE_VALUE)
			return SetError(IPC_ERR_BROKEN); // Транспорт не открыт или был закрыт

		HANDLE* ev = (HANDLE*) _alloca(sizeof(HANDLE) * (m_nHandleCount + 2));
		ev[0] = m_evHeReadyToRcv;
		ev[1] = m_evStop;
		memcpy(ev + 2, m_arrUserHandles, sizeof(HANDLE) * m_nHandleCount);
		switch (WaitForMultipleObjects(m_nHandleCount + 2, ev, FALSE, dwTimeout))
		{
		case WAIT_OBJECT_0: break;
		case WAIT_OBJECT_0 + 1: return SetError(IPC_ERR_UNKNOWN);
		case WAIT_TIMEOUT: return SetError(IPC_ERR_TIMEOUT);
		default: return SetError(IPC_ERR_USER_EVENT_SET);
		}

		DWORD dwWritten = 0;
		m_ovlSend.Offset = m_ovlSend.OffsetHigh = 0;
		if (WriteFile(m_hPipe, pvBuf, dwBufSize, &dwWritten, &m_ovlSend))
			return dwWritten;
		if (GetLastError() != ERROR_IO_PENDING)
		{
			CloseHandle(m_hPipe);
			m_hPipe = INVALID_HANDLE_VALUE;
			if (GetLastError() == ERROR_BROKEN_PIPE)
				return SetError(IPC_ERR_CLOSED);
			return SetError(IPC_ERR_UNKNOWN); // Невозможно отослать сообщение
		}

		ev[0] = m_ovlSend.hEvent;
		switch (WaitForMultipleObjects(m_nHandleCount + 2, ev, FALSE, dwTimeout))
		{
		case WAIT_OBJECT_0:
			return dwBufSize;
		case WAIT_OBJECT_0 + 1:
			CancelIo(m_hPipe);
			return SetError(IPC_ERR_UNKNOWN);
		case WAIT_TIMEOUT:
			CancelIo(m_hPipe);
			return SetError(IPC_ERR_TIMEOUT);
		default:
			CancelIo(m_hPipe);
			return SetError(IPC_ERR_USER_EVENT_SET);
		}
	}

	BOOL SetEvents(HANDLE *pUserEvents, DWORD dwUserEventsCount, HANDLE *pIPCEvents )
	{
		assert(_CrtIsValidHeapPointer(this));
		CCSLock lock(m_cs);
		assert(_CrtIsValidHeapPointer(this));

		assert(dwUserEventsCount == 1 && pIPCEvents == NULL); // старая реализация реально поддерживала только 1
		if (m_arrUserHandles)
			delete[] m_arrUserHandles;
		m_arrUserHandles = new HANDLE[dwUserEventsCount];
		memcpy(m_arrUserHandles, pUserEvents, sizeof(HANDLE) * dwUserEventsCount);
		m_nHandleCount = dwUserEventsCount;
		return TRUE;
	}

	BOOL GetEvents(DWORD *pdwUserEvents, DWORD *pdwIPCEvents )
	{
		assert(_CrtIsValidHeapPointer(this));
		CCSLock lock(m_cs);
		assert(_CrtIsValidHeapPointer(this));

		assert(false); // чего вообще эта функция хочет?
		return FALSE;
	}

	void ResetEvents()
	{
		assert(_CrtIsValidHeapPointer(this));
		CCSLock lock(m_cs);
		assert(_CrtIsValidHeapPointer(this));

		if (m_arrUserHandles)
		{
			delete[] m_arrUserHandles;
			m_arrUserHandles = NULL;
		}
		m_nHandleCount = 0;
	}

	DWORD GetConnectionLastErr()
	{
		assert(_CrtIsValidHeapPointer(this));
		CCSLock lock(m_cs);
		assert(_CrtIsValidHeapPointer(this));
		return m_dwLastError;
	}
};

class CPipeServer
{
	HANDLE m_hPipe;
	HANDLE m_evStop;
	CCritSec m_cs;
	OVERLAPPED m_ovl;
	char* m_pszName;
	char* m_pszPipeName;
	HSA m_hSA;
	int m_nCounter;
public:
	CPipeServer(const char *epName)
		: m_hPipe(INVALID_HANDLE_VALUE)
		, m_pszName(NULL)
		, m_pszPipeName(NULL)
		, m_nCounter(0)
	{
		assert(_CrtIsValidHeapPointer(this));
		m_evStop = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_ovl.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_pszName = new char[strlen(epName) + 1];
		strcpy(m_pszName, epName);
		m_pszPipeName = new char[strlen(epName) + 1 + sizeof(PIPE_PREFIX)];
		strcpy(m_pszPipeName, PIPE_PREFIX);
		strcat(m_pszPipeName, epName);

		m_hSA = SA_Create(SECURITY_LOCAL_RID /*SA_AUTHORITY_EVERYONE*/, 
			SA_ACCESS_MASK_ALL);
	}

	~CPipeServer()
	{
		assert(_CrtIsValidHeapPointer(this));

		SetEvent(m_evStop);

		CCSLock lock(m_cs);

		CloseHandle(m_evStop);
		m_evStop = NULL;

		delete[] m_pszPipeName;
		m_pszPipeName = NULL;
		delete[] m_pszName;
		m_pszName = NULL;
		CloseHandle(m_ovl.hEvent);
		m_ovl.hEvent = NULL;
		if (m_hPipe != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hPipe);
			m_hPipe = INVALID_HANDLE_VALUE;
		}

		SA_Destroy(m_hSA);
	}

	bool Create()
	{
		assert(_CrtIsValidHeapPointer(this));
		CCSLock lock(m_cs);
		assert(_CrtIsValidHeapPointer(this));

		m_hPipe = CreateNamedPipe(
			m_pszPipeName,
			PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED,
			PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			PIPE_READ_BUF_SIZE,
			PIPE_READ_BUF_SIZE,
			PIPE_WAIT_TIMEOUT,
			SA_Get(m_hSA));

		return m_hPipe != INVALID_HANDLE_VALUE;
	}

	HIPCCONNECTION ServerWaitForConnection(DWORD dwTimeout, HANDLE hBreakEvent)
	{
		assert(_CrtIsValidHeapPointer(this));
		CCSLock lock(m_cs);
		assert(_CrtIsValidHeapPointer(this));

		if (!ConnectNamedPipe(m_hPipe, &m_ovl))
		{
			switch(GetLastError())
			{
			case ERROR_PIPE_CONNECTED:
				break;
			case ERROR_IO_PENDING:
				{
					HANDLE hEvnts[] = { m_ovl.hEvent, m_evStop, hBreakEvent };
					switch (WaitForMultipleObjects(hBreakEvent?3:2, hEvnts, FALSE, dwTimeout))
					{
					case WAIT_OBJECT_0:
						break;
					case WAIT_OBJECT_0 + 1:
						return (HIPCCONNECTION) IPC_RC_INVALID_HANDLE; // destructing
					case WAIT_OBJECT_0 + 2:
						return (HIPCCONNECTION) IPC_RC_TIMEOUT; // user event
					case WAIT_TIMEOUT:
						return (HIPCCONNECTION) IPC_RC_TIMEOUT; // timeout
					}
					break;
				}
			default:
				return (HIPCCONNECTION) IPC_RC_INVALID_HANDLE;
			}
		}

		char szEvName[MAX_PATH];
		sprintf(szEvName, "%s_%s_%i", EVENT_SR2R_PREFIX, m_pszName, m_nCounter);
		HANDLE evSR2R = CreateEvent(SA_Get(m_hSA), FALSE, FALSE, szEvName);
		sprintf(szEvName, "%s_%s_%i", EVENT_CR2R_PREFIX, m_pszName, m_nCounter);
		HANDLE evCR2R = CreateEvent(SA_Get(m_hSA), FALSE, FALSE, szEvName);

		OVERLAPPED ovl = { 0, };
		ovl.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		DWORD dwWritten = 0;
		WriteFile(m_hPipe, &m_nCounter, sizeof(m_nCounter), &dwWritten, &ovl);
		WaitForSingleObject(ovl.hEvent, INFINITE);
		CloseHandle(ovl.hEvent);

		++m_nCounter;

		CPipeTransport* pipe = new CPipeTransport(m_hPipe, evSR2R, evCR2R);
		m_hPipe = INVALID_HANDLE_VALUE;
		Create();
		return (HIPCCONNECTION) pipe;
	}
};

class CPipeIpc: public IIpc
{
public:
	virtual DWORD GetVersion()
	{
		return (MODULE_VERSION_A << 24)
			| (MODULE_VERSION_B << 16)
			| (MODULE_VERSION_C << 8)
			| MODULE_VERSION_D;
	}

	virtual HIPCSERVER ServerStart (const char *epName)
	{
		CPipeServer* server = new CPipeServer(epName);
		if (server->Create())
			return (HIPCSERVER) server;
		delete server;
		return (HIPCSERVER) IPC_ERR_UNKNOWN;
	}

	virtual BOOL ServerStop (HIPCSERVER hServer)
	{
		assert(hServer);
		if (!hServer)
			return FALSE;
		delete static_cast<CPipeServer*>(hServer);
		return TRUE;
	}

	virtual HIPCCONNECTION ServerWaitForConnection (HIPCSERVER hServer, DWORD dwTimeout, HANDLE hBreakEvent /*= NULL*/)
	{
		assert(hServer);
		if (!hServer)
			return (HIPCCONNECTION) IPC_RC_ERROR;
		return static_cast<CPipeServer*>(hServer)->ServerWaitForConnection(dwTimeout, hBreakEvent);
	}

	virtual HIPCCONNECTION Connect (char *pszServerName, DWORD dwTimeout)
	{
		CPipeTransport* pipe = new CPipeTransport();
		HIPCCONNECTION res = pipe->Connect(pszServerName, dwTimeout);
		if (static_cast<CPipeTransport*>(res) != pipe)
			delete pipe;
		return res;
	}

	virtual BOOL CloseConnection (HIPCCONNECTION hConnection)
	{
		assert(hConnection);
		if (!hConnection)
			return FALSE;
		delete static_cast<CPipeTransport*>(hConnection);
		return TRUE;
	}

	virtual DWORD Recv (HIPCCONNECTION hConnection, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout)
	{
		assert(hConnection);
		if (!hConnection)
			return IPC_RC_ERROR;
		return static_cast<CPipeTransport*>(hConnection)->Recv(pvBuf, dwBufSize, dwTimeout);
	}

	virtual DWORD Send( HIPCCONNECTION hConnection, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout )
	{
		assert(hConnection);
		if (!hConnection)
			return IPC_RC_ERROR;
		return static_cast<CPipeTransport*>(hConnection)->Send(pvBuf, dwBufSize, dwTimeout);
	}

	virtual BOOL SetEvents( HIPCCONNECTION hConnection, HANDLE *pUserEvents, DWORD dwUserEventsCount, HANDLE *pIPCEvents )
	{
		assert(hConnection);
		if (!hConnection)
			return FALSE;
		return static_cast<CPipeTransport*>(hConnection)->SetEvents(pUserEvents, dwUserEventsCount, pIPCEvents);
	}

	virtual BOOL GetEvents( HIPCCONNECTION hConnection, DWORD *pdwUserEvents, DWORD *pdwIPCEvents )
	{
		assert(hConnection);
		if (!hConnection)
			return FALSE;
		return static_cast<CPipeTransport*>(hConnection)->GetEvents(pdwUserEvents, pdwIPCEvents);
	}

	virtual BOOL ResetEvents( HIPCCONNECTION hConnection )
	{
		assert(hConnection);
		if (!hConnection)
			return FALSE;
		static_cast<CPipeTransport*>(hConnection)->ResetEvents();
		return TRUE;
	}

	virtual DWORD GetConnectionLastErr( HIPCCONNECTION hConnection )
	{
		assert(hConnection);
		if (!hConnection)
			return IPC_ERR_UNKNOWN;
		return static_cast<CPipeTransport*>(hConnection)->GetConnectionLastErr();
	}
};

//////////////////////////////////////////////////////////////////////////////
static IIpc* g_pIpc = NULL;

BOOL WINAPI DllMain( HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved )
{
	if (fdwReason != DLL_PROCESS_ATTACH)
		return TRUE;

	if (!DisableThreadLibraryCalls(hinstDll))
		return FALSE;

	OSVERSIONINFO ver;
	ver.dwOSVersionInfoSize = sizeof(ver);
	GetVersionEx(&ver);
	if (ver.dwPlatformId  == VER_PLATFORM_WIN32_NT && ver.dwMajorVersion > 4)
	{
		static CPipeIpc pipeipc;
		g_pIpc = &pipeipc;
	}
	else
	{
		if (!IPC_Runtime::instance().isValid())
			return FALSE;
		static CMemoryMappedIpc mmipc;
		g_pIpc = &mmipc;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////
// IPC.DLL entry points

IPC_API DWORD __stdcall IPC_GetVersion()
{ return g_pIpc->GetVersion(); }

IPC_API HIPCSERVER __stdcall IPC_ServerStart (const char *epName)
{ return g_pIpc->ServerStart(epName); }

IPC_API BOOL __stdcall IPC_ServerStop (HIPCSERVER hServer)
{ return g_pIpc->ServerStop(hServer); }

IPC_API HIPCCONNECTION __stdcall IPC_ServerWaitForConnection (HIPCSERVER hServer, DWORD dwTimeout, HANDLE hBreakEvent /*= NULL*/)
{ return g_pIpc->ServerWaitForConnection(hServer, dwTimeout, hBreakEvent); }

IPC_API HIPCCONNECTION __stdcall IPC_Connect (char *pszServerName, DWORD dwTimeout)
{ return g_pIpc->Connect(pszServerName, dwTimeout); }

IPC_API BOOL __stdcall IPC_CloseConnection (HIPCCONNECTION hConnection)
{ return g_pIpc->CloseConnection(hConnection); }

IPC_API DWORD __stdcall IPC_Recv (HIPCCONNECTION hConnection, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout)
{ return g_pIpc->Recv(hConnection, pvBuf, dwBufSize, dwTimeout); }

IPC_API DWORD __stdcall IPC_Send( HIPCCONNECTION hConnection, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout )
{ return g_pIpc->Send(hConnection, pvBuf, dwBufSize, dwTimeout); }

IPC_API BOOL __stdcall IPC_SetEvents( HIPCCONNECTION hConnection, HANDLE *pUserEvents, DWORD dwUserEventsCount, HANDLE *pIPCEvents )
{ return g_pIpc->SetEvents(hConnection, pUserEvents, dwUserEventsCount, pIPCEvents ); }

IPC_API BOOL __stdcall IPC_GetEvents( HIPCCONNECTION hConnection, DWORD *pdwUserEvents, DWORD *pdwIPCEvents )
{ return g_pIpc->GetEvents(hConnection, pdwUserEvents, pdwIPCEvents); }

IPC_API BOOL __stdcall IPC_ResetEvents( HIPCCONNECTION hConnection )
{ return g_pIpc->ResetEvents(hConnection); }

IPC_API DWORD __stdcall IPC_GetConnectionLastErr( HIPCCONNECTION hConnection )
{ return g_pIpc->GetConnectionLastErr(hConnection); }

////////////////////////////////////////////////////////////////
// not implemented

IPC_API HIPCSERVER __stdcall IPC_ServerDgStart (char *pszDgServerName)
{ return ( HIPCSERVER )IPC_RC_ERROR; }

IPC_API BOOL __stdcall IPC_ServerDgStop (HIPCSERVER hServer)
{ return FALSE; }

IPC_API DWORD __stdcall IPC_DgRecv (char *pszDgServerName, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout)
{ return IPC_RC_ERROR; }

IPC_API DWORD __stdcall IPC_DgSend (char *pszDgServerName, void *pvBuf, DWORD dwBufSize, DWORD dwTimeout)
{ return IPC_RC_ERROR; }

IPC_API void __stdcall IPC_Init ()
{}

IPC_API void __stdcall IPC_Done ()
{}

