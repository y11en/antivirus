/*!
 * (C) 2003 "Kaspersky Lab"
 *
 * \file prss/test/main.cpp
 * \author
 * \date 2003
 * \brief 
 *
 */

#ifdef __unix
#  ifdef __HP_aCC
#pragma VERSIONID "@(#)$Id: main.cpp,v 1.9 2003/07/14 13:23:08 ptr Exp $"
#  else
#ident "@(#)$Id: main.cpp,v 1.9 2003/07/14 13:23:08 ptr Exp $"
#  endif
#endif

#include "prss_test.h"

#include "std/io/klio.h"

#include "build/general.h"

#include "../../../std/test/testing/testing.h"

#include <kca/init/init_kca.h>

#define KLCS_MODULENAME L"PRSS_TEST"

using namespace KLPAR;
using namespace KLERR;
using namespace KLSTD;

#ifdef WIN32
#pragma comment(lib, "Ws2_32.lib")
#endif

void testStorage(const wchar_t *szFileName);
void testStorage2(const wchar_t *szFileName);
void testStorage3(const wchar_t *szFileName);
// std::wstring MakePerThreadFileName(const wchar_t* szwTemplate);
void TestStorageSync(const std::wstring& wstrLocationAgent, const std::wstring& wstrLocationServer);
void TestStorageSync2(const std::wstring& wstrLocationAgent, const std::wstring& wstrLocationServer);
void TestStoragePolicy(
					const std::wstring& wstrUser,
					const std::wstring& wstrMachine,
					const std::wstring& wstrPolicy);

void TestSmartWrite();

static const wchar_t szwFileTml1[]=L"$prss_test1-%d.xml";
static const wchar_t szwFileTml2[]=L"$prss_test2-%d.xml";
static const wchar_t szwFileTml3[]=L"$prss_test3-%d.xml";
static const wchar_t szwFileTml4[]=L"$prss_test4-%d.xml";
static const wchar_t szwFileTml5[]=L"$prss_test5-%d.xml";
static const wchar_t szwFileTml6[]=L"$prss_test6-%d.xml";
static const wchar_t szwFileTml7[]=L"$prss_test_u-%d.xml";
static const wchar_t szwFileTml8[]=L"$prss_test_m-%d.xml";
static const wchar_t szwFileTml9[]=L"$prss_test_p-%d.xml";

DECLARE_MODULE_INIT_DEINIT( KLPRSS );


void CPRSSGeneralTest2::Initialize(KLTST2::TestData2* pTestData)
{
    KLCSKCA_Initialize();
}

KLSTD_NOTHROW void CPRSSGeneralTest2::Deinitialize() throw()
{
    KLCSKCA_Deinitialize();
}

long CPRSSGeneralTest2::Run()
{
  bool bResult=true;		
  KLERR_TRY
    std::wstring wstrFile1=MakePerThreadFileName(szwFileTml1);		
    KLSTD_Unlink(wstrFile1.c_str());
    testStorage(wstrFile1.c_str());
    testStorage2(wstrFile1.c_str());
    KLSTD_Unlink(wstrFile1.c_str());
  KLERR_CATCH(pError)
    bResult=false;
    KLERR_SAY_FAILURE(1, pError);
	KLERR_RETHROW();
  KLERR_ENDTRY
    return bResult?0:(-1);
}

void CTimingTest2::Initialize(KLTST2::TestData2* pTestData)
{
    KLCSKCA_Initialize();
}

KLSTD_NOTHROW void CTimingTest2::Deinitialize() throw()
{
    KLCSKCA_Deinitialize();
}

long CTimingTest2::Run()
{
  bool bResult=true;
  KLERR_TRY
    std::wstring wstrFile2=MakePerThreadFileName(szwFileTml2);
    KLSTD_Unlink(wstrFile2.c_str());
    testStorage3(wstrFile2.c_str());
    KLSTD_Unlink(wstrFile2.c_str());
  KLERR_CATCH(pError)
    bResult=false;
    KLERR_SAY_FAILURE(1, pError);			
	KLERR_RETHROW();
  KLERR_ENDTRY
  return bResult?0:(-1);
}

void CSyncTest2::Initialize(KLTST2::TestData2* pTestData)
{
    KLCSKCA_Initialize();
}

KLSTD_NOTHROW void CSyncTest2::Deinitialize() throw()
{
    KLCSKCA_Deinitialize();
}

long CSyncTest2::Run()
{
  bool bResult=true;
  KLERR_TRY
    std::wstring wstrFile3=MakePerThreadFileName(szwFileTml3);
    std::wstring wstrFile4=MakePerThreadFileName(szwFileTml4);

    KLSTD_Unlink(wstrFile3.c_str());
    KLSTD_Unlink(wstrFile4.c_str());

//    TestStorageSync(wstrFile3, wstrFile4);

    KLSTD_Unlink(wstrFile3.c_str());
    KLSTD_Unlink(wstrFile4.c_str());

  KLERR_CATCH(pError)
    bResult=false;
    KLERR_SAY_FAILURE(1, pError);    
	KLERR_RETHROW();
  KLERR_ENDTRY
  return bResult?0:(-1);
}

void CSyncTest3::Initialize(KLTST2::TestData2* pTestData)
{
    KLCSKCA_Initialize();
}

KLSTD_NOTHROW void CSyncTest3::Deinitialize() throw()
{
    KLCSKCA_Deinitialize();
}

long CSyncTest3::Run()
{
  bool bResult=true;
  KLERR_TRY
    std::wstring wstrFile5=MakePerThreadFileName(szwFileTml5);
    std::wstring wstrFile6=MakePerThreadFileName(szwFileTml6);

    KLSTD_Unlink(wstrFile5.c_str());
    KLSTD_Unlink(wstrFile6.c_str());

//    TestStorageSync2(wstrFile5, wstrFile6);

    KLSTD_Unlink(wstrFile5.c_str());
    KLSTD_Unlink(wstrFile6.c_str());

    KLERR_CATCH(pError)
      bResult=false;
      KLERR_SAY_FAILURE(1, pError);			
	  KLERR_RETHROW();
    KLERR_ENDTRY
    return bResult?0:(-1);
}

void CPolicyTest::Initialize(KLTST2::TestData2* pTestData)
{
    KLCSKCA_Initialize();
}

KLSTD_NOTHROW void CPolicyTest::Deinitialize() throw()
{
    KLCSKCA_Deinitialize();
}

long CPolicyTest::Run()
{
  bool bResult=true;
  KLERR_TRY
    std::wstring wstrFile7=MakePerThreadFileName(szwFileTml7);
    std::wstring wstrFile8=MakePerThreadFileName(szwFileTml8);
	std::wstring wstrFile9=MakePerThreadFileName(szwFileTml9);

    KLSTD_Unlink(wstrFile7.c_str());
    KLSTD_Unlink(wstrFile8.c_str());
	KLSTD_Unlink(wstrFile9.c_str());

    TestSmartWrite();
    TestStoragePolicy(wstrFile7, wstrFile8, wstrFile9);    

    KLSTD_Unlink(wstrFile7.c_str());
    KLSTD_Unlink(wstrFile8.c_str());
	KLSTD_Unlink(wstrFile9.c_str());

    KLERR_CATCH(pError)
      bResult=false;
      KLERR_SAY_FAILURE(1, pError);
	  KLERR_RETHROW();
    KLERR_ENDTRY
    return bResult?0:(-1);
}
// Local Variables:
// mode: C++
// End:
