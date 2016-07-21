// klav_direnum.h
//

#ifndef klav_direnum_h_INCLUDED
#define klav_direnum_h_INCLUDED

#include <klava/klavsys.h>

////////////////////////////////////////////////////////////////

#ifdef KL_PLATFORM_WINDOWS
# define KL_PATH_SEPARATOR '\\'
# define KL_IS_PATH_SEPARATOR(C) ((C)=='/' || (C)=='\\')
#else
# define KL_PATH_SEPARATOR '/'
# define KL_IS_PATH_SEPARATOR(C) ((C)=='/')
#endif

////////////////////////////////////////////////////////////////

enum // file type flags
{
	KLAV_FILE_F_REG = 0x0001,  // regular file
	KLAV_FILE_F_DIR = 0x0002,  // directory
	
	KLAV_FILE_F_ALL = 0x00FF   // all file types
};

size_t KLAVSYS_API KLAV_Dir_Name_Len (const char *path);

// returns combination of KLAV_FILE_F_xxx flags
uint32_t KLAVSYS_API KLAV_Query_File_Type (const char *path);

klav_bool_t KLAVSYS_API KLAV_Name_Has_Wildcards (const char *fname);

////////////////////////////////////////////////////////////////

struct KLAVSYS_API KLAV_Dir_Enum
{
public:
	KLAV_Dir_Enum ();
	~KLAV_Dir_Enum ();
	
	KLAV_ERR    open (const char *dir, const char *fname_mask, uint32_t flags);
	klav_bool_t get_next_file ();
	void        close ();

	// valid when get_next_file () returns true
	const char *   file_name () const;
	uint32_t       file_flags () const;
	klav_filepos_t file_size () const;

private:
	struct KLAV_Dir_Enum_Impl * m_pimpl;

	KLAV_Dir_Enum (const KLAV_Dir_Enum&);
	KLAV_Dir_Enum& operator= (const KLAV_Dir_Enum&);
};

////////////////////////////////////////////////////////////////

struct KLAV_NO_VTABLE KLAV_Process_File_Callback
{
	// return value: error - stop processing
	virtual KLAV_ERR KLAV_CALL process_file (const char *path) = 0;
};

KLAV_ERR KLAVSYS_API KLAV_Process_Files (
		const char *pathspec,
		klav_bool_t recursive,
		klav_bool_t sort_order,
		KLAV_Process_File_Callback *pcb
	);

////////////////////////////////////////////////////////////////


#endif // klav_direnum_h_INCLUDED

