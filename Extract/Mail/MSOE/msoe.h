//////////////////////////////////////////////////////////////////////////
// Header file for OE objects

#pragma warning ( disable : 4786 )
//#define _WIN32_DCOM

#include <stdio.h>
#include <wchar.h>
#include <objbase.h>
#include <crtdbg.h>

#include <vector>
#include <map>
#include <string>

#include "msoe_base.h"
#include "msoeAPI.h"

#define MSOE_MAILSYSTEM		MAIL_OS_TYPE_SYSTEM
#define MSOE_DATABASE		MAIL_OS_TYPE_DBX
#define MSOE_MESSAGE		MAIL_OS_TYPE_MIME
#define MSOE_MESSAGE_PART	3
#define MSOE_MESSAGE_OBJ	4
#define MSOE_MESSAGE_EMB	5

class PROEMailSystem;
class PROEIdentityDatabase;
class PROEDatabase;
class PROEMessage;
class PROEDBMessage;
class PROEMessagePart;

//////////////////////////////////////////////////////////////////////////
// PROEMailSystem class

class PROEMailSystem : public PROEObject
{
public:
	PROEMailSystem();
	~PROEMailSystem();
	virtual tERROR	Init();

	tERROR	GetNext(PROEObject *&p_obj);

private:
	typedef std::vector<PROEIdentityDatabase*> TIdentities;
	TIdentities		m_idetities;
	HANDLE          m_token_old;
	HANDLE          m_token_new;
};

//////////////////////////////////////////////////////////////////////////
// PROEMailSystem class

class PROEVistaMail : public PROEObject
{
public:
	PROEVistaMail(PROEObject* p_parent = NULL, hObjPtr hPtr = NULL);
	~PROEVistaMail();
	virtual tERROR	Init();

	PROEObject * LoadChild(PRPrmBuff &p_buff);
	tERROR	SaveChild(PRPrmBuff &p_buff, PROEObject *p_obj);
	tERROR  GetName(PRPrmBuff &p_buff);
	tERROR	GetCurObj(PROEObject *&p_obj);
	tERROR  ChangeTo(const char* name);
	tERROR	GetNext(PROEObject *&p_obj);
	tERROR  GetProp(tPROPID p_prop_id, PRPrmBuff &p_buff);

private:
	hObjPtr         m_nfio_ptr;
	HANDLE          m_token_old;
	HANDLE          m_token_new;
	tBOOL           m_is_folders;
	tCHAR           m_name[MAX_PATH];
};

//////////////////////////////////////////////////////////////////////////
// PROEDatabase class

class PROEDatabaseBase
{
public:
	PROEDatabaseBase(const char *p_dbx_file = NULL);
	~PROEDatabaseBase(){ Close(); }

	bool	CheckPath();

	HRESULT	Open(tagTABLESCHEMA *p_schema);
	void	Close();

public:
	IDatabaseSession * m_session;
	std::string		m_dbx_file;
	DWORD			m_attrs;
	IDatabase *		m_idb;
    HROWSET__*		m_rowset;
	IDatabaseExtension 	m_db_ext;
	tBOOL			m_com_inited;
};

//////////////////////////////////////////////////////////////////////////
// PROEDatabase class

class PROEDatabase : public PROEObject, private PROEDatabaseBase
{
public:
	PROEDatabase(PROEObject *p_parent);
	PROEDatabase(const char *p_dbx_file, tagFOLDERINFO *p_fldinfo);
	PROEDatabase(hIO p_io);

	void SetParent(PROEDatabase *p_parent);

	virtual ~PROEDatabase();
	virtual tERROR	Init();

	virtual tERROR	Commit();

	virtual PROEObject * LoadChild(PRPrmBuff &p_buff);
	virtual tERROR	SaveChild(PRPrmBuff &p_buff, PROEObject *p_obj);

// iObjPtr
	virtual tERROR	GetProp(tPROPID p_prop_id, PRPrmBuff &p_buff);
	virtual tERROR	GetName(PRPrmBuff &p_buff);
	virtual tERROR	GetNext(PROEObject *&p_obj);
	virtual tERROR	GetNextMessage(PROEObject *&p_obj);
	virtual tERROR	GetSize(tQWORD* p_result, IO_SIZE_TYPE p_type);

public:
	tDWORD			m_record_id;
	tDWORD			m_parent_id;

protected:
	friend class PROEDBMessage;

