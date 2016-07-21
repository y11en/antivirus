// klav_string.h
//

#ifndef klav_string_h_INCLUDED
#define klav_string_h_INCLUDED

#include <klava/klavdef.h>

////////////////////////////////////////////////////////////////

struct klav_string
{
public:
	klav_string ()
		: m_str (0) {}

	klav_string (const klav_string& s)
		: m_str (0) { assign (s); }	

	klav_string (const char *s, KLAV_Alloc *alloc)
		: m_str (0) { assign (s, alloc); }	

	klav_string (const char *s, size_t len, KLAV_Alloc *alloc)
		: m_str (0) { assign (s, len, alloc); }	

	klav_string (const wchar_t *s, KLAV_Alloc *alloc)
		: m_str (0) { assign (s, alloc); }	

	klav_string (const wchar_t *s, size_t len, KLAV_Alloc *alloc)
		: m_str (0) { assign (s, len, alloc); }	

	~klav_string ()
		{ clear (); }

	klav_string& operator= (const klav_string& s)
		{ assign (s); return *this; }
		
	const char * assign (const klav_string& s);
	const char * assign (const char *s, KLAV_Alloc *alloc);
	const char * assign (const char *s, size_t len, KLAV_Alloc *alloc);
	const char * assign (const wchar_t *s, KLAV_Alloc *alloc);
	const char * assign (const wchar_t *s, size_t len, KLAV_Alloc *alloc);

	void clear ();

	const char * c_str () const
		{ return m_str; }

	size_t length () const;

private:
	char * m_str;
};

////////////////////////////////////////////////////////////////

struct klav_wstring
{
public:
	klav_wstring ()
		: m_str (0) {}

	klav_wstring (const klav_wstring& s)
		: m_str (0) { assign (s); }	

	klav_wstring (const char *s, KLAV_Alloc *alloc)
		: m_str (0) { assign (s, alloc); }	

	klav_wstring (const char *s, size_t len, KLAV_Alloc *alloc)
		: m_str (0) { assign (s, len, alloc); }	

	klav_wstring (const wchar_t *s, KLAV_Alloc *alloc)
		: m_str (0) { assign (s, alloc); }	

	klav_wstring (const wchar_t *s, size_t len, KLAV_Alloc *alloc)
		: m_str (0) { assign (s, len, alloc); }	

	~klav_wstring ()
		{ clear (); }

	klav_wstring& operator= (const klav_wstring& s)
		{ assign (s); return *this; }
		
	const wchar_t * assign (const klav_wstring& s);
	const wchar_t * assign (const wchar_t *s, KLAV_Alloc *alloc);
	const wchar_t * assign (const wchar_t *s, size_t len, KLAV_Alloc *alloc);
	const wchar_t * assign (const char *s, KLAV_Alloc *alloc);
	const wchar_t * assign (const char *s, size_t len, KLAV_Alloc *alloc);

	void clear ();

	const wchar_t * c_str () const
		{ return m_str; }

	size_t length () const;

private:
	wchar_t * m_str;
};

#endif // klav_string_h_INCLUDED

