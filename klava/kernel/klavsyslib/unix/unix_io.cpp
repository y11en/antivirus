// unix_io.cpp
//
//

#if defined (__unix__)
#if !defined (PATH_MAX)
#define PATH_MAX 256
#endif //PATH_MAX
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include <klavsys_unix.h>
#include <klav_utils.h>
#include <klav_string.h>

////////////////////////////////////////////////////////////////

class KLAV_IO_Library_Unix;

struct KLAV_IO_Object_Unix : public KLAV_IFACE_IMPL(KLAV_IO_Object)
{
	friend class KLAV_IO_Library_Unix;

	KLAV_IO_Object_Unix (KLAV_Alloc * alloc);
	virtual ~KLAV_IO_Object_Unix ();

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

	KLAV_ERR open (int fd);
	
	void cleanup ();

private:
	KLAV_Alloc *        m_allocator;
	int                 m_fd;
	klav_filepos_t      m_current_pos;
	KLAV_Property_Bag_Holder m_props;
	klav_string         m_name;
};

////////////////////////////////////////////////////////////////

KLAV_IO_Object_Unix::KLAV_IO_Object_Unix (KLAV_Alloc * alloc)
  : m_allocator (alloc),
    m_fd (-1),
    m_current_pos (0)
{
	m_props = KLAV_Property_Bag_Create (allocator ());
}

KLAV_IO_Object_Unix::~KLAV_IO_Object_Unix ()
{
  cleanup ();
}

void KLAV_IO_Object_Unix::cleanup ()
{
	if (m_fd >= 0)
	{
		::close (m_fd);
		m_fd = -1;
	}
	m_current_pos = 0;

	if (m_name.length ()) {
		unlink(m_name.c_str());
	}
	    

	if (m_props.get () != 0)
	{
		m_props->clear_properties ();
	}
}

void KLAV_CALL KLAV_IO_Object_Unix::destroy ()
{
	KLAV_DELETE (this, allocator ());
}

uint32_t KLAV_CALL KLAV_IO_Object_Unix::get_io_version ()
{
	return KLAV_IO_VERSION_CURRENT;
}

void * KLAV_CALL KLAV_IO_Object_Unix::get_io_iface (
				int ifc_id
			)
{
	return 0;
}

KLAV_ERR KLAV_IO_Object_Unix::seek_read (klav_filepos_t pos, void* buf, uint32_t size, uint32_t* rsize)
{
  if ( m_fd == -1 ) 
    return KLAV_ENOINIT;
  if ( rsize ) 
    *rsize = 0;
  if ( ::lseek ( m_fd, pos, SEEK_SET ) == -1 ) 
    return KLAV_EUNKNOWN;
  do {
    int l_nread = ::read ( m_fd, buf, size );
    if ( l_nread == -1 ) 
      return KLAV_Get_System_Error ();
    if ( rsize ) {
      *rsize = l_nread;
      return KLAV_OK;
    }
    if ( size && !l_nread )
      return KLAV_EREAD;  
    buf = reinterpret_cast<uint8_t*> ( buf ) + l_nread;
    size -= l_nread;
  }
  while ( size );
  return KLAV_OK;
}

KLAV_ERR KLAV_IO_Object_Unix::seek_write (klav_filepos_t pos, const void* buf, uint32_t size, uint32_t* wsize)
{
  if ( m_fd == -1 ) 
    return KLAV_ENOINIT;
  if ( wsize ) 
    *wsize = 0;
  if ( ::lseek ( m_fd, pos, SEEK_SET ) == -1 ) 
    return KLAV_EUNKNOWN;
  do {
    int l_nwrite = ::write ( m_fd, buf, size );
    if ( l_nwrite == -1 ) 
      return KLAV_Get_System_Error ();
    if ( wsize ) {
      *wsize = l_nwrite;
      return KLAV_OK;
    }
    buf = reinterpret_cast<const uint8_t*> ( buf ) + l_nwrite;
    size -= l_nwrite;
  }
  while ( size );
  return KLAV_OK;
}

