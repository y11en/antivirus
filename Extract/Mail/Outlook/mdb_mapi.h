//////////////////////////////////////////////////////////////////////////
// Header file for MAPI routine

#if !defined( MDB_MAPI_H )
#define MDB_MAPI_H

#include "cmn_mapi.h"
#include <Extract/iface/i_mailmsg.h>

#define MDB_INTERNAL_DATA

#include "mdb_os.h"
#include "mdb_io.h"
#include "mdb_objptr.h"

//////////////////////////////////////////////////////////////////////////
// object methods

typedef struct tag_MDBObj_Info {
	hOS            hOS;
	tSTRING		   profile;
	IMAPISession * session;
	IMsgStore *    store;
	IMAPIFolder *  folder;
	IMAPIProp *    object;
	tDWORD         type;
	LPSPropValue   entry;
	tSTRING		   name;
	tBOOL		   issubfolders;
} MDBObj_Info;

EXTERN_C tERROR	ObjInfoGet(MDBObj_Info *p_d, hOBJECT p_obj, hOBJECT p_name, tDWORD p_open_mode);
EXTERN_C tERROR	ObjInfoIOCreate(MDBObj_Info *p_d, hOS p_os, hIO *p_ret_val, tDWORD p_open_mode, tDWORD p_access_mode);
EXTERN_C tERROR	ObjInfoCopy(MDBObj_Info *p_src, MDBObj_Info *p_dst, tBOOL f_move, tBOOL f_overwrite);
EXTERN_C tERROR	ObjInfoDelete(MDBObj_Info *p_d);
EXTERN_C tERROR	ObjInfoGetProp(MDBObj_Info *p_d, tPROPID p_prop, tCHAR* p_buffer, tDWORD p_size, tDWORD* p_out_size);
EXTERN_C tERROR	ObjInfoGetName(MDBObj_Info *p_d, PRProp_Buff *p_buff);
EXTERN_C tERROR	ObjInfoGetPath(MDBObj_Info *p_d, PRProp_Buff *p_buff);

//////////////////////////////////////////////////////////////////////////
// session methods

EXTERN_C tERROR	SessionInitialize();
EXTERN_C tERROR	SessionDeinitialize();

EXTERN_C tERROR	SessionThreadAttach();

EXTERN_C tERROR	SessionCreate(MDB_Data *p_d);
EXTERN_C tERROR	SessionDestroy(MDB_Data *p_d);

#define MDB_DATA()		(CUST_TO_MDB_Data(p_d->hOS))

//////////////////////////////////////////////////////////////////////////
// enum methods
EXTERN_C tERROR	EnumCreate(MDBEnum_Data *p_d);
EXTERN_C tERROR	EnumDestroy(MDBEnum_Data *p_d);

EXTERN_C tERROR	EnumOpenTable(MDBEnum_Data *p_d);
EXTERN_C tERROR	EnumOpenObject(MDBEnum_Data *p_d);

EXTERN_C tERROR	EnumSetObject(MDBEnum_Data *p_d, MDBObj_Info *p_objinfo);
EXTERN_C tERROR	EnumGetObject(MDBEnum_Data *p_d, MDBObj_Info *p_objinfo, tBOOL p_openobject);
EXTERN_C tERROR	EnumClearObject(MDBEnum_Data *p_d);
EXTERN_C tERROR	EnumNextObject(MDBEnum_Data *p_d);
EXTERN_C tERROR	EnumFindObject(MDBEnum_Data *p_d, SPropValue *p_prop);
EXTERN_C tERROR	EnumCloneObject(MDBEnum_Data *p_d, MDBEnum_Data *p_src);

EXTERN_C tERROR	EnumReset(MDBEnum_Data *p_d);
EXTERN_C tERROR	EnumNext(MDBEnum_Data *p_d);
EXTERN_C tERROR	EnumStepUp(MDBEnum_Data *p_d);
EXTERN_C tERROR	EnumStepDown(MDBEnum_Data *p_d);
EXTERN_C tERROR	EnumChangeTo(MDBEnum_Data *p_d, hSTRING p_string, tDWORD p_open_mode);
EXTERN_C tERROR	EnumChangeToData(MDBEnum_Data *p_d, tCHAR *p_buffer, tDWORD p_size);

//////////////////////////////////////////////////////////////////////////
// message methods
EXTERN_C tERROR	MessageCreate(MDBIO_Data *p_d);
EXTERN_C tERROR	MessageDestroy(MDBIO_Data *p_d);

EXTERN_C tERROR	MessageSetObject(MDBIO_Data *p_d, MDBObj_Info *p_objinfo);
EXTERN_C tERROR	MessageGetObject(MDBIO_Data *p_d, MDBObj_Info *p_objinfo);

EXTERN_C tERROR	MessageLoadData(MDBIO_Data *p_d, tCHAR *p_buffer, tDWORD p_size);
EXTERN_C tERROR	MessageSaveData(MDBIO_Data *p_d, tDWORD* p_out_size, tCHAR *p_buffer, tDWORD p_size);

//////////////////////////////////////////////////////////////////////////

EXTERN_C tERROR	FolderGetOwner(MDBObj_Info *p_d, IMAPIFolder *p_obj, IMAPIFolder **p_owner);
EXTERN_C tERROR	FolderGetPath(MDBObj_Info *p_d, IMAPIFolder *p_obj, PRProp_Buff *p_buff, tBOOL b_root);

//////////////////////////////////////////////////////////////////////////

#endif // MDB_MAPI_H
