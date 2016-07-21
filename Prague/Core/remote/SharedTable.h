/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	SharedTable.h
 * \author	Pavel Mezhuev
 * \date	09.12.2002 17:34:41
 * \brief	Интерфейс распределенной таблицы в памяти.
 * 
 */

//************************************************************************

#if defined (_WIN32)
#include <windows.h>
#include <time.h>
#include <service\sa.h>
#define INVALID_FILE_VALUE 0
#elif defined (__unix__)
#include <pthread.h>
#define INVALID_FILE_VALUE -1
#if defined (__Linux__)
#define  PTHREAD_MUTEX_RECURSIVE  PTHREAD_MUTEX_RECURSIVE_NP
#endif

#endif

#define	SHTR_INVALID_HANDLE	((hRecord)-1)
typedef int			hRecord;

#define SHTR_FLAG_REF			1
#define SHTR_FLAG_DEREF			2
#define SHTR_FLAG_NOLOCK		4
#define SHTR_FLAG_CLOSE			8

//************************************************************************

class CSharedTable
{
public:
	CSharedTable() 
		: m_hFile(INVALID_FILE_VALUE)
		, m_pTable(0)
#if defined (_WIN32)
		, m_hTempFile(INVALID_HANDLE_VALUE)
#endif
	{

	}
	~CSharedTable()
	{
		Done();
	}

	enum ECreationFlags { CrUsual, CrUsePipe, CrStartPipe };

	bool Init(const char *name, long data_size, bool key_unique, bool owner_process, ECreationFlags flags);
	void Done();

	long m_ProccessId;

protected:
	virtual bool	CompareKey(hRecord record, void *key, void *data){ return true; }
	virtual bool	InitKeyData(hRecord record, void *key, void *data){ return true; }
	virtual bool	DeinitData(hRecord record, void *data){ return true; }
	virtual void	ClearProcess(hRecord record, void *data, long process){}

public:
	void			Dump();

	hRecord			InsertRecord(void *key, long process=0);
	hRecord			ReplaceRecord(void *key, long process=0);
	void			DeleteRecord(void *key, long process=0);
	hRecord			FindRecord(void *key, long flags, long process=0, void* data=NULL);
	void            EnumRecords(void *key, long* processes, long* count);

	long			GetRecordProcess(hRecord record);
	void *			GetRecordData(hRecord record, void* data);

	void *			LockRecordData(hRecord record);
	void			UnlockRecordData(hRecord record);

	void			ClearProcessRecords(long process);

private:
	hRecord			AddRecord(void *key, long process);
	bool			ReviewTable(long count);

	long			m_RecordSize;
	long			m_RecordCount;

	long			m_DataSize;
	bool			m_fKeyUnique;
	bool			m_fOwnedProcess;
#if defined (_WIN32)
	HANDLE			m_hFile;
	HANDLE			m_hTempFile;
	LPBYTE			m_pTable;

	HANDLE			m_hSyncObj;
	HSA				m_hSecure;
	static long 	m_nTablesCount;
	static void*	m_pFileOpenServer;
	bool			m_bStopServer;
#elif defined (__unix__)
       	int			m_hFile;
	char*			m_pTable;
	pthread_mutex_t		m_hSyncObj;
#endif
};

//************************************************************************
