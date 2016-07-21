#include "jrnl_test.h"
#include <std/base/klstd.h>
#include "../../jrnl/journal.h"

#define KLCS_MODULENAME L"JRNLTST2"

void CJRNLGeneralTest3::Initialize(KLTST2::TestData2* pTestData)
{
}

void CJRNLGeneralTest3::Deinitialize() throw()
{
}

long CJRNLGeneralTest3::Run()
{
	KLERR_TRY
	{
		KLSTD::CAutoPtr<KLJRNL::Journal> pJournal;
        KLJRNL_CreateJournal(&pJournal, true);
		
		std::wstring wstrJrnlPath = L"c:\\test\\jrnl\\test_jrnl.txt";
        KLSTD_TRACE1(1, L"Will create test journal at \"%s\".\n", wstrJrnlPath.c_str() );
		
		KLJRNL::CreationInfo creationInfo;
        creationInfo.bLimitPages = false;
        creationInfo.bBinaryFormat = true;
        creationInfo.pageLimitType = KLJRNL::pltRecCount;
        creationInfo.nMaxRecordsInPage = 10000;
		
//        pJournal->Open(wstrJrnlPath, KLSTD::CF_CREATE_ALWAYS, KLSTD::ACCESS_FLAGS(KLSTD::AF_WRITE | KLSTD::AF_READ), creationInfo);
//		pJournal->Open(wstrJrnlPath, KLSTD::CF_OPEN_ALWAYS, KLSTD::ACCESS_FLAGS(KLSTD::AF_WRITE | KLSTD::AF_READ), creationInfo);
		pJournal->Open(wstrJrnlPath, KLSTD::CF_OPEN_EXISTING, KLSTD::ACCESS_FLAGS(KLSTD::AF_READ), creationInfo);

		pJournal->ResetIterator();
		long n = pJournal->GetRecordCount();
	}
	KLERR_CATCH(pError)
	{
	}
	KLERR_ENDTRY

	return true;
}