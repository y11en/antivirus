
#ifndef _REMOTE_MBL_H_
#define _REMOTE_MBL_H_

#include <windows.h>

#include <Prague/pr_remote.h>
#include <Prague/pr_loadr.h>
#include <Prague/pr_oid.h>
#include <Prague/iface/i_loader.h>
#include <Prague/iface/i_root.h>

#include <AV/iface/i_engine.h>

#include <Extract/plugin/p_msoe.h>

#include <Mail/iface/i_mailtask.h>
#include <Mail/plugin/p_mctask.h>
#include <Mail/structs/s_mc.h>


#pragma warning(push)
#pragma warning(disable:4786)
#include <vector>
#include <string>
#pragma warning(pop)

#undef PR_API
#ifdef __cplusplus
#define PR_API //extern "C"
#else
#define PR_API
#endif


extern PRRemoteAPI g_RP_API;
extern hROOT g_root;
extern tPROPID g_propMailerPID;
extern tPROPID g_propMailMessageOS_PID;
extern tPROPID g_propMessageIsIncoming;
extern tPROPID g_propCheckOnly;
extern tPROPID g_propVirtualName;

//#define npMAILMESSAGE_OS_PID		((tSTRING)("MAILMESSAGE_OS_PID")) // PID плагина, ответственного за парсинг процессируемого объекта
//#define npMAILER_PID				((tSTRING)("MAILER_PID")) 
//#define npMESSAGE_IS_INCOMING       ((tSTRING)("MESSAGE_IS_INCOMING")) // Флаг, выставляемый перехватчиком
//#define npCHECK_ONLY				((tSTRING)("CheckOnly")) // Флаг, который показывает, надо ли лечить переданный MC файл

typedef std::vector<std::string> PLUGIN_LIST;
tERROR RemoteMBLInit(HMODULE hModule, const PLUGIN_LIST& plugins_list);
tERROR RemoteMBLTerm();
hOBJECT RemoteMBLGet(tERROR* pERROR = NULL);

#endif//_REMOTE_MBL_H_