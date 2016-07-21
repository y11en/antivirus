// file_db.cpp
//
//

#include "tns_db.h"

#include <fcntl.h>
#include <sys/stat.h>

#ifdef _WIN32
# include <io.h>
# define fd_open(NAME,MODE,PROT) _open (NAME,MODE,PROT)
# define fd_close(FD)            _close(FD)
#if _MSC_VER > 1200
# define stat_struct             struct _stat64i32
#else
# define stat_struct             struct _stat
#endif
# define fd_stat(FD,ST)          _fstat(FD,ST)
# define fd_setsize(FD,SZ)       _chsize(FD,SZ)
# define fd_read(FD,BUF,SZ)      _read(FD,BUF,SZ)
# define fd_write(FD,BUF,SZ)     _write(FD,BUF,SZ)
# define fd_seek(FD,OFF)         _lseek(FD,OFF,SEEK_SET)
# define O_BINARY                _O_BINARY
#else // _WIN32
# include <unistd.h>
# define fd_open(NAME,MODE,PROT) ::open(NAME,MODE,PROT)
# define fd_close(FD)            ::close(FD)
# define stat_struct             struct stat
# define fd_stat(FD,ST)          ::fstat(FD,ST)
# define fd_setsize(FD,SZ)       ::ftruncate(FD,SZ)
# define fd_read(FD,BUF,SZ)      ::read(FD,BUF,SZ)
# define fd_write(FD,BUF,SZ)     ::write(FD,BUF,SZ)
# define fd_seek(FD,OFF)         ::lseek(FD,OFF,SEEK_SET)
# define O_BINARY                0
#endif // _WIN32

#include <kl_byteswap.h>
#include <vector>

////////////////////////////////////////////////////////////////

struct blk_data
{
	uint8_t * ptr;
	
	blk_data () : ptr (0) {}
	void clear () { ptr = 0; }
};

struct File_Block_Device::Impl
{
	typedef std::vector <blk_data> blk_data_t;
	blk_data_t   m_blk_data;
	int          m_fd;
	uint32_t     m_file_blks;
	
	Impl () : m_fd (-1), m_file_blks (0)
	{
	}

	~Impl ()
	{
		close ();
	}

	bool is_open () const
		{ return m_fd >= 0; }

	KLAV_ERR open (
				const char *fname,
				uint32_t flags,
				const TNDB_TREE_VERSION * ver,
				const TNDB_TREE_PARMS   * parms
			);

	KLAV_ERR set_file_size (uint32_t ncnt);
	KLAV_ERR close ();

	uint8_t * read_block (uint32_t blkno);
	KLAV_ERR  write_block (uint32_t blkno);
	KLAV_ERR  release_block (uint32_t blkno);

	KLAV_ERR  seek_read (uint32_t off, void *buf, uint32_t size);
	KLAV_ERR  seek_write (uint32_t off, const void *buf, uint32_t size);

	uint32_t get_blk_cnt () const
		{ return (uint32_t) m_blk_data.size (); }

	void set_blk_cnt (uint32_t new_cnt);
};

