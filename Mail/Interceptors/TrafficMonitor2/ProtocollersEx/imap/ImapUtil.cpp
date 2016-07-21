/*
*/

#include "ImapUtil.h"
#include <algorithm>

#pragma warning( disable : 4786 )

namespace TrafficProtocoller
{
namespace ImapUtil
{

//////////////////////////////////////////////////////////////////////////
bool ParseCommandResponse(
	const std::string& sData, 
	std::string& sId,
	std::string& sTag
	)
{
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool RetrieveUntaggedResponseItem(
	const char* pData, 
	size_t nDataSize, 
	size_t nIndex,
	std::string& sTag
	)
{
	using namespace std;
	
	if (!IsUntaggedResponse(pData, nDataSize))
		return false;
	
	size_t count = 0;
	const char* pDataEnd = pData + nDataSize; 
	const char* pPrev = NULL; 
	const char* pNext = pData + 2; 
	
	while (pNext != pDataEnd && count <= nIndex)
	{
		pPrev = pNext;
		pNext = std::find(pPrev, pDataEnd, ' ');
		if (pNext == pDataEnd && count != nIndex)
			return false;
		if (count == nIndex)
		{
			sTag.assign(pPrev, pNext);
			return true;
		}
		++pNext;
		++count;
	}
	
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool ParseUntaggedResponse(
	const std::string& sData, 
	std::string& sResponseTag, 
	std::string& sResponseData
	)
{
	using namespace std;
	if (!IsUntaggedResponse(sData))
		return false;
	string::size_type posSpace = sData.find(' ', 2);
	if (posSpace == string::npos)
	{
		sResponseTag.swap(sData.substr(2));
	}
	else
	{
		sResponseTag.swap(sData.substr(2, posSpace - 2));
		sResponseData.swap(sData.substr(posSpace + 1));
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
const char* ParseUntaggedResponse(
	const char* data_begin, 
	const char* data_end, 
	std::vector<std::string>& aItems, 
	size_t nMaxCount
	)
{
	if (!IsUntaggedResponse(data_begin, data_end))
		return 0;
	aItems.clear();
	const char* data_current = data_begin + 2;
	while (data_current != data_end && (nMaxCount == 0 || aItems.size() < nMaxCount))
	{
		const char* find_space = std::find(data_current, data_end, ' ');
		aItems.push_back(std::string(data_current, find_space));
		data_current = (find_space == data_end ? data_end : (find_space + 1));
	}
	return data_current;
}

//////////////////////////////////////////////////////////////////////////
//bool ParseUntaggedResponse(
//	const char* data_begin, 
//	const char* data_end, 
//	std::vector<std::string>& dest 
//	)
//{
//	using namespace std;
//	if (!IsUntaggedResponse(data_begin, data_end))
//		return false;
//	string::size_type posSpace = sData.find(' ', 2);
//	if (posSpace == string::npos)
//	{
//		sResponseTag.swap(sData.substr(2));
//	}
//	else
//	{
//		sResponseTag.swap(sData.substr(2, posSpace - 2));
//		sResponseData.swap(sData.substr(posSpace + 1));
//	}
//	return true;
//}

//////////////////////////////////////////////////////////////////////////
bool ParseCommandRequest(
	const std::string& sData, 
	std::string& sId,
	std::string& sTag, 
	std::string& sParams
	)
{
	using std::string;

	if (sData.empty())
		return false;

	string::size_type pos_last = sData.rfind("\r\n");
	if (pos_last == string::npos)
		pos_last = sData.size() - 1;
	else
		--pos_last;

	string::size_type pos1 = sData.find(' ');
	if (pos1 != string::npos)
	{
		string::size_type pos2 = sData.find(' ', pos1 + 1);
		if (pos2 == string::npos)
		{
			sTag.swap(sData.substr(pos1 + 1, pos_last - pos1));
			string().swap(sParams);
		}
		else
		{
			sTag.swap(sData.substr(pos1 + 1, pos2 - pos1 - 1));
			sParams.swap(sData.substr(pos2 + 1, pos_last - pos2));
		}
		sId.swap(sData.substr(0, pos1));
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
long GetAppendCommandDataSize(const std::string& sParams)
{
	using std::string;
	unsigned long result = 0L;
	if (!sParams.empty())
	{
		string::size_type pos = sParams.rfind(' ');
		if (pos != string::npos)
		{
			string sCount(sParams.substr(pos + 1));
			if (sCount.size() > 2 && 
				sCount[0] == '{' &&
				sCount[sCount.size() - 1] == '}')
			{
				result = atol(sCount.substr(1, sCount.size() - 2).c_str());
			}
		}
	}
	return result;
}

//////////////////////////////////////////////////////////////////////////
size_t SplitStringOfAtoms(
	const std::string& sAtoms, 
	std::vector<std::string>& vecAtoms
	)
{
	using namespace std;
	size_t count = 0;
	if (!sAtoms.empty())
	{
		string::size_type prev = 0, next = 0;
		while (next != string::npos)
		{
			prev = next;
			next = sAtoms.find(' ', prev);
			std::string sAtom;
			if (next == string::npos)
			{
				sAtom.swap(sAtoms.substr(prev));
			}
			else
			{
				sAtom.swap(sAtoms.substr(prev, next - prev));
				++next;
				if (next >= sAtoms.size())
					next = string::npos;
			}
			if (!sAtom.empty())
			{
				vecAtoms.push_back(sAtom);
				++count;
			}
		}
	}
	return count;	
}

//////////////////////////////////////////////////////////////////////////
bool StringOfAtomsIsContain(const std::string& atoms_all, const char* atom)
{
	using namespace std;
	if (!atoms_all.empty())
	{
		string::size_type prev = 0, next = 0;
		while (next != string::npos)
		{
			prev = next;
			next = atoms_all.find(' ', prev);
			std::string sAtom;
			if (next == string::npos)
			{
				if (Util::EqualNoCase(atoms_all.substr(prev), atom))
					return true;
			}
			else
			{
				if (Util::EqualNoCase(atoms_all.substr(prev, next - prev), atom))
					return true;
				++next;
				if (next >= atoms_all.size())
					next = string::npos;
			}
		}
	}
	return false;	
}

//////////////////////////////////////////////////////////////////////////
class EnvelopeData::AddressListEntry
{
public:
	AddressListEntry()  {}
	virtual ~AddressListEntry()  {}
};

//////////////////////////////////////////////////////////////////////////
class EnvelopeData::Address : public AddressListEntry
{
	friend class AddressList;
	typedef AddressListEntry Base;
public:
	Address(
		const std::string& name,
		const std::string& route,
		const std::string& mailbox,
		const std::string& host
		)
		: m_name(name)
		, m_route(route)
		, m_mailbox(mailbox)
		, m_host(host)
	{
	}
	const std::string& Name()  { return m_name; }
	const std::string& Route()  { return m_route; }
	const std::string& Mailbox()  { return m_mailbox; }
	const std::string& Host()  { return m_host; }
private:
	std::string m_name;
	std::string m_route;
	std::string m_mailbox;
	std::string m_host;
};

//////////////////////////////////////////////////////////////////////////
class EnvelopeData::Group : public AddressListEntry
{
	friend class AddressList;
	typedef AddressListEntry Base;
public:
	Group(const std::string& name) : m_name(name)  {}
	void ClearAddresses()  { m_addresses.clear(); }
	void AddAddress(const Address& addr)  { m_addresses.push_back(addr); }
private:
	std::string m_name;
	std::list<Address> m_addresses;
};

//////////////////////////////////////////////////////////////////////////
class EnvelopeData::AddressList
{
public:
	typedef boost::shared_ptr<AddressListEntry> EntryPtr;
public:
	void SerializeToMimeHeader(std::string& dest) const;
	void DeserializeFromMimeHeader(const std::string& src);
	void SerializeToImapFormat(std::string& dest) const;
private:
	std::list<EntryPtr> m_entries;
};

//////////////////////////////////////////////////////////////////////////
EnvelopeData::EnvelopeData()
{
}

//////////////////////////////////////////////////////////////////////////
void EnvelopeData::SerializeToMimeHeader(std::ostream& os) const
{
}

//////////////////////////////////////////////////////////////////////////
void EnvelopeData::DeserializeFromMimeHeader(const char* begin, const char* end)
{
	Group gr("som");
	gr.AddAddress(Address("", "", "", ""));
}

//////////////////////////////////////////////////////////////////////////
void EnvelopeData::SerializeToImapFormat(std::ostream& os) const
{
//private:
//	class Address;
//	std::string m_date;
//	std::string m_subject;
//	Address m_from;
//	Address m_sender;
//	Address m_reply_to;
//	Address m_to;
//	Address m_cc;
//	Address m_bcc;
//	std::string m_in_reply_to;
//	std::string m_message_id;
//};

}

//////////////////////////////////////////////////////////////////////////
class EnvelopeDataParser::IStateData
{
public:
	virtual ~IStateData()  {}
};

//////////////////////////////////////////////////////////////////////////
class EnvelopeStateDataLiteral : public EnvelopeDataParser::IStateData
{
public:
	EnvelopeStateDataLiteral()  {}
	EnvelopeStateDataLiteral(char ch) : m_data(ch, 1)  {}
	void Add(char ch)  { m_data += ch; }
	const std::string& Data() const  { return m_data; }
private:
	std::string m_data;
};

//////////////////////////////////////////////////////////////////////////
class EnvelopeStateDataString : public EnvelopeStateDataLiteral
{
	typedef EnvelopeStateDataLiteral Base;
public:
	EnvelopeStateDataString() : Base(), m_masked(false)  {}
	EnvelopeStateDataString(char ch) : Base(ch), m_masked(false)  {}
	bool MaskedFlag() const  { return m_masked; }
	void MaskedFlag(bool masked)  { m_masked = masked; }
private:
	bool m_masked;
};

//////////////////////////////////////////////////////////////////////////
EnvelopeDataParser::EnvelopeDataParser()
	: m_parse_result(parsingNotStarted)
	, m_current_item(itemNone)
	, m_parsed_data(new EnvelopeData())
{
	m_parse_stack.Push(stateBegin);
}

//////////////////////////////////////////////////////////////////////////
void EnvelopeDataParser::Reset()
{
	m_parse_result = parsingNotStarted;
	m_parse_stack.Clear();
	m_parse_stack.Push(stateBegin);
	m_current_item = itemNone;
	m_parsed_data = std::auto_ptr<EnvelopeData>(new EnvelopeData());
}

//////////////////////////////////////////////////////////////////////////
EnvelopeDataParser::ItemEnum EnvelopeDataParser::GetNextItem(ItemEnum current)
{
	return (current >= itemExtra 
				? itemExtra 
				: static_cast<ItemEnum>(current+1));
}

//////////////////////////////////////////////////////////////////////////
bool EnvelopeDataParser::AssignItemValue(ItemEnum item, const std::string& value)
{
	switch (item)
	{
		case itemDate:
			m_parsed_data->m_date = value;
			break;
		case itemSubject:
			m_parsed_data->m_subject = value;
			break;
		case itemInReplyTo:
			m_parsed_data->m_in_reply_to = value;
			break;
		case itemMessageId:
			m_parsed_data->m_message_id = value;
			break;
		default:
			return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
const char* EnvelopeDataParser::Parse(
	const char* data_begin, 
	const char* data_end, 
	ContentItemsList& content_items
	)
{
	const char* data_current = data_begin;
	bool bError = false;

	while (data_current != data_end && !bError && !m_parse_stack.Empty())
	{
		char ch = *data_current;
		bool bShiftPos = true;

		ParseStateStack::StateInfo state = m_parse_stack.Peek();

		switch (state.type)
		{
			case stateBegin:
			
				if (ch == '(')
				{
					m_parse_stack.Push(stateWaitItem);
				}
				else if (ch == ')')
				{
					m_parse_stack.Pop();
				}
				else
				{
					bError = true;
				}
				break;

			case stateWaitItem:
				
				if (ch == ')')
				{
					m_current_item = itemExtra;
					m_parse_stack.Pop();
					bShiftPos = false;
				}
				else if (ch == ' ')
				{
				}
				else 
				{
					if (ch == '"')
					{
						m_parse_stack.Pop();
						m_parse_stack.Push(stateString, ParseStateStack::IStateDataPtr(new EnvelopeStateDataLiteral(ch)));
					}
					else if (ch == '(')
					{
						m_parse_stack.Pop();
						m_parse_stack.Push(stateAddressBegin);
					}
					else
					{
						m_parse_stack.Pop();
						m_parse_stack.Push(stateLiteral, ParseStateStack::IStateDataPtr(new EnvelopeStateDataString()));
					}
				}
				break;

			case stateLiteral:

				{
					EnvelopeStateDataLiteral* pStateData = static_cast<EnvelopeStateDataLiteral*>(state.data.get());
					if (!pStateData)
						throw std::logic_error("State data is invalid.");
					bool bValueCompleted = false;				
					if (ch == ')')
					{
						m_current_item = itemExtra;
						m_parse_stack.Pop();
						bShiftPos = false;
						bValueCompleted = true;
					}
					else if (ch == ' ')
					{
						m_parse_stack.Pop();
						m_parse_stack.Push(stateWaitItem);
						bValueCompleted = true;
					}
					else
					{
						pStateData->Add(ch);
					}
					if (bValueCompleted)
					{
						AssignItemValue(m_current_item, pStateData->Data());
						m_current_item = GetNextItem(m_current_item);
					}
				}
				break;

			case stateString:

				{
					EnvelopeStateDataString* pStateData = static_cast<EnvelopeStateDataString*>(state.data.get());
					if (!pStateData)
						throw std::logic_error("State data is invalid.");
					if (pStateData->MaskedFlag())
					{
						pStateData->Add(ch);
						pStateData->MaskedFlag(false);
					}
					else if (ch == '"')
					{
						AssignItemValue(m_current_item, pStateData->Data());
						m_current_item = GetNextItem(m_current_item);
						m_parse_stack.Pop();
					}
					else if (ch == '\\')
					{
						pStateData->MaskedFlag(true);
					}
					else
					{
						pStateData->Add(ch);
					}
				}
				break;

			case stateAddressBegin:
				break;

		}  // switch (state.type)

		if (bShiftPos)
			++data_current;
	}

	return data_current;
}

}  // namespace ImapUtils
}  // namespace TrafficProtocoller

