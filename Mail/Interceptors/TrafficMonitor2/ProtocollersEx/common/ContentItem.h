#if !defined(__KL_CONTENTITEM_H)
#define __KL_CONTENTITEM_H

#include <list>
#include <boost/shared_ptr.hpp>
#include "DataBuffer.h"
#include "../interfaces/IProtocolAnalyzer.h"

#include <Prague/wrappers/io.hpp>
#include <Prague/wrappers/io_helpers.h>

namespace TrafficProtocoller
{

//////////////////////////////////////////////////////////////////////////
class ContentItem : public IContentItem
{
public:
	virtual data_source_t Source() const	{ return m_data_source; }
	virtual content_t Type() const			{ return m_content_type; }
	virtual hIO DataIO() const				{ return m_io.get(); }
	virtual hIO DetachDataIO()				{ return m_io.release(); }
	virtual void RecreateDataIO(const char* data = 0, size_t size = 0)
	{
		prague::IO<char> temp_io(prague::create_temp_io());
		if (data != 0 && size > 0)
			temp_io.assign(data, data + size);
		m_io.swap(temp_io);
	}
public:
	ContentItem(
		data_source_t data_source = dsUnknown, 
		content_t content_type = contentProtocolSpecific, 
		const char* data = 0, 
		size_t size = 0
		)
		: m_data_source(data_source)
		, m_content_type(content_type)
		, m_io(prague::create_temp_io())
	{
		if (data != 0 && size > 0)
			m_io.assign(data, data + size);
	}
	void Swap(ContentItem& dest)
	{
		std::swap(m_data_source, dest.m_data_source);
		std::swap(m_content_type, dest.m_content_type);
		m_io.swap(dest.m_io);
	}
	bool DataEmpty() const								{ return m_io.empty(); }
	size_t DataSize() const								{ return (size_t)m_io.size(); }
	char DataElement(size_t index) const				{ return m_io.at(index); }
	void ClearData()									{ m_io.clear(); }
	void AssignData(const char* data, size_t size)		{ m_io.assign(data, data + size); }
	void PushDataFront(const char* data, size_t size)	{ m_io.insert(m_io.begin(), data, data + size); }
	void PushDataBack(const char* data, size_t size)	{ m_io.insert(m_io.end(), data, data + size); }
	void AppendData(const char* data, size_t size)		{ PushDataBack(data, size); }
	void ReplaceData(const char* find_data, size_t find_data_size,
					 const char* replace_data, size_t replace_data_size);
private:
	ContentItem(const ContentItem&);
	ContentItem& operator = (const ContentItem&);
private:
	data_source_t m_data_source;
	content_t m_content_type;
	prague::IO<char> m_io;
};

//////////////////////////////////////////////////////////////////////////
inline void ContentItem::ReplaceData(
	const char* find_data, 
	size_t find_data_size,
	const char* replace_data, 
	size_t replace_data_size
	)
{
	if (!find_data || !replace_data)
		return;
	prague::IO<char>::iterator itFind = m_io.begin();
	while (true)
	{
		itFind = std::search(itFind, m_io.end(), find_data, find_data + find_data_size);
		if (itFind == m_io.end())
		{
			break;
		}
		else if (find_data_size >= replace_data_size)
		{
			for (size_t i = 0; i < replace_data_size; ++i)
			{
				m_io.write(itFind, replace_data[i]);
				++itFind;
			}
			size_t erase_count = find_data_size - replace_data_size;
			if (erase_count > 0)
			{
				prague::IO<char>::iterator itLastErase = itFind;
				itLastErase += erase_count;
				m_io.erase(itFind, itLastErase);
			}
		}
		else  // find_data_size < replace_data_size
		{
			for (size_t i = 0; i < find_data_size; ++i)
			{
				m_io.write(itFind, replace_data[i]);
				++itFind;
			}
			m_io.insert(itFind, replace_data + find_data_size, replace_data + replace_data_size);
			itFind += (replace_data_size - find_data_size);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
typedef boost::shared_ptr<ContentItem> ContentItemPtr;

//////////////////////////////////////////////////////////////////////////
typedef std::list<ContentItemPtr> ContentItemsList;

}  // namespace TrafficProtocoller

#endif  // !defined(__KL_CONTENTITEM_H)
