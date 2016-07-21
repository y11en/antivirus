#include "precompiled.h"
#include "EnumeratorImp.h"
#include "LicenseObjectHelper.h"
#include "LicenseObjectWrapper.h"

namespace KasperskyLicensing {
namespace Implementation {

//-----------------------------------------------------------------------------

LicenseObjectWrapper::LicenseObjectWrapper(const LicenseObject& obj, unsigned platform)
	: lic_object_id(obj.GetObjectId())
	, lic_object_name(obj.GetName())
	, target_platform_id(platform)
	, ref_count(0)
{
	// perform merge
	Merge(obj);

	PlatformEnumerator pe = obj.GetSupportedPlatforms();
	// copy supported platform list
	for (; !pe.IsDone(); pe.Next())
		platform_list.push_back(pe.Item());

}	

//-----------------------------------------------------------------------------

void LicenseObjectWrapper::Merge(const LicenseObject& obj)
{
	platform_list_t::iterator it(platform_list.begin()), end(platform_list.end());
	// perform supported platforms intersection
	for (; it != end; ++it)
	{
		PlatformEnumerator pe = obj.GetSupportedPlatforms();
		// find matching platform
		for (; !pe.IsDone(); pe.Next())
		{
			// platform is found
			if (pe.Item() == *it)
				// exit loop
				break;
		}
		// no matches found
		if (pe.IsDone())
			platform_list.erase(it++);
	}

	// merge license restrictions
	const LicenseObject::RestrictionList& rl = obj.GetRestrictionList();
	// set range iterators
	LicenseObject::RestrictionList::const_iterator rlb(rl.begin()), rle(rl.end());
	// for each restriction
	for(; rlb != rle; ++rlb)
		MergeRestriction(**rlb);

	// merge license objects
	const LicenseObject::LicenseObjectList& ol = obj.GetLicenseObjectList();
	// set range iterators
	LicenseObject::LicenseObjectList::const_iterator olb(ol.begin()), ole(ol.end());
	// for each license object
	for(; olb != ole; ++olb)
		MergeLicenseObject(**olb);

	// increment reference counter
	ref_count++;
}

//-----------------------------------------------------------------------------

void LicenseObjectWrapper::MergeRestriction(const LicenseRestriction& r)
{
	lic_restr_list_t::iterator it(restr_list.begin()), end(restr_list.end());
	// find corresponding restriction
	for(; it != end; ++it)
	{
		// restriction already exists
		if ((**it).GetId() == r.GetId())
			break;
	}
	// restriction does not exist
	if (it == end)
	{
		// make restriction wrapper
		AutoPointer<LicenseRestrictionWrapper> rw(new LicenseRestrictionWrapper(r));
		// append new restriction to list
		restr_list.push_back(rw.GetPointer());
		// release added restriction
		rw.Release();
	}
	// restriction already exists
	else
		// merge with the existing restriction
		(**it).Merge(r);
}

//-----------------------------------------------------------------------------

void LicenseObjectWrapper::MergeLicenseObject(const LicenseObject& o)
{
	LicenseObjectHelper hlp(o);
	// target platform must be supported
	if (!hlp.IsPlatformSupported(target_platform_id))
		return;

	lic_object_list_t::iterator it(lic_object_list.begin()), end(lic_object_list.end());
	// find corresponding license object
	for(; it != end; ++it)
	{
		// license object already exists
		if ((**it).GetObjectId() == o.GetObjectId())
			break;
	}
	// license object does not exist
	if (it == end)
	{
		// make license object wrapper
		AutoPointer<LicenseObjectWrapper> ow(new LicenseObjectWrapper(o, target_platform_id));
		// append new license object to list
		lic_object_list.push_back(ow.GetPointer());
		// release added license object
		ow.Release();
	}
	// license object already exists
	else
		// merge with the existing license object
		(**it).Merge(o);
}

//-----------------------------------------------------------------------------

void LicenseObjectWrapper::Subtract(const LicenseObject& obj)
{
	// remove license restrictions
	const LicenseObject::RestrictionList& rl = obj.GetRestrictionList();
	// set range iterators
	LicenseObject::RestrictionList::const_iterator rlb(rl.begin()), rle(rl.end());
	// for each restriction
	for(; rlb != rle; ++rlb)
		SubtractRestriction(**rlb);

	// remove license objects
	const LicenseObject::LicenseObjectList& ol = obj.GetLicenseObjectList();
	// set range iterators
	LicenseObject::LicenseObjectList::const_iterator olb(ol.begin()), ole(ol.end());
	// for each license object
	for(; olb != ole; ++olb)
		SubtractLicenseObject(**olb);

	// decrement reference counter
	ref_count--;
}

//-----------------------------------------------------------------------------

void LicenseObjectWrapper::SubtractRestriction(const LicenseRestriction& r)
{
	lic_restr_list_t::iterator it(restr_list.begin()), end(restr_list.end());
	// find corresponding restriction
	for(; it != end; ++it)
	{
		// restriction is found
		if ((**it).GetId() == r.GetId())
			break;
	}
	// restriction does not exist
	if (it == end)
		throw LicensingError(ILicensingError::LICERR_NOT_FOUND);

	// subtract license restriction
	(**it).Subtract(r);
	// if no restrictions remain
	if ((**it).IsEmpty())
		// remove restriction from list
		restr_list.erase(it);
}

//-----------------------------------------------------------------------------

void LicenseObjectWrapper::SubtractLicenseObject(const LicenseObject& o)
{
	lic_object_list_t::iterator it(lic_object_list.begin()), end(lic_object_list.end());
	// find corresponding license object
	for(; it != end; ++it)
	{
		// license object already exists
		if ((**it).GetObjectId() == o.GetObjectId())
			break;
	}
	// license object does not exist
	if (it == end)
		throw LicensingError(ILicensingError::LICERR_NOT_FOUND);

	// check for once referenced license object
	if ((**it).GetReferenceCount() > 1)
		// if object is referenced more than once
		(**it).Subtract(o);
	// remove once referenced object
	else
		lic_object_list.erase(it);
}

//-----------------------------------------------------------------------------

unsigned LicenseObjectWrapper::GetReferenceCount() const
{
	return ref_count;
}

//-----------------------------------------------------------------------------

const char_t* LicenseObjectWrapper::GetName() const
{
	return lic_object_name.c_str();
}

//-----------------------------------------------------------------------------

const LicenseObjectId& LicenseObjectWrapper::GetObjectId() const
{
	return lic_object_id;
}

//-----------------------------------------------------------------------------

ILicenseObject::PlatformEnumerator LicenseObjectWrapper::GetSupportedPlatforms() const
{
	typedef EnumeratorImp<PlatformEnumerator, platform_list_t::const_iterator> enumerator_t;
	// return enumerator implementation
	return new enumerator_t(platform_list.begin(), platform_list.end());
}

//-----------------------------------------------------------------------------

ILicenseObject::LicenseObjectEnumerator LicenseObjectWrapper::GetLicenseObjects() const
{
	typedef pointerator<lic_object_list_t::const_iterator, ILicenseObject> iterator_t;
	// set range iterators	
	iterator_t it(lic_object_list.begin()), end(lic_object_list.end());
	// return enumerator implementation
	return new EnumeratorImp<LicenseObjectEnumerator, iterator_t>(it, end);
}

//-----------------------------------------------------------------------------

ILicenseObject::RestrictionEnumerator LicenseObjectWrapper::GetRestrictions() const
{
	typedef pointerator<lic_restr_list_t::const_iterator, ILicenseRestriction> iterator_t;
	// set range iterators
	iterator_t begin(restr_list.begin()), end(restr_list.end());
	// construct enumerator implementation
	return new EnumeratorImp<RestrictionEnumerator, iterator_t>(begin, end);
}

//-----------------------------------------------------------------------------

const ILicenseRestriction& LicenseObjectWrapper::GetRestriction(unsigned id) const
{
	lic_restr_list_t::const_iterator it(restr_list.begin()), end(restr_list.end());
	
	// todo: perform search of parent license object also
	for(; it != end; ++it)
	{
		if((**it).GetId() == id)
			return **it;
	}

	throw LicensingError(ILicensingError::LICERR_NOT_FOUND);
}

//-----------------------------------------------------------------------------

} // namespace Implementation
} // namespace KasperskyLicensing
