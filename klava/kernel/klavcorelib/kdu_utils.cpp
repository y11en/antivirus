// kdu_utils.cpp
//

#include <memory.h>

#include <klava/kdu_utils.h>
#include <kl_byteswap.h>
#include <klava/kdb_ids.h>

#define ALWAYS_USE_KDU2 0

////////////////////////////////////////////////////////////////
// KDUBlockReader

KDUErr KDUBlockReader::getNextBlock (KDUBlockInfo& info)
{
	if (m_pos >= m_size)
		return KDU_E_EOF;

	if (m_size - m_pos < sizeof (KDU_BLOCK_HEADER_V1))
		return KDU_E_BAD_SIZE;

	const KDU_BLOCK_HEADER_V1 *hdr1 = (const KDU_BLOCK_HEADER_V1 *)(&m_data [m_pos]);

	if (! KDU_BLOCK_CHECK_MAGIC(hdr1))
		return KDU_E_BAD_MAGIC;

	// current scan position
	uint32_t pos = m_pos;
	info.start = &m_data [pos];

	if (KDU_BLOCK_MAGIC_IS_V1 (hdr1))
	{
		info.type = KDU_BLOCK_TYPE_V1 (hdr1);
		info.size = KDU_BLOCK_SIZE_V1 (hdr1);

		pos += sizeof (KDU_BLOCK_HEADER_V1);

		if (KDU_BLOCK_IS_SHLEN_EXTENDED (hdr1))
		{
			// KDU1 with 'extended' format
			if (m_size - pos < sizeof (uint32_t))
				return KDU_E_BAD_SIZE;

			uint32_t shlen_be = * (const uint32_t *)(&m_data [pos]);
			info.subhdr_len = kl_bswap_32_be (shlen_be);
			pos += sizeof (uint32_t);
		}
		else
		{
			// KDU1 with 'short' format
			info.subhdr_len = KDU_BLOCK_SHLEN_V1 (hdr1) * 4; // shlen is specified in words
		}
	}
	else if (KDU_BLOCK_MAGIC_IS_V2 (hdr1))
	{
		// V2 block format
		const KDU_BLOCK_HEADER_V2 *hdr2 = (const KDU_BLOCK_HEADER_V2 *)(&m_data [m_pos]);
		info.type = KDU_BLOCK_TYPE_V2 (hdr2);
		info.size = KDU_BLOCK_SIZE_V2 (hdr2);
		info.subhdr_len = KDU_BLOCK_SHLEN_V2 (hdr2);

		pos += sizeof (KDU_BLOCK_HEADER_V2);
	}
	else
	{
		return KDU_E_BAD_MAGIC;
	}

	info.subhdr = &m_data [pos];
	pos += KDU_BLOCK_ALIGN (info.subhdr_len);

	info.data = (const uint8_t *)(&m_data [pos]);

	if (pos > m_size || m_size - pos < info.size)
		return KDU_E_BAD_SIZE;

	m_pos = KDU_BLOCK_ALIGN (pos + info.size);

	return KDU_E_OK;
}

////////////////////////////////////////////////////////////////
// KDUSectionReader

KDUErr KDUSectionReader::getNextSection (KDUSectionInfo& info)
{
	if (m_pos >= m_size) return KDU_E_EOF;

	if (m_size - m_pos < sizeof (KDU_SECTION_HEADER)) return KDU_E_BAD_SIZE;

	const KDU_SECTION_HEADER *hdr = (const KDU_SECTION_HEADER *)(&m_data[m_pos]);

	info.start = &m_data[m_pos];
	info.sectionType = 0;
	info.fragmentID = 0;

	uint8_t stype = KDU_SECT_TYPE_UNKNOWN;
	uint8_t sfrag = KDU_SECT_FRAG_UNKNOWN;

	if (KDU_SECT_IS_V2 (hdr))
	{
		info.raw  = KDU_SECT_RAW_V2 (hdr);
		info.size = KDU_SECT_SIZE_V2 (hdr);

		if (! KDU_SECT_HAS_FRAG_V2 (hdr))
		{
			sfrag = KDU_SECT_FRAG_DEFAULT;
		}
	}
	else
	{
		info.raw  = KDU_SECT_RAW_V1 (hdr);
		info.size = KDU_SECT_SIZE_V1 (hdr);

		stype = KDU_SECT_TYPE_V1 (hdr);
		sfrag = KDU_SECT_FRAG_V1 (hdr);
	}

	unsigned int extra_size = 0; // size of optional fields in section header
	if (stype == KDU_SECT_TYPE_UNKNOWN) extra_size += sizeof (uint32_t);
	if (sfrag == KDU_SECT_FRAG_UNKNOWN) extra_size += sizeof (uint32_t);

	const uint32_t total_size = sizeof (KDU_SECTION_HEADER) + extra_size + info.size;
	if (total_size < info.size // integer overflow ?
	 || total_size > (m_size - m_pos))
		return KDU_E_BAD_SIZE;

	const uint32_t *pdata = (const uint32_t *)(&m_data[m_pos + sizeof (KDU_SECTION_HEADER)]);
	if (stype == KDU_SECT_TYPE_UNKNOWN)
	{
		info.sectionType = kl_bswap_32_be (*pdata); ++pdata;
	}
	else
	{
		if (! KDUMapWKSectionType_V1 (stype, info.sectionType))
			return KDU_E_BAD_ID;
	}
	if (sfrag == KDU_SECT_FRAG_UNKNOWN)
	{
		info.fragmentID = kl_bswap_32_be (*pdata); ++pdata;
	}
	else
	{
		if (! KDUMapWKFragmentID_V1 (sfrag, info.fragmentID))
			return KDU_E_BAD_ID;
	}
	info.data = (const uint8_t *)pdata;

	m_pos += KDU_SECT_ALIGN (total_size);

	return KDU_E_OK;
}

////////////////////////////////////////////////////////////////
// KDUAttr, KDUAttrReader

inline uint32_t kdu_read_value (const uint8_t *p, unsigned int nb)
{
	uint32_t val = ((*p & 0x80) == 0) ? 0 : 0xFFFFFFFF;
	for (; nb != 0; --nb) val = (val << 8) | (*p++);
	return val;
}

inline unsigned int kdu_value_size (uint32_t val)
{
	if ((val & 0x80000000) != 0) val = ~val;
	if ((val & 0xFFFF8000) == 0) {
		return ((val & 0xFFFFFF80) == 0) ? 1 : 2;
	} else {
		return ((val & 0xFF800000) == 0) ? 3 : 4;
	}
}

uint32_t KDUReadValue (const uint8_t *p, unsigned int nb)
{
	return kdu_read_value (p, nb);
}

unsigned int KDUValueSize (uint32_t val)
{
	return kdu_value_size (val);
}

void KDUWriteValueNB (uint8_t *buf, uint32_t val, unsigned int nb)
{
	for (unsigned int i=0; i < nb; ++i) {
		buf[i] = (uint8_t)(val >> ((nb - i - 1) * 8));
	}
}

unsigned int KDUWriteValue (uint8_t *buf, uint32_t val)
{
	const unsigned int nb = kdu_value_size (val);
	for (unsigned int i=0; i < nb; ++i) {
		buf[i] = (uint8_t)(val >> ((nb - i - 1) * 8));
	}
	return nb;
}

uint32_t KDUAttrReader::skipBlanks ()
{
	uint32_t org = m_pos;
	while (m_pos < m_size && m_data[m_pos] == 0) ++m_pos;
	return m_pos - org;
}

bool KDUAttrReader::atStartOfRecord () const
{
	if (m_pos >= m_size) return true;
	const uint8_t atf = m_data[m_pos];
	// does next attribute have SOR flag ?
	return (KDU_ATF_FLAGS (atf) & KDU_ATTR_F_SOR) != 0;
}

KDUErr KDUAttrReader::getNextAttr (KDUAttr& attr)
{
	uint32_t pos = m_pos;
	if (pos >= m_size) return KDU_E_EOF;

	const uint8_t atf = m_data[pos++];
	if ((KDU_ATF_FLAGS(atf) & KDU_ATTR_F_EXB) != 0) ++pos;

	unsigned int nb = KDU_ATF_NB (atf);
	if (m_size < pos || m_size - pos < nb) return KDU_E_BAD_ATTR;

	// skip attr
	if (KDU_ATTR_TYPE_IS_VECTOR(KDU_ATF_TYPE(atf))) {
		const uint32_t vlen = kdu_read_value (&m_data[pos], nb);
		pos += nb;
		if (m_size - pos < vlen) return KDU_E_BAD_ATTR;
		pos += vlen;
	} else {
		pos += nb;
	}

	attr.init (&m_data[m_pos]);
	m_pos = pos;

	return KDU_E_OK;
}

