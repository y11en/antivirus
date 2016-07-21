// dbcompiler.cpp : Defines the entry point for the console application.
//

#include <minmax.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include "..\dbheader.h"

#ifndef countof
#define countof(array) (sizeof(array) / sizeof(array[0]))
#endif

#define AP_COMPILER_VERSION "v2.1 (" __TIMESTAMP__ ")"

#include "..\..\..\prague\hash\hash_api\md5\hash_md5.h"

FILE* g_report = stdout;
int   g_duplicate_urls = 0;
int   g_report_level = 0;

void parse_url(const char* url, const char** domen1_start, const char** domen1_end)
{
	if (*url != '/')
	{
		if (0 == strncmp(url, "http://", 7))
			url += 7;
		if (0 == strncmp(url, "ftp://", 6))
			url += 6;
		while (*url == '/')
			url++;
	}
	*domen1_start = url;
	*domen1_end = strchr(url, '/');
	if (!*domen1_end)
		*domen1_end = url + strlen(url);
}

int compare_urls(const void* url1, const void* url2)
{
	const char* domen1_start;
	const char* domen1_end;
	const char* domen2_start;
	const char* domen2_end;
	const char* u1 = *(const char**)url1;
	const char* u2 = *(const char**)url2;
	parse_url(u1, &domen1_start, &domen1_end);
	parse_url(u2, &domen2_start, &domen2_end);
	const char* p1, *p2;
	p1 = domen1_start, p2 = domen2_start;
	while (p1 != u1 && p2 != u2)
	{
		p1--;
		p2--;
		if (*p1 < *p2)
			return -1;
		if (*p1 > *p2)
			return 1;
	}
	p1 = domen1_end, p2 = domen2_end;
	while (p1 != domen1_start && p2 != domen2_start)
	{
		p1--;
		p2--;
		if (*p1 < *p2)
			return -1;
		if (*p1 > *p2)
			return 1;
	}
	return strcmp(domen1_end, domen2_end);
}

int __cdecl compare_str(const void *p1, const void *p2)
{
	char* s1 = *(char**) p1;
	char* s2 = *(char**) p2;
	return strcmp(s1, s2);
}


char* skip_spaces(char* str)
{
	while (*str == ' ' || *str == 9)
		str++;
	return str;
}

void remove_trailing_spaces(char* token_start, char* pos)
{
	while (token_start < pos)
	{
		pos--;
		if (*pos == ' ' || *pos == 9)
			*pos = 0;
		else
			break;
	}
	return;
}


__inline bool MatchOkayA(char* Pattern)
{
	if (*Pattern == '*')
		Pattern++;
	if (*Pattern)
		return false;
	return true;
}

bool MatchWithPatternA(char* Pattern, char* Name, bool CaseSensivity)
{
	char ch;
	char pc;

	if (!Pattern || !Name)
		return false;
	
	if(*Pattern=='*')
	{
		Pattern++;
		while(*Name && *Pattern)
		{
			pc = *Pattern;
			ch = *Name;
			if (CaseSensivity == false)
			{
				if (ch >= 'A' && ch <= 'Z')
					ch |= 0x20;
				if (pc >= 'A' && pc <= 'Z')
					pc |= 0x20;
			}
			if ((ch == '*') || (pc == ch) || (pc == '?'))
			{
				if(MatchWithPatternA(Pattern+1,Name+1, CaseSensivity)) 
					return true;
			}
			
			Name++;
		}
		return MatchOkayA(Pattern);
	} 
	
	while(*Name && *Pattern != '*')
	{
		pc = *Pattern;
		ch = *Name;
		if (CaseSensivity == false)
		{
			if (ch >= 'A' && ch <= 'Z')
				ch |= 0x20;
			if (pc >= 'A' && pc <= 'Z')
				pc |= 0x20;
		}
		if(pc == ch || pc == '?')
		{
			Pattern++;
			Name++;
		} else
			return false;
	}
	if(*Name)
		return MatchWithPatternA(Pattern,Name, CaseSensivity);
	
	return MatchOkayA(Pattern);
}

//#include <shlwapi.h>
#define URL_UNESCAPE                    0x10000000
#define URL_ESCAPE_UNSAFE               0x20000000
#define URL_PLUGGABLE_PROTOCOL          0x40000000
#define URL_WININET_COMPATIBILITY       0x80000000
#define URL_DONT_ESCAPE_EXTRA_INFO      0x02000000
#define URL_DONT_UNESCAPE_EXTRA_INFO    URL_DONT_ESCAPE_EXTRA_INFO
#define URL_BROWSER_MODE                URL_DONT_ESCAPE_EXTRA_INFO
#define URL_ESCAPE_SPACES_ONLY          0x04000000
#define URL_DONT_SIMPLIFY               0x08000000
#define URL_NO_META                     URL_DONT_SIMPLIFY
#define URL_UNESCAPE_INPLACE            0x00100000
#define URL_CONVERT_IF_DOSPATH          0x00200000
#define URL_UNESCAPE_HIGH_ANSI_ONLY     0x00400000
#define URL_INTERNAL_PATH               0x00800000  // Will escape #'s in paths
#define URL_FILE_USE_PATHURL            0x00010000
#define URL_ESCAPE_PERCENT              0x00001000
#define URL_ESCAPE_SEGMENT_ONLY         0x00002000  // Treat the entire URL param as one URL segment.

int __inline isalnum(char wc)
{
	if (wc >= 'a' && wc <= 'z')
		return 1;
	if (wc >= 'A' && wc <= 'Z')
		return 1;
	if (wc >= '0' && wc <= '9')
		return 1;
	return 0;
}

