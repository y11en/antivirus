/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file KlBase.cpp
 * \author Андрей Казачков
 * \date 2002
 * \brief Функции модуля KLSTD
 *
 */

#ifdef _WIN32    
    #include "std/win32/klos_win32v50.h"
    #include <eh.h>
#endif
#include "std/base/klbase.h"
#include "std/base/klstdutils.h"
#include "build/general.h"
#include "avp/klids.h"
#include "std/err/error.h"
#include <std/hstd/hostdomain.h>
#include <std/err/errlocids.h>
#include "std/err/klerrors.h"
#include "std/thr/sync.h"
#include "std/thr/thread.h"
#include "std/conv/klconv.h"
#include "std/io/klio.h"
#include "std/thrstore/threadstore.h"
#include "std/thr/thread.h"
#include <std/thr/locks.h>
#include "std/trc/trace.h"
#include "std/gsoap/stubrouter.h"
#include "std/memory/klmem.h"
#include <common/reporter.h>
#include <common/bin2hex.h> //MakeHexDataW
#if defined(_WIN32) && defined(KLUSEFSSYNC)
#ifndef __out
#define __out
#define __in
#define __inout
#define __inout
#define __in_opt
#endif
#include "../../../Windows/Hook/Hook/FSSync.h"
//#pragma comment(lib, "FSSync.lib")
#endif
#include <time.h>
#include <stdlib.h>

#include <cstdio>
#include <sstream>

#include <fcntl.h>

#if defined(__unix) //|| defined(N_PLAT_NLM)

#ifdef USE_BOOST_THREADS
	#include <boost/thread/thread.hpp>	
	#include <boost/thread/xtime.hpp>
#endif

# include <climits>
# include <unistd.h>
# include <cerrno>

# include <sys/mman.h>
# include <sys/types.h> // should be replaced by boost::filesystem - ptr
# include <sys/stat.h>

# include <iostream>
# include <fstream>

# include <boost/filesystem/operations.hpp>
# include <boost/filesystem/path.hpp>

# include "../redir/redir.h"
# include "../redir/uuidP.h"

#ifndef HOST_NAME_MAX 
#define HOST_NAME_MAX 255
#endif

#endif // __unix || N_PLAT_NLM

#ifdef __unix
#  include <libgen.h>
#endif

#ifdef N_PLAT_NLM
# include <nunicode.h>
# include <nwnspace.h>
# include "std/conv/wcharcrt.h"
# include <mt/xmt.h>
# include <nwtime.h>
int counter_uniq=1;
# include "../redir/redir.h"
# include "../redir/uuidP.h"
#include <nlm\nit\nwenvrn.h>
#include "openssl/md5.h"

#endif

#include <avp/text2bin.h>
#include "common/measurer.h"

#ifndef N_PLAT_NLM
 #include "openssl/md5.h"
#endif


#ifdef _WIN32

#ifndef KLCSC_STATIC

#ifdef DEBUG
	#pragma comment(lib, "../openssl/lib/debug/ssleay32.lib")
	#pragma comment(lib, "../openssl/lib/debug/libeay32.lib")		
#else
	#pragma comment(lib, "../openssl/lib/release/ssleay32.lib")
	#pragma comment(lib, "../openssl/lib/release/libeay32.lib")	
#endif //DEBUG

#endif //KLCSC_STATIC

#endif //_WIN32





#ifndef _WIN32 

#include <std/klcspwd/strg_unix.h> // Windows has own protected storage
#include <std/conf/errors.h> // Registry is used instead of conf files under Windows

#endif

/*
	BEGIN DEPENDENCIES
	THIS MODULE DEPENDS ON FOLOWING MODULES:
		KLERR
	END DEPENDENCIES
*/

#define KLCS_MODULENAME L"KLSTD"

#ifdef _WIN32
	#include <crtdbg.h>
#endif

#include <std/thr/thr_callbacks.h>

using namespace KLSTD;
using namespace KLERR;
using namespace std;

void KLSTD_InitRandom();

namespace KLERR
{
	void InitModule();
	void DeinitModule();
};

static std::wstring g_wstrDefaultProduct=KLCS_PRODUCT_ADMINKIT;
static std::wstring g_wstrDefaultVersion=KLCS_VERSION_ADMINKIT;

#ifdef _DEBUG
	KLSTD_CLASS_DBG_INFO_LIST_TYPE __g_ListOfAllocatedClassesWithRefCounter__;

#ifdef _WIN32
	static CRITICAL_SECTION __g_csListOfAllocatedClassesWithRefCounterLock__;
	#define LOCK_LISTOFALLOCATEDCLASSESWITHREFCOUNTER ::EnterCriticalSection(&__g_csListOfAllocatedClassesWithRefCounterLock__)
	#define UNLOCK_LISTOFALLOCATEDCLASSESWITHREFCOUNTER ::LeaveCriticalSection(&__g_csListOfAllocatedClassesWithRefCounterLock__)
#else
	#error "not implemented"
#endif

#endif

static bool g_bShutdownFlag = false;

KLCSC_DECL void KLSTD_SetShutdownFlag()
{
    g_bShutdownFlag = true;
};


KLCSC_DECL void KLSTD_ClearShutdownFlag()
{
    g_bShutdownFlag = false;
};

KLCSC_DECL bool KLSTD_GetShutdownFlag()
{
    return g_bShutdownFlag;
}

