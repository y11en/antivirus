/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	msghdr.hpp
*		\brief	
*		
*		\author ¬ладислав ќвчарик
*		\date	03.05.2005 14:52:31
*		
*		
*/		
#if !defined(_MESSAGE_HEADER_INCLUDED_H_)
#define _MESSAGE_HEADER_INCLUDED_H_
#pragma warning(disable:4761)
#include <boost/spirit/core.hpp>
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif - need recognize a single LF and ignore the leading CR
 *			ѕарсер, который пропускает лидирующий CR и считывает
 *			единственный LF, такой парсер необходим дл€ толерантности
 *			к уже существующим HTTP-серверам и клиентам. „асто *NIX
 *			присылают в качестве тернимального симфола дл€ message-header
 *			не пару CRLF, а просто LF. —мотри rfc2616 параграф 19.3
 */
struct eol_tolerant_parser
				: public boost::spirit::parser<eol_tolerant_parser>
{
    typedef eol_tolerant_parser self_t;

    eol_tolerant_parser() {}

    template <typename ScannerT>
    typename boost::spirit::parser_result<self_t, ScannerT>::type
    parse(ScannerT const& scan) const
    {
        typename ScannerT::iterator_t save = scan.first;
        int len = 0;
		bool tolerant = false;
		//skip CR
        if (!scan.at_end() && *scan == '\r')    // CR
        {
            ++scan;
            ++len;
        }
		//recognize LF
        if (!scan.at_end() && *scan == '\n')    // LF
        {
            ++scan;
            ++len;
			tolerant = true;
        }
        if (len && tolerant)
            return scan.create_match(len, boost::spirit::nil_t(), save, scan.first);
        return scan.no_match();
    }
};
///////////////////////////////////////////////////////////////////////////////
eol_tolerant_parser const eol_tolerant_p = eol_tolerant_parser();
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif -	need skip any character while not find a symbol ':'
 *			ќп€ть же возвраща€сь к толерантности, пытаемс€ удовлетворить сервер
 *			располагающийс€ по адресу www.altavista.com, который присылает
 *			ошибочный токен - Last-Modified, вставл€€ перед символом ':'
 *			символ '"'.
 */
///////////////////////////////////////////////////////////////////////////////
struct colon_tolerant_parser
	: boost::spirit::parser<colon_tolerant_parser>
{
	typedef colon_tolerant_parser self_t;

    colon_tolerant_parser() {}

    template <typename ScannerT>
    typename boost::spirit::parser_result<self_t, ScannerT>::type
    parse(ScannerT const& scan) const
    {
		typename ScannerT::iterator_t save = scan.first;
        int len = 0;
		for(; !scan.at_end() && *scan != ':'; ++scan, ++len);
		if(!scan.at_end() && *scan == ':')
		{
			++scan;
			++len;
			return scan.create_match(len, boost::spirit::nil_t(), save, scan.first);
		}
		return scan.no_match();
	}
};
///////////////////////////////////////////////////////////////////////////////
colon_tolerant_parser const colon_tolerant_p = colon_tolerant_parser();
///////////////////////////////////////////////////////////////////////////////
/**
 * BNF for message header
 * message-header	= field-name ':' [field-value]
 * field-name		= token
 * field-value		= *(field-content | LWS)
 * field-content	= text | quoted-string
 * token			= +(character in the range ['\x20', '\x7f'] )
 * text				= any 8-bit sequence of data except control character ['\x0', '\x1f'],
					  but including LWS
 * quoted-string	= '"' *(qdtext | quoted-pair) '"'
 * qdtext			= text except '\"'
 * quoted-pair		= '\' any character in the range ['\x0', '\x7f']
 */
struct message_header
	: public boost::spirit::grammar<message_header>
{
	template<typename scannerT>
    struct definition
    {
		definition(message_header const& self)
		{
			using namespace boost::spirit;
			lws = !eol_tolerant_p >> +((ch_p(' ') | ch_p('\t')));
			separator = ch_p('(') | ch_p(')') | ch_p('<')
						| ch_p('>') | ch_p('@') | ch_p(',')
						| ch_p(';') | ch_p(':') | ch_p('\\')
						| ch_p('\"') | ch_p('/') | ch_p('[')
						| ch_p(']') | ch_p('?') | ch_p('=')
						| ch_p('{') | ch_p('}') | ch_p(' ')
						| ch_p('\t');
			token = +(range_p('\x20', '\x7f') - separator);
			// fix 27514: сервер веб-камеры присылал в Content-Type 13 символов с кодами
			// 0x42 0x82 0xD2 0x83 0xC1 0x80 0x40 0x35 0xD0 0x91 0x12 0x10 0x02
			fldvalue = *(range<unsigned char>('\x00', '\xff') - (ch_p('\r') | ch_p('\n')));
			first = token
					>> colon_tolerant_p//! need be more tolerant than ch_p(':')
					>> !fldvalue;
		}
		boost::spirit::rule<scannerT> const& start() const { return first; }
		boost::spirit::rule<scannerT>	first;
		boost::spirit::rule<scannerT>	lws;		//!LWS
		boost::spirit::rule<scannerT>	separator;	//!separator
		boost::spirit::rule<scannerT>	token;		//!field-name
		boost::spirit::rule<scannerT>	fldvalue;	//!field-value
	};
};
///////////////////////////////////////////////////////////////////////////////
}//namespace http
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(_MESSAGE_HEADER_INCLUDED_H_)