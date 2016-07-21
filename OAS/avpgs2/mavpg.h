/*
 *  mavpg.h
 *  
 *
 *  Created by Rustam Muginov on 14.08.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MAVPG_H_
#define _MAVPG_H_

#include "avpgimpl.h"
#include <pthread.h>
#include <Prague/iface/i_threadpool.h>
#include <Prague/plugin/p_thpoolimp.h>

class cMacAvpgImpl: public cAvpgImpl
{
public:
	cMacAvpgImpl( hOBJECT pPragueContext, tAVPG2_WORKING_MODE WorkingMode, tDWORD ThreadMaxCount );
	~cMacAvpgImpl();

	tERROR SetActivityMode ( tAVPG2_CLIENT_ACTIVITY_MODE ActivityMode );
	tERROR ResetDriverCache();
	tERROR CreateIO( hOBJECT* p_phObject, hSTRING p_hObjectName );

	virtual tERROR YieldEvent( hOBJECT EventObject, tDWORD YieldTimeout );
	virtual tERROR SetPreverdict( hOBJECT EventObject, tAVPG2_PROCESS_OBJECT_RESULT_FLAGS ResultFlags );


//	tERROR SetActivityMode ( tAVPG2_CLIENT_ACTIVITY_MODE ActivityMode );
//	tERROR ResetDriverCache();
//	tERROR CreateIO( hOBJECT* p_phObject, hSTRING p_hObjectName );
//
//public:
//	PVOID m_pDrvContext;
//	cAvpgImpl* m_pInstance;

protected:
	pthread_t m_ThreadID;
	bool m_RunningAsRoot; 
	bool m_Inited;
	cThreadPool *m_ThreadPool;

	static void* s_scan_events_thread( void *param );
	static void* s_scan_events_thread_fake( void *param );
	static void s_file_interceptor_callback ( const char * filename, long objectid, long uid, long pid, int action, void *context );

	void scan_events_thread();
	void scan_events_thread_fake();
	void file_interceptor_callback ( const char * filename, long objectid, long uid, long pid, int action);


};


#endif // _MAVPG_H_
