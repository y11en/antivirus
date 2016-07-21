/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	exceptions.hpp
*		\brief	возможные исключения
*		
*		\author Владислав Овчарик
*		\date	04.06.2005 11:46:18
*		
*		
*/
#if !defined(_HTTP_EXCEPTIONS_INCLUDE_HPP_)
#define _HTTP_EXCEPTIONS_INCLUDE_HPP_
///////////////////////////////////////////////////////////////////////////////
#include <stdexcept>
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
/**
 * \breif - класс исключение необходим выбрасывается при невозожности выделить 
 *			(распарсить) сообщение из входного потока. Т.е. входной поток не
 *			содержит ни request message ни response message
 */
class extract_error
	: public std::invalid_argument
{
public:
	extract_error(std::string const& wrong_packet)
		: std::invalid_argument(std::string("extract error:") + wrong_packet)
	{
	}
};
///////////////////////////////////////////////////////////////////////////////
}//namespace http
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(_HTTP_EXCEPTIONS_INCLUDE_HPP_)
