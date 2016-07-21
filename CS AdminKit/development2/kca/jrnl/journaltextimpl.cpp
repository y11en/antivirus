/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	jrnl/journalimpl.cpp
 * \author	Mikhail Karmazine
 * \date	22.01.2003 12:02:39
 * \brief	Реализация интерфейса KLJRNL::Journal
 * 
 */

#include <stdio.h>
#include "osdep.h"

#include "journaltextimpl.h"

#include <std/conv/klconv.h>
#include <std/err/klerrors.h>

#include <sstream>

#define KLCS_MODULENAME L"KLJRNL"

using namespace KLJRNL;
using namespace KLSTD;
using namespace KLPAR;
using namespace std;

#if defined(_WIN32) && defined(_UNICODE)
//#define UNICODE_JOURNAL
#endif

#ifdef UNICODE_JOURNAL
const char sUnicodeFlag[] = { '\xEF', '\xBB', '\xBF' };
#endif

// TEXT_REPR_TRUE and TEXT_REPR_FALSE shall have the same length
#ifdef UNICODE_JOURNAL
    #define JRNL_CHAR wchar_t
#else
    #define JRNL_CHAR char
#endif

#define CENTRAL_FILE_SIGNATURE  "KLJRNL (text) central file"


JournalTextImpl::JournalTextImpl( bool bMultiThread )
{
    m_fileCentral = NULL;
    // m_nInvalidState = 0;

    if( bMultiThread )
    {
        KLSTD_CreateCriticalSection( & m_pCS );
    }
}

JournalTextImpl::~JournalTextImpl()
{
    AutoCriticalSection acs( m_pCS );

    if( IsOpenedInternal() )
    {
        CloseInternal();
    }
}

wstring JournalTextImpl::GetCentralFile( const wstring & wstrTargetJournalFile )
{
    wstring wstrDir;
    wstring wstrFileName;
    wstring wstrFileExt;

    KLSTD_SplitPath( wstrTargetJournalFile, wstrDir, wstrFileName, wstrFileExt );

    wstring wstrResult;

    KLSTD_PathAppend( wstrDir, wstrFileName + L".ctrl", wstrResult, false );

    return wstrResult;
}

void JournalTextImpl::Open(
    const wstring &         wstrPath,
    CREATION_FLAGS          flagsCreation,
    ACCESS_FLAGS            flagsAccessFlags,
    const CreationInfo &    creationInfo, /* = CreationInfo() */
    long                    nTimeout /* = KLJRNL_DEF_OPEN_TIMEOUT */ )
{
    AutoCriticalSection acs( m_pCS );

    m_bChangeLimits = false;

	m_wstrCentralFilePath = GetCentralFile( wstrPath );

    if (( flagsCreation & CF_CREATE_NEW ) == CF_CREATE_NEW)
    {
		if ((flagsCreation & CF_CLEAR) == CF_CLEAR ||
			! KLSTD_IfExists( m_wstrCentralFilePath.c_str()))
		{
			KLERR_BEGIN
			{
//				_DeleteLostFiles(wstrPath);
				OpenCentralFile(CF_CREATE_ALWAYS, KLSTD::ACCESS_FLAGS(AF_WRITE | AF_READ), nTimeout );
				CreateNewCentralFileData( creationInfo );
			}
			KLERR_END
		
			Close();
		}
    }

	// File should exist at this moment
    OpenInternal(wstrPath, CF_OPEN_EXISTING, flagsAccessFlags, creationInfo, nTimeout);
	
//    AutoCriticalSection acs( m_pCS );
//
//    m_bChangeLimits = false;
//
//    if( ( flagsAccessFlags == AF_READ ) && ( flagsCreation & CF_CREATE_NEW ) )
//    {
//        if( ! KLSTD_IfExists( GetCentralFile( wstrPath ).c_str() ) )
//        {
//            CAutoPtr<JournalTextImpl> pNewJrnl;
//            pNewJrnl.Attach( new JournalTextImpl( false ) );
//            KLSTD_CHKMEM( pNewJrnl );
//
//            pNewJrnl->Open( wstrPath, CF_CREATE_NEW, AF_WRITE, creationInfo );
//        }
//    }
//
//    OpenInternal( wstrPath, flagsCreation, flagsAccessFlags, creationInfo, nTimeout );
}

void JournalTextImpl::GetUsedCreationInfo( CreationInfo & creationInfo )
{
    if( ! IsOpenedInternal() )
    {
        KLERR_MYTHROW0( ERR_CLOSED );
    }

    creationInfo = m_creationInfo;
}

void JournalTextImpl::OpenInternal(
    const wstring &         wstrPath,
    CREATION_FLAGS          flagsCreation,
    ACCESS_FLAGS            flagsAccessFlags,
    const CreationInfo &    creationInfo,
    long                    nTimeout )
{
    AutoCriticalSection acs( m_pCS );

    m_nCurrentPage = -1;

    if( IsOpenedInternal() )
    {
        KLERR_MYTHROW0( ERR_OPENED );
    }

    m_wstrPageFileTempl = wstrPath;
    m_wstrCentralFilePath = GetCentralFile( wstrPath );
    if( wstrPath == m_wstrCentralFilePath )
    {
        KLERR_MYTHROW1( ERR_INVALID_JOURNAL_FILE_NAME1, wstrPath.c_str() );
    }

    try
    {
        OpenCentralFile( flagsCreation, flagsAccessFlags, nTimeout );

        if( ! m_fileCentral )
        {
		    KLSTD_USES_CONVERSION;
            wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
            KLERR_MYTHROW2( ERR_CANT_OPEN2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str() );
        }

        if( CheckCentralFileSignature() )
        {
            ReadCentralFileData();
            if( flagsCreation & CF_CLEAR )
            {
                DeleteAllInternal();
            }
        } else
        {
            CreateNewCentralFileData( creationInfo );
        }
    }
    catch(...)
    {
        CloseNoThrow();
        throw;
    }
}

void JournalTextImpl::CloseNoThrow()
{
    KLERR_BEGIN
        if( IsOpenedInternal() )
        {
            CloseInternal();
        }
    KLERR_END;
}

void JournalTextImpl::CreateNewCentralFileData( const CreationInfo & creationInfo )
{
    bool bError = false;

    if( ( creationInfo.pageLimitType == pltRecCount ) && creationInfo.nMaxRecordsInPage < 1 )
    {
        KLERR_MYTHROW1( ERR_MAX_PAGE_REC_COUNT1, m_wstrCentralFilePath.c_str() );
    }

    // set cached values (some values shall be cleared by Close():
    m_creationInfo = creationInfo;

    bError = bError || ( fputs( CENTRAL_FILE_SIGNATURE, m_fileCentral ) == EOF );
    bError = bError || ( fputs( JRNL_DELIM, m_fileCentral ) == EOF );
    WriteInt32( JRNL_VERSION, m_fileCentral, TEXT_REPR_VERSION, bError );
    
    WriteInt32( long( m_creationInfo.pageLimitType ), m_fileCentral, TEXT_REPR_PAGE_LIMIT_TYPE, bError );
    if( m_creationInfo.pageLimitType == pltRecCount )
    {
        WriteInt32( m_creationInfo.nMaxRecordsInPage, m_fileCentral, TEXT_REPR_MAX_REC_IN_PAGE, bError );
    } else
    {
        WriteInt32( m_creationInfo.nMaxPageSize, m_fileCentral, TEXT_REPR_MAX_PAGE_SIZE, bError );
    }

    WriteFlag( m_creationInfo.bLimitPages, m_fileCentral, TEXT_REPR_LIMIT_PAGES, bError );

    m_nIdentityPos = ReadPosition( m_fileCentral, bError );
    m_nIdentity = 0;
    RewriteIdentity();

    if( bError )
    {
		KLSTD_USES_CONVERSION;
        wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
        KLERR_MYTHROW2( ERR_CANT_WRITE2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str() );
    }
}

void JournalTextImpl::RewriteIdentity()
{
    bool bError = false;

    bError = bError || fseek( m_fileCentral, m_nIdentityPos, SEEK_SET );
    WriteInt32( m_nIdentity, m_fileCentral, TEXT_REPR_IDENTITY, bError );

    if( bError )
    {
        KLSTD_USES_CONVERSION;
        wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
        KLERR_MYTHROW2( ERR_CANT_WRITE2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str() );
    }
}

