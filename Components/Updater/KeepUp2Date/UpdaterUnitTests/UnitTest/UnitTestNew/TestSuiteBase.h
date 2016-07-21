#ifndef TESTSUITE_BASE_H
#define TESTSUITE_BASE_H

#include "stdafx.h"

#include "logger.h"

#define QC_BOOST_CHECK_MESSAGE(P1, P2) \
	{\
		bool result = (P1);\
		if (!result) m_step.AddPoint (wstring (P2));\
		if (!result)\
		{\
			char buf[1024];\
			WideCharToMultiByte (CP_ACP, 0, wstring(P2).c_str(), wstring(P2).length()+1, buf, 1024, NULL, NULL);\
			BOOST_CHECK_MESSAGE (result, buf);\
			_ASSERTE (false);\
		}\
	}

#define QC_BOOST_REQUIRE_MESSAGE(P1, P2) \
	{\
		bool result = (P1);\
		if (!result) m_step.AddPoint (wstring (P2));\
		if (!result)\
		{\
			char buf[1024];\
			WideCharToMultiByte (CP_ACP, 0, wstring(P2).c_str(), wstring(P2).length()+1, buf, 1024, NULL, NULL);\
			BOOST_REQUIRE_MESSAGE (result, buf);\
		}\
	}

class TestSuiteBase
{
public:

	TestSuiteBase (const std::wstring& testName): m_logger ( Logger::GetInstance() ),
												 m_testp ( new Logger::Test (testName) )
	{
		m_logger.AddTest(m_testp);
	}

protected:

	Logger& m_logger;
	Logger::TestP m_testp;
	Logger::Step m_step;
};

#endif