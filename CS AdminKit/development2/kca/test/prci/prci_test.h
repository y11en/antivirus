/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	prci_test.h
 * \author	Andrew Kazachkov
 * \date	25.02.2003 13:26:27
 * \brief	
 * 
 */

#ifndef __KLPRCI_TEST_H__
#define __KLPRCI_TEST_H__

#include <std/base/klstd.h>
#include "testmodule.h"
//#include "helpers.h"
//#include "std/tp/tpcmdqueue.h"

class CGeneralTest2 : public KLSTD::KLBaseImpl<KLTST2::Test2>
{
public:
	IMPLEMENT_TEST2_INSTANTIATE(CGeneralTest2)

	void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
};

#endif //__KLPRCI_TEST_H__
