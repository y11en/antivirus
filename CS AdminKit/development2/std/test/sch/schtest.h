/*!
 * (C) 2003 "Kaspersky Lab"
 *
 * \file sch/test/test.h
 * \author
 * \date 2003
 * \brief 
 *
 */

#ifndef __KL_SCH_TEST_TEST_H
#define __KL_SCH_TEST_TEST_H

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

class CTaskFunctionality :
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CTaskFunctionality)

    void Initialize(KLTST2::TestData2* pTestData);
    KLSTD_NOTHROW void Deinitialize() throw();
	
	//!\brief Запускает тест и возвращает результаты выполнения теста.
	long Run();
};

class CExecution :
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CExecution)

    void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
};

class CIterator :
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CIterator)

    void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
};

class CSuspendResume :
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CSuspendResume)

    void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
};

class CPerformance :
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CPerformance)

    void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
};

#endif // __KL_SCH_TEST_TEST_H

// Local Variables:
// mode: C++
// End:
