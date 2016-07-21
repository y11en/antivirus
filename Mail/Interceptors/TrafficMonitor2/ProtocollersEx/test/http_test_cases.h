/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	http_test_cases.h
*		\brief	описание тестов для http traffic monitor-а
*		
*		\author Владислав Овчарик
*		\date	27.04.2005 11:44:51
*		
*		
*/		
#pragma once
#include <string>
///////////////////////////////////////////////////////////////////////////////
namespace http
{
bool test_chunk_parser(char const* begin, char const* end);
/**
 * \breif - сценарий, проверяющий функциональность детектирования входного потока с данными
 *			на принадлежность потока протоколу http
 *	begin - начало потока
 *	end   - конец потока
 *	\result - возвращает true если тест пройден успешно. в консоль выводится информация о
 *			  результате детектирования.
 */
bool test_detect_traffic(char const* begin, char const* end);
/**
 * \brief - простейший сценарий, на вход анализатора поступает поток данных,
 *			который заведомо принадлежит протоколу http, в результате анализатор
 *			должен веделить полные пакеты из потока и записать их содержимое в
 *			файл. Данные из потока считываются блоками произвольными длинны. 
 *	begin - начало потока
 *	end   - конец потока
 *  out   - имя выходного файла для содержимого потока http
 *	\result - возвращает true если тест пройден успешно и в птоке не осталось данных.
 */
bool test_full_packet_traffic(char const* begin, char const* end, std::string const& out);
/**
 * \breif - сценарий по которому данны в анализатор поступают асинхронно.
 *			в целом сценарий повторяет test_full_packet_traffic
 */
bool test_async_full_packet_traffic(char const* begin, char const* end, std::string const& out);
//!
void xoring_file(std::string const& fname, std::string const& out);
//!
bool test_processor(char const* begin, char const* end);
//!
bool test_processor(std::list<std::string> const& l);
}//namespace http
///////////////////////////////////////////////////////////////////////////////