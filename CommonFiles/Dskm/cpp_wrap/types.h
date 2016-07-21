/*
*/

#if !defined(__KL_DSKMWRAP_TYPES_H)
#define __KL_DSKMWRAP_TYPES_H

#include <dskm/dskm.h>
#include <string>
#include "exceptions.h"

namespace KL  {
namespace DskmLibWrapper  {

//////////////////////////////////////////////////////////////////////////
typedef AVP_byte byte_t;
typedef AVP_char char_t;
typedef std::basic_string<char_t> string_t;

//////////////////////////////////////////////////////////////////////////
struct KeyType
{
	enum Values
	{
		KLMasterPrivate	= DSKM_OTYPE_KEY_KL_MASTER_PRIVATE,
		KLMasterPublic	= DSKM_OTYPE_KEY_KL_MASTER_PUBLIC,
		KLPrivate		= DSKM_OTYPE_KEY_KL_PRIVATE,
		KLPublic		= DSKM_OTYPE_KEY_KL_PUBLIC,
		OperablePrivate	= DSKM_OTYPE_KEY_OPERABLE_PRIVATE,
		OperablePublic	= DSKM_OTYPE_KEY_OPERABLE_PUBLIC
	};
};

//////////////////////////////////////////////////////////////////////////
class ObjectType
{
public:
	enum Standart
	{
		KeyKLMasterPrivate	= DSKM_OTYPE_KEY_KL_MASTER_PRIVATE,
		KeyKLMasterPublic	= DSKM_OTYPE_KEY_KL_MASTER_PUBLIC,
		KeyKLPrivate		= DSKM_OTYPE_KEY_KL_PRIVATE,
		KeyKLPublic			= DSKM_OTYPE_KEY_KL_PUBLIC,
		KeyOperablePrivate	= DSKM_OTYPE_KEY_OPERABLE_PRIVATE,
		KeyOperablePublic	= DSKM_OTYPE_KEY_OPERABLE_PUBLIC,
		LicenseKey			= DSKM_OTYPE_LICENCE_KEY,
		LicenseKeyOrder		= DSKM_OTYPE_LICENCE_KEY_ORDER,
		AssociationFile		= DSKM_OTYPE_ASSOC_FILE
	};
	ObjectType(Standart standart_type)
		: m_code(static_cast<int>(standart_type))
	{
	}
	ObjectType(AVP_dword user_defined_type)
		: m_code(user_defined_type)
	{
		if (m_code < DSKM_OTYPE_USER_MIN || m_code > DSKM_OTYPE_USER_MAX)
			throw DskmLibWrapperException(DskmLibWrapperException::InvalidArgument);
	}
	bool IsStandart() const  { return (m_code > DSKM_OTYPE_USER_MAX || m_code < DSKM_OTYPE_USER_MIN); }
	bool IsUser() const  { return (m_code <= DSKM_OTYPE_USER_MAX && m_code >= DSKM_OTYPE_USER_MIN); }
	AVP_dword GetCode() const  { return m_code;	}
	operator AVP_dword () const  { return m_code; }
private:
	AVP_dword m_code;
};

}  // namespace DSKM
}  // namespace KL

#endif  // !defined(__KL_DSKMWRAP_TYPES_H)
