// LAX.h
//
// LAX: Lightweight API for XML
//
// (C) 2003 Kaspersky Labs
//

#ifndef LAX_h_INCLUDED
#define LAX_h_INCLUDED

#include <kl_types.h>
#include <stdlib.h>

#ifndef LAX_NO_STL
# include <exception>
# include <vector>
#endif

#include <utf8.h>

namespace LAX
{

////////////////////////////////////////////////////////////////
// string utilites

template <typename T>
inline size_t str_len (const T *s)
  { size_t n=0; if (s) for (; s[n]; ++n); return n; }

template <typename T>
inline bool str_eqn (const T *a, const T *b, size_t len)
  { for (size_t i=0; i<len; ++i) {
		if (a[i] != b[i]) return false;
	}
	return true;
  }

////////////////////////////////////////////////////////////////

inline unsigned int char_value (char c)
  { return (unsigned char)c; }

inline unsigned int char_value (wchar_t c)
  { return c; }

////////////////////////////////////////////////////////////////
// fixed-size UTF16/UTF32 character types

typedef char utf8_t;

#if KL_WCHAR_T_SIZE == 16

typedef wchar_t  utf16_t;
typedef uint32_t utf32_t;
#define UTF8_WCHAR_T UTF8_UTF16

#elif KL_WCHAR_T_SIZE == 32

typedef uint16_t utf16_t;
typedef wchar_t  utf32_t;
#define UTF8_WCHAR_T UTF8_UTF32

#else // KL_WCHAR_T_SIZE

typedef uint16_t utf16_t;
typedef uint32_t utf32_t;

#endif // KL_WCHAR_T_SIZE

////////////////////////////////////////////////////////////////
// Encodings

class UTF8
{
public:
	typedef utf8_t char_t;
	static inline const char *NAME () { return "UTF8"; }
};

class UTF16
{
public:
	typedef utf16_t char_t;
	static inline const char *NAME () { return "UTF16"; }
};

class UTF32
{
public:
	typedef utf32_t char_t;
	static inline const char *NAME () { return "UTF32"; }
};

////////////////////////////////////////////////////////////////
// std::string adapters

template <typename CHAR, typename STRING>
class STD_STRING_ADAPTER
{
public:
	typedef CHAR char_t;
	typedef STRING string_t;

	inline static void clear (string_t& str)
		{ str.resize (0); }

	inline static void append (string_t& str, const char_t *s, size_t len)
		{ str.append (s, len); }

	inline static void append_char (string_t& str, char_t c)
		{ str += c; }

	inline static char_t * reserve (string_t& str, size_t len)
		{ const size_t org = str.length ();
		  str.resize (org + len);
		  return (char_t*)(str.data() + org);
		}

	inline static void commit (string_t& str, char_t *buf, size_t size)
		{ str.resize ((buf - str.data()) + size); }

	inline static bool is_equal (const string_t& str, const char_t *s)
		{ return str == s; }

	inline static const char_t * c_str (const string_t& str)
		{ return str.c_str (); }
};

////////////////////////////////////////////////////////////////
// Transformations

////////////////////////////////////////////////////////////////
// UTF8 -> UTF8 transformation

template <typename STR_ADAPT>
class UTF8_UTF8
{
public:
	typedef UTF8 SRC_ENCODING;
	typedef UTF8 DST_ENCODING;
	typedef STR_ADAPT STRING_ADAPTER;
	typedef typename SRC_ENCODING::char_t char_t;
	typedef typename DST_ENCODING::char_t strchar_t;
	typedef typename STRING_ADAPTER::string_t string_t;

	inline static void append (string_t& str, const char_t *s, size_t len)
		{ STRING_ADAPTER::append (str, s, len); }

