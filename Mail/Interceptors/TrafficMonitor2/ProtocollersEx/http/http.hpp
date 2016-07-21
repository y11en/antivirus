/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	http.hpp
*		\brief	
*		
*		\author Владислав Овчарик
*		\date	27.04.2005 14:27:06
*		
*		
*/		
#if !defined(_HTTP_INCLUDED_HPP_)
#define _HTTP_INCLUDED_HPP_
///////////////////////////////////////////////////////////////////////////////
#pragma warning(disable:4786 4761)
#include "semantics.hpp"
#include "msghdr.hpp"
#include <boost/spirit/utility/loops.hpp>
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/**
 * BNF for request
 * request			= method space uri space http-version crlf
 *					  *(message-header crlf)
 *					  crlf
 * method			= "GET" | "OPTIONS" | "HEAD" |
 *					  "POST" | "PUT" | "DELETE" |
 *					  "TRACE" | "CONNECT"
 * uri				= this is uniform resource identifier we does not parse its
 *					  it is look on the sequence of characters in the range ['\x21', '\x7f']
 * space			= character '\x20'
 * crlf				= "\r\n"
 */
struct request
	: public boost::spirit::grammar<request>
{
	typedef request_data	data_type;
	//! default ctor to need check syntax
	request()
		: request_(0)
	{
	}
	//! extracted request data
	explicit request(request_data* r)
		: request_(r)
	{
	}
	request_data const* data() const { return request_; }
	//! message header part
	message_header const& msghdr() const { return msg_header_; }
	//! create semantic action to extract uri from request line
	detail::sequence uri() const
	{
		return request_
			? detail::sequence(&request_->uri)
			: detail::sequence(0);
	}
	//! create semantic action to extract request method from request line
	detail::sequence method() const
	{
		return request_
			? detail::sequence(&request_->method)
			: detail::sequence(0);
	}
	//! create semantic action to extract version field from request line
	detail::sequence version() const
	{
		return request_
			? detail::sequence(&request_->ver)
			: detail::sequence(0);
	}
	//! create semantic action to fill up message header
	detail::header header() const
	{
		return request_
			? detail::header(&request_->hdrs)
			: detail::header(0);
	}
	template<typename scannerT>
    struct definition
    {
		definition(request const& self)
		{
			using namespace boost::spirit;
			basic_methods = chseq_p("GET") | chseq_p("OPTIONS") | chseq_p("HEAD") |
						chseq_p("POST") | chseq_p("PUT") | chseq_p("DELETE") |
						chseq_p("TRACE") | chseq_p("CONNECT");
			/** next are WebDav's methods implemented by Microsoft RFC 2518*/
			webdav_methods = chseq_p("BCOPY") | chseq_p("BDELETE") | chseq_p("BMOVE") |
						chseq_p("BPROPFIND") | chseq_p("BPROPPATCH") | chseq_p("COPY") |
						chseq_p("LOCK") | chseq_p("MKCOL") | chseq_p("MOVE") |
						chseq_p("NOTIFY") | chseq_p("POLL") | chseq_p("PROPFIND") |
						chseq_p("PROPPATCH") | chseq_p("SEARCH") | chseq_p("SUBSCRIBE") |
						chseq_p("UNLOCK") | chseq_p("UNSUBSCRIBE") | chseq_p("X-MS-ENUMATTS");
			/** the extension of the WebDav's methods described in the RFC 3253*/
			subversion_methods = chseq_p("VERSION-CONTROL") | chseq_p("CHECKOUT") | chseq_p("REPORT") | 
						chseq_p("CHECKIN") | chseq_p("UNCHECKOUT") | chseq_p("UPDATE") |
						chseq_p("LABEL") | chseq_p("MERGE") | chseq_p("BASELINE-CONTROL") |
						chseq_p("MKACTIVITY") | chseq_p("MKWORKSPACE");
			version = +(range<unsigned char>('\x20', '\xff') - space_p);
			first =	(basic_methods | webdav_methods | subversion_methods)[self.method()]
					>> +(ch_p(' ') | ch_p('\t'))//! need be more tolerant
					>> (+(range<unsigned char>('\x20', '\xff') - space_p))[self.uri()]
					>> +(ch_p(' ') | ch_p('\t'))//! need be more tolerant
					>> str_p("HTTP/") >> version[self.version()]
					>> eol_tolerant_p//! need more tolerant the str_p("\r\n"), because the AdvApache server is a whole shit
					>> *(
							self.msghdr()[self.header()]
							>> eol_tolerant_p//! need more tolerant then str_p("\r\n")
						)
					>> eol_tolerant_p;//! need more tolerant the str_p("\r\n"), because the AdvApache server is a whole shit;
		}
		boost::spirit::rule<scannerT> const& start() const { return first; }
		boost::spirit::rule<scannerT>	first;
		boost::spirit::rule<scannerT>	basic_methods;
		boost::spirit::rule<scannerT>	webdav_methods;
		boost::spirit::rule<scannerT>	subversion_methods;
		boost::spirit::rule<scannerT>	version;
	};
private:
	request_data*	request_;		//! extracted data from HTTP request
	message_header	msg_header_;	//! grammar for message header
};
///////////////////////////////////////////////////////////////////////////////
/**
 * BNF for response
 * response			= http-version space status-code space reason-phrase crlf
 *					  *(message-header crlf)
 *					  crlf
 * status-code		= 3digit
 * reason-phrase	= *text exclude cr and lf
 */
