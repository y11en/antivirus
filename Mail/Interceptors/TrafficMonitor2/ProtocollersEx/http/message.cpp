/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	message.cpp
*		\brief	реализаци http сообщений
*		
*		\author Владислав Овчарик
*		\date	26.04.2005 17:41:19
*		
*		
*/
#define NOMINMAX
#pragma warning(disable : 4275)
#include <windows.h>
#include <Prague/prague.h>
#include <Prague/wrappers/io_helpers.h>
#include <http/chunk.hpp>
#include "message.hpp"
#include <fstream>
#include "dump.hpp"
///////////////////////////////////////////////////////////////////////////////
namespace
{
///////////////////////////////////////////////////////////////////////////////
//! возвращает массив возможных значений для media-type
static const char *cnttype_[] = 
{
	"application",
	"audio",
	"image",
	"message",
	"model",
	"multipart",
	"text",
	"video"
};

///////////////////////////////////////////////////////////////////////////////
}//unnamed namespace
///////////////////////////////////////////////////////////////////////////////
template<>
struct std::equal_to<char const*>
			: public std::binary_function<char const*, char const*, bool>
{
	bool operator()(char const* lhs, char const* rhs) const
	{
		return _stricmp(lhs, rhs) == 0;
	}
};
///////////////////////////////////////////////////////////////////////////////
hIO http::generic_message::assemble(http::message::build_data* d) const
{
	sync::local_mutex::scoped_lock lock(mutex_);
	prague::IO<char> packet(prague::create_temp_io());
	std::vector<char> const& h = *header_;
	char size[0x40] = { 0 };
	if(d)
	{
		size_t shift(0);
		//! НЕОБХОДИМО УЧИТЫВАТЬ, ЧТО d->length МОЖЕТ ИМЕТЬ ЗНАЧЕНИЕ UNSPECIFIED_LENGTH
		if(d->position <= h.size() && h.size() > 0)//! формируем только заголовок пакета
		{
			size_t local_length = ((d->length == unspecified_length) ? (h.size() - d->position) : d->length);
			char const* beg = &h[0] + d->position;
			char const* end = beg + ((local_length + d->position) > h.size() ? (h.size() - d->position) : local_length);
			packet.insert(packet.end(), beg, end);
			shift += end - beg;
		}
		int content_position(d->position + shift - h.size());//! смещенная позиция содержимого
		if(content_position >= 0 && content_position < content_.size())
		{
			size_t content_length = std::min<size_t>(d->length, ((size_t)content_.size() - content_position));
//			prague::IO<char>::const_iterator beg(content_.begin() + content_position);
//			prague::IO<char>::const_iterator end(beg + content_length);
//			packet.insert(packet.end(), beg, end);
			//TODO: Этот код необходимо перенести в prague::IO<> шаблон
			char tmp[0x1000] = { 0 };
			size_t length_chunk = std::min(content_length, 0x1000u);
			size_t pos = content_position;
			while(pos != (content_length + content_position) && length_chunk)
			{
				tDWORD read(0);
				content_.get()->SeekRead(&read, pos, &tmp[0], length_chunk);
				tDWORD written(0);
				packet.get()->SeekWrite(&written, packet.size(), &tmp[0], read);
				pos += read;
				length_chunk = std::min((content_length + content_position) - pos, 0x1000u);
			}
			shift += content_length;
		}
		d->position += shift;
	}
	else
	{
		if(!h.empty())
			packet.insert(packet.end(), (char const*)&h[0], (char const*)(&h[0] + h.size()));
		//TODO: Этот код необходимо перенести в prague::IO<> шаблон
		char tmp[0x1000] = { 0 };
		size_t content_length = (size_t)content_.size();
		size_t length_chunk = std::min(content_length, 0x1000u);
		size_t pos = 0;
		while(pos != content_length)
		{
			tDWORD read(0);
			content_.get()->SeekRead(&read, pos, &tmp[0], length_chunk);
			tDWORD written(0);
			packet.get()->SeekWrite(&written, packet.size(), &tmp[0], read);
			pos += read;
			length_chunk = std::min(content_length - pos, 0x1000u);
		}
//		packet.insert(packet.end(), content_.begin(), content_.end());
	}
	return packet.release();
}
///////////////////////////////////////////////////////////////////////////////
hIO http::generic_message::replace(hIO new_io)
{
	sync::local_mutex::scoped_lock lock(mutex_);
	hIO prev_io(content_.release());
	content_.reset(new_io);
	message_headers& msghdr = this->headers();
	vector_ptr const& t = msghdr.get(field_name(_CONTENT_LENGTH));
	if(t.get())//! такое поле существует и его нужно обработать
	{
		char buf[0x10] = { 0 };
		_itoa((size_t)content_.size(), buf, 10);
		msghdr.replace(field_name(_CONTENT_LENGTH), field_name(buf));
		content_size_ = (size_t)content_.size();
	}
	return prev_io;
}
///////////////////////////////////////////////////////////////////////////////
std::auto_ptr<std::vector<char> > http::generic_message::media_subtype() const
{
	std::auto_ptr<std::vector<char> > subtype(new std::vector<char>(1, 0));
	message_headers const& hdrs = const_cast<generic_message*>(this)->headers();
	http::vector_ptr val(hdrs.get(field_name(_CONTENT_TYPE)));
	if(val.get() && !val->empty())
	{
		char const* b = &(*val)[0];
		char const* e = b + val->size();
		std::string const cnttype(b, e);
		std::string::size_type pos(cnttype.find_first_of('/'));
		if(pos != std::string::npos)
		{
			std::string::const_iterator beg(cnttype.begin() + pos + 1);
			subtype->insert(subtype->begin(), beg, cnttype.end());
		}
	}
	return subtype;
}
///////////////////////////////////////////////////////////////////////////////
http::request_message::request_message(http::request_data const& data
									, std::auto_ptr<std::vector<char> > header
									, prague::IO<char>& content
									, size_t content_size)
							: generic_message(header, content, content_size)
							, data_(data)
{
}
///////////////////////////////////////////////////////////////////////////////
http::message_type http::request_message::type() const
{
	return request_type;
}
///////////////////////////////////////////////////////////////////////////////
http::content_type http::request_message::contenttype() const
{
	return _specific;
}
///////////////////////////////////////////////////////////////////////////////
http::content_encoding http::request_message::contentencoding() const
{
	return specific_encoding;
}
///////////////////////////////////////////////////////////////////////////////
http::message_headers& http::request_message::headers()
{
	return *static_cast<message_headers*>(this);
}
///////////////////////////////////////////////////////////////////////////////
std::auto_ptr<std::vector<char> > http::request_message::uri() const
{
	vector_ptr v(make_vector_ptr(data_.uri.begin(), data_.uri.end()));
	v->push_back(0);
	return v;
}
///////////////////////////////////////////////////////////////////////////////
std::auto_ptr<std::vector<char> > http::request_message::method() const
{
	vector_ptr v(make_vector_ptr(data_.method.begin(), data_.method.end()));
	v->push_back(0);
	return v;
}
///////////////////////////////////////////////////////////////////////////////
std::auto_ptr<std::vector<char> > http::request_message::host() const
{
	vector_ptr v(this->get(field_name(_HOST)));
	if(!v.get())
	{
		v = vector_ptr(new std::vector<char>);
		v->reserve(1);
	}
	v->push_back(0);
	return v;
}
///////////////////////////////////////////////////////////////////////////////
http::request_message::~request_message()
{
}
///////////////////////////////////////////////////////////////////////////////
http::response_message::response_message(response_data const& data
									, std::auto_ptr<std::vector<char> > header
									, prague::IO<char>& content
									, size_t content_size
									, method_type request_method)
							: generic_message(header, content, content_size)
							, data_(data)
							, method_(request_method)
{
}
///////////////////////////////////////////////////////////////////////////////
http::message_type http::response_message::type() const
{
	return response_type;
}
///////////////////////////////////////////////////////////////////////////////
http::content_type http::response_message::contenttype() const
{
	content_type cnt_typ(_specific);
	http::header_t::const_iterator cit = data_.hdrs.find(field_name(_CONTENT_TYPE));
	if(cit != data_.hdrs.end())
	{
		//todo: использовать парсер для разбора типа содержимого
		std::string const& cnttype = http::buffer_cast<std::string>(cit->second, 0);
		std::string::size_type pos(cnttype.find_first_of('/'));
		if(pos != std::string::npos)
		{
			std::string const& mtype = cnttype.substr(0, pos);
			const char **mtit = 
						std::find_if(cnttype_, cnttype_ + _countof(cnttype_),
										std::bind2nd(std::equal_to<char const*>(), mtype.c_str()));
			if(mtit != cnttype_ + _countof(cnttype_))
			{
				ptrdiff_t off = mtit - cnttype_;
				cnt_typ = static_cast<content_type>(_application + off);
			}
		}
	}
	return cnt_typ;
}
///////////////////////////////////////////////////////////////////////////////
http::message_headers& http::response_message::headers()
{
	return *static_cast<message_headers*>(this);
}
///////////////////////////////////////////////////////////////////////////////
static void convertCharsToString( const std::vector< char >& vec, std::string& str )
{
	vec.empty() ? str.clear() : str.assign(&vec[0], vec.size());
}
///////////////////////////////////////////////////////////////////////////////
http::content_encoding http::response_message::contentencoding() const
{
	const http::message_headers& msghdr = const_cast<http::response_message *>(this)->headers();
	const http::vector_ptr& encoding_header = msghdr.get(http::field_name(_CONTENT_ENCODING));
	if(encoding_header.get())
	{
		std::string encoding_str;
		convertCharsToString(*encoding_header.get(), encoding_str);
		PR_ASSERT(!encoding_str.empty());
		// RFC 1945, RFC 2616
		if(encoding_str == "x-gzip" || encoding_str == "gzip")
			return http::gzip_encoding;
		else if(encoding_str == "x-compress" || encoding_str == "compress")
			return http::compress_encoding;
		else if(encoding_str == "deflate")
			return http::deflate_encoding;
	}
	return http::specific_encoding;
}
///////////////////////////////////////////////////////////////////////////////
int http::response_message::status_code() const { return data_.scode; }
///////////////////////////////////////////////////////////////////////////////
std::auto_ptr<std::vector<char> > http::response_message::reason() const
{
	vector_ptr v(make_vector_ptr(data_.reason.begin(), data_.reason.end()));
	v->push_back(0);
	return v;
}
///////////////////////////////////////////////////////////////////////////////
http::response_message::~response_message()
{
}
///////////////////////////////////////////////////////////////////////////////
http::chunked_message::~chunked_message()
{
	if(dump<3>::need_dump() && !cache_->empty())
		dump<3>("!chunk", this).write(cache_->begin(), cache_->end());
}
///////////////////////////////////////////////////////////////////////////////
http::chunked_message::chunked_message(std::auto_ptr<message> msg)
						: msg_(msg)
						, cache_(new std::vector<char>)
						, complete_(false)
						, ready_to_deliver_(false)
{
	if(generic_message* gm = dynamic_cast<generic_message*>(msg_.get()))
	{
		gm->set_unknown_content_size();
		gm->set_chunked(true);
	}
}
///////////////////////////////////////////////////////////////////////////////
http::message_type http::chunked_message::type() const { return msg_->type(); }
///////////////////////////////////////////////////////////////////////////////
bool http::chunked_message::complete() const { return complete_; }
///////////////////////////////////////////////////////////////////////////////
http::content_type http::chunked_message::contenttype() const { return msg_->contenttype(); }
///////////////////////////////////////////////////////////////////////////////
http::content_encoding http::chunked_message::contentencoding() const { return msg_->contentencoding(); }
///////////////////////////////////////////////////////////////////////////////
hIO http::chunked_message::content() const { return msg_->content(); }
///////////////////////////////////////////////////////////////////////////////
std::vector<char> const& http::chunked_message::heading() const { return msg_->heading(); }
///////////////////////////////////////////////////////////////////////////////
hIO http::chunked_message::assemble(http::message::build_data* d) const
{
	static char const crlf[] = "\r\n";
	prague::IO<char> packet(prague::create_temp_io());
	prague::WeakIO<char> c(msg_->content());
	std::vector<char> const& h = msg_->heading();
	char size[0x40] = { 0 };
	if(d)//! сборка только части пакета
	{
		size_t shift(0);
		//! НЕОБХОДИМО УЧИТЫВАТЬ, ЧТО d->length МОЖЕТ ИМЕТЬ ЗНАЧЕНИЕ UNSPECIFIED_LENGTH
		if(d->position <= h.size() && h.size() > 0)//! формируем только заголовок пакета
		{
			size_t local_length = ((d->length == unspecified_length) ? (h.size() - d->position) : d->length);
			char const* beg = &h[0] + d->position;
			char const* end = beg + ((local_length + d->position) > h.size() ? (h.size() - d->position) : local_length);
			packet.insert(packet.end(), beg, end);
			shift += end - beg;
		}
		int content_position(d->position + shift - h.size());
		if(content_position >= 0 && content_position <= c.io().size())
		{
			size_t content_length = std::min<size_t>(d->length, ((size_t)c.io().size() - content_position));
			//prague::IO<char>::iterator beg(c.io().begin() + content_position);
			//prague::IO<char>::iterator end(beg + content_length);
			if(content_length > 0)
			{
				_itoa(content_length, size, 16);
				strcat(size, crlf);
				packet.insert(packet.end()
							, const_cast<char const*>(size)
							, const_cast<char const*>(size) + strlen(size));
				//packet.insert(packet.end(), beg, end);
				char tmp[0x1000] = { 0 };
				size_t length_chunk = std::min(content_length, 0x1000u);
				size_t pos = content_position;
				while(pos != (content_length + content_position) && length_chunk)
				{
					tDWORD read(0);
					c.io().get()->SeekRead(&read, pos, &tmp[0], length_chunk);
					tDWORD written(0);
					packet.get()->SeekWrite(&written, packet.size(), &tmp[0], read);
					pos += read;
					length_chunk = std::min((content_length + content_position) - pos, 0x1000u);
				}
				packet.insert(packet.end()
							, const_cast<char const*>(crlf)
							, const_cast<char const*>(crlf) + sizeof(crlf) - 1);
			}
			if((content_position + content_length == c.io().size()) && this->complete())//! need last-chunk
			{
				_itoa(0, size, 16);
				strcat(size, crlf);
				strcat(size, crlf);
				packet.insert(packet.end()
							, const_cast<char const*>(size)
							, const_cast<char const*>(size) + strlen(size));
			}
			shift += content_length;
		}
		d->position += shift;
	}
	else
	{
		if(!h.empty())
			packet.insert(packet.end(), &h[0], &h[0] + h.size());
		_itoa((int)c.io().size(), size, 16);
		strcat(size, crlf);
		packet.insert(packet.end()
					, const_cast<char const*>(size)
					, const_cast<char const*>(size) + strlen(size));
//		packet.insert(packet.end(), c.io().begin(), c.io().end());
		char tmp[0x1000] = { 0 };
		size_t content_length = (size_t)c.io().size();
		size_t length_chunk = std::min(content_length, 0x1000u);
		size_t pos = 0;
		while(pos != content_length && length_chunk)
		{
			tDWORD read(0);
			c.io().get()->SeekRead(&read, pos, &tmp[0], length_chunk);
			tDWORD written(0);
			packet.get()->SeekWrite(&written, packet.size(), &tmp[0], read);
			pos += read;
			length_chunk = std::min(content_length - pos, 0x1000u);
		}
		packet.insert(packet.end()
					, const_cast<char const*>(crlf)
					, const_cast<char const*>(crlf) + sizeof(crlf) - 1);
		_itoa(0, size, 16);
		strcat(size, crlf);
		strcat(size, crlf);
		packet.insert(packet.end()
					, const_cast<char const*>(size)
					, const_cast<char const*>(size) + strlen(size));
	}

	return packet.release();
}
///////////////////////////////////////////////////////////////////////////////
hIO http::chunked_message::replace(hIO new_io) { return msg_->replace(new_io); }
///////////////////////////////////////////////////////////////////////////////
//! возвращаем количество байт, обработанных из входной последовательности,
//! заданной интервалом [begin, end)
size_t http::chunked_message::append(char const* begin, char const* end)
{
	namespace spirit = boost::spirit;

	size_t size(0);//! количество обработанных байт
	if(begin != end && !this->complete())
	{
		//! serialize access to the cache
		sync::local_mutex::scoped_lock lock(mutex_);
		chunk_descr* c = this->current_chunk();
		if(c && !c->completed())//! обработка уже выделенного чанка
		{
			int need_bytes(c->unprocessed());
			size = std::min<int>(need_bytes, (end - begin));
			msg_->append(begin, begin + size);
			c->process(size);
			begin += size;
		}
		else//! обработка нового чанка
		{
			chunk_descr nc(extract_chunk(begin, end, size));
			complete_ = (nc.size() == 0);
			if(!complete_ && (nc.size() != 0xffffffffu))//! выделили не финальный чанк
			{
				begin += size;//! смещаем буфер на длину распарсенных данных
				size_t obtaine_size(std::min<size_t>(nc.size(), (end - begin)));
				size += obtaine_size;
				msg_->append(begin, begin + obtaine_size);
				begin += obtaine_size;
				nc.process(obtaine_size);
				this->add_chunk(nc);
			}
		}
	}
	return size;
}
///////////////////////////////////////////////////////////////////////////////
http::chunked_message::chunk_descr http::chunked_message::extract_chunk(char const* begin
																		, char const* end
																		, size_t& size)
{
	namespace spirit = boost::spirit;
	chunk_descr c(0xffffffffu);
	bool complete(false);
	cache_->insert(cache_->end(), begin, end);
	std::vector<char>::iterator it = cache_->begin();
	char const* begin_cache = &(*cache_)[0];
	char const* end_cache = begin_cache + cache_->size();
	//! пропускаем все лидирующие CRLF
	//! это перенесено сюда для разбора CRLF следющих поле chunked-data
	spirit::parse_info<> info = spirit::parse(begin_cache, end_cache, *eol_tolerant_p);
	if(info.hit)
	{
		it += info.length;
		begin_cache += info.length;
	}
	//! пытаемся выделить заголовок chunk-а или last-chunk & trailer
	last_chunk_data ld;
	last_chunk lch(&ld);
	info = spirit::parse(begin_cache, end_cache, lch);
	
	switch(recognize(info.hit, info.stop, end_cache))
	{
	case full_chunk://! message-body is complete
		{
			it += info.length;//! message-body полностью получен, так как найден last-chunk
			//! Если существуют mesage-header, то добавить их в msg_
			//TODO: ПОТРЕБУТСЯ ДОБАВЛЕНИЕ ИХ В ПОСЛЕДНИЙ ЧАНК
			http::header_t& hdrs = ld.trailer;
			message_headers& msghdrs = msg_->headers();
			for(http::header_t::iterator beg = hdrs.begin(); beg != hdrs.end(); ++beg)
				msghdrs.add(beg->first, beg->second);
			c = chunk_descr(0);
			complete = true;//! устанавливаем признак завершенности
		}
		break;
	case not_chunk:
		{
			chunk_data d;
			chunk ch(&d);
			info = spirit::parse(begin_cache, end_cache, ch);
			if(recognize(info.hit, info.stop, end_cache) == full_chunk)//! выделили полный chunk
			{
				//! добавляем данные в контекст
				it += info.length;
				complete = true;
				c = chunk_descr(d.size);
			}
		}
		break;
	}
	size = end - begin;
	size_t workon_size(std::distance(cache_->begin(), it));//! колличесто байт обработанных из кеша
	if(workon_size && complete)
	{
		int diff_size(cache_->size() - workon_size);//! колличество необработанных байт в кеше
		int buff_size((end - begin) - diff_size);//! колличество обработанных байт из входной последовательности
		size = (buff_size > 0 ? buff_size : 0);//! количесвто обработанных байт методом append
		cache_->clear();
	}
	return c;
}
///////////////////////////////////////////////////////////////////////////////
size_t http::chunked_message::size() const { return msg_->size(); }
///////////////////////////////////////////////////////////////////////////////
size_t http::chunked_message::max_size() const { return msg_->max_size(); }
///////////////////////////////////////////////////////////////////////////////
bool http::chunked_message::ready_to_deliver() const { return ready_to_deliver_; }
///////////////////////////////////////////////////////////////////////////////
void http::chunked_message::set_ready_to_deliver(bool ready) { ready_to_deliver_ = ready; }
///////////////////////////////////////////////////////////////////////////////
http::message_headers& http::chunked_message::headers() { return msg_->headers(); }
///////////////////////////////////////////////////////////////////////////////
void http::chunked_message::apply(http::base_action& act) { msg_->apply(act); }
///////////////////////////////////////////////////////////////////////////////