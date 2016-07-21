// klav_string.cpp
//

#include <klava/klav_string.h>
#include <string.h>
#include <utf8.h>

////////////////////////////////////////////////////////////////

static inline size_t wcs_len (const wchar_t *s)
{
	size_t n = 0;
	while (s [n] != 0) ++n;
	return n;
}

struct String_Hdr
{
	KLAV_Alloc * m_allocator;
	size_t       m_length;
};

#define STRING_HDR(STR) ((String_Hdr *)(((char *)(STR)) - sizeof (String_Hdr)))

////////////////////////////////////////////////////////////////

const char * klav_string::assign (const klav_string& s)
{
	if (s.m_str == 0)
	{
		clear ();
		return 0;
	}
	
	String_Hdr *hdr = STRING_HDR (s.m_str);
	return assign (s.m_str, hdr->m_length, hdr->m_allocator);
}

const char * klav_string::assign (const wchar_t *s, KLAV_Alloc *alloc)
{
	if (s == 0)
	{
		clear ();
		return 0;
	}
	
	return assign (s, wcs_len (s), alloc);
}

const char * klav_string::assign (const wchar_t *s, size_t len, KLAV_Alloc *alloc)
{
	size_t nlen = len * UTF8_MAX_GROW_FACTOR + 1;

	String_Hdr * hdr = (String_Hdr *) (alloc->alloc (sizeof (String_Hdr) + nlen));
	if (hdr == 0)
		return 0;

	hdr->m_allocator = alloc;

	char *nstr = ((char *) hdr) + sizeof (String_Hdr);
	nlen = utf8_encode_string (s, len, nstr);
	nstr [nlen] = 0;

	hdr->m_length = nlen;

	clear ();
	m_str = nstr;
	return m_str;
}

const char * klav_string::assign (const char *s, KLAV_Alloc *alloc)
{
	if (s == 0)
	{
		clear ();
		return 0;
	}
	
	return assign (s, strlen (s), alloc);
}

const char * klav_string::assign (const char *s, size_t len, KLAV_Alloc *alloc)
{
	String_Hdr * hdr = (String_Hdr *) (alloc->alloc (sizeof (String_Hdr) + len + 1));
	if (hdr == 0)
		return 0;

	hdr->m_allocator = alloc;
	hdr->m_length = len;
	
	char *nstr = ((char *) hdr) + sizeof (String_Hdr);

	memcpy (nstr, s, len);
	nstr [len] = 0;

	clear ();
	
	m_str = nstr;
	return m_str;
}

void klav_string::clear ()
{
	if (m_str != 0)
	{
		String_Hdr *hdr = STRING_HDR (m_str);
		hdr->m_allocator->free (hdr);
		m_str = 0;
	}
}

size_t klav_string::length () const
{
	if (m_str == 0)
		return 0;
		
	return STRING_HDR (m_str)->m_length;
}

////////////////////////////////////////////////////////////////

const wchar_t * klav_wstring::assign (const klav_wstring& s)
{
	if (s.m_str == 0)
	{
		clear ();
		return 0;
	}
	
	String_Hdr *hdr = STRING_HDR (s.m_str);
	return assign (s.m_str, hdr->m_length, hdr->m_allocator);
}

const wchar_t * klav_wstring::assign (const char *s, KLAV_Alloc *alloc)
{
	if (s == 0)
	{
		clear ();
		return 0;
	}

	return assign (s, strlen (s), alloc);
}

const wchar_t * klav_wstring::assign (const char *s, size_t len, KLAV_Alloc *alloc)
{
	size_t nlen = len + 1;

	String_Hdr * hdr = (String_Hdr *) (alloc->alloc (sizeof (String_Hdr) + nlen * sizeof (wchar_t)));
	if (hdr == 0)
		return 0;

	hdr->m_allocator = alloc;

	wchar_t *nstr = (wchar_t *)(((char *) hdr) + sizeof (String_Hdr));
	nlen = utf8_decode_string (s, len, nstr);
	nstr [nlen] = 0;

	hdr->m_length = nlen;

	clear ();
	m_str = nstr;
	return m_str;
}

const wchar_t * klav_wstring::assign (const wchar_t *s, KLAV_Alloc *alloc)
{
	if (s == 0)
	{
		clear ();
		return 0;
	}
	
	return assign (s, wcs_len (s), alloc);
}

const wchar_t * klav_wstring::assign (const wchar_t *s, size_t len, KLAV_Alloc *alloc)
{
	String_Hdr * hdr = (String_Hdr *) (alloc->alloc (sizeof (String_Hdr) + (len + 1) * sizeof (wchar_t)));
	if (hdr == 0)
		return 0;

	hdr->m_allocator = alloc;
	hdr->m_length = len;
	
	wchar_t *nstr = (wchar_t *)(((char *) hdr) + sizeof (String_Hdr));

	memcpy (nstr, s, len * sizeof (wchar_t));
	nstr [len] = 0;

	clear ();
	
	m_str = nstr;
	return m_str;
}

void klav_wstring::clear ()
{
	if (m_str != 0)
	{
		String_Hdr *hdr = STRING_HDR (m_str);
		hdr->m_allocator->free (hdr);
		m_str = 0;
	}
}

size_t klav_wstring::length () const
{
	if (m_str == 0)
		return 0;
		
	return STRING_HDR (m_str)->m_length;
}