void JournalTextImpl::ReadCentralFileData()
{
    bool bError = false;
    bool bBadValue = false;

    m_creationInfo.bBinaryFormat = false;

    long nJrnlVersion = ReadInt32( m_fileCentral, TEXT_REPR_VERSION, bError, bBadValue );
    if( ! bError )
    {
        if( nJrnlVersion > JRNL_VERSION )
        {
            KLERR_MYTHROW1( ERR_UNSOPPORTED_VERSION1, m_wstrCentralFilePath.c_str() );
        }
    }

    m_creationInfo.pageLimitType =
        PageLimitType( ReadInt32( m_fileCentral, TEXT_REPR_PAGE_LIMIT_TYPE, bError, bBadValue ) );
    if( m_creationInfo.pageLimitType == pltRecCount )
    {
        m_creationInfo.nMaxRecordsInPage =
            ReadInt32( m_fileCentral, TEXT_REPR_MAX_REC_IN_PAGE, bError, bBadValue );
    } else
    {
        m_creationInfo.nMaxPageSize =
            ReadInt32( m_fileCentral, TEXT_REPR_MAX_PAGE_SIZE, bError, bBadValue );
    }

    m_creationInfo.bLimitPages = ReadFlag( m_fileCentral, TEXT_REPR_LIMIT_PAGES, bError, bBadValue );

    m_nIdentityPos = ReadPosition( m_fileCentral, bError );
    m_nIdentity = ReadInt32( m_fileCentral, TEXT_REPR_IDENTITY, bError, bBadValue );

    bool bStopRead = false;
    while( ! bError && ! bStopRead )
    {
        PageText pageText;
        bool bValidRecord = false;

        pageText.nPageRecStartPos = ReadPosition( m_fileCentral, bError );

        pageText.nStartId =
            ReadInt32( m_fileCentral, TEXT_REPR_START_ID, bError, bBadValue );

        if( bError && feof( m_fileCentral ) )
        {
            bError = false;
            break;
        }

        pageText.nStartPosition =
            ReadInt32( m_fileCentral, TEXT_REPR_START_POSITION, bError, bBadValue );

        pageText.bMarkedAsDeletedFlag =
            ReadFlag( m_fileCentral, TEXT_REPR_FILE_MARKED_AS_DELETED, bError, bBadValue );

        pageText.bPhysDeletedFlag =
            ReadFlag( m_fileCentral, TEXT_REPR_FILE_PHYS_DELETED, bError, bBadValue );

        pageText.nTotalRecCount =
            ReadInt32( m_fileCentral, TEXT_REPR_TOTAL_REC_COUNT, bError, bBadValue );

        pageText.nDeletedRecCount =
            ReadInt32( m_fileCentral, TEXT_REPR_DELETED_REC_COUNT, bError, bBadValue );

        char szBuf[STATIC_BUF_LEN];

        bool bDelimFound = false;
        bError = bError ||
            ( fgets_bin_win32( szBuf, STATIC_BUF_LEN, m_fileCentral, & bDelimFound ) == NULL );

        if( ! bError && bDelimFound )
        {
            KLSTD_USES_CONVERSION;

            wstring wstrAddOn = KLSTD_A2CW( szBuf );
            vectUsedFileNameAddOns.push_back( wstrAddOn );
            pageText.wstrFileName = GetPageFilePathFromAddOn( wstrAddOn );

            bValidRecord = true;
        }

        if( bError )
        {
            bStopRead = true;

            // if we encountered eof too soon, then set bBadValue to true
            bBadValue = bBadValue || ( feof( m_fileCentral ) != 0 );
        } else
        {
            if( bValidRecord )
            {
                if( pageText.bMarkedAsDeletedFlag )
                {
                    if( ! pageText.bPhysDeletedFlag && m_bChangeAllowed )
                    {
                        KLSTD_Unlink( pageText.wstrFileName.c_str(), false );
                        if( ! KLSTD_IfExists( pageText.wstrFileName.c_str() ) )
                        {
                            pageText.bPhysDeletedFlag = true;
                        }
						long nCurrPos = ReadPosition( m_fileCentral, bError );
                        RewritePageData( pageText );
						bError = bError || fseek( m_fileCentral, nCurrPos, SEEK_SET );
                    }
                } else
                {
                    m_vectPages.push_back( pageText );
                }
            }

            bStopRead = bStopRead || ! bValidRecord;
        }
    }

    if( bError )
    {
        wstring wstrErrorDescr;
        if( bBadValue || feof( m_fileCentral ) )
        {
            wstrErrorDescr = L"not all of necessary values can be read";
        } else
        {
		    KLSTD_USES_CONVERSION;
            wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
        }
        KLERR_MYTHROW2( ERR_CANT_READ2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str() );
    }
}

bool JournalTextImpl::CheckCentralFileSignature()
{
    bool bCreateNewData = false;

    if( fseek( m_fileCentral, 0, SEEK_SET ) )
    {
        KLSTD_USES_CONVERSION;
        wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
        KLERR_MYTHROW2( ERR_CANT_READ2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str() );
    }

    char szBuf[STATIC_BUF_LEN];
    if( fgets_bin_win32( szBuf, STATIC_BUF_LEN, m_fileCentral ) )
    {
        if( strcmp( szBuf, CENTRAL_FILE_SIGNATURE ) )
        {
            KLERR_MYTHROW1( ERR_BAD_JOURNAL_FORMAT1, m_wstrCentralFilePath.c_str() );
        }
    } else
    {
        if( ( ! feof( m_fileCentral ) && ferror( m_fileCentral ) ) || 
            m_bInfoDataMustExists )
        {

		    KLSTD_USES_CONVERSION;
            wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
            KLERR_MYTHROW2( ERR_CANT_READ2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str() );
        }

        bCreateNewData = true;
    }

    return ! bCreateNewData;
}

void JournalTextImpl::OpenCentralFile(
    CREATION_FLAGS   flagsCreation,
    ACCESS_FLAGS     flagsAccessFlags,
    long             nTimeout )
{
    KLSTD_USES_CONVERSION;

    m_bChangeAllowed = false;
    m_bInfoDataMustExists = false;
    m_bReadAllowed = ( flagsAccessFlags & AF_READ ) != 0;

    // Today I know only about these flags, so remove other bits so as not to disturb us:
    flagsCreation = CREATION_FLAGS( flagsCreation & ( CF_OPEN_EXISTING | CF_CREATE_NEW | CF_CLEAR ) );

    if( ! flagsCreation )
    {
        KLSTD_THROW_BADPARAM(flagsCreation);
    }

    if( ! flagsAccessFlags )
    {
        KLSTD_THROW_BADPARAM(flagsAccessFlags);
    }

    SHARE_FLAGS shareFlags;
    if( flagsAccessFlags == AF_READ )
    {
       shareFlags = SHARE_FLAGS( m_bChangeLimits ? 0 : (SF_READ|SF_WRITE) );
    } else if( flagsAccessFlags & AF_WRITE )
    {
        shareFlags = SF_READ;
        flagsAccessFlags = ACCESS_FLAGS( flagsAccessFlags | AF_READ );
    } else 
    {   // unknown combination of flagsAccessFlags
        KLSTD_ASSERT( 0 );
    }

    CAutoPtr<KLSTD::File> pFile;
    KLSTD_CreateFile(
        m_wstrCentralFilePath,
        shareFlags,
        flagsCreation,
        flagsAccessFlags,
        EXTRA_FLAGS( EF_RANDOM_ACCESS | EF_DONT_WORRY_ABOUT_CLOSE ),
        & pFile,
        nTimeout
    );

    int nFileId = pFile->GetFileID();

    if( flagsAccessFlags == AF_READ )
    {
#ifdef N_PLAT_NLM
        m_fileCentral = fdopen( nFileId, "rb" );
#else
        m_fileCentral = fdopen( nFileId, "r" );
#endif
        m_bInfoDataMustExists = true;
    } else if( flagsAccessFlags & AF_WRITE )
    {
        m_bChangeAllowed = true;
#ifdef N_PLAT_NLM
        m_fileCentral = fdopen( nFileId, "r+b" );
#else
        m_fileCentral = fdopen( nFileId, "r+" );
#endif

        if( ( flagsCreation == CF_OPEN_EXISTING ) || ( flagsCreation == CF_TRUNCATE_EXISTING ) )
        {
            m_bInfoDataMustExists = true;
            // -----------------------------------------------------------------
        }

        if( m_fileCentral )
        {
            if( setvbuf( m_fileCentral, NULL, _IONBF, 0 ) )
            {
		        KLSTD_USES_CONVERSION;
                wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
                KLERR_MYTHROW2( ERR_CANT_OPEN2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str() );
            }
        }
    }

    if( m_fileCentral )
    {
#ifdef WIN32
        if( setmode( nFileId, O_BINARY ) == -1 )
        {
		    KLSTD_USES_CONVERSION;
            wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
            KLERR_MYTHROW2( ERR_CANT_OPEN2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str() );
        }
#endif
    }
}

