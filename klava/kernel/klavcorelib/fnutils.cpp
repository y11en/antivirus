// fnutils.cpp
//
// Filename-related utilites
//

#include <klava/klav_utils.h>

// returns size of directory prefix (without filename, including trailing '/')
size_t fns_name_off (const char *path)
{
	if (path == 0)
		return 0;

	size_t len = strlen (path);
	size_t i = len;
	for (; i > 0; --i)
	{
		if (fns_is_path_sep (path [i - 1]))
			break;
	}

	return i;
}

// returns size of the file name without the last extension, if presents
size_t fns_ext_off (const char *fname)
{
	if (fname == 0)
		return 0;

	size_t len = strlen (fname);
	size_t i = len;
	for (; i > 0; --i)
	{
		if (fns_is_path_sep (fname [i-1]))
			break;

		if (fname [i-1] == '.')
			return i - 1;
	}
	return len;
}

size_t fns_is_root (const char *path)
{
	if (path == 0 || path [0] == 0)
		return 0;

#ifdef KL_PLATFORM_WINDOWS
	if (path [0] == '\\')
	{
		if (path [1] == '\\')
		{
			// UNC name
			const char *s = path + 2;
			while (*s != 0 && *s != '\\') ++s;
			if (*s == '\\')
			{
				s++;
				while (*s != 0 && *s != '\\') ++s;
				if (*s == '\\')
					++s;
			}
			return s - path;
		}
		return 1;
	}
	if (((path [0] > 'A' && path [0] <= 'Z')
	  || (path [0] > 'a' && path [0] <= 'z'))
	  && path [1] == ':')
	{
		if (path [2] == '\\' || path [2] == '/')
			return 3;
		return 2;
	}
#else
	if (fns_is_path_sep (path [0]))
		return 1;
#endif
	return 0;
}
