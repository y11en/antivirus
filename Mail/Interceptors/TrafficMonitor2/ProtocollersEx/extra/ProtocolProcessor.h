#if !defined(__KL_PROTOCOLPROCESSING_PROCESSOR_H)
#define __KL_PROTOCOLPROCESSING_PROCESSOR_H

#include "../common/IProtocolAnalyzerEx.h"
#include "ISessionProxy.h"
#include "../common/DataBuffer.h"
#include "../common/ContentItem.h"

namespace TrafficProtocoller
{

//////////////////////////////////////////////////////////////////////////
template<typename Analyzer_, typename ContentHandler_>
class ProtocolProcessor
{
public:
	ProtocolProcessor(ISessionProxy& session_proxy)
		: m_session_proxy(session_proxy)
	{
	}
	detect_code_t Detect(data_source_t ds, const char* data, size_t size)
	{
		return m_analyzer.Detect(ds, data, size);
	}
	void Process(data_source_t ds, const char* data, size_t size);
private:
//	Util::DataBuffer& GetUnprocessedBuffer(data_source_t ds)
//	{
//		return (ds == ctxServer 
//					? m_unprocessed_server
//					: m_unprocessed_client);
//	}
private:
//	Util::DataBuffer m_unprocessed_server;
//	Util::DataBuffer m_unprocessed_client;
	ISessionProxy& m_session_proxy;
	Analyzer_ m_analyzer;
	ContentHandler_ m_content_handler;
};

//////////////////////////////////////////////////////////////////////////
template<typename Analyzer_, typename ContentHandler_>
void ProtocolProcessor<Analyzer_, ContentHandler_>::Process(
	data_source_t ds, 
	const char* data, 
	size_t size
	)
{
	m_analyzer.EnqueueData(ds, data, size);

	while (m_analyzer.HasContent())
	{
		std::auto_ptr<IContentItem> pItem = m_analyzer.ExtractContent();
		m_content_handler.Fix(*(pItem.get()));
		m_analyzer.AssembleContent(*(pItem.get()));
		ContentItem* pItemEx = static_cast<ContentItem*>(pItem.get());
		if (pItemEx && pItemEx->DataSize() > 0)
		{
			std::string sData;
			sData.reserve(pItemEx->DataSize());
			for (int i = 0; i < pItemEx->DataSize(); ++i)
			{
				sData += pItemEx->DataElement(i);
			}
			m_session_proxy.Send(ds, sData.c_str(), sData.size());
		}
	}
}

}  // namespace TrafficProtocoller

#endif  // !defined(__KL_PROTOCOLPROCESSING_PROCESSOR_H)
