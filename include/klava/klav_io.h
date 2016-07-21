// klav_io.h
//
//

#ifndef klav_io_h_INCLUDED
#define klav_io_h_INCLUDED

#include <klava/klaveng.h>
#include <klava/kl_dyn_obj.h>

////////////////////////////////////////////////////////////////
// IO interface version

#define KLAV_IO_VERSION_CURRENT KLAV_ENGINE_VERSION_CURRENT

////////////////////////////////////////////////////////////////
// Base IO interface

enum // query_map() and klav_iomap_t flags
{
	KLAV_IOMAP_F_ENTIRE    = 0x01,   // area can be mapped entirely
	KLAV_IOMAP_F_PARTIAL   = 0x02,   // area is sparse (via partial mapping)
	KLAV_IOMAP_F_MAPPED    = 0x10,   // area already presents in memory
	KLAV_IOMAP_F_WRITABLE  = 0x20,   // area data can be write-mapped
};

////////////////////////////////////////////////////////////////
// IOCTL codes

enum
{
	KLAV_IOCTL_GET_CURRENT_POS = 0x00020002,
	KLAV_IOCTL_SET_CURRENT_POS = 0x08020002,

	KLAV_IOCTL_GET_IO_ORIGIN   = 0x00020003,
	KLAV_IOCTL_GET_IO_HANDLE   = 0x00020004,
};

// flags for map_data()
enum
{
	KLAV_IOMAP_PARTIAL   = 0x0001,  // map contigious portion (may be less than requested size)
	KLAV_IOMAP_WRITABLE  = 0x0010,  // create writable mapping
};

////////////////////////////////////////////////////////////////
// IO mapping

struct KLAV_IO_Object;
typedef struct KLAV_IO_Object * hKLAV_IO_Object;

typedef struct KLAV_IO_Mapping
{
	void *          iomap_handle;
	const uint8_t * iomap_data;
	uint32_t        iomap_size;
	uint32_t        iomap_flags;

#ifdef __cplusplus
	KLAV_IO_Mapping ()
	{
		clear ();
	}
		
	void clear ()
	{
		iomap_handle = 0;
		iomap_data = 0;
		iomap_size = 0;
		iomap_flags = 0;
	}
#endif
} klav_iomap_t;

////////////////////////////////////////////////////////////////

#ifdef __cplusplus

struct KLAV_IO_Object : public KLDynObject
{
	virtual uint32_t  KLAV_CALL get_io_version (
		) = 0;

	virtual void * KLAV_CALL get_io_iface (
			int ifc_id
		) = 0;

	virtual KLAV_ERR KLAV_CALL seek_read (
			klav_filepos_t pos,
			void *buf,
			uint32_t size,
			uint32_t *pnrd
		) = 0;

	virtual KLAV_ERR KLAV_CALL seek_write (
			klav_filepos_t pos,
			const void *buf,
			uint32_t size,
			uint32_t *pnwr
		) = 0;

	virtual KLAV_ERR KLAV_CALL get_size (
			klav_filepos_t *psize
		) = 0;

	virtual KLAV_ERR KLAV_CALL set_size (
			klav_filepos_t size
		) = 0;

	virtual KLAV_ERR KLAV_CALL flush (
		) = 0;

	virtual KLAV_ERR KLAV_CALL ioctl (
			uint32_t code,
			void *buf,
			size_t bufsize
		) = 0;

	virtual KLAV_ERR KLAV_CALL query_map (
			klav_filepos_t pos,
			uint32_t       size,
			uint32_t *     pflags
		) = 0;

	virtual KLAV_ERR KLAV_CALL map_data (
			klav_filepos_t pos,
			uint32_t       size,
			uint32_t       flags,
			klav_iomap_t * iomap
		) = 0;

	virtual KLAV_ERR KLAV_CALL unmap_data (
			klav_iomap_t * iomap
		) = 0;

	virtual KLAV_ERR  KLAV_CALL close (
		) = 0;

	virtual hKLAV_Properties KLAV_CALL get_properties (
		) = 0;

    KLAV_DUMMY_METHODS_DECL(KLAV_IO_Object)
};

