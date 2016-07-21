// virtual_io.cpp
//

#include <klava/klav_io.h>
#include <klava/klav_utils.h>

enum // VIO buffer flags
{
	VIO_BUF_VALID  = 0x0001,  // VIO buffer is valid
	VIO_BUF_ALLOC  = 0x0002,  // VIO buffer is allocated (m_data is allocated)
	VIO_BUF_MAPPED = 0x0004   // VIO buffer is mapped (m_iomap is valid)
};

// VIO buffers are used as:
// - IO buffers, returned to client
// - virtually-written buffer
struct KLAV_VIO_Buffer
{
	uint8_t *         m_data;
	uint32_t          m_size;
	uint32_t          m_flags;
	klav_filepos_t    m_iopos;
	klav_iomap_t      m_iomap; // underlying IO mapping
	KLAV_VIO_Buffer * m_next;
	KLAV_VIO_Buffer * m_prev;

	void clear ()
	{
		m_data = 0;
		m_size = 0;
		m_flags = 0;
		m_iopos = 0;
		m_iomap.clear ();
		m_next = 0;
		m_prev = 0;
	}
};

typedef KLAV_Pool_Allocator <KLAV_VIO_Buffer> KLAV_VIO_Buffer_Pool;

////////////////////////////////////////////////////////////////

struct KLAV_VIO_Object : public KLAV_IFACE_IMPL(KLAV_IO_Object)
{
	KLAV_VIO_Object (KLAV_Alloc * allocator);

	virtual ~KLAV_VIO_Object ();

	KLAV_ERR init_io (KLAV_IO_Object *base, klav_filepos_t origin);

	KLAV_Alloc * allocator () const
		{ return m_allocator; }

	void cleanup ();

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

	virtual KLAV_ERR KLAV_CALL close ();

	virtual KLAV_Properties * KLAV_CALL get_properties ();

private:
	KLAV_VIO_Buffer_Pool m_vbuf_pool;
	KLAV_Alloc *    m_allocator;
	KLAV_IO_Object* m_base;

	// list of 'written buffers' (ordered by offsets)
	KLAV_VIO_Buffer * m_wbuf_first;
	KLAV_VIO_Buffer * m_wbuf_last;

	klav_filepos_t m_io_origin;
	klav_filepos_t m_io_size;           // logical IO size
	klav_filepos_t m_phys_io_size;      // physical IO size
	klav_filepos_t m_io_current_pos;

	void insert_written_buffer_before (
			KLAV_VIO_Buffer *buf,
			KLAV_VIO_Buffer *next
		);

	void release_written_buffer (KLAV_VIO_Buffer *buf);
	void release_written_buffers ();

	KLAV_ERR read_scatter (
			klav_filepos_t pos,
			uint8_t *databuf,
			uint32_t size,
			uint32_t *pnrd
		);

//	KLAV_ERR do_read_iobuf (
//			klav_filepos_t pos,
//			uint32_t size,
//			uint32_t flags,
//			KLAV_VIO_Buffer **ppbuf
//		);

//	KLAV_ERR do_release_iobuf (
//			KLAV_VIO_Buffer *pbuf
//		);

};

KLAV_VIO_Object::KLAV_VIO_Object (KLAV_Alloc * allocator)
	: m_vbuf_pool (allocator, 2000),
	  m_allocator (allocator),
	  m_base (0),
	  m_wbuf_first (0),
	  m_wbuf_last (0),
	  m_io_origin (0),
	  m_io_size (0),
	  m_phys_io_size (0),
	  m_io_current_pos (0)
{
}

KLAV_VIO_Object::~KLAV_VIO_Object ()
{
	cleanup ();
}

KLAV_ERR KLAV_VIO_Object::init_io (KLAV_IO_Object *base, klav_filepos_t origin)
{
	m_base = base;

	m_io_size = 0;
	m_phys_io_size = 0;
	m_io_origin = origin;

	klav_filepos_t phys_io_size = 0;

	KLAV_ERR err = m_base->get_size (& phys_io_size);
	if (KLAV_FAILED (err))
	{
		cleanup ();
		return err;
	}

	if (m_io_origin < phys_io_size)
		m_phys_io_size = phys_io_size - m_io_origin;

	m_io_size = m_phys_io_size;

	return err;
}

void KLAV_VIO_Object::cleanup ()
{
	m_base = 0;
	m_io_origin = 0;
	m_io_size = 0;
	m_io_current_pos = 0;

	release_written_buffers ();

	// TODO: zero-fill released pages
	m_vbuf_pool.cleanup ();
}

void KLAV_CALL KLAV_VIO_Object::destroy ()
{
	KLAV_DELETE (this, allocator ());
}

uint32_t KLAV_CALL KLAV_VIO_Object::get_io_version ()
{
	return KLAV_IO_VERSION_CURRENT;
}

void * KLAV_CALL KLAV_VIO_Object::get_io_iface (
			int ifc_id
		)
{
	return 0;
}

KLAV_ERR KLAV_CALL KLAV_VIO_Object::seek_read (
			klav_filepos_t pos,
			void *buf,
			uint32_t size,
			uint32_t *pnrd
		)
{
	if (pnrd != 0)
		*pnrd = 0;
	
	if (m_base == 0)
		return KLAV_ENOINIT;

	// TODO: check if pos + origin is within limits

	KLAV_ERR err = read_scatter (pos, (uint8_t *)buf, size, pnrd);

	return err;
}

KLAV_ERR KLAV_CALL KLAV_VIO_Object::seek_write (
			klav_filepos_t pos,
			const void * data,
			uint32_t size,
			uint32_t *pnwr
		)
{
	if (m_base == 0)
		return KLAV_ENOINIT;

	// TODO: check if pos + origin + size is within limits
	klav_filepos_t epos = pos + (klav_filepos_t) size;
	if (epos > m_io_size)
		m_io_size = epos;

	const uint8_t * data_ptr = (const uint8_t *) data;
	uint32_t        data_size = size;

	// find place to insert new 'virtual write' buffer
	KLAV_VIO_Buffer * bp = m_wbuf_first;

	KLAV_ERR err = KLAV_OK;

	while (data_size != 0)
	{
		uint32_t portion = 0;

		if (bp == 0 || bp->m_iopos > pos)
		{
			// create I/O buffer [pos, min (data_size, bp->m_iopos - pos))
			portion = data_size;
			if (bp != 0 && (klav_filepos_t) portion > (bp->m_iopos - pos))
				portion = (uint32_t) (bp->m_iopos - pos);

			KLAV_VIO_Buffer * wbuf = m_vbuf_pool.alloc ();
			if (wbuf == 0)
			{
				err = KLAV_ENOMEM;
				break;
			}

			wbuf->clear ();
			
			wbuf->m_data = (uint8_t *) m_allocator->alloc (portion);
			if (wbuf->m_data == 0)
			{
				m_vbuf_pool.free (wbuf);
				err = KLAV_ENOMEM;
				break;
			}

			::memcpy (wbuf->m_data, data_ptr, portion);

			wbuf->m_size  = portion;
			wbuf->m_flags = VIO_BUF_VALID | VIO_BUF_ALLOC;
			wbuf->m_iopos = pos;

			insert_written_buffer_before (wbuf, bp);
		}
		else // bp != 0 && bp->m_iopos <= pos
		{
			// do we touch this buffer?
			klav_filepos_t bp_end = bp->m_iopos + (klav_filepos_t) bp->m_size;
			if (pos < bp_end)
			{
				// offset in buffer
				uint32_t off = (uint32_t) (pos - bp->m_iopos);
				portion = data_size;
				if (portion > (bp->m_size - off))
					portion = bp->m_size - off;

				::memcpy (bp->m_data + off, data_ptr, portion);
			}

			bp = bp->m_next;
		}

		pos += portion;
		data_ptr += portion;
		data_size -= portion;
	}

	if (pnwr != 0)
		*pnwr = size - data_size;

	return err;
}

