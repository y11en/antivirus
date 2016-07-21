// xml_utils.cpp
//

#define LAX_IMPLEMENTATION
#include <Prague/xml_utils.h>

////////////////////////////////////////////////////////////////
// cXmlAttrMap

int cXmlAttrMap::addAttr (const char_t *name, size_t nlen, const char_t *value, size_t vlen)
{
	Attr attr;
	LAX::Xml<cXmlTransform>::unescape (name, nlen, attr.name);
	LAX::Xml<cXmlTransform>::unescape (value, vlen, attr.value);
	m_attrs.push_back (attr);
	return LAX::XML_OK;
}

const cXmlAttrMap::Attr * cXmlAttrMap::find (const char_t *name) const
{
	size_t cnt = size ();
	for (size_t i=0; i<cnt; ++i) {
		const Attr& attr = m_attrs[i];
		if (cStringAdapter::is_equal (attr.name, name)) {
			return &attr;
		}
	}
	return 0;
}


