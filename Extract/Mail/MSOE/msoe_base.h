//////////////////////////////////////////////////////////////////////////
// Header file for Base OE objects

#if !defined( __MSOE_BASE_INCLUDE )
#define __MSOE_BASE_INCLUDE

#include <Prague/prague.h>
#include <Prague/pr_oid.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_objptr.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_root.h>

#include <Extract/plugin/p_msoe.h>

#include "buff_prm.h"

#define TYPE_UNDEF					-1
#define MAX_NAME_SIZE				0x200

class PROEObject;

//////////////////////////////////////////////////////////////////////////

struct PROEFindObject
{
	PROEFindObject(tDWORD p_mask):mask(p_mask), folder(NULL), object(NULL){}
	~PROEFindObject();

	tDWORD			mask;
	PROEObject *	folder;
	PROEObject *	object;
	tCHAR			objname[MAX_NAME_SIZE];
};

#define FIND_FOLDER			1
#define FIND_OBJECT			2
#define FIND_OBJNAME		4
#define FIND_OBJANY			(FIND_OBJECT|FIND_OBJNAME)

//////////////////////////////////////////////////////////////////////////

class PROEObject
{
public:
	PROEObject(tDWORD p_type, PROEObject *p_parent = NULL, tBOOL p_folder = cTRUE)
		: m_type(p_type), m_parent(p_parent), m_ref(1), f_folder(p_folder), f_init(0), f_initfail(0), f_dirty(0)
		{ if( m_parent ) m_parent->AddRef(); }
	virtual ~PROEObject(){ if( m_parent ) m_parent->Release(); }

// reference methods
	void	AddRef(){ ++m_ref; }
	void	Release(){ if( !--m_ref ) delete this; }

// recognize and construct
	static	void	Initialize(hROOT root);
	static	void	Deinitialize();
	static	void	ThreadAttach(bool fAttach);
	static	tDWORD	Recognize(hOBJECT p_obj, tDWORD type);
	static	PROEObject * Construct(tDWORD p_type, hOBJECT p_parent);

// check methods
	inline bool		CheckInit();
	inline bool		CheckParentAccess();
	inline tERROR	CheckDelete();
	inline tERROR	SaveChanges();

	inline tERROR	GetNameZ(PRPrmBuff &p_buff);
	inline tERROR	GetFullNameZ(PRPrmBuff &p_buff, tBOOL f_path);

// identification methods
	PROEObject *	GetParent();
	bool			GetObject(tSTRING p_full_name, PROEFindObject &p_find);
	bool			GetObject(hOBJECT p_name, PROEFindObject &p_find);
	PROEObject *	LoadObject(PRPrmBuff &p_buff);
	tERROR			SaveObject(PRPrmBuff &p_buff);
	tERROR			GetFullName(PRPrmBuff &p_buff, tBOOL f_path);

// property methods
	virtual tORIG_ID GetOrigin(){ return OID_GENERIC_IO; }
	virtual tDWORD	GetAvailability(){ return fAVAIL_CHANGE_MODE|fAVAIL_CREATE|fAVAIL_SHARE|fAVAIL_DELETE_ON_CLOSE; }
	virtual tERROR	GetProp(tPROPID p_prop_id, PRPrmBuff &p_buff){ return errPROPERTY_NOT_FOUND; }
	virtual tERROR	SetProp(tPROPID p_prop_id, PRPrmBuff &p_buff){ return errPROPERTY_NOT_FOUND; }

// iOS
	virtual tERROR	Init(){ f_init = cTRUE; return errOK; }
	virtual tERROR	Copy(PROEFindObject &p_dst, tBOOL f_overwrite){ return errNOT_IMPLEMENTED; }
	virtual tERROR	Rename(PROEFindObject &p_dst, tBOOL f_overwrite){ return errNOT_IMPLEMENTED; }
	virtual tERROR	Delete(){ return errNOT_IMPLEMENTED; }
	virtual tERROR	Commit(){ return warnNOTHING_TO_COMMIT; }
	virtual tERROR	SetAccessMode(tDWORD p_access_mode){ return errOK; }

// iObjPtr
	virtual PROEObject * GetRoot();
	virtual PROEObject * GetChild(tSTRING p_name);
	virtual PROEObject * LoadChild(PRPrmBuff &p_buff);
	virtual tERROR	SaveChild(PRPrmBuff &p_buff, PROEObject *p_obj){ return p_obj->GetNameZ(p_buff); }
	virtual PROEObject * CreateChild(tDWORD p_create_mode, tCHAR *p_obj_name){ return NULL; }
	virtual tERROR	GetNext(PROEObject *&p_obj){ return errNOT_IMPLEMENTED; }
	virtual tERROR	GetName(PRPrmBuff &p_buff){ return errNOT_IMPLEMENTED; }

// iIO
	virtual tERROR	SeekRead(tDWORD* p_result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size){ return errNOT_IMPLEMENTED; }
	virtual tERROR	SeekWrite(tDWORD* p_result, tQWORD p_offset, tPTR p_buffer, tDWORD p_size){ return errNOT_IMPLEMENTED; }
	virtual tERROR	GetSize(tQWORD* p_result, IO_SIZE_TYPE p_type){ return errNOT_IMPLEMENTED; }
	virtual tERROR	SetSize(tQWORD p_new_size){ return errNOT_IMPLEMENTED; }
	virtual tERROR	Flush(){ return errNOT_IMPLEMENTED; }

public:
	PROEObject *	m_parent;
	tDWORD			m_type;

