#include "antispam_interface_imp2.h"

CAntispamCaller::CObjectToCheck_hObject::CObjectToCheck_hObject(): m_obj(), m_bReadyToWork(false)
{
};
CAntispamCaller::CObjectToCheck_hObject::CObjectToCheck_hObject(hOBJECT obj): m_obj(obj), m_bReadyToWork(false)
{
	if ( obj )
		m_bReadyToWork = true;
};

CAntispamCaller::CObjectToCheck_hObject::~CObjectToCheck_hObject()
{
};

tERROR CAntispamCaller::CObjectToCheck_hObject::PropertyGet( 
	OUT tDWORD* out_size, 
	IN tPROPID prop_id, 
	IN tPTR buffer, 
	IN tDWORD size 
	)
{
	if ( m_bReadyToWork )
		return m_obj->propGet(out_size, prop_id, buffer, size);
	else
		return errACCESS_DENIED;
};

hOBJECT CAntispamCaller::CObjectToCheck_hObject::GetObject()
{
	return m_obj;
}