void JournalTextImpl::Repair( const std::wstring & wstrPath, const CreationInfo & creationInfo )
{
    ChangeLimits( wstrPath, creationInfo );
}

void JournalTextImpl::ChangeLimits(
    const std::wstring &    wstrPath,
    const CreationInfo &    creationInfo /* = CreationInfo() */ )
{
    AutoCriticalSection acs( m_pCS );

    m_bChangeLimits = true;

    OpenInternal(
        wstrPath,
        CF_OPEN_EXISTING,
        AF_READ,
        creationInfo,
        0 );

    try
    {
        CAutoPtr<JournalTextImpl> pNewJrnl;
        pNewJrnl.Attach( new JournalTextImpl( false ) );
        KLSTD_CHKMEM( pNewJrnl );

        wstring wstrNewJrnlFile = GenerateTempJournalName( wstrPath );
        pNewJrnl->Open( wstrNewJrnlFile, CF_CREATE_NEW, AF_WRITE, creationInfo );
        pNewJrnl->m_wstrPageFileTempl = m_wstrPageFileTempl;

        wstring wstrThisCentralFile = m_wstrCentralFilePath;
        wstring wstrNewCentralFile = pNewJrnl->m_wstrCentralFilePath;

        if( m_vectPages.size() )
        {
            int nPageIdx = 0;
            bool bError = false;
            OpenPage( nPageIdx );
            SeekPage( m_vectPages[nPageIdx], m_vectPages[nPageIdx].nStartPosition, SEEK_SET, bError );
            if( bError )
            {
		        KLSTD_USES_CONVERSION;
                wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
                KLERR_MYTHROW2( ERR_CANT_READ2, m_vectPages[nPageIdx].wstrFileName.c_str(), wstrErrorDescr.c_str() );
            }

            ReadRecord( nPageIdx, true );
            while( ! IsEOFInternal( nPageIdx ) )
            {
                pNewJrnl->AddInternal( m_nCurrentId, m_strDataCurrent );
                ReadRecord( nPageIdx, false );
            }

            for( unsigned int i = 0; i < m_vectPages.size(); i++ )
            {
                ClosePage( i );

                PageText & page = m_vectPages[i];
                KLSTD_Unlink( page.wstrFileName.c_str(), false );
            }
        }
        ReleaseAllOpenedStuffNoThrow();

        pNewJrnl->Close();

        KLSTD_Unlink( wstrThisCentralFile.c_str(), true );

        KLSTD_Rename( wstrNewCentralFile.c_str(), wstrThisCentralFile.c_str() );
    }
    catch(...)
    {
        ReleaseAllOpenedStuffNoThrow();
        throw;
    }
}

void JournalTextImpl::Close()
{
    AutoCriticalSection acs( m_pCS );

    CloseInternal();
}

void JournalTextImpl::CloseAndRemoveFiles()
{
    AutoCriticalSection acs( m_pCS );

    if( ! IsOpenedInternal() )
    {
        KLERR_MYTHROW0( ERR_CLOSED );
    }

    wstring wstrCentralFilePath = m_wstrCentralFilePath;
    vector<wstring> vectPageFileNames;
    unsigned int i;
    for( i = 0; i < m_vectPages.size(); i++ )
    {
        vectPageFileNames.push_back( m_vectPages[i].wstrFileName );
    }

    CloseInternal();

    KLSTD_Unlink( m_wstrCentralFilePath.c_str(), true );
    for( i = 0; i < vectPageFileNames.size(); i++ )
    {
        KLSTD_Unlink( vectPageFileNames[i].c_str(), true );
    }
}

void JournalTextImpl::CloseInternal()
{
    // let the exception fly away, if any, but close all opened files anyway:
    try
    {
        if( ! IsOpenedInternal() )
        {
            KLERR_MYTHROW0( ERR_CLOSED );
        }

        for( unsigned int i = 0; i < m_vectPages.size(); i++ )
        {
            ClosePage( i );  // may throw an exception
        }

        ReleaseAllOpenedStuffNoThrow();
    }
    catch(...)
    {
        ReleaseAllOpenedStuffNoThrow();
        throw;
    }
}

void JournalTextImpl::ReleaseAllOpenedStuffNoThrow()
{
    for( unsigned int i = 0; i < m_vectPages.size(); i++ )
    {
        if( m_vectPages[i].file )
        {
            KLSTD_FClose( m_vectPages[i].file );
            m_vectPages[i].file = NULL;
        }
    }
    m_vectPages.clear();

    KLSTD_FClose( m_fileCentral );
    vectUsedFileNameAddOns.clear();

    m_fileCentral = NULL;
}

bool JournalTextImpl::IsOpened()
{ 
    AutoCriticalSection acs( m_pCS );

    return IsOpenedInternal();
};

AVP_longlong JournalTextImpl::GetJournalSize()
{
    if( ! IsOpenedInternal() )
    {
        KLERR_MYTHROW0( ERR_CLOSED );
    }

    AutoCriticalSection acs( m_pCS );

    m_nCurrentPage = -1;

    AVP_longlong llResult = 0;
    for( unsigned int nPageIdx = 0; nPageIdx < m_vectPages.size(); nPageIdx++ )
    {
        OpenPage( nPageIdx );

        PageText & page = m_vectPages[nPageIdx];

        bool bError = false;
        long nCurrPos = ReadPosition( page.file, bError );
        PageToEOF( page, bError );
        llResult += ReadPosition( page.file, bError );
        SeekPage( page, nCurrPos, SEEK_SET, bError );

        if( bError )
        {
		    KLSTD_USES_CONVERSION;
            wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
            KLERR_MYTHROW2( ERR_CANT_READ2, page.wstrFileName.c_str(), wstrErrorDescr.c_str() );
        }
    }

    bool bError = false;

    long nCurrPos = ReadPosition( m_fileCentral, bError );
    bError = bError || fseek( m_fileCentral, 0, SEEK_END );            
    llResult += ReadPosition( m_fileCentral, bError );
    bError = bError || fseek( m_fileCentral, nCurrPos, SEEK_SET );

    if( bError )
    {
		KLSTD_USES_CONVERSION;
        wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
        KLERR_MYTHROW2( ERR_CANT_READ2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str() );
    }

    return llResult;
}

bool JournalTextImpl::IsOpenedInternal()
{ 
    return m_fileCentral != NULL; 
}

long JournalTextImpl::Add( const Params* pParams )
{
    KLSTD_ASSERT( pParams );

    AutoCriticalSection acs( m_pCS );

    CAutoPtr<KLSTD::MemoryChunk> pChunk;
    KLPAR_SerializeToMemory( pParams, & pChunk );

    string strData( (const char *)pChunk->GetDataPtr(),  pChunk->GetDataSize() );

    long nId;
    AddInternal( nId, strData );

    return nId;
}

long JournalTextImpl::Add( JournalRecord* pRecord )
{
    KLSTD_ASSERT( pRecord );

    AutoCriticalSection acs( m_pCS );

    string strData;
    pRecord->Serialize( strData );

    long nId;
    AddInternal( nId, strData );

    return nId;
}

