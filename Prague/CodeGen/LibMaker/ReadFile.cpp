#include "stdafx.h"
#include "ReadFile.h"

char* dup_s(const char* s, int l)
{
	if (!s || !*s)
		return 0;
	
	int c = ::lstrlen(s);
	if ( l == -1 )
		l = c;
	else if (l < c)
		c = l;
	
	s = (const char*)::memcpy( new char[l+1], s, c);
	*(char*)(s+c) = 0;
	return (char*)s;
}

void trim( char* s ) {
	char* p = s;
	while( *p == ' ' )
		p++;
	int l = ::lstrlen( p );
	if ( p > s )
		::memmove( s, p, l+1 );
	p = s + l;
	while( (p > s) && (*(p-1) == ' ') )
		p--;
	*p = 0;
}

static const char* find( const char* s, int c, int l ) {
	if ( !s || !*s )
		return 0;
	if ( !l )
		l = INT_MAX;
	for( int i=0; *s && i<l; i++,s++ )
		if ( *s == c )
			return s;
		return 0;
}

static const char* find_back( const char* s, int c, int l ) {
	int i;
	if ( !s || !*s || !l )
		return 0;
	for( i=l-1, s=s+l-1; i>=0; i--,s-- )
		if ( *s == c )
			return s;
		return 0;
}

bool replace( char*& str, const char* to_replace, const char* value ) {
	
	if ( !str || !*str || !to_replace || !*to_replace )
		return false;
	
	char* f = ::strstr( str, to_replace );
	if ( f ) { // found string to replace
		int len = ::lstrlen( str );
		int len_rep = ::lstrlen( to_replace );
		int len_val = value ? ::lstrlen(value) : 0;
		int diff = len_rep - len_val;
		
		if ( !diff ) {
			if ( len_val )
				::memcpy( f, value, len_val );
		}
		else if ( diff > 0 ) {
			if ( len_val )
				::memcpy( f, value, len_val );
			::memmove( f+len_val, f+len_rep, len-(f-str)-len_rep+1 );
		}
		else {
			int shift = f - str;
			char* tmp;
			if ( shift )
				tmp = (char*)::memcpy( new char[len-diff+1], str, shift );
			else
				tmp = new char[len-diff+1];
			if ( len_val )
				::memcpy( tmp+shift, value, len_val );
			::memcpy( tmp+(shift+len_val), f+len_rep, len-shift-len_rep+1 );
			delete [] (char*)str;
			str = tmp;
		}
		
	}
	return true;
}

static char* get_token( const char* src, int& pos ) {
	if ( src[pos] == 0 )
		return 0;
	const char* colon = ::strchr( src+pos, ':' );
	if ( colon && ((*(colon+1)=='\\') || (*(colon+1)=='/')) )
		colon = ::strchr( colon+1, ':' );
	if ( colon ) {
		int p = pos;
		pos = colon - src + 1;
		while( *(src+pos) && (' ' == *(src+pos)) )
			pos++;
		while( *(colon-1) == ' ' )
			colon--;
		return ::dup_s( src+p, colon-src-p );
	}
	else {
		int p = pos;
		pos += ::lstrlen( src+pos );
		return ::dup_s( src+p );
	}
}

