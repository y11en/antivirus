
#include "prci_test.h"

#include "std/thr/thread.h"
#include "transport/tr/common.h"

#include "transport/ev/common.h"
#include "kca/prts/taskstorageserver.h"
#include <kca/init/init_kca.h>


using namespace KLSTD;

#ifdef WIN32
#pragma comment(lib, "Ws2_32.lib")
#endif

#define KLCS_MODULENAME L"PRCI_TEST"

int random(int nMin, int nMax);
void testPRCI();

//using namespace KLTST2;


void CGeneralTest2::Initialize(KLTST2::TestData2* pTestData)
{	
    KLCSKCA_Initialize();
};

KLSTD_NOTHROW void CGeneralTest2::Deinitialize() throw()
{
    KLCSKCA_Deinitialize();
};

long CGeneralTest2::Run()
{
	bool bResult=true;		
	KLERR_TRY
		testPRCI();
	KLERR_CATCH(pError)
		bResult=false;
		KLERR_SAY_FAILURE(1, pError);
        KLERR_RETHROW();
	KLERR_ENDTRY		
	return bResult?0:(-1);
};
