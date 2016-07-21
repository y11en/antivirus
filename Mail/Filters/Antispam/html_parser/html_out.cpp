// $Id: html_out.cpp,v 1.3 2000/06/02 10:58:47 slavikb Exp $
//

#include "htmparse.h"

void	html_print_endl( html_obj_printer *prn )
{
 ostream& os = prn->get_stream();
 os << "\r\n";
}

bool	html_is_name( const char *s )
{
 if( *s==0 ) return false;
 const char *p = s;
 for( ; *p!=0; ++p ) {
	int c = (unsigned char)(*p);
	if( (html_char_class(c) & HTML_CC_NAME)==0 ) return false;
 }
 return true;
}

void	html_print_qname( ostream& os, const char *s )
{
 if( html_is_name( s ) ) {
 	os << s;
 } else {
	os << '\"' << s << '\"';
 }
}

////////////////////////////////////////////////////////////////
// element printout

void	html_attr::print( html_obj_printer *prn )
{
 ostream& os = prn->get_stream();
 os << name;
 if( value!=NULL ) {
	os << '=';
	html_print_qname( os, value );
 }
}

static void str_find_and_replace(string &s, const char *pattern, const char *replacer)
{
	size_t pos = 0;
	size_t pat_size = strlen(pattern);
	while((pos = s.find(pattern, pos)) != string::npos)
		s.replace(pos, pat_size, replacer);
}

void	html_elt_text::print( html_obj_printer *prn )
{
	ostream &os = prn->get_stream();

	string s = str;
	str_find_and_replace(s, "&nbsp;", " "); // no-break space
	str_find_and_replace(s, "&amp;", "&"); // ampersand
	str_find_and_replace(s, "&lt;", "<"); // less-than sign
	str_find_and_replace(s, "&gt;", ">"); // greater-than sign
	str_find_and_replace(s, "&quot;", "\""); // double quote
	str_find_and_replace(s, "&copy;", "\xa9"); // copyright sign
	str_find_and_replace(s, "&laquo;", "\xab"); // left-pointing double angle quotation mark
	str_find_and_replace(s, "&raquo;", "\xbb"); // right-pointing double angle quotation mark
	os << s;
	if(flags & HTML_F_LF)
		os << "\r\n";
}

void	html_elt_sgml::print( html_obj_printer *prn )
{
 ostream& os = prn->get_stream();
 os << '<' << str << '>';
 if( flags & HTML_F_LF ) os << "\r\n";
}

void	html_elt_comment::print( html_obj_printer *prn )
{
 ostream& os = prn->get_stream();
 os << "<!--" << str << "-->";
 if( flags & HTML_F_LF ) os << "\r\n";
}

void	html_elt_tag::print_stag( html_obj_printer *prn, const char *tag, html_element *attrs, html_element *nattrs )
{
 ostream& os = prn->get_stream();
 unsigned long pflags = prn->get_flags();
 os << '<' << tag;

 print_attrs( prn, attrs, nattrs );

 os << '>';
 if( flags & HTML_F_LF ) os << "\r\n";
}

void	html_elt_tag::print_attrs( html_obj_printer *prn, html_element *attrs, html_element *nattrs )
{
 ostream& os = prn->get_stream();
 unsigned long pflags = prn->get_flags();

 html_attr *new_attrs = dynamic_cast<html_attr *>(nattrs);

 char buf[20];
 if( pflags & HTML_PF_IIDS ) {
	sprintf( buf, "I%lX", (unsigned long)this );

	html_attr *new_id_attr = new(alloca(sizeof(html_attr))) html_attr();
	if ( new_id_attr )
	{
		new_id_attr->next = new_attrs;
		new_id_attr->name = "ID";
		new_id_attr->value = buf;
		new_attrs = new_id_attr;
	}
 }

 html_print_attrs( prn, dynamic_cast<html_attr *>(attrs), new_attrs );
}

void	html_elt_tag::print_etag( html_obj_printer *prn )
{
 ostream& os = prn->get_stream();
 unsigned long pflags = prn->get_flags();

 // flag: print ending tag
 bool f_print_etag = false;

 // flag: output CR/LF after ending tag
 bool f_lf_etag = false;
 if( flags & HTML_F_LFE ) f_lf_etag = true;

 if( (pflags & HTML_PF_VALID)==0 ) {
	// print original ETAGs
	if( flags & HTML_F_ETAG ) f_print_etag = true;
 } else {
	// print only (and all) valid ETAGs
	if( (def->cflags & HTML_EF_EMPTY)==0 ) {
		f_print_etag = true;
		// output CR/LF after artifical ETAGs
		if( (flags & HTML_F_ETAG)==0 ) f_lf_etag = true;
	}
 }
 if( f_print_etag ) {
	os << "</" << def->tag << '>';
	if( f_lf_etag ) os << "\r\n";
 }
}

void	html_elt_tag::print( html_obj_printer *prn )
{
 print_stag( prn, def->tag, attrs.head );
 html_print_elements( prn, childs.head, NULL );
 print_etag( prn );
}

void	html_elt_etag::print( html_obj_printer *prn )
{
 // non-paired ETAGs
 if( (prn->get_flags() & HTML_PF_VALID)!=0 ) return;

 ostream& os = prn->get_stream();
 os << "</" << def->tag << '>';
 if( flags & HTML_F_LF ) os << "\r\n";
}

void	html_print_attrs( html_obj_printer *prn, html_attr *pa, html_attr *pn )
{
 ostream& os = prn->get_stream();

 int acnt = 0;
 html_attr *p;
 for( p=pa; p!=NULL; p=(html_attr *)p->next ) ++acnt;
 for( p=pn; p!=NULL; p=(html_attr *)p->next ) ++acnt;

 html_attr **atab = (html_attr **)alloca( acnt*sizeof(html_attr *) );
 acnt = 0;

 // append present attributes
 for( p=pa; p!=NULL; p=(html_attr *)p->next ) {
	atab[acnt++] = p;
 }

 // override values with 'new' attributes
 for( p=pn; p!=NULL; p=(html_attr *)p->next ) {
	html_attr *xp = p;
	if( p->value==NULL ) xp = NULL;
	for( int i=0; i<acnt; ++i ) {
		if( atab[i]!=NULL && strcmp( atab[i]->name, p->name )==0 ) {
			atab[i] = xp;	// substitute only first occurance
			xp = NULL;
		}
	}
	if( xp!=NULL ) atab[acnt++] = xp;
 }

 for( int i=0; i<acnt; ++i ) {
	if( atab[i]!=NULL ) {
		os << ' ';
		atab[i]->print( prn );
	}
 }
}

void	html_print_elements( html_obj_printer *prn, html_element *first, html_element *term )
{
 html_element *p = first;
 for( ; p!=NULL && p!=term; p=p->next ) {
	p->print( prn );
 }
}



