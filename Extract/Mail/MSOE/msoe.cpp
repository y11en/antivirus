//////////////////////////////////////////////////////////////////////////
// Source file for OE objects

#include "msoe.h"
#include "plugin_msoe.h"
#include <Extract/iface/i_mailmsg.h>
#include <Prague/iface/i_hash.h>
#include <Prague/iface/i_token.h>
#include <string>
#include <shlobj.h>
#include "hook.h"
#include <Prague/pr_remote.h>
#include <Core\remote\pr_process.h>
#include <malloc.h>

#include <hash/md5/plugin_hash_md5.h>
#include <nfio/win32_io.h>

#define QUERY_SIZE			0x080
#define READ_CHUNCK_SIZE	0x1000
#define READ_EML_SIGNATURE	20
#define READ_DBX_SIGNATURE	14

extern IStream * CreateIStreamIO(hIO p_io, tBOOL p_del_io);

WTSApi g_WTSApi;

//////////////////////////////////////////////////////////////////////////
// common OE objects methods

static hROOT g_root = NULL;

//////////////////////////////////////////////////////////////////////////
// helpers

tERROR CopyIO(hIO dst, hIO src)
{
	tQWORD l_offset = 0;
	tBYTE  l_buff[READ_CHUNCK_SIZE];
	tDWORD l_read_size = sizeof(l_buff);

	tERROR l_error = CALL_IO_SetSize(dst, 0);

	while( PR_SUCC(l_error) && l_read_size == sizeof(l_buff) )
	{
		l_error = CALL_IO_SeekRead(src, &l_read_size, l_offset, l_buff, sizeof(l_buff));
		if( PR_SUCC(l_error) )
			l_error = CALL_IO_SeekWrite(dst, NULL, l_offset, l_buff, l_read_size);
		l_offset += l_read_size;

		if( !(l_offset % (sizeof(l_buff) * 10)) )
			if( CALL_SYS_SendMsg(src, pmc_PROCESSING, pm_PROCESSING_YIELD, NULL, NULL, NULL) == errOPERATION_CANCELED )
				l_error = errOPERATION_CANCELED;
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

void	PROEObject::Initialize(hROOT root)
{
	g_root = root;
	CThreadContext::Init();
}

void	PROEObject::Deinitialize()
{
	CThreadContext::Done();
}

void	PROEObject::ThreadAttach(bool fAttach)
{
}

tDWORD	PROEObject::Recognize(hOBJECT p_that, tDWORD type)
{
	if( !p_that )
		return TYPE_UNDEF;

	if( PR_FAIL(CALL_SYS_ObjectCheck(p_that, p_that, IID_IO, PID_ANY, SUBTYPE_ANY, cFALSE)) )
		return TYPE_UNDEF;

	bool f_check_eml = !type || type == MSOE_MESSAGE;
	bool f_check_dbx = !type || type == MSOE_DATABASE;

	if( f_check_eml && CALL_SYS_PropertyGetDWord(p_that, pgPLUGIN_ID) == PID_MSOE )
	{
		PROEObject *l_obj = IO_GetObject((hIO)p_that);

		tDWORD l_ret_type = TYPE_UNDEF;
		switch( l_obj->m_type )
		{
		case MSOE_MESSAGE:
			l_ret_type = MSOE_MESSAGE_OBJ;
			break;
		case MSOE_MESSAGE_PART:
			if( static_cast<PROEMessagePart*>(l_obj)->m_origin == OID_MAIL_MSG_MIME )
				l_ret_type = MSOE_MESSAGE;
			break;
		}
		l_obj->Release();

		if( l_ret_type != TYPE_UNDEF )
			return l_ret_type;
	}

	tBYTE	l_buff[READ_EML_SIGNATURE];
	tDWORD	l_read_size;

	if( PR_FAIL(CALL_IO_SeekRead((hIO)p_that, &l_read_size, 0, l_buff,
			f_check_eml ? READ_EML_SIGNATURE : READ_DBX_SIGNATURE)) )
		return TYPE_UNDEF;

	if( f_check_dbx )
	{
		static tBYTE	g_dbx_signature[] = {0xCF, 0xAD, 0x12, 0xFE, 0xC5, 0xFD, 0x74, 0x6F, 0x66, 0xE3, 0xD1, 0x11, 0x9A, 0x4E};
		static tDWORD	g_dbx_signature_size = sizeof(g_dbx_signature);

		if( l_read_size >= g_dbx_signature_size )
			if( !memcmp(l_buff, g_dbx_signature, g_dbx_signature_size) )
				return MSOE_DATABASE;
	}
	if( f_check_eml /*&& l_read_size*/ )
	{
		if( CALL_SYS_PropertyGetDWord(p_that, pgOBJECT_ORIGIN) == OID_MAIL_MSG_MIME )
			return MSOE_MESSAGE;

		static const tCHAR * g_eml_header[] = {
			"From: ",
			"Received: ",
			"Message-ID: ",
			"MIME-Version: ",
			/*"Subject: ",*/
			"Content-Type: ",
			"Return-Path: "
		};
		
		for(int i = 0; i < sizeof(g_eml_header)/sizeof(tCHAR*); i++)
		{
			tDWORD len = (tDWORD)strlen(g_eml_header[i]);
			if( len >= l_read_size )
				continue;

			if( !memcmp(l_buff, g_eml_header[i], len) )
				return MSOE_MESSAGE;
		}
	}
	return TYPE_UNDEF;
}

PROEObject * PROEObject::Construct(tDWORD p_type, hOBJECT p_parent)
{
	PROEObject *l_obj = NULL;
	switch( p_type )
	{
	case MSOE_MAILSYSTEM:	
		if( (DWORD)(LOBYTE(LOWORD(GetVersion()))) <= 5 )
			l_obj = new PROEMailSystem(); 
		else
			l_obj = new PROEVistaMail(); 
		break;

	case MSOE_DATABASE:		l_obj = new PROEDatabase((hIO)p_parent); break;
	case MSOE_MESSAGE:		l_obj = new PROEIOMessage((hIO)p_parent); break;
	case MSOE_MESSAGE_OBJ:	l_obj = IO_GetObject((hIO)p_parent); break;
	default : return NULL;
	}
	if( !l_obj->CheckInit() )
	{
		l_obj->Release();
		return NULL;
	}
	return l_obj;
}

inline tERROR MapCommitError(HRESULT l_error)
{
	switch( l_error )
	{
	case E_ACCESSDENIED: return errACCESS_DENIED;
	case E_OUTOFMEMORY: return errNOT_ENOUGH_MEMORY;
	}
	return errOBJECT_WRITE;
}

//////////////////////////////////////////////////////////////////////////
// PROEMailSystem class

PROEMailSystem::PROEMailSystem()
	: PROEObject(MSOE_MAILSYSTEM, NULL), m_token_old(NULL), m_token_new(NULL)
{
}

PROEMailSystem::~PROEMailSystem()
{
	for( int i = 0; i != m_idetities.size(); ++i)
		m_idetities[i]->Release();

	if( m_token_new )
	{
		SetThreadToken(NULL, m_token_old);
		CloseHandle(m_token_new);
	}

	if( m_token_old )
		CloseHandle(m_token_old);

}

tERROR	PROEMailSystem::Init()
{
	g_WTSApi.ImpersonateToUser(m_token_new, m_token_old);

	HKEY user_key = HKEY_CURRENT_USER;

	hTOKEN hToken = NULL;
	tERROR l_error = CALL_SYS_ObjectCreateQuick(g_root, &hToken, IID_TOKEN, PID_ANY, SUBTYPE_ANY);
	if( hToken )
	{
		tCHAR l_user_name[MAX_PATH];
		l_error = CALL_SYS_PropertyGetStr(hToken, NULL, pgOBJECT_NAME, l_user_name, sizeof(l_user_name), cCP_ANSI);
		if( PR_SUCC(l_error) )
			PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "MSOE: Impersonated to user:%s", l_user_name));

		if( PR_SUCC(CALL_SYS_PropertyGetStr(hToken, NULL, pgOBJECT_FULL_NAME, l_user_name, sizeof(l_user_name), cCP_ANSI)) )
			RegOpenKey(HKEY_USERS, l_user_name, &user_key);

		CALL_SYS_ObjectClose(hToken);
	}

	HKEY l_identity_key = NULL;
	char l_identity_name[MAX_PATH], l_buff[MAX_PATH];

    LONG l_ret = RegOpenKey(user_key, "Identities", &l_identity_key);
	if(l_ret != ERROR_SUCCESS)
		return errOBJECT_CANNOT_BE_INITIALIZED;

	l_error = errOK;
	for(DWORD l_pos = 0; l_ret == ERROR_SUCCESS; l_pos++ )
	{
		if( RegEnumKey(l_identity_key, l_pos, l_buff, MAX_PATH) != ERROR_SUCCESS )
			break;

		HKEY l_sub_key = NULL;
		if( (l_ret = RegOpenKey(l_identity_key, l_buff, &l_sub_key)) != ERROR_SUCCESS )
			break;

		DWORD l_size = MAX_PATH;
		l_ret = RegQueryValueEx(l_sub_key, "Username", 0, NULL, (LPBYTE)l_identity_name, &l_size);

		RegCloseKey(l_sub_key);

		if( l_ret != ERROR_SUCCESS )
			break;

		strcat_s(l_buff, countof(l_buff), "\\Software\\Microsoft\\Outlook Express\\5.0");

		HKEY l_outlook_key = NULL;
		if( RegOpenKey(l_identity_key, l_buff, &l_outlook_key) != ERROR_SUCCESS )
			continue;

		l_size = MAX_PATH;
		if( RegQueryValueEx(l_outlook_key, "Store Root", 0, NULL, (LPBYTE)l_buff, &l_size) != ERROR_SUCCESS )
		{
			RegCloseKey(l_outlook_key);
			continue;
		}

		RegCloseKey(l_outlook_key);

		char l_dbx_path[0x400];
		CThreadContext::ExpandEnvironmentStringsEx(l_buff, l_dbx_path, sizeof(l_dbx_path));
		tDWORD l_len = (tDWORD)strlen(l_dbx_path) - 1;
		if( l_len > 0 && l_dbx_path[l_len] != '\\' && l_dbx_path[l_len] != '/' )
			l_dbx_path[++l_len] = '\\', l_dbx_path[++l_len] = '\0';

		m_idetities.push_back(new PROEIdentityDatabase(this, l_pos, l_identity_name, l_dbx_path));
	}

	RegCloseKey(l_identity_key);
	if( user_key != HKEY_CURRENT_USER )
		RegCloseKey(user_key);

	return l_ret == ERROR_SUCCESS ? errOK : errNOT_OK;
}

tERROR	PROEMailSystem::GetNext(PROEObject *&p_obj)
{
	if( !CheckInit() )
		return errOBJECT_NOT_INITIALIZED;

	PROEIdentityDatabase *l_identity = NULL;
	DWORD l_pos = p_obj ? ((PROEIdentityDatabase*)p_obj)->m_id_pos + 1 : 0;
	if( l_pos < m_idetities.size() )
		l_identity = m_idetities[l_pos];
	else
		return errEND_OF_THE_LIST;

	l_identity->AddRef();
	p_obj = l_identity;
	return errOK;
}

//////////////////////////////////////////////////////////////////////////
// PROEVistaMail class

PROEVistaMail::PROEVistaMail(PROEObject* p_parent, hObjPtr hPtr)
	: PROEObject(MSOE_MAILSYSTEM, p_parent),
	m_token_old(NULL), m_token_new(NULL), m_nfio_ptr(hPtr), m_is_folders(cTRUE)
{
}

PROEVistaMail::~PROEVistaMail()
{
	if( m_token_new )
	{
		SetThreadToken(NULL, m_token_old);
		CloseHandle(m_token_new);
	}

	if( m_token_old )
		CloseHandle(m_token_old);

	if( m_nfio_ptr )
		CALL_SYS_ObjectClose(m_nfio_ptr);
}

tERROR	PROEVistaMail::Init()
{
	if( m_nfio_ptr )
		return errOK;

	typedef HRESULT (WINAPI *tSHGetFolderPath)(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPSTR pszPath);
	static tSHGetFolderPath fnSHGetFolderPath = NULL;
	if( fnSHGetFolderPath == (tSHGetFolderPath)-1 )
		return errUNEXPECTED;

	if( !fnSHGetFolderPath )
	{
		HMODULE hShFolder = LoadLibraryA("shfolder.dll");
		fnSHGetFolderPath = hShFolder ? (tSHGetFolderPath)GetProcAddress(hShFolder, "SHGetFolderPathA") : NULL;
	}

	if( !fnSHGetFolderPath )
	{
		fnSHGetFolderPath = (tSHGetFolderPath)-1;
		return errUNEXPECTED;
	}

	g_WTSApi.ImpersonateToUser(m_token_new, m_token_old);

//	RegCloseKey(HKEY_CURRENT_USER);

	char mail_path[MAX_PATH];
	if( FAILED(fnSHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, m_token_new, SHGFP_TYPE_CURRENT, mail_path)) )
		return errOBJECT_CANNOT_BE_INITIALIZED;

	strcat_s(mail_path, countof(mail_path), "\\Microsoft\\Windows Mail");
	PR_TRACE((g_root, prtIMPORTANT, "MSOE: Windows Mail path <%s>", mail_path));

	tERROR l_error = CALL_SYS_ObjectCreate(g_root, &m_nfio_ptr, IID_OBJPTR, PID_NATIVE_FIO, 0);
	if( PR_SUCC(l_error) )
		l_error = CALL_SYS_PropertySetStr(m_nfio_ptr, 0, pgOBJECT_PATH, mail_path, 0, cCP_ANSI );
	if( PR_SUCC(l_error) )
		l_error = CALL_SYS_ObjectCreateDone(m_nfio_ptr);

	if( PR_FAIL(l_error) && m_nfio_ptr )
		CALL_SYS_ObjectClose(m_nfio_ptr), m_nfio_ptr = NULL;

//	RegCloseKey(HKEY_CURRENT_USER);
	return l_error;
}

tERROR PROEVistaMail::ChangeTo(const char* name)
{
	hSTRING str = NULL;
	tERROR err = CALL_SYS_ObjectCreateQuick(g_root, &str, IID_STRING, PID_ANY, 0);
	if( PR_SUCC(err) )
		err = CALL_String_ImportFromBuff(str, NULL, (tPTR)name, 0, cCP_ANSI, cSTRING_Z);
	if( PR_SUCC(err) )
		err = CALL_ObjPtr_ChangeTo(m_nfio_ptr, (hOBJECT)str);
	if( str )
		CALL_SYS_ObjectClose(str);
	return err;
}

tERROR PROEVistaMail::GetCurObj(PROEObject *&p_obj)
{
	tERROR l_error;
	if( CALL_SYS_PropertyGetDWord(m_nfio_ptr, pgIS_FOLDER) )
	{
		hObjPtr objptr = NULL;
		l_error = CALL_ObjPtr_Clone(m_nfio_ptr, &objptr);
		if( PR_SUCC(l_error) )
			l_error = CALL_ObjPtr_StepDown(objptr);

		if( PR_SUCC(l_error) && !m_parent )
		{
			CALL_SYS_PropertySetStr(objptr, 0, pgMASK, "*.oeaccount", 0, cCP_ANSI);

			while( PR_SUCC(l_error = CALL_ObjPtr_Next(objptr)) )
				if( !CALL_SYS_PropertyGetDWord(objptr, pgIS_FOLDER) )
					break;
			CALL_SYS_PropertySetStr(objptr, 0, pgMASK, "*", 0, cCP_ANSI);
			CALL_ObjPtr_Reset(objptr, cFALSE);
		}
		
		if( PR_SUCC(l_error) )
		{
			PROEVistaMail* mail = new PROEVistaMail(this, objptr);
			CALL_SYS_PropertyGetStr(m_nfio_ptr, 0, pgOBJECT_NAME, mail->m_name, sizeof(m_name), cCP_ANSI);
			p_obj = mail;
		}
		else if( objptr )
			CALL_SYS_ObjectClose(objptr);
	}
	else
	{
		hIO io = NULL;
		l_error = CALL_ObjPtr_IOCreate(m_nfio_ptr, &io, NULL, fACCESS_READ, fOMODE_OPEN_IF_EXIST);
		if( PR_SUCC(l_error) )
			p_obj = new PROEIOMessage(io, this);
	}
	return l_error;
}

PROEObject * PROEVistaMail::LoadChild(PRPrmBuff &p_buff)
{
	if( !CheckInit() )
		return NULL;

	PROEObject* obj = NULL;
	tSTRING l_load_name; p_buff >> l_load_name;

	if( PR_SUCC(ChangeTo(l_load_name)) )
		GetCurObj(obj);
	return obj;
}

tERROR	PROEVistaMail::SaveChild(PRPrmBuff &p_buff, PROEObject *p_obj)
{
	tCHAR buff[MAX_PATH] = "";
	tSTRING name = buff;
	if( p_obj->m_type == MSOE_MAILSYSTEM )
		name = ((PROEVistaMail*)p_obj)->m_name;
	else
	{
		CALL_SYS_PropertyGetStr(((PROEIOMessage*)p_obj)->m_io, 0,
			pgOBJECT_NAME, buff, sizeof(buff), cCP_ANSI);
	}
	return p_buff.SetStr(name);
}

tERROR PROEVistaMail::GetName(PRPrmBuff &p_buff)
{
	return p_buff << m_name ? errOK : errBUFFER_TOO_SMALL;
}

