// treesearchtest.cpp
//

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>

#if defined (_WIN32)
#define READ_FLAGS O_RDONLY|O_BINARY
#define WRITE_FLAGS O_RDWR|O_CREAT|O_TRUNC|O_BINARY
#define ACCESS_RIGHTS S_IREAD|S_IWRITE
#include <io.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#define READ_FLAGS O_RDONLY
#define WRITE_FLAGS O_RDWR|O_CREAT|O_TRUNC
#define ACCESS_RIGHTS S_IRUSR | S_IWUSR 
#endif

#define PROMPT_STRING ">"

#if defined (_MSC_VER)
#pragma warning(disable: 4786)
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <iomanip>

#include <klava/kdb_utils.h>

#include "treesearchtest.h"

void* _TREE_CALL Alloc(void*, unsigned int dwBytes);
void* _TREE_CALL Realloc(void*, void* pMem, unsigned int dwBytes);
void  _TREE_CALL Free(void*, void* pMem);

KLAV_ERR _TREE_CALL Read(void* pIO, uint32_t* pdwRead, uint8_t* pbBuffer, uint32_t dwSize);
KLAV_ERR _TREE_CALL Write(void* pIO, uint32_t* pdwWritten, uint8_t* pbBuffer, uint32_t dwSize);
KLAV_ERR _TREE_CALL SeekRead(void* pIO, uint32_t* pdwRead, uint64_t qwOffset, uint8_t* pbBuffer, uint32_t dwSize);
KLAV_ERR _TREE_CALL SeekWrite(void* pIO, uint32_t* pdwWritten, uint64_t qwOffset, uint8_t* pbBuffer, uint32_t dwSize);
KLAV_ERR _TREE_CALL Seek(void* pIO, uint64_t* pqwNewPosition, uint64_t qwPosition, uint32_t dwOrigin);

//std::ostream& formatted_hex(std::ostream& os)
//{
//	return os << "0x"<< std::setw(8) << std::setfill('0') << std::uppercase << std::hex;
//}

template <class Ch, class Tr>
std::basic_ostream <Ch,Tr>& formatted_hex(std::basic_ostream<Ch,Tr>& os)
{
	return os << "0x"<< std::setw(8) << std::setfill('0') << std::uppercase << std::hex;
}

const unsigned _default_start_val = 0x6a4738f;

////////////////////////////////////////////////////////////////////////////////
void* _TREE_CALL Alloc(void*, unsigned int dwBytes)
{ return calloc(sizeof(char), dwBytes); }

void* _TREE_CALL Realloc(void*, void* pMem, unsigned int dwBytes)
{ return realloc(pMem, dwBytes); }

void _TREE_CALL Free(void*, void* pMem)
{ free(pMem); }

////////////////////////////////////////////////////////////////////////////////
KLAV_ERR _TREE_CALL Read(void* pIO, uint32_t* pdwRead, uint8_t* pbBuffer, uint32_t dwSize)
{
	int aFD = (int)(pIO);
	KLAV_ERR anError = KLAV_OK;
	int aCount = read(aFD, pbBuffer, dwSize);
	if(aCount < 0) return KLAV_EUNEXPECTED;
	if(pdwRead) *pdwRead =  aCount;
	return anError;
}

////////////////////////////////////////////////////////////////////////////////
KLAV_ERR _TREE_CALL Write(void* pIO, uint32_t* pdwWritten, uint8_t* pbBuffer, uint32_t dwSize)
{
	int aFD = (int)(pIO);
	KLAV_ERR anError = KLAV_OK;
	int aCount = write(aFD, pbBuffer, dwSize);
	if(aCount < 0) return KLAV_EUNEXPECTED;
	if(pdwWritten) *pdwWritten =  aCount;
	return anError;
}

////////////////////////////////////////////////////////////////////////////////
KLAV_ERR _TREE_CALL SeekRead(void* pIO, uint32_t* pdwRead, uint64_t qwOffset,
                           uint8_t* pbBuffer, uint32_t dwSize)
{
	int aFD = (int)(pIO);
	off_t aCurrentPosition = lseek(aFD, 0, SEEK_CUR);
	if(aCurrentPosition < 0) return KLAV_EUNEXPECTED;
	lseek(aFD, (long)qwOffset, SEEK_SET);
	int aCount = read(aFD, pbBuffer, dwSize);
	lseek(aFD, (long)aCurrentPosition, SEEK_SET);
	if(aCount < 0) return KLAV_EUNEXPECTED;
	if(pdwRead) *pdwRead = aCount;
	return KLAV_OK;
}

////////////////////////////////////////////////////////////////////////////////
KLAV_ERR _TREE_CALL SeekWrite(void* pIO, uint32_t* pdwWritten, uint64_t qwOffset,
                            uint8_t* pbBuffer, uint32_t dwSize)
{
	int aFD = (int)(pIO);
	off_t aCurrentPosition = lseek(aFD, 0, SEEK_CUR);
	if(aCurrentPosition < 0) return KLAV_EUNEXPECTED;
	lseek(aFD, (long)qwOffset, SEEK_SET);
	int aCount = write(aFD, pbBuffer, dwSize);
	lseek(aFD, (long)aCurrentPosition, SEEK_SET);
	if(aCount < 0) return KLAV_EUNEXPECTED;
	if(pdwWritten) *pdwWritten = aCount;
	return KLAV_OK;
}

////////////////////////////////////////////////////////////////////////////////
KLAV_ERR _TREE_CALL Seek(void* pIO, uint64_t* pqwNewPosition, uint64_t qwPosition,
                       uint32_t dwOrigin)
{
	int aFD = (int)(pIO);
	int aWhence = 0;
	switch(dwOrigin)
	{
	case TREE_IO_CTX_SEEK_CUR:
		aWhence = SEEK_CUR;
		break;
	case TREE_IO_CTX_SEEK_SET:
		aWhence = SEEK_SET;
		break;
	case TREE_IO_CTX_SEEK_END:
		aWhence = SEEK_END;
		break;
	default:
		return KLAV_EUNEXPECTED;
	}
	off_t aPosition = lseek(aFD, (long)qwPosition, aWhence);
	if(aPosition < 0) return KLAV_EUNEXPECTED;
	if(pqwNewPosition) *pqwNewPosition = aPosition;
	return KLAV_OK;
}

////////////////////////////////////////////////////////////////////////////////
#include <memory.h>

class test_exception : public std::exception
{
public:

	virtual ~test_exception() throw() { delete[] m_msg; }

	const char * what () const throw () { return m_msg; }

	bool fatal() const { return m_fatal; }
	int32_t error() const { return m_error; }

	static void raise (const char *msg, uint32_t error = 0, bool fatal = false)
	{
		throw test_exception (msg, error, fatal);
	}

	static void raise (const std::string& msg, uint32_t error = 0, bool fatal = false)
	{
		raise(msg.c_str(), error, fatal);
	}

protected:

	test_exception (const char *msg, uint32_t error = 0, bool fatal = false)
		: m_msg (0), m_error (error), m_fatal (fatal)
	{
		if (msg == 0) msg = "";
		int len = strlen (msg);

		m_msg = new char [len+1];
		::memcpy (m_msg, msg, len+1);
	}

	char *m_msg;
	uint32_t m_error;
	bool m_fatal;
};

////////////////////////////////////////////////////////////////////////////////
//
// FileMappingHolder
//
////////////////////////////////////////////////////////////////////////////////
template<class T> class FileMappingHolder
{
public:
	typedef T* ptr_t;

	FileMappingHolder(const std::string& FileName)
	{ init(FileName.c_str()); }

	FileMappingHolder(const char*FileName)
	{ init(FileName); }

	void init(const char*FileName)
	{
		if(FileMap.create(FileName, KL_FILEMAP_READ))
			test_exception::raise(std::string("Can't load file \"") + FileName + std::string("\"."));

		// mapping target file
		MappingSize = 0;
		MappedBuffer = reinterpret_cast<ptr_t>(FileMap.map(0, 0, &MappingSize));
		if(! MappedBuffer || ! MappingSize)
			test_exception::raise(std::string("Can't map file \"") + FileName + std::string("\"."));
	}

	~FileMappingHolder()
	{
		FileMap.unmap(MappedBuffer, MappingSize);
	}

	const ptr_t get() const { return MappedBuffer; }
	FileMapping::kl_size_t size() const { return MappingSize; }

private:
	FileMapping FileMap;
	FileMapping::kl_size_t MappingSize;
	ptr_t MappedBuffer;

	FileMappingHolder(const FileMappingHolder&);
	FileMappingHolder& operator=(const FileMappingHolder&);
};


#include <stdlib.h>
#include <time.h>
////////////////////////////////////////////////////////////////
// integer random generator
static int random (uint32_t a, uint32_t b)
{
	if (a > b)
	{
		int c = a;
		a = b;
		b = c;
	}

	double x = a + rand() * (double)(b - a) / RAND_MAX;
	x = x - floor(x) < 0.5 ? floor(x) : ceil(x);
	return (int)x;
}


////////////////////////////////////////////////////////////////
//
// Signature generator
//
////////////////////////////////////////////////////////////////
struct SignatureGenerator
{
	virtual void get(std::vector<uint8_t>& sig) const = 0;
	virtual uint8_t get() const = 0;
};

class SignatureGeneratorImpl : public SignatureGenerator
{
public:
	enum
	{
		SIGNATURE_MIN_LEN = 4,
		SIGNATURE_MAX_LEN = 128,
	};

	SignatureGeneratorImpl(uint32_t minLen = SIGNATURE_MIN_LEN,
						   uint32_t maxLen = SIGNATURE_MAX_LEN,
						   uint32_t seed = (uint32_t)time(0))
	{
		m_minLen = minLen;
		m_maxLen = maxLen;

		srand(seed);
	}

	virtual void get(std::vector<uint8_t>& sig) const
	{
		sig.clear();

		int n = random(m_minLen, m_maxLen);
		while (n--) sig.push_back(get());
	}

	virtual uint8_t get() const { return (uint8_t)rand();}

protected:
	uint32_t m_minLen, m_maxLen;
};


#include <ctype.h>
struct AlphaSignatureGeneratorImpl : public SignatureGeneratorImpl
{
	virtual uint8_t get() const
	{
		int c;
		while (! isalpha(c = random('A', 'z')));
		return (uint8_t)c;
	}
};



////////////////////////////////////////////////////////////////
//
// Signature
//
////////////////////////////////////////////////////////////////

class Signature
{
public:

	// !!!!!!!!!!!!!!!!!!!!!
	void init_ex(const uint8_t *pBuffer, size_t nBufferSize);

	void init(const std::vector<uint8_t>& buffer);
	void init(const uint8_t *pSignature, size_t SignatureSize);
	void init(SignatureGenerator& sgen);
	void init(KDBToken token, const uint8_t *pSignature, size_t SignatureSize);

	void set(int pos = -1, int val = -1)
	{
		if (empty()) return;
		if (pos < 0) pos = random (0, size()-1);
		if (val < 0) val = (uint8_t)rand();
		m_binSignature[pos] = val;
	}

	void set(SignatureGenerator& sgen, size_t pos = -1)
	{
		if (empty()) return;

		std::vector<uint8_t> rawSignature;
		ConvertSignature2Raw(&m_binSignature.front(), m_binSignature.size(), rawSignature);

		if (pos == static_cast<size_t>(-1)) pos = random (0, rawSignature.size()-1);
		rawSignature[pos] = sgen.get();

		ConvertRaw2Signature(&rawSignature.front(), rawSignature.size(), m_binSignature);
	}

	const Signature& operator =(const Signature& s)
		{ if(&s != this) { m_binSignature = s.m_binSignature; } return *this; }

	bool operator <(const Signature& s) const
		{ return m_binSignature < s.m_binSignature; }

	bool operator ==(const Signature& s) const
		{ return m_binSignature == s.m_binSignature; }

	const uint8_t* ptr() const { return reinterpret_cast<const uint8_t*>(&m_binSignature.front()); }
	size_t size() const { return m_binSignature.size(); }
	bool empty() const { return !size(); }

private:

	std::vector<uint8_t> m_binSignature;

	friend std::ostream& operator<<(std::ostream& s, const Signature& t);

 static int CvtBinHexSignature2BinSignature(const uint8_t *pSignature,
											size_t nSignatureSize,
											std::vector<unsigned char>& binSignature);
 static void ConvertRaw2Signature(const uint8_t* pBuffer, size_t nBufferSize,
								  std::vector<uint8_t>& binSignature);
 static void ConvertSignature2Raw(const uint8_t* pBuffer, size_t nBufferSize,
								  std::vector<uint8_t>& rawSignature);
};

////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& s, const Signature& sig)
{
	s << '{';

	std::vector<uint8_t>::const_iterator ibeg = sig.m_binSignature.begin();
	const std::vector<uint8_t>::const_iterator eter = sig.m_binSignature.end();

	for (std::vector<uint8_t>::const_iterator iter = ibeg; iter != eter; ++iter)
	{
		if (iter != ibeg) s << ' ';
		uint32_t c = static_cast<uint32_t>(*iter);
		s << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << c;
	}
	return s << '}';
}

////////////////////////////////////////////////////////////////////////////////
void Signature::ConvertRaw2Signature(const uint8_t* pBuffer, size_t nBufferSize,
									 std::vector<uint8_t>& binSignature)
{
	uint32_t binSignatureSize = nBufferSize;
	binSignature.resize(binSignatureSize);

	KLAV_ERR error = TreeConvertRaw2Signature(pBuffer,
									 nBufferSize,
									 reinterpret_cast<uint8_t*>(&binSignature.front()),
									 binSignature.size(),
									 &binSignatureSize);
	if (error == KLAV_ESMALLBUF)
	{
		binSignature.resize(binSignatureSize);
		error = TreeConvertRaw2Signature(pBuffer,
										 nBufferSize,
										 reinterpret_cast<uint8_t*>(&binSignature.front()),
										 binSignature.size(),
										 &binSignatureSize);
	}

	if (KLAV_FAILED(error))
		test_exception::raise("Can't convert raw to signature.");
}

////////////////////////////////////////////////////////////////////////////////
void Signature::ConvertSignature2Raw(const uint8_t* pBuffer, size_t nBufferSize,
									 std::vector<uint8_t>& rawSignature)
{
	uint32_t rawSignatureSize = nBufferSize;
	rawSignature.resize(rawSignatureSize);

	KLAV_ERR error = TreeConvertSignature2Raw(pBuffer,
									 nBufferSize,
									 reinterpret_cast<uint8_t*>(&rawSignature.front()),
									 rawSignature.size(),
									 &rawSignatureSize);
	if (error == KLAV_ESMALLBUF)
	{
		rawSignature.resize(rawSignatureSize);
		error = TreeConvertRaw2Signature(pBuffer,
										 nBufferSize,
										 reinterpret_cast<uint8_t*>(&rawSignature.front()),
										 rawSignature.size(),
										 &rawSignatureSize);
	}

	if (KLAV_FAILED(error))
		test_exception::raise("Can't convert signature to raw.");
}

////////////////////////////////////////////////////////////////////////////////
// !!!!!!!!!!!!!!!!!!!!!!!
void Signature::init_ex(const uint8_t *pBuffer, size_t nBufferSize)
{
	m_binSignature.assign(pBuffer, pBuffer + nBufferSize);
}

////////////////////////////////////////////////////////////////////////////////
void Signature::init(const uint8_t *pBuffer, size_t nBufferSize)
{
	ConvertRaw2Signature(pBuffer, nBufferSize, m_binSignature);
//	m_binSignature.assign(pSignature, pSignature + SignatureSize);
}

void Signature::init(const std::vector<uint8_t>& buffer)
{
	ConvertRaw2Signature(&buffer.front(), buffer.size(), m_binSignature);
//	m_binSignature = buffer;
}

////////////////////////////////////////////////////////////////////////////////
void Signature::init(SignatureGenerator& gen)
{
	std::vector<uint8_t> raw;
	gen.get(raw);
	ConvertRaw2Signature(&raw.front(), raw.size(), m_binSignature);
}