KDUErr KDUAttrReader::getNextRecord (KDUAttr& firstAttr)
{
	KDUAttr attr;

	// skip attrs until found attr with SOR flag
	while (! atStartOfRecord ()) {
		KDUErr err = getNextAttr (attr);
		if (err != KDU_E_OK) return err;
	}

	return getNextAttr (firstAttr);
}

KDUErr KDUAttrReader::skipRecord ()
{
	KDUAttr attr;
	while (! atStartOfRecord ()) {
		KDUErr err = getNextAttr (attr);
		if (err != KDU_E_OK) return err;
	}
	return KDU_E_OK;
}

KDUErr KDUAttrReader::skipToNextRecord()
{
	KDUAttr attr;
	KDUErr err;
	do
	{
		if (skipBlanks())
			return KDU_E_OK;
		if (atStartOfRecord())
			return KDU_E_OK;
		err = getNextAttr(attr);
	} while (err == KDU_E_OK);
	return err;
}

KDUErr KDUAttrReader::getRecordAttr (KDUAttr& attr)
{
	if (atStartOfRecord ()) return KDU_E_EOF;
	return getNextAttr (attr);
}

uint32_t KDUAttr::getLength () const
{
	const uint8_t *p = m_data;
	const uint8_t atf = *p++;

	if ((KDU_ATF_FLAGS(atf) & KDU_ATTR_F_EXB) != 0) ++p;

	unsigned int nb = KDU_ATF_NB (atf);

	if (KDU_ATTR_TYPE_IS_VECTOR(KDU_ATF_TYPE(atf))) {
		const uint32_t vlen = kdu_read_value (p, nb);
		p += nb + vlen;
	} else {
		p += nb;
	}
	
	return (uint32_t)(p - m_data);
}

uint32_t KDUAttr::getValue () const
{
	return kdu_read_value (&m_data[hasExtraByte () ? 2 : 1], KDU_ATF_NB (atf ()));
}

const uint8_t * KDUAttr::getVector (uint32_t *plen) const
{
	const uint8_t *p = &m_data[hasExtraByte () ? 2 : 1];
	const unsigned int nb = KDU_ATF_NB (atf ());
	if (plen) *plen = kdu_read_value (p, nb);
	return p + nb;
}

////////////////////////////////////////////////////////////////
// Attribute writer

KDUErr KDUWriteAttr (KLAV_Bin_Writer *writer, uint8_t type, uint32_t value, uint8_t flags, uint8_t exval)
{
	uint8_t abuf[6];
	uint8_t *ap = &abuf[1];
	if (exval != 0) flags |= KDU_ATTR_F_EXB;
	if ((flags & KDU_ATTR_F_EXB) != 0) *ap++ = exval;
	const unsigned int nb = KDUWriteValue (ap, value);
	abuf[0] = KDU_MAKE_ATF (type, nb, flags);
	if (! writer->write (abuf, (uint32_t) (ap - abuf) + nb)) return KDU_E_WRITE_ERROR;
	return KDU_E_OK;
}

KDUErr KDUWriteAttrFixed (KLAV_Bin_Writer *writer, uint8_t type, uint32_t value, uint8_t nbytes, uint8_t flags, uint8_t exval)
{
	if (nbytes < 1 || nbytes > 4) return KDU_E_BAD_SIZE;
	uint8_t abuf[6];
	uint8_t *ap = &abuf[1];
	if (exval != 0) flags |= KDU_ATTR_F_EXB;
	if ((flags & KDU_ATTR_F_EXB) != 0) *ap++ = exval;
	KDUWriteValueNB (ap, value, nbytes);
	abuf[0] = KDU_MAKE_ATF (type, nbytes, flags);
	if (! writer->write (abuf, (uint32_t) ( ap - abuf ) + nbytes)) return KDU_E_WRITE_ERROR;
	return KDU_E_OK;
}

KDUErr KDUWriteVectorAttr (KLAV_Bin_Writer *writer, uint8_t type, const void *data, uint32_t size, uint8_t flags, uint8_t exval)
{
	KDUErr err = KDUWriteAttr (writer, type, size, flags, exval);
	if (err != KDU_E_OK) return err;
	if (! data) return err; // The caller will write the data itself
	if (! writer->write (data, size)) return KDU_E_WRITE_ERROR;
	return err;
}

////////////////////////////////////////////////////////////////
// Section writer

struct KDUSectHdrBuf
{
	KDU_SECTION_HEADER hdr;
	uint32_t w[2];
};

