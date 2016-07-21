// win_io.cpp
//
//

#if defined (_WIN32)

#include <klava/klavsys_windows.h>
#include <klava/klav_utils.h>

class KLAV_IO_Object_Win32 : public KLAV_IFACE_IMPL(KLAV_IO_Object)
{
public:
			KLAV_IO_Object_Win32 (KLAV_Alloc *alloc);
	virtual ~KLAV_IO_Object_Win32 ();

	virtual void KLAV_CALL destroy ();

	virtual uint32_t KLAV_CALL get_io_version ();

	virtual void * KLAV_CALL get_io_iface (
				int ifc_id
			);

	virtual KLAV_ERR KLAV_CALL seek_read (
			klav_filepos_t pos,
			void *buf,
			uint32_t size,
			uint32_t *pnrd
		);

	virtual KLAV_ERR KLAV_CALL seek_write (
			klav_filepos_t pos,
			const void *buf,
			uint32_t size,
			uint32_t *pnwr
		);

	virtual KLAV_ERR KLAV_CALL get_size (
			klav_filepos_t *psize
		);

	virtual KLAV_ERR KLAV_CALL set_size (
			klav_filepos_t size
		);

	virtual KLAV_ERR KLAV_CALL flush (
		);

	virtual KLAV_ERR KLAV_CALL ioctl (
			uint32_t code,
			void *buf,
			size_t bufsize
		);

	virtual KLAV_ERR KLAV_CALL query_map (
			klav_filepos_t pos,
			uint32_t       size,
			uint32_t *     pflags
		);

	virtual KLAV_ERR KLAV_CALL map_data (
			klav_filepos_t pos,
			uint32_t       size,
			uint32_t       flags,
			klav_iomap_t * iomap
		);

	virtual KLAV_ERR KLAV_CALL unmap_data (
			klav_iomap_t * iomap
		);

	virtual KLAV_ERR  KLAV_CALL close ();

	virtual KLAV_Properties * KLAV_CALL get_properties ();

	KLAV_Alloc * allocator () const
		{ return m_allocator; }

	KLAV_ERR open (HANDLE hfile);

	void cleanup ();

private:
	KLAV_Alloc *   m_allocator;
	HANDLE         m_handle;
	klav_filepos_t m_current_pos;
	KLAV_Property_Bag_Holder m_props;

	KLAV_IO_Object_Win32 (const KLAV_IO_Object_Win32&);
	KLAV_IO_Object_Win32& operator= (const KLAV_IO_Object_Win32&);
};

KLAV_IO_Object_Win32::KLAV_IO_Object_Win32 (KLAV_Alloc *alloc)
	: m_allocator (alloc), m_handle (INVALID_HANDLE_VALUE), m_current_pos (0)
{
	m_props = KLAV_Property_Bag_Create (allocator ());
}

KLAV_IO_Object_Win32::~KLAV_IO_Object_Win32 ()
{
	cleanup ();
}

void KLAV_IO_Object_Win32::cleanup ()
{
	if (m_handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle (m_handle);
		m_handle = INVALID_HANDLE_VALUE;
	}
	m_current_pos = 0;

	if (m_props.get () != 0)
	{
		m_props->clear_properties ();
	}
}

////////////////////////////////////////////////////////////////

void KLAV_CALL KLAV_IO_Object_Win32::destroy ()
{
	KLAV_DELETE (this, allocator ());
}

uint32_t KLAV_CALL KLAV_IO_Object_Win32::get_io_version ()
{
	return KLAV_IO_VERSION_CURRENT;
}

void * KLAV_CALL KLAV_IO_Object_Win32::get_io_iface (
				int ifc_id
			)
{
	return 0;
}

