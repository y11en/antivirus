/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	chunk.hpp
*		\brief	grammar for the chunked transfer coding
*		
*		\author Владислав Овчарик
*		\date	03.05.2005 14:39:36
*		
*		
*/
#if !defined(_CHUNK_INCLUDED_H_)
#define _CHUNK_INCLUDED_H_
///////////////////////////////////////////////////////////////////////////////
#pragma warning(disable:4786)
#include "semantics.hpp"
#include "msghdr.hpp"
#include <boost/spirit/utility/loops.hpp>
///////////////////////////////////////////////////////////////////////////////
//BNF for chunked transfer encoding
//
//chunked-body		= *chunk
//					  last-chunk
//					  trailer
//					  CRLF
//chunk				= chunk-size [chunk-extension] CRLF
//chunk-size		= 1*HEX
//last-chunk		= 1*("0") [chunk-extension] CRLF
//
//chunk-extension	= *(";" chunk-ext-name ["=" chunk-ext-val])
//chunk-ext-name	= token
//chunk-ext-val		= token | quoted-string
//chunk-data		= chunk-size(OCTET)
//trailer			= *(entity-header CRLF)
///////////////////////////////////////////////////////////////////////////////
/**
 * \beif	-	BNF for chunk
 */
namespace http
{
///////////////////////////////////////////////////////////////////////////////
struct chunk
	: public boost::spirit::grammar<chunk>
{
	chunk()
		: data_(0)
		, size_(0)
	{
	}
	chunk(chunk_data* data)
		: data_(data)
	{
		if(data_)
			data_->size = 0;
	}
	int& size() const { return data_ ? data_->size : size_; }
	detail::sequence extension() const
	{
		return data_
			? detail::sequence(&data_->extension)
			: detail::sequence(0);
	};
	template<typename scannerT>
    struct definition
    {
		definition(chunk const& self)
		{
			using namespace boost::spirit;
			//! need more tolerant the str_p("\r\n"), because the AdvApache server is a whole shit
			lws = !eol_tolerant_p >> +((ch_p(' ') | ch_p('\t')));

			separator = ch_p('(') | ch_p(')') | ch_p('<')
						| ch_p('>') | ch_p('@') | ch_p(',')
						| ch_p(';') | ch_p(':') | ch_p('\\')
						| ch_p('\"') | ch_p('/') | ch_p('[')
						| ch_p(']') | ch_p('?') | ch_p('=')
						| ch_p('{') | ch_p('}') | ch_p(' ')
						| ch_p('\t');

			text = range<unsigned char>('\x20', '\xff') | lws;

			token = +(range_p('\x20', '\x7f') - separator);

			qstring = ch_p('\"')
					  >> *(
							  (ch_p('\\') >> range_p('\x0','\x7f'))
							  |
							  (text - ch_p('\"'))
						  )
					  >> ch_p('\"');

			extension = *( ch_p(';') >> !((token >> !( ch_p('=') >> (token | qstring))) || *range_p('\x20','\x7f')) );

			first = hex_p[assign(self.size())]
					>> *(ch_p(' ') | ch_p('\t'))//! need be more tolerant
					>> (!extension)[self.extension()] >> eol_tolerant_p;//! need more tolerant
		}
		boost::spirit::rule<scannerT> const& start() const { return first; }
		boost::spirit::rule<scannerT>	first;
		boost::spirit::rule<scannerT>	lws;
		boost::spirit::rule<scannerT>	separator;
		boost::spirit::rule<scannerT>	text;
		boost::spirit::rule<scannerT>	token;
		boost::spirit::rule<scannerT>	qstring;
		boost::spirit::rule<scannerT>	extension;
	};
private:
	chunk_data*	data_;
	mutable int	size_;
};
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif	-	BNF for last chunk
 */
struct last_chunk
		: public boost::spirit::grammar<last_chunk>
{
	last_chunk()
		: data_(0)
	{
	}
	last_chunk(last_chunk_data* data)
		: data_(data)
	{
		data_ ? data_->size = 0 : static_cast<void>(0);
	}
	message_header const& msghdr() const { return msg_header_; }
	detail::header header() const
	{
		return data_
			? detail::header(&data_->trailer)
			: detail::header(0);
	}
	detail::sequence extension() const
	{
		return data_
			? detail::sequence(&data_->extension)
			: detail::sequence(0);
	}
	template<typename scannerT>
    struct definition
    {
		definition(last_chunk const& self)
		{
			using namespace boost::spirit;
			//! need more tolerant the str_p("\r\n"), becaus the AdvApache server is a whole shit
			lws = !eol_tolerant_p >> +((ch_p(' ') | ch_p('\t')));

			separator = ch_p('(') | ch_p(')') | ch_p('<')
						| ch_p('>') | ch_p('@') | ch_p(',')
						| ch_p(';') | ch_p(':') | ch_p('\\')
						| ch_p('\"') | ch_p('/') | ch_p('[')
						| ch_p(']') | ch_p('?') | ch_p('=')
						| ch_p('{') | ch_p('}') | ch_p(' ')
						| ch_p('\t');

			text = range<unsigned char>('\x20', '\xff') | lws;

			token = +(range_p('\x20', '\x7f') - separator);

			qstring = ch_p('\"')
					  >> *(
							(ch_p('\\') >> range_p('\x0','\x7f'))
							|
							(text - ch_p('\"'))
						  )
					  >> ch_p('\"');

			extension = *( ch_p(';') >> !((token >> !( ch_p('=') >> (token | qstring))) || *range_p('\x20','\x7f')) );

			first = +ch_p('0')
					>> *(ch_p(' ') | ch_p('\t'))//! need be more tolerant
					>> (!extension)[self.extension()] >> eol_tolerant_p
					>> !eol_tolerant_p >> *(
							self.msghdr()[self.header()]
							>> eol_tolerant_p//! the tolerant str_p("\r\n")
						)
					>> eol_tolerant_p;//! need more tolerant
		}
		boost::spirit::rule<scannerT> const& start() const { return first; }
		boost::spirit::rule<scannerT>	first;
		boost::spirit::rule<scannerT>	lws;
		boost::spirit::rule<scannerT>	separator;
		boost::spirit::rule<scannerT>	text;
		boost::spirit::rule<scannerT>	token;
		boost::spirit::rule<scannerT>	qstring;
		boost::spirit::rule<scannerT>	extension;
	};
private:
	last_chunk_data*	data_;
	message_header		msg_header_;
};
///////////////////////////////////////////////////////////////////////////////
}//namespace http
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(_CHUNK_INCLUDED_H_)