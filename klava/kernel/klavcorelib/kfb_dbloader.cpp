// kfb_dbloader.cpp
//
//

#include <klava/klavdef.h>
#include <klava/klavdb.h>
#include <klava/klav_utils.h>

#include <string.h>
#include <klava/crc32.h>

// specific alignment requirements for VM sections
#if defined KL_ARCH_PPC
// PowerPC code sections must be aligned to 64K
# define VM_SEGMENT_ALIGNMENT 0x10000
#else
// 4K default alignment should fit all others
# define VM_SEGMENT_ALIGNMENT 0x1000
#endif

////////////////////////////////////////////////////////////////

enum Sect_Flags
{
	SECT_F_ALLOC       = 0x01,   // fragment data was allocated
	SECT_F_VIRTUAL     = 0x02,   // fragment data was allocated using VM
};

struct KLAV_KFB_Loader::Section_Desc
{
	uint32_t     m_section_type;
	uint32_t     m_fragment_id;
	uint32_t     m_section_off;   // section offset
	uint32_t     m_section_size;  // nominal size
	uint32_t     m_section_csize; // compressed size
	uint32_t     m_section_crc32; // section CRC32
	klav_iomap_t m_iomap;        // iomap structure
	uint8_t *    m_align_data;    // (aligned) section data
	uint8_t *    m_alloc_data;    // allocated data
	uint32_t     m_alloc_size;    // allocated size
	uint16_t     m_alignment;     // alignment requirements
	uint16_t     m_kfb_flags;     // KFB section flags (KFB_SECT_F_XXX)
	uint16_t     m_flags;         // runtime flags, e.g. SECT_F_VIRTUAL
	uint8_t      m_prot;          // protection mode
	uint8_t      m_cmethod;       // compression method

	Section_Desc ()
	{
		memset (this, 0, sizeof (Section_Desc));
	}
};

////////////////////////////////////////////////////////////////

KLAV_KFB_Loader::KLAV_KFB_Loader (KLAV_Alloc * alloc, KLAV_Virtual_Memory *vm)
	: m_alloc (alloc),
	  m_vm (vm),
	  m_io (0),
	  m_kfb_release (0),
	  m_kfb_version (0),
	  m_sections (0),
	  m_kparms (0),
	  m_section_count (0),
	  m_kparm_count (0),
	  m_loadable_size (0),
	  m_resident_size (0),
	  m_open_mode (0),
	  m_kfb2_format (false),
	  m_opened (false)
{
}

KLAV_KFB_Loader::~KLAV_KFB_Loader ()
{
	cleanup ();
}

void KLAV_KFB_Loader::destroy ()
{
	KLAV_DELETE (this, m_alloc);
}

void KLAV_KFB_Loader::init (KLAV_Alloc *alloc, KLAV_Virtual_Memory *vm)
{
	m_alloc = alloc;
	m_vm = vm;
}

KLAV_ERR KLAV_KFB_Loader::open (const void *mapped_data, uint32_t mapped_size, uint32_t mode)
{
	if (m_opened)
		return KLAV_EALREADY;

	KLAV_IO_Object * buf_io = 0;
	KLAV_ERR err = KLAV_Buffer_IO_Create (m_alloc, mapped_data, mapped_size, true, & buf_io);
	if (KLAV_FAILED (err))
		return err;

	m_io = buf_io;
	m_open_mode = mode | DESTROY_IO;

	err = do_open ();
	if (KLAV_FAILED (err))
	{
		cleanup (); // will destroy IO object
		return err;
	}

	return KLAV_OK;
}

KLAV_ERR KLAV_KFB_Loader::open_io (KLAV_IO_Object *io, uint32_t mode)
{
	if (m_opened)
		return KLAV_EALREADY;

	m_io = io;
	m_open_mode = (mode & ~DESTROY_IO);

	KLAV_ERR err = do_open ();
	if (KLAV_FAILED (err))
	{
		// do not destroy IO if open failed
		cleanup ();
		return err;
	}

	m_open_mode |= (mode & DESTROY_IO);
	return KLAV_OK;
}

