/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	prci_test.h
 * \author	Andrew Kazachkov
 * \date	25.02.2003 13:26:27
 * \brief	
 * 
 */

#ifndef __KLJRNL_TEST_H__
#define __KLJRNL_TEST_H__


#include "build/general.h"

#include "testmodule.h"
#include "helpers.h"


class CJRNLGeneralTest2 : public KLSTD::KLBaseImpl<KLTST2::Test2>
{
public:
	IMPLEMENT_TEST2_INSTANTIATE(CJRNLGeneralTest2)

	void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
};

#endif //__KLJRNL_TEST_H__
