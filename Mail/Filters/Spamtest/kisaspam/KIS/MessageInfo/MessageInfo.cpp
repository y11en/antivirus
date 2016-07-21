#include "messageinfo.h"

extern IStream * CreateIStreamIO(hIO p_io, tBOOL p_del_io); 

void CreateIStreamBody(IStream **pStream, hIO hBody)
{
	if (pStream) 
	{
		*pStream=NULL;
		if ( hBody ) 
			*pStream = CreateIStreamIO(hBody, cFALSE);
	}
}

char *CreateCopyString(IN hOBJECT obj, IN tDWORD dwProp)
{
	cStringObj sz;
	sz.assign(obj, dwProp);    
	cStrBuff buf = sz.c_str(cCP_ANSI);
	size_t szAlloc = buf.count() ? buf.count() : 1;
	char *p_sz = (char *)malloc(szAlloc);
	if(p_sz)
	{
		if(buf.count())
			memcpy(p_sz, (char *)buf, buf.count());
		else
			*p_sz = 0;
	}
	return p_sz;
}

void DeleteString(IN char*& p_sz)
{
	if ( p_sz )
		free((char*)p_sz);
	p_sz = NULL;
}

bool IsHTML(hIO hBody)
{
	cStringObj sz;
	if ( 
		PR_SUCC(sz.assign(hBody, pgOBJECT_NAME)) &&
		sz == MSG_CONTENT_HTML_BODY
		)
		return true;
	if ( 
		PR_SUCC(sz.assign(hBody, pgMESSAGE_PART_CONTENT_TYPE)) &&
		PR_SUCC(sz.toupper()) &&
		sz == "TEXT/HTML"
		)
		return true;
	return false;
}

bool IsRTF( hIO hBody )
{
	cStringObj sz;
	if ( 
		PR_SUCC(sz.assign(hBody, pgOBJECT_NAME)) &&
		sz == MSG_CONTENT_RICH_BODY
		)
		return true;
	return false;
}

bool IsImage(hIO io)
{
	cStringObj sz;
	if ( PR_SUCC(sz.assign(io, pgMESSAGE_PART_CONTENT_TYPE)) )
	{
		sz.toupper();
		if ( 0 == sz.find("IMAGE/") )
			return true;
	}
	return false;
}

MESSAGE_INFO_KIS::~MESSAGE_INFO_KIS()
{
	for ( tDWORD dwCounter = 0; dwCounter < m_ios.size(); dwCounter++ )
	{
		hIO io = m_ios.at(dwCounter);
		if ( io )
		{
			io->sysCloseObject();
			io = NULL;
		}
	}
	m_ios.clear();

	if ( m_optr )
	{
		m_optr->sysCloseObject();
		m_optr = NULL;
	}
}


MESSAGE_INFO_KIS::MESSAGE_INFO_KIS(hOS p_os): 
	INFO_KIS((hOBJECT)p_os),
	m_os(p_os),
	m_optr(NULL)
{
	cERROR err = errOK;
	
	hIO io;
	if ( m_os && PR_SUCC(err = m_os->PtrCreate(&m_optr, NULL)) )
	{
		while ( m_optr && PR_SUCC(err = m_optr->Next()) )
		{
			err = m_optr->IOCreate(&io, 0, fACCESS_READ, fOMODE_OPEN_IF_EXIST);
			if ( PR_SUCC(err) )
			{
				// Запоминаем только тела писем и приаттаченные картинки (размеом не более 30 Кбайт)
				if ( 
					(io->get_pgOBJECT_ORIGIN() == OID_MAIL_MSG_BODY) ||
					IsImage(io)
					)
					m_ios.push_back(io);
				else
					io->sysCloseObject();
			}
		}
	}
}


MESSAGEPART_INFO* MESSAGE_INFO_KIS::GetMessagePart(unsigned long num)
{
	if ( num < m_ios.size() )
		return new MESSAGEPART_INFO_KIS(m_ios.at(num)); 
	return NULL; 
}

void MESSAGE_INFO_KIS::ReleaseMessagePart(MESSAGEPART_INFO* pMessagePart)
{
	if ( pMessagePart )
		delete pMessagePart;
}

const char* MESSAGE_INFO_KIS::GetPropStr(  unsigned long ulPropID )
{
	return INFO_KIS::GetPropStr(ulPropID);
}
const long  MESSAGE_INFO_KIS::GetPropLong( unsigned long ulPropID )
{
	return INFO_KIS::GetPropLong(ulPropID);
}

const char* MESSAGE_INFO_KIS::GetRawMessage()
{
	tERROR err = errOK;
	if(m_os)
	{
		hIO io = (hIO)m_os->sysGetParent(IID_IO);
		if(io)
		{
			tQWORD size = 0;
			if(PR_SUCC(err = io->GetSize(&size, IO_SIZE_TYPE_EXPLICIT)))
			{
				char *p = (char *)malloc((tDWORD)size + 1);
				if(p)
				{
					tDWORD read = 0, chunk_size = min(1024, (tDWORD)size);
					tQWORD offset = 0;
					while(PR_SUCC(io->SeekRead(&read, offset, (tPTR)(p + offset), chunk_size)) && read)
						offset += read;
					p[size] = 0;
					return p;
				}
			}
		}
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////////


MESSAGEPART_INFO_KIS::MESSAGEPART_INFO_KIS(hIO io): 
	INFO_KIS((hOBJECT)io),
	m_io(io),
	m_lpStream(NULL)
{
	if ( m_io )
		m_lpStream = CreateIStreamIO(m_io, cFALSE);
}

MESSAGEPART_INFO_KIS::~MESSAGEPART_INFO_KIS()
{
	ReleaseData();
}

IStream* MESSAGEPART_INFO_KIS::GetData()
{
	return m_lpStream;
}

void MESSAGEPART_INFO_KIS::ReleaseData()
{
	if (m_lpStream)
	{
		m_lpStream->Release();
		m_lpStream = NULL;
	}
}

const char* MESSAGEPART_INFO_KIS::GetPropStr(  unsigned long ulPropID )
{
	return INFO_KIS::GetPropStr(ulPropID);
}
const long  MESSAGEPART_INFO_KIS::GetPropLong( unsigned long ulPropID )
{
	if ( pgMESSAGE_PART_CONTENT_LENGTH == ulPropID )
	{
		tQWORD size = 0;
		if ( PR_SUCC(m_io->GetSize(&size, IO_SIZE_TYPE_EXPLICIT)))
			return (long)size;
	}
	return INFO_KIS::GetPropLong(ulPropID);
}


//////////////////////////////////////////////////////////////////////////

INFO_KIS::INFO_KIS(hOBJECT obj): m_obj(obj)
{
}

INFO_KIS::~INFO_KIS()
{
	for ( tDWORD i = 0; i < m_str_props.size(); i++ )
		DeleteString(m_str_props.at(i).sz);
}

const char* INFO_KIS::GetPropStr(  unsigned long ulPropID )
{
	char* sz = 0;
	int num = m_str_props.find(ulPropID);
	if ( -1 != num )
		return m_str_props.at(num).sz;
	sz = CreateCopyString( m_obj, ulPropID);
	if ( sz )
	{
		prop_t& prop = m_str_props.push_back(ulPropID);
		prop.sz = sz;
	}
	return sz;
}
const long  INFO_KIS::GetPropLong( unsigned long ulPropID )
{
	if ( m_obj )
		return m_obj->propGetDWord(ulPropID);
	return 0;
}
