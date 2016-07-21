#if !defined(_POLISTORAGE_H_)
#define _POLISTORAGE_H_

#include "PoliType.h"
#include <stdio.h>

class CFileStorage : public PoliStorage
{
public:
	CFileStorage() : m_fn(NULL), m_bRead(false){}
	explicit CFileStorage(const char* fname, bool bIsRead) {attach_(fname, bIsRead);}
	~CFileStorage() {detach_();}

	virtual bool read_(void* pOutBuff, const size_t OutBuffSize)
	{
		if (m_fn)
			return fread(pOutBuff, 1, OutBuffSize, m_fn) == OutBuffSize;
			
		return false;
	}

	virtual bool write_(const void* pDataBuff, const size_t DataBuffSize)
	{
		if (m_fn)
			return fwrite(pDataBuff, 1, DataBuffSize, m_fn) == DataBuffSize;
		
		return false;
	}
	
	bool attach_(const char* fname, bool bIsRead)
	{
		m_bRead = bIsRead;
		return (m_fn = fopen(fname, bIsRead?"rb":"wb"))!=NULL;
	}

	void detach_()
	{
		if (m_fn)
		{
			fclose(m_fn);
			m_fn = NULL;
		}
	}
	
protected:
	FILE*	m_fn;
	bool		m_bRead;
};

class CMemStorage : public PoliStorage
{
public:
	CMemStorage() 
	{
		m_bRead = false;
		pMemBasePtr = NULL;
		m_Size = 0;

		pMemCurPtr = pMemBasePtr;
		m_bAutoFree = true;
		
	}

	CMemStorage(const void* pReadBuff, const size_t BuffSize)
		: m_bRead(true)
		, pMemBasePtr((char*)pReadBuff)
		, m_Size(BuffSize)
		, pMemCurPtr((char*)pReadBuff)
		, m_bAutoFree(false)
	{
	}

	~CMemStorage()
	{
		if(m_bAutoFree && pMemBasePtr)
			free(pMemBasePtr);
	}
	
	virtual bool read_(void* pOutBuff, const size_t OutBuffSize)
	{
		if (!m_bRead || (m_Size - (pMemCurPtr - pMemBasePtr))<OutBuffSize)
		{
			POLI_ASSERT_(false);
			return false;
		}

		memcpy(pOutBuff, pMemCurPtr, OutBuffSize);
		pMemCurPtr+=OutBuffSize;
		return true;
	}

	virtual bool write_(const void* pDataBuff, const size_t DataBuffSize)
	{
		if (m_bRead)
		{
			POLI_ASSERT_(false);
			return false;
		}
		for (size_t i = 0; i<DataBuffSize; ++i)
			m_WriteStorage.push_back  (((unsigned char*)pDataBuff)[i]);

		return true;		
	}
	
	size_t size_()
	{
		if (m_bRead)
			return m_Size - (pMemCurPtr - pMemBasePtr);
		
		return m_WriteStorage.size  ();
		
	}
	
	void* get_lin_space_()
	{
		if (m_bRead)
			return pMemCurPtr;
		
		if(pMemBasePtr)
			free(pMemBasePtr);
		
		pMemBasePtr = (char*)malloc(size_  ());
		for (size_t i = 0; i<size_  (); ++i)
			pMemBasePtr[i] = m_WriteStorage[i];
		
		return pMemBasePtr;
	}
	
	void detach_()
	{
		m_bAutoFree = false;
	}
	
protected:
	bool m_bAutoFree;
	bool m_bRead;
	char* pMemBasePtr;
	char* pMemCurPtr;
	size_t m_Size;
	std::deque<char> m_WriteStorage;
};

#endif //_POLISTORAGE_H_
