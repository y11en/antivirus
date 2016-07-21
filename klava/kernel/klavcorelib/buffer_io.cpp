// buffer_io.cpp
//
//

#include <klava/klav_props.h>
#include <klava/klav_io.h>
#include <klava/klav_utils.h>

////////////////////////////////////////////////////////////////

class KLAV_Buffer_IO_Object : public KLAV_IFACE_IMPL (KLAV_IO_Object)
{
public:
	KLAV_Buffer_IO_Object (hKLAV_Alloc alloc);
	virtual ~KLAV_Buffer_IO_Object ();

	virtual void KLAV_CALL destroy ()
		{ KLAV_DELETE (this, m_allocator); }

	virtual uint32_t  KLAV_CALL get_io_version ()
		{ return KLAV_IO_VERSION_CURRENT; }

	virtual void * KLAV_CALL get_io_iface (int ifc_id)
		{ return 0; }

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

	virtual KLAV_ERR KLAV_CALL get_size (klav_filepos_t *psize);
	virtual KLAV_ERR KLAV_CALL set_size (klav_filepos_t size);

	virtual KLAV_ERR KLAV_CALL flush ()
		{ return KLAV_OK; }

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

	virtual KLAV_ERR KLAV_CALL unmap_data (klav_iomap_t * iomap);

	virtual KLAV_ERR  KLAV_CALL close ();

	virtual hKLAV_Properties KLAV_CALL get_properties ();

	KLAV_ERR init (const void * data, uint32_t size, bool ro);
	void cleanup ();

private:
	hKLAV_Alloc    m_allocator;
	KLAV_Property_Bag_Holder m_props;
	uint8_t *      m_data;
	uint32_t       m_data_size;   // actual data size
	uint32_t       m_io_size;     // logical size
	klav_filepos_t m_current_pos;
	bool           m_own_buffer;  // need to deallocate buffer data
	bool           m_read_only;   // read-only mode
};

////////////////////////////////////////////////////////////////

KLAV_Buffer_IO_Object::KLAV_Buffer_IO_Object (hKLAV_Alloc alloc)
	: m_allocator (alloc),
	m_props (0),
	m_data (0), m_data_size (0), m_io_size (0),
	m_current_pos (0),
	m_own_buffer (false), m_read_only (false)
{
	m_props = KLAV_Property_Bag_Create (alloc);
}

KLAV_Buffer_IO_Object::~KLAV_Buffer_IO_Object ()
{
	cleanup ();
}