struct response
	: public boost::spirit::grammar<response>
{
	typedef response_data	data_type;
	//! dafault ctor can be used if you want only check syntax
	response()
		: response_(0)
	{
	}
	//! if you want extract data
	response(response_data* r)
		: response_(r)
	{
	}
	response_data const* data() const { return response_; }
	//! message header part subrule
	message_header const& msghdr() const { return msg_header_; }
	//! create semantic action to extract reason field from response line
	detail::sequence reason() const
	{
		return response_
				? detail::sequence(&response_->reason)
				: detail::sequence(0);
	}
	//! create semantic action to extract version field from response line
	detail::sequence version() const
	{
		return response_
			? detail::sequence(&response_->ver)
			: detail::sequence(0);
	}
	//! create semantic action to extract status code from respnse line
	detail::digits status_code() const
	{ 
		return response_
				? detail::digits(&response_->scode)
				: detail::digits(0);
	}
	//! create semantic actin to fill up map of the message header tuple - (filed-name, filed-name)
	detail::header header() const
	{
		return response_
				? detail::header(&response_->hdrs)
				: detail::header(0);
	}
	template<typename scannerT>
    struct definition
    {
		definition(response const& self)
		{
			using namespace boost::spirit;
			version = +(range<unsigned char>('\x20', '\xff') - space_p);
			first = str_p("HTTP/") >> version[self.version()]
					>> +(ch_p(' ') | ch_p('\t'))//! need be more tolerant
					>> (repeat_p(3)[digit_p])[self.status_code()]//! некоторые сервера отвечают без reason
					>> *(ch_p(' ') | ch_p('\t'))//! need be more tolerant
					>> (*range<unsigned char>('\x20', '\xff'))[self.reason()]
					>> eol_tolerant_p//! need more tolerant the str_p("\r\n"), becaus the AdvApache server is a whole shit
					>> *(
							self.msghdr()[self.header()]
							>> eol_tolerant_p//! the tolerant str_p("\r\n")
						)
					>> eol_tolerant_p;//! need more tolerant the str_p("\r\n"), becaus the AdvApache server is a whole shit;
		}
		boost::spirit::rule<scannerT> const& start() const { return first; }
		boost::spirit::rule<scannerT>	first;
		boost::spirit::rule<scannerT>	version;
	};
private:
	response_data*	response_;
	message_header	msg_header_;
};

