/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	jrnl/journalimpl.h
 * \author	Mikhail Karmazine
 * \date	22.01.2003 12:02:39
 * \brief	Заголовок реализации интерфейса KLJRNL::Journal
 * 
 */

#ifndef __JOURNALIMPL_H__
#define __JOURNALIMPL_H__

#include "journal.h"
#include "common.h"

namespace KLJRNL
{

// cached data
struct Page
{
    std::wstring wstrFileName;
    std::wstring wstrIdxFileName;
    long nMarkedAsDeletedFlagPos;
    long nPhysDeletedFlagPos;
    long nFileNamePos;

    FILE* file; // if file == NULL, then other fields have no meaning
    FILE* fileIdx;

    long nTotalRecCountPos;
    long nTotalRecCount;

    long nDeletedRecCountPos;
    long nDeletedRecCount;

    long nChangingFlagPos;
    bool bChanging;

    long nStartPositionPos;
    long nStartPosition;

    long nPageSize;
    bool bPageIsFull;

    bool bAtEOF;

    bool bNeedRebuild;

    // use constructor to assign initial values of new page object - this helps
    // to not forget all necessary values in all places where new objects
    // are creating.
    Page(const std::wstring & _wstrFileName,
		 const std::wstring & _wstrIdxFileName,
		 long _nMarkedAsDeletedFlagPos,
		 long _nPhysDeletedFlagPos,
		 long _nFileNamePos,
		 FILE* _file,
		 FILE* _fileIdx,
		 long _nTotalRecCountPos,
		 long _nTotalRecCount,
		 long _nDeletedRecCountPos,
		 long _nDeletedRecCount,
		 long _nChangingFlagPos,
		 long _nStartPositionPos,
		 long _nStartPosition,
		 long _nPageSize,
		 bool _bPageIsFull)
    {
        bChanging = false;
        bAtEOF = false;

        wstrFileName = _wstrFileName;
        wstrIdxFileName = _wstrIdxFileName;
        nMarkedAsDeletedFlagPos = _nMarkedAsDeletedFlagPos;
        nPhysDeletedFlagPos = _nPhysDeletedFlagPos;
        nFileNamePos = _nFileNamePos;
        file = _file;
        fileIdx = _fileIdx;

        nTotalRecCountPos = _nTotalRecCountPos;
        nTotalRecCount = _nTotalRecCount;

        nDeletedRecCountPos = _nDeletedRecCountPos;
        nDeletedRecCount = _nDeletedRecCount;

        nChangingFlagPos = _nChangingFlagPos;

        nStartPositionPos = _nStartPositionPos;
        nStartPosition = _nStartPosition;

        nPageSize = _nPageSize;
        bPageIsFull = _bPageIsFull;
    }
};

struct SeekContext
{
    SeekContext(long _nId, long _nBegin, long _nEnd) :
        nBegin(_nBegin),
        nEnd(_nEnd)
    {
        nId = _nId;
        SetBegin(_nBegin);
        SetEnd(_nEnd);
    }

    long nLeftRecId;
    long nLeftRecEnd;

    inline long GetId() { 
		return nId; 
	}
    inline long GetBegin() { 
		return nBegin; 
	}
    inline void SetBegin(long nVal)
    {
        nBegin = nVal;
        nLeftRecId = -1;
        nLeftRecEnd = -1;
    }
    inline long GetEnd() { 
		return nEnd; 
	}
    inline void SetEnd(long nVal) {
        nEnd = nVal;
    }

private:
    long nBegin;
    long nEnd;
    long nId;
};

class JournalImpl : public KLSTD::KLBaseImpl<Journal>
{
public:
    JournalImpl( bool bMultiThread );
    ~JournalImpl();

    void Open(const std::wstring& wstrPath,
			  KLSTD::CREATION_FLAGS flagsCreation,
			  KLSTD::ACCESS_FLAGS flagsAccessFlags,
			  const CreationInfo& creationInfo = CreationInfo(),
			  long nTimeout = KLJRNL_DEF_OPEN_TIMEOUT );
    
    void GetUsedCreationInfo( CreationInfo & creationInfo );
    void ChangeLimits(const std::wstring& wstrPath, const CreationInfo& creationInfo);
	void ShrinkCentralFile(const std::wstring& wstrPath, const CreationInfo& creationInfo = CreationInfo(), long nTimeout = KLJRNL_DEF_OPEN_TIMEOUT);
    void Close();
    void CloseAndRemoveFiles();
    bool IsOpened();
    AVP_longlong GetJournalSize();
	
	/*
		Функция возвращает строку-идентификатор .ctrl файла журнала. 
		Идентификатор меняется при каждом создании .ctrl файла
	*/
	std::string GetJournalId();

    void Repair(const std::wstring & wstrPath, const CreationInfo & creationInfo);

    long Add(const KLPAR::Params* pParams);
    long Add(JournalRecord* pRecord);
    long Add(const std::string & str);
    void Update(const KLPAR::Params* pParams);
    void Update(JournalRecord* pRecord);
    void Update(const std::string & str);
    void Flush();

    void MoveToRecordN(long nRecIdx);
    void ResetIterator();
    bool SetIterator(long nId );
    void ResetIteratorEx(std::string& strJournalID);
    bool SetIteratorEx(long nId, std::string& strJournalID);

    long GetRecordCount();
    void Next();
    bool IsEOF();
    void GetCurrent(long & nId, JournalRecord* pRecord);
    void GetCurrent(long & nId, KLPAR::Params** ppParams);
    void GetCurrent(long & nId, std::string & strData);

