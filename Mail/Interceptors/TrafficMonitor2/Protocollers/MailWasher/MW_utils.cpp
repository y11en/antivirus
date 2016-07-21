#include "../../stdafx.h"
#include "MW_utils.h"

#include <Prague/pr_oid.h>
#include <Prague/iface/i_io.h>

#include <Extract/iface/i_mailmsg.h>
#include <Extract/plugin/p_msoe.h>
#pragma warning(disable:4786)

tBOOL NeedMoreData(tCHAR* data, tINT size)
{
	return (0 != memcmp(data+size-2, "\r\n", 5 ));
}

tBOOL NeedMoreMultyLineData(tCHAR* data, tINT size)
{
	return (0 != memcmp(data+size-5, "\r\n.\r\n", 5 ));
}

tBOOL FindPos(lookahead_stream& data, tQWORD begin, tQWORD& end, tCHAR* delimeter)
{
	tQWORD dwDelimeterSize = strlen(delimeter);
	if ( !dwDelimeterSize )
		return cFALSE;
	tQWORD ii = 0;
	tQWORD i = 0;
	tQWORD size = data.get_size();
	if ( !data.seek(begin) )
		return cFALSE;

	for ( i = 0; i < size; i++ )	
	{	
		if ( 
			data.get() == delimeter[ii] &&	
			i+1 <= size &&
			ii+1 <= dwDelimeterSize
			)
		{
			ii++;
			if ( ii == dwDelimeterSize )
			{
				end = begin+i;
				break;
			}
		}
		else
			ii = 0;
	}
	data.seek(0);
	return (ii == dwDelimeterSize);
}

tERROR GetIntLineParameter(tINT number, lookahead_stream& data, tQWORD begin, tQWORD end, tDWORD& dwParameter)
{
	tQWORD dwParameterBegin = begin;
	tQWORD dwParameterEnd = begin;
	tQWORD size = data.get_size();
	if ( !data.seek(begin) )
		return errOBJECT_SEEK;
	
	for ( tINT i = 0; i < number+1; i++ )
	{
		dwParameterBegin = dwParameterEnd;
		if (
			!FindPos(data, dwParameterBegin+1, dwParameterEnd, " ") ||
			dwParameterEnd > end
			)
		{
			dwParameterBegin += 1;
			if ( !FindPos(data, dwParameterBegin+1, dwParameterEnd, "\r\n") )
				break;
			else
				dwParameterEnd -= 1;
		}
	}
	if ( dwParameterBegin < dwParameterEnd ) 
	{
		tQWORD sz_size = dwParameterEnd-dwParameterBegin;
		tVOID* sz1 = malloc((size_t)(sz_size+1));
		if ( sz1 )
		{
			memset(sz1, 0, (tUINT)(sz_size+1));
			data.seek(dwParameterBegin);
			for (tQWORD qw = 0; qw < sz_size; )
				((tCHAR*)sz1)[qw++] = data.get();
			dwParameter = atoi((tCHAR*)sz1);
			free(sz1);
		}
	}
	data.seek(0);
	return errOK;
}

tERROR GetStrLineParameter(tINT number, lookahead_stream& data, tQWORD begin, tQWORD end, cStringObj* sz)
{
	cERROR err = errOK;
	tQWORD dwParameterBegin = begin;
	tQWORD dwParameterEnd = begin;
	tQWORD size = data.get_size();
	if ( !data.seek(begin) )
		return errOBJECT_SEEK;

	for ( tINT i = 0; i < number+1; i++ )
	{
		dwParameterBegin = dwParameterEnd;
		if (
			!FindPos(data, dwParameterBegin+1, dwParameterEnd, " ") ||
			dwParameterEnd > end
			)
		{
			dwParameterBegin += 1;
			if ( !FindPos(data, dwParameterBegin+1, dwParameterEnd, "\r\n") )
				break;
			else
				dwParameterEnd -= 1;
		}
	}
	if ( dwParameterBegin < dwParameterEnd ) 
	{
		tQWORD sz_size = dwParameterEnd-dwParameterBegin;
		tVOID* sz1 = malloc((size_t)(sz_size+1));
		if ( sz1 )
		{
			memset(sz1, 0, (tUINT)(sz_size+1));
			data.seek(dwParameterBegin);
			for (tQWORD qw = 0; qw < sz_size; )
				((tCHAR*)sz1)[qw++] = data.get();
			sz->assign(sz1, cCP_ANSI);
			free(sz1);
		}
	}
	data.seek(0);
	return err;
}

tERROR ParseLIST(MessageList_t* pMessageList, hIO io, cPOP3Session* pPOP3Session, OUT tDWORD* pdwMessageCount, OUT tDWORD* pdwMailboxSize)
{
	cERROR err = errOK;
	lookahead_stream data(1024); data.open(io);
	tQWORD size = data.get_size();
	tQWORD dwLineBegin = 0;
	tQWORD dwLineEnd = size;

	if ( 
		data.compare("+OK", 3) && 
		FindPos(data, dwLineBegin, dwLineEnd, "\r\n") )
	{
		err = GetIntLineParameter(1, data, 0, dwLineEnd, *pdwMessageCount);
		err = GetIntLineParameter(2, data, 0, dwLineEnd, *pdwMailboxSize);
	}
	
	if ( 
		data.compare("+OK", 3) && 
		FindPos(data, dwLineBegin, dwLineEnd, "\r\n") )
	{
		dwLineBegin = dwLineEnd+1;
		while ( FindPos(data, dwLineBegin, dwLineEnd, "\r\n") )
		{
			cPOP3Message hPOP3Message;
			*(cPOP3Session*)&hPOP3Message = *pPOP3Session;
			err = GetIntLineParameter(0, data, dwLineBegin, dwLineEnd, hPOP3Message.m_dwNumber);
			err = GetIntLineParameter(1, data, dwLineBegin, dwLineEnd, hPOP3Message.m_dwSize);
			if ( hPOP3Message.m_dwNumber && hPOP3Message.m_dwSize )
			{
				hPOP3Message.m_bNew = hPOP3Message.m_dwNumber >= pPOP3Session->m_dwFirstNewMessage;
				(*pMessageList)[hPOP3Message.m_dwNumber] = hPOP3Message;
			}
			dwLineBegin = dwLineEnd+1;
		}
	}

	pPOP3Session->m_dwMessageCount = pMessageList->size();
	pPOP3Session->m_dwNewMessageCount = pPOP3Session->m_dwMessageCount - pPOP3Session->m_dwFirstNewMessage +1;
	return err;
}

//tERROR ParseUIDL(MessageList_t* pMessageList, hIO io)
//{
//	cERROR err = errOK;
//	lookahead_stream data(1024); data.open(io);
//	tQWORD size = data.get_size();
//	tQWORD dwLineBegin = 0;
//	tQWORD dwLineEnd = size;
//
//	if ( 
//		data.compare("+OK", 3) && 
//		FindPos(data, dwLineBegin, dwLineEnd, "\r\n") )
//	{
//		dwLineBegin = dwLineEnd+1;
//		while ( FindPos(data, dwLineBegin, dwLineEnd, "\r\n") )
//		{
//			tDWORD dwMessageNumber = 0;
//			err = GetIntLineParameter(0, data, dwLineBegin, dwLineEnd, dwMessageNumber);
//			cPOP3Message& hPOP3Message = (*pMessageList)[dwMessageNumber];
//			err = GetStrLineParameter(1, data, dwLineBegin, dwLineEnd, &(hPOP3Message.m_szUIDL));
//			dwLineBegin = dwLineEnd+1;
//		}
//	}
//
//	return err;
//}
//
//tERROR ParseTOP(MessageList_t* pMessageList, tDWORD dwMessageNumber, hIO io)
//{
//	cERROR err = errOK;
//	lookahead_stream data(1024); data.open(io);
//	tQWORD size = data.get_size();
//	tQWORD dwLineBegin = 0;
//	tQWORD dwLineEnd = size;
//
//	if ( 
//		data.compare("+OK", 3) && 
//		FindPos(data, dwLineBegin, dwLineEnd, "\r\n") )
//	{
//		dwLineBegin = dwLineEnd+1;
//		cPOP3Message& hPOP3Message = (*pMessageList)[dwMessageNumber];
//		tINT iRecv = size;
//		{
//			// Вынимаем имя письма
//			hOS hMessageOS = NULL;
//			err = io->set_pgOBJECT_ORIGIN(OID_MAIL_MSG_MIME);
//			err = io->sysCreateObjectQuick( (hOBJECT*)&hMessageOS, IID_OS, PID_MSOE );
//			err = hPOP3Message.m_szName.assign(hMessageOS, pgOBJECT_NAME); 
//			err = hPOP3Message.m_szData.assign(hMessageOS, pgMESSAGE_INTERNET_HEADERS);
//			hMessageOS->sysCloseObject();
//			err = io->set_pgOBJECT_ORIGIN(0);
//		}
//		KLSTD_TRACE0(KLMC_TRACELEVEL, "ParseTOP => =========================================\n");
//		KLSTD_TRACE3(KLMC_TRACELEVEL, "ParseTOP => %d, %d, \n \t %s\n", hPOP3Message.m_dwNumber, hPOP3Message.m_dwSize, (tCHAR*)(hPOP3Message.m_szName.c_str(cCP_ANSI)) );
//	}
//	
//	return err;
//}
