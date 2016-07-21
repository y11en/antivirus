// #######################################################################################
// #######################################################################################

#define _ALIGN_N(_p_val, _p_al_size)	\
	((int)(_p_val)%(_p_al_size) ? (int)(_p_val) + (_p_al_size) - (int)(_p_val)%(_p_al_size) : (int)(_p_val))

#include <vector>
#include "inilib.h"

using namespace std;

typedef vector<BYTE> tBin;
typedef unsigned __int64 QWORD;

// #######################################################################################

BOOL	_SetFilePointer(HANDLE hFile, LONGLONG llDistanceToMove, QWORD * pNewFilePointer, DWORD dwMoveMethod);

void	_A2W(LPCSTR src, wstring& dst, UINT cp = CP_ACP);
void	_A2W(LPCSTR src, DWORD nSrcLen, wstring& dst, UINT cp = CP_ACP);
void	_W2A(LPCWSTR src, string& dst, UINT cp = CP_ACP);

// #######################################################################################
// CIni

struct sIniSection;
struct sCommentLine;

struct CIni
{
	CIni(LPCWSTR pFileName = NULL, bool fWrite = false);
	~CIni();

	HRESULT	Open(LPCWSTR pFileName, bool fWrite = false);

	sIniSection * GetSection(LPCSTR pSectName);
	
	void	ReadStr(LPCSTR pSectName, LPCSTR pKeyName, string& out, LPCSTR pDefault = NULL);
	void	WriteStr(LPCSTR pSectName, LPCSTR pKeyName, LPCSTR pData);
	
	void	ReadStrW(LPCSTR pSectName, LPCSTR pKeyName, wstring& out, LPCWSTR pDefault = NULL);
	void	WriteStrW(LPCSTR pSectName, LPCSTR pKeyName, LPCWSTR pData);

	DWORD	ReadInt(LPCSTR pSectName, LPCSTR pKeyName, DWORD nDefault = 0);
	void	WriteInt(LPCSTR pSectName, LPCSTR pKeyName, DWORD nData);
	
	void	ReadBin(LPCSTR pSectName, LPCSTR pKeyName, tBin& out);
	void	WriteBin(LPCSTR pSectName, LPCSTR pKeyName, PBYTE pData, DWORD nDataSize);

	unsigned		m_fWrite : 1;
	unsigned		m_fDirty : 1;
	HANDLE			m_hFile;

	sIniSection*	pIniSection;
	sCommentLine*	pPreComment;
};

// #######################################################################################
// CStreamBuf

struct CStreamBuf : public IStream
{
	CStreamBuf(tBin& _buf) : m_buf(_buf), m_pos(0) {}

	HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead = NULL)
	{
		if( cb > m_buf.size() - m_pos )
			cb = m_buf.size() - m_pos;
		
		if( cb )
			memcpy(pv, &m_buf[m_pos], cb), m_pos += cb;
		
		if( pcbRead ) *pcbRead = cb;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE Write(const void *pv, ULONG cb, ULONG *pcbWritten = NULL)
	{
		if( m_pos + cb > m_buf.size() )
		{
			m_buf.resize(m_pos + cb);
			if( m_buf.size() != m_pos + cb )
				return E_OUTOFMEMORY;
		}
		
		if( cb )
			memcpy(&m_buf[m_pos], pv, cb), m_pos += cb;
		
		if( pcbWritten ) *pcbWritten = cb;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize)
	{
		m_buf.resize(libNewSize.LowPart);
		if( m_pos + 1 > m_buf.size() )
			m_pos = m_buf.size() ? (m_buf.size() - 1) : 0;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
	{
		return E_NOTIMPL;
	}

	HRESULT	STDMETHODCALLTYPE Stat(STATSTG *pstatstg, DWORD grfStatFlag)
	{
		memset(pstatstg, 0, sizeof(STATSTG));
		pstatstg->cbSize.QuadPart = m_buf.size();
		return S_OK;
	}

	HRESULT	STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject) { return E_NOTIMPL; }
	ULONG	STDMETHODCALLTYPE AddRef() { return 0; }
	ULONG	STDMETHODCALLTYPE Release() { return 0; }

	HRESULT	STDMETHODCALLTYPE CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten) { return E_NOTIMPL; }
	HRESULT	STDMETHODCALLTYPE Commit(DWORD grfCommitFlags) { return E_NOTIMPL; }
	HRESULT	STDMETHODCALLTYPE Revert() { return E_NOTIMPL; }
	HRESULT	STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) { return E_NOTIMPL; }
	HRESULT	STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) { return E_NOTIMPL; }
	HRESULT	STDMETHODCALLTYPE Clone(IStream **ppstm) { return E_NOTIMPL; }

private:
	ULONG m_pos;
	tBin& m_buf;
};

// #######################################################################################
// _CoInit

struct _CoInit
{
	_CoInit() { ::CoInitialize(NULL); }
	~_CoInit() { ::CoUninitialize(); }
};

template <class T>
struct _AutoDelete
{
	_AutoDelete(T * _p) : p(_p) {}
	~_AutoDelete() { if( p ) delete p; }
	T * p;
};

// #######################################################################################
// #######################################################################################
