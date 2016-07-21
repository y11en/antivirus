// pr_io.cpp
//
// Prague implementation of KLAVA IO interfaces
//

#include <klava/klav_prague.h>
#include <klava/klav_props.h>

#include <plugin/p_nfio.h>
#include <pr_pid.h>

#ifdef _MSC_VER
# pragma intrinsic (memset)
# pragma intrinsic (memcpy)
# pragma intrinsic (strlen)
#endif // _MSC_VER

////////////////////////////////////////////////////////////////
// IO Object

class KLAV_Pr_IO_Object : public KLAV_IFACE_IMPL(KLAV_IO_Object)
{
public:
			KLAV_Pr_IO_Object (hOBJECT parent, hIO hio, klav_bool_t owner);
	virtual ~KLAV_Pr_IO_Object ();

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

	KLAV_Alloc * allocator ()
		{ return & m_allocator; }

	void cleanup ();

private:
	KLAV_Pr_Alloc            m_allocator;
	hIO                      m_hio;
	KLAV_Property_Bag_Holder m_props;
	klav_filepos_t           m_current_pos;
	klav_bool_t              m_is_owner;

	KLAV_Pr_IO_Object (const KLAV_Pr_IO_Object&);
	KLAV_Pr_IO_Object& operator= (const KLAV_Pr_IO_Object&);
};

////////////////////////////////////////////////////////////////

KLAV_Pr_IO_Object::KLAV_Pr_IO_Object (
		hOBJECT parent,
		hIO hio,
		klav_bool_t owner
	) :
		m_allocator (parent),
		m_hio (hio),
		m_current_pos (0),
		m_is_owner (owner)
{
	m_props = KLAV_Property_Bag_Create (allocator ());
}

KLAV_Pr_IO_Object::~KLAV_Pr_IO_Object ()
{
	cleanup ();
}

void KLAV_Pr_IO_Object::cleanup ()
{
	if (m_hio != 0)
	{
		if (m_is_owner)
		{
			m_hio->sysCloseObject ();
		}
		m_hio = 0;
	}
	m_current_pos = 0;

	if (m_props.get () != 0)
	{
		m_props->clear_properties ();
	}
}

void KLAV_CALL KLAV_Pr_IO_Object::destroy ()
{
	KLAV_DELETE (this, allocator ());
}

uint32_t KLAV_CALL KLAV_Pr_IO_Object::get_io_version ()
{
	return KLAV_IO_VERSION_CURRENT;
}

void * KLAV_CALL KLAV_Pr_IO_Object::get_io_iface (
				int ifc_id
			)
{
	switch (ifc_id)
	{
	case KLAV_IFIO_PR_HOBJECT:
		return (hOBJECT) m_hio;
	case KLAV_IFIO_PR_HIO:
		return (hIO) m_hio;
	default:
		break;
	}
	return 0;
}

KLAV_ERR KLAV_CALL KLAV_Pr_IO_Object::seek_read (
			klav_filepos_t pos,
			void *buf,
			uint32_t size,
			uint32_t *pnrd
		)
{
	if (m_hio == 0)
		return KLAV_ENOINIT;

	tDWORD result = 0;
	tERROR error = m_hio->SeekRead (& result, pos, buf, size);
	if (pnrd != 0)
		*pnrd = result;

	if (PR_FAIL (error))
		return KLAV_PR_ERROR (error);

	if (pnrd == 0 && result != size)
		return KLAV_EREAD;

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Pr_IO_Object::seek_write (
			klav_filepos_t pos,
			const void *buf,
			uint32_t size,
			uint32_t *pnwr
		)
{
	if (m_hio == 0)
		return KLAV_ENOINIT;

	tDWORD result = 0;
	tERROR error = m_hio->SeekWrite (& result, pos, (tPTR) buf, size);
	if (pnwr != 0)
		*pnwr = result;

	if (PR_FAIL (error))
		return KLAV_PR_ERROR (error);

	if (pnwr == 0 && result != size)
		return KLAV_EWRITE;

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Pr_IO_Object::get_size (klav_filepos_t *psize)
{
	if (m_hio == 0)
		return KLAV_ENOINIT;

	*psize = 0;

	tQWORD size = 0;
	tERROR error = m_hio->GetSize (& size, IO_SIZE_TYPE_EXPLICIT);
	if (PR_FAIL (error))
		return KLAV_PR_ERROR (error);

	*psize = (klav_filepos_t) size;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Pr_IO_Object::set_size (klav_filepos_t size)
{
	if (m_hio == 0)
		return KLAV_ENOINIT;

	tERROR error = m_hio->SetSize (size);
	if (PR_FAIL (error))
		return KLAV_PR_ERROR (error);

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Pr_IO_Object::flush ()
{
	if (m_hio == 0)
		return KLAV_ENOINIT;

	tERROR error = m_hio->Flush ();
	if (PR_FAIL (error))
		return KLAV_PR_ERROR (error);

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Pr_IO_Object::ioctl (
			uint32_t code,
			void *buf,
			size_t bufsize
		)
{
	if (m_hio == 0)
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
		* ((uint64_t *)buf) = (uint64_t)m_hio->propGetPtr(plNATIVE_HANDLE);
		break;

	default:
		return KLAV_ENOIMPL;
	}

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Pr_IO_Object::query_map (
			klav_filepos_t pos,
			uint32_t       size,
			uint32_t *     pflags
		)
{
	if (m_hio == 0)
		return KLAV_ENOINIT;

	*pflags = 0;      // until mapping is implemented
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Pr_IO_Object::map_data (
			klav_filepos_t pos,
			uint32_t       size,
			uint32_t       flags,
			klav_iomap_t * iomap
		)
{
	iomap->clear ();
	return KLAV_ENOTIMPL;
}

KLAV_ERR KLAV_CALL KLAV_Pr_IO_Object::unmap_data (klav_iomap_t * iomap)
{
	iomap->clear ();
	return KLAV_EINVAL;
}

KLAV_ERR KLAV_CALL KLAV_Pr_IO_Object::close ()
{
	if (m_hio == 0)
		return KLAV_EALREADY;

	tERROR error = errOK;

	if (m_is_owner)
	{
		error = m_hio->sysCloseObject ();
	}
	else
	{
		// set ACCESS_NONE (object should close handle)
		error = m_hio->set_pgOBJECT_ACCESS_MODE (0);
	}

	m_hio = 0;
	
	if (PR_FAIL (error))
		return KLAV_PR_ERROR (error);

	return KLAV_OK;
}

KLAV_Properties * KLAV_CALL KLAV_Pr_IO_Object::get_properties ()
{
	return m_props;
}

////////////////////////////////////////////////////////////////

KLAV_ERR KLAV_CALL
KLAV_Pr_Create_IO_Object (hOBJECT parent, hIO hio, klav_bool_t owner, KLAV_IO_Object **ppobj)
{
	KLAV_Pr_Alloc allocator (parent);
	*ppobj = 0;

	KLAV_Pr_IO_Object * io_obj = KLAV_NEW (& allocator) KLAV_Pr_IO_Object (parent, hio, owner != 0);
	if (io_obj == 0)
	{
		return KLAV_ENOMEM;
	}

	*ppobj = io_obj;
	return KLAV_OK;
}

