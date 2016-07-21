#if !defined(__KL_COMMONFILES_UTILS_CR_H)
#define __KL_COMMONFILES_UTILS_CR_H

//////////////////////////////////////////////////////////////////////////
class CCR
{
public:
	CCR()			{ ::InitializeCriticalSection(&m_cr); }
	~CCR()			{ ::DeleteCriticalSection(&m_cr); }
	void Lock()		{ ::EnterCriticalSection(&m_cr); }
	void Unlock()	{ ::LeaveCriticalSection(&m_cr); }
private:
	CRITICAL_SECTION m_cr;
};

//////////////////////////////////////////////////////////////////////////
class CAutoCR
{
public:
	CAutoCR(CCR *pcr) 
		: m_pcr(pcr) 
	{
		if (m_pcr) 
			m_pcr->Lock();
	}
	CAutoCR(CCR& cr) 
		: m_pcr(&cr) 
	{
		if (m_pcr) 
			m_pcr->Lock();
	}
	~CAutoCR() 
	{
		if (m_pcr) 
			m_pcr->Unlock();
	}
private:
	CCR* m_pcr;
};

#endif  // __KL_COMMONFILES_UTILS_CR_H
