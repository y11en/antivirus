/*
*/

#if !defined(__KL_DSKMWRAP_KEY_H)
#define __KL_DSKMWRAP_KEY_H

#include "data_buffer.h"
#include <memory>

namespace KL  {
namespace DskmLibWrapper  {

//////////////////////////////////////////////////////////////////////////
class Key : public DataBuffer
{
public:
	Key(void* pData, AVP_dword nDataSize, bool bCopy = false)
		: DataBuffer(pData, nDataSize, bCopy)
	{
	}
};

//////////////////////////////////////////////////////////////////////////
typedef std::auto_ptr<Key> KeyAP;

//////////////////////////////////////////////////////////////////////////
class KeyPair
{
public:
	KeyPair(
		void* pPrivateKeyData, AVP_dword nPrivateKeySize,
		void* pPublicKeyData, AVP_dword nPublicKeySize,
		bool bCopy = false
		)
		: m_private(pPrivateKeyData, nPrivateKeySize, bCopy)
		, m_public(pPublicKeyData, nPublicKeySize, bCopy)
	{
	}
	const Key& PrivateKey() const  { return m_private; }
	const Key& PublicKey() const  { return m_public; }
private:
	Key m_private;
	Key m_public;
};

//////////////////////////////////////////////////////////////////////////
typedef std::auto_ptr<KeyPair> KeyPairAP;

}  // namespace DSKM
}  // namespace KL

#endif  // !defined(__KL_DSKMWRAP_KEY_H)
