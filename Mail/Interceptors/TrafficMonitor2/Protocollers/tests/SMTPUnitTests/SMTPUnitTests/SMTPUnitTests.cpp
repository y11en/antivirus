#include <Prague/prague.h>
#include <Prague/iface/i_io.h>

#include "SMTPProtocoller\SMTPSessionAnalyzer.h"

#include "_aux.h"

#include <utils/prague/prague.hpp>
#include <plugin/p_tempfile.h>
#include <plugin/p_tm.h>

#include "../TaskManager/i_fake_taskmanager.h"

#include "TestManager.h"
#include <string>
using namespace std;


#include <iface/i_taskex.h>
#include <plugin/p_smtpprotocoller.h>

hROOT g_root;