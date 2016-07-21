#include "../IMAP4UnitTests/_aux.h"

#include <utils/prague/prague.hpp>
#include <plugin/p_tempfile.h>
#include <plugin/p_tm.h>

#include "../TaskManager/i_fake_taskmanager.h"

#include "../IMAP4UnitTests/TestManager.h"
#include <string>

using namespace std;

#include <iface/i_taskex.h>
#include <plugin/p_imapprotocoller.h>


#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	srand(time(0));

	TestManager tm;

	tm.LoadPrague();

	//tm.TestDD_Runner();
	
	//tm.TestMailProcess_Disinfect ("..\\DataManager\\test101");// - full
	//tm.TestMailProcess_Disinfect ("..\\DataManager\\test102");// - attachment

	//tm.TestMailProcess_Delete ("..\\DataManager\\test103");// - full
	//tm.TestMailProcess_Delete ("..\\DataManager\\test104");// - attachment
	

	return 0;
}

