#ifndef _bti_imp_h
#define _bti_imp_h

#include <Prague/prague.h>
#include <Prague/pr_oid.h>
#include <Prague/iface/e_ktime.h>
#include <Prague/iface/e_ktrace.h>
#include <Prague/iface/e_loader.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_mutex.h>
#include <Prague/iface/i_string.h>

#include <Extract/iface/i_btimages.h>
#include <Extract/plugin/p_windiskio.h>

#include "nfio/win32_io.h"

#define IMPEX_IMPORT_LIB
#include "Extract/plugin/p_windiskio.h"

extern hROOT g_root;

typedef struct _t_bti_data
{
	hSTRING		m_pRepositoryName;
	hIO			m_pRepositoryIO;
	cMutex*		m_pMutex;
}_bti_data;

tERROR bti_init(hSTRING hRepositoryName, hIO* pRepositoryIO);
tERROR bti_save(hIO pRepositoryIO, hIO llio);
tERROR bti_restore(hIO pRepositoryIO, hIO llio, tDWORD p_index);
tERROR bti_getimagedatetime(hIO pRepositoryIO, hIO llio, tDWORD p_index, tDATETIME* p_dt);

//+ ------------------------------------------------------------------------------------------
unsigned long CountCRC32(unsigned int size, unsigned char* ptr, unsigned long old_crc);
cMutex* bti_InitMutex(hOBJECT hbti, tDWORD dwRepositaryNameHash);

#endif  // _bti_imp_h