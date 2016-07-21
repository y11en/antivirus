// $Id: htparser.cpp,v 1.5 2000/05/06 13:12:47 slavikb Exp $
//

#include "htmparse.h"
#include <assert.h>

//DVi#include "htmproc.h"

////////////////////////////////////////////////////////////////
// HTML tokenizer

namespace INLINE
{
	inline int isdigit(int ch)
	{
		switch( ch ) 
		{
		case 48: //	0:
		case 49: //	1:
		case 50: //	2:
		case 51: //	3:
		case 52: //	4:
		case 53: //	5:
		case 54: //	6:
		case 55: //	7:
		case 56: //	8:
		case 57: //	9:
			return 1;
			break;
		default:
			return 0;
			break;
		}
	};
	
	inline int isalpha(int ch)
	{
		if ( 
			(ch >= 'A' && ch <= 'Z')
			||
			(ch >= 'a' && ch <= 'z')
			) 
			return 1;
		else
			return 0;
	};

	inline int isspace (int ch)
	{
		if (
			(ch >= 0x09 && ch <= 0x0d)
			||
			(ch == 0x20)
			)
			return 1;
		else
			return 0;
	};

	inline int toupper(int ch)
	{
		if ( INLINE::isalpha(ch) && ch >= 97 ) 
			return ch - 32;
		else
			return ch;
	};

}

html_parser::html_parser() : is(1024),	// 1024-char lookahead
	first_elt(NULL), last_elt(NULL)
{
}

html_parser::~html_parser()
{
 cleanup();
}

void	html_parser::parse( hIO _is, html_elt_list& elts )
{
 cleanup();

 // pass 1: tokenize stream
 is.open( _is );
 parse_sequence();
 //unknown_defs.clear();

 // pass 2: build element tree
 next_elt = first_elt;
 parse_elt_seq( elts, NULL );
}

void	html_parser::cleanup()
{
 is.close();
 first_elt = NULL;
 last_elt = NULL;
 unknown_defs.clear();
}

// scans next token and returns its identifier
int	html_parser::scan_token( string& buf )
{
 buf.resize(0);

 int tok = HTML_TOK_BLANK;
 while(true) {
	int ch = is.get();
	if( ch==-1 ) {
		// EOF encountered
		if( buf.length()!=0 ) break;
		return HTML_TOK_EOF;
	}
	if( ch=='<' ) {
		int ch1 = is.get();
		// check for comments and SGML markup
		if( ch1=='!' ) {
			int ch2 = is.get();
			if( ch2=='-' ) {
				int ch3 = is.get();
				if( ch3=='-' ) {
					// comment
					if( buf.length()!=0 ) {
						is.unget( ch3 );
						is.unget( ch2 );
						is.unget( ch1 );
						is.unget( ch );
						break;
					}
					scan_comment( buf );
					return HTML_TOK_COMMENT;
				}
				is.unget( ch3 );
			}

			is.unget( ch2 );
			is.unget( ch1 );

			if( buf.length()!=0 ) {
				is.unget( ch );
				break;
			}
			scan_sgml( buf );
			return HTML_TOK_SGML;
		}

		// SGML processing instruction
		if( ch1=='?' ) {
			is.unget( ch1 );
			if( buf.length()!=0 ) {
				is.unget( ch );
				break;
			}

			scan_sgml( buf );
			return HTML_TOK_SGML;
		}


		if( ch1=='/' ) {
			int ch2 = is.get();
			if( html_char_class(ch2) & HTML_CC_NMSTART ) {
				is.unget(ch2);
				if( buf.length()!=0 ) {
					is.unget(ch1);
					is.unget(ch);
					break;
				}
				scan_name(buf,true);
				return HTML_TOK_ETAG;
			}
		}

		if( html_char_class(ch1) & HTML_CC_NMSTART ) {
			is.unget(ch1);
			if( buf.length()!=0 ) {
				is.unget(ch);
				break;
			}
			scan_name(buf,true);
			return HTML_TOK_STAG;
		}

	}

	buf += ch;

	int cc = html_char_class(ch);
	if( (cc & HTML_CC_BLANK)==0 ) tok = HTML_TOK_CDATA;
 }

 return tok;
}

