//////////////////////////////////////////////////////////////////////////
// Header file buffer manipulation

#include <string.h>

//////////////////////////////////////////////////////////////////////////

#define CHECK_BUFFER() \
	if( !m_buff ) return true; \
	if( m_size < m_cur_size ) return false;

class PRPrmBuff
{
public:
	PRPrmBuff(tCHAR *p_buff, tDWORD p_size) :
	  m_buff(p_buff), m_size(p_size), m_cur_size(0){}

	tERROR Error()
	{
		return m_buff && m_size < m_cur_size ?
			errBUFFER_TOO_SMALL : errOK;
	}
	void StrEnd()
	{
		if( m_buff ) m_buff++;
		m_cur_size++;
	}
	tERROR SetStr(const char *str)
	{
		(*this)<<str; StrEnd();
		return Error();
	}

	bool Null()
	{
		m_cur_size += 1;
		CHECK_BUFFER()
		*m_buff = 0;
		return true;
	}
	bool operator << (tCHAR p_v)
	{
		m_cur_size += sizeof(tCHAR)+1;
		if( m_buff )
		{
			if( m_size < m_cur_size ) return false;
			*m_buff ++ = p_v;
			*m_buff = 0;
		}
		m_cur_size--;
		return true;
	}
	bool operator << (tSTRING p_v)
	{
		if( !p_v ) p_v = "";
		tDWORD l_len = (tDWORD)strlen(p_v)+1;
		m_cur_size += l_len;
		if( m_buff )
		{
			if( m_size < m_cur_size ) return false;
			memcpy(m_buff, p_v, l_len);
			m_buff += l_len-1;
		}
		m_cur_size --;
		return true;
	}
	void operator >> (tSTRING &p_v)
	{
		tDWORD l_len = (tDWORD)strlen(m_buff)+1;
		p_v = m_buff;
		m_buff += l_len;
		m_cur_size += l_len;
	}
	bool operator << (const unsigned char *p_v)
	{
		return operator << ((tSTRING)p_v);
	}
	bool operator << (const char *p_v)
	{
		return operator << ((tSTRING)p_v);
	}
	bool operator << (tDWORD p_v)
	{
		m_cur_size += sizeof(tDWORD);
		CHECK_BUFFER()
		*(tDWORD*)m_buff = p_v;
		m_buff += sizeof(tDWORD);
		return true;
	}
	void operator >> (tDWORD &p_v)
	{
		p_v = *(tDWORD*)m_buff;
		m_buff += sizeof(tDWORD);
		m_cur_size += sizeof(tDWORD);
	}

	bool operator << (tQWORD p_v)
	{
		m_cur_size += sizeof(tQWORD);
		CHECK_BUFFER()
		*(tQWORD*)m_buff = p_v;
		m_buff += sizeof(tQWORD);
		return true;
	}
	void operator >> (tQWORD &p_v)
	{
		p_v = *(tQWORD*)m_buff;
		m_buff += sizeof(tQWORD);
		m_cur_size += sizeof(tQWORD);
	}

#ifdef _WINBASE_
	bool operator << (FILETIME *p_v)
	{
		SYSTEMTIME l_st;
		FileTimeToSystemTime(p_v, &l_st);
		SystemTimeToTzSpecificLocalTime(NULL, &l_st, &l_st);

		m_cur_size += 20;
		CHECK_BUFFER()
		sprintf_s(m_buff, 20, "%4d/%02d/%02d %02d:%02d:%02d", l_st.wYear, l_st.wMonth, l_st.wDay,
			l_st.wHour, l_st.wMinute, l_st.wSecond, l_st.wMilliseconds);
		m_buff += 19; m_cur_size--;
		return true;
	}
#endif

public:
	tCHAR *	m_buff;
	tDWORD	m_size;
	tDWORD	m_cur_size;
};

//////////////////////////////////////////////////////////////////////////
