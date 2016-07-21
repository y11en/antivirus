/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	message.hpp
*		\brief	
*		
*		\author Владислав Овчарик
*		\date	04.05.2005 16:45:05
*		
*		
*/
#if !defined(_MESSAGE_INCLUDED_HPP_)
#define _MESSAGE_INCLUDED_HPP_
#pragma warning(disable:4786)
#include <list>
#include <algorithm>
#include <Prague/wrappers/io.hpp>
#include <utils/local_mutex.hpp>
#include <boost/utility.hpp>
#include "http.hpp"
#include "message.h"
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
namespace detail
{
///////////////////////////////////////////////////////////////////////////////
template<typename D> struct header_traits;
///////////////////////////////////////////////////////////////////////////////
template<typename D>
struct header_data_ref
{
	typedef D								header_type;
	typedef header_traits<D>				packet;
	typedef typename packet::buffer_type	buffer_type;

	header_data_ref(D& d, buffer_type& buffer)
		: data(d)
		, header(buffer)
	{
	}

	D&				data;	//! данные HTTP пакета, полученные в результате синтаксического разбора
	buffer_type&	header;	//! плоский буфер, на котором строится объект типа D
};
///////////////////////////////////////////////////////////////////////////////
//! 
inline void assemble(http::header_t const& hdrs, std::vector<char>& header)
{
	char const crlf[] = "\r\n";
	char const fld_sep[] = ": ";
	typedef http::header_t::const_iterator const_iterator;
	for(const_iterator beg = hdrs.begin(); beg != hdrs.end(); ++beg)
	{
		http::buffer_t const& field_name = beg->first;
		http::buffer_t const& field_value = beg->second;
		header.insert(header.end(), field_name.begin(), field_name.end());
		header.insert(header.end(), fld_sep, fld_sep + sizeof(fld_sep) - 1);
		header.insert(header.end(), field_value.begin(), field_value.end());
		header.insert(header.end(), crlf, crlf + sizeof(crlf) - 1);
	}
	header.insert(header.end(), crlf, crlf + sizeof(crlf) - 1);
}
///////////////////////////////////////////////////////////////////////////////
template<>
struct header_traits<http::request_data>
{
	typedef std::vector<char> buffer_type;

	static http::request_data assemble(http::request_data const& data, buffer_type& header)
	{
		namespace spirit = boost::spirit;

		char const sp[] = " ";
		char const ver[] = "HTTP/";
		char const crlf[] = "\r\n";
		//! собираем start-line
		header.insert(header.end(), data.method.begin(), data.method.end());
		header.insert(header.end(), sp, sp + sizeof(sp) - 1);
		header.insert(header.end(), data.uri.begin(), data.uri.end());
		header.insert(header.end(), sp, sp + sizeof(sp) - 1);
		header.insert(header.end(), ver, ver + sizeof(ver) - 1);
		header.insert(header.end(), data.ver.begin(), data.ver.end());
		header.insert(header.end(), crlf, crlf + sizeof(crlf) - 1);
		//! собираем message-header в плоский буфер
		detail::assemble(data.hdrs, header);
		//! разбираем сформированнй буфер и создаем структуру пакета
		http::request_data req_data;
		http::request req(&req_data);
		char const* begin = &header[0];
		char const* end = &header[0] + header.size();
		spirit::parse_info<> info = spirit::parse(begin, end, req);
		return req_data;
	}
};
///////////////////////////////////////////////////////////////////////////////
template<>
struct header_traits<http::response_data>
{
	typedef std::vector<char> buffer_type;

	static http::response_data assemble(http::response_data const& data, buffer_type& header)
	{
		namespace spirit = boost::spirit;

		char const sp[] = " ";
		char const ver[] = "HTTP/";
		char const crlf[] = "\r\n";
		char code[10] = { 0 };
		//! собираем start-line
		header.insert(header.end(), ver, ver + sizeof(ver) - 1);
		header.insert(header.end(), data.ver.begin(), data.ver.end());
		header.insert(header.end(), sp, sp + sizeof(sp) - 1);
		_itoa(data.scode, code, 10);
		header.insert(header.end(), code, code + strlen(code));
		header.insert(header.end(), sp, sp + sizeof(sp) - 1);
		header.insert(header.end(), data.reason.begin(), data.reason.end());
		header.insert(header.end(), crlf, crlf + sizeof(crlf) - 1);
		//! собираем message-header в плоский буфер
		detail::assemble(data.hdrs, header);
		//! разбираем сформированнй буфер и создаем структуру пакета
		http::response_data resp_data;
		http::response resp(&resp_data);
		char const* begin = &header[0];
		char const* end = &header[0] + header.size();
		spirit::parse_info<> info = spirit::parse(begin, end, resp);
		return resp_data;
	}
};
}//namespace detail
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif	-	implementation for message_headers interface
 */
template<typename MSG, typename HT>
struct message_headers_impl
			: public message_headers
{
	typedef typename detail::header_data_ref<HT>::buffer_type	buffer_type;
	typedef typename detail::header_data_ref<HT>::packet		packet;

	enum { padding = 2 * sizeof(char) };

	virtual bool add(buffer_t const& field_name, buffer_t const& field_value)
	{
		using namespace detail;
		bool added(false);
		sync::local_mutex::scoped_lock lock(this->access());
		header_data_ref<HT> hd(this->data());//! return reference data
		http::header_t& hdrs = hd.data.hdrs;
		http::header_t::iterator fit(hdrs.find(field_name));
		if(fit == hdrs.end())
		{
			hdrs.insert(std::make_pair(field_name, field_value));
			buffer_type header;
			header.reserve(hd.header.size() + field_name.size() + field_value.size() + padding);
			//! set a new header data
			hd.data = packet::assemble(hd.data, header);
			hd.header.swap(header);
			added = true;
		}
		return added;
	}
	virtual vector_ptr get(buffer_t const& field_name) const
	{
		using namespace detail;
		sync::local_mutex::scoped_lock lock(this->access());
		vector_ptr tmp;
		header_data_ref<HT> hd(this->data());//! 
		http::header_t& hdrs = hd.data.hdrs;
		http::header_t::iterator fit(hdrs.find(field_name));
		if(fit != hdrs.end())
		{
			tmp = vector_ptr(new std::vector<char>);
			tmp->reserve(fit->second.size());
			tmp->insert(tmp->end(), fit->second.begin(), fit->second.end());
		}
		return tmp;
	}
	virtual bool remove(buffer_t const& field_name)
	{
		using namespace detail;
		bool removed(false);
		sync::local_mutex::scoped_lock lock(this->access());
		header_data_ref<HT> hd(this->data());//! return reference data
		http::header_t& hdrs = hd.data.hdrs;
		http::header_t::iterator fit(hdrs.find(field_name));
		if(fit != hdrs.end())
		{
			size_t removed_size(fit->first.size() + fit->second.size());
			hdrs.erase(fit);
			buffer_type header;
			header.reserve(hd.header.size() - removed_size);
			//! set a new header data
			hd.data = packet::assemble(hd.data, header);
			hd.header.swap(header);
			removed = true;
		}
		return removed;
	}
	virtual void replace(buffer_t const& field_name, buffer_t const& field_value)
	{
		using namespace detail;
		sync::local_mutex::scoped_lock lock(this->access());
		header_data_ref<HT> hd(this->data());//! return reference data
		http::header_t& hdrs = hd.data.hdrs;
		size_t removed_size(0);
		http::header_t::iterator fit(hdrs.find(field_name));
		if(fit != hdrs.end())
		{
			removed_size = fit->first.size() + fit->second.size();
			hdrs.erase(fit);
		}
		hdrs.insert(std::make_pair(field_name, field_value));
		buffer_type header;
		header.reserve(hd.header.size() + field_name.size() + field_value.size() + padding - removed_size);
		//! set a new header data
		hd.data = packet::assemble(hd.data, header);
		hd.header.swap(header);
	}
	virtual void add_remove(buffer_t const& remove_name
					, buffer_t const& field_name, buffer_t const& field_value)
	{
		using namespace detail;
		sync::local_mutex::scoped_lock lock(this->access());
		bool modified(false);
		header_data_ref<HT> hd(this->data());//! return reference data
		http::header_t& hdrs = hd.data.hdrs;
		http::header_t::iterator fit(hdrs.find(remove_name));
		size_t removed_size(0);
		if(fit != hdrs.end())
		{
			modified = true;
			removed_size = fit->first.size() + fit->second.size();
			hdrs.erase(fit);
		}
		fit = hdrs.find(field_name);
		if(fit == hdrs.end())
		{
			modified = true;
			hdrs.insert(std::make_pair(field_name, field_value));
		}
		if(modified)
		{
			buffer_type header;
			header.reserve(hd.header.size() - removed_size + field_name.size() + field_value.size() + padding);
			//! set a new header data
			hd.data = packet::assemble(hd.data, header);
			hd.header.swap(header);
		}
	}
private:
	//! return syncronization primitiv to serialize access
	sync::local_mutex& access() const
	{
		MSG* _this = const_cast<MSG*>(static_cast<MSG const*>(this));
		return _this->mutex_;
	}
	//! return message-headers data
	detail::header_data_ref<HT> data() const
	{
		MSG* _this = const_cast<MSG*>(static_cast<MSG const*>(this));
		return detail::header_data_ref<HT>(_this->data_, *_this->header_);
	}
};
///////////////////////////////////////////////////////////////////////////////
//fwd declare
class chunked_message;
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif - базовая реализация сообщение между клинтом и сервером
 *			длина message-body известна или будет определена
 *			в момент разрыва соединения
 */
class generic_message
		: public message
		, private boost::noncopyable
{
	friend class chunked_message;
public:
	//! return true if message-body has been received completely
	virtual bool complete() const { return content_.size() == content_size_; }
	//! comsumer must not release given handle
	virtual hIO content() const { return content_.get(); }
	virtual std::vector<char> const& heading() const { return *header_; }
	//! return full assembled packet. consumer should release given handle
	virtual hIO assemble(build_data* d) const;
	//!
	virtual hIO replace(hIO);
	//! return current content size (message-body length)
	virtual size_t size() const	{ return (size_t)content_.size(); }
	//! return size of generic-message (message-body length & header length)
	virtual size_t max_size() const { return size() + header_->size(); }
	//! return whether message is ready for deliver
	virtual bool ready_to_deliver() const { return ready_to_deliver_; }
	//! set ready for deliver
	virtual void set_ready_to_deliver(bool ready) { ready_to_deliver_ = ready; }
	//! implement simple algorithm when length of message-body was specified
	virtual size_t append(char const* begin, char const* end)
	{
		size_t added_size(0);
		if(!this->complete())
		{
			sync::local_mutex::scoped_lock lock(mutex_);
			size_t need_size(content_size_ - size());
			size_t size(end - begin);
			added_size = std::min(need_size, size);
			content_.insert(content_.end(), begin, begin + added_size);
		}
		return added_size;
	}
	//! return content subtype
	std::auto_ptr<std::vector<char> > media_subtype() const;
	//!
	virtual	~generic_message()	{}
	//! return true if message has unknown content size
	bool has_unknown_content_size() const { return content_size_ == 0xfffffffflu; }
	//! return true for chunked
	bool is_chunked_message() const { return is_chunked_; }
	//! set message chunked
	void set_chunked(bool chunked) { is_chunked_ = chunked; }

protected:
	generic_message(vector_ptr header, prague::IO<char>& content, size_t content_size)
				: content_(content)
				, content_size_(content_size)
				, header_(header)
				, ready_to_deliver_(false)
	{
	}
	void set_unknown_content_size() { content_size_ = 0xfffffffflu; }
protected:
	vector_ptr					header_;		//! message-header store in the memory
	size_t						content_size_;	//! real content size
	prague::IO<char>			content_;		//! content store in the hIO
	mutable sync::local_mutex	mutex_;			//! need to serialize access to content & header
	bool						ready_to_deliver_; // message is ready for deliver (used by cached strategy)
	bool						is_chunked_; // message is chunked
};
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif - http request message
 */
class request_message
		: public generic_message
		, private message_headers_impl<request_message, request_data>
{
	friend class analyzer;
	friend message_headers_impl<request_message, request_data>;

	typedef request_data	header_data_type;
	typedef message_headers_impl<request_message, request_data> message_headers_t;
public:
	//! should return request_type
	virtual message_type type() const;
	//! should return specific
	virtual content_type contenttype() const;
	//! return content encoding
	virtual content_encoding contentencoding() const;
	//! return uniform resource identifier
	std::auto_ptr<std::vector<char> > uri() const;
	//! return request method
	std::auto_ptr<std::vector<char> > method() const;
	//! return host name
	std::auto_ptr<std::vector<char> > host() const;
	//! implementation of message interface
	virtual message_headers& headers();
	//!
	virtual ~request_message();

	DEFINE_APPLY_ACTION(base_action)
private:
	request_message(request_data const&, vector_ptr header, prague::IO<char>&, size_t);
private:

	/**
	 * @supplierCardinality 1 
	 */
	request_data	data_;//! the mesage-header after parsing
};
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif - http response message
 **/
