#ifndef TEST_LOCALFILE_H
#define TEST_LOCALFILE_H

#include "..\..\..\Client\helper\local_file.h"

#include "helper.h"
#include "Configuration.h"
#include "TestInterface.h"

#include "boost_test.h"



class Test_LocalFile: public BaseTest
{
public:
	Test_LocalFile (const string& ini_file, bool log_enable, const string& log_name):
					config_lf (Configuration::Init (ini_file.c_str()) .GetLocalFileConfig()),
						ptr_log (new Ini_Log (log_enable)),
					BaseTest ("LocalFile") {};
		
	void TestFileCopyOK ();
	void TestFileCopyWhenExists ();
	void TestFileUnlinkOK ();
	void TestFileUnlinkFail ();
	void TestFileRenameOK ();
	void TestFileRenameFail ();
	void TestFileExistenceOK ();
	void TestFileExistenceFail ();


private:

	shared_ptr<Log> ptr_log;

	const Config_LocalFile& config_lf;


};


class Suite_LocalFile: public test_suite
{
	public:

		Suite_LocalFile (const string& ini_file, const string& log_name);
};




#endif