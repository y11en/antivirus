/*!
 * (C) 2003 "Kaspersky Lab"
 *
 * \file test/thr/thrtest.h
 * \author
 * \date 2004
 * \brief 
 *
 */

#ifndef __KL_THR_TEST_TEST_H
#define __KL_THR_TEST_TEST_H

#ifdef __unix
#  ifdef __HP_aCC
#pragma VERSIONID "@(#)$Id: test.h,v 1.1 2003/02/26 17:54:19 ptr Exp $"
#  else
#ident "@(#)$Id: test.h,v 1.1 2003/02/26 17:54:19 ptr Exp $"
#  endif
#endif

#ifndef __unix
/*
 * Really this due to I (-ptr) modify tst2/helpers.h.
 * In VSS it has much more dependencies than really required
 *
 */
#  include "build/general.h"
#endif

#include "testmodule.h"
#include "helpers.h"

class CThreadTest :
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CThreadTest)

    void Initialize(KLTST2::TestData2* pTestData);
    KLSTD_NOTHROW void Deinitialize() throw();
	
	//!\brief Запускает тест и возвращает результаты выполнения теста.
	long Run();
};

class CCriticalSectionTest :
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CCriticalSectionTest)

    void Initialize(KLTST2::TestData2* pTestData);
    KLSTD_NOTHROW void Deinitialize() throw();
	
	//!\brief Запускает тест и возвращает результаты выполнения теста.
	long Run();
};

class CSemaphoreTest :
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CSemaphoreTest)

    void Initialize(KLTST2::TestData2* pTestData);
    KLSTD_NOTHROW void Deinitialize() throw();
	
	//!\brief Запускает тест и возвращает результаты выполнения теста.
	long Run();
};

class CThreadStoreTest :
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CThreadStoreTest)

    void Initialize(KLTST2::TestData2* pTestData);
    KLSTD_NOTHROW void Deinitialize() throw();
	
	//!\brief Запускает тест и возвращает результаты выполнения теста.
	long Run();
};

#endif // __KL_THR_TEST_TEST_H

// Local Variables:
// mode: C++
// End:
