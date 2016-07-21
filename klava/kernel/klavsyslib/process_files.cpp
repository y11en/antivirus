// process_files.cpp
//
//

#include <klava/klav_direnum.h>
#include <string>
#include <vector>

struct str_holder_array : public std::vector <char *>
{
	str_holder_array () {}
	~str_holder_array () { clear (); }

	void clear ();
};

void str_holder_array::clear ()
{
	size_t i, cnt = size ();
	for (i = 0; i < cnt; ++i)
	{
		char *s = (*this) [i];
		if (s != 0)
			free (s);
	}
	resize (0);
}

inline unsigned char to_lower (unsigned char c)
{
	if (c >= 'A' && c <= 'Z')
		return c | 0x20;
	return c;
}

inline int str_cmp_nocase (const char *a, const char *b)
{
	for (; to_lower (*a) == to_lower (*b); ++a,++b)
	{
		if (*a == 0)
			return 0;
	}

	return to_lower (*a) < to_lower (*b) ? -1 : 1;
}

static void push_name_in_order (str_holder_array& arr, const char *name)
{
	if (name == 0)
		name = "";
	
	size_t len = strlen (name);
	
	char *s = (char *) malloc (len + 1);
	if (s == 0)
		return;

	memcpy (s, name, len);
	s [len] = 0;

	size_t idx = arr.size ();
	arr.resize (idx + 1);

	for (; idx > 0; --idx)
	{
		if (str_cmp_nocase (arr [idx - 1], s) <= 0)
			break;

		arr [idx] = arr [idx - 1];
		arr [idx - 1] = 0;
	}
	arr [idx] = s;
}

static KLAV_ERR process_dir (
		std::string& path,
		const char *filespec,
		klav_bool_t recursive,
		klav_bool_t sort_order,
		KLAV_Process_File_Callback *pcb
	)
{
	KLAV_ERR master_err = KLAV_OK;
	KLAV_Dir_Enum dir_enum;

	size_t dir_len = path.length ();
	
	KLAV_ERR err = dir_enum.open (path.c_str (), filespec, KLAV_FILE_F_REG);
	if (KLAV_FAILED (err))
		return err;

	str_holder_array sorted_names;

	while (dir_enum.get_next_file ())
	{
		const char *fname = dir_enum.file_name ();

		if (sort_order)
		{
			push_name_in_order (sorted_names, fname);
		}
		else
		{
			path.resize (dir_len);
			path += fname;

			err = pcb->process_file (path.c_str ());
			if (KLAV_FAILED (err))
			{
				master_err = err;
				if (err == KLAV_ECANCEL)  // TODO: make this optional
					break;
			}
		}
	}

	dir_enum.close ();

	if (sort_order)
	{
		size_t cnt = sorted_names.size ();
		for (size_t i = 0; i < cnt; ++i)
		{
			const char *fname = sorted_names [i];

			path.resize (dir_len);
			path += fname;

			err = pcb->process_file (path.c_str ());
			if (KLAV_FAILED (err))
			{
				master_err = err;
				if (err == KLAV_ECANCEL)  // TODO: make this optional
					break;
			}
		}
	}
	
	sorted_names.clear ();

	if (master_err == KLAV_ECANCEL) // TODO: make this optional
		return master_err;

	if (recursive)
	{
		path.resize (dir_len);

		err = dir_enum.open (path.c_str (), "*", KLAV_FILE_F_DIR);
		if (KLAV_FAILED (err))
			return err;

		while (dir_enum.get_next_file ())
		{
			const char *subdir = dir_enum.file_name ();

			if (sort_order)
			{
				push_name_in_order (sorted_names, subdir);
			}
			else
			{
				path.resize (dir_len);
				path += subdir;
				path += KL_PATH_SEPARATOR;

				err = process_dir (path, filespec, recursive, sort_order, pcb);
				if (KLAV_FAILED (err))
				{
					master_err = err;
					if (err == KLAV_ECANCEL)  // TODO: make this optional
						break;
				}
			}
		}

		dir_enum.close ();

		if (sort_order)
		{
			size_t cnt = sorted_names.size ();
			for (size_t i = 0; i < cnt; ++i)
			{
				const char *subdir = sorted_names [i];

				path.resize (dir_len);
				path += subdir;
				path += KL_PATH_SEPARATOR;

				err = process_dir (path, filespec, recursive, sort_order, pcb);
				if (KLAV_FAILED (err))
				{
					master_err = err;
					if (err == KLAV_ECANCEL)  // TODO: make this optional
						break;
				}
			}
		}
	}

	return master_err;
}



KLAV_ERR KLAV_Process_Files (
		const char *pathspec,
		klav_bool_t recursive,
		klav_bool_t sort_order,
		KLAV_Process_File_Callback *pcb
	)
{
	std::string dir;
	const char * filespec = 0;

	size_t dirlen = KLAV_Dir_Name_Len (pathspec);
	filespec = pathspec + dirlen;

	if (! KLAV_Name_Has_Wildcards (filespec))
	{
		// determine whether pathspec points to the directory
		uint32_t ft = KLAV_Query_File_Type (pathspec);
		if ((ft & KLAV_FILE_F_DIR) != 0)
		{
			// directory
//			if (! recursive)
//				return KLAV_EINVAL;

			dir = pathspec;
			filespec = "*";
		}
		else if ((ft & KLAV_FILE_F_REG) != 0)
		{
			// single file
			return pcb->process_file (pathspec);
		}
		else
		{
			// missing file
			return KLAV_EINVAL;
		}
	}
	else
	{
		// directory with file mask
		if (dirlen == 0)
		{
			dir = ".";
		}
		else
		{
			dir.assign (pathspec, pathspec + dirlen);
		}

		uint32_t ft = KLAV_Query_File_Type (dir.c_str ());
		if ((ft & KLAV_FILE_F_DIR) == 0)
			return KLAV_EINVAL;
	}

	// NOTE: dir is always non-empty
	char ct = dir [dir.length () - 1];

	if (! KL_IS_PATH_SEPARATOR (ct))
		dir += KL_PATH_SEPARATOR;

	KLAV_ERR err = process_dir (dir, filespec, recursive, sort_order, pcb);

	return err;
}


