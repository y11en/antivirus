//////////////////////////////////////////////////////////////////////////
// Source file for MAPI routine

//#define CHECK_REFERENCE

#define INITGUID
#define USES_IID_IMsgServiceAdmin

#include "mdb_mapi.h"

#include <Prague/pr_remote.h>
#include <Prague/iface/i_buffer.h>
#include <Prague/plugin/p_win32_nfio.h>

#include <Core/remote/pr_process.h>
#include <edkmdb.h>

#include <mspst.h>
#include <vector>

#define MAX_PROFILE_ROWS	100
#define MAX_SERVICE_ROWS	100

#define KEY_MAIL_CLIENT		"SOFTWARE\\Clients\\Mail"
#define KEY_OUTLOOK			"Microsoft Outlook"
#define KEY_OUTLOOK_ID		"MSIComponentID"
#define KEY_PROFILE_NT		"Software\\Microsoft\\Windows NT\\Currentversion\\Windows Messaging Subsystem\\Profiles"
#define KEY_PROFILE_9x		"Software\\Microsoft\\Windows MessagingSubsystem\\Profiles"

#pragma comment(lib, "ole32.lib")

extern void HookMAPIFeatures(BOOL bHook, HANDLE hToken, BOOL bForceUISuppress);

static WTSApi    g_WTSApi;
static BOOL      g_isFault = FALSE;

//////////////////////////////////////////////////////////////////////////
// object methods

tERROR	ObjInfoGet(MDBObj_Info *p_d, hOBJECT p_obj, hOBJECT p_name, tDWORD p_open_mode)
{
	memset(p_d, 0, sizeof(MDBObj_Info));
	if( !p_name )
		return errOK;

	tIID l_iid = CALL_SYS_PropertyGetDWord(p_name, pgINTERFACE_ID);
	switch( l_iid )
	{
	case IID_OBJPTR:	return EnumGetObject(CUST_TO_MDBEnum_Data(p_name), p_d, cTRUE);
	case IID_IO:		return MessageGetObject(CUST_TO_MDBIO_Data(p_name), p_d);
	}

	tIID l_obj_iid = CALL_SYS_PropertyGetDWord(p_obj, pgINTERFACE_ID);

	hOS l_os = NULL;
	switch(l_obj_iid)
	{
	case IID_OS:	 l_os = (hOS)p_obj; break;
	case IID_OBJPTR: l_os = CUST_TO_MDBEnum_Data(p_obj)->hOS; break;
	default: return errUNEXPECTED;
	}

	hObjPtr &l_objptr = CUST_TO_MDB_Data(l_os)->tmpenum;

	if( !l_objptr )
		CALL_SYS_ObjectCreateQuick(l_os, &l_objptr, IID_OBJPTR, PID_MDB, 0);

	MDBEnum_Data *l_enum = CUST_TO_MDBEnum_Data(l_objptr);

	tERROR l_error = errNOT_IMPLEMENTED;

	if( l_iid == IID_STRING )
	{
		EnumDestroy(l_enum);
		l_error = EnumCreate(l_enum);

		if( l_obj_iid == IID_OBJPTR )
		{
			MDBObj_Info l_clon_info;
			EnumGetObject(CUST_TO_MDBEnum_Data(p_obj), &l_clon_info, cTRUE);
			l_error = EnumSetObject(l_enum, &l_clon_info);
		}

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
		l_error = EnumGetObject(l_enum, p_d, cTRUE);

	return l_error;
}

tERROR	ObjInfoIOCreate(MDBObj_Info *p_d, hOS p_os, hIO *p_ret_val, tDWORD p_open_mode, tDWORD p_access_mode)
{
	if( p_d->type != MAPI_MESSAGE )
		return errOBJECT_NOT_CREATED;

	hOBJECT l_ret_val = NULL;
	tERROR l_error = CALL_SYS_ObjectCreate(p_os, (hIO*)&l_ret_val, IID_IO, PID_MDB, 0);
	if( PR_SUCC(l_error) )
	{
		CALL_SYS_PropertySetDWord(l_ret_val, pgOBJECT_OPEN_MODE, p_open_mode);
		CALL_SYS_PropertySetDWord(l_ret_val, pgOBJECT_ACCESS_MODE, p_access_mode);
		l_error = MessageSetObject(CUST_TO_MDBIO_Data(l_ret_val), p_d);
		if( PR_SUCC(l_error) )
			l_error = CALL_SYS_ObjectCreateDone(l_ret_val);
	}
	if( PR_FAIL(l_error) && l_ret_val )
	{
		CALL_SYS_ObjectClose((hObjPtr)l_ret_val);
		l_ret_val = NULL;
	}
	*p_ret_val = (hIO)l_ret_val;
	return l_error;
}

tERROR	ObjInfoGetProp(MDBObj_Info *p_d, tPROPID p_prop, tCHAR* p_buffer, tDWORD p_size, tDWORD* p_out_size)
{
	tERROR l_error;
	PRProp_Buff l_buff = {p_buffer, p_size, tid_VOID, 0};

	switch( p_prop )
	{
		case pgOBJECT_PATH:		 l_error = ObjInfoGetPath(p_d, &l_buff); break;
		case pgOBJECT_NAME:		 l_error = ObjInfoGetName(p_d, &l_buff); break;
		case pgOBJECT_FULL_NAME:
			l_error = ObjInfoGetPath(p_d, &l_buff);
			if( PR_SUCC(l_error) )
				l_error = ObjInfoGetName(p_d, &l_buff);
			break;
		case pgOBJECT_SIZE:
			if( p_d->type == MAPI_MESSAGE )
				l_error = ObjectGetProp(p_d->object, PR_MESSAGE_SIZE, tid_DWORD, &l_buff);
			else
				l_error = PRBuffSetDWORD(&l_buff, 0);
			break;
		case pgOBJECT_SIZE_Q:
			if( p_d->type == MAPI_MESSAGE )
				l_error = ObjectGetProp(p_d->object, PR_MESSAGE_SIZE, tid_QWORD, &l_buff);
			else
				l_error = PRBuffSetQWORD(&l_buff, 0);
			break;

		case pgIS_MESSAGE_STORAGE:
		case pgCAN_CREATE_SUBFOLDERS:
			l_error = PRBuffSetDWORD(&l_buff, p_d->folder ? cTRUE : cFALSE);
			break;

		case pgSUBFOLDERS_COUNT:
			if( p_d->type == MAPI_FOLDER )
				l_error = PRBuffSetDWORD(&l_buff, p_d->issubfolders);
			else
				l_error = errOBJECT_INCOMPATIBLE;
			break;

		case pgMAIL_OBJECT_ENTRYID:
			if( p_d->type != MAPI_PROFSECT )
			{
				l_buff.type = tid_PTR;
				if( p_d->object )
					l_error = ObjectGetProp(p_d->object, PR_ENTRYID, tid_PTR, &l_buff);
				else if( p_d->entry )
					l_error = PRBuffSetBINARY(&l_buff, p_d->entry->Value.bin.lpb, p_d->entry->Value.bin.cb);
				else
					l_error = errOBJECT_INCOMPATIBLE;
			}
			else
				l_error = errOBJECT_INCOMPATIBLE;
			break;

		default: return errPROPERTY_NOT_FOUND;
	}

	if( p_out_size )
		*p_out_size = l_buff.out_size;

	return l_error;
}

tERROR	ObjInfoGetName(MDBObj_Info *p_d, PRProp_Buff *p_buff)
{
	if( p_d->type == MAPI_MESSAGE )
		return MessageGetName(p_d->object, p_buff);

	if( !p_d->name )
		return errOBJECT_INCOMPATIBLE;

	return PRBuffSetSTRING(p_buff, p_d->name);
}

tERROR	ObjInfoGetPath(MDBObj_Info *p_d, PRProp_Buff *p_buff)
{
	tERROR l_error;
	if( PR_FAIL(l_error = PRBuffSetNULL(p_buff)) )
		return l_error;

	if( p_d->profile )
	{
		if( PR_FAIL(l_error = PRBuffSetSTRING(p_buff, p_d->profile)) )
			return l_error;

		if( p_d->type == MAPI_PROFSECT )
			return errOK;

		if( PR_FAIL(l_error = PRBuffSetCHAR(p_buff, PRBUFF_DIRSLASH)) )
			return l_error;
	}

	if( !p_d->store || p_d->type == MAPI_STORE )
		return errOK;

	if( PR_FAIL(l_error = ObjectGetProp(p_d->store, PR_DISPLAY_NAME, tid_CHAR, p_buff)) )
		return l_error;

	if( PR_FAIL(l_error = PRBuffSetCHAR(p_buff, PRBUFF_DIRSLASH)) )
		return l_error;

	if( !p_d->folder )
		return errOBJECT_INVALID;

	return FolderGetPath(p_d, p_d->folder, p_buff, MDB_DATA()->type != MAIL_OS_TYPE_OUTLOOK);
}

tERROR	ObjInfoCopy(MDBObj_Info *p_src, MDBObj_Info *p_dst, tBOOL f_move, tBOOL f_overwrite)
{
	if( !p_dst->folder )
		return errOBJECT_INCOMPATIBLE;

	HRESULT l_result = E_FAIL;
	switch(p_src->type)
	{
	case MAPI_PROFSECT:
	case MAPI_STORE:
		return errNOT_IMPLEMENTED;
	case MAPI_FOLDER:
		l_result = p_src->folder->CopyFolder(
			p_src->entry->Value.bin.cb, (LPENTRYID)p_src->entry->Value.bin.lpb,
			NULL, p_dst->folder, NULL, 0, NULL, COPY_SUBFOLDERS|(f_move ? FOLDER_MOVE : 0)
			);
		break;
	case MAPI_MESSAGE:
		{
		ENTRYLIST l_list = {1, &p_src->entry->Value.bin};
		l_result = p_src->folder->CopyMessages(
			&l_list, NULL, p_dst->folder, 0, NULL, (f_move ? MESSAGE_MOVE  : 0)
			);
		}
		break;
	}
	return HR_FAILED(l_result) ? errOBJECT_INCOMPATIBLE : errOK;
}

tERROR	ObjInfoDelete(MDBObj_Info *p_d)
{
	HRESULT l_result = E_FAIL;
	switch(p_d->type)
	{
	case MAPI_PROFSECT:
	case MAPI_STORE:
		return errNOT_IMPLEMENTED;
	case MAPI_FOLDER:
		l_result = p_d->folder->DeleteFolder(
			p_d->entry->Value.bin.cb, (LPENTRYID)p_d->entry->Value.bin.lpb,
			0, NULL, DEL_FOLDERS|DEL_MESSAGES
			);
		break;
	case MAPI_MESSAGE:
		{
		ENTRYLIST l_list = {1, &p_d->entry->Value.bin};
		l_result = p_d->folder->DeleteMessages(&l_list, 0, NULL, 0);
		}
		break;
	}
	return HR_FAILED(l_result) ? errOBJECT_INCOMPATIBLE : errOK;
}

//////////////////////////////////////////////////////////////////////////
// helpers

tERROR CopyIO(hIO dst, hIO src)
{
	tQWORD l_offset = 0;
	tBYTE  l_buff[0x1000];
	tDWORD l_read_size = sizeof(l_buff);

	tERROR l_error = CALL_IO_SetSize(dst, 0);

	while( PR_SUCC(l_error) && l_read_size == sizeof(l_buff) )
	{
		l_error = CALL_IO_SeekRead(src, &l_read_size, l_offset, l_buff, sizeof(l_buff));
		if( PR_SUCC(l_error) )
			l_error = CALL_IO_SeekWrite(dst, NULL, l_offset, l_buff, l_read_size);
		l_offset += l_read_size;
	}
	return PR_FAIL(l_error) ? l_error : errOK;
}


hIO	CreateNativeIO(hIO io)
{
	tPID l_pid = CALL_SYS_PropertyGetDWord(io, pgPLUGIN_ID);
	if( l_pid == PID_NATIVE_FIO )
		return io;

	tCHAR l_tmp_path[MAX_PATH], l_tmp_name[MAX_PATH];
	if( !GetTempPath(sizeof(l_tmp_path), l_tmp_path) )
		return NULL;

	if( !GetTempFileName(l_tmp_path, "PR", 0 , l_tmp_name) )
		return NULL;

	hIO l_io = NULL;
	tERROR l_error = CALL_SYS_ObjectCreate(io, &l_io, IID_IO, PID_NATIVE_FIO, SUBTYPE_ANY);

	if( PR_SUCC(l_error) )
		l_error = CALL_SYS_PropertySetDWord(l_io, pgOBJECT_OPEN_MODE, fOMODE_CREATE_ALWAYS);
	if( PR_SUCC(l_error) )
		l_error = CALL_SYS_PropertySetDWord(l_io, pgOBJECT_ACCESS_MODE, fACCESS_RW);
	if( PR_SUCC(l_error) )
		l_error = CALL_SYS_PropertySetBool(l_io, plCONTROL_SIZE_ON_READ, cFALSE);
	if( PR_SUCC(l_error) )
		l_error = CALL_SYS_PropertySetStr(l_io, 0, pgOBJECT_NAME, l_tmp_name, 0, cCP_ANSI );

	if( PR_SUCC(l_error) )
		l_error = CALL_SYS_ObjectCreateDone(l_io);	

	if( PR_SUCC(l_error) )
		l_error = CopyIO(l_io, io);

	if( PR_SUCC(l_error) )
		l_error = CALL_SYS_PropertySetDWord(l_io, pgOBJECT_ACCESS_MODE, fACCESS_READ);

	if( PR_SUCC(l_error) )
		return l_io;

	CALL_SYS_ObjectClose(l_io);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// session methods

HANDLE	g_tmp_sync = NULL;
tBOOL	g_is_nt;

class CThreadReverterToSelf
{
	HANDLE m_hToken;
	bool m_bReverted;
public:
	CThreadReverterToSelf() : m_hToken(NULL), m_bReverted(false)
	{
		if(OpenThreadToken(GetCurrentThread(), TOKEN_IMPERSONATE, TRUE, &m_hToken))
			m_bReverted = RevertToSelf() != FALSE;
	}

	~CThreadReverterToSelf()
	{
		if(m_bReverted && m_hToken)
			SetThreadToken(NULL, m_hToken);

        if(m_hToken)
			CloseHandle(m_hToken);
	}
};

tERROR	SessionInitialize()
{
	OSVERSIONINFO l_OS;
	l_OS.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&l_OS);

	g_is_nt = l_OS.dwPlatformId == VER_PLATFORM_WIN32_NT;

	g_tmp_sync = CreateMutex(NULL, FALSE, NULL/*"AvpPstTmpSync"*/);
	if( !g_tmp_sync )
		return errNOT_OK;

	return errOK;
}

tERROR	SessionDeinitialize()
{
	if( g_tmp_sync )
	{
		CloseHandle(g_tmp_sync);
		g_tmp_sync = NULL;
	}

	return errOK;
}

tERROR	SessionThreadAttach()
{
	return errOK;
}

HRESULT	SessionCreateTmpProfile(MDB_Data *p_d, tCHAR *p_buff, tSTRING def_profile)
{
	WaitForSingleObject(g_tmp_sync, INFINITE);

	HRESULT l_result;
	int i;

/*	static bool g_bCleanUp = false;
	if( !g_bCleanUp )
	{
		g_bCleanUp = true;
		for(i = 0; i < MAX_PROFILE_ROWS; i++)
		{
			sprintf(p_buff, "AvpPstTmp%03.3u", i);
			p_d->prof_admin->DeleteProfile(p_buff, 0);
		}
	}
*/
	for(i = 0; i < MAX_PROFILE_ROWS; i++)
	{
		sprintf_s(p_buff, 13, "AvpPstTmp%03.3u", i);

		l_result = p_d->prof_admin->CreateProfile(p_buff, NULL, 0, 0);
		if( l_result != MAPI_E_NO_ACCESS )
		{
			if( def_profile )
				p_d->prof_admin->SetDefaultProfile(def_profile, 0);
			break;
		}
	}
	return l_result;
}

void	SessionDeleteTmpProfile(MDB_Data *p_d, tCHAR *p_tmp_profile)
{
	p_d->prof_admin->DeleteProfile(p_tmp_profile, 0);

	ReleaseMutex(g_tmp_sync);
}

HRESULT	SessionGetProfileTable(MDB_Data *p_d, IMAPITable **p_table)
{
	WaitForSingleObject(g_tmp_sync, INFINITE);

	HRESULT l_result = p_d->prof_admin->GetProfileTable(0, p_table);

	ReleaseMutex(g_tmp_sync);
	return l_result;
}

tERROR	SessionPstCreate(MDB_Data *p_d)
{
	if( !p_d->hRealIO )
		return errOBJECT_NOT_CREATED;

	p_d->hNativeIO = CreateNativeIO(p_d->hRealIO);
	if( !p_d->hNativeIO )
		return errOBJECT_NOT_CREATED;

	CALL_SYS_PropertyGetStr(p_d->hNativeIO, NULL, pgOBJECT_FULL_NAME, p_d->name, MAX_PATH, cCP_ANSI);

	if( !*p_d->name )
		return errOBJECT_NOT_CREATED;

	p_d->is_read_only = GetFileAttributes(p_d->name);
	if( p_d->is_read_only & FILE_ATTRIBUTE_READONLY )
		if( !SetFileAttributes(p_d->name, p_d->is_read_only & ~FILE_ATTRIBUTE_READONLY) )
			return errOBJECT_NOT_CREATED;

	tCHAR l_tmp_profile[20] = {0};
	HRESULT l_result = SessionCreateTmpProfile(p_d, l_tmp_profile, NULL);

	// Get a service administration object.
	LPSERVICEADMIN l_svc_admin = NULL;
	if( !HR_FAILED(l_result) )
		l_result = p_d->prof_admin->AdminServices(l_tmp_profile, NULL, 0, 0, &l_svc_admin);

	// Add the MS Personal Information Store service to the temporary profile.
	if( !HR_FAILED(l_result) )
	{
		l_result = l_svc_admin->CreateMsgService("MSPST MS", "", 0, 0);
		if( l_result == MAPI_E_NOT_FOUND )
			l_result = l_svc_admin->CreateMsgService("INTERSTOR", "", 0, 0);
	}

	// Configure the MS Personal Information Store per NewPST entries.
	if( !HR_FAILED(l_result) )
	{
		IMAPITable *l_svc_tbl = NULL;
    	l_result = l_svc_admin->GetMsgServiceTable(0, &l_svc_tbl);

		SizedSPropTagArray(1, sptaColumns) = {1, {PR_SERVICE_UID}};
		if( !HR_FAILED(l_result) )
			l_result = l_svc_tbl->SetColumns((LPSPropTagArray)&sptaColumns, 0);

		LPSRowSet l_svc_row = NULL;
		if( !HR_FAILED(l_result) )
			l_result = l_svc_tbl->QueryRows(1, 0, &l_svc_row);

		LPMAPIUID l_muid = NULL;
		if( l_svc_row && l_svc_row->cRows )
		{
			SPropValue l_props[6];

			l_props[0].ulPropTag = PR_PST_PATH;
			l_props[0].Value.lpszA = p_d->name;

			l_props[1].ulPropTag = PR_PST_PW_SZ_OLD;
			l_props[1].Value.lpszA = p_d->password;

			l_props[2].ulPropTag = PR_PST_ENCRYPTION;
			l_props[2].Value.l = PSTF_NO_ENCRYPTION;

			l_props[3].ulPropTag = PR_DISPLAY_NAME;
			l_props[3].Value.lpszA = NULL;

			l_props[4].ulPropTag = PR_PST_REMEMBER_PW;
			l_props[4].Value.b = FALSE;

			l_props[5].ulPropTag = PR_PST_PW_SZ_NEW;
			l_props[5].Value.lpszA = p_d->password;
  
			LPMAPIUID l_muid = (LPMAPIUID)l_svc_row->aRow[0].lpProps->Value.bin.lpb;
			l_result = l_svc_admin->ConfigureMsgService(l_muid, 0, 0, 2, l_props);
		}
		if( l_svc_tbl )
			l_svc_tbl->Release();
		if( l_svc_row )
			FreeProws(l_svc_row);
	}

	if( !HR_FAILED(l_result) )
		l_result = MAPILogonEx(0, l_tmp_profile, NULL,
			MAPI_NO_MAIL|MAPI_NEW_SESSION|MAPI_EXPLICIT_PROFILE, (LPMAPISESSION FAR *)&p_d->session);

	MAPI_IFACE_CREATE(p_d->session, session);

	if( *l_tmp_profile )
		SessionDeleteTmpProfile(p_d, l_tmp_profile);

	if( l_svc_admin )
		l_svc_admin->Release();

	return HR_FAILED(l_result) ? errOBJECT_NOT_CREATED : errOK;
}

ULONG ReplaceProfileValue(HKEY key, tDWORD p_propid, tDWORD p_type, tPTR p_val)
{
	tCHAR key_name[MAX_PATH];
	sprintf_s(key_name, countof(key_name), "%.4x%.4x", PR_PROP_TYPE(p_propid), PROP_ID(p_propid));

	DWORD l_size, l_type;
	if( RegQueryValueEx(key, key_name, 0, &l_type, NULL, &l_size) != ERROR_SUCCESS )
		return ERROR_SUCCESS;

	if( p_type == REG_DWORD )
	{
		if( l_size == sizeof(DWORD) )
			return RegSetValueEx(key, key_name, 0, REG_BINARY, (PBYTE)&p_val, l_size);
	}
	else
	{
		if( p_type = l_type )
			return RegSetValueEx(key, key_name, 0, p_type, (PBYTE)p_val, strlen((tCHAR*)p_val)+1);
	}
	return E_FAIL;
}

HRESULT	ConfigureExchangeService(tCHAR *p_profile)
{
/*	SPropValue l_props[6];

	l_props[0].ulPropTag = PR_PROFILE_CONFIG_FLAGS;
	l_props[0].Value.l = 20;

	l_props[1].ulPropTag = PR_PROFILE_UI_STATE;
	l_props[1].Value.l = 512;

	HRESULT l_result = p_svc_admin->ConfigureMsgService(p_svc_uid, 0, 0, 1, l_props);
*/
	char l_profile_path[MAX_PATH];
	sprintf_s(l_profile_path, countof(l_profile_path), "%s\\%s", g_is_nt ? KEY_PROFILE_NT : KEY_PROFILE_9x, p_profile);

	HKEY l_profile_key = NULL;
	LONG l_ret = RegOpenKey(HKEY_CURRENT_USER, l_profile_path, &l_profile_key);

	char l_section_name[MAX_PATH];
	for(DWORD l_pos = 0; l_ret == ERROR_SUCCESS; l_pos++ )
	{
		if( (l_ret = RegEnumKey(l_profile_key, l_pos, l_section_name, MAX_PATH)) != ERROR_SUCCESS )
			break;

		HKEY l_sub_key = NULL;
		if( RegOpenKey(l_profile_key, l_section_name, &l_sub_key) != ERROR_SUCCESS )
			continue;

		// Use manual connection option
		ReplaceProfileValue(l_sub_key, PR_PROFILE_CONFIG_FLAGS, REG_DWORD, (tPTR)20);
		// Set work offline as default
		ReplaceProfileValue(l_sub_key, PR_PROFILE_UI_STATE, REG_DWORD, (tPTR)512);
		
		RegCloseKey(l_sub_key);
	}

	if( l_profile_key )
		RegCloseKey(l_profile_key);

	return l_ret == ERROR_SUCCESS ? S_OK : E_FAIL;
}

HRESULT	SessionProfileCreate(MDB_Data *p_d, tSTRING p_profile_name, tBOOL p_default, IMAPISession **p_sessiom)
{
	HRESULT l_result = S_OK;
	if( !p_profile_name )
	{
		if( !*p_d->name )
			return l_result;
		p_profile_name = p_d->name;
	}

	tCHAR l_tmp_profile[20] = {0};
	if( p_d->type != MAIL_OS_TYPE_OUTLOOK )
	{
		l_result = SessionCreateTmpProfile(p_d, l_tmp_profile, p_default ? p_profile_name : NULL);

		LPSERVICEADMIN l_tmp_svc_admin = NULL;
		if( !HR_FAILED(l_result) )
			l_result = p_d->prof_admin->AdminServices(l_tmp_profile, NULL, 0, 0, &l_tmp_svc_admin);

		LPSERVICEADMIN l_svc_admin = NULL;
		if( !HR_FAILED(l_result) )
			l_result = p_d->prof_admin->AdminServices(p_profile_name, NULL, 0, 0, &l_svc_admin);

		IMAPITable *l_svc_tbl = NULL;
		if( !HR_FAILED(l_result) )
			l_result = l_svc_admin->GetMsgServiceTable(0, &l_svc_tbl);

		SizedSPropTagArray(3, sptaService) = {3, {PR_SERVICE_UID, PR_SERVICE_NAME, PR_DISPLAY_NAME}};

		LPSRowSet l_service_rows = NULL;
		if( !HR_FAILED(l_result) )
			l_result = HrQueryAllRows(l_svc_tbl, (LPSPropTagArray)&sptaService, NULL, NULL, MAX_SERVICE_ROWS, &l_service_rows);

		for(DWORD i = 0; !HR_FAILED(l_result) && i < l_service_rows->cRows; i++)
		{
			SPropValue& pValue = l_service_rows->aRow[i].lpProps[1];
			if( (pValue.ulPropTag & PROP_TYPE_MASK) == PT_ERROR )
				continue;

			tSTRING l_service_code = pValue.Value.lpszA;

			bool f_exchange = !strcmp(l_service_code, "MSEMS");

			bool f_copy = f_exchange;
			f_copy |= !strcmp(l_service_code, "MSPST MS");
			f_copy |= !strcmp(l_service_code, "MSUPST MS");
			f_copy |= !strcmp(l_service_code, "INTERSTOR");
			f_copy |= !strcmp(l_service_code, "CONTAB");

			if( !f_copy )
				continue;

			LPMAPIUID l_service_uid = (LPMAPIUID)l_service_rows->aRow[i].lpProps[0].Value.bin.lpb;
			tSTRING l_service_name = l_service_rows->aRow[i].lpProps[2].Value.lpszA;

			HRESULT l_res = l_svc_admin->CopyMsgService(l_service_uid, l_service_name, NULL,
				&IID_IMsgServiceAdmin, l_tmp_svc_admin, NULL, 0);

			if( !HR_FAILED(l_res) && f_exchange )
				ConfigureExchangeService(l_tmp_profile);
		}

		if( l_service_rows )
			FreeProws(l_service_rows);
		if( l_svc_tbl )
			l_svc_tbl->Release();
		if( l_svc_admin )
			l_svc_admin->Release();
		if( l_tmp_svc_admin )
			l_tmp_svc_admin->Release();

		p_profile_name = l_tmp_profile;
	}

	if( !HR_FAILED(l_result) )
	{
		ULONG nUIHandle = NULL;
		DWORD l_flags = MAPI_EXPLICIT_PROFILE|MAPI_EXTENDED;
		if( p_d->type == MAIL_OS_TYPE_OUTLOOK )
		{
			nUIHandle = (ULONG)::GetActiveWindow();
			l_flags |= MAPI_LOGON_UI|MAPI_ALLOW_OTHERS|MAPI_NEW_SESSION;
		}
		else
		{
			l_flags |= MAPI_NEW_SESSION|MAPI_NO_MAIL|/*MAPI_LOGON_UI|*/MAPI_NT_SERVICE;
		}

		l_result = MAPILogonEx(nUIHandle, p_profile_name, NULL,
			l_flags, (LPMAPISESSION FAR *)p_sessiom);
	}

	if( p_profile_name == l_tmp_profile && *l_tmp_profile )
		SessionDeleteTmpProfile(p_d, l_tmp_profile);

	return l_result;
}

tERROR	SessionClose(MDB_Data *p_d, IMAPISession *&p_session)
{
	if( !p_session )
		return errOK;

	p_session->Logoff(0, 0, 0);
	MAPI_IFACE_RELEASE(p_session, session);
	return errOK;
}

tERROR	SessionCreate(MDB_Data *p_d)
{
	tERROR l_error = errOBJECT_CANNOT_BE_INITIALIZED;
	if( HANDLE hSys = g_WTSApi.GetSystemToken() )
	{
		CloseHandle(hSys);
		return l_error;
	}

	HKEY l_mail_key = NULL;
	LONG l_result = RegOpenKey(HKEY_LOCAL_MACHINE, KEY_MAIL_CLIENT, &l_mail_key);
	if( l_result != ERROR_SUCCESS )
		return l_error;

	tCHAR l_default_mail[MAX_PATH];
	ULONG l_size = sizeof(l_default_mail);

	WaitForSingleObject(g_tmp_sync, INFINITE);
	if( !g_isFault )
	{
		l_result = RegQueryValueEx(l_mail_key, NULL, 0, NULL, (LPBYTE)l_default_mail, &l_size);

		bool fDefault = false;
		if( l_result == ERROR_SUCCESS )
		{
			if( !strcmp(l_default_mail, KEY_OUTLOOK) )
				fDefault = true;
			else
				l_result = RegSetValueEx(l_mail_key, NULL, 0, REG_SZ, (LPBYTE)KEY_OUTLOOK, strlen(KEY_OUTLOOK)+1);
		}

		if( l_result == ERROR_SUCCESS )
		{
//			RegCloseKey(HKEY_CURRENT_USER);

			p_d->priority = GetThreadPriority(GetCurrentThread());
			if( p_d->priority != THREAD_PRIORITY_NORMAL )
				SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);

			g_WTSApi.ImpersonateToUser(p_d->token, p_d->tokenold);

			__try {
				MAPIINIT_0 MAPIINIT= { 0, MAPI_MULTITHREAD_NOTIFICATIONS|MAPI_NT_SERVICE};
				l_result = MAPIInitialize(&MAPIINIT);
			}
			__except(1)
			{
				PR_TRACE((g_root, prtFATAL, "MDB\t!!!!!!! MAPIInitialize EXCEPTION !!!!!!!"));
				l_result = E_FAIL;
			}

			if( !fDefault )
				RegSetValueEx(l_mail_key, NULL, 0, REG_SZ, (LPBYTE)l_default_mail, strlen(l_default_mail)+1);

			if( !FAILED(l_result) )
				l_result = MAPIAdminProfiles(0, &p_d->prof_admin);

			if( !FAILED(l_result) )
			{
				HookMAPIFeatures(TRUE, p_d->token ? p_d->token : p_d->tokenold, p_d->is_force_ui_suppress);
				if( p_d->type == MAIL_OS_TYPE_PST )
					l_error = SessionPstCreate(p_d);
				else
					l_error = SessionProfileCreate(p_d, NULL, cTRUE, &p_d->session);
			}
			else
				l_error = errOBJECT_NOT_CREATED;

			if( FAILED(l_result) )
				g_isFault = cTRUE;
		}
	}

	RegCloseKey(l_mail_key);
	ReleaseMutex(g_tmp_sync);
	return l_error;
}

