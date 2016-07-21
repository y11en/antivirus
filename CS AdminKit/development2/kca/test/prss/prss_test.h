/*!
 * (C) 2003 "Kaspersky Lab"
 *
 * \file prss/test/test.h
 * \author
 * \date 2003
 * \brief 
 *
 */

#ifndef __KL_PRSS_TEST_TEST_H
#define __KL_PRSS_TEST_TEST_H

#ifdef __unix
#  ifdef __HP_aCC
#pragma VERSIONID "@(#)$Id: test.h,v 1.3 2003/04/04 11:55:46 ptr Exp $"
#  else
#ident "@(#)$Id: test.h,v 1.3 2003/04/04 11:55:46 ptr Exp $"
#  endif
#endif

#include "build/general.h"

#include "testmodule.h"
#include "helpers.h"

class CPRSSGeneralTest2 :
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CPRSSGeneralTest2)

    void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
};


class CTimingTest2 :
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CTimingTest2)

    void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
};

class CSyncTest2 :
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CSyncTest2)

    void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
};

class CSyncTest3 : public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CSyncTest3)

    void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
};

class CPolicyTest : public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CPolicyTest)

    void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
};

#endif // __KL_PRSS_TEST_TEST_H

// Local Variables:
// mode: C++
// End:
