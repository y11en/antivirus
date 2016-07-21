/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	analyzerimpl.cpp
*		\brief	реализация класса, основной обязанность которого является
*				накопление потока данных и выделение с накопленых данных
*				пакетов(сообщенией) принадлежащий HTTP протоколу.
*				Грамматика сообщений описывается в файле http.hpp.
*				Описание протокола см. RFC 2616
*		
*		\author Владислав Овчарик
*		\date	25.04.2005 17:02:25
*		
*		
*/
#define NOMINMAX
#pragma warning(disable : 4275)
#include <windows.h>
#include <Prague/wrappers/io_helpers.h>
#include "message.hpp"
#include "http.hpp"
#include "analyzerimpl.h"
#include "stream_error.h"
#include <fstream>
#include "dump.hpp"
///////////////////////////////////////////////////////////////////////////////
namespace
{
/**максимальный размер буфера разбора - 8Кб*/
unsigned int const MAX_PARSE_BUFFER_SIZE = 0x2000;
}//unnamed namespace
///////////////////////////////////////////////////////////////////////////////
namespace spirit = boost::spirit;
///////////////////////////////////////////////////////////////////////////////
http::analyzer::analyzer()
		: cache_(0x2000)
		, supp_(0)
		, error_(stream_error::nothing_todo())
{
}
///////////////////////////////////////////////////////////////////////////////
http::analyzer::analyzer(support_persistent* supp)
		: cache_(0x2000)
		, supp_(supp)
		, error_(stream_error::nothing_todo())
{
}
///////////////////////////////////////////////////////////////////////////////
http::analyzer::~analyzer()
{
}
///////////////////////////////////////////////////////////////////////////////
http::recognition http::analyzer::recognize(char const* begin, char const* end) const
{
	//! необходимо пропустить первые CRLF, если такие существуют это связано с тем,
	//! что некоторые клиенты версии 1.0 могут посылать дополнительный CRLF для 
	//! POST запроса, в результате нарушается BNF форма следующего ответа с сервера
	spirit::parse_info<> info = spirit::parse(begin, end, +eol_tolerant_p);
	if(info.hit)
		begin = info.stop;
	http::request::data_type data;
	http::request req(&data);
	info = spirit::parse(begin, end, req);
	
	recognition rcg = do_recognize(data, info.hit, info.stop, begin, end);

	if (not_mine == rcg)
	{
		// проверим, а не HTTP расширение ли это?
		http::request_ext::data_type data;
		http::request_ext req(&data);
		info = spirit::parse(begin, end, req);
		if (info.hit)
			rcg = its_mine;
	}
	//нет необходимости делать распознавание на ответах от сервера
	//так как протокол всегда инициируется клиентом
	return rcg;
}
///////////////////////////////////////////////////////////////////////////////
size_t http::analyzer::recovery_error(char const* begin, char const* end)
{
	size_t used_bytes(0);
	http::recognizer<http::request_data> r;
	http::request::data_type req_data;
	http::request req(&req_data);
	http::message_recognizer<http::request, http::recognizer<http::request_data> > req_msg(req, r);
	char const* tmp = http::find_message(begin, end, req_msg);
	if(tmp == end)
	{
		http::recognizer<http::response_data> r;
		http::response::data_type resp_data;
		http::response resp(&resp_data);
		http::message_recognizer<http::response, http::recognizer<http::response_data> > resp_msg(resp, r);
		tmp = http::find_message(begin, end, resp_msg);
	}
	used_bytes += (tmp - begin);//! размер мусора
	return used_bytes;
}
///////////////////////////////////////////////////////////////////////////////
/**
 * выделяем из входного буфера полные заголовки сообщений, если не удается
 * выделить из входного буфера(потока), хотя бы частично, request\response
 * сообщение, то будет выброшено исключение, говорящее о том, что входной
 * поток не принадлежит протоколу HTTP
 * 
 * в методе мы всегда сначала пытаемся выделить request сообщения, а потом
 * response сообщения, это не дорогой оверхед, так как для request-ов
 * будет проверка только первых 3-10 первый символов, на соответствие
 * поддерживаемых методов.
 * Параметр force влияет только на выделение response сообщение, так как
 * в сервер уже закрыл соединение и необходимо выделить максимально возможный
 * заголовок сообщения и отослать клиенту, так как возможны ситауции, когда
 * заголовок не полностью парсится из-за отсутсвия
 * финального CRLF(ошибка Microsoft-IIS/6.0)
 */
