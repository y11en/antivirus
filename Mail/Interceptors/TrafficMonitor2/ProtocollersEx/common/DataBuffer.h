/*
*/

#if !defined(__KL_TRAFFICPROTOCOLLER_DATABUFFER_H)
#define __KL_TRAFFICPROTOCOLLER_DATABUFFER_H

#include <memory>

namespace TrafficProtocoller
{
namespace Util
{

//////////////////////////////////////////////////////////////////////////
class DataBuffer
{
public:
	DataBuffer(const char* data = 0, size_t size = 0)
		: m_size(0)
	{
		if (data != 0 && size > 0)
			Assign(data, size);
	}
	bool Empty() const			{ return (m_size == 0); }
	size_t Size() const			{ return m_size; }
	const char* Data() const	{ return m_data.get(); }
	void Swap(DataBuffer& dest)
	{
		std::swap(m_size, dest.m_size);
		std::swap(m_data, dest.m_data);
	}
	void Clear()
	{
		m_data = std::auto_ptr<char>();
		m_size = 0;
	}
	void Assign(const char* data, size_t size)
	{
		if (data != 0 && size > 0)
		{
			std::auto_ptr<char> p(new char[size]);
			memcpy(p.get(), data, size*sizeof(char));
			m_data = p;
			m_size = size;
		}
		else
		{
			Clear();
		}
	}
	void Append(const char* data, size_t size)
	{
		PushBack(data, size);
	}
	void PushFront(const char* data, size_t size)
	{
		if (data != 0 && size > 0)
		{
			std::auto_ptr<char> p(new char[m_size + size]);
			memcpy(p.get(), data, size*sizeof(char));
			if (m_size > 0)
				memcpy(p.get() + size, m_data.get(), m_size*sizeof(char));
			m_data = p;
			m_size += size;
		}
	}
	void PushBack(const char* data, size_t size)
	{
		if (data != 0 && size > 0)
		{
			std::auto_ptr<char> p(new char[m_size + size]);
			if (m_size > 0)
				memcpy(p.get(), m_data.get(), m_size*sizeof(char));
			memcpy(p.get() + m_size, data, size*sizeof(char));
			m_data = p;
			m_size += size;
		}
	}
	void RemoveFront(size_t count)
	{
		if (!Empty() && count > 0)
		{
			if (count >= m_size)
			{
				Clear();
			}
			else
			{
				size_t new_size = m_size - count;
				std::auto_ptr<char> p(new char[new_size]);
				memcpy(p.get(), m_data.get() + count, new_size*sizeof(char));
				m_data = p;
				m_size = new_size;
			}
		}
	}
private:
	DataBuffer(const DataBuffer&);
	DataBuffer& operator = (const DataBuffer&);
private:
	size_t m_size;
	std::auto_ptr<char> m_data;  // it works properly with chars
};

}  // namespace Util
}  // namespace TrafficProtocoller

#endif  // !defined(__KL_TRAFFICPROTOCOLLER_DATABUFFER_H)
