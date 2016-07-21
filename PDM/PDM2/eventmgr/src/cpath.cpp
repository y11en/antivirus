#include <assert.h>
#include "../include/cpath.h"
#include "../include/envhelper.h"
#include "../../heuristic/srv.h"

void cPath::clear()
{
	if (m_path) tstrfree(m_path), m_path = NULL;
	if (m_fullpath) tstrfree(m_fullpath), m_fullpath = NULL;
	if (m_default_dir) tstrfree(m_default_dir), m_default_dir = NULL;
	m_filename = NULL;
	m_extension = NULL;
	m_fullpath_len = 0;
	m_attributes = FILE_ATTRIBUTE_UNDEFINED;
}

void cPath::init()
{
	m_path = NULL;
	m_fullpath = NULL;
	m_default_dir = NULL;
	env = NULL;
	clear();
}

cPath::cPath()
{
	init();
}

void cPath::assign(cEnvironmentHelper* pEnvironmentHelper, tcstring path, tcstring fullpath /* = NULL */, uint32_t attributes /* = FILE_ATTRIBUTE_UNDEFINED */, tcstring default_dir /* = NULL */)
{
	clear();
	env = pEnvironmentHelper;
	if (path)
		m_path = tstrdup(path);
	else if (fullpath)
		m_path = tstrdup(fullpath);
	else
		m_path = NULL;
	if (fullpath)
	{
		m_fullpath = tstrdup(fullpath);
		tstrupr(m_fullpath);
	}
	else
		m_fullpath = NULL;
	if (default_dir && tstrchar(default_dir))
		m_default_dir = tstrdup(default_dir);
	else
		m_default_dir = NULL;
	m_attributes = attributes;
}

cPath::cPath(cPath& path)
{
	init();
	assign(path.env, path.m_path, path.m_fullpath, path.m_attributes, path.m_default_dir);
}

cPath::cPath(cEnvironmentHelper* pEnvironmentHelper, tcstring path, tcstring fullpath /* = NULL */, uint32_t attributes /* = FILE_ATTRIBUTE_UNDEFINED */, tcstring default_dir /* = NULL */)
{
	init();
	assign(pEnvironmentHelper, path, fullpath, attributes, default_dir);
}

cPath::~cPath()
{
	clear();
}

tcstring cPath::get()
{
	return m_path;
}

tcstring cPath::appendSlash()
{
	if (!m_fullpath)
		getFull();
	if (!m_fullpath)
		return NULL;
	
	tcstring last = tstrgetlastchar(m_fullpath);
	if (last && *last == '\\')
		return m_fullpath;

	m_fullpath = tstrappend(m_fullpath, _T("\\"));
	return m_fullpath;
}

tcstring SkipUnicodePrefix(tcstring path)
{
	tcstring p = path;
	if (*p != '\\')
		return path;
	p = tstrinc(p);
	if (*p != '\\' && *p != '?')
		return path;
	p = tstrinc(p);
	if (*p != '?')
		return path;
	p = tstrinc(p);
	if (*p != '\\')
		return path;
	p = tstrinc(p);
	return p;
}

tstring SkipUnicodePrefix(tstring path)
{
	return const_cast<tstring>(SkipUnicodePrefix(tcstring(path)));
}

tcstring cPath::getFull()
{
	if (m_fullpath)
		return m_fullpath;
	
	tstring fullpath;
	tstring path = SkipUnicodePrefix(m_path);
	fullpath = path;

	if (tstrchr(fullpath, '%'))
		fullpath = env->ExpandEnvironmentStr(fullpath);

	if (0 == tstrnicmp(fullpath, _T("\\systemroot\\"), 12)) // drivers may use this prefix
	{
		tstring temp = tstrdup(_T("%systemroot%\\"));
		temp = tstrappend(temp, tstrninc(fullpath, 12));
		if (fullpath != path)
			tstrfree(fullpath);
		fullpath = temp;
	}

	if (m_default_dir) 
	{
		if (!env->PathIsNetwork(fullpath))
		{
			tcstring ptr;
			if (tstrchar(fullpath) == '\\') // root folder
			{
				ptr = tstrinc(m_default_dir);
				if (tstrchar(ptr) == ':') // drive present in default_dir
				{
					// append drive
					tstring temp = tstralloc(tstrlen(fullpath) + 20);
					if (!temp)
						return NULL;
					tstrncpy(temp, m_default_dir, 2);
					tstrcpy(tstrninc(temp, 2), fullpath);
					if (fullpath != path)
						tstrfree(fullpath);
					fullpath = temp;
				}
			}
			else
			{
				ptr = tstrinc(fullpath);
				if (tstrchar(ptr) != ':') // drive not present in fullpath
				{
					// insert default dir
					tstring temp = tstrdup(m_default_dir);
					if (!temp)
						return NULL;
					if (tstrchar(tstrgetlastchar(m_default_dir)) != '\\')
						temp = tstrappend(temp, _T("\\"));
					if (!temp)
						return NULL;
					temp = tstrappend(temp, fullpath);
					if (!temp)
						return NULL;
					if (fullpath != path)
						tstrfree(fullpath);
					fullpath = temp;
				}
			}
		}
		
		// repeat expand
		if (tstrchr(fullpath, '%')) {
			tstring temp = env->ExpandEnvironmentStr(fullpath);
			if (fullpath != path)
				tstrfree(fullpath);
			fullpath = temp;
		}
	}

	if (tstrchr(fullpath, '~'))
	{
		tstring temp = env->PathGetLong(fullpath);
		if (fullpath != path)
			tstrfree(fullpath);
		fullpath = temp;
	}

	if (fullpath == path)
		fullpath = tstrdup(path);

	tstring p = fullpath;
	tchar c;
	while(c = tstrchar(p))
	{
		if ('/' == c)
			tstrchar(p) = '\\';
		p = tstrinc(p);
	};
	
	m_fullpath = fullpath;
	//m_fullpath = srv->FileGetFullPath(m_path);
	if (m_fullpath)
	{
		tstrupr(m_fullpath);
		tcstring last = tstrgetlastchar(m_fullpath);
		if (last && (*last != '\\')) // last char is not slash
		{
			uint32_t attr = getAttributes();
			if (attr != FILE_ATTRIBUTE_INVALID && (attr & FILE_ATTRIBUTE_DIRECTORY))
				appendSlash();
		}
	}
	return m_fullpath;
}

