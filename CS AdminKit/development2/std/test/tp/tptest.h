/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file test/tp/tptest.h
 * \author
 * \date 2004
 * \brief 
 *
 */

#ifndef __KL_TP_TEST_TEST_H
#define __KL_TP_TEST_TEST_H

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


class CThreadPoolTest :
    public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CThreadPoolTest)

    void Initialize(KLTST2::TestData2* pTestData);
    KLSTD_NOTHROW void Deinitialize() throw();
	
	//!\brief Запускает тест и возвращает результаты выполнения теста.
	long Run();
};

#endif // __KL_TP_TEST_TEST_H

