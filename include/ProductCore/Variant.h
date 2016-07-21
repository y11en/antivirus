// Variant.h: interface for the cVariant class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include <vector>
#include <Prague/pr_cpp.h>
#include <Prague/pr_serializable.h>


typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
typedef char char_t;

struct cVectorData
{
	cVectorData() : m_v(NULL), m_type(tid_VOID), m_flags(0) {}
	tMemChunk * m_v;
	tTYPE_ID    m_type;
	tUINT       m_flags;
};

void utf16_to_utf8(const wchar_t *utf16, size_t utf16cc, tString &utf8);
void utf8_to_utf16(const char *utf8, size_t utf8cc, cStringObj &utf16);

int64_t str_to_i64(const char *string);
void    i64_to_str(int64_t val, tString &str);

//////////////////////////////////////////////////////////////////////
// cVariant

class cVariant
{
public:
	enum eType { vtVoid, vtString, vtVector, vtInt, vtSer, vtCtl, vtIco };

	cVariant()                      : m_type(vtVoid) {}
	cVariant(bool val)              : m_type(vtVoid) { Init((int64_t)val); }
	cVariant(int64_t val)           : m_type(vtVoid) { Init(val); }
	cVariant(tDWORD val)            : m_type(vtVoid) { Init((int64_t)val); }
	cVariant(tQWORD val)            : m_type(vtVoid) { Init((int64_t)val); }
	cVariant(const char_t *val)     : m_type(vtVoid) { Init(val); }
	cVariant(const wchar_t *val)    : m_type(vtVoid) { Init(val); }
	cVariant(const tString &val)    : m_type(vtVoid) { Init(val); }
	cVariant(const cVariant &other) : m_type(vtVoid) { Init(other); }
	~cVariant()                                      { Clear(); }

	cVariant &operator =(bool val)               { Init((int64_t)val); return *this; }
	cVariant &operator =(int64_t val)            { Init(val); return *this; }
	cVariant &operator =(tDWORD val)             { Init((int64_t)val); return *this; }
	cVariant &operator =(tQWORD val)             { Init((int64_t)val); return *this; }
	cVariant &operator =(const char_t *val)      { Init(val); return *this; }
	cVariant &operator =(const wchar_t *val)     { Init(val); return *this; }
	cVariant &operator =(const tString &val)     { Init(val); return *this; }
	cVariant &operator =(const cVariant &val)    { Init(val); return *this; }

	cVariant &operator +=(const cVariant &other);
	cVariant &operator -=(const cVariant &other);
	cVariant &operator *=(const cVariant &other);
	cVariant &operator /=(const cVariant &other);
	cVariant &operator %=(const cVariant &other);
	cVariant &operator &=(const cVariant &other);
	cVariant &operator ^=(const cVariant &other);
	cVariant &operator |=(const cVariant &other);

	cVariant &operator <<=(const cVariant &other);
	cVariant &operator >>=(const cVariant &other);

	bool operator ==(const cVariant &other) const;
	bool operator !=(const cVariant &other) const;
	bool operator >=(const cVariant &other) const;
	bool operator <=(const cVariant &other) const;
	bool operator >(const cVariant &other) const;
	bool operator <(const cVariant &other) const;

	operator bool() const;
	bool operator !() const { return !operator bool(); }

	void Init(int64_t val)            { Clear(); m_type = vtInt; m_int = val; }
	void Init(const char_t *val)      { initStr(); strRef() = val; }
	void Init(const wchar_t *val, size_t len = -1) { initStr(); utf16_to_utf8(val, len, strRef()); }
	void Init(const tString &val)     { initStr(); strRef() = val; }
	void Init(const cVectorData &val) { initVect(); vectRef() = val; }
	void Init(const cSer *val)        { Clear(); m_type = vtSer; m_int = (int64_t)val; }
	void Init(const CItemBase *val)   { Clear(); m_type = vtCtl; m_int = (int64_t)val; }
	void Init(const CIcon *val)       { Clear(); m_type = vtIco; m_int = (int64_t)val; }
	void Init(const cVariant &val);
	void Clear();
	void MakeInt();
	tString & MakeString();

	int  Compare(const cVariant &val) const;

	eType      Type() const        { return m_type; }
	int64_t    ToInt() const;
	tQWORD     ToQWORD() const     { return (tQWORD)ToInt(); }
	tDWORD     ToDWORD() const     { return (tDWORD)ToInt(); }
	void       ToString(tString &res) const;
	tString    ToString() const    { tString res; ToString(res); return res; }
	void       ToStringObj(cStringObj &res) const;
	cStringObj ToStringObj() const { cStringObj res; ToStringObj(res); return res; }
	
