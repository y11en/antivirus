// xml_utils.h
//

#ifndef xml_utils_h_INCLUDED
#define xml_utils_h_INCLUDED

#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Prague/pr_vector.h>

#define LAX_NO_STL
#define LAX_NO_IMPLEMENTATION
#include <LAX.h>

// uncomment this when Petrovich implemtents UTF-8 support for Prague strings
// const tCODEPAGE cCP_XML = cCP_UTF8;
const tCODEPAGE cCP_XML = cCP_ANSI;

////////////////////////////////////////////////////////////////
// String adapter for UTF-8 cStringObj

class cStringAdapter
{
public:
	typedef char char_t;
	typedef cStringObj string_t;

	inline static void clear (string_t& str)
		{ str.clear (); }

	inline static void append (string_t& str, const char_t *s, size_t len)
		{ str.append (s, cCP_XML, len); }

	inline static void append_char (string_t& str, char_t c)
		{ str.append (&c, cCP_XML, 1); }

	static bool is_equal (const string_t& str, const char_t *s)
		{ const char *s1 = str.c_str (cCP_XML);
		  for (;;) {
			if (*s1 != *s) return false;
			if (*s1 == 0) break;
			++s1; ++s;
		  }
		  return true;
		}

	inline static const char_t * c_str (const string_t& str)
		{ return str.c_str (cCP_XML); }
};

////////////////////////////////////////////////////////////////
// Attribute map implementation

struct cXmlAttr
{
	cStringObj name;
	cStringObj value;
};

class cXmlAttrMap
{
public:
	typedef char  char_t;
	typedef cStringObj string_t;
	typedef cStringAdapter STRING_ADAPTER;

	typedef cXmlAttr Attr;

	void clear ()
		{ m_attrs.clear (); }

	size_t size () const
		{ return m_attrs.size (); }

	bool empty () const
		{ return m_attrs.empty (); }

	const Attr& get (size_t idx) const
		{ return m_attrs[idx]; }

	const Attr& operator[] (size_t idx) const
		{ return m_attrs[idx]; }

	int addAttr (const char_t *name, size_t nlen, const char_t *value, size_t vlen);

	const Attr * find (const char_t *name) const;

private:
	cVector <Attr> m_attrs;
};

////////////////////////////////////////////////////////////////
// Adaptors for UTF-8 XML parser with no exceptions


//extern template LAX::UTF8_UTF8 <cStringAdapter>;
typedef LAX::UTF8_UTF8 <cStringAdapter> cXmlTransform;

//extern template LAX::STD_ATTR_MAP_ADAPTER<char, cXmlAttrMap>;

// cXmlReader
//extern template LAX::XmlReader <cXmlTransform, LAX::STD_ATTR_MAP_ADAPTER<char, cXmlAttrMap>, LAX::XmlNoExceptions>;
typedef LAX::XmlReader <cXmlTransform, LAX::STD_ATTR_MAP_ADAPTER<char, cXmlAttrMap>, LAX::XmlNoExceptions> cXmlReader;

// cXmlElement
typedef cXmlReader::Element cXmlElement;

// cXmlWriter
typedef LAX::XmlWriter <cXmlTransform> cXmlWriter; 
#endif // xml_utils_h_INCLUDED


