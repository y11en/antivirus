#include "../MailWasherUnitTests/_aux.h"

#include <utils/prague/prague.hpp>
#include <plugin/p_tempfile.h>
#include <plugin/p_tm.h>

#include "../TaskManager/i_fake_taskmanager.h"

#include "../MailWasherUnitTests/TestManager.h"
#include <string>

using namespace std;

#include <iface/i_taskex.h>
#include <plugin/p_MailWasher.h>




int main(int argc, char* argv[])
{
	srand(time(0));

	TestManager tm;

	tm.LoadPrague();
 

	//tm.TestHeadersRequest("..\\DataManager\\test1");
	//tm.TestMessagesRequest("..\\DataManager\\test1");

	tm.TestRetrMessage_MW_Messages("..\\DataManager\\test1");
	//tm.TestRetrMessage_MW_Headers("..\\DataManager\\test1");
	//tm.TestRetrHeaders_MW_Messages("..\\DataManager\\test1");
	//tm.TestRetrHeaders_MW_Headers("..\\DataManager\\test1");

	//tm.TestRetr_Deleted("..\\DataManager\\test1");
	//tm.TestTop_Deleted("..\\DataManager\\test1");
	//tm.TestDele_Deleted("..\\DataManager\\test1");

	//tm.TestList_Deleted ();//bug 30508
	//tm.TestStat_Deleted ();//bug 30508
	//tm.TestUidl_Deleted ();//bug 30508
	//tm.TestRset_Deleted ();//bug 30508

	//tm.TestPipelining_Cached("..\\DataManager\\test1");
	//tm.TestPipelining_NotCached("..\\DataManager\\test1");
	//tm.TestPipelining_Mixed("..\\DataManager\\test1");

	//tm.TestSessionWithDeleted("..\\DataManager\\test1");
	//tm.TestSessionWithDeleted("..\\DataManager\\test2");
	//tm.TestSessionWithDeleted("..\\DataManager\\test3");
	//tm.TestSessionWithDeleted("..\\DataManager\\test4");
	//tm.TestSessionWithDeleted("..\\DataManager\\test5");
	//tm.TestSessionWithDeleted("..\\DataManager\\test6");
	//tm.TestSessionWithDeleted("..\\DataManager\\test7");

	//читаем сначала топы, потом заголовки, ну и наоборот
	//tm.TestRepeatRequests ("..\\DataManager\\test1");

	//tm.TestCancelRequest ("..\\DataManager\\test1");
	//tm.TestCancelDelete ("..\\DataManager\\test1");

	//tm.TestCloseRequest ("..\\DataManager\\test1"); не прогонять тест, я думаю, некорректно делать в процессе
	//выкачки писем pm_PROCESS_CLOSE

	//tm.TestErrRequest ("..\\DataManager\\test1");
	//tm.TestErrDelete ("..\\DataManager\\test1"); //ok

	return 0;
}

