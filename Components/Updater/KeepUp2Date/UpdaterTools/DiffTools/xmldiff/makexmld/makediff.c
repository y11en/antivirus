#include <string.h>
#include <io.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>

#include "xml_diff.h"
#include "heap_al.h"
#include <md5.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <errno.h>

#define MAX_VER   5000
#define VER_LIMIT 400

unsigned int g_trace_level = 0;
FILE*        g_output = stdout;

void _Trace(unsigned int level, const char* format, ...)
{
	va_list _list;
	struct tm* _tm;
	time_t _t;

	if (g_trace_level < level)
		return;
	_t = time(0);
	_tm = localtime(&_t);
	fprintf(g_output, "%04d-%02d-%02d %02d:%02d:%02d ", _tm->tm_year + 1900, _tm->tm_mon+1, _tm->tm_mday, _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
	va_start(_list, format);
	vfprintf(g_output, format, _list);
	va_end(_list);
	if (format[strlen(format)-1] != '\n')
		fprintf(g_output, "\n");
}


int __cdecl qsort_int_compare(const void *p1, const void *p2) 
{
	int i1, i2;
	i1 = *(int*)p1;
	i2 = *(int*)p2;
	if (i1 < i2)
		return -1;
	else if (i1 > i2)
		return 1;
	return 0;
}

bool i_mkdir(char* path)
{
        bool res = false;
        int err;
        char* slash = path;
        err = mkdir(path);
        if (err==0 || (err==-1 && errno==EEXIST))
                return true;
        while (slash = strchr(slash, '\\'))
        {
                *slash = 0;
                err = mkdir(path);
                if (err==0 || (err==-1 && errno==EEXIST))
                        res = true;
                else
                        res = false;
                *slash = '\\';
                slash++;
        }
        return res;
}

bool iMakeDiff(tHeap* pHeap, tFile* pOldFile, tFile* pNewFile, tFile* pDiff)
{
	void *pData1, *pData2;
#ifdef _WINDOWS_
//	DWORD t1, t2;
#endif
	tFileSize dwSize1, dwSize2;
	size_t read;
	bool res;
	
	pOldFile->GetSize(pOldFile, &dwSize1);
	if (!pHeap->Alloc(pHeap, dwSize1, &pData1))
		return false;
	pOldFile->Seek(pOldFile, 0, SEEK_SET, NULL);
	pOldFile->Read(pOldFile, pData1, dwSize1, &read);
	if (read != dwSize1)
		return false;
	
	pNewFile->GetSize(pNewFile, &dwSize2);
	if (!pHeap->Alloc(pHeap, dwSize2, &pData2))
		return false;
	pNewFile->Seek(pNewFile, 0, SEEK_SET, NULL);
	pNewFile->Read(pNewFile, pData2, dwSize2, &read);
	if (read != dwSize2)
		return false;
	
#ifdef _WINDOWS_
//	t1 = GetTickCount();
#endif
	pDiff->Seek(pDiff, 0, SEEK_SET, NULL);
	res = MakeXMLDiff(pData1, dwSize1, pData2, dwSize2, pDiff);
#ifdef _WINDOWS_
//	t2 = GetTickCount();
//	_Trace( 0, "Elapsed time: %d ms\n", t2-t1);
#endif
	
	pHeap->Free(pHeap, pData1);
	pHeap->Free(pHeap, pData2);
	return res;
}

bool iSave(tFile* pFile, char* fname, char* ext)
{
	tFile sResFile;
	bool res = false;
	char path[_MAX_PATH];
	strcpy(path, fname);
	if (ext)
		strcat(path, ext);
	if (InitFileStdioN(&sResFile, path, "wb+"))
		CopyTFile(pFile, &sResFile);
	sResFile.Close(&sResFile);
	return res;
}

bool BuildBase(tFile* pOldFile, tFile* pNewFile, tFile* pBaseFile)
{
	char oldd[0x100], newd[0x100], based[0x100];
	tFileSize size, read, bytes, i;
	tFilePos pos = 0;
	pNewFile->GetSize(pNewFile, &size);
	pOldFile->GetSize(pOldFile, &bytes);
	if (bytes != size)
		return false;
	pOldFile->Seek(pOldFile, 0, SEEK_SET, NULL);
	pNewFile->Seek(pNewFile, 0, SEEK_SET, NULL);
	pBaseFile->Seek(pBaseFile, 0, SEEK_SET, NULL);
	while (size)
	{
		read = min(size, sizeof(oldd));
		if (!pOldFile->Read(pOldFile, oldd, read, &bytes))
			return false;
		if (!pNewFile->Read(pNewFile, newd, read, &bytes))
			return false;
		pBaseFile->Read(pBaseFile, based, read, &bytes);
	    if (bytes < read)
		{
			memcpy(based+bytes, oldd+bytes, read-bytes);
			bytes = read;
		}
	    for (i=0; i<bytes; i++)
		{
			if (oldd[i] != newd[i])
				based[i] = 0x15;//oldd[i];
		}
		if (!pBaseFile->Seek(pBaseFile, pos, SEEK_SET, NULL))
			return false;
		if (!pBaseFile->Write(pBaseFile, based, read, &bytes))
			return false;
		pos += read;
		size -= read;
	}
	if (!pBaseFile->SetSize(pBaseFile, pos))
		return false;
	return true;
}

bool iAddDiffToSet(tHeap* pHeap, tFile* pDiff, tXML_DIFF_HDR* phdr, tFile* pDiffSet, tFile*pOldDiff, bool* pbOldDiff, tFile* pBase, unsigned long* pdiff_size, unsigned long diff_size_limit, bool* pbLimitReached)
{
	tFile sDiffIncr;
	tFile* pDiffToWrite = pDiff;
	bool bDiffIncrInited = false;
	tFileSize full_size, incr_size, diff_size, set_size;
	
	phdr->incremental = false;
	pDiff->GetSize(pDiff, &full_size);
	pDiffSet->GetSize(pDiffSet, &set_size);
	incr_size = full_size;
	if (*pbOldDiff) // old diff present
	{
		if (bDiffIncrInited = InitFileMemio(&sDiffIncr, pHeap))
		{
			if (iMakeDiff(pHeap, pOldDiff, pDiff, &sDiffIncr))
			{
				//iSave(&sDiffIncr, path, ".inc");
				sDiffIncr.GetSize(&sDiffIncr, &incr_size);
				if (incr_size < full_size)
				{
					pDiffToWrite = &sDiffIncr;
					phdr->incremental = true;
				}
			}
		}		
	}
	CopyTFile(pDiff, pOldDiff);
	*pbOldDiff = true;
	
	pDiffToWrite->GetSize(pDiffToWrite, &diff_size);
	phdr->size = diff_size;
	if (diff_size_limit && set_size + diff_size > diff_size_limit)
	{
		_Trace( 0, "Info: Reached limit of %d (data size=%d)\n", diff_size_limit, set_size + diff_size);
		if (pbLimitReached)
		{
			*pbLimitReached = true;
			return true;
		}
		return false;
	}

	if (!WriteDiffHeader(pDiffSet, phdr))
	{
		_Trace( 0, "Error: cannot write to output file\n");
		return false;
	}
	pDiffToWrite->Seek(pDiffToWrite, 0, SEEK_SET, NULL);
	if (!CopyData(pDiffToWrite, pDiffSet, phdr->size))
	{
		_Trace( 0, "Error: cannot write to output file\n");
		return false;
	}
	pDiffSet->GetSize(pDiffSet, pdiff_size);
	{
		char from_ver_s[0x20], to_ver_s[0x20];
		FileNameFromTime(phdr->from_ver, from_ver_s, sizeof(from_ver_s));
		FileNameFromTime(phdr->to_ver, to_ver_s, sizeof(to_ver_s));
		if (phdr->incremental)
			_Trace( 0, "Added incremental diff from version %s to version %s size %d (full size %d) set size=%d\n", from_ver_s, to_ver_s, phdr->size, full_size, *pdiff_size);
		else				
			_Trace( 0, "Added diff from version %s to version %s size %d\n", from_ver_s, to_ver_s, phdr->size);
	}
	pBase->SetSize(pBase, 0);
	if (bDiffIncrInited)
		sDiffIncr.Close(&sDiffIncr);
	return true;
}

void make_fullpath(char* full_path, char** full_path_fn, char* path, char* subfolder, bool directory)
{
	char* fn;
	int len;
	if (!_fullpath(full_path, path, _MAX_PATH))
        strcpy(full_path, path);
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
		{
			fn = full_path;
			if (!*fn)
			{
				*fn++ = '.';
				*fn = 0;
			}
		}
	}
	*full_path_fn = fn;
}