	inline static void append_char (string_t& str, unsigned int uc)
		{ char tmp[10]; STRING_ADAPTER::append (str, tmp, utf8_encode_char (uc, tmp)); }
};

////////////////////////////////////////////////////////////////
// UTF8 -> UTF16 transformation

template <typename STR_ADAPT>
class UTF8_UTF16
{
public:
	typedef UTF8  SRC_ENCODING;
	typedef UTF16 DST_ENCODING;
	typedef STR_ADAPT STRING_ADAPTER;
	typedef typename SRC_ENCODING::char_t char_t;
	typedef typename DST_ENCODING::char_t strchar_t;
	typedef typename STRING_ADAPTER::string_t string_t;

	inline static void append (string_t& str, const char_t *s, size_t len)
		{ strchar_t *buf = STRING_ADAPTER::reserve (str, len);
		  STRING_ADAPTER::commit (str, buf, utf8_decode_string (s, len, buf));
		}

	inline static void append_char (string_t& str, unsigned int uc)
		{ STRING_ADAPTER::append_char (str, strchar_t(uc)); }
};

////////////////////////////////////////////////////////////////
// UTF16 -> UTF8 transformation

template <typename STR_ADAPT>
class UTF16_UTF8
{
public:
	typedef UTF16 SRC_ENCODING;
	typedef UTF8  DST_ENCODING;
	typedef STR_ADAPT STRING_ADAPTER;
	typedef typename SRC_ENCODING::char_t char_t;
	typedef typename DST_ENCODING::char_t strchar_t;
	typedef typename STRING_ADAPTER::string_t string_t;

	inline static void append (string_t& str, const char_t *s, size_t len)
		{ strchar_t *buf = STRING_ADAPTER::reserve (str, len * UTF8_MAX_GROW_FACTOR + 1);
		  STRING_ADAPTER::commit (str, buf, utf8_encode_string (s, len, buf));
		}

	inline static void append_char (string_t& str, unsigned int uc)
		{ char tmp[10]; STRING_ADAPTER::append (str, tmp, utf8_encode_char (uc, tmp)); }
};

////////////////////////////////////////////////////////////////
// UTF16 -> UTF16 transformation

template <typename STR_ADAPT>
class UTF16_UTF16
{
public:
	typedef UTF16 SRC_ENCODING;
	typedef UTF16 DST_ENCODING;
	typedef STR_ADAPT STRING_ADAPTER;
	typedef typename SRC_ENCODING::char_t char_t;
	typedef typename DST_ENCODING::char_t strchar_t;
	typedef typename STRING_ADAPTER::string_t string_t;

	inline static void append (string_t& str, const char_t *s, size_t len)
		{ STRING_ADAPTER::append (str, s, len); }

	inline static void append_char (string_t& str, unsigned int uc)
		{ STRING_ADAPTER::append_char (str, strchar_t(uc)); }
};

////////////////////////////////////////////////////////////////
// UTF8 -> UTF32 transformation

template <typename STR_ADAPT>
class UTF8_UTF32
{
public:
	typedef UTF8  SRC_ENCODING;
	typedef UTF32 DST_ENCODING;
	typedef STR_ADAPT STRING_ADAPTER;
	typedef typename SRC_ENCODING::char_t char_t;
	typedef typename DST_ENCODING::char_t strchar_t;
	typedef typename STRING_ADAPTER::string_t string_t;

	inline static void append (string_t& str, const char_t *s, size_t len)
		{ strchar_t *buf = STRING_ADAPTER::reserve (str, len);
		  STRING_ADAPTER::commit (str, buf, utf8_decode_string (s, len, buf));
		}

	inline static void append_char (string_t& str, unsigned int uc)
		{ STRING_ADAPTER::append_char (str, strchar_t(uc)); }
};

////////////////////////////////////////////////////////////////
// UTF32 -> UTF8 transformation

template <typename STR_ADAPT>
class UTF32_UTF8
{
public:
	typedef UTF32 SRC_ENCODING;
	typedef UTF8  DST_ENCODING;
	typedef STR_ADAPT STRING_ADAPTER;
	typedef typename SRC_ENCODING::char_t char_t;
	typedef typename DST_ENCODING::char_t strchar_t;
	typedef typename STRING_ADAPTER::string_t string_t;