uint32_t cPath::getAttributes()
{
	if (m_attributes == FILE_ATTRIBUTE_UNDEFINED)
		m_attributes = env->FileAttr(getFull());
	return m_attributes;
}

bool cPath::isExist()
{
	if (FILE_ATTRIBUTE_INVALID == getAttributes())
		return false;
	return true;
}

cPath& cPath::operator = (tcstring path)
{
	clear();
	if (path)
		m_path = tstrdup(path);
	return *this;
}

cPath& cPath::operator = (const cPath& path)
{
	clear();
	if (path.m_path) m_path = tstrdup(path.m_path);
	if (path.m_fullpath) m_fullpath = tstrdup(path.m_fullpath);
	m_attributes = path.m_attributes;
	return *this;
}

size_t cPath::getFullLen()
{
	if (!m_fullpath_len)
		m_fullpath_len = tstrlen(m_fullpath);
	return m_fullpath_len;
}

tcstring cPath::getFilename()
{
	if (m_filename)
		return m_filename;
	tcstring full = getFull();
	if (!full)
		return NULL;
	m_filename = tstrrchr(full, '\\');
	if (m_filename)
		m_filename = tstrinc(m_filename);
	else
		m_filename = full;
	return m_filename;
}

tcstring cPath::getExtension()
{
	if (m_extension)
		return m_extension;
	tcstring filename = getFilename();
	if (!filename)
		return NULL;
	m_extension = tstrrchr(filename, '.');
	if (m_extension)
		m_extension = tstrinc(m_extension);
	else
		m_extension = _T("");
	return m_extension;
}

bool cPath::compare(tcstring path)
{
	cPath cmp_path(env, path);
	return srvComparePath(env, getFull(), cmp_path.getFull());
}

cFile::cFile(cEnvironmentHelper* pEnvironmentHelper, tcstring path, tcstring fullpath, uint32_t attributes, tHANDLE handle, uint64_t uniq, FILE_FORMAT_INFO* pfi) : 
	cPath(pEnvironmentHelper, path, fullpath, attributes),
	m_handle(handle),
	m_bTryOpenDone(false),
	m_bReadTry(false),
	m_bReadable(false),
	m_bOpened(false),
	m_uniq(uniq)
{
	if (pfi)
		memcpy(&fi, pfi, sizeof(fi));
	else
		memset(&fi, 0, sizeof(fi));
}

cFile::cFile(cPath& path, FILE_FORMAT_INFO* pfi) : 
	cPath(path),
	m_handle(INVALID_HANDLE),
	m_bTryOpenDone(false),
	m_bReadTry(false),
	m_bReadable(false),
	m_bOpened(false),
	m_uniq(0)
{
	if (pfi)
		memcpy(&fi, pfi, sizeof(fi));
	else
		memset(&fi, 0, sizeof(fi));
}

bool cFile::Read(uint64_t pos, void* buff, uint32_t buff_size, uint32_t* bytes_read)
{
	if (m_handle == INVALID_HANDLE)
	{
		if (m_bTryOpenDone)
		{
			if (bytes_read)
				*bytes_read = 0;
			return false;
		}
		m_bTryOpenDone = true;
		m_handle = env->FileOpen(getFull());
		if (m_handle == INVALID_HANDLE)
		{
			if (bytes_read)
				*bytes_read = 0;
			return false;
		}
		m_bOpened = true;
	}
	bool res = env->FileRead(m_handle, pos, buff, buff_size, bytes_read);
	if (res && !m_bReadable)
		m_bReadTry = m_bReadable = true;
	return res;
}

bool cFile::isReadable()
{
	if (!m_bReadTry)
	{
		char c;
		m_bReadTry = true;
		m_bReadable = Read(0, &c, 1, NULL);
	}
	return m_bReadable;
}

bool cFile::isExecutable()
{
	return srvIsFileExecutable(env, *this);
}

uint64_t cFile::Size()
{
	if (m_handle != INVALID_HANDLE)
		return env->FileSizeByHandle(m_handle);
	return env->FileSizeByName(getFull());
}

cFile::~cFile()
{
	if (m_bOpened)
		env->FileClose(m_handle);
}

bool cFile::ReadLine(uint64_t pos, void* buff, uint32_t buff_size, uint32_t* bytes_read, uint32_t* delim_len)
{
	assert(bytes_read);
	if (!bytes_read)
		return false;

	if (!Read(pos, buff, buff_size, bytes_read))
		return false;

	for (unsigned i = 0; i < *bytes_read; ++i)
	{
		if (((char*)(buff))[i] == '\n')
		{
			if (delim_len)
				*delim_len = 1;
			*bytes_read = i;
			return true;
		}
		else if (((char*)(buff))[i] == '\r')
		{
			if (delim_len) {
				char next_char;
				if (i + 1 == *bytes_read) {
					if (!Read(pos + i + 1, &next_char, 1, NULL))
						*delim_len = 1;
				} else {
					next_char = ((char*)(buff))[i + 1];
				}
				*delim_len = (next_char == '\n' ? 2 : 1);
			}
			*bytes_read = i;
			return true;
		}
	}

	return true;
}
