/*
*/

#if !defined(__KL_BASEPROTOCOLANALYZERIMPL_H)
#define __KL_BASEPROTOCOLANALYZERIMPL_H

#include "../common/IProtocolAnalyzerEx.h"
#include "../common/ContentItem.h"
#include <utils/cr.h>

namespace TrafficProtocoller
{

//////////////////////////////////////////////////////////////////////////
class BaseProtocolAnalyzerImpl : public IProtocolAnalyzerEx
{
public:
	BaseProtocolAnalyzerImpl()  {}
	bool HasContent() const;
	std::auto_ptr<IContentItem> ExtractContent();
protected:
	void AddContentItemSafe(
				data_source_t data_source,
				content_t content_type,
				const char* data, 
				size_t size
				);
	void AddContentItemSafe(ContentItemPtr item);
	CCR& GetContentItemsListCS() const  { return m_content_items_cs; }
	ContentItemsList& GetContentItemsList()  { return m_content_items; }
private:
	mutable CCR m_content_items_cs;
	ContentItemsList m_content_items;
};

//////////////////////////////////////////////////////////////////////////
inline void BaseProtocolAnalyzerImpl::AddContentItemSafe(ContentItemPtr item)
{
	CAutoCR cr(GetContentItemsListCS());
	m_content_items.push_back(item);
}

//////////////////////////////////////////////////////////////////////////
inline void BaseProtocolAnalyzerImpl::AddContentItemSafe(
	data_source_t data_source,
	content_t content_type,
	const char* data, 
	size_t size
	)
{
	AddContentItemSafe(
					ContentItemPtr(
						new ContentItem(
									data_source,
									content_type, 
									data, 
									size
									)
						)
					);
}

//////////////////////////////////////////////////////////////////////////
inline bool BaseProtocolAnalyzerImpl::HasContent() const
{
	CAutoCR cr(GetContentItemsListCS());
	return !m_content_items.empty();
}

}  // namespace TrafficProtocoller

#endif   // !defined(__KL_BASEPROTOCOLANALYZERIMPL_H)
