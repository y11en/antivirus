#if !defined(__KL_LICENSING2_TEST_RUNTEST_H)
#define __KL_LICENSING2_TEST_RUNTEST_H

#include <string>

namespace KasperskyLicensing  {
namespace Test  {

class TestRunner
{
public:
	typedef bool (*TestFunctionPtr)(std::string& sErrorDesc);
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
		string sErrorDesc;
		if ((*pfn)(sErrorDesc))
		{
			++m_passed;
			cout << "passed" << endl;
			return true;
		}
		else
		{
			++m_failed;
			cout << "failed" << endl;
			cout << "\tDescription: " << sErrorDesc << endl;
			return false;
		}
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
private:
	int m_passed, m_failed;
};

}  // namespace Test
}  // namespace KasperskyLicensing

#endif  // !defined(__KL_LICENSING2_TEST_RUNTEST_H)