#ifdef _WIN32

static HMODULE g_hDllModule = NULL;

KLCSC_DECL void* KLSTD_GetModuleInstance()
{
    return g_hDllModule;
};


#ifndef KLCSC_STATIC

BOOL WINAPI DllMain(
  HINSTANCE hinstDLL,   // handle to the DLL module
  DWORD fdwReason,      // reason for calling function
  LPVOID lpvReserved)   // reserved
{
    switch(fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        g_hDllModule = hinstDLL;
        DisableThreadLibraryCalls(hinstDLL);
        break;
    };
    return TRUE;
};

#endif //KLCSC_STATIC

#endif //_WIN32

namespace KLSTD
{
    KLSTD::Reporter*    g_pReporter = NULL;
    KLSTD::ObjectLock*  g_plckReporter = NULL;
    KLSTD::CAutoPtr<KLSTD::CriticalSection> g_pSysTickCountCS;
    terminate_handler_t g_pTerminatePtr;
};

KLCSC_DECL void KLSTD_SetTerminateCallback(KLSTD::terminate_handler_t pHandler)
{
    KLSTD::g_pTerminatePtr = pHandler;
};

#ifndef _WIN32 //unix and novell only, windows has it's own protected storage

namespace KLCSPWD

{

    KLCSPWD::ProtectedStorage* g_pProtectedStorage = NULL;

    KLSTD::ObjectLock*       g_plckPwdStg = NULL;

};

#endif

KLCSC_DECL KLSTD::terminate_handler_t KLSTD_GetTerminateCallback()
{
    return KLSTD::g_pTerminatePtr;
};

namespace KLSTDSIGN
{
    void DoInitialize();
    void DoDeinitialize();
};

IMPLEMENT_MODULE_INIT_DEINIT(KLSTD)
IMPLEMENT_MODULE_LOCK(KLSTD)
IMPLEMENT_MODULE_LOCK(KLSTD_ATOMIC)

IMPLEMENT_MODULE_INIT_BEGIN(KLSTD)

#ifdef _WIN32
    set_terminate(KLSTD::g_pTerminatePtr);
    set_unexpected(&terminate);
#endif

	KLSTD_InitThreadRegistration();

    MODULE_LOCK_INIT(KLSTD)
    MODULE_LOCK_INIT(KLSTD_ATOMIC)
    g_bShutdownFlag = false;
    g_pSysTickCountCS = NULL;
    KLSTD_CreateCriticalSection(&g_pSysTickCountCS);
#if defined (__unix) || defined (N_PLAT_NLM)//#ifndef _WIN32
	KLSTD_InitAlternativeFileLocker();
#endif

#ifdef _DEBUG
#ifdef _WIN32
	::InitializeCriticalSection(&__g_csListOfAllocatedClassesWithRefCounterLock__);
#else
	#error "not implemented"
#endif
#endif

#ifdef N_PLAT_NLM
	// swith on long name support
	BYTE oldNameSpace = SetCurrentNameSpace( 4 );
#endif
#if defined(_WIN32) && defined(KLUSEFSSYNC)
    {
		RegisterThreadInDriver();
    };
#endif
    KLSTD::initTmpHeap();
	KLSTRT::InitStubRouter();
	KLERR::InitModule();
	KLERR_InitModuleLocalizationDefaults(
		KLCS_MODULENAME,
		KLSTD::c_LocErrDescriptions,
		KLSTD_COUNTOF(KLSTD::c_LocErrDescriptions));

	KLERR_InitModuleDescriptions(
		KLCS_MODULENAME,
		KLSTD::c_errorDescriptions,
		KLSTD_COUNTOF(KLSTD::c_errorDescriptions));
#ifndef _WIN32
	KLERR_InitModuleDescriptions(
		L"KLSTDCONF",
		KLSTDCONF::c_errorDescriptions,
		KLSTD_COUNTOF(KLSTDCONF::c_errorDescriptions));    
#endif
	KLSTD_InitRandom();
    KLSTD_InitializeGlobalThreadStore();
    KLSTD_InitializeThreads();
    ;
    g_pReporter = NULL;
    g_plckReporter = new KLSTD::ObjectLock;
    if(g_plckReporter)
        g_plckReporter->Allow();

#ifndef _WIN32
    KLCSPWD::g_pProtectedStorage = NULL;
    KLCSPWD::g_plckPwdStg = new KLSTD::ObjectLock;
    if(KLCSPWD::g_plckPwdStg)
        KLCSPWD::g_plckPwdStg->Allow();
#endif

#if defined(__unix)
    uuid_initialize();
#endif //__unix
        KLERR_BEGIN
            std::wstring wstrHost, wstrDomain;
            KLSTD::KlDomainType type = KLSTD::KLDT_WIN_DOMAIN;
            KLSTD_GetHostAndDomainName(wstrHost, &wstrDomain, &type);
            KLSTD_TRACE3(   1, 
                            L"\n\nCurrent host info:\n"
                            L"\tNetBIOS name: '%ls'\n"
                            L"\tDomain name: '%ls'\n"
                            L"\tDomain type: '%ls'\n\n",
                            wstrHost.c_str(),
                            wstrDomain.c_str(),
                            (type == KLSTD::KLDT_WIN_DOMAIN)
                                ?   L"domain"
                                :   L"workgroup");
