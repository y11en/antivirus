#include <std/base/klstd.h>
//#include <kca/prss/settingsstorage.h>
#include <kca/ss_srv/ss_srv.h>

#include "testmodule.h"
#include "helpers.h"

#include "prci/prci_test.h"
#include "prss/prss_test.h"
#include "pres/pres_test.h"
#include "prts/prts_test.h"
#include "jrnl/jrnl_test.h"
#include "ft/filetransfertest.h"

#include <kca/init/init_kca.h>

TESTMAP_2_DECL;

BEGIN_TESTMAP_2()

// PRCI
	ENTRY_TEST_2(L"PRCI_General",	CGeneralTest2)
//PRSS
	ENTRY_TEST_2(L"PRSS_General",	CPRSSGeneralTest2)
	ENTRY_TEST_2(L"PRSS_Timing",	CTimingTest2)
//	ENTRY_TEST_2(L"PRSS_Sync",		CSyncTest2)
//	ENTRY_TEST_2(L"PRSS_Sync2",		CSyncTest3)
	ENTRY_TEST_2(L"PRSS_Policy",	CPolicyTest)

//PRES
	ENTRY_TEST_2(L"PRES_General",	CPRESGeneralTest2)
	ENTRY_TEST_2(L"PRES_General2",	CPRESGeneralTest3)
	ENTRY_TEST_2(L"PRES_General3",	CPRESGeneralTest4)
//	ENTRY_TEST_2(L"PRES_General4",	CPRESGeneralTest5)

//PRTS
	ENTRY_TEST_2(L"PRTS_General",	CPRTSGeneralTest2)

//JNRL	
	ENTRY_TEST_2(L"JRNL_General",	CJRNLGeneralTest2)
	
//FT
//	ENTRY_TEST_2(L"FT_FolderSync",	CFileTransferFolderSync)	
//	ENTRY_TEST_2(L"FT_Multicast",	CFileTransferMulticast)

END_TESTMAP_2_EX()

#define FOO_DECL

DECLARE_MODULE_INIT_DEINIT2(FOO_DECL, KCATEST)

IMPLEMENT_MODULE_INIT_DEINIT(KCATEST)

#define KCATEST_PRSS_SERVER L"KCATEST_PRSS_SERVER"

IMPLEMENT_MODULE_INIT_BEGIN2(FOO_DECL, KCATEST)
    KLCSKCA_Initialize();
	PRTSTestInit();
	PRESTestInit();

    ::KLSTD_SetDefaultProductAndVersion(KLCS_PRODUCT_ADMSERVER, KLCS_VERSION_ADMSERVER);
    ::KLSSS_SetDefaultServer(KLCS_PRODUCT_ADMSERVER, KLCS_VERSION_ADMSERVER, KCATEST_PRSS_SERVER);
    ::KLSSS_AddSsServer(KLCS_PRODUCT_ADMSERVER, KLCS_VERSION_ADMSERVER, KCATEST_PRSS_SERVER);
IMPLEMENT_MODULE_INIT_END()



IMPLEMENT_MODULE_DEINIT_BEGIN2(FOO_DECL, KCATEST)
    ::KLSSS_RemoveSsServer(KCATEST_PRSS_SERVER);
	PRTSTestDeInit();
	PRESTestDeInit();
    KLCSKCA_Deinitialize();
IMPLEMENT_MODULE_DEINIT_END()


class CKCATestModule: public KLTST2_SIMPLE::TestModule2Imp
{
public:
	virtual void Initialize()
	{
        KCATEST_Initialize();
	}
	virtual KLSTD_NOTHROW void Deinitialize() throw()
	{
        KCATEST_Deinitialize();
	}
	IMPLEMENT_TEST_MODUL;
};

} // end KLTST2_SIMPLE

#ifdef _WIN32
extern "C" __declspec(dllexport) void GetModuleInterface(KLTST2::TestModule2** ppModule)
#else
void GetModuleInterface(KLTST2::TestModule2** ppModule)
#endif
{
	KLSTD_CHKOUTPTR(ppModule);
	*ppModule=new KLTST2_SIMPLE::CKCATestModule;
};