uint32_t KDUSectionHeaderSize (const KDUSectionInfo& info, int version)
{
	uint32_t hdrsize = sizeof (KDU_SECTION_HEADER);

	if (version == KDU_VER_UNKNOWN)
	{
		if (info.size >= KDU_SECTION_LIMIT_V1 || ALWAYS_USE_KDU2)
		{
			version = KDU_VER_2;
		}
		else
		{
			version = KDU_VER_1;
		}
	}

	if (version == KDU_VER_2)
	{
		// use V2 structure
		hdrsize += sizeof (uint32_t); // section type always presents

		if (info.fragmentID != 0)    // fragment ID presents if nonzero
			hdrsize += sizeof (uint32_t);

	}
	else
	{
		if (KDUWKSectionType_V1 (info.sectionType) == KDU_SECT_TYPE_UNKNOWN)
			hdrsize += sizeof (uint32_t);

		if (KDUWKFragmentID_V1(info.fragmentID) == KDU_SECT_FRAG_UNKNOWN)
			hdrsize += sizeof (uint32_t);
	}
	return hdrsize;
}

uint32_t KDUFillSectionHeader (const KDUSectionInfo& info, void *buf, int version)
{
	KDUSectHdrBuf *sb = (KDUSectHdrBuf *)buf;
	uint32_t *pe = sb->w;

	if (version == KDU_VER_UNKNOWN)
	{
		if (info.size >= KDU_SECTION_LIMIT_V1 || ALWAYS_USE_KDU2)
		{
			version = KDU_VER_2;
		}
		else
		{
			version = KDU_VER_1;
		}
	}

	if (version == KDU_VER_2)
	{
		// use V2 structure
		uint8_t has_frag = (info.fragmentID == 0) ? 0 : 1;

		KDU_SECT_SET_INFO_V2 (&sb->hdr, info.raw, has_frag, info.size);

		*pe++ = kl_bswap_32_be (info.sectionType);

		if (has_frag)
			*pe++ = kl_bswap_32_be (info.fragmentID);
	}
	else
	{
		// use V1 structure
		uint8_t stype = KDUWKSectionType_V1(info.sectionType);
		uint8_t sfrag = KDUWKFragmentID_V1(info.fragmentID);

		KDU_SECT_SET_INFO_V1(&sb->hdr, info.raw, stype, sfrag, info.size);

		if (stype == KDU_SECT_TYPE_UNKNOWN)
			*pe++ = kl_bswap_32_be (info.sectionType);

		if (sfrag == KDU_SECT_FRAG_UNKNOWN)
			*pe++ = kl_bswap_32_be (info.fragmentID);
	}

	return (uint32_t)((uint8_t *)pe - (uint8_t *)buf);
}

KDUErr KDUWriteSection (KLAV_Bin_Writer *writer, const KDUSectionInfo& info, int version)
{
	KDUSectHdrBuf sb;
	uint32_t tothdrsize = KDUFillSectionHeader (info, &sb, version);

	if (!writer->write (&sb, tothdrsize))
		return KDU_E_WRITE_ERROR;

	if (info.size != 0 && !writer->write (info.data, info.size))
		return KDU_E_WRITE_ERROR;
	
	const uint32_t pad = KDU_SECT_ALIGN (info.size) - info.size;
	if (pad)
	{
		uint32_t tmp = 0;
		if (! writer->write (&tmp, pad))
			return KDU_E_WRITE_ERROR;
	}

	return KDU_E_OK;
}

////////////////////////////////////////////////////////////////
// Block writer

uint32_t KDUBlockHeaderSize (const KDUBlockInfo& info, int version)
{
	uint32_t shlen = KDU_BLOCK_ALIGN (info.subhdr_len);

	if (version == KDU_VER_UNKNOWN)
	{
		if (info.size >= KDU_BLOCK_LIMIT_V1 || ALWAYS_USE_KDU2)
		{
			version = KDU_VER_2;
		}
		else
		{
			version = KDU_VER_1;
		}
	}

	if (version == KDU_VER_2)
	{
		// use V2 block format
		return sizeof (KDU_BLOCK_HEADER_V2) + shlen;
	}
	else
	{
		// use V1 block format
		if (shlen >= 0x07 * 4)
		{
			return sizeof(KDU_BLOCK_HEADER_V1) + sizeof (uint32_t) + shlen;
		}

		return sizeof(KDU_BLOCK_HEADER_V1) + shlen;
	}
}

uint32_t KDUFillBlockHeader (const KDUBlockInfo& info, void *buf, int version)
{
	uint32_t shlen = info.subhdr_len;
	uint8_t *p = (uint8_t *)buf;

	if (version == KDU_VER_UNKNOWN)
	{
		if (info.size >= KDU_BLOCK_LIMIT_V1 || ALWAYS_USE_KDU2)
		{
			version = KDU_VER_2;
		}
		else
		{
			version = KDU_VER_1;
		}
	}

	if (version == KDU_VER_2)
	{
		// use V2 block format
		KDU_BLOCK_HEADER_V2 *hdr = (KDU_BLOCK_HEADER_V2 *)buf;

		KDU_BLOCK_SET_MAGIC_V2 (hdr);
		KDU_BLOCK_SET_INFO_V2 (hdr, info.type, info.subhdr_len, info.size);

		uint32_t shlen_aligned = KDU_BLOCK_ALIGN (shlen);
		p += sizeof (KDU_BLOCK_HEADER_V2);

		uint32_t i;
		for (i = 0; i < shlen; ++i)
		{
			*p++ = info.subhdr [i];
		}
		// write padding
		for (; i < shlen_aligned; ++i)
		{
			*p++ = 0;
		}
	}
	else
	{
		KDU_BLOCK_HEADER_V1 *hdr = (KDU_BLOCK_HEADER_V1 *)buf;

		KDU_BLOCK_SET_MAGIC_V1 (hdr);
		uint8_t needExtraShlen;
		uint32_t padBytes;
		KDU_BLOCK_SET_INFO_EX_V1 (hdr, info.type, shlen, info.size, needExtraShlen, padBytes);

		p += sizeof (KDU_BLOCK_HEADER_V1);

		if (needExtraShlen)
		{
			*(uint32_t *)p = kl_bswap_32_be (shlen);
			p += sizeof (uint32_t);
		}

		for (uint32_t i = 0; i < shlen; ++i)
		{
			*p++ = info.subhdr [i];
		}

		for (; padBytes > 0; --padBytes)
		{
			*p++ = 0;
		}
	}

	return (uint32_t)(p - (uint8_t *)buf);
}

KDUErr KDUWriteBlock (KLAV_Bin_Writer *writer, const KDUBlockInfo& info, int version)
{
	uint32_t padding = 0;
	uint32_t pad_size = 0;

	if (version == KDU_VER_UNKNOWN)
	{
		if (info.size >= KDU_BLOCK_LIMIT_V1 || ALWAYS_USE_KDU2)
		{
			version = KDU_VER_2;
		}
		else
		{
			version = KDU_VER_1;
		}
	}

	if (version == KDU_VER_2)
	{
		// use V2 block format
		KDU_BLOCK_HEADER_V2 hdr;
		KDU_BLOCK_SET_MAGIC_V2 (&hdr);

		KDU_BLOCK_SET_INFO_V2 (&hdr, info.type, info.subhdr_len, info.size);
		if (!writer->write (&hdr, sizeof (hdr)))
			return KDU_E_WRITE_ERROR;

		if (info.subhdr_len != 0)
		{
			if (!writer->write (info.subhdr, info.subhdr_len))
				return KDU_E_WRITE_ERROR;

			pad_size = KDU_BLOCK_ALIGN (info.subhdr_len) - info.subhdr_len;
			if (pad_size != 0 && !writer->write (&padding, pad_size))
				return KDU_E_WRITE_ERROR;
		}
	}
	else
	{
		KDU_BLOCK_HEADER_V1 hdr;
		KDU_BLOCK_SET_MAGIC_V1 (&hdr);

		bool needExtraShlen;
		KDU_BLOCK_SET_INFO_EX_V1 (&hdr, info.type, info.subhdr_len, info.size, needExtraShlen, pad_size);
	
		if (!writer->write (&hdr, sizeof hdr))
			return KDU_E_WRITE_ERROR;

		uint32_t shlen_be = kl_bswap_32_be (info.subhdr_len);
		if (needExtraShlen && !writer->write (&shlen_be, sizeof (uint32_t)))
			return KDU_E_WRITE_ERROR;

		if (info.subhdr_len != 0 && !writer->write (info.subhdr, info.subhdr_len))
			return KDU_E_WRITE_ERROR;

		if (pad_size != 0 && !writer->write (&padding, pad_size))
			return KDU_E_WRITE_ERROR;
	}
		
	if (info.size != 0 && !writer->write (info.data, info.size))
		return KDU_E_WRITE_ERROR;
	
	pad_size = KDU_BLOCK_ALIGN (info.size) - info.size;
	if (pad_size != 0)
	{
		if (! writer->write (&padding, pad_size))
			return KDU_E_WRITE_ERROR;
	}

	return KDU_E_OK;
}

////////////////////////////////////////////////////////////////
// Formatting and parsing utilites

const char * KDUErrorText (KDUErr ec)
{
	switch (ec)
	{
	case KDU_E_OK:
		return "";
	case KDU_E_EOF:
		return "EOF";
	case KDU_E_BAD_MAGIC:
		return "Invalid KDU file signature (raw data block ?)";
	case KDU_E_BAD_SIZE:
		return "Error in KDU block format (invalid block/section size)";
	case KDU_E_BAD_ATTR:
		return "Error in KDU block format (invalid attribute)";
	case KDU_E_BAD_ID:
		return "Error in KDU block format (unrecognized ID)";
	case KDU_E_WRITE_ERROR:
		return "Error writing KDU data";
	case KDU_E_COMPATIBILITY:
		return "Incompatible KDU format version";
	}
	return "Unknown error code";
}

struct KDUName {
	unsigned int code;
	const char * name;
};
	
struct KDUName KDUBlockTypeNames[] = {
	{ KDU_BLOCK_TYPE_LINK,  "LINK" },
	{ KDU_BLOCK_TYPE_PATCH, "PATCH" }
};

#define KDU_TABLE_REF(TABLE) TABLE, sizeof(TABLE)/sizeof(TABLE[0])

static bool kdu_streq (const char *s1, const char *s2)
{
	for (;;s1++,s2++)
	{
		uint8_t c1 = *s1, c2 = *s2;
		if (c1 != c2)
		{
			if (c1 >= 'A' && c1 <= 'Z') c1 += ('a' - 'A');
			if (c2 >= 'A' && c2 <= 'Z') c2 += ('a' - 'A');
			if (c1 != c2)
				return false;
		}
		if (c1 == 0)
			break;
	}

	return true;
}

const char * KDUFormatName (const KDUName *table, unsigned int tabsize, unsigned int code)
{
	for (unsigned int i=0; i<tabsize; ++i) {
		const KDUName& n = table[i];
		if (n.code == code) return n.name;
	}
	return "";
}

int KDUParseName (const KDUName *table, unsigned int tabsize, const char *name)
{
	if (name == 0) return -1;
	for (unsigned int i=0; i<tabsize; ++i) {
		const KDUName& n = table[i];
		if (kdu_streq (name, n.name)) return n.code;
	}
	return -1;
}

const char * KDUFormatBlockType (unsigned int blockType)
{
	return KDUFormatName (KDU_TABLE_REF(KDUBlockTypeNames), blockType);
}

int KDUParseBlockType (const char *name)
{
	return KDUParseName (KDU_TABLE_REF(KDUBlockTypeNames), name);
}

struct WKIDTable {
	uint32_t id;
	uint32_t code; // short code
};

static WKIDTable WKSectionTypes[] = {
	{ KDBID_MANIFEST,  KDU_SECT_TYPE_MANIFEST },
	{ KDBID_LB_CONFIG, KDU_SECT_TYPE_LB },
};

static WKIDTable WKFragmentIDs[] = {
	{ 0,  KDU_SECT_FRAG_DEFAULT },
};

static bool KDUMapWKID (uint8_t code, uint32_t& id, const WKIDTable* table, unsigned int size)
{
	for (unsigned int i=0; i<size; ++i) {
		if (table[i].code == code) {
			id = table[i].id;
			return true;
		}
	}
	id = 0;
	return false;
}

static uint8_t KDUGetWKID (uint32_t id, const WKIDTable* table, unsigned int size)
{
	for (unsigned int i=0; i<size; ++i) {
		if (table[i].id == id) return (uint8_t) table[i].code;
	}
	return 0;
}

bool KDUMapWKSectionType_V1 (uint8_t stype, uint32_t& sectionType)
{
	return KDUMapWKID (stype, sectionType, KDU_TABLE_REF(WKSectionTypes));
}

bool KDUMapWKFragmentID_V1 (uint8_t sfrag, uint32_t& fragmentID)
{
	return KDUMapWKID (sfrag, fragmentID, KDU_TABLE_REF(WKFragmentIDs));
}

uint8_t KDUWKSectionType_V1 (uint32_t sectionType)
{
	return KDUGetWKID (sectionType, KDU_TABLE_REF(WKSectionTypes));
}

uint8_t KDUWKFragmentID_V1 (uint32_t fragmentID)
{
	return KDUGetWKID (fragmentID, KDU_TABLE_REF(WKFragmentIDs));
}

