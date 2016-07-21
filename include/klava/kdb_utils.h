// kdb_utils.h
//

#ifndef kdb_utils_h_INCLUDED
#define kdb_utils_h_INCLUDED

#include <kl_types.h>

#include <klava/kdb_ids.h>
#include <klava/kl_tchar.h>
#include <klava/kdu_utils.h>
#include <klava/klavdb.h>

#ifdef __cplusplus
# define KDB_INLINE inline
# define KDB_EXTERN_C extern "C"
#else
# define KDB_INLINE
# define KDB_EXTERN_C
#endif

////////////////////////////////////////////////////////////////
// KDB_ID formatting helpers

#ifdef __cplusplus

inline uint32_t KDBID_FROM_STRING (const char *s)
	{ return MAKE_KDBID (s[0],s[1],s[2],s[3]); }

inline unsigned char KDBID_CHAR_VALUE(unsigned char c)
	{ return c=='_' ? 0 : c; }

#endif // __cplusplus

#define KDBID_STRING_MAXSIZE 20

// str_kdbid must consist of 4 characters
// returns 1 if OK, 0 if invalid characters were encountered
KDB_EXTERN_C int KDBID_PARSE_STRING_QUAD (const char *str_kdbid, uint32_t *pval);

KDB_EXTERN_C int KDBID_STRING_CHAR (unsigned int c);

// buffer size must be at least KDBID_STRING_MAXSIZE
KDB_EXTERN_C size_t KDBID_STRING_FORMAT (KDBID id, char *buf);

////////////////////////////////////////////////////////////////
// KDB filename formatting

// buffer size must be at lease KDB_FILENAME_MAXSIZE
#define KDB_FILENAME_MAXSIZE 50

KDB_EXTERN_C size_t KDBFormatID (KDBID id, char *buf, int pad);

// returns number of characters written, 0 on error
KDB_EXTERN_C size_t KDBFormatFilename (KDBID sectionType, KDBID fragmentID, char *buf);

////////////////////////////////////////////////////////////////
// parser interface

typedef enum tagKDBToken {
	KDB_ERROR = -1,
	KDB_EOL = 0,   // end-of-line
	KDB_INT,       // integer value
	KDB_STR,       // string value
	KDB_BIN,       // binary value
} KDBToken;

typedef struct KDBParserImpl KDBParser;

KDB_INLINE
void KDBParser_Init (KDBParser *parser, const void *buffer, size_t len);

KDB_INLINE
int  KDBParser_EOF (KDBParser *parser);

KDB_INLINE
void KDBParser_NextLine (KDBParser *parser);

KDB_INLINE
uint32_t KDBParser_IntValue (KDBParser *parser);

KDB_INLINE
const char * KDBParser_StrValue (KDBParser *parser, size_t *plen);

KDB_INLINE
int KDBParser_ScanInt (KDBParser *parser, uint32_t *value);

KDB_INLINE
int KDBParser_ScanStr (KDBParser *parser, const char **ppstr, size_t *plen);

KDB_EXTERN_C
KDBToken KDBParser_GetToken (KDBParser *parser);


struct KDBParserImpl
{
	const char * pp;   // parse pointer
	const char * pe;   // end-of-stream
	const char * pt;   // start-of-token pointer
	const char * pte;  // end-of-token pointer
	uint32_t     ival; // integer value

#ifdef __cplusplus
	KDBParserImpl (const void *buf=0, size_t len=0)
		{ KDBParser_Init (this, buf, len); }

	void init (const void *buf, size_t len)
		{ KDBParser_Init (this, buf, len); }

	bool eof ()
		{ return KDBParser_EOF (this) != 0; }

	void nextLine ()
		{ KDBParser_NextLine (this); }

	uint32_t intValue ()
		{ return KDBParser_IntValue (this); }

	const char * strValue (size_t *plen)
		{ return KDBParser_StrValue (this, plen); }

	bool scanInt (uint32_t *value)
		{ return KDBParser_ScanInt (this, value) > 0; }

	bool scanStr (const char **ppstr, size_t *plen)
		{ return KDBParser_ScanStr (this, ppstr, plen) > 0; }

	KDBToken getToken ()
		{ return KDBParser_GetToken (this); }

	bool skipToken ()
		{ return KDBParser_GetToken (this) > 0; }
#endif // __cplusplus

};

////////////////////////////////////////////////////////////////
// parser implementation

#if defined (KDB_UTILS_IMPL) || defined (__cplusplus)

KDB_INLINE
void KDBParser_Init (KDBParser *parser, const void *buffer, size_t len)
{
	const char *pb = (const char *)buffer;
	parser->pp = pb;
	parser->pe = pb + len;
	parser->pt = pb;
	parser->pte = pb;
	parser->ival = 0;
}

KDB_INLINE
int  KDBParser_EOF (KDBParser *parser)
{
	return parser->pp == parser->pe;
}

KDB_INLINE
void KDBParser_NextLine (KDBParser *parser)
{
	while (parser->pp != parser->pe) {
		if (*(parser->pp)++ == '\n') break;
	}
}

KDB_INLINE
uint32_t KDBParser_IntValue (KDBParser *parser)
{
	return parser->ival;
}

