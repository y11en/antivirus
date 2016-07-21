#ifndef __html_h
#define __html_h

#include <windows.h>
#include <stuff/cpointer.h>
#include <stuff/parray.h>


class CHtmlTable;

// ---
class CElem {
public :
	static bool aligned;
	static bool lf;
	CElem() {}
	virtual bool flush( HANDLE file, int shift ) = 0;
};


// ---
class CStr : public CElem {
	const char* m_str;
	bool        m_lf;

public :
	CStr( const char* str, bool lf ) : m_str(str), m_lf(lf) {}
	~CStr() { if (m_str) delete [] (char*)m_str; }
	virtual bool flush( HANDLE file, int shift );
	bool& lf() { return m_lf; }

public:
	static const char* find_crlf( const char* b, const char* e, int& l );
	static char* copy_with_br( const char* b, const char* e, bool no_trailing_space, bool add_last_br = false );
};


// ---
class CHtml : public CElem {
protected:
	bool            m_closing;
	CAPointer<char> m_tag;
	CPArray<char>   m_attrs;
	CPArray<CElem>  m_elems;

protected:
	bool write_tag( HANDLE file, int shift, bool opened );
		
public :
	CHtml( const char* ptag, const char* pattr=0, const char* pval=0, bool closing=true );
	bool&  closing();
	CHtml* add( const char* ptag, const char* pattr=0, const char* pval=0, bool closing=true );
	CHtml* para( int shift = 0 );
	CHtml* span( const char* attr, const char* val );
	CHtml* style( const char* style );
	CHtml* table( CPArray<char>& titles, int shift = 0 );
	CHtml* href( const char* url, bool internal=true );
	CHtml* lable( const char* name );
	
	void   attr( const char* name, const char* val );
	void   str( const char* str );
	void   line( const char* line );
	void   styled_text( const char* style, const char* test );
	
	virtual bool   flush( HANDLE file, int shift );
};

	

// ---
inline CHtml::CHtml( const char* ptag, const char* pattr, const char* pval, bool closing ) : m_closing(closing) {
	int len = ptag ? ::lstrlen(ptag) : 0;
	m_tag = (char*)::memcpy( new char[len+1], ptag, len+1 );
	if ( pattr )
		attr( pattr, pval );
}


// ---
inline bool& CHtml::closing() {
	return m_closing;
}


// ---
inline CHtml* CHtml::add( const char* tag, const char* pattr, const char* pval, bool closing ) {
	CHtml* el = new CHtml( tag, pattr, pval, closing );
	if ( el )
		m_elems.Add( el );
	return el;
}


// ---
inline CHtml* CHtml::span( const char* pattr, const char* pval ) {
	CHtml* el = new CHtml( "span", pattr, pval, true );
	if ( el )
		m_elems.Add( el );
	return el;
}


// ---
inline CHtml* CHtml::style( const char* style ) {
	CHtml* el = new CHtml( "span", "class", style, true );
	if ( el )
		m_elems.Add( el );
	return el;
}


// ---
inline CHtml* CHtml::lable( const char* name ) {
	CHtml* el = new CHtml( "a", "name", name, true );
	if ( el )
		m_elems.Add( el );
	return el;
}


// ---
inline CHtml* CHtml::para( int shift ) {
	CHtml* ret = add( "p" );
	if ( shift )
		ret = ret->add( "span", "class", "indent" );
	return ret;
}


// ---
inline void CHtml::styled_text( const char* tstyle, const char* str ) {
	if ( str && *str ) 
		style(tstyle)->line( str );
}




#endif