    void DeleteOldRecords(long nRecordsToLeave);
    void DeleteCurrent();
    void DeleteOlderThanID(long nID);
    void DeleteRanges(const std::vector<std::pair<long, long> >& vectRanges);
    void DeleteAll();

    void AddInternal(long nId, std::string& strData);
    void UpdateInternal(std::string& strData);

    void SavePageHeader(Page& page);

    std::wstring m_wstrPageFileTempl;
    std::wstring m_wstrCentralFilePath;

private:
    KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;
    CreationInfo m_creationInfo;
    FILE* m_fileCentral;
	bool m_bCtrlFileNotCreated;

// cached data
    std::vector<Page> m_vectPages;
    long m_nCentralChangingFlagPos;
    long m_nFinishRecEndPos;
    long m_nIdentityPos;
    long m_nIdentity;

	// use this function to set inital cached data - this
	// helps to not forget all necessary values in all places 
	// where initializig done.
    void SetCachedData(long nCentralChangingFlagPos,     
					   long nFinishRecEndPos,            
					   long nIdentityPos,
					   long nIdentity)
    {
        m_nCentralChangingFlagPos = nCentralChangingFlagPos;
        m_nFinishRecEndPos = nFinishRecEndPos;
        m_nIdentityPos = nIdentityPos;
        m_nIdentity = nIdentity;
    }
// end of cached data

    std::vector<std::wstring> vectUsedFileNameAddOns;

    // *Internal methods - the same as not internal, but without critical sections check:
    void DeleteAllInternal();
    bool IsOpenedInternal();
    void CloseInternal();
    bool IsEOFInternal(int nPageIdx);
    void DeleteCurrentInternal(int & nPageIdx);
    void OpenInternal(const std::wstring& wstrPath,
					  KLSTD::CREATION_FLAGS flagsCreation,
					  KLSTD::ACCESS_FLAGS flagsAccessFlags,
					  const CreationInfo& creationInfo,
					  long nTimeout );


    FILE* _OpenFile(const std::wstring& wstrPath, 
					KLSTD::CREATION_FLAGS flagsCreation, 
					KLSTD::ACCESS_FLAGS flagsAccessFlags, 
					long nTimeout);
	void CloseNoThrow();

    void WriteFlag(bool bFlag, FILE* file, const char* pcszTextRepr, bool & bError);
    bool ReadFlag(FILE* file, const char* pcszTextRepr, bool & bError, bool & bBadValue);
    void WriteInt32(AVP_int32 nValue, FILE* file, const char* pcszTextRepr, bool & bError);
    AVP_int32 ReadInt32(FILE* file, const char* pcszTextRepr, bool & bError, bool & bBadValue);
    void WriteBuffer(const void* buffer, size_t size, FILE* file, bool & bError);
    long ReadPosition(FILE* file, bool & bError);

    bool IsLastPageOpened()
    {
        int nSize = m_vectPages.size();
        return ( nSize > 0 ) && ( m_vectPages[nSize-1].file );
    }
    int LastPageIdx() {
		return m_vectPages.size()-1; 
	}
    Page& GetLastPage() { 
		return m_vectPages[LastPageIdx()]; 
	}

    bool m_bChangeAllowed, m_bReadAllowed;
    bool m_bInfoDataMustExists;
    bool m_bChangingState, m_nInvalidState, m_bChangeLimits;
    int m_nCurrentPage;

    /* this values may have significant values even when m_nCurrentPage = -1: */
    long m_nCurrentStartPos, m_nCurrentDeletedFlagPos, m_nActualDataStartPos;
    long m_nCurrentId;
    std::string m_strDataCurrent;

    Page& GetCurrPage() {
		return m_vectPages[m_nCurrentPage]; 
	}

    void OpenCentralFile(KLSTD::CREATION_FLAGS flagsCreation, KLSTD::ACCESS_FLAGS flagsAccessFlags, long nTimeout);
    bool CheckCentralFileSignature();
    void CreateNewCentralFileData(const CreationInfo & creationInfo);
    void ReadCentralFileData();
    void OpenPage(int nPageIdx);
	void CreateCentralFileIdFile(const std::wstring& wstrCentralFilePath);
	std::string ReadCentralFileIdFile(const std::wstring& wstrCentralFilePath);

    FILE * OpenFileRaw(const std::wstring & wstrFileName, 
					   AVP_dword dwSharing,
					   AVP_dword dwCreation, 
					   AVP_dword dwFlags);

    void CreateAndOpenNewPage();
    std::wstring GetPageFilePathFromAddOn(const std::wstring& wstrAddOn);
    std::wstring GenerateNewPageFileNameAddOn(const std::wstring& wstrPreAddOn);
    std::wstring GetCentralFile(const std::wstring & wstrTargetJournalFile);

    void FlushJrnlFile(FILE* file, bool& bError);

    void ClosePage(int nPageIdx);
    void RemovePage(int nPageIdx);
    void PrepareLastPage();
    void SetChangingState(bool bState);
    void SetPageChangingState(int nPageIdx, bool bState);
    void ReleaseAllOpenedStuffNoThrow();
    void SeekPage(Page& page, long offset, int origin, bool & bError);
    void PageToEOF(Page& page, bool & bError);
    void GetCurrentCommon();
    
    int FindBorderPage(long nID, bool nIdMayBeFirst);
    void RebuildIndex(int nPageIdx);

    void SeekRecord(SeekContext & seekContext);
    void GoToEOF();

    void ReadRecord(int & nPageIdx, bool bReset);
    void ReadDataAndRecEndDelim(int nPageIdx, bool & bError, bool & bBadValue);
    void TryDeletePagePhysically(const Page & page, bool & bError);
};

}

#endif //__JOURNALIMPL_H__
