// kdu_utils.h
//

#ifndef kdu_utils_h_INCLUDED
#define kdu_utils_h_INCLUDED

#include <kl_types.h>

#include <klava/binstream.h>
#include <klava/kdu.h>

#ifdef __cplusplus

// format version
enum KDUVersion
{
	KDU_VER_UNKNOWN = 0,
	KDU_VER_1 = 1,
	KDU_VER_2 = 2
};

typedef enum KDUErr
{
	KDU_OK = 0,
	KDU_E_OK = KDU_OK,   // deprecated
	KDU_EOF = 1,
	KDU_E_EOF = KDU_EOF, // deprecated
	KDU_E_BAD_MAGIC,     // block magic invalid
	KDU_E_COMPATIBILITY, // block has extended format
	KDU_E_BAD_SIZE,      // invalid block/section size
	KDU_E_BAD_ATTR,      // invalid attribute type/flags or size
	KDU_E_BAD_ID,        // unknown identifier in sectionID/fragmentID
	KDU_E_WRITE_ERROR    // stream write error
} KDU_ERR;

struct KDUBlockInfo
{
	const uint8_t * start;       // block start in buffer (read-only)
	const uint8_t * data;        // block data
	const uint8_t * subhdr;      // ptr to sub-header
	uint32_t        size;        // block data size
	uint32_t        subhdr_len;  // sub-header size in bytes
	uint8_t         type;        // block type
};

struct KDUSectionInfo
{
	const uint8_t * start; // section start in buffer (read-only)
	const uint8_t * data;  // section data
	uint32_t        size;  // section data size
	uint32_t sectionType;  // type of section (aka engineID)
	uint32_t fragmentID;   // instance ID for given section type
	uint8_t  raw;          // raw section flag
};

////////////////////////////////////////////////////////////////
// KDU attribute

class KDUAttr {
public:
		KDUAttr (const uint8_t *data = 0)
			{ init (data); }

	void init (const uint8_t *data)
			{ m_data = data; }

	const uint8_t * data () const
		{ return m_data; }

	uint8_t atf () const
		{ return m_data[0]; }

	uint8_t type () const
		{ return KDU_ATF_TYPE(atf()); }

	uint8_t flags () const
		{ return KDU_ATF_FLAGS(atf()); }

	uint32_t getLength () const;

	bool isStartOfRecord () const
		{ return (flags () & KDU_ATTR_F_EXB) != 0; }

	bool isVector () const
		{ return KDU_ATTR_TYPE_IS_VECTOR(type()) != 0; }

	bool hasExtraByte () const
		{ return (flags () & KDU_ATTR_F_EXB) != 0; }

	uint8_t extraByte () const
		{ return hasExtraByte() ? m_data[1] : 0; }

	uint32_t getValue () const;
	const uint8_t * getVector (uint32_t *psize) const;

private:
	const uint8_t * m_data;     // data for vector attributes
};

////////////////////////////////////////////////////////////////
// KDU attribute section reader

class KDUAttrReader
{
public:
		KDUAttrReader (const void *data=0, uint32_t size=0)
			{ init (data,size); }

		KDUAttrReader (const KDUSectionInfo& info)
			{ init (info); }

	void init (const void *data, uint32_t size)
		{ m_data = (uint8_t *)data; m_size = size; m_pos = 0; }

	void init (const KDUSectionInfo& info)
		{ init (info.data, info.size); }

	const uint8_t * data () const
		{ return m_data; }

	uint32_t size () const
		{ return m_size; }

	const uint32_t pos () const
		{ return m_pos; }

	void reset ()
		{ m_pos = 0; }

	bool atEnd () const
		{ return m_pos >= m_size; }

	// returns number of zeroes skipped
	uint32_t skipBlanks ();

	bool atStartOfRecord () const;
	
	KDUErr getNextAttr (KDUAttr& attr);

	// get first attr from new record
	KDUErr getNextRecord (KDUAttr& firstAttr);
	// skips to the end of record (does not check for EOF)
	KDUErr skipRecord ();

	KDUErr skipToNextRecord();
	// check if we are at end of record
	bool   atEndOfRecord () const
		{ return atStartOfRecord (); }
	// returns KDU_E_EOF on end-of-record
	KDUErr getRecordAttr (KDUAttr& attr);

private:
	const uint8_t * m_data;
	uint32_t m_size;
	uint32_t m_pos;
};

////////////////////////////////////////////////////////////////
// Section reader