KLAV_ERR KLAV_KFB_Loader::do_open ()
{
	KLAV_ERR err = KLAV_OK;

	klav_filepos_t file_size = 0;
	err = m_io->get_size (& file_size);

	if (KLAV_FAILED (err))
		return handle_error (err);

	union {
		KFB1_FILE_HEADER kfb1;
		KFB2_FILE_HEADER kfb2;
	} hdr;

	err = m_io->seek_read (0, &hdr, sizeof(hdr), 0);
	if (KLAV_FAILED (err))
		return err;

	if (hdr.kfb1.kfb_magic [0] == KFB1_MAGIC_0
	 && hdr.kfb1.kfb_magic [1] == KFB1_MAGIC_1
	 && hdr.kfb1.kfb_magic [2] == KFB1_MAGIC_2
	 && hdr.kfb1.kfb_magic [3] == KFB1_MAGIC_3)
	{
		// read KFB1 file structure
		err = open_kfb1 (& hdr.kfb1, file_size);
	}
	else if (hdr.kfb2.kfb_magic [0] == KFB2_MAGIC_0
	 && hdr.kfb2.kfb_magic [1] == KFB2_MAGIC_1
	 && hdr.kfb2.kfb_magic [2] == KFB2_MAGIC_2
	 && hdr.kfb2.kfb_magic [3] == KFB2_MAGIC_3)
	{
		// read KFB2 file structure
		err = open_kfb2 (& hdr.kfb2, file_size);
	}
	else
	{
		return handle_error (KLAV_EFORMAT);
	}

	if (KLAV_FAILED (err))
		return handle_error (err);

	// check section table for validity
	m_resident_size = 0;
	m_loadable_size = 0;

	uint32_t i = 0;
	for (; i < m_section_count; ++i)
	{
		Section_Desc& sect = m_sections [i];

		if (sect.m_section_off > file_size
		 || (file_size - sect.m_section_off) < sect.m_section_csize)
			return handle_error (KLAV_ECORRUPT);

		switch (sect.m_cmethod)
		{
		case KFB_COMPRESSION_METHOD_NONE:
			sect.m_kfb_flags &= ~KFB_SECT_F_COMPRESSED;
			if (sect.m_section_size != sect.m_section_csize)
				return handle_error (KLAV_ECORRUPT);
			break;
		case KFB_COMPRESSION_METHOD_LZMA:
			if ((sect.m_kfb_flags & KFB_SECT_F_COMPRESSED) == 0)
				return handle_error (KLAV_ECORRUPT);
			break;
		default:
			return handle_error (KLAV_ENOTIMPL);
		}

		uint32_t topsz = sect.m_section_off + sect.m_section_csize;

		switch (sect.m_kfb_flags & KFB_SECT_F_LOADABILITY_MASK)
		{
		case KFB_SECT_F_RESIDENT:
			if (topsz > m_resident_size)
				m_resident_size = topsz;
			// fallback
		case KFB_SECT_F_LOADABLE:
			if (topsz > m_loadable_size)
				m_loadable_size = topsz;
			// fallback
		case KFB_SECT_F_NONLOADABLE:
		case 0: // nonloadable too
			break;
		default:
			return handle_error (KLAV_ECORRUPT);
		}
	}

	if (m_loadable_size < m_resident_size)
		m_loadable_size = m_resident_size;

	// read kernel parameters
	err = load_kparms ();
	if (KLAV_FAILED (err) && err != KLAV_ENOTFOUND)
		return handle_error(err);

	m_opened = true;

	return KLAV_OK;
}

KLAV_ERR KLAV_KFB_Loader::open_kfb1 (struct KFB1_FILE_HEADER *hdr, klav_filepos_t file_size)
{
	KLAV_ERR err = KLAV_OK;
	m_kfb2_format = false;

#ifdef KL_BIG_ENDIAN
	if ((hdr->kfb_flags & KFB_F_BIG_ENDIAN) == 0)
		return handle_error (KLAV_EFORMAT);
#else
	if ((hdr->kfb_flags & KFB_F_BIG_ENDIAN) != 0)
		return handle_error (KLAV_EFORMAT);
#endif

	uint8_t  hdr_words = hdr->kfb_hdrsize;
	uint32_t hdr_words_min = sizeof (KFB1_FILE_HEADER) / sizeof (uint32_t);

	if (hdr_words < hdr_words_min)
		return handle_error (KLAV_EFORMAT);

	if (file_size < (hdr_words * sizeof (uint32_t)))
		return handle_error (KLAV_EFORMAT);

	uint32_t arch_id = hdr->kfb_arch_id;
	if (! (arch_id == 0 || arch_id == KDBID_ARCH_CURRENT))
		return handle_error (KLAV_EFORMAT);

	uint32_t sectbl_off = hdr->kfb_sectbl_off;
	uint32_t sectbl_size = hdr->kfb_sectbl_size;

	if (sectbl_off < hdr_words * sizeof (uint32_t)
	 || sectbl_off > file_size
	 || (file_size - sectbl_off) < sectbl_size)
		return handle_error (KLAV_ECORRUPT);

	m_section_count = sectbl_size / sizeof (KFB1_SECTION_TABLE_RECORD);
	if (sectbl_size % sizeof (KFB1_SECTION_TABLE_RECORD) != 0)
		return KLAV_ECORRUPT;

	if (m_section_count > 0)
	{
		m_sections = (Section_Desc *) m_alloc->alloc (m_section_count * sizeof (Section_Desc));
		if (m_sections == 0)
			return KLAV_ENOMEM;
		memset (m_sections, 0, m_section_count * sizeof (Section_Desc));

		KFB1_SECTION_TABLE_RECORD * sbuf = (KFB1_SECTION_TABLE_RECORD *)
			m_alloc->alloc (m_section_count * sizeof (KFB1_SECTION_TABLE_RECORD));

		if (sbuf == 0)
			return handle_error (KLAV_ENOMEM);

		err = m_io->seek_read (sectbl_off, sbuf, sectbl_size, 0);
		if (KLAV_FAILED (err))
		{
			m_alloc->free (sbuf);
			return handle_error (err);
		}

		for (uint32_t i = 0; i < m_section_count; ++i)
		{
			KFB1_SECTION_TABLE_RECORD& s = sbuf [i];
			Section_Desc& sect = m_sections [i];

			sect.m_section_type  = s.kfb_section_type;
			sect.m_fragment_id   = s.kfb_fragment_id;
			sect.m_section_off   = s.kfb_section_off;
			sect.m_section_size  = s.kfb_section_size;
			sect.m_section_csize = s.kfb_section_size;
			sect.m_kfb_flags     = KFB_SECT_F_RESIDENT | KFB_SECT_F_VIRTUAL;
			sect.m_alignment     = KFB_ALIGNMENT_16;
			sect.m_cmethod       = KFB_COMPRESSION_METHOD_NONE;
			sect.m_flags         = 0;
		}

		m_alloc->free (sbuf);
	}

	return KLAV_OK;
}

KLAV_ERR KLAV_KFB_Loader::open_kfb2 (struct KFB2_FILE_HEADER *hdr, klav_filepos_t file_size)
{
	KLAV_ERR err = KLAV_OK;
	m_kfb2_format = true;

#ifdef KL_BIG_ENDIAN
	if ((hdr->kfb_flags & KFB_F_BIG_ENDIAN) == 0)
		return handle_error (KLAV_EFORMAT);
#else
	if ((hdr->kfb_flags & KFB_F_BIG_ENDIAN) != 0)
		return handle_error (KLAV_EFORMAT);
#endif

	uint8_t  hdr_words = hdr->kfb_hdrsize;
	uint32_t hdr_words_min = sizeof (KFB2_FILE_HEADER) / sizeof (uint32_t);

	if (hdr_words < hdr_words_min)
		return handle_error (KLAV_EFORMAT);

	if (file_size < (hdr_words * sizeof (uint32_t)))
		return handle_error (KLAV_EFORMAT);

	uint32_t arch_id = hdr->kfb_arch_id;
	if (! (arch_id == 0 || arch_id == KDBID_ARCH_CURRENT))
		return handle_error (KLAV_EFORMAT);

	uint32_t sectbl_off = hdr->kfb_sectbl_off;
	uint32_t sectbl_size = hdr->kfb_sectbl_size;

	if (sectbl_off < hdr_words * sizeof (uint32_t)
	 || sectbl_off > file_size
	 || (file_size - sectbl_off) < sectbl_size)
		return handle_error (KLAV_ECORRUPT);

	uint32_t record_size = (uint32_t) hdr->kfb_secrec_size * sizeof (uint32_t);
	if (record_size < sizeof (KFB2_SECTION_TABLE_RECORD))
		return KLAV_ECORRUPT;

	m_section_count = sectbl_size / record_size;
	if (sectbl_size % record_size != 0)
		return KLAV_ECORRUPT;

	if (m_section_count > 0)
	{
		m_sections = (Section_Desc *) m_alloc->alloc (m_section_count * sizeof (Section_Desc));
		if (m_sections == 0)
			return KLAV_ENOMEM;

		memset (m_sections, 0, m_section_count * sizeof (Section_Desc));

		uint8_t * sbuf = m_alloc->alloc (sectbl_size);
		if (sbuf == 0)
			return KLAV_ENOMEM;

		err = m_io->seek_read (sectbl_off, sbuf, sectbl_size, 0);
		if (KLAV_FAILED (err))
		{
			m_alloc->free (sbuf);
			return handle_error (err);
		}

		for (uint32_t i = 0; i < m_section_count; ++i)
		{
			KFB2_SECTION_TABLE_RECORD& s = *((KFB2_SECTION_TABLE_RECORD *)(sbuf + i * record_size));
			Section_Desc& sect = m_sections [i];

			sect.m_section_type  = s.kfb_section_type;
			sect.m_fragment_id   = s.kfb_fragment_id;
			sect.m_section_off   = s.kfb_section_off;
			sect.m_section_size  = s.kfb_section_usize;
			sect.m_section_csize = s.kfb_section_csize;
			sect.m_section_crc32 = s.kfb_section_crc32;
			sect.m_kfb_flags = s.kfb_section_flags;
			sect.m_alignment = s.kfb_section_alignment;
			sect.m_cmethod   = s.kfb_section_cmethod;
			sect.m_flags     = 0;
		}

		m_alloc->free (sbuf);
	}

	return KLAV_OK;
}

KLAV_ERR KLAV_KFB_Loader::load_kparms ()
{
	int idx = find_section (KDBID_KERNEL_PARMS, 0);
	if (idx < 0)
		return KLAV_ENOTFOUND;

	Section_Desc& sect = m_sections [idx];

	KLAV_ERR err = load_section_data (sect, KLAV_DBLOADER_FLAG_READ);
	if (KLAV_FAILED (err))
	{
		release_section (idx);
		return handle_error (err);
	}

	m_kparms = (KFB_KERNEL_PARM *) sect.m_align_data;
	m_kparm_count = sect.m_section_size / sizeof (KFB_KERNEL_PARM);

	return KLAV_OK;
}

KLAV_ERR KLAV_KFB_Loader::close ()
{
	cleanup ();
	return KLAV_OK;
}

KLAV_ERR KLAV_KFB_Loader::detach_source ()
{
	uint32_t i = 0;
	for (; i < m_section_count; ++i)
	{
		// some sections are mapped?
		Section_Desc& sect = m_sections [i];
		if (sect.m_iomap.iomap_data != 0)
			return KLAV_ECONFLICT;
	}

	if (m_io != 0)
	{
		if ((m_open_mode & DESTROY_IO) != 0)
			m_io->destroy ();
		m_io = 0;
	}

	return KLAV_OK;
}

