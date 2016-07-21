#include "stdafx.h"
#include "html.h"
#include <stdio.h>

bool CElem::aligned = true;
bool CElem::lf = true;

#define BR_STR "<br>\r\n"
#define BR_LEN 6



const char* CStr::find_crlf( const char* b, const char* e, int& l ) {
	l = 0;
	for( ; b<e; b++ ) {
		if ( (*b=='\r') && (*(b+1)=='\n') ) 
			l = 2;
		else if ( (*b=='\n') && (*(b+1)=='\r') )
			l = 2;
		else if ( (*b=='\n') || (*b=='\r') )
			l = 1;
		else
			continue;
		return b;
	}
	return e;
}


// ---
char* CStr::copy_with_br( const char* b, const char* e, bool no_trailing_space, bool add_last_br ) {
	
	char*       o;
	int         c = 0;
	const char* p = b;
	int         l;

	if ( !b || !*b ) {
		if ( add_last_br )
			return (char*)::memcpy( new char[BR_LEN+1], BR_STR, BR_LEN+1 );
		else
			return 0;
	}

	if ( !e )
		e = b + ::lstrlen( b );
	
	while( (e > b) && ((no_trailing_space && (*(e-1)==' ')) || (*(e-1)=='\r') || (*(e-1)=='\n')) )
		e--;
	
	if ( e == b )
		return 0;
	
	for( c=0,p=find_crlf(p,e,l)+l; p<e; p=find_crlf(p,e,l)+l )
		c++;
	
	if ( add_last_br )
		c++;

	l = e - b;
	o = new char[l + (c * BR_LEN) + 1];
	p = o;
	
	const char* br;
	for( br=find_crlf(b,e,l); b<e; br=find_crlf(b,e,l) ) {
		int d = br - b;
		::memcpy( (char*)p, b, d );
		p += d;
		if ( l ) {
			::memcpy( (char*)p, BR_STR, BR_LEN );
			p += BR_LEN;
		}
		b += d + l;
	}
	
	if ( add_last_br )
		::memcpy( (char*)p, BR_STR, BR_LEN+1 );
	else
		*(char*)p = 0;
	return o;
}



// ---
bool CStr::flush( HANDLE file, int shift ) {
	DWORD written;
	if ( CElem::lf && aligned ) {
		char align[20];
		while( shift ) {
			int pad = min( shift, sizeof(align) );
			::memset( align, ' ', pad );
			::WriteFile( file, align, pad, &written, 0 );
			shift -= pad;
		}
	}
	written = ::lstrlen( m_str );
	::WriteFile( file, m_str, written, &written, 0 );
	CElem::lf = m_lf;
	return true;
}



// ---
bool CHtml::write_tag( HANDLE file, int shift, bool opened ) {
	DWORD written;

	if ( !opened && !m_closing )
		return true;

	if ( CElem::lf && CElem::aligned ) {
		char  align[20];
		while( shift ) {
			int pad = min( shift, sizeof(align) );
			::memset( align, ' ', pad );
			::WriteFile( file, align, pad, &written, 0 );
			shift -= pad;
		}
	}

	if ( opened ) {
		::WriteFile( file, "<", 1, &written, 0 );
		::WriteFile( file, m_tag, ::lstrlen(m_tag), &written, 0 );
		int i, c;
		for( i=0,c=m_attrs.Count(); i<c; i++ ) {
			const char* attr = m_attrs[i];
			::WriteFile( file, " ", 1, &written, 0 );
			::WriteFile( file, attr, ::lstrlen(attr), &written, 0 );
		}
	}
	else {
		::WriteFile( file, "</", 2, &written, 0 );
		::WriteFile( file, m_tag, ::lstrlen(m_tag), &written, 0 );
	}

	if ( CElem::aligned ) {
		CElem::lf = true;
		::WriteFile( file, ">\r\n", 3, &written, 0 );
	}
	else {
		CElem::lf = false;
		::WriteFile( file, ">", 1, &written, 0 );
	}
	return true;
}




// ---
void CHtml::attr( const char* name, const char* val ) {
	int nlen = ::lstrlen( name );
	int vlen = (val ? ::lstrlen(val) : 0 );
	char* na = (char*)::memcpy( new char[nlen+vlen+1+3], name, nlen );
	if ( val ) {
		*(na+nlen++) = '=';
		if ( *val != '\"' )
			*(na+nlen++) = '\"';
		::memcpy( na+nlen, val, vlen ); nlen += vlen;
		if ( *val != '\"' )
			*(na+nlen++) = '\"';
	}
	*(na+nlen) = 0;
	m_attrs.Add( na );
}



// ---
void CHtml::line( const char* line ) {
	char* nl = CStr::copy_with_br( line, 0, true, true );
	m_elems.Add( new CStr(nl,BR_LEN>4) );
}



// ---
void CHtml::str( const char* str ) {
	char* nl = CStr::copy_with_br( str, 0, false, false );
	if ( nl )
		m_elems.Add( new CStr(nl,false) );
}



// ---
bool CHtml::flush( HANDLE file, int shift ) {
	int i, c;
	
	if ( !write_tag(file,shift,true) )
		return false;

	for( i=0,c=m_elems.Count(); i<c; i++ ) {
		if ( !m_elems[i]->flush(file,shift+2) )
			return false;
	}
	
	if ( !write_tag(file,shift,false) )
		return false;
	return true;
}




// ---
CHtml* CHtml::table( CPArray<char>& titles, int shift ) {
	uint   i, c;
	CHtml* title;
	CHtml* head;
	CHtml* table = para(shift)->add( "table" );

	//table->attr( "border", "1" );
	head = table->add("thead")->add( "tr" );
	
	for( i=0,c=titles.Count(); i<c; i++ ) {
		title = head->add( "th" );  
		//title->attr( "width", "1" );
		title->line( titles[i] );
	}
	
	return table->add( "tbody" );
}



// ---
CHtml* CHtml::href( const char* url, bool internal ) {
	char* nurl = 0;
	
	if ( !url || !*url )
		return 0;

	if ( internal && (*url != '#') ) {
		int len = ::lstrlen( url ) + 1;
		nurl = new char[ len + 1 ];
		*nurl = '#';
		::memcpy( nurl+1, url, len );
		url = nurl;
	}
	
	CHtml* el = new CHtml( "a", "href", url, true );
	if ( el )
		m_elems.Add( el );
	if ( nurl )
		delete [] nurl;
	return el;
}



