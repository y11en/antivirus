#ifndef __S_QB_H
#define __S_QB_H

#include <AV/structs/s_avs.h>
#include <ProductCore/plugin/p_qb.h>
#include <ProductCore/structs/s_taskmanager.h>

//-----------------------------------------------------------------------------
// cQBObject structure

struct cQBObject : public cInfectedObjectInfo
{
	cQBObject():
		m_qwObjectId(0),
		m_nSendToKL(0),
		m_qwSize(0)
	{}

	DECLARE_IID( cQBObject, cInfectedObjectInfo, PID_QB, 1 );

	enum Actions{ RESTORE = eIID };

	tOBJECT_ID            m_qwObjectId;
	tQWORD                m_qwSize;    // For internal use
	tDWORD                m_nSendToKL;
	cSerializableObj      m_AdditionalData;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cQBObject, 0 )
	SER_BASE( cInfectedObjectInfo, 0 )
	SER_VALUE( m_qwObjectId,            tid_QWORD,           "ObjectId" )
	SER_VALUE( m_qwSize,                tid_QWORD,           "ObjectSize" )
	SER_VALUE( m_nSendToKL,             tid_DWORD,           "SendToKL" )
	SER_VALUE_PTR_NO_NAMESPACE( m_AdditionalData, cSerializable::eIID, "AdditionalData" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

#endif//  __S_QB_H