void KLAV_KFB_Loader::cleanup ()
{
	delete_sections ();

	if ((m_open_mode & DESTROY_IO) != 0)
		m_io->destroy ();

	m_io = 0;

	m_kfb_release = 0;
	m_kfb_version = 0;
	m_sections = 0;
	m_kparms = 0;
	m_section_count = 0;
	m_kparm_count = 0;
	m_loadable_size = 0;
	m_resident_size = 0;
	m_open_mode = 0;
	m_kfb2_format = false;
	m_opened = false;
}

void KLAV_KFB_Loader::release_section (uint32_t idx)
{
	if (idx >= m_section_count || m_sections == 0)
		return;

	Section_Desc& sect = m_sections [idx];

	if (sect.m_alloc_data != 0 && (sect.m_flags & SECT_F_ALLOC) != 0)
	{
		if ((sect.m_flags & SECT_F_VIRTUAL) != 0)
		{
			m_vm->vm_free (sect.m_alloc_data, sect.m_alloc_size);
		}
		else
		{
			m_alloc->free (sect.m_alloc_data);
		}
	}

	if (sect.m_iomap.iomap_data != 0)
	{
		m_io->unmap_data (& sect.m_iomap);
		sect.m_iomap.clear ();
	}

	sect.m_alloc_data = 0;
	sect.m_align_data = 0;
	sect.m_alloc_size = 0;
	sect.m_flags &= ~(SECT_F_ALLOC|SECT_F_VIRTUAL);
}

void KLAV_KFB_Loader::delete_sections ()
{
	if (m_sections == 0)
		return;

	for (uint32_t i = 0; i < m_section_count; ++i)
	{
		release_section (i);
	}

	m_alloc->free (m_sections);
	m_sections = 0;
	m_section_count = 0;
}

