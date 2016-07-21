/*!
*		
*		
*		(C) 2005 Kaspersky Lab 
*		
*		\file	recognizer.hpp
*		\brief	
*		
*		\author Vladislav Ovcharik
*		\date	23.09.2005 20:06:39
*		
*		
*/		
#if !defined(RECOGNIZER_H_INCLUDED)
#define RECOGNIZER_H_INCLUDED
#include "../../../TrafficMonitor2/TrafficProtocollerTypes.h"
///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
namespace tp = TrafficProtocoller;
///////////////////////////////////////////////////////////////////////////////
enum recognition
{
	not_mine = tp::dcProtocolUnknown,			//! not a http protocol
	its_mine = tp::dcDataDetected,				//! its a header of the http packet
	likely_mine = tp::dcProtocolLikeDetected	//! some of the data is a http packet
};
///////////////////////////////////////////////////////////////////////////////
template<typename D>
struct recognizer
{	
	/***/
	static http::recognition detect(D const& data, bool hit
									, char const* stop
									, char const* begin
									, char const* end);
	/***/
	http::recognition operator() (D const& data, bool hit
									, char const* stop
									, char const* begin
									, char const* end) const
	{
		return recognizer<D>::detect(data, hit, stop, begin, end);
	}
};
///////////////////////////////////////////////////////////////////////////////
}//namespace http
#endif//!defined(RECOGNIZER_H_INCLUDED)