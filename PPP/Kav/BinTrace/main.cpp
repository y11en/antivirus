#include <io.h>
#include <share.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h> 
#include <sys/stat.h>

#include "file_al.h"
#include "trace.h"
#include "bintrace.h"
#include "btdeser.h"

#define MODE_STORE     1
#define MODE_DECODE    2

static char  g_storage_path[_MAX_PATH*2] = "";
static char* g_storage_fn = g_storage_path;
static char  g_log_path[_MAX_PATH*2] = "";
static char* g_log_fn = g_log_path;
static char  g_src_path[_MAX_PATH*2] = "";
static char* g_src_fn = g_log_path;
static char  g_out_path[_MAX_PATH*2] = "";
static char* g_out_fn = g_log_path;
static char  g_backup_path[_MAX_PATH*2] = "";
static char* g_backup_fn = g_storage_path;
static char* g_excludes[100] = {0};
static int   g_excludes_count = 0;
static int   g_errors_count = 0;
static int   g_mode = 0;

#define TDB_MAGIC '1BDT'
#define TDBSTORE_MAGIC '1SDT'

typedef struct _tdbi_header {
	unsigned long magic;         // signature "TDB1"
	unsigned long hsize;         // header size
	unsigned long last_fileid;   // last fileid
	unsigned long reserved[10];  
} t_tdbstore_header;

typedef struct _tdb_header {
	unsigned long magic;         // signature "TDB1"
	unsigned long hsize;         // header size
	unsigned long fileid;        // assigned fileid
	char filename[_MAX_PATH];    // filename
	time_t mtime;                // last modify time
	unsigned long traces_count;  // traces count
	char**        traces;        // traces strings
	unsigned long reserved[10];  
} t_tdb_header;

typedef struct _str_vector {
	char** item;
	unsigned int count;
	unsigned int allocated;
} t_str_vector;

bool init_str_vector(t_str_vector* pvector)
{
	memset(pvector, 0, sizeof(t_str_vector));
	return true;
}

bool push_str_vector(t_str_vector* pvector, char* str)
{
	char* p = (char*)malloc(strlen(str)+1);
	if (!p)
		return false;
	if (pvector->count == pvector->allocated)
	{
		pvector->allocated += 100;
		pvector->item = (char**)realloc(pvector->item, pvector->allocated * sizeof(char*));
		if (!pvector->item)
		{
			pvector->allocated = 0;
			pvector->count = 0;
			return false;
		}
	}
	strcpy(p, str);
	pvector->item[pvector->count] = p;
	pvector->count++;
	return true;
}

bool push_uniq_str_vector(t_str_vector* pvector, char* str)
{
	for (unsigned int i=0; i<pvector->count; i++)
	{
		if (!strcmp(pvector->item[i], str))
			return true;
	}
	return push_str_vector(pvector, str);
}

void free_str_vector(t_str_vector* pvector)
{
	for (unsigned int i=0; i<pvector->count; i++)
		free(pvector->item[i]);
	free(pvector->item);
	init_str_vector(pvector);
}

class cStrVector {
public:
	cStrVector()  { init_str_vector(&m_vector); };
	~cStrVector() { free_str_vector(&m_vector); };
	operator  t_str_vector*() { return &m_vector; };
	t_str_vector* operator->() { return &m_vector; };
private:
	t_str_vector m_vector;
};

bool make_storage_path(unsigned long fileid, char* filename)
{
	unsigned char* pnum = (unsigned char*)&fileid;
	sprintf(filename, "%02X\\%02X\\%02X\\%08x.tdb", pnum[3], pnum[2], pnum[1], fileid);
	return true;
}


typedef struct _tdb_list_item {
	unsigned long fileid;
	t_tdb_header* tdb;
} t_tdb_list_item;

typedef struct _tdb_list {
	unsigned long count;
	unsigned long allocated;
	t_tdb_list_item* items;
} t_tdb_list;

bool tdb_list_init(t_tdb_list* ptdb_list)
{
	memset(ptdb_list, 0, sizeof(t_tdb_list));
	return true;
}

bool tdb_list_free(t_tdb_list* ptdb_list)
{
	unsigned long i;
	for (i=0; i<ptdb_list->count; i++)
	{
		if (ptdb_list->items[i].tdb)
			free(ptdb_list->items[i].tdb);
	}
	free(ptdb_list->items);
	tdb_list_init(ptdb_list);
	return true;
}

t_tdb_header* tdb_load(unsigned long fileid)
{
	cFile tdbfile;
	make_storage_path(fileid, g_storage_fn);
	if (!tdbfile.Open(g_storage_path, "rb"))
	{
		_Trace( 0, TRACE_PREFIX "Error: cannot open storage file '%s'\n", g_storage_path);
		return NULL;
	}
	tFileSize size;
	tdbfile.GetSize(&size);
	t_tdb_header* tdbdata = (t_tdb_header*)malloc(size);
	if (!tdbdata)
		return NULL;
	if (!tdbfile.Read(tdbdata, size, 0))
	{
		_Trace( 0, TRACE_PREFIX "Error: cannot read storage file '%s'\n", g_storage_path);
		free(tdbdata);
		return NULL;
	}
	if (tdbdata->magic != TDB_MAGIC)
	{
		_Trace( 0, TRACE_PREFIX "Error: corrupted storage file '%s'\n", g_storage_path);
		free(tdbdata);
		return NULL;
	}
	unsigned long i;
	char* traces_base = (char*)tdbdata + tdbdata->hsize + tdbdata->traces_count*sizeof(unsigned long);
	tdbdata->traces = (char**)((char*)tdbdata + tdbdata->hsize);
	for (i=0; i<tdbdata->traces_count; i++)
		tdbdata->traces[i] = traces_base + (unsigned long)tdbdata->traces[i];
	
	return tdbdata;
}

t_tdb_header* tdb_list_find(t_tdb_list* ptdb_list, unsigned long fileid)
{
	unsigned long i;
	t_tdb_header* tdb;
	for (i=0; i<ptdb_list->count; i++)
	{
		if (ptdb_list->items[i].fileid == fileid)
		{
			tdb = ptdb_list->items[i].tdb;
			// bubble
			if (i > 0)
			{
				t_tdb_list_item tmp = ptdb_list->items[i];
				ptdb_list->items[i] = ptdb_list->items[i-1];
				ptdb_list->items[i-1] = tmp;
			}
			return tdb;
		}
	}
	tdb = tdb_load(fileid);
	if (ptdb_list->allocated <= ptdb_list->count)
	{
		ptdb_list->allocated += 10;
		ptdb_list->items = (t_tdb_list_item*) realloc(ptdb_list->items, ptdb_list->allocated * sizeof(t_tdb_list_item));
	}
	if (!ptdb_list->items)
	{
		free(tdb);
		tdb_list_init(ptdb_list);
		return NULL;
	}
	ptdb_list->items[ptdb_list->count].fileid = fileid;
	
	ptdb_list->items[ptdb_list->count].fileid = fileid;
	ptdb_list->items[ptdb_list->count].tdb = tdb;
	ptdb_list->count++;
	return tdb;
}

void make_full_path(char* fullpath, char* path, int size, char** filepart)
{
	struct _stat st;
    int len;
    _fullpath(fullpath, path ? path : ".", size);
    len = strlen(fullpath);
    if (0 == _stat(fullpath, &st))
    {
		if (st.st_mode & _S_IFDIR) // directory specified
		{
			if (fullpath[len-1] != '\\') // cut ending slash
			{
				fullpath[len++] = '\\';
				fullpath[len] = 0;
			}
			if (filepart)
				*filepart = fullpath+len;
		}
		else
		{
			if (filepart)
			{
				char* p = strrchr(fullpath, '\\');
				if (p)
					*filepart = p+1;
				else
					*filepart = fullpath;
			}
		}
	}
}

bool is_param(const char* param, const char* param_name, char** param_value)
{
	int len = strlen(param_name);
	if (0 != strnicmp(param, param_name, len))
		return false;
	if (param[len] == 0)
	{
		if (param_value)
			*param_value = 0;
		return true;
	}
	if (param[len] == ':' || param[len] == '=')
	{
		if (param_value)
			*param_value = (char*)param+len+1;
		return true;
	}
	return false;
}


bool convert_format(unsigned char* data, size_t size, bool verify_only)
{
	size_t i,j;
	for (i=0, j=0; i<size; i++)
	{
		char prefix;
		char type;
		if (data[i] == 0)
			break;
		if (data[i]<0x20 && data[i]!=0x09 && data[i]!=0x0A && data[i]!=0x0D) // invalid chars
			return false;
		if (data[i] != '%')
			continue;
		prefix = 0;
		type = 0;
		while (i<size)
		{
			i++;
			unsigned char c = data[i];
			if ( '0'==c || '#'==c || '+'==c || '-'==c || ' '==c ) // flags
				continue;
			if ( ( c>='1' && c<='9' ) || c=='.') // width.precision
				continue;
			if ( c == '*' )
			{
				if (!verify_only)
				{
					data[j] = '*';
					j++;
				}
				continue;
			}
			if (!prefix)
			{
				if ( 'h'==c || 'l'==c || 'L'==c || ( 'I'==c && '6'==data[i+1] && '4'==data[i+2] )) // prefixes
				{
					prefix = c;
					if ('I' == c) 
						i+=2;
					continue;
				}
			}
			// types
			switch (c)
			{
			case 'c': case 'C': // char
				type = 'i'; 
				break;
			case 'd': case 'i': case 'o': case 'u': case 'x': case 'X': // integer
				if ('I'==prefix) // __int64
					type = 'I';
				else
					type = 'i'; 
				break;
			case 'e': case 'E': case 'f': case 'g': case 'G': // float
				type = 'f';
				break;
			case 'p': // pointer
				type = 'p';
				break;
			case 'S': case 's': // string
				type = c;
				if (prefix == 'h')
					type = 's';
				if (prefix == 'l')
					type = 'S';
				break;
			case 't': // prague types
				type = 'i';
				if (data[i+1] == 's' && data[i+2] == 't' && data[i+3] == 'r') // hSTRING
					type = 'P';
				break;
			default:
				return false;
			}
			break;
		}
		if (!verify_only)
		{
			data[j] = type;
			j++;
		}
	}
	if (!verify_only)
	{
		while (j<i)
		{
			data[j] = 0;
			j++;
		}
	}
	return true;
}

int _mkdir_recurse (char* dir)
{
	int res;
	//      res = mkdir(dir);
	//      if (res == 0)
	//              return res;
	//      if (res == -1 && errno == EEXIST)
	//              return res = 0;
	char* p = strrchr(dir, '\\');
	if (p)
	{
		*p = 0;
		res = mkdir(dir);
		if (-1 == res && errno == ENOENT)
		{
			if (-1 != _mkdir_recurse(dir))
				res = mkdir(dir);
		}
		*p = '\\';
	}
	else
		res = mkdir(dir);
	
	return res;
}


int convert_file(char* input, char* backup, t_tdbstore_header* pstore_header, cFile* pIndex)
{       
	cStrVector traces;
	tFileSize fsize, _fsize = 0;
	cFile filein;
	cFile fileout;
	cFile tdb;
	unsigned int i;
	
	t_tdb_header tdb_header;
	memset(&tdb_header, 0, sizeof(tdb_header));
	tdb_header.magic = TDB_MAGIC;
	tdb_header.hsize = sizeof(tdb_header);
	make_full_path(tdb_header.filename, input, sizeof(tdb_header.filename), 0);
	
	if (!filein.Open(input, "rb", _SH_DENYWR))
	{
		_Trace( 0, TRACE_PREFIX "Error: cannot open source file '%s'\n", input);
		return 2;
	}
	
	if (!filein.GetSize(&fsize))
	{
		_Trace( 0, TRACE_PREFIX "Error: cannot get file size of '%s'\n", input);
		return 2;
	}
	
	unsigned char* data = (unsigned char*)malloc(fsize);
	if (!data)
	{
		_Trace( 0, TRACE_PREFIX "Error: not enough memory to load file (%u)\n", fsize);
		return 2;
	}
	
	if (!filein.Read(data, fsize, NULL))
	{
		_Trace( 0, TRACE_PREFIX "Error: cannot load file '%s'\n", input);
		return 2;
	}
	filein.Close();
	
	tdb_header.fileid = pstore_header->last_fileid;
	for(;;)
	{
		make_storage_path(tdb_header.fileid, g_storage_fn);
		if (-1 == _access(g_storage_path, 0)) // if not exist
			break;
		tdb_header.fileid++;
	}
	
	_fsize = fsize;
	if (fsize > 8)
		fsize -= 8;
	fsize &= ~7;
	tFileSize pos;
	for (pos=0; pos<fsize; pos+=4)
	{
		if ( *(unsigned long*)(data+pos) == TRACE_PREFIX_DW1 && *(unsigned long*)(data+pos+4) == TRACE_PREFIX_DW2 )
		{
			_Trace( 1, TRACE_PREFIX "Info: prefix found at %08X\n", pos, data+pos+8);
			// verify string
			
			if (!convert_format(data+pos+8, fsize-pos-8, true))
			{
				_Trace( 0, TRACE_PREFIX "Warning: invalid string at %08X, %s\n", pos, data+pos+8);
				continue;
			}
			
			// save full string
			data[pos] = 0x02; // binary trace prefix
			memcpy(data+pos+1, &traces->count, 3);
			memcpy(data+pos+4, &tdb_header.fileid, 4);
			push_str_vector(traces, (char*)data+pos+8);
			
			if (!convert_format(data+pos+8, fsize-pos-8, false))
			{
				_Trace( 0, TRACE_PREFIX "Warning: error converting string at %08X, %s\n", pos, data+pos+8);
				return 3;
			}
			_Trace( 1, TRACE_PREFIX "Info: trace at %08X replaced with '%s'\n", pos+8, data+pos+8);
		}
	}
	
	if (traces->count)
	{               
		pstore_header->last_fileid = tdb_header.fileid;
		_mkdir_recurse(g_storage_path);
		if (!tdb.Open(g_storage_path, "wb"))
		{
			_Trace( 0, TRACE_PREFIX "Error: cannot open file '%s'\n", g_storage_path);
			return 2;
		}
		
		bool wres = true;
		unsigned int tsize=0;
		unsigned long* poffsets = (unsigned long*)malloc(traces->count * sizeof(unsigned long));
		if (!poffsets)
		{
			_Trace( 0, TRACE_PREFIX "Error: not enough memory to save '%s'\n", g_storage_path);
			return 2;
		}
		for (i=0; i<traces->count; i++)
		{
			poffsets[i] = tsize;
			tsize += strlen(traces->item[i])+1;
		}
		char* ptraces = (char*)malloc(tsize);
		if (!ptraces)
		{
			_Trace( 0, TRACE_PREFIX "Error: not enough memory to save '%s'\n", g_storage_path);
			return 2;
		}
		for (i=0; i<traces->count; i++)
		{
			strcpy(ptraces+poffsets[i], traces->item[i]);
		}
		tdb_header.traces_count = traces->count;
		wres = tdb.Write(&tdb_header, sizeof(tdb_header), 0);
		wres = wres & tdb.Write(poffsets, traces->count * sizeof(unsigned long), 0);
		wres = wres & tdb.Write(ptraces, tsize, 0);
		if (!wres)
		{
			_Trace( 0, TRACE_PREFIX "Error: cannot save file '%s'\n", g_storage_path);
			return 2;
		}
		tdb.Close();
		
		char tdbname[32];
		sprintf(tdbname, "%08X.TDB", tdb_header.fileid);
		pIndex->Write(tdbname, strlen(tdbname), 0);
		pIndex->Write(",", 1, 0);
		pIndex->Write(g_src_path, strlen(g_src_path), 0);
		pIndex->Write("\n", 1, 0);
        
		if (-1 != unlink(backup))
		{
			_Trace( 0, TRACE_PREFIX "Warning: previous backup file has been deleted '%s'\n", backup);
		}
		if (0 != rename(input, backup))
		{
			_Trace( 0, TRACE_PREFIX "Error: cannot backup input file '%s'->'%s'\n", input, backup);
			return 2;
		}
		if (!fileout.Open(input, "wb", _SH_DENYWR))
		{
			_Trace( 0, TRACE_PREFIX "Error: cannot open output file '%s'\n", input);
			return 2;
		}
		if (!fileout.Write(data, _fsize, 0))
		{
			_Trace( 0, TRACE_PREFIX "Error: cannot save output file '%s'\n", input);
			return 2;
		}
		fileout.Close();
	}
	else
	{
		_Trace( 2, TRACE_PREFIX "Info: skipped - no traces found in file '%s'\n", input);
	}
	
	return 0;
}

