/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	jrnl/journalimpl.cpp
 * \author	Mikhail Karmazine
 * \date	22.01.2003 12:02:39
 * \brief	Реализация интерфейса KLJRNL::Journal
 * 
 */

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __unix__
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include "osdep.h"
#include "journalimpl.h"
#include <std/conv/klconv.h>
#include <std/err/klerrors.h>
#include <std/trc/trace.h>
#include <set>


#define KLCS_MODULENAME L"KLJRNL"

IMPLEMENT_MODULE_INIT_DEINIT(KLJRNL)

IMPLEMENT_MODULE_INIT_BEGIN2( KLCSKCA_DECL, KLJRNL)
	KLERR_InitModuleDescriptions(
		KLCS_MODULENAME,
		KLJRNL::c_errorDescriptions,
		KLSTD_COUNTOF(KLJRNL::c_errorDescriptions));
IMPLEMENT_MODULE_INIT_END()

IMPLEMENT_MODULE_DEINIT_BEGIN2( KLCSKCA_DECL, KLJRNL)
	KLERR_DeinitModuleDescriptions( KLCS_MODULENAME );
IMPLEMENT_MODULE_DEINIT_END()

const int OPEN_SHARED_CENTRAL_FILE_TIMEOUT = 10000;

// format of text representation of rec size is "%s %20ld%s", where 
// parameters are TEXT_REPR_REC_POS, int, JRNL_DELIM.
#define TEXT_MODE_IDX_REC_SIZE (sizeof(TEXT_REPR_REC_POS) - 1 + 1 + 20 + sizeof(JRNL_DELIM) - 1)

using namespace KLJRNL;
using namespace KLPAR;

// two delimeters shall not be encountered in other fields (except data field itself)
#define JRNL_DELIM_RECORD_BEGIN   "<jrnl record>\r\n"
#define JRNL_DELIM_RECORD_END     "</jrnl record>\r\n"

#define REC_BEGIN_SIZE      (KLSTD_COUNTOF(JRNL_DELIM_RECORD_BEGIN))
#define REC_END_SIZE        (KLSTD_COUNTOF(JRNL_DELIM_RECORD_END))

#define READ_BUFFER_SIZE 2048

#define CENTRAL_FILE_SIGNATURE  "KLJRNL central file"
#define CENTRAL_FILE_ID_SIGNATURE  "KLJRNL central file id"
#define PAGE_FILE_SIGNATURE     "KLJRNL page file"
#define BINARY_MODE_FORMAT_FLAG  "binary"
#define TEXT_MODE_FORMAT_FLAG  "text"

#ifndef TCHAR
    #if defined(UNICODE) || defined(_UNICODE)
        #define TCHAR wchar_t
    #else
        #define TCHAR char
    #endif
#endif

#ifdef N_PLAT_NLM
 #define OPEN_MODE_FILE_READ "rb"
 #define OPEN_MODE_FILE_UPDATE "r+b"
#else
 #define OPEN_MODE_FILE_READ "r"
 #define OPEN_MODE_FILE_UPDATE "r+"
#endif

void DuplicateSubstr(std::string& str, const char* pcszSubstr)
{
    int nSubstrLen = strlen(pcszSubstr);

    int nIndex = 0;
    while(std::string::npos != (nIndex = str.find(pcszSubstr, nIndex)))
    {
        str.insert(nIndex, pcszSubstr);
        nIndex += nSubstrLen * 2;
    }
}

int FindNonDuplicated(int nStartPos, std::string& str, const char* pcszSubstr)
{
    if (nStartPos < 0) nStartPos = 0;

    int nSubstrLen = strlen(pcszSubstr);
    int nIndex = nStartPos;
    while(std::string::npos != (nIndex = str.find(pcszSubstr, nIndex)))
    {
        int nIndexNext = str.find(pcszSubstr, nIndex + nSubstrLen);
        if (nIndexNext == nIndex + nSubstrLen)
            nIndex = nIndex +  2 * nSubstrLen;
        else
            break;
    }

    return nIndex;
}

void RemoveDuplicates(std::string& str, const char* pcszSubstr)
{
    int nSubstrLen = strlen(pcszSubstr);
    int nIndex = 0;
    while(std::string::npos != (nIndex = str.find(pcszSubstr, nIndex)))
    {
        int nIndexNext = str.find(pcszSubstr, nIndex + nSubstrLen);
        if (nIndexNext != nIndex + nSubstrLen)
        {
            nIndex += nSubstrLen;
        } 
		else
        {
            str.erase(nIndexNext, nSubstrLen);
            nIndex = nIndexNext;
        }
    }
}

////////////////////////////////////////////////////////////////////
// JournalImpl

JournalImpl::JournalImpl(bool bMultiThread):
    m_fileCentral(NULL),
    m_nInvalidState(0),
	m_bCtrlFileNotCreated(false)
{
    if (bMultiThread) KLSTD_CreateCriticalSection(& m_pCS);
}

JournalImpl::~JournalImpl()
{
	KLERR_BEGIN
	{
		KLSTD::AutoCriticalSection acs(m_pCS);
		if (IsOpenedInternal()) CloseInternal();
	}
	KLERR_ENDT(1)
}

std::wstring JournalImpl::GetCentralFile(const std::wstring & wstrTargetJournalFile)
{
    std::wstring wstrDir, wstrFileName, wstrFileExt;
    KLSTD_SplitPath(wstrTargetJournalFile, wstrDir, wstrFileName, wstrFileExt);
    std::wstring wstrResult;
    KLSTD_PathAppend(wstrDir, wstrFileName + L".ctrl", wstrResult, false);
    return wstrResult;
}

void _DeleteLostFiles(const std::wstring& wstrTemplateFilePath)
{
    std::wstring wstrTemplFileDir, wstrTemplFileName, wstrTemplFileExt;
    KLSTD_SplitPath(wstrTemplateFilePath, wstrTemplFileDir, wstrTemplFileName, wstrTemplFileExt);

    std::wstring wstrFilesMaskPath, wstrFilesMask = wstrTemplFileName + L"*" + wstrTemplFileExt;
    KLSTD_PathAppend(wstrTemplFileDir, wstrFilesMask, wstrFilesMaskPath, true);

#ifdef _WIN32

	WIN32_FIND_DATA wfd;
	HANDLE hSearch = FindFirstFile(KLSTD_W2CT2(wstrFilesMaskPath.c_str()), &wfd);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (wfd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY) 
				KLSTD_Unlink((wstrTemplFileDir + (const wchar_t*)KLSTD_T2W2(wfd.cFileName)).c_str(), false);
		} 
		while(FindNextFile(hSearch, &wfd));

		FindClose(hSearch);
	}
#elif defined(__unix__)

	DIR *dirDesc = opendir( KLSTD_W2A2(wstrFilesMaskPath.c_str()) );
	if ( dirDesc!=NULL ) 
	{
		struct dirent *dp;
		while( dp = readdir(dirDesc)  )
		{
			DIR *subDesc = opendir( dp->d_name );
			if ( subDesc!=NULL )
			{
				closedir( subDesc );
				continue;
			}
			unlink( dp->d_name );
		}
		
		closedir( dirDesc );		
	}
#else
	#warning Implemented only for Win32 and unix!!!
#endif
}

void JournalImpl::Open(const std::wstring &wstrPath,
					   KLSTD::CREATION_FLAGS flagsCreation,
					   KLSTD::ACCESS_FLAGS flagsAccessFlags,
					   const CreationInfo &creationInfo, /* = CreationInfo() */
					   long nTimeout /* = KLJRNL_DEF_OPEN_TIMEOUT */)
{
    KLSTD::AutoCriticalSection acs(m_pCS);

    m_bChangeLimits = false;
	m_wstrCentralFilePath = GetCentralFile(wstrPath);

    if ((flagsCreation & KLSTD::CF_CLEAR) == KLSTD::CF_CLEAR)
    {
		_DeleteLostFiles(wstrPath);
    }
    OpenInternal(wstrPath, flagsCreation, flagsAccessFlags, creationInfo, nTimeout);
}

void JournalImpl::GetUsedCreationInfo(CreationInfo & creationInfo)
{
    if (!IsOpenedInternal()) KLERR_MYTHROW0(ERR_CLOSED);
    creationInfo = m_creationInfo;
}

void JournalImpl::OpenInternal(const std::wstring &wstrPath,
							   KLSTD::CREATION_FLAGS flagsCreation,
							   KLSTD::ACCESS_FLAGS flagsAccessFlags,
							   const CreationInfo &creationInfo,
							   long nTimeout)
{
    KLSTD::AutoCriticalSection acs(m_pCS);

    m_bChangingState = false;
    m_nCurrentPage = -1;
	m_creationInfo = creationInfo;

    if (IsOpenedInternal()) KLERR_MYTHROW0(ERR_OPENED);

    m_wstrPageFileTempl = wstrPath;
    m_wstrCentralFilePath = GetCentralFile(wstrPath);
    if (wstrPath == m_wstrCentralFilePath) KLERR_MYTHROW1(ERR_INVALID_JOURNAL_FILE_NAME1, wstrPath.c_str());

	m_bCtrlFileNotCreated = false;
	KLERR_TRY
	{
		OpenCentralFile(flagsCreation, flagsAccessFlags, nTimeout);
	}
	KLERR_CATCH(pError)
	{
		if (flagsAccessFlags == KLSTD::AF_READ &&
			(pError->GetId() == KLSTD::STDE_NOENT || pError->GetId() == KLSTD::STDE_NOTFOUND))
		{
			m_bCtrlFileNotCreated = true;
			return;
		}
		KLERR_RETHROW();
	}
	KLERR_ENDTRY

	try
	{
		if (CheckCentralFileSignature())
		{
			ReadCentralFileData();
			if (flagsCreation & KLSTD::CF_CLEAR) DeleteAllInternal();
		} 
		else
		{
			if (flagsAccessFlags & KLSTD::AF_WRITE)
				CreateNewCentralFileData(creationInfo);
		}
	}
	catch(...)
	{
		CloseNoThrow();
		throw;
	}
}

void JournalImpl::CloseNoThrow()
{
    KLERR_BEGIN
	{
		if (IsOpenedInternal()) CloseInternal();
	}
    KLERR_END;
}

void JournalImpl::CreateNewCentralFileData(const CreationInfo & creationInfo)
{
    bool bError = false;

    if ((creationInfo.pageLimitType == pltRecCount) && creationInfo.nMaxRecordsInPage < 1)
        KLERR_MYTHROW1(ERR_MAX_PAGE_REC_COUNT1, m_wstrCentralFilePath.c_str());

    // set cached values (some values shall be cleared by Close():
    m_creationInfo = creationInfo;

    const char* pcszBinaryOrTextFormat = m_creationInfo.bBinaryFormat?BINARY_MODE_FORMAT_FLAG:TEXT_MODE_FORMAT_FLAG;

    bError = bError || (fputs(CENTRAL_FILE_SIGNATURE, m_fileCentral) == EOF);
    bError = bError || (fputs(JRNL_DELIM, m_fileCentral) == EOF);
    bError = bError || (fputs(pcszBinaryOrTextFormat, m_fileCentral) == EOF);
    bError = bError || (fputs(JRNL_DELIM, m_fileCentral) == EOF);
    WriteInt32(JRNL_VERSION, m_fileCentral, TEXT_REPR_VERSION, bError);
    
    WriteInt32(long(m_creationInfo.pageLimitType), m_fileCentral, TEXT_REPR_PAGE_LIMIT_TYPE, bError);
    if (m_creationInfo.pageLimitType == pltRecCount)
        WriteInt32(m_creationInfo.nMaxRecordsInPage, m_fileCentral, TEXT_REPR_MAX_REC_IN_PAGE, bError);
	else
        WriteInt32(m_creationInfo.nMaxPageSize, m_fileCentral, TEXT_REPR_MAX_PAGE_SIZE, bError);
    WriteFlag(m_creationInfo.bLimitPages, m_fileCentral, TEXT_REPR_LIMIT_PAGES, bError);
    WriteFlag(m_creationInfo.bUseRecNumIndex, m_fileCentral, TEXT_REPR_USE_REC_NUM_INDEX, bError);

    long nCentralChangingFlagPos = ReadPosition(m_fileCentral, bError);
    WriteFlag(false, m_fileCentral, TEXT_REPR_CHANGING, bError);

    long nIdentity = 0;
    long nIdentityPos = ReadPosition(m_fileCentral, bError);
    WriteInt32(nIdentity, m_fileCentral, TEXT_REPR_IDENTITY, bError);

    long nFinishRecEndPos = ReadPosition(m_fileCentral, bError);
    SetCachedData(nCentralChangingFlagPos, nFinishRecEndPos, nIdentityPos, nIdentity);

    if (bError)
    {
        std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
        KLERR_MYTHROW2(ERR_CANT_WRITE2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str());
    }
	
	CreateCentralFileIdFile(m_wstrCentralFilePath);
}

void JournalImpl::CreateCentralFileIdFile(const std::wstring& wstrCentralFilePath)
{
	KLSTD::CAutoPtr<KLSTD::File> pFile;
	KLSTD_CreateFile(wstrCentralFilePath + CTRL_FILE_ID_EXT, 0, KLSTD::CF_CREATE_ALWAYS, KLSTD::AF_WRITE, 0, &pFile, 5000);

	std::ostringstream s;
	s << CENTRAL_FILE_ID_SIGNATURE << JRNL_DELIM << std::string(KLSTD_W2A2(KLSTD_CreateGUIDString().c_str())).c_str();

	pFile->Write(s.str().c_str(), s.str().length());
}

std::string JournalImpl::ReadCentralFileIdFile(const std::wstring& wstrCentralFilePath)
{
	const int BUF_SIZE = 40;

	KLERR_BEGIN
	{
		KLSTD::CAutoPtr<KLSTD::File> pFile;
		KLSTD_CreateFile(wstrCentralFilePath + CTRL_FILE_ID_EXT, KLSTD::SF_READ, KLSTD::CF_OPEN_EXISTING, KLSTD::AF_READ, 0, &pFile, 5000);

		pFile->Seek(sizeof(CENTRAL_FILE_ID_SIGNATURE) + sizeof(JRNL_DELIM), KLSTD::ST_SET);

		char szBuffer[BUF_SIZE];
		KLSTD_ZEROSTRUCT(szBuffer);

		pFile->Read(szBuffer, BUF_SIZE);

		return szBuffer;
	}
	KLERR_ENDT(1)

	return "not_supported";
}

void JournalImpl::ReadCentralFileData()
{
    bool bError = false;
    bool bBadValue = false;

    char szBuf[STATIC_BUF_LEN];

    bError = bError || (fgets_bin_win32(szBuf, STATIC_BUF_LEN, m_fileCentral) == NULL);
    if (!bError) m_creationInfo.bBinaryFormat = (strcmp(szBuf, BINARY_MODE_FORMAT_FLAG) == 0);

    long nJrnlVersion = ReadInt32(m_fileCentral, TEXT_REPR_VERSION, bError, bBadValue);
    if (!bError && nJrnlVersion > JRNL_VERSION) KLERR_MYTHROW1(ERR_UNSOPPORTED_VERSION1, m_wstrCentralFilePath.c_str());

    m_creationInfo.pageLimitType = PageLimitType(ReadInt32(m_fileCentral, TEXT_REPR_PAGE_LIMIT_TYPE, bError, bBadValue));
    if (m_creationInfo.pageLimitType == pltRecCount)
		ReadInt32(m_fileCentral, TEXT_REPR_MAX_REC_IN_PAGE, bError, bBadValue);
    else
		ReadInt32(m_fileCentral, TEXT_REPR_MAX_PAGE_SIZE, bError, bBadValue);

	ReadFlag(m_fileCentral, TEXT_REPR_LIMIT_PAGES, bError, bBadValue);

    m_creationInfo.bUseRecNumIndex = ReadFlag(m_fileCentral, TEXT_REPR_USE_REC_NUM_INDEX, bError, bBadValue);

    long nCentralChangingFlagPos = ReadPosition(m_fileCentral, bError);
    bool bChanging = ReadFlag(m_fileCentral, TEXT_REPR_CHANGING, bError, bBadValue);
    long nIdentityPos = ReadPosition(m_fileCentral, bError);
    long nIdentity = ReadInt32(m_fileCentral, TEXT_REPR_IDENTITY, bError, bBadValue);
    if (!bError && bChanging && m_bChangeAllowed) KLERR_MYTHROW0(ERR_CORRUPTED);

    long nFinishRecEnd = ReadPosition(m_fileCentral, bError);
    bool bStopRead = false;
    while(!bError && !bStopRead)
    {
        bool bFileMarkedAsDeleted, bFilePhysDeleted, bValidRecord;
        long nMarkedAsDeletedFlagPos, nPhysDeletedFlagPos, nFileNamePos;
        std::wstring wstrPageFileName, wstrIdxFileName;

        bValidRecord = false;

        if (!bError)
        {
            nMarkedAsDeletedFlagPos = ReadPosition(m_fileCentral, bError);
            bFileMarkedAsDeleted = ReadFlag(m_fileCentral, TEXT_REPR_FILE_MARKED_AS_DELETED, bError, bBadValue);

            // stop read if no record at all - eof encountered.
            bStopRead = bError;
            if (bError && feof(m_fileCentral)) bError = false;
        }

		std::wstring wstrAddOn;
        if (!bError && !bStopRead)
        {
            nPhysDeletedFlagPos = ReadPosition(m_fileCentral, bError);
            bFilePhysDeleted = ReadFlag(m_fileCentral, TEXT_REPR_FILE_PHYS_DELETED, bError, bBadValue);

            char szBuf[STATIC_BUF_LEN];
            bool bDelimFound = false;
            nFileNamePos = ReadPosition(m_fileCentral, bError);
            bError = bError || (fgets_bin_win32(szBuf, STATIC_BUF_LEN, m_fileCentral, & bDelimFound) == NULL);
            if (!bError && bDelimFound)
            {
                wstrAddOn = (const wchar_t*)KLSTD_A2CW2(szBuf);
                wstrPageFileName = GetPageFilePathFromAddOn(wstrAddOn);

                if (! m_creationInfo.bUseRecNumIndex) // if use recnum index, then check whether record is valid later.
                    bValidRecord = true;
            }

            if (m_creationInfo.bUseRecNumIndex)
            {
                bDelimFound = false;
                bError = bError || (fgets_bin_win32(szBuf, STATIC_BUF_LEN, m_fileCentral, & bDelimFound) == NULL);
                if (!bError && bDelimFound)
                {
                    wstrAddOn = (const wchar_t*)KLSTD_A2CW2(szBuf);
                    wstrIdxFileName = GetPageFilePathFromAddOn(wstrAddOn);

                    bValidRecord = true;
                }
            }

            if (feof(m_fileCentral)) bError = false;
        }

        if (bError)
        {
            bStopRead = true;
            // if we encountered eof too soon, then set bBadValue to true
            bBadValue = bBadValue || (feof(m_fileCentral) != 0);
        } 
		else
        {
            if (bValidRecord)
            {
                nFinishRecEnd = ReadPosition(m_fileCentral, bError);

                Page page(wstrPageFileName, wstrIdxFileName, nMarkedAsDeletedFlagPos, nPhysDeletedFlagPos, nFileNamePos, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, false);
                if (bFileMarkedAsDeleted)
                {
                    if (!bFilePhysDeleted)
                    {
                        if (m_bChangeAllowed) TryDeletePagePhysically(page, bError);
                        bValidRecord = ! bError;
                    }
                } 
				else
                {
                    m_vectPages.push_back(page);
					OpenPage(m_vectPages.size() - 1);
					if (!m_vectPages.back().file) m_vectPages.pop_back();
                }
            }
            bStopRead = bStopRead || ! bValidRecord;
        }
    }

    if (bError)
    {
        std::wstring wstrErrorDescr;
        if (bBadValue || feof(m_fileCentral))
            wstrErrorDescr = L"not all of necessary values can be read";
		else
            wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
        KLERR_MYTHROW2(ERR_CANT_READ2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str());
    }

   SetCachedData(nCentralChangingFlagPos, nFinishRecEnd, nIdentityPos, nIdentity);
}

FILE* JournalImpl::_OpenFile(const std::wstring& wstrFilePath, 
							 KLSTD::CREATION_FLAGS flagsCreation, 
							 KLSTD::ACCESS_FLAGS flagsAccessFlags, 
							 long nTimeout)
{
	FILE* fileHandle = NULL;

    // Today I know only about these flags, so remove other bits so as not to disturb us:
    flagsCreation = KLSTD::CREATION_FLAGS(flagsCreation & (KLSTD::CF_OPEN_EXISTING | KLSTD::CF_CREATE_NEW | KLSTD::CF_CLEAR));
    if (!flagsCreation) KLSTD_THROW_BADPARAM(flagsCreation);
    if (!flagsAccessFlags) KLSTD_THROW_BADPARAM(flagsAccessFlags);

    KLSTD::SHARE_FLAGS shareFlags;
    if (flagsAccessFlags == KLSTD::AF_READ)
    {
       shareFlags = KLSTD::SHARE_FLAGS(KLSTD::SF_READ | KLSTD::SF_WRITE);
    } 
	else 
	if (flagsAccessFlags & KLSTD::AF_WRITE)
    {
        shareFlags = KLSTD::SF_READ;
        flagsAccessFlags = KLSTD::ACCESS_FLAGS(flagsAccessFlags | KLSTD::AF_READ);
    } 
	else  // unknown combination of flagsAccessFlags
        KLSTD_ASSERT(0);

	if ((flagsCreation & KLSTD::CF_CREATE_NEW) && ! (flagsCreation & KLSTD::CF_OPEN_EXISTING))
	{
		nTimeout = 0;
		shareFlags = KLSTD::SHARE_FLAGS(0);
	} 
	else
		nTimeout = OPEN_SHARED_CENTRAL_FILE_TIMEOUT;	// 10 секунд.

    KLSTD::CAutoPtr<KLSTD::File> pFile;
    KLSTD_CreateFile(wstrFilePath, shareFlags, flagsCreation, flagsAccessFlags, KLSTD::EXTRA_FLAGS(KLSTD::EF_RANDOM_ACCESS | KLSTD::EF_DONT_WORRY_ABOUT_CLOSE), &pFile, nTimeout);
    int nFileId = pFile->GetFileID();
    if (flagsAccessFlags == KLSTD::AF_READ)
    {
        fileHandle = fdopen(nFileId, OPEN_MODE_FILE_READ);
    } 
	else 
	if (flagsAccessFlags & KLSTD::AF_WRITE)
    {
        fileHandle = fdopen(nFileId, OPEN_MODE_FILE_UPDATE);
        if (fileHandle)
        {
            if (setvbuf(fileHandle, NULL, _IONBF, 0))
            {
                std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
                KLERR_MYTHROW2(ERR_CANT_OPEN2, wstrFilePath.c_str(), wstrErrorDescr.c_str());
            }
        }
    }

    if (fileHandle)
    {
#ifdef WIN32
        if (setmode(nFileId, O_BINARY) == -1)
        {
            std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
            KLERR_MYTHROW2(ERR_CANT_OPEN2, wstrFilePath.c_str(), wstrErrorDescr.c_str());
        }
#endif
    }

	return fileHandle;
}

void JournalImpl::ShrinkCentralFile(const std::wstring &wstrPath, const CreationInfo &creationInfo, long nTimeout)
{
	KLSTD::AutoCriticalSection acs(m_pCS);

    m_bChangingState = false;
    m_nCurrentPage = -1;
	m_creationInfo = creationInfo;

    if (IsOpenedInternal()) KLERR_MYTHROW0(ERR_OPENED);

	m_wstrPageFileTempl = wstrPath;
	m_wstrCentralFilePath = GetCentralFile(wstrPath);
	if (wstrPath == m_wstrCentralFilePath) KLERR_MYTHROW1(ERR_INVALID_JOURNAL_FILE_NAME1, wstrPath.c_str());
	OpenCentralFile(KLSTD::CF_OPEN_EXISTING, KLSTD::ACCESS_FLAGS(KLSTD::AF_READ | KLSTD::AF_WRITE), nTimeout);
	if (!CheckCentralFileSignature()) return;

	std::wstring wstrTempFilePath = m_wstrCentralFilePath + L".tmp";
	FILE* fileNewCentral = _OpenFile(wstrTempFilePath, KLSTD::CF_CREATE_NEW, KLSTD::ACCESS_FLAGS(KLSTD::AF_READ | KLSTD::AF_WRITE), 0);
	if (!fileNewCentral) 
	{
		std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
		KLERR_MYTHROW2(ERR_CANT_OPEN2, wstrTempFilePath.c_str(), wstrErrorDescr.c_str());
	}
	try
	{
		bool bError = false;
		bool bBadValue = false;
		char szBuf[STATIC_BUF_LEN];

		bError = bError || (fgets_bin_win32(szBuf, STATIC_BUF_LEN, m_fileCentral) == NULL);
		if (!bError) m_creationInfo.bBinaryFormat = (strcmp(szBuf, BINARY_MODE_FORMAT_FLAG) == 0);

		bError = bError || (fputs(CENTRAL_FILE_SIGNATURE, fileNewCentral) == EOF);
		bError = bError || (fputs(JRNL_DELIM, fileNewCentral) == EOF);
		bError = bError || (fputs(szBuf, fileNewCentral) == EOF);
		bError = bError || (fputs(JRNL_DELIM, fileNewCentral) == EOF);

		long nJrnlVersion = ReadInt32(m_fileCentral, TEXT_REPR_VERSION, bError, bBadValue);
		if (!bError && nJrnlVersion > JRNL_VERSION) 
			KLERR_MYTHROW1(ERR_UNSOPPORTED_VERSION1, m_wstrCentralFilePath.c_str());
		WriteInt32(nJrnlVersion, fileNewCentral, TEXT_REPR_VERSION, bError);

		m_creationInfo.pageLimitType = PageLimitType(ReadInt32(m_fileCentral, TEXT_REPR_PAGE_LIMIT_TYPE, bError, bBadValue));
		WriteInt32(m_creationInfo.pageLimitType, fileNewCentral, TEXT_REPR_PAGE_LIMIT_TYPE, bError);
		if (m_creationInfo.pageLimitType == pltRecCount)
			WriteInt32(ReadInt32(m_fileCentral, TEXT_REPR_MAX_REC_IN_PAGE, bError, bBadValue), fileNewCentral, TEXT_REPR_MAX_REC_IN_PAGE, bError);
		else
			WriteInt32(ReadInt32(m_fileCentral, TEXT_REPR_MAX_PAGE_SIZE, bError, bBadValue), fileNewCentral, TEXT_REPR_MAX_PAGE_SIZE, bError);

		WriteFlag(ReadFlag(m_fileCentral, TEXT_REPR_LIMIT_PAGES, bError, bBadValue), fileNewCentral, TEXT_REPR_LIMIT_PAGES, bError);

		m_creationInfo.bUseRecNumIndex = ReadFlag(m_fileCentral, TEXT_REPR_USE_REC_NUM_INDEX, bError, bBadValue);
		WriteFlag(m_creationInfo.bUseRecNumIndex, fileNewCentral, TEXT_REPR_USE_REC_NUM_INDEX, bError);

		bool bChanging = ReadFlag(m_fileCentral, TEXT_REPR_CHANGING, bError, bBadValue);
		WriteFlag(bChanging, fileNewCentral, TEXT_REPR_CHANGING, bError);

		WriteInt32(ReadInt32(m_fileCentral, TEXT_REPR_IDENTITY, bError, bBadValue), fileNewCentral, TEXT_REPR_IDENTITY, bError);
		if (!bError && bChanging && m_bChangeAllowed) KLERR_MYTHROW0(ERR_CORRUPTED);

		bool bStopRead = false;
		while(!bError && !bStopRead)
		{
			bool bFileMarkedAsDeleted, bFilePhysDeleted, bValidRecord;
			long nMarkedAsDeletedFlagPos, nPhysDeletedFlagPos, nFileNamePos;
			std::wstring wstrPageFileName, wstrIdxFileName;

			bValidRecord = false;

			if (!bError)
			{
				nMarkedAsDeletedFlagPos = ReadPosition(m_fileCentral, bError);
				bFileMarkedAsDeleted = ReadFlag(m_fileCentral, TEXT_REPR_FILE_MARKED_AS_DELETED, bError, bBadValue);

				// stop read if no record at all - eof encountered.
				bStopRead = bError;
				if (bError && feof(m_fileCentral))
					bError = false;
			}

			char szPageFileNameAddon[STATIC_BUF_LEN];
			char szIndexFileNameAddon[STATIC_BUF_LEN];
			if (!bError && !bStopRead)
			{
				nPhysDeletedFlagPos = ReadPosition(m_fileCentral, bError);
				bFilePhysDeleted = ReadFlag(m_fileCentral, TEXT_REPR_FILE_PHYS_DELETED, bError, bBadValue);

				bool bDelimFound = false;
				nFileNamePos = ReadPosition(m_fileCentral, bError);

				bError = bError || (fgets_bin_win32(szPageFileNameAddon, STATIC_BUF_LEN, m_fileCentral, & bDelimFound) == NULL);
				if (!bError && bDelimFound)
				{
					std::wstring wstrAddOn = (const wchar_t*)KLSTD_A2CW2(szPageFileNameAddon);
					wstrPageFileName = GetPageFilePathFromAddOn(wstrAddOn);

					if (!m_creationInfo.bUseRecNumIndex) // if use recnum index, then check whether record is valid later.
						bValidRecord = true;
				}

				if (m_creationInfo.bUseRecNumIndex)
				{
					bDelimFound = false;
					bError = bError || (fgets_bin_win32(szIndexFileNameAddon, STATIC_BUF_LEN, m_fileCentral, & bDelimFound) == NULL);
					if (!bError && bDelimFound)
					{
						std::wstring wstrAddOn = (const wchar_t*)KLSTD_A2CW2(szIndexFileNameAddon);
						wstrIdxFileName = GetPageFilePathFromAddOn(wstrAddOn);

						bValidRecord = true;
					}
				}

				if (feof(m_fileCentral)) bError = false;
			}

			if (bError)
			{
				bStopRead = true;
				// if we encountered eof too soon, then set bBadValue to true
				bBadValue = bBadValue || (feof(m_fileCentral) != 0);
			} 
			else
			{
				if (bValidRecord)
				{
					bool bAddRecord = true;
					Page page(wstrPageFileName, wstrIdxFileName, 
							  nMarkedAsDeletedFlagPos, nPhysDeletedFlagPos, nFileNamePos, 
							  NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, false);
					if (bFileMarkedAsDeleted)
					{
						if (!bFilePhysDeleted)
						{
							if (m_bChangeAllowed) 
							{
								TryDeletePagePhysically(page, bError);
								bAddRecord = bError;
							}
							bValidRecord = ! bError;
						}
						else
							bAddRecord = false;
					} 
					if (bAddRecord)
					{
						WriteFlag(bFileMarkedAsDeleted, fileNewCentral, TEXT_REPR_FILE_MARKED_AS_DELETED, bError);
						WriteFlag(bFilePhysDeleted, fileNewCentral, TEXT_REPR_FILE_PHYS_DELETED, bError);
						bError = bError || (fputs(szPageFileNameAddon, fileNewCentral) == EOF);
						bError = bError || (fputs(JRNL_DELIM, fileNewCentral) == EOF);
						if (m_creationInfo.bUseRecNumIndex) 
						{
							bError = bError || (fputs(szIndexFileNameAddon, fileNewCentral) == EOF);
							bError = bError || (fputs(JRNL_DELIM, fileNewCentral) == EOF);
						}
					}
				}
				bStopRead = bStopRead || ! bValidRecord;
			}
		}

		if (fileNewCentral) 
		{
			KLSTD_FClose(fileNewCentral);
			fileNewCentral = NULL;
		}
		if (bError)
		{
			std::wstring wstrErrorDescr;
			if (bBadValue || feof(m_fileCentral))
			{
				wstrErrorDescr = L"not all of necessary values can be read";
			} 
			else
			{
				wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
			}
			KLERR_MYTHROW2(ERR_CANT_READ2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str());
		}

		Close();
		std::wstring wstrOldCentralFile = m_wstrCentralFilePath + L".old";
		KLSTD_Rename(m_wstrCentralFilePath.c_str(), wstrOldCentralFile.c_str());
		try
		{
			KLSTD_Rename(wstrTempFilePath.c_str(), m_wstrCentralFilePath.c_str());
			KLSTD_Unlink(wstrOldCentralFile.c_str());
		}
		catch (...)
		{
			KLSTD_Rename(wstrOldCentralFile.c_str(), m_wstrCentralFilePath.c_str());
			throw;
		}
	}
	catch (...)
	{
		KLSTD_Unlink(wstrTempFilePath.c_str());
		if (fileNewCentral) 
		{
			KLSTD_FClose(fileNewCentral);
			fileNewCentral = NULL;
		}
		throw;
	}
}

bool JournalImpl::CheckCentralFileSignature()
{
	if (fseek(m_fileCentral, 0, SEEK_END) == 0 && ftell(m_fileCentral) == 0) return false;
    if (fseek(m_fileCentral, 0, SEEK_SET))
    {
        std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
        KLERR_MYTHROW2(ERR_CANT_READ2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str());
    }

    bool bCreateNewData = false;
    char szBuf[STATIC_BUF_LEN];
    if (fgets_bin_win32(szBuf, STATIC_BUF_LEN, m_fileCentral))
    {
        if (strcmp(szBuf, CENTRAL_FILE_SIGNATURE))
            KLERR_MYTHROW1(ERR_BAD_JOURNAL_FORMAT1, m_wstrCentralFilePath.c_str());
    } 
	else
    {
        if ((! feof(m_fileCentral) && ferror(m_fileCentral)) || m_bInfoDataMustExists)
        {
            std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
            KLERR_MYTHROW2(ERR_CANT_READ2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str());
        }

        bCreateNewData = true;
    }

    return ! bCreateNewData;
}

void JournalImpl::OpenCentralFile(KLSTD::CREATION_FLAGS flagsCreation, KLSTD::ACCESS_FLAGS flagsAccessFlags, long nTimeout)
{
    m_bChangeAllowed = false;
    m_bInfoDataMustExists = false;
    m_bReadAllowed = (flagsAccessFlags & KLSTD::AF_READ) != 0;

    // Today I know only about these flags, so remove other bits so as not to disturb us:
    flagsCreation = KLSTD::CREATION_FLAGS(flagsCreation & (KLSTD::CF_OPEN_EXISTING | KLSTD::CF_CREATE_NEW | KLSTD::CF_CLEAR));

    if (!flagsCreation) KLSTD_THROW_BADPARAM(flagsCreation);
    if (!flagsAccessFlags) KLSTD_THROW_BADPARAM(flagsAccessFlags);

    KLSTD::SHARE_FLAGS shareFlags;
    if (flagsAccessFlags == KLSTD::AF_READ)
    {
       shareFlags = KLSTD::SHARE_FLAGS(m_bChangeLimits?KLSTD::SF_READ:(KLSTD::SF_READ | KLSTD::SF_WRITE));
    } 
	else 
	if (flagsAccessFlags & KLSTD::AF_WRITE)
    {
        shareFlags = KLSTD::SF_READ;
        flagsAccessFlags = KLSTD::ACCESS_FLAGS(flagsAccessFlags | KLSTD::AF_READ);
    } 
	else  // unknown combination of flagsAccessFlags
        KLSTD_ASSERT(0);

	if ((flagsCreation & KLSTD::CF_CREATE_NEW) && !(flagsCreation & KLSTD::CF_OPEN_EXISTING))
	{
		nTimeout = 0;
		shareFlags = KLSTD::SHARE_FLAGS(0);
	} 
	else
		nTimeout = OPEN_SHARED_CENTRAL_FILE_TIMEOUT;	// 10 секунд.

    KLSTD::CAutoPtr<KLSTD::File> pFile;
    KLSTD_CreateFile(m_wstrCentralFilePath, shareFlags, flagsCreation, flagsAccessFlags, KLSTD::EXTRA_FLAGS(KLSTD::EF_RANDOM_ACCESS | KLSTD::EF_DONT_WORRY_ABOUT_CLOSE), &pFile, nTimeout);
    int nFileId = pFile->GetFileID();
    if (flagsAccessFlags == KLSTD::AF_READ)
    {
        m_fileCentral = fdopen(nFileId, OPEN_MODE_FILE_READ);
        m_bInfoDataMustExists = true;
    } 
	else 
	if (flagsAccessFlags & KLSTD::AF_WRITE)
    {
        m_bChangeAllowed = true;
        m_bInfoDataMustExists = (flagsCreation == KLSTD::CF_OPEN_EXISTING || flagsCreation == KLSTD::CF_TRUNCATE_EXISTING);

        m_fileCentral = fdopen(nFileId, OPEN_MODE_FILE_UPDATE);
        if (m_fileCentral)
        {
            if (setvbuf(m_fileCentral, NULL, _IONBF, 0))
            {
                std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
                KLERR_MYTHROW2(ERR_CANT_OPEN2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str());
            }
        }
    }
    if (m_fileCentral)
    {
#ifdef WIN32
        if (setmode(nFileId, O_BINARY) == -1)
        {
            std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
            KLERR_MYTHROW2(ERR_CANT_OPEN2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str());
        }
#endif
    }
}

void JournalImpl::Repair(const std::wstring & wstrPath, const CreationInfo & creationInfo)
{
    ChangeLimits(wstrPath, creationInfo);
}

void JournalImpl::ChangeLimits(const std::wstring &wstrPath, const CreationInfo &creationInfo)
{
    KLSTD::AutoCriticalSection acs(m_pCS);

    m_bChangeLimits = true;

    OpenInternal(wstrPath, KLSTD::CF_OPEN_EXISTING, KLSTD::AF_READ, creationInfo, 0);
    try
    {
        KLSTD::CAutoPtr<JournalImpl> pNewJrnl;
        pNewJrnl.Attach(new JournalImpl(false));
        KLSTD_CHKMEM(pNewJrnl);

        std::wstring wstrNewJrnlFile = GenerateTempJournalName(wstrPath);
        pNewJrnl->Open(wstrNewJrnlFile, KLSTD::CF_CREATE_ALWAYS, KLSTD::AF_WRITE, creationInfo);
        pNewJrnl->m_wstrPageFileTempl = m_wstrPageFileTempl;

        std::wstring wstrThisCentralFile = m_wstrCentralFilePath;
        std::wstring wstrNewCentralFile = pNewJrnl->m_wstrCentralFilePath;

        if (m_vectPages.size())
        {
            int nPageIdx = 0;
            bool bError = false;
            OpenPage(nPageIdx);
            SeekPage(m_vectPages[nPageIdx], m_vectPages[nPageIdx].nStartPosition, SEEK_SET, bError);
            if (bError)
            {
                std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
                KLERR_MYTHROW2(ERR_CANT_READ2, m_vectPages[nPageIdx].wstrFileName.c_str(), wstrErrorDescr.c_str());
            }

            ReadRecord(nPageIdx, true);
            while(! IsEOFInternal(nPageIdx))
            {
                pNewJrnl->AddInternal(m_nCurrentId, m_strDataCurrent);
                ReadRecord(nPageIdx, false);
            }

            for(unsigned int i = 0; i < m_vectPages.size(); i++)
            {
                ClosePage(i);

                Page & page = m_vectPages[i];
                KLSTD_Unlink(page.wstrFileName.c_str(), false);
                if (!page.wstrIdxFileName.empty()) KLSTD_Unlink(page.wstrFileName.c_str(), false);
            }
        }
        ReleaseAllOpenedStuffNoThrow();
        pNewJrnl->Close();
        KLSTD_Unlink(wstrThisCentralFile.c_str(), true);
        KLSTD_Rename(wstrNewCentralFile.c_str(), wstrThisCentralFile.c_str());
    }
    catch(...)
    {
        ReleaseAllOpenedStuffNoThrow();
        throw;
    }
}

