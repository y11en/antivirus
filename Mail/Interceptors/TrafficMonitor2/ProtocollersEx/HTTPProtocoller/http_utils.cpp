/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	http_utils.cpp
*		\brief	
*		
*		\author Владислав Овчарик
*		\date	28.06.2005 14:45:21
*		
*		
*/		
#define NOMINMAX
#pragma warning(disable:4786)
#include <windows.h>
#include <Prague/prague.h>
#include <http/analyzerimpl.h>
#include <Prague/wrappers/io_helpers.h>
#include "http_utils.h"
#include "resource.h"
#include "xor.hpp"

#include <version/ver_product.h>
#include <string>
using std::string;

///////////////////////////////////////////////////////////////////////////////
namespace
{
///////////////////////////////////////////////////////////////////////////////
__int64 const xor_key = 0xBC5C0E69EC4845E3;
///////////////////////////////////////////////////////////////////////////////
char const easter_egg_html[] = "avp/easter-egg.html";
char const kl_gif[] = "avp/kl.gif";
char const virus_gif[] = "avp/virus.gif";
char const virus_red_gif[] = "avp/virus_red.gif";
char const infected_wav[] = "avp/infected.wav";
char const bloodstain_gif[] = "avp/bloodstain.gif";
char const def_product[] = "Kaspersky Internet Security";
char const env_sub_key[] = VER_PRODUCT_REGISTRY_PATH "\\environment";
///////////////////////////////////////////////////////////////////////////////
//! страничка, которая появляется в результате удаления содержимого http пакета
char const templ_html[] = 
						"<html>\r\n"
						"<head>\r\n"
						"<title>%PRODUCT%</title>\r\n"
						"<STYLE type=\"text/css\"><!--BODY{background-color:#ffffff;font-family:verdana,sans-serif}PRE{font-family:sans-serif}--></STYLE>"
						"</head>\r\n"
						"<body>\r\n"
						"<h1>ACCESS DENIED</h1>\r\n"
						"<h2>The requested URL could not be retrieved</h2>\r\n"
						"<hr noshade size=\"1px\">\r\n"
						"<p>While trying to retrieve the URL: <u>%URL%</u></p>\r\n"
						"<p>The folowing error was encountered:\r\n"
						"<ul>\r\n"
						"<li><strong>The requested object is INFECTED. The following viruses <a href=http://www.viruslist.com/en/search?VN=%VIRUSNAME%>%VIRUSNAME%</a> were found</strong>\r\n"
						"</ul>\r\n"
						"</p>\r\n"
						"<p>Please contact your service provider if you feel this is incorrect.</p>"
						"<BR clear=\"all\">\r\n"
						"<HR noshade size=\"1px\">\r\n"
						"<ADDRESS>\r\n"
						"Generated %DATE% by %PRODUCT%\r\n"
						"</ADDRESS>\r\n"
						"</body>\r\n"
						"</html>";

//! страничка, которая появляется на запрос запрещенного URL пользователем
char const forbidden_header[] =
						"HTTP/1.1 499 Request has been forbidden by antivirus\r\n"
						"Server: kav/7.0\r\n"
						"Content-Type: text/html\r\n"
						"Content-Length: 0\r\n"
						"Connection: close\r\n"
						"\r\n";
char const forbidden_html[] = 
						"<html>\r\n"
						"<head>\r\n"
						"<title>%PRODUCT%</title>\r\n"
						"<STYLE type=\"text/css\"><!--BODY{background-color:#ffffff;font-family:verdana,sans-serif}PRE{font-family:sans-serif}--></STYLE>"
						"</head>\r\n"
						"<body>\r\n"
						"<h1>ACCESS DENIED</h1>\r\n"
						"<h2>The requested URL could not be retrieved</h2>\r\n"
						"<hr noshade size=\"1px\">\r\n"
						"<p>While trying to retrive the URL: <u>%URL%</u></p>\r\n"
						"<p>The folowing error was encountered:\r\n"
						"<ul>\r\n"
						"<li><strong>The requested object is forbidden.</strong>\r\n"
						"</ul>\r\n"
						"</p>\r\n"
						"<BR clear=\"all\">\r\n"
						"<HR noshade size=\"1px\">\r\n"
						"<ADDRESS>\r\n"
						"Generated %DATE% by %PRODUCT%\r\n"
						"</ADDRESS>\r\n"
						"</body>\r\n"
						"</html>";

//! страничка, которая появляется на запрос запрещенного баннера
char const adblock_header[] =
						"HTTP/1.1 200 OK\r\n"
						"Server: kav/7.0\r\n"
						"Content-Type: text/html\r\n"
						"Content-Length: 0\r\n"
						"Connection: close\r\n"
						"\r\n";
char const adblock_html[] = "<PRE><B>AD</B></PRE>";
///////////////////////////////////////////////////////////////////////////////
//! empty image header
char const image_header[] =
						"HTTP/1.1 200 OK\r\n"
						"Server: kav/7.0\r\n"
						"Content-Length: 0\r\n"
						"Content-Language: en\r\n"
						"Content-Type: image/gif\r\n"
						"Cache-Control: max-age=1000\r\n"
						"Connection: close\r\n"
						"\r\n";
//! transparent GIF 1x1 pixel
unsigned char const image_data[] = { 
	0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x01, 0x00, 0x01, 0x00, 0x80, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0x21, 0xF9, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x01, 0x00, 0x00, 0x02, 0x02, 0x44, 0x01, 0x00, 0x3B
};

///////////////////////////////////////////////////////////////////////////////
//! empty Shock-Wave Flash 
char const swf_header[] =
						"HTTP/1.1 200 OK\r\n"
						"Server: kav/7.0\r\n"
						"Content-Length: 0\r\n"
						"Content-Language: en\r\n"
						"Content-Type: application/x-shockwave-flash\r\n"
						"Cache-Control: max-age=1000\r\n"
						"Connection: close\r\n"
						"\r\n";
unsigned char const swf_data[] = { 
	0x46, 0x57, 0x53, 0x01, 0x25, 0x00, 0x00, 0x00, 0x78, 0x00, 0x05, 0x5F, 0x00, 0x00, 0x0F, 0xA0,
	0x00, 0x00, 0x0C, 0x01, 0x00, 0x43, 0x02, 0xEC, 0xE9, 0xD8, 0x3F, 0x03, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x40, 0x00, 0x00, 0x00 };

///////////////////////////////////////////////////////////////////////////////
char const easter_egg[] =
						"HTTP/1.1 200 OK\r\n"
						"Server: kav/7.0\r\n"
						"Content-Length: 0\r\n"
						"Content-Language: en\r\n"
						"Cache-Control: max-age=1000\r\n"
						"Connection: close\r\n"
						"\r\n";
}//namespace unnamed