std::pair<size_t, bool> http::analyzer::do_extract(std::auto_ptr<message>& msg, char const* begin
								, char const* end, bool force)
{
	message_type type(http::request_type);
	method_type method(_unknown);
	size_t used_bytes(0);
	bool litter(false);
	//! необходимо пропустить первые CRLF, если такие существуют это связано с тем,
	//! что некоторые клиенты версии 1.0 могут посылать дополнительный CRLF для 
	//! POST запроса, в результате нарушается BNF форма следующего ответа с сервера
	spirit::parse_info<> info = spirit::parse(begin, end, +eol_tolerant_p);
	if(info.hit)
	{
		used_bytes += info.length;
		begin = info.stop;
	}
	//! пытаемся выделить request/response сообщения
	header_t* hdrs = 0;
	response_data resp_data;
	request_data req_data;
	request req(&req_data);
	info = spirit::parse(begin, end, req);
	http::recognition request_result(do_recognize(req_data, info.hit, info.stop, begin, end));
	if(request_result == its_mine)//! найден полный заголовок для request message
	{
		hdrs = &req_data.hdrs;//! выделяем message-header для анализа пакета
		type = request_type;  //! определяем тип пакета как request
		method = request_method(req_data.method);
	}
	else
	{
		resp_data.scode = 0;
		http::response resp(&resp_data);
		info = spirit::parse(begin, end, resp);
		recognition  response_result(do_recognize(resp_data, info.hit, info.stop, begin, end));
		if(response_result == its_mine)
		{//! найден полный заголовок для response message
			hdrs = &resp_data.hdrs;	//! выделяем message-header для анализа пакета
			type = response_type;	//! определяем тип пакета как request
		}
		else if(response_result == likely_mine
				&& force
				&& !resp_data.hdrs.empty())
		{//! найден неполный заголовок для response message
			//! что смогли разобрать
			hdrs = &resp_data.hdrs;	//! выделяем message-header для анализа пакета
			type = response_type;	//! определяем тип пакета как request
		}

		if (not_mine == response_result && not_mine == request_result)
		{
			request_ext req_ext(&req_data);
			info = spirit::parse(begin, end, req_ext);
			
			if (info.hit)
			{
				// поймали расширение
				hdrs = &req_data.hdrs;
				type = request_type;
				method = request_method(req_data.method);
			}

			http::recognition request_ext_result = info.hit? its_mine : not_mine;

			//! необходима проверка, что нам передали буфер с данными, собержащими
			//! сообщения протокола HTTP. Это необходимо, если будут сначала работать
			//! на HTTP протоколе(rfc2616), а потом перейдут на одно из его расширений,
			//! например WebDav, который расширяет набор запросов к серверу.
			if(response_result == not_mine
				&& request_result == not_mine
				&& request_ext_result == not_mine
				&& used_bytes == 0)//! учитываем, что может быть получен буфер состоящий из CRLF
			{
				//! возможна ситуация, когда размер HTTP пакета отличается от размера присланных данных
				//! такое возможно для версии http сервера VxWorks 5.4. Этот сервер в некоторых RESPONSE
				//! задваиваивает message-body HTTP пакета.
				//! nакое поведение можно было наблюдать - http://img.mail.ru/r/vir.gif?1993331897
				//! пытаемся востановить протокол HTTP.
				//! возможна ситуация когда мы не сможем выделить пакет, тогда наверное нужно брость исключение!????
				used_bytes += this->recovery_error(begin, end);
				if(dump<7>::need_dump() && used_bytes)//! сбрасываем мусор в дамп
					//! ВНИМАНИЕ - если used_bytes == 0, определенный интервал [begin, begin + used_bytes) НЕКОРРЕКТЕН!!!!
					dump<7>("!litter", this).write(begin, begin + used_bytes);
				if(used_bytes)//! выполняем обработку ошибки с учетом установленной стратегии
				{
					litter = true;//! выставляем признак того что был найден и отправлен мусор на сервер или клиента
					error_(begin, begin + used_bytes);
				}
			}
		}
	}
	//! анализ длины сообщения, типа кодировки и т.п. и т.д.
	if(hdrs)
	{
		//! учитываем очередь запросов, если persistent connection
		if(supp_)
			method = (type == request_type)
						? (supp_->request_method(method), method) 
						: supp_->request_method();
		//todo: сделать оптимизацию по поиску полей,
		//		так как некоторые поля принадлежат только response message
		size_t real_size(end - info.stop);//! реальный размер полученных данных)
		size_t content_size(0);
		header_t::iterator tit = hdrs->find(field_name(_TRANSFER_ENCODING));
		header_t::iterator cit = hdrs->find(field_name(_CONTENT_LENGTH));
		bool is_transfer_encoding(tit != hdrs->end());
		//! Content-Length должна игнорироваться, если указан Transfer-Encoding
		bool is_content_length((cit != hdrs->end()) && !is_transfer_encoding);
		if(is_content_length && !is_transfer_encoding)//! найдено поле Content-Length
			content_size = http::buffer_cast<int>(cit->second, 0);//! и отсутсвует поле Transfer-Encoding
		//! check HEAD request in the request queue for persistent connection
		if(type == response_type && method == _head)//LENGTh MUSt BE ZERO FOR ALL RESPONSE
		{
			content_size = 0;//! for HEAD request response body MUST be zero
			is_content_length = true;//! the content length was specified and equals zero
		}
		//! calculate message length see rfc2616/paragraph 4.4
		if(type == response_type && content_size == 0 && !is_content_length)
		{//! исполняется только, если не было в заголовке Content-Length
			if(!(resp_data.scode > 99 && resp_data.scode < 200)
				&& !(resp_data.scode == 204 || resp_data.scode == 304))
				content_size = unspecified_length;
			//! проверяем response на handshake для SSL
			vector_ptr reason(http::buffer_cast<vector_ptr>(resp_data.reason, 0));
			reason->push_back(0);
			if(detect_ssl_handshake_response(resp_data.scode, _strlwr(&(*reason)[0])))
				content_size = 0;
		}
		char const* end_of = info.stop + std::min(content_size, real_size);
		//! создать сообщение c пустым содержимым
		msg = create_message(type, content_size, begin, info.stop, 0, 0, method);
		char const* content_begin = info.stop;
		char const* content_end = content_begin + std::min(content_size, real_size);
		if(content_begin != content_end)//! Добавляем содаржимое в сообщение
			real_size = msg->append(content_begin, content_end);
		//! вычисляем число байт разобранных из входного буфера
		used_bytes += (info.stop - begin) + std::min(content_size, real_size);
	}
	return std::make_pair(used_bytes, litter);
}
///////////////////////////////////////////////////////////////////////////////
std::auto_ptr<http::message> http::analyzer::do_extract(http::message* msg, bool force)
{
	std::auto_ptr<message> local_msg;
	if(!cache_.empty())
	{
		bool completed(msg ? msg->complete() : false);
		bool nothing(false);//если переменная установлена в true, то в кеше нет полного заголовка пакета
		cache_iterator it(cache_.begin());
		while(it != cache_.end() && !completed && !nothing)
		{
			//std::copy(it, it + chunk_size, tmp.begin());
			if(!msg)
			{
				std::pair<size_t, bool> const& result = do_extract(local_msg, it, cache_.end(), force);
				size_t used_bytes(result.first);
				if(!local_msg.get())//в кеше есть заголовок сообщения, но он не полон
				{
					//! пропускаем данные, которые были пропущены при обработке входного потока
					//! на тот случай, если в кеш попали неправильные данные, extract будет
					//! действовать в соотвествии с установленной стратегией
					nothing = (used_bytes == 0);
					it += used_bytes;
					//! в пакете есть мусор, который нужно удалить из кеша, так как его уже отправили по назначению
					if(result.second)
					{
						cache_.erase(cache_.begin(), it);
						it = cache_.begin();
					}
				}
				else
				{
					msg = local_msg.get();
					it += used_bytes;
				}
			}
			else
			{
				size_t size(msg->append(it, cache_.end()));
				it += size;//! корректируем текущий итератор
				completed = msg->complete();
				//assert(completed || size != 0);
			}
		}
		//! удаляем из кеша данные считанные в сообщение
		if(msg || !nothing)
			cache_.erase(cache_.begin(), it);
	}
	return local_msg;
}
///////////////////////////////////////////////////////////////////////////////
std::auto_ptr<http::message> http::analyzer::extract(bool force) { return do_extract(0, force); }
///////////////////////////////////////////////////////////////////////////////
void http::analyzer::extract(http::message& msg) { do_extract(&msg, false); }
///////////////////////////////////////////////////////////////////////////////
void http::analyzer::enqueue(char const* begin, char const* end)
{
	cache_.append(begin, end);
}
///////////////////////////////////////////////////////////////////////////////
//! size_t real_size - действительный размер контента вычисляется из данных http пакета
std::auto_ptr<http::message> http::analyzer::create_message(http::message_type type
														 , size_t real_size		//! действительный размер контента
														 , char const* hdr_begin
														 , char const* hdr_end
														 , char const* begin
														 , char const* end
														 , method_type request_method)
{
	typedef prague::IO<char>::iterator iterator;
	std::auto_ptr<message> msg;
	std::auto_ptr<std::vector<char> > header(new std::vector<char>);
	prague::IO<char> content(prague::create_temp_io());
	http::request_data req_data;
	http::response_data resp_data;
	header_t* hdrs = 0;
	header->reserve(hdr_end - hdr_begin);//! reserve space for header buffer
	header->insert(header->end(), hdr_begin, hdr_end);//! fill up header buffer
	content.insert(content.end(), begin, end);//! fill up content as single unit

	switch(type)
	{
	case request_type:
		{
			http::request req(&req_data);
			char const* local_beg = &(*header)[0];
			char const* local_end = local_beg + header->size();
			spirit::parse_info<> info = spirit::parse(local_beg, local_end, req);
			msg = std::auto_ptr<message>(new request_message(req_data, header, content, real_size));
			hdrs = &req_data.hdrs;
		}
		break;
	case response_type:
		{
			http::response resp(&resp_data);
			char const* local_beg = &(*header)[0];
			char const* local_end = local_beg + header->size();
			spirit::parse_info<> info = spirit::parse(local_beg, local_end, resp);
			msg = std::auto_ptr<message>(new response_message(resp_data, header, content, real_size, request_method));
			hdrs = &resp_data.hdrs;
		}
		break;
	default:
		throw std::runtime_error("http message type is undefined");
	}
	//! detect is packet has chunked message-body
	header_t::iterator tit = hdrs->find(field_name(_TRANSFER_ENCODING));
	if(tit != hdrs->end())
	{
		std::string const& s = http::buffer_cast<std::string>(tit->second, 0);
		if(_stricmp(s.c_str(), _CHUNKED) == 0)
			msg = std::auto_ptr<message>(new chunked_message(msg));
	}
	return msg;
}
///////////////////////////////////////////////////////////////////////////////
//! return cached data and clear internal cache
std::auto_ptr<std::deque<char> > http::analyzer::flush()
{
	std::auto_ptr<std::deque<char> > tmp(new std::deque<char>(cache_.size()));
	std::copy(cache_.begin(), cache_.end(), tmp->begin());
	cache_.erase(cache_.begin(), cache_.end());
	return tmp;
}
///////////////////////////////////////////////////////////////////////////////
http::analyzer* http::analyzer::clone() const
{
	return new analyzer(this->supp_); 
}
///////////////////////////////////////////////////////////////////////////////
http::support_persistent* http::analyzer::support_keep_alive(http::support_persistent* supp)
{
	support_persistent* tmp = this->supp_;
	this->supp_ = supp;
	return tmp;
}
///////////////////////////////////////////////////////////////////////////////