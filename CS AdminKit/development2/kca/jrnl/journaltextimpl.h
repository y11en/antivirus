/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	jrnl/journalimpl.h
 * \author	Mikhail Karmazine
 * \date	22.01.2003 12:02:39
 * \brief	Заголовок реализации интерфейса KLJRNL::Journal
 * 
 */

#ifndef __JOURNALTEXTIMPL_H__
#define __JOURNALTEXTIMPL_H__

#include "journal.h"
#include "common.h"

namespace KLJRNL
{

    // cached data
    struct PageText
    {
        long nPageRecStartPos;

        long nStartId;
        long nStartPosition;
        bool bMarkedAsDeletedFlag;
        bool bPhysDeletedFlag;
        long nTotalRecCount;
        long nDeletedRecCount;

        std::wstring wstrFileName;

        FILE* file;

        // calculated data
        bool bPageIsFull;
        bool bAtEOF;

        PageText()
            : nPageRecStartPos(0)
            , bMarkedAsDeletedFlag(false)
            , bPhysDeletedFlag(false)
            , nTotalRecCount(0)
            , nDeletedRecCount(0)
            , nStartId(0)
            , file(NULL)
            , bPageIsFull(false)
            , bAtEOF(false)
        {};
    };


    class JournalTextImpl : public KLSTD::KLBaseImpl<Journal>
    {
    public:
        JournalTextImpl( bool bMultiThread );

        ~JournalTextImpl();

        virtual void Open(
            const std::wstring &    wstrPath,
            KLSTD::CREATION_FLAGS   flagsCreation,
            KLSTD::ACCESS_FLAGS     flagsAccessFlags,
            const CreationInfo &    creationInfo = CreationInfo(),
            long                    nTimeout = KLJRNL_DEF_OPEN_TIMEOUT );

        virtual void GetUsedCreationInfo( CreationInfo & creationInfo );

        virtual void ChangeLimits(
            const std::wstring &    wstrPath,
            const CreationInfo &    creationInfo );

		virtual void ShrinkCentralFile(            
			const std::wstring &    wstrPath,
            const CreationInfo &    creationInfo = CreationInfo(),
            long                    nTimeout = KLJRNL_DEF_OPEN_TIMEOUT ){};

        virtual void Close();

        virtual void CloseAndRemoveFiles();

        virtual bool IsOpened();
        
        virtual AVP_longlong GetJournalSize();

        virtual void Repair( const std::wstring & wstrPath, const CreationInfo & creationInfo );

        virtual long Add( const KLPAR::Params* pParams );
        virtual long Add( JournalRecord* pRecord );
        virtual long Add( const std::string & str );
        virtual void Update( const KLPAR::Params* pParams );
        virtual void Update( JournalRecord* pRecord );
        virtual void Update( const std::string & str );
        virtual void Flush();

        virtual void ResetIterator();
        virtual bool SetIterator( long nId );
        virtual void MoveToRecordN( long nRecIdx );
		virtual void ResetIteratorEx(std::string& strJournalID){
			ResetIterator();
		}
		virtual bool SetIteratorEx(long nId, std::string& strJournalID){
			return SetIterator(nId);
		}
        virtual long GetRecordCount();

        virtual void Next();
        virtual bool IsEOF();
        virtual void GetCurrent( long & nId, JournalRecord* pRecord );
        virtual void GetCurrent( long & nId, KLPAR::Params** ppParams );
        virtual void GetCurrent( long & nId, std::string & strData );

        virtual void DeleteOldRecords( long nRecordsToLeave );
        virtual void DeleteCurrent();
        virtual void DeleteOlderThanID( long nID );
        virtual void DeleteRanges( const std::vector< std::pair<long, long> > & vectRanges );
        virtual void DeleteAll();
        
        virtual void AddInternal( long & nId, std::string& strData );
        virtual void UpdateInternal( std::string& strData );

