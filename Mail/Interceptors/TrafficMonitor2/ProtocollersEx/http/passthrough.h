/*!
*		
*		
*		(C) 2005 Kaspersky Lab 
*		
*		\file	passthrough.h
*		\brief	
*		
*		\author Vladislav Ovcharik
*		\date	28.02.2006 9:31:56
*		
*		
*/		
#if !defined(_PASSTHROUGH_H_INCLUDED)
#define _PASSTHROUGH_H_INCLUDED
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
enum msg_direction
{
	to_server,		//from client to server
	to_client,		//from server to client
	this_is_big_ass
};
///////////////////////////////////////////////////////////////////////////////
struct __declspec(novtable) passthrough
{
	virtual void passto(msg_direction dir, char const* beg, char const* end) = 0;
protected:
	virtual ~passthrough() {}
};
///////////////////////////////////////////////////////////////////////////////
}//namesapce http
///////////////////////////////////////////////////////////////////////////////
#endif//!defiend(_PASSTHROUGH_H_INCLUDED)