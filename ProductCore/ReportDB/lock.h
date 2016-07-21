#ifndef __RDB_LOCK_
#define __RDB_LOCK_

class cLock {
public:
	cLock();
	~cLock();
	void Enter(bool bExclusive = true);
	void Leave();

private:
	void* m_pData;
};

class cAutoLock
{
public:
	cAutoLock(cLock* pLock, bool bExclusive = true) : m_pLock(pLock)
	{
		m_pLock->Enter(bExclusive);
	}
	~cAutoLock()
	{
		m_pLock->Leave();
	}
private:
	cLock* m_pLock;
};


#endif // __RDB_LOCK_