	inline static void append (string_t& str, const char_t *s, size_t len)
		{ strchar_t *buf = STRING_ADAPTER::reserve (str, len * UTF8_MAX_GROW_FACTOR + 1);
		  STRING_ADAPTER::commit (str, buf, utf8_encode_string (s, len, buf));
		}

	inline static void append_char (string_t& str, unsigned int uc)
		{ char tmp[10]; STRING_ADAPTER::append (str, tmp, utf8_encode_char (uc, tmp)); }
};

////////////////////////////////////////////////////////////////
// UTF32 -> UTF32 transformation

template <typename STR_ADAPT>
class UTF32_UTF32
{
public:
	typedef UTF32 SRC_ENCODING;
	typedef UTF32 DST_ENCODING;
	typedef STR_ADAPT STRING_ADAPTER;
	typedef typename SRC_ENCODING::char_t char_t;
	typedef typename DST_ENCODING::char_t strchar_t;
	typedef typename STRING_ADAPTER::string_t string_t;

	inline static void append (string_t& str, const char_t *s, size_t len)
		{ STRING_ADAPTER::append (str, s, len); }

	inline static void append_char (string_t& str, unsigned int uc)
		{ STRING_ADAPTER::append_char (str, strchar_t(uc)); }
};

////////////////////////////////////////////////////////////////
// utility structure - string reference

template<class CHAR>
struct str_t {
	typedef CHAR char_t;

	const char_t * m_str;
	size_t         m_len;

	str_t () : m_str(NULL), m_len(0)   {}
	str_t (const char_t *s) : m_str(s), m_len(str_len(s))  {}
	str_t (const char_t *s, size_t n) : m_str(s), m_len(n)  {}

	void clear ()
		{ m_str = NULL; m_len = 0; }

	void assign (const char_t *s, size_t n)
		{ m_str = s; m_len = n; }

	void assign (const char_t *s)
		{ m_str = s; m_len = str_len (s); }

	const char_t * c_str() const    { return m_str; }
	size_t  length() const          { return m_len; }

	bool operator== (const str_t& b) const
		{ return m_len == b.m_len && str_eqn (m_str, b.m_str, m_len); }
};

////////////////////////////////////////////////////////////////
// Default attribute map implementation

#ifndef LAX_NO_STL

template<typename TRANSFORM>
class XmlAttrMap
{
public:
	typedef typename TRANSFORM::char_t  char_t;
	typedef typename TRANSFORM::strchar_t strchar_t;
	typedef typename TRANSFORM::string_t string_t;
	typedef typename TRANSFORM::STRING_ADAPTER STRING_ADAPTER;

	struct Attr {
		string_t name;
		string_t value;
	};
	typedef std::vector <Attr> AttrBag;

	void clear ()
		{ m_attrs.resize (0); }

	size_t size () const
		{ return m_attrs.size (); }

	bool empty () const
		{ return m_attrs.empty (); }

	const Attr& get (size_t idx) const
		{ return m_attrs[idx]; }

	const Attr& operator[] (size_t idx) const
		{ return m_attrs[idx]; }

    int addAttr (const char_t *name, size_t nlen, const char_t *value, size_t vlen);

    const strchar_t * find (const strchar_t *name, const strchar_t *defval=0) const;

    AttrBag m_attrs;
};

#endif // LAX_NO_STL

////////////////////////////////////////////////////////////////
// Standard attribute map adapter

template <typename CHAR_TYPE, class ATTR_MAP>
class STD_ATTR_MAP_ADAPTER
{
public:
	typedef CHAR_TYPE char_t;
	typedef ATTR_MAP  attrmap_t;

	inline static void clearAttrs (attrmap_t& attrs)
		{ attrs.clear (); }