	tDWORD			m_ref;
	unsigned		f_folder:1;
	unsigned		f_init:1;
	unsigned		f_initfail:1;
	unsigned		f_dirty:1;
};

//////////////////////////////////////////////////////////////////////////

bool PROEObject::CheckInit()
{
	if( f_init ) return true;
	if( f_initfail ) return false;

	bool f_ret = PR_SUCC(Init());
	f_ret ? f_init = 1 : f_initfail = 1;
	return f_ret;
}

bool PROEObject::CheckParentAccess()
{
	if( !m_parent )
		return true;
	return PR_SUCC(m_parent->SetAccessMode(fACCESS_RW));
}

tERROR	PROEObject::CheckDelete()
{
	tERROR l_err = Delete();
	if( PR_SUCC(l_err) && m_parent )
		m_parent->f_dirty = 1;
	return l_err;
}

tERROR	PROEObject::SaveChanges()
{
	if( !f_dirty )
		return errOK;

	tERROR l_err;
	switch(l_err = Commit())
	{
	case errOK: if( m_parent ) m_parent->f_dirty = 1;
	case warnPARENT_NOT_COMMITED:
	case warnNOTHING_TO_COMMIT: f_dirty = 0; return errOK;
	}
	return l_err;
}

tERROR	PROEObject::GetNameZ(PRPrmBuff &p_buff)
{
	tERROR l_error = GetName(p_buff);
	if( PR_SUCC(l_error) )
		p_buff.StrEnd();
	return l_error;
}

tERROR	PROEObject::GetFullNameZ(PRPrmBuff &p_buff, tBOOL f_path)
{
	tERROR l_error = GetFullName(p_buff, f_path);
	if( PR_SUCC(l_error) )
		p_buff.StrEnd();
	return l_error;
}

//////////////////////////////////////////////////////////////////////////

extern PROEObject * OS_GetObject(hOS p_hobj);

extern tERROR		ObjPtr_SetObject(hObjPtr p_hobj, PROEObject *p_ptr, PROEObject *p_obj);
extern PROEObject *	ObjPtr_GetObject(hObjPtr p_hobj);
extern PROEObject *	ObjPtr_GetPtrObj(hObjPtr p_hobj);

extern tERROR		IO_Create(hOS p_os, PROEFindObject &p_find, tDWORD p_access_mode, tDWORD p_open_mode, hIO* p_result);
extern PROEObject *	IO_GetObject(hIO p_hobj);

#endif // __MSOE_BASE_INCLUDE
//////////////////////////////////////////////////////////////////////////