KLAV_ERR File_Block_Device::Impl::open (
			const char *fname,
			uint32_t flags,
			const TNDB_TREE_VERSION * tree_ver,
			const TNDB_TREE_PARMS   * tree_parms
		)
{
	KLAV_ERR err = KLAV_OK;
	unsigned int mode = O_BINARY;
	
	if ((flags & File_Block_Device::OPEN_READONLY) != 0)
	{
		// open read-only
		mode |= O_RDONLY;
	}
	else if ((flags & File_Block_Device::ALLOW_CREATE) != 0)
	{
		mode |= O_RDWR | O_CREAT;
		if ((flags & File_Block_Device::CREATE_ALWAYS) == 0)
		{
			mode |= O_EXCL;
		}
		else
		{
			mode |= O_TRUNC;
		}
	}
	else
	{
		// open read-write
		mode |= O_RDWR;
	}
	
	m_fd = fd_open (fname, mode, S_IREAD|S_IWRITE);
	if (m_fd < 0)
		return KLAV_EUNKNOWN;

	stat_struct st;
	if (fd_stat (m_fd, & st) < 0)
		return KLAV_EREAD;

	uint32_t file_size = (uint32_t) st.st_size;

	m_file_blks = file_size / TNS_BLK_SIZE;

	if (file_size == 0)
	{
		// file has been just created: fill header structure
		err = set_file_size (1);
		if (KLAV_FAILED (err))
			return err;

		TNS_DBF_HDR * hdr = (TNS_DBF_HDR *) read_block (0);
		if (hdr == 0)
			return KLAV_EREAD;

		memset (hdr, 0, TNS_BLK_SIZE);
		hdr->tndb_magic [0] = TNS_DBF_MAGIC_0;
		hdr->tndb_magic [1] = TNS_DBF_MAGIC_1;
		hdr->tndb_magic [2] = TNS_DBF_MAGIC_2;
		hdr->tndb_magic [3] = TNS_DBF_MAGIC_3;
		hdr->tndb_ver_major = TNS_DBF_VER_MAJOR;
		hdr->tndb_ver_minor = TNS_DBF_VER_MINOR;
		hdr->tndb_endian = TNS_DBF_ENDIAN_NATIVE;
		hdr->tndb_release = 0;
		hdr->tndb_seqno = 0;
		hdr->tndb_blk_cnt = 0;

		if (tree_ver != 0)
		{
			hdr->tndb_release = tree_ver->tndb_release;
			hdr->tndb_seqno   = tree_ver->tndb_seqno;
		}

		if (tree_parms != 0)
		{
			hdr->tndb_parms [0] = * tree_parms;
			hdr->tndb_parms [1] = * tree_parms;
		}

		err = write_block (0);
		if (KLAV_FAILED (err))
			return err;
	}
	else
	{
		// read header, get block count, compare to the file size
		m_blk_data.resize(m_file_blks);

		TNS_DBF_HDR * hdr = (TNS_DBF_HDR *) read_block (0);
		if (hdr == 0)
			return KLAV_EREAD;

		if (hdr->tndb_magic [0] != TNS_DBF_MAGIC_0
		 || hdr->tndb_magic [1] != TNS_DBF_MAGIC_1
		 || hdr->tndb_magic [2] != TNS_DBF_MAGIC_2
		 || hdr->tndb_magic [3] != TNS_DBF_MAGIC_3)
			return KLAV_EFORMAT;

		if (hdr->tndb_ver_major != TNS_DBF_VER_MAJOR
		 || hdr->tndb_ver_minor > TNS_DBF_VER_MINOR)
			return KLAV_EFORMAT;

		if (hdr->tndb_endian != TNS_DBF_ENDIAN_NATIVE)
			return KLAV_EFORMAT;

		if ((file_size % TNS_BLK_SIZE) != 0)
			return KLAV_ECORRUPT;

		if (m_file_blks < (hdr->tndb_blk_cnt + TNS_HDR_BLKS))
			return KLAV_ECORRUPT;

		// check version and perform rollback, if required
		if (tree_ver != 0)
		{
			if (hdr->tndb_release != tree_ver->tndb_release)
				return KLAV_ECONFLICT;

			if (hdr->tndb_seqno != tree_ver->tndb_seqno)
			{
				if (tree_ver->tndb_seqno != (hdr->tndb_seqno - 1))
					return KLAV_ECONFLICT;

				// perform rollback
				if ((flags & File_Block_Device::OPEN_READONLY) != 0)
					return KLAV_ECONFLICT; // cannot rollback in read-only mode

				hdr->tndb_seqno--;

				err = write_block (0);
				if (KLAV_FAILED (err))
					return err;
			}
		}

		set_blk_cnt (m_file_blks);
	}

	return err;
}

KLAV_ERR File_Block_Device::Impl::close ()
{
	KLAV_ERR err = KLAV_OK;

	set_blk_cnt (0);

	if (m_fd >= 0)
	{
		if (fd_close (m_fd) < 0)
			err = KLAV_EWRITE;
		m_fd = -1;
	}

	return err;
}

void File_Block_Device::Impl::set_blk_cnt (uint32_t new_cnt)
{
	uint32_t cur_cnt = (uint32_t) m_blk_data.size ();

	for (uint32_t i = new_cnt; i < cur_cnt; ++i)
	{
		uint8_t * ptr = m_blk_data [i].ptr;
		if (ptr != 0)
		{
			free (ptr);
			m_blk_data [i].clear ();
		}
	}
	
	m_blk_data.resize (new_cnt);
}

KLAV_ERR File_Block_Device::Impl::set_file_size (uint32_t ncnt)
{
	uint32_t fsz = ncnt * TNS_BLK_SIZE;

	if (m_file_blks < ncnt)
	{	
		if (fd_setsize (m_fd, fsz) < 0)
			return KLAV_EWRITE;

		// TODO: zero-fill blocks

		m_file_blks = ncnt;
	}

	set_blk_cnt (ncnt);

	return KLAV_OK;
}