	inline static int addAttr (attrmap_t& attrs,
		const char_t *name, size_t nlen,
		const char_t *value, size_t vlen)
		{ return attrs.addAttr (name, nlen, value, vlen); }
};

////////////////////////////////////////////////////////////////
// XML character classification tables

extern const unsigned int XmlNamingBitmap[];
extern const unsigned char XmlNameStartPages[];
extern const unsigned char XmlNamePages[];

inline bool XmlIsSpace (unsigned int ch)
	{ return ch==0x20 || ch==0x0D || ch==0x0A || ch==0x09; }

inline bool XmlIsNameStartChar (unsigned int ch) {
		unsigned char hi = (unsigned char)(ch >> 8);
		unsigned char lo = (unsigned char)(ch);
		return (XmlNamingBitmap[(XmlNameStartPages[hi] << 3) + ((lo) >> 5)] & (1 << ((lo) & 0x1F))) != 0;
	}

inline bool XmlIsNameChar (unsigned int ch) {
		unsigned char hi = (unsigned char) (ch >> 8);
		unsigned char lo = (unsigned char) (ch);
		return (XmlNamingBitmap[(XmlNamePages[hi] << 3) + ((lo) >> 5)] & (1 << ((lo) & 0x1F))) != 0;
	}

////////////////////////////////////////////////////////////////
// XML utilites

template <typename TRANSFORM>
class Xml
{
public:
	typedef typename TRANSFORM::char_t   char_t;
	typedef typename TRANSFORM::string_t string_t;

 static bool escape (const char_t *s, size_t len, string_t& buf);
 static bool unescape (const char_t *s, size_t len, string_t& buf);
};

////////////////////////////////////////////////////////////////
// XML writer

template <typename TRANSFORM>
class XmlWriter
{
public:
	typedef typename TRANSFORM::char_t   char_t;
	typedef typename TRANSFORM::string_t string_t;
	typedef Xml<TRANSFORM> XML;

	XmlWriter () : m_buf(NULL)  {}
	XmlWriter (string_t& buf) : m_buf (&buf) {}

	void init (string_t& buf) { m_buf = &buf; }

    void beginElement (const char_t *tagname)  // <tagname
		{ append_raw ('<'); append (tagname); }

	void writeAttribute (const char_t *name, const char_t *value, size_t vlen = (size_t)-1);

    void endElement ()  // "/>"
		{ append_raw ("/>"); }

	void beginElementBody() // ">"
		{ append_raw ('>'); }

	void endElement (const char_t *tagname)    // </tagname>
		{ append_raw ("</"); append (tagname); append_raw ('>'); }

	void writeProlog (const char *version=NULL, const char *encoding=NULL);

	void beginPI (const char_t *target)       // <?target
		{ append_raw ("<?"); append (target); append_raw (' '); }

	void endPI() // ?>
		{ append_raw ("?>"); }

	void beginCDATA() // <![CDATA[
		{ append_raw ("<![CDATA["); }

	void endCDATA() // ]]>
		{ append_raw ("]]>"); }

	void beginComment() // <!--
		{ append_raw ("<!--"); }

	void endComment() // -->
		{ append_raw ("-->"); }

	void writeComment (const char_t *text, size_t len = (size_t)-1)
		{ beginComment (); writeText (text, len);  endComment (); }

	void endLine()
		{ append_raw ("\r\n"); }

	void writeText (const char_t *text, size_t len = (size_t)-1)
		{ XML::escape (text, len, buffer ()); }

	void indent (int len, char c=' ')
		{ for (; len>0; --len) append_raw (c); }

	string_t& buffer ()  { return *m_buf; }

	inline void append (const unsigned int ch)
		{ TRANSFORM::append_char (*m_buf, ch); }

	inline void append (const char_t *s)
		{ TRANSFORM::append (*m_buf, s, str_len (s)); }

	inline void append (const char_t *s, size_t len)
		{ TRANSFORM::append (*m_buf, s, len); }

private:
	inline void append_raw (char ch)
		{ TRANSFORM::STRING_ADAPTER::append_char (*m_buf, ch); }