long JournalTextImpl::Add( const std::string & str )
{
    AutoCriticalSection acs( m_pCS );

    string strData( str );

    long nId;
    AddInternal( nId, strData );

    return nId;
}

void JournalTextImpl::Update( const KLPAR::Params* pParams )
{
    KLSTD_ASSERT( pParams );

    AutoCriticalSection acs( m_pCS );

    CAutoPtr<KLSTD::MemoryChunk> pChunk;
    KLPAR_SerializeToMemory( pParams, & pChunk );

    string strData( (const char *)pChunk->GetDataPtr(),  pChunk->GetDataSize() );

    UpdateInternal( strData );
}

void JournalTextImpl::Update( JournalRecord* pRecord )
{
    KLSTD_ASSERT( pRecord );

    AutoCriticalSection acs( m_pCS );

    string strData;
    pRecord->Serialize( strData );

    UpdateInternal( strData );
}

void JournalTextImpl::Update( const std::string & str )
{
    AutoCriticalSection acs( m_pCS );

    string strData( str );

    UpdateInternal( strData );
}

void JournalTextImpl::UpdateInternal( string& strData )
{
    if( ! IsOpenedInternal() )
    {
        KLERR_MYTHROW0( ERR_CLOSED );
    }

    if( ! m_bChangeAllowed )
    {
        KLERR_MYTHROW0( ERR_CHANGE_NOT_ALLOWED );
    }

    if( ! m_bReadAllowed )
    {
        KLERR_MYTHROW0( ERR_READ_NOT_ALLOWED );
    }

    if( ( m_nCurrentPage < 0 ) || ( IsEOFInternal( m_nCurrentPage ) ) )
    {
        KLERR_MYTHROW0( ERR_NO_CURRENT_POS );
    }

    if( m_strDataCurrent.size() != strData.size() )
    {
        KLERR_MYTHROW0( ERR_UPDATED_DATA_HAS_OTHER_SIZE );
    }

    if( IsContainsSpacesOnly( strData ) )
    {
        KLERR_MYTHROW0( ERR_EMPTY_DATA );
    }

    PageText & page = m_vectPages[m_nCurrentPage];
    bool bError = false;
    long nCurrPosition = ReadPosition( page.file, bError );
    SeekPage( page, m_nCurrentStartPos, SEEK_SET, bError );
    if( ! bError )
    {
        bError = bError || ( fwrite( strData.c_str(), 1, strData.size(), page.file ) != strData.size() );
        SeekPage( page, nCurrPosition, SEEK_SET, bError );
    }

    if( bError )
    {
		KLSTD_USES_CONVERSION;
        wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
        KLERR_MYTHROW2( ERR_CANT_WRITE2, page.wstrFileName.c_str(), wstrErrorDescr.c_str() );            
    }
}

void JournalTextImpl::AddInternal( long & nId, string& strData )
{
    if( ! IsOpenedInternal() )
    {
        KLERR_MYTHROW0( ERR_CLOSED );
    }

    if( ! m_bChangeAllowed )
    {
        KLERR_MYTHROW0( ERR_CHANGE_NOT_ALLOWED );
    }

    if( IsContainsSpacesOnly( strData ) )
    {
        KLERR_MYTHROW0( ERR_EMPTY_DATA );
    }
    
    m_nCurrentPage = -1;

    PrepareLastPage();

    nId = ++m_nIdentity;
    RewriteIdentity();

    bool bError = false;

    /* SetChangingState( true );
    SetPageChangingState( LastPageIdx(), true ); */
    
    PageText& page = GetLastPage();

    PageToEOF( page, bError );
    bError = bError || ( fwrite( strData.c_str(), 1, strData.size(), page.file ) != strData.size() );

#ifdef UNICODE_JOURNAL
    bError = bError || ( fputws( JRNL_DELIM_UNICODE, page.file ) == EOF );
#else
    bError = bError || ( fputs( JRNL_DELIM, page.file ) == EOF );
#endif

    if( ! bError )
    {
        page.nTotalRecCount++;
        if( m_creationInfo.pageLimitType == pltUnlimited )
        {
            page.bPageIsFull = false;
        } else if( m_creationInfo.pageLimitType == pltRecCount )
        {
            page.bPageIsFull = ( page.nTotalRecCount >= m_creationInfo.nMaxRecordsInPage );
        } else
        {
            long nPageSize = ReadPosition( page.file, bError );
            page.bPageIsFull = ( nPageSize >= m_creationInfo.nMaxPageSize );
        }
        RewritePageData( page );
    }

    if( bError )
    {
		KLSTD_USES_CONVERSION;
        wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
        KLERR_MYTHROW2( ERR_CANT_WRITE2, page.wstrFileName.c_str(), wstrErrorDescr.c_str() );            
    }
}

void JournalTextImpl::Flush()
{
    if( ! IsOpenedInternal() )
    {
        KLERR_MYTHROW0( ERR_CLOSED );
    }

    if( ! m_bChangeAllowed )
    {
        KLERR_MYTHROW0( ERR_CHANGE_NOT_ALLOWED );
    }

    bool bError = false;
    wstring wstrErrFilePath;
    for( unsigned int i = 0; !bError && ( i < m_vectPages.size() ); i++ )
    {
        if( m_vectPages[i].file )
        {
            wstrErrFilePath = m_vectPages[i].wstrFileName;
            FlushJrnlFile( m_vectPages[i].file, bError );
        }
    }

    if( bError )
    {
		KLSTD_USES_CONVERSION;
        wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
        KLERR_MYTHROW2( ERR_CANT_WRITE2, wstrErrFilePath.c_str(), wstrErrorDescr.c_str() );
    }
}

void JournalTextImpl::PrepareLastPage()
{
    bool bCreateNewPage = false;

    if( ! IsLastPageOpened() )
    {
        if( m_vectPages.size() == 0 )
        {
            bCreateNewPage = true;
        } else
        {
            OpenPage( LastPageIdx() );
        }
    }

    if( ! bCreateNewPage )
    {
        PageText& page = GetLastPage();
        bCreateNewPage = page.bPageIsFull;

        if( bCreateNewPage )
        {
            ClosePage( LastPageIdx() );
        }
    }

    if( bCreateNewPage )
    {
        CreateAndOpenNewPage();
        if( m_creationInfo.bLimitPages )
        {
            while( m_vectPages.size() > 2 )
            {
                RemovePage( 0 );
            }
        }
    }
}

void JournalTextImpl::ClosePage( int nPageIdx )
{
    KLSTD_ASSERT( ( nPageIdx >= 0 ) && ( (unsigned)nPageIdx < m_vectPages.size() ) );
    
    PageText& page = m_vectPages[ nPageIdx ];

    if( page.file )
    {
        bool bError = false;

        bError = bError || KLSTD_FClose( page.file );

        if( bError )
        {
		    KLSTD_USES_CONVERSION;
            wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
            KLERR_MYTHROW2( ERR_CANT_CLOSE2, page.wstrFileName.c_str(), wstrErrorDescr.c_str() );
        }

        page.file = NULL;
    }
}

void JournalTextImpl::RemovePage( int nPageIdx )
{
    KLSTD_ASSERT( ( nPageIdx >= 0 ) && ( (unsigned)nPageIdx < m_vectPages.size() ) );

    ClosePage( nPageIdx );
    
    PageText& page = m_vectPages[ nPageIdx ];

    page.bMarkedAsDeletedFlag = true;

    KLSTD_Unlink( page.wstrFileName.c_str(), false );
    if( ! KLSTD_IfExists( page.wstrFileName.c_str() ) )
    {
        page.bPhysDeletedFlag = true;
    }

    RewritePageData( page );

    m_vectPages.erase( m_vectPages.begin() + nPageIdx );
}

