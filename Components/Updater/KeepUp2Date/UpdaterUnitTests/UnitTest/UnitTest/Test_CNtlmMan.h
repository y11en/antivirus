#ifndef TEST_CNTLMMAN_H
#define TEST_CNTLMMAN_H

#define SECURITY_WIN32

#include <winsock2.h>
#include <windows.h>
#include <winnt.h>
#include <sspi.h>

#include "..\..\..\Client\core\win32\updater_ntlm.h"
#include "..\..\..\Client\helper\base64.h"

#include "helper.h"
#include "Configuration.h"
#include "TestInterface.h"

#include "boost_test.h"



class Test_CNtlmMan: public BaseTest
{
public:
	Test_CNtlmMan (const string& ini_file, bool log_enable, const string& log_name);
		
	void Test();

private:
	Ini_Log m_log;
	char user[64];
	char host[64];
	char domen[16];

};


class Suite_CNtlmMan: public test_suite
{
	public:

		Suite_CNtlmMan (const string& ini_file, const string& log_name);
};




#endif