void JournalImpl::Close()
{
    KLSTD::AutoCriticalSection acs(m_pCS);
    CloseInternal();
}

void JournalImpl::CloseAndRemoveFiles()
{
    KLSTD::AutoCriticalSection acs(m_pCS);

    if (!IsOpenedInternal()) KLERR_MYTHROW0(ERR_CLOSED);

    std::wstring wstrCentralFilePath = m_wstrCentralFilePath;
    std::vector<std::wstring> vectPageFileNames;
    unsigned int i;
    for(i = 0; i < m_vectPages.size(); i++)
    {
        vectPageFileNames.push_back(m_vectPages[i].wstrFileName);
        if (! m_vectPages[i].wstrIdxFileName.empty())
            vectPageFileNames.push_back(m_vectPages[i].wstrIdxFileName);
    }

    CloseInternal();

    KLSTD_Unlink(m_wstrCentralFilePath.c_str(), true);
    KLSTD_Unlink((m_wstrCentralFilePath + CTRL_FILE_ID_EXT).c_str(), false);
    for(i = 0; i < vectPageFileNames.size(); i++)
    {
        std::wstring wstrTst = vectPageFileNames[i];
        KLSTD_Unlink(vectPageFileNames[i].c_str(), true);
    }
}

void JournalImpl::CloseInternal()
{
    // let the exception fly away, if any, but close all opened files anyway:
    try
    {
        if (!IsOpenedInternal()) KLERR_MYTHROW0(ERR_CLOSED);
        for(unsigned int i = 0; i < m_vectPages.size(); i++)
            ClosePage(i);  // may throw an exception

        if (m_bChangingState && !m_nInvalidState)
        {
            // if exception will be thrown earlier, journal will stay in "changing" state.
            // thus, next open will throw ERR_CORRUPTED exception, which will indicate that 
            // Repair method call shall be done for the journal.

            bool bError = false;

            bError = bError || fseek(m_fileCentral, m_nIdentityPos, SEEK_SET);
            WriteInt32(m_nIdentity, m_fileCentral, TEXT_REPR_IDENTITY, bError);
            if (bError)
            {
                std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
                KLERR_MYTHROW2(ERR_CANT_WRITE2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str());
            }

            SetChangingState(false);
        }

        ReleaseAllOpenedStuffNoThrow();
    }
    catch(...)
    {
        ReleaseAllOpenedStuffNoThrow();
        throw;
    }
}

void JournalImpl::ReleaseAllOpenedStuffNoThrow()
{
    for(unsigned int i = 0; i < m_vectPages.size(); i++)
    {
        if (m_vectPages[i].file)
        {
            KLSTD_FClose(m_vectPages[i].file);
            m_vectPages[i].file = NULL;
        }
    }
    m_vectPages.clear();

    if (m_fileCentral) 
	{
		KLSTD_FClose(m_fileCentral);
	    m_fileCentral = NULL;
	}
    vectUsedFileNameAddOns.clear();
    m_nInvalidState = 0;
}

bool JournalImpl::IsOpened()
{ 
    KLSTD::AutoCriticalSection acs(m_pCS);
    return IsOpenedInternal();
}

AVP_longlong JournalImpl::GetJournalSize()
{
    if (m_nInvalidState) KLERR_MYTHROW0(ERR_PREVIOUS_OPERATION_FAILED);
    if (!IsOpenedInternal()) KLERR_MYTHROW0(ERR_CLOSED);

    KLSTD::AutoCriticalSection acs(m_pCS);

    m_nCurrentPage = -1;

    AVP_longlong llResult = 0;
    for(unsigned int nPageIdx = 0; nPageIdx < m_vectPages.size(); nPageIdx++)
    {
        OpenPage(nPageIdx);
        llResult += m_vectPages[ nPageIdx ].nPageSize;
    }

    bool bError = false;

    long nCurrPos = ReadPosition(m_fileCentral, bError);
    bError = bError || fseek(m_fileCentral, 0, SEEK_END);            
    llResult += ReadPosition(m_fileCentral, bError);
    bError = bError || fseek(m_fileCentral, nCurrPos, SEEK_SET);
    if (bError)
    {
        std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
        KLERR_MYTHROW2(ERR_CANT_READ2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str());
    }

    return llResult;
}

bool JournalImpl::IsOpenedInternal()
{ 
    if (m_nInvalidState) KLERR_MYTHROW0(ERR_PREVIOUS_OPERATION_FAILED);
    return (m_fileCentral != NULL || m_bCtrlFileNotCreated); 
}

long JournalImpl::Add(const Params* pParams)
{
    KLSTD_ASSERT(pParams);

    KLSTD::AutoCriticalSection acs(m_pCS);

    KLSTD::CAutoPtr<KLSTD::MemoryChunk> pChunk;
    KLPAR_SerializeToMemory(pParams, & pChunk);

    std::string strData((const char *)pChunk->GetDataPtr(),  pChunk->GetDataSize());
    AddInternal(++m_nIdentity, strData);
    return m_nIdentity;
}

long JournalImpl::Add(JournalRecord* pRecord)
{
    KLSTD_ASSERT(pRecord);

    KLSTD::AutoCriticalSection acs(m_pCS);

    long nId = ++m_nIdentity;

    std::string strData;
    pRecord->Serialize(strData);
    AddInternal(nId, strData);

    return nId;
}

long JournalImpl::Add(const std::string & str)
{
    KLSTD::AutoCriticalSection acs(m_pCS);

    std::string strData(str);
    long nId = ++m_nIdentity;
    AddInternal(nId, strData);

    return nId;
}

void JournalImpl::Update(const KLPAR::Params* pParams)
{
    KLSTD_ASSERT(pParams);

    KLSTD::AutoCriticalSection acs(m_pCS);

    KLSTD::CAutoPtr<KLSTD::MemoryChunk> pChunk;
    KLPAR_SerializeToMemory(pParams, & pChunk);
    std::string strData((const char *)pChunk->GetDataPtr(),  pChunk->GetDataSize());
    UpdateInternal(strData);
}

void JournalImpl::Update(JournalRecord* pRecord)
{
    KLSTD_ASSERT(pRecord);

    KLSTD::AutoCriticalSection acs(m_pCS);

    std::string strData;
    pRecord->Serialize(strData);
    UpdateInternal(strData);
}

void JournalImpl::Update(const std::string & str)
{
    KLSTD::AutoCriticalSection acs(m_pCS);

    std::string strData(str);
    UpdateInternal(strData);
}

void JournalImpl::UpdateInternal(std::string& strData)
{
    if (m_nInvalidState) KLERR_MYTHROW0(ERR_PREVIOUS_OPERATION_FAILED);
    if (!IsOpenedInternal()) KLERR_MYTHROW0(ERR_CLOSED);
    if (!m_bChangeAllowed) KLERR_MYTHROW0(ERR_CHANGE_NOT_ALLOWED);
    if (!m_bReadAllowed) KLERR_MYTHROW0(ERR_READ_NOT_ALLOWED);
    if ((m_nCurrentPage < 0) || (IsEOFInternal(m_nCurrentPage))) KLERR_MYTHROW0(ERR_NO_CURRENT_POS);
    if (m_strDataCurrent.size() != strData.size()) KLERR_MYTHROW0(ERR_UPDATED_DATA_HAS_OTHER_SIZE);

    Page & page = m_vectPages[m_nCurrentPage];
    bool bError = false;
    long nCurrPosition = ReadPosition(page.file, bError);
    SeekPage(page, m_nActualDataStartPos, SEEK_SET, bError);
    if (! bError)
    {
        DuplicateSubstr(strData, JRNL_DELIM_RECORD_BEGIN);
        DuplicateSubstr(strData, JRNL_DELIM_RECORD_END);

        bError = bError || (fwrite(strData.c_str(), 1, strData.size(), page.file) != strData.size());
        SeekPage(page, nCurrPosition, SEEK_SET, bError);
    }

    if (bError)
    {
        std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
        KLERR_MYTHROW2(ERR_CANT_WRITE2, page.wstrFileName.c_str(), wstrErrorDescr.c_str());            
    }
}

void JournalImpl::AddInternal(long nId, std::string& strData)
{
    if (m_nInvalidState) KLERR_MYTHROW0(ERR_PREVIOUS_OPERATION_FAILED);
    if (!IsOpenedInternal()) KLERR_MYTHROW0(ERR_CLOSED);
    if (!m_bChangeAllowed) KLERR_MYTHROW0(ERR_CHANGE_NOT_ALLOWED);
    if (m_nIdentity < nId) m_nIdentity = nId;

    m_nCurrentPage = -1;

    PrepareLastPage();

    bool bError = false;

    SetChangingState(true);
    SetPageChangingState(LastPageIdx(), true);
    
    Page& page = GetLastPage();

    PageToEOF(page, bError);
    long nRecStart = ReadPosition(page.file, bError);
    bError = bError || (fputs(JRNL_DELIM_RECORD_BEGIN, page.file) == EOF);
    WriteInt32(nId, page.file, TEXT_REPR_ID, bError);
    WriteFlag(false, page.file, TEXT_REPR_DELETED, bError);

    if (!bError)
    {
        DuplicateSubstr(strData, JRNL_DELIM_RECORD_BEGIN);
        DuplicateSubstr(strData, JRNL_DELIM_RECORD_END);

        bError = bError || (fwrite(strData.c_str(), 1, strData.size(), page.file) != strData.size());
        bError = bError || (fputs(JRNL_DELIM_RECORD_END, page.file) == EOF);
        if (! bError)
        {
            page.nTotalRecCount++;
            page.nPageSize = ReadPosition(page.file, bError);
        }

        if (m_creationInfo.pageLimitType == pltUnlimited)
        {
            page.bPageIsFull = false;
        } 
		else 
		if (m_creationInfo.pageLimitType == pltRecCount)
        {
            page.bPageIsFull = (page.nTotalRecCount >= m_creationInfo.nMaxRecordsInPage);
        } 
		else
            page.bPageIsFull = (page.nPageSize >= m_creationInfo.nMaxPageSize);
    }

    if (page.fileIdx) WriteInt32(nRecStart, page.fileIdx, TEXT_REPR_REC_POS, bError);
    if (bError)
    {
        std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
        KLERR_MYTHROW2(ERR_CANT_WRITE2, page.wstrFileName.c_str(), wstrErrorDescr.c_str());            
    }
}

void JournalImpl::Flush()
{
    if (m_nInvalidState) KLERR_MYTHROW0(ERR_PREVIOUS_OPERATION_FAILED);
    if (!IsOpenedInternal()) KLERR_MYTHROW0(ERR_CLOSED);
    if (!m_bChangeAllowed) KLERR_MYTHROW0(ERR_CHANGE_NOT_ALLOWED);

    bool bError = false;
    std::wstring wstrErrFilePath;
    for(unsigned int i = 0; !bError && (i < m_vectPages.size()); i++)
    {
        Page & page = m_vectPages[i];

        if (page.file && page.bChanging)
        {
            SavePageHeader(page);
            wstrErrFilePath = page.wstrFileName;
            FlushJrnlFile(page.file, bError);
        }

        if (! bError && page.fileIdx)
        {
            wstrErrFilePath = page.wstrIdxFileName;
            FlushJrnlFile(page.fileIdx, bError);
        }
    }

    if (bError)
    {
        std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
        KLERR_MYTHROW2(ERR_CANT_WRITE2, wstrErrFilePath.c_str(), wstrErrorDescr.c_str());
    }
}

void JournalImpl::PrepareLastPage()
{
    bool bCreateNewPage = false;

    if (!IsLastPageOpened())
    {
        if (m_vectPages.size() == 0)
            bCreateNewPage = true;
        else
            OpenPage(LastPageIdx());
    }

    if (! bCreateNewPage)
    {
        Page& page = GetLastPage();
        bCreateNewPage = page.bPageIsFull;
        if (bCreateNewPage) ClosePage(LastPageIdx());
    }

    if (bCreateNewPage)
    {
        CreateAndOpenNewPage();
        if (m_creationInfo.bLimitPages)
        {
            while(m_vectPages.size() > 2)
                RemovePage(0);
        }
    }
}

void JournalImpl::SavePageHeader(Page & page)
{
    if (page.file)
    {
        bool bError = false;

        long nCurrentPos = ReadPosition(page.file, bError);
        SeekPage(page, page.nTotalRecCountPos, SEEK_SET, bError);
        WriteInt32(page.nTotalRecCount, page.file, TEXT_REPR_TOTAL_REC_COUNT, bError);
        WriteInt32(page.nDeletedRecCount, page.file, TEXT_REPR_DELETED_REC_COUNT, bError);
        WriteInt32(page.nStartPosition, page.file, TEXT_REPR_START_POSITION, bError);
        SeekPage(page, nCurrentPos, SEEK_SET, bError);

        if (bError)
        {
            std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
            KLERR_MYTHROW2(ERR_CANT_CLOSE2, page.wstrFileName.c_str(), wstrErrorDescr.c_str());
        }
    }
}

void JournalImpl::ClosePage(int nPageIdx)
{
    KLSTD_ASSERT((nPageIdx >= 0) && ((unsigned int)nPageIdx < m_vectPages.size()));
    
    Page& page = m_vectPages[ nPageIdx ];

    if (page.file)
    {
        if (page.bChanging)
        {
            SavePageHeader(page);
            SetPageChangingState(nPageIdx, false);
        }

        bool bError = false;

        bError = KLSTD_FClose(page.file) != 0;
        page.file = NULL;

        // here try close the file even if prev op failed:
        if (page.fileIdx) 
		{
			bError = KLSTD_FClose(page.fileIdx) || bError; 
			page.fileIdx = NULL;
		}
        if (bError)
        {
            std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
            KLERR_MYTHROW2(ERR_CANT_CLOSE2, page.wstrFileName.c_str(), wstrErrorDescr.c_str());
        }
    }
}

void JournalImpl::RemovePage(int nPageIdx)
{
    KLSTD_ASSERT((nPageIdx >= 0) && ((unsigned int)nPageIdx < m_vectPages.size()));

    ClosePage(nPageIdx);
    
    Page& page = m_vectPages[ nPageIdx ];

    bool bError = false;
    bError = bError || fseek(m_fileCentral, page.nMarkedAsDeletedFlagPos, SEEK_SET);
    WriteFlag(true, m_fileCentral, TEXT_REPR_FILE_MARKED_AS_DELETED, bError);

    FlushJrnlFile(m_fileCentral, bError);

    TryDeletePagePhysically(page, bError);

    if (!bError)
    {
        m_vectPages.erase(m_vectPages.begin() + nPageIdx);
    } 
	else
    {
        std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
        KLERR_MYTHROW2(ERR_CANT_WRITE2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str());
    }
}

void JournalImpl::OpenPage(int nPageIdx)
{
    KLSTD_ASSERT((nPageIdx >= 0) && ((unsigned int)nPageIdx < m_vectPages.size()));

    Page & page = m_vectPages[ nPageIdx ];
    if (!page.file)
    {
        std::wstring wstrFileName = page.wstrFileName;
        std::wstring wstrIdxFileName = page.wstrIdxFileName;

        FILE* fileTemp = NULL;
        FILE* fileTempIdx = NULL;
        KLERR_TRY
        {
            std::string strOpenMode;
			KLSTD::SHARE_FLAGS shareFlags;
            KLSTD::ACCESS_FLAGS accessFlags;
            if (m_bChangeAllowed)
            {
                strOpenMode = OPEN_MODE_FILE_UPDATE;
                shareFlags = KLSTD::SF_READ;
                accessFlags = KLSTD::ACCESS_FLAGS(KLSTD::AF_READ | KLSTD::AF_WRITE);
            } 
			else
            {
                strOpenMode = OPEN_MODE_FILE_READ;
                shareFlags = KLSTD::SHARE_FLAGS(KLSTD::SF_READ | KLSTD::SF_WRITE);
                accessFlags = KLSTD::AF_READ;
            }

            fileTemp = OpenFileRaw(wstrFileName, shareFlags, KLSTD::CF_OPEN_EXISTING, accessFlags);
			if (fileTemp)
			{
				if (m_creationInfo.bUseRecNumIndex)
					fileTempIdx = OpenFileRaw(wstrIdxFileName, shareFlags, KLSTD::CF_OPEN_EXISTING, accessFlags);

				char szBuf[STATIC_BUF_LEN];

				bool bSignatureOk = fgets_bin_win32(szBuf, STATIC_BUF_LEN, fileTemp) && 
									(strcmp(szBuf, PAGE_FILE_SIGNATURE) == 0);
				if (! bSignatureOk) KLERR_MYTHROW1(ERR_BAD_JOURNAL_FORMAT1, wstrFileName.c_str());

				bool bError = false;
				bool bBadValue = false;

				// omit "changing" flag, it necessary only for repair.
				page.nChangingFlagPos = ReadPosition(fileTemp, bError);
				ReadFlag(fileTemp, TEXT_REPR_CHANGING, bError, bBadValue);

				// !!!!! the order of TEXT_REPR_TOTAL_REC_COUNT, TEXT_REPR_DELETED_REC_COUNT and
				// TEXT_REPR_START_POSITION used in SavePageToDisk function !!!!!
				page.nTotalRecCountPos = ReadPosition(fileTemp, bError);
				page.nTotalRecCount = ReadInt32(fileTemp, TEXT_REPR_TOTAL_REC_COUNT, bError, bBadValue);
				page.nDeletedRecCountPos = ReadPosition(fileTemp, bError);
				page.nDeletedRecCount = ReadInt32(fileTemp, TEXT_REPR_DELETED_REC_COUNT, bError, bBadValue);
				page.nStartPositionPos = ReadPosition(fileTemp, bError);
				page.nStartPosition = ReadInt32(fileTemp, TEXT_REPR_START_POSITION, bError, bBadValue);
				if (m_bChangeLimits) page.nStartPosition = ReadPosition(fileTemp, bError);

				{
					long nCurrPos = ReadPosition(fileTemp, bError);
					bError = bError || fseek(fileTemp, 0, SEEK_END);            
					page.nPageSize = ReadPosition(fileTemp, bError);
					bError = bError || fseek(fileTemp, nCurrPos, SEEK_SET);
				}

				if (m_bChangeAllowed) // nPageSize not used in read-only mode
				{
					if (m_creationInfo.pageLimitType == pltUnlimited)
					{
						page.bPageIsFull = false;
					} 
					else 
					if (m_creationInfo.pageLimitType == pltRecCount)
					{
						page.bPageIsFull = (page.nTotalRecCount >= m_creationInfo.nMaxRecordsInPage);
					}
					else
						page.bPageIsFull = (page.nPageSize >= m_creationInfo.nMaxPageSize);
				}

				if (bError)
				{
					std::wstring wstrErrorDescr;
					if (bBadValue || feof(fileTemp))
					{
						wstrErrorDescr = L"not all necessary values can be read";
					} 
					else
					{
						wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
					}
					KLERR_MYTHROW2(ERR_CANT_READ2, wstrFileName.c_str(), wstrErrorDescr.c_str());            
				}

				page.file = fileTemp;
				page.fileIdx = fileTempIdx;
				fileTempIdx = NULL;
				fileTemp = NULL;
				// file handle now is stored in m_vectPages, so set fileTemp                              
				// to NULL. In case of later exception in this try-catch block the                             
				// file will not be closed.

			} // if (fileTemp) 
        }
        KLERR_CATCH(pError)
        {
			KLSTD_TRACE0(1, L"Cannot open page:\n");
			KLERR_SAY_FAILURE(1, pError);
            if (fileTemp)
            {
                KLSTD_FClose(fileTemp);
                fileTemp = NULL;
            }

            if (fileTempIdx)
            {
                KLSTD_FClose(fileTempIdx);
                fileTempIdx = NULL;
            }
        }
		KLERR_ENDTRY;
    }
}

FILE * JournalImpl::OpenFileRaw(
    const std::wstring & wstrFileName, 
    AVP_dword dwSharing,
    AVP_dword dwCreation, 
    AVP_dword dwFlags)
{
    FILE* file;

    KLSTD::CAutoPtr<KLSTD::File> pFile;

	KLERR_TRY
	{
		KLSTD_CreateFile(wstrFileName, dwSharing, dwCreation, dwFlags, KLSTD::EXTRA_FLAGS(KLSTD::EF_RANDOM_ACCESS | KLSTD::EF_DONT_WORRY_ABOUT_CLOSE), &pFile);
	}
	KLERR_CATCH(pError)
	{
		KLSTD_TRACE0(1, L"JournalImpl::OpenFileRaw error:\n");
		KLERR_SAY_FAILURE(1, pError);
		if ((dwCreation & KLSTD::CF_OPEN_EXISTING) && !(dwCreation & KLSTD::CF_CREATE_NEW))
		{
			KLSTD_TRACE0(1, L"JournalImpl::OpenFileRaw: assume file is absent.\n")
		}
		else
			KLERR_RETHROW();
	}
	KLERR_ENDTRY

	if (!pFile) return NULL;
	
	std::string strOpenMode;
	if ( dwFlags==KLSTD::AF_READ ) strOpenMode = OPEN_MODE_FILE_READ;
	else strOpenMode = OPEN_MODE_FILE_UPDATE;
	
    file = fdopen( pFile->GetFileID(), strOpenMode.c_str() );
    try
    {
        if (!file
#ifdef WIN32
            || (setmode(pFile->GetFileID(), O_BINARY) == -1)
#endif
         )
        {
            std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
            KLERR_MYTHROW2(ERR_CANT_OPEN2, wstrFileName.c_str(), wstrErrorDescr.c_str());
        }
    }
    catch(...)
    {
        if (file)
        {
            KLSTD_FClose(file);
            file = NULL;
        }
        throw;
    }

    return file;
}

void JournalImpl::CreateAndOpenNewPage()
{
    long nMarkedAsDeletedFlagPos, nPhysDeletedFlagPos, nFileNamePos, nChangingFlagPos;
    long nTotalRecCountPos, nDeletedRecCountPos, nStartPositionPos, nStartPosition;
    long nPageSize;
    std::wstring wstrPageFileName;
    std::wstring wstrIdxPageFileName;

    std::wstring wstrFileNameAddOn = GenerateNewPageFileNameAddOn(L"");
    wstrPageFileName = GetPageFilePathFromAddOn(wstrFileNameAddOn);

    std::wstring wstrIdxFileNameAddOn;
    if (m_creationInfo.bUseRecNumIndex)
    {
        wstrIdxFileNameAddOn = GenerateNewPageFileNameAddOn(wstrFileNameAddOn + IDX_FILENAME_ADDON);
        wstrIdxPageFileName = GetPageFilePathFromAddOn(wstrIdxFileNameAddOn);
    }

    FILE* file = NULL;
    FILE* fileIdx = NULL;
    try
    {
        file = OpenFileRaw(wstrPageFileName, KLSTD::SF_READ, KLSTD::CF_CREATE_ALWAYS, KLSTD::AF_READ | KLSTD::AF_WRITE);
        if (! wstrIdxPageFileName.empty())
            fileIdx = OpenFileRaw(wstrIdxPageFileName, KLSTD::SF_READ, KLSTD::CF_CREATE_ALWAYS, KLSTD::AF_READ | KLSTD::AF_WRITE);

        {
            bool bError = false;

            bError = bError || (fputs(PAGE_FILE_SIGNATURE, file) == EOF);
            bError = bError || (fputs(JRNL_DELIM, file) == EOF);

            nChangingFlagPos = ReadPosition(file, bError);
            WriteFlag(false, file, TEXT_REPR_CHANGING, bError);

            nTotalRecCountPos = ReadPosition(file, bError);
            WriteInt32(0, file, TEXT_REPR_TOTAL_REC_COUNT, bError);

            nDeletedRecCountPos = ReadPosition(file, bError);
            WriteInt32(0, file, TEXT_REPR_DELETED_REC_COUNT, bError);

            nStartPositionPos = ReadPosition(file, bError);
            // write fake value, just to move pointer to the next field position
            WriteInt32(0, file, TEXT_REPR_START_POSITION, bError);

            nStartPosition = ReadPosition(file, bError);
            bError = bError || fseek(file, nStartPositionPos, SEEK_SET);
            WriteInt32(nStartPosition, file, TEXT_REPR_START_POSITION, bError);
            bError = bError || fseek(file, nStartPosition, SEEK_SET);

            nPageSize = nStartPosition; //keep in mind nPageSize when writing anything later!!!!
          
            FlushJrnlFile(file, bError);

            if (bError)
            {
                std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
                KLERR_MYTHROW2(ERR_CANT_WRITE2, wstrPageFileName.c_str(), wstrErrorDescr.c_str());            
            }
        }

        {
            bool bCentralError = false;

            bCentralError = bCentralError || fseek(m_fileCentral, m_nFinishRecEndPos, SEEK_SET);

            nMarkedAsDeletedFlagPos = ReadPosition(m_fileCentral, bCentralError);
            WriteFlag(false, m_fileCentral, TEXT_REPR_FILE_MARKED_AS_DELETED, bCentralError);

            nPhysDeletedFlagPos = ReadPosition(m_fileCentral, bCentralError);
            WriteFlag(false, m_fileCentral, TEXT_REPR_FILE_PHYS_DELETED, bCentralError);

            nFileNamePos = ReadPosition(m_fileCentral, bCentralError);
            bCentralError = bCentralError || (fputs(KLSTD_W2CA2(wstrFileNameAddOn.c_str()), m_fileCentral) == EOF);
            bCentralError = bCentralError || (fputs(JRNL_DELIM, m_fileCentral) == EOF);

            if (m_creationInfo.bUseRecNumIndex)
            {
                bCentralError = bCentralError || (fputs(KLSTD_W2CA2(wstrIdxFileNameAddOn.c_str()), m_fileCentral) == EOF);
                bCentralError = bCentralError || (fputs(JRNL_DELIM, m_fileCentral) == EOF);
            }

            long nNewFinishRecEndPos = ReadPosition(m_fileCentral, bCentralError);
            if (! bCentralError)
            {
                Page page(wstrPageFileName, wstrIdxPageFileName,
						  nMarkedAsDeletedFlagPos, nPhysDeletedFlagPos,
						  nFileNamePos, file, fileIdx, nTotalRecCountPos,
						  0,  // _nTotalRecCount
						  nDeletedRecCountPos, 0,  // _nDeletedRecCount
						  nChangingFlagPos, nStartPositionPos, nStartPosition,
						  nPageSize, false);

                m_nFinishRecEndPos = nNewFinishRecEndPos;
                m_vectPages.push_back(page);
                file = NULL; // now it is stored in m_vectPages and state is correct.
                             // So in case of any exception later in this try-catch block 
                             // it will not be closed.
            }

            if (bCentralError)
            {
                std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
                KLERR_MYTHROW2(ERR_CANT_WRITE2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str());
            }
        }
    }
    catch(...)
    {
        if (file)
        {
            KLSTD_FClose(file);
            file = NULL;
        }

        if (fileIdx)
        {
            KLSTD_FClose(fileIdx);
            fileIdx = NULL;
        }

        throw;
    }
}