KLAV_ERR KLAV_IO_Object_Unix::get_size (klav_filepos_t* size)
{
  if ( m_fd == -1 ) 
    return KLAV_ENOINIT;

  struct stat l_stat;
  if ( ::fstat ( m_fd, &l_stat ) ) 
    return KLAV_Get_System_Error ();
  *size = l_stat.st_size;
  return KLAV_OK;
}

KLAV_ERR KLAV_IO_Object_Unix::set_size (klav_filepos_t size)
{
  if ( m_fd == -1 ) 
    return KLAV_ENOINIT;
  if ( ::ftruncate ( m_fd, size ) ) 
    return KLAV_Get_System_Error ();
  return KLAV_OK;
}

KLAV_ERR KLAV_IO_Object_Unix::flush()
{
  if ( m_fd == -1 ) 
    return KLAV_ENOINIT;
  if ( ::fsync ( m_fd ) ) 
    return KLAV_Get_System_Error ();
  return KLAV_OK;
}


KLAV_ERR KLAV_CALL KLAV_IO_Object_Unix::ioctl (
                                        uint32_t code,
                                        void *buf,
                                        size_t bufsize
                                        )
{
  if (m_fd == -1)
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
		* ((uint64_t *)buf) = m_fd;
		break;
    
	default:
      return KLAV_ENOIMPL;
    }

  return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_IO_Object_Unix::query_map (
			klav_filepos_t pos,
			uint32_t       size,
			uint32_t *     pflags
		)
{
	*pflags = 0;      // until mapping will be implemented
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_IO_Object_Unix::map_data (
			klav_filepos_t pos,
			uint32_t       size,
			uint32_t       flags,
			klav_iomap_t * iomap
		)
{
	iomap->clear ();
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_IO_Object_Unix::unmap_data (
			klav_iomap_t * iomap
		)
{
	iomap->clear ();
	return KLAV_EINVAL;
}

KLAV_ERR KLAV_IO_Object_Unix::close ()
{
	if (m_fd == -1) 
		return KLAV_ENOINIT;

	int st = ::close (m_fd);
	m_fd = -1;
	m_current_pos = 0;

	if (st == -1)
		return KLAV_Get_System_Error ();

	return KLAV_OK;
}

KLAV_Properties * KLAV_CALL KLAV_IO_Object_Unix::get_properties ()
{
	return m_props;
}

KLAV_ERR KLAV_IO_Object_Unix::open (int fd)
{
  if ( fd == -1 )
    return KLAV_EINVAL;
  
  m_fd = fd;
  m_current_pos = 0;
  
  return KLAV_OK;
}

////////////////////////////////////////////////////////////////
// IO library implementation

class KLAV_IO_Library_Unix : 
		public KLAV_IFACE_IMPL(KLAV_IO_Library),
		public KLAV_IFACE_IMPL(KLAV_Temp_Object_Manager)
{
public:
	KLAV_IO_Library_Unix (KLAV_Alloc *alloc);
	virtual ~KLAV_IO_Library_Unix ();

	virtual uint32_t KLAV_CALL get_io_version ();

	// get IO extension interface
	virtual void * KLAV_CALL get_io_iface (
				uint32_t io_iface_id
			);

	virtual KLAV_ERR KLAV_CALL create_file (
				KLAV_CONTEXT_TYPE context,
				const char * path,
				uint32_t access,
				uint32_t flags,
				KLAV_Properties *props,
				KLAV_IO_Object **pobject
			);

	virtual KLAV_ERR KLAV_CALL delete_file (
				KLAV_CONTEXT_TYPE context,
				const char * path,
				uint32_t     flags
			);

	virtual KLAV_ERR KLAV_CALL get_file_attrs (
				KLAV_CONTEXT_TYPE context,
				const char * path,
				uint32_t     attrs_grp,
				uint32_t *   pvalue
			);

	virtual KLAV_ERR KLAV_CALL set_file_attrs (
				KLAV_CONTEXT_TYPE context,
				const char *    path,
				uint32_t        attrs_grp,
				uint32_t        pvalue
			);

	virtual KLAV_ERR KLAV_CALL move_file (
				KLAV_CONTEXT_TYPE context,
				const char * src_path,
				const char * dst_path,
				uint32_t     flags
			);

	virtual KLAV_ERR KLAV_CALL create_temp_object (
				KLAV_CONTEXT_TYPE context,
				KLAV_Properties * props,
				KLAV_IO_Object **ptempobj
			);

	virtual KLAV_ERR KLAV_CALL make_full_path (
				KLAV_CONTEXT_TYPE context,
				const char * src_path,
				uint32_t     flags,
				KLAV_Alloc * path_allocator,
				char ** pdst_path
			);

  KLAV_Alloc * allocator () const
	{ return m_allocator; }

private:
  KLAV_Alloc * m_allocator;
};

////////////////////////////////////////////////////////////////

KLAV_IO_Library_Unix::KLAV_IO_Library_Unix (KLAV_Alloc *alloc)
	: m_allocator (alloc)
{
	if (m_allocator == 0)
		m_allocator = KLAV_Get_System_Allocator ();
}

KLAV_IO_Library_Unix::~KLAV_IO_Library_Unix ()
{
}

uint32_t KLAV_CALL KLAV_IO_Library_Unix::get_io_version ()
{
	return KLAV_IO_VERSION_CURRENT;
}

void * KLAV_CALL KLAV_IO_Library_Unix::get_io_iface (
			uint32_t io_iface_id
		)
{
	return 0;
}

KLAV_ERR KLAV_CALL KLAV_IO_Library_Unix::create_file (
			KLAV_CONTEXT_TYPE context,
			const char * path,
			uint32_t access,
			uint32_t flags,
			KLAV_Properties *props,
			KLAV_IO_Object **pobject
		)
{
  *pobject = 0;

  int l_flags = 0;
  if (access == KLAV_IO_READWRITE)
     l_flags = O_RDWR;
  else if (access == KLAV_IO_READONLY)
    l_flags = O_RDONLY;
  else
    return KLAV_EINVAL;

  switch (flags & KLAV_FILE_CREATE_MODE){
  case KLAV_FILE_CREATE_NEW:
    l_flags |= O_CREAT|O_EXCL;
  case KLAV_FILE_CREATE_ALWAYS:
    l_flags |= O_CREAT|O_TRUNC;
    break;
  case KLAV_FILE_OPEN_EXISTING:
    l_flags |= O_EXCL;
    break;
  case KLAV_FILE_OPEN_ALWAYS:
    l_flags |= O_CREAT;
    break;
  case KLAV_FILE_TRUNCATE_EXISTING:
    l_flags |= O_TRUNC;
    break;
  default:
    return KLAV_EINVAL;
  }

  int l_mode = S_IRUSR;
  if ((flags & KLAV_FILE_F_READ_ONLY) == 0)
    l_mode |= S_IWUSR;

  int fd = -1;

  fd = open ( path, l_flags, l_mode );
  if ( fd == -1 )
    return KLAV_Get_System_Error ();

  KLAV_IO_Object_Unix *io_object = KLAV_NEW (allocator ()) KLAV_IO_Object_Unix (allocator ());
  if (io_object == 0) {
    close (fd);
    return KLAV_ENOMEM;
  }

  KLAV_ERR err = io_object->open (fd);
  if (KLAV_FAILED (err)) {
    io_object->destroy ();
    close (fd);
    return err;
  }

  *pobject = io_object;

  return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_IO_Library_Unix::delete_file (
						      KLAV_CONTEXT_TYPE context,
						      const char * path,
						      uint32_t     flags
						      ) 
{
	KLAV_ERR err = KLAV_OK;

	if (path == 0 || path [0] == 0)
		return KLAV_EINVAL;

	if ( unlink ( path ) )	
		err = KLAV_Get_System_Error ();
	return err;
}

KLAV_ERR KLAV_CALL KLAV_IO_Library_Unix::get_file_attrs (
							 KLAV_CONTEXT_TYPE context,
							 const char * path,
							 uint32_t     attrs_grp,
							 uint32_t *   pvalue
							 ) 
{ 
	if (path == 0 || pvalue == 0 || attrs_grp != KLAV_IO_ATTRS_GENERAL)
		return KLAV_EINVAL;

	struct stat l_stat;
	
	if (stat(path,&l_stat))
		return KLAV_Get_System_Error ();

	uint32_t io_attrs = 0;

	if (S_ISDIR(l_stat.st_mode))
		io_attrs |= KLAV_IO_ATTR_IS_DIRECTORY;
	else if (S_ISCHR(l_stat.st_mode)||S_ISBLK(l_stat.st_mode))
		io_attrs |= KLAV_IO_ATTR_IS_DEVICE;
	else if (S_ISREG(l_stat.st_mode))
		io_attrs |= KLAV_IO_ATTR_IS_FILE;
	else
		return KLAV_EINVAL;
	*pvalue = io_attrs;

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_IO_Library_Unix::set_file_attrs (
							 KLAV_CONTEXT_TYPE context,
							 const char *    path,
							 uint32_t        attrs_grp,
							 uint32_t		 pvalue
							 ) { return KLAV_ENOTIMPL; }

KLAV_ERR KLAV_CALL KLAV_IO_Library_Unix::move_file (
						    KLAV_CONTEXT_TYPE context,
						    const char * src_path,
						    const char * dst_path,
						    uint32_t     flags
						    ) 
{ 
	KLAV_ERR err = KLAV_OK;

	if (src_path == 0 || src_path [0] == 0)
		return KLAV_EINVAL;

	if (dst_path == 0 || dst_path [0] == 0)
		return KLAV_EINVAL;
	
	if ( rename ( src_path, dst_path ) )	
		err = KLAV_Get_System_Error ();
	return err;	
}

KLAV_ERR KLAV_CALL KLAV_IO_Library_Unix::make_full_path (
				KLAV_CONTEXT_TYPE context,
				const char * src_path,
				uint32_t     flags,
				KLAV_Alloc * path_allocator,
				char ** pdst_path
			)
{
	if (pdst_path == 0)
		return KLAV_EINVAL;
		
	*pdst_path = 0;
	if (src_path == 0 || src_path [0] == 0)
		return KLAV_EINVAL;

	if (path_allocator == 0)
		return KLAV_EINVAL;

	// TODO: implement properly...
	size_t len = strlen (src_path);

	*pdst_path = (char *) path_allocator->alloc (len + 1);
	if (*pdst_path == 0)
		return KLAV_ENOMEM;

	strcpy (*pdst_path, src_path);

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_IO_Library_Unix::create_temp_object (
			KLAV_CONTEXT_TYPE context,
			KLAV_Properties * props,
			KLAV_IO_Object **ptempobj
		)
{
  *ptempobj = 0;

  const char * name_prefix = "";

  if (name_prefix == 0)
    name_prefix = "";
  
  char temp_path_buf [PATH_MAX] = {0};
  char pattern [] = "XXXXXX.tmp";
  strncat (temp_path_buf , name_prefix, sizeof (temp_path_buf) - sizeof (pattern) - 1 );
  strcat (temp_path_buf, pattern );

  int fd = mkstemp ( temp_path_buf );
  if (fd == -1 ) {
    KLAV_ERR err = KLAV_Get_System_Error ();
    return err;
  }

  KLAV_IO_Object_Unix *io_object = KLAV_NEW (allocator ()) KLAV_IO_Object_Unix (allocator ());
  if (io_object == 0) {
    close (fd);
    unlink (temp_path_buf );
    return KLAV_ENOMEM;
  }
  
  KLAV_ERR err = io_object->open (fd);
  if (KLAV_FAILED (err)) {
    io_object->destroy ();
    close (fd);
    unlink (temp_path_buf );
    return err;
  }
  io_object->m_name.assign(temp_path_buf, allocator ());
  *ptempobj = io_object;
  
  return KLAV_OK;
}

////////////////////////////////////////////////////////////////

static KLAV_IO_Library_Unix g_io_library (0);

hKLAV_IO_Library KLAVSYS_Get_IO_Library ()
{
	return & g_io_library;
}

hKLAV_Temp_Object_Manager KLAV_CALL KLAVSYS_Get_Temp_Object_Manager ()
{
	return & g_io_library;
}

#endif // __unix__