KLAV_ERR KLAV_Buffer_IO_Object::init (const void * data, uint32_t size, bool ro)
{
	if (! m_props)
		return KLAV_ENOMEM;

	cleanup ();

	if (size == 0)
		return KLAV_OK;

	if (data == 0)
	{
		data = m_allocator->alloc (size);
		if (data == 0)
			return KLAV_ENOMEM;

		m_own_buffer = true;
	}

	m_data = (uint8_t *) data;
	m_data_size = size;
	m_io_size = size;

	m_read_only = ro;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Buffer_IO_Object::seek_read (
			klav_filepos_t pos,
			void *buf,
			uint32_t size,
			uint32_t *pnrd
		)
{
	if (pnrd != 0)
		*pnrd = 0;

	if (pos > m_io_size || buf == 0)
		return KLAV_EINVAL;

	uint32_t nrd = (m_io_size - (uint32_t) pos);
	if (nrd < size)
	{
		if (pnrd == 0)
			return KLAV_EREAD;
	}
	else
	{
		nrd = size;
	}

	memcpy (buf, m_data + (size_t) pos, nrd);

	if (pnrd != 0)
		*pnrd = nrd;

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Buffer_IO_Object::seek_write (
			klav_filepos_t pos,
			const void *buf,
			uint32_t size,
			uint32_t *pnwr
		)
{
	if (pnwr != 0)
		*pnwr = 0;

	if (m_read_only)
		return KLAV_EACCESS;

	if (pos > m_io_size || buf == 0)
		return KLAV_EINVAL;

	uint32_t nwr = (m_io_size - (uint32_t) pos);
	if (nwr < size)
	{
		if (pnwr == 0)
			return KLAV_EWRITE;
	}
	else
	{
		nwr = size;
	}

	memcpy (m_data + (size_t) pos, buf, nwr);

	if (pnwr != 0)
		*pnwr = nwr;

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Buffer_IO_Object::get_size (klav_filepos_t *psize)
{
	*psize = m_io_size;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Buffer_IO_Object::set_size (klav_filepos_t size)
{
	if (m_read_only)
		return KLAV_EACCESS;

	if (size > m_data_size)
		return KLAV_EINVAL;

	m_io_size = (uint32_t) size;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Buffer_IO_Object::ioctl (
			uint32_t code,
			void *buf,
			size_t bufsize
		)
{
	memset (buf, 0, bufsize);

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

	default:
		return KLAV_ENOIMPL;
	}

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Buffer_IO_Object::query_map (
			klav_filepos_t pos,
			uint32_t       size,
			uint32_t *     pflags
		)
{
	*pflags = 0;

	if (pos > m_io_size || (m_io_size - pos < size))
		return KLAV_EINVAL;

	*pflags = KLAV_IOMAP_F_ENTIRE | KLAV_IOMAP_F_MAPPED;
	
	if (! m_read_only)
		*pflags |= KLAV_IOMAP_F_WRITABLE;

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Buffer_IO_Object::map_data (
			klav_filepos_t pos,
			uint32_t       size,
			uint32_t       flags,
			klav_iomap_t * iomap
		)
{
	if (iomap == 0)
		return KLAV_EINVAL;

	iomap->clear ();

	if (pos > m_io_size)
		return KLAV_EINVAL;

	if ((flags & KLAV_IOMAP_WRITABLE) != 0 && m_read_only)
		return KLAV_EACCESS;

	uint32_t sz = m_io_size - (uint32_t) pos;
	if (sz < size)
	{
		if ((flags & KLAV_IOMAP_PARTIAL) == 0)
			return KLAV_EREAD;
	}
	else
	{
		sz = size;
	}

	iomap->iomap_data = m_data + pos;
	iomap->iomap_size = sz;
	iomap->iomap_flags = KLAV_IOMAP_F_MAPPED;
	iomap->iomap_handle = 0;

	if ((flags & KLAV_IOMAP_WRITABLE) != 0)
		iomap->iomap_flags |= KLAV_IOMAP_F_WRITABLE;

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Buffer_IO_Object::unmap_data (klav_iomap_t * iomap)
{
	if (iomap == 0)
		return KLAV_EINVAL;

	iomap->clear ();

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Buffer_IO_Object::close ()
{
	cleanup ();
	return KLAV_OK;
}

hKLAV_Properties KLAV_CALL KLAV_Buffer_IO_Object::get_properties ()
{
	return m_props.get ();
}

void KLAV_Buffer_IO_Object::cleanup ()
{
	if (m_own_buffer && m_data != 0)
		m_allocator->free (m_data);

	m_data = 0;
	m_data_size = 0;
	m_io_size = 0;
	m_current_pos = 0;
	m_read_only = false;
	m_own_buffer = false;
}

////////////////////////////////////////////////////////////////

KLAV_ERR KLAV_Buffer_IO_Create (
			hKLAV_Alloc alloc,
			const void *data,
			size_t size,
			klav_bool_t ro,
			hKLAV_IO_Object *pio
		)
{
	*pio = 0;
	if (alloc == 0)
		return KLAV_EINVAL;

	KLAV_Buffer_IO_Object *buf = KLAV_NEW (alloc) KLAV_Buffer_IO_Object (alloc);
	if (buf == 0)
		return KLAV_ENOMEM;

	uint32_t sz = (uint32_t) size;
	if (sz != size)
		return KLAV_EINVAL;

	KLAV_ERR err = buf->init (data, sz, (ro != 0));
	if (KLAV_FAILED (err))
	{
		buf->destroy ();
		return err;
	}

	*pio = buf;
	return KLAV_OK;
}

