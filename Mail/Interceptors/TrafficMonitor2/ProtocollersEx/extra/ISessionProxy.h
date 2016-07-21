 #if !defined(__KL_ISESSIONPROXY_H)
#define __KL_ISESSIONPROXY_H

#include "../interfaces/IProtocolAnalyzer.h"

namespace TrafficProtocoller
{

//////////////////////////////////////////////////////////////////////////
class ISessionProxy
{
public:
	virtual ~ISessionProxy()  {}
	virtual void Send(data_source_t ds, const char* pData, size_t nDataSize) = 0;
//	void Send(data_source_t ds, const IContentItem& item)
//	{
//		if (!item.DataEmpty())
//			Send(ds, item.Data(), item.DataSize());
//	}
};

}  // namespace TrafficProtocoller

#endif  // !defined(__KL_ISESSIONPROXY_H)