typedef KLDynObjectHolder <KLAV_IO_Object> KLAV_IO_Object_Holder;

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL(KLAV_IO_Object)
{
	void     (KLAV_CALL *destroy) (
				struct KLAV_IO_Object *
			);

	uint32_t (KLAV_CALL *get_io_version) (
				struct KLAV_IO_Object *
			);

	void *   (KLAV_CALL *get_io_iface) (
				struct KLAV_IO_Object *,
				uint32_t
			);

	KLAV_ERR (KLAV_CALL *seek_read) (
				struct KLAV_IO_Object *,
				klav_filepos_t,
				void *,
				uint32_t,
				uint32_t *
			);

	KLAV_ERR (KLAV_CALL *seek_write) (
				struct KLAV_IO_Object *,
				klav_filepos_t,
				const void *,
				uint32_t,
				uint32_t *
			);

	KLAV_ERR (KLAV_CALL *get_size) (
				struct KLAV_IO_Object *,
				klav_filepos_t *
			);

	KLAV_ERR (KLAV_CALL *set_size) (
				struct KLAV_IO_Object *,
				klav_filepos_t
			);

	KLAV_ERR (KLAV_CALL *flush) (
				struct KLAV_IO_Object *
			);

	KLAV_ERR (KLAV_CALL *ioctl) (
				struct KLAV_IO_Object *,
				uint32_t,
				void *,
				size_t
			);

	KLAV_ERR (KLAV_CALL *map_query) (
				struct KLAV_IO_Object *,
				klav_filepos_t,
				uint32_t,
				uint32_t *
			);

	KLAV_ERR (KLAV_CALL *map_data) (
				struct KLAV_IO_Object *,
				klav_filepos_t,
				uint32_t,
				uint32_t,
				klav_iomap_t *
			);

	KLAV_ERR (KLAV_CALL *map_release) (
				struct KLAV_IO_Object *,
				klav_iomap_t *
			);

	KLAV_ERR  (KLAV_CALL *close) (
				struct KLAV_IO_Object *
			);

	hKLAV_Properties (KLAV_CALL *get_properties) (
				struct KLAV_IO_Object *
			);
};

KLAV_DECLARE_C_INTERFACE(KLAV_IO_Object);

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// IO library interface

enum  // file creation access types
{
	KLAV_IO_READONLY  = 0x00,
	KLAV_IO_READWRITE = 0x01
};

enum  // file creation flags
{
	KLAV_FILE_CREATE_MODE       = 0x00FF,
	KLAV_FILE_CREATE_NEW        = 0x0001,
	KLAV_FILE_CREATE_ALWAYS     = 0x0002,
	KLAV_FILE_OPEN_EXISTING     = 0x0003,
	KLAV_FILE_OPEN_ALWAYS       = 0x0004,
	KLAV_FILE_TRUNCATE_EXISTING = 0x0005,
	// flags
	KLAV_FILE_F_READ_ONLY       = 0x0100,
	KLAV_FILE_F_TEMPORARY       = 0x0200,   // delete on close
};

// attribute group: general file attributes
#define KLAV_IO_ATTRS_GENERAL 0x0

// file is a directory
#define KLAV_IO_ATTR_IS_DIRECTORY 0x00000010
// device file (for UNIX)
#define KLAV_IO_ATTR_IS_DEVICE    0x00000040
// regular file, i.e. not directory, device, pipe, socket, etc.
#define KLAV_IO_ATTR_IS_FILE      0x00000080
//file or direcory is read only
#define KLAV_IO_ATTR_READ_ONLY	  0x00000100

enum // make_full_path () flags
{
	KLAV_PATH_F_SEARCH_MODE_MASK = 0x0000FF,  // search mode mask
	KLAV_PATH_F_NORMALIZE_CASE   = 0x010000,  // convert for lowercase for Windows
};

enum // make_full_path search modes
{
	KLAV_PATH_SEARCH_MODE_NONE    = 0,   // just normalize absolute path
	KLAV_PATH_SEARCH_MODE_EXE     = 1,   // search executable file
	KLAV_PATH_SEARCH_MODE_DLL     = 2,   // search DLL (shared lib)
	KLAV_PATH_SEARCH_MODE_BASE    = 3,   // search using KAV base dir
	KLAV_PATH_SEARCH_MODE_PRODUCT = 4,   // search using product dir
};

#ifdef __cplusplus

struct KLAV_NO_VTABLE KLAV_IO_Library
{
	// get IO library version
	virtual uint32_t KLAV_CALL get_io_version (
			) = 0;

	// get IO extension interface
	virtual void * KLAV_CALL get_io_iface (
				uint32_t ext_iface_id
			) = 0;

	virtual KLAV_ERR KLAV_CALL create_file (
				KLAV_CONTEXT_TYPE context,
				const char * path,
				uint32_t access,
				uint32_t flags,
				KLAV_Properties *props,
				KLAV_IO_Object **pobject
			) = 0;

	virtual KLAV_ERR KLAV_CALL delete_file (
				KLAV_CONTEXT_TYPE context,
				const char * path,
				uint32_t     flags
			) = 0;

	virtual KLAV_ERR KLAV_CALL get_file_attrs (
				KLAV_CONTEXT_TYPE context,
				const char * path,
				uint32_t     attrs_grp,
				uint32_t *   pvalue
			) = 0;

	virtual KLAV_ERR KLAV_CALL set_file_attrs (
				KLAV_CONTEXT_TYPE context,
				const char *    path,
				uint32_t        attrs_grp,
				uint32_t		pvalue
			) = 0;

	virtual KLAV_ERR KLAV_CALL move_file (
				KLAV_CONTEXT_TYPE context,
				const char * src_path,
				const char * dst_path,
				uint32_t     flags
			) = 0;

	virtual KLAV_ERR KLAV_CALL make_full_path (
				KLAV_CONTEXT_TYPE context,
				const char * src_path,
				uint32_t     flags,     // KLAV_PATH_F_XXX, KLAV_PATH_SEARCH_XXX
				KLAV_Alloc * path_allocator,
				char ** pdst_path
			) = 0;

