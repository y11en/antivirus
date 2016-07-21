#include "precompiled.h"
#include "LicenseObjectHelper.h"

namespace KasperskyLicensing {
namespace Implementation {

//-----------------------------------------------------------------------------

LicenseObjectHelper::LicenseObjectHelper(const ILicenseObject& obj): lic_object(obj)
{
}

//-----------------------------------------------------------------------------
	
bool LicenseObjectHelper::IsPlatformSupported(unsigned platform_id) const
{
	ILicenseObject::PlatformEnumerator pe = lic_object.GetSupportedPlatforms();

	for (; !pe.IsDone(); pe.Next())
	{
		if (pe.Item() == platform_id)
			return true;
	}

	return false;
}

//-----------------------------------------------------------------------------

ILicenseObject::LicenseObjectEnumerator 
	LicenseObjectHelper::FindLicenseObject(const ILicenseContext& context, unsigned platform_id) const
{
	// set context enumerator
	ILicenseContext::ItemEnumerator ce = context.GetItems();

	if (!ce.IsDone())
		return FindLicenseObject(ce, platform_id);

	return ILicenseObject::LicenseObjectEnumerator(0);
}

//-----------------------------------------------------------------------------

ILicenseObject::LicenseObjectEnumerator 
	LicenseObjectHelper::FindLicenseObject(ILicenseContext::ItemEnumerator& ce, unsigned platform_id) const
{
	ILicenseObject::LicenseObjectEnumerator oe = lic_object.GetLicenseObjects();
	// find corresponding license object
	for (; !oe.IsDone(); oe.Next())
	{
		LicenseObjectHelper obj(oe.Item());

		// license object is found
		if ((ce.Item() == oe.Item().GetObjectId() && obj.IsPlatformSupported(platform_id)))
		{
			// if at the end of context - exit recursion
			if (ce.Next().IsDone())
				return oe;
			else
				return obj.FindLicenseObject(ce, platform_id);
		}
	}

	// object not found
	return ILicenseObject::LicenseObjectEnumerator(0);
}

//-----------------------------------------------------------------------------

bool LicenseObjectHelper::ContainsLicenseObject(
	const ILicenseContext& context, 
	unsigned platform_id
) const
{
	// if root context is specified
	if (context.IsEmpty())
		return true;

	ILicenseObject::LicenseObjectEnumerator e = FindLicenseObject(context, platform_id);

	// object not found
	if (e.IsDone())
		return false;

	return true;
}

//-----------------------------------------------------------------------------

bool operator == (const LicenseObjectId& lhs, const LicenseObjectId& rhs)
{
	return
		lhs.id == rhs.id						&&
		lhs.type == rhs.type					&&
		lhs.major_version == rhs.major_version	&&
		lhs.minor_version == rhs.minor_version;	
}

//-----------------------------------------------------------------------------

} // namespace Implementation
} // namespace KasperskyLicensing
