#pragma comment(linker, "/ignore:4006")


#include "Test_FileTransaction.h"
#include "Test_LocalFile.h"
#include "Test_NetCore.h"
#include "Test_CNetCore.h"
#include "Test_UpdaterTransaction.h"
#include "Test_UpdateInfo.h"
//#include "Test_CNtlmMan.h"
#include "Test_CHTTP.h"
#include "Test_IndexFileXMLParser.h"
#include "Test_SitesFileXMLParser.h"
#include "Test_PrimaryIndexXMLParser.h"
#include "helper.h"

#include <map>

// Boost.Test
#include <boost/test/unit_test.hpp>
//#include <boost/test/parameterized_test.hpp>

using boost::unit_test::test_suite;

bool Parse_args (int argc, char* argv[], map<string, string>& params)
{
	int arg = 1;
	while(arg < argc)
    {
        if(arg < argc-1 && !strcmp("-ini", argv[arg]))
        { // ini-file name
            arg++;
			params[argv[arg-1]] = argv[arg];
        };
        
		if(arg < argc-1 && !strcmp("-log", argv[arg]))
        { // log file name
            arg++;
            params[argv[arg-1]] = argv[arg];
        }

		if(arg < argc-1 && !strcmp("-td", argv[arg]))
        { // td import file name
            arg++;
            params[argv[arg-1]] = argv[arg];
        }
        arg++;
    };
	return true;
};

test_suite* init_unit_test_suite (int argc, char * argv[])
{
	map<string,string> params;
	test_suite* test = BOOST_TEST_SUITE ("Unit test Shell");

	if ((Parse_args (argc, argv, params)) && (params["-ini"] != ""))
	{
		if (params["-log"] != "") DeleteFile (params["-log"].c_str());
		map<string, int>& classes = Configuration::Init (params["-ini"].c_str()). GetClasses();
		if (params["-td"] != "") Logger::Init (params["-td"]);

		//if (classes[CLASSES_LOCALFILE]) test->add (new Suite_LocalFile(params["-ini"], params["-log"]));
		//if (classes[CLASSES_NETCORE]) test->add (new Suite_NetCore(params["-ini"], params["-log"]));
		if (classes[CLASSES_CNETCORE]) test->add (new Suite_CNetCore(params["-ini"], params["-log"]));
		//if (classes[CLASSES_UPDATERTRANSACTION]) test->add (new Suite_UpdaterTransaction(params["-ini"], params["-log"]));
		//if (classes[CLASSES_UPDATEINFO]) test->add (new Suite_UpdateInfo(params["-ini"], params["-log"]));
		//obsolete//if (classes[CLASSES_CNTLMMAN]) test->add (new Suite_CNtlmMan(params["-ini"], params["-log"]));
		//if (classes[CLASSES_CHTTP]) test->add (new Suite_CHTTP(params["-ini"], params["-log"]));
		//if (classes[CLASSES_INDEXFILEXMLPARSER]) test->add (new Suite_IndexFileXMLParser(params["-ini"], params["-log"]));
		//if (classes[CLASSES_SITESFILEXMLPARSER]) test->add (new Suite_SitesFileXMLParser(params["-ini"], params["-log"]));
		//obsolete//if (classes[CLASSES_PRIMARYINDEXXMLPARSER]) test->add (new Suite_PrimaryIndexXMLParser(params["-ini"], params["-log"]));
	};

	return test;
}
	