    KLAV_DUMMY_METHODS_DECL(KLAV_IO_Library)
};

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL(KLAV_IO_Library)
{
	uint32_t (KLAV_CALL *get_io_version) (
				struct KLAV_IO_Library *
			);

	void *   (KLAV_CALL *get_io_iface) (
				struct KLAV_IO_Library *,
				uint32_t
			);

	KLAV_ERR (KLAV_CALL *create_file) (
				struct KLAV_IO_Library *,
				KLAV_CONTEXT_TYPE,
				const char *,
				uint32_t,
				uint32_t,
				struct KLAV_Properties *,
				struct KLAV_IO_Object **
			);

	KLAV_ERR (KLAV_CALL *delete_file) (
				struct KLAV_IO_Library *,
				KLAV_CONTEXT_TYPE,
				const char *,
				uint32_t
			);

	KLAV_ERR (KLAV_CALL *get_file_attrs) (
				struct KLAV_IO_Library *,
				KLAV_CONTEXT_TYPE,
				const char *,
				uint32_t,
				uint32_t *
			);

	KLAV_ERR (KLAV_CALL *set_file_attrs) (
				struct KLAV_IO_Library *,
				KLAV_CONTEXT_TYPE,
				const char *,
				uint32_t,
				uint32_t *
			);

	KLAV_ERR (KLAV_CALL *move_file) (
				struct KLAV_IO_Library *,
				KLAV_CONTEXT_TYPE,
				const char *,
				const char *,
				uint32_t
			);

	KLAV_ERR (KLAV_CALL *make_full_path) (
				struct KLAV_IO_Library *,
				KLAV_CONTEXT_TYPE,
				const char *,
				uint32_t,
				struct KLAV_Alloc *,
				char **
			);
};

KLAV_DECLARE_C_INTERFACE(KLAV_IO_Library);

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// Parameter structure for KLAV_IO_Library creation

struct KLAV_IO_Library_Create_Parms
{
	const char * iolib_product_dir; // product directory
	const char * iolib_base_dir;    // av base directory
};

////////////////////////////////////////////////////////////////
// Virtual IO object

KLAV_ERR KLAV_Create_Virtual_IO (
			struct KLAV_Alloc *allocator,
			struct KLAV_IO_Object *base_io,
			klav_filepos_t origin,
			struct KLAV_IO_Object **ppvio
		);

////////////////////////////////////////////////////////////////
// Temporary object manager

#ifdef __cplusplus

struct KLAV_NO_VTABLE KLAV_Temp_Object_Manager
{
	virtual KLAV_ERR KLAV_CALL create_temp_object (
				KLAV_CONTEXT_TYPE context,
				KLAV_Properties * props,
				KLAV_IO_Object **ptempobj
			) = 0;

    KLAV_DUMMY_METHODS_DECL(KLAV_Temp_Object_Manager)
};

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL(KLAV_Temp_Object_Manager)
{
	KLAV_ERR (KLAV_CALL *create_temp_object) (
				struct KLAV_Temp_Object_Manager *,
				KLAV_CONTEXT_TYPE,
				struct KLAV_Properties *,
				struct KLAV_IO_Object **
			);
};

KLAV_DECLARE_C_INTERFACE(KLAV_Temp_Object_Manager);

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// Dynamic library loader

// flags
enum
{
	KLAV_DSO_LOAD_NATIVE = 0x01,  // use native system loader
};

typedef void * hKLAV_Module;

#ifdef __cplusplus

struct KLAV_NO_VTABLE KLAV_DSO_Loader
{
	virtual KLAV_ERR KLAV_CALL dl_open  (const char *path, hKLAV_Module *phdl) = 0;
	virtual void *   KLAV_CALL dl_sym   (hKLAV_Module hdl, const char *name) = 0;
	virtual void     KLAV_CALL dl_close (hKLAV_Module hdl) = 0;

    KLAV_DUMMY_METHODS_DECL(KLAV_DSO_Loader)
};

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL(KLAV_DSO_Loader)
{
	KLAV_ERR * (KLAV_CALL *dl_open)  (struct KLAV_DSO_Loader *, const char *, hKLAV_Module*);
	void *     (KLAV_CALL *dl_sym)   (struct KLAV_DSO_Loader *, hKLAV_Module, const char *);
	void       (KLAV_CALL *dl_close) (struct KLAV_DSO_Loader *, hKLAV_Module);
};

KLAV_DECLARE_C_INTERFACE(KLAV_DSO_Loader);

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// Utilites 

KLAV_ERR KLAV_IO_Get_Current_Pos (struct KLAV_IO_Object *io, klav_filepos_t *ppos);

KLAV_ERR KLAV_IO_Set_Current_Pos (struct KLAV_IO_Object *io, klav_filepos_t pos);

////////////////////////////////////////////////////////////////
// I/O object implementation over memory buffer

KLAV_ERR KLAV_Buffer_IO_Create (
		hKLAV_Alloc alloc, 
		const void *data,
		size_t size, 
		klav_bool_t ro, 
		hKLAV_IO_Object *pio
	);

#endif // klav_io_h_INCLUDED
