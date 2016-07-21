// CriticalSection.h: interface for the CCriticalSection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRITICALSECTION_H__BB1B72C3_8FAA_4A04_B496_276141251479__INCLUDED_)
#define AFX_CRITICALSECTION_H__BB1B72C3_8FAA_4A04_B496_276141251479__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCriticalSection  
{
public:
	CCriticalSection();
	virtual ~CCriticalSection();

	void Enter();
	void Leave();

private:
	CRITICAL_SECTION m_cs;
	CCriticalSection(const CCriticalSection& rhs){}

	BOOL m_bIsConstructed;
};

class CAutoCriticalSection
{
public:
	CAutoCriticalSection(CCriticalSection &cs) : m_cs(cs)
	{
		m_cs.Enter();
	}

	~CAutoCriticalSection()
	{
		m_cs.Leave();
	}

private:
	CAutoCriticalSection(CAutoCriticalSection&);
	CAutoCriticalSection& operator=(CAutoCriticalSection&);

	CCriticalSection &m_cs;
};

#endif // !defined(AFX_CRITICALSECTION_H__BB1B72C3_8FAA_4A04_B496_276141251479__INCLUDED_)
