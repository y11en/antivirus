//////////////////////////////////////////////////////////////////////////
// Source file for IIOStream implementation

#include <prague.h>
#include <objbase.h>

#include <iface/i_io.h>

#define READ_CHUNCK_SIZE	0x1000

//////////////////////////////////////////////////////////////////////////
// IIOStream implementation

class IIOStream : public IStream
{
public:
	IIOStream(hIO io, tBOOL del_io) :
	  m_io(io), m_ref(1), m_pos(0), m_del_io(del_io){}

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
		tDWORD l_ref = --m_ref;
		if( !l_ref )
		{
			if( m_del_io )
				CALL_SYS_ObjectClose(m_io);
			delete this;
		}
		return l_ref;
	}

	STDMETHOD(Read)(void *pv, ULONG cb, ULONG *pcbRead)
	{
		tDWORD out_size = 0;
		tERROR error = cb ? CALL_IO_SeekRead(m_io, &out_size, m_pos, pv, cb) : errOK;

		if( pcbRead )
			*pcbRead = out_size;

		if( PR_FAIL(error) )
			return S_FALSE;

		m_pos += out_size;
		return out_size ? S_OK : S_FALSE;
	}

	STDMETHOD(Write)(const void *pv, ULONG cb, ULONG *pcbWritten)
	{
		tDWORD out_size = 0;
		tERROR error = CALL_IO_SeekWrite(m_io, &out_size, m_pos, (tPTR)pv, cb);

		if( pcbWritten )
			*pcbWritten = out_size;

		if( PR_FAIL(error) )
			return S_FALSE;

		m_pos += out_size;
		return out_size ? S_OK : S_FALSE;
	}

	STDMETHOD(Seek)(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition)
	{
		switch( dwOrigin )
		{
		case STREAM_SEEK_SET: m_pos = dlibMove.QuadPart; break;
		case STREAM_SEEK_CUR: m_pos += dlibMove.QuadPart; break;
		case STREAM_SEEK_END: m_pos = GetSize() + dlibMove.QuadPart; break;
		default: return STG_E_INVALIDFUNCTION;
		}
		if( plibNewPosition )
			plibNewPosition->QuadPart = m_pos;
		return S_OK;
	}

	STDMETHOD(SetSize)(ULARGE_INTEGER libNewSize)
	{
		if( PR_FAIL(CALL_IO_SetSize(m_io, libNewSize.QuadPart)) )
			return S_FALSE;

		return S_OK;
	}

	STDMETHOD(CopyTo)(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten)
	{
		tQWORD l_size = 0;
		tBYTE  l_buff[READ_CHUNCK_SIZE];
		tDWORD l_read_size = READ_CHUNCK_SIZE;

		while( l_size < cb.QuadPart && l_read_size == READ_CHUNCK_SIZE )
		{
			if( FAILED(Read(l_buff, READ_CHUNCK_SIZE, (ULONG*)&l_read_size)) )
				return S_FALSE;
			if( FAILED(pstm->Write(l_buff, l_read_size, NULL)) )
				return S_FALSE;
			l_size += l_read_size;
		}

		if( pcbRead )
			pcbRead->QuadPart = l_size;

		if( pcbWritten )
			pcbWritten->QuadPart = l_size;
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
		pstatstg->type = STGTY_STREAM;
		pstatstg->cbSize.QuadPart = GetSize();
		pstatstg->grfMode = 0;
		pstatstg->grfLocksSupported = LOCK_WRITE;
		pstatstg->clsid = CLSID_NULL;
		pstatstg->grfStateBits = 0;
		return S_OK;
	}

	STDMETHOD(Clone)(IStream **ppstm)
	{
		IIOStream *clone = new IIOStream(*this);
		clone->m_ref = 1;
		*ppstm = clone;
		return S_OK;
	}

private:
	tQWORD GetSize()
	{
		tQWORD l_size = 0;
		CALL_IO_GetSize(m_io, &l_size, IO_SIZE_TYPE_EXPLICIT);
		return l_size;
	}

	hIO		m_io;
	tBOOL	m_del_io;
	tDWORD	m_ref;
	tQWORD	m_pos;
};

//////////////////////////////////////////////////////////////////////////

IStream * CreateIStreamIO(hIO p_io, tBOOL p_del_io)
{
	return new IIOStream(p_io, p_del_io);
}

//////////////////////////////////////////////////////////////////////////
