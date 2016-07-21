/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	MIMETransport.h
*		\brief	Coding/Encoding of MIME between native and transport format
*		
*		\author Vitaly DVi Denisov
*		\date	27.01.2005 15:39:52
*		
*		Example:	
*		
*		
*		
*/		
#ifndef _MIMETRANSPORT_H_
#define _MIMETRANSPORT_H_

#include <Prague/prague.h>
#include <Prague/iface/i_io.h>

#define TEMP_BUFFER_SIZE	1024

inline tERROR ReplaceCRLFDotDotToCRLFDot( hIO in, hIO out )
{
	cERROR err = errOK;
	//////////////////////////////////////////////////////////////////////////
	//
	// заменяем  \r\n.. на \r\n.
	//
	if ( in != out )
		out->SetSize(0);
	tQWORD qwReadPos = 0;
	tQWORD qwWritePos = 0;
	tDWORD dwRead = 0;
	tDWORD dwWritten = 0;
	tCHAR pBuf[TEMP_BUFFER_SIZE]; memset(pBuf,         0,           TEMP_BUFFER_SIZE);
	tCHAR pCRLFDot[3];            memcpy(pCRLFDot,     "\r\n.",     3);
	tCHAR pCRLFDotDot[4];         memcpy(pCRLFDotDot,  "\r\n..",    4);
	tCHAR pEND_OF_DATA[5];        memcpy(pEND_OF_DATA, "\r\n.\r\n", 5);
	do 
	{
		dwRead = 0;
		dwWritten = 0;
		memset(pBuf,0,TEMP_BUFFER_SIZE);

		err = in->SeekRead( &dwRead, qwReadPos, pBuf, TEMP_BUFFER_SIZE);
		if ( PR_SUCC(err) && dwRead ) 
		{
			tDWORD i = 0;
			tDWORD i_begin = i;
			do
			{
				i_begin = i;
				// Ищем '\r'
				bool found = false;
				while(i < dwRead && !(found = pBuf[i] == '\r'))
					++i;
				// Записываем весь буфер, предваряющий '\r'
				err = out->SeekWrite( &dwWritten, qwWritePos, pBuf + i_begin, i - i_begin );
				qwWritePos += dwWritten;
				
				if(found)
				{
					tDWORD dwLeft = dwRead - i;
					tDWORD dwRead5 = 0;
					tCHAR pBuf5[5]; memset(pBuf5,0,5);
					if ( dwLeft < 5 )
						// Прочитаем еще 5 байт от позиции i, не сдвигая qwReadPos
						err = in->SeekRead( &dwRead5, qwReadPos + i, pBuf5, 5 );
					else
						memcpy( pBuf5, pBuf + i, 5 );
					if ( 0 == memcmp(pBuf5, pEND_OF_DATA, 5 ) )
						// Достигнут конец данных - прекращаем декодирование
						goto END_OF_DATA;
					else if ( 0 == memcmp(pBuf5, pCRLFDotDot, 4) )
					{
						err = out->SeekWrite( &dwWritten, qwWritePos, pCRLFDot, 3 );
						i += 4;
					}
					else
					{
						err = out->SeekWrite( &dwWritten, qwWritePos, pBuf5, 1 );
						i += 1;
					}
					qwWritePos += dwWritten;
				}
			}
			while ( i < dwRead );
			qwReadPos += i;
		}
	} 
	while ( PR_SUCC(err) && dwRead );
	//
	// заменяем  \r\n.. на \r\n.
	//
	//////////////////////////////////////////////////////////////////////////
END_OF_DATA:
	err = out->SetSize(qwWritePos);
	return err;
}

inline tERROR ReplaceCRLFDotToCRLFDotDot( hIO in, hIO out )
{
	cERROR err = errOK;
	//////////////////////////////////////////////////////////////////////////
	//
	// заменяем  \r\n. на \r\n..
	//
	if ( in != out )
		out->SetSize(0);
	tQWORD qwReadPos = 0;
	tQWORD qwWritePos = 0;
	tDWORD dwRead = 0;
	tDWORD dwWritten = 0;
	tCHAR pBuf[TEMP_BUFFER_SIZE]; memset(pBuf,         0,           TEMP_BUFFER_SIZE);
	tCHAR pCRLFDot[3];            memcpy(pCRLFDot,     "\r\n.",     3);
	tCHAR pCRLFDotDot[4];         memcpy(pCRLFDotDot,  "\r\n..",    4);
	tCHAR pEND_OF_DATA[5];        memcpy(pEND_OF_DATA, "\r\n.\r\n", 5);
	do 
	{
		dwRead = 0;
		dwWritten = 0;
		memset(pBuf,0,TEMP_BUFFER_SIZE);

		err = in->SeekRead( &dwRead, qwReadPos, pBuf, TEMP_BUFFER_SIZE);
		if ( PR_SUCC(err) && dwRead ) 
		{
			tDWORD i = 0;
			tDWORD i_begin = i;
			do
			{
				i_begin = i;
				// Ищем '\r'
				bool found = false;
				while(i < dwRead && !(found = pBuf[i] == '\r'))
					++i;
				// Записываем весь буфер, предваряющий '\r'
				err = out->SeekWrite( &dwWritten, qwWritePos, pBuf + i_begin, i - i_begin );
				qwWritePos += dwWritten;
				
				if(found)
				{
					tDWORD dwLeft = dwRead - i;
					tDWORD dwRead3 = 0;
					tCHAR pBuf3[3]; memset(pBuf3,0,3);
					if ( dwLeft < 3 )
						// Прочитаем еще 3 байта от позиции i, не сдвигая qwReadPos
						err = in->SeekRead( &dwRead3, qwReadPos + i, pBuf3, sizeof(pBuf3));
					else
						memcpy(pBuf3, pBuf + i, 3);
					if ( 0 == memcmp(pBuf3, pCRLFDot, 3 ) )
					{
						err = out->SeekWrite( &dwWritten, qwWritePos, pCRLFDotDot, 4 );
						i += 3;
					}
					else
					{
						err = out->SeekWrite( &dwWritten, qwWritePos, pBuf3, 1 );
						i += 1;
					}
					qwWritePos += dwWritten;
				}
			}
			while ( i < dwRead );
			qwReadPos += i;
		}
	} 
	while ( PR_SUCC(err) && dwRead );
	//
	// заменяем  \r\n. на \r\n..
	//
	//////////////////////////////////////////////////////////////////////////
//END_OF_DATA:
	err = out->SeekWrite( &dwWritten, qwWritePos, pEND_OF_DATA, 5 );
	return err;
}

#endif//_MIMETRANSPORT_H_