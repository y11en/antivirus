/*!
 * (C) 2003 "Kaspersky Lab"
 *
 * \file par/test/partest.h
 * \author
 * \date 2003
 * \brief 
 *
 */

#ifndef __KL_PAR_TEST_TEST_H
#define __KL_PAR_TEST_TEST_H

#ifdef __unix
#  ifdef __HP_aCC
#pragma VERSIONID "@(#)$Id: test.h,v 1.3 2003/04/04 11:55:45 ptr Exp $"
#  else
#ident "@(#)$Id: test.h,v 1.3 2003/04/04 11:55:45 ptr Exp $"
#  endif
#endif

#ifndef __unix
/*
 * Really this due to I (-ptr) modify tst2/helpers.h.
 * In VSS it has much more dependencies than really required
 *
	*/
#endif

#include "../../../tst/kltester/testmodule.h"
#include "../../../tst/kltester/helpers.h"

class CGeneralTest2 :
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
    IMPLEMENT_TEST2_INSTANTIATE(CGeneralTest2)

    void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
};

class CCloningTest2 :
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CCloningTest2)

    void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
};

class CDuplicatingTest2 :
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CDuplicatingTest2)

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
    
class CSerTest:
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CSerTest)

    void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	long Run();
};


#endif // __KL_PAR_TEST_TEST_H

// Local Variables:
// mode: C++
// End:
