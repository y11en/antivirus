/*!
*		
*		
*		(C) 2005 Kaspersky Lab 
*		
*		\file	preprocessing.h
*		\brief	
*		
*		\author Vladislav Ovcharik
*		\date	28.09.2005 10:32:42
*		
*		
*/		
#if !defined(PREPROCESSING_H_INCLUDED)
#define PREPROCESSING_H_INCLUDED
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
class message;
///////////////////////////////////////////////////////////////////////////////
struct __declspec(novtable) preprocessing
{
	/**
	 * return true if requested resource should not be processed
	 */
	virtual bool skip_that(char const* url) const = 0;
	/**
	 * return true if received packed should be skipped
	 */
	virtual bool skip_message(message const& msg) const = 0;
	/**
	 * return timeout for the message
	 */
	virtual clock_t timeout() const = 0;
};
///////////////////////////////////////////////////////////////////////////////
}//namespace http
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(PREPROCESSING_H_INCLUDED)