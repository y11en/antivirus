/*!
*		(C) 2002 Kaspersky Lab 
*		
*		\file	main.cpp
*		\brief	
*		
*		\author Андрей Брыксин
*		\date	10.10.2002 11:54:52
*		
*		Example:	
*/		


#include <srvp/init/init_srvp.h>
#include <tst/kltester/testmodule.h>
#include <exception>
#include <list>
#include <vector>

#include "test.h"

using namespace KLPAR;
using namespace KLERR;
using namespace KLSTD;

#pragma comment(lib, "Ws2_32.lib")


///////////////////////////////////////////////////////////////////////////////
// Test 2

using namespace KLTST2;

class CGeneralTest2 : public KLSTD::KLBaseImpl<KLTST2::Test2>
{
public:
	IMPLEMENT_TEST2_INSTANTIATE(CGeneralTest2)

	void Initialize(KLTST2::TestData2* pTestData)
	{
	};
	KLSTD_NOTHROW void Deinitialize() throw()
	{
		KLERR_BEGIN
		KLERR_END
	};
	virtual long Run()
	{
		return m_Test.Run()?0:(-1);
	};

protected:
	CTest	m_Test;
};

BEGIN_TESTMAP_2()
	ENTRY_TEST_2(L"WNF_General", CGeneralTest2)
END_TESTMAP_2_EX()

class CTestModule2: public KLTST2_SIMPLE::TestModule2Imp
{
public:
	virtual void Initialize() {
//		::KLCSSRVP_Initialize();
	}

	virtual KLSTD_NOTHROW void Deinitialize() throw() {
//		::KLCSSRVP_Deinitialize();
	}

	IMPLEMENT_TEST_MODUL;
};

IMPLEMENT_TEST_MODUL_ENTRY_POINT();