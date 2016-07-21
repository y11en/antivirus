//#define _CRTIMP
#ifdef _DEBUG
#include <stdio.h>
#endif

#include "xml_diff.h"
#include <time.h>
#include <string.h>
#include <memory.h>
#ifdef USE_FILE_TYPE_PRAGUE
#include <prague.h>
#include <pr_cpp.h>
#include <iface/i_root.h>
#include <plugin/p_hash_md5.h>
#else
#include <md5.h>
#endif

static char * _strstr (const char * str1, const char * str2)
{
	char *cp = (char *) str1;
	char *s1, *s2;
	if ( !*str2 )
		return((char *)str1);
	while (*cp)
	{
		s1 = cp;
		s2 = (char *) str2;
		while ( *s1 && *s2 && !(*s1-*s2) )
			s1++, s2++;
		if (!*s2)
			return(cp);
		cp++;
	}
	return(NULL);
}

static size_t GetSubstringPosBM(const unsigned char* Str, size_t lstrlen, const unsigned char* SubStr, size_t SubStrLen)
{
	unsigned char Table[0x100];
	int i;
	unsigned char *PStr,*PEndStr,*PSubStr,*PEndSubStr;
	unsigned char *PStrCur;
	if ((SubStrLen==0)||(lstrlen<SubStrLen)) return -1;
	memset(Table,SubStrLen,sizeof(Table));
	for(i=SubStrLen-2;i>=0;i--)
		if (Table[SubStr[i]]==SubStrLen)
			Table[SubStr[i]]=(unsigned char)(SubStrLen-i-1);
		PSubStr=PEndSubStr=(unsigned char*)SubStr+SubStrLen-1;
		PStrCur=PStr=(unsigned char*)Str+SubStrLen-1;
		PEndStr=(unsigned char*)Str+lstrlen;
		do {
			if((*PStr)==(*PSubStr)) {
				if(PSubStr==SubStr) return PStr-Str;
				PStr--;
				PSubStr--;
			} else {
				PSubStr=PEndSubStr;
				PStr=PStrCur+=Table[(*PStrCur)];
			}
		} while (PStr<PEndStr);
		return -1;
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

/*
#ifdef USE_FILE_TYPE_PRAGUE
#include <prague.h>
#include <iface/i_root.h>
#include <iface/e_ktime.h>
static _time_t pr_mktime(struct tm* tb)
{
	tDATETIME dt;
	static tERROR (pr_call * fDTSet)(tDT *dt, tDWORD year, tDWORD month, tDWORD day, tDWORD hour, tDWORD minute, tDWORD second, tDWORD ns ) = NULL;
	
	if (!g_root)
		return -1;
	if (NULL == fDTSet)
	{
		if (PR_FAIL(g_root->ResolveImportFunc( (tFUNC_PTR)&fDTSet, ECLSID_KERNEL, 0x0e262b4fl, PID_DIFF)))
			return -1;
	}
	
	if (PR_FAIL(fDTSet(&dt, tm.tm_year+1970, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, 0)))
		return -1;
}
#endif
*/

void time2tm(_time_t _t, struct tm* ptm)
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

_time_t tm2time(struct tm* ptm)
{
	unsigned long time = 0;
	time =  (unsigned long)ptm->tm_year * 12 * 31 * 24 * 60 * 60;
	time += (unsigned long)ptm->tm_mon  * 31 * 24 * 60 * 60;
	time += (unsigned long)ptm->tm_mday * 24 * 60 * 60;
	time += (unsigned long)ptm->tm_hour * 60 * 60;
	time += (unsigned long)ptm->tm_min  * 60;
	time += (unsigned long)ptm->tm_sec;
	return (_time_t)time;
}

_time_t GetXMLTimeStamp(tFile* pXMLFile)
{
	char buff[0x200];
	size_t bytes;
	char* timestamp;
	_time_t _time;
	struct tm t;
	tFilePos pos;
	int isdst = 0;
	
	if (!pXMLFile->Seek(pXMLFile, 0, SEEK_CUR, &pos))
		return 0;
	if (!pXMLFile->Seek(pXMLFile, 0, SEEK_SET, NULL))
		return 0;
	*buff = 0;
	pXMLFile->Read(pXMLFile, buff, sizeof(buff)-1, &bytes);
	pXMLFile->Seek(pXMLFile, pos, SEEK_SET, NULL);
	buff[bytes] = 0; // zero-terminate string
	timestamp = _strstr(buff, "UpdateDate=\"");
	if (!timestamp)
		return 0;
	timestamp+=12;

	if (strlen(timestamp) < 13)
		return 0;
	if (timestamp[13] != '\"')
		return 0;
	if (timestamp[8] != ' ')
		return 0;
	memset(&t, 0, sizeof(t));
	t.tm_mday = num_from_ascii(timestamp+0, 2);
	t.tm_mon  = num_from_ascii(timestamp+2, 2) - 1;
	t.tm_year = num_from_ascii(timestamp+4, 4) - 1970;
	t.tm_hour = num_from_ascii(timestamp+9, 2);
	t.tm_min  = num_from_ascii(timestamp+11, 2);
	_time = tm2time(&t);
	if (_time == (_time_t)-1)
		return 0;
#ifdef _PRAGUE_TRACE_
	{
		char ver_s[0x20];
		FileNameFromTime(_time, ver_s, sizeof(ver_s));
		PR_TRACE((NULL, prtNOTIFY, "diff\tGetXMLTimeStamp: %s, src:%s", ver_s, timestamp));
	}
#endif
	return (long)_time;
}

_time_t TimeFromFileName(char* fname)
{
    _time_t _t;
    struct tm t;
    int l;
	int isdst = 0;
    if (!fname)
        return 0;
    l = strlen(fname);
    if (l < 13) // 8 chars full date + delimiter + 4-6 chars for time
        return 0;
    if (fname[8] != ' ') // date/time delimiter
        return 0;
	memset(&t, 0, sizeof(t));
	t.tm_year = num_from_ascii(fname+0, 4) - 1970;
	t.tm_mon  = num_from_ascii(fname+4, 2) - 1;
	t.tm_mday = num_from_ascii(fname+6, 2);
	t.tm_hour = num_from_ascii(fname+9, 2);
	t.tm_min  = num_from_ascii(fname+11, 2);
	t.tm_sec  = num_from_ascii(fname+13, 2);
	t.tm_isdst = isdst;
	_t = tm2time(&t);
    if (_t == (_time_t)-1)
        return 0;
    return _t;
}


int FileNameFromTime(_time_t _t, char* fname, size_t fname_buff_size)
{
	struct tm t;
	struct tm* pt = &t;
	int l = 0;
	int year, div, d;
	if (!fname || !fname_buff_size)
		return 0;
	fname[0] = 0;
	if (fname_buff_size < 14)
		return 0;
	time2tm(_t, pt);
	year = pt->tm_year+1970;
	div = 1000;
	for (l=0;l<4; l++)
	{
		d = year/div;
		fname[l] = '0' + d;
		year -= d*div;
		div /= 10;
	}
	fname[4]  = '0' + ((pt->tm_mon+1) / 10);
	fname[5]  = '0' + ((pt->tm_mon+1) % 10);
	fname[6]  = '0' + (pt->tm_mday / 10);
	fname[7]  = '0' + (pt->tm_mday % 10);
	fname[8]  = ' ';
	fname[9]  = '0' + (pt->tm_hour / 10);
	fname[10] = '0' + (pt->tm_hour % 10);
	fname[11] = '0' + (pt->tm_min  / 10);
	fname[12] = '0' + (pt->tm_min  % 10);
	fname[13] = 0; 
	return 13;
}

bool ApplyXMLDiff(tFile* pOldFile, tFile* pDiffFile, tFile* pNewFile)
{
	size_t copy, cut, insert;
    {
        char buff[10000];
        tFileSize size;
        tFilePos pos;
        pDiffFile->Seek(pDiffFile, 0, SEEK_CUR, &pos);
        pDiffFile->Read(pDiffFile, buff, sizeof(buff), &size);
        pDiffFile->Seek(pDiffFile, pos, SEEK_SET, NULL);
        pDiffFile->GetSize(pDiffFile, &size);
    }
	do
	{
		if (!GetNum(pDiffFile, &copy))
			return false;
		if (!GetNum(pDiffFile, &cut))
			return false;
		if (!GetNum(pDiffFile, &insert))
			return false;
		if (copy)
		{
			if (!CopyData(pOldFile, pNewFile, copy))
				return false;
		}
		if (cut)
		{
			if (!pOldFile->Seek(pOldFile, cut, SEEK_CUR, NULL))
				return false;
		}
		if (insert)
		{
			if (!CopyData(pDiffFile, pNewFile, insert))
				return false;
		}
	} while(copy || cut || insert);
    {
        char buff[10000];
        tFileSize size;
        tFilePos pos;
        pDiffFile->Seek(pNewFile, 0, SEEK_CUR, &pos);
        pDiffFile->Seek(pNewFile, 0, SEEK_SET, NULL);
        pDiffFile->Read(pNewFile, buff, sizeof(buff), &size);
        pDiffFile->GetSize(pNewFile, &size);
        pDiffFile->Seek(pNewFile, pos, SEEK_CUR, NULL);
    }
	return true;
}

void prepstr(char* buf, char* str, size_t size, size_t strsize)
{
	char c;
	if (!size)
		return;
	do {
		c = *str++;
		if (c < 0x20) c='.';
		size--;
		if (size <= 3) c = '.';
		if (size == 0 || strsize == 0) c = 0;
		*buf++ = c;
		strsize--;
	} while(c && size);
}

bool MakeXMLDiff(const char* pOldFileData, size_t nOldFileSize, const char* pNewFileData, size_t nNewFileSize, tFile* pDiffFile)
{
	char* ofd = (char*)pOldFileData;
	size_t ofs = nOldFileSize;
	char* nfd = (char*)pNewFileData;
	size_t nfs = nNewFileSize;

	size_t ofp=0; // old file position
	size_t nfp=0; // new file position
	size_t copy, skip, insert; // bytes to copy, skip, insert
	size_t p, i, i1, step;

	while (nfp < nfs) // while not end of new file
	{
		copy=0;
		
		// copy equal part
		do 
		{
			if (ofp >= ofs) // end of old file reached
				break;
			if (ofd[ofp] != nfd[nfp])
				break;
			copy++;
			ofp++;
			nfp++;
		} while (nfp < nfs);

		//  when difference found try synchronize streams
		skip = 0; // ignore end of source file
		insert = nfs - nfp; // insert rest of new file
		step = 8; // size of substring to search, also step size
		for (i1=step; ; i1+=step)
		{
			i = i1;
			// search in new file for string from old
			if (ofp+i+step > ofs) // not enought data 
				break;
			p = GetSubstringPosBM(nfd+nfp, nfs-nfp, ofd+ofp+i, step);
			if (p != -1)
			{
				// match found
				while (p && i>i1-step && ofd[ofp+i-1]==nfd[nfp+p-1])
				{
					p--;
					i--;
				}
				if (i+p < skip+insert)
				{
					skip = i;
					insert = p;
				}
			}
			// search in old file for string from new file
			p = GetSubstringPosBM(ofd+ofp, ofs-ofp, nfd+nfp+i, step);
			if (p != -1)
			{
				// match found
				while (p && i>i1-step && ofd[ofp+p-1]==nfd[nfp+i-1])
				{
					p--;
					i--;
				}
				if (i+p < skip+insert)
				{
					insert = i;
					skip = p;
				}
			}
			if (skip+insert <= i)
				break;
		}
		if (!PutNum(pDiffFile, copy))
			return false;
		if (!PutNum(pDiffFile, skip))
			return false;
		if (!PutNum(pDiffFile, insert))
			return false;
		if (insert)
		{
			if (!PutData(pDiffFile, nfd+nfp, insert))
				return false;
		}	
#if defined(_DEBUG) && (0)
		{
			char buf[0x20];
			if (copy)
			{
				prepstr(buf, ofd+ofp-copy, sizeof(buf), copy);
				printf("copy\t%d\t%08x-%08x\t%s\n", copy, ofp-copy, ofp, buf);
			}
			if (skip)
			{
				prepstr(buf, ofd+ofp, sizeof(buf), skip);
				printf("skip\t%d\t%08x-%08x\t%s\n", skip, ofp, ofp+skip, buf);
			}
			if (insert)
			{
				prepstr(buf, nfd+nfp, sizeof(buf), insert);
				printf("insert\t%d\t%08x-%08x\t%s\n", insert, nfp, nfp+insert, buf);
			}
		}
#endif
		ofp += skip;
		nfp += insert;
	} // loop until end of new file

	// put <END OF DATA> mark
	if (!PutNum(pDiffFile, 0))
		return false;
	if (!PutNum(pDiffFile, 0))
		return false;
	if (!PutNum(pDiffFile, 0))
		return false;
	if (pDiffFile->Seek(pDiffFile, 0, SEEK_CUR, &p))
		pDiffFile->SetSize(pDiffFile, p);
	return true;
}

bool ApplyXmlDiffSet(tFile* pOldFile, tFile* pDiffFile, tFile* pResultFile, tFile* pTempFile, tFile* pTempFile2)
{
	tXML_DIFF_HDR hdr;
	long signature;
	tFilePos nCurPos, nLastNonIncrDiff=0;
	long old_ver = GetXMLTimeStamp(pOldFile);
	long dest_ver;
	char hash[16], hash2[16];
	long ext_hdr_len;
	
#ifdef _PRAGUE_TRACE_
	PR_TRACE((NULL, prtNOTIFY, "diff\tApplyXmlDiffSet"));
#endif
	if (!old_ver)
		return false;	
	if (!pDiffFile->Seek(pDiffFile, 0, SEEK_CUR, &nCurPos))
		return false;
	if (!pDiffFile->Read(pDiffFile, &signature, sizeof(signature), NULL))
		return false;
	switch (signature)
	{
	case DIF_SIGNATURE:
	case DIF_SIGNATURE2:
		break;
	default:
#ifdef _PRAGUE_TRACE_
	PR_TRACE((NULL, prtNOTIFY, "diff\tApplyXmlDiffSet signature incompatible"));
#endif
		pDiffFile->Seek(pDiffFile, nCurPos, SEEK_SET, NULL);
		return false;
	}
	if (!pDiffFile->Read(pDiffFile, &dest_ver, sizeof(dest_ver), NULL))
		return false;

	if (signature == DIF_SIGNATURE2)
	{
		tFilePos pos;
		if (!GetNum(pDiffFile, &ext_hdr_len))
			return false;
		if (!pDiffFile->Seek(pDiffFile, 0, SEEK_CUR, &pos))
			return false;
		if (!pDiffFile->Read(pDiffFile, &hash, sizeof(hash), NULL))
			return false;
		if (!pDiffFile->Seek(pDiffFile, pos+ext_hdr_len, SEEK_SET, NULL))
			return false;
	}
	
#ifdef _PRAGUE_TRACE_
	{
		char dest_ver_s[0x20], old_ver_s[0x20];
		FileNameFromTime(dest_ver, dest_ver_s, sizeof(dest_ver_s));
		FileNameFromTime(old_ver, old_ver_s, sizeof(old_ver_s));
		PR_TRACE((NULL, prtNOTIFY, "diff\tApplyXmlDiffSet old_ver=%s dest_ver=%s", old_ver_s, dest_ver_s));
	}
#endif
	if (old_ver == dest_ver)
	{
		// already latest version
		if (!CopyTFile(pOldFile, pResultFile))
			return false;
	}
	else
	{
		//nCurPos += sizeof(signature) + sizeof(dest_ver);
		if (!pDiffFile->Seek(pDiffFile, 0, SEEK_CUR, &nCurPos))
			return false;
		do
		{
			if (!ReadDiffHeader(pDiffFile, &hdr))
				return false;
//			if (hdr.from_ver == 0 && hdr.to_ver == 0) // extended header
//			{
//			}
			if (hdr.size == 0)
				return false;
			if (!hdr.incremental)
				nLastNonIncrDiff = nCurPos;
			if (hdr.from_ver > hdr.to_ver)
			{
				unsigned long tmp = hdr.from_ver;
				hdr.from_ver = hdr.to_ver;
				hdr.to_ver = tmp;
			}
#ifdef _PRAGUE_TRACE_
		{
			char from_ver_s[0x20], to_ver_s[0x20], old_ver_s[0x20];
			FileNameFromTime(hdr.from_ver, from_ver_s, sizeof(from_ver_s));
			FileNameFromTime(hdr.to_ver, to_ver_s, sizeof(to_ver_s));
			FileNameFromTime(old_ver, old_ver_s, sizeof(old_ver_s));
			PR_TRACE((NULL, prtNOTIFY, "diff\tApplyXmlDiffSet from_ver=%s to_ver=%s old_ver=%s", from_ver_s, to_ver_s, old_ver_s));
		}
#endif
			if (old_ver >= hdr.from_ver && old_ver <= hdr.to_ver)
			{
				// diff found
				break;
			}
			if (!pDiffFile->Seek(pDiffFile, hdr.size, SEEK_CUR, &nCurPos))
				return false;
		} while (1);
		
		// diff found
#ifdef _PRAGUE_TRACE_
	PR_TRACE((NULL, prtNOTIFY, "diff\tApplyXmlDiffSet: diff found"));
#endif
		if (hdr.incremental)
		{
			tFilePos nLastPos = nCurPos;
			if (!nLastNonIncrDiff)
				return false;
			if (!pDiffFile->Seek(pDiffFile, nLastNonIncrDiff, SEEK_SET, NULL))
				return false;
			if (!ReadDiffHeader(pDiffFile, &hdr))
				return false;
			if (!pTempFile->SetSize(pTempFile, 0))
				return false;
			if (!CopyData(pDiffFile, pTempFile, hdr.size))
				return false;
			if (!pDiffFile->Seek(pDiffFile, 0, SEEK_CUR, &nCurPos))
				return false;
			do 
			{
				tFile* tmp;
				if (nCurPos > nLastPos)
					return false;
				if (!ReadDiffHeader(pDiffFile, &hdr))
					return false;
				if (!pTempFile->Seek(pTempFile, 0, SEEK_SET, NULL))
					return false;
				if (!pTempFile2->SetSize(pTempFile2, 0))
					return false;
				// apply diff on diff
				if (!ApplyXMLDiff(pTempFile, pDiffFile, pTempFile2))
					return false;
				// swap temp files
				tmp = pTempFile;
				pTempFile = pTempFile2;
				pTempFile2 = tmp;
				if (!pDiffFile->Seek(pDiffFile, 0, SEEK_CUR, &nCurPos))
					return false;
			} while (nCurPos <= nLastPos);
			// full diff now in pTempFile
			if (!pTempFile->Seek(pTempFile, 0, SEEK_SET, NULL))
				return false;
			pDiffFile = pTempFile;
		}
		if (!ApplyXMLDiff(pOldFile, pDiffFile, pResultFile))
			return false;
	}
	if (signature == DIF_SIGNATURE2)
	{
		if (!GetFileHash(pResultFile, hash2))
			return false;
		if (0 != memcmp(hash, hash2, sizeof(hash)))
			return false;
	}
	return true;
}

bool ReadDiffHeader(tFile* pFile, tXML_DIFF_HDR* phdr)
{
	if (!pFile->Read(pFile, &phdr->from_ver, sizeof(phdr->from_ver), NULL))
		return false;
	if (!pFile->Read(pFile, &phdr->to_ver, sizeof(phdr->to_ver), NULL))
		return false;
	if (!GetNum(pFile, &phdr->size))
		return false;
	phdr->incremental = 0;
	if (!pFile->Read(pFile, &phdr->incremental, 1, NULL))
		return false;
	return true;
}

bool WriteDiffHeader(tFile* pFile, tXML_DIFF_HDR* phdr)
{
	if (!pFile->Write(pFile, &phdr->from_ver, sizeof(phdr->from_ver), NULL))
		return false;
	if (!pFile->Write(pFile, &phdr->to_ver, sizeof(phdr->to_ver), NULL))
		return false;
	if (!PutNum(pFile, phdr->size))
		return false;
	if (!pFile->Write(pFile, &phdr->incremental, 1, NULL))
		return false;
	return true;
}

#ifdef USE_FILE_TYPE_PRAGUE

bool GetFileHash(tFile* pFile, char hash[16])
{
	char buffer[0x200];
	size_t bytes;
	bool res = true;
	hHASH hMD5;
	if (PR_FAIL(CALL_SYS_ObjectCreateQuick(g_root, (hOBJECT*)&hMD5, IID_HASH, PID_HASH_MD5, SUBTYPE_ANY)))
		return false;
	if (!pFile->Seek(pFile, 0, SEEK_SET, NULL))
		res = false;
	else
	{
		do {
			pFile->Read(pFile, buffer, sizeof(buffer), &bytes);
			if (!bytes)
				break;
			if (PR_FAIL(CALL_Hash_Update(hMD5, buffer, bytes)))
			{
				res = false;
				break;
			}
		} while(bytes);
		if (PR_FAIL(CALL_Hash_GetHash(hMD5, hash, 16)))
			res = false;
	}
	if (!pFile->Seek(pFile, 0, SEEK_SET, NULL))
		res = false;
	CALL_SYS_ObjectClose(hMD5);
	return res;
}

#else // !USE_FILE_TYPE_PRAGUE

bool GetFileHash(tFile* pFile, char hash[16])
{
	char buffer[0x200];
	size_t bytes;
	md5_state_t md5state;
	md5_init(&md5state);
	if (!pFile->Seek(pFile, 0, SEEK_SET, NULL))
		return false;
	do {
		pFile->Read(pFile, buffer, sizeof(buffer), &bytes);
		if (!bytes)
			break;
		md5_append(&md5state, buffer, bytes);
	} while(bytes);
	md5_finish(&md5state, hash);
	if (!pFile->Seek(pFile, 0, SEEK_SET, NULL))
		return false;
	return true;
}

#endif // USE_FILE_TYPE_PRAGUE
