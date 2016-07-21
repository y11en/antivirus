/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	klcsc.h
 * \author	Andrew Kazachkov ( Damir Shiyafetdinov ( 08.08.05 )
 * \date	26.11.2002 18:55:48
 * \brief	Главный заголовочный файл библиотеки KLCSC ( используется для сборки под development2 )
 * 
 */


#ifndef KLCSC_H_3F512D6C_C58C_4f39_B336_106D54398172
#define KLCSC_H_3F512D6C_C58C_4f39_B336_106D54398172

#include <build/general.h>
#include <std/base/klstd.h>
#include <std/err/error.h>
#include <std/par/params.h>
#include <std/par/errors.h>
#include <kca/prss/settingsstorage.h>
#include <kca/prci/componentinstance.h>
#include <kca/prci/simple_params.h>
#include <kca/prcp/agentproxy.h>
#include <kca/prts/tasksstorage.h>
#include <kca/prss/errors.h>
#include <kca/prts/prxsmacros.h>
#include <kca/pres/eventsstorage.h>
#include <std/sch/common.h>
#include <std/sch/errors.h>
#include <std/sch/everydayschedule.h>
#include <std/sch/everymonthschedule.h>
#include <std/sch/everyweekschedule.h>
#include <std/sch/millisecondsschedule.h>
#include <std/sch/schedule.h>
#include <std/sch/scheduler.h>
#include <std/sch/task.h>
#include <std/sch/taskparams.h>
#include <std/sch/taskresults.h>
#include <std/sch/tasksiterator.h>
#include <std/thr/thread.h>
#include <std/tp/threadspool.h>

#ifdef _STLP_NEW_PLATFORM_SDK
#include <clocale>
#define LC_CTYPE 2
#endif

#ifdef _WIN32
#pragma warning( disable : 4290 )
#endif

void KLCSC_Initialize();
void KLCSC_Deinitialize();

#endif // KLCSC_H_3F512D6C_C58C_4f39_B336_106D54398172