	hOBJECT	GetNativeIO();

	std::string		m_name;
	DWORD			m_pos;
	DWORD			m_msgcount;
	hIO				m_tmp_nfio;
	hIO				m_io;
	hIO				m_nfio;
	DWORD			m_type;

	typedef std::vector<PROEDatabase*> TChilds;
	TChilds			m_childs;
};

//////////////////////////////////////////////////////////////////////////
// PROEDatabaseMail class

class PROEIdentityDatabase : public PROEDatabase
{
public:
	PROEIdentityDatabase(PROEMailSystem *p_parent, tDWORD p_id_pos, const char *p_identity, const char *p_dbx_path);
	~PROEIdentityDatabase();
	virtual tERROR	Init();

// Methods
	tERROR	GetName(PRPrmBuff &p_buff);

private:
	friend class PROEMailSystem;

	std::string		m_dbx_path;
	std::string		m_identity;
	tDWORD			m_id_pos;
};

//////////////////////////////////////////////////////////////////////////
// PROEMessage class

class PROEMessage : public PROEObject
{
public:
	PROEMessage(PROEObject *p_parent = NULL);
	~PROEMessage();
	virtual tERROR	Init();

	virtual tORIG_ID GetOrigin(){ return OID_MAIL_MSG_MIME; }

	virtual PROEObject * LoadChild(PRPrmBuff &p_buff);
	virtual tERROR	SaveChild(PRPrmBuff &p_buff, PROEObject *p_obj);

// iObjPtr
	virtual tERROR	GetName(PRPrmBuff &p_buff);
	virtual tERROR	GetNext(PROEObject *&p_obj);

	virtual tERROR	GetProp(tPROPID p_prop_id, PRPrmBuff &p_buff);
	virtual tERROR	SetProp(tPROPID p_prop_id, PRPrmBuff &p_buff);

	virtual IStream * GetStream(){ return NULL; }
	virtual tERROR	GetSize(tQWORD* p_result, IO_SIZE_TYPE p_type);

	virtual PROEObject * CreateChild(tDWORD p_create_mode, tCHAR *p_obj_name);

protected:
	tERROR	GetHeaders(PRPrmBuff &p_buff);
	tERROR	GetBodyName(HBODY__ *p_body, tSTRING p_buff, tSIZE_T p_buff_sz);
	tERROR  GetBodyID(HBODY__ *p_body, tCHAR *p_id_buff);
	tERROR	GetBodyHash(HBODY__ *p_body, tQWORD *p_id_hash);
	tERROR  DeleteAllBodies();

	friend class PROEMessagePart;

	IMimeMessageW *	m_imsg;
	DWORD			m_partnum;
	HBODY__ **		m_bodies;
	std::string		m_x_prop;
	tBOOL			m_com_inited;
};

class PROEDBMessage : public PROEMessage
{
public:
	PROEDBMessage(PROEDatabase *p_db, DWORD p_pos, tagMESSAGEINFO *p_info);
	virtual IStream * GetStream();
	virtual tERROR	GetProp(tPROPID p_prop_id, PRPrmBuff &p_buff);

	virtual tERROR	Commit();
	virtual tERROR	Delete();

	bool IsAtachments();
private:
	friend class PROEDatabase;

	IDatabase *		m_idb;
	DWORD			m_pos;
	DWORD			m_offset;
	DWORD			m_record_id;
};

class PROEIOMessage : public PROEMessage
{
public:
	PROEIOMessage(hIO p_io, PROEObject* pParent = NULL) : PROEMessage(pParent), m_io(p_io), m_stream(NULL){}
	~PROEIOMessage();
	virtual IStream * GetStream();

	virtual tERROR	SetAccessMode(tDWORD p_access_mode);
	virtual tERROR	Commit();

public:
	hIO				m_io;
	IStream *		m_stream;
};

//////////////////////////////////////////////////////////////////////////
// PROEMessagePart class

class PROEMessagePart : public PROEObject
{
public:
	PROEMessagePart(PROEMessage *p_msg, HBODY__ *p_body, DWORD p_pos);
	~PROEMessagePart();
	virtual tERROR	Init();

