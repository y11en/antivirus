#ifndef _CCLIENT_TASKSTAT_H__INCLUDED_
#define _CCLIENT_TASKSTAT_H__INCLUDED_

// Task state defines

#pragma once

enum ETaskState
{
	TS_UNKNOWN	= 0,		// unknown
	TS_RUNNING	= 1,		// task is running
	TS_DONE		= 2,		// done
	TS_FAILED	= 3,		// failed
	TS_ABORTED	= 4,		// aborted by user
	TS_PAUSED	= 5,		// paused by user
	TS_STARTING	= 6,		// task is starting
	TS_STOPING	= 7,		// task is stoping
	TS_ERREXEC	= 8,		// Error extecuting
	TS_RESTARTING	= 9,	// Restarting task
};

#endif // _CCLIENT_TASKSTAT_H__INCLUDED_