#ifdef _WIN32
            DWORD dwDefAcp = GetACP();
            LCID nDefLCID = GetSystemDefaultLCID();
            LANGID nDefLANGID = GetSystemDefaultLangID();
            KLSTD_TRACE3(   1, 
                            L"Default codepage: %u, "
                            L"Default LCID: %u, "
                            L"Default LangID: %u\n", dwDefAcp, nDefLCID, nDefLANGID);
#endif
        KLERR_ENDT(1)

#ifndef KLCSC_STATIC
    KLSTDSIGN::DoInitialize();
#endif

IMPLEMENT_MODULE_INIT_END()

IMPLEMENT_MODULE_DEINIT_BEGIN(KLSTD)

#ifndef KLCSC_STATIC
    KLSTDSIGN::DoDeinitialize();
#endif

#if defined(__unix)
    uuid_deinitialize();
#endif //__unix

    if(g_plckReporter)
    {
        g_plckReporter->Disallow();
        g_plckReporter->Wait();
    };
    g_pReporter = NULL;
    delete g_plckReporter;
    g_plckReporter = NULL;

#ifndef _WIN32
    if(KLCSPWD::g_plckPwdStg)
    {
        KLCSPWD::g_plckPwdStg->Disallow();
        KLCSPWD::g_plckPwdStg->Wait();
    };
    KLCSPWD::g_pProtectedStorage = NULL;
    delete KLCSPWD::g_plckPwdStg;
    KLCSPWD::g_plckPwdStg = NULL;
#endif
    ;
    MODULE_LOCK_DEINIT(KLSTD);
    MODULE_LOCK_DEINIT(KLSTD_ATOMIC);
    KLSTD_DeinitializeThreads();
    KLSTD_DeinitializeGlobalThreadStore();
#ifndef _WIN32
	KLERR_DeinitModuleDescriptions(L"KLSTDCONF");
#endif
	KLERR_DeinitModuleDescriptions(KLCS_MODULENAME);
    KLERR_DeinitModuleLocalizationDefaults(KLCS_MODULENAME);
	KLERR::DeinitModule();
	KLSTRT::DeinitStubRouter();
    KLSTD::deinitTmpHeap();
    g_pSysTickCountCS = NULL;
#if defined(_WIN32) && defined(KLUSEFSSYNC)
	UnregisterThreadInDriver();
#endif

#ifdef _DEBUG
	LOCK_LISTOFALLOCATEDCLASSESWITHREFCOUNTER;
	
    KLERR_BEGIN
	{
		KLSTD_TRACE0(1, L"[KLSTD_CLASS_DBG_INFO] Lost objects dump start ================================\n");
		for(KLSTD_CLASS_DBG_INFO_LIST_TYPE::iterator i = __g_ListOfAllocatedClassesWithRefCounter__.begin();i != __g_ListOfAllocatedClassesWithRefCounter__.end();i++)
			KLSTD_TRACE2(1, L"[KLSTD_CLASS_DBG_INFO] Lost object: %s (Address: 0x%X)\n", (*i).m_sClassName.c_str(), (*i).m_pClassPointer);
		KLSTD_TRACE0(1, L"[KLSTD_CLASS_DBG_INFO] Lost objects dump end ==================================\n");
	}
	KLERR_END

	UNLOCK_LISTOFALLOCATEDCLASSESWITHREFCOUNTER;

#ifdef _WIN32
	::DeleteCriticalSection(&__g_csListOfAllocatedClassesWithRefCounterLock__);
#else
	#error "not implemented"
#endif

#if defined (__unix) || defined (N_PLAT_NLM)//#ifndef _WIN32
	KLSTD_DeInitAlternativeFileLocker();
#endif

#endif
    g_pSysTickCountCS = NULL;

	KLSTD_DeInitThreadRegistration();

#ifdef _WIN32
    KLSTD::g_pTerminatePtr = NULL;
    set_terminate(NULL);
    set_unexpected(NULL);
#endif

IMPLEMENT_MODULE_DEINIT_END()

KLCSC_DECL void KLSTD_GetModuleLock(KLSTD::CriticalSection** ppCS)
{
    KLSTD_CHKOUTPTR(ppCS);
    g_cKLSTD_Lock.CopyTo(ppCS);
}

KLCSC_DECL void KLSTD_GetAtomicLock(KLSTD::CriticalSection** ppCS)
{
    KLSTD_CHKOUTPTR(ppCS);
    g_cKLSTD_ATOMIC_Lock.CopyTo(ppCS);
}

KLCSC_DECL void KLSTD_SetDefaultProductAndVersion(
                                            const std::wstring& wstrProduct,
                                            const std::wstring& wstrVersion)
{
    KLCS_MODULE_LOCK(KLSTD, acs);
    g_wstrDefaultProduct=wstrProduct;
    g_wstrDefaultVersion=wstrVersion;
};

KLCSC_DECL void KLSTD_GetDefaultProductAndVersion(
                                 std::wstring& wstrProduct,
                                 std::wstring& wstrVersion)
{
    KLCS_MODULE_LOCK(KLSTD, acs);
    wstrProduct=g_wstrDefaultProduct;
    wstrVersion=g_wstrDefaultVersion;
};

KLCSC_DECL void KLSTD_ThrowErrnoCode(
		int				nCode,
		const char*		szaFile,
		int				nLine)
{
	int code=STDE_SYSTEM;
	switch(nCode){
	case EPERM:
		code=STDE_NOTPERM;
		break;
	case ENOENT:
		code=STDE_NOENT;
		break;
	case EIO:
		code=STDE_IO;
		break;
	case ENXIO:
		code=STDE_NXIO;
		break;
	case ENOMEM:
		code=STDE_NOMEMORY;
		break;
	case EACCES:
		code=STDE_NOACCESS;
		break;
	case EFAULT:
		code=STDE_FAULT;
		break;
	case EEXIST:
		code=STDE_EXIST;
		break;
	case ENODEV:
		code=STDE_NODEV;
		break;
	case ENOTDIR:
		code=STDE_NOTDIR;
		break;
	case EISDIR:
		code=STDE_ISDIR;
		break;
	case EINVAL:
		code=STDE_BADPARAM;
		break;
	case ENFILE:
		code=STDE_NFILE;
		break;
	case EMFILE:
		code=STDE_MFILE;
		break;
	case ENOTTY:
		code=STDE_NOTTY;
		break;
	case EFBIG:
		code=STDE_TOOBIG;
		break;
	case ENOSPC:
		code=STDE_NOSPC;
		break;
	case ESPIPE:
		code=STDE_SPIPE;
		break;
	case EROFS:
		code=STDE_ROFS;
		break;
	case ENAMETOOLONG:
		code=STDE_NAMETOOLONG;
		break;
	case ENOTEMPTY:
		code=STDE_NOTEMPTY;
		break;
	};
	if(code==STDE_SYSTEM){
		KLSTD_USES_CONVERSION;
		const char* str=strerror(nCode);
		if(str)
			::KLERR_throwError(KLCS_MODULENAME, STDE_SYSTEM, szaFile, nLine, NULL, nCode, KLSTD_A2W(str));
		else
			::KLERR_throwError(KLCS_MODULENAME, STDE_SYSTEM, szaFile, nLine, NULL, nCode, L"errno");
	}
	else
		::KLERR_throwError(KLCS_MODULENAME, code, szaFile, nLine, NULL,   NULL, NULL, NULL, NULL);
};


namespace
{
    void GetErrorString(long nCode, std::wstring& wstrResult)
    {		
        wstrResult.clear();
#ifdef _WIN32
        std::vector<TCHAR> vecBuffer;
        vecBuffer.resize(MAX_PATH+1);
        
	if(FormatMessage(
			    FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_FROM_SYSTEM,
			    NULL,
			    nCode,
			    0, 
			    &vecBuffer[0],
			    MAX_PATH,
			    NULL))
        {
            wstrResult = (const wchar_t*)KLSTD_T2W2(&vecBuffer[0]);
        };
        //remove \r\n
        if( wstrResult.size() > 2 && 
            wstrResult[wstrResult.size() -2] == _T('\r') && 
            wstrResult[wstrResult.size() -1] == _T('\n'))
        {
            wstrResult.resize(wstrResult.size() - 2);
        };
#else
        const char* str = strerror(nCode);
        if(str)
        {
            wstrResult = (const wchar_t*)KLSTD_A2CW2(str);
        };
#endif
        if(wstrResult.empty())
        {
            std::wostringstream os;
            os << L"System error " << unsigned(nCode) << L" occured";
            wstrResult = os.str();
        };
    };
};

	KLCSC_DECL void KLSTD_GetSysErrorString(long nCode, std::wstring& wstrResult)
    {
		GetErrorString( nCode, wstrResult );
	}

	static long LastError2StdError(long nCode)
	{
#ifdef WIN32
		switch(nCode){ // winerror.h
		case NO_ERROR:
			return STDE_GENERAL;
		case ERROR_FILE_NOT_FOUND:
			return STDE_NOTFOUND;
		case E_FAIL:
			return STDE_GENERAL;
		case ERROR_NOT_ENOUGH_MEMORY:
			return STDE_NOMEMORY;
		case ERROR_ACCESS_DENIED:
			return STDE_NOACCESS;
		case ERROR_INVALID_PARAMETER:
			return STDE_BADPARAM;
		case ERROR_INVALID_HANDLE:
			return STDE_BADHANDLE;
		case ERROR_MORE_DATA:
			return STDE_MOREDATA;
		case ERROR_INVALID_FUNCTION:
			return STDE_NOFUNC;
		case ERROR_NOACCESS:
			return STDE_FAULT;
		case WAIT_TIMEOUT:
			return STDE_TIMEOUT;
		case ERROR_PATH_NOT_FOUND:
			return STDE_NOENT;
		case ERROR_IO_DEVICE:
			return STDE_IO;
		case ERROR_FILE_EXISTS:
		case ERROR_ALREADY_EXISTS:
			return STDE_EXIST;
		case ERROR_BAD_UNIT:
			return STDE_NODEV;
		case ERROR_DIRECTORY:
			return STDE_NOTDIR;
		case ERROR_TOO_MANY_OPEN_FILES:
			return STDE_MFILE;
		case ERROR_OUTOFMEMORY:
			return STDE_NOSPC;
		case ERROR_BUFFER_OVERFLOW:
			return STDE_NAMETOOLONG;
		case ERROR_DIR_NOT_EMPTY:
			return STDE_NOTEMPTY;
		default:
			return STDE_SYSTEM;
		};
#endif
#if defined(__unix) || defined(N_PLAT_NLM)
		switch(nCode){ // winerror.h
		case 0:
			return STDE_GENERAL;
		case ENOENT:
			return STDE_NOTFOUND;
		case ENOMEM:
			return STDE_NOMEMORY;
		case EACCES:
			return STDE_NOACCESS;			
		case EINVAL:
		case E2BIG:
			return STDE_BADPARAM;
		case EBADF:
			return STDE_BADHANDLE;
		case EFAULT:
			return STDE_FAULT;
		case ETIMEDOUT:
			return STDE_TIMEOUT;		
		case EIO:
			return STDE_IO;
		case EEXIST:
			return STDE_EXIST;
		case ENODEV:
			return STDE_NODEV;
		case ENOTDIR:
			return STDE_NOTDIR;
		case EMFILE:
			return STDE_MFILE;			
		case ENOSPC:
			return STDE_NOSPC;
		case ENOTEMPTY:
			return STDE_NOTEMPTY;
		default:
			return STDE_SYSTEM;
		};
#endif 
	};

	KLCSC_DECL void KLSTD_ThrowLastError(
			const wchar_t*	szwMessage,
			const char*		szaFile,
			int				nLine,
			bool			bTryConvert)
	{
#ifdef WIN32
		KLSTD_ThrowLastErrorCode(szwMessage, GetLastError(), true, szaFile, nLine, bTryConvert);
#else
		KLSTD_ThrowLastErrorCode(szwMessage, errno, true, szaFile, nLine, bTryConvert);
#endif
	};


	KLCSC_DECL void KLSTD_ThrowLastErrorCode(
			const wchar_t*	szwMessage,
			long			nCode,
			bool			bThrowAlways,
			const char*		szaFile,
			int				nLine,
			bool			bTryConvert)
	{
		if(!bThrowAlways && nCode==0)
			return;
		long code=bTryConvert?LastError2StdError(nCode):STDE_SYSTEM;
		if(code==STDE_SYSTEM)
		{// Ошибку расшифровать не удалось
			std::wstring wstrMessage;
			if(szwMessage)
				wstrMessage=szwMessage;
			else
			{
				GetErrorString(nCode, wstrMessage);
			};
			CAutoPtr<Error> pError;
			KLERR_CreateError(
				&pError,
				KLCS_MODULENAME,
				STDE_SYSTEM,
				szaFile,
				nLine,
				(wstrMessage.empty()
                       ?   (L"?")
                       :   (wstrMessage.c_str())));
			pError->SetErrorSubcode(nCode);
			throw pError.Detach();
		}
		else
        {
            std::wstring wstrString;
            GetErrorString(nCode, wstrString);
            ::KLERR_throwError(
                        KLCS_MODULENAME, 
                        code, 
                        szaFile, 
                        nLine, 
                        wstrString.c_str(), 
                        NULL, 
                        NULL, 
                        NULL, 
                        NULL);
        };
	};




