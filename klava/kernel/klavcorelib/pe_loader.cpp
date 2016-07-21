// mod_loader.cpp
//
//

#include <klava/klavsys.h>
#include <klava/kl_pe.h>

#include <klava/klav_string.h>
#include <string.h>

#include <klava/klav_utils.h>
#include <crt_s.h>

#ifdef KL_ARCH_X86

////////////////////////////////////////////////////////////////

inline size_t SECT_ALIGN (size_t v, size_t sect_align)
{
	return (v + (sect_align - 1)) & ~(sect_align - 1);
}

inline size_t FILE_ALIGN (size_t v, size_t file_align)
{
	return v & ~(file_align - 1);
}

////////////////////////////////////////////////////////////////

struct KLAV_PE_Module;


class KLAV_PE_Loader_Impl : public KLAV_IFACE_IMPL(KLAV_DSO_Loader)
{
public:
			KLAV_PE_Loader_Impl (
					KLAV_Alloc *alloc,
					const char * module_dir,
					KLAV_Virtual_Memory *vmem,
					KLAV_IO_Library *iolib,
					KLAV_DSO_Loader *native_ldr
				);

	virtual ~KLAV_PE_Loader_Impl ();

	// KLAV_DSO_Loader methods

	virtual KLAV_ERR KLAV_CALL dl_open  (const char *path, void **phdl);
	virtual void *   KLAV_CALL dl_sym   (void *hdl, const char *name);
	virtual void     KLAV_CALL dl_close (void *hdl);

	// helpers

	KLAV_Alloc * allocator () const
		{ return m_allocator; }

	KLAV_Virtual_Memory * vmem () const
		{ return m_vmem; }

	KLAV_IO_Library * iolib () const
		{ return m_iolib; }

	KLAV_DSO_Loader * native_ldr () const
		{ return m_native_ldr; }

private:
	KLAV_Alloc * m_allocator;
	KLAV_Virtual_Memory * m_vmem;
	KLAV_IO_Library * m_iolib;
	KLAV_DSO_Loader * m_native_ldr;

	KLAV_PE_Module * m_mod_head;
	KLAV_PE_Module * m_mod_tail;

	klav_string m_module_dir;

	void cleanup ();

	void add_mod_to_list (KLAV_PE_Module *mod);
	void remove_mod_from_list (KLAV_PE_Module *mod);
};

////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
 typedef int (__stdcall *PFN_DLL_MAIN)(void *, unsigned int, void *);
#else
 typedef int (*PFN_DLL_MAIN)(void *, unsigned int, void *) __attribute__ ((stdcall));
#endif

struct KLAV_PE_Module
{
	KLAV_PE_Module (
		KLAV_PE_Loader_Impl * host,
		char * path
	);

	~KLAV_PE_Module ();

	KLAV_ERR load ();
	KLAV_ERR unload ();

	void * symbol (const char *name);

	KLAV_Alloc * allocator () const
		{ return m_host->allocator (); }

	KLAV_PE_Loader_Impl * m_host;
	KLAV_PE_Module *       m_next;
	KLAV_PE_Module *       m_prev;
	char *                 m_path;   // absolute path (allocated)
	size_t                 m_refcnt;

	void    * m_dso_hmod;  // native loaded DLL
	uint8_t * m_img_data;  // image data
	size_t    m_img_size;  // image size

	void * m_init_context; // library context data (set by INIT call)

	KL_PE_EXPORT_DIRECTORY * m_exports;

	PFN_DLL_MAIN m_entry_point;

	void cleanup ();

	KLAV_ERR call_mod_init ();
	void call_mod_fini ();
};

////////////////////////////////////////////////////////////////

struct KLAV_PE_Load_Context
{
	KLAV_PE_Module  * m_mod;
	KLAV_IO_Object  * m_io;
	KL_PE_SECTION_HEADER * m_sectbl;
	size_t                 m_sectcnt;

	void *  m_reloc_data;
	size_t  m_reloc_size;

	KLAV_PE_Load_Context (KLAV_PE_Module *mod);
	~KLAV_PE_Load_Context ();

	void close_io ();
};

////////////////////////////////////////////////////////////////

KLAV_PE_Load_Context::KLAV_PE_Load_Context (KLAV_PE_Module *mod)
	: m_mod (mod), m_io (0), m_sectbl (0), m_sectcnt (0), m_reloc_data (0), m_reloc_size (0)
{
}

KLAV_PE_Load_Context::~KLAV_PE_Load_Context ()
{
	if (m_reloc_data != 0)
	{
		m_mod->allocator ()->free (m_reloc_data);
		m_reloc_data = 0;
	}

	m_reloc_size = 0;

	if (m_sectbl != 0)
	{
		m_mod->allocator ()->free (m_sectbl);
		m_sectbl = 0;
	}

	m_sectcnt = 0;

	close_io ();
}

void KLAV_PE_Load_Context::close_io ()
{
	if (m_io != 0)
	{
		m_io->destroy ();
		m_io = 0;
	}
}

////////////////////////////////////////////////////////////////

KLAV_PE_Loader_Impl::KLAV_PE_Loader_Impl (
			KLAV_Alloc *alloc,
			const char *module_dir,
			KLAV_Virtual_Memory *vmem,
			KLAV_IO_Library *iolib,
			KLAV_DSO_Loader *native_ldr
		) :
		m_allocator (alloc),
		m_vmem (vmem),
		m_iolib (iolib),
		m_native_ldr (native_ldr),
		m_mod_head (0),
		m_mod_tail (0)
{
	m_module_dir.assign (module_dir, alloc);
}

KLAV_PE_Loader_Impl::~KLAV_PE_Loader_Impl ()
{
	cleanup ();
}

void KLAV_PE_Loader_Impl::cleanup ()
{
	KLAV_PE_Module * mod = m_mod_head;

	while (mod != 0)
	{
		KLAV_PE_Module * next = mod->m_next;
		KLAV_DELETE (mod, allocator ());
		mod = next;
	}

	m_mod_head = 0;
	m_mod_tail = 0;
}

KLAV_ERR KLAV_CALL KLAV_PE_Loader_Impl::dl_open  (const char *path, void **phdl)
{
	*phdl = 0;
	KLAV_ERR err = KLAV_OK;

	if (path == 0 || path [0] == 0)
		return KLAV_EINVAL;

	// make full path
	char * full_path_buf = 0;
	
	size_t mod_dir_len = m_module_dir.length ();
	if (mod_dir_len != 0)
	{
		size_t path_len = strlen (path);
		const size_t path_bufsize = mod_dir_len + path_len + 10;

		full_path_buf = (char *) allocator ()->alloc (path_bufsize);
		if (full_path_buf == 0)
			return KLAV_ENOMEM;

		strcpy_s (full_path_buf, path_bufsize, m_module_dir.c_str ());
		
		char *p = full_path_buf + mod_dir_len;
		if (! KLAV_Fname_Is_Path_Separator (full_path_buf [mod_dir_len - 1]))
			*p++ = KLAV_Fname_Path_Separator ();

		strcpy_s (p, full_path_buf + path_bufsize - p, path);

		path = full_path_buf;
	}

	// normalize full path
	char *npath = 0;
	err = m_iolib->make_full_path (0, path, KLAV_PATH_F_NORMALIZE_CASE, allocator (), &npath);
	
	if (full_path_buf != 0)
		allocator ()->free (full_path_buf);
	
	if (KLAV_FAILED (err))
		return err;

	KLAV_PE_Module *mod = m_mod_head;

	while (mod != 0)
	{
		if (strcmp (mod->m_path, npath) == 0)
			break;

		mod = mod->m_next;
	}

	if (mod != 0)
	{
		allocator ()->free (npath);
		mod->m_refcnt ++;

		*phdl = mod;
		return KLAV_OK;
	}

	mod = KLAV_NEW (allocator ()) KLAV_PE_Module (this, npath);
	if (mod == 0)
		return KLAV_ENOMEM;

	err = mod->load ();
	if (KLAV_FAILED (err))
	{
		KLAV_DELETE (mod, allocator ());
		return err;
	}

	mod->m_refcnt = 1;

	add_mod_to_list (mod);

	*phdl = mod;

	return KLAV_OK;
}

void * KLAV_CALL KLAV_PE_Loader_Impl::dl_sym (void *hdl, const char *name)
{
	if (hdl == 0)
		return 0;

	KLAV_PE_Module * mod = m_mod_head;

	while (mod != 0)
	{
		if (mod == hdl)
			break;
		mod = mod->m_next;
	}

	if (mod == 0)
		return 0;

	return mod->symbol (name);
}

void     KLAV_CALL KLAV_PE_Loader_Impl::dl_close (void *hdl)
{
	KLAV_PE_Module * mod = m_mod_head;

	while (mod != 0)
	{
		if (mod == hdl)
			break;
		mod = mod->m_next;
	}

	if (mod == 0)
		return;

	if (--(mod->m_refcnt) == 0)
	{
		remove_mod_from_list (mod);
		mod->unload ();

		KLAV_DELETE (mod, allocator ());
	}
}

void KLAV_PE_Loader_Impl::add_mod_to_list (KLAV_PE_Module *mod)
{
	mod->m_next = 0;
	mod->m_prev = m_mod_tail;

	if (m_mod_tail != 0)
		m_mod_tail->m_next = mod;
	else
		m_mod_head = mod;

	m_mod_tail = mod;
}

void KLAV_PE_Loader_Impl::remove_mod_from_list (KLAV_PE_Module *mod)
{
	if (mod->m_next != 0)
		mod->m_next->m_prev = mod->m_prev;
	else
		m_mod_tail = mod->m_prev;

	if (mod->m_prev != 0)
		mod->m_prev->m_next = mod->m_next;
	else
		m_mod_head = mod->m_next;
}

////////////////////////////////////////////////////////////////

KLAV_PE_Module::KLAV_PE_Module (
		KLAV_PE_Loader_Impl * host,
		char * path
	) :
	m_host (host),
	m_next (0),
	m_prev (0),
	m_path (path),
	m_refcnt (0),
	m_dso_hmod (0),
	m_img_data (0),
	m_img_size (0),
	m_init_context (0),
	m_entry_point (0),
	m_exports (0)
{
}

KLAV_PE_Module::~KLAV_PE_Module ()
{
	cleanup ();
}

KLAV_ERR KLAV_PE_Module::load ()
{
	KLAV_ERR err = KLAV_OK;
	KLAV_PE_Load_Context load_ctx (this);

	if (m_path == 0 || m_path [0] == 0 || m_host->iolib () == 0)
		return KLAV_EINVAL;

	err = m_host->iolib ()->create_file (0, m_path, KLAV_IO_READONLY, KLAV_FILE_OPEN_EXISTING, 0, & load_ctx.m_io);
	if (KLAV_FAILED (err))
		return err;

	bool use_system_loader = true;
	bool pe_fmt = false;
	size_t sect_alignment = 0x1000;
	size_t file_alignment = 0x200;

	KL_PE_DOS_HEADER dos_hdr;
	KL_PE_NT_HEADERS nt_hdr;

	uint32_t nt_hdr_off = 0;

	// read MZ header
	err = load_ctx.m_io->seek_read (0, & dos_hdr, sizeof (dos_hdr), 0);
	if (KLAV_FAILED (err))
		return err;

	if (dos_hdr.e_magic == KL_PE_DOS_SIGNATURE)
	{
		// read NT header
		nt_hdr_off = dos_hdr.e_lfanew;

		err = load_ctx.m_io->seek_read (nt_hdr_off, & nt_hdr, sizeof (nt_hdr), 0);
		if (KLAV_FAILED (err))
			return err;

		// check file parameters
		if (nt_hdr.Signature != KL_PE_NT_SIGNATURE || nt_hdr.OptionalHeader.Magic != KL_PE_NT_OPTIONAL_HDR_MAGIC)
			return KLAV_EUNKNOWN;

		if (! (nt_hdr.FileHeader.Machine == KL_PE_FILE_MACHINE_I386
		 || nt_hdr.FileHeader.Machine == KL_PE_FILE_MACHINE_I486
		 || nt_hdr.FileHeader.Machine == KL_PE_FILE_MACHINE_PENTIUM))
			return KLAV_EUNKNOWN;

		if ((nt_hdr.FileHeader.Characteristics & KL_PE_FILE_EXECUTABLE_IMAGE) == 0
		 || (nt_hdr.FileHeader.Characteristics & KL_PE_FILE_32BIT_MACHINE) == 0
		 || (nt_hdr.FileHeader.Characteristics & KL_PE_FILE_DLL) == 0)
			return KLAV_EUNKNOWN;

		// check section alignment
		sect_alignment = nt_hdr.OptionalHeader.SectionAlignment;
		if ((sect_alignment & (sect_alignment - 1)) != 0)
			return KLAV_EUNKNOWN;

		// check file alignment
		file_alignment = nt_hdr.OptionalHeader.FileAlignment;
		if ((file_alignment & (file_alignment - 1)) != 0)
			return KLAV_EUNKNOWN;

	// check for absense of import directory
#ifndef DISABLE_NEW_LOADER
		uint32_t import_size = nt_hdr.OptionalHeader.DataDirectory [KL_PE_DIRECTORY_ENTRY_IMPORT].Size;
		if (import_size == 0)
			use_system_loader = false;
#endif
		pe_fmt = true;
	}

	if (use_system_loader)
	{
		// use platform-specific loader, if present
		load_ctx.close_io ();

		KLAV_DSO_Loader * native_ldr = m_host->native_ldr ();
		if (native_ldr == 0)
			return KLAV_EUNKNOWN;

		err = native_ldr->dl_open (m_path, & m_dso_hmod);
		if (KLAV_FAILED (err))
			return err;

		if (pe_fmt)
		{
			m_entry_point = (PFN_DLL_MAIN) ((uint8_t *) m_dso_hmod + nt_hdr.OptionalHeader.AddressOfEntryPoint);

			err = call_mod_init ();
			if (KLAV_FAILED (err))
				return err;
		}

		return KLAV_OK;
	}

	load_ctx.m_sectcnt = nt_hdr.FileHeader.NumberOfSections;
	if (load_ctx.m_sectcnt == 0)
		return KLAV_EUNKNOWN;

	load_ctx.m_sectbl = (KL_PE_SECTION_HEADER *) allocator ()->alloc (load_ctx.m_sectcnt * sizeof (KL_PE_SECTION_HEADER));
	if (load_ctx.m_sectbl == 0)
		return KLAV_ENOMEM;

	err = load_ctx.m_io->seek_read (
			nt_hdr_off + sizeof (KL_PE_NT_HEADERS),
			load_ctx.m_sectbl,
			(uint32_t) load_ctx.m_sectcnt * sizeof (KL_PE_SECTION_HEADER),
			0
		);
	if (KLAV_FAILED (err))
		return err;

	uint32_t pref_base_addr = nt_hdr.OptionalHeader.ImageBase;

	klav_filepos_t file_size = 0;
	err = load_ctx.m_io->get_size (& file_size);
	if (KLAV_FAILED (err))
		return err;

	// calculate loadable image size

	uint32_t secno = 0;
	for (secno = 0; secno < load_ctx.m_sectcnt; ++secno)
	{
		KL_PE_SECTION_HEADER& sect = load_ctx.m_sectbl [secno];
		size_t sect_addr = SECT_ALIGN (sect.VirtualAddress, sect_alignment);

		// size = max (VirtualSize, SizeOfRawData)
		size_t sect_size = sect.SizeOfRawData;
		if (sect_size < sect.VirtualSize)
			sect_size = sect.VirtualSize;

		// align to section alignment
		sect_size = SECT_ALIGN (sect_size, sect_alignment);

		if ((sect.Characteristics & KL_PE_SCN_MEM_DISCARDABLE) != 0)
			continue;

		size_t top_addr = sect_addr + sect_size;

		if (top_addr > m_img_size)
			m_img_size = top_addr;
	}

	if (m_img_size == 0)
		return KLAV_EUNKNOWN;

	// allocate memory and load file
	err = m_host->vmem ()->vm_alloc ((void **)& m_img_data,
			(uint32_t) m_img_size,
			KLAV_MEM_PROT_READ|KLAV_MEM_PROT_WRITE, 0);
	if (KLAV_FAILED (err))
		return err;

	memcpy (m_img_data, & dos_hdr, sizeof (dos_hdr));
	memcpy (m_img_data + nt_hdr_off, & nt_hdr, sizeof (nt_hdr));
	memcpy (m_img_data + nt_hdr_off + sizeof (nt_hdr),
		load_ctx.m_sectbl, load_ctx.m_sectcnt * sizeof (KL_PE_SECTION_HEADER));

	// read sections into memory and set up section data pointers
	for (secno = 0; secno < load_ctx.m_sectcnt; ++secno)
	{
		KL_PE_SECTION_HEADER& sect = load_ctx.m_sectbl [secno];
		if ((sect.Characteristics & KL_PE_SCN_MEM_DISCARDABLE) != 0)
			continue;

		size_t sect_addr = SECT_ALIGN (sect.VirtualAddress, sect_alignment);
		size_t file_pos = FILE_ALIGN (sect.PointerToRawData, file_alignment);

		size_t data_size = sect.VirtualSize;
		if (data_size == 0)
			data_size = sect.SizeOfRawData;

		if (data_size != 0)
		{
			err = load_ctx.m_io->seek_read (file_pos, m_img_data + sect_addr, (uint32_t) data_size, 0);
			if (KLAV_FAILED (err))
				return err;
		}
	}

	uint32_t base_addr = (uint32_t) (size_t) m_img_data;
	if (base_addr != pref_base_addr)
	{
		// need to relocate...
		// get ptr to .reloc section
		size_t reloc_rva  = nt_hdr.OptionalHeader.DataDirectory [KL_PE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
		reloc_rva = SECT_ALIGN (reloc_rva, sect_alignment); // TODO: is this needed?
		
		uint32_t reloc_size = nt_hdr.OptionalHeader.DataDirectory [KL_PE_DIRECTORY_ENTRY_BASERELOC].Size;

		if (reloc_size != 0)
		{
			size_t reloc_addr = 0;

			for (secno = 0; secno < load_ctx.m_sectcnt; ++secno)
			{
				KL_PE_SECTION_HEADER& sect = load_ctx.m_sectbl [secno];
				size_t sect_rva = SECT_ALIGN (sect.VirtualAddress, sect_alignment);
				if (sect_rva == reloc_rva)
				{
					reloc_addr = FILE_ALIGN (sect.PointerToRawData, file_alignment);
					break;
				}
			}

			if (reloc_addr == 0)
				return KLAV_EUNKNOWN;

			uint32_t fixup = base_addr - pref_base_addr;

			load_ctx.m_reloc_data = allocator ()->alloc (reloc_size);
			if (load_ctx.m_reloc_data == 0)
				return KLAV_ENOMEM;

			load_ctx.m_reloc_size = reloc_size;

			err = load_ctx.m_io->seek_read (reloc_addr, load_ctx.m_reloc_data, reloc_size, 0);
			if (KLAV_FAILED (err))
				return err;

			// perform relocations
			uint8_t * relptr = (uint8_t *) load_ctx.m_reloc_data;
			uint8_t * relend = relptr + reloc_size;

			while (relptr < relend)
			{
				KL_PE_BASE_RELOCATION * chunk = (KL_PE_BASE_RELOCATION *) relptr;
				uint32_t va = chunk->VirtualAddress;
				uint32_t sz = chunk->SizeOfBlock;

				const uint16_t *rp = (const uint16_t *) (chunk + 1);
				relptr += sz;

				while (rp < (const uint16_t *)relptr)
				{
					uint16_t rel = *rp++;
					uint32_t off = rel & 0x0FFF;
					uint32_t rtype = (rel & 0xF000) >> 12;

					uint32_t * target = (uint32_t *) (m_img_data + va + off);

					switch (rtype)
					{
					case 0:  // REL_BASED_ABSOLUTE
						break;
					case 3:  // REL_BASED_HIGHLOW
						*target += fixup;
						break;
					default:
						return KLAV_EUNKNOWN;
					}
				}
			}
		}
	}

	// set memory protection
	for (secno = 0; secno < load_ctx.m_sectcnt; ++secno)
	{
		KL_PE_SECTION_HEADER& sect = load_ctx.m_sectbl [secno];
		if ((sect.Characteristics & KL_PE_SCN_MEM_DISCARDABLE) != 0)
			continue;

		size_t sect_addr = SECT_ALIGN (sect.VirtualAddress, sect_alignment);
		size_t sect_size = sect.VirtualSize;
		if (sect_size < sect.SizeOfRawData)
			sect_size = sect.SizeOfRawData;
		sect_size = SECT_ALIGN (sect_size, sect_alignment);

		if ((sect.Characteristics & KL_PE_SCN_MEM_EXECUTE) != 0)
		{
			// code
			err = m_host->vmem ()->vm_protect (
					m_img_data + sect_addr,
					(uint32_t) sect_size,
					KLAV_MEM_PROT_READ|KLAV_MEM_PROT_EXEC
				);
			if (KLAV_FAILED (err))
				return err;
		}
		else if ((sect.Characteristics & KL_PE_SCN_MEM_READ) != 0
			  && (sect.Characteristics & KL_PE_SCN_MEM_WRITE) == 0)
		{
			// read-only data
			err = m_host->vmem ()->vm_protect (
					m_img_data + sect_addr,
					(uint32_t) sect_size,
					KLAV_MEM_PROT_READ
				);
			if (KLAV_FAILED (err))
				return err;
		}
	}

	// get export directory
	size_t exports_rva = nt_hdr.OptionalHeader.DataDirectory [KL_PE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

	if (exports_rva != 0)
		m_exports = (KL_PE_EXPORT_DIRECTORY *) (m_img_data + exports_rva);
	else
		m_exports = 0;

	// get entry point
	if (nt_hdr.OptionalHeader.AddressOfEntryPoint != 0)
	{
		m_entry_point = (PFN_DLL_MAIN) (m_img_data + nt_hdr.OptionalHeader.AddressOfEntryPoint);

		if (! m_entry_point (m_img_data, 1, 0)) // DLL_PROCESS_ATTACH
		{
			return KLAV_EUNKNOWN;
		}

		err = call_mod_init ();
		if (KLAV_FAILED (err))
			return err;
	}

	return KLAV_OK;
}

KLAV_ERR KLAV_PE_Module::call_mod_init ()
{
	if (m_entry_point == 0)
		return KLAV_EUNKNOWN;

	// call entry point (KLAV_MOD_LOAD)
	KLAV_Module_Init_Struct init_struct;
	memset (& init_struct, 0, sizeof (init_struct));
	init_struct.mod_init_sizeof = sizeof (KLAV_Module_Init_Struct);
	init_struct.mod_init_error = KLAV_OK;
	init_struct.mod_init_alloc = allocator ();
	init_struct.mod_init_context = 0;

	void * hmod = (m_dso_hmod == 0) ? m_img_data : m_dso_hmod;
	if (hmod == 0)
		return KLAV_EUNKNOWN;

	m_entry_point (m_dso_hmod, KLAV_MODULE_INIT, & init_struct);
	if (init_struct.mod_init_error != 0)
	{
		if (KLAV_FAILED (init_struct.mod_init_error))
			return init_struct.mod_init_error;
		return KLAV_EUNKNOWN;
	}

	m_init_context = init_struct.mod_init_context;

	return KLAV_OK;
}

void KLAV_PE_Module::call_mod_fini ()
{
	if (m_entry_point == 0)
		return;

	// call entry point (KLAV_MOD_LOAD)
	KLAV_Module_Init_Struct init_struct;
	memset (& init_struct, 0, sizeof (init_struct));
	init_struct.mod_init_sizeof = sizeof (KLAV_Module_Init_Struct);
	init_struct.mod_init_error = KLAV_OK;
	init_struct.mod_init_alloc = allocator ();
	init_struct.mod_init_context = m_init_context;

	void * hmod = (m_dso_hmod == 0) ? m_img_data : m_dso_hmod;
	if (hmod == 0)
		return;

	m_entry_point (hmod, KLAV_MODULE_FINI, & init_struct);
}

void KLAV_PE_Module::cleanup ()
{
	if (m_img_data != 0)
	{
		m_host->vmem ()->vm_free (m_img_data, (uint32_t) m_img_size);
		m_img_data = 0;
	}

	m_img_size = 0;
	m_init_context = 0;
	m_entry_point = 0;

	if (m_dso_hmod != 0)
	{
		m_host->native_ldr ()->dl_close (m_dso_hmod);
		m_dso_hmod = 0;
	}

	if (m_path != 0)
	{
		allocator ()->free (m_path);
		m_path = 0;
	}
}

KLAV_ERR KLAV_PE_Module::unload ()
{
	call_mod_fini ();

	cleanup ();

	return KLAV_OK;
}

void * KLAV_PE_Module::symbol (const char *name)
{
	if (name == 0 || *name == 0)
		return 0;

	if (m_dso_hmod != 0)
	{
		return m_host->native_ldr ()->dl_sym (m_dso_hmod, name);
	}

	if (m_exports == 0)
		return 0;

	if (m_exports->AddressOfFunctions == 0
	 || m_exports->AddressOfNames == 0
	 || m_exports->AddressOfNameOrdinals == 0)
		return 0;

	uint32_t name_cnt = m_exports->NumberOfNames;
	uint32_t * name_addrs = (uint32_t *) (m_img_data + m_exports->AddressOfNames);

	// TODO: implement binary search
	uint32_t name_idx = 0;
	for (name_idx = 0; name_idx < name_cnt; ++name_idx)
	{
		const char * exp_name = (const char *) (m_img_data + name_addrs [name_idx]);
		if (strcmp (name, exp_name) == 0)
			break;
	}

	if (name_idx == name_cnt)
		return 0; // name not found

	uint16_t * name_ordinals = (uint16_t *) (m_img_data + m_exports->AddressOfNameOrdinals);
	uint32_t name_ord = name_ordinals [name_idx];

	if (name_ord >= m_exports->NumberOfFunctions)
		return 0; // unexpected...

	uint32_t * func_rvas = (uint32_t *) (m_img_data + m_exports->AddressOfFunctions);
	uint32_t func_rva = func_rvas [name_ord];
	if (func_rva == 0)
		return 0;

	void *pfunc = m_img_data + func_rva;

	return pfunc;
}

////////////////////////////////////////////////////////////////

KLAV_ERR KLAV_CALL KLAV_PE_Loader_Create (
			KLAV_Alloc      *alloc,
			const char * module_dir,
			KLAV_IO_Library *iolib,
			KLAV_Virtual_Memory *vmem,
			KLAV_DSO_Loader *native_ldr,
			KLAV_DSO_Loader **pmodldr
		)
{
	if (pmodldr == 0)
		return KLAV_EINVAL;

	*pmodldr = 0;

	if (alloc == 0 || iolib == 0 || vmem == 0)
		return KLAV_EINVAL;

	KLAV_PE_Loader_Impl *ldr = KLAV_NEW (alloc) KLAV_PE_Loader_Impl (alloc, module_dir, vmem, iolib, native_ldr);
	if (ldr == 0)
		return KLAV_ENOMEM;

	*pmodldr = (KLAV_DSO_Loader *) ldr;

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_PE_Loader_Destroy (
			KLAV_DSO_Loader * modldr
		)
{
	if (modldr == 0)
		return KLAV_EINVAL;

	KLAV_PE_Loader_Impl * ldr = (KLAV_PE_Loader_Impl *) modldr;

	KLAV_DELETE (ldr, ldr->allocator ());

	return KLAV_OK;
}

#else // KL_ARCH_X86

// just redirect to the native module loader

KLAV_ERR KLAV_CALL KLAV_PE_Loader_Create (
			KLAV_Alloc      *alloc,
			const char * module_dir,
			KLAV_IO_Library *iolib,
			KLAV_Virtual_Memory *vmem,
			KLAV_DSO_Loader *native_ldr,
			KLAV_DSO_Loader **pmodldr
		)
{
	if (pmodldr == 0)
		return KLAV_EINVAL;

	*pmodldr = 0;

	if (native_ldr == 0)
		return KLAV_EINVAL;

	*pmodldr = native_ldr;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_PE_Loader_Destroy (
			KLAV_DSO_Loader * modldr
		)
{
	return KLAV_OK;
}

#endif // KL_ARCH_X86

