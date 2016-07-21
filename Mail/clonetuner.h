/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	clonetuner.h
*		\brief	Inline CloneTuner function
*		
*		\author Vitaly DVi Denisov
*		\date	02.03.2004 12:06:22
*		
*		Example:	
*		
*		
*		
*/		

#ifndef _CLONETUNER_H_
#define _CLONETUNER_H_

#include "stdafx.h"
#include <iface/i_registrysummator.h>

inline cRegistrySummator* CloneTuner( cRegistrySummator* pRegSum, const char* szKeyFormat, ... )
{
	va_list args;
	va_start(args, szKeyFormat);
	cRegistrySummator* pRegSum_Result;
	int nChars		=	512; 
	int nUsed		=	-1;
	int nTriesLeft	=	4;
	char* szBuffer	= (char*)malloc( nChars+1 );

	do	
	{
		nChars += 2048;
		nUsed = _vsnprintf(
			szBuffer,
			nChars,
			szKeyFormat,
			args);
		
		if ( nUsed < 0 )
		{
			free( szBuffer );
			szBuffer = (char*)malloc( nChars+1 );
		}
		
	} while ( nUsed < 0 && --nTriesLeft > 0);

	tERROR err = pRegSum->CloneTuner(&pRegSum_Result, szBuffer);

	free( szBuffer );
	va_end(args);
	return pRegSum_Result;
}

#endif//_CLONETUNER_H
