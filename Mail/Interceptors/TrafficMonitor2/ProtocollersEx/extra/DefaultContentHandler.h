#if !defined(__KL_DEFAULTCONTENTHANDLER_H)
#define __KL_DEFAULTCONTENTHANDLER_H

#include "IContentHandler.h"

namespace TrafficProtocoller
{

//////////////////////////////////////////////////////////////////////////
class DefaultContentHandler : public IContentHandler
{
public:
	virtual void Fix(IContentItem& content_item)
	{
//		std::string s("\r\n[processed by KAV]\r\n");
//		switch(content_item.Type())
//		{
//			case contentProtocolSpecific:
//				s = "\r\n[KAV: skipped]\r\n";
//				break;
//			case contentMime:
//				s = "\r\n[KAV: MIME processed]\r\n";
//				break;
//		}
//		if (!s.empty())
//			content_item.AppendData(s.c_str(), s.size());
	}
};

}  // namespace TrafficProtocoller

#endif  // !defined(__KL_DEFAULTCONTENTHANDLER_H)
