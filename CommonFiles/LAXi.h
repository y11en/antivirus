// LAXi.h
//
// LAX: Lightweight API for XML
// (possibly) non-inline methods implementation
//
// (C) 2003 Kaspersky Labs
//

#ifndef LAXi_h_INCLUDED
#define LAXi_h_INCLUDED

namespace LAX
{

#define XML_METHOD(RET_TYPE,NAME) \
template <typename TRANSFORM> \
RET_TYPE Xml <TRANSFORM>::NAME

#define XML_WRITER_METHOD(RET_TYPE,NAME) \
template <typename TRANSFORM> \
RET_TYPE XmlWriter <TRANSFORM>::NAME

#define XML_READER_METHOD(RET_TYPE,NAME) \
template <typename TRANSFORM, typename ATTR_MAP_ADAPTER, typename ERROR_POLICY> \
RET_TYPE XmlReader <TRANSFORM, ATTR_MAP_ADAPTER, ERROR_POLICY>::NAME

#define XML_ATTR_MAP_METHOD(RET_TYPE,NAME) \
template <typename TRANSFORM> \
RET_TYPE XmlAttrMap <TRANSFORM>::NAME

////////////////////////////////////////////////////////////////
// Xml methods

XML_METHOD (bool, escape) (const char_t *s, size_t len, string_t& buf)
{
	if (len == (size_t)-1)
		len = str_len (s);
		
	bool ok = true;

    const char_t *pb = s;
    const char_t *pe = pb + len;

    const char_t *p = pb;

    while (p < pe) {
        unsigned int uc = char_value (*p++);
        const char *ent = NULL;
        switch(uc) {
        case '\"': ent = "&quot;"; break;
        case '\'': ent = "&apos;"; break;
        case '&':  ent = "&amp;"; break;
        case '<':  ent = "&lt;"; break;
        case '>':  ent = "&gt;"; break;
        default:
			// control characters
            if (uc < 0x20 && uc != '\n' && uc != '\r') {
				if (uc <= UTF8_MAX_CHAR) {
				    char nbuf[20];
					nbuf[0] = '&';
					nbuf[1] = '#';
					nbuf[2] = 'x';
					// assert (uc != 0)
					int n = 3;
					unsigned int shift = 20;
					unsigned int mask = 0xF << shift;
					while (shift != 0 && (uc & mask) == 0) { shift -= 4; mask >>= 4; }
					for (;;) {
						unsigned int digit = (uc & mask) >> shift;
						nbuf[n++] = digit + ((digit < 10) ? '0' : 'A'-10);
						if (shift == 0) break;
						shift -= 4;
						mask >>= 4;
					}
					nbuf[n++] = ';';
					nbuf[n] = 0;
					ent = nbuf;
				} else {
					ent = "?";
					ok = false;
				}
            }
        }
        if (ent != NULL) {
            if (pb < p) TRANSFORM::append (buf, pb, p-pb-1);
            for (; *ent != 0; ++ent) TRANSFORM::STRING_ADAPTER::append_char (buf, *ent);
            pb = p;
        }
    }
    if (pb < pe) TRANSFORM::append (buf, pb, pe-pb);
	return ok;
}

XML_METHOD (bool, unescape) (const char_t *s, size_t len, string_t& buf)
{
	if (len == (size_t)-1)
		len = str_len (s);
		
	bool ok = true;

    const char_t *pb = s;
    const char_t *pe = pb + len;

    const char_t *p = pb;

    while (p < pe)
    {
        const char_t *ent = p;
        while (ent < pe && *ent != '&') ++ent;
        if (ent == pe) break;
        const char_t *etail = ++ent;
        while (etail < pe && *etail != ';') ++etail;
        if (etail == pe) break;
        // check entity [ent,etail)
        // null character is disallowed...
        unsigned int uc = 0;

        if (etail - ent < 2) { p = etail+1; continue; }
        if (ent[0] == '#') {
            // numeric entity reference
            if (ent[1] == 'x') {
                for (const char_t *pn=ent+2; pn<etail; ++pn) {
                    unsigned int c = char_value (*pn);
                    if(c >= '0' && c <= '9' ) uc = (uc * 16) + (c-'0');
                     else if( c>='A' && c<='F' ) uc = (uc * 16) + (c-'A'+10);
                     else if( c>='a' && c<='f' ) uc = (uc * 16) + (c-'a'+10);
                     else { uc = 0; ok = false; break; }
                }
            } else {
                for (const char_t *pn=ent+1; pn<etail; ++pn) {
                    unsigned int c = char_value (*pn);
                    if (c >= '0' && c <= '9') uc = (uc * 10) + (c-'0');
                     else { uc = 0; ok = false; break; }
                }
            }
        } else {
            // named entity reference
            switch (etail - ent) {
            case 2: // &lt; &gt;
                if (ent[0] == 'l' && ent[1]=='t') {
                    uc = '<';
                } else if (ent[0] == 'g' && ent[1] == 't') {
                    uc = '>';
                } break;
            case 3: // &amp;
                if (ent[0] == 'a' && ent[1] == 'm' && ent[2] == 'p') {
                    uc = '&';
                } break;
            case 4: // &apos; &quot;
                if (ent[0] == 'a' && ent[1] == 'p' && ent[2] == 'o' && ent[3] == 's') {
                    uc = '\'';
                } else if (ent[0] == 'q' && ent[1] == 'u' && ent[2] == 'o' && ent[3] == 't') {
                    uc = '\"';
                } break;
			default:
				ok = false;
				break;
            }
        }
        p = etail + 1;
        if (uc != 0) {
            if (pb < p) TRANSFORM::append (buf, pb, ent-pb-1);
            TRANSFORM::append_char (buf, uc); // TODO: check for conversion errors?
            pb = p;
        }
    }
    if (pb < pe) TRANSFORM::append (buf, pb, pe-pb);
	return ok;
}

////////////////////////////////////////////////////////////////
// XmlWriter methods

XML_WRITER_METHOD (void, writeAttribute) (const char_t *name, const char_t *value, size_t vlen)
{
	append_raw (' '); append (name);
	append_raw ("=\"");
	XML::escape (value, vlen, buffer ());
	append_raw ('\"');
}

XML_WRITER_METHOD (void, writeProlog) (const char *version, const char *encoding)
{
	if (version == NULL) version = "1.0";
	if (encoding == NULL) encoding = TRANSFORM::DST_ENCODING::NAME ();
	append_raw ("<?xml version=\"");
	append_raw (version);
	append_raw ("\" encoding=\"");
	append_raw (encoding);
	append_raw ("\"?>");
}

////////////////////////////////////////////////////////////////
// XmlReader methods

XML_READER_METHOD (int, nextEntity) ()
{
	if (m_ptr >= m_end) return XML_END;
	unsigned int ch = char_value (*m_ptr);
	if (ch == '<') return detectTagType();
	return XML_TEXT;
}

XML_READER_METHOD (int, skipEntity) (int ent)
{
	if (ent == XML_TEXT) {
		while (m_ptr<m_end && *m_ptr!='<') ++m_ptr;
	} else {
		return skipNonTextEntity (ent);
	}
	return XML_OK;
}

XML_READER_METHOD (void, skipSpaces) ()
{
	for (; m_ptr<m_end; ++m_ptr) {
		if (! XmlIsSpace (char_value (*m_ptr))) break;
	}
}

XML_READER_METHOD (int, skipBlanks) ()
{
	for (;;) {
		skipSpaces ();
		if (m_ptr >= m_end) break;
		unsigned int ch = char_value (*m_ptr);
		if (ch == '<') {
			int ent = detectTagType();
			if (ent < 0) return ent;
			if (ent == XML_COMMENT || ent == XML_PI) {
				int err = skipNonTextEntity (ent);
				if (err < 0) return err;
				continue;
			}
		}
		break;
	}
	return XML_OK;
}

// assumes m_ptr[0] == '<'
XML_READER_METHOD (int, detectTagType) ()
{
	if (m_end - m_ptr < 2) return XML_UNKNOWN;
    if (m_ptr[1] == '/') return XML_ELEMENT_END;
    if (XmlIsNameStartChar (char_value(m_ptr[1]))) return XML_ELEMENT;
    return detectSpecialEntity();
}

// returns 0 if no match, else match length
XML_READER_METHOD (size_t, matchPrefix) (const char_t *s, const char *cmp)
{
	const char_t *p = s;
	for (; *cmp!=0; ++cmp,++p) {
		if (char_value (*p) != *(const unsigned char *)cmp) return 0;
	}
	return p - s;
}

XML_READER_METHOD (int, detectSpecialEntity) ()
{
	if (matchPrefix (m_ptr, "<!--")) return XML_COMMENT;
	if (matchPrefix (m_ptr, "<![CDATA[")) return XML_CDATA;
	if (matchPrefix (m_ptr, "<?")) return XML_PI;
	return XML_UNKNOWN;
}

XML_READER_METHOD (int, readSpecialEntity) (int ent, str_t& content)
{
	content.clear();

	const char *head = NULL;
	const char *tail = NULL;
	const char *xtail = NULL;

	switch (ent) {
	case XML_COMMENT:
		head = "<!--"; tail = "--"; xtail = ">"; break;
	case XML_CDATA:
		head = "<![CDATA["; tail = "]]>"; break;
	case XML_PI:
		head = "<?"; tail = "?>"; break;
	case XML_END:
		return ent;
	default:
		return raiseError (XML_E_BAD_ENTITY);
	}

	skipSpaces();
	const size_t hlen = matchPrefix (m_ptr, head);
	if (! hlen) return raiseError (XML_E_BAD_ENTITY);

	m_ptr += hlen;
	const char_t *beg = m_ptr;

	size_t len = 0;
	int err = skipToTrailer (tail, &len);
	if (err < 0) return err;

	if (xtail != NULL) {
		size_t tmp = 0;
		err = skipToTrailer (xtail, &tmp);
	}

	content.assign (beg, len);
	return err;
}

// returns number of characters before trailer
XML_READER_METHOD (int, skipToTrailer) (const char *s, size_t *psize)
{
	const char_t *beg = m_ptr;

	while (m_ptr < m_end)
	{
		if (*m_ptr++ == s[0])
		{
			if (s[1] == 0)
			{
				*psize = m_ptr - beg;
				return XML_OK;
			}
			size_t len = matchPrefix (m_ptr, s+1);
			if (len != 0 && (size_t) (m_end - m_ptr) >= len)
			{
				m_ptr += len;
				*psize = m_ptr - beg - str_len (s);
				return XML_OK;
			}
		}
	}

	return raiseError (XML_E_SYNTAX);
}

XML_READER_METHOD (int, skipNonTextEntity) (int ent)
{
    if (ent == XML_ELEMENT) {
        Element elt;
        return readElement (elt, XML_F_ELT_REQUIRED|XML_F_SKIP_ATTRS);
    } else if (ent == XML_ELEMENT_END) {
        str_t etagName;
        return readEndingTag (etagName);
    } else {
        str_t content;
        return readSpecialEntity (ent, content);
    }
}

XML_READER_METHOD (int, readEndingTag) (str_t& name)
{
    skipSpaces();

    if (! matchPrefix (m_ptr, "</")) return raiseError (XML_E_NO_ETAG);
    m_ptr += 2;     // skip '</'

    int err = readName (name);
	if (err != XML_OK) return err;

    skipSpaces ();
    if (m_ptr == m_end || *m_ptr != '>') return raiseError (XML_E_SYNTAX);
    ++m_ptr;
	return XML_OK;
}

XML_READER_METHOD (int, readElement) (Element& elt, unsigned int flags, const char_t *checkName)
{
	elt.clear ();

    for (;;) {
        skipSpaces ();
        int ent = nextEntity();
        if (ent == XML_ELEMENT) break;
        if ((ent == XML_COMMENT || ent == XML_PI)) {
            skipNonTextEntity (ent);
        } else {
			if (ent < 0) return ent;
            if((flags & XML_F_ELT_REQUIRED) != 0) return raiseError (XML_E_NO_ELEMENT);
            return XML_END;
        }
    }

	const char_t *beg = m_ptr++; // skip "<"

    // scan element name
    str_t name;
    int err = readName (name);
	if (err != XML_OK) return err;

    if (flags & XML_F_CHECK_NAME) {
        if (! str_eqn (checkName, name.c_str(), name.length())) {
			return raiseError (XML_E_BAD_NAME);
        }
    }

	for (;;) {
		if (m_ptr >= m_end) return raiseError (XML_E_NO_ETAG);
		if (*m_ptr++ == '>') break;
	}

	elt.init (beg, m_ptr, name);

    // scan contents ?
    if (flags & XML_F_SKIP_BODY) {
        err = skipElementBody (elt);
		if (err != XML_OK) return err;
        err = endElement (elt);
    }

    return err;
}

XML_READER_METHOD (int, skipElementBody) (const Element& elt, size_t *psize)
{
    if (psize != 0)
        *psize = 0;

    if (! elt.hasBody ())
        return XML_OK;

    const char_t *beg = m_ptr;

    for (;;)
    {
        int ent = nextEntity();

        if (ent == XML_ELEMENT)
        {
            Element tmp;
            readElement (tmp, XML_F_ELT_REQUIRED | XML_F_SKIP_BODY | XML_F_SKIP_ATTRS, NULL);
        }
        else if (ent == XML_ELEMENT_END)
        {
            break;
        }
        else if (ent == XML_END)
        {
            return XML_E_NO_ELEMENT;
        }
        else
        {
	    if (ent < 0 || ent == XML_UNKNOWN)
		return raiseError (XML_E_NO_ETAG);
            skipEntity (ent);
        }
    }

    if (psize != 0)
        *psize = m_ptr-beg;

    return XML_OK;
}

XML_READER_METHOD (int, readElementBody) (const Element& elt, str_t& body)
{
    body.m_str = m_ptr;
    return skipElementBody (elt, &body.m_len);
}

XML_READER_METHOD (int, endElement) (const Element& elt)
{
	if (! elt.hasBody ()) return XML_OK;

	int err = skipElementBody (elt);
	if (err != XML_OK) return err;

	str_t etag;
	err = readEndingTag (etag);
	if (err != XML_OK) return err;

	if (! (etag == elt.xmlTag ())) {
		return raiseError (XML_E_NO_ETAG);
	}
	return XML_OK;
}

XML_READER_METHOD (int, readName) (str_t& name)
{
	const char_t *beg = m_ptr;
	if (beg == m_end || ! XmlIsNameStartChar (*beg))
		return raiseError (XML_E_SYNTAX);

    for (++m_ptr; m_ptr<m_end && XmlIsNameChar (char_value (*m_ptr)); ++m_ptr);
    name.assign (beg, m_ptr - beg);
	return XML_OK;
}

XML_READER_METHOD (bool, readText) (str_t& text)
{
	text.clear ();
	const char_t *beg = m_ptr;
	bool nb = false;

	for (; m_ptr<m_end; ++m_ptr) {
		const unsigned int uc = *((const char_t *)m_ptr);
		if (uc == '<') break;
		if (!nb && !XmlIsSpace (uc)) nb = true;
	}
	text.assign (beg, m_ptr - beg);
	return nb;
}

XML_READER_METHOD (bool, readText) (string_t& s, bool skipNonTextEntities)
{
	TRANSFORM::STRING_ADAPTER::clear (s);
	bool nb = false;

	for (;;) {
		str_t xs;
		if (readText (xs)) nb = true;
		XML::unescape (xs.c_str(), xs.length(), s);

		if (! skipNonTextEntities) break;

		int ent = nextEntity ();

		if (ent == XML_CDATA)
		{
			readSpecialEntity (XML_CDATA, xs);
			if (! nb) {
				const char_t *s = xs.c_str ();
				for (size_t i=0; i < xs.length (); ++i)
				{
					if (!XmlIsSpace (*s)) { nb = true; break; }
				}
			}
			append (s, xs);
			continue;
		}
		else if (! (ent == XML_COMMENT || ent == XML_PI))
		{
			break;
		}

		if (skipNonTextEntity (ent) < 0) break;
	}
	return nb;
}

XML_READER_METHOD (int, readElementAttrs) (const Element& elt, AttrMap& attrs)
{
	ATTR_MAP_ADAPTER::clearAttrs (attrs);

	const str_t& tagname = elt.xmlTag ();
	const char_t *p = tagname.c_str() + tagname.length();
	const char_t *pe = elt.end () - 1;

    while (p < pe) {
		const char_t *beg = p;

		// one or more spaces are required here
		while (XmlIsSpace (*p)) ++p;

		if (! XmlIsNameStartChar (*p)) {
			if (*p == '/' || *p == '>' || *p == '?') break;
			return raiseError (XML_E_SYNTAX);
		}

		// whitespace required before name
		if (p == beg) return raiseError (XML_E_SYNTAX);

		// scan attribute name
		const char_t *nb = p++; // name start
		while (XmlIsNameChar (*p)) ++p;
		
		const size_t nl = p-nb; // name length

		// scan '=', skipping whitespace
		while (XmlIsSpace (*p)) ++p;
		if (*p++ != '=') return raiseError (XML_E_SYNTAX);
		while (XmlIsSpace (*p)) ++p;

		// scan quoted string
		const char_t q = *p++;
		if (! (q == '\'' || q == '\"')) return raiseError (XML_E_SYNTAX);

		const char_t *vb = p;   // value start
		for (; *p!=q; ++p) {
			if (p >= pe) return raiseError (XML_E_SYNTAX);
		}
		const size_t vl = p++ - vb; // value length

		int ec = ATTR_MAP_ADAPTER::addAttr (attrs, nb, nl, vb, vl);
		if (ec < 0) return ec;
    }
	return XML_OK;
}

XML_READER_METHOD (int, raiseError) (int code)
{
	return ERROR_POLICY::raiseError (code);
}

XML_READER_METHOD (const char *, getEntityName) (XML_ENTITY ent)
{
	switch (ent) {
	case XML_END:     return "END";
    case XML_TEXT:    return "TEXT";
    case XML_ELEMENT: return "ELEMENT";
    case XML_ELEMENT_END: return "ELEMENT_END";
    case XML_COMMENT: return "COMMENT";
    case XML_CDATA:   return "CDATA";
    case XML_PI:      return "PI";
	default:          return "???";
	}
}

////////////////////////////////////////////////////////////////
// XmlAttrMap<> methods

#ifndef LAX_NO_STL

XML_ATTR_MAP_METHOD (int, addAttr) (const char_t *name, size_t nlen, const char_t *value, size_t vlen)
{
	const size_t idx = m_attrs.size(); m_attrs.resize (idx+1);
	Attr& attr = m_attrs[idx];
	Xml<TRANSFORM>::unescape (name, nlen, attr.name);
	Xml<TRANSFORM>::unescape (value, vlen, attr.value);
	return XML_OK;
}

XML_ATTR_MAP_METHOD (const typename XmlAttrMap<TRANSFORM>::strchar_t *, find) (const strchar_t *name, const strchar_t *defval) const
{
	typename AttrBag::const_iterator iter = m_attrs.begin ();
	typename AttrBag::const_iterator iend = m_attrs.end ();
	for (; iter != iend; ++iter) {
		const Attr& attr = *iter;
		if (STRING_ADAPTER::is_equal (attr.name, name))
		{
			return STRING_ADAPTER::c_str (attr.value);
		}
	}
	return defval;
}

#endif // LAX_NO_STL


// #if defined(LAX_IMPLEMENTATION) || !defined(LAX_NO_IMPLEMENTATION)
// Commented out 25.04.2005 by Konstantin Lebedev
// XmlErrorText declared in LAX.h as "inline const char * XmlErrorText (int code);"

////////////////////////////////////////////////////////////////
// Error text

const char * XmlErrorText (int code)
{
	switch (code) {
	case XML_OK:           return "XML:OK";
	case XML_E_SYNTAX:     return "XML:invalid syntax";
	case XML_E_BAD_ENTITY: return "XML:invalid entity";
	case XML_E_NO_ELEMENT: return "XML:element expected";
	case XML_E_BAD_NAME:   return "XML:invalid tag name";
	case XML_E_NO_ETAG:    return "XML:ending tag not found";
	case XML_E_BAD_ETAG:   return "XML:invalid ending tag";
	}
	return "XML:unknown error";
}

#if defined(LAX_IMPLEMENTATION) || !defined(LAX_NO_IMPLEMENTATION)


////////////////////////////////////////////////////////////////
// Unicode character table
// nametab.h from EXPAT (C) James Clark

const unsigned int XmlNamingBitmap[] = {
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
0x00000000, 0x04000000, 0x87FFFFFE, 0x07FFFFFE,
0x00000000, 0x00000000, 0xFF7FFFFF, 0xFF7FFFFF,
0xFFFFFFFF, 0x7FF3FFFF, 0xFFFFFDFE, 0x7FFFFFFF,
0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFE00F, 0xFC31FFFF,
0x00FFFFFF, 0x00000000, 0xFFFF0000, 0xFFFFFFFF,
0xFFFFFFFF, 0xF80001FF, 0x00000003, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0xFFFFD740, 0xFFFFFFFB, 0x547F7FFF, 0x000FFFFD,
0xFFFFDFFE, 0xFFFFFFFF, 0xDFFEFFFF, 0xFFFFFFFF,
0xFFFF0003, 0xFFFFFFFF, 0xFFFF199F, 0x033FCFFF,
0x00000000, 0xFFFE0000, 0x027FFFFF, 0xFFFFFFFE,
0x0000007F, 0x00000000, 0xFFFF0000, 0x000707FF,
0x00000000, 0x07FFFFFE, 0x000007FE, 0xFFFE0000,
0xFFFFFFFF, 0x7CFFFFFF, 0x002F7FFF, 0x00000060,
0xFFFFFFE0, 0x23FFFFFF, 0xFF000000, 0x00000003,
0xFFF99FE0, 0x03C5FDFF, 0xB0000000, 0x00030003,
0xFFF987E0, 0x036DFDFF, 0x5E000000, 0x001C0000,
0xFFFBAFE0, 0x23EDFDFF, 0x00000000, 0x00000001,
0xFFF99FE0, 0x23CDFDFF, 0xB0000000, 0x00000003,
0xD63DC7E0, 0x03BFC718, 0x00000000, 0x00000000,
0xFFFDDFE0, 0x03EFFDFF, 0x00000000, 0x00000003,
0xFFFDDFE0, 0x03EFFDFF, 0x40000000, 0x00000003,
0xFFFDDFE0, 0x03FFFDFF, 0x00000000, 0x00000003,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0xFFFFFFFE, 0x000D7FFF, 0x0000003F, 0x00000000,
0xFEF02596, 0x200D6CAE, 0x0000001F, 0x00000000,
0x00000000, 0x00000000, 0xFFFFFEFF, 0x000003FF,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0xFFFFFFFF, 0xFFFF003F, 0x007FFFFF,
0x0007DAED, 0x50000000, 0x82315001, 0x002C62AB,
0x40000000, 0xF580C900, 0x00000007, 0x02010800,
0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
0x0FFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x03FFFFFF,
0x3F3FFFFF, 0xFFFFFFFF, 0xAAFF3F3F, 0x3FFFFFFF,
0xFFFFFFFF, 0x5FDFFFFF, 0x0FCF1FDC, 0x1FDC1FFF,
0x00000000, 0x00004C40, 0x00000000, 0x00000000,
0x00000007, 0x00000000, 0x00000000, 0x00000000,
0x00000080, 0x000003FE, 0xFFFFFFFE, 0xFFFFFFFF,
0x001FFFFF, 0xFFFFFFFE, 0xFFFFFFFF, 0x07FFFFFF,
0xFFFFFFE0, 0x00001FFF, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
0xFFFFFFFF, 0x0000003F, 0x00000000, 0x00000000,
0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
0xFFFFFFFF, 0x0000000F, 0x00000000, 0x00000000,
0x00000000, 0x07FF6000, 0x87FFFFFE, 0x07FFFFFE,
0x00000000, 0x00800000, 0xFF7FFFFF, 0xFF7FFFFF,
0x00FFFFFF, 0x00000000, 0xFFFF0000, 0xFFFFFFFF,
0xFFFFFFFF, 0xF80001FF, 0x00030003, 0x00000000,
0xFFFFFFFF, 0xFFFFFFFF, 0x0000003F, 0x00000003,
0xFFFFD7C0, 0xFFFFFFFB, 0x547F7FFF, 0x000FFFFD,
0xFFFFDFFE, 0xFFFFFFFF, 0xDFFEFFFF, 0xFFFFFFFF,
0xFFFF007B, 0xFFFFFFFF, 0xFFFF199F, 0x033FCFFF,
0x00000000, 0xFFFE0000, 0x027FFFFF, 0xFFFFFFFE,
0xFFFE007F, 0xBBFFFFFB, 0xFFFF0016, 0x000707FF,
0x00000000, 0x07FFFFFE, 0x0007FFFF, 0xFFFF03FF,
0xFFFFFFFF, 0x7CFFFFFF, 0xFFEF7FFF, 0x03FF3DFF,
0xFFFFFFEE, 0xF3FFFFFF, 0xFF1E3FFF, 0x0000FFCF,
0xFFF99FEE, 0xD3C5FDFF, 0xB080399F, 0x0003FFCF,
0xFFF987E4, 0xD36DFDFF, 0x5E003987, 0x001FFFC0,
0xFFFBAFEE, 0xF3EDFDFF, 0x00003BBF, 0x0000FFC1,
0xFFF99FEE, 0xF3CDFDFF, 0xB0C0398F, 0x0000FFC3,
0xD63DC7EC, 0xC3BFC718, 0x00803DC7, 0x0000FF80,
0xFFFDDFEE, 0xC3EFFDFF, 0x00603DDF, 0x0000FFC3,
0xFFFDDFEC, 0xC3EFFDFF, 0x40603DDF, 0x0000FFC3,
0xFFFDDFEC, 0xC3FFFDFF, 0x00803DCF, 0x0000FFC3,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0xFFFFFFFE, 0x07FF7FFF, 0x03FF7FFF, 0x00000000,
0xFEF02596, 0x3BFF6CAE, 0x03FF3F5F, 0x00000000,
0x03000000, 0xC2A003FF, 0xFFFFFEFF, 0xFFFE03FF,
0xFEBF0FDF, 0x02FE3FFF, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x1FFF0000, 0x00000002,
0x000000A0, 0x003EFFFE, 0xFFFFFFFE, 0xFFFFFFFF,
0x661FFFFF, 0xFFFFFFFE, 0xFFFFFFFF, 0x77FFFFFF,
};
const unsigned char XmlNameStartPages[] = {
0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00,
0x00, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
0x10, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x13,
0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x15, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x17,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x18,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const unsigned char XmlNamePages[] = {
0x19, 0x03, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x00,
0x00, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
0x10, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x13,
0x26, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x27, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x17,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x18,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

#endif // defined(LAX_IMPLEMENTATION) || !defined(LAX_NO_IMPLEMENTATION)

} // namespace LAX


#endif // LAXi_h_INCLUDED