tERROR	SessionDestroy(MDB_Data *p_d)
{
	SessionClose(p_d, p_d->session);

	if( p_d->prof_admin )
	{
		p_d->prof_admin->Release();
		HookMAPIFeatures(FALSE, NULL, p_d->is_force_ui_suppress);
		MAPIUninitialize();
	}

	tERROR l_error = errOK;

	if( p_d->fModified && p_d->hNativeIO != p_d->hRealIO )
	{
		l_error = CALL_SYS_PropertySetDWord(p_d->hRealIO, pgOBJECT_ACCESS_MODE, fACCESS_RW);
		if( PR_SUCC(l_error) )
			l_error = CopyIO(p_d->hRealIO, p_d->hNativeIO);
	}

	if( p_d->is_read_only & FILE_ATTRIBUTE_READONLY )
		SetFileAttributes(p_d->name, p_d->is_read_only);

	if( p_d->hNativeIO && p_d->hNativeIO != p_d->hRealIO )
	{
		CALL_SYS_ObjectClose(p_d->hNativeIO);
		DeleteFile(p_d->name);
	}

	if( p_d->priority != THREAD_PRIORITY_NORMAL )
		SetThreadPriority(GetCurrentThread(), p_d->priority);

	if( p_d->token )
	{
		SetThreadToken(NULL, p_d->tokenold);
		CloseHandle(p_d->token);
		p_d->token = NULL;
	}

	if( p_d->tokenold )
	{
		CloseHandle(p_d->tokenold);
		p_d->tokenold = NULL;
	}

//	RegCloseKey(HKEY_CURRENT_USER);
	return l_error;
}

