/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	message.h
*		\brief	описание интерфейсов сообщений HTTP протокола
*		
*		\author Владислав Овчарик
*		\date	26.04.2005 16:15:29
*		
*		
*/
#if !defined(_HTTP_MESSAGE_INCLUDE_H_)
#define _HTTP_MESSAGE_INCLUDE_H_
///////////////////////////////////////////////////////////////////////////////
#include "http.h"
#include "action.h"
#include <time.h>

#include <Prague/iface/i_io.h>
///////////////////////////////////////////////////////////////////////////////
namespace http
{

///////////////////////////////////////////////////////////////////////////////
//! fwd declaration
class analyzer;
class chunked_message;
class generic_message;
///////////////////////////////////////////////////////////////////////////////
//! length of the message-body is not specified exactly. It's may be either
//!	"Transfer-Encoding=chuncked" or length will determine at the moment of 
//!	connection close
size_t const unspecified_length = 0xffffffffu;
///////////////////////////////////////////////////////////////////////////////
//! возможные типы сообщений
enum message_type
{
	request_type	= 1,
	response_type	= 2
};
///////////////////////////////////////////////////////////////////////////////
//! возможные типы контентов
enum content_type
{
	_specific,
	_mime,
	_html,
	_binary,
	_application,
	_audio,
	_image,
	_message,
	_model,
	_multipart,
	_text,
	_video
};

//////////////////////////////////////////////////////////////////////////
// Possible content encodings
enum content_encoding
{
	specific_encoding = 0,
	gzip_encoding,		// x-gzip, gzip
	compress_encoding,	// x-compress, compress
	deflate_encoding	// deflate
};

//////////////////////////////////////////////////////////////////////////
// Possible cache control values
enum cache_control
{
	specific_cache_control = 0,
	no_cache
};

///////////////////////////////////////////////////////////////////////////////
/**
 * \brief	-	интерфейс для управления заголовками пакета
 *				message-header is a pair of field-name and field-value
 */
struct __declspec(novtable) message_headers
{
	//! add a new message-header. return true if header will add successful
	virtual bool add(buffer_t const& field_name, buffer_t const& field_value) = 0;
	//! return copy of field-value at the flat buffer
	//! return 0 if the requested name will not find
	virtual vector_ptr get(buffer_t const& field_name) const = 0;
	//! remove message-header by name
	virtual bool remove(buffer_t const& field_name) = 0;
	//! replace existing message-header or add new
	virtual void replace(buffer_t const& field_name, buffer_t const& field_value) = 0;
	//! add a new message-header and remove 
	virtual void add_remove(buffer_t const& remove_name
						, buffer_t const& field_name
						, buffer_t const& field_value) = 0;

	virtual ~message_headers() {}
};
/**
 * \brief - интерфейс сообщения между клиентом и сервером
 */
class __declspec(novtable) message
{
public:
	//! данные описывающие какой объем информации должен быть собран
	struct build_data
	{
		build_data()
			: length(0)
			, position(0)
		{
		}
		static bool has_built(build_data const* bd) { return bd && (bd->position != 0); }
		size_t length;		//! количество 
		size_t position;	//! начиная с какой позиции
	};
	//! return type of http message
	virtual message_type type() const = 0;
	//! return true if message-body has been received completely
	virtual bool complete() const = 0;
	//! return content type
	virtual content_type contenttype() const = 0; 
	//! return content encoding
	virtual content_encoding contentencoding() const = 0;
	//! return message content
	virtual hIO content() const = 0;
	//! return message-headers & request/response line as buffer
	virtual std::vector<char> const& heading() const = 0;
	//! return full assembled packet. consumer should release hIO object
	virtual hIO assemble(build_data* d) const = 0;
	//! replace message-body(content) and return previous IO.
	//! Customer must release IO if it will be need
	virtual hIO replace(hIO) = 0;
	//! apply action on message
	virtual void apply(base_action& ) = 0;
	//! return size of content as single unit
	virtual size_t size() const = 0;
	//! return size of generic-message (message-body length & header length)
	virtual size_t max_size() const = 0;
	//! append data to the content (message-body)
	virtual size_t append(char const* begin, char const* end) = 0;
	//! return reference to object that controls message headers
	virtual message_headers& headers() = 0;
	//! return whether message is ready for deliver
	virtual bool ready_to_deliver() const = 0;
	//! set ready for deliver
	virtual void set_ready_to_deliver(bool ready) = 0;
	//!
	virtual	~message()
	{
	}
};
///////////////////////////////////////////////////////////////////////////////
}//namespace http
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(_HTTP_MESSAGE_INCLUDE_H_)