KLAV_ERR KLAV_CALL KLAV_VIO_Object::get_size (
			klav_filepos_t *psize
		)
{
	if (m_base == 0)
	{
		*psize = 0;
		return KLAV_ENOINIT;
	}

	*psize = m_io_size;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_VIO_Object::set_size (
			klav_filepos_t size
		)
{
	if (m_base == 0)
		return KLAV_ENOINIT;

	// TODO: check if io_origin + size is within limits
	if (size < m_io_size)
	{
		m_io_size = size;

		// TODO: shrink virtual IO buffers, if necessary
		KLAV_VIO_Buffer * pb = m_wbuf_first;
		while (pb != 0)
		{
			KLAV_VIO_Buffer *next = pb->m_next;

			if (pb->m_iopos >= size)
			{
				release_written_buffer (pb);
			}
			else
			{
				klav_filepos_t pb_end = pb->m_iopos + (klav_filepos_t) pb->m_size;
				if (pb_end > size)
				{
					// clear buffer contents past the end of file
					uint32_t off = (uint32_t) (size - pb->m_iopos);
					::memset (pb->m_data + off, 0, pb->m_size - off);
				}
			}

			pb = next;
		}
	}
	else
	{
		m_io_size = size;
	}

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_VIO_Object::flush ()
{
	// virtual IO does not actually write to the file system
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_VIO_Object::ioctl (
			uint32_t code,
			void *buf,
			size_t bufsize
		)
{
	switch (code)
	{
	case KLAV_IOCTL_GET_CURRENT_POS:
		if (bufsize != sizeof (klav_filepos_t))
			return KLAV_EINVAL;
		* ((klav_filepos_t *)buf) = m_io_current_pos;
		break;

	case KLAV_IOCTL_SET_CURRENT_POS:
		if (bufsize != sizeof (klav_filepos_t))
			return KLAV_EINVAL;
		m_io_current_pos = * ((klav_filepos_t *)buf);
		break;

	case KLAV_IOCTL_GET_IO_ORIGIN:
		if (bufsize != sizeof (klav_filepos_t))
			return KLAV_EINVAL;
		* ((klav_filepos_t *)buf) = m_io_origin;
		break;

	default:
		return KLAV_ENOTIMPL;
	}

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_VIO_Object::query_map (
			klav_filepos_t pos,
			uint32_t       size,
			uint32_t *     pflags
		)
{
	*pflags = 0; // TODO: implement
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_VIO_Object::map_data (
			klav_filepos_t pos,
			uint32_t       size,
			uint32_t       flags,
			klav_iomap_t * iomap
		)
{
	iomap->clear ();
	return KLAV_ENOTIMPL;
}

KLAV_ERR KLAV_CALL KLAV_VIO_Object::unmap_data (
			klav_iomap_t * iomap
		)
{
	iomap->clear ();
	return KLAV_EINVAL;
}

KLAV_ERR KLAV_CALL KLAV_VIO_Object::close ()
{
	cleanup ();
	return KLAV_OK;
}

KLAV_Properties * KLAV_CALL KLAV_VIO_Object::get_properties ()
{
	return 0; // TODO:
}

////////////////////////////////////////////////////////////////

/*
KLAV_ERR KLAV_VIO_Object::do_read_iobuf (
			klav_filepos_t pos,
			uint32_t size,
			uint32_t flags,
			KLAV_VIO_Buffer **ppbuf
		)
{
	*ppbuf = 0;

	if (m_base == 0)
		return KLAV_ENOINIT;

	// TODO: check if pos + m_io_origin < MAX_FILEPOS
	// TODO: check id pos + m_io_origin + size is within limits

	if (size != 0)
	{
		klav_filepos_t epos = pos + (klav_filepos_t) size;

		// TODO: check if we touch some of the 'virtually written' regions
		KLAV_VIO_Buffer * pb = m_wbuf_first;
		for (; pb != 0; pb = pb->m_next)
		{
			if (epos <= pb->m_iopos)
				{ pb = 0; break; }

			if (pos < pb->m_iopos + (klav_filepos_t) pb->m_size)
				break; // hit!
		}
		if (pb != 0)
		{
			// allocate buffer and copy data
			KLAV_VIO_Buffer *vbuf = m_vbuf_pool.alloc ();
			if (vbuf == 0)
				return KLAV_ENOMEM;

			vbuf->clear ();

			// TODO: if request fits into single buffer, just return pointer to internal data?
			// (this may require copying of the w-buffer on write or invalidation)

			vbuf->m_data = m_allocator->alloc (size);
			if (vbuf->m_data == 0)
			{
				m_vbuf_pool.free (vbuf);
				return KLAV_ENOMEM;
			}

			// perform scattered read into the buffer
			uint32_t nrd = 0;
			KLAV_ERR err = read_scatter (pos, vbuf->m_data, size, &nrd);
			if (KLAV_FAILED (err))
			{
				m_allocator->free (vbuf->m_data);
				vbuf->m_data = 0;

				m_vbuf_pool.free (vbuf);
				return err;
			}

			if (nrd < size)
			{
				if ((flags & KLAV_IOBUF_READ_PARTIAL) != 0)
				{
					// TODO: do we need it here?
					::memset (vbuf->m_data + nrd, 0, size - nrd);
				}
				else
				{
					m_allocator->free (vbuf->m_data);
					vbuf->m_data = 0;

					m_vbuf_pool.free (vbuf);
					return KLAV_EREAD;
				}
			}

			vbuf->m_size = nrd;
			vbuf->m_flags = VIO_BUF_VALID | VIO_BUF_ALLOC;
			vbuf->m_iopos = pos;
			*ppbuf = vbuf;

			return KLAV_OK;
		}
	}

	uint32_t size_avail = 0;

	if (pos < m_io_size)
	{
		size_avail = size;
		if ((m_io_size - pos) < (klav_filepos_t) size_avail)
			size_avail = (uint32_t) (m_io_size - pos);
	}

	if (size_avail < size)
	{
		if ((flags & KLAV_IOBUF_READ_PARTIAL) == 0)
			return KLAV_EREAD;
	}

	// create iobuf
	KLAV_VIO_Buffer * vbuf = m_vbuf_pool.alloc ();
	if (vbuf == 0)
	{
		return KLAV_ENOMEM;
	}

	vbuf->clear ();

	// are we reading outside physical data IO range ?
	if (pos >= m_phys_io_size || ((m_phys_io_size - pos) < (klav_filepos_t)size))
	{
		vbuf->m_data = m_allocator->alloc (size);
		if (vbuf->m_data == 0)
		{
			m_vbuf_pool.free (vbuf);
			return KLAV_ENOMEM;
		}

		KLAV_ERR err = read_scatter (pos, vbuf->m_data, size, &vbuf->m_size);
		if (KLAV_SUCCEEDED (err) && vbuf->m_size < size)
		{
			if ((flags & KLAV_IOBUF_READ_PARTIAL) == 0)
				err = KLAV_EREAD;
		}

		if (KLAV_FAILED (err))
		{
			m_allocator->free (vbuf->m_data);
			m_vbuf_pool.free (vbuf);
		}

		// TODO: do we need it here ?
		::memset (vbuf->m_data + vbuf->m_size, 0, size - vbuf->m_size);

		vbuf->m_flags = VIO_BUF_VALID | VIO_BUF_ALLOC;
		vbuf->m_iopos = pos;

		*ppbuf = vbuf;
		return KLAV_OK;
	}

	klav_filepos_t io_pos = m_io_origin + pos;

	// try to read the whole block (size may be greater than size_avail)
	// this will stimulate allocation of a larger buffer
	KLAV_ERR err = m_base->read_iobuf (io_pos, size, flags, &(vbuf->m_iobuf));
	if (KLAV_FAILED (err))
	{
		m_vbuf_pool.free (vbuf);
		return err;
	}

	vbuf->m_data  = (uint8_t *) vbuf->m_iobuf.iob_data;

	// compensate for larger buffer size passed to read_iobuf
	vbuf->m_size  = (vbuf->m_iobuf.iob_size > size_avail) ? size_avail : vbuf->m_iobuf.iob_size;

	vbuf->m_flags = VIO_BUF_VALID | VIO_BUF_MAPPED;
	vbuf->m_iopos = pos;

	*ppbuf = vbuf;
	return KLAV_OK;
}
*/
/*
KLAV_ERR KLAV_VIO_Object::do_release_iobuf (
			KLAV_VIO_Buffer *vbuf
		)
{
	if (vbuf == 0 || (vbuf->m_flags & KLAV_IOBUF_F_VALID) == 0)
		return KLAV_EINVAL;

	if ((vbuf->m_flags & VIO_BUF_VALID) != 0)
	{
		if (vbuf->m_data != 0)
			m_allocator->free (vbuf->m_data);
	}

	if (vbuf->m_iobuf.iob_data != 0)
	{
		if (m_base != 0)
		{
			m_base->release_iobuf (& (vbuf->m_iobuf));
		}
	}

	vbuf->clear ();

	m_vbuf_pool.free (vbuf);

	return KLAV_OK;
}
*/

KLAV_ERR KLAV_VIO_Object::read_scatter (
			klav_filepos_t pos,
			uint8_t *databuf,
			uint32_t req_size,
			uint32_t *pnrd
		)
{
	// limit data, available to read, to match virtual IO size
	uint32_t size_avail = 0;

	if (pos < m_io_size)
	{
		size_avail = req_size;

		if ((m_io_size - pos) < (klav_filepos_t) size_avail)
			size_avail = (uint32_t) (m_io_size - pos);
	}

	if (pnrd != 0)
	{
		*pnrd = 0;
	}
	else
	{
		if (size_avail < req_size)
			return KLAV_EREAD;
	}

	uint32_t  remain = size_avail;
	uint8_t * dbuf = databuf;

	KLAV_VIO_Buffer * pb = m_wbuf_first;
	KLAV_ERR err = KLAV_OK;

	while (remain != 0)
	{
		uint32_t portion = 0;

		if (pb == 0 || pos < pb->m_iopos)
		{
			// read portion from file
			portion = remain;
			if (pb != 0 && (klav_filepos_t) portion > (pb->m_iopos - pos))
				portion = (uint32_t) (pb->m_iopos - pos);

			uint32_t phys_portion = 0;
			if (pos < m_phys_io_size)
			{
				phys_portion = portion;
				if ((klav_filepos_t) phys_portion > (m_phys_io_size - pos))
					phys_portion = (uint32_t) (m_phys_io_size - pos);
			}

			if (phys_portion != 0)
			{
				// perform physical read
				err = m_base->seek_read (m_io_origin + pos, dbuf, phys_portion, pnrd);
				if (KLAV_FAILED (err))
					break;

				if (pnrd != 0 && *pnrd != phys_portion)
				{
					// we read less data than expected
					portion = *pnrd;
					pos += portion;
					dbuf += portion;
					remain -= portion;
					break;
				}
			}

			if (phys_portion < portion)
			{
				// fill remaining buffer with zero data
				::memset (dbuf + phys_portion, 0, portion - phys_portion);
			}
		}
		else
		{
			// copy portion from buffer
			klav_filepos_t pb_end = pb->m_iopos + pb->m_size;

			if (pos < pb_end)
			{
				uint32_t off = (uint32_t)(pos - pb->m_iopos);
				portion = pb->m_size - off;
				if (portion > remain)
					portion = remain;

				::memcpy (dbuf, pb->m_data + off, portion);
			}

			pb = pb->m_next;
		}

		pos += portion;
		dbuf += portion;
		remain -= portion;
	}

	if (pnrd != 0)
		*pnrd = size_avail - remain;

	return err;
}

void KLAV_VIO_Object::release_written_buffers ()
{
	KLAV_VIO_Buffer *buf = m_wbuf_first;
	m_wbuf_first = 0;
	m_wbuf_last = 0;

	while (buf != 0)
	{
		KLAV_VIO_Buffer * next = buf->m_next;

		// TODO: zero-fill in debug mode
		if (buf->m_data != 0)
			m_allocator->free (buf->m_data);

		buf->clear ();

		m_vbuf_pool.free (buf);
		buf = next;
	}
}

void KLAV_VIO_Object::insert_written_buffer_before (
		KLAV_VIO_Buffer *buf,
		KLAV_VIO_Buffer *next
	)
{
	buf->m_next = next;

	if (next != 0)
	{
		buf->m_prev = next->m_prev;
		next->m_prev = buf;
	}
	else
	{
		buf->m_prev = m_wbuf_last;
		m_wbuf_last = buf;
	}

	if (buf->m_prev != 0)
		buf->m_prev->m_next = buf;
	else
		m_wbuf_first = buf;
}

void KLAV_VIO_Object::release_written_buffer (KLAV_VIO_Buffer *buf)
{
	if (buf->m_prev != 0)
		buf->m_prev->m_next = buf->m_next;
	else
		m_wbuf_first = buf->m_next;

	if (buf->m_next != 0)
		buf->m_next->m_prev = buf->m_prev;
	else
		m_wbuf_last = buf->m_prev;

	buf->clear ();

	m_vbuf_pool.free (buf);
}

KLAV_ERR KLAV_Create_Virtual_IO (
			KLAV_Alloc *allocator,
			KLAV_IO_Object *base_io,
			klav_filepos_t origin,
			KLAV_IO_Object **ppobj
		)
{
	KLAV_ERR err = KLAV_OK;
	*ppobj = 0;

	KLAV_VIO_Object * vio = KLAV_NEW (allocator) KLAV_VIO_Object (allocator);
	if (vio == 0)
		return KLAV_ENOMEM;

	err = vio->init_io (base_io, origin);
	if (KLAV_FAILED (err))
	{
		vio->destroy ();
		return err;
	}

	*ppobj = vio;
	return KLAV_OK;
}