KLAV_ERR KLAV_KFB_Loader::load_section_data (
			Section_Desc& sect,
			uint32_t new_prot
		)
{
	KLAV_ERR err = KLAV_OK;

	new_prot &= KLAV_MEM_PROT_MASK;
	if (new_prot == 0)
		new_prot = KLAV_MEM_PROT_READ;

	bool need_decompress = false;
	bool need_alloc = false;
	bool need_vm = false;

	// if protection is set to smth else than READ, use mapped data directly
	if (new_prot != KLAV_MEM_PROT_READ)
		need_alloc = true;

	if (sect.m_cmethod != KFB_COMPRESSION_METHOD_NONE)
		need_decompress = true;

	if (need_decompress)
		need_alloc = true;

	if ((sect.m_kfb_flags & KFB_SECT_F_VIRTUAL) != 0)
		need_vm = true;

	if (sect.m_section_size == 0)
		return KLAV_OK;

	if (sect.m_alloc_data != 0)
	{
		// already allocated/read, just changing protection
		if (sect.m_prot != new_prot)
		{
			if ((sect.m_flags & SECT_F_VIRTUAL) != 0)
			{
				err = m_vm->vm_protect (sect.m_alloc_data, sect.m_alloc_size, new_prot);
				if (err != KLAV_OK)
					return err;
			}

			sect.m_prot = (uint8_t) new_prot;
		}

		return KLAV_OK;
	}

	// section data not yet allocated

	if (m_io == 0)  // IO object already released?
		return KLAV_EACCESS;

	if (m_vm == 0)
		need_vm = false; // if VM is unavailable, don't bother..

	if (! need_alloc && allow_map ())
	{
		// try to use file mapping
		uint32_t map_flags = 0;
		err = m_io->query_map (sect.m_section_off, sect.m_section_size, & map_flags);
		if (KLAV_FAILED (err))
			map_flags = 0;
			
		if ((map_flags & KLAV_IOMAP_F_ENTIRE) != 0)
		{
			err = m_io->map_data (sect.m_section_off, sect.m_section_size, 0, & sect.m_iomap);
			if (KLAV_SUCCEEDED (err))
			{
				// check CRC32
				sect.m_alloc_data = (uint8_t *) sect.m_iomap.iomap_data;
				sect.m_align_data = (uint8_t *) sect.m_iomap.iomap_data;
				sect.m_prot = (uint8_t) new_prot;

				if ((sect.m_kfb_flags & KFB_SECT_F_CRC32) != 0)
				{
					uint32_t crc = KlavCRC32 (sect.m_align_data, sect.m_section_size, 0);
					if (crc != sect.m_section_crc32)
						return handle_error (KLAV_ECORRUPT);
				}

				return KLAV_OK;
			}
		}
	}

	if (need_vm)
	{
		// allocate using virtual memory

		// determine virtual memory size to allocate (respecting segment alignment factor)
		uint32_t pagesize = m_vm->vm_pagesize();
		uint32_t alloc_size = sect.m_section_size;

		if (pagesize < VM_SEGMENT_ALIGNMENT)
			alloc_size += VM_SEGMENT_ALIGNMENT;
		
		uint8_t * alloc_data = 0;
		err = m_vm->vm_alloc ((void **)& alloc_data, alloc_size, KLAV_MEM_PROT_READ|KLAV_MEM_PROT_WRITE, 0);
		if (err != KLAV_OK)
			return handle_error (err);

		uint8_t * align_data = alloc_data;

		// align data pointer
		if (pagesize < VM_SEGMENT_ALIGNMENT)
		{
			uintptr_t ud = (uintptr_t) alloc_data;
			uintptr_t ad = (ud + (VM_SEGMENT_ALIGNMENT - 1)) & ~(VM_SEGMENT_ALIGNMENT - 1);

			align_data += (ad - ud);
		}
		
		sect.m_flags = SECT_F_ALLOC|SECT_F_VIRTUAL;
		sect.m_prot = KLAV_MEM_PROT_WRITE;
		sect.m_alloc_data = alloc_data;
		sect.m_align_data = align_data;
		sect.m_alloc_size = alloc_size;
	}
	else
	{
		// align at least to 16 bytes
		uint32_t alignment = sect.m_alignment;
		if (alignment > KFB_ALIGNMENT_MAX)
			alignment = KFB_ALIGNMENT_MAX;

		uint32_t align_size = KFB_ALIGNMENT_SIZE (alignment);

		if (align_size < 16)
			align_size = 16;

		sect.m_alloc_size = sect.m_section_size + align_size;
		sect.m_alloc_data = m_alloc->alloc (sect.m_alloc_size);
		if (sect.m_alloc_data == 0)
			return KLAV_ENOMEM;

		sect.m_align_data = sect.m_alloc_data;
		uintptr_t ud = (uintptr_t) sect.m_alloc_data;
		uintptr_t ad = (ud + (align_size - 1)) & ~((uintptr_t)align_size - 1);
		sect.m_align_data += (ad - ud);
		sect.m_prot = (uint8_t) new_prot;
		sect.m_flags = SECT_F_ALLOC;
	}

	// decompress if needed
	if (need_decompress)
	{
		// read compressed data to temporary buffer
		uint8_t *cbuf = m_alloc->alloc (sect.m_section_csize);
		if (cbuf == 0)
			return handle_error (KLAV_ENOMEM);

		err = m_io->seek_read (sect.m_section_off, cbuf, sect.m_section_csize, 0);
		if (KLAV_FAILED (err))
		{
			m_alloc->free (cbuf);
			return handle_error (err);
		}

		if ((sect.m_kfb_flags & KFB_SECT_F_CRC32) != 0)
		{
			uint32_t crc = KlavCRC32 (cbuf, sect.m_section_csize, 0);
			if (crc != sect.m_section_crc32)
			{
				m_alloc->free (cbuf);
				return handle_error (KLAV_ECORRUPT);
			}
		}

		err = KLAV_LZMA_Decompress (m_alloc, cbuf, sect.m_section_csize, sect.m_align_data, sect.m_section_size);
		m_alloc->free (cbuf);
		if (KLAV_FAILED (err))
			return handle_error (err);
	}
	else
	{
		// direct read data
		err = m_io->seek_read (sect.m_section_off, sect.m_align_data, sect.m_section_size, 0);
		if (KLAV_FAILED (err))
			return handle_error (err);

		if ((sect.m_kfb_flags & KFB_SECT_F_CRC32) != 0)
		{
			uint32_t crc = KlavCRC32 (sect.m_align_data, sect.m_section_size, 0);
			if (crc != sect.m_section_crc32)
				return handle_error (KLAV_ECORRUPT);
		}
	}

	if ((sect.m_flags & SECT_F_VIRTUAL) != 0 && sect.m_prot != new_prot)
	{
		// need to change protection mode
		err = m_vm->vm_protect (sect.m_alloc_data, sect.m_alloc_size, new_prot);
		if (err != KLAV_OK)
			return handle_error (err);

		sect.m_prot = (uint8_t) new_prot;
	}

	return KLAV_OK;
}

KLAV_ERR KLAV_KFB_Loader::handle_error (KLAV_ERR err)
{
	return err;
}

int KLAV_KFB_Loader::find_section (uint32_t section_type, uint32_t fragment_id) const
{
	for (uint32_t i = 0; i < m_section_count; ++i)
	{
		const Section_Desc& sect = m_sections [i];
		if (sect.m_section_type == section_type
		 && sect.m_fragment_id == fragment_id)
		{
			return i;
		}
	}
	
	return -1;
}

KLAV_ERR KLAV_CALL KLAV_KFB_Loader::load_fragment (uint32_t sectionType, uint32_t fragmentID, uint32_t flags, KLAV_DB_Fragment* dbFragment)
{
	uint32_t prot = flags & KLAV_MEM_PROT_MASK;

	if (prot == 0 || dbFragment == 0)
		return KLAV_EINVAL;

	int idx = find_section (sectionType, fragmentID);
	if (idx < 0)
		return KLAV_ENOTFOUND;

	Section_Desc& sect = m_sections [idx];
	KLAV_ERR err = load_section_data (sect, prot);
	if (err != KLAV_OK)
	{
		release_section (idx);
		return err;
	}

	dbFragment->kdb_handle = (void *)(size_t)(idx + 1);
	dbFragment->kdb_data = sect.m_align_data;
	dbFragment->kdb_size = sect.m_section_size;

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_KFB_Loader::release_fragment (KLAV_DB_Fragment *dbFragment)
{
	if (dbFragment == 0) return KLAV_EINVAL;

	uint32_t idx = (uint32_t)((size_t) dbFragment->kdb_handle - 1);
	if (idx >= m_section_count)
		return KLAV_EINVAL;

	release_section (idx);

	dbFragment->clear ();

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_KFB_Loader::protect_fragment (KLAV_DB_Fragment *dbFragment, uint32_t new_prot)
{
	new_prot &= KLAV_MEM_PROT_MASK;

	if (dbFragment == 0 || new_prot == 0)
		return KLAV_EINVAL;

	uint32_t idx = (uint32_t)((size_t)dbFragment->kdb_handle - 1);
	if (idx >= m_section_count)
		return KLAV_EINVAL;

	Section_Desc& sect = m_sections [idx];
	if (sect.m_align_data == 0)
		return KLAV_EINVAL;

	KLAV_ERR err = load_section_data (sect, new_prot);
	if (err != KLAV_OK)
		return err;

	dbFragment->kdb_data = sect.m_align_data;
	return KLAV_OK;
}

uint32_t KLAV_CALL KLAV_KFB_Loader::get_engine_parm (uint32_t id)
{
	for (uint32_t i = 0; i < m_kparm_count; ++i)
	{
		const KFB_KERNEL_PARM& parm = m_kparms [i];
		if (parm.kfb_kparm_id == id)
			return parm.kfb_kparm_value;
	}
	return 0;
}

KLAV_ERR KLAV_CALL KLAV_KFB_Loader::get_ldr_iface (uint32_t id, void **pifc)
{
	*pifc = 0;
	return KLAV_ENOTIMPL;
}

uint32_t KLAV_CALL KLAV_KFB_Loader::get_db_version ()
{
	return m_kfb_version;
}

uint32_t KLAV_CALL KLAV_KFB_Loader::get_db_release ()
{
	return m_kfb_release;
}