	const char *        c_str() const       { return m_type == vtString ? strRef().c_str() : NULL; }
	const cSer *        c_ser(tUINT nSerId = cSer::eIID) const { if( m_type != vtSer ) return NULL; cSer * pSer = (cSer *)m_int; if( !pSer ) return NULL; if( nSerId != cSer::eIID && !pSer->isBasedOn(nSerId) ) return NULL; return pSer; }
	const cVectorData * c_vector() const    { return m_type == vtVector ? &vectRef() : NULL; }
	      CItemBase *   c_ctl() const       { return m_type == vtCtl ? (CItemBase *)m_int : NULL; }
		  CIcon *       c_ico() const       { return m_type == vtIco ? (CIcon  *)m_int : NULL; }

	tString &     strRef() const  { return *(tString *)m_string; }
	cVectorData & vectRef() const { return *(cVectorData *)m_vector; }

private:
	void    initStr()     { if( m_type != vtString ) { Clear(); new (m_string) tString; m_type = vtString; } }
	void    destroyStr()  { if( m_type == vtString ) { strRef().~tString(); m_type = vtVoid; } }
	void    initVect()    { if( m_type != vtString ) { Clear(); new (m_vector) cVectorData; m_type = vtVector; } }
	void    destroyVect() { if( m_type == vtString ) { vectRef().~cVectorData(); m_type = vtVoid; } }

private:
	eType       m_type;
	
	union
	{
		int64_t     m_int;
		char        m_string[sizeof(tString)];
		char        m_vector[sizeof(cVectorData)];
	};
};

typedef cVariant cVar;
typedef std::vector<cVariant &> cVariantRefs;
typedef std::vector<cVariant> cVariants;

#define variant_strref(var) (var.Type() == cVariant::vtString ? var.strRef() : var.ToString())


inline void cVariant::Clear()
{
	switch(m_type)
	{
	case vtString: destroyStr(); break;
	case vtVector: destroyVect(); break;
	}
	m_type = vtVoid;
}

inline void cVariant::Init(const cVariant &val)
{
	if( val.m_type == vtString ) { Init(val.strRef()); return; }
	if( val.m_type == vtVector ) { Init(val.vectRef()); return; }

	Clear();
	m_type = val.m_type;
	switch(m_type)
	{
	case vtIco:
	case vtSer:
	case vtCtl:
	case vtInt: m_int = val.m_int; break;
	}
}

inline cVariant &cVariant::operator +=(const cVariant &other)
{
	switch(m_type)
	{
	case vtVoid:
		Init(other);
		break;
	case vtInt:
		if( other.m_type == vtString )
			MakeString(), strRef() += other.strRef();
		else
			m_int += other.ToInt();
		break;
	case vtString:
		strRef() += other.m_type == vtString ? other.strRef() : other.ToString();
		break;
	}
	return *this;
}

inline cVariant &cVariant::operator -=(const cVariant &other)
{
	MakeInt();
	m_int -= other.ToInt();
	return *this;
}

inline cVariant &cVariant::operator *=(const cVariant &other)
{
	MakeInt();
	m_int *= other.ToInt();
	return *this;
}

inline cVariant &cVariant::operator /=(const cVariant &other)
{
	MakeInt();
	if( int64_t dev = other.ToInt() )
		m_int /= dev;
	return *this;
}

inline cVariant &cVariant::operator %=(const cVariant &other)
{
	MakeInt();
	if( int64_t dev = other.ToInt() )
		m_int %= dev;
	return *this;
}

inline cVariant &cVariant::operator &=(const cVariant &other)
{
	MakeInt();
	m_int &= other.ToInt();
	return *this;
}

inline cVariant &cVariant::operator ^=(const cVariant &other)
{
	MakeInt();
	m_int ^= other.ToInt();
	return *this;
}

inline cVariant &cVariant::operator |=(const cVariant &other)
{
	MakeInt();
	m_int |= other.ToInt();
	return *this;
}

inline cVariant &cVariant::operator <<=(const cVariant &other)
{
	MakeInt();
	m_int <<= other.ToInt();
	return *this;
}

inline cVariant &cVariant::operator >>=(const cVariant &other)
{
	MakeInt();
	m_int >>= other.ToInt();
	return *this;
}

inline bool cVariant::operator ==(const cVariant &other) const
{
	if( m_type != other.m_type )
		return false;

	switch(m_type)
	{
	case vtInt:    return m_int == other.m_int;
	case vtString: return strRef() == other.strRef();
	}
	return true;
}

inline bool cVariant::operator !=(const cVariant &other) const
{
	return !(*this == other);
}

inline bool cVariant::operator >=(const cVariant &other) const
{
	return !(*this < other);
}

