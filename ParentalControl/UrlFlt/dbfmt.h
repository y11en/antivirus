
#ifndef __PARCTL_DB_FMT_H
#define __PARCTL_DB_FMT_H

// ########################################################################
// ########################################################################

#define _MAKEDWORD(h, l)     ((uint32_t)(((uint16_t)((uint32_t)(l) & 0xffff)) | ((uint32_t)((uint16_t)((uint32_t)(h) & 0xffff))) << 16))
#define _LOWORD(l)           ((uint16_t)((uint32_t)(l) & 0xffff))
#define _HIWORD(l)           ((uint16_t)((uint32_t)(l) >> 16))

struct _ParctlCatgRange
{
	uint8_t		m_id;
	uint8_t		m_weight;
};

struct _ParctlCatgRanges
{
	uint8_t				m_n;
	_ParctlCatgRange	m_a[1];
};

#define _ParctlCatgRanges_size(_n)	(sizeof(_ParctlCatgRanges) + (_n - 1)*sizeof(_ParctlCatgRange))

struct _ParctlUrlHash
{
	uint8_t		m_hash[8];
};

struct _ParctlDbUrls
{
	uint8_t		m_offset[4];
	uint8_t		m_size[4];
	uint8_t		m_count[4];

	uint8_t		m_hashesOffset[4];
	uint8_t		m_hashesSize[4];
};

struct _ParctlDbHdr
{
	uint8_t			m_magic[4];
	uint8_t			m_ver[4];

	uint8_t			m_catgsOffset[4];
	uint8_t			m_catgsSize[4];

	uint8_t			m_wordsOffset[4];
	uint8_t			m_wordsSize[4];

	_ParctlDbUrls	m_urlsBlack;
	_ParctlDbUrls	m_urlsWhite;

	// Categories offsets for URLs
	uint8_t			m_urlsCatgsOffset[4];
	uint8_t			m_urlsCatgsSize[4];
};

// ########################################################################

void		_WriteInt16(uint8_t * buf, uint16_t v);
uint16_t	_ReadInt16(uint8_t * buf);
void		_WriteInt32(uint8_t * buf, uint32_t v);
uint32_t	_ReadInt32(uint8_t * buf);
void		_WriteInt32Ex(uint8_t * buf, uint32_t cb, uint32_t v);
uint32_t	_ReadInt32Ex(uint8_t * buf, uint32_t cb);

void		_ParctlXorBuf(uint8_t * buf, uint32_t size, bool forward, bool smart = true);

// ########################################################################
// ########################################################################

#endif // __PARCTL_DB_FMT_H
