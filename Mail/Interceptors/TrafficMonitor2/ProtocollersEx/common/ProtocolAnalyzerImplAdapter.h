/*
*/

#if !defined(__KL_TRAFFICPROTOCOLLER_IPROTOCOLANALYZERIMPLADAPTER_H)
#define __KL_TRAFFICPROTOCOLLER_IPROTOCOLANALYZERIMPLADAPTER_H

#include "../interfaces/IProtocolAnalyzer.h"

namespace TrafficProtocoller
{
namespace Util
{

//////////////////////////////////////////////////////////////////////////
template <typename AnalyzerExClass>
class ProtocolAnalyzerImplAdapter : public IProtocolAnalyzer
{
public:
	detect_code_t Detect(data_source_t src, const tBYTE* data, tDWORD data_size)
	{ 
		return m_impl.Detect(src, reinterpret_cast<const char*>(data), data_size); 
	}
	tBOOL HasContent() const
	{ 
		return m_impl.HasContent(); 
	}
	tERROR EnqueueData(data_source_t src, const tBYTE* data, tDWORD data_size)
	{ 
		m_impl.EnqueueData(src, reinterpret_cast<const char*>(data), data_size);
		return 0;
	}
	tERROR ExtractContent(IContentItem** ppContent)
	{
		*ppContent = m_impl.ExtractContent().release();
		return 0;
	}
	tERROR AssembleContent(IContentItem* pContent) const
	{
		if (pContent)
			m_impl.AssembleContent(*pContent);
		return 0;
	}
	tERROR ReleaseContent(IContentItem* pContent) const
	{
		delete pContent;
		return 0;
	}
private:
	AnalyzerExClass m_impl;
};

}  // namespace Util
}  // namespace TrafficProtocoller

#endif  // !defined(__KL_TRAFFICPROTOCOLLER_IPROTOCOLANALYZERIMPLADAPTER_H)