	inline void append_raw (const char *s)
		{ for (; *s != 0; ++s) append_raw (*s); }

	string_t * m_buf;
};

////////////////////////////////////////////////////////////////
// Error policy

inline const char * XmlErrorText (int code);

#ifndef LAX_NO_STL

class XmlException : public std::exception
{
public:
	XmlException (const char *s) : m_msg (s) {}

	const char *what () const throw ()
          { return m_msg; } 

protected:
	const char *m_msg;
};

template <typename EXCEPTION>
class XmlUseExceptions
{
public:
static inline int raiseError (int code)
	{ throw EXCEPTION (XmlErrorText (code)); return code; }
};

#endif // LAX_NO_STL

class XmlNoExceptions
{
public:
 static inline int raiseError (int code) { return code; }
};

////////////////////////////////////////////////////////////////
// XML reader

enum XML_ERROR
{
	XML_OK = 0,
	XML_E_SYNTAX     = -1,  // invalid character or attribute syntax
	XML_E_BAD_ENTITY = -2,  // invalid entity type
	XML_E_NO_ELEMENT = -3,  // element expected
	XML_E_BAD_NAME   = -4,  // invalid name (name does not match)
	XML_E_NO_ETAG    = -5,  // ending tag not found
	XML_E_BAD_ETAG   = -6,  // invalid ending tag (etag mismatch)
};

enum XML_ENTITY
{
	XML_END = 1,     // end of input
	XML_TEXT,        // character data
	XML_ELEMENT,     // start of element (<...>)
	XML_ELEMENT_END, // end of element (</...>)
	XML_COMMENT,     // comment (<!--...-->)
	XML_CDATA,       // CDATA section (<![CDATA[...]]>)
	XML_PI,          // processing instruction (<?...?>)
	XML_UNKNOWN      // unknown entity (error ?)
};

//
// XmlReader::readElement() flags
//
enum XML_ELEMENT_FLAGS {
    XML_F_ELT_REQUIRED  = 0x0001, 
    // readElement() throws exception instead of returning false,
    // if element is not present
    
    XML_F_SKIP_BODY     = 0x0002,
    // readElement() skips the element body, including ending tag
    // Usage of this flag is equivalent to the sequence:
    // readElement() ... skipElementBody() ... endElement()

    XML_F_SKIP_ATTRS    = 0x0004,
    // do not extract element attributes

    XML_F_CHECK_NAME    = 0x0008
    // use the third readElement() argument:
    // compares found element name to the given name, throws exception if
    // element name does not match.
};

template <typename TRANSFORM, typename ATTR_MAP_ADAPTER, typename ERROR_POLICY>
class XmlReader
{
public:
	typedef typename TRANSFORM::char_t char_t;
	typedef typename TRANSFORM::strchar_t strchar_t;
	typedef typename TRANSFORM::string_t string_t;
	typedef typename ATTR_MAP_ADAPTER::attrmap_t AttrMap;
	typedef Xml<TRANSFORM> XML;
	typedef ERROR_POLICY ErrorPolicy;

	typedef str_t<char_t> str_t;

	XmlReader ()
		: m_ptr (NULL), m_end (NULL) {}

	XmlReader (const char_t *xml, size_t len)
		: m_ptr (xml), m_end (xml+len) {}

	void init (const char_t *xml, size_t len)
		{ m_ptr = xml; m_end = xml + len; }

	// get parse pointer
	const char_t * ptr () const
		{ return m_ptr; }

	// move parse pointer, skipping characters
	void skip (size_t n)
		{ m_ptr = (n < left ()) ? m_ptr+n : m_end; }

	// get number of characters left in the buffer
	size_t left () const
		{ return m_end-m_ptr; }

	// get next entity (or error)
	int nextEntity ();

	// skips the current entity
	int skipEntity (int ent);

