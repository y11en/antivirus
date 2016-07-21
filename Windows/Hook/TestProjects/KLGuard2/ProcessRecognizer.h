// ProcessRecognizer.h: interface for the CProcessRecognizer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSRECOGNIZER_H__947A38B1_9BF3_4585_A0EC_F3B2F67DF8EC__INCLUDED_)
#define AFX_PROCESSRECOGNIZER_H__947A38B1_9BF3_4585_A0EC_F3B2F67DF8EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "md5.h"

class CProcessRecognizer
{
public:
	CProcessRecognizer();
	CProcessRecognizer(PWCHAR pwchFileName);
	virtual ~CProcessRecognizer();

	bool Recognize(PWCHAR pwchFileName);
	bool RecognizeWithSizeCheck(PWCHAR pwchFileName, ULONG ObjectSize);
	bool IsRecognized(){return m_bRecognized;}

	const md5_byte_t* GetHash() {return m_Hash;}
	ULONG GetObjectSize() {return m_ObjectSize;};

	bool IsEQU(const md5_byte_t* pHash);
private:
	bool m_bRecognized;

	md5_byte_t m_Hash[16];
	ULONG m_ObjectSize;
};

class CHashContainer
{
public:
	CHashContainer()
	{
		memset(m_Hash, 0, sizeof(m_Hash));
		m_ObjectSize = 0;
	}
	
	CHashContainer(const md5_byte_t* pHash, ULONG ObjectSize)
	{
		memcpy(m_Hash, pHash, sizeof(m_Hash));
		m_ObjectSize = ObjectSize;
	};

	~CHashContainer()
	{
	};

	const md5_byte_t* GetHash() {return m_Hash;}
	ULONG GetObjectSize(){return m_ObjectSize;}
private:
	md5_byte_t m_Hash[16];
	ULONG	m_ObjectSize;
};

#endif // !defined(AFX_PROCESSRECOGNIZER_H__947A38B1_9BF3_4585_A0EC_F3B2F67DF8EC__INCLUDED_)