tERROR	PROEVistaMail::GetNext(PROEObject *&p_obj)
{
	if( !CheckInit() )
		return errOBJECT_NOT_INITIALIZED;

	tERROR l_error = errOK;
	if( !p_obj )
	{
		m_is_folders = cTRUE;
		CALL_ObjPtr_Reset(m_nfio_ptr, cFALSE);
	}

GetNextLoop:
	while( PR_SUCC(l_error = CALL_ObjPtr_Next(m_nfio_ptr)) )
	{
		tDWORD is_folder = CALL_SYS_PropertyGetDWord(m_nfio_ptr, pgIS_FOLDER);
		if( m_is_folders != is_folder )
			continue;

		tCHAR buff[MAX_PATH];
		if( PR_FAIL(CALL_SYS_PropertyGetStr(m_nfio_ptr, 0, pgOBJECT_NAME, buff, sizeof(buff), cCP_ANSI)) )
		{
			PR_TRACE((g_root, prtERROR, "MSOE Vista: Cannot get object name"));
			continue;
		}
		PR_TRACE((g_root, prtIMPORTANT, "MSOE Vista: Next object name %s", buff));

		if( !is_folder )
		{
			tCHAR* ext = strrchr(buff, '.');
			if( !ext || _stricmp(++ext, "eml") )
				continue;
		}

		if( PR_SUCC(GetCurObj(p_obj)) )
			break;
	}

	if( l_error == errEND_OF_THE_LIST && m_is_folders )
	{
		m_is_folders = cFALSE;
		CALL_ObjPtr_Reset(m_nfio_ptr, cFALSE);
		goto GetNextLoop;
	}

	if( l_error != errEND_OF_THE_LIST )
		PR_TRACE((g_root, prtIMPORTANT, "MSOE Vista: GetNext failed %x", l_error));

	return l_error;
}

tERROR PROEVistaMail::GetProp(tPROPID p_prop_id, PRPrmBuff &p_buff)
{
	if( !m_parent )
		return errPROPERTY_NOT_FOUND;

	switch(p_prop_id)
	{
	case pgIS_MESSAGE_STORAGE:
		return p_buff << (tBOOL)(!!m_parent->m_parent);

	case pgSUBFOLDERS_COUNT:
		return p_buff << (tDWORD)1;

//	case pgCAN_CREATE_SUBFOLDERS:
	}

	return errPROPERTY_NOT_FOUND;
}

//////////////////////////////////////////////////////////////////////////
// PROEDatabaseBase class

PROEDatabaseBase::PROEDatabaseBase(const char *p_dbx_file)
	: m_idb(NULL), m_rowset(NULL), m_attrs(0), m_session(NULL), m_com_inited(cFALSE)
{
	if( p_dbx_file )
		m_dbx_file = p_dbx_file;

}

bool PROEDatabaseBase::CheckPath()
{
	if( m_dbx_file.length() < 2 )
		return false;

	if( (m_dbx_file[0] == '\\' || m_dbx_file[0] == '/') &&
		(m_dbx_file[1] == '\\' || m_dbx_file[1] == '/') )
		return false;

	tCHAR l_drive[4] = {m_dbx_file[0], m_dbx_file[1],'\\' , 0};
	if( GetDriveType(l_drive) == DRIVE_REMOTE )
		return false;

	return true;
}

HRESULT	PROEDatabaseBase::Open(tagTABLESCHEMA *p_schema)
{
	if( !CheckPath() )
		return E_FAIL;

	if( !m_com_inited )
		m_com_inited = SUCCEEDED(CoInitialize(NULL));
	HRESULT l_result = CoCreateInstance(CLSID_DatabaseSession, NULL,
			CLSCTX_SERVER, IID_IDatabaseSession,  (void**)&m_session);

	if( !FAILED(l_result) )
	{
		m_attrs = GetFileAttributes(m_dbx_file.c_str());
		if( m_attrs & FILE_ATTRIBUTE_READONLY )
			if( !SetFileAttributes(m_dbx_file.c_str(), m_attrs & ~FILE_ATTRIBUTE_READONLY) )
				l_result = E_FAIL;
	}

	if( !FAILED(l_result) )
	{
		CThreadContext::SetThreadContext(true);
		unsigned long exc_code = 0;
		__try
		{
			l_result = m_session->OpenDatabase(m_dbx_file.c_str(), 5, p_schema, &m_db_ext, &m_idb);
			// на  Vista запись все-таки не поддерживается, так что даже пытаться не будем...
/*
			if(l_result == E_NOTIMPL)
			{
				// OpenDatabase returns E_NOTIMPL under Vista
				size_t lenw = (m_dbx_file.size() + 1) * sizeof(wchar_t);
				unsigned short *strw = (unsigned short *)alloca(lenw);
				if(MultiByteToWideChar(CP_ACP, 0, m_dbx_file.c_str(), -1, (LPWSTR)strw, lenw))
					l_result = m_session->OpenDatabaseW(strw, 5, p_schema, &m_db_ext, &m_idb);
			}
*/
		}
		__except(exc_code = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER)
		{
			PR_TRACE((g_root, prtERROR, "MSOE: OpenDatabase raises exception 0x%x", exc_code));
			l_result = E_FAIL;
		}
		CThreadContext::OpenBaseDone();
	}

    if( !FAILED(l_result) )
		l_result = m_idb->CreateRowset(0, 0, &m_rowset);
	else
		PR_TRACE((g_root, prtERROR, "MSOE: OpenDatabase failed %x", l_result));

	return l_result;
}

void PROEDatabaseBase::Close()
{
	if( m_attrs & FILE_ATTRIBUTE_READONLY )
		SetFileAttributes(m_dbx_file.c_str(), m_attrs);

	try
	{
		if( m_rowset )
			m_idb->CloseRowset(&m_rowset);

		if( m_idb )
			m_idb->Release();
	}
	catch(...){}

	if( m_session )
	{
		m_session->Release();
		CThreadContext::SetThreadContext(false);
	}

	m_attrs = 0;
	m_rowset = NULL;
	m_idb = NULL;
	m_session = NULL;

	if(m_com_inited)
	{
		CoUninitialize();
		m_com_inited = cFALSE;
	}
}

//////////////////////////////////////////////////////////////////////////
// PROEDatabase class

PROEDatabase::PROEDatabase(const char *p_dbx_file, tagFOLDERINFO *p_fldinfo)
	: PROEObject(MSOE_DATABASE), PROEDatabaseBase(p_dbx_file),
		m_pos(0), m_msgcount(0), m_io(NULL), m_tmp_nfio(NULL), m_nfio(NULL)
{
	if( p_fldinfo )
	{
		if( p_fldinfo->szName )
			m_name = p_fldinfo->szName;
		m_record_id = p_fldinfo->dwRecordID;
		m_parent_id = p_fldinfo->dwParentID;
		m_type = (DWORD)(p_fldinfo->u_Type); 
	}
}

PROEDatabase::PROEDatabase(PROEObject *p_parent)
	: PROEObject(MSOE_DATABASE, p_parent), m_pos(0), m_msgcount(0),
		m_record_id(-1), m_io(NULL), m_tmp_nfio(NULL), m_nfio(NULL), m_type(0)
{
	p_parent->Release();
}

PROEDatabase::PROEDatabase(hIO p_io)
	: PROEObject(MSOE_DATABASE, NULL), m_pos(0), m_msgcount(0), m_io(p_io), m_nfio(NULL), m_type(0)
{
	m_tmp_nfio = CreateNativeIO(p_io);
	if( m_tmp_nfio )
	{
		tCHAR l_dbx_file[MAX_PATH];
		CALL_SYS_PropertyGetStr(m_tmp_nfio, NULL, pgOBJECT_FULL_NAME, l_dbx_file, MAX_PATH, cCP_ANSI);
		m_dbx_file = l_dbx_file;
	}
}

PROEDatabase::~PROEDatabase()
{
	for( int i = 0; i != m_childs.size(); ++i)
	{
		PROEDatabase *l_db = m_childs[i];
		l_db->SaveChanges();
		l_db->Release();
	}

	PROEDatabaseBase::Close();

	if( m_nfio )
		CALL_SYS_ObjectClose(m_nfio);

	if( m_tmp_nfio && m_tmp_nfio != m_io )
	{
		CALL_SYS_ObjectClose(m_tmp_nfio);
		DeleteFile(m_dbx_file.c_str());
	}

	m_parent = NULL;
}

void PROEDatabase::SetParent(PROEDatabase *p_parent)
{
	m_parent = p_parent;
	if( p_parent )
	{
		m_pos = (DWORD)p_parent->m_childs.size();
		p_parent->m_childs.push_back(this);
	}
}

tERROR	PROEDatabase::Init()
{
	if( !m_dbx_file.length() )
		return errOK;

	HRESULT l_result = PROEDatabaseBase::Open(&g_MessageTableSchema);
	if( !FAILED(l_result) )
		l_result = m_idb->GetRecordCount(0, &m_msgcount);

	if( FAILED(l_result) )
		return errOBJECT_CANNOT_BE_INITIALIZED;

	return errOK;
}