void JournalImpl::ResetIteratorEx(std::string& strJournalID)
{
	ResetIterator();
	strJournalID = ReadCentralFileIdFile(m_wstrCentralFilePath);
}

bool JournalImpl::SetIteratorEx(long nId, std::string& strJournalID)
{
	bool rc = SetIterator(nId);
	strJournalID = ReadCentralFileIdFile(m_wstrCentralFilePath);
	return rc;
}

void JournalImpl::ResetIterator()
{
    KLSTD::AutoCriticalSection acs(m_pCS);

    if (m_nInvalidState) KLERR_MYTHROW0(ERR_PREVIOUS_OPERATION_FAILED);
    if (!IsOpenedInternal()) KLERR_MYTHROW0(ERR_CLOSED);
    if (!m_bReadAllowed) KLERR_MYTHROW0(ERR_READ_NOT_ALLOWED);

    int nPageIdx = 0;
    if (m_vectPages.size())
    {
        OpenPage(nPageIdx);
    
        bool bError = false;
        Page& page = m_vectPages[nPageIdx];
        SeekPage(page, page.nStartPosition, SEEK_SET, bError);
        if (bError)
        {
            std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
            KLERR_MYTHROW2(ERR_CANT_READ2, page.wstrFileName.c_str(), wstrErrorDescr.c_str());
        }

        ReadRecord(nPageIdx, true); // may change nPageIdx;
    }
    m_nCurrentPage = nPageIdx;
}

bool JournalImpl::SetIterator(long nId)
{
    if (m_nInvalidState) KLERR_MYTHROW0(ERR_PREVIOUS_OPERATION_FAILED);
    if (!IsOpenedInternal()) KLERR_MYTHROW0(ERR_CLOSED);

	bool bResult = false;

    KLSTD::AutoCriticalSection acs(m_pCS);

    m_nCurrentPage = FindBorderPage(nId, true);
    if (m_nCurrentPage < 0)
    {
        GoToEOF();
    } 
	else
    {
        bool bError = false;
        SeekPage(m_vectPages[m_nCurrentPage], 0, SEEK_END, bError);
        long nPageSize = ReadPosition(m_vectPages[m_nCurrentPage].file, bError);

        if (bError)
        {
            std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
            KLERR_MYTHROW2(ERR_CANT_READ2, m_vectPages[m_nCurrentPage].wstrFileName.c_str(), wstrErrorDescr.c_str());
        }

        SeekContext seekContext(nId, m_vectPages[m_nCurrentPage].nStartPosition, nPageSize);
        SeekRecord(seekContext);

        if (!IsEOFInternal(m_nCurrentPage))
        {
//            while(m_nCurrentId < nId) ReadRecord(m_nCurrentPage, false);
			long nPreviousId = m_nCurrentId;
            while(m_nCurrentId < nId) 
			{
				ReadRecord(m_nCurrentPage, false);
				if (nPreviousId == m_nCurrentId) break;
				nPreviousId = m_nCurrentId;
			}

			bResult = ! IsEOFInternal(m_nCurrentPage) && (m_nCurrentId == nId);
        }
    }

	return bResult;
}

void JournalImpl::MoveToRecordN(long nRecIdx)
{
    if (m_nInvalidState) KLERR_MYTHROW0(ERR_PREVIOUS_OPERATION_FAILED);
    if (!IsOpenedInternal()) KLERR_MYTHROW0(ERR_CLOSED);
    if (nRecIdx <= 0)
    {
        ResetIterator();
        return;
    }

    KLSTD::AutoCriticalSection acs(m_pCS);

    int nTargetPage = -1;
    long nTotalCount = 0;
    for(unsigned int nPageIdx = 0; nPageIdx < m_vectPages.size(); nPageIdx++)
    {
        OpenPage(nPageIdx);
        Page & page = m_vectPages[nPageIdx];
        long nPageCount = page.nTotalRecCount - page.nDeletedRecCount;

        if ((nTotalCount + nPageCount) > nRecIdx)
        {
            nTargetPage = nPageIdx;
            break;
        }
        nTotalCount += nPageCount;
    }

    if (nTargetPage < 0)
    {
        GoToEOF();
    } 
	else
    {
        bool bError = false;

        m_nCurrentPage = nTargetPage;
        Page & pageCurr = m_vectPages[m_nCurrentPage];

        if (! pageCurr.fileIdx)
        {
            SeekPage(pageCurr, pageCurr.nStartPosition, SEEK_SET, bError);
            while(! IsEOFInternal(m_nCurrentPage) && (nTotalCount <= nRecIdx))
            {
                ReadRecord(m_nCurrentPage, false);
                nTotalCount++;
            }
        } 
		else
        {
            std::wstring wstrErrFileName =  pageCurr.wstrIdxFileName;
            long nOffset =
                (nRecIdx - nTotalCount) *
                (
                    m_creationInfo.bBinaryFormat ?
                    sizeof(AVP_int32) : TEXT_MODE_IDX_REC_SIZE
               );

            bError = bError || fseek(pageCurr.fileIdx, nOffset, SEEK_SET);
            pageCurr.bAtEOF = false;

            if (!bError && feof(pageCurr.fileIdx)) KLERR_MYTHROW1(ERR_UNEXPECTED_END_OF_INDEX1, pageCurr.wstrIdxFileName.c_str());
            bool bBadValue = false;
            long nPos = ReadInt32(pageCurr.fileIdx, TEXT_REPR_REC_POS, bError, bBadValue);
            if (!bError)
            {
                wstrErrFileName =  pageCurr.wstrFileName;
                SeekPage(pageCurr, nPos, SEEK_SET, bError);
                if (!bError) ReadRecord(m_nCurrentPage, false);
            }

            if (bError)
            {
                std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
                KLERR_MYTHROW2(ERR_CANT_READ2, wstrErrFileName.c_str(), wstrErrorDescr.c_str());
            }
        }
    }
}

void JournalImpl::SeekRecord(SeekContext & seekContext)
{
    Page & page = m_vectPages[m_nCurrentPage];

    bool bError = false;

    if (seekContext.nLeftRecId < 0)
    {
        SeekPage(page, seekContext.GetBegin(), SEEK_SET, bError);
        if (!bError)
        {
            int nPageIdxTmp = m_nCurrentPage;
            ReadRecord(nPageIdxTmp, false);
            if (IsEOFInternal(nPageIdxTmp) || (nPageIdxTmp != m_nCurrentPage))
            {
                // Искомая запись должна была быть в этой странице в указанном в seekContext интервале.
                // Раз ее здесь нет, значит, надо идти на ближайшую след. запись или EOF.
                m_nCurrentPage = nPageIdxTmp;
                return;
            }
            seekContext.SetBegin(m_nCurrentStartPos);
            seekContext.nLeftRecId = m_nCurrentId;
            seekContext.nLeftRecEnd = ReadPosition(page.file, bError);

            if (seekContext.nLeftRecId >= seekContext.GetId()) return;
            if (seekContext.nLeftRecEnd >= seekContext.GetEnd())
            {
                ReadRecord(m_nCurrentPage, false);
                return;
            }
        }
    }

    if (seekContext.nLeftRecId > seekContext.GetId())
    {
        KLSTD_ASSERT(0);
    } 
	else 
	if (seekContext.nLeftRecId < seekContext.GetId())
    {
        int nPosToGo = unsigned(seekContext.GetBegin() + seekContext.GetEnd()) / 2;
        if (nPosToGo <= seekContext.nLeftRecEnd)
        {
            seekContext.SetBegin(seekContext.nLeftRecEnd);
        } 
		else
        {
            SeekPage(page, nPosToGo, SEEK_SET, bError);
            if (!bError)
            {
                int nPageIdxTmp = m_nCurrentPage;
                ReadRecord(nPageIdxTmp, false);
                if (m_nCurrentId == seekContext.GetId()) return;
                if (IsEOFInternal(nPageIdxTmp) || 
                    (nPageIdxTmp != m_nCurrentPage) ||
                    (m_nCurrentId > seekContext.GetId()))
                {
                    seekContext.SetEnd(nPosToGo);
                } 
				else
                    seekContext.SetBegin(nPosToGo);
            }
        }

        if (!bError) SeekRecord(seekContext);
    }

    if (bError)
    {
        std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
        KLERR_MYTHROW2(ERR_CANT_READ2, m_vectPages[m_nCurrentPage].wstrFileName.c_str(), wstrErrorDescr.c_str());
    }
}

long JournalImpl::GetRecordCount()
{
    if (m_nInvalidState) KLERR_MYTHROW0(ERR_PREVIOUS_OPERATION_FAILED);
    if (!IsOpenedInternal()) KLERR_MYTHROW0(ERR_CLOSED);

    KLSTD::AutoCriticalSection acs(m_pCS);

    long nTotalCount = 0;

    for(unsigned int nPageIdx = 0; nPageIdx < m_vectPages.size(); nPageIdx++)
    {
        OpenPage(nPageIdx);
        Page & page = m_vectPages[nPageIdx];
        nTotalCount += page.nTotalRecCount - page.nDeletedRecCount;
    }

    return nTotalCount;
}

void JournalImpl::GoToEOF()
{
    m_nCurrentPage = m_vectPages.size(); // go to EOF
}

void JournalImpl::ReadRecord(int & nPageIdx, bool bReset)
{
    KLSTD_ASSERT((nPageIdx >= 0) && ((unsigned int)nPageIdx < m_vectPages.size()));

    bool bError = false;
    bool bBadValue = false;

    bool bStop = false;

    char szBuf[STATIC_BUF_LEN];

    while(! bStop)
    {
        bool bSeekNext = false;

        Page& page = m_vectPages[nPageIdx];
        m_nCurrentStartPos = ReadPosition(page.file, bError);

        // first, we have to have a JRNL_DELIM_RECORD_BEGIN - so, an empty std::string must be read:
        {
            unsigned int nStartDelimLen = strlen(JRNL_DELIM_RECORD_BEGIN);
            size_t nBytesRead = fread(szBuf, 1, nStartDelimLen, page.file);
            bError = bError || (nBytesRead != nStartDelimLen);
            if (! bError)
            {
                szBuf[ nStartDelimLen ] = 0;
                bSeekNext = strcmp(szBuf, JRNL_DELIM_RECORD_BEGIN) != 0;
            }
        }

        if (! bError && ! bSeekNext)
        {
            m_nCurrentId = ReadInt32(page.file, TEXT_REPR_ID, bError, bBadValue);
            bSeekNext = bSeekNext || (bError && bBadValue);
            if (bSeekNext) bError = false;
        }

        if (! bError && ! bSeekNext)
        {
            m_nCurrentDeletedFlagPos = ReadPosition(page.file, bError);
            bool bDeletedFlag = ReadFlag(page.file, TEXT_REPR_DELETED, bError, bBadValue);
            if (! bError)
            {
                bSeekNext = bDeletedFlag;
            } 
			else
            {
                bSeekNext = bSeekNext || (bError && bBadValue);
                if (bSeekNext) bError = false;
            }
        }

        if (! bError)
        {
            m_nActualDataStartPos =  ReadPosition(page.file, bError);
            ReadDataAndRecEndDelim(nPageIdx, bError, bBadValue);
            bSeekNext = bSeekNext || (bError && bBadValue);
            if (bSeekNext) bError = false;
        }

        if (bError && feof(page.file))
        {
            bError = false;

            if (nPageIdx == LastPageIdx())
            {
                bStop = true;
            } 
			else
            {
                if (bReset)
                {
                    /* long nTest = page.nStartPosition;
                    nTest = ReadPosition(page.file, bError); */
                    page.nStartPosition = ReadPosition(page.file, bError);
                }

                if (! bError)
                { // Next page:
                    nPageIdx++;
                    Page& pageNew = m_vectPages[nPageIdx];
                    OpenPage(nPageIdx);
                    SeekPage(pageNew, pageNew.nStartPosition, SEEK_SET, bError);
                }
            }
        } 
		else
            bStop = ! bSeekNext;

        if (bError)
        {
            std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
            KLERR_MYTHROW2(ERR_CANT_READ2, m_vectPages[nPageIdx].wstrFileName.c_str(), wstrErrorDescr.c_str());
        }
    }

    if (bReset)
    {
        /* long nTest = m_vectPages[nPageIdx].nStartPosition;
        nTest = ReadPosition(m_vectPages[nPageIdx].file, bError); */
        if (feof(m_vectPages[nPageIdx].file))
            m_vectPages[nPageIdx].nStartPosition = ReadPosition(m_vectPages[nPageIdx].file, bError);
		else
            m_vectPages[nPageIdx].nStartPosition = m_nCurrentStartPos;
    }
}

void JournalImpl::ReadDataAndRecEndDelim(int nPageIdx, bool & bError, bool & bBadValue)
{
    KLSTD_ASSERT((nPageIdx >= 0) && ((unsigned int)nPageIdx < m_vectPages.size()));

    Page& page = m_vectPages[ nPageIdx ];
    long nInitialPos = ReadPosition(page.file, bError);
    if (!bError)
    {
        bool bStop = false;

        char szBuf[READ_BUFFER_SIZE];

        m_strDataCurrent.erase();

        while(!bStop)
        {
            size_t nBytesRead = fread(szBuf, 1, READ_BUFFER_SIZE, page.file);

            if (nBytesRead == 0)
            {
                bBadValue = feof(page.file) != 0;
                bError = true;
                bStop = true;
            } 
			else
            {
	            std::string strBuff((char*)szBuf, nBytesRead);
			    if (m_strDataCurrent.capacity() < m_strDataCurrent.size() + strBuff.size())
				    m_strDataCurrent.reserve(m_strDataCurrent.size() * 2);

                int nPrevDataSize = m_strDataCurrent.size();
                m_strDataCurrent += strBuff;

                int nIndexNextRec = FindNonDuplicated(nPrevDataSize - 2 * REC_BEGIN_SIZE, m_strDataCurrent, JRNL_DELIM_RECORD_BEGIN);
                int nIndexRecordEnd = FindNonDuplicated(nPrevDataSize - 2 * REC_END_SIZE, m_strDataCurrent, JRNL_DELIM_RECORD_END);
                bool bNextRecFound = nIndexNextRec != std::string::npos;
                bool bRecEndFound = nIndexRecordEnd != std::string::npos;
                if (bRecEndFound || bNextRecFound || feof(page.file))
                {
                    bStop = true;

                    // bool bWasEof = false;
                    long nNextRecStart;
                    if (bNextRecFound)
                    {
                        nNextRecStart = nInitialPos + nIndexNextRec;
                    } 
					else 
					if (bRecEndFound)
                    {
                        nNextRecStart = nInitialPos + nIndexRecordEnd + strlen(JRNL_DELIM_RECORD_END);
                    } else
                    {
                        // bWasEof = true;
                        nNextRecStart = nInitialPos + nBytesRead;
                    }

                    // return file pointer back to start of JRNL_DELIM, which is next record start
                    // bool bEof1 = feof(page.file) != 0;
                    SeekPage(page, nNextRecStart, SEEK_SET, bError);
                    // bool bEof2 = feof(page.file) != 0;

                    if (! bError)
                    {
                        if (! bRecEndFound)
                        { // next record started, but this record not finished
                            bError = true;
                            bBadValue = true;
                        } 
						else
                        {
                            bool bRecordEndOk;
                            if (bNextRecFound)
                                bRecordEndOk = nIndexRecordEnd < nIndexNextRec;
							else
                                bRecordEndOk = true;

                            if (! bRecordEndOk)
                            {
                                bError = true;
                                bBadValue = true;
                            } 
							else
                                m_strDataCurrent.erase(nIndexRecordEnd);
                        }
                    }
                }
            }
        }

        if (! bError)
        {
            RemoveDuplicates(m_strDataCurrent, JRNL_DELIM);
            RemoveDuplicates(m_strDataCurrent, JRNL_DELIM_RECORD_END);
        }
    }
}

void JournalImpl::Next()
{
    KLSTD::AutoCriticalSection acs(m_pCS);
    if (! IsEOFInternal(m_nCurrentPage)) ReadRecord(m_nCurrentPage, false);
}

bool JournalImpl::IsEOF()
{
    KLSTD::AutoCriticalSection acs(m_pCS);
    return IsEOFInternal(m_nCurrentPage);
}

bool JournalImpl::IsEOFInternal(int nPageIdx)
{
    if (m_nInvalidState) KLERR_MYTHROW0(ERR_PREVIOUS_OPERATION_FAILED);
    if (!IsOpenedInternal()) KLERR_MYTHROW0(ERR_CLOSED);
    if (!m_bReadAllowed) KLERR_MYTHROW0(ERR_READ_NOT_ALLOWED);
    if (nPageIdx < 0) KLERR_MYTHROW0(ERR_NO_CURRENT_POS);
    if ((unsigned int)nPageIdx >= m_vectPages.size()) return true;
    return (nPageIdx == LastPageIdx()) && (feof(GetLastPage().file));
}

void JournalImpl::GetCurrent(long & nId, JournalRecord* pRecord)
{
    KLSTD::AutoCriticalSection acs(m_pCS);

    GetCurrentCommon();
    pRecord->Deserialize(m_strDataCurrent);
    nId = m_nCurrentId;
}

void JournalImpl::GetCurrent(long & nId, Params** ppParams)
{
    KLSTD::AutoCriticalSection acs(m_pCS);

    GetCurrentCommon();
    KLPAR_DeserializeFromMemory(m_strDataCurrent.c_str(), m_strDataCurrent.size(), ppParams);
    nId = m_nCurrentId;
}

void JournalImpl::GetCurrent(long & nId, std::string & strData)
{
    KLSTD::AutoCriticalSection acs(m_pCS);

    GetCurrentCommon();
    strData = m_strDataCurrent;
    nId = m_nCurrentId;
}

void JournalImpl::GetCurrentCommon()
{
    if (m_nInvalidState) KLERR_MYTHROW0(ERR_PREVIOUS_OPERATION_FAILED);
    if (!IsOpenedInternal()) KLERR_MYTHROW0(ERR_CLOSED);
    if (!m_bReadAllowed) KLERR_MYTHROW0(ERR_READ_NOT_ALLOWED);
    if ((m_nCurrentPage < 0) || (IsEOFInternal(m_nCurrentPage))) KLERR_MYTHROW0(ERR_NO_CURRENT_POS);
}

void JournalImpl::DeleteOldRecords(long nRecordsToLeave)
{
    KLSTD::AutoCriticalSection acs(m_pCS);

    if (m_nInvalidState) KLERR_MYTHROW0(ERR_PREVIOUS_OPERATION_FAILED);
    if (!IsOpenedInternal()) KLERR_MYTHROW0(ERR_CLOSED);
    if (!m_bChangeAllowed) KLERR_MYTHROW0(ERR_CHANGE_NOT_ALLOWED);

    m_nCurrentPage = -1;
    if (m_vectPages.size() == 0) return;

    // nBorderPageIdx - lowest page index from range of pages with records to leave
    int nBorderPageIdx;
    unsigned int nRecCountToKillInBorderPage = 0;
    unsigned int i;

    int nRecCount = 0;
    for(i = m_vectPages.size()-1; i >= 0; i--)
    {
        nBorderPageIdx = i;
        OpenPage(i);
        nRecCount += m_vectPages[i].nTotalRecCount - m_vectPages[i].nDeletedRecCount;

        if (nRecCount >= nRecordsToLeave)
        {
            nRecCountToKillInBorderPage = nRecCount - nRecordsToLeave;
            break;
        }
    }

    size_t nFinalSize = m_vectPages.size() - nBorderPageIdx;
    while(m_vectPages.size() > nFinalSize)
        RemovePage(0);

    int nPageIdx = 0;
    bool bError = false;
    SeekPage(m_vectPages[nPageIdx], m_vectPages[nPageIdx].nStartPosition, SEEK_SET, bError);
    if (bError)
    {
        std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
        KLERR_MYTHROW2(ERR_CANT_READ2, m_vectPages[nPageIdx].wstrFileName.c_str(), wstrErrorDescr.c_str());
    }

    ReadRecord(nPageIdx, true);
    for(i = 0; i < nRecCountToKillInBorderPage; i++)
    {
        if (IsEOFInternal(nPageIdx)) break;
        DeleteCurrentInternal(nPageIdx);
    }

    RebuildIndex(0);
}

void JournalImpl::DeleteCurrent()
{
    KLSTD::AutoCriticalSection acs(m_pCS);

    if (m_nInvalidState) KLERR_MYTHROW0(ERR_PREVIOUS_OPERATION_FAILED);
    if (!IsOpenedInternal()) KLERR_MYTHROW0(ERR_CLOSED);
    if (!m_bChangeAllowed) KLERR_MYTHROW0(ERR_CHANGE_NOT_ALLOWED);
    if ((m_nCurrentPage < 0) || (IsEOFInternal(m_nCurrentPage))) KLERR_MYTHROW0(ERR_NO_CURRENT_POS);

    DeleteCurrentInternal(m_nCurrentPage);
    RebuildIndex(m_nCurrentPage);
}

void JournalImpl::DeleteCurrentInternal(int & nPageIdx)
{
    bool bError = false;

    Page& page = m_vectPages[nPageIdx];

    long nPageCurrPosition = ReadPosition(page.file, bError);

    if (!bError)
    {
        SetChangingState(true);
        SetPageChangingState(nPageIdx, true);
        SeekPage(page, m_nCurrentDeletedFlagPos, SEEK_SET, bError);
        WriteFlag(true, page.file, TEXT_REPR_DELETED, bError);
        SeekPage(page, nPageCurrPosition, SEEK_SET, bError);
    }

    if (!bError && (page.nStartPosition == m_nCurrentStartPos))
        page.nStartPosition = ReadPosition(page.file, bError);

    if (bError)
    {
        std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
        KLERR_MYTHROW2(ERR_CANT_WRITE2, page.wstrFileName.c_str(), wstrErrorDescr.c_str());
    }

    page.nDeletedRecCount++;

    if (page.bPageIsFull && (page.nDeletedRecCount == page.nTotalRecCount))
    {
        RemovePage(nPageIdx);

        if ((unsigned int)nPageIdx < m_vectPages.size())
        {
            OpenPage(nPageIdx);
            SeekPage(m_vectPages[nPageIdx], m_vectPages[nPageIdx].nStartPosition, SEEK_SET, bError);
        }
    }

    if (!IsEOFInternal(nPageIdx)) ReadRecord(nPageIdx, false);
}

void JournalImpl::DeleteOlderThanID(long nID)
{
    nID++;

    KLSTD::AutoCriticalSection acs(m_pCS);

    if (m_nInvalidState) KLERR_MYTHROW0(ERR_PREVIOUS_OPERATION_FAILED);
    if (!IsOpenedInternal()) KLERR_MYTHROW0(ERR_CLOSED);
    if (!m_bChangeAllowed) KLERR_MYTHROW0(ERR_CHANGE_NOT_ALLOWED);

    m_nCurrentPage = -1;
    if (nID > m_nIdentity)
    {
        DeleteAllInternal();
        return;
    }

    int nBorderPageIdx = FindBorderPage(nID, false);

    // nBorderPageIdx - greatest page index from range of pages with records to remove
    if (nBorderPageIdx >= 0)
    {
        size_t nFinalSize = m_vectPages.size() - nBorderPageIdx;
        while(m_vectPages.size() > nFinalSize)
            RemovePage(0);

        int nPageIdx = 0;
        bool bError = false;
        SeekPage(m_vectPages[nPageIdx], m_vectPages[nPageIdx].nStartPosition, SEEK_SET, bError);
        if (bError)
        {
            std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
            KLERR_MYTHROW2(ERR_CANT_READ2, m_vectPages[nPageIdx].wstrFileName.c_str(), wstrErrorDescr.c_str());
        }

        ReadRecord(nPageIdx, true);
        while(!IsEOFInternal(nPageIdx))
        {
            if (m_nCurrentId < nID)
				DeleteCurrentInternal(nPageIdx);
            else
                break;
        }

        RebuildIndex(0);
    }
}

void JournalImpl::DeleteRanges(const std::vector< std::pair<long, long> > & vectRanges)
{
    KLSTD::AutoCriticalSection acs(m_pCS);

    if (m_nInvalidState) KLERR_MYTHROW0(ERR_PREVIOUS_OPERATION_FAILED);
    if (!IsOpenedInternal()) KLERR_MYTHROW0(ERR_CLOSED);
    if (!m_bChangeAllowed) KLERR_MYTHROW0(ERR_CHANGE_NOT_ALLOWED);

    {
        for(unsigned int i = 0; i < m_vectPages.size(); i++)
            m_vectPages[i].bNeedRebuild = false;
    }

    KLERR_TRY
        for(unsigned int nRange = 0; nRange < vectRanges.size(); nRange++)
        {
            long nStart = vectRanges[nRange].first;
            long nFinish = vectRanges[nRange].second;
            if (nStart > nFinish) std::swap(nStart, nFinish);

            SetIterator(nStart);
            while(!IsEOFInternal(m_nCurrentPage))
            {
                if (m_nCurrentId > nFinish) break;
                m_vectPages[m_nCurrentPage].bNeedRebuild = true;
                DeleteCurrentInternal(m_nCurrentPage);
            }
        }

        {
            for(unsigned int i = 0; i < m_vectPages.size(); i++)
            {
                if (m_vectPages[i].bNeedRebuild) RebuildIndex(i);
            }
        }
    KLERR_CATCH(pError)
        m_nInvalidState++;
        KLSTD_TRACE1(1, L"Error during deleting ranges - leaving journal %ls in invalid state:\n", m_wstrCentralFilePath.c_str());
        KLERR_SAY_FAILURE(1, pError);
        KLERR_RETHROW();        
    KLERR_ENDTRY
}

void JournalImpl::RebuildIndex(int nPageIdx)
{
	if (nPageIdx >= (int)m_vectPages.size()) return;
    Page & page = m_vectPages[nPageIdx];
    if (!page.fileIdx) return;

    bool bError = false;
    FlushJrnlFile(page.fileIdx, bError);
    bError = bError || fseek(page.fileIdx, 0, SEEK_SET);
    FlushJrnlFile(page.fileIdx, bError);
    
    int nPageIdx2Read = nPageIdx;

    SeekPage(page, page.nStartPosition, SEEK_SET, bError);
    while(! bError && ! IsEOFInternal(nPageIdx2Read))
    {
        ReadRecord(nPageIdx2Read, false);
        if (nPageIdx2Read != nPageIdx)
            break;
        else
            WriteInt32(m_nCurrentStartPos, page.fileIdx, TEXT_REPR_REC_POS, bError);
    }
    
    FlushJrnlFile(page.fileIdx, bError);
}

int JournalImpl::FindBorderPage(long nID, bool nIdMayBeFirst)
{
    int nBorderPageIdx = -1;

    int nPageIdx = 0;
    while((unsigned int)nPageIdx < m_vectPages.size())
    {
        OpenPage(nPageIdx);

        bool bError = false;
        SeekPage(m_vectPages[nPageIdx], m_vectPages[nPageIdx].nStartPosition, SEEK_SET, bError);
        if (bError)
        {
            std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
            KLERR_MYTHROW2(ERR_CANT_READ2, m_vectPages[nPageIdx].wstrFileName.c_str(), wstrErrorDescr.c_str());
        }

        ReadRecord(nPageIdx, true); // may change nPageIdx
        if (IsEOFInternal(nPageIdx)) break;
        if (nIdMayBeFirst)
        {
            if (m_nCurrentId > nID)
            {
                if (nBorderPageIdx < 0) nBorderPageIdx = 0;
                break;
            }
        } 
		else
	        if (m_nCurrentId >= nID) break;

        nBorderPageIdx = nPageIdx;

        nPageIdx++;
    }
    
    return nBorderPageIdx;
}

void JournalImpl::DeleteAll()
{
    KLSTD::AutoCriticalSection acs(m_pCS);

    DeleteAllInternal();

}

void JournalImpl::DeleteAllInternal()
{
    if (m_nInvalidState) KLERR_MYTHROW0(ERR_PREVIOUS_OPERATION_FAILED);
    if (!IsOpenedInternal()) KLERR_MYTHROW0(ERR_CLOSED);
    if (!m_bChangeAllowed) KLERR_MYTHROW0(ERR_CHANGE_NOT_ALLOWED);

    m_nCurrentPage = -1;
    while(m_vectPages.size()) RemovePage(0);
}

void JournalImpl::WriteFlag(bool bFlag, FILE* file, const char* pcszTextRepr, bool & bError)
{
    if (!bError)
    {
        if (m_creationInfo.bBinaryFormat)
        {
            bError = (EOF == fputc(bFlag ? 1 : 0, file));
        } 
		else
        {
            const char* szFlag = bFlag ? TEXT_REPR_TRUE : TEXT_REPR_FALSE;
            WriteBuffer(pcszTextRepr, strlen(pcszTextRepr), file, bError);
            WriteBuffer(" ", 1, file, bError);
            WriteBuffer(szFlag, strlen(szFlag), file, bError);
            WriteBuffer(JRNL_DELIM, strlen(JRNL_DELIM), file, bError);
        }
    }
}

bool JournalImpl::ReadFlag(FILE* file, const char* pcszTextRepr, bool & bError, bool & bBadValue)
{
    bool bResult = false;

    if (! bError)
    {
        if (m_creationInfo.bBinaryFormat)
        {
            int nFlag = fgetc(file);
            if (nFlag == EOF)
                bError = true;
			else
                bResult = (nFlag != 0);
        } 
		else
        {
            char szBuf[ STATIC_BUF_LEN ];
            bError = (fgets_bin_win32(szBuf, STATIC_BUF_LEN, file) == NULL);
            if (! bError)
            {
                char szFlagBuf[10];
                std::string strFormat(pcszTextRepr);
                strFormat+= " %9s";
                int nScanResult = sscanf(szBuf, strFormat.c_str(), & szFlagBuf);

                if ((! nScanResult) || (nScanResult == EOF))
                {
                    bBadValue = true;
                    bError = true;
                } 
				else
                    bResult = (strcmp(szFlagBuf, TEXT_REPR_FALSE) != 0);
            }
        }
    }

    return bResult;
}

void JournalImpl::WriteInt32(AVP_int32 nValue, FILE* file, const char* pcszTextRepr, bool & bError)
{
    if (!bError)
    {
        if (m_creationInfo.bBinaryFormat)
        {
            AVP_int32 nFileRepr = MachineToLittleEndian(nValue);
            bError = (fwrite(& nFileRepr, sizeof(AVP_int32), 1, file) != 1);
        } 
		else
        {
            long nLongValue = nValue;
			// change TEXT_MODE_IDX_REC_SIZE if change this format.
            int nBytesWritten = fprintf(file, "%s %20ld%s", pcszTextRepr, nLongValue, JRNL_DELIM);
            bError = bError || (nBytesWritten == 0);
        }
    }
}

AVP_int32 JournalImpl::ReadInt32(FILE* file, const char* pcszTextRepr, bool & bError, bool & bBadValue)
{
    AVP_int32 nResult;

    if (!bError)
    {
        if (m_creationInfo.bBinaryFormat)
        {
            bError = (fread(& nResult, sizeof(AVP_int32), 1, file) != 1);
            if (!bError) nResult = LittleEndianToMachine(nResult);
        } 
		else
        {
            char szBuf[ STATIC_BUF_LEN ];
            bError = (fgets_bin_win32(szBuf, STATIC_BUF_LEN, file) == NULL);
            if (!bError)
            {
                std::string strFormat(pcszTextRepr);
                strFormat+= " %ld";
                int nScanResult = sscanf(szBuf, strFormat.c_str(), & nResult);
                if ((!nScanResult) || (nScanResult == EOF))
                {
                    bBadValue = true;
                    bError = true;
                }
            }
        }
    }

    return nResult;
}

void JournalImpl::WriteBuffer(const void* buffer, size_t size, FILE* file, bool & bError)
{
    bError = bError || (fwrite(buffer, 1, size, file) != size);
}

long JournalImpl::ReadPosition(FILE* file, bool & bError)
{
    long nResult = 0;

    if (! bError)
    {
        nResult = ftell(file);
        bError = (nResult < 0);
    }

    return nResult;
}

std::wstring JournalImpl::GetPageFilePathFromAddOn(const std::wstring& wstrAddOn)
{
    std::wstring wstrTemplFileDir; 
    std::wstring wstrTemplFileName;
    std::wstring wstrTemplFileExt;

    KLSTD_SplitPath(m_wstrPageFileTempl, wstrTemplFileDir, wstrTemplFileName, wstrTemplFileExt);

    std::wstring wstrResult, wstrFileName;
    if (wstrAddOn == L"0")
        wstrFileName = wstrTemplFileName + wstrTemplFileExt;
    else
        wstrFileName = wstrTemplFileName + wstrAddOn + wstrTemplFileExt;

    KLSTD_PathAppend(wstrTemplFileDir, wstrFileName, wstrResult, true);

    return wstrResult;
}

std::wstring JournalImpl::GenerateNewPageFileNameAddOn(const std::wstring& wstrPreAddOn)
{
    return wstrPreAddOn + L"_" + KLSTD_CreateGUIDString();
/*
    int nCounter = 0;
    bool bStop = false;
    while(! bStop)
    {
        basic_stringstream<wchar_t> ssAddOn;
        if (wstrPreAddOn.empty())
            ssAddOn << m_vectPages.size();
        else
            ssAddOn << wstrPreAddOn;
        
		
        if (nCounter) ssAddOn << L"_" << nCounter;
        wstrResult = ssAddOn.str();

        bool bUsed = false;
        for(int i = 0; i < vectUsedFileNameAddOns.size(); i++)
        {
            bUsed = vectUsedFileNameAddOns[i] == wstrResult;
            if (bUsed) break;
        }

        bStop = ! bUsed && ! KLSTD_IfExists(GetPageFilePathFromAddOn(wstrResult).c_str());
        nCounter++;
    }

    vectUsedFileNameAddOns.push_back(wstrResult);

    return wstrResult;*/
}

void JournalImpl::FlushJrnlFile(FILE* file, bool & bError)
{
    if (! bError)
    {
        long nPos = ReadPosition(file, bError);
        bError = bError || fflush(file);
        bError = bError || fseek(file, nPos, SEEK_SET);
    }
}

void JournalImpl::SetChangingState(bool bState)
{
    if (m_bChangingState != bState)
    {
        bool bError = false;
        bError = bError || fseek(m_fileCentral, m_nCentralChangingFlagPos, SEEK_SET);
        WriteFlag(bState, m_fileCentral, TEXT_REPR_CHANGING, bError);

        if (!bError) m_bChangingState = bState;

        FlushJrnlFile(m_fileCentral, bError);
        if (bError)
        {
            std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
            KLERR_MYTHROW2(ERR_CANT_WRITE2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str());
        }
    }
}

void JournalImpl::SetPageChangingState(int nPageIdx, bool bState)
{
    KLSTD_ASSERT((nPageIdx >= 0) && ((unsigned int)nPageIdx < m_vectPages.size()));

    Page& page = m_vectPages[ nPageIdx ];
    if (page.bChanging != bState)
    {
        bool bError = false;

        SeekPage(page, page.nChangingFlagPos, SEEK_SET, bError);
        WriteFlag(bState, page.file, TEXT_REPR_CHANGING, bError);

        page.bChanging = bState;
        FlushJrnlFile(page.file, bError);

        if (bError)
        {
            std::wstring wstrErrorDescr = (const wchar_t*)KLSTD_A2CW2(strerror(errno));
            KLERR_MYTHROW2(ERR_CANT_WRITE2, page.wstrFileName.c_str(), wstrErrorDescr.c_str());
        }
    }
}

void JournalImpl::SeekPage(Page& page, long offset, int origin, bool & bError)
{
    if (!bError)
    {
        bError = bError || fseek(page.file, offset, origin);
        page.bAtEOF = false;
    }
}

void JournalImpl::PageToEOF(Page& page, bool & bError)
{
    if (!bError)
    {
        if (!page.bAtEOF)
        {
            bError = bError || fseek(page.file, 0, SEEK_END);
            if (page.fileIdx) bError = bError || fseek(page.fileIdx, 0, SEEK_END);
            if (! bError) page.bAtEOF = true;
        }
    }
}

void JournalImpl::TryDeletePagePhysically(const Page & page, bool & bError)
{
    if (! bError)
    {
        KLSTD_Unlink(page.wstrFileName.c_str(), false);
        bool bStillOk = ! KLSTD_IfExists(page.wstrFileName.c_str());

        if (bStillOk && ! page.wstrIdxFileName.empty())
        {
            KLSTD_Unlink(page.wstrIdxFileName.c_str(), false);
            bStillOk &= ! KLSTD_IfExists(page.wstrIdxFileName.c_str());
        }

        if (bStillOk)
        {
            long nCurrPos = ReadPosition(m_fileCentral, bError);
            bError = bError || fseek(m_fileCentral, page.nPhysDeletedFlagPos, SEEK_SET);
            WriteFlag(true, m_fileCentral, TEXT_REPR_FILE_PHYS_DELETED, bError);
            bError = bError || fseek(m_fileCentral, nCurrPos, SEEK_SET);
        }
    }
}

void KLJRNL_CreateJournal(KLJRNL::Journal** ppJournal, bool bMultiThread)
{
    KLSTD_CHKOUTPTR(ppJournal);
    KLSTD::CAutoPtr<JournalImpl> pJournalImpl;
    pJournalImpl.Attach(new JournalImpl(bMultiThread));
    KLSTD_CHKMEM(pJournalImpl);

    *ppJournal = pJournalImpl.Detach();
}