///////////////////////////////////////////////////////////////////////////////
/**
* BNF for HTTP extension request
* request			= method space uri space http-version crlf
*					  *(message-header crlf)
*					  crlf
* method			= *
* uri				= this is uniform resource identifier we does not parse its
*					  it is look on the sequence of characters in the range ['\x21', '\x7f']
* space			= character '\x20'
* crlf				= "\r\n"
*/
struct request_ext
	: public boost::spirit::grammar<request_ext>
{
	typedef request_data	data_type;
	//! default ctor to need check syntax
	request_ext()
		: request_(0)
	{
	}
	//! extracted request data
	explicit request_ext(request_data* r)
		: request_(r)
	{
	}
	request_data const* data() const { return request_; }
	//! message header part
	message_header const& msghdr() const { return msg_header_; }
	//! create semantic action to extract uri from request line
	detail::sequence uri() const
	{
		return request_
			? detail::sequence(&request_->uri)
			: detail::sequence(0);
	}
	//! create semantic action to extract request method from request line
	detail::sequence method() const
	{
		return request_
			? detail::sequence(&request_->method)
			: detail::sequence(0);
	}
	//! create semantic action to extract version field from request line
	detail::sequence version() const
	{
		return request_
			? detail::sequence(&request_->ver)
			: detail::sequence(0);
	}
	//! create semantic action to fill up message header
	detail::header header() const
	{
		return request_
			? detail::header(&request_->hdrs)
			: detail::header(0);
	}

	template<typename scannerT>
	struct definition
	{
		definition(request_ext const& self)
		{
			using namespace boost::spirit;
			
			version = +(range<unsigned char>('\x20', '\xff') - space_p);

			separators = ch_p('(') | ch_p(')') | ch_p('<')
				| ch_p('>') | ch_p('@') | ch_p(',')
				| ch_p(';') | ch_p(':') | ch_p('\\')
				| ch_p('\"') | ch_p('/') | ch_p('[')
				| ch_p(']') | ch_p('?') | ch_p('=')
				| ch_p('{') | ch_p('}') | ch_p(' ')
				| ch_p('\t');

			lowercase = range<unsigned char>('a', 'z');

			method = +(range<unsigned char>('\x21', '\x7c') - lowercase - separators);

			first =	method[self.method()]
			>> +(ch_p(' ') | ch_p('\t'))//! need be more tolerant
				>> (+(range<unsigned char>('\x20', '\xff') - space_p))[self.uri()]
			>> +(ch_p(' ') | ch_p('\t'))//! need be more tolerant
				>> str_p("HTTP/") >> version[self.version()]
			>> eol_tolerant_p//! need more tolerant the str_p("\r\n"), because the AdvApache server is a whole shit
				>> *(
				self.msghdr()[self.header()]
			>> eol_tolerant_p//! need more tolerant then str_p("\r\n")
				)
				>> eol_tolerant_p;//! need more tolerant the str_p("\r\n"), because the AdvApache server is a whole shit;
		}
		boost::spirit::rule<scannerT> const& start() const { return first; }
		boost::spirit::rule<scannerT>	first;
		boost::spirit::rule<scannerT>	version;
		boost::spirit::rule<scannerT>	separators;
		boost::spirit::rule<scannerT>	lowercase;
		boost::spirit::rule<scannerT>	method;
	};
private:
	request_data*	request_;		//! extracted data from HTTP request
	message_header	msg_header_;	//! grammar for message header
};


///////////////////////////////////////////////////////////////////////////////
/**
 * G - грамматика для разбора входного потока
 * R - стратегия оценивающая правильноть разобранныз данных
 */
template<typename G, typename R>
class message_recognizer
{
public:
	message_recognizer(G const& grammar, R const& recognizer)
		: grammar_(grammar)
		, recognizer_(recognizer)
	{
	}
	/** return true if http message starts from 'beg' iterator*/
	bool operator()(char const* beg, char const* end) const
	{
		boost::spirit::parse_info<> info = boost::spirit::parse(beg, end, grammar_);
		return (recognizer_(*grammar_.data(), info.hit, info.stop, beg, end) != http::not_mine);
	}
private:
	R const&	recognizer_;
	G const&	grammar_;
};
///////////////////////////////////////////////////////////////////////////////
/** 
 * return iterator where message is start
 * Pred - стратегия принимающая решение о том, находится ли в воследовательности [beg, end)
 *		  сообщение протокола HTTP
 */
template<typename It, typename Pred>
It find_message(It beg, It end, Pred found)
{
	for(; beg != end && !found(beg, end); ++beg)
		0;
	return beg;
}
///////////////////////////////////////////////////////////////////////////////
}//namesapce http
///////////////////////////////////////////////////////////////////////////////
#endif// !defined(_HTTP_INCLUDED_HPP_)