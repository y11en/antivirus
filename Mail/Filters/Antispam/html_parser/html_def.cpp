// $Id: html_def.cpp,v 1.3 2000/04/20 13:52:59 slavikb Exp $
//

#include "htmparse.h"

html_elt_def __html_elt_def[] = {
#define	DEF_HTML_ELEMENT(N,F,C)		{ #N, F, C },
#define	DEF_HTML_ELEMENT_EX(N,L,F,C)    { L, F, C },
#include "htmltags.tab"
#undef DEF_HTML_ELEMENT
};

const	int HTML_ELT_COUNT = sizeof(__html_elt_def)/sizeof(__html_elt_def[0]);

int	html_find_tag( const char *name )
{
 int lb = 0;
 int hb = HTML_ELT_COUNT;
 while( hb>lb ) {
	int i = (hb+lb)/2;
	int st = strcmp( name, __html_elt_def[i].tag );
	if( st==0 ) return i;
	if( st<0 ) hb = i; else lb = i+1;
 }
 return -1;
}

html_elt_def *html_find_element( const char *name )
{
 int idx = html_find_tag( name );
 if( idx<0 ) return NULL;
 return &__html_elt_def[idx];
}

html_elt_def *html_get_element( int idx )
{
 if( idx<0 || idx>=sizeof(__html_elt_def)/sizeof(__html_elt_def[0]) ) return NULL;
 return &__html_elt_def[idx];
}

const char CA_UPR = 'A'-'a';

static bool	normalize_copy_tag_a( const char *s, char *d )
{
 for( ; *s!=0; ++s,++d ) {
	char c = *s;
	if( c & ~0x7F ) return false;
	if( c>='a' && c<='z' ) *d = c+CA_UPR; else *d = c;
 }
 *d = 0;
 return true;
}

static bool	normalize_copy_tag_w( const wchar_t *s, char *d )
{
 for( ; *s!=0; ++s,++d ) {
	wchar_t wc = *s;
	if( wc & ~0x7F ) return false;
	char c = (char)wc;
	if( c>='a' && c<='z' ) *d = c+CA_UPR; else *d = c;
 }
 *d = 0;
 return true;
}

int	html_find_tag_i( const char *name )
{
 int blen = (strlen(name)+1)*sizeof(char);
 char *nbuf = (char *)alloca( blen );
 if( !normalize_copy_tag_a( name, nbuf ) ) return -1;
 return html_find_tag( nbuf );
}

html_elt_def *html_find_element_i( const char *name )
{
 int idx = html_find_tag_i( name );
 if( idx<0 ) return NULL;
 return &__html_elt_def[idx];
}

int	html_find_tag_w( const wchar_t *wname )
{
 const wchar_t *pw = wname;
 while( *pw!=0 ) ++pw;
 int blen = (pw-wname+1);
 char *nbuf = (char *)alloca( blen );
 if( !normalize_copy_tag_w( wname, nbuf ) ) return -1;
 return html_find_tag( nbuf );
}

html_elt_def *html_find_element_w( const wchar_t *wname )
{
 int idx = html_find_tag_w( wname );
 if( idx<0 ) return NULL;
 return &__html_elt_def[idx];
}

html_elt_def_dynamic::html_elt_def_dynamic()
{
 tag = NULL;
 cflags = HTML_EF_DYNAMIC;
 cmodel = 0;
}

void	html_elt_def::duplicate()
{
 if( (cflags & HTML_EF_DYNAMIC)!=0 ) {
	// WARNING: unsafe operation (reinterpret_cast)
	html_elt_def_dynamic *ddef = (html_elt_def_dynamic *)this;
	ddef->duplicate();
 }
}

void	html_elt_def::release()
{
 if( (cflags & HTML_EF_DYNAMIC)!=0 ) {
	// WARNING: unsafe operation (reinterpret_cast)
	html_elt_def_dynamic *ddef = (html_elt_def_dynamic *)this;
	ddef->release();
 }
}




