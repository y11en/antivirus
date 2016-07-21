#ifndef _POLISTORAGEBLOCK_H__INCLUDE_
#define _POLISTORAGEBLOCK_H__INCLUDE_

#include "block.h"
#include "inl/PoliType.h"
#include <assert.h>

namespace KLUTIL
{

class KLUTIL_API CPoliStorageBlock: public PoliStorage, public CBlock
{
private:
	const char* m_pReadPos;
public:
	CPoliStorageBlock(): CBlock(), m_pReadPos(NULL) {}
	//! Отчищает блок данных, надо вызывать после каждой (де)сериализации.
	void ClearAfterDeserialization()
	{
		Clear();
		m_pReadPos = NULL;
	}
	virtual bool read_(void* pOutBuff, const size_t OutBuffSize)
	{
		if (m_pReadPos == NULL && (m_pReadPos = static_cast<const char*>(GetData())) == NULL)
			return false;
		unsigned res = (unsigned)GetDataSize() - (unsigned)(m_pReadPos - static_cast<const char*>(GetData()));
		assert(res >= OutBuffSize); // Данные исчезли, а состояние не сбросили. Надо вызывать ClearAfterDeserialization()!
		if (res < OutBuffSize)
			return false;
		if (res > OutBuffSize)
			res = OutBuffSize;
		memcpy(pOutBuff, m_pReadPos, res);
		m_pReadPos += res;
		return true;
	}
	virtual bool write_(const void* pDataBuff, const size_t DataBuffSize)
	{
		MakeReserve(DataBuffSize);
		memcpy(GetDataEnd(), pDataBuff, DataBuffSize);
		AddDataSize(DataBuffSize);
		return true;
	}
};

} // namespace KLUTIL

#endif
