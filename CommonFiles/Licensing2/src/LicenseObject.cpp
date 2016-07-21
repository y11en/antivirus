#include "precompiled.h"
#include "LicenseKeyPid.h"
#include "EnumeratorImp.h"
#include "LicenseObject.h"

namespace KasperskyLicensing {
namespace Implementation {

//-----------------------------------------------------------------------------

LicenseObject::LicenseObject(const data_tree::node& data, const LicenseEnvironment& env)
{
	// fill license object identifier
	lic_object_id.id = GET_AVP_PID_ID(data.id());
	lic_object_id.type = static_cast<LicenseObjectId::Type>(GET_AVP_PID_APP(data.id()));

	// extract version
	unsigned version = data.get_child(AVP_PID_KEY_LO_VERSION).get_value().as_uint();
	lic_object_id.minor_version = version & 0xFFFF;
	lic_object_id.major_version = version >> 16 & 0xFFFF;

	// extract name
	data.get_child(AVP_PID_KEY_LO_NAME).get_value().as_string(lic_object_name);

	// fill platform list
	data_tree::node::child_enumerator platforms =
		data.get_child(AVP_PID_KEY_PRODUCT_PLATFORM_LIST).get_children();
	for (; !platforms.IsDone(); platforms.Next())
		platform_list.push_back(static_cast<PlatformType>(GET_AVP_PID_ID(platforms.Item().id())));

	// fill license restrictions
	data_tree::node::child_enumerator restrictions = 
		data.get_child(AVP_PID_KEY_RESTRICTION_LIST).get_children();
	for(; !restrictions.IsDone(); restrictions.Next())
	{
		AutoPointer<LicenseRestriction> restriction(new LicenseRestriction(restrictions.Item(), env));
		restr_list.push_back(restriction.GetPointer());
		restriction.Release();
	}

	// fill child license objects
	data_tree::node::child_enumerator children = 
		data.get_child(AVP_PID_KEY_LO_LIST).get_children();
	for(; !children.IsDone(); children.Next())
	{
		AutoPointer<LicenseObject> licobj(new LicenseObject(children.Item(), env));
		lic_object_list.push_back(licobj.GetPointer());
		licobj.Release();
	}
}

//-----------------------------------------------------------------------------

const ILicenseRestriction& LicenseObject::GetRestriction(unsigned id) const
{
	RestrictionList::const_iterator 
		it	= restr_list.begin(),
		end	= restr_list.end();

	// find license restriction by id
	for (; it != end && (*it)->GetId() != id; ++it);

	// if not found throw corresponding exception
	if (it == end)
		throw LicensingError(ILicensingError::LICERR_NOT_FOUND);

	return **it;
}

//-----------------------------------------------------------------------------

ILicenseObject::RestrictionEnumerator LicenseObject::GetRestrictions() const
{
	typedef pointerator<RestrictionList::const_iterator, ILicenseRestriction> iterator_t;
	// set range iterators
	iterator_t begin(restr_list.begin()), end(restr_list.end());
	// construct enumerator implementation
	return new EnumeratorImp<RestrictionEnumerator, iterator_t>(begin, end);
}

//-----------------------------------------------------------------------------

ILicenseObject::LicenseObjectEnumerator LicenseObject::GetLicenseObjects() const
{
	typedef pointerator<LicenseObjectList::const_iterator, ILicenseObject> iterator_t;
	// set range iterators	
	iterator_t it(lic_object_list.begin()), end(lic_object_list.end());
	// return enumerator implementation
	return new EnumeratorImp<LicenseObjectEnumerator, iterator_t>(it, end);
}

//-----------------------------------------------------------------------------

ILicenseObject::PlatformEnumerator LicenseObject::GetSupportedPlatforms() const
{
	typedef EnumeratorImp<PlatformEnumerator, platform_list_t::const_iterator> enumerator_t;
	// return enumerator implementation
	return new enumerator_t(platform_list.begin(), platform_list.end());
}

//-----------------------------------------------------------------------------

const LicenseObjectId& LicenseObject::GetObjectId() const
{
	return lic_object_id;
}

//-----------------------------------------------------------------------------

const char_t* LicenseObject::GetName() const
{
	return lic_object_name.c_str();
}

//-----------------------------------------------------------------------------

const LicenseObject::RestrictionList& LicenseObject::GetRestrictionList() const
{
	return restr_list;
}

//-----------------------------------------------------------------------------

const LicenseObject::LicenseObjectList& LicenseObject::GetLicenseObjectList() const
{
	return lic_object_list;
}

//-----------------------------------------------------------------------------

} // namespace Implementation
} // namespace KasperskyLicensing