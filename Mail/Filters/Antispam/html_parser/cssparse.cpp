// cssparse.cpp
//
// $Revision: 1.5 $
// $Date: 2000/05/06 13:12:47 $
//

#include "htmparse.h"

static
void	css_skip_white( const char **pps )
{
 const char *ps = *pps;
 while( isspace(*ps) ) ++ps;
 if( *ps==0 ) { *pps=ps; return; }
 while(true) {
	if( ps[0]!='/' || ps[1]!='*' ) break;
	// skip comment
	ps += 2;
	while(true) {
		if( *ps==0 ) { *pps = ps; return; }
		if( ps[0]=='*' && ps[1]=='/' ) { ps+=2; break; }
		++ps;
	}
 }
 *pps = ps;
}

static 
bool	css_scan_name( const char **pps, string& buf )
{
 buf.resize(0);
 css_skip_white( pps );
 while(true) {
	char c = **pps;
	if( !(isalnum(c) || c=='-' || c=='_') ) break;
	buf += c;
	++(*pps);
 }
 return buf.size()!=0;
}

static
bool	css_scan_value( const char **pps, string& buf )
{
 buf.resize(0);
 css_skip_white( pps );
 while(true) {
	char c = **pps;
	if( c=='\"' || c=='\'' ) {
		// scan quoted
		char ec = c;
		buf += c;
		++(*pps);
		while(true) {
			c = **pps;
			if( c==0 ) return false;

			buf += c;
			++(*pps);

			if( c==ec ) break;
			if( c=='\\' ) {
				c = **pps;
				if( c==0 ) return false;
				buf += c;
				++(*pps);
			}
		}
	} else {
		// skip embedded comment
		if( c=='/' && (*pps)[1]=='*' ) css_skip_white(pps);
		c = **pps;
		if( c==';' || c==0 ) break;

		buf += c;
		++(*pps);
	}
 }

 // trim trailing spaces
 int l = buf.length();
 while( l>0 && isspace(buf[l-1]) ) --l;
 buf.resize(l);

 return true;
}

// quick-and-dirty CSS parser
bool	html_style_parse( const char *str, html_elt_list& style )
{
 const char *p = str;

 string nbuf;
 string vbuf;

 html_attr *last = NULL;

 while(true) {
	nbuf.resize(0);
	vbuf.resize(0);

	if( !css_scan_name( &p, nbuf ) ) break;

	css_skip_white( &p );
	if( *p=='=' || *p==':' ) {
		++p; 
		css_scan_value( &p, vbuf );
		if( *p==';' ) ++p;
	}

	html_attr *attr = RC_OBJECT_NEW( html_attr );
	if ( attr )
	{
		attr->name = html_alloc_string( nbuf.c_str(), nbuf.length() );
		attr->value = html_alloc_string( vbuf.c_str(), vbuf.length() );
	}

	if( style.head==NULL ) style.head = attr;
	if( last!=NULL ) last->next = attr; else style.head = attr;
	last = attr;
 }
 return true;
}

static
void	css_format_elt( html_attr *attr, string& buf )
{
 buf += attr->name;
 buf += ':';
 buf += attr->value;
 buf += ';';
}

bool	html_style_format( html_attr *style, html_attr *nstyle, string& buf )
{
 int ncnt = 0;
 html_element *pe = nstyle;
 for( ; pe!=NULL; pe=pe->next ) ++ncnt;

 html_attr **ppna = (html_attr **)alloca( ncnt * sizeof(html_attr *) );
 int i = 0;
 for( pe=nstyle; pe!=NULL; pe=pe->next ) {
	ppna[i++] = dynamic_cast<html_attr *>(pe);
 }

 buf.resize(0);

 for( pe=style; pe!=NULL; pe=pe->next ) {
	html_attr *pa = dynamic_cast<html_attr *>(pe);
	html_attr *attr = pa;
	for( i=0; i<ncnt; ++i ) {
		if( ppna[i]!=NULL && strcmp(pa->name,ppna[i]->name)==0 ) {
			attr = ppna[i];
			ppna[i] = NULL;
			break;
		}
	}
	if( !buf.empty() ) buf += ' ';
	css_format_elt( attr, buf );
 }

 for( i=0; i<ncnt; ++i ) {
	if( ppna[i]!=NULL ) {
		if( !buf.empty() ) buf += ' ';
		css_format_elt( ppna[i], buf );
	}
 }

 return true;
}

// returns number of characters parsed, -1 if error
int	css_parse_url( const char *str, int len, string& url )
{
 const unsigned char *p = (const unsigned char *)str;
 const unsigned char *pe = p + len;

 url.resize(0);

 while( p<pe && *p==' ' ) ++p;
 if( p==pe ) return p-(const unsigned char *)str;

 if( pe-p<5 || strncmp( (const char *)p, "url(", 4 )!=0 ) return -1;
 p += 4;

 while( p<pe && *p==' ' ) ++p;

 bool f_ok = false;

 for( ; p<pe; ++p ) {
	int c = *p;
	if( c==')' || c==' ' ) { f_ok=true; break; }
	if( c=='\\' ) {
		if( ++p==pe ) break;
		c = *p;
	}
	if( c==0 || c<0x20 ) break;
	url += c;
 }
 if( f_ok ) {
	while( p<pe && *p==' ' ) ++p;
	if( p<pe && *p==')' ) {
		++p;
	} else {
		f_ok = false;
	}
 }
 if( !f_ok ) {
	url.resize(0);
	return -1;
 }
 return p - (const unsigned char *)str;
}

bool	css_format_url( const char *url, string& s )
{
 s.resize(0);

 while( *url==' ' ) ++url;
 if( url==0 ) return true;

 s = "url(";
 const char *p = url;
 for( ; *p!=0; ++p ) {
	int c = *((unsigned char *)p);
	if( c<0x20 ) break;
	if( c==' ' || c==',' || c=='(' || c==')'
	 || c==',' || c=='\'' || c=='\"' || c=='\\' ) s += '\\';
	s += c;
 }
 if( *p!=0 ) {
	// terminated prematurely
	s.resize(0);
	return false;
 }
 s += ')';
 return true;
}

////////////////////////////////////////////////////////////////
// border styles

static const char * CSS_BORDER_STYLES[] =
{ "none", "dotted", "dashed", "solid", "double", "groove", "ridge", "inset", "outset" };


const char * css_border_style_name( int id )
{
 if( id<0 || id>=sizeof(CSS_BORDER_STYLES)/sizeof(CSS_BORDER_STYLES[0]) ) return NULL;
 return CSS_BORDER_STYLES[id];
}

int	css_parse_border_style( const char *str, int len, int *p_style )
{
 const char *p = str;
 const char *pe = p+len;
 css_skip_white( &p );	// TODO: use 'pe'

 string buf;
 if( !css_scan_value(&p,buf) ) return 0;

 for( int i=0; i<sizeof(CSS_BORDER_STYLES)/sizeof(CSS_BORDER_STYLES[0]); ++i ) {
	const char *ss = CSS_BORDER_STYLES[i];
	if( strcmp( ss, buf.c_str() )==0 ) { *p_style = i; return p-str; }
 }
 return 0;
}

int	css_find_any_border_style( const char *s )
{
 while( *s!=0 ) {
	css_skip_white( &s );
	const char *pe = s;
	while( *pe!=0 && !isspace(*pe) ) ++pe;
	if( pe==s ) continue;

	for( int i=0; i<sizeof(CSS_BORDER_STYLES)/sizeof(CSS_BORDER_STYLES[0]); ++i ) {
		const char *ss = CSS_BORDER_STYLES[i];
		if( strncmp( s, ss, pe-s )==0 ) return i;
	}
	s = pe;
 }
 return -1;
}

