/*
*/

#pragma warning( disable : 4786 )

#include "ImapMessageDataParser.h"
#include <string>
#include <locale>
#include <sstream>
#include "../common/DataBuffer.h"
#include "../common/StringUtil.h"

//#ifdef _DEBUG
//	#include <fstream>
//#endif

namespace TrafficProtocoller
{
namespace ImapUtil
{

//////////////////////////////////////////////////////////////////////////
class ImapMessageDataParser::IStateData
{
public:
	IStateData()  {}
	virtual ~IStateData()  {}
private:
	IStateData(const IStateData&);
	IStateData& operator = (const IStateData&);
};

namespace detail
{

//////////////////////////////////////////////////////////////////////////
class ParamNameStateData : public ImapMessageDataParser::IStateData
{
public:
	const std::string& name()								{ return m_name; }
	void append_name(char ch)								{ m_name += ch; }
	void append_name(const char* begin, const char* end)	{ m_name.append(begin, end); }
private:
	std::string m_name;
};

//////////////////////////////////////////////////////////////////////////
//class ParamValueStateData : public ImapMessageDataParser::IStateData
//{
//public:
//	const std::string& GetName()						{ return m_name; }
//	void AppendName(char ch)							{ m_name += ch; }
//	void AppendName(const char* begin, const char* end)	{ m_name.append(begin, end); }
//private:
//	std::string m_name;
//};

//////////////////////////////////////////////////////////////////////////
class WaitSymbolStateData : public ImapMessageDataParser::IStateData
{
public:
	WaitSymbolStateData(char ch) : m_symbol(ch)  {}
	char symbol()  { return m_symbol; }
private:
	char m_symbol;
};

//////////////////////////////////////////////////////////////////////////
class ParamValueBaseStateData : public ImapMessageDataParser::IStateData
{
public:
	ParamValueBaseStateData(const std::string& name) : m_name(name)  {}
	const std::string& name()  { return m_name; }
private:
	ParamValueBaseStateData();
	ParamValueBaseStateData(const ParamValueBaseStateData&);
	std::string m_name;
};

//////////////////////////////////////////////////////////////////////////
class ParamValueStringStateData : public ParamValueBaseStateData
{
	typedef ParamValueBaseStateData Base;
public:
	ParamValueStringStateData(const std::string& name, bool internal, char prev_symbol) 
		: Base(name)
		, m_prev(prev_symbol) 
		, m_internal(internal)
	{
	}
	bool is_internal() const	{ return m_internal; }
	char prev() const	{ return m_prev; }
	void prev(char ch)  { m_prev = ch; }
private:
	ParamValueStringStateData();
	bool m_internal;
	char m_prev;
};

//////////////////////////////////////////////////////////////////////////
class ParamValueListStateData : public ParamValueBaseStateData
{
	typedef ParamValueBaseStateData Base;
public:
	ParamValueListStateData(const std::string& name, bool internal, char prev_symbol) 
		: Base(name)
		, m_internal(internal)
		, m_prev_symbol(prev_symbol)  
	{
	}
	bool is_internal() const	{ return m_internal; }
	char prev_symbol() const	{ return m_prev_symbol; }
	void prev_symbol(char ch)	{ m_prev_symbol = ch; }
private:
	bool m_internal;
	char m_prev_symbol;
};

//////////////////////////////////////////////////////////////////////////
class ParamValueLiteralStateData : public ParamValueBaseStateData
{
	typedef ParamValueBaseStateData Base;
public:
	ParamValueLiteralStateData(
		const std::string& name,
		content_t content_type,
		size_t need_size
		) 
		: Base(name)
		, m_need_size(need_size)
		, m_content_item(new ContentItem(dsServer, content_type))
	{
	}
	ContentItemPtr content_item() const  { return m_content_item; }
	size_t need_size() const	{ return m_need_size; }
	void need_size(size_t size)	{ m_need_size = size; }
	const char* append(const char* begin, const char* end)		
	{ 
		if (m_need_size > 0)
		{
			size_t append_size = std::distance(begin, end);
			if (append_size > m_need_size)
				append_size = m_need_size;
			m_content_item->AppendData(begin, append_size);
			m_need_size -= append_size;
			return (begin + append_size);
		}
		else
		{
			return begin;
		}
	}
private:
	size_t m_need_size;
	ContentItemPtr m_content_item;
};

//////////////////////////////////////////////////////////////////////////
inline ContentItemPtr ToContentItem(const ParamValueLiteralStateData& literal)
{
	return literal.content_item();
}

//////////////////////////////////////////////////////////////////////////
inline ContentItemPtr ToContentItem(
	data_source_t ds, 
	content_t ct,
	const char* begin, 
	const char* end
	)
{
	return ContentItemPtr(new ContentItem(ds, ct, begin, std::distance(begin, end)));
}

}  // namespace detail

//////////////////////////////////////////////////////////////////////////
ImapMessageDataParser::ImapMessageDataParser()
{
	Reset();
}

//////////////////////////////////////////////////////////////////////////
void ImapMessageDataParser::Reset()
{
	m_parse_stack.clear();
	m_parse_stack.push_back(state_info(stateBegin));
	m_parse_result = parsingNotStarted;
}

//////////////////////////////////////////////////////////////////////////
const char* ImapMessageDataParser::Parse(
	const char* data_begin, 
	const char* data_end,
	ContentItemsList& content_items
	)
{
	using namespace detail;

	if (m_parse_stack.empty())
		throw std::logic_error("ImapMessageDataParser: parse stack is empty.");

	const char* data_current = data_begin;
	const char* last_content_item = data_begin;
	bool bError = false;

	while (data_current != data_end && !bError && !m_parse_stack.empty())
	{
		char ch = *data_current;
		bool bShiftPos = true;

		state_info state = m_parse_stack.back();

		switch (state.type)
		{
			case stateBegin:

				if (ch == '(')
				{
					m_parse_stack.push_back(
										state_info(
											stateParamName, 
											IStateDataPtr(new ParamNameStateData())
											)
										);
				}
				else if (ch == ')')
				{
					m_parse_stack.pop_back();
					m_parse_stack.push_back(
										state_info(
											stateWaitSymbol,
											IStateDataPtr(new WaitSymbolStateData('\n'))
											)
										);
					m_parse_stack.push_back(
										state_info(
											stateWaitSymbol,
											IStateDataPtr(new WaitSymbolStateData('\r'))
											)
										);
				}
				else
				{
					bError = true;
				}
				break;

			case stateWaitParamName:

				if (ch == ' ')
				{
				}
				else if (ch == ')')
				{
					m_parse_stack.pop_back();
					bShiftPos = false;
				}
				else
				{
					m_parse_stack.pop_back();
					m_parse_stack.push_back(
										state_info(
											stateParamName, 
											IStateDataPtr(new ParamNameStateData())
											)
										);
					bShiftPos = false;
				}
				break;

			case stateParamName:
				{
					ParamNameStateData* pStateData = static_cast<ParamNameStateData*>(state.data.get());
					if (ch == ')')
					{
						m_parse_stack.pop_back();
						bShiftPos = false;
					}
					else if (ch == ' ')
					{
						m_parse_stack.pop_back();
						m_parse_stack.push_back(
											state_info(
												stateRecognizeParamValue,
												state.data
												)
											);
					}
					else if (ch == '[')
					{
						if (pStateData)
							pStateData->append_name(ch);
						m_parse_stack.pop_back();
						m_parse_stack.push_back(state_info(stateExtParamName, state.data));
					}
					else
					{
						if (pStateData)
							pStateData->append_name(ch);
					}
				}
				break;

			case stateExtParamName:
				{
					ParamNameStateData* pStateData = static_cast<ParamNameStateData*>(state.data.get());
					if (pStateData)
						pStateData->append_name(ch);
					if (ch == ']')
					{
						m_parse_stack.pop_back();
						m_parse_stack.push_back(state_info(statePartialDescriptor, state.data));
					}
				}
				break;
			case statePartialDescriptor:
				{
					// ожидаем возможность описания куска сообщения, например: "<10>"
					// ( как в ответе: "* 2 fetch (body[2]<10> {70} ....)" )
					
					ParamNameStateData* pStateData = static_cast<ParamNameStateData*>(state.data.get());
					
					if (ch == '<')
					{
						// действительно описание куска данных - пропускаем
					}
					else
					if (ch == ' ')
					{
						m_parse_stack.pop_back();
						m_parse_stack.push_back(state_info(stateRecognizeParamValue, state.data));
					}
					else
					if (ch == '>')
					{
						m_parse_stack.pop_back();
						m_parse_stack.push_back(state_info(stateRecognizeParamValue, state.data));
						m_parse_stack.push_back(state_info(stateWaitSymbol,IStateDataPtr(new WaitSymbolStateData(' '))));
					}
				}
				break;

			case stateWaitSymbol:
				{
					WaitSymbolStateData* pStateData = static_cast<WaitSymbolStateData*>(state.data.get());
					if (!pStateData)
						throw std::logic_error("State data is invalid");
					if (ch == pStateData->symbol())
					{
						m_parse_stack.pop_back();
					}
					else
					{
//#ifdef _DEBUG
//						std::ofstream file("c:\\imap_parse.txt");
//						for (const char* it = data_begin; it != data_end; ++it)
//						{
//							if (it == data_current)
//								file << "<[HERE]>";
//							file << *it;
//						}
//#endif
						bError = true;
					}
				}
				break;

			case stateRecognizeParamValue:
				{
					ParamNameStateData* pStateData = static_cast<ParamNameStateData*>(state.data.get());
					m_parse_stack.pop_back();
					if (ch == '(')
					{
						m_parse_stack.push_back(
											state_info(
												stateParamValueList,
												IStateDataPtr(new ParamValueListStateData(pStateData->name(), false, ch))
												)
											);
					}
					else if (ch == '"')
					{
						m_parse_stack.push_back(
											state_info(
												stateParamValueString, 
												IStateDataPtr(new ParamValueStringStateData(pStateData->name(), false, ch))
												)
											);
					}
					else if (ch == '{')
					{
						if (last_content_item != data_current)
						{
							content_items.push_back(
											ToContentItem(
													dsServer, 
													contentProtocolSpecific, 
													last_content_item, 
													data_current
													)
											);
							last_content_item = data_current;
						}
						const std::string& sParamName = pStateData->name();
						content_t ct = contentProtocolSpecific;
						//if (Util::EqualNoCase(sParamName, "RFC822"))
						if (Util::FindNoCase(sParamName, std::string("RFC822")))
						{
							ct = contentMime;
						}
						else if (sParamName.size() >= 6 &&
								 Util::EqualNoCase(sParamName.substr(0, 5), "BODY[") &&
								 sParamName[sParamName.size() - 1] == ']')
						{
							std::string sSection = sParamName.substr(5, sParamName.size() - 6);
							if (sSection.empty() || Util::EqualNoCase(sSection, "TEXT"))
							{
								ct = contentMime;
							}
							else if (sSection.find_first_not_of("0123456789") == std::string::npos)
							{
								// запрашивается раздел письма, как правило вложение
								ct = contentPartition;  // content type can't be determined sharply
							}
						}
						m_parse_stack.push_back(
											state_info(
												stateParamValueLiteral, 
												IStateDataPtr(new ParamValueLiteralStateData(pStateData->name(), ct, 0))
												)
											);
					}
					else
					{
						m_parse_stack.push_back(state_info(stateParamValueSimple));
						bShiftPos = false;
					}
				}
				break;

			case stateParamValueList:
				{
					ParamValueListStateData* pStateData = static_cast<ParamValueListStateData*>(state.data.get());
					if (!pStateData)
						throw std::logic_error("State data is invalid");
					if (ch == ')')
					{
						m_parse_stack.pop_back();
						if (!pStateData->is_internal())
							m_parse_stack.push_back(state_info(stateWaitParamName));
					}
					else if (ch == '(')
					{
						m_parse_stack.push_back(
											state_info(
												stateParamValueList,
												IStateDataPtr(new ParamValueListStateData(std::string(), true, ch))
												)
											);
					}
					else
					{
						// skip all list content
					}
				}
				break;

			case stateParamValueString:
				{
					ParamValueStringStateData* pStateData = static_cast<ParamValueStringStateData*>(state.data.get());
					if (!pStateData)
						throw std::logic_error("State data is invalid");
					if (ch == '"' && pStateData->prev() != '\\')
					{
						// string finished
						m_parse_stack.pop_back();
						if (!pStateData->is_internal())
							m_parse_stack.push_back(state_info(stateWaitParamName));
					}
					else
					{
						if (ch == '\\' && pStateData->prev() == '\\')
							pStateData->prev(' ');  // current backslash is quoted by previous, use neutral symbol as next used previous
						else
							pStateData->prev(ch);
					}
				}
				break;

			case stateParamValueSimple:
				if (ch == ' ' || ch == ')')
				{
					m_parse_stack.pop_back();
					m_parse_stack.push_back(state_info(stateWaitParamName));
					bShiftPos = false;
				}
				else
				{
				}
				break;

			case stateParamValueLiteral:
				{
					ParamValueLiteralStateData* pStateData = static_cast<ParamValueLiteralStateData*>(state.data.get());
					if (!pStateData)
						throw std::logic_error("State data is invalid");
					if (ch == '}')
					{
						// literal size finished
						
						// if it is protocol specific literal, add size of data 
						// (in other case it will be added when will be assembled)
						ContentItemPtr pItem = pStateData->content_item();
						if (pItem->Type() == contentProtocolSpecific)
						{
							std::ostringstream os;
							os << '{' << pStateData->need_size() << "}\r\n";
							std::string sData;
							os.str().swap(sData);
							pItem->PushDataBack(sData.c_str(), sData.size());
						}
						m_parse_stack.pop_back();
						m_parse_stack.push_back(state_info(stateReadLiteralData, state.data));
						m_parse_stack.push_back(
											state_info(
												stateWaitSymbol,
												IStateDataPtr(new WaitSymbolStateData('\n'))
												)
											);
						m_parse_stack.push_back(
											state_info(
												stateWaitSymbol,
												IStateDataPtr(new WaitSymbolStateData('\r'))
												)
											);
					}
					else if (std::isdigit(ch, std::locale::classic()))
					{
						size_t cur = 10*pStateData->need_size() + (ch - '0');
						pStateData->need_size(cur);
					}
					else
					{
						bError = true;
					}
				}
				break;

			case stateReadLiteralData:
				{
					ParamValueLiteralStateData* pStateData = static_cast<ParamValueLiteralStateData*>(state.data.get());
					if (!pStateData)
						throw std::logic_error("State data is invalid");
					data_current = pStateData->append(data_current, data_end);
					bShiftPos = false;
					if (pStateData->need_size() == 0)
					{
						m_parse_stack.pop_back();
						m_parse_stack.push_back(state_info(stateCheckContinueLiteralDataSpace, state.data));
					}
				}
				break;

			case stateCheckContinueLiteralDataSpace:
				{
					last_content_item = data_current;
					if (ch == ' ')
					{
						m_parse_stack.pop_back();
						m_parse_stack.push_back(state_info(stateCheckContinueLiteralDataBrace, state.data));
					}
					else
					{
						// add created literal to content items list
						ParamValueLiteralStateData* pStateData = static_cast<ParamValueLiteralStateData*>(state.data.get());
						content_items.push_back(ToContentItem(*pStateData));
						last_content_item = data_current;
						// update stack
						m_parse_stack.pop_back();
						m_parse_stack.push_back(state_info(stateWaitParamName));
						bShiftPos = false;
					}
				}
				break;

			case stateCheckContinueLiteralDataBrace:
				{
					if (ch == '{')
					{
						m_parse_stack.pop_back();
						m_parse_stack.push_back(state_info(stateParamValueLiteral, state.data));
					}
					else
					{
						// add created literal to content items list
						ParamValueLiteralStateData* pStateData = static_cast<ParamValueLiteralStateData*>(state.data.get());
						content_items.push_back(ToContentItem(*pStateData));
						last_content_item = data_current - 1; // exclude previous space
						// update stack
						m_parse_stack.pop_back();
						m_parse_stack.push_back(state_info(stateWaitParamName));
						bShiftPos = false;
					}
				}
				break;
		
		}  // switch (state.type)

		if (bShiftPos)
			++data_current;

	}  // while (data_current != data_end && !bError && !m_parse_stack.empty())

	// add rest data content item, if neeeded
	{
		bool bAddItem = true;
		if (!bError && !m_parse_stack.empty())
		{
			const state_info& state = m_parse_stack.back();
			if (state.type == stateParamValueLiteral ||
				state.type == stateReadLiteralData ||
				state.type == stateCheckContinueLiteralDataSpace ||
				state.type == stateCheckContinueLiteralDataBrace)
			{
				bAddItem = false;
			}
		}
		if (bAddItem)
		{
			content_items.push_back(
						ToContentItem(
								dsServer, 
								contentProtocolSpecific, 
								last_content_item, 
								data_current
								)
						);
		}
	}

	m_parse_result = (
						bError 
							? parsingFailed 
							: (m_parse_stack.empty() ? parsingOK : parsingContinues)
					 );

	return data_current;
}


}  // namespace ImapUtil
}  // namespace TrafficProtocoller
	
