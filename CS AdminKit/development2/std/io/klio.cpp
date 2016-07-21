/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file klio.cpp
 * \author јндрей  азачков
 * \date 2002
 * \brief –абота с файлами
 *
 */

#include "build/general.h"
#include "std/base/kldefs.h"
#include "avp/klids.h"
#include "std/io/klio.h"
#include "std/err/error.h"
#include "std/err/klerrors.h"
#include "std/conv/klconv.h"
#include "std/trc/trace.h"
#include <std/err/errlocids.h>

#include <time.h>
#include <stdlib.h>

#include <cstdio>
#include <sstream>

#if defined(__unix) || defined(N_PLAT_NLM)
# include <sys/stat.h>
# include <sys/types.h>
# include <dirent.h>
# include <climits>
# include <unistd.h>
# include <cerrno>

 #ifdef N_PLAT_NLM
   #include <C:/novell/ndk/libc/include/fnmatch.h> //?????????
   # include "../conv/_conv.h"
   # include <nwfileio.h>
 #else
  #include <fnmatch.h>
 #endif


# include <sys/mman.h>
# include <sys/types.h> // should be replaced by boost::filesystem - ptr
# include <sys/stat.h>

# include "../redir/typedef.h"
# include "../redir/redir.h"


# include <iostream>
# include <fstream>

# include <boost/filesystem/operations.hpp>
# include <boost/filesystem/path.hpp>
#include <boost/filesystem/exception.hpp>
#endif // __unix || N_PLAT_NLM

#ifdef __unix
#  include <libgen.h>
#endif

#ifdef N_PLAT_NLM
# include <nunicode.h>
# include <nwnspace.h>
# include "wcharcrt.h"
#endif

#ifdef _WIN32
#  include <share.h>
#ifndef STRICT
# define STRICT
#endif
#  include <windows.h>
#  include <tchar.h>
#  include <fcntl.h>
#  include <io.h>	
#endif

#include "common/measurer.h"

#define KLCS_MODULENAME L"KLSTD"

#ifdef _WIN32
	#include <crtdbg.h>
#endif

using namespace KLSTD;
using namespace KLERR;
using namespace std;

namespace
{
    struct sleep_range_t
    {
        int uFrom, uTo;
    };
    
    const sleep_range_t sleep_table[]=
    {
        {50,    100},
        {100,   200},
        {150,   300},
        {200,   400},
        {250,   500},
        {300,   600},
        {350,   700},
        {400,   800},
        {450,   900},
        {500,   1000}
    };

    int GetSleepTimeoutFrom(size_t i)
    {
        if(i >= KLSTD_COUNTOF(sleep_table))
            i = KLSTD_COUNTOF(sleep_table)-1;
        unsigned nFrom = sleep_table[i].uFrom;
        unsigned nTo = sleep_table[i].uTo;
        return KLSTD_Random(nFrom, nTo);
    };
    
#ifdef _WIN32
    const LONG c_lVmWareCachingFix = 20;
    const size_t c_nVmWareIterations = 30;
#endif
};

namespace KLSTD
{
    void LocalizeIoError(
                KLERR::Error*       pError,
                int                 nCode,
                const wchar_t*      szwString3,
                const wchar_t*      szwString4 = NULL,
                const wchar_t*      szwString5 = NULL)
    {
        if(pError)
        {
            std::wostringstream osCode;
            osCode  << unsigned(pError->GetId()) 
                    << L"/0x" 
                    << std::hex 
                    << pError->GetErrorSubcode();
            KLERR_SetErrorLocalization(
                    pError,                    
                    nCode,
                    NULL,//module name 
                    //substitution parameters
                    osCode.str().c_str(),
                    pError->GetMsg(),
                    szwString3,
                    szwString4,
                    szwString5);
        };
    };
};

namespace KLSTD
{

    #ifdef _WIN32
        typedef HANDLE fhandle_t;
    #else
        typedef int fhandle_t;
    #endif

class CFileMapping
    :   public KLBaseImpl<FileMapping>
    {
	public:
		CFileMapping(fhandle_t hMapping, void* pData, AVP_qword qwSize)
            :   KLBaseImpl<FileMapping>()
		{
			m_hMapping=hMapping;
			m_pData=pData;
			m_qwSize=qwSize;
		};

		virtual ~CFileMapping()
		{
#ifdef _WIN32
            if(m_pData)
                UnmapViewOfFile(m_pData);
            CloseHandle(m_hMapping);
#else
            if( m_pData )
            {
                munmap( m_pData, m_qwSize );
            };
            //close( m_hMapping ); NEVER !!!
#endif
		};

		void* GetPointer()
		{
            return m_pData;
		};

		AVP_qword GetSize()const 
		{
            return m_qwSize;
		};
	protected:
		AVP_qword	m_qwSize;
		fhandle_t   m_hMapping;
		void*		m_pData;
	};

	class CFile: public KLBaseImpl<File>{
	public:	
		CFile(
                const wstring&  wstrFile, 
                fhandle_t       hFile, 
                AVP_dword       dwAccess, 
                bool            bDontWorryAboutClose )
        :   KLBaseImpl<File>()
        ,   m_wstrFile(wstrFile)
        ,   m_hFile(hFile)
        ,   m_dwAccess(dwAccess)
        ,   m_bDontWorryAboutClose(bDontWorryAboutClose)//added by Karmazine
#ifdef _WIN32
        ,   m_iFile(-1)        
#endif
		{;}

		virtual ~CFile()
		{
            if( ! m_bDontWorryAboutClose )
            {
#ifdef _WIN32
			    if(m_iFile!=-1)
				    close(m_iFile);
			    else
				    CloseHandle(m_hFile);
#else
                if ( m_hFile != -1 ) {
					CloseHandle( m_hFile );
                }
#endif
            }
		}

		AVP_dword Read(void* pBuffer, AVP_dword dwBuffer)
		{
			if(!pBuffer && dwBuffer)
				KLSTD_THROW_BADPARAM(pBuffer);
			if(dwBuffer==0)
				return 0;			
            DWORD dwRead=0;
            KLERR_TRY
			    if(!ReadFile(m_hFile, pBuffer, dwBuffer, &dwRead, NULL))
				    KLSTD_THROW_LASTERROR();
            KLERR_CATCH(pError)
                LocalizeIoError(pError, STDEL_FILE_IO_ERROR, m_wstrFile.c_str());
                KLERR_RETHROW();
            KLERR_ENDTRY
			return dwRead;
		}

		AVP_dword Write(const void* pBuffer, AVP_dword dwBuffer)
		{
			if(!pBuffer && dwBuffer)
				KLSTD_THROW_BADPARAM(pBuffer);
			if(dwBuffer==0)
				return 0;

			DWORD dwWritten=0;
            KLERR_TRY
			    if(!WriteFile(m_hFile, pBuffer, dwBuffer, &dwWritten, NULL))
				    KLSTD_THROW_LASTERROR();
            KLERR_CATCH(pError)
                LocalizeIoError(pError, STDEL_FILE_IO_ERROR, m_wstrFile.c_str());
                KLERR_RETHROW();
            KLERR_ENDTRY
			return dwWritten;
		}

