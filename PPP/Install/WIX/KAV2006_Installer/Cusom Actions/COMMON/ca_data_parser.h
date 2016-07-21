/*
*/

#if !defined(__MSISUPPORT_CA_DATA_PARSER_H)
#define __MSISUPPORT_CA_DATA_PARSER_H

//////////////////////////////////////////////////////////////////////////
#include <string>
#include <map>

//////////////////////////////////////////////////////////////////////////
namespace MsiSupport
{
namespace Utility
{

//////////////////////////////////////////////////////////////////////////
// CustomActionData parser
// Data example: aaa=value1;bbb="'value2'";ccc='"value2"'
//////////////////////////////////////////////////////////////////////////
template <typename char_type>
class CADataParser
{
public:
	typedef char_type char_t;
	typedef std::basic_string<char_t> string_t;
	typedef std::map<string_t, string_t> values_map_t;
public:
	CADataParser() : m_error(false) {}
	bool error()  { return m_error; }
	bool parse(const string_t& s)  
	{ 
		if (s.empty())
		{
			m_values.clear();
			m_error = false;
			return true;
		}
		else
		{
			return parse(s.c_str(), s.c_str() + s.size()); 
		}
	}
	bool parse(const char_t* begin, const char_t* end);
	const string_t& value(const string_t& name)
	{
		values_map_t::const_iterator it = m_values.find(name);
		return (it == m_values.end() ? m_empty_string : it->second);
	}
	const values_map_t& values()  { return m_values; }
private:
	bool m_error;
	values_map_t m_values;
	const string_t m_empty_string;
};

//////////////////////////////////////////////////////////////////////////
template <typename char_type>
bool CADataParser<char_type>::parse(const char_t* begin, const char_t* end)
{
	m_values.clear();
	
	const char_t* current = begin;
	string_t sName, sValue;
	enum State
	{
		stateWait,
		stateWaitComma,
		stateName,
		stateValue,
        /*
		stateSimpleValue,
		stateQuotedValue,
		stateDoubleQuotedValue,
        stateBracedValue,
        */
		stateError
	};
	State state = stateWait;
	
	while (current != end && state != stateError)
	{
		char_t ch = *current;
		switch(state)
		{
			case stateWait:

				if (ch == char_t(' ') || ch == char_t('\t') || ch == char_t(';'))
				{
					// skip whitespaces & commas
				}
				else if (ch == char_t('='))
				{
					state = stateError;
				}
				else
				{
					// name starts
					sName = ch;
					string_t().swap(sValue);
					state = stateName;
				}
				break;

			case stateWaitComma:

				if (ch == char_t(' ') || ch == char_t('\t'))
				{
					// skip whitespaces
				}
				else if (ch == char_t(';'))
				{
					state = stateWait;
				}
				else
				{
					state = stateError;
				}
				break;

			case stateName:

				if (ch == char_t('='))
				{
					state = stateValue;
				}
				else
				{
					sName += ch;
				}
				break;

			case stateValue:

				/*
                if (ch == char_t('\''))
				{
					state = stateQuotedValue;
				}
				else if (ch == char_t('"'))
				{
					state = stateDoubleQuotedValue;
				}
                else if (ch == char_t('{'))
                {
                    state = stateBracedValue;
                }
				else if (ch == char_t(' ') || ch == char_t('\t'))
				{
					// skip whitespaces
				}
				else if (ch == char_t(';'))
				{
					m_values.insert(values_map_t::value_type(sName, sValue));
					state = stateWait;
				}
                */
				if (ch == char_t('|'))
				{
					m_values.insert(values_map_t::value_type(sName, sValue));
					state = stateWait;
				}
				else
				{
					sValue += ch;
				}
				break;

            /*
			case stateSimpleValue:

				if (ch == char_t(' ') || ch == char_t('\t'))
				{
					m_values.insert(values_map_t::value_type(sName, sValue));
					state = stateWaitComma;
				}
				else if (ch == char_t(';'))
				{
					m_values.insert(values_map_t::value_type(sName, sValue));
					state = stateWait;
				}
				else
				{
					sValue += ch;
				}
				break;

			case stateQuotedValue:

				if (ch == char_t('\''))
				{
					m_values.insert(values_map_t::value_type(sName, sValue));
					state = stateWaitComma;
				}
				else
				{
					sValue += ch;
				}
				break;

			case stateDoubleQuotedValue:

				if (ch == char_t('"'))
				{
					m_values.insert(values_map_t::value_type(sName, sValue));
					state = stateWaitComma;
				}
				else
				{
					sValue += ch;
				}
				break;

            case stateBracedValue:

                if (ch == char_t('}'))
                {
                    m_values.insert(values_map_t::value_type(sName, sValue));
                    state = stateWaitComma;
                }
                else
                {
                    sValue += ch;
                }
                break;
            */

		}  // switch(state)
		
		++current;
	}

	if (state == stateWait || state == stateWaitComma)
	{
		m_error = false;
	}
	else if (state == stateValue /*|| state == stateSimpleValue*/)
	{
		m_values.insert(values_map_t::value_type(sName, sValue));
		m_error = false;
	}
	else
	{
		m_error = true;
	}

	return (!m_error);
}

}  // namespace Utility
}  // namespace MsiSupport

#endif  // !defined(__MSISUPPORT_CA_DATA_PARSER_H)
