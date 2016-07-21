#if !defined(__KL_IMAPPARSEUTILS_H)
#define __KL_IMAPPARSEUTILS_H

#include <memory>
#include <string>
#include <vector>
#include "../common/StringUtil.h"
#include "../common/ContentItem.h"
#include "../common/ParseStateStack.h"

namespace TrafficProtocoller
{
namespace ImapUtil
{

class EnvelopeData;
class EnvelopeDataParser;

//////////////////////////////////////////////////////////////////////////
inline bool IsUntaggedResponse(const std::string& sData)
{
	return (sData.size() >= 2 && sData[0] == '*' && sData[1] == ' ');
}

//////////////////////////////////////////////////////////////////////////
inline bool IsUntaggedResponse(const char* pData, size_t nDataSize)
{
	return (pData && nDataSize >= 2 && pData[0] == '*' && pData[1] == ' ');
}

//////////////////////////////////////////////////////////////////////////
inline bool IsUntaggedResponse(const char* data_begin, const char* data_end)
{
	return (data_begin != data_end && *data_begin == '*' &&
			(data_begin + 1) != data_end && *(data_begin + 1) == ' ');
}

//////////////////////////////////////////////////////////////////////////
inline bool IsCommandContinueResponse(const std::string& sData)
{
	return (
		(sData.size() >= 2 && sData[0] == '+' && sData[1] == ' ') ||
		(sData.size() == 3 && sData[0] == '+' && sData[1] == '\r' && sData[1] == '\n')
		);
}

//////////////////////////////////////////////////////////////////////////
bool ParseCommandResponse(
				const std::string& sData, 
				std::string& sId,
				std::string& sTag
				);
bool RetrieveUntaggedResponseItem(
				const char* pData, 
				size_t nDataSize, 
				size_t nIndex,
				std::string& sTag
				);
bool ParseUntaggedResponse(
				const std::string& sData, 
				std::string& sResponseTag, 
				std::string& sResponseData
				);
const char* ParseUntaggedResponse(
				const char* data_begin, 
				const char* data_end, 
				std::vector<std::string>& aItems, 
				size_t nMaxCount = 0
				);
bool ParseCommandRequest(
				const std::string& sData, 
				std::string& sId,
				std::string& sTag, 
				std::string& sParams
				);
long GetAppendCommandDataSize(const std::string& sParams);
size_t SplitStringOfAtoms(const std::string& sAtoms, std::vector<std::string>& vecAtoms);
bool StringOfAtomsIsContain(const std::string& atoms_all, const char* atom);

//////////////////////////////////////////////////////////////////////////
class EnvelopeData
{
	friend class EnvelopeDataParser;
public:
	EnvelopeData();
	void SerializeToMimeHeader(std::ostream& os) const;
	void DeserializeFromMimeHeader(const char* begin, const char* end);
	void SerializeToImapFormat(std::ostream& os) const;
private:
	EnvelopeData(const EnvelopeData&);
	EnvelopeData& operator = (const EnvelopeData&);
	class AddressList;
	class AddressListEntry;
	class Address;
	class Group;
	std::string m_date;
	std::string m_subject;
	std::auto_ptr<AddressList> m_from;
	std::auto_ptr<AddressList> m_sender;
	std::auto_ptr<AddressList> m_reply_to;
	std::auto_ptr<AddressList> m_to;
	std::auto_ptr<AddressList> m_cc;
	std::auto_ptr<AddressList> m_bcc;
	std::string m_in_reply_to;
	std::string m_message_id;
};

//////////////////////////////////////////////////////////////////////////
class EnvelopeDataParser
{
public:
	EnvelopeDataParser();
	void Reset();
	const char* Parse(const char* data_begin, const char* data_end, ContentItemsList& content_items);
	enum parse_result_enum
	{
		parsingFailed = 0,
		parsingNotStarted,
		parsingOK,
		parsingContinues
	};
	parse_result_enum ParseResult() const  { return m_parse_result; }
public:
	class IStateData;
private:
	enum parse_state_enum
	{
		stateBegin,
		stateWaitSpace,
		stateWaitItem,
		stateLiteral,
		stateString,
		stateAddressBegin,
		stateWaitFinish,
		stateError
	};
	typedef Util::ParseStateStack<parse_state_enum, IStateData> ParseStateStack;
	enum ItemEnum
	{
		itemNone = 0,
		itemDate,
		itemSubject,
		itemFrom,
		itemSender,
		itemReplyTo,
		itemTo,
		itemCc,
		itemBcc,
		itemInReplyTo,
		itemMessageId,
		itemExtra
	};
private:
	static ItemEnum GetNextItem(ItemEnum current);
	bool AssignItemValue(ItemEnum item, const std::string& value);
private:
	parse_result_enum m_parse_result;
	ParseStateStack m_parse_stack;
	ItemEnum m_current_item;
	std::auto_ptr<EnvelopeData> m_parsed_data;
};

}  // namespace ImapUtils
}  // namespace TrafficProtocoller

#endif
