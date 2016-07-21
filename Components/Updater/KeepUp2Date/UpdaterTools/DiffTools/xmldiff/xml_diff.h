#ifndef __XML_DIFF_H__
#define __XML_DIFF_H__

#include "file_al.h"
#include "heap_al.h"

#define DIF_SIGNATURE  0x31444C4B // KLD1
#define DIF_SIGNATURE2 0x32444C4B // KLD2

//#ifndef _TIME_T_DEFINED
typedef long _time_t;
//#define _TIME_T_DEFINED
//#endif

typedef struct tag_XML_DIFF_HDR {
	long from_ver;
	long to_ver;
	unsigned long size;
	char incremental;
} tXML_DIFF_HDR;

_time_t GetXMLTimeStamp(tFile* pXMLFile);

_time_t TimeFromFileName(char* fname);
int FileNameFromTime(_time_t _t, char* fname, size_t fname_buff_size);

bool MakeXMLDiff(const char* pOldFileData, size_t nOldFileSize, const char* pNewFileData, size_t nNewFileSize, tFile* pDiffFile);
bool ApplyXMLDiff(tFile* pOldFile, tFile* pDiffFile, tFile* pNewFile);
bool ApplyXmlDiffSet(tFile* pOldFile, tFile* pDiffFile, tFile* pResultFile, tFile* pTempFile, tFile* pTempFile2);
bool ReadDiffHeader(tFile* pFile, tXML_DIFF_HDR* phdr);
bool WriteDiffHeader(tFile* pFile, tXML_DIFF_HDR* phdr);
bool GetFileHash(tFile* pFile, char hash[16]);

#endif // __XML_DIFF_H__