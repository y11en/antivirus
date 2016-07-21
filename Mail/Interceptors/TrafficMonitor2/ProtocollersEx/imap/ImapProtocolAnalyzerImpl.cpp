/*
*/

#pragma warning( disable : 4786 )

#include "ImapProtocolAnalyzerImpl.h"
#include "ImapUtil.h"
#include "../common/StringUtil.h"
#include <deque>
#include <sstream>

namespace TrafficProtocoller
{

//////////////////////////////////////////////////////////////////////////
namespace 
{
	char CRLF[] = { '\r', '\n' };
}

//////////////////////////////////////////////////////////////////////////
namespace
{
	class FetchProcessingStateData : public ImapProtocolAnalyzerImpl::IProcessStateData
	{
	public:
		ImapUtil::ImapMessageDataParser& MessageDataParser()  { return m_message_data_parser; } 
	private:
		ImapUtil::ImapMessageDataParser m_message_data_parser;
	};
}

//////////////////////////////////////////////////////////////////////////
detect_code_t ImapProtocolAnalyzerImpl::Detect(
	data_source_t ds, 
	const char* data, 
	size_t size
	)
{
	using namespace ImapUtil;

	bool bWorking = true;


	// split data & add it into internal buffer
	if (!ApplyDetectData(ds, data, size))
	{
		// произошло переполение буферов, сбросим их
		bWorking = false;
		m_detect_state = dsFailed;
	}	

	while (bWorking)
	{
		switch (m_detect_state)
		{
			case dsWaitServerGreeting:
				if (m_detect_buffer_server.HasCompleteData())
				{
					detect_state temp_state = dsFailed;
					const std::string& sResponse = m_detect_buffer_server.PeekCompleteData();
					std::string sResponseTag, sResponseData;
					if (ParseUntaggedResponse(sResponse, sResponseTag, sResponseData))
					{
						if (Util::EqualNoCase(sResponseTag, "OK"))
						{
							// some servers returns protocol name in the OK response comment
							// and some clients don't ask for CAPABILITY
							temp_state = (StringOfAtomsIsContain(sResponseData, "IMAP4rev1") ||
							              StringOfAtomsIsContain(sResponseData, "IMAP4")
												? dsDetected 
												: dsWaitCapabilityResponse);
						}
						else if (Util::EqualNoCase(sResponseTag, "PREAUTH"))
						{
							temp_state = dsWaitCapabilityResponse;
						}
					}
					m_detect_buffer_server.PopCompleteData();
					m_detect_state = temp_state;
				}
				else
				{
					bWorking = false;
				}
				break;
			case dsWaitCapabilityResponse:
				if (m_detect_buffer_server.HasCompleteData())
				{
					const std::string& sResponse = m_detect_buffer_server.PeekCompleteData();
					std::string sResponseTag, sResponseData;
					if (ParseUntaggedResponse(sResponse, sResponseTag, sResponseData))
					{
						if (Util::EqualNoCase(sResponseTag, "CAPABILITY"))
						{
							m_detect_state = 
								(StringOfAtomsIsContain(sResponseData, "IMAP4rev1") ||
								 StringOfAtomsIsContain(sResponseData, "IMAP4")
									? dsDetected 
									: dsFailed);
							bWorking = false;
						}
					}
					m_detect_buffer_server.PopCompleteData();
				}
				else
				{
					bWorking = false;
				}
				break;
			case dsDetected:
			case dsFailed:
				bWorking = false;
				break;
			default:
				m_detect_state = dsFailed;
				bWorking = false;
				break;
		}
	}
	
	// clear complete client data, because we not using it
	m_detect_buffer_client.ClearCompleteData();
	
	// return value depends on new state
	switch (m_detect_state)
	{
		case dsWaitServerGreeting:
		case dsWaitCapabilityResponse:
			return dcNeedMoreInfo;
		case dsDetected:
			m_process_server_skip_to_crlf = m_detect_buffer_server.HasIncompleteData();
			m_process_client_skip_to_crlf = m_detect_buffer_client.HasIncompleteData();
			return dcDataDetected;
		default:
			m_detect_buffer_server.ClearDataAll();
			m_detect_buffer_client.ClearDataAll();
			return dcProtocolUnknown;
	}
}

//////////////////////////////////////////////////////////////////////////
void ImapProtocolAnalyzerImpl::EnqueueData(data_source_t ds, const char* data, size_t data_size)
{
	if (data == 0 || data_size <= 0)
		return;

	const char* data_current = 0;
	const char* data_end = 0;
	Util::DataBuffer temp_buffer;
	Util::DataBuffer& unprocessed_buffer = GetUnprocessedBuffer(ds);

	if (unprocessed_buffer.Empty())
	{
		data_current = data;
		data_end = data + data_size;
	}
	else
	{
		temp_buffer.Swap(unprocessed_buffer);
		temp_buffer.Append(data, data_size);
		data_current = temp_buffer.Data();
		data_end = temp_buffer.Data() + temp_buffer.Size();
	}

	// check for incomplete data from detect
	{
		bool& skip_to_crlf = SkipDataToCrLfFlag(ds);
		if (skip_to_crlf)
		{
			// we must skip all data to first CRLF
			const char* pFind = std::search(data_current, data_end, CRLF, CRLF + 2);
			if (pFind == data_end)
			{
				// CRLF was not found at the end of block, skip whole block
				AddContentItemSafe(
							ds,
							contentProtocolSpecific,
							data_current,
							std::distance(data_current, data_end)
							);
				data_current = data_end;
				return;
			}
			else
			{
				// CRLF was found, skip beginning of block & continue with the rest
				skip_to_crlf = false;
				pFind += 2;
				AddContentItemSafe(
							ds,
							contentProtocolSpecific,
							data_current,
							std::distance(data_current, pFind)
							);
				data_current = pFind;
			}
		}
	}

	bool bStopProcessing = false;

	// process data
	while (data_current != data_end && !bStopProcessing)
	{
		switch (m_process_state)
		{

			case psWait:

				if (ds == dsServer)
				{
					// check for end of line
					const char* find_crlf = std::search(data_current, data_end, CRLF, CRLF + 2);
					if (find_crlf == data_end)
					{
						// end of line was not found, can't recognize response in that case
						// this can be FETCH response, but data is incomplete
						bStopProcessing = true;
					}
					else
					{
						const char* fetch_begin = RecognizeFetchResponse(data_current, find_crlf);
						if (fetch_begin == 0)
						{
							// this is not FETCH response, skip data
							AddContentItemSafe(
										ds,
										contentProtocolSpecific,
										data_current,
										std::distance(data_current, find_crlf + 2)
										);
							data_current = find_crlf + 2;
						}
						else
						{
							AddContentItemSafe(
										ds,
										contentProtocolSpecific,
										data_current,
										std::distance(data_current, fetch_begin)
										);
							data_current = fetch_begin;
							m_process_state = psFetchMessageData;
							m_process_state_data = IProcessStateDataAP(new FetchProcessingStateData());
						}
					}
				}
				else
				{
					// skip all client commands
					AddContentItemSafe(
								ds,
								contentProtocolSpecific,
								data_current,
								std::distance(data_current, data_end)
								);
					data_current = data_end;
				}
				break;

			case psFetchMessageData:

				if (ds == dsServer)
				{
					using namespace ImapUtil;
					FetchProcessingStateData* pStateData = static_cast<FetchProcessingStateData*>(m_process_state_data.get());
					if (!pStateData)
						throw std::logic_error("State data is invalid.");
					ImapUtil::ImapMessageDataParser& parser = pStateData->MessageDataParser();
					const char* fetch_end = data_current;
					{
						CAutoCR cr(GetContentItemsListCS());
						fetch_end = parser.Parse(data_current, data_end, GetContentItemsList());
					}
					switch (parser.ParseResult())
					{
						case ImapMessageDataParser::parsingOK:
							data_current = fetch_end;
							m_process_state = psWait;
							m_process_state_data = IProcessStateDataAP();
							break;
						case ImapMessageDataParser::parsingContinues:
							data_current = fetch_end;
							bStopProcessing = true;
							break;
						default:  // error
							data_current = fetch_end;
							m_process_state = psWait;
							m_process_state_data = IProcessStateDataAP();
							m_process_server_skip_to_crlf = true;
							break;
					}
				}
				else
				{
					// skip all client commands
					AddContentItemSafe(
								ds,
								contentProtocolSpecific,
								data_current,
								std::distance(data_current, data_end)
								);
					data_current = data_end;
				}
				break;

			default:

				// unknown state, skip all data
				AddContentItemSafe(
							ds, 
							contentProtocolSpecific,
							data_current,
							std::distance(data_current, data_end)
							);
				data_current = data_end;
				break;

		}  // switch (m_process_state)

	}  // while (data_current != data_end && !bStopProcessing)

	// add unprocessed data into internal buffer
	if (data_current != data_end)
	{
		unprocessed_buffer.Assign(data_current, std::distance(data_current, data_end));
	}
}

//////////////////////////////////////////////////////////////////////////
void ImapProtocolAnalyzerImpl::AssembleContent(IContentItem& content) const
{
	if (content.Type() != contentProtocolSpecific)
	{
		ContentItem& content_ex = static_cast<ContentItem&>(content);
		std::ostringstream os;
		os << '{' << content_ex.DataSize() << "}\r\n";
		std::string sData;
		os.str().swap(sData);
		content_ex.PushDataFront(sData.c_str(), sData.size());
	}
}

//////////////////////////////////////////////////////////////////////////
// This method tries to recognize FETCH-response in data buffer
// Return:
//		0 - this is not FETCH response
//		pointer to FETCH-response data beginning - ok, response recognized
//
const char* ImapProtocolAnalyzerImpl::RecognizeFetchResponse(
	const char* data_begin, 
	const char* data_end
	)
{
	using namespace ImapUtil;
	std::vector<std::string> aItems;
	const char* fetch_data_begin = ParseUntaggedResponse(data_begin, data_end, aItems, 2);
	if (fetch_data_begin &&
		fetch_data_begin != data_end &&
		aItems.size() >= 2 && 
		Util::EqualNoCase(aItems[1], "FETCH"))
	{
		return fetch_data_begin;
	}
	else
	{
		return 0;
	}
}

}  // namespace TrafficProtocoller
