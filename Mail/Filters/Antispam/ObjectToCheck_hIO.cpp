#include "antispam_interface_imp2.h"

CAntispamCaller::CObjectToCheck_hIO::CObjectToCheck_hIO(): 
	m_io(0), 
	CObjectToCheck_hObject(),
	CObjectIOToCheck()
{
};
CAntispamCaller::CObjectToCheck_hIO::CObjectToCheck_hIO(hOBJECT obj): m_io(0), CObjectToCheck_hObject(obj)
{
	if (obj->propGetDWord(pgINTERFACE_ID) == IID_IO)
	{
		m_io = (hIO)obj;
		m_bReadyToWork = true;
	}
};
CAntispamCaller::CObjectToCheck_hIO::CObjectToCheck_hIO(CObjectToCheck* pobj): m_io(0)
{
	CObjectToCheck_hObject* pObj = (CObjectToCheck_hObject*)pobj;
	if ( pObj )
	{
		hOBJECT obj = pObj->GetObject();
		if ( obj ) 
		{
			if (obj->propGetDWord(pgINTERFACE_ID) == IID_IO)
			{
				m_io = (hIO)obj;
				m_bReadyToWork = true;
			}
		}
	}
};
CAntispamCaller::CObjectToCheck_hIO::~CObjectToCheck_hIO()
{
};
tERROR CAntispamCaller::CObjectToCheck_hIO::SeekRead( 
	OUT tDWORD* out_size, 
	IN tQWORD offset, 
	IN tPTR buffer, 
	IN tDWORD size 
	)
{
	if ( m_bReadyToWork )
		return m_io->SeekRead(out_size, offset, buffer, size);
	else
		return errACCESS_DENIED;
};
tERROR CAntispamCaller::CObjectToCheck_hIO::GetSize( OUT tQWORD* out_size )
{
	if ( m_bReadyToWork )
		return m_io->GetSize(out_size, IO_SIZE_TYPE_EXPLICIT);
	else
		return errACCESS_DENIED;
};
hIO CAntispamCaller::CObjectToCheck_hIO::GetIO()
{
	return m_io;
};
