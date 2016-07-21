
#ifndef __ISTREAM_STUB_H__
#define __ISTREAM_STUB_H__

//////////////////////////////////////////////////////////////////////////
// Source file for IIOStream implementation

#define READ_CHUNCK_SIZE	0x1000

#include <io.h>

//////////////////////////////////////////////////////////////////////////
// IIOStream implementation

class IStreamStub : public IStream
{
    LPCTSTR m_lpszFileName;
    int m_handle;
    int m_ref;
public:
    IStreamStub(): m_lpszFileName(NULL), m_ref(0), m_handle(-1)
    {}

    int open( const char *filename, int oflag, int pmode = 0)
    {
        m_handle = ::_open( filename, oflag, pmode );
        if (-1 != m_handle)
            ::_lseek(m_handle, 0L, SEEK_SET);
        return m_handle;
    }

    int close()
    {
        return 1;//_close( m_handle );
    }

	STDMETHOD(QueryInterface)(REFIID riid, void **ppvObject)
	{
		return S_OK;
	}

	STDMETHOD_(ULONG, AddRef)(void)
	{
		return ++m_ref;
	}

	STDMETHOD_(ULONG, Release)(void)
	{
		return --m_ref;
	}

	STDMETHOD(Read)(void *pv, ULONG cb, ULONG *pcbRead)
	{
        *pcbRead = ::_read( m_handle, pv, cb);
        return *pcbRead != -1 ? S_OK : E_FAIL;
	}

	STDMETHOD(Write)(const void *pv, ULONG cb, ULONG *pcbWritten)
	{
        *pcbWritten = ::_write( m_handle, pv, cb);
        return *pcbWritten != -1 ? S_OK : E_FAIL;
	}

	STDMETHOD(Seek)(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
	{
        /**plibNewPosition = _lseek(m_handle, dlibMove.QuadPart, 
            STREAM_SEEK_SET == dwOrigin ? SEEK_SET :
            STREAM_SEEK_CUR == dwOrigin ? SEEK_CUR :
            SEEK_END 
            );
        return *plibNewPosition == -1 ? S_OK : E_FAIL;*/
		return S_OK;
	}

	STDMETHOD(SetSize)(ULARGE_INTEGER libNewSize)
	{
		return S_OK;
	}

	STDMETHOD(CopyTo)(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
	{
		return S_OK;
	}

	STDMETHOD(Commit)(DWORD grfCommitFlags)
	{
		return S_OK;
	}

	STDMETHOD(Revert)(void)
	{
		return S_OK;
	}

	STDMETHOD(LockRegion)(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
	{
		return S_OK;
	}

	STDMETHOD(UnlockRegion)(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
	{
		return S_OK;
	}

	STDMETHOD(Stat)(STATSTG *pstatstg, DWORD grfStatFlag)
	{
		return S_OK;
	}

	STDMETHOD(Clone)(IStream **ppstm)
	{
		return S_OK;
	}

};

#endif//__ISTREAM_STUB_H__