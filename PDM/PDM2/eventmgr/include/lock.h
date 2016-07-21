#if !defined(AFX_lock_H__AE7059D3_5964_4A19_B268_107390FA0F7F__INCLUDED_)
#define AFX_lock_H__AE7059D3_5964_4A19_B268_107390FA0F7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class cLock
{
public:
	cLock();
	~cLock();
	
	void lock(bool exclusive = true);
	void unlock();

private: 
	void* m_pLock;
};

class cAutoLock
{
public:
	cAutoLock(cLock& lock, bool exclusive = true) : m_Lock(lock)
	{
		m_Lock = lock;
		m_Lock.lock(exclusive);
	}
	~cAutoLock()
	{
		m_Lock.unlock();
	}
private:
	cLock& m_Lock;
};


#endif // !defined(AFX_lock_H__AE7059D3_5964_4A19_B268_107390FA0F7F__INCLUDED_)
