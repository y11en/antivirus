// GuardClient.h: interface for the CGuardClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined (AFX_GUARDCLIENT_H__BA8467D4_16C9_4498_8AC9_5D920D064C9F__INCLUDED_)
#define AFX_GUARDCLIENT_H__BA8467D4_16C9_4498_8AC9_5D920D064C9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <Hook/avpgcom.h>
#include <Hook/Hookspec.h>
#include <Hook/fssync.h>
#include "GuardFilter.h"
#include "FltList.h"

const int Verdict_WaitVerdictFromTestEvent = 0x01000000;

typedef std::map <ULONG, VERDICT>	PendingEventMap;

struct PendingEventSupport
{
	HANDLE m_hEventReply;
	CRITICAL_SECTION m_csListSync;
	PendingEventMap m_EventMap;
};

class HANDLE_EX
{
public:
	HANDLE_EX ()
	{
		hHandle = NULL;
		hInitEvent = ::CreateEvent (NULL, TRUE, FALSE, NULL);
		assert (hInitEvent);

		hStopEvent = ::CreateEvent (NULL, TRUE, FALSE, NULL);
		assert (hStopEvent);

	}
	virtual ~HANDLE_EX ()
	{
		::CloseHandle (hInitEvent);
		::CloseHandle (hStopEvent);
	}

	const HANDLE_EX& operator= (HANDLE_EX& handle)
	{
		hHandle = handle.hHandle;

		if (hHandle != NULL && hHandle != INVALID_HANDLE_VALUE)
		{
			::SetEvent (hInitEvent);
		}
		else
		{
			::SetEvent (hStopEvent);
		}

		return *this;
	}

	const HANDLE_EX& operator= (HANDLE handle)
	{
		hHandle = handle;

		if (hHandle != NULL && hHandle != INVALID_HANDLE_VALUE)
		{
			::SetEvent (hInitEvent);
		}
		else
		{
			::SetEvent (hStopEvent);
		}
		
		return *this;
	}

	operator HANDLE () const
	{
		return hHandle;
	}

	HANDLE GetInitializeEvent ()
	{
		return hInitEvent;
	}

	HANDLE GetStopEvent ()
	{
		return hStopEvent;
	}


protected:
	HANDLE hHandle;
	HANDLE hInitEvent;
	HANDLE hStopEvent;

};

struct EventQueueElem
{
	PVOID m_pMessage;
};

class CGuardClient
{
public:
	CKAHCOMMEXPORT CGuardClient ();
	CKAHCOMMEXPORT virtual ~CGuardClient ();

	enum eExchangeType
	{
		IN_BUF_VALID = 1,
		OUT_BUF_VALID = 2
	};
	
	CKAHCOMMEXPORT BOOL Create (HANDLE hShutDownEvent, ULONG uAppID, ULONG uPriority, DWORD dwClientFlags = _CLIENT_FLAG_WITHOUTWATCHDOG | _CLIENT_FLAG_WITHOUTCASCADE, BOOL bState = TRUE);
	CKAHCOMMEXPORT virtual ULONG AddFilter (const CGuardFilter &filter, IN OUT DWORD &dwOrderID); // если dwOrderID == -1, то dwOrderID вычисляется
	CKAHCOMMEXPORT ULONG AddFilterAfter (const ULONG uFltID, const CGuardFilter& filter);
	CKAHCOMMEXPORT virtual ULONG AddFilter (DWORD HookID, DWORD FunctionMj, DWORD FunctionMi, DWORD dwFlags, PFILTER_PARAM pFirstParam, ...);
	CKAHCOMMEXPORT void StopClient (BOOL bResident);

	CKAHCOMMEXPORT BOOL IsCreated ();
	
	//Methods
public:
	CKAHCOMMEXPORT BOOL GetFilter (const DWORD dwFilterID, CGuardFilter & filter);
	CKAHCOMMEXPORT BOOL GetAllUserFilters (FLTLIST & flt_list, bool bForceAll = false);
	CKAHCOMMEXPORT BOOL RemoveAllUserFilters (bool bForceAll = false);
	CKAHCOMMEXPORT BOOL AddFilterList (const FLTLIST & flt_list);
	CKAHCOMMEXPORT BOOL RemoveBlockFilters ();
	CKAHCOMMEXPORT BOOL Disable ();
	CKAHCOMMEXPORT BOOL Enable ();
	CKAHCOMMEXPORT ULONG AddFilter2Top (CGuardFilter& filter);
//	CKAHCOMMEXPORT BOOL DriverExchange (IN DWORD driverID, IN DWORD ioctl, IN OUT void* pBuf, IN OUT DWORD* dwBufSize, IN BOOL IsInBufferValid = FALSE);
	CKAHCOMMEXPORT BOOL IsFiltersPresent ();
	CKAHCOMMEXPORT virtual VERDICT OnEvent (PVOID pMessage)
	{
		return Verdict_Pass;
	}

	CKAHCOMMEXPORT VOID DrvMemFree( void** ptr);

	CKAHCOMMEXPORT void RemoveElemsFromEventQueue (UUID *pUuid, VERDICT VerdictOfRemovedElems);

	CKAHCOMMEXPORT BOOL RemoveFiltersWithParam (ULONG nParam, bool bForceAll = false);
	CKAHCOMMEXPORT void GetFiltersWithParam (ULONG nParam, FILTER_ID_LST &filters, bool bForceAll = false);
	
	CKAHCOMMEXPORT BOOL ChangeFilterParam(DWORD dwFilterID,
											ULONG nParam,
											ULONG dwParamFlags,
											FLT_PARAM_OPERATION ParamFlt,
											ULONG uParamSize,
											LPCVOID pParamValue);

	
protected:
	virtual void OnStopClient (BOOL bResidend)
	{
	}

public:
	CKAHCOMMEXPORT BOOL	RemoveAllFilters ();
	CKAHCOMMEXPORT PVOID GetDriverContext ();
	CKAHCOMMEXPORT BOOL SaveFilters ();
	CKAHCOMMEXPORT DWORD GetLastFilterOrder ();
	CKAHCOMMEXPORT BOOL RemoveFilter (ULONG uFilterID);
	CKAHCOMMEXPORT BOOL RemoveUserFilter (const UUID & uidUsrID);

protected:
	
	HANDLE				m_hThread;
	HANDLE				m_hEvQueueThread;
	PVOID				m_DriverContext;
	
	DWORD				m_dwAppID;

	BOOL				m_bInitialState;
	HANDLE				m_hShutDownEvent;
	BOOL				m_bResident;

	PendingEventSupport m_PendingSupport;
	
	HANDLE				m_hEventQueueSemaphore;
	HANDLE				m_hStopEvQueueDispatcher;
	CRITICAL_SECTION	m_hcsEventQueue;
	std::list<EventQueueElem> m_EventQueue;

private:
	static DWORD WINAPI MainProc (LPVOID);
	static DWORD WINAPI EventQueueDispatcher (LPVOID);
};

#endif // !defined (AFX_GUARDCLIENT_H__BA8467D4_16C9_4498_8AC9_5D920D064C9F__INCLUDED_)