tERROR	PROEDatabase::Commit()
{
	if( !m_idb )
		return warnNOTHING_TO_COMMIT;

	bool f_copy_to_io = m_io && m_tmp_nfio != m_io;
	tERROR l_error = errOK;
	tDWORD l_old_mode = 0;

    HLOCK__ *l_lock = NULL;
    m_idb->Lock(&l_lock);

	if( m_nfio )
		CALL_SYS_PropertySetDWord(m_nfio, pgOBJECT_ACCESS_MODE, fACCESS_NONE);

	if( m_tmp_nfio )
	{
		l_old_mode = CALL_SYS_PropertyGetDWord(m_tmp_nfio, pgOBJECT_ACCESS_MODE);
		l_error = CALL_SYS_PropertySetDWord(m_tmp_nfio, pgOBJECT_ACCESS_MODE, fACCESS_NONE);
	}

	if( PR_SUCC(l_error) )
	{
		ULONG client_count = 0;
		m_idb->GetClientCount(&client_count);

//		if( client_count == 1 )
//			try { m_idb->Compact(NULL, 0); } catch(...) {}

		if( l_old_mode )
			l_error = CALL_SYS_PropertySetDWord(m_tmp_nfio, pgOBJECT_ACCESS_MODE, l_old_mode);
	}
	else
		l_error = errOK;

	if( l_lock )
		m_idb->Unlock(&l_lock);

	if( PR_SUCC(l_error) && f_copy_to_io )
	{
		PROEDatabaseBase::Close();

		l_error = CALL_SYS_PropertySetDWord(m_io, pgOBJECT_ACCESS_MODE, fACCESS_RW);
		if( PR_SUCC(l_error) )
			l_error = CopyIO(m_io, m_tmp_nfio);
	}

	if( m_nfio )
		CALL_SYS_PropertySetDWord(m_nfio, pgOBJECT_ACCESS_MODE, fACCESS_READ);

	return PR_SUCC(l_error) ? warnPARENT_NOT_COMMITED : l_error;
}

PROEObject * PROEDatabase::LoadChild(PRPrmBuff &p_buff)
{
	if( !CheckInit() )
		return NULL;

	tDWORD l_type; p_buff >> l_type;
	tDWORD l_record_id; p_buff >> l_record_id;

	PROEObject *l_obj = NULL;
	if( l_type == MSOE_DATABASE )
	{
		for(unsigned int i = 0; i < m_childs.size(); i++)
			if( m_childs[i]->m_record_id == l_record_id )
			{
				l_obj = m_childs[i];
				l_obj->AddRef();
				break;
			}
	}
	else
	{
		tagMESSAGEINFO l_msg_info;
		memset(&l_msg_info, 0, sizeof(tagMESSAGEINFO));
		l_msg_info.dwRecordID = l_record_id;

		if( !FAILED(m_idb->FindRecord(0, -1, &l_msg_info, NULL)) )
			l_obj = new PROEDBMessage(this, -1, &l_msg_info);
	}
	return l_obj;
}

tERROR	PROEDatabase::SaveChild(PRPrmBuff &p_buff, PROEObject *p_obj)
{
	p_buff << p_obj->m_type;
	if( p_obj->m_type == MSOE_DATABASE )
		p_buff << ((PROEDatabase*)p_obj)->m_record_id;
	else
		p_buff << (tDWORD)((PROEDBMessage*)p_obj)->m_record_id;
	return p_buff.Error();
}

tERROR PROEDatabase::GetProp(tPROPID p_prop_id, PRPrmBuff &p_buff)
{
	if( m_dbx_file.empty() )
		return errPROPERTY_NOT_FOUND;

	switch(p_prop_id)
	{
	case pgIS_MESSAGE_STORAGE:
	case pgCAN_CREATE_SUBFOLDERS:
		return p_buff << (tBOOL)cTRUE;

	case pgSUBFOLDERS_COUNT:
		return p_buff << m_childs.size();

	case pgFOLDER_TYPE:
		return p_buff << (tDWORD)m_type;
	}

	return errPROPERTY_NOT_FOUND;
}

tERROR PROEDatabase::GetName(PRPrmBuff &p_buff)
{
	return p_buff << m_name.c_str() ? errOK : errBUFFER_TOO_SMALL;
}

tERROR PROEDatabase::GetNext(PROEObject *&p_obj)
{
	if( !CheckInit() )
		return errOBJECT_NOT_INITIALIZED;

	PROEDatabase *l_db = NULL;
	if( !p_obj )
	{
		if( m_childs.size() )
			l_db = m_childs[0];
		else
			return GetNextMessage(p_obj);
	}
	else if( p_obj->m_type == MSOE_DATABASE )
	{
		DWORD l_pos = ((PROEDatabase*)p_obj)->m_pos + 1;
		if( l_pos < m_childs.size() )
			l_db = m_childs[l_pos];
		else
		{
			p_obj = NULL;
			return GetNextMessage(p_obj);
		}
	}
	else if( p_obj->m_type == MSOE_MESSAGE )
		return GetNextMessage(p_obj);

	l_db->AddRef();
	p_obj = l_db;
	return errOK;
}

tERROR PROEDatabase::GetNextMessage(PROEObject *&p_obj)
{
	if( !CheckInit() )
		return errOBJECT_NOT_INITIALIZED;

	if( !m_idb )
		return errEND_OF_THE_LIST;

	PROEDBMessage *l_msg = (PROEDBMessage*)p_obj;

	DWORD l_pos = l_msg ? l_msg->m_pos + 1 : 0;
	if( l_pos >= m_msgcount )
		return errEND_OF_THE_LIST;

	DWORD l_seek = 0, l_query_rows = 0;
    tagMESSAGEINFO l_msg_info;

	if( FAILED(m_idb->SeekRowset(m_rowset, SEEK_BEGINNING, l_pos, &l_seek)) )
		return errOBJECT_BAD_INTERNAL_STATE;

	if( FAILED(m_idb->QueryRowset(m_rowset, 1, (void**)&l_msg_info, &l_query_rows)) || !l_query_rows )
		return errOBJECT_BAD_INTERNAL_STATE;

	p_obj = new PROEDBMessage(this, l_pos, &l_msg_info);

	m_idb->FreeRecord(&l_msg_info);
	return errOK;
}

tERROR	PROEDatabase::GetSize(tQWORD* p_result, IO_SIZE_TYPE p_type)
{
	if( !CheckInit() )
		return errOBJECT_NOT_INITIALIZED;

	if( !m_idb || p_type == IO_SIZE_TYPE_EXPLICIT )
		return PROEObject::GetSize(p_result, p_type);

	ULONG l_size1, l_size2, l_size3, l_size4;
	if( FAILED(m_idb->GetSize(&l_size1, &l_size2, &l_size3, &l_size4)) )
		return errOBJECT_INVALID;
	*p_result = l_size4;
	return errOK;
}

hOBJECT	PROEDatabase::GetNativeIO()
{
	if( !CheckInit() )
		return NULL;

	if( m_nfio )
		return (hOBJECT)m_nfio;

	if( m_io || !m_idb )
		return NULL;

	tERROR l_error = CALL_SYS_ObjectCreate(g_root, &m_nfio, IID_IO, PID_NATIVE_FIO, SUBTYPE_ANY);

	if( PR_SUCC(l_error) )
		l_error = CALL_SYS_PropertySetDWord(m_nfio, pgOBJECT_OPEN_MODE, fOMODE_OPEN_IF_EXIST);
	if( PR_SUCC(l_error) )
		l_error = CALL_SYS_PropertySetDWord(m_nfio, pgOBJECT_ACCESS_MODE, fACCESS_READ);
	if( PR_SUCC(l_error) )
		l_error = CALL_SYS_PropertySetStr(m_nfio, 0, pgOBJECT_NAME, (tPTR)m_dbx_file.c_str(), 0, cCP_ANSI );

	if( PR_SUCC(l_error) )
		l_error = CALL_SYS_ObjectCreateDone(m_nfio);

	if( PR_FAIL(l_error) && m_nfio )
	{
		CALL_SYS_ObjectClose(m_nfio);
		m_nfio = NULL;
	}
	return (hOBJECT)m_nfio;
}

//////////////////////////////////////////////////////////////////////////
// PROERootDatabase class

PROEIdentityDatabase::PROEIdentityDatabase(PROEMailSystem *p_parent, tDWORD p_id_pos, const char *p_identity, const char *p_dbx_path)
	: PROEDatabase(p_parent), m_id_pos(p_id_pos)
{
	if( p_identity )
		m_identity = p_identity;
	if( p_dbx_path )
		m_dbx_path = p_dbx_path;
}

PROEIdentityDatabase::~PROEIdentityDatabase()
{
}

tERROR PROEIdentityDatabase::Init()
{
	if( !m_dbx_path.length() )
		return errOBJECT_CANNOT_BE_INITIALIZED;

	std::string &l_folders_dbx = m_dbx_path + "Folders.dbx";

	::PROEDatabaseBase l_folders_db(l_folders_dbx.c_str());
	HRESULT l_result = l_folders_db.Open(&g_FoldersTableSchema);

	if( FAILED(l_result) )
		return errOBJECT_CANNOT_BE_INITIALIZED;

	std::vector<PROEDatabase*> l_db_map;

    tagFOLDERINFO l_fldinfo[QUERY_SIZE];
	DWORD l_rows_queried = QUERY_SIZE;
	while( !FAILED(l_result) && l_rows_queried == QUERY_SIZE )
	{
		l_result = l_folders_db.m_idb->QueryRowset(l_folders_db.m_rowset, QUERY_SIZE, (void**)&l_fldinfo, &l_rows_queried);
		if( FAILED(l_result) )
			break;

		for(DWORD i = 0; i < l_rows_queried; i++)
		{
			if( l_fldinfo[i].dwParentID == -1 )
			{
				m_record_id = l_fldinfo[i].dwRecordID;
				continue;
			}

			std::string l_dbx_path;
			if( l_fldinfo[i].szDBFile )
				l_dbx_path = m_dbx_path + l_fldinfo[i].szDBFile;

			PROEDatabase *l_db = new PROEDatabase(l_dbx_path.c_str(), &l_fldinfo[i]);
			l_db_map.push_back(l_db);
			l_folders_db.m_idb->FreeRecord(&l_fldinfo[i]);
		}
	}

	if( m_record_id == -1 )
		return errOBJECT_CANNOT_BE_INITIALIZED;

	size_t size = l_db_map.size();
	for(unsigned int i = 0; i < size; i++)
	{
		PROEDatabase *l_db = l_db_map[i];
		PROEDatabase *l_parent = NULL;

		tDWORD parent_id = l_db->m_parent_id;
		if( parent_id == m_record_id )
			l_parent = this;
		else
		{
			for(unsigned int j = 0; j < size; j++)
				if( parent_id == l_db_map[j]->m_record_id )
				{
					l_parent = l_db_map[j];
					break;
				}
		}
		l_db->SetParent(l_parent);
	}

	return PROEDatabase::Init();
}

tERROR	PROEIdentityDatabase::GetName(PRPrmBuff &p_buff)
{
	return p_buff << m_identity.c_str() ? errOK : errBUFFER_TOO_SMALL;
}

//////////////////////////////////////////////////////////////////////////
// PROEMessage class

PROEMessage::PROEMessage(PROEObject *p_parent)
	: PROEObject(MSOE_MESSAGE, p_parent, cFALSE), m_imsg(NULL), m_bodies(NULL), m_com_inited(cFALSE)
{
}

PROEMessage::~PROEMessage()
{
	if( m_bodies )
		CoTaskMemFree(m_bodies);
	if( m_imsg )
		m_imsg->Release();
	if( m_com_inited )
		CoUninitialize();
}

tERROR PROEMessage::Init()
{
	if( !m_com_inited )
		m_com_inited = SUCCEEDED(CoInitialize(NULL));
	HRESULT l_result = CoCreateInstance(CLSID_IMimeMessage, NULL,
		CLSCTX_SERVER, IID_IMimeMessage, (void**)&m_imsg);

	if( !FAILED(l_result) )
		l_result = m_imsg->InitNew();

	IStream *l_stream = GetStream();
	if( !l_stream )
		return errOBJECT_CANNOT_BE_INITIALIZED;

	if( !FAILED(l_result) )
	{
		PRVarProp prop(VT_BOOL);
		prop.boolVal = VARIANT_FALSE;
		m_imsg->SetOption(OID_HIDE_TNEF_ATTACHMENTS, prop);
	}

	if( !FAILED(l_result) )
		l_result = m_imsg->Load(l_stream);

	l_stream->Release();

	if( !FAILED(l_result) )
		m_imsg->GetAttachments(&m_partnum, &m_bodies);

	if( FAILED(l_result) )
		return errOBJECT_CANNOT_BE_INITIALIZED;

	return errOK;
}

tERROR	PROEMessage::SaveChild(PRPrmBuff &p_buff, PROEObject *p_obj)
{
	PROEMessagePart *l_part = (PROEMessagePart*)p_obj;

	l_part->GetNameZ(p_buff);
//	p_buff << (tDWORD)l_part->m_pos;
	return p_buff.Error();
}

PROEObject * PROEMessage::LoadChild(PRPrmBuff &p_buff)
{
	tSTRING l_load_name; p_buff >> l_load_name;
//	tDWORD	l_load_pos;  p_buff >> l_load_pos;

	tCHAR l_name[MAX_PATH];
	tDWORD l_part_pos = -1;

	for(unsigned int i = 0; i < m_partnum; i++)
	{
		if( !m_bodies[i] )
			continue;

		if( PR_FAIL(GetBodyName(m_bodies[i], l_name, sizeof(l_name))) )
			continue;

		if( strcmp(l_name, l_load_name) )
			continue;
		
		l_part_pos = i;
//		if( l_load_pos == l_part_pos )
			break;
	}

	if( l_part_pos == -1 )
		return NULL;

	return new PROEMessagePart(this, m_bodies[l_part_pos], l_part_pos);
}

tERROR PROEMessage::GetName(PRPrmBuff &p_buff)
{
	if( !CheckInit() )
		return errOBJECT_NOT_INITIALIZED;

	PRVarProp l_time(VT_FILETIME);
	PRVarProp l_desc(VT_LPWSTR);
	PRVarProp l_subject(VT_LPWSTR);

	bool f_time = true;
	if( !FAILED(GET_PROP(l_time, ATT_RECVTIME)) )
	{
		p_buff << "[From:";
		if( !FAILED(GET_PROP(l_desc, HDR_FROM)) )
			p_buff << l_desc;
	}
	else if( !FAILED(GET_PROP(l_time, ATT_SENTTIME)) )
	{
		p_buff << "[To:";
		if( !FAILED(GET_PROP(l_desc, HDR_TO)) )
			p_buff << l_desc;
	}
	else
	{
		p_buff << "[From:None";
		f_time = false;
	}

	p_buff << "][Subject:";
	if( !FAILED(GET_PROP(l_subject, HDR_SUBJECT)) )
		p_buff << l_subject;

	if( f_time )
	{
		p_buff << "][Time:";
		p_buff << (FILETIME*)&l_time.date;
	}
	p_buff << ']';
	return p_buff.Error();
}

tERROR PROEMessage::GetBodyName(HBODY__ *m_body, tSTRING p_buff, tSIZE_T p_buff_sz)
{
	PRVarProp l_filename(VT_LPWSTR);
	PRVarProp l_cnttype(VT_LPWSTR);

	if( !FAILED(GET_BODY_PROP(l_filename, ATT_FILENAME)) )
		strcpy_s(p_buff, p_buff_sz, l_filename);
	else if( !FAILED(GET_BODY_PROP(l_cnttype, HDR_CNTTYPE)) )
		strcpy_s(p_buff, p_buff_sz, l_cnttype);
	else
		return errPROPERTY_NOT_FOUND;
	return errOK;
}

tERROR PROEMessage::GetHeaders(PRPrmBuff &p_buff)
{
	IStream *l_stream;
	if( FAILED(m_imsg->GetMessageSource(&l_stream, 0)) )
		return errUNEXPECTED;

	tERROR l_error = errOK;

	if(m_partnum)
	{
		tagBODYOFFSETS l_offsets;
		for(unsigned int i = 0; i < m_partnum && l_error == errOK; i++)
		{
			m_imsg->GetBodyOffsets(m_bodies[i], &l_offsets);

			tDWORD l_offs_from = i ? l_offsets.dwMsgOffset : 0;
			LLONGDEF(l_offs, l_offs_from);
			if( FAILED(l_stream->Seek(l_offs, STREAM_SEEK_SET, NULL)) )
			{
				l_error = errUNEXPECTED;
				break;
			}

			tDWORD l_size = l_offsets.dwContentOffset - l_offs_from;
			tCHAR *l_buff = new tCHAR[l_size + 1];

			if( FAILED(l_stream->Read(l_buff, l_size, NULL)) )
				l_error = errUNEXPECTED;
			else
			{
				l_buff[l_size] = 0;
				p_buff << l_buff;
			}
			delete[] l_buff;
		}
	}
	else
	{
		STATSTG l_ss;
		if(FAILED(l_stream->Stat(&l_ss, STATFLAG_NONAME)))
			l_error = errUNEXPECTED;
		else
		{
			tDWORD l_size = (tDWORD)l_ss.cbSize.QuadPart;
			tCHAR *l_buff = new tCHAR[l_size + 1];

			if( FAILED(l_stream->Read(l_buff, l_size, NULL)) )
				l_error = errUNEXPECTED;
			else
			{
				l_buff[l_size] = 0;
				p_buff << l_buff;
			}
			delete[] l_buff;
		}
	}
	l_stream->Release();
	if( PR_FAIL(l_error) )
		return l_error;

	p_buff.StrEnd();
	return p_buff.Error();
}

