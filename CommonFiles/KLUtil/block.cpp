#include "StdAfx.h"
#include "except.h"
#include <assert.h>
#include "block.h"

using namespace std;
using namespace KLUTIL;

///////////////////////////////////////////////////////////////////////////////////////
// Helpers
///////////////////////////////////////////////////////////////////////////////////////

namespace
{
	char HexCharToInt(char c)
	{
		if (c>='0' && c<='9')
			return c-'0';
		if (c>='a' && c<='f')
			return c-'a'+10;
		if (c>='A' && c<='F')
			return c-'A'+10;
		assert(false); // wrong symbol
		THROW_TEXT(CExcept, "неверная hex строка (недопустимые символы)");
	}
}

///////////////////////////////////////////////////////////////////////////////////////
// CBlockBase
///////////////////////////////////////////////////////////////////////////////////////

CBlockBase::CBlockBase(const CBlockBase& t) throw()
	: m_pData( t.m_pData ), m_nDataSize( t.m_nDataSize )
{
}

CBlockBase::CBlockBase(const void* pData, size_t nDataSize) throw()
	: m_pData( static_cast<char*>(const_cast<void*>(pData)) ), m_nDataSize(nDataSize)
{
}

string CBlockBase::GetHex() const
{
	string res;
	const char pszHexDigits[] = "0123456789ABCDEF";
	res.resize(m_nDataSize*2, '0');
	string::iterator resit=res.begin();
	for (const char *p=m_pData, *end=m_pData+m_nDataSize; p<end; ++p)
	{
		*(resit++) = pszHexDigits[(*p & 0xF0) >> 4];
		*(resit++) = pszHexDigits[*p & 0x0F];
	}
	return res;
}

string CBlockBase::ToString() const
{
	return string(static_cast<const char*>(m_pData), m_nDataSize);
}

///////////////////////////////////////////////////////////////////////////////////////
// CBlock
///////////////////////////////////////////////////////////////////////////////////////

CBlock::CBlock() throw()
	: CBlockBase(NULL, 0)
	, m_nAllocatedSize(0)
{
}

CBlock::CBlock(const CBlockBase& blob)
	: CBlockBase(blob.m_pData==NULL ? NULL : new char[blob.m_nDataSize], blob.m_nDataSize)
	, m_nAllocatedSize(blob.m_nDataSize)
{
	memcpy(m_pData, blob.m_pData, blob.m_nDataSize);
}

CBlock::CBlock(const CBlock& blob)
	: CBlockBase(blob.m_pData==NULL ? NULL : new char[blob.m_nDataSize], blob.m_nDataSize)
	, m_nAllocatedSize(blob.m_nDataSize)
{
	memcpy(m_pData, blob.m_pData, blob.m_nDataSize);
}

CBlock::CBlock(const void* pData, size_t nDataSize)
	: CBlockBase(new char[nDataSize], nDataSize)
	, m_nAllocatedSize(nDataSize)
{
	memcpy(m_pData, pData, nDataSize);
}

CBlock::CBlock(size_t nReserveSize)
	: CBlockBase(new char[nReserveSize], 0)
	, m_nAllocatedSize(nReserveSize)
{
}

CBlock::CBlock(const string& szHexData)
	: CBlockBase(NULL, 0)
{
	if (szHexData.size() % 2 != 0)
		THROW_TEXT(CExcept, "неверная hex строка (нечётная длина)");
	m_nAllocatedSize = m_nDataSize = szHexData.size()/2;
	m_pData = new char[m_nAllocatedSize];
	
	const char* pChar = szHexData.data();
	for (char *p=m_pData, *end=m_pData+m_nDataSize; p<end; ++p)
	{
		*p = (HexCharToInt(*pChar) << 4) | (HexCharToInt(*(pChar+1))) ;
		pChar += 2;
	}
}

CBlock::~CBlock()
{
	if (m_pData)
		delete[] m_pData;
}

void CBlock::Clear() throw()
{
	m_nDataSize = 0;
}

void CBlock::ClearSecure() throw()
{
	memset(m_pData, 0, m_nDataSize);
	m_nDataSize = 0;
}

void CBlock::MakeAvailable(size_t nSize)
{
	if (nSize > m_nAllocatedSize)
	{
		char* pNewData = new char [nSize];
		assert( (m_pData==NULL && m_nDataSize==0) || (m_pData!=NULL && m_nDataSize>0));
		if (m_pData)
		{
			memcpy(pNewData, m_pData, m_nDataSize);
			delete[] m_pData;
		}
		m_pData = pNewData;
		m_nAllocatedSize = nSize;
	}
}

void CBlock::MakeReserve(size_t nReserveSize)
{
	MakeAvailable(m_nDataSize + nReserveSize);
}

void CBlock::SetSize(size_t nSize, char fill)
{
	MakeAvailable(nSize);
	if (m_nDataSize<nSize)
		memset(m_pData+m_nDataSize, fill, nSize-m_nDataSize);
	m_nDataSize = nSize;
}

void* CBlock::GetData() throw()
{ 
	assert(m_pData); 
	return m_pData; 
}

void* CBlock::GetDataEnd() throw()
{ 
	assert(m_pData);
	return m_pData+m_nDataSize; 
}

size_t CBlock::GetAllocatedSize() const throw()
{
	return m_nAllocatedSize;
}

size_t CBlock::GetReserveSize() const throw()
{ 
	return m_nAllocatedSize-m_nDataSize; 
}

void CBlock::AppendData(size_t nDataSize, const void* pData)
{
	MakeReserve(nDataSize);
	memcpy(GetDataEnd(), pData, nDataSize);
	AddDataSize(static_cast<int>(nDataSize));
}

void CBlock::AddDataSize(int nAddSize) throw()
{ 
	assert(((int)m_nDataSize)+nAddSize>=0);
	m_nDataSize += nAddSize; 
	assert(m_nDataSize<=m_nAllocatedSize); 
}

void CBlock::operator <<(CBlock& rBlob) throw()
{
	if (m_pData) delete[] m_pData;
	m_pData = rBlob.m_pData;
	m_nAllocatedSize = rBlob.m_nAllocatedSize;
	m_nDataSize = rBlob.m_nDataSize;
	rBlob.m_pData = NULL;
	rBlob.m_nAllocatedSize = rBlob.m_nDataSize = 0;
}

void CBlock::AttachData(void* pData, size_t nDataSize) throw()
{
	if (m_pData)
		delete[] m_pData;
	m_pData = static_cast<char*>(pData);
	m_nAllocatedSize = m_nDataSize = nDataSize;
}

void* CBlock::DetachData() throw()
{
	void* pRes = m_pData;
	m_pData = NULL;
	m_nAllocatedSize = m_nDataSize = 0;
	return pRes;
}

///////////////////////////////////////////////////////////////////////////////////////
// CSecBlock
///////////////////////////////////////////////////////////////////////////////////////

CSecBlock::CSecBlock() throw(): CBlock() {}

CSecBlock::CSecBlock(const CBlockBase& blob): CBlock(blob) {}

CSecBlock::CSecBlock(const void* pData, size_t nDataSize): CBlock(pData, nDataSize) {}

CSecBlock::CSecBlock(size_t nReserveSize): CBlock(nReserveSize) {}

CSecBlock::CSecBlock(const string& szHexData): CBlock(szHexData) {}

CSecBlock::~CSecBlock()
{
	ClearSecure();
}