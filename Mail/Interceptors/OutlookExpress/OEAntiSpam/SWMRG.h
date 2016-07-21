/******************************************************************************
Module:  SWMRG.h
Notices: Copyright (c) 2000 Jeffrey Richter
******************************************************************************/


#pragma once


///////////////////////////////////////////////////////////////////////////////

class CSWMRG {
public:
   CSWMRG();                 // Constructor
   ~CSWMRG();                // Destructor

   VOID WaitToRead();        // Call this to gain shared read access
   VOID WaitToWrite();       // Call this to gain exclusive write access
   VOID Done();              // Call this when done accessing the resource

private:
   CRITICAL_SECTION m_cs;    // Permits exclusive access to other members
   HANDLE m_hsemReaders;     // Readers wait on this if a writer has access
   HANDLE m_hsemWriters;     // Writers wait on this if a reader has access
   int    m_nWaitingReaders; // Number of readers waiting for access
   int    m_nWaitingWriters; // Number of writers waiting for access
   int    m_nActive;         // Number of threads currently with access
                             //   (0=no threads, >0=# of readers, -1=1 writer)
};

class CAutoLockRead
{
public:
	CAutoLockRead(const CSWMRG &locker) 
	{
		m_pLocker = const_cast<CSWMRG*>(&locker);
		m_pLocker->WaitToRead();
	};

	~CAutoLockRead() 
	{
		if (m_pLocker)
			m_pLocker->Done();
	};

	void Unlock()
	{
		m_pLocker->Done();
		m_pLocker = NULL;
	}
	
private:
	CAutoLockRead& operator=(CAutoLockRead& rhs);
	CSWMRG *m_pLocker;
};

class CAutoLockWrite
{
public:
	CAutoLockWrite(CSWMRG &locker) 
	{
		m_pLocker = &locker;
		m_pLocker->WaitToWrite();
	}

	~CAutoLockWrite() 
	{
		if (m_pLocker)
			m_pLocker->Done();
	}

	void Unlock()
	{
		m_pLocker->Done();
		m_pLocker = NULL;
	}

private:
	CAutoLockWrite& operator=(CAutoLockWrite& rhs);
	CSWMRG *m_pLocker;
};

//////////////////////////////// End of File //////////////////////////////////
