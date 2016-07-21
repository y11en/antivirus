/*
*/

#pragma warning( disable : 4786 )

#include "BaseProtocolAnalyzerImpl.h"

namespace TrafficProtocoller
{

//////////////////////////////////////////////////////////////////////////
std::auto_ptr<IContentItem> BaseProtocolAnalyzerImpl::ExtractContent()
{
	CAutoCR cr(GetContentItemsListCS());
	if (!m_content_items.empty())
	{
		ContentItemPtr pItem = m_content_items.front();
		m_content_items.pop_front();
		if (pItem.get())
		{
			std::auto_ptr<ContentItem> pItemCopy(new ContentItem());
			pItem->Swap(*pItemCopy.get());
			return pItemCopy;
		}
	}
	return std::auto_ptr<IContentItem>();
}

}  // namespace TrafficProtocoller