class KDUSectionReader
{
public:
	KDUSectionReader (const void *data=0, uint32_t size=0)
		{ init (data,size); }

	KDUSectionReader (const KDUBlockInfo& info)
		{ init (info); }
	
	void init (const void *data, uint32_t size)
		{ m_data = (uint8_t *)data; m_size = size; m_pos = 0; }

	void init (const KDUBlockInfo& info)
		{ init (info.data, info.size); }

	const uint8_t * data () const
		{ return m_data; }

	uint32_t size () const
		{ return m_size; }

	const uint32_t pos () const
		{ return m_pos; }

	void reset ()
		{ m_pos = 0; }

	bool atEnd () const
		{ return m_pos >= m_size; }

	KDUErr getNextSection (KDUSectionInfo& info);

private:
	const uint8_t * m_data;
	uint32_t m_size;
	uint32_t m_pos;
};

////////////////////////////////////////////////////////////////
// Block reader

class KDUBlockReader
{
public:
	KDUBlockReader (const void *data=0, uint32_t size=0)
		{ init (data,size); }

	void init (const void *data, uint32_t size)
		{ m_data = (uint8_t *)data; m_size = size; m_pos = 0; }

	const uint8_t * data () const
		{ return m_data; }

	uint32_t size () const
		{ return m_size; }

	const uint32_t pos () const
		{ return m_pos; }

	void reset ()
		{ m_pos = 0; }

	bool atEnd () const
		{ return m_pos >= m_size; }

	KDUErr getNextBlock (KDUBlockInfo& Info);

private:
	const uint8_t * m_data;
	uint32_t m_size;
	uint32_t m_pos;
};

////////////////////////////////////////////////////////////////
// Attribute writer

// write scalar attribute
KDUErr KDUWriteAttr (KLAV_Bin_Writer *writer, uint8_t type, uint32_t value, uint8_t flags = 0, uint8_t exval = 0);

// write scalar attribute with fixed size
KDUErr KDUWriteAttrFixed (KLAV_Bin_Writer *writer, uint8_t type, uint32_t value, uint8_t nbytes, uint8_t flags = 0, uint8_t exval = 0);

// write vector attribute
KDUErr KDUWriteVectorAttr (KLAV_Bin_Writer *writer, uint8_t type, const void *data, uint32_t size, uint8_t flags = 0, uint8_t exval = 0);

////////////////////////////////////////////////////////////////
// Section writer

uint32_t KDUSectionHeaderSize (const KDUSectionInfo& info, int version);
uint32_t KDUFillSectionHeader (const KDUSectionInfo& info, void *buf, int version);

KDUErr KDUWriteSection (KLAV_Bin_Writer *writer, const KDUSectionInfo& info, int version = KDU_VER_UNKNOWN);

////////////////////////////////////////////////////////////////
// Block writer

uint32_t KDUBlockHeaderSize (const KDUBlockInfo& info, int version);
uint32_t KDUFillBlockHeader (const KDUBlockInfo& info, void *buf, int version);

KDUErr KDUWriteBlock (KLAV_Bin_Writer *writer, const KDUBlockInfo& info, int version = KDU_VER_UNKNOWN);

////////////////////////////////////////////////////////////////
// Utilites

// returns text corresponding to KDUErr code
const char * KDUErrorText (KDUErr ec);

// returns empty string ("") if none matches
const char * KDUFormatBlockType (unsigned int blockType);
const char * KDUFormatSectType (unsigned int sectType);
const char * KDUFormatSectFrag (unsigned int sectFrag);

// returns -1 if none matches
int KDUParseBlockType (const char *name);

// read multibyte integer
uint32_t KDUReadValue (const uint8_t *p, unsigned int nb);

// returns number of bytes required for multibyte integer
unsigned int KDUValueSize (uint32_t val);

// write multibyte integer, returns number of bytes written (buf must be at least 4 bytes size)
unsigned int KDUWriteValue (uint8_t *buf, uint32_t val);

// maps well-known codes to sectionType/fragmentID, false if no match
bool KDUMapWKSectionType_V1 (uint8_t stype, uint32_t& sectionType);
bool KDUMapWKFragmentID_V1 (uint8_t sfrag, uint32_t& fragmentID);

// returns well-known codes for sectionType/fragmentID
uint8_t KDUWKSectionType_V1 (uint32_t sectionType);
uint8_t KDUWKFragmentID_V1 (uint32_t fragmentID);

#endif // __cplusplus

#endif // kdu_utils_h_INCLUDED
