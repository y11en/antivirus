// $Id: html_utl.cpp,v 1.3 2000/05/06 13:12:47 slavikb Exp $
//

#include "htmparse.h"

////////////////////////////////////////////////////////////////
// allocation utilites

char *	html_alloc_string( const char *s, int len )
{
 if( len==-1 ) len = strlen(s);
 char *p = (char *)malloc((len+1)*sizeof(char));
 if( p!=NULL && s!=NULL ) {
	memcpy( p, s, len*sizeof(char) );
	p[len] = 0;
 }
 return p;
}

html_elt_list::~html_elt_list()
{
 if( head!=NULL ) {
	destroy();
 }
}

void	html_elt_list::destroy()
{
 html_element *p = head;
 head = NULL;
 while( p!=NULL ) {
	html_element *pn = p->next;
	p->release();
	p = pn;
 }
}

////////////////////////////////////////////////////////////////
// other utilites

html_attr * html_find_attr( html_elt_list_iterator& iter, const char *id )
{
 while(true) {
	html_element *pe = iter.current();
	if( pe==NULL ) return NULL;

	html_attr *pa = dynamic_cast<html_attr *>(pe);
	if( pa!=NULL && pa->name!=NULL && strcmp(pa->name,id)==0 ) return pa;

	iter.next();
 }
}

const char *html_get_attr_s( html_elt_list& attrs, const char *id )
{
 html_elt_list_iterator iter(&attrs);
 html_attr *pa = html_find_attr( iter, id );
 if( pa==NULL ) return NULL;
 return pa->value;
}

bool	html_get_attr_i( html_elt_list& attrs, const char *id, int& val )
{
 val = 0;
 const char *s = html_get_attr_s( attrs, id );
 if( s==NULL ) return false;
 char *pe = NULL;
 long l = strtol( s, &pe, 10 );
 if( pe==s || pe==NULL ) return false;
 val = (int)l;
 return true;
}

bool	html_get_attr_ul( html_elt_list& attrs, const char *id, unsigned long& val )
{
 val = 0;
 const char *s = html_get_attr_s( attrs, id );
 if( s==NULL ) return false;
 char *pe = NULL;
 val = strtoul( s, &pe, 10 );
 if( pe==s || pe==NULL ) return false;
 return true;
}

void	html_remove_attr( html_elt_list& attrs, const char *id )
{
 html_elt_list_iterator iter(&attrs);
 while(true) {
	if( html_find_attr( iter, id )==NULL ) break;
	iter.erase();
 }
}

void	html_set_attr_s( html_elt_list& attrs, const char *id, const char *val )
{
 html_elt_list_iterator iter(&attrs);
 html_attr *pa = html_find_attr( iter, id );
 if( pa==NULL ) {
	// TODO: use the proper allocator
	pa = RC_OBJECT_NEW( html_attr );
	if ( pa )
	{
		pa->name = html_alloc_string( id, strlen(id) );
		iter.insert( pa );
		pa->release();
	}
 } else {
	if( pa->value!=NULL ) {
		pa->get_allocator()->free(pa->value);
		pa->value = NULL;
	}
 }
 if( val!=NULL ) pa->value = html_alloc_string( val, strlen(val) );

 // delete clones
 iter.next();
 while(true) {
	if( html_find_attr( iter, id )==NULL ) break;
	iter.erase();
 }
}

void	html_set_attr_i( html_elt_list& attrs, const char *id, int val )
{
 char buf[20];
 sprintf( buf, "%d", val );
 html_set_attr_s( attrs, id, buf );
}

void	html_set_attr_ul( html_elt_list& attrs, const char *id, unsigned long val )
{
 char buf[20];
 sprintf( buf, "%lu", val );
 html_set_attr_s( attrs, id, buf );
}