//////////////////////////////////////////////////////////////////////////
// enum methods

tERROR	EnumCreate(MDBEnum_Data *p_d)
{
	p_d->ptrState = cObjPtrState_BEFORE;

	IMAPISession *l_session = MDB_DATA()->session;
	if( !l_session )
		p_d->tbltype = MAPI_PROFSECT;
	else
	{
		MAPI_IFACE_COPY(p_d->session, l_session, session);
		p_d->tbltype = MAPI_STORE;
	}
	return errOK;
}

tERROR	EnumDestroy(MDBEnum_Data *p_d)
{
	EnumReset(p_d);

	MAPI_IFACE_RELEASE(p_d->folder, folder);
	MAPI_IFACE_RELEASE(p_d->store, store);
	MAPI_IFACE_RELEASE(p_d->session, session);
	MAPI_BUFFER_FREE(p_d->profile);
	return errOK;
}

tERROR	EnumSetObject(MDBEnum_Data *p_d, MDBObj_Info *p_objinfo)
{
	if( !p_objinfo->type )
		return errOK;

	p_d->tbltype = p_objinfo->type;

	MAPI_BUFFER_COPY(p_d->profile, p_objinfo->profile);
	MAPI_IFACE_COPY(p_d->session, p_objinfo->session, session);
	MAPI_IFACE_COPY(p_d->store, p_objinfo->store, store);
	MAPI_IFACE_COPY(p_d->folder, p_objinfo->folder, folder);

	EnumReset(p_d);

	SPropValue *l_prop, l_prof_prop;
	if( p_d->tbltype == MAPI_PROFSECT )
	{
		if( !p_d->profile )
			return errOK;
		l_prop = &l_prof_prop;
		l_prop->ulPropTag = PR_DISPLAY_NAME;
		l_prop->Value.lpszA = p_d->profile;
	}
	else
	{
		if( !p_objinfo->entry )
			return errOK;
		l_prop = p_objinfo->entry;
	}

	return EnumFindObject(p_d, l_prop);
}

