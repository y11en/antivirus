/*
*/

#pragma warning( disable : 4786 )

#include "NntpProtocolAnalyzerImpl.h"
//#include "NntpUtil.h"
#include "../common/StringUtil.h"
#include <deque>
#include <sstream>

namespace TrafficProtocoller
{

namespace 
{

	//////////////////////////////////////////////////////////////////////////
	char CRLF[] = { '\r', '\n' };

	//////////////////////////////////////////////////////////////////////////
	class MessageBodyStateData : public NntpProtocolAnalyzerImpl::IProcessStateData
	{
	public:
		MessageBodyStateData(data_source_t ds)
			: m_content_item(new ContentItem(ds, contentMime))
		{
		}
		bool DataEmpty()
		{
			return m_content_item->DataEmpty();
		}
		void AppendData(const char* begin, const char* end)
		{
			m_content_item->AppendData(begin, std::distance(begin, end));
		}
		ContentItemPtr GetContentItem() const
		{
			return m_content_item;
		}
	private:
		ContentItemPtr m_content_item;
	};

	//////////////////////////////////////////////////////////////////////////
	int GetServerResponseCode(const std::string& sResponse)
	{
		if (sResponse.size() == 3 ||
			(sResponse.size() > 3 && sResponse[3] == ' '))
		{
			return atoi(sResponse.substr(0, 3).c_str());
		}
		else
		{
			return 0;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	template <typename _II>
	int GetServerResponseCode(_II begin, _II end)
	{
		_II current = begin;
		for (int i = 0; i < 4; ++i)
		{
			if (current == end)
				break;
			++current;
		}
		return GetServerResponseCode(std::string(begin, current));
	}

	//////////////////////////////////////////////////////////////////////////
	template <typename _II>
	std::string GetClientCommandName(_II begin, _II end)
	{
		std::string sResult;
		if (begin != end)
		{
			_II itSpace = std::find(begin, end, ' ');
			if (itSpace == end)
				sResult.assign(begin, end);
			else
				sResult.assign(begin, itSpace);
		}
		return sResult;
	}

	//////////////////////////////////////////////////////////////////////////
	inline std::string GetClientCommandName(const std::string& sRequest)
	{
		return GetClientCommandName(sRequest.begin(), sRequest.end());
	}

}  // namespace

//////////////////////////////////////////////////////////////////////////
detect_code_t NntpProtocolAnalyzerImpl::Detect(
	data_source_t ds, 
	const char* data, 
	size_t size
	)
{
	switch (m_detect_state)
	{
		case dsDataFollowed:
			return dcDataDetected;
		case dsFailed:
			return dcProtocolUnknown;
	}


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
					int nResponseCode = GetServerResponseCode(m_detect_buffer_server.PeekCompleteData());
					m_detect_buffer_server.PopCompleteData();
					m_detect_state = ((nResponseCode == 200 || nResponseCode == 201) 
											? dsWaitSignificantCommand
											: dsFailed);
				}
				else
				{
					bWorking = false;
				}
				break;

			case dsWaitSignificantCommand:

				m_detect_buffer_server.ClearCompleteData();  // ignore server responses

				if (m_detect_buffer_client.HasCompleteData())
				{
					std::string sCommandName(GetClientCommandName(m_detect_buffer_client.PeekCompleteData()));
					m_detect_buffer_client.PopCompleteData();
					if (sCommandName.empty())
					{
						// state is not changing
					}
					else if (Util::EqualNoCase(sCommandName, "GROUP"))
					{
						m_detect_state = dsGroupResponse;
					}
					else if (Util::EqualNoCase(sCommandName, "NEWGROUPS"))
					{
						m_detect_state = dsNewGroupsResponse;
					}
					else if (Util::EqualNoCase(sCommandName, "NEWNEWS"))
					{
						m_detect_state = dsNewNewsResponse;
					}
					else if (Util::EqualNoCase(sCommandName, "POST"))
					{
						m_detect_state = dsPostResponseBeforeDetect;
					}
				}
				else
				{
					bWorking = false;
				}
				break;

			case dsGroupResponse:

				if (m_detect_buffer_server.HasCompleteData())
				{
					int nResponseCode = GetServerResponseCode(m_detect_buffer_server.PeekCompleteData());
					m_detect_buffer_server.PopCompleteData();
					m_detect_state = ((nResponseCode == 211 || nResponseCode == 411) 
											? dsDetected
											: dsWaitSignificantCommand);
				}
				else
				{
					bWorking = false;
				}
				break;

			case dsNewGroupsResponse:

				if (m_detect_buffer_server.HasCompleteData())
				{
					int nResponseCode = GetServerResponseCode(m_detect_buffer_server.PeekCompleteData());
					m_detect_buffer_server.PopCompleteData();
					m_detect_state = (nResponseCode == 231 
											? dsDetected
											: dsWaitSignificantCommand);
				}
				else
				{
					bWorking = false;
				}
				break;

			case dsNewNewsResponse:

				if (m_detect_buffer_server.HasCompleteData())
				{
					int nResponseCode = GetServerResponseCode(m_detect_buffer_server.PeekCompleteData());
					m_detect_buffer_server.PopCompleteData();
					m_detect_state = (nResponseCode == 230
											? dsDetected
											: dsWaitSignificantCommand);
				}
				else
				{
					bWorking = false;
				}
				break;

			case dsPostResponseBeforeDetect:

				if (m_detect_buffer_server.HasCompleteData())
				{
					int nResponseCode = GetServerResponseCode(m_detect_buffer_server.PeekCompleteData());
					m_detect_buffer_server.PopCompleteData();
					if (nResponseCode == 340)
					{
						m_detect_state = dsDataFollowed;
						m_process_state = psPostDataFromDetect;
						bWorking = false;
					}
					else
					{
						m_detect_state = dsWaitSignificantCommand;
					}
				}
				else
				{
					bWorking = false;
				}
				break;

			case dsDetected:

				m_detect_buffer_server.ClearCompleteData();

				if (m_detect_buffer_client.HasCompleteData())
				{
					std::string sCommandName(GetClientCommandName(m_detect_buffer_client.PeekCompleteData()));
					m_detect_buffer_client.PopCompleteData();
					if (sCommandName.empty())
					{
						// state is not changing
					}
					else if (Util::EqualNoCase(sCommandName, "ARTICLE") ||
							 Util::EqualNoCase(sCommandName, "BODY"))
					{
						m_detect_state = dsDataFollowed;
						m_process_state = psArticleResponse;
						bWorking = false;
					}
					else if (Util::EqualNoCase(sCommandName, "POST"))
					{
						m_detect_state = dsPostResponse;
					}
				}
				else
				{
					bWorking = false;
				}
				break;

			case dsPostResponse:

				if (m_detect_buffer_server.HasCompleteData())
				{
					int nResponseCode = GetServerResponseCode(m_detect_buffer_server.PeekCompleteData());
					m_detect_buffer_server.PopCompleteData();
					if (nResponseCode == 340)
					{
						m_detect_state = dsDataFollowed;
						m_process_state = psPostDataFromDetect;
						bWorking = false;
					}
					else
					{
						m_detect_state = dsDetected;
					}
				}
				else
				{
					bWorking = false;
				}
				break;

			case dsFailed:
				
				bWorking = false;
				break;

		}
	}
	
