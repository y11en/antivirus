/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	interval.h
*		\brief	
*		
*		\author Vitaly DVi Denisov
*		\date	05.02.2004 17:51:47
*		
*		Example:	
		INTERVAL_BEGIN
		{
			// Получение времени, прошедшего с момента INTERVAL_BEGIN (в миллисекундах)
			unsigned long ulTimeWasSpent = INTERVAL_GET_CURRENT(INTERVAL_NOW);
			// Переход на метку quit. если таймаут ulTimeOut истек с момента INTERVAL_BEGIN
			INTERVAL_CHECK_TIMEOUT(ulTimeOut, quit, err); 
			// Получить, сколько соталось от текущего таймаута
			INTERVAL_GET_TIME_LEFT(ulTimeOut, INTERVAL_NOW);
		}
		INTERVAL_END;
*		
*		
*		
*/		



#ifndef _INTERVAL_H_
#define _INTERVAL_H_


#if defined (WIN32)
	#include "windows.h"
#else 
	#include <sys/time.h>
	tDWORD GetTickCount()
	{
		timeval tv;
		gettimeofday(&tv, 0);
		return tv.tv_usec*1000;
	}
#endif

#define INTERVAL_NOW	GetTickCount()
#define INTERVAL_BEGIN(/*unsigned long*/_ulBeginTicks_)	\
	{\
		unsigned int ulBeginTicks	=	_ulBeginTicks_;
#define INTERVAL_GET_CURRENT(/*unsigned long*/_ulNow_)	\
		( _ulNow_ >= ulBeginTicks ) ?	\
			( _ulNow_ - ulBeginTicks ) :	\
			( 0xffffffff - ulBeginTicks + _ulNow_ );
#define INTERVAL_CHECK_TIMEOUT(/*unsigned long*/_ulTimeOut_, _quit_point_, /*tERROR*/_error_)	\
		{	\
			unsigned long ulNow = INTERVAL_GET_CURRENT(INTERVAL_NOW);	\
			if ( (_ulTimeOut_ - ulNow) < 0 ) \
			{	\
				_error_ = errTIMEOUT;	\
				goto _quit_point_;	\
			}	\
		}
#define INTERVAL_GET_TIME_LEFT(/*unsigned long*/_ulTimeOut_, /*unsigned long*/_ulNow_)	\
		(unsigned long)( _ulNow_ >= ulBeginTicks ) ? ( _ulNow_ - ulBeginTicks ) : 0
	
#define INTERVAL_END	\
	}


#endif//_INTERVAL_H_