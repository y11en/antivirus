#ifndef TEST_CHTTP_H
#define TEST_CHTTP_H


#include <winsock2.h>
#include "..\..\..\Client\core\netlib.h"
//#include "..\..\..\Client\core\netfachttp.h"

#include "helper.h"
#include "Configuration.h"
#include "TestInterface.h"

#include "boost_test.h"



class Test_CHTTP: public BaseTest
{
public:
	Test_CHTTP (const string& ini_file, bool log_enable, const string& log_name):
					m_log (log_enable), BaseTest ("CHTTP") {}
		
	//CHTTPData
	void TestGeneratingRequest();
	void TestMakingAuthPhrase();

	//CHTTPHeader
	void TestParsingHeader();

	//CHTTPRequester
	void TestCHttpRequester();

private:
	Ini_Log m_log;

};


class Suite_CHTTP: public test_suite
{
	public:

		Suite_CHTTP (const string& ini_file, const string& log_name);
};




#endif