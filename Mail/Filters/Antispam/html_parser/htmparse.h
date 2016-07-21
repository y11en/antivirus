// $Id: htmparse.h,v 1.9 2000/06/02 10:58:47 slavikb Exp $
//

#ifndef _HTMPARSE_H_
#define _HTMPARSE_H_

#pragma warning(disable:4786)

#include <Prague/prague.h>
#include <Prague/iface/i_io.h>

#include <assert.h>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <list>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <vector>

#include "util/rcobj.h"

using namespace std;

////////////////////////////////////////////////////////////////
// object model

// forward declarations
struct	html_object;
struct	html_elt_def;
struct	html_elt_def_dynamic;
struct	html_attr;
struct	html_element;
struct	html_elt_list;
struct	html_elt_text;
struct	html_elt_tag;
struct	html_elt_etag;

////////////////////////////////////////////////////////////////
// HTML object printer

class	html_obj_printer {
public:
	html_obj_printer( ostream& os=cout, unsigned long f=0 )
			: pos(&os), flags(f)	{}

	void	put_stream( ostream& os )	{
		pos = &os; 
	}
	void	put_flags( unsigned long f )	{ flags = f; }

 virtual ostream&	get_stream() const	{ 
	 return *pos; 
 }
 virtual unsigned long	get_flags() const	{ return flags; }

protected:
	ostream		*pos;
	unsigned long	flags;
};

////////////////////////////////////////////////////////////////
// utilites

char *	html_alloc_string( const char *s, int l=-1 );

template<class T>
class	rc_object_vector : public vector<OBJREF_PTR(T)> {};

////////////////////////////////////////////////////////////////
// HTML objects

// base class for dynamically allocated objects
struct	html_object : public std_dynamic_object {
	unsigned long	flags;

		html_object() : flags(0) {}
	virtual	~html_object()		{ clear(); }

	virtual	void	clear()		{ flags=0; }
	virtual void	print( html_obj_printer *prn )	{}
};

typedef	OBJREF_PTR(html_object)	html_objref;

// tag definitions: have both static and dynamic form
struct	html_elt_def {
	char *		tag;
	unsigned int	cflags;		// content flags
	unsigned int	cmodel;		// content model

	void	duplicate();
	void	release();
};

// dynamic tag definition
// NOTE: ORDER OF INHERITANCE IS IMPORTANT !!!
struct	html_elt_def_dynamic : public html_elt_def, public html_object {

		html_elt_def_dynamic();
		~html_elt_def_dynamic()	{ clear(); }

	void	duplicate()	{ html_object::duplicate(); }
	void	release()	{ html_object::release(); }

	void	clear()		{ get_allocator()->free(tag); tag=NULL; }
};

// static tag definition lookup
int	html_find_tag( const char *name );
int	html_find_tag_i( const char *name );
int	html_find_tag_w( const wchar_t *wname );

html_elt_def	* html_get_element( int i );
html_elt_def	* html_find_element( const char *name );
html_elt_def	* html_find_element_i( const char *name );	// case-insensitive
html_elt_def	* html_find_element_w( const wchar_t *wname );	// case-insensitive

// other objects
struct	html_element : public html_object {
	html_element	*next;
	html_objref	xdata;		// extended data

		html_element() : next(NULL)	{}
		~html_element()	{ clear(); }

	void	clear()		{ xdata.release(); }
};

// element list
struct	html_elt_list {
	html_element	*head;

		html_elt_list() : head(NULL)	{}
		~html_elt_list();

	void	destroy();
};

// attributes
struct	html_attr : public html_element {
	char		*name;		// may be definition?
	char		*value;

		html_attr() : name(NULL), value(NULL)	{}
		~html_attr()	{ clear(); }

		void	clear()	{ get_allocator()->free(name); name=NULL;
				  get_allocator()->free(value); value=NULL;
				  html_element::clear(); }

		void	print( html_obj_printer *prn );
};

struct	html_elt_text : public html_element {
	char		*str;

		html_elt_text() : str(NULL)	{}
		~html_elt_text()	{ clear(); }

	void	clear()	{ get_allocator()->free(str); str=NULL;
				  html_element::clear(); }

	void	print( html_obj_printer *prn );
};

struct	html_elt_sgml : public html_elt_text {
	void	print( html_obj_printer *prn );
};

struct	html_elt_comment : public html_elt_text {
	void	print( html_obj_printer *prn );
};

struct	html_elt_tag : public html_element {
	html_elt_def *	def;
	html_elt_list	attrs;
	html_elt_list	childs;

		html_elt_tag() : def(NULL)	{}
		~html_elt_tag()			{ clear(); }

	void	clear()	{ if(def!=NULL) { def->release(); def=NULL; }
				  attrs.destroy();
				  childs.destroy();
				  html_element::clear(); }

	void	print( html_obj_printer *prn );

 virtual void	print_stag( html_obj_printer *prn, const char *tag, html_element *attrs, html_element *nattrs=NULL );
 virtual void	print_attrs( html_obj_printer *prn, html_element *attrs, html_element *nattrs=NULL ); 
 virtual void	print_etag( html_obj_printer *prn );
};

struct	html_elt_etag : public html_element {
	html_elt_def *	def;

		html_elt_etag() : def(NULL)	{}
		~html_elt_etag()		{ clear(); }

	void	clear()	{ if(def!=NULL) { def->release(); def=NULL; }
				  html_element::clear(); }

	void	print( html_obj_printer *prn );
};

////////////////////////////////////////////////////////////////
// element list iterator

class html_elt_list_iterator {
public:
	html_elt_list_iterator( html_elt_list *pl )
			: list(pl)	{ reset(); }

	// resets to the head
	void	reset()		{ ppcur = &(list->head); }

	// seeks from the current position to the specified element
	html_element * seek( html_element *to ) {
			while( *ppcur!=NULL && *ppcur!=to ) {
				ppcur = &((*ppcur)->next);
			}
			return *ppcur;
		}

	html_element * current() const	{ return *ppcur; }

		// erases current element
	void	erase() {
			html_element *elt = *ppcur;
			if(elt!=NULL) {
				*ppcur = elt->next;
				elt->release();
			}
		}

		// inserts element before current
		// calls duplicate() !!!
	void	insert( html_element *elt ) {
			elt->duplicate();
			elt->next = *ppcur;
			*ppcur = elt;
			ppcur = &(elt->next);
		}

	html_element *	next()	{
			if( *ppcur!=NULL ) ppcur = &((*ppcur)->next);
			return *ppcur;
		}

protected:
	html_elt_list		*list;
	html_element		**ppcur;
};

////////////////////////////////////////////////////////////////
// lookahead character stream

class	lookahead_stream {
public:
	lookahead_stream( int sz=10 ) : is(NULL),buf(NULL),pos(0),size(0),pos_in_io(0L),buf_size(0)
	{ 
		buf = new unsigned char[sz+1]; 
		if ( buf )
		{
			memset(buf, 0, sz+1); size=sz; 
		}
	}
	~lookahead_stream()
		{ clear(); if(buf!=NULL) { delete buf; buf=NULL; } }
		
	void	open( hIO s )	{ clear(); is=s; }
	void	close()			{ clear(); }
	void	clear()			{ pos=0; is=NULL; pos_in_io=0L; buf_size=0;  }

	int	get()	
	{ 
		if ( !buf )
			return -1;

		if( !buf[0] || pos >= buf_size )
		{
			tDWORD dwRead = 0;
			cERROR err = errOK;
			if ( PR_SUCC(err= is->SeekRead(&dwRead, pos_in_io, buf, size) ) && dwRead )
			{
				pos = 0;
				buf_size = dwRead;
				pos_in_io += buf_size;
			}
			else
				return -1;
		}
		return buf[pos++]; 
	}

	void	unget( int ch )
	{ 
		if ( ch==-1 )
			return;
		if ( !buf )
			return;

		if ( !pos )
		{
			// Тут нам придется откатиться на один буфер назад.
			// Откатимся-ка мы на пол-буфера, чтобы в будущем не выходить за пределы нового буфера
			tDWORD dwRead = 0;
			cERROR err = errOK;
			int _size = buf_size/2;
			tQWORD pos_in_io_new = pos_in_io - buf_size - _size;
			if ( pos_in_io_new < 0 )
				pos_in_io_new = 0L;
			if ( PR_SUCC(err= is->SeekRead(&dwRead, pos_in_io_new, buf, size) ) && dwRead )
			{
				pos = _size;
				buf_size = dwRead;
				pos_in_io = pos_in_io_new + buf_size;
			}
		}

		if ( pos )
			pos--;
	}

protected:
	hIO is;
	unsigned char *buf;
	int	pos;
	int	buf_size;
	int	size;
	tQWORD pos_in_io;

private:
	lookahead_stream( const lookahead_stream& );
	lookahead_stream& operator=( const lookahead_stream& );
};

////////////////////////////////////////////////////////////////
// HTML elements classification

enum {
	// tag presence (required or optional)
	HTML_EF_EMPTY	= 0x00001,	// no closing tag
	HTML_EF_OETAG	= 0x00002,	// optional end tag
	HTML_EF_OSTAG	= 0x00004,	// optional start tag
	HTML_EF_OPAQUE	= 0x00008,	// opaque content
	// common content types
	HTML_EF_INLINE	= 0x00010,	// %inline
	HTML_EF_BLOCK	= 0x00020,	// %block
	HTML_EF_HEAD	= 0x00040,	// %head.content & %head.misc
	HTML_EF_MISC	= 0x00080,	// internal elements
	// special content types
	HTML_EF_COL	= 0x00100,	// COL
	HTML_EF_TR	= 0x00200,	// TR
	HTML_EF_THTD	= 0x00400,	// TH|TD

	HTML_EF_NOCORE	= 0x01000,	// element has no core attributes (ID,CLASS,...)
	HTML_EF_NOI18N	= 0x02000,	// element has no i18n attributes (LANG,DIR)
	HTML_EF_NOEVENTS = 0x04000,	// element has no ovent attributes (ONMOUSEOVER)

	HTML_EF_DYNAMIC	= 0x10000,	// defninition was dynamically created

	HTML_EF_SCRIPT = 0x20000, // this is script

	// content flags
	HTML_EF_FLOW	= HTML_EF_INLINE|HTML_EF_BLOCK,
	HTML_EF_SPECIAL = HTML_EF_COL|HTML_EF_TR|HTML_EF_THTD,
	HTML_EF_BODY	= HTML_EF_FLOW|HTML_EF_MISC|HTML_EF_SPECIAL,
};

// special MISC content elements
enum {
	HTML_MISC_NONE,
	HTML_MISC_HTML,
	HTML_MIST_HEAD,
	HTML_MISC_BODY,
	HTML_MISC_INS,
	HTML_MISC_DEL,
};

// content models
enum {
	HTML_ECM_OUTER,			// outer content
	HTML_ECM_HTML,			// <HTML> element
	HTML_ECM_INLINE,		// inline content
	HTML_ECM_FLOW,			// flow content
};

////////////////////////////////////////////////////////////////
// HTML elements

enum {
#define	DEF_HTML_ELEMENT(N,F,C)	HTML_ELT_##N,
#define	DEF_HTML_ELEMENT_EX(N,L,F,C) HTML_ELT_##N,
#include "htmltags.tab"
#undef DEF_HTML_ELEMENT
	// sentinel
	HTML_ELT_LAST
};

extern html_elt_def __html_elt_def[];

#define	HTML_ELT_DEF(N)	(&__html_elt_def[HTML_ELT_##N])

////////////////////////////////////////////////////////////////
// HTML attributes

enum {
#define	DEF_HTML_ATTR(N,S,F)	HTML_ATTR_##N,
#include "htmlattr.tab"
#undef DEF_HTML_ATTR
	// sentinel
	HTML_ATTR_LAST
};

// attribute classes
enum {
	HTML_ATTR_CLASS_CORE = 1,
	HTML_ATTR_CLASS_EVENTS,
	HTML_ATTR_CLASS_FONT,
	HTML_ATTR_CLASS_FORM,
	HTML_ATTR_CLASS_TABLE
};

const char * html_attr_name( int attr );
int	html_attr_class( int attr );

const int HTML_MAX_ELT_ATTRS = 64;
int	html_enum_elt_attrs( int elt, unsigned char *attrs );

bool	html_find_elt_attr( int elt, int attr );

////////////////////////////////////////////////////////////////
// HTML character classes

enum {
	HTML_CC_NMSTART = 0x01,
	HTML_CC_NAME	= 0x02,
	HTML_CC_BLANK	= 0x04,
};

int	html_char_class( int ch );

// checks if symbol can be not quoted
bool	html_is_name( const char *s );

////////////////////////////////////////////////////////////////
// data structures (allocated from obj_pool,
// so aligned to OBJECT_POOL_ALIGN)

enum {	// token flags
	HTML_F_LF = 0x0001,	// line feed after this token
	HTML_F_LFE = 0x0002,	// line feed after closing tag
	HTML_F_NOTAG = 0x0004,	// tag was created artifically
	HTML_F_ETAG = 0x0008,	// ending tag presents
};

////////////////////////////////////////////////////////////////
// HTML parser

enum {	// token types
	HTML_TOK_STAG = 0,	// start tag
	HTML_TOK_ETAG,		// end tag
	HTML_TOK_SGML,		// SGML declaration (<!XXX>,<?XXX>)
	HTML_TOK_CDATA,		// character data
	HTML_TOK_COMMENT,	// comment
	HTML_TOK_BLANK,		// whitespace
	HTML_TOK_EOF = -1
};

class	html_parser {
public:
	html_parser();
	~html_parser();
	
	void		cleanup();
	void		parse( hIO is, html_elt_list& list );

	inline vector<html_elt_def_dynamic *>* get_unknows_defs(){
		return &unknown_defs;
	};

protected:
	lookahead_stream	is;

	html_element		*first_elt;
	html_element		*last_elt;
	html_element		*next_elt;

	vector<html_elt_def_dynamic *>	unknown_defs;

	int	scan_token( string& buf );
	bool	scan_comment( string& buf );
	bool	scan_quoted( string& buf, int ec, bool f_inline );
	bool	scan_sgml( string& buf );
	bool	scan_name( string& buf, bool f_norm );
	bool	scan_to_etag( const char *etag, string& buf );
	bool	scan_attr_value( string& buf );
	bool	skip_white();

	bool	skip_tag_tail();
	bool	scan_tag_attrs( string& buf, html_elt_list& list );

	html_elt_def	*get_elt_defn( const char *name );

	void	add_tag( string& buf );
	void	add_etag( string& buf );
	void	add_sgml( string& buf );
	void	add_cdata( string& buf );
	void	add_comment( string& buf );
	void	append_element( html_element *elt );

	void	parse_sequence();

	html_element	*get_token()	{ return next_elt; }
	html_element	*next_token()	{
				html_element *elt = next_elt;
				if( elt!=NULL ) next_elt = elt->next;
				return elt;
			}

	struct elt_parse_state {
		elt_parse_state		*link;		// parent state
		html_elt_tag		*elt;		// element
	};

	bool	find_etag( html_elt_etag *p, elt_parse_state *state );
	void	parse_elt_seq( html_elt_list& list, elt_parse_state *state );
	void	parse_elt( html_elt_tag *elt, elt_parse_state *state );

private:
	html_parser( const html_parser& );
	html_parser& operator=( const html_parser& );
};

////////////////////////////////////////////////////////////////
// Postprocessor & validator

// checks for <HTML> element, inserts <HEAD> and <BODY> elements;
// returns false if document structure is incorrect (no <HTML> tag)

bool	html_postprocess( html_elt_list& elts );

// raises level of all childs, placing them after parent node
void	html_tag_raise_childs( html_elt_tag *tag );

// moves elements from (tag->next) until (term) under (tag)
void	html_tag_drown_childs( html_elt_tag *tag, html_element *term );

////////////////////////////////////////////////////////////////
// HTML output

enum {
	HTML_PF_IIDS = 0x0001,	// print internal IDs
	HTML_PF_VALID = 0x0002,	// print valid ETAGs
	HTML_PF_NONBSP = 0x0004, // remove multiple &nbsp;
};

void	html_print_endl( html_obj_printer *prn );

// prints attributes, from po + pn, pn overrides po
void	html_print_attrs( html_obj_printer *prn, html_attr *pa, html_attr *pn = NULL );

void	html_print_elements( html_obj_printer *prn, html_element *first, html_element *term=NULL );

///////////////////////////////////////////////////////////////
// HTML document

class	html_document : public std_dynamic_object {
public:
	html_document();
	~html_document();

	void	init()		{ clear(); }

	void	clear();
	bool	read( hIO is );
	void	write( ostream& os, int f=HTML_PF_VALID );

	html_elt_list&	get_contents()
			{ return contents; }

	const html_elt_list&	get_contents()	const
			{ return contents; }

	html_elt_tag *	get_head() const	{ return head; }
	html_elt_tag *	get_body() const	{ return body; }

protected:
	html_elt_list		contents;
	html_elt_tag		*head;
	html_elt_tag		*body;
};

typedef	OBJREF_PTR(html_document) html_document_ptr;

////////////////////////////////////////////////////////////////
// utilites

// moves iterator to the specified attribute
html_attr * html_find_attr( html_elt_list_iterator& iter, const char *id );

bool	html_get_attr_i( html_elt_list& attrs, const char *id, int& val );
bool	html_get_attr_ul( html_elt_list& attrs, const char *id, unsigned long& val );
const char *html_get_attr_s( html_elt_list& attrs, const char *id );

void	html_set_attr_i( html_elt_list& attrs, const char *id, int val );
void	html_set_attr_ul( html_elt_list& attrs, const char *id, unsigned long val );
void	html_set_attr_s( html_elt_list& attrs, const char *id, const char *val );

void	html_remove_attr( html_elt_list& attrs, const char *id );

////////////////////////////////////////////////////////////////
// CSS stuff

bool	html_style_parse( const char *str, html_elt_list& style );
bool	html_style_format( html_attr *style, html_attr *nstyle, string& buf );

int	css_parse_url( const char *str, int len, string& url );
bool	css_format_url( const char *url, string& s );

// CSS border styles
enum {
	CSS_BORDER_NONE,
	CSS_BORDER_DOTTED,
	CSS_BORDER_DASHED,
	CSS_BORDER_SOLID,
	CSS_BORDER_DOUBLE,
	CSS_BORDER_GROOVE,
	CSS_BORDER_RIDGE,
	CSS_BORDER_INSET,
	CSS_BORDER_OUTSET
};

const char * css_border_style_name( int id );

// returns number of chars skipped (0 if none matches)
int	css_parse_border_style( const char *str, int len, int *p_style );

int	css_find_any_border_style( const char *s );

////////////////////////////////////////////////////////////////
// comment escaping

void	html_escape_comment( const char *s, string& d );
void	html_unescape_comment( const char *s, string& d );

//////////////////////////////////////////////////////////////////////////
// color manipulating

#ifndef GetRValue
#define GetRValue(rgb)      ((unsigned char)(rgb))
#endif
#ifndef GetGValue
#define GetGValue(rgb)      ((unsigned char)(((unsigned short)(rgb)) >> 8))
#endif
#ifndef GetBValue
#define GetBValue(rgb)      ((unsigned char)((rgb)>>16))
#endif

bool	html_color_parse( const char *s, unsigned long *p_rgb );
void	html_color_format( unsigned long rgb, char *buf );


#endif	// _HTMPARSE_H_