        std::wstring m_wstrPageFileTempl;
        std::wstring m_wstrCentralFilePath;
    private:
        KLSTD::CAutoPtr<KLSTD::CriticalSection> m_pCS;

        CreationInfo m_creationInfo;

        FILE* m_fileCentral;

        // cached data
        long m_nIdentityPos;
        long m_nIdentity;
        std::vector<PageText> m_vectPages;

        bool IsOpenedInternal();
        void CloseInternal();
        
        void OpenInternal(
            const std::wstring &    wstrPath,
            KLSTD::CREATION_FLAGS   flagsCreation,
            KLSTD::ACCESS_FLAGS     flagsAccessFlags,
            const CreationInfo &    creationInfo,
            long                    nTimeout );

        void DeleteAllInternal();
        bool IsEOFInternal( int nPageIdx );
        void DeleteCurrentInternal( int & nPageIdx );
        bool CheckCentralFileSignature();
        void ReadCentralFileData();
        void CreateNewCentralFileData( const CreationInfo & creationInfo );
        void CloseNoThrow();
        void RewritePageData( PageText& page );
        void RewriteIdentity();
        int FindBorderPage( long nID, bool nIdMayBeFirst );
        bool IsContainsSpacesOnly( const std::string & strData );
        

        long m_nCentralChangingFlagPos;
        //long m_nFinishRecEndPos;
        // end of cached data

        std::vector<std::wstring> vectUsedFileNameAddOns;

        // *Internal methods - the same as not internal, but without critical sections check:



        void WriteFlag( bool bFlag, FILE* file, const char* pcszTextRepr, bool & bError );
        bool ReadFlag( FILE* file, const char* pcszTextRepr, bool & bError, bool & bBadValue );

        void WriteInt32( AVP_int32 nValue, FILE* file, const char* pcszTextRepr, bool & bError );
        AVP_int32 ReadInt32( FILE* file, const char* pcszTextRepr, bool & bError, bool & bBadValue );

        void WriteBuffer( const void* buffer, size_t size, FILE* file, bool & bError );

        long ReadPosition( FILE* file, bool & bError );

        bool IsLastPageOpened()
        {
            int nSize = m_vectPages.size();
            return ( nSize > 0 ) && ( m_vectPages[nSize-1].file );
        }

        int LastPageIdx() { return m_vectPages.size()-1; }
        PageText& GetLastPage() { return m_vectPages[LastPageIdx()]; }

        bool m_bChangeAllowed;
        bool m_bReadAllowed;
        bool m_bInfoDataMustExists;
        long m_nCurrentId;
        bool m_bChangeLimits;
        long m_nCurrentStartPos;

        int m_nCurrentPage;

        /* this values may have significant values even when m_nCurrentPage = -1: */
        std::string m_strDataCurrent;

        PageText& GetCurrPage() { return m_vectPages[m_nCurrentPage]; }

        void OpenCentralFile( KLSTD::CREATION_FLAGS   flagsCreation,
                              KLSTD::ACCESS_FLAGS     flagsAccessFlags,
                              long                    nTimeout );


        void OpenPage( int nPageIdx );
        void CreateAndOpenNewPage();

        std::wstring GetPageFilePathFromAddOn( const std::wstring& wstrAddOn );
        std::wstring GenerateNewPageFileNameAddOn();
        std::wstring GetCentralFile( const std::wstring & wstrTargetJournalFile );
        void FlushJrnlFile( FILE* file, bool & bError );

        void ClosePage( int nPageIdx );
        void RemovePage( int nPageIdx );
        void PrepareLastPage();
        void ReleaseAllOpenedStuffNoThrow();
        void SeekPage( PageText& page, long offset, int origin, bool & bError );
        void PageToEOF( PageText& page, bool & bError );
        void GetCurrentCommon();

        void ReadRecord( int & nPageIdx, bool bReset );
    };

}

#endif //__JOURNALTEXTIMPL_H__

// Local Variables:
// mode: C++
// End:
