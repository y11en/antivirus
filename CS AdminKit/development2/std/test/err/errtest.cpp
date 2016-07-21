#include "std/err/klerrors.h"
#include "errtest.h"
#include "avp/parray.h"

#include "std/trc/trace.h"
#include "std/conv/klconv.h"
#include "std/io/klio.h"
#include "std/thr/thread.h"
#include "std/time/kltime.h"
#include "testmodule.h"
#include "helpers.h"
#include <std/base/klstdutils.h>
#include <std/fmt/stdfmtstr.h>
#include <std/conf/std_conf.h>
#include <std/par/paramslogger.h>

#include <iostream>
#include <sstream>

using namespace KLERR;
using namespace KLSTD;

#define KLCS_MODULENAME L"ERR_TEST"

#if defined(_WIN32)
    #define KLSTDTST_TEST_PATH_START L"c:\\0F0BD01F"
    #define KLSTDTST_TEST_PATH L"\\2FF4\\460c\\92AA\\14EF83BD0BA2\\"
#elif defined(__unix)
    #define KLSTDTST_TEST_PATH_START L"./0F0BD01F"
    #define KLSTDTST_TEST_PATH L"/2FF4/460c/92AA/14EF83BD0BA2/"
#endif

static const ErrorDescription desc[]={
	{10401, L"Bye !"},
	{10403, L"Hello %ls %ls !!!!"}
};

static const ErrorDescription desc_loc[]={
	{1, L"Test1 %1 !"},
	{2, L"Test2 %1 %2 !!!!"}
};

void CErrorTest::Initialize(KLTST2::TestData2* pTestData)
{
	KLSTD_Initialize();	
	//KLPAR_Initialize();			
}

KLSTD_NOTHROW void CErrorTest::Deinitialize() throw()
{
	KLERR_BEGIN
		//KLPAR_Deinitialize();	

	    KLSTD_Deinitialize();
	KLERR_END
}

long CErrorTest::Run()
{
	bool bResult=true;
	KLERR_TRY
		::KLERR_InitModuleDescriptions(KLCS_MODULENAME, desc, KLSTD_COUNTOF(desc));
        ::KLERR_InitModuleLocalizationDefaults(KLCS_MODULENAME, desc_loc, KLSTD_COUNTOF(desc_loc));
        ;
        const wchar_t c_szwFormat[] = L"%1";
        const wchar_t* pars[] = {L"par1"};
        KLSTD::klwstr_t wstrRes;
        KLSTD_FormatMessage(c_szwFormat, pars, KLSTD_COUNTOF(pars),  wstrRes.outref());
        ;
        for(size_t i = 0; i < 2; ++i)
        {
            int id = 0;
            int locid = 0;
        KLERR_TRY
            switch(i)
            {
            case 0:
                {
                KLERR_LOCMYTHROW0(
                        ErrLocAdapt((locid = 1), KLCS_MODULENAME, L"locpar1"),
                        (id = 10401));
                };
                break;
            case 1:
                {
                KLERR_LOCMYTHROW2(
                        ErrLocAdapt((locid = 2), KLCS_MODULENAME, L"locpar1", L"locpar2"),
                        (id = 10401),
                        L"par1",
                        L"par2");
                };
                break;
            };
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(3, pError);
            if(!KLERR_IsErrorLocalized(pError))
            {
                KLERR_RETHROW();
            };
            if(id != pError->GetId())
            {
                KLERR_RETHROW();
            };
            KLSTD::klwstr_t wstrTest;
            KLERR_LocFormatErrorString(pError, wstrTest.outref());
            if(wstrTest.empty())
            {
                KLERR_RETHROW();
            };
        KLERR_ENDTRY
        };
	KLERR_CATCH(pError)
		KLERR_SAY_FAILURE(1, pError);
	KLERR_FINALLY
		::KLERR_DeinitModuleDescriptions(KLCS_MODULENAME);
        ::KLERR_DeinitModuleLocalizationDefaults(KLCS_MODULENAME);
        KLERR_RETHROW();
    KLERR_ENDTRY
    return 0;
};

void CStdTest::Initialize(KLTST2::TestData2* pTestData)
{
	KLSTD_Initialize();
	KLPAR_Initialize();		
}

KLSTD_NOTHROW void CStdTest::Deinitialize() throw()
{
	KLERR_BEGIN
		KLPAR_Deinitialize();	
	KLSTD_Deinitialize();
	KLERR_END
}

