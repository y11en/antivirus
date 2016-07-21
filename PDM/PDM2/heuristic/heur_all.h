#ifndef _HEUR_ALL_H
#define _HEUR_ALL_H

#include "../eventmgr/include/eventmgr.h"

#include "heur_base.h"
#include "heur_invader.h"
#include "heur_killav.h"
#include "heur_hosts.h"
#include "heur_killfiles.h"
#include "heur_virus.h"
#include "heur_install.h"
#include "heur_psw.h"
#include "heur_delself.h"
#include "../heuristic2/heur_dumb.h"
#include "../heuristic2/heur_emul.h"

static void RegisterHeuristicHandlers(cEventMgr& event_mgr)
{
	event_mgr.RegisterHandler(new cEventHandlerHeuristic,     true);
	event_mgr.RegisterHandler(new cEventHandlerHeurInvader,   true);
	event_mgr.RegisterHandler(new cEventHandlerHeurKillAV,    true);
	event_mgr.RegisterHandler(new cEventHandlerHeurHosts,     true);
	event_mgr.RegisterHandler(new cEventHandlerHeurKillFiles, true);
	event_mgr.RegisterHandler(new cEventHandlerHeurVirus,     true);
	event_mgr.RegisterHandler(new cEventHandlerHeurInstall,   true);
	event_mgr.RegisterHandler(new cEventHandlerHeurDumb,      true);
	event_mgr.RegisterHandler(new cEventHandlerHeurPsw,       true);
}

#endif // _HEUR_ALL_H