	// return value depends on new state
	switch (m_detect_state)
	{
//		case dsDetected:
//			return dcProtocolDetected;
		case dsDetected:
		case dsDataFollowed:
			m_process_server_skip_to_crlf = m_detect_buffer_server.HasIncompleteData();
			m_process_client_skip_to_crlf = m_detect_buffer_client.HasIncompleteData();
			return dcDataDetected;
		case dsFailed:
			m_detect_buffer_server.ClearDataAll();
			m_detect_buffer_client.ClearDataAll();
			return dcProtocolUnknown;
		default:
			return dcNeedMoreInfo;
	}
}

//////////////////////////////////////////////////////////////////////////
void NntpProtocolAnalyzerImpl::EnqueueData(data_source_t ds, const char* data, size_t data_size)
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

				if (ds == dsClient)
				{
					// check for end of line
					const char* find_crlf = std::search(data_current, data_end, CRLF, CRLF + 2);
					if (find_crlf == data_end)
					{
						// end of line was not found, can't recognize response in that case
						bStopProcessing = true;
					}
					else
					{
						// check commands
						std::string sCommandName(GetClientCommandName(data_current, find_crlf));
						if (Util::EqualNoCase(sCommandName, "ARTICLE") || Util::EqualNoCase(sCommandName, "BODY"))
						{
							m_process_state = psArticleResponse;
						}
						else if (Util::EqualNoCase(sCommandName, "POST"))
						{
							m_process_state = psPostResponse;
						}
						else
						{
							// no need to process line
						}
						// send command text
						AddContentItemSafe(
									ds,
									contentProtocolSpecific,
									data_current,
									std::distance(data_current, find_crlf + 2)
									);
						data_current = find_crlf + 2;
					}
				}
				else
				{
					// skip all server posted data
					AddContentItemSafe(
								ds,
								contentProtocolSpecific,
								data_current,
								std::distance(data_current, data_end)
								);
					data_current = data_end;
				}
				break;

			case psPostResponse:

				if (ds == dsServer)
				{
					// check for end of line
					const char* find_crlf = std::search(data_current, data_end, CRLF, CRLF + 2);
					if (find_crlf == data_end)
					{
						// end of line was not found, can't recognize response in that case
						bStopProcessing = true;
					}
					else
					{
						int nResponseCode = GetServerResponseCode(data_current, find_crlf);
						if (nResponseCode == 340)
						{
							// server response code is OK - user can send article
							m_process_state = psPostData;
							m_process_state_data = IProcessStateDataAP(new MessageBodyStateData(dsClient));
						}
						else
						{
							// user can't send article, so we shall return to wait state
							m_process_state = psWait;
						}
						// send response
						AddContentItemSafe(
									ds,
									contentProtocolSpecific,
									data_current,
									std::distance(data_current, find_crlf + 2)
									);
						data_current = find_crlf + 2;
					}
				}
				else
				{
					// skip all client data
					AddContentItemSafe(
								ds,
								contentProtocolSpecific,
								data_current,
								std::distance(data_current, data_end)
								);
					data_current = data_end;
				}
				break;
			
			case psPostDataFromDetect:
				m_process_state = psPostData;
				m_process_state_data = IProcessStateDataAP(new MessageBodyStateData(dsClient));
				break;
				
			case psPostData:
				
				if (ds == dsClient)
				{
					MessageBodyStateData* pStateData = static_cast<MessageBodyStateData*>(m_process_state_data.get());
					if (!pStateData)
						throw std::logic_error("State data is invalid.");
					// in this state we must collect post data for check
					// check for end of line
					const char* find_crlf = data_current;
					while (true)
					{
						find_crlf = std::search(data_current, data_end, CRLF, CRLF + 2);
						if (find_crlf == data_end)
						{
							// end of line was not found, can't recognize response in that case
							bStopProcessing = true;
							break;
						}
						else
						{
							// check for end of post
							if ((data_current + 1) == find_crlf && *data_current == '.')
							{
								if (!pStateData->DataEmpty())
									AddContentItemSafe(pStateData->GetContentItem());
								AddContentItemSafe(
											ds, 
											contentProtocolSpecific, 
											data_current, 
											std::distance(data_current, find_crlf + 2)
											);
								data_current = find_crlf + 2;
								m_process_state = psWait;
								m_process_state_data = IProcessStateDataAP();
								break;
							}
							else
							{
								if (*data_current == '.')
								{
									// if '.' is the first symbol in line, skip it (will be restored later in AssembleContent)
									++data_current;
								}
								pStateData->AppendData(data_current, find_crlf + 2);
								data_current = find_crlf + 2;
							}
						}
					}
				}
				else
				{
					// skip all server data
					AddContentItemSafe(
								ds,
								contentProtocolSpecific,
								data_current,
								std::distance(data_current, data_end)
								);
					data_current = data_end;
				}
				break;

			case psArticleResponse:

				if (ds == dsServer)
				{
					// check for end of line
					const char* find_crlf = std::search(data_current, data_end, CRLF, CRLF + 2);
					if (find_crlf == data_end)
					{
						// end of line was not found, can't recognize response in that case
						bStopProcessing = true;
					}
					else
					{
						int nResponseCode = GetServerResponseCode(data_current, find_crlf);
						if (nResponseCode == 220 || nResponseCode == 222)  // whole article or its body only
						{
							// server response code is OK - user can send article
							m_process_state = psArticleData;
							m_process_state_data = IProcessStateDataAP(new MessageBodyStateData(dsServer));
						}
						else
						{
							// some error occured, no article will be send
							m_process_state = psWait;
						}
						// send response
						AddContentItemSafe(
									ds,
									contentProtocolSpecific,
									data_current,
									std::distance(data_current, find_crlf + 2)
									);
						data_current = find_crlf + 2;
					}
				}
				else
				{
					// skip all client data
					AddContentItemSafe(
								ds,
								contentProtocolSpecific,
								data_current,
								std::distance(data_current, data_end)
								);
					data_current = data_end;
				}
				break;
			
			case psArticleData:

				if (ds == dsServer)
				{
					MessageBodyStateData* pStateData = static_cast<MessageBodyStateData*>(m_process_state_data.get());
					if (!pStateData)
						throw std::logic_error("State data is invalid.");
					// in this state we must collect post data for check
					// check for end of line
					const char* find_crlf = data_current;
					while (true)
					{
						find_crlf = std::search(data_current, data_end, CRLF, CRLF + 2);
						if (find_crlf == data_end)
						{
							// end of line was not found, can't recognize response in that case
							bStopProcessing = true;
							break;
						}
						else
						{
							// check for end of post
							if ((data_current + 1) == find_crlf && *data_current == '.')
							{
								if (!pStateData->DataEmpty())
									AddContentItemSafe(pStateData->GetContentItem());
								AddContentItemSafe(
											ds, 
											contentProtocolSpecific, 
											data_current, 
											std::distance(data_current, find_crlf + 2)
											);
								data_current = find_crlf + 2;
								m_process_state = psWait;
								m_process_state_data = IProcessStateDataAP();
								break;
							}
							else
							{
								if (*data_current == '.')
								{
									// if '.' is the first symbol in line, skip it (will be restored later in AssembleContent)
									++data_current;
								}
								pStateData->AppendData(data_current, find_crlf + 2);
								data_current = find_crlf + 2;
							}
						}
					}
				}
				else
				{
					// skip all client data
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
void NntpProtocolAnalyzerImpl::AssembleContent(IContentItem& content) const
{
	if (content.Type() == contentMime)
	{
		// replace dots at the beginning of lines on double dots
		ContentItem& content_ex = static_cast<ContentItem&>(content);
		content_ex.ReplaceData("\r\n.", 3, "\r\n..", 4);
	}
}

}  // namespace TrafficProtocoller