long CStdTest::Run()
{
	bool bResult=true;
    KLSTD_USES_CONVERSION;
	KLERR_TRY
        {
            const std::wstring& wstrGuid1 = KLSTD_CreateGUIDString();
            const std::wstring& wstrGuid2 = KLSTD_CreateGUIDString();
            if(wstrGuid1.size() < 32 || wstrGuid2.size() < 32)
            {
                KLSTD_THROW(KLSTD::STDE_GENERAL);
            };
            if(wstrGuid1 == wstrGuid2)
            {
                KLSTD_THROW(KLSTD::STDE_GENERAL);
            };
        };
        {
            std::wstring wstrFile1, wstrFile2;
            KLSTD_GetTempFile(wstrFile1);
            KLSTD_GetTempFile(wstrFile2);

            if( wstrFile1.empty() || wstrFile2.empty() )
            {
                KLSTD_THROW(KLSTD::STDE_GENERAL);
            };
            if(wstrFile1 == wstrFile2)
            {
                KLSTD_THROW(KLSTD::STDE_GENERAL);
            };
        };
        {
            std::wostringstream osPathStart;
            osPathStart << KLSTDTST_TEST_PATH_START << L"-" << (unsigned)KLSTD_GetCurrentThreadId();

            std::wostringstream os; 
            os << osPathStart.str() << KLSTDTST_TEST_PATH;
            KLSTD_CreatePath(os.str().c_str());
            bool bResult = KLSTD_DeletePath(osPathStart.str().c_str());
            if(!bResult)
                KLSTD_THROW(KLSTD::STDE_GENERAL);
        };
    //reading conf files is not required under Windows
    //#ifndef _WIN32 now required :-)
        {//test config files
            const char c_szwTestData[] =
                    "[TestSection1]\n"
                    "\n"
                    "Var1 = \"Some string !!!\"\n"
                    "# comment comment \n"
                    "Var2 = Unqoted\n"
                    "Var3 = Unqoted # comment comment\n"
                    "Var4 = # comment comment\n"
                    "Var4 = \"Escaped \\\"string\\\"\"\n"
                    "[TestSection2]\n"
                    "Var15 =\n"
                    "Var16 = 25434378977882287323783783278327\n"
                    "# Var18 = 25434378977882287323783783278327\n"
                    "\n"
                    "Var17 =";
            const wchar_t* c_szwNames[] = 
            {
                L"TestSection1", 
                L"TestSection2", 
                NULL
            };
            const wchar_t* c_szwNames2[] = 
            {
                L"TestSection2", 
                NULL
            };
            const wchar_t c_szwIni[] = L"$test.conf";

            KLSTD::CAutoPtr<KLSTD::File> pFile;
            KLSTD_CreateFile(
                        c_szwIni, 
                        KLSTD::SF_READ|KLSTD::SF_WRITE,
                        KLSTD::CF_CREATE_ALWAYS,
                        KLSTD::AF_WRITE,
                        KLSTD::EF_SEQUENTIAL_SCAN,
                        &pFile);
            pFile->Write(c_szwTestData, KLSTD_COUNTOF(c_szwTestData)-1);
            pFile = NULL;
            KLPAR::ParamsPtr pData1, pData2, pData3;
            KLSTD_ReadConfig(
                        c_szwIni,
                        NULL,
                        NULL,
                        &pData1);
            KLSTD_ReadConfig(
                        c_szwIni,
                        c_szwNames,
                        NULL,
                        &pData2);
            KLSTD_ReadConfig(
                        c_szwIni,
                        c_szwNames2,
                        NULL,
                        &pData3);

            if(!pData1->DoesExist(L"TestSection1"))
                KLSTD_THROW(KLSTD::STDE_GENERAL);
            if(!pData1->DoesExist(L"TestSection2"))
                KLSTD_THROW(KLSTD::STDE_GENERAL);
            KLPARLOG_LogParams(1, pData1);
            if(pData1->Compare(pData2) != 0)
                KLSTD_THROW(KLSTD::STDE_GENERAL);
            if(pData3->DoesExist(L"TestSection1"))
                KLSTD_THROW(KLSTD::STDE_GENERAL);
        };
    //#endif		
	const wchar_t c_szwTest1[]=L"01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";
	const char c_szaTest1[]="01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";
#ifdef _WIN32 // win32 only
	const TCHAR c_szTest1[]=_T("01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");
#endif
	for(int i=0; i < 100000; ++i)
	{
		std::wstring wstrX;
		wstrX.assign( KLSTD_A2CW2(c_szaTest1) );
		KLSTD_ASSERT_THROW(wstrX == c_szwTest1);
		std::string astrX;
		astrX.assign( KLSTD_W2CA2(c_szwTest1) );
		KLSTD_ASSERT_THROW(astrX == c_szaTest1);
#ifdef _WIN32 // win32 only
		std::wstring wstrX2=KLSTD_T2CW2(c_szTest1);
		KLSTD_ASSERT_THROW(wstrX2 == c_szwTest1);
		
		KLSTD_T2CW2 wstrX3(c_szTest1);
		KLSTD_ASSERT_THROW(wcscmp(wstrX3, c_szwTest1) == 0);
#endif
#ifdef N_PLAT_NLM
		ThreadSwitch();
#endif
		
	};
	KLERR_CATCH(pError)
		bResult=false;
        KLERR_RETHROW();
	KLERR_ENDTRY
		return bResult?0:(-1);
}

