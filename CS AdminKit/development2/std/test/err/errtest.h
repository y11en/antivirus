/*!
 * (C) 2003 "Kaspersky Lab"
 *
 * \file err/schtest.h
 * \author
 * \date 2003
 * \brief 
 *
 */

#ifndef __KL_ERR_TEST_TEST_H
#define __KL_ERR_TEST_TEST_H

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

#include "std/thr/thread.h"
#include "avp/parray.h"

#include "testmodule.h"
#include "helpers.h"

class CErrorTest :
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CErrorTest)

    void Initialize(KLTST2::TestData2* pTestData);
    KLSTD_NOTHROW void Deinitialize() throw();
	
	//!\brief Запускает тест и возвращает результаты выполнения теста.
	long Run();
};

class CStdTest :
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CStdTest)

    void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
};

class CIOTest :
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CIOTest)

	volatile long m_lCounter, m_lCounter2;
    CPArray<KLSTD::Thread>  m_arrThreads;

    void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
    static unsigned long KLSTD_THREADDECL ThreadFunction(void *arpg);
};

class CStlTest :
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CStlTest)

    void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
};

class CRWLockTest :
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CRWLockTest)

    void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
};

#endif // __KL_ERR_TEST_TEST_H

// Local Variables:
// mode: C++
// End:
