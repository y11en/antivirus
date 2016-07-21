// klav_utils.h
//
//

#ifndef klav_utils_h_INCLUDED
#define klav_utils_h_INCLUDED

#include <klava/klavdb.h>
#include <klava/klavsys.h>
#include <klava/klav_string.h>
#include <klava/klavstl/buffer.h>
#include <klava/klavstl/vector.h>

////////////////////////////////////////////////////////////////
// Allocator implementation on top of malloc / free

struct KLAV_Malloc_Alloc : public KLAV_IFACE_IMPL(KLAV_Alloc)
{
public:
	virtual ~KLAV_Malloc_Alloc ()
		{}

	virtual uint8_t * KLAV_CALL alloc (size_t size)
		{ return (uint8_t *)::calloc (1, size); }

	virtual uint8_t * KLAV_CALL realloc (void *ptr, size_t newsize)
		{ return (uint8_t *)::realloc (ptr, newsize); }

	virtual void KLAV_CALL free (void *ptr)
		{ if (ptr != 0) ::free (ptr); }
};

extern KLAV_Malloc_Alloc g_klav_malloc;

#define KLAV_GLOBAL_MALLOC_IMPL() KLAV_Malloc_Alloc g_klav_malloc;
#define KLAV_GLOBAL_MALLOC() ((KLAV_Alloc *) & g_klav_malloc)

////////////////////////////////////////////////////////////////
// Property map implementation

#ifdef __cplusplus

typedef KLDynObjectHolder <KLAV_Property_Bag> KLAV_Property_Bag_Holder;

#endif // __cplusplus

KLAV_EXTERN_C
KLAV_Property_Bag * KLAV_CALL KLAV_Property_Bag_Create (
		KLAV_Alloc *allocator
	);

KLAV_EXTERN_C
KLAV_Property_Bag * KLAV_CALL KLAV_Property_Bag_Create_Ex (
		KLAV_Alloc *allocator,
		KLAV_Properties *chain
	);

// copy inheritable properties from src to dst
KLAV_ERR KLAV_CALL KLAV_Inherit_Properties (
		KLAV_Properties * src,
		KLAV_Properties * dst
	);

////////////////////////////////////////////////////////////////
// Filename utils

inline klav_bool_t fns_is_path_sep (unsigned char c)
{
#ifdef KL_PLATFORM_WINDOWS
	if (c == '\\') return true;
#endif // KL_PLATFORM_WINDOWS
	if (c == '/') return true;
	return false;
}

inline char fns_path_sep ()
{
#ifdef KL_PLATFORM_WINDOWS
	return '\\';
#else  // KL_PLATFORM_WINDOWS
	return '/';
#endif // KL_PLATFORM_WINDOWS
}

// returns offset of the file name
size_t fns_name_off (const char *path);

// returns offset of the last extension, strlen(fname) if extension does not present
size_t fns_ext_off (const char *fname);

// returns size of absolute path prefix (C:\, \ or \\SERVER\SHARE\)
size_t fns_is_root (const char *path);

#define KLAV_Fname_Is_Path_Separator(C) fns_is_path_sep(C)
#define KLAV_Fname_Path_Separator()     fns_path_sep()
#define KLAV_Fname_File_Offset(P)       fns_name_off(P)
#define KLAV_Fname_Ext_Offset(P)        fns_ext_off(P)
#define KLAV_Fname_Is_Absolute_Path(P)  fns_is_root(P)

////////////////////////////////////////////////////////////////

typedef klavstl::trivial_vector <void *, klav_allocator> klav_ptr_vector;

////////////////////////////////////////////////////////////////
// 'Pool allocator' - optimized allocator for fixed-size items

class KLAV_Pool_Item_Allocator
{
public:
	KLAV_Pool_Item_Allocator (
			KLAV_Alloc * allocator,
			uint32_t item_size,
			uint32_t block_size
		)
		: m_allocator (allocator),
		  m_last_block (0),
		  m_free_list (0)
	{
		// items are aligned to 16 bytes
		m_item_size = KLAV_ALIGN (item_size, 16);
		// first 16 bytes in each block are reserved for block header
		m_items_in_blk = (block_size - 16) / m_item_size;
	}

	~KLAV_Pool_Item_Allocator ()
	{
		cleanup ();
	}

	void * alloc ();
	void free (void * data);

	void cleanup ();

	KLAV_Alloc * allocator () const
		{ return m_allocator; }

private:
	KLAV_Alloc * m_allocator;
	uint32_t     m_item_size;
	uint32_t     m_items_in_blk;

	// element/block list node
	struct ListNode
	{
		// for freelist - ptr to next item, for blocks - ptr to next block
		ListNode * m_ptr;
		// number of items used (only for block headers)
		uint32_t   m_used;
	};

	ListNode *  m_last_block;
	ListNode *  m_free_list;
	
	KLAV_Pool_Item_Allocator (const KLAV_Pool_Item_Allocator&);
	KLAV_Pool_Item_Allocator& operator= (const KLAV_Pool_Item_Allocator&);
};

////////////////////////////////////////////////////////////////

template <class T>
class KLAV_Pool_Allocator : public KLAV_Pool_Item_Allocator
{
public:
	KLAV_Pool_Allocator (KLAV_Alloc *alloc, uint32_t block_size)
		: KLAV_Pool_Item_Allocator (alloc, sizeof (T), block_size) {}

	T * alloc ()
	{
		void *p = KLAV_Pool_Item_Allocator::alloc ();
		if (p == 0)
			return 0;

		return new (p) T ();
	}

	void free (T *p)
	{
		if (p != 0)
		{
			p->~T ();
			KLAV_Pool_Item_Allocator::free (p);
		}
	}

private:
	KLAV_Pool_Allocator<T> (const KLAV_Pool_Allocator <T>&);
	KLAV_Pool_Allocator<T>& operator= (const KLAV_Pool_Allocator <T>&);
};

////////////////////////////////////////////////////////////////
// DSO-based procedure resolver

KLAV_EXTERN_C
hKLAV_Proc_Resolver KLAV_CALL KLAV_Create_DSO_Proc_Resolver (
			hKLAV_Alloc      alloc,
			hKLAV_DSO_Loader dso_loader,
			hKLAV_Proc_Resolver chain
		);
			
KLAV_EXTERN_C
KLAV_ERR KLAV_CALL KLAV_Destroy_DSO_Proc_Resolver (
			hKLAV_Proc_Resolver obj
		);

////////////////////////////////////////////////////////////////
// Single-file ('KFB') DBLoader implementation

class KLAV_KFB_Loader : public KLAV_IFACE_IMPL(KLAV_DB_Loader)
{
public:
	KLAV_KFB_Loader (KLAV_Alloc *alloc = 0, KLAV_Virtual_Memory *vm = 0);
	virtual ~KLAV_KFB_Loader ();

	virtual void destroy ();

	void init (KLAV_Alloc *alloc, KLAV_Virtual_Memory *vm);

	enum  // open mode
	{
		ALLOW_MAPPING  = 0x01, // allow file mapping
		DESTROY_IO     = 0x02, // destroy IO on close
	};

	KLAV_ERR open (const void * data, uint32_t size, uint32_t mode = ALLOW_MAPPING);
	KLAV_ERR open_io (KLAV_IO_Object *io, uint32_t mode);
	KLAV_ERR close ();

	// detach source KFB data
	KLAV_ERR detach_source ();

	// KlavDBLoader interface methods
	KLAV_ERR KLAV_CALL load_fragment (uint32_t sect_type, uint32_t frag_id, uint32_t flags, KLAV_DB_Fragment* frag);
	KLAV_ERR KLAV_CALL release_fragment (KLAV_DB_Fragment *frag);
	KLAV_ERR KLAV_CALL protect_fragment (KLAV_DB_Fragment *frag, uint32_t flags);
	uint32_t KLAV_CALL get_engine_parm (uint32_t id);
	KLAV_ERR KLAV_CALL get_ldr_iface (uint32_t id, void **pifc);

	uint32_t KLAV_CALL get_db_version ();
	uint32_t KLAV_CALL get_db_release ();

private:
	KLAV_Alloc *          m_alloc;
	KLAV_Virtual_Memory * m_vm;
	KLAV_IO_Object *      m_io;
	uint32_t              m_kfb_release;
	uint32_t              m_kfb_version;
	
	struct Section_Desc;
	Section_Desc * m_sections;

	struct KFB_KERNEL_PARM * m_kparms;

	uint32_t m_section_count;
	uint32_t m_kparm_count;

	uint32_t m_loadable_size;
	uint32_t m_resident_size;

	uint32_t m_open_mode;

	bool m_kfb2_format;  // KFB2 format
	bool m_opened;       // flag: database is opened

	void delete_sections ();
	void release_section (uint32_t idx);

	KLAV_ERR do_open ();

	KLAV_ERR open_kfb1 (struct KFB1_FILE_HEADER *hdr, klav_filepos_t size);
	KLAV_ERR open_kfb2 (struct KFB2_FILE_HEADER *hdr, klav_filepos_t size);

	KLAV_ERR load_section_data (
				Section_Desc& sect,
				uint32_t new_prot
			);

	void cleanup ();
	int  find_section (uint32_t section_type, uint32_t fragment_id) const;
	KLAV_ERR load_kparms ();
	KLAV_ERR handle_error (KLAV_ERR err);

	bool allow_map () const
		{ return (m_open_mode & ALLOW_MAPPING) != 0; }

	KLAV_KFB_Loader (const KLAV_KFB_Loader&);
	KLAV_KFB_Loader& operator= (const KLAV_KFB_Loader&);
};

////////////////////////////////////////////////////////////////
// Native KFB file loader

class KLAV_KFB_File_Loader : public KLAV_KFB_Loader
{
public:
		KLAV_KFB_File_Loader ();
		~KLAV_KFB_File_Loader ();

	KLAV_ERR open (KLAV_Alloc *alloc, KLAV_Virtual_Memory *vm, const char *filepath);
	KLAV_ERR close ();

private:
	struct KFB_File_Loader_Impl * m_pimpl;

	KLAV_KFB_File_Loader (const KLAV_KFB_File_Loader&);
	KLAV_KFB_File_Loader& operator= (const KLAV_KFB_File_Loader&);
};

////////////////////////////////////////////////////////////////
// Multi-file DBLoader implementation

// database fragment description
struct KLAV_DB_Fragment_Desc
{
	const char    * filename;       // file name (no prefix or extension added)
	uint32_t        section_type;   // DB section type
	uint32_t        fragment_id;    // DB fragment ID
	uint32_t        flags;          // DBLOADER flags
	const uint8_t * signature;      // signature to check
	uint32_t        signature_len;  // signature length
};

// database fragment loader
class KLAV_NO_VTABLE KLAV_DB_Fragment_Loader
{
public:
	virtual KLAV_ERR KLAV_CALL load_fragment (
			const KLAV_DB_Fragment_Desc& fragmentDesc,   // fragment description
			KLAV_DB_Fragment *pdbFragment               // [out,retval]
		) = 0;

	virtual KLAV_ERR KLAV_CALL release_fragment (
			const KLAV_DB_Fragment *pdbFragment
		) = 0;
};

class KLAV_MultiFile_DB_Loader : public KLAV_IFACE_IMPL(KLAV_DB_Loader)
{
public:
			KLAV_MultiFile_DB_Loader ();
	virtual ~KLAV_MultiFile_DB_Loader ();

	KLAV_ERR open (KLAV_Alloc *alloc, KLAV_DB_Fragment_Loader *loader, KLAV_Virtual_Memory *vm, bool checkSignature);
	KLAV_ERR close ();

	// KlavDBLoader interface methods
	KLAV_ERR KLAV_CALL load_fragment (uint32_t sectionType, uint32_t fragmentID, uint32_t flags, KLAV_DB_Fragment* dbFragment);
	KLAV_ERR KLAV_CALL release_fragment (KLAV_DB_Fragment *dbFragment);
	KLAV_ERR KLAV_CALL protect_fragment (KLAV_DB_Fragment *dbFragment, uint32_t new_prot);
	uint32_t KLAV_CALL get_engine_parm (uint32_t id);

private:
	class MultiFile_DB_Loader_Impl * m_pimpl;
};

////////////////////////////////////////////////////////////////
// Native multi-file DBFragment loader

class KLAV_File_Fragment_Loader : public KLAV_IFACE_IMPL(KLAV_DB_Fragment_Loader)
{
public:
		     KLAV_File_Fragment_Loader ();
	virtual	~KLAV_File_Fragment_Loader ();

	KLAV_ERR init (KLAV_Alloc *alloc, const char *folder, const char *prefix, const char *suffix);
	void deinit ();

	virtual KLAV_ERR KLAV_CALL load_fragment (
			const KLAV_DB_Fragment_Desc& fragmentDesc,   // fragment description
			KLAV_DB_Fragment *pdbFragment               // [out,retval]
		);

	virtual KLAV_ERR KLAV_CALL release_fragment (
			const KLAV_DB_Fragment *pdbFragment
		);

private:
	KLAV_Alloc * m_allocator;
	const char * m_dbFolder;
	const char * m_dbPrefix;
	const char * m_dbSuffix;
	char *       m_namebuf;
	size_t       m_namebuflen;

	bool allocbuf (size_t len);
	void freebuf ();
};

////////////////////////////////////////////////////////////////
#endif // klav_utils_h_INCLUDED