	virtual tORIG_ID	GetOrigin();
	virtual tDWORD	GetAvailability(){ return PROEObject::GetAvailability()|fAVAIL_READ|fAVAIL_WRITE|fAVAIL_EXTEND|fAVAIL_TRUNCATE; }

	virtual tERROR	SetAccessMode(tDWORD p_access_mode);

	virtual tERROR	GetName(PRPrmBuff &p_buff);
	virtual tERROR	GetProp(tPROPID p_prop_id, PRPrmBuff &p_buff);
	virtual tERROR	SetProp(tPROPID p_prop_id, PRPrmBuff &p_buff);

	virtual tERROR	Delete();
	virtual tERROR	Commit();

// iIO
	virtual tERROR	SeekRead(tDWORD* p_result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size);
	virtual tERROR	SeekWrite(tDWORD* p_result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size);
	virtual tERROR	GetSize(tQWORD* p_result, IO_SIZE_TYPE p_type);
	virtual tERROR	SetSize(tQWORD p_new_size);
	virtual tERROR	Flush();

	tORIG_ID		m_origin;

private:
	friend class PROEMessage;
	inline void		SetBody(HBODY__ *p_body);

	DWORD			m_pos;
	IMimeMessageW *	m_imsg;
	HBODY__ *		m_body;
	IStream *		m_stream;
	IStream *		m_mdf_stream;
	hIO				m_mdf_io;
	std::string		m_x_prop;
};

//////////////////////////////////////////////////////////////////////////

class PRVarProp : public VARIANT
{
public:
	PRVarProp(int p_type){ vt = p_type; pbVal=0; m_buff=NULL; }
	~PRVarProp()
	{
		if( (vt == VT_LPSTR || vt == VT_LPWSTR) && pbVal )
			CoTaskMemFree(pbVal);
		if( m_buff )
			free(m_buff);
	}

	operator tagPROPVARIANT*() { return (tagPROPVARIANT*)this; }

	operator const char*()
	{
		if( vt == VT_LPSTR ) return (char*)pbVal;
		if( vt != VT_LPWSTR || !pbVal ) return "";
		if( m_buff ) return m_buff;

		long buflen = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)pbVal, -1, NULL, 0, NULL, NULL);
		if( buflen == 0 ) return "";
		m_buff = (char*)malloc(buflen);
		if( !WideCharToMultiByte(CP_ACP, 0, (LPWSTR)pbVal, -1, m_buff, buflen, NULL, NULL) )
			return "";
		return m_buff;
	}

private:
	char* m_buff;
};

class PRBuffProp : public VARIANT
{
public:
	PRBuffProp(int p_type, PRPrmBuff &p_buff){vt = p_type; pbVal = (PBYTE)p_buff.m_buff;}
};

#define GET_PROP(var, prop) \
	m_imsg->GetPropA((char*)prop, 0, (tagPROPVARIANT*)&var)

#define SET_PROP(var, prop) \
	m_imsg->SetPropA((char*)prop, 0, (tagPROPVARIANT*)&var)

#define GET_BODY_PROP(var, prop) \
	m_imsg->GetBodyProp(m_body, (char*)prop, 0, (tagPROPVARIANT*)&var)

#define SET_BODY_PROP(var, prop) \
	m_imsg->SetBodyProp(m_body, (char*)prop, 0, (tagPROPVARIANT*)&var)

#define COPY_BODY_PROP(dst, prop) \
	PRVarProp l_##prop(VT_LPSTR); \
	if( !FAILED(m_imsg->GetBodyProp(m_body, (char*)prop, 0, (tagPROPVARIANT*)&l_##prop)) ) \
		m_imsg->SetBodyProp(dst, (char*)prop, 0, (tagPROPVARIANT*)&l_##prop)

#define GET_STREAM() \
	IStream *l_stream = m_mdf_stream ? m_mdf_stream : m_stream; \
	if( !l_stream ) return errOBJECT_NOT_INITIALIZED;

#define	LLONGDEF(name, val)		LARGE_INTEGER name; name.QuadPart = val
#define	ULLONGDEF(name, val)	ULARGE_INTEGER name; name.QuadPart = val

//////////////////////////////////////////////////////////////////////////