bool unescape_char(const char* hexstr, int chars, char* pwc)
{
	char res = 0;
	char wc;
	int i;
	for (i=0; i<chars; i++)
	{
		res <<= 4;
		wc = *hexstr++;
		if (wc >= '0' && wc <= '9')
			res |= (wc - '0');
		else if (wc >= 'a' && wc <= 'f')
			res |= (wc - 'a' + 10);
		else if (wc >= 'A' && wc <= 'F')
			res |= (wc - 'a' + 10);
		else
			return false;
	}
	*pwc = res;
	return true;
}

bool UrlUnescapeInplace(char* pszUrl)
{
    const char* src;
    char *dst;
	char wc;
	
    if(!pszUrl)
		return false;
	
    src = dst = pszUrl;
	
    do
	{
		wc = *src;
		if (wc == '%') 
		{
			if (src[1] == 'U')
			{
				if (unescape_char(src+2, 4, &wc)) 
					src += 4; /* Advance to end of escape */
			}
			else 
			{
				if (unescape_char(src+1, 2, &wc)) 
					src += 2; /* Advance to end of escape */
			}
		}
		*dst++ = wc;
		src++;
    } while (wc);
	
    return true;
}

bool UrlSimplifyInplace(char* pszUrl)
{
	char *wk1, *wk2, *mp, *root;
    int state;
    unsigned long nLen, nWkLen;

	/*
	* state =
	*         0   initial  1,3
	*         1   have 2[+] alnum  2,3
	*         2   have scheme (found :)  4,6,3
	*         3   failed (no location)
	*         4   have //  5,3
	*         5   have 1[+] alnum  6,3
	*         6   have location (found /) save root location
	*/
	
	wk1 = wk2 = (char*)pszUrl;
	state = 0;
	while (*wk1) 
	{
		switch (state) 
		{
		case 0:
			if (!isalnum(*wk1)) {state = 3; break;}
			*wk2++ = *wk1++;
			if (!isalnum(*wk1)) {state = 3; break;}
			*wk2++ = *wk1++;
			state = 1;
			break;
		case 1:
			*wk2++ = *wk1;
			if (*wk1++ == ':') state = 2;
			break;
		case 2:
			if (*wk1 != '/') {state = 3; break;}
			*wk2++ = *wk1++;
			if (*wk1 != '/') {state = 6; break;}
			*wk2++ = *wk1++;
			state = 4;
			break;
		case 3:
			if (wk1 == wk2)
				return true;
			nWkLen = strlen(wk1);
			memcpy(wk2, wk1, (nWkLen + 1) * sizeof(char));
			wk1 += nWkLen;
			wk2 += nWkLen;
			break;
		case 4:
			if (!isalnum(*wk1) && (*wk1 != '-') && (*wk1 != '.')) {state = 3; break;}
			while(isalnum(*wk1) || (*wk1 == '-') || (*wk1 == '.')) *wk2++ = *wk1++;
			state = 5;
			break;
		case 5:
			if (*wk1 != '/') {state = 3; break;}
			*wk2++ = *wk1++;
			state = 6;
			break;
		case 6:
			/* Now at root location, cannot back up any more. */
			/* "root" will point at the '/' */
			root = wk2-1;
			while (*wk1) {
//					PR_TRACE((this, prtNOTIFY, "wk1=%c\n", (tCHAR)*wk1));
				mp = strchr(wk1, '/');
				if (!mp) {
					nWkLen = strlen(wk1);
					memcpy(wk2, wk1, (nWkLen + 1) * sizeof(char));
					wk1 += nWkLen;
					wk2 += nWkLen;
					continue;
				}
				nLen = mp - wk1 + 1;
				memcpy(wk2, wk1, nLen * sizeof(char));
				wk2 += nLen;
				wk1 += nLen;
				if (*wk1 == '.') {
//						TRACE("found '/.'\n");
					if (*(wk1+1) == '/') {
						/* case of /./ -> skip the ./ */
						wk1 += 2;
					}
					else if (*(wk1+1) == '.') {
						/* found /..  look for next / */
//							TRACE("found '/..'\n");
						if (*(wk1+2) == '/' || *(wk1+2) == '?' || *(wk1+2) == '#' || *(wk1+2) == 0) {
							/* case /../ -> need to backup wk2 */
//								TRACE("found '/../'\n");
							*(wk2-1) = '\0';  /* set end of string */
							mp = strrchr(root, '/');
							if (mp && (mp >= root)) {
								/* found valid backup point */
								wk2 = mp + 1;
								if(*(wk1+2) != '/')
									wk1 += 2;
								else
									wk1 += 3;
							}
							else {
								/* did not find point, restore '/' */
								*(wk2-1) = '/';
							}
						}
					}
				}
			}
			*wk2 = '\0';
			break;
		default:
			//FIXME("how did we get here - state=%d\n", state);
			return false;
		}
	}
	*wk2 = '\0';
//		PR_TRACE((this, prtNOTIFY, "UrlCanonicalize: Simplified, orig <%S>, simple <%S>\n", pszUrl, lpszUrlCpy));
	return true;
}

bool UrlFixupInplace(char* pszUrl, unsigned int* pnSize)
{
	char *src, *dst, *domen;
	char wc;

	src = dst = pszUrl;

	while (1)
	{
		wc = *src;
		if (!wc)
			break;
		if (wc == ':' && src[1] == '/')
		{
			if (src[2] == '/')
				domen = src + 3;
			else
			{
				src++; // replace ":/" with "/"
				wc = '/';
			}
		}
		if (src != dst)
			*dst = wc;
		src++;
		dst++;
	};
	while (dst > pszUrl && (dst[-1] == '\r' || dst[-1] == '\n' || dst[-1] == '/'))
		dst--;
	*dst = 0;
	if (pnSize)
		*pnSize = dst - pszUrl + 1;
	return true;
}

