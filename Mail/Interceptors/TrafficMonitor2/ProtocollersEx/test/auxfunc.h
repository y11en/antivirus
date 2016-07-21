/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	auxfunc.h
*		\brief	вспомогательные функции для поддержки тестирования
*		
*		\author Владислав Овчарик
*		\date	27.04.2005 9:59:54
*		
*		
*/		
#pragma once
#include <list>
#include <memory>
#include <string>
///////////////////////////////////////////////////////////////////////////////
namespace aux
{
///////////////////////////////////////////////////////////////////////////////
//! return list of files that contain http packet
std::auto_ptr<std::list<std::string> > http_packets(char const* src_dir);
//! return pair of mapped memory pointer & size of memory
std::pair<char*, size_t> create_http_stream(std::string const& f);
std::pair<char*, size_t> create_http_stream(std::list<std::string> const& l);
///////////////////////////////////////////////////////////////////////////////
}//namespace aux
///////////////////////////////////////////////////////////////////////////////