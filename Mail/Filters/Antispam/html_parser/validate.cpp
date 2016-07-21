// validate.cpp
//
// $Revision: 1.3 $
// $Date: 2000/05/06 13:12:47 $
//

#include "htmparse.h"

#ifndef NO_RCSID
static char rcsid[] = "$Id: validate.cpp,v 1.3 2000/05/06 13:12:47 slavikb Exp $";
#endif

////////////////////////////////////////////////////////////////
// Postprocessor / validator

enum {
	HF_NEUTRAL =	0x0001,		// SGML|COMMENT
	HF_SGML =	0x0002,
	HF_COMMENT =	0x0004,
	HF_CDATA =	0x0008,
	HF_HTML =	0x0010,
	HF_HEAD =	0x0020,
	HF_HELT =	0x0040,
	HF_TITLE =	0x0080,
	HF_BODY =	0x0100,
	HF_BELT =	0x0200,
};

static int	html_elt_flags( html_element *pe )
{
 html_elt_sgml *p_sgml = dynamic_cast<html_elt_sgml *>(pe);
 if( p_sgml!=NULL ) return HF_NEUTRAL|HF_SGML;

 html_elt_comment *p_cmt = dynamic_cast<html_elt_comment *>(pe);
 if( p_cmt!=NULL ) return HF_NEUTRAL|HF_COMMENT;

 html_elt_text *p_text = dynamic_cast<html_elt_text *>(pe);
 if( p_text!=NULL ) return HF_CDATA|HF_BELT;

 html_elt_tag *tag = dynamic_cast<html_elt_tag *>(pe);
 if( tag==NULL ) return 0;

 if( tag->def==HTML_ELT_DEF(HTML) ) {
	return HF_HTML;
 } else if( tag->def==HTML_ELT_DEF(HEAD) ) {
	return HF_HEAD;
 } else if( tag->def==HTML_ELT_DEF(BODY) ) {
	return HF_BODY;
 } else if( tag->def==HTML_ELT_DEF(TITLE) ) {
	return HF_TITLE|HF_HELT;
 }

 int cf = tag->def->cflags;
 int f = HF_BELT;
 if( cf & HTML_EF_HEAD ) f |= HF_HELT;

 return f;
}

// raises level of all childs, placing them after parent node
void	html_tag_raise_childs( html_elt_tag *tag )
{
 if( tag==NULL || tag->childs.head==NULL ) return;

 html_element *elt = tag->childs.head;
 for( ; elt->next!=NULL; elt=elt->next );
 elt->next = tag->next;
 tag->next = tag->childs.head;
 tag->childs.head = NULL;
}

// moves elements from (tag->next) until (term) under (tag)
void	html_tag_drown_childs( html_elt_tag *tag, html_element *term )
{
 html_element **pplast = &(tag->childs.head);
 while( *pplast!=NULL ) pplast = &((*pplast)->next);

 html_element *elt = tag->next;
 while( elt!=NULL && elt!=term ) {
	html_element *next = elt->next;

	tag->next = next;
	elt->next = *pplast;
	*pplast = elt;
	pplast = &(elt->next);

	elt = next;
 }
}

// removes <HTML>, <HEAD> and <BODY> tags
static void	remove_special_tags( html_elt_list& elts )
{
 html_elt_list_iterator iter(&elts);
 while(true) {
	html_element *elt = iter.current();
	if( elt==NULL ) break;

	html_elt_tag *tag = dynamic_cast<html_elt_tag *>(elt);
	if( tag!=NULL ) {
		if( html_elt_flags(tag) & (HF_HTML|HF_HEAD|HF_BODY) ) {
			html_tag_raise_childs(tag);
			iter.erase();
			continue;
		} else {
			remove_special_tags( tag->childs );
		}
	}
	iter.next();
 }
}

// searches for <HEAD> element, if not found - creates it
static html_elt_tag *make_head_element( html_elt_list& elts )
{
 html_elt_list_iterator iter(&elts);

 // try to locate <HEAD>
 html_element *elt = iter.current();
 for( ; elt!=NULL; elt=elt->next ) {
	int ef = html_elt_flags(elt);
	if( ef & HF_HEAD ) return dynamic_cast<html_elt_tag *>(elt);
	if( (ef & HF_NEUTRAL)==0 ) break;
 }

 // scan to the first non-HEAD element
 html_element *term = iter.current();
 for( ; term!=NULL; term=term->next ) {
	int ef = html_elt_flags(term);
	if( (ef & (HF_NEUTRAL|HF_HEAD|HF_HELT))==0 ) break;
 }
 // now: create new <HEAD> element at (iter);
 // move all elements from (iter) to (term) under new <HEAD> element
 html_elt_tag *head = RC_OBJECT_NEW( html_elt_tag );
 if ( head )
 {
	 head->def = HTML_ELT_DEF(HEAD);
	 head->flags |= HTML_F_LF|HTML_F_LFE;
	 iter.insert(head);
	 head->release();
	 html_tag_drown_childs( head, term );
 }
 return head;
}

static html_elt_tag *find_html_element( html_elt_list& elts )
{
 html_elt_list_iterator iter(&elts);
 while(true) {
	html_element *elt = iter.current();
	if( elt==NULL ) break;
	int ef = html_elt_flags( elt );
	if( (ef & HF_NEUTRAL)==0 ) {
		if( ef & HF_HTML ) return dynamic_cast<html_elt_tag *>(elt);
		break;
	}
	iter.next();
 }
 return NULL;
}

bool	html_postprocess( html_elt_list& elts )
{
 html_elt_tag *html = find_html_element(elts);
 if( html==NULL ) return false;

 // place all following elements under <HTML>
 html_tag_drown_childs( html, NULL );

 html_elt_tag *head = make_head_element( html->childs );
 if ( head )
	 remove_special_tags( head->childs );

 // create BODY element
 html_elt_list_iterator iter(&(html->childs));
 iter.seek(head);
 iter.next();

 html_elt_tag *body = dynamic_cast<html_elt_tag *>(iter.current());
 if( body!=NULL && (html_elt_flags(body) & HF_BODY)==0 ) body = NULL;

 if( body==NULL ) {
	// if <BODY> not found, create it
	body = RC_OBJECT_NEW(html_elt_tag);
	if ( body )
	{
		body->def = HTML_ELT_DEF(BODY);
		body->flags |= HTML_F_LF|HTML_F_LFE;
		iter.insert( body );
		body->release();
	}
 }
 // put all childs under the BODY
 html_tag_drown_childs( body, NULL );
 remove_special_tags( body->childs );

 return true;
}