tERROR PROEMessage::GetBodyHash(HBODY__ *p_body, tQWORD *p_id_hash)
{
	hHASH  l_hash_obj = NULL;
	tBYTE  l_hash_data[32];
	tERROR l_error = errUNEXPECTED;

	if( !g_root )
		return l_error;

	IStream *l_stream = NULL;
	if( FAILED(m_imsg->GetMessageSource(&l_stream, 0)) )
		return l_error;

	tagBODYOFFSETS l_offsets;
	HRESULT l_result = m_imsg->GetBodyOffsets(p_body, &l_offsets);

	tDWORD l_body_size;
	if( !FAILED(l_result) )
	{
		l_body_size = l_offsets.dwEndOffset - l_offsets.dwBodyOffset;

		LLONGDEF(l_offs, l_offsets.dwBodyOffset);
		l_result = l_stream->Seek(l_offs, STREAM_SEEK_SET, NULL);
	}

	if( !FAILED(l_result) )
		l_error = CALL_SYS_ObjectCreateQuick(g_root, &l_hash_obj, IID_HASH, PID_HASH_MD5, SUBTYPE_ANY);

	tBYTE l_buff[READ_CHUNCK_SIZE];
	tDWORD l_out_size, l_read_size = 0;

	while( PR_SUCC(l_error) && l_read_size < l_body_size )
	{
		tDWORD l_size = l_body_size - l_read_size;
		if( l_size > sizeof(l_buff) )
			l_size = sizeof(l_buff);

		if( !FAILED(l_stream->Read(l_buff, l_size, (ULONG*)&l_out_size)) )
		{
			l_error = CALL_Hash_Update(l_hash_obj, l_buff, l_out_size);
			l_read_size += l_out_size;
		}
		else
			l_error = errUNEXPECTED;
	}

	if( PR_SUCC(l_error) )
		l_error = CALL_Hash_GetHash(l_hash_obj, l_hash_data, sizeof(l_hash_data));

	if( l_hash_obj )
		CALL_SYS_ObjectClose(l_hash_obj);

	if( l_stream )
		l_stream->Release();

	if( PR_FAIL(l_error) )
		return l_error;

	*(p_id_hash) = *(tQWORD*)(&l_hash_data[0]);
	*(p_id_hash)^= *(tQWORD*)(&l_hash_data[8]);
	return errOK;
}

tERROR PROEMessage::GetBodyID(HBODY__ *p_body, tCHAR *p_id_buff)
{
	IStream *l_stream;
	if( FAILED(m_imsg->GetMessageSource(&l_stream, 0)) )
		return errUNEXPECTED;

	tagBODYOFFSETS l_offsets;
	HRESULT l_result = m_imsg->GetBodyOffsets(p_body, &l_offsets);

	if( !FAILED(l_result) )
	{
		LLONGDEF(l_offs, l_offsets.dwMsgOffset);
		l_result = l_stream->Seek(l_offs, STREAM_SEEK_SET, NULL);

		tDWORD l_size = l_offsets.dwBodyOffset - l_offsets.dwMsgOffset;
		if( !FAILED(l_result) )
			l_result = l_stream->Read(p_id_buff, l_size, NULL);
		if( !FAILED(l_result) )
			p_id_buff[l_size] = 0;
	}

	l_stream->Release();
	return FAILED(l_result) ? errNOT_OK : errOK;
}

tERROR PROEMessage::DeleteAllBodies()
{
	for(int i = m_partnum-1; i >= 0; i--)
	{
		if( !m_bodies[i] )
			continue;

		if( m_imsg->IsBodyType(m_bodies[i], MSGBODY_ATTACH) == S_OK )
			continue;

		if( FAILED(m_imsg->DeleteBody(m_bodies[i], 0)) )
			return errOBJECT_INVALID;

		m_bodies[i] = NULL;
	}
	return errOK;
}

tERROR PROEMessage::GetProp(tPROPID p_prop_id, PRPrmBuff &p_buff)
{
	if( !CheckInit() )
		return errOBJECT_NOT_INITIALIZED;

	if( p_prop_id == pgOBJECT_NAME )
		return GetName(p_buff);

	enum _MESSAGEPROPERTY l_hdr_prop;
	switch( p_prop_id )
	{
	case pgMESSAGE_FROM:
	case pgMESSAGE_ORIGINAL_FROM:
	case pgMESSAGE_DISPLAY_FROM:
	case pgMESSAGE_ORIGINAL_DISPLAY_FROM:
		l_hdr_prop = HDR_FROM;
		break;
	case pgMESSAGE_TO:
	case pgMESSAGE_DISPLAY_TO:
		l_hdr_prop = HDR_TO;
		break;
	case pgMESSAGE_CC:
	case pgMESSAGE_DISPLAY_CC:
		l_hdr_prop = HDR_CC;
		break;
	case pgMESSAGE_SUBJECT:
		l_hdr_prop = HDR_SUBJECT;
		break;
	case pgMESSAGE_DATE:
		l_hdr_prop = ATT_SENTTIME;
		break;
	case pgMESSAGE_MAILID:
		l_hdr_prop = HDR_MESSAGEID;
		break;
	case pgMESSAGE_MIME_VERSION:
		l_hdr_prop = HDR_MIMEVER;
		break;
	case pgMESSAGE_RETURN_PATH:
		l_hdr_prop = HDR_RETURNPATH;
		break;
	case pgMESSAGE_INTERNET_HEADERS:
		return GetHeaders(p_buff);

	case pgMESSAGE_X_PROP_NAME:
		return p_buff.SetStr(m_x_prop.c_str());
	case pgMESSAGE_X_PROP_VALUE:
		l_hdr_prop = (_MESSAGEPROPERTY)(int)m_x_prop.c_str();
		break;

	default:
		return errPROPERTY_NOT_FOUND;
	}

	PRVarProp l_prop_val(VT_LPWSTR);
	if( FAILED(GET_PROP(l_prop_val, l_hdr_prop)) )
		return errPROPERTY_NOT_FOUND;

	return p_buff.SetStr(l_prop_val);
}

tERROR PROEMessage::SetProp(tPROPID p_prop_id, PRPrmBuff &p_buff)
{
	if( !CheckInit() )
		return errOBJECT_NOT_INITIALIZED;

	enum _MESSAGEPROPERTY l_hdr_prop;
	switch( p_prop_id )
	{
	case pgMESSAGE_FROM:
	case pgMESSAGE_ORIGINAL_FROM:
		l_hdr_prop = HDR_FROM;
		break;
	case pgMESSAGE_TO:
		l_hdr_prop = HDR_TO;
		break;
	case pgMESSAGE_CC:
		l_hdr_prop = HDR_CC;
		break;
	case pgMESSAGE_SUBJECT:
		l_hdr_prop = HDR_SUBJECT;
		break;
	case pgMESSAGE_DATE:
		l_hdr_prop = ATT_SENTTIME;
		break;
	case pgMESSAGE_RETURN_PATH:
		l_hdr_prop = HDR_RETURNPATH;
		break;

	case pgMESSAGE_X_PROP_NAME:
		m_x_prop = p_buff.m_buff;
		return warnPROPERTY_NOT_COMMITED;
	case pgMESSAGE_X_PROP_VALUE:
		l_hdr_prop = (_MESSAGEPROPERTY)(int)m_x_prop.c_str();
		break;

	default:
		return errPROPERTY_NOT_FOUND;
	}

	PRBuffProp l_prop_val(VT_LPSTR, p_buff);
	if( FAILED(SET_PROP(l_prop_val, l_hdr_prop)) )
		return errUNEXPECTED;

	return errOK;
}

tERROR PROEMessage::GetNext(PROEObject *&p_obj)
{
	if( !CheckInit() )
		return errOBJECT_NOT_INITIALIZED;

	if( !m_imsg )
		return errOBJECT_INVALID;

	PROEMessagePart *l_part = (PROEMessagePart*)p_obj;

	for( DWORD l_pos = l_part ? l_part->m_pos + 1 : 0; l_pos < m_partnum; l_pos++ )
		if( m_bodies[l_pos] )
		{
			p_obj = new PROEMessagePart(this, m_bodies[l_pos], l_pos);
			return errOK;
		}
	return errEND_OF_THE_LIST;
}

tERROR	PROEMessage::GetSize(tQWORD* p_result, IO_SIZE_TYPE p_type)
{
	if( !CheckInit() )
		return errOBJECT_NOT_INITIALIZED;

	if( p_type == IO_SIZE_TYPE_EXPLICIT )
		return PROEObject::GetSize(p_result, p_type);

	ULONG l_size = 0;
	if( FAILED(m_imsg->GetMessageSize(&l_size, 0)) )
		return errOBJECT_INVALID;
	*p_result = l_size;
	return errOK;
}

PROEObject * PROEMessage::CreateChild(tDWORD p_create_mode, tCHAR *p_obj_name)
{
	if( !CheckInit() )
		return NULL;

	PROEObject *l_obj = NULL;

	IStream *l_stream = NULL;
	if( FAILED(CreateStreamOnHGlobal(NULL, TRUE, &l_stream)) )
		return l_obj;

	HBODY__ *m_body = NULL;
	HRESULT l_result = m_imsg->AttachFile(p_obj_name, l_stream, &m_body);

	if( !FAILED(l_result) )
	{
		DWORD l_pos = m_partnum++;
		m_bodies = (HBODY__ **)CoTaskMemRealloc(m_bodies, m_partnum * sizeof(DWORD));
		m_bodies[l_pos] = m_body;

		l_obj = new PROEMessagePart(this, m_body, l_pos);
	}

	l_stream->Release();
	return l_obj;
}

//////////////////////////////////////////////////////////////////////////
// PROEDBMessage

PROEDBMessage::PROEDBMessage(PROEDatabase *p_db, DWORD p_pos, tagMESSAGEINFO *p_info)
	: PROEMessage(p_db), m_pos(p_pos)
{
	m_idb = p_db->m_idb;
	m_offset = p_info->dwStreamOffset;
	m_record_id = p_info->dwRecordID;
}

IStream * PROEDBMessage::GetStream()
{
	IStream *l_stream = NULL;
	if( FAILED(m_idb->OpenStream(ACCESS_READ, m_offset, &l_stream)) )
		return NULL;
	return l_stream;
}

tERROR	PROEDBMessage::GetProp(tPROPID p_prop_id, PRPrmBuff &p_buff)
{
	if( p_prop_id != pgOBJECT_NATIVE_IO )
		return PROEMessage::GetProp(p_prop_id, p_buff);

	hOBJECT hNativeIO = static_cast<PROEDatabase*>(m_parent)->GetNativeIO();
	if( !hNativeIO )
		return errPROPERTY_NOT_FOUND;

	return p_buff << (tDWORD)hNativeIO;
}

bool PROEDBMessage::IsAtachments()
{
	for(DWORD i = 0; i < m_partnum; i++)
		if( m_bodies[i] && m_imsg->IsBodyType(m_bodies[i], MSGBODY_ATTACH) == S_OK )
			return true;
	return false;
}

tERROR	PROEDBMessage::Commit()
{
    HLOCK__* l_lock = NULL;
    HRESULT l_result = m_idb->Lock(&l_lock);

	tagMESSAGEINFO l_msg_info;
	memset(&l_msg_info, 0, sizeof(tagMESSAGEINFO));
	l_msg_info.dwRecordID = m_record_id;

	if( !FAILED(l_result) )
		l_result = m_idb->FindRecord(0, -1, &l_msg_info, NULL);

	if( !FAILED(l_result) )
	{
		DWORD l_offset = 0;
		l_result = m_idb->CreateStream(&l_offset);

		IStream *l_stream = NULL;
		if( !FAILED(l_result) )
			l_result = m_idb->OpenStream(ACCESS_WRITE, l_offset, &l_stream);

		if( !FAILED(l_result) )
			l_result = m_imsg->Save(l_stream, 0);

		if( !FAILED(l_result) )
			l_result = l_stream->Commit(0);

		if( !FAILED(l_result) )
			l_result = m_idb->ChangeStreamLock(l_stream, ACCESS_READ);

		if( l_stream )
			l_stream->Release();

		if( !FAILED(l_result) )
		{
			PRVarProp l_subject(VT_LPWSTR);
			if( !FAILED(GET_PROP(l_subject, HDR_SUBJECT)) )
			{
				l_msg_info.szSubject1 = (char*)(const char*)l_subject;
				l_msg_info.szSubject2 = (char*)(const char*)l_subject;
			}

			if( !IsAtachments() )
				l_msg_info.dwFlags &= ~0x4000;

			l_msg_info.dwStreamOffset = m_offset = l_offset;
			l_result = m_idb->UpdateRecord(&l_msg_info);
		}
		else
		{
			if( l_offset )
				m_idb->DeleteStream(l_offset);
		}

		m_idb->FreeRecord(&l_msg_info);
	}

	if( l_lock )
		m_idb->Unlock(&l_lock);

	if( FAILED(l_result) )
		return MapCommitError(l_result);

	return errOK;
}

tERROR	PROEDBMessage::Delete()
{
    HLOCK__* l_lock = NULL;
    HRESULT l_result = m_idb->Lock(&l_lock);

	tagMESSAGEINFO l_msg_info;
	memset(&l_msg_info, 0, sizeof(tagMESSAGEINFO));
	l_msg_info.dwRecordID = m_record_id;

	if( !FAILED(l_result) )
		l_result = m_idb->FindRecord(0, -1, &l_msg_info, NULL);

	if( !FAILED(l_result) )
		l_result = m_idb->DeleteStream(m_offset);

	if( !FAILED(l_result) )
		l_result = m_idb->DeleteRecord(&l_msg_info);

	if( !FAILED(l_result) )
	{
		m_offset = 0;
		m_record_id = -1;
		m_pos--;
	}

	if( l_lock )
		m_idb->Unlock(&l_lock);

	return FAILED(l_result) ? errUNEXPECTED : errOK;
}

//////////////////////////////////////////////////////////////////////////
// PROEIOMessage

PROEIOMessage::~PROEIOMessage()
{
}

tERROR	PROEIOMessage::SetAccessMode(tDWORD p_access_mode)
{
	return CALL_SYS_PropertySetDWord(m_io, pgOBJECT_ACCESS_MODE, p_access_mode);
}

IStream * PROEIOMessage::GetStream()
{
	if( !m_stream )
		m_stream = CreateIStreamIO(m_io, cFALSE);
	return m_stream;
}

tERROR	PROEIOMessage::Commit()
{
	if( PR_FAIL(SetAccessMode(fACCESS_RW)) )
		return errACCESS_DENIED;

	hIO tmp_io = NULL;

	tERROR l_error = CALL_SYS_ObjectCreate(m_io, &tmp_io, IID_IO, PID_TMPFILE, 0);
	if( PR_SUCC(l_error) )
	{
		CALL_SYS_PropertySetDWord(tmp_io, pgOBJECT_ACCESS_MODE, fACCESS_RW);
		CALL_SYS_PropertySetDWord(tmp_io, pgOBJECT_OPEN_MODE, fOMODE_CREATE_ALWAYS + fOMODE_SHARE_DENY_READ);
		l_error = CALL_SYS_ObjectCreateDone(tmp_io);
	}

	if( PR_SUCC(l_error) )
	{
		IStream *l_stream = CreateIStreamIO(tmp_io, cFALSE);
		if( !l_stream )
			l_error = errUNEXPECTED;
		else
		{
			HRESULT l_result = m_imsg->Save(l_stream, 0);

			if( FAILED(l_result) )
				l_error = MapCommitError(l_result);

			l_stream->Release();
		}
	}

	if( PR_SUCC(l_error) )
		l_error = CopyIO(m_io, tmp_io);

	if( tmp_io )
		CALL_SYS_ObjectClose(tmp_io);

	if( PR_FAIL(l_error) )
		return l_error;

	return errOK;;
}

//////////////////////////////////////////////////////////////////////////
// PROEMessagePart class

PROEMessagePart::PROEMessagePart(PROEMessage *p_msg, HBODY__ *p_body, DWORD p_pos)
	: PROEObject(MSOE_MESSAGE_PART, p_msg, cFALSE), m_pos(p_pos), m_body(p_body),
		m_stream(NULL), m_mdf_stream(NULL), m_mdf_io(NULL), m_origin(0)
{
	m_imsg = p_msg->m_imsg;
}

PROEMessagePart::~PROEMessagePart()
{
	if( m_stream )
		m_stream->Release();
	if( m_mdf_stream )
		m_mdf_stream->Release();
}

tERROR	PROEMessagePart::Init()
{
	if( FAILED(m_imsg->BindToObject(m_body, IID_IStream, (void**)&m_stream)) )
		return errOBJECT_INVALID;

	if( m_imsg->IsContentType(m_body, "message", "rfc822") == S_OK )
		m_origin = OID_MAIL_MSG_MIME;
	else if( m_imsg->IsBodyType(m_body, MSGBODY_ATTACH) == S_OK )
		m_origin = OID_MAIL_MSG_ATTACH;
	else
		m_origin = OID_MAIL_MSG_BODY;

	return errOK;
}

tORIG_ID PROEMessagePart::GetOrigin()
{
	return m_origin;
}

tERROR PROEMessagePart::SetAccessMode(tDWORD p_access_mode)
{
	if( !CheckInit() )
		return errOBJECT_NOT_INITIALIZED;

	if( !(p_access_mode & fACCESS_WRITE) || m_mdf_stream )
		return errOK;

	if( !CheckParentAccess() )
		return errACCESS_DENIED;

	tERROR l_error = CALL_SYS_ObjectCreate(g_root, &m_mdf_io, IID_IO, PID_TMPFILE, 0);
	if( PR_SUCC(l_error) )
	{
		CALL_SYS_PropertySetDWord(m_mdf_io, pgOBJECT_ACCESS_MODE, fACCESS_RW);
		CALL_SYS_PropertySetDWord(m_mdf_io, pgOBJECT_OPEN_MODE, fOMODE_CREATE_ALWAYS + fOMODE_SHARE_DENY_READ);
		l_error = CALL_SYS_ObjectCreateDone(m_mdf_io);
	}

	if( PR_FAIL(l_error) && m_mdf_io )
	{
		CALL_SYS_ObjectClose(m_mdf_io);
		m_mdf_io = NULL;
		return l_error;
	}

	m_mdf_stream = CreateIStreamIO(m_mdf_io, cTRUE);
	if( !m_mdf_stream )
		return errNOT_ENOUGH_MEMORY;

	LLONGDEF(l_offs, 0);
	m_stream->Seek(l_offs, STREAM_SEEK_SET, NULL);

	ULLONGDEF(l_size, -1);
	m_stream->CopyTo(m_mdf_stream, l_size, NULL, NULL);
	return errOK;
}

tERROR PROEMessagePart::GetName(PRPrmBuff &p_buff)
{
	PRVarProp l_filename(VT_LPWSTR);
	PRVarProp l_cnttype(VT_LPWSTR);

	if( !FAILED(GET_BODY_PROP(l_filename, ATT_FILENAME)) )
		p_buff << l_filename;
	else if( !FAILED(GET_BODY_PROP(l_cnttype, HDR_CNTTYPE)) )
		p_buff << l_cnttype;
	else
		return errPROPERTY_NOT_FOUND;
	return p_buff.Error();
}

tERROR PROEMessagePart::GetProp(tPROPID p_prop_id, PRPrmBuff &p_buff)
{
	enum _MESSAGEPROPERTY l_hdr_prop;
	switch( p_prop_id )
	{
	case pgMESSAGE_PART_CONTENT_TYPE:
		l_hdr_prop = HDR_CNTTYPE;
		break;
	case pgMESSAGE_PART_TRANSFER_ENCODING:
		l_hdr_prop = HDR_CNTXFER;
		break;
	case pgMESSAGE_PART_FILE_NAME:
		l_hdr_prop = ATT_FILENAME;
		break;
	case pgMESSAGE_PART_DISPLAY_NAME:
		return GetNameZ(p_buff);
	case pgMESSAGE_PART_CONTENT_ID:
		l_hdr_prop = HDR_CNTID;
		break;
	case pgMESSAGE_PART_CONTENT_DESCRIPTION:
		l_hdr_prop = HDR_CNTDESC;
		break;
	case pgMESSAGE_PART_CHARSET:
		l_hdr_prop = PAR_CHARSET;
		break;

	case pgMESSAGE_X_PROP_NAME:
		return p_buff.SetStr(m_x_prop.c_str());
	case pgMESSAGE_X_PROP_VALUE:
		l_hdr_prop = (_MESSAGEPROPERTY)(int)m_x_prop.c_str();
		break;

	default:
		return errPROPERTY_NOT_FOUND;
	}

	PRVarProp l_prop_val(VT_LPWSTR);
	if( FAILED(GET_BODY_PROP(l_prop_val, l_hdr_prop)) )
		return errPROPERTY_NOT_FOUND;

	return p_buff.SetStr(l_prop_val);
}

tERROR PROEMessagePart::SetProp(tPROPID p_prop_id, PRPrmBuff &p_buff)
{
	enum _MESSAGEPROPERTY l_hdr_prop;
	switch( p_prop_id )
	{
	case pgMESSAGE_PART_CONTENT_TYPE:
		l_hdr_prop = HDR_CNTTYPE;
		break;
	case pgMESSAGE_PART_CONTENT_ID:
		l_hdr_prop = HDR_CNTID;
		break;
	case pgMESSAGE_PART_TRANSFER_ENCODING:
		l_hdr_prop = HDR_CNTXFER;
		break;
	case pgMESSAGE_PART_FILE_NAME:
		l_hdr_prop = ATT_FILENAME;
		break;
	case pgMESSAGE_PART_CONTENT_DESCRIPTION:
		l_hdr_prop = HDR_CNTDESC;
		break;
	case pgMESSAGE_PART_CHARSET:
		l_hdr_prop = PAR_CHARSET;
		break;

	case pgMESSAGE_X_PROP_NAME:
		m_x_prop = p_buff.m_buff;
		return warnPROPERTY_NOT_COMMITED;
	case pgMESSAGE_X_PROP_VALUE:
		l_hdr_prop = (_MESSAGEPROPERTY)(int)m_x_prop.c_str();
		break;

	default:
		return errPROPERTY_NOT_FOUND;
	}

	PRBuffProp l_prop_val(VT_LPSTR, p_buff);
	if( FAILED(SET_BODY_PROP(l_prop_val, l_hdr_prop)) )
		return errUNEXPECTED;

	return errOK;
}

void   PROEMessagePart::SetBody(HBODY__ *p_body)
{
	m_body = p_body;
	((PROEMessage*)m_parent)->m_bodies[m_pos] = p_body;
}

tERROR PROEMessagePart::Delete()
{
	if( !CheckParentAccess() )
		return errACCESS_DENIED;	

	if( m_imsg->IsBodyType(m_body, MSGBODY_ATTACH) != S_OK )
	{
		((PROEMessage*)m_parent)->DeleteAllBodies();
	}
	else
	{
		if( FAILED(m_imsg->DeleteBody(m_body, 0)) )
			return errOBJECT_INVALID;

		SetBody(NULL);
	}

	if( FAILED(m_imsg->Commit(0)) )
		return errOBJECT_INVALID;

	return errOK;
}

tERROR	PROEMessagePart::Commit()
{
	if( !m_mdf_stream )
		return errOK;

	HRESULT l_result = S_OK;

	HBODY__ *l_new_body = NULL;
	if( m_imsg->IsBodyType(m_body, MSGBODY_ATTACH) == S_OK )
	{
		PRVarProp l_filename(VT_LPWSTR);
		GET_BODY_PROP(l_filename, ATT_FILENAME);

		const char* l_name = l_filename;
		l_result = m_imsg->AttachFile(l_name ? l_name : "",
			m_mdf_stream, &l_new_body);
	}
	else
	{
		ULONG l_body_def = 0;
		if( m_imsg->IsContentType(m_body, "text", "plain") == S_OK )
			l_body_def = 1;
		else if( m_imsg->IsContentType(m_body, "text", "html") == S_OK )
			l_body_def = 2;

		l_result = m_imsg->SetTextBody(l_body_def, ENCODING_ANSI, m_body, m_mdf_stream, &l_new_body);
	}

	if( !FAILED(l_result) )
	{
		COPY_BODY_PROP(l_new_body, HDR_CNTTYPE);
		COPY_BODY_PROP(l_new_body, PAR_CHARSET);
		COPY_BODY_PROP(l_new_body, HDR_CNTXFER);
		COPY_BODY_PROP(l_new_body, HDR_CNTDESC);
  
		l_result = m_imsg->DeleteBody(m_body, 0);
		SetBody(l_new_body);
	}

	if( FAILED(l_result) )
		MapCommitError(l_result);

	return errOK;
}

// iIO
tERROR PROEMessagePart::SeekRead(tDWORD* p_result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size)
{
	GET_STREAM()

	if( l_stream == m_mdf_stream )
	{
		STATSTG l_ss;
		if( FAILED(l_stream->Stat(&l_ss, STATFLAG_NONAME)) )
			return errOBJECT_SEEK;

		if( p_offset > l_ss.cbSize.QuadPart )
			return errOUT_OF_OBJECT;
	}

	LLONGDEF(l_offs, p_offset); ULLONGDEF(l_new_offs, 0);
	if( FAILED(l_stream->Seek(l_offs, STREAM_SEEK_SET, &l_new_offs)) )
		return errOBJECT_SEEK;

	if( p_offset != l_new_offs.QuadPart )
		return errOUT_OF_OBJECT;

	__try {
		if( FAILED(l_stream->Read(p_buffer, p_size, (ULONG*)p_result)) )
			return errOBJECT_READ;
	}
	__except(1) {
		PR_TRACE((g_root, prtFATAL, "MSOE: !!!! PROEMessagePart::SeekRead EXCEPTION !!!!"));
		return errUNEXPECTED;
	}

	return errOK;
}

tERROR PROEMessagePart::SeekWrite(tDWORD* p_result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size)
{
	GET_STREAM()

	LLONGDEF(l_offs, p_offset);
	if( FAILED(l_stream->Seek(l_offs, STREAM_SEEK_SET, NULL)) )
		return errOBJECT_SEEK;

	if( FAILED(l_stream->Write(p_buffer, p_size, (ULONG*)p_result)) )
		return errOBJECT_READ;

	return errOK;
}

tERROR PROEMessagePart::GetSize(tQWORD* p_result, IO_SIZE_TYPE p_type)
{
	if( !CheckInit() )
		return errOBJECT_NOT_INITIALIZED;
	GET_STREAM()

	STATSTG l_ss;
	if( FAILED(l_stream->Stat(&l_ss, STATFLAG_NONAME)) )
		return errOBJECT_DATA_SIZE_UNDERSTATED;

	*p_result = l_ss.cbSize.QuadPart;
	return errOK;
}

tERROR PROEMessagePart::SetSize(tQWORD p_new_size)
{
	GET_STREAM()

	ULLONGDEF(l_size, p_new_size);
	if( FAILED(l_stream->SetSize(l_size)) )
		return errOBJECT_RESIZE;

	return errOK;
}

tERROR PROEMessagePart::Flush()
{
	return errOK;
}

//////////////////////////////////////////////////////////////////////////
