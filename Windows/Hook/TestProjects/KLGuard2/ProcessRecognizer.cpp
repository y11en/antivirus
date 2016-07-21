// ProcessRecognizer.cpp: implementation of the CProcessRecognizer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProcessRecognizer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcessRecognizer::CProcessRecognizer()
{
	m_bRecognized = false;
}

CProcessRecognizer::CProcessRecognizer(PWCHAR pwchFileName)
{
	m_bRecognized = Recognize(pwchFileName);
}

bool CProcessRecognizer::Recognize(PWCHAR pwchFileName)
{
	m_bRecognized = false;
	HANDLE hFile = CreateFile(pwchFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	CAutoFileHandle autoFH(hFile);
	if (hFile == INVALID_HANDLE_VALUE)
		return m_bRecognized;

	if (Get_MD5(hFile, m_Hash))
	{
		m_bRecognized = true;
		m_ObjectSize = GetFileSize(hFile, NULL);
	}

	return m_bRecognized;
}

bool CProcessRecognizer::RecognizeWithSizeCheck(PWCHAR pwchFileName, ULONG ObjectSize)
{
	m_bRecognized = false;
	HANDLE hFile = CreateFile(pwchFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	CAutoFileHandle autoFH(hFile);

	if (hFile == INVALID_HANDLE_VALUE)
		return m_bRecognized;

	m_ObjectSize = GetFileSize(hFile, NULL);
	if (m_ObjectSize != ObjectSize)
		return m_bRecognized;

	if (Get_MD5(hFile, m_Hash))
		m_bRecognized = true;

	return m_bRecognized;
}

CProcessRecognizer::~CProcessRecognizer()
{

}

bool CProcessRecognizer::IsEQU(const md5_byte_t* pHash)
{
	if (!m_bRecognized)
		return false;

	int nRet = memcmp(m_Hash, pHash, sizeof(m_Hash));
	
	return nRet == 0 ? true : false;
}