tERROR	EnumGetObject(MDBEnum_Data *p_d, MDBObj_Info *p_objinfo, tBOOL p_openobject)
{
	if( p_openobject )
	{
		tERROR error = EnumOpenObject(p_d);
		if( PR_FAIL(error) )
			return error;
	}

	p_objinfo->hOS = p_d->hOS;
	p_objinfo->profile = p_d->profile;
	p_objinfo->session = p_d->session;
	p_objinfo->store = p_d->store;
	p_objinfo->folder = p_d->folder;
	p_objinfo->object = p_d->object;
	p_objinfo->type = p_d->tbltype;
	p_objinfo->entry = p_d->objdata ? &p_d->objdata->aRow[0].lpProps[0] : NULL;

	if( p_d->tbltype != MAPI_MESSAGE )
	{
		p_objinfo->name = p_d->objdata ? p_d->objdata->aRow[0].lpProps[1].Value.lpszA : NULL;
		if( p_d->tbltype == MAPI_FOLDER )
			if( p_d->objdata )
			{
				if( (p_d->objdata->aRow[0].lpProps[2].ulPropTag & PROP_TYPE_MASK) == PT_ERROR )
					p_objinfo->issubfolders = p_d->objdata->aRow[0].lpProps[3].Value.b;
				else
					p_objinfo->issubfolders = p_d->objdata->aRow[0].lpProps[2].Value.l;
			}
			else
				p_objinfo->issubfolders = cTRUE;
	}

	return errOK;
}

tERROR	EnumOpenTable(MDBEnum_Data *p_d)
{
	HRESULT l_result = E_FAIL;

	p_d->fFolder = cTRUE;
	SizedSPropTagArray(4, sptaColumns) = {2, {PR_ENTRYID, PR_DISPLAY_NAME}};
	SizedSSortOrderSet(1, sstaColumns) = {0, 0, 0, {PR_DISPLAY_NAME, TABLE_SORT_ASCEND}};

	switch( p_d->tbltype )
	{
	case MAPI_PROFSECT:
		sptaColumns.aulPropTag[0] = PR_DEFAULT_PROFILE;
		l_result = SessionGetProfileTable(MDB_DATA(), &p_d->table);
		break;
	case MAPI_STORE:
		sptaColumns.cValues = 3;
		sptaColumns.aulPropTag[2] = PR_MDB_PROVIDER;
		l_result = p_d->session->GetMsgStoresTable(0, &p_d->table);
		break;
	case MAPI_FOLDER:
		sstaColumns.cSorts = 1;
		sptaColumns.cValues = 4;
		sptaColumns.aulPropTag[2] = PR_FOLDER_CHILD_COUNT;
		sptaColumns.aulPropTag[3] = PR_SUBFOLDERS;
		l_result = p_d->folder->GetHierarchyTable(0, &p_d->table);
		break;
	case MAPI_MESSAGE:
		sptaColumns.cValues = 2;
		sptaColumns.aulPropTag[1] =  PROP_TAG( PT_LONG, 0x800C);
		l_result = p_d->folder->GetContentsTable(0, &p_d->table);
		p_d->fFolder = cFALSE;
		break;
	}

	MAPI_IFACE_CREATE(p_d->table, table);

	if( l_result == MAPI_E_NO_SUPPORT )
	{
		PR_TRACE((p_d->hOS, prtERROR, "MDB\tCann't open table : not supported"));
		p_d->ptrState = cObjPtrState_AFTER;
		return errNOT_SUPPORTED;
	}

	if( l_result == E_ACCESSDENIED )
	{
		PR_TRACE((p_d->hOS, prtERROR, "MDB\tCann't open table : access denied"));
		p_d->ptrState = cObjPtrState_AFTER;
		return errACCESS_DENIED;
	}

	if( !p_d->table )
	{
		p_d->ptrState = cObjPtrState_UNDEFINED;
		return errOBJECT_INVALID;
	}

    if( !HR_FAILED(l_result) )
		l_result = p_d->table->SetColumns((LPSPropTagArray)&sptaColumns, 0);
	if( !HR_FAILED(l_result) && sstaColumns.cSorts )
		p_d->table->SortTable((LPSSortOrderSet)&sstaColumns, 0);
    if( !HR_FAILED(l_result) )
		l_result = p_d->table->SeekRow( BOOKMARK_BEGINNING, 0, NULL);
	return l_result == hrSuccess ? errOK : errOBJECT_INVALID;
}

tERROR	EnumStoreCheck(MDBEnum_Data *p_d, SBinary p_mdb_provider)
{
	if( !MDB_DATA()->is_public_folders && MDB_DATA()->type != MAIL_OS_TYPE_OUTLOOK )
		if( !memcmp(p_mdb_provider.lpb, pbExchangeProviderPublicGuid, p_mdb_provider.cb) )
			return errNOT_OK;

/*	if( !MDB_DATA()->is_private_folders )
		if( !memcmp(p_mdb_provider.lpb, pbExchangeProviderPrimaryUserGuid, p_mdb_provider.cb) )
			return errNOT_OK;
*/
	return errOK;
}

tERROR	EnumFindObject(MDBEnum_Data *p_d, SPropValue *p_prop)
{
	EnumClearObject(p_d);
	MAPI_IFACE_RELEASE(p_d->table, table);

	if( PR_FAIL(EnumOpenTable(p_d)) )
		return errOBJECT_INCOMPATIBLE;

	if( p_d->tbltype != MAPI_MESSAGE || p_prop->ulPropTag != PR_DISPLAY_NAME )
	{
		SRestriction l_restr;
		l_restr.rt = RES_PROPERTY;
		l_restr.res.resProperty.relop = RELOP_EQ;
		l_restr.res.resProperty.ulPropTag = p_prop->ulPropTag;
		l_restr.res.resProperty.lpProp = p_prop;

		if( !HR_FAILED(p_d->table->FindRow(&l_restr, BOOKMARK_BEGINNING, 0)) )
		{
			tERROR error = EnumNextObject(p_d);
			if( PR_SUCC(error) )
				error = EnumOpenObject(p_d);
			return error;
		}
	}
	else
	{
		tCHAR l_buff[0x1000];
		while( PR_SUCC(EnumNext(p_d)) )
		{
			PRProp_Buff l_prm_buff = {l_buff, sizeof(l_buff), tid_STRING, 0};
			if( PR_SUCC(MessageGetName(p_d->object, &l_prm_buff)) )
				if( !strcmp(p_prop->Value.lpszA, l_buff) )
					return errOK;
		}
	}

	if( p_d->tbltype == MAPI_FOLDER && p_prop->ulPropTag == PR_DISPLAY_NAME )
	{
		p_d->tbltype = MAPI_MESSAGE;
		return EnumFindObject(p_d, p_prop);
	}

	p_d->ptrState = cObjPtrState_UNDEFINED;
	return errOBJECT_INCOMPATIBLE;
}

tERROR	EnumOpenObject(MDBEnum_Data *p_d)
{
	if( p_d->objentry )
		return (p_d->object || p_d->objtype == MAPI_PROFSECT) ? errOK : errOBJECT_NOT_CREATED;

	if( !p_d->objdata )
		return errOK;

	p_d->objentry = &p_d->objdata->aRow[0].lpProps[0];

	HRESULT l_result = S_OK;
	if( p_d->tbltype == MAPI_PROFSECT )
	{
		if( !p_d->session )
		{
			MAPI_BUFFER_COPY(p_d->profile, p_d->objdata->aRow[0].lpProps[1].Value.lpszA);
			tBOOL p_default = p_d->objdata->aRow[0].lpProps[0].Value.b;

			l_result = SessionProfileCreate(MDB_DATA(), p_d->profile, p_default, &p_d->session);
			if( HR_FAILED(l_result) )
				PR_TRACE((p_d->hOS, prtERROR, "MDB\tCann't open session error : %08X", l_result));

			MAPI_IFACE_CREATE(p_d->session, session);
		}

		if( p_d->session )
			p_d->objtype = MAPI_PROFSECT;
	}
	else if( p_d->tbltype == MAPI_STORE )
	{
		if( !p_d->store )
		{
			if( !g_isFault )
			{
				// this code needs to fix problem whis "MAPI_E_FAILONEPROVIDER" error 
				WaitForSingleObject(g_tmp_sync, INFINITE);
//				__try {
					for(int i = 0; i < 5; i++ )
					{
						DWORD l_flags = MAPI_BEST_ACCESS|MDB_NO_MAIL;
						if( MDB_DATA()->type != MAIL_OS_TYPE_OUTLOOK )
							l_flags |= MDB_TEMPORARY|MDB_NO_DIALOG|MAPI_DEFERRED_ERRORS;

						l_result = p_d->session->OpenMsgStore((ULONG)::GetActiveWindow(), 
								p_d->objentry->Value.bin.cb,
								(LPENTRYID)p_d->objentry->Value.bin.lpb,
								NULL, l_flags,
								&p_d->store
							);

						if( l_result != MAPI_E_FAILONEPROVIDER )
							break;
						Sleep(50);
					}
/*				}
				__except(1) {
					PR_TRACE((p_d->hOS, prtFATAL, "MDB\t!!!!!!! OpenMsgStore EXCEPTION !!!!!!!"));
					g_isFault = TRUE;
					l_result = MAPI_E_FAILONEPROVIDER;
				}
*/				ReleaseMutex(g_tmp_sync);

				MAPI_IFACE_CREATE(p_d->store, store);
			}
			else
				l_result = MAPI_E_FAILONEPROVIDER;
		}

		if( p_d->store )
		{
			p_d->objtype = MAPI_STORE;
			MAPI_IFACE_COPY(p_d->object, p_d->store, object);
		}
	}
	else
	{
		l_result = p_d->session->OpenEntry(
				p_d->objentry->Value.bin.cb,
				(LPENTRYID)p_d->objentry->Value.bin.lpb,
				NULL, MAPI_BEST_ACCESS,
				(ULONG*)&p_d->objtype,
				(LPUNKNOWN*)&p_d->object
			);

		MAPI_IFACE_CREATE(p_d->object, object)
	}

	if( FAILED(l_result) )
		PR_TRACE((p_d->hOS, prtERROR, "MDB\tCan't open mail object. Error : %08X", l_result));

	if( l_result == E_ACCESSDENIED ||
		l_result == MAPI_E_LOGON_FAILED ||
		l_result == MAPI_E_NOT_FOUND ||
		l_result == MAPI_E_UNCONFIGURED ||
		l_result == MAPI_E_FAILONEPROVIDER )
		return errACCESS_DENIED;

	return FAILED(l_result) ? errUNEXPECTED : errOK;
}

tERROR	EnumCloneObject(MDBEnum_Data *p_d, MDBEnum_Data *p_src)
{
	p_d->tbltype = p_src->tbltype;

	MAPI_BUFFER_COPY(p_d->profile, p_src->profile);
	MAPI_IFACE_COPY(p_d->session, p_src->session, session);
	MAPI_IFACE_COPY(p_d->store, p_src->store, store);
	MAPI_IFACE_COPY(p_d->folder, p_src->folder, folder);

	if( !p_src->table )
		return errOK;

	tERROR error = EnumOpenTable(p_d);
	if( PR_SUCC(error) )
	{
		ULONG pos = 0, pos1, pos2;
		HRESULT result = p_src->table->QueryPosition(&pos, &pos1, &pos2);
		if( !pos )
			result = E_FAIL;
		if( !FAILED(result) )
			result = p_d->table->SeekRow(BOOKMARK_BEGINNING, pos-1, NULL);
		if( FAILED(result) )
			error = errUNEXPECTED;
	}
		
	return EnumNextObject(p_d);
}

tERROR	EnumNextObject(MDBEnum_Data *p_d)
{
	if( !p_d->table )
	{
		tERROR l_err = EnumOpenTable(p_d);
		if( l_err == errNOT_SUPPORTED || l_err == errACCESS_DENIED )
			return errEND_OF_THE_LIST;
		if( PR_FAIL(l_err) )
		{
			p_d->ptrState = cObjPtrState_UNDEFINED;
			return errOBJECT_BAD_INTERNAL_STATE;
		}
	}

	HRESULT l_result = p_d->table->QueryRows(1, 0, &p_d->objdata);

	if( !p_d->objdata || HR_FAILED(l_result) )
	{
		p_d->ptrState = cObjPtrState_UNDEFINED;
		return errOBJECT_BAD_INTERNAL_STATE;
	}

	if( !p_d->objdata->cRows )
	{
		p_d->objdata = NULL;

		if( p_d->tbltype == MAPI_FOLDER)
		{
			MAPI_IFACE_RELEASE(p_d->table, table);
			p_d->tbltype = MAPI_MESSAGE;
			return errACCESS_DENIED;
		}
		else
		{
			p_d->ptrState = cObjPtrState_AFTER;
			return errEND_OF_THE_LIST;
		}
	}

	if( (p_d->objdata->aRow[0].lpProps[1].ulPropTag & PROP_TYPE_MASK) == PT_ERROR )
	{
		if( p_d->tbltype != MAPI_MESSAGE )
			return errACCESS_DENIED;
	}
	else
	{
		 if( p_d->tbltype == MAPI_MESSAGE && p_d->objdata->aRow[0].lpProps[1].Value.l )
			 return errACCESS_DENIED;
	}

	if( p_d->tbltype == MAPI_STORE )
	{
		if( PR_FAIL(EnumStoreCheck(p_d, p_d->objdata->aRow[0].lpProps[2].Value.bin)) )
		{
			PR_TRACE((p_d->hOS, prtERROR, "MDB\tMail store skipped by type"));
			return errACCESS_DENIED;
		}
	}

	p_d->ptrState = cObjPtrState_PTR;
	return errOK;
}

tERROR	EnumClearObject(MDBEnum_Data *p_d)
{
	if( p_d->objdata )
	{
		FreeProws(p_d->objdata);
		p_d->objdata = NULL;
		p_d->objentry = NULL;
	}
	MAPI_IFACE_RELEASE(p_d->object, object);

	if( p_d->tbltype == MAPI_STORE )
		MAPI_IFACE_RELEASE(p_d->store, store);

	if( p_d->tbltype == MAPI_PROFSECT )
	{
		MAPI_IFACE_RELEASE(p_d->store, store);
		SessionClose(MDB_DATA(), p_d->session);
	}
	return errOK;
}

tERROR	EnumReset(MDBEnum_Data *p_d)
{
	EnumClearObject(p_d);

	MAPI_IFACE_RELEASE(p_d->table, table);

	p_d->ptrState = cObjPtrState_BEFORE;
	if( p_d->tbltype == MAPI_MESSAGE )
		p_d->tbltype = MAPI_FOLDER;

	return errOK;
}

tERROR	EnumNext(MDBEnum_Data *p_d)
{
	if( p_d->ptrState == cObjPtrState_UNDEFINED )
		return errOBJECT_BAD_INTERNAL_STATE;

	if( p_d->ptrState == cObjPtrState_AFTER )
		return errEND_OF_THE_LIST;

	tERROR error = errACCESS_DENIED;
	while( error == errACCESS_DENIED )
	{
		EnumClearObject(p_d);
		error = EnumNextObject(p_d);
	}
	return error;
}

tERROR	EnumStepUp(MDBEnum_Data *p_d)
{
	tERROR l_error = errOBJECT_INCOMPATIBLE;

	MDBObj_Info l_objinfo;
	memset(&l_objinfo, 0, sizeof(MDBObj_Info));
	l_objinfo.profile = p_d->profile;
	l_objinfo.session = p_d->session;

	IMAPIFolder *l_owner = NULL;
	LPSPropValue l_entry = NULL;

	switch( p_d->tbltype )
	{
	case MAPI_PROFSECT:
		return l_error;
	case MAPI_STORE:
		if( MDB_DATA()->session )
			return l_error;
		l_objinfo.type = MAPI_PROFSECT;
		break;
	case MAPI_FOLDER:
	case MAPI_MESSAGE:
		if( PR_FAIL(FolderGetOwner(&l_objinfo, p_d->folder, &l_owner)) )
			return l_error;
		if( l_owner )
		{
			l_objinfo.store = p_d->store;
			l_objinfo.type = MAPI_FOLDER;
			ObjectGetSProp(p_d->folder, PR_ENTRYID, &l_entry);
		}
		else
		{
			l_objinfo.type = MAPI_STORE;
			ObjectGetSProp(p_d->store, PR_ENTRYID, &l_entry);
		}
		l_objinfo.folder = l_owner;
		l_objinfo.entry = l_entry;
		break;
	}

	l_error = EnumSetObject(p_d, &l_objinfo);

	MAPI_BUFFER_FREE(l_entry);
	MAPI_IFACE_RELEASE(l_owner, folder);

	return l_error;
}

tERROR	EnumStepDown(MDBEnum_Data *p_d)
{
	tERROR error = EnumOpenObject(p_d);
	if( PR_FAIL(error) )
		return error;

	switch( p_d->objtype )
	{
	case MAPI_PROFSECT:
		p_d->tbltype = MAPI_STORE;
		break;
	case MAPI_STORE:
		{
		IMAPIFolder *l_folder = NULL;
		ULONG   l_objtype;
		HRESULT l_result;
		if( MDB_DATA()->type != MAIL_OS_TYPE_OUTLOOK )
		{
			l_result = p_d->store->OpenEntry(0, NULL, NULL, MAPI_BEST_ACCESS,
				&l_objtype, (LPUNKNOWN*)&l_folder);
		}
		else
		{
			LPSPropValue l_root_entry;
			l_result = ObjectGetSProp(p_d->store, PR_IPM_SUBTREE_ENTRYID, &l_root_entry);
			if( !FAILED(l_result) )
				l_result = p_d->store->OpenEntry(l_root_entry->Value.bin.cb, (LPENTRYID)l_root_entry->Value.bin.lpb,
					NULL, MAPI_BEST_ACCESS, &l_objtype, (LPUNKNOWN*)&l_folder);
		}

		if( FAILED(l_result) )
			return errOBJECT_INCOMPATIBLE;

		MAPI_IFACE_CREATE(l_folder, folder);
		MAPI_IFACE_COPY(p_d->folder, l_folder, folder);
		MAPI_IFACE_RELEASE(l_folder, folder);

		p_d->tbltype = MAPI_FOLDER;
		}
		break;
	case MAPI_FOLDER:
		if( !p_d->object )
			return errOBJECT_INVALID;
		p_d->tbltype = MAPI_FOLDER;
		MAPI_IFACE_COPY(p_d->folder, (IMAPIFolder*)p_d->object, folder);
		break;
	case MAPI_MESSAGE:
		return errOBJECT_INCOMPATIBLE;
	}

	return EnumReset(p_d);
}

tERROR	EnumChangeTo(MDBEnum_Data *p_d, hSTRING p_string, tDWORD p_open_mode)
{
	tERROR l_error = errOK;

	tDWORD l_len;
	CALL_String_LengthEx(p_string, &l_len, cSTRING_WHOLE, cCP_ANSI, cSTRING_Z);

	tCHAR *l_buff = NULL;
	MAPIAllocateBuffer(l_len, (LPVOID*)&l_buff);

	CALL_String_ExportToBuff(p_string, NULL, cSTRING_WHOLE, l_buff, l_len, cCP_ANSI, cSTRING_Z);

	SPropValue l_prop;
	l_prop.ulPropTag = PR_DISPLAY_NAME;

	char *l_name = l_buff;
	while( PR_SUCC(l_error) && l_name && *l_name )
	{
		char *l_next = strchr(l_name, PRBUFF_DIRSLASH);
		if( l_next )
			*l_next++ = 0;

		l_prop.Value.lpszA = l_name;
		l_error = EnumFindObject(p_d, &l_prop);

		if( l_next )
		{
			if( PR_SUCC(l_error) )
				l_error = EnumStepDown(p_d);
		}
		else if( PR_FAIL(l_error) && (p_open_mode & fOMODE_CREATE_IF_NOT_EXIST) )
		{
			IMAPIFolder* l_folder = NULL;
			if( p_d->folder && !FAILED(p_d->folder->CreateFolder(FOLDER_GENERIC, l_name, NULL, NULL, 0, &l_folder)) )
			{
				l_folder->Release();
				l_error = errOK;
			}
		}

		l_name = l_next;
	}

	if( l_buff )
		MAPIFreeBuffer(l_buff);

	return l_error;
}

tERROR	EnumChangeToData(MDBEnum_Data *p_d, tCHAR *p_buffer, tDWORD p_size)
{
	SPropValue l_prop;
	tDWORD l_size;

// Load profile
	l_size = *(tDWORD*)p_buffer;
	p_buffer += sizeof(tDWORD);

	bool f_eq_profile = !l_size || (p_d->profile && !strcmp(p_buffer, p_d->profile));

	if( !f_eq_profile )
	{
		p_d->tbltype = MAPI_PROFSECT;
		EnumReset(p_d);

		l_prop.ulPropTag = PR_DISPLAY_NAME;
		l_prop.Value.lpszA = p_buffer;

		if( PR_FAIL(EnumFindObject(p_d, &l_prop)) )
			return errOBJECT_NOT_FOUND;
	}
	p_buffer += l_size;

// Load store
	tDWORD l_key_size = *(tDWORD*)p_buffer;
	p_buffer += sizeof(tDWORD);

	bool f_eq_store = f_eq_profile;
	if( f_eq_store )
	{
		LPSPropValue l_record_key = NULL;

		if( !p_d->store ||
			PR_FAIL(ObjectGetSProp(p_d->store, PR_RECORD_KEY, &l_record_key)) ||
			memcmp(l_record_key->Value.bin.lpb, p_buffer, l_key_size) )
			f_eq_store = false;

		if( l_record_key )
			MAPIFreeBuffer(l_record_key);
	}

	if( !f_eq_store )
	{
		p_d->tbltype = MAPI_STORE;
		if( f_eq_profile )
			EnumReset(p_d);

		l_prop.ulPropTag = PR_RECORD_KEY;
		l_prop.Value.bin.cb = l_key_size;
		l_prop.Value.bin.lpb = (BYTE*)p_buffer;

		if( PR_FAIL(EnumFindObject(p_d, &l_prop)) )
			return errOBJECT_NOT_FOUND;
	}
	p_buffer += l_key_size;

// Load folder
	l_size = *(tDWORD*)p_buffer;
	p_buffer += sizeof(tDWORD);

	p_d->tbltype = MAPI_FOLDER;
	if( f_eq_store )
		EnumReset(p_d);

	MAPI_IFACE_RELEASE(p_d->folder, folder);

	if( HR_FAILED(p_d->store->OpenEntry(l_size, (LPENTRYID)p_buffer, NULL, MAPI_BEST_ACCESS,
			(ULONG*)&p_d->objtype, (LPUNKNOWN*)&p_d->folder)) )
		return errOBJECT_NOT_FOUND;

	MAPI_IFACE_CREATE(p_d->folder, folder);

	p_buffer += l_size;

// Load message
	l_size = *(tDWORD*)p_buffer;
	p_buffer += sizeof(tDWORD);

	EnumReset(p_d);
	p_d->tbltype = MAPI_MESSAGE;

	l_prop.ulPropTag = PR_ENTRYID;
	l_prop.Value.bin.cb = l_size;
	l_prop.Value.bin.lpb = (BYTE*)p_buffer;

	return EnumFindObject(p_d, &l_prop);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

tERROR	MessageCreate(MDBIO_Data *p_d)
{
	if( !p_d->message )
		return errOBJECT_NOT_CREATED;

	p_d->Origin = OID_MAIL_MSG_REF;
	return errOK;
}

tERROR	MessageSetObject(MDBIO_Data *p_d, MDBObj_Info *p_objinfo)
{
	MAPI_BUFFER_COPY(p_d->profile, p_objinfo->profile);
	MAPI_IFACE_COPY(p_d->session, p_objinfo->session, session);
	MAPI_IFACE_COPY(p_d->store, p_objinfo->store, store);
	MAPI_IFACE_COPY(p_d->folder, p_objinfo->folder, folder);
	MAPI_IFACE_COPY(p_d->message, (IMessage*)p_objinfo->object, message);
	return errOK;
}

tERROR	MessageGetObject(MDBIO_Data *p_d, MDBObj_Info *p_objinfo)
{
	p_objinfo->hOS = p_d->hOS;
	p_objinfo->profile = p_d->profile;
	p_objinfo->session = p_d->session;
	p_objinfo->store = p_d->store;
	p_objinfo->folder = p_d->folder;
	p_objinfo->type = MAPI_MESSAGE;
	p_objinfo->object = p_d->message;

	if( !p_d->entry )
		if( PR_FAIL(ObjectGetSProp(p_d->message, PR_ENTRYID, &p_d->entry)) )
			return errNOT_OK;

	p_objinfo->entry = p_d->entry;
	return errOK;
}

tERROR	MessageLoadData(MDBIO_Data *p_d, tCHAR *p_buffer, tDWORD p_size)
{
	hObjPtr &l_objptr = CUST_TO_MDB_Data(p_d->hOS)->tmpenum;
	if( !l_objptr )
		CALL_SYS_ObjectCreateQuick(p_d->hOS, &l_objptr, IID_OBJPTR, PID_MDB, 0);

	tERROR l_error = EnumChangeToData(CUST_TO_MDBEnum_Data(l_objptr), p_buffer, p_size);
	if( PR_FAIL(l_error) )
		return l_error;

	MDBObj_Info l_obj_info;
	EnumGetObject(CUST_TO_MDBEnum_Data(l_objptr), &l_obj_info, cTRUE);
	return MessageSetObject(p_d, &l_obj_info);
}

tERROR	MessageSaveData(MDBIO_Data *p_d, tDWORD* p_out_size, tCHAR *p_buffer, tDWORD p_size)
{
	PRProp_Buff l_buff = {p_buffer, p_size, tid_PTR, 0};

	PRBuffSetSTRING(&l_buff, p_d->profile);
	ObjectGetProp(p_d->store, PR_RECORD_KEY, tid_PTR, &l_buff);
	ObjectGetProp(p_d->folder, PR_ENTRYID, tid_PTR, &l_buff);
	ObjectGetProp(p_d->message, PR_ENTRYID, tid_PTR, &l_buff);

	*p_out_size = l_buff.out_size;

	return l_buff.out_size > p_size ? errBUFFER_TOO_SMALL : errOK;
}

tERROR	MessageDestroy(MDBIO_Data *p_d)
{
	MAPI_BUFFER_FREE(p_d->entry);
	MAPI_IFACE_RELEASE(p_d->message, message);
	MAPI_IFACE_RELEASE(p_d->folder, folder);
	MAPI_IFACE_RELEASE(p_d->store, store);
	MAPI_IFACE_RELEASE(p_d->session, session);
	MAPI_BUFFER_FREE(p_d->profile);
	return errOK;
}

//////////////////////////////////////////////////////////////////////////

tERROR  FolderGetOwner(MDBObj_Info *p_d, IMAPIFolder *p_obj, IMAPIFolder **p_owner)
{
	if( !p_obj )
		return errOBJECT_INVALID;

	SizedSPropTagArray(2, sptaColumns) = {2, {PR_FOLDER_TYPE, PR_PARENT_ENTRYID}};

	LPSPropValue l_props = NULL; ULONG l_num = 0;
	if( HR_FAILED(p_obj->GetProps((LPSPropTagArray)&sptaColumns, 0, &l_num, &l_props)) || !l_props )
		return errOBJECT_INVALID;

	HRESULT l_result = MAPI_E_NOT_FOUND;

	ULONG l_objtype;
	if( l_props[0].Value.ul != FOLDER_ROOT )
	{
		if( p_d->store )
			l_result = p_d->store->OpenEntry(l_props[1].Value.bin.cb, (LPENTRYID)l_props[1].Value.bin.lpb,
					NULL, MAPI_DEFERRED_ERRORS, &l_objtype, (LPUNKNOWN*)p_owner);

		if( !p_owner && p_d->session )
			l_result = p_d->session->OpenEntry(l_props[1].Value.bin.cb, (LPENTRYID)l_props[1].Value.bin.lpb,
					NULL, MAPI_DEFERRED_ERRORS, &l_objtype, (LPUNKNOWN*)p_owner);
	}

	if( l_result == MAPI_E_NOT_FOUND )
	{
		l_result = hrSuccess;
		*p_owner = NULL;
	}

	MAPI_IFACE_CREATE(*p_owner, folder);

	MAPIFreeBuffer(l_props);
	return l_result == hrSuccess ? errOK : errOBJECT_INVALID;
}

tERROR	FolderGetPath(MDBObj_Info *p_d, IMAPIFolder *p_obj, PRProp_Buff *p_buff, tBOOL b_root)
{
	tERROR l_error;

	IMAPIFolder *l_owner = NULL;
	if( PR_FAIL(l_error = FolderGetOwner(p_d, p_obj, &l_owner)) )
		return l_error;

	if( !l_owner )
		return errEOF;

	l_error = FolderGetPath(p_d, l_owner, p_buff, b_root);

	if( l_error == errOK || b_root )
	{
		l_error = ObjectGetProp(p_obj, PR_DISPLAY_NAME, tid_CHAR, p_buff);
		if( PR_SUCC(l_error) )
			l_error = PRBuffSetCHAR(p_buff, PRBUFF_DIRSLASH);
	}

	MAPI_IFACE_RELEASE(l_owner, folder);

	return PR_SUCC(l_error) ? errOK : l_error;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