int EnumSources(t_tdbstore_header* pstore_hdr, cFile* pIndex)
{
    struct _finddata_t fd;
    long hFind;
	make_full_path(g_src_path, g_src_path, _MAX_PATH, &g_src_fn);
	hFind = _findfirst(g_src_path, &fd);
    if (hFind == -1) 
    {
        _Trace( 0, TRACE_PREFIX "Error: File(s) not found '%s'\n", g_src_path);
        return errno;
    }
    do
    {
		bool skip = false;
		if (fd.attrib & _A_SUBDIR)
			continue;
        strcpy(g_src_fn, fd.name);
        strcpy(g_backup_fn, fd.name);
		//              for (i=0; i<g_excludes_count; i++)
		//              {
		//                      if (MatchWithPattern(src_fn, g_excludes[i], false))
		//                      {
		//                              _Trace( 2, TRACE_PREFIX "Info: '%s' excluded by '%s'\n", src_path, g_excludes[i]);
		//                              skip = true;
		//                              break;
		//                      }
		//              }
		//              if (!skip)
		{
			int result = convert_file(g_src_path, g_backup_path, pstore_hdr, pIndex);
		}               
    } while (_findnext(hFind, &fd) == 0);
    _findclose(hFind);
	return 0;
}

size_t find_btag(char* buffer, size_t data_offset, size_t data_size)
{
	char c;
	size_t size = 0;
	while (data_size)
	{
		c = buffer[data_offset+size];
		if (c == 0x01 || c == 0x02)
			break;
		size++;
	}
	//      data_offset+=size;
	//      data_size-=size;
	return size;
}

#define _MAX_ARGS 32

void free_allocated_args(void* args[], bool allocated[], size_t argc)
{
	size_t i;
	for (i=0; i<argc; i++)
	{
		if (allocated[i])
		{
			free(args[i]);
			allocated[i] = false;
			args[i] = 0;
		}
	}
}

