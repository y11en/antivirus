// parctl_mkdb.cpp
//

#define KLAV_PRAGUE 0

#ifdef _MSC_VER
# pragma warning(disable:4786)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <klava/kdb_utils.h>
#include <klava/kdu_utils.h>
#include <kl_byteswap.h>

#include <string>
#include <vector>

#include <klava/kl_sys_mmap.h>
#include <klava/klavtree.h>

#include "..\dbfmt.h"
#include "utils.h"

// ########################################################################
// ########################################################################

struct CParctlWords
{
public:
	CParctlWords();
	~CParctlWords();

	bool		Init();
	void		Clear();
	bool		Add(std::wstring& _word, uint32_t catRelOffset);
	uint32_t	Write(uint8_t * buf);

private:	
	struct _MemIO
	{
		_MemIO(std::vector<uint8_t>& p_buf) : buf(p_buf), c(0) {}
		
		std::vector<uint8_t>&	buf;
		uint32_t				c;
	};
	
	static void* _TREE_CALL _Alloc_(void*, unsigned int dwBytes) { return calloc(sizeof(char), dwBytes); }
	static void* _TREE_CALL _Realloc_(void*, void* pMem, unsigned int dwBytes) { return realloc(pMem, dwBytes); }
	static void  _TREE_CALL _Free_(void*, void* pMem) { free(pMem); }

	static KLAV_ERR _TREE_CALL _Read(void* pIO, uint32_t* pdwRead, uint8_t* pbBuffer, uint32_t dwSize);
	static KLAV_ERR _TREE_CALL _Write(void* pIO, uint32_t* pdwWritten, uint8_t* pbBuffer, uint32_t dwSize);
	static KLAV_ERR _TREE_CALL _SeekRead(void* pIO, uint32_t* pdwRead, uint64_t qwOffset, uint8_t* pbBuffer, uint32_t dwSize);
	static KLAV_ERR _TREE_CALL _SeekWrite(void* pIO, uint32_t* pdwWritten, uint64_t qwOffset, uint8_t* pbBuffer, uint32_t dwSize);
	static KLAV_ERR _TREE_CALL _Seek(void* pIO, uint64_t* pqwNewPosition, uint64_t qwPosition, uint32_t dwOrigin);

private:
	uint32_t				m_nWords;
	sHEAP_CTX				m_heap_ctx;
	sRT_CTX *				m_tree_ctx;
	std::vector<uint8_t>	m_tmp;
};

// ########################################################################

CParctlWords::CParctlWords() : m_nWords(0)
{
	m_heap_ctx.pHeap = 0;
	m_heap_ctx.Alloc = _Alloc_;
	m_heap_ctx.Realloc = _Realloc_;
	m_heap_ctx.Free = _Free_;
	
	m_tree_ctx = NULL;
}

CParctlWords::~CParctlWords()
{
	Clear();
	TreeDone(m_tree_ctx);
}

bool CParctlWords::Init()
{
	return KLAV_SUCC(TreeInit(&m_heap_ctx, &m_tree_ctx));
}

void CParctlWords::Clear()
{
	TreeUnload_Any(m_tree_ctx);
//	m_sigs.clear();
}

bool CParctlWords::Add(std::wstring& _word, uint32_t catRelOffset)
{
	size_t n;
	
	if( _word.size() )
	{
		if( _word[0] == L'*' )
			_word.erase(0, 1);
		else
			_word.insert(0, 1, L' ');
	}
	
	if( n = _word.size() )
	{
		if( _word[n - 1] == L'*' )
			_word.erase(n - 1, 1);
		else
			_word.append(1, L' ');
	}

	tBin _sgn; uint32_t dwRawDataSize = 0;
	TreeConvertRaw2Signature((const uint8_t *)_word.c_str(), sizeof(wchar_t)*_word.size(), NULL, 0, &dwRawDataSize);
	_sgn.resize(dwRawDataSize);
	if( !_sgn.size() || _sgn.size() != dwRawDataSize )
		return false;
	
	TreeConvertRaw2Signature((const uint8_t *)_word.c_str(), sizeof(wchar_t)*_word.size(), &_sgn[0], _sgn.size(), &dwRawDataSize);
	
	tSIGNATURE_ID res_id;
	if( KLAV_FAILED(TreeAddSignature(m_tree_ctx, (const uint8_t *)&_sgn[0], _sgn.size(), catRelOffset == -1 ? -1 : (catRelOffset + 1), &res_id)) )
		return false;
	
	m_nWords++;
	return true;
}

uint32_t CParctlWords::Write(uint8_t * buf)
{
	if( buf )
	{
		int size = m_tmp.size();
		if( size )
			memcpy(buf, &m_tmp.front(), size);
		m_tmp.clear();
		return size;
	}

	if( !m_nWords )
		return 0;
	
	_MemIO _io(m_tmp);

	sIO_CTX IOContext = {(void *)&_io, _Read, _Write, _SeekRead, _SeekWrite, _Seek};
	if( KLAV_FAILED(TreeSave_Static(m_tree_ctx, &IOContext, 0, 0)) )
		return -1;

	return m_tmp.size();
}

// ########################################################################

KLAV_ERR CParctlWords::_Read(void* pIO, uint32_t* pdwRead, uint8_t* pbBuffer, uint32_t dwSize)
{
	_MemIO& _io = *(_MemIO *)pIO;

	uint32_t read = dwSize;
	if( _io.c >= _io.buf.size() )
		read = 0;
	else if( read > (_io.buf.size() - _io.c) )
		read = _io.buf.size() - _io.c;
	
	if( read )
	{
		memcpy(pbBuffer, &_io.buf.front() + _io.c, read);
		_io.c += read;
	}

	if( pdwRead ) *pdwRead = read;
	return read == dwSize ? KLAV_OK : KLAV_EOF;
}

KLAV_ERR CParctlWords::_SeekRead(void* pIO, uint32_t* pdwRead, uint64_t qwOffset, uint8_t* pbBuffer, uint32_t dwSize)
{
	_MemIO& _io = *(_MemIO *)pIO;

	uint32_t read = dwSize;
	if( (uint32_t)qwOffset >= _io.buf.size() )
		read = 0;
	else if( read > (_io.buf.size() - (uint32_t)qwOffset) )
		read = _io.buf.size() - (uint32_t)qwOffset;

	if( read )
		memcpy(pbBuffer, &_io.buf.front() + (uint32_t)qwOffset, read);

	if( pdwRead ) *pdwRead = read;
	return read == dwSize ? KLAV_OK : KLAV_EOF;
}

KLAV_ERR CParctlWords::_Write(void* pIO, uint32_t* pdwWritten, uint8_t* pbBuffer, uint32_t dwSize)
{
	_MemIO& _io = *(_MemIO *)pIO;
	
	uint32_t size = _io.c + dwSize;
	if( size > _io.buf.size() )
	{
		_io.buf.resize(size);
		if( _io.buf.size() != size )
			return KLAV_ENOMEM;
	}
	
	memcpy(&_io.buf.front() + _io.c, pbBuffer, dwSize);
	_io.c += dwSize;
	
	if( pdwWritten ) *pdwWritten = dwSize;
	return KLAV_OK;
}

KLAV_ERR CParctlWords::_SeekWrite(void* pIO, uint32_t* pdwWritten, uint64_t qwOffset, uint8_t* pbBuffer, uint32_t dwSize)
{
	_MemIO& _io = *(_MemIO *)pIO;

	uint32_t size = (uint32_t)(qwOffset + dwSize);
	if( size > _io.buf.size() )
	{
		_io.buf.resize(size);
		if( _io.buf.size() != size )
			return KLAV_ENOMEM;
	}

	memcpy(&_io.buf.front() + qwOffset, pbBuffer, dwSize);

	if( pdwWritten ) *pdwWritten = dwSize;
	return KLAV_OK;
}

KLAV_ERR CParctlWords::_Seek(void* pIO, uint64_t* pqwNewPosition, uint64_t qwPosition, uint32_t dwOrigin)
{
	_MemIO& _io = *(_MemIO *)pIO;

	switch( dwOrigin )
	{
	case TREE_IO_CTX_SEEK_CUR:
		_io.c += (int32_t)(int64_t)qwPosition;
		break;
	
	case TREE_IO_CTX_SEEK_SET:
		_io.c = (int32_t)qwPosition;
		break;
	
	case TREE_IO_CTX_SEEK_END:
		if( _io.buf.size() < qwPosition )
			return KLAV_EUNEXPECTED;
		_io.c = _io.buf.size() - (int32_t)qwPosition;
		break;
	
	default:
		return KLAV_EUNEXPECTED;
	}
	
	if( pqwNewPosition ) *pqwNewPosition = _io.c;
	return KLAV_OK;
}

