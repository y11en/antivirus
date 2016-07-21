#ifndef __STR_HISTORY
#define __STR_HISTORY

#include "stdafx.h"
#include <vector>

//+ ------------------------------------------------------------------------------------------
#define _MAX_HISTORY_LENGTH	256

typedef std::vector<CString> _StrHistory;

class CDrvHistory
{
public:
	CDrvHistory()
	{
		m_EventsCount = 0;
	};
	virtual ~CDrvHistory(){};
	
	void push_back(CString& str)
	{
		if (!m_History.empty())
		{
			_StrHistory::iterator it = m_History.end() - 1;
			if (*it == str)
			{
				return;
			}
		}

		m_History.push_back(str);
		m_EventsCount++;
		
		if (m_History.size() > _MAX_HISTORY_LENGTH)
			m_History.erase(m_History.begin());
	}

	void clearall()
	{
		m_EventsCount = 0;
		m_History.erase(m_History.begin(), m_History.end());
	}
	
	_StrHistory m_History;
	ULONG m_EventsCount;
};

//+ ------------------------------------------------------------------------------------------

class CStrHistory : public COwnSync
{
public:
	CStrHistory();
	~CStrHistory();

	void PushEvent(CString* pstrMsg);
	bool PopEvent(CString* pstrMsg);

	unsigned long GetEventCount();
private:
	_StrHistory m_History;
};

#endif //__STR_HISTORY