inline bool cVariant::operator <=(const cVariant &other) const
{
	return !(*this > other);
}

inline bool cVariant::operator >(const cVariant &other) const
{
	switch(m_type)
	{
	case vtInt:
		return m_int > other.ToInt();
	case vtString:
		if( other.m_type == vtString )
			return strRef() > other.strRef();
	}
	return false;
}

inline bool cVariant::operator <(const cVariant &other) const
{
	switch(m_type)
	{
	case vtInt:
		return m_int < other.ToInt();
	case vtString:
		if( other.m_type == vtString )
			return strRef() < other.strRef();
	}
	return false;
}

inline int cVariant::Compare(const cVariant &other) const
{
	switch(m_type)
	{
	case vtInt:
		{
			int64_t val = other.ToInt();
			return m_int == val ? 0 : m_int < val ? -1 : 1;
		}
	case vtString:
		if( other.m_type == vtString )
			return strRef().compare(other.strRef());
	}
	return 0;
}

inline cVariant::operator bool() const
{
	switch(m_type)
	{
	case vtIco:
	case vtCtl:
	case vtSer:
	case vtInt:    return !!m_int;
	case vtString: return !strRef().empty();
	}
	return false;
}

inline int64_t cVariant::ToInt() const
{
	switch(m_type)
	{
	case vtInt:    return m_int;
	case vtString: return str_to_i64(strRef().c_str());
	}
	return 0;
}

inline void cVariant::ToString(tString &res) const
{
	res.clear();
	switch(m_type)
	{
	case vtString:
		res = strRef();
		return;
	case vtInt:
		i64_to_str(m_int, res);
		return;
	}
}

inline void cVariant::ToStringObj(cStringObj &res) const
{
	res.clear();
	switch(m_type)
	{
	case vtString:
		utf8_to_utf16(strRef().c_str(), strRef().size(), res);
		return;
	case vtInt:
		res.format(cCP_UNICODE, "%I64d", m_int);
		return;
	}
}

inline void cVariant::MakeInt()
{
	switch(m_type)
	{
	case vtVoid:   m_int = 0; break;
	case vtString: Init(ToInt()); break;
	}
	m_type = vtInt;
}

inline tString & cVariant::MakeString()
{
	if( m_type == vtInt )
	{
		int64_t v = m_int;
		initStr();
		i64_to_str(v, strRef());
	}
	else
		initStr();

	return strRef();
}





//////////////////////////////////////////////////////////////////////////
// helper functions

inline int64_t str_to_i64(const char *string)
{
	tQWORD val = 0; sscanf(string, PRINTF_LONGLONG, &val);
	return val;
}

inline int i64_to_str(int64_t val, char * dstBuf, size_t size)
{
	return _snprintf(dstBuf, size, PRINTF_LONGLONG, val);
}

inline void i64_to_str(int64_t val, tString &str)
{
	str.resize(22);
	i64_to_str(val, const_cast<char *>(str.c_str()), str.capacity());
	str.resize(strlen(str.c_str()));
}

#ifdef WIN32

inline void utf16_to_utf8(const wchar_t *utf16, size_t utf16cc, tString &utf8)
{
	utf8.clear();

	if( utf16cc == (size_t)-1 )
		utf16cc = wcslen(utf16);

	if( !utf16cc )
		return;

	size_t utf8cc = WideCharToMultiByte(CP_UTF8, 0, utf16, utf16cc, NULL, 0, NULL, NULL);

	utf8.resize(utf8cc);

	WideCharToMultiByte(CP_UTF8, 0, utf16, utf16cc, const_cast<char *>(utf8.c_str()), utf8cc, NULL, NULL);
}

inline void utf8_to_utf16(const char *utf8, size_t utf8cc, cStringObj &utf16)
{
	utf16.clear();

	if( utf8cc == (size_t)-1 )
		utf8cc = strlen(utf8);

	if( !utf8cc )
		return;

	size_t utf16cc = MultiByteToWideChar(CP_UTF8, 0, utf8, utf8cc, NULL, 0);

	utf16.resize(utf16cc);

	MultiByteToWideChar(CP_UTF8, 0, utf8, utf8cc, const_cast<wchar_t *>(utf16.data()), utf16cc);
}

#else // WIN32

#include <QtCore>

inline void utf16_to_utf8(const wchar_t *utf16, size_t utf16cc, tString &utf8)
{
	utf8 = (const char *)QString::fromUtf16((const ushort *)utf16, utf16cc).toUtf8();
}

inline void utf8_to_utf16(const char *utf8, size_t utf8cc, cStringObj &utf16)
{
	utf16 = (const tWCHAR*)QString::fromUtf8(utf8, utf8cc).utf16();
}

#endif // WIN32
