/*
*/

#if !defined(__KL_NNTPPROTOCOLANALYZERIMPL_H)
#define __KL_NNTPPROTOCOLANALYZERIMPL_H

#include "../common/BaseProtocolAnalyzerImpl.h"
#include "../common/SplitTextBuffer.h"
#include "../common/ContentItem.h"

namespace TrafficProtocoller
{

//////////////////////////////////////////////////////////////////////////
class NntpProtocolAnalyzerImpl : public BaseProtocolAnalyzerImpl
{
	typedef BaseProtocolAnalyzerImpl Base;
public:
	NntpProtocolAnalyzerImpl()  
		: Base()
		, m_detect_state(dsWaitServerGreeting)
		, m_detect_buffer_server("\r\n")
		, m_detect_buffer_client("\r\n")
		, m_process_state(psWait)
	{
	}
	detect_code_t Detect(data_source_t ds, const char* data, size_t size);
	void EnqueueData(data_source_t ds, const char* data, size_t data_size);
	void AssembleContent(IContentItem& content) const;
private:
	Util::SplitTextBuffer& GetDetectBuffer(data_source_t ds)  
	{
		return (ds == dsServer 
					? m_detect_buffer_server 
					: m_detect_buffer_client);
	}
	bool ApplyDetectData(data_source_t ds, const char* data, size_t size)
	{
		return GetDetectBuffer(ds).PutRawData(data, size);
	}
	bool HasIncompleteDetectData(data_source_t ds)
	{
		return GetDetectBuffer(ds).HasIncompleteData();
	}
	void ClearIncompleteDetectData(data_source_t ds)
	{
		GetDetectBuffer(ds).ClearIncompleteData();
	}
	bool& SkipDataToCrLfFlag(data_source_t ds)
	{
		return (ds == dsServer 
					? m_process_server_skip_to_crlf 
					: m_process_client_skip_to_crlf);
	}
	Util::DataBuffer& GetUnprocessedBuffer(data_source_t ds)
	{
		return (ds == dsServer ? m_unprocessed_server : m_unprocessed_client);
	}
private:
	// enums & types
	enum detect_state
	{
		dsWaitServerGreeting = 0,
		dsWaitSignificantCommand,
		dsGroupResponse,
		dsPostResponseBeforeDetect,
		dsPostResponse,
		dsNewGroupsResponse,
		dsNewNewsResponse,
		dsDetected,
		dsDataFollowed,
		dsFailed
	};
	enum process_state
	{
		psWait = 0,
		psPostResponse,
		psPostData,
		psPostDataFromDetect,
		psArticleResponse,
		psArticleData
	};
public:
	class IProcessStateData
	{
	public:
		virtual ~IProcessStateData()  {}
	};
private:
	typedef std::auto_ptr<IProcessStateData> IProcessStateDataAP;
private:
	// internals used for detect
	detect_state m_detect_state;
	Util::SplitTextBuffer m_detect_buffer_server;
	Util::SplitTextBuffer m_detect_buffer_client;
	// internals used while processing
	bool m_process_server_skip_to_crlf;
	bool m_process_client_skip_to_crlf;
	Util::DataBuffer m_unprocessed_server;
	Util::DataBuffer m_unprocessed_client;
	process_state m_process_state;
	IProcessStateDataAP m_process_state_data;
};

}  // namespace TrafficProtocoller

#endif   // !defined(__KL_NNTPPROTOCOLANALYZERIMPL_H)
