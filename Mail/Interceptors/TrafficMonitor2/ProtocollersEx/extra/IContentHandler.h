#if !defined(__KL_ICONTENTHANDLER_H)
#define __KL_ICONTENTHANDLER_H

#include "../interfaces/IProtocolAnalyzer.h"

namespace TrafficProtocoller
{

//////////////////////////////////////////////////////////////////////////
class IContentHandler
{
public:
	virtual ~IContentHandler()  {}
	virtual void Fix(IContentItem& content_item) = 0;
};

}  // namespace TrafficProtocoller

#endif  // !defined(__KL_ICONTENTHANDLER_H)