void JournalTextImpl::RewritePageData( PageText& page )
{
    bool bError = false;

    bError = bError || fseek( m_fileCentral, page.nPageRecStartPos, SEEK_SET );

    WriteInt32( page.nStartId, m_fileCentral, TEXT_REPR_START_ID, bError );
    WriteInt32( page.nStartPosition, m_fileCentral, TEXT_REPR_START_POSITION, bError );
    WriteFlag( page.bMarkedAsDeletedFlag, m_fileCentral, TEXT_REPR_FILE_MARKED_AS_DELETED, bError );
    WriteFlag( page.bPhysDeletedFlag, m_fileCentral, TEXT_REPR_FILE_PHYS_DELETED, bError );
    WriteInt32( page.nTotalRecCount, m_fileCentral, TEXT_REPR_TOTAL_REC_COUNT, bError );
    WriteInt32( page.nDeletedRecCount, m_fileCentral, TEXT_REPR_DELETED_REC_COUNT, bError );

    if( bError )
    {
        KLSTD_USES_CONVERSION;
        wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
        KLERR_MYTHROW2( ERR_CANT_WRITE2, m_wstrCentralFilePath.c_str(), wstrErrorDescr.c_str() );
    }
}

void JournalTextImpl::OpenPage( int nPageIdx )
{
    KLSTD_ASSERT( ( nPageIdx >= 0 ) && ( (unsigned)nPageIdx < m_vectPages.size() ) );

    PageText & page = m_vectPages[ nPageIdx ];

    if( ! page.file )
    {
        wstring wstrFileName = page.wstrFileName;
        FILE* fileTemp = NULL;
        try
        {
            string strOpenMode;
            SHARE_FLAGS shareFlags;
            ACCESS_FLAGS accessFlags;
            if( m_bChangeAllowed )
            {
#ifdef N_PLAT_NLM
                strOpenMode = "r+b";
#else
                strOpenMode = "r+";
#endif
                shareFlags = SF_READ;
                accessFlags = ACCESS_FLAGS( AF_READ | AF_WRITE );
            } else
            {
#ifdef N_PLAT_NLM
                strOpenMode = "rb";
#else
                strOpenMode = "r";
#endif
                shareFlags = SHARE_FLAGS( SF_READ | SF_WRITE );
                accessFlags = AF_READ;
            }

            CAutoPtr<KLSTD::File> pFile;
            KLSTD_CreateFile(
                wstrFileName,
                shareFlags,
                CF_OPEN_EXISTING,
                accessFlags,
                EXTRA_FLAGS( EF_RANDOM_ACCESS | EF_DONT_WORRY_ABOUT_CLOSE ),
                & pFile
            );
            fileTemp = fdopen( pFile->GetFileID(), strOpenMode.c_str() );
            if( ! fileTemp 
#ifdef WIN32
                || ( setmode( pFile->GetFileID(), O_BINARY ) == -1 )
#endif
              )
            {
		        KLSTD_USES_CONVERSION;
                wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
                KLERR_MYTHROW2( ERR_CANT_OPEN2, wstrFileName.c_str(), wstrErrorDescr.c_str() );
            }

            page.file = fileTemp;
            fileTemp = NULL; // file handle now is stored in m_vectPages, so set fileTemp
                             // to NULL. In case of later exception in this try-catch block the 
                             // file will not be closed.

            if( m_creationInfo.pageLimitType == pltUnlimited )
            {
                page.bPageIsFull = false;
            } else if( m_creationInfo.pageLimitType == pltRecCount )
            {
                page.bPageIsFull = ( page.nTotalRecCount >= m_creationInfo.nMaxRecordsInPage );
            } else
            {
                bool bError = false;
                long nCurrPos = ReadPosition( page.file, bError );
                PageToEOF( page, bError );
                long nPageSize = ReadPosition( page.file, bError );
                SeekPage( page, nCurrPos, SEEK_SET, bError );
                if( ! bError )
                {
                    page.bPageIsFull = ( nPageSize >= m_creationInfo.nMaxPageSize );
                }

                if( bError )
                {
		            KLSTD_USES_CONVERSION;
                    wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
                    KLERR_MYTHROW2( ERR_CANT_WRITE2, page.wstrFileName.c_str(), wstrErrorDescr.c_str() );
                }
            }
        }
        catch(...)
        {
            if( fileTemp )
            {
                KLSTD_FClose( fileTemp );
            }
            throw;
        }
    }
}

void JournalTextImpl::CreateAndOpenNewPage()
{
    wstring wstrFileNameAddOn = GenerateNewPageFileNameAddOn();
    PageText page;
    page.wstrFileName = GetPageFilePathFromAddOn( wstrFileNameAddOn );

    CAutoPtr<KLSTD::File> pFile;
    KLSTD_CreateFile(
        page.wstrFileName,
        SF_READ,
        CF_CREATE_ALWAYS,
        ACCESS_FLAGS( AF_READ | AF_WRITE ),
        EXTRA_FLAGS( EF_RANDOM_ACCESS | EF_DONT_WORRY_ABOUT_CLOSE ),
        & pFile
    );
    FILE* file = fdopen( pFile->GetFileID(), "r+" );
    try
    {
        if( ! file
#ifdef WIN32
            || ( setmode( pFile->GetFileID(), O_BINARY ) == -1 )
#endif
          )
        {
		    KLSTD_USES_CONVERSION;
            wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
            KLERR_MYTHROW2( ERR_CANT_OPEN2, page.wstrFileName.c_str(), wstrErrorDescr.c_str() );
        }

#ifdef UNICODE_JOURNAL
        if( fwrite( sUnicodeFlag, 1, sizeof(sUnicodeFlag), file ) != sizeof(sUnicodeFlag) )
        {
		    KLSTD_USES_CONVERSION;
            wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
            KLERR_MYTHROW2( ERR_CANT_WRITE2, page.wstrFileName.c_str(), wstrErrorDescr.c_str() );
        }
#endif

        {
            KLSTD_USES_CONVERSION;

            bool bCentralError = false;

            bCentralError = bCentralError || fseek( m_fileCentral, 0, SEEK_END );

            if( ! bCentralError )
            {
                page.nPageRecStartPos = ReadPosition( m_fileCentral, bCentralError );
                page.nStartId = m_nIdentity;

#ifdef UNICODE_JOURNAL
                page.nStartPosition = sizeof( sUnicodeFlag );
#else
                page.nStartPosition = 0;
#endif
                page.bMarkedAsDeletedFlag = false;
                page.bPhysDeletedFlag = false;
                page.nTotalRecCount = 0;
                page.nDeletedRecCount = 0;

                if( ! bCentralError )
                {
                    RewritePageData( page );

                    KLSTD_USES_CONVERSION;
                    bCentralError = bCentralError || ( fputs( KLSTD_W2CA( wstrFileNameAddOn.c_str() ), m_fileCentral ) == EOF );
                    bCentralError = bCentralError || ( fputs( JRNL_DELIM, m_fileCentral ) == EOF );

                    if( ! bCentralError )
                    {
                        page.file = file;
                        m_vectPages.push_back( page );
                    }
                }
            }
        }
    }
    catch(...)
    {
        if( file )
        {
            KLSTD_FClose( file );
        }
        throw;
    }
}

void JournalTextImpl::ResetIterator()
{
    AutoCriticalSection acs( m_pCS );

    if( ! IsOpenedInternal() )
    {
        KLERR_MYTHROW0( ERR_CLOSED );
    }

    if( ! m_bReadAllowed )
    {
        KLERR_MYTHROW0( ERR_READ_NOT_ALLOWED );
    }

    int nPageIdx = 0;
    if( m_vectPages.size() )
    {
        OpenPage( nPageIdx );
    
        bool bError = false;

        PageText& page = m_vectPages[nPageIdx];

        SeekPage( page, page.nStartPosition, SEEK_SET, bError );

        if( bError )
        {
		    KLSTD_USES_CONVERSION;
            wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
            KLERR_MYTHROW2( ERR_CANT_READ2, page.wstrFileName.c_str(), wstrErrorDescr.c_str() );
        }

        m_nCurrentId = page.nStartId;
        ReadRecord( nPageIdx, true ); // may change nPageIdx;
    }
    m_nCurrentPage = nPageIdx;
}

