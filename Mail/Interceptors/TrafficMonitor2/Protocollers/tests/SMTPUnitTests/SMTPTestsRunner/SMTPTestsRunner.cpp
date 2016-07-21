#include <Prague/prague.h>
#include <Prague/iface/i_io.h>

#include "SMTPProtocoller\SMTPSessionAnalyzer.h"

#include "../SMTPUnitTests/_aux.h"

#include <utils/prague/prague.hpp>
#include <plugin/p_tempfile.h>
#include <plugin/p_tm.h>

#include "../TaskManager/i_fake_taskmanager.h"

#include "../SMTPUnitTests/TestManager.h"
#include <string>

using namespace std;

#include <iface/i_taskex.h>
#include <plugin/p_smtpprotocoller.h>




int main(int argc, char* argv[])
{
	srand(time(0));

	TestManager tm;

	tm.LoadPrague();

	tm.TestDD_Runner();
	//tm.TestMailProcess_WithVirus("..\\DataManager\\test50", 9);
	//tm.TestMailProcess_WithVirus("..\\DataManager\\test51", 17);

	return 0;
}

