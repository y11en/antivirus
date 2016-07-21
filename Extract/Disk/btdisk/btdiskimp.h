#ifndef __bt_disk__
#define __bt_disk__

#include "Prague/pr_cpp.h"
#include "Prague/pr_err.h"
#include "Prague/pr_oid.h"

#include "Extract/plugin/p_windiskio.h"

#include "cd.h"

#define cNAME_MAX_LEN	(1024 * 32)

//+ ------------------------------------------------------------------------------------------

typedef struct _t_bt_reopen_data
{
	tCHAR		m_DrvLetter;
	tDWORD		m_BootAreaIdx;
}_bt_reopen_data;
//+ ------------------------------------------------------------------------------------------

//+ ------------------------------------------------------------------------------------------

typedef struct _t_os_btdiskdata
{
	cString*		m_pObjectName;
	tQWORD			m_Hash;
}_os_btdiskdata;

//+ ------------------------------------------------------------------------------------------
typedef struct _t_disk_descr
{
	__uint32		m_load_rba;
	__uint16		m_sector_count;
	__uint8			m_platform_id;
}_disk_descr;

typedef struct _t_ptr_btdiskdata
{
	cIO*			m_pDiskIO;
	cString*		m_pObjectName;
	cString*		m_pObjectPath;
	tObjPtrState	m_PtrState;
	
	tDWORD			m_BootAreaIdxNext;
	_disk_descr		m_disk_descr;
}_ptr_btdiskdata;

//+ ------------------------------------------------------------------------------------------

typedef struct _t_io_btdiskdata
{
	cString*		m_pObjectName;
	cString*		m_pObjectPath;
	cIO*			m_pDiskIO;

	tDWORD			m_BootAreaIdx;
	_disk_descr		m_disk_descr;

}_io_btdiskdata;

//+ ------------------------------------------------------------------------------------------

tBOOL _get_bootarea_sector_by_idx(cIO* pDiskIo, tDWORD bt_idx, _disk_descr* pdisk_descr);

#endif // __bt_disk__