	// skip whitespaces,comments and PIs
	int skipBlanks ();

	//
	// Element structure, filled by the readElement() method
	//
	class Element
	{
	public:
		Element() : m_beg (NULL), m_end (NULL) {}

        // clears name and all attributes
        void clear() { m_beg = NULL; m_end = NULL; m_name.clear(); }

        // returns element name as pointer to the parse buffer
        const str_t& xmlTag () const
			{ return m_name; }

		bool compareXmlTag (const char_t *name) const
		{
			size_t len = m_name.length ();
			return str_eqn (name, m_name.c_str (), len) && name [len] == 0;
		}

		// extracts element name into the string
		int getTagName (string_t& name) const
			{ return assign (name, m_name); }

		// true, if element has body (i.e. starting tag
		// is not terminated with '/>' )
		bool hasBody () const
			{ return m_end > m_beg && *(m_end-2) != '/'; }

		int readAttrs (AttrMap& attrs)
			{ return XmlReader::readElementAttrs (*this, attrs); }

		void init (const char_t *pb, const char_t *pe, const str_t& name)
			{ m_beg = pb; m_end = pe; m_name = name; }

		const char_t * begin () const
			{ return m_beg; }

		const char_t * end () const
			{ return m_end; }

	private:
		const char_t *m_beg;
		const char_t *m_end;
		str_t         m_name;
	};

	// reads element starting tag along with its attributes (and, optionally, body)
	// depending on flags (see XML_ELEMENT_FLAGS) can either scan element
	// starting tag or the entire element, including ending tag.
	//
	// typical code for enumerating multiple XML elements:
	//
	// Element elt;
	// while (readElement (elt, 0) != XML_END)
	// {
	//     // process name and attributes
	//     if (elt.hasBody ()) {
	//         // scan element body (calling readElement() recrsively)
	//     }
	//     // scan the ending tag, if needed
	//     parser.endElement (elt);
	// }
	//
	int readElement (Element& elt, unsigned int flags, const char_t *checkName=NULL);

	int readElement (Element& elt, unsigned int flags, AttrMap& attrs, const char_t *checkName=NULL)
		{ int err = readElement (elt, flags, checkName);
		  if (err != XML_OK) return err;
		  return readElementAttrs (elt, attrs);
		}

	// skip element body (including text and any child elements), returns skipped length
	int skipElementBody (const Element& elt, size_t *psize = 0);

	// reads element body into the 'body' buffer
	int readElementBody (const Element& elt, str_t& body);

	// scan element ending tag if needed
	int endElement (const Element& elt);

	// scan character data, returns false if there are only blanks
	bool readText (str_t& text);

	// scan character data, returns false if there are only blanks
	bool readText (string_t& s, bool skipNonTextEntities = true);

 static const char * getEntityName (XML_ENTITY ent);

 static int assign (string_t& s, const str_t& xs)
	{ TRANSFORM::STRING_ADAPTER::clear (s);
	  TRANSFORM::append (s, xs.c_str (), xs.length ());
	  return XML_OK; }

 static int append (string_t& s, const str_t& xs)
	{ TRANSFORM::append (s, xs.c_str (), xs.length ()); return XML_OK; }

private:
	const char_t *m_ptr;
	const char_t *m_end;

    // detects tag type: ELT, ELT_END, PI, CDATA, COMMENT
	int detectTagType();
	int detectSpecialEntity();
	static size_t matchPrefix (const char_t *s, const char *cmp);
	int skipToTrailer (const char *s, size_t *psize);
	void skipSpaces();
	int skipNonTextEntity (int ent);
	int readSpecialEntity (int ent, str_t& content);
	int readEndingTag (str_t& etagName);
	int readName (str_t& name);
	static int raiseError (int code);

	friend class Element;
	static int readElementAttrs (const Element& elt, AttrMap& attrs);
};

}  // namespace LAX

#include <LAXi.h>

#endif // LAX_h_INCLUDED

