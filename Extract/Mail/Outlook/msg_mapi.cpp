//////////////////////////////////////////////////////////////////////////
// Source file for MAPI routine

#define INITGUID
#define USES_IID_IStreamDocfile
#define USES_IID_IMessage

#include "msg_mapi.h"
#include "mdb_os.h"

#include <Prague/iface/i_buffer.h>
#include <Prague/iface/i_hash.h>
#include <Prague/plugin/p_hash_md5.h>


#include <string>
using std::string;

#define PR_SMTP_ADDRESS 0x39FE001E

//////////////////////////////////////////////////////////////////////////
// object methods

tERROR	ObjInfoGet(MsgObj_Info *p_d, hOBJECT p_obj, hOBJECT p_name, tDWORD p_open_mode)
{
	memset(p_d, 0, sizeof(MsgObj_Info));
	if( !p_name )
		return errOK;

	tIID l_iid = CALL_SYS_PropertyGetDWord(p_name, pgINTERFACE_ID);
	switch( l_iid )
	{
	case IID_OBJPTR: return EnumGetObject(CUST_TO_MsgEnum_Data(p_name), p_d);
	case IID_IO:	 return ContentGetObject(CUST_TO_MsgIO_Data(p_name), p_d);
	}

	tIID l_obj_iid = CALL_SYS_PropertyGetDWord(p_obj, pgINTERFACE_ID);

	hOS l_os = NULL;
	switch(l_obj_iid)
	{
	case IID_OS:	 l_os = (hOS)p_obj; break;
	case IID_OBJPTR: l_os = CUST_TO_MsgEnum_Data(p_obj)->hOS; break;
	default: return errUNEXPECTED;
	}

	hObjPtr &l_objptr = CUST_TO_Msg_Data(l_os)->tmpenum;

	if( !l_objptr )
		CALL_SYS_ObjectCreateQuick(l_os, &l_objptr, IID_OBJPTR, PID_MAILMSG, 0);

	MsgEnum_Data *l_enum = CUST_TO_MsgEnum_Data(l_objptr);

	tERROR l_error = errNOT_IMPLEMENTED;

	if( l_iid == IID_STRING )
	{
		l_error = EnumReset(l_enum);
		if( PR_SUCC(l_error) )
			l_error = EnumChangeTo(l_enum, (hSTRING)p_name, p_open_mode);
	}
	else if( l_iid == IID_BUFFER )
	{
		tQWORD l_size;
		CALL_IO_GetSize((hIO)p_name, &l_size, IO_SIZE_TYPE_EXPLICIT);

		tPTR l_buffer;
		CALL_Buffer_Lock((hBUFFER)p_name, &l_buffer);

		l_error = EnumChangeToData(l_enum, (tCHAR*)l_buffer, (tDWORD)l_size);

		CALL_Buffer_Unlock((hBUFFER)p_name);
	}

	if( PR_SUCC(l_error) )
		l_error = EnumGetObject(l_enum, p_d);

	return l_error;
}

tERROR	ObjInfoIOCreate(MsgObj_Info *p_d, hOS p_os, hIO *p_ret_val, tDWORD p_open_mode, tDWORD p_access_mode)
{
	*p_ret_val = NULL;

	hOBJECT l_io = NULL;
	tERROR l_error = CALL_SYS_ObjectCreate(p_os, &l_io, IID_IO, PID_MAILMSG, 0);
	if( PR_FAIL(l_error) )
		return l_error;

	ContentSetObject(CUST_TO_MsgIO_Data(l_io), p_d);

	CALL_SYS_PropertySetDWord(l_io, pgOBJECT_OPEN_MODE, p_open_mode);
	l_error = CALL_SYS_PropertySetDWord(l_io, pgOBJECT_ACCESS_MODE, p_access_mode);
	if( PR_SUCC(l_error) )
		l_error = CALL_SYS_ObjectCreateDone(l_io);

	if( PR_FAIL(l_error) && l_io )
		CALL_SYS_ObjectClose(l_io);
	else
		*p_ret_val = (hIO)l_io;
	return l_error;
}

tERROR	ObjInfoGetProp(MsgObj_Info *p_d, tPROPID p_prop, tCHAR* p_buffer, tDWORD p_size, tDWORD* p_out_size)
{
	tERROR l_error;
	PRProp_Buff l_buff = {p_buffer, p_size, tid_VOID, 0};

	switch( p_prop )
	{
		case pgMESSAGE_PART_DISPLAY_NAME:
		case pgOBJECT_NAME:
			l_error = ObjInfoGetName(p_d, &l_buff);
			break;
		case pgMESSAGE_PART_FILE_NAME:
			if( p_d->cntype == MSGCNT_ATTACH )
			{
				l_error = ObjectGetProp(p_d->attach, PR_ATTACH_LONG_FILENAME, tid_CHAR, &l_buff);
				if( l_error == errPROPERTY_NOT_FOUND )
					l_error = ObjectGetProp(p_d->attach, PR_ATTACH_FILENAME, tid_CHAR, &l_buff);
			}
			else
				return errPROPERTY_NOT_FOUND;
			break;
		case pgMESSAGE_PART_CONTENT_TYPE:
			if( p_d->cntype == MSGCNT_ATTACH )
				l_error = ObjectGetProp(p_d->attach, PR_ATTACH_MIME_TAG, tid_CHAR, &l_buff);
			else
				return errPROPERTY_NOT_FOUND;
			break;
		case pgOBJECT_PATH:
			l_error = ObjInfoGetPath(p_d, &l_buff);
			break;
		case pgOBJECT_FULL_NAME:
			l_error = ObjInfoGetFullName(p_d, &l_buff);
			break;
		case pgOBJECT_SIZE:
			if( p_d->cntype == MSGCNT_ATTACH )
				l_error = ObjectGetProp(p_d->attach, PR_ATTACH_SIZE, tid_DWORD, &l_buff);
			else
				l_error = PRBuffSetDWORD(&l_buff, 0);
			break;
		case pgOBJECT_SIZE_Q:
			if( p_d->cntype == MSGCNT_ATTACH )
				l_error = ObjectGetProp(p_d->attach, PR_ATTACH_SIZE, tid_QWORD, &l_buff);
			else
				l_error = PRBuffSetQWORD(&l_buff, 0);
			break;
		default: return errPROPERTY_NOT_FOUND;
	}

	if( p_out_size )
		*p_out_size = l_buff.out_size;

	return l_error;
}

tERROR	ObjInfoSetProp(MsgObj_Info *p_d, tPROPID p_prop, tCHAR* p_buffer, tDWORD p_size, tDWORD* p_out_size)
{
	tERROR l_error;
	PRProp_Buff l_buff = {p_buffer, p_size, tid_VOID, 0};

	switch( p_prop )
	{
		case pgMESSAGE_PART_FILE_NAME:
			if( p_d->cntype == MSGCNT_ATTACH )
			{
				l_error = ObjectSetProp(p_d->attach, PR_DISPLAY_NAME, tid_CHAR, &l_buff);
				l_error = ObjectSetProp(p_d->attach, PR_ATTACH_FILENAME, tid_CHAR, &l_buff);
				l_error = ObjectSetProp(p_d->attach, PR_ATTACH_LONG_FILENAME, tid_CHAR, &l_buff);
			}
			else
				return errPROPERTY_NOT_FOUND;
			break;
		default: return errPROPERTY_NOT_FOUND;
	}
	return l_error;
}

tERROR	ObjInfoGetName(MsgObj_Info *p_d, PRProp_Buff *p_buff)
{
	if( p_d->cntype == MSGCNT_ATTACH )
	{
		tERROR l_error = ObjectGetProp(p_d->attach, PR_DISPLAY_NAME, tid_CHAR, p_buff);
		if( l_error == errPROPERTY_NOT_FOUND )
			l_error = ObjectGetProp(p_d->attach, PR_ATTACH_LONG_FILENAME, tid_CHAR, p_buff);
		if( l_error == errPROPERTY_NOT_FOUND )
			l_error = ObjectGetProp(p_d->attach, PR_ATTACH_FILENAME, tid_CHAR, p_buff);
		return l_error;
	}

	tCHAR *l_name = NULL;
	switch( p_d->cntype )
	{
	case MSGCNT_PLAIN_BODY:	l_name = MSG_CONTENT_PLAIN_BODY; break;
	case MSGCNT_RICH_BODY:	l_name = MSG_CONTENT_RICH_BODY; break;
	case MSGCNT_HTML_BODY:	l_name = MSG_CONTENT_HTML_BODY; break;
	default : return errPROPERTY_NOT_FOUND;
	}
	return PRBuffSetSTRING(p_buff, l_name);
}

tERROR	ObjInfoGetPath(MsgObj_Info *p_d, PRProp_Buff *p_buff)
{
	return PRBuffSetNULL(p_buff);
}

tERROR	ObjInfoGetFullName(MsgObj_Info *p_d, PRProp_Buff *p_buff)
{
	tERROR l_error;
	if( PR_FAIL(l_error = ObjInfoGetPath(p_d, p_buff)) )
		return l_error;

	return ObjInfoGetName(p_d, p_buff);
}

tERROR	ObjInfoCopy(MsgObj_Info *p_src, MsgObj_Info *p_dst, tBOOL f_move, tBOOL f_overwrite)
{
	return errNOT_IMPLEMENTED;
}

tERROR	ObjInfoDelete(MsgObj_Info *p_d)
{
	HRESULT l_result = hrSuccess;
	ULONG l_body_prop = 0;
	switch( p_d->cntype )
	{
	case MSGCNT_ATTACH:
		l_result = p_d->message->DeleteAttach(p_d->attachnum, 0, NULL, 0);
		break;
	case MSGCNT_PLAIN_BODY: l_body_prop = PR_BODY; break;
	case MSGCNT_RICH_BODY:	l_body_prop = PR_RTF_COMPRESSED; break;
	case MSGCNT_HTML_BODY:	l_body_prop = PR_BODY_HTML; break;
	default : return errPARAMETER_INVALID;
	}

	if( l_body_prop )
	{
		SizedSPropTagArray(1, sptaColumns) = {1, {l_body_prop}};
		l_result = p_d->message->DeleteProps((LPSPropTagArray)&sptaColumns, NULL);
	}

	if( HR_FAILED(l_result) )
		return l_result == E_ACCESSDENIED ? errACCESS_DENIED : errOBJECT_INVALID;

	return MessageChange(CUST_TO_Msg_Data(p_d->os));
}

//////////////////////////////////////////////////////////////////////////
// message methods

tERROR	MessageCreate(Msg_Data *p_d)
{
	if( p_d->isembended )
		return ContentGetEmbMessage(CUST_TO_MsgIO_Data(p_d->hIO), &p_d->message);

	if( !p_d->message )
		return errOBJECT_NOT_CREATED;

	MAPI_IFACE_ADDREF(p_d->message, message);
	return errOK;
}

tERROR	MessageDestroy(Msg_Data *p_d)
{
	MAPI_IFACE_RELEASE(p_d->message, message);
	return errOK;
}

tERROR	MessageGetRecipients(Msg_Data *p_d, PRProp_Buff *p_buff, ULONG p_type)
{
	IMAPITable *l_table = NULL;
	HRESULT hr = S_OK;

	if( FAILED(hr=p_d->message->GetRecipientTable(0, &l_table)) || !l_table )
		return errPROPERTY_NOT_FOUND;

	SizedSPropTagArray(4, sptaColumns) = {4, {PR_RECIPIENT_TYPE, PR_DISPLAY_NAME, PR_ADDRTYPE, PR_EMAIL_ADDRESS}};

	LPSRowSet l_rows = NULL;
	if( FAILED(hr=HrQueryAllRows(l_table, (LPSPropTagArray)&sptaColumns, NULL, NULL, 0, &l_rows )) || !l_rows )
	{
		l_table->Release();
		return errPROPERTY_NOT_FOUND;
	}

	tERROR l_error = errOK;
	for(ULONG i = 0; i < l_rows->cRows; i++)
	{
		LPSPropValue l_props = l_rows->aRow[i].lpProps;
		if( 
			(l_props[0].Value.ul != p_type) ||
			(l_props[1].Value.ul == MAPI_E_NOT_FOUND) ||
			(l_props[2].Value.ul == MAPI_E_NOT_FOUND) ||
			(l_props[3].Value.ul == MAPI_E_NOT_FOUND) 
			)
			continue;

		PRBuffSetCHAR(p_buff, '\"');
		PRBuffSetSTRING(p_buff, l_props[1].Value.lpszA);
		PRBuffSetSTRING(p_buff, "\" <");
		PRBuffSetSTRING(p_buff, l_props[2].Value.lpszA);
		PRBuffSetCHAR(p_buff, ':');
		PRBuffSetSTRING(p_buff, l_props[3].Value.lpszA);
		PRBuffSetCHAR(p_buff, '>');

		if( PR_FAIL(l_error = PRBuffSetSTRING(p_buff, "; ")) )
			break;
	}

	l_table->Release();
	FreeProws(l_rows);
	return l_error;
}

tERROR MessageGetAddress(Msg_Data * p_d, ULONG prop, PRProp_Buff *p_buff)
{
	if (!p_d || !p_buff || !p_d->hIO)
		return errPARAMETER_INVALID;

	// Извлекаем имя профиля
	char buff[MAX_PATH];
	tDWORD size = 0;
	CALL_SYS_PropertyGetStr(p_d->hIO, &size, pgOBJECT_PATH, buff, MAX_PATH, cCP_ANSI);
	string path = buff;
	string profile;

	tDWORD pos = path.find_first_of("\\");

	if (!path.empty())
	{
		if (string::npos != pos)
		{
			profile = path.substr(0, pos);
		}
		else
		{
			profile = path;
		}
	}
	
	// получаем идентификатор записи отправителя 
	// и по данной записи находим его SMTP адрес
	
	tERROR l_error = errNOT_FOUND;

	LPSPropValue pPropVal = NULL;
	
	if (PR_SUCC(ObjectGetSProp(p_d->message, prop, &pPropVal)))
	{
		SBinary binary = pPropVal->Value.bin;
		LPMAPISESSION pSession = NULL;
		if (SUCCEEDED(MAPILogonEx(0, (LPTSTR)profile.c_str(), "", MAPI_NEW_SESSION | MAPI_EXTENDED | MAPI_ALLOW_OTHERS | MAPI_EXPLICIT_PROFILE , &pSession)))
		{
			LPADRBOOK pAddrBook = NULL;
			if (SUCCEEDED(pSession->OpenAddressBook(0, NULL, AB_NO_DIALOG, &pAddrBook)))
			{
				ULONG ulObjType = 0;
				LPUNKNOWN pUnk = NULL;
				if (SUCCEEDED(pAddrBook->OpenEntry(binary.cb, (LPENTRYID)binary.lpb, NULL, MAPI_BEST_ACCESS, &ulObjType, &pUnk)))
				{
					LPMAPIPROP pMapiProp = NULL;
					switch (ulObjType)
					{
					case MAPI_ABCONT:
						pMapiProp = (LPABCONT)pUnk;
						break;
					case MAPI_MAILUSER:
						pMapiProp = (LPMAILUSER)pUnk;
						break;
					case MAPI_DISTLIST:
						pMapiProp = (LPDISTLIST)pUnk;
						break;
					default:
						if (pUnk)
							pUnk->Release();
					}

					if (pMapiProp)
					{
						LPSPropValue pProp = NULL;
						if (S_OK == HrGetOneProp(pMapiProp, PR_SMTP_ADDRESS, &pProp))
						{
							PRBuffSetSTRING(p_buff, pProp->Value.lpszA ? pProp->Value.lpszA : "");
							l_error = errOK;
							MAPIFreeBuffer(pProp);
						}
						pMapiProp->Release();
						pMapiProp = NULL;
					}
					
				}
				pAddrBook->Release();
				pAddrBook = NULL;
			}
			pSession->Release();
			pSession = NULL;
		}
		MAPIFreeBuffer(pPropVal);
		pPropVal = NULL;
	}

	return l_error;
}

tERROR	MessageGetHash(Msg_Data *p_d, PRProp_Buff *p_buff)
{
	tBYTE  l_data[0x100];
	tBYTE  l_hash_data[32];
	hHASH  l_hash_obj = NULL;
	tQWORD l_hash = 0;

	PRProp_Buff l_buff = {(tCHAR*)l_data, sizeof(l_data), tid_PTR, 0};

	tERROR error = ObjectGetProp(p_d->message, PR_ENTRYID, tid_PTR, &l_buff);
	if( PR_SUCC(error) )
		error = ObjectGetProp(p_d->message, PR_LAST_MODIFICATION_TIME, tid_DATETIME, &l_buff);

	if( PR_SUCC(error) )
		error = CALL_SYS_ObjectCreateQuick(g_root, &l_hash_obj, IID_HASH, PID_HASH_MD5, SUBTYPE_ANY);

	if( PR_SUCC(error) )
		error = CALL_Hash_Update(l_hash_obj, l_data, l_buff.out_size);

	if( PR_SUCC(error) )
		error = CALL_Hash_GetHash(l_hash_obj, l_hash_data, sizeof(l_hash_data));

	if( l_hash_obj )
		CALL_SYS_ObjectClose(l_hash_obj);

	if( PR_SUCC(error) )
	{
		l_hash = *(tQWORD*)(&l_hash_data[0]);
		l_hash^= *(tQWORD*)(&l_hash_data[8]);
		error = PRBuffSetQWORD(p_buff, l_hash);
	}

	return error;
}

tERROR	MessageGetXProp(Msg_Data *p_d, PRProp_Buff *p_buff, ULONG p_prop_name, ULONG p_addr_type, ULONG p_email_addr)
{
	PRBuffSetCHAR(p_buff, '\"');
	ObjectGetProp(p_d->message, p_prop_name, tid_CHAR, p_buff);
	PRBuffSetSTRING(p_buff, "\" <");
	ObjectGetProp(p_d->message, p_addr_type, tid_CHAR, p_buff);
	PRBuffSetCHAR(p_buff, ':');
	ObjectGetProp(p_d->message, p_email_addr, tid_CHAR, p_buff);
	return PRBuffSetCHAR(p_buff, '>');
}

tERROR	MessageGetProp(Msg_Data *p_d, tPROPID p_prop, tCHAR* p_buffer, tDWORD p_size, tDWORD* p_out_size)
{
	tERROR l_error = errPROPERTY_NOT_FOUND;
	PRProp_Buff l_buff = {p_buffer, p_size, tid_STRING, 0};

	switch( p_prop )
	{
	case pgOBJECT_NAME:
		l_error = MessageGetName(p_d->message, &l_buff);
		break;
	case pgOBJECT_HASH:
		l_error = MessageGetHash(p_d, &l_buff);
		break;
	case pgMESSAGE_FROM:
		l_error = MessageGetXProp(p_d, &l_buff, PR_SENDER_NAME, PR_SENDER_ADDRTYPE, PR_SENDER_EMAIL_ADDRESS);
		break;		
	case pgMESSAGE_FROM_SMTP:
		l_error = MessageGetAddress(p_d, PR_SENDER_ENTRYID, &l_buff);
		break;
	case pgMESSAGE_ORIGINAL_FROM:
		l_error = MessageGetXProp(p_d, &l_buff, PR_SENT_REPRESENTING_NAME, PR_SENT_REPRESENTING_ADDRTYPE, PR_SENT_REPRESENTING_EMAIL_ADDRESS);
		break;
	case pgMESSAGE_DISPLAY_FROM:
		l_error = ObjectGetProp(p_d->message, PR_SENDER_NAME, tid_CHAR, &l_buff);
		break;
	case pgMESSAGE_ORIGINAL_DISPLAY_FROM:
		l_error = ObjectGetProp(p_d->message, PR_SENT_REPRESENTING_NAME, tid_CHAR, &l_buff);
		break;
	case pgMESSAGE_TO:
		l_error = MessageGetRecipients(p_d, &l_buff, MAPI_TO);
		break;
	case pgMESSAGE_DISPLAY_TO:
		l_error = ObjectGetProp(p_d->message, PR_DISPLAY_TO, tid_CHAR, &l_buff);
		break;
	case pgMESSAGE_CC:
		l_error = MessageGetRecipients(p_d, &l_buff, MAPI_CC);
		break;
	case pgMESSAGE_DISPLAY_CC:
		l_error = ObjectGetProp(p_d->message, PR_DISPLAY_CC, tid_CHAR, &l_buff);
		break;
	case pgMESSAGE_SUBJECT:
		l_error = ObjectGetProp(p_d->message, PR_SUBJECT, tid_CHAR, &l_buff);
		break;
	case pgMESSAGE_DATE:
		l_error = ObjectGetProp(p_d->message, PR_CLIENT_SUBMIT_TIME, tid_DATETIME, &l_buff);
		if( l_error == errPROPERTY_NOT_FOUND )
			l_error = ObjectGetProp(p_d->message, PR_PROVIDER_SUBMIT_TIME, tid_DATETIME, &l_buff);
		break;
	case pgMESSAGE_MAILID:
		l_error = ObjectGetProp(p_d->message, PR_INTERNET_MESSAGE_ID, tid_CHAR, &l_buff);
		break;
	case pgMESSAGE_INTERNET_HEADERS:
		l_error = ObjectGetProp(p_d->message, PR_TRANSPORT_MESSAGE_HEADERS, tid_CHAR, &l_buff);
		break;
	case pgMESSAGE_MIME_VERSION:
	case pgMESSAGE_RETURN_PATH:
	default:
		return errPROPERTY_NOT_FOUND;
	}

	if( p_out_size )
		*p_out_size = l_buff.out_size;

	return l_error;
}

tERROR	MessageSetProp(Msg_Data *p_d, tPROPID p_prop, tCHAR* p_buffer, tDWORD p_size, tDWORD* p_out_size)
{
	tERROR l_error;
	PRProp_Buff l_buff = {p_buffer, p_size, tid_STRING, 0};

	switch( p_prop )
	{
	case pgMESSAGE_SUBJECT:
		l_error = ObjectSetProp(p_d->message, PR_SUBJECT, tid_CHAR, &l_buff);
		break;
	default:
		return errPROPERTY_NOT_FOUND;
	}
	if( PR_SUCC(l_error) )
		MessageChange(p_d);

	return l_error;
}

tERROR	MessageCommit(Msg_Data *p_d)
{
	if( FAILED(p_d->message->SaveChanges(FORCE_SAVE|KEEP_OPEN_READWRITE)) )
		return errACCESS_DENIED;

	if( p_d->hIO )
		CALL_SYS_SendMsg(p_d->hIO, pmc_OS_CHANGE, pm_OS_CHANGE, NULL, NULL, NULL);

	p_d->fSaveChanges = cFALSE;
	return errOK;
}

tERROR	MessageChange(Msg_Data *p_d)
{
	p_d->fSaveChanges = cTRUE;
	if( p_d->isembended && p_d->hIO )
		ContentChange(CUST_TO_MsgIO_Data(p_d->hIO));
	return errOK;
}

//////////////////////////////////////////////////////////////////////////
// enum methods

tERROR	EnumCreate(MsgEnum_Data *p_d, IMessage *p_message)
{
	if( !p_message )
		return errOBJECT_NOT_CREATED;

	MAPI_IFACE_COPY(p_d->message, p_message, message);

	p_d->fFolder = cFALSE;
	p_d->Origin = OID_GENERIC_IO;
	return EnumReset(p_d);
}

tERROR	EnumDestroy(MsgEnum_Data *p_d)
{
	EnumReset(p_d);

	MAPI_IFACE_RELEASE(p_d->message, message);
	return errOK;
}

tERROR	EnumSetObject(MsgEnum_Data *p_d, MsgObj_Info *p_objinfo)
{
	EnumReset(p_d);

	p_d->cntype = p_objinfo->cntype;
	if( p_d->cntype != MSGCNT_ATTACH )
		return errOK;

	SPropValue l_prop;
	l_prop.ulPropTag = PR_ATTACH_NUM;
	l_prop.Value.ul = p_objinfo->attachnum;

	return EnumFindAttach(p_d, &l_prop);
}

tERROR	EnumGetObject(MsgEnum_Data *p_d, MsgObj_Info *p_objinfo)
{
	p_objinfo->message = p_d->message;
	p_objinfo->attach = p_d->attach;
	p_objinfo->attachnum = p_d->attachnum;
	p_objinfo->cntype = p_d->cntype;
	p_objinfo->os = p_d->hOS;
	return errOK;
}

tERROR	EnumOpenTable(MsgEnum_Data *p_d)
{
	HRESULT l_result = p_d->message->GetAttachmentTable(0, &p_d->table);
	MAPI_IFACE_CREATE(p_d->table, table);

	if( !p_d->table )
	{
		p_d->ptrState = cObjPtrState_UNDEFINED;
		l_result = E_FAIL;
	}

	SizedSPropTagArray(1, sptaColumns) = {1, {PR_ATTACH_NUM}};
    if( !HR_FAILED(l_result) )
		l_result = p_d->table->SetColumns((LPSPropTagArray)&sptaColumns, 0);
    if( !HR_FAILED(l_result) )
		l_result = p_d->table->SeekRow( BOOKMARK_BEGINNING, 0, NULL);
	return l_result == hrSuccess ? errOK : errOBJECT_INVALID;
}

tERROR	EnumNextContent(MsgEnum_Data *p_d)
{
	HRESULT l_result;
	LPSRowSet l_rowset = NULL;

	p_d->cntype++;
	if( p_d->cntype == MSGCNT_PLAIN_BODY )
		if( !PR_FAIL(ObjectGetProp(p_d->message, PR_BODY, tid_STRING, NULL)) )
			goto next;
		else
			p_d->cntype++;

	if( p_d->cntype == MSGCNT_RICH_BODY )
		if( !PR_FAIL(ObjectGetProp(p_d->message, PR_RTF_COMPRESSED, tid_STRING, NULL)) )
			goto next;
		else
			p_d->cntype++;

	if( p_d->cntype == MSGCNT_HTML_BODY )
		if( !PR_FAIL(ObjectGetProp(p_d->message, PR_BODY_HTML, tid_STRING, NULL)) )
			goto next;
		else
			p_d->cntype++;

	if( p_d->cntype > MSGCNT_ATTACH )
		p_d->cntype = MSGCNT_ATTACH;

	if( !p_d->table )
		if( PR_FAIL(EnumOpenTable(p_d)) )
		{
			p_d->ptrState = cObjPtrState_UNDEFINED;
			return errOBJECT_BAD_INTERNAL_STATE;
		}

	l_result = p_d->table->QueryRows(1, 0, &l_rowset);

	if( !l_rowset )
	{
		p_d->ptrState = cObjPtrState_UNDEFINED;
		return errOBJECT_BAD_INTERNAL_STATE;
	}

	if( !l_rowset->cRows )
	{
		FreeProws(l_rowset);
		p_d->ptrState = cObjPtrState_AFTER;
		p_d->cntype = 0;
		return errEND_OF_THE_LIST;
	}

	p_d->attachnum = l_rowset->aRow[0].lpProps->Value.ul;
	l_result = p_d->message->OpenAttach(p_d->attachnum, NULL, MAPI_BEST_ACCESS, &p_d->attach);

	MAPI_IFACE_CREATE(p_d->attach, attach);

	if( HR_FAILED(l_result) || !p_d->attach )
	{
		FreeProws(l_rowset);
		p_d->ptrState = cObjPtrState_UNDEFINED;
		return errOBJECT_BAD_INTERNAL_STATE;
	}

	FreeProws(l_rowset);

next:
	p_d->ptrState = cObjPtrState_PTR;
	return errOK;
}

tERROR	EnumFindAttach(MsgEnum_Data *p_d, SPropValue *p_prop)
{
	if( PR_FAIL(EnumOpenTable(p_d)) )
		return errOBJECT_INCOMPATIBLE;

	SRestriction l_restr;
	l_restr.rt = RES_PROPERTY;
	l_restr.res.resProperty.relop = RELOP_EQ;
	l_restr.res.resProperty.ulPropTag = p_prop->ulPropTag;
	l_restr.res.resProperty.lpProp = p_prop;

	if( HR_FAILED(p_d->table->FindRow(&l_restr, BOOKMARK_BEGINNING, 0)) )
	{
		p_d->ptrState = cObjPtrState_UNDEFINED;
		return errOBJECT_INCOMPATIBLE;
	}
	p_d->cntype = MSGCNT_ATTACH;
	return EnumNextContent(p_d);
}

tERROR	EnumReset(MsgEnum_Data *p_d)
{
	p_d->ptrState = cObjPtrState_BEFORE;
	p_d->cntype = 0;

	MAPI_IFACE_RELEASE(p_d->attach, attach);
	MAPI_IFACE_RELEASE(p_d->table, table);
	return errOK;
}

tERROR	EnumNext(MsgEnum_Data *p_d)
{
	if( p_d->ptrState == cObjPtrState_UNDEFINED )
		return errOBJECT_BAD_INTERNAL_STATE;

	if( p_d->ptrState == cObjPtrState_AFTER )
		return errEND_OF_THE_LIST;

	MAPI_IFACE_RELEASE(p_d->attach, attach);
	return EnumNextContent(p_d);
}

tERROR	EnumChangeTo(MsgEnum_Data *p_d, hSTRING p_string, tDWORD p_open_mode)
{
	tERROR l_error = errOK;

	tDWORD l_len;
	CALL_String_LengthEx(p_string, &l_len, cSTRING_WHOLE, cCP_ANSI, cSTRING_Z);

	tCHAR *l_buff = NULL;
	MAPIAllocateBuffer(l_len, (LPVOID*)&l_buff);

	CALL_String_ExportToBuff(p_string, NULL, cSTRING_WHOLE, l_buff, l_len, cCP_ANSI, cSTRING_Z);

	if( !strcmp(l_buff, MSG_CONTENT_PLAIN_BODY) )
		p_d->cntype = MSGCNT_PLAIN_BODY;
	else if( !strcmp(l_buff, MSG_CONTENT_RICH_BODY) )
		p_d->cntype = MSGCNT_RICH_BODY;
	else if( !strcmp(l_buff, MSG_CONTENT_HTML_BODY) )
		p_d->cntype = MSGCNT_HTML_BODY;
	else
	{
		SPropValue l_prop;
		l_prop.ulPropTag = PR_DISPLAY_NAME;
		l_prop.Value.lpszA = l_buff;

		l_error = EnumFindAttach(p_d, &l_prop);

		if( PR_FAIL(l_error) && (p_open_mode&fOMODE_CREATE_IF_NOT_EXIST) )
		{
			if( p_open_mode & fOMODE_CREATE_PLAIN_BODY )
				p_d->cntype = MSGCNT_PLAIN_BODY;
			else if( p_open_mode & fOMODE_CREATE_RICH_BODY )
				p_d->cntype = MSGCNT_RICH_BODY;
			else if( p_open_mode & fOMODE_CREATE_HTML_BODY )
				p_d->cntype = MSGCNT_HTML_BODY;
			else
			{
				if( FAILED(p_d->message->CreateAttach(NULL, 0, (ULONG*)&p_d->attachnum, &p_d->attach)) )
					l_error = errOBJECT_NOT_CREATED;
				else if( FAILED(ObjectSetPropDWORD(p_d->attach, PR_ATTACH_METHOD, ATTACH_BY_VALUE)) )
					l_error = errOBJECT_NOT_CREATED;
				else
				{
					MAPI_IFACE_CREATE(p_d->attach, attach);
					p_d->cntype = MSGCNT_ATTACH;

					PRProp_Buff l_prop_buff = {l_buff, l_len, tid_VOID, 0};
					l_error = ObjectSetProp(p_d->attach, PR_DISPLAY_NAME, tid_CHAR, &l_prop_buff);

					if( PR_SUCC(l_error) )
						if( FAILED(p_d->attach->SaveChanges(FORCE_SAVE|KEEP_OPEN_READWRITE)) )
							l_error = errOBJECT_NOT_CREATED;

					if( PR_SUCC(l_error) )
						MessageChange(CUST_TO_Msg_Data(p_d->hOS));
				}
			}
		}
	}

	if( l_buff )
		MAPIFreeBuffer(l_buff);

	return l_error;
}

tERROR	EnumChangeToData(MsgEnum_Data *p_d, tCHAR *p_buffer, tDWORD p_size)
{
	EnumReset(p_d);

	p_d->cntype = *(tDWORD*)p_buffer;
	p_buffer += sizeof(tDWORD);

	if( p_d->cntype != MSGCNT_ATTACH )
		return errOK;

	tDWORD attachnum = *(tDWORD*)p_buffer;
	p_buffer += sizeof(tDWORD);

	while( PR_SUCC(EnumNext(p_d)) )
		if( p_d->attachnum == attachnum )
			return errOK;

	return errOBJECT_NOT_FOUND;
}

//////////////////////////////////////////////////////////////////////////
// content methods

#define	LLONGDEF(name, val)		LARGE_INTEGER name; name.QuadPart = val
#define	ULLONGDEF(name, val)	ULARGE_INTEGER name; name.QuadPart = val

tERROR	ContentCreate(MsgIO_Data *p_d, IMessage *p_message)
{
	if( !p_message )
		return errOBJECT_NOT_CREATED;

	MAPI_IFACE_COPY(p_d->message, p_message, message);
	p_d->Origin = OID_GENERIC_IO;
	return errOK;
}

tERROR	ContentDestroy(MsgIO_Data *p_d)
{
	MAPI_IFACE_RELEASE(p_d->attach, attach);
	MAPI_IFACE_RELEASE(p_d->message, message);
	return errOK;
}

tERROR	ContentGetEmbMessage(MsgIO_Data *p_d, IMessage **p_message)
{
	if( p_d->objmethod != ATTACH_EMBEDDED_MSG || !p_d->attach )
		return errOBJECT_NOT_CREATED;

	HRESULT l_error = p_d->attach->OpenProperty(PR_ATTACH_DATA_OBJ, &IID_IMessage,
			0, MAPI_MODIFY, (IUnknown**)p_message);

	MAPI_IFACE_CREATE(*p_message, message);
	return HR_FAILED(l_error) ? errOBJECT_NOT_CREATED : errOK;
}

tERROR	ContentSetObject(MsgIO_Data *p_d, MsgObj_Info *p_objinfo)
{
	MAPI_IFACE_COPY(p_d->attach, p_objinfo->attach, attach);
	p_d->attachnum = p_objinfo->attachnum;
	p_d->cntype = p_objinfo->cntype;

	return errOK;
}

tERROR	ContentGetObject(MsgIO_Data *p_d, MsgObj_Info *p_objinfo)
{
	p_objinfo->message = p_d->message;
	p_objinfo->attach = p_d->attach;
	p_objinfo->attachnum = p_d->attachnum;
	p_objinfo->cntype = p_d->cntype;
	p_objinfo->os = p_d->hOS;
	return errOK;
}

tERROR	ContentSetMode(MsgIO_Data *p_d, tDWORD p_mode)
{
	if( p_d->AccessMode == p_mode )
		return errOK;

	if( (p_mode & fACCESS_WRITE) && p_d->storage )
		return errNOT_SUPPORTED;

	if( p_d->fModified || p_d->fSaveChanges )
		ContentCommit(p_d);

	ContentStreamClose(p_d);
	p_d->AccessMode = p_mode;
	return ContentStreamOpen(p_d);
}

tERROR	ContentStreamOpen(MsgIO_Data *p_d)
{
	ULONG l_mode = 0, l_flags = 0;

	if( p_d->AccessMode & fACCESS_READ )
		l_mode |= STGM_READ;

	if( p_d->AccessMode & fACCESS_WRITE )
	{
		l_flags |= MAPI_MODIFY;
		l_mode |= STGM_WRITE;
	}

	if( p_d->OpenMode & fOMODE_CREATE_IF_NOT_EXIST )
		l_flags |= MAPI_CREATE;

	p_d->objmethod = NO_ATTACHMENT;

	HRESULT l_error = hrSuccess;
	if( p_d->cntype == MSGCNT_PLAIN_BODY || p_d->cntype == MSGCNT_HTML_BODY )
	{
		ULONG l_prop = (p_d->cntype == MSGCNT_PLAIN_BODY) ? PR_BODY : PR_BODY_HTML;
		l_error = p_d->message->OpenProperty(l_prop, &IID_IStream, l_mode, l_flags, (IUnknown**)&p_d->stream);
		if( FAILED(l_error) )
		{
			SizedSPropTagArray(1, sptaColumns) = {1, {l_prop}};

			LPSPropValue l_value = NULL; ULONG l_num = 0;
			l_error = p_d->message->GetProps((LPSPropTagArray)&sptaColumns, 0, &l_num, &l_value);
			if( l_value )
			{
				if( (l_value->ulPropTag & PROP_TYPE_MASK) == PT_ERROR )
					l_error = l_value->Value.err;

				if( l_error == hrSuccess )
					l_error = CreateILockBytesOnHGlobal(NULL, TRUE, &p_d->lockbyte);

				if( !FAILED(l_error) )
				{
					tSTRING l_strval = l_value->Value.lpszA;
					ULLONGDEF(l_offs, 0);
					l_error = p_d->lockbyte->WriteAt(l_offs, l_strval, (ULONG)(strlen(l_strval)+1), NULL);
				}

				MAPIFreeBuffer(l_value);
			}
		}
		p_d->Origin = OID_MAIL_MSG_BODY;
	}
	else if( p_d->cntype == MSGCNT_RICH_BODY )
	{
		IStream *l_uncompressed_stream = NULL;
		l_error = p_d->message->OpenProperty(PR_RTF_COMPRESSED, &IID_IStream,
				l_mode, l_flags, (IUnknown**)&l_uncompressed_stream);
		if( !HR_FAILED(l_error) )
			l_error = WrapCompressedRTFStream(l_uncompressed_stream, l_flags, &p_d->stream);
		if( l_uncompressed_stream )
			l_uncompressed_stream->Release();

		p_d->Origin = OID_MAIL_MSG_BODY;
	}
	else if( p_d->cntype == MSGCNT_ATTACH )
	{
		if( !p_d->attach )
			return errOBJECT_NOT_CREATED;

		p_d->objmethod = ObjectGetPropDWORD(p_d->attach, PR_ATTACH_METHOD);

		p_d->Origin = OID_MAIL_MSG_ATTACH;

		switch( p_d->objmethod )
		{
		case ATTACH_BY_VALUE:
			l_error = p_d->attach->OpenProperty(PR_ATTACH_DATA_BIN, &IID_IStream,
					l_mode, l_flags, (IUnknown**)&p_d->stream);
			break;
		case ATTACH_OLE:
			l_error = p_d->attach->OpenProperty(PR_ATTACH_DATA_OBJ, &IID_IStreamDocfile,
					l_mode, l_flags, (IUnknown**)&p_d->stream);
			if( HR_FAILED(l_error) )
				l_error = p_d->attach->OpenProperty(PR_ATTACH_DATA_BIN, &IID_IStreamDocfile,
						l_mode, l_flags, (IUnknown**)&p_d->stream);
			if( HR_FAILED(l_error) )
			{
				l_error = p_d->attach->OpenProperty(PR_ATTACH_DATA_OBJ, &IID_IStorage,
						l_mode, l_flags, (IUnknown**)&p_d->storage);

				if( !HR_FAILED(l_error) )
				{
					l_error = CreateILockBytesOnHGlobal(NULL, TRUE, &p_d->lockbyte);

					if( !HR_FAILED(l_error) )
						l_error = StgCreateDocfileOnILockBytes(p_d->lockbyte,
							STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &p_d->tmpstg);

					if( !HR_FAILED(l_error) )
						l_error = p_d->storage->CopyTo(0, NULL, NULL, p_d->tmpstg);

					if( !HR_FAILED(l_error) )
						p_d->tmpstg->Commit(STGC_DEFAULT);
				}
			}
			break;
		case ATTACH_EMBEDDED_MSG:
			p_d->Origin = OID_MAIL_MSG_REF;
			return errOK;
		case ATTACH_BY_REF_ONLY:
		case ATTACH_BY_REF_RESOLVE:
		case ATTACH_BY_REFERENCE:
		default: return errOBJECT_NOT_CREATED;
		}
	}

	if( HR_FAILED(l_error) )
		return errOBJECT_NOT_CREATED;

	if( l_flags & MAPI_CREATE )
	{
		p_d->OpenMode &= ~fOMODE_CREATE_IF_NOT_EXIST;
		ContentChange(p_d);
	}

	return errOK;
}

tERROR	ContentStreamClose(MsgIO_Data *p_d)
{
	if( p_d->stream )
	{
		p_d->stream->Release();
		p_d->stream = NULL;
	}
	if( p_d->storage )
	{
		if( p_d->tmpstg )
		{
			p_d->tmpstg->Release();
			p_d->tmpstg = NULL;
		}
		p_d->storage->Release();
		p_d->storage = NULL;
	}
	if( p_d->lockbyte )
	{
		p_d->lockbyte->Release();
		p_d->lockbyte = NULL;
	}

	return errOK;
}

tERROR	ContentSeekRead(MsgIO_Data *p_d, tQWORD p_offset, tPTR p_buffer, tDWORD p_size, tDWORD* p_result)
{
	if( p_d->objmethod == ATTACH_EMBEDDED_MSG )
		return errNOT_IMPLEMENTED;

	if( !p_d->lockbyte )
		return errOBJECT_INVALID;

	ULLONGDEF(l_offs, p_offset);
	if( FAILED(p_d->lockbyte->ReadAt(l_offs, p_buffer, p_size, (ULONG*)p_result)) )
		return errOBJECT_INVALID;

	if( *p_result < p_size )
	{
		if( !*p_result )
		{
			STATSTG l_ss;
			if( FAILED(p_d->lockbyte->Stat(&l_ss, STATFLAG_NONAME)) )
				return errOBJECT_INVALID;
			if( l_ss.cbSize.QuadPart < p_offset )
				return errOUT_OF_OBJECT;
		}
		return errEOF;
	}

	return errOK;
}

tERROR	ContentSeekWrite(MsgIO_Data *p_d, tQWORD p_offset, tPTR p_buffer, tDWORD p_size, tDWORD* p_result)
{
	if( p_d->objmethod == ATTACH_EMBEDDED_MSG )
		return errNOT_IMPLEMENTED;

	if( !(p_d->AccessMode & fACCESS_WRITE) )
		return errOBJECT_READ_ONLY;

	if( !p_d->lockbyte )
		return errOBJECT_INVALID;

	ULLONGDEF(l_offs, p_offset);
	if( FAILED(p_d->lockbyte->WriteAt(l_offs, p_buffer, p_size, (ULONG*)p_result)) )
		return errOBJECT_INVALID;
	return errOK;
}

tERROR	ContentGetSize(MsgIO_Data *p_d, IO_SIZE_TYPE p_type, tQWORD* p_result)
{
	if( p_d->objmethod == ATTACH_EMBEDDED_MSG )
		return errNOT_IMPLEMENTED;

	STATSTG l_ss;
	HRESULT l_error = E_FAIL;
	if( p_d->lockbyte )
		l_error = p_d->lockbyte->Stat(&l_ss, STATFLAG_NONAME);
	else if( p_d->stream )
		l_error = p_d->stream->Stat(&l_ss, STATFLAG_NONAME);

	if( FAILED(l_error) )
	{
		if( p_d->attach && p_type != IO_SIZE_TYPE_EXPLICIT )
		{
			PRProp_Buff l_buff = {(tCHAR*)p_result, sizeof(tQWORD), tid_VOID, 0};
			return ObjectGetProp(p_d->attach, PR_ATTACH_SIZE, tid_QWORD, &l_buff);
		}
		return errOBJECT_INVALID;
	}

	*p_result = l_ss.cbSize.QuadPart;
	return errOK;
}

tERROR	ContentSetSize(MsgIO_Data *p_d, tQWORD p_new_size)
{
	if( p_d->objmethod == ATTACH_EMBEDDED_MSG )
		return errNOT_IMPLEMENTED;

	if( !p_d->lockbyte )
		return errOBJECT_INVALID;

	ULLONGDEF(l_new_size, p_new_size);
	if( FAILED(p_d->lockbyte->SetSize(l_new_size)) )
		return errOBJECT_RESIZE;
	return errOK;
}

tERROR	ContentCopy(MsgIO_Data *p_d, tBOOL dir)
{
	if( !p_d->stream )
		return errOBJECT_INCOMPATIBLE;

	LLONGDEF(l_offs, 0);
	if( FAILED(p_d->stream->Seek(l_offs, STREAM_SEEK_SET, NULL)) )
		return errOBJECT_SEEK;

	tCHAR	l_buff[PROCESS_CHUNCK_SIZE];
	tDWORD	l_buff_size = PROCESS_CHUNCK_SIZE;
	tQWORD	l_size = 0;
	tERROR	l_error = errOK;

	while( l_buff_size == PROCESS_CHUNCK_SIZE )
	{
		if( dir )
		{
			if( FAILED(p_d->stream->Read(l_buff, PROCESS_CHUNCK_SIZE, (ULONG*)&l_buff_size)) )
				return errOBJECT_READ;

			if( PR_FAIL(l_error = CALL_IO_SeekWrite(p_d->superio, NULL, l_size, l_buff, l_buff_size)) )
				return l_error;
		}
		else
		{
			if( PR_FAIL(l_error = CALL_IO_SeekRead(p_d->superio, &l_buff_size, l_size, l_buff, PROCESS_CHUNCK_SIZE)) )
				return l_error;

			if( FAILED(p_d->stream->Write(l_buff, l_buff_size, NULL)) )
				return errOBJECT_WRITE;
		}
		l_size += l_buff_size;
	}

	if( PR_SUCC(l_error) && !dir )
	{
		ULLONGDEF(_size, l_size);
		p_d->stream->SetSize(_size);
	}

	return errOK;
}

tERROR	ContentFlush(MsgIO_Data *p_d)
{
	if( !p_d->fModified )
		return errOK;

	HRESULT l_error = S_OK;
	if( p_d->superio )
	{
		tERROR l_error = CALL_IO_Flush(p_d->superio);
		if( PR_SUCC(l_error) )
			l_error = ContentCopy(p_d, cFALSE);
		if( PR_FAIL(l_error) )
			return l_error;
	}
	else if( p_d->tmpstg )
	{
		l_error = p_d->tmpstg->CopyTo(0, NULL, NULL, p_d->storage);
	}
	else if( p_d->lockbyte && (p_d->cntype == MSGCNT_PLAIN_BODY || p_d->cntype == MSGCNT_HTML_BODY) )
	{
		HGLOBAL l_gl;
		GetHGlobalFromILockBytes(p_d->lockbyte, &l_gl);

		SPropValue l_value;
		l_value.ulPropTag = (p_d->cntype == MSGCNT_PLAIN_BODY) ? PR_BODY : PR_BODY_HTML;
		l_value.Value.lpszA = (char*)GlobalLock(l_gl);

		l_error = p_d->message->SetProps(1, &l_value, NULL);

		GlobalUnlock(l_gl);
	}

	if( FAILED(l_error) )
		return errOBJECT_WRITE;

	p_d->fModified = cFALSE;
	p_d->fSaveChanges = cTRUE;
	return errOK;
}

tERROR	ContentCommit(MsgIO_Data *p_d)
{
	tERROR l_error = ContentFlush(p_d);
	if( PR_FAIL(l_error) )
		return l_error;

	if( p_d->cntype == MSGCNT_ATTACH )
	{
		if( FAILED(p_d->attach->SaveChanges(FORCE_SAVE|KEEP_OPEN_READWRITE)) )
			return errACCESS_DENIED;
	}
	p_d->fSaveChanges = cFALSE;
	MessageChange(CUST_TO_Msg_Data(p_d->hOS));
	return cTRUE;
}

tERROR	ContentChange(MsgIO_Data *p_d)
{
	p_d->fSaveChanges = cTRUE;
	return MessageChange(CUST_TO_Msg_Data(p_d->hOS));
}

tERROR	ContentLoadData(MsgIO_Data *p_d, tCHAR *p_buffer, tDWORD p_size)
{
	hObjPtr &l_objptr = CUST_TO_Msg_Data(p_d->hOS)->tmpenum;
	if( !l_objptr )
		CALL_SYS_ObjectCreateQuick(p_d->hOS, &l_objptr, IID_OBJPTR, PID_MAILMSG, 0);

	tERROR l_error = EnumChangeToData(CUST_TO_MsgEnum_Data(l_objptr), p_buffer, p_size);
	if( PR_FAIL(l_error) )
		return l_error;

	MsgObj_Info l_obj_info;
	EnumGetObject(CUST_TO_MsgEnum_Data(l_objptr), &l_obj_info);
	return ContentSetObject(p_d, &l_obj_info);
}

tERROR	ContentSaveData(MsgIO_Data *p_d, tDWORD* p_out_size, tCHAR *p_buffer, tDWORD p_size)
{
	PRProp_Buff l_buff = {p_buffer, p_size, tid_PTR, 0};

	PRBuffSetDWORD(&l_buff, p_d->cntype);
	if( p_d->cntype == MSGCNT_ATTACH )
		PRBuffSetDWORD(&l_buff, p_d->attachnum);

	*p_out_size = l_buff.out_size;

	return l_buff.out_size > p_size ? errBUFFER_TOO_SMALL : errOK;
}

//////////////////////////////////////////////////////////////////////////

tERROR	TransformerProcessChunck(MsgTR_Data *p_d, tQWORD p_size, tQWORD* p_result)
{
	MsgIO_Data *l_cont = CUST_TO_MsgIO_Data(p_d->io);
	tCHAR	l_buff[PROCESS_CHUNCK_SIZE];
	tDWORD	l_buff_size = PROCESS_CHUNCK_SIZE;
	tQWORD	l_size = 0;
	tERROR	l_error = errOK;

	if( !p_d->iseqio && !p_d->oseqio )
		return errOBJECT_NOT_INITIALIZED;

	while( l_size < p_size && l_buff_size == PROCESS_CHUNCK_SIZE )
	{
		if( p_d->oseqio )
		{
			if( FAILED(l_cont->stream->Read(l_buff, PROCESS_CHUNCK_SIZE, (ULONG*)&l_buff_size)) )
				return errOBJECT_READ;
			if( PR_FAIL(l_error = CALL_SeqIO_Write(p_d->oseqio, NULL, l_buff, l_buff_size)) )
				return l_error;
		}
		else
		{
			if( PR_FAIL(l_error = CALL_SeqIO_Read(p_d->iseqio, &l_buff_size, l_buff, PROCESS_CHUNCK_SIZE)) )
				return l_error;
			if( FAILED(l_cont->stream->Write(l_buff, l_buff_size, NULL)) )
				return errOBJECT_WRITE;
		}
		l_size += l_buff_size;
	}
	*p_result = l_size;
	return errOK;
}

tERROR	TransformerProcess(MsgTR_Data *p_d, tQWORD* p_result)
{
	MsgIO_Data *l_cont = CUST_TO_MsgIO_Data(p_d->io);

	if( !p_d->iseqio && !p_d->oseqio )
		return errOBJECT_NOT_INITIALIZED;

	LLONGDEF(l_offs, 0);
	if( FAILED(l_cont->stream->Seek(l_offs, STREAM_SEEK_SET, NULL)) )
		return errOBJECT_SEEK;

	tERROR l_error = CALL_SeqIO_SeekSet(p_d->iseqio ? p_d->iseqio : p_d->oseqio, NULL, 0);

	if( PR_SUCC(l_error) )
		l_error = TransformerProcessChunck(p_d, cMAX_LONGLONG, p_result);

	if( PR_SUCC(l_error) && p_d->iseqio )
	{
		ULLONGDEF(l_size, *p_result);
		l_cont->stream->SetSize(l_size);
	}
	return l_error;
}

tERROR	TransformerGetSize(MsgTR_Data *p_d, tQWORD* p_result)
{
	return ContentGetSize(CUST_TO_MsgIO_Data(p_d->io), IO_SIZE_TYPE_APPROXIMATE, p_result);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
