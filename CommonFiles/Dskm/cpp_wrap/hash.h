/*
*/

#if !defined(__KL_DSKMWRAP_HASH_H)
#define __KL_DSKMWRAP_HASH_H

#include "data_buffer.h"
#include <memory>

namespace KL  {
namespace DskmLibWrapper  {

//////////////////////////////////////////////////////////////////////////
class Hash : public DataBuffer
{
public:
	Hash(void* pData, AVP_dword nDataSize, bool bCopy = false)
		: DataBuffer(pData, nDataSize, bCopy)
	{
	}
};

//////////////////////////////////////////////////////////////////////////
typedef std::auto_ptr<Hash> HashAP;

}  // namespace DSKM
}  // namespace KL

#endif  // !defined(__KL_DSKMWRAP_HASH_H)