////////////////////////////////////////////////////////////////////////////////
void Signature::init(KDBToken token, const uint8_t *pSignature, size_t SignatureSize)
{
	KLAV_ERR error;
	m_binSignature.resize(0);

	switch (token)
	{
	case KDB_STR:
	{		
		uint32_t binSignatureSize = 0;
		uint8_t doomy = 0;
		error = TreeConvertHex2Signature(reinterpret_cast<const uint8_t*>(pSignature),
										 SignatureSize,
										 &doomy/* reinterpret_cast<uint8_t*>(&m_binSignature.front())*/,
										 0, &binSignatureSize);
		if (error == KLAV_ESMALLBUF)
		{
			m_binSignature.resize(binSignatureSize);
			error = TreeConvertHex2Signature(reinterpret_cast<const uint8_t*>(pSignature),
											 SignatureSize,
											 reinterpret_cast<uint8_t*>(&m_binSignature.front()),
											 binSignatureSize, &binSignatureSize);
		}
		if (KLAV_FAILED(error))
		{
			std::stringstream es;
			es << "Can't convert signature ";
			es << std::string(reinterpret_cast<const char*>(pSignature), SignatureSize);
			es << ". Error: " << formatted_hex << error << '.';
			test_exception::raise(es.str());
		}
		break;
    }

	case KDB_BIN:
	{
		error = CvtBinHexSignature2BinSignature(pSignature, SignatureSize, m_binSignature);
		if (KLAV_FAILED(error))
		{
			std::stringstream es;
			es << "Can't convert signature ";
			es << std::string(reinterpret_cast<const char*>(pSignature), SignatureSize) << '.';
			test_exception::raise(es.str());
		}
		break;
	}

	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////
int Signature::CvtBinHexSignature2BinSignature(const uint8_t *pSignature,
											   size_t nSignatureSize,
											   std::vector<uint8_t>& binSignature)
{
	if (nSignatureSize % 2) return -1;
	for (size_t i = 0; i < nSignatureSize; )
	{
		int val = 0, n = 0;
		uint8_t ch = pSignature[i++];

		if (ch >= '0' && ch <= '9') n = ch - '0';
		else if (ch >= 'A' && ch <= 'F') n = ch -'A' + 10;
		else if (ch >= 'a' && ch <= 'f') n = ch -'a' + 10;
		else return -1;
		val = n;

		ch = pSignature[i++];
		if (ch >= '0' && ch <= '9') n = ch - '0';
		else if (ch >= 'A' && ch <= 'F') n = ch -'A' + 10;
		else if (ch >= 'a' && ch <= 'f') n = ch -'a' + 10;
		else return -1;
		val = (val << 4) | n;

		binSignature.push_back((uint8_t)val);
	}
	return 0;
}



////////////////////////////////////////////////////////////////////////////////
//
// Signature tree
//
////////////////////////////////////////////////////////////////////////////////

class TreeSignature
{
public:

	enum { DefaultHashLevel = 16, DefaultdwHashTableSizeKb = 64 };
	enum Action {COMMAND_ADD_SIGNATURE = 1, COMMAND_DEL_SIGNATURE = 2};
	enum TreeType
	{
		UnknownTreeType = 0,
		DynamicTreeType = cFORMAT_DYNAMIC,
		StaticTreeType  = cFORMAT_STATIC
	};

	TreeSignature();
	~TreeSignature();

	void Load(TreeType ttype, const std::string& FileName,
			  const std::string& SignatureFileName = std::string());

	void Save2Static(const std::string& FileName,
					 uint32_t HashLevel = DefaultHashLevel,
					 uint32_t dwHashTableSizeKb = DefaultdwHashTableSizeKb);
	void Save2Dynamic(const std::string& FileName);

	size_t Size() const { return m_sigs.size(); }
	void Clear();

	bool IsEmpty() const;
	bool IsStatic() const;
	bool IsDynamic() const;
	int GetType() const;

	void ProcessSignaturesFromFile(Action action, const std::string& FileName);

	void AddSignature(const Signature& sig, tSIGNATURE_ID SignatureID = 0);
	void AddParsingSignature(const uint8_t* pBuffer, size_t BufferSize);

	void DelSignature(const Signature& sig);
	void DelParsingSignature(const uint8_t* pBuffer, size_t nBufferSize);

	void Dump() const;

private:

	friend class TreeScanner;
	friend std::ostream& operator<<(std::ostream& s, const TreeSignature& t);

	typedef std::map<tSIGNATURE_ID, Signature> SignatureMap;
	SignatureMap m_sigs;

	sHEAP_CTX m_heap_ctx;
	sRT_CTX *m_tree_ctx;

	void GetInfo(TREE_INFO* pTreeInfo) const;

	void GetSignatureFromDynamicTree();
	static KLAV_ERR _TREE_CALL TreeEnumCallbackFunc(TreeSignature* self,
												  tSIGNATURE_ID SignatureID,
												  const uint8_t* pSignature,
												  uint32_t dwSignatureSize);

	void ProcessingBuffer(Action action, const uint8_t* pBuffer, size_t nBufferSize);

	typedef void (*tParseBufferCallbackFunc) (const Signature&, tSIGNATURE_ID SignatureID, TreeSignature* self);
	void ParseBuffer(const uint8_t *pBuffer, size_t nBufferSize, tParseBufferCallbackFunc func, TreeSignature* self);

	static void AddSignature2Map(const Signature& sig, tSIGNATURE_ID SignatureID, TreeSignature* self);
	static void AddSignature(const Signature& sig, tSIGNATURE_ID SignatureID, TreeSignature* self);
	static void DelSignature(const Signature& sig, tSIGNATURE_ID SignatureID, TreeSignature* self);

	TreeSignature(const TreeSignature&);
	TreeSignature& operator = (const TreeSignature&);
};

////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& s, const TreeSignature& t)
{
	size_t size = t.m_sigs.size();
	TreeSignature::SignatureMap::const_iterator iter = t.m_sigs.begin();
	TreeSignature::SignatureMap::const_iterator eter = t.m_sigs.end();
	for (int i=0; iter != eter; ++iter, ++i)
	{
		tSIGNATURE_ID SignatureID = iter->first;
		Signature sig = iter->second;

		s << std::setw(4) << std::setfill(' ') << std::right << std::dec << i << '/' << size << "; ";
		s << SignatureID << "; " << sig << std::endl;
	}
	return s;
}

////////////////////////////////////////////////////////////////////////////////
TreeSignature::TreeSignature()
{
	m_heap_ctx.pHeap = 0;
	m_heap_ctx.Alloc = Alloc;
	m_heap_ctx. Realloc = Realloc;
	m_heap_ctx.Free = Free;

	KLAV_ERR error = TreeInit(&m_heap_ctx, &m_tree_ctx);
	if (KLAV_FAILED(error))
	{
		std::stringstream es;
		es << "Can't initialize tree. Error " << formatted_hex << error;
		test_exception::raise(es.str());
	} 
}

////////////////////////////////////////////////////////////////////////////////
TreeSignature::~TreeSignature()
{
	Clear();
	TreeDone(m_tree_ctx);
}

////////////////////////////////////////////////////////////////////////////////
void TreeSignature::Clear()
{
	TreeUnload_Any(m_tree_ctx);
	m_sigs.clear();
}

////////////////////////////////////////////////////////////////////////////////
void TreeSignature::GetInfo(TREE_INFO* pTreeInfo) const
{
	memset(pTreeInfo, 0, sizeof(TREE_INFO));

	KLAV_ERR error = TreeGetInfo(m_tree_ctx, pTreeInfo);
	if (KLAV_FAILED(error))
	{
		if (error != KLAV_ENOINIT)
		{
			std::stringstream es;
			es << "Can't get tree info. Error " << formatted_hex << error;
			test_exception::raise(es.str(), error);
		}
	} 
}

////////////////////////////////////////////////////////////////////////////////
int TreeSignature::GetType() const
{
	TREE_INFO TreeInfo;
	GetInfo (&TreeInfo);
	return TreeInfo.dwFormat;
}

////////////////////////////////////////////////////////////////////////////////
bool TreeSignature::IsStatic() const
{
	return GetType() == cFORMAT_STATIC;
}

bool TreeSignature::IsDynamic() const
{
	return GetType() == cFORMAT_DYNAMIC;
}

bool TreeSignature::IsEmpty() const
{
	return GetType() == 0;
}

////////////////////////////////////////////////////////////////////////////////
void TreeSignature::ParseBuffer(const uint8_t *pBuffer, size_t nBufferSize,
								tParseBufferCallbackFunc func, TreeSignature* self)
{
	KDBParser Parser;
	KDBParser_Init(&Parser, pBuffer, nBufferSize);

	for ( ; ! KDBParser_EOF(&Parser); KDBParser_NextLine(&Parser))
	{
		KDBToken token = KDBParser_GetToken(&Parser);
		if(token != KDB_INT)
		{
//			test_exception::raise("Error: incorrect signature ID format");
			continue;
		}
		tSIGNATURE_ID SignatureID = KDBParser_IntValue(&Parser);

		token = KDBParser_GetToken(&Parser);
		if(token != KDB_STR)
		{
//			test_exception::raise("Error: incorrect signature format");
			continue;
		}

		size_t SignatureSize = 0;
		const uint8_t *pSignature = reinterpret_cast<const uint8_t*>(
									KDBParser_StrValue(&Parser, &SignatureSize));
		Signature sig;
		sig.init(token, pSignature, SignatureSize);

		if (func)
		{
			(*func)(sig, SignatureID, self);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void TreeSignature::AddSignature2Map(const Signature& sig,
									 tSIGNATURE_ID sig_id,
									 TreeSignature* self)
{
	if (self)
	{
		self->m_sigs.insert(std::pair<tSIGNATURE_ID, Signature> (sig_id, sig));
	}
}

////////////////////////////////////////////////////////////////////////////////
void TreeSignature::Load(TreeType ttype,
						 const std::string& FileName,
						 const std::string& SignatureFileName)
{ 
	if (IsStatic())
		test_exception::raise("Can't load dynamic tree over static tree.");

	int fd = open(FileName.c_str (), READ_FLAGS);
	if (fd < 0)
	{
		test_exception::raise(std::string("Can't open file \"") + FileName);
	}

	sIO_CTX IOContext = {(void*)fd, Read, Write, SeekRead, SeekWrite, Seek};
	KLAV_ERR error = KLAV_OK;
	switch (ttype)
	{
	case DynamicTreeType:
		error = TreeLoad_Dynamic(m_tree_ctx, &IOContext);
		break;

	case StaticTreeType:
		error = TreeLoad_Static(m_tree_ctx, &IOContext);
		break;

	case UnknownTreeType:
		error = TreeLoad_Any(m_tree_ctx, &IOContext);
		break;
	}

	close(fd);
	if (KLAV_FAILED(error))
	{
		std::stringstream es;
		es << "Can't load tree. Error " << formatted_hex << error << '.';
		test_exception::raise(es.str());
	}

	if (SignatureFileName.size())
	{
		FileMappingHolder<uint8_t> map(SignatureFileName);
		ParseBuffer(map.get(), map.size(), (tParseBufferCallbackFunc)AddSignature2Map, this);
	}
	else if (IsDynamic())
	{
		GetSignatureFromDynamicTree();
	}
}

////////////////////////////////////////////////////////////////////////////////
void TreeSignature::Save2Static(const std::string& FileName,
								uint32_t HashLevel, uint32_t dwHashTableSizeKb)
{
	int fd = open(FileName.c_str(), WRITE_FLAGS, ACCESS_RIGHTS);
	if (fd < 0)
	{
		test_exception::raise(std::string("Can't open file \"") + FileName +
							  std::string("\" to save static tree."));
	}

	sIO_CTX IOContext = {(void*)fd, Read, Write, SeekRead, SeekWrite, Seek};
	KLAV_ERR error = TreeSave_Static(m_tree_ctx, &IOContext, HashLevel, dwHashTableSizeKb);
	close(fd);
	if (KLAV_FAILED(error))
	{
		std::stringstream es;
		es << "Can't save static tree. Error " << formatted_hex << error << '.';
		test_exception::raise(es.str());
	}
}

////////////////////////////////////////////////////////////////////////////////
void TreeSignature::Save2Dynamic(const std::string& FileName)
{
	if (IsStatic())
	{
		test_exception::raise("Can't convert static tree into dynamic tree.");
	}

	int fd = open(FileName.c_str(), WRITE_FLAGS, ACCESS_RIGHTS);
	if (fd < 0)
	{
		std::string es("Can't open file \"");
		es += FileName;
		es += "\" to save dynamic tree.";
		test_exception::raise(es);
	}

	sIO_CTX IOContext = {(void*)fd, Read, Write, SeekRead, SeekWrite, Seek};
	KLAV_ERR error = TreeSave_Dynamic(m_tree_ctx, &IOContext);
	close(fd);
	if (KLAV_FAILED(error))
	{
		std::stringstream es;
		es << "Can't save dynamic tree. Error " << formatted_hex << error << '.';
		test_exception::raise(es.str());
	} 
}

////////////////////////////////////////////////////////////////////////////////
void TreeSignature::ProcessSignaturesFromFile(Action action, const std::string& FileName)
{
	if (IsStatic())
	{
		test_exception::raise("Can't modify static tree.");
	}

	FileMappingHolder<uint8_t> map(FileName);
	switch (action)
	{
	case COMMAND_ADD_SIGNATURE :
		AddParsingSignature(map.get(), map.size());
		break;

	case COMMAND_DEL_SIGNATURE :
		DelParsingSignature(map.get(), map.size());
		break;
	}
	return;
}

////////////////////////////////////////////////////////////////////////////////
void TreeSignature::AddSignature(const Signature& sig, tSIGNATURE_ID sig_id,
								 TreeSignature* self)
{
	self->AddSignature(sig, sig_id);
}

////////////////////////////////////////////////////////////////////////////////
void TreeSignature::AddSignature(const Signature& sig, tSIGNATURE_ID sig_id)
{
	if (sig_id == 0)
	{
		TREE_INFO TreeInfo;
		GetInfo(&TreeInfo);
		sig_id = TreeInfo.MaxSignatureID + 1;
	}

	tSIGNATURE_ID result_sig_id;
	KLAV_ERR error = TreeAddSignature(m_tree_ctx, sig.ptr(), sig.size(),
									sig_id, &result_sig_id);
	if (KLAV_FAILED(error))
	{
		std::stringstream es;
		if (error == KLAV_EDUPLICATE)
		{
			es << "Signature already exists. " << std::endl;
		}
		else
		{
			es << "Can't add signature into the tree. ";
			es << "Error " << formatted_hex << error << ".\n" << std::endl;
		}

		es << "id=" << std::dec << sig_id;
		es << " "   << sig;

		test_exception::raise(es.str(), error);
	}

	SignatureMap::iterator iter = m_sigs.find(result_sig_id);
	if (iter != m_sigs.end())
	{
		m_sigs.erase(iter);
	}
	m_sigs.insert(std::pair<tSIGNATURE_ID, Signature> (result_sig_id, sig));

	return;
}

////////////////////////////////////////////////////////////////////////////////
void TreeSignature::AddParsingSignature(const uint8_t* pBuffer, size_t nBufferSize)
{
	ProcessingBuffer(COMMAND_ADD_SIGNATURE, pBuffer, nBufferSize);
}

////////////////////////////////////////////////////////////////////////////////
void TreeSignature::DelSignature(const Signature& sig, tSIGNATURE_ID SignatureID,
								 TreeSignature* self)
{
	self->DelSignature(sig);
}

////////////////////////////////////////////////////////////////////////////////
void TreeSignature::DelSignature(const Signature& sig)
{
	KLAV_ERR error = TreeDeleteSignature(m_tree_ctx, sig.ptr(), sig.size());

	if (KLAV_FAILED(error))
	{
		std::stringstream es;
		es << "Can't delete signature from the tree. ";
		es << "Error: " << formatted_hex << error << '.' << std::endl;
		es << "[" << sig << "]";
		test_exception::raise(es.str(), error);
	}

	Signature del_sig(sig);

	TreeSignature::SignatureMap::iterator iter = m_sigs.begin();
	const TreeSignature::SignatureMap::iterator iend = m_sigs.end();
	for( ; iter != iend; ++iter)
	{
		Signature sig = iter->second;

		if (sig == del_sig)
		{
			m_sigs.erase(iter);
			break;
		}
	}
	return;
}

////////////////////////////////////////////////////////////////////////////////
void TreeSignature::DelParsingSignature(const uint8_t* pBuffer, size_t nBufferSize)
{
	ProcessingBuffer(COMMAND_DEL_SIGNATURE, pBuffer, nBufferSize);
}

////////////////////////////////////////////////////////////////////////////////
void TreeSignature::ProcessingBuffer(Action cmd,
									 const uint8_t* pBuffer, size_t nBufferSize)
{
	switch (cmd)
	{
	case COMMAND_ADD_SIGNATURE:
		ParseBuffer(pBuffer, nBufferSize, (tParseBufferCallbackFunc)AddSignature, this);
		break;

	case COMMAND_DEL_SIGNATURE:
		ParseBuffer(pBuffer, nBufferSize, (tParseBufferCallbackFunc)DelSignature, this);
		break;
	}
	return;
}

////////////////////////////////////////////////////////////////////////////////
void TreeSignature::GetSignatureFromDynamicTree()
{
	KLAV_ERR error = TreeEnumSignatures(m_tree_ctx,
									  (tTreeEnumCallbackFunc)TreeEnumCallbackFunc,
									  this);
	if (KLAV_FAILED(error))
	{
		std::stringstream es;
		es << "Can't enum signatures. Error " << formatted_hex << error << '.';
		test_exception::raise(es.str(), error);
	}
}

KLAV_ERR _TREE_CALL TreeSignature::TreeEnumCallbackFunc(TreeSignature* self,
													  tSIGNATURE_ID SignatureID,
													  const uint8_t* pSignature,
													  uint32_t dwSignatureSize)
{
	Signature sig;
	sig.init_ex(pSignature, dwSignatureSize);

	self->m_sigs.insert(std::pair<tSIGNATURE_ID, Signature> (SignatureID, sig));

	return KLAV_OK; 
}

////////////////////////////////////////////////////////////////////////////////
void TreeSignature::Dump() const
{
	if (IsStatic())
	{
		test_exception::raise("Can't print dump of static tree.");
	}

	KLAV_ERR error = TreeDump(m_tree_ctx);
	if (KLAV_FAILED(error))
	{
		std::stringstream es;
		es << "Error dumping tree. " << "Error: " << formatted_hex << error << '.';
		test_exception::raise(es.str());
	}
}


////////////////////////////////////////////////////////////////////////////////
//
// Tree scanner
//
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <iterator>

class TreeScanner
{
public:

	TreeScanner() { m_pResult = 0; }

	////////////////////////////////////////////////////////
	//
	// signature item
	//
	////////////////////////////////////////////////////////
	struct SignatureItem
	{
		size_t offset;
		tSIGNATURE_ID id;

		SignatureItem() : offset(0), id(0) {}
		SignatureItem(size_t o, tSIGNATURE_ID i) : offset(o), id(i) {}
		SignatureItem(const SignatureItem& s) { *this = s; }

		const SignatureItem& operator =(const SignatureItem& s)
			{ if(&s != this) { offset = s.offset; id = s.id; } return *this; }

		bool operator <(const SignatureItem& s) const
			{ return (offset < s.offset || (offset == s.offset && id < s.id)); }

		bool operator ==(const SignatureItem& s) const
			{ return offset == s.offset && id == s.id; }
	};


	////////////////////////////////////////////////////////
	//
	// scanning result
	//
	////////////////////////////////////////////////////////
	class ScanResult
	{
	public:
		~ScanResult() { clear(); }
		
		void clear() { m_data.clear(); }

		void insert(size_t off, tSIGNATURE_ID id)
		{
			std::pair<ScanResultSet::iterator, bool> pr =
			m_data.insert(SignatureItem(off, id));
		}

		bool operator !=(const ScanResult& x) const { return !(*this == x); }
		bool operator ==(const ScanResult& x) const
		{
			return (m_data.size() == x.m_data.size()) ?
				std::equal(m_data.begin(), m_data.end(), x.m_data.begin()) : false;
		}

		const ScanResult operator-(const ScanResult& x) const
		{
			ScanResult r;

			typedef std::vector<SignatureItem> ScanResultVector;
			ScanResultVector out (m_data.size() + x.m_data.size());

			ScanResultVector::iterator difend =
				std::set_symmetric_difference(m_data.begin(), m_data.end(),
											  x.m_data.begin(), x.m_data.end(),
											  out.begin());
			std::copy (out.begin(), difend,
					   std::insert_iterator<ScanResultSet>(r.m_data, r.m_data.begin()));
			return r;
		}

		friend std::ostream& operator<<(std::ostream& s, const ScanResult& r);

	private:
		// scan result
		struct SignatureItemLess
		{
			bool operator() (const SignatureItem& a, const SignatureItem& b) const
				{ return a.offset < b.offset || (a.offset == b.offset && a.id < b.id); }
		};
		typedef std::set<SignatureItem, SignatureItemLess> ScanResultSet;

		ScanResultSet m_data;
	};

	// scan file
	void Scan(const std::string& file_name,
			  ScanResult& result,
			  TreeSignature& tree,
			  size_t portion = 0);

	// scan buffer
	void Scan(ScanResult& result,
			  TreeSignature& tree,
			  const void* pBuffer, FileMapping::kl_size_t nBufferSize,
			  size_t portion = 0);


	// linear scan buffer
	void LinearScan(ScanResult& result,
					const TreeSignature& tree,
					const void* pBuffer, FileMapping::kl_size_t nBufferSize);

	// test scan
	void TestScan(TreeSignature& tree,
				  const void* buffer,
				  FileMapping::kl_size_t buffer_size,
				  TreeScanner::ScanResult& tree_scan_result,
				  TreeScanner::ScanResult& linear_scan_result);

	// get average scan time
	enum
	{
		PERF_SCAN_SKIP = 10,	// number of scan cycles to skip
		PERF_SCAN_COUNT  = 20	// number of scan cycles to average
	};
	uint32_t PerformanceScan(TreeSignature& tree,
							 const std::string& file_name,
							 FileMapping::kl_size_t *size);

	uint32_t PerformanceScan(TreeSignature& tree,
							 const void* pBuffer, FileMapping::kl_size_t nBufferSize);

private:

	ScanResult* m_pResult;

	static KLAV_ERR _TREE_CALL TreeSignatureFoundCallbackFunc(TreeScanner* self,
													tSIGNATURE_ID SignatureID,
													int dwPosInBuffer,
													uint64_t qwPosLinear,
													uint32_t dwSignatureSize);

	static KLAV_ERR _TREE_CALL TreeSignatureFoundCallbackFakeFunc(TreeScanner* self,
													tSIGNATURE_ID SignatureID,
													int dwPosInBuffer,
													uint64_t qwPosLinear,
													uint32_t dwSignatureSize);
};

////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& s, const TreeScanner::SignatureItem& i)
{

	return s << "off=" << formatted_hex << i.offset << std::dec << "; id=" << i.id;
}

std::ostream& operator<<(std::ostream& s, const TreeScanner::ScanResult& r)
{
	size_t size = r.m_data.size();
	TreeScanner::ScanResult::ScanResultSet::const_iterator i_iter = r.m_data.begin();
	TreeScanner::ScanResult::ScanResultSet::const_iterator i_eter = r.m_data.end();
	for (size_t n=0; i_iter != i_eter; ++i_iter, ++n)
	{
		s << std::setw(4) << std::setfill(' ') << std::dec << n << '/' << size << "; "
			<< *i_iter << std::endl;
	}

	std::set<tSIGNATURE_ID> idset;
	for (i_iter = r.m_data.begin(); i_iter != i_eter; ++i_iter)
	{
		idset.insert((*i_iter).id);
	}

	if (r.m_data.size())
	{
		s << "Total :" << std::endl;
	}

	std::set<tSIGNATURE_ID>::iterator j_iter = idset.begin();
	std::set<tSIGNATURE_ID>::iterator j_eter = idset.end();
	for ( ; j_iter != j_eter; ++j_iter)
	{
		tSIGNATURE_ID id = *j_iter;

		i_iter = r.m_data.begin();
		i_eter = r.m_data.end();
		size_t total = 0;
		for (; i_iter != i_eter; ++i_iter)
		{
			TreeScanner::SignatureItem sig = *i_iter;
			if (sig.id == id)
			{
				++total;
			}
		}

		s << "id=" << id << "(" << total << ")" << std::endl;
	}

	return s;
}

////////////////////////////////////////////////////////////////////////////////
void TreeScanner::Scan(const std::string& FileName,
					   ScanResult& result,
					   TreeSignature& tree,
					   size_t portion)
{
	FileMappingHolder<uint8_t> map(FileName);
	Scan(result, tree, map.get(), map.size(), portion);
}

////////////////////////////////////////////////////////////////////////////////
void TreeScanner::Scan(ScanResult& result,
					   TreeSignature& tree,
					   const void* pBuffer, FileMapping::kl_size_t nBufferSize,
					   size_t portion)
{
	m_pResult = &result;
	result.clear();

	if (portion == 0)
		portion = nBufferSize;

	// initializing tree searcher
	sSEARCHER_RT_CTX* SearcherRTCtx;
	KLAV_ERR error = TreeGetSeacherContext(tree.m_tree_ctx,
										 &tree.m_heap_ctx,
										 &SearcherRTCtx);
	if (KLAV_FAILED(error))
	{
		std::stringstream es;
		es << "Can't initialize tree searcher. ";
		es << "Error " << formatted_hex << error << '.';
		test_exception::raise(es.str());
	}    

	const uint8_t *buf = reinterpret_cast<const uint8_t *>(pBuffer);
	size_t done = 0;

	while (done < nBufferSize)
	{
		size_t chunk = nBufferSize - done;
		if (chunk > portion)
			chunk = portion;

		error = TreeSearchData(SearcherRTCtx,
							   buf + done,
							   chunk,
							   0,
							   chunk,
							   (tTreeSignatureFoundCallbackFunc)TreeSignatureFoundCallbackFunc,
							   this);
		if (KLAV_FAILED(error))
		{
			std::stringstream es;
			es << "Can't search data. Error " << formatted_hex << error << '.';
			test_exception::raise(es.str());
		}

		done += chunk;
	}

	TreeDoneSeacherContext(SearcherRTCtx);
}

////////////////////////////////////////////////////////////////////////////////
KLAV_ERR _TREE_CALL TreeScanner::TreeSignatureFoundCallbackFunc(
												TreeScanner* self,
												tSIGNATURE_ID SignatureID,
												int dwPosInBuffer,
												uint64_t qwPosLinear,
												uint32_t dwSignatureSize)
{
	size_t off = static_cast<size_t>(qwPosLinear);
	self->m_pResult->insert(off, SignatureID);
	return KLAV_OK;
}

KLAV_ERR _TREE_CALL TreeScanner::TreeSignatureFoundCallbackFakeFunc(
												TreeScanner* self,
												tSIGNATURE_ID SignatureID,
												int dwPosInBuffer,
												uint64_t qwPosLinear,
												uint32_t dwSignatureSize)
{
	return KLAV_OK;
}

////////////////////////////////////////////////////////////////////////////////
void TreeScanner::LinearScan(ScanResult& result,
							 const TreeSignature& tree,
							 const void* pBuffer,
							 FileMapping::kl_size_t nBufferSize)
{
	result.clear();
	const uint8_t* data = reinterpret_cast<const uint8_t*>(pBuffer);

	TreeSignature::SignatureMap::const_iterator iter = tree.m_sigs.begin();
	const TreeSignature::SignatureMap::const_iterator iend = tree.m_sigs.end();
	for ( ; iter != iend; ++iter)
	{
		tSIGNATURE_ID SignatureID = iter->first;
		Signature sig = iter->second;

		if (sig.size() > nBufferSize) continue;

		FileMapping::kl_size_t size = nBufferSize - sig.size();
		for (FileMapping::kl_size_t offset = 0; offset <= size; ++offset)
		{
			const uint8_t* pp = sig.ptr();
			const uint8_t* pe = pp + sig.size();

			for (int i=0; pp != pe; ++pp, ++i)
				if(data[offset+i] != *pp) break;

			if (pp == pe)    // signature found
				result.insert(offset, SignatureID);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void TreeScanner::TestScan(TreeSignature& tree,
						   const void* buffer,
						   FileMapping::kl_size_t buffer_size,
						   TreeScanner::ScanResult& tree_scan_result,
						   TreeScanner::ScanResult& linear_scan_result)
{
	Scan(tree_scan_result, tree, buffer, buffer_size);
	LinearScan(linear_scan_result, tree, buffer, buffer_size);
}

////////////////////////////////////////////////////////////////////////////////
uint32_t TreeScanner::PerformanceScan(TreeSignature& tree,
									  const std::string& FileName,
									  FileMapping::kl_size_t *size)
{
	FileMappingHolder<uint8_t> map(FileName);
	if (size) *size = map.size();

	uint32_t dta = 0;
	try
	{
		dta = PerformanceScan(tree, map.get(), map.size());
	}
	catch (std::runtime_error e)
	{
		test_exception::raise(std::string("[") + FileName + std::string("] ") + e.what());
	}
	return dta;
}

////////////////////////////////////////////////////////////////////////////////
uint32_t TreeScanner::PerformanceScan(TreeSignature& tree,
									  const void* pBuffer,
									  FileMapping::kl_size_t nBufferSize)
{
	// initializing tree searcher
	sSEARCHER_RT_CTX* SearcherRTCtx;
	KLAV_ERR error = TreeGetSeacherContext(tree.m_tree_ctx, &tree.m_heap_ctx,
										 &SearcherRTCtx);
	if (KLAV_FAILED(error))
	{
		std::stringstream es;
		es << "Can't initialize tree searcher. ";
		es << "Error " << formatted_hex << error << '.';
		test_exception::raise(es.str());
	}    

	int i;
	for (i=0; i<PERF_SCAN_SKIP; ++i)
	{
		error = TreeSearchData(SearcherRTCtx,
							   reinterpret_cast<const uint8_t*>(pBuffer),
							   nBufferSize,
							   0,
							   nBufferSize,
							   (tTreeSignatureFoundCallbackFunc)TreeSignatureFoundCallbackFakeFunc,
							   0);
		if (KLAV_FAILED(error)) break;
	}

	if (KLAV_FAILED(error))
	{
		std::stringstream es;
		es << "Can't search data. Error " << formatted_hex << error << '.';
		test_exception::raise(es.str());
	}    

	uint32_t dt_samples[PERF_SCAN_COUNT];

	KL_Perf_Counter cnt;
	for (i=0; i<PERF_SCAN_COUNT; ++i)
	{
		cnt.set();
		error = TreeSearchData(SearcherRTCtx,
							   reinterpret_cast<const uint8_t*>(pBuffer),
							   nBufferSize,
							   0,
							   nBufferSize,
							   (tTreeSignatureFoundCallbackFunc)TreeSignatureFoundCallbackFakeFunc,
							   0);
		uint32_t dt = cnt.reset();
		dt_samples[i] = dt;
		if (KLAV_FAILED(error)) break;
	}

	TreeDoneSeacherContext(SearcherRTCtx);

	if (KLAV_FAILED(error))
	{
		std::stringstream es;
		es << "Can't search data. Error " << formatted_hex << error << '.';
		test_exception::raise(es.str());
	}    

	uint32_t dt_total = 0;
	for (i=0; i<PERF_SCAN_COUNT; ++i)
	{
		std::cout << "Scan " << std::dec << i << ": " << dt_samples [i] << std::endl;
		dt_total += dt_samples[i];
	}

	uint32_t dta = dt_total / PERF_SCAN_COUNT;
	return dta;
}



////////////////////////////////////////////////////////////////////////////////
//
// Tree test
//
////////////////////////////////////////////////////////////////////////////////
class TreeTest
{
public:
	TreeTest(int argc, char** argv);
	~TreeTest();

	int process();

protected:
	
	int process(std::istream& inputStream);

	static int LoadDynamicTree(TreeTest * self);
	static int LoadStaticTree(TreeTest * self);

	static int AddSignaturesFromFile(TreeTest * self);
	static int DelSignaturesFromFile(TreeTest * self);

	static int AddSignature(TreeTest * self);
	static int DelSignature(TreeTest * self);

	static int ClearTree(TreeTest * self);

	static int GenerateSignaturesFromFile(TreeTest * self);

	static int Save2DynamicTree(TreeTest * self);
	static int Save2StaticTree(TreeTest * self);
	static int Save2StaticTreeEx(TreeTest * self);

	static int Dump(TreeTest * self);
	static int DynamicTreeDump(TreeTest * self);

	static int ScanFile(TreeTest * self);
	static int PerformanceScan(TreeTest * self);
	static int PortionScan(TreeTest * self);

	static int ExhaustiveScanFile(TreeTest * self);
	static int StressTest(TreeTest * self);

	static int LoadCommandFromFile(TreeTest * self);

	static int TruncateFileNamePrintMode(TreeTest * self);

	static int Exit(TreeTest * self);
	static int Quit(TreeTest * self);
	static int Help(TreeTest * self);

	static void Welcome();

private:
	TreeSignature m_ts;

	struct CommandHandler
	{
		typedef int (*TestFunction)(TreeTest* self);

		CommandHandler(const char *cmd = 0,  TestFunction fun = 0, const char *help = 0)
			: m_fun(fun), m_cmd(cmd), m_help(help) {}

		bool operator ==(const CommandHandler& a) const
		{
			return strcmp(m_cmd, a.m_cmd) == 0;
		}

		TestFunction m_fun;
		const char *m_cmd;
		const char *m_help;
	};

	typedef std::vector<CommandHandler> CommandHandlerMap;
	CommandHandlerMap m_CommandHandlerMap;

	bool m_Interpreter;
	std::istream * m_InputStream;
//	std::auto_ptr<std::istream> m_InputStream;

	enum { NO, YES } m_TruncateFileNamePrintMode;

	void ischeck(const char *msg);
	std::string PrepareFName(const std::string& fname) const;
};

////////////////////////////////////////////////////////////////////////////////
TreeTest::TreeTest(int argc, char** argv)
	:	m_Interpreter(argc < 2 ? true : false),
		m_InputStream(m_Interpreter ? &std::cin : new std::stringstream)
{
	if (! m_Interpreter)
	{
		for (int i = 1; i < argc; ++i)
		{
			*(static_cast<std::stringstream*>(m_InputStream)) << argv[i] << std::endl;
//			*(static_cast<std::stringstream*>(m_InputStream.get())) << argv[i] << std::endl;
		}
	}

	if (m_Interpreter) Welcome();

	m_CommandHandlerMap.push_back(CommandHandler( "rd",           LoadDynamicTree, "<fname>          Load dynamic tree from file.")),
	m_CommandHandlerMap.push_back(CommandHandler( "rs",            LoadStaticTree, "<fname>          Load static tree from file."));
	m_CommandHandlerMap.push_back(CommandHandler( "rf",     AddSignaturesFromFile, "<fname>          Load signatures from file."));
	m_CommandHandlerMap.push_back(CommandHandler( "df",     DelSignaturesFromFile, "<fname>          Delete signatures from file."));
	m_CommandHandlerMap.push_back(CommandHandler(  "a",              AddSignature, "<id> <sig>       Add signature with event id."));
	m_CommandHandlerMap.push_back(CommandHandler(  "d",              DelSignature, "<sig>            Delete signature with event id."));
	m_CommandHandlerMap.push_back(CommandHandler(  "c",                 ClearTree, "                 Clear signature tree."));
	m_CommandHandlerMap.push_back(CommandHandler(  "g",GenerateSignaturesFromFile, "<fname> <count> <min> <max> <like_count> <random>\n"
																				   "                      Generate 'count' signatures with length\n"
																				   "                      between 'min' and 'max' on file 'fname'.\n"
																				   "                      'Like_count'- numbef of signature on offset.\n"
																				   "                      Random starting point."));
	m_CommandHandlerMap.push_back(CommandHandler( "wd",          Save2DynamicTree, "<fname>          Save dynamic tree to file."));
	m_CommandHandlerMap.push_back(CommandHandler( "ws",           Save2StaticTree, "<fname>          Save static tree to file."));
	m_CommandHandlerMap.push_back(CommandHandler("wsx",         Save2StaticTreeEx, "<fname> <HashLevel> <HashTableSizeKb>\n"
																				   "                      Save static tree to file, setting hash table size."));
	m_CommandHandlerMap.push_back(CommandHandler( "wg",                      Dump, "                 Print tree dump (useful for dynamic tree only)."));
	m_CommandHandlerMap.push_back(CommandHandler( "wf",           DynamicTreeDump, "                 Print signatures with event id."));
	m_CommandHandlerMap.push_back(CommandHandler(  "s",                  ScanFile, "<fname>          Scan file."));
	m_CommandHandlerMap.push_back(CommandHandler( "ps",           PerformanceScan, "<fname>          Scan and report performance data."));
	m_CommandHandlerMap.push_back(CommandHandler( "sc",               PortionScan, "<fname> <size>   Scan with given portion size."));
	m_CommandHandlerMap.push_back(CommandHandler( "se",        ExhaustiveScanFile, "<sig_fname> <scan_fname>\n"
																				   "                      Exhaustive scan."));
	m_CommandHandlerMap.push_back(CommandHandler( "st",                StressTest, "<tree_width> <tree_height>\n"
																				   "                      Stress testing."));
	m_CommandHandlerMap.push_back(CommandHandler(  "f",       LoadCommandFromFile, "<fname>          Load commands from file."));
	m_CommandHandlerMap.push_back(CommandHandler("tfn", TruncateFileNamePrintMode, "{y[es] | n[o]}       Truncate file name mode."));
	m_CommandHandlerMap.push_back(CommandHandler(  "e",                      Exit, "                 Exit test. Some as quit."));
	m_CommandHandlerMap.push_back(CommandHandler(  "q",                      Quit, "                 Quit."));
	m_CommandHandlerMap.push_back(CommandHandler(  "h",                      Help, "                 Display this help."));

	m_TruncateFileNamePrintMode = NO;
}

////////////////////////////////////////////////////////////////////////////////
TreeTest::~TreeTest()
{
	if (! m_Interpreter) delete m_InputStream;
//	if (m_Interpreter) m_InputStream.release();
}

////////////////////////////////////////////////////////////////////////////////
void TreeTest::Welcome()
{
	std::cout << "Welcome to the tree search test interpreter." << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
void TreeTest::ischeck(const char *msg)
{
	if (! (*m_InputStream).good())
	{
		test_exception::raise(msg);
	}
	return;
}

////////////////////////////////////////////////////////////////////////////////
int TreeTest::LoadDynamicTree(TreeTest *self)
{
	std::string FileName;
	*(self->m_InputStream) >> FileName;
	self->ischeck("can't read file name");

	std::cout << "Loading a dynamic tree from file \"";
	std::cout << self->PrepareFName(FileName);
	std::cout << "\"." << std::endl;

	self->m_ts.Load(TreeSignature::DynamicTreeType, FileName);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
int TreeTest::Save2DynamicTree(TreeTest *self)
{
	std::string FileName;
	*(self->m_InputStream) >> FileName;
	self->ischeck("can't read file name");

	std::cout << "Saving the dynamic tree to file \"";
	std::cout << self->PrepareFName(FileName);
	std::cout << "\"." << std::endl;

	self->m_ts.Save2Dynamic(FileName);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
int TreeTest::LoadStaticTree(TreeTest *self)
{
	std::string FileName;
	*(self->m_InputStream) >> FileName;
	self->ischeck("can't read file name");

	std::cout << "Loading a static tree from file \"";
	std::cout << self->PrepareFName(FileName);
	std::cout << "\"." << std::endl;

	self->m_ts.Load(TreeSignature::StaticTreeType, FileName);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
int TreeTest::Save2StaticTree(TreeTest *self)
{
	std::string FileName;
	*(self->m_InputStream) >> FileName;
	self->ischeck("can't read file name");

	size_t HashLevel = 16, dwHashTableSizeKb = 64;

	std::cout << "Saving the static tree to file \"";
	std::cout << self->PrepareFName(FileName);
	std::cout << "\"." << std::endl;
	std::cout << "Hash level: " << std::dec << HashLevel;
	std::cout << "Hash table size: " << std::dec << dwHashTableSizeKb << "[Kb]" << std::endl;
	
	self->m_ts.Save2Static(FileName, HashLevel, dwHashTableSizeKb);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
int TreeTest::Save2StaticTreeEx(TreeTest *self)
{
	std::string FileName;
	uint32_t HashLevel = 0, dwHashTableSizeKb = 0;

	*(self->m_InputStream) >> FileName;
	self->ischeck("can't read file name");
	*(self->m_InputStream) >> std::dec >> HashLevel;
	self->ischeck("can't read hash level");
	*(self->m_InputStream) >> std::dec >> dwHashTableSizeKb;
	self->ischeck("can't read hash table size");

	std::cout << "Saving the static tree to file \"";
	std::cout << self->PrepareFName(FileName);
	std::cout << "\"." << std::endl;
	std::cout << "Hash level: " << std::dec << HashLevel;
	std::cout << "Hash table size: " << std::dec << dwHashTableSizeKb << "[Kb]" << std::endl;
	
	self->m_ts.Save2Static(FileName, HashLevel, dwHashTableSizeKb);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
int TreeTest::AddSignaturesFromFile(TreeTest *self)
{
	std::string FileName;
	*(self->m_InputStream) >> FileName;
	self->ischeck("can't read file name");

	std::cout << "Adding signatures from file \"";
	std::cout << self->PrepareFName(FileName);
	std::cout << "\"."<< std::endl;
	self->m_ts.ProcessSignaturesFromFile(TreeSignature::COMMAND_ADD_SIGNATURE, FileName);
	return 0;
}

int TreeTest::DelSignaturesFromFile(TreeTest *self)
{
	std::string FileName;
	*(self->m_InputStream) >> FileName;
	self->ischeck("can't read file name");

	std::cout << "Deleting a signatures from file \"";
	std::cout << self->PrepareFName(FileName);
	std::cout << "\"." << std::endl;

	self->m_ts.ProcessSignaturesFromFile(TreeSignature::COMMAND_DEL_SIGNATURE, FileName);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
int TreeTest::AddSignature(TreeTest *self)
{
	uint32_t id = 0;
	*(self->m_InputStream) >> std::hex >> id;

//	int id = 0;
//	*(self->m_InputStream) >> std::dec >> id;
	self->ischeck("can't read signature id");

	std::stringstream ss;
	ss << formatted_hex << id;
	std::string strSignatureID(ss.str());

	std::string strSignature;
	*(self->m_InputStream) >> strSignature;
	self->ischeck("can't read signature");

	std::cout << "Adding a signature " << strSignature << " with ID ";
	std::cout << strSignatureID << std::endl;
//	std::cout << std::dec << strSignatureID << std::endl;

	if (self->m_ts.IsStatic())
	{
		test_exception::raise("Can't modify static tree.");
	}
	std::string buf = strSignatureID + ' ' + strSignature;
	self->m_ts.AddParsingSignature(reinterpret_cast<const uint8_t*>(buf.data()), buf.size());
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
int TreeTest::DelSignature(TreeTest *self)
{
	int id = 0;
	*(self->m_InputStream) >> std::dec >> id;
	self->ischeck("can't read signature id");

	std::stringstream ss;
	ss << id;
	std::string strSignatureID(ss.str());

	std::string strSignature;
	*(self->m_InputStream) >> strSignature;
	self->ischeck("can't read signature");

	std::cout << "Deleting a signature " << strSignature << " with ID ";
	std::cout << std::dec << strSignatureID << std::endl;

	if (self->m_ts.IsStatic())
	{
		test_exception::raise("Can't modify static tree.");
	}
	std::string buf = strSignatureID + ' ' + strSignature;
	self->m_ts.DelParsingSignature(reinterpret_cast<const uint8_t*>(buf.data()), buf.size());
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
int TreeTest::Dump(TreeTest *self)
{
	std::cout << "Dump of the tree." << std::endl;
	self->m_ts.Dump();
	return 0;
}

int TreeTest::DynamicTreeDump(TreeTest *self)
{
	std::cout << "List of signatures from the tree." << std::endl;
	if (! self->m_ts.IsEmpty() && ! self->m_ts.IsDynamic())
	{
		test_exception::raise("This is not a dynamic tree.");
	}

	std::cout << self->m_ts;
	std::cout << "Total: " << std::dec << self->m_ts.Size() << " signatures." << std::endl;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
int TreeTest::ClearTree(TreeTest *self)
{
	std::cout << "Cleaning the tree." << std::endl;
	self->m_ts.Clear();
	return 0;
}


////////////////////////////////////////////////////////////////////////////////
int TreeTest::GenerateSignaturesFromFile(TreeTest *self)
{
	// read parameters
	std::string FileName;
	size_t SignatureCount = 0, MinSignatureSize = 0, MaxSignatureSize = 0, repeat_cnt = 0;
	bool random_init = false;

	*(self->m_InputStream) >> FileName;
	self->ischeck("can't read file name");
	*(self->m_InputStream) >> std::dec >> SignatureCount;
	self->ischeck("can't read signatures count\n");
	*(self->m_InputStream) >> std::dec >> MinSignatureSize;
	self->ischeck("can't read minsize of signature\n");
	*(self->m_InputStream) >> std::dec >> MaxSignatureSize;
	self->ischeck("can't read maxsize of signature\n");
	if (! MaxSignatureSize || (MaxSignatureSize < MinSignatureSize))
	{
		test_exception::raise("wrong command arguments\n");
	}
	*(self->m_InputStream) >> std::dec >> repeat_cnt;
	self->ischeck("can't read repeat counter\n");
	*(self->m_InputStream) >> std::dec >> random_init;
	self->ischeck("can't read random sign\n");

	// random initialize
	unsigned start_val = (random_init) ? (unsigned)time(NULL) : _default_start_val;
	srand(start_val);

	// banner
	std::cout << "Generating " << std::dec << SignatureCount << " signatures";
	std::cout << "; minsize = " << MinSignatureSize << "; maxsize = " << MaxSignatureSize;
	std::cout << "; repeat counter = " << repeat_cnt << "; random start value = " << start_val;
	std::cout << " from file \"";
	std::cout << self->PrepareFName(FileName);
	std::cout << "\"" << std::endl;

	if (self->m_ts.IsStatic())
	{
		test_exception::raise("Can't modify static tree.");
	}

	FileMappingHolder<uint8_t> map(FileName);

	if (MinSignatureSize > map.size() || MaxSignatureSize > map.size())
	{
		test_exception::raise(std::string("File \"") + self->PrepareFName(FileName) +
							  std::string("\"-skiped. Too small file size."));
	}

	if (repeat_cnt > MaxSignatureSize - MinSignatureSize)
	{
		repeat_cnt = MaxSignatureSize - MinSignatureSize;
	}

	// maximum number of trial to generate signature from one constant offset;
	// (expert value)
	size_t max_trial = MaxSignatureSize - MinSignatureSize;

	//
	size_t total = 0;

	for (size_t i = 0; i < SignatureCount; ++i)
	{
		uint32_t offset = random(0, map.size() - MaxSignatureSize);

		for (size_t r_cnt = 0; r_cnt < repeat_cnt; ++r_cnt)
		{
			tSIGNATURE_ID sig_id = i * repeat_cnt + r_cnt + 1;   // id >= 1

			Signature sig;
			for (size_t j = 0; j < max_trial; ++j)
			{
				uint32_t sig_size = random (MinSignatureSize, MaxSignatureSize);
				sig.init(map.get() + offset, sig_size);

				try
				{
					self->m_ts.AddSignature(sig, sig_id);
					++total;
					break;
				}
				catch (test_exception& e)
				{
					if (e.error() != KLAV_EDUPLICATE)
					{
						throw e;
					}
				}
			}
		}
	}

	std::cout << self->m_ts << std::endl;
	std::cout << "Total signature generated " << std::dec << total << std::endl;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
int TreeTest::ScanFile(TreeTest * self)
{
	std::string FileName;
	*(self->m_InputStream) >> FileName;
	self->ischeck("can't read file name");

	TreeScanner scanner;
	TreeScanner::ScanResult scan_result;

	std::cout << "Scanning file \"";
	std::cout << self->PrepareFName(FileName);
	std::cout << "\"" << std::endl;

	scanner.Scan(FileName, scan_result, self->m_ts);

	std::cout << scan_result;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
int TreeTest::PerformanceScan(TreeTest * self)
{
	std::string FileName;
	*(self->m_InputStream) >> FileName;
	self->ischeck("can't read file name");

	std::cout << "Getting performance data." << std::endl;

	if (self->m_ts.IsEmpty())
	{
		std::cout << "Tree search is empty." << std::endl;
		return 0;
	}

	TreeScanner scanner;

	FileMapping::kl_size_t size = 0;
	uint32_t dta = scanner.PerformanceScan(self->m_ts, FileName, &size);

	std::cout << "File \"";
	std::cout << self->PrepareFName(FileName);
	std::cout << "\" size: " << std::dec << size << std::endl;
	std::cout << "Average scan time: " << dta << std::endl;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
int TreeTest::PortionScan(TreeTest * self)
{
	std::string FileName;
	size_t portion = 0;

	*(self->m_InputStream) >> FileName;
	self->ischeck("can't read file name");
	*(self->m_InputStream) >> portion;
	self->ischeck("can't read portion size");

	std::cout << "Scanning file \"";
	std::cout << self->PrepareFName(FileName);
	std::cout << "\" with portion size " << std::dec << portion << " bytes" << std::endl;

	TreeScanner scanner;
	TreeScanner::ScanResult scan_result;

	scanner.Scan(FileName, scan_result, self->m_ts, portion);

	std::cout << scan_result;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
int TreeTest::ExhaustiveScanFile(TreeTest * self)
{
	std::string SigFileName, DataFileName;

	*(self->m_InputStream) >> SigFileName;
	self->ischeck("can't read file name");
	*(self->m_InputStream) >> DataFileName;
	self->ischeck("can't read file name");

	std::cout << "Exhaustive scanning. ";
	std::cout << "Data file \"";
	std::cout << self->PrepareFName(DataFileName);
	std::cout << "\"; ";

	std::cout << "Signatures file \"";
	std::cout << self->PrepareFName(SigFileName);
	std::cout << "\"" << std::endl;

	TreeSignature tree;
	TreeScanner scanner;

	// add signature from file
	tree.ProcessSignaturesFromFile(TreeSignature::COMMAND_ADD_SIGNATURE, SigFileName);

	// save static tree to temporary file
	std::string StaticTreeFileName = SigFileName + ".static_tree";
	uint8_t HashLevel = 16;
	size_t dwHashTableSizeKb = 64;
	tree.Save2Static(StaticTreeFileName, HashLevel, dwHashTableSizeKb);

	// clear tree and load static tree and repeatedly load signature to tree
	tree.Clear();
	tree.Load(TreeSignature::StaticTreeType, StaticTreeFileName, SigFileName);

	// delete static tree temporary file
	int ret = remove(StaticTreeFileName.c_str());
	if (ret < 0)
	{
		test_exception::raise(std::string("Can't delete file \"") +
							  self->PrepareFName(StaticTreeFileName) + std::string("\""));
	}

	FileMappingHolder<uint8_t> map(DataFileName);
	for (size_t offset = 0; offset < map.size(); ++offset)
	{
		void* scanBuffer = map.get() + offset;
		size_t scanBufferSize = map.size() - offset;

		TreeScanner::ScanResult linear_scan_result;
		TreeScanner::ScanResult tree_scan_result;

		scanner.LinearScan(linear_scan_result, tree, scanBuffer, scanBufferSize);

		for (size_t portion = 1; portion <= scanBufferSize; ++portion)
		{
			tree_scan_result.clear();

			scanner.Scan(tree_scan_result, tree, scanBuffer, scanBufferSize, portion);

			if (tree_scan_result != linear_scan_result)
			{
				std::stringstream es;
				es << "Skipped signature! Data file: " << DataFileName << "; ";
				es << "Start offset:" << formatted_hex << offset << "; ";
				es << "Portion:" << std::dec << portion << std::endl;
				es << tree_scan_result - linear_scan_result;
				test_exception::raise(es.str(), 0, true);
			}
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
int TreeTest::StressTest(TreeTest * self)
{
	size_t tree_width = 0, tree_height = 0;

	*(self->m_InputStream) >> std::dec >> tree_width;
	self->ischeck("can't read tree wigth\n");
	*(self->m_InputStream) >> std::dec >> tree_height;
	self->ischeck("can't read tree height\n");
	if (tree_width == 0 || tree_height == 0)
	{
		test_exception::raise("wrong command arguments\n");
	}

	AlphaSignatureGeneratorImpl gen;
	TreeSignature tree;
	TreeScanner scanner;

	// testing data buffer
	std::vector<uint8_t> data_buffer;

	for (size_t w_iter = 0; w_iter < tree_width; ++w_iter)
	{
		Signature sig;
		sig.init(gen);

		for (size_t m_iter = 0; m_iter < tree_height; )
		{
			// random modify signature
			sig.set(gen);

			// adding signature
			tSIGNATURE_ID sig_id = w_iter * tree_height + m_iter + 1;

			try
			{
				tree.AddSignature(sig, sig_id);
			}
			catch (test_exception& e)
			{
				if (e.error() != KLAV_EDUPLICATE)
					throw e;

				continue;
			}

			// modify data buffer
			data_buffer.insert(data_buffer.end(), sig.ptr(), sig.ptr() + sig.size());

			// scaning data buffer 
			TreeScanner::ScanResult tree_scan_result, linear_scan_result;
			scanner.TestScan(tree,
							 reinterpret_cast<const void*>(&data_buffer.front()),
							 data_buffer.size(),
							 tree_scan_result, linear_scan_result);

			// compare scan result
			if (tree_scan_result != linear_scan_result)
			{
				std::stringstream es;
				es << "Signature not found !" << formatted_hex << '.';
				test_exception::raise(es.str());
			}

			++m_iter;
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
int TreeTest::LoadCommandFromFile(TreeTest * self)
{
	std::string FileName;
	*(self->m_InputStream) >> FileName;
	self->ischeck("can't read file name");

	std::cout << "Executing commands from a file \"";
	std::cout << self->PrepareFName(FileName);
	std::cout << "\"." << std::endl;

	std::fstream File(FileName.c_str(), std::ios::in);
	if (! File )
	{
		std::string es;
		es += "can't open file \"";
		es += FileName;
		es += "\" with commands.\n";
		test_exception::raise(es);
	}

	bool Interpretor = self->m_Interpreter;
	self->m_Interpreter = false;

	std::istream * s = self->m_InputStream;
	self->m_InputStream = &File;

	if (self->process(File))
	{
		std::cout << "Errors occured during executing commands from the file \"";
		std::cout << self->PrepareFName(FileName);
		std::cout << "\"." << std::endl;
	}
	else
	{
		std::cout << "Commands from the file \"";
		std::cout << self->PrepareFName(FileName);
		std::cout << "\" have been executed successfully." << std::endl;
	}

	self->m_Interpreter = Interpretor;
	self->m_InputStream = s;

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
int TreeTest::TruncateFileNamePrintMode(TreeTest * self)
{
	std::string mode;
	*(self->m_InputStream) >> mode;
	self->ischeck("can't read file print mode.");

	if (mode == "y" || mode == "yes" || mode == "Y" || mode == "YES")
		self->m_TruncateFileNamePrintMode = TreeTest::YES;
	else if (mode == "n" || mode == "no" || mode == "N" || mode == "NO")
		self->m_TruncateFileNamePrintMode = TreeTest::NO;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// truncate file path
static std::string BaseFileName(const std::string& name)
{
	std::string::size_type off = name.find_last_of("/\\");
	return name.substr(off + 1);
}

std::string TreeTest::PrepareFName(const std::string& fname) const
{
	return (m_TruncateFileNamePrintMode == YES) ? BaseFileName(fname) : fname;
}

////////////////////////////////////////////////////////////////////////////////
int TreeTest::Exit(TreeTest * self) { return 1; }
int TreeTest::Quit(TreeTest * self) { return 1; }
int TreeTest::Help(TreeTest * self)
{
	std::cout << "Test commands:" << std::endl;
	for (size_t i = 0; i < self->m_CommandHandlerMap.size(); ++i)
	{
		std::cout << '-';
		std::cout << std::setw(3) << std::setfill(' ') << std::left;
		std::cout << self->m_CommandHandlerMap[i].m_cmd << " ";
		std::cout << self->m_CommandHandlerMap[i].m_help << std::endl;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
int TreeTest::process(std::istream& is)
{
	if (m_Interpreter) std::cout << PROMPT_STRING;

	std::string buffer;
	while (is >> buffer)
	{
		try
		{
			CommandHandlerMap::const_iterator iter;
			CommandHandlerMap::const_iterator beg = m_CommandHandlerMap.begin();
			CommandHandlerMap::const_iterator end = m_CommandHandlerMap.end();

			CommandHandler cmd1(buffer.c_str());
			CommandHandler cmd2(buffer.c_str()+1);

			if ((m_Interpreter && (iter = std::find(beg, end, cmd1)) == end &&
								  (iter = std::find(beg, end, cmd2)) == end) ||

				(m_Interpreter == 0 && (buffer.length() < 2 || buffer[0] != '-' ||
										(iter = std::find(beg, end, cmd2)) == end)))
			{
				std::cout << "Unknown command" << std::endl;
			}
			else
			{
				if ((*iter).m_fun)
				{
					if ((*iter).m_fun(this))
					{
						break;	// stop
					}
				}
			}
		}
		catch (test_exception& e)
		{
			std::cout << e.what() << std::endl;
			if (m_Interpreter)
			{
				char ch;
				is.clear();
				while (is.good())   // EOF or failure stops the reading
				{
					is.get(ch);
					if( ch == '\n' ) break;
				}
			}
			else
				return e.fatal() ? 2 : 1;
		}
		catch (...)
		{
			std::cout << "Unexpected error." << std::endl;
			if (m_Interpreter)
				std::getline(is, buffer);
			else
				return 2;
		}
		if (m_Interpreter) std::cout << PROMPT_STRING;
	}
	if (m_Interpreter) std::cout << std::endl;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
int TreeTest::process()
{
	return process(*m_InputStream);
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	try
	{
		TreeTest t(argc, argv);
		return t.process();
	}
	catch (std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unexpected error." << std::endl;
	}
	return -1;
}