int DoDecode()
{
	cFile tracebin;
	cFile tracetxt;
	char* buffer = NULL;
	size_t buffer_size = 0;
	size_t data_size = 0;
	size_t req_size = 0x1000;
	size_t data_offset = 0;
	unsigned char b;
	__int64 _time, _prev_time = 0;
	unsigned int fileid;
	unsigned int traceid;
	t_tdb_list tdb_list;
	void* args[_MAX_ARGS];
	bool  allocated[_MAX_ARGS];
	size_t i;
	
	tdb_list_init(&tdb_list);
	for (i=0; i<_MAX_ARGS; i++)
	{
		allocated[i] = false;
		args[i] = 0;
	}
	
	if (!tracebin.Open(g_src_path, "r+b"))
	{
		_Trace( 0, TRACE_PREFIX "Error: Cannot open input '%s'\n", g_src_path);
		return 1;
	}
	cCachedRead in(&tracebin);
	_mkdir_recurse(g_out_path);
	if (!tracetxt.Open(g_out_path, "w+b"))
	{
		_Trace( 0, TRACE_PREFIX "Error: Cannot open output '%s'\n", g_out_path);
		return 1;
	}
	cCachedWrite out(&tracetxt);
	
	size_t argc = 0;
	tFilePos pos = 0;
	bool retry = false;
	t_tdb_header* tdb = 0;
	size_t out_buff_size = 0x1000;
	char* out_buffer = (char*)malloc(out_buff_size);
	char* trace_fmt = 0;
	int header_time_format = 0;
	int flags = 0;
	char* header_internal_format = 0;
	char* header_output_format = 0;
	size_t data_len;
	do {
		free_allocated_args(args, allocated, argc);
		if (trace_fmt)
		{
			free(trace_fmt);
			char* trace_fmt = 0;
		}
		if (retry)
			out.Write(&b, 1, 0);                    
		retry = true;
		in.SetPos(pos);
		while (true)
		{
			if (!in.Read(&b))
				return 0;
			if (b == 0x01 || b == 0x02)
				break;
			out.Write(&b, 1, 0);
			pos++;
		} 
		pos = in.GetPos();
		if (!_bt_deserialize_int64(&_time, in))
			continue;
		if (b == 0x01)
		{
			if (!_bt_deserialize_int((int*)&data_len, in))
				continue;
			if (!_bt_deserialize("iiss", in, args, allocated, &argc))
				continue;
			if (argc >= 1)
				header_time_format = (enumBTTimeFormats)(int)args[0];
			if (argc >= 2)
				flags = (int)args[1];
			if (argc >= 3 && args[2])
				header_internal_format = strdup((char*)args[2]);
			if (argc >= 4 && args[3])
				header_output_format = strdup((char*)args[3]);
		}
		
		switch (header_time_format)
		{
		case eBTTimeFormat_time_t:
			if (_time < 60*60*24*365)
				_time += _prev_time;
			break;
		}
		_prev_time = _time;
		
		if (b == 0x02)
		{
			size_t out_len;
			_bt_deserialize_int((int*)&fileid, in);
			_bt_deserialize_int((int*)&traceid, in);
			tdb = tdb_list_find(&tdb_list, fileid);
			if (!_bt_deserialize_int((int*)&data_len, in))
				continue;
			if (!tdb)
				continue;
			if (traceid >= tdb->traces_count)
				continue;
			char* trace = tdb->traces[traceid];
			if (!trace)
				continue;
			char* trace_fmt = strdup(trace);
			if (!trace_fmt)
				continue;
			
			do
			{
				out_len = 0;
				switch (header_time_format)
				{
				case eBTTimeFormat_time_t:
					{
						struct tm* _tm;
						_tm = localtime((time_t*)&_time);
						out_len = _snprintf(out_buffer, out_buff_size, "%04d.%02d.%02d %02d:%02d:%02d\t", _tm->tm_year + 1900, _tm->tm_mon+1, _tm->tm_mday, _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
					}
					break;
				}
				if (out_len < 0)
				{
					out_buff_size += 0x1000;
					out_buffer = (char*)realloc(out_buffer, out_buff_size);
					if (!out_buffer)
						return 1;
				}
			} while (out_len < 0);
			out.Write(out_buffer, out_len, 0);
			
			if (header_internal_format)
			{
				if (!_bt_deserialize(header_internal_format, in, args, allocated, &argc))
					continue;
				if (header_output_format)
				{
					while (0 > (out_len = _vsnprintf(out_buffer, out_buff_size, header_output_format, (char*)args)))
					{
						out_buff_size += 0x1000;
						out_buffer = (char*)realloc(out_buffer, out_buff_size);
					}
					out.Write(out_buffer, out_len, 0);
					out_len = _snprintf(out_buffer, out_buff_size, "\t");
					out.Write(out_buffer, out_len, 0);
				}
				free_allocated_args(args, allocated, argc);
			}
			if (!convert_format((unsigned char*)trace_fmt, strlen(trace_fmt), false))
				continue;
			if (!_bt_deserialize(trace_fmt, in, args, allocated, &argc))
				continue;
			while (0 > (out_len = _vsnprintf(out_buffer, out_buff_size, trace, (char*)args)))
			{
				out_buff_size += 0x1000;
				out_buffer = (char*)realloc(out_buffer, out_buff_size);
			}
			out.Write(out_buffer, out_len, 0);
		}
		
		retry = false;
		pos = in.GetPos();
        } while (true);
		
        //make_storage_path(tdb_header.fileid, g_storage_fn);
		
        tdb_list_free(&tdb_list);
		
        return 1;
}

int DoStore()
{
	if (!g_storage_path[0])
	{
		_Trace( 0, TRACE_PREFIX "Error: Storage path not specified\n");
		return 1;
	}
	
	if (!g_backup_path[0])
	{
		_Trace( 0, TRACE_PREFIX "Error: Backup path not specified\n");
		return 1;
	}
	
	if (!g_src_path[0])
	{
		_Trace( 0, TRACE_PREFIX "Error: Source file not specified\n");
		return 1;
	}
	
//	_Trace( 0, TRACE_PREFIX "Test: %d %x %d %x %s %S %S %p %f %I64x\n", 1, 0x80000001, -1, 0x99999999, 
//		"test", L"TEST", L"реяр", -8, (float)-1, (__int64)-1);
//	_Trace( 0, TRACE_PREFIX "Test2: hello\n");
	
	{
		t_tdbstore_header store_hdr;
		cFile store;
		cFile index;
		strcpy(g_storage_fn, "tdbstore.dat");
		if (!store.Open(g_storage_path, "r+b", _SH_DENYWR))
		{
			if (!store.Open(g_storage_path, "w+b", _SH_DENYWR))
			{
				_Trace( 0, TRACE_PREFIX "Error: Cannot open store '%s'\n", g_storage_path);
				return 1;
			}
		}
		memset(&store_hdr, 0, sizeof(store_hdr));
		store_hdr.magic = TDBSTORE_MAGIC;
		store_hdr.hsize = sizeof(store_hdr);
		if (!store.Read(&store_hdr, sizeof(store_hdr), 0) || store_hdr.magic != TDBSTORE_MAGIC)
		{
			memset(&store_hdr, 0, sizeof(store_hdr));
			store_hdr.magic = TDBSTORE_MAGIC;
			store_hdr.hsize = sizeof(store_hdr);
		}
		
		strcpy(g_storage_fn, "tdbstore.idx");
		if (!index.Open(g_storage_path, "a+", _SH_DENYWR))
		{
			_Trace( 0, TRACE_PREFIX "Error: Cannot open store index '%s'\n", g_storage_path);
			return 1;
		}
		EnumSources(&store_hdr, &index);
		if (!store.Seek(0, SEEK_SET, NULL))
		{
			_Trace( 0, TRACE_PREFIX "Error: Cannot update store");
			return 1;
		}
		if (!store.Write(&store_hdr, sizeof(store_hdr), 0))
		{
			_Trace( 0, TRACE_PREFIX "Error: Cannot update store");
			return 1;
		}
	}
	if (g_errors_count)
		return 4;
	return 0;
}

void usage()
{
	printf("Trace Database utility v1.0   " __TIMESTAMP__ "\n");
	printf("Usage:\n");
	printf("\tTDB.EXE store <source_mask> -storage=<path> -backup=<path> [-log=<filename>] [-verbose]\n");
	printf("\tTDB.EXE decode <tracefile> -storage=<path> [-log=<filename>] [-verbose]\n");
	return;
}

int main(int argc, char* argv[])
{
	int i;
	
	// parse params
	for (i=1; i<argc; i++)
	{
		bool invalid = false;
		if (argv[i][0] == '-' || argv[i][0] == '/')
		{
			char* param = argv[i]+1;
			char* pvalue;
			if (is_param(param, "help", 0) || is_param(param, "?", 0))
			{
				usage();
				return 0;
			}
			if (is_param(param, "storage", &pvalue))
			{ 
				if (pvalue == 0)
					invalid = true;
				else
				{
					make_full_path(g_storage_path, pvalue, sizeof(g_storage_path), &g_storage_fn);
					_mkdir_recurse(g_storage_path);
					mkdir(g_storage_path);
					make_full_path(g_storage_path, pvalue, sizeof(g_storage_path), &g_storage_fn);
				}
			}
			else if (is_param(param, "backup", &pvalue))
			{ 
				if (pvalue == 0)
					invalid = true;
				else
				{
					make_full_path(g_backup_path, pvalue, sizeof(g_storage_path), &g_backup_fn);
					_mkdir_recurse(g_backup_path);
					mkdir(g_backup_path);
					make_full_path(g_backup_path, pvalue, sizeof(g_storage_path), &g_backup_fn);
				}
			}
			else if (is_param(param, "log", &pvalue) || is_param(param, "log", &pvalue))
            {
                make_full_path(g_log_path, pvalue ? pvalue : ".", _MAX_PATH, &g_log_fn);
				struct tm* _tm;
				time_t _t;
				_t = time(0);
				_tm = localtime(&_t);
				sprintf(g_log_fn, "%04d.%02d.%02d.LOG", _tm->tm_year + 1900, _tm->tm_mon+1, _tm->tm_mday);
                g_output = fopen(g_log_path, "a+");
                if (!g_output)
                {
					g_output = stdout;
					_Trace( 0, TRACE_PREFIX "Error: cannot open report file - %s\n", argv[i]);
					return 1;
                }
                setvbuf(g_output,  NULL, _IONBF, 0 );
            }
            else if (is_param(param, "verbose", &pvalue))
				g_trace_level = (pvalue ? strtoul(pvalue, 0, 10) : 100);
		}
		else if (!g_mode)
		{
			if (is_param(argv[i], "s", NULL) || is_param(argv[i], "store", NULL))
				g_mode = MODE_STORE;
			else if (is_param(argv[i], "d", NULL) || is_param(argv[i], "decode", NULL))
				g_mode = MODE_DECODE;
			else
				invalid = true;
		}
		else if (!g_src_path[0])
			make_full_path(g_src_path, argv[i], _MAX_PATH, &g_src_fn);
		else if (!g_out_path[0])
			make_full_path(g_out_path, argv[i], _MAX_PATH, &g_out_fn);
		else
			invalid = true;
		
		if (invalid)
		{
			_Trace( 0, TRACE_PREFIX "Error: invalid parameter - %s\n", argv[i]);
			return 1;
		}
	}
	
	if (g_mode == MODE_STORE)
		return DoStore();
	if (g_mode == MODE_DECODE)
		return DoDecode();
	
	usage();
	//_Trace( 0, TRACE_PREFIX "Error: Invalid parameters\n");
	return 1;
}