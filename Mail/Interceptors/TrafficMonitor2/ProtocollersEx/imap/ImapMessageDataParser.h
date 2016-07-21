#if !defined(__KL_IMAPMESSAGEDATAPARSER_H)
#define __KL_IMAPMESSAGEDATAPARSER_H

#include <deque>
#include <boost/shared_ptr.hpp>
#include "../common/ContentItem.h"

namespace TrafficProtocoller
{
namespace ImapUtil
{

//////////////////////////////////////////////////////////////////////////
class ImapMessageDataParser
{
public:
	ImapMessageDataParser();
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
	typedef boost::shared_ptr<IStateData> IStateDataPtr;
private:
	enum state_type_enum
	{
		stateBegin = 0,
		stateWaitParamName,
		stateParamName,
		stateExtParamName,
		stateWaitSymbol,
		stateRecognizeParamValue,
		stateParamValueList,
		stateParamValueListEnd,
		stateParamValueString,
		stateParamValueSimple,
		stateParamValueLiteral,
		stateReadLiteralData,
		stateCheckContinueLiteralDataSpace,
		stateCheckContinueLiteralDataBrace,
		statePartialDescriptor
	};
	struct state_info
	{
		state_info(state_type_enum new_type)
			: type(new_type)
		{
		}
		state_info(state_type_enum new_type, IStateDataPtr new_data)
			: type(new_type)
			, data(new_data)
		{
		}
		state_type_enum type;
		IStateDataPtr data;
	private:
		state_info();
	};
private:
	parse_result_enum m_parse_result;
	std::deque<state_info> m_parse_stack;
};

}  // namespace ImapUtil
}  // namespace TrafficProtocoller
	
#endif  // !defined(__KL_IMAPMESSAGEDATAPARSER_H)
