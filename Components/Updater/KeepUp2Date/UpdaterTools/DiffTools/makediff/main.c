#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <io.h>
#include <direct.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h> 
#include <sys/stat.h>

#include "../shared/differr.h"
#include "../shared/base64p.h"
#include "../shared/types.h"
#include "../shared/diff.h"
#include "../shared/md5.h"

#include "klz.h"
#include "trace.h"

#ifdef _WIN32
# include <windows.h>
#endif

#define _VERSION_ "v1.2.2("__TIMESTAMP__")"

static unsigned long g_rebuild_versions = ~0; // maximum possible
static unsigned long g_max_history_days = 0;
static unsigned long g_max_history_count = 0;
static unsigned int  g_priority = 15;
static unsigned int  g_excludes_count = 0;
static char*         g_excludes[100] = {0};
static bool          g_gen_klz = false;
static bool          g_verify_result = false;
static bool          g_force = false;
static int           g_tfc_size = 1024;
static time_t        g_time_limit = 0;

static char          log_path[_MAX_PATH*2] = "";
static char*         log_fn = log_path;
static char          diff_log_path[_MAX_PATH*2] = "";
static char*         diff_log_fn = diff_log_path;
static char          src_path[_MAX_PATH*2] = "";
static char*         src_fn = src_path;
static char          cache_path[_MAX_PATH*2] = "";
static char*         cache_fn = cache_path;
static char          sign_cache_path[_MAX_PATH*2] = "";
static char*         sign_cache_fn = sign_cache_path;
static char          hist_path[_MAX_PATH*2] = "";
static char*         hist_sub = hist_path;
static char*         hist_fn = hist_path;
static char          res_path[_MAX_PATH*2] = "";
static char*         res_fn = res_path;

static char          diff_list_path[_MAX_PATH*2] = "";
static char*         diff_list_fn = diff_list_path;
static unsigned long diff_list_count = 0;
static unsigned long diff_list_minutes = 0;
FILE*                diff_list_file = NULL;


#define SIGN_SIZE            (64)              /* 3.0 digital sign size */
#define HASH_SIZE 16

typedef struct tag_DIFF_CACHE_DATA {
	unsigned char hash_from[HASH_SIZE];
	unsigned char hash_to[HASH_SIZE];
	unsigned long diff_size;
} DIFF_CACHE_DATA;

#define SIGN_CACHE_MAGIC 'SIGC'
typedef struct tag_SRC_CACHE_DATA {
	unsigned long Magic;
	unsigned char hash[HASH_SIZE];
	unsigned char sign[SIGN_SIZE];
} SRC_CACHE_DATA;

static bool GetHashFromText(char* text, unsigned char hash[]);
static bool GetHashAsText(unsigned char hash[], char* buff, size_t buff_size);
static bool GetFileSignatureFixed(char* filename, unsigned char* pSignature, size_t nSignSize);
static bool GetFileNameFromTimeAndHash(time_t _t, unsigned char hash[], char* fname, size_t fname_buff_size);

static FILE* g_diff_trace = NULL;

void _CloseDiffTrace(void)
{
	if (g_diff_trace)
		fclose(g_diff_trace);
	g_diff_trace = NULL;
}

void _OpenDiffTrace(const char* filename)
{
	if (0 == *diff_log_path)
		return;
	_CloseDiffTrace();
	g_diff_trace = fopen(filename, "a+");
	if (!g_diff_trace)
		_Trace(0, "Error: cannot open diff log '%s'\n", filename);
}

void _DiffTrace(const char* format, ...)
{
	va_list _list;
	va_start(_list, format);
	if (g_diff_trace)
	{
		struct tm* _tm;
		time_t _t;
		_t = time(0);
		_tm = localtime(&_t);
		fprintf(g_diff_trace, "%04d-%02d-%02d %02d:%02d:%02d ", _tm->tm_year + 1900, _tm->tm_mon+1, _tm->tm_mday, _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
		vfprintf(g_diff_trace, format, _list);
	}
	_Trace1v(1, format, _list);
	va_end(_list);
}

void _DiffTrace2(const char* format, ...)
{
	va_list _list;
	va_start(_list, format);
	if (g_diff_trace)
		vfprintf(g_diff_trace, format, _list);
	_Trace2v(1, format, _list);
	va_end(_list);
}

void make_fullpath(char* full_path, char** full_path_fn, char* path, char* subfolder, bool directory)
{
	char* fn;
	int len;
	char* dup_path = NULL;
	_Trace( 4, "Info: make_fullpath: '%s' sub='%s' dir=%d\n", path, subfolder?subfolder:"", directory);
	if (full_path == path)
	{
		_Trace( 3, "Info: duping '%s'\n", path);
		dup_path = path = _strdup(path);
	}
	if (!_fullpath(full_path, path, _MAX_PATH))
	{
		_Trace( 1, "Info: _fullpath failed - coping '%s'\n", path);
        strcpy(full_path, path);
	}
	_Trace( 4, "Info: _fullpath result '%s'->'%s'\n", path, full_path);
	if (dup_path)
		free(dup_path);
	if (subfolder)
	{
		strcat(full_path, "\\");
		strcat(full_path, subfolder);
	}
	fn = full_path;
	while (fn = strstr(fn, "\\\\"))
	{
		if (fn == full_path)
			fn++;
		else
			strcpy(fn, fn+1);
	}
	len = strlen(full_path);
	if (!len)
		return;
	if (!full_path_fn)
		return;
	if (directory)
	{
		fn = full_path + strlen(full_path);
		if (fn != full_path && fn[-1] != '\\')
		{
			*fn++ = '\\';
			*fn = 0;
		}
	}
	else
	{
		fn = strrchr(full_path, '\\');
		if (fn)
			fn++;
		else
			fn = full_path;
		if (!*fn)
		{
			*fn++ = '.';
			*fn = 0;
		}
	}
	*full_path_fn = fn;
}

bool i_mkdir(char* path, char* file_part)
{
	bool res = false;
	int err;
	char* slash = path;

	if (file_part)
	{
		char c = *file_part;
		*file_part = 0;
		res = i_mkdir(path, NULL);
		*file_part = c;
		return res;
	}


	err = _mkdir(path);
	if (err==0 || (err==-1 && errno==EEXIST))
		return true;
	while (slash = strchr(slash, '\\'))
	{
		*slash = 0;
		err = _mkdir(path);
		if (err==0 || (err==-1 && errno==EEXIST))
			res = true;
		else
			res = false;
		*slash = '\\';
		slash++;
	}
	return res;
}

static size_t _getfsize(char* pathname)
{
	FILE* file;
	size_t bytes = 0;
	file = fopen(pathname, "rb");
	if (NULL == file)
		return 0;
	bytes = GetFSize(file);
	fclose(file);
	return bytes;
}

static bool _exist(char* pathname)
{
	return (0 == _access(pathname, 0));
}

static bool _delete(char* pathname, char* trace_prefix, bool silent)
{
	int Result;
    Result = _unlink( pathname );
	if (!Result)
	{
		if (!silent)
			_Trace(0, "%s%sFile '%s' has been deleted\n", trace_prefix ? trace_prefix : "", trace_prefix ? ": " : "", pathname);
		return true;
	}
	if (Result != ENOENT && !silent)
		_Trace(0, "%s%sCannot delete file '%s', %s\n", trace_prefix ? trace_prefix : "", trace_prefix ? ": " : "", pathname, strerror(errno));
	return false;
}

static bool get_cache_info(char* cache_path, void* info, size_t size)
{
	FILE* file;
	size_t bytes = 0;
	if (info)
		memset(info, 0, size);
	else
		size = 0;
	file = fopen(cache_path, "rb");
	if (NULL == file)
		return false;
	if (info)
		bytes = fread(info, 1, size, file);
	fclose(file);
	return (bytes == size);
}

static bool update_cache_info(char* cache_path, void* info, size_t size)
{
	FILE* file;
	file = fopen(cache_path, "wb");
	if (NULL == file)
		return false;
	if (info)
		fwrite(info, 1, size, file);
	fclose(file);
	return true;
}


static int num_from_ascii(char* s, size_t size)
{
    size_t n=0, i=0;
    char c;
    for (; i<size; i++)
    {
        c = s[i];
        if (c < '0' || c > '9')
            return 0;
        c -= '0';
        n *= 10;
        n += c;
    }
    return n;
}

void time2tm(time_t _t, struct tm* ptm)
{
	unsigned long time = (unsigned long)_t;
	memset(ptm, 0, sizeof(struct tm));
	ptm->tm_sec = time % 60;
	time /= 60;
	ptm->tm_min = time % 60;
	time /= 60;
	ptm->tm_hour = time % 24;
	time /= 24;
	ptm->tm_mday = time % 31;
	time /= 31;
	ptm->tm_mon = time % 12;
	time /= 12;
	ptm->tm_year = time;
}

time_t tm2time(struct tm* ptm)
{
	unsigned long time = 0;
	time =  (unsigned long)ptm->tm_year * 60 * 60 * 24 * 31 * 12;
	time += (unsigned long)ptm->tm_mon  * 60 * 60 * 24 * 31;
	time += (unsigned long)ptm->tm_mday * 60 * 60 * 24;
	time += (unsigned long)ptm->tm_hour * 60 * 60;
	time += (unsigned long)ptm->tm_min  * 60;
	time += (unsigned long)ptm->tm_sec;
	return (time_t)time;
}

time_t TimeFromFileName(char* fname)
{
    time_t _t;
    struct tm t;
    int l;
    if (!fname)
        return 0;
    l = strlen(fname);
    if (l < 13) // 8 chars full date + delimiter + 4-6 chars for time
        return 0;
    if (fname[8] != '.') // date/time delimiter
        return 0;
    memset(&t, 0, sizeof(t));
	t.tm_year = num_from_ascii(fname+0, 4) - 1900;
	t.tm_mon  = num_from_ascii(fname+4, 2) - 1;
	t.tm_mday = num_from_ascii(fname+6, 2);
	t.tm_hour = num_from_ascii(fname+9, 2);
	t.tm_min  = num_from_ascii(fname+11, 2);
	t.tm_sec  = num_from_ascii(fname+13, 2);
	_t = tm2time(&t);
    if (_t == (time_t)-1)
        return 0;
    return _t;
}

int FileNameFromTime(time_t _t, char* fname, size_t fname_buff_size)
{
    struct tm _tm;
    int l = 0;
    if (!fname || !fname_buff_size)
        return 0;
    fname[0] = 0;
    if (fname_buff_size < 14)
        return 0;
	if (_t == 0)
	{
		strcpy(fname, "????????.??????");
		return true;
	}
    //pt = localtime(&_t);
	time2tm(_t, &_tm);
    _itoa(_tm.tm_year+1900, fname, 10);
    fname[4]  = '0' + ((_tm.tm_mon+1) / 10);
    fname[5]  = '0' + ((_tm.tm_mon+1) % 10);
    fname[6]  = '0' + (_tm.tm_mday / 10);
    fname[7]  = '0' + (_tm.tm_mday % 10);
    fname[8]  = '.';
    fname[9]  = '0' + (_tm.tm_hour / 10);
    fname[10] = '0' + (_tm.tm_hour % 10);
    fname[11] = '0' + (_tm.tm_min  / 10);
    fname[12] = '0' + (_tm.tm_min  % 10);
    fname[13] = '0' + (_tm.tm_sec  / 10);
    fname[14] = '0' + (_tm.tm_sec  % 10);
    fname[15] = 0; 
    return 13;
}

bool GetFileNameFromTimeAndHash(time_t _t, unsigned char hash[], char* fname, size_t fname_buff_size)
{
	if (fname_buff_size < 16 + 32 + 1)
	{
		_Trace( 1, "Error: GetFileNameFromTimeAndHash: buffer too small\n");
		return false;
	}
	if (!FileNameFromTime(_t, fname, fname_buff_size))
		return false;
	fname[15] = '.';
	if (!GetHashAsText(hash, fname+16, fname_buff_size-16))
		return false;
	return true;
}

static bool CleanupHistory(time_t _t, unsigned char hash[])
{
	//char Sign[SIGN_SIZE];
	if (!GetFileNameFromTimeAndHash(_t, hash, hist_fn, _MAX_PATH))
		return false;
	//if (GetFileSignatureFixed(hist_path, Sign, SIGN_SIZE))
	//{
	//	strcpy(sign_cache_fn, Sign);
	//	_delete(sign_cache_path, "History cleanup", false);
	//}
	return _delete(hist_path, "History cleanup", false);
}

static bool CleanupCache(void)
{
	long hFind;
    struct _finddata_t fd;
	strcpy(cache_fn, "*.*");
	hFind =_findfirst(cache_path, &fd);
	if (hFind == -1) 
		return true;
	do
	{
		DIFF_CACHE_DATA dci;
		char* ptr;
		long hFind2;
		if (fd.attrib & _A_SUBDIR)
			continue;
		strcpy(res_fn, fd.name);
		if (_exist(res_path))
			continue;
		strcpy(cache_fn, fd.name);
		if (!get_cache_info(cache_path, &dci, sizeof(dci)))
		{
			_Trace( 1, "Error: internal CleanupCache(cache) failed\n");
			continue;
		}
		strcpy(hist_sub, fd.name);
		ptr = strrchr(hist_sub, '.');
		if (!ptr)
		{
			_Trace( 1, "Error: internal CleanupCache(ext) failed\n");
			continue;
		}
		*ptr = '\\';
		hist_fn = ptr+1;

		// find history copy
		hist_fn[0] = '*';
		hist_fn[1] = '.';
		if (!GetHashAsText(dci.hash_from, hist_fn+2, _MAX_PATH))
		{
			_Trace( 1, "Error: internal CleanupCache(hash) failed\n");
			continue;
		}
		hFind2 =_findfirst(hist_path, &fd);
		if (hFind2 == -1) 
			_delete(cache_path, "Cache cleanup", false);
		else
			_findclose(hFind2);
	} while (_findnext(hFind, &fd) == 0);
	_findclose(hFind);
	return true;
}

static bool CleanupSignCache(void)
{
	long hFind;
	struct _finddata_t fd;
	strcpy(sign_cache_fn, "*.*");
	hFind =_findfirst(sign_cache_path, &fd);
	if (hFind == -1) 
		return true;
	do
	{
		if (fd.attrib & _A_SUBDIR)
			continue;
		strcpy(src_fn, fd.name);
		if (_exist(src_path))
			continue;
		strcpy(sign_cache_fn, fd.name);
		_delete(sign_cache_path, "SignCache cleanup", false);
	} while (_findnext(hFind, &fd) == 0);
	_findclose(hFind);
	return true;
}
static bool DelOldResult(void)
{
	bool res;
	strcpy(cache_fn, res_fn);
	_delete(cache_path, "DelOldResult", true);
	res = _delete(res_path, "DelOldResult", true);
	return res;
}

static bool GetFileSignature(char* filename, unsigned char* pSignature, size_t nSignSize)
{
	FILE* file;
	size_t FLen;
	unsigned int i;
	
	if (nSignSize < SIGN_SIZE)
		return false;

	/* open file */
	file = fopen(filename, "rb");
    if ( NULL == file )
		return false;
	
//	char sign[5];
//	fseek(file, 0, SEEK_SET);
//	fread(sign, 1, 4, file);
//	if (memcmp(sign, "DIFF", 4) == 0)
//		return GetSignatureFromDIFF(file, pSignature);
//	if (memcmp(sign, "BDIF", 4) == 0)
//		return GetSignatureFromBDIF(file, pSignature);
    
	// default - read from end of file
	FLen = GetFSize(file);

    /* read digital sign */
    if ( FLen < SIGN_SIZE )
	{
		fclose(file);
		return false;
	}

	fseek(file, (FLen-SIGN_SIZE), SEEK_SET);
	if ( 0 == fread(pSignature, SIGN_SIZE, 1, file) )
	{
		fclose(file);
		return false;
	}
	fclose(file);
	
	/* generic check for sign */
	if ( pSignature[0] != 0x0D || pSignature[1] != 0x0A )
		return false; /* not signed */
	if ( pSignature[2] != ';'  || pSignature[3] != 0x20 )
		return false; /* not signed */
	if ( pSignature[SIGN_SIZE-2] != 0xAD || pSignature[SIGN_SIZE-1] != 0xAD )
		return false; /* not signed */
	for ( i = 4; i < (SIGN_SIZE-2); i++ )
	{
		char SB = pSignature[i];
		bool Good = cFALSE;
		
		if (( SB >= '0' && SB <= '9' )
			|| ( SB >= 'a' && SB <= 'z' )
			|| ( SB >= 'A' && SB <= 'Z' )
			|| ( SB == '+' || SB == '/' ))
			; // good
		else
			return false; /* invalid char in sign */
	}
	return true;
}

static bool GetFileSignatureFixed(char* filename, unsigned char* pSignature, size_t nSignSize)
{
	unsigned char Sign[SIGN_SIZE];
	char* p;
	if (nSignSize < SIGN_SIZE - 6 + 1)
		return false;
	if (!GetFileSignature(filename, Sign, SIGN_SIZE))
		return false;
	Sign[SIGN_SIZE-2] = 0; // drop 'нн'
	strcpy(pSignature, Sign+4);
	// signature is base64 encoded, so fix char restricted on file system '/' & '+'
	while (p = strchr(pSignature, '/'))
		*p = '-';
	while (p = strchr(pSignature, '+'))
		*p = '_';
	return true;
}

// ----------------------------------------------------------------------------

static bool GetFileHash(char* filename, char* filepart, unsigned char* HashBuff, size_t buff_len)
{
	FILE*  BF;
	tMD5_CTX MD5;
	unsigned char* FMem;
	size_t  FLen;
	tBOOL  BRes;
	unsigned char  Sign[SIGN_SIZE];
	bool  bHasSign;
	SRC_CACHE_DATA scd;

	/* check params */
	if ( NULL == filename || NULL == HashBuff || buff_len != HASH_SIZE )
		return false;

	bHasSign = GetFileSignature(filename, Sign, sizeof(Sign));
	if (bHasSign)
	{
		strcpy(sign_cache_fn, filepart);
		if (get_cache_info(sign_cache_path, &scd, sizeof(scd)))
		{
			if (scd.Magic == SIGN_CACHE_MAGIC && 0 == memcmp(scd.sign, Sign, SIGN_SIZE))
			{
				memcpy(HashBuff, scd.hash, HASH_SIZE);
				return true;
			}
		}
	}

	/* open file */
	BF = fopen(filename, "rb");
    if ( NULL == BF )
		return false;

    FLen = GetFSize(BF);
	
	/* alloc mem */
	if ( NULL == (FMem = (unsigned char*)malloc(FLen+16)) )
	{
		fclose(BF);
		return false;
	}
	
	/* read file */
	fseek(BF, 0, SEEK_SET);
	BRes = (FLen == fread(FMem, 1, FLen, BF));
	fclose(BF);
	
	/* check result */
	if ( !BRes )
	{
		free(FMem);
		return false;
	}
	
	/* calc md5 */
	MD5Init(&MD5);
	MD5Update(&MD5, FMem, FLen);
	MD5Final(&MD5, HashBuff);
	free(FMem);
	
	if (bHasSign)
	{
		scd.Magic = SIGN_CACHE_MAGIC;
		memcpy(scd.hash, HashBuff, HASH_SIZE);
		memcpy(scd.sign, Sign, SIGN_SIZE);
		// sign_cache_path already has valid filename
		update_cache_info(sign_cache_path, &scd, sizeof(scd));
	}
	return true;
}

static bool GenDiffNameByHash(char* filename, unsigned char Hash[HASH_SIZE], char* buffer, size_t buff_len)
{
	unsigned char  B64[8];
	char* diff_ext;
	size_t Wrt;
	size_t len, i;
	
	if ( NULL == filename || NULL == buffer || buff_len < 4 )
		return false;
	
	len = strlen(filename);
	diff_ext = filename + len;
	while( diff_ext > filename && diff_ext[0] != '\\' )
		diff_ext--;
	if ( diff_ext[0] == '\\' )
		diff_ext++;
	
	/* check buff */
	len = strlen(diff_ext);
	if ( buff_len < len + 4 + 1 )
		return false;
	
	/* prebuild name */
	memcpy(buffer, diff_ext, len);
	buffer[len++] = '.';
	
	/* convert to base64 */
	InitBase64P();
	memset(&B64[0], 0, sizeof(B64));
	EncodeBase64(&Hash[0], 3, &B64[0], sizeof(B64), &Wrt, 0);
	if ( Wrt < 3 ) 
		return false;
	
	/* recreate name */
	memcpy(&buffer[len], &B64[0], 3);
	buffer[len+3] = '\0';
	for (i=0; i<3; i++)
	{
		if (buffer[len+i] >= 'A' && buffer[len+i] <= 'Z')
			buffer[len+i] |= 0x20;
	}
	
	return true;
}

// ----------------------------------------------------------------------------

typedef struct file_ver {
	time_t ver;
	unsigned char hash[HASH_SIZE]; 
} tfile_ver;

int __cdecl qsort_file_ver_compare(const void *p1, const void *p2) 
{
    time_t i1, i2;
    i1 = ((tfile_ver*)p1)->ver;
    i2 = ((tfile_ver*)p2)->ver;
    if (i1 > i2)
        return -1;
    else if (i1 < i2)
        return 1;
    return 0;
}


bool GetHashFromText(char* text, unsigned char hash[])
{
	unsigned long* lhash = (unsigned long*)hash;
	long n = strlen(text);
	if (n < 32)
		;//return false;
	else if (4 == sscanf(text, "%08X%08X%08X%08X", &lhash[0], &lhash[1], &lhash[2], &lhash[3]))
		return true;
	_Trace( 1, "Error: GetHashFromText: parameter invalid '%s'\n", text);
	return false;
}

bool GetHashFromFileName(char* fname, unsigned char hash[])
{
	unsigned long* lhash = (unsigned long*)hash;
	long n = strlen(fname);
	if (n != 32+16)
		; //return false;
	else if (fname[15] != '.')
		; //return false;
	else if (4 == sscanf(fname+16, "%08X%08X%08X%08X", &lhash[0], &lhash[1], &lhash[2], &lhash[3]))
		return true;
	_Trace( 1, "Error: GetHashFromFilename: parameter invalid '%s'\n", fname);
	return false;
}

bool GetHashAsText(unsigned char hash[], char* buff, size_t buff_size)
{
	unsigned long* lhash = (unsigned long*)hash;
	if (buff_size < 32 + 1)
	{
		_Trace( 1, "Error: GetHashAsText: buffer too small\n");
		return false;
	}
	sprintf(buff, "%08X%08X%08X%08X", lhash[0], lhash[1], lhash[2], lhash[3]);
	return true;
}

#define LCASE(c) (c>='A'? (c<='Z'? (c|0x20) : c) : c)

bool MatchOkay(char* Pattern)
{
	while (*Pattern == '*')
		Pattern++;
	if (*Pattern)
		return FALSE;
	return TRUE;
}

bool MatchWithPattern(char* String, char* Pattern, bool CaseSensetivity)
{
	char ch;
	char cp;
	if(*Pattern=='*')
	{
		Pattern++;
		while((ch=*String) && (cp=*Pattern))
		{
			if (CaseSensetivity == FALSE)
			{
				ch = LCASE(ch);
				cp = LCASE(cp);
			}
			if ((ch == '*') || (ch == cp) || (cp == '?'))
			{
				if(MatchWithPattern(String+1, Pattern+1, CaseSensetivity)) 
					return TRUE;
			}
			String++;
		}
		return MatchOkay(Pattern);
	} 
	
	while((ch=*String) && ((cp=*Pattern) != '*'))
	{
		if (CaseSensetivity == FALSE)
		{
			ch = LCASE(ch);
			cp = LCASE(cp);
		}
		
		if((cp == ch) || (cp == '?'))
		{
			Pattern++;
			String++;
		} else
			return FALSE;
	}
	
	if(*String)
		return MatchWithPattern(String, Pattern, CaseSensetivity);
	
	return MatchOkay(Pattern);
}

bool copy_file(char* src_path, char* dst_path)
{
	bool res = true;
	const unsigned long cReadSize = 64*1024;
	FILE* src;
	FILE* dst;
	size_t bytes;
	unsigned char* buff;
	
	buff = malloc(cReadSize);
	if (!buff)
	{
		_Trace( 0, "Error: not enought memory\n");
		return false;
	}
	
	src = fopen(src_path, "rb");
	dst = fopen(dst_path, "wb");
	if (!src)
	{
		res = false;
		_Trace( 1, "Error: cannot open file '%s'\n", src_path);
	} 
	if (!dst)
	{
		res = false;
		_Trace( 1, "Error: cannot open file '%s'\n", dst_path);
	} 
	if (res) do 
	{
		bytes = fread(buff, 1, cReadSize, src);
		if (bytes == 0)
			break;
		if (bytes != fwrite(buff, 1, bytes, dst))
		{
			res = false;
			_Trace( 1, "Error: write error to file '%s', %s\n", src_path, strerror(errno));
			break;
		}
	} while (bytes);
	
	if (src) fclose(src);
	if (dst) fclose(dst);
//	if (res)
//	{
//		_Trace( 3, "Info: file copied succesfully '%s' -> '%s'\n", src_path, dst_path);
//	}
//	else
//	{
//		_Trace( 2, "Error: file copy failed '%s' -> '%s', %s\n", src_path, dst_path, strerror(errno));
//	}
	return res;
}

#define MAX_VER 500

char* GetDiffErrorStr(int error)
{
	switch(error)
	{
#define _ERR_STR(e) case e: return #e;
		_ERR_STR(DIFF_ERR_PARAM);
		_ERR_STR(DIFF_ERR_NOMEM);
		_ERR_STR(DIFF_ERR_FILE);
		_ERR_STR(DIFF_ERR_SIZE);
		_ERR_STR(DIFF_ERR_OTHER);
		_ERR_STR(DIFF_ERR_CORRUPT);
		_ERR_STR(DIFF_ERR_VERSION);
		_ERR_STR(DIFF_ERR_AVCPACK);
		_ERR_STR(DIFF_ERR_DATA);
		_ERR_STR(DIFF_ERR_SIGN);
		_ERR_STR(DIFF_ERR_BAD16);
		_ERR_STR(DIFF_ERR_PACK);
#undef _ERR_STR
	}
	return "UNKNOWN ERROR";
}

bool Make1FileDiff(tUINT nLimitSize, tUINT* pnResultSize)
{
	int Result;
	_DiffTrace("Info: %s converting: %s + %s => %s\n", res_fn, hist_path, src_path, res_path);
	Result = MakeDiff(hist_path, src_path, res_path, DIFF_FLG_TBLPACK_LZMA, nLimitSize, pnResultSize); 
	if ( DIFF_ERR_OK != Result )
	{
		_DiffTrace("Error: converting failed with %s (%d)\n", GetDiffErrorStr(Result), Result);
		return false; // error here 
	}
	else
	{
		_DiffTrace("Info: %s, size=%d\n", res_fn, *pnResultSize);
	}
	return true;
}


void EnumHistoryFiles(void)
{
	int Result;
	unsigned char hash[HASH_SIZE];
	unsigned char src_hash[HASH_SIZE];
	tfile_ver ver_arr[MAX_VER];
    struct _finddata_t fd;
    long hFind;
	unsigned int i, j, processed = 0;
	time_t src_ver = 0;
    unsigned int ver_count = 0;
	long klz_size = 0;
	int new_file = TRUE;
	
	_Trace( 2, "Info: processing file '%s'\n", src_fn);

	*hist_sub = 0;
	make_fullpath(hist_path, &hist_fn, hist_path, src_fn, true);
    i_mkdir(hist_path, NULL);
	_Trace( 3, "Info: history folder '%s'\n", hist_path);
	
	if (!GetFileHash(src_path, src_fn, &src_hash[0], HASH_SIZE))
	{
		_Trace( 1, "Error: cannot get hash for %s\n", src_path);
		return;
	}
	
	// add new source file into history
	if (g_rebuild_versions == 0 && g_gen_klz)
	{
		// klz gen only - don't use history
	}
	else
	{
		// find history copy
		hist_fn[0] = '*';
		hist_fn[1] = '.';
		if (!GetHashAsText(src_hash, hist_fn+2, _MAX_PATH))
		{
			_Trace( 1, "Error: internal GetHashAsText failed\n");
			return;
		}
		hFind =_findfirst(hist_path, &fd);
		if (hFind != -1) 
		{
			_Trace( 3, "Info: file already in history as %s\n", fd.name);
			src_ver = TimeFromFileName(fd.name);
			new_file = FALSE;
			_findclose(hFind);
		}
		
		if (!src_ver)
		{
			// need to history
			src_ver = time(0);
			if (!GetFileNameFromTimeAndHash(src_ver, src_hash, hist_fn, _MAX_PATH))
			{
				_Trace( 1, "Error: internal GetFileNameFromTimeAndHash failed\n");
				return;
			}
			if (!copy_file(src_path, hist_path))
			{
				_Trace( 0, "Error: cannot copy file to history '%s'->'%s'\n", src_path, hist_path);
			}
			else
			{
				_Trace( 1, "Info: file copied to history '%s'->'%s'\n", src_path, hist_path);
			}
		}
	}
	
	// collect suitable history copies
	if (g_rebuild_versions)
	{
		strcpy(hist_fn, "*.*");
		hFind =_findfirst(hist_path, &fd);
		if (hFind != -1)     
		{
			do
			{
				time_t ver;
				if (fd.attrib & _A_SUBDIR)
					continue;
				strcpy(hist_fn, fd.name);
				ver = TimeFromFileName(fd.name);
				if (!ver)
				{
					_Trace( 2, "Warning: cannot retrive timestamp from history copy '%s', history copy ignored\n", fd.name);
					continue;
				}
				if (ver == src_ver)
					continue; // just created history copy 
				if (ver > src_ver)
				{
					_Trace( 3, "Info: history copy '%s' newer then source file '%s', history copy ignored\n", fd.name, src_fn);
					continue;
				}
				if (!GetHashFromFileName(fd.name, hash))
				{
					_Trace( 2, "Warning: cannot retrive hash from history copy filename '%s', history copy ignored\n", fd.name);
					continue;
				}
				if (ver_count < MAX_VER)
				{
					ver_arr[ver_count].ver = ver;
					memcpy(ver_arr[ver_count].hash, hash, HASH_SIZE);
					ver_count++;
				}
				else
				{
					int i;
					time_t min_ver = 0;
					for (i=1; i<MAX_VER; i++)
					{
						if (ver_arr[i].ver < ver_arr[min_ver].ver) 
							min_ver = i;
					}
					ver_arr[min_ver].ver = ver;
					memcpy(ver_arr[min_ver].hash, hash, HASH_SIZE);
				}
			} while (_findnext(hFind, &fd) == 0);
			_findclose(hFind);
		}
		
		qsort(ver_arr,ver_count,sizeof(ver_arr[0]), qsort_file_ver_compare);
		if (g_max_history_days || g_max_history_count)
		{
			time_t curr_t = time(0);
			for (i=0; i<ver_count; i++)
			{
				if (g_max_history_count && i >= g_max_history_count)
				{
					_Trace( 2, "Info: reached limit by history copies count %d, now %d\n", g_max_history_count, ver_count);
					break;
				}
				if (g_max_history_days && (curr_t - ver_arr[i].ver) > 0 && (unsigned)(curr_t - ver_arr[i].ver)/(60*60*24) >= g_max_history_days)
				{
					_Trace( 2, "Info: reached limit by history days %d\n", g_max_history_days);
					break;
				}
			}
			if (i < ver_count)
			{
				// delete old versions
				for (j=i; j<ver_count; j++)
				{
					CleanupHistory(ver_arr[j].ver, ver_arr[j].hash);
				}	
				ver_count = i;
			}
		}
		_Trace( 1, "Info: %s, found %d versions\n", src_fn, ver_count);
		if ( ver_count > g_rebuild_versions )
		{
			ver_count = g_rebuild_versions;
			_Trace( 1, "Info: %s, build only %d versions (-build=<n> parameter in use)\n", src_fn, ver_count);
		}
		if (ver_count == 0)
			_Trace( 1, "Info: %s, no history copies found for\n", src_fn);

	}

	// generate packed .KLZ
	if (g_gen_klz) 
	{
		bool bCached = false;
		unsigned char klz_hash[HASH_SIZE];
		strcpy(res_fn, src_fn);
		strcat(res_fn, ".klz");

		// check cache
		strcpy(cache_fn, res_fn);
		if (get_cache_info(cache_path, klz_hash, HASH_SIZE))
		{
			if (0 == memcmp(klz_hash, src_hash, HASH_SIZE))
			{
				bCached = true;
				klz_size = _getfsize(res_path);
				_Trace( 2, "Info: %s, already created, size=%d, skipped\n", res_fn, klz_size);
			}
		}
		if (!bCached)
		{
			// delete old version (if any)
			_delete(res_path, "DelOldResult", true);
			_Trace( 0, "Info: %s, packing: %s -> %s\n", res_fn, src_path, res_path);
			Result = GenKLZ(src_path, src_fn, res_path, res_fn);
			if (DIFF_ERR_OK == Result)
			{
				// cache_path already contain valid name
				update_cache_info(cache_path, src_hash, HASH_SIZE);
				klz_size = _getfsize(res_path);
				_Trace( 0, "Info: %s, size=%d\n", res_fn, klz_size);
				if (diff_list_file)
					fprintf(diff_list_file, "%s\n", res_fn);
			}
		}
	}

	// make chain terminator
	if (g_rebuild_versions && new_file)
	{
		DIFF_CACHE_DATA dci;
		tDIFF_TERM_HDR dift;
		memset(&dift, 0, sizeof(dift));
		memcpy(dift.Sign, DIFT_SIGNATURE, 4);
#if (16 != HASH_SIZE)
#error sizeof(dift.Hash) != HASH_SIZE
#endif
		memcpy(dift.Hash, src_hash, HASH_SIZE);
		GenDiffNameByHash(src_fn, src_hash, res_fn, _MAX_PATH);
		if (!update_cache_info(res_path, &dift, sizeof(dift)))
		{
			_Trace( 0, "Error: cannot create chain terminator '%s'\n", res_path);
		}
		memset(&dci, 0, sizeof(dci));
		memcpy(dci.hash_from, src_hash, sizeof(src_hash));
		memcpy(dci.hash_to, src_hash, sizeof(src_hash));
		dci.diff_size = sizeof(dift);
		strcpy(cache_fn, res_fn);
		if (!update_cache_info(cache_path, &dci, sizeof(dci)))
		{
			_Trace( 0, "Error: cannot update cache '%s'\n", cache_fn);
		}
		if (diff_list_file)
			fprintf(diff_list_file, "%s\n", res_fn);
	}
		
	// for each suitable history copy
	for (i=0; i<ver_count; i++)
	{
		DIFF_CACHE_DATA dci;
		tUINT nLimitSize = 0;
		tUINT nResultSize = 0;
		if (g_time_limit && g_time_limit < time(0))
		{
			_Trace( 0, "Error: Time limit exceeded, stopping...\n");
			break;
		}
		GenDiffNameByHash(src_fn, ver_arr[i].hash, res_fn, _MAX_PATH);
		// check cache
		strcpy(cache_fn, res_fn);
		if (g_force)
		{
			_Trace( 3, "Info: force mode - ignore chache\n");
		}
		else if (get_cache_info(cache_path, &dci, sizeof(dci)))
		{
			if (0 == memcmp(&dci.hash_to, &src_hash, HASH_SIZE))
			{
				// todo: verify 
				bool bValid = true;
				if (g_verify_result)
				{
					if (_getfsize(res_path) != dci.diff_size)
						bValid = false;
				}
				if (bValid)
				{
					_Trace( 2, "Info: %s, already created, size=%d, skipped\n", res_fn, dci.diff_size);
					continue;
				}
			}
		}
		GetFileNameFromTimeAndHash(ver_arr[i].ver, ver_arr[i].hash, hist_fn, _MAX_PATH);
		// delete old version
		_delete(res_path, "DelOldResult", true);
		_OpenDiffTrace(res_fn);
		if (klz_size)
			nLimitSize = klz_size + g_tfc_size;
		else
			nLimitSize = _getfsize(src_path) + g_tfc_size;
		if (Make1FileDiff(nLimitSize, &nResultSize))
		{
			// success
			if (diff_list_file && i<diff_list_count && (unsigned)src_ver - (unsigned)ver_arr[i].ver < diff_list_minutes)
				fprintf(diff_list_file, "%s\n", res_fn);
		}
		// always update cache info - we cache both succ and fail to avoid repeatable re-generate tries on fail 
		memcpy(&dci.hash_from, &ver_arr[i].hash, HASH_SIZE);
		memcpy(&dci.hash_to, src_hash, HASH_SIZE);
		dci.diff_size = nResultSize;
		update_cache_info(cache_path, &dci, sizeof(dci));
		_CloseDiffTrace();
	}

	return;
} 

static bool IsValidResult(void)
{
	char* ext;
	char sign[4];
	unsigned char src_hash[HASH_SIZE];
	size_t res_size;
	size_t klz_size;

	_DiffTrace("Validating: ");
	strcpy(src_fn, res_fn);
	ext = strrchr(src_fn, '.');
	if (!ext)
	{
		_DiffTrace2("%s # FAILED: no diff extension\n", res_fn);
		return false;
	}
	
	// get KLZ size
	strcpy(ext, ".klz");
	klz_size = _getfsize(src_path);

	*ext = 0; // truncate diff ext
	ext++;
	if (!_exist(src_path))
	{
		_DiffTrace2("%s # FAILED: source not found '%s'\n", res_fn, src_path);
		return false;
	}
	if (!GetFileHash(src_path, src_fn, &src_hash[0], HASH_SIZE))
	{
		_DiffTrace2("%s # FAILED: GetFileHash(%s)\n", res_fn, src_path);
		return false;
	}
	if (!get_cache_info(res_path, sign, 4))
	{
		_DiffTrace2("%s # FAILED: cannot read\n", res_fn);
		return false;
	}
	if (0 == memcmp(sign, "KLZF", 4))
	{
		unsigned char klz_hash[HASH_SIZE];
		strcpy(cache_fn, res_fn);
		res_size = _getfsize(res_path);
		_DiffTrace2("KLZF %s(%d)", res_fn, res_size);
		if (!get_cache_info(cache_path, klz_hash, HASH_SIZE))
		{
			_DiffTrace2(" # FAILED: no cache info\n");
			return false;
		}
		if (0 != memcmp(src_hash, klz_hash, HASH_SIZE))
		{
			_DiffTrace2(" # FAILED: wrong cache info\n");
			return false;
		}
		_DiffTrace2(" # OK\n");
		return true;
	}
	if (0 == memcmp(sign, "DIFF", 4))
	{
		DIFF_CACHE_DATA dci;
		size_t src_size = _getfsize(src_path);
		size_t diff_size = 0;
		_DiffTrace2("DIFF ");
		for(;;)
		{
			strcpy(cache_fn, res_fn);
			if (diff_size)
				_DiffTrace2(" -> ");
			res_size = _getfsize(res_path);
			diff_size += res_size;
			_DiffTrace2("%s(%d)", res_fn, res_size);
			if (!get_cache_info(cache_path, &dci, sizeof(dci)))
			{
				_DiffTrace2(" # FAILED: no cache info\n");
				return false;
			}
			if (diff_size!=res_size && g_tfc_size)
			{
				diff_size += g_tfc_size; // add approximate transport overhit
				_DiffTrace2("+(tfc:%d)", g_tfc_size);
			}
			if (diff_size > src_size)
			{
				_DiffTrace2(" # SIZE: exhaused source size (%d>%d)\n", diff_size, src_size);
				return false;
			}
			if (klz_size && diff_size > klz_size)
			{
				_DiffTrace2(" # SIZE: exhaused KLZ size (%d>%d)\n", diff_size, klz_size);
				return false;
			}
			if (0 == memcmp(src_hash, dci.hash_to, HASH_SIZE))
			{
				_DiffTrace2(" # OK");
				if (diff_size != res_size)
					_DiffTrace2(", total size %d", diff_size);
				_DiffTrace2("\n");
				return true;
			}
			// next iteration
			if (!GenDiffNameByHash(src_fn, dci.hash_to, res_fn, _MAX_PATH))
			{
				_DiffTrace2(" # FAILED: GenDiffNameByHash()\n");
				return false;
			}
			if (!_exist(res_path))
			{
				_DiffTrace2(" # FAILED: Cannot find next file '%s', broken diff sequense?\n", res_path);
				return false;	
			}
		}
	}
	if (0 == memcmp(sign, DIFT_SIGNATURE, 4))
	{
		tDIFF_TERM_HDR dterm;
		DIFF_CACHE_DATA dci;
		unsigned long* lhash;
		_DiffTrace2(DIFT_SIGNATURE " %s ", res_fn);
		strcpy(cache_fn, res_fn);
		if (!get_cache_info(cache_path, &dci, sizeof(dci)))
		{
			_DiffTrace2("# FAILED: cannot read cache %s\n", cache_fn);
			return false;
		}
		if (0 != memcmp(dci.hash_to, dci.hash_from, sizeof(dci.hash_to)))
		{
			_DiffTrace2("# FAILED: cache say different from/to hashes\n");
			return false;
		}
		if (!get_cache_info(res_path, &dterm, sizeof(dterm)))
		{
			_DiffTrace2("# FAILED: cannot read file\n", res_fn);
			return false;
		}
		if (0 != memcmp(dci.hash_to, dterm.Hash, sizeof(dci.hash_to)))
		{
			_DiffTrace2("# FAILED: different cache/DIFT hashes\n");
			return false;
		}
		lhash = (unsigned long*)dterm.Hash;
		_DiffTrace2(" # OK, hash=%08X%08X%08X%08X\n", lhash[0], lhash[1], lhash[2], lhash[3]);
		return true;
	}
	// unknown format
	_DiffTrace2("%s # FAILED: unknown format\n", res_fn);
	return false;
}

void VerifyResult(void)
{
	struct _finddata_t fd;
    long hFind;

	strcpy(res_fn, "*.*");
    hFind =_findfirst(res_path, &fd);
    if (hFind == -1) 
    {
        _Trace( 2, "Info: Verify: There are no files in result '%s'\n", res_path);
        return;
    }
    do
    {
		if (fd.attrib & _A_SUBDIR)
			continue;
        strcpy(res_fn, fd.name);
		_OpenDiffTrace(res_fn);
		if (IsValidResult())
		{
			_CloseDiffTrace();
			continue;
		}
		strcpy(cache_fn, fd.name);
		_delete(cache_path, "Validate", true);
		strcpy(res_fn, fd.name);
		if (_delete(res_path, "Validate", true))
			_DiffTrace("Validating: DELETED %s\n", fd.name);
		_CloseDiffTrace();
    } while (_findnext(hFind, &fd) == 0);
    _findclose(hFind);
	return;
}


int EnumSourceFiles(void)
{
    struct _finddata_t fd;
    long hFind;
	unsigned int i;
	unsigned int nFilesFound = 0;
	
	make_fullpath(cache_path, &cache_fn, hist_path, "~cache", true);
	make_fullpath(sign_cache_path, &sign_cache_fn, cache_path, "sign", true);

    if (!i_mkdir(hist_path, NULL))
		return ENOTDIR;
	if (!i_mkdir(cache_path, NULL))
		return ENOTDIR;
	if (!i_mkdir(sign_cache_path, NULL))
		return ENOTDIR;
    if (*diff_log_path && !i_mkdir(diff_log_path, NULL))
		return ENOTDIR;
    if (!i_mkdir(res_path, NULL))
		return ENOTDIR;

	_Trace( 0, "Info: processing source  '%s'\n", src_path);
	_Trace( 3, "Info: history root  '%s'\n", hist_path);
	_Trace( 3, "Info: result folder '%s'\n", res_path);

	if (g_verify_result)
		CleanupCache();
	
    hFind =_findfirst(src_path, &fd);
    if (hFind == -1) 
    {
        _Trace( 0, "Warning: File(s) not found '%s'\n", src_path);
        return 0;// ENOENT; // 05.07.2006 SSB: Майк, доработай, плиз, сабж таким образом, чтобы он возвращал 0, если ни одного файла в каталоге не нашел.
    }
    do
    {
		bool skip = false;
		if (fd.attrib & _A_SUBDIR)
			continue;
		if (g_time_limit && g_time_limit < time(0))
		{
			_Trace( 0, "Error: Time limit exceeded, stopping...\n");
			break;
		}
        strcpy(src_fn, fd.name);
		for (i=0; i<g_excludes_count; i++)
		{
			if (MatchWithPattern(src_fn, g_excludes[i], false))
			{
				_Trace( 2, "Info: '%s' excluded by '%s'\n", src_path, g_excludes[i]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			nFilesFound++;
			EnumHistoryFiles();
			*hist_fn = 0;
			*res_fn = 0;
		}
    } while (_findnext(hFind, &fd) == 0);
    _findclose(hFind);

	if (g_verify_result)
	{
		VerifyResult();
		CleanupCache();
		CleanupSignCache();
	}
	// if (0 == nFilesFound) // 05.07.2006 SSB: Майк, доработай, плиз, сабж таким образом, чтобы он возвращал 0, если ни одного файла в каталоге не нашел.
	// 	return ENOENT;
    return 0;
}

char* get_my_name(int argc, char** argv, char* default_name)
{
	char* pname;
	
	if ( 0 == argc || NULL == argv || NULL == argv[0] )
		return default_name;

	pname = argv[0] + strlen(argv[0]);
	while ( pname > argv[0] )
	{
		if ( *pname == '\\' || *pname == '/' )
		{
			pname++;
			break;
		}
		
		pname--;
	}
	return pname;
}

void PrintUsage(int argc, char** argv)
{

	_Trace2( 0, "Diff creator for KAV bases "_VERSION_"\n");	
	_Trace2( 0, "Usage: %s <new_files_mask> <history_path> <diff_result_path> [params]\n", get_my_name(argc, argv, "makediff.exe"));
	_Trace2( 0, "Params: -build=<n>         : build <n> differences for each file (default: all possible)\n");
	_Trace2( 0, "        -days=<n>          : limit per-file history to <n> days (default: unlimited)\n");
	_Trace2( 0, "        -versions=<n>      : limit per-file history to <n> versions (default: unlimited)\n");
	_Trace2( 0, "        -verbose[=<n>]     : set information level <n> (default=0: only critical messages)\n");
	_Trace2( 0, "        -exclude=<mask>    : exclude source files by mask\n");
	_Trace2( 0, "        -log[=<path[name]> : output to log file (default: output to console)\n");
	_Trace2( 0, "        -difflog=<path>    : maintain additional per-diff log files\n");
	_Trace2( 0, "        -validate=<on|off> : perform result and chains verification (default: off)\n");
	_Trace2( 0, "        -traffic=<n>       : additional traffic overhit size used in chains verification (default: 1024)\n");
	_Trace2( 0, "        -klz               : pack source file to .KLZ\n");
	_Trace2( 0, "        -time=<n>          : time limit in seconds (default: no limit)\n");
	_Trace2( 0, "        -list=<filename>   : file to write changed diff list\n");
	_Trace2( 0, "        -listcount=<n>     : number of diffs per file to save in changed diff list (default: all)\n");
	_Trace2( 0, "        -listminutes=<n>   : time interval to save in changed diff list (default: all)\n");
	_Trace2( 0, "        -force             : force diffs re-build (don't use cache info)\n");
#ifdef _WIN32
	_Trace2( 0, "        -priority=(idle|normal|high|realtime) : set process priority (default: normal)\n");
#endif
}

int __cdecl main(int argc, char** argv)
{
	int i;
	int err = 0;
	// parse parameters
	for (i=1; i<argc; i++)
	{
		if (argv[i][0] == '-' || argv[i][0] == '/')
		{
			char *p = argv[i]+1, *p1, *p2;
			p1 = strchr(p, ':');
			p2 = strchr(p, '=');
			if (!p1) p1 = p2;
			if (p2 && p2 < p1) p1 = p2;
			if (p1) *p1++ = 0;
			
			if (p1 && !_stricmp(p, "time"))
				g_time_limit = time(0) + strtoul(p1, NULL, 10);
			else if (p1 && !_stricmp(p, "build"))
				g_rebuild_versions = strtoul(p1, NULL, 10);
			else if (p1 && !_stricmp(p, "days"))
				g_max_history_days = strtoul(p1, NULL, 10);
			else if (p1 && !_stricmp(p, "versions"))
				g_max_history_count = strtoul(p1, NULL, 10);
			else if ((!_stricmp(p, "log") || !_stricmp(p, "report")))
				make_fullpath(log_path, &log_fn, p1 ? p1 : ".", 0, false);
			else if (p1 && !_stricmp(p, "difflog"))
				make_fullpath(diff_log_path, &diff_log_fn, p1, 0, true);
			else if (p1 && !_stricmp(p, "list"))
			{
				make_fullpath(diff_list_path, &diff_list_fn, p1, 0, false);
				if (!diff_list_count)
					diff_list_count = ~0;
				if (!diff_list_minutes)
					diff_list_minutes = ~0;
			}
			else if (p1 && !_stricmp(p, "listcount"))
				diff_list_count = strtoul(p1, NULL, 10);
			else if (p1 && !_stricmp(p, "listminutes"))
				diff_list_minutes = strtoul(p1, NULL, 10);
			else if (!_stricmp(p, "verbose"))
				g_trace_level = (p1 ? strtoul(p1, NULL, 10) : 2);
			else if (!_stricmp(p, "klz"))
				g_gen_klz = true;
			else if (!_stricmp(p, "force"))
				g_force = true;
			else if (p1 && !_stricmp(p, "traffic"))
				g_tfc_size = strtoul(p1, NULL, 10);
			else if (!_stricmp(p, "validate"))
			{
				if (!p1 || !_stricmp(p, "on") || !_stricmp(p, "1"))
					g_verify_result = true;
				else if (p1 && (!_stricmp(p, "off") || !_stricmp(p, "0")))
					g_verify_result = false;
				else
				{
					_Trace( 0, "Error: unknown parameter argument - verify=%s\n", p1);
					return  EINVAL;
				}
			}
			else if (p1 && (!_stricmp(p, "exclude") || !_stricmp(p, "x")))
			{
				if (g_excludes_count < sizeof(g_excludes)/sizeof(g_excludes[0]))
					g_excludes[g_excludes_count++] = p1;
				else
					_Trace( 0, "Warning: too many excludes, parameter ignored - %s\n", argv[i]);
			}
			else if (!_stricmp(p, "?") || !_stricmp(p, "h") || !_stricmp(p, "help"))
			{
				PrintUsage(argc, argv);
				return EINVAL;
			}
#ifdef _WIN32
			else if (!_stricmp(p, "idle") && p1)
			{
				if (!_stricmp(p1, "idle"))
					SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
				else if (!_stricmp(p1, "normal"))
					SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
				else if (!_stricmp(p1, "high"))
					SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
				else if (!_stricmp(p1, "realtime"))
					SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
				else
				{ 
					_Trace( 0, "Error: unknown parameter - %s\n", argv[i]);
					return EINVAL;
				}
			}
#endif
			else
			{ 
				_Trace( 0, "Error: unknown parameter - %s\n", argv[i]);
				return EINVAL;
			}
		}
		else if (!*src_path)
			make_fullpath(src_path, &src_fn, argv[i], 0, false);
		else if (!*hist_path)
			make_fullpath(hist_path, &hist_sub, argv[i], 0, true);
		else if (!*res_path)
			make_fullpath(res_path, &res_fn, argv[i], 0, true);
		else
		{
			_Trace( 0, "Error: too many parameters - %s\n", argv[i]);
			return EINVAL;
		}
		
	}

	if (!res_path[0])
	{
		PrintUsage(argc, argv);
		return EINVAL;
	}

	if (*log_path)
	{
		struct _stat st;
		bool bDir = false;
		char lastch = log_path[strlen(log_path)-1];
		if (lastch == '\\' || lastch == '/')
			bDir = true;
		else if (0 == _stat(log_path, &st) && (st.st_mode & _S_IFDIR)) // directory specified
			bDir = true;

		if (bDir)
		{
			struct tm* _tm;
			time_t _t;
			_t = time(0);
			_tm = localtime(&_t);
			make_fullpath(log_path, &log_fn, log_path, 0, true);
			sprintf(log_fn, "DIF-%04d%02d%02d.LOG", _tm->tm_year + 1900, _tm->tm_mon+1, _tm->tm_mday);
		}
		i_mkdir(log_path, log_fn);

		g_output = fopen(log_path, "a+");
		
		if (!g_output)
		{
			g_output = stdout;
			_Trace( 0, "Error: cannot open report file - %s\n", log_path);
			return EINVAL;
		}
		setvbuf(g_output,  NULL, _IONBF, 0 );
	}	
	
	if (*diff_list_path)
	{
		char* diff_list_fn = diff_list_path;
		make_fullpath(diff_list_path, &diff_list_fn, diff_list_path, 0, false);
		i_mkdir(diff_list_path, diff_list_fn);
		diff_list_file = fopen(diff_list_path, "w");
		if (!diff_list_file)
		{
			g_output = stdout;
			_Trace( 0, "Error: cannot open list file - %s\n", diff_list_path);
			return EINVAL;
		}
	}
	
	_Trace( 0, "\n");
	_Trace( 0, "-----------------------------------------------------------------------------------------------------\n");
	_Trace( 0, "\n");
	_Trace( 0, "Info: Started makediff " _VERSION_ " from %s\n", res_path);	

	err = EnumSourceFiles();
	_CloseDiffTrace();

	_Trace( 0, "Info: Job completed.\n");	
	return err;
}
