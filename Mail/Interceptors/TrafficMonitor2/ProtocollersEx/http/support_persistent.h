/*!
*		
*		
*		(C) 2005 Kaspersky Lab 
*		
*		\file	support_persistent.h
*		\brief	
*		
*		\author Vladislav Ovcharik
*		\date	03.02.2006 11:24:37
*		
*		
*/		
#if !defined(_SUPPORT_PERSISTENT_H_INCLUDED)
#define _SUPPORT_PERSISTENT_H_INCLUDED
#include "http.h"
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
struct __declspec(novtable) support_persistent
{
	virtual http::method_type	request_method() const = 0;
	virtual void				request_method(http::method_type) = 0;
};
///////////////////////////////////////////////////////////////////////////////
}//namespace http
#endif//!defined(_SUPPORT_PERSISTENT_H_INCLUDED)