KLAV_ERR File_Block_Device::Impl::seek_read (uint32_t off, void *buf, uint32_t size)
{
	int st = fd_seek (m_fd, off);
	if (st < 0)
		return KLAV_EREAD;

	int nr = fd_read (m_fd, buf, size);
	if (nr < 0 || (uint32_t) nr != size)
		return KLAV_EREAD;

	return KLAV_OK;
}

KLAV_ERR File_Block_Device::Impl::seek_write (uint32_t off, const void *buf, uint32_t size)
{
	int st = fd_seek (m_fd, off);
	if (st < 0)
		return KLAV_EWRITE;

	int nw = fd_write (m_fd, buf, size);
	if (nw < 0 || (uint32_t) nw != size)
		return KLAV_EWRITE;

	return KLAV_OK;
}

uint8_t * File_Block_Device::Impl::read_block (uint32_t blkno)
{
	if (blkno >= m_blk_data.size ())
		return 0;

	uint8_t * ptr = m_blk_data [blkno].ptr;
	if (ptr != 0)
		return ptr;

	ptr = (uint8_t *) malloc (TNS_BLK_SIZE);
	if (ptr == 0)
		return 0; // KLAV_ENOMEM

	KLAV_ERR err = seek_read (blkno * TNS_BLK_SIZE, ptr, TNS_BLK_SIZE);
	if (KLAV_FAILED (err))
	{
		free (ptr);
		return 0;
	}
	
	m_blk_data [blkno].ptr = ptr;

	return ptr;
}

KLAV_ERR  File_Block_Device::Impl::write_block (uint32_t blkno)
{
	if (blkno >= m_blk_data.size ())
		return KLAV_EINVAL;

	uint8_t * ptr = m_blk_data [blkno].ptr;
	if (ptr == 0)
		return KLAV_EINVAL;

	KLAV_ERR err = seek_write (blkno * TNS_BLK_SIZE, ptr, TNS_BLK_SIZE);

	return err;
}

KLAV_ERR  File_Block_Device::Impl::release_block (uint32_t blkno)
{
	if (blkno >= m_blk_data.size ())
		return 0;

	uint8_t * ptr = m_blk_data [blkno].ptr;
	if (ptr != 0)
	{
		m_blk_data [blkno].clear ();
		free (ptr);
	}

	return KLAV_OK;
}

////////////////////////////////////////////////////////////////

File_Block_Device::File_Block_Device ()
	: m_pimpl (new Impl ())
{
}

File_Block_Device::~File_Block_Device ()
{
	if (m_pimpl != 0)
	{
		delete m_pimpl;
		m_pimpl = 0;
	}
}

KLAV_ERR File_Block_Device::open (
				const char *name,
				uint32_t flags,
				const TNDB_TREE_VERSION * tree_ver,
				const TNDB_TREE_PARMS   * tree_parms
			)
{
	close ();

	KLAV_ERR err = m_pimpl->open (name, flags, tree_ver, tree_parms);
	if (KLAV_FAILED (err))
		close ();

	return err;
}

KLAV_ERR File_Block_Device::close ()
{
	return m_pimpl->close ();
}

KLAV_ERR File_Block_Device::get_size (uint32_t *pnblks)
{
	*pnblks = 0;
	if (! m_pimpl->is_open ())
		return KLAV_ENOINIT;

	*pnblks = m_pimpl->get_blk_cnt ();
	return KLAV_OK;
}

KLAV_ERR File_Block_Device::set_size (uint32_t nblks)
{
	if (! m_pimpl->is_open ())
		return KLAV_ENOINIT;

	return m_pimpl->set_file_size (nblks);
}

uint8_t * File_Block_Device::read_block (uint32_t blkno)
{
	if (! m_pimpl->is_open ())
		return 0;

	return m_pimpl->read_block (blkno);
}

KLAV_ERR File_Block_Device::write_block (uint32_t blkno)
{
	if (! m_pimpl->is_open ())
		return KLAV_ENOINIT;

	return m_pimpl->write_block (blkno);
}

KLAV_ERR File_Block_Device::release_block (uint32_t blkno)
{
	if (! m_pimpl->is_open ())
		return KLAV_ENOINIT;

	return m_pimpl->release_block (blkno);
}

KLAV_ERR File_Block_Device::flush ()
{
	return KLAV_OK;
}