namespace
{
    const int c_nIterations=10000;
    using namespace std;
	
    unsigned long KLSTD_THREADDECL threadproc(void* p)
    {
		std::wstring* pString=(std::wstring*)p;
		{
			for(int i=0; i < c_nIterations; ++i)
			{
				std::wstring wstrX=*pString;
				std::wstring wstrY=wstrX;
				std::wstring wstrZ=wstrY;
				wstrX.size();
				wstrY.size();
				wstrZ.size();
				wstrZ=*pString;
				//		if(wstrX.size() != wstrY.size())
				//			wclog << L"wstrX.size() != wstrY.size()" << endl;
				//		wstrZ.size();
			};
		};
		delete pString;
		return 0;
    }
	

    void PrintInfo(const std::wstring& x)
    {
#ifndef _STLPORT_VERSION
        ///*
		wchar_t* p=(wchar_t*)x.c_str();
		PBYTE pdw=(PBYTE)p;
        //if(((long)(*(pdw-1))) != 0)
		//DebugBreak();
        long lRefCnt = (*(pdw-1));
        if(lRefCnt)
            std::cout << "Refcnt=" << lRefCnt << endl;
        //*/
#endif
    };
	
    bool stl_test_Do()
    {
		const wchar_t c_templateStr[] = L"qwertyQWERTY1234567890-0987654321YTREWQytrewq";
		const int c_nThreads=10;	

		KLSTD::Thread *phThreads[c_nThreads];		
		std::wstring wstrTest=c_templateStr;		
		PrintInfo(wstrTest);
		{			
			int i=0;
			for(i=0; i < c_nThreads; ++i)
			{
				unsigned thrdaddr=0;			 
				KLSTD_CreateThread( &phThreads[i] );

				phThreads[i]->Start( L"SLT_TEST_THREAD", &threadproc,  new std::wstring(wstrTest) );
			};

			for(i=0; i < c_nThreads; ++i)
			{
				phThreads[i]->Join();
			}
		};
		PrintInfo(wstrTest);
	
		return (wstrTest==c_templateStr);
    }    
};


void CStlTest::Initialize(KLTST2::TestData2* pTestData)
{
	KLSTD_Initialize();
	KLPAR_Initialize();		
}

KLSTD_NOTHROW void CStlTest::Deinitialize() throw()
{
	KLERR_BEGIN

		KLPAR_Deinitialize();	
	KLSTD_Deinitialize();
	KLERR_END
}

long CStlTest::Run()
{
	bool bResult=true;
	KLERR_TRY
		bResult=stl_test_Do();
	KLERR_CATCH(pError)
		bResult=false;
	KLERR_ENDTRY
		return bResult?0:(-1);
}



namespace
{
    using namespace std;
	
    struct rwlock_data_t
    {
        rwlock_data_t()
            :   m_lThreadCounter(0)
            ,   m_bStop(false)
        {;}
        KLSTD::CAutoPtr<KLSTD::ReadWriteLock>   m_pLock;
        volatile long                           m_lThreadCounter;
        volatile bool                           m_bStop;
    };

    unsigned long KLSTD_THREADDECL rwlock_threadproc(void* p)
    {
        rwlock_data_t* pData = (rwlock_data_t*)p;
        srand(KLSTD::GetSysTickCount()*KLSTD_GetCurrentThreadId());
        KLSTD_ASSERT_THROW(pData);
        while(!pData->m_bStop)
		{
			int randval = rand();
            if(randval % 17 == 1)
            {
                KLSTD::AutoReadWriteLock arwl(pData->m_pLock);
                KLSTD_InterlockedIncrement(&pData->m_lThreadCounter);
                KLSTD_Sleep(KLSTD_Random(0, 100));
                KLSTD_InterlockedDecrement(&pData->m_lThreadCounter);
            }
            else
            {
                KLSTD_Sleep(KLSTD_Random(0, 100));
            };
        };
		return 0;
    }
	