bool is_file( const char* src ) {
	bool found = false;
	const char* n;
	char* f = ::strchr( src, '\x28' );
	if ( f )
		n = ::dup_s( src, f-src );
	else
		n = src;
	
	HANDLE h = ::CreateFile(n,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if ( h != INVALID_HANDLE_VALUE ) {
		found = true;
		::CloseHandle( h );
	}
	else {
		DWORD err = GetLastError();
		switch( err ) {
      case ERROR_FILE_NOT_FOUND: 
      case ERROR_PATH_NOT_FOUND: 
      case ERROR_INVALID_NAME:
			break;
      case ERROR_ACCESS_DENIED:
			found = true;
			break;
		}
	}
	
	if ( n != src )
		delete [] (char*)n;
	return found;
}

// -----------------------------------------------------------------------------------------

const CReadFile::m_bsize = 0x20;

CReadFile::CReadFile(HANDLE file)
{
	m_file = file;
	m_flen = GetFileSize(m_file, 0);
	m_fpos = 0;
	m_buff = new char[m_bsize],
	m_bpos = m_bsize,
	m_blen = m_bsize;	
}

CReadFile::~CReadFile()
{
	delete []m_buff;
}

bool CReadFile::readln(char*& line, int& length)
 {
	int len;
	int line_len = 0;
	const char* found = 0;
	
	line = 0;
	while(update() && (0 !=(len = m_blen-m_bpos)))
	{
		int nc;
		
		found = (const char*) memchr(m_buff + m_bpos, '\r', len);
		if (found)
			nc = '\n';
		else {
			found = (const char*) memchr(m_buff + m_bpos, '\n', len);
			nc = '\r';
		}
		if (found)
			len = found - (m_buff + m_bpos);
		
		if (len)
		{
			char* tmp = new char[line_len + len + 1];
			if (line_len)
			{
				memcpy(tmp, line, line_len);
				delete []line;
			}
			line = tmp;
			memcpy(line + line_len, m_buff + m_bpos, len);
			line_len += len;
			*(line + line_len) = 0;
			m_bpos += len;
		}
		
		if (found)
		{
			m_bpos++;
			if (update() && (nc == *(m_buff + m_bpos)))
				m_bpos++;
			break;
		}
	}
	length = line_len;
	return found || (line_len > 0);
}

bool CReadFile::update()
{
	if (m_bpos >= m_blen)
	{
		if (m_fpos < m_flen)
		{
			DWORD rest = m_flen - m_fpos;
			DWORD read_now = rest > m_bsize ? m_bsize : rest;
			if (ReadFile(m_file,m_buff,read_now,&read_now,0) && read_now)
			{
				m_fpos += read_now;
				m_blen = read_now;
				m_bpos = 0;
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}
	else
		return true;
}

// -----------------------------------------------------------------------------------------
CErrorLine::~CErrorLine()
{
	if (m_path)
		delete [](char*)m_path;
	if (m_file)
		delete [](char*)m_file;
	if (m_str_id)
		delete [](char*)m_str_id;
	if (m_msg)
		delete [](char*)m_msg;
}

CErrorLine::CErrorLine( char*& src, const char* ignore_path, const char* ignore_f_name ) 
	: m_path(0), m_file(0), m_line(0), m_add(false), m_is_err(false), m_str_id(0), m_msg(0)
{
	if ( !src || !*src )
		return;
	
	if ( ::strstr(src,"Microsoft") ) {
		m_msg = src;
		src = 0;
		m_is_err = false;
		return;
	}
	
	int len = ::lstrlen( src );
	while( *(src+len-1) == ' ' )
		*(src + --len) = 0;
	bool file_not_found = true;
	int pos = 0;
	char* token = get_token( src, pos );
	while( token ) {
		trim( token );
		if ( file_not_found && is_file(token) ) {
			file_not_found = false;
			const char* start_line = ::find( token, '\x28', 0 );
			if ( start_line ) {
				char* p;
				token[ start_line-token ] = 0;
				m_line = ::strtol( ++start_line, &p, 10 );
			}
			
			const char* file = ::find_back( token, '\\', ::lstrlen(token) );
			if ( file ) {
				file++;
				m_file = ::dup_s( file );
				*(char*)file = 0;
				m_path = ::dup_s( token );
				if ( !::lstrcmpi(m_path,ignore_path) ) {
					delete [] (char*)m_path;
					m_path = 0;
				}
				if ( !::lstrcmpi(m_file,ignore_f_name) ) {
					delete [] (char*)m_file;
					m_file = 0;
				}
			}
			else {
				m_file = token;
				token = 0;
				if ( !::lstrcmpi(m_file,ignore_f_name) ) {
					delete [] (char*)m_file;
					m_file = 0;
				}
			}
		}
		else if ( !m_str_id && ::strstr(token,"error ") ) {
			replace( token, "error ", 0 );
			m_is_err = true;
			m_str_id = token;
			token = 0;
		}
		else if ( !m_str_id && ::strstr(token,"warning ") ) {
			replace( token, "warning ", 0 );
			m_str_id = token;
			token = 0;
		}
		else if ( !m_msg ) {
			m_msg = token;
			token = 0;
		}
		else {
			int l = ::lstrlen(m_msg) + ::lstrlen(token) + 1 + 3/*" : "*/;
			m_msg = ::dup_s( m_msg, l );
			::lstrcat( (char*)m_msg, " : " );
			::lstrcat( (char*)m_msg, token );
		}
		
		if ( token )
			delete [] token;
		token = get_token( src, pos );
	}
}


// -----------------------------------------------------------------------------------------

CId::CId(DWORD line, const char* method_name ) 
	: m_method_name( method_name ), m_line(line), m_cls(0), m_subcls(0), m_method(0)
{
}

CId::CId(const CId& o ) 
	: m_method_name(dup_s(o.m_method_name)), m_line(o.m_line), m_cls(o.m_cls), m_subcls(o.m_subcls), m_method(o.m_method)
{
}

CId::~CId()
{
	if (m_method_name) delete [](char*)m_method_name;
}