bool JournalTextImpl::SetIterator( long nId )
{
    if( ! IsOpenedInternal() )
    {
        KLERR_MYTHROW0( ERR_CLOSED );
    }

	bool bResult = false;

    AutoCriticalSection acs( m_pCS );

    m_nCurrentPage = FindBorderPage( nId, true );
    if( m_nCurrentPage < 0 )
    {
        m_nCurrentPage = m_vectPages.size(); // go to EOF
    } else
    {
        OpenPage( m_nCurrentPage );

        bool bError = false;

        PageText& page = m_vectPages[m_nCurrentPage];
        SeekPage( page, page.nStartPosition, SEEK_SET, bError );
        m_nCurrentId = page.nStartId;

        if( ! bError )
        {
            while( ! IsEOFInternal(m_nCurrentPage) && ( m_nCurrentId < nId ) )
            {
                ReadRecord( m_nCurrentPage, false );
            }
			bResult = ! IsEOFInternal(m_nCurrentPage) && ( m_nCurrentId == nId );
        }
        
        if( bError )
        {
		    KLSTD_USES_CONVERSION;
            wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
            KLERR_MYTHROW2( ERR_CANT_READ2, m_vectPages[m_nCurrentPage].wstrFileName.c_str(), wstrErrorDescr.c_str() );
        }
    }

	return bResult;
}

void JournalTextImpl::MoveToRecordN( long nRecIdx )
{
    if( ! IsOpenedInternal() )
    {
        KLERR_MYTHROW0( ERR_CLOSED );
    }

    if( nRecIdx <= 0 )
    {
        ResetIterator();
        return;
    }

    AutoCriticalSection acs( m_pCS );

    int nTargetPage = -1;
    long nTotalCount = 0;
    for( unsigned int nPageIdx = 0; nPageIdx < m_vectPages.size(); nPageIdx++ )
    {
        OpenPage( nPageIdx );
        PageText & page = m_vectPages[nPageIdx];
        long nPageCount = page.nTotalRecCount - page.nDeletedRecCount;

        if( ( nTotalCount + nPageCount ) > nRecIdx )
        {
            nTargetPage = nPageIdx;
            break;
        }
        nTotalCount += nPageCount;
    }

    if( nTargetPage < 0 )
    {
        m_nCurrentPage = m_vectPages.size();
    } else
    {
        bool bError = false;

        m_nCurrentPage = nTargetPage;
        PageText & pageCurr = m_vectPages[m_nCurrentPage];

        SeekPage( pageCurr, pageCurr.nStartPosition, SEEK_SET, bError );
        m_nCurrentId = pageCurr.nStartId;
        while( ! IsEOFInternal(m_nCurrentPage) && (nTotalCount <= nRecIdx ) )
        {
            ReadRecord( m_nCurrentPage, false );
            nTotalCount++;
        }
    }
}

long JournalTextImpl::GetRecordCount()
{
    if( ! IsOpenedInternal() )
    {
        KLERR_MYTHROW0( ERR_CLOSED );
    }

    AutoCriticalSection acs( m_pCS );

    long nTotalCount = 0;

    for( unsigned int nPageIdx = 0; nPageIdx < m_vectPages.size(); nPageIdx++ )
    {
        OpenPage( nPageIdx );
        PageText & page = m_vectPages[nPageIdx];
        nTotalCount += page.nTotalRecCount - page.nDeletedRecCount;
    }

    return nTotalCount;
}

int JournalTextImpl::FindBorderPage( long nID, bool nIdMayBeFirst )
{
    int nBorderPageIdx = -1;

    unsigned int nPageIdx;
    for( nPageIdx = 0; nPageIdx < m_vectPages.size(); nPageIdx++ )
    {
        if( nIdMayBeFirst )
        {
            if( m_vectPages[nPageIdx].nStartId > nID )
            {
                if( nBorderPageIdx < 0 )
                {
                    nBorderPageIdx = 0;
                }
                break;
            }
        } else
        {
            if( m_vectPages[nPageIdx].nStartId >= nID )
            {
                break;
            }
        }

        nBorderPageIdx = nPageIdx;
    }
    
    return nBorderPageIdx;
}

void JournalTextImpl::ReadRecord( int & nPageIdx, bool bReset )
{
    KLSTD_ASSERT( ( nPageIdx >= 0 ) && ( (unsigned )nPageIdx < m_vectPages.size() ) );

    bool bError = false;
    bool bBadValue = false;

    bool bStop = false;

    char szBuf[STATIC_BUF_LEN * sizeof(wchar_t)];

    while( ! bStop )
    {
        PageText& page = m_vectPages[nPageIdx];
        m_nCurrentStartPos = ReadPosition( page.file, bError );

        bool bDelimFound = false;
        m_strDataCurrent = "";

        while( ! feof( page.file ) && ! ferror( page.file ) && ! bDelimFound )
        {
#ifdef UNICODE_JOURNAL
            char* pszResult = (char *)fgetws( (wchar_t*)szBuf, STATIC_BUF_LEN, page.file );
#else
            char* pszResult = fgets( szBuf, STATIC_BUF_LEN * sizeof(wchar_t), page.file );
#endif
            bDelimFound = false;
            int nReadSize = 0;

            if( pszResult )
            {
#ifdef UNICODE_JOURNAL
                wchar_t* pEndl = wcsstr( (wchar_t*)szBuf, JRNL_DELIM_UNICODE );
#else
                char* pEndl = strstr( szBuf, JRNL_DELIM );
#endif

                if( ! pEndl )
                {
#ifdef UNICODE_JOURNAL
                    nReadSize = wcslen( (wchar_t*)szBuf ) * sizeof(wchar_t);
#else
                    nReadSize = strlen( szBuf );
#endif
                } else
                {
                    nReadSize = ( ((char*)pEndl) - pszResult );
                    bDelimFound = true;
                }

                m_strDataCurrent += string( szBuf, nReadSize );
            }
        }

        if( bDelimFound )
        {
            m_nCurrentId++;
        }

        if( ferror( page.file ) )
        {
		    KLSTD_USES_CONVERSION;
            wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
            KLERR_MYTHROW2( ERR_CANT_READ2, page.wstrFileName.c_str(), wstrErrorDescr.c_str() );
        }

        bStop = ! IsContainsSpacesOnly( m_strDataCurrent );

        bool bError = false;

        if( feof( page.file ) )
        {
            if( bReset )
            {
                page.nStartPosition = ReadPosition( page.file, bError );
                page.nStartId = m_nCurrentId - 1;
                if( m_bChangeAllowed )
                {
                    RewritePageData( page );
                }
            }

            if( ! bError )
            {
                if( nPageIdx == LastPageIdx() )
                {
                    bStop = true;
                } else
                {
                    nPageIdx++;
                    PageText& pageNew = m_vectPages[nPageIdx];
                    OpenPage( nPageIdx );
                    SeekPage( pageNew, pageNew.nStartPosition, SEEK_SET, bError );
                }
            }
        }

        if( bError )
        {
		    KLSTD_USES_CONVERSION;
            wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
            KLERR_MYTHROW2( ERR_CANT_READ2, m_vectPages[nPageIdx].wstrFileName.c_str(), wstrErrorDescr.c_str() );
        }
    }

    if( bReset )
    {
        if( feof( m_vectPages[nPageIdx].file ) )
        {
            m_vectPages[nPageIdx].nStartPosition = ReadPosition( m_vectPages[nPageIdx].file, bError );
        } else
        {
            m_vectPages[nPageIdx].nStartPosition = m_nCurrentStartPos;
        }
        m_vectPages[nPageIdx].nStartId = m_nCurrentId - 1;

        if( m_bChangeAllowed )
        {
            RewritePageData( m_vectPages[nPageIdx] );
        }
    }
}

bool JournalTextImpl::IsContainsSpacesOnly( const string & strData )
{
    bool bResult = true;

    const JRNL_CHAR* pStartChar = (const JRNL_CHAR*)strData.c_str();

    int nSize = strData.size() / sizeof( JRNL_CHAR );
    for( int i = 0; i < nSize ; i++ )
    {
#ifdef UNICODE_JOURNAL
        if( ! iswspace( *(pStartChar+i) ) )
#else
        if( ! isspace( *(pStartChar+i) ) )
#endif
        {
            bResult = false;
            break;
        }
    }

    return bResult;
}