// ########################################################################

struct DBStructure
{
public:
	struct CatgRange
	{
		uint8_t		m_id;
		uint8_t		m_weight;
	};

	class CatgsRanges : public std::vector<CatgRange>
	{
	public:
		CatgsRanges() : m_relOffset(-1) {}
		
		uint32_t	m_relOffset;
	};

	class CatgsRangesArray : public std::vector<CatgsRanges>
	{
	public:
		CatgsRangesArray() : m_curRelOffset(0) {}
		
		uint32_t Add(CatgsRanges& p_catgRange)
		{
			for(uint32_t i = 0, n = size(); i < n; i++)
			{
				CatgsRanges& c = at(i);
				if( c.size() != p_catgRange.size() )
					continue;

				size_type k, l;
				
				for(k = 0; k < p_catgRange.size(); k++)
				{
					CatgRange& _range = p_catgRange[k];
					
					for(l = 0; l < c.size(); l++)
					{
						CatgRange& _rangeC = c[l];
						if( _rangeC.m_id == _range.m_id &&
							_rangeC.m_weight == _range.m_weight )
							break;
					}
				
					if( l == c.size() )
						break;
				}
				
				if( k == p_catgRange.size() )
					return c.m_relOffset;
			}

			push_back(p_catgRange);
			CatgsRanges& c = at(size() - 1);
			c.m_relOffset = m_curRelOffset;
			
			m_curRelOffset += sizeof(uint8_t) + c.size() * sizeof(_ParctlCatgRange);
			return c.m_relOffset;
		}
		
		uint32_t	m_curRelOffset;
	};

public:
	struct Url
	{
		std::string		m_name;
		uint32_t		m_catRelOffset;
	};

	typedef std::vector<Url>	Urls;

public:
	DBStructure()
	{
		m_words.Init();
	}
	
	CatgsRangesArray	m_catgs;
	Urls				m_urlsBlack;
	Urls				m_urlsWhite;
	CParctlWords		m_words;
};

// ########################################################################
// ########################################################################

void _ReadCatgs(char * buf, DBStructure::CatgsRanges& _catgs)
{
	char * sep;
	while( *buf )
	{
		char * catId = buf;
		char * catWeight = strchr(buf, '(');
		if( catWeight )
		{
			*catWeight = 0;
			catWeight++;
			
			sep = strchr(catWeight, ')');
			if( !sep )
				break;

			*sep = 0;
			sep++;
		}

		if( !catWeight )
			break;
		
		DBStructure::CatgRange& _catg = *_catgs.insert(_catgs.end(), DBStructure::CatgRange());
		_catg.m_id = atoi(catId);
		_catg.m_weight = atoi(catWeight);

		buf = sep;
	}
}

uint32_t ReadActgOffset(char * buf, DBStructure& dbStruct)
{
	uint32_t _catRelOffset = -1;
	if( buf )
	{
		DBStructure::CatgsRanges _catgs; _ReadCatgs(buf, _catgs);
		if( _catgs.size() )
			_catRelOffset = dbStruct.m_catgs.Add(_catgs);
	}

	return _catRelOffset;
}

static void _PrepareUrl(std::string& url)
{
	if( url.empty() )
		return;

	_strlwr(&url[0]);

	// Восстанавливаем '='
	char * r;
	char * b = (char *)url.c_str();
	char * p = b;
	while(r = strstr(p, "%3d"))
	{
		int nPos = r - b;
		url.erase(nPos, sizeof("%3d") - 1);
		url.insert(nPos, "=");
		
		b = (char *)url.c_str();
		p = b + nPos + 1;
	}
}