bool add_hash(FILE* out, char* url, tANTIPHISHING_DB* pheader)
{
	char* md5_ctx = new char[hash_api_md5.ContextSize];
	unsigned long hash[4];
	unsigned long hash_size = sizeof(hash);
	hash_api_md5.Init(0, md5_ctx, hash_api_md5.ContextSize, 16, NULL);
	hash_api_md5.Update(md5_ctx, url, strlen(url));
	hash_api_md5.Finalize(md5_ctx, &hash_size, &hash);
	hash[0] ^= hash[2];
	hash[1] ^= hash[3];
	if (!fwrite(hash, 8, 1, out))
		return false;
	if (g_report_level >= 2)
		fprintf(g_report, "Info: Stored %08X%08X \"%s\"\n", hash[1], hash[0], url);
	pheader->hashes_count++;
	return true;
}

bool add_wildcard(FILE* out, char* wildcard, tANTIPHISHING_DB* pheader)
{
	char c, c1;
	char* pwc = wildcard;
	do 
	{
		c = *pwc++;
		c1 =  c ^ 0x56;
		if (!fwrite(&c1, 1, 1, out))
			return false;
	} while(c);
	pheader->wildcards_count++;
	if (g_report_level >= 1)
		fprintf(g_report, "Info: Stored wildcard \"%s\"\n", wildcard);
	return true;
}

typedef struct _str_vector_item {
	char* str;
	unsigned int weight;
} t_str_vector_item;

typedef struct _str_vector {
	t_str_vector_item* item;
	unsigned count;
	unsigned allocated;
} t_str_vector;


bool init_str_vector(t_str_vector* pvector)
{
	memset(pvector, 0, sizeof(t_str_vector));
	return true;
}

bool copy_str_vector_item(t_str_vector* pvector, unsigned idx_dst, unsigned idx_src)
{
	if (idx_dst == idx_src)
		return true;
	if (idx_src >= pvector->count)
		return false;
	if (idx_dst >= pvector->count)
		return false;
	pvector->item[idx_dst].str = pvector->item[idx_src].str;
	pvector->item[idx_dst].weight = pvector->item[idx_src].weight;
	return true;
}

bool free_str_vector_item(t_str_vector* pvector, unsigned idx)
{
	if (idx >= pvector->count)
		return false;
	free(pvector->item[idx].str);
	memset(&pvector->item[idx], 0, sizeof(t_str_vector_item));
	return true;
}

bool push_str_vector(t_str_vector* pvector, char* str, unsigned int weight)
{
	char* p = (char*)malloc(strlen(str)+1);
	if (!p)
		return false;
	if (pvector->count == pvector->allocated)
	{
		pvector->allocated += 100;
		pvector->item = (t_str_vector_item*)realloc(pvector->item, pvector->allocated * sizeof(t_str_vector_item));
		if (!pvector->item)
		{
			pvector->allocated = 0;
			pvector->count = 0;
			return false;
		}
	}
	strcpy(p, str);
	pvector->item[pvector->count].str = p;
	pvector->item[pvector->count].weight = weight;
	pvector->count++;
	return true;
}

bool push_uniq_str_vector(t_str_vector* pvector, char* str, unsigned int weight)
{
	for (unsigned i=0; i<pvector->count; i++)
	{
		if (!strcmp(pvector->item[i].str, str))
			return true;
	}
	return push_str_vector(pvector, str, weight);
}


void free_str_vector(t_str_vector* pvector, char* str)
{
	for (unsigned i=0; i<pvector->count; i++)
		free_str_vector_item(pvector, i);
	free(pvector->item);
	init_str_vector(pvector);
}

bool isInList(char* url, t_str_vector* plist, unsigned* pmatched_item)
{
	int domain_len = 0;
	char* domain = strstr(url, "://");
	if (domain)
	{
		domain+=3;
		char* domain_end = strchr(domain, '/');
		if (domain_end)
			domain_len =  domain_end - domain;
		if (!domain_len)
			domain_len = strlen(domain);
	}
	for (unsigned i=0; i<plist->count; i++)
	{
		char* item = plist->item[i].str;
		if (NULL == strchr(item, '/')) // this is domain
		{
			if (!domain || !domain_len)
				continue;
			bool matched = false;
			char c = domain[domain_len];
			domain[domain_len] = 0;
			if (strchr(item, '*') || strchr(item, '?')) // this is mask
			{
				if (MatchWithPatternA(item, domain, false))
					matched = true;
			}
			else
			{
				do 
				{
					int wclen = strlen(item);
					if (domain_len < wclen)
						break;
					if (domain_len > wclen && domain[domain_len-wclen-1] != '.')
						break;
					if (!strnicmp(item, domain+(domain_len-wclen), wclen))
						matched = true;
				} while(false);
			}
			domain[domain_len] = c;
			if (matched)
			{
				if (pmatched_item)
					*pmatched_item = i;
				return true;
			}
		}
		else
		{
			if (MatchWithPatternA(item, url, false))
			{
				if (pmatched_item)
					*pmatched_item = i;
				return true;
			}
		}
	}
	return false;
}

