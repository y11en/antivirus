//////////////////////////////////////////////////////////////////////////
// Header file for MAPI routine

#if !defined( MSG_MAPI_H )
#define MSG_MAPI_H

#include "cmn_mapi.h"

#include "msg_os.h"
#include "msg_io.h"
#include "msg_objptr.h"
#include "msg_tr.h"

//////////////////////////////////////////////////////////////////////////
// message content methods

#define MSGCNT_PLAIN_BODY		1
#define MSGCNT_RICH_BODY		2
#define MSGCNT_HTML_BODY		3
#define MSGCNT_ATTACH			4

#define PR_BODY_HTML			PROP_TAG(PT_TSTRING, 0x1013)
#define PR_INTERNET_MESSAGE_ID	PROP_TAG(PT_TSTRING, 0x1035)

#define PROCESS_CHUNCK_SIZE		1024

typedef struct tag_MsgObj_Info {
	hOS			   os;
	IMessage *     message;
	tDWORD         cntype;
	IAttach *      attach;
	tDWORD         attachnum;
} MsgObj_Info;

EXTERN_C tERROR	ObjInfoGet(MsgObj_Info *p_d, hOBJECT p_obj, hOBJECT p_name, tDWORD p_open_mode);
EXTERN_C tERROR	ObjInfoCopy(MsgObj_Info *p_src, MsgObj_Info *p_dst, tBOOL f_move, tBOOL f_overwrite);
EXTERN_C tERROR	ObjInfoIOCreate(MsgObj_Info *p_d, hOS p_os, hIO *p_ret_val, tDWORD p_open_mode, tDWORD p_access_mode);
EXTERN_C tERROR	ObjInfoDelete(MsgObj_Info *p_d);
EXTERN_C tERROR	ObjInfoGetProp(MsgObj_Info *p_d, tPROPID p_prop, tCHAR* p_buffer, tDWORD p_size, tDWORD* p_out_size);
EXTERN_C tERROR	ObjInfoSetProp(MsgObj_Info *p_d, tPROPID p_prop, tCHAR* p_buffer, tDWORD p_size, tDWORD* p_out_size);
EXTERN_C tERROR	ObjInfoGetName(MsgObj_Info *p_d, PRProp_Buff *p_buff);
EXTERN_C tERROR	ObjInfoGetPath(MsgObj_Info *p_d, PRProp_Buff *p_buff);
EXTERN_C tERROR	ObjInfoGetFullName(MsgObj_Info *p_d, PRProp_Buff *p_buff);

//////////////////////////////////////////////////////////////////////////
// message methods
EXTERN_C tERROR	MessageCreate(Msg_Data *p_d);
EXTERN_C tERROR	MessageDestroy(Msg_Data *p_d);

EXTERN_C tERROR	MessageGetProp(Msg_Data *p_d, tPROPID p_prop, tCHAR* p_buffer, tDWORD p_size, tDWORD* p_out_size);
EXTERN_C tERROR	MessageSetProp(Msg_Data *p_d, tPROPID p_prop, tCHAR* p_buffer, tDWORD p_size, tDWORD* p_out_size);

EXTERN_C tERROR	MessageCommit(Msg_Data *p_d);
EXTERN_C tERROR	MessageChange(Msg_Data *p_d);

//////////////////////////////////////////////////////////////////////////
// enum methods
EXTERN_C tERROR	EnumCreate(MsgEnum_Data *p_d, IMessage *p_message);
EXTERN_C tERROR	EnumDestroy(MsgEnum_Data *p_d);

EXTERN_C tERROR	EnumSetObject(MsgEnum_Data *p_d, MsgObj_Info *p_objinfo);
EXTERN_C tERROR	EnumGetObject(MsgEnum_Data *p_d, MsgObj_Info *p_objinfo);

EXTERN_C tERROR	EnumOpenTable(MsgEnum_Data *p_d);
EXTERN_C tERROR	EnumNextContent(MsgEnum_Data *p_d);
EXTERN_C tERROR	EnumFindAttach(MsgEnum_Data *p_d, SPropValue *p_prop);

EXTERN_C tERROR	EnumReset(MsgEnum_Data *p_d);
EXTERN_C tERROR	EnumNext(MsgEnum_Data *p_d);

EXTERN_C tERROR	EnumChangeTo(MsgEnum_Data *p_d, hSTRING p_string, tDWORD p_open_mode);
EXTERN_C tERROR	EnumChangeToData(MsgEnum_Data *p_d, tCHAR *p_buffer, tDWORD p_size);

//////////////////////////////////////////////////////////////////////////
// content methods
EXTERN_C tERROR	ContentCreate(MsgIO_Data *p_d, IMessage *p_message);
EXTERN_C tERROR	ContentDestroy(MsgIO_Data *p_d);

EXTERN_C tERROR	ContentGetEmbMessage(MsgIO_Data *p_d, IMessage **p_message);

EXTERN_C tERROR	ContentSetObject(MsgIO_Data *p_d, MsgObj_Info *p_objinfo);
EXTERN_C tERROR	ContentGetObject(MsgIO_Data *p_d, MsgObj_Info *p_objinfo);

EXTERN_C tERROR	ContentSetMode(MsgIO_Data *p_d, tDWORD p_mode);
EXTERN_C tERROR	ContentStreamOpen(MsgIO_Data *p_d);
EXTERN_C tERROR	ContentStreamClose(MsgIO_Data *p_d);

EXTERN_C tERROR	ContentSeekRead(MsgIO_Data *p_d, tQWORD p_offset, tPTR p_buffer, tDWORD p_size, tDWORD* p_result);
EXTERN_C tERROR	ContentSeekWrite(MsgIO_Data *p_d, tQWORD p_offset, tPTR p_buffer, tDWORD p_size, tDWORD* p_result);
EXTERN_C tERROR	ContentGetSize(MsgIO_Data *p_d, IO_SIZE_TYPE p_type, tQWORD* p_result);
EXTERN_C tERROR	ContentSetSize(MsgIO_Data *p_d, tQWORD p_new_size);
EXTERN_C tERROR	ContentFlush(MsgIO_Data *p_d);
EXTERN_C tERROR	ContentCommit(MsgIO_Data *p_d);
EXTERN_C tERROR	ContentChange(MsgIO_Data *p_d);
EXTERN_C tERROR	ContentCopy(MsgIO_Data *p_d, tBOOL dir);

EXTERN_C tERROR	ContentLoadData(MsgIO_Data *p_d, tCHAR *p_buffer, tDWORD p_size);
EXTERN_C tERROR	ContentSaveData(MsgIO_Data *p_d, tDWORD* p_out_size, tCHAR *p_buffer, tDWORD p_size);

//////////////////////////////////////////////////////////////////////////
// transformer methods

EXTERN_C tERROR	TransformerProcessChunck(MsgTR_Data *p_d, tQWORD p_size, tQWORD* p_result);
EXTERN_C tERROR	TransformerProcess(MsgTR_Data *p_d, tQWORD* p_result);
EXTERN_C tERROR	TransformerGetSize(MsgTR_Data *p_d, tQWORD* p_result);

//////////////////////////////////////////////////////////////////////////

#endif // MSG_MAPI_H
