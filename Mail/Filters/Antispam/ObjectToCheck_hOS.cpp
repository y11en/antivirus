#include "antispam_interface_imp2.h"

CAntispamCaller::CObjectToCheck_hOS::CObjectToCheck_hOS(hOS os): m_os(os), m_bOSCreatedInternal(false), CObjectToCheck_hObject()
{
	if ( os )
		m_bReadyToWork = true;
};
CAntispamCaller::CObjectToCheck_hOS::CObjectToCheck_hOS(CObjectToCheck* pobj): m_os(0), m_bOSCreatedInternal(false)
{
	CObjectToCheck_hObject* pObj = (CObjectToCheck_hObject*)pobj;
	if ( pObj )
	{
		hOBJECT obj = pObj->GetObject();
		if ( obj ) 
		{
			tERROR err = obj->propSetBool(pgPROXY_CREATE_REMOTE, cTRUE);

			if (obj->propGetDWord(pgINTERFACE_ID) == IID_OS)
				m_os = (hOS)obj;
			else if ( obj->propGetDWord(pgINTERFACE_ID) == IID_IO )
			{
				tERROR err = obj->sysCreateObjectQuick( (hOBJECT*)&m_os, IID_OS, PID_MSOE );
				if ( PR_FAIL(err) ) 
					m_os = 0;
				else
					m_bOSCreatedInternal = true;
			}
		}
	}
};
CAntispamCaller::CObjectToCheck_hOS::~CObjectToCheck_hOS()
{
	if ( m_bOSCreatedInternal )
		m_os->sysCloseObject();
};

tERROR CAntispamCaller::CObjectToCheck_hOS::PtrCreate(OUT tUINT* pObjPtr)
{
	tERROR err = errOK;
	hObjPtr optr;
	if ( m_os )
	{
		err = m_os->PtrCreate(&optr, NULL);
		if ( PR_SUCC(err) )
			*pObjPtr = (tUINT)optr;
	}
	return err;
};

tERROR CAntispamCaller::CObjectToCheck_hOS::ObjectCreate(IN tUINT pObjPtr, OUT CObjectToCheck** ppObject)
{
	tERROR err = errOK;
	hIO io;
	hObjPtr optr = (hObjPtr)pObjPtr;
	err = optr->Next();
	if ( PR_SUCC(err) )
	{
		err = optr->IOCreate(&io, 0, fACCESS_READ, fOMODE_OPEN_IF_EXIST);
		if ( PR_SUCC(err) )
		{
			CObjectToCheck_hObject* pObject = new CObjectToCheck_hObject((hOBJECT)io);
			if ( !pObject )
				err = errNOT_OK;
			*ppObject = pObject;
		}
	}
	return err;
};

tERROR CAntispamCaller::CObjectToCheck_hOS::ObjectClose(IN tUINT pObjPtr, IN CObjectToCheck* pObject)
{
	tERROR err = errOK;
	hOBJECT obj = ((CObjectToCheck_hObject*)pObject)->GetObject();
	if ( obj ) 
		err = obj->sysCloseObject();
	delete pObject;
	return err;
};

tERROR CAntispamCaller::CObjectToCheck_hOS::PtrClose(IN tUINT pObjPtr)
{
	if ( pObjPtr )
		return ((hObjPtr)pObjPtr)->sysCloseObject();
	else
		return errOBJECT_NOT_FOUND;
};

hOS CAntispamCaller::CObjectToCheck_hOS::GetOS()
{
	return m_os;
};