		AVP_qword Seek(AVP_longlong llOffset, SEEK_TYPE type)
		{			
			LONG lDistanceToMoveHigh=(LONG)(llOffset >> 32);
			LONG lDistanceToMove=(LONG)llOffset;
			DWORD dwResult= 0;
            KLERR_TRY
                dwResult= SetFilePointer(
                                m_hFile, 
                                lDistanceToMove, 
                                &lDistanceToMoveHigh, 
                                type);
			if(dwResult==-1)
				KLSTD_THROW_LASTERROR();
            KLERR_CATCH(pError)
                LocalizeIoError(pError, STDEL_FILE_IO_ERROR, m_wstrFile.c_str());
                KLERR_RETHROW();
            KLERR_ENDTRY
			return AVP_qword(dwResult) | (AVP_qword(lDistanceToMoveHigh) << 32);
		}

		AVP_qword GetSize()
		{
			DWORD dwSizeHigh=0;
			DWORD dwResult = 0;
            KLERR_TRY
                dwResult = GetFileSize(m_hFile, &dwSizeHigh);
			    if(dwResult==-1)
				    KLSTD_THROW_LASTERROR();
            KLERR_CATCH(pError)
                LocalizeIoError(pError, STDEL_FILE_IO_ERROR, m_wstrFile.c_str());
                KLERR_RETHROW();
            KLERR_ENDTRY
			return AVP_qword(dwResult) | (AVP_qword(dwSizeHigh) << 32);
		}

		const std::wstring	GetName()const 
		{
			return m_wstrFile;
		}

		void Map(	AVP_dword		dwAccess,
					AVP_qword		qwOffset,
					AVP_dword		dwSize,
					FileMapping**	ppMapping)
		{
            KLSTD_CHKOUTPTR(ppMapping);
            ;
			if((dwAccess & ~(AF_READ|AF_WRITE)) || !(dwAccess & AF_READ))
				KLSTD_THROW_BADPARAM(dwAccess);
			
			// Error if mapping has more access than the file does
			if( (dwAccess | m_dwAccess )!=m_dwAccess)
				KLSTD_THROW(STDE_NOTPERM);

#ifdef _WIN32			
			DWORD dwProtect=(dwAccess & AF_WRITE) ? PAGE_READWRITE: PAGE_READONLY;
			HANDLE	hMapping=NULL;
#else
			int dwProtect = PROT_READ;
            if ( dwAccess & AF_WRITE ) {
              dwProtect |= PROT_WRITE;
            }
#endif
			void*	pData=NULL;
			KLERR_TRY
#ifdef _WIN32			
				hMapping=CreateFileMapping(m_hFile, NULL, dwProtect, 0, dwSize, NULL);
				if(!hMapping || hMapping==INVALID_HANDLE_VALUE)
					KLSTD_THROW_LASTERROR();
				pData=MapViewOfFile(
						hMapping,
						(dwAccess & AF_WRITE) ? FILE_MAP_WRITE: FILE_MAP_READ,
						DWORD(qwOffset >> 32),
						(DWORD)qwOffset, 
						dwSize);
				*ppMapping= new CFileMapping(hMapping, pData, dwSize);
				KLSTD_CHKMEM(*ppMapping);
#endif
#if defined(__unix) // || defined(N_PLAT_NLM)
  /*
     Don't use this code for Novell NetWare!

     Reasons:
       - mmap function declared, but available only in libc
       - there are reported that mmap function in libc do nothing

  */
                size_t p_sz = getpagesize();
                size_t len = (dwSize / p_sz + 1) * p_sz; // align to page size

                // by the way, not always we need MAP_SHARED: MAP_PRIVATE?
                pData = mmap( 0, len, dwProtect, MAP_SHARED, m_hFile, 0 );
				if( pData == MAP_FAILED )
					KLSTD_THROW_LASTERROR();

				*ppMapping= new CFileMapping( m_hFile, pData, len );
				KLSTD_CHKMEM(*ppMapping);
#endif
			KLERR_CATCH(pError)
#ifdef _WIN32
				if(hMapping && hMapping != INVALID_HANDLE_VALUE)
					CloseHandle(hMapping);
				if(pData)
					UnmapViewOfFile(pData);
#endif // _WIN32
				KLERR_RETHROW();
			KLERR_ENDTRY
		};

		void Lock(AVP_qword qwOffset, AVP_qword qwBytes, long lTimeout)
		{
			unsigned long t0= (lTimeout!=KLSTD_INFINITE) ? KLSTD::GetSysTickCount(): 0;
			for(;;)
			{
				if(!LockFile(m_hFile, DWORD(qwOffset), DWORD(qwOffset >> 32), DWORD(qwBytes), DWORD(qwBytes >> 32)))
				{
#ifdef _WIN32
					DWORD dwError=GetLastError();
					if(dwError!=ERROR_LOCK_VIOLATION)
						KLSTD_THROW_LASTERROR();
#else
					if(errno!=EDEADLK)
						KLSTD_THROW_LASTERROR();
#endif
					if(
                        lTimeout==0 ||
                        (lTimeout!=KLSTD_INFINITE &&
                            (KLSTD::GetSysTickCount()-t0 > (unsigned long)lTimeout)))
						KLSTD_THROW(STDE_TIMEOUT);
					KLSTD_Sleep(500);
				}else
					break;				
			};
		};

		void Unlock(AVP_qword qwOffset, AVP_qword qwBytes)
		{
			if(!UnlockFile(m_hFile, DWORD(qwOffset), DWORD(qwOffset >> 32), DWORD(qwBytes), DWORD(qwBytes >> 32)))
				KLSTD_THROW_LASTERROR();
		};

		void SetEOF(AVP_qword qwNewSize)
		{
            KLERR_TRY
			    if(!::SetEndOfFile(m_hFile))
				    KLSTD_THROW_LASTERROR();
            KLERR_CATCH(pError)
                LocalizeIoError(pError, STDEL_FILE_IO_ERROR, m_wstrFile.c_str());
                KLERR_RETHROW();
            KLERR_ENDTRY
		}

		int GetFileID()
		{
#ifdef _WIN32
			if(m_iFile==-1){ 
				long lFlags=_O_BINARY|((m_dwAccess & AF_WRITE) ? _O_RDWR : _O_RDONLY);
				m_iFile=_open_osfhandle((long)m_hFile, lFlags);
			}
			return m_iFile;
#else
            return m_hFile;
#endif
		}
        
        bool Flush(bool bThrowError)
        {
            bool bResult = true;
            KLERR_TRY
                if(!FlushFileBuffers(m_hFile))
                {
                    if(bThrowError)
                        KLSTD_THROW_LASTERROR();
                    bResult=false;
                };
            KLERR_CATCH(pError)
                LocalizeIoError(pError, STDEL_FILE_IO_ERROR, m_wstrFile.c_str());
                KLERR_RETHROW();
            KLERR_ENDTRY
            return bResult;
        };		
	protected:
		AVP_dword		m_dwAccess;
        fhandle_t		m_hFile;
#ifdef _WIN32
		int				m_iFile;
#endif
		const wstring   m_wstrFile;
        bool            m_bDontWorryAboutClose;//added by Karmazine
	};
	
#ifdef _WIN32
	class CStdioFile: public KLBaseImpl<StdioFile>
    {
	public:
		CStdioFile(
                const wstring& wstrFile, 
                AVP_dword dwSharing, 
                AVP_dword dwCreation, 
                AVP_dword dwAccess)
        :   KLBaseImpl<StdioFile>()
        ,   m_wstrFile(wstrFile)
		{
			wchar_t szMode[4]; // C-runtime open string
			int nMode = 0;

			// determine read/write mode depending on file mode
			switch(dwCreation) {
				case CF_OPEN_EXISTING:
					szMode[nMode++] = L'r';
					if (dwAccess & AF_WRITE) szMode[nMode++] = L'+';
				break;
				case CF_CREATE_NEW:
					szMode[nMode++] = L'w';
					if (dwAccess & AF_READ) szMode[nMode++] = L'+';
				break;
				case CF_OPEN_ALWAYS:
					szMode[nMode++] = L'a';
					if (dwAccess & AF_READ) szMode[nMode++] = L'+';
				break;
				case CF_CREATE_ALWAYS:
					szMode[nMode++] = L'w';
					if (dwAccess & AF_READ) szMode[nMode++] = L'+';
				break;
				default:
					KLSTD_THROW_BADPARAM(dwCreation);
			};

			szMode[nMode++] = '\0';

			m_hFile = _tfsopen(
                            KLSTD_W2CT2(m_wstrFile.c_str()), 
                            KLSTD_W2CT2(szMode), 
                            ((dwSharing & SF_READ)? 0:_SH_DENYRD) | 
                                ((dwSharing & SF_WRITE)?0:_SH_DENYWR));
			if (!m_hFile) 
                KLSTD_THROW_LASTERROR_CODE(_doserrno);
		}

		virtual ~CStdioFile()
		{
			if (m_hFile) fclose(m_hFile);
		}

		bool ReadString(wchar_t* szBuffer, const int nSize)
		{
			KLSTD_CHKINPTR(szBuffer);
			if (nSize == 0) KLSTD_THROW_BADPARAM(nSize);

			bool rc = false;

			if (fgetws(szBuffer, nSize, m_hFile) == NULL)
			{
				if (ferror(m_hFile)) KLSTD_THROW_LASTERROR_CODE(_doserrno);
				rc = false;		// end of file reached
			}
			else
				rc = true;

			return rc;
		}
		
		void WriteString(const wchar_t* szBuffer)
		{
			KLSTD_CHKINPTR(szBuffer);

			if (fputws(szBuffer, m_hFile) == WEOF) KLSTD_THROW_LASTERROR_CODE(_doserrno);
		}

		void Seek(AVP_long lOffset, SEEK_TYPE type)
		{
			if (fseek(m_hFile, lOffset, ((type == ST_SET)?SEEK_CUR:((type & ST_END)?SEEK_CUR:SEEK_SET))) != 0) KLSTD_THROW_LASTERROR_CODE(_doserrno);
		}

		long GetSize()
		{
			Seek(0, ST_END);
			long rc = ftell(m_hFile);

			if (rc == -1L) KLSTD_THROW_LASTERROR_CODE(_doserrno);

			return rc;
		}

		const std::wstring	GetName()const 
		{
			return m_wstrFile;
		}

		int GetFileID()
		{
			return m_hFile->_file;
		}
	protected:
		FILE*			m_hFile;
		const wstring	m_wstrFile;
	};

#endif // _WIN32
	
	time_t GetFileCreateTime( const std::wstring &wstrFilePath )
	{
		time_t createTime = 0;
#ifdef _WIN32
		WIN32_FILE_ATTRIBUTE_DATA fileAttr;
		if(!GetFileAttributesEx(KLSTD_W2CT2(wstrFilePath.c_str()),
			GetFileExInfoStandard, &fileAttr ))
		{
			KLSTD_THROW_LASTERROR();
		};
		SYSTEMTIME st;
		FileTimeToSystemTime( &fileAttr.ftCreationTime, &st );
		
		struct tm tm={0};
		tm.tm_sec=st.wSecond;		//Seconds after minute (0 Ц 59)
		tm.tm_min=st.wMinute;		//Minutes after hour (0 Ц 59)
		tm.tm_hour=st.wHour;		//Hours since midnight (0 Ц 23)
		tm.tm_mday=st.wDay;			//Day of month (1 Ц 31)
		tm.tm_mon=st.wMonth-1;		//Month (0 Ц 11; January = 0)
		tm.tm_year=st.wYear-1900;	//Year (current year minus 1900)
		createTime = KLSTD_mkgmtime(&tm);
#else
		struct stat stat_struct;
		int result = stat( KLSTD_W2A2( wstrFilePath.c_str() ), &stat_struct );
		createTime = stat_struct.st_ctime;
#endif
		return createTime;
		
	}

} // namespace KLSTD

KLCSC_DECL void KLSTD_CreateFile(
				const std::wstring&	wstrPath,
				AVP_dword			dwSharing,
				AVP_dword			dwCreation,
				AVP_dword			dwAccess,
				AVP_dword			dwFlags,
				KLSTD::File**		ppFile,
				long				lTimeout)
{
    KLSTD_CHKOUTPTR(ppFile);
    ;
	KLSTD_TRACE3( 4, L"KLSTD_CreateFile path - '%ls' timeout - %d creation flags - 0x%X\n", 
		wstrPath.c_str(), lTimeout, dwCreation );
#ifdef N_PLAT_NLM
    bool n_path = false;
    if ( wcschr(wstrPath.c_str(),L':') ) {
	KLSTD_TRACE1( 4, L": - OK %ls\n",wstrPath.c_str());
    }
    else {
	KLSTD_TRACE1( 4, L": - NO %ls\n",wstrPath.c_str());
        n_path = true;
    }	
#endif

KL_TMEASURE_BEGIN(L"KLSTD_CreateFile", 4);
	int dwWinCreation = 0;
	switch(dwCreation){
	case CF_OPEN_EXISTING:
		dwWinCreation=OPEN_EXISTING;
		break;
	case CF_CREATE_NEW:
		dwWinCreation=CREATE_NEW;
		break;
	case CF_OPEN_ALWAYS:
		dwWinCreation=OPEN_ALWAYS;
		break;
	case CF_TRUNCATE_EXISTING:
		dwWinCreation=TRUNCATE_EXISTING;
		break;
	case CF_CREATE_ALWAYS:
		dwWinCreation=CREATE_ALWAYS;
		break;
	default:
		KLSTD_THROW_BADPARAM(dwCreation);
	};
	
	int dwWinAccess=((dwAccess & AF_READ) ? GENERIC_READ : 0)|((dwAccess & AF_WRITE) ? GENERIC_WRITE : 0);
	int dwWinShare=((dwSharing & SF_READ) ? FILE_SHARE_READ : 0)|((dwSharing & SF_WRITE) ? FILE_SHARE_WRITE : 0);
	int dwWinFlags=
		((dwFlags & EF_RANDOM_ACCESS) ? FILE_FLAG_RANDOM_ACCESS : 0) |
		((dwFlags & EF_SEQUENTIAL_SCAN) ? FILE_FLAG_SEQUENTIAL_SCAN : 0) |
		((dwFlags & EF_TEMPORARY) ? FILE_ATTRIBUTE_TEMPORARY: 0) |
        ((dwFlags & EF_DELETE_ON_CLOSE) ? FILE_FLAG_DELETE_ON_CLOSE: 0) 
#ifdef _WIN32
        | ((dwFlags & EF_WRITE_THROUGH) ? FILE_FLAG_WRITE_THROUGH : 0)
#endif
        ;

    //<-- Added by Karmazine
    bool bDontWorryAboutClose = ( dwFlags & EF_DONT_WORRY_ABOUT_CLOSE ) != 0;
    // -->    

	HANDLE hFile = INVALID_HANDLE_VALUE;
	KLERR_TRY
        KLSTD::kltick_t t0= (lTimeout!=KLSTD_INFINITE) ? KLSTD_GetSysTickCount() : 0;
		for(int ii = 0 ;; ++ii)
		{
#if defined(_WIN32)
			hFile = CreateFile(
						KLSTD_W2CT2(wstrPath.c_str()),
						dwWinAccess,
						dwWinShare,
						NULL,
						dwWinCreation,
						dwWinFlags,
						NULL);
#elif defined(__unix)
            bool bFileLocked = false;
            hFile=CreateFile(
						KLSTD_W2CA2(wstrPath.c_str()),
						dwWinAccess,
						dwWinShare,
						NULL,
						dwWinCreation,
						dwWinFlags,
						0,
                        bFileLocked);
#elif defined(N_PLAT_NLM)
            bool bFileLocked = false;

			hFile=CreateFile(
						KLSTD_W2CT2(wstrPath.c_str()),
						dwWinAccess,
						dwWinShare,
						NULL,
						dwWinCreation,
						dwWinFlags,
						0,
                        bFileLocked);
#else
    #error("Not implemented")
#endif						
			if(hFile==INVALID_HANDLE_VALUE)
			{
#ifdef _WIN32
				DWORD dwError=GetLastError();
				if(dwError!=ERROR_LOCK_VIOLATION && dwError!=ERROR_SHARING_VIOLATION)
                {
                  KLSTD_TRACE2(
                                3,
                                L"Win32 error 0x%X while opening file '%ls'\n", 
                                dwError,
                                wstrPath.c_str());
                    KLSTD_THROW_LASTERROR_CODE(dwError);
                };

#else

				if ( !bFileLocked )

				{

					KLSTD_TRACE2(

                                3,

                                L"errno - 0x%X while opening file '%ls'\n", 

                                errno,

                                wstrPath.c_str());

                    KLSTD_THROW_LASTERROR_CODE(errno);

				};

#endif
                if( lTimeout==0 || 
                    (   lTimeout != KLSTD_INFINITE && 
                        (KLSTD_GetSysTickCount()-t0 > (KLSTD::kltick_t)lTimeout))
                )
                {
					KLSTD_THROW(STDE_TIMEOUT);
                };
        #if defined(_DEBUG)
                KLSTD_TRACE0(4, L"File is busy - waiting\n");
        #endif
				KLSTD_Sleep(GetSleepTimeoutFrom(ii));
			}

            else
				break;
		}
		*ppFile = new CFile( wstrPath, hFile, dwAccess, bDontWorryAboutClose);
		KLSTD_CHKMEM(*ppFile);
	KLERR_CATCH(pError)
		if(hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
        LocalizeIoError(pError, STDEL_FILE_OPEN_ERROR, wstrPath.c_str());
		KLERR_RETHROW();
	KLERR_ENDTRY
KL_TMEASURE_END();
}

#ifdef _WIN32
KLCSC_DECL void KLSTD_CreateStdioFile(
				const std::wstring&	wstrPath,
				AVP_dword			dwSharing,
				AVP_dword			dwCreation,
				AVP_dword			dwAccess,
				KLSTD::StdioFile**	ppFile)
{
	KLSTD_CHKOUTPTR(ppFile);

	*ppFile = new KLSTD::CStdioFile(wstrPath, dwSharing, dwCreation, dwAccess);
	KLSTD_CHKMEM(*ppFile);
}
#endif // _WIN32

KLCSC_DECL void KLSTD_SplitPath(
						const std::wstring&		wstrFullPath,
						std::wstring&			wstrDir,
						std::wstring&			wstrName,
						std::wstring&			wstrExt)
	{
#ifdef _WIN32
		wchar_t drive[_MAX_DRIVE];
		wchar_t dir[_MAX_DIR];
		wchar_t fname[_MAX_FNAME];
		wchar_t ext[_MAX_EXT];
		_wsplitpath( wstrFullPath.c_str(), drive, dir, fname, ext);
		wstrDir=drive;
		wstrDir+=dir;
		wstrName=fname;
		wstrExt=ext;
#endif // _WIN32
#ifdef N_PLAT_NLM
        _conv c;
        char fullpathNovell[_MAX_PATH];
        wcstombs(fullpathNovell,wstrFullPath.c_str(),_MAX_PATH);

		char driveNovell[64];
		char dirNovell[254];
		char fnameNovell[256];
		char extNovell[256];
	
 	   char *p = strchr(fullpathNovell,':');
       if (p) {
          strncpy(driveNovell,fullpathNovell,p-fullpathNovell);
          driveNovell[p-fullpathNovell]=0;
          p++;
       }
       else {
          driveNovell[0]=0;
          p = fullpathNovell;
       }   

	   char *p1= strrchr(p,'\\');
	   if (p1){
		 p1++;
         strncpy(dirNovell,p,p1-p);
         dirNovell[p1-p]=0;
         p++;
		}
        else {
         p1= strrchr(p,'/');
         if (p1) {
		   p1++;
           strncpy(dirNovell,p,p1-p);
           dirNovell[p1-p]=0;
           p++;
         }
         else {
           dirNovell[0]=0;
           p1 =p;						 
         } 
        }
                 
  	    p = strchr(p1,'.');
        if (p){
          strncpy(fnameNovell,p1,p-p1);
          fnameNovell[p-p1]=0;
          strcpy(extNovell,p);    
        }
		else {
          strcpy(fnameNovell,p1);    
          extNovell[0]=0;
        }  
              
		wchar_t drive[64];
		wchar_t dir[254];
		wchar_t fname[256];
		wchar_t ext[256];

		mbstowcs(drive,driveNovell,strlen(driveNovell)+1);
		mbstowcs(dir,dirNovell,254);
		mbstowcs(fname,fnameNovell,256);
		mbstowcs(ext,extNovell,256);

		wstrDir=drive;
		wstrDir+=L":";
		wstrDir+=dir;
		wstrName=fname;
		wstrExt=ext;      
#endif

#ifdef __unix
        std::string path = (const char*)KLSTD_W2CA2( wstrFullPath.c_str() );
        // hmm, let's see: /home/x/archives.org
        // what is fname, what is path (consider cases, when archives.org
        // exist and ones i) catalog; ii) plain file)
        // I use follow conventions (basename(3)):
        // path           dirname        basename
        // "/usr/lib"     "/usr"         "lib"
        // "/usr/"        "/"            "usr"
        // "usr"          "."            "usr"
        // "/"            "/"            "/"
        // "."            "."            "."
        // ".."           "."            ".."

        std::string base( basename( const_cast<char *>(path.c_str()) ) );
        std::string dir( dirname( const_cast<char *>(path.c_str()) ) );
        wstrDir = KLSTD_A2CW2( dir.c_str() );
        std::string::size_type p = base.rfind( '.' );
        std::string ext;
        if ( p != std::string::npos ) {
          ext = base.substr( p );
          base = base.substr( 0, p );
        }
        wstrName = KLSTD_A2CW2( base.c_str() );
        wstrExt = KLSTD_A2CW2(ext.c_str() );
		
		KLSTD_TRACE4(4, L"SplitPath (full - '%ls' dir - '%ls' name - '%ls' ext - '%ls')\n", 
			wstrFullPath.c_str(), wstrDir.c_str(), wstrName.c_str(), wstrExt.c_str() );
#endif
	}

	KLCSC_DECL void KLSTD_MakePath(					
						const std::wstring&		wstrDir,
						const std::wstring&		wstrName,
						const std::wstring&		wstrExt,
						std::wstring&			wstrFullPath)
	{
#ifdef _WIN32
        KLSTD_CHK(wstrName, wstrDir.size() + wstrName.size() + wstrExt.size() < _MAX_PATH);
		wchar_t path_buffer[_MAX_PATH + 4];
		_wmakepath(path_buffer, NULL, wstrDir.c_str(), wstrName.c_str(), wstrExt.c_str());
		wstrFullPath=path_buffer;
#endif
#if defined (__unix) || defined (N_PLAT_NLM)

        bool add_slash = true;
        wstrFullPath.reserve( wstrDir.size() + wstrName.size() + wstrDir.size() + 2 );
        if ( wstrDir.size() > 0 ) {
          if ( wstrDir[wstrDir.size()-1] == L'/' ) {
            add_slash = false;
          }
        }

        wstrFullPath = wstrDir;
        wstrFullPath += add_slash ? L"/" : L"";
        wstrFullPath += wstrName;
        if ( wstrExt.size() > 0 ) {			
			if ( wstrExt[0]!=L'.' ) wstrFullPath += L".";
			wstrFullPath += wstrExt;
        }

		KLSTD_TRACE4(4, L"MakePath(full - '%ls' dir - '%ls' name - '%ls' ext - '%ls')\n", 
			wstrFullPath.c_str(), wstrDir.c_str(), wstrName.c_str(), wstrExt.c_str() );
#endif
	}


KLCSC_DECL bool KLSTD_IfExists(const wchar_t* szwFileName)
{
    bool bResult = false;
#ifdef _WIN32
    bResult = (_taccess(KLSTD_W2CT2(szwFileName), 0)==0);
#else
	struct stat st;
    bResult = ( stat( KLSTD_W2CA2( szwFileName ), &st ) == 0 );
#endif
    return bResult;
};

//! for ss only
KLCSC_DECL bool KLSTD_IfExists2(const wchar_t* szwFileName)
{
    bool bResult = false;
    KLERR_TRY
#ifdef _WIN32
    KLSTD_W2CT2 tstrFileName(szwFileName);        
    for(size_t ii = 0; ; ++ii)
    {        
        const int nRes = _taccess(tstrFileName, 0);
        const int error = errno;
        if(-1 == nRes && EACCES == error)
        {
            KLSTD_TRACE0(1, L"\nError Access Denied !!!\n");
            if(ii < c_nVmWareIterations)
            {
                KLSTD_Sleep(GetSleepTimeoutFrom(ii));
                continue;
            }
            else
            {
                KLSTD_THROW_ERRNO_CODE(error);
            };
        }
        else if(-1 == nRes && error != ENOENT)
        {
            KLSTD_THROW_ERRNO_CODE(error);
        };
        bResult = (nRes==0);
        break;
    };
#else
	struct stat st;
    int nRes = stat( KLSTD_W2CA2( szwFileName ), &st);
    if(-1 == nRes)
    {
        KLSTD_THROW_ERRNO();
    };
    bResult = (nRes == 0 );
#endif
    KLERR_CATCH(pError)
        LocalizeIoError(pError, STDEL_FILESYSTEM_ERROR, szwFileName);
        KLERR_RETHROW();
    KLERR_ENDTRY
    return bResult;
};


KLCSC_DECL void KLSTD_Rename(const wchar_t* szwOldFileName, const wchar_t* szwNewFileName)
{
    KLSTD_CHK(szwOldFileName, szwOldFileName && szwOldFileName[0]);
    KLSTD_CHK(szwNewFileName, szwNewFileName && szwNewFileName[0]);
    KLSTD_TRACE2(4, L"KLSTD_Rename('%ls', '%ls')\n", szwOldFileName, szwNewFileName);
    KLERR_TRY
    #ifdef _WIN32
        KLSTD_W2CT2 tstrOldName(szwOldFileName);
        KLSTD_W2CT2 tstrNewName(szwNewFileName);
        if(!MoveFile(tstrOldName, tstrNewName))
        {
            DWORD dwError = GetLastError();
            KLSTD_THROW_LASTERROR_CODE2(dwError);
        };
    #else
        if ( rename( 
                KLSTD_W2CA2( szwOldFileName ), 
                KLSTD_W2CA2( szwNewFileName ) ) )
        {
            KLSTD_THROW_ERRNO();
        };
    #endif
    KLERR_CATCH(pError)
        LocalizeIoError(pError, STDEL_FILE_MOVE_ERROR, szwOldFileName, szwNewFileName);
        KLERR_RETHROW();
    KLERR_ENDTRY
};

KLCSC_DECL void KLSTD_Rename2(const wchar_t* szwOldFileName, const wchar_t* szwNewFileName)
{
    KLSTD_CHK(szwOldFileName, szwOldFileName && szwOldFileName[0]);
    KLSTD_CHK(szwNewFileName, szwNewFileName && szwNewFileName[0]);
    KLSTD_TRACE2(4, L"KLSTD_Rename2('%ls', '%ls')\n", szwOldFileName, szwNewFileName);
    KLERR_TRY
    #ifdef _WIN32
        KLSTD_W2CT2 tstrOldName(szwOldFileName);
        KLSTD_W2CT2 tstrNewName(szwNewFileName);
        for(size_t ii = 0; ; ++ii)
        {
            if(!MoveFile(tstrOldName, tstrNewName))
            {
                DWORD dwError = GetLastError();
                if(ERROR_ACCESS_DENIED == dwError || ERROR_ALREADY_EXISTS == dwError)
                {
                    KLSTD_TRACE0(1, L"\nError Access Denied !!!\n");
                    if(ii < c_nVmWareIterations)
                    {
                        KLSTD_Sleep(GetSleepTimeoutFrom(ii));
                        continue;
                    }
                    else
                    {
                        KLSTD_THROW_LASTERROR_CODE2(dwError);
                    };
                }
                else
                {
                    KLSTD_THROW_LASTERROR_CODE2(dwError);
                };
            };
            break;
        };
    #else
        if ( rename( 
                KLSTD_W2CA2( szwOldFileName ), 
                KLSTD_W2CA2( szwNewFileName ) ) )
        {
            KLSTD_THROW_ERRNO();
        };
    #endif
    KLERR_CATCH(pError)
        LocalizeIoError(pError, STDEL_FILE_MOVE_ERROR, szwOldFileName, szwNewFileName);
        KLERR_RETHROW();
    KLERR_ENDTRY
};


KLCSC_DECL bool KLSTD_Unlink(const wchar_t* szwFileName, bool bThrowExcpt)
{
    KLSTD_TRACE1(4, L"KLSTD_Unlink('%ls')\n", szwFileName);
    KLERR_TRY
    #ifdef _WIN32
        BOOL bResult = TRUE;
        DWORD dwCode = 0;
        KLSTD_W2CT2 wstrName(KLSTD::FixNullString(szwFileName));

        if(!DeleteFile(wstrName))
        {
            bResult = FALSE;
            dwCode = GetLastError();
        };
        if(!bResult)
	    {
		    if(!bThrowExcpt)
			    return false;
		    DWORD dwError=dwCode;
		    if(dwError==ERROR_FILE_NOT_FOUND || dwError==ERROR_PATH_NOT_FOUND)
			    return false;
		    KLSTD_THROW_LASTERROR_CODE(dwError);
	    };
    #else
	    if(unlink(KLSTD_W2CA2(KLSTD::FixNullString(szwFileName)))!=0)
	    {
		    if(!bThrowExcpt)
			    return false;
		    if(errno==ENOENT)
			    return false;
		    KLSTD_THROW_ERRNO();
	    };
    #endif		
    KLERR_CATCH(pError)
        LocalizeIoError(pError, STDEL_FILE_DELETE_ERROR, szwFileName);
        KLERR_RETHROW();
    KLERR_ENDTRY
	return true;
};

KLCSC_DECL bool KLSTD_Unlink2(const wchar_t* szwFileName, bool bThrowExcpt)
{
    KLSTD_TRACE1(4, L"KLSTD_Unlink2('%ls')\n", szwFileName);
    KLERR_TRY
    #ifdef _WIN32
        BOOL bResult = TRUE;
        DWORD dwCode = 0;
        KLSTD_W2CT2 wstrName(KLSTD::FixNullString(szwFileName));

        for(size_t ii = 0; ; ++ii)
        {
            if(!DeleteFile(wstrName))
            {
                bResult = FALSE;
                dwCode = GetLastError();
                if(ERROR_ACCESS_DENIED == dwCode)
                {
                    if(ii < c_nVmWareIterations)
                    {
                        KLSTD_TRACE0(1, L"\nError Access Denied !!!\n");
                        KLSTD_Sleep(GetSleepTimeoutFrom(ii));
                        bResult = TRUE;
                        dwCode = 0;
                        continue;
                    }
                    else
                    {
                        break;
                    }
                };
            };
            break;
        };
        if(!bResult)
	    {
		    if(!bThrowExcpt)
			    return false;
		    DWORD dwError=dwCode;
		    if(dwError==ERROR_FILE_NOT_FOUND || dwError==ERROR_PATH_NOT_FOUND)
			    return false;
		    KLSTD_THROW_LASTERROR_CODE(dwError);
	    };
    #else
	    if(unlink(KLSTD_W2CA2(KLSTD::FixNullString(szwFileName)))!=0)
	    {
		    if(!bThrowExcpt)
			    return false;
		    if(errno==ENOENT)
			    return false;
		    KLSTD_THROW_ERRNO();
	    };
    #endif		
    KLERR_CATCH(pError)
        LocalizeIoError(pError, STDEL_FILE_DELETE_ERROR, szwFileName);
        KLERR_RETHROW();
    KLERR_ENDTRY
	return true;
};


KLCSC_DECL bool KLSTD_CreateDirectory(const wchar_t* szwDir, bool bExcept)
{
    KLERR_TRY
        KLSTD_TRACE2(3, L"KLSTD_CreateDirectory('%ls', %u)\n", szwDir, int(bExcept));
#if defined(_WIN32)
        if( ! CreateDirectory( KLSTD_W2CT2( szwDir ), NULL ) )
        {
            DWORD dwError = GetLastError();
            if(bExcept)
                KLSTD_THROW_LASTERROR_CODE(dwError);
            return false;
        };
#else
    #if defined(N_PLAT_NLM)
        if ( ::mkdir( KLSTD_W2CA2(szwDir) ) != 0 ) {
    #elif defined(__unix)
        if ( ::mkdir( KLSTD_W2CA2(szwDir), S_IRWXU | S_IRWXG | S_IRWXO ) != 0 ) {
    #endif
            int err = errno;
            KLSTD_TRACE1(1, L"errno = 0x%X\n", err);
            if( bExcept ){
                KLSTD_THROW_LASTERROR_CODE( err );
            };
            return false;
        };
#endif
    KLERR_CATCH(pError)
        LocalizeIoError(pError, STDEL_FILESYSTEM_ERROR, szwDir);
        KLERR_RETHROW();
    KLERR_ENDTRY
    return true;
};

KLCSC_DECL bool KLSTD_RemoveDirectory(const wchar_t* szwDir, bool bExcept)
{
    KLERR_TRY
        KLSTD_TRACE2(3, L"KLSTD_RemoveDirectory('%ls', %u)\n", szwDir, int(bExcept));
#ifdef _WIN32
	if(!RemoveDirectory(KLSTD_W2CT2(szwDir)))
	{
		DWORD dwError=GetLastError();
		if( dwError == ERROR_FILE_NOT_FOUND || dwError == ERROR_PATH_NOT_FOUND)
			return false;
		if(bExcept)
			KLSTD_THROW_LASTERROR_CODE(dwError);
		return false;
	};
#else
    if ( rmdir( KLSTD_W2CA2( szwDir ) ) != 0 ) 
    {
        if ( errno == ENOENT ) 
        {
            return false;
        }
        if ( bExcept ) 
        {
            KLSTD_THROW_LASTERROR_CODE(errno);
        }
        return false;
    }
#endif
    KLERR_CATCH(pError)
        LocalizeIoError(pError, STDEL_DIR_DELETE_ERROR, szwDir);
        KLERR_RETHROW();
    KLERR_ENDTRY
	return true;
};

KLCSC_DECL bool KLSTD_IsDirectory(const wchar_t* szwFileName)
{
	KLSTD_CHK(szwFileName, szwFileName[0]);
    bool bResult = false;
    KLERR_TRY
#ifdef _WIN32
        DWORD dwAttr = GetFileAttributes(KLSTD_W2CT2(szwFileName));
        if(dwAttr == 0xFFFFFFFF)
        {
            KLSTD_THROW_LASTERROR();
        };
        bResult = ( dwAttr & FILE_ATTRIBUTE_DIRECTORY )
                    ?   true
                    :   false;
#else
        try
        {
            KLSTD_W2CA2 astrPath(szwFileName);
            bResult = boost::filesystem::is_directory((const char*)astrPath);
        }
        catch(const boost::filesystem::filesystem_error& error)
        {
            if(error.native_error())
                KLSTD_THROW_LASTERROR_CODE2(error.native_error());
            else
                throw;
        };
#endif
    KLERR_CATCH(pError)
        KLERR_SAY_FAILURE(3, pError);
        LocalizeIoError(pError, STDEL_FILESYSTEM_ERROR, szwFileName);
        KLERR_RETHROW();
    KLERR_ENDTRY
    return bResult;
};

KLCSC_DECL bool KLSTD_PathAppend( const std::wstring& wstrPath,
                                  const std::wstring& wstrMore,
                                  std::wstring& wstrResult,
                                  bool bExcept )
{
    wstrResult.clear();
    wstrResult.reserve(wstrPath.size() + wstrMore.size() + 1);
    wstrResult = wstrPath;
	if( wstrPath.size() > 0 &&
        !wcschr(KLSTD::c_szwPathDelimiters, wstrPath[wstrPath.size()-1]) )
    {
         wstrResult += KLSTD::SLASH;
    };
    wstrResult += wstrMore;
    return true;
}


KLCSC_DECL void KLSTD_GetTempFile(std::wstring& wstrTmpFileName)
{
    wstrTmpFileName.clear();
#if defined(_WIN32)
	TCHAR szPath[2*MAX_PATH]=_T("");
	if(!GetTempPath(KLSTD_COUNTOF(szPath)-1, szPath))
		KLSTD_THROW_LASTERROR();    
    /*
    Under 9x and NT4 following code may return the same name if called from
    multiple threads so we use GUIDS instead.

	TCHAR szName[MAX_PATH]=_T("");	
	if(!GetTempFileName(szPath, _T("klc"), 0, szName))
		KLSTD_THROW_LASTERROR();
	wstrTmpFileName=KLSTD_T2CW(szName);
    */
    KLSTD_T2CW2 wstrTempDir(szPath);
    const wchar_t* pTempDir = wstrTempDir;
#elif defined(__unix)
    KLSTD_A2CW2 wstrTempDir(P_tmpdir);
    const wchar_t* pTempDir = wstrTempDir;
#elif defined(N_PLAT_NLM)
    #  warning "Not emplemented yet"
    std::wstring mystr = L"sys:/adminkit/temp.tmp";
    wstrTmpFileName = mystr;
#else
    #error "Not implemented"
#endif
    KLSTD_MakePath(
                pTempDir,
                KLSTD_CreateGUIDString(),
                L".tmp",
                wstrTmpFileName);
};

KLCSC_DECL int KLSTD_FClose( FILE *stream )
{
#ifdef _WIN32
	return fclose( stream );
#else
	int fid = fileno( stream );
	KLSTD_AlternativeUnLockFile( fid );
	return fclose( stream );
#endif
}

KLCSC_DECL void KLSTD_CopyFile(
                               const wchar_t*   szwExistingFileName,
                               const wchar_t*   szwNewFileName,
                               bool             bOverwrite)
{

    KLSTD_CHKINPTR(szwExistingFileName);
    KLSTD_CHKINPTR(szwNewFileName);
    KLERR_TRY
    #ifdef _WIN32
        if(!CopyFile(KLSTD_W2CT2(szwExistingFileName), KLSTD_W2CT2(szwNewFileName), !bOverwrite))
            KLSTD_THROW_LASTERROR();
    #else
        try
        {
            namespace fs = boost::filesystem;
            fs::path from( fs::initial_path() );
            fs::path to( fs::initial_path() );
            string p = (const char*)KLSTD_W2CA2( szwExistingFileName );
            from = fs::system_complete( fs::path( p, fs::native ) );
            p = (const char*)KLSTD_W2CA2( szwNewFileName );
            to = fs::system_complete( fs::path( p, fs::native ) );
            if ( !bOverwrite && fs::exists( to ) )
            {
              std::cerr << "\nCan't overwrite: " << to.native_file_string() << std::endl;
              return;
            }
            fs::copy_file( from, to );
        }
        catch(const boost::filesystem::filesystem_error& error)
        {
            if(error.native_error())
                KLSTD_THROW_LASTERROR_CODE2(error.native_error());
            else
                throw;
        };
    #endif
    KLERR_CATCH(pError)
        LocalizeIoError(pError, STDEL_FILE_COPY_ERROR, szwExistingFileName, szwNewFileName);
        KLERR_RETHROW();
    KLERR_ENDTRY
};
	
KLCSC_DECL void KLSTD_GetFilesByMask(
                const std::wstring&         wstrMask,
                std::vector<std::wstring>&  vecNames)
{
#ifdef _WIN32
    HANDLE hSearch=NULL;
    vecNames.clear();
    vecNames.reserve(100);
    KLERR_TRY
        WIN32_FIND_DATA fd = {0};
        hSearch=FindFirstFile(KLSTD_W2CT2(wstrMask.c_str()), &fd); 
        if(hSearch != INVALID_HANDLE_VALUE)
        {
            do{
                if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    continue;
                vecNames.push_back((const wchar_t*)KLSTD_T2W2(fd.cFileName));
            }while(FindNextFile(hSearch, &fd));
        };
        DWORD dwError=GetLastError();
        if(
                dwError &&                          // error
                dwError != ERROR_NO_MORE_FILES &&   //no more files
                !( dwError==ERROR_FILE_NOT_FOUND && hSearch == INVALID_HANDLE_VALUE))// no files at all
            KLSTD_THROW_LASTERROR_CODE(dwError);
    KLERR_CATCH(pError)
        KLERR_SAY_FAILURE(3, pError);
    KLERR_FINALLY
        LocalizeIoError(pError, STDEL_FILESYSTEM_ERROR, wstrMask.c_str());
        if(hSearch)
            FindClose(hSearch);
        KLERR_RETHROW();
    KLERR_ENDTRY        
#else
	std::wstring wstrDir, wstrName, wstrExt;
	KLSTD_SplitPath( wstrMask, wstrDir, wstrName, wstrExt );

	wstrName = wstrName + wstrExt;

	KLSTD_TRACE2( 4, L"KLSTD_GetFilesByMask  dir - '%ls' mask - '%ls'\n", 
					wstrDir.c_str(), wstrName.c_str() );

	DIR *dirDesc = opendir( KLSTD_W2CA2(wstrDir.c_str()) );
	if ( dirDesc!=NULL ) 
	{
		struct dirent *dp;
		while( dp = readdir(dirDesc)  )
		{
			if ( fnmatch( KLSTD_W2CA2(wstrName.c_str()), dp->d_name, FNM_PATHNAME )==0 )
			{
				vecNames.push_back((const wchar_t*)KLSTD_A2CW2(dp->d_name));
			}
		}
		
		closedir( dirDesc );		
	}
#endif
};
#ifdef _WIN32
KLCSC_DECL void KLSTD_GetSubdirsByMask(
                const std::wstring&         wstrMask,
                std::vector<std::wstring>&  vecNames)
{
	HANDLE hSearch=NULL;
    vecNames.clear();
    vecNames.reserve(100);
    KLERR_TRY
        WIN32_FIND_DATA fd = {0};
        hSearch=FindFirstFile(KLSTD_W2CT2(wstrMask.c_str()), &fd); 
        if(hSearch != INVALID_HANDLE_VALUE)
        {
            do{
                if( 0 == (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                    continue;

                if( 0 == _tcsicmp(fd.cFileName, _T(".")) || 
                    0 == _tcsicmp(fd.cFileName, _T("..")) )
                {
                    continue;
                };

                vecNames.push_back((const wchar_t*)KLSTD_T2W2(fd.cFileName));
            }
            while(FindNextFile(hSearch, &fd));
        };
        DWORD dwError=GetLastError();
        if(
                dwError &&                          // error
                dwError != ERROR_NO_MORE_FILES &&   //no more files
                !( dwError==ERROR_FILE_NOT_FOUND && hSearch == INVALID_HANDLE_VALUE))// no files at all
            KLSTD_THROW_LASTERROR_CODE(dwError);
    KLERR_CATCH(pError)
        KLERR_SAY_FAILURE(3, pError);
    KLERR_FINALLY
        LocalizeIoError(pError, STDEL_FILESYSTEM_ERROR, wstrMask.c_str());
        if(hSearch)
            FindClose(hSearch);
        KLERR_RETHROW();
    KLERR_ENDTRY
};
#endif

#ifndef __unix__
	#define ISDELIM(_x) ( (_x)==L'/' || (_x)==L'\\')
#else
    #define ISDELIM(_x) _x==L'/'
#endif

KLCSC_DECL void KLSTD_CreatePath(const wchar_t* szwFullPath)
{
    KLERR_TRY
        KLSTD_CHK(szwFullPath, szwFullPath && szwFullPath[0]);
        	    KLSTD_TRACE1( 4, L"KLSTD_CreatePath path - '%ls'\n", szwFullPath );
    #ifdef _WIN32
        {
            DWORD dwAttr=::GetFileAttributes(KLSTD_W2CT2(szwFullPath));
            if(dwAttr != 0xFFFFFFFF && (dwAttr & FILE_ATTRIBUTE_DIRECTORY))
                return ;
        };
    #elif defined(N_PLAT_NLM)
        if( mkdir_510(KLSTD_W2CA2(szwFullPath), S_IRWXU|S_IRWXG|S_IRWXO)==(-1) && 
            errno == EEXIST )
		{
            return;
        };
    #else
        if( mkdir(KLSTD_W2CA2(szwFullPath), S_IRWXU|S_IRWXG|S_IRWXO)==(-1) && 
            errno == EEXIST )
        {
            return;
        };
    #endif
        const int nBuffer = wcslen(szwFullPath) + 1;
        std::vector<wchar_t> vecBuffer;
        vecBuffer.resize(nBuffer+1);
        wchar_t* szBuffer=&vecBuffer[0];//(wchar_t*)alloca(nBuffer * sizeof(wchar_t));
        bool bIsServername = nBuffer >= 3 && ISDELIM(szwFullPath[0]) && ISDELIM(szwFullPath[1]);
        bool bShareName = false;
        for(const wchar_t* p=szwFullPath;; ++p)
        {
            if(ISDELIM(*p) || (*p) == 0)
            {
			    #define PREV() (*(p-1))

                // ignore cases of \  \\  :\ 
			    if ( ISDELIM(*p) )
			    {
				    if ( ISDELIM(*p) && ( p == szwFullPath || (PREV() == L':') || (ISDELIM(PREV())) ) )
				    {
					    continue;
				    }
			    }
                #undef PREV
                if(bIsServername)
                {
                    bIsServername = false;
                    bShareName = true;
                    continue;
                };
                int nLength=p-szwFullPath;
                memcpy(szBuffer, szwFullPath, sizeof(wchar_t)*nLength);
                szBuffer[nLength]=0;
                KLSTD_ASSERT(nLength<nBuffer);
    #ifdef _WIN32
                if(bShareName)
                {
                    bShareName = false;
                }
                else
                {
                    KLSTD_W2CT2 dir(szBuffer);
                    LPCTSTR lpDirectory=dir;
                    if(!CreateDirectory(lpDirectory, NULL))
                    {
                        DWORD dwError=GetLastError();
                        if(dwError != ERROR_ALREADY_EXISTS && dwError != ERROR_FILE_EXISTS)
                            KLSTD_THROW_LASTERROR_CODE(dwError);
                        DWORD dwAttr=GetFileAttributes(lpDirectory);
                        if(dwAttr == INVALID_FILE_ATTRIBUTES)
                            KLSTD_THROW_LASTERROR();
                        if((dwAttr & FILE_ATTRIBUTE_DIRECTORY) == 0)
                            KLSTD_THROW(STDE_NOTDIR);
                    };
                };
    #else			
                KLSTD_W2A2 dir(szBuffer);
			    char *lpDirectory = dir;    
    #ifdef N_PLAT_NLM 
                if(mkdir_510(lpDirectory, S_IRWXU|S_IRWXG|S_IRWXO)==(-1))
			    {
				    if ( errno!=EEXIST) KLSTD_THROW_LASTERROR();
                                }  
    #else
                if(mkdir(lpDirectory, S_IRWXU|S_IRWXG|S_IRWXO)==(-1))
                {
                    if ( errno!=EEXIST) KLSTD_THROW_LASTERROR();
                } ;
    #endif 			
    #endif
                if((*p) == 0)
                    break;
            };
        };
    KLERR_CATCH(pError)
        LocalizeIoError(
                pError, 
                STDEL_FILESYSTEM_ERROR, 
                KLSTD::FixNullString(szwFullPath));
        KLERR_RETHROW();
    KLERR_ENDTRY
};