void JournalTextImpl::Next()
{
    AutoCriticalSection acs( m_pCS );

    if( ! IsEOFInternal( m_nCurrentPage ) )
    {
        ReadRecord( m_nCurrentPage, false );
    }
}

bool JournalTextImpl::IsEOF()
{
    AutoCriticalSection acs( m_pCS );

    return IsEOFInternal( m_nCurrentPage );
}

bool JournalTextImpl::IsEOFInternal( int nPageIdx )
{
    if( ! IsOpenedInternal() )
    {
        KLERR_MYTHROW0( ERR_CLOSED );
    }

    if( ! m_bReadAllowed )
    {
        KLERR_MYTHROW0( ERR_READ_NOT_ALLOWED );
    }

    if( nPageIdx < 0 )
    {
        KLERR_MYTHROW0( ERR_NO_CURRENT_POS );
    }

    if( (unsigned )nPageIdx >= m_vectPages.size() )
    {
        return true;
    }

    return ( nPageIdx == LastPageIdx() ) && ( feof( GetLastPage().file ) );
}

void JournalTextImpl::GetCurrent( long & nId, JournalRecord* pRecord )
{
    AutoCriticalSection acs( m_pCS );

    GetCurrentCommon();

    pRecord->Deserialize( m_strDataCurrent );

    nId = m_nCurrentId;
}

void JournalTextImpl::GetCurrent( long & nId, Params** ppParams )
{
    AutoCriticalSection acs( m_pCS );

    GetCurrentCommon();

    KLPAR_DeserializeFromMemory( m_strDataCurrent.c_str(), m_strDataCurrent.size(), ppParams );
    nId = m_nCurrentId;
}

void JournalTextImpl::GetCurrent( long & nId, std::string & strData )
{
    AutoCriticalSection acs( m_pCS );

    GetCurrentCommon();

    strData = m_strDataCurrent;
    nId = m_nCurrentId;
}

void JournalTextImpl::GetCurrentCommon()
{
    if( ! IsOpenedInternal() )
    {
        KLERR_MYTHROW0( ERR_CLOSED );
    }

    if( ! m_bReadAllowed )
    {
        KLERR_MYTHROW0( ERR_READ_NOT_ALLOWED );
    }

    if( ( m_nCurrentPage < 0 ) || ( IsEOFInternal( m_nCurrentPage ) ) )
    {
        KLERR_MYTHROW0( ERR_NO_CURRENT_POS );
    }
}

void JournalTextImpl::DeleteOldRecords( long nRecordsToLeave )
{
    AutoCriticalSection acs( m_pCS );

    if( ! IsOpenedInternal() )
    {
        KLERR_MYTHROW0( ERR_CLOSED );
    }

    if( ! m_bChangeAllowed )
    {
        KLERR_MYTHROW0( ERR_CHANGE_NOT_ALLOWED );
    }

    m_nCurrentPage = -1;

    if( m_vectPages.size() == 0 )
    {
        return;
    }

    // nBorderPageIdx - lowest page index from range of pages with records to leave
    int nBorderPageIdx;
    int nRecCountToKillInBorderPage = 0;
    int i;

    int nRecCount = 0;
    for( i = m_vectPages.size()-1; i >= 0; i-- )
    {
        nBorderPageIdx = i;
        OpenPage( i );
        nRecCount += m_vectPages[i].nTotalRecCount - m_vectPages[i].nDeletedRecCount;

        if( nRecCount >= nRecordsToLeave )
        {
            nRecCountToKillInBorderPage = nRecCount - nRecordsToLeave;
            break;
        }
    }

    size_t nFinalSize = m_vectPages.size() - nBorderPageIdx;
    while( m_vectPages.size() > nFinalSize )
    {
        RemovePage( 0 );
    }

    int nPageIdx = 0;
    bool bError = false;
    OpenPage( nPageIdx );
    SeekPage( m_vectPages[nPageIdx], m_vectPages[nPageIdx].nStartPosition, SEEK_SET, bError );
    if( bError )
    {
		KLSTD_USES_CONVERSION;
        wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
        KLERR_MYTHROW2( ERR_CANT_READ2, m_vectPages[nPageIdx].wstrFileName.c_str(), wstrErrorDescr.c_str() );
    }

    m_nCurrentId = m_vectPages[nPageIdx].nStartId;
    ReadRecord( nPageIdx, true );
    for( i = 0; i < nRecCountToKillInBorderPage; i++ )
    {
        if( IsEOFInternal( nPageIdx ) )
        {
            break;
        }
        DeleteCurrentInternal( nPageIdx );
    }
}

void JournalTextImpl::DeleteCurrent()
{
    AutoCriticalSection acs( m_pCS );

    if( ! IsOpenedInternal() )
    {
        KLERR_MYTHROW0( ERR_CLOSED );
    }

    if( ! m_bChangeAllowed )
    {
        KLERR_MYTHROW0( ERR_CHANGE_NOT_ALLOWED );
    }

    if( ( m_nCurrentPage < 0 ) || ( IsEOFInternal( m_nCurrentPage ) ) )
    {
        KLERR_MYTHROW0( ERR_NO_CURRENT_POS );
    }

    DeleteCurrentInternal( m_nCurrentPage );
}

void JournalTextImpl::DeleteCurrentInternal( int & nPageIdx )
{
    bool bError = false;

    PageText& page = m_vectPages[nPageIdx];

    long nPageCurrPosition = ReadPosition( page.file, bError );

    if( ! bError )
    {
        SeekPage( page, m_nCurrentStartPos, SEEK_SET, bError );

        for( unsigned int i = 0; i < ( m_strDataCurrent.size() / sizeof(JRNL_CHAR) ); i++ )
        {
#ifdef UNICODE_JOURNAL
            bError = bError || ( fputwc( L' ', page.file ) == WEOF );
#else
            bError = bError || ( fputc( ' ', page.file ) == EOF );
#endif
        }

        SeekPage( page, nPageCurrPosition, SEEK_SET, bError );
    }

    if( ! bError && ( page.nStartPosition == m_nCurrentStartPos ) )
    {
        page.nStartPosition = ReadPosition( page.file, bError );
        page.nStartId = m_nCurrentId;
    }

    page.nDeletedRecCount++;
    if( ! bError )
    {
        RewritePageData( page );
    }

    if( bError )
    {
		KLSTD_USES_CONVERSION;
        wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
        KLERR_MYTHROW2( ERR_CANT_WRITE2, page.wstrFileName.c_str(), wstrErrorDescr.c_str() );
    }

    if( page.bPageIsFull && ( page.nDeletedRecCount == page.nTotalRecCount ) )
    {
        RemovePage( nPageIdx );

        if( (unsigned)nPageIdx < m_vectPages.size() )
        {
            OpenPage( nPageIdx );
            SeekPage( m_vectPages[nPageIdx], m_vectPages[nPageIdx].nStartPosition, SEEK_SET, bError );
        }
    }

    if( ! IsEOFInternal( nPageIdx ) )
    {
        ReadRecord( nPageIdx, false );
    }
}

void JournalTextImpl::DeleteOlderThanID( long nID )
{
    nID++;

    AutoCriticalSection acs( m_pCS );

    if( ! IsOpenedInternal() )
    {
        KLERR_MYTHROW0( ERR_CLOSED );
    }

    if( ! m_bChangeAllowed )
    {
        KLERR_MYTHROW0( ERR_CHANGE_NOT_ALLOWED );
    }

    m_nCurrentPage = -1;

    if( nID > m_nIdentity )
    {
        DeleteAllInternal();
        return;
    }

    int nBorderPageIdx = FindBorderPage( nID, false );

    // nBorderPageIdx - greatest page index from range of pages with records to remove
    if( nBorderPageIdx >= 0 )
    {
        size_t nFinalSize = m_vectPages.size() - nBorderPageIdx;
        while( m_vectPages.size() > nFinalSize )
        {
            RemovePage( 0 );
        }

        int nPageIdx = 0;
        bool bError = false;
        OpenPage( nPageIdx );
        SeekPage( m_vectPages[nPageIdx], m_vectPages[nPageIdx].nStartPosition, SEEK_SET, bError );
        if( bError )
        {
		    KLSTD_USES_CONVERSION;
            wstring wstrErrorDescr = KLSTD_A2CW( strerror( errno ) );
            KLERR_MYTHROW2( ERR_CANT_READ2, m_vectPages[nPageIdx].wstrFileName.c_str(), wstrErrorDescr.c_str() );
        }

        m_nCurrentId = m_vectPages[nPageIdx].nStartId;
        ReadRecord( nPageIdx, true );
        while( ! IsEOFInternal( nPageIdx ) )
        {
            if( m_nCurrentId < nID )
            {
                DeleteCurrentInternal( nPageIdx );
            } else
            {
                break;
            }
        }
    }
}

