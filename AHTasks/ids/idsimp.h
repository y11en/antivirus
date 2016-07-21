#ifndef __idsimp__
#define __idsimp__

#include <Windows.h>

//+ ------------------------------------------------------------------------------------------


#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_reg.h>

#include <AHTasks/plugin/p_idstask.h>
#include <AHTasks/structs/s_ids.h>

//+ ------------------------------------------------------------------------------------------
#include <CKAH/CKAHUM.h>
using namespace CKAHIDS;

void CALLBACK cbAttacksNotify(LPVOID lpContext, const CKAHIDS::AttackNotify *pAttackNotify);
void CALLBACK idsTrace(CKAHUM::LogLevel Level, LPCSTR szString);

//+ ------------------------------------------------------------------------------------------

typedef struct _t_idstask
{
	hOBJECT		m_hThis;
	tTaskState	m_TaskState;
	tDWORD		m_TaskID;
	hOBJECT		m_hBL;

	// internal data
	cIDSStatistics	m_Statistics;
	cIDSSettings	m_Settings;
}_idstask;

#endif // __idsimp__