bool ReadSrc(CIni& ini, DBStructure& dbStruct)
{
	sIniSection * pUrlBlack = ini.GetSection("Sites.black");
	sIniSection * pUrlWhite = ini.GetSection("Sites.white");
	sIniSection * pWords    = ini.GetSection("Words");

	sIniLine * pLine;

	if( pUrlBlack )
	{
		for(pLine = pUrlBlack->line_head; pLine; pLine = pLine->next)
		{
			DBStructure::Url& _url = *dbStruct.m_urlsBlack.insert(dbStruct.m_urlsBlack.end(), DBStructure::Url());
			_url.m_name = pLine->name;
			_PrepareUrl(_url.m_name);
			_url.m_catRelOffset = ReadActgOffset(pLine->value, dbStruct);
		}
	}

	if( pUrlWhite )
	{
		for(pLine = pUrlWhite->line_head; pLine; pLine = pLine->next)
		{
			DBStructure::Url& _url = *dbStruct.m_urlsWhite.insert(dbStruct.m_urlsWhite.end(), DBStructure::Url());
			_url.m_name = pLine->name;
			_PrepareUrl(_url.m_name);
			_url.m_catRelOffset = -1;
		}
	}

	if( pWords )
	{
		std::wstring _word;
		for(pLine = pWords->line_head; pLine; pLine = pLine->next)
		{
			_A2W(pLine->name, _word, CP_UTF8);
			if( _word.size() )
				_wcslwr(&_word[0]);
			dbStruct.m_words.Add(_word, ReadActgOffset(pLine->value, dbStruct));
		}
	}
	
	return true;
}

uint32_t WriteDb(DBStructure& dbStruct, uint8_t * buf)
{
	uint8_t * p = buf;
	
	_ParctlDbHdr * _hdr = (_ParctlDbHdr *)p;
	p += sizeof(_ParctlDbHdr);
	
	if( buf )
	{
		memcpy(_hdr->m_magic, "UFDB", sizeof(_hdr->m_magic));
		_WriteInt32(_hdr->m_ver, _MAKEDWORD(1, 1));
	}

	// Write categories ranges
	{
		uint8_t * _catgsBegin = p;
		
		if( buf )
			_WriteInt32(_hdr->m_catgsOffset, _catgsBegin - buf);

		for(uint32_t i = 0; i < dbStruct.m_catgs.size(); i++)
		{
			_ParctlCatgRanges * _catgs = (_ParctlCatgRanges *)p;
			uint8_t _catgs_item_n = dbStruct.m_catgs[i].size();
			if( buf )
				_catgs->m_n = _catgs_item_n;
			p += sizeof(_catgs_item_n);
		
			if( buf )
			{
				for(uint8_t k = 0; k < _catgs_item_n; k++)
				{
					_ParctlCatgRange * _c = (_ParctlCatgRange *)&_catgs->m_a[k];
					_c->m_id = dbStruct.m_catgs[i][k].m_id;
					_c->m_weight = dbStruct.m_catgs[i][k].m_weight;
				}
			}

			p += _catgs_item_n*sizeof(_ParctlCatgRange);
		}

		if( buf )
			_WriteInt32(_hdr->m_catgsSize, p - _catgsBegin);
	}

	// Write words
	{
		if( buf )
			_WriteInt32(_hdr->m_wordsOffset, p - buf);

		uint32_t size = dbStruct.m_words.Write(buf ? p : NULL);
		if( size == -1 )
			return -1;
		p += size;

		if( buf )
			_WriteInt32(_hdr->m_wordsSize, size);
	}

	// Write urls
	{
		_ParctlDbUrls * _urlsTmp[2] = {&_hdr->m_urlsBlack, &_hdr->m_urlsWhite};
		DBStructure::Urls * _urlsDbTmp[2] = {&dbStruct.m_urlsBlack, &dbStruct.m_urlsWhite};
		
		for(int u = 0; u < countof(_urlsTmp); u++)
		{
			_ParctlDbUrls * __urls = _urlsTmp[u];
			DBStructure::Urls& _urlsDb = *_urlsDbTmp[u];

			// Write urls
			{
				uint8_t * urlsBegin = p;

				if( buf )
				{
					_WriteInt32(__urls->m_offset, p - buf);
					_WriteInt32(__urls->m_count, _urlsDb.size());
				}

				for(uint32_t i = 0; i < _urlsDb.size(); i++)
				{
					const char * _url = _urlsDb[i].m_name.c_str();
					int len = strlen(_url) + 1;

					if( buf )
						memcpy(p, _url, len);
					p += len;
				}

				if( buf )
				{
					_WriteInt32(__urls->m_size, p - urlsBegin);
					_ParctlXorBuf(urlsBegin, p - urlsBegin, true, false);
				}
			}

			// Write hashed urls
			{
				if( buf )
				{
					_WriteInt32(__urls->m_hashesOffset, p - buf);
					_WriteInt32(__urls->m_hashesSize, 0);
				}
			}
		}
	}

	// Write urls categories offsets
	{
		uint8_t * _urlsCatgsBegin = p;

		if( buf )
			_WriteInt32(_hdr->m_urlsCatgsOffset, p - buf);

		uint32_t i;
		
		for(i = 0; i < dbStruct.m_urlsBlack.size(); i++)
		{
			if( buf )
				_WriteInt32(p, dbStruct.m_urlsBlack[i].m_catRelOffset);

			p += sizeof(uint32_t);
		}

// 		for(i = 0; i < dbStruct.m_urlsBlackHashes.size(); i++)
// 		{
// 			if( buf )
// 				_WriteInt32(p, dbStruct.m_urlsBlackHashes[i].m_catRelOffset);
// 
// 			p += sizeof(uint32_t);
// 		}

		for(i = 0; i < dbStruct.m_urlsWhite.size(); i++)
		{
			if( buf )
				_WriteInt32(p, dbStruct.m_urlsWhite[i].m_catRelOffset);

			p += sizeof(uint32_t);
		}

// 		for(i = 0; i < dbStruct.m_urlsWhiteHashes.size(); i++)
// 		{
// 			if( buf )
// 				_WriteInt32(p, dbStruct.m_urlsWhiteHashes[i].m_catRelOffset);
// 
// 			p += sizeof(uint32_t);
// 		}

		if( buf )
			_WriteInt32(_hdr->m_urlsCatgsSize, p - _urlsCatgsBegin);
	}

	return p - buf;
}

// ########################################################################
// ########################################################################

int wmain(int argc, wchar_t *argv[])
{
	printf("PARCTL_MKDB version 1.0 (C) 2007 Kaspersky labs.\n\n");

	if( argc < 3 )
	{
		fwprintf(stderr, L"Wrong number of arguments!\n");
		fwprintf(stderr, L"Usage: PARCTL_MKDB <src_file> <db_file>\n");
		return E_FAIL;
	}

	const wchar_t * srcFileName = argv[1];
	const wchar_t * dbFileName = argv[2];

	fwprintf(stdout, L"Reading source file \"%s\"...\n", srcFileName);
	
	DBStructure dbStruct;
	HRESULT hr = S_OK;
	{
		CIni _dbText;
		if( FAILED(hr = _dbText.Open(srcFileName)) )
		{
			fwprintf(stderr, L"Error open source file: 0x%08x\n", hr);
			return hr;
		}
		
		if( !ReadSrc(_dbText, dbStruct) )
		{
			fwprintf(stderr, L"Error reading file!\n");
			return E_FAIL;
		}
	}

	fwprintf(stdout, L"Writing destination database \"%s\"...\n", dbFileName);
	tBin dbBody;
	{
		uint32_t size = WriteDb(dbStruct, NULL);
		if( size == -1 )
		{
			fwprintf(stderr, L"Error!\n");
			return E_FAIL;
		}

		dbBody.resize(size);
		if( dbBody.size() != size )
		{
			fwprintf(stderr, L"Error: Not enough system memory\n");
			return E_OUTOFMEMORY;
		}

		memset(&dbBody.front(), 0, size);
		size = WriteDb(dbStruct, &dbBody.front());
		if( size == -1 )
		{
			fwprintf(stderr, L"Error!\n");
			return E_FAIL;
		}
	}

	// write output buffer to file
	if( dbBody.size() )
	{
		HANDLE hFile = ::CreateFileW(dbFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN|FILE_ATTRIBUTE_NORMAL, NULL);
		if( hFile == INVALID_HANDLE_VALUE )
		{
			fwprintf(stderr, L"Can't open file\n");
			return E_FAIL;
		}

		// Write body
		DWORD dwWritten;
		if( !::WriteFile(hFile, &dbBody[0], dbBody.size(), &dwWritten, NULL) )
		{
			fwprintf(stderr, L"Error writing file\n");
			::CloseHandle(hFile);
			return E_FAIL;
		}

		::CloseHandle(hFile);
	}

	return S_OK;
}

// ########################################################################
// ########################################################################
