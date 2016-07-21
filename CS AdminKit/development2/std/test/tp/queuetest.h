/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	queuetest.h
 * \author	Andrew Kazachkov
 * \date	13.04.2005 12:35:48
 * \brief	
 * 
 */

#ifndef __QUEUETEST_H__
#define __QUEUETEST_H__

#include "std/base/klstd.h"	
#include "testmodule.h"
#include "helpers.h"
#include "std/tp/tpcmdqueue.h"


class CQueueTest : public KLSTD::KLBaseImpl<KLTST2::Test2>
{
  public:
	IMPLEMENT_TEST2_INSTANTIATE(CQueueTest)

    KLSTD::CPointer<KLCMDQUEUE::CmdQueue<int, int, CQueueTest> > m_pQueue;
    volatile long m_lSent, m_lReceived;

	void Initialize(KLTST2::TestData2* pTestData);

	KLSTD_NOTHROW void Deinitialize() throw();

	long Run();

    void OnCmd(int);

    static unsigned long KLSTD_THREADDECL ThreadFunction(void *arpg);

    static unsigned long KLSTD_THREADDECL RunnerFunction(void *arpg);

    void Do();
};


#endif //__QUEUETEST_H__