KLAV_ERR KLAV_CALL KLAV_IO_Object_Win32::seek_read (
			klav_filepos_t pos,
			void *buf,
			uint32_t size,
			uint32_t *pnrd
		)
{
	if (pnrd != 0)
		*pnrd = 0;

	if (m_handle == INVALID_HANDLE_VALUE)
		return KLAV_ENOINIT;

	LARGE_INTEGER lsz;
	lsz.QuadPart = pos;

	LONG pos_hi = lsz.HighPart;
	if (SetFilePointer (m_handle, lsz.LowPart, &pos_hi, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		DWORD ec = GetLastError ();
		if (ec != NO_ERROR)
			return KLAV_Translate_System_Error (ec);
	}

	DWORD nrd = 0;
	if (! ReadFile (m_handle, buf, size, &nrd, 0))
	{
		return KLAV_Get_System_Error ();
	}

	if (pnrd != 0)
	{
		*pnrd = nrd;
	}
	else
	{
		if (nrd < size)
			return KLAV_EREAD;
	}

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_IO_Object_Win32::seek_write (
			klav_filepos_t pos,
			const void *buf,
			uint32_t size,
			uint32_t *pnwr
		)
{
	if (pnwr != 0)
		*pnwr = 0;

	if (m_handle == INVALID_HANDLE_VALUE)
		return KLAV_ENOINIT;

	LARGE_INTEGER lsz;
	lsz.QuadPart = pos;

	LONG pos_hi = lsz.HighPart;
	if (SetFilePointer (m_handle, lsz.LowPart, &pos_hi, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		DWORD ec = GetLastError ();
		if (ec != NO_ERROR)
			return KLAV_Translate_System_Error (ec);
	}

	DWORD nwr = 0;
	if (! WriteFile (m_handle, buf, size, &nwr, 0))
	{
		return KLAV_Get_System_Error ();
	}

	if (pnwr != 0)
	{
		*pnwr = nwr;
	}
	else
	{
		if (nwr < size)
			return KLAV_EWRITE;
	}

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_IO_Object_Win32::get_size (
			klav_filepos_t *psize
		)
{
	*psize = 0;
	
	if (m_handle == INVALID_HANDLE_VALUE)
		return KLAV_ENOINIT;

	DWORD size_hi = 0;
	DWORD size_lo = GetFileSize (m_handle, &size_hi);
	if (size_lo == INVALID_FILE_SIZE)
	{
		DWORD ec = GetLastError ();
		if (ec != NO_ERROR)
		{
			return KLAV_Translate_System_Error (ec);
		}
	}

	LARGE_INTEGER lsz;
	lsz.LowPart = size_lo;
	lsz.HighPart = size_hi;

	*psize = lsz.QuadPart;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_IO_Object_Win32::set_size (
			klav_filepos_t size
		)
{
	if (m_handle == INVALID_HANDLE_VALUE)
		return KLAV_ENOINIT;

	LARGE_INTEGER lsz;
	lsz.QuadPart = size;

	LONG size_lo = lsz.LowPart;
	LONG size_hi = lsz.HighPart;

	size_lo = SetFilePointer (m_handle, size_lo, &size_hi, FILE_BEGIN);
	if (size_lo == INVALID_SET_FILE_POINTER)
	{
		DWORD ec = GetLastError ();
		if (ec != NO_ERROR)
		{
			return KLAV_Translate_System_Error (ec);
		}
	}

	if (! SetEndOfFile (m_handle))
	{
		return KLAV_Get_System_Error ();
	}

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_IO_Object_Win32::flush (
		)
{
	if (m_handle == INVALID_HANDLE_VALUE)
		return KLAV_ENOINIT;

	if (! FlushFileBuffers (m_handle))
	{
		return KLAV_Get_System_Error ();
	}

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_IO_Object_Win32::ioctl (
			uint32_t code,
			void *buf,
			size_t bufsize
		)
{
	if (m_handle == INVALID_HANDLE_VALUE)
		return KLAV_ENOINIT;

	switch (code)
	{
	case KLAV_IOCTL_GET_CURRENT_POS:
		if (bufsize != sizeof (klav_filepos_t))
			return KLAV_EINVAL;
		* ((klav_filepos_t *)buf) = m_current_pos;
		break;

	case KLAV_IOCTL_SET_CURRENT_POS:
		if (bufsize != sizeof (klav_filepos_t))
			return KLAV_EINVAL;
		m_current_pos = * ((klav_filepos_t *)buf);
		break;

	case KLAV_IOCTL_GET_IO_ORIGIN:
		if (bufsize != sizeof (klav_filepos_t))
			return KLAV_EINVAL;
		* ((klav_filepos_t *)buf) = 0;
		break;

	case KLAV_IOCTL_GET_IO_HANDLE:
		if (bufsize != sizeof (uint64_t))
			return KLAV_EINVAL;
		* ((uint64_t *)buf) = (uint64_t)m_handle;
		break;

	default:
		return KLAV_ENOIMPL;
	}

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_IO_Object_Win32::query_map (
			klav_filepos_t pos,
			uint32_t       size,
			uint32_t *     pflags
		)
{
	*pflags = 0;      // until mapping will be implemented
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_IO_Object_Win32::map_data (
			klav_filepos_t pos,
			uint32_t       size,
			uint32_t       flags,
			klav_iomap_t * iomap
		)
{
	iomap->clear ();
	return KLAV_ENOTIMPL;
}

KLAV_ERR KLAV_CALL KLAV_IO_Object_Win32::unmap_data (
			klav_iomap_t * iomap
		)
{
	iomap->clear ();
	return KLAV_EINVAL;
}

KLAV_ERR KLAV_CALL KLAV_IO_Object_Win32::close ()
{
	if (m_handle == INVALID_HANDLE_VALUE)
		return KLAV_EALREADY;

	KLAV_ERR err = KLAV_OK;

	if (! CloseHandle (m_handle))
		err = KLAV_Get_System_Error ();

	m_handle = INVALID_HANDLE_VALUE;
	m_current_pos = 0;

	return KLAV_OK;
}

KLAV_Properties * KLAV_CALL KLAV_IO_Object_Win32::get_properties ()
{
	return m_props;
}

KLAV_ERR KLAV_IO_Object_Win32::open (HANDLE hfile)
{
	if (hfile == 0 || hfile == INVALID_HANDLE_VALUE)
		return KLAV_EINVAL;

	if (m_handle != INVALID_HANDLE_VALUE)
		return KLAV_EALREADY;

	m_handle = hfile;
	m_current_pos = 0;

	return KLAV_OK;
}

////////////////////////////////////////////////////////////////

KLAV_EXTERN_C
KLAV_ERR KLAVSYS_Create_IO_Object (
			KLAV_Alloc * alloc,
			HANDLE hdl,
			KLAV_Properties *props,
			KLAV_IO_Object **pobject
		)
{
	KLAV_ERR err = KLAV_OK;
	*pobject = 0;

	if (hdl == INVALID_HANDLE_VALUE)
		return KLAV_EINVAL;

	KLAV_IO_Object_Win32 * io_object = KLAV_NEW (alloc) KLAV_IO_Object_Win32 (alloc);
	if (io_object == 0)
		return KLAV_ENOMEM;

	err = io_object->open (hdl);
	if (KLAV_FAILED (err))
	{
		io_object->destroy ();
		return err;
	}
	
	*pobject = io_object;
	return KLAV_OK;
}

#endif //_WIN32