class response_message
		: public generic_message
		, private message_headers_impl<response_message, response_data>
{
	friend class analyzer;
	friend message_headers_impl<response_message, response_data>;

	typedef response_data	header_data_type;
	typedef message_headers_impl<response_message, response_data> message_headers_t;
public:
	//! should always return response_type
	virtual message_type type() const;
	//! calculate type of the content analyze Content-Type field
	virtual content_type contenttype() const;
	//! return content encoding
	virtual content_encoding contentencoding() const;
	//! return status code
	int status_code() const;
	//! return reason string
	std::auto_ptr<std::vector<char> > reason() const;
	//! return request method
	method_type request_method() const { return method_; }
	//! return private interface to manipulate message-header in the message-entity
	virtual message_headers& headers();
	//!
	virtual ~response_message();

	DEFINE_APPLY_ACTION(base_action)
private:
	response_message(response_data const&
					, vector_ptr header
					, prague::IO<char>&
					, size_t
					, method_type request_method);
private:

	/**
	 * @supplierCardinality 1 
	 */
	response_data	data_;		//! the message-header after parsing
	method_type		method_;	//! the request method for these response

};
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif - decorator to realize chunked message
 *			it transforms chunked transfer encoding message-body
 *			to the simple message-body
 */
class chunked_message
		: public message
		, private boost::noncopyable
{
	friend class analyzer;
	//! size & completeness
	class chunk_descr
	{
		typedef bool (chunk_descr::*unspecified_bool_type)() const;
	public:
		explicit chunk_descr(int size)
			: size_(size)
			, processed_size_(0)
		{
		}
		void process(int size) { processed_size_ += size; }
		size_t unprocessed() const { return size_ - processed_size_; } 
		bool completed() const { return processed_size_ == size_; }
		operator unspecified_bool_type() const { return completed() ? &chunk_descr::completed : 0; }
		size_t size() const { return size_; }
	private:
		int		size_;
		int		processed_size_;
	};
public:
	//! return type of http message
	virtual message_type type() const;
	//! return true if message-body has been received completely
	virtual bool complete() const;
	//! return content type
	virtual content_type contenttype() const; 
	//! return content encoding
	virtual content_encoding contentencoding() const;
	//! return message content
	virtual hIO content() const;
	virtual std::vector<char> const& heading() const;
	//! consumer should release hIO object
	virtual hIO assemble(build_data* d) const;
	//!
	virtual hIO replace(hIO);
	//!
	virtual void apply(http::base_action& act);
	//! summarize all chunk-data size
	virtual size_t size() const;
	//! summarize all chunk-data size and size of the header buffer
	virtual size_t max_size() const;
	//! return whether message is ready for deliver
	virtual bool ready_to_deliver() const;
	//! set ready for deliver
	virtual void set_ready_to_deliver(bool ready);
	//! parse buffer to extract chunked content and fill up content
	virtual size_t append(char const* begin, char const* end);
	//!
	virtual message_headers& headers();
	//!
	virtual ~chunked_message();
private:
	enum severity { full_chunk, partial_chunk, not_chunk };
	template<typename It>
	static severity recognize(bool hit, It stop, It end)
	{
		if(hit)
			return full_chunk;
		if(!hit && stop == end)
			return partial_chunk;
		return not_chunk;
	}
	//! 
	chunk_descr extract_chunk(char const* begin, char const* end, size_t& size);
	chunked_message(std::auto_ptr<message> msg);
	//! return current processing chunk that is not completed
	//! if chunk was processed that it will return 0
	chunk_descr* current_chunk()
	{
		chunk_descr* c = 0;
		if(!chunks_.empty())
			c = &chunks_.back();
		return c;
	}
	//! add a new chunk description
	void add_chunk(chunk_descr const& cn)
	{
		assert(cn.size() != 0 && cn.size() != 0xffffffffu);
		chunks_.push_back(cn);
	}
private:

	/**
	 * @supplierCardinality 1 
	 */
	std::auto_ptr<message>				msg_;	//! message with Content-Length = xxxx
	std::auto_ptr<std::vector<char> >	cache_;	//! small cache
	sync::local_mutex					mutex_;	//! serialize access to the cache
	bool								complete_;
	std::list<chunk_descr>				chunks_;
	bool								ready_to_deliver_;
};
///////////////////////////////////////////////////////////////////////////////
}//namespace http
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(_MESSAGE_INCLUDED_HPP_)