/*  andkaz: random number generator may be used either storing own 

    state for each thread or using lock

*/

namespace

{

#if defined(__unix) && defined(USE_BOOST_THREADS)

    boost::mutex    g_rnd_mutex;

    bool            g_bRandInitialized = false;

    boost::mutex    g_guid_mutex;

#endif

};


void KLSTD_InitRandom()
{

#if defined(_WIN32) || defined(N_PLAT_NLM)
	srand( KLSTD_GetCurrentThreadId() * GetSysTickCount() + time(NULL));

#elif defined(__unix) && defined(USE_BOOST_THREADS)

    boost::detail::thread::lock_ops<boost::mutex>::lock( g_rnd_mutex );

    if(!g_bRandInitialized)

    {

        long int nSeed = 0; 

        int nWasRead = 0;

        {

            int fd = open("/dev/urandom", O_RDONLY);

		    if (fd == -1)

			    fd = open("/dev/random", O_RDONLY | O_NONBLOCK);

            nWasRead = read(fd, &nSeed, sizeof(nSeed));

            if(fd != -1)

                close(fd);

        };

        if(nWasRead == sizeof(nSeed))

            srand48(KLSTD_GetCurrentThreadId() * GetSysTickCount() + time(NULL) + nSeed);

        else

            srand48(KLSTD_GetCurrentThreadId() * GetSysTickCount() + time(NULL));

        g_bRandInitialized  = true;

    };

    boost::detail::thread::lock_ops<boost::mutex>::unlock( g_rnd_mutex );

#else

    #error "Not implemented"

#endif
}

int KLSTD_Random(int nMin, int nMax)
{
	int nResult;
#if defined(_WIN32) || defined(N_PLAT_NLM)

    nResult = int(AVP_longlong(rand())*AVP_longlong(nMax-nMin)/AVP_longlong(RAND_MAX) + nMin);

#elif defined(__unix) && defined(USE_BOOST_THREADS)   

    boost::detail::thread::lock_ops<boost::mutex>::lock( g_rnd_mutex );

    // The lrand48() and nrand48() functions shall return non-negative, 

    // long integers, uniformly distributed over the interval [0,2**31). 

    // i.e. 0..2147483647

    nResult = int(AVP_longlong(lrand48())*AVP_longlong(nMax-nMin+1) / 
                    AVP_longlong(2147483647) + nMin);

    boost::detail::thread::lock_ops<boost::mutex>::unlock( g_rnd_mutex );

#else

    #error "Not implemented"

#endif  
    return nResult;
}

namespace KLSTD
{
	class CMemoryChunk: public KLBaseImpl<MemoryChunk>
	{
	public:
		CMemoryChunk(size_t nSize)
		{
			m_pData=malloc(nSize);			
			KLSTD_CHKMEM(m_pData);
			m_nData=nSize;
		};
		virtual ~CMemoryChunk()
		{
			if(m_pData)
				free(m_pData);
		};
		void*	GetDataPtr()
		{
			return m_pData;
		};
		size_t	GetDataSize() const 
		{
			return m_nData;
		};
	protected:
		void*	m_pData;
		size_t	m_nData;
	};

	KLCSC_DECL void assertion_check(bool bOK, const char* szaString, const char* szaFile, int nLine)
	{
		if(!bOK)
        {
#if defined(_WIN32) && defined(_DEBUG)
            while(1 == _CrtDbgReport(_CRT_ASSERT, szaFile, nLine, NULL, "%s", szaString))
                __asm int 3;
#endif
            KLERR_throwError(L"KLSTD", KLSTD::STDE_ASSRT, szaFile, nLine, NULL, szaString);
        };
    };
};

KLCSC_DECL void KLSTD_AllocMemoryChunk(size_t nSize, KLSTD::MemoryChunk** ppChunk)
{
	KLSTD_CHKOUTPTR(ppChunk);
	*ppChunk=NULL;
	*ppChunk= new CMemoryChunk(nSize);
    KLSTD_CHKMEM(*ppChunk);
};

#ifdef _WIN32
	#pragma comment(lib, "Rpcrt4.lib")
#endif

namespace KLSTD
{

	static volatile long g_nGuid1 = 0;

    static void KLSTD_UuidCreate(UUID& uuid)
    {
        KLSTD_ZEROSTRUCT(uuid);
 
#if defined(_WIN32)
		int nResult=UuidCreate(&uuid);
		if( nResult != RPC_S_OK &&
            nResult != RPC_S_UUID_LOCAL_ONLY)

#elif defined(__unix)



        DWORD dwC2 = ::KLSTD_GetCurrentProcessId();

        DWORD dwC3 = ::KLSTD_GetCurrentThreadId();

        DWORD dwC5 = KLSTD_InterlockedIncrement(&g_nGuid1);



        UUID uuidTemp;

        

        boost::detail::thread::lock_ops<boost::mutex>::lock( g_guid_mutex );

        uuid_generate(uuidTemp);

        boost::detail::thread::lock_ops<boost::mutex>::unlock( g_guid_mutex  );



        MD5_CTX ctxMd5;

        MD5_Init(&ctxMd5);

        MD5_Update(&ctxMd5, &dwC2,  sizeof(dwC2));

        MD5_Update(&ctxMd5, &dwC3,  sizeof(dwC3));

        MD5_Update(&ctxMd5, &dwC5,  sizeof(dwC5));

        MD5_Update(&ctxMd5, &uuid,  sizeof(uuid));

        unsigned char digest[MD5_DIGEST_LENGTH];

        KLSTD_ZEROSTRUCT(digest);

        MD5_Final(digest, &ctxMd5);



        memcpy(&uuid, &digest[0], min(sizeof(uuid), sizeof(digest)));

        if(0)

#endif
        {
            //if we failed try to create smth guid-like by ourselves
            DWORD dwC1 = KLSTD::GetSysTickCount();
            DWORD dwC2 = ::KLSTD_GetCurrentProcessId();
            DWORD dwC3 = ::KLSTD_GetCurrentThreadId();
            DWORD dwC4 = ::rand();
            DWORD dwC5 = KLSTD_InterlockedIncrement(&g_nGuid1);

            MD5_CTX ctxMd5;
            MD5_Init(&ctxMd5);
            MD5_Update(&ctxMd5, &dwC1,  sizeof(dwC1));
            MD5_Update(&ctxMd5, &dwC2,  sizeof(dwC2));
            MD5_Update(&ctxMd5, &dwC3,  sizeof(dwC3));
            MD5_Update(&ctxMd5, &dwC4,  sizeof(dwC4));
            MD5_Update(&ctxMd5, &dwC5,  sizeof(dwC5));

#ifdef _WIN32
			FILETIME ft;
			GetSystemTimeAsFileTime(&ft);
            TCHAR szComputerNameBuffer[MAX_COMPUTERNAME_LENGTH + 1];
            KLSTD_ZEROSTRUCT(szComputerNameBuffer);
            DWORD dwComputerNameBuffer = MAX_COMPUTERNAME_LENGTH + 1;
            GetComputerName(szComputerNameBuffer, &dwComputerNameBuffer);
            const size_t c_nComputerNameSize = 
                        _tcslen(szComputerNameBuffer)*
                            sizeof(szComputerNameBuffer[0]);
#elif defined(N_PLAT_NLM)
			time_t ft =time(NULL);
            //__impl::Thread::gettime( &ft );
         	 char 	szComputerNameBuffer[48]="";
 			GetFileServerName(0,szComputerNameBuffer);
            const size_t c_nComputerNameSize = 
                        strlen(szComputerNameBuffer)*
                            sizeof(szComputerNameBuffer[0]);
#else
            time_t ft =time(NULL);
            char szComputerNameBuffer[HOST_NAME_MAX+1] = "";
            gethostname(szComputerNameBuffer, sizeof(szComputerNameBuffer));
            const size_t c_nComputerNameSize = 
                        strlen(szComputerNameBuffer)*
                            sizeof(szComputerNameBuffer[0]);
#endif
            MD5_Update(&ctxMd5, &ft, sizeof(ft));
            MD5_Update( &ctxMd5, 
                        &szComputerNameBuffer[0], 
                        c_nComputerNameSize);
            unsigned char digest[MD5_DIGEST_LENGTH];
            KLSTD_ZEROSTRUCT(digest);
            MD5_Final(digest, &ctxMd5);            
            memcpy(&uuid, &digest[0], min(sizeof(uuid), sizeof(digest)));
        };
    };

	std::wstring CreateLocallyUniqueString(const char* szaPostfix)
	{
		KLSTD_USES_CONVERSION;
		::UUID uuid;
        KLSTD_UuidCreate(uuid);
		char text[B2T_LENGTH(sizeof(uuid)) + 1];
		BinToText(&uuid, sizeof(uuid), text, sizeof(text));
		text[sizeof(text)-1]=0;
		for(char* ptr=strchr(text, '/'); ptr; ptr=strchr(ptr, '/'))
			*(ptr++)='_';
		if(!szaPostfix || !szaPostfix[0])
			return KLSTD_A2CW(text);
		int nBuffer=sizeof(wchar_t)*(strlen(text)+strlen(szaPostfix)+5);
		wchar_t* pBuffer=(wchar_t*)alloca(nBuffer);
		KLSTD_SWPRINTF(pBuffer, nBuffer, L"%hs-%hs", text, szaPostfix);
		return pBuffer;
    };
};

KLCSC_DECL std::wstring KLSTD_CreateLocallyUniqueString()
{
	std::wstring result = KLSTD::CreateLocallyUniqueString(NULL);
	while( result[0]==L'_' )	// нельзя использовать строки начинающиеся с символа '_'
		result = KLSTD::CreateLocallyUniqueString(NULL);
	
	return result;
};

std::wstring KLSTD_CreateGUIDString()
{
	KLSTD_USES_CONVERSION;
	std::wstring rc;    
	::UUID uuid;
	KLSTD_ZEROSTRUCT(uuid);    
    KLSTD_UuidCreate(uuid);
#ifdef _WIN32
	_TUCHAR* pszResult = NULL;
	RPC_STATUS nStatus;
    nStatus = UuidToString(&uuid, &pszResult);
    if(nStatus != RPC_S_OK)
    {
        KLSTD_THROW_LASTERROR_CODE2(nStatus);
    };
    rc = KLSTD_T2CW((TCHAR*)pszResult);
    RpcStringFree(&pszResult);
	return rc;
#else
	return KLSTD::MakeHexDataW(&uuid, sizeof(uuid));
#endif	
}


/*!
  \brief	Creates globally unique string suitable both for case sensetive 
            and case insensetive comparison 

  \param	pwstrResult [out] resulting string 
*/
KLCSC_DECL void KLSTD_CreateGloballyUniqueString(
                                        KLSTD::AKWSTR* pwstrResult)
{
    KLSTD_CHKOUTPTR(pwstrResult);
    KLSTD::klwstr_t klstrResult = KLSTD_CreateGUIDString().c_str();
    *pwstrResult = klstrResult.detach();
};


#ifdef __unix
	wchar_t *_ltow( long value, wchar_t *s, int radix )
	{
      std::wstringstream st;
      switch ( radix ) {
        case 8:
          st.setf( std::ios_base::oct );
          break;
        case 10:
          st.setf( std::ios_base::dec );
          break;
        case 16:
          st.setf( std::ios_base::hex );
          break;
        default:
          KLSTD_ASSERT(false);
          break;
      }
      st << value;
      const std::wstring& str = st.str();
	  wcscpy( s, str.c_str() );
      return s;
	}

	char *_ltoa( long value, char *s, int radix )
	{
      std::stringstream st;
      switch ( radix ) {
        case 8:
          st.setf( std::ios_base::oct );
          break;
        case 10:
          st.setf( std::ios_base::dec );
          break;
        case 16:
          st.setf( std::ios_base::hex );
          break;
        default:
          KLSTD_ASSERT(false);
          break;
      }
      st << value;
      const std::string& str = st.str();
	  strcpy( s, str.c_str() );
      return s;
	}
	
//	AVP_longlong _wtoi64(const wchar_t* psz)
//	{
//		#error "not implemented"
//	}
#endif

#if defined(_WIN32)
	int KLCSC_DECL KLSTD_CalcHeap(
						int&	nUsed,
						int&	nUsedSize,
						int&	nFree,
						int&	nFreeSize,
						int&	nMaximumUsedSize)
	{
		nUsed=nUsedSize=nFree=nFreeSize=nMaximumUsedSize=0;
		_HEAPINFO hinfo={0};
		int heapstatus;
		while( ( heapstatus = _heapwalk( &hinfo ) ) == _HEAPOK )
		{
			if(hinfo._useflag == _USEDENTRY)
			{
				++nUsed;
				nUsedSize+=hinfo._size;
				if(hinfo._size > (unsigned)nMaximumUsedSize)
					nMaximumUsedSize=hinfo._size;
			}
			else if(hinfo._useflag == _FREEENTRY)
			{
				++nFree;
				nFreeSize+=hinfo._size;
			}
			else
				KLSTD_ASSERT(false);
		};
		return heapstatus;
	};

	void KLCSC_DECL DumpHeapData()
	{
		int	nUsed,
			nUsedSize,
			nFree,
			nFreeSize,
			nMaximumUsedSize;

		KLSTD_CalcHeap(nUsed, nUsedSize, nFree, nFreeSize, nMaximumUsedSize);
		wchar_t szwBuffer[256]=L"";
		swprintf(
			szwBuffer,
			L"Heap state:\n"
				L"\tFull size:\t%d bytes\n"	
				L"\tMax used block size:\t%d bytes\n"
				L"\tUsed blocks:\t%d\n"
				L"\tUsed size:\t%d bytes\n"				
				L"\tFree blocks:\t%d\n"
				L"\tFree size:\t%d bytes\n\n",
			nUsedSize+nFreeSize,
			nMaximumUsedSize,
			nUsed,
			nUsedSize,
			nFree,
			nFreeSize);
		OutputDebugStringW(szwBuffer);
	};
#endif

KLCSC_DECL void KLSTD_SplitString( const std::wstring & wstr,
                        const std::wstring & wstrDelim,
                        std::vector<std::wstring> & vectValues )
{
    vectValues.clear();

    if( wstr.size() && wstrDelim.size() )
    {
        std::wstring::size_type pos = 0;
        std::wstring::size_type posStart = 0;
        while( pos != wstring::npos )
        {
            pos = wstr.find( wstrDelim, posStart );
            std::wstring::const_pointer pStart = wstr.c_str() + posStart;
            if( pos == wstring::npos )
            {
                vectValues.push_back( wstring( pStart ) );
            } else
            {
                vectValues.push_back( wstring( pStart, pos - posStart ) );
                posStart = pos + wstrDelim.size();
            }
        }
    }
}

KLCSC_DECL std::wstring & KLSTD_TrimString( std::wstring & wstr )
{
    { // TODO: extract this code as KLSTD_TrimStringLeft(...)
        int nStartIndex = 0;
        for( ; (unsigned)nStartIndex < wstr.size(); nStartIndex++ )
        {
            if( ! iswspace( wstr[nStartIndex] ) )
            {
                break;
            }
        }
        if( nStartIndex > 0 )
        {
            wstr.erase( 0, nStartIndex );
        }
    }

    //<-- Changed by andkaz 27.07.2005 13:59:40
    if( !wstr.empty() )
    // -->
    { // TODO: extract this code as KLSTD_TrimStringRight(...)
        int nEndIndex = wstr.size() - 1;
        for( ; nEndIndex >= 0; nEndIndex-- )
        {
            if( ! iswspace( wstr[nEndIndex] ) )
            {
                break;
            }
        }
        if( (unsigned)nEndIndex < wstr.size() - 1 )
        {
            wstr.erase( nEndIndex + 1 );
        }
    }

    return wstr;
}


KLCSC_DECL void KLSTD_ThrowAppPending(const char* szaFile, int nLine)
{
    KLSTD::CAutoPtr<KLERR::Error> pError;
    KLERR_CreateLocError(
        ErrLocAdapt(
            STDEL_APP_PENDING,
            KLCS_MODULENAME),
        &pError,
        KLCS_MODULENAME,
        KLSTD::STDE_UNAVAIL,
        szaFile,
        nLine,
        NULL);
    throw pError.Detach();
};


#ifdef _DEBUG

namespace KLSTD {

__CKLStdClassDbgInfo__::__CKLStdClassDbgInfo__(const wchar_t* pwcClassName, void* pClassPointer)
{
	m_sClassName = pwcClassName?pwcClassName:L"";
	m_pClassPointer = pClassPointer;
}

};

KLCSC_DECL KLSTD_CLASS_DBG_INFO_LIST_TYPE::iterator KLSTD_CLASS_DBG_INFO_RegisterClass(const wchar_t* pwcClassName, void* pClassPointer)
{
	LOCK_LISTOFALLOCATEDCLASSESWITHREFCOUNTER;

	KLSTD_CLASS_DBG_INFO_LIST_TYPE::iterator rc = __g_ListOfAllocatedClassesWithRefCounter__.end();

	KLERR_BEGIN
	{
		rc = __g_ListOfAllocatedClassesWithRefCounter__.insert(__g_ListOfAllocatedClassesWithRefCounter__.end(), KLSTD::__CKLStdClassDbgInfo__(pwcClassName, pClassPointer));
	}
	KLERR_END

	UNLOCK_LISTOFALLOCATEDCLASSESWITHREFCOUNTER;

	return rc;
}

KLCSC_DECL void KLSTD_CLASS_DBG_INFO_UnregisterClass(KLSTD_CLASS_DBG_INFO_LIST_TYPE::iterator itGlobalListPosition)
{
	LOCK_LISTOFALLOCATEDCLASSESWITHREFCOUNTER;

	KLERR_BEGIN
	{
		if (itGlobalListPosition != __g_ListOfAllocatedClassesWithRefCounter__.end())
			__g_ListOfAllocatedClassesWithRefCounter__.erase(itGlobalListPosition);
	}
	KLERR_END

	UNLOCK_LISTOFALLOCATEDCLASSESWITHREFCOUNTER;
}

#endif
