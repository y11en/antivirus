#include "precompiled.h"
#include "ObjectImageHolder.h"
#include "LicenseContext.h"
#include "LicenseKeyspace.h"
#include "ControlDatabase.h"
#include "LicenseUtility.h"
#include "EnumeratorImp.h"
#include "Licensing.h"

namespace KasperskyLicensing{

//-----------------------------------------------------------------------------

AutoPointer<ILicensing> MakeLicensing(
	IStorageRW& keyStorage, 
	const ProgramClientInfo& clientInfo,
	const char_t* dskmPath
)
{
	return new Implementation::Licensing(keyStorage, clientInfo, dskmPath);
}

//-----------------------------------------------------------------------------
	
AutoPointer<ILicensing> MakeLicensingEx(
	IStorageRW& keyStorage, 
	IStorageR& baseStorage, 
	ControlMode ctrlMode, 
	const ProgramClientInfo& clientInfo,
	const char_t* dskmPath
)
{
	// create licensing object
	std::auto_ptr<Implementation::Licensing> 
		licensing(new Implementation::Licensing(keyStorage, clientInfo, dskmPath));
	// set required control mode
	licensing->SetControlMode(baseStorage, ctrlMode);

	return licensing.release();
}

//-----------------------------------------------------------------------------

namespace Implementation{

//-----------------------------------------------------------------------------

void Deallocate(void* ptr)
{
	delete ptr;
}
	
//-----------------------------------------------------------------------------
	
Licensing::Licensing(
	IStorageRW& keyStorage, 
	const ProgramClientInfo& clientInfo,
	const char_t* dskmPath
) 
	: key_storage(keyStorage)
	, key_space(clientInfo, dskmPath)
{
	bool bIncorrectKeysSkipped = false;
	// get stored object names
	IStorageRW::NameEnumerator names = key_storage.GetObjectNames();
	// load available license keys from storage
	for (; !names.IsDone(); names.Next())
	{
		AutoPointer<IObjectImage> aImage = key_storage.LoadObject(names.Item());
		// skip null objects
		if (!aImage.GetPointer())
			continue;
		try
		{
			// try to add license key
			key_space.AddLicenseKey(*aImage, LicenseContext());
		}
		// skip invalid keys
		catch(ILicensingError&)
		{
			bIncorrectKeysSkipped = true;
		}
	}
	if (bIncorrectKeysSkipped)
		warnings.push_back(WARN_INCORRECT_KEYS_SKIPPED);
}

//-----------------------------------------------------------------------------

AutoPointer<ILicenseKey> 
	Licensing::ParseLicenseKey(const void * image, size_t size) const
{
	ObjectImageHolder key(TXT(""), image, size);
	return key_space.ParseLicenseKey(key).Release();
}

//-----------------------------------------------------------------------------

AutoPointer<ILicenseUtility> Licensing::MakeLicenseUtility(const ILicenseContext& context)
{
	return new LicenseUtility(key_storage, key_space, context);
}

//-----------------------------------------------------------------------------

AutoPointer<ILicenseContext> Licensing::MakeLicenseContext() const
{
	return new LicenseContext();
}

//-----------------------------------------------------------------------------

ILicensing::WarningEnumerator Licensing::GetInitWarnings() const
{
	typedef EnumeratorImp<WarningEnumerator, std::vector<WarningCode>::const_iterator> enumerator_t;
	return new enumerator_t(warnings.begin(), warnings.end());
}

//-----------------------------------------------------------------------------

void Licensing::SetControlMode(IStorageR& baseStorage, ControlMode ctrlMode)
{
	ControlDatabase& cdb = key_space.GetControlDatabase();
	// load database according specified control mode
	cdb.LoadDatabase(baseStorage, ctrlMode);
	// store errors occured
	if (cdb.GetIndexStatus() != ControlDatabase::INDEX_OK)
		warnings.push_back(WARN_INCORRECT_DBINDEX);
	if (cdb.GetBlackListStatus() != ControlDatabase::BLST_OK)
		warnings.push_back(WARN_INCORRECT_BLACKLIST);
	if (cdb.GetAvdbStatus() != ControlDatabase::AVDB_OK)
		warnings.push_back(WARN_INCONSISTENT_AVDATABASE);
}

//-----------------------------------------------------------------------------

} // namespace Implementation
} // namespace KasperskyLicensing