    void rwlock_test_Do()
    {
#ifdef _WIN32
		const int c_nThreads=10;	
		KLSTD::Thread *phThreads[c_nThreads];
        rwlock_data_t data;        
        KLSTD_CreateReadWriteLock(&data.m_pLock);
		{
			for(int i=0; i < c_nThreads; ++i)
			{
				KLSTD_CreateThread( &phThreads[i] );

				phThreads[i]->Start( L"RWLOCK_TEST_THREAD", &rwlock_threadproc,  &data );
			};
        };
        ;
        KLERR_TRY
            const int c_nIterations=100;
			for(int j = 0; j < c_nIterations; ++j)
			{
				AutoReadWriteLock arwl(data.m_pLock, true);
				if(data.m_lThreadCounter != 0)
                {
                    KLSTD_THROW(KLSTD::STDE_GENERAL);
                };
			};
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
        KLERR_FINALLY
            data.m_bStop = true;

			for(int i=0; i < c_nThreads; ++i)
			{
				phThreads[i]->Join();
			}
		KLERR_RETHROW();
        KLERR_ENDTRY		
#endif
    };
    
	
}

void CRWLockTest::Initialize(KLTST2::TestData2* pTestData)
{
	KLSTD_Initialize();
	KLPAR_Initialize();		
}

KLSTD_NOTHROW void CRWLockTest::Deinitialize() throw()
{
	KLERR_BEGIN
		KLPAR_Deinitialize();	
	KLSTD_Deinitialize();
	KLERR_END
}

long CRWLockTest::Run()
{
	bool bResult=true;
	KLERR_TRY
		rwlock_test_Do();
	KLERR_CATCH(pError)
		bResult=false;
	KLERR_RETHROW();
	KLERR_ENDTRY
		return bResult?0:(-1);
}    

namespace
{
    const size_t c_nIO_Iterations = 20;
    const size_t c_nIOThreads = 10;
    const wchar_t c_szwFname[]=L"$err_test_testfile.foo";
};


void CIOTest::Initialize(KLTST2::TestData2* pTestData)
{
	m_lCounter = 0;
    m_lCounter2 = 0;
	KLSTD_Initialize();
	KLPAR_Initialize();
	m_arrThreads.SetSize(c_nIOThreads);
	for(int i=0; i < c_nIOThreads; ++i)
	{
		KLERR_BEGIN
			KLSTD::Thread* pThread=NULL;
		KLSTD_CreateThread(&pThread);
		m_arrThreads.Add(pThread);
		KLERR_ENDT(1)
	};
};

KLSTD_NOTHROW void CIOTest::Deinitialize() throw()
{
	KLERR_BEGIN
		KLPAR_Deinitialize();	
	KLSTD_Deinitialize();
	KLERR_END
}

unsigned long KLSTD_THREADDECL CIOTest::ThreadFunction(void *arpg)

{
    CIOTest* pThis = (CIOTest*)arpg;
	KLSTD_Sleep(KLSTD_Random(0, 100));
	KLERR_TRY
		for(size_t i=0; i < c_nIO_Iterations; ++i)
		{
			CAutoPtr<FileSemaphoreNoReadLock> pSem;
			KLSTD_CreateFileSemaphoreNoReadLock(
				c_szwFname,
				&pSem);
            {
                AutoFileSemaphoreNoReadLock af(pSem, false, KLSTD_INFINITE);
                long lX = pThis->m_lCounter2;
                ++lX;
                KLSTD_Sleep(KLSTD_Random(100, 300));
                pThis->m_lCounter2 = lX;
            };
            KLSTD_Sleep(KLSTD_Random(100, 700));
		};
        KLERR_CATCH(pError)
            KLERR_SAY_FAILURE(1, pError);
		KLSTD_InterlockedIncrement(&pThis->m_lCounter);
        KLERR_ENDTRY
			return 0;
};

long CIOTest::Run()
{
	m_lCounter = 0;
    m_lCounter2 = 0;
	std::wstring wstrFileName;
	KLSTD_GetTempFile(wstrFileName);
	bool bResult=true;        
	int i = 0;
	for(i=0; i < c_nIOThreads; ++i)
	{
		KLERR_BEGIN
			m_arrThreads[i]->Start(L"", ThreadFunction, (void*)this);
		KLERR_ENDT(1)
	};
	;
	for(i=0; i < c_nIOThreads; ++i)
	{
		KLERR_BEGIN
			m_arrThreads[i]->Join();
		KLERR_ENDT(1)
	};
	KLSTD_CHK(m_lCounter, m_lCounter == 0);
    KLSTD_CHK(m_lCounter2, m_lCounter2 == c_nIO_Iterations*c_nIOThreads);
	return 0;
}


