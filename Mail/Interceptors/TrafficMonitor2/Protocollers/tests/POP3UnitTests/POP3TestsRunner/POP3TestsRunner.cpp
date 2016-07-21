#include "POP3Protocoller\POP3SessionAnalyzer.h"

#include "../POP3UnitTests/TrafficMonitor.h"
#include "../POP3UnitTests/_aux.h"

#include <utils/prague/prague.hpp>
#include <plugin/p_tempfile.h>
#include <plugin/p_tm.h>

#include "../TaskManager/i_fake_taskmanager.h"

#include "../POP3UnitTests/TestManager.h"
#include <string>

using namespace std;

#include <iface/i_taskex.h>
#include <plugin/p_pop3protocoller.h>




int main(int argc, char* argv[])
{
	srand(time(0));

	TestManager tm;

	tm.LoadPrague();

	tm.TestDD_Runner();
	//tm.TestMailProcess("..\\DataManager\\test50");

	return 0;
}

