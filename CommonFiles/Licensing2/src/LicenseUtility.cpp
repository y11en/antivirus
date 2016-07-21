#include "precompiled.h"
#include "EnumeratorImp.h"
#include "ObjectImageHolder.h"
#include "LicenseObjectHelper.h"
#include "LicenseContext.h"
#include "LicenseKeyspace.h"
#include "LicenseUtility.h"


namespace KasperskyLicensing {
namespace Implementation {

//-----------------------------------------------------------------------------

LicenseUtility::LicenseUtility(
	IStorageRW& keyStorage, 
	LicenseKeyspace& keySpace, 
	const ILicenseContext& context
)
	: key_storage(keyStorage)
	, key_space(keySpace)
	, lic_context(context)
{
}
	
//-----------------------------------------------------------------------------

const ILicenseContext& LicenseUtility::GetLicenseContext() const
{
	return lic_context;
}

//-----------------------------------------------------------------------------

const ILicenseObject& LicenseUtility::GetLicenseObject() const
{
	return key_space.GetLicenseObject(lic_context);
}

//-----------------------------------------------------------------------------

const ILicenseKey& LicenseUtility::InstallLicenseKey(const char_t* name, const void* data, size_t size)
{
	ObjectImageHolder image(name, data, size);
	// add license key to keyspace
	const ILicenseKey& key = key_space.AddLicenseKey(image, lic_context);
	// save license key
	try
	{
		// try to save license key
		key_storage.StoreObject(image);
	}
	catch(ILicensingError&)
	{
		// restore previous state
		key_space.RemoveLicenseKey(key.GetKeyInfo().serial_number);
		// re-throw error
		throw;
	}
	return key;
}

//-----------------------------------------------------------------------------

void LicenseUtility::UninstallLicenseKey(const LicenseKeyInfo::SerialNumber& sn)
{
	// check for license key presence
	const ILicenseKey& key = key_space.GetLicenseKey(sn, lic_context);
	// remove key from storage
	key_storage.RemoveObject(key.GetName());
	// remove key from keyspace
	key_space.RemoveLicenseKey(sn);
}

//-----------------------------------------------------------------------------

ILicenseUtility::LicenseKeyStatus 
	LicenseUtility::GetLicenseKeyStatus(const ILicenseKey& key) const
{
	return key_space.GetLicenseKeyStatus(key, lic_context);
}

//-----------------------------------------------------------------------------

ILicenseUtility::KeyEnumerator LicenseUtility::GetInstalledKeys()
{
	return key_space.GetLicenseKeys(lic_context);
}

//-----------------------------------------------------------------------------

} // namespace Implementation
} // namespace KasperskyLicensing