void JournalTextImpl::DeleteRanges( const std::vector< std::pair<long, long> > & vectRanges )
{
    AutoCriticalSection acs( m_pCS );

    if( ! IsOpenedInternal() )
    {
        KLERR_MYTHROW0( ERR_CLOSED );
    }

    if( ! m_bChangeAllowed )
    {
        KLERR_MYTHROW0( ERR_CHANGE_NOT_ALLOWED );
    }

    for( unsigned int nRange = 0; nRange < vectRanges.size(); nRange++ )
    {
        long nStart = vectRanges[nRange].first;
        long nFinish = vectRanges[nRange].second;
        if( nStart > nFinish )
        {
            swap( nStart, nFinish );
        }

        SetIterator( nStart );
        while( ! IsEOFInternal( m_nCurrentPage ) )
        {
            if( m_nCurrentId > nFinish )
            {
                break;
            }
            DeleteCurrentInternal( m_nCurrentPage );
        }
    }
}

void JournalTextImpl::DeleteAll()
{
    AutoCriticalSection acs( m_pCS );

    DeleteAllInternal();

}

void JournalTextImpl::DeleteAllInternal()
{
    if( ! IsOpenedInternal() )
    {
        KLERR_MYTHROW0( ERR_CLOSED );
    }

    if( ! m_bChangeAllowed )
    {
        KLERR_MYTHROW0( ERR_CHANGE_NOT_ALLOWED );
    }

    m_nCurrentPage = -1;

    while( m_vectPages.size() )
    {
        RemovePage( 0 );
    }
}

void JournalTextImpl::WriteFlag( bool bFlag, FILE* file, const char* pcszTextRepr, bool & bError )
{
    if( ! bError )
    {
        const char* szFlag = bFlag ? TEXT_REPR_TRUE : TEXT_REPR_FALSE;
        WriteBuffer( pcszTextRepr, strlen(pcszTextRepr), file, bError );
        WriteBuffer( " ", 1, file, bError );
        WriteBuffer( szFlag, strlen(szFlag), file, bError );
        WriteBuffer( JRNL_DELIM, strlen(JRNL_DELIM), file, bError );
    }
}

bool JournalTextImpl::ReadFlag( FILE* file, const char* pcszTextRepr, bool & bError, bool & bBadValue )
{
    bool bResult = false;

    if( ! bError )
    {
        char szBuf[ STATIC_BUF_LEN ];
        bError = ( fgets_bin_win32( szBuf, STATIC_BUF_LEN, file ) == NULL );
        if( ! bError )
        {
            char szFlagBuf[10];
            string strFormat( pcszTextRepr );
            strFormat+= " %9s";
            int nScanResult = sscanf( szBuf, strFormat.c_str(), & szFlagBuf );

            if( ( ! nScanResult ) || ( nScanResult == EOF ) )
            {
                bBadValue = true;
                bError = true;
            } else
            {
                bResult = ( strcmp( szFlagBuf, TEXT_REPR_FALSE ) != 0 );
            }
        }
    }

    return bResult;
}

void JournalTextImpl::WriteInt32( AVP_int32 nValue, FILE* file, const char* pcszTextRepr, bool & bError )
{
    if( ! bError )
    {
        long nLongValue = nValue;
        int nBytesWritten =
            fprintf(
                file,
                "%s %20ld%s",
                pcszTextRepr,
                nLongValue,
                JRNL_DELIM
            );
        bError = bError || ( nBytesWritten == 0 );
    }
}

AVP_int32 JournalTextImpl::ReadInt32( FILE* file, const char* pcszTextRepr, bool & bError, bool & bBadValue )
{
    AVP_int32 nResult;

    if( ! bError )
    {
        char szBuf[ STATIC_BUF_LEN ];
        bError = ( fgets_bin_win32( szBuf, STATIC_BUF_LEN, file ) == NULL );
        if( ! bError )
        {
            string strFormat( pcszTextRepr );
            strFormat+= " %ld";
            int nScanResult = sscanf( szBuf, strFormat.c_str(), & nResult );

            if( ( ! nScanResult ) || ( nScanResult == EOF ) )
            {
                bBadValue = true;
                bError = true;
            }
        }
    }

    return nResult;
}

void JournalTextImpl::WriteBuffer( const void* buffer, size_t size, FILE* file, bool & bError )
{
    bError = bError || ( fwrite( buffer, 1, size, file ) != size );
}

long JournalTextImpl::ReadPosition( FILE* file, bool & bError )
{
    long nResult = 0;

    if( ! bError )
    {
        nResult = ftell( file );
        bError = ( nResult < 0 );
    }

    return nResult;
}

wstring JournalTextImpl::GetPageFilePathFromAddOn( const wstring& wstrAddOn )
{
    wstring wstrTemplFileDir;
    wstring wstrTemplFileName;
    wstring wstrTemplFileExt;

    KLSTD_SplitPath( m_wstrPageFileTempl, wstrTemplFileDir, wstrTemplFileName, wstrTemplFileExt );

    wstring wstrResult;

    wstring wstrFileName;
    if( wstrAddOn == L"0" )
    {
        wstrFileName = wstrTemplFileName + wstrTemplFileExt;
    } else
    {
        wstrFileName = wstrTemplFileName + wstrAddOn + wstrTemplFileExt;
    }

    KLSTD_PathAppend(
        wstrTemplFileDir,
        wstrFileName,
        wstrResult,
        true
    );

    return wstrResult;
}

wstring JournalTextImpl::GenerateNewPageFileNameAddOn()
{
    wstring wstrResult;

    int nCounter = 0;
    bool bStop = false;
    while( ! bStop )
    {
        basic_stringstream<wchar_t> ssAddOn;
        ssAddOn << m_vectPages.size();
        if( nCounter )
        {
            ssAddOn << L"_" << nCounter;
        }
        wstrResult = ssAddOn.str();

        bool bUsed = false;
        for( unsigned int i = 0; i < vectUsedFileNameAddOns.size(); i++ )
        {
            bUsed = vectUsedFileNameAddOns[i] == wstrResult;
            if( bUsed )
            {
                break;
            }
        }

        bStop = ! bUsed && ! KLSTD_IfExists( GetPageFilePathFromAddOn( wstrResult ).c_str() );
        nCounter++;
    }

    vectUsedFileNameAddOns.push_back( wstrResult );

    return wstrResult;
}

void JournalTextImpl::FlushJrnlFile( FILE* file, bool & bError )
{
    if( ! bError )
    {
        long nPos = ReadPosition( file, bError );
        bError = bError || fflush( file );
        bError = bError || fseek( file, nPos, SEEK_SET );
    }
}

void JournalTextImpl::SeekPage( PageText& page, long offset, int origin, bool & bError )
{
    if( ! bError )
    {
        bError = bError || fseek( page.file, offset, origin );

        page.bAtEOF = false;
    }
}

void JournalTextImpl::PageToEOF( PageText& page, bool & bError )
{
    if( ! bError )
    {
        if( ! page.bAtEOF )
        {
            bError = bError || fseek( page.file, 0, SEEK_END );
            if( ! bError )
            {
                page.bAtEOF = true;
            }
        }
    }
}

void KLJRNL_CreateSimpleJournal( KLJRNL::Journal** ppJournal, bool bMultiThread )
{
    KLSTD_CHKOUTPTR(ppJournal);
    CAutoPtr<JournalTextImpl> pJournalImpl;
    pJournalImpl.Attach( new JournalTextImpl( bMultiThread ) );
    KLSTD_CHKMEM( pJournalImpl );

    *ppJournal = pJournalImpl.Detach();
}
