/*
*/

#if !defined(__KL_DSKMWRAP_DATA_BUFFER_H)
#define __KL_DSKMWRAP_DATA_BUFFER_H

#include <dskm/dskm.h>
#include <memory>
#include "exceptions.h"

namespace KL  {
namespace DskmLibWrapper  {

//////////////////////////////////////////////////////////////////////////
class DataBuffer
{
public:
	DataBuffer(void* pData, AVP_dword nDataSize, bool bCopy = false)
		: m_data(0)
		, m_size(0)
	{
		if (bCopy)
			CopyFrom(pData, nDataSize);
		else
			Assign(pData, nDataSize);
	}
	~DataBuffer()
	{
		Clear();
	}
	const void* Data() const
		{ return m_data; }
	AVP_dword DataSize() const
		{ return m_size; }
private:
	DataBuffer(const DataBuffer&);
	DataBuffer& operator = (const DataBuffer&);
	void Clear()
	{
		if (m_data)
		{
			::DSKM_Free(m_data);
			m_data = 0;
		}
	}
	void Assign(void* pData, AVP_dword nDataSize)
	{
		Clear();
		m_data = pData;
		m_size = nDataSize;
	}
	void CopyFrom(void* pData, AVP_dword nDataSize)
	{
		Clear();
		m_data = ::DSKM_Alloc(nDataSize);
		if (!m_data)
			throw DskmLibWrapperException(DskmLibWrapperException::OutOfMemory);
		memcpy(m_data, pData, nDataSize);
		m_size = nDataSize;
	}
private:
	void* m_data;
	AVP_dword m_size;
};

//////////////////////////////////////////////////////////////////////////
typedef std::auto_ptr<DataBuffer> DataBufferAP;

}  // namespace DSKM
}  // namespace KL

#endif  // !defined(__KL_DSKMWRAP_DATA_BUFFER_H)