void Usage()
{
	_Trace( 0, "Usage: MAKEXMLD.EXE <new_file> <history_path> [diff_file] [diff_size_limit_bytes] [-log[:report.txt]]\n");
}

int __cdecl main(int argc, char* argv[])
{
	tHeap sHeap1;
	tFile sNewFile, sBackupFile, sOldFile, sDiff, sOldDiff, sDiffSet, sTestFile, sBase;

	bool bDiff = false;
	int   ver_count = 0;
	long  ver_arr[MAX_VER];	
	int i;
	struct _finddata_t fd;
	long hFind;
	time_t new_ver;
	tXML_DIFF_HDR hdr;
	long signature;
	bool bDiffOk, bOldDiff=false;
	unsigned long diff_size_limit = 0;
	unsigned long diff_size;
	bool bLimitReached = false;
	char hash[16];

	char src_path[_MAX_PATH*2] = "";
	char hist_path[_MAX_PATH*2] = "";
	char* hist_fn = hist_path;
	char res_path[_MAX_PATH*2] = "";
	char log_path[_MAX_PATH*2] = "";
	char* log_fn = log_path;

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
			
			if ((!stricmp(p, "log") || !stricmp(p, "report")))
				make_fullpath(log_path, &log_fn, p1 ? p1 : ".", 0, false);
			else if ((!stricmp(p, "?") || !stricmp(p, "h") || !stricmp(p, "help")))
			{
				Usage();
				return 0;
			}
			else
			{ 
				_Trace( 0, "Error: unknown parameter - %s\n", argv[i]);
				return -1;
			}
		}
		else if (!*src_path)
			make_fullpath(src_path, NULL, argv[i], 0, false);
		else if (!*hist_path)
			make_fullpath(hist_path, &hist_fn, argv[i], 0, true);
		else if (!*res_path)
			make_fullpath(res_path, NULL, argv[i], 0, false);
		else if (!diff_size_limit)
			diff_size_limit = atol(argv[i]);
		else
		{
			_Trace( 0, "Error: invalid parameter - %s\n", argv[i]);
			return -1;
		}
		
	}	

	if (*log_path)
	{
		struct _stat st;
		if (0 != _stat(log_path, &st))
		{
			// not exist yet
			char c;
			if (log_path[strlen(log_path)-1] == '\\') // directory specified
				make_fullpath(log_path, &log_fn, log_path, 0, true);
			else
				make_fullpath(log_path, &log_fn, log_path, 0, false);
			c = *log_fn;
			*log_fn = 0;
			i_mkdir(log_path);
			*log_fn = c;
		}

		if (log_path[strlen(log_path)-1] == '\\')
			log_path[strlen(log_path)-1] = 0;
		if (0 == _stat(log_path, &st))
		{
			if (st.st_mode & _S_IFDIR) // directory specified
			{
				struct tm* _tm;
				time_t _t;
				_t = time(0);
				_tm = localtime(&_t);
				make_fullpath(log_path, &log_fn, log_path, 0, true);
				sprintf(log_fn, "DIF-%04d%02d%02d.LOG", _tm->tm_year + 1900, _tm->tm_mon+1, _tm->tm_mday);
			}
		}

		g_output = fopen(log_path, "a+");
		
		if (!g_output)
		{
			g_output = stdout;
			_Trace( 0, "Error: cannot open report file - %s\n", argv[i]);
			return -1;
		}
		setvbuf(g_output,  NULL, _IONBF, 0 );
	}

	_Trace( 0, "\n");
	_Trace( 0, "------------------------------------------------------ MAKEXMLD v1.4 (" __TIMESTAMP__") ---\n");
	_Trace( 0, "\n");

	InitHeapRtl(&sHeap1);

	if (!src_path[0] || !hist_path[0])
	{
		Usage();
		return 1;
	}

	if (!InitFileStdioN(&sNewFile, src_path, "rb"))
	{
		_Trace( 0, "Error: cannot open new_file ('%s')\n", src_path);
		return 2;
	}

	new_ver = GetXMLTimeStamp(&sNewFile);
	if (!new_ver)
	{
		_Trace( 0, "Error: cannot retrive XML timestamp (UpdateDate field) from '%s'\n", src_path);
		return 3;
	}
	
	if (!GetFileHash(&sNewFile, hash))
	{
		_Trace( 0, "Error: cannot calc MD5 for '%s'\n", src_path);
		return 3;
	}


	i_mkdir(hist_path);

	// Make file history copy
	if (!FileNameFromTime(new_ver, hist_fn, _MAX_PATH))
	{
		_Trace( 0, "Error: cannot make history copy file name (name gen)\n");
		return 1;
	}
	strcat(hist_fn, ".xml");
	if (!InitFileStdioN(&sBackupFile, hist_path, "wb"))
	{
		_Trace( 0, "Error: cannot create history copy ('%s')\n", hist_path);
		return 2;
	}
	if (!CopyTFile(&sNewFile, &sBackupFile))
	{
		_Trace( 0, "Error: cannot write history copy file ('%s')\n", hist_path);
		return 2;
	}
	_Trace( 0, "history copy created: %s\n", hist_path);
	sBackupFile.Close(&sBackupFile);

	if (!res_path[0])
	{
		sNewFile.Close(&sNewFile);
		return 0;
	}

	if (!InitFileStdioN(&sDiffSet, res_path, "wb+"))
	{
		_Trace( 0, "Error: cannot open output file ('%s')\n", res_path);
		return 2;
	}	
	
	// header
	signature = DIF_SIGNATURE2;
	sDiffSet.Write(&sDiffSet, &signature, sizeof(signature), NULL);
	sDiffSet.Write(&sDiffSet, &new_ver, sizeof(new_ver), NULL);
	// extended header
	PutNum(&sDiffSet, sizeof(hash));
	sDiffSet.Write(&sDiffSet, hash, sizeof(hash), NULL);

	sDiffSet.GetSize(&sDiffSet, &diff_size);

	strcpy(hist_fn, "*.xml");
	hFind =_findfirst(hist_path, &fd);
	if (hFind != -1) 
	{
		do
		{
			time_t ver;
			strcpy(hist_fn, fd.name);
			ver = TimeFromFileName(fd.name);
			if (!ver || ver >= new_ver)
				continue;
			if (ver_count < MAX_VER)
				ver_arr[ver_count++] = ver;
			else
			{
				int i;
				time_t min_ver = 0;
				for (i=1; i<MAX_VER; i++)
				{
					if (ver_arr[i] < ver_arr[min_ver]) 
						min_ver = i;
				}
				ver_arr[min_ver] = ver;
			}
		} while (_findnext(hFind, &fd) == 0);
		_findclose(hFind);
	}
	qsort(ver_arr,ver_count,sizeof(ver_arr[0]), qsort_int_compare);
	i=ver_count;
	InitFileMemio(&sDiff, &sHeap1);
	InitFileMemio(&sOldDiff, &sHeap1);
	InitFileMemio(&sTestFile, &sHeap1);
	InitFileMemio(&sBase, &sHeap1);
	memset(&hdr, 0, sizeof(hdr));
	while(i)
	{
		tFileSize old_size;
		bDiffOk = false;
		i--;
		FileNameFromTime(ver_arr[i], hist_fn, _MAX_PATH);
		strcat(hist_fn, ".xml");
		if (!InitFileStdioN(&sOldFile, hist_path, "rb"))
		{
			_Trace( 0, "Error: cannot open old file ('%s')\n", hist_path);
			continue;
		}
		sOldFile.GetSize(&sOldFile, &old_size);
		if (!old_size)
		{
			sOldFile.Close(&sOldFile);
			continue;
		}
		if (bDiff)
		{
			sOldFile.Seek(&sOldFile, 0, SEEK_SET, NULL);
			sDiff.Seek(&sDiff, 0, SEEK_SET, NULL);
			sTestFile.SetSize(&sTestFile, 0);
			if (!ApplyXMLDiff(&sOldFile, &sDiff, &sTestFile))
			{
				bDiff = false;
				bDiffOk = false;
			}
			else
			{
				if (CompareTFile(&sNewFile, &sTestFile))
				{
					char to_ver_s[0x20];
					FileNameFromTime(ver_arr[i], to_ver_s, sizeof(to_ver_s));
					_Trace( 0, "Diff from version %s is the same\n", to_ver_s);
					BuildBase(&sOldFile, &sNewFile, &sBase);
					bDiffOk = true;
				}
				else
				{
					bDiff = false;
					if (BuildBase(&sOldFile, &sNewFile, &sBase))
					{
						tFile sDiff2;
						//iSave(&sBase, hist_path, ".base");
						if (InitFileMemio(&sDiff2, &sHeap1))
						{
							if (iMakeDiff(&sHeap1, &sBase, &sNewFile, &sDiff2))
							{
								tFile sTestFile2;
								//iSave(&sDiff2, path, ".dif3");
								sTestFile.Seek(&sTestFile, 0, SEEK_SET, NULL);
								sDiff2.Seek(&sDiff2, 0, SEEK_SET, NULL);
								sOldFile.Seek(&sOldFile, 0, SEEK_SET, NULL);
								if (InitFileMemio(&sTestFile2, &sHeap1))
								{
									if (ApplyXMLDiff(&sOldFile, &sDiff2, &sTestFile2))
									{
										if (CompareTFile(&sNewFile, &sTestFile2))
										{
											tFileSize size1, size2;
											sDiff.GetSize(&sDiff, &size1);
											sDiff2.GetSize(&sDiff2, &size2);
											if (!diff_size_limit || diff_size + size2 < diff_size_limit)
											{
//												if (size2 < size1*2)
												{
													//iSave(&sDiff2, path, ".dif_base");
													if (CopyTFile(&sDiff2, &sDiff))
													{
														char to_ver_s[0x20];
														FileNameFromTime(ver_arr[i], to_ver_s, sizeof(to_ver_s));
														_Trace( 0, "Diff from version %s is the same (rebased, new size %d)\n", to_ver_s, size2);
														bDiff = true;
														bDiffOk = true;
													}
												}
											}
										}
									}
									sTestFile2.Close(&sTestFile2);
								}
							}
							sDiff2.Close(&sDiff2);
						}
					}
				}
			}
		}



		if (!bDiffOk && hdr.from_ver)
		{
			if (!iAddDiffToSet(&sHeap1, &sDiff, &hdr, &sDiffSet, &sOldDiff, &bOldDiff, &sBase, &diff_size, diff_size_limit, &bLimitReached))
				return 4;
			if (bLimitReached)
			{
				hdr.from_ver = 0;
				break;
			}
		}

		hdr.to_ver = ver_arr[i];
		
		if (!bDiff)
		{
			hdr.from_ver = 0;
			if (!iMakeDiff(&sHeap1, &sOldFile, &sNewFile, &sDiff))
			{
				sOldFile.Close(&sOldFile);
				continue;
			}
			BuildBase(&sOldFile, &sNewFile, &sBase);
			//iSave(&sBase, hist_path, ".base");
			sDiff.GetSize(&sDiff, &hdr.size);
//			if (diff_size_limit && diff_size + hdr.size > diff_size_limit)
//				break;
			bDiff = true;
			hdr.from_ver = hdr.to_ver = ver_arr[i];
			{
				char from_ver_s[0x20];
				FileNameFromTime(hdr.from_ver, from_ver_s, sizeof(from_ver_s));
				_Trace( 0, "Generated diff from version %s size %d\n", from_ver_s, hdr.size);
				//iSave(&sDiff, path, ".dif");
			}
		}
		sOldFile.Close(&sOldFile);
	}

	if (hdr.from_ver)
	{
		if (!iAddDiffToSet(&sHeap1, &sDiff, &hdr, &sDiffSet, &sOldDiff, &bOldDiff, &sBase, &diff_size, diff_size_limit, &bLimitReached))
			return 4;
	}

	sOldDiff.Close(&sOldDiff);
	sDiffSet.Close(&sDiffSet);
	sDiff.Close(&sDiff);
	sTestFile.Close(&sTestFile);
	sNewFile.Close(&sNewFile);


	if (ver_count>VER_LIMIT)
	{
		_Trace( 0, "Versions limit reached - cleaning up %d files...\n", ver_count - VER_LIMIT);
		for (i=0; i< ver_count - VER_LIMIT; i++)
		{
			int res;
			FileNameFromTime(ver_arr[i], hist_fn, _MAX_PATH);
			strcat(hist_fn, ".xml");
			res = unlink(hist_path);
			_Trace( 0, "Delete %s res=%d\n", hist_path, res);
		}
	}

	//if (diff_size > sizeof(signature))
		_Trace( 0, "Total diff size %d\n", diff_size);
//	else
//	{
//		_Trace( 0, "Error: cannot find any appropriate file for diff or size limit too small.\n");
//		unlink(res_path);
//	}
	
	
	return 0;
}