KDB_INLINE
const char * KDBParser_StrValue (KDBParser *parser, size_t *size)
{
	*size = parser->pte - parser->pt;
	return parser->pt;
}

KDB_INLINE
int KDBParser_ScanInt (KDBParser *parser, uint32_t *value)
{
	if (KDBParser_GetToken (parser) == KDB_INT) {
		*value = KDBParser_IntValue (parser);
		return 1;
	}
	*value = 0;
	return 0;
}

KDB_INLINE
int KDBParser_ScanStr (KDBParser *parser, const char **ppstr, size_t *plen)
{
	KDBToken tok = KDBParser_GetToken (parser);
	if (tok == KDB_STR || tok == KDB_BIN) {
		*ppstr = KDBParser_StrValue (parser, plen);
		return 1;
	}
	*ppstr = NULL;
	*plen = 0;
	return 0;
}

#endif // defined (KDB_UTILS_IMPL) || defined (__cplusplus)

////////////////////////////////////////////////////////////////
// Utility classes

#ifdef __cplusplus

struct KdbManifestKernelParm
{
	uint32_t	id;
	uint32_t	value;

	KdbManifestKernelParm ()
		{ clear (); }

	void clear () { id = 0; value = 0; }
};

struct KdbManifestSectionInfo
{
	uint32_t       sectionType;
	uint32_t       fragmentID;
	uint32_t       mergerTypeFlags;
	uint32_t       size;
	const uint8_t* signature;
	uint32_t       signatureLen;

	KdbManifestSectionInfo ()
		{ clear (); }

	void clear ();

	uint32_t getSectionType() const
		{ return sectionType; }

	uint32_t getFragmentID() const
		{ return fragmentID; }

	uint32_t getMergerType () const
		{ return KDU_KPM_TYPE (mergerTypeFlags); }

	uint32_t getMergerFlags () const
		{ return KDU_KPM_FLAGS (mergerTypeFlags); }

	uint32_t getSize() const
		{ return size; }

	const uint8_t* getSignature (uint32_t* psize) const
		{ *psize = signatureLen; return signature; }
};

////////////////////////////////////////////////////////////////
// Manifest reader

class KdbManifestReader
{
public:

	typedef KdbManifestKernelParm  KernelParm;
	typedef KdbManifestSectionInfo SectionInfo;

	class KernelParmEnum
	{
	public:
		KernelParmEnum (const uint8_t* sectbuf = 0, uint32_t bufsize = 0)
			{ init (sectbuf, bufsize); }

		void init (const uint8_t* sectbuf, uint32_t bufsize);

		KLAV_ERR getNext (KernelParm& parm);

	protected:
		KDUAttrReader m_reader;
	};

	class SectionEnum
	{
	public:
		SectionEnum (const uint8_t* sectbuf = 0, uint32_t bufsize = 0)
			{ init (sectbuf, bufsize); }
		
		void init (const uint8_t* sectbuf, uint32_t bufsize);

		KLAV_ERR getNextSection (SectionInfo& sectInfo);

	protected:
		KDUAttrReader m_reader;
	};

	KdbManifestReader();

	// reads kernel parameters
	KLAV_ERR init (const uint8_t* buffer, uint32_t size);

	// reads kernel parameters
	KernelParmEnum enumKernelParms ()
		{ return KernelParmEnum (m_parmSectionData, m_parmSectionSize); }

	// reads section table
	SectionEnum enumSections ()
		{ return SectionEnum (m_dbListSectionData, m_dbListSectionSize); }

	uint32_t get_db_version () const
		{ return m_dbVersion; }

	uint32_t get_db_release () const
		{ return m_dbIncarnation; }

	bool findKernelParm (uint32_t id, uint32_t *pval);
	uint32_t getKernelParm (uint32_t id);

protected:
	uint32_t        m_dbVersion;
	uint32_t        m_dbIncarnation;
	const uint8_t * m_parmSectionData;
	uint32_t        m_parmSectionSize;
	const uint8_t * m_dbListSectionData;
	uint32_t        m_dbListSectionSize;

	void clear ();
};

////////////////////////////////////////////////////////////////
// KdbManifestWriter

class KdbManifestWriter
{
public:
		KdbManifestWriter (KLAV_Alloc *alloc);
		~KdbManifestWriter ();

	typedef KdbManifestSectionInfo SectionInfo;

	KLAV_ERR copy (KdbManifestReader& reader);

	void clear ();

	void set_db_version (uint32_t release, uint32_t version);

	KLAV_ERR setKernelParm (uint32_t id, uint32_t value);
	uint32_t getKernelParm (uint32_t id);
	void clearKernelParm (uint32_t id);
	void clearKernelParms ();

	void clearSections ();
	KLAV_ERR addSection (const SectionInfo& info);
	KLAV_ERR removeSection (uint32_t sectionType, uint32_t fragmentID);

	KLAV_ERR write (KLAV_Bin_Writer *writer, bool writeSignatures);

private:
	class KdbManifestWriterImpl *m_pimpl;
};

#endif // __cplusplus


#endif // kdb_utils_h_INCLUDED


