// NOOPThread.h: interface for the CNOOPThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOOPTHREAD_H__5D22DBB4_D8AB_44D8_AF9E_066FE54670FD__INCLUDED_)
#define AFX_NOOPTHREAD_H__5D22DBB4_D8AB_44D8_AF9E_066FE54670FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include "../../TrafficProtocoller.h"
using namespace TrafficProtocoller;
#include "../../ThreadImp.h"
#include <Prague/pr_cpp.h>
#include <Prague/pr_wrappers.h>

class CNOOPThread : public CThreadImp
{
public:
	CNOOPThread();
	virtual ~CNOOPThread();

	// параметры инициализации
	struct InitParams_t
	{
		InitParams_t() :
			hParent(0),
			data(0),
			size(0),
			dsTo(dsUnknown),
			dwTimeout(10000)
		{
		}
			
		hOBJECT hParent;
		session_t* pSession; // Сессия, в рамках которой осуществляется публикация
		tBYTE* data; // Данные для публикации
		tDWORD size; // Размер данных
		data_target_t dsTo; // Направление публикации
		tDWORD dwTimeout; // Таймаут публикации
	};

	tERROR Init(InitParams_t* pInitParams);
	virtual tERROR _Run();
	tERROR Start(session_t* pSession, tCHAR* data, tDWORD size, data_target_t dsTo, tDWORD dwTimeout, hOBJECT hParent);

public:
	tBOOL IsStarted()
	{
		return m_bStarted;
	}
	tBOOL IsWaitingForResponse()
	{
		return m_dwWaitingForResponse > 0;
	}
	void  FlushResponse();
private:
	volatile long m_bStarted;
	volatile long m_dwWaitingForResponse;
protected:
	InitParams_t m_InitParams;
};

#endif // !defined(AFX_NOOPTHREAD_H__5D22DBB4_D8AB_44D8_AF9E_066FE54670FD__INCLUDED_)
