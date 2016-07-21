#include "precompiled.h"
#include "EnumeratorImp.h"
#include "LicenseContext.h"
#include "LicenseKeyspace.h"
#include "LicenseObjectHelper.h"

namespace KasperskyLicensing {
namespace Implementation {


//-----------------------------------------------------------------------------

bool operator == (const ProductInfo& lhs, const ProductInfo& rhs)
{
	return 
		lhs.localization_id	== rhs.localization_id		&&
		lhs.market_sector_id == rhs.market_sector_id	&&
		lhs.package == rhs.package						&&
		lhs.product_id == rhs.product_id				&&
		lhs.sales_channel_id == rhs.sales_channel_id;
}

//-----------------------------------------------------------------------------

bool operator != (const ProductInfo& lhs, const ProductInfo& rhs)
{
	return !operator==(lhs, rhs);
}

//-----------------------------------------------------------------------------

class LicenseKeyPredicate
{
public:
	LicenseKeyPredicate(const ILicenseContext& c, PlatformType id)
		: context(c), platform_id(id) {}
	bool operator() (ILicenseUtility::KeyEnumerator::ValueType key) const
	{
		LicenseObjectHelper obj(key.GetLicenseObject());
		return obj.ContainsLicenseObject(context, platform_id);
	}
private:
	LicenseContext context;
	PlatformType platform_id;
};

//-----------------------------------------------------------------------------

LicenseKeyspace::LicenseKeyspace(const ProgramClientInfo clientInfo, const char_t* dskmPath)
	: client_info(clientInfo)
	, verifier(dskmPath)
	, control_db(dskmPath)
{
}

//-----------------------------------------------------------------------------

ILicenseUtility::KeyEnumerator LicenseKeyspace::GetLicenseKeys(const ILicenseContext& context) const
{
	typedef pointerator<key_list_t::const_iterator, ILicenseKey> iterator_t;
	// set range iterators
	iterator_t begin(key_list.begin()), end(key_list.end());
	// enumerator implementation
	typedef EnumeratorImp<ILicenseUtility::KeyEnumerator, iterator_t, LicenseKeyPredicate> enumerator_t;

	return new enumerator_t (begin, end, LicenseKeyPredicate(context, client_info.platform_id));
}

//-----------------------------------------------------------------------------

const ILicenseObject& LicenseKeyspace::GetLicenseObject(const ILicenseContext& context) const
{
	// license object does not exist
	if (!lic_object.GetPointer())
		throw LicensingError(ILicensingError::LICERR_NOT_FOUND);
	// root license context is specified
	if (context.IsEmpty())
		return *lic_object;
	// set helper
	LicenseObjectHelper obj(*lic_object);
	// try to find corresponding license object
	ILicenseObject::LicenseObjectEnumerator e = 
		obj.FindLicenseObject(context, client_info.platform_id);
	// object not found
	if (e.IsDone())
		throw LicensingError(ILicensingError::LICERR_NOT_FOUND);

	return e.Item();
}

//-----------------------------------------------------------------------------

const ILicenseKey& LicenseKeyspace::AddLicenseKey(
	const IObjectImage& image, 
	const ILicenseContext& context
)
{
	// parse license key
	AutoPointer<LicenseKey> key(ParseLicenseKey(image));
	// check license key status
	if (GetLicenseKeyStatus(*key, context) != ILicenseUtility::KEY_STATUS_OK)
		throw LicensingError(ILicensingError::LICERR_KEY_MISMATCH);
	// check for duplicates
	if (FindLicenseKey(key->GetKeyInfo().serial_number) != key_list.end())
		throw LicensingError(ILicensingError::LICERR_DUPLICATE_KEY);

	// license object does not exist yet
	if (!lic_object.GetPointer())
	{
		// create and set root license object
		lic_object = AutoPointer<LicenseObjectWrapper>
			(new LicenseObjectWrapper(key->GetLicenseRoot(), client_info.platform_id));
	}
	else
	{
		// otherwise merge license object
		lic_object->Merge(key->GetLicenseRoot());
	}
	// add license key
	key_list.push_back(key.GetPointer());
	// release object
	key.Release();
	// return last added element
	return *key_list.back();
}

//-----------------------------------------------------------------------------

void LicenseKeyspace::RemoveLicenseKey(const LicenseKeyInfo::SerialNumber& sn)
{
	key_list_t::const_iterator it(FindLicenseKey(sn)), begin(key_list.begin());
	// license key does not exist
	if (it == key_list.end())
		throw LicensingError(ILicensingError::LICERR_NOT_FOUND);
	// special case: the last key is being removed
	if (key_list.size() == 1)
	{
		// reset root license object
		lic_object.Reset();
		// clear key list
		key_list.clear();
	}
	// more keys are in the list
	else
	{
		// subtract corresponding license object
		lic_object->Subtract((**it).GetLicenseRoot());
		// set erase iterator
		key_list_t::iterator jt(key_list.begin());
		// position iterator to corresponding element
		std::advance(jt, std::distance(it, begin));
		// erase corresponding license key
		key_list.erase(jt);
	}
}

//-----------------------------------------------------------------------------

LicenseKeyspace::key_list_t::const_iterator 
	LicenseKeyspace::FindLicenseKey(const LicenseKeyInfo::SerialNumber& sn) const
{
	// set range iterators
	key_list_t::const_iterator it(key_list.begin()), end(key_list.end());
	// find license key with the same sequence number
	for (; it != end; ++it)
	{
		// required key is found
		if ((*it)->GetKeyInfo().serial_number.sequence_number == sn.sequence_number)
			return it;
	}
	return end;
}

//-----------------------------------------------------------------------------

const ILicenseKey& LicenseKeyspace::GetLicenseKey(
	const LicenseKeyInfo::SerialNumber& sn,
	const ILicenseContext& context
) const
{
	key_list_t::const_iterator it = FindLicenseKey(sn);
	// license key does not exist
	if (it == key_list.end())
		throw LicensingError(ILicensingError::LICERR_NOT_FOUND);

	LicenseObjectHelper obj((*it)->GetLicenseObject());
	// object not found
	if (!obj.ContainsLicenseObject(context, client_info.platform_id))
		throw LicensingError(ILicensingError::LICERR_NOT_FOUND);

	return **it;
}

//-----------------------------------------------------------------------------

AutoPointer<LicenseKey> 
	LicenseKeyspace::ParseLicenseKey(const const IObjectImage& image) const
{
	// at first check signature of license key image
	if (!verifier.VerifyKeySignature(image))
		throw LicensingError(ILicensingError::LICERR_INVALID_SIGNATURE);

	return new LicenseKey(image, control_db);
}

//-----------------------------------------------------------------------------

ILicenseUtility::LicenseKeyStatus LicenseKeyspace::GetLicenseKeyStatus(
	const ILicenseKey& key, 
	const ILicenseContext& context
) const
{
	const LicenseKeyInfo& key_info = key.GetKeyInfo();

	// check for proper product
	if (key_info.product_info != client_info.product_info)
		return ILicenseUtility::KEY_INCOMPATIBLE_PRODUCT;
	
	// beta key for non-beta product
	if (key_info.license_type == LicenseKeyInfo::LTYPE_BETA && 
			client_info.version_type != ProgramClientInfo::VER_TYPE_BETA)
		return ILicenseUtility::KEY_INCOMPATIBLE_VERSION_TYPE;

	// beta product for non-beta key
	if (client_info.version_type == ProgramClientInfo::VER_TYPE_BETA &&
			key_info.license_type != LicenseKeyInfo::LTYPE_BETA)
		return ILicenseUtility::KEY_INCOMPATIBLE_VERSION_TYPE;

	// check for license object presence
	LicenseObjectHelper obj(key.GetLicenseObject());
	// object not found
	if (!obj.ContainsLicenseObject(context, client_info.platform_id))
		return ILicenseUtility::KEY_LICENSE_OBJECT_MISSING;

	return ILicenseUtility::KEY_STATUS_OK;
}

//-----------------------------------------------------------------------------

ControlDatabase& LicenseKeyspace::GetControlDatabase()
{
	return control_db;
}

//-----------------------------------------------------------------------------


} // namespace Implementation 
} // namespace KasperskyLicensing