std::string utils::encode_url(char const* url)
{
	std::string r;
	char c;
	char buf[4] = {'%', 0, 0, 0};
	while(c = *url++)
	{
		// see RFC 1738
		if ( isalnum( static_cast< unsigned char >( c ) )
			|| c == '%'
			// special characters
			|| c == '$' || c == '-' || c == '_' || c == '.' || c == '+' || c == '!' || c == '*' || c == '\'' 
			|| c == '(' || c == ')' || c == ','
			// reserved characters
			|| c == ';' || c == '/' || c == '?' || c == ':' || c == '@' || c == '=' || c == '&')
		{
			r += c;
		}
		else
		{
			wsprintf(buf + 1, "%2X", c);
			r += buf;
		}
	}
	return r;
}
///////////////////////////////////////////////////////////////////////////////
//! формирует HTML страничку, содержащую информацию о вирусе
http::vector_ptr utils::make_default_html(char const* html, char const* url, char const* virus_name)
{
	typedef util::scoped_handle<HKEY, LONG (WINAPI*)(HKEY), &::RegCloseKey, 0, 0> reg_key_t;
	string code = html ? html : "";
	string product = def_product ? def_product : "";
	reg_key_t key;
	if(::RegOpenKeyExA(HKEY_LOCAL_MACHINE, env_sub_key, 0, KEY_READ, &key.get_ref()) == ERROR_SUCCESS)
	{
		unsigned long size(0);
		unsigned long type(0);
		if(::RegQueryValueExA(key.get(), "ProductName", 0, &type, 0, &size) == ERROR_SUCCESS)
		{
			char * buff = new char[size + 1];
			if (buff)
			{
				::RegQueryValueExA(key.get(), "ProductName", 0, &type, (LPBYTE)buff, &size);
				buff[size]=0;
				product = buff;
				delete [] buff;
			}
		}
	}

	// PRODUCT replace
	string::size_type pos = code.find("%PRODUCT%");
	while(pos != string::npos)
	{
		code.replace(pos, strlen("%PRODUCT%"), product.c_str(), product.length());
		pos = code.find("%PRODUCT%");
	}
	
	// URL replace
	pos = code.find("%URL%");
	std::string encoded_url = url ? encode_url(url) : "";
	while(pos != string::npos)
	{
		code.replace(pos, strlen("%URL%"), encoded_url.c_str(), encoded_url.length());
		pos = code.find("%URL%");
	}

	// VIRUSNAME replace
	pos = code.find("%VIRUSNAME%");
	std::string virName = virus_name ? virus_name : "";
	while(pos != string::npos)
	{
		code.replace(pos, strlen("%VIRUSNAME%"), virName.c_str(), virName.length());
		pos = code.find("%VIRUSNAME%");
	}

	// DATE replace
	pos = code.find("%DATE%");
	time_t cur_time = time(0);
	std::string date;
	if(char *pTime = ctime(&cur_time))
		date = pTime;	
	while(pos != string::npos)
	{
		code.replace(pos, strlen("%DATE%"), date.c_str(), date.length());
		pos = code.find("%DATE%");
	}

	return http::vector_ptr(new std::vector<char>(code.begin(), code.end()));
}
///////////////////////////////////////////////////////////////////////////////
//! return mesage with forbidden response code
std::auto_ptr<http::message> utils::make_forbidden_message(char const* url)
{
	http::analyzer a;
	a.enqueue(forbidden_header, forbidden_header + sizeof(forbidden_header) - 1);
	std::auto_ptr<http::message> forbidden_msg(a.extract());
	prague::IO<char> err_io(prague::create_temp_io());
	http::vector_ptr html(make_default_html(forbidden_html, url, 0));
	char const* begin = &(*html)[0];
	char const* end = begin + html->size();
	err_io.insert(err_io.end(), begin, end);
	forbidden_msg->replace(err_io.release());
	return forbidden_msg;
}
///////////////////////////////////////////////////////////////////////////////
//! return mesage with AD block response code
std::auto_ptr<http::message> utils::make_adblock_message(char const* url)
{
	http::analyzer a;
	a.enqueue(adblock_header, adblock_header + sizeof(adblock_header) - 1);
	std::auto_ptr<http::message> adblock_msg(a.extract());
	prague::IO<char> err_io(prague::create_temp_io());
	http::vector_ptr html(make_default_html(adblock_html, url, 0));
	char const* begin = &(*html)[0];
	char const* end = begin + html->size();
	err_io.insert(err_io.end(), begin, end);
	adblock_msg->replace(err_io.release());
	return adblock_msg;
}
///////////////////////////////////////////////////////////////////////////////
//! replace mesage with tiny transparent GIF
std::auto_ptr<http::message>  utils::make_empty_image_reply()
{
	http::analyzer a;
	a.enqueue(image_header, image_header + sizeof(image_header) - 1);
	std::auto_ptr<http::message> image_msg(a.extract());
	prague::IO<char> err_io(prague::create_temp_io());
	http::vector_ptr image(new std::vector<char>);
	const char* begin = (const char*)&image_data[0];
	const char* end = begin + sizeof(image_data);
	image->insert(image->end(), begin, end);
	http::vector_ptr html(image);
	begin = &(*html)[0];
	end = begin + html->size();
	err_io.insert(err_io.end(), begin, end);
	image_msg->replace(err_io.release());
	return image_msg;
}

///////////////////////////////////////////////////////////////////////////////
//! replace mesage with tiny transparent GIF
std::auto_ptr<http::message>  utils::make_empty_swf_reply()
{
	http::analyzer a;
	a.enqueue(swf_header, swf_header + sizeof(swf_header) - 1);
	std::auto_ptr<http::message> swf_msg(a.extract());
	prague::IO<char> err_io(prague::create_temp_io());
	http::vector_ptr image(new std::vector<char>);
	const char* begin = (const char*)&swf_data[0];
	const char* end = begin + sizeof(swf_data);
	image->insert(image->end(), begin, end);
	http::vector_ptr html(image);
	begin = &(*html)[0];
	end = begin + html->size();
	err_io.insert(err_io.end(), begin, end);
	swf_msg->replace(err_io.release());
	return swf_msg;
}

///////////////////////////////////////////////////////////////////////////////
void utils::replace_as_default_html(http::message* msg, std::vector<char> const* vname, char const* url)
{
	char const* virus_name = "UNSPECIFIED VIRUS NAME";
	if(vname && !vname->empty())
		virus_name = &(*vname)[0];
	//todo: ЗДЕСЬ НЕОБХОДИМО ФОРМИРОВАТЬ НОВЫЙ КОНТЕНТ В ЗАВИСИМОСТИ ОТ ТИПА КОНТЕНТА ПАКЕТА
	prague::IO<char> err_io(prague::create_temp_io());
	http::vector_ptr html(make_default_html(templ_html, url, virus_name));
	char const* begin = &(*html)[0];
	char const* end = begin + html->size();
	err_io.insert(err_io.end(), begin, end);
	msg->replace(err_io.release());
}
///////////////////////////////////////////////////////////////////////////////
std::auto_ptr<http::message>  utils::make_default_forbidden_reply(char const* url, std::vector<char> const* vname)
{
	http::analyzer a;
	a.enqueue(forbidden_header, forbidden_header + sizeof(forbidden_header) - 1);
	std::auto_ptr<http::message> forbidden_msg(a.extract());
	replace_as_default_html(forbidden_msg.get(), vname, url);
	return forbidden_msg;
}