int main(int argc, char* argv[])
{
	FILE *out;
	unsigned long discarded_by_whitelist = 0;
	unsigned urls_count = 0;
	unsigned wildcards_count = 0;
	unsigned params_wildcards_count = 0;
	unsigned heur_wildcards_count = 0;
	unsigned discarded_by_heur_wildcards = 0;
	unsigned discarded_by_wildcards = 0;
	unsigned was_urls_count = 0;
	tANTIPHISHING_DB header;
	unsigned i, j;
	unsigned long* pheader = (unsigned long*)&header;
	unsigned long header_key = 0x56D3932A;
	char* input_name = NULL;
	char* db_name = NULL;
	char* post_exec = NULL;
	char* report_name = NULL;
	char* reccount_file_name = NULL;
	bool bWasErrors = false;
	unsigned last;
	unsigned smart_check = 0;
	unsigned smart_weight = 100;
	unsigned minimum_weight = 100;
	unsigned discarded_by_minimum_weight = 0;
	
	t_str_vector blacklist_files;
	t_str_vector whitelist_files;
	t_str_vector urls;
	t_str_vector whitelist;
	t_str_vector wildcards;

	init_str_vector(&blacklist_files);
	init_str_vector(&whitelist_files);
	init_str_vector(&urls);
	init_str_vector(&whitelist);
	init_str_vector(&wildcards);

	struct tm* _tm;
	time_t _t;

	_t = time(0);
	_tm = localtime(&_t);

	for (i=1; i<(unsigned)argc; i++)
	{
		bool invalid = false;
		if (argv[i][0] == '-' || argv[i][0] == '/')
		{
			char* p=argv[i]+1;
			if (p[0]=='R' || p[0]=='r')
			{
				if (!p[1])
				{
					report_name = "aphish.log";
				}
				else if (p[1] == ':')
				{
					report_name = p+2;
				}
				else
					invalid = true;
			}
			else if (p[0]=='V' || p[0]=='v')
			{
				if (!p[1])
				{
					g_report_level = 1000;
				}
				else if (p[1] == ':')
				{
					g_report_level = strtoul(p+2, NULL, 10);
				}
				else
					invalid = true;
			}
			else if (p[0]=='C' || p[0]=='c')
			{
				if (p[1] == ':')
				{
					post_exec = p+2;
				}
				else
					invalid = true;
			}
			else if (p[0]=='W' || p[0]=='w')
			{
				if (p[1] == ':')
					push_uniq_str_vector(&whitelist_files, p+2, 0);
				else
					invalid = true;
			}
			else if (p[0]=='B' || p[0]=='b')
			{
				if (p[1] == ':')
					push_uniq_str_vector(&blacklist_files, p+2, 0);
				else
					invalid = true;
			}
			else if (p[0]=='N' || p[0]=='n')
			{
				if (p[1] == ':')
				{
					reccount_file_name = p+2;
				}
				else
					invalid = true;
			}
			else if (p[0]=='S' || p[0]=='s')
			{
				if (p[1] == ':')
				{
					smart_check = strtoul(p+2, NULL, 10);
					if (!smart_check)
						invalid = true;
				}
				else if ((p[1] == 'M' || p[1] == 'm') && p[2] == ':')
				{
					smart_weight = strtoul(p+3, NULL, 10);
				}
				else
					invalid = true;
			}
			else if (p[0]=='M' || p[0]=='m')
			{
				if (p[1] == ':')
					minimum_weight = strtoul(p+2, NULL, 10);
				else
					invalid = true;
			}
			else
				invalid = true;

			if (invalid)
			{
				fprintf(g_report, "Warning: invalid switch - '%s'\n", argv[i]);
				return EINVAL;
			}
		}
		else if (!input_name)
		{
			input_name = argv[i];
			push_uniq_str_vector(&blacklist_files, input_name, 0);
		}
		else if (!db_name)
			db_name = argv[i];
		else 
		{
			fprintf(g_report, "Warning: Unexpected parameter - '%s'\n", argv[i]);
			return EINVAL;
		}
	}

	if (!input_name)
	{
		fprintf(stdout, "Anti-Phishing database compiler " AP_COMPILER_VERSION "\n");
		fprintf(stdout, "Kaspersky Lab. INTERNAL USE ONLY!\n");
		fprintf(stdout, "Usage: dbcompiler.exe <infile> [outfile] [-w:<whitelist.txt>] [-c:<command>]\n");
		fprintf(stdout, "Where: <infile>               - main blacklist\n");
		fprintf(stdout, "                                you may specify more blacklists using -b switch\n");
		fprintf(stdout, "       [outfile]              - output database filename (default: aphish.dat)\n");
		fprintf(stdout, "       [-r[:report.txt]]      - report\n");
		fprintf(stdout, "                                if no filename specified 'aphish.log' will be used\n");
		fprintf(stdout, "                                if directory specified filename will be generated from date/time\n");
		fprintf(stdout, "       [-v[:<level>]]         - verbose reporting (default level: maximum)\n");
		fprintf(stdout, "       [-w:whitelist.txt]     - file containing whitelist url wildcard(s) to exclude\n");
		fprintf(stdout, "                                multiple whitelist files may be specified\n");
		fprintf(stdout, "       [-b:blacklist.txt]     - file containing blacklist of urls to include\n");
		fprintf(stdout, "                                multiple blacklist files may be specified\n");
		fprintf(stdout, "                                supported blacklist formats are: APWG(CSV), PhishTank(XML), plain text\n");
		fprintf(stdout, "       [-m:<minimum_weight>]  - urls with less weight are discarded\n");
		fprintf(stdout, "       [-s:<count>]           - search for similar urls and replace them with wildcards,\n");
		fprintf(stdout, "                                <count> specifies minimum count of urls to be found to replace\n");
		fprintf(stdout, "       [-sm:<minimum_weight>] - urls with less weight not checked for similarity (see -s switch)\n");
		fprintf(stdout, "       [-c:<command>]         - command to execute on successful compile (post-build step)\n");
		fprintf(stdout, "       [-n:filename.txt]      - file to save number of records in database\n");
		fprintf(stdout, "\n");
		fprintf(stdout, "Return codes (correspond CRT error codes):\n");
		fprintf(stdout, "    0  - No error\n");
		fprintf(stdout, "    2  - File not found\n");
		fprintf(stdout, "    5  - Write error\n");
		fprintf(stdout, "    12 - Not enought memory\n");
		fprintf(stdout, "    22 - Invalid parameter\n");
		fprintf(stdout, "    Note: If database compiled successfuly and -c switch specified returned error code is\n");
		fprintf(stdout, "          result of post-build step command execution\n");
		return 0;
	}

	// verify output name
	if (!db_name)
		db_name = "aphish.dat";

	// open g_report
	if (report_name)
	{
		struct _stat st;
		int len;
		char log_path[_MAX_PATH*2];
		_fullpath(log_path, report_name ? report_name : ".", _MAX_PATH);
		len = strlen(log_path);
		if (log_path[len-1] == '\\') // cut ending slash
		{
			log_path[len-1] = 0;
			len--;
		}
		if (0 == _stat(log_path, &st))
		{
			if (st.st_mode & _S_IFDIR) // directory specified
			{
				sprintf(log_path+len, "\\%04d.%02d.%02d.log", _tm->tm_year + 1900, _tm->tm_mon+1, _tm->tm_mday, _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
			}
		}
		g_report = fopen(log_path, "a+");
	}
	if (!g_report)
	{
		g_report = stdout;
		fprintf(g_report, "Error: Cannot open g_report file '%s'\n", report_name);
	}
	// set no buffering for g_report
	setvbuf(g_report,  NULL, _IONBF, 0 );

	fprintf(g_report, "\n\n---\nStarted on %04d-%02d-%02d %02d:%02d:%02d Anti-Phishing database compiler " AP_COMPILER_VERSION "\n", _tm->tm_year + 1900, _tm->tm_mon+1, _tm->tm_mday, _tm->tm_hour, _tm->tm_min, _tm->tm_sec);

	out = fopen(db_name, "wb+");
	if (!out)
	{
		fprintf(g_report, "Error: Cannot open output file '%s'\n", db_name);
		return ENOENT;
	}

	// load whitelists
	for (i=0; i<whitelist_files.count; i++)
	{
		char* whitelist_name = whitelist_files.item[i].str;
		FILE* wl = fopen(whitelist_name, "rt");
		unsigned loaded = 0;
		if (wl)
		{
			if (g_report_level >= 1)
				fprintf(g_report, "Info: Loading whitelist '%s'...\n", whitelist_name);
			char buff[0x2000];
			while (fgets(buff, sizeof(buff), wl))
			{
				char* p;
				if (buff[0] == '#' || buff[0] == ';') // skip comments
					continue;
				buff[sizeof(buff) - 1] = 0; // ensure zero-terminated
				p = strchr(buff, 0x0a);
				if (p) *p = 0;
				p = strchr(buff, 0x0d);
				if (p) *p = 0;
				if (!*buff)
					continue;
				if (!push_str_vector(&whitelist, buff, 0))
				{
					fprintf(g_report, "Error: Not enough memory while loading whitelist\n");
					return ENOMEM;
				}
				loaded++;
				if (g_report_level >= 2)
					fprintf(g_report, "Info: Load whitelist wildcard '%s'\n", buff);
			}
			fprintf(g_report, "Info: Whitelist '%s' loaded. Wildcards count %d\n", whitelist_name, loaded);
			fclose(wl);
		}
		else
		{
			fprintf(g_report, "Error: Cannot open whitelist file '%s'\n", whitelist_name);
			return ENOENT;
		}
	}
	fprintf(g_report, "Info: Total whitelist wildcards %d\n", whitelist.count);

	// load blacklist
	for (i=0; i<blacklist_files.count; i++)
	{
		unsigned loaded_urls = 0;
		unsigned loaded_whildcards = 0;
		int file_format = 0; 
		char* blacklist_name = blacklist_files.item[i].str;
		FILE* bl = fopen(blacklist_name, "rt");
		if (bl)
		{
			unsigned long lineno = 0;
			if (g_report_level >= 1)
				fprintf(g_report, "Info: Loading blacklist '%s'...\n", blacklist_name);
			char buff[0x2000];
			// read lines
			while (fgets(buff, sizeof(buff), bl))
			{
				lineno++;
				bool check_mask = false;
				unsigned weight = 100;
				char* p;
				if (buff[0] == '#' || buff[0] == ';') // skip comments
					continue;
				buff[sizeof(buff) - 1] = 0; // ensure zero-terminated
				p = strchr(buff, 0x0a);
				if (p) *p = 0;
				p = strchr(buff, 0x0d);
				if (p) *p = 0;
				char* url = skip_spaces(buff);
				if (!*url)
					continue;
				
				if (file_format == 1 || *url == '<') // XML
				{
					if (0 != strnicmp(url, "<url>", 5))
						continue;
					url+=5;
					char* end = strstr(url, "</url>");
					if (!end)
					{
						fprintf(g_report, "Error: Cannot parse URL in line %d: %s\n", lineno, url);
						continue;
					}
					*end = 0;
					file_format = 1;
					if (0 == strnicmp(url, "<![CDATA[", 9))
					{
						url += 9;
						end = strstr(url, "]]>");
						if (!end)
						{
							fprintf(g_report, "Error: Cannot parse URL in line %d: %s\n", lineno, url);
							bWasErrors = true;
							continue;
						}
						*end = 0;
					}
				}
				else if (file_format == 2 || strstr(url, "\",URL")) // APWGphish.txt format
				{
					int tokens_count = 0;
					char* token[40];
					char* cur_token = buff;
					char* pos;
					file_format = 2;
					do
					{
						cur_token = skip_spaces(cur_token);
						if (*cur_token == '\"')
						{
							bool quoted;
							cur_token++;
							do 
							{
								quoted = false;
								pos = strchr(cur_token, '\"');
								if (!pos)
								{
									fprintf(g_report, "Error: Cannot parse parameter %d in line %d: %s\n", tokens_count, lineno, buff);
									break;
								}
								do *pos++ = 0; while (*pos == '\"');
								remove_trailing_spaces(cur_token, pos);
								pos = skip_spaces(pos);
								if (*pos && *pos != ',')
								{
									if (!strcmp(cur_token, "3D"))
									{
										cur_token = pos;
										quoted = true;
										continue;
									}
									fprintf(g_report, "Error: Cannot parse parameter %d in line %d: %s\n", tokens_count, lineno, buff);
									break;
								}
								*pos++ = 0;
							} while (quoted);
							token[tokens_count++] = cur_token;
						}
						else
						{
							token[tokens_count++] = cur_token;
							pos = strchr(cur_token, ',');
							if (!pos)
								pos = cur_token + strlen(cur_token);
							remove_trailing_spaces(cur_token, pos);
							if (*pos == ',')
								*pos++ = 0;
						}
						cur_token = pos;
					} while (cur_token && *cur_token && tokens_count < countof(token));
					if (tokens_count != 6)
					{
						fprintf(g_report, "Error: Wrong parameters count(%d) in line %d: %s\n", tokens_count, lineno, buff);
						bWasErrors = true;
						continue;
					}
					url = token[3]; // canonicalized URL
					char* params = strchr(url, '#');
					if (params)
						*params = 0;
					
					char* weight_str = token[2];
					while (*weight_str > 0 && *weight_str <= ' ') 
						weight_str++;
					weight = strtoul(weight_str, 0, 10);
					// 		if (weight<minimum_weight)
					// 		{
					// 			discarded_by_minimum_weight++;
					// 			if (g_report_level >= 2)
					// 				fprintf(g_report, "Info: Discarded by weight %d<%d URL \"%s\"\n", weight, minimum_weight, url);
					// 			continue;
					// 		}
				}
				else // plain format
				{
					check_mask = true;
				}

				url = skip_spaces(url);
				if (*url == 0) // empty URL
					continue;

				UrlSimplifyInplace(url);
				UrlUnescapeInplace(url);
				UrlFixupInplace(url, NULL);
				strlwr(url);

				if (check_mask && strchr(url, '*'))
				{
					if (!push_uniq_str_vector(&wildcards, url, weight))
					{
						fprintf(g_report, "Error: Not enough memory while loading blacklist\n");
						return ENOMEM;
					}
					loaded_whildcards++;
					if (g_report_level >= 2)
						fprintf(g_report, "Info: Load blacklist wildcard '%s'\n", url);
				}
				else if (check_mask && !strchr(url, '/') && !strchr(url, '?')) // pure domen name
				{
					if (!strchr(url, '.')) // 1-st level domen - deny
					{
						fprintf(g_report, "Error: blacklist entry '%s' is not valid\n", url);
						bWasErrors = true;
						continue;
					}
					memmove(url+7, url, strlen(url)+1); // make space for http://
					memcpy(url, "http://", 7);
					strcat(url, "/*");
					if (!push_uniq_str_vector(&wildcards, url, weight))
					{
						fprintf(g_report, "Error: Not enough memory while loading blacklist\n");
						return ENOMEM;
					}
					loaded_whildcards++;
					if (g_report_level >= 2)
						fprintf(g_report, "Info: Load blacklist wildcard '%s'\n", url);
					memmove(url+9, url+7, strlen(url+7)+1); // add sub-domens wildcard
					memcpy(url+7, "*.", 2);
					if (!push_uniq_str_vector(&wildcards, url, weight))
					{
						fprintf(g_report, "Error: Not enough memory while loading blacklist\n");
						return ENOMEM;
					}
					loaded_whildcards++;
					if (g_report_level >= 2)
						fprintf(g_report, "Info: Load blacklist wildcard '%s'\n", url);
				}
				else
				{
					if (!push_str_vector(&urls, url, weight))
					{
						fprintf(g_report, "Error: Not enough memory while loading blacklist\n");
						return ENOMEM;
					}
					loaded_urls++;
					if (g_report_level >= 2)
						fprintf(g_report, "Info: Load blacklist weight %3d url '%s'\n", weight, url);
				}

			}
			fprintf(g_report, "Info: Blacklist '%s' loaded. Urls %d, wildcards %d\n", blacklist_name, loaded_urls, loaded_whildcards);
			fclose(bl);
		}
		else
		{
			fprintf(g_report, "Error: Cannot open blacklist file '%s'\n", blacklist_name);
			return ENOENT;
		}
	}
	was_urls_count = urls.count;

	fprintf(g_report, "Info: Total blacklist urls %d, wildcards %d\n", urls.count, wildcards.count);

	// check whitelist
	last = 0;
	for (j=0; j<urls.count; j++)
	{
		char* url = urls.item[j].str;
		if(isInList(url, &whitelist, &i))
		{
			discarded_by_whitelist++;
			if (g_report_level >= 1)
				fprintf(g_report, "Info: Discarded by whitelist '%s' Url '%s'\n", whitelist.item[i].str, url);
			continue;
		}
		copy_str_vector_item(&urls, last, j);
		last++;
	}
	urls.count = last;

	fprintf(g_report, "Info: URLs discarded by whitelist %d (%d left)\n", discarded_by_whitelist, urls.count);


	// check ?[PARAMETERS]
	last = 0;
	for (j=0; j<urls.count; j++)
	{
		char* url = urls.item[j].str;
		char* params = NULL;
		bool bParam = false;

		if (urls.item[j].weight >= minimum_weight)
		{
			params = strchr(url, '?');
			if (params)
			{
				if (0 == strnicmp(params, "?[PARA", 5))
					bParam = true;
			}
		}
		
		if (!bParam)
		{
			copy_str_vector_item(&urls, last, j);
			last++;
			continue;
		}

		strcpy(params, "*");

		if (!push_uniq_str_vector(&wildcards, url, urls.item[j].weight))
		{
			fprintf(g_report, "Error: Not enough memory while chacking ?[PARAMETERS]\n");
			return ENOMEM;
		}
		if (g_report_level >= 2)
			fprintf(g_report, "Info: Added wildcard '%s' due to ?[PARAMETERS]\n", url);
		params_wildcards_count++;
	}
	urls.count = last;

	fprintf(g_report, "Info: Wildcards by ?[PARAMETERS]   %d\n", params_wildcards_count);

	// check already added wildcards
	last = 0;
	for (j=0; j<urls.count; j++)
	{
		bool wildcard_matched = false;
		char* url = urls.item[j].str;

		if(isInList(url, &wildcards, &i))
		{
			discarded_by_wildcards++;
			if (g_report_level >= 2)
				fprintf(g_report, "Info: Discarded by whildcard '%s' Url '%s'\n", wildcards.item[i].str, url);
			continue;
		}

		copy_str_vector_item(&urls, last, j);
		last++;
	}
	urls.count = last;

	fprintf(g_report, "Info: URLs discarded by black+param wildcards %d (%d left)\n", discarded_by_wildcards, urls.count);
	
	// sort URLs
	qsort(urls.item, urls.count, sizeof(urls.item[0]), compare_urls);

	// delete duplicate URLs
	last = 0;
	if (urls.count)
	{
		for (i=1; i<urls.count; i++)
		{
			if (strcmp(urls.item[last].str, urls.item[i].str))
			{
				last++;
				copy_str_vector_item(&urls, last, i);
			}
			else
			{
				g_duplicate_urls++;
				if (g_report_level >= 2)
					fprintf(g_report, "Info: Discarded duplicate URL \"%s\"\n", urls.item[i].str);
			}
		}
		urls.count = last+1;
	}

	fprintf(g_report, "Info: URLs discarded as duplicated %d (%d left)\n", g_duplicate_urls, urls.count);

	// replace multiple hashes with wildcards
	last = 0;
	if (smart_check && urls.count)
	{
		unsigned dup_count = 0;
		unsigned dup_weight = 0;
		char* last_part_start = 0;
		char* last_part_end = 0;
		for (i=1; i<urls.count; i++)
		{
			if (urls.item[i].weight < smart_weight)
				continue;

			
			char* last_url = urls.item[last].str;
			char* part_start = 0;
			char* part_end = 0;
			char* url = urls.item[i].str;
			char c, prev_c=0;
			bool ok = false;
			bool domen = false;
			while (c = *last_url)
			{
				if (c == *url)
				{
					if (c == '/')
					{
						if (prev_c == '/')
							domen = true;
						else
							domen = false;
					}
					if (c == '/' || c == '.')
						part_start = last_url+1;
					last_url++;
					url++;
					prev_c = c;
					continue;
				}
				char* end1, *end2;
				if (!part_start)
					break;
				if (domen)
				{
					int i, c=0;
					for (i=0; ; i++)
					{
						if (!last_url[i])
							break;
						if (last_url[i] == '/')
							break;
						if (last_url[i] == '.')
							c++;
					}
					if (c < 2) // 1st or 2nd level domen
						break;
				}
				end1 = strchr(last_url, '/');
				end2 = strchr(last_url, '.');
				part_end = min(end1 ? end1 : end2, end2 ? end2 : end1);
				if (!part_end)
					break;
				if (part_start >= part_end) // ???
					break;
				last_url = part_end + 1;
				end1 = strchr(url, '/');
				end2 = strchr(url, '.');
				end1 = min(end1 ? end1 : end2, end2 ? end2 : end1);
				if (!end1)
					break;
				url = end1+1;
				if (strcmp(last_url, url))
					break;
				ok = true;
				break;
			}
			if (ok)
			{
				last_part_start = part_start;
				last_part_end = part_end;
				dup_count++;
			}
			else
			{
				if (dup_count >= smart_check)
				{
					*last_part_start = '*';
					strcpy(last_part_start+1, last_part_end);
					char* params = strstr(urls.item[last].str, "?[para");
					if (params)
						strcpy(params+1, "*");
					if (!push_uniq_str_vector(&wildcards, urls.item[last].str, 0))
					{
						fprintf(g_report, "Error: Not enough memory while loading whitelist\n");
						return ENOMEM;
					}
					heur_wildcards_count++;
					if (g_report_level >= 1)
						fprintf(g_report, "Info: Replaced %5d URLs with wildcard '%s'\n", dup_count+1, urls.item[last].str);
					free_str_vector_item(&urls, last); // free url replaced with wildcard
					// other urls will discard later
				}
				last=i;
				dup_count = 0;
			}
		}
		last = 0;
		for (i=0; i<urls.count; i++)
		{
			if (urls.item[i].str)
			{
				copy_str_vector_item(&urls, last, i);
				last++;
			}
		}
		urls.count = last;
	}

	fprintf(g_report, "Info: Wildcards by heuristic       %d\n", heur_wildcards_count);

	// check already added wildcards
	last = 0;
	for (j=0; j<urls.count; j++)
	{
		bool wildcard_matched = false;
		char* url = urls.item[j].str;

		for (i=0; i<wildcards.count; i++)
		{
			if (MatchWithPatternA(wildcards.item[i].str, url, false))
			{
				wildcard_matched = true;
				break;
			}
		}
		if (wildcard_matched)
		{
			if (g_report_level >= 2)
				fprintf(g_report, "Info: Discarded by whildcard '%s' Url '%s'\n", wildcards.item[i].str, url);
			discarded_by_heur_wildcards++;
			continue;
		}

		copy_str_vector_item(&urls, last, j);
		last++;
	}
	urls.count = last;

	fprintf(g_report, "Info: URLs discarded by heuristic wildcards %d (%d left)\n", discarded_by_heur_wildcards, urls.count);

	// drop urls under minimum weight
	last = 0;
	for (i=0; i<urls.count; i++)
	{
		if (urls.item[i].weight < minimum_weight)
		{
			discarded_by_minimum_weight++;
			if (g_report_level >= 2)
				fprintf(g_report, "Info: Discarded by weight %d<%d URL \"%s\"\n", urls.item[i].weight, minimum_weight, urls.item[i].str);
			continue;
		}

		if (urls.item[i].str)
		{
			copy_str_vector_item(&urls, last, i);
			last++;
		}
	}
	urls.count = last;

	fprintf(g_report, "Info: URLs discarded by weight %d (%d left)\n", discarded_by_minimum_weight, urls.count);

	// add test URLs
	if (!push_uniq_str_vector(&urls, "http://www.kaspersky.com/test/aphish_h", 100))
	{
		fprintf(g_report, "Error: Not enough memory while adding \"http://www.kaspersky.com/test/aphish_h\"\n");
		return ENOMEM;
	}
	if (!push_uniq_str_vector(&wildcards, "http://www.kaspersky.com/test/aphish_w/*", 100))
	{
		fprintf(g_report, "Error: Not enough memory while adding \"http://www.kaspersky.com/test/aphish_w/*\"\n");
		return ENOMEM;
	}

	// fill header
	memset(&header, 0, sizeof(header));
	memcpy(header.signature, ANTIPHISHING_DB_SIGNATURE, 4);
	if (!fwrite(&header, sizeof(header), 1, out))
	{
		fprintf(g_report, "Error: write operation failed\n");
		return EIO;
	}

	// add hashes
	header.hashes_offset = ftell(out);
	for (i=0; i<urls.count; i++)
	{
		if (!add_hash(out, urls.item[i].str, &header))
		{
			fprintf(g_report, "Error: write operation failed\n");
			return EIO;
		}
	}
	header.hashes_size = ftell(out) - header.hashes_offset;

	// wildcards
	header.wildcards_offset = ftell(out);
	for (i=0; i<wildcards.count; i++)
	{
		if (!add_wildcard(out, wildcards.item[i].str, &header))
		{
			fprintf(g_report, "Error: add_wildcard failed\n");
			return EIO;
		}
	}
	header.wildcards_size = ftell(out) - header.wildcards_offset;

	// finalizing
	urls_count = header.hashes_count;
	wildcards_count = header.wildcards_count;
	header.total_db_size = ftell(out);
	header.header_size = sizeof(header);
	header.timestamp = time(0);
	for (i=1; i<sizeof(header)/sizeof(unsigned long);i++)
	{
		pheader[i] ^= header_key;
		header_key += header_key;
	}

	fseek(out, 0, SEEK_SET);
	if (!fwrite(&header, sizeof(header), 1, out))
	{
		fprintf(g_report, "Error: write operation failed\n");
		return EIO;
	}

	if (g_report_level >= 1)
	{
		fprintf(g_report, "----\n");
		fprintf(g_report, "Info: Initial URLs count           %d\n", was_urls_count);
		fprintf(g_report, "Info: URLs discarded as duplicated %d\n", g_duplicate_urls);
		fprintf(g_report, "Info: URLs discarded by whitelist  %d\n", discarded_by_whitelist);
		fprintf(g_report, "Info: URLs discarded by weight     %d\n", discarded_by_minimum_weight);
		fprintf(g_report, "Info: Wildcards by ?[PARAMETERS]   %d\n", params_wildcards_count);
		fprintf(g_report, "Info: Wildcards by heuristic       %d\n", heur_wildcards_count);
		fprintf(g_report, "Info: URLs discarded by black+param wildcards %d\n", discarded_by_wildcards);
		fprintf(g_report, "Info: URLs discarded by heuristic wildcards %d\n", discarded_by_heur_wildcards);
	}
	fprintf(g_report, "Info: Database URLs count             %d\n", urls_count);
	fprintf(g_report, "Info: Database wildcards              %d\n", wildcards_count);
	fseek(out, 0, SEEK_END);
	fprintf(g_report, "Info: Database size                   %d\n", ftell(out));
	
	fclose(out);

	if (reccount_file_name)
	{
		FILE* reccount_file = fopen(reccount_file_name, "w");
		if (!reccount_file)
			fprintf(g_report, "Error: Cannot save number of records into file '%s'\n", reccount_file_name);
		else
		{
			fprintf(reccount_file, "%d", urls_count + wildcards_count);
			fclose(reccount_file);
		}
	}

	if (bWasErrors)
		fprintf(g_report, "Info: Database '%s' compiled, but some URL's ignored due to parsing errors.\n", db_name);

	fprintf(g_report, "Info: Database '%s' successfuly compiled.\n", db_name);

	if (post_exec)
	{
		if (g_report_level >= 1)
			fprintf(g_report, "Info: Performing post-build step '%s'...\n", post_exec);
		int ret = system(post_exec);
		fprintf(g_report, "Info: Post-buld '%s' result: %d.\n", post_exec, ret);
		if (ret)
		{
			fprintf(g_report, "Error: Database '%s' compiled, but post-build returned error %d.\n", db_name, ret);
			return ret;
		}
	}

	_t = time(0);
	_tm = localtime(&_t);
	fprintf(g_report, "Finished on %04d-%02d-%02d %02d:%02d:%02d\n----\n\n", _tm->tm_year + 1900, _tm->tm_mon+1, _tm->tm_mday, _tm->tm_hour, _tm->tm_min, _tm->tm_sec);

	return 0;
}
