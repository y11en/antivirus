#if !defined(__KL_DSKMWRAP_TEST_RUNTEST_H)
#define __KL_DSKMWRAP_TEST_RUNTEST_H

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <dskm/cpp_wrap/dskmwrap.h>

namespace KL  {
namespace DskmLibWrapper  {
namespace Test  {

class TestRunner
{
public:
	typedef bool (*TestFunctionPtr)(std::vector<std::string>& errors);
public:
	TestRunner()
		: m_passed(0)
		, m_failed(0)
	{
	}
	bool RunTest(const char* szTestName, TestFunctionPtr pfn)
	{
		using namespace std;
		std::string sHeader("Test [");
		sHeader += szTestName;
		sHeader += "]";
		if (sHeader.size() >= 65)
		{
			sHeader += "...";
		}
		else
		{
			sHeader += std::string(68 - sHeader.size(), '.');
		}
		cout << sHeader;
		bool bPassed = false;
		vector<string> errors;
		try
		{
			bPassed = (*pfn)(errors);
		}
		catch (DskmLibException& e)
		{
			errors.push_back("DSKM library error: " + e.GetErrorCodeDesc());
		}
		catch (DskmLibWrapperException& e)
		{
			errors.push_back("DSKM library wrapper error: " + e.GetReasonDesc());
		}
		catch (std::exception& e)
		{
			errors.push_back(std::string("C++ exception: ") + e.what());
		}
		if (bPassed)
		{
			++m_passed;
			cout << "passed" << endl;
		}
		else
		{
			++m_failed;
			cout << "failed" << endl;
			for (vector<string>::const_iterator it = errors.begin();
					it != errors.end();
					++it)
			{
				cout << "\tError: " << *it << endl;
			}
		}
		return bPassed;
	}
	void ResetStatistics()
	{
		m_passed = m_failed = 0;
	}
	void PrintStatistics(std::ostream& os)
	{
		using namespace std;
		os << "Tests run    : " << (m_passed + m_failed) << endl
		   << "Tests passed : " << m_passed << endl
		   << "Tests failed : " << m_failed << endl;
	}
	int Passed() const  { return m_passed; }
	int Failed() const  { return m_failed; }
private:
	int m_passed, m_failed;
};

}  // namespace Test
}  // namespace DskmLibWrapper
}  // namespace KL

#endif  // !defined(__KL_DSKMWRAP_TEST_RUNTEST_H)