// scans COMMENT text (starting from <!-- i, excluding -->)
bool	html_parser::scan_comment( string& buf )
{
 int st = 0;
 while(true) {
	int ch = is.get();
	if( ch==-1 ) break;
	switch(st) {
	case 0: if( ch=='-' ) { st=1; break; }
		buf += ch;
		break;
	case 1: if( ch=='-' ) { st=2; break; }
		buf += '-';
		buf += ch;
		st = 0;
		break;
	case 2: if( ch=='>' ) return true;
		// '--' is recognized as the end of the comment
		// all characters after '--' (except '>') are dropped
		break;
	}
 }
 return false;
}

bool	html_parser::scan_quoted( string& buf, int ec, bool f_inline )
{
 while(true) {
	int ch = is.get();
	if( ch==-1 ) return false;
	if( ch==ec ) {
		if( f_inline ) buf += ch;
		break;
	}
	buf += ch;
 }
 return true;
}

// scans SGML declaration
bool	html_parser::scan_sgml( string& buf )
{
 while(true) {
	int ch = is.get();
	if( ch==-1 ) return false;
	if( ch=='>' ) break;
	buf += ch;
	if( ch=='\"' || ch=='\'' ) scan_quoted( buf, ch, true );
 }
 return true;
}

bool	html_parser::scan_name( string& buf, bool f_norm )
{
 while(true) {
	int ch = is.get();
	if( ch==-1 ) return false;
	int cc = html_char_class(ch);
	if( (cc & HTML_CC_NAME)==0 ) {
		is.unget(ch);
		break;
	}
	if( f_norm ) ch = INLINE::toupper(ch);
	buf += ch;
 }
 return true;
}

bool	html_parser::scan_attr_value( string& buf )
{
 int ch = is.get();
 if( ch=='\"' || ch=='\'' ) {
	scan_quoted( buf, ch, false );
 } else {
	is.unget(ch);
	while(true) {
		ch = is.get();
		if( ch==-1 ) return false;
		if( ch=='>' ) { is.unget(ch); break; }
		if( INLINE::isspace(ch) ) break;
		buf += ch;
	}
 }
 return true;
}

bool	html_parser::scan_to_etag( const char *etag, string& buf )
{
 buf.resize(0);
 while(true) {
	int ch = is.get();
	if( ch==-1 ) return false;
	buf += ch;
	if( ch=='<' ) {
		int mark = buf.length()-1;
		ch = is.get();
		if( ch==-1 ) return false;
		buf += ch;

		if( ch=='/' ) {
			const char *ep = etag;
			while(*ep!=0) {
				ch = is.get();
				if( ch==-1 ) return false;
				if( INLINE::toupper(ch) != INLINE::toupper(*ep) ) {
					is.unget( ch );
					break;
				}
				buf += ch;
				++ep;
			}
			if( *ep==0 ) {	// everything is OK
				ch = is.get();
				if( (html_char_class(ch) & HTML_CC_NAME)==0 ) {
					is.unget(ch);
					buf.resize(mark);
					return true;
				}
			}
		}
	}
 }
}

bool	html_parser::skip_white()
{
 while(true) {
	int ch = is.get();
	if( ch==-1 ) return false;
	if( (html_char_class(ch) & HTML_CC_BLANK)==0 ) {
		is.unget(ch);
		break;
	}
 }
 return true;
}

bool	html_parser::scan_tag_attrs( string& buf, html_elt_list& attrs )
{
 html_elt_list_iterator iter(&attrs);
 iter.seek(NULL);

 int ch;
 while(true) {
	while(true) {	// skip whitespace or invalid content
		ch = is.get();
		if( ch==-1 ) return false;
		if( ch=='>' ) return true;
		int cc = html_char_class(ch);
		if( cc & HTML_CC_NMSTART ) { is.unget(ch); break; }
	}

	buf.resize(0);
	scan_name( buf, true );

	// create named attribute
	html_attr *attr = RC_OBJECT_NEW(html_attr);
	if ( attr )
	{
		attr->name = html_alloc_string( buf.c_str(), buf.length() );
		iter.insert(attr);
		attr->release();
	}

	skip_white();
	ch = is.get();
	if( ch=='=' ) {
		// scan attribute value
		buf.resize(0);
		skip_white();
		scan_attr_value( buf );
		// create name=value attribute
		if( attr!=NULL ) attr->value = html_alloc_string( buf.c_str(), buf.length() );
	} else if (ch==-1) {
		return false;
	} else {
		is.unget(ch);
	}
 }
 return true;
}

bool	html_parser::skip_tag_tail()
{
 while(true) {
	int ch = is.get();
	if( ch==-1 ) return false;
	if( ch=='>' ) break;
	if( ch=='\"' || ch=='\'' ) {
		while(true) {
			int c = is.get();
			if( c==-1 ) return false;
			if( c==ch ) break;
		}
	}
 }
 return true;
}

void	html_parser::parse_sequence()
{
 string buf;
 bool f_eof = false;

 while(!f_eof) {
	int tok = scan_token( buf );
	switch(tok) {
	case HTML_TOK_STAG:
		add_tag( buf );
		break;
	case HTML_TOK_ETAG:
		add_etag( buf );
		break;
	case HTML_TOK_SGML:
		add_sgml( buf );
		break;
	case HTML_TOK_CDATA:
		add_cdata( buf );
		break;
	case HTML_TOK_COMMENT:
		add_comment( buf );
		break;
	case HTML_TOK_BLANK:
		if( last_elt!=NULL && strpbrk( buf.c_str(), "\r\n" )!=NULL ) {
			last_elt->flags |= HTML_F_LF;
		}
		break;
	case HTML_TOK_EOF:
		f_eof = true;
		break;
	}
 }
}

void	html_parser::add_tag( string& buf )
{
 // create element
 html_elt_def *def = get_elt_defn( buf.c_str() );

 html_elt_tag *elt = RC_OBJECT_NEW(html_elt_tag);
 if ( elt )
 {
	 elt->def = def;
	 append_element( elt );
	 scan_tag_attrs( buf, elt->attrs );	// buf will be erased
 }

 // check for elements with 'opaque' content
 if( def && (def->cflags & HTML_EF_OPAQUE) ) {
	// process opaque content
	scan_to_etag( def->tag, buf );
	skip_tag_tail();

	add_cdata( buf );
	// generate ending tag
	html_elt_etag *etag = RC_OBJECT_NEW(html_elt_etag);
	if ( etag )
	{
		etag->def = def;
		def->duplicate();
		append_element( etag );
	}
 }
}

void	html_parser::add_etag( string& buf )
{
 html_elt_etag *etag = RC_OBJECT_NEW(html_elt_etag);
 if ( etag )
 {
	 etag->def = get_elt_defn( buf.c_str() );
	 append_element( etag );
 }
 skip_tag_tail();
}

void	html_parser::add_sgml( string& buf )
{
 html_elt_sgml * elt = RC_OBJECT_NEW(html_elt_sgml);
 if ( elt )
 {
	 elt->str = html_alloc_string( buf.c_str(), buf.length() );
	 append_element( elt );
 }
}

void	html_parser::add_cdata( string& buf )
{
 html_elt_text * elt = RC_OBJECT_NEW(html_elt_text);
 if ( elt )
 {
	 elt->str = html_alloc_string( buf.c_str(), buf.length() );
	 append_element( elt );
 }
}

void	html_parser::add_comment( string& buf )
{
 html_elt_comment * elt = RC_OBJECT_NEW(html_elt_comment);
 if ( elt )
 {
	 elt->str = html_alloc_string( buf.c_str(), buf.length() );
	 append_element( elt );
 }
}

void	html_parser::append_element( html_element *elt )
{
 if( last_elt!=NULL ) {
	last_elt->next = elt;
 } else {
	first_elt = elt;
 }
 last_elt = elt;
}

html_elt_def	*html_parser::get_elt_defn( const char *name )
{
 // search known tags
 html_elt_def *def = html_find_element( name );
 if( def!=NULL ) return def;

 // search unknown tags
 int cnt = unknown_defs.size();
 int i;
 for( i=0; i<cnt; ++i ) {
	html_elt_def_dynamic *udef = unknown_defs[i];
	if( strcmp( udef->tag, name )==0 ) {
		udef->duplicate();
		return udef;
	}
 }

 // create new definition
 html_elt_def_dynamic *udef = RC_OBJECT_NEW(html_elt_def_dynamic);
 if ( udef )
 {
	 udef->tag = html_alloc_string( name, strlen(name) );
	 unknown_defs.push_back(udef);
 }

 return udef;
}

////////////////////////////////////////////////////////////////
// character classificator

int	html_char_class( int ch )
{
 if( INLINE::isspace(ch) ) return HTML_CC_BLANK;
 if( INLINE::isalpha(ch) ) return HTML_CC_NAME|HTML_CC_NMSTART;
 if( INLINE::isdigit(ch) || ch=='.' || ch=='-' || ch==':' ) return HTML_CC_NAME;
 return 0;
}

////////////////////////////////////////////////////////////////
// element decoder (pass 2)

// search through element stack
bool	html_parser::find_etag( html_elt_etag *p, elt_parse_state *state )
{
 for( ; state!=NULL; state=state->link ) {
	if( state->elt!=NULL ) {
		if( p->def == (state->elt)->def ) return true;
	}
 }
 return false;
}

void	html_parser::parse_elt_seq( html_elt_list& elts, elt_parse_state *state )
{
 html_elt_list_iterator iter( &elts );
 iter.seek(NULL);	// seek to tail

 while(true) {
	html_element *elt = get_token();
	if( elt==NULL ) break;

	// TODO: check type & tag validity for given context
	html_elt_tag *tag = dynamic_cast<html_elt_tag *>(elt);
	html_elt_etag *etag = dynamic_cast<html_elt_etag *>(elt);
	if( tag!=NULL ) {
		if( state!=NULL && state->elt!=NULL ) {
			html_elt_def *def = (state->elt)->def;
//			if( def->flags & HTML_EF_OETAG ) {
			if( def->cmodel != 0 ) {
				// check if element matches parent content model
				if( ((tag->def)->cflags & def->cmodel)==0 ) break;
			}
		}

		next_token();
		parse_elt( tag, state );
	} else if( etag!=NULL ) {
		if( find_etag( etag, state ) ) break;
			// TODO: mark as defective ?
		next_token();
	} else {
		next_token();
	}

	iter.insert(elt);
	elt->release();
 }
}

void	html_parser::parse_elt( html_elt_tag *elt, elt_parse_state *state )
{
 // empty tag ?
 if( (elt->def)->cflags & HTML_EF_EMPTY ) {
	// NOTE: DIRTY HACK:
	// make all TM_ELT comments, found after no-ETAG element, its childs

	html_elt_list_iterator iter( &(elt->childs) );
	iter.seek(NULL);	// seek to tail

	while(true) {
		html_element *nxt = get_token();
		if( nxt==NULL ) break;
		html_elt_comment *cmt = dynamic_cast<html_elt_comment *>(nxt);
		if( cmt==NULL ) break;

//DVi		const char *txt = cmt->str;
//DVi		if( strncmp( txt, HTML_TM_SIGN, HTML_TM_SIGN_LEN )!=0 ) break;

//DVi		const html_tm_sign& SV = HTML_TM_SIGNS[TM_SIGN_ELT];
//DVi		if( strncmp( txt+HTML_TM_SIGN_LEN, SV.txt, SV.len )!=0 ) break;

		next_token();

		iter.insert(cmt);
		cmt->release();
	}
	return;
 }

 elt_parse_state nst;
 nst.link = state;
 nst.elt = elt;

 parse_elt_seq( elt->childs, &nst );
 
 html_element *nxt = get_token();
 html_elt_etag *etag = dynamic_cast<html_elt_etag *>(nxt);

 if( etag!=NULL && etag->def==elt->def ) {
	// found our own ETAG
	elt->flags |= HTML_F_ETAG;
	if( etag->flags & HTML_F_LF ) elt->flags |= HTML_F_LFE;
	next_token();
	// kill ending tag
	etag->release();
 }
}

////////////////////////////////////////////////////////////////
// comment escaping & unescaping
//
// characters converted:
// & <-> &amp;
// < <-> &lt;
// > <-> &gt;

void	html_escape_comment( const char *s, string& d )
{
 d.resize(0);
 for( ; *s!=0; ++s ) {
	switch(*s) {
	case '&': d += "&amp;"; break;
	case '<': d += "&lt;"; break;
	case '>': d += "&gt;"; break;
	default:  d += *s;
	}
 }
}

void	html_unescape_comment( const char *s, string& d )
{
 d.resize(0);
 for( ; *s!=0; ++s ) {
	if( *s=='&' ) {
		if( s[1]=='a' && s[2]=='m' && s[3]=='p' && s[4]==';' ) {
			d += '&';
			s += 4;
			continue;
		}
		if( s[1]=='l' && s[2]=='t' && s[3]==';' ) {
			d += '<';
			s += 3;
			continue;
		}
		if( s[1]=='g' && s[2]=='t' && s[3]==';' ) {
			d += '>';
			s += 3;
			continue;
		}
	}
	d += *s;
 }
}

